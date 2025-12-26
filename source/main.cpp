#include <3ds.h>
#include <stdio.h>
#include <math.h>
#include <array>
#include "DownloadFromURL.hpp"
#include "input.hpp"
#include "rw.hpp"
#include "json.hpp"
#include "string.hpp"
#include "gui.hpp"
using json = nlohmann::ordered_json;
using namespace std;
const string domain = "https://drive.google.com/uc?export=download&id=";
const string defaultStore = "1m99FhKG-zpNd7VoAOjFV11dyJsDbnUv9";

// const vector defaultBGC = {100,150,200};
// const vector defaultTC = {255,255,255};
// const vector defaultSTC = {180,200,255};
// const vector defaultSTHC = {80,130,180};

const array<int, 3> defaultBGC   = {215, 215, 215};
const array<int, 3> defaultTC    = {000, 000, 000};
const array<int, 3> defaultSTC   = {255, 255, 255};
const array<int, 3> defaultSTHC  = {167, 167, 167};


int main_count;
json settings_json;

int main(int argc, char** argv);

void GoBack(){
    string prev = readFile("/3ds/3DS-Storefront/default_store.3sf");
    if (!prev.empty()) {
        consoleClear();
        printf("Loading...\n");
        DrawBG();
        DrawFG();
        Buffer(0);
        writeFile("/3ds/3DS-Storefront/store.3sf", prev);
        consoleClear();
        httpcExit();
        gfxExit();
        main(1, nullptr);
    }
}

void LoadBookmark()
{
    string saved_stores = readFile("/3ds/3DS-Storefront/saves.3sf");
    consoleClear();
    vector<string> lines = split(saved_stores, '\n');
    unordered_map<string, string> save_map;
    vector<string> names;
    for (int i = 0; i < (int)lines.size()-1; i++)
    {
        size_t colon_pos = lines[i].find(':');
        if (colon_pos != string::npos)
        {
            string name = lines[i].substr(0, colon_pos);
            names.push_back(name);
            string url = lines[i].substr(colon_pos + 1);
            save_map[name] = url;
        }
    }
    int selected_save = 0;
    DrawMenu(names, selected_save);
    Buffer(0);
    while (aptMainLoop())
    {
        hidScanInput();
        if (hidKeysDown() & KEY_START)
        {
            aptExit();
            svcExitProcess();
            return;
        }
        if (hidKeysDown() & KEY_DOWN){
            selected_save++;
            if (selected_save >= (int)names.size()){
                selected_save = 0;
            }
            DrawMenu(names, selected_save);
            Buffer(0);
        }
        if (hidKeysDown() & KEY_UP){
            selected_save--;
            if (selected_save < 0){
                selected_save = names.size() - 1;
            }
            DrawMenu(names, selected_save);
            Buffer(0);
        }
        if (hidKeysDown() & KEY_A){
            string selected_name = names[selected_save];
            string selected_url = save_map[selected_name];
            consoleClear();
            printf("Loading...\n");
            DrawBG();
            DrawFG();
            Buffer(0);
            writeFile("/3ds/3DS-Storefront/store.3sf", selected_url);
            consoleClear();
            httpcExit();
            gfxExit();
            main(1, nullptr);
            return;
        }
        gspWaitForVBlank();
    }
}

void SaveBookmark(string store_url)
{
    string save_name = getTextInput("Enter store save name:", 256);
    if (!save_name.empty())
    {
        string curr_save = readFile("/3ds/3DS-Storefront/saves.3sf");
        writeFile("/3ds/3DS-Storefront/saves.3sf", curr_save + save_name + ":" + store_url + "\n");
    }
}

void LoadNewStorefront(string store_url){
    string newUrl = getTextInput("Enter new storefront ID:", 256);
    if (!newUrl.empty())
    {
        DrawBG();
        DrawFG();
        Buffer(0);
        writeFile("/3ds/3DS-Storefront/prev_store.3sf", store_url);
        writeFile("/3ds/3DS-Storefront/store.3sf", newUrl);
        consoleClear();
        httpcExit();
        gfxExit();
        main(1, nullptr);
    }
}

