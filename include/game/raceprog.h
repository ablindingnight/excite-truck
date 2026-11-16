#ifndef GAME_RACEPROG_H
#define GAME_RACEPROG_H
// The race's progress (world.cpp's WorldState at 0x560, after the eight per-racer records; fn_801E7E3C returns it).
// One declaration for every unit (port ODR work, 2026-11-16); units used to view it as RaceSetup, RaceSetupHdr,
// LapInfo, RaceLaps, RaceClock or void*. Names unproven.
#include "game/common.h"

struct RaceProgress {
    s32 leaderLap;  // offset 0x0: the leader's lap
    s32 f4;         // offset 0x4
    s32 laps;       // offset 0x8: laps in the race
    f32 start;      // offset 0xC
    f32 time;       // offset 0x10: time since the intro started
};

extern "C" RaceProgress* fn_801E7E3C(void);

#endif
