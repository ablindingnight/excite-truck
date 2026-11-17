#ifndef GAME_TADDS_H
#define GAME_TADDS_H
// o = a + v * k on any three-field vector types (caruimod, fire, placecar, carbody). One definition for the port
// (aids audit 2, 2026-11-17: the units carried identical copies, carbody as CbAddScaled).
#include "types.h"

/* Helper name unproven */
template <class O, class A, class V> static inline void TAddS(O* o, const A* a, const V* v, f32 k) {
    o->x = a->x + v->x * k;
    o->y = a->y + v->y * k;
    o->z = a->z + v->z * k;
}

#endif
