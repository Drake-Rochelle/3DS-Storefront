#include "gui.hpp"
#include <3ds.h>
#include <citro2d.h>
#include <tex3ds.h>
#include <string>
#include <array>
#include <math.h>
#include <time.h>
#include "earcut.hpp"
#include <tex3ds.h>
// #include "vignette.h"
using namespace std;
C3D_RenderTarget* top;
C3D_RenderTarget* bottom;
C2D_TextBuf g_textBuf;
u32 color;
float font_size;
int draw_buffer;
int LINE_HEIGHT;
u32 BG_COLOR;
u32 TEXT_COLOR;
u32 SELECTED_TEXT_COLOR;
u32 SELECTED_TEXT_HIGHLIGHT_COLOR;
const int LINE_DENSITY = 23;
// C3D_Tex vignette;
// C2D_Image vignette_2d;
// Tex3DS_SubTexture vignette_subtex;

void GUI_Init(int screen_index)
{
    // // Upload raw RGBA8 data
    // C3D_TexInit(&vignette, vignette_width, vignette_height, GPU_RGBA8);
    // C3D_TexUpload(&vignette, vignette_data);

    // // Describe the subtexture region (whole image)
    // vignette_subtex.width  = vignette_width;
    // vignette_subtex.height = vignette_height;
    // vignette_subtex.left   = 0.0f;
    // vignette_subtex.top    = 1.0f;
    // vignette_subtex.right  = (float)vignette_width  / (float)vignette.width;
    // vignette_subtex.bottom = 1.0f - ((float)vignette_height / (float)vignette.height);

    // // Hook up the C2D_Image
    // vignette_2d.tex    = &vignette;
    // vignette_2d.subtex = &vignette_subtex;

    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    color = C2D_Color32(0,0,0,255);
    font_size = 1.0f;
    LINE_HEIGHT = (float)LINE_DENSITY;
    if (screen_index == 0 || screen_index > 1){
        top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    }
    if (screen_index > 0){
        bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
    }
    g_textBuf = C2D_TextBufNew(4096);

    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    if (screen_index == 0 || screen_index == 2){
        C2D_TargetClear(top, C2D_Color32(0,0,0,255));
        C2D_SceneBegin(top);
        draw_buffer = 0;
    }
    else{
        C2D_TargetClear(bottom, C2D_Color32(0,0,0,255));
        C2D_SceneBegin(bottom);
        draw_buffer = 1;
    }
}

void SetBGColor(u32 c){
    BG_COLOR = c;
}
void SetTextColor(u32 c){
    TEXT_COLOR = c;
}
void SetSelectedTextColor(u32 c){
    SELECTED_TEXT_COLOR = c;
}
void SetSelectedTextHighlightColor(u32 c){
    SELECTED_TEXT_HIGHLIGHT_COLOR = c;
}
void SetBGColor(int r,int g, int b, int a){
    BG_COLOR = C2D_Color32(r,g,b,a);
}
void SetTextColor(int r,int g, int b, int a){
    TEXT_COLOR = C2D_Color32(r,g,b,a);
}
void SetSelectedTextColor(int r,int g, int b, int a){
    SELECTED_TEXT_COLOR = C2D_Color32(r,g,b,a);
}
void SetSelectedTextHighlightColor(int r,int g, int b, int a){
    SELECTED_TEXT_HIGHLIGHT_COLOR = C2D_Color32(r,g,b,a);
}

void GUI_Exit()
{
    C3D_FrameEnd(0);
    C2D_TextBufClear(g_textBuf);
    draw_buffer = 0;
}

