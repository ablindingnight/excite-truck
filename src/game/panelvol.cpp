// Panel volumes (0x801BF214-0x801C109C): loose body panels ("panelvol"). A pool of rigid bodies, each a
// quad that a car sheds (Attach): the quad is built from a box on the car, hangs off a cloth/strip
// simulation (fn_801C1BA4 handle) and is re-fitted to the simulated corners every update. A body
// is reused when free, else the oldest one is taken. Names are guesses apart from the strings.
#include "game/common.h"
#include "game/vec3_init.h"
#include "game/rvec.h"
#include "game/rmtx.h"
#include "game/collvol.h"
#include "game/rigid.h"

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
    virtual void Update(void);
    virtual void vf28(u8* buf);
    virtual int vf44(void);
    RigidBody* owner;   // offset 0x1EC
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
extern f32 __float_epsilon[];  // FLT_EPSILON

// DUMP_DATA lbl_802F98D0 {s32,s32,s32,s32}
static const PanelEdge lbl_802F98D0[6] = {
#include "dumpdata/lbl_802F98D0.inc"
};

void Mat3Mul(Mat3* out, const Mat3* a, const Mat3* b);
void* operator new(size_t size, const char* name);
void operator delete(void* p);

namespace {
extern const f32 kNegate;
extern const f32 kPanelU;
extern const f32 kPanelV;
extern const f32 kAxisTolerance;
}

