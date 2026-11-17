// Rigid bodies (0x8017B0B0-0x8017D378): a collision volume with mass, an inertia tensor, linear and angular
// velocity, accumulated force and torque, and a list of pending impulses (from a pool of 0x28-byte records,
// "ext_imp_tab") that are averaged and applied once per tick, with the hit object notified. Also packs and
// unpacks the velocity into the state packet and keeps the body in its collision-grid cell. Names are guesses
// apart from the strings.
#include "game/common.h"
#include <revolution/MTX/vec.h>
#include "game/rvec.h"
#include "game/corner.h"
#include "game/rmtx.h"
#include "game/collshape.h"
#include "game/collvol.h"
#include "game/rigid.h"
#include "game/mathutil.h"

extern "C" {
void* memset(void* dst, int c, u32 n);
}  // extern "C"
f32 Dot(float, float, float, float, float, float);
extern "C" {
void* fn_801B60F4(const void* pos, f32 r);                            // grid cell at
void fn_801B6428(void* cell, void* body);                             // cell remove
void fn_801B63E0(void* cell, void* body);                             // cell add
double sin(double x);
double cos(double x);
}

void* operator new[](size_t size, const char* tag);

extern f32 __float_epsilon[];  // FLT_EPSILON


static inline void Mul(RMat33* out, const RMat33* l, const RMat33* r) {
    if (out == l || out == r) {
        f32 m00 = Dot(r->x.x, l->x.x, r->y.x, l->x.y, r->z.x, l->x.z);
        f32 m10 = Dot(r->x.x, l->y.x, r->y.x, l->y.y, r->z.x, l->y.z);
        f32 m20 = Dot(r->x.x, l->z.x, r->y.x, l->z.y, r->z.x, l->z.z);
        f32 m01 = Dot(r->x.y, l->x.x, r->y.y, l->x.y, r->z.y, l->x.z);
        f32 m11 = Dot(r->x.y, l->y.x, r->y.y, l->y.y, r->z.y, l->y.z);
        f32 m21 = Dot(r->x.y, l->z.x, r->y.y, l->z.y, r->z.y, l->z.z);
        f32 m02 = Dot(r->x.z, l->x.x, r->y.z, l->x.y, r->z.z, l->x.z);
        f32 m12 = Dot(r->x.z, l->y.x, r->y.z, l->y.y, r->z.z, l->y.z);
        f32 m22 = Dot(r->x.z, l->z.x, r->y.z, l->z.y, r->z.z, l->z.z);
        out->x.x = m00;
        out->y.x = m10;
        out->z.x = m20;
        out->x.y = m01;
        out->y.y = m11;
        out->z.y = m21;
        out->x.z = m02;
        out->y.z = m12;
        out->z.z = m22;
    } else {
        out->x.x = Dot(r->x.x, l->x.x, r->y.x, l->x.y, r->z.x, l->x.z);
        out->y.x = Dot(r->x.x, l->y.x, r->y.x, l->y.y, r->z.x, l->y.z);
        out->z.x = Dot(r->x.x, l->z.x, r->y.x, l->z.y, r->z.x, l->z.z);
        out->x.y = Dot(r->x.y, l->x.x, r->y.y, l->x.y, r->z.y, l->x.z);
        out->y.y = Dot(r->x.y, l->y.x, r->y.y, l->y.y, r->z.y, l->y.z);
        out->z.y = Dot(r->x.y, l->z.x, r->y.y, l->z.y, r->z.y, l->z.z);
        out->x.z = Dot(r->x.z, l->x.x, r->y.z, l->x.y, r->z.z, l->x.z);
        out->y.z = Dot(r->x.z, l->y.x, r->y.z, l->y.y, r->z.z, l->y.z);
        out->z.z = Dot(r->x.z, l->z.x, r->y.z, l->z.y, r->z.z, l->z.z);
    }
}

struct RPacket {
    u8 pad0[0x14];
    RVec3 vel;  // offset 0x14
};

