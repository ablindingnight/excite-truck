// Panel volumes (0x801BF210-0x801C109C): loose body panels ("panelvol"). A pool of rigid bodies, each a
// quad that a car sheds (Attach): the quad is built from a box on the car, hangs off a cloth/strip
// simulation (fn_801C1BA4 handle) and is re-fitted to the simulated corners every update. A body
// is reused when free, else the oldest one is taken. Names are guesses apart from the strings.
#include "game/common.h"

struct RVec3 {
    f32 x;
    f32 y;
    f32 z;
};

struct RMtx {
    RVec3 rot[3];  // offset 0x0
    RVec3 pos;     // offset 0x24
};

struct Mat3 {
    f32 m[9];
};

struct RPacket;
class CollShape;

struct BodyDesc {
    s32 kind;  // offset 0x0
    s32 size;  // offset 0x4
    f32 mass;  // offset 0x8
    f32 ix;    // offset 0xC
    f32 iy;    // offset 0x10
    f32 iz;    // offset 0x14
};

class Phob {
public:
    const RMtx* World(void);
    const RVec3* Vel(void);
};

class CollVolume {
public:
    virtual void Init(BodyDesc* d);   // 0x8
    virtual ~CollVolume(void);        // 0xC
    virtual void Reset(void);         // 0x10
    virtual void vf14(void);          // 0x14
    virtual void vf18(void);          // 0x18
    virtual void vf1C(void);          // 0x1C
    virtual void vf20(void);          // 0x20
    virtual void vf24(void);          // 0x24
    virtual void vf28(u8* buf);       // 0x28
    virtual s32 vf2C(RPacket* p);     // 0x2C
    virtual void vf30(f32 t, RPacket* a, RPacket* b, s32* na, s32* nb);  // 0x30
    virtual s32 vf34(RPacket* p);     // 0x34
    virtual void vf38(u8* p);         // 0x38
    virtual void vf3C(void);          // 0x3C
    virtual int vf40(void);           // 0x40
    virtual int vf44(void);           // 0x44
    void Update(void);
    void AddShape(CollShape* s);
    s32 a4;             // offset 0x4
    s32 a8;             // offset 0x8
    void* desc;         // offset 0xC
    u32 type;           // offset 0x10
    CollShape* sub[4];  // offset 0x14
    s32 nAdded;         // offset 0x24
    s32 nSub;           // offset 0x28
    f32 radius;         // offset 0x2C
    RVec3 offset;       // offset 0x30
    RMtx mtx;           // offset 0x3C
    s16 hint[2];        // offset 0x6C
    RMtx world;         // offset 0x70
    RMtx prev;          // offset 0xA0
};

class RigidBody : public CollVolume {
public:
    RigidBody(void);
    ~RigidBody(void) {}
    virtual void Init(BodyDesc* d);
    virtual void Reset(void);
    virtual void OnHit(const RVec3* f, const RVec3* at, int kind, void* c);  // 0x48
    virtual void OnImpulse(const RVec3* v, const RVec3* at, int kind, void* c, void* by);  // 0x4C
    virtual void OnImpulses(RVec3* f, RVec3* t);  // 0x50
    void Flush(RVec3* f, RVec3* t);
    u8 padD0[0x118 - 0xD0];
    RVec3 vel;   // offset 0x118
    RVec3 avel;  // offset 0x124
    u8 pad130[0x1E8 - 0x130];
    u8 b1E8;  // offset 0x1E8
};

// The render state, written at the body's offset in the frame buffer.
struct PanelPose {
    RMtx mtx;   // offset 0x0
    s16 hint;   // offset 0x30
    s32 model;  // offset 0x34
    s32 part;   // offset 0x38
    s32 car;    // offset 0x3C
};

