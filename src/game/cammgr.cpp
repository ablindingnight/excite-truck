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
    CamMat3 rot;  // offset 0x0
    CamVec3 pos;  // offset 0x24
};

// One player's camera record.
struct CamRec {
    CamRec(void);  // fn_8026A600 (out of line, after this unit)
    u8 pad0[0x10];      // offset 0x0
    s32 mode;           // offset 0x10
    s32 nextMode;       // offset 0x14
    u8 pad18[0x4];      // offset 0x18
    s32 w1C;            // offset 0x1C
    s32 target;         // offset 0x20
    s32 nextTarget;     // offset 0x24
    u8 pad28[0x30];     // offset 0x28
    CamPose pose;       // offset 0x58
    CamPose lastPose;   // offset 0x88
    s32 lastFrame;      // offset 0xB8
    f32 fov;            // offset 0xBC
    CamPose carFrame;   // offset 0xC0
    u8 padF0[0x30];     // offset 0xF0
    u8 listener[0x18];  // offset 0x120
    u8 b138;           // offset 0x138
    u8 b139;           // offset 0x139
    u8 pad13A[0x2];
    f32 f13C;          // offset 0x13C
    u8* car;           // offset 0x140
    u8 pad144[0x10];   // offset 0x144
    f32 f154;          // offset 0x154
    f32 f158;          // offset 0x158
    u8 pad15C[0x44];   // offset 0x15C
    f32 f1A0;          // offset 0x1A0
    u8 pad1A4[0x48];   // offset 0x1A4
    s16 h1EC;          // offset 0x1EC
    u8 pad1EE[0x2];    // offset 0x1EE
    f32 f1F0;          // offset 0x1F0
    f32 prevFov;       // offset 0x1F4
    CamPose prevPose;  // offset 0x1F8
};

// Walks the ideal racing line for camera placement.
struct IdealFollower {
    u8 pad0[0x2C];   // offset 0x0
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

extern u8 lbl_80499950[];
extern u8 lbl_80499920[];

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
void fn_801EA994(u8* p);
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
        return lbl_80563C30 * x * x * x + lbl_80563C34 * x * x;
    }
    return lbl_80563C30 * x * x * x - lbl_80563C34 * x * x;
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

void fn_80266388(u8* p) {}  // stub (417)

void fn_80266A0C(u8* p) {}  // stub (421)

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
    ListenerRef l = {c->target, &c->pose, c->listener};
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

void fn_80268358(u8* p) {}  // stub (527)

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
    fn_80268BD4(lbl_80499920, PTR(f.line, 0x1C), 0.0f, 2.088f);
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
    fn_80268BD4(lbl_80499950, pos.seg, pos.t, 4.176f);
    if (f.buf) {
        delete[] f.buf;
    }
    f.buf = 0;
    fn_8013530C(&f, 0);
}

void fn_80268E94(CamRec* c, int idx) {}  // stub (344)

void fn_802693F4(CamRec* c, u8* own, int idx) {}  // stub (495)

void fn_80269BB0(u8* p) {}  // stub (400)

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

void fn_8026A2C8(u8* p) { fn_801EA994(p); }

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

void fn_8026A49C(u8* p) { fn_80269BB0(p); }

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

// A fixed camera mount: offset and look point in the car frame, field of view, enabled.
struct CamMount {
    f32 off[3];  // offset 0x0
    f32 look[3]; // offset 0xC
    f32 fov;     // offset 0x18
    u8 on;       // offset 0x1C
};

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
