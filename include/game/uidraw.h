#ifndef GAME_UIDRAW_H
#define GAME_UIDRAW_H
struct GTextFont;
// Base text/font/placement/scale/color/alignment, followed by scroll speed/time/margin.
extern "C" {
void DrawColoredQuad(unsigned long,float,float,float,float,float);
void CalculateScreenPosition(float*,float*,int,int);
// PROVISIONAL: relative source order of color/UV inputs; each PPC register-class argument sequence is unchanged.
void DrawTexturedQuad(float, float, float, float, long, float, float, float, float, unsigned long);

void DrawAnimatedWidget(float,unsigned long,int,float,float,float,float,float,float,float);
void fn_802304EC(const char*,GTextFont*,float,float,float,float,unsigned long,int,float,float,float);
void fn_80230650(const char*,GTextFont*,float,float,float,float,unsigned long,int,float,float,float);
void fn_802307AC(const char*,GTextFont*,float,float,float,float,unsigned long,int,float,float,float);
}
#endif
