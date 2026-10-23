// Collision volumes (0x80179D08-0x8017B0B0): a physics object's set of collision shapes with its world
// transform (and last frame's), plus the box volume ("box-volume" shapes). Some CollVolume virtuals live in
// the neighbouring physics object unit. Names are guesses apart from the strings.
#include "game/common.h"

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

struct CVec3 {
    f32 x;
    f32 y;
    f32 z;
};

struct CMtx {
    CVec3 rot[3];  // offset 0x0
    CVec3 pos;     // offset 0x24
};

class CollShape {
public:
    CollShape(const CMtx* m) {
        mtx = m;
        a4 = 0;
        a8 = 0;
        f18 = 0x424F5820;
        stiff = 50000.0f;
        skin = 0.2f;
        a20 = 100;
        a1C = 0;
    }
    virtual ~CollShape(void);
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
    virtual f32 Radius(void);  // 0x3C
    s32 a4;          // offset 0x4
    s32 a8;          // offset 0x8
    const CMtx* mtx;  // offset 0xC
    f32 stiff;       // offset 0x10
    f32 skin;        // offset 0x14
    u32 f18;         // offset 0x18
    s32 a1C;         // offset 0x1C
    s32 a20;         // offset 0x20
};

class BoxShape : public CollShape {
public:
    BoxShape(const CMtx* m, f32 x, f32 y, f32 z) : CollShape(m) {
        half.x = x;
        half.y = y;
        half.z = z;
        a34 = 0;
        radius = 0.2f + (f32)sqrt(x * x + y * y + z * z);
    }
    CVec3 half;  // offset 0x24
    f32 radius;  // offset 0x30
    s32 a34;     // offset 0x34
};

struct CollDesc {
    s32 type;     // offset 0x0
    u8 pad4[0x4];
    CMtx mtx;     // offset 0x8
    s32 shape;    // offset 0x38
    CVec3 half;   // offset 0x3C
};

class CollVolume {
public:
    CollVolume(void);
    virtual void Init(CollDesc* d);    // 0x8
    virtual ~CollVolume(void);         // 0xC
    virtual void Reset(void);          // 0x10
    virtual void vf14(void);           // 0x14
    virtual void vf18(void);           // 0x18
    virtual void vf1C(void);           // 0x1C
    virtual void vf20(void);           // 0x20
    virtual void vf24(void);           // 0x24
    virtual void vf28(void);           // 0x28
    virtual s32 vf2C(void);            // 0x2C
    virtual void vf30(f32 t, struct CollPacket* a, struct CollPacket* b, s32* na, s32* nb);  // 0x30
    virtual s32 vf34(struct CollPacket* p);  // 0x34
    virtual void vf38(u8* out);  // 0x38
    virtual void vf3C(void);           // 0x3C
    virtual int vf40(void);            // 0x40
    virtual int vf44(void);            // 0x44
    void Update(void);
    void AddShape(CollShape* s);
    void Xform(CVec3* out, const CVec3* in);
    s32 a4;              // offset 0x4
    s32 a8;              // offset 0x8
    CollDesc* desc;      // offset 0xC
    s32 type;            // offset 0x10
    CollShape* sub[4];   // offset 0x14
    s32 nAdded;          // offset 0x24
    s32 nSub;            // offset 0x28
    f32 radius;          // offset 0x2C
    CVec3 offset;        // offset 0x30
    CMtx mtx;            // offset 0x3C
    s16 hint[2];         // offset 0x6C
    CMtx world;          // offset 0x70
    CMtx prev;           // offset 0xA0
};

class BoxVolume : public CollVolume {
public:
    BoxVolume(void);
    virtual void Init(CollDesc* d);
    virtual ~BoxVolume(void);
    virtual void vf28(void);
};

ModuleTag lbl_8055E478;

extern f32 lbl_80558DC4[];  // FLT_EPSILON

