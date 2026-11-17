#ifndef GAME_CAMFRAME_H
#define GAME_CAMFRAME_H

#include "game/common.h"
#include "game/camrec.h"  // CamPose

// Camera-frame helpers shared by the camera units (camfov, camstate). A camera keeps its rows at
// +0x58/+0x64/+0x70 (right, up, view axis) and its position at +0x7C.

#include <revolution/MTX/vec.h>

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

// Rebuilds the camera pose's rows (CamRec::pose, 0x58) as an orthonormal basis that keeps the view axis (row 2).
static inline void Orthonormalize(CamPose* p) {
    CsVec x;
    CsVec y;
    CsVec z;
    x.Set(p->rot.m[0], p->rot.m[1], p->rot.m[2]);
    z.Set(p->rot.m[6], p->rot.m[7], p->rot.m[8]);
    PSVECCrossProduct((const Vec*)&z.x, (const Vec*)&x.x, (Vec*)&y.x);
    PSVECNormalize((const Vec*)&x.x, (Vec*)&x.x);
    PSVECNormalize((const Vec*)&y.x, (Vec*)&y.x);
    PSVECCrossProduct((const Vec*)&x.x, (const Vec*)&y.x, (Vec*)&z.x);
    p->rot.m[0] = x.x;
    p->rot.m[1] = x.y;
    p->rot.m[2] = x.z;
    p->rot.m[3] = y.x;
    p->rot.m[4] = y.y;
    p->rot.m[5] = y.z;
    p->rot.m[6] = z.x;
    p->rot.m[7] = z.y;
    p->rot.m[8] = z.z;
}

#endif