extern "C" {
}  // extern "C"
f32 Dot(float, float, float, float, float, float);
// The identity-initialised 3x4 frame (camera library), as far as this unit uses it: an RMtx with a constructor.
struct CamFrame34 : RMtx {
    CamFrame34(void);
};
// The race-car record (aiobst.cpp), declared as far as this unit uses it.
class RaceCarRec {
public:
    const RMtx* Mtx(void);
};
extern "C" {
void __ct__4VPosFfff(RVec3* v, f32 x, f32 y, f32 z);
void fn_80144B54(RMtx* dst, const RMtx* src);
f32 fn_80144CC8(f32 x);
void fn_80144DAC(RVec3* dst, const RVec3* src);
void fn_8017E388(RMtx* m);
// RagdollShape::RagdollShape(frame, strip, owner, r) in sphcoll, called by its linker name.
CollShape* __ct__12RagdollShapeFUlUlUlf(void* mem, const RMtx* m, void* strip, CollVolume* o, f32 r);
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
s32 fn_801C0168(RigidBody* car);
s32 fn_801C0170(RigidBody* car);
f32 fn_801C0178(void);
void fn_801BF75C(RMtx* out, const RMtx* a, const RMtx* b);
void fn_801BFB2C(PanelVol* p, RMtx* out, const RVec3* pts);
void fn_801BFD34(RVec3* out, const RVec3* v, const RMtx* m);
const RVec3* fn_801BF754(RigidBody* car);
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
        f32 m0 = Dot(b->m[0], a->m[0], b->m[3], a->m[1], b->m[6], a->m[2]);
        f32 m3 = Dot(b->m[0], a->m[3], b->m[3], a->m[4], b->m[6], a->m[5]);
        f32 m6 = Dot(b->m[0], a->m[6], b->m[3], a->m[7], b->m[6], a->m[8]);
        f32 m1 = Dot(b->m[1], a->m[0], b->m[4], a->m[1], b->m[7], a->m[2]);
        f32 m4 = Dot(b->m[1], a->m[3], b->m[4], a->m[4], b->m[7], a->m[5]);
        f32 m7 = Dot(b->m[1], a->m[6], b->m[4], a->m[7], b->m[7], a->m[8]);
        f32 m2 = Dot(b->m[2], a->m[0], b->m[5], a->m[1], b->m[8], a->m[2]);
        f32 m5 = Dot(b->m[2], a->m[3], b->m[5], a->m[4], b->m[8], a->m[5]);
        f32 m8 = Dot(b->m[2], a->m[6], b->m[5], a->m[7], b->m[8], a->m[8]);
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
        out->m[0] = Dot(b->m[0], a->m[0], b->m[3], a->m[1], b->m[6], a->m[2]);
        out->m[3] = Dot(b->m[0], a->m[3], b->m[3], a->m[4], b->m[6], a->m[5]);
        out->m[6] = Dot(b->m[0], a->m[6], b->m[3], a->m[7], b->m[6], a->m[8]);
        out->m[1] = Dot(b->m[1], a->m[0], b->m[4], a->m[1], b->m[7], a->m[2]);
        out->m[4] = Dot(b->m[1], a->m[3], b->m[4], a->m[4], b->m[7], a->m[5]);
        out->m[7] = Dot(b->m[1], a->m[6], b->m[4], a->m[7], b->m[7], a->m[8]);
        out->m[2] = Dot(b->m[2], a->m[0], b->m[5], a->m[1], b->m[8], a->m[2]);
        out->m[5] = Dot(b->m[2], a->m[3], b->m[5], a->m[4], b->m[8], a->m[5]);
        out->m[8] = Dot(b->m[2], a->m[6], b->m[5], a->m[7], b->m[8], a->m[8]);
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

PanelVol* fn_801BF214(void) { return new ("panel") PanelVol; }

struct WVec : RVec3 {
    WVec(void) { DefaultConstructVec3(this); }
};

inline PanelVert::PanelVert(void) { fn_801BFEB0(this); }

static inline f32 PanelSurfaceOffset(void) { return kPanelV; }

static inline void SetupPanel(PanelVol* p, RigidBody* car, s32 part, const RVec3* off, const RVec3* lo, const RVec3* hi, int n, const s32*& ids) {
    int i;
    p->lastUsed = fn_801C0178();
    p->owner = car;
    p->model = fn_801C0170(car);
    p->part = part;
    p->car = fn_801C0168(p->owner);
    fn_801C014C(p, off);
    WVec neg;
    fn_801C0124(&neg, off, kNegate);
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
    WVec a;
    WVec b;
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
    const RVec3* av;
    int zb = 2;
    if (b.y - a.y > b.x - a.x) {
        yb = 1;
        zb = 2;
        xb = 4;
    }
    for (i = 0; i < 4; i++) {
        RVec3 c;
        __ct__4VPosFfff(&c, (i & xb) ? a.x : b.x, (i & yb) ? a.y : b.y, (i & zb) ? a.z : b.z);
        fn_801BFD34(&verts[i].p, &c, &m);
        verts[i].u = kPanelU;
        verts[i].v = kPanelV;
    }
    WVec pts[4];
    for (int k = 0; k < 4; k++) {
        fn_80144DAC(&pts[k], &verts[k].p);
    }
    CamFrame34 frame;
    fn_801BFB2C(p, &frame, pts);
    fn_801BF75C(&p->rest, &m, &frame);
    PanelLink links[2];
    for (i = 0; i < n; ids++, i++) {
        links[i].id = *ids;
        links[i].mtx = ((RaceCarRec*)car)->Mtx();
        links[i].c = 0;
    }
    fn_801C1CBC(p->strip, 4, verts, 6, edges, n, links);
    av = fn_801BF754(car);
    const RMtx* cm = ((RaceCarRec*)car)->Mtx();
    fn_801C256C(p->strip, car->Vel(), &cm->pos, av);
}

int fn_801BF2A4(RigidBody* car, s32 part, const RVec3* off, const RVec3* lo, const RVec3* hi, int n, const s32* ids) {
    if ((f32)__fabs(lo->x - hi->x) <= __float_epsilon[0] && (f32)__fabs(lo->y - hi->y) <= __float_epsilon[0] &&
        (f32)__fabs(lo->z - hi->z) <= __float_epsilon[0]) {
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
    if (p) {
        SetupPanel(p, car, part, off, lo, hi, n, ids);
        return 1;
    }
    return 0;
}

const RVec3* fn_801BF754(RigidBody* car) { return (const RVec3*)((u8*)car + 0x124); }

// out = a relative to b: a's basis times b's transposed basis, and a's position in b's frame.
void fn_801BF75C(RMtx* out, const RMtx* a, const RMtx* b) {
    Mat3 t = *(const Mat3*)&b->rot;
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
    MulM((Mat3*)&out->rot, (const Mat3*)&a->rot, tp);
    RVec3* op = &out->pos;
    op->x = a->pos.x - b->pos.x;
    op->y = a->pos.y - b->pos.y;
    op->z = a->pos.z - b->pos.z;
    const RVec3* r = &b->rot.x;
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
    Sub(&m.rot.x, &pts[p->e0b], &pts[p->e0a]);
    Sub(&m.rot.y, &pts[p->e1b], &pts[p->e1a]);
    PSVECCrossProduct(&m.rot.x, &m.rot.y, &m.rot.z);
    if (PSVECDotProduct(&m.rot.z, &m.rot.z) > kAxisTolerance) {
        Orthonormalize((Mat3*)&m.rot);
    } else {
        Identity((Mat3*)&m.rot);
    }
    fn_801BFD34(&out->pos, &p->rest.pos, &m);
    Mat3Mul((Mat3*)&out->rot, (const Mat3*)&p->rest.rot, (const Mat3*)&m.rot);
}

void fn_801BFD34(RVec3* out, const RVec3* v, const RMtx* m) {
    MulVec(out, v, &m->rot.x);
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
    v->Transform();
}

void fn_801BFF64(RMtx* m, const RVec3* v) {
    RVec3 t;
    MulVec(&t, v, &m->rot.x);
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

s32 fn_801C0168(RigidBody* car) { return *(s32*)((u8*)car + 0x5A4); }

s32 fn_801C0170(RigidBody* car) { return *(s32*)((u8*)car + 0x59C); }

f32 fn_801C0178(void) { return lbl_8055E3C8; }

}  // extern "C"

PanelVol::~PanelVol(void) {
    lbl_8055E7BC--;
    fn_801C1C7C(strip);
}

namespace {
// Template instantiation places the allocation tag after the vtable.
template <class T> struct PanelAllocTag {
    static char text[];
};

// Define these values in their emission order to match the float pool.
const f32 kNegate = -1.0f;
const f32 kPanelU = 4.545f;
const f32 kPanelV = 0.127f;
const f32 kAxisTolerance = 0.0001f;
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
    desc.ix = 4.227986812591553f;
    desc.iy = 4.227986812591553f;
    desc.iz = 4.227986812591553f;
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
    void* mem = operator new(0x34, PanelAllocTag<PanelVol>::text);
    CollShape* s = (CollShape*)mem;
    if (mem) {
        f32 scale = 0.25f;
        s = __ct__12RagdollShapeFUlUlUlf(mem, &mtx, strip, this, scale);
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
    m.rot.x.x = 1.0f;
    m.rot.x.y = 0.0f;
    m.rot.x.z = 0.0f;
    m.rot.y.x = 0.0f;
    m.rot.y.y = 1.0f;
    m.rot.y.z = 0.0f;
    m.rot.z.x = 0.0f;
    m.rot.z.y = 0.0f;
    m.rot.z.z = 1.0f;
    m.pos.x = 0.0f;
    m.pos.z = 0.0f;
    m.pos.y = -500.0f;
    part = -1;
    car = 0;
    mtx = m;
    Transform();
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

static inline void DbgLine(RVec3 a, RVec3 b) {}

// Helper name unproven: the scale arrives as a parameter, so it stays the right operand of each multiply.
static inline void AddScaled(RVec3* o, const RVec3* v, f32 s) {
    o->x += v->x * s;
    o->y += v->y * s;
    o->z += v->z * s;
}
void PanelVol::Update(void) {
    if (!owner) {
        return;
    }
    if (!strip) {
        return;
    }
    RVec3 f;
    RVec3 t;
    Flush(&f, &t);
    s32 n = 8;
    s32 id;
    RVec3 p = mtx.pos;
    RVec3 nrm;
    if (!fn_801E9E78(hint, &p, &nrm, &id)) {
        return;
    }
    f32 xv = nrm.x;
    const f32 k = PanelSurfaceOffset();
    p.x += xv * k;
    p.y += nrm.y * k;
    p.z += nrm.z * k;
    nrm.x *= 0.100000024f;
    nrm.y *= 0.100000024f;
    nrm.z *= 0.100000024f;
    AddScaled(&nrm, &dir, 0.9f);
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
    RMtx w;
    CamFrame34 m;
    fn_80144DAC(&m.pos, &pts[org]);
    fn_801C0E60(&m.rot.x, &pts[e0b], &pts[e0a]);
    fn_801C0E60(&m.rot.y, &pts[e1b], &pts[e1a]);
    fn_801C0E4C(&m.rot.z, &m.rot.x, &m.rot.y);
    f32 eps = fn_80144CC8(0.01f);
    if (fn_801C0E44(&m.rot.z) > eps) {
        fn_801C0D6C((Mat3*)&m.rot);
    } else {
        fn_801C0D3C((Mat3*)&m.rot);
    }
    fn_801C092C(&w, &rest, &m);
    mtx = w;
    Transform();
}

extern "C" {

void fn_801C092C(RMtx* out, const RMtx* a, const RMtx* b) {
    MulVec(&out->pos, &a->pos, &b->rot.x);
    out->pos.x += b->pos.x;
    out->pos.y += b->pos.y;
    out->pos.z += b->pos.z;
    MulM((Mat3*)&out->rot, (const Mat3*)&a->rot, (const Mat3*)&b->rot);
}

void fn_801C0D3C(Mat3* m) { Identity(m); }

void fn_801C0D6C(Mat3* m) { Orthonormalize(m); }

f32 fn_801C0E44(const RVec3* v) { return PSVECDotProduct(v, v); }

void fn_801C0E4C(RVec3* out, const RVec3* a, const RVec3* b) { PSVECCrossProduct(a, b, out); }

void fn_801C0E60(RVec3* out, const RVec3* a, const RVec3* b) { Sub(out, a, b); }

}  // extern "C"

template <class T>
static inline void SetXYZ(T* v, f32 x, f32 y, f32 z) { /* Helper name unproven */
    v->y = y;
    v->x = x;
    v->z = z;
}

static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

static inline void LerpMtx(RMtx* out, const RMtx* a, const RMtx* b, f32 t) {
    f32* d = (f32*)out;
    const f32* x = (const f32*)a;
    const f32* y = (const f32*)b;
    for (int i = 0; i < 12; i++) {
        *d++ = Lerp(*x++, *y++, t);
    }
}

void PanelVol::vf28(u8* buf) {
    PanelPose* out = (PanelPose*)(buf + a8);
    if ((f32)__fabs(lbl_8055E460 - 1.0f) <= __float_epsilon[0]) {
        out->mtx = world;
    } else {
        RMtx t;
        LerpMtx(&t, &prev, &world, lbl_8055E460);
        out->mtx = t;
    }
    out->hint = hint[0];
    PanelPose* q = (PanelPose*)(buf + a8);
    q->model = model;
    q->part = part;
    q->car = car;
}

namespace {
template <class T> char PanelAllocTag<T>::text[] = "panelvol";
}
