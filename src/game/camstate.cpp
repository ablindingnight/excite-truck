// Camera states (0x80273B4C-0x80276068): the physics camera's per-mode state objects and their
// update routines.
// Written against raw field offsets; the large routines are still stubs. Names are guesses.
#include "game/common.h"
#include "game/carbody.h"
#include "game/rvec.h"
#include <revolution/MTX/vec.h>
#include "game/camframe.h"
#include "game/camrec.h"
#include "game/math_types.h"
#include "game/plainvec.h"
#include <string.h>
#include "game/mathutil.h"
// Helpers unproven (names and existence): the forms that keep retail's operand order and unfolded identities where a
// plain literal does not.
static inline void CsAddScaled(f32* o, const f32* a, const f32* d, f32 s) { o[0] = a[0] + d[0] * s; o[1] = a[1] + d[1] * s; o[2] = a[2] + d[2] * s; }

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))


extern "C" {
void fn_80276BE4(u8* a, u8* b);
void fn_802704D4(int a, u8* p);
}

// A camera pose copied as twelve words.
struct PoseWords {
    u32 w[12];
};

ModuleTag lbl_8055F570;

extern "C" {

void fn_80273B4C(u8* p) {
    CamRec* cr = (CamRec*)p;
    u8* car = cr->car;
    if (car) {
        f32 euler[3];
        f32 side = U8(car, 0x260C) ? -1.0f : 1.0f;
        f32 zero = 0.0f;
        f32 ang = Mul(zero, 0.7853982f);
        euler[0] = zero;
        euler[1] = ang * side;
        euler[2] = zero;
        Mat3 rot;
        Mat3FromEuler(&rot, (const Vec3*)euler);
        Mat3Mul((Mat3*)&cr->pose, &rot, (Mat3*)&cr->pose);
    }
}

// CFrame: common.h's PlainValMatrix.

static inline void MulVec(CVec3* out, const CVec3* in, const PlainValMatrix* m) {
    if (out == in) {
        f32 x = Dot3(in->x, m->basis[0].x, in->y, m->basis[1].x, in->z, m->basis[2].x);
        f32 y = Dot3(in->x, m->basis[0].y, in->y, m->basis[1].y, in->z, m->basis[2].y);
        f32 z = Dot3(in->x, m->basis[0].z, in->y, m->basis[1].z, in->z, m->basis[2].z);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, m->basis[0].x, in->y, m->basis[1].x, in->z, m->basis[2].x);
        out->y = Dot3(in->x, m->basis[0].y, in->y, m->basis[1].y, in->z, m->basis[2].y);
        out->z = Dot3(in->x, m->basis[0].z, in->y, m->basis[1].z, in->z, m->basis[2].z);
    }
}

int fn_801EA098(const s16* hint, f32* pos, f32* normal, int* flags, f32 x, f32 z);

double cos(double x);

f32 fn_8018AE3C(u8* car);
f32 fn_8018AE44(u8* car);
f32 fn_8018AE4C(u8* car);

