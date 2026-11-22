#ifndef GAME_HITCH_H
#define GAME_HITCH_H

#include "game/common.h"
#include "game/rvec.h"

class CollVolume;

// A spring joint between a point on its owner volume and a point on another volume (or a fixed world point): the
// trailer's hitch to its car, obstacles' dynamic links. obstacles.cpp's fn_801AD0E0/AD148/AD28C set it up and
// ApplyJointSpringDamping applies it. One declaration for every unit (port ODR work, 2026-11-16). Field names unproven.
struct Hitch {
    Hitch(void) {
        owner = 0;
        other = 0;
        stretch = 0.0f;
        load = 0.0f;
    }
    PlainRVec3 at;      // offset 0x0: the point on `other` (its local frame), or the world point
    CollVolume* other;  // offset 0xC: the other volume (0: a fixed world point; stored from an int id)
    PlainRVec3 local;   // offset 0x10: the point on `owner`, in its local frame
    f32 k;              // offset 0x1C: spring constant
    f32 damp;           // offset 0x20: damping
    f32 limit;          // offset 0x24
    f32 stretch;        // offset 0x28: rest length
    f32 load;           // offset 0x2C
    f32 f30;            // offset 0x30
    CollVolume* owner;  // offset 0x34
};

#endif
