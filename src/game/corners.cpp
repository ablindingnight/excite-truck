// Body corners (0x801ADB44-0x801AE59C): contact points on a rigid body that are pushed out of
// the ground with spring/friction impulses, allocated from a pooled free store. Names are
// guesses apart from the messages.
#include "game/common.h"
#include "macros.h"
#include <revolution/MTX.h>

struct RVec3 {
    f32 x;
    f32 y;
    f32 z;
};

struct RMat33 {
    f32 m[3][3];
};

struct RMtx {
    RMat33 rot;  // offset 0x0
    RVec3 pos;   // offset 0x24
};

class Phob {
public:
    virtual void Init(void* d);       // 0x8
    virtual ~Phob(void);              // 0xC
    virtual void Reset(void);         // 0x10
    virtual void vf14(void);          // 0x14
    virtual void vf18(void);          // 0x18
    virtual void Update(void);        // 0x1C
    virtual void vf20(void);          // 0x20
    virtual void Notify(const RVec3* v, const RVec3* at, int kind, void* c, void* by);  // 0x24
    virtual void vf28(u8* buf);       // 0x28
    virtual s32 vf2C(void);           // 0x2C
    virtual void vf30(void);          // 0x30
    virtual s32 vf34(void);           // 0x34
    virtual void vf38(void);          // 0x38
    virtual void vf3C(void);          // 0x3C
    virtual int vf40(void);           // 0x40
    virtual void vf44(void);          // 0x44
    s32 a4;          // offset 0x4
    s32 a8;          // offset 0x8
    void* desc;      // offset 0xC
    s32 type;        // offset 0x10
    void* sub[4];    // offset 0x14
    s32 nAdded;      // offset 0x24
    s32 nSub;        // offset 0x28
    f32 radius;      // offset 0x2C
    RVec3 offset;    // offset 0x30
    RMtx mtx;        // offset 0x3C
    s16 hint[2];     // offset 0x6C
};

class RigidBody : public Phob {
public:
    virtual void OnHit(const RVec3* f, const RVec3* at, int kind, void* c);  // 0x48
    void Push(Phob* obj, void* by, const RVec3* v, const RVec3* at, int kind, const RVec3* at2, void* c, f32 w);
    void VelAt(RVec3* out, const RVec3* p);
    u8 pad70[0xD4 - 0x70];
    f32 mass;  // offset 0xD4
    u8 padD8[0x148 - 0xD8];
    RMat33 worldInvI;  // offset 0x148
};

extern "C" {
int fn_801E9EC4(s16* hint, RVec3* hit, RVec3* n, u32* flags);  // ground contact under a point
f32 fn_8019DF88(f32 mass, f32 e, const RVec3* pos, const RVec3* vel, const RMat33* invI, const RVec3* at,
                const RVec3* n);  // contact impulse
void fn_802E30C8(const char* fmt, ...);
}

extern f32 lbl_80558DC4[];  // FLT_EPSILON
extern s32 lbl_8055E3C4;    // frame counter

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

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

static inline void Scale(RVec3* v, f32 s) {
    v->x = v->x * s;
    v->y = v->y * s;
    v->z = v->z * s;
}

static inline void AddScaled(RVec3* v, const RVec3* d, f32 s) {
    v->x = v->x + d->x * s;
    v->y = v->y + d->y * s;
    v->z = v->z + d->z * s;
}

// A contact point in body space.
struct Corner {
    Corner(void) {
        pos.x = 0.0f;
        pos.y = 0.0f;
        pos.z = 0.0f;
        spring = 1000.0f;
        friction = -1000.0f;
        damping = 0.0f;
        vel.x = 0.0f;
        vel.y = 0.0f;
        vel.z = 0.0f;
        unk0 = 0;
    }
    void Hit(RigidBody* b);
    void Stop(void);

    s32 unk0;      // offset 0x0
    RVec3 pos;     // offset 0x4
    f32 spring;    // offset 0x10
    f32 friction;  // offset 0x14
    f32 damping;   // offset 0x18
    RVec3 vel;     // offset 0x1C
};

struct CornerBlock {
    CornerBlock(void) {
        count = 0;
        next = 0;
    }
    CornerBlock* next;    // offset 0x0
    s32 count;            // offset 0x4
    Corner corners[64];   // offset 0x8
};

struct CornerPool {
    CornerPool(const char* n) {
        head = 0;
        name = n;
    }
    CornerBlock* head;  // offset 0x0
    const char* name;   // offset 0x4
};

void* operator new(size_t size, const char* name);

ModuleTag lbl_8055E5F8;
UnkStaticFFFF lbl_8055E600;
CornerPool* lbl_8055E604;

