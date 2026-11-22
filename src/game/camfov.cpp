// Camera field of view (0x8026E870-0x80270AE8): per-mode FOV and distance tables, the widescreen
// FOV correction and the camera blend helpers ("physcam: %d != %d (tv = %d)"). Written against raw
// field offsets; the large routines and the initialiser are still stubs. Names are guesses apart from
// the strings.
#include "game/common.h"
#include "game/mathutil.h"
#include "game/raceopts.h"
#include <revolution/MTX/vec.h>
#include "game/camframe.h"
#include "game/camrect.h"
#include "game/camrec.h"
#include "game/plainvec.h"
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

extern CamRect lbl_8049A218[4];

extern "C" {


// Places the camera at `pos` looking at `target` with `up` as the up hint; nothing changes the frame
// when the target is too close or straight above/below.
void SetupCameraLookAt(const f32* pos, const f32* target, u8* cam, const f32* up) {
    CamRec* cr = (CamRec*)cam;
    cr->pose.pos.v[0] = pos[0];
    cr->pose.pos.v[1] = pos[1];
    cr->pose.pos.v[2] = pos[2];
    CsVec d;
    d.x = target[0] - cr->pose.pos.v[0];
    d.y = target[1] - cr->pose.pos.v[1];
    d.z = target[2] - cr->pose.pos.v[2];
    if (PSVECDotProduct((const Vec*)&d.x, (const Vec*)&d.x) < 1.0f) {
        return;
    }
    if (AbsF(d.x) < 0.01f && AbsF(d.z) < 0.01f) {
        return;
    }
    cr->pose.rot.m[6] = d.x;
    cr->pose.rot.m[7] = d.y;
    cr->pose.rot.m[8] = d.z;
    PSVECCrossProduct((const Vec*)up, (const Vec*)((f32*)&cr->pose.rot.m[6]), (Vec*)((f32*)&cr->pose));
    PSVECCrossProduct((const Vec*)((f32*)&cr->pose.rot.m[6]), (const Vec*)((f32*)&cr->pose), (Vec*)((f32*)&cr->pose.rot.m[3]));
    Orthonormalize(&((CamRec*)cam)->pose);
}

// Mode set as a bit mask over modes 4..23 (the original tests a mask, not a switch).
#define CAM_MODE_BIT(m) (1 << ((m) - 4))

int IsCameraModeValid(int mode) {
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
f32 CalculateCameraPitchOffset(int mode, f32 t) {
    // The target's jump table covers modes 4-23, so mode 4 and at least one more mode in 8-22 had their own
    // labels on the default value (MWCC builds a table from six labels on); which one besides 4 is unknown.
    switch (mode) {
    case 4:
    case 8:
    default:
        return -5.5f;
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
}

// The constants this unit reads are its own literals; the forms below give retail's .sdata2 creation order and
// operand order where a plain literal does not. Helpers unproven (names and existence).
// TanHalf / AtanTwice: retail creates 0.5 before 0.4 and 2.0.
static inline f32 TanHalf(f32 a) { return (f32)tan(a / 2.0f); }
static inline f32 AtanTwice(f32 t) { return 2.0f * (f32)atan(t); }
f32 fn_8026EAEC(f32 fov) {
    if (fn_80178CA4() > 1) {
        fov = AtanTwice(0.4f * TanHalf(fov));
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

static inline int Boosting(u8* car) { return F32(car, 0x258C) > 0.001f; }
// MulF: a product whose constant arrives as an inline parameter (a literal written in a multiply is always the
// first operand; retail has the variable first at these sites).
// AddScaled / Scaled: o = a + d * s and o = a * s; retail loads the scale once for the three components.
static inline void AddScaled(f32* o, const f32* a, const f32* d, f32 s) {
    o[0] = a[0] + d[0] * s;
    o[1] = a[1] + d[1] * s;
    o[2] = a[2] + d[2] * s;
}
static inline void Scaled(f32* o, const f32* a, f32 s) {
    o[0] = a[0] * s;
    o[1] = a[1] * s;
    o[2] = a[2] * s;
}
static inline int Above(f32 v, f32 k) { return v > k; }

// The view angle for the camera: blended from the last one (0x194) toward the speed-based angle, or
// toward the boost angle while boosting (not while an AI driver is in mode 1).
f32 CalculateCameraFieldOfView(const u8* p) {
    const CamRec* cr = (const CamRec*)p;
    f32 fov;
    f32 r = fn_8026EAEC(!Boosting(cr->car) ? lbl_8055F534 : lbl_8055F530);
    f32 lo = lbl_8055F528;
    fov = fn_8026EAEC(LerpT(cr->f13C, lo, lbl_8055F52C));
    u8* car = cr->car;
    int boost = 0;
    int on = Boosting(car) || car[0x259C];
    if (on && Above(F32(car, 0x25E0), 0.5f)) {
        boost = 1;
    }
    if (cr->w4 != cr->mode) {
        r = boost ? r : fov;
    } else if (fn_801769F8() || IsLoadingActive() || S32(cr->car, 0x2620) < 1) {
        r = cr->f194;
    } else if (boost && *(s32*)(fn_801B2ED0(S32(cr->car, 0x1EC))->v10() + 0x24) != 1) {
        r = cr->f194 + 0.05f * (r - cr->f194);
    } else {
        r = cr->f194 + 0.02f * (fov - cr->f194);
    }
    return r;
}

double cos(double x);

// Returns the view angle; scales *scale so that wider views keep the reference framing.
f32 CalculateViewAngleScale(u8* p, f32* scale) {
    f32 ref;
    f32 fov = CalculateCameraFieldOfView(p);
    if (fov > lbl_8055F52C) {
        ref = cos(Mul(lbl_8055F52C, 0.25f));
        *scale *= (f32)cos(Mul(fov, 0.25f)) / ref;
    }
    return fov;
}


// Cubic ease of a 0..1 screen fraction (mirrored below the midpoint).
static inline f32 Ease(f32 x) {
    if (x < -1.0f) {
        return -1.0f;
    }
    if (x > 1.0f) {
        return 1.0f;
    }
    if (x > 0.0f) {
        return x * (-2.0f * x * x) + 3.0f * x * x;
    }
    return x * (-2.0f * x * x) - 3.0f * x * x;
}

// Adjusts the camera position for the player's split-screen viewport: lifts it by `lift` scaled with
// the viewport's height, then pulls it toward the target by its vertical placement.

// Unit01: v mapped from lo..hi and clamped to 0..1 (retail keeps the subtraction of a zero lo).
static inline f32 Unit01(f32 v, f32 lo, f32 hi) {
    f32 t = (v - lo) / (hi - lo);
    t = 1.0f < t ? 1.0f : t;
    t = t > 0.0f ? t : 0.0f;
    return t;
}
void fn_8026ED64(s32* cam, f32 lift, f32* pos, f32* target) {
    CamRect* r = lbl_8049A218;  // the base stays in r3 and the scaled index is added to it (retail); &lbl[*cam] swaps the two
    r += *cam;
    f32 e = Ease(r->v[3]);
    f32 t = Unit01(r->v[1], 0.0f, 20.0f);
    f32 k = LerpT(t, 0.0f, 1.0f);
    pos[1] = pos[1] + lift * (-4.0f * (k * e));
    CsVec d;
    d.x = target[0] - pos[0];
    d.y = target[1] - pos[1];
    d.z = target[2] - pos[2];
    f32 s = -1.0f - 0.5f * Ease(r->v[3]);
    pos[0] = target[0] + d.x * s;
    pos[1] = target[1] + d.y * s;
    pos[2] = target[2] + d.z * s;
}

f32 fn_801E9FDC(const s16* hint, f32 x, f32 z);
int fn_801784DC(f32* pos, f32* h, int a, int b, int (*filter)(int));
f32 fn_8026A520(int i);
int fn_8026F408(int n);

// The ground height at (x, z), raised to any obstacle top found there.
/* Helper name unproven: the result goes out through a reference (retail keeps it in the caller's variable), the point
   is built with the by-value CsVec::Set. */
static inline void GroundAt(f32& h, const s16* hint, f32 x, f32 y, f32 z) {
    h = fn_801E9FDC(hint, x, z);
    CsVec q;
    q.Set(x, y, z);
    f32 top;
    if (fn_801784DC(&q.x, &top, 0, 0, fn_8026F408)) {
        h = h > top ? h : top;
    }
}

/* Helper name unproven: the ground height averaged over four corners around a point that arrives BY VALUE. The caller
   passes the components of its const local: x (offset 0 of an unreferenced local) is pasted by the inliner as a
   converted read, which is generated before the 10.0 constant in the two sums; y and z get parameter objects.
   This replaces the (f32)c.x casts and the y/z locals (FACTS 2026-11-17 12:06). */
static inline f32 GroundAvg(const s16* hint, f32 x, f32 y, f32 z) {
    f32 h0;
    f32 h1;
    f32 h2;
    f32 h3;
    GroundAt(h0, hint, x + 10.0f, y, z + 10.0f);
    GroundAt(h1, hint, x - 10.0f, y, z + 10.0f);
    GroundAt(h2, hint, x - 10.0f, y, z - 10.0f);
    GroundAt(h3, hint, x + 10.0f, y, z - 10.0f);
    return 0.25f * (h0 + (h1 + (h2 + h3)));
}

/* Helper name unproven */
static inline void EaseGroundHeight(f32 t, bool onGround, f32 h, CamRect* r) {
    f32 lo;
    f32 hi;
    f32 up;
    f32 down;
    up = 0.016683351f * LerpT(t, 400.0f, 20.0f);
    down = onGround ? 6.6733403f : 0.33366704f;
    if (h < r->v[1]) {
        lo = r->v[1] - down;
        h = h > lo ? h : lo;
        r->v[1] = h;
    } else {
        hi = r->v[1] + up;
        h = h < hi ? h : hi;
        r->v[1] = h;
    }
}

// Per-frame camera height tracking: averages the ground around the car into the player's rect
// (lbl_8049A218), eases the rect's lift/zoom terms, then lifts pos/target and applies fn_8026ED64.
void UpdateCameraHeightTracking(s32* cam, f32* pos, f32* target) {
    const CamRec* cr = (const CamRec*)cam;
    f32 avg;
    f32 v;
    f32 h;
    u8* car;
    bool onGround;
    const s16* hint;
    f32 lift;
    f32 zoom;
    f32 k;
    f32 t;
    f32 m;
    f32 top;
    f32 rv;
    CamRect* r;
    car = cr->car;
    r = &lbl_8049A218[*cam];
    if (!fn_801769F8()) {
        hint = (const s16*)(car + 0x6C);
        onGround = S32(car, 0x2620) > 0;
        const CVec3 c = *(CVec3*)(car + 0x60);
        avg = GroundAvg(hint, c.x, c.y, c.z);
        if (onGround) {
            r->v[0] = avg - c.y;
            r->v[2] = 0.0f;
        } else {
            r->v[2] += 0.016683351f;
        }
        h = r->v[0] + (c.y - avg);
        rv = r->v[2];
        t = Unit01(rv, 0.0f, 1.0f);
        EaseGroundHeight(t, onGround, h, r);
        zoom = fn_8026A520(S32(car, 0x1EC)) > 0.0f;
        if (zoom) {
            v = 0.04170838f + r->v[3];
            r->v[3] = v < 1.0f ? v : 1.0f;
        } else {
            v = r->v[3] - 0.008341676f;
            r->v[3] = v > 0.0f ? v : 0.0f;
        }
    }
    lift = MaxF(0.0f, r->v[1]);
    f32 span = 0.2f;  /* Unused local, unproven: retail creates 0.2 in the pool here with no code that reads it. */
    top = lbl_8049A208[3];
    m = 0.1f + lbl_8049A208[2];
    top = top > m ? top : m;
    k = (lift - lbl_8049A208[2]) / (top - lbl_8049A208[2]);
    f32 kMax = 1.0f;
    k = kMax < k ? kMax : k;
    k = k > 0.0f ? k : 0.0f;
    k = LerpT(k, 0.0f, 1.0f);
    pos[1] = pos[1] + lbl_8049A208[0] * k;
    target[1] = target[1] + lbl_8049A208[1] * k;
    fn_8026ED64(cam, MinF(1.0f, 5.0f * k), pos, target);
}

int fn_8026F408(int n) { return n <= 8; }

extern f32 __float_epsilon[];  // FLT_EPSILON
int fn_801EA03C(const s16* hint, f32* pos);

#include "game/pvec3a.h"

// A camera frame's rows (right, up, view axis) without its position.
struct CamAxes {
    CVec3 right, up, fwd;
};


// A camera frame: its rows and its position.
struct CamFovFrame {
    CamAxes axes;  // offset 0x0
    PVec3A pos;     // offset 0x24
};

// The chase/follow camera (fields used by UpdateChaseCameraMode and UpdateChaseCameraView; the rest is camstate's).
struct ChaseCam {
    u8 pad0[0x4];
    s32 mode;           // offset 0x4
    f32 pitchT;         // offset 0x8
    f32 blend;          // offset 0xC
    u8 pad10[0x8];
    s32 lastMode;       // offset 0x18
    u8 pad1C[0x3C];
    CamFovFrame view;      // offset 0x58: rows and eye (0x7C)
    u8 pad88[0x34];
    f32 fov;            // offset 0xBC
    CamAxes axes;       // offset 0xC0: the car's frame
    PVec3A pos;          // offset 0xE4
    CamFovFrame tf;        // offset 0xF0: the trailing frame
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

extern f32 lbl_8055E3C8;        // race time

// Extra eye distance by mode (wider on a wide screen).
static inline f32 BackOffset(int mode, int wide) {
    switch (mode) {
    case 8:
        return wide ? 6.0f : 1.75f;
    default:
        return wide ? 4.0f : 0.0f;
    }
}

// Places a follow camera (mode, previous mode): resets the trailing frame on a mode change or in the air,
// blends the target height and the eye distance between the two modes, side-steps the eye when the car
// moves sideways, keeps it above the ground and aims it.
static inline f32 CarHeight(u8* car) { return F32(car, 0x5B8); }  /* Helper name unproven */
static inline f32 CarScale(u8* car) { return F32(car, 0x5E4); }  /* Helper name unproven */
void UpdateChaseCameraMode(int mode, int prev, u8* p) {
    ChaseCam* c = (ChaseCam*)p;
    int wide = fn_80178CA4() > 1;
    f32 off = BackOffset(mode, wide);
    int seven = mode == 7;
    f32 carDistance = F32(c->car, 0x5B4);
    f32 dist = carDistance + off;
    f32 lift = F32(c->car, 0x5B8);
    int air = F32(c->car, 0x2688) > 0.0f;
    if (c->reset || air) {
        c->blend = 0.0f;
        if (!IsCameraModeValid(prev) && mode != 23) {
            c->lastMode = 4;
            c->pitchT = 0.0f;
        } else {
            c->lastMode = prev;
        }
        if ((c->lastMode != 23 && mode != 23) || c->hold) {
            c->pitchT = 0.0f;
        }
        if (!IsCameraModeValid(prev) || c->hold || air) {
            c->tf.pos = c->pos;
            c->tf.axes = c->axes;
            AddScaled(c->tf.pos.v, c->tf.pos.v, &c->axes.fwd.x, -20.0f);
            if (!seven) {
                c->tf.pos.v[1] += 10.0f;
            } else {
                c->tf.pos.v[1] = c->tf.pos.v[1] - 10.0f;
            }
            c->pitchT = 0.0f;
        }
    }
    f32 t = 1.6f * c->blend;
    t = 1.0f < t ? 1.0f : t;
    t = t > 0.0f ? t : 0.0f;
    f32 ease = Ease(t);
    if (mode == 23) {
        if (!fn_801769F8()) {
            c->pitchT += 0.016683351f;
        }
        ease = 1.0f;
    }
    CsVec tgt;
    CsVec d;
    tgt.x = c->pos.v[0];
    tgt.y = c->pos.v[1];
    tgt.z = c->pos.v[2];
    c->fov = CalculateViewAngleScale(p, &dist);
    f32 a = fn_8026EA44(mode);
    f32 b = fn_8026EA44(c->lastMode);
    tgt.y = tgt.y + CarScale(c->car) * (lift + LerpT(ease, b, a));
    c->view = c->tf;
    CsVec up1(0.0f, 1.0f, 0.0f);
    SetupCameraLookAt(c->view.pos.v, &tgt.x, p, &up1.x);
    d.Set(0.0f, 0.0f, 0.0f);
    int still = 0;
    if (PSVECDotProduct((const Vec*)c->move, (const Vec*)c->move) < 1.0f) {
        still = 1;
        d.x = c->move[0];
        d.y = c->move[1];
        d.z = c->move[2];
    } else {
        f32 inv = 1.0f / PSVECMag((const Vec*)c->move);
        d.x = c->move[0] * inv;
        d.y = c->move[1] * inv;
        d.z = c->move[2] * inv;
    }
    if (!fn_801769F8()) {
        if (still || PSVECDotProduct((const Vec*)&d.x, (const Vec*)&c->view.axes.fwd.x) >= 0.0f) {
            f32 k = -0.15015016f * PSVECDotProduct((const Vec*)&d.x, (const Vec*)&c->view.axes.right.x);
            c->view.pos.v[0] = c->view.pos.v[0] + c->view.axes.right.x * k;
            c->view.pos.v[1] = c->view.pos.v[1] + c->view.axes.right.y * k;
            c->view.pos.v[2] = c->view.pos.v[2] + c->view.axes.right.z * k;
        } else {
            f32 k = -0.15015016f *
                    (PSVECDotProduct((const Vec*)&d.x, (const Vec*)&c->view.axes.right.x) > 0.0f ? 1.0f : -1.0f);
            c->view.pos.v[0] = c->view.pos.v[0] + c->view.axes.right.x * k;
            c->view.pos.v[1] = c->view.pos.v[1] + c->view.axes.right.y * k;
            c->view.pos.v[2] = c->view.pos.v[2] + c->view.axes.right.z * k;
        }
    }
    CsVec up2(0.0f, 1.0f, 0.0f);
    SetupCameraLookAt(c->view.pos.v, &tgt.x, p, &up2.x);
    f32 k;
    f32 sf = c->speedFrac;
    switch (mode) {
    case 8:
        k = 1.0f - Mul(0.25f, sf);
        break;
    default:
        k = 1.0f - 0.35f * sf;
        break;
    }
    f32 y;
    f32 scale = F32(c->car, 0x5E4);
    f32 pa = CalculateCameraPitchOffset(mode, c->pitchT);
    f32 pb = CalculateCameraPitchOffset(c->lastMode, c->pitchT);
    f32 pitch = LerpT(ease, pb, pa);
    f32 back = pitch * (k * scale) - dist;
    c->view.pos.v[0] = tgt.x + c->view.axes.fwd.x * back;
    c->view.pos.v[1] = tgt.y + c->view.axes.fwd.y * back;
    c->view.pos.v[2] = tgt.z + c->view.axes.fwd.z * back;
    c->tf = c->view;
    c->view.pos.v[1] = c->view.pos.v[1] - 0.1f;
    if (seven || U8(c->car, 0x260C)) {
        Scaled(c->view.pos.v, c->pos.v, 2.0f);
        c->view.pos.v[0] = c->view.pos.v[0] - c->tf.pos.v[0];
        c->view.pos.v[1] = c->view.pos.v[1] - c->tf.pos.v[1];
        c->view.pos.v[2] = c->view.pos.v[2] - c->tf.pos.v[2];
        f32 h = 5.0f;
        switch (mode) {
        case 4:
            h = 4.3f;
            break;
        case 5:
            break;
        case 6:
            h = 8.0f;
            break;
        }
        c->view.pos.v[1] = c->view.pos.v[1] + (h + Mul(2.0f, lift)) * CarScale(c->car);
        c->view.pos.v[1] = c->view.pos.v[1] > (1.0f + c->pos.v[1]) ? c->view.pos.v[1] : (1.0f + c->pos.v[1]);
        CsVec up3(0.0f, 1.0f, 0.0f);
        SetupCameraLookAt(c->view.pos.v, &tgt.x, p, &up3.x);
    }
    f32 gap;
    if (seven) {
        gap = 2.2f;
    } else {
        gap = 0.5f;
    }
    c->time = lbl_8055E3C8;
    y = c->view.pos.v[1];
    if (!fn_801EA03C((const s16*)(c->car + 0x6C), c->view.pos.v)) {
        c->view.pos.v[1] = y;
        c->bump = 9.150001f - 0.97f * (9.150001f - c->bump);
    } else {
        c->view.pos.v[1] = c->view.pos.v[1] + gap;
        if (c->view.pos.v[1] < y) {
            c->view.pos.v[1] = y;
        }
        c->bump = Mul(c->bump, 0.97f);
    }
    f32 bumpv = c->bump;
    f32 v = 9.150001f < bumpv ? 9.150001f : bumpv;
    v = v > 0.0f ? v : 0.0f;
    c->bump = v;
    c->view.pos.v[1] = c->view.pos.v[1] + c->bump;
    UpdateCameraHeightTracking((s32*)p, c->view.pos.v, &tgt.x);
    CsVec up4(0.0f, 1.0f, 0.0f);
    SetupCameraLookAt(c->view.pos.v, &tgt.x, p, &up4.x);
    if (!fn_801769F8()) {
        f32 b2 = 0.016683351f + c->blend;
        c->blend = b2 < 100.0f ? b2 : 100.0f;
    }
}

f32 fn_8026FCE0(f32 t, f32 a) {
    f32 sq = t * t;
    return (1.0f - a) * t + a * sq;
}

s32 fn_8018B06C(u8* car);
void fn_802E30C8(const char* fmt, ...);

// The physics camera (mode 4 on a split screen or the TV output): trails the car like the chase camera
// but swings the trailing frame round when the car drives backwards; other modes use UpdateChaseCameraMode.

void UpdateChaseCameraFOV(int mode, int prev, u8* p) {
    ChaseCam* c = (ChaseCam*)p;
    PVec3A target;
    CVec3 d;
    CVec3 q;
    PVec3A f;
    CVec3 side;
    PVec3A fw;
    PVec3A eye;
    CVec3 r;
    int tv = (fn_801769CC()->type >> 17) & 1;
    if ((fn_80178CA4() <= 4 && !tv) || mode != 4) {
        UpdateChaseCameraMode(mode, prev, p);
    } else {
        f32 k = c->speedFrac;
        switch (mode) {
        case 8:
            k = 1.0f - Mul(0.25f, k);
            break;
        default:
            k = 1.0f - 0.35f * k;
            break;
        }
        f32 radius = F32(c->car, 0x5B4);
        f32 dist = 7.0f + radius * k;
        f32 lift = CarHeight(c->car) + 2.7f;
        c->fov = CalculateViewAngleScale(p, &dist);
        target = c->pos;
        target.v[1] += 1.5f;
        if (prev != mode && prev <= 11) {
            fn_802E30C8("physcam: %d != %d (tv = %d)", prev, mode, 11);
            c->tf.pos = c->pos;
            c->tf.axes = c->axes;
            AddScaled(c->tf.pos.v, c->tf.pos.v, &c->axes.fwd.x, -10.0f);
        }
        f32 along = PSVECDotProduct((const Vec*)&c->axes.fwd.x, (const Vec*)c->vel);
        // The original computes a travel direction here and never uses it (its compares survive).
        f32 dir = 0.0f;
        if (along > 3.5555556f || along < -5.3333335f) {
            dir = along > 0.0f ? 1.0f : -1.0f;
        }
        f32 on = fn_8018B06C(c->car) < 0 ? -1.0f : 1.0f;
        if (U8(c->car, 0x260C)) {
            on *= -1.0f;
        }
        c->lastOn = c->lastOn + 0.1f * (on - c->lastOn);
        CVec3* forward;
        int reverse = c->lastOn < 0.0f;
        int back = PSVECDotProduct((const Vec*)&c->tf.axes.fwd.x, (const Vec*)c->vel) < 0.0f;
        if (PSVECDotProduct((const Vec*)&c->tf.axes.fwd.x, (const Vec*)&c->axes.fwd.x) < 0.0f) {
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
            f32 m = Mul(2.0f, PSVECDotProduct((const Vec*)&d.x, (const Vec*)&c->tf.axes.fwd.x));
            q.x = c->tf.pos.v[0] + c->tf.axes.fwd.x * m;
            q.y = c->tf.pos.v[1] + c->tf.axes.fwd.y * m;
            q.z = c->tf.pos.v[2] + c->tf.axes.fwd.z * m;
            d.x = q.x - target.v[0];
            d.y = q.y - target.v[1];
            d.z = q.z - target.v[2];
        }
        f32 len = PSVECMag((const Vec*)&d.x);
        len = len > 0.1f ? len : 0.1f;
        f32 inv = 1.0f / len;
        c->tf.axes.fwd.x = d.x * inv;
        c->tf.axes.fwd.y = d.y * inv;
        c->tf.axes.fwd.z = d.z * inv;
        f32 t = (PSVECMag((const Vec*)c->vel) - 1.3333334f) / 26.666666f;
        t = 1.0f < t ? 1.0f : t;
        t = t > 0.0f ? t : 0.0f;
        f32 s = fn_8026FCE0(t, -0.5f);
        s = s + 0.9f * (c->smooth - s);
        c->smooth = s;
        f = *(PVec3A*)&c->axes.fwd;
        if (reverse) {
            f.v[0] *= -1.0f;
            f.v[1] *= -1.0f;
            f.v[2] *= -1.0f;
        }
        f.v[1] *= 0.2f;
        f32 m = PSVECMag((const Vec*)f.v);
        m = 0.05f > m ? 0.05f : m;
        f32 inv2 = 1.0f / m;
        f.v[0] *= inv2;
        f.v[1] *= inv2;
        f.v[2] *= inv2;
        if (PSVECDotProduct((const Vec*)f.v, (const Vec*)&c->tf.axes.fwd.x) > -0.95f) {
            f32 w = s + 0.7f * (1.0f - s);
            r.Set(LerpT(w, f.v[0], c->tf.axes.fwd.x), LerpT(w, f.v[1], c->tf.axes.fwd.y), LerpT(w, f.v[2], c->tf.axes.fwd.z));
            c->tf.axes.fwd.x = r.x;
            c->tf.axes.fwd.y = r.y;
            c->tf.axes.fwd.z = r.z;
        } else {
            f32 y2 = c->tf.axes.fwd.y;
            f32 sideZ = -c->tf.axes.fwd.x;
            side.x = c->tf.axes.fwd.z;
            side.y = y2;
            side.z = sideZ;
            f32 kk = 0.1f * (PSVECDotProduct((const Vec*)&side.x, (const Vec*)f.v) > 0.0f ? 1.0f : -1.0f);
            c->tf.axes.fwd.x = c->tf.axes.fwd.x + side.x * kk;
            c->tf.axes.fwd.y = c->tf.axes.fwd.y + side.y * kk;
            c->tf.axes.fwd.z = c->tf.axes.fwd.z + side.z * kk;
        }
        forward = &c->tf.axes.fwd;
        f32 n = PSVECMag((const Vec*)&forward->x);
        n = 0.05f > n ? 0.05f : n;
        f32 inv3 = 1.0f / n;
        forward->x = forward->x * inv3;
        forward->y = forward->y * inv3;
        forward->z = forward->z * inv3;
        AddScaled(c->tf.pos.v, target.v, &forward->x, -13.0f);
        fw = *(PVec3A*)&c->tf.axes.fwd;
        f32 out = -dist;
        c->view.pos.v[0] = c->pos.v[0] + fw.v[0] * out;
        c->view.pos.v[1] = c->pos.v[1] + fw.v[1] * out;
        c->view.pos.v[1] += lift;
        f32 viewZ = c->pos.v[2] + fw.v[2] * out;
        c->view.pos.v[2] = viewZ;
        eye = c->view.pos;
        if (fn_801EA03C((const s16*)(c->car + 0x6C), eye.v)) {
            f32 g = 0.5f + eye.v[1];
            eye.v[1] = c->view.pos.v[1] > g ? c->view.pos.v[1] : g;
        } else {
            eye.v[0] = c->view.pos.v[0];
            eye.v[1] = c->view.pos.v[1];
            eye.v[2] = c->view.pos.v[2];
        }
        CsVec up(0.0f, 1.0f, 0.0f);
        SetupCameraLookAt(eye.v, target.v, p, &up.x);
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

void CopyCameraData(int to, int from, u8* cam) { UpdateChaseCameraFOV(to, from, cam); }

void fn_802704D4(int to, u8* cam) { CopyCameraData(to, ((CamRec*)cam)->w4, cam); }

f32 fn_802704E0(int mode, f32 t) { return CalculateCameraPitchOffset(mode, t); }

// Chase camera: keeps a trailing frame behind the car's frame, eases the view direction toward the
// car's heading, places the eye behind the target above the ground and aims it.
void UpdateChaseCameraView(u8* p) {
    ChaseCam* c = (ChaseCam*)p;
    f32 on;
    f32 s;
    f32 lift;
    PVec3A target;
    CVec3 d;
    PVec3A f;
    PVec3A eye;
    CVec3 r;
    int wide = fn_80178CA4() > 1;
    f32 dist = AddScaledF(wide ? 8.0f : 4.0f, 1.0f - Mul(0.35f, c->speedFrac), F32(c->car, 0x5B4));
    lift = CarHeight(c->car) + 2.1f;
    if (c->mode != 9) {
        c->tf.pos = c->pos;
        c->tf.axes = c->axes;
        AddScaled(c->tf.pos.v, c->tf.pos.v, &c->axes.fwd.x, -10.0f);
    }
    on = U8(c->car, 0x260C) ? 1.0f : 0.0f;
    c->fov = CalculateViewAngleScale(p, &dist);
    target = c->pos;
    target.v[1] += Mul(wide ? 2.5f : 2.0f, F32(c->car, 0x5E4));
    if ((f32)__fabs(on - c->lastOn) <= __float_epsilon[0]) {
        f32 altitude = F32(c->car, 0x25D8);
        int below = altitude < -0.1f;
        int moving = !((f32)__fabs(on) <= __float_epsilon[0]);
        d.x = target.v[0] - c->tf.pos.v[0];
        d.y = target.v[1] - c->tf.pos.v[1];
        d.z = target.v[2] - c->tf.pos.v[2];
        f32 len = PSVECMag((const Vec*)&d.x);
        len = len > 0.1f ? len : 0.1f;
        f32 inv = 1.0f / len;
        c->tf.axes.fwd.x = d.x * inv;
        c->tf.axes.fwd.y = d.y * inv;
        c->tf.axes.fwd.z = d.z * inv;
        f32 t = (PSVECMag((const Vec*)c->vel) - 1.3333334f) / 26.666666f;
        t = 1.0f < t ? 1.0f : t;
        t = t > 0.0f ? t : 0.0f;
        f32 k;
        if ((moving && !below) || (below && !moving)) {
            k = Mul(t, 0.1f);
        } else {
            k = fn_8026FCE0(t, -0.5f);
        }
        s = k + 0.9f * (c->smooth - k);
        c->smooth = s;
        f = *(PVec3A*)&c->axes.fwd;
        if (moving) {
            f.v[0] *= -1.0f;
            f.v[1] *= -1.0f;
            f.v[2] *= -1.0f;
        }
        f.v[1] *= 0.2f;
        f32 m = PSVECMag((const Vec*)f.v);
        m = 0.05f > m ? 0.05f : m;
        f32 inv2 = 1.0f / m;
        f.v[0] *= inv2;
        f.v[1] *= inv2;
        f.v[2] *= inv2;
        f32 w = s + 0.7f * (1.0f - s);
        r.Set(LerpT(w, f.v[0], c->tf.axes.fwd.x), LerpT(w, f.v[1], c->tf.axes.fwd.y), LerpT(w, f.v[2], c->tf.axes.fwd.z));
        c->tf.axes.fwd.x = r.x;
        c->tf.axes.fwd.y = r.y;
        c->tf.axes.fwd.z = r.z;
    } else {
        c->tf.axes.fwd.x *= -1.0f;
        c->tf.axes.fwd.y *= -1.0f;
        c->tf.axes.fwd.z *= -1.0f;
    }
    f32 back = Mul(-13.0f, F32(c->car, 0x5E4));
    c->tf.pos.v[0] = target.v[0] + c->tf.axes.fwd.x * back;
    c->tf.pos.v[1] = target.v[1] + c->tf.axes.fwd.y * back;
    c->tf.pos.v[2] = target.v[2] + c->tf.axes.fwd.z * back;
    f32 out = Mul(-dist, F32(c->car, 0x5E4));
    c->view.pos.v[0] = c->pos.v[0] + c->tf.axes.fwd.x * out;
    c->view.pos.v[1] = c->pos.v[1] + c->tf.axes.fwd.y * out;
    c->view.pos.v[2] = c->pos.v[2] + c->tf.axes.fwd.z * out;
    c->view.pos.v[1] = c->view.pos.v[1] + lift * F32(c->car, 0x5E4);
    eye = c->view.pos;
    if (fn_801EA03C((const s16*)(c->car + 0x6C), eye.v)) {
        f32 g = Mul(0.5f + eye.v[1], F32(c->car, 0x5E4));
        eye.v[1] = c->view.pos.v[1] > g ? c->view.pos.v[1] : g;
    } else {
        eye.v[0] = c->view.pos.v[0];
        eye.v[1] = c->view.pos.v[1];
        eye.v[2] = c->view.pos.v[2];
    }
    UpdateCameraHeightTracking((s32*)p, eye.v, target.v);
    CsVec up(0.0f, 1.0f, 0.0f);
    SetupCameraLookAt(eye.v, target.v, p, &up.x);
    c->lastOn = on;
}

}  // extern "C"


ModuleTag lbl_8055F520;
// The four view angles are set by the static initialiser in retail (not constant data): 75, 95, 125 and 110 degrees.
f32 lbl_8055F528 = DegToRadRev(75.0f);
f32 lbl_8055F52C = DegToRadRev(95.0f);  // reference field of view
f32 lbl_8055F530 = DegToRadRev(125.0f);
f32 lbl_8055F534 = DegToRadRev(110.0f);
f32 lbl_8049A208[4];  // lift terms
CamRect lbl_8049A218[4];
