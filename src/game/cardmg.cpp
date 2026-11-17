// Car damage (0x8018BFA0-0x8018FB18): the detachable body parts ("right hood hinge", ...), wheel loss
// ("cardmg: wheel %d is already missing from car %d") and the part break-away timers. Written against
// raw field offsets; the large routines and the initialiser are still stubs. Names are guesses apart
// from the strings.
#include "game/common.h"
#include "game/carlayout.h"
#include "game/mtx33.h"
#include "game/rvec.h"
#include "game/raceopts.h"
#include "game/gamedb.h"
#include <revolution/MTX/vec.h>
#include "game/rigid.h"
#include "game/math_types.h"
#include "game/carbody.h"
#include "game/plainvec.h"
#include "macros.h"

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

// One entry of the damageable part table.
struct DamagePart {
    s32 kind;          // offset 0x0
    const char* name;  // offset 0x4
    s32 bit;           // offset 0x8, the part model visibility bit
    EVec axis;         // offset 0xC, hinge axis
    EVec pivot;        // offset 0x18
    EVec anchor;       // offset 0x24
    f32 size;          // offset 0x30
    s32 x34;           // offset 0x34
    s32 x38;           // offset 0x38
    s32 mirror;        // offset 0x3C
};

// The damageable parts: name, model visibility bit, hinge axis (set up by the initialiser), pivot and anchor.
// DUMP_DATA lbl_80332840 {s32,cstr,s32,f32[3],f32[3],f32[3],f32,s32,s32,s32}
DamagePart lbl_80332840[17] = {
#include "dumpdata/lbl_80332840.inc"
};

// The breakable lights and windows (and the wheel and hood-latch helpers): name, part index, hit radius, body
// point (-1: none).
struct LightPart {
    const char* name;  // offset 0x0
    s32 index;         // offset 0x4
    f32 radius;        // offset 0x8
    s32 point;         // offset 0xC
};

LightPart lbl_80332D08[13] = {
    {"left headlight", 2, 0.0254f, 15},
    {"right headlight", 3, 0.0254f, 16},
    {"left taillight", 0, 0.0254f, 17},
    {"right taillight", 1, 0.0254f, 18},
    {"front window", 4, 0.0254f, 11},
    {"left window", 5, 0.0254f, 13},
    {"right window", 6, 0.0254f, 14},
    {"rear window", 7, 0.0254f, 12},
    {"$W0", 8, 0.2032f, -1},
    {"$W1", 9, 0.2032f, -1},
    {"$W2", 10, 0.2032f, -1},
    {"$W3", 11, 0.2032f, -1},
    {"hood latch", 12, 0.0254f, -1},
};

f32 Dot(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz);

static inline void MulM(DMat3* out, const DMat3* a, const DMat3* b) {
    if (out == b || out == a) {
        f32 m0 = Dot(a->m[0], b->m[0], a->m[3], b->m[1], a->m[6], b->m[2]);
        f32 m3 = Dot(a->m[0], b->m[3], a->m[3], b->m[4], a->m[6], b->m[5]);
        f32 m6 = Dot(a->m[0], b->m[6], a->m[3], b->m[7], a->m[6], b->m[8]);
        f32 m1 = Dot(a->m[1], b->m[0], a->m[4], b->m[1], a->m[7], b->m[2]);
        f32 m4 = Dot(a->m[1], b->m[3], a->m[4], b->m[4], a->m[7], b->m[5]);
        f32 m7 = Dot(a->m[1], b->m[6], a->m[4], b->m[7], a->m[7], b->m[8]);
        f32 m2 = Dot(a->m[2], b->m[0], a->m[5], b->m[1], a->m[8], b->m[2]);
        f32 m5 = Dot(a->m[2], b->m[3], a->m[5], b->m[4], a->m[8], b->m[5]);
        f32 m8 = Dot(a->m[2], b->m[6], a->m[5], b->m[7], a->m[8], b->m[8]);
        out->m[0] = m0;
        out->m[3] = m3;
        out->m[6] = m6;
        out->m[1] = m1;
        out->m[4] = m4;
        out->m[7] = m7;
        out->m[2] = m2;
        out->m[5] = m5;
        out->m[8] = m8;
    } else {
        out->m[0] = Dot(a->m[0], b->m[0], a->m[3], b->m[1], a->m[6], b->m[2]);
        out->m[3] = Dot(a->m[0], b->m[3], a->m[3], b->m[4], a->m[6], b->m[5]);
        out->m[6] = Dot(a->m[0], b->m[6], a->m[3], b->m[7], a->m[6], b->m[8]);
        out->m[1] = Dot(a->m[1], b->m[0], a->m[4], b->m[1], a->m[7], b->m[2]);
        out->m[4] = Dot(a->m[1], b->m[3], a->m[4], b->m[4], a->m[7], b->m[5]);
        out->m[7] = Dot(a->m[1], b->m[6], a->m[4], b->m[7], a->m[7], b->m[8]);
        out->m[2] = Dot(a->m[2], b->m[0], a->m[5], b->m[1], a->m[8], b->m[2]);
        out->m[5] = Dot(a->m[2], b->m[3], a->m[5], b->m[4], a->m[8], b->m[5]);
        out->m[8] = Dot(a->m[2], b->m[6], a->m[5], b->m[7], a->m[8], b->m[8]);
    }
}

// Helpers unproven (names and existence): the forms that keep retail's single constant load, operand order and
// unfolded identities where a plain literal does not.
static inline f32 DmgUnit(f32 v, f32 lo, f32 hi) { return (v - lo) / (hi - lo); }
static inline void DmgScale3(f32* v, f32 k) { v[0] *= k; v[1] *= k; v[2] *= k; }
template <class O> static inline void DmgScaleTo(O* o, const f32* v, f32 k) { o->x = v[0] * k; o->y = v[1] * k; o->z = v[2] * k; }
template <class O, class A, class D> static inline void DmgAddScaled(O* o, const A* a, const D* d, f32 k) { o->x = a->x + d->x * k; o->y = a->y + d->y * k; o->z = a->z + d->z * k; }
static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

