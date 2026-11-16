#ifndef GAME_RMTX_H
#define GAME_RMTX_H

#include "game/rvec.h"

// A 3x3 matrix as three rows (rotations, inertia tensors). Name unproven.
struct RMat33 {
    RMat33(void) {}
    RVec3 x;  // offset 0x0
    RVec3 y;  // offset 0xC
    RVec3 z;  // offset 0x18
    void Transpose(void) {
        f32 t;
        t = x.y;
        x.y = y.x;
        y.x = t;
        t = x.z;
        x.z = z.x;
        z.x = t;
        t = y.z;
        y.z = z.y;
        z.y = t;
    }
};

// The 0x30 frame the collision and rigid-body code keeps (CollVolume's mtx/world/prev at 0x3C/0x70/0xA0): the
// rotation, then the position. One declaration for every unit (port ODR work, 2026-11-16). Other units still spell
// the same layout as CMtx, BMtx, CFrame or TMtx; they join as their accesses are converted. Name unproven.
struct RMtx {
    RMat33 rot;  // offset 0x0
    RVec3 pos;   // offset 0x24
};

#endif
