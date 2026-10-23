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
    u8 pad0[0x58];     // offset 0x0
    CamPose pose;      // offset 0x58
    u8 pad88[0x34];    // offset 0x88
    f32 fov;           // offset 0xBC
    u8 padC0[0x168];   // offset 0xC0
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
void fn_80268BD4(u8* rec, u8* seg, f32 t, f32 k);
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
void fn_802734F0(u8* p);
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
extern u8 lbl_80499080[];
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

void fn_80265EC4(u8* p) {}  // stub (107)

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

void fn_80266170(u8* p) {}  // stub (90)

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
    F32(p, 0x48) = 3.0f + -2.5f * c;
    F32(p, 0x4C) = lbl_8055F484 * (fn_80178CA4() == 1 ? 1.0f : 0.8f);
}

void fn_802671CC(u8* p) {}  // stub (149)

void fn_80267420(u8* p) {}  // stub (152)

void fn_80267680(u8* p) {}  // stub (119)

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

void fn_802678E8(u8* p) {}  // stub (342)

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

void fn_80267EE0(u8* p) { fn_802734F0(p); }

u8* fn_80267EE4(int i) { return lbl_80499080 + i * 0x228; }

void fn_80267EF8(u8* p) {}  // stub (238)

void fn_802682B0(u8* p) {
    if (U8(p, 0x138)) {
        lbl_80499050.m[0][0] = 1.0f;
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

int fn_80268BA4(int mode) {
    switch (mode) {
    case 4:
    case 5:
    case 6:
    case 8:
    case 9:
    case 23:
        return 1;
    }
    return 0;
}

void fn_80268BD4(u8* rec, u8* seg, f32 t, f32 k) {}  // stub (112)

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

void fn_80268E94(u8* p) {}  // stub (344)

void fn_802693F4(u8* p) {}  // stub (495)

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
    u8* e = lbl_80499080 + i * 0x228;
    *(Words12*)(e + 0x1A4) = *s;
    F32(e, 0x1D4) = v;
}

u32 fn_8026A344(int i) { return U32(fn_80267EE4(i), 0x20); }

u32 fn_8026A368(int i) { return U32(fn_80267EE4(i), 0x10); }

u8 fn_8026A38C(void) { return lbl_8055F488; }

void fn_8026A394(u8* p) {}  // stub (66)

void fn_8026A49C(u8* p) { fn_80269BB0(p); }

void fn_8026A4A0(int i, CamPose* out, f32* fov) {
    CamRec* e = &((CamRec*)lbl_80499080)[i];
    out->rot = e->pose.rot;
    out->pos = e->pose.pos;
    *fov = e->fov;
}

void fn_8026A51C(u8* p) { fn_802693F4(p); }

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

void fn_8026A584(u8* p) {}  // stub (31)
}  // extern "C"
