// Camera field of view (0x8026E870-0x80270AE8): per-mode FOV and distance tables, the widescreen
// FOV correction and the camera blend helpers ("physcam: %d != %d (tv = %d)"). Written against raw
// field offsets; the large routines and the initialiser are still stubs. Names are guesses apart from
// the strings.
#include "game/common.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

extern "C" {
int fn_80178CA4(void);
double tan(double x);
double atan(double x);
}

extern f32 lbl_8049A208[];
extern f32 lbl_8049A218[];

extern "C" {

void fn_8026E870(u8* p) {}  // stub (105)

int fn_8026EA14(int mode) {
    switch (mode) {
    case 4:
    case 5:
    case 6:
    case 8:
    case 23:
        return 1;
    }
    return 0;
}

f32 fn_8026EA44(int mode) {
    switch (mode) {
    case 6:
        return 3.7f;
    case 4:
    case 8:
        return 1.9f;
    case 7:
        return 1.0f;
    }
    return 1.9f;
}

void fn_8026EA8C(u8* p) {}  // stub (24)

f32 fn_8026EAEC(f32 fov) {
    if (fn_80178CA4() > 1) {
        fov = 2.0f * (f32)atan(0.4f * (f32)tan(0.5f * fov));
    }
    return fov;
}

void fn_8026EB4C(u8* p) {}  // stub (102)

void fn_8026ECE4(u8* p) {}  // stub (32)

void fn_8026ED64(u8* p) {}  // stub (121)

void fn_8026EF48(u8* p) {}  // stub (304)

int fn_8026F408(int n) { return n <= 8; }

void fn_8026F420(u8* p) {}  // stub (560)

f32 fn_8026FCE0(f32 t, f32 a) {
    f32 sq = t * t;
    return (1.0f - a) * t + a * sq;
}

void fn_8026FCFC(u8* a, u32 b, u8* c) {}  // stub (472)

void fn_8027045C(void) {
    lbl_8049A208[0] = 8.0f;
    lbl_8049A208[1] = 0.0f;
    lbl_8049A208[2] = 0.0f;
    lbl_8049A208[3] = 80.0f;
}

void fn_80270484(void) {
    f32 z = 0.0f;
    f32* q = lbl_8049A218;
    for (int i = 0; i < 4; i++) {
        q[0] = z;
        q[1] = z;
        q[3] = z;
        q[2] = z;
        q += 4;
    }
}

void fn_802704D0(u8* a, u32 b, u8* c) { fn_8026FCFC(a, b, c); }

void fn_802704D4(u8* a, u8* b) { fn_802704D0(a, U32(b, 0x4), b); }

void fn_802704E0(u8* p) { fn_8026EA8C(p); }

void fn_802704E4(u8* p) {}  // stub (360)

void fn_80270A84(u8* p) {}  // stub (25)
}  // extern "C"
