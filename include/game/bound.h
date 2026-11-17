#ifndef GAME_BOUND_H
#define GAME_BOUND_H

#include "game/common.h"
#include "game/v2.h"

// A ground-plane boundary polygon (arenabnd.cpp allocates them, tag "arenabnd", from .val nodes via fn_801399FC):
// point count, capacity and the (x, z) points, as v2.h V2s. Names are guesses.
typedef V2 BoundPt;

struct Bound {
    s32 n;           // offset 0x0
    s32 cap;         // offset 0x4
    BoundPt pts[1];  // offset 0x8
};

#endif
