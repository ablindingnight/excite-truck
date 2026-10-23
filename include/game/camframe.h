#ifndef GAME_CAMFRAME_H
#define GAME_CAMFRAME_H

#include "game/common.h"

// Camera-frame helpers shared by the camera units (camfov, camstate). A camera keeps its rows at
// +0x58/+0x64/+0x70 (right, up, view axis) and its position at +0x7C.

extern "C" {
void PSVECCrossProduct(const f32* a, const f32* b, f32* out);
void PSVECNormalize(const f32* in, f32* out);
}

// |x| as a float (a plain (f32)__fabs(x) allocates the compare differently).
static inline f32 AbsF(f32 x) { return __fabs(x); }

struct CsVec {
    CsVec(void) {}
    CsVec(f32 a, f32 b, f32 c) : x(a), y(b), z(c) {}
    void Set(f32 a, f32 b, f32 c) {
        x = a;
        y = b;
        z = c;
    }
    f32 x, y, z;
};

// Rebuilds the camera rows as an orthonormal basis that keeps the view axis (row 2).
static inline void Orthonormalize(u8* p) {
    CsVec x;
    CsVec y;
    CsVec z;
    x.Set(*(f32*)(p + 0x58), *(f32*)(p + 0x5C), *(f32*)(p + 0x60));
    z.Set(*(f32*)(p + 0x70), *(f32*)(p + 0x74), *(f32*)(p + 0x78));
    PSVECCrossProduct(&z.x, &x.x, &y.x);
    PSVECNormalize(&x.x, &x.x);
    PSVECNormalize(&y.x, &y.x);
    PSVECCrossProduct(&x.x, &y.x, &z.x);
    *(f32*)(p + 0x58) = x.x;
    *(f32*)(p + 0x5C) = x.y;
    *(f32*)(p + 0x60) = x.z;
    *(f32*)(p + 0x64) = y.x;
    *(f32*)(p + 0x68) = y.y;
    *(f32*)(p + 0x6C) = y.z;
    *(f32*)(p + 0x70) = z.x;
    *(f32*)(p + 0x74) = z.y;
    *(f32*)(p + 0x78) = z.z;
}

#endif
