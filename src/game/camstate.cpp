// Camera states (0x80273A88-0x80276068): the physics camera's per-mode state objects and their
// update routines, plus the array destructors registered by the camera definitions' initialiser.
// Written against raw field offsets; the large routines are still stubs. Names are guesses.
#include "game/common.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

struct Mat3 {
    f32 m[9];
};

void Mat3Mul(Mat3* out, const Mat3* a, const Mat3* b);

extern const f32 lbl_80563FA8;
extern const f32 lbl_80563FB4;  // pi/4
extern const f32 lbl_80563FAC;
extern const f32 lbl_80563FB0;
extern const f32 lbl_80563FF0;
extern const f32 lbl_80563FF4;
extern const f32 lbl_80563FF8;
extern const f32 lbl_80564010;
extern const f32 lbl_80564014;
extern const f32 lbl_80564018;
extern const f32 lbl_8056401C;

extern "C" {
f32 PSVECMag(const f32* v);
void fn_80015A20(Mat3* out, const f32* euler);  // Y, then X, then Z
void __destroy_arr(void* p, void* dtor, u32 size, u32 n);
u8* fn_80273978(u8* p, int del);
u8* fn_80273B08(u8* p, int del);
void fn_80272354(u8* p);
void fn_80276BE4(u8* a, u8* b);
void fn_802704D4(int a, u8* p);
}

void operator delete(void* p);

extern u8 lbl_8049F9D8[];
extern u8 lbl_8049A2C8[];
extern void* lbl_80347364[];

// A camera pose: three rows of four words.
struct CamPose {
    u32 w[12];
};

ModuleTag lbl_8055F570;

extern "C" {

void fn_80273A88(void) { __destroy_arr(lbl_8049F9D8, (void*)fn_80273978, 0x1C, 0x20); }

u8* fn_80273AA4(u8* p) {
    *(void**)(p + 0x18) = lbl_80347364;
    U32(p, 0x0) = 0;
    return p;
}

u8* fn_80273ABC(u8* p) {
    fn_80272354(p);
    return p;
}

void fn_80273AEC(void) { __destroy_arr(lbl_8049A2C8, (void*)fn_80273B08, 0x60, 0xC8); }

u8* fn_80273B08(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_80273B48(u8* p) {}

void fn_80273B4C(u8* p) {
    u8* car = PTR(p, 0x140);
    if (car) {
        f32 euler[3];
        f32 side = U8(car, 0x260C) ? lbl_80563FA8 : lbl_80563FAC;
        f32 zero = lbl_80563FB0;
        f32 ang = zero * lbl_80563FB4;
        euler[0] = zero;
        euler[1] = ang * side;
        euler[2] = zero;
        Mat3 rot;
        fn_80015A20(&rot, euler);
        Mat3Mul((Mat3*)(p + 0x58), &rot, (Mat3*)(p + 0x58));
    }
}

void fn_80273BD0(u8* p) {}  // stub (422)

void fn_80274268(u8* p) {}  // stub (125)

void fn_8027445C(u8* p) { fn_80274268(p); }

void fn_80274460(u8* p) {}  // stub (128)

void fn_80274660(u8* unused, u8* p) {
    f32 zero = lbl_80563FB0;
    f32 one = lbl_80563FAC;
    F32(p, 0x5C) = zero;
    F32(p, 0x58) = one;
    F32(p, 0x60) = zero;
    F32(p, 0x64) = zero;
    F32(p, 0x68) = one;
    F32(p, 0x6C) = zero;
    F32(p, 0x70) = zero;
    F32(p, 0x74) = zero;
    F32(p, 0x78) = one;
    F32(p, 0x7C) = zero;
    F32(p, 0x80) = zero;
    F32(p, 0x84) = zero;
    f32 axis[3];
    axis[0] = lbl_80563FF0;
    axis[1] = zero;
    axis[2] = zero;
    Mat3 rot;
    fn_80015A20(&rot, axis);
    Mat3Mul((Mat3*)(p + 0x58), &rot, (Mat3*)(p + 0x58));
    F32(p, 0x7C) = F32(p, 0xE4);
    F32(p, 0x80) = F32(p, 0xE8);
    F32(p, 0x84) = F32(p, 0xEC);
    f32 h;
    if (S32(p, 0x10) == 10) {
        h = lbl_80563FF4;
    } else {
        h = lbl_80563FF8;
    }
    F32(p, 0x80) += h;
    F32(p, 0x124) = lbl_80563FB0;
}

void fn_80274730(u8* p, f32 a, f32 b, f32 c, f32 d) {}  // stub (179)

void fn_802749FC(u8* p) {}  // stub (294)

void fn_80274E94(u8* p) { fn_80274730(p, -1.0f, -1.0f, -1.0f, 0.0f); }

void fn_80274EA8(u8* p) {
    f32 d[3];
    d[0] = F32(p, 0x7C) - F32(p, 0xE4);
    d[1] = F32(p, 0x80) - F32(p, 0xE8);
    d[2] = F32(p, 0x84) - F32(p, 0xEC);
    f32 m = PSVECMag(d);
    if (m > lbl_80564010) {
        f32 s = -(m - lbl_80564010) / m;
        F32(p, 0x7C) = F32(p, 0x7C) + d[0] * s;
        F32(p, 0x80) = F32(p, 0x80) + d[1] * s;
        F32(p, 0x84) = F32(p, 0x84) + d[2] * s;
    }
    fn_80274730(p, lbl_80563FA8, lbl_80564014, lbl_80564018, lbl_8056401C);
}

void fn_80274F74(u8* p) {}  // stub (84)

void fn_802750C4(u8* p) {}  // stub (58)

void fn_802751AC(u8* p) {}  // stub (153)

void fn_80275410(u8* p) {}  // stub (165)

void fn_802756A4(u8* p) {}  // stub (165)

void fn_80275938(u8* p) {}  // stub (153)

void fn_80275B9C(u8* p) {}  // stub (92)

void fn_80275D0C(u8* p) {}  // stub (173)

void fn_80275FC0(u8* p) {
    *(CamPose*)(p + 0x58) = *(CamPose*)(p + 0x1A4);
    F32(p, 0xBC) = F32(p, 0x1D4);
}

void fn_80276044(u8* p) { fn_80276BE4(p + 0x58, p + 0xBC); }

void fn_80276054(u8* p) { fn_802704D4(4, p); }

}  // extern "C"
