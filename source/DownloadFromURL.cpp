#include <3ds.h>
#include <stdio.h>
#include <regex>
bool downloadToBuffer(const std::string& url, std::vector<u8>& out, int redirectLimit = 5)
{
    if (redirectLimit <= 0)
    {
        return false;
    }
    Result res;
    httpcContext ctx;
    res = httpcOpenContext(&ctx, HTTPC_METHOD_GET, url.c_str(), 1);
    if (R_FAILED(res)) return false;
    res = httpcSetSSLOpt(&ctx, SSLCOPT_DisableVerify);
    if (R_FAILED(res)) return false;
    res = httpcAddRequestHeaderField(&ctx, "User-Agent", "3ds-downloader");
    if (R_FAILED(res)) return false;
    res = httpcBeginRequest(&ctx);
    if (R_FAILED(res)) return false;
    u32 status = 0;
    res = httpcGetResponseStatusCode(&ctx, &status);
    if (R_FAILED(res)) return false;
    if (status >= 300 && status < 400)
    {
        char newUrl[1024] = {0};
        res = httpcGetResponseHeader(&ctx, "Location", newUrl, sizeof(newUrl));
        httpcCloseContext(&ctx);

        if (R_FAILED(res))
        {
            return false;
        }
        return downloadToBuffer(std::string(newUrl), out, redirectLimit - 1);
    }
    if (status != 200)
    {
        httpcCloseContext(&ctx);
        return false;
    }
    u32 size = 0;
    res = httpcGetDownloadSizeState(&ctx, NULL, &size);
    if (R_FAILED(res)) return false;
    out.resize(size);
    u32 pos = 0;
    while (pos < size)
    {
        u32 read = 0;
        res = httpcDownloadData(&ctx, &out[pos], size - pos, &read);
        if (R_FAILED(res)) break;
        pos += read;
    }
    httpcCloseContext(&ctx);
    return pos == size;
}
void ensureDir(FS_Archive archive, const std::string& fullPath)
{
    size_t start = 1; // skip leading '/'
    while (true)
    {
        size_t slash = fullPath.find('/', start);
        if (slash == std::string::npos)
            break;
        std::string sub = fullPath.substr(0, slash + 1); // include trailing slash
        FS_Path p = fsMakePath(PATH_ASCII, sub.c_str());
        FSUSER_CreateDirectory(archive, p, 0);
        start = slash + 1;
    }
    FS_Path p = fsMakePath(PATH_ASCII, fullPath.c_str());
    FSUSER_CreateDirectory(archive, p, 0);
}
bool downloadToSD(const std::string& url, const std::string& outPath)
{
    std::vector<u8> buffer;
    if (!downloadToBuffer(url, buffer))
    {
        return false;
    }
    Handle fileHandle;
    FS_Path filePath  = fsMakePath(PATH_ASCII, outPath.c_str());
    FS_Path archPath  = fsMakePath(PATH_EMPTY, ""); // SDMC root
    Result res = FSUSER_OpenFileDirectly(
        &fileHandle,
        ARCHIVE_SDMC,
        archPath,
        filePath,
        FS_OPEN_WRITE | FS_OPEN_CREATE,
        0
    );
    if (R_FAILED(res))
    {
        return false;
    }
    u32 bytesWritten = 0;
    res = FSFILE_Write(fileHandle, &bytesWritten, 0,buffer.data(), buffer.size(),FS_WRITE_FLUSH);
    FSFILE_Close(fileHandle);
    return R_SUCCEEDED(res) && bytesWritten == buffer.size();
}
bool downloadURL(const std::string& url, const std::string& dir, const std::string& fileName)
{
    std::string outFile = dir+"/"+fileName;
    FS_Archive sdmcArchive;
    FS_Path sdmcPath = fsMakePath(PATH_EMPTY, "");
    FSUSER_OpenArchive(&sdmcArchive, ARCHIVE_SDMC, sdmcPath);
    ensureDir(sdmcArchive, dir);
    if (downloadToSD(url, outFile))
        return true;
    else
        return false;
}