// Chase camera: follows the car's camera mount (the second one when the car is flipped), levels the
// up vector like fn_802749FC, and adds the car's bounce unless snapping.
// Helper names unproven: the frame row is a scale then an added scale, as in the target.
static inline void ScV(f32* o, const f32* a, f32 s) {
    o[0] = a[0] * s;
    o[1] = a[1] * s;
    o[2] = a[2] * s;
}
static inline void AddSV(f32* o, const f32* a, f32 s) {
    o[0] += a[0] * s;
    o[1] += a[1] * s;
    o[2] += a[2] * s;
}
void fn_80273BD0(u8* p) {
    CamRec* c = (CamRec*)p;
    Mat3 rot;
    Mat3 flip;
    CVec3 n;
    CVec3 pos;
    CsVec bounce;
    f32 euler[3];
    f32 flipEuler[3];
    CVec3 w;
    if (c->b138) {
        c->v28.v[0] = 0.0f;
        c->v28.v[1] = 1.0f;
        c->v28.v[2] = 0.0f;
        c->v34.v[0] = 0.0f;
        c->v34.v[1] = 0.0f;
        c->v34.v[2] = 0.0f;
    }
    c->pose = c->carFrame;
    u8* car = c->car;
    MulVec(&w, car[0x260C] ? (const CVec3*)(car + 0x5D0) : (const CVec3*)(car + 0x5C4), (const PlainValMatrix*)&c->pose);
    c->pose.pos.v[0] = c->pose.pos.v[0] + w.x;
    c->pose.pos.v[1] = c->pose.pos.v[1] + w.y;
    c->pose.pos.v[2] = c->pose.pos.v[2] + w.z;
    int flags = 0;
    static f32 cosLimit = (f32)cos(0.5235987901687622);
    if (!fn_801EA098((s16*)(c->car + 0x6C), &pos.x, &n.x, &flags, c->carFrame.pos.v[0], c->carFrame.pos.v[2])) {
        n.x = c->carFrame.rot.m[3];
        n.y = c->carFrame.rot.m[4];
        n.z = c->carFrame.rot.m[5];
    } else {
        f32 t = (PSVECDotProduct((const Vec*)&n.x, (const Vec*)&c->carFrame.rot.m[3]) - cosLimit) / (1.0f - cosLimit);
        f32 s = 0.0f > t ? 0.0f : t;
        n.x = n.x * s;
        n.y = n.y * s;
        n.z = n.z * s;
        n.x = n.x + c->carFrame.rot.m[3] * (1.0f - s);
        n.y = n.y + c->carFrame.rot.m[4] * (1.0f - s);
        n.z = n.z + c->carFrame.rot.m[5] * (1.0f - s);
    }
    f32 k = c->b138 ? 0.0f : 0.95f;
    c->v28.v[0] = c->v28.v[0] * k;
    c->v28.v[1] = c->v28.v[1] * k;
    c->v28.v[2] = c->v28.v[2] * k;
    c->v28.v[0] = c->v28.v[0] + n.x * (1.0f - k);
    c->v28.v[1] = c->v28.v[1] + n.y * (1.0f - k);
    c->v28.v[2] = c->v28.v[2] + n.z * (1.0f - k);
    ScV(&c->pose.rot.m[3], &c->v28.v[0], 0.5f);
    AddSV(&c->pose.rot.m[3], &c->carFrame.rot.m[3], 0.5f);
    PSVECCrossProduct((const Vec*)&c->pose.rot.m[3], (const Vec*)&c->pose.rot.m[6], (Vec*)&c->pose.rot.m[0]);
    PSVECCrossProduct((const Vec*)&c->pose.rot.m[0], (const Vec*)&c->pose.rot.m[3], (Vec*)&c->pose.rot.m[6]);
    Orthonormalize(&((CamRec*)p)->pose);
    if (!c->b138) {
        ScV(c->v34.v, c->v34.v, 0.96f);
        bounce.Set(fn_8018AE3C(c->car), fn_8018AE44(c->car), fn_8018AE4C(c->car));
        CsAddScaled(c->v34.v, c->v34.v, &bounce.x, 0.04000002f);
        f32 f = 5.0f * (-0.005f * fn_8018AE4C(c->car));
        c->pose.pos.v[0] = c->pose.pos.v[0] + c->carFrame.rot.m[6] * f;
        c->pose.pos.v[1] = c->pose.pos.v[1] + c->carFrame.rot.m[7] * f;
        c->pose.pos.v[2] = c->pose.pos.v[2] + c->carFrame.rot.m[8] * f;
        f32 g = 2.0f * (5.0f * (-0.005f * (fn_8018AE44(c->car) - 0.9f * c->v34.v[1])));
        c->pose.pos.v[0] = c->pose.pos.v[0] + c->carFrame.rot.m[3] * g;
        c->pose.pos.v[1] = c->pose.pos.v[1] + c->carFrame.rot.m[4] * g;
        c->pose.pos.v[2] = c->pose.pos.v[2] + c->carFrame.rot.m[5] * g;
    }
    fn_80273B4C(p);
    euler[0] = ((CarBody*)c->car)->f5C0;
    euler[1] = 0.0f;
    euler[2] = 0.0f;
    Mat3FromEuler(&rot, (const Vec3*)euler);
    Mat3Mul((Mat3*)&c->pose, &rot, (Mat3*)(const PlainValMatrix*)&c->pose);
    if (c->car[0x260C]) {
        flipEuler[0] = 0.0f;
        flipEuler[1] = 3.1415927f;
        flipEuler[2] = 0.0f;
        Mat3FromEuler(&flip, (const Vec3*)flipEuler);
        Mat3Mul((Mat3*)&c->pose, &flip, (Mat3*)(const PlainValMatrix*)&c->pose);
    }
}


