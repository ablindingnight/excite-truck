// Track obstacles (0x801A9660-0x801ADB44): the obstacle factory ("fencepst", "obst", "DynamicObstacle"),
// static and dynamic obstacle rigid bodies, the extents/plane/polyvol/pmshvol readers and the
// fence-pole binding ("fencepole: binding to %p"). Written against raw field offsets; the large
// routines are still stubs. Names are guesses apart from the strings.
#include "game/common.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

class VO {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(const char* a, const char* b);
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
    virtual void v48(const f32* push, const f32* at, int force, int flags);
};

// A 12-word body pose.
struct Pose12 {
    u32 w[12];
};

extern s32 lbl_8055E3C4;

extern "C" {
f32 PSVECDotProduct(const f32* a, const f32* b);
void PSVECCrossProduct(const f32* a, const f32* b, f32* out);
void fn_801ABC28(u8* p);
}

struct ObVec3 {
    f32 x, y, z;
};

// A visitor called for each obstacle in a linked chain.
class VVisit {
public:
    virtual void Visit(u8* o);
};

struct CVec3 {
    f32 x, y, z;
};

class CollVolume {
public:
    void Update(void);
    void Xform(CVec3* out, const CVec3* in);
};

struct RVec3;

class RigidBody {
public:
    void Reset(void);
    void Update(void);
    void Flush(RVec3* a, RVec3* b);
    void VelAt(RVec3* out, const RVec3* at);
    void AddForceAt(const RVec3* f, const RVec3* at);
    void OnHit(const RVec3* v, const RVec3* at, int force, void* extra);
};

// A handle whose constructor marks it unused.
struct ObHandle {
    s32 v;
    ObHandle(void) { v = 0xFFFF0000; }
};

extern "C" {
u8* __ct__9RigidBodyFv(u8* p);
void __dt__10CollVolumeFv(void* p, int del);
void vf20__10CollVolumeFv(u8* p);
s32 fn_801C1BA4(int nPart, int nStick, int nAnchor);  // ppsys.cpp: new PSysObj
void fn_801D08C0(u8* p);
void fn_801D0A24(u8* p, int a);
void fn_801A9AB0(u8* p, u8* q);
void fn_801C1C7C(u8* p);
void fn_802E30C8(const char* fmt, ...);
f32 PSVECSquareDistance(const u8* a, const u8* b);
void fn_801AC240(u8* p);
void fn_801ABFF8(u8* p, VVisit* f);
void fn_801ACF94(void);
void fn_801AD8C8(u8* p, const Pose12* pose, const ObVec3* lo, const ObVec3* hi);
void fn_801AD99C(u8* p, f32* v, f32* off);
void fn_801D09A8(u8* p, s32 id);
}

void operator delete(void* p);
void* operator new(unsigned long n, const char* tag);

extern void* lbl_8033585C[];
extern void* lbl_80335808[];
extern u8* lbl_80435628[];

// Cars, player cars, AI cars and network cars all carry a car tag.
static inline int IsCar(u8* o) {
    u32 t = U32(o, 0x10);
    return t == 'CAR ' || t == 'PCAR' || t == 'ACAR' || t == 'NCAR';
}
extern void* lbl_80335744[];
extern void* lbl_80335940[];
extern f32 lbl_8055E3C8;

ModuleTag lbl_8055E5D8;
ObHandle lbl_8055E5E0;
s32 lbl_8055E5E4;
s32 lbl_8055E5E8;
s32 lbl_8055E5EC;
s32 lbl_8055E5F0;

