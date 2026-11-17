#ifndef GAME_OFRAME_H
#define GAME_OFRAME_H
// OFrame: one declaration for the units that each carried an identical copy (drone, trackobj). Port ODR work,
// 2026-11-16 (samesize.py). Names unproven.
#include "game/common.h"
#include "game/plainvec.h"

// An object's placement: three basis vectors and the origin (0x30).
struct OFrame {
    PVec x;    // offset 0x0
    PVec y;    // offset 0xC
    PVec z;    // offset 0x18
    PVec pos;  // offset 0x24
};

#endif
