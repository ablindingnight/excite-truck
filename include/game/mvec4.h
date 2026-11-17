#ifndef GAME_MVEC4_H
#define GAME_MVEC4_H
// A 4-float vector (masslist, mlist and unk_801BD620's mass-point lists). Names unproven.
#include "game/common.h"

struct MVec4 {
    MVec4(void) {}
    MVec4(f32 x_, f32 y_, f32 z_) {  // w left unset
        x = x_;
        y = y_;
        z = z_;
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
