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
void fn_802CE86C(u32 color, u32 flags, f32 size, f32 x, f32 y, f32 z) {
    ValVec3 pos;
    CamMtx* right;
    CamMtx* up;
    f32 sx;
    f32 sy;
    int i;
    right = fn_802C86DC();
    up = fn_802C86DC();
    f32 sign[3] = {1.0f, -1.0f, -1.0f};
    pos.x = x;
    pos.y = y;
    pos.z = z;
    sx = sign[flags & 1];
    sy = sign[flags & 2];
    for (i = 0; i < 4; i++) {
        f32 u = (i & 1) ? 1.0f : 0.0f;
        f32 v = (i & 2) ? 1.0f : 0.0f;
        f32 a = size * ((2.0f * u - 1.0f) * sx);
        f32 b = size * ((1.0f + -2.0f * v) * sy);
        ValVec3 p;
        p.x = pos.x + right->m[0] * a + up->m[3] * b;
        p.y = pos.y + right->m[1] * a + up->m[4] * b;
        p.z = pos.z + right->m[2] * a + up->m[5] * b;
        if (fn_801E4F3C(color, i >= 2, p.x, p.y, p.z, u, v)) {
            break;
        }
    }
}

}  // extern "C"
