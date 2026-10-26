// Rigid bodies (0x8017B0B0-0x8017D378): a collision volume with mass, an inertia tensor, linear and angular
// velocity, accumulated force and torque, and a list of pending impulses (from a pool of 0x28-byte records,
// "ext_imp_tab") that are averaged and applied once per tick, with the hit object notified. Also packs and
// unpacks the velocity into the state packet and keeps the body in its collision-grid cell. Names are guesses
// apart from the strings.
#include "game/common.h"

extern "C" {
void* memset(void* dst, int c, u32 n);
void PSVECCrossProduct(const void* a, const void* b, void* out);
f32 PSVECDotProduct(const void* a, const void* b);
}  // extern "C"
f32 Dot(float, float, float, float, float, float);
extern "C" {
void* fn_801B60F4(const void* pos, f32 r);                            // grid cell at
void fn_801B6428(void* cell, void* body);                             // cell remove
void fn_801B63E0(void* cell, void* body);                             // cell add
void fn_801AE158(void* contact);
void fn_801ADB44(void* contact, void* body);
void PSVECNormalize(const void* src, void* dst);
f32 PSVECMag(const void* v);
double sin(double x);
double cos(double x);
}

void* operator new[](size_t size, const char* tag);

extern f32 lbl_80558DC4[];  // FLT_EPSILON

struct RVec3 {
    f32 x;
    f32 y;
    f32 z;
};

struct RMat33 {
    f32 m[3][3];
    void Transpose(void) {
        f32 t;
        t = m[0][1];
        m[0][1] = m[1][0];
        m[1][0] = t;
        t = m[0][2];
        m[0][2] = m[2][0];
        m[2][0] = t;
        t = m[1][2];
        m[1][2] = m[2][1];
        m[2][1] = t;
    }
};

struct RMtx {
    RMat33 rot;  // offset 0x0
    RVec3 pos;   // offset 0x24
};

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

static inline void Mul(RMat33* out, const RMat33* l, const RMat33* r) {
    if (out == l || out == r) {
        f32 m00 = Dot(r->m[0][0], l->m[0][0], r->m[1][0], l->m[0][1], r->m[2][0], l->m[0][2]);
        f32 m10 = Dot(r->m[0][0], l->m[1][0], r->m[1][0], l->m[1][1], r->m[2][0], l->m[1][2]);
        f32 m20 = Dot(r->m[0][0], l->m[2][0], r->m[1][0], l->m[2][1], r->m[2][0], l->m[2][2]);
        f32 m01 = Dot(r->m[0][1], l->m[0][0], r->m[1][1], l->m[0][1], r->m[2][1], l->m[0][2]);
        f32 m11 = Dot(r->m[0][1], l->m[1][0], r->m[1][1], l->m[1][1], r->m[2][1], l->m[1][2]);
        f32 m21 = Dot(r->m[0][1], l->m[2][0], r->m[1][1], l->m[2][1], r->m[2][1], l->m[2][2]);
        f32 m02 = Dot(r->m[0][2], l->m[0][0], r->m[1][2], l->m[0][1], r->m[2][2], l->m[0][2]);
        f32 m12 = Dot(r->m[0][2], l->m[1][0], r->m[1][2], l->m[1][1], r->m[2][2], l->m[1][2]);
        f32 m22 = Dot(r->m[0][2], l->m[2][0], r->m[1][2], l->m[2][1], r->m[2][2], l->m[2][2]);
        out->m[0][0] = m00;
        out->m[1][0] = m10;
        out->m[2][0] = m20;
        out->m[0][1] = m01;
        out->m[1][1] = m11;
        out->m[2][1] = m21;
        out->m[0][2] = m02;
        out->m[1][2] = m12;
        out->m[2][2] = m22;
    } else {
        out->m[0][0] = Dot(r->m[0][0], l->m[0][0], r->m[1][0], l->m[0][1], r->m[2][0], l->m[0][2]);
        out->m[1][0] = Dot(r->m[0][0], l->m[1][0], r->m[1][0], l->m[1][1], r->m[2][0], l->m[1][2]);
        out->m[2][0] = Dot(r->m[0][0], l->m[2][0], r->m[1][0], l->m[2][1], r->m[2][0], l->m[2][2]);
        out->m[0][1] = Dot(r->m[0][1], l->m[0][0], r->m[1][1], l->m[0][1], r->m[2][1], l->m[0][2]);
        out->m[1][1] = Dot(r->m[0][1], l->m[1][0], r->m[1][1], l->m[1][1], r->m[2][1], l->m[1][2]);
        out->m[2][1] = Dot(r->m[0][1], l->m[2][0], r->m[1][1], l->m[2][1], r->m[2][1], l->m[2][2]);
        out->m[0][2] = Dot(r->m[0][2], l->m[0][0], r->m[1][2], l->m[0][1], r->m[2][2], l->m[0][2]);
        out->m[1][2] = Dot(r->m[0][2], l->m[1][0], r->m[1][2], l->m[1][1], r->m[2][2], l->m[1][2]);
        out->m[2][2] = Dot(r->m[0][2], l->m[2][0], r->m[1][2], l->m[2][1], r->m[2][2], l->m[2][2]);
    }
}

