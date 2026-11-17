#ifndef GAME_CORNER_H
#define GAME_CORNER_H

#include "game/rvec.h"

class RigidBody;

// A contact corner: a point on a rigid body that pushes back from the ground (corners.cpp defines Hit and Stop and
// pools them). A body keeps its corners in a list (RigidBody::contacts, 0xD0). One declaration for corners, rigid,
// carbody and obstacles (port ODR work, 2026-11-16). Name unproven.
// A contact point in body space.
struct Corner {
    Corner(void) {
        pos.x = 0.0f;
        pos.y = 0.0f;
        pos.z = 0.0f;
        spring = 1000.0f;
        friction = -1000.0f;
        damping = 0.0f;
        vel.x = 0.0f;
        vel.y = 0.0f;
        vel.z = 0.0f;
        next = 0;
    }
    void Hit(RigidBody* b);
    void Stop(void);

    Corner* next;  // offset 0x0: the body's list (RigidBody::contacts)
    RVec3 pos;     // offset 0x4
    f32 spring;    // offset 0x10
    f32 friction;  // offset 0x14
    f32 damping;   // offset 0x18
    RVec3 vel;     // offset 0x1C
};

#endif