bool is_integer(const string& s) {
    if (s.empty()) return false;

    size_t i = 0;
    if (s[0] == '-' || s[0] == '+')
        i = 1;

    for (; i < s.size(); i++) {
        if (!isdigit(s[i]))
            return false;
    }

    return true;
}

void ChangeColors(){
    vector<string> options = {
        "Background Color",
        "Text Color",
        "Selected Text Color",
        "Selected Text Highlight Color",
    };
    int selected_option = 0;
    while (aptMainLoop())
    {
        hidScanInput();
        DrawMenu(options, selected_option);
        Buffer(0);
        if (hidKeysDown() & KEY_A){
            string input = getTextInput("Example: (255, 255, 255) or \"default\"",256);
            if (input == "default"){
                if (selected_option == 0)
                {
                    SetBGColor(100,150,200);
                    settings_json["Background Color"] = C2D_Color32(defaultBGC[0],defaultBGC[1],defaultBGC[2],255);
                }
                else if (selected_option == 1)
                {
                    SetTextColor(255,255,255);
                    settings_json["Text Color"] = C2D_Color32(defaultTC[0],defaultTC[1],defaultTC[2],255);
                }
                else if (selected_option == 2)
                {
                    SetSelectedTextColor(180,200,255);
                    settings_json["Selected Text Color"] = C2D_Color32(defaultSTC[0],defaultSTC[1],defaultSTC[2],255);
                }
                else if (selected_option == 3)
                {
                    SetSelectedTextHighlightColor(80,130,180);
                    settings_json["Selected Text Highlight Color"] = C2D_Color32(defaultSTHC[0],defaultSTHC[1],defaultSTHC[2],255);
                }
                writeFile("/3ds/3DS-Storefront/settings.json",settings_json.dump());
                break;
            }
            replace(input.begin(), input.end(), '(', ' ');
            replace(input.begin(), input.end(), ')', ' ');
            vector<string> color = split(input, ',');
            vector<int> rgb = {0,0,0};
            bool brk = false;
            if (color.size() == 3)
            {
                for (int i = 0; i < 3; i++)
                {
                    if (is_integer(color[i]))
                    {
                        rgb[i] = stoi(color[i]);
                    }
                    else
                    {
                        brk = true;
                    }
                }
            }
            else
            {
                break;
            }
            if (brk)
            {
                break;
            }
            u32 color_int = C2D_Color32(rgb[0], rgb[1], rgb[2], 255);
            if (selected_option == 0)
            {
                SetBGColor(color_int);
                settings_json["Background Color"] = color_int;
            }
            else if (selected_option == 1)
            {
                SetTextColor(color_int);
                settings_json["Text Color"] = color_int;
            }
            else if (selected_option == 2)
            {
                SetSelectedTextColor(color_int);
                settings_json["Selected Text Color"] = color_int;
            }
            else if (selected_option == 3)
            {
                SetSelectedTextHighlightColor(color_int);
                settings_json["Selected Text Highlight Color"] = color_int;
            }
            writeFile("/3ds/3DS-Storefront/settings.json",settings_json.dump());
            return;
        }
        if (hidKeysDown() & KEY_DOWN)
        {
            selected_option++;
            if (selected_option >= (int)options.size()){
                selected_option = 0;
            }
        }
        if (hidKeysDown() & KEY_UP)
        {
            selected_option--;
            if (selected_option < 0){
                selected_option = options.size() - 1;
            }
        }
        if (hidKeysDown() & KEY_START)
        {
            aptExit();
            svcExitProcess();
        }
    }
}

void ChangeTextSize(){
    string input = getTextInput("Default is 14",256);
    int value = stoi(input);
    SetFontSize(((float)value)/20.0f);
    settings_json["Text Size"] = ((float)value)/20.0f;
    writeFile("/3ds/3DS-Storefront/settings.json",settings_json.dump());
}

