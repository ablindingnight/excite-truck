#ifndef GAME_SCOLOR_H
#define GAME_SCOLOR_H
// SColor: one declaration for the units that each carried an identical copy (smoke, stuntpart). Port ODR work,
// 2026-11-16 (samesize.py). Names unproven.
#include "game/common.h"

struct SColor {
    SColor(void) {}
    SColor(f32 a, f32 b, f32 c) {
        r = a;
        g = b;
        b_ = c;
    }
    f32 r;
    f32 g;
    f32 b_;
};

#endif
