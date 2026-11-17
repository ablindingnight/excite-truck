#ifndef GAME_CARGEAR_H
#define GAME_CARGEAR_H

#include "game/common.h"

// One gear of a car's gearbox.
struct Gear {
    f32 ratio;  // offset 0x0
    f32 up;     // offset 0x4: upshift speed
    f32 down0;  // offset 0x8: downshift speed at full load
    f32 down1;  // offset 0xC: downshift speed at no load
};

// A car's gearbox state (cargear.cpp keeps one per car, 0x9C apart; carbody fetches and clears it). Names are
// guesses.
struct CarGear {
    CarGear(void) {}
    void Update(int car);
    void Fetch(int car);
    void Clear(int car);
    void Compute(int car);
    f32 f0;         // offset 0x0
    f32 f4;         // offset 0x4
    s32 request;    // offset 0x8: -10 for automatic
    s32 target;     // offset 0xC
    s32 prev;       // offset 0x10
    s32 gear;       // offset 0x14
    f32 shiftTime;  // offset 0x18
    f32 sinceShift; // offset 0x1C
    s32 count;      // offset 0x20
    f32 f24;        // offset 0x24
    f32 f28;        // offset 0x28
    Gear gears[7];  // offset 0x2C
};

#endif
