// Camera manager (0x80265DD8-0x8026A600): the per-player camera records, camera type dispatch
// ("Unrecognized camera type (%d) (id=%d)", "Physcam: get_camera_frame: ..."), crash and cheer sound
// cues ("crash_vol", "cheer3.sfx") and the per-slot shake levels. Written against raw field offsets;
// the large routines and the initialiser are still stubs. Names are guesses apart from the strings.
#include "game/common.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

class VD {
public:
    virtual ~VD(void);
};

struct Words12 {
    u32 w[12];
};

class CenterLine;

// A camera pose: rotation then position.
struct CamMat3 {
    f32 m[9];
};

struct CamVec3 {
    f32 v[3];
};

struct CamPose {
    CamPose(void) {}
    CamMat3 rot;  // offset 0x0
    CamVec3 pos;  // offset 0x24
};

// One player's camera record.
// Mount camera timing (plain data; copied word by word).
struct CamMountTime {
    u32 w[4];   // offset 0x0
    f32 start;  // offset 0x10, when the mount last changed
};

// Per-camera state that a record copy must not duplicate (its copy constructor copies nothing).
struct CamLink {
    CamLink(void) {}
    CamLink(const CamLink&) {}
    u32 w;
};

struct CamRec {
    CamRec(void);  // fn_8026A600 (out of line, after this unit)
    s32 w0;             // offset 0x0
    s32 w4;             // offset 0x4
    f32 f8;             // offset 0x8
    f32 fC;             // offset 0xC
    s32 mode;           // offset 0x10
    s32 nextMode;       // offset 0x14
    s32 w18;            // offset 0x18
    s32 w1C;            // offset 0x1C
    s32 target;         // offset 0x20
    s32 nextTarget;     // offset 0x24
    u32 w28[6];         // offset 0x28
    CamVec3 eye;        // offset 0x40
    u32 w4C[3];         // offset 0x4C
    CamPose pose;       // offset 0x58
    CamPose lastPose;   // offset 0x88
    s32 lastFrame;      // offset 0xB8
    f32 fov;            // offset 0xBC
    CamPose carFrame;   // offset 0xC0
    u32 wF0[6];
    u32 w108;
    u32 w10C;
    u32 w110;
    u32 w114;
    u32 w118;
    u32 w11C;
    CamVec3 vel;        // offset 0x120
    CamVec3 carVel;     // offset 0x12C
    u8 b138;            // offset 0x138
    u8 b139;            // offset 0x139
    f32 f13C;           // offset 0x13C
    u8* car;            // offset 0x140
    u8* lastCar;        // offset 0x144
    u32 w148[3];        // offset 0x148
    f32 f154;           // offset 0x154
    f32 f158;           // offset 0x158
    f32 f15C;           // offset 0x15C
    u32 w160[3];        // offset 0x160
    f32 f16C;           // offset 0x16C
    f32 f170;           // offset 0x170
    f32 f174;           // offset 0x174
    u32 w178[2];        // offset 0x178
    f32 f180;           // offset 0x180
    f32 f184;           // offset 0x184
    CamLink link;       // offset 0x188
    f32 f18C;           // offset 0x18C
    f32 f190;           // offset 0x190
    f32 f194;           // offset 0x194
    f32 f198;           // offset 0x198
    f32 f19C;           // offset 0x19C
    f32 f1A0;           // offset 0x1A0
    u32 w1A4[12];       // offset 0x1A4
    f32 f1D4;           // offset 0x1D4
    CamMountTime mount; // offset 0x1D8
    s16 h1EC;           // offset 0x1EC
    f32 f1F0;           // offset 0x1F0
    f32 prevFov;        // offset 0x1F4
    CamPose prevPose;   // offset 0x1F8
};

// Walks the ideal racing line for camera placement.
struct IdealFollower {
    u8 pad0[0x4];    // offset 0x0
    u8* seg;         // offset 0x4, current line segment
    f32 t;           // offset 0x8, how far along it
    u8 padC[0x4];    // offset 0xC
    f32 dir[2];      // offset 0x10, heading (x, z)
    u8 pad18[0x14];  // offset 0x18
    u8* line;        // offset 0x2C
    u8 pad30[0x20];  // offset 0x30
    u8* buf;         // offset 0x50
    u8 pad54[0x14];  // offset 0x54
};

// A position on the ideal line: the segment and how far along it.
struct PathPos {
    u8* seg;       // offset 0x0
    f32 t;         // offset 0x4
    u8 pad8[0x8];  // offset 0x8
};

extern "C" {
void fn_80134E34(IdealFollower* f, f32 dist);
void fn_80136468(PathPos* out, IdealFollower* f);
void fn_8013530C(IdealFollower* f, int del);
void fn_80268BD4(u8* rec, const u8* seg, f32 t, f32 k);
}

extern CamPose lbl_80499950;  // trackside camera frames
extern CamPose lbl_80499920;

extern "C" IdealFollower* __ct__13IdealFollowerFiP10CenterLine(IdealFollower* f, int mode, CenterLine* line);

extern "C" {
void fn_801EF024(void* cb);
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

struct Mat3;
void Mat3Mul(Mat3* out, const Mat3* a, const Mat3* b);

// A scratch 3x4 matrix.
struct Mtx34 {
    f32 m[3][4];
};

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

extern "C" {

u8* fn_80265DD8(u8* p) {
    fn_80265E08(p);
    return p;
}

void fn_80265E08(u8* p) {
    f32 z = 0.0f;
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
        e->x = 0.017453292f * fn_80129DC4(6, -lbl_8055C4E0, lbl_8055C4E0);
        e->y = 0.017453292f * fn_80129DC4(6, -lbl_8055C4E4, lbl_8055C4E4);
        e->n = fn_80129D90(6, lbl_8055C4E8, lbl_8055C4EC);
        e++;
    }
}

extern const f32 lbl_80563C20;  // 0.0
extern const f32 lbl_80563C28;  // -1.0
extern const f32 lbl_80563C2C;  // 1.0
extern const f32 lbl_80563C30;  // -2.0
extern const f32 lbl_80563C34;  // 3.0

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
    if (x < lbl_80563C28) {
        return lbl_80563C28;
    }
    if (x > lbl_80563C2C) {
        return lbl_80563C2C;
    }
    if (x > lbl_80563C20) {
        return -2.0f * x * x * x + 3.0f * x * x;
    }
    return -2.0f * x * x * x - 3.0f * x * x;
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

f32 PSVECMag(const f32* v);

// out = from + dir * scale (0x10), lifted by |dir| * rise (0x14).
void fn_80266070(u8* p, f32* out, const f32* from, const f32* dir) {
    f32 v[3];
    f32 k = F32(p, 0x10);
    v[0] = dir[0] * k;
    v[1] = dir[1] * k;
    v[2] = dir[2] * k;
    v[1] += F32(p, 0x14) * PSVECMag(dir);
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
    F32(p, 0xC) = 1.0f;
}

extern f32 lbl_8055C4BC;  // funky fov scale
extern s32 lbl_8055C4B4;  // funky fov ease start time
extern s32 lbl_8055C4B8;  // funky fov ease end time

static inline f32 MinF(f32 a, f32 b) { return a < b ? a : b; }
static inline f32 ClampF(f32 x) {
    x = lbl_80563C2C < x ? lbl_80563C2C : x;
    return x > lbl_80563C20 ? x : lbl_80563C20;
}

// "funky_fov": on a camera cut, aim the view angle at a scaled (capped) fov, then ease toward it.
struct FunkyFov {
    s32 last;    // offset 0x0
    s32 acc;     // offset 0x4
    f32 target;  // offset 0x8
    f32 t;       // offset 0xC
};

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
    f32 a = f->acc;
    f32 e = lbl_8055C4B8;
    f32 st = lbl_8055C4B4;
    f32 x = (a - st) / (e - st);
    f->t = ShakeEase(lbl_80563C20 + ClampF(x) * lbl_80563C2C);
}

