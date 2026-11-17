#ifndef GAME_LERPMTX_H
#define GAME_LERPMTX_H
// Blends two 3x4 frames word by word (panelvol, wheel, mutphob, ball). One definition for the port (aids audit 2,
// 2026-11-17: the units carried identical copies).
#include "game/rmtx.h"
#include "game/mathutil.h"

/* Helper name unproven */
static inline void LerpMtx(RMtx* out, const RMtx* a, const RMtx* b, f32 t) {
    f32* d = (f32*)out;
    const f32* x = (const f32*)a;
    const f32* y = (const f32*)b;
    for (int i = 0; i < 12; i++) {
        *d++ = Lerp(*x++, *y++, t);
    }
}

#endif
