#ifndef GAME_FONTTEXT_H
#define GAME_FONTTEXT_H
// The existing bitmap producer owns the private global Font record and its C++ destructor.
// Keep that nominal type; GTextFont is the different glyph/outline record. No layout is exposed here.
struct Font;
typedef Font BitmapFont;
extern "C" {
void DeleteFont(BitmapFont*);
int fn_801E4A6C(BitmapFont*,const char*);
int CalculateFontStringWidth(BitmapFont*,const char*,int);
int fn_801E4B00(BitmapFont*);
int fn_801E4B48(BitmapFont*);
int fn_801E4BA8(BitmapFont*);
void DrawTextWithFormatting(BitmapFont*,unsigned long,unsigned long,int,int,const char*,...);
void FormatAndDrawText(BitmapFont*,unsigned long,unsigned long,int,int,const char*,int);
}
#endif
