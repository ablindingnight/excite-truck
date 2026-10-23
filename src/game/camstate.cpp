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

struct Vec3;
void Mat3FromEuler(Mat3* out, const Vec3* euler);  // Y, then X, then Z (racing.cpp)

extern "C" {
f32 PSVECMag(const f32* v);
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
        Mat3FromEuler(&rot, (const Vec3*)euler);
        Mat3Mul((Mat3*)(p + 0x58), &rot, (Mat3*)(p + 0x58));
    }
}

void fn_80273BD0(u8* p) {}  // stub (422)

struct CVec3 {
    f32 x, y, z;
};

// Rotation + translation (camera frame).
struct CFrame {
    f32 m[9];  // offset 0x0, rows
    CVec3 t;   // offset 0x24
};

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

static inline void MulVec(CVec3* out, const CVec3* in, const CFrame* m) {
    if (out == in) {
        f32 x = Dot3(in->x, m->m[0], in->y, m->m[3], in->z, m->m[6]);
        f32 y = Dot3(in->x, m->m[1], in->y, m->m[4], in->z, m->m[7]);
        f32 z = Dot3(in->x, m->m[2], in->y, m->m[5], in->z, m->m[8]);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, m->m[0], in->y, m->m[3], in->z, m->m[6]);
        out->y = Dot3(in->x, m->m[1], in->y, m->m[4], in->z, m->m[7]);
        out->z = Dot3(in->x, m->m[2], in->y, m->m[5], in->z, m->m[8]);
    }
}

void fn_80274460(u8* p);
extern const f32 lbl_80563FE4;
extern const f32 lbl_80563FE8;
extern const f32 lbl_80563FEC;

// Camera frame (0x58) = the car-relative frame (0xC0), moved by a lift that follows the car's 0x2458 value;
// flag 0x260C on the car takes fn_80274460 instead.
void fn_80274268(u8* p) {
    u8* car = PTR(p, 0x140);
    if (U8(car, 0x260C)) {
        fn_80274460(p);
        return;
    }
    CVec3 v;
    CVec3 w;
    *(CFrame*)(p + 0x58) = *(CFrame*)(p + 0xC0);
    v.x = lbl_80563FB0;
    v.y = lbl_80563FE8;
    v.z = lbl_80563FEC + (F32(car, 0x2458) - lbl_80563FE4);
    MulVec(&w, &v, (CFrame*)(p + 0x58));
    F32(p, 0x7C) = F32(p, 0x7C) + w.x;
    F32(p, 0x80) = F32(p, 0x80) + w.y;
    F32(p, 0x84) = F32(p, 0x84) + w.z;
    fn_80273B4C(p);
}

void fn_8027445C(u8* p) { fn_80274268(p); }

extern const f32 lbl_80563FE0;

// Camera frame (0x58) = the car-relative frame (0xC0) turned about y by lbl_80563FE0 and lifted.
void fn_80274460(u8* p) {
    Mat3 rot;
    f32 euler[3];
    CVec3 v;
    CVec3 w;
    *(CFrame*)(p + 0x58) = *(CFrame*)(p + 0xC0);
    euler[0] = lbl_80563FB0;
    euler[1] = lbl_80563FE0;
    euler[2] = lbl_80563FB0;
    Mat3FromEuler(&rot, (const Vec3*)euler);
    Mat3Mul((Mat3*)(p + 0x58), &rot, (Mat3*)(p + 0x58));
    v.x = lbl_80563FB0;
    v.y = lbl_80563FE8;
    v.z = lbl_80563FB0;
    MulVec(&w, &v, (CFrame*)(p + 0x58));
    F32(p, 0x7C) = F32(p, 0x7C) + w.x;
    F32(p, 0x80) = F32(p, 0x80) + w.y;
    F32(p, 0x84) = F32(p, 0x84) + w.z;
}

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
    Mat3FromEuler(&rot, (const Vec3*)axis);
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

extern const f32 lbl_80564020;  // small angle
double sin(double x);
double cos(double x);

// Rotation (0x58, 3x3) from the rotation vector w (axis * angle), Rodrigues' formula; identity for a
// tiny angle. t goes to 0xBC.
void fn_80274F74(u8* p, const f32* w, f32 t) {
    f32 a = PSVECMag(w);
    if (a > lbl_80564020) {
        f32 x, s, y, z, c, k;
        f32 inv = lbl_80563FAC / a;
        x = inv * w[0];
        y = inv * w[1];
        z = inv * w[2];
        s = (f32)sin(a);
        c = (f32)cos(a);
        k = lbl_80563FAC - c;
        F32(p, 0x58) = c + k * (x * x);
        F32(p, 0x5C) = y * (k * x) + s * z;
        F32(p, 0x60) = z * (k * x) - s * y;
        F32(p, 0x64) = y * (k * x) - s * z;
        F32(p, 0x68) = c + k * (y * y);
        F32(p, 0x6C) = z * (k * y) + s * x;
        F32(p, 0x70) = z * (k * x) + s * y;
        F32(p, 0x74) = z * (k * y) - s * x;
        F32(p, 0x78) = c + k * (z * z);
    } else {
        F32(p, 0x5C) = lbl_80563FB0;
        F32(p, 0x58) = lbl_80563FAC;
        F32(p, 0x60) = lbl_80563FB0;
        F32(p, 0x64) = lbl_80563FB0;
        F32(p, 0x68) = lbl_80563FAC;
        F32(p, 0x6C) = lbl_80563FB0;
        F32(p, 0x70) = lbl_80563FB0;
        F32(p, 0x74) = lbl_80563FB0;
        F32(p, 0x78) = lbl_80563FAC;
    }
    F32(p, 0xBC) = t;
}

