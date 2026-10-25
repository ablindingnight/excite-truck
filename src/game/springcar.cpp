// Spring-wall car (0x801A520C-0x801A6344): a CarBody driven by a controller object, kept on the
// track by up to eight soft "spring walls" (each pushes the car back along the wall normal with a
// depth that ramps in over time), and a lift that props the car up when it is stuck on its rear
// wheels. Also hands the car back to the course when it has been off the terrain for too long.
// Names are guesses apart from the message.
#include "game/common.h"

struct TVec {
    f32 x, y, z;
};

struct TPos {
    f32 x, y, z;
};

class SpringCar;

// The object steering the car (vtable at 0x0).
class Controller {
public:
    virtual ~Controller(void);
    virtual void Init(SpringCar* car, void* desc);                       // 0x0C
    virtual void Attach(SpringCar* car);                                 // 0x10
    virtual int vf14(void);                                              // 0x14
    virtual int vf18(u8* buf);                                           // 0x18
    virtual void vf1C(f32 t, int a, int b, int* na, int* nb);            // 0x1C
    virtual int vf20(u8* p);                                             // 0x20
    virtual void Reset(void);                                            // 0x24
    virtual void Detach(void);                                           // 0x28
    virtual void Update(void);                                           // 0x2C
    virtual void vf30(void);
    virtual void vf34(void);                                             // 0x34
    virtual void vf38(const TVec* a, const TVec* b, int kind, int c);    // 0x38
    virtual void vf3C(const TVec* a, const TVec* b, int kind, int c);    // 0x3C
    virtual void vf40(void);                                             // 0x40
    virtual void vf44(void);                                             // 0x44
};

struct WallPlane {
    u8 pad0[0x24];
    TVec origin;  // offset 0x24
};

struct WallObj {
    u8 pad0[0xC];
    WallPlane* plane;  // offset 0xC
};

struct Lift {
    void Reset(void) {
        state = 1;
        count = 0;
        t = 0.0f;
        f = 0.0f;
    }
    void Apply(SpringCar* car);
    void Step(SpringCar* car, f32 rate);

    s32 state;  // offset 0x0
    f32 f;      // offset 0x4
    f32 t;      // offset 0x8
    s32 count;  // offset 0xC
};

struct SpringWall {
    void Apply(SpringCar* car, TVec* acc);
    void Set(WallObj* w, SpringCar* car, const TVec* other);

    TVec n;           // offset 0x0
    f32 vel;          // offset 0xC
    f32 depth;        // offset 0x10
    f32 dist;         // offset 0x14
    f32 time;         // offset 0x18
    f32 ramp;         // offset 0x1C
    WallObj* owner;   // offset 0x20
};

struct SaveRec {
    u8 pad0[0x2C0];
    s32 ctl;  // offset 0x2C0
};

struct WheelState {
    u8 contact;  // offset 0x0
    u8 pad1[0x74 - 0x1];
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

class Course {
public:
    virtual void vf08(void);
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
    virtual void vf3C(void);
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

class CarBody {
public:
    CarBody(void);
    virtual void Init(void* desc);   // 0x08
    virtual ~CarBody(void);          // 0x0C
    virtual void Reset(void);        // 0x10
    virtual void vf14(void);         // 0x14
    virtual void vf18(void);
    virtual void vf1C(void);
    virtual void vf20(void);
    virtual void vf24(void);
    virtual void vf28(u8* buf);                                    // 0x28
    virtual int vf2C(u8* buf);                                     // 0x2C
    virtual void vf30(f32 t, int a, int b, int* na, int* nb);      // 0x30
    virtual int vf34(u8* p);                                       // 0x34
    virtual void vf38(void);
    virtual void vf3C(void);
    virtual void vf40(void);
    virtual void vf44(void);
    virtual void vf48(const TVec* a, const TVec* b, int kind, int c);  // 0x48
    virtual void vf4C(const TVec* a, const TVec* b, int kind, int c);  // 0x4C
    virtual void vf50(void);
    virtual void vf54(void);   // 0x54
    virtual void vf58(void);   // 0x58
    virtual void Update(void); // 0x5C

