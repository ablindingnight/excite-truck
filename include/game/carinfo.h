#ifndef GAME_CARINFO_H
#define GAME_CARINFO_H

#include "game/common.h"

struct CarDrawState;  // game/carbody.h

// carinfo.cpp's per-slot car record (0xBC; fn_801EA9A0 returns one of the eight). Names are guesses; draw, lap, state,
// driver and slot come from the units that read them.

struct CarSetup {
    CarSetup(void) {}
    s32 type;        // offset 0x0
    char name[0x20];   // offset 0x4
    char model[0x20];  // offset 0x24
    char paint[0x20];  // offset 0x44
    u32 x64;         // offset 0x64
    u32 driver;      // offset 0x68
    u32 x6C;         // offset 0x6C
    u8 slot;         // offset 0x70
    u32 flags;       // offset 0x74 (efxman passes it to fn_80287CE8)
};

struct CarParams {
    CarParams(void) {}
    u32 x0;   // offset 0x0
    s32 pos;  // offset 0x4
    u32 x8;   // offset 0x8
    f32 xC;   // offset 0xC
    s32 lap;  // offset 0x10
    f32 x14;  // offset 0x14
    f32 x18;  // offset 0x18
    f32 x1C;  // offset 0x1C
    f32 x20;  // offset 0x20
    s32 state;  // offset 0x24
    u8 impaired;  // offset 0x28
    f32 x2C;  // offset 0x2C
    u32 x30;  // offset 0x30
    u32 x34;  // offset 0x34
};

struct CarInfo {
    u8 active;         // offset 0x0
    s32 index;         // offset 0x4
    CarDrawState* draw;  // offset 0x8: the car's draw state in the frame buffer (carmodel fn_801EECAC sets it)
    CarParams params;  // offset 0xC
    CarSetup setup;    // offset 0x44
};

#endif
