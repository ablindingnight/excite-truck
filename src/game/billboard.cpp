// Camera-facing billboard quads (0x802CE860-0x802CE9FC): emits the four corners of a quad along the
// camera's right/up axes, optionally mirrored. Names are guesses.
#include "game/common.h"

struct CamMtx {
    f32 m[12];
};

extern "C" {
CamMtx* fn_802C86DC(void);
void fn_801E4F20(int n);
void fn_801E4FF4(void);
int fn_801E4F3C(u32 color, int back, f32 x, f32 y, f32 z, f32 u, f32 v);
}

ModuleTag lbl_8055FCF8;

extern "C" {

void fn_802CE860(int n) { fn_801E4F20(n * 4); }

void fn_802CE868(void) { fn_801E4FF4(); }

// NonMatching: float register numbering in the corner loop.
// Helper names unproven: vector steps as the target evaluates them.
static inline void AddScaledV3(void* o_, const void* a_, f32 s) {
    f32* o = (f32*)o_;
    const f32* a = (const f32*)a_;
    o[0] += a[0] * s;
    o[1] += a[1] * s;
    o[2] += a[2] * s;
}
static inline void MadV3(void* o_, const void* a_, const void* b_, f32 s) {
    f32* o = (f32*)o_;
    const f32* a = (const f32*)a_;
    const f32* b = (const f32*)b_;
    o[0] = a[0] + b[0] * s;
    o[1] = a[1] + b[1] * s;
    o[2] = a[2] + b[2] * s;
}
void fn_802CE86C(u32 color, u32 flags, f32 size, f32 x, f32 y, f32 z) {
    ValVec3 pos;
    CamMtx* right;
    CamMtx* up;
    int i;
    right = fn_802C86DC();
    up = fn_802C86DC();
    f32 sign[3] = {1.0f, -1.0f, -1.0f};
    pos.x = x;
    pos.y = y;
    pos.z = z;
    for (i = 0; i < 4; i++) {
        f32 u = (i & 1) ? 1.0f : 0.0f;
        f32 v = (i & 2) ? 1.0f : 0.0f;
        f32 b;
        f32 a = size * ((2.0f * u - 1.0f) * sign[flags & 1]);
        b = size * ((1.0f + -2.0f * v) * sign[flags & 2]);
        ValVec3 p;
        MadV3(&p.x, &pos.x, &right->m[0], a);
        AddScaledV3(&p.x, &up->m[3], b);
        if (fn_801E4F3C(color, i >= 2, p.x, p.y, p.z, u, v)) {
            break;
        }
    }
}

}  // extern "C"
