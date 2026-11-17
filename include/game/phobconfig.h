#ifndef GAME_PHOBCONFIG_H
#define GAME_PHOBCONFIG_H

#include "game/common.h"

// A physics-object (phob) spawn configuration: mutphob's full layout; soevent's spawn events embed one. Field names
// are guesses.
struct PhobConfig {
    f32 maxLife;    // offset 0x0
    f32 mass;       // offset 0x4
    f32 mx;         // offset 0x8: inertia
    f32 my;         // offset 0xC
    f32 mz;         // offset 0x10
    f32 spring;     // offset 0x14
    f32 friction;   // offset 0x18
    s32 surf;       // offset 0x1C
    u8 groundTest;  // offset 0x20
    s32 effect;     // offset 0x24: hit effect, <= 0 = none
    s32 kind;       // offset 0x28
    f32 a;          // offset 0x2C: rad (sphere, tube, hinged) or len (cube)
    f32 b;          // offset 0x30: len (tube) or hit (hinged)
};

#endif
