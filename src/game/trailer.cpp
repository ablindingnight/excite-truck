// Trailers (0x801B8CC0-0x801BA088, "trlr_car"): a car body towed by the car with the same index, held
// by a hitch constraint between the "hitch" joints of the two models (or fixed offsets for the
// alternative low hitch). The trailer's yaw is damped towards the car's; if the hitch stays stretched
// for 5 seconds while the car is moving it is reglued behind the car. Names are guesses apart from
// the strings.
#include "game/common.h"
#include "game/raceopts.h"
#include <revolution/MTX/vec.h>
#include "game/rvec.h"
#include "game/collvol.h"
#include "game/rigid.h"
#include "game/carbody.h"
#include "game/hitch.h"
#include "game/plainvec.h"
#include "game/mathutil.h"

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
void fn_801796CC(u8 type, const void* data, u32 n);
}

class CarBody;

extern "C" {
void fn_801AD148(Hitch* h, const RVec3* at, CarBody* car, f32 k, f32 len);
void fn_801AD28C(Hitch* h, const RVec3* at, CarBody* car, const RVec3* carAt, f32 k, f32 len);
void ApplyJointSpringDamping(Hitch* h);
}

struct RVec3;
struct RPacket;
struct BodyState;
struct BodyDesc;

extern "C" {
CarBody* fn_80182010(CarBody* b);          // CarBody ctor
void fn_801836E0(CarBody* b, int del);     // CarBody dtor
void fn_80189C64(CarBody* b);
void fn_8018A08C(CarBody* b);
void fn_8018ADA4(CarBody* b, f32 steer);
CarBody* fn_8017960C(int index);
}

class Trailer : public CarBody {
public:
    Trailer(void);
    virtual void Init(BodyDesc* desc);
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


static inline void MulVec(RVec3* out, const RVec3* in, const PlainRVec3* m) {
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


static inline void MulVecTSelf(RVec3* v, const PlainRVec3* m) {
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
    MulVec(&s.mtx.pos, &d, &c->world.rot.x);
    s.mtx.pos.x += c->world.pos.x;
    s.mtx.pos.y += c->world.pos.y;
    s.mtx.pos.z += c->world.pos.z;
    s.mtx.rot = car->world.rot;
    f32 v = PSVECDotProduct((const Vec*)&car->vel, (const Vec*)&mtx.rot.z);
    s.vel.x = s.mtx.rot.z.x * v;
    s.vel.y = s.mtx.rot.z.y * v;
    s.vel.z = s.mtx.rot.z.z * v;
    s.a3C = 0;
    s.hint = car->hint[0];
    SetState(&s);
    hitch.owner = this;
    if (!low) {
        RVec3 at;
        at.x = hitchLocal.x + offset.x;
        at.y = hitchLocal.y + offset.y;
        at.z = hitchLocal.z + offset.z;
        fn_801AD148(&hitch, &at, car, 175196.86f, 0.305f * 10.0f);  // 10 feet
    } else {
        RVec3 at;
        at.x = hitchLocal.x + offset.x;
        at.y = hitchLocal.y + offset.y;
        at.z = hitchLocal.z + offset.z;
        fn_801AD28C(&hitch, &at, car, &hitchCar, 175196.86f, 10.0f);
    }
}

extern "C" {

void GetTrailerJointPosition(RVec3* out, int model, const char* name, RVec3 def) {
    *(WV3*)out = *(WV3*)&def;
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

Trailer* CreateTrailer(void) { return new ("trlr_car") Trailer; }

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
        if (t[0].id == id) {
            t[0].id = -1;
        }
        if (t[1].id == id) {
            t[1].id = -1;
        }
        if (t[2].id == id) {
            t[2].id = -1;
        }
    }
    TrailerTarget t[3];
};

// A racer's trailer state (0xE0; was named RaceEntry here, which recordmgr uses for its race entries).
struct TrailerEntry {
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

struct TrailerEntries {
    u8 pad0[0x4];
    TrailerEntry entries[4];  // offset 0x4
    u8 pad384[0x394 - 0x384];
    s32 count;             // offset 0x394
};

// Points each racer entry's trailer fields at its trailer body.
/* Helper name unproven: the loop over the entries; the boundary is needed (in place -0.53) */
static inline void AttachTrailers(TrailerEntries* r) {
    TrailerEntry* e = r->entries;
    for (int i = 0; i < r->count; e++, i++) {
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

void fn_801B916C(TrailerEntries* r) {
    if (fn_801769CC()->type & 0x400) {
        AttachTrailers(r);
    }
}

}  // extern "C"

Trailer::~Trailer(void) {
    lbl_80436D28[slot] = 0;
    lbl_8055E774--;
}

/* Helper name unproven: a named model point returned by value, with a default. */
static inline RVec3 FindPoint(int model, const char* name, RVec3 def) {
    RVec3 r;
    GetTrailerJointPosition(&r, model, name, def);
    return r;
}

void Trailer::Init(BodyDesc* desc) {
    low = (fn_801769CC()->type >> 12) & 1;
    CarBody::Init(desc);
    RVec3 p;
    GetTrailerJointPosition(&p, model, "hitch", RVec3(0.0f, 0.0f, 3.0f));
    hitchLocal = p;
    f32 b[6];
    fn_802C77F0(model, b);
    if (low) {
        hitchLocal = RVec3(0.0f, 0.0f, b[5] - 0.2032f);
    }
    f32 cb[6];
    fn_802C77F0(car->model, cb);
    RVec3 cd = low ? RVec3(0.0f, 0.0f, cb[4] - 0.1524f) : RVec3(0.0f, 0.4318f, cb[4] - 0.2032f);
    hitchCar = FindPoint(car->model, "hitch", cd);
    hitchCar.y += 0.127f;
}

void Trailer::Reset(void) {
    detached = 0;
    slack = 0.0f;
    CarBody::Reset();
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

// Helper name unproven.
static inline void SubR(RVec3* o, const RVec3* a, const RVec3* b) {
    o->x = a->x - b->x;
    o->y = a->y - b->y;
    o->z = a->z - b->z;
}
// inline: only the vtable refers to it, so it is generated after the table (retail's "regluing" string follows
// __vt__7Trailer; FACTS 2026-11-16 deferred functions).
inline void Trailer::Update(void) {
    RVec3 torque;
    RVec3 dav;
    SubR(&dav, &avel, &car->avel);
    torque.y = dav.y;
    torque.y *= -4071.7498f;
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
        MulVecTSelf(&l, &mtx.rot.x);
        dv.x = vel.x - c->vel.x;
        dv.y = vel.y - c->vel.y;
        dv.z = vel.z - c->vel.z;
        f32 a = 0.15f * PSVECDotProduct((const Vec*)&dv, (const Vec*)&mtx.rot.z);
        a = (0.25f < a) ? 0.25f : a;
        a = (a > -0.25f) ? a : -0.25f;
        f32 s = 0.2f * l.x - 0.3f * avel.y - a;
        s = (1.0f < s) ? 1.0f : s;
        s = (s > -1.0f) ? s : -1.0f;
        fn_8018ADA4(this, s);
    }
    if (!detached) {
        ApplyJointSpringDamping(&hitch);
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
        MulVec(&a, &hitchLocal, &world.rot.x);
        a.x += world.pos.x;
        a.y += world.pos.y;
        a.z += world.pos.z;
        RVec3 b;
        CarBody* c = car;
        MulVec(&b, &hitchCar, &c->world.rot.x);
        b.x += c->world.pos.x;
        b.y += c->world.pos.y;
        b.z += c->world.pos.z;
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
