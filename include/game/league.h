#ifndef GAME_LEAGUE_H
#define GAME_LEAGUE_H

#include "game/common.h"

// A league entry (league.cpp allocates four, tag "league"): the car size its races use and its name. Shared by league,
// eve, aiwidth and placecar, which each had their own view. Field names are guesses.
struct League {
    f32 carWid;     // offset 0x0
    f32 carLen;     // offset 0x4
    char name[32];  // offset 0x8
};

extern "C" League* fn_80137C54(s32 i);  // league i

#endif
