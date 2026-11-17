#ifndef GAME_AIGRID_H
#define GAME_AIGRID_H

#include "game/common.h"
#include "game/racesetup.h"

// The starting grid the AI races from: up to eight car definitions and the count (aidriver's race record holds one;
// aiskill reads each car's driver). Names are guesses.
struct AIGrid {
    CarDef cars[8];  // offset 0x0
    s32 count;       // offset 0x3C0
};

#endif
