// Quaternion slerp (0x8012F57C-0x8012F7E0). Names are guesses.
#include "game/common.h"

extern "C" {
double sqrt(double x);
double atan2(double y, double x);
double sin(double x);
}

extern f32 lbl_80558DC4[];  // FLT_EPSILON (MSL)

struct Quat {
    f32 x, y, z, w;
};

static inline void QuatCopy(Quat* d, const Quat* s) {
    f32 x = s->x;
    f32 y = s->y;
    f32 z = s->z;
    f32 w = s->w;
    d->x = x;
    d->y = y;
    d->z = z;
    d->w = w;
}

static inline void QuatScale(Quat* d, const Quat* s, f32 k) {
    d->x = s->x * k;
    d->y = s->y * k;
    d->z = s->z * k;
    d->w = s->w * k;
}

static inline void QuatAdd(Quat* d, const Quat* a, const Quat* b) {
    d->x = a->x + b->x;
    d->y = a->y + b->y;
    d->z = a->z + b->z;
    d->w = a->w + b->w;
}

static inline f32 fabsf(f32 x) { return __fabs(x); }
static inline f32 sqrtf(f32 x) { return sqrt(x); }
static inline f32 sinf(f32 x) { return sin(x); }
static inline f32 atan2f(f32 y, f32 x) { return atan2(y, x); }

ModuleTag lbl_8055E028;

extern "C" void fn_8012F57C(Quat* out, Quat* a, Quat* b, f32 t) {
    f32 cosom = a->w * b->w + a->x * b->x + a->y * b->y + a->z * b->z;
    f32 sinom;
    if (cosom < 0.0f) {
        cosom = -cosom;
        a->x *= -1.0f;
        a->y *= -1.0f;
        a->z *= -1.0f;
        a->w *= -1.0f;
    }
    sinom = sqrtf(fabsf(1.0f - cosom * cosom));
    if (fabsf(sinom) < lbl_80558DC4[0]) {
        QuatCopy(out, a);
    } else {
        f32 omega = atan2f(sinom, cosom);
        f32 inv = 1.0f / sinom;
        f32 s0 = inv * sinf((1.0f - t) * omega);
        f32 s1 = inv * sinf(t * omega);
        f32 len;
        Quat t0;
        Quat t1;
        QuatScale(&t0, a, s0);
        QuatScale(&t1, b, s1);
        QuatAdd(out, &t0, &t1);
        len = out->w * out->w + out->z * out->z + out->y * out->y + out->x * out->x;
        if (!(fabsf(len) <= lbl_80558DC4[0])) {
            f32 s = 1.0f / sqrtf(len);
            out->x *= s;
            out->y *= s;
            out->z *= s;
            out->w *= s;
        }
    }
}