class PanelVol : public RigidBody {
public:
    PanelVol(void);
    virtual void Init(BodyDesc* d);
    virtual ~PanelVol(void);
    virtual void Reset(void);
    virtual void vf1C(void);
    virtual void vf28(u8* buf);
    virtual int vf44(void);
    Phob* owner;   // offset 0x1EC
    s32 model;     // offset 0x1F0
    s32 part;      // offset 0x1F4
    s32 car;       // offset 0x1F8
    f32 f1FC;      // offset 0x1FC
    f32 f200;      // offset 0x200
    f32 f204;      // offset 0x204
    f32 f208;      // offset 0x208
    f32 f20C;      // offset 0x20C
    f32 f210;      // offset 0x210
    void* strip;   // offset 0x214
    f32 lastUsed;  // offset 0x218
    s32 org;       // offset 0x21C: corner indices: origin, then two edges
    s32 e0a;       // offset 0x220
    s32 e0b;       // offset 0x224
    s32 e1a;       // offset 0x228
    s32 e1b;       // offset 0x22C
    RMtx rest;     // offset 0x230: the body relative to the corner frame
    RVec3 pos;     // offset 0x260
    RVec3 dir;     // offset 0x26C
};

// The fields of a sphere collision shape this unit touches.
struct SphereView {
    void* vtbl;          // offset 0x0
    CollVolume* owner;   // offset 0x4
    u8 pad8[0x28 - 0x8];
    RVec3 center;        // offset 0x28
};

struct CVec {
    CVec(void) {}
    CVec(f32 a, f32 b, f32 c) {
        x = a;
        y = b;
        z = c;
    }
    f32 x;
    f32 y;
    f32 z;
};

struct PanelVert {
    PanelVert(void);
    RVec3 p;  // offset 0x0
    f32 u;    // offset 0xC
    f32 v;    // offset 0x10
};

struct PanelEdge {
    s32 a;
    s32 b;
    s32 c;
    s32 d;
};

struct PanelLink {
    s32 id;
    const RMtx* mtx;
    s32 c;
};

struct Vec3C {
    Vec3C(void);
    RVec3 v;
};

ModuleTag lbl_8055E7B0;
UnkStaticFFFF lbl_8055E7B8;
s32 lbl_8055E7BC;  // bodies in the pool
PanelVol* lbl_80437D48[16];

extern f32 lbl_8055E3C8;    // time
extern f32 lbl_8055E460;    // tick alpha
extern f32 lbl_80558DC4[];  // FLT_EPSILON

static const PanelEdge lbl_802F98D0[6] = {
    {0, 1, 0, 0}, {1, 3, 0, 0}, {3, 2, 0, 0}, {2, 0, 0, 0}, {0, 3, 0, 0}, {1, 2, 0, 0},
};

void Mat3Mul(Mat3* out, const Mat3* a, const Mat3* b);
void* operator new(size_t size, const char* name);
void operator delete(void* p);

extern "C" {
f32 fn_80144514(f32 a, f32 b, f32 c, f32 d, f32 e, f32 f);
void fn_80144B44(RVec3* v, f32 x, f32 y, f32 z);
void fn_80144B54(RMtx* dst, const RMtx* src);
f32 fn_80144CC8(f32 x);
void fn_80144DAC(RVec3* dst, const RVec3* src);
void fn_80144DC8(RMtx* m);
const RMtx* fn_801602B4(Phob* car);
void fn_8017E388(RMtx* m);
CollShape* fn_801A25F8(void* mem, const RMtx* m, void* strip, CollVolume* o, f32 r);
void fn_8019EECC(CollVolume* v, RVec3* c);
void* fn_801C1BA4(int verts, int edges, int links);
void fn_801C1C7C(void* strip);
void fn_801C1CBC(void* strip, int nv, PanelVert* v, int ne, const PanelEdge* e, int nl, PanelLink* l);
void fn_801C256C(void* strip, const RVec3* vel, const RVec3* pos, const RVec3* avel);
void fn_801C2694(void* strip, RVec3* out, s32* n, const RVec3* pos, const RVec3* dir, RVec3* f, RVec3* t);
void fn_801E9DC4(s16* h);
int fn_801E9E78(s16* h, RVec3* p, RVec3* n, s32* id);
void* memset(void* dst, int c, size_t n);
f32 PSVECDotProduct(const void* a, const void* b);
void PSVECCrossProduct(const void* a, const void* b, void* out);
void PSVECNormalize(const void* in, void* out);
void StatsUpdate(void);

void fn_801BFE7C(RVec3* out, const RVec3* a, const RVec3* b);
void fn_801BFEB0(PanelVert* v);
s16* fn_801BFEB4(CollVolume* v);
void fn_801BFEBC(RVec3* v, f32 x, f32 y, f32 z);
void fn_801BFECC(CollVolume* v, const RMtx* m);
void fn_801BFF64(RMtx* m, const RVec3* v);
void fn_801C00B8(CollShape* s, const RVec3* c);
u32 fn_801C00F0(CollShape* s);
CollShape* fn_801C00F8(CollVolume* v, int i);
void fn_801C0124(RVec3* out, const RVec3* v, f32 s);
void fn_801C014C(CollVolume* v, const RVec3* off);
s32 fn_801C0168(Phob* car);
s32 fn_801C0170(Phob* car);
f32 fn_801C0178(void);
void fn_801BF75C(RMtx* out, const RMtx* a, const RMtx* b);
void fn_801BFB2C(PanelVol* p, RMtx* out, const RVec3* pts);
void fn_801BFD34(RVec3* out, const RVec3* v, const RMtx* m);
const RVec3* fn_801BF754(Phob* car);
void fn_801C092C(RMtx* out, const RMtx* a, const RMtx* b);
void fn_801C0D3C(Mat3* m);
void fn_801C0D6C(Mat3* m);
f32 fn_801C0E44(const RVec3* v);
void fn_801C0E4C(RVec3* out, const RVec3* a, const RVec3* b);
void fn_801C0E60(RVec3* out, const RVec3* a, const RVec3* b);
}

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

