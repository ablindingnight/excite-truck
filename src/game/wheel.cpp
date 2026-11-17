// Loose wheels (0x801AFC28-0x801B06C8): up to 16 rigid spheres ("wheel-volume") that a crashing car throws
// off. Each spawn takes the next wheel of the pool, places it at the car wheel's pose and velocity and gives it
// that wheel's radius; a wheel is live for 40 seconds (it collides after the first 0.3), keeps rolling upright
// with a small torque, and is parked below the world (y -500) when idle or off the ground. Also packs its
// pose into the render buffer and its state into replay packets. Names are guesses apart from the string.
#include "game/common.h"
#include "game/lerpmtx.h"
#include "game/wheel.h"
#include "game/carbody.h"
#include <revolution/MTX/vec.h>
#include "game/collshape.h"
#include "game/rvec.h"
#include "game/rmtx.h"
#include "game/collvol.h"
#include "game/rigid.h"
#include "game/shapes.h"
#include "game/mathutil.h"

extern "C" {
int fn_801E9E30(s16* h, const void* pos);  // still on the ground
}

void* operator new(size_t size, const char* tag);

extern f32 lbl_8055E3C8;    // time
extern f32 lbl_8055E460;    // tick alpha
extern f32 __float_epsilon[];  // FLT_EPSILON

// GVec3: valvec3.h's ValVec3.
// The ground lookup's copy of a position (copied as raw words).
struct GMtx3 {
    s32 v[3];
};

struct RPacket;
struct BodyDesc;
class CollVolume;


struct WheelDesc {
    u8 pad0[0x18];
    f32 radius;  // offset 0x18
    f32 stiff;   // offset 0x1C
    f32 stiffK;  // offset 0x20
    f32 skin;    // offset 0x24
};

// The wheel's render state, written at its offset in the frame buffer.
struct WheelPose {
    RMtx mtx;   // offset 0x0
    s16 hint;   // offset 0x30
    s32 car;    // offset 0x34
    s32 index;  // offset 0x38
    f32 spin;   // offset 0x3C
    u8 live;    // offset 0x40
};

extern "C" CarBody* fn_8017960C(int car);

ModuleTag lbl_8055E648;
s32 lbl_8055E650;  // wheels in the pool
s32 lbl_8055E654;  // next wheel to throw
Wheel* lbl_80435BA8[16];

extern "C" void fn_801B04B0(Wheel* w, const RMtx* m, const RVec3* v, const RVec3* av, const s16* hint);

extern "C" {

void fn_801AFC28(void) {
    lbl_8055E650 = 0;
    lbl_8055E654 = 0;
}

void fn_801AFC38(void) { lbl_8055E650 = 0; }

void fn_801AFC44(int car, int index, const RMtx* m, const RVec3* v, const RVec3* av) {
    if (lbl_8055E650 >= 1) {
        Wheel* w = lbl_80435BA8[lbl_8055E654];
        CarBody* c = fn_8017960C(car);
        fn_801B04B0(w, m, v, av, &c->hint[0]);
        f32 now = lbl_8055E3C8;
        w->car = car;
        w->index = index;
        w->time = now;
        w->SetActive(1);
        SphereShape* s = (SphereShape*)w->Shape();
        f32 radiusv = c->wheels[index].v3C;
        s->radius = radiusv;
        if (++lbl_8055E654 >= lbl_8055E650) {
            lbl_8055E654 = 0;
        }
    }
}

}  // extern "C"

Wheel::Wheel(void) {
    index = 0;
    car = 0;
    time = 0.0f;
}

Wheel::~Wheel(void) {}

void Wheel::Init(BodyDesc* bd) {
    WheelDesc* d = (WheelDesc*)bd;
    SphereShape* s;
    RigidBody::Init(bd);
    f32 kk = d->stiffK;
    f32 r = 0.0254f * d->radius;
    f32 k = 4.45f * d->stiff / 0.0254f;
    f32 stiff = k * (Sq(kk));
    RVec3 c(0.0f, 0.0f, 0.0f);
    f32 skin = d->skin;
    mtx.pos = RVec3(0.0f, -200.0f, 0.0f);
    CollVolume::Transform();
    s = new ("wheel-volume") SphereShape(this, &mtx, stiff, skin, r, c);
    AddShape(s);
    b1E8 = 1;
    time = 0.0f;
    lbl_80435BA8[lbl_8055E650++] = this;
}


static inline void LerpV(RVec3* d, const RVec3* a, const RVec3* b, f32 t) {
    f32 x = Lerp(a->x, b->x, t);
    f32 y = Lerp(a->y, b->y, t);
    f32 z = Lerp(a->z, b->z, t);
    d->x = x;
    d->y = y;
    d->z = z;
}

