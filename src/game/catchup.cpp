// Catch-up time solver (0x80138BE4-0x80139044): given two bodies moving along a line with constant
// acceleration (position, speed, acceleration each), solves the quadratic for when the second reaches
// the first, with fallbacks for equal accelerations, and records which case applied. Names are guesses.
#include "game/common.h"

extern "C" {
double sqrt(double x);
}

extern f32 lbl_80558DC4[];  // FLT_EPSILON (MSL)

struct Motion {
    f32 p1;  // offset 0x0
    f32 v1;  // offset 0x4
    f32 a1;  // offset 0x8
    f32 p2;  // offset 0xC
    f32 v2;  // offset 0x10
    f32 a2;  // offset 0x14

    f32 Dp(void) { return p2 - p1; }
    f32 Dv(void) { return v2 - v1; }
    f32 Da(void) { return a2 - a1; }
};

static inline f32 MaxF(f32 a, f32 b) { return a > b ? a : b; }
static inline f32 Diff(f32 a, f32 b) { return a - b; }
static inline f32 MinF(f32 a, f32 b) { return a < b ? a : b; }

ModuleTag lbl_8055E0F8;
s32 lbl_8055E100;  // solver case

extern "C" {

int fn_80138BE4(f32 a, f32 b) { return a * b < 0.0f; }

int fn_80138BFC(f32 x) { return (f32)__fabs(x) <= 10.0f * lbl_80558DC4[0]; }

static inline f32 AbsF(f32 v) { return (f32)__fabs(v); }

int fn_80138C28(Motion* m, f32* t, f32* miss) {
    f32 a;
    f32 b;
    f32 c;
    f32 disc;
    if (miss) {
        *miss = 0.0f;
    }
    a = (m->a2 - m->a1) / 2;
    b = m->v2 - m->v1;
    c = m->p2 - m->p1;
    if (!fn_80138BFC(a)) {
        disc = b * b - 4.0f * a * c;
        if (disc >= 0.0f) {
            f32 r[2];
            r[0] = (-b + (f32)sqrt(disc)) / (2.0f * a);
            r[1] = (-b - (f32)sqrt(disc)) / (2.0f * a);
            if (fn_80138BE4(r[0], r[1])) {
                *t = MaxF(r[0], r[1]);
            } else if (r[0] >= 0.0f) {
                *t = MinF(r[0], r[1]);
            } else {
                *t = -1.0f;
            }
            if (*t >= 0.0f) {
                return 1;
            }
        } else if (miss) {
            f32 x = -b > 0.0f ? -b : 0.0f;
            x = (x + b) * (2.0f * AbsF(a));
            *miss = (x * x - disc) / (2.0f * c);
        }
    }
    return 0;
}

f32 fn_80138DF4(Motion* m) {
    f32 r;
    if (!fn_80138BFC(m->Dp())) {
        if (!fn_80138BFC(m->Da())) {
            f32 t;
            f32 miss;
            if (fn_80138C28(m, &t, &miss)) {
                r = (m->v2 + t * m->a2 - m->v1) / MaxF(t, 0.01f);
                lbl_8055E100 = -5;
            } else {
                lbl_8055E100 = -4;
                r = miss;
            }
        } else {
            f32 dv = m->Dv();
            r = dv * dv / (2.0f * m->Dp());
            if (fn_80138BE4(m->Dv(), m->Dp())) {
                r = -r;
            }
            if (fn_80138BE4(m->Dv(), m->Dp())) {
                lbl_8055E100 = -3;
            } else {
                lbl_8055E100 = -2;
            }
        }
    } else {
        lbl_8055E100 = -1;
        r = 0.0f;
    }
    return r;
}

f32 fn_80138F38(Motion* m) {
    f32 t;
    if (m->v2 > 0.0f && m->a2 < 0.0f && fn_80138C28(m, &t, 0)) {
        f32 stop = -m->v2 / m->a2;
        if (stop <= t) {
            f32 r;
            Motion l = *m;
            l.p2 = m->p2 + stop * m->v2 + 0.5f * m->a2 * (stop * stop);
            l.a2 = 0.0f;
            l.v2 = 0.0f;
            r = fn_80138DF4(&l);
            lbl_8055E100 += 6;
            return r;
        }
    }
    return fn_80138DF4(m);
}

f32 fn_80139030(Motion* m) {
    lbl_8055E100 = -6;
    return fn_80138F38(m);
}

}  // extern "C"
