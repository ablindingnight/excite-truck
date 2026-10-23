// Trailers (0x801B8CC0-0x801BA088, "trlr_car"): a car body towed by the car with the same index, held
// by a hitch constraint between the "hitch" joints of the two models (or fixed offsets for the
// alternative low hitch). The trailer's yaw is damped towards the car's; if the hitch stays stretched
// for 5 seconds while the car is moving it is reglued behind the car. Names are guesses apart from
// the strings.
#include "game/common.h"

void* operator new(size_t size, const char* tag);

extern "C" {
void fn_802E30C8(const char* fmt, ...);  // debug print
const char* fn_802C7A10(int model);      // model name
void fn_802C77F0(int model, f32* bounds);
int fn_801E320C(int model, const char* name);  // has joint
struct Joint;
Joint* fn_801E3004(int model, const char* name, int flags);
void fn_801E3488(struct RVec3* out, const Joint* j);  // joint position
void fn_801E323C(Joint* j);
struct TFlags {
    u8 pad0[0x4];
    u32 flags;  // offset 0x4
};
TFlags* fn_801769CC(void);
void fn_801796CC(u8 type, const void* data, u32 n);
f32 PSVECDotProduct(const void* a, const void* b);
}

struct RVec3 {
    RVec3(void) {}
    RVec3(f32 x_, f32 y_, f32 z_) {
        x = x_;
        y = y_;
        z = z_;
    }
    f32 x;
    f32 y;
    f32 z;
};

struct TRot {
    RVec3 axis[3];
};

struct TMtx {
    union {
        RVec3 rot[3];  // offset 0x0
        TRot r;
    };
    RVec3 pos;     // offset 0x24
};

// The hitch constraint (a spring between two bodies, 0x8xxxx code).
struct Hitch {
    Hitch(void) {
        owner = 0;
        a0C = 0;
        stretch = 0.0f;
        load = 0.0f;
    }
    u8 pad0[0xC];
    s32 a0C;        // offset 0xC
    u8 pad10[0x24 - 0x10];
    f32 limit;      // offset 0x24
    f32 stretch;    // offset 0x28
    f32 load;       // offset 0x2C
    f32 f30;        // offset 0x30
    void* owner;    // offset 0x34
};

class CarBody;

extern "C" {
void fn_801AD148(Hitch* h, const RVec3* at, f32 k, f32 len, CarBody* car);
void fn_801AD28C(Hitch* h, const RVec3* at, f32 k, f32 len, CarBody* car, const RVec3* carAt);
void fn_801AD394(Hitch* h);
}

// The state handed to a body's SetState (vtable 0x54).
struct BodyState {
    TMtx mtx;  // offset 0x0
    RVec3 vel;  // offset 0x30
    s32 a3C;   // offset 0x3C
    s16 hint;  // offset 0x40
};

class RigidBody {
public:
    void AddTorque(const RVec3* t);
};

class CarBody : public RigidBody {
public:
    CarBody(void);
    virtual void Init(void* desc);         // 0x8
    virtual ~CarBody(void);                // 0xC
    virtual void Reset(void);              // 0x10
    virtual void vf14(void);               // 0x14
    virtual void vf18(void);               // 0x18
    virtual void Update(void);             // 0x1C
    virtual void vf20(void);               // 0x20
    virtual void vf24(void);               // 0x24
    virtual void vf28(void);               // 0x28
    virtual void vf2C(void);               // 0x2C
    virtual void vf30(void);               // 0x30
    virtual void vf34(void);               // 0x34
    virtual void vf38(void);               // 0x38
    virtual void vf3C(void);               // 0x3C
    virtual void vf40(void);               // 0x40
    virtual void vf44(void);               // 0x44
    virtual void vf48(void);               // 0x48
    virtual void vf4C(void);               // 0x4C
    virtual void vf50(void);               // 0x50
    virtual void SetState(const BodyState* s);  // 0x54
    virtual void vf58(void);               // 0x58
    virtual void vf5C(void);               // 0x5C

    u8 pad4[0x30 - 0x4];
    RVec3 offset;    // offset 0x30
    TMtx mtx;       // offset 0x3C
    s16 hint;       // offset 0x6C
    u8 pad6E[0x70 - 0x6E];
    TMtx world;     // offset 0x70
    u8 padA0[0x118 - 0xA0];
    RVec3 vel;       // offset 0x118
    RVec3 avel;      // offset 0x124
    u8 pad130[0x1EC - 0x130];
    s32 index;      // offset 0x1EC
    u8 pad1F0[0x59C - 0x1F0];
    s32 model;      // offset 0x59C
    u8 pad5A0[0x25D4 - 0x5A0];
    f32 speed;      // offset 0x25D4
    u8 pad25D8[0x268C - 0x25D8];
    s32 a268C;      // offset 0x268C
    f32 f2690;      // offset 0x2690
    f32 f2694;      // offset 0x2694
    u8 pad2698[0x2700 - 0x2698];
};

