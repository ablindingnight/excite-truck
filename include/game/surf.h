#ifndef GAME_SURF_H
#define GAME_SURF_H

#include "game/common.h"

// A drawable surface resource (drsurf.cpp: magic-checked, byte-swapped and drawn from its display list; gndtex
// batches them as ground models through fn_802D6B54/fn_802D6B78). Names are guesses.
struct Surf {
    u32 magic;     // offset 0x0
    u8 pad4[4];
    u8 fmt;        // offset 0x8
    u8 b9;         // offset 0x9
    u8 flags;      // offset 0xA
    u8 dlBlocks;   // offset 0xB
    u32 vfmt;      // offset 0xC
    void* fmtDL;   // offset 0x10
    u32 tex;       // offset 0x14
    u8 pad18[4];
    s32 nverts;    // offset 0x1C
    void* verts;   // offset 0x20
    void* attr;    // offset 0x24
    void* uv;      // offset 0x28
    u32 dlSize;    // offset 0x2C
    void* dl;      // offset 0x30
    s32 nfacing;   // offset 0x34: facing entries (4 bytes each from 0x38; fn_802D6594 swaps them)
    f32 off0;      // offset 0x38
    f32 off1;      // offset 0x3C
};

#endif
