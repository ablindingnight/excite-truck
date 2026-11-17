#ifndef GAME_ACHQUERY_H
#define GAME_ACHQUERY_H
// An achievement total query (achieve.cpp's AchievementsQuery): the category asked for, then the profile's total
// and the overall total (profile + session + race). Names from achieve.cpp; unproven.
#include "game/common.h"

struct AchQuery {
    s32 category;  // offset 0x0
    s32 profile;   // offset 0x4
    s32 total;     // offset 0x8
};

#endif
