#ifndef GAME_DASHHELPER_H
#define GAME_DASHHELPER_H

#include "game/gart.h"
#include "types.h"

extern "C" {
void fn_80010E94(gArtSet* art, int part, f32 x, f32 y, f32 w, f32 h);
// The art set comes last: loading's two callers load the width constant before the art pointer (FACTS 2026-11-17 14:00).
void fn_80011280(u32 color, f32 x, f32 y, f32 w, f32 h, gArtSet* art);
}

#endif
