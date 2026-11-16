// Collision volumes (0x80179D08-0x8017B0B0): a physics object's set of collision shapes with its world
// transform (and last frame's), plus the box volume ("box-volume" shapes). Some CollVolume virtuals live in
// the neighbouring physics object unit. Names are guesses apart from the strings.
#include "game/common.h"
#include "game/collshape.h"
#include "game/rmtx.h"
#include "game/collvol.h"
#include "game/shapes.h"

extern "C" {
f32 PSVECMag(const void* v);
double cos(double x);
double sqrt(double x);
double acos(double x);
double sin(double x);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_801E9DC4(s16* h);
void PSVECNormalize(const void* src, void* dst);
void PSVECCrossProduct(const void* a, const void* b, void* out);
}

void* operator new(size_t size, const char* tag);

// The 3x3 rotation block: copied as one 36-byte struct (word copy), indexed like an array.
/* Type shape from the target (one 9-word copy in CollVolume::Update, 2026-11-08); name and the conversion operator are guesses. */
// The volume description (named as the RigidBody family names it; unproven).
struct BodyDesc {
    s32 type;     // offset 0x0
    u8 pad4[0x4];
    RMtx mtx;     // offset 0x8
    s32 shape;    // offset 0x38
    RVec3 half;   // offset 0x3C
};

struct RVec3;

struct RPacket;

class PhobStatic : public CollVolume {
public:
    PhobStatic(void);
    virtual void Init(BodyDesc* d);
    virtual ~PhobStatic(void);
    virtual void vf28(u8* buf);
};

ModuleTag lbl_8055E478;

extern f32 __float_epsilon[];  // FLT_EPSILON

struct RPacket {
    u16 rot[3];  // offset 0x0
    u8 pad6[2];
    RVec3 pos;   // offset 0x8
};

#define CLAMP01(x) ((x) = MIN(1.0f, (x)), (x) = MAX((x), 0.0f))

static inline f32 Unlerp(f32 a, f32 b, f32 x) {
    f32 t = MIN(1.0f, (x - a) / (b - a));
    t = MAX(t, 0.0f);
    return t;
}

static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

static inline void Identity(RMtx* m) {
    m->rot.x.x = 1.0f;
    m->rot.x.y = 0.0f;
    m->rot.x.z = 0.0f;
    m->rot.y.x = 0.0f;
    m->rot.y.y = 1.0f;
    m->rot.y.z = 0.0f;
    m->rot.z.x = 0.0f;
    m->rot.z.y = 0.0f;
    m->rot.z.z = 1.0f;
    m->pos.x = 0.0f;
    m->pos.y = 0.0f;
    m->pos.z = 0.0f;
}

CollVolume::CollVolume(void) {
    desc = 0;
    Identity(&mtx);
    Transform();
    fn_801E9DC4(hint);
}

extern "C" void fn_80179D8C(CollVolume* v, s32 a) { v->a8 = a; }

void CollVolume::Init(BodyDesc* d) {
    desc = d;
    offset.x = 0.0f;
    offset.y = 0.0f;
    offset.z = 0.0f;
    nAdded = 0;
    nSub = 0;
    radius = 0.0f;
    type = d->type;
    a4 = 0;
    Identity(&mtx);
    fn_801E9DC4(hint);
}

void CollVolume::Reset(void) {
    int i;
    a4 = 0;
    for (i = 0; i < nSub; i++) {
        sub[i]->vf28();
    }
    Transform();
    prev = world;
    fn_801E9DC4(hint);
}

// CollShape::vf28 (inline, game/collshape.h): retail keeps its weak copy here (0x80179EE4), this unit being the
// first linked one whose vtable needs it.

CollVolume::~CollVolume(void) {
    int i;
    for (i = 0; i < nSub; i++) {
        delete sub[i];
    }
}

void CollVolume::AddShape(CollShape* s) {
    int n = nAdded;
    sub[n] = s;
    nSub = nAdded = n + 1;
    {
        f32 r = s->Radius();
        radius = radius > r ? radius : r;
    }
}

void CollVolume::vf18(void) {}

extern "C" void fn_8017A008(u8* dst, const u8* src, int n) {
    while (--n >= 0) {
        *dst++ = *src++;
    }
}

static inline f32 Frac(f32 x, f32 lo, f32 hi) { return (x - lo) / (hi - lo); }