struct RPacket {
    u8 pad0[0x14];
    RVec3 vel;  // offset 0x14
};

struct BodyDesc {
    u8 pad0[0x8];
    f32 mass;   // offset 0x8
    f32 ix;     // offset 0xC
    f32 iy;     // offset 0x10
    f32 iz;     // offset 0x14
};

class RShape {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void Wake(void);  // 0x14
};

class Phob {
public:
    Phob(void);
    virtual void Init(BodyDesc* d);   // 0x8
    virtual ~Phob(void);              // 0xC
    virtual void Reset(void);         // 0x10
    virtual void vf14(void);          // 0x14
    virtual void vf18(void);          // 0x18
    virtual void Update(void);        // 0x1C
    virtual void vf20(void);          // 0x20
    virtual void Notify(const RVec3* v, const RVec3* at, int kind, void* c, void* by);  // 0x24
    virtual void vf28(u8* buf);       // 0x28
    virtual s32 vf2C(void);           // 0x2C
    virtual void vf30(f32 t, RPacket* a, RPacket* b, s32* na, s32* nb);  // 0x30
    virtual s32 vf34(RPacket* p);     // 0x34
    virtual void vf38(void);          // 0x38
    virtual void vf3C(void);          // 0x3C
    virtual int vf40(void);           // 0x40
    virtual void vf44(void);          // 0x44
    void Xform(void);
    void Xform2(void);
    s32 a4;          // offset 0x4
    s32 a8;          // offset 0x8
    void* desc;      // offset 0xC
    s32 type;        // offset 0x10
    RShape* sub[4];  // offset 0x14
    s32 nAdded;      // offset 0x24
    s32 nSub;        // offset 0x28
    f32 radius;      // offset 0x2C
    RVec3 offset;    // offset 0x30
    RMtx mtx;        // offset 0x3C
    s16 hint[2];     // offset 0x6C
    RMtx world;      // offset 0x70
    RMtx prev;       // offset 0xA0
};

struct Contact {
    Contact* next;  // offset 0x0
};

struct ImpRec {
    ImpRec(void);
    Phob* obj;   // offset 0x0
    void* by;    // offset 0x4
    RVec3 v;     // offset 0x8
    RVec3 at;    // offset 0x14
    int kind;    // offset 0x20
    void* c;     // offset 0x24
};

