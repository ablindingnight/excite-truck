#ifndef GAME_UIVERTEX_H
#define GAME_UIVERTEX_H
#include "game/math_types.h"
struct UIVertex {Vec3 position; unsigned long color; float u,v;};
typedef char UIVertexSize24[sizeof(UIVertex)==24?1:-1];
extern "C" {UIVertex fn_80230A68(unsigned long,float,float,float,float,float);void fn_80231380(UIVertex*,unsigned long,float,float,float,float,float); }
#endif
