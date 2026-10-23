#ifndef GAME_PATHSTATICS_H
#define GAME_PATHSTATICS_H

#include "game/common.h"

// Header statics defined by every unit that includes the path/career header; each
// unit's __sinit constructs its own copies.
struct PathInfo {
    PathInfo(void) {
        a4 = 0x7B;
        a0 = 0;
        a8 = 0;
        aC = 0;
        a10 = 0;
    }
    s32 a0, a4, a8, aC, a10;
};

struct PathLimit {
    PathLimit(void) { v = 0x7F000000; }
    s32 v;
};

#endif