static inline f32 Deq(u16 u) {
    f32 t = MIN(1.0f, Frac(u, 0.0f, 65535.0f));
    t = MAX(t, 0.0f);
    return Lerp(-6.2831855f, 6.2831855f, t);
}

static inline void SetV(RVec3* v, f32 x, f32 y, f32 z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

static inline void AxisAngle(RVec3* m, const RVec3* v) {
    f32 a = PSVECMag(v);
    if (a > 0.0001f) {
        f32 inv = 1.0f / a;
        f32 z;
        f32 y;
        f32 s;
        f32 x;
        f32 c;
        x = inv * v->x;
        y = inv * v->y;
        z = inv * v->z;
        s = sin(a);
        c = cos(a);
        f32 t = 1.0f - c;
        f32 tx = t * x;
        f32 ty = t * y;
        m[0].x = c + t * (x * x);
        m[0].z = z * tx - s * y;
        m[0].y = y * tx + s * z;
        m[1].y = c + t * (y * y);
        m[1].x = y * tx - s * z;
        m[2].x = z * tx + s * y;
        m[1].z = z * ty + s * x;
        m[2].z = c + t * (z * z);
        m[2].y = z * ty - s * x;
    } else {
        m[0].x = 1.0f;
        m[0].y = 0.0f;
        m[0].z = 0.0f;
        m[1].x = 0.0f;
        m[1].y = 1.0f;
        m[1].z = 0.0f;
        m[2].x = 0.0f;
        m[2].y = 0.0f;
        m[2].z = 1.0f;
    }
}

extern "C" void fn_8017AE94(RMtx* out, const RMtx* a, const RMtx* b, f32 t);

void CollVolume::vf30(f32 t, RPacket* a, const RPacket* b, s32* na, s32* nb) {
    RMtx ma;
    RMtx mb;
    RVec3 va;
    RVec3 vb;
    fn_8017A008((u8*)&ma.pos, (u8*)&a->pos, 12);
    fn_8017A008((u8*)&mb.pos, (u8*)&b->pos, 12);
    SetV(&va, Deq(a->rot[0]), Deq(a->rot[1]), Deq(a->rot[2]));
    SetV(&vb, Deq(b->rot[0]), Deq(b->rot[1]), Deq(b->rot[2]));
    AxisAngle(&ma.rot.x, &va);
    AxisAngle(&mb.rot.x, &vb);
    fn_8017AE94(&mtx, &ma, &mb, t);
    Transform();
    *nb = 0x14;
    *na = 0x14;
}

void CollVolume::Update(void) { Transform(); }

void CollVolume::vf20(void) {}

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

static inline void MulVec(const RMtx* m, const RVec3* in, RVec3* out) {
    if (out == in) {
        f32 x = Dot3(in->x, m->rot.x.x, in->y, m->rot.y.x, in->z, m->rot.z.x);
        f32 y = Dot3(in->x, m->rot.x.y, in->y, m->rot.y.y, in->z, m->rot.z.y);
        f32 z = Dot3(in->x, m->rot.x.z, in->y, m->rot.y.z, in->z, m->rot.z.z);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, m->rot.x.x, in->y, m->rot.y.x, in->z, m->rot.z.x);
        out->y = Dot3(in->x, m->rot.x.y, in->y, m->rot.y.y, in->z, m->rot.z.y);
        out->z = Dot3(in->x, m->rot.x.z, in->y, m->rot.y.z, in->z, m->rot.z.z);
    }
}

void CollVolume::Transform(void) {
    prev = world;
    MulVec(&mtx, &offset, &world.pos);
    world.pos.x += mtx.pos.x;
    world.pos.y += mtx.pos.y;
    world.pos.z += mtx.pos.z;
    world.rot = mtx.rot;
    if (vf44() && vf40()) {
        int i;
        for (i = 0; i < nSub; i++) {
            sub[i]->vf0C();
        }
    }
}

// CollShape::vf0C (inline, game/collshape.h): retail keeps its weak copy here (0x8017A800).

