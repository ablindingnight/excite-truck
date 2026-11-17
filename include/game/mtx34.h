#ifndef GAME_MTX34_H
#define GAME_MTX34_H
// A plain 3x4 matrix (rows of rotation + translation), copied whole (cammgr's camera matrix, trackmap's view,
// modeinl's copy helper). Plain data; names unproven.
#include "game/common.h"

struct Mtx34 {
    f32 m[3][4];
};

#endif
