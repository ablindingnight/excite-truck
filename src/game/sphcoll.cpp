// Collision volumes (0x80198F64-0x801A520C): spheres, flat spheres, sphere groups ("SphereGroupPart")
// and their parts, the mesh-backed track collision ("track.msh"), bounds and debug drawing, and
// the per-type collide dispatch ("Unsupported collision type: Sphere-%x"). Written against raw
// field offsets; the large collide routines are still stubs. Names are guesses apart from the
// strings.
#include "game/common.h"
// MATCHING-AID: this unit emits CollShapeIface's table (see game/collshape.h).
#define COLLSHAPE_IFACE_DECL
#include "game/collshape.h"
#include "game/rvec.h"
#include "game/rigid.h"
#include "game/shapes.h"
#include "game/springcar.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

struct PVec3A {
    f32 v[3];
};

// o = c + d * k
static inline void MulAdd(f32* o, const f32* c, const f32* d, f32 k) {
    o[0] = c[0] + d[0] * k;
    o[1] = c[1] + d[1] * k;
    o[2] = c[2] + d[2] * k;
}

// A box corner built component-wise (its constructor arguments are evaluated z, y, x).
struct BoxCorner {
    f32 x, y, z;
    BoxCorner(f32 ax, f32 ay, f32 az) {
        x = ax;
        y = ay;
        z = az;
    }
};

static inline f32 MinF(f32 a, f32 b) { return a < b ? a : b; }
static inline f32 MaxF(f32 a, f32 b) { return a > b ? a : b; }

static inline BoxCorner VMin(const RVec3& a, const RVec3& b) {
    return BoxCorner(MinF(a.x, b.x), MinF(a.y, b.y), MinF(a.z, b.z));
}

static inline BoxCorner VMax(const RVec3& a, const RVec3& b) {
    return BoxCorner(MaxF(a.x, b.x), MaxF(a.y, b.y), MaxF(a.z, b.z));
}

