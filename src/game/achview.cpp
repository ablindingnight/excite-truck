// Achievements viewer (0x80259148-0x8025C028): the trophy cabinet ("trophy", "cnt1".."cnt3",
// "MenuDoAchievementsViewer: bad return value %d ...") with its camera orbit and trophy spin helpers.
// Written against raw field offsets; the large routines and the initialiser are still stubs. Names
// are guesses apart from the strings.
#include "game/common.h"
#include "macros.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

// Interface of UIObjectsImpl (uiobjs.cpp).
class UIObjects {
public:
    virtual ~UIObjects(void);
    virtual void AddImage(const char* name, const char* file, int frame);
    virtual void AddFont(const char* name, const char* file);
    virtual void AddSound(const char* name, const char* file, u8 loop);
    virtual void s18(void);
    virtual void s1C(void);
    virtual void s20(void);
    virtual void s24(void);
    virtual void s28(void);
    virtual void s2C(void);
    virtual void s30(void);
    virtual void s34(void);
    virtual void PlaySound(const char* name);               // 0x38
    virtual void s3C(void);
    virtual void SetSoundVolume(const char* name, f32 v);   // 0x40
};

class VA {
public:
    virtual void v08(void);
    virtual void v0C(u8* q);
};

extern "C" {
void fn_80236914(void);
void fn_802C6CF8(s32 h);
void fn_802CAC54(u8* p);
void fn_80214AF0(int s);
void fn_80214A9C(int s);
f32 PSVECDotProduct(const f32* a, const f32* b);
double sqrt(double x);
double sin(double x);
}

const char* AchievementsGetLevelModel(int cat, int level);

extern "C" int fn_801345D8(char* buf, int n, const char* fmt, ...);

// Copies at most n characters, always terminating.
static inline void CopyName(const char* s, int n, char* d) {
    while ((*d++ = *s++) != 0) {
        if (--n <= 0) {
            d[-1] = 0;
            break;
        }
    }
}

// Shows one trophy: its model name and its level caption.
static inline void SetTrophy(u8* self, s32 cat, s32 level) {
    u8* m = PTR(self, 0x18);
    S32(m, 0x4C) = level;
    const char* name = AchievementsGetLevelModel(cat, level);
    if (name) {
        CopyName(name, 50, (char*)(m + 0x4));
    } else {
        U8(m, 0x4) = 0;
    }
    if (level > 0) {
        fn_801345D8((char*)(m + 0x36), 0x14, "Level %d", level);
    } else {
        U8(m, 0x36) = 0;
    }
}

struct Words9 {
    u32 w[9];
};

extern UIObjects* lbl_8055F394;
extern "C" UIObjects* fn_80258354(void);
extern void* lbl_80345158[];
extern bool lbl_8055F3A0;
extern f32 lbl_8055F3A4;

