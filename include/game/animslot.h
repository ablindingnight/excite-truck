#ifndef GAME_ANIMSLOT_H
#define GAME_ANIMSLOT_H
// AnimSlot: one declaration for the units that each carried an identical copy (animobj, carmodel, wheelmod). Port ODR work,
// 2026-11-16 (samesize.py). Names unproven.
#include "game/common.h"
#include "game/vecmatrix.h"

// Frame: vecmatrix.h's VecMatrix.
// An animation slot.
struct AnimSlot {
    VecMatrix frame;  // offset 0x0
    s16 flags;    // offset 0x30
};

#endif
