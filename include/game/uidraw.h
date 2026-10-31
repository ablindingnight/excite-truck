#ifndef GAME_UIDRAW_H
#define GAME_UIDRAW_H
struct GTextFont;
// Base text/font/placement/scale/color/alignment, followed by scroll speed/time/margin.
extern "C" {
void fn_80230924(unsigned long,float,float,float,float,float);
void fn_80230C30(float*,float*,int,int);
void fn_80231220(float,float,float,float,long,unsigned long,float,float,float,float);

void fn_80230150(float,unsigned long,int,float,float,float,float,float,float,float);
void fn_802304EC(const char*,GTextFont*,float,float,float,float,unsigned long,int,float,float,float);
void fn_80230650(const char*,GTextFont*,float,float,float,float,unsigned long,int,float,float,float);
void fn_802307AC(const char*,GTextFont*,float,float,float,float,unsigned long,int,float,float,float);
}
#endif
