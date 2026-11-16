#ifndef GAME_HIGHSCORE_H
#define GAME_HIGHSCORE_H

#include "game/common.h"

// One entry of an event's high-score table (stats.cpp; StatsGetHighScore returns one). Shared by its readers
// (awards, betarace, eventpath, recscreen) for the port (one definition).
struct HighScore {
    const char* name;  // offset 0x0
    s32 score;         // offset 0x4
    s32 grade;         // offset 0x8
    f32 time;          // offset 0xC, tie-break
    u32 choice;        // offset 0x10
};

#endif