void fn_80274460(u8* p);

// Camera frame (0x58) = the car-relative frame (0xC0), moved by a lift that follows the car's 0x2458 value;
// flag 0x260C on the car takes fn_80274460 instead.
void fn_80274268(u8* p) {
    CamRec* cr = (CamRec*)p;
    u8* car = cr->car;
    if (U8(car, 0x260C)) {
        fn_80274460(p);
        return;
    }
    CVec3 v;
    CVec3 w;
    *(PlainValMatrix*)&cr->pose = *(PlainValMatrix*)&cr->carFrame;
    f32 h = F32(car, 0x2458) - 1.397f;
    v.Set(0.0f, 0.8f, -0.1f + h);
    MulVec(&w, &v, (PlainValMatrix*)&cr->pose);
    cr->pose.pos.v[0] = cr->pose.pos.v[0] + w.x;
    cr->pose.pos.v[1] = cr->pose.pos.v[1] + w.y;
    cr->pose.pos.v[2] = cr->pose.pos.v[2] + w.z;
    fn_80273B4C(p);
}

void fn_8027445C(u8* p) { fn_80274268(p); }


// Camera frame (0x58) = the car-relative frame (0xC0) turned about y by pi and lifted.
void fn_80274460(u8* p) {
    CamRec* cr = (CamRec*)p;
    Mat3 rot;
    f32 euler[3];
    CVec3 v;
    CVec3 w;
    *(PlainValMatrix*)&cr->pose = *(PlainValMatrix*)&cr->carFrame;
    euler[0] = 0.0f;
    euler[1] = 3.1415927f;
    euler[2] = 0.0f;
    Mat3FromEuler(&rot, (const Vec3*)euler);
    Mat3Mul((Mat3*)&cr->pose, &rot, (Mat3*)&cr->pose);
    v.x = 0.0f;
    v.y = 0.8f;
    v.z = 0.0f;
    MulVec(&w, &v, (PlainValMatrix*)&cr->pose);
    cr->pose.pos.v[0] = cr->pose.pos.v[0] + w.x;
    cr->pose.pos.v[1] = cr->pose.pos.v[1] + w.y;
    cr->pose.pos.v[2] = cr->pose.pos.v[2] + w.z;
}

void fn_80274660(u8* unused, u8* p) {
    CamRec* cr = (CamRec*)p;
    f32 zero = 0.0f;
    f32 one = 1.0f;
    cr->pose.rot.m[1] = zero;
    cr->pose.rot.m[0] = one;
    cr->pose.rot.m[2] = zero;
    cr->pose.rot.m[3] = zero;
    cr->pose.rot.m[4] = one;
    cr->pose.rot.m[5] = zero;
    cr->pose.rot.m[6] = zero;
    cr->pose.rot.m[7] = zero;
    cr->pose.rot.m[8] = one;
    cr->pose.pos.v[0] = zero;
    cr->pose.pos.v[1] = zero;
    cr->pose.pos.v[2] = zero;
    f32 axis[3];
    axis[0] = 1.5707964f;
    axis[1] = zero;
    axis[2] = zero;
    Mat3 rot;
    Mat3FromEuler(&rot, (const Vec3*)axis);
    Mat3Mul((Mat3*)&cr->pose, &rot, (Mat3*)&cr->pose);
    cr->pose.pos.v[0] = cr->carFrame.pos.v[0];
    cr->pose.pos.v[1] = cr->carFrame.pos.v[1];
    cr->pose.pos.v[2] = cr->carFrame.pos.v[2];
    f32 h;
    if (cr->mode == 10) {
        h = 10.1f;
    } else {
        h = 50.0f;
    }
    cr->pose.pos.v[1] += h;
    cr->vel.v[1] = 0.0f;
}