void Corner::Hit(RigidBody* b) {
    RVec3 hit;
    RVec3 n;
    RVec3 p;
    RVec3 d;
    RVec3 v;
    RVec3 vt;
    RVec3 imp;
    RVec3 fr;
    RVec3 ji;
    u32 flags;
    if (Dot3(pos.x, b->mtx.rot.m[0][1], pos.y, b->mtx.rot.m[1][1], pos.z, b->mtx.rot.m[2][1]) > 0.0f) {
        return;
    }
    MulVec(&p, &pos, &b->mtx.rot);
    p.x = p.x + b->mtx.pos.x;
    p.y = p.y + b->mtx.pos.y;
    p.z = p.z + b->mtx.pos.z;
    f32 depth = 0.0f;
    flags = 0;
    int found = fn_801E9EC4(b->hint, &hit, &n, &flags);
    if (found) {
        d.x = hit.x - p.x;
        d.y = hit.y - p.y;
        d.z = hit.z - p.z;
        depth = PSVECDotProduct((Vec*)&d, (Vec*)&n);
    }
    if (!found || depth > 0.15f) {
        n.x = p.x - b->mtx.pos.x;
        n.y = p.y - b->mtx.pos.y;
        n.z = p.z - b->mtx.pos.z;
        PSVECNormalize((Vec*)&n, (Vec*)&n);
        Scale(&n, -1.0f);
        depth = 0.2f;
    }
    if (depth > 0.0f) {
        b->VelAt(&v, &p);
        f32 vn = PSVECDotProduct((Vec*)&v, (Vec*)&n);
        vt.x = v.x + n.x * -vn;
        vt.y = v.y + n.y * -vn;
        vt.z = v.z + n.z * -vn;
        AddScaled(&vel, &vt, 0.016683363f);
        f32 mag = PSVECMag((Vec*)&vel);
        if (mag > 0.02f) {
            f32 s = 0.02f / mag;
            vel.x = vel.x * s;
            vel.y = vel.y * s;
            vel.z = vel.z * s;
        }
        f32 k;
        if ((f32)__fabs(damping) <= lbl_80558DC4[0]) {
            k = depth * spring * (vn < 0.0f ? 1.0f : 0.2f);
        } else {
            k = depth * spring - vn * damping;
            k = 0.0f > k ? 0.0f : k;
        }
        imp.x = n.x * k;
        imp.y = n.y * k;
        imp.z = n.z * k;
        f32 c = -3.0f * b->mass;
        fr.x = vt.x * c;
        fr.y = vt.y * c;
        fr.z = vt.z * c;
        f32 c2 = -30.0f * b->mass;
        fr.x = fr.x + vel.x * c2;
        fr.y = fr.y + vel.y * c2;
        fr.z = fr.z + vel.z * c2;
        f32 fm = PSVECMag((Vec*)&fr);
        f32 lim = k * friction;
        lim = MAX(lim, 4.45f);
        if (fm > lim) {
            f32 s = lim / fm;
            fr.x = fr.x * s;
            fr.y = fr.y * s;
            fr.z = fr.z * s;
        }
        imp.x = imp.x + fr.x;
        imp.y = imp.y + fr.y;
        imp.z = imp.z + fr.z;
        b->OnHit(&imp, &p, flags, 0);
        f32 j = fn_8019DF88(b->mass, 0.0f, &b->mtx.pos, &v, &b->worldInvI, &p, &n);
        if (j > 0.0f) {
            ji.x = n.x * j;
            ji.y = n.y * j;
            ji.z = n.z * j;
            Phob* by = (Phob*)(b->nAdded > 0 ? b->sub[0] : 0);
            f32 w = 0.001f > depth ? 0.001f : depth;
            b->Push(by, 0, &ji, &p, flags, 0, 0, w);
            if (b->type == 'PCAR') {
                fn_802E30C8("%08x: hit corner %f,%f,%f", lbl_8055E3C4, p.x, p.y, p.z);
            }
        }
    } else {
        vel.x = 0.0f;
        vel.y = 0.0f;
        vel.z = 0.0f;
    }
}

void Corner::Stop(void) {
    RVec3 zero;
    zero.x = 0.0f;
    zero.y = 0.0f;
    zero.z = 0.0f;
    vel = zero;
}

extern "C" {

Corner* fn_801AE180(void) {
    CornerPool* pool = lbl_8055E604;
    if (!pool->head || pool->head->count == 64) {
        CornerBlock* b = new (pool->name) CornerBlock;
        b->next = pool->head;
        pool->head = b;
    }
    CornerBlock* b = pool->head;
    return &b->corners[b->count++];
}

void fn_801AE4C4(void) { lbl_8055E604 = new ("corner-alloc") CornerPool("corners"); }

void fn_801AE50C(void) {
    CornerBlock* next;
    CornerPool* pool = lbl_8055E604;
    if (pool) {
        while (pool->head) {
            next = pool->head->next;
            delete pool->head;
            pool->head = next;
        }
        delete pool;
    }
    lbl_8055E604 = 0;
}

}  // extern "C"
