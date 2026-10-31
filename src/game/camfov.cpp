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

// Mode set as a bit mask over modes 4..23 (the original tests a mask, not a switch).
#define CAM_MODE_BIT(m) (1 << ((m) - 4))

int fn_8026EA14(int mode) {
    int r = 0;
    u32 i = mode - 4;
    if (i <= 19 && ((1 << i) & (CAM_MODE_BIT(4) | CAM_MODE_BIT(5) | CAM_MODE_BIT(6) | CAM_MODE_BIT(8) | CAM_MODE_BIT(23)))) {
        r = 1;
    }
    return r;
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
f32 fn_8026EB4C(const u8* p) {
    f32 fov;
    f32 r = fn_8026EAEC(!Boosting(PTR(p, 0x140)) ? lbl_8055F534 : lbl_8055F530);
    f32 lo = lbl_8055F528;
    fov = fn_8026EAEC(LerpF(F32(p, 0x13C), lo, lbl_8055F52C));
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
        return x * (lbl_80563E70 * x * x) + lbl_80563E74 * x * x;
    }
    return x * (lbl_80563E70 * x * x) - lbl_80563E74 * x * x;
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
        f32 h = r->v[0] + (c.y - avg);
        f32 rv = r->v[2];
        f32 t = (rv - lbl_80563E78) / lbl_80563E30;
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

extern const f32 lbl_80563EC0;  // 4.0
extern const f32 lbl_80563ED0;  // 0.35
extern const f32 lbl_80563ED8;  // 8.0
extern const f32 lbl_80563E44;  // -10.0
extern const f32 lbl_80563E58;  // 2.0
extern const f32 lbl_80563EA4;  // 0.2
extern const f32 lbl_80563F00;  // 4/3
extern const f32 lbl_80563F04;  // 80/3
extern const f32 lbl_80563F08;  // -0.5
extern const f32 lbl_80563F0C;  // 0.9
extern const f32 lbl_80563F14;  // 0.7
extern const f32 lbl_80563F18;  // -13.0
extern const f32 lbl_80563F20;  // 2.1
extern const f32 lbl_80563F24;  // 2.5
extern const f32 lbl_80563F28;
extern f32 lbl_80558DC4[];  // FLT_EPSILON
f32 PSVECMag(const f32* v);
int fn_801EA03C(const s16* hint, f32* pos);

// A vector copied as words (the original's struct copies of these move lwz/stw pairs).
struct PVec3 {
    f32 v[3];
};

// A camera frame's rows (right, up, view axis) without its position.
struct CamAxes {
    CVec3 right, up, fwd;
};

static inline f32 AddMul(f32 a, f32 b, f32 c) { return a + c * b; }
static inline f32 MulF(f32 a, f32 b) { return a * b; }

// A camera frame: its rows and its position.
struct CamFrame {
    CamAxes axes;  // offset 0x0
    PVec3 pos;     // offset 0x24
};

// The chase/follow camera (fields used by fn_8026F420 and fn_802704E4; the rest is camstate's).
struct ChaseCam {
    u8 pad0[0x4];
    s32 mode;           // offset 0x4
    f32 pitchT;         // offset 0x8
    f32 blend;          // offset 0xC
    u8 pad10[0x8];
    s32 lastMode;       // offset 0x18
    u8 pad1C[0x3C];
    CamFrame view;      // offset 0x58: rows and eye (0x7C)
    u8 pad88[0x34];
    f32 fov;            // offset 0xBC
    CamAxes axes;       // offset 0xC0: the car's frame
    PVec3 pos;          // offset 0xE4
    CamFrame tf;        // offset 0xF0: the trailing frame
    f32 move[3];        // offset 0x120
    f32 vel[3];         // offset 0x12C
    u8 reset;           // offset 0x138
    u8 hold;            // offset 0x139
    u8 pad13A[0x2];
    f32 speedFrac;      // offset 0x13C
    u8* car;            // offset 0x140
    u8 pad144[0x48];
    f32 bump;           // offset 0x18C
    f32 time;           // offset 0x190
    u8 pad194[0x4];
    f32 smooth;         // offset 0x198
    f32 lastOn;         // offset 0x19C
};

extern const f32 lbl_80563EB8;  // 6.0
extern const f32 lbl_80563EBC;  // 1.75
extern const f32 lbl_80563EC4;  // -20.0
extern const f32 lbl_80563EC8;  // 1.6
extern const f32 lbl_80563ECC;  // -0.15
extern const f32 lbl_80563ED4;  // 4.3
extern const f32 lbl_80563EDC;  // 2.2
extern const f32 lbl_80563EE0;  // 9.15
extern const f32 lbl_80563EE4;  // 0.97
extern const f32 lbl_80563EE8;  // 100.0
extern f32 lbl_8055E3C8;        // race time

// Extra eye distance by mode (wider on a wide screen).
static inline f32 BackOffset(int mode, int wide) {
    switch (mode) {
    case 8:
        return wide ? lbl_80563EB8 : lbl_80563EBC;
    default:
        return wide ? lbl_80563EC0 : lbl_80563E78;
    }
}

// Places a follow camera (mode, previous mode): resets the trailing frame on a mode change or in the air,
// blends the target height and the eye distance between the two modes, side-steps the eye when the car
// moves sideways, keeps it above the ground and aims it.
void fn_8026F420(int mode, int prev, u8* p) {
    ChaseCam* c = (ChaseCam*)p;
    int wide = fn_80178CA4() > 1;
    f32 off = BackOffset(mode, wide);
    int seven = mode == 7;
    f32 dist = F32(c->car, 0x5B4) + off;
    f32 lift = F32(c->car, 0x5B8);
    int air = F32(c->car, 0x2688) > lbl_80563E78;
    if (c->reset || air) {
        c->blend = lbl_80563E78;
        if (!fn_8026EA14(prev) && mode != 23) {
            c->lastMode = 4;
            c->pitchT = lbl_80563E78;
        } else {
            c->lastMode = prev;
        }
        if ((c->lastMode != 23 && mode != 23) || c->hold) {
            c->pitchT = lbl_80563E78;
        }
        if (!fn_8026EA14(prev) || c->hold || air) {
            c->tf.pos = c->pos;
            c->tf.axes = c->axes;
            c->tf.pos.v[0] = c->tf.pos.v[0] + c->axes.fwd.x * lbl_80563EC4;
            c->tf.pos.v[1] = c->tf.pos.v[1] + c->axes.fwd.y * lbl_80563EC4;
            c->tf.pos.v[2] = c->tf.pos.v[2] + c->axes.fwd.z * lbl_80563EC4;
            if (!seven) {
                c->tf.pos.v[1] = c->tf.pos.v[1] + lbl_80563E84;
            } else {
                c->tf.pos.v[1] = c->tf.pos.v[1] - lbl_80563E84;
            }
            c->pitchT = lbl_80563E78;
        }
    }
    f32 t = lbl_80563EC8 * c->blend;
    t = lbl_80563E30 < t ? lbl_80563E30 : t;
    t = t > lbl_80563E78 ? t : lbl_80563E78;
    f32 ease = Ease(t);
    if (mode == 23) {
        if (!fn_801769F8()) {
            c->pitchT = c->pitchT + lbl_80563E88;
        }
        ease = lbl_80563E30;
    }
    CsVec tgt;
    CsVec d;
    tgt.x = c->pos.v[0];
    tgt.y = c->pos.v[1];
    tgt.z = c->pos.v[2];
    c->fov = fn_8026ECE4(p, &dist);
    f32 a = fn_8026EA44(mode);
    f32 b = fn_8026EA44(c->lastMode);
    tgt.y = tgt.y + F32(c->car, 0x5E4) * (lift + LerpF(ease, b, a));
    c->view = c->tf;
    CsVec up1(lbl_80563E78, lbl_80563E30, lbl_80563E78);
    fn_8026E870(c->view.pos.v, &tgt.x, p, &up1.x);
    d.Set(lbl_80563E78, lbl_80563E78, lbl_80563E78);
    int still = 0;
    if (PSVECDotProduct(c->move, c->move) < lbl_80563E30) {
        still = 1;
        d.x = c->move[0];
        d.y = c->move[1];
        d.z = c->move[2];
    } else {
        f32 inv = lbl_80563E30 / PSVECMag(c->move);
        d.x = c->move[0] * inv;
        d.y = c->move[1] * inv;
        d.z = c->move[2] * inv;
    }
    if (!fn_801769F8()) {
        if (still || PSVECDotProduct(&d.x, &c->view.axes.fwd.x) >= lbl_80563E78) {
            f32 k = lbl_80563ECC * PSVECDotProduct(&d.x, &c->view.axes.right.x);
            c->view.pos.v[0] = c->view.pos.v[0] + c->view.axes.right.x * k;
            c->view.pos.v[1] = c->view.pos.v[1] + c->view.axes.right.y * k;
            c->view.pos.v[2] = c->view.pos.v[2] + c->view.axes.right.z * k;
        } else {
            f32 k = lbl_80563ECC *
                    (PSVECDotProduct(&d.x, &c->view.axes.right.x) > lbl_80563E78 ? lbl_80563E30 : lbl_80563E6C);
            c->view.pos.v[0] = c->view.pos.v[0] + c->view.axes.right.x * k;
            c->view.pos.v[1] = c->view.pos.v[1] + c->view.axes.right.y * k;
            c->view.pos.v[2] = c->view.pos.v[2] + c->view.axes.right.z * k;
        }
    }
    CsVec up2(lbl_80563E78, lbl_80563E30, lbl_80563E78);
    fn_8026E870(c->view.pos.v, &tgt.x, p, &up2.x);
    f32 k;
    f32 sf = c->speedFrac;
    switch (mode) {
    case 8:
        k = lbl_80563E30 - lbl_80563E68 * sf;
        break;
    default:
        k = lbl_80563E30 - lbl_80563ED0 * sf;
        break;
    }
    f32 scale = F32(c->car, 0x5E4);
    f32 pa = fn_8026EA8C(mode, c->pitchT);
    f32 pb = fn_8026EA8C(c->lastMode, c->pitchT);
    f32 back = LerpF(ease, pb, pa) * (k * scale) - dist;
    c->view.pos.v[0] = tgt.x + c->view.axes.fwd.x * back;
    c->view.pos.v[1] = tgt.y + c->view.axes.fwd.y * back;
    c->view.pos.v[2] = tgt.z + c->view.axes.fwd.z * back;
    c->tf = c->view;
    c->view.pos.v[1] = c->view.pos.v[1] - lbl_80563EA8;
    if (seven || U8(c->car, 0x260C)) {
        c->view.pos.v[0] = c->pos.v[0] * lbl_80563E58 - c->tf.pos.v[0];
        c->view.pos.v[1] = c->pos.v[1] * lbl_80563E58 - c->tf.pos.v[1];
        c->view.pos.v[2] = c->pos.v[2] * lbl_80563E58 - c->tf.pos.v[2];
        f32 h = lbl_80563E4C;
        switch (mode) {
        case 4:
            h = lbl_80563ED4;
            break;
        case 5:
            break;
        case 6:
            h = lbl_80563ED8;
            break;
        }
        c->view.pos.v[1] = c->view.pos.v[1] + (h + lbl_80563E58 * lift) * F32(c->car, 0x5E4);
        f32 floor = lbl_80563E30 + c->pos.v[1];
        c->view.pos.v[1] = c->view.pos.v[1] > floor ? c->view.pos.v[1] : floor;
        CsVec up3(lbl_80563E78, lbl_80563E30, lbl_80563E78);
        fn_8026E870(c->view.pos.v, &tgt.x, p, &up3.x);
    }
    f32 gap = seven ? lbl_80563EDC : lbl_80563E50;
    c->time = lbl_8055E3C8;
    f32 y = c->view.pos.v[1];
    if (!fn_801EA03C((const s16*)(c->car + 0x6C), c->view.pos.v)) {
        c->view.pos.v[1] = y;
        c->bump = lbl_80563EE0 - lbl_80563EE4 * (lbl_80563EE0 - c->bump);
    } else {
        c->view.pos.v[1] = c->view.pos.v[1] + gap;
        if (c->view.pos.v[1] < y) {
            c->view.pos.v[1] = y;
        }
        c->bump = c->bump * lbl_80563EE4;
    }
    f32 v = lbl_80563EE0 < c->bump ? lbl_80563EE0 : c->bump;
    v = v > lbl_80563E78 ? v : lbl_80563E78;
    c->bump = v;
    c->view.pos.v[1] = c->view.pos.v[1] + c->bump;
    fn_8026EF48((const s32*)p, c->view.pos.v, &tgt.x);
    CsVec up4(lbl_80563E78, lbl_80563E30, lbl_80563E78);
    fn_8026E870(c->view.pos.v, &tgt.x, p, &up4.x);
    if (!fn_801769F8()) {
        f32 b2 = lbl_80563E88 + c->blend;
        c->blend = b2 < lbl_80563EE8 ? b2 : lbl_80563EE8;
    }
}

f32 fn_8026FCE0(f32 t, f32 a) {
    f32 sq = t * t;
    return (1.0f - a) * t + a * sq;
}

u8* fn_801769CC(void);
s32 fn_8018B06C(u8* car);
void fn_802E30C8(const char* fmt, ...);
extern const f32 lbl_80563EEC;  // 7.0
extern const f32 lbl_80563EF0;  // 2.7
extern const f32 lbl_80563EF4;  // 1.5
extern const f32 lbl_80563EF8;  // 32/9
extern const f32 lbl_80563EFC;  // -16/3
extern const f32 lbl_80563F10;  // -0.95

// The physics camera (mode 4 on a split screen or the TV output): trails the car like the chase camera
// but swings the trailing frame round when the car drives backwards; other modes use fn_8026F420.
void fn_8026FCFC(int mode, int prev, u8* p) {
    ChaseCam* c = (ChaseCam*)p;
    PVec3 target;
    CVec3 d;
    CVec3 q;
    PVec3 f;
    CVec3 side;
    PVec3 fw;
    PVec3 eye;
    CVec3 r;
    int tv = (U32(fn_801769CC(), 0x4) >> 17) & 1;
    if ((fn_80178CA4() <= 4 && !tv) || mode != 4) {
        fn_8026F420(mode, prev, p);
    } else {
        f32 k;
        f32 sf = c->speedFrac;
        switch (mode) {
        case 8:
            k = lbl_80563E30 - lbl_80563E68 * sf;
            break;
        default:
            k = lbl_80563E30 - lbl_80563ED0 * sf;
            break;
        }
        f32 dist = lbl_80563EEC + F32(c->car, 0x5B4) * k;
        f32 lift = lbl_80563EF0 + F32(c->car, 0x5B8);
        c->fov = fn_8026ECE4(p, &dist);
        target = c->pos;
        target.v[1] += lbl_80563EF4;
        if (prev != mode && prev <= 11) {
            fn_802E30C8("physcam: %d != %d (tv = %d)", prev, mode, 11);
            c->tf.pos = c->pos;
            c->tf.axes = c->axes;
            c->tf.pos.v[0] = c->tf.pos.v[0] + c->axes.fwd.x * lbl_80563E44;
            c->tf.pos.v[1] = c->tf.pos.v[1] + c->axes.fwd.y * lbl_80563E44;
            c->tf.pos.v[2] = c->tf.pos.v[2] + c->axes.fwd.z * lbl_80563E44;
        }
        f32 along = PSVECDotProduct(&c->axes.fwd.x, c->vel);
        // The original computes a travel direction here and never uses it (its compares survive).
        f32 dir = lbl_80563E78;
        if (along > lbl_80563EF8 || along < lbl_80563EFC) {
            dir = along > lbl_80563E78 ? lbl_80563E30 : lbl_80563E6C;
        }
        f32 on = fn_8018B06C(c->car) < 0 ? lbl_80563E6C : lbl_80563E30;
        if (U8(c->car, 0x260C)) {
            on *= lbl_80563E6C;
        }
        c->lastOn = c->lastOn + lbl_80563EA8 * (on - c->lastOn);
        int reverse = c->lastOn < lbl_80563E78;
        int back = PSVECDotProduct(&c->tf.axes.fwd.x, c->vel) < lbl_80563E78;
        if (PSVECDotProduct(&c->tf.axes.fwd.x, &c->axes.fwd.x) < lbl_80563E78) {
            back = !back;
        }
        if (!back) {
            d.x = target.v[0] - c->tf.pos.v[0];
            d.y = target.v[1] - c->tf.pos.v[1];
            d.z = target.v[2] - c->tf.pos.v[2];
        } else {
            d.x = target.v[0] - c->tf.pos.v[0];
            d.y = target.v[1] - c->tf.pos.v[1];
            d.z = target.v[2] - c->tf.pos.v[2];
            f32 m = lbl_80563E58 * PSVECDotProduct(&d.x, &c->tf.axes.fwd.x);
            q.x = c->tf.pos.v[0] + c->tf.axes.fwd.x * m;
            q.y = c->tf.pos.v[1] + c->tf.axes.fwd.y * m;
            q.z = c->tf.pos.v[2] + c->tf.axes.fwd.z * m;
            d.x = q.x - target.v[0];
            d.y = q.y - target.v[1];
            d.z = q.z - target.v[2];
        }
        f32 len = PSVECMag(&d.x);
        len = len > lbl_80563EA8 ? len : lbl_80563EA8;
        f32 inv = lbl_80563E30 / len;
        c->tf.axes.fwd.x = d.x * inv;
        c->tf.axes.fwd.y = d.y * inv;
        c->tf.axes.fwd.z = d.z * inv;
        f32 t = (PSVECMag(c->vel) - lbl_80563F00) / lbl_80563F04;
        t = lbl_80563E30 < t ? lbl_80563E30 : t;
        t = t > lbl_80563E78 ? t : lbl_80563E78;
        f32 s = fn_8026FCE0(t, lbl_80563F08);
        s = s + lbl_80563F0C * (c->smooth - s);
        c->smooth = s;
        f = *(PVec3*)&c->axes.fwd;
        if (reverse) {
            f.v[0] *= lbl_80563E6C;
            f.v[1] *= lbl_80563E6C;
            f.v[2] *= lbl_80563E6C;
        }
        f.v[1] *= lbl_80563EA4;
        f32 m = PSVECMag(f.v);
        m = lbl_80563E60 > m ? lbl_80563E60 : m;
        f32 inv2 = lbl_80563E30 / m;
        f.v[0] *= inv2;
        f.v[1] *= inv2;
        f.v[2] *= inv2;
        if (PSVECDotProduct(f.v, &c->tf.axes.fwd.x) > lbl_80563F10) {
            f32 w = s + lbl_80563F14 * (lbl_80563E30 - s);
            r.z = f.v[2] + w * (c->tf.axes.fwd.z - f.v[2]);
            r.y = f.v[1] + w * (c->tf.axes.fwd.y - f.v[1]);
            r.x = f.v[0] + w * (c->tf.axes.fwd.x - f.v[0]);
            c->tf.axes.fwd.x = r.x;
            c->tf.axes.fwd.y = r.y;
            c->tf.axes.fwd.z = r.z;
        } else {
            side.x = c->tf.axes.fwd.z;
            side.y = c->tf.axes.fwd.y;
            side.z = -c->tf.axes.fwd.x;
            f32 kk = lbl_80563EA8 * (PSVECDotProduct(&side.x, f.v) > lbl_80563E78 ? lbl_80563E30 : lbl_80563E6C);
            c->tf.axes.fwd.x = c->tf.axes.fwd.x + side.x * kk;
            c->tf.axes.fwd.y = c->tf.axes.fwd.y + side.y * kk;
            c->tf.axes.fwd.z = c->tf.axes.fwd.z + side.z * kk;
        }
        CVec3& v = c->tf.axes.fwd;
        f32 n = PSVECMag(&v.x);
        n = lbl_80563E60 > n ? lbl_80563E60 : n;
        f32 inv3 = lbl_80563E30 / n;
        v.x = v.x * inv3;
        v.y = v.y * inv3;
        v.z = v.z * inv3;
        c->tf.pos.v[0] = target.v[0] + v.x * lbl_80563F18;
        c->tf.pos.v[1] = target.v[1] + v.y * lbl_80563F18;
        c->tf.pos.v[2] = target.v[2] + v.z * lbl_80563F18;
        fw = *(PVec3*)&c->tf.axes.fwd;
        f32 out = -dist;
        c->view.pos.v[0] = c->pos.v[0] + fw.v[0] * out;
        c->view.pos.v[1] = c->pos.v[1] + fw.v[1] * out + lift;
        c->view.pos.v[2] = c->pos.v[2] + fw.v[2] * out;
        eye = c->view.pos;
        if (fn_801EA03C((const s16*)(c->car + 0x6C), eye.v)) {
            f32 g = lbl_80563E50 + eye.v[1];
            eye.v[1] = c->view.pos.v[1] > g ? c->view.pos.v[1] : g;
        } else {
            eye.v[0] = c->view.pos.v[0];
            eye.v[1] = c->view.pos.v[1];
            eye.v[2] = c->view.pos.v[2];
        }
        CsVec up(lbl_80563E78, lbl_80563E30, lbl_80563E78);
        fn_8026E870(eye.v, target.v, p, &up.x);
    }
}

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

void fn_802704D0(int to, int from, u8* cam) { fn_8026FCFC(to, from, cam); }

void fn_802704D4(int to, u8* cam) { fn_802704D0(to, S32(cam, 0x4), cam); }

f32 fn_802704E0(int mode, f32 t) { return fn_8026EA8C(mode, t); }

// Chase camera: keeps a trailing frame behind the car's frame, eases the view direction toward the
// car's heading, places the eye behind the target above the ground and aims it.
void fn_802704E4(u8* p) {
    ChaseCam* c = (ChaseCam*)p;
    f32 on;
    f32 s;
    f32 lift;
    PVec3 target;
    CVec3 d;
    PVec3 f;
    PVec3 eye;
    CVec3 r;
    int wide = fn_80178CA4() > 1;
    f32 dist = AddMul(wide ? lbl_80563ED8 : lbl_80563EC0, lbl_80563E30 - MulF(lbl_80563ED0, c->speedFrac), F32(c->car, 0x5B4));
    lift = F32(c->car, 0x5B8) + lbl_80563F20;
    if (c->mode != 9) {
        c->tf.pos = c->pos;
        c->tf.axes = c->axes;
        c->tf.pos.v[0] = c->tf.pos.v[0] + c->axes.fwd.x * lbl_80563E44;
        c->tf.pos.v[1] = c->tf.pos.v[1] + c->axes.fwd.y * lbl_80563E44;
        c->tf.pos.v[2] = c->tf.pos.v[2] + c->axes.fwd.z * lbl_80563E44;
    }
    on = U8(c->car, 0x260C) ? lbl_80563E30 : lbl_80563E78;
    c->fov = fn_8026ECE4(p, &dist);
    target = c->pos;
    target.v[1] += MulF(wide ? lbl_80563F24 : lbl_80563E58, F32(c->car, 0x5E4));
    if ((f32)__fabs(on - c->lastOn) <= lbl_80558DC4[0]) {
        int below = F32(c->car, 0x25D8) < lbl_80563F28;
        int moving = !((f32)__fabs(on) <= lbl_80558DC4[0]);
        d.x = target.v[0] - c->tf.pos.v[0];
        d.y = target.v[1] - c->tf.pos.v[1];
        d.z = target.v[2] - c->tf.pos.v[2];
        f32 len = PSVECMag(&d.x);
        len = len > lbl_80563EA8 ? len : lbl_80563EA8;
        f32 inv = lbl_80563E30 / len;
        c->tf.axes.fwd.x = d.x * inv;
        c->tf.axes.fwd.y = d.y * inv;
        c->tf.axes.fwd.z = d.z * inv;
        f32 t = (PSVECMag(c->vel) - lbl_80563F00) / lbl_80563F04;
        t = lbl_80563E30 < t ? lbl_80563E30 : t;
        t = t > lbl_80563E78 ? t : lbl_80563E78;
        f32 k;
        if ((moving && !below) || (below && !moving)) {
            k = t * lbl_80563EA8;
        } else {
            k = fn_8026FCE0(t, lbl_80563F08);
        }
        s = k + lbl_80563F0C * (c->smooth - k);
        c->smooth = s;
        f = *(PVec3*)&c->axes.fwd;
        if (moving) {
            f.v[0] *= lbl_80563E6C;
            f.v[1] *= lbl_80563E6C;
            f.v[2] *= lbl_80563E6C;
        }
        f.v[1] *= lbl_80563EA4;
        f32 m = PSVECMag(f.v);
        m = lbl_80563E60 > m ? lbl_80563E60 : m;
        f32 inv2 = lbl_80563E30 / m;
        f.v[0] *= inv2;
        f.v[1] *= inv2;
        f.v[2] *= inv2;
        f32 w = s + lbl_80563F14 * (lbl_80563E30 - s);
        r.z = f.v[2] + w * (c->tf.axes.fwd.z - f.v[2]);
        r.y = f.v[1] + w * (c->tf.axes.fwd.y - f.v[1]);
        r.x = f.v[0] + w * (c->tf.axes.fwd.x - f.v[0]);
        c->tf.axes.fwd.x = r.x;
        c->tf.axes.fwd.y = r.y;
        c->tf.axes.fwd.z = r.z;
    } else {
        c->tf.axes.fwd.x *= lbl_80563E6C;
        c->tf.axes.fwd.y *= lbl_80563E6C;
        c->tf.axes.fwd.z *= lbl_80563E6C;
    }
    f32 back = F32(c->car, 0x5E4) * lbl_80563F18;
    c->tf.pos.v[0] = target.v[0] + c->tf.axes.fwd.x * back;
    c->tf.pos.v[1] = target.v[1] + c->tf.axes.fwd.y * back;
    c->tf.pos.v[2] = target.v[2] + c->tf.axes.fwd.z * back;
    f32 out = F32(c->car, 0x5E4) * -dist;
    c->view.pos.v[0] = c->pos.v[0] + c->tf.axes.fwd.x * out;
    c->view.pos.v[1] = c->pos.v[1] + c->tf.axes.fwd.y * out;
    c->view.pos.v[2] = c->pos.v[2] + c->tf.axes.fwd.z * out;
    c->view.pos.v[1] = c->view.pos.v[1] + lift * F32(c->car, 0x5E4);
    eye = c->view.pos;
    if (fn_801EA03C((const s16*)(c->car + 0x6C), eye.v)) {
        f32 g = (lbl_80563E50 + eye.v[1]) * F32(c->car, 0x5E4);
        eye.v[1] = c->view.pos.v[1] > g ? c->view.pos.v[1] : g;
    } else {
        eye.v[0] = c->view.pos.v[0];
        eye.v[1] = c->view.pos.v[1];
        eye.v[2] = c->view.pos.v[2];
    }
    fn_8026EF48((const s32*)p, eye.v, target.v);
    CsVec up(lbl_80563E78, lbl_80563E30, lbl_80563E78);
    fn_8026E870(eye.v, target.v, p, &up.x);
    c->lastOn = on;
}

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
