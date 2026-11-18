// Camera manager (0x80265DD8-0x8026A600): the per-player camera records, camera type dispatch
// ("Unrecognized camera type (%d) (id=%d)", "Physcam: get_camera_frame: ..."), crash and cheer sound
// cues ("crash_vol", "cheer3.sfx") and the per-slot shake levels. Written against raw field offsets;
// the large routines and the initialiser are still stubs. Names are guesses apart from the strings.
#include "game/common.h"
#include "game/carbody.h"
#include "game/rvec.h"
#include <revolution/MTX/vec.h>
#include <string.h>
#include "game/camrec.h"
#include "game/camview.h"
#include "game/idealline.h"
#include "game/deity.h"
#include "game/centerline.h"
#include "game/math_types.h"

struct CameraConstant { f32 value; };

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

extern "C" const f32 lbl_80563C44;
extern "C" const f32 lbl_80563C48;
extern "C" const f32 lbl_80563C4C;
extern "C" const f32 lbl_80563C50;
extern "C" const f32 lbl_80563C6C;
extern "C" const f32 lbl_80563C98;
extern "C" const f32 lbl_80563C9C;

class VD {
public:
    virtual ~VD(void);
};

#include "game/words12.h"
#include "game/mathutil.h"

class CenterLine;


// MATCHING-AID: fn_80268E00 keeps idealline.h's IdealFollower as plain storage built and destroyed by linker name:
// retail passes the constructor's RETURNED pointer to fn_80134E34, which a named IdealFollower local cannot give
// (+1 instruction for every form tried, aids audit 2 2026-11-17); the other two users are real IdealFollower locals.
struct IdealFollowerData {
    u8 pad0[0x4];    // offset 0x0
    IdealPoint* seg; // offset 0x4, current line segment
    f32 t;           // offset 0x8, how far along it
    u8 padC[0x4];    // offset 0xC
    f32 dir[2];      // offset 0x10, heading (x, z)
    u8 pad18[0x14];  // offset 0x18
    CenterLine* line;  // offset 0x2C
    u8 pad30[0x20];  // offset 0x30
    u8* buf;         // offset 0x50
    u8 pad54[0x14];  // offset 0x54
};

// A position on the ideal line: the segment and how far along it.
#include "game/ilpos.h"

extern "C" {
f32 fn_80134E34(IdealFollowerData* f, f32 dist);  // aidriver: returns the follower's distance
ILPos fn_80136468(IdealFollowerData* f, f32 m);  // idealline: returns the ILPos by value
void fn_80268BD4(u8* rec, const IdealPoint* seg, f32 t, f32 k);
}

extern CamPose lbl_80499950;  // trackside camera frames
extern CamPose lbl_80499920;

extern "C" IdealFollowerData* __ct__13IdealFollowerFiP10CenterLine(IdealFollowerData* f, int mode, CenterLine* line);
// The base LineFollower destructor (idealline.cpp) that IdealFollower's inlined destructor ends with, by its
// mangled name like the constructor above (IdealFollowerData is a flat struct here).
extern "C" void __dt__12LineFollowerFv(IdealFollowerData* f, int del);

extern "C" {
void EventUnInstallHandler(void* cb);
void fn_801BFD34(u8* a, f32* b, u8* c);
f32 fn_80129DC4(int stream, f32 lo, f32 hi);
s32 fn_80129D90(int stream, s32 lo, s32 hi);
int fn_80178CA4(void);
void fn_80278288(void);
void fn_802733F0(void);
void fn_80209110(void* p);
void fn_801E9DC4(void* p);
void fn_802735F4(int i, u8 b);
int fn_802734F0(int mode);
int fn_80267EE0(int mode);
int fn_801EA994(void);  // racer count
void fn_80265E08(u8* p);
void fn_80266150(u8* p);
u8* fn_80267EE4(int i);
int fn_8026A554(s32* p);
}

// One random shake sample.
struct ShakeEntry {
    f32 x;  // offset 0x0
    f32 y;  // offset 0x4
    s32 n;  // offset 0x8
};

extern f32 lbl_8055C4C0;
extern f32 lbl_8055F47C;

// A scratch 3x4 matrix.
#include "game/mtx34.h"

extern Mtx34 lbl_80499050;
extern f32 lbl_8055E3C8;
extern f32 lbl_8055C4E0;
extern f32 lbl_8055C4E4;
extern s32 lbl_8055C4E8;
extern s32 lbl_8055C4EC;
extern s32 lbl_8055CFF0;
extern s32 lbl_8055CFF4;
extern ShakeEntry lbl_804999A0[];
extern VD* lbl_8055F490;
extern u32 lbl_8055F494;
extern s32 lbl_8055F4A0;
extern s32 lbl_8055F4A4;
extern u8 lbl_8055F4A8[8];
extern CamRec lbl_80499080[4];
extern s32 lbl_80499E50[];
extern f32 lbl_80499980[];
extern u8 lbl_8055F488;
extern f32 lbl_8055F484;

// The first camera literal occupies an initialized slot in the retail constant pool.
#ifdef __MWERKS__
#pragma push
#pragma explicit_zero_data on
#endif
extern "C" const CameraConstant lbl_80563C20 = {0.0f};
#ifdef __MWERKS__
#pragma pop
#endif

extern "C" const CameraConstant lbl_80563C24 = {0.017453292f};
extern "C" const CameraConstant lbl_80563C28 = {-1.0f};
extern "C" const CameraConstant lbl_80563C2C = {1.0f};
extern "C" const CameraConstant lbl_80563C30 = {-2.0f};
extern "C" const CameraConstant lbl_80563C34 = {3.0f};

extern "C" {

u8* fn_80265DD8(u8* p) {
    fn_80265E08(p);
    return p;
}

void fn_80265E08(u8* p) {
    f32 z = lbl_80563C20.value;
    U32(p, 0x0) = 0;
    F32(p, 0x14) = z;
    F32(p, 0x10) = z;
    F32(p, 0xC) = z;
    U32(p, 0x4) = 0;
    U32(p, 0x8) = 0;
}

void fn_80265E2C(void) {
    ShakeEntry* e = lbl_804999A0;
    for (int i = 0; i < 100; i++) {
        e->x = lbl_80563C24.value * fn_80129DC4(6, -lbl_8055C4E0, lbl_8055C4E0);
        e->y = lbl_80563C24.value * fn_80129DC4(6, -lbl_8055C4E4, lbl_8055C4E4);
        e->n = fn_80129D90(6, lbl_8055C4E8, lbl_8055C4EC);
        e++;
    }
}


// Camera shake: walks the random shake table (lbl_804999A0), easing between consecutive entries.
struct CamShake {
    s32 idx;   // offset 0x0, current table entry
    s32 acc;   // offset 0x4, time into it
    s32 last;  // offset 0x8, last clock
    f32 t;     // offset 0xC, eased fraction
    f32 x;     // offset 0x10
    f32 y;     // offset 0x14
};

static inline f32 ShakeEase(f32 x) {
    if (x < lbl_80563C28.value) {
        return lbl_80563C28.value;
    }
    if (x > lbl_80563C2C.value) {
        return lbl_80563C2C.value;
    }
    if (x > lbl_80563C20.value) {
        const f32 cubic = lbl_80563C30.value * x;
        const f32 quadratic = lbl_80563C34.value * x;
        return cubic * x * x + quadratic * x;
    }
    const f32 cubic = lbl_80563C30.value * x;
    const f32 quadratic = lbl_80563C34.value * x;
    return cubic * x * x - quadratic * x;
}

void fn_80265EC4(CamShake* s, s32 now, int paused) {
    if (paused) {
        s->last = now;
        return;
    }
    s->acc += now - s->last;
    s->last = now;
    while (s->acc > lbl_804999A0[s->idx].n) {
        s->acc -= lbl_804999A0[s->idx].n;
        s->idx = (s->idx + 1) % 100;
    }
    s->t = ShakeEase((f32)s->acc / (f32)lbl_804999A0[s->idx].n);
    ShakeEntry* a = &lbl_804999A0[s->idx];
    ShakeEntry* b = &lbl_804999A0[(s->idx + 1) % 100];
    s->x = a->x + s->t * (b->x - a->x);
    s->y = a->y + s->t * (b->y - a->y);
}


// out = from + dir * scale (0x10), lifted by |dir| * rise (0x14).
void fn_80266070(u8* p, f32* out, const f32* from, const f32* dir) {
    f32 v[3];
    f32 k = F32(p, 0x10);
    v[0] = dir[0] * k;
    v[1] = dir[1] * k;
    v[2] = dir[2] * k;
    v[1] += F32(p, 0x14) * PSVECMag((const Vec*)dir);
    f32 x = from[0] + v[0];
    f32 y = from[1] + v[1];
    f32 z = from[2] + v[2];
    out[0] = x;
    out[1] = y;
    out[2] = z;
}

u8* fn_80266120(u8* p) {
    fn_80266150(p);
    return p;
}

void fn_80266150(u8* p) {
    U32(p, 0x4) = 0;
    U32(p, 0x0) = 0;
    F32(p, 0x8) = lbl_8055C4C0;
    F32(p, 0xC) = lbl_80563C2C.value;
}

extern f32 lbl_8055C4BC;  // funky fov scale
extern s32 lbl_8055C4B4;  // funky fov ease start time
extern s32 lbl_8055C4B8;  // funky fov ease end time

static inline f32 ClampF(f32 x) {
    x = lbl_80563C2C.value < x ? lbl_80563C2C.value : x;
    return x > lbl_80563C20.value ? x : lbl_80563C20.value;
}

// "funky_fov": on a camera cut, aim the view angle at a scaled (capped) fov, then ease toward it.
struct FunkyFov {
    s32 last;    // offset 0x0
    s32 acc;     // offset 0x4
    f32 target;  // offset 0x8
    f32 t;       // offset 0xC
};

// Helper name unproven: the clamped value arrives as a parameter, so it stays the left operand of the multiply.
static inline f32 FovRemap(f32 t) { return lbl_80563C20.value + t * lbl_80563C2C.value; }
void fn_80266170(FunkyFov* f, s32 now, int paused, int fresh, f32 fov) {
    if (fresh) {
        f->last = now;
        f->acc = 0;
        f->target = MinF(lbl_8055C4BC * fov, lbl_8055C4C0);
    }
    if (paused) {
        f->last = now;
        return;
    }
    f->acc += now - f->last;
    f->last = now;
    f32 e;
    f32 a = f->acc;
    e = lbl_8055C4B8;
    f32 st = lbl_8055C4B4;
    f32 hi = lbl_80563C2C.value;
    f32 x = (a - st) / (e - st);
    x = hi < x ? hi : x;
    x = x > lbl_80563C20.value ? x : lbl_80563C20.value;
    f->t = ShakeEase(FovRemap(x));
}

extern f32 lbl_8055C4C4;

// out = from + dir * (distance to `to`, eased by the blend at 0xC).
void fn_802662D8(u8* p, f32* out, const f32* from, const f32* dir, const f32* to) {
    f32 d = PSVECDistance((const Vec*)from, (const Vec*)to) - lbl_8055C4C4;
    f32 k = F32(p, 0xC);
    f32 t = lbl_80563C20.value + k * (d - lbl_80563C20.value);
    f32 x = from[0] + dir[0] * t;
    f32 y = from[1] + dir[1] * t;
    f32 z = from[2] + dir[2] * t;
    out[0] = x;
    out[1] = y;
    out[2] = z;
}

}  // extern "C"

