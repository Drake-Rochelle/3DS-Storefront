#include <3ds.h>
#include <stdio.h>
#include "DownloadFromURL.hpp"
#include "input.hpp"
#include "rw.hpp"
#include "json.hpp"
#include "print.hpp"
using json = nlohmann::json;
int main(int argc, char** argv)
{
    std::string defaultUrl = "https://drive.google.com/uc?export=download&id=1T5wBtKLsx759Vl1M2a_wrZg3xJbVqoGV";
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
    fsInit();
    httpcInit(0);
    printf("Fetching storefront\...\n");
    std::string store_url = readFile("/3ds/3DS-Storefront/store.url");
    if (!store_url.empty())
    {
        downloadURL(store_url, "/3ds/3DS-Storefront", "storefront.json");
    }
    else
    {
        writeFile("/3ds/3DS-Storefront/store.url", defaultUrl);
        store_url = defaultUrl;
        downloadURL(store_url, "/3ds/3DS-Storefront", "storefront.json");
    }
    std::string jsonContent = readFile("/3ds/3DS-Storefront/storefront.json");
    json storeJson = json::parse(jsonContent, nullptr, false);
    std::vector<std::string> keys;

    for (auto& el : storeJson.items()) {
        keys.push_back(el.key());
    }
    int selected = 0;
    while (aptMainLoop())
    {
        consoleClear();
        for (size_t i = 0; i < keys.size(); i++)
        {
            if (i == selected)
            {
                printf(">%s\n", keys[i].c_str());
            }
            else
            {
                printf(" %s\n", keys[i].c_str());
            }
        }
        hidScanInput();
        if (hidKeysDown() & KEY_START) break;
        if (hidKeysDown() & KEY_DOWN){
            selected = (selected + 1) % keys.size();
        }
        if (hidKeysDown() & KEY_UP){
            selected = (selected - 1) % keys.size();
        }
        if (hidKeysDown() & KEY_A){
            std::string url = storeJson[keys[selected]][0];
            std::string fileName = storeJson[keys[selected]][1];
            consoleClear();
            printf("Downloading %s...\n", fileName.c_str());
            std::string dir = storeJson[keys[selected]][2];
            if (!fileName.empty()) {
                if (downloadURL(url, dir, fileName))
                {
                    consoleClear();
                    printf("Downloaded %s to %s.\n\nPress any button to continue.", fileName.c_str(), dir.c_str());
                }
                else
                {
                    consoleClear();
                    printf("Failed to download %s.\n", fileName.c_str());
                }
                while (aptMainLoop())
                {
                    hidScanInput();
                    if (hidKeysDown()) break;
                    gspWaitForVBlank();
                }
                
            }
        }
        gspWaitForVBlank();
    }
    httpcExit();
    gfxExit();
    return 0;
}