// A contact point in body space (corners.cpp); the body keeps its corners in a list.
class RigidBody;
struct ImpRec {
    ImpRec(void);
    CollShape* obj;  // offset 0x0: the pushing shape (notified through its slot 0x24)
    void* by;    // offset 0x4
    RVec3 v;     // offset 0x8
    RVec3 at;    // offset 0x14
    int kind;    // offset 0x20
    void* c;     // offset 0x24
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
    CollVolume::Init(d);
    contacts = 0;
    SetMass(((RigidDesc*)d)->mass, ((RigidDesc*)d)->ix, ((RigidDesc*)d)->iy, ((RigidDesc*)d)->iz);
    cell = 0;
    ResetState();
}

void RigidBody::Reset(void) {
    CollVolume::Reset();
    ResetState();
}

void RigidBody::ResetState(void) {
    Corner* c;
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
        c->Stop();
    }
}

static inline f32 LbToKg(f32 x) { return 0.4545f * x; }
static inline f32 FtToM(f32 x) { return 0.305f * x; }

void RigidBody::SetMass(f32 m, f32 ix, f32 iy, f32 iz) {
    mass = LbToKg(m);
    inertia.x = FtToM(FtToM(LbToKg(ix)));
    inertia.y = FtToM(FtToM(LbToKg(iy)));
    inertia.z = FtToM(FtToM(LbToKg(iz)));
    invInertia.x = 1.0f / inertia.x;
    invMass = 1.0f / mass;
    invInertia.y = 1.0f / inertia.y;
    invInertia.z = 1.0f / inertia.z;
    invI.x.x = invInertia.x;
    invI.x.y = 0.0f;
    invI.x.z = 0.0f;
    invI.y.x = 0.0f;
    invI.y.y = invInertia.y;
    invI.y.z = 0.0f;
    invI.z.x = 0.0f;
    invI.z.y = 0.0f;
    invI.z.z = invInertia.z;
    worldInvI = invI;
}

