#ifndef GAME_CARDRAWWHEEL_H
#define GAME_CARDRAWWHEEL_H

#include "game/common.h"

// One wheel in the car's draw state (0x6C bytes): CarDrawState::wheels, also
// carmodel's per-wheel state and wheelmod's (it reads up to flags at 0x60; its names: v18 = roll, load = speed).
struct CarDrawWheel {
    f32 pos[3];     // offset 0x0: interpolated mount point
    f32 ground[3];  // offset 0xC
    f32 normal[3];  // offset 0x18
    f32 slip;       // offset 0x24
    f32 steer;      // offset 0x28
    f32 v18;        // offset 0x2C
    f32 load;       // offset 0x30
    f32 spin;       // offset 0x34
    f32 comp;       // offset 0x38
    f32 nRight;     // offset 0x3C
    f32 nFwd;       // offset 0x40
    f32 v10;        // offset 0x44
    f32 f48[6];     // offset 0x48
    s32 flags;      // offset 0x60
    u8 grounded;    // offset 0x64
    u8 pad65[3];
    s32 groundH;    // offset 0x68
};

#endif
