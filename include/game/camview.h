#ifndef GAME_CAMVIEW_H
#define GAME_CAMVIEW_H

#include "game/camrec.h"

// The view the camera manager fills for one player each frame (0xE0 bytes): pose, projection, viewport rect and
// what the camera follows. interlude.cpp still declares its own copy (with its own CamPose): unify when that does.
struct CamView : CamPose {  // rot at 0x0, pos at 0x24
    f32 fov;         // offset 0x30
    f32 tanHalf;     // offset 0x34, tan of half the view angle
    s32 x;           // offset 0x38, viewport rect (split screen)
    s32 y;           // offset 0x3C
    s32 w;           // offset 0x40
    s32 h;           // offset 0x44
    f32 nearZ;       // offset 0x48
    f32 farZ;        // offset 0x4C
    f32 rumble;      // offset 0x50
    f32 f54;         // offset 0x54
    s32 modeB;       // offset 0x58
    s32 modeA;       // offset 0x5C
    s32 car;         // offset 0x60
    s16 carId;       // offset 0x64
    u8 pad66[2];
    s32 flags;       // offset 0x68
    u8 pad6C[0xE0 - 0x6C];
};

#endif
