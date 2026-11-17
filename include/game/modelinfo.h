#ifndef GAME_MODELINFO_H
#define GAME_MODELINFO_H
// A loaded "MODL" model resource (model.cpp: fn_802C6B44 loads one by name, fn_802C6C34 by index): its draw
// elements, surface lists, extras, bounding sphere and box. One declaration for every unit (port ODR work,
// 2026-11-16; carmodel/waterpool radius+center and chunks count/texs were views). Names unproven.
#include "game/common.h"
#include "game/valvec3.h"

struct MElem;
struct MSurfList;
struct MExtra;

struct ModelInfo {
    s32 nElems;           // offset 0x0
    MElem* elems;         // offset 0x4
    s32 nLists;           // offset 0x8
    MSurfList** lists;    // offset 0xC
    s32 nExtras;          // offset 0x10
    MExtra** extras;      // offset 0x14
    s32 maxInst;          // offset 0x18
    f32 radius;           // offset 0x1C
    ValVec3 center;         // offset 0x20
    f32 box[6];           // offset 0x2C
    u32 pad44;
};

#endif
