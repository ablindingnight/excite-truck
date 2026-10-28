// Camera states (0x80273B4C-0x80276068): the physics camera's per-mode state objects and their
// update routines.
// Written against raw field offsets; the large routines are still stubs. Names are guesses.
#include "game/common.h"
#include "game/camframe.h"
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
void fn_80276BE4(u8* a, u8* b);
void fn_802704D4(int a, u8* p);
}

// A camera pose: three rows of four words.
struct CamPose {
    u32 w[12];
};

ModuleTag lbl_8055F570;

extern "C" {

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

struct CVec3 {
    f32 x, y, z;
};

// Rotation + translation (camera frame).
struct CFrame {
    f32 m[9];  // offset 0x0, rows
    CVec3 t;   // offset 0x24
};

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

// The per-mode camera state (partial; the remaining routines still use raw offsets).
struct CamState {
    u8 pad0[0x28];
    CVec3 up;         // offset 0x28, smoothed up vector
    CVec3 upVel;      // offset 0x34
    u8 pad40[0xC];
    CVec3 look;       // offset 0x4C, look-at point
    CFrame frame;     // offset 0x58, camera rows + position
    u8 pad88[0x34];
    f32 fov;          // offset 0xBC
    CFrame carFrame;  // offset 0xC0, the followed car's frame
    u8 padF0[0x48];
    u8 snap;          // offset 0x138, jump instead of smoothing
    u8 pad139[7];
    u8* car;          // offset 0x140
};

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

void PSVECCrossProduct(const f32* a, const f32* b, f32* out);
void PSVECNormalize(const f32* in, f32* out);
f32 PSVECDotProduct(const f32* a, const f32* b);
int fn_801EA098(const s16* hint, f32* pos, f32* normal, int* flags, f32 x, f32 z);

double cos(double x);
extern const double lbl_80563FB8;  // ground tilt limit (radians)
extern const f32 lbl_80563FDC;

f32 fn_8018AE3C(u8* car);
f32 fn_8018AE44(u8* car);
f32 fn_8018AE4C(u8* car);
extern const f32 lbl_80563FC0;
extern const f32 lbl_80563FC4;
extern const f32 lbl_80563FC8;
extern const f32 lbl_80563FCC;
extern const f32 lbl_80563FD0;
extern const f32 lbl_80563FD4;
extern const f32 lbl_80563FD8;
extern const f32 lbl_80563FE0;

// Chase camera: follows the car's camera mount (the second one when the car is flipped), levels the
// up vector like fn_802749FC, and adds the car's bounce unless snapping.
void fn_80273BD0(u8* p) {
    CamState* c = (CamState*)p;
    Mat3 rot;
    Mat3 flip;
    CVec3 n;
    CVec3 pos;
    CsVec bounce;
    f32 euler[3];
    f32 flipEuler[3];
    CVec3 w;
    if (c->snap) {
        c->up.x = lbl_80563FB0;
        c->up.y = lbl_80563FAC;
        c->up.z = lbl_80563FB0;
        c->upVel.x = lbl_80563FB0;
        c->upVel.y = lbl_80563FB0;
        c->upVel.z = lbl_80563FB0;
    }
    c->frame = c->carFrame;
    u8* car = c->car;
    MulVec(&w, car[0x260C] ? (const CVec3*)(car + 0x5D0) : (const CVec3*)(car + 0x5C4), &c->frame);
    c->frame.t.x = c->frame.t.x + w.x;
    c->frame.t.y = c->frame.t.y + w.y;
    c->frame.t.z = c->frame.t.z + w.z;
    int flags = 0;
    static f32 cosLimit = (f32)cos(lbl_80563FB8);
    if (!fn_801EA098((s16*)(c->car + 0x6C), &pos.x, &n.x, &flags, c->carFrame.t.x, c->carFrame.t.z)) {
        n.x = c->carFrame.m[3];
        n.y = c->carFrame.m[4];
        n.z = c->carFrame.m[5];
    } else {
        f32 t = (PSVECDotProduct(&n.x, &c->carFrame.m[3]) - cosLimit) / (lbl_80563FAC - cosLimit);
        f32 s = lbl_80563FB0 > t ? lbl_80563FB0 : t;
        n.x = n.x * s;
        n.y = n.y * s;
        n.z = n.z * s;
        n.x = n.x + c->carFrame.m[3] * (lbl_80563FAC - s);
        n.y = n.y + c->carFrame.m[4] * (lbl_80563FAC - s);
        n.z = n.z + c->carFrame.m[5] * (lbl_80563FAC - s);
    }
    f32 k = c->snap ? lbl_80563FB0 : lbl_80563FC0;
    c->up.x = c->up.x * k;
    c->up.y = c->up.y * k;
    c->up.z = c->up.z * k;
    c->up.x = c->up.x + n.x * (lbl_80563FAC - k);
    c->up.y = c->up.y + n.y * (lbl_80563FAC - k);
    c->up.z = c->up.z + n.z * (lbl_80563FAC - k);
    c->frame.m[3] = c->up.x * lbl_80563FC4 + c->carFrame.m[3] * lbl_80563FC4;
    c->frame.m[4] = c->up.y * lbl_80563FC4 + c->carFrame.m[4] * lbl_80563FC4;
    c->frame.m[5] = c->up.z * lbl_80563FC4 + c->carFrame.m[5] * lbl_80563FC4;
    PSVECCrossProduct(&c->frame.m[3], &c->frame.m[6], &c->frame.m[0]);
    PSVECCrossProduct(&c->frame.m[0], &c->frame.m[3], &c->frame.m[6]);
    Orthonormalize(p);
    if (!c->snap) {
        c->upVel.x = c->upVel.x * lbl_80563FC8;
        c->upVel.y = c->upVel.y * lbl_80563FC8;
        c->upVel.z = c->upVel.z * lbl_80563FC8;
        bounce.Set(fn_8018AE3C(c->car), fn_8018AE44(c->car), fn_8018AE4C(c->car));
        c->upVel.x = c->upVel.x + bounce.x * lbl_80563FCC;
        c->upVel.y = c->upVel.y + bounce.y * lbl_80563FCC;
        c->upVel.z = c->upVel.z + bounce.z * lbl_80563FCC;
        f32 f = lbl_80563FD0 * (lbl_80563FD4 * fn_8018AE4C(c->car));
        c->frame.t.x = c->frame.t.x + c->carFrame.m[6] * f;
        c->frame.t.y = c->frame.t.y + c->carFrame.m[7] * f;
        c->frame.t.z = c->frame.t.z + c->carFrame.m[8] * f;
        f32 g = lbl_80563FD8 * (lbl_80563FD0 * (lbl_80563FD4 * (fn_8018AE44(c->car) - lbl_80563FDC * c->upVel.y)));
        c->frame.t.x = c->frame.t.x + c->carFrame.m[3] * g;
        c->frame.t.y = c->frame.t.y + c->carFrame.m[4] * g;
        c->frame.t.z = c->frame.t.z + c->carFrame.m[5] * g;
    }
    fn_80273B4C(p);
    euler[0] = *(f32*)(c->car + 0x5C0);
    euler[1] = lbl_80563FB0;
    euler[2] = lbl_80563FB0;
    Mat3FromEuler(&rot, (const Vec3*)euler);
    Mat3Mul((Mat3*)&c->frame, &rot, (Mat3*)&c->frame);
    if (c->car[0x260C]) {
        flipEuler[0] = lbl_80563FB0;
        flipEuler[1] = lbl_80563FE0;
        flipEuler[2] = lbl_80563FB0;
        Mat3FromEuler(&flip, (const Vec3*)flipEuler);
        Mat3Mul((Mat3*)&c->frame, &flip, (Mat3*)&c->frame);
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
    v.z = lbl_80563FEC + (F32(car, 0x2458) - lbl_80563FE4);
    v.y = lbl_80563FE8;
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

void PSVECCrossProduct(const f32* a, const f32* b, f32* out);
void PSVECNormalize(const f32* in, f32* out);
f32 PSVECDistance(const f32* a, const f32* b);
double tan(double x);
double atan(double x);
extern f32 lbl_80558DC4[];  // epsilon
extern const f32 lbl_80563FC4;
extern const f32 lbl_80563FD8;
extern const f32 lbl_80563FFC;  // fov limits
extern const f32 lbl_80564000;
extern const f32 lbl_80564004;
extern const f32 lbl_80564008;
extern const f32 lbl_8056400C;

// Aims the camera frame (0x58) from its position (0x7C) at the look point (0x4C), with world y as up
// (the car's forward axis when looking straight down). A positive `dist` pulls the camera toward that
// distance by `stiff`; the fov follows from `width` / `zoom` and the remaining distance unless `fov` is
// given.
void fn_80274730(u8* p, f32 fov, f32 dist, f32 zoom, f32 stiff) {
    f32 dx = F32(p, 0x4C) - F32(p, 0x7C);
    f32 dy = F32(p, 0x50) - F32(p, 0x80);
    f32 dz = F32(p, 0x54) - F32(p, 0x84);
    F32(p, 0x70) = dx;
    F32(p, 0x74) = dy;
    F32(p, 0x78) = dz;
    f32 eps = lbl_80558DC4[0];
    if ((f32)__fabs(dx) <= eps && (f32)__fabs(dz) <= eps) {
        F32(p, 0x70) = F32(p, 0xD8);
        F32(p, 0x78) = F32(p, 0xE0);
    }
    CsVec up(lbl_80563FB0, lbl_80563FAC, lbl_80563FB0);
    PSVECCrossProduct(&up.x, (f32*)(p + 0x70), (f32*)(p + 0x58));
    PSVECCrossProduct((f32*)(p + 0x70), (f32*)(p + 0x58), (f32*)(p + 0x64));
    Orthonormalize(p);
    f32 d = PSVECDistance((f32*)(p + 0x7C), (f32*)(p + 0x4C));
    if (dist > lbl_80563FB0) {
        f32 k = (lbl_80563FAC - stiff) * (d - dist);
        F32(p, 0x7C) = F32(p, 0x7C) + F32(p, 0x70) * k;
        F32(p, 0x80) = F32(p, 0x80) + F32(p, 0x74) * k;
        F32(p, 0x84) = F32(p, 0x84) + F32(p, 0x78) * k;
    }
    d = PSVECDistance((f32*)(p + 0x7C), (f32*)(p + 0x4C));
    f32 lo = lbl_80563FFC;
    f32 v = lbl_80564000;
    f32 half = lbl_80564004;
    f32 inv = zoom > lbl_80564008 ? lbl_80563FAC / zoom : lbl_80563FAC;
    f32 w = lbl_8056400C * inv;
    f32 a = lbl_80563FD8 * (f32)atan(w / d * (f32)tan(half * lbl_80563FC4));
    v = v < a ? v : a;
    v = v > lo ? v : lo;
    if (fov > lbl_80563FB0) {
        F32(p, 0xBC) = fov;
        F32(p, 0x194) = fov;
    } else {
        f32 prev = F32(p, 0x194);
        f32 f = prev + lbl_80563FAC * (v - prev);
        F32(p, 0xBC) = f;
        F32(p, 0x194) = f;
    }
    F32(p, 0x120) = lbl_80563FB0;
    F32(p, 0x124) = lbl_80563FB0;
    F32(p, 0x128) = lbl_80563FB0;
}


// Moves the camera by `move` in the car frame, then levels its up vector (0x28) toward the ground
// normal below the target (or the car's up axis), and rebuilds the frame around it.
void fn_802749FC(u8* p, const CVec3* move) {
    CamState* c = (CamState*)p;
    if (c->snap) {
        c->up.x = lbl_80563FB0;
        c->up.y = lbl_80563FAC;
        c->up.z = lbl_80563FB0;
        c->upVel.x = lbl_80563FB0;
        c->upVel.y = lbl_80563FB0;
        c->upVel.z = lbl_80563FB0;
    }
    CVec3 n;
    CVec3 pos;
    CVec3 w;
    c->frame = c->carFrame;
    MulVec(&w, move, &c->frame);
    c->frame.t.x = c->frame.t.x + w.x;
    c->frame.t.y = c->frame.t.y + w.y;
    c->frame.t.z = c->frame.t.z + w.z;
    int flags = 0;
    static f32 cosLimit = (f32)cos(lbl_80563FB8);
    if (!fn_801EA098((s16*)(c->car + 0x6C), &pos.x, &n.x, &flags, c->carFrame.t.x, c->carFrame.t.z)) {
        n.x = c->carFrame.m[3];
        n.y = c->carFrame.m[4];
        n.z = c->carFrame.m[5];
    } else {
        f32 t = (PSVECDotProduct(&n.x, &c->carFrame.m[3]) - cosLimit) / (lbl_80563FAC - cosLimit);
        f32 s = lbl_80563FB0 > t ? lbl_80563FB0 : t;
        n.x = n.x * s;
        n.y = n.y * s;
        n.z = n.z * s;
        n.x = n.x + c->carFrame.m[3] * (lbl_80563FAC - s);
        n.y = n.y + c->carFrame.m[4] * (lbl_80563FAC - s);
        n.z = n.z + c->carFrame.m[5] * (lbl_80563FAC - s);
    }
    f32 k = c->snap ? lbl_80563FB0 : lbl_80563FDC;
    c->up.x = c->up.x * k;
    c->up.y = c->up.y * k;
    c->up.z = c->up.z * k;
    c->up.x = c->up.x + n.x * (lbl_80563FAC - k);
    c->up.y = c->up.y + n.y * (lbl_80563FAC - k);
    c->up.z = c->up.z + n.z * (lbl_80563FAC - k);
    c->frame.m[3] = c->up.x * lbl_80563FC4 + c->carFrame.m[3] * lbl_80563FC4;
    c->frame.m[4] = c->up.y * lbl_80563FC4 + c->carFrame.m[4] * lbl_80563FC4;
    c->frame.m[5] = c->up.z * lbl_80563FC4 + c->carFrame.m[5] * lbl_80563FC4;
    PSVECCrossProduct(&c->frame.m[3], &c->frame.m[6], &c->frame.m[0]);
    PSVECCrossProduct(&c->frame.m[0], &c->frame.m[3], &c->frame.m[6]);
    Orthonormalize(p);
}

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

struct CamVec {
    CamVec(void) {}
    CamVec(f32 a, f32 b, f32 c) : x(a), y(b), z(c) {}
    void Set(f32 a, f32 b, f32 c) {
        x = a;
        y = b;
        z = c;
    }
    f32 x, y, z;
};

static inline CamVec operator*(const CVec3& v, f32 k) { return CamVec(k * v.x, k * v.y, k * v.z); }
static inline CamVec operator+(CVec3 a, const CamVec& b) { return CamVec(a.x + b.x, a.y + b.y, a.z + b.z); }

struct CamPreset {
    CVec3 off;
    CVec3 rot;
    f32 fov;
};

extern const CamPreset lbl_802FA788;
extern const CamPreset lbl_802FA7A4;
extern const CamPreset lbl_802FA7C0;
extern const CamPreset lbl_802FA7DC;
extern const CamPreset lbl_802FA7F8;
extern const f32 lbl_80564024;  // degrees to radians
extern const f32 lbl_80564028;
extern const f32 lbl_8056402C;
extern const f32 lbl_80564030;
extern const f32 lbl_80563FD8;
extern const f32 lbl_80563FDC;
void fn_801BFD34(u8* out, const CamVec* v, const u8* frame);

// Rotation from the rotation vector w (axis * angle), Rodrigues' formula; identity for a tiny angle.
static inline void RotFromVec(Mat3* r, const CamVec& w) {
    f32 a = PSVECMag(&w.x);
    if (a > lbl_80564020) {
        f32 x, s, y, z, c, k;
        f32 inv = lbl_80563FAC / a;
        x = inv * w.x;
        y = inv * w.y;
        z = inv * w.z;
        s = (f32)sin(a);
        c = (f32)cos(a);
        k = lbl_80563FAC - c;
        r->m[0] = c + k * (x * x);
        r->m[1] = y * (k * x) + s * z;
        r->m[2] = z * (k * x) - s * y;
        r->m[3] = y * (k * x) - s * z;
        r->m[4] = c + k * (y * y);
        r->m[5] = z * (k * y) + s * x;
        r->m[6] = z * (k * x) + s * y;
        r->m[7] = z * (k * y) - s * x;
        r->m[8] = c + k * (z * z);
    } else {
        r->m[1] = lbl_80563FB0;
        r->m[0] = lbl_80563FAC;
        r->m[2] = lbl_80563FB0;
        r->m[3] = lbl_80563FB0;
        r->m[4] = lbl_80563FAC;
        r->m[5] = lbl_80563FB0;
        r->m[6] = lbl_80563FB0;
        r->m[7] = lbl_80563FB0;
        r->m[8] = lbl_80563FAC;
    }
}

// Rotation (0x58, 3x3) from the rotation vector w (axis * angle), Rodrigues' formula; identity for a
// tiny angle. t goes to 0xBC.
void fn_80274F74(u8* p, const f32* w, f32 t) {
    RotFromVec((Mat3*)(p + 0x58), *(const CamVec*)w);
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

// A fixed camera placement: offset from the car frame, rotation vector in degrees, field of view.
// Camera frame = the car frame (0xC0) turned by the preset rotation and moved to its offset.
void fn_802751AC(u8* p) {
    CamPreset c = lbl_802FA788;
    *(CFrame*)(p + 0x58) = *(CFrame*)(p + 0xC0);
    Mat3 rot;
    CamVec off(c.off.x, c.off.y, c.off.z);
    RotFromVec(&rot, c.rot * lbl_80564024);
    fn_801BFD34(p + 0x7C, &off, p + 0xC0);
    Mat3Mul((Mat3*)(p + 0x58), &rot, (Mat3*)(p + 0xC0));
    F32(p, 0xBC) = lbl_80564024 * c.fov;
}

// As fn_802751AC, but the offset follows the car's first point (0x2450) instead of the preset's.
void fn_80275410(u8* p) {
    u8* car = PTR(p, 0x140);
    CamPreset c = lbl_802FA7A4;
    CamVec off = *(CVec3*)(car + 0x2450) + CamVec(lbl_80563FE8, lbl_80564028, lbl_8056402C);
    *(CFrame*)(p + 0x58) = *(CFrame*)(p + 0xC0);
    Mat3 rot;
    RotFromVec(&rot, c.rot * lbl_80564024);
    fn_801BFD34(p + 0x7C, &off, p + 0xC0);
    Mat3Mul((Mat3*)(p + 0x58), &rot, (Mat3*)(p + 0xC0));
    F32(p, 0xBC) = lbl_80564024 * c.fov;
}

// As fn_802751AC, but the offset follows the car's second point (0x2538) instead of the preset's.
void fn_802756A4(u8* p) {
    u8* car = PTR(p, 0x140);
    CamPreset c = lbl_802FA7C0;
    CamVec off = *(CVec3*)(car + 0x2538) + CamVec(lbl_80564030, lbl_80563FDC, lbl_80563FD8);
    *(CFrame*)(p + 0x58) = *(CFrame*)(p + 0xC0);
    Mat3 rot;
    RotFromVec(&rot, c.rot * lbl_80564024);
    fn_801BFD34(p + 0x7C, &off, p + 0xC0);
    Mat3Mul((Mat3*)(p + 0x58), &rot, (Mat3*)(p + 0xC0));
    F32(p, 0xBC) = lbl_80564024 * c.fov;
}

void fn_80275938(u8* p) {
    CamPreset c = lbl_802FA7DC;
    *(CFrame*)(p + 0x58) = *(CFrame*)(p + 0xC0);
    Mat3 rot;
    CamVec off(c.off.x, c.off.y, c.off.z);
    RotFromVec(&rot, c.rot * lbl_80564024);
    fn_801BFD34(p + 0x7C, &off, p + 0xC0);
    Mat3Mul((Mat3*)(p + 0x58), &rot, (Mat3*)(p + 0xC0));
    F32(p, 0xBC) = lbl_80564024 * c.fov;
}

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


// Orbits the camera around its target at `radius`, keeping it above the ground.
static inline f32 CsLerp(f32 a, f32 b, f32 k) { return a + k * (b - a); }

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
        CsVec t(CsLerp(pos.x, F32(p, 0xE4), lbl_80563FC4), CsLerp(pos.y, F32(p, 0xE8), lbl_80563FC4),
                CsLerp(pos.z, F32(p, 0xEC), lbl_80563FC4));
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

void PSVECCrossProduct(const f32* a, const f32* b, f32* out);

// As fn_802751AC, but relative to a level frame at the target: forward = the car's forward axis,
// up = world y made perpendicular.
void fn_80275D0C(u8* p) {
    CamPreset c = lbl_802FA7F8;
    *(CFrame*)(p + 0x58) = *(CFrame*)(p + 0xC0);
    CFrame f;
    f.t.x = F32(p, 0xE4);
    f.t.y = F32(p, 0xE8);
    f.t.z = F32(p, 0xEC);
    f.m[6] = F32(p, 0xD8);
    f.m[7] = F32(p, 0xDC);
    f.m[8] = F32(p, 0xE0);
    f.m[3] = lbl_80563FB0;
    f.m[4] = lbl_80563FAC;
    f.m[5] = lbl_80563FB0;
    PSVECCrossProduct(&f.m[3], &f.m[6], &f.m[0]);
    PSVECCrossProduct(&f.m[6], &f.m[0], &f.m[3]);
    Mat3 rot;
    CamVec off(c.off.x, c.off.y, c.off.z);
    RotFromVec(&rot, c.rot * lbl_80564024);
    fn_801BFD34(p + 0x7C, &off, (u8*)&f);
    Mat3Mul((Mat3*)(p + 0x58), &rot, (Mat3*)&f);
    F32(p, 0xBC) = lbl_80564024 * c.fov;
}

void fn_80275FC0(u8* p) {
    *(CamPose*)(p + 0x58) = *(CamPose*)(p + 0x1A4);
    F32(p, 0xBC) = F32(p, 0x1D4);
}

void fn_80276044(u8* p) { fn_80276BE4(p + 0x58, p + 0xBC); }

void fn_80276054(u8* p) { fn_802704D4(4, p); }

}  // extern "C"