extern "C" {

void fn_80259148(void) {
    lbl_8055F394 = fn_80258354();
    lbl_8055F394->AddSound("achieve_enter", "a_achieve.sfx", 0);
    lbl_8055F394->AddSound("achieve_trophy", "a_trophy1.sfx", 0);
    lbl_8055F394->AddSound("achieve_tally", "a_tally.sfx", 1);
}

void fn_802591D8(void) {
    delete lbl_8055F394;
    lbl_8055F394 = 0;
}

u32 fn_8025921C(void) { return 0; }

void fn_80259224(u8* p) {}  // stub (279)

u8* fn_80259680(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_80345158;
        if (S32(p, 0x50) != -1) {
            fn_802C6CF8(S32(p, 0x50));
            S32(p, 0x50) = -1;
        }
        fn_802CAC54(PTR(p, 0xA8));
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

u8* fn_80259700(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_80259740(int x, f32 v) {
    fn_80236914();
    lbl_8055F3A4 = v;
    lbl_8055F3A0 = x == 2;
}

void fn_8025978C(u8* p) {}  // stub (654)

u8* fn_8025A1C4(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_8025A204(u8* p) {}  // stub (178)

u8* fn_8025A4CC(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_8025A50C(u8* p) {}  // stub (204)

void fn_8025A83C(u8* p) {
    F32(p, 0x84) = 1.0f;
    F32(p, 0x88) = 0.0f;
    F32(p, 0x8C) = 0.0f;
    F32(p, 0x90) = 0.0f;
    F32(p, 0x94) = 1.0f;
    F32(p, 0x98) = 0.0f;
    F32(p, 0x9C) = 0.0f;
    F32(p, 0xA0) = 0.0f;
    F32(p, 0xA4) = 1.0f;
    ((VA*)p)->v0C(p + 4);
}

void fn_8025A87C(u8* p) {
    F32(p, 0x84) = 1.0f;
    F32(p, 0x88) = 0.0f;
    F32(p, 0x8C) = 0.0f;
    F32(p, 0x90) = 0.0f;
    F32(p, 0x94) = 1.0f;
    F32(p, 0x98) = 0.0f;
    F32(p, 0x9C) = 0.0f;
    F32(p, 0xA0) = 0.0f;
    F32(p, 0xA4) = 1.0f;
    U8(p, 0x4) = 0;
    U8(p, 0x36) = 0;
}

}  // extern "C"

// Appends s to d (n bytes in all), always terminating.
static inline void CatName(const char* s, int n, char* d) {
    while (*d) {
        n--;
        d++;
    }
    if (n > 0) {
        CopyName(s, n, d);
    }
}

extern char lbl_80345208[];  // "*uiaction.res/"

extern "C" {
char* strrchr(const char* s, int c);
int fn_8012C33C(const char* name);
s32 fn_802C6FC0(const char* name, int a, int b);

// Loads the trophy model "*uiaction.res/<name>.mo0" (replacing the old one) and resets its rotation.
void fn_8025A8B8(u8* p, const char* name) {
    if (S32(p, 0x50) != -1) {
        fn_802C6CF8(S32(p, 0x50));
        S32(p, 0x50) = -1;
    }
    if (*name && strlen(name)) {
        char buf[64];
        CopyName(lbl_80345208, 64, buf);
        CatName(name, 64, buf);
        if (strrchr(buf, '.')) {
            char* dot = strrchr(buf, '.');
            *dot = 0;
        }
        CatName(".mo0", 64, buf);
        if (fn_8012C33C(buf)) {
            S32(p, 0x50) = fn_802C6FC0(buf, 0, 0);
        }
    }
    F32(p, 0x84) = 1.0f;
    F32(p, 0x88) = 0.0f;
    F32(p, 0x8C) = 0.0f;
    F32(p, 0x90) = 0.0f;
    F32(p, 0x94) = 1.0f;
    F32(p, 0x98) = 0.0f;
    F32(p, 0x9C) = 0.0f;
    F32(p, 0xA0) = 0.0f;
    F32(p, 0xA4) = 1.0f;
}

}  // extern "C"

// Pad state as read by fn_801298E8 (partial).
struct ViewPad {
    f32 x;            // offset 0x0
    f32 y;            // offset 0x4
    u8 pad8[0x38];
    u32 hold;         // offset 0x40
    u8 pad44[0xC];
    u32 flags;        // offset 0x50, 0x20: pointer direction valid
    f32 dir[3];       // offset 0x54
    u8 pad60[0x4];
};

struct Mat3 {
    f32 m[9];
};

f32 Max(f32 a, f32 b);
f32 Remap(f32 lo, f32 hi, f32 a, f32 b, f32 x);
void Mat3RotY(Mat3* m, f32 a);
void Mat3Mul(Mat3* out, const Mat3* a, const Mat3* b);

extern f32 lbl_8055F398;     // trophy spin angle
extern f32 lbl_80498EAC[3];  // smoothed view direction

extern "C" {
int fn_801298E8(ViewPad* pad, int chan);
void StatsUpdate__Fv(f32* v);  // vector default constructor (empty, out of line)
void fn_80144DAC(f32* dst, const f32* src);
f32 fn_80144CC8(f32 x);
void fn_801BFEBC(f32* v, f32 x, f32 y, f32 z);
void fn_801F3198(f32* v);
void fn_80144B44(f32* v, f32 x, f32 y, f32 z);
f32 fn_8017F70C(f32 x);
void fn_801C0E4C(f32* out, const f32* a, const f32* b);
Mat3* fn_802C86DC(void);
void fn_8025AD3C(f32* out, const f32* a, const f32* b, f32 t);
f32 fn_8025AD88(const f32* a, const f32* b);
void fn_8025AD8C(u8* self, f32* out, const f32* a, const f32* b, f32 t);
f32 fn_8025ADF4(f32 x);

static inline f32 LerpF(f32 a, f32 b, f32 t) { return a + t * (b - a); }

// Turns the trophy: the +/- buttons spin it, the stick or pointer tilts it, then the frame at p + 0x84
// (right, up, forward) is rebuilt around the tilted up vector.
void fn_8025AA90(u8* p, f32 dt) {
    ViewPad pad;
    Mat3 rot;
    f32 r[3];
    f32 c[3];
    f32 b[3];
    f32 a[3];
    f32 fwd[3];
    f32 side[3];
    f32 v[3];
    if (S32(p, 0x50) == -1 || !U8(p, 0x4A)) {
        return;
    }
    if (fn_801298E8(&pad, 0)) {
        f32 plus = (pad.hold & 2) ? 1.0f : 0.0f;
        f32 minus = (pad.hold & 8) ? -1.0f : 0.0f;
        lbl_8055F398 += dt * (3.1415927f * (minus + plus));
        StatsUpdate__Fv(v);
        if (pad.flags & 0x20) {
            fn_80144DAC(v, pad.dir);
        } else {
            fn_801BFEBC(v, pad.x, -fn_8025ADF4(Max(0.0f, 1.0f - fn_80144CC8(pad.x) + fn_80144CC8(pad.y))), pad.y);
        }
        fn_8025AD8C(p + 0x84, lbl_80498EAC, lbl_80498EAC, v, dt);
        fn_801BFEBC((f32*)(p + 0x90), lbl_80498EAC[0], -lbl_80498EAC[1], lbl_80498EAC[2]);
        fn_801F3198((f32*)(p + 0x90));
    }
    fn_80144B44(side, 1.0f, 0.0f, 0.0f);
    fn_80144B44(fwd, 0.0f, 0.0f, 1.0f);
    f32 tilt = fn_8017F70C(fn_8025AD88(side, (f32*)(p + 0x90)));
    fn_8017F70C(fn_8025AD88((f32*)(p + 0x90), fwd));
    if (tilt > 0.707f) {
        StatsUpdate__Fv(a);
        StatsUpdate__Fv(b);
        StatsUpdate__Fv(c);
        fn_801C0E4C(c, side, (f32*)(p + 0x90));
        fn_801C0E4C(a, (f32*)(p + 0x90), c);
        fn_801C0E4C(b, (f32*)(p + 0x90), fwd);
        fn_8025AD3C(r, a, b, Remap(0.707f, 0.996f, 0.0f, 1.0f, tilt));
        fn_801C0E4C((f32*)(p + 0x9C), r, (f32*)(p + 0x90));
        fn_801F3198((f32*)(p + 0x9C));
        fn_801C0E4C((f32*)(p + 0x84), (f32*)(p + 0x90), (f32*)(p + 0x9C));
    } else {
        fn_801C0E4C((f32*)(p + 0x9C), side, (f32*)(p + 0x90));
        fn_801F3198((f32*)(p + 0x9C));
        fn_801C0E4C((f32*)(p + 0x84), (f32*)(p + 0x90), (f32*)(p + 0x9C));
    }
    Mat3RotY(&rot, lbl_8055F398);
    Mat3Mul((Mat3*)(p + 0x84), &rot, (Mat3*)(p + 0x84));
    Mat3Mul((Mat3*)(p + 0x84), (Mat3*)(p + 0x84), fn_802C86DC());
}

void fn_8025AD3C(f32* out, const f32* a, const f32* b, f32 t) {
    f32 dz = b[2] - a[2];
    f32 dy = b[1] - a[1];
    f32 dx = b[0] - a[0];
    out[2] = a[2] + t * dz;
    out[1] = a[1] + t * dy;
    out[0] = a[0] + t * dx;
}

f32 fn_8025AD88(const f32* a, const f32* b) { return PSVECDotProduct(a, b); }

void fn_8025AD8C(u8* self, f32* out, const f32* a, const f32* b, f32 t) {
    f32 s = 4.1f * t;
    f32 k = 1.0f >= s ? s : 1.0f;
    out[0] = a[0] + k * (b[0] - a[0]);
    out[1] = a[1] + k * (b[1] - a[1]);
    out[2] = a[2] + k * (b[2] - a[2]);
}

f32 fn_8025ADF4(f32 x) { return (f32)sqrt(x); }

}  // extern "C"

struct Perspective {
    Perspective(f32 near, f32 far, f32 fov);
    ~Perspective(void);
    f32 near;  // offset 0x0
    f32 far;   // offset 0x4
    f32 fov;   // offset 0x8
};

struct LightDir {
    f32 x, y, z;
};

extern f32 lbl_8055F39C;  // trophy view field of view

extern "C" {
u8* fn_8025B134(u8* p);
void fn_8025B0E8(Words9* d, const Words9* s);
f32 fn_8025B0A8(f32* r);
f32 fn_8025B0C4(f32 x);
u8* fn_8022EDC4(void);
void fn_80144D80(Mat3* m, f32 x, f32 y, f32 z);
void fn_802C77F0(s32 model, f32* bounds);
void fn_801C2B9C(f32* out, const f32* pos, const f32* dir, f32 d);
void fn_801E67B4(f32* out, const f32* v, const u8* m);
void fn_801C0E60(f32* out, const f32* a, const f32* b);
void fn_801BFE7C(f32* out, const f32* a, const f32* b);
void fn_802E30C8(const char* fmt, ...);
u32 fn_802C9950(u32 c);
u32 fn_802C9994(u32 c);
f32 fn_802C99E8(f32 v);
LightDir* fn_802C9A40(void);
void fn_802C9A1C(const LightDir* d);
void fn_802D7554(int on);
void fn_802CA294(s32 tex);
void fn_802D9230(void);
void fn_802C744C(s32 model, u8* m);

// Trophy size per level.
struct TrophySizes {
    f32 s[5];
};

static const TrophySizes kTrophySizes = {0.75f, 0.8f, 0.85f, 0.95f, 1.0f};

// Draws the trophy model: scaled to the view, placed in front of the camera so its size class
// fills the screen, lit from a fixed direction.
void fn_8025AE18(u8* p) {
    Mat3 scale;
    TrophySizes sizes;
    f32 bounds[6];
    LightDir dir;
    if (S32(p, 0x50) == -1 || !U8(p, 0x4A)) {
        return;
    }
    fn_8025B0E8((Words9*)(p + 0x54), (Words9*)fn_8025B134(p + 0x84));
    u8* cam = fn_8022EDC4();
    fn_80144D80(&scale, F32(cam, 0x38), F32(cam, 0x38), F32(cam, 0x38));
    Mat3Mul((Mat3*)(p + 0x54), (Mat3*)(p + 0x54), &scale);
    fn_802C77F0(S32(p, 0x50), bounds);
    sizes = kTrophySizes;
    int level = S32(p, 0x4C);
    if (level <= 0) {
        fn_802E30C8("AchievementTrophyModel: bad level value %d", level);
    } else {
        f32 r = 4.3f * sizes.s[level - 1] / 2.0f;
        f32 d = r / fn_8025B0C4(0.5f * lbl_8055F39C);
        f32 nearD = 0.1f + r;
        u8* m = (u8*)fn_802C86DC();
        fn_801C2B9C((f32*)(p + 0x78), (f32*)(m + 0x24), (f32*)(m + 0x18), Max(d, nearD));
        f32 c[3];
        StatsUpdate__Fv(c);
        fn_801BFEBC(c, fn_8025B0A8(bounds), fn_8025B0A8(bounds + 2), fn_8025B0A8(bounds + 4));
        fn_801E67B4(c, c, p + 0x54);
        fn_801C0E60((f32*)(p + 0x78), (f32*)(p + 0x78), c);
        fn_801BFE7C((f32*)(p + 0x78), (f32*)(p + 0x78), (f32*)(cam + 0x24));
    }
    dir.x = 0.2f;
    dir.y = -0.7f;
    dir.z = 0.7f;
    u32 oldAmb = fn_802C9950(0xFF646464);
    u32 oldDiff = fn_802C9994(0xFFB4B4B4);
    f32 oldSpec = fn_802C99E8(1.0f);
    LightDir saved = *fn_802C9A40();
    fn_802C9A1C(&dir);
    fn_802D7554(1);
    fn_802CA294(S32(p, 0xA8));
    fn_802D9230();
    Perspective persp(0.1f, 100.0f, lbl_8055F39C);
    fn_802C744C(S32(p, 0x50), p + 0x54);
    fn_802D7554(0);
    fn_802C9A1C(&saved);
    fn_802C99E8(oldSpec);
    fn_802C9994(oldDiff);
    fn_802C9950(oldAmb);
}

extern const f32 lbl_80563B1C;  // 0.5

f32 fn_8025B0A8(f32* r) {
    f32 d = r[1] - r[0];
    f32 out = r[0];
    out += d * lbl_80563B1C;
    return out;
}

f32 fn_8025B0C4(f32 x) { return (f32)sin(x); }

void fn_8025B0E8(Words9* d, const Words9* s) { *d = *s; }

u8* fn_8025B134(u8* p) { return p; }

void fn_8025B138(u8* p) {
    s32 level = S32(p, 0x1C);
    S32(p, 0x74) = level;
    SetTrophy(p, S32(p, 0x70), level);
    fn_801345D8((char*)(p + 0x20), 0x50, "%s", AchievementsGetLevelModel(S32(p, 0x70), S32(p, 0x74)));
}

void fn_8025B22C(u8* p) {
    for (int i = 1; i <= 5; i++) {
        if (S32(p, 0x74) == i) {
            fn_80214AF0(i + 5);
        } else {
            fn_80214A9C(i + 5);
        }
        if (i <= S32(p, 0x1C)) {
            fn_80214AF0(i);
            fn_80214A9C(i + 10);
        } else {
            fn_80214AF0(i + 10);
            fn_80214A9C(i);
        }
    }
}

}  // extern "C"

extern "C" void fn_80216D28(void);
extern "C" void fn_80214D50(int s);

static inline int MaxI(int a, int b) { return a > b ? a : b; }
static inline int MinI(int a, int b) { return a < b ? a : b; }

// Moves the shown level by d within [1, unlocked]; a bump sound when it can't move.
static inline void StepLevel(u8* p, int d) {
    int old = S32(p, 0x74);
    S32(p, 0x74) = old + d;
    S32(p, 0x74) = MinI(MaxI(S32(p, 0x74), 1), S32(p, 0x1C));
    if (old != S32(p, 0x74)) {
        SetTrophy(p, S32(p, 0x70), S32(p, 0x74));
        fn_801345D8((char*)(p + 0x20), 0x50, "%s", AchievementsGetLevelModel(S32(p, 0x70), S32(p, 0x74)));
    } else {
        fn_80216D28();
        fn_80214D50(1);
    }
}

extern "C" {

int fn_8025B2BC(u8* p, int key) {
    switch (key) {
    case 0x128:
        StepLevel(p, -1);
        return 1;
    case 0x126:
        StepLevel(p, 1);
        return 1;
    }
    return 0;
}

}  // extern "C"

int AchievementsGetLevel(int category, s32 value);
s32 AchievementsGetLevelThreshold(int category, int level);

extern "C" {
void fn_80130FCC(char* buf, s32 size, s32 value);
extern f32 lbl_80558DC4[];  // FLT_EPSILON

// An achievement row: its value, level, next threshold and trophy model name.
void fn_8025B520(u8* p) {
    int show;
    s32 value;
    F32(p, 0x28) = (f32)S32(p, 0x1C);
    value = (s32)(F32(p, 0x28) + lbl_80558DC4[0]);
    fn_80130FCC((char*)(p + 0x2C), 0x14, value);
    show = 1;
    if (U8(p, 0xC4) && (U8(p, 0x58) || !U8(p, 0xC5))) {
        show = 0;
    }
    int level = AchievementsGetLevel(S32(p, 0x18), value);
    S32(p, 0xC8) = level;
    s32 next = AchievementsGetLevelThreshold(S32(p, 0x18), level + 1);
    if (next >= 0) {
        fn_80130FCC((char*)(p + 0x40), 0x14, next);
    } else {
        CopyName("", 0x14, (char*)(p + 0x40));
    }
    const char* model = AchievementsGetLevelModel(S32(p, 0x18), S32(p, 0xC8));
    if (model && show) {
        CopyName(model, 50, (char*)(p + 0x60));
    } else {
        CopyName("", 50, (char*)(p + 0x60));
    }
    U8(p, 0x58) = 1;
    U8(p, 0x59) = 0;
    S32(p, 0x5C) = -1000;
    if (U8(p, 0xC4)) {
        lbl_8055F394->SetSoundVolume("achieve_tally", 0.0f);
        lbl_8055F394->PlaySound("achieve_tally");
    }
    fn_80214A9C(0x10);
    fn_80214A9C(0xD);
    CopyName((const char*)(p + 0x60), 50, (char*)(p + 0x92));
}

void fn_8025B784(u8* p) {}

void fn_8025B788(u8* p) {}  // stub (285)

}  // extern "C"

// True while the tally animation (f_cc) still runs.
static inline bool Tallying(u8* p) { return F32(p, 0xCC) < 1.5f && U8(p, 0xC5); }

extern "C" {
void fn_80216CA8(void);

int fn_8025BBFC(u8* p, int key) {
    if (U8(p, 0x58) || Tallying(p)) {
        fn_80216D28();
    }
    switch (key) {
    case 0xD:
        if (U8(p, 0xC4)) {
            if (U8(p, 0x58)) {
                F32(p, 0x28) = (f32)S32(p, 0x20);
            } else if (!Tallying(p)) {
                U8(p, 0x59) = 1;
                S32(p, 0x5C) = -2;
            }
        } else if ((S32(p, 0xC8) > 1 || fn_8025921C()) && !Tallying(p)) {
            S32(p, 0x5C) = 3;
            U8(p, 0x59) = 1;
        } else {
            fn_80214D50(1);
            return 0;
        }
        break;
    case 0x1B:
        if (!U8(p, 0xC4)) {
            U8(p, 0x59) = 1;
            S32(p, 0x5C) = -1;
        } else {
            return 0;
        }
        break;
    case 0x126:
        if (!U8(p, 0xC4)) {
            U8(p, 0x59) = 1;
            S32(p, 0x5C) = 1;
            fn_80216CA8();
        } else {
            return 0;
        }
        break;
    case 0x128:
        if (!U8(p, 0xC4)) {
            U8(p, 0x59) = 1;
            S32(p, 0x5C) = 2;
            fn_80216CA8();
        } else {
            return 0;
        }
        break;
    default:
        return 0;
    }
    return 1;
}

void fn_8025BE0C(u8* p) {
    s32 level = S32(p, 0x24);
    SetTrophy(p, S32(p, 0x20), level);
}

void fn_8025BED0(u8* p) {
    u8* o = PTR(p, 0x1C);
    if (U8(o, 0xC5) && !U8(o, 0x58)) {
        s32 level = S32(o, 0xC8);
        S32(p, 0x24) = level;
        s32 cat = S32(p, 0x20) = S32(o, 0x18);
        SetTrophy(p, cat, level);
        U8(PTR(p, 0x18), 0x4A) = 1;
    }
}

}  // extern "C"

ModuleTag lbl_8055F388;
UnkStatic123s lbl_80498E98;
UnkStatic7F lbl_8055F390;
extern const f32 lbl_80563B38;
f32 lbl_8055F39C = lbl_80563B38;
