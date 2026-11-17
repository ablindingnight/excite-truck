#ifndef GAME_RACERESULTENTRY_H
#define GAME_RACERESULTENTRY_H
// RaceResultEntry: one declaration for the units that each carried an identical copy (gamedo, gameloop). Port ODR work,
// 2026-11-16 (samesize.py). Names unproven.
#include "game/common.h"
#include "game/bink_internal.h"

// The drivers/vehicles of one race.
struct RaceResultEntry {
    u8 pad0[0x10];
    s32 value;  // offset 0x10
};

#endif
