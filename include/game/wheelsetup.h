#ifndef GAME_WHEELSETUP_H
#define GAME_WHEELSETUP_H

#include "game/common.h"

// One wheel's suspension setup (CarSetup::wheels[4], car setup files). carsetup's defaults set radius to 0.3556 m
// (14 in); caruimod reads it as the wheel radius. Other names are guesses.
struct WheelSetup {
    f32 rideHeight;      // offset 0x0
    f32 spring;          // offset 0x4
    f32 bump;            // offset 0x8
    f32 rebound;         // offset 0xC
    f32 bumpstopHeight;  // offset 0x10
    f32 bumpstopRate;    // offset 0x14
    f32 radius;          // offset 0x18
    f32 unk1C;           // offset 0x1C
    f32 unk20;           // offset 0x20
};

#endif
