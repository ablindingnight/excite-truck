#ifndef GAME_CARSTAT_H
#define GAME_CARSTAT_H
// CarStat: one declaration for the units that each carried an identical copy (carstats, garage, mroll). Port ODR work,
// 2026-11-16 (samesize.py). Names unproven.
#include "game/common.h"
#include "game/bink_internal.h"

struct CarStat {
    const char* name;
    s32 value;
};

#endif
