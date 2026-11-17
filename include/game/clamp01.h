#ifndef GAME_CLAMP01_H
#define GAME_CLAMP01_H
// Clamp to 0..1: the upper bound first (1 < x), then the lower (> 0). Many units carried their own copy under this
// name, spelled with a local, a reassigned parameter or MaxF(MinFRev(..)); every spelling gives the same code, so one
// definition serves them all (aids audit 3, 2026-11-17: each unit converted only where code-identical).
#include "types.h"

/* Helper name unproven */
static inline f32 Clamp01(f32 x) {
    f32 r = 1.0f < x ? 1.0f : x;
    return r > 0.0f ? r : 0.0f;
}

#endif
