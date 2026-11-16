#ifndef GAME_RVEC_H
#define GAME_RVEC_H

#include "game/common.h"

// The game's 3-vector (collision, rigid bodies, shapes). One declaration for every unit (port ODR work, 2026-11-16).
//
// Why two layers (harness DataOrphans scratchpad rv_ctor / rv_pod / rv_layer, all 12 users recompiled):
// - Units that construct vectors need the constructors (RVec3(x, y, z), Set).
// - Initialising a vector from another (RVec3 p = mtx.pos) copies it with integer loads/stores (lwz/stw) in retail.
//   MWCC does that only when the copy is a constructor-less struct's: a plain RVec3 with constructors copies with
//   lfs/stfs (PanelVol::Update EXACT -> 64.0). Deriving from a constructor-less base keeps the implicit copy plain.
// - An ARRAY of the constructor class differs from a plain array (sphcoll MeshShape::vf10's 64-hit buffer is plain
//   storage), and the constructor class cannot be brace-initialised (write RVec3(x, y, z) elements).
// Names unproven (no retail symbols); RVec3 is the name the project's mangled symbols already use.
struct PlainRVec3 {
    f32 x;
    f32 y;
    f32 z;
};

struct RVec3 : PlainRVec3 {
    RVec3(void) {}
    RVec3(f32 a, f32 b, f32 c) {
        x = a;
        y = b;
        z = c;
    }
    void Set(f32 a, f32 b, f32 c) {
        x = a;
        y = b;
        z = c;
    }
    RVec3& operator+=(const PlainRVec3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
};

#endif
