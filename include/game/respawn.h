#ifndef GAME_RESPAWN_H
#define GAME_RESPAWN_H

#include "game/common.h"

// Where a car is put (placecar.cpp): its frame, velocity and line-hint node (-1 = find one). The place-only
// functions (fn_801B7C8C, CalculateCarPositionAndDirection, fn_801B8060, ...) fill just this part.
struct CarPlace {  /* Helper name unproven */
    ValMatrix place;  // offset 0x0
    ValVec3 vel;      // offset 0x30
    s32 unk3C;        // offset 0x3C
    s16 hint;         // offset 0x40
};

// A car's start pose (fn_801B8100): the place plus how the car starts.
struct Respawn : CarPlace {
    s32 mode;   // offset 0x44
    s32 unk48;  // offset 0x48
    s32 unk4C;  // offset 0x4C
};

#endif
