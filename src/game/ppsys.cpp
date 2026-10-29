// Point-particle systems (0x801C109C-0x801C2F58): "ppsys", the Verlet cloth/strip solver behind the loose
// panels (panelvol). Particles (position, previous position, acceleration, two per-particle weights and a
// free flag) are joined by distance sticks and pinned to moving frames by anchors; a step integrates,
// relaxes the constraints and resets the accelerations to gravity. Names are guesses apart from the strings.
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

// A distance constraint between two particles.
struct PStick {
    s32 a;      // offset 0x0
    s32 b;      // offset 0x4
    f32 rest;   // offset 0x8
    u8 slack;   // offset 0xC: may shorten (rope) rather than stay rigid
    f32 stiff;  // offset 0x10
};

// A particle pinned to a point in a moving frame.
struct PAnchor {
    s32 idx;          // offset 0x0
    const RMtx* mtx;  // offset 0x4
    RVec3 local;      // offset 0x8
    u8 on;            // offset 0x14
};

struct PLink {
    s32 a;      // offset 0x0
    s32 b;      // offset 0x4
    f32 c;      // offset 0x8
    f32 d;      // offset 0xC
    f32 e;      // offset 0x10
    f32 f;      // offset 0x14
    void* g;    // offset 0x18
};

// Setup records.
struct PVertDesc {
    RVec3 pos;  // offset 0x0
    f32 w34;    // offset 0xC
    f32 w30;    // offset 0x10
};

struct PEdgeDesc {
    s32 a;      // offset 0x0
    s32 b;      // offset 0x4
    f32 extra;  // offset 0x8
    f32 stiff;  // offset 0xC: negative = slack
};

struct PPinDesc {
    s32 idx;           // offset 0x0
    const RMtx* mtx;   // offset 0x4
    const RVec3* at;   // offset 0x8: world point, or 0 = the particle's position
};

struct PSysDesc {
    PVertDesc* verts;   // offset 0x0
    PEdgeDesc* edges;   // offset 0x4
    PPinDesc* pins;     // offset 0x8
    PLink* links;       // offset 0xC
    s32 nVerts;         // offset 0x10
    s32 nEdges;         // offset 0x14
    s32 nPins;          // offset 0x18
    s32 nLinks;         // offset 0x1C
};

class PSys {
public:
    PSys(int nPart, int nStick, int nAnchor, int nLink);
    ~PSys(void);
    u8* mem;            // offset 0x0
    s32 maxPart;        // offset 0x4
    s32 maxStick;       // offset 0x8
    s32 maxAnchor;      // offset 0xC
    s32 maxLink;        // offset 0x10
    s32 nPart;          // offset 0x14
    s32 nStick;         // offset 0x18
    s32 nAnchor;        // offset 0x1C
    s32 nLink;          // offset 0x20
    RVec3* pos;         // offset 0x24
    RVec3* old;         // offset 0x28
    RVec3* acc;         // offset 0x2C
    f32* w30;           // offset 0x30
    f32* w34;           // offset 0x34
    s32* free;          // offset 0x38
    RVec3 gravity;      // offset 0x3C
    PStick* sticks;     // offset 0x48
    PAnchor* anchors;   // offset 0x4C
    PLink* links;       // offset 0x50
    f32 dt;             // offset 0x54
    RVec3 org;          // offset 0x58
    RVec3 up;           // offset 0x64
};

// The handle panelvol holds: a PSys with nothing added.
class PSysObj : public PSys {
public:
    PSysObj(int nPart, int nStick, int nAnchor, int nLink) : PSys(nPart, nStick, nAnchor, nLink) {}
    ~PSysObj(void) {}
};

ModuleTag lbl_8055E7C0;
UnkStaticFFFF lbl_8055E7C8;

extern f32 lbl_80558DC4[];  // FLT_EPSILON

void* operator new(size_t size, const char* name);
void* operator new[](size_t size, const char* name);
void operator delete(void* p);
void operator delete[](void* p);

extern "C" {
f32 PSVECDistance(const void* a, const void* b);
void PSVECCrossProduct(const void* a, const void* b, void* out);
void fn_801C1218(PSys* s);
void fn_801C1364(PSys* s);
void fn_801C1320(PSys* s);
void fn_801C1B60(PSys* s);
}

