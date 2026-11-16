#ifndef GAME_V2_H
#define GAME_V2_H
// A 2D point/vector (track-plane x and z, screen x and y). One declaration for every unit (port ODR work,
// 2026-11-16): V2 is the plain ctor-less form most units copy word by word; V2C adds the constructors the units
// that build V2 values need (idealline, arenabnd), with the same layout.
#include "game/common.h"

struct V2 {
    f32 x;  // offset 0x0
    f32 y;  // offset 0x4
};

struct V2C : V2 {
    V2C(void) {}
    V2C(f32 ax, f32 ay) {
        x = ax;
        y = ay;
    }
};

#endif
