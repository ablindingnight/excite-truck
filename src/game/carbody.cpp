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

struct BVec {
    BVec(void) {}
    BVec(f32 a, f32 b, f32 c) : x(a), y(b), z(c) {}
    f32 x, y, z;
};

// Plain 3-vector without constructors: copies move words.
struct HVec {
    f32 x, y, z;
};

struct CollShape;

// The body's collision shape interface (vtable slots used here).
class CbShapeV {
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
    virtual void SetGroup(int g);  // 0x40
    virtual void SetMask(int m);   // 0x44
};


// A 12-word body pose.
struct CbPose {
    u32 w[12];
};

class CollVolume {
public:
    virtual ~CollVolume(void);
    void Update(void);
    void AddShape(CollShape* s);
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
    void Init(struct BodyDesc* d);
    void Flush(RVec3* force, RVec3* torque);
    void AddForceAt(const RVec3* f, const RVec3* at);
    void AddTorque(const RVec3* t);
    void AddImpulse(f32 k, const RVec3* j, const RVec3* at);
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

extern f32 lbl_80558DC4[];  // FLT_EPSILON

static inline int Positive(f32 x) { return x > lbl_80558DC4[0]; }

ModuleTag lbl_8055E4C0;
static inline u32 MakeILPos(u32 seg, u32 frac) { return (seg << 16) | frac; }
extern const u32 lbl_8055E4C8 = MakeILPos(0x7F00, 0);
CarHandle lbl_8055E4CC;

extern "C" {

}  // extern "C"

extern const f32 lbl_80561CA0;  // torque unit scale
extern const f32 lbl_80561CA4;  // 1e-5
extern const f32 lbl_80561CA8;  // 5252 (hp = torque * rpm / 5252)
extern const f32 lbl_80561CAC;        // 0.0
extern const f32 lbl_80561CB0;  // -2.0
extern const f32 lbl_80561CB4;  // 8000
extern const f32 lbl_80561CB8;  // 0.3
extern const f32 lbl_80561CBC;  // 1e-4

static inline f32 FMax(f32 a, f32 b) { return a > b ? a : b; }

extern "C" {

// Builds the engine torque curve from v = {hp, hp rpm, peak torque, torque rpm, redline rpm, idle torque}: a
// parabola through the torque peak and the hp point below it, another from the peak down to 30% at redline.
void fn_8018185C(const u8* p, f32* v) {
    F32(p, 0x1C) = v[1];
    F32(p, 0x28) = v[3];
    f32 vv = v[5];
    F32(p, 0x18) = lbl_80561CA0 * vv;
    f32 hp = lbl_80561CA0 * (lbl_80561CA8 * v[0] / FMax(v[1], lbl_80561CA4));
    F32(p, 0x24) = lbl_80561CAC;
    f32 peak = v[2] * lbl_80561CA0;
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

void fn_801819A0(u8* p, const u8* cfg, f32 a, f32 b, f32 c) {
    f32 v[6];
    v[0] = F32(cfg, 0x54) * a / lbl_80561CC4;
    v[2] = F32(cfg, 0x50) * a / lbl_80561CC8;
    v[1] = F32(cfg, 0x5C) * c;
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
    static bool firstTime = true;  // reset the tunables to their defaults
    if (firstTime) {
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
    if (firstTime) {
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
    firstTime = false;
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
    static bool firstInit = true;
    u32 flags = U32(fn_801769CC(), 0x4);
    lbl_8055E4F4 = (flags >> 19) & 1;
    lbl_8055E4D0 = 0;
    fn_801BD694(8);
    fn_801AFC28();
    lbl_8055E4D4 = 1;
    fn_80181AFC();
    if (firstInit) {
        lbl_8055E4E0.a = 10.0f;
        lbl_8055E4E0.b = 2.0f;
    }
    firstInit = false;
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

// The car's identity block from its descriptor (0x6C..0xDF), kept at 0x1F4.
struct CbIdent {
    char name[0x40];   // 0x00
    char model[0x20];  // 0x40 (0x234 in the body)
    s32 w60;           // 0x60
    s32 w64;           // 0x64
    s32 vehicle;       // 0x68 (0x25C)
    u8 b6C;            // 0x6C
    s32 w70;           // 0x70
};

// Descriptor block 0x18..0x67, kept at 0x274.
struct CbDescBlock {
    s32 w[14];  // 0x00
    s32 w38;    // 0x38
    s32 w3C;    // 0x3C
    s16 h40;    // 0x40
    s32 w44;    // 0x44
    s32 w48;    // 0x48
    s32 w4C;    // 0x4C
};

// One wheel's setup inside CbSetup.
struct CbWheelSetup {
    s32 w[9];
};

// Tuned car setup (0x138 bytes): read from the car's physics tables, kept twice in the body (0x32C, 0x464).
struct CbSetup {
    f32 mass;                                                           // 0x00
    f32 f04, f08, f0C, f10, f14, f18, f1C, f20, f24, f28, f2C, f30;    // 0x04
    s32 i34, i38, i3C, i40, i44, i48, i4C;                              // 0x34
    f32 f50, f54, f58, f5C, f60, f64, f68, f6C, f70, f74, f78, f7C;    // 0x50
    CbWheelSetup wheels[4];                                             // 0x80
    f32 f110, f114, f118, f11C, f120, f124, f128;                       // 0x110
    HVec modelOffset;                                                   // 0x12C
};

struct ValVec3C {
    f32 x, y, z;
};

class CbValueNode {
public:
    virtual const char* GetName(void);
    virtual int Has(const char* key);
    virtual int GetPair(int index, const char** key, const char** value);
    virtual const char* GetValue(const char* key);
    virtual const char* GetString(const char* key, const char* def);
    virtual s32 GetIntValue(const char* key);
    virtual s32 GetInt(const char* key, s32 def);
    virtual f32 GetFloat(const char* key);
    virtual f32 GetFloat(const char* key, f32 def);
    virtual s32 GetIntValue2(const char* key);
    virtual int GetBool(const char* key, int def);
    virtual ValVec3C GetVec3(const char* key);
    virtual ValVec3C GetVec3Or(const char* key, ValVec3C def);
    virtual void v3C(void);
    virtual void v40(void);
    virtual void v44(void);
    virtual void v48(void);
    virtual void v4C(void);
    virtual void v50(void);
    virtual s32 GetEnum(const char* key, const char* def);  // 0x54
};

class CbValueTable {
public:
    virtual CbValueNode* FindChild(const char* name);  // 0x8
};

CbValueTable* fn_80131808(const char* name);
void fn_80131818(CbValueTable* table);
int fn_801345D8(char* buf, int n, const char* fmt, ...);
s32 fn_802792E4(s32 vehicle);
s32 fn_80279874(void);
int fn_8027AA48(void);
void fn_80279F58(CbSetup* setup, CbValueTable* table);
void* fn_8027A798(const char* model);
void fn_8027A8B4(CbSetup* setup, void* tune);
void fn_801B86BC(s32 car, void* look);
u8* fn_801B2EDC(s32 car, u8* body);
void fn_801795D8(u8* p, s32 car);
void fn_801BDA74(s32 car, u8* setup);
void fn_801BEB30(s32 car, u8** out);
void fn_801836DC(void);
void fn_8018C5A0(u8* p, u8* desc);
void fn_8018A654(u8* p, const u8* cfg);
u8* fn_80209448(u8* p);
u8* fn_80209D4C(u8* p);
u8* fn_8020DE74(u8* p);
void fn_801D108C(s32 car);
int fn_8020503C(const char* name);
void fn_802E30C8(const char* fmt, ...);
void fn_80182DF4(u8* p);
extern u8 lbl_8055E4F7;  // the zone-mode limiter message was shown
void vf20__10CollVolumeFv(u8* p);
}  // extern "C"
struct PlayerLook;
struct BodyDesc;
void GetPlayerLook(PlayerLook* look, s32 vehicle);
u8* FindVehicle(s32 vehicle);
extern "C" {

// CarBody::Init: copies the descriptor's identity and setup blocks, reads the chassis tuning from
// "<model>.car/physics.val" (camera, head positions in inches, suspension, speed limiter, gravity boosts),
// sets up the rigid body, wheels, collision shapes and sub-systems, then resets the body.
void CarBody_vf08(u8* p, u8* desc) {
    char path[0x50];
    CbSetup setup;
    U8(p, 0x2680) = 0;
    S32(p, 0x59C) = S32(desc, 0xE4);
    S32(p, 0x5A4) = S32(desc, 0xE8);
    S32(p, 0x5A0) = S32(desc, 0xDC);
    if (S32(p, 0x5A0) == 0) {
        S32(p, 0x5A0) = 0x0FF807FF;
    }
    if (U32(fn_801769CC(), 0x4) & 0x400) {
        U32(p, 0x5A0) |= 0x80000000;
    }
    S32(p, 0x1EC) = S32(desc, 0xE0);
    S32(p, 0x1F0) = S32(desc, 0x68);
    *(CbIdent*)(p + 0x1F4) = *(CbIdent*)(desc + 0x6C);
    S32(p, 0x26C) = fn_802792E4(S32(p, 0x25C));
    *(CbDescBlock*)(p + 0x274) = *(CbDescBlock*)(desc + 0x18);
    S32(p, 0x328) = fn_80279874();
    if (!fn_8027AA48()) {
        S32(p, 0x328) = 0;
    }
    fn_801345D8(path, 0x50, "%s.car/physics.val", (char*)(p + 0x234));
    CbValueTable* table = fn_80131808(path);
    ValVec3C head;
    ValVec3C back;
    CbValueNode* chassis = table->FindChild("Chassis");
    F32(p, 0x5B4) = chassis->GetFloat("chase_camera_dist", 0.0f);
    F32(p, 0x5B8) = chassis->GetFloat("chase_camera_height", 0.0f);
    head = chassis->GetVec3("head_pos");
    F32(p, 0x5C4) = head.x;
    F32(p, 0x5C8) = head.y;
    back = chassis->GetVec3Or("lookback_head_pos", *(ValVec3C*)(p + 0x5D0));
    F32(p, 0x5D0) = 0.2f * head.x;
    F32(p, 0x5D4) = head.y + 3.0f;
    F32(p, 0x5CC) = head.z;
    F32(p, 0x5D8) = head.z;
    F32(p, 0x5D0) = back.x;
    F32(p, 0x5D4) = back.y;
    F32(p, 0x5C4) = F32(p, 0x5C4) * 0.0254f;
    F32(p, 0x5C8) = F32(p, 0x5C8) * 0.0254f;
    F32(p, 0x5CC) = F32(p, 0x5CC) * 0.0254f;
    F32(p, 0x5D0) = back.x * 0.0254f;
    F32(p, 0x5D4) = back.y * 0.0254f;
    F32(p, 0x5D8) = back.z * 0.0254f;
    F32(p, 0x5C0) = 0.0174f * chassis->GetFloat("head_pitch");
    if (chassis->Has("treehit_class")) {
        S32(p, 0x5E8) = chassis->GetEnum("treehit_class", "light");
    } else {
        S32(p, 0x5E8) = 1;
    }
    GetPlayerLook((PlayerLook*)(p + 0x2C4), S32(p, 0x25C));
    fn_801B86BC(S32(desc, 0xE0), p + 0x2C4);
    U8(p, 0x25C0) = (U32(FindVehicle(S32(p, 0x25C)), 0xA0) >> 2) & 1;
    fn_80279F58(&setup, table);
    fn_8027A8B4(&setup, fn_8027A798((char*)(p + 0x234)));
    setup.mass = setup.mass * F32(p, 0x2EC);
    setup.f110 = setup.f110 * (0.5f * (F32(p, 0x2F4) + F32(p, 0x2F8)));
    setup.f114 = setup.f114 * F32(p, 0x2FC);
    for (s32 i = 0; i < 4; i++) {
        CbValueNode* axle = table->FindChild((i == 0 || i == 1) ? "Front" : "Rear");
        F32(p + i * 0x74, 0x23E8) = F32(desc + i * 4, 0x10C);
        F32(p + i * 0x74, 0x23EC) = 0.0254f * axle->GetFloat("max_compression", 8.0f) - F32(p + i * 0x74, 0x23E8);
        *(f32*)((u8*)&setup.wheels[i] + 0x18) = F32(p + i * 0x74, 0x23E8);
    }
    F32(desc, 0x8) = setup.mass / 0.4545f;
    F32(desc, 0xC) = setup.f04 / 0.042279866f;
    F32(desc, 0x10) = setup.f08 / 0.042279866f;
    F32(desc, 0x14) = setup.f0C / 0.042279866f;
    ((RigidBody*)p)->Init((BodyDesc*)desc);
    PTR(p, 0x270) = fn_801B2EDC(S32(p, 0x1EC), p);
    fn_801795D8(p, S32(p, 0x1EC));
    F32(p, 0x5AC) = -(0.0254f * chassis->GetFloat("susp_y", 0.0f));
    F32(p, 0x5B0) = setup.f128;
    F32(p, 0x5A8) = 444.44446f;
    if (U32(fn_801769CC(), 0x4) != 0x180201) {
        F32(p, 0x5A8) = 0.44444445f * (F32(p, 0x2C8) + chassis->GetFloat("speed_limiter", F32(p, 0x5A8)));
    } else if (!lbl_8055E4F7) {
        lbl_8055E4F7 = 1;
        fn_802E30C8("%s", "CAR: Disabling speed limiter in zone mode.");
    }
    f32 boost = chassis->GetFloat("gravity_boost", 0.0f);
    F32(p, 0x5DC) = boost;
    F32(p, 0x5E0) = chassis->GetFloat("down_gravity_boost", boost);
    U8(p, 0x26E0) = chassis->GetBool("squishy", 0);
    F32(p, 0x30) = setup.modelOffset.x;
    F32(p, 0x34) = setup.modelOffset.y;
    F32(p, 0x38) = setup.modelOffset.z;
    vf20__10CollVolumeFv(p);
    if (U32(p, 0x10) == 'ACAR') {
        setup.f2C = 0.75f;
    }
    *(CbSetup*)(p + 0x32C) = setup;
    *(CbSetup*)(p + 0x464) = *(CbSetup*)(p + 0x32C);
    fn_801BDA74(S32(p, 0x1EC), p + 0x32C);
    u8* wheels = 0;
    fn_801BEB30(S32(p, 0x1EC), &wheels);
    for (int w = 0; w < 4; w++) {
        *(BVec*)(p + 0x23DC + w * 0x74) = BVec(F32(wheels, w * 0x10), F32(wheels, w * 0x10 + 4), F32(wheels, w * 0x10 + 8));
    }
    fn_801836DC();
    fn_80182DF4(p);
    fn_8018C5A0(p, desc);
    fn_8018A654(p, desc);
    int player = S32(desc, 0x0) == 'PCAR';
    PTR(p, 0x257C) = fn_80209448(p);
    if (player) {
        PTR(p, 0x2580) = fn_80209D4C(p);
    } else {
        PTR(p, 0x2580) = 0;
    }
    PTR(p, 0x2584) = fn_8020DE74(p);
    fn_801D108C(S32(p, 0x1EC));
    fn_80131818(table);
    u8 trees = 0;
    if (player && fn_8020503C("player_destroys_trees")) {
        trees = 1;
    }
    U8(p, 0x25C8) = trees;
    if (U32(fn_801769CC(), 0x4) == 0x180304) {
        U8(p, 0x25C8) = 1;
    }
    fn_801838C8(p);
}

// One wheel-contact corner (fn_801AE180 allocates it), linked into the body's list at 0xD0.
struct CbCorner {
    CbCorner* next;  // 0x00
    BVec pos;        // 0x04
    f32 spring;      // 0x10
    f32 friction;    // 0x14
    f32 damping;     // 0x18
    BVec vel;        // 0x1C
};

CbCorner* fn_801AE180(void);
s32 fn_801E3004(s32 model, const char* name, s32 a);
HVec* fn_801E3630(s32 joint);
void fn_801E323C(s32 joint);
void fn_802C7760(s32 model, f32* x0, f32* x1, f32* y0, f32* y1, f32* z0, f32* z1);
u8* fn_8019FFD8(u8* p, u8* frame, u8* owner);
void fn_801A0258(u8* grp, const HVec* c, const BVec* mat, f32 r, f32 k, f32 d, f32 m, f32 im);
u8* fn_801A2354(u8* p, u8* frame, const BVec* c, u8* owner, f32 k, f32 d, f32 hx, f32 hy, f32 hz);
void fn_801A0444(u8* grp, u8* box);
}  // extern "C"
void* operator new(unsigned long n, const char* tag);
extern "C" {

static inline void AddBodyCorner(u8* p, f32 x, f32 y, f32 z, f32 spring) {
    BVec pos;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    CbCorner* c = fn_801AE180();
    c->pos = pos;
    c->spring = spring;
    c->friction = 0.5f;
    c->vel.x = 0.0f;
    c->vel.y = 0.0f;
    c->vel.z = 0.0f;
    c->damping = 0.0f;
    c->next = *(CbCorner**)(p + 0xD0);
    *(CbCorner**)(p + 0xD0) = c;
}

// One sphere of the body's collision group, at a point in body space (made relative to the centre of mass).
static inline void AddBodySphere(u8* p, u8* g, f32 x, f32 y, f32 z, const BVec* mat, f32 r, f32 k, f32 d, f32 m, f32 im) {
    BVec at(x, y, z);
    HVec rel;
    rel.x = at.x - F32(p, 0x30);
    rel.y = at.y - F32(p, 0x34);
    rel.z = at.z - F32(p, 0x38);
    fn_801A0258(g, &rel, mat, r, k, d, m, im);
}

// Builds the body's collision: two wheel contact corners at the "wheel" joint, then a group of twelve spheres
// sized from the model's bounding box (front, middle and rear rows) and a box around the whole body.
void fn_80182DF4(u8* p) {
    f32 minX = 0.0f;
    f32 maxX = 0.0f;
    f32 minY = 0.0f;
    f32 maxY = 0.0f;
    f32 minZ = 0.0f;
    f32 maxZ = 0.0f;
    f32 stiff;
    f32 damp;
    f32 mass = 4938240000.0f;
    f32 invMass = 0.5f / mass;
    f32 zRear;
    f32 zFront;
    stiff = 50000.0f;
    damp = 0.3f;
    fn_802C7760(S32(p, 0x59C), &minX, &maxX, &minY, &maxY, &minZ, &maxZ);
    s32 wheel = fn_801E3004(S32(p, 0x59C), "wheel", -1);
    if (wheel) {
        HVec* o = fn_801E3630(wheel);
        AddBodyCorner(p, o->x, maxY + F32(p, 0x34) + 0.1016f, o->z, 2.0f * stiff);
        AddBodyCorner(p, -o->x, maxY + F32(p, 0x34) + 0.1016f, o->z, 2.0f * stiff);
        fn_801E323C(wheel);
    }
    minY = FMax(minY, 0.2f);
    HVec c = *(HVec*)(p + 0x30);
    minX += c.x;
    maxX += c.x;
    minY += c.y;
    maxY += c.y;
    minZ += c.z;
    maxZ += c.z;
    if (S32(p, 0x1F0) == 84 && !(U32(fn_801769CC(), 0x4) & 0x1000)) {
        maxZ -= 30.0f * 0.0254f;  // 30 in
    }
    F32(p, 0x5BC) = maxZ;
    u8* g = (u8*)operator new(0x184, "car-volume");
    if (g) {
        g = fn_8019FFD8(g, p + 0x3C, p);
    }
    PTR(p, 0x268) = g;
    BVec rear(0.7f, 0.1f, 1.0f);
    BVec mid(0.7f, 0.1f, 1.0f);
    BVec front(0.7f, 0.1f, 1.0f);
    f32 a = maxX - 0.45000002f;
    f32 b = 0.8f * a;
    f32 yRear = 0.36f + minY;
    f32 yMid = 0.45000002f + minY;
    zRear = maxZ - 0.45000002f;
    f32 h = 0.5f * (maxY - minY);
    f32 zMid = 0.5f * (minZ + maxZ);
    zFront = 0.44999998f + minZ;
    f32 zRearMid = 0.5f * (zRear + zMid);
    f32 xSide = maxX - h;
    f32 xFront = maxX - 0.44999998f;
    f32 zFrontMid = 0.5f * (zFront + zMid);
    f32 ySide = minY + h;
    f32 yFront = 0.44999998f + minY;
    AddBodySphere(p, PTR(p, 0x268), -b, 0.254f + yRear, zRear, &mid, 0.36f, stiff, damp, mass, invMass);
    AddBodySphere(p, PTR(p, 0x268), 0.0f, 0.254f + yRear, zRear, &mid, 0.36f, stiff, damp, mass, invMass);
    AddBodySphere(p, PTR(p, 0x268), b, 0.254f + yRear, zRear, &mid, 0.36f, stiff, damp, mass, invMass);
    AddBodySphere(p, PTR(p, 0x268), -a, 0.254f + yMid, zRearMid, &mid, 0.45000002f, stiff, damp, mass, invMass);
    AddBodySphere(p, PTR(p, 0x268), a, 0.254f + yMid, zRearMid, &mid, 0.45000002f, stiff, damp, mass, invMass);
    AddBodySphere(p, PTR(p, 0x268), 0.8f * -xSide, 0.254f + ySide, zMid, &rear, h, stiff, damp, mass, invMass);
    AddBodySphere(p, PTR(p, 0x268), 0.8f * xSide, 0.254f + ySide, zMid, &rear, h, stiff, damp, mass, invMass);
    AddBodySphere(p, PTR(p, 0x268), -xFront, 0.254f + yFront, zFrontMid, &front, 0.44999998f, stiff, damp, mass, invMass);
    AddBodySphere(p, PTR(p, 0x268), xFront, 0.254f + yFront, zFrontMid, &front, 0.44999998f, stiff, damp, mass, invMass);
    AddBodySphere(p, PTR(p, 0x268), -xFront, 0.254f + yFront, zFront, &front, 0.44999998f, stiff, damp, mass, invMass);
    AddBodySphere(p, PTR(p, 0x268), 0.0f, 0.254f + yFront, zFront, &front, 0.44999998f, stiff, damp, mass, invMass);
    AddBodySphere(p, PTR(p, 0x268), xFront, 0.254f + yFront, zFront, &front, 0.44999998f, stiff, damp, mass, invMass);
    BVec center(0.5f * (minX + maxX), 0.5f * (minY + maxY), 0.5f * (minZ + maxZ));
    u8* box = (u8*)operator new(0x74, "car_box");
    if (box) {
        box = fn_801A2354(box, p + 0x3C, &center, p, 5.0f * stiff, damp, 0.5f * (maxX - minX), 0.5f * (maxY - minY),
                          0.5f * (maxZ - minZ));
    }
    fn_801A0444(PTR(p, 0x268), box);
    ((CollVolume*)p)->AddShape((CollShape*)PTR(p, 0x268));
    U8(p, 0x1E8) = 1;
    ((CbShapeV*)PTR(p, 0x268))->SetGroup(0x6D);
    if (U32(p, 0x10) == 'PCAR') {
        ((CbShapeV*)PTR(p, 0x268))->SetMask(1);
    } else {
        ((CbShapeV*)PTR(p, 0x268))->SetMask(6);
    }
}

void fn_801836DC(void) {}

}  // extern "C"

class CarHost {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual int v14(void);
    virtual u8* Info(void);  // 0x18
    virtual void v1C(void);
    virtual void v20(void);
    virtual void v24(void);
    virtual void v28(void);
    virtual void v2C(void);
    virtual void v30(void);
    virtual void Unimpair(void);   // 0x34
    virtual int IsImpaired(void);  // 0x38
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
    virtual void Reset(void);   // 0x10
    virtual void v14(void);
    virtual void v18(void);
    virtual void v1C(void);
    virtual void v20(void);
    virtual void v24(void);
    virtual void v28(void);
    virtual void Land(f32 sink);  // 0x2C
    virtual void v30(void);
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

void CarBody_vf10(u8* p) {
    *(CbPose*)(p + 0x3C) = *(CbPose*)(p + 0x274);
    ((CollVolume*)p)->Update();
    ((RigidBody*)p)->Reset();
    fn_801838C8(p);
}

}  // extern "C"

// The AI line follower of a car (deity.cpp).
class IdealFollower {
public:
    void ClearFlag(void);
    u8 pad0[0xC];
    s32 fC;  // offset 0xC
};

// A car's driver as seen by the race (slot 0x18: its line follower).
class CbDriver {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual IdealFollower* Follower(void);  // 0x18
};

// The body's own virtuals (CollVolume/RigidBody slots); 0x54 places it at a pose.
class CbBodyV {
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
    virtual void v48(void);
    virtual void v4C(void);
    virtual void v50(void);
    virtual void SetPose(const u8* pose);  // 0x54
};

// The body's model handle at 0x268 (slot 0x28 resets it).
class CbModel {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(void);
    virtual void v1C(void);
    virtual void v20(void);
    virtual void v24(void);
    virtual void Reset(void);  // 0x28
};

// The body's spring points and their velocities (162 each).
struct CbSprings {
    u8 pad0[0x5FC];
    BVec pts[162];   // offset 0x5FC
    BVec vels[162];  // offset 0xD94
};

// One body joint (18 of them at 0x152C): six orientation keys and two scalars.
struct CbJoint {
    u8 pad0[0xC];
    f32 fC;         // offset 0xC
    f32 f10;        // offset 0x10
    u8 pad14[4];
    f32 q[6][4];    // offset 0x18
    u8 b78;         // offset 0x78
    u8 pad79[3];
    s32 w7C;        // offset 0x7C
};

extern "C" {
void fn_8018BE54(u8* p, f32 s);
void fn_8018B0D0(u8* p, u32 v);
CbDriver* fn_801B2ED0(int id);
void fn_801B8100(u8* pose, s32 car, s32 player);
void fn_80189C64(u8* p);
void fn_801BDA74(s32 id, u8* mass);
void fn_8018F2C0(u8* p, u8* q);
int fn_801CF064(void);
s32* fn_801E1F44(void);
void fn_8018F6F8(u8* p);

// Puts the body back to its start state: clears the suspension/sway/damage state, the joints and the
// wheels, re-places the car on its start pose and reapplies the tuned setup.
void fn_801838C8(u8* p) {
    F32(p, 0x5E4) = 0.0f;
    fn_8018BE54(p, 1.0f);
    F32(p, 0x5EC) = 0.0f;
    F32(p, 0x5F0) = 1.0f;
    F32(p, 0x26E4) = 0.0f;
    F32(p, 0x26E8) = 0.0f;
    F32(p, 0x26EC) = 0.0f;
    F32(p, 0x26F0) = 0.0f;
    F32(p, 0x26F4) = 0.0f;
    F32(p, 0x26F8) = 0.0f;
    F32(p, 0x26FC) = 0.0f;
    fn_80189C64(p);
    S32(p, 0x24) = S32(p, 0x28);
    fn_8018B0D0(p, 0);
    F32(p, 0x25E8) = 0.0f;
    F32(p, 0x25D8) = 0.0f;
    F32(p, 0x25D4) = 0.0f;
    F32(p, 0x25D0) = 0.0f;
    F32(p, 0x25CC) = 0.0f;
    F32(p, 0x25EC) = 0.0f;
    F32(p, 0x25E4) = 0.0f;
    F32(p, 0x25E0) = 0.0f;
    F32(p, 0x25DC) = 0.0f;
    F32(p, 0x25E8) = 1.0f;
    F32(p, 0x25F8) = 0.0f;
    F32(p, 0x25F4) = 0.0f;
    F32(p, 0x25F0) = 0.0f;
    F32(p, 0x25FC) = 0.0f;
    F32(p, 0x2604) = 0.0f;
    F32(p, 0x2600) = 0.0f;
    F32(p, 0x2608) = 0.0f;
    U8(p, 0x260C) = 0;
    U8(p, 0x260D) = 0;
    U8(p, 0x260E) = 0;
    F32(p, 0x2614) = 0.0f;
    F32(p, 0x2618) = 0.0f;
    F32(p, 0x2594) = 0.0f;
    F32(p, 0x2598) = 0.0f;
    F32(p, 0x261C) = 0.0f;
    F32(p, 0x2588) = 0.0f;
    F32(p, 0x2590) = 0.0f;
    F32(p, 0x258C) = -5.0f;
    U8(p, 0x259C) = 0;
    U8(p, 0x259D) = 0;
    F32(p, 0x25BC) = 0.0f;
    F32(p, 0x25C4) = 0.0f;
    F32(p, 0x25A0) = 0.0f;
    F32(p, 0x25A4) = 0.0f;
    F32(p, 0x25A8) = 1.0f;
    F32(p, 0x25AC) = 0.0f;
    S32(p, 0x2620) = 0;
    F32(p, 0x2654) = 0.0f;
    F32(p, 0x2658) = 0.0f;
    F32(p, 0x265C) = 0.0f;
    F32(p, 0x2660) = 0.0f;
    F32(p, 0x2664) = 0.0f;
    F32(p, 0x2668) = 0.0f;
    F32(p, 0x266C) = 0.0f;
    F32(p, 0x2670) = 0.0f;
    F32(p, 0x2674) = 0.0f;
    S32(p, 0x2678) = 0;
    S32(p, 0x267C) = 0;
    F32(p, 0x2688) = 0.0f;
    S32(p, 0x268C) = 0;
    F32(p, 0x2690) = 0.0f;
    F32(p, 0x2694) = 0.0f;
    F32(p, 0x2698) = 0.0f;
    F32(p, 0x269C) = 0.0f;
    F32(p, 0x26A8) = 0.0f;
    memset(p + 0x26A0, 0, 8);
    S32(p, 0x2648) = 0;
    U8(p, 0x2624) = 0;
    U8(p, 0x2625) = 0;
    S32(p, 0x263C) = 0;
    S32(p, 0x2638) = 0;
    F32(p, 0x2628) = 0.0f;
    F32(p, 0x262C) = 0.0f;
    F32(p, 0x2630) = 0.0f;
    F32(p, 0x2634) = 0.0f;
    S32(p, 0x2644) = 0;
    U8(p, 0x2640) = 0;
    F32(p, 0x264C) = 0.0f;
    F32(p, 0x2650) = 0.0f;
    U8(p, 0x26AC) = 0;
    fn_801BF1B8(PTR(p, 0x1EC), 0);
    S32(p, 0x26B0) = -1;
    F32(p, 0x26B4) = 0.0f;
    F32(p, 0x26B8) = 0.0f;
    F32(p, 0x26BC) = 0.0f;
    fn_801BDA74(S32(p, 0x1EC), p + 0x32C);
    CbSprings* sp = (CbSprings*)p;
    for (int i = 0; i < 162; i++) {
        memset(&sp->pts[i], 0, sizeof(BVec));
        sp->pts[i].x = 0.0f;
        sp->pts[i].y = 0.0f;
        sp->pts[i].z = 0.0f;
        sp->vels[i].x = 0.0f;
        sp->vels[i].y = 0.0f;
        sp->vels[i].z = 0.0f;
    }
    CbJoint* j = (CbJoint*)(p + 0x152C);
    for (int i = 0; i < 18; i++, j++) {
        j->f10 = 0.0f;
        j->fC = 0.0f;
        j->w7C = 0;
        j->b78 = 0;
        for (int k = 0; k < 6; k++) {
            j->q[k][0] = 1.0f;
            j->q[k][1] = 0.0f;
            j->q[k][2] = 0.0f;
            j->q[k][3] = 0.0f;
        }
    }
    S32(p, 0x1E3C) = 0;
    S32(p, 0x1E40) = 0;
    u8* w = p + 0x23AC;
    for (int i = 0; i < 4; i++, w += 0x74) {
        F32(w, 0x8) = 0.0f;
        F32(w, 0x4) = 0.0f;
        F32(w, 0x0) = 0.0f;
        F32(w, 0x18) = 0.0f;
        F32(w, 0x14) = 0.0f;
        F32(w, 0x10) = 0.0f;
        F32(w, 0x1C) = 0.0f;
        F32(w, 0xC) = 0.0f;
        U8(w, 0x20) = 0;
        F32(w, 0x24) = 0.0f;
        F32(w, 0x28) = 0.0f;
        F32(w, 0x2C) = 0.0f;
        F32(w, 0x30) = 0.0f;
        F32(w, 0x34) = 0.0f;
        F32(w, 0x38) = 0.0f;
        F32(w, 0x44) = 0.0f;
        F32(w, 0x48) = 0.0f;
        F32(w, 0x4C) = 0.0f;
        F32(w, 0x50) = 0.0f;
        F32(w, 0x54) = 0.0f;
        F32(w, 0x58) = 0.0f;
        F32(w, 0x5C) = 1.0f;
        F32(w, 0x60) = 0.0f;
        S32(w, 0x64) = 0;
        F32(w, 0x68) = 0.0f;
        U8(w, 0x6C) = 0;
        S32(w, 0x70) = 0;
    }
    IdealFollower* f = fn_801B2ED0(S32(p, 0x1EC))->Follower();
    f->fC = 0;
    fn_801B8100(p + 0x274, S32(p, 0x1EC), S32(p, 0x1F0));
    ((CbBodyV*)p)->SetPose(p + 0x274);
    f->ClearFlag();
    fn_8018F2C0(p, !fn_801CF064() ? p + 0x1E44 : p + 0x20F8);
    ((CbModel*)PTR(p, 0x268))->Reset();
    if (*fn_801E1F44() == 1) {
        fn_8018F6F8(p);
    }
    F32(p, 0x26CC) = 1.0f;
    F32(p, 0x26D0) = 1.0f;
    S32(p, 0x26D4) = 0;
    F32(p, 0x26D8) = -1000.0f;
    F32(p, 0x26DC) = -1000.0f;
    F32(p, 0x2684) = 9.81f;
    (*(CbFx**)(p + 0x2584))->Reset();
}

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

}  // extern "C"

// The physics solver's per-body records (masslist.cpp names them MassSet/ContactSet/CrossSet/CxEntry).
struct CbSolVec {
    f32 x, y, z, w;
};
struct CbSolMass {
    CbSolVec pos;     // offset 0x0
    CbSolVec rot[3];  // offset 0x10 (rows)
    CbSolVec quat;    // offset 0x40
    CbSolVec vel;     // offset 0x50
    CbSolVec avel;    // offset 0x60
    u8 pad70[0x20];
    CbSolVec force;   // offset 0x90
    CbSolVec torque;  // offset 0xA0
};
struct CbSolContact {
    CbSolVec v0;        // offset 0x0
    CbSolVec v10;       // offset 0x10
    CbSolVec v20;       // offset 0x20
    CbSolVec v30;       // offset 0x30
    CbSolVec pts[13];   // offset 0x40
    CbSolVec v110[4];   // offset 0x110
    CbSolVec v150[4];   // offset 0x150
    CbSolVec v190;      // offset 0x190
    CbSolVec v1A0;      // offset 0x1A0
    u8 pad1B0[0x40];
};
struct CbSolCross {
    CbSolVec pts[7];    // offset 0x0, body-space wheel points
    CbSolVec v70;       // offset 0x70
    CbSolVec v80;       // offset 0x80
    CbSolVec v90;       // offset 0x90
    CbSolVec vA0;       // offset 0xA0
    CbSolVec vB0;       // offset 0xB0
    u8 padC0[0x60];
    CbSolVec v120;      // offset 0x120
    CbSolVec v130;      // offset 0x130
    CbSolVec v140;      // offset 0x140
    CbSolVec v150;      // offset 0x150
    u8 pad160[0x30];
};
struct CbSolCx {
    s16 id;             // offset 0x0
    u8 pad2[0x2];
    s32 surface[4];     // offset 0x4, per wheel
};

// A wheel's per-step state in the car body (4 of them at 0x23AC).
struct CbWheel {
    f32 load;        // offset 0x0
    f32 loadN;       // offset 0x4
    f32 spin;        // offset 0x8
    f32 slip;        // offset 0xC
    f32 v10;         // offset 0x10
    f32 steer;       // offset 0x14
    f32 v18;         // offset 0x18
    f32 prevSlip;    // offset 0x1C
    u8 grounded;     // offset 0x20
    u8 pad21[0x3];
    f32 contact[3];  // offset 0x24
    f32 local[3];    // offset 0x30, body-space mount point
    f32 v3C;         // offset 0x3C
    f32 v40;         // offset 0x40
    f32 nRight;      // offset 0x44, ground normal . body right
    f32 nFwd;        // offset 0x48, ground normal . body forward
    f32 ground[3];   // offset 0x4C, contact plane anchor
    f32 normal[3];   // offset 0x58, contact plane normal
    f32 groundH;     // offset 0x64
    f32 comp;        // offset 0x68
    s8 sound;        // offset 0x6C
    u8 pad6D[0x3];
    u32 flags;       // offset 0x70
};

struct SurfType {
    f32 grip;   // offset 0x0
    f32 slip;   // offset 0x4
    f32 drag;   // offset 0x8
    s8 sound;   // offset 0xC
};

class CbFlagsV {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual u32 Flags(void);  // 0x14
};

class CbStepV {
public:
    virtual void v08(void);
    virtual void Step(void);  // 0xC
};

class CbTickV {
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
    virtual void v48(void);
    virtual void v4C(void);
    virtual void v50(void);
    virtual void v54(void);
    virtual void v58(void);
    virtual void Tick(void);  // 0x5C
};

static inline CbSolVec CbSolVec4(f32 x, f32 y, f32 z) {
    CbSolVec v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

static inline CbSolVec CbSolVec4(f32 x, f32 y, f32 z, f32 w) {
    CbSolVec v;
    v.x = x;
    v.y = y;
    v.z = z;
    v.w = w;
    return v;
}

class Corner {
public:
    Corner* next;
    void Hit(RigidBody* b);
};

extern "C" {
void fn_80190E6C(void);
u8 fn_801C5B60(void);
void fn_80196A9C(u8* p);
void fn_801C56AC(void);
void fn_801BEDA8(CbSolMass** m, CbSolContact** c, CbSolCross** x, CbSolCx** cx, s32* n);
void fn_801BEDD4(void);
void fn_801BF1B4(void);
SurfType* fn_801D04A8(int i);
f32 fn_802B03C0(const f32* p);
int fn_802B02C8(const u8* p);
int fn_802BF4F0(void);
f32 fn_802BF590(void);
void fn_8018CB3C(u8* p);
u32 fn_8018B06C(u8* p);
double asin(double x);
double fmod(double x, double y);
f32 PSVECMag(const void* v);
void PSVECNormalize(const void* a, void* b);
f32 PSVECDotProduct(const void* a, const void* b);
extern u8* lbl_8055E908;
extern const double lbl_80561E80;  // 2 pi
extern const double lbl_80561ED8;  // 4503599627370496 (int -> float bias)
extern const f32 lbl_80561CB0, lbl_80561CC0, lbl_80561CDC, lbl_80561DE4, lbl_80561E5C, lbl_80561CCC, lbl_80561CD0, lbl_80561CD8, lbl_80561CE4, lbl_80561CFC,
    lbl_80561D00, lbl_80561D04, lbl_80561D2C, lbl_80561D34, lbl_80561D38, lbl_80561D44, lbl_80561D50,
    lbl_80561D60, lbl_80561D98, lbl_80561DA0, lbl_80561DA4, lbl_80561DC0, lbl_80561DD0, lbl_80561DEC,
    lbl_80561E58, lbl_80561E60, lbl_80561E64, lbl_80561E68, lbl_80561E6C, lbl_80561E70, lbl_80561E74,
    lbl_80561E78, lbl_80561E7C, lbl_80561E88, lbl_80561E8C, lbl_80561E90, lbl_80561E94, lbl_80561E98,
    lbl_80561E9C, lbl_80561EA0, lbl_80561EA4, lbl_80561EA8, lbl_80561EAC, lbl_80561EB0, lbl_80561EB4,
    lbl_80561EB8, lbl_80561EBC, lbl_80561EC0, lbl_80561EC4, lbl_80561EC8, lbl_80561ECC, lbl_80561ED0;
}

static inline f32 Clamp01(f32 x) {
    f32 t = lbl_80561CD0 < x ? lbl_80561CD0 : x;
    return t > lbl_80561CAC ? t : lbl_80561CAC;
}

static inline void AddScaled(f32* f, const u8* dir, f32 k) {
    f[0] = f[0] + F32(dir, 0) * k;
    f[1] = f[1] + F32(dir, 4) * k;
    f[2] = f[2] + F32(dir, 8) * k;
}

struct CbEvent {
    s32 id;
    f32 a;
    f32 b;
    s32 c;
};

struct CbLeanEvent {
    s32 id;
    f32 lean;
    s32 dive;
};

struct CbSpinEvent {
    s32 id;
    u8 left;
};

// out = v[0] * row0 + v[1] * row1 + v[2] * row2 (m: rows at 0, 3, 6), safe when out aliases v.
static inline void ObRotateRows(BVec* out, const f32* m, const f32* v) {
    if ((const f32*)out == v) {
        f32 x = v[2] * m[6] + v[0] * m[0] + v[1] * m[3];
        f32 y = v[2] * m[7] + v[0] * m[1] + v[1] * m[4];
        f32 z = v[2] * m[8] + v[0] * m[2] + v[1] * m[5];
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = v[2] * m[6] + v[0] * m[0] + v[1] * m[3];
        out->y = v[2] * m[7] + v[0] * m[1] + v[1] * m[4];
        out->z = v[2] * m[8] + v[0] * m[2] + v[1] * m[5];
    }
}

int IsLoadingActive(void);

extern "C" {
void fn_801BEB1C(s32 id, CbSolContact** c);
void fn_801796CC(int kind, void* data, int size);
void fn_801796EC(int kind, void* data, int size);
int fn_80129D18(int a, int b);
f32 fn_80129DC4(int a, f32 lo, f32 hi);
f32 fn_801E9FDC(s16* cell, f32 x, f32 z);
int fn_802B04D4(const u8* p);
f32 fn_80181A94(void);
f32 fn_80181AC0(void);
void fn_8014E61C(s32 id, int kind, f32 v);
void fn_80161A38(int a, s32 id, int c);
int fn_8018A100(u8* p);
int fn_801E1048(s32 id);
f32 fn_801DDC8C(f32 x, f32 z);
f32 fn_801DCF48(BVec* n, f32 x, f32 z);
void fn_802E30C8(const char* fmt, ...);
extern char lbl_803325AC[];
extern s32 lbl_8055E3C4;
int fn_8014E708(s32 id);
void fn_801DF074(int a, int b);
f32 fn_801B8950(s32 id);
void fn_801B8964(s32 id, f32 v);
void fn_8018E330(u8* p);
f32 fn_801DCEC0(void);
double sqrt(double x);
void PSVECCrossProduct(const void* a, const void* b, void* out);
extern const f32 lbl_80561CB8, lbl_80561CEC, lbl_80561CF0, lbl_80561CF4, lbl_80561CF8, lbl_80561D08, lbl_80561D0C,
    lbl_80561D40, lbl_80561D64, lbl_80561D7C, lbl_80561D84, lbl_80561D94, lbl_80561D9C, lbl_80561DA8,
    lbl_80561DAC, lbl_80561DB0, lbl_80561DB4, lbl_80561DB8, lbl_80561DBC, lbl_80561DC4, lbl_80561DC8,
    lbl_80561DCC, lbl_80561DD4, lbl_80561DD8, lbl_80561DDC, lbl_80561DE0, lbl_80561DE8, lbl_80561DF0,
    lbl_80561DF4, lbl_80561DF8, lbl_80561DFC, lbl_80561E00, lbl_80561E04, lbl_80561E08, lbl_80561E0C,
    lbl_80561E10, lbl_80561E14, lbl_80561E18, lbl_80561E1C, lbl_80561E20, lbl_80561E24, lbl_80561E28,
    lbl_80561E2C, lbl_80561E30, lbl_80561E34, lbl_80561E38, lbl_80561E3C, lbl_80561E40, lbl_80561E44,
    lbl_80561E48, lbl_80561E4C;
}

extern "C" {

// Before a physics step: copy the solver's wheel ground points/normals in, run the timers (stun, wheelie, air,
// boost, heat, spin), apply the self-righting, stunt, landing, ramp, water, slope and steering forces/torques,
// track boost/overheat state and tricks (flips and spins counted while airborne), and drive the sound/FX hooks.
void fn_80183E7C(u8* p) {
    CbSolContact* sol;
    f32 airLift = lbl_80561CAC;
    if (IsLoadingActive()) {
        F32(p, 0x25DC) = lbl_80561CAC;
        F32(p, 0x25E0) = lbl_80561CAC;
        F32(p, 0x25E4) = lbl_80561CD0;
        U8(p, 0x260D) = 0;
    }
    if (U8(p, 0x25C0)) {
        F32(p, 0x25BC) = lbl_8055E4E0.a;
    }
    fn_801BEB1C(S32(p, 0x1EC), &sol);
    CbWheel* w = (CbWheel*)(p + 0x23AC);
    for (int i = 0; i < 4; i++, w++) {
        *(HVec*)w->ground = *(HVec*)&sol->v150[i];
        *(HVec*)w->normal = *(HVec*)&sol->v110[i];
    }
    if (!U8(p, 0x260D)) {
        F32(p, 0x2598) = lbl_80561CAC;
    } else {
        F32(p, 0x2598) = F32(p, 0x2598) + lbl_80561D9C;
    }
    f32 stun = F32(p, 0x2650) - lbl_80561D9C;
    stun = stun > lbl_80561CAC ? stun : lbl_80561CAC;
    F32(p, 0x2650) = stun;
    if ((f32)__fabs(stun) > lbl_80558DC4[0]) {
        if (F32(p, 0x25D4) < lbl_80561DA0) {
            f32 s = lbl_80561CDC;
            s = s < stun ? s : stun;
            F32(p, 0x2650) = s;
        }
        f32 roll;
        if (S32(p, 0x2620) > 2 && (roll = PSVECDotProduct(p + 0x124, p + 0x48), (f32)__fabs(roll) < lbl_80561CD8)) {
            f32 a = Clamp01((F32(p, 0x2650) - lbl_80561CAC) / lbl_80561CD0);
            f32 b = Clamp01((F32(p, 0x25D4) - lbl_80561DA4) / lbl_80561DA8);
            f32 k = (lbl_80561CAC + b * lbl_80561CD0) * lbl_80561DAC * lbl_80561D98 * lbl_80561CE4 * F32(p, 0xD4) *
                    (lbl_80561CAC + a * lbl_80561CD0);
            BVec t;
            t.x = F32(p, 0x48) * (roll > lbl_80561CAC ? k : -k);
            t.y = F32(p, 0x4C) * (roll > lbl_80561CAC ? k : -k);
            t.z = F32(p, 0x50) * (roll > lbl_80561CAC ? k : -k);
            ((RigidBody*)p)->AddTorque((const RVec3*)&t);
        }
    }
    if (F32(p, 0x258C) > lbl_80561CCC || U8(p, 0x259C)) {
        CbEvent ev;
        ev.id = S32(p, 0x1EC);
        ev.a = lbl_80561CD0;
        ev.b = F32(p, 0x25D0);
        ev.c = -1;
        fn_801796CC(4, &ev, sizeof(ev));
    }
    if (F32(p, 0x2608) - F32(p, 0x25E0) > lbl_80561D40) {
        F32(p, 0x2608) = lbl_80561CAC;
        if (fn_80129D18(4, 10) < 3) {
            (*(CbFx**)(p + 0x2584))->v1C();
            CbEvent ev;
            ev.id = S32(p, 0x1EC);
            ev.a = lbl_80561CD0;
            ev.b = F32(p, 0x25D0);
            ev.c = -1;
            fn_801796CC(4, &ev, sizeof(ev));
        }
    }
    F32(p, 0x2608) = F32(p, 0x25E0) + lbl_80561DB0 * (F32(p, 0x2608) - F32(p, 0x25E0));
    int still;
    if (!(U32(fn_801769CC(), 4) & 0x4000)) {
        still = (f32)__fabs(F32(p, 0x2690)) <= lbl_80558DC4[0];
    } else {
        still = !((f32)__fabs(F32(p, 0x2690) - lbl_80561CD0) <= lbl_80558DC4[0]);
    }
    if ((f32)__fabs(F32(p, 0x2650)) > lbl_80558DC4[0]) {
        still = 0;
    }
    if (still && (U32(p, 0x10) != 'PCAR' || ((CarHost*)PTR(p, 0x270))->v14())) {
        if (S32(p, 0x2620) > 2 || F32(p, 0x4C) >= lbl_80561DB4) {
            F32(p, 0x2614) = lbl_80561CAC;
        } else if (S32(p, 0x2620) == 0 || (f32)__fabs(F32(p, 0x2614)) > lbl_80558DC4[0] || F32(p, 0x4C) < lbl_80561CAC) {
            f32 t = F32(p, 0x2614) + lbl_80561D9C;
            int slow = F32(p, 0x25D4) < lbl_80561D50;
            F32(p, 0x2614) = t;
            if (slow) {
                F32(p, 0x2614) = t + lbl_80561DB8;
            }
        } else {
            F32(p, 0x2614) = lbl_80561CAC;
        }
        if (F32(p, 0x2614) >= lbl_80561D44) {
            if (F32(p, 0x4C) > lbl_80561CDC) {
                F32(p, 0x2614) = lbl_80561CAC;
            } else if ((f32)__fabs(F32(p, 0x58)) <= lbl_80561CDC) {
                f32 side = -F32(p, 0x40);
                if ((f32)__fabs(-F32(p, 0x40)) < lbl_80561CCC) {
                    side = lbl_80561D00;
                }
                BVec axis;
                axis.x = F32(p, 0x54) * side;
                axis.y = F32(p, 0x58) * side;
                axis.z = F32(p, 0x5C) * side;
                f32 k = lbl_80561DAC * lbl_80561CF4 * lbl_80561D98 * F32(p, 0xD4);
                BVec t;
                t.x = axis.x * k;
                t.y = axis.y * k;
                t.z = axis.z * k;
                f32 spin = lbl_80561CF8 * (PSVECDotProduct(p + 0x124, &axis) - lbl_80561CF0);
                f32 damp = lbl_80561CD0 - Clamp01(spin);
                t.x = t.x * damp;
                t.y = t.y * damp;
                t.z = t.z * damp;
                ((RigidBody*)p)->AddTorque((const RVec3*)&t);
            }
        }
    }
    if (S32(p, 0x2620) == 0) {
        S32(p, 0x2648) = 7;
    } else if (S32(p, 0x2620) < 3) {
        if (S32(p, 0x2648) == 0) {
            S32(p, 0x2648) = 1;
        }
    } else {
        S32(p, 0x2648) = 0;
        U8(p, 0x2624) = 0;
        U8(p, 0x2625) = 0;
    }
    if (S32(p, 0x2648) > 0 && S32(p, 0x2648) < 7) {
        BVec t;
        t.x = F32(p, 0x124) * lbl_80561DBC;
        t.y = F32(p, 0x128) * lbl_80561DBC;
        t.z = F32(p, 0x12C) * lbl_80561DBC;
        if ((f32)__fabs(F32(p, 0x2650)) <= lbl_80558DC4[0]) {
            ((RigidBody*)p)->AddTorque((const RVec3*)&t);
        }
        S32(p, 0x2648) = S32(p, 0x2648) + 1;
    }
    BVec a;
    BVec b;
    BVec up;
    a.x = F32(p, 0x23F8) - F32(p, 0x2554);
    a.y = F32(p, 0x23FC) - F32(p, 0x2558);
    a.z = F32(p, 0x2400) - F32(p, 0x255C);
    b.x = F32(p, 0x246C) - F32(p, 0x24E0);
    b.y = F32(p, 0x2470) - F32(p, 0x24E4);
    b.z = F32(p, 0x2474) - F32(p, 0x24E8);
    PSVECCrossProduct(&a, &b, &up);
    f32 nz = F32(p, 0x25AC) + lbl_80561D0C * (up.z - F32(p, 0x25AC));
    f32 ny = F32(p, 0x25A8) + lbl_80561D0C * (up.y - F32(p, 0x25A8));
    f32 nx = F32(p, 0x25A4) + lbl_80561D0C * (up.x - F32(p, 0x25A4));
    F32(p, 0x25A8) = ny;
    F32(p, 0x25A4) = nx;
    f32 len = PSVECMag(p + 0x25A4);
    F32(p, 0x25AC) = nz;
    if (len <= lbl_80561CCC) {
        F32(p, 0x25A4) = lbl_80561CAC;
        F32(p, 0x25A8) = lbl_80561CD0;
        F32(p, 0x25AC) = lbl_80561CAC;
    } else {
        f32 inv = lbl_80561CD0 / len;
        F32(p, 0x25A4) = F32(p, 0x25A4) * inv;
        F32(p, 0x25A8) = F32(p, 0x25A8) * inv;
        F32(p, 0x25AC) = F32(p, 0x25AC) * inv;
    }
    f32 dn;
    if (F32(p, 0x2590) > lbl_80561DC0 && (dn = PSVECDotProduct(p + 0x25A4, p + 0x118)) > lbl_80561DC0 &&
        dn < lbl_80561DA4 && S32(p, 0x2620) > 0) {
        f32 k = (lbl_80561DA4 - dn) * F32(p, 0xD4) / lbl_80561D9C;
        BVec f;
        f.x = F32(p, 0x25A4) * k;
        f.y = F32(p, 0x25A8) * k;
        f.z = F32(p, 0x25AC) * k;
        ((RigidBody*)p)->AddForceAt((const RVec3*)&f, (const RVec3*)(p + 0x60));
    }
    int stunts;
    u32 ramp = (U32(fn_801769CC(), 4) >> 20) & 1;
    u32 jump = (U32(fn_801769CC(), 4) >> 21) & 1;
    stunts = 0;
    if (ramp || jump) {
        stunts = 1;
    }
    if (stunts && S32(p, 0x2620) && F32(p, 0x2618) > lbl_80561DB4) {
        f32 sink = -PSVECDotProduct(p + 0x25A4, p + 0x118);
        f32 lim = landingTune.v[5] + Clamp01((F32(p, 0x2618) - lbl_80561DC4) / lbl_80561DC8) * (landingTune.v[4] - landingTune.v[5]);
        if (jump) {
            lim = driveTune.v[4];
        }
        BVec dir;
        dir.x = F32(p, 0x118);
        dir.y = F32(p, 0x11C);
        dir.z = F32(p, 0x120);
        f32 sp = PSVECMag(p + 0x118);
        sp = sp > lbl_80561CD0 ? sp : lbl_80561CD0;
        f32 inv = lbl_80561CD0 / sp;
        dir.x = dir.x * inv;
        dir.y = dir.y * inv;
        dir.z = dir.z * inv;
        f32 into = -PSVECDotProduct(&dir, p + 0x25A4);
        BVec behind;
        BVec ahead;
        ahead.x = F32(p, 0x60) + F32(p, 0x118) * lbl_80561CDC;
        behind.x = F32(p, 0x60) + F32(p, 0x118) * lbl_80561CB0;
        ahead.z = F32(p, 0x68) + F32(p, 0x120) * lbl_80561CDC;
        behind.z = F32(p, 0x68) + F32(p, 0x120) * lbl_80561CB0;
        ahead.y = F32(p, 0x64) + F32(p, 0x11C) * lbl_80561CDC;
        behind.y = F32(p, 0x64) + F32(p, 0x11C) * lbl_80561CB0;
        ahead.y = fn_801E9FDC((s16*)(p + 0x6C), ahead.x, ahead.z);
        behind.y = fn_801E9FDC((s16*)(p + 0x6C), behind.x, behind.z);
        BVec slope;
        slope.x = ahead.x - behind.x;
        slope.y = ahead.y - behind.y;
        slope.z = ahead.z - behind.z;
        f32 sl = PSVECMag(&slope);
        sl = sl > lbl_80561CCC ? sl : lbl_80561CCC;
        inv = lbl_80561CD0 / sl;
        slope.x = slope.x * inv;
        slope.y = slope.y * inv;
        slope.z = slope.z * inv;
        f32 flat = PSVECDotProduct(&slope, p + 0x25A4);
        int smooth = 0;
        if (lbl_80561CDC * (flat / lbl_80561DCC + into / lbl_80561D08) > lbl_80561CD0 && lbl_8055E4F6) {
            smooth = 1;
        }
        f32 tilt = PSVECDotProduct(p + 0x25A4, p + 0x48);
        int bad = 0;
        int hard = 0;
        if (!smooth && tilt > lim && F32(p, 0x2618) > landingTune.v[1]) {
            hard = 1;
        }
        if (hard && sink < landingTune.v[8]) {
            bad = 1;
        }
        if (!fn_802B04D4(p + 0x60)) {
            if (ramp) {
                if (tilt < lbl_80561DD0 && F32(p, 0x2618) > lbl_80561CDC) {
                    if ((f32)__fabs(lbl_8055E4F0) > lbl_80558DC4[0]) {
                        (*(CbFx**)(p + 0x2584))->v24();
                    }
                    F32(p, 0x2590) = lbl_8055E4F0;
                    fn_8014E61C(S32(p, 0x1EC), 2, lbl_80561DD0 - tilt);
                } else if (!bad || (f32)__fabs(F32(p, 0x2690)) > lbl_80558DC4[0]) {
                    (*(CbFx**)(p + 0x2584))->Land(sink);
                } else {
                    U8(p, 0x259D) = 0;
                    f32 b2;
                    f32 boost = landingTune.v[0];
                    b2 = F32(p, 0x2618) * fn_80181A94();
                    b2 = F32(p, 0x2CC) * fn_80181AC0() + b2;
                    boost = boost < b2 ? boost : b2;
                    F32(p, 0x258C) = F32(p, 0x3A0) * boost;
                    fn_8014E61C(S32(p, 0x1EC), 1, sink);
                    fn_80161A38(2, S32(p, 0x1EC), 0);
                    (*(CbFx**)(p + 0x2584))->v20();
                }
            }
            if (jump && fn_8018A100(p) && fn_801E1048(S32(p, 0x1EC))) {
                fn_8014E61C(S32(p, 0x1EC), 0, lbl_80561CAC);
                if (bad) {
                    fn_8014E61C(S32(p, 0x1EC), 1, sink);
                }
            }
        }
    }
    if (F32(p, 0x2618) > lbl_80561DB4) {
        f32 sink = -PSVECDotProduct(p + 0x25A4, p + 0x118);
        if (sink > lbl_80561DD4 &&
            (F32(p, 0x64) - F32(p, 0x2558) - lbl_80561D2C * lbl_80561D9C * sink < lbl_80561CAC || S32(p, 0x2620))) {
            f32 d = lbl_80561CAC;
            if (sink - lbl_80561DD4 > lbl_80561CAC) {
                d = sink - lbl_80561DD4;
            }
            f32 k = d * F32(p, 0xD4) / lbl_80561D9C;
            BVec f;
            f.x = F32(p, 0x25A4) * k;
            f.y = F32(p, 0x25A8) * k;
            f.z = F32(p, 0x25AC) * k;
            ((RigidBody*)p)->AddForceAt((const RVec3*)&f, (const RVec3*)(p + 0x60));
        }
    }
    if (S32(p, 0x2620) && F32(p, 0x2618) > lbl_80561DB4) {
        BVec right;
        right.x = F32(p, 0x3C);
        right.y = F32(p, 0x40);
        right.z = F32(p, 0x44);
        f32 slide = -PSVECDotProduct(&right, p + 0x118);
        f32 t = Clamp01((F32(p, 0x2618) - lbl_80561DB4) / lbl_80561DD8);
        f32 d = lbl_80561CAC;
        if (slide > lbl_80561CAC) {
            if (slide - lbl_80561DDC > lbl_80561CAC) {
                d = slide - lbl_80561DDC;
            }
        } else if (lbl_80561DDC + slide < lbl_80561CAC) {
            d = lbl_80561DDC + slide;
        }
        f32 k = (lbl_80561CAC + t * lbl_80561CDC) * d * F32(p, 0xD4) / lbl_80561D9C;
        BVec f;
        f.x = right.x * k;
        f.y = right.y * k;
        f.z = right.z * k;
        f32 n = -PSVECDotProduct(p + 0x25A4, &f);
        f.x = f.x + F32(p, 0x25A4) * n;
        f.y = f.y + F32(p, 0x25A8) * n;
        f.z = f.z + F32(p, 0x25AC) * n;
        ((RigidBody*)p)->AddForceAt((const RVec3*)&f, (const RVec3*)(p + 0x60));
    }
    if ((f32)__fabs(F32(p, 0x2690)) <= lbl_80558DC4[0] && F32(p, 0x2618) > lbl_80561DE0 &&
        (f32)__fabs(F32(p, 0x2650)) <= lbl_80558DC4[0]) {
        f32 stall = lbl_80561D34 * F32(p, 0x264C);
        stall = stall < lbl_80561CD0 ? stall : lbl_80561CD0;
        f32 pitch = F32(p, 0x2600) + lbl_80561D64 * stall * fn_80129DC4(4, lbl_80561DE4, lbl_80561CD0);
        f32 roll = F32(p, 0x2604);
        BVec fl;
        fl.x = F32(p, 0x118);
        fl.y = lbl_80561CAC;
        fl.z = F32(p, 0x120);
        f32 m = PSVECMag(&fl);
        m = m > lbl_80561CD0 ? m : lbl_80561CD0;
        f32 inv = lbl_80561CD0 / m;
        fl.x = fl.x * inv;
        fl.y = fl.y * inv;
        fl.z = fl.z * inv;
        if (PSVECDotProduct(&fl, p + 0x54) < lbl_80561DCC) {
            fl.x = fl.x * lbl_80561DE4;
            fl.y = fl.y * lbl_80561DE4;
            fl.z = fl.z * lbl_80561DE4;
        }
        f32 nose = PSVECDotProduct(&fl, p + 0x48);
        f32 wantR;
        f32 wantP = lbl_80561CFC * pitch - nose;
        wantR = -roll - F32(p, 0x40) * (lbl_80561CD0 - (f32)__fabs(F32(p, 0x58)));
        f32 wp = PSVECDotProduct(p + 0x124, p + 0x3C);
        f32 wr = PSVECDotProduct(p + 0x124, p + 0x54);
        f32 gain = lbl_80561CFC;
        if (U8(p, 0x2680)) {
            gain = lbl_80561CD0;
        }
        f32 lead = lbl_80561CF8 * (lbl_80561CAC + Clamp01((F32(p, 0x2618) - lbl_80561DE0) / lbl_80561DB4) * lbl_80561CDC);
        if (!U8(p, 0x2680)) {
            lead = lbl_80561CAC;
        }
        f32 tp = gain * F32(p, 0xD8) * ((wantP - wp) + lead * (pitch - nose)) / lbl_80561D9C;
        f32 tr = gain * F32(p, 0xE0) * (wantR - wr) / lbl_80561D9C;
        BVec t;
        t.x = F32(p, 0x3C) * tp + F32(p, 0x54) * tr;
        t.y = F32(p, 0x40) * tp + F32(p, 0x58) * tr;
        t.z = F32(p, 0x44) * tp + F32(p, 0x5C) * tr;
        ((RigidBody*)p)->AddTorque((const RVec3*)&t);
    }
    if (S32(p, 0x2648) == 7) {
        BVec y(lbl_80561CAC, lbl_80561CD0, lbl_80561CAC);
        BVec side;
        PSVECCrossProduct(&y, p + 0x54, &side);
        side.y = lbl_80561CAC;
        f32 k = lbl_80561CDC * -PSVECDotProduct(p + 0x118, &side) * F32(p, 0xD4);
        BVec f;
        f.x = side.x * k;
        f.y = side.y * k;
        f.z = side.z * k;
        ((RigidBody*)p)->AddForceAt((const RVec3*)&f, (const RVec3*)(p + 0x60));
    }
    f32 stalled = F32(p, 0x264C);
    F32(p, 0x264C) = lbl_80561CAC;
    if (S32(p, 0x2648) == 7) {
        if (!U8(p, 0x2624) && U8(p, 0x260D) && F32(p, 0x2598) < lbl_80561DE0 && F32(p, 0x2618) >= lbl_80561DE8 &&
            F32(p, 0x2618) <= lbl_80561CFC && F32(p, 0x25D4) > lbl_80561DEC) {
            U8(p, 0x2624) = 1;
        }
        if (U8(p, 0x2624) && F32(p, 0x2618) >= lbl_80561CB8 && (U8(p, 0x2624) = 0, F32(p, 0x11C) > lbl_80561DDC) &&
            (f32)__fabs(F32(p, 0x2588)) <= lbl_80558DC4[0] && (f32)__fabs(F32(p, 0x2690)) <= lbl_80558DC4[0]) {
            s32 id = S32(p, 0x1EC);
            U8(p, 0x2625) = 1;
            fn_801796EC(13, &id, 4);
            (*(CbFx**)(p + 0x2584))->v28();
            fn_8014E61C(S32(p, 0x1EC), 6, lbl_80561CAC);
            fn_80161A38(1, S32(p, 0x1EC), 0);
            BVec kick(lbl_80561CAC, lbl_80561D34 * F32(p, 0xD4), lbl_80561CAC);
            ((RigidBody*)p)->AddImpulse(lbl_80561CD0, (const RVec3*)&kick, (const RVec3*)(p + 0x60));
        }
        f32 top = lbl_80561D60 * fn_80181A48(p);
        f32 cap = lbl_80561DD0 * top;
        if (F32(p, 0x261C) < cap) {
            cap = F32(p, 0x261C);
        }
        F32(p, 0x261C) = cap;
        f32 flat = sqrt(F32(p, 0x118) * F32(p, 0x118) + F32(p, 0x120) * F32(p, 0x120));
        int nosedive = 0;
        f32 dive = -F32(p, 0x2600);
        if (lbl_8055A3AC && U32(p, 0x10) == 'PCAR' && dive > lbl_80561CAC && dive > lbl_80561DD0) {
            f32 st = lbl_80561CD0;
            if (lbl_80561D9C + stalled < lbl_80561CD0) {
                st = lbl_80561D9C + stalled;
            }
            F32(p, 0x264C) = st;
            nosedive = 1;
            dive = lbl_80561DD0 + Clamp01((st - lbl_80561CDC) / lbl_80561CDC) * lbl_80561DF0;
        }
        f32 lean = lbl_80561CEC * dive;
        lean = lean < lbl_80561CD0 ? lean : lbl_80561CD0;
        lean = lean > lbl_80561DE4 ? lean : lbl_80561DE4;
        if (F32(p, 0x2618) > lbl_80561CD0) {
            CbLeanEvent ev;
            ev.id = S32(p, 0x1EC);
            ev.lean = lean;
            ev.dive = nosedive;
            fn_801796CC(15, &ev, sizeof(ev));
        }
        f32 target;
        if (lean >= lbl_80561CAC) {
            if (lbl_80561DF8 * F32(p, 0x261C) < top) {
                top = lbl_80561DF8 * F32(p, 0x261C);
            }
            target = F32(p, 0x261C) + Clamp01((lean - lbl_80561CAC) / lbl_80561DB4) * (top - F32(p, 0x261C));
        } else {
            f32 t = Clamp01((lean - lbl_80561CAC) / lbl_80561DF4);
            target = F32(p, 0x261C) + t * (lbl_80561DD0 * F32(p, 0x261C) - F32(p, 0x261C));
            airLift = airLift + lbl_8055A3B0 * -lean;
        }
        BVec dir;
        dir.x = F32(p, 0x118);
        dir.y = lbl_80561CAC;
        dir.z = F32(p, 0x120);
        f32 want = target - lbl_80561DA0 * F32(p, 0x25E4);
        f32 m = PSVECMag(&dir);
        m = m > lbl_80561CCC ? m : lbl_80561CCC;
        f32 one = lbl_80561CD0;
        f32 inv = one / m;
        dir.x = dir.x * inv;
        dir.y = dir.y * inv;
        dir.z = dir.z * inv;
        if (want < flat) {
            one = one + lbl_80561D2C * F32(p, 0x25E4);
        }
        f32 k = one * ((want - flat) * F32(p, 0xD4));
        BVec f;
        f.x = dir.x * k;
        f.y = dir.y * k;
        f.z = dir.z * k;
        ((RigidBody*)p)->AddForceAt((const RVec3*)&f, (const RVec3*)(p + 0x60));
    }
    if (U32(p, 0x10) == 'PCAR') {
        f32 x = F32(p, 0x60);
        f32 z = F32(p, 0x68);
        f32 floor = fn_801DDC8C(F32(p, 0x60), F32(p, 0x68));
        floor = floor > lbl_80561CAC ? floor : lbl_80561CAC;
        f32 t = F32(p, 0x2688) - lbl_80561DFC;
        t = t > lbl_80561CAC ? t : lbl_80561CAC;
        F32(p, 0x2688) = t;
        if (floor > lbl_80561E00) {
            BVec f;
            if (F32(p, 0x64) - lbl_80561E04 < fn_801DCF48(&f, x, z) && F32(p, 0x11C) < floor) {
                f32 push = lbl_80561E08 + (floor - F32(p, 0x11C));
                f.x = lbl_80561CAC;
                f.z = lbl_80561CAC;
                f.y = push * F32(p, 0xD4) / lbl_80561D9C;
                ((RigidBody*)p)->AddForceAt((const RVec3*)&f, (const RVec3*)(p + 0x60));
                fn_802E30C8(lbl_803325AC, lbl_8055E3C4, push / lbl_80561D60, floor / lbl_80561D60);
                if ((f32)__fabs(F32(p, 0x2688)) <= lbl_80558DC4[0]) {
                    fn_8014E61C(S32(p, 0x1EC), 8, lbl_80561CAC);
                }
                F32(p, 0x2688) = lbl_80561CD0;
            }
            if ((f32)__fabs(F32(p, 0x2688)) > lbl_80558DC4[0]) {
                F32(p, 0x2688) = lbl_80561CD0;
            }
        } else if ((f32)__fabs(t) > lbl_80558DC4[0]) {
            if (F32(p, 0x11C) > lbl_80561E0C) {
                F32(p, 0x2688) = lbl_80561CD0;
            }
            if (S32(p, 0x2620) == 4 || (f32)__fabs(F32(p, 0x2690)) > lbl_80558DC4[0]) {
                F32(p, 0x2688) = lbl_80561CAC;
            }
        }
    }
    if (F32(p, 0x258C) > lbl_80558DC4[0] && !U8(p, 0x259D) && F32(p, 0x2618) < lbl_80561D00 &&
        F32(p, 0x25D4) > lbl_80561DA4) {
        s32 id = S32(p, 0x1EC);
        fn_801796CC(14, &id, 4);
    }
    f32 t2588 = F32(p, 0x2588) - lbl_80561D9C;
    if (t2588 < lbl_80561CAC) {
        t2588 = lbl_80561CAC;
    }
    f32 t2590 = F32(p, 0x2590) - lbl_80561D9C;
    F32(p, 0x2588) = t2588;
    if (t2590 < lbl_80561CAC) {
        t2590 = lbl_80561CAC;
    }
    f32 boost = F32(p, 0x258C) - lbl_80561D9C;
    F32(p, 0x2590) = t2590;
    if (boost < lbl_80561E10) {
        boost = lbl_80561E10;
    }
    F32(p, 0x258C) = boost;
    f32 pad = F32(p, 0x25BC);
    if (pad > lbl_80558DC4[0]) {
        if (pad < boost) {
            pad = boost;
        }
        f32 left = F32(p, 0x25BC) - lbl_80561D9C;
        F32(p, 0x258C) = pad;
        if (left < lbl_80561CAC) {
            left = lbl_80561CAC;
        }
        F32(p, 0x25BC) = left;
        if (!fn_8014E708(S32(p, 0x1EC))) {
            s32 id = S32(p, 0x1EC);
            fn_801796CC(16, &id, 4);
        }
    }
    if (S32(p, 0x2620) > 0 && F32(p, 0x2618) > lbl_80561D64 && lbl_8055E4F4 && S32(p, 0x1F0) == 0x50) {
        fn_801DF074(5, 1);
    }
    if (S32(p, 0x2620) == 0) {
        F32(p, 0x2618) = F32(p, 0x2618) + lbl_80561D9C;
    } else {
        F32(p, 0x2618) = lbl_80561CAC;
        sqrt(F32(p, 0x118) * F32(p, 0x118) + F32(p, 0x120) * F32(p, 0x120));
        F32(p, 0x261C) = F32(p, 0x25D4);
    }
    int sunk = 0;
    if (fn_802BF4F0()) {
        sunk = 1;
        CbWheel* wh = (CbWheel*)(p + 0x23AC);
        for (int i = 0; i < 4; i++, wh++) {
            CbWheel c = *wh;
            BVec pt;
            ObRotateRows(&pt, (const f32*)(p + 0x3C), c.local);
            pt.x = pt.x + F32(p, 0x60);
            pt.y = pt.y + F32(p, 0x64);
            pt.z = pt.z + F32(p, 0x68);
            if (pt.y > fn_802BF590()) {
                sunk = 0;
                break;
            }
        }
    }
    if (sunk) {
        F32(p, 0x2588) = lbl_80561CAC;
    }
    int deep = 0;
    if (fn_802BF4F0() && F32(p, 0x64) < lbl_80561CFC + fn_802BF590()) {
        deep = 1;
    }
    if (F32(p, 0x25C4) > lbl_80561CCC || (S32(p, 0x2620) > 0 && fn_802B04D4(p + 0x60) && deep)) {
        if ((f32)__fabs(F32(p, 0x25C4)) <= lbl_80558DC4[0]) {
            (*(CbFx**)(p + 0x2584))->v30();
            if ((f32)__fabs(F32(p, 0x2690)) <= lbl_80558DC4[0]) {
                fn_8018E330(p);
            }
        }
        f32 k = lbl_80561CB0 * F32(p, 0xD4);
        BVec f;
        f.x = F32(p, 0x118) * k;
        F32(p, 0x25C4) = F32(p, 0x25C4) + lbl_80561D9C;
        f.y = F32(p, 0x11C) * k;
        f.z = F32(p, 0x120) * k;
        ((RigidBody*)p)->AddForceAt((const RVec3*)&f, (const RVec3*)(p + 0x60));
        f32 surf = lbl_80561DB4 + fn_802BF590();
        f32 grow = lbl_80561CD0 + Clamp01((F32(p, 0x25C4) - lbl_80561D84) / lbl_80561D7C) * lbl_80561E14;
        BVec front;
        front.x = F32(p, 0x60) + F32(p, 0x54) * lbl_80561CD8;
        front.y = F32(p, 0x64) + F32(p, 0x58) * lbl_80561CD8;
        f32 df = Clamp01((front.y - surf) / (surf - lbl_80561D08 - surf));
        front.z = F32(p, 0x68) + F32(p, 0x5C) * lbl_80561CD8;
        f32 tf = Clamp01((F32(p, 0x25C4) - lbl_80561CAC) / lbl_80561CB8);
        BVec ff;
        ff.x = lbl_80561CAC;
        ff.z = lbl_80561CAC;
        ff.y = (lbl_80561CAC + df * lbl_80561CD0) * (lbl_80561D40 + tf * lbl_80561D7C) * (lbl_80561CDC * grow) * F32(p, 0xD4) /
               lbl_80561D9C;
        ((RigidBody*)p)->AddForceAt((const RVec3*)&ff, (const RVec3*)&front);
        BVec back;
        back.x = F32(p, 0x60) + F32(p, 0x54) * lbl_80561CB0;
        back.y = F32(p, 0x64) + F32(p, 0x58) * lbl_80561CB0;
        f32 db = Clamp01((back.y - surf) / (surf - lbl_80561D08 - surf));
        back.z = F32(p, 0x68) + F32(p, 0x5C) * lbl_80561CB0;
        f32 tb = Clamp01((F32(p, 0x25C4) - lbl_80561D40) / lbl_80561D38);
        BVec fb;
        fb.x = lbl_80561CAC;
        fb.z = lbl_80561CAC;
        fb.y = (lbl_80561CAC + db * lbl_80561CD0) * (lbl_80561D00 + tb * lbl_80561DD0) * (lbl_80561CB8 * grow) * F32(p, 0xD4) /
               lbl_80561D9C;
        ((RigidBody*)p)->AddForceAt((const RVec3*)&fb, (const RVec3*)&back);
    } else {
        F32(p, 0x25C4) = lbl_80561CAC;
    }
    f32 cool = heatTune.v[1];
    if (F32(p, 0x2618) < lbl_80561D08) {
        cool = heatTune.v[0];
        if (sunk) {
            cool = heatTune.v[2];
        }
    }
    f32 heat = F32(p, 0x25A0) - lbl_80561D9C * cool;
    if (heat < lbl_80561CAC) {
        heat = lbl_80561CAC;
    }
    F32(p, 0x25A0) = heat;
    U8(p, 0x259C) = 0;
    if (lbl_8055E4F4) {
        if ((f32)__fabs(F32(p, 0x2690)) > lbl_80558DC4[0] && U32(p, 0x10) == 'PCAR') {
            f32 h = heatTune.v[7];
            if (heat < h) {
                h = heat;
            }
            F32(p, 0x25A0) = h;
        }
        if (F32(p, 0x258C) < lbl_80561D00 && U8(p, 0x260D) && F32(p, 0x2618) < lbl_80561D40 &&
            F32(p, 0x25A0) < lbl_80561E18 && F32(p, 0x2588) < lbl_80561DC0 &&
            (f32)__fabs(F32(p, 0x2690)) <= lbl_80558DC4[0]) {
            f32 h = F32(p, 0x25A0) + lbl_80561D9C * F32(p, 0x398) * heatTune.v[3] * F32(p, 0x2D4);
            h = h < lbl_80561CD0 ? h : lbl_80561CD0;
            F32(p, 0x25A0) = h;
            U8(p, 0x259C) = 1;
        }
        if (S32(p, 0x2620) > 2) {
            int boosting = 1;
            if (F32(p, 0x258C) <= lbl_80561DC0 && !U8(p, 0x259C)) {
                boosting = 0;
            }
            if (!boosting || F32(p, 0x25E0) <= lbl_80561CDC) {
                f32 vmax = F32(p, 0x5A8);
                if (F32(p, 0x25D4) > vmax) {
                    f32 over = Clamp01((F32(p, 0x25D4) - vmax) / (lbl_80561DDC + vmax - vmax));
                    f32 k = heatTune.v[12] * lbl_80561D98 * -(lbl_80561CAC + over * lbl_80561CD0) * F32(p, 0xD4) /
                            F32(p, 0x25D4);
                    BVec f;
                    f.x = F32(p, 0x118) * k;
                    f.y = F32(p, 0x11C) * k;
                    f.z = F32(p, 0x120) * k;
                    if (U32(p, 0x10) == 'PCAR') {
                        ((RigidBody*)p)->AddForceAt((const RVec3*)&f, (const RVec3*)(p + 0x60));
                    }
                }
            }
        }
        if (F32(p, 0x25A0) > lbl_80561DB0 && F32(p, 0x2588) < lbl_80561CCC) {
            F32(p, 0x25A0) = heatTune.v[6];
            F32(p, 0x2588) = heatTune.v[8];
            fn_801DF074(6, 1);
            (*(CbFx**)(p + 0x2584))->v14();
        }
    }
    f32 vol = fn_801B8950(S32(p, 0x1EC));
    int loud = 0;
    if (F32(p, 0x25E0) > lbl_80561CDC && F32(p, 0x258C) > lbl_80561DC0) {
        loud = 1;
    }
    f32 want = lbl_80561CAC;
    if (loud) {
        want = lbl_80561CD0;
    }
    f32 dv = want - vol;
    dv = dv < lbl_80561DFC ? dv : lbl_80561DFC;
    f32 dvc = lbl_80561E1C;
    if (dv > lbl_80561E1C) {
        dvc = dv;
    }
    fn_801B8964(S32(p, 0x1EC), vol + dvc);
    int trick = 0;
    if ((f32)__fabs(F32(p, 0x2690)) <= lbl_80558DC4[0] && (f32)__fabs(F32(p, 0x2650)) <= lbl_80558DC4[0]) {
        f32 yaw = lbl_80561E20 * F32(p, 0x25D8) * F32(p, 0x25F0) * F32(p, 0x5B0);
        f32 sp = PSVECMag(p + 0x118);
        f32 m = lbl_80561CD0;
        if (sp > m) {
            m = sp;
        }
        BVec dir;
        f32 inv = lbl_80561CD0 / m;
        dir.x = F32(p, 0x118) * inv;
        dir.y = F32(p, 0x11C) * inv;
        dir.z = F32(p, 0x120) * inv;
        if (U8(p, 0x2680)) {
            BVec d2;
            d2.y = lbl_80561CFC * dir.y;
            d2.x = dir.x;
            d2.z = dir.z;
            BVec side;
            PSVECCrossProduct(&d2, p + 0x54, &side);
            f32 g = lbl_80561CD0;
            if ((s32)fn_8018B06C(p) < 0) {
                g = lbl_80561CAC;
            }
            f32 st = F32(p, 0x25F0) * g;
            f32 lat = PSVECDotProduct(&side, p + 0x48);
            f32 t = Clamp01((F32(p, 0x25D4) - lbl_80561E24) / lbl_80561E28);
            yaw = yaw + t * (lbl_80561D44 * lbl_80561DEC * (lbl_80561D38 * st - lat) - yaw);
        }
        f32 now = lbl_8055E3C8;
        if (S32(p, 0x2638) == 0 && S32(p, 0x2644) > 0 &&
            (now - F32(p, 0x2634) > lbl_80561D38 || F32(p, 0x2618) < lbl_80561D00)) {
            fn_8014E61C(S32(p, 0x1EC), 7, (f32)S32(p, 0x2644));
            S32(p, 0x2644) = 0;
            trick = 1;
        }
        if (F32(p, 0x2618) < lbl_80561CDC) {
            S32(p, 0x263C) = 0;
            if (S32(p, 0x2638)) {
                S32(p, 0x2638) = 0;
                if (S32(p, 0x2644) > 1) {
                    fn_8014E61C(S32(p, 0x1EC), 7, (f32)(S32(p, 0x2644) - 1));
                    trick = 1;
                }
            }
            S32(p, 0x2644) = 0;
        }
        if (S32(p, 0x2638)) {
            CbSpinEvent ev;
            ev.id = S32(p, 0x1EC);
            ev.left = S32(p, 0x2638) == 1;
            fn_801796CC(17, &ev, sizeof(ev));
        }
        if (!lbl_8055A3B4 || U32(p, 0x10) != 'PCAR' || F32(p, 0x2618) <= lbl_80561CDC || F32(p, 0x25D4) <= lbl_80561DD4 ||
            S32(p, 0x2638)) {
            if (S32(p, 0x2638)) {
                f32 sgn = lbl_80561CD0;
                if (S32(p, 0x2638) == 1) {
                    sgn = lbl_80561DE4;
                }
                BVec fwd;
                fwd.x = F32(p, 0x54);
                yaw = lbl_80561E30 * sgn;
                fwd.z = F32(p, 0x5C);
                fwd.y = lbl_80561CAC;
                BVec v;
                v.x = dir.x;
                v.z = dir.z;
                v.y = lbl_80561CAC;
                if (PSVECDotProduct(&fwd, &v) < lbl_80561CAC) {
                    U8(p, 0x2640) = 1;
                } else if (U8(p, 0x2640)) {
                    S32(p, 0x2638) = 0;
                    F32(p, 0x2628) = lbl_80561CAC;
                    F32(p, 0x262C) = lbl_80561CAC;
                    F32(p, 0x2634) = now;
                }
            }
        } else {
            int held = 0;
            if (F32(p, 0x25E4) > lbl_80561D40 && U8(p, 0x260D)) {
                held = 1;
            }
            if (F32(p, 0x25F0) < lbl_80561E2C) {
                F32(p, 0x2628) = now;
            }
            if (F32(p, 0x25F0) > lbl_80561D64) {
                F32(p, 0x262C) = now;
            }
            if (!held) {
                F32(p, 0x2630) = now;
            }
            if (S32(p, 0x2644) == 0 && !held) {
                F32(p, 0x2628) = lbl_80561D94;
                F32(p, 0x262C) = lbl_80561D94;
            }
            f32 l = F32(p, 0x2628);
            f32 r = F32(p, 0x262C);
            f32 gap = l - r;
            if (l < r) {
                r = l;
            }
            int side = F32(p, 0x2628) <= F32(p, 0x262C) ? 2 : 1;
            if ((f32)__fabs(gap) < lbl_80561D38 && now - r < lbl_80561CD0 && held &&
                (S32(p, 0x263C) == 0 || side == S32(p, 0x263C))) {
                S32(p, 0x2638) = side;
                S32(p, 0x263C) = side;
                U8(p, 0x2640) = 0;
                f32 spin = PSVECDotProduct(p + 0x124, p + 0x48);
                f32 sgn = lbl_80561CD0;
                if (S32(p, 0x2638) == 1) {
                    sgn = lbl_80561DE4;
                }
                if (sgn * spin >= lbl_80561DE8 || S32(p, 0x2644)) {
                    F32(p, 0x2628) = now;
                    F32(p, 0x262C) = lbl_80561CAC;
                    S32(p, 0x2644) = S32(p, 0x2644) + 1;
                } else {
                    S32(p, 0x2638) = 0;
                }
            }
        }
        f32 spin = PSVECDotProduct(p + 0x124, p + 0x48);
        f32 fs = Clamp01((F32(p, 0x25D4) - lbl_80561E00) / lbl_80561E34);
        f32 fa = Clamp01((F32(p, 0x2618) - lbl_80561D00) / lbl_80561E40);
        f32 k = lbl_80561E3C * F32(p, 0x5E4) * F32(p, 0x5E4) * (lbl_80561CD0 + fa * lbl_80561E14) *
                (yaw * (lbl_80561CD0 + fs * lbl_80561E38) - spin);
        BVec t;
        t.x = F32(p, 0x48) * k;
        t.y = F32(p, 0x4C) * k;
        t.z = F32(p, 0x50) * k;
        ((RigidBody*)p)->AddTorque((const RVec3*)&t);
    }
    int spins = S32(p, 0x2644) - (S32(p, 0x2638) != 0);
    if (spins > 0 && !trick) {
        fn_8014E61C(S32(p, 0x1EC), 9, (f32)spins);
    }
    if (S32(p, 0x2620) < 3) {
        f32 grav = F32(p, 0x5DC);
        f32 gravUp = F32(p, 0x5E0);
        if (S32(p, 0x2620) == 0) {
            f32 pitch = F32(p, 0x2600);
            if (pitch < lbl_80561E44) {
                grav = grav + __fabsf(pitch) * (lbl_80561CDC * grav - grav);
                gravUp = gravUp + (f32)__fabs(pitch) * (lbl_80561CDC * gravUp - gravUp);
            } else if (pitch > lbl_80561CCC) {
                grav = grav + __fabsf(pitch) * (lbl_80561CDC + grav - grav);
                gravUp = gravUp + (f32)__fabs(pitch) * (lbl_80561CDC + gravUp - gravUp);
            }
        }
        if (!U8(p, 0x2625) && F32(p, 0x2618) >= lbl_80561CB8) {
            f32 s = lbl_80561CD0 + F32(p, 0x5F0) * (lbl_8055E4D8 - lbl_80561CD0);
            grav = grav * s;
            gravUp = gravUp * s;
        }
        if (F32(p, 0x11C) > lbl_80561CAC) {
            gravUp = grav;
        }
        airLift = airLift + gravUp;
        BVec f;
        f.x = lbl_80561CAC;
        f.z = lbl_80561CAC;
        f.y = airLift * (lbl_80561D98 * -F32(p, 0xD4));
        ((RigidBody*)p)->AddForceAt((const RVec3*)&f, (const RVec3*)(p + 0x60));
        F32(p, 0x2684) = lbl_80561D98 * (lbl_80561CD0 + airLift);
    }
    f32 depth = fn_801DCEC0();
    F32(p, 0x26D0) = depth;
    int fast = F32(p, 0x25D4) >= lbl_80561CDC;
    F32(p, 0x26CC) = F32(p, 0x26CC) + lbl_80561D00 * (depth - F32(p, 0x26CC));
    if (fast || F32(p, 0x25F4) >= lbl_80561D00 || F32(p, 0x25E4) <= lbl_80561DD0) {
        F32(p, 0x26A8) = lbl_80561CAC;
    } else {
        F32(p, 0x26A8) = F32(p, 0x26A8) + lbl_80561D9C;
    }
    u8 stuck = F32(p, 0x26A8) > lbl_80561CD8;
    if (stuck != U8(p, 0x26AC)) {
        U8(p, 0x26AC) = stuck;
        fn_801BF1B8(PTR(p, 0x1EC), stuck);
    }
    if (U8(p, 0x26E0)) {
        f32 sy = F32(p, 0x26E8) * lbl_80561E48;
        f32 vy = F32(p, 0x26F4) * lbl_80561E10;
        f32 sz = F32(p, 0x26EC) * lbl_80561E48;
        f32 vz = F32(p, 0x26F8) * lbl_80561E10;
        f32 ax = F32(p, 0x26E4) * lbl_80561E48 + F32(p, 0x26F0) * lbl_80561E10;
        if (S32(p, 0x2620) > 0) {
            f32 r = lbl_80561CD8 * F32(p, 0x2604);
            F32(p, 0x26FC) = F32(p, 0x2600);
            ax = ax + r;
        }
        BVec acc;
        ObRotateRows(&acc, (const f32*)(p + 0x3C), (const f32*)(p + 0x16C));
        f32 ay = sy + vy + lbl_80561D34 * F32(p, 0x26FC) * (lbl_80561CD0 - F32(p, 0x25F0) * F32(p, 0x25F0)) +
                 acc.y * lbl_80561E4C;
        ax = ax + acc.x * lbl_80561E4C;
        f32 az = sz + vz + acc.z * lbl_80561E4C;
        f32 nvx = F32(p, 0x26F0) + ax * lbl_80561D9C;
        f32 nvy = F32(p, 0x26F4) + ay * lbl_80561D9C;
        f32 dx = nvx * lbl_80561D9C;
        f32 nvz = F32(p, 0x26F8) + az * lbl_80561D9C;
        f32 dy = nvy * lbl_80561D9C;
        f32 dz = nvz * lbl_80561D9C;
        int lean = F32(p, 0x26FC) > lbl_80561CB8;
        F32(p, 0x26F0) = nvx;
        F32(p, 0x26F4) = nvy;
        F32(p, 0x26F8) = nvz;
        F32(p, 0x26E4) = F32(p, 0x26E4) + dx;
        F32(p, 0x26E8) = F32(p, 0x26E8) + dy;
        F32(p, 0x26EC) = F32(p, 0x26EC) + dz;
        if (lean) {
            f32 k = lbl_80561CD8 * lbl_80561D98 * nvy * F32(p, 0xD4);
            BVec f;
            f.x = F32(p, 0x48) * k;
            f.y = F32(p, 0x4C) * k;
            f.z = F32(p, 0x50) * k;
            ((RigidBody*)p)->AddForceAt((const RVec3*)&f, (const RVec3*)(p + 0x60));
        }
    }
}

u32 DeityCar_vf58(u8* p) { return 0; }

void fn_80186DE4(u8* p) { fn_8018A8A8(p); }

}  // extern "C"

extern "C" {

// One physics step for every live car: run each body's pre-step (fn_80183E7C), feed contacts, accumulated forces
// and the body state into the solver records, step the solver, then per car apply drag, boost, lift and
// air-control forces, read the solved pose and velocities back, rebuild the four wheels' contact state, and work
// out the throttle/brake/steer limits (water, offroad, airborne, boost) before the drive model update.
void fn_80186DE8(u8* p) {
    CbSolMass* mass;
    CbSolContact* contact;
    CbSolCross* cross;
    CbSolCx* cx;
    s32 n;
    s32 i;
    fn_80190E6C();
    if (fn_801C5B60() == 0) {
        fn_80196A9C(0);
    }
    fn_801C56AC();
    fn_801B83A0()->sC8();
    n = 0;
    fn_801BEDA8(&mass, &contact, &cross, &cx, &n);
    for (i = 0; i < n; i++) {
        u8* car;
        car = lbl_804348E8[i];
        CbSolMass* m = &mass[i];
        fn_80183E7C(car);
        U8(car, 0x1E9) = 1;
        if (F32(car, 0x4C) < lbl_80561CAC) {
            Corner* c;
            for (c = *(Corner**)(car + 0xD0); c; c = c->next) {
                c->Hit((RigidBody*)car);
            }
        }
        BVec f;
        BVec t;
        ((RigidBody*)car)->Flush((RVec3*)&f, (RVec3*)&t);
        if (U8(car, 0x26AC)) {
            f.x = lbl_80561CAC;
            f.y = lbl_80561CAC;
            f.z = lbl_80561CAC;
            t.x = lbl_80561CAC;
            t.y = lbl_80561CAC;
            t.z = lbl_80561CAC;
        }
        m->force = CbSolVec4(m->force.x + f.x, m->force.y + f.y, m->force.z + f.z);
        m->torque = CbSolVec4(m->torque.x + t.x, m->torque.y + t.y, m->torque.z + t.z);
        if (PSVECDotProduct(car + 0x124, car + 0x124) > lbl_80561CE4) {
            PSVECNormalize(car + 0x124, car + 0x124);
            F32(car, 0x124) = F32(car, 0x124) * lbl_80561D34;
            F32(car, 0x128) = F32(car, 0x128) * lbl_80561D34;
            F32(car, 0x12C) = F32(car, 0x12C) * lbl_80561D34;
        }
        m->pos = CbSolVec4(F32(car, 0x60), F32(car, 0x64), F32(car, 0x68));
        m->vel = CbSolVec4(F32(car, 0x118), F32(car, 0x11C), F32(car, 0x120));
        m->avel = CbSolVec4(F32(car, 0x124), F32(car, 0x128), F32(car, 0x12C));
    }
    fn_801BEDD4();
    for (i = 0; i < n; i++) {
        CbSolMass* m;
        f32 steer;
        f32 lim;
        CbSolVec a;
        CbSolCross* x;
        int wh;
        f32 bk2;
        f32 air;
        u8* car;
        int off;
        x = &cross[i];
        car = lbl_804348E8[i];
        SurfType* st;
        m = &mass[i];
        CbSolContact* c = &contact[i];
        int grounded;
        f32 airK;
        CbSolVec r;
        f32 hold;
        u8 player;
        f32 slide;
        f32 oy;
        f32 wz;
        int surf;
        CbSolCx* e;
        f32 s6;
        int landing;
        CbSolVec grip4;
        int wet2;
        f32 cut;
        e = &cx[i];
        grip4 = x->vB0;
        air = lbl_80561CD0 - F32(car, 0x25D4);
        if (air > lbl_80561CD0) {
            air = lbl_80561CD0;
        }
        if (air > lbl_80561CAC) {
            air = air;
        } else {
            air = lbl_80561CAC;
        }
        f32 ds;
        f32 grip;
        airK = lbl_80561CD0 + lbl_80561D44 * air;
        surf = 0;
        if (e->surface[0] > surf) {
            surf = e->surface[0];
        }
        if (e->surface[1] > surf) {
            surf = e->surface[1];
        }
        if (e->surface[2] > surf) {
            surf = e->surface[2];
        }
        if (e->surface[3] > surf) {
            surf = e->surface[3];
        }
        f32 wet;
        f32 hb;
        st = fn_801D04A8(surf);
        f32 bk;
        wet = lbl_80561CD0;
        if (__fabsf(F32(car, 0x2650)) > lbl_80558DC4[0]) {
            f32 d = (lbl_80561D2C - F32(car, 0x2650) - lbl_80561CFC) / lbl_80561CFC;
            if (lbl_80561CD0 < d) {
                d = lbl_80561CD0;
            }
            if (d > lbl_80561CAC) {
                d = d;
            } else {
                d = lbl_80561CAC;
            }
            wet = lbl_80561D00 + d * lbl_80561DD0;
        }
        f32 slip;
        grip = st->grip * wet;
        f32 wy;
        CbSolVec v;
        slip = st->slip * wet;
        f32 gas;
        f32 steerK;
        landing = 0;
        if (lbl_8055E3C8 - F32(car, 0x26DC) < lbl_80561CDC && F32(car, 0x25D4) < lbl_80561DA0) {
            landing = 1;
        }
        if (S32(car, 0x2620) >= 3 && !landing) {
            f32 k;
            f32 drag;
            f32 speed = PSVECMag(car + 0x118);
            speed = speed > lbl_80561CD0 ? speed : lbl_80561CD0;
            drag = lbl_80561D98 * (-st->drag * F32(car, 0xD4)) / speed;
            f32 force[3];
            force[0] = F32(car, 0x118) * drag;
            force[1] = F32(car, 0x11C) * drag;
            force[2] = F32(car, 0x120) * drag;
            fn_801769CC();
            f32 top = lbl_80561D60 * fn_80181A48(car);
            if (F32(car, 0x258C) > lbl_80561DC0 && F32(car, 0x25D4) < top && F32(car, 0x25E4) < lbl_80561D00 &&
                (s32)fn_8018B06C(car) > 0) {
                f32 k = lbl_80561D98 * fn_80181A14(car) * F32(car, 0x25E0) * F32(car, 0xD4);
                AddScaled(force, 0x54 + car, k);
            }
            if (F32(car, 0x5EC) > lbl_80561CCC && (s32)fn_8018B06C(car) > 0 && F32(car, 0x25E4) < lbl_80561D00 &&
                F32(car, 0x25E0) > lbl_80561CDC) {
                f32 k = lbl_80561D98 * (lbl_8055E4E8[1] * (F32(car, 0x25E0) * F32(car, 0xD4)));
                AddScaled(force, car + 0x54, k);
            }
            if (F32(car, 0x258C) > lbl_80561CB0 || F32(car, 0x25E4) > lbl_80561CFC || F32(car, 0x5EC) > lbl_80561CCC) {
                f32 fwd = PSVECDotProduct(car + 0x3C, car + 0x118);
                f32 b = F32(car, 0x25E4);
                if (lbl_80561CD8 + F32(car, 0x258C) > b) {
                    b = lbl_80561CD8 + F32(car, 0x258C);
                }
                f32 k = lbl_80561E58 * (-fwd * F32(car, 0xD4)) * Clamp01(b);
                AddScaled(force, car + 0x3C, k);
            }
            speed = PSVECMag(car + 0x118);
            speed = speed > lbl_80561CD0 ? speed : lbl_80561CD0;
            k = lbl_80561D98 * (lbl_80561E5C * F32(car, 0xD4)) * F32(car, 0x25E4) / speed;
            force[0] = force[0] + F32(car, 0x118) * k;
            force[1] = force[1] + F32(car, 0x11C) * k;
            force[2] = force[2] + F32(car, 0x120) * k;
            if (U8(car, 0x259C) && F32(car, 0x25D4) < top) {
                f32 t = (F32(car, 0x25D4) - lbl_80561E64) / lbl_80561E68;
                f32 boost = (heatTune.v[5] * F32(car, 0x2D8) * F32(car, 0x39C)) * lbl_80561D98;
                f32 lo = lbl_80561E60 * boost;
                k = (lo + Clamp01(t) * (lbl_80561D38 * boost - lo)) * F32(car, 0x25E0) * F32(car, 0xD4);
                AddScaled(force, car + 0x54, k);
            }
            ((RigidBody*)car)->AddForceAt((const RVec3*)force, (const RVec3*)(car + 0x60));
        }
        v = x->pts[6];
        f32 brake;
        s6 = v.w;
        if (F32(car, 0x2618) > lbl_80561CFC) {
            s6 = s6 * heatTune.v[4];
        }
        v.x = s6;
        x->pts[6] = v;
        x->vA0 = CbSolVec4(grip4.x * grip, grip4.y * grip, grip4.z * (airK * slip), grip4.w * (slip * airK));
        wh = S32(car, 0x26B0);
        if (wh >= 0) {
            f32 a;
            f32 b;
            a = F32(car, wh * 0x74 + 0x23B4);
            f32 s1[4];
            f32 s0[4];
            s1[0] = lbl_80561CAC;
            s1[1] = lbl_80561CAC;
            s1[2] = lbl_80561CAC;
            s1[3] = lbl_80561CAC;
            s0[0] = lbl_80561CAC;
            s0[1] = lbl_80561CAC;
            s0[2] = lbl_80561CAC;
            s0[3] = lbl_80561CAC;
            s1[wh] = (a - lbl_80561E6C) * (a - lbl_80561E70) * (lbl_80561E74 * a) * lbl_80561E78;
            b = fmod(lbl_80561E7C + a, lbl_80561E80);
            *(CbSolVec*)&x->v130 = *(CbSolVec*)s1;
            s0[wh] = -((b - lbl_80561E6C) * (b - lbl_80561E70) * (lbl_80561E74 * b)) * lbl_80561E78;
            *(CbSolVec*)&x->v140 = *(CbSolVec*)s0;
        }
        player = 0;
        if (U32(car, 0x10) == 'PCAR' && ((*(CbFlagsV**)(car + 0x270C))->Flags() & 0x20)) {
            player = 1;
        }
        F32(car, 0x3C) = m->rot[0].x;
        F32(car, 0x40) = m->rot[0].y;
        F32(car, 0x44) = m->rot[0].z;
        F32(car, 0x48) = m->rot[1].x;
        F32(car, 0x4C) = m->rot[1].y;
        F32(car, 0x50) = m->rot[1].z;
        F32(car, 0x54) = m->rot[2].x;
        F32(car, 0x58) = m->rot[2].y;
        F32(car, 0x5C) = m->rot[2].z;
        F32(car, 0x60) = m->pos.x;
        F32(car, 0x64) = m->pos.y;
        F32(car, 0x68) = m->pos.z;
        ((CollVolume*)car)->Update();
        *(s16*)(car + 0x6C) = e->id;
        f32 vz;
        f32 wx;
        f32 vx = m->vel.x;
        f32 vy = m->vel.y;
        vz = m->vel.z;
        wx = m->avel.x;
        wz = m->avel.z;
        f32 throttle;
        f32 ox;
        f32 oz;
        int boosting;
        f32 vmax;
        wy = m->avel.y;
        f32 out;
        ox = F32(car, 0x16C);
        oy = F32(car, 0x170);
        oz = F32(car, 0x174);
        F32(car, 0x16C) = lbl_80561E88 * vx + F32(car, 0x118) * lbl_80561E8C;
        F32(car, 0x170) = vy * lbl_80561E88 + F32(car, 0x11C) * lbl_80561E8C;
        F32(car, 0x174) = vz * lbl_80561E88 + F32(car, 0x120) * lbl_80561E8C;
        F32(car, 0x178) = wx * lbl_80561E88 + F32(car, 0x124) * lbl_80561E8C;
        F32(car, 0x17C) = wy * lbl_80561E88 + F32(car, 0x128) * lbl_80561E8C;
        F32(car, 0x180) = wz * lbl_80561E88 + F32(car, 0x12C) * lbl_80561E8C;
        F32(car, 0x118) = vx;
        F32(car, 0x11C) = vy;
        F32(car, 0x120) = vz;
        F32(car, 0x124) = wx;
        F32(car, 0x128) = wy;
        F32(car, 0x12C) = wz;
        F32(car, 0x26B4) = F32(car, 0x16C) - ox;
        F32(car, 0x26B8) = F32(car, 0x170) - oy;
        F32(car, 0x26BC) = F32(car, 0x174) - oz;
        f32 w0[4];
        f32 w1[4];
        f32 w2[4];
        f32 w3[4];
        f32 w4[4];
        f32 w5[4];
        f32 w6[4];
        f32 half[4];
        *(CbSolVec*)w0 = c->v30;
        *(CbSolVec*)w1 = c->v10;
        *(CbSolVec*)w2 = c->v0;
        *(CbSolVec*)w3 = x->v130;
        *(CbSolVec*)w4 = x->v120;
        *(CbSolVec*)w5 = x->v140;
        *(CbSolVec*)w6 = x->v70;
        CbWheel* w;
        CbSolVec rpm = c->pts[4];
        F32(car, 0x25D0) = rpm.x / lbl_80561CC0;
        F32(car, 0x25D4) = PSVECMag(0x118 + car);
        F32(car, 0x25D8) = PSVECDotProduct(car + 0x118, car + 0x54);
        F32(car, 0x25CC) = rpm.y;
        steer = F32(car, 0x25F0);
        f32* water;
        w = (CbWheel*)(car + 0x23AC);
        grounded = 0;
        for (int j = 0; j < 4; j++, w = (CbWheel*)((u8*)w + 0x74)) {
            f32 load = w0[j];
            w->load = load;
            f32 spin = w1[j];
            w->loadN = load / lbl_80561CC0;
            w->spin = spin;
            w1[j] = fmod(lbl_80561E6C + spin, lbl_80561E80);
            w->prevSlip = w->slip;
            w->slip = w2[j];
            w->steer = asin(steer * w4[j] + w3[j]);
            w->v18 = w5[j];
            w->contact[0] = c->pts[j].x;
            w->contact[1] = c->pts[j].y;
            f32 nrm[3];
            w->contact[2] = c->pts[j].z;
            nrm[0] = c->pts[9 + j].x;
            nrm[1] = c->pts[9 + j].y;
            nrm[2] = c->pts[9 + j].z;
            w->local[0] = x->pts[j].x;
            w->local[2] = x->pts[j].z;
            w->local[1] = x->pts[j].y - w6[j];
            u32 fl;
            f32 up = PSVECDotProduct(nrm, car + 0x54);
            w->nRight = PSVECDotProduct(nrm, car + 0x3C);
            w->nFwd = up;
            w->grounded = nrm[1] > lbl_80561CCC;
            f32 comp = c->pts[5 + j].w;
            if (comp > lbl_80561E90) {
                c->pts[5 + j] = CbSolVec4(lbl_80561CAC, lbl_80561CAC, lbl_80561CAC);
                *(CbSolVec*)half = c->v30;
                half[j] = half[j] * lbl_80561CDC;
                c->v30 = *(CbSolVec*)half;
            }
            fl = w->flags & 0xE;
            if (nrm[1] > lbl_80561CCC) {
                f32 d = comp - lbl_80561E94;
                grounded++;
                if (d < lbl_80561CAC) {
                    d = lbl_80561CAC;
                }
                w->comp = d;
                int sid = e->surface[j];
                w->groundH = fn_802B03C0(w->ground);
                w->sound = fn_801D04A8(sid)->sound;
            } else {
                w->comp = lbl_80561CAC;
            }
            w->flags = fl;
        }
        S32(car, 0x2620) = grounded;
        c->v10 = *(CbSolVec*)w1;
        ds = F32(car, 0x25DC) - F32(car, 0x25F0);
        lim = lbl_80561E9C;
        if (ds * F32(car, 0x25F0) > lbl_80561CAC) {
            lim = lbl_80561E98;
        }
        if (ds > lim) {
            ds = lim;
        }
        if (ds < -lim) {
            ds = -lim;
        }
        F32(car, 0x25F0) = F32(car, 0x25F0) + ds;
        water = (f32*)(lbl_8055E918 + i * 0x9C);
        f32* ground = (f32*)(lbl_8055E908 + i * 0x34);
        throttle = Clamp01(F32(car, 0x25E0) + water[1]);
        if (!(U32(fn_801769CC(), 4) & 0x4000) && (f32)__fabs(F32(car, 0x2690)) > lbl_80558DC4[0]) {
            throttle = lbl_80561CAC;
            F32(car, 0x25E4) = lbl_80561CD0;
        }
        if (F32(car, 0x2588) > lbl_80561DC0) {
            U32(car, 0x268C) = 10;
            if (F32(car, 0x25D4) > lbl_80561D60 * heatTune.v[11]) {
                f32 b = heatTune.v[10];
                if (F32(car, 0x25E4) > b) {
                    b = F32(car, 0x25E4);
                }
                F32(car, 0x25E4) = b;
                throttle = heatTune.v[9];
                if (F32(car, 0x25E0) < throttle) {
                    throttle = F32(car, 0x25E0);
                }
            }
        } else if ((U32(fn_801769CC(), 4) & 0x80000) && F32(car, 0x25A0) > lbl_80561D04) {
            U32(car, 0x268C) = 6;
        } else {
            U32(car, 0x268C) = 0;
        }
        steerK = F32(car, 0x25F0);
        if (player) {
            f32 sp;
            f32 cl;
            f32 t;
            sp = F32(car, 0x25D4);
            if (sp > lbl_80561EA0) {
                cl = sp;
                cl = cl <= lbl_80561EA4 ? cl : lbl_80561EA4;
                steerK = steerK * (lbl_80561CD0 + F32(car, 0x450) * (lbl_80561EA8 + lbl_80561EAC * cl - lbl_80561CD0));
            }
            if (sp > lbl_80561CAC) {
                t = Clamp01((sp / lbl_80561DEC) * lbl_80561D60);
                steerK = steerK * (lbl_80561CD0 - lbl_80561CDC * t) * (lbl_80561CD0 - lbl_80561D38 * t);
            }
        }
        slide = (f32)__fabs(F32(car, 0x25CC)) - lbl_80561EB0 * (f32)__fabs(F32(car, 0x25D8)) - lbl_80561D44;
        if (slide < lbl_80561CAC) {
            slide = lbl_80561CAC;
        }
        cut = lbl_80561CD0;
        if (!player) {
            if (S32(car, 0x267C)) {
                f32 s = lbl_80561D2C * steerK - lbl_80561D38 * F32(car, 0x128);
                if (s > cut) {
                    s = cut;
                }
                steerK = lbl_80561DE4;
                if (s > lbl_80561DE4) {
                    steerK = s;
                }
            }
        } else {
            cut = lbl_80561CAC;
        }
        gas = throttle;
        if (slide > lbl_80561CAC && S32(car, 0x2620) > 2) {
            gas = lbl_80561CD0 - cut;
            if (gas > throttle) {
                gas = throttle;
            }
        }
        brake = F32(car, 0x25E8);
        f32 rpmT = rpm.x / lbl_80561CC0;
        throttle = lbl_80561EB4 * (lbl_80561EB8 - rpmT);
        if (throttle < gas) {
            throttle = gas;
        }
        if (rpmT > ground[12]) {
            throttle = lbl_80561CAC;
        }
        hold = F32(car, 0x25EC);
        bk = Clamp01((rpmT - lbl_80561EBC - lbl_80561EC0 * throttle) / lbl_80561EC0);
        if (bk < brake) {
            brake = bk;
        }
        if (F32(car, 0x25D4) > lbl_80561DA4 && lbl_80561DA4 + lbl_80561EC4 * F32(car, 0x25D4) < F32(car, 0x25CC)) {
            throttle = lbl_80561CAC;
        }
        if ((f32)__fabs(F32(car, 0x2650)) > lbl_80558DC4[0]) {
            throttle = lbl_80561CAC;
        }
        if ((f32)!((f32)__fabs(water[6]) <= lbl_80558DC4[0]) != lbl_80561CAC) {  // a bool converted to float
            brake = lbl_80561CAC;
        }
        boosting = 1;
        F32(car, 0x25FC) = throttle;
        vmax = F32(car, 0x5A8);
        if (F32(car, 0x258C) <= lbl_80561DC0 && !U8(car, 0x259C)) {
            boosting = 0;
        }
        if (boosting && F32(car, 0x25E0) > lbl_80561CDC) {
            vmax = lbl_80561D60 * fn_80181A48(car) - lbl_80561DA4;
        }
        vmax = vmax + lbl_80561D60 * (lbl_8055E4E8[0] * F32(car, 0x5EC));
        off = fn_802B02C8(car + 0x60);
        wet2 = 0;
        if (fn_802BF4F0() && F32(car, 0x2558) < fn_802BF590()) {
            wet2 = 1;
        }
        if (wet2) {
            vmax = vmax * lbl_8055A3A8;
        } else if (off) {
            vmax = vmax * lbl_8055A3A4;
        }
        out = throttle;
        if (F32(car, 0x25D4) > vmax && brake > lbl_80561CDC && U32(car, 0x10) == 'PCAR') {
            out = lbl_80561CD0 - lbl_80561EC8 * (F32(car, 0x25D4) - vmax);
            out = out > lbl_80561CAC ? out : lbl_80561CAC;
            out = out < throttle ? out : throttle;
            boosting = 1;
            if (F32(car, 0x258C) <= lbl_80561DC0 && !U8(car, 0x259C)) {
                boosting = 0;
            }
            if (!boosting) {
                out = out + Clamp01(lbl_80561CD0 + F32(car, 0x258C)) * (throttle - out);
            }
        }
        F32(car, 0x25F4) = out;
        F32(car, 0x25F8) = brake;
        a = x->pts[6];
        a.w = F32(car, 0x34C) - F32(car, 0x350);
        if ((f32)__fabs(F32(car, 0x2650)) > lbl_80558DC4[0]) {
            a.w = a.w * lbl_80561CAC;
        }
        x->pts[6] = a;
        r = c->pts[4];
        if (r.x >= lbl_80561ECC) {
            r.x = r.x;
        } else {
            r.x = lbl_80561ECC;
        }
        r.w = ground[0];
        c->pts[4] = r;
        if (rpmT >= lbl_80561ED0) {
            out = lbl_80561CAC;
        }
        c->v190 = CbSolVec4(steerK, out, brake, water[0]);
        bk2 = F32(car, 0x25E4);
        hb = lbl_80561D50 * hold;
        if (hb > bk2) {
            hb = hb;
        } else {
            hb = bk2;
        }
        hb = hb > lbl_80561CAC ? hb : lbl_80561CAC;
        bk2 = bk2 > lbl_80561CAC ? bk2 : lbl_80561CAC;
        c->v1A0 = CbSolVec4(bk2, bk2, hb, hb);
        fn_8018CB3C(car);
        ((CbStepV*)PTR(car, 0x270))->Step();
        ((CbTickV*)car)->Tick();
    }
    fn_801BF1B4();
}


void vf20__10CollVolumeFv(u8* p);
extern const f32 lbl_80561CAC;  // 0.0
extern const f32 lbl_80561CDC;  // 0.5
extern const f32 lbl_80561D90;  // -5.0
extern const f32 lbl_80561D9C;  // 1/60 s
extern const f32 lbl_80561DE4;  // -1.0
extern const f32 lbl_80561E5C;  // -1.5
extern const f32 lbl_80561EE0;  // 0.5/60
extern const f32 lbl_80561EE4;  // -100.0
extern const f32 lbl_80561EE8;  // 0.95
extern const f32 lbl_80561EEC;  // 1 - 0.95
extern const f32 lbl_80561EF0;  // 15 degrees
extern const f32 lbl_80561EF4;  // -2.5 degrees

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

u32 DeityCar_vf7C(u8* p) { return 0; }


// The body pose in the render snapshot: rotation rows, then the position (copied block by block).
struct SnapRot {
    f32 m[9];
};
struct SnapPos {
    f32 v[3];
};
struct SnapPose {
    SnapRot rot;
    SnapPos pos;
};

static inline int NearZero(f32 x) { return (f32)__fabs(x) <= lbl_80558DC4[0]; }

extern f32 lbl_8055E460;  // render interpolation factor between the last two physics steps
extern u8 lbl_8055E4F5;
extern s32 lbl_8055E3C4;  // frame counter
void fn_8018DA5C(u8* p, u8* m);
u32 fn_8018B06C(u8* p);
void fn_8018A9F0(u8* p, u8* m);
double sqrt(double);
static inline f32 Len2(f32 x, f32 z) { return sqrt(x * x + z * z); }

// Fills this car's render snapshot (0xFA8 bytes at buf + 0x8): the body pose interpolated between the last
// two physics steps, state flags and gauges, and the four wheels (interpolated hub positions, suspension and
// spin). If a wheel sinks below its contact height, the whole body is lifted by the deepest one.
void fn_80188E2C(u8* p, u8* buf) {
    SnapPose tmp;
    u8* m = buf + S32(p, 0x8);
    f32 t = lbl_8055E460;
    memset(m, 0, 0xFA8);
    SnapPose* pose = (SnapPose*)(buf + S32(p, 0x8));
    if ((f32)__fabs(t - 1.0f) <= lbl_80558DC4[0]) {
        *pose = *(SnapPose*)(p + 0x70);
    } else {
        f32* o = (f32*)&tmp;
        const f32* prev = (const f32*)(p + 0xA0);
        const f32* cur = (const f32*)(p + 0x70);
        for (int i = 0; i < 12; i++) {
            o[i] = prev[i] + t * (cur[i] - prev[i]);
        }
        *pose = tmp;
    }
    *(s16*)((u8*)pose + 0x30) = *(s16*)(p + 0x6C);
    u16 flags = 0;
    if (((CarHost*)PTR(p, 0x270))->IsImpaired()) {
        flags |= 0x40;
    }
    if (!NearZero(F32(p, 0x2690))) {
        flags |= 0x20;
    }
    F32(m, 0x34) = F32(p, 0x25D0);
    F32(m, 0x4C) = Len2(F32(p, 0x118), F32(p, 0x120));
    F32(m, 0x3C) = F32(p, 0x25F0);
    F32(m, 0x38) = F32(p, 0x25F4);
    F32(m, 0x40) = F32(p, 0x25E4);
    F32(m, 0x44) = F32(p, 0x25F8);
    U32(m, 0x48) = fn_8018B06C(p);
    F32(m, 0x50) = 100.0f;
    if (lbl_8055E4F5 && S32(p, 0x24) == 0) {
        F32(m, 0x22C) = (lbl_8055E3C4 & 4) ? 0.0f : 1.0f;
    } else {
        F32(m, 0x22C) = 1.0f;
    }
    U16(m, 0x6E) = flags;
    F32(m, 0x70) = F32(p, 0x118);
    F32(m, 0x74) = F32(p, 0x11C);
    F32(m, 0x78) = F32(p, 0x120);
    F32(m, 0x64) = F32(p, 0x25D4);
    F32(m, 0x2C4) = F32(p, 0x2588);
    F32(m, 0x28C) = U8(p, 0x26E0) ? FMax(F32(p, 0x2690), 0.01f) : F32(p, 0x2690);
    F32(m, 0x290) = F32(p, 0x2694);
    F32(m, 0x294) = F32(p, 0x2698);
    S32(m, 0x298) = S32(p, 0x268C);
    f32 v = F32(p, 0x25BC);
    if (Positive(v)) {
        F32(m, 0x2D4) = v;
    } else {
        F32(m, 0x2D4) = 0.0f;
    }
    F32(m, 0x2A8) = F32(p, 0x25A0);
    S32(m, 0x29C) = S32(p, 0x5A0);
    if (F32(p, 0x25E4) > 0.1f && !(flags & 0x20)) {
        U32(m, 0x29C) |= 0x20000000;
    } else {
        U32(m, 0x29C) |= 0x10000000;
    }
    F32(m, 0x238) = F32(p, 0x2660);
    F32(m, 0x23C) = F32(p, 0x2664);
    F32(m, 0x240) = F32(p, 0x2668);
    f32 sink = 0.0f;
    u8* wheels = m + 0x7C;
    u8* w = p + 0x23AC;
    u8* o = wheels;
    for (int i = 0; i < 4; i++) {
        HVec a = *(HVec*)(w + 0x30);
        a.x = a.x - F32(p, 0x30);
        a.y = a.y - F32(p, 0x34);
        a.z = a.z - F32(p, 0x38);
        HVec b = a;
        a.y = a.y + F32(w, 0xC);
        b.y = b.y + F32(w, 0x1C);
        HVec c;
        f32 xv = b.x;
        c.x = xv + lbl_8055E460 * (a.x - xv);
        c.y = b.y + lbl_8055E460 * (a.y - b.y);
        f32 zv = b.z;
        c.z = zv + lbl_8055E460 * (a.z - zv);
        F32(o, 0x0) = c.x;
        F32(o, 0x4) = c.y;
        F32(o, 0x8) = c.z;
        sink = FMax(sink, c.y - F32(w, 0x40));
        F32(o, 0xC) = F32(w, 0x4C);
        F32(o, 0x10) = F32(w, 0x50);
        F32(o, 0x14) = F32(w, 0x54);
        F32(o, 0x18) = F32(w, 0x58);
        F32(o, 0x1C) = F32(w, 0x5C);
        F32(o, 0x20) = F32(w, 0x60);
        F32(o, 0x24) = F32(w, 0xC);
        F32(o, 0x28) = F32(w, 0x14);
        F32(o, 0x2C) = F32(w, 0x18);
        F32(o, 0x30) = F32(w, 0x0);
        F32(o, 0x34) = F32(w, 0x8) + 0.016683351f * (F32(w, 0x0) * (lbl_8055E460 - 1.0f));
        F32(o, 0x38) = F32(w, 0x68);
        F32(o, 0x3C) = F32(w, 0x44);
        F32(o, 0x40) = F32(w, 0x48);
        F32(o, 0x44) = F32(w, 0x10);
        F32(o, 0x48) = 0.0f;
        F32(o, 0x4C) = 0.0f;
        F32(o, 0x50) = 0.0f;
        F32(o, 0x54) = 0.0f;
        F32(o, 0x58) = 0.0f;
        F32(o, 0x5C) = 0.0f;
        S32(o, 0x60) = S32(w, 0x70);
        U8(o, 0x64) = U8(w, 0x20);
        S32(o, 0x68) = S32(w, 0x64);
        w += 0x74;
        o += 0x6C;
    }
    F32(m, 0x2C8) = F32(((CarHost*)PTR(p, 0x270))->Info(), 0x48);
    F32(m, 0x2CC) = F32(p, 0x26CC);
    F32(m, 0x2D0) = F32(p, 0x5E4);
    S32(m, 0x2D8) = S32(p, 0x26D4);
    fn_8018DA5C(p, m);
    if (sink > 0.01f) {
        F32(m, 0x24) = F32(m, 0x24) + F32(m, 0xC) * sink;
        F32(m, 0x28) = F32(m, 0x28) + F32(m, 0x10) * sink;
        F32(m, 0x2C) = F32(m, 0x2C) + F32(m, 0x14) * sink;
        for (int i = 0; i < 4; i++) {
            F32(wheels + i * 0x6C, 0x4) = FMax(F32(wheels + i * 0x6C, 0x4) - sink, -0.6096f);
        }
        u8* e = m + 0xA7C;
        for (int i = 0; i < S32(m, 0xA78); i++) {
            F32(e, 0x24) = F32(e, 0x24) + F32(m, 0xC) * sink;
            F32(e, 0x28) = F32(e, 0x28) + F32(m, 0x10) * sink;
            F32(e, 0x2C) = F32(e, 0x2C) + F32(m, 0x14) * sink;
            e += 0x34;
        }
    }
    if (S32(p, 0x26C0)) {
        fn_8018A9F0(p, m);
    } else {
        S32(m, 0xFA4) = 0;
    }
}

// A rotation of plain row vectors, copied row by row.
struct HRot {
    HRot(const HRot& o) : rows0(o.rows0), rows1(o.rows1), rows2(o.rows2) {}
    HVec rows0, rows1, rows2;
};

// A frame: rotation rows, then the position.
struct HFrame {
    HFrame(const HRot& r) : rot(r) {}
    HRot rot;
    HVec pos;
};

static inline f32 HitDot(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }
static inline f32 HitMin(f32 a, f32 b) { return a < b ? a : b; }

void fn_8018E3EC(u8* p, const HVec* imp, const HVec* at, int a, s32 id, int kind, u8* other, int c);
void PSVECNormalize(const void* a, void* b);
f32 PSVECMag(const void* v);
f32 PSVECDotProduct(const void* a, const void* b);

// Collision response: scales the impulse `imp` at the world point `at` by the mass ratio of the two cars
// for a car-on-car hit (and drops it when this car is the heavier one hit from the front), by the body
// stiffness otherwise, and passes it on in centre-of-mass space when it is large enough.
void fn_8018956C(u8* p, const HVec* imp, const HVec* at, int kind, u8* other, int c) {
    fn_80189C64(p);
    f32 mag = PSVECMag(imp);
    int carHit = other && kind == 0x6D;
    f32 scale = 1.0f;
    s32 otherId = -1;
    if (kind == 0x6D && other) {
        u32 t = U32(other, 0x10);
        if (t == 'CAR ' || t == 'PCAR' || t == 'ACAR' || t == 'NCAR' || t == 'TRLR') {
            otherId = S32(other, 0x1EC);
            HVec dir = *imp;
            if (mag > 0.1f) {
                PSVECNormalize(&dir, &dir);
            }
            f32 a = __fabs(PSVECDotProduct(&dir, p + 0x118));
            f32 b = __fabs(PSVECDotProduct(&dir, other + 0x118));
            f32 d = b - a;
            if (d > 6.6666665f && b > 1.5f * a) {
                scale = HitMin(b / FMax(a, 0.001f), 5.0f);
            } else if (d < -6.6666665f && a > 1.5f * b) {
                scale = FMax(b / FMax(a, 0.001f), 0.2f);
                HVec local;
                local.x = at->x - F32(p, 0x94);
                local.y = at->y - F32(p, 0x98);
                local.z = at->z - F32(p, 0x9C);
                f32 x = HitDot(local.x, F32(p, 0x70), local.y, F32(p, 0x74), local.z, F32(p, 0x78));
                f32 y = HitDot(local.x, F32(p, 0x7C), local.y, F32(p, 0x80), local.z, F32(p, 0x84));
                f32 z = HitDot(local.x, F32(p, 0x88), local.y, F32(p, 0x8C), local.z, F32(p, 0x90));
                local.x = x;
                local.y = y;
                local.z = z;
                if (local.z < -0.508f && PSVECDotProduct(imp, p + 0x88) > 0.0f) {
                    scale = 0.0f;
                }
            }
        }
    } else if (kind == 0x6E && !other) {
    } else if (kind == 0x70 && other) {
        u32 t = U32(other, 0x10);
        if (t == 'CAR ' || t == 'PCAR' || t == 'ACAR' || t == 'NCAR') {
            otherId = S32(other, 0x1EC);
        }
    }
    HVec hit = *imp;
    f32 k = F32(p, 0x5F4);
    if (carHit) {
        k *= scale;
    } else if (kind != 0x70) {
        k *= 0.4f * F32(p, 0x318);
    } else {
        k *= 7.0f;
    }
    if (!(U32(fn_801769CC(), 0x4) & 0x4000)) {
        k *= 10.0f;
    }
    hit.x *= k;
    hit.y *= k;
    hit.z *= k;
    if (mag * k > 741.6667f) {
        const HRot* rot = (const HRot*)(p + 0x3C);
        const HVec* org = (const HVec*)(p + 0x60);
        const HVec* com = (const HVec*)(p + 0x30);
        HFrame fr(*rot);
        if (&fr.pos == com) {
            f32 x = HitDot(com->x, rot->rows0.x, com->y, rot->rows1.x, com->z, rot->rows2.x);
            f32 y = HitDot(com->x, rot->rows0.y, com->y, rot->rows1.y, com->z, rot->rows2.y);
            f32 z = HitDot(com->x, rot->rows0.z, com->y, rot->rows1.z, com->z, rot->rows2.z);
            fr.pos.x = x;
            fr.pos.y = y;
            fr.pos.z = z;
        } else {
            fr.pos.x = HitDot(com->x, rot->rows0.x, com->y, rot->rows1.x, com->z, rot->rows2.x);
            fr.pos.y = HitDot(com->x, rot->rows0.y, com->y, rot->rows1.y, com->z, rot->rows2.y);
            fr.pos.z = HitDot(com->x, rot->rows0.z, com->y, rot->rows1.z, com->z, rot->rows2.z);
        }
        fr.pos.x = fr.pos.x + org->x;
        f32 yv = fr.pos.y;
        f32 y_ = org->y;
        fr.pos.y = y_ + yv;
        f32 zv = org->z;
        fr.pos.z = zv + fr.pos.z;
        f32 dx = at->x - fr.pos.x;
        f32 y2 = at->y;
        f32 dy = y2 - fr.pos.y;
        f32 dz = at->z - fr.pos.z;
        HVec lp;
        lp.x = HitDot(dx, fr.rot.rows0.x, dy, fr.rot.rows0.y, dz, fr.rot.rows0.z);
        lp.y = HitDot(dx, fr.rot.rows1.x, dy, fr.rot.rows1.y, dz, fr.rot.rows1.z);
        lp.z = HitDot(dx, fr.rot.rows2.x, dy, fr.rot.rows2.y, dz, fr.rot.rows2.z);
        HVec li;
        if (&li == &hit) {
            f32 x = HitDot(hit.x, fr.rot.rows0.x, hit.y, fr.rot.rows0.y, hit.z, fr.rot.rows0.z);
            f32 y = HitDot(hit.x, fr.rot.rows1.x, hit.y, fr.rot.rows1.y, hit.z, fr.rot.rows1.z);
            f32 z = HitDot(hit.x, fr.rot.rows2.x, hit.y, fr.rot.rows2.y, hit.z, fr.rot.rows2.z);
            li.x = x;
            li.y = y;
            li.z = z;
        } else {
            li.x = HitDot(hit.x, fr.rot.rows0.x, hit.y, fr.rot.rows0.y, hit.z, fr.rot.rows0.z);
            li.y = HitDot(hit.x, fr.rot.rows1.x, hit.y, fr.rot.rows1.y, hit.z, fr.rot.rows1.z);
            li.z = HitDot(hit.x, fr.rot.rows2.x, hit.y, fr.rot.rows2.y, hit.z, fr.rot.rows2.z);
        }
        fn_8018E3EC(p, &li, &lp, 0, otherId, kind, other, c);
    }
}

int fn_80189BDC(u8* p) {
    if (U32(p, 0x10) != 'TRLR') {
        u8* o = ((VG*)PTR(p, 0x270))->v10();
        if (o && (S32(o, 0x24) == 1 || S32(o, 0x24) == 3)) {
            return 0;
        }
    }
    return fn_801B83A0()->sCC(S32(p, 0x1EC));
}

extern const f32 lbl_80561CAC;  // 0.0

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

struct CbShapeList {
    u8 pad[0x14];
    CbShape* shapes[4];  // offset 0x14
};

static inline CbShape* ShapeAt(u8* p, int i) {
    if (i >= 0 && i < S32(p, 0x24)) {
        return ((CbShapeList*)p)->shapes[i];
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
void fn_801B30C0(s32 car, s32 reset, void* start);
void fn_8026A394(u8* car);
f32 PSVECDotProduct(const void* a, const void* b);
f32 PSVECMag(const void* v);
void fn_8018A750(u8* p);

// Puts the car down at `s` (a frame for the body origin, the velocity at 0x30, the start point at
// 0x3C and the surface at 0x40) at rest: clears the impulses, wheel loads and gear state.
void fn_80189CB4(u8* p, const u8* s) {
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
    f32 vz;
    f32 vy;
    f32 vx;
    vx = F32(s, 0x30);
    vy = F32(s, 0x34);
    vz = F32(s, 0x38);
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
    f32 speed = F32(p, 0x25CC);
    fn_801BE400(S32(p, 0x1EC), &frame, speed);
    fn_801B30C0(S32(p, 0x1EC), 0, PTR(s, 0x3C));
    for (int i = 0; i < S32(p, 0x24); i++) {
        ShapeAt(p, i)->Reset();
    }
    for (int w = 0; w < 4; w++) {
        F32(p, 0x23AC + w * 0x74) = 0.0f;
        F32(p, 0x2414 + w * 0x74) = 0.0f;
    }
    fn_8018A750(p);
    S32(p, 0x2638) = 0;
    F32(p, 0x2628) = 0.0f;
    F32(p, 0x262C) = 0.0f;
    U8(p, 0x2640) = 0;
    S32(p, 0x26D4)++;
    fn_8026A394(p);
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

void DeityCar_vf78(u8* p) {}

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
extern const f32 lbl_80561CAC;
extern const f32 lbl_80561CCC, lbl_80561CDC, lbl_80561CE4, lbl_80561CEC, lbl_80561CF8,
    lbl_80561D00, lbl_80561D0C, lbl_80561D28, lbl_80561D34, lbl_80561D44, lbl_80561D68, lbl_80561DE4, lbl_80561EBC,
    lbl_80561F0C, lbl_80561F10, lbl_80561F14, lbl_80561F18;

// Registers the body's tunables (mass, moments, power, geometry, springs, damping, tires) with a tuning
// menu. Matched by the upstream project (orangedude27/excite-truck).
// MATCHING-AID: register pinning, not original source. Revisit.
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
// MATCHING-AID: machine-extracted inline helper (loopext); it raised the score but nothing proves the original had it.
static inline void Loop1_fn_8018A750(u8* p, SprVert* verts, SprEdge* edges, SprPin* pins) {
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
}

void fn_8018A750(u8* p) {
    if (PTR(p, 0x26C4)) {
        fn_801C2170(PTR(p, 0x26C4), PTR(p, 0x26C0), p + 0x70);
    }
    if (PTR(p, 0x26C8)) {
        SprVert verts[8];
        SprEdge edges[4];
        SprPin pins[4];
        int n = S32(PTR(p, 0x26C0), 0xC8);
        Loop1_fn_8018A750(p, verts, edges, pins);
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
        s32 n;
        BVec* pos = 0;
        BVec* old = 0;
        n = 0;
        fn_801C2BDC(PTR(p, 0x26C4), &n, &pos, &old);
        for (int i = 0; i < S32(PTR(p, 0x26C0), 0xA4); i++) {
            u8* d = PTR(p, 0x26C0);
            ToBody(p, (BVec*)(m + 0xE24) + (S32(d, 0xA0) + i), &pos[((s32*)(d + 0x20))[i]]);
        }
    }
    if (PTR(p, 0x26C8)) {
        s32 n;
        BVec* pos = 0;
        BVec* old = 0;
        n = 0;
        fn_801C2BDC(PTR(p, 0x26C8), &n, &pos, &old);
        for (s32 i = 0; i < S32(PTR(p, 0x26C0), 0xC8); i++) {
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
    o->y = F32(p, 0x5AC) + (h.y + F32(m, 0xEC));
    o->z = lbl_80561CAC;
    o = (BVec*)(m + 0xE24) + S32(PTR(p, 0x26C0), 0xD4);
    o->x = lbl_80561CAC;
    o->y = F32(p, 0x5AC) + (F32(m, 0x158) + h.z);
    o->z = lbl_80561CAC;
    o = (BVec*)(m + 0xE24) + S32(PTR(p, 0x26C0), 0xD8);
    o->x = lbl_80561CAC;
    o->y = F32(p, 0x5AC) + (h.w + F32(m, 0x1C4));
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

// A car's network/replay state packet (quantized).
struct CbPacket {
    u8 pad0[0x8];
    u8 x[4];       // 0x08 position x (unaligned f32)
    u8 pad0C[0x4];
    u8 z[4];       // 0x10 position z
    u8 pad14[0xC];
    u16 rpm;       // 0x20
    u8 steer;      // 0x22
    u8 throttle;   // 0x23
    u8 brake;      // 0x24
    s8 gear;       // 0x25
    u8 wheel[4][5];  // 0x26 per wheel: spin, slip, suspension, skid, flags
    u16 flags;     // 0x3A
    u8 boost;      // 0x3C
    u8 pad3D[3];
    s32 w40;       // 0x40
};

#define PK_MIN(a, b) ((a) < (b) ? (a) : (b))
#define PK_MAX(a, b) ((a) > (b) ? (a) : (b))
// A quantized value v (0..q) back to lo..lo+span.
static inline f32 PkNorm(u32 v, f32 lo, f32 range) {
    f32 x = PK_MIN(1.0f, ((f32)v - lo) / range);
    return PK_MAX(x, 0.0f);
}
static inline f32 PkUnpack(u32 v, f32 q, f32 lo, f32 span) { return lo + PkNorm(v, 0.0f, q) * span; }

static inline void PkCopy(void* dst, const void* src, int n) {
    u8* d = (u8*)dst;
    const u8* s = (const u8*)src;
    while (n-- > 0) {
        *d++ = *s++;
    }
}

int vf30__9RigidBodyFfP7RPacketP7RPacketPlPl(u8* p, f32 t, CbPacket* a, CbPacket* b, s32* na, s32* nb);
void fn_8018ADB4(u8* p, f32 v);
void fn_8018ADBC(u8* p, f32 v);
void fn_801E9DD0(s16* surface, f32 x, f32 z);
extern u8* lbl_8055E918;

// CarBody::vf30: applies a state packet interpolated between `cur` and `prev` (t = 0 is cur): the rigid body
// part, engine speed, steering, pedals, gear and the four wheels (suspension, spin with 2 pi wrap, slip, skid,
// flags), then the ground lookup at prev's position. Both sizes read are 0x44 bytes.
void fn_8018B0D8(u8* p, f32 t, CbPacket* cur, CbPacket* prev, s32* na, s32* nb) {
    int teleport = (prev->flags >> 9) & 1;
    if (teleport) {
        vf30__9RigidBodyFfP7RPacketP7RPacketPlPl(p, t, cur, cur, na, nb);
    } else {
        vf30__9RigidBodyFfP7RPacketP7RPacketPlPl(p, t, cur, prev, na, nb);
    }
    F32(p, 0x25D8) = F32(p, 0x25CC) = F32(p, 0x25D4) = PSVECMag(p + 0x118);
    f32 v0 = PkUnpack(prev->rpm, 65535.0f, -2000.0f, 14000.0f);
    f32 v1 = PkUnpack(cur->rpm, 65535.0f, -2000.0f, 14000.0f);
    F32(p, 0x25D0) = v1 + t * (v0 - v1);
    v0 = PkUnpack(prev->steer, 255.0f, -1.2f, 2.4f);
    v1 = PkUnpack(cur->steer, 255.0f, -1.2f, 2.4f);
    F32(p, 0x25DC) = F32(p, 0x25F0) = v1 + t * (v0 - v1);
    if (cur->boost) {
        f32 boost;
        if (teleport) {
            boost = PkNorm(cur->boost, 0.0f, 255.0f);
        } else {
            boost = PkNorm(prev->boost, 0.0f, 255.0f);
            boost = PkNorm(cur->boost, 0.0f, 255.0f);
        }
    }
    F32(p, 0x24A8) = 0.0f;
    F32(p, 0x251C) = 0.0f;
    F32(p, 0x25F8) = 1.0f;
    F32(p, 0x23C0) = F32(p, 0x2434) = F32(p, 0x25F0) * F32(p, 0x5B0);
    v0 = PkUnpack(prev->throttle, 255.0f, 0.0f, 1.0f);
    v1 = PkUnpack(cur->throttle, 255.0f, 0.0f, 1.0f);
    F32(p, 0x25E8) = 1.0f;
    fn_8018ADB4(p, v1 + t * (v0 - v1));
    v0 = PkUnpack(prev->brake, 255.0f, 0.0f, 1.0f);
    v1 = PkUnpack(cur->brake, 255.0f, 0.0f, 1.0f);
    fn_8018ADBC(p, v1 + t * (v0 - v1));
    U32(p, 0x4) = cur->flags | prev->flags;
    U8(p, 0x260D) = (cur->flags >> 8) & 1;
    u8* w = p + 0x23AC;
    const u8* c = cur->wheel[0];
    const u8* q = prev->wheel[0];
    for (int i = 0; i < 4; i++) {
        f32 s0 = PkUnpack(q[2], 255.0f, 0.0f, 2.0f);
        f32 s1 = PkUnpack(c[2], 255.0f, 0.0f, 2.0f);
        F32(w, 0x1C) = F32(w, 0xC) = s1 + t * (s0 - s1);
        f32 a0;
        f32 a1 = PkUnpack(c[0], 255.0f, -6.2831855f, 12.566371f);
        a0 = PkUnpack(q[0], 255.0f, -6.2831855f, 12.566371f);
        if (a0 > a1) {
            if (a0 - a1 > 3.1415927f) {
                a1 += 6.2831855f;
            }
        } else if (a1 - a0 > 3.1415927f) {
            a0 += 6.2831855f;
        }
        F32(w, 0x8) = a1 + t * (a0 - a1);
        f32 l1;
        f32 l0 = PkUnpack(q[1], 255.0f, -100.0f, 200.0f);
        l1 = PkUnpack(c[1], 255.0f, -100.0f, 200.0f);
        F32(w, 0x0) = l1 + t * (l0 - l1);
        f32 k0 = PkUnpack(q[3], 255.0f, 0.0f, 10.0f);
        f32 k1 = PkUnpack(c[3], 255.0f, 0.0f, 10.0f);
        u8 f = c[4];
        F32(w, 0x68) = k1 + t * (k0 - k1);
        S32(w, 0x70) = f;
        U8(w, 0x6C) = 1;
        U8(w, 0x20) = f & 1;
        w += 0x74;
        c += 5;
        q += 5;
    }
    u8* rec = lbl_8055E918 + S32(p, 0x1EC) * 0x9C;
    S32(rec, 0x14) = cur->gear;
    S32(rec, 0x8) = cur->gear;
    S32(p, 0x2678) = cur->w40;
    *nb = 0x44;
    *na = 0x44;
    f32 x, z;
    PkCopy(&x, prev->x, 4);
    PkCopy(&z, prev->z, 4);
    fn_801E9DD0((s16*)(p + 0x6C), x, z);
}

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
