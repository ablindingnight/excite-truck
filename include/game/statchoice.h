#ifndef GAME_STATCHOICE_H
#define GAME_STATCHOICE_H
// StatChoice: one declaration for the units that each carried an identical copy (carpick, frname). Port ODR work,
// 2026-11-16 (samesize.py). Names unproven.
#include "game/common.h"

// The car and colour a record was set with.
struct StatChoice {
    s32 vehicle;  // offset 0x0
    s32 color;    // offset 0x4
    u8 valid;     // offset 0x8
};

#endif
