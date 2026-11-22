#ifndef GAME_SPOT_H
#define GAME_SPOT_H

#include "types.h"

class Vec2;

// A ground position set from an x/z point (y = 0). One declaration for aiobst, ball and boostman (port ODR work,
// 2026-11-16). Retail never inlines From: all three units call the one copy at 0x80160230 (in aiobst's range), so it
// is declared here and defined once in aiobst.cpp. Plain x/y/z, not derived from ValVec3: a ValVec3 base changes
// boostman's BoostArea dtor and LoadBoosterAreas. Name unproven.
struct Spot {
    void From(const Vec2& v);
    f32 x;
    f32 y;
    f32 z;
};

#endif