extern "C" {
CarBody* fn_80182010(CarBody* b);          // CarBody ctor
void fn_80182074(CarBody* b, void* desc);  // CarBody::Init
void fn_801836E0(CarBody* b, int del);     // CarBody dtor
void fn_8018381C(CarBody* b);              // CarBody::Reset
void fn_80189C64(CarBody* b);
void fn_8018A08C(CarBody* b);
void fn_8018ADA4(CarBody* b, f32 steer);
CarBody* fn_8017960C(int index);
}

class Trailer : public CarBody {
public:
    Trailer(void);
    virtual void Init(void* desc);
    virtual ~Trailer(void);
    virtual void Reset(void);
    virtual void Update(void);
    void Reglue(void);

    s32 slot;          // offset 0x2700
    Hitch hitch;       // offset 0x2704
    RVec3 hitchLocal;   // offset 0x273C: on the trailer
    RVec3 hitchCar;     // offset 0x2748: on the car
    CarBody* car;      // offset 0x2754
    f32 slack;         // offset 0x2758: time the hitch has been stretched
    u8 detached;       // offset 0x275C
    u8 low;            // offset 0x275D: alternative (low) hitch
};

ModuleTag lbl_8055E768;
UnkStaticFFFF lbl_8055E770;
s32 lbl_8055E774;          // trailers
Trailer* lbl_80436D28[8];  // by slot

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

inline Trailer::Trailer(void) {
    slot = lbl_8055E774++;
    lbl_80436D28[slot] = this;
    car = fn_8017960C(slot);
    detached = 0;
}

static inline f32 Mul(f32 a, f32 b) { return a * b; }

static inline void MulVecTSelf(RVec3* v, const RVec3* m) {
    f32 x = Dot3(v->x, m[0].x, v->y, m[0].y, v->z, m[0].z);
    f32 y = Dot3(v->x, m[1].x, v->y, m[1].y, v->z, m[1].z);
    f32 z = Dot3(v->x, m[2].x, v->y, m[2].y, v->z, m[2].z);
    v->x = x;
    v->y = y;
    v->z = z;
}

// Puts the trailer behind its car with the hitch points together and reattaches the hitch.
inline void Trailer::Reglue(void) {
    RVec3 d;
    d.x = hitchCar.x - hitchLocal.x;
    d.y = hitchCar.y - hitchLocal.y;
    d.z = hitchCar.z - hitchLocal.z;
    d.x -= offset.x;
    d.y -= offset.y;
    d.z -= offset.z;
    if (low) {
        d.z -= 2.0f;
    }
    BodyState s;
    s.hint = -1;
    CarBody* c = car;
    MulVec(&s.mtx.pos, &d, c->world.rot);
    s.mtx.pos.x += c->world.pos.x;
    s.mtx.pos.y += c->world.pos.y;
    s.mtx.pos.z += c->world.pos.z;
    s.mtx.r = car->world.r;
    f32 v = PSVECDotProduct(&car->vel, &mtx.rot[2]);
    s.vel.x = s.mtx.rot[2].x * v;
    s.vel.y = s.mtx.rot[2].y * v;
    s.vel.z = s.mtx.rot[2].z * v;
    s.a3C = 0;
    s.hint = car->hint;
    SetState(&s);
    hitch.owner = this;
    if (!low) {
        RVec3 at;
        at.x = hitchLocal.x + offset.x;
        at.y = hitchLocal.y + offset.y;
        at.z = hitchLocal.z + offset.z;
        fn_801AD148(&hitch, &at, 175196.86f, 3.05f, car);
    } else {
        RVec3 at;
        at.x = hitchLocal.x + offset.x;
        at.y = hitchLocal.y + offset.y;
        at.z = hitchLocal.z + offset.z;
        fn_801AD28C(&hitch, &at, 175196.86f, 10.0f, car, &hitchCar);
    }
}

extern "C" {

// The position of joint 'name' of 'model' (or 'def' when the model has none).
void fn_801B8CC0(RVec3* out, int model, const char* name, RVec3 def) {
    *out = def;
    if (fn_801E320C(model, name)) {
        Joint* j = fn_801E3004(model, name, -1);
        if (j) {
            fn_801E3488(out, j);
            fn_801E323C(j);
        }
    } else {
        fn_802E30C8("trailer: can't find \"%s\" in %s", name, fn_802C7A10(model));
    }
}

Trailer* fn_801B8D80(void) { return new ("trlr_car") Trailer; }

void fn_801B8E28(int slot) {
    Trailer* t = lbl_80436D28[slot];
    fn_80189C64(t);
    t->Reglue();
    fn_8018A08C(t);
}

struct TrailerTarget {
    s32 id;  // offset 0x0
    u8 pad4[0x18 - 0x4];
};

struct TrailerTargets {
    void Drop(s32 id) {
        for (int k = 0; k < 3; k++) {
            if (t[k].id == id) {
                t[k].id = -1;
            }
        }
    }
    TrailerTarget t[3];
};

struct RaceEntry {
    u8 pad0[0x60];
    s32 trailer;        // offset 0x60
    u8 pad64[0x74 - 0x64];
    s32 body;           // offset 0x74
    f32 f78;            // offset 0x78
    f32 f7C;            // offset 0x7C
    f32 f80;            // offset 0x80
    f32 f84;            // offset 0x84
    s32 a88;            // offset 0x88
    TrailerTargets targets;  // offset 0x8C
    u8 padD4[0xE0 - 0xD4];
    void Attach(CarBody* t) {
        s32 id = t->index;
        if (body == id) {
            body = -1;
        }
        targets.Drop(id);
        body = id;
        f78 = 0.5f;
        f7C = t->f2690;
        f80 = t->f2694;
        f84 = f7C - f80;
        a88 = t->a268C;
    }
};

struct RaceEntries {
    u8 pad0[0x4];
    RaceEntry entries[4];  // offset 0x4
    u8 pad384[0x394 - 0x384];
    s32 count;             // offset 0x394
};

// Points each racer entry's trailer fields at its trailer body.
void fn_801B916C(RaceEntries* r) {
    if (fn_801769CC()->flags & 0x400) {
        for (int i = 0; i < r->count; i++) {
            RaceEntry* e = &r->entries[i];
            Trailer* t;
            if (e->trailer >= 0 && e->trailer < lbl_8055E774) {
                t = lbl_80436D28[e->trailer];
            } else {
                t = 0;
            }
            if (t) {
                e->Attach(t);
            }
        }
    }
}

}  // extern "C"