static inline void MulVec(RVec3* out, const RVec3* in, const RVec3* m) {
    if (out == in) {
        f32 x = Dot3(in->x, m[0].x, in->y, m[1].x, in->z, m[2].x);
        f32 y = Dot3(in->x, m[0].y, in->y, m[1].y, in->z, m[2].y);
        f32 z = Dot3(in->x, m[0].z, in->y, m[1].z, in->z, m[2].z);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, m[0].x, in->y, m[1].x, in->z, m[2].x);
        out->y = Dot3(in->x, m[0].y, in->y, m[1].y, in->z, m[2].y);
        out->z = Dot3(in->x, m[0].z, in->y, m[1].z, in->z, m[2].z);
    }
}

static inline void MulM(Mat3* out, const Mat3* a, const Mat3* b) {
    if (out == a || out == b) {
        f32 m0 = fn_80144514(b->m[0], a->m[0], b->m[3], a->m[1], b->m[6], a->m[2]);
        f32 m3 = fn_80144514(b->m[0], a->m[3], b->m[3], a->m[4], b->m[6], a->m[5]);
        f32 m6 = fn_80144514(b->m[0], a->m[6], b->m[3], a->m[7], b->m[6], a->m[8]);
        f32 m1 = fn_80144514(b->m[1], a->m[0], b->m[4], a->m[1], b->m[7], a->m[2]);
        f32 m4 = fn_80144514(b->m[1], a->m[3], b->m[4], a->m[4], b->m[7], a->m[5]);
        f32 m7 = fn_80144514(b->m[1], a->m[6], b->m[4], a->m[7], b->m[7], a->m[8]);
        f32 m2 = fn_80144514(b->m[2], a->m[0], b->m[5], a->m[1], b->m[8], a->m[2]);
        f32 m5 = fn_80144514(b->m[2], a->m[3], b->m[5], a->m[4], b->m[8], a->m[5]);
        f32 m8 = fn_80144514(b->m[2], a->m[6], b->m[5], a->m[7], b->m[8], a->m[8]);
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
        out->m[0] = fn_80144514(b->m[0], a->m[0], b->m[3], a->m[1], b->m[6], a->m[2]);
        out->m[3] = fn_80144514(b->m[0], a->m[3], b->m[3], a->m[4], b->m[6], a->m[5]);
        out->m[6] = fn_80144514(b->m[0], a->m[6], b->m[3], a->m[7], b->m[6], a->m[8]);
        out->m[1] = fn_80144514(b->m[1], a->m[0], b->m[4], a->m[1], b->m[7], a->m[2]);
        out->m[4] = fn_80144514(b->m[1], a->m[3], b->m[4], a->m[4], b->m[7], a->m[5]);
        out->m[7] = fn_80144514(b->m[1], a->m[6], b->m[4], a->m[7], b->m[7], a->m[8]);
        out->m[2] = fn_80144514(b->m[2], a->m[0], b->m[5], a->m[1], b->m[8], a->m[2]);
        out->m[5] = fn_80144514(b->m[2], a->m[3], b->m[5], a->m[4], b->m[8], a->m[5]);
        out->m[8] = fn_80144514(b->m[2], a->m[6], b->m[5], a->m[7], b->m[8], a->m[8]);
    }
}