    u8 pad4[0x8 - 0x4];
    s32 slot;    // offset 0x8
    u8 padC[0x54 - 0xC];
    TVec up;     // offset 0x54
    TPos pos;    // offset 0x60
    s16 hint;    // offset 0x6C
    u8 pad6E[0xD4 - 0x6E];
    f32 mass;    // offset 0xD4
    u8 padD8[0x118 - 0xD8];
    TVec vel;    // offset 0x118
    f32 spin;    // offset 0x124
    f32 roll;    // offset 0x128
    u8 pad12C[0x1EC - 0x12C];
    s32 index;   // offset 0x1EC
    u8 pad1F0[0x270 - 0x1F0];
    StatsSrc* src;  // offset 0x270
    u8 pad274[0x23CC - 0x274];
    WheelState wheels[4];  // offset 0x23CC
    u8 pad259C[0x25D4 - 0x259C];
    f32 speed;   // offset 0x25D4
    u8 pad25D8[0x2620 - 0x25D8];
    s32 airborne;  // offset 0x2620
    u8 pad2624[0x26B4 - 0x2624];
    TVec push;   // offset 0x26B4
    u8 pad26C0[0x2700 - 0x26C0];
};

class SpringCar : public CarBody {
public:
    SpringCar(Controller* c);
    virtual void Init(void* desc);
    virtual ~SpringCar(void);
    virtual void Reset(void);
    virtual void vf14(void);
    virtual void vf28(u8* buf);
    virtual int vf2C(u8* buf);
    virtual void vf30(f32 t, int a, int b, int* na, int* nb);
    virtual int vf34(u8* p);
    virtual void vf48(const TVec* a, const TVec* b, int kind, int c);
    virtual void vf4C(const TVec* a, const TVec* b, int kind, int c);
    virtual void vf54(void);
    virtual void vf58(void);
    virtual void Update(void);
    void SetController(Controller* c);
    void CheckLost(void);
    void HitWall(WallObj* w);
    void ClearWalls(void) {
        int i;
        for (i = 0; i < 8; i++) {
            walls[i].owner = 0;
        }
    }

