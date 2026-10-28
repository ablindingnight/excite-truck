// Car setup (0x80279CE4-0x8027AA08): a car's physical parameters from its .car/physics.val
// (chassis, transmission, engine and the four wheels' suspension, converted from US units to
// metric), plus a table of named tweak multipliers applied on top. Names are guesses apart from
// the keys and messages.
#include "game/common.h"

extern "C" {
int sprintf(char* buf, const char* fmt, ...);
void* memset(void* dst, int c, size_t n);
int strcmp(const char* a, const char* b);
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* table);
}

static inline f32 Lb(f32 x) { return 0.4545f * x; }    // pounds to kilograms
static inline f32 Ft(f32 x) { return 0.305f * x; }     // feet to metres
static inline f32 In(f32 x) { return 0.0254f * x; }    // inches to metres
static inline f32 Lbf(f32 x) { return 4.45f * x; }     // pounds-force to newtons
static inline f32 Min(f32 a, f32 b) { return (b < a) ? b : a; }
static inline f32 Max(f32 a, f32 b) { return (a > b) ? a : b; }

// Copies at most n - 1 characters and always terminates.
static inline void CopyName(char* dst, const char* src, int n) {
    char* d = dst;
    int left = n;
    while ((*d++ = *src++) != 0) {
        if (--left <= 0) {
            d[-1] = 0;
            break;
        }
    }
}

struct WheelSetup {
    f32 rideHeight;      // offset 0x0
    f32 spring;          // offset 0x4
    f32 bump;            // offset 0x8
    f32 rebound;         // offset 0xC
    f32 bumpstopHeight;  // offset 0x10
    f32 bumpstopRate;    // offset 0x14
    f32 unk18;           // offset 0x18
    f32 unk1C;           // offset 0x1C
    f32 unk20;           // offset 0x20
};

static inline void WheelDefaults(WheelSetup* w, int front) {
    w->rideHeight = 0.127f;
    w->spring = front ? 70078.734f : 43799.215f;
    w->bump = 1896.7213f;
    w->rebound = 6127.868f;
    w->bumpstopHeight = 0.2794f;
    w->bumpstopRate = 105118.11f;
    w->unk1C = 350393.72f;
    w->unk20 = 8754.098f;
    w->unk18 = 0.3556f;
}

struct CarSetup {
    f32 mass;             // offset 0x0
    f32 mx;               // offset 0x4
    f32 my;               // offset 0x8
    f32 mz;               // offset 0xC
    f32 ftrack;           // offset 0x10
    f32 rtrack;           // offset 0x14
    f32 wheelbase;        // offset 0x18
    f32 balance;          // offset 0x1C
    f32 collideCmHeight;  // offset 0x20
    f32 cmHeight;         // offset 0x24
    f32 airDrag;          // offset 0x28
    f32 torqueBalance;    // offset 0x2C
    f32 finalDrive;       // offset 0x30
    s32 numGears;         // offset 0x34
    f32 gears[6];         // offset 0x38
    f32 torqueMax;        // offset 0x50
    f32 powerMax;         // offset 0x54
    f32 torqueRpm;        // offset 0x58
    f32 powerRpm;         // offset 0x5C
    f32 drag;             // offset 0x60
    f32 redline;          // offset 0x64
    f32 idleSpeed;        // offset 0x68
    f32 ebHeat;           // offset 0x6C
    f32 ebStrength;       // offset 0x70
    f32 jbTime;           // offset 0x74
    f32 fsway;            // offset 0x78
    f32 rsway;            // offset 0x7C
    WheelSetup wheels[4];  // offset 0x80
    f32 grip;             // offset 0x110
    f32 slip;             // offset 0x114
    f32 loadFactor;       // offset 0x118
    f32 unk11C;           // offset 0x11C
    f32 brake;            // offset 0x120
    f32 speedSteering;    // offset 0x124
    f32 wheelLock;        // offset 0x128
    ValVec3 modelOffset;  // offset 0x12C
};

struct TweakMul {
    f32 mass;     // offset 0x0
    f32 inertia;  // offset 0x4
    f32 engine;   // offset 0x8
    f32 drive;    // offset 0xC
    f32 bump;     // offset 0x10
    f32 rebound;  // offset 0x14
    f32 spring;   // offset 0x18
    f32 grip;     // offset 0x1C
    f32 slip;     // offset 0x20
};

