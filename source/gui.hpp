#pragma once
#include <3ds.h>
#include <citro2d.h>
#include <tex3ds.h>
#include <string>
#include <array>
#include <math.h>
#include <time.h>
#include "earcut.hpp"
using namespace std;
enum class HAlign { Left, Center, Right };
enum class VAlign { Top, Middle, Bottom };
extern C3D_RenderTarget* top;
extern C3D_RenderTarget* bottom;
extern C2D_TextBuf g_textBuf;
extern u32 color;
extern float font_size;
extern int draw_buffer;
extern int LINE_HEIGHT;
extern u32 BG_COLOR;
extern u32 TEXT_COLOR;
extern u32 SELECTED_TEXT_COLOR;
extern u32 SELECTED_TEXT_HIGHLIGHT_COLOR;

void GUI_Init(int screen_index);
void GUI_Exit();
void DrawText(float x, float y, const string& text, HAlign halign = HAlign::Left, VAlign valign = VAlign::Top);
void Buffer(int screen_index);
void SetColor(int r, int g, int b, int a = 255);
void SetColor(u32 c);
void SetFontSize(float s);
void FillRect(float x, float y, float w, float h);
void FillCircle(float x, float y, float r);
void DrawLine(float x1, float y1, float x2, float y2);
void FillTrig(float x1, float y1, float x2, float y2, float x3, float y3);
void FillPoly(const vector<array<float, 2>>& pts);
int get_time_ms();
void DrawBG();
void DrawFG();
void DrawMenu(const vector<string>& list, int selected);
void SetBGColor(u32 c);
void SetTextColor(u32 c);
void SetTextColor(u32 c);
void SetSelectedTextColor(u32 c);
void SetSelectedTextHighlightColor(u32 c);
void SetBGColor(int r,int g, int b, int a = 255);
void SetTextColor(int r,int g, int b, int a = 255);
void SetSelectedTextColor(int r,int g, int b, int a = 255);
void SetSelectedTextHighlightColor(int r,int g, int b, int a = 255);


