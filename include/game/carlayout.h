#ifndef GAME_CARLAYOUT_H
#define GAME_CARLAYOUT_H
// A car damage layout (carlayout.cpp, shared and reference-counted per car name): its deformable bones with
// travel limits, the links between them, the wheel bones and the per-bone bounding boxes (cardmg walks them; the
// car body keeps a pointer, CarBody::layout). One declaration for every unit (port ODR work, 2026-11-16).
#include "game/common.h"

struct LayoutBone {
    int id;           // offset 0x0
    ValVec3 min;      // offset 0x4: travel limits, in metres
    ValVec3 max;      // offset 0x10
    ValVec3 center;   // offset 0x1C
    f32 dmgDist;      // offset 0x28
    f32 weakness;     // offset 0x2C
    u8 welded;        // offset 0x30
};

struct LayoutBounds {
    ValVec3 min;  // offset 0x0
    ValVec3 max;  // offset 0xC
};

struct LayoutLink {
    int i;       // offset 0x0
    int j;       // offset 0x4
    f32 linear;  // offset 0x8
    f32 planar;  // offset 0xC
    f32 pop;     // offset 0x10
};

struct LayoutWheel {
    int boneId;  // offset 0x0
    f32 xScale;  // offset 0x4
    f32 yScale;  // offset 0x8
    f32 zScale;  // offset 0xC
};

struct CarLayout {
    int numBones;               // offset 0x0
    LayoutBone bones[162];      // offset 0x4, indexed from 1
    LayoutLink* links;          // offset 0x20EC
    int numLinks;               // offset 0x20F0
    LayoutWheel wheels[4];      // offset 0x20F4
    ValueTable* physics;        // offset 0x2134
    LayoutBounds bounds[32];    // offset 0x2138
    ValueTable* physicsTable;   // offset 0x2438
    char name[0x20];            // offset 0x243C
    int refs;                   // offset 0x245C
    CarLayout* next;            // offset 0x2460
};

#endif
