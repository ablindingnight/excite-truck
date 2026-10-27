// Collision volumes (0x80198F64-0x801A520C): spheres, flat spheres, sphere groups ("SphereGroupPart")
// and their parts, the mesh-backed track collision ("track.msh"), bounds and debug drawing, and
// the per-type collide dispatch ("Unsupported collision type: Sphere-%x"). Written against raw
// field offsets; the large collide routines are still stubs. Names are guesses apart from the
// strings.
#include "game/common.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

struct RVec3 {
    f32 x, y, z;
};

struct PVec3 {
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

struct Phob;

class RigidBody {
public:
    void Push(Phob* who, void* a, const RVec3* b, const RVec3* c, int d, const RVec3* e, void* f, f32 g);
    void VelAt(RVec3* out, const RVec3* at);
    void AddTorque(const RVec3* t);
};

class VGroup {
public:
    virtual void v08(void);
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

extern void* lbl_80333968[];
extern void* lbl_80333818[];
extern void* lbl_803337C8[];
extern void* lbl_803338C0[];
extern void* lbl_80333868[];

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

extern void* lbl_80333778[];
extern void* lbl_803336F8[];
extern const f32 lbl_8056234C;  // 2.0f

struct Pose12s {
    u32 w[12];
};

class VMesh {
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
void fn_801D2414(u8* p);
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
    virtual void s08(void);
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

class VEx {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(const char* a, const char* b);
    virtual void v1C(const RVec3* imp, const RVec3* at, u32 a, u32 b);  // push
    virtual void v20(u32 who, const RVec3* f, const RVec3* at, u32 a, u32 b, u32 c, f32 w);  // friction
    virtual void v24(void);
    virtual void v28(void);
    virtual void v2C(void);
    virtual void v30(void);
    virtual void v34(u8* q);
    virtual void v38(void);
    virtual f32 v3C(void);  // bounding radius
    virtual void v40(void);
    virtual void v44(void);
    virtual void v48(void);
    virtual void v4C(void);
    virtual u8* v50(void);
};

#define VE(p) ((VEx*)(p))

class SpringCar {
public:
    void HitWall(void* w);
};

extern "C" {
f32 PSVECDotProduct(const void* a, const void* b);
double sqrt(double x);
void fn_801E3FC8(u8* c, u32 color, f32 r);
void fn_801E44F4(u8* m, u32 color, f32 a, f32 b, f32 c);
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
int fn_8019AA34(u8* p, u8* c, u8* q, u8* d, u32 a, u32 b, f32 r, f32 s);
int fn_8019CCCC(u8* p, u8* c, u8* q, f32 r);
void fn_8019AC44(u8* p, const f32* c, f32* d, f32 r);
int fn_8019DDFC(const f32* from, const f32* dir, f32* t, f32* n, const f32* c, f32 r);
void fn_8019ECC4(u8* p, u32 a, u32 b);
u8* fn_801A1520(u8* p, u32 a, u32 swap);
void fn_8019F1A4(u8* p);
void fn_8019EBC0(u8* p);
void fn_8019F03C(u8* p);
void fn_801A49FC(u8* p);
void fn_801A49E8(u8* p);
void fn_801A44D4(u8* l);
void fn_801A4868(u8* p, u32 v);
}

void* operator new(size_t size, const char* tag);
void operator delete(void* p);

extern void* lbl_803339B8[];
extern void* lbl_80333918[];
extern void* lbl_80333728[];

ModuleTag lbl_8055E590;
UnkStaticFFFF lbl_8055E598;
UnkStaticFFFF lbl_8055E59C;
u8* lbl_8055E5A0;  // track collision mesh

extern "C" {

int fn_80198F64(const f32* c, u32 world, f32* hit, f32* depth, f32* n, const f32* vel, f32 r) { return 0; }  // stub (470)

// Friction impulse at contact point pt (normal n) between volumes a and b: the opposing tangential
// relative velocity, scaled by -1000 and clamped to half of mu * (a's + b's friction).
void fn_801996BC(RVec3* out, const RVec3* pt, const RVec3* n, u8* a, u8* b, f32 mu) {
    RVec3 dn;
    RVec3 rel;
    RVec3 va;
    RVec3 vb;
    RVec3 f;
    RigidBody* ra = (RigidBody*)PTR(a, 0x4);
    RigidBody* rb = (RigidBody*)PTR(b, 0x4);
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
    f32 fb = F32(b, 0x14);
    f32 fa = F32(a, 0x14);
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
        ((SpringCar*)p)->HitWall(w);
    }
}

void fn_80199894(u8* p, u8* q, u32 x, const RVec3* n, u32 a, u32 b, f32 s, f32 u, f32 v) {}  // stub (955)

u32 fn_8019A780(u8* p) { return U32(p, 0x8); }

extern const f32 lbl_805622DC;  // -1.0

// Contact between p and q along n; volumes without an owner hand the test to q with the normal flipped.
void fn_8019A788(u8* p, u8* q, u32 x, const RVec3* n, u32 a, u32 b, f32 s, f32 u, f32 v) {
    if (U32(p, 0x4) || a) {
        fn_80199894(p, q, x, n, a, b, s, u, v);
    } else {
        RVec3 m;
        m.x = n->x * lbl_805622DC;
        m.y = n->y * lbl_805622DC;
        m.z = n->z * lbl_805622DC;
        fn_80199894(q, p, x, &m, b, a, s, v, u);
    }
}

f32 fn_8019DF88(const u8* com, const RVec3* v, const u8* iinv, const RVec3* at, const f32* n, f32 mass, f32 e);
void fn_801B34DC(u8* p, u8* q, const RVec3* at, const RVec3* f, const f32* n, u32 a);  // collision sound

// Sphere (c, r) of p against the point at: a spring push along the normal, then friction from the body.
int fn_8019A820(u8* p, const f32* c, const RVec3* at, u32 a, u32 b, f32 r) {
    PVec3 d;
    PVec3 n;
    RVec3 vel;
    RVec3 imp;
    RVec3 fr;
    d.v[0] = c[0] - at->x;
    d.v[1] = c[1] - at->y;
    d.v[2] = c[2] - at->z;
    f32 len = PSVECMag(d.v);
    RigidBody* rb = (RigidBody*)PTR(p, 0x4);
    if (len > 0.001f && len < r) {
        f32 pen = r - len;
        f32 inv = 1.0f / len;
        n.v[0] = d.v[0] * inv;
        n.v[1] = d.v[1] * inv;
        n.v[2] = d.v[2] * inv;
        rb->VelAt(&vel, at);
        f32 k = pen * F32(p, 0x10);
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

extern f32 lbl_80558DC4[];  // FLT_EPSILON
void PSVECNormalize(const void* in, void* out);

// Sphere (c, r) of p against sphere (d, s) of q: reports the contact (normal flattened in y) to the
// volume that has an owner.
int fn_8019AA34(u8* p, u8* c, u8* q, u8* d, u32 a, u32 b, f32 r, f32 s) {
    PVec3 dd;
    PVec3 n;
    PVec3 at;
    f32 rs = r + s;
    const f32* cv = (const f32*)c;
    const f32* dv = (const f32*)d;
    f32 dx = cv[0] - dv[0];
    f32 dz = cv[2] - dv[2];
    f32 dy = cv[1] - dv[1];
    dd.v[0] = dx;
    dd.v[2] = dz;
    dd.v[1] = dy;
    int hit = 0;
    if ((f32)__fabs(dx) < rs && (f32)__fabs(dz) < rs && (f32)__fabs(dy) < rs) {
        f32 dist = PSVECMag(dd.v);
        if (!((f32)__fabs(dist) <= lbl_80558DC4[0])) {
            f32 pen = s + (-dist + r);
            if (pen > 0.0f) {
                f32 inv = 1.0f / dist;
                n.v[0] = dd.v[0] * inv;
                n.v[1] = dd.v[1] * inv * 0.2f;
                n.v[2] = dd.v[2] * inv;
                PSVECNormalize(n.v, n.v);
                f32 k = -0.5f * ((r + dist) - s);
                MulAdd(at.v, cv, n.v, k);
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
extern const f32 lbl_80562320;
extern const f32 lbl_80562324;
extern const f32 lbl_80562328;
extern const f32 lbl_80562330;
extern const f32 lbl_805622D4;
extern const f32 lbl_8056232C;
extern const f32 lbl_805622D0;
extern "C" int fn_801E9F08(u8* hint, const f32* at, RVec3* pos, RVec3* n, u32* surf, f32 maxDist);

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
    u32 surf;
    RigidBody* o = (RigidBody*)PTR(p, 0x4);
    if (lbl_8055E5A0) {
        n = 64;
        fn_801CF080(lbl_8055E5A0, (const RVec3*)c, hits, &n, 0, r);
        for (int i = 0; i < n; i++) {
            fn_8019A820(p, c, &hits[i], 0x6E, 0, r);
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
        f32 k = depth * F32(p, 0x10);
        f32 vn = -PSVECDotProduct(&v, &nrm);
        imp.z = nrm.z * k;
        imp.y = nrm.y * k;
        imp.x = nrm.x * k;
        vt.x = v.x + nrm.x * vn;
        vt.y = v.y + nrm.y * vn;
        vt.z = v.z + nrm.z * vn;
        d[0] = d[0] + vt.x * lbl_8056231C;
        d[1] = d[1] + vt.y * lbl_8056231C;
        d[2] = d[2] + vt.z * lbl_8056231C;
        f32 dm = PSVECMag(d);
        if (dm > lbl_80562320) {
            f32 s = lbl_80562320 / dm;
            d[0] *= s;
            d[1] *= s;
            d[2] *= s;
        }
        f.x = vt.x * lbl_80562324;
        f.y = vt.y * lbl_80562324;
        f.z = vt.z * lbl_80562324;
        f32 m = lbl_80562328 * F32(o, 0xD4);
        f.x = f.x + d[0] * m;
        f.y = f.y + d[1] * m;
        f.z = f.z + d[2] * m;
        f32 fm = PSVECMag(&f.x);
        f32 lim = k * F32(p, 0x14);
        lim = lim > lbl_80562320 ? lim : lbl_80562320;
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
        f32 e = lbl_80562330 * fn_8019DF88((u8*)o + 0x60, &v, (u8*)o + 0x148, &pt, &nrm.x, F32(o, 0xD4), lbl_805622D0);
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
    f32 s = F32(q, 0x24);
    return fn_8019AA34(p, p + 0x34, q, q + 0x34, a, b, F32(p, 0x24), s);
}

// Sphere (centre c, radius r) against the side of tube q: in tube space, within the half length and off
// the axis, a contact at the tube surface with the radial normal.
int fn_8019B08C(u8* p, const f32* c, u8* q, f32 r) {
    CFrame* f = (CFrame*)PTR(q, 0xC);
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
    f32 len = F32(q, 0x28);
    if (l.z < len && l.z > -len) {
        f32 dist = (f32)sqrt(l.x * l.x + l.y * l.y);
        if (!((f32)__fabs(dist) <= lbl_80558DC4[0])) {
            r += -dist + F32(q, 0x24);
            if (r > 0.0f) {
                BoxCorner ct(f->t.x, f->t.y, f->t.z);
                BoxCorner cc(c[0], c[1], c[2]);
                RVec3 s;
                RVec3 w;
                RVec3 nw;
                RVec3 n;
                RVec3 nn;
                s.x = l.x;
                s.y = l.y;
                s.z = 0.0f;
                f32 k = (F32(q, 0x24) - r) / dist;
                s.x *= k;
                s.z = l.z;
                s.y *= k;
                CFrame* m = (CFrame*)PTR(q, 0xC);
                MulVec(&w, &s, m);
                w.x += m->t.x;
                w.y += m->t.y;
                w.z += m->t.z;
                n.x = l.x;
                n.y = l.y;
                n.z = 0.0f;
                PSVECNormalize(&n.x, &n.x);
                MulVec(&nw, &n, (CFrame*)PTR(q, 0xC));
                nn = nw;
                fn_8019A788(p, q, (u32)&w, &nn, 0, 0, r, 0.0f, 0.0f);
                return 1;
            }
        }
    }
    return 0;
}

int fn_8019B4D8(u8* p, u8* q, const RVec3* c, u32 a, f32 r) { return 0; }  // stub (541)

u32 fn_8019BD4C(u8* p) { return U32(p, 0xc); }

void fn_8019BD54(u8* p, u8* q, int a, int b) {}  // stub (704)

// Sphere p against ring q (major radius 0x24, tube radius 0x28): in ring space, within the slab, the
// nearest point of the ring's centre circle gives the contact.
int fn_8019C854(u8* p, u8* q) {
    RVec3 l;
    RVec3 a;
    RVec3 n;
    RVec3 w;
    RVec3 nw;
    CFrame* f = (CFrame*)PTR(q, 0xC);
    l.x = F32(p, 0x34) - f->t.x;
    l.y = F32(p, 0x38) - f->t.y;
    l.z = F32(p, 0x3C) - f->t.z;
    f32 y = Dot3(l.x, f->m[3], l.y, f->m[4], l.z, f->m[5]);
    f32 x = Dot3(l.x, f->m[0], l.y, f->m[1], l.z, f->m[2]);
    f32 z = Dot3(l.x, f->m[6], l.y, f->m[7], l.z, f->m[8]);
    l.x = x;
    l.y = y;
    l.z = z;
    f32 rs = F32(p, 0x24);
    f32 R = F32(q, 0x24);
    f32 tr = F32(q, 0x28);
    if (l.z < rs + tr && l.z > -(rs + tr)) {
        f32 dist = (f32)sqrt(l.x * l.x + l.y * l.y);
        f32 pen = (rs + tr) - (f32)__fabs(dist - R);
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
            CFrame* m = (CFrame*)PTR(q, 0xC);
            MulVec(&w, &a, m);
            w.x += m->t.x;
            w.y += m->t.y;
            w.z += m->t.z;
            MulVec(&nw, &n, (CFrame*)PTR(q, 0xC));
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
    PVec3 a;
    PVec3 b;
    PVec3 axis;
    PVec3 org;
    f32 rad;
    int hit;
    if (fn_8019B08C(p, (f32*)c, q, r)) {
        return 1;
    }
    m = PTR(q, 0xC);
    h = LoadF((f32*)(q + 0x28));
    axis = *(PVec3*)(m + 0x18);
    org = *(PVec3*)(m + 0x24);
    rad = LoadF((f32*)(q + 0x24));
    MulAdd(a.v, org.v, axis.v, h);
    MulAdd(b.v, org.v, axis.v, -h);
    hit = 0;
    hit = (hit | fn_8019AA34(p, c, q, (u8*)a.v, 0, 0, r, rad)) != 0;
    hit = (hit | fn_8019AA34(p, c, q, (u8*)b.v, 0, 0, r, rad)) != 0;
    return hit;
}

void fn_8019CE3C(u8* p, u32 a) { fn_8019CCCC(p, p + 0x34, (u8*)a, F32(p, 0x24)); }

extern const f32 lbl_8056232C;  // small distance
f32 PSVECSquareDistance(const void* a, const void* b);

// Sets flat sphere q's radius from how edge-on it is to sphere p, then collides them.
void fn_8019CE4C(u8* p, u8* q) {
    f32* c = (f32*)(p + 0x34);
    f32 d[3];
    d[0] = c[0] - F32(q, 0x34);
    d[1] = c[1] - F32(q, 0x38);
    d[2] = c[2] - F32(q, 0x3C);
    f32 sq = PSVECSquareDistance(c, q + 0x34);
    f32 r;
    if (sq > lbl_8056232C) {
        f32 inv = 1.0f / (f32)sqrt(sq);
        d[0] *= inv;
        d[1] *= inv;
        d[2] *= inv;
        f32 k = (f32)__fabs(PSVECDotProduct(d, PTR(q, 0xC) + 0xC));
        r = F32(q, 0x54) + k * (F32(q, 0x58) - F32(q, 0x54));
    } else {
        f32 b = F32(q, 0x58);
        f32 a = F32(q, 0x54);
        r = a > b ? a : b;
    }
    F32(q, 0x24) = r;
    fn_8019B068(p, q, 0, 0);
}


// Sets flat sphere q's radius from how edge-on it is to p, then collides them.
void fn_8019CF60(u8* p, u8* q) {
    u8* f = PTR(p, 0xC);
    f32 d[3];
    d[0] = F32(f, 0x24) - F32(q, 0x34);
    d[1] = F32(f, 0x28) - F32(q, 0x38);
    d[2] = F32(f, 0x2C) - F32(q, 0x3C);
    f32 sq = PSVECSquareDistance(f + 0x24, q + 0x34);
    f32 r;
    if (sq > lbl_8056232C) {
        f32 inv = 1.0f / (f32)sqrt(sq);
        d[0] *= inv;
        d[1] *= inv;
        d[2] *= inv;
        f32 c = (f32)__fabs(PSVECDotProduct(d, PTR(q, 0xC) + 0xC));
        r = F32(q, 0x54) + c * (F32(q, 0x58) - F32(q, 0x54));
    } else {
        f32 b = F32(q, 0x58);
        f32 a = F32(q, 0x54);
        r = a > b ? a : b;
    }
    F32(q, 0x24) = r;
    fn_8019CE3C(q, (u32)p);
}

void fn_801A1F44(RVec3* out, u8* q);

// Sets flat sphere p's radius from how edge-on it is to box q, then collides them.
void fn_8019D06C(u8* p, u8* q, int a, int b) {
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
        f32 k = (f32)__fabs(PSVECDotProduct(d, PTR(p, 0xC) + 0xC));
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
    d[0] = c[0] - F32(s, 0x34);
    d[1] = c[1] - F32(s, 0x38);
    d[2] = c[2] - F32(s, 0x3C);
    f32 sq = PSVECSquareDistance(c, s + 0x34);
    if (sq > lbl_8056232C) {
        f32 inv = 1.0f / (f32)sqrt(sq);
        d[0] *= inv;
        d[1] *= inv;
        d[2] *= inv;
        f32 k = (f32)__fabs(PSVECDotProduct(d, PTR(s, 0xC) + 0xC));
        r = F32(s, 0x54) + k * (F32(s, 0x58) - F32(s, 0x54));
    } else {
        f32 hi = F32(s, 0x58);
        f32 lo = F32(s, 0x54);
        r = lo > hi ? lo : hi;
    }
    F32(s, 0x24) = r;
}

// Sets both flat spheres' radii from how edge-on they are to each other, then collides them.
void fn_8019D19C(u8* p, u8* q) {
    FlatRadius(p, (f32*)(q + 0x34));
    FlatRadius(q, (f32*)(p + 0x34));
    fn_8019B068(p, q, 0, 0);
}

// Sets flat sphere p's radius from how edge-on it is to ring q, then collides them.
void fn_8019D378(u8* p, u8* q) {
    f32* c = (f32*)(PTR(q, 0xC) + 0x24);
    f32 d[3];
    d[0] = c[0] - F32(p, 0x34);
    d[1] = c[1] - F32(p, 0x38);
    d[2] = c[2] - F32(p, 0x3C);
    f32 sq = PSVECSquareDistance(c, p + 0x34);
    f32 r;
    if (sq > lbl_8056232C) {
        f32 inv = 1.0f / (f32)sqrt(sq);
        d[0] *= inv;
        d[1] *= inv;
        d[2] *= inv;
        f32 k = (f32)__fabs(PSVECDotProduct(d, PTR(p, 0xC) + 0xC));
        r = F32(p, 0x54) + k * (F32(p, 0x58) - F32(p, 0x54));
    } else {
        f32 b = F32(p, 0x58);
        f32 a = F32(p, 0x54);
        r = a > b ? a : b;
    }
    F32(p, 0x24) = r;
    fn_8019C854(p, q);
}

void PSVECCrossProduct(const void* a, const void* b, void* out);
extern const f32 lbl_80562348;  // parallel-line threshold

static inline f32 Det33(f32 m[3][3]) {
    return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) + m[0][1] * (m[1][2] * m[2][0] - m[1][0] * m[2][2]) +
           m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
}

// Closest approach of the lines p0 + t*d0 and p1 + s*d1; 0 when they are parallel.
int fn_8019D484(const f32* p0, const f32* d0, const f32* p1, const f32* d1, f32* t, f32* s) {
    f32 m[3][3];
    PVec3 w;
    PVec3 n;
    w.v[0] = p1[0] - p0[0];
    w.v[1] = p1[1] - p0[1];
    w.v[2] = p1[2] - p0[2];
    PSVECCrossProduct(d0, d1, n.v);
    f32 nn = PSVECDotProduct(n.v, n.v);
    if (nn > lbl_80562348) {
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
    ca = *(RVec3*)(PTR(p, 0xC) + 0x24);
    cb = *(RVec3*)(PTR(q, 0xC) + 0x24);
    da = *(RVec3*)(PTR(p, 0xC) + 0x18);
    db = *(RVec3*)(PTR(q, 0xC) + 0x18);
    d.x = cb.x - ca.x;
    d.y = cb.y - ca.y;
    d.z = cb.z - ca.z;
    PSVECCrossProduct(&da.x, &db.x, &n.x);
    f32 nn = PSVECDotProduct(&n.x, &n.x);
    f32 la = F32(p, 0x28);
    f32 lb = F32(q, 0x28);
    if (nn < lbl_8056232C) {
        mid.x = (ca.x + cb.x) / 2.0f;
        mid.y = (ca.y + cb.y) / 2.0f;
        mid.z = (ca.z + cb.z) / 2.0f;
        w.x = mid.x - ca.x;
        w.y = mid.y - ca.y;
        w.z = mid.z - ca.z;
        s = PSVECDotProduct(&da.x, &w.x);
        w.x = mid.x - cb.x;
        w.y = mid.y - cb.y;
        w.z = mid.z - cb.z;
        t = PSVECDotProduct(&db.x, &w.x);
    } else {
        if (!((f32)__fabs(nn) <= lbl_80558DC4[0])) {
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
    if ((f32)__fabs(dist) <= lbl_80558DC4[0]) {
        return 0;
    }
    f32 pen = (F32(p, 0x24) + F32(q, 0x24)) - dist;
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
    bool hit = fn_8019D618(p, q, 0, 0);
    if (hit) {
        return 1;
    }
    PVec3 a;
    PVec3 b;
    PVec3 dir = *(PVec3*)(PTR(p, 0xC) + 0x18);
    PVec3 mid = *(PVec3*)(PTR(p, 0xC) + 0x24);
    f32 h = F32(p, 0x28);
    MulAdd(a.v, mid.v, dir.v, h);
    MulAdd(b.v, mid.v, dir.v, -h);
    f32 r = F32(p, 0x24);
    hit |= fn_8019B08C(p, a.v, q, r);
    hit |= fn_8019B08C(p, b.v, q, r);
    return hit;
}

// Capsule p against capsule q: bounding spheres, then the axis segments, then p's end spheres.
int fn_8019DC04(u8* p, u8* q) {
    PVec3 d;
    PVec3 a;
    PVec3 b;
    PVec3 axis;
    PVec3 org;
    u8* fq = PTR(q, 0xC);
    u8* fp = PTR(p, 0xC);
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
        u8* m = PTR(p, 0xC);
        f32 h = F32(p, 0x28);
        axis = *(PVec3*)(m + 0x18);
        org = *(PVec3*)(m + 0x24);
        f32 rad = F32(p, 0x24);
        MulAdd(a.v, org.v, axis.v, h);
        MulAdd(b.v, org.v, axis.v, -h);
        hit |= fn_8019CCCC(p, (u8*)a.v, q, rad);
        if (hit) {
            return 1;
        }
        hit |= fn_8019CCCC(p, (u8*)b.v, q, rad);
        return hit;
    }
    return 0;
}

f32 fn_8019DDEC(u8* p) { return F32(p, 0x24) + F32(p, 0x28); }

extern const f32 lbl_805622D0;  // 0.0

// Ray from + t*dir against the sphere (c, r): the nearer hit if it lies in [0, *t), with its normal.
int fn_8019DDFC(const f32* from, const f32* dir, f32* t, f32* n, const f32* c, f32 r) {
    PVec3 m;
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
    PVec3 r;
    PVec3 nn;
    PVec3 un;
    RVec3 rn;
    RVec3 t;
    PVec3 u;
    f32 rx = at->x - F32(com, 0x0);
    f32 ry = at->y - F32(com, 0x4);
    f32 rz = at->z - F32(com, 0x8);
    f32 vx, vy, vz;
    vz = v->z;
    vy = v->y;
    vx = v->x;
    vel.x = vx;
    vel.y = vy;
    vel.z = vz;
    r.v[0] = rx;
    r.v[1] = ry;
    r.v[2] = rz;
    nn = *(const PVec3*)n;
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

extern f32 lbl_80558DC4[];       // FLT_EPSILON
extern const f32 lbl_805622D0;  // 0.0

// Clips the ray parameter range [*tmin, *tmax] against the slab |o + t*d| <= r.
int fn_8019E1F4(f32* tmin, f32* tmax, f32 o, f32 d, f32 r) {
    if ((f32)__fabs(d) <= lbl_80558DC4[0]) {
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
    f32 dx = from->x - m->t.x;
    f32 dy = from->y - m->t.y;
    f32 dz = from->z - m->t.z;
    lp.x = Dot3(dx, m->m[0], dy, m->m[1], dz, m->m[2]);
    lp.y = Dot3(dx, m->m[3], dy, m->m[4], dz, m->m[5]);
    lp.z = Dot3(dx, m->m[6], dy, m->m[7], dz, m->m[8]);
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
    PVec3 dir;
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
        vt = lbl_80333918;
    }
};

// p against box q's edges: the four edges along each of q's x and z axes are tested as tubes of
// radius 0.75 inset by that radius.
int fn_8019E500(u8* p, u8* q) {
    CFrame fr = *(CFrame*)VE(q)->v50();
    BoxAxis ax[3] = {0};
    ax[0].dir = *(PVec3*)&fr.m[0];
    ax[0].ext = F32(q, 0x24);
    ax[1].dir = *(PVec3*)&fr.m[3];
    ax[1].ext = F32(q, 0x28);
    ax[2].dir = *(PVec3*)&fr.m[6];
    ax[2].ext = F32(q, 0x2C);
    bool hit = false;
    f32 r = 0.75f;
    for (int i = 0; i < 3; i++) {
        if (i == 1) {
            continue;
        }
        EdgeFrame e;
        BoxAxis* aa = &ax[(i + 1) % 3];
        BoxAxis* ab = &ax[(i + 2) % 3];
        BoxAxis* ai = &ax[i];
        e.axis.x = ai->dir.v[0];
        e.axis.y = ai->dir.v[1];
        e.axis.z = ai->dir.v[2];
        f32 ea = aa->ext - r;
        f32 eb = ab->ext - r;
        e.b.x = ab->dir.v[0];
        e.b.y = ab->dir.v[1];
        e.b.z = ab->dir.v[2];
        e.a.x = aa->dir.v[0];
        e.a.y = aa->dir.v[1];
        e.a.z = aa->dir.v[2];
        for (int j = 0; j < 4; j++) {
            e.pos = fr.t;
            f32 k = ea * ((j & 1) ? -1.0f : 1.0f);
            e.pos.x += aa->dir.v[0] * k;
            e.pos.y += aa->dir.v[1] * k;
            e.pos.z += aa->dir.v[2] * k;
            k = eb * ((j & 2) ? -1.0f : 1.0f);
            e.pos.x += ab->dir.v[0] * k;
            e.pos.y += ab->dir.v[1] * k;
            e.pos.z += ab->dir.v[2] * k;
            TubeVol* t = (TubeVol*)lbl_80435020;
            if (t) {
                t->Init(&e, U32(q, 0x4), F32(q, 0x10), F32(q, 0x14), r, ai->ext);
            }
            hit |= fn_8019D618((u8*)t, p, 1, q);
        }
    }
    return hit;
}

u8* fn_8019E834(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

// Capsule p against q: the point of p's axis nearest q's centre (clamped to +-half length 0x28) is
// tested as a sphere of p's radius; types 0x73 / 0x74 take fn_8019E500 instead.
// Body type ids (0x20 of a body); the two values here take the edge-tube test.
enum SphBodyType { SPH_TYPE_73 = 0x73, SPH_TYPE_74 = 0x74 };

int fn_8019E874(u8* p, u8* q) {
    if ((SphBodyType)S32(p, 0x20) == SPH_TYPE_73 || (SphBodyType)S32(p, 0x20) == SPH_TYPE_74) {
        return fn_8019E500(p, q);
    }
    CFrame oq;
    CFrame fp;
    RVec3 c;
    RVec3 d;
    oq = *(CFrame*)VE(q)->v50();
    fp = *(CFrame*)PTR(p, 0xC);
    c = fp.t;
    d.x = c.x - oq.t.x;
    d.y = c.y - oq.t.y;
    d.z = c.z - oq.t.z;
    f32 nh;
    f32 h = F32(p, 0x28);
    nh = -h;
    f32 s = -PSVECDotProduct(&fp.m[6], &d);
    f32 v = h < s ? h : s;
    v = v > nh ? v : nh;
    c.x = c.x + fp.m[6] * v;
    c.y = c.y + fp.m[7] * v;
    c.z = c.z + fp.m[8] * v;
    return fn_8019B4D8(p, q, &c, 0, F32(p, 0x24));
}

// Capsule p against q, with the centre offset kept for a debug view that is compiled out.
int fn_8019EA7C(u8* p, u8* q) {
    CFrame* fq = (CFrame*)VE(q)->v50();
    CFrame* fp = (CFrame*)PTR(p, 0xC);
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

void fn_8019EBC0(u8* p) {
    u8* o = PTR(p, 0x4);
    if (o) {
        VE(o)->v48();
    }
}

void fn_8019EBE0(u8* p, void* a, const RVec3* b, const RVec3* c, int d, const RVec3* e, void* f, f32 g) {
    if (PTR(p, 0x4)) {
        ((RigidBody*)PTR(p, 0x4))->Push((Phob*)p, a, b, c, d, e, f, g);
    } else if (PTR(p, 0x8)) {
        fn_801A4A10(PTR(p, 0x8), c, f, b);
    }
}

void fn_8019EC84(u8* p) {
    u8* o = PTR(p, 0x4);
    if (o) {
        VE(o)->v4C();
    }
}

void fn_8019ECA4(u8* p, u8* q) {
    PTR(p, 0x8) = q;
    VE(p)->v34(q + 4);
}

void fn_8019ECBC(u8* p, u32 v) { U32(p, 0xc) = v; }

extern void* lbl_80333B98[];

// Collision volume base constructor from file data (byte-swapped when `swap`).
void fn_8019ECC4(u8* p, u32 a, u32 swap) {
    *(void**)p = lbl_80333B98;
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

extern void* lbl_80333B48[];

// Sphere volume constructor from file data (byte-swapped when `swap`).
u8* fn_8019EDB8(u8* p, u32 a, u32 swap) {
    fn_8019ECC4(p, a, swap);
    *(void**)p = lbl_80333B48;
    if (swap) {
        Swap32(p + 0x24);
        f32* w = (f32*)(p + 0x28);
        for (int i = 0; i < 3; i++) {
            Swap32((u8*)&w[i]);
        }
        if (S32(p, 0x20) == 0x3EB) {
            F32(p, 0x24) += 3.35f;
        }
    }
    U8(p, 0x4C) = 0;
    F32(p, 0x34) = 0.0f;
    F32(p, 0x38) = 0.0f;
    F32(p, 0x3C) = 0.0f;
    F32(p, 0x40) = 0.0f;
    F32(p, 0x44) = 0.0f;
    F32(p, 0x48) = 0.0f;
    F32(p, 0x50) = 0.0f;
    return p;
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
void fn_8019EF00(CVolume* v) {
    CFrame* f = v->frame;
    MulVec(&v->world, &v->local, f);
    v->world.x = v->world.x + f->t.x;
    v->world.y = v->world.y + f->t.y;
    v->world.z = v->world.z + f->t.z;
}

void fn_8019F03C(u8* p) {
    U8(p, 0x4C) = 0;
    F32(p, 0x40) = 0.0f;
    F32(p, 0x44) = 0.0f;
    F32(p, 0x48) = 0.0f;
    F32(p, 0x34) = 0.0f;
    F32(p, 0x38) = 0.0f;
    F32(p, 0x3C) = 0.0f;
}

void fn_802E317C(const char* fmt, ...);

// Collides sphere `p` against volume `o` by its type tag.
void fn_8019F064(u8* p, u8* o) {
    switch (S32(o, 0x18)) {
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
        fn_802E317C("Unsupported collision type: Sphere-%x", S32(o, 0x18));
        break;
    }
}

void fn_8019F1A4(u8* p) { fn_8019AC44(p, (f32*)(p + 0x34), (f32*)(p + 0x40), F32(p, 0x24)); }

void fn_8019F1B4(u8* p, u32 a, u32 b, u32 c, u32 d) { fn_8019DDFC((f32*)a, (f32*)b, (f32*)c, (f32*)d, (f32*)(p + 0x34), F32(p, 0x24)); }

void fn_8019F1D4(const u8* p, RVec3* lo, RVec3* hi) {
    RVec3 c;
    RVec3 e;
    f32 r = F32(p, 0x24);
    e.x = r;
    e.y = r;
    e.z = r;
    c.x = F32(p, 0x34);
    c.y = F32(p, 0x38);
    c.z = F32(p, 0x3C);
    MulAdd(&lo->x, &c.x, &e.x, lbl_805622DC);
    MulAdd(&hi->x, &c.x, &e.x, lbl_805622E0);
}

void fn_8019F248(u8* p) {
    f32 r = F32(p, 0x24);
    r = r > 0.2f ? r : 0.2f;
    fn_801E3FC8(p + 0x34, lbl_8055E598.value, r);
}

void fn_802E30C8(const char* fmt, ...);

// Collides flat sphere `p` against volume `o` by its type tag.
void fn_8019F26C(u8* p, u8* o) {
    switch (S32(o, 0x18)) {
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
        *(s32*)tag = S32(o, 0x18);
        fn_802E30C8("flatsphere: Can't collide with type %s", tag);
        break;
    }
    }
}

void fn_8019F3D0(u8* p) {}

u8* fn_8019F3D4(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

extern const f32 lbl_8056237C;

// Re-centres the tube on its attachment (offset by 0x58 / lift), then refreshes it.
void fn_8019F414(u8* p) {
    u8* o = PTR(p, 0x4);
    u8* m = PTR(p, 0x54);
    PVec3 t = *(PVec3*)(m + 0x30);
    f32 lift = lbl_8056237C + F32(p, 0x24) + F32(m, 0xC);
    t.v[0] = t.v[0] - F32(o, 0x30) + F32(p, 0x58);
    t.v[1] = t.v[1] - F32(o, 0x34) + lift;
    t.v[2] = t.v[2] - F32(o, 0x38);
    F32(p, 0x28) = t.v[0];
    F32(p, 0x2C) = t.v[1];
    F32(p, 0x30) = t.v[2];
    if (o) {
        fn_8019EECC(o, (RVec3*)(p + 0x28));
    }
    fn_8019EF00((CVolume*)p);
}

// Projects v onto the plane of p's frame row 1 (the result feeds a debug view that is compiled out).
void fn_8019F4D4(u8* p, const RVec3* v) {
    fn_8019EC84(p);
    f32 d = -PSVECDotProduct(&((CFrame*)PTR(p, 0xC))->m[3], v);
    CFrame* f = (CFrame*)PTR(p, 0xC);
    f32 x = v->x + f->m[3] * d;
    f32 y = v->y + f->m[4] * d;
    f32 z = v->z + f->m[5] * d;
    BoxCorner c(x, y, z);
}

void fn_8019F560(u8* p) { fn_8019F1A4(p); }

static inline void InitVolume(u8* s, u32 id, u32 owner) {
    U32(s, 0xC) = id;
    U32(s, 0x4) = owner;
}

extern void* lbl_80333A58[];

// Box volume (half sizes hx, hy, hz): a sphere bounding the largest half size, then the box fields.
u8* fn_8019F564(u8* p, u32 frame, const RVec3* c, u32 owner, f32 k, f32 skin, f32 a, f32 hx, f32 hy, f32 hz) {
    f32 cz = c->z;
    f32 big = hy > hz ? hy : hz;
    f32 cy = c->y;
    big = hx > big ? hx : big;
    f32 cx = c->x;
    U32(p, 0x8) = 0;
    InitVolume(p, frame, owner);
    S32(p, 0x18) = 'SPHR';
    F32(p, 0x10) = k;
    F32(p, 0x14) = skin;
    S32(p, 0x20) = 100;
    U32(p, 0x1C) = 0;
    *(void**)p = lbl_80333B48;
    F32(p, 0x24) = 0.5f * big;
    F32(p, 0x34) = 0.0f;
    F32(p, 0x38) = 0.0f;
    F32(p, 0x3C) = 0.0f;
    F32(p, 0x40) = 0.0f;
    F32(p, 0x44) = 0.0f;
    F32(p, 0x48) = 0.0f;
    U8(p, 0x4C) = 0;
    F32(p, 0x50) = 0.0f;
    F32(p, 0x28) = cx;
    F32(p, 0x2C) = cy;
    F32(p, 0x30) = cz;
    if (owner) {
        fn_8019EECC((u8*)owner, (RVec3*)(p + 0x28));
    }
    *(void**)p = lbl_80333A58;
    F32(p, 0x54) = a;
    F32(p, 0x58) = 0.5f * hx;
    F32(p, 0x5C) = 0.5f * hy;
    F32(p, 0x60) = 0.5f * hz;
    F32(p, 0x64) = 0.0f;
    F32(p, 0x68) = 0.0f;
    F32(p, 0x6C) = 0.0f;
    F32(p, 0x70) = 0.0f;
    F32(p, 0x74) = 0.0f;
    F32(p, 0x78) = 0.0f;
    F32(p, 0x7C) = 0.0f;
    F32(p, 0x80) = 0.0f;
    F32(p, 0x84) = 0.0f;
    F32(p, 0x88) = 0.0f;
    F32(p, 0x8C) = 0.0f;
    F32(p, 0x90) = 0.0f;
    F32(p, 0x94) = 0.0f;
    F32(p, 0x98) = 0.0f;
    F32(p, 0x9C) = 0.0f;
    F32(p, 0xA0) = 0.0f;
    F32(p, 0xA4) = 0.0f;
    F32(p, 0xA8) = 0.0f;
    F32(p, 0xAC) = 0.0f;
    F32(p, 0xB0) = 0.0f;
    F32(p, 0xB4) = 0.0f;
    F32(p, 0xB8) = 0.0f;
    F32(p, 0xBC) = 0.0f;
    F32(p, 0xC0) = 0.0f;
    return p;
}

void fn_8019F6F0(u8* p) {
    fn_8019F03C(p);
    for (int i = 0; i < 24; i++) {
        ((SphCache*)p)->v[i] = 0.0f;
    }
}

void fn_8019F780(u8* p) {}  // stub (514)

void fn_8019FF88(u8* p) { fn_8019EBC0(p); }

void fn_8019FF8C(u8* p) {
    fn_8019F03C(p);
    F32(p, 0x28) = F32(p, 0x54);
    F32(p, 0x2C) = F32(p, 0x58);
    F32(p, 0x30) = F32(p, 0x5C);
    U8(p, 0x78) = 0;
}

u8* fn_8019FFD8(u8* p, u32 a, u32 b) {
    U32(p, 0xC) = a;
    U32(p, 0x4) = b;
    U32(p, 0x8) = 0;
    U32(p, 0x18) = 'GRUP';
    F32(p, 0x10) = 0.0f;
    F32(p, 0x14) = 0.0f;
    U32(p, 0x20) = 100;
    U32(p, 0x1C) = 0;
    *(void**)p = lbl_803339B8;
    F32(p, 0x170) = 0.0f;
    F32(p, 0x178) = 0.0f;
    F32(p, 0x16C) = 0.0f;
    F32(p, 0x168) = 0.0f;
    F32(p, 0x164) = 0.0f;
    U32(p, 0x174) = 0;
    U32(p, 0x17C) = 0;
    S32(p, 0x180) = -1;
    return p;
}

u8* fn_801A0040(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_803339B8;
        for (int i = 0; i < S32(p, 0x174); i++) {
            delete ((SphGroupDel*)p)->vols[i];
        }
        delete *(VDel**)(p + 0x17C);
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

u8* fn_801A010C(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_801A014C(u8* p) { VE(PTR(p, 0x17C))->v0C(); }

void fn_801A0160(u8* p) {
    if (lbl_8055E3C4 != S32(p, 0x180)) {
        S32(p, 0x180) = lbl_8055E3C4;
        for (int i = 0; i < S32(p, 0x174); i++) {
            ((SphGroup*)p)->vols[i]->v0C();
        }
    }
}

void fn_801A01E0(const u8* p, RVec3* lo, RVec3* hi) {
    RVec3 c;
    RVec3 e;
    f32 r = F32(p, 0x178);
    u8* o = PTR(p, 0xC);
    e.x = r;
    e.y = r;
    e.z = r;
    c.x = F32(o, 0x24);
    c.y = F32(o, 0x28);
    c.z = F32(o, 0x2C);
    MulAdd(&lo->x, &c.x, &e.x, lbl_805622DC);
    MulAdd(&hi->x, &c.x, &e.x, lbl_805622E0);
}

extern void* lbl_80333B48[];

void fn_801A0378(u8* p, u8* s);
extern const f32 lbl_805622D0;  // 0.0

// Adds a sphere part (radius r) at c to the group.
void fn_801A0258(u8* grp, const RVec3* c, f32 r, f32 s0, f32 s1) {
    u8* s = (u8*)operator new(0x54, "SphereGroupPart");
    if (s) {
        f32 cz = c->z;
        f32 cy = c->y;
        f32 cx = c->x;
        InitVolume(s, U32(grp, 0xC), U32(grp, 0x4));
        U32(s, 0x8) = 0;
        S32(s, 0x18) = 'SPHR';
        F32(s, 0x10) = s0;
        F32(s, 0x14) = s1;
        S32(s, 0x20) = 100;
        U32(s, 0x1C) = 0;
        *(void**)s = lbl_80333B48;
        F32(s, 0x24) = r;
        F32(s, 0x34) = lbl_805622D0;
        F32(s, 0x38) = lbl_805622D0;
        F32(s, 0x3C) = lbl_805622D0;
        F32(s, 0x40) = lbl_805622D0;
        F32(s, 0x44) = lbl_805622D0;
        F32(s, 0x48) = lbl_805622D0;
        U8(s, 0x4C) = 0;
        F32(s, 0x50) = lbl_805622D0;
        F32(s, 0x28) = cx;
        F32(s, 0x2C) = cy;
        F32(s, 0x30) = cz;
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
    e[0] = F32(s, 0x28);
    e[1] = F32(s, 0x2C);
    e[2] = F32(s, 0x30);
    e[3] = F32(s, 0x24);
    g->vols[g->count]->SetA(g->setA);
    f32 m = PSVECMag((f32*)(s + 0x28));
    f32 r = 0.5f + (m + F32(s, 0x24));
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

void fn_801A0464(u8* p) {}  // stub (683)

extern const f32 lbl_805622E0;  // 1.0
void fn_801BEAEC(int car, int wheel, PVec3* out);                       // wheel position (masslist.cpp)
void fn_801BEB44(int car, int wheel, const RVec3* at, const PVec3* n);  // wheel contact (masslist.cpp)

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

// Per-frame update of the sphere group; with a track mesh loaded and a car owner, each of its 4 wheels
// (moved by the car's velocity) is tested against the mesh and every hit reported to the car.
void fn_801A0F10(u8* p) {
    RVec3* h;
    RVec3 hits[64];
    PVec3 wp;
    PVec3 c;
    PVec3 d;
    int n;
    u8* car;
    fn_801A0160(p);
    for (int i = 0; i < S32(p, 0x174); i++) {
        ((SphGroup*)p)->vols[i]->v14();
    }
    if (lbl_8055E5A0) {
        car = PTR(p, 0x4);
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
                h = hits;
                for (int k = 0; k < n; h++, k++) {
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
int fn_801A1148(u8* p, u32 from, u32 dir, u32 t, u32 out) {
    const f32* c = (const f32*)(PTR(p, 0xC) + 0x24);
    const f32* f = (const f32*)from;
    f32 fy;
    f32 fx;
    f32 fz;
    fx = f[0];
    fy = f[1];
    fz = f[2];
    f32 d[3];
    d[0] = c[0] - fx;
    d[1] = c[1] - fy;
    d[2] = c[2] - fz;
    f32 proj = PSVECDotProduct(d, (void*)dir);
    f32 rad = F32(p, 0x178);
    if (proj >= -rad && proj < *(f32*)t + rad) {
        fn_801A0160(p);
        int hit = 0;
        for (int i = 0; i < S32(p, 0x174); i++) {
            u8* s = (u8*)((SphGroupDel*)p)->vols[i];
            if (fn_8019DDFC((f32*)from, (f32*)dir, (f32*)t, (f32*)out, (f32*)(s + 0x34), F32(s, 0x24))) {
                hit = 1;
            }
        }
        return hit;
    }
    return 0;
}

void fn_801A1250(u8* p) {
    u8* q = p;
    for (int i = 0; i < S32(p, 0x174); i++) {
        VE(PTR(q, 0x24))->v28();
        q += 4;
    }
    S32(p, 0x180) = -1;
}

void fn_801A12C4(u8* p, u32 v) {
    U32(p, 0x1C) = v;
    ((VSet*)PTR(p, 0x17C))->SetB(v);
    for (int i = 0; i < S32(p, 0x174); i++) {
        ((SphGroupSet*)p)->vols[i]->SetB(v);
    }
}

void fn_801A1358(u8* p, u32 v) { U32(p, 0x1c) = v; }

void fn_801A1360(u8* p, u32 v) {
    U32(p, 0x20) = v;
    ((VSet*)PTR(p, 0x17C))->SetA(v);
    for (int i = 0; i < S32(p, 0x174); i++) {
        ((SphGroupSet*)p)->vols[i]->SetA(v);
    }
}

void fn_801A13F4(u8* p) {
    ((VEx*)PTR(p, 0x17C))->v4C();
    for (int i = 0; i < S32(p, 0x174); i++) {
        ((SphGroup*)p)->vols[i]->v4C();
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

u8* fn_801A1520(u8* p, u32 a, u32 swap) {
    fn_8019ECC4(p, a, swap);
    *(void**)p = lbl_80333968;
    if (swap) {
        Swap32(p + 0x24);
        Swap32(p + 0x28);
        if (S32(p, 0x20) == 0x3EB) {
            F32(p, 0x24) += 3.35f;
        }
    }
    return p;
}

// Ray from + t*dir against the capsule's side (axis clamped to +-half length 0x28, radius 0x24):
// the hit if it lies in (0, *t), with its normal.
static inline f32 ClampF(f32 v, f32 lo, f32 hi) {
    v = hi < v ? hi : v;
    return v > lo ? v : lo;
}

int fn_801A15CC(u8* p, const f32* from, const f32* dir, f32* t, f32* n) {
    PVec3 hit;
    PVec3 on;
    f32 tr = 0.0f;
    f32 s = 0.0f;
    u8* m = PTR(p, 0xC);
    f32* axis = (f32*)(m + 0x18);
    f32* org = (f32*)(m + 0x24);
    if (fn_8019D484(from, dir, org, axis, &tr, &s)) {
        s = ClampF(s, -F32(p, 0x28), F32(p, 0x28));
        if (tr > 0.0f && tr < *t) {
            MulAdd(hit.v, from, dir, tr);
            MulAdd(on.v, org, axis, s);
            f32 d2 = PSVECSquareDistance(hit.v, on.v);
            f32 r = F32(p, 0x24);
            if (d2 < r * r) {
                n[0] = hit.v[0] - on.v[0];
                n[1] = hit.v[1] - on.v[1];
                n[2] = hit.v[2] - on.v[2];
                f32 len = PSVECMag(n);
                f32 k = lbl_805622D4;
                f32 inv = 1.0f / (k > len ? k : len);
                n[0] *= inv;
                n[1] *= inv;
                n[2] *= inv;
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
void fn_801A17B8(u8* p) {
    TubeSeg s = *(TubeSeg*)(PTR(p, 0xC) + 0x18);
    f32 a[3];
    f32 b[3];
    MulAdd(a, s.mid, s.dir, F32(p, 0x28));
    MulAdd(b, s.mid, s.dir, -F32(p, 0x28));
    fn_801E41D8(a, b, 0x40FFFF00, lbl_8056234C * F32(p, 0x24));
}

u8* fn_801A1898(u8* p, u32 a, u32 swap) {
    fn_801A1520(p, a, swap);
    *(void**)p = lbl_80333918;
    return p;
}

// Axis-aligned bounds of the volume: both axis end points (org +- axis * k), widened by k.
void fn_801A18D4(u8* p, RVec3* lo, RVec3* hi) {
    PVec3 org;
    PVec3 axis;
    RVec3 a;
    RVec3 b;
    u8* m = PTR(p, 0xC);
    f32 k = F32(p, 0x24);
    axis = *(PVec3*)(m + 0x18);
    org = *(PVec3*)(m + 0x24);
    MulAdd(&b.x, org.v, axis.v, -k);
    MulAdd(&a.x, org.v, axis.v, k);
    {
        RVec3 mn;
        RVec3 la;
        RVec3 lb;
        lb.x = b.x - k;
        lb.y = b.y - k;
        lb.z = b.z - k;
        la.x = a.x - k;
        la.y = a.y - k;
        la.z = a.z - k;
        SetV(&mn, MinF(la.x, lb.x), MinF(la.y, lb.y), MinF(la.z, lb.z));
        *lo = mn;
    }
    {
        RVec3 mx;
        RVec3 ha;
        RVec3 hb;
        hb.x = b.x + k;
        hb.y = b.y + k;
        hb.z = b.z + k;
        ha.x = a.x + k;
        ha.y = a.y + k;
        ha.z = a.z + k;
        SetV(&mx, MaxF(ha.x, hb.x), MaxF(ha.y, hb.y), MaxF(ha.z, hb.z));
        *hi = mx;
    }
}

void fn_802E30C8(const char* fmt, ...);

// Collides `p` against volume `o` by its type tag.
void fn_801A1ABC(u8* p, u8* o) {
    switch (S32(o, 0x18)) {
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
        fn_8019EA7C(p, PTR(o, 0x17C));
        break;
    default:
        fn_802E30C8("Unsupported collision type: Tube-%x", S32(o, 0x18));
        break;
    }
}

// Collides the tube's two end caps as spheres.
void fn_801A1B9C(u8* p) {
    PVec3 a;
    PVec3 b;
    PVec3 zero;
    PVec3 dir = *(PVec3*)(PTR(p, 0xC) + 0x18);
    PVec3 mid = *(PVec3*)(PTR(p, 0xC) + 0x24);
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

void fn_801A1CA0(const u8* p) {
    if (S32(p, 0x20) < 1000) {
        fn_801E44F4(PTR(p, 0xC), 0x40FFFF00, F32(p, 0x24), F32(p, 0x24), F32(p, 0x28));
    } else {
        fn_801E44F4(PTR(p, 0xC), 0x4000FF80, F32(p, 0x24), F32(p, 0x24), F32(p, 0x28));
    }
}

u8* fn_801A1CE4(u8* p, u32 a, u32 swap) {
    fn_8019ECC4(p, a, swap);
    *(void**)p = lbl_803338C0;
    if (swap) {
        Swap32(p + 0x24);
        Swap32(p + 0x28);
        Swap32(p + 0x2C);
        Swap32(p + 0x30);
        Swap32(p + 0x34);
    }
    return p;
}

void fn_801A1DD4(u8* a, u8* obj) {
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

int fn_801A1E7C(u8* p, u8* a, u8* b, u8* c, RVec3* out) {
    f32 z = F32(p, 0x2C);
    f32 y = F32(p, 0x28);
    f32 x = F32(p, 0x24);
    if (fn_8019E2D0((RVec3*)a, (RVec3*)b, (f32*)c, (CFrame*)((VEx*)p)->v50(), x, y, z)) {
        out->x = 0.0f;
        out->y = 1.0f;
        out->z = 0.0f;
        return 1;
    }
    return 0;
}

void fn_801A1F44(RVec3* out, u8* q) {
    u8* o = VE(q)->v50();
    *(PVec3*)out = *(PVec3*)(o + 0x24);
}

// Axis-aligned bounds of the box: its 8 corners (+-hx, +-hy, +-hz) through the volume's frame.
void fn_801A1F98(u8* p, RVec3* lo, RVec3* hi) {
    f32 hx = F32(p, 0x24);
    f32 hy = F32(p, 0x28);
    f32 hz = F32(p, 0x2C);
    f32 nx = -hx;
    f32 ny = -hy;
    f32 nz = -hz;
    RVec3 c[8] = {
        {hx, hy, hz},   {nx, hy, hz},   {hx, ny, hz},   {nx, ny, hz},
        {hx, hy, nz},   {nx, hy, nz},   {hx, ny, nz},   {nx, ny, nz},
    };
    RVec3 w;
    *lo = ((CFrame*)VE(p)->v50())->t;
    *hi = ((CFrame*)VE(p)->v50())->t;
    for (int i = 0; i < 8; i++) {
        CFrame* m = (CFrame*)VE(p)->v50();
        MulVec(&w, &c[i], m);
        w.x = w.x + m->t.x;
        w.y = w.y + m->t.y;
        w.z = w.z + m->t.z;
        lo->x = lo->x < w.x ? lo->x : w.x;
        lo->y = lo->y < w.y ? lo->y : w.y;
        lo->z = lo->z < w.z ? lo->z : w.z;
        hi->x = hi->x > w.x ? hi->x : w.x;
        hi->y = hi->y > w.y ? hi->y : w.y;
        hi->z = hi->z > w.z ? hi->z : w.z;
    }
}

void fn_801A22A4(u8* p) {
    Xform* src = (Xform*)((VEx*)p)->v50();
    Xform x;
    x.rot = src->rot;
    x.pos = src->pos;
    const RVec3* c = (const RVec3*)(p + 0x24);
    f32 cz = c->z;
    f32 cy = c->y;
    f32 cx = c->x;
    fn_801E44F4((u8*)&x, 0x50000000, cx, cy, cz);
}

u8* fn_801A2354(u8* p, u32 a, const RVec3* c, u32 b, f32 s0, f32 s1, f32 hx, f32 hy, f32 hz) {
    f32 z = hz - 0.2f;
    f32 y = hy - 0.2f;
    f32 x = hx - 0.2f;
    F32(p, 0x10) = s0;
    U32(p, 0xC) = a;
    U32(p, 0x4) = b;
    U32(p, 0x8) = 0;
    S32(p, 0x18) = 'BOX ';
    F32(p, 0x14) = s1;
    S32(p, 0x20) = 100;
    U32(p, 0x1C) = 0;
    *(void**)p = lbl_803338C0;
    F32(p, 0x24) = x;
    F32(p, 0x28) = y;
    F32(p, 0x2C) = z;
    U32(p, 0x34) = 0;
    F32(p, 0x30) = 0.2f + (f32)sqrt(z * z + (x * x + y * y));
    F32(p, 0x38) = c->x;
    *(void**)p = lbl_80333868;
    F32(p, 0x3C) = c->y;
    F32(p, 0x40) = c->z;
    return p;
}

u8* fn_801A2434(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

// Takes the frame's rotation (0x44), then the world centre (0x68) = frame * local centre (0x38).
void fn_801A2474(u8* p) {
    CFrame* f = (CFrame*)PTR(p, 0xC);
    ((PVec3*)(p + 0x44))[0] = ((PVec3*)f->m)[0];
    ((PVec3*)(p + 0x44))[1] = ((PVec3*)f->m)[1];
    ((PVec3*)(p + 0x44))[2] = ((PVec3*)f->m)[2];
    MulVec((RVec3*)(p + 0x68), (RVec3*)(p + 0x38), f);
    F32(p, 0x68) = F32(p, 0x68) + f->t.x;
    F32(p, 0x6C) = F32(p, 0x6C) + f->t.y;
    F32(p, 0x70) = F32(p, 0x70) + f->t.z;
}

u8* fn_801A25F8(u8* p, u32 a, u32 b, u32 c, f32 r) {
    U32(p, 0xC) = a;
    U32(p, 0x30) = b;
    U32(p, 0x4) = c;
    U32(p, 0x8) = 0;
    S32(p, 0x18) = 'RAGD';
    F32(p, 0x10) = lbl_805622E0;
    F32(p, 0x14) = lbl_805622E0;
    S32(p, 0x20) = 100;
    U32(p, 0x1C) = 0;
    *(void**)p = lbl_80333818;
    fn_801A2674(p, r, 0, -1);
    return p;
}

void fn_801A2674(u8* p, f32 r, u32 a, u32 b) {
    F32(p, 0x24) = r;
    U32(p, 0x28) = a;
    U32(p, 0x2C) = b;
}

void fn_801A2684(const u8* p, RVec3* lo, RVec3* hi) {
    f32 r = F32(p, 0x24);
    u8* o = PTR(p, 0xC);
    RVec3 e;
    e.x = lbl_805622E0;
    e.y = lbl_805622E0;
    e.z = lbl_805622E0;
    MulAdd(&lo->x, (f32*)(o + 0x24), &e.x, -r);
    MulAdd(&hi->x, (f32*)(o + 0x24), &e.x, r);
}

void fn_801A26FC(u8* p) {}  // stub (360)

u8* fn_801A2C9C(u8* p) { return p + 0x44; }

f32 fn_801A2CA4(u8* p) { return F32(p, 0x178); }

void fn_801A2CAC(u8* p) { fn_801E3FC8(PTR(p, 0xC) + 0x24, lbl_8055E598.value, F32(p, 0x24)); }

u8* fn_801A2CC0(u8* p, u32 a, u32 swap) {
    fn_8019ECC4(p, a, swap);
    *(void**)p = lbl_803337C8;
    if (swap) {
        Swap32(p + 0x24);
        Swap32(p + 0x28);
    }
    return p;
}

extern char lbl_80333448[];  // this unit's string pool
extern u8 lbl_8055E5A4;        // warned: ring vs box
extern u8 lbl_8055E5A5;        // warned: ring vs ragdoll

// Collides ring `p` against volume `o` by its type tag.
void fn_801A2D50(u8* p, u8* o) {
    char* pool = lbl_80333448;
    switch (S32(o, 0x18)) {
    case 'POLY':
        break;
    case 'SPHR':
        fn_8019C854(o, p);
        break;
    case 'GRUP':
        for (int i = 0; i < S32(o, 0x174); i++) {
            fn_8019C854((u8*)((SphGroupDel*)o)->vols[i], p);
        }
        break;
    case 'BOX ':
        if (!lbl_8055E5A4) {
            lbl_8055E5A4 = 1;
            fn_802E30C8("%s", pool + 0x22C);  // "Unsupported collision type: Ring-box"
        }
        break;
    case 'RAGD':
        if (!lbl_8055E5A5) {
            lbl_8055E5A5 = 1;
            fn_802E30C8("%s", pool + 0x254);  // "Unsupported collision type: Ring-Ragdoll"
        }
        break;
    case 'FLSR':
        fn_8019D378(o, p);
        break;
    default:
        fn_802E317C(pool + 0x280, S32(o, 0x18));  // "Unsupported collision type: Ring-%x"
        break;
    }
}

void fn_801A2EC8(u8* p) {}

// Axis-aligned bounds: the 8 corners of the local box (+-(r + h), +-(r + h), +-h) through the frame.
void fn_801A2ECC(const u8* p, RVec3* lo, RVec3* hi) {
    f32 h = F32(p, 0x28);
    f32 a = F32(p, 0x24) + h;
    RVec3 c[8] = {
        {-a, -a, -h}, {-a, -a, h}, {-a, a, -h}, {-a, a, h},
        {a, -a, -h},  {a, -a, h},  {a, a, -h},  {a, a, h},
    };
    RVec3 w;
    *lo = ((CFrame*)PTR(p, 0xC))->t;
    *hi = ((CFrame*)PTR(p, 0xC))->t;
    for (int i = 0; i < 8; i++) {
        CFrame* m = (CFrame*)PTR(p, 0xC);
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

void fn_801A3174(u8* p) { fn_801E3FC8(PTR(p, 0xC) + 0x24, lbl_8055E598.value, F32(p, 0x24) + F32(p, 0x28)); }

u8* fn_801A3190(u8* p, u32 a, const PolyPlane* planes, s32 n, const RVec3* lo, const RVec3* hi, f32 f1, f32 f2) {
    U32(p, 0xC) = a;
    U32(p, 0x4) = 0;
    U32(p, 0x8) = 0;
    S32(p, 0x18) = 'POLY';
    F32(p, 0x10) = f1;
    F32(p, 0x14) = f2;
    S32(p, 0x20) = 100;
    U32(p, 0x1C) = 0;
    *(void**)p = lbl_80333778;
    U32(p, 0xDC) = 0;
    S32(p, 0xBC) = n;
    for (int i = 0; i < S32(p, 0xBC); i++) {
        ((PolyVol*)p)->planes[i].n = planes[i].n;
        ((PolyVol*)p)->planes[i].d = planes[i].d;
    }
    F32(p, 0x24) = lo->x;
    F32(p, 0x28) = lo->y;
    F32(p, 0x2C) = lo->z;
    F32(p, 0x30) = hi->x;
    F32(p, 0x34) = hi->y;
    F32(p, 0x38) = hi->z;
    return p;
}

ValueTable* fn_80131808(const char* name);  // open a .val table
void fn_80131818(ValueTable* table);        // release it

// Polyhedron volume ('POLY') from the named table: extents, then its "plane" (n, k) and
// "curve" (center, axis, radius) entries.
u8* fn_801A324C(u8* p, u32 id, const char* name, f32 s0, f32 s1) {
    int i;
    ValueNode* n;
    ValueTable* t;
    ValueNode* ext;
    U32(p, 0xC) = id;
    F32(p, 0x10) = s0;
    U32(p, 0x4) = 0;
    U32(p, 0x8) = 0;
    S32(p, 0x18) = 'POLY';
    F32(p, 0x14) = s1;
    S32(p, 0x20) = 100;
    U32(p, 0x1C) = 0;
    *(void**)p = lbl_80333778;
    t = fn_80131808(name);
    S32(p, 0xBC) = 0;
    S32(p, 0xDC) = 0;
    ext = t->FindChild("extents");
    *(ValVec3*)(p + 0x24) = ext->GetVec3("min");
    *(ValVec3*)(p + 0x30) = ext->GetVec3("max");
    for (i = 0; (n = (ValueNode*)t->GetChild(i)) != 0; i++) {
        if (strcmp(n->GetName(), "plane") == 0) {
            u8* pl = p + S32(p, 0xBC)++ * 0x10;
            *(ValVec3*)(pl + 0x3C) = n->GetVec3("n");
            F32(pl, 0x48) = n->GetFloat("k");
        } else if (strcmp(n->GetName(), "curve") == 0) {
            u8* c = p + S32(p, 0xDC)++ * 0x1C;
            *(ValVec3*)(c + 0xC0) = n->GetVec3("center");
            *(ValVec3*)(c + 0xCC) = n->GetVec3("axis");
            F32(c, 0xD8) = n->GetFloat("radius");
        }
    }
    fn_80131818(t);
    return p;
}

// Axis-aligned bounds of the extents (0x24 min, 0x30 max) through the frame, padded by 1.5 in x and z.
void fn_801A34D0(u8* p, RVec3* lo, RVec3* hi) {
    RVec3 b;
    RVec3 a;
    CFrame* f = (CFrame*)PTR(p, 0xC);
    MulVec(&a, (RVec3*)(p + 0x24), f);
    a.x = a.x + f->t.x;
    a.y = a.y + f->t.y;
    a.z = a.z + f->t.z;
    CFrame* g = (CFrame*)PTR(p, 0xC);
    MulVec(&b, (RVec3*)(p + 0x30), g);
    b.x = b.x + g->t.x;
    b.y = b.y + g->t.y;
    b.z = b.z + g->t.z;
    *hi = a;
    *lo = *hi;
    lo->x = lo->x < b.x ? lo->x : b.x;
    lo->y = lo->y < b.y ? lo->y : b.y;
    lo->z = lo->z < b.z ? lo->z : b.z;
    hi->x = hi->x > b.x ? hi->x : b.x;
    hi->y = hi->y > b.y ? hi->y : b.y;
    hi->z = hi->z > b.z ? hi->z : b.z;
    hi->x += 1.5f;
    hi->z += 1.5f;
    lo->x -= 1.5f;
    lo->z -= 1.5f;
}

f32 fn_801A3880(u8* p) {
    f32 a = PSVECDotProduct(p + 0x30, p + 0x30);
    f32 b = PSVECDotProduct(p + 0x24, p + 0x24);
    return (f32)sqrt(b > a ? b : a);
}

// Sphere (and flat sphere 'FLSR') against the world: each point attached to the owner body
// (lifted 0.25), then the sphere itself; contacts go to fn_8019A788.
// The owner body as seen here: its frame at 0x3C and its attached points.
struct OwnerBody {
    u8 pad0[0x3C];
    CFrame frame;  // offset 0x3C
};

void fn_801A38E4(u32 world, u8* p) {
    RVec3 pt;
    PVec3 n1;
    PVec3 hit1;
    PVec3 n2;
    PVec3 hit2;
    PVec3 vel1;
    PVec3 vel2;
    PVec3 down;
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
    case 'SPHR': {
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
                    if (fn_80198F64(&pt.x, world, hit1.v, &depth1, n1.v, vel1.v, 0.25f)) {
                        fn_8019A788(p, (u8*)world, (u32)hit1.v, (RVec3*)n1.v, 0, 0, depth1, 0.0f, 0.0f);
                    }
                }
            }
        }
        vel2.v[0] = 0.0f;
        vel2.v[1] = 0.0f;
        vel2.v[2] = 0.0f;
        if (fn_80198F64((f32*)(p + 0x34), world, hit2.v, &depth2, n2.v, vel2.v, F32(p, 0x24))) {
            fn_8019A788(p, (u8*)world, (u32)hit2.v, (RVec3*)n2.v, 0, 0, depth2, 0.0f, 0.0f);
        }
        break;
    }
    }
}


// Debug draw: a line from the frame origin along each of the volume's rays (direction * length, frame space).
void fn_801A3BE0(u8* p) {
    PVec3 o;
    RVec3 v;
    RVec3 w;
    const f32* d = (const f32*)(p + 0x3C);
    u32 color = lbl_8055E598.value;
    o = *(PVec3*)&((CFrame*)PTR(p, 0xC))->t;
    for (int i = 0; i < S32(p, 0xBC); d += 4, i++) {
        f32 len = d[3];
        v.x = d[0] * len;
        v.y = d[1] * len;
        v.z = d[2] * len;
        CFrame* f = (CFrame*)PTR(p, 0xC);
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
u8* fn_801A3DAC(u8* p, u32 id, const char* name, u32 owner) {
    InitVolume(p, id, owner);
    U32(p, 0x8) = 0;
    S32(p, 0x18) = 'PMSH';
    F32(p, 0x10) = 1.0f;
    F32(p, 0x14) = 1.0f;
    S32(p, 0x20) = 100;
    U32(p, 0x1C) = 0;
    *(void**)p = lbl_80333728;
    MeshRes* m = (MeshRes*)fn_801CFEAC(name);
    PTR(p, 0x24) = (u8*)m;
    RVec3* mn = (RVec3*)(p + 0x28);
    RVec3* mx = (RVec3*)(p + 0x34);
    *mx = m->verts[0];
    *mn = *mx;
    for (int i = 1; i < ((MeshRes*)PTR(p, 0x24))->count; i++) {
        BoxCorner lo = VMin(*mn, ((MeshRes*)PTR(p, 0x24))->verts[i]);
        *mn = *(RVec3*)&lo;
        BoxCorner hi = VMax(*mx, ((MeshRes*)PTR(p, 0x24))->verts[i]);
        *mx = *(RVec3*)&hi;
    }
    return p;
}

u8* fn_801A3F5C(u8* p, u32 a, u32 b) {
    fn_8019ECC4(p, a, b);
    *(void**)p = lbl_80333728;
    if (b) {
        PTR(p, 0x24) = p + 0x40;
        fn_801CFA48(p + 0x40);
    }
    return p;
}

u8* fn_801A3FB8(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_80333728;
        fn_801CFF14(PTR(p, 0x24));
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

void fn_801A401C(u8* p, u8* o) {
    if (S32(o, 0x18) == 'GRUP') {
        ((VGroup*)o)->v10(p);
    } else if (S32(o, 0x18) == 'SPHR') {
        RVec3 hits[64];
        int n = 64;
        u8* sph = o;
        f32 r = F32(sph, 0x24);
        fn_801CF080(PTR(p, 0x24), (RVec3*)(sph + 0x34), hits, &n, U32(p, 0xC), lbl_8056234C * r);
        for (int i = 0; i < n; i++) {
            fn_8019AA18(sph, (u32)&hits[i], 0x6E, (u32)p);
        }
    }
}

// Ray from + t*dir against the mesh volume, tested in the volume's frame; the hit normal is turned
// back into world space.
int fn_801A40F8(u8* p, const RVec3* from, const RVec3* dir, f32* t, RVec3* n) {
    RVec3 lf;
    RVec3 ld;
    RVec3 ln;
    CFrame* f = (CFrame*)PTR(p, 0xC);
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
    MulVecT(&ld, dir, (CFrame*)PTR(p, 0xC));
    if (fn_801CF828(PTR(p, 0x24), (u32)&lf, (u32)&ld, (u32)t, (u32)&ln)) {
        MulVec(n, &ln, (CFrame*)PTR(p, 0xC));
        return 1;
    }
    return 0;
}

void fn_801A442C(const u8* p, RVec3* a, RVec3* b) {
    *a = *(const RVec3*)(p + 0x28);
    *b = *(const RVec3*)(p + 0x34);
}

f32 fn_801A4460(u8* p) {
    f32 a = PSVECDotProduct(p + 0x34, p + 0x34);
    f32 b = PSVECDotProduct(p + 0x28, p + 0x28);
    return (f32)sqrt(b > a ? b : a);
}

void fn_801A44C4(u8* p) { fn_801CFF18(PTR(p, 0x24), U32(p, 0xC)); }

void fn_801A44D4(u8* l) {
    for (int i = 0; i < S32(l, 0x0); i++) {
        fn_801A49FC(((u8**)PTR(l, 0x4))[i]);
    }
}

void fn_801A4538(void* a) { fn_8012C4E4(a, (void*)fn_801A44D4, 0); }

void* fn_8012C370(const char* name, u32 type, u32* info, s32* size, bool* first, bool* swapped);  // res.cpp
u8* fn_801A4B5C(u8* o, bool swap, u8* vol);
void fn_801A4768(s32* p, s32 base);
void fn_801A4780(s32* p, s32 base);
void fn_801A4798(s32* p, s32 base);
void fn_801A47B0(s32* p, s32 base);

static inline void Swap16(u8* b) {
    u8 t = b[0];
    b[0] = b[1];
    b[1] = t;
}

// Loads the static-object list resource ('SOBL'): on first load byte-swaps it, turns its offsets into
// pointers and sets up each object.
u8* fn_801A4548(const char* name) {
    u32 info;
    bool swapped = 0;
    u8* l = (u8*)fn_8012C370(name, 'SOBL', &info, 0, 0, &swapped);
    if (l && swapped) {
        Swap32(l + 0x0);
        Swap32(l + 0x8);
        Swap32(l + 0x4);
        Swap32(l + 0xC);
        Swap32(l + 0x10);
        fn_801A4768((s32*)(l + 0x4), (s32)l);
        fn_801A4780((s32*)(l + 0x10), (s32)l);
        fn_801A4798((s32*)(l + 0xC), (s32)l);
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
            if (o) {
                fn_801A4B5C(o, sw, o + 0x4C);
            }
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
    if (U8(p, 0x40) != v) {
        OnMsg m;
        m.id = U32(p, 0x38);
        m.on = v;
        fn_801796EC(0xC, &m, 8);
        U8(p, 0x40) = v;
    }
}

void fn_801A48C8(u8* p) { VE(PTR(p, 0x48))->v18("dyno", "unknown"); }

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
        fn_801D2414(p);
        if (PTR(p, 0x48)) {
            fn_801D0738(p, c, f, b);
        }
    }
}

u8* fn_801A4A8C(u8* p, u32 a, u32 b, u8* mesh, const Pose12s* pose, u32 c) {
    U32(p, 0x38) = a;
    *(void**)p = lbl_803336F8;
    U32(p, 0x48) = c;
    U32(p, 0x34) = b;
    PTR(p, 0x3C) = mesh;
    *(Pose12s*)(p + 0x4) = *pose;
    ((VMesh*)mesh)->v2C(p);
    fn_801A48E4(p);
    return p;
}

// Static object from file data (byte-swapped when `swap`); its collision volume is built in place at
// vol according to the volume's tag.
u8* fn_801A4B5C(u8* o, bool swap, u8* vol) {
    *(void**)o = lbl_803336F8;
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
        if (vol) {
            vol = fn_801A1CE4(vol, (u32)o, swap);
        }
        PTR(o, 0x3C) = vol;
        break;
    case 'SPHR':
        if (vol) {
            vol = fn_8019EDB8(vol, (u32)o, swap);
        }
        PTR(o, 0x3C) = vol;
        break;
    case 'TUBE':
        if (vol) {
            vol = fn_801A1898(vol, (u32)o, swap);
        }
        PTR(o, 0x3C) = vol;
        break;
    case 'RING':
        if (vol) {
            vol = fn_801A2CC0(vol, (u32)o, swap);
        }
        PTR(o, 0x3C) = vol;
        break;
    case 'PMSH':
        if (vol) {
            vol = fn_801A3F5C(vol, (u32)o, swap);
        }
        PTR(o, 0x3C) = vol;
        break;
    default:
        PTR(o, 0x3C) = 0;
        break;
    }
    fn_801A48E4(o);
    return o;
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

u8* fn_801A4F00(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

u8* fn_801A4F40(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

u8* fn_801A4F80(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

u8* fn_801A4FC0(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

u8* fn_801A5000(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

u8* fn_801A5040(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

u8* fn_801A5080(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

u8* fn_801A50C0(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

f32 fn_801A5100(u8* p) { return F32(p, 0x24); }

void fn_801A5108(u8* p) { S32(p, 0x180) = -1; }

f32 fn_801A5114(u8* p) { return F32(p, 0x30); }

f32 fn_801A511C(u8* p) { return F32(p, 0x24); }

f32 fn_801A5124(u8* p) { return F32(p, 0x28); }

u32 fn_801A512C(u8* p) { return 0; }

u8* fn_801A5134(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

u32 fn_801A5174(u8* p) { return U32(p, 0x48); }

u8* fn_801A517C(u8* p) { return p + 0x28; }

u8 fn_801A5184(u8* p) { return U8(p, 0x40); }

void fn_801A518C(u8* p, u32 v) { fn_801A4868(p, v); }

void fn_801A5190(u8* p) {
    u8* o = PTR(p, 0x3C);
    if (o) {
        VE(o)->v40();
    }
}

s32 fn_801A51B0(u8* p) {
    u8* o = PTR(p, 0x3C);
    if (o) {
        return S32(o, 0x20);
    }
    return -1;
}

u32 fn_801A51CC(u8* p) { return U32(p, 0x34); }

void fn_801A51D4(u8* p, u32 v) { U32(p, 0x34) = v; }



}  // extern "C"
