#ifndef GAME_CARRES_H
#define GAME_CARRES_H

#include "game/common.h"

// A loaded vehicle resource (fn_802792E4 by vehicle name): the bone count first and the vehicle's value table at
// 0x2134 (carmodel reads wheel classes from it, engsnd its engine sounds). Names are guesses.
struct CarRes {
    s32 numBones;        // offset 0x0
    u8 pad4[0x2130];
    ValueTable* values;  // offset 0x2134
};

#endif
