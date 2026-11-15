#ifndef GAME_VPOS_H
#define GAME_VPOS_H

#include <types.h>

struct VPos {
    VPos(f32 x, f32 y, f32 z);
    f32 x;
    f32 y;
    f32 z;
};

#endif