    s32 lost;              // offset 0x2700
    u8 dead;               // offset 0x2704
    s32 f2708;             // offset 0x2708
    Controller* ctl;       // offset 0x270C
    u8 lifting;            // offset 0x2710
    Lift lift;             // offset 0x2714
    SpringWall walls[8];   // offset 0x2724
};

extern "C" {
void fn_80182074(CarBody* b, void* desc);  // CarBody::Init
void fn_8018381C(CarBody* b);              // CarBody::Reset
void fn_80183DF8(CarBody* b);
void fn_80186DE4(CarBody* b);
void fn_80188E2C(CarBody* b, u8* buf);
void fn_8018956C(CarBody* b, const TVec* a, const TVec* bb, int kind, int c);
void fn_80189CB4(CarBody* b);
void fn_8018AF00(CarBody* b);
void fn_8018B09C(CarBody* b, const TVec* a, const TVec* bb, int kind, int c);
void fn_8018B0D8(CarBody* b, f32 t, int a, int bb, int* na, int* nb);
int fn_8018BA54(CarBody* b, u8* p);
void fn_8017B3A8(CarBody* b, const TVec* imp, const TVec* at);
Course* fn_801B83A0(void);
int fn_801E9E30(s16* h, TPos* pos);
void fn_80265D38(CarBody* b, s32 i);
void fn_80265D3C(CarBody* b);
void fn_802E30C8(const char* fmt, ...);
f32 PSVECDotProduct(const TVec* a, const TVec* b);
}

extern s32 lbl_8055E3C4;    // frame counter
extern f32 lbl_8055E3C8;    // race clock
extern u8 lbl_8055A518;
extern f32 lbl_80558DC4[];  // FLT_EPSILON

#define NearZero(x) ((f32)__fabs(x) <= lbl_80558DC4[0])

ModuleTag lbl_8055E5A8;
UnkStaticFFFF lbl_8055E5B0;

static f32 PlaneDist(const TVec* n, const TVec* p, const TVec* o);

static inline void AddScaled(TVec* o, const TPos* a, const TVec* b, f32 k) {
    o->x = a->x + b->x * k;
    o->y = a->y + b->y * k;
    o->z = a->z + b->z * k;
}

static inline void Scale(TVec* v, f32 k) {
    v->x = v->x * k;
    v->y = v->y * k;
    v->z = v->z * k;
}

SpringCar::SpringCar(Controller* c) { ctl = c; }

void SpringCar::Init(void* desc) {
    fn_80182074(this, desc);
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
        TPos p = pos;
        if (!fn_801E9E30(&hint, &p)) {
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

void SpringCar::Update(void) {
    u32 i;
    fn_80186DE4(this);
    CheckLost();
    ctl->Update();
    if (lifting && lbl_8055A518) {
        lift.Step(this, src->Get()->stats->rate);
    }
    {
        TVec acc;
        acc.x = 0.0f;
        acc.y = 0.0f;
        acc.z = 0.0f;
        for (i = 0; i < 8; i++) {
            walls[i].Apply(this, &acc);
        }
    }
}

void SpringCar::Reset(void) {
    fn_8018381C(this);
    ClearWalls();
    lift.Reset();
    ctl->Detach();
    ctl->Reset();
    lost = 0;
    dead = 0;
}

void SpringCar::vf14(void) {
    fn_80183DF8(this);
    ctl->vf34();
}

void SpringWall::Apply(SpringCar* car, TVec* acc) {
    if (owner) {
        TVec v;
        TVec p;
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
        t = 1.0f >= t ? t : 1.0f;
        t = t > 0.0f ? t : 0.0f;
        p.x = car->pos.x + v.x * 0.016683351f;
        p.y = car->pos.y + v.y * 0.016683351f;
        p.z = car->pos.z + v.z * 0.016683351f;
        target = vel + t * (depth - vel);
        dist = PlaneDist(&n, &p, &owner->plane->origin);
        ramping = 0.0f > this->dist, time < ramp;
        if (ramping) {
            f32 pen = target - d;
            f32 extra;
            f32 total;
            TVec f;
            TVec imp;
            TPos at;
            TVec pt;
            pen = 0.0f > pen ? 0.0f : pen;
            extra = 0.0f;
            if (d < 0.0f) {
                f32 u = (dist - 0.5f) / -0.5f;
                u = 1.0f >= u ? u : 1.0f;
                u = u > 0.0f ? u : 0.0f;
                extra = 0.0f + u * ((6.6666665f + -d) - 0.0f);
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
            if (v.y > 0.44444445f && car->airborne == 0) {
                f32 q = 0.1f * v.y;
                q = 0.44444445f >= q ? q : 0.44444445f;
                imp.y = -q * (car->mass / 0.016683351f);
            }
            pen = pen + extra;
            at = *(TPos*)&car->up;
            at.y = 0.0f;
            if (!NearZero(pen) && PSVECDotProduct((TVec*)&at, &n) < 0.0f && (f32)__fabs(car->roll) < 1.5707964f) {
                f32 m = 17.777779f / pen;
                m = 1.0f >= m ? m : 1.0f;
                m = 0.8f * m;
                pt.x = car->pos.x + at.x * m;
                pt.y = car->pos.y + at.y * m;
                pt.z = car->pos.z + at.z * m;
            } else {
                pt.x = car->pos.x;
                pt.y = car->pos.y;
                pt.z = car->pos.z;
            }
            fn_8017B3A8(car, &imp, &pt);
            car->push.x = 0.0f;
            car->push.y = 0.0f;
            car->push.z = 0.0f;
        }
        time = time + 0.016683351f;
        if (time > 0.2f + ramp && !ramping) {
            owner = 0;
        }
    }
}

static f32 PlaneDist(const TVec* n, const TVec* p, const TVec* o) {
    TVec d;
    d.x = p->x - o->x;
    d.y = p->y - o->y;
    d.z = p->z - o->z;
    return PSVECDotProduct(n, &d);
}

void SpringWall::Set(WallObj* w, SpringCar* car, const TVec* other) {
    if (owner == 0) {
        TVec p;
        f32 k;
        n = *(TVec*)w->plane;
        AddScaled(&p, &car->pos, &car->vel, -0.016683351f);
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
        TVec z;
        z.x = 0.0f;
        z.y = 0.0f;
        z.z = 0.0f;
        ctl->vf38(&z, &z, 0x3EA, 0);
    }
}

void SpringCar::vf48(const TVec* a, const TVec* b, int kind, int c) {
    fn_8018B09C(this, a, b, kind, c);
    ctl->vf38(a, b, kind, c);
    if (kind == 0x6D) {
        lift.Reset();
    }
}

void SpringCar::vf4C(const TVec* a, const TVec* b, int kind, int c) {
    fn_8018956C(this, a, b, kind, c);
    ctl->vf3C(a, b, kind, c);
    if (kind == 0x6D) {
        lift.Reset();
    }
}

void SpringCar::vf58(void) {
    fn_8018AF00(this);
    ctl->vf40();
}

void SpringCar::vf54(void) {
    int i;
    fn_80189CB4(this);
    dead = 1;
    ctl->vf44();
    for (i = 0; i < 8; i++) {
        walls[i].owner = 0;
    }
}

int SpringCar::vf2C(u8* buf) { return ctl->vf18(buf + 0x44) + 0x44; }

void SpringCar::vf30(f32 t, int a, int b, int* na, int* nb) {
    int x, y, x2, y2;
    fn_8018B0D8(this, t, a, b, &x, &y);
    ctl->vf1C(t, a + x, b + y, &x2, &y2);
    *na = x + x2;
    *nb = y + y2;
}

int SpringCar::vf34(u8* p) {
    u8* q = p + fn_8018BA54(this, p);
    q += ctl->vf20(q);
    return q - p;
}

void SpringCar::vf28(u8* buf) {
    fn_80188E2C(this, buf);
    SaveRec* r = (SaveRec*)(buf + slot);
    r->ctl = ctl->vf14();
}

void Lift::Apply(SpringCar* car) {
    TVec at;
    TVec imp;
    if (car->spin > 0.2617994f) {
        f32 k = 2.0f * (-0.1f - car->up.y);
        k = 0.0f > k ? 0.0f : k;
        at.x = car->pos.x + car->up.x * k;
        at.y = car->pos.y + car->up.y * k;
        at.z = car->pos.z + car->up.z * k;
    } else {
        at.x = car->pos.x;
        at.y = car->pos.y;
        at.z = car->pos.z;
    }
    imp.y = f;
    imp.x = 0.0f;
    imp.z = 0.0f;
    fn_8017B3A8(car, &imp, &at);
}

void Lift::Step(SpringCar* car, f32 rate) {
    if (state) {
        int stop = 1;
        if (!car->wheels[0].contact && !car->wheels[1].contact && car->wheels[2].contact && car->wheels[3].contact) {
            f32 r = car->speed / rate;
            if (r > 0.6f && car->vel.y > 0.0f && car->vel.y < 0.5f * car->speed) {
                if (++count > 4) {
                    f32 m = 1.0f - r;
                    t = lbl_8055E3C8;
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
