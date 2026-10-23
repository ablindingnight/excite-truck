// Camera field of view (0x8026E870-0x80270AE8): per-mode FOV and distance tables, the widescreen
// FOV correction and the camera blend helpers ("physcam: %d != %d (tv = %d)"). Written against raw
// field offsets; the large routines and the initialiser are still stubs. Names are guesses apart from
// the strings.
#include "game/common.h"
#include "game/camframe.h"
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

// A screen rectangle; its out-of-line constructor (fn_80270AE8, in physcam.cpp) zeroes it.
class CamRect {
public:
    CamRect(void);
    f32 v[4];
};

extern CamRect lbl_8049A218[4];

extern "C" {

f32 PSVECDotProduct(const f32* a, const f32* b);
extern const f32 lbl_80563E30;  // minimum squared distance
extern const f32 lbl_80563E34;  // minimum horizontal offset

// Places the camera at `pos` looking at `target` with `up` as the up hint; nothing changes the frame
// when the target is too close or straight above/below.
void fn_8026E870(const f32* pos, const f32* target, u8* cam, const f32* up) {
    F32(cam, 0x7C) = pos[0];
    F32(cam, 0x80) = pos[1];
    F32(cam, 0x84) = pos[2];
    CsVec d;
    d.x = target[0] - F32(cam, 0x7C);
    d.y = target[1] - F32(cam, 0x80);
    d.z = target[2] - F32(cam, 0x84);
    if (PSVECDotProduct(&d.x, &d.x) < lbl_80563E30) {
        return;
    }
    if (AbsF(d.x) < lbl_80563E34 && AbsF(d.z) < lbl_80563E34) {
        return;
    }
    F32(cam, 0x70) = d.x;
    F32(cam, 0x74) = d.y;
    F32(cam, 0x78) = d.z;
    PSVECCrossProduct(up, (f32*)(cam + 0x70), (f32*)(cam + 0x58));
    PSVECCrossProduct((f32*)(cam + 0x70), (f32*)(cam + 0x58), (f32*)(cam + 0x64));
    Orthonormalize(cam);
}

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

// Camera pitch offset by mode. Draft: the original compiles to a jump table over 4..23.
f32 fn_8026EA8C(int mode, f32 t) {
    switch (mode) {
    case 5:
    case 7:
        return -10.0f;
    case 6:
        return -18.0f;
    case 23: {
        f32 v = -18.0f + 5.0f * t;
        if (v < -5.5f) {
            return v;
        }
        return -5.5f;
    }
    }
    return -5.5f;
}

f32 fn_8026EAEC(f32 fov) {
    if (fn_80178CA4() > 1) {
        f32 half = 0.5f;
        fov = 2.0f * (f32)atan(0.4f * (f32)tan(fov * half));
    }
    return fov;
}

// The AI driver of a car (aicar.cpp); slot 0x10 returns its current behaviour.
class CamDriver {
public:
    virtual void v08(void);
    virtual void v0C(int a);
    virtual u8* v10(void);
};

}  // extern "C"
int IsLoadingActive(void);
extern "C" {
CamDriver* fn_801B2ED0(int id);
int fn_801769F8(void);
extern f32 lbl_8055F528;  // fov range by speed
extern f32 lbl_8055F52C;
extern f32 lbl_8055F530;  // boost fov
extern f32 lbl_8055F534;  // normal fov
extern const f32 lbl_80563E50;
extern const f32 lbl_80563E5C;
extern const f32 lbl_80563E60;
extern const f32 lbl_80563E64;

static inline int Boosting(u8* car) { return F32(car, 0x258C) > lbl_80563E5C; }
static inline f32 LerpF(f32 t, f32 a, f32 b) { return a + t * (b - a); }
static inline int Above(f32 v, f32 k) { return v > k; }

// The view angle for the camera: blended from the last one (0x194) toward the speed-based angle, or
// toward the boost angle while boosting (not while an AI driver is in mode 1).
f32 fn_8026EB4C(u8* p) {
    f32 fov;
    f32 r = fn_8026EAEC(!Boosting(PTR(p, 0x140)) ? lbl_8055F534 : lbl_8055F530);
    fov = fn_8026EAEC(LerpF(F32(p, 0x13C), lbl_8055F528, lbl_8055F52C));
    u8* car = PTR(p, 0x140);
    int boost = 0;
    int on = Boosting(car) || car[0x259C];
    if (on && Above(F32(car, 0x25E0), lbl_80563E50)) {
        boost = 1;
    }
    if (S32(p, 0x4) != S32(p, 0x10)) {
        r = boost ? r : fov;
    } else if (fn_801769F8() || IsLoadingActive() || S32(PTR(p, 0x140), 0x2620) < 1) {
        r = F32(p, 0x194);
    } else if (boost && *(s32*)(fn_801B2ED0(S32(PTR(p, 0x140), 0x1EC))->v10() + 0x24) != 1) {
        r = F32(p, 0x194) + lbl_80563E60 * (r - F32(p, 0x194));
    } else {
        r = F32(p, 0x194) + lbl_80563E64 * (fov - F32(p, 0x194));
    }
    return r;
}

double cos(double x);
extern const f32 lbl_80563E68;    // 0.25

// Returns the view angle; scales *scale so that wider views keep the reference framing.
f32 fn_8026ECE4(u8* p, f32* scale) {
    f32 ref;
    f32 fov = fn_8026EB4C(p);
    if (fov > lbl_8055F52C) {
        ref = cos(lbl_8055F52C * lbl_80563E68);
        *scale *= (f32)cos(fov * lbl_80563E68) / ref;
    }
    return fov;
}

extern const f32 lbl_80563E6C;
extern const f32 lbl_80563E70;
extern const f32 lbl_80563E74;
extern const f32 lbl_80563E78;
extern const f32 lbl_80563E7C;
extern const f32 lbl_80563E80;

// Cubic ease of a 0..1 screen fraction (mirrored below the midpoint).
static inline f32 Ease(f32 x) {
    if (x < lbl_80563E6C) {
        return lbl_80563E6C;
    }
    if (x > lbl_80563E30) {
        return lbl_80563E30;
    }
    if (x > lbl_80563E78) {
        return lbl_80563E70 * x * x * x + lbl_80563E74 * x * x;
    }
    return lbl_80563E70 * x * x * x - lbl_80563E74 * x * x;
}

// Adjusts the camera position for the player's split-screen viewport: lifts it by `lift` scaled with
// the viewport's height, then pulls it toward the target by its vertical placement.
void fn_8026ED64(const s32* cam, f32 lift, f32* pos, f32* target) {
    CamRect* r = &lbl_8049A218[*cam];
    f32 e = Ease(r->v[3]);
    f32 t = (r->v[1] - lbl_80563E78) / lbl_80563E7C;
    t = lbl_80563E30 < t ? lbl_80563E30 : t;
    t = t > lbl_80563E78 ? t : lbl_80563E78;
    f32 k = lbl_80563E78 + t * lbl_80563E30;
    pos[1] = pos[1] + lift * (lbl_80563E80 * (k * e));
    CsVec d;
    d.x = target[0] - pos[0];
    d.y = target[1] - pos[1];
    d.z = target[2] - pos[2];
    f32 s = lbl_80563E6C - lbl_80563E50 * Ease(r->v[3]);
    pos[0] = target[0] + d.x * s;
    pos[1] = target[1] + d.y * s;
    pos[2] = target[2] + d.z * s;
}

extern const f32 lbl_80563E4C;
extern const f32 lbl_80563E84;  // ground sample spacing
extern const f32 lbl_80563E88;
extern const f32 lbl_80563E8C;
extern const f32 lbl_80563E90;
extern const f32 lbl_80563E94;
extern const f32 lbl_80563E98;
extern const f32 lbl_80563E9C;
extern const f32 lbl_80563EA0;
extern const f32 lbl_80563EA8;
f32 fn_801E9FDC(const s16* hint, f32 x, f32 z);
int fn_801784DC(f32* pos, f32* h, int a, int b, int (*filter)(int));
f32 fn_8026A520(int i);
int fn_8026F408(int n);

// A plain (copyable) vector.
struct CVec3 {
    f32 x, y, z;
};

// Raises a ground height to any obstacle top found at (x, z).
static inline f32 OverObstacle(f32 h, f32 x, f32 y, f32 z) {
    CsVec q(x, y, z);
    f32 top;
    if (fn_801784DC(&q.x, &top, 0, 0, fn_8026F408)) {
        h = h > top ? h : top;
    }
    return h;
}

// Per-frame camera height tracking: averages the ground around the car into the player's rect
// (lbl_8049A218), eases the rect's lift/zoom terms, then lifts pos/target and applies fn_8026ED64.
void fn_8026EF48(const s32* cam, f32* pos, f32* target) {
    CamRect* r = &lbl_8049A218[*cam];
    u8* car = PTR(cam, 0x140);
    if (!fn_801769F8()) {
        CVec3 c = *(CVec3*)(car + 0x60);
        const s16* hint = (const s16*)(car + 0x6C);
        bool onGround = S32(car, 0x2620) > 0;
        f32 y = c.y;
        f32 z = c.z;
        f32 h0 = fn_801E9FDC(hint, lbl_80563E84 + c.x, lbl_80563E84 + z);
        h0 = OverObstacle(h0, lbl_80563E84 + c.x, y, lbl_80563E84 + z);
        f32 h1 = fn_801E9FDC(hint, c.x - lbl_80563E84, lbl_80563E84 + z);
        h1 = OverObstacle(h1, c.x - lbl_80563E84, y, lbl_80563E84 + z);
        f32 h2 = fn_801E9FDC(hint, c.x - lbl_80563E84, z - lbl_80563E84);
        h2 = OverObstacle(h2, c.x - lbl_80563E84, y, z - lbl_80563E84);
        f32 h3 = fn_801E9FDC(hint, lbl_80563E84 + c.x, z - lbl_80563E84);
        h3 = OverObstacle(h3, lbl_80563E84 + c.x, y, z - lbl_80563E84);
        f32 avg = lbl_80563E68 * (h0 + (h1 + (h2 + h3)));
        if (onGround) {
            r->v[0] = avg - c.y;
            r->v[2] = lbl_80563E78;
        } else {
            r->v[2] = r->v[2] + lbl_80563E88;
        }
        f32 t = (r->v[2] - lbl_80563E78) / lbl_80563E30;
        f32 h = r->v[0] + (c.y - avg);
        t = lbl_80563E30 < t ? lbl_80563E30 : t;
        t = t > lbl_80563E78 ? t : lbl_80563E78;
        f32 up = lbl_80563E88 * (lbl_80563E8C + t * lbl_80563E90);
        f32 down = onGround ? lbl_80563E94 : lbl_80563E98;
        if (h < r->v[1]) {
            f32 lo = r->v[1] - down;
            h = h > lo ? h : lo;
            r->v[1] = h;
        } else {
            f32 hi = r->v[1] + up;
            h = h < hi ? h : hi;
            r->v[1] = h;
        }
        f32 zoom = fn_8026A520(S32(car, 0x1EC)) > lbl_80563E78;
        if (zoom != lbl_80563E78) {
            f32 v = lbl_80563E9C + r->v[3];
            r->v[3] = v < lbl_80563E30 ? v : lbl_80563E30;
        } else {
            f32 v = r->v[3] - lbl_80563EA0;
            r->v[3] = v > lbl_80563E78 ? v : lbl_80563E78;
        }
    }
    f32 lift = lbl_80563E78 > r->v[1] ? lbl_80563E78 : r->v[1];
    f32 top = lbl_8049A208[3];
    f32 m = lbl_80563EA8 + lbl_8049A208[2];
    top = top > m ? top : m;
    f32 k = (lift - lbl_8049A208[2]) / (top - lbl_8049A208[2]);
    k = lbl_80563E30 < k ? lbl_80563E30 : k;
    k = k > lbl_80563E78 ? k : lbl_80563E78;
    k = lbl_80563E78 + k * lbl_80563E30;
    pos[1] = pos[1] + lbl_8049A208[0] * k;
    target[1] = target[1] + lbl_8049A208[1] * k;
    fn_8026ED64(cam, lbl_80563E30 < lbl_80563E4C ? lbl_80563E30 : lbl_80563E4C, pos, target);
}

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
    f32* q = (f32*)lbl_8049A218;
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

f32 fn_802704E0(int mode, f32 t) { return fn_8026EA8C(mode, t); }

void fn_802704E4(u8* p) {}  // stub (360)

}  // extern "C"

extern const f32 lbl_80563F2C;
extern const f32 lbl_80563F30;
extern const f32 lbl_80563F34;
extern const f32 lbl_80563F38;

ModuleTag lbl_8055F520;
f32 lbl_8055F528 = lbl_80563F2C;
f32 lbl_8055F52C = lbl_80563F30;  // reference field of view
f32 lbl_8055F530 = lbl_80563F34;
f32 lbl_8055F534 = lbl_80563F38;
CamRect lbl_8049A218[4];