static inline RVec3 LerpV(const RVec3& a, const RVec3& b, f32 t) {
    return RVec3(Lerp(a.x, b.x, t), Lerp(a.y, b.y, t), Lerp(a.z, b.z, t));
}


void Wheel::vf28(u8* buf) {
    WheelPose* p = (WheelPose*)(buf + a8);
    if ((f32)__fabs(lbl_8055E460 - 1.0f) <= __float_epsilon[0]) {
        p->mtx = world;
    } else {
        RMtx m;
        LerpMtx(&m, &prev, &world, lbl_8055E460);
        p->mtx = m;
    }
    p->hint = hint[0];
    p = (WheelPose*)(buf + a8);
    p->car = car;
    p->index = index;
    p->spin = PSVECDotProduct((const Vec*)&avel, (const Vec*)&mtx.rot.x);
    p->live = live;
}

void Wheel::vf20(void) {
    CollVolume::vf20();
    f32 now = lbl_8055E3C8;
    live = now > time && now < time + 40.0f;
    if (mtx.pos.y <= -499.5f) {
        live = 0;
    }
    SetActive(live && now >= time + 0.3f);
}

void Wheel::Reset(void) {
    RigidBody::Reset();
    time = 0.0f;
    live = 0;
    mtx.pos = RVec3(0.0f, -500.0f, 0.0f);
    CollVolume::Transform();
    SetActive(1);
    CollShape* s = Shape();
    if (s) {
        s->vf28();  // slot 0x28 (wakes the shape)
        ((SphereShape*)s)->radius = 1.0f;
    }
    lbl_8055E654 = 0;
    SetActive(0);
}

void Wheel::Update(void) {
    if (live) {
        RigidBody::Update();
        if (mtx.pos.y < -500.0f) {
            mtx.pos.y = -500.0f;
        }
        avel.x *= 0.9999666f;
        avel.y *= 0.9999666f;
        avel.z *= 0.9999666f;
        vel.x *= 0.99966633f;
        vel.y *= 0.99966633f;
        vel.z *= 0.99966633f;
        f32 spin = PSVECMag((const Vec*)&avel);
        int park = 1;
        if (spin > 0.01f) {
            RVec3 t;
            ValVec3 pos;
            f32 lean = mtx.rot.x.y;
            RVec3 up(0.0f, 1.0f, 0.0f);
            PSVECCrossProduct((const Vec*)&mtx.rot.x, (const Vec*)&up, (Vec*)&t);
            lean = 13.5725f * lean;
            f32 len = PSVECMag((const Vec*)&t);
            f32 k = lean / ((1.0f > len) ? 1.0f : len);
            t.x *= k;
            t.y *= k;
            t.z *= k;
            AddTorque(&t);
            RVec3 t2(0.0f, 100.0f * (PSVECDotProduct((const Vec*)&mtx.rot.x, (const Vec*)&vel) / spin), 0.0f);
            AddTorque(&t2);
            *(GMtx3*)&pos = *(const GMtx3*)&mtx.pos;
            if (fn_801E9E30(hint, &pos)) {
                park = 0;
            }
        }
        if (park) {
            mtx.pos.y = -500.0f;
        }
    } else {
        nImp = 0;
    }
}

extern "C" void fn_801B04B0(Wheel* w, const RMtx* m, const RVec3* v, const RVec3* av, const s16* hint) {
    w->mtx = *m;
    w->CollVolume::Transform();
    w->vel = *v;
    w->avel = *av;
    w->hint[0] = *hint;
}

s32 Wheel::vf2C(RPacket* p) {
    s32 n = 1;
    if (*(u8*)p) {
        n = 0x21;
    }
    return n;
}

s32 Wheel::vf34(RPacket* p) {
    u8* b = (u8*)p;
    b[0] = live;
    if (live) {
        return RigidBody::vf34((RPacket*)(b + 1)) + 1;
    }
    return 1;
}

void Wheel::vf30(f32 t, RPacket* a, const RPacket* b, s32* na, s32* __restrict nb) {
    *na = vf2C(a);
    *nb = vf2C((RPacket*)b);
    if (*(u8*)a && *(u8*)b) {
        s32 sa;
        s32 sb;
        live = 1;
        RigidBody::vf30(t, (RPacket*)((u8*)a + 1), (const RPacket*)((const u8*)b + 1), &sa, &sb);
    } else {
        live = 0;
        mtx.pos.y = -500.5f;
        vel.x = 0.0f;
        vel.y = 0.0f;
        vel.z = 0.0f;
    }
}
