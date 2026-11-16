#ifndef GAME_PLAINVEC_H
#define GAME_PLAINVEC_H

#include "game/common.h"

// The plain 3-float vector spellings the game units use, one declaration each (port ODR work, 2026-11-16). They
// are kept apart from RVec3/Vec3 because their copies and constructors differ (a struct with a constructor copies
// with floats, retail's code depends on which). Names unproven.

// x, y, z with no constructor (copies as a plain struct).
struct V3 {
    void Set(f32 x_, f32 y_, f32 z_) {
        x = x_;
        y = y_;
        z = z_;
    }
    f32 x;
    f32 y;
    f32 z;
};

// A ValVec3 built from three floats.
struct ValV3 : ValVec3 {
    ValV3(f32 x_, f32 y_, f32 z_) {
        x = x_;
        y = y_;
        z = z_;
    }
};

// x, y, z with no constructor; Set from three floats (precip) or from a ValVec3 (trackobj).
struct PVec {
    void Set(f32 a, f32 b, f32 c) {
        x = a;
        y = b;
        z = c;
    }
    void Set(const ValVec3& v) {
        x = v.x;
        y = v.y;
        z = v.z;
    }
    f32 x, y, z;
};

// x, y, z with no constructor (phob's Set helper).
struct PVec3 {
    void Set(f32 x_, f32 y_, f32 z_) {
        x = x_;
        y = y_;
        z = z_;
    }
    f32 x;
    f32 y;
    f32 z;
};

// x, y, z with no constructor.
struct CVec3 {
    f32 x, y, z;
};

#endif
