#ifndef GAME_VPOS_H
#define GAME_VPOS_H

#include <types.h>
#include "game/rvec.h"

// An RVec3 with an out-of-line (x, y, z) constructor (modeinl.cpp), used where an RVec3 is expected: betarace
// passes temporaries, ball and panelvol construct one where it is declared. ppsys, masslist, trackmod and achview
// still call the constructor by name on locals declared earlier: their stack order needs the slot declared before
// another vector that it is constructed after (aids audit 2, 2026-11-17; Codex X-ray #4). Name unproven.
struct VPos : RVec3 {
    VPos(f32 x, f32 y, f32 z);
};

#endif