struct CollPacket {
    u16 rot[3];  // offset 0x0
    u8 pad6[2];
    CVec3 pos;   // offset 0x8
};

#define CLAMP01(x) ((x) = MIN(1.0f, (x)), (x) = MAX((x), 0.0f))

static inline f32 Unlerp(f32 a, f32 b, f32 x) {
    f32 t = (x - a) / (b - a);
    CLAMP01(t);
    return t;
}

static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

static inline void Identity(CMtx* m) {
    m->rot[0].x = 1.0f;
    m->rot[0].y = 0.0f;
    m->rot[0].z = 0.0f;
    m->rot[1].x = 0.0f;
    m->rot[1].y = 1.0f;
    m->rot[1].z = 0.0f;
    m->rot[2].x = 0.0f;
    m->rot[2].y = 0.0f;
    m->rot[2].z = 1.0f;
    m->pos.x = 0.0f;
    m->pos.y = 0.0f;
    m->pos.z = 0.0f;
}

CollVolume::CollVolume(void) {
    desc = 0;
    Identity(&mtx);
    Update();
    fn_801E9DC4(hint);
}

extern "C" void fn_80179D8C(CollVolume* v, s32 a) { v->a8 = a; }

void CollVolume::Init(CollDesc* d) {
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
    Update();
    prev = world;
    fn_801E9DC4(hint);
}

extern "C" void fn_80179EE4(void) {}

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

