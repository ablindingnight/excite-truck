#ifndef GAME_MVEC4_H
#define GAME_MVEC4_H
// A 4-float vector (masslist, mlist and unk_801BD620's mass-point lists). Names unproven.
#include "game/common.h"

// w is left unset by the default and three-argument constructors (retail: e.g. fn_801BEB44 copies the unwritten
// stack word 0x14(r1) into the contact row's w). On the console that is a stack leftover; on the PC it can be any
// bit pattern (NaN). The PC build zeroes it so the solver's fourth lanes are deterministic; Wii code is unchanged.
struct MVec4 {
#ifdef __MWERKS__
    MVec4(void) {}
#else
    MVec4(void) { w = 0.0f; }
#endif
    MVec4(f32 x_, f32 y_, f32 z_) {  // w left unset (PC: zero, see above)
        x = x_;
        y = y_;
        z = z_;
#ifndef __MWERKS__
        w = 0.0f;
#endif
    }
    MVec4(f32 x_, f32 y_, f32 z_, f32 w_) {
        x = x_;
        y = y_;
        z = z_;
        w = w_;
    }
    f32 x;
    f32 y;
    f32 z;
    f32 w;
};

#endif