class RigidBody : public Phob {
public:
    RigidBody(void);
    virtual void Init(BodyDesc* d);
    virtual void Reset(void);
    virtual void vf18(void) {}
    virtual void Update(void);
    virtual s32 vf2C(void) { return 0x20; }
    virtual void vf30(f32 t, RPacket* a, RPacket* b, s32* na, s32* nb);
    virtual s32 vf34(RPacket* p);
    virtual int vf40(void) { return 1; }
    virtual void vf3C(void) {}
    virtual void OnHit(const RVec3* f, const RVec3* at, int kind, void* c);  // 0x48
    virtual void OnImpulse(const RVec3* v, const RVec3* at, int kind, void* c, void* by);  // 0x4C
    virtual void OnImpulses(RVec3* f, RVec3* t);  // 0x50
    void ResetState(void);
    void SetMass(f32 m, f32 ix, f32 iy, f32 iz);
    void AddForceAt(const RVec3* f, const RVec3* at);
    void AddTorque(const RVec3* t);
    void AddImpulse(f32 w, const RVec3* v, const RVec3* at);
    void Push(Phob* obj, void* by, const RVec3* v, const RVec3* at, int kind, const RVec3* at2, void* c, f32 w);
    void ClearImpulses(void);
    void VelAt(RVec3* out, const RVec3* p);
    void VelAt2(RVec3* out, const RVec3* p);
    void Flush(RVec3* f, RVec3* t);
    void SetState(const RMtx* m, const RVec3* v, const RVec3* w);
    void UpdateCell(void);
    Contact* contacts;  // offset 0xD0
    f32 mass;           // offset 0xD4
    RVec3 inertia;      // offset 0xD8
    f32 invMass;        // offset 0xE4
    RVec3 invInertia;   // offset 0xE8
    RMat33 invI;        // offset 0xF4
    RVec3 vel;          // offset 0x118
    RVec3 avel;         // offset 0x124
    RVec3 force;        // offset 0x130
    RVec3 torque;       // offset 0x13C
    RMat33 worldInvI;   // offset 0x148
    RVec3 accel;        // offset 0x16C
    RVec3 aaccel;       // offset 0x178
    void* cell;         // offset 0x184
    RVec3 impF;         // offset 0x188
    RVec3 impT;         // offset 0x194
    f32 impW;           // offset 0x1A0
    ImpRec* imps[16];   // offset 0x1A4
    s32 nImp;           // offset 0x1E4
    u8 b1E8;            // offset 0x1E8
    u8 b1E9;            // offset 0x1E9
};

ModuleTag lbl_8055E480;
s32 lbl_8055E488;
s32 lbl_8055E48C;
ImpRec* lbl_8055E490;

RigidBody::RigidBody(void) {
    cell = 0;
    b1E8 = 0;
}

void RigidBody::Init(BodyDesc* d) {
    Phob::Init(d);
    contacts = 0;
    SetMass(d->mass, d->ix, d->iy, d->iz);
    cell = 0;
    ResetState();
}

void RigidBody::Reset(void) {
    Phob::Reset();
    ResetState();
}

void RigidBody::ResetState(void) {
    Contact* c;
    nImp = 0;
    vel.x = 0.0f;
    vel.y = 0.0f;
    vel.z = 0.0f;
    avel.x = 0.0f;
    avel.y = 0.0f;
    avel.z = 0.0f;
    force.x = 0.0f;
    force.y = 0.0f;
    force.z = 0.0f;
    torque.x = 0.0f;
    torque.y = 0.0f;
    torque.z = 0.0f;
    accel.x = 0.0f;
    accel.y = 0.0f;
    accel.z = 0.0f;
    aaccel.x = 0.0f;
    aaccel.y = 0.0f;
    aaccel.z = 0.0f;
    impF.x = 0.0f;
    impF.y = 0.0f;
    impF.z = 0.0f;
    impT.x = 0.0f;
    impT.y = 0.0f;
    impT.z = 0.0f;
    impW = 0.0f;
    b1E9 = 1;
    memset(imps, 0, sizeof(imps));
    worldInvI = invI;
    if (cell) {
        fn_801B6428(cell, this);
    }
    cell = 0;
    for (c = contacts; c; c = c->next) {
        fn_801AE158(c);
    }
}