static inline void SetV(CVec3* v, f32 x, f32 y, f32 z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

static inline void AxisAngle(CVec3* m, const CVec3* v) {
    f32 a = PSVECMag(v);
    if (a > 0.0001f) {
        f32 inv = 1.0f / a;
        f32 x = inv * v->x;
        f32 y = inv * v->y;
        f32 z = inv * v->z;
        f32 s = sin(a);
        f32 c = cos(a);
        f32 t = 1.0f - c;
        f32 tx = t * x;
        f32 ty = t * y;
        m[0].x = c + t * (x * x);
        m[0].y = y * tx + s * z;
        m[0].z = z * tx - s * y;
        m[1].x = y * tx - s * z;
        m[1].y = c + t * (y * y);
        m[1].z = z * ty + s * x;
        m[2].x = z * tx + s * y;
        m[2].y = z * ty - s * x;
        m[2].z = c + t * (z * z);
    } else {
        m[0].y = 0.0f;
        m[0].x = 1.0f;
        m[0].z = 0.0f;
        m[1].x = 0.0f;
        m[1].y = 1.0f;
        m[1].z = 0.0f;
        m[2].x = 0.0f;
        m[2].y = 0.0f;
        m[2].z = 1.0f;
    }
}

extern "C" void fn_8017AE94(CMtx* out, const CMtx* a, const CMtx* b, f32 t);

void CollVolume::vf30(f32 t, CollPacket* a, CollPacket* b, s32* na, s32* nb) {
    CMtx ma;
    CMtx mb;
    CVec3 va;
    CVec3 vb;
    fn_8017A008((u8*)&ma.pos, (u8*)&a->pos, 12);
    fn_8017A008((u8*)&mb.pos, (u8*)&b->pos, 12);
    SetV(&va, Deq(a->rot[0]), Deq(a->rot[1]), Deq(a->rot[2]));
    SetV(&vb, Deq(b->rot[0]), Deq(b->rot[1]), Deq(b->rot[2]));
    AxisAngle(ma.rot, &va);
    AxisAngle(mb.rot, &vb);
    fn_8017AE94(&mtx, &ma, &mb, t);
    Update();
    *nb = 0x14;
    *na = 0x14;
}

void CollVolume::vf1C(void) { Update(); }

void CollVolume::vf20(void) {}

static inline void MulVec(const CMtx* m, const CVec3* in, CVec3* out) {
    if (out == in) {
        f32 x = in->z * m->rot[2].x + (in->x * m->rot[0].x + in->y * m->rot[1].x);
        f32 y = in->z * m->rot[2].y + (in->x * m->rot[0].y + in->y * m->rot[1].y);
        f32 z = in->z * m->rot[2].z + (in->x * m->rot[0].z + in->y * m->rot[1].z);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = in->z * m->rot[2].x + (in->x * m->rot[0].x + in->y * m->rot[1].x);
        out->y = in->z * m->rot[2].y + (in->x * m->rot[0].y + in->y * m->rot[1].y);
        out->z = in->z * m->rot[2].z + (in->x * m->rot[0].z + in->y * m->rot[1].z);
    }
}

void CollVolume::Update(void) {
    prev = world;
    MulVec(&mtx, &offset, &world.pos);
    world.pos.x += mtx.pos.x;
    world.pos.y += mtx.pos.y;
    world.pos.z += mtx.pos.z;
    world.rot[0] = mtx.rot[0];
    world.rot[1] = mtx.rot[1];
    world.rot[2] = mtx.rot[2];
    if (vf44() && vf40()) {
        int i;
        for (i = 0; i < nSub; i++) {
            sub[i]->vf0C();
        }
    }
}

extern "C" void fn_8017A800(void) {}

s32 CollVolume::vf34(CollPacket* p) {
    f32(*m)[3] = (f32(*)[3]) & mtx;
    f32 v[3];
    f32 q[4];
    f32 tr;
    f32 th;
    f32 sn;
    p->pos = mtx.pos;
    tr = mtx.rot[0].x + mtx.rot[1].y + mtx.rot[2].z;
    if (tr > 0.0f) {
        f32 r = sqrt(1.0f + tr);
        f32 inv = 0.5f / r;
        q[3] = 0.5f * r;
        q[0] = inv * (m[1][2] - m[2][1]);
        q[1] = inv * (m[2][0] - m[0][2]);
        q[2] = inv * (m[0][1] - m[1][0]);
    } else {
        int i = 0;
        int k;
        int j;
        f32 r;
        f32 inv;
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
        q[i] = 0.5f * r;
        q[3] = inv * (m[j][k] - m[k][j]);
        q[j] = inv * (m[i][j] + m[j][i]);
        q[k] = inv * (m[i][k] + m[k][i]);
    }
    th = acos(q[3]);
    sn = sin(th);
    if ((f32)__fabs(sn) > lbl_80558DC4[0]) {
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

void CollVolume::Xform(CVec3* out, const CVec3* in) {
    MulVec(&mtx, in, out);
    out->x += mtx.pos.x;
    out->y += mtx.pos.y;
    out->z += mtx.pos.z;
}

BoxVolume::BoxVolume(void) {}

void BoxVolume::Init(CollDesc* d) {
    CollVolume::Init(d);
    mtx = d->mtx;
    Update();
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

extern "C" void fn_8017AE94(CMtx* out, const CMtx* a, const CMtx* b, f32 t) {
    int i;
    f32* o = &out->rot[0].x;
    const f32* p = &a->rot[0].x;
    const f32* q = &b->rot[0].x;
    CVec3 x;
    CVec3 y;
    CVec3 z;
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
    PSVECNormalize(&out->rot[0], &out->rot[0]);
    PSVECNormalize(&out->rot[1], &out->rot[1]);
    PSVECNormalize(&out->rot[2], &out->rot[2]);
    x = out->rot[0];
    z = out->rot[2];
    PSVECCrossProduct(&z, &x, &y);
    PSVECNormalize(&x, &x);
    PSVECNormalize(&y, &y);
    PSVECCrossProduct(&x, &y, &z);
    out->rot[0] = x;
    out->rot[1] = y;
    out->rot[2] = z;
}

BoxVolume::~BoxVolume(void) {}

void BoxVolume::vf28(void) {}

s32 CollVolume::vf2C(void) { return 0x14; }

void CollVolume::vf38(u8* out) { *out = 0; }