f32 PSVECDistance(const f32* a, const f32* b);
extern f32 lbl_8055C4C4;
extern const f32 lbl_80563C20;

// out = from + dir * (distance to `to`, eased by the blend at 0xC).
void fn_802662D8(u8* p, f32* out, const f32* from, const f32* dir, const f32* to) {
    f32 d = PSVECDistance(from, to) - lbl_8055C4C4;
    f32 k = F32(p, 0xC);
    f32 t = lbl_80563C20 + k * (d - lbl_80563C20);
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

struct ShakeVec {
    f32 x, y, z;
    ShakeVec(f32 ax, f32 ay, f32 az) : x(ax), y(ay), z(az) {}
};

extern "C" {
int fn_8026B0C0(int view);
double sin(double x);
double cos(double x);
extern f32 lbl_8055E3C8;        // race time
extern const f32 lbl_80563C40;  // smallest rotation
extern const f32 lbl_80563C50;
extern const f32 lbl_80563CA0;  // smallest shake
extern const f32 lbl_80563CA4;  // 2 pi
extern const f32 lbl_80563CA8;
extern const f32 lbl_80563CAC;  // base frequency
extern const f32 lbl_80563CB0;  // jitter range
extern const f32 lbl_80563CB4;
extern const f32 lbl_80563CB8;  // wave weight

// Rotation from the rotation vector w (axis * angle), Rodrigues' formula; identity for a tiny angle.
static inline void ShakeRot(ShakeMat* r, const ShakeVec& w) {
    f32 a = PSVECMag(&w.x);
    if (a > lbl_80563C40) {
        f32 x, s, y, z, c, k;
        f32 inv = lbl_80563C2C / a;
        x = inv * w.x;
        y = inv * w.y;
        z = inv * w.z;
        s = (f32)sin(a);
        c = (f32)cos(a);
        k = lbl_80563C2C - c;
        r->m[0][0] = c + k * (x * x);
        r->m[0][1] = y * (k * x) + s * z;
        r->m[0][2] = z * (k * x) - s * y;
        r->m[1][0] = y * (k * x) - s * z;
        r->m[1][1] = c + k * (y * y);
        r->m[1][2] = z * (k * y) + s * x;
        r->m[2][0] = z * (k * x) + s * y;
        r->m[2][1] = z * (k * y) - s * x;
        r->m[2][2] = c + k * (z * z);
    } else {
        r->m[0][1] = lbl_80563C20;
        r->m[0][0] = lbl_80563C2C;
        r->m[0][2] = lbl_80563C20;
        r->m[1][0] = lbl_80563C20;
        r->m[1][1] = lbl_80563C2C;
        r->m[1][2] = lbl_80563C20;
        r->m[2][0] = lbl_80563C20;
        r->m[2][1] = lbl_80563C20;
        r->m[2][2] = lbl_80563C2C;
    }
}

// The track a car is on, as the camera code sees it.
struct CamTrack {
    u8 pad0[0x4];
    u8* seg;       // offset 0x4
    u8 pad8[0x24];
    u8* path;      // offset 0x2C
};

struct CamTrackPos {
    u8* seg;       // offset 0x0
    f32 t;         // offset 0x4
    u8 pad8[0x4];  // offset 0x8
};

class CamTrackOwner {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual CamTrack* GetTrack(void);  // 0x18
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

void fn_80136594(CamTrackPos* out, CamTrack* trk, const CamXZ* xz);
int fn_8026A2C8(void);
CamRacerObj* fn_801B2EC0(int i);
f32 PSVECSquareDistance(const f32* a, const f32* b);
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
    CamTrack* trk = (*(CamTrackOwner**)(car + 0x270))->GetTrack();
    CamXZ xz(pose->pos.v[0], pose->pos.v[2]);
    CamTrackPos at;
    fn_80136594(&at, trk, &xz);
    f32 camDist = lbl_8055C4CC + (F32(at.seg, 0x1C) + F32(at.seg, 0x18) * at.t);
    int n = fn_8026A2C8();
    f32 best = lbl_8055F49C;
    u8* near = 0;
    for (int i = 0; i < n; i++) {
        u8* r = fn_801B2EC0(i)->GetCar();
        rp = (const f32*)(r + 0x60);
        CamXZ rxz(rp[0], rp[2]);
        CamTrackPos rat;
        fn_80136594(&rat, trk, &rxz);
        f32 d = (F32(rat.seg, 0x1C) + F32(rat.seg, 0x18) * rat.t) - camDist;
        if (d < lbl_80563C20) {
            u32 lap = trk->seg ? U8(trk->seg, 0x34) : 3;
            d += *(f32*)(trk->path + lap * 4 + 8);
        }
        if (d * d < lbl_8055F49C) {
            f32 sq = PSVECSquareDistance(eye, rp);
            if (sq < best) {
                best = sq;
                near = r;
            }
        }
    }
    if (near) {
        f32 a = (F32(near, 0x25CC) - lbl_8055C4D0) / (lbl_8055C4D4 - lbl_8055C4D0);
        a = (lbl_80563C2C < a) ? lbl_80563C2C : a;
        a = (a > lbl_80563C20) ? a : lbl_80563C20;
        f32 ka = lbl_80563C20 + a * lbl_80563C2C;
        f32 b = (best - lbl_8055F49C) / (lbl_80563C20 - lbl_8055F49C);
        b = (lbl_80563C2C < b) ? lbl_80563C2C : b;
        b = (b > lbl_80563C20) ? b : lbl_80563C20;
        f32 k = ka * (lbl_80563C20 + b * lbl_80563C2C);
        f32 move = lbl_8055C4D8 * k;
        f32 turn = lbl_8055C4DC * k;
        if (lbl_8055F498) {
            ShakeVec j(fn_80129DC4(6, -move, move), fn_80129DC4(6, -move, move), fn_80129DC4(6, -move, move));
            pose->pos.v[0] += j.x;
            pose->pos.v[1] += j.y;
            pose->pos.v[2] += j.z;
        }
        if (lbl_8055C4C8) {
            ShakeVec w(fn_80129DC4(6, -turn, turn), fn_80129DC4(6, -turn, turn), fn_80129DC4(6, -turn, turn));
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
    CamTrack* trk = (*(CamTrackOwner**)(c->car + 0x270))->GetTrack();
    CamXZ xz(c->pose.pos.v[0], c->pose.pos.v[2]);
    CamTrackPos at;
    fn_80136594(&at, trk, &xz);
    f32 camDist = lbl_8055C4CC + (F32(at.seg, 0x1C) + F32(at.seg, 0x18) * at.t);
    int n = fn_8026A2C8();
    f32 best = lbl_8055F49C;
    u8* near = 0;
    for (int i = 0; i < n; i++) {
        u8* r = fn_801B2EC0(i)->GetCar();
        rp = (const f32*)(r + 0x60);
        CamXZ rxz(rp[0], rp[2]);
        CamTrackPos rat;
        fn_80136594(&rat, trk, &rxz);
        f32 d = (F32(rat.seg, 0x1C) + F32(rat.seg, 0x18) * rat.t) - camDist;
        if (d < lbl_80563C20) {
            u32 lap = trk->seg ? U8(trk->seg, 0x34) : 3;
            d += *(f32*)(trk->path + lap * 4 + 8);
        }
        if (d * d < lbl_8055F49C) {
            f32 sq = PSVECSquareDistance(eye, rp);
            if (sq < best) {
                best = sq;
                near = r;
            }
        }
    }
    if (near) {
        f32 a = (F32(near, 0x25CC) - lbl_8055C4D0) / (lbl_8055C4D4 - lbl_8055C4D0);
        a = (lbl_80563C2C < a) ? lbl_80563C2C : a;
        a = (a > lbl_80563C20) ? a : lbl_80563C20;
        f32 ka = lbl_80563C20 + a * lbl_80563C2C;
        f32 b = (best - lbl_8055F49C) / (lbl_80563C20 - lbl_8055F49C);
        b = (lbl_80563C2C < b) ? lbl_80563C2C : b;
        b = (b > lbl_80563C20) ? b : lbl_80563C20;
        f32 k = ka * (lbl_80563C20 + b * lbl_80563C2C);
        f32 move = lbl_8055C4D8 * k;
        f32 turn = lbl_8055C4DC * k;
        if (lbl_8055F498) {
            ShakeVec j(fn_80129DC4(6, -move, move), fn_80129DC4(6, -move, move), fn_80129DC4(6, -move, move));
            c->pose.pos.v[0] += j.x;
            c->pose.pos.v[1] += j.y;
            c->pose.pos.v[2] += j.z;
        }
        if (lbl_8055C4C8) {
            ShakeVec w(fn_80129DC4(6, -turn, turn), fn_80129DC4(6, -turn, turn), fn_80129DC4(6, -turn, turn));
            ShakeMat rot;
            ShakeRot(&rot, w);
            ShakeMul((ShakeMat*)&c->pose.rot, (ShakeMat*)&c->pose.rot, &rot);
        }
    }
}


void fn_802670A0(u8* p, int idx) {
    int n = fn_80178CA4();
    int cols = (n > 1) + 1;
    int rows = (n + cols - 1) / cols;
    int w = lbl_8055CFF0 / cols;
    int h = lbl_8055CFF4 / rows;
    S32(p, 0x40) = w;
    S32(p, 0x44) = h;
    S32(p, 0x38) = w * (idx / rows);
    S32(p, 0x3C) = h * (idx % rows);
}

void fn_80267130(u8* p) {
    f32 t = (F32(p, 0x30) - 0.39269909f) / 0.78539819f;
    f32 c = (1.0f < t) ? 1.0f : t;
    c = (c > 0.0f) ? c : 0.0f;
    f32 k = -2.5f;
    F32(p, 0x48) = 3.0f + c * k;
    F32(p, 0x4C) = lbl_8055F484 * (fn_80178CA4() == 1 ? 1.0f : 0.8f);
}

struct KVec3 {
    f32 x, y, z;
};

// Rotation + translation (camera frame).
struct KFrame {
    f32 m[9];  // offset 0x0, rows
    KVec3 t;   // offset 0x24
};

static inline f32 KDot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

static inline void KMulVec(KVec3* out, const KVec3* in, const KFrame* m) {
    if (out == in) {
        f32 x = KDot3(in->x, m->m[0], in->y, m->m[3], in->z, m->m[6]);
        f32 y = KDot3(in->x, m->m[1], in->y, m->m[4], in->z, m->m[7]);
        f32 z = KDot3(in->x, m->m[2], in->y, m->m[5], in->z, m->m[8]);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = KDot3(in->x, m->m[0], in->y, m->m[3], in->z, m->m[6]);
        out->y = KDot3(in->x, m->m[1], in->y, m->m[4], in->z, m->m[7]);
        out->z = KDot3(in->x, m->m[2], in->y, m->m[5], in->z, m->m[8]);
    }
}

f32 fn_801EA0E8(f32 x, f32 z);  // ground height
extern f32 lbl_8055D020;        // aspect

// Keeps the camera's near plane above the ground: lifts it by the deepest corner (not in modes 17/21).
void fn_802671CC(u8* p) {
    KVec3 v;
    KVec3 w;
    f32 nearZ = F32(p, 0x48);
    f32 s = nearZ * F32(p, 0x34);
    f32 hx = s / lbl_8055D020;
    f32 hy = ((f32)S32(p, 0x44) * s) / (f32)S32(p, 0x40);
    f32 lift = lbl_80563C20;
    for (int i = 0; i < 4; i++) {
        f32 cx = (i & 1) ? hx : -hx;
        f32 cy = (i & 2) ? hy : -hy;
        v.x = cx;
        v.y = cy;
        v.z = nearZ;
        KMulVec(&w, &v, (KFrame*)p);
        w.x += F32(p, 0x24);
        w.y += F32(p, 0x28);
        w.z += F32(p, 0x2C);
        f32 d = fn_801EA0E8(w.x, w.z) - w.y;
        lift = lift > d ? lift : d;
    }
    if (S32(p, 0x58) != 0x11 && S32(p, 0x58) != 0x15) {
        F32(p, 0x28) = F32(p, 0x28) + lift;
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
extern char lbl_803467D0[];  // "crash_vol"
extern char lbl_803467DC[];  // "cheer3.sfx"

// Camera manager start-up: initialises the four camera records, the named/fixed cameras, the crash
// volume handle and the cheer sound, then resets.
void fn_80267420(void) {
    fn_801769CC();
    u8* c = (u8*)lbl_80499080;
    for (int i = 0; i < 4;) {
        memset(c, 0, 0x228);
        S32(c, 0x0) = i++;
        F32(c, 0x8) = lbl_80563C20;
        F32(c, 0xC) = lbl_80563C20;
        S32(c, 0x24) = 0;
        S32(c, 0x20) = 0;
        S32(c, 0x14) = 10;
        S32(c, 0x10) = 10;
        F32(c, 0x28) = lbl_80563C20;
        F32(c, 0x2C) = lbl_80563C2C;
        F32(c, 0x30) = lbl_80563C20;
        F32(c, 0x34) = lbl_80563C20;
        F32(c, 0x38) = lbl_80563C20;
        F32(c, 0x3C) = lbl_80563C20;
        U8(c, 0x138) = 1;
        F32(c, 0x58) = lbl_80563C2C;
        F32(c, 0x5C) = lbl_80563C20;
        F32(c, 0x60) = lbl_80563C20;
        F32(c, 0x64) = lbl_80563C20;
        F32(c, 0x68) = lbl_80563C2C;
        F32(c, 0x6C) = lbl_80563C20;
        F32(c, 0x70) = lbl_80563C20;
        F32(c, 0x74) = lbl_80563C20;
        F32(c, 0x78) = lbl_80563C2C;
        F32(c, 0x7C) = lbl_80563C20;
        F32(c, 0x80) = lbl_80563C20;
        F32(c, 0x84) = lbl_80563C20;
        *(CamPose*)(c + 0x88) = *(CamPose*)(c + 0x58);
        F32(c, 0x120) = lbl_80563C20;
        F32(c, 0x124) = lbl_80563C20;
        F32(c, 0x128) = lbl_80563C20;
        F32(c, 0x12C) = lbl_80563C20;
        F32(c, 0x130) = lbl_80563C20;
        F32(c, 0x134) = lbl_80563C20;
        F32(c, 0xF0) = lbl_80563C2C;
        F32(c, 0xF4) = lbl_80563C20;
        F32(c, 0xF8) = lbl_80563C20;
        F32(c, 0xFC) = lbl_80563C20;
        F32(c, 0x100) = lbl_80563C2C;
        F32(c, 0x104) = lbl_80563C20;
        F32(c, 0x108) = lbl_80563C20;
        F32(c, 0x10C) = lbl_80563C20;
        F32(c, 0x110) = lbl_80563C2C;
        F32(c, 0x114) = lbl_80563C20;
        F32(c, 0x118) = lbl_80563C20;
        F32(c, 0x11C) = lbl_80563C20;
        F32(c, 0xC0) = lbl_80563C2C;
        F32(c, 0xC4) = lbl_80563C20;
        F32(c, 0xC8) = lbl_80563C20;
        F32(c, 0xCC) = lbl_80563C20;
        F32(c, 0xD0) = lbl_80563C2C;
        F32(c, 0xD4) = lbl_80563C20;
        F32(c, 0xD8) = lbl_80563C20;
        F32(c, 0xDC) = lbl_80563C20;
        F32(c, 0xE0) = lbl_80563C2C;
        F32(c, 0xE4) = lbl_80563C20;
        F32(c, 0xE8) = lbl_80563C20;
        F32(c, 0xEC) = lbl_80563C20;
        S32(c, 0x140) = 0;
        S32(c, 0x144) = 0;
        S32(c, 0x4) = 10;
        S32(c, 0x1C) = 10;
        F32(c, 0x1A4) = lbl_80563C2C;
        F32(c, 0x1A8) = lbl_80563C20;
        F32(c, 0x1AC) = lbl_80563C20;
        F32(c, 0x1B0) = lbl_80563C20;
        F32(c, 0x1B4) = lbl_80563C2C;
        F32(c, 0x1B8) = lbl_80563C20;
        F32(c, 0x1BC) = lbl_80563C20;
        F32(c, 0x1C0) = lbl_80563C20;
        F32(c, 0x1C4) = lbl_80563C2C;
        F32(c, 0x1C8) = lbl_80563C20;
        F32(c, 0x1CC) = lbl_80563C20;
        F32(c, 0x1D0) = lbl_80563C20;
        F32(c, 0x1D4) = lbl_80563C54;
        c += 0x228;
    }
    fn_80276B8C();
    fn_80268D94();
    fn_80268E00();
    fn_80278214();
    fn_80273354();
    fn_8027045C();
    lbl_8055F494 = fn_802090D4(lbl_803467D0);
    lbl_8055F490 = fn_80207848(lbl_803467DC, 0);
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
extern const double lbl_80563C38;  // int -> float conversion
extern const f32 lbl_80563C5C;
extern const f32 lbl_80563C60;
extern const f32 lbl_80563C64;

// Re-reads the saved pose (the identity passed in is not used).
static inline void ResetSavedPose(u8* c, const CamPose& pose) { fn_80276BE4(c + 0x1A4, c + 0x1D4); }

// Resets the four players' camera records, the shake table, the split-screen rects and the shake levels.
void fn_80267680(void) {
    fn_802733C8();
    u8* c = (u8*)lbl_80499080;
    for (int i = 0; i < 4; i++) {
        F32(c, 0x8) = lbl_80563C20;
        F32(c, 0xC) = lbl_80563C20;
        S32(c, 0x144) = 0;
        S32(c, 0xB8) = -1;
        U8(c, 0x138) = 1;
        U8(c, 0x139) = 1;
        F32(c, 0x13C) = lbl_80563C20;
        F32(c, 0x40) = lbl_80563C20;
        F32(c, 0x44) = lbl_80563C20;
        F32(c, 0x48) = lbl_80563C20;
        F32(c, 0x28) = lbl_80563C20;
        F32(c, 0x2C) = lbl_80563C2C;
        F32(c, 0x30) = lbl_80563C20;
        S32(c, 0x148) = 0;
        F32(c, 0x194) = lbl_80563C54;
        fn_80265E08(c + 0x160);
        fn_80266150(c + 0x178);
        F32(c, 0x158) = lbl_80563C58;
        F32(c, 0x15C) = i;
        F32(c, 0x198) = lbl_80563C20;
        F32(c, 0x19C) = lbl_80563C20;
        S32(c, 0x14C) = -1;
        S32(c, 0x150) = -1;
        if (!fn_801796B4()) {
            volatile f32 id[12];  // identity pose (unused; volatile keeps its stack layout)
            id[0] = lbl_80563C2C;
            id[1] = lbl_80563C20;
            id[2] = lbl_80563C20;
            id[3] = lbl_80563C20;
            id[4] = lbl_80563C2C;
            id[5] = lbl_80563C20;
            id[6] = lbl_80563C20;
            id[7] = lbl_80563C20;
            id[8] = lbl_80563C2C;
            id[9] = lbl_80563C20;
            id[10] = lbl_80563C20;
            id[11] = lbl_80563C20;
            fn_80276BE4(c + 0x1A4, c + 0x1D4);
        }
        fn_801E9DC4(c + 0x1EC);
        F32(c, 0x1F8) = lbl_80563C2C;
        F32(c, 0x1FC) = lbl_80563C20;
        F32(c, 0x200) = lbl_80563C20;
        F32(c, 0x204) = lbl_80563C20;
        F32(c, 0x208) = lbl_80563C2C;
        F32(c, 0x20C) = lbl_80563C20;
        F32(c, 0x210) = lbl_80563C20;
        F32(c, 0x214) = lbl_80563C20;
        F32(c, 0x218) = lbl_80563C2C;
        F32(c, 0x21C) = lbl_80563C20;
        F32(c, 0x220) = lbl_80563C20;
        F32(c, 0x224) = lbl_80563C20;
        F32(c, 0x1F4) = lbl_80563C5C;
        F32(c, 0x1F0) = lbl_80563C60;
        c += 0x228;
    }
    lbl_8055F488 = 0;
    lbl_8055F48C = lbl_80563C64;
    fn_80209190(lbl_8055F494, lbl_80563C2C);
    fn_80265E2C();
    fn_80268B94(0);
    fn_80270484();
    lbl_80499980[0] = lbl_80563C20;
    lbl_80499980[1] = lbl_80563C20;
    lbl_80499980[2] = lbl_80563C20;
    lbl_80499980[3] = lbl_80563C20;
    lbl_80499980[4] = lbl_80563C20;
    lbl_80499980[5] = lbl_80563C20;
    lbl_80499980[6] = lbl_80563C20;
    lbl_80499980[7] = lbl_80563C20;
}

void fn_8026785C(void) {
    fn_801EF024((void*)fn_8026A554);
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
int fn_801C5B68(u8* view, int idx);
void fn_8026B768(int target, int idx);
int fn_8026B734(int target, f32* out);
u8* fn_8017960C(int car);
int fn_801769F8(void);
f32 fn_801B8950(int car);
f32 PSVECDotProduct(const f32* a, const f32* b);
double tan(double x);
double fabs(double x);
void fn_802693F4(CamRec* c, u8* own, int idx);
void fn_80268E94(CamRec* c, int idx);
extern s32 lbl_8055E3C4;  // frame counter
extern f32 lbl_8055E460;  // tick alpha
extern u8 lbl_8055F480;
extern const f32 lbl_80563C68;  // default fov
extern const f32 lbl_80563C6C;  // half
extern const f32 lbl_80563C70;
extern const f32 lbl_80563C74;
extern const f32 lbl_80563C78;
extern const f32 lbl_80563C7C;  // shake decay per frame

// Per-frame camera update for player idx: run the current camera type, blend from last frame's pose by the
// tick alpha, and fill the view (pose, fov, rumble, flags). The shake levels decay unless paused.
void fn_802678E8(u8* view, int idx) {
    fn_802670A0(view, idx);
    if (fn_801C5B68(view, idx)) {
        fn_80267130(view);
        return;
    }
    CamRec* c = (CamRec*)fn_80267EE4(idx);
    c->f1A0 = lbl_80563C28;
    fn_8026B768(c->target, idx);
    if (lbl_8055E3C4 != c->lastFrame) {
        c->lastPose = c->pose;
        c->lastFrame = lbl_8055E3C4;
    }
    if (c->nextMode < 0 || c->nextMode >= 0x22) {
        c->nextMode = 4;
    }
    S32(view, 0x5C) = c->w1C;
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
        const f32* a = (const f32*)&c->lastPose;
        const f32* b = (const f32*)&c->pose;
        f32* d = (f32*)&blend;
        for (int i = 0; i < 12; i++) {
            f32 y = b[i];
            f32 x = a[i];
            d[i] = x + k * (y - x);
        }
        *(CamPose*)view = blend;
    } else {
        *(CamPose*)view = c->pose;
    }
    F32(view, 0x30) = c->fov;
    F32(view, 0x34) = tan(lbl_80563C6C * c->fov);
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
                f32 k = ((f32)__fabs(PSVECDotProduct(&c->pose.rot.m[6], &c->carFrame.rot.m[6])) - lbl_80563C70) / lbl_80563C74;
                k = (lbl_80563C2C < k) ? lbl_80563C2C : k;
                k = (k > lbl_80563C20) ? k : lbl_80563C20;
                rumble = r * (lbl_80563C2C + k * lbl_80563C78);
                goto done;
            }
        }
    }
    rumble = lbl_80563C20;
done:
    F32(view, 0x50) = rumble;
    F32(view, 0x54) = c->f1A0;
    if (c->b138) {
        flags |= 0x10;
    }
    c->b138 = 0;
    c->b139 = 0;
    S32(view, 0x58) = c->mode;
    S32(view, 0x60) = c->target;
    S32(view, 0x68) = flags;
    if (own) {
        *(s16*)(view + 0x64) = c->h1EC;
    } else {
        *(s16*)(view + 0x64) = *(s16*)(c->car + 0x6C);
    }
    if (!fn_801769F8()) {
        for (int i = 0; i < 8; i++) {
            f32 v = lbl_80499980[i] - lbl_80563C7C;
            lbl_80499980[i] = (lbl_80563C20 > v) ? lbl_80563C20 : v;
        }
    }
    fn_80267130(view);
    fn_802671CC(view);
}

void fn_80267E40(u32 id, f32 strength) {
    f32 now = lbl_8055E3C8;
    for (int i = 0; i < 4; i++) {
        u8* c = fn_80267EE4(i);
        if (U32(c, 0x140) == id && strength > F32(c, 0x154) * (1.0f - (now - F32(c, 0x158)) / 0.5f)) {
            F32(c, 0x158) = now;
            F32(c, 0x154) = strength;
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
extern const f32 lbl_80563C24;  // degrees to radians
extern const f32 lbl_80563C64;  // "no crash" time
extern const f32 lbl_80563C6C;  // fade time
extern const f32 lbl_80563C80;  // ms to s
extern const f32 lbl_80563C84;  // fade back in after
extern const f32 lbl_80563C88;  // 360
extern f32 lbl_8055F48C;        // crash start time

// Crash camera: for three seconds after a crash, fade the crash volume out and back in (the cheer the
// other way) and swing the camera once round a pivot ahead of it; then restore the volume and resume.
void fn_80267EF8(u8* p) {
    f32 t = lbl_80563C80 * (f32)(s32)fn_802E2C20() - lbl_8055F48C;
    if (t > lbl_80563C34) {
        lbl_8055F488 = 0;
        lbl_8055F48C = lbl_80563C64;
        fn_80209190(lbl_8055F494, lbl_80563C2C);
        if (!fn_8000D75C()) {
            fn_801769EC();
        }
        return;
    }
    f32 vol;
    f32 u = t / lbl_80563C34;
    vol = lbl_80563C20;
    if (t < lbl_80563C6C) {
        vol = lbl_80563C2C + (t / lbl_80563C6C) * lbl_80563C28;
    } else if (t > lbl_80563C84) {
        vol = vol + ((t - lbl_80563C84) / lbl_80563C6C) * lbl_80563C2C;
    }
    fn_80209190(lbl_8055F494, vol);
    u8* cheer = (u8*)lbl_8055F490;
    f32 cv = lbl_80563C2C - vol;
    if (cv != F32(cheer, 0x8)) {
        U8(cheer, 0x4D) = 1;
        F32(cheer, 0x8) = cv;
    }
    f32 ang = lbl_80563C24 * (lbl_80563C20 + ShakeEase(u) * lbl_80563C88);
    CamPose cam;
    CamPose car;
    cam = *(CamPose*)(p + 0x58);
    car = *(CamPose*)(p + 0xC0);
    f32 dist = PSVECDistance(car.pos.v, cam.pos.v);
    CamPose f;
    f = cam;
    f.pos.v[0] += f.rot.m[6] * dist;
    f.pos.v[1] += f.rot.m[7] * dist;
    f.pos.v[2] += f.rot.m[8] * dist;
    f32 euler[3];
    euler[0] = lbl_80563C20;
    euler[1] = ang;
    euler[2] = lbl_80563C20;
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
        // An index the compiler cannot fold (found by decomp-permuter) gives the target's stfsu.
        lbl_80499050.m[row = 0][*rowp] = 1.0f;
        lbl_80499050.m[0][1] = 0.0f;
        lbl_80499050.m[0][2] = 0.0f;
        lbl_80499050.m[0][3] = 0.0f;
        lbl_80499050.m[1][0] = 1.0f;
        lbl_80499050.m[1][1] = 0.0f;
        lbl_80499050.m[1][2] = 0.0f;
        lbl_80499050.m[1][3] = 0.0f;
        lbl_80499050.m[2][0] = 1.0f;
        lbl_80499050.m[2][1] = 0.0f;
        lbl_80499050.m[2][2] = 0.0f;
        lbl_80499050.m[2][3] = 0.0f;
    }
    fn_801BFD34(p + 0x7C, &lbl_80499050.m[2][1], p + 0xC0);
    Mat3Mul((Mat3*)(p + 0x58), (Mat3*)&lbl_80499050, (Mat3*)(p + 0xC0));
    F32(p, 0xBC) = lbl_8055F47C;
}

void PSVECNormalize(const f32* in, f32* out);
void PSVECCrossProduct(const f32* a, const f32* b, f32* out);
void fn_8026E850(f32* pos, const f32* target, u8* cam, const f32* up);
int fn_801E9E78(void* ground, KVec3* p, KVec3* n, s32* hit);
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

extern CamMount lbl_803467E8[3];

extern "C" {
// Mount camera points averaged over the ideal-line frames: two points in the frame, and its up vector.
struct MountView {
    KVec3 a;   // offset 0x0
    KVec3 b;   // offset 0xC
    KVec3 up;  // offset 0x18
};

// Fixed mount camera on the ideal line: for the current and the next mount, place both mount points in
// the line's frame (an average of two samples), then blend from one mount to the other over time.
void fn_80268358(CamRec* c) {
    f32 m[2][6] = {
        {lbl_803467E8[lbl_8055F4A0].off[0], lbl_803467E8[lbl_8055F4A0].off[1], lbl_803467E8[lbl_8055F4A0].off[2],
         lbl_803467E8[lbl_8055F4A0].look[0], lbl_803467E8[lbl_8055F4A0].look[1], lbl_803467E8[lbl_8055F4A0].look[2]},
        {lbl_803467E8[lbl_8055F4A4].off[0], lbl_803467E8[lbl_8055F4A4].off[1], lbl_803467E8[lbl_8055F4A4].off[2],
         lbl_803467E8[lbl_8055F4A4].look[0], lbl_803467E8[lbl_8055F4A4].look[1], lbl_803467E8[lbl_8055F4A4].look[2]},
    };
    MountView out[2];
    f32 zero = lbl_80563C20;
    f32 half = lbl_80563C6C;
    for (int i = 0; i < 2; i++) {
        MountView* o = &out[i];
        o->a.x = zero;
        o->a.y = zero;
        o->a.z = zero;
        o->b.x = zero;
        o->b.y = zero;
        o->b.z = zero;
        o->up.x = zero;
        o->up.y = zero;
        o->up.z = zero;
        KFrame fr[2];
        for (int j = 0; j < 2; j++) {
            IdealFollower f;
            __ct__13IdealFollowerFiP10CenterLine(&f, 0, 0);
            f32* p0 = (f32*)f.seg;
            f32* p1 = *(f32**)(f.seg + 0x14);
            s32 hit = 0;
            f32 x = p0[0] + f.t * (p1[0] - p0[0]);
            f32 z = p0[1] + f.t * (p1[1] - p0[1]);
            CamXZ xz(x, z);
            KVec3 v;
            v.x = xz.x;
            v.y = lbl_80563C20;
            v.z = xz.z;
            KVec3 n;
            fn_801E9E78(&lbl_8055F4A8, &v, &n, &hit);
            KFrame* r = &fr[j];
            r->m[6] = f.dir[0];
            r->m[7] = lbl_80563C20;
            r->m[8] = f.dir[1];
            PSVECNormalize(&r->m[6], &r->m[6]);
            PSVECCrossProduct(&n.x, &r->m[6], &r->m[0]);
            PSVECNormalize(&r->m[0], &r->m[0]);
            PSVECCrossProduct(&r->m[6], &r->m[0], &r->m[3]);
            PSVECNormalize(&r->m[3], &r->m[3]);
            r->t.x = v.x;
            r->t.y = v.y;
            r->t.z = v.z;
            if (f.buf) {
                delete[] f.buf;
            }
            f.buf = 0;
            fn_8013530C(&f, 0);
        }
        KVec3 in = *(KVec3*)&m[i][0];
        for (int k = 0; k < 2; k++) {
            KVec3 res;
            KMulVec(&res, &in, &fr[k]);
            res.x += fr[k].t.x;
            res.y += fr[k].t.y;
            res.z += fr[k].t.z;
            o->a.x += res.x * half;
            o->a.y += res.y * half;
            o->a.z += res.z * half;
        }
        KVec3 in2 = *(KVec3*)&m[i][3];
        for (int k = 0; k < 2; k++) {
            KVec3 res2;
            KMulVec(&res2, &in2, &fr[k]);
            res2.x += fr[k].t.x;
            res2.y += fr[k].t.y;
            res2.z += fr[k].t.z;
            o->b.x += res2.x * half;
            o->b.y += res2.y * half;
            o->b.z += res2.z * half;
        }
        o->up.x += fr[0].m[3] * half;
        o->up.y += fr[0].m[4] * half;
        o->up.z += fr[0].m[5] * half;
        o->up.x += fr[1].m[3] * half;
        o->up.y += fr[1].m[4] * half;
        o->up.z += fr[1].m[5] * half;
    }
    f32 t = lbl_8055E3C8 - c->mount.start;
    f32 ka;
    if (lbl_803467E8[lbl_8055F4A0].on) {
        ka = ShakeEase(t / lbl_8055C4F0);
    } else {
        ka = t / lbl_8055C4F0;
    }
    f32 kb;
    if (lbl_803467E8[lbl_8055F4A0].on) {
        kb = ShakeEase(t / lbl_8055C4F4);
    } else {
        kb = t / lbl_8055C4F4;
    }
    KVec3 bl;
    bl.x = out[0].b.x + ka * (out[1].b.x - out[0].b.x);
    bl.y = out[0].b.y + ka * (out[1].b.y - out[0].b.y);
    bl.z = out[0].b.z + ka * (out[1].b.z - out[0].b.z);
    KVec3 al;
    al.x = out[0].a.x + kb * (out[1].a.x - out[0].a.x);
    al.y = out[0].a.y + kb * (out[1].a.y - out[0].a.y);
    al.z = out[0].a.z + kb * (out[1].a.z - out[0].a.z);
    KVec3 up;
    up.x = out[0].up.x + ka * (out[1].up.x - out[0].up.x);
    up.y = out[0].up.y + ka * (out[1].up.y - out[0].up.y);
    up.z = out[0].up.z + ka * (out[1].up.z - out[0].up.z);
    PSVECNormalize(&up.x, &up.x);
    c->fov = lbl_803467E8[lbl_8055F4A0].fov + ka * (lbl_803467E8[lbl_8055F4A4].fov - lbl_803467E8[lbl_8055F4A0].fov);
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
void PSVECCrossProduct(const f32* a, const f32* b, f32* out);
void fn_801EA140(f32* pos);  // drop onto the ground
extern const f32 lbl_80563C8C;
extern const f32 lbl_80563C90;
extern const f32 lbl_80563C94;  // height above the ground

struct CamPt {
    f32 x, y;
};

// Builds a fixed camera frame (rows right/up/forward + position) beside the track at path position
// (seg, t): offset sideways by the segment width, set on the ground, looking `angle` off the track.
void fn_80268BD4(u8* rec, const u8* seg, f32 t, f32 angle) {
    const u8* next = PTR(seg, 0x14);
    CamPt pos;
    CamPt d;
    CamPt pt;
    d.x = F32(seg, 0x8) + t * (F32(next, 0x8) - F32(seg, 0x8));
    d.y = F32(seg, 0xC) + t * (F32(next, 0xC) - F32(seg, 0xC));
    pos.x = F32(seg, 0x0) + t * (F32(next, 0x0) - F32(seg, 0x0));
    pos.y = F32(seg, 0x4) + t * (F32(next, 0x4) - F32(seg, 0x4));
    f32 inv = lbl_80563C2C / (f32)sqrt(d.x * d.x + d.y * d.y);
    F32(rec, 0x28) = lbl_80563C20;
    f32 nx = d.x * inv;
    d.x = d.y * inv;
    d.y = -nx;
    f32 off = lbl_80563C8C * F32(seg, 0x28) - lbl_80563C90;
    pt.x = pos.x + d.x * off;
    pt.y = pos.y + d.y * off;
    F32(rec, 0x24) = pt.x;
    F32(rec, 0x2C) = pt.y;
    fn_801EA140((f32*)(rec + 0x24));
    f32 px = d.x;
    F32(rec, 0x28) = F32(rec, 0x28) + lbl_80563C94;
    f32 c = (f32)cos(angle);
    f32 sn = (f32)sin(angle);
    F32(rec, 0xC) = lbl_80563C20;
    F32(rec, 0x10) = lbl_80563C2C;
    F32(rec, 0x14) = lbl_80563C20;
    d.x = px * c - d.y * sn;
    d.y = px * sn + d.y * c;
    F32(rec, 0x18) = d.x;
    F32(rec, 0x1C) = lbl_80563C20;
    F32(rec, 0x20) = d.y;
    PSVECCrossProduct((f32*)(rec + 0xC), (f32*)(rec + 0x18), (f32*)rec);
}

void fn_80268D94(void) {
    IdealFollower f;
    __ct__13IdealFollowerFiP10CenterLine(&f, 0, 0);
    fn_80268BD4((u8*)&lbl_80499920, PTR(f.line, 0x1C), 0.0f, 2.088f);
    if (f.buf) {
        delete[] f.buf;
    }
    f.buf = 0;
    fn_8013530C(&f, 0);
}

void fn_80268E00(void) {
    IdealFollower f;
    PathPos pos;
    fn_80134E34(__ct__13IdealFollowerFiP10CenterLine(&f, 0, 0), -1.0f);
    fn_80136468(&pos, &f);
    if (!pos.seg) {
        pos.seg = PTR(f.line, 0x1C);
    }
    fn_80268BD4((u8*)&lbl_80499950, pos.seg, pos.t, 4.176f);
    if (f.buf) {
        delete[] f.buf;
    }
    f.buf = 0;
    fn_8013530C(&f, 0);
}


// One axis of the hit shake: a sine wave of the given phase plus jitter, scaled by the amplitude.
static inline f32 ShakeAxis(f32 amp, f32 phase, f32 twoPi) {
    f32 s = (f32)sin(phase * twoPi);
    return amp * (fn_80129DC4(0, lbl_80563CB0, lbl_80563CB4) + s * lbl_80563CB8);
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
                    ShakeVec w(ShakeAxis(amp, lbl_80563CAC * t, twoPi), ShakeAxis(amp, lbl_80563C50 * (lbl_80563CAC * t), twoPi),
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
void fn_80273604(CamRec* c, int which);
void fn_80275B9C(f32 a, f32 b, f32 c, CamRec* rec);
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

// Runs camera type c->mode for one frame: follows the car's frame, flags cuts (a mode change other than
// between 4 and 25, or a new car), smooths the rumble, dispatches on the type and finally arms the hit
// shake from the car's impact speed. Sets *own for types that fill the sound pose themselves.
void fn_802693F4(CamRec* c, u8* own, int idx) {
    u8* car = c->car;
    const KVec3* off = (const KVec3*)(car + 0x30);
    c->carFrame = *(CamPose*)(car + 0x3C);
    c->carVel.v[0] = F32(car, 0x118);
    c->carVel.v[1] = F32(car, 0x11C);
    c->carVel.v[2] = F32(car, 0x120);
    c->vel.v[0] = c->carVel.v[0];
    c->vel.v[1] = c->carVel.v[1];
    c->vel.v[2] = c->carVel.v[2];
    KVec3 d;
    KMulVec(&d, off, (const KFrame*)&c->carFrame);
    c->carFrame.pos.v[0] += d.x;
    c->carFrame.pos.v[1] += d.y;
    c->carFrame.pos.v[2] += d.z;
    if (c->mode != c->w4 && !(c->mode == 4 && c->w4 == 0x19) && !(c->w4 == 4 && c->mode == 0x19)) {
        c->b138 = 1;
    }
    fn_802733F4(c);
    if (c->lastCar != c->car) {
        c->lastCar = c->car;
        c->b139 = 1;
        c->b138 = 1;
    }
    if (c->b138) {
        fn_801E9DC4(&c->h1EC);
    }
    c->eye.v[0] = c->carFrame.pos.v[0];
    c->eye.v[1] = c->carFrame.pos.v[1];
    c->eye.v[2] = c->carFrame.pos.v[2];
    if (!fn_801769F8()) {
        f32 r = F32(c->car, 0x25D4) * lbl_80563CBC;
        r = (lbl_80563C2C < r) ? lbl_80563C2C : r;
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
            fn_80275FC0(c);
            f32 zero = lbl_80563C20;
            c->vel.v[0] = zero;
            c->vel.v[1] = zero;
            c->vel.v[2] = zero;
            *own = 1;
            break;
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
            fn_80273604(c, 1);
            break;
        case 0xD:
            fn_80273604(c, 2);
            break;
        case 0xE:
            fn_80273604(c, 3);
            break;
        case 0xF:
            fn_80273604(c, 4);
            break;
        case 0x13:
            c->pose = lbl_80499920;
            break;
        case 0x14:
            c->pose = lbl_80499950;
            break;
        case 0x1A:
            fn_80275B9C(lbl_80563CC0, lbl_80563CC4, lbl_80563CC8, c);
            break;
        case 0x1E:
            fn_80275B9C(lbl_80563CDC + lbl_80563C20 * lbl_80563CE0, lbl_80563CD4 + lbl_80563C20 * lbl_80563CD8,
                        lbl_80563CCC + lbl_80563C20 * lbl_80563CD0, c);
            break;
        case 0x1B:
            fn_802704D4(7, c);
            break;
        case 0x1C:
            fn_80275B9C(lbl_80563CA4, lbl_80563CE4, lbl_80563CC8, c);
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
        f32 k = (c->f1F0 - now) / ((now - lbl_80563CEC) - now);
        k = (lbl_80563C2C < k) ? lbl_80563C2C : k;
        k = (k > lbl_80563C20) ? k : lbl_80563C20;
        w = lbl_80563C20 + k * lbl_80563C2C;
        f32 s = PSVECMag((const f32*)(c->car + 0x26B4)) * scale;
        s = (lbl_80563CEC < s) ? lbl_80563CEC : s;
        f32 bump = w * s;
        f32 e = ShakeEase((lbl_80563C6C - (now - c->f158)) / lbl_80563C6C);
        if (bump > lbl_80563CF0 && bump > c->f154 * e) {
            c->f158 = now;
            c->f154 = bump;
        }
    } else {
        c->f158 = lbl_80563C20;
        c->f154 = lbl_80563C20;
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
    S32(fn_80267EE4(i), 0x14) = a;
    fn_802735F4(i, b);
}

void fn_8026A248(s32 a, int i) { S32(fn_80267EE4(i), 0x24) = a; }

s32 fn_8026A27C(int i) {
    s32 v = lbl_80499E50[i];
    lbl_80499E50[i] = 0;
    return v;
}

void fn_8026A298(int i) { lbl_80499E50[i] = -1; }

void fn_8026A2B0(int i) { lbl_80499E50[i] = 1; }

int fn_8026A2C8(void) { return fn_801EA994(); }

void fn_8026A2CC(const Words12* s, int i, f32 v) {
    u8* e = (u8*)lbl_80499080 + i * 0x228;
    *(Words12*)(e + 0x1A4) = *s;
    F32(e, 0x1D4) = v;
}

u32 fn_8026A344(int i) { return U32(fn_80267EE4(i), 0x20); }

u32 fn_8026A368(int i) { return U32(fn_80267EE4(i), 0x10); }

u8 fn_8026A38C(void) { return lbl_8055F488; }

extern const f32 lbl_80563CBC;
extern const f32 lbl_80563C2C;
extern f32 lbl_8055E3C8;

// Starts a camera transition for the camera following `car`.
void fn_8026A394(u8* car) {
    for (int i = 0; i < 4; i++) {
        CamRec* c = (CamRec*)fn_80267EE4(i);
        if (c->car == car) {
            c->b139 = 1;
            c->b138 = 1;
            f32 speed = F32(c->car, 0x25D4);
            f32 v = lbl_80563CBC * speed;
            f32 m = lbl_80563C2C;
            c->f13C = m < v ? m : v;
            c->f154 = lbl_80563C20;
            c->f158 = lbl_80563C20;
            c->prevPose = c->pose;
            c->prevFov = c->fov;
            c->f1F0 = lbl_8055E3C8;
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
    return 0.0f;
}

void fn_8026A54C(f32 v) { lbl_8055F484 = v; }

int fn_8026A554(s32* p) {
    s32 i = *p;
    if (i >= 0 && (u32)i < 8) {
        lbl_80499980[i] = 0.5f;
    }
    return 4;
}

}  // extern "C"

// Folded degree/radian constants (the initialisers are dynamic in the original, so they load these).
extern const f32 lbl_80563CF4;  // 82.5 deg
extern const f32 lbl_80563CF8;  // 30 * 30
extern const f32 lbl_80563CFC;  // 42.5 deg
extern const f32 lbl_80563D00;  // 55 deg
extern const f32 lbl_80563D04;  // 60 deg

ModuleTag lbl_8055F470;
UnkStaticFFFF lbl_8055F478;
f32 lbl_8055F47C = lbl_80563CF4;  // default field of view
CamRec lbl_80499080[4];
f32 lbl_8055F49C = lbl_80563CF8;
CamMount lbl_803467E8[3] = {
    {{0.0f, 0.7f, 0.0f}, {0.0f, 0.8f, 9.0f}, lbl_80563CFC, 1},
    {{0.0f, -0.5f, -30.5f}, {0.0f, 2.1f, 1.0f}, lbl_80563D00, 1},
    {{0.1f, 1.1f, -0.2f}, {-9.5f, 2.5f, 4.2f}, lbl_80563D04, 0},
};

inline f32 Dot(f32 a, f32 b, f32 c, f32 d, f32 e, f32 f) { return e * f + (a * b + c * d); }