void fn_8026E850(f32* pos, const f32* target, u8* cam, const f32* up);
f32 PSVECDotProduct(const f32* a, const f32* b);

// Re-aims the camera, then slides it along its view axis toward `dist` from the target.
void fn_802750C4(u8* p, f32 k, f32 dist) {
    f32 d[3];
    f32 up[3];
    up[0] = lbl_80563FB0;
    up[1] = lbl_80563FAC;
    up[2] = lbl_80563FB0;
    fn_8026E850((f32*)(p + 0x7C), (f32*)(p + 0xE4), p, up);
    d[0] = F32(p, 0xE4) - F32(p, 0x7C);
    d[1] = F32(p, 0xE8) - F32(p, 0x80);
    d[2] = F32(p, 0xEC) - F32(p, 0x84);
    f32 e = PSVECDotProduct((f32*)(p + 0x70), d) - dist;
    f32 t = e * k;
    f32 x = F32(p, 0x7C) + F32(p, 0x70) * t;
    f32 y = F32(p, 0x80) + F32(p, 0x74) * t;
    f32 z = F32(p, 0x84) + F32(p, 0x78) * t;
    F32(p, 0x7C) = x;
    F32(p, 0x80) = y;
    F32(p, 0x84) = z;
}

void fn_802751AC(u8* p) {}  // stub (153)

void fn_80275410(u8* p) {}  // stub (165)

void fn_802756A4(u8* p) {}  // stub (165)

void fn_80275938(u8* p) {}  // stub (153)

double fmod(double x, double y);
double sin(double x);
double cos(double x);
int fn_801E9E30(u8* ground, f32* pos);
extern f32 lbl_8055E3C8;         // game time
extern f32 lbl_8055F588;         // last ground height
extern const double lbl_80564038;  // 2 pi
extern const f32 lbl_80563FD8;
extern const f32 lbl_80563FC4;
extern const f32 lbl_80564040;

struct CsVec {
    CsVec(void) {}
    CsVec(f32 a, f32 b, f32 c) : x(a), y(b), z(c) {}
    f32 x, y, z;
};

// Orbits the camera around its target at `radius`, keeping it above the ground.
void fn_80275B9C(u8* p, f32 radius, f32 height, f32 speed) {
    F32(p, 0x15C) = fmod(speed * lbl_8055E3C8, lbl_80564038);
    f32 s = radius * (f32)sin(F32(p, 0x15C));
    f32 c = radius * (f32)cos(F32(p, 0x15C));
    CsVec pos;
    CsVec off;
    off.x = c;
    off.y = lbl_80563FB0;
    off.z = s;
    pos.x = F32(p, 0xE4) + off.x;
    pos.y = F32(p, 0xE8) + off.y;
    pos.z = F32(p, 0xEC) + off.z;
    if (!fn_801E9E30(p + 0x1EC, &pos.x)) {
        pos.y = lbl_80563FD8 + lbl_8055F588;
        CsVec t(pos.x + lbl_80563FC4 * (F32(p, 0xE4) - pos.x), pos.y + lbl_80563FC4 * (F32(p, 0xE8) - pos.y),
                pos.z + lbl_80563FC4 * (F32(p, 0xEC) - pos.z));
        pos = t;
    } else {
        lbl_8055F588 = pos.y;
    }
    pos.y += height;
    f32 up[3];
    up[0] = lbl_80563FB0;
    up[1] = lbl_80563FAC;
    up[2] = lbl_80563FB0;
    fn_8026E850(&pos.x, (f32*)(p + 0xE4), p, up);
    F32(p, 0xBC) = lbl_80564040;
}

void fn_80275D0C(u8* p) {}  // stub (173)

void fn_80275FC0(u8* p) {
    *(CamPose*)(p + 0x58) = *(CamPose*)(p + 0x1A4);
    F32(p, 0xBC) = F32(p, 0x1D4);
}

void fn_80276044(u8* p) { fn_80276BE4(p + 0x58, p + 0xBC); }

void fn_80276054(u8* p) { fn_802704D4(4, p); }

}  // extern "C"
