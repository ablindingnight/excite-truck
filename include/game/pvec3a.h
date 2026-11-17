#ifndef GAME_PVEC3A_H
#define GAME_PVEC3A_H
// PVec3A: one declaration for the units that each carried an identical copy (camfov, sphcoll). Port ODR work,
// 2026-11-16 (samesize.py). Names unproven.
#include "game/common.h"

// A vector copied as words (the original's struct copies of these move lwz/stw pairs).
struct PVec3A {
    f32 v[3];
};

#endif