static inline f32 LbToKg(f32 x) { return 0.4545f * x; }
static inline f32 FtToM(f32 x) { return 0.305f * x; }

void RigidBody::SetMass(f32 m, f32 ix, f32 iy, f32 iz) {
    mass = LbToKg(m);
    inertia.x = FtToM(FtToM(LbToKg(ix)));
    inertia.y = FtToM(FtToM(LbToKg(iy)));
    inertia.z = FtToM(FtToM(LbToKg(iz)));
    invMass = 1.0f / mass;
    invInertia.x = 1.0f / inertia.x;
    invInertia.y = 1.0f / inertia.y;
    invInertia.z = 1.0f / inertia.z;
    invI.m[0][0] = invInertia.x;
    invI.m[0][1] = 0.0f;
    invI.m[0][2] = 0.0f;
    invI.m[1][0] = 0.0f;
    invI.m[1][1] = invInertia.y;
    invI.m[1][2] = 0.0f;
    invI.m[2][0] = 0.0f;
    invI.m[2][1] = 0.0f;
    invI.m[2][2] = invInertia.z;
    worldInvI = invI;
}

void RigidBody::AddForceAt(const RVec3* f, const RVec3* at) {
    if (!((f32)__fabs(PSVECDotProduct(f, f)) <= lbl_80558DC4[0])) {
        RVec3 r;
        RVec3 t;
        r.x = at->x - mtx.pos.x;
        r.y = at->y - mtx.pos.y;
        r.z = at->z - mtx.pos.z;
        PSVECCrossProduct(&r, f, &t);
        force.x += f->x;
        force.y += f->y;
        force.z += f->z;
        torque.x += t.x;
        torque.y += t.y;
        torque.z += t.z;
    }
}

void RigidBody::AddTorque(const RVec3* t) {
    torque.x += t->x;
    torque.y += t->y;
    torque.z += t->z;
}

void RigidBody::AddImpulse(f32 w, const RVec3* v, const RVec3* at) {
    RVec3 r;
    RVec3 t;
    impW += w;
    impF.x += v->x * w;
    impF.y += v->y * w;
    impF.z += v->z * w;
    r.x = at->x - mtx.pos.x;
    r.y = at->y - mtx.pos.y;
    r.z = at->z - mtx.pos.z;
    PSVECCrossProduct(&r, v, &t);
    impT.x += t.x * w;
    impT.y += t.y * w;
    impT.z += t.z * w;
}

static inline ImpRec* AllocImp(void) {
    if (lbl_8055E488 < lbl_8055E48C) {
        return &lbl_8055E490[lbl_8055E488++];
    }
    return 0;
}

void RigidBody::Push(Phob* obj, void* by, const RVec3* v, const RVec3* at, int kind, const RVec3* at2, void* c,
                     f32 w) {
    if (nImp < 16) {
        ImpRec* p = AllocImp();
        if (p) {
            p->obj = obj;
            p->by = by;
            p->v.x = v->x * w;
            p->v.y = v->y * w;
            p->v.z = v->z * w;
            if (at2) {
                p->at = *at2;
            } else {
                p->at = *at;
            }
            p->kind = kind;
            p->c = c;
            imps[nImp++] = p;
        }
    }
    AddImpulse(w, v, at);
}

void RigidBody::ClearImpulses(void) {
    nImp = 0;
    impF.x = 0.0f;
    impF.y = 0.0f;
    impF.z = 0.0f;
    impT.x = 0.0f;
    impT.y = 0.0f;
    impT.z = 0.0f;
    impW = 0.0f;
}

void RigidBody::VelAt(RVec3* out, const RVec3* p) {
    RVec3 r;
    r.x = p->x - mtx.pos.x;
    r.y = p->y - mtx.pos.y;
    r.z = p->z - mtx.pos.z;
    PSVECCrossProduct(&avel, &r, out);
    out->x += vel.x;
    out->y += vel.y;
    out->z += vel.z;
}