struct ShakeMat {
    f32 m[3][3];
};

inline f32 Dot(f32 a, f32 b, f32 c, f32 d, f32 e, f32 f);

// d = a * b
static inline void ShakeMul(ShakeMat* pd, const ShakeMat* pa, const ShakeMat* pb) {
    ShakeMat& d = *pd;
    const ShakeMat& a = *pa;
    const ShakeMat& b = *pb;
    if (pd == pa || pd == pb) {
        f32 t00 = Dot(b.m[0][0], a.m[0][0], b.m[1][0], a.m[0][1], b.m[2][0], a.m[0][2]);
        f32 t10 = Dot(b.m[0][0], a.m[1][0], b.m[1][0], a.m[1][1], b.m[2][0], a.m[1][2]);
        f32 t20 = Dot(b.m[0][0], a.m[2][0], b.m[1][0], a.m[2][1], b.m[2][0], a.m[2][2]);
        f32 t01 = Dot(b.m[0][1], a.m[0][0], b.m[1][1], a.m[0][1], b.m[2][1], a.m[0][2]);
        f32 t11 = Dot(b.m[0][1], a.m[1][0], b.m[1][1], a.m[1][1], b.m[2][1], a.m[1][2]);
        f32 t21 = Dot(b.m[0][1], a.m[2][0], b.m[1][1], a.m[2][1], b.m[2][1], a.m[2][2]);
        f32 t02 = Dot(b.m[0][2], a.m[0][0], b.m[1][2], a.m[0][1], b.m[2][2], a.m[0][2]);
        f32 t12 = Dot(b.m[0][2], a.m[1][0], b.m[1][2], a.m[1][1], b.m[2][2], a.m[1][2]);
        f32 t22 = Dot(b.m[0][2], a.m[2][0], b.m[1][2], a.m[2][1], b.m[2][2], a.m[2][2]);
        d.m[0][0] = t00;
        d.m[1][0] = t10;
        d.m[2][0] = t20;
        d.m[0][1] = t01;
        d.m[1][1] = t11;
        d.m[2][1] = t21;
        d.m[0][2] = t02;
        d.m[1][2] = t12;
        d.m[2][2] = t22;
    } else {
        d.m[0][0] = Dot(b.m[0][0], a.m[0][0], b.m[1][0], a.m[0][1], b.m[2][0], a.m[0][2]);
        d.m[1][0] = Dot(b.m[0][0], a.m[1][0], b.m[1][0], a.m[1][1], b.m[2][0], a.m[1][2]);
        d.m[2][0] = Dot(b.m[0][0], a.m[2][0], b.m[1][0], a.m[2][1], b.m[2][0], a.m[2][2]);
        d.m[0][1] = Dot(b.m[0][1], a.m[0][0], b.m[1][1], a.m[0][1], b.m[2][1], a.m[0][2]);
        d.m[1][1] = Dot(b.m[0][1], a.m[1][0], b.m[1][1], a.m[1][1], b.m[2][1], a.m[1][2]);
        d.m[2][1] = Dot(b.m[0][1], a.m[2][0], b.m[1][1], a.m[2][1], b.m[2][1], a.m[2][2]);
        d.m[0][2] = Dot(b.m[0][2], a.m[0][0], b.m[1][2], a.m[0][1], b.m[2][2], a.m[0][2]);
        d.m[1][2] = Dot(b.m[0][2], a.m[1][0], b.m[1][2], a.m[1][1], b.m[2][2], a.m[1][2]);
        d.m[2][2] = Dot(b.m[0][2], a.m[2][0], b.m[1][2], a.m[2][1], b.m[2][2], a.m[2][2]);
    }
}