double tan(double x);
double atan(double x);
extern f32 __float_epsilon[];  // epsilon

// Aims the camera frame (0x58) from its position (0x7C) at the look point (0x4C), with world y as up
// (the car's forward axis when looking straight down). A positive `dist` pulls the camera toward that
// distance by `stiff`; the fov follows from `width` / `zoom` and the remaining distance unless `fov` is
// given.
static inline f32 CsTanHalf(f32 a) { return (f32)tan(a / 2.0f); }
static inline f32 CsAtanTwice(f32 t) { return 2.0f * (f32)atan(t); }
void fn_80274730(u8* p, f32 fov, f32 dist, f32 zoom, f32 stiff) {
    CamRec* cr = (CamRec*)p;
    f32 dx = cr->v4C.v[0] - cr->pose.pos.v[0];
    f32 dy = cr->v4C.v[1] - cr->pose.pos.v[1];
    f32 dz = cr->v4C.v[2] - cr->pose.pos.v[2];
    cr->pose.rot.m[6] = dx;
    cr->pose.rot.m[7] = dy;
    cr->pose.rot.m[8] = dz;
    f64 absX = __fabs(dx);
    f32 absXFloat = (f32)absX;
    if (absXFloat <= __float_epsilon[0]) {
        f64 absZ = __fabs(dz);
        f32 absZFloat = (f32)absZ;
        if (absZFloat <= __float_epsilon[0]) {
            cr->pose.rot.m[6] = cr->carFrame.rot.m[6];
            cr->pose.rot.m[8] = cr->carFrame.rot.m[8];
        }
    }
    CsVec up(0.0f, 1.0f, 0.0f);
    PSVECCrossProduct((const Vec*)&up.x, (const Vec*)((f32*)&cr->pose.rot.m[6]), (Vec*)((f32*)&cr->pose));
    PSVECCrossProduct((const Vec*)((f32*)&cr->pose.rot.m[6]), (const Vec*)((f32*)&cr->pose), (Vec*)((f32*)&cr->pose.rot.m[3]));
    Orthonormalize(&((CamRec*)p)->pose);
    f32 d = PSVECDistance((const Vec*)((f32*)&cr->pose.pos), (const Vec*)((f32*)&cr->v4C));
    if (dist > 0.0f) {
        f32 k = (1.0f - stiff) * (d - dist);
        cr->pose.pos.v[0] = cr->pose.pos.v[0] + cr->pose.rot.m[6] * k;
        cr->pose.pos.v[1] = cr->pose.pos.v[1] + cr->pose.rot.m[7] * k;
        cr->pose.pos.v[2] = cr->pose.pos.v[2] + cr->pose.rot.m[8] * k;
    }
    d = PSVECDistance((const Vec*)((f32*)&cr->pose.pos), (const Vec*)((f32*)&cr->v4C));
    f32 lo = 0.32724923f;
    f32 v = 1.9634954f;
    f32 half = 1.1780972f;
    f32 inv = zoom > 0.1f ? 1.0f / zoom : 1.0f;
    f32 w = 30.5f * inv;
    f32 a = CsAtanTwice(w / d * CsTanHalf(half));
    v = v < a ? v : a;
    v = v > lo ? v : lo;
    if (fov > 0.0f) {
        cr->fov = fov;
        cr->f194 = fov;
    } else {
        f32 prev = cr->f194;
        f32 f = Lerp(prev, v, 1.0f);
        cr->fov = f;
        cr->f194 = f;
    }
    cr->vel.v[0] = 0.0f;
    cr->vel.v[1] = 0.0f;
    cr->vel.v[2] = 0.0f;
}


