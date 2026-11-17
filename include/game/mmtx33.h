#ifndef GAME_MMTX33_H
#define GAME_MMTX33_H
// MMtx33: one declaration for the units that each carried an identical copy (mlist, model). Port ODR work,
// 2026-11-16 (samesize.py). Names unproven.
#include "game/common.h"

struct MMtx33 {
    f32 xx, xy, xz;
    f32 yx, yy, yz;
    f32 zx, zy, zz;
};

#endif
