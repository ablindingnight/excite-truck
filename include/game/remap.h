#ifndef GAME_REMAP_H
#define GAME_REMAP_H

#include "game/common.h"

// A texture/material remap entry: model.cpp keeps a pool of them (fn_802C7888 copies 'n' entries in); wheelmod fills
// one on the stack. Field names are guesses.
struct Remap {
    char from[16];   // offset 0x0
    s32 inst;        // offset 0x10
    char to[16];     // offset 0x14
    s32 unk24;       // offset 0x24
    s32* out;        // offset 0x28: if set, receives -1 when copied
};

#endif
