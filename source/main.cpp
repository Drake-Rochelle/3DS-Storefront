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
    PrintConsole topScreen, bottomScreen;
    consoleInit(GFX_TOP, &topScreen);
    consoleInit(GFX_BOTTOM, &bottomScreen);
    consoleSelect(&topScreen);
    fsInit();
    httpcInit(0);
    printf("Fetching storefront\...\n");
    std::string store_url = readFile("/3ds/3DS-Storefront/store.url");
    if (!store_url.empty())
    {
        bool downloadSuccess = downloadURL(store_url, "/3ds/3DS-Storefront", "storefront.json");
        if (!downloadSuccess)
        {
            consoleClear();
            printf("Failed to fetch storefront from %s\nUsing previous storefront.\n", store_url.substr(20,store_url.size()).c_str());
            while (aptMainLoop())
            {
                hidScanInput();
                if (hidKeysDown()) break;
                gspWaitForVBlank();
            }
            
        }
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
    std::string printString = "";
    for (size_t i = 0; i < keys.size(); i++)
    {
        if (i == selected)
        {
            printString += ">" + keys[i] + "\n";
        }
        else
        {
            printString += " " + keys[i] + "\n";
        }
    }
    consoleClear();
    printf("%s", printString.c_str());
    consoleSelect(&bottomScreen);
    printf("Use UP/DOWN to navigate.\nPress A to download.\nPress X to change storefront.\nPress START to exit.\n");
    consoleSelect(&topScreen);
    while (aptMainLoop())
    {
        hidScanInput();
        if (hidKeysDown() & KEY_START) break;
        if (hidKeysDown() & KEY_DOWN){
            selected = (selected + 1) % keys.size();
            std::string printString = "";
            for (size_t i = 0; i < keys.size(); i++)
            {
                if (i == selected)
                {
                    printString += ">" + keys[i] + "\n";
                }
                else
                {
                    printString += " " + keys[i] + "\n";
                }
            }
            consoleClear();
            printf("%s", printString.c_str());
        }
        if (hidKeysDown() & KEY_UP){
            selected = (selected - 1) % keys.size();
            std::string printString = "";
            for (size_t i = 0; i < keys.size(); i++)
            {
                if (i == selected)
                {
                    printString += ">" + keys[i] + "\n";
                }
                else
                {
                    printString += " " + keys[i] + "\n";
                }
            }
            consoleClear();
            printf("%s", printString.c_str());
        }
        if (hidKeysDown() & KEY_X)
        {
            std::string newUrl = getTextInput("Enter new store name:", 256);
            if (!newUrl.empty())
            {
                std::string new_Url = "https://tinyurl.com/" + newUrl;
                writeFile("/3ds/3DS-Storefront/store.url", new_Url);
                consoleClear();
                httpcExit();
                gfxExit();
                main(0, nullptr);
                return 0;
            }
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