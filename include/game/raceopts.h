#ifndef GAME_RACEOPTS_H
#define GAME_RACEOPTS_H
// The current race's options: the 0x14-byte tail of world.cpp's RaceSetup (at 0x3E4), which fn_801E87A8 returns.
// One declaration for every unit (port ODR work, 2026-11-16); the units used to view it as EventCfg, RaceOpts,
// Strength, ViewOpts, ViewInfo, ViewFlags, StuntOpts, FireOpts, CarDesc or a bare int/u32 array. Names unproven.
#include "game/common.h"

struct RaceOpts {
    s32 event;       // offset 0x0: the event (GetEvent index)
    u32 type;        // offset 0x4: the event type (0x180302, 0x180304, ...; bit 0x200 = arena)
    s32 difficulty;  // offset 0x8
    s32 level;       // offset 0xC: the AI strength level (aiskill, catchupval)
    u32 flags;       // offset 0x10: 2 = high resolution ground
};

extern "C" RaceOpts* fn_801E87A8(void);

#endif
