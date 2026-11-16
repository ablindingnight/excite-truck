// Spring-wall car (0x801A520C-0x801A6344): a CarBody driven by a controller object, kept on the
// track by up to eight soft "spring walls" (each pushes the car back along the wall normal with a
// depth that ramps in over time), and a lift that props the car up when it is stuck on its rear
// wheels. Also hands the car back to the course when it has been off the terrain for too long.
// Names are guesses apart from the message.
#include "game/common.h"
#include "game/collvol.h"
#include "game/rigid.h"
#include "game/springcar.h"


struct SaveRec {
    u8 pad0[0x2C0];
    s32 ctl;  // offset 0x2C0
};

struct Stats {
    u8 pad0[0x20];
    f32 rate;  // offset 0x20
};

struct StatsHolder {
    u8 pad0[0x4];
    Stats* stats;  // offset 0x4
};

class StatsSrc {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void vf18(void);
    virtual StatsHolder* Get(void);  // 0x1C
};

// The game mode (fn_801B83A0, GameMode family: destructor at 0x08).
class Course {
public:
    virtual ~Course(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void vf18(void);
    virtual void vf1C(void);
    virtual void vf20(void);
    virtual void vf24(void);
    virtual void vf28(void);
    virtual void vf2C(void);
    virtual void vf30(void);
    virtual void vf34(void);
    virtual void vf38(void);
    virtual RigidBody* vf3C(void);
    virtual void vf40(void);
    virtual void vf44(void);
    virtual void vf48(void);
    virtual void vf4C(void);
    virtual void vf50(void);
    virtual void vf54(void);
    virtual void vf58(void);
    virtual void vf5C(void);
    virtual void vf60(void);
    virtual void vf64(void);
    virtual void vf68(void);
    virtual void vf6C(void);
    virtual void vf70(void);
    virtual void vf74(void);
    virtual void vf78(void);
    virtual void vf7C(void);
    virtual void vf80(void);
    virtual void vf84(void);
    virtual void vf88(void);
    virtual void vf8C(void);
    virtual void vf90(void);
    virtual void vf94(void);
    virtual void vf98(void);
    virtual void vf9C(void);
    virtual void vfA0(void);
    virtual void vfA4(void);
    virtual void vfA8(void);
    virtual void Respawn(s32 player);  // 0xAC
};

// The rigid body a car body is (rigid.cpp); CarBody below is still declared flat.
struct RVec3;

struct RVec3;
struct RPacket;
struct BodyState;
struct BodyDesc;

extern "C" {
Course* fn_801B83A0(void);
int fn_801E9E30(s16* h, RVec3* pos);
void fn_80265D38(CarBody* b, s32 i);
void fn_80265D3C(CarBody* b);
void fn_802E30C8(const char* fmt, ...);
f32 PSVECDotProduct(const RVec3* a, const RVec3* b);
}

extern s32 lbl_8055E3C4;    // frame counter
extern f32 lbl_8055E3C8;    // race clock
extern u8 lbl_8055A518;
extern f32 __float_epsilon[];  // FLT_EPSILON


ModuleTag lbl_8055E5A8;
UnkStaticFFFF lbl_8055E5B0;

static f32 PlaneDist(const RVec3* n, const RVec3* p, const RVec3* o);

static inline void AddScaled(RVec3* o, const RVec3* a, const RVec3* b, f32 k) {
    o->x = a->x + b->x * k;
    o->y = a->y + b->y * k;
    o->z = a->z + b->z * k;
}

static inline void Scale(RVec3* v, f32 k) {
    v->x = v->x * k;
    v->y = v->y * k;
    v->z = v->z * k;
}

SpringCar::SpringCar(Controller* c) { ctl = c; }

void SpringCar::Init(BodyDesc* desc) {
    CarBody::Init(desc);
    fn_80265D38(this, index);
    f2708 = 0;
    dead = 0;
    lost = 0;
    ctl->Init(this, desc);
    ctl->Reset();
    lift.Reset();
    lifting = 0;
}

void SpringCar::SetController(Controller* c) {
    if (ctl) {
        ctl->Detach();
    }
    ctl = c;
    if (c) {
        c->Attach(this);
        ctl->Reset();
    }
}

SpringCar::~SpringCar(void) {
    fn_80265D3C(this);
    ctl->Detach();
    delete ctl;
    ctl = 0;
}

void SpringCar::CheckLost(void) {
    if (lost == 0 && ((lbl_8055E3C4 + index) & 0x1F)) {
        return;
    }
    {
        RVec3 p = mtx.pos;
        if (!fn_801E9E30(hint, &p)) {
            lost++;
        } else {
            lost = 0;
        }
    }
    if (lost > 10) {
        fn_801B83A0()->Respawn(index);
        lost = 0;
    }
}

void SpringCar::vf5C(void) {
    u32 i;
    CarBody::vf5C();
    CheckLost();
    ctl->Update();
    if (lifting && lbl_8055A518) {
        lift.Step(this, src->Get()->stats->rate);
    }
    {
        RVec3 acc;
        acc.x = 0.0f;
        acc.y = 0.0f;
        acc.z = 0.0f;
        for (i = 0; i < 8; i++) {
            walls[i].Apply(this, &acc);
        }
    }
}

#pragma push
#pragma opt_unroll_loops off
void SpringCar::Reset(void) {
    CarBody::Reset();
    ClearWalls();
    lift.Reset();
    ctl->Detach();
    ctl->Reset();
    lost = 0;
    dead = 0;
}
#pragma pop

void SpringCar::vf14(void) {
    CarBody::vf14();
    ctl->vf34();
}

static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

/* Helper name unproven: whether the wall is still ramping. */
inline int SpringWall::Ramping(void) {
    return time < ramp;
}

/* Helper name unproven */
static inline void AddScaledV(RVec3* out, const RVec3* a, const RVec3* b, f32 k) {
    out->x = a->x + b->x * k;
    out->y = a->y + b->y * k;
    out->z = a->z + b->z * k;
}
/* Helper name unproven */
static inline f32 AbsPos(f32 x) { return 0.0f < x ? x : -x; }
void SpringWall::Apply(SpringCar* car, RVec3* acc) {
    if (owner) {
        RVec3 v;
        RVec3 p;
        f32 d;
        f32 t;
        f32 target;
        f32 dist;
        int ramping;
        v.x = acc->x + car->vel.x;
        v.y = acc->y + car->vel.y;
        v.z = acc->z + car->vel.z;
        d = PSVECDotProduct(&v, &n);
        t = time / ramp;
        t = 1.0f < t ? 1.0f : t;
        t = t > 0.0f ? t : 0.0f;
        target = vel + t * (depth - vel);
        AddScaledV(&p, &car->mtx.pos, &v, 0.016683351f);
        dist = PlaneDist(&n, &p, &owner->plane->origin);
        // Unused, as in retail: only its compare of the stored distance survives (what it fed is unproven;
        // its arms create no new float literal - retail has no -1.0f in this function's pool).
        f32 side = AbsPos(this->dist);
        ramping = Ramping();
        if (ramping) {
            f32 climbMax;
            f32 pen = target - d;
            f32 extra;
            f32 total;
            RVec3 imp;
            RVec3 f;
            RVec3 pt;
            pen = 0.0f > pen ? 0.0f : pen;
            extra = 0.0f;
            if (d < 0.0f) {
                f32 u = (dist - 0.5f) / -0.5f;
                u = 1.0f < u ? 1.0f : u;
                u = u > 0.0f ? u : 0.0f;
                extra = Lerp(0.0f, 6.6666665f + -d, u);
                pen = 0.0f;
            }
            total = extra + pen;
            f.x = n.x * total;
            f.y = n.y * total;
            f.z = n.z * total;
            acc->x = acc->x + f.x;
            acc->y = acc->y + f.y;
            acc->z = acc->z + f.z;
            {
                f32 k = car->mass / 0.016683351f;
                imp.x = f.x * k;
                imp.y = f.y * k;
                imp.z = f.z * k;
            }
            climbMax = 0.44444445f;
            if (v.y > climbMax && car->airborne == 0) {
                f32 mass = car->mass;
                f32 q = 0.1f * v.y;
                q = climbMax < q ? climbMax : q;
                imp.y = -q * (mass / 0.016683351f);
            }
            pen += extra;
            RVec3 at = *(RVec3*)&car->mtx.rot.z;
            f64 absWide = __fabs(pen);
            f32 absPen = (f32)absWide;
            at.y = 0.0f;
            if (!(absPen <= __float_epsilon[0]) && PSVECDotProduct((RVec3*)&at, &n) < 0.0f && (f32)__fabs(car->avel.y) < 1.5707964f) {
                f32 m = 17.777779f / pen;
                m = 1.0f < m ? 1.0f : m;
                m = 0.8f * m;
                pt.x = car->mtx.pos.x + at.x * m;
                pt.y = car->mtx.pos.y + at.y * m;
                pt.z = car->mtx.pos.z + at.z * m;
            } else {
                pt.x = car->mtx.pos.x;
                pt.y = car->mtx.pos.y;
                pt.z = car->mtx.pos.z;
            }
            ((RigidBody*)car)->AddForceAt((const RVec3*)&imp, (const RVec3*)&pt);
            car->push.x = 0.0f;
            car->push.y = 0.0f;
            car->push.z = 0.0f;
        }
        time += 0.016683351f;
        if (time > 0.2f + ramp && !ramping) {
            owner = 0;
        }
    }
}

static f32 PlaneDist(const RVec3* n, const RVec3* p, const RVec3* o) {
    RVec3 d;
    d.x = p->x - o->x;
    d.y = p->y - o->y;
    d.z = p->z - o->z;
    return PSVECDotProduct(n, &d);
}

void SpringWall::Set(WallObj* w, SpringCar* car, const RVec3* other) {
    if (Free()) {
        RVec3 p;
        f32 k;
        n = *(RVec3*)w->plane;
        AddScaled(&p, &car->mtx.pos, &car->vel, -0.016683351f);
        if (other && PSVECDotProduct(other, &n) < 0.0f) {
            Scale(&n, -1.0f);
        }
        dist = PlaneDist(&n, &p, &w->plane->origin);
        vel = PSVECDotProduct(&car->vel, &n);
        k = -vel;
        k = 17.777779f < k ? 17.777779f : k;
        k = k > 0.8888889f ? k : 0.8888889f;
        ramp = 0.2f;
        depth = 0.2f * k;
        time = 0.0f;
        owner = w;
    } else if (owner == w) {
        time = 0.0f;
    }
}

void SpringCar::HitWall(WallObj* w) {
    SpringWall* found = 0;
    SpringWall* other = 0;
    int i;
    for (i = 0; i < 8; i++) {
        if (walls[i].owner && walls[i].owner != w) {
            other = &walls[i];
        }
        if (!found || found->owner != w) {
            if (walls[i].owner == w || walls[i].owner == 0) {
                found = &walls[i];
            }
        }
    }
    if (found) {
        found->Set(w, this, &other->n);
    } else {
        fn_802E30C8("%5.5d: springwall: Out of walls!", lbl_8055E3C4);
    }
    {
        RVec3 z;
        z.x = 0.0f;
        z.y = 0.0f;
        z.z = 0.0f;
        ctl->vf38(&z, &z, 0x3EA, 0);
    }
}

void SpringCar::OnHit(const RVec3* a_, const RVec3* b_, int kind, void* c_) {
    const RVec3* a = (const RVec3*)a_;
    const RVec3* b = (const RVec3*)b_;
    int c = (int)c_;
    CarBody::OnHit(a_, b_, kind, c_);
    ctl->vf38(a, b, kind, c);
    if (kind == 0x6D) {
        lift.Reset();
    }
}

void SpringCar::OnImpulse(const RVec3* a_, const RVec3* b_, int kind, void* c_, void* by) {
    const RVec3* a = (const RVec3*)a_;
    const RVec3* b = (const RVec3*)b_;
    int c = (int)c_;
    CarBody::OnImpulse(a_, b_, kind, c_, by);
    ctl->vf3C(a, b, kind, c);
    if (kind == 0x6D) {
        lift.Reset();
    }
}

void SpringCar::vf58(void) {
    CarBody::vf58();
    ctl->vf40();
}

void SpringCar::SetState(const BodyState* s) {
    int i;
    CarBody::SetState(s);
    dead = 1;
    ctl->vf44();
    for (i = 0; i < 8; i++) {
        walls[i].owner = 0;
    }
}

s32 SpringCar::vf2C(RPacket* pk) { return ctl->vf18((u8*)pk + 0x44) + 0x44; }

void SpringCar::vf30(f32 t, RPacket* a, const RPacket* b, s32* na, s32* nb) {
    s32 x, y;
    int x2, y2;
    CarBody::vf30(t, a, b, &x, &y);
    ctl->vf1C(t, (int)a + x, (int)b + y, &x2, &y2);
    *na = x + x2;
    *nb = y + y2;
}

s32 SpringCar::vf34(RPacket* pk) {
    u8* p = (u8*)pk;
    u8* q = p + CarBody::vf34(pk);
    q += ctl->vf20(q);
    return q - p;
}

void SpringCar::vf28(u8* buf) {
    CarBody::vf28(buf);
    SaveRec* r = (SaveRec*)(buf + a8);
    r->ctl = ctl->vf14();
}

void Lift::Apply(SpringCar* car) {
    RVec3 at;
    RVec3 imp;
    if (car->avel.x > 0.2617994f) {
        f32 k = 2.0f * (-0.1f - car->mtx.rot.z.y);
        k = 0.0f > k ? 0.0f : k;
        at.x = car->mtx.pos.x + car->mtx.rot.z.x * k;
        at.y = car->mtx.pos.y + car->mtx.rot.z.y * k;
        at.z = car->mtx.pos.z + car->mtx.rot.z.z * k;
    } else {
        at.x = car->mtx.pos.x;
        at.y = car->mtx.pos.y;
        at.z = car->mtx.pos.z;
    }
    imp.y = f;
    imp.x = 0.0f;
    imp.z = 0.0f;
    ((RigidBody*)car)->AddForceAt((const RVec3*)&imp, (const RVec3*)&at);
}

void Lift::Step(SpringCar* car, f32 rate) {
    if (state) {
        int stop = 1;
        if (!car->wheels[0].grounded && !car->wheels[1].grounded && car->wheels[2].grounded && car->wheels[3].grounded) {
            f32 r = car->speed / rate;
            if (r > 0.6f && car->vel.y > 0.0f && car->vel.y < 0.5f * car->speed) {
                if (++count > 4) {
                    f32 m = 1.0f - r;
                    f32 now = lbl_8055E3C8;
                    t = now;
                    m = 1.0f < m ? 1.0f : m;
                    m = m > 0.0f ? m : 0.0f;
                    f = 9.81f * (m * car->mass);
                    state = 0;
                    while (--count) {
                        Apply(car);
                    }
                } else {
                    stop = 0;
                }
            }
        }
        if (stop) {
            count = 0;
        }
    } else {
        Apply(car);
        if (car->airborne > 0 && lbl_8055E3C8 - t > 1.0f) {
            Reset();
        }
    }
}