PSys::PSys(int nPart, int nStick, int nAnchor, int nLink) {
    int sb = nStick * sizeof(PStick);
    maxStick = nStick;
    maxPart = nPart;
    int ab = nAnchor * sizeof(PAnchor);
    maxAnchor = nAnchor;
    maxLink = nLink;
    mem = new ("psys_mem") u8[ab + sb + (nLink * sizeof(PLink) + nPart * 0x30)];
    u8* p = mem;
    pos = (RVec3*)p;
    p += nPart * sizeof(RVec3);
    old = (RVec3*)p;
    p += nPart * sizeof(RVec3);
    acc = (RVec3*)p;
    p += nPart * sizeof(RVec3);
    w30 = (f32*)p;
    p += nPart * sizeof(f32);
    w34 = (f32*)p;
    p += nPart * sizeof(f32);
    free = (s32*)p;
    p += nPart * sizeof(s32);
    sticks = (PStick*)p;
    p += sb;
    anchors = (PAnchor*)p;
    p += ab;
    if (nLink > 0) {
        links = (PLink*)p;
    } else {
        links = 0;
    }
    this->nPart = 0;
    this->nStick = 0;
    this->nAnchor = 0;
    this->nLink = 0;
    dt = 1.0f / 60.0f;
    gravity.x = 0.0f;
    gravity.y = -9.81f;
    gravity.z = 0.0f;
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    org.x = 0.0f;
    org.y = 0.0f;
    org.z = 0.0f;
}

PSys::~PSys(void) { delete[] mem; }

static inline void AddTo(RVec3* out, const RVec3* v) {
    out->x += v->x;
    out->y += v->y;
    out->z += v->z;
}

static inline void Set(RVec3* out, const RVec3* v) {
    out->x = v->x;
    out->y = v->y;
    out->z = v->z;
}

static inline void AddScaled(RVec3* out, const RVec3* v, const RVec3* d, f32 k) {
    out->x = v->x + d->x * k;
    out->y = v->y + d->y * k;
    out->z = v->z + d->z * k;
}