Trailer::~Trailer(void) {
    lbl_80436D28[slot] = 0;
    lbl_8055E774--;
}

void Trailer::Init(void* desc) {
    low = (fn_801769CC()->flags >> 12) & 1;
    fn_80182074(this, desc);
    RVec3 p;
    fn_801B8CC0(&p, model, "hitch", RVec3(0.0f, 0.0f, 3.0f));
    hitchLocal = p;
    f32 b[6];
    fn_802C77F0(model, b);
    if (low) {
        hitchLocal = RVec3(0.0f, 0.0f, b[5] - 0.2032f);
    }
    f32 cb[6];
    fn_802C77F0(car->model, cb);
    RVec3 cd = low ? RVec3(0.0f, 0.0f, cb[4] - 0.1524f) : RVec3(0.0f, 0.4318f, cb[4] - 0.2032f);
    RVec3 q;
    fn_801B8CC0(&q, car->model, "hitch", cd);
    hitchCar = q;
    hitchCar.y = q.y + 0.127f;
}

void Trailer::Reset(void) {
    detached = 0;
    slack = 0.0f;
    fn_8018381C(this);
    Reglue();
}

// Line message: the two hitch points.
struct HitchMsg {
    s32 a0;     // offset 0x0
    s32 car;    // offset 0x4
    s32 a8;     // offset 0x8
    RVec3 from;  // offset 0xC
    RVec3 to;    // offset 0x18
    u8 pad24[0x54 - 0x24];
};

void Trailer::Update(void) {
    RVec3 torque;
    RVec3 dav;
    dav.x = avel.x - car->avel.x;
    dav.y = avel.y - car->avel.y;
    dav.z = avel.z - car->avel.z;
    torque.y = Mul(dav.y, -4071.75f);
    torque.z = 0.0f;
    torque.x = 0.0f;
    AddTorque(&torque);
    if (!low) {
        fn_8018ADA4(this, detached ? 0.25f : 0.0f);
    } else {
        CarBody* c = car;
        RVec3 l;
        RVec3 dv;
        l.x = c->mtx.pos.x - mtx.pos.x;
        l.y = c->mtx.pos.y - mtx.pos.y;
        l.z = c->mtx.pos.z - mtx.pos.z;
        MulVecTSelf(&l, mtx.rot);
        dv.x = vel.x - c->vel.x;
        dv.y = vel.y - c->vel.y;
        dv.z = vel.z - c->vel.z;
        f32 a = 0.15f * PSVECDotProduct(&dv, &mtx.rot[2]);
        a = (0.25f < a) ? 0.25f : a;
        a = (a > -0.25f) ? a : -0.25f;
        f32 s = 0.2f * l.x - 0.3f * avel.y - a;
        s = (1.0f < s) ? 1.0f : s;
        s = (s > -1.0f) ? s : -1.0f;
        fn_8018ADA4(this, s);
    }
    if (!detached) {
        fn_801AD394(&hitch);
        int over = hitch.load > hitch.limit;
        if (over) {
            slack += 0.016683351f;
        }
        if (slack > 5.0f && car->speed > 2.2222223f) {
            slack = 0.0f;
            fn_802E30C8("trailer: regluing to car...");
            Reglue();
        }
    }
    if (low) {
        RVec3 a;
        MulVec(&a, &hitchLocal, world.rot);
        a.x += world.pos.x;
        a.y += world.pos.y;
        a.z += world.pos.z;
        RVec3 b;
        MulVec(&b, &hitchCar, car->world.rot);
        b.x += car->world.pos.x;
        b.y += car->world.pos.y;
        b.z += car->world.pos.z;
        HitchMsg m;
        m.a0 = 1;
        m.car = car->index;
        m.a8 = 2;
        m.from = a;
        m.to = b;
        fn_801796CC(6, &m, 0x54);
        fn_80189C64(this);
        fn_80189C64(car);
    }
}
