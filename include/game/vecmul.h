#ifndef GAME_VECMUL_H
#define GAME_VECMUL_H
// Row vector times the first three rows of a Vec[] matrix (out = in * m), safe when out aliases in, and the plain
// three-float setter. The particle and light units carried identical copies (MulVec, SetVec); one definition for
// the port (aids audit 3, 2026-11-17: each unit converted only where code-identical).
#include <revolution/MTX/mtxtypes.h>
#include "game/mathutil.h"

/* Helper name unproven */
static inline void MulVec(Vec* out, const Vec* in, const Vec* m) {
    if (out == in) {
        f32 x = Dot3(in->x, m[0].x, in->y, m[1].x, in->z, m[2].x);
        f32 y = Dot3(in->x, m[0].y, in->y, m[1].y, in->z, m[2].y);
        f32 z = Dot3(in->x, m[0].z, in->y, m[1].z, in->z, m[2].z);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, m[0].x, in->y, m[1].x, in->z, m[2].x);
        out->y = Dot3(in->x, m[0].y, in->y, m[1].y, in->z, m[2].y);
        out->z = Dot3(in->x, m[0].z, in->y, m[1].z, in->z, m[2].z);
    }
}

/* Helper name unproven */
static inline void SetVec(Vec* v, f32 x, f32 y, f32 z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

#endif