struct Tweak {
    char name[0x20];  // offset 0x0
    TweakMul mul;     // offset 0x20
};

Tweak lbl_804A1968[32];
ModuleTag lbl_8055F620;

extern "C" {

void fn_8027A888(TweakMul* m);

void fn_80279CE4(void) {
    for (int i = 0; i < 32; i++) {
        memset(&lbl_804A1968[i], 0, sizeof(Tweak));
    }
}

void fn_80279D3C(void) {}

void fn_80279D40(CarSetup* s) {
    memset(s, 0, sizeof(CarSetup));
    s->mass = 1477.125f;
    s->mx = 2705.9114f;
    s->my = 2705.9114f;
    s->mz = 422.79865f;
    s->ftrack = 1.524f;
    s->rtrack = 1.524f;
    s->wheelbase = 2.7432f;
    s->balance = 0.5f;
    s->torqueBalance = 1.0f;
    s->finalDrive = 4.0f;
    s->numGears = 4;
    s->gears[0] = 3.8f;
    s->gears[1] = 2.2f;
    s->gears[2] = 1.65f;
    s->gears[3] = 1.15f;
    s->collideCmHeight = 0.254f;
    s->cmHeight = 0.254f;
    s->airDrag = 0.55815005f;
    s->fsway = 21023.623f;
    s->rsway = 10511.8115f;
    s->torqueMax = 339.3125f;
    s->powerMax = 223800.0f;
    s->torqueRpm = 4800.0f;
    s->powerRpm = 8000.0f;
    s->redline = 6000.0f;
    s->idleSpeed = 1000.0f;
    s->drag = 0.21601303f;
    s->ebHeat = 1.0f;
    s->ebStrength = 1.0f;
    s->jbTime = 1.0f;
    WheelDefaults(&s->wheels[0], 1);
    WheelDefaults(&s->wheels[1], 1);
    WheelDefaults(&s->wheels[2], 0);
    WheelDefaults(&s->wheels[3], 0);
    s->grip = 1.0f;
    s->slip = 1.0f;
    s->loadFactor = 1.0f;
    s->unk11C = 0.7f;
    s->brake = 2714.5f;
    s->speedSteering = 1.0f;
    s->wheelLock = 0.5235988f;
}

void fn_80279F58(CarSetup* s, ValueTable* t) {
    ValueNode* tr;
    ValueNode* c;
    int i;
    ValueNode* e;
    fn_80279D40(s);
    c = t->FindChild("Chassis");
    s->mass = Lb(c->GetFloat("mass"));
    s->mx = Ft(Ft(Lb(c->GetFloat("mx"))));
    s->my = Ft(Ft(Lb(c->GetFloat("my"))));
    s->mz = Ft(Ft(Lb(c->GetFloat("mz"))));
    s->wheelbase = In(c->GetFloat("wheelbase"));
    s->ftrack = In(c->GetFloat("ftrack"));
    s->rtrack = In(c->GetFloat("rtrack"));
    s->balance = 0.5f;
    s->cmHeight = In(c->GetFloat("cm_height"));
    s->collideCmHeight = In(c->GetFloat("collide_cm_height", 29.999998f));
    s->airDrag = c->GetFloat("air_drag", 6.0f) * 0.093025006f;

    tr = t->FindChild("Transmission");
    s->torqueBalance = tr->GetFloat("torque_balance");
    s->numGears = tr->GetIntValue("num_gears");
    for (i = 0; i < s->numGears; i++) {
        char key[0x20];
        sprintf(key, "g%d", i + 1);
        s->gears[i] = tr->GetFloat(key);
    }

    e = t->FindChild("EngineNormal");
    s->torqueMax = Ft(Lbf(e->GetFloat("torque_max")));
    s->torqueRpm = e->GetFloat("torque_rpm");
    s->powerMax = 746.0f * e->GetFloat("power_max");
    s->powerRpm = e->GetFloat("power_rpm");
    s->drag = Ft(Lbf(e->GetFloat("drag")));
    s->idleSpeed = e->GetFloat("idle_speed");
    s->redline = e->GetFloat("redline");
    s->ebHeat = e->GetFloat("eb_heat_factor", 1.0f);
    s->ebStrength = e->GetFloat("eb_strength_factor", 1.0f);
    s->jbTime = e->GetFloat("jb_time_factor", 1.0f);

    s->grip = 1.5f * Max(Min(c->GetFloat("grip", 1.1f), 1.2f), 0.05f);
    s->slip = 2.0f * c->GetFloat("slip", 1.0f);
    s->loadFactor = c->GetFloat("load_factor", 1.0f);
    s->brake *= c->GetFloat("brake_factor", 1.0f);
    s->fsway = Lbf(c->GetFloat("fsway", 120.0f)) / 0.0254f;
    s->rsway = Lbf(c->GetFloat("rsway", 60.0f)) / 0.0254f;
    f32 ride = In(c->GetFloat("ride_height", 5.0f));
    s->modelOffset.x = 0.0f;
    s->modelOffset.z = -(s->balance - 0.5f) * s->wheelbase;
    s->modelOffset.y = -s->collideCmHeight;
    if (c->Has("model_offset")) {
        const ValVec3& o = c->GetVec3("model_offset");
        f32 ox = o.x;
        s->modelOffset.x += ox * 0.0254f;
        s->modelOffset.y += o.y * 0.0254f;
        s->modelOffset.z += o.z * 0.0254f;
    }

    t->FindChild("Front");
    t->FindChild("Rear");
    for (i = 0; i < 4; i++) {
        WheelSetup* w = &s->wheels[i];
        int front = 0;
        if (i == 0 || i == 1) {
            front = 1;
        }
        f32 spring = c->GetFloat("spring_factor", 1.0f);
        f32 shock = c->GetFloat("shock_factor", 1.0f);
        f32 bump = c->GetFloat("bump_factor", 1.0f);
        f32 rebound = c->GetFloat("rebound_factor", shock);
        rebound = c->GetFloat(front ? "frebound_factor" : "rrebound_factor", rebound);
        w->unk18 = 2.0f;
        w->spring *= spring;
        w->bump *= bump;
        w->rebound *= rebound;
        w->rideHeight = ride;
        w->bumpstopHeight = In(c->GetFloat("bumpstop_height", 11.0f));
        w->bumpstopRate = Lbf(c->GetFloat("bumpstop_rate", 0.0f)) / 0.0254f;
    }
    s->speedSteering = c->GetFloat("speed_sensitive_steering", 0.65f);
    s->wheelLock = 0.017453292f * c->GetFloat("wheel_lock", 27.5f);
}

void fn_8027A714(CarSetup* s, const char* name) {
    char path[0x100];
    sprintf(path, "%s.car/physics.val", name);
    ValueTable* t = fn_80131808(path);
    if (!t) {
        fn_802E317C("carsetup: can't get %s", path);
    }
    fn_80279F58(s, t);
    fn_80131818(t);
}

TweakMul* fn_8027A798(const char* name) {
    Tweak* t = lbl_804A1968;
    for (int i = 0; i < 32; i++, t++) {
        if (strcmp(t->name, name) == 0) {
            return &t->mul;
        }
    }
    t = lbl_804A1968;
    for (int i = 0; i < 32; i++, t++) {
        if (!t->name[0]) {
            CopyName(t->name, name, 0x20);
            fn_8027A888(&t->mul);
            return &t->mul;
        }
    }
    fn_802E317C("carsetup: out of tweaks!");
    return 0;
}

void fn_8027A888(TweakMul* m) {
    m->mass = 1.0f;
    m->inertia = 1.0f;
    m->engine = 1.0f;
    m->drive = 1.0f;
    m->bump = 1.0f;
    m->rebound = 1.0f;
    m->spring = 1.0f;
    m->grip = 1.0f;
    m->slip = 1.0f;
}

void fn_8027A8B4(CarSetup* s, const TweakMul* m) {
    s->mass *= m->mass;
    s->mx *= m->inertia;
    s->my *= m->inertia;
    s->mz *= m->inertia;
    s->powerMax *= m->engine;
    s->torqueMax *= m->engine;
    s->finalDrive *= m->drive;
    s->grip *= m->grip;
    s->slip *= m->slip;
    for (int i = 0; i < 4; i++) {
        s->wheels[i].spring *= m->spring;
        s->wheels[i].bump *= m->bump;
        s->wheels[i].rebound *= m->rebound;
    }
}

}  // extern "C"
