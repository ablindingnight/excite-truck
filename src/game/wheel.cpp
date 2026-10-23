// Loose wheels (0x801AFC28-0x801B06C8): up to 16 rigid spheres ("wheel-volume") that a crashing car throws
// off. Each spawn takes the next wheel of the pool, places it at the car wheel's pose and velocity and gives it
// that wheel's radius; a wheel is live for 40 seconds (it collides after the first 0.3), keeps rolling upright
// with a small torque, and is parked below the world (y -500) when idle or off the ground. Also packs its
// pose into the render buffer and its state into replay packets. Names are guesses apart from the string.
#include "game/common.h"

extern "C" {
void PSVECCrossProduct(const void* a, const void* b, void* out);
f32 PSVECDotProduct(const void* a, const void* b);
f32 PSVECMag(const void* v);
int fn_801E9E30(s16* h, const void* pos);  // still on the ground
}

void* operator new(size_t size, const char* tag);

extern f32 lbl_8055E3C8;    // time
extern f32 lbl_8055E460;    // tick alpha
extern f32 lbl_80558DC4[];  // FLT_EPSILON

struct RVec3 {
    RVec3(void) {}
    RVec3(f32 a, f32 b, f32 c) {
        x = a;
        y = b;
        z = c;
    }
    f32 x;
    f32 y;
    f32 z;
};

struct GVec3 {
    f32 x;
    f32 y;
    f32 z;
};

// The ground lookup's copy of a position (copied as raw words).
struct GMtx3 {
    s32 v[3];
};

struct RMtx {
    RVec3 rot[3];  // offset 0x0
    RVec3 pos;     // offset 0x24
};

struct RPacket;
struct BodyDesc;
class CollVolume;

extern "C" void fn_8019EECC(CollVolume* v, RVec3* c);

class CollShape {
public:
    CollShape(CollVolume* o, const RMtx* m, u32 t, f32 k, f32 s) {
        mtx = m;
        owner = o;
        a8 = 0;
        type = t;
        stiff = k;
        skin = s;
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
    virtual void Wake(void);  // 0x28
    CollVolume* owner;  // offset 0x4
    s32 a8;             // offset 0x8
    const RMtx* mtx;    // offset 0xC
    f32 stiff;          // offset 0x10
    f32 skin;           // offset 0x14
    u32 type;           // offset 0x18
    s32 a1C;            // offset 0x1C
    s32 a20;            // offset 0x20
};

class SphereShape : public CollShape {
public:
    SphereShape(CollVolume* o, const RMtx* m, f32 k, f32 s, f32 r, const RVec3& c)
        : CollShape(o, m, 0x53504852, k, s) {
        radius = r;
        center.x = c.x;
        center.y = c.y;
        center.z = c.z;
        f34 = 0.0f;
        f38 = 0.0f;
        f3C = 0.0f;
        f40 = 0.0f;
        f44 = 0.0f;
        f48 = 0.0f;
        b4C = 0;
        f50 = 0.0f;
        if (o) {
            fn_8019EECC(o, &center);
        }
    }
    virtual ~SphereShape(void);
    f32 radius;    // offset 0x24
    RVec3 center;  // offset 0x28
    f32 f34;       // offset 0x34
    f32 f38;       // offset 0x38
    f32 f3C;       // offset 0x3C
    f32 f40;       // offset 0x40
    f32 f44;       // offset 0x44
    f32 f48;       // offset 0x48
    u8 b4C;        // offset 0x4C
    f32 f50;       // offset 0x50
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
    virtual void vf38(void);          // 0x38
    virtual void vf3C(void);          // 0x3C
    virtual int vf40(void);           // 0x40
    virtual void vf44(void);          // 0x44
    void Update(void);
    void AddShape(CollShape* s);
    void SetActive(int on) { nAdded = on ? nSub : 0; }
    CollShape* Shape(void) { return (nAdded > 0) ? sub[0] : 0; }
    s32 a4;             // offset 0x4
    s32 a8;             // offset 0x8
    void* desc;         // offset 0xC
    s32 type;           // offset 0x10
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
    virtual void vf30(f32 t, RPacket* a, RPacket* b, s32* na, s32* nb);
    virtual s32 vf34(RPacket* p);
    virtual void OnHit(void);       // 0x48
    virtual void OnImpulse(void);   // 0x4C
    virtual void OnImpulses(void);  // 0x50
    void Update(void);
    void AddTorque(const RVec3* t);
    u8 padD0[0x118 - 0xD0];
    RVec3 vel;   // offset 0x118
    RVec3 avel;  // offset 0x124
    u8 pad130[0x1E4 - 0x130];
    s32 nImp;  // offset 0x1E4
    u8 b1E8;   // offset 0x1E8
    u8 b1E9;   // offset 0x1E9
};

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

struct CarWheel {
    f32 radius;  // offset 0x0
    u8 pad4[0x70];
};

struct CarRec {
    u8 pad0[0x6C];
    s16 hint;  // offset 0x6C
    u8 pad6E[0x23E8 - 0x6E];
    CarWheel wheels[4];  // offset 0x23E8
};

extern "C" CarRec* fn_8017960C(int car);

class Wheel : public RigidBody {
public:
    Wheel(void);
    virtual void Init(BodyDesc* d);
    virtual ~Wheel(void);
    virtual void Reset(void);
    virtual void vf1C(void);
    virtual void vf20(void);
    virtual void vf28(u8* buf);
    virtual s32 vf2C(RPacket* p);
    virtual void vf30(f32 t, RPacket* a, RPacket* b, s32* na, s32* nb);
    virtual s32 vf34(RPacket* p);
    f32 time;   // offset 0x1EC
    s32 car;    // offset 0x1F0
    s32 index;  // offset 0x1F4
    u8 live;    // offset 0x1F8
};

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
        CarRec* c = fn_8017960C(car);
        fn_801B04B0(w, m, v, av, &c->hint);
        f32 now = lbl_8055E3C8;
        w->car = car;
        w->index = index;
        w->time = now;
        w->SetActive(1);
        SphereShape* s = (SphereShape*)w->Shape();
        s->radius = c->wheels[index].radius;
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
    RVec3 c(0.0f, 0.0f, 0.0f);
    f32 k = 4.45f * d->stiff / 0.0254f;
    f32 r = 0.0254f * d->radius;
    f32 skin = d->skin;
    f32 stiff = k * (kk * kk);
    mtx.pos = RVec3(0.0f, -200.0f, 0.0f);
    CollVolume::Update();
    s = new ("wheel-volume") SphereShape(this, &mtx, stiff, skin, r, c);
    AddShape(s);
    b1E8 = 1;
    time = 0.0f;
    lbl_80435BA8[lbl_8055E650++] = this;
}

