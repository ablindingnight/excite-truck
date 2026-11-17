#ifndef GAME_BMTX33_H
#define GAME_BMTX33_H
// A 3x3 rotation (rows x, y, z) of the bone and drone code (bonemgr, boneset, drone). Plain data; names unproven.
#include "game/common.h"

struct BMtx33 {
    // Through row pointers, as bonemgr's code stores them (field stores schedule differently).
    void Identity(void) {
        struct Row {
            f32 x, y, z;
        };
        Row* r = (Row*)this;
        r[0].x = 1.0f;
        r[0].y = 0.0f;
        r[0].z = 0.0f;
        r[1].x = 0.0f;
        r[1].y = 1.0f;
        r[1].z = 0.0f;
        r[2].x = 0.0f;
        r[2].y = 0.0f;
        r[2].z = 1.0f;
    }
    f32 xx, xy, xz;
    f32 yx, yy, yz;
    f32 zx, zy, zz;
};

#endif
