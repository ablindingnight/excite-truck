#ifndef GAME_TSCALE_H
#define GAME_TSCALE_H
// o = a * k and o += v * k on any three-field vector types (Vec, RVec3, ValVec3). Many units carried identical copies
// under their own names (Scale, Scaled, ScaleV, ScaleB, ScaleVec_hm; AddScaled, AddSB, LAddScaled, DbgStep, ...);
// one definition for the port, like tadds.h's TAddS (aids audit 3, 2026-11-17: each unit converted only where
// code-identical).
#include "types.h"

/* Helper name unproven */
template <class O, class A> static inline void TScale(O* o, const A* a, f32 k) {
    o->x = a->x * k;
    o->y = a->y * k;
    o->z = a->z * k;
}

/* Helper name unproven */
template <class O, class V> static inline void TAddSTo(O* o, const V* v, f32 k) {
    o->x += v->x * k;
    o->y += v->y * k;
    o->z += v->z * k;
}

#endif