static inline void SetV(RVec3* v, f32 x, f32 y, f32 z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

// Rotation + translation of a collision frame.
struct CFrame {
    f32 m[9];   // offset 0x0, rows
    RVec3 t;    // offset 0x24
};

extern "C" const f32 lbl_805622DC;  // -1.0
extern "C" const f32 lbl_805622E0;  // 1.0

static inline f32 Sq(f32 x) { return x * x; }
static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

static inline void MulVec(RVec3* out, const RVec3* in, const CFrame* m) {
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

static inline void MulVecT(RVec3* out, const RVec3* in, const CFrame* m) {
    if (out == in) {
        f32 x = Dot3(in->x, m->m[0], in->y, m->m[1], in->z, m->m[2]);
        f32 y = Dot3(in->x, m->m[3], in->y, m->m[4], in->z, m->m[5]);
        f32 z = Dot3(in->x, m->m[6], in->y, m->m[7], in->z, m->m[8]);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, m->m[0], in->y, m->m[1], in->z, m->m[2]);
        out->y = Dot3(in->x, m->m[3], in->y, m->m[4], in->z, m->m[5]);
        out->z = Dot3(in->x, m->m[6], in->y, m->m[7], in->z, m->m[8]);
    }
}

struct W3 {
    u32 a, b, c;
};


class VGroup {
public:
    virtual ~VGroup(void);  // 0x08 (a CollShape: slot 0 is its destructor)
    virtual void v0C(void);
    virtual void v10(u8* a);
};

extern "C" void fn_801A4A10(u8* p, const RVec3* c, void* f, const RVec3* b);

// A rotation plus a translation.
struct Mat3x3 {
    f32 m[9];
};

struct XVec3 {
    f32 v[3];
};

struct Xform {
    Mat3x3 rot;  // offset 0x0
    XVec3 pos;   // offset 0x24
};


class VEx;

class VDel {
public:
    virtual ~VDel(void);
};

struct SphGroupDel {
    u8 pad0[0x24];    // offset 0x0
    VDel* vols[20];   // offset 0x24
};

// A bounding plane: unit normal plus distance.
struct PolyPlane {
    W3 n;   // offset 0x0
    f32 d;  // offset 0xC
};

struct PolyVol {
    u8 pad0[0x3C];         // offset 0x0
    PolyPlane planes[8];   // offset 0x3C
};

extern const f32 lbl_8056234C;  // 2.0f

struct Pose12s {
    u32 w[12];
};

class VMesh {
public:
    virtual ~VMesh(void);  // 0x08 (a CollShape: slot 0 is its destructor)
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(void);
    virtual void v1C(void);
    virtual void v20(void);
    virtual void v24(void);
    virtual void v28(void);
    virtual void v2C(u8* vol);
};

extern "C" {
void fn_801CF080(u8* mesh, const RVec3* c, RVec3* hits, int* n, u32 mask, f32 r);
void fn_801A48E4(u8* p);
}

class VSet;

// A sphere group's full layout as far as it is used here.
struct SphGroupAll {
    u8 pad0[0x20];      // offset 0x0
    u32 setA;           // offset 0x20
    VSet* vols[16];     // offset 0x24
    f32 sph[16][4];     // offset 0x64
    u8 pad164[0xC];     // offset 0x164
    f32 radius;         // offset 0x170
    s32 count;          // offset 0x174
    f32 radius2;        // offset 0x178
};

extern "C" {
void fn_801D2414(u8*, const RVec3*, void*);  // (SObj*, const Vec*, Car*) in soevent.cpp
void fn_801D0738(u8* p, const RVec3* c, void* f, const RVec3* b);
f32 PSVECMag(const f32* v);
}

// The per-volume cached values cleared on reset.
struct SphCache {
    u8 pad0[0x64];  // offset 0x0
    f32 v[24];      // offset 0x64
};

// The volume interface viewed through its two setters.
class VSet {
public:
    virtual ~VSet(void);  // CollShape family: destructor at 0x08, SetSurface (SetA) at 0x40
    virtual void s0C(void);
    virtual void s10(void);
    virtual void s14(void);
    virtual void s18(void);
    virtual void s1C(void);
    virtual void s20(void);
    virtual void s24(void);
    virtual void s28(void);
    virtual void s2C(void);
    virtual void s30(void);
    virtual void s34(void);
    virtual void s38(void);
    virtual void s3C(void);
    virtual void SetA(u32 v);
    virtual void SetB(u32 v);
};

struct SphGroupSet {
    u8 pad0[0x24];    // offset 0x0
    VSet* vols[20];   // offset 0x24
};

// A sphere group: its member volumes start at +0x24.
struct SphGroup {
    u8 pad0[0x24];   // offset 0x0
    VEx* vols[20];   // offset 0x24
};
extern s32 lbl_8055E3C4;
extern "C" void fn_801A2674(u8* p, f32 r, u32 a, u32 b);

static inline void Swap32(u8* b) {
    u8 t;
    t = b[0];
    b[0] = b[3];
    b[3] = t;
    t = b[1];
    b[1] = b[2];
    b[2] = t;
}

static inline void SwapN(u8* p, int n) {
    int i;
    for (i = 0; i < n / 2; i++) {
        u8 t = p[i];
        p[i] = p[n - 1 - i];
        p[n - 1 - i] = t;
    }
}

// A collision shape (CollShape family: destructor at 0x08, vf20 friction, Radius 0x3C).
class VEx {
public:
    virtual ~VEx(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(const char* a, const char* b);
    virtual void v1C(const RVec3* imp, const RVec3* at, u32 a, u32 b);  // push
    virtual void v20(u32 who, const RVec3* f, const RVec3* at, u32 a, u32 b, u32 c, f32 w);  // friction
    virtual void v24(void);
    virtual void v28(void);
    virtual void v2C(void);
    virtual u8* v30(void);
    virtual void v34(u8* q);
    virtual void v38(void);
    virtual f32 v3C(void);  // bounding radius
    virtual void v40(void);
    virtual void v44(void);
    virtual void v48(void);
    virtual void v4C(void);
    virtual u8* v50(void);
    virtual u8* v54(void);
};

#define VE(p) ((VEx*)(p))


extern "C" {
f32 PSVECDotProduct(const void* a, const void* b);
double sqrt(double x);
void fn_801E3FC8(u8* c, f32 r, u32 color);
void fn_801E44F4(u8* m, f32 a, f32 b, f32 c, u32 color);
void fn_801CFA48(u8* p);
void fn_801CFF14(u8* p);
void fn_801CFF18(u8* p, u32 a);
u8* fn_801CFEAC(const char* name);
int fn_801CF828(u8* m, u32 a, u32 b, u32 c, u32 d);
void fn_8012C4E4(void* a, void* fn, int b);
void fn_801796EC(int kind, void* msg, int size);
void fn_801D0650(u8* p);
void fn_801D06C4(u8* p);
int fn_8019A820(u8* p, const f32* c, const RVec3* at, u32 a, u32 b, f32 r);
int fn_8019AA34(u8* p, const f32* cv, u8* q, const f32* dv, u32 a, u32 b, f32 r, f32 s);
int fn_8019CCCC(u8* p, u8* c, u8* q, f32 r);
void fn_8019AC44(u8* p, const f32* c, f32* d, f32 r);
int fn_8019DDFC(const f32* from, const f32* dir, f32* t, f32* n, const f32* c, f32 r);
// CollShape and its interface are declared in game/collshape.h.

// MATCHING-AID (transitional): the builders still written as C store these vtables by their linker names
// (BoxSphereShape: fn_8019F564 stays C, 94.9%; the sphere part fn_801A0258; trackobj's PolyShape and
// StaticCollObject builders fn_801A324C / fn_801A4A8C, held while Codex works trackobj).
extern "C" void* __vt__11SphereShape[];
extern "C" void* __vt__14BoxSphereShape[];
extern "C" void* __vt__9PolyShape[];
extern "C" void* __vt__16StaticCollObject[];
void fn_801A49FC(u8* p);
void fn_801A49E8(u8* p);
void fn_801A44D4(u8* l);
void fn_801A4868(u8* p, u32 v);
}

void* operator new(size_t size, const char* tag);
void operator delete(void* p);

extern "C" void* __vt__9TubeShape[];  // MATCHING-AID: TubeVol::Init stores it by name

ModuleTag lbl_8055E590;
UnkStaticFFFF lbl_8055E598;
UnkStaticFFFF lbl_8055E59C;
u8* lbl_8055E5A0;  // track collision mesh
u8 lbl_8055E5A4;        // warned: ring vs box
u8 lbl_8055E5A5;        // warned: ring vs ragdoll

extern "C" {

}  // extern "C"

// A rounded edge of a polyhedron volume ("curve": a cylinder around an axis).
struct PolyCurve {
    RVec3 center;  // offset 0x0
    RVec3 axis;    // offset 0xC
    f32 radius;    // offset 0x18
};

// One penetrated face or edge: push direction and depth.
struct PolyPush {
    RVec3 n;  // offset 0x0
    f32 d;    // offset 0xC
};

extern "C" const f32 lbl_805622D0;  // 0.0
extern "C" const f32 lbl_805622D4;  // 0.001

extern "C" {

// Sphere (centre c, radius r, moving along vel) against a polyhedron volume. In the volume's frame it
// collects every face the sphere reaches (allowing for its motion) and every rounded edge it cuts into,
// pushes the centre out by their depth-weighted average and returns the contact point, depth and normal.
int fn_80198F64(const f32* c, u32 world, f32* hit, f32* depth, f32* n, const f32* vel, f32 r) {
    f32 d;
    f32 x;
    f32 m;
    f32 dep;
    PolyPlane* pl;
    PolyCurve* cv;
    PolyPush* q;
    f32 t;
    f32 sum;
    CFrame* g;
    f32 y;
    u8* o;
    f32 k;
    f32 a;
    CFrame* f;
    int count;
    f32 pen;
    f32 z;
    f32 sq;
    f32 inv;
    o = (u8*)world;
    f = (CFrame*)PTR(o, 0xC);
    RVec3 lc;
    lc.x = c[0] - f->t.x;
    lc.y = c[1] - f->t.y;
    lc.z = c[2] - f->t.z;
    {
        x = Dot3(lc.x, f->m[0], lc.y, f->m[1], lc.z, f->m[2]);
        y = Dot3(lc.x, f->m[3], lc.y, f->m[4], lc.z, f->m[5]);
        z = Dot3(lc.x, f->m[6], lc.y, f->m[7], lc.z, f->m[8]);
        lc.x = x;
        lc.y = y;
        lc.z = z;
    }
    RVec3 lo = *(RVec3*)(o + 0x24);
    RVec3 hi = *(RVec3*)(o + 0x30);
    if (lc.x > hi.x + r || lc.y > hi.y + r || lc.z > hi.z + r || lc.x < lo.x - r || lc.y < lo.y - r ||
        lc.z < lo.z - r) {
        return 0;
    }
    RVec3 lv;
    MulVecT(&lv, (const RVec3*)vel, (CFrame*)PTR(o, 0xC));
    sum = lbl_805622D0;
    count = 0;
    PolyPush push[9];

    pl = ((PolyVol*)o)->planes;
    for (int i = 0; i < S32(o, 0xBC); pl++, i++) {
        f64 mag = __fabs(PSVECDotProduct(&lv, &pl->n));
        a = (f32)mag;
        f32 distance = PSVECDotProduct(&lc, &pl->n) - pl->d;
        d = distance + a;
        if (d >= lbl_805622D4) {
            sum += d * d;
            q = &push[count++];
            q->n = *(RVec3*)&pl->n;
            q->d = d;
        }
    }
    cv = (PolyCurve*)(o + 0xC0);
    for (int i = 0; i < S32(o, 0xDC); cv++, i++) {
        RVec3 dd;
        dd.x = lc.x - cv->center.x;
        dd.y = lc.y - cv->center.y;
        dd.z = lc.z - cv->center.z;
        t = -PSVECDotProduct(&dd, &cv->axis);
        dd.x += cv->axis.x * t;
        dd.y += cv->axis.y * t;
        dd.z += cv->axis.z * t;
        m = PSVECMag(&dd.x);
        pen = cv->radius - m;
        if (m > 0.01f && pen > lbl_805622D4) {
            q = &push[count++];
            sum += pen * pen;
            q->n.x = dd.x * (lbl_805622DC / m);
            q->n.y = dd.y * (lbl_805622DC / m);
            q->n.z = dd.z * (lbl_805622DC / m);
            q->d = pen;
        }
    }
    if (count == 0) {
        return 0;
    }
    inv = lbl_805622E0 / sum;
    RVec3 pc = lc;
    q = push;
    for (; count > 0; count--, q++) {
        sq = q->d * q->d;
        k = -q->d * (inv * sq);
        pc.x += q->n.x * k;
        pc.y += q->n.y * k;
        pc.z += q->n.z * k;
    }
    RVec3 w;
    g = (CFrame*)PTR(o, 0xC);
    MulVec(&w, &pc, g);
    w.x += g->t.x;
    w.y += g->t.y;
    w.z += g->t.z;
    RVec3 dv;
    dv.x = w.x - c[0];
    dv.y = w.y - c[1];
    dv.z = w.z - c[2];
    m = PSVECMag(&dv.x);
    dep = r - m;
    *depth = dep;
    if (dep > lbl_805622D0) {

        hit[0] = w.x;
        hit[1] = w.y;
        k = lbl_805622DC / m;
        hit[2] = w.z;
        n[0] = dv.x * k;
        n[1] = dv.y * k;
        n[2] = dv.z * k;
        return 1;
    }
    return 0;
}

// Friction impulse at contact point pt (normal n) between volumes a and b: the opposing tangential
// relative velocity, scaled by -1000 and clamped to half of mu * (a's + b's friction).
void fn_801996BC(RVec3* out, const RVec3* pt, const RVec3* n, u8* a, u8* b, f32 mu) {
    RVec3 dn;
    RVec3 rel;
    RVec3 va;
    RVec3 vb;
    RVec3 f;
    RigidBody* ra = (RigidBody*)((u8*&)((CollShape*)a)->owner);
    RigidBody* rb = (RigidBody*)((u8*&)((CollShape*)b)->owner);
    if (ra) {
        ra->VelAt(&va, pt);
    } else {
        va.x = 0.0f;
        va.y = 0.0f;
        va.z = 0.0f;
    }
    if (rb) {
        rb->VelAt(&vb, pt);
    } else {
        vb.x = 0.0f;
        vb.y = 0.0f;
        vb.z = 0.0f;
    }
    rel.x = va.x - vb.x;
    rel.y = va.y - vb.y;
    rel.z = va.z - vb.z;
    f32 d = PSVECDotProduct(n, &rel);
    dn.x = n->x * d;
    dn.y = n->y * d;
    dn.z = n->z * d;
    rel.x = rel.x - dn.x;
    rel.y = rel.y - dn.y;
    rel.z = rel.z - dn.z;
    f.x = rel.x;
    f.y = rel.y;
    f.z = rel.z;
    f.x *= -1000.0f;
    f.y *= -1000.0f;
    f.z *= -1000.0f;
    f32 mag = PSVECMag(&f.x);
    f32 fb = ((CollShape*)b)->skin;
    f32 fa = ((CollShape*)a)->skin;
    f32 lim = 0.5f * mu * (fa + fb);
    if (mag > lim) {
        f32 k = lim / mag;
        f.x *= k;
        f.y *= k;
        f.z *= k;
    }
    *out = f;
}

void fn_80199870(u8* p, void* w) {
    u32 t = U32(p, 0x10);
    if (t == 'PCAR' || t == 'ACAR') {
        ((SpringCar*)p)->HitWall((WallObj*)w);
    }
}

extern const f32 lbl_805622E0;  // 1.0
void fn_801BEAEC(int car, int wheel, PVec3A* out);                       // wheel position (masslist.cpp)
void fn_801BEB44(int car, int wheel, const RVec3* at, const PVec3A* n);  // wheel contact (masslist.cpp)

static inline int IsCarObj(const u8* o) {
    if (o) {
        switch (S32(o, 0x10)) {
        case 'ACAR':
        case 'CAR ':
        case 'NCAR':
        case 'PCAR':
            return 1;
        }
    }
    return 0;
}

static inline f32 ClampExt(f32 v, f32 e) {
    if (e < v) {
        v = e;
    }
    return v > -e ? v : -e;
}

extern f32 __float_epsilon[];  // FLT_EPSILON
void fn_801B34DC(u8* p, u8* q, const RVec3* at, const RVec3* f, const f32* n, u32 a);  // collision sound
extern const f32 lbl_805622E8;  // 0.5
extern const f32 lbl_80562300;  // 0.2
extern const f32 lbl_80562308;  // 0.05

// The race rules object (fn_801B83A0) seen through its car-hit hook.
class HitRules {
public:
    virtual ~HitRules(void);  // 0x08 (the arena master GameMode: slot 0 is its destructor)
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void vf18(void);
    virtual void vf1C(void);
    virtual void vf20(void);
    virtual void vf24(void);
    virtual void vf28(void);
    virtual void vf2C(void);
    virtual void vf30(void);
    virtual void vf34(void);
    virtual void vf38(void);
    virtual void vf3C(void);
    virtual void vf40(void);
    virtual void vf44(void);
    virtual void vf48(void);
    virtual void vf4C(void);
    virtual void vf50(void);
    virtual void vf54(void);
    virtual void vf58(void);
    virtual void vf5C(void);
    virtual void vf60(void);
    virtual void vf64(void);
    virtual void vf68(void);
    virtual void vf6C(void);
    virtual void vf70(void);
    virtual void vf74(void);
    virtual void vf78(void);
    virtual void vf7C(void);
    virtual void vf80(void);
    virtual void vf84(void);
    virtual void vf88(void);
    virtual void vf8C(void);
    virtual void vf90(void);
    virtual void vf94(void);
    virtual void vf98(void);
    virtual void CarHit(u8* car, s32 kind, u8* vol);  // 0x9C
};

// The current-player context a volume's owner exposes (v30): read, switched to a car, restored.
class PlayerCtx {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual int Get(void);       // 0x14
    virtual void Set(int ctx);  // 0x18
};

extern "C" {
HitRules* fn_801B83A0(void);
f32 fn_8018AE7C(u8* car);
void fn_802C1A0C(PlayerCtx* c, int car);
void PSVECCrossProduct(const void* a, const void* b, void* out);
}

// Resolves one contact between volumes p and q at point x along normal n (pointing into p) with compression s:
// volumes tagged 1000+ only report the touch; otherwise a spring push at heights clamped to +-5 ft, and while
// closing, the restitution and clamped friction impulses through both bodies' inverse inertia, plus a car-hit
// event for hard enough player crashes.
static inline f32 InvMass(f32 m) { return lbl_805622E0 / m; }

static inline f32 VDot(const RVec3* a, const RVec3* b) { return PSVECDotProduct(a, b); }

void fn_80199894(u8* p, u8* q, const RVec3* pt, const RVec3* n, u32 a, u32 b, f32 s, f32 u, f32 v) {
    RVec3 pa;
    RVec3 pq;
    RVec3 imp;
    RVec3 va;
    RVec3 vb;
    RVec3 rel;
    RVec3 fr;
    RVec3 ra;
    RVec3 rb;
    RVec3 tt;
    RVec3 ca;
    RVec3 cb;
    RVec3 ia;
    RVec3 ib;
    RVec3 ta;
    RVec3 tb;
    RVec3 J;
    RVec3 tv;
    RVec3 tt2;
    RVec3 ca2;
    RVec3 cb2;
    RVec3 ia2;
    RVec3 ib2;
    RVec3 ta2;
    RVec3 tb2;
    RVec3 z1;
    RVec3 z2;
    RVec3 z3;
    RVec3 z4;
    CFrame* fp;
    CFrame* fq;
    RigidBody* pb;
    RigidBody* qb;
    s32 pk = ((CollShape*)p)->surface;
    s32 qk = ((CollShape*)q)->surface;
    fp = (CFrame*)((u8*&)((CollShape*)p)->frame);
    fq = (CFrame*)((u8*&)((CollShape*)q)->frame);
    pb = (RigidBody*)((u8*&)((CollShape*)p)->owner);
    qb = (RigidBody*)((u8*&)((CollShape*)q)->owner);
    if (qk == 0x3EA) {
        fn_80199870((u8*)pb, q);
        return;
    }
    if (qk >= 1000 && pb && IsCarObj((u8*)pb)) {
        fn_801B83A0()->CarHit((u8*)pb, qk, q);
    }
    if (qk >= 1000 || pk >= 1000) {
        z1.x = lbl_805622D0;
        z1.y = lbl_805622D0;
        z1.z = lbl_805622D0;
        VE(p)->v1C(&z1, pt, qk, (u32)qb);
        z2.x = lbl_805622D0;
        z2.y = lbl_805622D0;
        z2.z = lbl_805622D0;
        VE(q)->v1C(&z2, pt, pk, (u32)pb);
        z3.x = lbl_805622D0;
        z3.y = lbl_805622D0;
        z3.z = lbl_805622D0;
        VE(q)->v20((u32)p, &z3, pt, qk, (u32)pt, (u32)pb, lbl_805622D4);
        z4.x = lbl_805622D0;
        z4.y = lbl_805622D0;
        z4.z = lbl_805622D0;
        VE(p)->v20((u32)q, &z4, pt, pk, (u32)pt, (u32)qb, lbl_805622D4);
        return;
    }
    pa.x = pt->x;
    pa.y = pt->y;
    pa.z = pt->z;
    pq.x = pt->x;
    pq.y = pt->y;
    pq.z = pt->z;
    if (pb && qb) {
        f32 ha = MaxF(MinF(1.5239999f, ((CFrame*)((u8*&)((CollShape*)p)->frame))->t.y - pt->y), -1.5239999f);
        f32 hb = MaxF(MinF(1.5239999f, ((CFrame*)((u8*&)((CollShape*)q)->frame))->t.y - pt->y), -1.5239999f);
        pa.y = pt->y + 0.95f * ha;
        pq.y = pt->y + 0.95f * hb;
    }
    if (pb) {
        pb->VelAt(&va, pt);
    } else {
        va.x = lbl_805622D0;
        va.y = lbl_805622D0;
        va.z = lbl_805622D0;
    }
    if (qb) {
        qb->VelAt(&vb, pt);
    } else {
        vb.x = lbl_805622D0;
        vb.y = lbl_805622D0;
        vb.z = lbl_805622D0;
    }
    rel.x = va.x - vb.x;
    rel.y = va.y - vb.y;
    rel.z = va.z - vb.z;
    f32 vn = PSVECDotProduct(&rel, n);
    f32 st = MaxF(((CollShape*)p)->stiff, ((CollShape*)q)->stiff);
    if (vn > lbl_805622D0) {
        st *= 0.8f;
    }
    f32 k = s * st;
    if (k > 222500.0f) {
        k = 222500.0f;
    }
    f32 kk = lbl_805622E8 * k;
    imp.x = n->x * kk;
    imp.y = n->y * kk;
    imp.z = n->z * kk;
    fr.x = lbl_805622D0;
    fr.y = lbl_805622D0;
    fr.z = lbl_805622D0;
    fn_801996BC(&fr, pt, n, p, q, k);
    RVec3* rp = &rel;
    imp.x += fr.x;
    imp.y += fr.y;
    imp.z += fr.z;
    ra.x = pt->x - fp->t.x;
    ra.y = pt->y - fp->t.y;
    ra.z = pt->z - fp->t.z;
    rb.x = pt->x - fq->t.x;
    rb.y = pt->y - fq->t.y;
    rb.z = pt->z - fq->t.z;
    if (vn < lbl_805622D0) {
        PSVECCrossProduct(&ra, n, &ca);
        PSVECCrossProduct(&rb, n, &cb);
        if (pb) {
            MulVec(&ia, &ca, (CFrame*)((u8*)pb + 0x148));
        } else {
            ia.x = lbl_805622D0;
            ia.y = lbl_805622D0;
            ia.z = lbl_805622D0;
        }
        PSVECCrossProduct(&ia, &ra, &ta);
        if (qb) {
            MulVec(&ib, &cb, (CFrame*)((u8*)qb + 0x148));
        } else {
            ib.x = lbl_805622D0;
            ib.y = lbl_805622D0;
            ib.z = lbl_805622D0;
        }
        PSVECCrossProduct(&ib, &rb, &tb);
        tt.x = ta.x + tb.x;
        tt.y = ta.y + tb.y;
        tt.z = ta.z + tb.z;
        f32 ima = pb ? InvMass(F32(pb, 0xD4)) : lbl_805622D0;
        f32 imb = qb ? InvMass(F32(qb, 0xD4)) : lbl_805622D0;
        f32 e = lbl_805622D0;
        if (-vn < 8.888889f) {
            e = lbl_80562300;
        }
        f32 j = -(lbl_805622E0 + e) * PSVECDotProduct(rp, n) / (ima + imb + VDot(n, &tt));
        J.x = n->x * j;
        J.y = n->y * j;
        J.z = n->z * j;
        f32 fq2 = ((CollShape*)q)->skin;
        f32 fp2 = ((CollShape*)p)->skin;
        f32 mu = lbl_805622E8 * (fp2 + fq2);
        tv.x = rel.x + n->x * -vn;
        tv.y = rel.y + n->y * -vn;
        tv.z = rel.z + n->z * -vn;
        f32 tl = PSVECMag(&tv.x);
        if (tl > lbl_80562308) {
            f32 inv = lbl_805622E0 / MaxF(tl, lbl_80562308);
            tv.x *= inv;
            tv.y *= inv;
            tv.z *= inv;
            PSVECCrossProduct(&ra, &tv, &ca2);
            PSVECCrossProduct(&rb, &tv, &cb2);
            if (pb) {
                MulVec(&ia2, &ca2, (CFrame*)((u8*)pb + 0x148));
            } else {
                ia2.x = lbl_805622D0;
                ia2.y = lbl_805622D0;
                ia2.z = lbl_805622D0;
            }
            PSVECCrossProduct(&ia2, &ra, &ta2);
            if (qb) {
                MulVec(&ib2, &cb2, (CFrame*)((u8*)qb + 0x148));
            } else {
                ib2.x = lbl_805622D0;
                ib2.y = lbl_805622D0;
                ib2.z = lbl_805622D0;
            }
            PSVECCrossProduct(&ib2, &rb, &tb2);
            tt2.x = ta2.x + tb2.x;
            tt2.y = ta2.y + tb2.y;
            tt2.z = ta2.z + tb2.z;
            f32 jt = lbl_805622DC * PSVECDotProduct(rp, &tv) / (ima + imb + VDot(&tv, &tt2));
            jt = ClampExt(jt, (f32)__fabs(j * mu));
            J.x = J.x + tv.x * jt;
            J.y = J.y + tv.y * jt;
            J.z = J.z + tv.z * jt;
        }
        f32 w = lbl_805622E0;
        if (pb && IsCarObj((u8*)pb) && (u32)(qk - 0x73) <= 1) {
            int fast = 1;
            if (!U8(pb, 0x25C8)) {
                bool big = F32(pb, 0x25BC) > __float_epsilon[0];
                if (!big) {
                    fast = 0;
                }
            }
            if ((fast || qk == 0x74) && PSVECDotProduct(&J, &J) >= 10000.0f &&
                F32(pb, 0x25D4) > 6.6666665f && VE(q)->v30()) {
                f32 sc = fn_8018AE7C((u8*)pb);
                J.x *= sc;
                J.y *= sc;
                J.z *= sc;
                PlayerCtx* ctx = (PlayerCtx*)VE(q)->v30();
                int was = ctx->Get();
                fn_802C1A0C(ctx, S32(pb, 0x1EC));
                int now = ctx->Get();
                if (was && !now) {
                    ctx->Set(was);
                    fn_801A4A10(((CollShape*)q)->data, pt, pb, &J);
                    ctx->Set(now);
                }
            }
        }
        VE(p)->v20((u32)q, &J, &pa, U32(q, 0x20), (u32)pt, (u32)qb, w);
        J.x *= lbl_805622DC;
        J.y *= lbl_805622DC;
        J.z *= lbl_805622DC;
        VE(q)->v20((u32)p, &J, &pq, U32(p, 0x20), (u32)pt, (u32)pb, w);
        if (a) {
            fn_801B34DC((u8*)a, (u8*)b, pt, &J, &n->x, U32(q, 0x20));
        } else {
            fn_801B34DC(p, q, pt, &J, &n->x, U32(q, 0x20));
        }
    }
    VE(p)->v1C(&imp, &pa, U32(q, 0x20), (u32)qb);
    imp.x *= lbl_805622DC;
    imp.y *= lbl_805622DC;
    imp.z *= lbl_805622DC;
    VE(q)->v1C(&imp, &pq, U32(p, 0x20), (u32)pb);
}

u32 CollShape::vf30(void) {
    u8* p = (u8*)this; return U32(p, 0x8); }

extern const f32 lbl_805622DC;  // -1.0

// Contact between p and q along n; volumes without an owner hand the test to q with the normal flipped.
void fn_8019A788(u8* p, u8* q, u32 x, const RVec3* n, u32 a, u32 b, f32 s, f32 u, f32 v) {
    if (U32(p, 0x4) || a) {
        fn_80199894(p, q, (const RVec3*)x, n, a, b, s, u, v);
    } else {
        RVec3 m;
        m.x = n->x * lbl_805622DC;
        m.y = n->y * lbl_805622DC;
        m.z = n->z * lbl_805622DC;
        fn_80199894(q, p, (const RVec3*)x, &m, b, a, s, v, u);
    }
}

f32 fn_8019DF88(const u8* com, const RVec3* v, const u8* iinv, const RVec3* at, const f32* n, f32 mass, f32 e);
void fn_801B34DC(u8* p, u8* q, const RVec3* at, const RVec3* f, const f32* n, u32 a);  // collision sound

// Sphere (c, r) of p against the point at: a spring push along the normal, then friction from the body.
int fn_8019A820(u8* p, const f32* c, const RVec3* at, u32 a, u32 b, f32 r) {
    PVec3A d;
    PVec3A n;
    RVec3 vel;
    RVec3 imp;
    RVec3 fr;
    d.v[0] = c[0] - at->x;
    d.v[1] = c[1] - at->y;
    d.v[2] = c[2] - at->z;
    f32 len = PSVECMag(d.v);
    RigidBody* rb = (RigidBody*)((u8*&)((CollShape*)p)->owner);
    if (len > 0.001f && len < r) {
        f32 pen = r - len;
        f32 inv = 1.0f / len;
        n.v[0] = d.v[0] * inv;
        n.v[1] = d.v[1] * inv;
        n.v[2] = d.v[2] * inv;
        rb->VelAt(&vel, at);
        f32 k = pen * ((CollShape*)p)->stiff;
        PSVECDotProduct(&vel, n.v);
        imp.x = n.v[0] * k;
        imp.y = n.v[1] * k;
        imp.z = n.v[2] * k;
        VE(p)->v1C(&imp, at, a, 0);
        f32 f = fn_8019DF88((u8*)rb + 0x60, &vel, (u8*)rb + 0x148, at, n.v, F32(rb, 0xD4), 0.3f);
        if (f > 0.0f) {
            fr.x = n.v[0] * f;
            fr.y = n.v[1] * f;
            fr.z = n.v[2] * f;
            f32 lo = 0.001f;
            f32 w = lo > pen ? lo : pen;
            VE(p)->v20(b, &fr, at, a, 0, 0, w);
            fn_801B34DC(p, 0, at, &fr, n.v, a);
        }
        return 1;
    }
    return 0;
}

void fn_8019AA18(u8* p, u32 a, u32 b, u32 c) { fn_8019A820(p, (f32*)(p + 0x34), (RVec3*)a, b, c, F32(p, 0x24)); }

extern f32 __float_epsilon[];  // FLT_EPSILON
void PSVECNormalize(const void* in, void* out);

static inline f32 AbsF(f32 v) { return (f32)__fabs(v); }

// Sphere (c, r) of p against sphere (d, s) of q: reports the contact (normal flattened in y) to the
// volume that has an owner.
static inline f32 SphereMagnitude(f32 value) { f64 wide = __fabs(value); return (f32)wide; }

int fn_8019AA34(u8* p, const f32* cv, u8* q, const f32* dv, u32 a, u32 b, f32 r, f32 s) {
    PVec3A dd;
    PVec3A n;
    PVec3A at;
    dd.v[0] = cv[0] - dv[0];
    dd.v[1] = cv[1] - dv[1];
    dd.v[2] = cv[2] - dv[2];
    f32 rs = r + s;
    int hit = 0;
    if (SphereMagnitude(dd.v[0]) < rs && AbsF(dd.v[2]) < rs && AbsF(dd.v[1]) < rs) {
        f32 dist = PSVECMag(dd.v);
        if (!(AbsF(dist) <= __float_epsilon[0])) {
            f32 pen = s + (-dist + r);
            if (pen > 0.0f) {
                f32 inv = 1.0f / dist;
                n.v[0] = dd.v[0] * inv;
                n.v[1] = dd.v[1] * inv;
                n.v[2] = dd.v[2] * inv;
                n.v[1] *= 0.2f;
                PSVECNormalize(n.v, n.v);
                f32 k = -0.5f * ((r + dist) - s);
                at.v[0] = cv[0] + n.v[0] * k;
                at.v[1] = cv[1] + n.v[1] * k;
                at.v[2] = cv[2] + n.v[2] * k;
                if (U32(p, 0x4)) {
                    fn_8019A788(p, q, (u32)at.v, (RVec3*)n.v, a, b, pen, 0.0f, 0.0f);
                } else {
                    n.v[0] *= -1.0f;
                    n.v[1] *= -1.0f;
                    n.v[2] *= -1.0f;
                    fn_8019A788(q, p, (u32)at.v, (RVec3*)n.v, a, b, pen, 0.0f, 0.0f);
                }
                hit = 1;
            }
        }
    }
    return hit;
}

extern const f32 lbl_8056231C;
extern const f32 lbl_80562324;
extern const f32 lbl_805622D4;
extern const f32 lbl_8056232C;
extern const f32 lbl_805622D0;
extern "C" int fn_801E9F08(u8* hint, const f32* at, RVec3* pos, RVec3* n, int* surf, f32 maxDist);

// Sphere (centre c, radius r) on the ground: every track-mesh hit pushes via fn_8019A820; then the
// ground contact gives a spring impulse, a clamped friction force (with d, the slip accumulator, nudged
// by the tangential velocity), a spin damping torque and the bounce/sound events. Off the ground d is cleared.
void fn_8019AC44(u8* p, const f32* c, f32* d, f32 r) {
    RVec3 hits[64];
    RVec3 pt;
    RVec3 nrm;
    RVec3 dv;
    RVec3 v;
    RVec3 imp;
    RVec3 vt;
    RVec3 f;
    RVec3 t;
    RVec3 fr;
    int n;
    int surf;
    RigidBody* o = (RigidBody*)((u8*&)((CollShape*)p)->owner);
    if (lbl_8055E5A0) {
        n = 64;
        fn_801CF080(lbl_8055E5A0, (const RVec3*)c, hits, &n, 0, r);
        RVec3* h = hits;
        for (int i = 0; i < n; h++, i++) {
            fn_8019A820(p, c, h, 0x6E, 0, r);
        }
    }
    if (fn_801E9F08((u8*)o + 0x6C, c, &pt, &nrm, &surf, r)) {
        f32 dx = c[0] - pt.x;
        f32 dy = c[1] - pt.y;
        f32 dz = c[2] - pt.z;
        dv.x = dx;
        dv.y = dy;
        dv.z = dz;
        f32 depth = r - PSVECDotProduct(&nrm, &dv);
        o->VelAt(&v, &pt);
        f32 k = depth * ((CollShape*)p)->stiff;
        f32 vn = -PSVECDotProduct(&v, &nrm);
        f32 ny = nrm.y;
        f32 nx = nrm.x;
        imp.x = nx * k;
        imp.y = ny * k;
        imp.z = nrm.z * k;
        vt.x = v.x + nx * vn;
        vt.y = v.y + ny * vn;
        vt.z = v.z + nrm.z * vn;
        d[0] = d[0] + vt.x * lbl_8056231C;
        d[1] = d[1] + vt.y * lbl_8056231C;
        d[2] = d[2] + vt.z * lbl_8056231C;
        f32 dm = PSVECMag(d);
        if (dm > 0.02f) {
            f32 s = 0.02f / dm;
            d[0] *= s;
            d[1] *= s;
            d[2] *= s;
        }
        f.x = vt.x * lbl_80562324;
        f.y = vt.y * lbl_80562324;
        f.z = vt.z * lbl_80562324;
        const f32 value = -2400.0f;
        f32 mass = F32(o, 0xD4);
        f32 m = mass * value;
        f.x = f.x + d[0] * m;
        f.y = f.y + d[1] * m;
        f.z = f.z + d[2] * m;
        f32 fm = PSVECMag(&f.x);
        f32 lim = k * ((CollShape*)p)->skin;
        lim = lim > 0.02f ? lim : 0.02f;
        if (fm > lim) {
            f32 s = lim / fm;
            f.x *= s;
            f.y *= s;
            f.z *= s;
        }
        imp.x += f.x;
        imp.y += f.y;
        imp.z += f.z;
        f32 w = -PSVECDotProduct((u8*)o + 0x124, &nrm);
        w = lbl_8056232C * (w * k);
        t.x = nrm.x * w;
        t.y = nrm.y * w;
        t.z = nrm.z * w;
        o->AddTorque(&t);
        VE(p)->v1C(&imp, &pt, surf, 0);
        f32 e = 0.33f * fn_8019DF88((u8*)o + 0x60, &v, (u8*)o + 0x148, &pt, &nrm.x, F32(o, 0xD4), lbl_805622D0);
        if (e > lbl_805622D0) {
            fr.x = nrm.x * e;
            fr.y = nrm.y * e;
            fr.z = nrm.z * e;
            f32 lo = lbl_805622D4;
            f32 ww = lo > depth ? lo : depth;
            VE(p)->v20(0, &fr, &pt, surf, 0, 0, ww);
            fn_801B34DC(p, 0, &pt, &fr, &nrm.x, surf);
        }
    } else {
        d[0] = lbl_805622D0;
        d[1] = lbl_805622D0;
        d[2] = lbl_805622D0;
    }
}

int fn_8019B068(u8* p, u8* q, u32 a, u32 b) {
    f32 s = ((SphereShape*)q)->radius;
    return fn_8019AA34(p, (f32*)(p + 0x34), q, (f32*)(q + 0x34), a, b, ((SphereShape*)p)->radius, s);
}

// Sphere (centre c, radius r) against the side of tube q: in tube space, within the half length and off
// the axis, a contact at the tube surface with the radial normal.
int fn_8019B08C(u8* p, const f32* c, u8* q, f32 r) {
    CFrame* f = (CFrame*)((u8*&)((CollShape*)q)->frame);
    RVec3 l;
    l.x = c[0] - f->t.x;
    l.y = c[1] - f->t.y;
    l.z = c[2] - f->t.z;
    f32 x = Dot3(l.x, f->m[0], l.y, f->m[1], l.z, f->m[2]);
    f32 y = Dot3(l.x, f->m[3], l.y, f->m[4], l.z, f->m[5]);
    f32 z = Dot3(l.x, f->m[6], l.y, f->m[7], l.z, f->m[8]);
    l.x = x;
    l.y = y;
    l.z = z;
    f32 len = ((TubeShape*)q)->half;
    if (l.z < len && l.z > -len) {
        f32 dist = (f32)sqrt(Sq(l.x) + Sq(l.y));
        if (!((f32)__fabs(dist) <= __float_epsilon[0])) {
            f32 R = ((TubeShape*)q)->rad;
            r += -dist + R;
            if (r > 0.0f) {
                RVec3 cc;
                cc.x = c[0];
                cc.y = c[1];
                cc.z = c[2];
                RVec3 ct;
                ct.x = f->t.x;
                ct.y = f->t.y;
                ct.z = f->t.z;
                RVec3 s;
                RVec3 w;
                RVec3 nw;
                RVec3 n;
                RVec3 nn;
                s.x = l.x;
                s.y = l.y;
                s.z = 0.0f;
                f32 k = (((TubeShape*)q)->rad - r) / dist;
                s.x *= k;
                s.z = l.z;
                s.y *= k;
                CFrame* m = (CFrame*)((u8*&)((CollShape*)q)->frame);
                MulVec(&w, &s, m);
                w.x += m->t.x;
                w.y += m->t.y;
                w.z += m->t.z;
                n.x = l.x;
                n.y = l.y;
                n.z = 0.0f;
                PSVECNormalize(&n.x, &n.x);
                MulVec(&nw, &n, (CFrame*)((u8*&)((CollShape*)q)->frame));
                nn = nw;
                fn_8019A788(p, q, (u32)&w, &nn, 0, 0, r, 0.0f, 0.0f);
                return 1;
            }
        }
    }
    return 0;
}

extern const f32 lbl_80562300;  // 0.2
extern const f32 lbl_8056233C;  // 0.025025027
extern "C" void fn_802E30C8(const char* fmt, ...);
extern "C" void fn_802E317C(const char* fmt, ...);

// l = (p - frame origin) in the frame's axes.
static inline void ToBoxSpace(RVec3* l, const RVec3* p, const CFrame* f) {
    f32 dx = p->x - f->t.x;
    f32 dy = p->y - f->t.y;
    f32 dz = p->z - f->t.z;
    l->x = Dot3(dx, f->m[0], dy, f->m[1], dz, f->m[2]);
    l->y = Dot3(dx, f->m[3], dy, f->m[4], dz, f->m[5]);
    l->z = Dot3(dx, f->m[6], dy, f->m[7], dz, f->m[8]);
}

// Clamps box-space point l into the box of half extents ext.
static inline void ClampToBox(RVec3* l, const RVec3* ext) {
    l->x = MaxF(MinF(ext->x, l->x), -ext->x);
    l->y = MaxF(MinF(ext->y, l->y), -ext->y);
    l->z = MaxF(MinF(ext->z, l->z), -ext->z);
}

// collide_explicit_sphere_box: sphere p (centre c, radius r) against box q (half extents at 0x24). The closest
// box point gives the compression; a sphere centre already inside the box ("delta_length" ~ 0) is treated as
// tunneling and pushed back along the box body's velocity when it moves fast enough.
int fn_8019B4D8(u8* p, u8* q, const RVec3* c, u32 debug, f32 r) {
    CFrame f = *(CFrame*)VE(q)->v50();
    const RVec3* ext = (const RVec3*)&((BoxShape*)q)->half;
    RVec3 l;
    RVec3 cp;
    RVec3 d;
    ToBoxSpace(&l, c, &f);
    if (debug) {
        fn_802E30C8("cesb: cp1: %f, %f, %f R:(%f, %f, %f)", l.x, l.y, l.z, ext->x, ext->y, ext->z);
    }
    ClampToBox(&l, ext);
    MulVec(&cp, &l, &f);
    cp.x = cp.x + f.t.x;
    cp.y = cp.y + f.t.y;
    cp.z = cp.z + f.t.z;
    d.x = cp.x - c->x;
    d.y = cp.y - c->y;
    d.z = cp.z - c->z;
    f32 comp;
    f32 len = sqrt(PSVECDotProduct(&d, &d));
    comp = lbl_80562300 + (-len + r);
    if (len < 0.01f) {
        if (debug) {
            fn_802E30C8("-collide_explicit_sphere_box: delta_length = %f", len);
        }
        RigidBody* rb = (RigidBody*)((u8*&)((BoxShape*)q)->owner);
        if (rb) {
            d.x = F32(rb, 0x118) * lbl_805622DC;
            d.y = F32(rb, 0x11C) * lbl_805622DC;
            d.z = F32(rb, 0x120) * lbl_805622DC;
            len = PSVECMag(&d.x);
            if (debug) {
                fn_802E30C8("tunneling: %1.2f", len);
            }
            if (len < 4.4444447f) {
                if (debug) {
                    fn_802E30C8("too slow");
                }
                return 0;
            }
            comp = 0.254f;
            if (len < 0.01f) {
                return 0;
            }
        } else if (rb) {
            fn_802E317C("volume: explicit_sphere_box method: code should never get here");
            RVec3 v;
            rb->VelAt(&v, c);
            f32 xv = c->x;
            cp.x = xv + v.x * lbl_8056233C;
            cp.z = c->z + v.z * lbl_8056233C;
            cp.y = c->y + v.y * lbl_8056233C;
            ToBoxSpace(&l, &cp, &f);
            if (debug) {
                fn_802E30C8("cesb: cp1: %f, %f, %f R:(%f, %f, %f)", l.x, l.y, l.z, ext->x, ext->y, ext->z);
            }
            ClampToBox(&l, ext);
            RVec3 back;
            MulVec(&back, &l, &f);
            back.x = back.x + f.t.x;
            back.y = back.y + f.t.y;
            back.z = back.z + f.t.z;
            if (debug) {
                fn_802E30C8("backing up to %f, %f, %f", back.x, back.y, back.z);
            }
            d.x = back.x - c->x;
            d.y = back.y - c->y;
            d.z = back.z - c->z;
            len = PSVECMag(&d.x);
            if (len < 0.01f) {
                if (debug) {
                    fn_802E30C8("-collide_explicit_sphere_box: (2) delta_length = %f", len);
                }
                return 0;
            }
            comp = 0.254f;
        } else {
            return 0;
        }
    }
    if (comp > lbl_805622D0) {
        RVec3 n;
        f32 k = lbl_805622DC / len;
        n.x = d.x * k;
        n.y = d.y * k;
        n.z = d.z * k;
        if (debug) {
            fn_802E30C8("colliding! cylinder_box(%p, %p: %d)", p, q, ((CollShape*)p)->surface);
            CFrame* pf = (CFrame*)((u8*&)((CollShape*)p)->frame);
            fn_802E30C8("%f,%f,%f", pf->t.x, pf->t.y, pf->t.z);
            fn_802E30C8("comp = %f!", comp);
        }
        fn_8019A788(p, q, (u32)&cp, &n, 0, 0, comp, lbl_805622D0, lbl_805622D0);
        if (debug) {
            fn_802E30C8("-collide_explicit_sphere_box: COLLIDE!");
        }
        return 1;
    }
    if (debug) {
        fn_802E30C8("-collide_explicit_sphere_box: MISS (compression = %f)", comp);
    }
    return 0;
}

u32 BoxShape::vf50(void) {
    u8* p = (u8*)this; return U32(p, 0xc); }

extern const f32 lbl_805622E8;  // 0.5
f32 PSVECSquareDistance(const void* a, const void* b);
f32 lbl_8055A4C0 = 0.8f;           // 0.8, car-on-car friction
extern const f32 lbl_80562308;     // 0.05
extern const f32 lbl_80562340;     // 0.0016683352

// Sphere p against box q (half extents 0x24..0x2C, bounding radius 0x30, end-cap flags at 0x34), the box
// optionally offset by `off` in its own frame. Clamps the sphere centre into the box (`dir`, a direction in box
// space, keeps the x face open when moving away from it); a centre fully inside is backed out along the
// relative velocity. Cars touching cars scrape with a friction scaled by how aligned they are.
static inline void Clip(f32* v, f32 e, u32* in, u32 bit) {
    if (*v > e) {
        *v = e;
    } else if (*v < -e) {
        *v = -e;
    } else {
        *in |= bit;
    }
}

int fn_8019BD54(u8* p, u8* q, const RVec3* off, const RVec3* dir) {
    CFrame f;
    RVec3 w;
    RVec3 cp;
    RVec3 l;
    RVec3 c;
    RVec3 va;
    RVec3 vb;
    RVec3 rv;
    RVec3 back;
    RVec3 dd;
    RVec3 d;
    RVec3 n;
    RVec3 o;
    f32 d2;
    if (!off) {
        d2 = PSVECSquareDistance(p + 0x34, VE(q)->v50() + 0x24);
    } else {
        CFrame* m = (CFrame*)VE(q)->v50();
        MulVec(&w, off, m);
        w.x = w.x + m->t.x;
        w.y = w.y + m->t.y;
        w.z = w.z + m->t.z;
        d2 = PSVECSquareDistance(p + 0x34, &w);
    }
    f32 rr = F32(p, 0x24) + F32(q, 0x30);
    if (d2 > rr * rr) {
        return 0;
    }
    f = *(CFrame*)VE(q)->v50();
    if (off) {
        MulVec(&o, off, &f);
        f.t.x = f.t.x + o.x;
        f.t.y = f.t.y + o.y;
        f.t.z = f.t.z + o.z;
    }
    RigidBody* pb = (RigidBody*)PTR(p, 0x4);
    RigidBody* qb = (RigidBody*)PTR(q, 0x4);
    u32 caps;
    u32 in = 0;
    c.y = F32(p, 0x38);
    c.x = F32(p, 0x34);
    c.z = F32(p, 0x3C);
    ToBoxSpace(&l, &c, &f);
    if (!dir || l.x * dir->x > lbl_805622D0 || F32(q, 0x28) < (f32)__fabs(dir->y)) {
        f32* localX = &l.x;
        Clip(localX, F32(q, 0x24), &in, 1);
    }
    Clip(&l.y, F32(q, 0x28), &in, 2);
    caps = U32(q, 0x34);
    if ((caps & 1) && F32(q, 0x2C) < l.z) {
        return 0;
    }
    if ((caps & 2) && l.z < -F32(q, 0x2C)) {
        return 0;
    }
    Clip(&l.z, F32(q, 0x2C), &in, 4);
    if (in == 7) {
        if (pb) {
            pb->VelAt(&va, &c);
        } else {
            va.x = lbl_805622D0;
            va.y = lbl_805622D0;
            va.z = lbl_805622D0;
        }
        if (qb) {
            qb->VelAt(&vb, &c);
        } else {
            vb.x = lbl_805622D0;
            vb.y = lbl_805622D0;
            vb.z = lbl_805622D0;
        }
        rv.x = va.x - vb.x;
        rv.y = va.y - vb.y;
        rv.z = va.z - vb.z;
        back.x = c.x + rv.x * lbl_80562340;
        back.y = c.y + rv.y * lbl_80562340;
        back.z = c.z + rv.z * lbl_80562340;
        dd.x = f.t.x - c.x;
        dd.y = f.t.y - c.y;
        dd.z = f.t.z - c.z;
        if (PSVECDotProduct(&dd, &rv) > lbl_805622D4) {
            ToBoxSpace(&l, &back, &f);
        }
    }
    MulVec(&cp, &l, &f);
    cp.x = cp.x + f.t.x;
    cp.y = cp.y + f.t.y;
    cp.z = cp.z + f.t.z;
    d.x = cp.x - c.x;
    d.y = cp.y - c.y;
    d.z = cp.z - c.z;
    f32 len = sqrt(PSVECDotProduct(&d, &d));
    if (len < lbl_80562308) {
        return 0;
    }
    f32 comp = lbl_80562300 + (-len + F32(p, 0x24));
    f32 fq = lbl_805622D0;
    if (fq < comp) {
        f32 fp = fq;
        f32 k = lbl_805622DC / len;
        n.x = d.x * k;
        n.y = d.y * k;
        n.z = d.z * k;
        if (qb && pb) {
            u32 qf = U32(q, 0x1C);
            u32 pf = U32(p, 0x1C);
            u32 mask = ((~(qf ^ pf) >> 2 & 1) ? 2 : 0) | 1;
            bool qa = (qf & mask) != 0;
            bool pa = (pf & mask) != 0;
            if ((qa && IsCarObj((u8*)pb)) || (pa && IsCarObj((u8*)qb))) {
                f32 fr = lbl_8055A4C0;
                f32 t = 2.8f * (PSVECDotProduct((u8*)pb + 0x54, (u8*)qb + 0x54) - lbl_805622E8);
                fr *= MaxF(MinF(lbl_805622E0, t), lbl_805622D0);
                if (pa) {
                    fp = fr;
                }
                if (qa) {
                    fq = fr;
                }
            }
        }
        if (pb) {
            fn_8019A788(p, q, (u32)&cp, &n, 0, 0, comp, fp, fq);
        } else {
            n.x = n.x * lbl_805622DC;
            n.y = n.y * lbl_805622DC;
            n.z = n.z * lbl_805622DC;
            fn_8019A788(q, p, (u32)&cp, &n, 0, 0, comp, fq, fp);
        }
        return 1;
    }
    return 0;
}

// Sphere p against ring q (major radius 0x24, tube radius 0x28): in ring space, within the slab, the
// nearest point of the ring's centre circle gives the contact.
int fn_8019C854(u8* p, u8* q) {
    RVec3 l;
    RVec3 a;
    RVec3 n;
    RVec3 w;
    RVec3 nw;
    CFrame* f = (CFrame*)((u8*&)((CollShape*)q)->frame);
    l.x = F32(p, 0x34) - f->t.x;
    l.y = F32(p, 0x38) - f->t.y;
    l.z = F32(p, 0x3C) - f->t.z;
    f32 x = Dot3(l.x, f->m[0], l.y, f->m[1], l.z, f->m[2]);
    f32 y = Dot3(l.x, f->m[3], l.y, f->m[4], l.z, f->m[5]);
    f32 z = Dot3(l.x, f->m[6], l.y, f->m[7], l.z, f->m[8]);
    l.x = x;
    l.y = y;
    l.z = z;
    f32 rs = F32(p, 0x24);
    f32 R = F32(q, 0x24);
    f32 tr = F32(q, 0x28);
    if (l.z < rs + tr && l.z > -(rs + tr)) {
        f32 dist = (f32)sqrt(Sq(l.x) + Sq(l.y));
        f32 pen = (rs + tr) - AbsF(dist - R);
        if (pen > 0.0f) {
            if (dist > R + tr) {
                f32 k = (dist - rs) / dist;
                a.x = l.x * k;
                a.y = l.y * k;
                a.z = l.z * k;
            } else if (dist < R - tr) {
                f32 k = (dist + rs) / dist;
                a.x = l.x * k;
                a.y = l.y * k;
                a.z = l.z * k;
            } else {
                a = l;
            }
            a.z = 0.0f;
            n.x = l.x - a.x;
            n.y = l.y - a.y;
            n.z = l.z - a.z;
            PSVECNormalize(&n.x, &n.x);
            a.z = l.z;
            CFrame* m = (CFrame*)((u8*&)((CollShape*)q)->frame);
            MulVec(&w, &a, m);
            w.x += m->t.x;
            w.y += m->t.y;
            w.z += m->t.z;
            MulVec(&nw, &n, (CFrame*)((u8*&)((CollShape*)q)->frame));
            fn_8019A788(p, q, (u32)&w, &nw, 0, 0, pen, 0.0f, 0.0f);
            return 1;
        }
    }
    return 0;
}

// Capsule (q: frame at 0xC with axis at 0x18 and centre at 0x24, half length 0x28, radius 0x24):
// the cylinder test, then a sphere test at each end.
// A float field read through a pointer (a getter in the original; found by the permuter).
static inline f32 LoadF(const f32* f) { return *f; }

int fn_8019CCCC(u8* p, u8* c, u8* q, f32 r) {
    u8* m;
    f32 h;
    PVec3A a;
    PVec3A b;
    PVec3A axis;
    PVec3A org;
    f32 rad;
    int hit;
    if (fn_8019B08C(p, (f32*)c, q, r)) {
        return 1;
    }
    m = ((u8*&)((CollShape*)q)->frame);
    h = LoadF((f32*)(q + 0x28));
    axis = *(PVec3A*)(m + 0x18);
    org = *(PVec3A*)(m + 0x24);
    rad = LoadF((f32*)(q + 0x24));
    MulAdd(a.v, org.v, axis.v, h);
    MulAdd(b.v, org.v, axis.v, -h);
    hit = 0;
    hit = (hit | fn_8019AA34(p, (f32*)c, q, a.v, 0, 0, r, rad)) != 0;
    hit = (hit | fn_8019AA34(p, (f32*)c, q, b.v, 0, 0, r, rad)) != 0;
    return hit;
}

void fn_8019CE3C(u8* p, u32 a) { fn_8019CCCC(p, p + 0x34, (u8*)a, F32(p, 0x24)); }

extern const f32 lbl_8056232C;  // small distance
f32 PSVECSquareDistance(const void* a, const void* b);

// Sets flat sphere q's radius from how edge-on it is to sphere p, then collides them.
void fn_8019CE4C(u8* p, u8* q) {
    f32* c = (f32*)(p + 0x34);
    f32 d[3];
    d[0] = c[0] - ((FlatSphereShape*)q)->f34;
    d[1] = c[1] - ((FlatSphereShape*)q)->f38;
    d[2] = c[2] - ((FlatSphereShape*)q)->f3C;
    f32 sq = PSVECSquareDistance(c, q + 0x34);
    f32 r;
    if (sq > lbl_8056232C) {
        f32 inv = 1.0f / (f32)sqrt(sq);
        d[0] *= inv;
        d[1] *= inv;
        d[2] *= inv;
        f32 k = (f32)__fabs(PSVECDotProduct(d, ((u8*&)((CollShape*)q)->frame) + 0xC));
        r = ((FlatSphereShape*)q)->across + k * (((FlatSphereShape*)q)->along - ((FlatSphereShape*)q)->across);
    } else {
        f32 b = ((FlatSphereShape*)q)->along;
        f32 a = ((FlatSphereShape*)q)->across;
        r = a > b ? a : b;
    }
    ((FlatSphereShape*)q)->radius = r;
    fn_8019B068(p, q, 0, 0);
}


// Sets flat sphere q's radius from how edge-on it is to p, then collides them.
void fn_8019CF60(u8* p, u8* q) {
    u8* f = ((u8*&)((CollShape*)p)->frame);
    f32 d[3];
    d[0] = F32(f, 0x24) - ((FlatSphereShape*)q)->f34;
    d[1] = F32(f, 0x28) - ((FlatSphereShape*)q)->f38;
    d[2] = F32(f, 0x2C) - ((FlatSphereShape*)q)->f3C;
    f32 sq = PSVECSquareDistance(f + 0x24, q + 0x34);
    f32 r;
    if (sq > lbl_8056232C) {
        f32 inv = 1.0f / (f32)sqrt(sq);
        d[0] *= inv;
        d[1] *= inv;
        d[2] *= inv;
        f32 c = (f32)__fabs(PSVECDotProduct(d, ((u8*&)((CollShape*)q)->frame) + 0xC));
        r = ((FlatSphereShape*)q)->across + c * (((FlatSphereShape*)q)->along - ((FlatSphereShape*)q)->across);
    } else {
        f32 b = ((FlatSphereShape*)q)->along;
        f32 a = ((FlatSphereShape*)q)->across;
        r = a > b ? a : b;
    }
    ((FlatSphereShape*)q)->radius = r;
    fn_8019CE3C(q, (u32)p);
}

void fn_801A1F44(RVec3* out, u8* q);

// Sets flat sphere p's radius from how edge-on it is to box q, then collides them.
void fn_8019D06C(u8* p, u8* q, const RVec3* a, const RVec3* b) {
    RVec3 c;
    fn_801A1F44(&c, q);
    f32 d[3];
    d[0] = c.x - F32(p, 0x34);
    d[1] = c.y - F32(p, 0x38);
    d[2] = c.z - F32(p, 0x3C);
    f32 sq = PSVECSquareDistance(&c, p + 0x34);
    f32 r;
    if (sq > lbl_8056232C) {
        f32 inv = 1.0f / (f32)sqrt(sq);
        d[0] *= inv;
        d[1] *= inv;
        d[2] *= inv;
        f32 k = (f32)__fabs(PSVECDotProduct(d, ((u8*&)((CollShape*)p)->frame) + 0xC));
        r = F32(p, 0x54) + k * (F32(p, 0x58) - F32(p, 0x54));
    } else {
        f32 hi = F32(p, 0x58);
        f32 lo = F32(p, 0x54);
        r = lo > hi ? lo : hi;
    }
    F32(p, 0x24) = r;
    fn_8019BD54(p, q, a, b);
}

// Flat sphere s takes a radius between its min (0x54) and max (0x58) by how edge-on it is to c.
static inline void FlatRadius(u8* s, const f32* cp) {
    const f32* c = cp;
    f32 d[3];
    f32 r;
    d[0] = c[0] - ((FlatSphereShape*)s)->f34;
    d[1] = c[1] - ((FlatSphereShape*)s)->f38;
    d[2] = c[2] - ((FlatSphereShape*)s)->f3C;
    f32 sq = PSVECSquareDistance(c, s + 0x34);
    if (sq > lbl_8056232C) {
        f32 inv = 1.0f / (f32)sqrt(sq);
        d[0] *= inv;
        d[1] *= inv;
        d[2] *= inv;
        f32 k = (f32)__fabs(PSVECDotProduct(d, ((u8*&)((FlatSphereShape*)s)->frame) + 0xC));
        r = ((FlatSphereShape*)s)->across + k * (((FlatSphereShape*)s)->along - ((FlatSphereShape*)s)->across);
    } else {
        f32 hi = ((FlatSphereShape*)s)->along;
        f32 lo = ((FlatSphereShape*)s)->across;
        r = lo > hi ? lo : hi;
    }
    ((FlatSphereShape*)s)->radius = r;
}

// Sets both flat spheres' radii from how edge-on they are to each other, then collides them.
void fn_8019D19C(u8* p, u8* q) {
    FlatRadius(p, (f32*)(q + 0x34));
    FlatRadius(q, (f32*)(p + 0x34));
    fn_8019B068(p, q, 0, 0);
}

// Sets flat sphere p's radius from how edge-on it is to ring q, then collides them.
void fn_8019D378(u8* p, u8* q) {
    f32* c = (f32*)(((u8*&)((CollShape*)q)->frame) + 0x24);
    f32 d[3];
    d[0] = c[0] - ((FlatSphereShape*)p)->f34;
    d[1] = c[1] - ((FlatSphereShape*)p)->f38;
    d[2] = c[2] - ((FlatSphereShape*)p)->f3C;
    f32 sq = PSVECSquareDistance(c, p + 0x34);
    f32 r;
    if (sq > lbl_8056232C) {
        f32 inv = 1.0f / (f32)sqrt(sq);
        d[0] *= inv;
        d[1] *= inv;
        d[2] *= inv;
        f32 k = (f32)__fabs(PSVECDotProduct(d, ((u8*&)((CollShape*)p)->frame) + 0xC));
        r = ((FlatSphereShape*)p)->across + k * (((FlatSphereShape*)p)->along - ((FlatSphereShape*)p)->across);
    } else {
        f32 b = ((FlatSphereShape*)p)->along;
        f32 a = ((FlatSphereShape*)p)->across;
        r = a > b ? a : b;
    }
    ((FlatSphereShape*)p)->radius = r;
    fn_8019C854(p, q);
}

void PSVECCrossProduct(const void* a, const void* b, void* out);

static inline f32 Det33(f32 m[3][3]) {
    return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) + m[0][1] * (m[1][2] * m[2][0] - m[1][0] * m[2][2]) +
           m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
}

// Closest approach of the lines p0 + t*d0 and p1 + s*d1; 0 when they are parallel.
int fn_8019D484(const f32* p0, const f32* d0, const f32* p1, const f32* d1, f32* t, f32* s) {
    f32 m[3][3];
    PVec3A w;
    PVec3A n;
    w.v[0] = p1[0] - p0[0];
    w.v[1] = p1[1] - p0[1];
    w.v[2] = p1[2] - p0[2];
    PSVECCrossProduct(d0, d1, n.v);
    f32 nn = PSVECDotProduct(n.v, n.v);
    if (nn > 0.0000010000001f) {
        m[0][0] = w.v[0];
        m[0][1] = d1[0];
        m[0][2] = n.v[0];
        m[1][0] = w.v[1];
        m[1][1] = d1[1];
        m[1][2] = n.v[1];
        m[2][0] = w.v[2];
        m[2][1] = d1[2];
        m[2][2] = n.v[2];
        *t = Det33(m) / nn;
        m[0][1] = d0[0];
        m[1][1] = d0[1];
        m[2][1] = d0[2];
        *s = Det33(m) / nn;
        return 1;
    }
    return 0;
}

extern const f32 lbl_805622E8;  // 0.5

// Tube p against tube q: closest points of the two axes (parameters clamped to the half lengths when
// `clamp`, else out-of-range is a miss); overlapping radii report one contact at the midpoint.
int fn_8019D618(u8* p, u8* q, int clamp, u8* who) {
    f32 m[3][3];
    RVec3 ca;
    RVec3 cb;
    RVec3 pa;
    RVec3 pb;
    RVec3 da;
    RVec3 db;
    RVec3 d;
    RVec3 n;
    RVec3 mid;
    RVec3 w;
    RVec3 diff;
    RVec3 at;
    RVec3 nrm;
    f32 s;
    f32 t;
    ca = *(RVec3*)(((u8*&)((CollShape*)p)->frame) + 0x24);
    cb = *(RVec3*)(((u8*&)((CollShape*)q)->frame) + 0x24);
    da = *(RVec3*)(((u8*&)((CollShape*)p)->frame) + 0x18);
    db = *(RVec3*)(((u8*&)((CollShape*)q)->frame) + 0x18);
    d.x = cb.x - ca.x;
    d.y = cb.y - ca.y;
    d.z = cb.z - ca.z;
    PSVECCrossProduct(&da.x, &db.x, &n.x);
    f32 nn = PSVECDotProduct(&n.x, &n.x);
    f32 la = F32(p, 0x28);
    f32 lb = ((TubeShape*)q)->half;
    if (nn < lbl_8056232C) {
        f32 zv;
        f32 yv;
        f32 xv = ca.x;
        mid.x = (xv + cb.x) / 2.0f;
        yv = ca.y;
        zv = cb.z;
        mid.z = (ca.z + zv) * 0.5f;
        mid.y = (yv + cb.y) / 2.0f;
        w.x = mid.x - ca.x;
        w.y = mid.y - ca.y;
        w.z = mid.z - ca.z;
        s = PSVECDotProduct(&da.x, &w.x);
        w.x = mid.x - cb.x;
        w.y = mid.y - cb.y;
        w.z = mid.z - cb.z;
        t = PSVECDotProduct(&db.x, &w.x);
    } else {
        if (!((f32)__fabs(nn) <= __float_epsilon[0])) {
        m[0][0] = d.x;
        m[0][1] = db.x;
        m[0][2] = n.x;
        m[1][0] = d.y;
        m[1][1] = db.y;
        m[1][2] = n.y;
        m[2][0] = d.z;
        m[2][1] = db.z;
        m[2][2] = n.z;
        s = Det33(m) / nn;
        m[0][1] = da.x;
        m[1][1] = da.y;
        m[2][1] = da.z;
        t = Det33(m) / nn;
        } else {
            return 0;
        }
    }
    if (!clamp) {
        if (s < -la || s > la || t < -lb || t > lb) {
            return 0;
        }
    } else {
        if (s < -la) {
            s = -la;
        }
        if (s > la) {
            s = la;
        }
        if (t < -lb) {
            t = -lb;
        }
        if (t > lb) {
            t = lb;
        }
    }
    MulAdd(&pa.x, &ca.x, &da.x, s);
    MulAdd(&pb.x, &cb.x, &db.x, t);
    diff.x = pb.x - pa.x;
    diff.y = pb.y - pa.y;
    diff.z = pb.z - pa.z;
    f32 dist = PSVECMag(&diff.x);
    if ((f32)__fabs(dist) <= __float_epsilon[0]) {
        return 0;
    }
    f32 pen = (((TubeShape*)q)->rad + F32(p, 0x24)) - dist;
    if (pen < 0.0f) {
        return 0;
    }
    f32 k = -1.0f / dist;
    nrm.x = diff.x * k;
    nrm.y = diff.y * k;
    at.x = (pa.x + pb.x) / 2.0f;
    at.y = (pa.y + pb.y) / 2.0f;
    nrm.z = diff.z * k;
    at.z = (pa.z + pb.z) / 2.0f;
    fn_8019A788(who ? who : p, q, (u32)&at.x, &nrm, 0, 0, pen, 0.0f, 0.0f);
    return 1;
}

// Tube p vs q: the cylinder first, then both end caps as spheres.
int fn_8019DAB0(u8* p, u8* q) {
    bool hit = false;
    hit |= fn_8019D618(p, q, 0, 0);
    if (hit) {
        return 1;
    }
    PVec3A a;
    PVec3A b;
    PVec3A dir = *(PVec3A*)(((u8*&)((CollShape*)p)->frame) + 0x18);
    PVec3A mid = *(PVec3A*)(((u8*&)((CollShape*)p)->frame) + 0x24);
    f32 h = ((TubeShape*)p)->half;
    f32 r = ((TubeShape*)p)->rad;
    MulAdd(a.v, mid.v, dir.v, h);
    MulAdd(b.v, mid.v, dir.v, -h);
    hit |= fn_8019B08C(p, a.v, q, r);
    hit |= fn_8019B08C(p, b.v, q, r);
    return hit;
}

// Capsule p against capsule q: bounding spheres, then the axis segments, then p's end spheres.
int fn_8019DC04(u8* p, u8* q) {
    PVec3A d;
    PVec3A b;
    PVec3A a;
    PVec3A axis;
    PVec3A org;
    u8* fq = ((u8*&)((CollShape*)q)->frame);
    u8* fp = ((u8*&)((CollShape*)p)->frame);
    d.v[0] = F32(fp, 0x24) - F32(fq, 0x24);
    d.v[1] = F32(fp, 0x28) - F32(fq, 0x28);
    d.v[2] = F32(fp, 0x2C) - F32(fq, 0x2C);
    f32 rq = VE(q)->v3C();
    f32 rp = VE(p)->v3C();
    f32 r = rp + rq;
    f32 r2 = r * r;
    if (PSVECDotProduct(d.v, d.v) < r2) {
        bool hit = false;
        hit |= fn_8019DAB0(p, q);
        if (hit) {
            return 1;
        }
        u8* m = ((u8*&)((CollShape*)p)->frame);
        f32 h = ((TubeShape*)p)->half;
        axis = *(PVec3A*)(m + 0x18);
        org = *(PVec3A*)(m + 0x24);
        f32 rad = ((TubeShape*)p)->rad;
        MulAdd(b.v, org.v, axis.v, -h);
        MulAdd(a.v, org.v, axis.v, h);
        hit |= fn_8019CCCC(p, (u8*)a.v, q, rad);
        if (hit) {
            return 1;
        }
        hit |= fn_8019CCCC(p, (u8*)b.v, q, rad);
        return hit;
    }
    return 0;
}

f32 TubeBase::Radius(void) {
    u8* p = (u8*)this; return this->rad + this->half; }

extern const f32 lbl_805622D0;  // 0.0

// Ray from + t*dir against the sphere (c, r): the nearer hit if it lies in [0, *t), with its normal.
int fn_8019DDFC(const f32* from, const f32* dir, f32* t, f32* n, const f32* c, f32 r) {
    PVec3A m;
    m.v[0] = from[0] - c[0];
    m.v[1] = from[1] - c[1];
    m.v[2] = from[2] - c[2];
    f32 b = lbl_8056234C * PSVECDotProduct(m.v, dir);
    f32 mm = PSVECDotProduct(m.v, m.v);
    f32 disc = b * b - 4.0f * (mm - r * r);
    if (disc > 1e-5f) {
        f32 t0 = 0.5f * (-b - (f32)sqrt(disc));
        if (t0 >= lbl_805622D0 && t0 < *t) {
            *t = t0;
            n[0] = m.v[0] + dir[0] * t0;
            n[1] = m.v[1] + dir[1] * t0;
            n[2] = m.v[2] + dir[2] * t0;
            f32 len = PSVECMag(n);
            f32 k = 0.0001f;
            f32 inv = 1.0f / (k > len ? k : len);
            n[0] *= inv;
            n[1] *= inv;
            n[2] *= inv;
            return 1;
        }
    }
    return 0;
}

// Impulse magnitude for a contact at `at` with normal n on a body (centre of mass com, inverse inertia
// iinv, mass, restitution e) moving at v: -(1 + e) v.n / (1/mass + n.((iinv (r x n)) x r)), capped.
f32 fn_8019DF88(const u8* com, const RVec3* v, const u8* iinv, const RVec3* at, const f32* n, f32 mass, f32 e) {
    RVec3 vel;
    PVec3A r;
    PVec3A nn;
    PVec3A un;
    RVec3 rn;
    RVec3 t;
    PVec3A u;
    f32 rx = at->x - F32(com, 0x0);
    f32 ry = at->y - F32(com, 0x4);
    f32 rz = at->z - F32(com, 0x8);
    f32 vz, vy, vx;
    vx = v->x;
    vy = v->y;
    vz = v->z;
    vel.x = vx;
    vel.y = vy;
    vel.z = vz;
    r.v[0] = rx;
    r.v[1] = ry;
    r.v[2] = rz;
    nn = *(const PVec3A*)n;
    if (PSVECDotProduct(&vel, n) < 0.0f) {
        un = nn;
        PSVECNormalize(un.v, un.v);
        PSVECCrossProduct(r.v, un.v, &rn);
        MulVec(&t, &rn, (const CFrame*)iinv);
        PSVECCrossProduct(&t, r.v, u.v);
        f32 denom = 1.0f / mass + PSVECDotProduct(un.v, u.v);
        if ((f32)__fabs(denom) > 1e-7f) {
            f32 j = (-(1.0f + e) * PSVECDotProduct(&vel, un.v)) / denom;
            f32 cap = 100000.0f;
            cap = cap < j ? cap : j;
            return cap;
        }
    }
    return 0.0f;
}

extern f32 __float_epsilon[];       // FLT_EPSILON
extern const f32 lbl_805622D0;  // 0.0

// Clips the ray parameter range [*tmin, *tmax] against the slab |o + t*d| <= r.
int fn_8019E1F4(f32* tmin, f32* tmax, f32 o, f32 d, f32 r) {
    if ((f32)__fabs(d) <= __float_epsilon[0]) {
        if (o < -r || o > r) {
            return 0;
        }
        return 1;
    }
    f32 t2 = (-r - o) / d;
    f32 t1 = (r - o) / d;
    f32 lo = t2 < t1 ? t2 : t1;
    *tmin = *tmin > lo ? *tmin : lo;
    f32 hi = t2 > t1 ? t2 : t1;
    *tmax = *tmax < hi ? *tmax : hi;
    if (*tmin > *tmax) {
        return 0;
    }
    return !(*tmax < lbl_805622D0);
}

// out = rotation^T * in (into the frame's space).

// Ray from + t*dir against the box (frame m, half sizes hx, hy, hz): slab test in box space; the entry
// distance replaces *t when nearer.
int fn_8019E2D0(const RVec3* from, const RVec3* dir, f32* t, const CFrame* m, f32 hx, f32 hy, f32 hz) {
    RVec3 lp;
    RVec3 ld;
    f32 tmin;
    f32 tmax;
    lp.x = from->x - m->t.x;
    lp.y = from->y - m->t.y;
    lp.z = from->z - m->t.z;
    {
        f32 x = Dot3(lp.x, m->m[0], lp.y, m->m[1], lp.z, m->m[2]);
        f32 y = Dot3(lp.x, m->m[3], lp.y, m->m[4], lp.z, m->m[5]);
        f32 z = Dot3(lp.x, m->m[6], lp.y, m->m[7], lp.z, m->m[8]);
        lp.x = x;
        lp.y = y;
        lp.z = z;
    }
    MulVecT(&ld, dir, m);
    tmin = -1e8f;
    tmax = 1e8f;
    if (fn_8019E1F4(&tmin, &tmax, lp.x, ld.x, hx) && fn_8019E1F4(&tmin, &tmax, lp.y, ld.y, hy) &&
        fn_8019E1F4(&tmin, &tmax, lp.z, ld.z, hz) && tmin < *t) {
        *t = tmin;
        return 1;
    }
    return 0;
}

// One box half-axis: direction and extent.
struct BoxAxis {
    PVec3A dir;
    f32 ext;
};

// Frame of one box edge: the two cross axes, the edge axis and the edge centre.
struct EdgeFrame {
    RVec3 a;
    RVec3 b;
    RVec3 axis;
    RVec3 pos;
};

u8 lbl_80435020[0x30];  // scratch tube volume for box edges

// Placement-constructs a tube volume (radius r, half length len) on frame f.
// A tube volume built on the fly (same layout as the file tubes).
struct TubeVol {
    void* vt;           // offset 0x0
    u32 owner;          // offset 0x4
    u32 f8;             // offset 0x8
    EdgeFrame* frame;   // offset 0xC
    f32 f10;            // offset 0x10
    f32 f14;            // offset 0x14
    s32 type;           // offset 0x18
    u32 f1C;            // offset 0x1C
    s32 f20;            // offset 0x20
    f32 r;              // offset 0x24
    f32 len;            // offset 0x28

    void Init(EdgeFrame* f, u32 o, f32 a, f32 b, f32 rr, f32 l) {
        frame = f;
        owner = o;
        f8 = 0;
        type = 'TUBE';
        f10 = a;
        f14 = b;
        f20 = 100;
        f1C = 0;
        r = rr;
        len = l;
        vt = __vt__9TubeShape;
    }
};

// p against box q's edges: the four edges along each of q's x and z axes are tested as tubes of
// radius 0.75 inset by that radius.
}
template <class T>
static inline void SetXYZ(T* v, f32 x, f32 y, f32 z) { /* Helper name unproven */
    v->x = x;
    v->y = y;
    v->z = z;
}
extern "C" {

int fn_8019E500(u8* p, u8* q) {
    TubeVol* t;
    CFrame fr = *(CFrame*)VE(q)->v50();
    BoxAxis ax[3] = {0};
    ax[0].dir = *(PVec3A*)&fr.m[0];
    ax[1].dir = *(PVec3A*)&fr.m[3];
    ax[2].dir = *(PVec3A*)&fr.m[6];
    ax[0].ext = ((BoxShape*)q)->half.x;
    ax[1].ext = ((BoxShape*)q)->half.y;
    ax[2].ext = ((BoxShape*)q)->half.z;
    bool hit = false;
    int i;
    f32 r = 0.75f;
    for (i = 0; i < 3; i++) {
        if (i == 1) {
            continue;
        }
        EdgeFrame e;
        BoxAxis* ab = &ax[(i + 2) % 3];
        BoxAxis* aa = &ax[(i + 1) % 3];
        int j;
        BoxAxis* ai = (BoxAxis*)((char*)ax + i * (int)sizeof(BoxAxis));
        e.axis.x = ai->dir.v[0];
        e.axis.y = ai->dir.v[1];
        e.axis.z = ai->dir.v[2];
        f32 eb = ab->ext - r;
        f32 ea = aa->ext - r;
        e.b.x = ab->dir.v[0];
        e.b.y = ab->dir.v[1];
        e.b.z = ab->dir.v[2];
        SetXYZ(&e.a, aa->dir.v[0], aa->dir.v[1], aa->dir.v[2]);
        for (j = 0; j < 4; j++) {
            e.pos = fr.t;
            f32 k = ea * ((j & 1) ? -1.0f : 1.0f);
            e.pos.x += aa->dir.v[0] * k;
            e.pos.y += aa->dir.v[1] * k;
            e.pos.z += aa->dir.v[2] * k;
            k = eb * ((j & 2) ? -1.0f : 1.0f);
            e.pos.x += ab->dir.v[0] * k;
            e.pos.y += ab->dir.v[1] * k;
            e.pos.z += ab->dir.v[2] * k;
            t = (TubeVol*)lbl_80435020;
            if (t) {
                t->Init(&e, U32(q, 0x4), ((BoxShape*)q)->stiff, ((BoxShape*)q)->skin, r, ai->ext);
            }
            hit |= fn_8019D618((u8*)t, p, 1, q);
        }
    }
    return hit;
}

// Capsule p against q: the point of p's axis nearest q's centre (clamped to +-half length 0x28) is
// tested as a sphere of p's radius; types 0x73 / 0x74 take fn_8019E500 instead.
// Body type ids (0x20 of a body); the two values here take the edge-tube test.
enum SphBodyType { SPH_TYPE_73 = 0x73, SPH_TYPE_74 = 0x74 };

int fn_8019E874(u8* p, u8* q) {
    if ((SphBodyType)((CollShape*)p)->surface == SPH_TYPE_73 || (SphBodyType)((CollShape*)p)->surface == SPH_TYPE_74) {
        return fn_8019E500(p, q);
    }
    CFrame oq;
    CFrame fp;
    RVec3 c;
    RVec3 d;
    oq = *(CFrame*)VE(q)->v50();
    fp = *(CFrame*)((u8*&)((CollShape*)p)->frame);
    c = fp.t;
    d.x = c.x - oq.t.x;
    d.y = c.y - oq.t.y;
    d.z = c.z - oq.t.z;
    f32 nh;
    f32 h = ((TubeShape*)p)->half;
    nh = -h;
    f32 s = -PSVECDotProduct(&fp.m[6], &d);
    f32 v = h < s ? h : s;
    v = v > nh ? v : nh;
    c.x = c.x + fp.m[6] * v;
    c.y = c.y + fp.m[7] * v;
    c.z = c.z + fp.m[8] * v;
    return fn_8019B4D8(p, q, &c, 0, ((TubeShape*)p)->rad);
}

// Capsule p against q, with the centre offset kept for a debug view that is compiled out.
int fn_8019EA7C(u8* p, u8* q) {
    CFrame* fq = (CFrame*)VE(q)->v50();
    CFrame* fp = (CFrame*)((u8*&)((CollShape*)p)->frame);
    f32 x = fp->t.x - fq->t.x;
    f32 y = fp->t.y - fq->t.y;
    f32 z = fp->t.z - fq->t.z;
    BoxCorner d(x, y, z);
    bool hit = false;
    hit |= fn_8019E874(p, q);
    return hit;
}

void fn_8019EB14(u8* tree, int i) {
    u8* n = tree + i * 8;
    u8 t;
    t = n[0];
    n[0] = n[1];
    n[1] = t;
    t = n[2];
    n[2] = n[3];
    n[3] = t;
    t = n[4];
    n[4] = n[5];
    n[5] = t;
    t = n[6];
    n[6] = n[7];
    n[7] = t;
    if (*(u16*)(n + 4)) {
        for (int k = 0; k < 4; k++) {
            fn_8019EB14(tree, *(u16*)(n + 4) + k);
        }
    }
}

void CollShape::vf1C(void) {
    u8* p = (u8*)this;
    u8* o = ((u8*&)this->owner);
    if (o) {
        VE(o)->v48();
    }
}

void CollShape::vf20(void* a, const RVec3* b, const RVec3* c, int d, const RVec3* e, void* f, f32 g) {
    if (owner) {
        ((RigidBody*)owner)->Push(this, a, b, c, d, e, f, g);
    } else if (data) {
        fn_801A4A10(data, c, f, b);
    }
}

// Forwards an impulse notice (RigidBody::Push records the shape) to the owner's slot 0x4C, RigidBody::OnImpulse,
// with the same arguments (the call passes them through untouched in r4-r8).
void CollShape::vf24(const RVec3* v, const RVec3* at, int kind, void* c, void* by) {
    if (owner) {
        ((RigidBody*)owner)->OnImpulse(v, at, kind, c, by);
    }
}

void CollShape::vf2C(u8* q) {
    u8* p = (u8*)this;
    this->data = q;
    VE(p)->v34(q + 4);
}

void CollShape::vf34(CFrame* p_v) {
    u8* p = (u8*)this;
    u32 v = (u32)p_v; U32(p, 0xc) = v; }

// Collision volume base constructor from file data (byte-swapped when `swap`).
CollShape::CollShape(u32 a, u32 swap) {
    u8* p = (u8*)this;
    if (swap) {
        Swap32(p + 0x18);
        Swap32(p + 0x10);
        Swap32(p + 0x14);
        Swap32(p + 0x20);
        Swap32(p + 0x1C);
    }
    U32(p, 0x8) = a;
    U32(p, 0x4) = 0;
    U32(p, 0xC) = a + 4;
}

// Sphere volume constructor from file data (byte-swapped when `swap`).
SphereShape::SphereShape(u32 a, u32 swap) : CollShape(a, swap) {
    u8* p = (u8*)this;
    if (swap) {
        Swap32(p + 0x24);
        f32* w = (f32*)&this->center;
        for (int i = 0; i < 3; i++) {
            Swap32((u8*)&w[i]);
        }
        if (this->surface == 0x3EB) {
            this->radius += 3.35f;
        }
    }
    this->b4C = 0;
    this->f34 = 0.0f;
    this->f38 = 0.0f;
    this->f3C = 0.0f;
    this->f40 = 0.0f;
    this->f44 = 0.0f;
    this->f48 = 0.0f;
    this->f50 = 0.0f;
}

void fn_8019EECC(u8* p, RVec3* v) {
    v->x += F32(p, 0x30);
    v->y += F32(p, 0x34);
    v->z += F32(p, 0x38);
}


struct CVolume {
    u8 pad0[0xC];
    CFrame* frame;  // offset 0xC
    u8 pad10[0x18];
    RVec3 local;    // offset 0x28
    RVec3 world;    // offset 0x34
};

// World centre = frame * local centre.
void SphereShape::vf0C(void) {
    CVolume* v = (CVolume*)this;
    CFrame* f = v->frame;
    MulVec(&v->world, &v->local, f);
    v->world.x = v->world.x + f->t.x;
    v->world.y = v->world.y + f->t.y;
    v->world.z = v->world.z + f->t.z;
}

void SphereShape::vf28(void) {
    u8* p = (u8*)this;
    this->b4C = 0;
    this->f40 = 0.0f;
    this->f44 = 0.0f;
    this->f48 = 0.0f;
    this->f34 = 0.0f;
    this->f38 = 0.0f;
    this->f3C = 0.0f;
}

void fn_802E317C(const char* fmt, ...);

// Collides sphere `p` against volume `o` by its type tag.
void SphereShape::vf10(u8* o) {
    u8* p = (u8*)this;
    switch (((CollShape*)o)->type) {
    case 'RAGD':
        break;
    case 'FLSR':
        fn_8019CE4C(p, o);
        break;
    case 'SPHR':
        fn_8019B068(p, o, 0, 0);
        break;
    case 'TUBE':
        fn_8019CE3C(p, (u32)o);
        break;
    case 'BOX ':
        fn_8019BD54(p, o, 0, 0);
        break;
    case 'GRUP':
        ((VGroup*)o)->v10(p);
        break;
    case 'RING':
        fn_8019C854(p, o);
        break;
    case 'POLY':
        ((VGroup*)o)->v10(p);
        break;
    case 'PMSH':
        ((VGroup*)o)->v10(p);
        break;
    default:
        fn_802E317C("Unsupported collision type: Sphere-%x", ((CollShape*)o)->type);
        break;
    }
}

void SphereShape::vf14(void) {
    u8* p = (u8*)this; fn_8019AC44(p, (f32*)(p + 0x34), (f32*)(p + 0x40), this->radius); }

int SphereShape::vf18(const RVec3* from, const RVec3* dir, f32* t, RVec3* n) {
    u8* p = (u8*)this;
    return fn_8019DDFC((const f32*)from, (const f32*)dir, t, (f32*)n, (f32*)(p + 0x34), this->radius);
}

void SphereShape::vf38(RVec3* lo, RVec3* hi) const {
    RVec3 c;
    RVec3 e;
    f32 r = this->radius;
    e.x = r;
    e.y = r;
    e.z = r;
    c.x = this->f34;
    c.y = this->f38;
    c.z = this->f3C;
    MulAdd(&lo->x, &c.x, &e.x, lbl_805622DC);
    MulAdd(&hi->x, &c.x, &e.x, lbl_805622E0);
}

void SphereShape::vf4C(void) {
    u8* p = (u8*)this;
    f32 r = this->radius;
    r = r > 0.2f ? r : 0.2f;
    fn_801E3FC8(p + 0x34, r, lbl_8055E598.value);
}

void fn_802E30C8(const char* fmt, ...);

// Collides flat sphere `p` against volume `o` by its type tag.
void FlatSphereShape::vf10(u8* o) {
    u8* p = (u8*)this;
    switch (((CollShape*)o)->type) {
    case 'RAGD':
        break;
    case 'FLSR':
        fn_8019D19C(p, o);
        break;
    case 'SPHR':
        fn_8019CE4C(o, p);
        break;
    case 'TUBE':
        fn_8019CF60(o, p);
        break;
    case 'BOX ':
        fn_8019D06C(p, o, 0, 0);
        break;
    case 'GRUP':
    case 'POLY':
        ((VGroup*)o)->v10(p);
        break;
    case 'RING':
        fn_8019D378(p, o);
        break;
    default: {
        char tag[5] = "fooo";
        *(s32*)tag = ((CollShape*)o)->type;
        fn_802E30C8("flatsphere: Can't collide with type %s", tag);
        break;
    }
    }
}

void FlatSphereShape::vf14(void) {}

// Re-centres the tube on its attachment (offset by 0x58 / lift), then refreshes it.
void AttachedSphereShape::vf0C(void) {
    u8* p = (u8*)this;
    u8* o = ((u8*&)this->owner);
    u8* m = this->attach;
    PVec3A t = *(PVec3A*)(m + 0x30);
    f32 lift = 0.06f + this->radius + F32(m, 0xC);
    t.v[0] = t.v[0] - F32(o, 0x30) + this->f58;
    t.v[1] = t.v[1] - F32(o, 0x34) + lift;
    t.v[2] = t.v[2] - F32(o, 0x38);
    this->center.z = t.v[2];
    this->center.x = t.v[0];
    this->center.y = t.v[1];
    if (o) {
        fn_8019EECC(o, (RVec3*)&this->center);
    }
    ((SphereShape*)p)->SphereShape::vf0C();
}

// Projects v onto the plane of p's frame row 1 (the result feeds a debug view that is compiled out).
void AttachedSphereShape::vf24(const RVec3* v, const RVec3* at, int kind, void* ctx, void* by) {
    ((CollShape*)((u8*)this))->CollShape::vf24(v, at, kind, ctx, by);
    f32 d = -PSVECDotProduct(&((CFrame*)((u8*&)this->frame))->m[3], v);
    CFrame* f = (CFrame*)((u8*&)this->frame);
    f32 x = v->x + f->m[3] * d;
    f32 y = v->y + f->m[4] * d;
    f32 z = v->z + f->m[5] * d;
    BoxCorner c(x, y, z);
}

void AttachedSphereShape::vf14(void) {
    u8* p = (u8*)this; ((SphereShape*)p)->SphereShape::vf14(); }

static inline void InitVolume(u8* s, u32 id, u32 owner) {
    U32(s, 0xC) = id;
    U32(s, 0x4) = owner;
}


// Box volume (half sizes hx, hy, hz): a sphere bounding the largest half size, then the box fields.
u8* fn_8019F564(u8* p, u32 frame, const RVec3* c, u32 owner, f32 k, f32 skin, f32 a, f32 hx, f32 hy, f32 hz) {
    f32 big = hy > hz ? hy : hz;
    big = hx > big ? hx : big;
    f32 cy;
    f32 cz = c->z;
    cy = c->y;
    f32 cx = c->x;
    InitVolume(p, frame, owner);
    ((BoxSphereShape*)p)->type = 'SPHR';
    U32(p, 0x8) = 0;
    ((BoxSphereShape*)p)->stiff = k;
    ((BoxSphereShape*)p)->skin = skin;
    ((BoxSphereShape*)p)->surface = 100;
    ((BoxSphereShape*)p)->x1C = 0;
    *(void**)p = __vt__11SphereShape;
    ((BoxSphereShape*)p)->radius = 0.5f * big;
    ((BoxSphereShape*)p)->f34 = 0.0f;
    ((BoxSphereShape*)p)->f38 = 0.0f;
    ((BoxSphereShape*)p)->f3C = 0.0f;
    ((BoxSphereShape*)p)->f40 = 0.0f;
    ((BoxSphereShape*)p)->f44 = 0.0f;
    ((BoxSphereShape*)p)->f48 = 0.0f;
    ((BoxSphereShape*)p)->b4C = 0;
    ((BoxSphereShape*)p)->f50 = 0.0f;
    ((BoxSphereShape*)p)->center.x = cx;
    ((BoxSphereShape*)p)->center.y = cy;
    ((BoxSphereShape*)p)->center.z = cz;
    if (owner) {
        fn_8019EECC((u8*)owner, (RVec3*)(p + 0x28));
    }
    *(void**)p = __vt__14BoxSphereShape;
    ((BoxSphereShape*)p)->halfBox.x = a;
    ((BoxSphereShape*)p)->halfBox.y = 0.5f * hx;
    ((BoxSphereShape*)p)->halfBox.z = 0.5f * hy;
    ((BoxSphereShape*)p)->cache[0] = 0.5f * hz;
    ((BoxSphereShape*)p)->cache[1] = 0.0f;
    ((BoxSphereShape*)p)->cache[2] = 0.0f;
    ((BoxSphereShape*)p)->cache[3] = 0.0f;
    ((BoxSphereShape*)p)->cache[4] = 0.0f;
    ((BoxSphereShape*)p)->cache[5] = 0.0f;
    ((BoxSphereShape*)p)->cache[6] = 0.0f;
    ((BoxSphereShape*)p)->cache[7] = 0.0f;
    ((BoxSphereShape*)p)->cache[8] = 0.0f;
    ((BoxSphereShape*)p)->cache[9] = 0.0f;
    ((BoxSphereShape*)p)->cache[10] = 0.0f;
    ((BoxSphereShape*)p)->cache[11] = 0.0f;
    ((BoxSphereShape*)p)->cache[12] = 0.0f;
    ((BoxSphereShape*)p)->cache[13] = 0.0f;
    ((BoxSphereShape*)p)->cache[14] = 0.0f;
    ((BoxSphereShape*)p)->cache[15] = 0.0f;
    ((BoxSphereShape*)p)->cache[16] = 0.0f;
    ((BoxSphereShape*)p)->cache[17] = 0.0f;
    ((BoxSphereShape*)p)->cache[18] = 0.0f;
    ((BoxSphereShape*)p)->cache[19] = 0.0f;
    ((BoxSphereShape*)p)->cache[20] = 0.0f;
    ((BoxSphereShape*)p)->cache[21] = 0.0f;
    ((BoxSphereShape*)p)->cache[22] = 0.0f;
    ((BoxSphereShape*)p)->cache[23] = 0.0f;
    ((BoxSphereShape*)p)->fC0 = 0.0f;
    return p;
}

void BoxSphereShape::vf28(void) {
    u8* p = (u8*)this;
    ((SphereShape*)p)->SphereShape::vf28();
    for (int i = 0; i < 24; i++) {
        ((SphCache*)p)->v[i] = 0.0f;
    }
}

extern const f32 lbl_80562300;
extern const f32 lbl_80562380;  // 1.5
extern const f32 lbl_805623AC;  // 10
extern const f32 lbl_805622E8;  // 0.5
extern f32 lbl_8055E3C8;  // race time
extern "C" double fn_801E9DDC(u8* hint, f32 x, f32 z);
extern "C" void fn_801B3B24(u8* p, const RVec3* at, const RVec3* v);
extern "C" double fmod(double x, double y);
extern "C" double sin(double x);

// A box volume's per-corner slip accumulators.
struct BoxSlip {
    u8 pad0[0x64];  // offset 0x0
    RVec3 slip[8];  // offset 0x64
};

static inline f32 Lerp_hm(f32 lo, f32 range, f32 t) { return lo + t * range; }  /* Helper name unproven */
// Box (half extents at 0x58) on the ground: each of the eight corners below the track height gets a spring
// impulse, clamped friction (with its own slip accumulator at 0x64) and the bounce event. In water (surface 7)
// the box floats instead: buoyancy from the submerged depth on a bobbing water line, and drag.
#pragma push
#pragma opt_loop_invariants off  /* Pragma unproven: helps but not exact (PRAGMA_AUDIT_2026-11-15) */
void BoxSphereShape::vf14(void) {
    int i;
    RVec3 at;
    RVec3 pos;
    RVec3 nrm;
    RVec3 v;
    int surf;
    u8* p = (u8*)this;
    RigidBody* rb = (RigidBody*)owner;
    at.x = this->f34;
    at.y = this->f38;
    at.z = this->f3C;
    if (fn_801E9F08((u8*)rb->hint, &at.x, &pos, &nrm, &surf, this->radius + lbl_80562380)) {
        if (surf == 7) {
            if (!this->b4C) {
                rb->VelAt(&v, &pos);
                fn_801B3B24(p, &pos, &v);
                this->b4C = 1;
            }
            this->f50 = pos.y;
        } else {
            this->b4C = 0;
            const RVec3* h = (const RVec3*)(p + 0x58);
            BoxCorner corners[8] = {
                BoxCorner(-h->x, -h->y, -h->z), BoxCorner(h->x, -h->y, -h->z),
                BoxCorner(-h->x, h->y, -h->z),  BoxCorner(h->x, h->y, -h->z),
                BoxCorner(-h->x, -h->y, h->z),  BoxCorner(h->x, -h->y, h->z),
                BoxCorner(-h->x, h->y, h->z),   BoxCorner(h->x, h->y, h->z),
            };
            for (i = 0; i < 8; i++) {
                RVec3 w;
                RVec3 d;
                RVec3 vel;
                RVec3 vt;
                RVec3 imp;
                RVec3 f;
                RVec3 fr;
                CFrame* m = (CFrame*)((u8*&)this->frame);
                MulVec(&w, (RVec3*)corners + i, m);
                w.x = w.x + m->t.x;
                w.y = w.y + m->t.y;
                w.z = w.z + m->t.z;
                pos.x = w.x;
                pos.z = w.z;
                pos.y = fn_801E9DDC((u8*)rb->hint, w.x, w.z);
                d.x = pos.x - w.x;
                d.y = pos.y - w.y;
                f32 zv = pos.z;
                d.z = zv - w.z;
                f32 depth = PSVECDotProduct(&d, &nrm);
                f32* acc = &((BoxSlip*)p)->slip[i].x;
                if (depth > lbl_805622D0) {
                    rb->VelAt(&vel, &w);
                    f32 vn = PSVECDotProduct(&vel, &nrm);
                    vt.x = vel.x + nrm.x * -vn;
                    vt.y = vel.y + nrm.y * -vn;
                    vt.z = vel.z + nrm.z * -vn;
                    acc[0] = acc[0] + vt.x * lbl_8056231C;
                    acc[1] = acc[1] + vt.y * lbl_8056231C;
                    acc[2] = acc[2] + vt.z * lbl_8056231C;
                    f32 am = PSVECMag(acc);
                    if (am > 0.02f) {
                        f32 s = 0.02f / am;
                        acc[0] *= s;
                        acc[1] *= s;
                        acc[2] *= s;
                    }
                    f32 k = depth * this->stiff * (vn < lbl_805622D0 ? lbl_805622E0 : lbl_80562300);
                    imp.x = nrm.x * k;
                    imp.y = nrm.y * k;
                    imp.z = nrm.z * k;
                    f32 c = -3.0f * F32(rb, 0xD4);
                    f.x = vt.x * c;
                    f.y = vt.y * c;
                    f.z = vt.z * c;
                    f32 g = -30.0f * F32(rb, 0xD4);
                    f.x = Lerp_hm(f.x, g, acc[0]);
                    f.y = f.y + acc[1] * g;
                    f.z = f.z + acc[2] * g;
                    f32 fm = PSVECMag(&f.x);
                    f32 lim = k * this->halfBox.x;
                    if (fm > lim) {
                        f32 s = lim / fm;
                        f.x *= s;
                        f.y *= s;
                        f.z *= s;
                    }
                    imp.x += f.x;
                    imp.y += f.y;
                    imp.z += f.z;
                    VE(p)->v1C(&imp, &w, surf, 0);
                    f32 b = fn_8019DF88((u8*)rb + 0x60, &vel, (u8*)rb + 0x148, &w, &nrm.x, F32(rb, 0xD4), lbl_805622D0);
                    if (b > lbl_805622D0) {
                        fr.x = nrm.x * b;
                        fr.y = nrm.y * b;
                        fr.z = nrm.z * b;
                        f32 ww = lbl_805622D4 > depth ? lbl_805622D4 : depth;
                        VE(p)->v20(0, &fr, &w, surf, 0, 0, ww);
                    }
                } else {
                    ((BoxSlip*)p)->slip[i].x = lbl_805622D0;
                    ((BoxSlip*)p)->slip[i].y = lbl_805622D0;
                    ((BoxSlip*)p)->slip[i].z = lbl_805622D0;
                }
            }
        }
    } else {
        for (i = 0; i < 24; i++) {
            ((SphCache*)p)->v[i] = lbl_805622D0;
        }
    }
    if (this->b4C) {
        RVec3 bv;
        RVec3 drag;
        RVec3 up;
        f32 sub = this->f50 - this->f38 + this->radius;
        rb->VelAt(&bv, (RVec3*)(p + 0x34));
        f32 ph = fmod(this->f34 + 6.2831855f * lbl_8055E3C8 / 2.0f, 6.2831854820251465);
        f32 s = sin(ph);
        sub += 0.0254f * (lbl_805622E0 + s);
        if (sub > lbl_805622D0) {
            f32 r = this->radius;
            f32 area = 3.1415927f * (r * r);
            f32 vol = 0.4f * (4.1887903f * r * (r * r));
            f32 dk = -area * (500.0f + lbl_805623AC * PSVECMag(&bv.x));
            drag.x = bv.x * dk;
            drag.y = bv.y * dk;
            drag.z = bv.z * dk;
            rb->AddForceAt(&drag, (RVec3*)(p + 0x34));
            f32 lift = 1000.0f + lbl_8056232C * sub;
            sub = MinF(sub, this->radius * lbl_8056234C);
            up.y = 9.81f * (lift * (vol * sub));
            up.x = lbl_805622D0;
            up.z = lbl_805622D0;
            rb->AddForceAt(&up, (RVec3*)(p + 0x34));
        } else {
            this->b4C = 0;
        }
    }
}
#pragma pop

void OffsetSphereShape::vf1C(void) {
    u8* p = (u8*)this; ((CollShape*)p)->CollShape::vf1C(); }

void OffsetSphereShape::vf28(void) {
    u8* p = (u8*)this;
    ((SphereShape*)p)->SphereShape::vf28();
    this->center.x = this->localOffset.x;
    this->center.y = this->localOffset.y;
    this->center.z = this->localOffset.z;
    this->b78 = 0;
}

SphereGroup::SphereGroup(u32 a, u32 b) : CollShape((const CFrame*)a, (CollVolume*)b, 'GRUP', 0.0f, 0.0f) {
    u8* p = (u8*)this;
    this->f170 = 0.0f;
    this->f178 = 0.0f;
    this->f16C = 0.0f;
    this->f168 = 0.0f;
    this->f164 = 0.0f;
    ((u32&)this->count) = 0;
    ((u32&)this->child) = 0;
    this->f180 = -1;
}

SphereGroup::~SphereGroup(void) {
    for (int i = 0; i < S32(this, 0x174); i++) {
        delete ((SphGroupDel*)this)->vols[i];
    }
    delete *(VDel**)((u8*)this + 0x17C);
}

void SphereGroup::vf0C(void) {
    u8* p = (u8*)this; VE(this->child)->v0C(); }

void fn_801A0160(u8* p) {
    if (lbl_8055E3C4 != S32(p, 0x180)) {
        S32(p, 0x180) = lbl_8055E3C4;
        for (int i = 0; i < S32(p, 0x174); i++) {
            ((SphGroup*)p)->vols[i]->v0C();
        }
    }
}

void SphereGroup::vf38(RVec3* lo, RVec3* hi) const {
    RVec3 c;
    RVec3 e;
    f32 r = this->f178;
    u8* o = ((u8*&)this->frame);
    e.x = r;
    e.y = r;
    e.z = r;
    c.x = F32(o, 0x24);
    c.y = F32(o, 0x28);
    c.z = F32(o, 0x2C);
    MulAdd(&lo->x, &c.x, &e.x, lbl_805622DC);
    MulAdd(&hi->x, &c.x, &e.x, lbl_805622E0);
}

void fn_801A0378(u8* p, u8* s);
extern const f32 lbl_805622D0;  // 0.0

// Adds a sphere part (radius r) at c to the group.
void fn_801A0258(u8* grp, const RVec3* c, const void*, f32 r, f32 s0, f32 s1, f32, f32) {
    u8* s = (u8*)operator new(0x54, "SphereGroupPart");
    if (s) {
        f32 cz = c->z;
        f32 cy = c->y;
        f32 cx = c->x;
        InitVolume(s, U32(grp, 0xC), U32(grp, 0x4));
        U32(s, 0x8) = 0;
        ((SphereShape*)s)->type = 'SPHR';
        ((SphereShape*)s)->stiff = s0;
        ((SphereShape*)s)->skin = s1;
        ((SphereShape*)s)->surface = 100;
        ((SphereShape*)s)->x1C = 0;
        *(void**)s = __vt__11SphereShape;
        ((SphereShape*)s)->radius = r;
        ((SphereShape*)s)->f34 = lbl_805622D0;
        ((SphereShape*)s)->f38 = lbl_805622D0;
        ((SphereShape*)s)->f3C = lbl_805622D0;
        ((SphereShape*)s)->f40 = lbl_805622D0;
        ((SphereShape*)s)->f44 = lbl_805622D0;
        ((SphereShape*)s)->f48 = lbl_805622D0;
        ((SphereShape*)s)->b4C = 0;
        ((SphereShape*)s)->f50 = lbl_805622D0;
        ((SphereShape*)s)->center.x = cx;
        ((SphereShape*)s)->center.y = cy;
        ((SphereShape*)s)->center.z = cz;
        if (U32(s, 0x4)) {
            fn_8019EECC((u8*)U32(s, 0x4), (RVec3*)(s + 0x28));
        }
    }
    fn_801A0378(grp, s);
}

void fn_801A0378(u8* p, u8* s) {
    SphGroupAll* g = (SphGroupAll*)p;
    g->vols[g->count] = (VSet*)s;
    f32* e = g->sph[g->count];
    e[0] = ((SphereShape*)s)->center.x;
    e[1] = ((SphereShape*)s)->center.y;
    e[2] = ((SphereShape*)s)->center.z;
    e[3] = ((SphereShape*)s)->radius;
    g->vols[g->count]->SetA(g->setA);
    f32 m = PSVECMag((f32*)(s + 0x28));
    f32 r = 0.5f + (m + ((SphereShape*)s)->radius);
    if (r > g->radius2) {
        g->radius2 = r;
        g->radius = r;
    }
    g->count++;
}

void fn_801A0444(u8* p, u8* q) {
    PTR(p, 0x17C) = q;
    F32(p, 0x164) = F32(q, 0x24);
    F32(p, 0x168) = F32(q, 0x28);
    F32(p, 0x16C) = F32(q, 0x2C);
}


extern const f32 lbl_805623B8;  // 0.3048 (feet to metres)
extern const f32 lbl_805623BC;  // 1.1

// Squared distance between the frame centres of p and q.
static inline f32 CentreDist2(u8* p, u8* q) {
    RVec3 d;
    CFrame* fq = (CFrame*)PTR(q, 0xC);
    CFrame* fp = (CFrame*)((u8*&)((SphereGroup*)p)->frame);
    d.x = fp->t.x - fq->t.x;
    d.y = fp->t.y - fq->t.y;
    d.z = fp->t.z - fq->t.z;
    return PSVECDotProduct(&d, &d);
}

// d = p's frame centre relative to box q's centre.
static inline void FromBoxCentre(RVec3* d, u8* p, u8* q) {
    RVec3 c;
    fn_801A1F44(&c, q);
    CFrame* fp = (CFrame*)((u8*&)((SphereGroup*)p)->frame);
    d->x = fp->t.x - c.x;
    d->y = fp->t.y - c.y;
    d->z = fp->t.z - c.z;
}

// Collides sphere group p against volume q by q's type tag: groups (the heavier side drives), boxes (each member
// in box space, then the owning car's wheels clamped onto the box), spheres, tubes, rings, ragdolls, polygon
// volumes and polygon meshes (each member, then the car's wheels against the volume or mesh).

static inline f32 GroupBodyMass(const u8* body) { return F32(body, 0xD4); }
static inline void HitGroup(u8* p, u8* q) {
    u8* pb;
    u8* qb = PTR(q, 0x4);
    if (qb && (pb = ((u8*&)((SphereGroup*)p)->owner)) != 0 && GroupBodyMass(pb) < GroupBodyMass(qb)) {
        ((VGroup*)q)->v10(p);
        return;
    }
    f32 dd = CentreDist2(p, q);
    f32 r = ((SphereGroup*)p)->f178 + F32(q, 0x178);
    if (dd < r * r) {
        fn_801A0160(q);
        for (int i = 0; i < S32(q, 0x174); i++) {
            fn_8019BD54((u8*)((SphGroup*)q)->vols[i], ((SphereGroup*)p)->child, 0, 0);
        }
    }
}

void SphereGroup::vf10(u8* q) {
    u8* p = (u8*)this;
    RVec3* h;
    s32 type = S32(q, 0x18);
    if (type == 'GRUP') {
        HitGroup(p, q);
    } else if (type == 'BOX ') {
        RVec3 d;
        FromBoxCentre(&d, p, q);
        if ((f32)__fabs(PSVECDotProduct(&d, PTR(q, 0xC))) < F32(q, 0x24) + this->f178 &&
            (f32)__fabs(PSVECDotProduct(&d, PTR(q, 0xC) + 0x18)) < F32(q, 0x2C) + this->f178) {
            u8* pb;
            CFrame* f = (CFrame*)PTR(q, 0xC);
            pb = ((u8*&)this->owner);
            RVec3 l;
            l.x = F32(pb, 0x60) - f->t.x;
            l.y = F32(pb, 0x64) - f->t.y;
            l.z = F32(pb, 0x68) - f->t.z;
            f32 x = Dot3(l.x, f->m[0], l.y, f->m[1], l.z, f->m[2]);
            f32 y = Dot3(l.x, f->m[3], l.y, f->m[4], l.z, f->m[5]);
            f32 z = Dot3(l.x, f->m[6], l.y, f->m[7], l.z, f->m[8]);
            l.x = x;
            l.y = y;
            l.z = z;
            fn_801A0160(p);
            for (int i = 0; i < this->count; i++) {
                fn_8019BD54((u8*)((SphGroup*)p)->vols[i], q, 0, &l);
            }
            u8* car;
            if (S32(q, 0x20) < 8 && IsCarObj(car = ((u8*&)this->owner))) {
                int id = S32(car, 0x1EC);
                for (int w = 0; w < 4; w++) {
                    PVec3A wp;
                    RVec3 bl;
                    RVec3 bw;
                    RVec3 n;
                    fn_801BEAEC(id, w, &wp);
                    CFrame* m = (CFrame*)PTR(q, 0xC);
                    bl.x = wp.v[0] - m->t.x;
                    bl.y = wp.v[1] - m->t.y;
                    bl.z = wp.v[2] - m->t.z;
                    f32 bx = Dot3(bl.x, m->m[0], bl.y, m->m[1], bl.z, m->m[2]);
                    f32 by = Dot3(bl.x, m->m[3], bl.y, m->m[4], bl.z, m->m[5]);
                    f32 bz = Dot3(bl.x, m->m[6], bl.y, m->m[7], bl.z, m->m[8]);
                    bl.x = bx;
                    bl.y = by;
                    bl.z = bz;
                    f32 ex = F32(q, 0x24);
                    f32 ey = F32(q, 0x28);
                    f32 ez = F32(q, 0x2C);
                    bl.x = ClampExt(bl.x, ex);
                    bl.y = ClampExt(bl.y, ey);
                    bl.z = ClampExt(bl.z, ez);
                    m = (CFrame*)PTR(q, 0xC);
                    MulVec(&bw, &bl, m);
                    bw.x = bw.x + m->t.x;
                    bw.y = bw.y + m->t.y;
                    bw.z = bw.z + m->t.z;
                    n.x = wp.v[0] - bw.x;
                    n.y = wp.v[1] - bw.y;
                    n.z = wp.v[2] - bw.z;
                    f32 len = PSVECMag(&n.x);
                    if (len > 0.01f) {
                        f32 inv = lbl_805622E0 / len;
                        n.x *= inv;
                        n.y *= inv;
                        n.z *= inv;
                        fn_801BEB44(id, w, &bw, (PVec3A*)&n);
                    }
                }
            }
        }
    } else if (type == 'SPHR' || type == 'FLSR') {
        PVec3A c = *(PVec3A*)(this->child + 0x38);
        u8* pb = ((u8*&)this->owner);
        if (pb) {
            c.v[0] = c.v[0] + F32(pb, 0x30);
            c.v[1] = c.v[1] + F32(pb, 0x34);
            c.v[2] = c.v[2] + F32(pb, 0x38);
        }
        if (S32(q, 0x18) == 'SPHR') {
            fn_8019BD54(q, this->child, (RVec3*)&c, 0);
        } else {
            fn_8019D06C(q, this->child, (RVec3*)&c, 0);
        }
    } else if (type == 'TUBE') {
        fn_8019EA7C(q, this->child);
    } else if (type == 'RING') {
        fn_801A0160(p);
        for (int i = 0; i < this->count; i++) {
            fn_8019C854((u8*)((SphGroup*)p)->vols[i], q);
        }
    } else if (type == 'RAGD') {
        ((VGroup*)q)->v10(p);
    } else if (type == 'POLY') {
        int id;
        u8* car;
        fn_801A0160(p);
        for (int i = 0; i < this->count; i++) {
            ((VGroup*)q)->v10((u8*)((SphGroup*)p)->vols[i]);
        }
        car = ((u8*&)this->owner);
        if (IsCarObj(car)) {
            id = S32(car, 0x1EC);
            for (int w = 0; w < 4; w++) {
                PVec3A wp;
                RVec3 n;
                RVec3 hit;
                RVec3 vel;
                f32 depth;
                fn_801BEAEC(id, w, &wp);
                vel.x = F32(car, 0x3C) * lbl_805623B8;
                vel.y = F32(car, 0x40) * lbl_805623B8;
                vel.z = F32(car, 0x44) * lbl_805623B8;
                if (fn_80198F64(wp.v, (u32)q, &hit.x, &depth, &n.x, &vel.x, lbl_805622E0)) {
                    fn_801BEB44(id, w, &hit, (PVec3A*)&n);
                }
            }
        }
    } else if (type == 'PMSH') {
        u8* car;
        fn_801A0160(p);
        for (int i = 0; i < this->count; i++) {
            ((VGroup*)q)->v10((u8*)((SphGroup*)p)->vols[i]);
        }
        car = ((u8*&)this->owner);
        if (IsCarObj(car)) {
            int id = S32(car, 0x1EC);
            for (int w = 0; w < 4; w++) {
                RVec3 hits[64];
                PVec3A wp;
                PVec3A c;
                PVec3A d;
                int n;
                fn_801BEAEC(id, w, &wp);
                c = wp;
                c.v[0] = c.v[0] + F32(car, 0x48) * lbl_805623BC;
                c.v[1] = c.v[1] + F32(car, 0x4C) * lbl_805623BC;
                c.v[2] = c.v[2] + F32(car, 0x50) * lbl_805623BC;
                n = 64;
                fn_801CF080(PTR(q, 0x24), (RVec3*)c.v, hits, &n, U32(q, 0xC), lbl_8056234C);
                for (int k = 0; k < n; k++) {
                    h = &hits[k];
                    d.v[0] = c.v[0] - h->x;
                    d.v[1] = c.v[1] - h->y;
                    d.v[2] = c.v[2] - h->z;
                    f32 len = PSVECMag(d.v);
                    if (len > lbl_805622D4) {
                        f32 inv = lbl_805622E0 / len;
                        d.v[0] *= inv;
                        d.v[1] *= inv;
                        d.v[2] *= inv;
                        fn_801BEB44(id, w, h, &d);
                    }
                }
            }
        }
    }
}

// Per-frame update of the sphere group; with a track mesh loaded and a car owner, each of its 4 wheels
// (moved by the car's velocity) is tested against the mesh and every hit reported to the car.
void SphereGroup::vf14(void) {
    RVec3 hits[64];
    PVec3A wp;
    PVec3A c;
    PVec3A d;
    int n;
    u8* car;
    fn_801A0160(((u8*)this));
    for (int i = 0; i < this->count; i++) {
        ((SphGroup*)((u8*)this))->vols[i]->v14();
    }
    if (lbl_8055E5A0) {
        car = ((u8*&)this->owner);
        if (IsCarObj(car)) {
            int id = S32(car, 0x1EC);
            for (int w = 0; w < 4; w++) {
                fn_801BEAEC(id, w, &wp);
                c = wp;
                c.v[0] = c.v[0] + F32(car, 0x48) * lbl_805622E0;
                c.v[1] = c.v[1] + F32(car, 0x4C) * lbl_805622E0;
                c.v[2] = c.v[2] + F32(car, 0x50) * lbl_805622E0;
                n = 64;
                fn_801CF080(lbl_8055E5A0, (RVec3*)c.v, hits, &n, 0, lbl_8056234C);
                for (int k = 0; k < n; k++) {
                    RVec3* h = &hits[k];
                    d.v[0] = c.v[0] - h->x;
                    d.v[1] = c.v[1] - h->y;
                    d.v[2] = c.v[2] - h->z;
                    f32 len = PSVECMag(d.v);
                    if (len > 0.001f) {
                        f32 inv = lbl_805622E0 / len;
                        d.v[0] *= inv;
                        d.v[1] *= inv;
                        d.v[2] *= inv;
                        fn_801BEB44(id, w, h, &d);
                    }
                }
            }
        }
    }
}

// Sweeps a ray against the group: a cheap slab test on the bounding sphere, then each member.
int SphereGroup::vf18(const RVec3* from, const RVec3* dir, f32* t, RVec3* out) {
    const f32* c = (const f32*)(((u8*&)this->frame) + 0x24);
    f32 d[3];
    d[0] = c[0] - ((const f32*)from)[0];
    d[1] = c[1] - ((const f32*)from)[1];
    d[2] = c[2] - ((const f32*)from)[2];
    f32 proj = PSVECDotProduct(d, ((const f32*)dir));
    f32 rad = this->f178;
    if (proj >= -rad && proj < *t + rad) {
        fn_801A0160(((u8*)this));
        int hit = 0;
        for (int i = 0; i < this->count; i++) {
            u8* s = (u8*)((SphGroupDel*)((u8*)this))->vols[i];
            if (fn_8019DDFC((f32*)((const f32*)from), (f32*)((const f32*)dir), t, ((f32*)out), (f32*)(s + 0x34), F32(s, 0x24))) {
                hit = 1;
            }
        }
        return hit;
    }
    return 0;
}

void SphereGroup::vf28(void) {
    u8* q = ((u8*)this);
    for (int i = 0; i < this->count; i++) {
        VE(PTR(q, 0x24))->v28();
        q += 4;
    }
    this->f180 = -1;
}

void SphereGroup::vf44(u32 v) {
    this->x1C = v;
    ((VSet*)this->child)->SetB(v);
    for (int i = 0; i < this->count; i++) {
        ((SphGroupSet*)((u8*)this))->vols[i]->SetB(v);
    }
}

void CollShape::vf44(u32 v) {
    u8* p = (u8*)this; this->x1C = v; }

void SphereGroup::SetSurface(int v) {
    U32(((u8*)this), 0x20) = ((u32)v);
    ((VSet*)this->child)->SetA(((u32)v));
    for (int i = 0; i < this->count; i++) {
        ((SphGroupSet*)((u8*)this))->vols[i]->SetA(((u32)v));
    }
}

void SphereGroup::vf4C(void) {
    ((VEx*)this->child)->v4C();
    for (int i = 0; i < this->count; i++) {
        ((SphGroup*)((u8*)this))->vols[i]->v4C();
    }
}

// Sphere group layout (partial).
struct SphPart {
    f32 x, y, z;  // offset 0x0, local centre
    f32 r;        // offset 0xC
};

struct SphGroupData {
    u8 pad0[0x24];
    u8* vols[16];       // offset 0x24
    SphPart parts[16];  // offset 0x64
    f32 c[3];           // offset 0x164
    f32 rad;            // offset 0x170
    s32 count;          // offset 0x174
    f32 srad;           // offset 0x178
    u8* bound;          // offset 0x17C
};

// Scales the group and its members by s.
void fn_801A1474(SphGroupData* g, f32 s) {
    g->srad = g->rad * s;
    for (int i = 0; i < g->count; i++) {
        SphPart* e = &g->parts[i];
        RVec3 t;
        t.x = e->x * s;
        t.y = e->y * s;
        t.z = e->z * s;
        *(RVec3*)(g->vols[i] + 0x28) = t;
        F32(g->vols[i], 0x24) = e->r * s;
    }
    f32 x = g->c[0] * s;
    f32 y = g->c[1] * s;
    f32 z = g->c[2] * s;
    u8* b = g->bound;
    F32(b, 0x24) = x;
    F32(b, 0x28) = y;
    F32(b, 0x2C) = z;
}

TubeBase::TubeBase(u32 a, u32 swap) : CollShape(a, swap) {
    u8* p = (u8*)this;
    if (swap) {
        Swap32(p + 0x24);
        Swap32(p + 0x28);
        if (this->surface == 0x3EB) {
            this->rad += 3.35f;
        }
    }
}

// Ray from + t*dir against the capsule's side (axis clamped to +-half length 0x28, radius 0x24):
// the hit if it lies in (0, *t), with its normal.
static inline f32 ClampF(f32 v, f32 lo, f32 hi) {
    v = hi < v ? hi : v;
    return v > lo ? v : lo;
}

int TubeBase::vf18(const RVec3* from, const RVec3* dir, f32* t, RVec3* n) {
    PVec3A hit;
    PVec3A on;
    f32 tr = 0.0f;
    f32 s = 0.0f;
    u8* m = ((u8*&)this->frame);
    f32* axis = (f32*)(m + 0x18);
    f32* org = (f32*)(m + 0x24);
    if (fn_8019D484(((const f32*)from), ((const f32*)dir), org, axis, &tr, &s)) {
        s = ClampF(s, -this->half, this->half);
        if (tr > 0.0f && tr < *t) {
            MulAdd(hit.v, ((const f32*)from), ((const f32*)dir), tr);
            MulAdd(on.v, org, axis, s);
            f32 d2 = PSVECSquareDistance(hit.v, on.v);
            f32 r = this->rad;
            if (d2 < r * r) {
                ((f32*)n)[0] = hit.v[0] - on.v[0];
                ((f32*)n)[1] = hit.v[1] - on.v[1];
                ((f32*)n)[2] = hit.v[2] - on.v[2];
                f32 len = PSVECMag(((f32*)n));
                f32 k = lbl_805622D4;
                f32 inv = 1.0f / (k > len ? k : len);
                ((f32*)n)[0] *= inv;
                ((f32*)n)[1] *= inv;
                ((f32*)n)[2] *= inv;
                *t = tr;
                return 1;
            }
        }
    }
    return 0;
}

// Axis + centre of a tube volume (plain data, copied as words).
struct TubeSeg {
    f32 dir[3];  // offset 0x0
    f32 mid[3];  // offset 0xC
};

void fn_801E41D8(f32* a, f32* b, u32 color, f32 r);


// Debug-draws the tube as a capsule.
void TubeBase::vf4C(void) {
    u8* p = (u8*)this;
    TubeSeg s = *(TubeSeg*)(((u8*&)this->frame) + 0x18);
    f32 a[3];
    f32 b[3];
    MulAdd(a, s.mid, s.dir, this->half);
    MulAdd(b, s.mid, s.dir, -this->half);
    fn_801E41D8(a, b, 0x40FFFF00, lbl_8056234C * this->rad);
}

TubeShape::TubeShape(u32 a, u32 swap) : TubeBase(a, swap) {}

// Axis-aligned bounds of the volume: both axis end points (org +- axis * k), widened by k.
// Helper name unproven: arguments read right to left, stores go x, y, z.
static inline void SetV3(void* o_, f32 x, f32 y, f32 z) {
    f32* o = (f32*)o_;
    o[0] = x;
    o[1] = y;
    o[2] = z;
}
static inline void SetLowerXY(RVec3* out, const RVec3& a, f32 k, f32 y) {
    out->x = a.x - k;
    out->y = y;
}

void TubeShape::vf38(RVec3* lo, RVec3* hi) const {
    PVec3A org;
    PVec3A axis;
    RVec3 a;
    RVec3 b;
    u8* m = ((u8*&)this->frame);
    f32 k = this->rad;
    org = *(PVec3A*)(m + 0x24);
    axis = *(PVec3A*)(m + 0x18);
    MulAdd(&a.x, org.v, axis.v, k);
    MulAdd(&b.x, org.v, axis.v, -k);
    {
        RVec3 mn;
        RVec3 la;
        RVec3 lb;
        SetV3(&lb, b.x - k, b.y - k, b.z - k);
        const struct { f32 y; } bias = {a.y - k};
        SetLowerXY(&la, a, k, bias.y);
        const f32 laZ = a.z - k;
        la.z = laZ;
        SetV(&mn, la.x < lb.x ? la.x : lb.x, MinF(la.y, lb.y), MinF(la.z, lb.z));
        *lo = mn;
    }
    {
        RVec3 mx;
        RVec3 ha;
        RVec3 hb;
        SetV3(&hb, b.x + k, b.y + k, b.z + k);
        SetV3(&ha, a.x + k, a.y + k, a.z + k);
        SetV(&mx, ha.x > hb.x ? ha.x : hb.x, MaxF(ha.y, hb.y), MaxF(ha.z, hb.z));
        *hi = mx;
    }
}

void fn_802E30C8(const char* fmt, ...);

// Collides `p` against volume `o` by its type tag.
void TubeShape::vf10(u8* o) {
    u8* p = (u8*)this;
    switch (((CollShape*)o)->type) {
    case 'RAGD':
    case 'POLY':
    case 'RING':
        break;
    case 'FLSR':
        fn_8019CF60(p, o);
        break;
    case 'SPHR':
        fn_8019CE3C(o, (u32)p);
        break;
    case 'BOX ':
        fn_8019EA7C(p, o);
        break;
    case 'TUBE':
        fn_8019DC04(p, o);
        break;
    case 'GRUP':
        fn_8019EA7C(p, ((SphereGroup*)o)->child);
        break;
    default:
        fn_802E30C8("Unsupported collision type: Tube-%x", ((CollShape*)o)->type);
        break;
    }
}

// Collides the tube's two end caps as spheres.
void TubeShape::vf14(void) {
    u8* p = (u8*)this;
    PVec3A a;
    PVec3A b;
    PVec3A zero;
    PVec3A dir = *(PVec3A*)(PTR(p, 0xC) + 0x18);
    PVec3A mid = *(PVec3A*)(PTR(p, 0xC) + 0x24);
    f32 h = F32(p, 0x28);
    MulAdd(a.v, mid.v, dir.v, h);
    MulAdd(b.v, mid.v, dir.v, -h);
    f32 r = F32(p, 0x24);
    zero.v[0] = lbl_805622D0;
    zero.v[1] = lbl_805622D0;
    zero.v[2] = lbl_805622D0;
    fn_8019AC44(p, (f32*)&a, (f32*)&zero, r);
    fn_8019AC44(p, (f32*)&b, (f32*)&zero, r);
}

void TubeShape::vf4C(void) {
    const u8* p = (const u8*)this;
    if (this->surface < 1000) {
        f32 x = this->rad;
        f32 z = this->half;
        fn_801E44F4(((u8*&)this->frame), x, x, z, 0x40FFFF00);
    } else {
        f32 x = this->rad;
        f32 z = this->half;
        fn_801E44F4(((u8*&)this->frame), x, x, z, 0x4000FF80);
    }
}

BoxShape::BoxShape(u32 a, u32 swap) : CollShape(a, swap) {
    u8* p = (u8*)this;
    if (swap) {
        Swap32((u8*)&this->half);
        Swap32(p + 0x28);
        Swap32(p + 0x2C);
        Swap32(p + 0x30);
        Swap32(p + 0x34);
    }
}

void BoxShape::vf10(u8* obj) {
    u8* a = (u8*)this;
    switch (S32(obj, 0x18)) {
    case 'FLSR':
        fn_8019D06C(obj, a, 0, 0);
        break;
    case 'SPHR':
        fn_8019BD54(obj, a, 0, 0);
        break;
    case 'TUBE':
        fn_8019EA7C(obj, a);
        break;
    case 'GRUP':
        ((VGroup*)obj)->v10(a);
        break;
    }
}

int BoxShape::vf18(const RVec3* a, const RVec3* b, f32* c, RVec3* out) {
    u8* p = (u8*)this;
    f32 z = this->half.z;
    f32 y = this->half.y;
    f32 x = this->half.x;
    if (fn_8019E2D0((RVec3*)((u8*)a), (RVec3*)((u8*)b), (f32*)((u8*)c), (CFrame*)((VEx*)p)->v50(), x, y, z)) {
        out->x = 0.0f;
        out->y = 1.0f;
        out->z = 0.0f;
        return 1;
    }
    return 0;
}

void fn_801A1F44(RVec3* out, u8* q) {
    u8* o = VE(q)->v50();
    *(PVec3A*)out = *(PVec3A*)(o + 0x24);
}

// Axis-aligned bounds of the box: its 8 corners (+-hx, +-hy, +-hz) through the volume's frame.
static inline void BoxCorners(RVec3* c, f32 hx, f32 hy, f32 hz) {
    f32 nz;
    f32 ny;
    f32 nx;
    ny = -hy;
    nx = -hx;
    nz = -hz;
    c[0].y = hy; c[0].x = hx; c[0].z = hz;
    c[1].x = nx; c[1].y = hy; c[1].z = hz;
    c[2].x = hx; c[2].y = ny; c[2].z = hz;
    c[3].x = nx; c[3].y = ny; c[3].z = hz;
    c[4].x = hx; c[4].y = hy; c[4].z = nz;
    c[5].x = nx; c[5].y = hy; c[5].z = nz;
    c[6].x = hx; c[6].y = ny; c[6].z = nz;
    c[7].x = nx; c[7].y = ny; c[7].z = nz;
}

void BoxShape::vf38(RVec3* lo, RVec3* hi) const {
    RVec3 c[8];
    BoxCorners(c, this->half.x, this->half.y, this->half.z);
    RVec3 w;
    *lo = ((CFrame*)VE(((const u8*)this))->v50())->t;
    *hi = ((CFrame*)VE(((const u8*)this))->v50())->t;
    for (int i = 0; i < 8; i++) {
        CFrame* m = (CFrame*)VE(((const u8*)this))->v50();
        MulVec(&w, &c[i], m);
        w.x = w.x + m->t.x;
        w.y = w.y + m->t.y;
        w.z = w.z + m->t.z;
        lo->x = lo->x < w.x ? lo->x : w.x;
        lo->y = lo->y < w.y ? lo->y : w.y;
        lo->z = lo->z < w.z ? lo->z : w.z;
        hi->x = MaxF(hi->x, w.x);
        hi->y = hi->y > w.y ? hi->y : w.y;
        hi->z = hi->z > w.z ? hi->z : w.z;
    }
}

void BoxShape::vf4C(void) {
    u8* p = (u8*)this;
    Xform* src = (Xform*)((VEx*)p)->v50();
    Xform x;
    x.rot = src->rot;
    x.pos = src->pos;
    const RVec3* c = (const RVec3*)&this->half;
    f32 cz = c->z;
    f32 cy = c->y;
    f32 cx = c->x;
    fn_801E44F4((u8*)&x, cx, cy, cz, 0x50000000);
}

OrientedBoxShape::OrientedBoxShape(u32 a, const RVec3* c, u32 b, f32 s0, f32 s1, f32 hx, f32 hy, f32 hz)
    : BoxShape((CollVolume*)b, (const RMtx*)a, s0, s1, hx - 0.2f, hy - 0.2f, hz - 0.2f) {
    F32(this, 0x38) = c->x;
    F32(this, 0x3C) = c->y;
    F32(this, 0x40) = c->z;
}

// Takes the frame's rotation (0x44), then the world centre (0x68) = frame * local centre (0x38).
void OrientedBoxShape::vf0C(void) {
    u8* p = (u8*)this;
    CFrame* f = (CFrame*)((u8*&)this->frame);
    ((PVec3A*)&this->rot)[0] = ((PVec3A*)f->m)[0];
    ((PVec3A*)&this->rot)[1] = ((PVec3A*)f->m)[1];
    ((PVec3A*)&this->rot)[2] = ((PVec3A*)f->m)[2];
    MulVec((RVec3*)&this->world, (RVec3*)&this->centre, f);
    this->world.x = this->world.x + f->t.x;
    this->world.y = this->world.y + f->t.y;
    this->world.z = this->world.z + f->t.z;
}

RagdollShape::RagdollShape(u32 a, u32 b, u32 c, f32 r) : CollShape((const CFrame*)a, (CollVolume*)c, 'RAGD', lbl_805622E0, lbl_805622E0) {
    u8* p = (u8*)this;
    ((u32&)this->f30) = b;
    fn_801A2674(p, r, 0, -1);
}

void fn_801A2674(u8* p, f32 r, u32 a, u32 b) {
    ((RagdollShape*)p)->radius = r;
    U32(p, 0x28) = a;
    U32(p, 0x2C) = b;
}

void RagdollShape::vf38(RVec3* lo, RVec3* hi) const {
    f32 r = this->radius;
    u8* o = ((u8*&)this->frame);
    RVec3 e;
    e.x = lbl_805622E0;
    e.y = lbl_805622E0;
    e.z = lbl_805622E0;
    MulAdd(&lo->x, (f32*)(o + 0x24), &e.x, -r);
    MulAdd(&hi->x, (f32*)(o + 0x24), &e.x, r);
}

extern const f32 lbl_805623AC;  // 10.0
void fn_801C2BDC(s32 sys, s32* n, RVec3** pts, s32* stride);
void fn_801C28A8(s32 sys, const Xform* f, const RVec3* ext, const RVec3* vel, const RVec3* spin, s32 a, s32 b,
                 RVec3* at, RVec3* push);

static inline f32 DistSq(const RVec3* a, const RVec3* b) {
    RVec3 d;
    d.x = a->x - b->x;
    d.y = a->y - b->y;
    d.z = a->z - b->z;
    return PSVECDotProduct(&d, &d);
}

// Moves frame x by c, given in its own axes.
static inline void Shift(Xform* x, const RVec3* c) {
    RVec3 w;
    MulVec(&w, c, (CFrame*)x);
    x->pos.v[0] += w.x;
    x->pos.v[1] += w.y;
    x->pos.v[2] += w.z;
}

// Particle sphere against an object: a group collides its box-shaped body (moving with the car, if any), a
// box itself; the particle system resolves the contact and a hard push plays the hit sound.
struct RVec3C : RVec3 {
    RVec3C(f32 a, f32 b, f32 c) {
        x = a;
        y = b;
        z = c;
    }
};

static inline f32 BoxRad(u8* o) { return F32(o, 0x30); }


void RagdollShape::vf10(u8* o) {
    u8* p = (u8*)this;
    // Reads the box half extents z first. The helper and its name are reconstructed from the load order.
    struct HalfExtents {
        static RVec3C Read(u8* shape) {
            f32 x;
            f32 y;
            f32 z;
            z = F32(shape, 0x2C);
            y = F32(shape, 0x28);
            x = F32(shape, 0x24);
            return RVec3C(x, y, z);
        }
    };
    if (((CollShape*)o)->type == 'GRUP') {
        f32 dd = DistSq(&((CFrame*)((u8*&)this->frame))->t, &((CFrame*)((u8*&)((CollShape*)o)->frame))->t);
        f32 r = this->radius + VE(o)->v3C();
        if (dd < r * r) {
            u8* box = PTR(o, 0x17C);
            RVec3 c = *(RVec3*)(box + 0x38);
            RVec3 vel;
            vel.x = lbl_805622D0;
            vel.y = lbl_805622D0;
            vel.z = lbl_805622D0;
            RVec3 spin;
            spin.x = lbl_805622D0;
            spin.y = lbl_805622D0;
            spin.z = lbl_805622D0;
            Xform x;
            u8* body = ((u8*&)((CollShape*)o)->owner);
            if (body) {
                vel = *(RVec3*)(body + 0x118);
                spin = *(RVec3*)(body + 0x124);
                c.x += F32(body, 0x30);
                c.y += F32(body, 0x34);
                c.z += F32(body, 0x38);
            }
            x = *(Xform*)VE(box)->v54();
            Shift(&x, &c);
            RVec3 at;
            RVec3 push;
            fn_801C28A8(this->f30, &x, &(const RVec3C&)HalfExtents::Read(box), &vel, &spin, this->f28, this->f2C, &at, &push);
            if (PSVECDotProduct(&push, &push) > lbl_805623AC) {
                fn_801B34DC(o, 0, &at, &push, &((const RVec3C&)RVec3C(lbl_805622D0, lbl_805622E0, lbl_805622D0)).x, U32(p, 0x20));
            }
        }
    } else if (((CollShape*)o)->type == 'BOX ') {
        f32 dd = DistSq(&((CFrame*)((u8*&)this->frame))->t, &((CFrame*)VE(o)->v50())->t);
        f32 r = this->radius + BoxRad(o);
        if (dd < r * r) {
            RVec3 vel;
            vel.x = lbl_805622D0;
            vel.y = lbl_805622D0;
            vel.z = lbl_805622D0;
            RVec3 spin;
            spin.x = lbl_805622D0;
            spin.y = lbl_805622D0;
            spin.z = lbl_805622D0;
            u8* body = ((u8*&)((CollShape*)o)->owner);
            if (body) {
                vel = *(RVec3*)(body + 0x118);
                spin = *(RVec3*)(body + 0x124);
            }
            fn_801C28A8(this->f30, (Xform*)VE(o)->v50(), &(const RVec3C&)HalfExtents::Read(o), &vel, &spin, 0, -1, 0, 0);
        }
    } else if (((CollShape*)o)->type == 'POLY') {
        s32 n = 0;
        RVec3* pts;
        s32 stride;
        fn_801C2BDC(this->f30, &n, &pts, &stride);
        for (int i = 0; i < n; i++) {
            RVec3 hit;
            f32 depth;
            RVec3 dir;
            if (fn_80198F64(&pts[i].x, (u32)o, &hit.x, &depth, &dir.x, &((const RVec3C&)RVec3C(lbl_805622D0, lbl_805622D0, lbl_805622D0)).x, 0.25f)) {
                MulAdd(&pts[i].x, &pts[i].x, &dir.x, depth);
            }
        }
    }
}

u8* OrientedBoxShape::vf54(void) {
    u8* p = (u8*)this; return (u8*)&this->rot; }

f32 SphereGroup::Radius(void) {
    u8* p = (u8*)this; return this->f178; }

void RagdollShape::vf4C(void) {
    u8* p = (u8*)this; fn_801E3FC8(((u8*&)this->frame) + 0x24, this->radius, lbl_8055E598.value); }

RingVol::RingVol(u32 a, u32 swap) : CollShape(a, swap) {
    u8* p = (u8*)this;
    if (swap) {
        Swap32(p + 0x24);
        Swap32(p + 0x28);
    }
}


// Collides ring `p` against volume `o` by its type tag.
void RingVol::vf10(u8* o) {
    switch (((CollShape*)o)->type) {
    case 'POLY':
        break;
    case 'SPHR':
        fn_8019C854(o, ((u8*)this));
        break;
    case 'GRUP':
        for (int i = 0; i < ((SphereGroup*)o)->count; i++) {
            fn_8019C854((u8*)((SphGroupDel*)o)->vols[i], ((u8*)this));
        }
        break;
    case 'BOX ':
        if (!lbl_8055E5A4) {
            lbl_8055E5A4 = 1;
            fn_802E30C8("%s", "Unsupported collision type: Ring-box");
        }
        break;
    case 'RAGD':
        if (!lbl_8055E5A5) {
            lbl_8055E5A5 = 1;
            fn_802E30C8("%s", "Unsupported collision type: Ring-Ragdoll");
        }
        break;
    case 'FLSR':
        fn_8019D378(o, ((u8*)this));
        break;
    default:
        fn_802E317C("Unsupported collision type: Ring-%x", ((CollShape*)o)->type);
        break;
    }
}

void RingVol::vf14(void) {}

// Axis-aligned bounds: the 8 corners of the local box (+-(r + h), +-(r + h), +-h) through the frame.
void RingVol::vf38(RVec3* lo, RVec3* hi) const {
    f32 h = this->ring;
    f32 a = this->inner + h;
    RVec3 c[8] = {
        RVec3(-a, -a, -h), RVec3(-a, -a, h), RVec3(-a, a, -h), RVec3(-a, a, h),
        RVec3(a, -a, -h),  RVec3(a, -a, h),  RVec3(a, a, -h),  RVec3(a, a, h),
    };
    RVec3 w;
    *lo = ((CFrame*)((u8*&)this->frame))->t;
    *hi = ((CFrame*)((u8*&)this->frame))->t;
    for (int i = 0; i < 8; i++) {
        CFrame* m = (CFrame*)((u8*&)this->frame);
        MulVec(&w, &c[i], m);
        w.x = w.x + m->t.x;
        w.y = w.y + m->t.y;
        w.z = w.z + m->t.z;
        lo->x = lo->x < w.x ? lo->x : w.x;
        lo->y = lo->y < w.y ? lo->y : w.y;
        lo->z = lo->z < w.z ? lo->z : w.z;
        hi->x = MaxF(hi->x, w.x);
        hi->y = MaxF(hi->y, w.y);
        hi->z = MaxF(hi->z, w.z);
    }
}

void RingVol::vf4C(void) {
    u8* p = (u8*)this; fn_801E3FC8(((u8*&)this->frame) + 0x24, this->inner + this->ring, lbl_8055E598.value); }

PolyShape::PolyShape(u32 a, const PolyPlane* planes, s32 n, const RVec3* lo, const RVec3* hi, f32 f1, f32 f2) : CollShape((const CFrame*)a, (CollVolume*)0, 'POLY', f1, f2) {
    u8* p = (u8*)this;
    ((u32&)this->fDC) = 0;
    this->n = n;
    for (int i = 0; i < this->n; i++) {
        ((PolyVol*)p)->planes[i].n = planes[i].n;
        ((PolyVol*)p)->planes[i].d = planes[i].d;
    }
    this->lo.x = lo->x;
    this->lo.y = lo->y;
    this->lo.z = lo->z;
    this->hi.x = hi->x;
    this->hi.y = hi->y;
    this->hi.z = hi->z;
}

ValueTable* fn_80131808(const char* name);  // open a .val table
void fn_80131818(ValueTable* table);        // release it

// Polyhedron volume ('POLY') from the named table: extents, then its "plane" (n, k) and
// "curve" (center, axis, radius) entries.
u8* fn_801A324C(u8* p, u32 id, const char* name, f32 s0, f32 s1) {
    u8* pl;
    ValueNode* n;
    ValueTable* t;
    ValueNode* ext;
    int i;
    U32(p, 0xC) = id;
    U32(p, 0x4) = 0;
    U32(p, 0x8) = 0;
    ((PolyShape*)p)->stiff = s0;
    ((PolyShape*)p)->type = 'POLY';
    ((PolyShape*)p)->skin = s1;
    ((PolyShape*)p)->surface = 100;
    ((PolyShape*)p)->x1C = 0;
    *(void**)p = __vt__9PolyShape;
    t = fn_80131808(name);
    ((PolyShape*)p)->n = 0;
    ((PolyShape*)p)->fDC = 0;
    ext = t->FindChild("extents");
    *(ValVec3*)&((PolyShape*)p)->lo = ext->GetVec3("min");
    *(ValVec3*)&((PolyShape*)p)->hi = ext->GetVec3("max");
    for (i = 0; (n = (ValueNode*)t->GetChild(i)) != 0; i++) {
        if (strcmp(n->GetName(), "plane") == 0) {
            pl = p + ((PolyShape*)p)->n++ * 0x10;
            *(ValVec3*)(pl + 0x3C) = n->GetVec3("n");
            F32(pl, 0x48) = n->GetFloat("k");
        } else if (strcmp(n->GetName(), "curve") == 0) {
            u8* c;
            c = p + ((PolyShape*)p)->fDC++ * 0x1C;
            *(ValVec3*)(c + 0xC0) = n->GetVec3("center");
            *(ValVec3*)(c + 0xCC) = n->GetVec3("axis");
            F32(c, 0xD8) = n->GetFloat("radius");
        }
    }
    fn_80131818(t);
    return p;
}

// Axis-aligned bounds of the extents (0x24 min, 0x30 max) through the frame, padded by 1.5 in x and z.
void PolyShape::vf38(RVec3* lo, RVec3* hi) const {
    u8* p = (u8*)this;
    RVec3 b;
    RVec3 a;
    CFrame* f = (CFrame*)PTR(p, 0xC);
    MulVec(&a, (RVec3*)(p + 0x24), f);
    a.x = a.x + f->t.x;
    a.y = a.y + f->t.y;
    a.z = a.z + f->t.z;
    CFrame* g = (CFrame*)PTR(p, 0xC);
    MulVec(&b, (RVec3*)(p + 0x30), g);
    f32 lx = a.x;
    b.x = b.x + g->t.x;
    b.y = b.y + g->t.y;
    b.z = b.z + g->t.z;
    *hi = a;
    *lo = *hi;
    lo->x = lx < b.x ? lx : b.x;
    lo->y = lo->y < b.y ? lo->y : b.y;
    lo->z = lo->z < b.z ? lo->z : b.z;
    f32 bx = b.x;
    f32 hx = hi->x;
    hi->x = hx > bx ? hx : bx;
    hi->y = hi->y > b.y ? hi->y : b.y;
    hi->z = hi->z > b.z ? hi->z : b.z;
    hi->x += 1.5f;
    hi->z += 1.5f;
    lo->x -= 1.5f;
    lo->z -= 1.5f;
}

f32 PolyShape::Radius(void) {
    u8* p = (u8*)this;
    f32 a = PSVECDotProduct((u8*)&this->hi, (u8*)&this->hi);
    f32 b = PSVECDotProduct((u8*)&this->lo, (u8*)&this->lo);
    return (f32)sqrt(b > a ? b : a);
}

// Sphere (and flat sphere 'FLSR') against the world: each point attached to the owner body
// (lifted 0.25), then the sphere itself; contacts go to fn_8019A788.
// The owner body as seen here: its frame at 0x3C and its attached points.
struct OwnerBody {
    u8 pad0[0x3C];
    CFrame frame;  // offset 0x3C
};

void PolyShape::vf10(u8* p) {
    RVec3 pt;
    PVec3A n1;
    PVec3A hit1;
    PVec3A n2;
    PVec3A hit2;
    PVec3A vel1;
    PVec3A vel2;
    PVec3A down;
    f32 depth1;
    f32 depth2;
    switch (S32(p, 0x18)) {
    case 'FLSR': {
        down.v[0] = 0.0f;
        down.v[1] = -1.0f;
        down.v[2] = 0.0f;
        f32 k = (f32)__fabs(PSVECDotProduct(down.v, PTR(p, 0xC) + 0xC));
        F32(p, 0x24) = F32(p, 0x54) + k * (F32(p, 0x58) - F32(p, 0x54));
    }
    case 'SPHR':
        break;
    default:
        return;
    }
    {
        OwnerBody* owner = (OwnerBody*)PTR(p, 0x4);
        if (owner) {
            u8* e = PTR(owner, 0xD0);
            if (e) {
                for (; e; e = PTR(e, 0x0)) {
                    RVec3* lp = (RVec3*)(e + 0x4);
                    MulVec(&pt, lp, &owner->frame);
                    pt.x = pt.x + owner->frame.t.x;
                    pt.y = pt.y + owner->frame.t.y;
                    pt.z = pt.z + owner->frame.t.z;
                    pt.y += 0.25f;
                    vel1.v[0] = 0.0f;
                    vel1.v[1] = 0.0f;
                    vel1.v[2] = 0.0f;
                    if (fn_80198F64(&pt.x, ((u32)this), hit1.v, &depth1, n1.v, vel1.v, 0.25f)) {
                        fn_8019A788(p, (u8*)((u32)this), (u32)hit1.v, (RVec3*)n1.v, 0, 0, depth1, 0.0f, 0.0f);
                    }
                }
            }
        }
        vel2.v[0] = 0.0f;
        vel2.v[1] = 0.0f;
        vel2.v[2] = 0.0f;
        if (fn_80198F64((f32*)(p + 0x34), ((u32)this), hit2.v, &depth2, n2.v, vel2.v, F32(p, 0x24))) {
            fn_8019A788(p, (u8*)((u32)this), (u32)hit2.v, (RVec3*)n2.v, 0, 0, depth2, 0.0f, 0.0f);
        }
    }
}


// Debug draw: a line from the frame origin along each of the volume's rays (direction * length, frame space).
void PolyShape::vf4C(void) {
    u8* p = (u8*)this;
    PVec3A o;
    RVec3 v;
    RVec3 w;
    const f32* d = (const f32*)(p + 0x3C);
    u32 color = lbl_8055E598.value;
    o = *(PVec3A*)&((CFrame*)((u8*&)this->frame))->t;
    for (int i = 0; i < this->n; d += 4, i++) {
        f32 len = d[3];
        v.x = d[0] * len;
        v.y = d[1] * len;
        v.z = d[2] * len;
        CFrame* f = (CFrame*)((u8*&)this->frame);
        MulVec(&w, &v, f);
        w.x = w.x + f->t.x;
        w.y = w.y + f->t.y;
        w.z = w.z + f->t.z;
        fn_801E41D8(o.v, &w.x, color, 0.3f);
    }
}

// Mesh resource (fn_801CFEAC): its vertices.
struct MeshRes {
    u8 pad0[0x8];
    RVec3* verts;  // offset 0x8
    s32 count;     // offset 0xC
};

// Mesh volume ('PMSH') from the named mesh: bounding box min (0x28) / max (0x34) over its vertices.
MeshShape::MeshShape(u32 id, const char* name, u32 owner) : CollShape((const CFrame*)id, (CollVolume*)owner, 'PMSH', 1.0f, 1.0f) {
    u8* p = (u8*)this;
    MeshRes* m = (MeshRes*)fn_801CFEAC(name);
    this->mesh = (u8*)m;
    RVec3* mn = (RVec3*)(p + 0x28);
    RVec3* mx = (RVec3*)(p + 0x34);
    *mx = m->verts[0];
    *mn = *mx;
    for (int i = 1; i < ((MeshRes*)this->mesh)->count; i++) {
        BoxCorner lo = VMin(*mn, ((MeshRes*)this->mesh)->verts[i]);
        *mn = *(RVec3*)&lo;
        BoxCorner hi = VMax(*mx, ((MeshRes*)this->mesh)->verts[i]);
        *mx = *(RVec3*)&hi;
    }
}

MeshShape::MeshShape(u32 a, u32 b) : CollShape(a, b) {
    u8* p = (u8*)this;
    if (b) {
        PTR(p, 0x24) = p + 0x40;
        fn_801CFA48(p + 0x40);
    }
}

MeshShape::~MeshShape(void) {
    fn_801CFF14(PTR(this, 0x24));
}

void MeshShape::vf10(u8* o) {
    if (((CollShape*)o)->type == 'GRUP') {
        ((VGroup*)o)->v10(((u8*)this));
    } else if (((CollShape*)o)->type == 'SPHR') {
        PlainRVec3 hits[64];  // plain storage: an array of the constructor type changes the registers
        int n = 64;
        u8* sph = o;
        f32 r = F32(sph, 0x24);
        fn_801CF080(this->mesh, (RVec3*)(sph + 0x34), (RVec3*)hits, &n, U32(((u8*)this), 0xC), lbl_8056234C * r);
        for (int i = 0; i < n; i++) {
            fn_8019AA18(sph, (u32)&hits[i], 0x6E, (u32)((u8*)this));
        }
    }
}

// Ray from + t*dir against the mesh volume, tested in the volume's frame; the hit normal is turned
// back into world space.
int MeshShape::vf18(const RVec3* from, const RVec3* dir, f32* t, RVec3* n) {
    u8* p = (u8*)this;
    RVec3 lf;
    RVec3 ld;
    RVec3 ln;
    CFrame* f = (CFrame*)((u8*&)this->frame);
    lf.x = from->x - f->t.x;
    lf.y = from->y - f->t.y;
    lf.z = from->z - f->t.z;
    {
        f32 x = Dot3(lf.x, f->m[0], lf.y, f->m[1], lf.z, f->m[2]);
        f32 y = Dot3(lf.x, f->m[3], lf.y, f->m[4], lf.z, f->m[5]);
        f32 z = Dot3(lf.x, f->m[6], lf.y, f->m[7], lf.z, f->m[8]);
        lf.x = x;
        lf.y = y;
        lf.z = z;
    }
    MulVecT(&ld, dir, (CFrame*)((u8*&)this->frame));
    if (fn_801CF828(this->mesh, (u32)&lf, (u32)&ld, (u32)t, (u32)&ln)) {
        MulVec(n, &ln, (CFrame*)((u8*&)this->frame));
        return 1;
    }
    return 0;
}

void MeshShape::vf38(RVec3* a, RVec3* b) const {
    *a = *(const RVec3*)(((const u8*)this) + 0x28);
    *b = *(const RVec3*)(((const u8*)this) + 0x34);
}

f32 MeshShape::Radius(void) {
    u8* p = (u8*)this;
    f32 a = PSVECDotProduct(p + 0x34, p + 0x34);
    f32 b = PSVECDotProduct(p + 0x28, p + 0x28);
    return (f32)sqrt(b > a ? b : a);
}

void MeshShape::vf4C(void) {
    u8* p = (u8*)this; fn_801CFF18(this->mesh, U32(p, 0xC)); }

void fn_801A44D4(u8* l) {
    for (int i = 0; i < S32(l, 0x0); i++) {
        fn_801A49FC(((u8**)PTR(l, 0x4))[i]);
    }
}

void fn_801A4538(void* a) { fn_8012C4E4(a, (void*)fn_801A44D4, 0); }

void* fn_8012C370(const char* name, u32 type, u32* info, s32* size, bool* first, bool* swapped);  // res.cpp
void fn_801A4768(s32* p, s32 base);
void fn_801A4780(s32* p, s32 base);
void fn_801A4798(s32* p, s32 base);
void fn_801A47B0(s32* p, s32 base);

static inline void Swap16(u8* b) {
    u8 t = b[0];
    b[0] = b[1];
    b[1] = t;
}

/* Helper name unproven */
static inline void SwapStaticObjectHeader(u8* l) {
    u8* objs = l + 0x4;
    Swap32(l + 0x0);
    Swap32(l + 0x8);
    Swap32(objs);
    Swap32(l + 0xC);
    Swap32(l + 0x10);
    fn_801A4768((s32*)objs, (s32)l);
    fn_801A4780((s32*)(l + 0x10), (s32)l);
    fn_801A4798((s32*)(l + 0xC), (s32)l);
}

// Loads the static-object list resource ('SOBL'): on first load byte-swaps it, turns its offsets into
// pointers and sets up each object.
u8* fn_801A4548(const char* name) {
    u32 info;
    bool swapped = 0;
    u8* l = (u8*)fn_8012C370(name, 'SOBL', &info, 0, 0, &swapped);
    if (l && swapped) {
        SwapStaticObjectHeader(l);
        int i;
        for (i = 0; i < S32(l, 0x0); i++) {
            Swap32(PTR(l, 0x4) + i * 4);
            fn_801A47B0((s32*)(PTR(l, 0x4) + i * 4), (s32)l);
        }
        for (i = 0; i < S32(l, 0x8); i++) {
            Swap16(PTR(l, 0xC) + i * 2);
        }
        fn_8019EB14(PTR(l, 0x10), 0);
        for (i = 0; i < S32(l, 0x0); i++) {
            u8* o = ((u8**)PTR(l, 0x4))[i];
            bool sw = swapped;
            u8* vol = o + 0x4C;
            new (o) StaticCollObject(sw, vol);
        }
        PTR(l, 0x14) = (u8*)fn_801A4538;
    }
    return l;
}

void fn_801A4768(s32* p, s32 base) {
    if (*p) {
        *p += base;
    }
}

void fn_801A4780(s32* p, s32 base) {
    if (*p) {
        *p += base;
    }
}

void fn_801A4798(s32* p, s32 base) {
    if (*p) {
        *p += base;
    }
}

void fn_801A47B0(s32* p, s32 base) {
    if (*p) {
        *p += base;
    }
}

void fn_801A47C8(u8* l) {
    for (int i = 0; i < S32(l, 0x0); i++) {
        U8(((u8**)PTR(l, 0x4))[i], 0x40) = 1;
    }
}

void fn_801A47FC(u8* l) {
    if (l) {
        for (int i = 0; i < S32(l, 0x0); i++) {
            fn_801A49E8(((u8**)PTR(l, 0x4))[i]);
        }
    }
}

struct OnMsg {
    u32 id;
    u8 on;
};

void fn_801A4868(u8* p, u32 v) {
    if (((StaticCollObject*)p)->b40 != v) {
        OnMsg m;
        m.id = ((StaticCollObject*)p)->f38;
        m.on = v;
        fn_801796EC(0xC, &m, 8);
        ((StaticCollObject*)p)->b40 = v;
    }
}

void StaticCollObject::vf0C(void) {
    u8* p = (u8*)this; VE(this->vol)->v18("dyno", "unknown"); }

void fn_801EA1E0(f32 x, f32 z, f32* y, int a);
extern const f32 lbl_805622DC;  // -1.0
extern const f32 lbl_805622E0;  // 1.0

// Refreshes the attached volume and drops it (and the anchor) onto the ground.
void fn_801A48E4(u8* p) {
    U8(p, 0x40) = 1;
    ((VMesh*)PTR(p, 0x3C))->v0C();
    u8* o = PTR(p, 0x3C);
    s32 type = S32(o, 0x18);
    if (type == 'SPHR') {
        fn_801EA1E0(F32(o, 0x34), F32(o, 0x3C), (f32*)(o + 0x38), 0);
        fn_801EA1E0(F32(p, 0x28), F32(p, 0x30), (f32*)(p + 0x2C), 0);
    }
    if (type == 'TUBE') {
        f32 dir = F32(p, 0x20) > lbl_805622D0 ? lbl_805622DC : lbl_805622E0;
        f32 len = F32(PTR(p, 0x3C), 0x28);
        f32 r = dir * len;
        f32 end[3];
        end[0] = F32(p, 0x28) + F32(p, 0x1C) * r;
        end[1] = F32(p, 0x2C) + F32(p, 0x20) * r;
        end[2] = F32(p, 0x30) + F32(p, 0x24) * r;
        fn_801EA1E0(end[0], end[2], (f32*)(p + 0x2C), 0);
    }
}

void fn_801A49E8(u8* p) {
    if (PTR(p, 0x48)) {
        fn_801D0650(p);
    }
}

void fn_801A49FC(u8* p) {
    if (PTR(p, 0x48)) {
        fn_801D06C4(p);
    }
}

void fn_801A4A10(u8* p, const RVec3* c, void* f, const RVec3* b) {
    if (U8(p, 0x40)) {
        fn_801D2414(p, c, f);
        if (PTR(p, 0x48)) {
            fn_801D0738(p, c, f, b);
        }
    }
}

u8* fn_801A4A8C(u8* p, u32 a, u32 b, u8* mesh, const Pose12s* pose, u32 c) {
    ((StaticCollObject*)p)->f38 = a;
    *(void**)p = __vt__16StaticCollObject;
    ((u32&)((StaticCollObject*)p)->vol) = c;
    ((StaticCollObject*)p)->f34 = b;
    ((u8*&)((StaticCollObject*)p)->shape) = mesh;
    *(Pose12s*)(p + 0x4) = *pose;
    ((VMesh*)mesh)->v2C(p);
    fn_801A48E4(p);
    return p;
}

// Static object from file data (byte-swapped when `swap`); its collision volume is built in place at
// vol according to the volume's tag.
StaticCollObject::StaticCollObject(bool swap, u8* vol) {
    u8* o = (u8*)this;
    if (swap) {
        Swap32(o + 0x34);
        Swap32(o + 0x38);
        f32* w = (f32*)(o + 0x4);
        for (int i = 0; i < 12; i++) {
            Swap32((u8*)&w[i]);
        }
    }
    U32(o, 0x48) = 0;
    s32 tag = S32(vol, 0x18);
    if (swap) {
        Swap32((u8*)&tag);
    }
    switch (tag) {
    case 'BOX ':
        PTR(o, 0x3C) = (u8*)new (vol) BoxShape((u32)o, swap);
        break;
    case 'SPHR':
        PTR(o, 0x3C) = (u8*)new (vol) SphereShape((u32)o, swap);
        break;
    case 'TUBE':
        PTR(o, 0x3C) = (u8*)new (vol) TubeShape((u32)o, swap);
        break;
    case 'RING':
        PTR(o, 0x3C) = (u8*)new (vol) RingVol((u32)o, swap);
        break;
    case 'PMSH':
        PTR(o, 0x3C) = (u8*)new (vol) MeshShape((u32)o, swap);
        break;
    default:
        PTR(o, 0x3C) = 0;
        break;
    }
    fn_801A48E4(o);
}

void fn_801A4E28(void) { lbl_8055E5A0 = fn_801CFEAC("track.msh"); }

void fn_801A4E54(void) {
    if (lbl_8055E5A0) {
        fn_801CFF14(lbl_8055E5A0);
        lbl_8055E5A0 = 0;
    }
}

int fn_801A4E88(u32 a, u32 b, u32 c, u32 d, s32* out) {
    if (lbl_8055E5A0 && fn_801CF828(lbl_8055E5A0, a, b, c, d)) {
        *out = 0x6E;
        return 1;
    }
    return 0;
}

f32 SphereShape::Radius(void) {
    u8* p = (u8*)this; return this->radius; }

void SphereGroup::vf48(void) {
    u8* p = (u8*)this; this->f180 = -1; }

f32 BoxShape::Radius(void) {
    u8* p = (u8*)this; return this->radius; }

f32 RagdollShape::Radius(void) {
    u8* p = (u8*)this; return this->radius; }

f32 RingVol::Radius(void) {
    u8* p = (u8*)this; return this->ring; }

int CollShape::vf18(const RVec3*, const RVec3*, f32*, RVec3*) { return 0; }

u32 StaticCollObject::vf08(void) {
    u8* p = (u8*)this; return ((u32&)this->vol); }

u8* StaticCollObject::vf10(void) {
    u8* p = (u8*)this; return p + 0x28; }

u8 StaticCollObject::vf14(void) {
    u8* p = (u8*)this; return this->b40; }

void StaticCollObject::vf18(u32 v) {
    u8* p = (u8*)this; fn_801A4868(p, v); }

void StaticCollObject::vf1C(void) {
    u8* p = (u8*)this;
    u8* o = ((u8*&)this->shape);
    if (o) {
        VE(o)->v40();
    }
}

s32 StaticCollObject::vf20(void) {
    u8* p = (u8*)this;
    u8* o = ((u8*&)this->shape);
    if (o) {
        return S32(o, 0x20);
    }
    return -1;
}

u32 StaticCollObject::vf24(void) {
    u8* p = (u8*)this; return this->f34; }

void StaticCollObject::vf28(u32 v) {
    u8* p = (u8*)this; this->f34 = v; }

u8 lbl_8055A518 = 1;  // unused in ET (its users are gone); last object of .sdata



}  // extern "C"

