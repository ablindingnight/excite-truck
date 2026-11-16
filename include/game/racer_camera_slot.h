#ifndef GAME_RACER_CAMERA_SLOT_H
#define GAME_RACER_CAMERA_SLOT_H

#include <types.h>
#include "game/math_types.h"
#include "game/common.h"  // ValVec3, PlainValMatrix (the camera frame: rotation rows + position)

// The camera frame and motion recorded for one racer.
struct CameraRacerSlot {
    u8 active;          // offset 0x0
    PlainValMatrix frame;       // offset 0x4: the camera frame when the orbit started
    f32 fov;            // offset 0x34
    f32 yaw;            // offset 0x38
    ValVec3 target;        // offset 0x3C
    ValVec3 pos;           // offset 0x48
    ValVec3 vel;           // offset 0x54
    f32 t;              // offset 0x60
    f32 spin;           // offset 0x64
    s32 frameNo;        // offset 0x68
    CameraRacerSlot(void);
};

#endif