static inline void Sub(RVec3* out, const RVec3* a, const RVec3* b) {
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

static inline void Orthonormalize(Mat3* m) {
    CVec a(m->m[0], m->m[1], m->m[2]);
    CVec b;
    CVec c(m->m[6], m->m[7], m->m[8]);
    PSVECCrossProduct(&c, &a, &b);
    PSVECNormalize(&a, &a);
    PSVECNormalize(&b, &b);
    PSVECCrossProduct(&a, &b, &c);
    m->m[0] = a.x;
    m->m[1] = a.y;
    m->m[2] = a.z;
    m->m[3] = b.x;
    m->m[4] = b.y;
    m->m[5] = b.z;
    m->m[6] = c.x;
    m->m[7] = c.y;
    m->m[8] = c.z;
}

static inline void Identity(Mat3* m) {
    m->m[0] = 1.0f;
    m->m[1] = 0.0f;
    m->m[2] = 0.0f;
    m->m[3] = 0.0f;
    m->m[4] = 1.0f;
    m->m[5] = 0.0f;
    m->m[6] = 0.0f;
    m->m[7] = 0.0f;
    m->m[8] = 1.0f;
}

inline PanelVol::PanelVol(void) {
    owner = 0;
    model = -1;
    lbl_80437D48[lbl_8055E7BC++] = this;
    strip = fn_801C1BA4(4, 6, 2);
}

extern "C" {

void fn_801BF210(void) {}

PanelVol* fn_801BF214(void) { return new ("panel") PanelVol; }

int fn_801BF2A4(Phob* car, s32 part, const RVec3* off, const RVec3* lo, const RVec3* hi, int n, const s32* ids) {
    if ((f32)__fabs(lo->x - hi->x) <= lbl_80558DC4[0] && (f32)__fabs(lo->y - hi->y) <= lbl_80558DC4[0] &&
        (f32)__fabs(lo->z - hi->z) <= lbl_80558DC4[0]) {
        return 0;
    }
    PanelVol* p = 0;
    int i;
    for (i = 0; i < lbl_8055E7BC; i++) {
        if (lbl_80437D48[i]->owner == 0) {
            p = lbl_80437D48[i];
            break;
        }
    }
    if (!p) {
        p = lbl_80437D48[0];
        f32 best = p->lastUsed;
        for (i = 0; i < lbl_8055E7BC; i++) {
            if (lbl_80437D48[i]->lastUsed <= best) {
                best = lbl_80437D48[i]->lastUsed;
                p = lbl_80437D48[i];
            }
        }
    }
    if (!p) {
        return 0;
    }
    p->lastUsed = fn_801C0178();
    p->owner = car;
    p->model = fn_801C0170(car);
    p->part = part;
    p->car = fn_801C0168(p->owner);
    fn_801C014C(p, off);
    RVec3 neg;
    fn_801C0124(&neg, off, -1.0f);
    CollShape* s = fn_801C00F8(p, 0);
    if (s && fn_801C00F0(s) == 'SPHR') {
        fn_801C00B8(s, &neg);
    }
    RMtx m;
    fn_80144B54(&m, car->World());
    fn_801BFF64(&m, &neg);
    fn_801BFECC(p, &m);
    fn_801BFEBC(&p->vel, 0.0f, 0.0f, 0.0f);
    fn_801BFEBC(&p->avel, 0.0f, 0.0f, 0.0f);
    fn_801E9DC4(fn_801BFEB4(p));
    PanelVert verts[4];
    PanelEdge edges[6] = {
        {0, 1, 0, 0}, {1, 3, 0, 0}, {3, 2, 0, 0}, {2, 0, 0, 0}, {0, 3, 0, 0}, {1, 2, 0, 0},
    };
    RVec3 a;
    RVec3 b;
    fn_801BFE7C(&a, lo, off);
    fn_801BFE7C(&b, hi, off);
    p->org = 0;
    p->e0a = 0;
    p->e0b = 1;
    p->e1a = 0;
    p->e1b = 2;
    fn_8017E388(&p->rest);
    int xb = 1;
    int yb = 4;
    int zb = 2;
    if (b.y - a.y > b.x - a.x) {
        yb = 1;
        zb = 2;
        xb = 4;
    }
    for (i = 0; i < 4; i++) {
        RVec3 c;
        fn_80144B44(&c, (i & xb) ? a.x : b.x, (i & yb) ? a.y : b.y, (i & zb) ? a.z : b.z);
        fn_801BFD34(&verts[i].p, &c, &m);
        verts[i].u = 4.545f;
        verts[i].v = 0.127f;
    }
    RVec3 pts[4];
    for (i = 0; i < 4; i++) {
        fn_80144DAC(&pts[i], &verts[i].p);
    }
    RMtx frame;
    fn_80144DC8(&frame);
    fn_801BFB2C(p, &frame, pts);
    fn_801BF75C(&p->rest, &m, &frame);
    PanelLink links[2];
    for (i = 0; i < n; i++) {
        links[i].id = *ids++;
        links[i].mtx = fn_801602B4(car);
        links[i].c = 0;
    }
    fn_801C1CBC(p->strip, 4, verts, 6, edges, n, links);
    const RVec3* av = fn_801BF754(car);
    const RMtx* cm = fn_801602B4(car);
    fn_801C256C(p->strip, car->Vel(), &cm->pos, av);
    return 1;
}

const RVec3* fn_801BF754(Phob* car) { return (const RVec3*)((u8*)car + 0x124); }

// out = a relative to b: a's basis times b's transposed basis, and a's position in b's frame.
void fn_801BF75C(RMtx* out, const RMtx* a, const RMtx* b) {
    Mat3 t = *(const Mat3*)b->rot;
    f32 s;
    s = t.m[1];
    t.m[1] = t.m[3];
    t.m[3] = s;
    s = t.m[2];
    t.m[2] = t.m[6];
    t.m[6] = s;
    s = t.m[5];
    t.m[5] = t.m[7];
    t.m[7] = s;
    const Mat3* tp = &t;
    MulM((Mat3*)out->rot, (const Mat3*)a->rot, tp);
    RVec3* op = &out->pos;
    op->x = a->pos.x - b->pos.x;
    op->y = a->pos.y - b->pos.y;
    op->z = a->pos.z - b->pos.z;
    const RVec3* r = b->rot;
    f32 x = Dot3(op->x, r[0].x, op->y, r[0].y, op->z, r[0].z);
    f32 y = Dot3(op->x, r[1].x, op->y, r[1].y, op->z, r[1].z);
    f32 z = Dot3(op->x, r[2].x, op->y, r[2].y, op->z, r[2].z);
    op->x = x;
    op->y = y;
    op->z = z;
}

void fn_801BFB2C(PanelVol* p, RMtx* out, const RVec3* pts) {
    RMtx m;
    m.pos = pts[p->org];
    Sub(&m.rot[0], &pts[p->e0b], &pts[p->e0a]);
    Sub(&m.rot[1], &pts[p->e1b], &pts[p->e1a]);
    PSVECCrossProduct(&m.rot[0], &m.rot[1], &m.rot[2]);
    if (PSVECDotProduct(&m.rot[2], &m.rot[2]) > 0.0001f) {
        Orthonormalize((Mat3*)m.rot);
    } else {
        Identity((Mat3*)m.rot);
    }
    fn_801BFD34(&out->pos, &p->rest.pos, &m);
    Mat3Mul((Mat3*)out->rot, (const Mat3*)p->rest.rot, (const Mat3*)m.rot);
}

void fn_801BFD34(RVec3* out, const RVec3* v, const RMtx* m) {
    MulVec(out, v, m->rot);
    out->x += m->pos.x;
    out->y += m->pos.y;
    out->z += m->pos.z;
}

void fn_801BFE7C(RVec3* out, const RVec3* a, const RVec3* b) {
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}

void fn_801BFEB0(PanelVert* v) {}

s16* fn_801BFEB4(CollVolume* v) { return v->hint; }

void fn_801BFEBC(RVec3* v, f32 x, f32 y, f32 z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

void fn_801BFECC(CollVolume* v, const RMtx* m) {
    v->mtx = *m;
    v->Update();
}

void fn_801BFF64(RMtx* m, const RVec3* v) {
    RVec3 t;
    MulVec(&t, v, m->rot);
    m->pos.x = m->pos.x + t.x;
    m->pos.y = m->pos.y + t.y;
    m->pos.z = m->pos.z + t.z;
}

void fn_801C00B8(CollShape* s, const RVec3* c) {
    SphereView* sp = (SphereView*)s;
    sp->center.x = c->x;
    sp->center.y = c->y;
    sp->center.z = c->z;
    if (sp->owner) {
        fn_8019EECC(sp->owner, &sp->center);
    }
}

u32 fn_801C00F0(CollShape* s) { return *(u32*)((u8*)s + 0x18); }

CollShape* fn_801C00F8(CollVolume* v, int i) {
    if (i >= 0 && i < v->nAdded) {
        return v->sub[i];
    }
    return 0;
}

void fn_801C0124(RVec3* out, const RVec3* v, f32 s) {
    out->x = v->x * s;
    out->y = v->y * s;
    out->z = v->z * s;
}

void fn_801C014C(CollVolume* v, const RVec3* off) {
    v->offset = *off;
    v->CollVolume::vf20();
}

s32 fn_801C0168(Phob* car) { return *(s32*)((u8*)car + 0x5A4); }

s32 fn_801C0170(Phob* car) { return *(s32*)((u8*)car + 0x59C); }

f32 fn_801C0178(void) { return lbl_8055E3C8; }

}  // extern "C"

PanelVol::~PanelVol(void) {
    lbl_8055E7BC--;
    fn_801C1C7C(strip);
}

void PanelVol::Init(BodyDesc* d) {
    RVec3 c;
    RVec3 b;
    RVec3 a;
    BodyDesc desc;
    memset(&desc, 0, sizeof(desc));
    desc.size = sizeof(desc);
    desc.kind = d->kind;
    desc.mass = 45.45f;
    desc.ix = 4.22799f;
    desc.iy = 4.22799f;
    desc.iz = 4.22799f;
    RigidBody::Init(&desc);
    f1FC = 0.0f;
    f200 = 0.0f;
    f204 = 2.0f;
    f208 = 0.0f;
    f20C = 3.0f;
    f210 = -0.2f;
    c.x = 0.0f;
    a.y = 0.0f;
    b.z = 0.0f;
    void* mem = operator new(0x34, "panelvol");
    CollShape* s = (CollShape*)mem;
    if (mem) {
        s = fn_801A25F8(mem, &mtx, strip, this, 0.25f);
    }
    AddShape(s);
    b1E8 = 0;
}

void PanelVol::Reset(void) {
    owner = 0;
    lastUsed = -1000.0f;
    if (owner) {
        model = (s32)owner;
    } else {
        model = -1;
    }
    RigidBody::Reset();
    RMtx m;
    m.rot[0].x = 1.0f;
    m.rot[0].y = 0.0f;
    m.rot[0].z = 0.0f;
    m.rot[1].x = 0.0f;
    m.rot[1].y = 1.0f;
    m.rot[1].z = 0.0f;
    m.rot[2].x = 0.0f;
    m.rot[2].y = 0.0f;
    m.rot[2].z = 1.0f;
    m.pos.x = 0.0f;
    m.pos.z = 0.0f;
    m.pos.y = -500.0f;
    part = -1;
    car = 0;
    mtx = m;
    Update();
    vel.x = 0.0f;
    vel.y = 0.0f;
    vel.z = 0.0f;
    avel.x = 0.0f;
    avel.y = 0.0f;
    avel.z = 0.0f;
    pos.x = 0.0f;
    pos.y = -1000.0f;
    pos.z = 0.0f;
    dir.x = 0.0f;
    dir.y = 1.0f;
    dir.z = 0.0f;
}

int PanelVol::vf44(void) { return owner != 0; }

static inline void DbgLine(const RVec3& a, const RVec3& b) {}

void PanelVol::vf1C(void) {
    if (!owner || !strip) {
        return;
    }
    RVec3 f;
    RVec3 t;
    Flush(&f, &t);
    s32 id;
    s32 n = 8;
    RVec3 p = mtx.pos;
    RVec3 nrm;
    if (!fn_801E9E78(hint, &p, &nrm, &id)) {
        return;
    }
    p.x += nrm.x * 0.127f;
    p.y += nrm.y * 0.127f;
    p.z += nrm.z * 0.127f;
    nrm.x *= 0.1f;
    nrm.y *= 0.1f;
    nrm.z *= 0.1f;
    nrm.x += dir.x * 0.9f;
    nrm.y += dir.y * 0.9f;
    nrm.z += dir.z * 0.9f;
    PSVECNormalize(&nrm, &nrm);
    pos = p;
    dir = nrm;
    RVec3 pts[8];
    fn_801C2694(strip, pts, &n, &p, &nrm, &f, &t);
    int i;
    for (i = 0; i < n; i++) {
        RVec3 a = pts[i];
        RVec3 b = a;
        b.y += 0.2f;
        DbgLine(a, b);
    }
    RMtx m;
    fn_80144DC8(&m);
    fn_80144DAC(&m.pos, &pts[org]);
    fn_801C0E60(&m.rot[0], &pts[e0b], &pts[e0a]);
    fn_801C0E60(&m.rot[1], &pts[e1b], &pts[e1a]);
    fn_801C0E4C(&m.rot[2], &m.rot[0], &m.rot[1]);
    f32 eps = fn_80144CC8(0.01f);
    if (fn_801C0E44(&m.rot[2]) > eps) {
        fn_801C0D6C((Mat3*)m.rot);
    } else {
        fn_801C0D3C((Mat3*)m.rot);
    }
    RMtx w;
    fn_801C092C(&w, &rest, &m);
    mtx = w;
    Update();
}

extern "C" {

void fn_801C092C(RMtx* out, const RMtx* a, const RMtx* b) {
    MulVec(&out->pos, &a->pos, b->rot);
    out->pos.x += b->pos.x;
    out->pos.y += b->pos.y;
    out->pos.z += b->pos.z;
    MulM((Mat3*)out->rot, (const Mat3*)a->rot, (const Mat3*)b->rot);
}

void fn_801C0D3C(Mat3* m) { Identity(m); }

void fn_801C0D6C(Mat3* m) { Orthonormalize(m); }

f32 fn_801C0E44(const RVec3* v) { return PSVECDotProduct(v, v); }

void fn_801C0E4C(RVec3* out, const RVec3* a, const RVec3* b) { PSVECCrossProduct(a, b, out); }

void fn_801C0E60(RVec3* out, const RVec3* a, const RVec3* b) { Sub(out, a, b); }

}  // extern "C"

void PanelVol::vf28(u8* buf) {
    PanelPose* out = (PanelPose*)(buf + a8);
    f32 k = lbl_8055E460;
    if ((f32)__fabs(k - 1.0f) <= lbl_80558DC4[0]) {
        out->mtx = world;
    } else {
        RMtx t;
        int i;
        for (i = 0; i < 4; i++) {
            const RVec3* a = &world.rot[i];
            const RVec3* b = &prev.rot[i];
            RVec3* d = &t.rot[i];
            d->x = b->x + k * (a->x - b->x);
            d->y = b->y + k * (a->y - b->y);
            d->z = b->z + k * (a->z - b->z);
        }
        out->mtx = t;
    }
    out->hint = hint[0];
    PanelPose* q = (PanelPose*)(buf + a8);
    q->model = model;
    q->part = part;
    q->car = car;
}