// Moves the camera by `move` in the car frame, then levels its up vector (0x28) toward the ground
// normal below the target (or the car's up axis), and rebuilds the frame around it.
void fn_802749FC(u8* p, const CVec3* move) {
    CamRec* c = (CamRec*)p;
    if (c->b138) {
        c->v28.v[0] = 0.0f;
        c->v28.v[1] = 1.0f;
        c->v28.v[2] = 0.0f;
        c->v34.v[0] = 0.0f;
        c->v34.v[1] = 0.0f;
        c->v34.v[2] = 0.0f;
    }
    CVec3 n;
    CVec3 pos;
    CVec3 w;
    c->pose = c->carFrame;
    MulVec(&w, move, (const PlainValMatrix*)&c->pose);
    c->pose.pos.v[0] = c->pose.pos.v[0] + w.x;
    c->pose.pos.v[1] = c->pose.pos.v[1] + w.y;
    c->pose.pos.v[2] = c->pose.pos.v[2] + w.z;
    int flags = 0;
    static f32 cosLimit = (f32)cos(0.5235987901687622);
    if (!fn_801EA098((s16*)(c->car + 0x6C), &pos.x, &n.x, &flags, c->carFrame.pos.v[0], c->carFrame.pos.v[2])) {
        n.x = c->carFrame.rot.m[3];
        n.y = c->carFrame.rot.m[4];
        n.z = c->carFrame.rot.m[5];
    } else {
        f32 t = (PSVECDotProduct((const Vec*)&n.x, (const Vec*)&c->carFrame.rot.m[3]) - cosLimit) / (1.0f - cosLimit);
        f32 s = 0.0f > t ? 0.0f : t;
        n.x = n.x * s;
        n.y = n.y * s;
        n.z = n.z * s;
        n.x = n.x + c->carFrame.rot.m[3] * (1.0f - s);
        n.y = n.y + c->carFrame.rot.m[4] * (1.0f - s);
        n.z = n.z + c->carFrame.rot.m[5] * (1.0f - s);
    }
    f32 k = c->b138 ? 0.0f : 0.9f;
    c->v28.v[0] = c->v28.v[0] * k;
    c->v28.v[1] = c->v28.v[1] * k;
    c->v28.v[2] = c->v28.v[2] * k;
    c->v28.v[0] = c->v28.v[0] + n.x * (1.0f - k);
    c->v28.v[1] = c->v28.v[1] + n.y * (1.0f - k);
    c->v28.v[2] = c->v28.v[2] + n.z * (1.0f - k);
    ScV(&c->pose.rot.m[3], &c->v28.v[0], 0.5f);
    AddSV(&c->pose.rot.m[3], &c->carFrame.rot.m[3], 0.5f);
    PSVECCrossProduct((const Vec*)&c->pose.rot.m[3], (const Vec*)&c->pose.rot.m[6], (Vec*)&c->pose.rot.m[0]);
    PSVECCrossProduct((const Vec*)&c->pose.rot.m[0], (const Vec*)&c->pose.rot.m[3], (Vec*)&c->pose.rot.m[6]);
    Orthonormalize(&((CamRec*)p)->pose);
}

void fn_80274E94(u8* p) { fn_80274730(p, -1.0f, -1.0f, -1.0f, 0.0f); }

void fn_80274EA8(u8* p) {
    CamRec* cr = (CamRec*)p;
    f32 d[3];
    d[0] = cr->pose.pos.v[0] - cr->carFrame.pos.v[0];
    d[1] = cr->pose.pos.v[1] - cr->carFrame.pos.v[1];
    d[2] = cr->pose.pos.v[2] - cr->carFrame.pos.v[2];
    f32 m = PSVECMag((const Vec*)d);
    if (m > 183.0f) {
        f32 s = -(m - 183.0f) / m;
        cr->pose.pos.v[0] = cr->pose.pos.v[0] + d[0] * s;
        cr->pose.pos.v[1] = cr->pose.pos.v[1] + d[1] * s;
        cr->pose.pos.v[2] = cr->pose.pos.v[2] + d[2] * s;
    }
    fn_80274730(p, -1.0f, 25.0f, 6.0f, 0.15f);
}

