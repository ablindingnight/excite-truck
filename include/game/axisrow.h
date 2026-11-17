#ifndef GAME_AXISROW_H
#define GAME_AXISROW_H
// One matrix row stored through by-value parameters (namedcam, playcar). One definition for the port (aids
// audit 2, 2026-11-17: the units carried identical copies).
#include "types.h"

/* Helper name unproven: three values stored through by-value parameters (one matrix row). */
static inline void AxisRow(f32& a, f32& b, f32& c, f32 x, f32 y, f32 z) {
    a = x;
    b = y;
    c = z;
}

#endif
