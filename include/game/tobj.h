#ifndef GAME_TOBJ_H
#define GAME_TOBJ_H
// A placed track object (trackmod.cpp's list; TrackMod handlers get one per Swap/Init/Add). Names unproven
// (no retail symbols). The handler's own data starts at 0x2C.
#include "game/common.h"
#include "game/math_types.h"

struct TObj {
    void* Data(void) { return &d2C; }  // the handler's data (0x2C on)
    const void* Data(void) const { return &d2C; }

    u8 pad0[4];
    void* model;    // offset 0x4
    void** models;  // offset 0x8 (fabric: its model table)
    TObj* next;     // offset 0xC
    u8 pad10[4];
    s32 slot;       // offset 0x14
    u8 pad18[8];
    f32 pos[3];     // offset 0x20
    s32 d2C;        // offset 0x2C: first word of the handler's data
    f32 d30;        // offset 0x30
    Mat3 rot;       // offset 0x34
};

#endif