void DrawText(float x, float y, const string& text, HAlign halign, VAlign valign)
{
    if (draw_buffer == 1) {
        x -= 40;
    }

    C2D_Text textObj;
    C2D_TextParse(&textObj, g_textBuf, text.c_str());
    C2D_TextOptimize(&textObj);

    // Measure text
    float width, height;
    C2D_TextGetDimensions(&textObj, font_size, font_size, &width, &height);

    // Alignment offsets
    float dx = 0.0f;
    float dy = 0.0f;

    switch (halign) {
        case HAlign::Center: dx = -width / 2.0f; break;
        case HAlign::Right:  dx = -width;        break;
        default: break;
    }

    switch (valign) {
        case VAlign::Middle: dy = -height / 2.0f; break;
        case VAlign::Bottom: dy = -height;        break;
        default: break;
    }

    // Draw
    C2D_DrawText(&textObj,
                 C2D_WithColor,
                 x + dx, y + dy,
                 0.0f,
                 font_size, font_size,
                 color);
}
void Buffer(int screen_index)
{
    draw_buffer = screen_index;
    C3D_FrameEnd(0);
    C2D_TextBufClear(g_textBuf);
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    if (screen_index == 0)
    {
        C2D_TargetClear(top, C2D_Color32(255,255,255,255));
        C2D_SceneBegin(top);
    }
    else if (screen_index == 1)
    {
        C2D_TargetClear(bottom, C2D_Color32(255,255,255,255));
        C2D_SceneBegin(bottom);
    }
}

void SetColor(int r, int g, int b, int a)
{
    color = C2D_Color32(r,g,b,a);
}
void SetColor(u32 c)
{
    color = c;
}

void SetFontSize(float s)
{
    font_size = s;
    LINE_HEIGHT = (int)(((float)LINE_DENSITY)*s);
}

void FillRect(float x, float y, float w, float h)
{
    if (draw_buffer == 1){
        x-=40;
    }
    C2D_DrawRectSolid(x, y, 0.0f, w, h, color);
}

void FillCircle(float x, float y, float r)
{
    if (draw_buffer == 1){
        x-=40;
    }
    C2D_DrawCircleSolid(x, y, 0.0f, r, color);
}

void DrawLine(float x1, float y1, float x2, float y2)
{
    if (draw_buffer == 1){
        x1-=40;
        x2-=40;
    }
    C2D_DrawLine(x1, y1, color, x2, y2, color, 2.0f, 0.0f);
}
void FillTrig(float x1, float y1, float x2, float y2, float x3, float y3)
{
    if (draw_buffer == 1){
        x1-=40;
        x2-=40;
        x3-=40;
    }
    C2D_DrawTriangle(x1, y1, color, x2, y2, color, x3, y3, color, 0.0f);
}

void FillPoly(const vector<array<float, 2>>& pts)
{
    // Wrap in a vector-of-rings
    vector<vector<array<float, 2>>> polygon;
    polygon.push_back(pts);

    auto indices = mapbox::earcut<uint16_t>(polygon);

    for (size_t i = 0; i < indices.size(); i += 3) {
        int a = indices[i];
        int b = indices[i + 1];
        int c = indices[i + 2];
        if (draw_buffer == 0){
            C2D_DrawTriangle(
                pts[a][0], pts[a][1], color,
                pts[b][0], pts[b][1], color,
                pts[c][0], pts[c][1], color,
                0.0f
            );
        }
        else
        {
            C2D_DrawTriangle(
                pts[a][0]-40, pts[a][1], color,
                pts[b][0]-40, pts[b][1], color,
                pts[c][0]-40, pts[c][1], color,
                0.0f
            );
        }
    }
}
int get_time_ms()
{
    return svcGetSystemTick()/CPU_TICKS_PER_MSEC;
}

#include <string>

