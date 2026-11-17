#ifndef GAME_VEHCHOICE_H
#define GAME_VEHCHOICE_H

#include "game/common.h"

// A vehicle + colour choice with a set flag (stats.cpp; recscreen reads them). Names are guesses.
struct VehicleChoice {
    s32 vehicle;  // offset 0x0
    s32 color;    // offset 0x4
    bool flag;    // offset 0x8: the choice is set
};

#endif
