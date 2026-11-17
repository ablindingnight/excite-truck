#ifndef GAME_OBJTELEPORT_H
#define GAME_OBJTELEPORT_H

#include "game/common.h"

// A teleport target (ringchal; retail's weak __ct__11ObjTeleportFv / __dt__11ObjTeleportFv): the pose a car is put at
// (placecar's fn_801B8060 reads the origin and the forward axis basis[2]), whether it is set, and its radius.
// Field names are guesses.
struct ObjTeleport {
    ObjTeleport(void) {
        b30 = 0;
        valid = 0;
        radius = 80.0f;
    }
    ~ObjTeleport(void) {}
    ValMatrix mtx;  // offset 0x0
    u8 b30;         // offset 0x30
    u8 valid;       // offset 0x31
    u8 pad32[2];
    f32 radius;     // offset 0x34
};

#endif