double sin(double x);
double cos(double x);

// CamVec: rvec.h's RVec3.
static inline RVec3 operator*(const CVec3& v, f32 k) { return RVec3(k * v.x, k * v.y, k * v.z); }
static inline RVec3 operator+(CVec3 a, const RVec3& b) { return RVec3(a.x + b.x, a.y + b.y, a.z + b.z); }

struct CamPreset {
    CVec3 off;
    CVec3 rot;
    f32 fov;
};

void fn_801BFD34(u8* out, const RVec3* v, const u8* frame);

// A camera frame offset: rotation then translation (one 48-byte local; helper name unproven).
struct CamRotOff {
    Mat3 rot;    // offset 0x0
    RVec3 pos;  // offset 0x24
};

// Rotation from the rotation vector w (axis * angle), Rodrigues' formula; identity for a tiny angle.
static inline void RotFromVec(Mat3* r, const RVec3& w) {
    f32 a = PSVECMag((const Vec*)&w.x);
    if (a > 0.0001f) {
        f32 y, z, s, x, c, k;  // retail keeps x, s, z, y in f31..f28: the reverse of this order
        f32 inv = 1.0f / a;
        x = inv * w.x;
        y = inv * w.y;
        z = inv * w.z;
        s = (f32)sin(a);
        c = (f32)cos(a);
        k = 1.0f - c;
        r->m[0][1] = y * (k * x) + s * z;
        r->m[0][2] = z * (k * x) - s * y;
        r->m[1][0] = y * (k * x) - s * z;
        r->m[1][1] = c + k * (y * y);
        r->m[2][0] = z * (k * x) + s * y;
        r->m[0][0] = c + k * (x * x);
        r->m[1][2] = z * (k * y) + s * x;
        r->m[2][2] = c + k * (z * z);
        r->m[2][1] = z * (k * y) - s * x;
    } else {
        r->m[0][0] = 1.0f;
        r->m[0][1] = 0.0f;
        r->m[0][2] = 0.0f;
        r->m[1][0] = 0.0f;
        r->m[1][1] = 1.0f;
        r->m[1][2] = 0.0f;
        r->m[2][0] = 0.0f;
        r->m[2][1] = 0.0f;
        r->m[2][2] = 1.0f;
    }
}

// Rotation (0x58, 3x3) from the rotation vector w (axis * angle), Rodrigues' formula; identity for a
// tiny angle. t goes to 0xBC.
void fn_80274F74(u8* p, const f32* w, f32 t) {
    CamRec* cr = (CamRec*)p;
    RotFromVec((Mat3*)&cr->pose, *(const RVec3*)w);
    cr->fov = t;
}

void fn_8026E850(f32* pos, const f32* target, u8* cam, const f32* up);

// Re-aims the camera, then slides it along its view axis toward `dist` from the target.
void fn_802750C4(u8* p, f32 k, f32 dist) {
    CamRec* cr = (CamRec*)p;
    f32 d[3];
    f32 up[3];
    up[0] = 0.0f;
    up[1] = 1.0f;
    up[2] = 0.0f;
    fn_8026E850((f32*)&cr->pose.pos, (f32*)&cr->carFrame.pos, p, up);
    d[0] = cr->carFrame.pos.v[0] - cr->pose.pos.v[0];
    d[1] = cr->carFrame.pos.v[1] - cr->pose.pos.v[1];
    d[2] = cr->carFrame.pos.v[2] - cr->pose.pos.v[2];
    f32 e = PSVECDotProduct((const Vec*)((f32*)&cr->pose.rot.m[6]), (const Vec*)d) - dist;
    f32 t = e * k;
    f32 x = cr->pose.pos.v[0] + cr->pose.rot.m[6] * t;
    f32 y = cr->pose.pos.v[1] + cr->pose.rot.m[7] * t;
    f32 z = cr->pose.pos.v[2] + cr->pose.rot.m[8] * t;
    cr->pose.pos.v[0] = x;
    cr->pose.pos.v[1] = y;
    cr->pose.pos.v[2] = z;
}