void RigidBody::VelAt2(RVec3* out, const RVec3* p) {
    RVec3 r;
    r.x = p->x - mtx.pos.x;
    r.y = p->y - mtx.pos.y;
    r.z = p->z - mtx.pos.z;
    PSVECCrossProduct(&aaccel, &r, out);
    out->x += accel.x;
    out->y += accel.y;
    out->z += accel.z;
}

static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

static inline void SetV(RVec3* v, f32 x, f32 y, f32 z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

static inline void MulVec(RVec3* out, const RVec3* in, const RMat33* m) {
    if (out == in) {
        f32 x = Dot3(in->x, m->m[0][0], in->y, m->m[1][0], in->z, m->m[2][0]);
        f32 y = Dot3(in->x, m->m[0][1], in->y, m->m[1][1], in->z, m->m[2][1]);
        f32 z = Dot3(in->x, m->m[0][2], in->y, m->m[1][2], in->z, m->m[2][2]);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, m->m[0][0], in->y, m->m[1][0], in->z, m->m[2][0]);
        out->y = Dot3(in->x, m->m[0][1], in->y, m->m[1][1], in->z, m->m[2][1]);
        out->z = Dot3(in->x, m->m[0][2], in->y, m->m[1][2], in->z, m->m[2][2]);
    }
}