// ShakeVec: rvec.h's RVec3.
extern "C" {
int fn_8026B0C0(int view);
double sin(double x);
double cos(double x);
extern const f32 lbl_80563C40;  // smallest rotation
extern const f32 lbl_80563CA0;  // smallest shake
extern const f32 lbl_80563CA4;  // 2 pi
extern const f32 lbl_80563CA8;
extern const f32 lbl_80563CAC;  // base frequency
extern const f32 lbl_80563CB0;  // jitter range
extern const f32 lbl_80563CB4;
extern const f32 lbl_80563CB8;  // wave weight

// Rotation from the rotation vector w (axis * angle), Rodrigues' formula; identity for a tiny angle.
static inline void ShakeRot(ShakeMat* r, const RVec3& w) {
    f32 a = PSVECMag((const Vec*)&w.x);
    if (a > lbl_80563C40) {
        f32 c;
        f32 zz;
        f32 yy;
        f32 xx;
        f32 zDiag;
        f32 yDiag;
        f32 inv;
        f32 s;
        f32 k;
        f32 y;
        f32 x;
        f32 z;
        inv = lbl_80563C2C.value / a;
        x = inv * w.x;
        y = inv * w.y;
        z = inv * w.z;
        s = (f32)sin(a);
        c = (f32)cos(a);
        xx = x * x;
        yy = y * y;
        zz = z * z;
        k = lbl_80563C2C.value - c;
        r->m[0][0] = c + k * xx;
        r->m[0][1] = y * (k * x) + s * z;
        r->m[0][2] = z * (k * x) - s * y;
        yDiag = c + k * yy;
        r->m[1][0] = y * (k * x) - s * z;
        r->m[1][1] = yDiag;
        r->m[1][2] = z * (k * y) + s * x;
        zDiag = c + k * zz;
        r->m[2][0] = z * (k * x) + s * y;
        r->m[2][1] = z * (k * y) - s * x;
        r->m[2][2] = zDiag;
    } else {
        r->m[0][0] = lbl_80563C2C.value;
        r->m[0][1] = lbl_80563C20.value;
        r->m[0][2] = lbl_80563C20.value;
        r->m[1][0] = lbl_80563C20.value;
        r->m[1][1] = lbl_80563C2C.value;
        r->m[1][2] = lbl_80563C20.value;
        r->m[2][0] = lbl_80563C20.value;
        r->m[2][1] = lbl_80563C20.value;
        r->m[2][2] = lbl_80563C2C.value;
    }
}

struct CamTrackPos {
    IdealPoint* seg;  // offset 0x0
    f32 t;         // offset 0x4
    u8 pad8[0x4];  // offset 0x8
};

class CamRacerObj {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void vf18(void);
    virtual void vf1C(void);
    virtual u8* GetCar(void);  // 0x20
};

struct CamXZ {
    CamXZ(f32 ax, f32 az) : x(ax), z(az) {}
    f32 x, z;
};

#ifdef __MWERKS__
void fn_80136594(CamTrackPos* out, LineFollower* trk, const CamXZ* xz);  // PORT: idealline returns the ILPos by value (a by-value form here is not code-identical)
#else
// PORT: idealline's fn_80136594 returns its 12-byte ILPos by value; the PPC passes the result slot as the first
// argument (the form above), the host ABI does not (i386: hidden result pointer the callee pops). Call the by-value
// function under its C name and copy the result into the caller's slot.
struct PortILPos12 {
    u32 w[3];
};
PortILPos12 port_ro_fn_80136594(LineFollower* trk, const CamXZ* xz) __asm__("_fn_80136594");
static inline void fn_80136594(CamTrackPos* out, LineFollower* trk, const CamXZ* xz) {
    PortILPos12 r = port_ro_fn_80136594(trk, xz);
    __builtin_memcpy(out, &r, sizeof(*out) < sizeof(r) ? sizeof(*out) : sizeof(r));
}
#endif
int fn_8026A2C8(void);
CamRacerObj* fn_801B2EC0(int i);
extern f32 lbl_8055F49C;  // pass-by range squared
extern u8 lbl_8055F498;   // shake position
extern u8 lbl_8055C4C8;   // shake rotation
extern f32 lbl_8055C4CC;  // track distance ahead of the camera
extern f32 lbl_8055C4D0;  // slowest speed that shakes
extern f32 lbl_8055C4D4;  // speed for full shake
extern f32 lbl_8055C4D8;  // position shake
extern f32 lbl_8055C4DC;  // rotation shake

// Trackside camera rumble: find the car about to pass the camera (just ahead of it along the track and
// nearest in space) and shake the camera's position and orientation by its speed and closeness.
void fn_80266388(u8* self, int paused, CamPose* pose, const f32* eye, u8* car) {
    if (paused) {
        return;
    }
    const f32* rp;
    IdealFollower* trk = ((DeityCar*)((CarBody*)car)->src)->Ideal();  // CarBody::src (0x270): the car's DeityCar
    CamXZ xz(pose->pos.v[0], pose->pos.v[2]);
    CamTrackPos at;
    fn_80136594(&at, trk, &xz);
    f32 camDist = lbl_8055C4CC + (at.seg->dist + at.seg->len * at.t);
    int n = fn_8026A2C8();
    f32 best = lbl_8055F49C;
    u8* near = 0;
    for (int i = 0; i < n; i++) {
        u8* r = fn_801B2EC0(i)->GetCar();
        rp = (const f32*)(r + 0x60);
        CamXZ rxz(rp[0], rp[2]);
        CamTrackPos rat;
        fn_80136594(&rat, trk, &rxz);
        f32 d = (rat.seg->dist + rat.seg->len * rat.t) - camDist;
        if (d < lbl_80563C20.value) {
            u32 lap = trk->il.seg ? trk->il.seg->type : 3;
            d += trk->line->len[lap];
        }
        if (d * d < lbl_8055F49C) {
            f32 sq = PSVECSquareDistance((const Vec*)eye, (const Vec*)rp);
            if (sq < best) {
                best = sq;
                near = r;
            }
        }
    }
    if (near) {
        f32 a = InvLerp(lbl_8055C4D0, lbl_8055C4D4, F32(near, 0x25CC));
        f32 ka = FovRemap(ClampF(a));
        f32 b = (best - lbl_8055F49C) / (lbl_80563C20.value - lbl_8055F49C);
        f32 k = ka * FovRemap(ClampF(b));
        f32 move = lbl_8055C4D8 * k;
        f32 turn = lbl_8055C4DC * k;
        if (lbl_8055F498) {
            RVec3 j(fn_80129DC4(6, -move, move), fn_80129DC4(6, -move, move), fn_80129DC4(6, -move, move));
            pose->pos.v[0] += j.x;
            pose->pos.v[1] += j.y;
            pose->pos.v[2] += j.z;
        }
        if (lbl_8055C4C8) {
            RVec3 w(fn_80129DC4(6, -turn, turn), fn_80129DC4(6, -turn, turn), fn_80129DC4(6, -turn, turn));
            ShakeMat rot;
            ShakeRot(&rot, w);
            ShakeMul((ShakeMat*)&pose->rot, (ShakeMat*)&pose->rot, &rot);
        }
    }
}

void fn_80274F74(u8* p, const f32* w, f32 fov);
int fn_801769F8(void);

// Trackside camera aimed by a rotation vector, with the same pass-by rumble as fn_80266388.
void fn_80266A0C(CamRec* c, const f32* w, const f32* eye, f32 fov) {
    fn_80274F74((u8*)c, w, fov);
    if (fn_801769F8()) {
        return;
    }
    const f32* rp;
    IdealFollower* trk = ((DeityCar*)((CarBody*)c->car)->src)->Ideal();
    CamXZ xz(c->pose.pos.v[0], c->pose.pos.v[2]);
    CamTrackPos at;
    fn_80136594(&at, trk, &xz);
    f32 camDist = lbl_8055C4CC + (at.seg->dist + at.seg->len * at.t);
    int n = fn_8026A2C8();
    f32 best = lbl_8055F49C;
    u8* near = 0;
    for (int i = 0; i < n; i++) {
        u8* r = fn_801B2EC0(i)->GetCar();
        rp = (const f32*)(r + 0x60);
        CamXZ rxz(rp[0], rp[2]);
        CamTrackPos rat;
        fn_80136594(&rat, trk, &rxz);
        f32 d = (rat.seg->dist + rat.seg->len * rat.t) - camDist;
        if (d < lbl_80563C20.value) {
            u32 lap = trk->il.seg ? trk->il.seg->type : 3;
            d += trk->line->len[lap];
        }
        if (d * d < lbl_8055F49C) {
            f32 sq = PSVECSquareDistance((const Vec*)eye, (const Vec*)rp);
            if (sq < best) {
                best = sq;
                near = r;
            }
        }
    }
    if (near) {
        f32 a = InvLerp(lbl_8055C4D0, lbl_8055C4D4, F32(near, 0x25CC));
        f32 ka = FovRemap(ClampF(a));
        f32 b = (best - lbl_8055F49C) / (lbl_80563C20.value - lbl_8055F49C);
        f32 k = ka * FovRemap(ClampF(b));
        f32 move = lbl_8055C4D8 * k;
        f32 turn = lbl_8055C4DC * k;
        if (lbl_8055F498) {
            RVec3 j(fn_80129DC4(6, -move, move), fn_80129DC4(6, -move, move), fn_80129DC4(6, -move, move));
            c->pose.pos.v[0] += j.x;
            c->pose.pos.v[1] += j.y;
            c->pose.pos.v[2] += j.z;
        }
        if (lbl_8055C4C8) {
            RVec3 w(fn_80129DC4(6, -turn, turn), fn_80129DC4(6, -turn, turn), fn_80129DC4(6, -turn, turn));
            ShakeMat rot;
            ShakeRot(&rot, w);
            ShakeMul((ShakeMat*)&c->pose.rot, (ShakeMat*)&c->pose.rot, &rot);
        }
    }
}


void fn_802670A0(CamView* p, int idx) {
    int n = fn_80178CA4();
    int cols = (n > 1) + 1;
    int rows = (n + cols - 1) / cols;
    int w = lbl_8055CFF0 / cols;
    int h = lbl_8055CFF4 / rows;
    p->w = w;
    p->h = h;
    p->x = w * (idx / rows);
    p->y = h * (idx % rows);
}

void fn_80267130(CamView* p) {
    f32 t = (p->fov - lbl_80563C44) / lbl_80563C48;
    f32 c = (lbl_80563C2C.value < t) ? lbl_80563C2C.value : t;
    c = (c > lbl_80563C20.value) ? c : lbl_80563C20.value;
    f32 k = lbl_80563C4C;
    p->nearZ = lbl_80563C34.value + c * k;
    p->farZ = lbl_8055F484 * (fn_80178CA4() == 1 ? lbl_80563C2C.value : lbl_80563C50);
}

// KFrame: common.h's PlainValMatrix.

static inline void KMulVec(ValVec3* out, const ValVec3* in, const PlainValMatrix* m) {
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

f32 fn_801EA0E8(f32 x, f32 z);  // ground height
extern f32 lbl_8055D020;        // aspect

// Keeps the camera's near plane above the ground: lifts it by the deepest corner (not in modes 17/21).
void fn_802671CC(CamView* p) {
    ValVec3 v;
    ValVec3 w;
    f32 nearZ = p->nearZ;
    f32 s = nearZ * p->tanHalf;
    f32 hx = s / lbl_8055D020;
    f32 hy = ((f32)p->h * s) / (f32)p->w;
    f32 lift = lbl_80563C20.value;
    for (int i = 0; i < 4; i++) {
        f32 cx = (i & 1) ? hx : -hx;
        f32 cy = (i & 2) ? hy : -hy;
        v.x = cx;
        v.y = cy;
        v.z = nearZ;
        KMulVec(&w, &v, (PlainValMatrix*)p);
        w.x += p->pos.v[0];
        w.y += p->pos.v[1];
        w.z += p->pos.v[2];
        f32 d = fn_801EA0E8(w.x, w.z) - w.y;
        lift = lift > d ? lift : d;
    }
    if (p->modeB != 0x11 && p->modeB != 0x15) {
        p->pos.v[1] = p->pos.v[1] + lift;
    }
}

void fn_801769CC(void);
void fn_80276B8C(void);
void fn_80268D94(void);
void fn_80268E00(void);
void fn_80278214(void);
void fn_80267680(void);
void fn_80273354(void);
void fn_8027045C(void);
u32 fn_802090D4(const char* name);  // mixer volume handle
VD* fn_80207848(const char* file, int flags);  // load a sound
void fn_801EEFDC(void* handler, int type);
int fn_8026A554(s32* p);
extern const f32 lbl_80563C54;  // default view angle

// Camera manager start-up: initialises the four camera records, the named/fixed cameras, the crash
/// volume handle and the cheer sound, then resets. phob's start-up passes the view count (the clamp result is built
// in r3 there); nothing here reads it.
void fn_80267420(int /* nViews */) {
    fn_801769CC();
    CamRec* c = lbl_80499080;
    for (int i = 0; i < 4;) {
        memset(c, 0, 0x228);
        c->w0 = i++;
        c->f8 = lbl_80563C20.value;
        c->fC = lbl_80563C20.value;
        c->nextTarget = 0;
        c->target = 0;
        c->nextMode = 10;
        c->mode = 10;
        c->v28.v[0] = lbl_80563C20.value;
        c->v28.v[1] = lbl_80563C2C.value;
        c->v28.v[2] = lbl_80563C20.value;
        c->v34.v[0] = lbl_80563C20.value;
        c->v34.v[1] = lbl_80563C20.value;
        c->v34.v[2] = lbl_80563C20.value;
        c->b138 = 1;
        c->pose.rot.m[0] = lbl_80563C2C.value;
        c->pose.rot.m[1] = lbl_80563C20.value;
        c->pose.rot.m[2] = lbl_80563C20.value;
        c->pose.rot.m[3] = lbl_80563C20.value;
        c->pose.rot.m[4] = lbl_80563C2C.value;
        c->pose.rot.m[5] = lbl_80563C20.value;
        c->pose.rot.m[6] = lbl_80563C20.value;
        c->pose.rot.m[7] = lbl_80563C20.value;
        c->pose.rot.m[8] = lbl_80563C2C.value;
        c->pose.pos.v[0] = lbl_80563C20.value;
        c->pose.pos.v[1] = lbl_80563C20.value;
        c->pose.pos.v[2] = lbl_80563C20.value;
        c->lastPose = c->pose;
        c->vel.v[0] = lbl_80563C20.value;
        c->vel.v[1] = lbl_80563C20.value;
        c->vel.v[2] = lbl_80563C20.value;
        c->carVel.v[0] = lbl_80563C20.value;
        c->carVel.v[1] = lbl_80563C20.value;
        c->carVel.v[2] = lbl_80563C20.value;
        c->poseF0.rot.m[0] = lbl_80563C2C.value;
        c->poseF0.rot.m[1] = lbl_80563C20.value;
        c->poseF0.rot.m[2] = lbl_80563C20.value;
        c->poseF0.rot.m[3] = lbl_80563C20.value;
        c->poseF0.rot.m[4] = lbl_80563C2C.value;
        c->poseF0.rot.m[5] = lbl_80563C20.value;
        c->poseF0.rot.m[6] = lbl_80563C20.value;
        c->poseF0.rot.m[7] = lbl_80563C20.value;
        c->poseF0.rot.m[8] = lbl_80563C2C.value;
        c->poseF0.pos.v[0] = lbl_80563C20.value;
        c->poseF0.pos.v[1] = lbl_80563C20.value;
        c->poseF0.pos.v[2] = lbl_80563C20.value;
        c->carFrame.rot.m[0] = lbl_80563C2C.value;
        c->carFrame.rot.m[1] = lbl_80563C20.value;
        c->carFrame.rot.m[2] = lbl_80563C20.value;
        c->carFrame.rot.m[3] = lbl_80563C20.value;
        c->carFrame.rot.m[4] = lbl_80563C2C.value;
        c->carFrame.rot.m[5] = lbl_80563C20.value;
        c->carFrame.rot.m[6] = lbl_80563C20.value;
        c->carFrame.rot.m[7] = lbl_80563C20.value;
        c->carFrame.rot.m[8] = lbl_80563C2C.value;
        c->carFrame.pos.v[0] = lbl_80563C20.value;
        c->carFrame.pos.v[1] = lbl_80563C20.value;
        c->carFrame.pos.v[2] = lbl_80563C20.value;
        c->car = 0;
        c->lastCar = 0;
        c->w4 = 10;
        c->w1C = 10;
        c->saved.rot.m[0] = lbl_80563C2C.value;
        c->saved.rot.m[1] = lbl_80563C20.value;
        c->saved.rot.m[2] = lbl_80563C20.value;
        c->saved.rot.m[3] = lbl_80563C20.value;
        c->saved.rot.m[4] = lbl_80563C2C.value;
        c->saved.rot.m[5] = lbl_80563C20.value;
        c->saved.rot.m[6] = lbl_80563C20.value;
        c->saved.rot.m[7] = lbl_80563C20.value;
        c->saved.rot.m[8] = lbl_80563C2C.value;
        c->saved.pos.v[0] = lbl_80563C20.value;
        c->saved.pos.v[1] = lbl_80563C20.value;
        c->saved.pos.v[2] = lbl_80563C20.value;
        c->f1D4 = lbl_80563C54;
        c++;
    }
    fn_80276B8C();
    fn_80268D94();
    fn_80268E00();
    fn_80278214();
    fn_80273354();
    fn_8027045C();
    lbl_8055F494 = fn_802090D4("crash_vol");
    lbl_8055F490 = fn_80207848("cheer3.sfx", 0);
    fn_801EEFDC((void*)fn_8026A554, 13);
    fn_80267680();
}

void fn_802733C8(void);
int fn_801796B4(void);
void fn_80276BE4(u8* pose, u8* fov);
void fn_80209190(u32 handle, f32 vol);
void fn_80270484(void);
void fn_80268B94(s32 v);
extern f32 lbl_8055F48C;
extern const f32 lbl_80563C54;  // default view angle
extern const f32 lbl_80563C58;
extern const f32 lbl_80563C5C;
extern const f32 lbl_80563C60;
extern const f32 lbl_80563C64;

// Re-reads the saved pose (the identity passed in is not used).
static inline void ResetSavedPose(u8* c, const CamPose& pose) { fn_80276BE4(c + 0x1A4, c + 0x1D4); }

// Resets the four players' camera records, the shake table, the split-screen rects and the shake levels.
#pragma push
#pragma opt_lifetimes off
void fn_80267680(void) {
    fn_802733C8();
    CamRec* c = lbl_80499080;
    for (int i = 0; i < 4; i++) {
        c->f8 = lbl_80563C20.value;
        c->fC = lbl_80563C20.value;
        c->lastCar = 0;
        c->lastFrame = -1;
        c->b138 = 1;
        c->b139 = 1;
        c->f13C = lbl_80563C20.value;
        c->eye.v[0] = lbl_80563C20.value;
        c->eye.v[1] = lbl_80563C20.value;
        c->eye.v[2] = lbl_80563C20.value;
        c->v28.v[0] = lbl_80563C20.value;
        c->v28.v[1] = lbl_80563C2C.value;
        c->v28.v[2] = lbl_80563C20.value;
        c->w148 = 0;
        c->f194 = lbl_80563C54;
        fn_80265E08((u8*)&c->w160);
        fn_80266150((u8*)&c->w178);
        c->f158 = lbl_80563C58;
        c->f15C = i;
        c->f198 = lbl_80563C20.value;
        c->f19C = lbl_80563C20.value;
        c->w14C = -1;
        c->w150 = -1;
        if (!fn_801796B4()) {
            CamPose id;
            id.rot.m[0] = lbl_80563C2C.value;
            id.rot.m[1] = lbl_80563C20.value;
            id.rot.m[2] = lbl_80563C20.value;
            id.rot.m[3] = lbl_80563C20.value;
            id.rot.m[4] = lbl_80563C2C.value;
            id.rot.m[5] = lbl_80563C20.value;
            id.rot.m[6] = lbl_80563C20.value;
            id.rot.m[7] = lbl_80563C20.value;
            id.rot.m[8] = lbl_80563C2C.value;
            id.pos.v[0] = lbl_80563C20.value;
            id.pos.v[1] = lbl_80563C20.value;
            id.pos.v[2] = lbl_80563C20.value;
            fn_80276BE4((u8*)&c->saved, (u8*)&c->f1D4);
        }
        fn_801E9DC4((u8*)&c->h1EC);
        c->prev.pose.rot.m[0] = lbl_80563C2C.value;
        c->prev.pose.rot.m[1] = lbl_80563C20.value;
        c->prev.pose.rot.m[2] = lbl_80563C20.value;
        c->prev.pose.rot.m[3] = lbl_80563C20.value;
        c->prev.pose.rot.m[4] = lbl_80563C2C.value;
        c->prev.pose.rot.m[5] = lbl_80563C20.value;
        c->prev.pose.rot.m[6] = lbl_80563C20.value;
        c->prev.pose.rot.m[7] = lbl_80563C20.value;
        c->prev.pose.rot.m[8] = lbl_80563C2C.value;
        c->prev.pose.pos.v[0] = lbl_80563C20.value;
        c->prev.pose.pos.v[1] = lbl_80563C20.value;
        c->prev.pose.pos.v[2] = lbl_80563C20.value;
        c->prev.fov = lbl_80563C5C;
        c->prev.start = lbl_80563C60;
        c++;
    }
    lbl_8055F488 = 0;
    lbl_8055F48C = lbl_80563C64;
    fn_80209190(lbl_8055F494, lbl_80563C2C.value);
    fn_80265E2C();
    fn_80268B94(0);
    fn_80270484();
    f32* q = lbl_80499980;
    for (int i = 0; i < 8; i++) {
        *q++ = lbl_80563C20.value;
    }
}
#pragma pop

void fn_8026785C(void) {
    EventUnInstallHandler((void*)fn_8026A554);
    fn_80278288();
    fn_802733F0();
    if (lbl_8055F490) {
        delete lbl_8055F490;
        lbl_8055F490 = 0;
    }
    fn_80209110(&lbl_8055F494);
}

u8* fn_802678C0(int i) { return fn_80267EE4(i) + 0x58; }

u8* fn_802678E4(int i) { return fn_80267EE4(i); }

// What the sound listener follows: the target car and this camera's pose.
struct ListenerRef {
    s32 id;     // offset 0x0
    void* mtx;  // offset 0x4
    void* vel;  // offset 0x8
};

void fn_8020655C(const ListenerRef& l, int i);
int fn_801C5B68(CamView* view, int idx);
void fn_8026B768(int target, int idx);
int fn_8026B734(int target, f32* out);
u8* fn_8017960C(int car);
int fn_801769F8(void);
f32 fn_801B8950(int car);
double tan(double x);
double fabs(double x);
void fn_802693F4(CamRec* c, u8* own, int idx);
void fn_80268E94(CamRec* c, int idx);
extern s32 lbl_8055E3C4;  // frame counter
extern f32 lbl_8055E460;  // tick alpha
extern u8 lbl_8055F480;
extern const f32 lbl_80563C68;  // default fov
extern const f32 lbl_80563C70;
extern const f32 lbl_80563C74;
extern const f32 lbl_80563C78;
extern const f32 lbl_80563C7C;  // shake decay per frame

// Per-frame camera update for player idx: run the current camera type, blend from last frame's pose by the
// tick alpha, and fill the view (pose, fov, rumble, flags). The shake levels decay unless paused.
#pragma push
#pragma optimization_level 3
void fn_802678E8(CamView* view, int idx) {
    fn_802670A0(view, idx);
    if (fn_801C5B68(view, idx)) {
        fn_80267130(view);
        return;
    }
    CamRec* c = (CamRec*)fn_80267EE4(idx);
    c->f1A0 = lbl_80563C28.value;
    fn_8026B768(c->target, idx);
    if (lbl_8055E3C4 != c->lastFrame) {
        c->lastPose = c->pose;
        c->lastFrame = lbl_8055E3C4;
    }
    if (c->nextMode < 0 || c->nextMode >= 0x22) {
        c->nextMode = 4;
    }
    view->modeA = c->w1C;
    lbl_8055F480 = 0;
    c->target = c->nextTarget;
    c->mode = c->nextMode;
    c->fov = lbl_80563C68;
    c->car = fn_8017960C(c->target);
    f32 pos[3];
    pos[0] = c->pose.pos.v[0];
    pos[1] = c->pose.pos.v[1];
    pos[2] = c->pose.pos.v[2];
    u8 own = 0;
    int flags;
    if (c->car) {
        fn_802693F4(c, &own, idx);
        flags = fn_80267EE0(c->mode);
        if (U8(c->car, 0x260C)) {
            flags |= 0x20;
        }
    } else {
        flags = fn_80267EE0(c->mode);
    }
    fn_80268E94(c, idx);
    ListenerRef l = {c->target, &c->pose, &c->vel};
    fn_8020655C(l, idx);
    if (c->mode != 0x11) {
        CamPose blend;
        f32 k = lbl_8055E460;
        f32* d = (f32*)&blend;
        const f32* a = (const f32*)&c->lastPose;
        const f32* b = (const f32*)&c->pose;
        for (int i = 0; i < 12; i++) {
            f32 y = b[i];
            f32 x = a[i];
            d[i] = x + k * (y - x);
        }
        *(CamPose*)view = blend;
    } else {
        *(CamPose*)view = c->pose;
    }
    view->fov = c->fov;
    view->tanHalf = tan(lbl_80563C6C * c->fov);
    f32 rumble;
    f32 r;
    if (!fn_801769F8()) {
        if (fn_8026B734(c->target, &r)) {
            rumble = r;
            goto done;
        } else {
            bool chase = false;
            int m = c->mode;
            if ((m >= 0 && m < 10) || m == 0xE) {
                chase = true;
            }
            if (chase) {
                r = fn_801B8950(c->target);
                f32 k = ((f32)__fabs(PSVECDotProduct((const Vec*)&c->pose.rot.m[6], (const Vec*)&c->carFrame.rot.m[6])) - lbl_80563C70) / lbl_80563C74;
                k = (lbl_80563C2C.value < k) ? lbl_80563C2C.value : k;
                k = (k > lbl_80563C20.value) ? k : lbl_80563C20.value;
                rumble = r * (lbl_80563C2C.value + k * lbl_80563C78);
                goto done;
            }
        }
    }
    rumble = lbl_80563C20.value;
done:
    view->rumble = rumble;
    view->f54 = c->f1A0;
    if (c->b138) {
        flags |= 0x10;
    }
    c->b138 = 0;
    c->b139 = 0;
    view->modeB = c->mode;
    view->car = c->target;
    view->flags = flags;
    if (own) {
        view->carId = c->h1EC;
    } else {
        view->carId = ((CarBody*)c->car)->hint[0];
    }
    if (!fn_801769F8()) {
        for (int i = 0; i < 8; i++) {
            f32 v = lbl_80499980[i] - lbl_80563C7C;
            lbl_80499980[i] = (lbl_80563C20.value > v) ? lbl_80563C20.value : v;
        }
    }
    fn_80267130(view);
    fn_802671CC(view);
}
#pragma pop

void fn_80267E40(u32 id, f32 strength) {
    f32 now = lbl_8055E3C8;
    for (int i = 0; i < 4; i++) {
        CamRec* c = (CamRec*)fn_80267EE4(i);
        if ((u32)c->car == id && strength > c->f154 * (lbl_80563C2C.value - (now - c->f158) / lbl_80563C6C)) {
            c->f158 = now;
            c->f154 = strength;
            return;
        }
    }
}

int fn_80267EE0(int mode) { return fn_802734F0(mode); }

u8* fn_80267EE4(int i) { return (u8*)lbl_80499080 + i * 0x228; }

}  // extern "C"
int fn_8000D75C(void);
extern "C" {
u32 fn_802E2C20(void);
void fn_801769EC(void);
void fn_8022FF10(CamMat3* out, const f32* euler);
extern const f32 lbl_80563C64;  // "no crash" time
extern const f32 lbl_80563C80;  // ms to s
extern const f32 lbl_80563C84;  // fade back in after
extern const f32 lbl_80563C88;  // 360
extern f32 lbl_8055F48C;        // crash start time

// Crash camera: for three seconds after a crash, fade the crash volume out and back in (the cheer the
// other way) and swing the camera once round a pivot ahead of it; then restore the volume and resume.
void fn_80267EF8(u8* p) {
    f32 t = lbl_80563C80 * (f32)(s32)fn_802E2C20() - lbl_8055F48C;
    if (t > lbl_80563C34.value) {
        lbl_8055F488 = 0;
        lbl_8055F48C = lbl_80563C64;
        fn_80209190(lbl_8055F494, lbl_80563C2C.value);
        if (!fn_8000D75C()) {
            fn_801769EC();
        }
        return;
    }
    f32 vol;
    f32 ang;
    f32 u = t / lbl_80563C34.value;
    vol = lbl_80563C20.value;
    if (t < lbl_80563C6C) {
        f32 fadeIn = t / lbl_80563C6C;
        vol = lbl_80563C2C.value + fadeIn * lbl_80563C28.value;
    } else if (t > lbl_80563C84) {
        f32 fadeOut = (t - lbl_80563C84) / lbl_80563C6C;
        vol += fadeOut * lbl_80563C2C.value;
    }
    fn_80209190(lbl_8055F494, vol);
    u8* cheer = (u8*)lbl_8055F490;
    f32 cv = lbl_80563C2C.value - vol;
    if (cv != F32(cheer, 0x8)) {
        U8(cheer, 0x4D) = 1;
        F32(cheer, 0x8) = cv;
    }
    f32 ease = ShakeEase(u);
    ang = lbl_80563C24.value * (lbl_80563C20.value + ease * lbl_80563C88);
    CamPose cam;
    CamPose car;
    cam = *(CamPose*)(p + 0x58);
    car = *(CamPose*)(p + 0xC0);
    f32 dist = PSVECDistance((const Vec*)car.pos.v, (const Vec*)cam.pos.v);
    CamPose f;
    f = cam;
    f.pos.v[0] += f.rot.m[6] * dist;
    f.pos.v[1] += f.rot.m[7] * dist;
    f.pos.v[2] += f.rot.m[8] * dist;
    f32 euler[3];
    euler[0] = lbl_80563C20.value;
    euler[1] = ang;
    euler[2] = lbl_80563C20.value;
    CamMat3 rot;
    fn_8022FF10(&rot, euler);
    Mat3Mul((Mat3*)&f.rot, (Mat3*)&f.rot, (Mat3*)&rot);
    f32 back = -dist;
    f.pos.v[0] += f.rot.m[6] * back;
    f.pos.v[1] += f.rot.m[7] * back;
    f.pos.v[2] += f.rot.m[8] * back;
    *(CamPose*)(p + 0x58) = f;
}

void fn_802682B0(u8* p) {
    int row;
    int* rowp = &row;
    if (U8(p, 0x138)) {
        // MATCHING-AID: an index the compiler cannot fold (decomp-permuter) gives the target's stfsu; row loops,
        // a row pointer and straight pointer stores were tried 2026-10-28 (all 5 rows off).
        lbl_80499050.m[row = 0][*rowp] = lbl_80563C2C.value;
        lbl_80499050.m[0][1] = lbl_80563C20.value;
        lbl_80499050.m[0][2] = lbl_80563C20.value;
        lbl_80499050.m[0][3] = lbl_80563C20.value;
        lbl_80499050.m[1][0] = lbl_80563C2C.value;
        lbl_80499050.m[1][1] = lbl_80563C20.value;
        lbl_80499050.m[1][2] = lbl_80563C20.value;
        lbl_80499050.m[1][3] = lbl_80563C20.value;
        lbl_80499050.m[2][0] = lbl_80563C2C.value;
        lbl_80499050.m[2][1] = lbl_80563C20.value;
        lbl_80499050.m[2][2] = lbl_80563C20.value;
        lbl_80499050.m[2][3] = lbl_80563C20.value;
    }
    fn_801BFD34(p + 0x7C, &lbl_80499050.m[2][1], p + 0xC0);
    Mat3Mul((Mat3*)(p + 0x58), (Mat3*)&lbl_80499050, (Mat3*)(p + 0xC0));
    F32(p, 0xBC) = lbl_8055F47C;
}

void fn_8026E850(f32* pos, const f32* target, u8* cam, const f32* up);
int fn_801E9E78(void* ground, ValVec3* p, ValVec3* n, s32* hit);
extern f32 lbl_8055C4F0;  // mount blend time (aim, up, fov)
extern f32 lbl_8055C4F4;  // mount blend time (position)

}  // extern "C"

// A fixed camera mount: offset and look point in the car frame, field of view, enabled.
struct CamMount {
    f32 off[3];  // offset 0x0
    f32 look[3]; // offset 0xC
    f32 fov;     // offset 0x18
    u8 on;       // offset 0x1C
};

extern const f32 lbl_80563CF4;  // 82.5 deg
extern const f32 lbl_80563CF8;  // 30 * 30
ModuleTag lbl_8055F470;
UnkStaticFFFF lbl_8055F478;
f32 lbl_8055F47C = lbl_80563CF4;  // default field of view
u8 lbl_8055F480;
f32 lbl_8055F484;
u8 lbl_8055F488;
f32 lbl_8055F48C;
VD* lbl_8055F490;
u32 lbl_8055F494;
u8 lbl_8055F498;
Mtx34 lbl_80499050;
CamRec lbl_80499080[4];
CamPose lbl_80499920;
CamPose lbl_80499950;
f32 lbl_80499980[8];
ShakeEntry lbl_804999A0[100];
s32 lbl_80499E50[4];
f32 lbl_8055F49C = lbl_80563CF8;
s32 lbl_8055F4A0;
s32 lbl_8055F4A4;
u8 lbl_8055F4A8[8];
extern const f32 lbl_80563CFC;  // 42.5 deg
extern const f32 lbl_80563D00;  // 55 deg
extern const f32 lbl_80563D04;  // 60 deg
extern const CamMount lbl_803467E8[3] = {
    {{0.0f, 0.7f, 0.0f}, {0.0f, 0.8f, 9.0f}, lbl_80563CFC, 1},
    {{0.0f, -0.5f, -30.5f}, {0.0f, 2.1f, 1.0f}, lbl_80563D00, 1},
    {{0.1f, 1.1f, -0.2f}, {-9.5f, 2.5f, 4.2f}, lbl_80563D04, 0},
};

extern "C" {
// Mount camera points averaged over the ideal-line frames: two points in the frame, and its up vector.
struct MountView {
    ValVec3 a;   // offset 0x0
    ValVec3 b;   // offset 0xC
    ValVec3 up;  // offset 0x18
};

// Fixed mount camera on the ideal line: for the current and the next mount, place both mount points in
// the line's frame (an average of two samples), then blend from one mount to the other over time.

// d += v * k. Returns d: chained calls reload through the returned reference, which the optimizer cannot forward
// (the target's frsp right after each stfs).
static inline ValVec3& CamMad(ValVec3& d, const f32* v, f32 k) {
    d.x += v[0] * k;
    d.y += v[1] * k;
    d.z += v[2] * k;
    return d;
}
// A vector built from three values: the arguments evaluate right to left, so z loads first.
// KV3: rvec.h's RVec3.
// Helper name unproven: arguments read right to left, stores go x, y, z.
static inline void SetM3(f32* o, f32 x, f32 y, f32 z) {
    o[0] = x;
    o[1] = y;
    o[2] = z;
}


void fn_80268358(CamRec* c) {
    RVec3 mv[4] = {RVec3(lbl_803467E8[lbl_8055F4A0].off[0], lbl_803467E8[lbl_8055F4A0].off[1], lbl_803467E8[lbl_8055F4A0].off[2]), RVec3(lbl_803467E8[lbl_8055F4A0].look[0], lbl_803467E8[lbl_8055F4A0].look[1], lbl_803467E8[lbl_8055F4A0].look[2]), RVec3(lbl_803467E8[lbl_8055F4A4].off[0], lbl_803467E8[lbl_8055F4A4].off[1], lbl_803467E8[lbl_8055F4A4].off[2]), RVec3(lbl_803467E8[lbl_8055F4A4].look[0], lbl_803467E8[lbl_8055F4A4].look[1], lbl_803467E8[lbl_8055F4A4].look[2])};
    f32 (*m)[6] = (f32(*)[6])mv;
    MountView out[2];
    f32 zero;
    f32 t;
    f32 half = lbl_80563C6C;
    zero = lbl_80563C20.value;
    int wo = 0;
    for (int i = 0; i < 2; i++, wo += sizeof(out[0])) {
        int j;
        int k;
        MountView* o = (MountView*)((u8*)out + wo);
        o->a.x = zero;
        o->a.y = zero;
        o->a.z = zero;
        o->b.x = zero;
        o->b.y = zero;
        o->b.z = zero;
        o->up.x = zero;
        o->up.y = zero;
        o->up.z = zero;
        PlainValMatrix fr[2];
        for (j = 0; j < 2; j++) {
            f32* p1;
            s32 hit;
            f32* p0;
            f32 z;
            f32 x;
            PlainValMatrix* r;
            IdealFollower f(0, 0);
            p0 = (f32*)f.il.seg;
            p1 = (f32*)f.il.seg->next;
            x = LerpT(f.il.t, p0[0], p1[0]);
            z = LerpT(f.il.t, p0[1], p1[1]);
            ValVec3 v;
            CamXZ xz(x, z);
            v.x = xz.x;
            v.y = lbl_80563C20.value;
            v.z = xz.z;
            ValVec3 n;
            hit = 0;
            fn_801E9E78(&lbl_8055F4A8, &v, &n, &hit);
            r = &fr[j];
            SetM3(&r->basis[2].x, f.dir.x, lbl_80563C20.value, f.dir.y);
            PSVECNormalize((const Vec*)&r->basis[2].x, (Vec*)&r->basis[2].x);
            PSVECCrossProduct((const Vec*)&n.x, (const Vec*)&r->basis[2].x, (Vec*)&r->basis[0].x);
            PSVECNormalize((const Vec*)&r->basis[0].x, (Vec*)&r->basis[0].x);
            PSVECCrossProduct((const Vec*)&r->basis[2].x, (const Vec*)&r->basis[0].x, (Vec*)&r->basis[1].x);
            PSVECNormalize((const Vec*)&r->basis[1].x, (Vec*)&r->basis[1].x);
            r->origin = v;
        }
        ValVec3 in = *(ValVec3*)&m[i][0];
        for (int k = 0; k < 2; k++) {
            ValVec3 res;
            KMulVec(&res, &in, &fr[k]);
            res.x += fr[k].origin.x;
            res.y += fr[k].origin.y;
            res.z += fr[k].origin.z;
            o->a.x += res.x * half;
            o->a.y += res.y * half;
            o->a.z += res.z * half;
        }
        ValVec3 in2 = *(ValVec3*)&m[i][3];
        for (k = 0; k < 2; k++) {
            ValVec3 res2;
            KMulVec(&res2, &in2, &fr[k]);
            res2.x += fr[k].origin.x;
            res2.y += fr[k].origin.y;
            res2.z += fr[k].origin.z;
            o->b.x += res2.x * half;
            o->b.y += res2.y * half;
            o->b.z += res2.z * half;
        }
        CamMad(CamMad(o->up, &fr[0].basis[1].x, half), &fr[1].basis[1].x, half);
    }
    t = lbl_8055E3C8 - c->mount.start;
    f32 ka;
    ka = lbl_803467E8[lbl_8055F4A0].on ? ShakeEase(t / lbl_8055C4F0) : t / lbl_8055C4F0;
    f32 kb;
    kb = lbl_803467E8[lbl_8055F4A0].on ? ShakeEase(t / lbl_8055C4F4) : t / lbl_8055C4F4;
    RVec3 bl(Lerp(out[0].b.x, out[1].b.x, ka), Lerp(out[0].b.y, out[1].b.y, ka), Lerp(out[0].b.z, out[1].b.z, ka));
    RVec3 al(Lerp(out[0].a.x, out[1].a.x, kb), Lerp(out[0].a.y, out[1].a.y, kb), Lerp(out[0].a.z, out[1].a.z, kb));
    RVec3 up(Lerp(out[0].up.x, out[1].up.x, ka), Lerp(out[0].up.y, out[1].up.y, ka), Lerp(out[0].up.z, out[1].up.z, ka));
    PSVECNormalize((const Vec*)&up.x, (Vec*)&up.x);
    c->fov = Lerp(lbl_803467E8[lbl_8055F4A0].fov, lbl_803467E8[lbl_8055F4A4].fov, ka);
    fn_8026E850(&bl.x, &al.x, (u8*)c, &up.x);
}

void fn_80268B94(s32 v) {
    lbl_8055F4A0 = v;
    lbl_8055F4A4 = v;
    fn_801E9DC4(&lbl_8055F4A8);
}

// Mode set as a bit mask over modes 4..23 (the original tests a mask, not a switch).
#define CAM_MODE_BIT(m) (1 << ((m) - 4))

int fn_80268BA4(int mode) {
    int r = 0;
    u32 i = mode - 4;
    if (i <= 19 && ((1 << i) & (CAM_MODE_BIT(4) | CAM_MODE_BIT(5) | CAM_MODE_BIT(6) | CAM_MODE_BIT(8) | CAM_MODE_BIT(9) | CAM_MODE_BIT(23)))) {
        r = 1;
    }
    return r;
}

double sqrt(double x);
double sin(double x);
double cos(double x);
void fn_801EA140(f32* pos);  // drop onto the ground
extern const f32 lbl_80563C8C;
extern const f32 lbl_80563C90;
extern const f32 lbl_80563C94;  // height above the ground

struct CamPt {
    f32 x, y;
};

static inline f32 Dot4_hm(f32 ax, f32 ay, f32 bx, f32 by) { return ax * bx + ay * by; }  /* Helper name unproven */
static inline void ScalePerpendicular(CamPt* d, f32 inv) {
    const f32 x = d->x * inv;
    d->x = d->y * inv;
    d->y = -x;
}

// Builds a fixed camera frame (rows right/up/forward + position) beside the track at path position
// (seg, t): offset sideways by the segment width, set on the ground, looking `angle` off the track.
void fn_80268BD4(u8* rec, const IdealPoint* seg, f32 t, f32 angle) {
    f32 inv;
    const IdealPoint* next;
    next = seg->next;
    f32 sn;
    CamPt pos;
    CamPt d;
    CamPt pt;
    pos.x = LerpT(t, seg->pos.x, next->pos.x);
    pos.y = LerpT(t, seg->pos.y, next->pos.y);
    d.x = LerpT(t, seg->dir.x, next->dir.x);
    d.y = LerpT(t, seg->dir.y, next->dir.y);
    inv = lbl_80563C2C.value / (f32)sqrt(Dot4_hm(d.x, d.y, d.x, d.y));
    f32 px;
    f32 c;
    f32 off;
    ScalePerpendicular(&d, inv);
    off = lbl_80563C8C * seg->width - lbl_80563C90;
    pt.x = pos.x + d.x * off;
    pt.y = pos.y + d.y * off;
    F32(rec, 0x28) = lbl_80563C20.value;
    F32(rec, 0x24) = pt.x;
    F32(rec, 0x2C) = pt.y;
    fn_801EA140((f32*)(rec + 0x24));
    px = d.x;
    F32(rec, 0x28) = F32(rec, 0x28) + lbl_80563C94;
    c = (f32)cos(angle);
    sn = (f32)sin(angle);
    d.x = d.x * c - d.y * sn;
    d.y = px * sn + d.y * c;
    F32(rec, 0xC) = lbl_80563C20.value;
    F32(rec, 0x10) = lbl_80563C2C.value;
    F32(rec, 0x14) = lbl_80563C20.value;
    F32(rec, 0x18) = d.x;
    F32(rec, 0x1C) = lbl_80563C20.value;
    F32(rec, 0x20) = d.y;
    PSVECCrossProduct((const Vec*)((f32*)(0xC + rec)), (const Vec*)((f32*)(0x18 + rec)), (Vec*)((f32*)rec));
}

void fn_80268D94(void) {
    IdealFollower f(0, 0);
    fn_80268BD4((u8*)&lbl_80499920, f.line->points, lbl_80563C20.value, lbl_80563C98);
}

void fn_80268E00(void) {
    IdealFollowerData f;
    const ILPos& at = fn_80136468(&f, fn_80134E34(__ct__13IdealFollowerFiP10CenterLine(&f, 0, 0), lbl_80563C28.value));
    ILPos& pos = const_cast<ILPos&>(at);  // the returned temporary itself (not a const object)
    if (!pos.seg) {
        pos.seg = f.line->points;
    }
    fn_80268BD4((u8*)&lbl_80499950, pos.seg, pos.t, lbl_80563C9C);
    if (f.buf) {
        delete[] f.buf;
    }
    f.buf = 0;
    __dt__12LineFollowerFv(&f, 0);
}


// One axis of the hit shake: a sine wave of the given phase plus jitter, scaled by the amplitude.
static inline f32 ShakeAxis(f32 amp, f32 phase, f32 twoPi) {
    f32 s = (f32)sin(phase * twoPi);
    f32 r = fn_80129DC4(0, lbl_80563CB0, lbl_80563CB4);
    return amp * (r + s * lbl_80563CB8);
}

// Hit shake for chase cameras: for half a second after a bump the camera wobbles about all three axes
// (a sine wave per axis plus jitter), easing out with the time since the hit.
void fn_80268E94(CamRec* c, int idx) {
    f32 amp, twoPi, t;
    if (!fn_8026B0C0(idx) && !fn_801769F8()) {
        bool chase = false;
        int m = c->mode;
        if ((m >= 0 && m < 10) || m == 0xE) {
            chase = true;
        }
        if (chase) {
            t = lbl_8055E3C8 - c->f158;
            if (t < lbl_80563C6C) {
                amp = c->f154 * ShakeEase((lbl_80563C6C - t) / lbl_80563C6C);
                if (amp > lbl_80563CA0) {
                    twoPi = lbl_80563CA4;
                    RVec3 w(ShakeAxis(amp, lbl_80563CAC * t, twoPi), ShakeAxis(amp, lbl_80563C50 * (lbl_80563CAC * t), twoPi),
                               ShakeAxis(amp, lbl_80563CA8 * (lbl_80563CAC * t), twoPi));
                    ShakeMat rot;
                    ShakeRot(&rot, w);
                    ShakeMul((ShakeMat*)&c->pose.rot, (ShakeMat*)&c->pose.rot, &rot);
                }
            }
        }
    }
}

void fn_802733F4(CamRec* c);
void fn_80273BD0(CamRec* c);
void fn_8027445C(CamRec* c);
void fn_80274268(CamRec* c);
void fn_802704D4(int type, CamRec* c);
int fn_8026A698(CamRec* c, int idx);
void fn_80274460(CamRec* c);
void fn_8027847C(CamRec* c);
void fn_802704E4(CamRec* c);
void fn_80274660(int type, CamRec* c);
void fn_8026DAA8(CamRec* c);
void fn_80275FC0(CamRec* c);
void fn_80276044(CamRec* c);
void fn_80276054(CamRec* c);
u32 fn_801769A8(void);
void fn_80273490(int type, CamRec* c);
void camera_car(CamRec* c, int which);
void fn_80275B9C(CamRec* rec, f32 a, f32 b, f32 c);
void fn_80277A14(CamRec* c, int idx);
void fn_80278360(CamRec* c);
void fn_802E317C(const char* fmt, ...);
int fn_80268BA4(int mode);
void fn_802682B0(u8* p);
void fn_80268358(CamRec* c);
extern f32 lbl_8055C4B0;
extern const f32 lbl_80563CBC;  // rumble per speed
extern const f32 lbl_80563CC0;
extern const f32 lbl_80563CC4;
extern const f32 lbl_80563CC8;
extern const f32 lbl_80563CCC;
extern const f32 lbl_80563CD0;
extern const f32 lbl_80563CD4;
extern const f32 lbl_80563CD8;
extern const f32 lbl_80563CDC;
extern const f32 lbl_80563CE0;
extern const f32 lbl_80563CE4;
extern const f32 lbl_80563CE8;
extern const f32 lbl_80563CEC;
extern const f32 lbl_80563CF0;  // smallest bump

// Camera rumble target for a car speed: rumble per speed, capped at 1.
static inline f32 RumbleForSpeed(f32 speed) {
    f32 r = lbl_80563CBC * speed;
    return (lbl_80563C2C.value < r) ? lbl_80563C2C.value : r;
}

// Runs camera type c->mode for one frame: follows the car's frame, flags cuts (a mode change other than
// between 4 and 25, or a new car), smooths the rumble, dispatches on the type and finally arms the hit
// shake from the car's impact speed. Sets *own for types that fill the sound pose themselves.
void fn_802693F4(CamRec* c, u8* own, int idx) {
    u8* car = c->car;
    const ValVec3* off = (const ValVec3*)(car + 0x30);
    c->carFrame = *(CamPose*)(car + 0x3C);
    c->carVel.v[0] = F32(car, 0x118);
    c->carVel.v[1] = F32(car, 0x11C);
    c->carVel.v[2] = F32(car, 0x120);
    c->vel.v[0] = c->carVel.v[0];
    c->vel.v[1] = c->carVel.v[1];
    c->vel.v[2] = c->carVel.v[2];
    ValVec3 d;
    KMulVec(&d, off, (const PlainValMatrix*)&c->carFrame);
    c->carFrame.pos.v[0] += d.x;
    c->carFrame.pos.v[1] += d.y;
    c->carFrame.pos.v[2] += d.z;
    if (c->mode != c->w4 && !(c->mode == 4 && c->w4 == 0x19) && !(c->w4 == 4 && c->mode == 0x19)) {
        c->b138 = 1;
    }
    fn_802733F4(c);
    if (c->lastCar != c->car) {
        c->b139 = 1;
        c->lastCar = c->car;
        c->b138 = 1;
    }
    if (c->b138) {
        fn_801E9DC4(&c->h1EC);
    }
    c->eye.v[0] = c->carFrame.pos.v[0];
    c->eye.v[1] = c->carFrame.pos.v[1];
    c->eye.v[2] = c->carFrame.pos.v[2];
    if (!fn_801769F8()) {
        f32 r = RumbleForSpeed(F32(c->car, 0x25D4));
        if (!c->b138 && !c->b139) {
            c->f13C = c->f13C + lbl_80563CA0 * (r - c->f13C);
        } else {
            c->f13C = r;
        }
    }
    if (!fn_8026A698(c, idx)) {
        switch (c->mode) {
        case 0:
            fn_80273BD0(c);
            break;
        case 1:
            fn_8027445C(c);
            break;
        case 2:
            fn_80274268(c);
            break;
        case 3:
            fn_80274460(c);
            break;
        case 0x19:
            fn_8027847C(c);
            break;
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 0x17:
            fn_802704D4(c->mode, c);
            if (lbl_8055F488) {
                fn_80267EF8((u8*)c);
            }
            break;
        case 9:
            fn_802704E4(c);
            break;
        case 0xA:
        case 0x10:
            fn_80274660(c->mode, c);
            break;
        case 0x11:
            fn_8026DAA8(c);
            *own = 1;
            break;
        case 0x12:
            fn_802682B0((u8*)c);
            *own = 1;
            break;
        case 0x15:
        {
            fn_80275FC0(c);
            f32 zero = lbl_80563C20.value;
            c->vel.v[0] = zero;
            c->vel.v[1] = zero;
            c->vel.v[2] = zero;
            *own = 1;
            break;
        }
        case 0x21:
            fn_80276044(c);
            *own = 1;
            break;
        case 0x16:
            fn_80276054(c);
            break;
        case 0x18:
            if ((fn_801769A8() >> 8) & 1) {
                fn_80268358(c);
            } else {
                fn_8027445C(c);
            }
            break;
        case 0xB:
            fn_80273490(c->mode, c);
            *own = 1;
            break;
        case 0xC:
            camera_car(c, 1);
            break;
        case 0xD:
            camera_car(c, 2);
            break;
        case 0xE:
            camera_car(c, 3);
            break;
        case 0xF:
            camera_car(c, 4);
            break;
        case 0x13:
            c->pose = lbl_80499920;
            break;
        case 0x14:
            c->pose = lbl_80499950;
            break;
        case 0x1A:
            fn_80275B9C(c, lbl_80563CC0, lbl_80563CC4, lbl_80563CC8);
            break;
        case 0x1E:
        {
            f32 speed = lbl_80563CCC + lbl_80563C20.value * lbl_80563CD0;
            f32 height = lbl_80563CD4 + lbl_80563C20.value * lbl_80563CD8;
            f32 radius = lbl_80563CDC + lbl_80563C20.value * lbl_80563CE0;
            fn_80275B9C(c, radius, height, speed);
            break;
        }
        case 0x1B:
            fn_802704D4(7, c);
            break;
        case 0x1C:
            fn_80275B9C(c, lbl_80563CA4, lbl_80563CE4, lbl_80563CC8);
            break;
        case 0x1D:
            fn_802704D4(7, c);
            break;
        case 0x20:
            fn_80277A14(c, idx);
            break;
        default:
            fn_802E317C("Unrecognized camera type (%d) (id=%d)", c->mode, idx);
            break;
        }
    }
    fn_80278360(c);
    c->w4 = c->mode;
    c->f194 = c->fov;
    if (fn_80268BA4(c->mode) && !c->b138 && !c->b139) {
        f32 w;
        f32 now = lbl_8055E3C8;
        f32 scale = lbl_80563CE8 * lbl_8055C4B0;
        f32 k = (c->prev.start - now) / ((now - lbl_80563CEC) - now);
        k = (lbl_80563C2C.value < k) ? lbl_80563C2C.value : k;
        k = (k > lbl_80563C20.value) ? k : lbl_80563C20.value;
        w = lbl_80563C20.value + k * lbl_80563C2C.value;
        f32 s = PSVECMag((const Vec*)((const f32*)(c->car + 0x26B4)));
        s *= scale;
        f32 value = lbl_80563CEC;
        s = (value < s) ? lbl_80563CEC : s;
        f32 bump = w * s;
        f32 curF158 = c->f158;
        f32 e = ShakeEase((lbl_80563C6C - (now - curF158)) / lbl_80563C6C);
        f32 currentShake = c->f154 * e;
        if (bump > lbl_80563CF0 && bump > currentShake) {
            c->f158 = now;
            c->f154 = bump;
        }
    } else {
        c->f158 = lbl_80563C20.value;
        c->f154 = lbl_80563C20.value;
    }
}

void fn_80273BD0(CamRec* c);
void fn_8027445C(CamRec* c);
void fn_80274268(CamRec* c);
void fn_802704D4(int type, CamRec* c);
void fn_802E30C8(const char* fmt, ...);

// Where camera type `type` would put player idx's camera right now: runs it on a copy of the record and
// returns the pose and fov, leaving the live camera alone.
void fn_80269BB0(int idx, int type, CamPose* pose, f32* fov) {
    CamRec c = lbl_80499080[idx];
    c.w4 = 0x15;
    c.mode = type;
    c.b138 = 1;
    switch (type) {
    case 1:
        fn_8027445C(&c);
        break;
    case 0:
        fn_80273BD0(&c);
        break;
    case 4:
    case 5:
        fn_802704D4(type, &c);
        break;
    case 2:
        fn_80274268(&c);
        break;
    case 0xB:
    case 0x11:
        fn_802E30C8("Physcam: get_camera_frame: Unsupported camera to live transition to %d", type);
        break;
    }
    *pose = c.pose;
    *fov = c.fov;
}

void fn_8026A1F0(s32 a, int i, u8 b) {
    ((CamRec*)fn_80267EE4(i))->nextMode = a;
    fn_802735F4(i, b);
}

void fn_8026A248(s32 a, int i) { ((CamRec*)fn_80267EE4(i))->nextTarget = a; }

s32 fn_8026A27C(int i) {
    s32 v = lbl_80499E50[i];
    lbl_80499E50[i] = 0;
    return v;
}

void fn_8026A298(int i) { lbl_80499E50[i] = -1; }

void fn_8026A2B0(int i) { lbl_80499E50[i] = 1; }

int fn_8026A2C8(void) { return fn_801EA994(); }

void fn_8026A2CC(const Words12* s, int i, f32 v) {
    CamRec* e = &lbl_80499080[i];
    *(Words12*)&e->saved = *s;  // copied as words
    e->f1D4 = v;
}

u32 fn_8026A344(int i) { return ((CamRec*)fn_80267EE4(i))->target; }

u32 fn_8026A368(int i) { return ((CamRec*)fn_80267EE4(i))->mode; }

int fn_8026A38C(void) { return lbl_8055F488; }

extern const f32 lbl_80563CBC;

// Starts a camera transition for the camera following `car`.
void fn_8026A394(u8* car) {
    for (int i = 0; i < 4; i++) {
        CamRec* c = (CamRec*)fn_80267EE4(i);
        if (c->car == car) {
            c->b139 = 1;
            c->b138 = 1;
            c->f13C = RumbleForSpeed(F32(c->car, 0x25D4));
            c->f154 = lbl_80563C20.value;
            c->f158 = lbl_80563C20.value;
            c->prev.pose = c->pose;
            c->prev.fov = c->fov;
            c->prev.start = lbl_8055E3C8;
            return;
        }
    }
}

void fn_8026A49C(int idx, int type, CamPose* pose, f32* fov) { fn_80269BB0(idx, type, pose, fov); }

void fn_8026A4A0(int i, CamPose* out, f32* fov) {
    CamRec* e = &lbl_80499080[i];
    out->rot = e->pose.rot;
    out->pos = e->pose.pos;
    *fov = e->fov;
}

void fn_8026A51C(CamRec* c, u8* own, int idx) { fn_802693F4(c, own, idx); }

f32 fn_8026A520(int i) {
    if (i >= 0 && (u32)i < 8) {
        return lbl_80499980[i];
    }
    return lbl_80563C20.value;
}

void fn_8026A54C(f32 v) { lbl_8055F484 = v; }

int fn_8026A554(s32* p) {
    s32 i = *p;
    if (i >= 0 && (u32)i < 8) {
        lbl_80499980[i] = lbl_80563C6C;
    }
    return 4;
}

}  // extern "C"

// Folded degree/radian constants (the initialisers are dynamic in the original, so they load these).


inline f32 Dot(f32 a, f32 b, f32 c, f32 d, f32 e, f32 f) { return e * f + (a * b + c * d); }

// .sdata2 constants owned by this unit (0x80563C20-0x80563D08), in address order.
extern const float lbl_80563C40 = 0.0001f;
extern const float lbl_80563C44 = 0.3926991f;
extern const float lbl_80563C48 = 0.7853981f;
extern const float lbl_80563C4C = -2.5f;
extern const float lbl_80563C50 = 0.8f;
extern const float lbl_80563C54 = 1.1780972f;
extern const float lbl_80563C58 = -100.0f;
extern const float lbl_80563C5C = 1.5707964f;
extern const float lbl_80563C60 = -1000.0f;
extern const float lbl_80563C64 = -999.0f;
extern const float lbl_80563C68 = 1.701696f;
extern const float lbl_80563C6C = 0.5f;
extern const float lbl_80563C70 = 0.97f;
extern const float lbl_80563C74 = -0.100000024f;
extern const float lbl_80563C78 = -0.5f;
extern const float lbl_80563C7C = 0.016683351f;
extern const float lbl_80563C80 = 0.001f;
extern const float lbl_80563C84 = 2.5f;
extern const float lbl_80563C88 = 360.0f;
extern const float lbl_80563C8C = 0.3f;
extern const float lbl_80563C90 = 0.305f;
extern const float lbl_80563C94 = 2.0f;
extern const float lbl_80563C98 = 2.088f;
extern const float lbl_80563C9C = 4.176f;
extern const float lbl_80563CA0 = 0.05f;
extern const float lbl_80563CA4 = 9.0f;
extern const float lbl_80563CA8 = 1.2f;
extern const float lbl_80563CAC = 6.2831855f;
extern const float lbl_80563CB0 = -0.05235988f;
extern const float lbl_80563CB4 = 0.05235988f;
extern const float lbl_80563CB8 = 0.12217305f;
extern const float lbl_80563CBC = 0.030000001f;
extern const float lbl_80563CC0 = 8.0f;
extern const float lbl_80563CC4 = 1.6f;
extern const float lbl_80563CC8 = 0.9f;
extern const float lbl_80563CCC = 4.0f;
extern const float lbl_80563CD0 = -3.0f;
extern const float lbl_80563CD4 = 300.0f;
extern const float lbl_80563CD8 = -295.0f;
extern const float lbl_80563CDC = 20.0f;
extern const float lbl_80563CE0 = -15.0f;
extern const float lbl_80563CE4 = 2.6f;
extern const float lbl_80563CE8 = 0.0014f;
extern const float lbl_80563CEC = 0.25f;
extern const float lbl_80563CF0 = 0.04f;
extern const float lbl_80563CF4 = 1.4398966f;
extern const float lbl_80563CF8 = 900.0f;
extern const float lbl_80563CFC = 0.7417649f;
extern const float lbl_80563D00 = 0.9599311f;
extern const float lbl_80563D04 = 1.0471976f;

// .sdata constants owned by this unit (0x8055C4B0-0x8055C4F8), in address order.
f32 lbl_8055C4B0 = 0.65f;
s32 lbl_8055C4B4 = 1000;
s32 lbl_8055C4B8 = 2500;
f32 lbl_8055C4BC = 3.0f;
f32 lbl_8055C4C0 = 130.0f;
f32 lbl_8055C4C4 = 30.0f;
u8 lbl_8055C4C8 = 1;
f32 lbl_8055C4CC = 10.0f;
f32 lbl_8055C4D0 = 45.0f;
f32 lbl_8055C4D4 = 46.0f;
f32 lbl_8055C4D8 = 0.02f;
f32 lbl_8055C4DC = 0.002f;
f32 lbl_8055C4E0 = 12.0f;
f32 lbl_8055C4E4 = 4.0f;
s32 lbl_8055C4E8 = 500;
s32 lbl_8055C4EC = 1500;
f32 lbl_8055C4F0 = 4.0f;
f32 lbl_8055C4F4 = 4.0f;

CamRec::CamRec(void) {
    fn_80265DD8(reinterpret_cast<u8*>(&w160));
    fn_80266120(reinterpret_cast<u8*>(&w178));
}
