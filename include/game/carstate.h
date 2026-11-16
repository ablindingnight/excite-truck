#ifndef GAME_CARSTATE_H
#define GAME_CARSTATE_H

#include "game/common.h"

// A car's damage snapshot (carstate keeps a pool of them; CarBody holds the live one and a saved copy).
// Field names unproven.
struct CarColors {
    u8 c[164][4];  // packed dents: one byte per axis (fn_8018F538)
};

struct CarFlags {
    u8 f[18];  // part states (joints[0..16].state)
};

struct CarState {
    CarColors col;   // offset 0x0
    f32 time;        // offset 0x290
    f32 duration;    // offset 0x294
    s32 x298;        // offset 0x298
    s32 x29C;        // offset 0x29C
    CarFlags flags;  // offset 0x2A0
};

typedef char CarStateSize[sizeof(CarState) == 0x2B4 ? 1 : -1];

#endif
