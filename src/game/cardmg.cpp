// Car damage (0x8018BFA0-0x8018FB18): the detachable body parts ("right hood hinge", ...), wheel loss
// ("cardmg: wheel %d is already missing from car %d") and the part break-away timers. Written against
// raw field offsets; the large routines and the initialiser are still stubs. Names are guesses apart
// from the strings.
#include "game/common.h"
#include "macros.h"

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

// One entry of the damageable part table.
struct DamagePart {
    s32 kind;          // offset 0x0
    u8 data[0x3C];     // offset 0x4
};

extern DamagePart lbl_80332840[];

extern "C" {

void fn_8018BFA0(u8* p) {}  // stub (347)

void fn_8018C50C(u8* p) {}

void fn_8018C510(u8* p) {}

f32 PSVECDistance(const f32* a, const f32* b);
extern const f32 lbl_80561F70;  // large distance

// Index of the damage point nearest to pos (entry 0 is skipped).
int fn_8018C514(u8* p, const f32* pos) {
    f32 best = lbl_80561F70;
    int idx = 0;
    for (int i = 1; i < *(s32*)PTR(p, 0x26C); i++) {
        f32 d = PSVECDistance(pos, (f32*)(PTR(p, 0x26C) + i * 0x34 + 0x20));
        if (d < best) {
            best = d;
            idx = i;
        }
    }
    return idx;
}

void fn_8018C5A0(u8* p) {}  // stub (359)

void fn_8018CB3C(u8* p) {}  // stub (968)

struct EVec {
    f32 x, y, z;
};

struct DMat3 {
    f32 m[9];
};

// A part's transform (rotation + position).
struct DFrame {
    DMat3 rot;  // offset 0x0
    EVec pos;   // offset 0x24
};

// A detachable part's live state (0x80 bytes from car + 0x152C).
struct DmgSlot {
    s32 on;         // offset 0x0
    u8 pad4[0x14];  // offset 0x4
    DFrame cur;     // offset 0x18
    DFrame prev;    // offset 0x48
    u8 pad78[4];    // offset 0x78
    s32 state;      // offset 0x7C: 0 attached, 1 hanging, 2/4 flying, 3 gone
};

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
extern f32 lbl_80558DC4[];  // FLT_EPSILON

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
void fn_8018DA5C(u8* p, u8* m) {
    DmgCarView* car = (DmgCarView*)p;
    DmgPose* mdl = (DmgPose*)m;
    EVec* e = car->ease;
    for (int i = 0; i < car->nPoints; i++) {
        f32 x = car->ease[i].x * 0.8f;
        f32 y = car->ease[i].y * 0.8f;
        f32 z = car->ease[i].z * 0.8f;
        car->ease[i].x = x + car->goal[i].x * (1.0f - 0.8f);
        car->ease[i].y = y + car->goal[i].y * (1.0f - 0.8f);
        car->ease[i].z = z + car->goal[i].z * (1.0f - 0.8f);
        SetE(&mdl->pt[i], e + i);
    }
    if (U8(p, 0x26E0)) {
        f32 lean = F32(p, 0x26E8);
        f32 twist = 0.2f * F32(p, 0x26E4);
        f32 roll = 0.1f * F32(p, 0x26EC);
        lean = lean > -0.25f ? lean : -0.25f;
        for (int i = 1; i < car->nPoints; i++) {
            u8* pt = PTR(p, 0x26C) + i * 0x34;
            EVec v;
            v.x = 0.0f;
            v.y = 0.0f;
            v.z = 0.0f;
            f32 z = F32(pt, 0x28);
            f32 az = (f32)__fabs(z);
            v.z += -0.5f * z * roll;
            v.x += roll * (-0.3f * (F32(pt, 0x20) * (az - 0.7f)));
            v.y += roll * (-0.3f * (F32(pt, 0x24) * (az - 0.7f)));
            f32 w = DmgMax(0.0f, 3.0f - az);
            v.y += 0.5f * lean * (F32(pt, 0x24) * w);
            v.x += twist * DmgMax(0.0f, 3.0f - (f32)__fabs(F32(pt, 0x28)));
            mdl->pt[i] = v;
        }
    }
    U32(m, 0x29C) |= U32(p, 0x1E40);
    DmgSlot* s = (DmgSlot*)(p + 0x152C);
    DamagePart* tp = lbl_80332840;
    f32 t = lbl_8055E460;
    S32(m, 0xA78) = 0;
    for (int i = 0; i < 17; i++) {
        if (s->on && s->state != 0 && s->state != 1) {
            if (s->state == 3) {
                U32(m, 0x29C) &= ~(1 << *(s32*)(tp->data + 4));
            }
            if (s->state == 2 || s->state == 4) {
                DmgDraw* d = (DmgDraw*)(m + 0xA7C) + S32(m, 0xA78)++;
                if ((f32)__fabs(t - 1.0f) <= lbl_80558DC4[0]) {
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
                d->mask = 1 << *(s32*)(tp->data + 4);
            }
        }
        tp++;
        s++;
    }
}

// A deformable point of the car body (0x34 bytes, from the model at car + 0x26C).
struct DentVec {
    f32 x, y, z;
};

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

f32 PSVECMag(const f32* v);
void PSVECNormalize(const f32* v, f32* out);
f32 PSVECDotProduct(const f32* a, const f32* b);

// Dents point i of the body by `push`: moves its spring goal (pushing out only along the point's outward
// direction is cancelled), clamps it to the point's box, then passes the push on to linked points (not back to
// `from`), up to three links deep.
void fn_8018DE94(u8* p, int i, const DentVec* push, int from, int depth) {
    DentPoint* pt = (DentPoint*)PTR(p, 0x26C) + i;
    f32 k = 10.0f * (0.0000068539325f * pt->soft);
    f32 mag = PSVECMag(&push->x);
    DentVec d;
    d.x = push->x * k;
    d.y = push->y * k;
    d.z = push->z * k;
    d.y = d.y < 0.0f ? d.y : 0.0f;
    DentVec* g = (DentVec*)(p + 0x5FC) + pt->goal;
    g->x += d.x * 2.0f;
    g->y += d.y * 2.0f;
    g->z += d.z * 2.0f;
    if (PSVECDotProduct(&g->x, &pt->pos.x) > 0.0f) {
        DentVec n = pt->pos;
        PSVECNormalize(&n.x, &n.x);
        f32 out = -PSVECDotProduct(&g->x, &n.x);
        g->x += n.x * out;
        g->y += n.y * out;
        g->z += n.z * out;
    }
    g->x = DentClamp(g->x, pt->lo.x, pt->hi.x);
    g->y = DentClamp(g->y, pt->lo.y, pt->hi.y);
    g->z = DentClamp(g->z, pt->lo.z, pt->hi.z);
    if (depth <= 2) {
        DentVec* gi = (DentVec*)(p + 0x5FC) + i;
        DentVec at;
        at.x = pt->pos.x + gi->x;
        at.y = pt->pos.y + gi->y;
        at.z = pt->pos.z + gi->z;
        for (int n = 0; n < S32(PTR(p, 0x26C), 0x20F0); n++) {
            int other;
            DentLink* l = (DentLink*)PTR(PTR(p, 0x26C), 0x20EC) + n;
            other = l->a;
            if ((other == i && l->b != from) || (l->b == i && other != from)) {
                if (other == i) {
                    other = l->b;
                }
                DentPoint* o = (DentPoint*)PTR(p, 0x26C) + other;
                DentVec* go = (DentVec*)(p + 0x5FC) + other;
                DentVec op;
                op.x = o->pos.x + go->x;
                f32 py = o->pos.y + go->y;
                op.y = py;
                op.z = o->pos.z + go->z;
                DentVec dir;
                dir.x = op.x - at.x;
                dir.y = op.y - at.y;
                dir.z = op.z - at.z;
                f32 len = PSVECMag(&dir.x);
                DentVec f;
                if (len > 0.01f) {
                    f32 inv = 1.0f / len;
                    dir.x *= inv;
                    dir.y *= inv;
                    dir.z *= inv;
                    f32 t = -PSVECDotProduct(&dir.x, &push->x);
                    f.x = push->x + dir.x * t;
                    f.y = push->y + dir.y * t;
                    f.z = push->z + dir.z * t;
                    f32 c = l->across;
                    f.x *= c;
                    f.y *= c;
                    f.z *= c;
                    f32 s = l->along * PSVECDotProduct(&push->x, &dir.x);
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
    virtual void v08(void);
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
};

extern "C" {
GameMode* fn_801B83A0(void);
void fn_80161A38(int a, u8* car, int b);
void fn_801DFF1C(u8* car);
extern const f32 lbl_80561F74;
extern const f32 lbl_80561F8C;
extern f32 lbl_8055E3C8;

void fn_8018E330(u8* p) {
    fn_801B83A0()->ResetCar(p);
    fn_80161A38(0, PTR(p, 0x1EC), 0);
    fn_801DFF1C(PTR(p, 0x1EC));
    f32 zero = lbl_80561F74;
    f32 k = lbl_80561F8C;
    F32(p, 0x258C) = zero;
    F32(p, 0x2690) = k;
    F32(p, 0x25BC) = zero;
    F32(p, 0x26A0) = lbl_8055E3C8;
    S32(p, 0x26A4) = S32(((DmgModel*)PTR(p, 0x270))->Current(), 0x4);
    F32(p, 0x2588) = lbl_80561F74;
    F32(p, 0x25A0) = lbl_80561F74;
    U8(p, 0x259D) = 0;
    U8(p, 0x259C) = 0;
    S32(p, 0x2638) = 0;
    S32(p, 0x2644) = 0;
    F32(p, 0x2630) = lbl_80561F74;
    F32(p, 0x262C) = lbl_80561F74;
    F32(p, 0x2628) = lbl_80561F74;
}

void fn_8018E3EC(u8* p) {}  // stub (949)


}  // extern "C"

extern const f32 lbl_80562038;  // 255
extern const f32 lbl_80561F88;  // 1.0
extern const f32 lbl_8056203C;  // -0.508
extern const f32 lbl_80562040;  // 1.016
static inline f32 FMin(f32 a, f32 b) { return b < a ? b : a; }
static inline f32 FMax(f32 a, f32 b) { return a > b ? a : b; }

// A dent offset (-0.508..0.508) packed into a byte.
static inline u8 PackDent(f32 v) {
    f32 t = FMax(FMin((v - -0.508f) / 1.016f, 1.0f), lbl_80561F74);
    return lbl_80561F74 + t * lbl_80562038 + 0.5f;
}

#define DMG_MIN(a, b) ((a) < (b) ? (a) : (b))

// A packed dent byte back to its offset.
static inline f32 UnpackDent(u8 b) {
    f32 t = DMG_MIN(lbl_80561F88, (b - lbl_80561F74) / lbl_80562038);
    t = FMax(t, lbl_80561F74);
    return lbl_8056203C + t * lbl_80562040;
}

extern "C" {

// Restores the damage state saved by fn_8018F538.
void fn_8018F2C0(u8* p, u8* q) {
    f32 h = F32(q, 0x290);
    F32(p, 0x2690) = h;
    F32(p, 0x2694) = h;
    F32(p, 0x2698) = F32(q, 0x294);
    S32(p, 0x268C) = S32(q, 0x298);
    u8* o = q;
    u8* d = p;
    for (int i = 0; i < S32(p, 0x5F8); o += 4, i++, d += 0xC) {
        F32(d, 0x5FC) = UnpackDent(o[0]);
        F32(d, 0x600) = UnpackDent(o[1]);
        F32(d, 0x604) = UnpackDent(o[2]);
        F32(d, 0xD94) = F32(d, 0x5FC);
        F32(d, 0xD98) = F32(d, 0x600);
        F32(d, 0xD9C) = F32(d, 0x604);
    }
    S32(p, 0x15A8) = U8(q, 0x2A0);
    U8(p, 0x15A4) = 0;
    S32(p, 0x1628) = U8(q, 0x2A1);
    U8(p, 0x1624) = 0;
    S32(p, 0x16A8) = U8(q, 0x2A2);
    U8(p, 0x16A4) = 0;
    S32(p, 0x1728) = U8(q, 0x2A3);
    U8(p, 0x1724) = 0;
    S32(p, 0x17A8) = U8(q, 0x2A4);
    U8(p, 0x17A4) = 0;
    S32(p, 0x1828) = U8(q, 0x2A5);
    U8(p, 0x1824) = 0;
    S32(p, 0x18A8) = U8(q, 0x2A6);
    U8(p, 0x18A4) = 0;
    S32(p, 0x1928) = U8(q, 0x2A7);
    U8(p, 0x1924) = 0;
    S32(p, 0x19A8) = U8(q, 0x2A8);
    U8(p, 0x19A4) = 0;
    S32(p, 0x1A28) = U8(q, 0x2A9);
    U8(p, 0x1A24) = 0;
    S32(p, 0x1AA8) = U8(q, 0x2AA);
    U8(p, 0x1AA4) = 0;
    S32(p, 0x1B28) = U8(q, 0x2AB);
    U8(p, 0x1B24) = 0;
    S32(p, 0x1BA8) = U8(q, 0x2AC);
    U8(p, 0x1BA4) = 0;
    S32(p, 0x1C28) = U8(q, 0x2AD);
    U8(p, 0x1C24) = 0;
    S32(p, 0x1CA8) = U8(q, 0x2AE);
    U8(p, 0x1CA4) = 0;
    S32(p, 0x1D28) = U8(q, 0x2AF);
    U8(p, 0x1D24) = 0;
    S32(p, 0x1DA8) = U8(q, 0x2B0);
    U8(p, 0x1DA4) = 0;
    S32(p, 0x1E3C) = S32(q, 0x29C);
}

}  // extern "C"

extern "C" {

// Saves the damage state (dents packed to bytes, part states) into a snapshot.
void fn_8018F538(u8* p, u8* q) {
    F32(q, 0x290) = F32(p, 0x2690);
    F32(q, 0x294) = F32(p, 0x2698);
    S32(q, 0x298) = S32(p, 0x268C);
    u8* d = p;
    u8* o = q;
    for (int i = 0; i < S32(p, 0x5F8); d += 0xC, i++, o += 4) {
        o[0] = PackDent(F32(d, 0x5FC));
        o[1] = PackDent(F32(d, 0x600));
        o[2] = PackDent(F32(d, 0x604));
    }
    U8(q, 0x2A0) = S32(p, 0x15A8);
    U8(q, 0x2A1) = S32(p, 0x1628);
    U8(q, 0x2A2) = S32(p, 0x16A8);
    U8(q, 0x2A3) = S32(p, 0x1728);
    U8(q, 0x2A4) = S32(p, 0x17A8);
    U8(q, 0x2A5) = S32(p, 0x1828);
    U8(q, 0x2A6) = S32(p, 0x18A8);
    U8(q, 0x2A7) = S32(p, 0x1928);
    U8(q, 0x2A8) = S32(p, 0x19A8);
    U8(q, 0x2A9) = S32(p, 0x1A28);
    U8(q, 0x2AA) = S32(p, 0x1AA8);
    U8(q, 0x2AB) = S32(p, 0x1B28);
    U8(q, 0x2AC) = S32(p, 0x1BA8);
    U8(q, 0x2AD) = S32(p, 0x1C28);
    U8(q, 0x2AE) = S32(p, 0x1CA8);
    U8(q, 0x2AF) = S32(p, 0x1D28);
    U8(q, 0x2B0) = S32(p, 0x1DA8);
    S32(q, 0x29C) = S32(p, 0x1E3C);
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

struct CVec4 {
    CVec4(void) {}
    CVec4(const CVec4& o) {
        x = o.x;
        y = o.y;
        z = o.z;
        w = o.w;
    }
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

struct PVec3 {
    f32 x, y, z;
};

struct PFrame {
    PVec3 basis[3];
};

struct PColor {
    f32 r, g, b, a;
};

struct WFrame {
    PVec3 basis[3];
};

struct RVec3;
class RigidBody {
public:
    void VelAt(RVec3* out, const RVec3* at);
};

extern "C" {
void fn_802E30C8(const char* fmt, ...);
void fn_801BEB30(s32 car, u8** out);
void fn_801AFC44(s32 car, int wheel, WFrame* m, WVec3* vel, WVec3* imp);

// Knocks wheel i off the car: dims its channel in the car's cross-link colour, then launches it from
// the hub with the car's velocity plus its own spin.
void fn_8018F76C(u8* p, int i) {
    u8* w = p + i * 0x74;
    if (U32(w, 0x241C) & 8) {
        fn_802E30C8("cardmg: wheel %d is already missing from car %d", i, S32(p, 0x1EC));
    }
    u8* x;
    fn_801BEB30(S32(p, 0x1EC), &x);
    PColor pc = *(PColor*)(x + 0x70);
    CVec4 c = *(CVec4*)&pc;
    (&c.x)[i] *= 0.15f;
    *(CVec4*)(x + 0x70) = c;
    WFrame m = *(WFrame*)(p + 0x3C);
    WVec3 hub = *(WVec3*)(w + 0x23D0);
    hub.x += m.basis[1].x * 0.3f;
    hub.y += m.basis[1].y * 0.3f;
    hub.z += m.basis[1].z * 0.3f;
    WVec3 vel;
    ((RigidBody*)p)->VelAt((RVec3*)&vel, (RVec3*)&hub);
    f32 spin = F32(w, 0x23AC);
    WVec3 imp;
    imp.x = m.basis[0].x * spin + F32(p, 0x124);
    imp.y = m.basis[0].y * spin + F32(p, 0x128);
    imp.z = m.basis[0].z * spin + F32(p, 0x12C);
    fn_801AFC44(S32(p, 0x1EC), i, &m, &vel, &imp);
    U32(w, 0x241C) |= 8;
}

struct DVec {
    f32 x, y, z;
};

// Live state of one damageable part (17 of them at car + 0x152C).
struct PartState {
    u32 attached;    // offset 0x0
    s32 hp;          // offset 0x4
    u8 pad8[0x70];   // offset 0x8
    u8 flag78;       // offset 0x78
    u8 pad79[0x3];
    s32 state;       // offset 0x7C
};

struct BoneBox {
    DVec lo;  // offset 0x0
    DVec hi;  // offset 0xC
};

// Car model (partial).
struct CarModel {
    u8 pad0[0x2138];
    BoneBox boxes[1];  // offset 0x2138, one per bone
};

void fn_801BF2A4(u8* car, u32 mask, DVec* center, DVec* lo, DVec* hi, s32 kind, u8* fx);
extern const f32 lbl_80561FE4;  // -0.5

// Breaks off every part that is detached (or flagged to break) at its bone box centre.
void fn_8018F9B4(u8* p) {
    PartState* w = (PartState*)(p + 0x152C);
    DamagePart* t = lbl_80332840;
    for (int i = 0; i < 17; i++) {
        if ((w->attached && w->hp > 0 && !w->state) || w->state == 2) {
            s32 bone = *(s32*)((u8*)t + 0x8);
            DVec* lo = &((CarModel*)PTR(p, 0x26C))->boxes[bone].lo;
            DVec* hi = lo + 1;
            DVec c;
            c.x = lo->x + (lo + 1)->x;
            c.y = lo->y + hi->y;
            c.z = lo->z + (lo + 1)->z;
            c.x *= lbl_80561FE4;
            c.y *= lbl_80561FE4;
            c.z *= lbl_80561FE4;
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
extern ModuleTag lbl_8055E500;  // this unit's module tag

extern "C" {

extern const f32 lbl_80562058;
extern const f32 lbl_8056205C;
extern const f32 lbl_80562060;
extern const f32 lbl_80562064;

// Unit start-up: register the module tag and fill the damage table's computed entries.
void fn_8018FACC(void) {
    RegisterModuleTag(&lbl_8055E500);
    *(f32*)(lbl_80332840[0].data + 0x8) = lbl_80562058;
    *(f32*)(lbl_80332840[1].data + 0x8) = lbl_8056205C;
    *(f32*)(lbl_80332840[8].data + 0xC) = lbl_80562060;
    *(f32*)(lbl_80332840[9].data + 0xC) = lbl_80562064;
}

}  // extern "C"