// A fixed camera placement: offset from the car frame, rotation vector in degrees, field of view.
// Camera frame = the car frame (0xC0) turned by the preset rotation and moved to its offset.
void fn_802751AC(u8* p) {
    CamRec* cr = (CamRec*)p;
    CamPreset c = {{0.0f, 1.1f, 0.9f}, {8.0f, 0.0f, 0.0f}, 86.25f};
    *(PlainValMatrix*)&cr->pose = *(PlainValMatrix*)&cr->carFrame;
    CamRotOff f;
    f32 z = c.off.z;
    f.pos.Set(c.off.x, c.off.y, z);
    RotFromVec(&f.rot, c.rot * 0.017453292f);
    fn_801BFD34((u8*)&cr->pose.pos, &f.pos, (u8*)&cr->carFrame);
    Mat3Mul((Mat3*)&cr->pose, &f.rot, (Mat3*)&cr->carFrame);
    f32 fovv = c.fov;
    cr->fov = 0.017453292f * fovv;
}

// As fn_802751AC, but the offset follows the car's first point (0x2450) instead of the preset's.
void fn_80275410(u8* p) {
    CamRec* cr = (CamRec*)p;
    CamPreset c;
    u8* car = cr->car;
    const CamPreset k = {{1.7f, 0.4f, -0.3f}, {8.0f, -21.0f, 0.0f}, 65.25f};
    c = k;
    RVec3 off = *(CVec3*)(car + 0x2450) + RVec3(0.8f, 0.65f, -1.75f);
    *(PlainValMatrix*)&cr->pose = *(PlainValMatrix*)&cr->carFrame;
    Mat3 rot;
    RotFromVec(&rot, c.rot * 0.017453292f);
    fn_801BFD34((u8*)&cr->pose.pos, &off, (u8*)&cr->carFrame);
    Mat3Mul((Mat3*)&cr->pose, &rot, (Mat3*)&cr->carFrame);
    f32 fovv = c.fov;
    cr->fov = 0.017453292f * fovv;
}

// As fn_802751AC, but the offset follows the car's second point (0x2538) instead of the preset's.
void fn_802756A4(u8* p) {
    CamRec* cr = (CamRec*)p;
    CamPreset c;
    u8* car = cr->car;
    const CamPreset k = {{1.9f, 0.7f, 0.1f}, {0.7f, 193.0f, -21.9f}, 64.3f};
    c = k;
    RVec3 off = *(CVec3*)(car + 0x2538) + RVec3(0.85f, 0.9f, 2.0f);
    *(PlainValMatrix*)&cr->pose = *(PlainValMatrix*)&cr->carFrame;
    Mat3 rot;
    RotFromVec(&rot, c.rot * 0.017453292f);
    fn_801BFD34((u8*)&cr->pose.pos, &off, (u8*)&cr->carFrame);
    Mat3Mul((Mat3*)&cr->pose, &rot, (Mat3*)&cr->carFrame);
    f32 fovv = c.fov;
    cr->fov = 0.017453292f * fovv;
}

void fn_80275938(u8* p) {
    CamRec* cr = (CamRec*)p;
    CamPreset c = {{-1.0f, 0.9f, -3.2f}, {5.8f, 63.6f, 1.8f}, 87.75f};
    *(PlainValMatrix*)&cr->pose = *(PlainValMatrix*)&cr->carFrame;
    CamRotOff f;
    f32 z = c.off.z;
    f.pos.Set(c.off.x, c.off.y, z);
    RotFromVec(&f.rot, c.rot * 0.017453292f);
    fn_801BFD34((u8*)&cr->pose.pos, &f.pos, (u8*)&cr->carFrame);
    Mat3Mul((Mat3*)&cr->pose, &f.rot, (Mat3*)&cr->carFrame);
    f32 fovv = c.fov;
    cr->fov = 0.017453292f * fovv;
}

