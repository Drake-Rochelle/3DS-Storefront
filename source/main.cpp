#include <3ds.h>
#include <stdio.h>
#include "DownloadFromURL.hpp"
#include "input.hpp"
#include "rw.hpp"
#include "json.hpp"
using json = nlohmann::json;
int main(int argc, char** argv)
{
    std::string domain = "https://drive.google.com/uc?export=download&id=";
    std::string defaultStore = "1m99FhKG-zpNd7VoAOjFV11dyJsDbnUv9";
    gfxInitDefault();
    PrintConsole topScreen, bottomScreen;
    consoleInit(GFX_TOP, &topScreen);
    consoleInit(GFX_BOTTOM, &bottomScreen);
    consoleSelect(&topScreen);
    fsInit();
    httpcInit(0);
    std::string store_url = readFile("/3ds/3DS-Storefront/store.url");
    if (store_url.empty())
    {
        writeFile("/3ds/3DS-Storefront/store.url", defaultStore);
        store_url = defaultStore;
    }
    printf("Loading...\n");
    bool downloadSuccess = downloadURL(domain+store_url, "/3ds/3DS-Storefront", "storefront.json");
    if (!downloadSuccess)
    {
        consoleClear();
        printf("Failed to load storefront %s\nWill use previous storefront.\n\nPress any button to continue.", store_url.c_str());
        while (aptMainLoop())
        {
            hidScanInput();
            if (hidKeysDown()) break;
            gspWaitForVBlank();
        }
    }
    std::string jsonContent = readFile("/3ds/3DS-Storefront/storefront.json");
    json storeJson = json::parse(jsonContent, nullptr, false);
    std::vector<std::string> keys;
    for (auto& el : storeJson.items()) {
        keys.push_back(el.key());
    }
    std::vector<std::string> stores;
    for (int i = 0; i < (int)keys.size(); i++)
    {
        if ((std::string)(storeJson[keys[i]][1]) == "storefront")
        {
            stores.push_back(keys[i]);
        }
    }
    if (stores.size() > 0)
    {
        consoleSelect(&bottomScreen);
        printf("Use UP/DOWN to navigate.\nPress A to select storefront.\nPress START to exit.\n");
        consoleSelect(&topScreen);
        int cursor = 0;
        std::string printString;
        printString += "Select a storefront:\n\n";
        if (cursor == 0)
        {
            printString += ">Current Storefront\n";
        }
        else
        {
            printString += " Current Storefront\n";
        }
        for (int i = 0; i < (int)stores.size(); i++)
        {
            if (i == cursor-1)
            {
                printString += ">" + stores[i] + "\n";
            }
            else
            {
                printString += " " + stores[i] + "\n";
            }
        }
        consoleClear();
        printf("%s", printString.c_str());
        while (aptMainLoop())
        {
            hidScanInput();
            if (hidKeysDown() & KEY_START)
            {
                aptExit();
                svcExitProcess();
            }
            if (hidKeysDown() & KEY_DOWN){
                cursor++;
                if (cursor >= (int)stores.size()+1){
                    cursor = 0;
                }
                std::string printString = "";
                printString += "Select a storefront:\n\n";
                if (cursor == 0)
                {
                    printString += ">Current Storefront\n";
                }
                else
                {
                    printString += " Current Storefront\n";
                }
                for (int i = 0; i < (int)stores.size(); i++)
                {
                    if (i == cursor-1)
                    {
                        printString += ">" + stores[i] + "\n";
                    }
                    else
                    {
                        printString += " " + stores[i] + "\n";
                    }
                }
                consoleClear();
                printf("%s", printString.c_str());
            }
            if (hidKeysDown() & KEY_UP){
                cursor--;
                if (cursor < 0){
                    cursor = stores.size();
                }
                std::string printString = "";
                printString += "Select a storefront:\n\n";
                if (cursor == 0)
                {
                    printString += ">Current Storefront\n";
                }
                else
                {
                    printString += " Current Storefront\n";
                }
                for (int i = 0; i < (int)stores.size(); i++)
                {
                    if (i == cursor-1)
                    {
                        printString += ">" + stores[i] + "\n";
                    }
                    else
                    {
                        printString += " " + stores[i] + "\n";
                    }
                }
                consoleClear();
                printf("%s", printString.c_str());
            }
            if (hidKeysDown() & KEY_A) break;
            gspWaitForVBlank();
        }
        if (cursor != 0)
        {
            consoleClear();
            consoleSelect(&bottomScreen);
            consoleClear();
            consoleSelect(&topScreen);
            printf("Loading...\n");
            std::string newStoreUrl = storeJson[stores[cursor-1]][0];
            writeFile("/3ds/3DS-Storefront/prev_store.url", store_url);
            writeFile("/3ds/3DS-Storefront/store.url", newStoreUrl);
            consoleClear();
            httpcExit();
            gfxExit();
            main(0, nullptr);
            return 0;
        }
    }

    int selected = 0;
    std::string printString = "";
    for (int i = 0; i < (int)keys.size(); i++)
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
    printf("Use UP/DOWN to navigate.\nPress A to download.\nPress X to change storefront.\nPress START to exit.\nPress SELECT to return to main storefront.\n");
    consoleSelect(&topScreen);
    while (aptMainLoop())
    {
        hidScanInput();
        if (hidKeysDown() & KEY_START)
        {
            aptExit();
            svcExitProcess();
        }
        if (hidKeysDown() & KEY_DOWN){
            selected++;
            if (selected >= (int)keys.size()){
                selected = 0;
            }
            std::string printString = "";
            for (int i = 0; i < (int)keys.size(); i++)
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
            selected--;
            if (selected < 0){
                selected = keys.size() - 1;
            }
            std::string printString = "";
            for (int i = 0; i < (int)keys.size(); i++)
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
                writeFile("/3ds/3DS-Storefront/prev_store.url", store_url);
                writeFile("/3ds/3DS-Storefront/store.url", newUrl);
                consoleClear();
                httpcExit();
                gfxExit();
                main(0, nullptr);
                return 0;
            }
        }
        if (hidKeysDown() & KEY_A){
            if (storeJson[keys[selected]].size() < 3)
            {
                consoleClear();
                printf("Storefronts are not downloadable.\nWould you like to enter storefront?\n\nPress A to continue.\nPress B to exit.");
                while (aptMainLoop())
                {
                    hidScanInput();
                    if (hidKeysDown() & KEY_START) svcExitProcess();
                    if (hidKeysDown() & KEY_B){
                        std::string printString = "";
                        for (int i = 0; i < (int)keys.size(); i++)
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
                        break;
                    }
                    if (hidKeysDown() & KEY_A)
                    {
                        writeFile("/3ds/3DS-Storefront/prev_store.url", store_url);
                        writeFile("/3ds/3DS-Storefront/store.url", (std::string)(storeJson[keys[selected]][0]));
                        consoleClear();
                        httpcExit();
                        gfxExit();
                        main(0, nullptr);
                        return 0;
                    }
                    gspWaitForVBlank();
                }
            }
            else
            {
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
                        if (hidKeysDown() & KEY_START)
                        {
                            aptExit();
                            svcExitProcess();
                        }
                        if (hidKeysDown()) break;
                        gspWaitForVBlank();
                    }
                }
            }
        }
        if (hidKeysDown() & KEY_SELECT) {
            std::string prev = readFile("/3ds/3DS-Storefront/prev_store.url");
            if (!prev.empty()) {
                consoleClear();
                printf("Loading...\n");
                consoleSelect(&bottomScreen);
                consoleClear();
                consoleSelect(&topScreen);
                writeFile("/3ds/3DS-Storefront/store.url", prev);
                consoleClear();
                httpcExit();
                gfxExit();
                main(0, nullptr);
                return 0;
            }
        }
        gspWaitForVBlank();
    }
    httpcExit();
    gfxExit();
    return 0;
}