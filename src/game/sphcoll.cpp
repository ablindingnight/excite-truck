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

struct W3 {
    u32 a, b, c;
};

struct Phob;

class RigidBody {
public:
    void Push(Phob* who, void* a, const RVec3* b, const RVec3* c, int d, const RVec3* e, void* f, f32 g);
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

class VEx {
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
    virtual void v34(u8* q);
    virtual void v38(void);
    virtual void v3C(void);
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
void fn_8019A820(u8* p, u8* c, u32 a, u32 b, u32 d, f32 r);
int fn_8019AA34(u8* p, u8* c, u8* q, u8* d, u32 a, u32 b, f32 r, f32 s);
int fn_8019CCCC(u8* p, u8* c, u8* q, f32 r);
void fn_8019AC44(u8* p, u8* c, u8* d, f32 r);
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

void fn_80198F64(u8* p) {}  // stub (470)

void fn_801996BC(u8* p) {}  // stub (109)

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

void fn_8019A820(u8* p, u8* c, u32 a, u32 b, u32 d, f32 r) {}

void fn_8019AA18(u8* p, u32 a, u32 b, u32 c) { fn_8019A820(p, p + 0x34, a, b, c, F32(p, 0x24)); }

int fn_8019AA34(u8* p, u8* c, u8* q, u8* d, u32 a, u32 b, f32 r, f32 s) { return 0; }  // stub (528)

void fn_8019AC44(u8* p, u8* c, u8* d, f32 r) {}

int fn_8019B068(u8* p, u8* q, u32 a, u32 b) {
    f32 s = F32(q, 0x24);
    return fn_8019AA34(p, p + 0x34, q, q + 0x34, a, b, F32(p, 0x24), s);
}

int fn_8019B08C(u8* p, f32* c, u8* q, f32 r) { return 0; }  // stub (275)

void fn_8019B4D8(u8* p) {}  // stub (541)

u32 fn_8019BD4C(u8* p) { return U32(p, 0xc); }

void fn_8019BD54(u8* p, u8* q, int a, int b) {}  // stub (704)

void fn_8019C854(u8* p, u8* q) {}  // stub (286)

// Capsule (q: frame at 0xC with axis at 0x18 and centre at 0x24, half length 0x28, radius 0x24):
// the cylinder test, then a sphere test at each end.
int fn_8019CCCC(u8* p, u8* c, u8* q, f32 r) {
    if (fn_8019B08C(p, (f32*)c, q, r)) {
        return 1;
    }
    u8* m = PTR(q, 0xC);
    f32 h = F32(q, 0x28);
    PVec3 a;
    PVec3 b;
    PVec3 axis = *(PVec3*)(m + 0x18);
    PVec3 org = *(PVec3*)(m + 0x24);
    f32 rad = F32(q, 0x24);
    MulAdd(a.v, org.v, axis.v, h);
    MulAdd(b.v, org.v, axis.v, -h);
    bool hit = false;
    hit |= fn_8019AA34(p, c, q, (u8*)a.v, 0, 0, r, rad);
    hit |= fn_8019AA34(p, c, q, (u8*)b.v, 0, 0, r, rad);
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

int fn_8019D618(u8* p, u8* q, int a, int b) { return 0; }  // stub (294)

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

void fn_8019DC04(u8* p, u8* q) {}  // stub (122)

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

void fn_8019DF88(u8* p) {}  // stub (155)

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

int fn_8019E2D0(u8* a, u8* b, u8* c, u8* m, f32 x, f32 y, f32 z) { return 0; }  // stub (140)

void fn_8019E500(u8* p) {}  // stub (205)

u8* fn_8019E834(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_8019E874(u8* p) {}  // stub (130)

void fn_8019EA7C(u8* p, u8* q) {}  // stub (38)

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

void fn_8019ECC4(u8* p, u32 a, u32 b) {}

extern void* lbl_80333B48[];

// Sphere volume constructor from file data (byte-swapped when `swap`).
u8* fn_8019EDB8(u8* p, u32 a, u32 swap) {
    fn_8019ECC4(p, a, swap);
    *(void**)p = lbl_80333B48;
    if (swap) {
        Swap32(p + 0x24);
        Swap32(p + 0x28);
        Swap32(p + 0x2C);
        Swap32(p + 0x30);
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

// Rotation + translation of a collision frame.
struct CFrame {
    f32 m[9];   // offset 0x0, rows
    RVec3 t;    // offset 0x24
};

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

void fn_8019F1A4(u8* p) { fn_8019AC44(p, p + 0x34, p + 0x40, F32(p, 0x24)); }

void fn_8019F1B4(u8* p, u32 a, u32 b, u32 c, u32 d) { fn_8019DDFC((f32*)a, (f32*)b, (f32*)c, (f32*)d, (f32*)(p + 0x34), F32(p, 0x24)); }

void fn_8019F1D4(u8* p, RVec3* lo, RVec3* hi) {
    f32 r = F32(p, 0x24);
    RVec3 e;
    e.x = r;
    e.y = r;
    e.z = r;
    RVec3 c;
    c.x = F32(p, 0x34);
    c.y = F32(p, 0x38);
    c.z = F32(p, 0x3C);
    lo->x = c.x + r * -1.0f;
    lo->y = c.y + r * -1.0f;
    lo->z = c.z + r * -1.0f;
    hi->x = c.x + r * 1.0f;
    hi->y = c.y + r * 1.0f;
    hi->z = c.z + r * 1.0f;
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
    u8* m = PTR(p, 0x54);
    u8* o = PTR(p, 0x4);
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

void fn_8019F4D4(u8* p) {}  // stub (35)

void fn_8019F560(u8* p) { fn_8019F1A4(p); }

static inline void InitVolume(u8* s, u32 id, u32 owner) {
    U32(s, 0xC) = id;
    U32(s, 0x4) = owner;
}

extern void* lbl_80333A58[];

// Box volume (half sizes hx, hy, hz): a sphere bounding the largest half size, then the box fields.
u8* fn_8019F564(u8* p, u32 frame, const RVec3* c, u32 owner, f32 k, f32 skin, f32 a, f32 hx, f32 hy, f32 hz) {
    f32 cx = c->x;
    f32 cy = c->y;
    f32 cz = c->z;
    f32 big = hy > hz ? hy : hz;
    big = hx > big ? hx : big;
    InitVolume(p, frame, owner);
    U32(p, 0x8) = 0;
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

void fn_801A01E0(u8* p, RVec3* lo, RVec3* hi) {
    f32 r = F32(p, 0x178);
    u8* o = PTR(p, 0xC);
    RVec3 e;
    e.x = r;
    e.y = r;
    e.z = r;
    RVec3 c;
    c.x = F32(o, 0x24);
    c.y = F32(o, 0x28);
    c.z = F32(o, 0x2C);
    lo->x = c.x + r * -1.0f;
    lo->y = c.y + r * -1.0f;
    lo->z = c.z + r * -1.0f;
    hi->x = c.x + r * 1.0f;
    hi->y = c.y + r * 1.0f;
    hi->z = c.z + r * 1.0f;
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

void fn_801A0F10(u8* p) {}  // stub (142)

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

void fn_801A15CC(u8* p) {}  // stub (123)

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

void fn_801A18D4(u8* p) {}  // stub (122)

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
    fn_8019AC44(p, (u8*)&a, (u8*)&zero, r);
    fn_8019AC44(p, (u8*)&b, (u8*)&zero, r);
}

void fn_801A1CA0(u8* p) {
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
    if (fn_8019E2D0(a, b, c, ((VEx*)p)->v50(), x, y, z)) {
        out->x = 0.0f;
        out->y = 1.0f;
        out->z = 0.0f;
        return 1;
    }
    return 0;
}

void fn_801A1F44(RVec3* out, u8* q) {
    u8* o = VE(q)->v50();
    *(W3*)out = *(W3*)(o + 0x24);
}

void fn_801A1F98(u8* p) {}  // stub (195)

void fn_801A22A4(u8* p) {
    Xform* src = (Xform*)((VEx*)p)->v50();
    Xform x;
    x.rot = src->rot;
    x.pos = src->pos;
    RVec3* c = (RVec3*)(p + 0x24);
    fn_801E44F4((u8*)&x, 0x50000000, c->x, c->y, c->z);
}

u8* fn_801A2354(u8* p, u32 a, const RVec3* c, u32 b, f32 s0, f32 s1, f32 hx, f32 hy, f32 hz) {
    f32 y = hy - 0.2f;
    f32 x = hx - 0.2f;
    f32 z = hz - 0.2f;
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
    F32(p, 0x10) = 1.0f;
    F32(p, 0x14) = 1.0f;
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

void fn_801A2684(u8* p, RVec3* lo, RVec3* hi) {
    f32 r = F32(p, 0x24);
    u8* o = PTR(p, 0xC);
    f32 k = 1.0f;
    RVec3 e;
    e.x = k;
    e.y = k;
    e.z = k;
    f32 a = k * -r;
    f32 b = k * r;
    lo->x = F32(o, 0x24) + a;
    lo->y = F32(o, 0x28) + a;
    lo->z = F32(o, 0x2C) + a;
    hi->x = F32(o, 0x24) + b;
    hi->y = F32(o, 0x28) + b;
    hi->z = F32(o, 0x2C) + b;
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

void fn_801A2ECC(u8* p) {}  // stub (170)

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

void fn_801A324C(u8* p) {}  // stub (161)

void fn_801A34D0(u8* p) {}  // stub (236)

f32 fn_801A3880(u8* p) {
    f32 a = PSVECDotProduct(p + 0x30, p + 0x30);
    f32 b = PSVECDotProduct(p + 0x24, p + 0x24);
    return (f32)sqrt(b > a ? b : a);
}

void fn_801A38E4(u8* p) {}  // stub (191)

void fn_801A3BE0(u8* p) {}  // stub (115)

void fn_801A3DAC(u8* p) {}  // stub (108)

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

void fn_801A40F8(u8* p) {}  // stub (205)

void fn_801A442C(u8* p, RVec3* a, RVec3* b) {
    *a = *(RVec3*)(p + 0x28);
    *b = *(RVec3*)(p + 0x34);
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

void fn_801A4548(u8* p) {}  // stub (136)

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

void fn_801A4B5C(u8* p) {}  // stub (179)

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