void RigidBody::Update(void) {
    RVec3 g;
    RVec3 dr;
    Contact* c;
    SetV(&g, 0.0f, 9.81f * -mass, 0.0f);
    force.x += g.x;
    force.y += g.y;
    force.z += g.z;
    for (c = contacts; c; c = c->next) {
        fn_801ADB44(c, this);
    }
    if ((nAdded > 0 ? sub[0] : 0) && b1E9 && b1E8) {
        (nAdded > 0 ? sub[0] : 0)->Wake();
    }
    if (!((f32)__fabs(impW) <= lbl_80558DC4[0])) {
        int i;
        f32 inv = 1.0f / impW;
        impF.x *= inv;
        impF.y *= inv;
        impF.z *= inv;
        impT.x *= inv;
        impT.y *= inv;
        impT.z *= inv;
        force.x += impF.x * 59.94f;
        force.y += impF.y * 59.94f;
        force.z += impF.z * 59.94f;
        torque.x += impT.x * 59.94f;
        torque.y += impT.y * 59.94f;
        torque.z += impT.z * 59.94f;
        for (i = 0; i < nImp; i++) {
            ImpRec* p = imps[i];
            if (p->by) {
                int j;
                for (j = i + 1; j < nImp; j++) {
                    ImpRec* q = imps[j];
                    if (p->by == q->by) {
                        RVec3 mid;
                        p->v.x += q->v.x;
                        p->v.y += q->v.y;
                        p->v.z += q->v.z;
                        SetV(&mid, Lerp(p->at.x, q->at.x, 0.5f), Lerp(p->at.y, q->at.y, 0.5f), Lerp(p->at.z, q->at.z, 0.5f));
                        p->at = mid;
                        imps[j] = imps[--nImp];
                    }
                }
            }
        }
        for (i = 0; i < nImp; i++) {
            ImpRec* p = imps[i];
            f32 k = 1.0f / impW;
            p->v.x *= k;
            p->v.y *= k;
            p->v.z *= k;
            if (p->obj) {
                p->obj->Notify(&p->v, &p->at, p->kind, p->c, p->by);
            } else {
                OnImpulse(&p->v, &p->at, p->kind, p->c, p->by);
            }
        }
        OnImpulses(&impF, &impT);
        impF.x = 0.0f;
        impF.y = 0.0f;
        impF.z = 0.0f;
        impT.x = 0.0f;
        impT.y = 0.0f;
        impT.z = 0.0f;
    }
    {
        f32 k = 0.0166833512f * invMass;
        vel.x += force.x * k;
        vel.y += force.y * k;
        vel.z += force.z * k;
        accel.x = force.x * invMass;
        accel.y = force.y * invMass;
        accel.z = force.z * invMass;
    }
    nImp = 0;
    impW = 0.0f;
    if (PSVECDotProduct(&vel, &vel) > 90000.0f) {
        PSVECNormalize(&vel, &vel);
        vel.x *= 300.0f;
        vel.y *= 300.0f;
        vel.z *= 300.0f;
    }
    mtx.pos.x += vel.x * 0.0166833512f;
    mtx.pos.y += vel.y * 0.0166833512f;
    mtx.pos.z += vel.z * 0.0166833512f;
    MulVec(&aaccel, &torque, &worldInvI);
    avel.x += aaccel.x * 0.0166833512f;
    avel.y += aaccel.y * 0.0166833512f;
    avel.z += aaccel.z * 0.0166833512f;
    if (PSVECDotProduct(&avel, &avel) > 100.0f) {
        PSVECNormalize(&avel, &avel);
        avel.x *= 10.0f;
        avel.y *= 10.0f;
        avel.z *= 10.0f;
    }
    {
        RMtx r;
        f32 ang;
        dr.x = avel.x * 0.0166833512f;
        dr.y = avel.y * 0.0166833512f;
        dr.z = avel.z * 0.0166833512f;
        ang = PSVECMag(&dr);
        if (ang > 0.0001f) {
            f32 inv = 1.0f / ang;
            f32 ax = inv * dr.x;
            f32 ay = inv * dr.y;
            f32 az = inv * dr.z;
            f32 s = sin(ang);
            f32 co = cos(ang);
            f32 t = 1.0f - co;
            f32 tx = t * ax;
            f32 ty = t * ay;
            r.rot.m[0][0] = co + t * (ax * ax);
            r.rot.m[0][1] = ay * tx + s * az;
            r.rot.m[0][2] = az * tx - s * ay;
            r.rot.m[1][0] = ay * tx - s * az;
            r.rot.m[1][1] = co + t * (ay * ay);
            r.rot.m[1][2] = az * ty + s * ax;
            r.rot.m[2][0] = az * tx + s * ay;
            r.rot.m[2][1] = az * ty - s * ax;
            r.rot.m[2][2] = co + t * (az * az);
        } else {
            r.rot.m[0][0] = 1.0f;
            r.rot.m[0][1] = 0.0f;
            r.rot.m[0][2] = 0.0f;
            r.rot.m[1][0] = 0.0f;
            r.rot.m[1][1] = 1.0f;
            r.rot.m[1][2] = 0.0f;
            r.rot.m[2][0] = 0.0f;
            r.rot.m[2][1] = 0.0f;
            r.rot.m[2][2] = 1.0f;
        }
        {
            RMat33* rr = &mtx.rot;
            Mul(rr, rr, &r.rot);
        }
    }
    {
        RMat33 tmp;
        RMat33 rt = mtx.rot;
        rt.Transpose();
        {
            RMat33* ii = &invI;
            Mul(&tmp, &rt, ii);
        }
        {
            RMat33* wi = &worldInvI;
            RMat33* rr = &mtx.rot;
            Mul(wi, &tmp, rr);
        }
    }
    force.x = 0.0f;
    force.y = 0.0f;
    force.z = 0.0f;
    torque.x = 0.0f;
    torque.y = 0.0f;
    torque.z = 0.0f;
    Xform2();
    UpdateCell();
}

void RigidBody::OnImpulses(RVec3* f, RVec3* t) {}

void RigidBody::OnImpulse(const RVec3* v, const RVec3* at, int kind, void* c, void* by) {}

extern "C" void fn_8017C7C4(void) {}

