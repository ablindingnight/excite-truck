#ifndef GAME_SQRTF_H
#define GAME_SQRTF_H
// A float square root through the double sqrt (minworld, pickup, ball). One definition for the port
// (aids audit 2, 2026-11-17: the three units carried identical copies).
#include "types.h"

extern "C" double sqrt(double x);

/* Helper name unproven */
static inline f32 SqrtF(f32 v) { return (f32)sqrt(v); }

#endif
