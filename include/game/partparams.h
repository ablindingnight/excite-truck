#ifndef GAME_PARTPARAMS_H
#define GAME_PARTPARAMS_H
// A particle system's parameters (partman.cpp: PartParams() zeroes them, LoadParticleParams reads them from a .val node,
// whose keys give the field names: "Max_particles", "Spew_rate", "Upwards_kick", "Size_ramp", ...). The effect units
// (fire, smoke, sparks, precip, stuntpart, trail, waterpool) fill one in code. One declaration for every unit (port
// ODR work, 2026-11-16).
#include "game/common.h"

struct PartParams {
    PartParams(void);
    s32 type;          // offset 0x0
    s32 maxParticles;  // offset 0x4
    f32 maxLife;       // offset 0x8
    f32 spewRate;      // offset 0xC
    ValVec3 spread;         // offset 0x10
    ValVec3 velScale;       // offset 0x1C
    f32 upKick;        // offset 0x28
    f32 angSpread;     // offset 0x2C
    f32 gravity;       // offset 0x30
    f32 drag;          // offset 0x34
    u8 randomRot;      // offset 0x38
    f32 farDist;       // offset 0x3C
    f32 nearAlpha;     // offset 0x40
    s32 priority;      // offset 0x44
    f32 shading;       // offset 0x48
    f32 sizeAccel;     // offset 0x4C
    f32 sizeRamp;      // offset 0x50
    f32 sizeConst;     // offset 0x54
    f32 alphaRamp;     // offset 0x58
    f32 alphaConst;    // offset 0x5C
    f32 redRamp;       // offset 0x60
    f32 redConst;      // offset 0x64
    f32 greenRamp;     // offset 0x68
    f32 greenConst;    // offset 0x6C
    f32 blueRamp;      // offset 0x70
    f32 blueConst;     // offset 0x74
    f32 angRamp;       // offset 0x78
    f32 angConst;      // offset 0x7C
    f32 animRamp;      // offset 0x80
    s32 frames;        // offset 0x84
    u8 randomFrame;    // offset 0x88
    u8 unk89;          // offset 0x89
};

#endif