extern "C" {

u8* fn_801792F8(void);
void fn_802E317C(const char* fmt, ...);
u8* fn_801A9700(u8* p);
u8* fn_801AC26C(void);
u8* fn_801AC330(u8* p);

// Creates the obstacle object for the current track's obstacle type.
u8* fn_801A9660(void) {
    u8* o;
    switch (S32(fn_801792F8(), 0x50)) {
    case 12:
        o = (u8*)operator new(0x258, "fencepst");
        if (o) {
            o = fn_801AC330(o);
        }
        return o;
    case 17:
    case 18:
    case 21:
        return fn_801AC26C();
    case 6:
    case 9:
    case 14:
    case 15:
    case 19:
    case 22:
        fn_802E317C("obstacle: tried to create an Obstacle intended only for the SOL");
        return 0;
    default:
        o = (u8*)operator new(0x23C, "obst");
        if (o) {
            o = fn_801A9700(o);
        }
        return o;
    }
}

u8* fn_801A9700(u8* p) {
    __ct__9RigidBodyFv(p);
    *(void**)p = lbl_8033585C;
    return p;
}

u8* fn_801A973C(u8* p, int del) {
    if (p) {
        if (p) {
            __dt__10CollVolumeFv(p, 0);
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

}  // extern "C"

extern const f32 lbl_8056247C;  // 0.0
extern const f32 lbl_80562478;  // 0.5

// A contact corner (corners.cpp), kept in a body's list at 0xD0.
struct BodyCorner {
    BodyCorner* next;  // offset 0x0
    ObVec3 pos;        // offset 0x4
    f32 spring;        // offset 0x10
    f32 friction;      // offset 0x14
    f32 damping;       // offset 0x18
    ObVec3 vel;        // offset 0x1C
};

extern "C" BodyCorner* fn_801AE180(void);  // new corner

static inline void AddCorner(u8* body, f32 x, f32 y, f32 z, f32 spring) {
    ObVec3 pos;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    BodyCorner* c = fn_801AE180();
    c->pos = pos;
    c->spring = spring;
    c->friction = lbl_80562478;
    c->vel.x = lbl_8056247C;
    c->vel.y = lbl_8056247C;
    c->vel.z = lbl_8056247C;
    c->damping = lbl_8056247C;
    c->next = *(BodyCorner**)(body + 0xD0);
    *(BodyCorner**)(body + 0xD0) = c;
}

extern "C" {

// Puts a contact corner at each of the eight corners of a box with half extents (x, y, z).
void fn_801A9798(u8* p, f32 x, f32 y, f32 z, f32 spring) {
    AddCorner(p, -x, y, -z, spring);
    AddCorner(p, x, y, -z, spring);
    AddCorner(p, -x, y, z, spring);
    AddCorner(p, x, y, z, spring);
    AddCorner(p, -x, -y, -z, spring);
    AddCorner(p, x, -y, -z, spring);
    AddCorner(p, -x, -y, z, spring);
    AddCorner(p, x, -y, z, spring);
}

void fn_801A9AB0(u8* p, u8* q) {}  // stub (2089)

extern const f32 lbl_8056247C;  // 0.0
extern const f32 lbl_805624AC;  // 1.0
extern const f32 lbl_805624A4;  // -1.0
extern const f32 lbl_805624D8;  // 0.01
extern const f32 lbl_80562478;  // 0.5

void fn_801ABB54(u8* p) {
    f32 d = PSVECDotProduct((f32*)(p + 0x124), (f32*)(p + 0x54));
    f32 imp = lbl_8056247C;
    f32 ad = (f32)__fabs(d);
    f32 lim = F32(p, 0x238);
    if (ad > lim) {
        lim = lim < ad ? lim : ad;
        imp = -(d > lbl_8056247C ? lbl_805624AC : lbl_805624A4) * lim;
    } else if (ad > lbl_805624D8) {
        imp = -d * lbl_80562478;
    }
    F32(p, 0x124) = F32(p, 0x124) + F32(p, 0x54) * imp;
    F32(p, 0x128) = F32(p, 0x128) + F32(p, 0x58) * imp;
    F32(p, 0x12C) = F32(p, 0x12C) + F32(p, 0x5C) * imp;
}

}  // extern "C"

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

// out = m * v (m: 3x3 rows at 0, 3, 6), safe when out aliases v.
static inline void ObRotate(ObVec3* out, const f32* m, const ObVec3* v) {
    if (out == v) {
        f32 x = Dot3(v->x, m[0], v->y, m[3], v->z, m[6]);
        f32 y = Dot3(v->x, m[1], v->y, m[4], v->z, m[7]);
        f32 z = Dot3(v->x, m[2], v->y, m[5], v->z, m[8]);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(v->x, m[0], v->y, m[3], v->z, m[6]);
        out->y = Dot3(v->x, m[1], v->y, m[4], v->z, m[7]);
        out->z = Dot3(v->x, m[2], v->y, m[5], v->z, m[8]);
    }
}

// Message sent when a solid obstacle wants its hit point reported.
struct ObHitMsg {
    ObVec3 pos;  // offset 0x0
    u8* obj;     // offset 0xC
    u8 flag;     // offset 0x10
};

// One frame of a sleeping/settled rigid obstacle (Frame copy at 0x1EC, rest timer at 0x21C).
class ObSolid {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void Fell(void);  // 0x10
};

extern "C" {
void fn_801AC254(u8* p);
void fn_801796CC(int kind, void* msg, int size);

void fn_801ABC28(u8* p) {
    if (U8(p, 0x222)) {
        ObVec3 lin;
        ObVec3 ang;
        U8(p, 0x1E9) = 0;
        ((RigidBody*)p)->Flush((RVec3*)&lin, (RVec3*)&ang);
        return;
    }
    if (PSVECDotProduct((f32*)(p + 0x118), (f32*)(p + 0x118)) > 0.01f ||
        PSVECDotProduct((f32*)(p + 0x124), (f32*)(p + 0x124)) > 0.01f) {
        if (F32(p, 0x64) > -500.0f) {
            fn_801AC240(p);
        }
    }
    if (U8(p, 0x220)) {
        if (lbl_8055E3C4 == 1 || lbl_8055E3C8 > lbl_805624AC + F32(p, 0x21C)) {
            fn_801AC254(p);
            F32(p, 0x124) = lbl_8056247C;
            F32(p, 0x128) = lbl_8056247C;
            F32(p, 0x12C) = lbl_8056247C;
            F32(p, 0x118) = lbl_8056247C;
            F32(p, 0x11C) = lbl_8056247C;
            F32(p, 0x120) = lbl_8056247C;
        } else if (!U8(p, 0x221)) {
            ((RigidBody*)p)->Update();
            F32(p, 0x118) *= 0.99983317f;
            F32(p, 0x11C) *= 0.99983317f;
            F32(p, 0x120) *= 0.99983317f;
            F32(p, 0x124) *= 0.99983317f;
            F32(p, 0x128) *= 0.99983317f;
            F32(p, 0x12C) *= 0.99983317f;
            if (F32(p, 0x238) > lbl_8056247C) {
                fn_801ABB54(p);
            }
            int up = 0;
            if (U8(p, 0x234) && F32(p, 0x4C) > lbl_80562478) {
                up = 1;
            }
            U8(p, 0x234) = up;
        } else {
            ((RigidBody*)p)->Update();
            F32(p, 0x124) = lbl_8056247C;
            F32(p, 0x128) = lbl_8056247C;
            F32(p, 0x12C) = lbl_8056247C;
            F32(p, 0x118) = lbl_8056247C;
            F32(p, 0x11C) = lbl_8056247C;
            F32(p, 0x120) = lbl_8056247C;
            *(Pose12*)(p + 0x3C) = *(Pose12*)(p + 0x1EC);
            ((CollVolume*)p)->Update();
        }
        if (F32(p, 0x64) < -500.0f) {
            ((ObSolid*)p)->Fell();
        }
    }
    if (U8(p, 0x235)) {
        ObHitMsg m;
        ObRotate(&m.pos, (f32*)(p + 0x70), (ObVec3*)(p + 0x228));
        m.pos.x += F32(p, 0x94);
        m.pos.y += F32(p, 0x98);
        m.pos.z += F32(p, 0x9C);
        m.obj = p;
        m.flag = U8(p, 0x234);
        fn_801796CC(9, &m, sizeof(ObHitMsg));
    }
    S32(p, 0x1E4) = 0;
}

void fn_801ABFF4(u8* p) { vf20__10CollVolumeFv(p); }

void fn_801ABFF8(u8* p, VVisit* f) {
    u8* o = PTR(p, 0x224);
    if (o && U8(p, 0x223) && lbl_8055E3C4 > 1) {
        while (o != p) {
            U8(o, 0x223) = 0;
            f->Visit(o);
            o = PTR(o, 0x224);
        }
        U8(p, 0x223) = 0;
    }
    fn_801AC240(p);
}

}  // extern "C"

// Visitor handed to the linked obstacles when this one is hit hard.
struct HitVisit {
    void* vt;     // offset 0x0
    f32 v[3];     // offset 0x4
    f32 at[3];    // offset 0x10
    s32 force;    // offset 0x1C
    u8* extra;    // offset 0x20
};

extern void* lbl_803357F0[];
extern const f32 lbl_805624E8;  // 16/9

extern "C" {

void fn_801AC0A0(u8* p, const f32* v, const f32* at, int force, u8* extra) {
    if (force >= 100 && !U8(p, 0x220)) {
        f32 r = F32(p, 0xD4);
        f32 k = lbl_805624E8 * r;
        f32 k2 = k * k;
        if (PSVECDotProduct(v, v) > k2) {
            HitVisit h;
            h.vt = lbl_803357F0;
            h.v[0] = v[0];
            h.v[1] = v[1];
            h.v[2] = v[2];
            h.at[0] = at[0];
            h.at[1] = at[1];
            h.at[2] = at[2];
            h.force = force;
            h.extra = extra;
            fn_801ABFF8(p, (VVisit*)&h);
        }
    }
    ((RigidBody*)p)->OnHit((const RVec3*)v, (const RVec3*)at, force, 0);
}

void fn_801AC184(u8* p) {
    fn_801AC240(p);
    *(Pose12*)(p + 0x3C) = *(Pose12*)(p + 0x1EC);
    ((CollVolume*)p)->Update();
    ((RigidBody*)p)->Reset();
    U8(p, 0x223) = 1;
    U8(p, 0x234) = U8(p, 0x235);
}

void fn_801AC240(u8* p) {
    U8(p, 0x220) = 1;
    F32(p, 0x21C) = lbl_8055E3C8;
}

void fn_801AC254(u8* p) {
    if (U8(p, 0x220)) {
        U8(p, 0x220) = 0;
    }
}

u8* fn_801AC26C(void) {
    u8* o = (u8*)operator new(0x244, "DynamicObstacle");
    if (o) {
        fn_801A9700(o);
        *(void**)o = lbl_80335744;
        *(void**)(o + 0x23C) = (u8*)lbl_80335744 + 0x54;
    }
    return o;
}

u8* fn_801AC2C8(u8* p, int del) {
    if (p) {
        lbl_8055E5E4 = 0;
        if (p) {
            if (p) {
                __dt__10CollVolumeFv(p, 0);
            }
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

u8* fn_801AC330(u8* p) {
    fn_801A9700(p);
    *(void**)p = lbl_80335808;
    if (!lbl_8055E5EC) {
        lbl_8055E5F0 = 0;
    }
    S32(p, 0x250) = lbl_8055E5EC++;
    lbl_80435628[S32(p, 0x250)] = p;
    S32(p, 0x254) = -1;
    return p;
}

u8* fn_801AC3AC(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_80335808;
        if (--lbl_8055E5EC == 0 && lbl_8055E5F0) {
            fn_801C1C7C((u8*)lbl_8055E5F0);
            lbl_8055E5F0 = 0;
        }
        if (p) {
            lbl_8055E5E4 = 0;
            if (p) {
                if (p) {
                    __dt__10CollVolumeFv(p, 0);
                }
            }
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

void fn_801AC454(u8* p, u8* q) {
    U32(p, 0x23C) = 0;
    U32(p, 0x240) = 0;
    fn_801A9AB0(p, q);
    s32 idx = S32(p, 0x250);
    F32(p, 0x248) = F32(q, 0x5C);
    F32(p, 0x244) = 0.0f;
    F32(p, 0x24C) = 0.0f;
    U32(p, 0x23C) = 0;
    if (idx > 0) {
        u8* prev = lbl_80435628[idx - 1];
        if (PSVECSquareDistance(prev + 0x60, p + 0x60) < 16.0f) {
            PTR(p, 0x23C) = prev;
            fn_802E30C8("fencepole: binding to %p", prev);
        }
    }
}

void fn_801AC508(void) {
    s32 n = lbl_8055E5EC;
    if (n > 0) {
        s32 t = n * 5;
        lbl_8055E5F0 = fn_801C1BA4(t, t - 1, n);
    }
}

void fn_801AC544(u8* p) {}  // stub (321)

void fn_801ACA48(u8* p) {
    fn_801ABC28(p);
    if (U8(p, 0x220)) {
        f32 b = 0.9f * F32(p, 0x128);
        F32(p, 0x118) = 0.0f;
        F32(p, 0x11C) = 0.0f;
        F32(p, 0x120) = 0.0f;
        F32(p, 0x124) = (0.9f * F32(p, 0x124));
        F32(p, 0x128) = b;
        F32(p, 0x12C) = 0.9f * F32(p, 0x12C);
        ObVec3 c;
        PSVECCrossProduct((f32*)(p + 0x48), (f32*)(p + 0x1F8), &c.x);
        if (F32(p, 0x4C) < 0.92f) {
            F32(p, 0x124) = F32(p, 0x124) + 0.1f * c.x;
            F32(p, 0x128) = F32(p, 0x128) + 0.1f * c.y;
            F32(p, 0x12C) = F32(p, 0x12C) + 0.1f * c.z;
        }
        F32(p, 0x60) = F32(p, 0x210);
        F32(p, 0x64) = F32(p, 0x214);
        F32(p, 0x68) = F32(p, 0x218);
    }
}

}  // extern "C"

static inline f32 ObLerp(f32 b, f32 a, f32 t) { return a + t * (b - a); }

// A pose as stored for drawing (rotation rows + position).
struct ObRot {
    f32 m[9];
};

struct ObPose {
    ObRot rot;   // offset 0x0
    ObVec3 pos;  // offset 0x24
};

extern f32 lbl_8055E460;    // draw interpolation factor (1 = current frame)
extern f32 lbl_80558DC4[];  // FLT_EPSILON

extern "C" {

// Writes this obstacle's draw state into the frame buffer: its pose (blended from the previous one when the
// draw falls between physics steps) and part id; links also work out both anchor points.
void fn_801ACB30(u8* p, u8* buf) {
    f32 f = lbl_8055E460;
    u8* out = buf + S32(p, 0x8);
    if ((f32)__fabs(f - lbl_805624AC) <= lbl_80558DC4[0]) {
        *(ObPose*)out = *(ObPose*)(p + 0x70);
    } else {
        ObPose t;
        f32* prev = (f32*)(p + 0xA0);
        f32* cur = (f32*)(p + 0x70);
        f32* d = (f32*)&t;
        for (int i = 0; i < 12; i++) {
            d[i] = ObLerp(cur[i], prev[i], f);
        }
        *(ObPose*)out = t;
    }
    *(s16*)(out + 0x30) = *(s16*)(p + 0x6C);
    if (PTR(p, 0x23C)) {
        ObVec3 a;
        ObRotate(&a, (f32*)(p + 0x3C), (ObVec3*)(p + 0x244));
        a.x += F32(p, 0x60);
        a.y += F32(p, 0x64);
        a.z += F32(p, 0x68);
        u8* o = PTR(p, 0x23C);
        ObVec3 b;
        ObRotate(&b, (f32*)(o + 0x3C), (ObVec3*)(o + 0x244));
        b.x += F32(o, 0x60);
        b.y += F32(o, 0x64);
        b.z += F32(o, 0x68);
    }
}

// Fence span update sent to the renderer: the posts between two linked fence posts.
struct FenceMsg {
    s32 kind;         // offset 0x0
    s32 post;         // offset 0x4
    s32 count;        // offset 0x8
    f32 pts[6][3];    // offset 0xC
};

void fn_801C2694(s32 rope, f32 (*out)[3], s32* n, f32* d, f32* c, f32* b, f32* a);
void fn_801796CC(int kind, void* msg, int size);
extern const f32 lbl_8056247C;  // 0.0
extern const f32 lbl_805624AC;  // 1.0
extern const f32 lbl_80562510;  // -10000.0

// Samples the fence rope and sends each linked post pair its span of points.
void fn_801ACF94(void) {
    if (lbl_8055E5F0) {
        f32 out[0x13C][3];
        FenceMsg m;
        f32 d[3];
        f32 c[3];
        f32 b[3];
        f32 a[3];
        s32 n = 0x13C;
        a[0] = lbl_8056247C;
        a[1] = lbl_8056247C;
        a[2] = lbl_8056247C;
        b[0] = lbl_8056247C;
        b[1] = lbl_8056247C;
        b[2] = lbl_8056247C;
        c[0] = lbl_8056247C;
        c[1] = lbl_805624AC;
        c[2] = lbl_8056247C;
        d[0] = lbl_8056247C;
        d[1] = lbl_80562510;
        d[2] = lbl_8056247C;
        fn_801C2694(lbl_8055E5F0, out, &n, d, c, b, a);
        for (int i = 0; i < lbl_8055E5EC; i++) {
            u8* next;
            u8* post;
            post = lbl_80435628[i];
            next = PTR(post, 0x23C);
            if (next) {
                int first = S32(next, 0x254);
                int last = S32(post, 0x254);
                m.kind = 0;
                m.post = i;
                m.count = last - first + 1;
                for (int k = 0; k < m.count; k++) {
                    m.pts[k][0] = out[first + k][0];
                    m.pts[k][1] = out[first + k][1];
                    m.pts[k][2] = out[first + k][2];
                }
                fn_801796CC(6, &m, 0x54);
            }
        }
    }
}

void fn_801AD0E0(u8* p, const f32* a, s32 id, const f32* b, f32 x, f32 y) {
    f32 t = 0.02f * x;
    S32(p, 0xC) = 0;
    F32(p, 0x28) = 0.0f;
    F32(p, 0x2C) = 0.0f;
    F32(p, 0x0) = a[0];
    F32(p, 0x4) = a[1];
    F32(p, 0x8) = a[2];
    S32(p, 0xC) = id;
    F32(p, 0x10) = b[0];
    F32(p, 0x14) = b[1];
    F32(p, 0x18) = b[2];
    F32(p, 0x1C) = x;
    F32(p, 0x24) = y;
    F32(p, 0x20) = 0.1f * t;
}

// Links this joint between a point on our volume and the same point in `body`'s local frame.
void fn_801AD148(u8* p, const CVec3* a, u8* body, f32 x, f32 y) {
    CVec3 w;
    CVec3 l;
    ((CollVolume*)PTR(p, 0x34))->Xform(&w, a);
    if (body) {
        l.x = w.x - F32(body, 0x60);
        l.y = w.y - F32(body, 0x64);
        l.z = w.z - F32(body, 0x68);
        f32* m = (f32*)(body + 0x3C);
        f32 lx = Dot3(l.x, m[0], l.y, m[1], l.z, m[2]);
        f32 ly = Dot3(l.x, m[3], l.y, m[4], l.z, m[5]);
        f32 lz = Dot3(l.x, m[6], l.y, m[7], l.z, m[8]);
        l.x = lx;
        l.y = ly;
        l.z = lz;
    } else {
        l = w;
    }
    fn_801AD0E0(p, (const f32*)&l, (s32)body, (const f32*)a, x, y);
}

f32 PSVECMag(const CVec3* v);

// Links this joint between a point on our volume and one on `other` (or a world point).
void fn_801AD28C(u8* p, const CVec3* a, CollVolume* other, const CVec3* b, f32 x, f32 y) {
    CVec3 wa;
    CVec3 wb;
    CVec3 d;
    ((CollVolume*)PTR(p, 0x34))->Xform(&wa, a);
    if (other) {
        other->Xform(&wb, b);
    } else {
        wb = *b;
    }
    d.x = wb.x - wa.x;
    d.y = wb.y - wa.y;
    d.z = wb.z - wa.z;
    F32(p, 0x28) = PSVECMag(&d);
    f32 len = F32(p, 0x28);
    fn_801AD0E0(p, (const f32*)b, (s32)other, (const f32*)a, x, y);
    F32(p, 0x28) = len;
}

extern const f32 lbl_80562518;  // 0.001
double sqrt(double x);

// Applies the joint's spring (beyond the rest length) and damping between the two attachment points.
void fn_801AD394(u8* p) {
    CVec3 wa;
    CVec3 wb;
    CVec3 d;
    ((CollVolume*)PTR(p, 0x34))->Xform(&wa, (const CVec3*)(p + 0x10));
    if (PTR(p, 0xC)) {
        ((CollVolume*)PTR(p, 0xC))->Xform(&wb, (const CVec3*)p);
    } else {
        wb = *(const CVec3*)p;
    }
    d.x = wb.x - wa.x;
    d.y = wb.y - wa.y;
    d.z = wb.z - wa.z;
    f32 len = PSVECMag(&d);
    const f32& dx = d.x;
    f32 dz = d.z;
    f32 hxx = dx * dx;
    f32 hzz = dz * dz;
    F32(p, 0x30) = (f32)sqrt(hxx + hzz);
    f32 stretch = len - F32(p, 0x28);
    F32(p, 0x2C) = stretch;
    if (len > lbl_80562518 && len < F32(p, 0x28) + F32(p, 0x24)) {
        CVec3 dir;
        CVec3 force;
        CVec3 va;
        CVec3 vb;
        CVec3 dv;
        f32 inv = lbl_805624AC / len;
        f32 zero = lbl_8056247C;
        force.x = zero;
        force.y = zero;
        force.z = zero;
        dir.x = d.x * inv;
        dir.y = d.y * inv;
        dir.z = d.z * inv;
        ((RigidBody*)PTR(p, 0x34))->VelAt((RVec3*)&va, (const RVec3*)&wa);
        if (PTR(p, 0xC)) {
            ((RigidBody*)PTR(p, 0xC))->VelAt((RVec3*)&vb, (const RVec3*)&wb);
        } else {
            vb.x = lbl_8056247C;
            vb.y = lbl_8056247C;
            vb.z = lbl_8056247C;
        }
        dv.x = vb.x - va.x;
        dv.y = vb.y - va.y;
        dv.z = vb.z - va.z;
        f32 c = F32(p, 0x20);
        force.x = dv.x * c;
        force.y = dv.y * c;
        force.z = dv.z * c;
        if (len > F32(p, 0x28)) {
            f32 k = F32(p, 0x1C) * (len - F32(p, 0x28));
            force.x = force.x + dir.x * k;
            force.y = force.y + dir.y * k;
            force.z = force.z + dir.z * k;
        }
        ((RigidBody*)PTR(p, 0x34))->AddForceAt((const RVec3*)&force, (const RVec3*)&wa);
        if (PTR(p, 0xC)) {
            force.x = force.x * lbl_805624A4;
            force.y = force.y * lbl_805624A4;
            force.z = force.z * lbl_805624A4;
            ((RigidBody*)PTR(p, 0xC))->AddForceAt((const RVec3*)&force, (const RVec3*)&wa);
        }
    }
}

void fn_801AD5EC(void) { fn_801AC508(); }

void fn_801AD5F0(u8* p) { fn_801ACF94(); }

}  // extern "C"

class ObstacleSink {
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
    virtual void Hit(const f32* push, const f32* at, u32 a, u32 b);  // 0x48
};

extern "C" {
extern const f32 lbl_805624F4;  // 5.0
extern const f32 lbl_8056249C;  // 0.8

void fn_801AD5F4(u8* p, ObstacleSink* q) {
    f32 push[3];
    push[0] = F32(p, 0x4) * lbl_805624F4;
    push[1] = F32(p, 0x8) * lbl_805624F4;
    push[2] = F32(p, 0xC) * lbl_805624F4;
    f32 at[3];
    f32 c2 = F32(p, 0x18) + lbl_8056249C * (F32((u8*)q, 0x68) - F32(p, 0x18));
    f32 c1 = F32(p, 0x14) + lbl_8056249C * (F32((u8*)q, 0x64) - F32(p, 0x14));
    f32 c0 = F32(p, 0x10) + lbl_8056249C * (F32((u8*)q, 0x60) - F32(p, 0x10));
    at[0] = c0;
    at[1] = c1;
    at[2] = c2;

    q->Hit(push, at, U32(p, 0x1C), U32(p, 0x20));
}

void fn_801D0934(u8* p);

// DynamicObstacle destructor; the secondary base at 0x23C is torn down first.
u8* fn_801AD6B0(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_80335744;
        *(void**)(p + 0x23C) = (u8*)lbl_80335744 + 0x54;
        u8* sec = p;
        if (p) {
            sec = p + 0x23C;
        }
        fn_801D0934(sec);
        S32(p, 0x240) = 0;
        if (p) {
            lbl_8055E5E4 = 0;
            if (p) {
                if (p) {
                    __dt__10CollVolumeFv(p, 0);
                }
            }
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

void fn_801AD750(u8* p, u8* q) {
    fn_801A9AB0(p, q);
    U32(p, 0x240) = U32(q, 0x80);
    u8* b = p;
    if (b) {
        b += 0x23C;
    }
    fn_801D08C0(b);
}

void fn_801AD7A4(u8* p, int a, int b, int c) {
    if (c < 8) {
        if (p) {
            p += 0x23C;
        }
        fn_801D0A24(p, b);
    }
}

void fn_801AD7C4(u8* p, const f32* v, const f32* at, int force, u8* car) {
    fn_801AC0A0(p, v, at, force, car);
    if (car && IsCar(car)) {
        s32 id = S32(car, 0x1EC);
        if (p) {
            p += 0x23C;
        }
        fn_801D09A8(p, id);
    }
}

u8 fn_801AD860(u8* p) { return U8(p, 0x220); }

u32 fn_801AD868(u8* p) { return 0; }

void fn_801AD870(u8* p, int a, int b, s32* c, s32* d) {
    *d = 0;
    *c = 0;
}

u32 fn_801AD880(u8* p) { return 0; }

void fn_801AD888(u8* p) {
    ((VO*)p)->v10();
    fn_801AC254(p);
}

void fn_801AD8C8(u8* p, const Pose12* pose, const ObVec3* lo, const ObVec3* hi) {
    *(Pose12*)(p + 0x3C) = *pose;
    ((CollVolume*)p)->Update();
    F32(p, 0x118) = lo->x;
    F32(p, 0x11C) = lo->y;
    F32(p, 0x120) = lo->z;
    F32(p, 0x124) = hi->x;
    F32(p, 0x128) = hi->y;
    F32(p, 0x12C) = hi->z;
    fn_801AC240(p);
}

void fn_801AD99C(u8* p, f32* v, f32* off) {
    f32 push[3];
    f32 at[3];
    f32 s = F32(p, 0xD4) / 0.01668335f;
    push[0] = v[0] * s;
    push[1] = v[1] * s;
    push[2] = v[2] * s;
    at[0] = F32(p, 0x60) + off[0];
    at[1] = F32(p, 0x64) + off[1];
    at[2] = F32(p, 0x68) + off[2];
    ((VO*)p)->v48(push, at, 100, 0);
}

void fn_801ADA38(u8* p, int on) {
    if (on) {
        void* f = lbl_80335940;
        fn_801ABFF8(p, (VVisit*)&f);
    } else {
        U8(p, 0x220) = 0;
    }
}

u8 fn_801ADA7C(u8* p) { return U8(p, 0x220); }

s32 fn_801ADA84(u8* p) { return *(s16*)(p + 0x6C); }

u8* fn_801ADA8C(u8* p) { return p + 0x118; }

u8* fn_801ADA94(u8* p) { return p + 0x60; }

void fn_801ADA9C(u8* p) { ((VO*)PTR(p, 0x240))->v18("dyno", "unknown"); }

u32 fn_801ADAB8(u8* p) { return U32(p, 0x240); }

void fn_801ADAC0(u8* self, u8* p) { fn_801AC240(p); }



void fn_801ADAF4(u8* p) { fn_801AD888(p - 0x23C); }

void fn_801ADAFC(u8* p, const Pose12* pose, const ObVec3* lo, const ObVec3* hi) { fn_801AD8C8(p - 0x23C, pose, lo, hi); }

void fn_801ADB04(u8* p, f32* v, f32* off) { fn_801AD99C(p - 0x23C, v, off); }

void fn_801ADB0C(u8* p, int on) { fn_801ADA38(p - 0x23C, on); }

void fn_801ADB14(u8* p) { fn_801ADA7C(p - 0x23C); }

void fn_801ADB1C(u8* p) { fn_801ADA84(p - 0x23C); }

void fn_801ADB24(u8* p) { fn_801ADA8C(p - 0x23C); }

void fn_801ADB2C(u8* p) { fn_801ADA94(p - 0x23C); }

void fn_801ADB34(u8* p) { fn_801ADA9C(p - 0x23C); }

void fn_801ADB3C(u8* p) { fn_801ADAB8(p - 0x23C); }
}  // extern "C"