double fmod(double x, double y);
double sin(double x);
double cos(double x);
int fn_801E9E30(u8* ground, f32* pos);
extern f32 lbl_8055E3C8;         // game time
f32 lbl_8055F588;         // last ground height


// Orbits the camera around its target at `radius`, keeping it above the ground.

void fn_80275B9C(u8* p, f32 radius, f32 height, f32 speed) {
    CamRec* cr = (CamRec*)p;
    cr->f15C = fmod(speed * lbl_8055E3C8, 6.2831854820251465);
    f32 s = radius * (f32)sin(cr->f15C);
    f32 c = radius * (f32)cos(cr->f15C);
    CsVec pos;
    CsVec off;
    off.x = c;
    off.y = 0.0f;
    off.z = s;
    pos.x = cr->carFrame.pos.v[0] + off.x;
    pos.y = cr->carFrame.pos.v[1] + off.y;
    pos.z = cr->carFrame.pos.v[2] + off.z;
    if (!fn_801E9E30((u8*)&cr->h1EC, &pos.x)) {
        pos.y = 2.0f + lbl_8055F588;
        CsVec t(Lerp(pos.x, cr->carFrame.pos.v[0], 0.5f), Lerp(pos.y, cr->carFrame.pos.v[1], 0.5f),
                Lerp(pos.z, cr->carFrame.pos.v[2], 0.5f));
        pos = t;
    } else {
        lbl_8055F588 = pos.y;
    }
    pos.y += height;
    f32 up[3];
    up[0] = 0.0f;
    up[1] = 1.0f;
    up[2] = 0.0f;
    fn_8026E850(&pos.x, (f32*)&cr->carFrame.pos, p, up);
    cr->fov = 1.0471976f;
}


// As fn_802751AC, but relative to a level frame at the target: forward = the car's forward axis,
// up = world y made perpendicular.
void fn_80275D0C(u8* p) {
    CamRec* cr = (CamRec*)p;
    CamPreset c = {{-28.5f, 9.1f, -9.1f}, {16.3f, 70.4f, -13.4f}, 18.75f};
    *(PlainValMatrix*)&cr->pose = *(PlainValMatrix*)&cr->carFrame;
    CamRotOff g;  // declared before f: retail has f below g on the stack
    PlainValMatrix f;
    f.origin.x = cr->carFrame.pos.v[0];
    f.origin.y = cr->carFrame.pos.v[1];
    f.origin.z = cr->carFrame.pos.v[2];
    f.basis[2].x = cr->carFrame.rot.m[6];
    f.basis[2].y = cr->carFrame.rot.m[7];
    f.basis[2].z = cr->carFrame.rot.m[8];
    f.basis[1].x = 0.0f;
    f.basis[1].y = 1.0f;
    f.basis[1].z = 0.0f;
    PSVECCrossProduct((const Vec*)&f.basis[1].x, (const Vec*)&f.basis[2].x, (Vec*)&f.basis[0].x);
    PSVECCrossProduct((const Vec*)&f.basis[2].x, (const Vec*)&f.basis[0].x, (Vec*)&f.basis[1].x);
    g.pos.Set(c.off.x, c.off.y, c.off.z);
    RotFromVec(&g.rot, RVec3(DegToRad(c.rot.x), DegToRad(c.rot.y), DegToRad(c.rot.z)));
    fn_801BFD34((u8*)&cr->pose.pos, &g.pos, (u8*)&f);
    Mat3Mul((Mat3*)&cr->pose, &g.rot, (Mat3*)&f);
    f32 fovv = c.fov;
    cr->fov = 0.017453292f * fovv;
}

void fn_80275FC0(u8* p) {
    CamRec* cr = (CamRec*)p;
    *(PoseWords*)&cr->pose = *(PoseWords*)&cr->saved;
    cr->fov = cr->f1D4;
}

void fn_80276044(u8* p) { fn_80276BE4(p + 0x58, p + 0xBC); }

void fn_80276054(u8* p) { fn_802704D4(4, p); }

}  // extern "C"