int main(int argc, char** argv)
{
    main_count++;
    if (main_count == 1)
    {
        GUI_Init(0);
        string json_string = readFile("/3ds/3DS-Storefront/settings.json");
        if (!json_string.empty())
        {
            settings_json = json::parse(json_string);
        }
        else
        {
            settings_json = json();
            settings_json["Text Size"] = 0.7f;
            settings_json["Background Color"] = C2D_Color32(defaultBGC[0],defaultBGC[1],defaultBGC[2],255);
            settings_json["Text Color"] = C2D_Color32(defaultTC[0],defaultTC[1],defaultTC[2],255);
            settings_json["Selected Text Color"] = C2D_Color32(defaultSTC[0],defaultSTC[1],defaultSTC[2],255);
            settings_json["Selected Text Highlight Color"] = C2D_Color32(defaultSTHC[0],defaultSTHC[1],defaultSTHC[2],255);
            writeFile("/3ds/3DS-Storefront/settings.json", settings_json.dump());
        }
        SetFontSize(settings_json["Text Size"]);
        SetBGColor(settings_json["Background Color"]);
        SetTextColor(settings_json["Text Color"]);
        SetSelectedTextColor(settings_json["Selected Text Color"]);
        SetSelectedTextHighlightColor(settings_json["Selected Text Highlight Color"]);
    }
    if (argc == 1){
        writeFile("/3ds/3DS-Storefront/cursor.int","0");
    }
    gfxInitDefault();
    PrintConsole bottomScreen;
    consoleInit(GFX_BOTTOM, &bottomScreen);
    consoleSelect(&bottomScreen);
    fsInit();
    httpcInit(0);
    string store_url = readFile("/3ds/3DS-Storefront/store.3sf");
    string default_store_url = readFile("/3ds/3DS-Storefront/default_store.3sf");
    string prev_store_url = readFile("/3ds/3DS-Storefront/prev_store.3sf");
    if (default_store_url.empty())
    {
        writeFile("/3ds/3DS-Storefront/default_store.3sf", defaultStore);
    }
    if (prev_store_url.empty())
    {
        writeFile("/3ds/3DS-Storefront/prev_store.3sf", defaultStore);
    }
    if (store_url.empty())
    {
        writeFile("/3ds/3DS-Storefront/store.3sf", defaultStore);
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
    string jsonContent = readFile("/3ds/3DS-Storefront/storefront.json");
    json storeJson = json::parse(jsonContent, nullptr, false);
    vector<string> keys;
    for (auto& el : storeJson.items()) {
        keys.push_back(el.key());
    }
    vector<string> stores;
    for (int i = 0; i < (int)keys.size(); i++)
    {
        if ((string)(storeJson[keys[i]][1]) == "storefront")
        {
            stores.push_back(keys[i]);
        }
    }
    string cursor = readFile("/3ds/3DS-Storefront/cursor.int");
    int selected = 0;
    if (!cursor.empty())
    {
        selected = stoi(cursor);
    }
    DrawMenu(keys, selected);
    Buffer(0);
    consoleClear();
    printf("Use UP/DOWN to navigate.\nUse LEFT/RIGHT for fast navigation.\nPress A to download.\nPress Y to return to the hub.\nPress SELECT to open settings.\nPress START to exit.");
    while (aptMainLoop())
    {
        hidScanInput();
        if (hidKeysDown() & KEY_START)
        {
            aptExit();
            svcExitProcess();
        }
        if (hidKeysDown() & KEY_L)
        {
            SaveBookmark(store_url);
        }
        if (hidKeysDown() & KEY_R)
        {
            LoadBookmark();
            return 0;
        }
        if (hidKeysDown() & KEY_DOWN){
            selected++;
            if (selected >= (int)keys.size()){
                selected = 0;
            }
            writeFile("/3ds/3DS-Storefront/cursor.int", to_string(selected));
            DrawMenu(keys, selected);
            Buffer(0);
        }
        if (hidKeysDown() & KEY_UP){
            selected--;
            if (selected < 0){
                selected = keys.size() - 1;
            }
            writeFile("/3ds/3DS-Storefront/cursor.int", to_string(selected));
            DrawMenu(keys, selected);
            Buffer(0);
        }
        if (hidKeysDown() & KEY_LEFT){
            selected-=5;
            if (selected < 0){
                selected = keys.size() - 1;
            }
            writeFile("/3ds/3DS-Storefront/cursor.int", to_string(selected));
            DrawMenu(keys, selected);
            Buffer(0);
        }
        if (hidKeysDown() & KEY_RIGHT){
            selected+=5;
            if (selected >= (int)keys.size()){
                selected = 0;
            }
            writeFile("/3ds/3DS-Storefront/cursor.int", to_string(selected));
            DrawMenu(keys, selected);   
            Buffer(0);
        }
        if (hidKeysDown() & KEY_X)
        {
            LoadNewStorefront(store_url);
        }
        if (hidKeysDown() & KEY_A){
            if (storeJson[keys[selected]].size() < 3)
            {
                consoleClear();
                printf("Loading...\n");
                DrawBG();
                DrawFG();
                Buffer(0);
                writeFile("/3ds/3DS-Storefront/prev_store.3sf", store_url);
                writeFile("/3ds/3DS-Storefront/store.3sf", (string)(storeJson[keys[selected]][0]));
                consoleClear();
                httpcExit();
                gfxExit();
                main(1, nullptr);
                return 0;
            }
            else
            {
                string url = storeJson[keys[selected]][0];
                string fileName = storeJson[keys[selected]][1];
                consoleClear();
                printf("Downloading %s...\n", fileName.c_str());
                string dir = storeJson[keys[selected]][2];
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
                    DrawMenu(keys, selected);
                    Buffer(0);
                    consoleClear();
                    printf("Use UP/DOWN to navigate.\nUse LEFT/RIGHT for fast navigation.\nPress A to download.\nPress Y to return to the hub.\nPress SELECT to open settings.\nPress START to exit.");
                }
            }
        }
        if (hidKeysDown() & KEY_SELECT) {
            vector<string> settings = {
                "Save Bookmark",
                "Load Bookmark",
                "Change Colors",
                "Change Text Size",
                "Go to Storefront ID"
            };
            int selected_setting = 0;
            consoleClear();
            while (aptMainLoop())
            {
                hidScanInput();
                DrawMenu(settings, selected_setting);
                Buffer(0);
                if (hidKeysDown() & KEY_A){
                    if (selected_setting == 0)
                    {
                        SaveBookmark(store_url);
                    }
                    else if (selected_setting == 1)
                    {
                        LoadBookmark();
                    }
                    else if (selected_setting == 2)
                    {
                        ChangeColors();
                    }
                    else if (selected_setting == 3)
                    {
                        ChangeTextSize();
                    }
                    else if (selected_setting == 4)
                    {
                        LoadNewStorefront(store_url);
                    }
                }
                if (hidKeysDown() & KEY_DOWN)
                {
                    selected_setting++;
                    if (selected_setting >= (int)settings.size()){
                        selected_setting = 0;
                    }
                }
                if (hidKeysDown() & KEY_UP)
                {
                    selected_setting--;
                    if (selected_setting < 0){
                        selected_setting = settings.size() - 1;
                    }
                }
                if (hidKeysDown() & KEY_B)
                {
                    break;
                }
                if (hidKeysDown() & KEY_START)
                {
                    aptExit();
                    svcExitProcess();
                }
            }
            DrawMenu(keys, selected);   
            Buffer(0);
        }
        if (hidKeysDown() & KEY_Y) {
            GoBack();
            return 0;
        }
        gspWaitForVBlank();
    }
    httpcExit();
    gfxExit();
    return 0;
}
