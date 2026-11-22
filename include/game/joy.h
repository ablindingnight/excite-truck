#ifndef GAME_JOY_H
#define GAME_JOY_H

#include "game/common.h"

// joy.cpp's pad state (ReadJoyStatus fills one; inputbind.cpp keeps arrays of them, 0x64 apart). Names are guesses.

struct JoyInfo {
    u32 hold;  // offset 0x0
    u32 bits;  // offset 0x4
    s32 a;     // offset 0x8
    s32 b;     // offset 0xC
};

struct JoyStatus {
    // separate members: struct copies use lfs/stfs (an array copies as words)
    f32 x;    // offset 0x0
    f32 y;    // offset 0x4
    f32 ax2, ax3, ax4, ax5, ax6, ax7;  // offset 0x8
    u8 buttons[32];  // offset 0x20
    u32 dir;         // offset 0x40
    JoyInfo info;    // offset 0x44
    s32 unk54[2];    // offset 0x54
    u32 f5C;         // offset 0x5C
    u8 connected;    // offset 0x60
};

#endif
