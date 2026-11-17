#ifndef GAME_SURFTYPE_H
#define GAME_SURFTYPE_H
// A ground surface type (surftype.cpp's table, fn_801D04A8): grip, slip and drag, its sound and effect. carbody
// reads the first four fields. Names unproven.
#include "game/common.h"

struct SurfType {
    f32 grip;       // offset 0x0
    f32 slip;       // offset 0x4
    f32 drag;       // offset 0x8
    s8 sound;       // offset 0xC
    u8 effect;      // offset 0xD
    s32 soundType;  // offset 0x10
};

#endif