s32 CollVolume::vf34(RPacket* p) {
    f32 v[3];
    f32 q[4];
    f32* qp;
    f32(*m)[3] = (f32(*)[3]) & mtx;
    f32 tr;
    f32 th;
    f32 sn;
    p->pos = mtx.pos;
    tr = mtx.rot.x.x + mtx.rot.y.y + mtx.rot.z.z;
    if (tr > 0.0f) {
        f32 r = sqrt(1.0f + tr);
        f32 inv = 0.5f / r;
        q[3] = 0.5f * r;
        q[0] = inv * (m[1][2] - m[2][1]);
        q[1] = inv * (m[2][0] - m[0][2]);
        q[2] = inv * (m[0][1] - m[1][0]);
    } else {
        qp = q;
        int i;
        int k;
        int j;
        f32 r;
        f32 inv;
        i = 0;
        if (m[1][1] > m[0][0]) {
            i = 1;
        }
        if (m[2][2] > m[i][i]) {
            i = 2;
        }
        j = (i + 1) % 3;
        k = (j + 1) % 3;
        r = sqrt(1.0f + (m[i][i] - (m[j][j] + m[k][k])));
        inv = 0.5f / r;
        qp[i] = 0.5f * r;
        qp[3] = inv * (m[j][k] - m[k][j]);
        qp[j] = inv * (m[i][j] + m[j][i]);
        qp[k] = inv * (m[i][k] + m[k][i]);
    }
    th = acos(q[3]);
    sn = sin(th);
    if ((f32)__fabs(sn) > __float_epsilon[0]) {
        f32 f = 2.0f * th / sn;
        v[0] = q[0] * f;
        v[1] = q[1] * f;
        v[2] = q[2] * f;
    } else {
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = 0.0f;
    }
    p->rot[0] = 0.5f + Lerp(0.0f, 65535.0f, Unlerp(-6.2831855f, 6.2831855f, v[0]));
    p->rot[1] = 0.5f + Lerp(0.0f, 65535.0f, Unlerp(-6.2831855f, 6.2831855f, v[1]));
    p->rot[2] = 0.5f + Lerp(0.0f, 65535.0f, Unlerp(-6.2831855f, 6.2831855f, v[2]));
    return 0x14;
}

void CollVolume::Xform(RVec3* out, const RVec3* in) {
    MulVec(&mtx, in, out);
    out->x += mtx.pos.x;
    out->y += mtx.pos.y;
    out->z += mtx.pos.z;
}

PhobStatic::PhobStatic(void) {}

void PhobStatic::Init(BodyDesc* d) {
    CollVolume::Init(d);
    mtx = d->mtx;
    Transform();
    switch (d->shape) {
    case 0: {
        BoxShape* s = new ("box-volume") BoxShape(&mtx, d->half.x, d->half.y, d->half.z);
        AddShape(s);
        break;
    }
    default:
        fn_802E30C8("Collision volume:  Not supported");
        break;
    }
}

extern "C" void fn_8017AE94(RMtx* out, const RMtx* a, const RMtx* b, f32 t) {
    int i;
    f32* o = &out->rot.x.x;
    const f32* p = &a->rot.x.x;
    const f32* q = &b->rot.x.x;
    RVec3 z;
    RVec3 y;
    RVec3 x;
    for (i = 0; i < 2; i++) {
        o[0] = p[0] + t * (q[0] - p[0]);
        o[1] = p[1] + t * (q[1] - p[1]);
        o[2] = p[2] + t * (q[2] - p[2]);
        o[3] = p[3] + t * (q[3] - p[3]);
        o[4] = p[4] + t * (q[4] - p[4]);
        o[5] = p[5] + t * (q[5] - p[5]);
        o += 6;
        p += 6;
        q += 6;
    }
    PSVECNormalize(&out->rot.x, &out->rot.x);
    PSVECNormalize(&out->rot.y, &out->rot.y);
    PSVECNormalize(&out->rot.z, &out->rot.z);
    x.Set(out->rot.x.x, out->rot.x.y, out->rot.x.z);
    z.Set(out->rot.z.x, out->rot.z.y, out->rot.z.z);
    PSVECCrossProduct(&z, &x, &y);
    PSVECNormalize(&x, &x);
    PSVECNormalize(&y, &y);
    PSVECCrossProduct(&x, &y, &z);
    out->rot.x = x;
    out->rot.y = y;
    out->rot.z = z;
}

PhobStatic::~PhobStatic(void) {}

void PhobStatic::vf28(u8* buf) {}

s32 CollVolume::vf2C(RPacket* p) { return 0x14; }

void CollVolume::vf38(u8* out) { *out = 0; }
