#ifndef GAME_MAT3_INL_H
#define GAME_MAT3_INL_H

// Mat3Mul as the header inline it is in the retail build (opt-in per unit until every caller is converted).
//
// Proof that it is an inline: drivmod fn_802009CC / fn_80201018 and interlude fn_801C5B68 contain its whole body
// expanded in place, with the eighteen 6-argument dot products left as calls to the weak Dot__Fffffff. That is what
// MWCC does with an expanded inline whose size estimate is over its nested-inline budget (512 units; this body
// measures 527): the body is expanded, but nothing it calls is. racing.cpp holds the copy the linker kept
// (Mat3RotZ, Mat3Mul, Mat3RotX, Mat3RotY after Mat3FromEuler = reverse first-use order of weak copies).
//
// What it means for callers: an inline body that calls Mat3Mul is itself over the budget, so every inline it uses
// stays a call (the "nothing is inlined here" functions: achview's trophy Update and Draw). No dont_inline or
// inline_max_size pragma is needed. See FACTS.md "2026-11-17 (fable-rules2)".
//
// Units that include this get a weak copy of Mat3Mul after its first caller; the linker drops it for racing.cpp's.
// The PC build keeps the plain declaration in math_types.h (one definition, in racing.cpp).
#include "game/math_types.h"

#ifdef __MWERKS__
// The 6-argument dot product: modeinl holds the kept weak copy (Dot__Fffffff).
inline f32 Dot(f32 a, f32 b, f32 c, f32 d, f32 e, f32 f) { return a * b + c * d + e * f; }

inline void Mat3Mul(Mat3* out, const Mat3* a, const Mat3* b) {
    if (out == a || out == b) {
        f32 m00 = Dot(b->m[0][0], a->m[0][0], b->m[1][0], a->m[0][1], b->m[2][0], a->m[0][2]);
        f32 m10 = Dot(b->m[0][0], a->m[1][0], b->m[1][0], a->m[1][1], b->m[2][0], a->m[1][2]);
        f32 m20 = Dot(b->m[0][0], a->m[2][0], b->m[1][0], a->m[2][1], b->m[2][0], a->m[2][2]);
        f32 m01 = Dot(b->m[0][1], a->m[0][0], b->m[1][1], a->m[0][1], b->m[2][1], a->m[0][2]);
        f32 m11 = Dot(b->m[0][1], a->m[1][0], b->m[1][1], a->m[1][1], b->m[2][1], a->m[1][2]);
        f32 m21 = Dot(b->m[0][1], a->m[2][0], b->m[1][1], a->m[2][1], b->m[2][1], a->m[2][2]);
        f32 m02 = Dot(b->m[0][2], a->m[0][0], b->m[1][2], a->m[0][1], b->m[2][2], a->m[0][2]);
        f32 m12 = Dot(b->m[0][2], a->m[1][0], b->m[1][2], a->m[1][1], b->m[2][2], a->m[1][2]);
        f32 m22 = Dot(b->m[0][2], a->m[2][0], b->m[1][2], a->m[2][1], b->m[2][2], a->m[2][2]);
        out->m[0][0] = m00;
        out->m[1][0] = m10;
        out->m[2][0] = m20;
        out->m[0][1] = m01;
        out->m[1][1] = m11;
        out->m[2][1] = m21;
        out->m[0][2] = m02;
        out->m[1][2] = m12;
        out->m[2][2] = m22;
    } else {
        out->m[0][0] = Dot(b->m[0][0], a->m[0][0], b->m[1][0], a->m[0][1], b->m[2][0], a->m[0][2]);
        out->m[1][0] = Dot(b->m[0][0], a->m[1][0], b->m[1][0], a->m[1][1], b->m[2][0], a->m[1][2]);
        out->m[2][0] = Dot(b->m[0][0], a->m[2][0], b->m[1][0], a->m[2][1], b->m[2][0], a->m[2][2]);
        out->m[0][1] = Dot(b->m[0][1], a->m[0][0], b->m[1][1], a->m[0][1], b->m[2][1], a->m[0][2]);
        out->m[1][1] = Dot(b->m[0][1], a->m[1][0], b->m[1][1], a->m[1][1], b->m[2][1], a->m[1][2]);
        out->m[2][1] = Dot(b->m[0][1], a->m[2][0], b->m[1][1], a->m[2][1], b->m[2][1], a->m[2][2]);
        out->m[0][2] = Dot(b->m[0][2], a->m[0][0], b->m[1][2], a->m[0][1], b->m[2][2], a->m[0][2]);
        out->m[1][2] = Dot(b->m[0][2], a->m[1][0], b->m[1][2], a->m[1][1], b->m[2][2], a->m[1][2]);
        out->m[2][2] = Dot(b->m[0][2], a->m[2][0], b->m[1][2], a->m[2][1], b->m[2][2], a->m[2][2]);
    }
}
#endif

#endif
