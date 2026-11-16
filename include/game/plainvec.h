#ifndef GAME_PLAINVEC_H
#define GAME_PLAINVEC_H

#include "game/common.h"

// The plain 3-float vector spellings the game units use, one declaration each (port ODR work, 2026-11-16). They
// are kept apart from RVec3/Vec3 because their copies and constructors differ (a struct with a constructor copies
// with floats, retail's code depends on which). Names unproven.

// The plain 3-vector spellings the units used are all ValVec3 (vector unification, 2026-11-16).
typedef ValVec3 V3;
typedef ValVec3 PVec;
typedef ValVec3 PVec3;
typedef ValVec3 CVec3;
typedef ValVec3 BVec;
typedef ValVec3 VecP;

// A ValVec3 built from three floats.
struct ValV3 : ValVec3 {
    ValV3(f32 x_, f32 y_, f32 z_) {
        x = x_;
        y = y_;
        z = z_;
    }
};

// Three floats as an array (copies as words).
struct WV3 {
    f32 v[3];
};

#endif
