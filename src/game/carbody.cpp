// Car body (0x8018185C-0x8018BFA0): the truck's chassis, suspension and wheel setup ("Chassis",
// "Front", "Rear", "susp_y", "squishy", "car_box", "car-volume"), terrain morph kicks and the
// per-car tuning accessors. Written against raw field offsets; the large routines are still stubs.
// Names are guesses apart from the strings.
#include "game/common.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define U16(p, o) (*(u16*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

class VR {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(void);
    virtual void v1C(void);
    virtual void v20(void);
    virtual void v24(void);
    virtual void v28(void);
    virtual void v2C(void);
    virtual void v30(void);
    virtual void v34(void);
    virtual void v38(void);
};

// A large interface reached through slot 0xCC.
class VBig {
public:
    virtual void s08(void);
    virtual void s0C(void);
    virtual void s10(void);
    virtual void s14(void);
    virtual void s18(void);
    virtual void s1C(void);
    virtual void s20(void);
    virtual void s24(void);
    virtual void s28(void);
    virtual void s2C(void);
    virtual void s30(void);
    virtual void s34(void);
    virtual void s38(void);
    virtual void s3C(void);
    virtual void s40(void);
    virtual void s44(void);
    virtual void s48(void);
    virtual void s4C(void);
    virtual void s50(void);
    virtual void s54(void);
    virtual void s58(void);
    virtual void s5C(void);
    virtual void s60(void);
    virtual void s64(void);
    virtual void s68(void);
    virtual void s6C(void);
    virtual void s70(void);
    virtual void s74(void);
    virtual void s78(void);
    virtual void s7C(void);
    virtual void s80(void);
    virtual void s84(void);
    virtual void s88(void);
    virtual void s8C(void);
    virtual void s90(void);
    virtual void s94(void);
    virtual void s98(void);
    virtual void s9C(void);
    virtual void sA0(void);
    virtual void sA4(void);
    virtual void sA8(void);
    virtual void sAC(void);
    virtual void sB0(void);
    virtual void sB4(void);
    virtual void sB8(void);
    virtual void sBC(void);
    virtual void sC0(void);
    virtual void sC4(void);
    virtual void sC8(void);
    virtual int sCC(s32 id);
};

class VG {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual u8* v10(void);
};

// Two tuning values set together.
struct F32Pair {
    f32 a;  // offset 0x0
    f32 b;  // offset 0x4
};

struct RVec3;

// A 12-word body pose.
struct CbPose {
    u32 w[12];
};

class CollVolume {
public:
    virtual ~CollVolume(void);
    void Update(void);
};

extern "C" {
void fn_801838C8(u8* p);
void fn_801A1474(u8* p, f32 s);
void fn_8018A180(u8* p);
u8* fn_801D2008(u8* name);
s32 fn_801C1C1C(void);
s32 fn_801C1BA4(s32 a, s32 b, s32 c);
}

class RigidBody : public CollVolume {
public:
    RigidBody(void);
    void Reset(void);
    void OnHit(const RVec3* a, const RVec3* b, int c, void* d);
    void SetMass(f32 m, f32 ix, f32 iy, f32 iz);
    int vf34(struct RPacket* pk);
    void ClearImpulses(void);
};

class CarBody : public RigidBody {
public:
    CarBody(void);
    virtual ~CarBody(void);
};

// A block of tuning values.
struct TuneBlock {
    f32 v[16];
};

// The car body's tuning: the live bodies, then the landing, drive and heat (boost) tunables.
struct LandingTune {
    f32 v[10];
    void Reset(void) {
        v[0] = 2.5f;
        v[1] = 2.0f;
        v[2] = 0.5f;
        v[3] = 1.0f;
        v[6] = 2.0f;
        v[7] = 180.0f;
        v[8] = 100.0f;
        v[4] = 0.975f;
        v[5] = 0.975f;
    }
};

struct DriveTune {
    f32 v[8];
    void Reset(void) {
        v[0] = 6.0f;
        v[1] = 1.0f;
        v[2] = 2.0f;
        v[3] = 1.0f;
        v[6] = 1.5f;
        v[7] = 180.0f;
        v[4] = 0.975f;
    }
};

struct HeatTune {
    f32 v[16];
    void Reset(f32 burn, int e3) {
        v[0] = 0.05f * (e3 ? 1.5f : 1.0f);
        v[1] = 0.1f * (e3 ? 1.5f : 1.0f);
        v[2] = 2.0f;
        v[3] = burn;
        v[4] = 1.0f;
        v[5] = 3.7f;
        v[6] = 0.625f;
        v[7] = 0.0f;
        v[8] = 6.25f;
        v[10] = 0.5f;
        v[9] = 1.0f;
        v[11] = 145.0f;
        v[12] = 1.0f;
        v[14] = 1.0f;
        v[15] = 0.5f;
        v[13] = 170.0f;
    }
};


// A handle whose constructor marks it unused.
struct CarHandle {
    s32 v;
    CarHandle(void) { v = 0xFFFF0000; }
};

extern "C" {
void fn_8018C510(void);
void fn_801AFC38(void);
void fn_801BD790(void);
void fn_801BF1B8(u8* p, int a);
int fn_801B2F2C(u8* p);
void fn_801C1C7C(u8* p);
void fn_801D21B4(u8* p);
void fn_801C2BF8(u8* p, f32* m);
f32 fn_80181AEC(void);
void fn_8018A8A8(u8* p);
void fn_801D1184(void);
f32 fn_80129DC4(int stream, f32 lo, f32 hi);
f32 fn_802798E8(u8* p);
void fn_802799EC(u8* p, f32 v);
u8* fn_801769CC(void);
int fn_801769C4(void);
void fn_801BD694(int n);
void fn_801AFC28(void);
void fn_80181AFC(void);
void fn_80204A1C(const char* name, u8* flag);
void fn_8018C50C(void);
void fn_8018F538(u8* p, u8* q);
void fn_80279C00(u8* p, u8* q);
void fn_80279B28(u8* p, u8* q);
VBig* fn_801B83A0(void);
void fn_801D14EC(void);
void fn_801BD808(void);
}

u8* lbl_804348E8[8];             // the live car bodies (aicar and betarace read them too)
static LandingTune landingTune;  // 0x80434908
static DriveTune driveTune;      // 0x80434930
static HeatTune heatTune;        // 0x80434950
extern u8 lbl_8055E4F4;
extern f32 lbl_8055E3C8;
extern F32Pair lbl_8055E4E0;
extern u8* lbl_8055E918;
extern s32 lbl_8055E4D0;
extern s32 lbl_8055E4D4;
extern u8 lbl_8055E4F6;
f32 lbl_8055A3A0 = 1.0f;  // physics writes it
f32 lbl_8055A3A4 = 0.9f;  // offroad speed factor
f32 lbl_8055A3A8 = 0.9f;  // water speed factor
u8 lbl_8055A3AC = 1;      // enable stalling
f32 lbl_8055A3B0 = 1.5f;
u8 lbl_8055A3B4 = 1;
u8 lbl_8055A3B5 = 1;      // reset the tunables to their defaults
u8 lbl_8055A3B6 = 1;

