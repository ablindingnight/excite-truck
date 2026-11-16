#ifndef GAME_VALVEC3_H
#define GAME_VALVEC3_H

// The game's canonical plain 3-vector (no constructor: copies as a plain struct). common.h and math_types.h (Vec3)
// both use it (vector unification, 2026-11-16). Name unproven.
struct ValVec3 {
    float x, y, z;
};

#endif