void RigidBody::AddForceAt(const RVec3* f, const RVec3* at) {
    if (!((f32)__fabs(PSVECDotProduct((const Vec*)f, (const Vec*)f)) <= __float_epsilon[0])) {
        RVec3 r;
        RVec3 t;
        r.x = at->x - mtx.pos.x;
        r.y = at->y - mtx.pos.y;
        r.z = at->z - mtx.pos.z;
        PSVECCrossProduct((const Vec*)&r, (const Vec*)f, (Vec*)&t);
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
    PSVECCrossProduct((const Vec*)&r, (const Vec*)v, (Vec*)&t);
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

void RigidBody::Push(CollShape* obj, void* by, const RVec3* v, const RVec3* at, int kind, const RVec3* at2, void* c,
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
    PSVECCrossProduct((const Vec*)&avel, (const Vec*)&r, (Vec*)out);
    out->x += vel.x;
    out->y += vel.y;
    out->z += vel.z;
}

void RigidBody::VelAt2(RVec3* out, const RVec3* p) {
    RVec3 r;
    r.x = p->x - mtx.pos.x;
    r.y = p->y - mtx.pos.y;
    r.z = p->z - mtx.pos.z;
    PSVECCrossProduct((const Vec*)&aaccel, (const Vec*)&r, (Vec*)out);
    out->x += accel.x;
    out->y += accel.y;
    out->z += accel.z;
}



static inline void MulVec(RVec3* out, const RVec3* in, const RMat33* m) {
    if (out == in) {
        f32 x = Dot3(in->x, m->x.x, in->y, m->y.x, in->z, m->z.x);
        f32 y = Dot3(in->x, m->x.y, in->y, m->y.y, in->z, m->z.y);
        f32 z = Dot3(in->x, m->x.z, in->y, m->y.z, in->z, m->z.z);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, m->x.x, in->y, m->y.x, in->z, m->z.x);
        out->y = Dot3(in->x, m->x.y, in->y, m->y.y, in->z, m->z.y);
        out->z = Dot3(in->x, m->x.z, in->y, m->y.z, in->z, m->z.z);
    }
}


/* Helper name unproven: the body rotation matrix. */
inline RMat33* RigidBody::Rot(void) {
    return &mtx.rot;
}

/* Helper name unproven: the inverse inertia matrix. */
inline RMat33* RigidBody::InvInertia(void) {
    return &invI;
}

// Helper names unproven: the scale arrives as a parameter, so it stays the right operand of each multiply.
static inline void AddScaled(RVec3* o, const RVec3* v, f32 s) {
    o->x += v->x * s;
    o->y += v->y * s;
    o->z += v->z * s;
}
static inline void Scaled(RVec3* o, const RVec3* v, f32 s) {
    o->x = v->x * s;
    o->y = v->y * s;
    o->z = v->z * s;
}
/* Helper name unproven: hands each pending impulse, scaled by the total weight, to its object. */
inline void RigidBody::NotifyAll(void) {
    int i;
    for (i = 0; i < nImp; i++) {
        ImpRec* p = imps[i];
        f32 k = 1.0f / impW;
        p->v.x *= k;
        p->v.y *= k;
        p->v.z *= k;
        if (p->obj) {
            p->obj->vf24(&p->v, &p->at, p->kind, p->c, p->by);
        } else {
            OnImpulse(&p->v, &p->at, p->kind, p->c, p->by);
        }
    }
}

void RigidBody::Update(void) {
    int i;
    ImpRec* p;
    f32 az;
    f32 ax;
    RMat33* wi;
    f32 k;
    f32 ay;
    f32 ang;
    RMat33* ii;
    f32 tx;
    f32 co;
    Corner* c;
    f32 t;
    int j;
    f32 yy;
    f32 ty;
    f32 zz;
    RMat33* rr;
    f32 inv;
    f32 s;
    f32 xx;
    ImpRec* q;
    RVec3 g;
    RVec3 dr;

    g.Set(0.0f, 9.81f * -mass, 0.0f);
    force.x += g.x;
    force.y += g.y;
    force.z += g.z;
    for (c = contacts; c; c = c->next) {
        c->Hit(this);
    }
    if ((nAdded > 0 ? sub[0] : 0) && b1E9 && b1E8) {
        (nAdded > 0 ? sub[0] : 0)->vf14();
    }
    if (!(FAbs(impW) <= __float_epsilon[0])) {

        inv = 1.0f / impW;
        impF.x *= inv;
        impF.y *= inv;
        impF.z *= inv;
        impT.x *= inv;
        impT.y *= inv;
        impT.z *= inv;
        AddScaled(&force, &impF, 59.94f);
        AddScaled(&torque, &impT, 59.94f);
        for (i = 0; i < nImp; i++) {
            p = imps[i];
            if (p->by) {

                for (j = i + 1; j < nImp; j++) {
                    q = imps[j];
                    if (p->by == q->by) {
                        RVec3 mid;
                        p->v.x += q->v.x;
                        p->v.y += q->v.y;
                        p->v.z += q->v.z;
                        mid.Set(Lerp(p->at.x, q->at.x, 0.5f), Lerp(p->at.y, q->at.y, 0.5f), Lerp(p->at.z, q->at.z, 0.5f));
                        p->at = mid;
                        imps[j] = imps[--nImp];
                    }
                }
            }
        }
        NotifyAll();
        OnImpulses(&impF, &impT);
        impF.x = 0.0f;
        impF.y = 0.0f;
        impF.z = 0.0f;
        impT.x = 0.0f;
        impT.y = 0.0f;
        impT.z = 0.0f;
    }
    impW = 0.0f;
    nImp = 0;
    AddScaled(&vel, &force, 0.0166833512f * invMass);
    Scaled(&accel, &force, invMass);
    if (PSVECDotProduct((const Vec*)&vel, (const Vec*)&vel) > 90000.0f) {
        PSVECNormalize((const Vec*)&vel, (Vec*)&vel);
        vel.x *= 300.0f;
        vel.y *= 300.0f;
        vel.z *= 300.0f;
    }
    AddScaled(&mtx.pos, &vel, 0.0166833512f);
    MulVec(&aaccel, &torque, &worldInvI);
    AddScaled(&avel, &aaccel, 0.0166833512f);
    if (PSVECDotProduct((const Vec*)&avel, (const Vec*)&avel) > 100.0f) {
        PSVECNormalize((const Vec*)&avel, (Vec*)&avel);
        avel.x *= 10.0f;
        avel.y *= 10.0f;
        avel.z *= 10.0f;
    }
    {
        RMtx r;

        Scaled(&dr, &avel, 0.0166833512f);
        ang = PSVECMag((const Vec*)&dr);
        if (ang > 0.0001f) {
            inv = 1.0f / ang;
            ax = inv * dr.x;
            ay = inv * dr.y;
            az = inv * dr.z;

            s = sin(ang);

            co = cos(ang);
            yy = ay * ay;
            xx = ax * ax;
            t = 1.0f - co;
            zz = az * az;
            tx = t * ax;
            ty = t * ay;
            r.rot.x.x = co + t * xx;
            r.rot.x.y = (s * az) + (ay * tx);
            r.rot.x.z = az * tx - s * ay;
            r.rot.y.x = (ay * tx) - (s * az);
            r.rot.y.y = co + t * yy;
            r.rot.z.x = az * tx + s * ay;
            r.rot.y.z = az * ty + s * ax;
            r.rot.z.y = az * ty - s * ax;
            r.rot.z.z = co + t * zz;
        } else {
            r.rot.x.x = 1.0f;
            r.rot.x.y = 0.0f;
            r.rot.x.z = 0.0f;
            r.rot.y.x = 0.0f;
            r.rot.y.y = 1.0f;
            r.rot.y.z = 0.0f;
            r.rot.z.x = 0.0f;
            r.rot.z.y = 0.0f;
            r.rot.z.z = 1.0f;
        }
        {
            rr = Rot();
            RMat33* rotation = &r.rot;
            Mul(rr, rr, rotation);
        }
    }
    {
        RMat33 tmp;
        RMat33 rt = mtx.rot;
        rt.Transpose();
        {
            ii = InvInertia();
            RMat33* rtp = &rt;
            RMat33* tp = &tmp;
            Mul(tp, rtp, ii);
        }
        {
            wi = &worldInvI;
            rr = &mtx.rot;
            Mul(wi, &tmp, rr);
        }
    }
    force.x = 0.0f;
    force.y = 0.0f;
    force.z = 0.0f;
    torque.x = 0.0f;
    torque.y = 0.0f;
    torque.z = 0.0f;
    CollVolume::Update();
    UpdateCell();
}

void RigidBody::OnImpulses(RVec3* f, RVec3* t) {}

void RigidBody::OnImpulse(const RVec3* v, const RVec3* at, int kind, void* c, void* by) {}

// CollShape::vf14 (empty; slot 0x14 of the sphcoll shape tables), by its linker name: CollShape is not declared here.
extern "C" void vf14__9CollShapeFv(void) {}

void RigidBody::Flush(RVec3* f, RVec3* t) {
    RMat33* ii;
    RMat33* wi;
    RMat33* rr;
    if ((nAdded > 0 ? sub[0] : 0) && b1E9 && b1E8) {
        (nAdded > 0 ? sub[0] : 0)->vf14();
    }
    if (!(FAbs(impW) <= __float_epsilon[0])) {
        int i;
        f32 inv = 1.0f / impW;
        impF.x *= inv;
        impF.y *= inv;
        impF.z *= inv;
        impT.x *= inv;
        impT.y *= inv;
        impT.z *= inv;
        AddScaled(&force, &impF, 59.94f);
        AddScaled(&torque, &impT, 59.94f);
        NotifyAll();
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
            ii = InvInertia();
            RMat33* rtp = &rt;
            RMat33* tp = &tmp;
            Mul(tp, rtp, ii);
        }
        {
            wi = &worldInvI;
            rr = &mtx.rot;
            Mul(wi, &tmp, rr);
        }
    }
    CollVolume::Update();
    UpdateCell();
}

void RigidBody::SetState(const RMtx* m, const RVec3* v, const RVec3* w) {
    mtx = *m;
    Transform();
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

void RigidBody::vf30(f32 t, RPacket* a, const RPacket* b, s32* na, s32* nb) {
    RVec3 va;
    RVec3 vb;
    nImp = 0;
    CollVolume::vf30(t, a, b, na, nb);
    fn_8017D140((u8*)&va, (u8*)&a->vel, 12);
    fn_8017D140((u8*)&vb, (u8*)&b->vel, 12);
    vel.Set(Lerp(va.x, vb.x, t), Lerp(va.y, vb.y, t), Lerp(va.z, vb.z, t));
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
    CollVolume::vf34(p);
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
