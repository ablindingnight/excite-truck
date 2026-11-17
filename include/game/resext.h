#ifndef GAME_RESEXT_H
#define GAME_RESEXT_H

#include "game/common.h"

// An extension block chained off an ideal-line resource (idealres.cpp byte-swaps them by type; aiskill reads a
// float out of one, fn_80138B1C finds it). The payload is words or floats by type. Names are guesses.
struct ResExt {
    u32 type;      // offset 0x0
    ResExt* next;  // offset 0x4
    union {
        u32 data[1];   // offset 0x8
        f32 fdata[1];
    };
};

#endif
