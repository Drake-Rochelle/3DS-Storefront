#include <3ds.h>
#include <stdio.h>
#include <string>
#include "DownloadFromURL.hpp"
std::string readFile(const std::string& path)
{
    Handle file;
    FS_Path filePath = fsMakePath(PATH_ASCII, path.c_str());
    FS_Path archPath = fsMakePath(PATH_EMPTY, "");

    Result res = FSUSER_OpenFileDirectly(
        &file,
        ARCHIVE_SDMC,
        archPath,
        filePath,
        FS_OPEN_READ,
        0
    );

    if (R_FAILED(res))
        return "";

    u64 size = 0;
    FSFILE_GetSize(file, &size);

    std::string out;
    out.resize(size);

    u32 bytesRead = 0;
    FSFILE_Read(file, &bytesRead, 0, &out[0], size);

    FSFILE_Close(file);
    return out;
}
bool writeFile(const std::string& path, const std::string& data)
{
    FS_Archive sdmcArchive;
    FS_Path sdmcPath = fsMakePath(PATH_EMPTY, "");
    FSUSER_OpenArchive(&sdmcArchive, ARCHIVE_SDMC, sdmcPath);
    // Extract directory portion
    size_t lastSlash = path.find_last_of('/');
    if (lastSlash != std::string::npos)
    {
        std::string dir = path.substr(0, lastSlash);
        if (!dir.empty())
        {
            FS_Archive dirArchive;
            FS_Path sdmcPath = fsMakePath(PATH_EMPTY, "");
            FSUSER_OpenArchive(&dirArchive, ARCHIVE_SDMC, sdmcPath);
            ensureDir(dirArchive, dir);
            FSUSER_CloseArchive(dirArchive);
        }
    }

    Handle file;
    FS_Path filePath = fsMakePath(PATH_ASCII, path.c_str());
    FS_Path archPath = fsMakePath(PATH_EMPTY, "");

    Result res = FSUSER_OpenFileDirectly(
        &file,
        ARCHIVE_SDMC,
        archPath,
        filePath,
        FS_OPEN_WRITE | FS_OPEN_CREATE,
        0
    );

    if (R_FAILED(res))
        return false;

    FSFILE_SetSize(file, 0);

    u32 bytesWritten = 0;
    res = FSFILE_Write(file, &bytesWritten, 0,
                       data.data(), data.size(),
                       FS_WRITE_FLUSH);
    
    FSFILE_Close(file);
    FSUSER_CloseArchive(sdmcArchive);
    return R_SUCCEEDED(res) && bytesWritten == data.size();
}