extern f32 lbl_80558DC4[];  // FLT_EPSILON

static inline int Positive(f32 x) { return x > lbl_80558DC4[0]; }

ModuleTag lbl_8055E4C0;
CarHandle lbl_8055E4CC;
UnkStatic7F lbl_8055E4C8;

extern "C" {

}  // extern "C"

extern const f32 lbl_80561CA0;  // torque unit scale
extern const f32 lbl_80561CA4;  // 1e-5
extern const f32 lbl_80561CA8;  // 5252 (hp = torque * rpm / 5252)
extern f32 lbl_80561CAC;        // 0.0
extern const f32 lbl_80561CB0;  // -2.0
extern const f32 lbl_80561CB4;  // 8000
extern const f32 lbl_80561CB8;  // 0.3
extern const f32 lbl_80561CBC;  // 1e-4

static inline f32 FMax(f32 a, f32 b) { return a > b ? a : b; }

extern "C" {

// Builds the engine torque curve from v = {hp, hp rpm, peak torque, torque rpm, redline rpm, idle torque}: a
// parabola through the torque peak and the hp point below it, another from the peak down to 30% at redline.
void fn_8018185C(u8* p, f32* v) {
    F32(p, 0x1C) = v[1];
    F32(p, 0x28) = v[3];
    F32(p, 0x18) = lbl_80561CA0 * v[5];
    f32 hp = lbl_80561CA0 * (lbl_80561CA8 * v[0] / FMax(v[1], lbl_80561CA4));
    F32(p, 0x24) = lbl_80561CAC;
    f32 peak = lbl_80561CA0 * v[2];
    f32 d = F32(p, 0x1C) - v[3];
    f32 a = (hp - peak) / FMax(d * d, lbl_80561CA4);
    f32 top = F32(p, 0x28);
    f32 red = v[4];
    f32 low = lbl_80561CA0 * (lbl_80561CB8 * v[2]);
    f32 peak2 = lbl_80561CA0 * v[2];
    F32(p, 0x20) = hp / lbl_80561CB4;
    F32(p, 0x0) = a;
    F32(p, 0x4) = F32(p, 0x18) + lbl_80561CB0 * a * v[3];
    F32(p, 0x8) = peak + a * (v[3] * v[3]);
    d = red - top;
    f32 b = (low - peak2) / FMax(d * d, lbl_80561CBC);
    F32(p, 0xC) = b;
    F32(p, 0x14) = peak2 + b * (top * top);
    F32(p, 0x10) = F32(p, 0x18) + lbl_80561CB0 * b * top;
}

extern const f32 lbl_80561CC4;
extern const f32 lbl_80561CC8;

void fn_801819A0(u8* p, u8* cfg, f32 a, f32 b, f32 c) {
    f32 v[6];
    v[0] = F32(cfg, 0x54) * a / lbl_80561CC4;
    v[1] = F32(cfg, 0x5C) * c;
    v[2] = F32(cfg, 0x50) * a / lbl_80561CC8;
    v[3] = F32(cfg, 0x58) * b;
    v[4] = F32(cfg, 0x68);
    v[5] = F32(cfg, 0x60);
    fn_8018185C(p, v);
}

f32 fn_80181A14(u8* p) {
    f32 k;
    if (U8(p, 0x259D)) {
        k = driveTune.v[6];
    } else {
        k = landingTune.v[6];
    }
    return F32(p, 0x2D8) * k;
}

f32 fn_80181A48(u8* p) {
    f32 k;
    if (U8(p, 0x259D)) {
        k = driveTune.v[7];
    } else if (lbl_8055E4F4) {
        k = heatTune.v[13];
    } else {
        k = landingTune.v[7];
    }
    return F32(p, 0x2D0) + k;
}

f32 fn_80181A94(void) {
    if (lbl_8055E4F4) {
        return heatTune.v[14];
    }
    return landingTune.v[2];
}

f32 fn_80181AC0(void) {
    if (lbl_8055E4F4) {
        return heatTune.v[15];
    }
    return landingTune.v[3];
}

f32 fn_80181AEC(void) { return driveTune.v[3]; }

extern f32 lbl_8055E4F0;        // bad landing bounce time
extern f32 lbl_8055E4E8[2];
extern f32 lbl_8055E4D8;
int OptE3Demo(void);
extern "C" {
void fn_802049DC(const char* name, f32* var, f32 lo, f32 hi, f32 step);  // debug tunable
int fn_8020503C(const char* name);                                       // debug switch
}

// Resets the car body tunables (when asked) and registers them with the debug tuner.
void fn_80181AFC(void) {
    if (lbl_8055A3B5) {
        landingTune.Reset();
        driveTune.Reset();
        lbl_8055E4E8[0] = 30.0f;
        lbl_8055E4E8[1] = 2.0f;
    }
    fn_802049DC("Bad landing bounce time", &lbl_8055E4F0, 0.0f, 5.0f, 0.25f);
    fn_802049DC("Offroad speed factor", &lbl_8055A3A4, 0.1f, 1.0f, 0.1f);
    fn_802049DC("Water speed factor", &lbl_8055A3A8, 0.1f, 1.0f, 0.1f);
    int e3 = OptE3Demo();
    f32 burn = 0.4f * (e3 ? 0.625f : 1.0f);
    if (lbl_8055A3B5) {
        heatTune.Reset(burn, e3);
        lbl_8055E4D8 = 1.7f;
    }
    if (U32(fn_801769CC(), 0x4) & 0x100000) {
        fn_802049DC("OK-Landing Min Air-time Required", &landingTune.v[1], 0.0f, 1.5f, 0.1f);
        fn_802049DC("OK-Landing Max Speed", &landingTune.v[7], 140.0f, 200.0f, 5.0f);
        fn_802049DC("OK-Landing Boost Strength", &landingTune.v[6], 0.25f, 6.0f, 0.25f);
        fn_802049DC("OK-Landing Boost Time =", &landingTune.v[3], 0.0f, 4.0f, 0.25f);
        fn_802049DC("OK-Landing Boost Time += (Air-time X", &landingTune.v[2], 0.0f, 4.0f, 0.25f);
        fn_802049DC("OK Landing Max Boost Time", &landingTune.v[0], 1.0f, 12.0f, 1.0f);
    }
    if (U32(fn_801769CC(), 0x4) & 0x80000) {
        fn_802049DC("TEMP fade rate (ground)", &heatTune.v[0], 0.0f, 2.0f, 0.05f);
        fn_802049DC("TEMP fade rate (air)", &heatTune.v[1], 0.0f, 2.0f, 0.05f);
        fn_802049DC("TEMP burn rate", &heatTune.v[3], 0.0f, 2.0f, 0.05f);
        fn_802049DC("Boost strength", &heatTune.v[5], 0.25f, 6.0f, 0.25f);
        fn_802049DC("Overheat time", &heatTune.v[8], 0.0f, 10.0f, 0.25f);
        fn_802049DC("Overheat braking", &heatTune.v[10], 0.0f, 1.0f, 0.1f);
        fn_802049DC("Overheat throttle", &heatTune.v[9], 0.0f, 1.0f, 0.1f);
        fn_802049DC("Overheat target speed", &heatTune.v[11], 0.0f, 180.0f, 5.0f);
        fn_802049DC("Nonboost slowdown", &heatTune.v[12], 0.0f, 10.0f, 0.25f);
    }
    if (fn_8020503C("unlimited_turbo")) {
        heatTune.v[3] = 0.0f;
    } else {
        heatTune.v[3] = burn;
    }
    lbl_8055A3B5 = 0;
}

void fn_80181EAC(u8* p) {
    f32 a = fn_80129DC4(4, 0.6f, 0.85f);
    f32 b = fn_80129DC4(4, 0.05f, 0.25f);
    f32 v = fn_802798E8(p);
    if (v > a && v > b) {
        fn_802799EC(p, v - b);
    }
}

void fn_80181F30(void) {
    u32 flags = U32(fn_801769CC(), 0x4);
    lbl_8055E4F4 = (flags >> 19) & 1;
    lbl_8055E4D0 = 0;
    fn_801BD694(8);
    fn_801AFC28();
    lbl_8055E4D4 = 1;
    fn_80181AFC();
    if (lbl_8055A3B6) {
        lbl_8055E4E0.a = 10.0f;
        lbl_8055E4E0.b = 2.0f;
    }
    lbl_8055A3B6 = 0;
    fn_80204A1C("Enable Stalling", &lbl_8055A3AC);
    lbl_8055E4F6 = 0;
    fn_8018C50C();
}

void fn_80181FC4(void) {
    fn_8018C510();
    fn_801AFC38();
    fn_801BD790();
}

void fn_80181FEC(void) {
    fn_801D1184();
    fn_801D14EC();
}

}  // extern "C"

