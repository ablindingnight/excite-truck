#ifndef GAME_VALVEC3_H
#define GAME_VALVEC3_H

// The game's canonical plain 3-vector (no constructor: copies as a plain struct). common.h, math_types.h (Vec3) and
// plainvec.h (V3, PVec, PVec3, CVec3, BVec, VecP) all use it (vector unification, 2026-11-16). Name unproven.
struct ValVec3 {
    void Set(float x_, float y_, float z_) {
        x = x_;
        y = y_;
        z = z_;
    }
    void Set(const ValVec3& v) {
        x = v.x;
        y = v.y;
        z = v.z;
    }
    float x, y, z;
};

#endif