static inline void MulVec(EVec* out, const EVec* in, const DMat3* m) {
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

// A point of frame f in its parent's space.
static inline void ToWorld(EVec* out, const EVec* in, const DFrame* f) {
    MulVec(out, in, &f->rot);
    out->x += f->pos.x;
    out->y += f->pos.y;
    out->z += f->pos.z;
}

extern "C" {

// Rotates frame a by b's rotation about b's position: out = a * b, keeping b's pivot where a puts it.
void fn_8018BFA0(DFrame* out, const DFrame* a, const DFrame* b) {
    f32 v[3];
    MulM(&out->rot, &a->rot, &b->rot);
    MulVec((EVec*)v, &b->pos, &a->rot);
    out->pos.x = a->pos.x + v[0];
    out->pos.y = a->pos.y + v[1];
    out->pos.z = a->pos.z + v[2];
    MulVec((EVec*)v, &b->pos, &out->rot);
    out->pos.x = out->pos.x - v[0];
    out->pos.y = out->pos.y - v[1];
    out->pos.z = out->pos.z - v[2];
}

void fn_8018C50C(void) {}

void fn_8018C510(void) {}


// Index of the damage point nearest to pos (entry 0 is skipped).
int fn_8018C514(u8* p, const f32* pos) {
    f32 best = 10000000.0f;
    int idx = 0;
    for (int i = 1; i < *(s32*)((u8*&)((CarBody*)p)->layout); i++) {
        f32 d = PSVECDistance((const Vec*)pos, (const Vec*)((f32*)(((u8*&)((CarBody*)p)->layout) + i * 0x34 + 0x20)));
        if (d < best) {
            best = d;
            idx = i;
        }
    }
    return idx;
}

}  // extern "C"

// The car's paint/flag state block (carstate.cpp).

extern "C" {
void* memset(void* p, int c, unsigned long n);
int fn_801E2CCC(s32 model, const char* name, DFrame** bone);
void fn_802E317C(const char* fmt, ...);
void fn_80279B28(u8* block, CarState* out);
void fn_80279CD8(CarState* s);
}

extern "C" {

// Sets the car's damage up: finds each part's bone and the body points it rides on (its anchor and its
// origin), the dent stiffness from the car's mass and bounce (scaled by the event's player_damage_tweak),
// the paint state, and the body point each light, window and helper sits nearest to.
void fn_8018C5A0(u8* p, u8* desc) {
    memset(p + 0x152C, 0, 0x900);
    DamagePart* d = lbl_80332840;
    CbJoint* s = (CbJoint*)(p + 0x152C);
    for (int i = 0; i < 17; i++, d++, s++) {
        if (fn_801E2CCC(((CarBody*)p)->model, d->name, &s->on)) {
            EVec v;
            ToWorld(&v, &d->anchor, s->on);
            s->point = fn_8018C514(p, &v.x);
            EVec o;
            o.x = 0.0f;
            o.y = 0.0f;
            o.z = 0.0f;
            ToWorld(&v, &o, s->on);
            s->hub = fn_8018C514(p, &v.x);
            s->limit = 1.5707964f;
        }
    }
    ((CarBody*)p)->f5F8 = *(s32*)((u8*&)((CarBody*)p)->layout);
    ((CarBody*)p)->f5F4 = 1818.0f / (0.4545f * ((CarDesc*)desc)->mass);
    if (((CarBody*)p)->f314 >= -0.0001f) {
        ((CarBody*)p)->f5F4 = ((CarBody*)p)->f5F4 * ((CarBody*)p)->f314;
    } else {
        ((CarBody*)p)->f5F4 = (f32)__fabs(((CarBody*)p)->f314);
    }
    if (((CarBody*)p)->f1F0 == 0x50) {
        ValueNode* t = ((EventInfo*)GetEvent(fn_801769CC()->event))->node;
        ((CarBody*)p)->f5F4 = ((CarBody*)p)->f5F4 * t->GetFloat("player_damage_tweak", 1.0f);
    }
    ((CarBody*)p)->f5F4 *= 0.5f;
    if (((CarBody*)p)->f328) {
        fn_80279B28(((CarBody*)p)->f328, (CarState*)(p + 0x1E44));
    } else {
        fn_80279CD8((CarState*)(p + 0x1E44));
    }
    ((CarBody*)p)->saved = ((CarBody*)p)->state;
    for (int i = 0; i < 16; i++) {
        U8(p, 0x1E2C + i) = 0xFF;
    }
    LightPart* l = lbl_80332D08;
    for (int i = 0; i < 13; i++, l++) {
        u8* const& point = &(p + l->index)[0x1E2C];
        DFrame* bone = 0;
        if (l->name[0] == '$') {
            if (l->name[1] == 'W') {
                *point = fn_8018C514(p, (f32*)(p + (l->name[2] - '0') * 0x74 + 0x23DC));
            } else {
                fn_802E317C("cardmg: unknown special helper %s", l->name);
            }
        }
        if (fn_801E2CCC(((CarBody*)p)->model, l->name, &bone)) {
            *point = fn_8018C514(p, &bone->pos.x);
        }
    }
}

}  // extern "C"



struct RVec3;

// A rotation plus position, filled from euler angles (Mtx33::SetEuler, modeinl.cpp). Name unproven.
struct EulerFrame : Mtx33 {
    EVec pos;  // offset 0x24
};

extern "C" {
s32 fn_80129D18(int a, int b);
}

extern "C" {

static inline f32 MinF(f32 v, f32 hi) { return hi < v ? hi : v; }
static inline f32 MaxF(f32 v, f32 lo) { return v > lo ? v : lo; }
static inline f32 ClampF(f32 v, f32 lo, f32 hi) { return MaxF(MinF(v, hi), lo); }
static inline f32 Lower(f32 a, f32 b) { return a < b ? a : b; }

static inline void Scale(EVec* out, const EVec* v, f32 k) {
    out->x = v->x * k;
    out->y = v->y * k;
    out->z = v->z * k;
}

// Turns m by the euler angles e (applied first).
static inline void TurnEuler(DMat3* m, const EVec* e) {
    DMat3 r;
    Mat3FromEuler((Mat3*)&r, (const Vec3*)e);
    Mat3Mul((Mat3*)m, (Mat3*)&r, (Mat3*)m);
}

// Swings the hinged parts (hood, decklid, ...). A hanging part (2) is driven by the torque of the
// wind and car motion about its hinge, damped and limited, and may snap shut and reattach; a bouncing
// part (4) swings freely between its stops. Both rebuild the part frame on the car each tick.
void fn_8018CB3C(u8* p) {
    CbJoint* s = (CbJoint*)(p + 0x152C);
    const DamagePart* d = lbl_80332840;
    for (int i = 0; i < 17; i++, s++, d++) {
        if (s->on && s->state == 2) {
            DFrame f = *s->on;
            EVec ang;
            Scale(&ang, &d->axis, s->angle);
            TurnEuler(&f.rot, &ang);
            EVec hinge;
            MulVec(&hinge, &d->pivot, &f.rot);
            hinge.x += f.pos.x;
            hinge.y += f.pos.y;
            hinge.z += f.pos.z;
            EVec at;
            MulVec(&at, &s->on->pos, (DMat3*)(p + 0x3C));
            at.x += F32(p, 0x60);
            at.y += F32(p, 0x64);
            at.z += F32(p, 0x68);
            EVec h;
            MulVec(&h, &hinge, (DMat3*)(p + 0x3C));
            h.x += F32(p, 0x60);
            h.y += F32(p, 0x64);
            h.z += F32(p, 0x68);
            EVec v1;
            ((RigidBody*)p)->VelAt((RVec3*)&v1, (RVec3*)&h);
            EVec v2;
            ((RigidBody*)p)->VelAt2((RVec3*)&v2, (RVec3*)&at);
            EVec arm;
            arm.x = h.x - at.x;
            arm.y = h.y - at.y;
            arm.z = h.z - at.z;
            v2.y += 4.905f;
            Scale(&v2, &v2, 0.4f);
            if (fn_80129D18(4, 4) < 3) {
                DmgAddScaled(&v2, &v2, &v1, 0.3f);
            }
            EVec torque;
            PSVECCrossProduct((const Vec*)&arm.x, (const Vec*)&v2.x, (Vec*)&torque.x);
            EVec ax;
            MulVec(&ax, &d->axis, &s->on->rot);
            EVec axw;
            MulVec(&axw, &ax, (DMat3*)(p + 0x3C));
            f32 t = -PSVECDotProduct((const Vec*)&torque.x, (const Vec*)&axw.x);
            t = ClampF(t, -5.0f, 5.0f);
            f32 w = s->spin + t;
            s->spin = w - 0.0002f * (0.016683351f * w);
            s->spin = ClampF(s->spin, -20.0f, 20.0f);
            if (s->angle > 1.0f) {
                s->angle = 1.0f;
                s->spin = Lower(s->spin, 0.0f);
            } else if (s->angle < 0.044999998f) {
                if ((fn_801769CC()->type & 0x4000) && s->spin < 0.0f && fn_80129D18(4, 10) < 1) {
                    s->angle = 0.0f;
                    s->spin = 0.0f;
                    s->state = 0;
                } else {
                    s->spin = s->spin + 0.016683351f * (2000.0f * (0.044999998f - s->angle));
                }
            }
            s->angle = s->angle + 0.016683351f * s->spin;
            s->angle = MaxF(s->angle, -0.044999998f);
            EulerFrame m;
            m.pos = s->on->pos;
            EVec* hub = (EVec*)(p + 0xD94) + s->hub;
            m.pos.x += hub->x;
            m.pos.y += hub->y;
            m.pos.z += hub->z;
            EVec e;
            Scale(&e, &d->axis, s->angle);
            EVec er;
            MulVec(&er, &e, &s->on->rot);
            m.SetEuler(er.y, er.x, er.z);
            s->prev = s->cur;
            fn_8018BFA0(&s->cur, (DFrame*)(p + 0x70), (DFrame*)&m);
            if (!s->valid) {
                s->valid = 1;
                s->prev = s->cur;
            }
            s->spin = 0.0f;
        } else if (s->on && s->state == 4) {
            s->angle = s->angle + 0.016683351f * s->spin;
            if (s->angle < 0.0f || s->angle > s->limit) {
                s->angle = 2.0f * ClampF(s->angle, 0.0f, s->limit) - s->angle;
                s->spin = -s->spin;
            }
            EulerFrame m;
            m.pos = s->on->pos;
            EVec* hub = (EVec*)(p + 0xD94) + s->hub;
            m.pos.x += hub->x;
            m.pos.y += hub->y;
            m.pos.z += hub->z;
            EVec e;
            Scale(&e, &d->axis, s->angle);
            EVec er;
            MulVec(&er, &e, &s->on->rot);
            m.SetEuler(er.y, er.x, er.z);
            s->prev = s->cur;
            if (fn_801769CC()->flags & 2) {
                DFrame cf = *(DFrame*)(p + 0x70);
                DmgScale3(&cf.rot.m[0], -1.0f);
                fn_8018BFA0(&s->cur, &cf, (DFrame*)&m);
                DmgScale3(&s->cur.rot.m[0], -1.0f);
            } else {
                fn_8018BFA0(&s->cur, (DFrame*)(p + 0x70), (DFrame*)&m);
            }
            if (!s->valid) {
                s->valid = 1;
                s->prev = s->cur;
            }
        }
    }
}


// A loose part handed to the renderer.
struct DmgDraw {
    DFrame frame;  // offset 0x0
    u32 mask;      // offset 0x30
};

// Views of the car (deformation points) and its draw model used here.
struct DmgCarView {
    u8 pad0[0x5F8];
    s32 nPoints;      // offset 0x5F8
    EVec goal[162];   // offset 0x5FC
    EVec ease[1];     // offset 0xD94
};

struct DmgPose {
    u8 pad0[0x2E0];
    EVec pt[1];  // offset 0x2E0
};

extern f32 lbl_8055E460;    // tick alpha
extern f32 __float_epsilon[];  // FLT_EPSILON

static inline void SetE(EVec* d, const EVec* s) {
    d->x = s->x;
    d->y = s->y;
    d->z = s->z;
}
static inline f32 DmgMax(f32 a, f32 b) { return a > b ? a : b; }
static inline f32 LerpF(f32 a, f32 b, f32 t) { return a + t * (b - a); }

// Fills the model's damage pose: eases the wheel offsets toward their targets, bends the body points by the
// lean/twist (when the deformation is on), hides the parts that are gone and lists the flying ones with their
// frame interpolated to the tick.
/* Helper name unproven: one damage part per joint - hides a part that is gone (state 3), lists a flying one (states 2, 4) with
   its frame interpolated to the tick. Machine-extracted (loopext); the boundary is needed (in place: fn_8018DA5C 70 -> 102
   diff rows, measured 2026-11-16) */
static inline void ListDamagePart(CbJoint* s, u8* m, DamagePart* tp, f32 t) {
    if (s->state == 3) {
        U32(m, 0x29C) &= ~(1 << tp->bit);
    }
    if (s->state == 2 || s->state == 4) {
        DmgDraw* d = (DmgDraw*)(m + 0xA7C) + S32(m, 0xA78)++;
        f64 absStep = __fabs(t - 1.0f);
        f32 step = (f32)absStep;
        if (step <= __float_epsilon[0]) {
            d->frame = s->cur;
        } else {
            DFrame f;
            f32* o = (f32*)&f;
            f32* a = (f32*)&s->prev;
            f32* b = (f32*)&s->cur;
            for (int j = 0; j < 12; j++) {
                *o++ = LerpF(*a++, *b++, t);
            }
            d->frame = f;
        }
        d->mask = 1 << tp->bit;
    }
}

static inline void EaseV(EVec* e, const EVec* g, f32 a, f32 b) {
    e->x *= a;
    e->y *= a;
    e->z *= a;
    e->x += g->x * b;
    e->y += g->y * b;
    e->z += g->z * b;
}

static inline f32 AbsFlt(f32 v) { return (f32)__fabs(v); }

/* Helper name unproven */
static inline void ApplyDamagePointDeformation(DmgCarView* car, u8* p, f32 roll, f32 lean, f32 twist, DmgPose* mdl) {
    for (int i = 1; i < car->nPoints; i++) {
        u8* pt = PTR(p, 0x26C) + i * 0x34;
        EVec v;
        v.x = 0.0f;
        v.y = 0.0f;
        v.z = 0.0f;
        f32 z = F32(pt, 0x28);
        double az_d = __fabs(z);
        f32 az = az_d;
        v.x += roll * (-0.3f * (F32(pt, 0x20) * (az - 0.7f)));
        v.y += roll * (-0.3f * (F32(pt, 0x24) * (az - 0.7f)));
        v.z += -0.5f * z * roll;
        f32 w = DmgMax(0.0f, 3.0f - az);
        v.y += 0.5f * lean * (F32(pt, 0x24) * w);
        v.x += twist * DmgMax(0.0f, 3.0f - AbsFlt(F32(pt, 0x28)));
        mdl->pt[i] = v;
    }
}

#pragma push
#pragma opt_lifetimes off  /* Pragma unproven: helps but not exact (pragcheck 2026-11-16: fn_8018DA5C 99.19 with it, 97.63 without) */
void fn_8018DA5C(u8* p, u8* m) {
    DmgCarView* car = (DmgCarView*)p;
    f32 roll;
    DmgPose* mdl = (DmgPose*)m;
    EVec* e = car->ease;
    for (int i = 0; i < car->nPoints; i++) {
        EaseV(&car->ease[i], &car->goal[i], 0.8f, 1.0f - 0.8f);
        SetE(&mdl->pt[i], e + i);
    }
    if (((CarBody*)p)->f26E0) {
        f32 lean = ((CarBody*)p)->f26E8;
        f32 twist = 0.2f * ((CarBody*)p)->f26E4;
        roll = 0.1f * ((CarBody*)p)->f26EC;
        lean = lean > -0.25f ? lean : -0.25f;
        ApplyDamagePointDeformation(car, p, roll, lean, twist, mdl);
    }
    ((u32&)((CarDrawState*)m)->f29C) |= ((u32&)((CarBody*)p)->f1E40);
    DamagePart* tp = lbl_80332840;
    CbJoint* s = (CbJoint*)&((CarBody*)p)->joints;
    f32 t = lbl_8055E460;
    ((CarDrawState*)m)->nPoints = 0;
    for (int i = 0; i < 17; i++) {
        if (s->on && s->state != 0 && s->state != 1) {
            ListDamagePart(s, m, tp, t);
        }
        tp++;
        s++;
    }
}
#pragma pop

// A deformable point of the car body (0x34 bytes, from the model at car + 0x26C).
struct DentPoint {
    s32 unk0;     // offset 0x0
    s32 goal;     // offset 0x4: index into the car's spring goals (car + 0x5FC)
    DentVec lo;   // offset 0x8
    DentVec hi;   // offset 0x14
    DentVec pos;  // offset 0x20
    f32 unk2C;    // offset 0x2C
    f32 soft;     // offset 0x30
};

// A spring link between two dent points (0x14 bytes).
struct DentLink {
    s32 a;        // offset 0x0
    s32 b;        // offset 0x4
    f32 along;    // offset 0x8: share of the push passed along the link
    f32 across;   // offset 0xC: share of the push passed across it
    f32 lift;     // offset 0x10
};

static inline f32 DentClamp(f32 v, f32 lo, f32 hi) {
    v = hi < v ? hi : v;
    return v > lo ? v : lo;
}


// Dents point i of the body by `push`: moves its spring goal (pushing out only along the point's outward
// direction is cancelled), clamps it to the point's box, then passes the push on to linked points (not back to
// `from`), up to three links deep.
void fn_8018DE94(u8* p, int i, const DentVec* push, int from, int depth) {
    DentPoint* pt = (DentPoint*)((u8*&)((CarBody*)p)->layout) + i;
    f32 k = 10.0f * (0.0000068539325f * pt->soft);
    f32 mag = PSVECMag((const Vec*)&push->x);
    DentVec d;
    d.x = push->x * k;
    d.y = push->y * k;
    d.z = push->z * k;
    d.y = d.y < 0.0f ? d.y : 0.0f;
    DentVec* g = (DentVec*)(p + 0x5FC) + pt->goal;
    g->AddScaled(d, 2.0f);
    if (PSVECDotProduct((const Vec*)&g->x, (const Vec*)&pt->pos.x) > 0.0f) {
        DentVec n = pt->pos;
        PSVECNormalize((const Vec*)&n.x, (Vec*)&n.x);
        f32 out = -PSVECDotProduct((const Vec*)&g->x, (const Vec*)&n.x);
        g->x += n.x * out;
        g->y += n.y * out;
        g->z += n.z * out;
    }
    g->x = DentClamp(g->x, pt->lo.x, pt->hi.x);
    g->y = DentClamp(g->y, pt->lo.y, pt->hi.y);
    g->z = DentClamp(g->z, pt->lo.z, pt->hi.z);
    if (depth <= 2) {
        int other;
        DentVec* gi = (DentVec*)(p + 0x5FC) + i;
        DentVec at;
        at.x = pt->pos.x + gi->x;
        at.y = pt->pos.y + gi->y;
        at.z = pt->pos.z + gi->z;
        for (int n = 0; n < S32(((u8*&)((CarBody*)p)->layout), 0x20F0); n++) {
            DentLink* l = (DentLink*)PTR(((u8*&)((CarBody*)p)->layout), 0x20EC) + n;
            other = l->a;
            if ((other == i && l->b != from) || (l->b == i && other != from)) {
                if (other == i) {
                    other = l->b;
                }
                DentPoint* o = (DentPoint*)((u8*&)((CarBody*)p)->layout) + other;
                DentVec* go = (DentVec*)(p + 0x5FC) + other;
                DentVec op;
                // MATCHING-AID (kept, audited 2026-11-16): y read ahead of x (single-use reads); the target adds y first but stores x
                // first. Natural forms measured in fn_8018DE94 (EXACT with this): plain sums 14 diff rows, y operands swapped 14,
                // y first 18, reversed operands 18, copy then add 24, only go->y in a local 6.
                f32 yv = go->y;
                f32 y2 = o->pos.y;
                op.x = o->pos.x + go->x;
                op.y = y2 + yv;
                op.z = o->pos.z + go->z;
                DentVec dir;
                dir.x = op.x - at.x;
                dir.y = op.y - at.y;
                dir.z = op.z - at.z;
                f32 len = PSVECMag((const Vec*)&dir.x);
                DentVec f;
                if (len > 0.01f) {
                    f32 inv = 1.0f / len;
                    dir.x *= inv;
                    dir.y *= inv;
                    dir.z *= inv;
                    f32 t = -PSVECDotProduct((const Vec*)&dir.x, (const Vec*)&push->x);
                    f.x = push->x + dir.x * t;
                    f.y = push->y + dir.y * t;
                    f.z = push->z + dir.z * t;
                    f32 c = l->across;
                    f.x *= c;
                    f.y *= c;
                    f.z *= c;
                    f32 s = l->along * PSVECDotProduct((const Vec*)&push->x, (const Vec*)&dir.x);
                    f.x += dir.x * s;
                    f.y += dir.y * s;
                    f.z += dir.z * s;
                    DentVec up;
                    up.x = 0.0f;
                    up.y = 1.0f;
                    up.z = 0.0f;
                    f32 m = mag * l->lift;
                    f.x += up.x * m;
                    f.y += up.y * m;
                    f.z += up.z * m;
                } else {
                    f32 a = l->along;
                    f.x = push->x * a;
                    f.y = push->y * a;
                    f.z = push->z * a;
                }
                fn_8018DE94(p, other, &f, i, depth + 1);
            }
        }
    }
}

}  // extern "C"

// The running game mode (only the slot used here is named).
class GameMode {
public:
    virtual ~GameMode(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(void);
    virtual void v1C(void);
    virtual void v20(void);
    virtual void v24(void);
    virtual void v28(void);
    virtual void v2C(void);
    virtual void v30(void);
    virtual void v34(void);
    virtual void v38(void);
    virtual void v3C(void);
    virtual void v40(void);
    virtual void v44(void);
    virtual void v48(void);
    virtual void v4C(void);
    virtual void v50(void);
    virtual void v54(void);
    virtual void v58(void);
    virtual void v5C(void);
    virtual void v60(void);
    virtual void v64(void);
    virtual void v68(void);
    virtual void v6C(void);
    virtual void v70(void);
    virtual void v74(void);
    virtual void v78(void);
    virtual void v7C(void);
    virtual void v80(void);
    virtual void v84(void);
    virtual void v88(void);
    virtual void v8C(void);
    virtual void v90(void);
    virtual void v94(void);
    virtual void v98(void);
    virtual void v9C(void);
    virtual void vA0(void);
    virtual void ResetCar(u8* car);  // 0xA4
};

class DmgModel {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual u8* Current(void);  // 0x10
    virtual void v14(void);
    virtual void v18(void);
    virtual void v1C(void);
    virtual void v20(void);
    virtual void v24(void);
    virtual void v28(void);
    virtual void v2C(void);
    virtual void Impair(s32 by);  // 0x30
    virtual void v34(void);
    virtual int Busy(void);       // 0x38
};

extern "C" {
GameMode* fn_801B83A0(void);
void fn_80161A38(int a, u8* car, int b);
void fn_801DFF1C(u8* car);
extern f32 lbl_8055E3C8;

void fn_8018E330(u8* p) {
    fn_801B83A0()->ResetCar(p);
    fn_80161A38(0, ((u8*&)((CarBody*)p)->index), 0);
    fn_801DFF1C(((u8*&)((CarBody*)p)->index));
    f32 zero = 0.0f;
    f32 k = 0.5f;
    ((CarBody*)p)->f258C = zero;
    ((CarBody*)p)->f2690 = k;
    ((CarBody*)p)->f25BC = zero;
    ((CarBody*)p)->f26A0 = lbl_8055E3C8;
    ((CarBody*)p)->f26A4 = S32(((DmgModel*)((u8*&)((CarBody*)p)->src))->Current(), 0x4);
    ((CarBody*)p)->f2588 = 0.0f;
    ((CarBody*)p)->f25A0 = 0.0f;
    ((CarBody*)p)->f259D = 0;
    ((CarBody*)p)->f259C = 0;
    ((CarBody*)p)->f2638 = 0;
    ((CarBody*)p)->f2644 = 0;
    ((CarBody*)p)->f2630 = 0.0f;
    ((CarBody*)p)->f262C = 0.0f;
    ((CarBody*)p)->f2628 = 0.0f;
}

// DVec: valvec3.h's ValVec3.
// The car's spring goals, one per body point.
struct DentCar {
    u8 pad0[0x5FC];
    DentVec goal[1];  // offset 0x5FC
};

// A vector as the collision code passes it around (copied word by word).
struct AVec {
    f32 v[3];
};

// A body point viewed from its spring-goal index on (the point array starts 4 bytes into the model).
struct DentNode {
    s32 goal;    // offset 0x0
    DentVec lo;  // offset 0x4
    DentVec hi;  // offset 0x10
    DentVec pos; // offset 0x1C
    f32 unk28;   // offset 0x28
    f32 soft;    // offset 0x2C
    u8 fixed;    // offset 0x30: never dented
    u8 pad31[0x3];
};

// A breaking-glass event: where, how fast, which car.
struct GlassMsg {
    EVec pos;  // offset 0x0
    EVec vel;  // offset 0xC
    s16 car;   // offset 0x18
};

void fn_801BF2A4(u8* car, u32 mask, ValVec3* center, ValVec3* lo, ValVec3* hi, s32 kind, u8* fx);
f32 lbl_8055A460 = 4.0f;  // 4
extern f32 __float_epsilon[];      // FLT_EPSILON
int fn_80189BDC(u8* p);
u8* fn_8017960C(s32 car);
void fn_8018A174(u8* p);
void fn_801796CC(int kind, void* msg, int size);
void fn_801B8610(s32 car, s32 by);
void fn_8018F76C(u8* p, int i);
void fn_802E30C8(const char* fmt, ...);

static inline int NearZero(f32 x) { return (f32)__fabs(x) <= __float_epsilon[0]; }
static inline int Boosting(const u8* car) { return F32(car, 0x25BC) > __float_epsilon[0]; }
static inline int HitTooSoon(const u8* car) { return lbl_8055E3C8 < 3.0f + F32(car, 0x26D8); }

// Which corner of the car a body-space point is nearest: 0/1 front left/right, 3/2 rear left/right.
// Retail bug: the rear branch tests `front` again (false there) instead of `left`, so a rear hit is always
// corner 3 (retail keeps the dead li 2 behind a beq on the front flag).
static inline int HitCorner(const AVec* at) {
    int front = at->v[2] > 0.0f;
    int left = at->v[0] < 0.0f;
    int corner;
    if (front) {
        corner = !left;
    } else {
        corner = 3;
        if (front) {
            corner = 2;
        }
    }
    return corner;
}

// A collision impulse `imp` at body point `at` (negated when `flip`) from car `other` (-1: the world) of
// surface kind `kind`: accumulates the impulse, decides whether the hit is hard enough to crash the car, dents
// the nearest body point and spreads the dent, breaks off parts and lights pushed past their limits, and
// raises the car's impairment.
static inline double AbsDbl(double v) { return __fabs(v); }

void fn_8018E3EC(u8* p, const AVec* imp, const AVec* at, int flip, s32 other, int kind, u8*, int) {
    DentVec nd;
    DentVec dg;
    DentVec c0;
    DentVec w;
    DentVec r;
    ValVec3 c;
    GlassMsg msg;
    AVec di;
    AVec da;
    AVec h;
    DentVec cand;
    AVec g0;
    f32 mass = ((CarBody*)p)->mass;
    f32 inv = -0.1f / mass;
    ((CarBody*)p)->f26F0 += imp->v[0] * inv;
    ((CarBody*)p)->f26F4 += imp->v[1] * inv;
    ((CarBody*)p)->f26F8 += imp->v[2] * inv;
    if (!fn_80189BDC(p)) {
        return;
    }
    if (NearZero(((CarBody*)p)->f2690)) {
        if ((((CarBody*)p)->mtx.rot.y.y < 0.0f && kind != 0x6D) || kind == 0x3E7) {
            fn_8018E330(p);
        }
    }
    if (kind == 0x73) {
        int fast = ((CarBody*)p)->f25C8 || Boosting(p);
        if (fast) {
            return;
        }
    }
    if (kind == 0x74) {
        return;
    }
    di = *imp;
    if (flip) {
        di.v[0] *= -1.0f;
        di.v[1] *= -1.0f;
        di.v[2] *= -1.0f;
    }
    da = *at;
    f32 mag = PSVECMag((const Vec*)di.v);
    if (!(fn_801769CC()->type & 0x4000) && ((CarBody*)p)->f2690 < 0.001f) {
        if (mag < 1500.0f) {
            return;
        }
        if (HitTooSoon(p)) {
            return;
        }
        if (other >= 0 && Boosting(p)) {
            return;
        }
        h = di;
        h.v[1] = MinF(h.v[1], 0.0f);
        f32 hm = PSVECMag((const Vec*)h.v);
        if (other >= 0) {
            hm *= 0.3f;
            if (((CarBody*)p)->f1F0 != 0x50 && !Boosting(p)) {
                if (Boosting(fn_8017960C(other)) || hm > 15000.0f) {
                    if (((CarBody*)p)->speed < F32(fn_8017960C(other), 0x25D4)) {
                        fn_8018A174(p);
                    }
                }
            }
        } else if (kind == 0x73) {
            hm *= lbl_8055A460;
        }
        if (other < 0 || !Boosting(fn_8017960C(other))) {
            if (hm < 30000.0f) {
                return;
            }
            if (other < 0 && ((CarBody*)p)->speed < 13.333333f && hm < 300000.0f) {
                return;
            }
        }
        if (fn_801769CC()->type == 0x180304 && ((CarBody*)p)->f1F0 == 0x50 && other >= 0) {
            return;
        }
        fn_8018E330(p);
    }
    DentNode* best = 0;
    f32 bestD = 100000.0f;
    for (int i = 1; i < ((CarBody*)p)->f5F8; i++) {
        DentNode* d = (DentNode*)(((u8*&)((CarBody*)p)->layout) + 4) + i;
        if (!d->fixed) {
            cand.x = d->pos.x + ((DentCar*)p)->goal[i].x * 1.8f;
            cand.y = d->pos.y + ((DentCar*)p)->goal[i].y * 1.8f;
            cand.z = d->pos.z + ((DentCar*)p)->goal[i].z * 1.8f;
            f32 dd = PSVECSquareDistance((const Vec*)&cand.x, (const Vec*)da.v);
            if (dd < 2.25f && dd < bestD) {
                best = d;
                bestD = dd;
            }
        }
    }
    if (best) {
        g0 = *(AVec*)((DentVec*)(p + 0x5FC) + best->goal);
        fn_8018DE94(p, best->goal, (DentVec*)&di, -1, 0);
        DmgScaleTo(&nd, di.v, -1.0f);
        PSVECNormalize((const Vec*)&nd.x, (Vec*)&nd.x);
        DentVec* gb = (DentVec*)(p + 0x5FC) + best->goal;
        dg.x = g0.v[0] - gb->x;
        dg.y = g0.v[1] - gb->y;
        dg.z = g0.v[2] - gb->z;
        f32 moved = (f32)AbsDbl(PSVECDotProduct((const Vec*)&nd.x, (const Vec*)&dg.x));
        gb = (DentVec*)(p + 0x5FC) + best->goal;
        c0.x = best->pos.x + gb->x;
        c0.y = best->pos.y + gb->y;
        c0.z = best->pos.z + gb->z;
        int count = ((CarBody*)p)->f5F8;
        for (int i = 1; i < count; i++) {
            DentPoint* q = (DentPoint*)((u8*&)((CarBody*)p)->layout) + i;
            DentVec* g = (DentVec*)(p + 0x5FC) + q->goal;
            w.x = q->pos.x + g->x;
            w.y = q->pos.y + g->y;
            w.z = q->pos.z + g->z;
            r.x = w.x - c0.x;
            r.y = w.y - c0.y;
            r.z = w.z - c0.z;
            f32 t = PSVECDotProduct((const Vec*)&nd.x, (const Vec*)&r.x);
            f32 a = t < moved ? t : moved;
            if (a > 0.0f) {
                f32 r2 = PSVECDotProduct((const Vec*)&r.x, (const Vec*)&r.x);
                if (0.1f > r2) {
                    r2 = 0.1f;
                }
                f32 s = 0.2f / r2;
                if (1.0f < s) {
                    s = 1.0f;
                }
                f32 m = -a * s;
                g->x += nd.x * m;
                g->y += nd.y * m;
                g->z += nd.z * m;
                g->x = DentClamp(g->x, q->lo.x, q->hi.x);
                g->y = DentClamp(g->y, q->lo.y, q->hi.y);
                g->z = DentClamp(g->z, q->lo.z, q->hi.z);
            }
        }
        LightPart* l;
        CbJoint* sl = (CbJoint*)(p + 0x152C);
        DamagePart* t = lbl_80332840;
        for (int i = 0; i < 17; i++) {
            if ((sl->on && sl->point > 0 && !sl->state) || sl->state == 2) {
                DentVec* g = (DentVec*)(p + 0x5FC) + sl->point;
                if (PSVECDotProduct((const Vec*)&g->x, (const Vec*)&g->x) > t->size * t->size) {
                    if (!((f32)__fabs(PSVECDotProduct((const Vec*)&t->axis.x, (const Vec*)&t->axis.x)) <= __float_epsilon[0])) {
                        if ((u32)t->kind <= 1) {
                            if (sl->state != 2) {
                                sl->state = 2;
                                sl->valid = 0;
                            }
                        } else {
                            int roll = fn_80129D18(4, 10);
                            if (sl->state == 2) {
                                if (roll < 2) {
                                    sl->state = 3;
                                    sl->valid = 0;
                                }
                            } else {
                                if (roll < 7) {
                                    sl->state = 2;
                                } else if (roll < 8) {
                                    sl->state = 3;
                                } else {
                                    sl->state = 1;
                                }
                                sl->valid = 0;
                            }
                            if (sl->state == 2) {
                                sl->angle = sl->angle > 0.01f ? sl->angle : 0.01f;
                                sl->spin = sl->spin > 0.2f ? sl->spin : 0.2f;
                            }
                        }
                    } else {
                        sl->state = 3;
                        sl->valid = 0;
                    }
                    if (sl->state == 3) {
                        ValVec3* lo = &((CarBody*)p)->layout->bounds[t->bit].min;
                        ValVec3* hi = lo + 1;
                        c.x = lo->x + (lo + 1)->x;
                        c.y = lo->y + hi->y;
                        c.z = lo->z + (lo + 1)->z;
                        c.x *= -0.5f;
                        c.y *= -0.5f;
                        c.z *= -0.5f;
                        fn_801BF2A4(p, 1 << t->bit, &c, lo, lo + 1, t->x34, (u8*)t + 0x38);
                    }
                }
            }
            sl++;
            t++;
        }
        l = lbl_80332D08;
        for (int i = 0; i < 13; i++) {
            int idx = l->index;
            u32 bit = 1 << idx;
            u8 pt = U8(p, 0x1E2C + idx);
            if (pt != 0xFF && !(((u32&)((CarBody*)p)->f1E3C) & bit)) {
                DentVec* g = (DentVec*)(p + 0x5FC) + pt;
                if (PSVECDotProduct((const Vec*)&g->x, (const Vec*)&g->x) > l->radius * l->radius) {
                    ((u32&)((CarBody*)p)->f1E3C) |= bit;
                    if (l->point > 0) {
                        ((u32&)((CarBody*)p)->f1E40) |= 1 << l->point;
                    }
                    if (idx >= 2 && idx <= 7) {
                        DentPoint* q = (DentPoint*)((u8*&)((CarBody*)p)->layout) + U8(p, 0x1E2C + idx);
                        ToWorld(&msg.pos, (EVec*)&q->pos, (DFrame*)&((CarBody*)p)->mtx);
                        ((RigidBody*)p)->VelAt((RVec3*)&msg.vel, (RVec3*)&msg.pos);
                        msg.car = *(s16*)&((CarBody*)p)->hint;
                        fn_801796CC(7, &msg, sizeof(GlassMsg));
                    }
                    if (idx >= 8 && idx <= 11 && fn_80129D18(4, 10) < 5) {
                        fn_8018F76C(p, idx - 8);
                    }
                    if (idx == 12) {
                        if (fn_80129D18(4, 10) < 5) {
                            ((CarBody*)p)->a268C = 6;
                        } else {
                            ((CarBody*)p)->a268C = 3;
                        }
                        CbJoint* hood = (CbJoint*)(p + 0x152C);
                        if (hood->state != 3) {
                            hood->state = 2;
                            hood->valid = 0;
                            hood->spin = 10.0f;
                            hood->angle = hood->angle > 0.5f ? hood->angle : 0.5f;
                        }
                    }
                }
            }
            l++;
        }
    }
    f32 was = ((CarBody*)p)->f2690;
    if (fn_801769CC()->type & 0x4000) {
        int hit = 0;
        s32 st = ((CarBody*)p)->a268C;
        if (st == 0) {
            if (mag > 300.0f) {
                f32 cap = kind != 0x70 ? 0.1f : 0.35f;
                f32 add = 6.6666666e-06f * mag;
                add = add < cap ? add : cap;
                f32 v = ((CarBody*)p)->f2690 += add;
                if (v > 0.6f || (v > 0.5f && add > 0.03f)) {
                    if (fn_80129D18(4, 2) == 0) {
                        ((CarBody*)p)->a268C = 2;
                    } else {
                        ((CarBody*)p)->a268C = 3;
                    }
                    if (((CarBody*)p)->f26B0 < 0 && fn_80129D18(4, 6) == 0) {
                        ((CarBody*)p)->f26B0 = HitCorner(&da);
                    }
                }
            }
        } else if (st >= 1 && st <= 4) {
            if (mag > 300.0f) {
                f32 cap = kind != 0x70 ? 0.1f : 0.35f;
                f32 add = 6.6666666e-06f * mag;
                add = add < cap ? add : cap;
                f32 v = ((CarBody*)p)->f2690 += add;
                if (kind == 0x70 && other == ((CarBody*)p)->index && v > 0.9f) {
                    ((CarBody*)p)->f2690 = was > 0.9f ? was : 0.9f;
                }
                if (((CarBody*)p)->f2690 > 1.0f) {
                    ((CarBody*)p)->f2690 = 1.0f;
                    switch (fn_80129D18(4, 3)) {
                    case 0:
                        ((CarBody*)p)->a268C = 10;
                        break;
                    case 1:
                        ((CarBody*)p)->a268C = 11;
                        break;
                    default:
                        ((CarBody*)p)->a268C = 12;
                        break;
                    }
                    if (fn_80129D18(4, 4) == 0) {
                        int corner = HitCorner(&da);
                        if (!(U32(p + corner * 0x74, 0x241C) & 8)) {
                            fn_8018F76C(p, corner);
                        }
                    }
                    fn_802E30C8("car %d: impaired", ((CarBody*)p)->index);
                    hit = 1;
                    fn_801B8610(((CarBody*)p)->index, other);
                }
            }
        } else if (st >= 9) {
            if (!((DmgModel*)((u8*&)((CarBody*)p)->src))->Busy()) {
                fn_802E30C8("car %d: re-impairing?", ((CarBody*)p)->index);
                hit = 1;
            }
        }
        if (hit) {
            ((DmgModel*)((u8*&)((CarBody*)p)->src))->Impair(other);
        }
    }
    ((CarBody*)p)->f269C = 0.0f;
}


}  // extern "C"

static inline f32 FMin(f32 a, f32 b) { return b < a ? b : a; }
static inline f32 FMax(f32 a, f32 b) { return a > b ? a : b; }

// A dent offset (-0.508..0.508) packed into a byte.
static inline u8 PackDent(f32 v) {
    f32 t = FMax(FMin((v - -0.508f) / 1.016f, 1.0f), 0.0f);
    return LerpF(0.0f, 255.0f, t) + 0.5f;
}

#define DMG_MIN(a, b) ((a) < (b) ? (a) : (b))

// A packed dent byte back to its offset.
static inline f32 UnpackDent(u32 b, f32 span, f32 lo, f32 hi) {
    f32 t = DMG_MIN(1.0f, (b - lo) / (hi - lo));
    t = FMax(t, lo);
    return -0.508f + t * span;
}

extern "C" {

// Restores the damage state saved by fn_8018F538.
void fn_8018F2C0(u8* p, const u8* q) {
    f32 h = ((CarState*)q)->time;
    ((CarBody*)p)->f2690 = h;
    ((CarBody*)p)->f2694 = h;
    ((CarBody*)p)->f2698 = ((CarState*)q)->duration;
    ((CarBody*)p)->a268C = ((CarState*)q)->x298;
    const u8* o = q;
    u8* d = p;
    const f32 dentSpan = 1.016f;
    for (int i = 0; i < ((CarBody*)p)->f5F8; o += 4, i++, d += 0xC) {
        F32(d, 0x5FC) = UnpackDent(o[0], dentSpan, 0.0f, 255.0f);
        F32(d, 0x600) = UnpackDent(o[1], dentSpan, 0.0f, 255.0f);
        f32 z = UnpackDent(o[2], dentSpan, 0.0f, 255.0f);
        F32(d, 0x604) = z;
        F32(d, 0xD94) = F32(d, 0x5FC);
        F32(d, 0xD98) = F32(d, 0x600);
        F32(d, 0xD9C) = z;
    }
    u8* s = p;
    for (int i = 0; i < 17; i++, s += 0x80) {
        S32(s, 0x15A8) = ((CarState*)q)->flags.f[i];
        U8(s, 0x15A4) = 0;
    }
    ((CarBody*)p)->f1E3C = ((CarState*)q)->x29C;
}

}  // extern "C"

extern "C" {

// Saves the damage state (dents packed to bytes, part states) into a snapshot.
void fn_8018F538(u8* p, u8* q) {
    ((CarState*)q)->time = ((CarBody*)p)->f2690;
    ((CarState*)q)->duration = ((CarBody*)p)->f2698;
    ((CarState*)q)->x298 = ((CarBody*)p)->a268C;
    u8* d = p;
    u8* o = q;
    for (int i = 0; i < ((CarBody*)p)->f5F8; d += 0xC, i++, o += 4) {
        o[0] = PackDent(F32(d, 0x5FC));
        o[1] = PackDent(F32(d, 0x600));
        o[2] = PackDent(F32(d, 0x604));
    }
    ((CarState*)q)->flags.f[0] = ((CarBody*)p)->joints[0].state;
    ((CarState*)q)->flags.f[1] = ((CarBody*)p)->joints[1].state;
    ((CarState*)q)->flags.f[2] = ((CarBody*)p)->joints[2].state;
    ((CarState*)q)->flags.f[3] = ((CarBody*)p)->joints[3].state;
    ((CarState*)q)->flags.f[4] = ((CarBody*)p)->joints[4].state;
    ((CarState*)q)->flags.f[5] = ((CarBody*)p)->joints[5].state;
    ((CarState*)q)->flags.f[6] = ((CarBody*)p)->joints[6].state;
    ((CarState*)q)->flags.f[7] = ((CarBody*)p)->joints[7].state;
    ((CarState*)q)->flags.f[8] = ((CarBody*)p)->joints[8].state;
    ((CarState*)q)->flags.f[9] = ((CarBody*)p)->joints[9].state;
    ((CarState*)q)->flags.f[10] = ((CarBody*)p)->joints[10].state;
    ((CarState*)q)->flags.f[11] = ((CarBody*)p)->joints[11].state;
    ((CarState*)q)->flags.f[12] = ((CarBody*)p)->joints[12].state;
    ((CarState*)q)->flags.f[13] = ((CarBody*)p)->joints[13].state;
    ((CarState*)q)->flags.f[14] = ((CarBody*)p)->joints[14].state;
    ((CarState*)q)->flags.f[15] = ((CarBody*)p)->joints[15].state;
    ((CarState*)q)->flags.f[16] = ((CarBody*)p)->joints[16].state;
    ((CarState*)q)->x29C = ((CarBody*)p)->f1E3C;
}

void fn_8018F6F8(u8* p) {
    DamagePart* t = lbl_80332840;
    u8* w = p + 0x152C;
    for (int i = 0; i < 17; i++) {
        if (U32(w, 0x0) && !S32(w, 0x7C) && (u32)(t->kind - 14) <= 2) {
            S32(w, 0x7C) = 4;
            U8(w, 0x78) = 0;
            F32(w, 0xC) = 0.0f;
            F32(w, 0x10) = F32(w, 0x14) / 0.75f;
        }
        t++;
        w += 0x80;
    }
}

}  // extern "C"

// The solver's 4-vector record and the game-side vector built from it (carbody CbVec4, minworld MWV4).
/* Helper names unproven (CDSolVec, RVec3, CDF, CDVec4, Set, Sol) */
struct CDSolVec {
    CDSolVec(void) {}
    f32 x, y, z, w;
};

static inline CDSolVec CDSolVec4(f32 x, f32 y, f32 z, f32 w) {
    CDSolVec v;
    v.x = x;
    v.y = y;
    v.z = z;
    v.w = w;
    return v;
}

// CDV3: rvec.h's RVec3.
struct CDF {
    CDF(f32 a) { v = a; }
    f32 v;
};

struct CDVec4 {
    CDVec4(CDSolVec s) {
        CDF f(s.w);
        Set(RVec3(s.x, s.y, s.z), f);
    }
    void Set(const RVec3& p, CDF w_) {
        x = p.x;
        y = p.y;
        z = p.z;
        w = w_.v;
    }
    CDSolVec Sol(void) const { return CDSolVec4(x, y, z, w); }
    f32& operator[](int i) { return (&x)[i]; }
    f32 x, y, z, w;
};

struct WVec3 {
    WVec3(void) {}
    WVec3(const WVec3& o) {
        x = o.x;
        y = o.y;
        z = o.z;
    }
    f32 x, y, z;
};

struct PFrame {
    PVec3 basis[3];
};

struct WFrame {
    PVec3 basis[3];
};

// The wheel's launch frame: the car's rotation rows with the hub as the position.
struct WLaunch {
    WFrame rot;
    WVec3 pos;
};

extern "C" {
void fn_802E30C8(const char* fmt, ...);
void fn_801BEB30(s32 car, u8** out);
void fn_801AFC44(s32 car, int wheel, WLaunch* m, WVec3* vel, WVec3* imp);

// Knocks wheel i off the car: dims its channel in the car's cross-link colour, then launches it from
// the hub with the car's velocity plus its own spin.
void fn_8018F76C(u8* p, int i) {
    u8* w = p + i * 0x74;
    if (U32(w, 0x241C) & 8) {
        fn_802E30C8("cardmg: wheel %d is already missing from car %d", i, ((CarBody*)p)->index);
    }
    u8* x;
    WLaunch m;
    fn_801BEB30(((CarBody*)p)->index, &x);
    CDVec4 c(*(CDSolVec*)(x + 0x70));
    c[i] *= 0.15f;
    *(CDSolVec*)(x + 0x70) = c.Sol();
    m.rot = *(WFrame*)&((CarBody*)p)->mtx;
    m.pos = *(WVec3*)(w + 0x23D0);
    m.pos.x += m.rot.basis[1].x * 0.3f;
    m.pos.y += m.rot.basis[1].y * 0.3f;
    f32 zv = m.rot.basis[1].z;
    m.pos.z += zv * 0.3f;
    WVec3 vel;
    ((RigidBody*)p)->VelAt((RVec3*)&vel, (RVec3*)&m.pos);
    f32 s = F32(w, 0x23AC);
    if ((f32)__fabs(s) < 5.0f) {
        s = s < 0.0f ? -5.0f : 5.0f;
    }
    f32 spin = F32(w, 0x23AC);
    WVec3 imp;
    imp.x = m.rot.basis[0].x * spin;
    imp.y = m.rot.basis[0].y * spin;
    imp.z = m.rot.basis[0].z * spin;
    imp.x += ((CarBody*)p)->avel.x;
    imp.y += ((CarBody*)p)->avel.y;
    imp.z += ((CarBody*)p)->avel.z;
    fn_801AFC44(((CarBody*)p)->index, i, &m, &vel, &imp);
    U32(w, 0x241C) |= 8;
}

// Live state of one damageable part (17 of them at car + 0x152C).
struct PartState {
    u32 attached;    // offset 0x0
    s32 hp;          // offset 0x4
    u8 pad8[0x70];   // offset 0x8
    u8 flag78;       // offset 0x78
    u8 pad79[0x3];
    s32 state;       // offset 0x7C
};


// Breaks off every part that is detached (or flagged to break) at its bone box centre.
void fn_8018F9B4(u8* p) {
    PartState* w = (PartState*)(p + 0x152C);
    DamagePart* t = lbl_80332840;
    for (int i = 0; i < 17; i++) {
        if ((w->attached && w->hp > 0 && !w->state) || w->state == 2) {
            s32 bone = *(s32*)((u8*)t + 0x8);
            ValVec3* lo = &((CarBody*)p)->layout->bounds[bone].min;
            ValVec3* hi = lo + 1;
            ValVec3 c;
            c.x = lo->x + (lo + 1)->x;
            c.y = lo->y + hi->y;
            c.z = lo->z + (lo + 1)->z;
            c.x *= -0.5f;
            c.y *= -0.5f;
            c.z *= -0.5f;
            fn_801BF2A4(p, 1 << bone, &c, lo, lo + 1, *(s32*)((u8*)t + 0x34), (u8*)t + 0x38);
            w->state = 3;
            w->flag78 = 0;
        }
        w++;
        t++;
    }
}

}  // extern "C"

class ModuleTag;
void RegisterModuleTag(ModuleTag* tag);
ModuleTag lbl_8055E500;  // this unit's module tag

extern "C" {


// Unit start-up: register the module tag and fill the damage table's computed entries.
void fn_8018FACC(void) {
    RegisterModuleTag(&lbl_8055E500);
    lbl_80332840[0].axis.x = -0.6981317f;
    lbl_80332840[1].axis.x = 0.6981317f;
    lbl_80332840[8].axis.y = 1.2217305f;
    lbl_80332840[9].axis.y = -1.2217305f;
}

}  // extern "C"

// The race-start lead time (seconds). Defined here, read by racing (RacingOverlay::Draw): retail keeps it as the last
// entry of this unit's .sdata2 and racing loads it from here. Name unproven.
extern const f32 lbl_80562068 = 3.0f;