CarBody::CarBody(void) {
    *(s16*)((u8*)this + 0x2B4) = -1;
    fn_801BD808();
    lbl_804348E8[lbl_8055E4D0++] = (u8*)this;
}

extern "C" {

void fn_80182074(u8* p) {}  // stub (864)

void fn_80182DF4(u8* p) {}  // stub (570)

void fn_801836DC(void) {}

}  // extern "C"

class CarHost {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(void);
    virtual void v1C(void);
    virtual void v20(void);
    virtual void v24(void);
    virtual void v28(void);
    virtual void v2C(void);
    virtual void v30(void);
    virtual void Unimpair(void);   // 0x34
    virtual int IsImpaired(void);  // 0x38
};

struct BVec {
    BVec(void) {}
    BVec(f32 a, f32 b, f32 c) : x(a), y(b), z(c) {}
    f32 x, y, z;
};

static inline void Scale(BVec* o, const BVec* a, f32 k) {
    o->x = a->x * k;
    o->y = a->y * k;
    o->z = a->z * k;
}

struct CVec4 {
    CVec4(void) {}
    CVec4(f32 x_, f32 y_, f32 z_, f32 w_) {
        x = x_;
        y = y_;
        z = z_;
        w = w_;
    }
    CVec4(const CVec4& o) {
        x = o.x;
        y = o.y;
        z = o.z;
        w = o.w;
    }
    f32 x, y, z, w;
};

class CarGear {
public:
    void Fetch(int id);
    void Clear(int id);
};

// A sub-object owned by the body, destroyed through its virtual destructor.
class CbPart {
public:
    virtual ~CbPart(void);
    virtual void Update(int impaired);  // 0x0C
};

// Body effects at 0x2580/0x2584, updated every frame.
class CbFx {
public:
    virtual ~CbFx(void);
    virtual void Update(void);  // 0x0C
};

extern "C" void fn_801D1100(u8* p);
extern "C" void fn_802792EC(u8* p);
extern "C" void fn_8018A6F4(u8* p);

CarBody::~CarBody(void) {
    fn_801D1100(PTR(this, 0x1EC));
    if (*(CbPart**)((u8*)this + 0x257C)) {
        delete *(CbPart**)((u8*)this + 0x257C);
    }
    if (*(CbPart**)((u8*)this + 0x2580)) {
        delete *(CbPart**)((u8*)this + 0x2580);
    }
    delete *(CbPart**)((u8*)this + 0x2584);
    fn_8018A6F4((u8*)this);
    fn_802792EC(PTR(this, 0x26C));
    lbl_8055E4D0--;
}