static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

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

static inline void LerpMtx(RMtx* out, const RMtx* a, const RMtx* b, f32 t) {
    f32* d = (f32*)out;
    const f32* x = (const f32*)a;
    const f32* y = (const f32*)b;
    for (int i = 0; i < 12; i++) {
        d[i] = Lerp(x[i], y[i], t);
    }
}

void Wheel::vf28(u8* buf) {
    WheelPose* p = (WheelPose*)(buf + a8);
    f32 t = lbl_8055E460;
    if ((f32)__fabs(t - 1.0f) <= lbl_80558DC4[0]) {
        p->mtx = world;
    } else {
        RMtx m;
        RVec3* d = (RVec3*)&m;
        const RVec3* a = (const RVec3*)&prev;
        const RVec3* b = (const RVec3*)&world;
        for (int i = 0; i < 4; i++) {
            d[i].x = Lerp(a[i].x, b[i].x, t);
            d[i].y = Lerp(a[i].y, b[i].y, t);
            d[i].z = Lerp(a[i].z, b[i].z, t);
        }
        p->mtx = m;
    }
    p->hint = hint[0];
    p = (WheelPose*)(buf + a8);
    p->car = car;
    p->index = index;
    p->spin = PSVECDotProduct(&avel, &mtx.rot[0]);
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
    CollVolume::Update();
    SetActive(1);
    CollShape* s = Shape();
    if (s) {
        s->Wake();
        ((SphereShape*)s)->radius = 1.0f;
    }
    lbl_8055E654 = 0;
    SetActive(0);
}

void Wheel::vf1C(void) {
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
        f32 spin = PSVECMag(&avel);
        int park = 1;
        if (spin > 0.01f) {
            RVec3 t;
            GVec3 pos;
            f32 lean = mtx.rot[0].y;
            RVec3 up(0.0f, 1.0f, 0.0f);
            PSVECCrossProduct(&mtx.rot[0], &up, &t);
            lean = 13.5725f * lean;
            f32 len = PSVECMag(&t);
            f32 k = lean / ((1.0f > len) ? 1.0f : len);
            t.x *= k;
            t.y *= k;
            t.z *= k;
            AddTorque(&t);
            RVec3 t2(0.0f, 100.0f * (PSVECDotProduct(&mtx.rot[0], &vel) / spin), 0.0f);
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
    w->CollVolume::Update();
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

void Wheel::vf30(f32 t, RPacket* a, RPacket* b, s32* na, s32* nb) {
    *na = vf2C(a);
    *nb = vf2C(b);
    if (*(u8*)a && *(u8*)b) {
        s32 sa;
        s32 sb;
        live = 1;
        RigidBody::vf30(t, (RPacket*)((u8*)a + 1), (RPacket*)((u8*)b + 1), &sa, &sb);
    } else {
        live = 0;
        mtx.pos.y = -500.5f;
        vel.x = 0.0f;
        vel.y = 0.0f;
        vel.z = 0.0f;
    }
}
