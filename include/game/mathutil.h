#ifndef GAME_MATHUTIL_H
#define GAME_MATHUTIL_H

// Small scalar helpers the game expands inline in many units. Each unit used to carry its own copies under invented
// names (Lerp, LerpF, FrameLerp, MaxF, FMax, Max, AbsF, ...); identical bodies are now defined once here
// (aids audit 2, 2026-11-16: each unit converted only where it is code-identical on every function).
// Helper names unproven. Operand order and ternary orientation are part of each body and decide the generated code
// (argument evaluation, compare orientation), so bodies that differ only in that order keep separate names
// (MinF / MinFRev). Do not merge them.
#include "types.h"

// a + t * (b - a)
inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }
// the same blend with t first in the argument list
inline f32 LerpT(f32 t, f32 a, f32 b) { return a + t * (b - a); }
// v mapped from lo..hi to 0..1 (no clamp), and the same with v last
inline f32 Unlerp(f32 v, f32 lo, f32 hi) { return (v - lo) / (hi - lo); }
inline f32 InvLerp(f32 lo, f32 hi, f32 v) { return (v - lo) / (hi - lo); }
// three products summed (arguments in pairs: a*b + c*d + e*f)
inline f32 Dot3(f32 a, f32 b, f32 c, f32 d, f32 e, f32 f) { return a * b + c * d + e * f; }
// x*x + y*y
inline f32 Len2(f32 x, f32 y) { return x * x + y * y; }
inline f32 Sq(f32 x) { return x * x; }
// a product whose second factor arrives as an inline parameter (keeps the variable first)
inline f32 Mul(f32 a, f32 b) { return a * b; }
inline f32 DegToRad(f32 d) { return 0.017453292f * d; }

inline f32 MaxF(f32 a, f32 b) { return a > b ? a : b; }
inline f32 MinF(f32 a, f32 b) { return a < b ? a : b; }
// the minimum with the compare written the other way round (b first)
inline f32 MinFRev(f32 a, f32 b) { return b < a ? b : a; }
inline int MaxI(int a, int b) { return a > b ? a : b; }
inline int MinI(int a, int b) { return a < b ? a : b; }
inline int MinIRev(int a, int b) { return b < a ? b : a; }

// |x| through the double fabs, narrowed to float
inline f32 FAbs(f32 x) { return (f32)__fabs(x); }
inline double AbsD(double x) { return __fabs(x); }

// 0xAARRGGBB
inline u32 ARGB(u8 a, u8 r, u8 g, u8 b) { return a << 24 | r << 16 | g << 8 | b; }

#endif