std::string Ellipsize(const std::string& text) {
    // Find the last dot in the string
    size_t dotPos = text.rfind('.');
    std::string stem, ext;

    if (dotPos != std::string::npos) {
        stem = text.substr(0, dotPos);   // everything before the last '.'
        ext  = text.substr(dotPos);      // including the dot
    } else {
        stem = text;                     // no extension
        ext  = "";
    }

    // Measure full text first
    C2D_Text textObj;
    C2D_TextParse(&textObj, g_textBuf, text.c_str());
    C2D_TextOptimize(&textObj);
    float w, h;
    C2D_TextGetDimensions(&textObj, font_size, font_size, &w, &h);

    if (w < 400 - LINE_HEIGHT) {
        return text; // fits already
    }

    std::string out = stem;
    const std::string ell = "...";

    while (true) {
        std::string candidate = out + ell + ext;

        C2D_TextParse(&textObj, g_textBuf, candidate.c_str());
        C2D_TextOptimize(&textObj);
        C2D_TextGetDimensions(&textObj, font_size, font_size, &w, &h);

        if (w <= 400 - LINE_HEIGHT || out.empty()) {
            return candidate;
        }

        out.pop_back(); // trim stem one char at a time
    }
}

void DrawBG(){
    SetColor(BG_COLOR);
    FillRect(0,0,400,240);
    int s = 80;
    SetColor(SELECTED_TEXT_HIGHLIGHT_COLOR);
    for (int x = 0; x < 400+1; x+=s){
        DrawLine(x,0,x,240);
    }
    for (int y = 0; y < 240+1; y+=s){
        DrawLine(0,y,400,y);
    }
}

void DrawFG(){
    u32 c1 = C2D_Color32(0,0,0,50);
    u32 c2 = C2D_Color32(0,0,0,0);
    float i = 3.0f;
    C2D_DrawRectangle(0,0,0,400,120/i,c1,c1,c2,c2);
    C2D_DrawRectangle(0,0,0,200/i,240,c1,c2,c1,c2);
    C2D_DrawRectangle(0,240-(120/i),0,400,120/i,c2,c2,c1,c1);
    C2D_DrawRectangle(400-(200/i),0,0,200/i,240,c2,c1,c2,c1);
}

void DrawMenu(const vector<string>& list, int selected) {
    int total = list.size();
    int VISIBLE_COUNT = (int)((float)240/(float)LINE_HEIGHT);
    // Compute window start
    int half = VISIBLE_COUNT / 2;
    int start = selected - half;
    if (start < 0) start = 0;
    if (start + VISIBLE_COUNT > total)
        start = max(0, total - VISIBLE_COUNT);

    DrawBG();
    // Print only the visible slice
    for (int i = start; i < start + VISIBLE_COUNT && i < total; i++) {
        if (i == selected)
        {
            SetColor(SELECTED_TEXT_HIGHLIGHT_COLOR);
            FillRect(0,(i-start)*LINE_HEIGHT, 400, LINE_HEIGHT);
            SetColor(SELECTED_TEXT_COLOR);
        }
        else
        {
            SetColor(TEXT_COLOR);
        }
        DrawText(LINE_HEIGHT,(i-start)*LINE_HEIGHT,Ellipsize(list[i]));
    }
    DrawFG();
}


void GUI()
{
    int start = get_time_ms();
    int curr_screen = 0;
    GUI_Init(curr_screen+2);
    while (aptMainLoop()) {
        SetColor(255,0,0);
        FillRect(0,0,200,120);
        SetColor(0,0,255);
        FillCircle(200, 120, 30);
        SetColor(0,0,0);
        DrawLine(10.0f, 10.0f, 300.0f, 200.0f);
        SetColor(0,255,0);
        vector<array<float,2>> polygon = {
            {00.0f,00.0f},
            {20.0f,00.0f},
            {40.0f,40.0f},
            {00.0f,20.0f},
            {00.0f,00.0f},
        };
        FillPoly(polygon);
        SetColor(0,0,0);
        SetFontSize(1.5f);

        float xp = sin((get_time_ms()-start)/2000.0)*50.0+200.0;

        DrawText(xp,140,"Hello World!", HAlign::Center, VAlign::Middle);

        curr_screen++;
        curr_screen %= 2;

        Buffer(curr_screen);

        hidScanInput();
        if (hidKeysDown() & KEY_SELECT) {
            GUI_Exit();
            break;
        }
    }
}


