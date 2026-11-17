#ifndef GAME_TVTX_H
#define GAME_TVTX_H

#include "game/common.h"
#include "game/valvec3.h"

// A textured, coloured vertex (0x18): tristrip's strip buffer (lbl_80487578) and trackmod's billboard quads. Names
// are guesses.
struct TVtx {
    ValVec3 pos;  // offset 0x0
    u32 color;    // offset 0xC
    f32 u, v;     // offset 0x10
};

#endif
