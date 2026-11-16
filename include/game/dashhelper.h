#ifndef GAME_DASHHELPER_H
#define GAME_DASHHELPER_H

#include "game/gart.h"
#include "types.h"

extern "C" {
void fn_80010E94(gArtSet* art, int part, f32 x, f32 y, f32 w, f32 h);
void fn_80011280(u32 color, gArtSet* art, f32 x, f32 y, f32 w, f32 h);
}

#endif
