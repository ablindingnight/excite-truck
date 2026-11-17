#ifndef GAME_SHADOWLOD_H
#define GAME_SHADOWLOD_H
// One shadow level of detail: a model and the distance it is used to (carmodel.cpp stores the distance, wheelmod.cpp
// its square). Shared by carmodel.h and wheelmod (port ODR). Names unproven.
#include "game/common.h"

struct ShadowLod {
    s32 model;  // offset 0x0
    f32 dist;   // offset 0x4
};

#endif