void RigidBody::Flush(RVec3* f, RVec3* t) {
    if ((nAdded > 0 ? sub[0] : 0) && b1E9 && b1E8) {
        (nAdded > 0 ? sub[0] : 0)->Wake();
    }
    if (!((f32)__fabs(impW) <= lbl_80558DC4[0])) {
        int i;
        f32 inv = 1.0f / impW;
        impF.x *= inv;
        impF.y *= inv;
        impF.z *= inv;
        impT.x *= inv;
        impT.y *= inv;
        impT.z *= inv;
        force.x += impF.x * 59.94f;
        force.y += impF.y * 59.94f;
        force.z += impF.z * 59.94f;
        torque.x += impT.x * 59.94f;
        torque.y += impT.y * 59.94f;
        torque.z += impT.z * 59.94f;
        for (i = 0; i < nImp; i++) {
            ImpRec* p = imps[i];
            f32 k = 1.0f / impW;
            p->v.x *= k;
            p->v.y *= k;
            p->v.z *= k;
            if (p->obj) {
                p->obj->Notify(&p->v, &p->at, p->kind, p->c, p->by);
            } else {
                OnImpulse(&p->v, &p->at, p->kind, p->c, p->by);
            }
        }
        OnImpulses(&impF, &impT);
        impF.x = 0.0f;
        impF.y = 0.0f;
        impF.z = 0.0f;
        impT.x = 0.0f;
        impT.y = 0.0f;
        impT.z = 0.0f;
    }
    nImp = 0;
    impW = 0.0f;
    *f = force;
    *t = torque;
    force.x = 0.0f;
    force.y = 0.0f;
    force.z = 0.0f;
    torque.x = 0.0f;
    torque.y = 0.0f;
    torque.z = 0.0f;
    {
        RMat33 tmp;
        RMat33 rt = mtx.rot;
        rt.Transpose();
        {
            RMat33* ii = &invI;
            Mul(&tmp, &rt, ii);
        }
        {
            RMat33* wi = &worldInvI;
            RMat33* rr = &mtx.rot;
            Mul(wi, &tmp, rr);
        }
    }
    Xform2();
    UpdateCell();
}

void RigidBody::SetState(const RMtx* m, const RVec3* v, const RVec3* w) {
    mtx = *m;
    Xform();
    vel = *v;
    avel = *w;
}

void RigidBody::UpdateCell(void) {
    void* c = fn_801B60F4(&mtx.pos, radius);
    if (c != cell) {
        if (cell) {
            fn_801B6428(cell, this);
        }
        cell = c;
        if (c) {
            fn_801B63E0(c, this);
        }
    }
}

extern "C" void fn_8017D140(u8* dst, const u8* src, int n) {
    while (--n >= 0) {
        *dst++ = *src++;
    }
}

void RigidBody::vf30(f32 t, RPacket* a, RPacket* b, s32* na, s32* nb) {
    RVec3 va;
    RVec3 vb;
    nImp = 0;
    Phob::vf30(t, a, b, na, nb);
    fn_8017D140((u8*)&va, (u8*)&a->vel, 12);
    fn_8017D140((u8*)&vb, (u8*)&b->vel, 12);
    SetV(&vel, Lerp(va.x, vb.x, t), Lerp(va.y, vb.y, t), Lerp(va.z, vb.z, t));
    accel.x = vb.x - va.x;
    accel.y = vb.y - va.y;
    accel.z = vb.z - va.z;
    accel.x *= 9.98999882f;
    accel.y *= 9.98999882f;
    accel.z *= 9.98999882f;
    *nb = 0x20;
    *na = 0x20;
}

s32 RigidBody::vf34(RPacket* p) {
    Phob::vf34(p);
    p->vel = vel;
    return 0x20;
}

void RigidBody::OnHit(const RVec3* f, const RVec3* at, int kind, void* c) { AddForceAt(f, at); }

extern "C" void fn_8017D2CC(int n) {
    lbl_8055E48C = n;
    lbl_8055E488 = 0;
    lbl_8055E490 = new ("ext_imp_tab") ImpRec[n];
}

ImpRec::ImpRec(void) {}

extern "C" void fn_8017D334(void) {
    lbl_8055E488 = 0;
    lbl_8055E48C = 0;
    delete[] lbl_8055E490;
}
