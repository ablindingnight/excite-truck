#ifndef GAME_MTX33_H
#define GAME_MTX33_H

#include "types.h"
#include <math.h>

// A 3x3 rotation matrix, rows xx..xz, yx..yz, zx..zz. One declaration for modeinl (which defines SetEuler), boats,
// caruimod and cardmg (port ODR work, 2026-11-16; joint's row view became math_types.h's Mat3). Name unproven.
struct Mtx33 {
    void SetRotX(f32 a) {
        f32 c = cos(a);
        f32 s = sin(a);
        xx = 1.0f;
        xy = 0.0f;
        xz = 0.0f;
        yx = 0.0f;
        yy = c;
        yz = s;
        zx = 0.0f;
        zy = -s;
        zz = c;
    }
    void SetRotY(f32 a) {
        f32 c = cos(a);
        f32 s = sin(a);
        xx = c;
        xy = 0.0f;
        xz = -s;
        yx = 0.0f;
        yy = 1.0f;
        yz = 0.0f;
        zx = s;
        zy = 0.0f;
        zz = c;
    }
    void SetRotZ(f32 a) {
        f32 c = cos(a);
        f32 s = sin(a);
        xx = c;
        xy = s;
        xz = 0.0f;
        yx = -s;
        yy = c;
        yz = 0.0f;
        zx = 0.0f;
        zy = 0.0f;
        zz = 1.0f;
    }
    static Mtx33 RotX(f32 a) {
        Mtx33 m;
        m.SetRotX(a);
        return m;
    }
    static Mtx33 RotZ(f32 a) {
        Mtx33 m;
        m.SetRotZ(a);
        return m;
    }
    void SetEuler(f32 y, f32 x, f32 z);
    f32 xx, xy, xz;
    f32 yx, yy, yz;
    f32 zx, zy, zz;
};

#endif