extern "C" {

u8* fn_801837DC(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_8018381C(u8* p) {
    *(CbPose*)(p + 0x3C) = *(CbPose*)(p + 0x274);
    ((CollVolume*)p)->Update();
    ((RigidBody*)p)->Reset();
    fn_801838C8(p);
}

void fn_801838C8(u8* p) {}  // stub (332)

void fn_80183DF8(u8* p) {
    if (PTR(p, 0x328)) {
        fn_8018F538(p, p + 0x1E44);
        fn_80279C00(PTR(p, 0x328), p + 0x1E44);
        if (fn_801769C4() == 2 && U32(p, 0x10) != 'PCAR' && !(U8(p, 0x260) & 4)) {
            fn_80181EAC(PTR(p, 0x328));
            fn_80279B28(PTR(p, 0x328), p + 0x1E44);
        }
    }
}

void fn_80183E7C(u8* p) {}  // stub (3032)

u32 fn_80186DDC(u8* p) { return 0; }

void fn_80186DE4(u8* p) { fn_8018A8A8(p); }

void fn_80186DE8(u8* p) {}  // stub (1830)

void vf20__10CollVolumeFv(u8* p);
extern f32 lbl_80561CAC;  // 0.0
extern f32 lbl_80561CDC;  // 0.5
extern f32 lbl_80561D90;  // -5.0
extern f32 lbl_80561D9C;  // 1/60 s
extern f32 lbl_80561DE4;  // -1.0
extern f32 lbl_80561E5C;  // -1.5
extern f32 lbl_80561EE0;  // 0.5/60
extern f32 lbl_80561EE4;  // -100.0
extern f32 lbl_80561EE8;  // 0.95
extern f32 lbl_80561EEC;  // 1 - 0.95
extern f32 lbl_80561EF0;  // 15 degrees
extern f32 lbl_80561EF4;  // -2.5 degrees

static inline f32 SwayMin(f32 a, f32 b) { return a < b ? a : b; }
static inline f32 SwayMax(f32 a, f32 b) { return a > b ? a : b; }
static inline f32 SwayDot(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

// o = a + b * k
static inline void AddScaled(BVec* o, const BVec* a, const BVec* b, f32 k) {
    o->x = a->x + b->x * k;
    o->y = a->y + b->y * k;
    o->z = a->z + b->z * k;
}

// v times the body rotation (rows at 0x3C).
static inline void SwayRotate(BVec* out, const BVec* v, const f32* m) {
    if (out == v) {
        f32 x = SwayDot(v->x, m[0], v->y, m[1], v->z, m[2]);
        f32 y = SwayDot(v->x, m[3], v->y, m[4], v->z, m[5]);
        f32 z = SwayDot(v->x, m[6], v->y, m[7], v->z, m[8]);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = SwayDot(v->x, m[0], v->y, m[1], v->z, m[2]);
        out->y = SwayDot(v->x, m[3], v->y, m[4], v->z, m[5]);
        out->z = SwayDot(v->x, m[6], v->y, m[7], v->z, m[8]);
    }
}

// Per-frame body update: the effect parts, the settle timer, and the body sway, a damped spring
// (velocity 0x2654, angle 0x2660) driven by the filtered body-space acceleration (0x266C); only the
// pitch is kept, held between -2.5 and 15 degrees.
void fn_80188A80(u8* p) {
    BVec a;
    BVec f;
    vf20__10CollVolumeFv(p);
    if (*(CbPart**)(p + 0x257C)) {
        (*(CbPart**)(p + 0x257C))->Update(((CarHost*)PTR(p, 0x270))->IsImpaired());
    }
    if (*(CbFx**)(p + 0x2580)) {
        (*(CbFx**)(p + 0x2580))->Update();
    }
    if (*(CbFx**)(p + 0x2584)) {
        (*(CbFx**)(p + 0x2584))->Update();
    }
    if (F32(p, 0x269C) < lbl_80561CDC) {
        F32(p, 0x269C) = F32(p, 0x269C) + lbl_80561D9C;
    } else {
        F32(p, 0x2694) = SwayMin(F32(p, 0x2690), lbl_80561EE0 + F32(p, 0x2694));
    }
    BVec* vel = (BVec*)(p + 0x2654);
    BVec* ang = (BVec*)(p + 0x2660);
    BVec* acc = (BVec*)(p + 0x266C);
    SwayRotate(&a, (BVec*)(p + 0x16C), (f32*)(p + 0x3C));
    Scale(&a, &a, lbl_80561DE4);
    AddScaled(&f, &a, acc, lbl_80561E5C);
    AddScaled(&f, &f, vel, lbl_80561D90);
    AddScaled(&f, &f, ang, lbl_80561EE4);
    AddScaled(vel, vel, &f, lbl_80561D9C);
    AddScaled(ang, ang, vel, lbl_80561D9C);
    Scale(acc, acc, lbl_80561EE8);
    AddScaled(acc, acc, &a, lbl_80561EEC);
    acc->z = lbl_80561CAC;
    acc->y = lbl_80561CAC;
    if (ang->z < lbl_80561EF4) {
        ang->z = lbl_80561EF4;
        vel->z = SwayMax(vel->z, lbl_80561CAC);
    }
    ang->z = SwayMin(lbl_80561EF0, ang->z);
}

u32 fn_80188E24(u8* p) { return 0; }

void fn_80188E2C(u8* p) {}  // stub (464)

void fn_8018956C(u8* p) {}  // stub (412)

int fn_80189BDC(u8* p) {
    if (U32(p, 0x10) != 'TRLR') {
        u8* o = ((VG*)PTR(p, 0x270))->v10();
        if (o && (S32(o, 0x24) == 1 || S32(o, 0x24) == 3)) {
            return 0;
        }
    }
    return fn_801B83A0()->sCC(S32(p, 0x1EC));
}

extern f32 lbl_80561CAC;  // 0.0

void fn_80189C64(u8* p) {
    if (U8(p, 0x26AC)) {
        U8(p, 0x26AC) = 0;
        fn_801BF1B8(PTR(p, 0x1EC), 0);
    }
    F32(p, 0x26A8) = lbl_80561CAC;
}

// A collision shape of the body (slots at 0x14, count at 0x24).
class CbShape {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(void);
    virtual void v1C(void);
    virtual void v20(void);
    virtual void v24(void);
    virtual void v28(void);
    virtual void v2C(void);
    virtual void v30(void);
    virtual void v34(void);
    virtual void v38(void);
    virtual void v3C(void);
    virtual void v40(void);
    virtual void v44(void);
    virtual void Reset(void);  // 0x48
};

static inline CbShape* ShapeAt(u8* p, int i) {
    if (i >= 0 && i < S32(p, 0x24)) {
        return ((CbShape**)(p + 0x14))[i];
    }
    return 0;
}

// A 3x4 frame: rotation rows, then the position.
struct CbFrame {
    BVec rows[3];
    BVec pos;
};

static inline f32 PlaceDot(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

static inline void PlaceRotate(BVec* out, const BVec* v, const CbFrame* m) {
    if (out == v) {
        f32 x = PlaceDot(v->x, m->rows[0].x, v->y, m->rows[1].x, v->z, m->rows[2].x);
        f32 y = PlaceDot(v->x, m->rows[0].y, v->y, m->rows[1].y, v->z, m->rows[2].y);
        f32 z = PlaceDot(v->x, m->rows[0].z, v->y, m->rows[1].z, v->z, m->rows[2].z);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = PlaceDot(v->x, m->rows[0].x, v->y, m->rows[1].x, v->z, m->rows[2].x);
        out->y = PlaceDot(v->x, m->rows[0].y, v->y, m->rows[1].y, v->z, m->rows[2].y);
        out->z = PlaceDot(v->x, m->rows[0].z, v->y, m->rows[1].z, v->z, m->rows[2].z);
    }
}

void fn_801E9DD0(s16* surface, f32 x, f32 z);
void fn_801BE400(s32 car, CbFrame* frame, f32 a);
void fn_801B30C0(s32 car, void* start, s32 reset);
void fn_8026A394(u8* p, s32 n, s32 c);
f32 PSVECDotProduct(const void* a, const void* b);
f32 PSVECMag(const void* v);
void fn_8018A750(u8* p);

// Puts the car down at `s` (a frame for the body origin, the velocity at 0x30, the start point at
// 0x3C and the surface at 0x40) at rest: clears the impulses, wheel loads and gear state.
void fn_80189CB4(u8* p, u8* s) {
    CbFrame frame;
    BVec off;
    BVec rotated;
    F32(p, 0x130) = 0.0f;
    F32(p, 0x134) = 0.0f;
    F32(p, 0x138) = 0.0f;
    F32(p, 0x13C) = 0.0f;
    F32(p, 0x140) = 0.0f;
    F32(p, 0x144) = 0.0f;
    ((RigidBody*)p)->ClearImpulses();
    fn_80189C64(p);
    frame = *(CbFrame*)s;
    Scale(&off, (BVec*)(p + 0x30), lbl_80561DE4);
    PlaceRotate(&rotated, &off, &frame);
    frame.pos.x = frame.pos.x + rotated.x;
    frame.pos.y = frame.pos.y + rotated.y;
    frame.pos.z = frame.pos.z + rotated.z;
    *(CbFrame*)(p + 0x3C) = frame;
    ((CollVolume*)p)->Update();
    f32 vx = F32(s, 0x30);
    f32 vy = F32(s, 0x34);
    f32 vz = F32(s, 0x38);
    F32(p, 0x2A4) = vx;
    F32(p, 0x2A8) = vy;
    F32(p, 0x2AC) = vz;
    F32(p, 0x118) = vx;
    F32(p, 0x11C) = vy;
    F32(p, 0x120) = vz;
    F32(p, 0x124) = 0.0f;
    F32(p, 0x128) = 0.0f;
    F32(p, 0x12C) = 0.0f;
    F32(p, 0x25D8) = F32(p, 0x25CC) = PSVECDotProduct(p + 0x118, p + 0x54);
    F32(p, 0x261C) = F32(p, 0x25D4) = PSVECMag(p + 0x118);
    F32(p, 0x16C) = 0.0f;
    F32(p, 0x170) = 0.0f;
    F32(p, 0x174) = 0.0f;
    F32(p, 0x26B4) = 0.0f;
    F32(p, 0x26B8) = 0.0f;
    F32(p, 0x26BC) = 0.0f;
    *(s16*)(p + 0x6C) = *(s16*)(s + 0x40);
    fn_801E9DD0((s16*)(p + 0x6C), frame.pos.x, frame.pos.z);
    ((CarGear*)(lbl_8055E918 + S32(p, 0x1EC) * 0x9C))->Clear(S32(p, 0x1EC));
    fn_801BE400(S32(p, 0x1EC), &frame, F32(p, 0x25CC));
    fn_801B30C0(S32(p, 0x1EC), 0, S32(s, 0x3C));
    for (int i = 0; i < S32(p, 0x24); i++) {
        ShapeAt(p, i)->Reset();
    }
    for (int w = 0; w < 4; w++) {
        F32(p, 0x23AC + w * 0x74) = 0.0f;
        F32(p, 0x2414 + w * 0x74) = 0.0f;
    }
    fn_8018A750(p);
    s32 n = S32(p, 0x26D4);
    S32(p, 0x2638) = 0;
    F32(p, 0x2628) = 0.0f;
    F32(p, 0x262C) = 0.0f;
    U8(p, 0x2640) = 0;
    S32(p, 0x26D4) = n + 1;
    fn_8026A394(p, n, 0);
}

void fn_8018A088(u8* p) {}

void fn_8018A08C(u8* p) { F32(p, 0x26D8) = lbl_8055E3C8; }

void fn_8018A098(u8* p, f32 t) {
    U8(p, 0x259D) = 1;
    f32 m = F32(p, 0x2CC) * fn_80181AEC();
    F32(p, 0x258C) = F32(p, 0x3A0) * (m + t * (driveTune.v[0] - m));
}

int fn_8018A100(u8* p) { return F32(p, 0x2618) > driveTune.v[1]; }

void fn_8018A120(u8* p) {
    if (!fn_801B2F2C(PTR(p, 0x1EC))) {
        F32(p, 0x25BC) = lbl_8055E4E0.a;
        ((VR*)PTR(p, 0x2584))->v38();
    }
}

void fn_8018A174(u8* p) { F32(p, 0x2650) = 4.0f; }

}  // extern "C"

class CarElem {
public:
    void Fetch(int id);
};



extern u8* lbl_8055E908;

extern "C" {
void fn_8018F2C0(u8* p, u8* q);
void fn_8018AF04(u8* p);
void fn_802E30C8(const char* fmt, ...);
void vf20__10CollVolumeFv(u8* p);
void fn_801BDA74(s32 id, u8* mass);
extern const f32 lbl_80561D54;
extern const f32 lbl_80561D58;
extern const f32 lbl_80561CD0;

// Re-applies the tuned body setup (mass, inertia, elements, gearing) after a change.
void fn_8018A180(u8* p) {
    if (PTR(p, 0x328)) {
        fn_80279B28(PTR(p, 0x328), p + 0x1E44);
        fn_8018F2C0(p, p + 0x1E44);
        if (S32(p, 0x20DC) < 9 && ((CarHost*)PTR(p, 0x270))->IsImpaired()) {
            fn_802E30C8("car: unimpairing car %d", S32(p, 0x1EC));
            ((CarHost*)PTR(p, 0x270))->Unimpair();
            fn_8018AF04(p);
        }
    }
    F32(p, 0x30) = F32(p, 0x458);
    F32(p, 0x34) = F32(p, 0x45C);
    F32(p, 0x38) = F32(p, 0x460);
    vf20__10CollVolumeFv(p);
    ((RigidBody*)p)->SetMass(F32(p, 0x32C) / lbl_80561D54, F32(p, 0x330) / lbl_80561D58, F32(p, 0x334) / lbl_80561D58,
                             F32(p, 0x338) / lbl_80561D58);
    F32(p, 0xD4) = F32(p, 0x32C);
    F32(p, 0xE4) = lbl_80561CD0 / F32(p, 0x32C);
    fn_801BDA74(S32(p, 0x1EC), p + 0x32C);
    ((CarElem*)(lbl_8055E908 + S32(p, 0x1EC) * 0x34))->Fetch(S32(p, 0x1EC));
    ((CarGear*)(lbl_8055E918 + S32(p, 0x1EC) * 0x9C))->Fetch(S32(p, 0x1EC));
}

void fn_8018A2BC(u8* p) {}

// Physics tuning sliders: a sink taking (label, value, units, min, max, step).
struct TuneSink {
    virtual void Add(void* name, void* value, int* units, float lo, float hi, float step);
};

extern char lbl_803322B0[];  // carbody string pool ("Yaw Moment" at +0x350 ...)
// Unit strings, each a small .sdata object.
extern int lbl_8055A3F8;  // " m"
extern int lbl_8055A3FC;  // " kg"
extern int lbl_8055A400;  // " kg m2"
extern int lbl_8055A408;  // " N/m"
extern int lbl_8055A410;  // " N-s/m"
extern int lbl_8055A418;  // "Mass"
extern int lbl_8055A420;  // ""
extern int lbl_8055A424;  // " m/s"
extern int lbl_8055A42C;  // " G"
extern f32 lbl_80561CAC, lbl_80561CCC, lbl_80561CDC, lbl_80561CE4, lbl_80561CEC, lbl_80561CF8,
    lbl_80561D00, lbl_80561D0C, lbl_80561D28, lbl_80561D34, lbl_80561D44, lbl_80561D68, lbl_80561DE4, lbl_80561EBC,
    lbl_80561F0C, lbl_80561F10, lbl_80561F14, lbl_80561F18;

// Registers the body's tunables (mass, moments, power, geometry, springs, damping, tires) with a tuning
// menu. Matched by the upstream project (orangedude27/excite-truck).
// MATCHING-AID: register pinning + scheduling/opt_propagation pragmas, not original source. Revisit.
// Natural form found 2026-10-27: string literals for the names, units held in locals
// (const char* m = " m", kg = " kg", kgm2 = " kg m2", nm = " N/m", nsm = " N-s/m"; "Mass", "", " m/s", " G"
// inline), u8* body = p + 0x32C, Add(const char*, void*, const char*, ...), no pragmas. That differs only in
// the name pool offsets (literals start at +0x1F0 instead of +0x350) until the unit's earlier strings exist.
#pragma push
#pragma scheduling invalid_target
#pragma opt_propagation off
void fn_8018A2C0(u8* p, TuneSink* t) {
    register char* base = (char*)p;
    register char* names = lbl_803322B0;
    register char* offset = base + 0x32C;
    register int* m = &lbl_8055A3F8;
    register int* kg = &lbl_8055A3FC;
    register int* kgm2 = &lbl_8055A400;
    register int* nm = &lbl_8055A408;
    register int* nsm = &lbl_8055A410;
    register int* mass = &lbl_8055A418;
    register TuneSink* obj = t;

    obj->Add(mass, offset, kg, lbl_80561CE4, lbl_80561F0C, lbl_80561F10);
    obj->Add(names + 0x350, offset + 8, kgm2, lbl_80561CE4, lbl_80561D68, lbl_80561F10);    // Yaw Moment
    obj->Add(names + 0x35C, offset + 4, kgm2, lbl_80561CE4, lbl_80561D68, lbl_80561F10);    // Pitch Moment
    obj->Add(names + 0x36C, offset + 0xC, kgm2, lbl_80561CE4, lbl_80561D68, lbl_80561F10);  // Roll Moment
    obj->Add(names + 0x378, base + 0x2DC, &lbl_8055A420, lbl_80561D00, lbl_80561D44, lbl_80561D0C);  // Engine Power
    obj->Add(names + 0x388, offset + 0x30, &lbl_8055A420, lbl_80561CDC, lbl_80561CEC, lbl_80561D0C);  // Final Drive
    obj->Add(names + 0x394, base + 0x5A8, &lbl_8055A424, lbl_80561D34, lbl_80561D28, lbl_80561CD0);  // Speed Limiter
    obj->Add(names + 0x3A4, offset + 0x18, m, lbl_80561D00, lbl_80561CF8, lbl_80561CCC);    // Wheelbase
    obj->Add(names + 0x3B0, offset + 0x10, m, lbl_80561D00, lbl_80561CF8, lbl_80561CCC);    // Front Track
    obj->Add(names + 0x3BC, offset + 0x14, m, lbl_80561D00, lbl_80561CF8, lbl_80561CCC);    // Rear Track
    obj->Add(names + 0x3C8, offset + 0x20, m, lbl_80561DE4, lbl_80561CD0, lbl_80561CCC);    // CG Height
    obj->Add(names + 0x3D4, offset + 0x80, m, lbl_80561CAC, lbl_80561CD0, lbl_80561CCC);    // Front Height
    obj->Add(names + 0x3E4, offset + 0xC8, m, lbl_80561CAC, lbl_80561CD0, lbl_80561CCC);    // Rear Height
    obj->Add(names + 0x3F0, offset + 0x84, nm, lbl_80561EBC, lbl_80561D68, lbl_80561F14);   // Front Springs
    obj->Add(names + 0x400, offset + 0xCC, nm, lbl_80561EBC, lbl_80561D68, lbl_80561F14);   // Rear Springs
    obj->Add(names + 0x410, offset + 0x8C, nsm, lbl_80561F14, lbl_80561F18, lbl_80561F10);  // Front Damping
    obj->Add(names + 0x420, offset + 0xD4, nsm, lbl_80561F14, lbl_80561F18, lbl_80561F10);  // Rear Damping
    obj->Add(names + 0x430, offset + 0x110, &lbl_8055A42C, lbl_80561D00, lbl_80561D44, lbl_80561D00);  // Tire Grip
    obj->Add(names + 0x43C, offset + 0x114, &lbl_8055A420, lbl_80561D00, lbl_80561CEC, lbl_80561D00);  // Tire Slip
}
#pragma pop

void fn_8018A654(u8* p, const u8* cfg) {
    if (*(s8*)(cfg + 0xEC)) {
        u8* m = fn_801D2008((u8*)cfg + 0xEC);
        PTR(p, 0x26C0) = m;
        if (S32(m, 0x10) > 0) {
            S32(p, 0x26C4) = fn_801C1C1C();
        } else {
            S32(p, 0x26C4) = 0;
        }
        s32 n = S32(PTR(p, 0x26C0), 0xC8);
        if (n > 0) {
            S32(p, 0x26C8) = fn_801C1BA4(n * 2, n, n);
        } else {
            S32(p, 0x26C8) = 0;
        }
    } else {
        PTR(p, 0x26C0) = 0;
        S32(p, 0x26C4) = 0;
        S32(p, 0x26C8) = 0;
    }
}

void fn_8018A6F4(u8* p) {
    if (PTR(p, 0x26C8)) {
        fn_801C1C7C(PTR(p, 0x26C8));
    }
    if (PTR(p, 0x26C4)) {
        fn_801C1C7C(PTR(p, 0x26C4));
    }
    if (PTR(p, 0x26C0)) {
        fn_801D21B4(PTR(p, 0x26C0));
    }
}

}  // extern "C"

// The spring system descriptors (ppsys.cpp).
struct SprPos {
    f32 v[3];
};

struct SprVert {
    SprPos pos;  // offset 0x0
    f32 w34;     // offset 0xC
    f32 w30;     // offset 0x10
};

struct SprEdge {
    s32 a;      // offset 0x0
    s32 b;      // offset 0x4
    f32 extra;  // offset 0x8
    f32 stiff;  // offset 0xC
};

struct SprPin {
    s32 idx;          // offset 0x0
    const u8* mtx;    // offset 0x4
    const f32* at;    // offset 0x8
};

extern "C" {
void fn_801C1CBC(u8* sys, int nv, const SprVert* v, int ne, const SprEdge* e, int np, const SprPin* pins);
void fn_801C2170(u8* sys, u8* desc, void* owner);
}


extern "C" {

// Builds the body's spring system: per attachment point a pinned particle and a free one, joined by an edge.
void fn_8018A750(u8* p) {
    if (PTR(p, 0x26C4)) {
        fn_801C2170(PTR(p, 0x26C4), PTR(p, 0x26C0), p + 0x70);
    }
    if (PTR(p, 0x26C8)) {
        SprVert verts[8];
        SprEdge edges[4];
        SprPin pins[4];
        int n = S32(PTR(p, 0x26C0), 0xC8);
        for (int i = 0; i < S32(PTR(p, 0x26C0), 0xC8); i++) {
            SprVert* v = &verts[i * 2];
            v->pos.v[0] = F32(p, 0x94);
            v->pos.v[1] = F32(p, 0x98);
            v->pos.v[2] = F32(p, 0x9C);
            v->w34 = lbl_80561CD0;
            v->w30 = lbl_80561CDC;
            v[1].pos = v[0].pos;
            v[1].w34 = v[0].w34;
            v[1].w30 = v[0].w30;
            edges[i].a = i * 2;
            edges[i].b = i * 2 + 1;
            edges[i].extra = lbl_80561CAC;
            edges[i].stiff = lbl_80561CD0 - F32(PTR(p, 0x26C0) + i * 8, 0xA8);
            pins[i].idx = i * 2;
            pins[i].mtx = p + 0x70;
            pins[i].at = 0;
        }
        fn_801C1CBC(PTR(p, 0x26C8), n * 2, verts, n, edges, n, pins);
    }
}

}  // extern "C"

extern "C" {
double fmod(double x, double y);
void fn_801C27D8(u8* sim, BVec* gravity, BVec* up, BVec* force, BVec* torque);

// Feeds the flapping parts (flags/aerials at 0x26C4/0x26C8) their wind and motion input.
void fn_8018A8A8(u8* p) {
    BVec gravity(0.0f, -10000.0f, 0.0f);
    BVec up(0.0f, 1.0f, 0.0f);
    BVec zero(0.0f, 0.0f, 0.0f);
    if (PTR(p, 0x26C4)) {
        f32 t = lbl_8055E3C8;
        BVec wind(1681.65f * ((f32)fmod(t, 0.16f) - 0.08f), 1954.35f * ((f32)fmod(t, 0.14f) - 0.07f),
                  1363.5f * ((f32)fmod(t, 0.2f) - 0.1f));
        fn_801C27D8(PTR(p, 0x26C4), &gravity, &up, &wind, &zero);
    }
    if (PTR(p, 0x26C8)) {
        BVec v;
        Scale(&v, (BVec*)(p + 0x118), -227.25f);
        fn_801C27D8(PTR(p, 0x26C8), &gravity, &up, &v, &zero);
    }
}

void fn_801C2BDC(u8* sim, s32* n, BVec** pos, BVec** old);
void fn_801BEB30(s32 car, u8** out);

static inline f32 FlagDot(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

// v in the body frame (rotation rows at 0x70, position at 0x94), in place.
static inline void ToBody(u8* p, BVec* o, const BVec* v) {
    o->x = v->x - F32(p, 0x94);
    o->y = v->y - F32(p, 0x98);
    o->z = v->z - F32(p, 0x9C);
    f32 x = FlagDot(o->x, F32(p, 0x70), o->y, F32(p, 0x74), o->z, F32(p, 0x78));
    f32 y = FlagDot(o->x, F32(p, 0x7C), o->y, F32(p, 0x80), o->z, F32(p, 0x84));
    f32 z = FlagDot(o->x, F32(p, 0x88), o->y, F32(p, 0x8C), o->z, F32(p, 0x90));
    o->x = x;
    o->y = y;
    o->z = z;
}

// Copies the flapping parts' simulated points (0x26C4 flag, 0x26C8 aerial) into the car model's point
// list `m` (0xE24) in body space, then pins the four wheel points at the suspension heights.
void fn_8018A9F0(u8* p, u8* m) {
    if (PTR(p, 0x26C4)) {
        BVec* pos = 0;
        BVec* old = 0;
        s32 n = 0;
        fn_801C2BDC(PTR(p, 0x26C4), &n, &pos, &old);
        for (int i = 0; i < S32(PTR(p, 0x26C0), 0xA4); i++) {
            u8* d = PTR(p, 0x26C0);
            ToBody(p, (BVec*)(m + 0xE24) + (S32(d, 0xA0) + i), &pos[((s32*)(d + 0x20))[i]]);
        }
    }
    if (PTR(p, 0x26C8)) {
        BVec* pos = 0;
        BVec* old = 0;
        s32 n = 0;
        fn_801C2BDC(PTR(p, 0x26C8), &n, &pos, &old);
        for (int i = 0; i < S32(PTR(p, 0x26C0), 0xC8); i++) {
            u8* d = PTR(p, 0x26C0);
            ToBody(p, (BVec*)(m + 0xE24) + ((s32*)(d + 0xAC))[i * 2], &pos[i * 2 + 1]);
        }
    }
    u8* x = 0;
    fn_801BEB30(S32(p, 0x1EC), &x);
    CVec4 h = *(CVec4*)(x + 0x70);
    BVec* o = (BVec*)(m + 0xE24) + S32(PTR(p, 0x26C0), 0xCC);
    o->x = lbl_80561CAC;
    o->y = F32(p, 0x5AC) + (F32(m, 0x80) + h.x);
    o->z = lbl_80561CAC;
    o = (BVec*)(m + 0xE24) + S32(PTR(p, 0x26C0), 0xD0);
    o->x = lbl_80561CAC;
    o->y = F32(p, 0x5AC) + (F32(m, 0xEC) + h.y);
    o->z = lbl_80561CAC;
    o = (BVec*)(m + 0xE24) + S32(PTR(p, 0x26C0), 0xD4);
    o->x = lbl_80561CAC;
    o->y = F32(p, 0x5AC) + (F32(m, 0x158) + h.z);
    o->z = lbl_80561CAC;
    o = (BVec*)(m + 0xE24) + S32(PTR(p, 0x26C0), 0xD8);
    o->x = lbl_80561CAC;
    o->y = F32(p, 0x5AC) + (F32(m, 0x1C4) + h.w);
    o->z = lbl_80561CAC;
    S32(m, 0xFA4) = S32(PTR(p, 0x26C0), 0xDC);
}

void fn_8018ADA4(u8* p, f32 v) { F32(p, 0x25DC) = v; }

void fn_8018ADAC(u8* p, f32 v) { F32(p, 0x25DC) = v; }

void fn_8018ADB4(u8* p, f32 v) { F32(p, 0x25E0) = v; }

void fn_8018ADBC(u8* p, f32 v) { F32(p, 0x25E4) = v; }

void fn_8018ADC4(u8* p, f32 v) { F32(p, 0x25E8) = v; }

void fn_8018ADCC(u8* p, f32 v) { F32(p, 0x25EC) = v; }

void fn_8018ADD4(u8* p, s32 v) { S32(lbl_8055E918 + S32(p, 0x1EC) * 0x9C, 0x8) = v; }

void fn_8018ADEC(u8* p) { S32(lbl_8055E918 + S32(p, 0x1EC) * 0x9C, 0x8) = -10; }

void fn_8018AE08(u8* p) { S32(lbl_8055E918 + S32(p, 0x1EC) * 0x9C, 0x8) = -10; }

void fn_8018AE24(u8* p, u8 v) { U8(p, 0x260D) = v; }

void fn_8018AE2C(u8* p, f32 v) { F32(p, 0x2600) = v; }

void fn_8018AE34(u8* p, f32 v) { F32(p, 0x2604) = v; }

f32 fn_8018AE3C(void) { return 0.0f; }

f32 fn_8018AE44(void) { return 0.0f; }

f32 fn_8018AE4C(void) { return 0.0f; }

f32 fn_8018AE54(void) { return 0.0f; }

// Shared body-tuning block (lbl_80434990), built on first use.
struct BodyTune {
    BodyTune(void) {}
    u8 data[0x10];
};

BodyTune* fn_8018AE5C(void) {
    static BodyTune s;
    return &s;
}

f32 fn_8018AE7C(u8* p) {
    if (U32(fn_801769CC(), 0x4) == 0x180304) {
        return 0.15f;
    }
    if (Positive(F32(p, 0x25BC))) {
        return 0.15f;
    }
    if (F32(p, 0xD4) > 1363.5f) {
        return 0.15f;
    }
    return 0.5f;
}

void fn_8018AF00(u8* p) {}

}  // extern "C"


static inline void SetVec4(u8* d, CVec4 v) {
    CVec4* o = (CVec4*)d;
    o->x = v.x;
    o->y = v.y;
    o->z = v.z;
    o->w = v.w;
}

extern "C" {
int fn_801CF064(void);
s32* fn_801E1F44(void);
void fn_801BEB30(s32 car, u8** out);
void fn_8018F6F8(u8* p);

// Puts the car back in one piece: restores the saved damage state (the alternate copy when fn_801CF064 says so),
// clears the wheels' detached flags and resets the matching cross-link state.
void fn_8018AF04(u8* p) {
    fn_8018F2C0(p, !fn_801CF064() ? p + 0x1E44 : p + 0x20F8);
    CVec4 load;
    u8* w = p + 0x241C;
    for (int i = 0; i < 4; i++, w += 0x74) {
        S32(w, 0x0) &= ~8;
        (&load.x)[i] = F32(w, -0x34);
    }
    u8* x;
    fn_801BEB30(S32(p, 0x1EC), &x);
    SetVec4(x + 0x70, load);
    SetVec4(x + 0x130, CVec4(0.0f, 0.0f, 0.0f, 0.0f));
    SetVec4(x + 0x140, CVec4(0.0f, 0.0f, 0.0f, 0.0f));
    if (S32(p, 0x26B0) >= 0) {
        S32(p, 0x26B0) = -1;
    }
    S32(p, 0x1E40) = 0;
    if (*fn_801E1F44() == 1) {
        fn_8018F6F8(p);
    }
    F32(p, 0x2650) = lbl_80561CAC;
    F32(p, 0x264C) = lbl_80561CAC;
    F32(p, 0x25C4) = lbl_80561CAC;
}

u32 fn_8018B06C(u8* p) { return U32(lbl_8055E918 + S32(p, 0x1EC) * 0x9C, 0x14); }

u32 fn_8018B084(u8* p) { return U32(lbl_8055E918 + S32(p, 0x1EC) * 0x9C, 0x20); }

void fn_8018B09C(u8* p, const RVec3* a, const RVec3* b, int c, void* d) {
    ((RigidBody*)p)->OnHit(a, b, c, d);
    F32(p, 0x26DC) = lbl_8055E3C8;
}

void fn_8018B0D0(u8* p, u32 v) { U32(p, 0x2610) = v; }

void fn_8018B0D8(u8* p) {}  // stub (607)

}  // extern "C"

static inline f32 PackClamp01(f32 x) {
    x = 1.0f < x ? 1.0f : x;
    return x > 0.0f ? x : 0.0f;
}

static inline f32 PackLerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

// The car's track position tracker (car + 0x270).
class CarTrackPos {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual u8* Segment(void);  // 0x18
};

// Quantises x from [lo, hi] to 0..range (rounded).
static inline int Pack(f32 x, f32 lo, f32 hi, f32 range) {
    return (int)(0.5f + PackLerp(0.0f, range, PackClamp01((x - lo) / (hi - lo))));
}

extern "C" {
s32 fn_801363A0(u8* seg, u8* pos);

// Writes the car's state into a network/replay packet: engine and control values, then per wheel
// (steer angle, suspension, spin, slip) and its contact flags.
int fn_8018BA54(u8* p, u8* pk) {
    ((RigidBody*)p)->vf34((RPacket*)pk);
    U16(pk, 0x20) = Pack(F32(p, 0x25D0), -2000.0f, 12000.0f, 65535.0f);
    U8(pk, 0x22) = Pack(F32(p, 0x25F0), -1.2f, 1.2f, 255.0f);
    U8(pk, 0x23) = Pack(F32(p, 0x25F4), 0.0f, 1.0f, 255.0f);
    U8(pk, 0x24) = Pack(F32(p, 0x25E4), 0.0f, 1.0f, 255.0f);
    f32 k = lbl_80561CD0;
    U8(pk, 0x3C) = (int)(0.5f + PackLerp(0.0f, 255.0f, k));
    u8* seg = ((CarTrackPos*)PTR(p, 0x270))->Segment();
    S32(pk, 0x40) = fn_801363A0(seg, seg + 4);
    if (U8(pk, 0x3C) == 0) {
        U8(pk, 0x3C) = 1;
    }
    U16(pk, 0x3A) = S32(p, 0x4);
    U8(pk, 0x25) = S32(lbl_8055E918 + S32(p, 0x1EC) * 0x9C, 0x14);
    u8* w = p + 0x23AC;
    u8* out = pk + 0x26;
    for (int i = 0; i < 4; i++) {
        u32 flags = U32(w, 0x70);
        if (U8(w, 0x20)) {
            flags |= 1;
        }
        out[2] = Pack(F32(w, 0xC), 0.0f, 2.0f, 255.0f);
        out[0] = Pack(F32(w, 0x8), -6.2831855f, 6.2831855f, 255.0f);
        out[1] = Pack(F32(w, 0x0), -100.0f, 100.0f, 255.0f);
        out[3] = Pack(F32(w, 0x68), 0.0f, 10.0f, 255.0f);
        out[4] = flags;
        w += 0x74;
        out += 5;
    }
    return 0x44;
}

void fn_8018BDEC(u8* p) {
    f32 m[12];
    f32* mtx = m;
    f32 z = 0.0f;
    f32 one = 1.0f;
    m[1] = z;
    m[0] = one;
    m[2] = z;
    m[3] = z;
    m[4] = one;
    m[5] = z;
    m[6] = z;
    m[7] = z;
    m[8] = one;
    m[9] = z;
    m[10] = z;
    m[11] = z;
    if (PTR(p, 0x26C4)) {
        fn_801C2BF8(PTR(p, 0x26C4), mtx);
    }
}

void fn_8018BE54(u8* p, f32 s) {
    if ((f32)__fabs(F32(p, 0x5E4) - s) <= lbl_80558DC4[0]) {
        return;
    }
    F32(p, 0x5E4) = s;
    fn_801A1474(PTR(p, 0x268), F32(p, 0x5E4));
    f32 k = F32(p, 0x5E4);
    f32 k2 = k * k;
    F32(p, 0x330) = F32(p, 0x468) * k2;
    F32(p, 0x334) = F32(p, 0x46C) * k2;
    F32(p, 0x338) = F32(p, 0x470) * k2;
    F32(p, 0x344) = F32(p, 0x47C) * k;
    F32(p, 0x33C) = F32(p, 0x474) * k;
    F32(p, 0x340) = F32(p, 0x478) * k;
    F32(p, 0x34C) = F32(p, 0x484) * k;
    F32(p, 0x3AC) = k * F32(p, 0x4E4);
    F32(p, 0x3D0) = k * F32(p, 0x508);
    F32(p, 0x3F4) = k * F32(p, 0x52C);
    F32(p, 0x418) = k * F32(p, 0x550);
    fn_8018A180(p);
}

void fn_8018BF44(u32* p) { *p = 0x0FF807FF; }

u32 fn_8018BF54(u8* p) { return 0; }

void fn_8018BF5C(u8* p) {}

void fn_8018BF60(u8* p) {}

int fn_8018BF64(void) { return 0x44; }

}  // extern "C"
