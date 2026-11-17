#ifndef GAME_VERTEX_H
#define GAME_VERTEX_H
// Vertex: one declaration for the units that each carried an identical copy (dashhelper, trackmap). Port ODR work,
// 2026-11-16 (samesize.py). Names unproven.
#include "game/common.h"

struct Vertex {
    f32 data[6];
};

#endif
