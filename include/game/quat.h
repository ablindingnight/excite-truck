#ifndef GAME_QUAT_H
#define GAME_QUAT_H
// A quaternion (quat.cpp's routines; camframe, drone and path interpolate with them). Plain data.
#include "game/common.h"

struct Quat {
    f32 x, y, z, w;
};

#endif