static inline void Sub(RVec3* out, const RVec3* a, const RVec3* b) {
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

static inline void Scale(RVec3* out, const RVec3* v, f32 k) {
    out->x = v->x * k;
    out->y = v->y * k;
    out->z = v->z * k;
}

static inline void AddPart(PSys* s, const RVec3* p, f32 b, f32 a) {
    int i = s->nPart++;
    s->pos[i].x = p->x;
    s->pos[i].y = p->y;
    s->pos[i].z = p->z;
    s->old[i].x = p->x;
    s->old[i].y = p->y;
    s->old[i].z = p->z;
    s->acc[i].x = 0.0f;
    s->acc[i].y = 0.0f;
    s->acc[i].z = 0.0f;
    s->w30[i] = a;
    s->w34[i] = b;
    s->free[i] = 1;
}

static inline void AddStick(PSys* s, s32 a, s32 b, f32 extra, f32 stiff) {
    if ((f32)__fabs(stiff) <= lbl_80558DC4[0]) {
        stiff = 1.0f;
    }
    PStick* k = &s->sticks[s->nStick++];
    k->a = a;
    k->b = b;
    k->rest = extra + PSVECDistance(&s->pos[a], &s->pos[b]);
    k->stiff = __fabs(stiff);
    k->slack = stiff < -0.001f;
}

static inline void AddAnchor(PSys* s, s32 idx, const RMtx* m, const RVec3* at) {
    RVec3 d;
    d.x = at->x - m->pos.x;
    d.y = at->y - m->pos.y;
    d.z = at->z - m->pos.z;
    PAnchor* a = &s->anchors[s->nAnchor++];
    a->on = 1;
    a->idx = idx;
    a->mtx = m;
    a->local.x = d.x * m->rot[0].x + d.y * m->rot[0].y + d.z * m->rot[0].z;
    a->local.y = d.x * m->rot[1].x + d.y * m->rot[1].y + d.z * m->rot[1].z;
    a->local.z = d.x * m->rot[2].x + d.y * m->rot[2].y + d.z * m->rot[2].z;
    s->free[idx] = 0;
}

extern "C" {

void fn_801C1218(PSys* s) {
    f32 damp = -0.995f;
    f32 keep = 1.995f;
    for (int i = 0; i < s->nPart; i++) {
        RVec3* p = &s->pos[i];
        RVec3* o = &s->old[i];
        RVec3* a = &s->acc[i];
        RVec3 t = *p;
        f32 dt2 = s->dt * s->dt;
        p->x *= keep;
        p->y *= keep;
        p->z *= keep;
        p->x += o->x * damp;
        p->y += o->y * damp;
        p->z += o->z * damp;
        p->x += a->x * dt2;
        p->y += a->y * dt2;
        p->z += a->z * dt2;
        o->x = t.x;
        o->y = t.y;
        o->z = t.z;
    }
}

void fn_801C1320(PSys* s) {
    for (int i = 0; i < s->nPart; i++) {
        Set(&s->acc[i], &s->gravity);
    }
}

// Constraint relaxation (placeholder: the original solver is not reconstructed yet).
void fn_801C1364(PSys* s) {}

void fn_801C1B60(PSys* s) {
    fn_801C1218(s);
    fn_801C1364(s);
    fn_801C1320(s);
}

void fn_801C1B9C(void) {}

void fn_801C1BA0(void) {}

PSysObj* fn_801C1BA4(int nPart, int nStick, int nAnchor) {
    return new ("ppsys") PSysObj(nPart, nStick, nAnchor, 0);
}

PSysObj* fn_801C1C1C(const PSysDesc* d) { return new ("ppsys") PSysObj(d->nVerts, d->nEdges, d->nPins, d->nLinks); }

void fn_801C1C7C(PSysObj* s) { delete s; }

void fn_801C1CBC(PSys* s, int nv, const PVertDesc* v, int ne, const PEdgeDesc* e, int np, const PPinDesc* p) {
    int i;
    s->nPart = 0;
    s->nStick = 0;
    s->nAnchor = 0;
    s->nLink = 0;
    for (i = 0; i < nv; i++, v++) {
        AddPart(s, &v->pos, v->w34, v->w30);
    }
    for (i = 0; i < ne; i++, e++) {
        AddStick(s, e->a, e->b, e->extra, e->stiff);
    }
    for (i = 0; i < np; i++, p++) {
        AddAnchor(s, p->idx, p->mtx, p->at ? p->at : &s->pos[p->idx]);
    }
}

void fn_801C2170(PSys* s, const PSysDesc* d, void* owner) {
    int i;
    s->nPart = 0;
    s->nStick = 0;
    s->nAnchor = 0;
    s->nLink = 0;
    for (i = 0; i < d->nVerts; i++) {
        AddPart(s, &d->verts[i].pos, d->verts[i].w34, d->verts[i].w30);
    }
    for (i = 0; i < d->nEdges; i++) {
        const PEdgeDesc* e = &d->edges[i];
        AddStick(s, e->a, e->b, e->extra, e->stiff);
    }
    for (i = 0; i < d->nPins; i++) {
        PAnchor* a = &s->anchors[s->nAnchor++];
        const PPinDesc* p = &d->pins[i];
        a->on = 1;
        a->idx = p->idx;
        a->mtx = (const RMtx*)owner;
        a->local.x = p->at->x;
        a->local.y = p->at->y;
        a->local.z = p->at->z;
        s->free[p->idx] = 0;
    }
    for (i = 0; i < d->nLinks; i++) {
        const PLink* l = &d->links[i];
        PLink* k = &s->links[s->nLink++];
        k->a = l->a;
        k->b = l->b;
        k->c = l->c;
        k->d = l->d;
        k->e = l->e;
        k->f = l->f;
        k->g = owner;
    }
}

// Gives every particle the velocity of a body moving with 'vel' and spinning with 'avel' about 'at'.
void fn_801C256C(PSys* s, const RVec3* vel, const RVec3* at, const RVec3* avel) {
    for (int i = 0; i < s->nPart; i++) {
        RVec3 v;
        RVec3 r;
        Sub(&r, &s->pos[i], at);
        PSVECCrossProduct(avel, &r, &v);
        AddTo(&v, vel);
        AddScaled(&s->old[i], &s->pos[i], &v, -(1.0f / 60.0f));
    }
}

void fn_801C2694(PSys* s, RVec3* out, s32* n, const RVec3* org, const RVec3* up, const RVec3* f) {
    s->org = *org;
    s->up = *up;
    RVec3 a;
    Scale(&a, f, 0.0110011f);
    for (int i = 0; i < s->nPart; i++) {
        AddTo(&s->acc[i], &a);
    }
    fn_801C1B60(s);
    s32 k = s->nPart;
    for (int j = 0; j < s->nPart; j++) {
        Set(out, &s->pos[j]);
        out++;
    }
    *n = k;
}

void fn_801C27D8(PSys* s, const RVec3* org, const RVec3* up, const RVec3* f) {
    s->org = *org;
    s->up = *up;
    RVec3 a;
    Scale(&a, f, 0.0110011f);
    for (int i = 0; i < s->nPart; i++) {
        AddTo(&s->acc[i], &a);
    }
    fn_801C1B60(s);
}

// Particle collision against boxes (placeholder).
void fn_801C28A8(PSys* s) {}

void fn_801C2B9C(RVec3* out, const RVec3* a, const RVec3* d, f32 k) {
    out->x = a->x + d->x * k;
    out->y = a->y + d->y * k;
    out->z = a->z + d->z * k;
}

void fn_801C2BDC(PSys* s, s32* n, RVec3** pos, RVec3** old) {
    *n = s->nPart;
    *pos = s->pos;
    *old = s->old;
}

// Debug draw of the sticks (placeholder).
void fn_801C2BF8(PSys* s, void* ctx) {}

}  // extern "C"
