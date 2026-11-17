#ifndef GAME_TEXSTATE_H
#define GAME_TEXSTATE_H

#include "game/common.h"

// The cached texture/TEV state (lbl_804C5700, defined in gx.cpp; drsurf also sets it). Field names are guesses.
struct TexState {
    u8 dirty;   // offset 0x0
    s32 mode;   // offset 0x4
    u8 base2;   // offset 0x8
    u8 env;     // offset 0x9
    u8 envA;    // offset 0xA
    u8 bB;      // offset 0xB
    u8 lmap;    // offset 0xC
    u8 ind;     // offset 0xD
};

#endif
