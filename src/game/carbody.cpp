// Car body (0x8018185C-0x8018BFA0): the truck's chassis, suspension and wheel setup ("Chassis",
// "Front", "Rear", "susp_y", "squishy", "car_box", "car-volume"), terrain morph kicks and the
// per-car tuning accessors. Written against raw field offsets; the large routines are still stubs.
// Names are guesses apart from the strings.
#include "game/common.h"
#include "game/rvec.h"
#include "game/corner.h"
#include "game/raceopts.h"
#include <revolution/MTX/vec.h>
#include "game/collvol.h"
#include "game/rigid.h"
#include "game/carbody.h"
#include "game/carelem.h"
#include "game/deity.h"
#include "game/idealline.h"
#include "game/rmtx.h"
#include "game/shapes.h"
#include "game/gamedb.h"
#include <string.h>
#include "game/mathutil.h"


template <class O, class P, class V> static inline void CbProbe(O* a, O* b, const P* pos, const V* vel, f32 ka, f32 kb) { a->x = pos->x + vel->x * ka; a->y = pos->y + vel->y * ka; a->z = pos->z + vel->z * ka; b->x = pos->x + vel->x * kb; b->y = pos->y + vel->y * kb; b->z = pos->z + vel->z * kb; }
static inline f32 CbAdd(f32 a, f32 b) { return a + b; }
/* Helper name unproven */
template <class O, class V> static inline void CbSpring3(O* o, const V* x, f32 ks, const V* v, f32 kd) { f32 ax = x->x * ks; f32 ay = x->y * ks; f32 az = x->z * ks; o->x = ax + v->x * kd; o->y = ay + v->y * kd; o->z = az + v->z * kd; }
static inline void CbScaleTo(f32* o, const f32* v, f32 k) { o[0] = v[0] * k; o[1] = v[1] * k; o[2] = v[2] * k; }

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define U16(p, o) (*(u16*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

class VR {
public:
    virtual ~VR(void);  // 0x08 (the 0x2584 effects object; ~CarBody deletes it through slot 0)
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

// The game mode (fn_801B83A0, GameMode family: destructor at 0x08), reached through slot 0xCC.
class VBig {
public:
    virtual ~VBig(void);
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

// CbVec: rvec.h's RVec3.

struct CollShape;

// The body's collision shape interface (vtable slots used here).
class CbShapeV {
public:
    virtual ~CbShapeV(void);  // 0x08 (the 0x184 car volume (sphcoll lbl_803339B8): slot 0 fn_801A0040 is its destructor)
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

struct BodyDesc;

struct RPacket;
struct BodyState;

extern "C" {
void fn_801838C8(u8* p);
void fn_801A1474(u8* p, f32 s);
void fn_8018A180(u8* p);
u8* fn_801D2008(u8* name);
s32 fn_801C1C1C(const void*);  // const PSysDesc*
s32 fn_801C1BA4(s32 a, s32 b, s32 c);
}

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
void fn_801BF1B8(u8* p, u8 a);
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
f32 lbl_8055A3A0 = 1.0f;  // physics writes it
f32 lbl_8055A3A4 = 0.9f;  // offroad speed factor
f32 lbl_8055A3A8 = 0.9f;  // water speed factor
u8 lbl_8055A3AC = 1;      // enable stalling
f32 lbl_8055A3B0 = 1.5f;
u8 lbl_8055A3B4 = 1;

extern f32 __float_epsilon[];  // FLT_EPSILON

static inline int Positive(f32 x) { return x > __float_epsilon[0]; }
/* Helper name unproven: |x| held as a double and narrowed separately (DOUBLE ABS THEN NARROW) */
static inline f32 AbsNarrow(f32 x) {
    f64 absValue = __fabs(x);
    f32 value = (f32)absValue;
    return value;
}
static inline int NearZero(f32 x) {
    f64 absValue = __fabs(x);
    f32 value = (f32)absValue;
    return value <= __float_epsilon[0];
}

ModuleTag lbl_8055E4C0;
static inline u32 MakeILPos(u32 seg, u32 frac) { return (seg << 16) | frac; }
extern const u32 lbl_8055E4C8 = MakeILPos(0x7F00, 0);
CarHandle lbl_8055E4CC;
s32 lbl_8055E4D0;
s32 lbl_8055E4D4;
f32 lbl_8055E4D8;
F32Pair lbl_8055E4E0;
f32 lbl_8055E4E8[2];
f32 lbl_8055E4F0;
u8 lbl_8055E4F4;
u8 lbl_8055E4F5;
u8 lbl_8055E4F6;
u8 lbl_8055E4F7;  // the zone-mode limiter message was shown



// TODO(unpool): pool labels still read by name. As literals these sites drop (x - 0.0f, x / 1.0f and
// literal * literal fold away; creation order moves), so they need the original helper forms first.


extern "C" {

// Builds the engine torque curve from v = {hp, hp rpm, peak torque, torque rpm, redline rpm, idle torque}: a
// parabola through the torque peak and the hp point below it, another from the peak down to 30% at redline.
void fn_8018185C(const u8* p, f32* v) {
    F32(p, 0x1C) = v[1];
    F32(p, 0x18) = 1.3515152f * (v[5]);
    F32(p, 0x28) = v[3];
    f32 hp = 1.3515152f * (5252.0f * v[0] / MaxF(v[1], 0.00001f));
    F32(p, 0x24) = 0.0f;
    f32 r0 = v[3];
    f32 peak = Mul(1.3515152f, v[2]);
    f32 d = F32(p, 0x1C) - r0;
    f32 a = (hp - peak) / MaxF(d * d, 0.00001f);
    F32(p, 0x0) = a;
    F32(p, 0x8) = peak + a * (r0 * r0);
    f32 low = 1.3515152f * (0.3f * v[2]);
    F32(p, 0x4) = F32(p, 0x18) + -2.0f * a * r0;
    f32 peak2 = 1.3515152f * v[2];
    f32 top = F32(p, 0x28);
    F32(p, 0x20) = hp / 8000.0f;
    f32 red = v[4];
    d = red - top;
    f32 b = (low - peak2) / MaxF(d * d, 0.0001f);
    F32(p, 0xC) = b;
    F32(p, 0x14) = peak2 + b * (top * top);
    F32(p, 0x10) = F32(p, 0x18) + -2.0f * b * top;
}
static f32 RpmFromRad(f32 w) { return w / 0.10472f; } /* Helper name unproven */


void fn_801819A0(u8* p, const u8* cfg, f32 a, f32 b, f32 c) {
    f32 v[6];
    v[0] = F32(cfg, 0x54) * a / 746.0f;
    v[2] = F32(cfg, 0x50) * a / 1.35725f;
    v[1] = F32(cfg, 0x5C) * c;
    v[3] = F32(cfg, 0x58) * b;
    v[4] = F32(cfg, 0x68);
    v[5] = F32(cfg, 0x60);
    fn_8018185C(p, v);
}
static f32 SafeInv(f32 x) { return 1.0f / MaxF(x, 0.01f); } /* Helper name unproven */

f32 fn_80181A14(u8* p) {
    CarBody* cb = (CarBody*)p;
    f32 k;
    if (cb->f259D) {
        k = driveTune.v[6];
    } else {
        k = landingTune.v[6];
    }
    return cb->f2D8 * k;
}

f32 fn_80181A48(u8* p) {
    CarBody* cb = (CarBody*)p;
    f32 k;
    if (cb->f259D) {
        k = driveTune.v[7];
    } else if (lbl_8055E4F4) {
        k = heatTune.v[13];
    } else {
        k = landingTune.v[7];
    }
    return cb->f2D0 + k;
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

extern "C++" int OptE3Demo(void);  // cmdline.cpp (C++ linkage; this part of the file sits in an extern "C" block)
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
    if (fn_801769CC()->type & 0x100000) {
        fn_802049DC("OK-Landing Min Air-time Required", &landingTune.v[1], 0.0f, 1.5f, 0.1f);
        fn_802049DC("OK-Landing Max Speed", &landingTune.v[7], 140.0f, 200.0f, 5.0f);
        fn_802049DC("OK-Landing Boost Strength", &landingTune.v[6], 0.25f, 6.0f, 0.25f);
        fn_802049DC("OK-Landing Boost Time =", &landingTune.v[3], 0.0f, 4.0f, 0.25f);
        fn_802049DC("OK-Landing Boost Time += (Air-time X", &landingTune.v[2], 0.0f, 4.0f, 0.25f);
        fn_802049DC("OK Landing Max Boost Time", &landingTune.v[0], 1.0f, 12.0f, 1.0f);
    }
    if (fn_801769CC()->type & 0x80000) {
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
    u32 flags = fn_801769CC()->type;
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

// ValVec3C: valvec3.h's ValVec3.
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
    virtual ValVec3 GetVec3(const char* key);
    virtual ValVec3 GetVec3Or(const char* key, ValVec3 def);
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
CarLayout* fn_802792E4(const char* car);  // carlayout.cpp
s32 fn_80279874(s32);
int fn_8027AA48(void);
void fn_80279F58(CbSetup* setup, CbValueTable* table);
void* fn_8027A798(const char* model);
void fn_8027A8B4(CbSetup* setup, void* tune);
void fn_801B86BC(s32 car, void* look);
u8* fn_801B2EDC(s32 car, u8* body);
void fn_801795D8(u8* p, s32 car);
void fn_801BDA74(s32 car, u8* setup);
void fn_801BEB30(s32 car, u8** out);
void fn_801836DC(u8* p, u8* desc);
void fn_8018C5A0(u8* p, u8* desc);
void fn_8018A654(u8* p, const u8* cfg);
u8* fn_80209448(u8* p);
u8* fn_80209D4C(u8* p);
u8* fn_8020DE74(u8* p);
void fn_801D108C(s32 car);
int fn_8020503C(const char* name);
void fn_802E30C8(const char* fmt, ...);
void fn_80182DF4(u8* p, u8* desc);
}  // extern "C"
struct PlayerLook;
struct BodyDesc;
void GetPlayerLook(PlayerLook* look, s32 vehicle);
extern "C" {

// CarBody::Init: copies the descriptor's identity and setup blocks, reads the chassis tuning from
// "<model>.car/physics.val" (camera, head positions in inches, suspension, speed limiter, gravity boosts),
// sets up the rigid body, wheels, collision shapes and sub-systems, then resets the body.
/* MATCHING-AID: this CarBody member is written on the raw object under its mangled name (extern "C"); as a
   member body, MWCC keeps this+K in an extra register and the code differs. */
void Init__7CarBodyFP8BodyDesc(u8* p, u8* desc) {
    CarBody* cb = (CarBody*)p;
    char path[0x50];
    CbSetup setup;
    cb->f2680 = 0;
    cb->model = ((CarDesc*)desc)->model;
    cb->f5A4 = ((CarDesc*)desc)->fE8;
    cb->f5A0 = ((CarDesc*)desc)->ident.w70;
    if (cb->f5A0 == 0) {
        cb->f5A0 = 0x0FF807FF;
    }
    if (fn_801769CC()->type & 0x400) {
        U32(p, 0x5A0) |= 0x80000000;
    }
    cb->index = ((CarDesc*)desc)->index;
    cb->f1F0 = ((CarDesc*)desc)->f68;
    *(CbIdent*)(p + 0x1F4) = *(CbIdent*)&((CarDesc*)desc)->ident;
    cb->layout = fn_802792E4((char*)(p + 0x234));
    s32 id = cb->ident.vehicle;
    *(CbDescBlock*)(p + 0x274) = *(CbDescBlock*)&((CarDesc*)desc)->block;
    S32(p, 0x328) = fn_80279874(id);
    if (!fn_8027AA48()) {
        S32(p, 0x328) = 0;
    }
    fn_801345D8(path, 0x50, "%s.car/physics.val", (char*)(p + 0x234));
    CbValueTable* table = fn_80131808(path);
    CbValueNode* chassis = table->FindChild("Chassis");
    cb->f5B4 = chassis->GetFloat("chase_camera_dist", 0.0f);
    cb->f5B8 = chassis->GetFloat("chase_camera_height", 0.0f);
    const ValVec3& head = chassis->GetVec3("head_pos");
    cb->f5C4 = head.x;
    cb->f5C8 = head.y;
    cb->f5CC = head.z;
    cb->f5D0 = cb->f5C4;
    cb->f5D0 *= 0.2f;
    cb->f5D4 = cb->f5C8;
    cb->f5D4 += 3.0f;
    cb->f5D8 = cb->f5CC;
    const ValVec3& back = chassis->GetVec3Or("lookback_head_pos", *(ValVec3*)(p + 0x5D0));
    cb->f5D0 = back.x;
    cb->f5D4 = back.y;
    cb->f5D8 = back.z;
    cb->f5C4 *= 0.0254f;
    cb->f5C8 *= 0.0254f;
    cb->f5CC *= 0.0254f;
    cb->f5D0 *= 0.0254f;
    cb->f5D4 *= 0.0254f;
    cb->f5D8 *= 0.0254f;
    cb->f5C0 = 0.0174f * chassis->GetFloat("head_pitch");
    const char* treeClasses = "light\0medium\0heavy\0\0";
    const char* key = "treehit_class";
    cb->f5E8 = chassis->Has(key) ? chassis->GetEnum(key, treeClasses) : 1;
    GetPlayerLook((PlayerLook*)(p + 0x2C4), cb->ident.vehicle);
    fn_801B86BC(((CarDesc*)desc)->index, p + 0x2C4);
    cb->f25C0 = (FindVehicle(cb->ident.vehicle)->flags >> 2) & 1;
    fn_80279F58(&setup, table);
    fn_8027A8B4(&setup, fn_8027A798((char*)(p + 0x234)));
    setup.mass = setup.mass * cb->f2EC;
    setup.f110 = setup.f110 * (0.5f * (cb->f2F4 + cb->f2F8));
    setup.f114 = setup.f114 * cb->f2FC;
    for (s32 i = 0; i < 4; i++) {
        CbValueNode* axle = table->FindChild((i == 0 || i == 1) ? "Front" : "Rear");
        F32(p + i * 0x74, 0x23E8) = F32(desc + i * 4, 0x10C);
        F32(p + i * 0x74, 0x23EC) = 0.0254f * axle->GetFloat("max_compression", 8.0f) - F32(p + i * 0x74, 0x23E8);
        *(f32*)((u8*)&setup.wheels[i] + 0x18) = F32(p + i * 0x74, 0x23E8);
    }
    ((CarDesc*)desc)->mass = setup.mass / 0.4545f;
    ((CarDesc*)desc)->ix = setup.f04 / 0.042279866f;
    ((CarDesc*)desc)->iy = setup.f08 / 0.042279866f;
    ((CarDesc*)desc)->iz = setup.f0C / 0.042279866f;
    ((RigidBody*)p)->RigidBody::Init((BodyDesc*)desc);
    ((u8*&)cb->src) = fn_801B2EDC(cb->index, p);
    fn_801795D8(p, cb->index);
    cb->f5AC = -(0.0254f * chassis->GetFloat("susp_y", 0.0f));
    cb->f5B0 = setup.f128;
    cb->f5A8 = 444.44446f;
    if (fn_801769CC()->type != 0x180201) {
        cb->f5A8 = 0.44444445f * (cb->f2C8 + chassis->GetFloat("speed_limiter", cb->f5A8));
    } else if (!lbl_8055E4F7) {
        lbl_8055E4F7 = 1;
        fn_802E30C8("%s", "CAR: Disabling speed limiter in zone mode.");
    }
    cb->f5DC = chassis->GetFloat("gravity_boost", 0.0f);
    cb->f5E0 = chassis->GetFloat("down_gravity_boost", cb->f5DC);
    cb->f26E0 = chassis->GetBool("squishy", 0);
    cb->offset.x = setup.modelOffset.x;
    cb->offset.y = setup.modelOffset.y;
    cb->offset.z = setup.modelOffset.z;
    ((CollVolume*)p)->CollVolume::vf20();
    if (U32(p, 0x10) == 'ACAR') {
        setup.f2C = 0.75f;
    }
    *(CbSetup*)&cb->setupA = setup;
    *(CbSetup*)&cb->setupB = *(CbSetup*)&cb->setupA;
    fn_801BDA74(cb->index, (u8*)&cb->setupA);
    u8* wheels = 0;
    fn_801BEB30(cb->index, &wheels);
    for (int w = 0; w < 4; w++) {
        RVec3* d = (RVec3*)(p + 0x23DC + w * 0x74);
        d->Set(F32(wheels, w * 0x10), F32(wheels, w * 0x10 + 4), F32(wheels, w * 0x10 + 8));
    }
    fn_801836DC(p, desc);
    fn_80182DF4(p, desc);
    fn_8018C5A0(p, desc);
    fn_8018A654(p, desc);
    bool player = ((CarDesc*)desc)->kind == 'PCAR';
    cb->f257C = fn_80209448(p);
    if (player) {
        cb->f2580 = fn_80209D4C(p);
    } else {
        cb->f2580 = 0;
    }
    cb->f2584 = fn_8020DE74(p);
    fn_801D108C(cb->index);
    fn_80131818(table);
    u8 trees = 0;
    if (player && fn_8020503C("player_destroys_trees")) {
        trees = 1;
    }
    cb->f25C8 = trees;
    if (fn_801769CC()->type == 0x180304) {
        cb->f25C8 = 1;
    }
    fn_801838C8(p);
}

Corner* fn_801AE180(void);
s32 fn_801E3004(s32 model, const char* name, s32 a);
HVec* fn_801E3630(s32 joint);
void fn_801E323C(s32 joint);
void fn_802C7760(s32 model, f32* x0, f32* x1, f32* y0, f32* y1, f32* z0, f32* z1);
void fn_801A0258(u8* grp, const HVec* c, const RVec3* mat, f32 r, f32 k, f32 d, f32 m, f32 im);
void fn_801A0444(u8* grp, u8* box);
}  // extern "C"
void* operator new(unsigned long n, const char* tag);
extern "C" {

static inline void AddBodyCorner(u8* p, RVec3* pos, f32 x, f32 y, f32 z, f32 spring) {
    pos->x = x;
    pos->y = y;
    pos->z = z;
    Corner* c = fn_801AE180();
    c->pos = *pos;
    c->spring = spring;
    c->friction = 0.5f;
    c->vel.x = 0.0f;
    c->vel.y = 0.0f;
    c->vel.z = 0.0f;
    c->damping = 0.0f;
    c->next = ((RigidBody*)p)->contacts;
    ((RigidBody*)p)->contacts = c;
}

// One sphere of the body's collision group, at a point in body space (made relative to the centre of mass).
static inline void AddBodySphere(u8* p, u8* g, HVec* rel, const RVec3& at, const RVec3* mat, f32 r, f32 k, f32 d, f32 m, f32 im) {
    rel->x = at.x - F32(p, 0x30);
    rel->y = at.y - F32(p, 0x34);
    rel->z = at.z - F32(p, 0x38);
    fn_801A0258(g, rel, mat, r, k, d, m, im);
}

// Builds the body's collision: two wheel contact corners at the "wheel" joint, then a group of twelve spheres
// sized from the model's bounding box (front, middle and rear rows) and a box around the whole body.
void fn_80182DF4(u8* p, u8* desc) {
    CarBody* cb = (CarBody*)p;
    RVec3 rear;
    RVec3 mid;
    RVec3 front;
    RVec3 center;
    f32 minX = 0.0f;
    f32 maxX = 0.0f;
    f32 minY = 0.0f;
    f32 maxY = 0.0f;
    f32 minZ = 0.0f;
    f32 maxZ = 0.0f;
    f32 stiff;
    f32 damp;
    stiff = 50000.0f;
    f32 mass = 4938240000.0f;
    f32 invMass = 0.5f / mass;
    f32 zRear;
    f32 zFront;
    damp = 0.3f;
    fn_802C7760(cb->model, &minX, &maxX, &minY, &maxY, &minZ, &maxZ);
    s32 wheel = fn_801E3004(cb->model, "wheel", -1);
    if (wheel) {
        HVec* o = fn_801E3630(wheel);
        {
            RVec3 pos;
            AddBodyCorner(p, &pos, o->x, maxY + cb->offset.y + 0.1016f, o->z, 2.0f * stiff);
        }
        {
            RVec3 pos;
            AddBodyCorner(p, &pos, -o->x, maxY + cb->offset.y + 0.1016f, o->z, 2.0f * stiff);
        }
        fn_801E323C(wheel);
    }
    minY = MaxF(minY, 0.2f);
    HVec c = *(HVec*)&cb->offset;
    minX += c.x;
    maxX += c.x;
    minY += c.y;
    maxY += c.y;
    minZ += c.z;
    maxZ += c.z;
    if (cb->f1F0 == 84 && !(fn_801769CC()->type & 0x1000)) {
        maxZ -= 30.0f * 0.0254f;  // 30 in
    }
    cb->f5BC = maxZ;
    u8* g = (u8*)new ("car-volume") SphereGroup((const RMtx*)&cb->mtx, (CollVolume*)p);
    cb->f268 = g;
    rear.Set(0.7f, 0.1f, 1.0f);
    mid.Set(0.7f, 0.1f, 1.0f);
    front.Set(0.7f, 0.1f, 1.0f);
    zRear = maxZ - 0.45000002f;
    zFront = 0.44999998f + minZ;
    f32 yMid = 0.45000002f + minY;
    f32 a = maxX - 0.45000002f;
    f32 yRear = 0.36f + minY;
    f32 h = 0.5f * (maxY - minY);
    f32 zMid = 0.5f * (minZ + maxZ);
    f32 zRearMid = 0.5f * (zRear + zMid);
    f32 b = 0.8f * a;
    f32 xSide = maxX - h;
    f32 zFrontMid = 0.5f * (zFront + zMid);
    f32 yFront = 0.44999998f + minY;
    f32 ySide = h + minY;
    f32 xFront = maxX - 0.44999998f;
    f32 nb = -b;
    f32 yr = 0.254f + yRear;
    {
        HVec rel;
        AddBodySphere(p, cb->f268, &rel, RVec3(nb, yr, zRear), &mid, 0.36f, stiff, damp, mass, invMass);
    }
    {
        HVec rel;
        AddBodySphere(p, cb->f268, &rel, RVec3(0.0f, 0.254f + yRear, zRear), &mid, 0.36f, stiff, damp, mass, invMass);
    }
    {
        HVec rel;
        AddBodySphere(p, cb->f268, &rel, RVec3(b, 0.254f + yRear, zRear), &mid, 0.36f, stiff, damp, mass, invMass);
    }
    {
        HVec rel;
        AddBodySphere(p, cb->f268, &rel, RVec3(-a, 0.254f + yMid, zRearMid), &mid, 0.45000002f, stiff, damp, mass, invMass);
    }
    {
        HVec rel;
        AddBodySphere(p, cb->f268, &rel, RVec3(a, 0.254f + yMid, zRearMid), &mid, 0.45000002f, stiff, damp, mass, invMass);
    }
    {
        HVec rel;
        AddBodySphere(p, cb->f268, &rel, RVec3(0.8f * -xSide, 0.254f + ySide, zMid), &rear, h, stiff, damp, mass, invMass);
    }
    {
        HVec rel;
        AddBodySphere(p, cb->f268, &rel, RVec3(0.8f * xSide, 0.254f + ySide, zMid), &rear, h, stiff, damp, mass, invMass);
    }
    {
        HVec rel;
        AddBodySphere(p, cb->f268, &rel, RVec3(-xFront, 0.254f + yFront, zFrontMid), &front, 0.44999998f, stiff, damp, mass, invMass);
    }
    {
        HVec rel;
        AddBodySphere(p, cb->f268, &rel, RVec3(xFront, 0.254f + yFront, zFrontMid), &front, 0.44999998f, stiff, damp, mass, invMass);
    }
    {
        HVec rel;
        AddBodySphere(p, cb->f268, &rel, RVec3(-xFront, 0.254f + yFront, zFront), &front, 0.44999998f, stiff, damp, mass, invMass);
    }
    {
        HVec rel;
        AddBodySphere(p, cb->f268, &rel, RVec3(0.0f, 0.254f + yFront, zFront), &front, 0.44999998f, stiff, damp, mass, invMass);
    }
    {
        HVec rel;
        AddBodySphere(p, cb->f268, &rel, RVec3(xFront, 0.254f + yFront, zFront), &front, 0.44999998f, stiff, damp, mass, invMass);
    }
    center.Set(0.5f * (minX + maxX), 0.5f * (minY + maxY), 0.5f * (minZ + maxZ));
    u8* box = (u8*)new ("car_box") OrientedBoxShape((const RMtx*)&cb->mtx, &center, (CollVolume*)p, 5.0f * stiff, damp, 0.5f * (maxX - minX), 0.5f * (maxY - minY),
                          0.5f * (maxZ - minZ));
    fn_801A0444(cb->f268, box);
    ((CollVolume*)p)->AddShape((CollShape*)cb->f268);
    cb->b1E8 = 1;
    ((CbShapeV*)cb->f268)->SetGroup(0x6D);
    if (U32(p, 0x10) == 'PCAR') {
        ((CbShapeV*)cb->f268)->SetMask(1);
    } else {
        ((CbShapeV*)cb->f268)->SetMask(6);
    }
}

void fn_801836DC(u8* p, u8* desc) {}

}  // extern "C"

static inline void Scale(RVec3* o, const RVec3* a, f32 k) {
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
    CVec4(const CVec4& o) { Set(o.x, o.y, o.z, o.w); }
    // Arguments evaluate right to left: the copy reads w first and stores x first.
    void Set(f32 x_, f32 y_, f32 z_, f32 w_) {
        x = x_;
        y = y_;
        z = z_;
        w = w_;
    }
    f32 x, y, z, w;
};

#include "game/cargear.h"

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
    if ((CbPart*)this->f257C) {
        delete (CbPart*)this->f257C;
    }
    if ((CbPart*)this->f2580) {
        delete (CbPart*)this->f2580;
    }
    delete (CbPart*)this->f2584;
    fn_8018A6F4((u8*)this);
    fn_802792EC(((u8*&)this->layout));
    lbl_8055E4D0--;
}

extern "C" {

u8* __dt__12CarSoundBaseFv(u8* p, int del) {  // ~CarSoundBase (unk_8020DE74), weak copy
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

}  // extern "C"

void CarBody::Reset(void) {
    u8* p = (u8*)this;
    *(CbPose*)(p + 0x3C) = *(CbPose*)(p + 0x274);
    ((CollVolume*)p)->Transform();
    ((RigidBody*)p)->RigidBody::Reset();
    fn_801838C8(p);
}


// The body's own virtuals (CollVolume/RigidBody slots); 0x54 places it at a pose.
class CbBodyV {
public:
    virtual void v08(void);
    virtual ~CbBodyV(void);  // 0x0C (CarBody/RigidBody family: Init at 0x08, destructor at 0x0C)
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
    virtual ~CbModel(void);  // 0x08 (the same car volume: slot 0 is its destructor)
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
    RVec3 pts[162];   // offset 0x5FC
    RVec3 vels[162];  // offset 0xD94
};

extern "C" {
void fn_8018BE54(u8* p, f32 s);
void fn_8018B0D0(u8* p, u32 v);
DeityCar* fn_801B2ED0(int id);
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
    CarBody* cb = (CarBody*)p;
    cb->f5E4 = 0.0f;
    fn_8018BE54(p, 1.0f);
    cb->f5EC = 0.0f;
    cb->f5F0 = 1.0f;
    cb->f26E4 = 0.0f;
    cb->f26E8 = 0.0f;
    cb->f26EC = 0.0f;
    cb->f26F0 = 0.0f;
    cb->f26F4 = 0.0f;
    cb->f26F8 = 0.0f;
    cb->f26FC = 0.0f;
    fn_80189C64(p);
    cb->nAdded = cb->nSub;
    fn_8018B0D0(p, 0);
    cb->f25E8 = 0.0f;
    cb->f25D8 = 0.0f;
    cb->speed = 0.0f;
    cb->f25D0 = 0.0f;
    cb->f25CC = 0.0f;
    cb->f25EC = 0.0f;
    cb->f25E4 = 0.0f;
    cb->f25E0 = 0.0f;
    cb->f25DC = 0.0f;
    cb->f25E8 = 1.0f;
    cb->f25F8 = 0.0f;
    cb->f25F4 = 0.0f;
    cb->f25F0 = 0.0f;
    cb->f25FC = 0.0f;
    cb->f2604 = 0.0f;
    cb->f2600 = 0.0f;
    cb->f2608 = 0.0f;
    cb->f260C = 0;
    cb->f260D = 0;
    cb->f260E = 0;
    cb->f2614 = 0.0f;
    cb->f2618 = 0.0f;
    cb->f2594 = 0.0f;
    cb->f2598 = 0.0f;
    cb->f261C = 0.0f;
    cb->f2588 = 0.0f;
    cb->f2590 = 0.0f;
    cb->f258C = -5.0f;
    cb->f259C = 0;
    cb->f259D = 0;
    cb->f25BC = 0.0f;
    cb->f25C4 = 0.0f;
    cb->f25A0 = 0.0f;
    cb->f25A4 = 0.0f;
    cb->f25A8 = 1.0f;
    cb->f25AC = 0.0f;
    cb->airborne = 0;
    cb->f2654 = 0.0f;
    cb->f2658 = 0.0f;
    cb->f265C = 0.0f;
    cb->f2660 = 0.0f;
    cb->f2664 = 0.0f;
    cb->f2668 = 0.0f;
    cb->f266C = 0.0f;
    cb->f2670 = 0.0f;
    cb->f2674 = 0.0f;
    cb->f2678 = 0;
    cb->f267C = 0;
    cb->f2688 = 0.0f;
    cb->a268C = 0;
    cb->f2690 = 0.0f;
    cb->f2694 = 0.0f;
    cb->f2698 = 0.0f;
    cb->f269C = 0.0f;
    cb->f26A8 = 0.0f;
    memset(p + 0x26A0, 0, 8);
    cb->f2648 = 0;
    cb->f2624 = 0;
    cb->f2625 = 0;
    cb->f263C = 0;
    cb->f2638 = 0;
    cb->f2628 = 0.0f;
    cb->f262C = 0.0f;
    cb->f2630 = 0.0f;
    cb->f2634 = 0.0f;
    cb->f2644 = 0;
    cb->f2640 = 0;
    cb->f264C = 0.0f;
    cb->f2650 = 0.0f;
    cb->f26AC = 0;
    fn_801BF1B8(PTR(p, 0x1EC), 0);
    cb->f26B0 = -1;
    cb->push.x = 0.0f;
    cb->push.y = 0.0f;
    cb->push.z = 0.0f;
    fn_801BDA74(cb->index, (u8*)&cb->setupA);
    CbSprings* sp = (CbSprings*)p;
    for (int i = 0; i < 162; i++) {
        memset(&sp->pts[i], 0, sizeof(RVec3));
        sp->pts[i].x = 0.0f;
        sp->pts[i].y = 0.0f;
        sp->pts[i].z = 0.0f;
        sp->vels[i].x = 0.0f;
        sp->vels[i].y = 0.0f;
        sp->vels[i].z = 0.0f;
    }
    CbJoint* j = ((CarBody*)p)->joints;
    for (int i = 0; i < 18; i++, j++) {
        j->spin = 0.0f;
        j->angle = 0.0f;
        j->state = 0;
        j->valid = 0;
        f32 (*q)[4] = (f32 (*)[4])&j->cur;  // both frames as six identity rows
        for (int k = 0; k < 6; k++) {
            q[k][0] = 1.0f;
            q[k][1] = 0.0f;
            q[k][2] = 0.0f;
            q[k][3] = 0.0f;
        }
    }
    cb->f1E3C = 0;
    cb->f1E40 = 0;
    u8* w = (u8*)&cb->wheels[0];
    for (int i = 0; i < 4; i++, w += 0x74) {
        ((CbWheel*)w)->spin = 0.0f;
        ((CbWheel*)w)->loadN = 0.0f;
        ((CbWheel*)w)->load = 0.0f;
        ((CbWheel*)w)->v18 = 0.0f;
        ((CbWheel*)w)->steer = 0.0f;
        ((CbWheel*)w)->v10 = 0.0f;
        ((CbWheel*)w)->prevSlip = 0.0f;
        ((CbWheel*)w)->slip = 0.0f;
        ((CbWheel*)w)->grounded = 0;
        ((CbWheel*)w)->contact[0] = 0.0f;
        ((CbWheel*)w)->contact[1] = 0.0f;
        ((CbWheel*)w)->contact[2] = 0.0f;
        ((CbWheel*)w)->local[0] = 0.0f;
        ((CbWheel*)w)->local[1] = 0.0f;
        ((CbWheel*)w)->local[2] = 0.0f;
        ((CbWheel*)w)->plane.nRight = 0.0f;
        ((CbWheel*)w)->plane.nFwd = 0.0f;
        ((CbWheel*)w)->plane.ground[0] = 0.0f;
        ((CbWheel*)w)->plane.ground[1] = 0.0f;
        ((CbWheel*)w)->plane.ground[2] = 0.0f;
        ((CbWheel*)w)->normal[0] = 0.0f;
        ((CbWheel*)w)->normal[1] = 1.0f;
        ((CbWheel*)w)->normal[2] = 0.0f;
        ((CbWheel*)w)->groundH = 0;
        ((CbWheel*)w)->comp = 0.0f;
        U8(w, 0x6C) = 0;
        S32(w, 0x70) = 0;
    }
    IdealFollower* f = fn_801B2ED0(cb->index)->Ideal();
    f->il.mode = 0;
    fn_801B8100(p + 0x274, cb->index, cb->f1F0);
    ((CbBodyV*)p)->SetPose(p + 0x274);
    f->ClearFlag();
    fn_8018F2C0(p, !fn_801CF064() ? (u8*)&cb->state : (u8*)&cb->saved);
    ((CbModel*)cb->f268)->Reset();
    if (*fn_801E1F44() == 1) {
        fn_8018F6F8(p);
    }
    cb->f26CC = 1.0f;
    cb->f26D0 = 1.0f;
    cb->f26D4 = 0;
    cb->f26D8 = -1000.0f;
    cb->f26DC = -1000.0f;
    cb->f2684 = 9.81f;
    ((CbFx*)((CarBody*)p)->f2584)->Reset();
}

}  // extern "C"

void CarBody::vf14(void) {
    u8* p = (u8*)this;
    if (PTR(p, 0x328)) {
        fn_8018F538(p, p + 0x1E44);
        fn_80279C00(PTR(p, 0x328), p + 0x1E44);
        if (fn_801769C4() == 2 && U32(p, 0x10) != 'PCAR' && !(U8(p, 0x260) & 4)) {
            fn_80181EAC(PTR(p, 0x328));
            fn_80279B28(PTR(p, 0x328), p + 0x1E44);
        }
    }
}


// The physics solver's per-body records (masslist.cpp names them MassSet/ContactSet/CrossSet/CxEntry).
/* Helper name unproven */ struct CbSolPlain {
    f32 x, y, z, w;
};
struct CbSolVec : CbSolPlain {
    CbSolVec(void) {}
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

#include "game/surftype.h"

class CbFlagsV {
public:
    virtual ~CbFlagsV(void);  // 0x08 (the car's Controller (0x270C, Hybrid/IDriver): slot 0 is its destructor)
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
    virtual ~CbTickV(void);  // 0x0C (CarBody/RigidBody family: Init at 0x08, destructor at 0x0C)
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
    v.y = y;
    v.x = x;
    v.z = z;
    v.w = w;
    return v;
}

// CbV3: rvec.h's RVec3.
// The game-side 4-vector: built from a solver record through its xyz and w, written back through a temporary.
/* Helper names unproven (RVec3, CbF, CbVec4, Set, Sol, CbFrame): the shapes come from the target copies (2026-11-08), the names are guesses. CbF (a one-float value passed by value) gives retail's float store + word copy of the w lane (2026-11-14). */
struct CbF {
    CbF(f32 a) { v = a; }
    f32 v;
};

struct CbVec4 {
    CbVec4(void) {}
    CbVec4(f32 x_, f32 y_, f32 z_, f32 w_) {
        x = x_;
        y = y_;
        z = z_;
        w = w_;
    }
    CbVec4(CbSolPlain s) {
        Set(RVec3(s.x, s.y, s.z), CbF(s.w));
    }
    void Set(const RVec3& p, CbF w_) {
        x = p.x;
        y = p.y;
        z = p.z;
        w = w_.v;
    }
    CbSolVec Sol(void) const { return CbSolVec4(x, y, z, w); }
    f32& operator[](int i) { return (&x)[i]; }
    f32 x, y, z, w;
};

extern "C" {
void fn_80190E6C(void);
int fn_801C5B60(void);
void fn_80196A9C(void);  // aicar.cpp defines it with an unused u8* parameter; retail's call here passes nothing
void fn_801C56AC(void);
void fn_801BEDA8(CbSolMass** m, CbSolContact** c, CbSolCross** x, CbSolCx** cx, s32* n);
void fn_801BEDD4(void);
void fn_801BF1B4(void);
SurfType* fn_801D04A8(int i);
int fn_802B03C0(const f32* p);
int fn_802B02C8(const u8* p);
int fn_802BF4F0(void);
f32 fn_802BF590(void);
void fn_8018CB3C(u8* p);
u32 fn_8018B06C(u8* p);
double asin(double x);
double fmod(double x, double y);
extern CarElem* lbl_8055E908;  // carelem.cpp: one per car
}

static inline f32 Clamp01(f32 x) {
    f32 t = 1.0f < x ? 1.0f : x;
    return t > 0.0f ? t : 0.0f;
}
static inline f32 Ramp(f32 x, f32 lo, f32 hi) { return Clamp01((x - lo) / (hi - lo)); } /* Helper name unproven */

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
    u8 dive;  // retail stores a byte (stb); the event stays 12 bytes
};

struct CbSpinEvent {
    s32 id;
    bool left;  // retail stores the compare result unmasked (srwi + stb)
};

// out = v[0] * row0 + v[1] * row1 + v[2] * row2 (m: rows at 0, 3, 6), safe when out aliases v.
/* Helper name unproven: rows of m dotted with v (the inverse of ObRotateRows for a rotation) */
static inline void ObUnrotateRows(RVec3* out, const f32* m, const f32* v) {
    if ((const f32*)out == v) {
        f32 x = Dot3(v[0], m[0], v[1], m[1], v[2], m[2]);
        f32 y = Dot3(v[0], m[3], v[1], m[4], v[2], m[5]);
        f32 z = Dot3(v[0], m[6], v[1], m[7], v[2], m[8]);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(v[0], m[0], v[1], m[1], v[2], m[2]);
        out->y = Dot3(v[0], m[3], v[1], m[4], v[2], m[5]);
        out->z = Dot3(v[0], m[6], v[1], m[7], v[2], m[8]);
    }
}
static inline void ObRotateRows(RVec3* out, const f32* m, const f32* v) {
    if ((const f32*)out == v) {
        f32 x = Dot3(v[0], m[0], v[1], m[3], v[2], m[6]);
        f32 y = Dot3(v[0], m[1], v[1], m[4], v[2], m[7]);
        f32 z = Dot3(v[0], m[2], v[1], m[5], v[2], m[8]);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(v[0], m[0], v[1], m[3], v[2], m[6]);
        out->y = Dot3(v[0], m[1], v[1], m[4], v[2], m[7]);
        out->z = Dot3(v[0], m[2], v[1], m[5], v[2], m[8]);
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
f32 fn_801DDC8C(void* q, f32 x, f32 z);  // heightfld; retail passes p + 0x6C first
f32 fn_801DCF48(RVec3* n, f32 x, f32 z);
void fn_802E30C8(const char* fmt, ...);
extern s32 lbl_8055E3C4;
int fn_8014E708(s32 id);
void fn_801DF074(int a, int b);
f32 fn_801B8950(s32 id);
void fn_801B8964(s32 id, f32 v);
void fn_8018E330(u8* p);
f32 fn_801DCEC0(s16 id, f32 x, f32 z);  // heightfld.cpp's stub ignores its arguments (retail passes them)
double sqrt(double x);
}

extern "C" {

// Before a physics step: copy the solver's wheel ground points/normals in, run the timers (stun, wheelie, air,
// boost, heat, spin), apply the self-righting, stunt, landing, ramp, water, slope and steering forces/torques,
// track boost/overheat state and tricks (flips and spins counted while airborne), and drive the sound/FX hooks.
/* Helper name unproven: eases the stored ground normal towards a new one */
static inline void EaseNormal(u8* p, f32 t, const RVec3& up) {
    RVec3 n(LerpT(t, ((CarBody*)p)->f25A4, up.x), LerpT(t, ((CarBody*)p)->f25A8, up.y), LerpT(t, ((CarBody*)p)->f25AC, up.z));
    *(RVec3*)(p + 0x25A4) = n;
}
/* Helper name unproven: world up crossed with a vector */
static inline void UpCross(RVec3* out, const Vec* v) {
    RVec3 y(0.0f, 1.0f, 0.0f);
    PSVECCrossProduct((const Vec*)&y, v, (Vec*)out);
}
/* Helper name unproven: an upward impulse at the centre of mass */
static inline void KickUp(u8* p, f32 k) {
    RVec3 kick(0.0f, 10.0f * k, 0.0f);
    ((RigidBody*)p)->AddImpulse(1.0f, (const RVec3*)&kick, (const RVec3*)(p + 0x60));
}
/* Helper name unproven: a vertical force at the centre of mass */
static inline void LiftUp(u8* p, f32 k) {
    RVec3 f;
    f.x = 0.0f;
    f.z = 0.0f;
    f.y = k;
    ((RigidBody*)p)->AddForceAt((const RVec3*)&f, (const RVec3*)(p + 0x60));
}
/* Helper name unproven: Clamp01((x - lo) / w) with lo and w kept as values */
static inline f32 RampW(f32 x, f32 lo, f32 w) { return Clamp01((x - lo) / w); }
static inline int CarHot(u8* car) { return ((CarBody*)car)->f258C > 0.001f; }
static inline int CarOnGas(u8* car) { return ((CarBody*)car)->f25E0 > 0.5f; }
/* Helper name unproven */
template <class O, class A, class V> static inline void CbAddScaled(O* o, const A* a, const V* v, f32 k) { o->x = a->x + v->x * k; o->y = a->y + v->y * k; o->z = a->z + v->z * k; }
/* Helper name unproven */
template <class O> static inline void CbScaleSelf(O* o, f32 k) { o->x = o->x * k; o->y = o->y * k; o->z = o->z * k; }
void fn_80183E7C(u8* p) {
    CarBody* cb = (CarBody*)p;
    CbSolContact* sol;
    if (IsLoadingActive()) {
        cb->f25DC = 0.0f;
        cb->f25E0 = 0.0f;
        cb->f25E4 = 1.0f;
        cb->f260D = 0;
    }
    if (cb->f25C0) {
        cb->f25BC = lbl_8055E4E0.a;
    }
    fn_801BEB1C(cb->index, &sol);
    for (int i = 0; i < 4; i++) {
        ((RVec3*)((CbWheel*)(p + 0x23AC))[i].plane.ground)->Set(sol->v150[i].x, sol->v150[i].y, sol->v150[i].z);
        ((RVec3*)((CbWheel*)(p + 0x23AC))[i].normal)->Set(sol->v110[i].x, sol->v110[i].y, sol->v110[i].z);
    }
    if (cb->f260D) {
        cb->f2598 = CbAdd(cb->f2598, 0.016683351f);
    } else {
        cb->f2598 = 0.0f;
    }
    cb->f2650 = MaxF(0.0f, cb->f2650 - 0.016683351f);
    if (!NearZero(cb->f2650)) {
        if (cb->speed < 8.888889f) {
            cb->f2650 = MinF(cb->f2650, 0.5f);
        }
        f32 roll;
        if (cb->airborne > 2 && (roll = PSVECDotProduct((const Vec*)(p + 0x124), (const Vec*)(p + 0x48)), (f32)__fabs(roll) < 2.0f)) {
            f32 a = LerpT(Ramp(cb->f2650, 0.0f, 1.0f), 0.0f, 1.0f);
            f32 m = cb->mass;
            f32 b = Clamp01((cb->speed - 4.4444447f) / 8.888888f);
            f32 k = LerpT(b, 0.0f, 1.0f) * (m * 100.0f * 9.81f * 0.305f * a);
            RVec3 t;
            t.x = cb->mtx.rot.y.x * (roll > 0.0f ? k : -k);
            t.y = cb->mtx.rot.y.y * (roll > 0.0f ? k : -k);
            t.z = cb->mtx.rot.y.z * (roll > 0.0f ? k : -k);
            ((RigidBody*)p)->AddTorque((const RVec3*)&t);
        }
    }
    if (cb->f258C > 0.01f || cb->f259C) {
        CbEvent ev;
        ev.id = cb->index;
        ev.a = 1.0f;
        ev.b = cb->f25D0;
        ev.c = -1;
        fn_801796CC(4, &ev, sizeof(ev));
    }
    if (cb->f2608 - cb->f25E0 > 0.2f) {
        cb->f2608 = 0.0f;
        if (fn_80129D18(4, 10) < 3) {
            ((CbFx*)((CarBody*)p)->f2584)->v1C();
            CbEvent ev;
            ev.id = cb->index;
            ev.a = 1.0f;
            ev.b = cb->f25D0;
            ev.c = -1;
            fn_801796CC(4, &ev, sizeof(ev));
        }
    }
    cb->f2608 = LerpT(0.99f, cb->f25E0, cb->f2608);
    f32 airLift = 0.0f;
    int still;
    if (!(fn_801769CC()->type & 0x4000)) {
        still = NearZero(cb->f2690);
    } else {
        still = !(NearZero(cb->f2690 - 1.0f));
    }
    if (!NearZero(cb->f2650)) {
        still = 0;
    }
    if (still && (U32(p, 0x10) != 'PCAR' || ((DeityCar*)cb->src)->v14())) {
        if (cb->airborne <= 2 && cb->mtx.rot.y.y < 0.35f) {
            if (cb->airborne == 0 || !(AbsNarrow(cb->f2614) <= __float_epsilon[0]) || cb->mtx.rot.y.y < 0.0f) {
                f32 t = CbAdd(cb->f2614, 0.016683351f);
                cb->f2614 = t;
                if (cb->speed < 8.0f) {
                    cb->f2614 = CbAdd(t, 0.025025027f);
                }
            } else {
                cb->f2614 = 0.0f;
            }
        } else {
            cb->f2614 = 0.0f;
        }
        if (!(cb->f2614 < 3.0f)) {
            if (cb->mtx.rot.y.y > 0.5f) {
                cb->f2614 = 0.0f;
            } else if (!(AbsNarrow(cb->mtx.rot.z.y) > 0.5f)) {
                f32 side = -cb->mtx.rot.x.y;
                if ((f32)__fabs(-cb->mtx.rot.x.y) < 0.01f) {
                    side = 0.1f;
                }
                RVec3 axis;
                axis.x = cb->mtx.rot.z.x * side;
                axis.y = cb->mtx.rot.z.y * side;
                axis.z = cb->mtx.rot.z.z * side;
                f32 m = cb->mass;
                f32 k = m * 9.81f * 30.0f * 0.305f;
                RVec3 t;
                t.x = axis.x * k;
                t.y = axis.y * k;
                t.z = axis.z * k;
                f32 spin = 5.0f * (PSVECDotProduct((const Vec*)(p + 0x124), (const Vec*)&axis) - 1.5f);
                f32 damp = 1.0f - Clamp01(spin);
                t.x = t.x * damp;
                t.y = t.y * damp;
                t.z = t.z * damp;
                ((RigidBody*)p)->AddTorque((const RVec3*)&t);
            }
        }
    }
    if (cb->airborne == 0) {
        cb->f2648 = 7;
    } else if (cb->airborne >= 3) {
        cb->f2648 = 0;
        cb->f2624 = 0;
        cb->f2625 = 0;
    } else if (cb->f2648 == 0 && cb->airborne < 3) {
        cb->f2648 = 1;
    }
    if (cb->f2648 >= 1 && cb->f2648 <= 6) {
        RVec3 t;
        CbScaleTo(&t.x, &cb->avel.x, -10000.0f);
        if (NearZero(cb->f2650)) {
            ((RigidBody*)p)->AddTorque((const RVec3*)&t);
        }
        cb->f2648 = cb->f2648 + 1;
    }
    RVec3 a;
    RVec3 b;
    RVec3 up;
    a.x = cb->wheels[0].plane.ground[0] - cb->wheels[3].plane.ground[0];
    a.y = cb->wheels[0].plane.ground[1] - cb->wheels[3].plane.ground[1];
    a.z = cb->wheels[0].plane.ground[2] - cb->wheels[3].plane.ground[2];
    b.x = cb->wheels[1].plane.ground[0] - cb->wheels[2].plane.ground[0];
    b.y = cb->wheels[1].plane.ground[1] - cb->wheels[2].plane.ground[1];
    b.z = cb->wheels[1].plane.ground[2] - cb->wheels[2].plane.ground[2];
    PSVECCrossProduct((const Vec*)&a, (const Vec*)&b, (Vec*)&up);
    EaseNormal(p, 0.05f, up);
    f32 len = PSVECMag((const Vec*)(p + 0x25A4));
    if (len > 0.01f) {
        f32 inv = 1.0f / len;
        cb->f25A4 = cb->f25A4 * inv;
        cb->f25A8 = cb->f25A8 * inv;
        cb->f25AC = cb->f25AC * inv;
    } else {
        cb->f25A4 = 0.0f;
        cb->f25A8 = 1.0f;
        cb->f25AC = 0.0f;
    }
    f32 dn;
    if (cb->f2590 > 0.001f && (dn = PSVECDotProduct((const Vec*)(p + 0x25A4), (const Vec*)(p + 0x118))) > 0.001f &&
        dn < 4.4444447f && cb->airborne > 0) {
        f32 mass0 = cb->mass;
        f32 k = (4.4444447f - dn) * mass0 / 0.016683351f;
        RVec3 f;
        f.x = cb->f25A4 * k;
        f.y = cb->f25A8 * k;
        f.z = cb->f25AC * k;
        ((RigidBody*)p)->AddForceAt((const RVec3*)&f, (const RVec3*)(p + 0x60));
    }
    int stunts;
    u32 ramp = (fn_801769CC()->type >> 20) & 1;
    u32 jump = (fn_801769CC()->type >> 21) & 1;
    stunts = 0;
    if (ramp || jump) {
        stunts = 1;
    }
    if (stunts && cb->airborne && cb->f2618 > 0.35f) {
        f32 sink = -PSVECDotProduct((const Vec*)(p + 0x25A4), (const Vec*)(p + 0x118));
        f32 lim = LerpT(Clamp01((cb->f2618 - 2.9f) / 0.099999905f), landingTune.v[5], landingTune.v[4]);
        if (jump) {
            lim = driveTune.v[4];
        }
        HVec dir = *(HVec*)(p + 0x118);
        f32 inv = 1.0f / MaxF(1.0f, PSVECMag((const Vec*)(p + 0x118)));
        dir.x = dir.x * inv;
        dir.y = dir.y * inv;
        dir.z = dir.z * inv;
        f32 into = -PSVECDotProduct((const Vec*)&dir, (const Vec*)(p + 0x25A4));
        RVec3 slope;
        RVec3 ahead;
        RVec3 behind;
        CbProbe(&ahead, &behind, &cb->mtx.pos, &cb->vel, 0.5f, -2.0f);
        // MATCHING-AID: CollVolume::hint (0x6C) by raw address: a member address costs this function an instruction (3 forms tried 2026-11-16)
        ahead.y = fn_801E9FDC((s16*)(p + 0x6C), ahead.x, ahead.z);
        behind.y = fn_801E9FDC((s16*)(p + 0x6C), behind.x, behind.z);
        slope.x = ahead.x - behind.x;
        slope.y = ahead.y - behind.y;
        slope.z = ahead.z - behind.z;
        inv = 1.0f / MaxF(0.01f, PSVECMag((const Vec*)&slope));
        slope.x = slope.x * inv;
        slope.y = slope.y * inv;
        slope.z = slope.z * inv;
        f32 flat = PSVECDotProduct((const Vec*)&slope, (const Vec*)(p + 0x25A4));
        int smooth = 0;
        if (0.5f * (flat / -0.2f + into / 0.4f) > 1.0f && lbl_8055E4F6) {
            smooth = 1;
        }
        f32 tilt = PSVECDotProduct((const Vec*)(p + 0x25A4), (const Vec*)(p + 0x48));
        int bad = 0;
        int hard = 0;
        if (!smooth && tilt > lim && cb->f2618 > landingTune.v[1]) {
            hard = 1;
        }
        if (hard && sink < landingTune.v[8]) {
            bad = 1;
        }
        if (!fn_802B04D4(p + 0x60)) {
            if (ramp) {
                if (tilt < 0.9f && cb->f2618 > 0.5f) {
                    if (!((f32)__fabs(lbl_8055E4F0) <= __float_epsilon[0])) {
                        ((CbFx*)((CarBody*)p)->f2584)->v24();
                    }
                    cb->f2590 = lbl_8055E4F0;
                    fn_8014E61C(cb->index, 2, 0.9f - tilt);
                } else if (bad && !(!NearZero(cb->f2690))) {
                    cb->f259D = 0;
                    f32 b2;
                    f32 boost = landingTune.v[0];
                    b2 = cb->f2618 * fn_80181A94();
                    b2 = cb->f2CC * fn_80181AC0() + b2;
                    boost = MinF(b2, boost);
                    cb->f258C = cb->setupA.f74 * boost;
                    fn_8014E61C(cb->index, 1, sink);
                    fn_80161A38(2, cb->index, 0);
                    ((CbFx*)((CarBody*)p)->f2584)->v20();
                } else {
                    ((CbFx*)((CarBody*)p)->f2584)->Land(sink);
                }
            }
            if (jump && fn_8018A100(p) && fn_801E1048(cb->index)) {
                fn_8014E61C(cb->index, 0, 0.0f);
                if (bad) {
                    fn_8014E61C(cb->index, 1, sink);
                }
            }
        }
    }
    if (cb->f2618 > 0.35f) {
        f32 sink = -PSVECDotProduct((const Vec*)(p + 0x25A4), (const Vec*)(p + 0x118));
        if (sink > 13.333333f &&
            (cb->mtx.pos.y - cb->wheels[3].plane.ground[1] - 4.0f * (0.016683351f * sink) < 0.0f || cb->airborne)) {
            f32 ex = MaxF(sink - 13.333333f, 0.0f);
            f32 k = ex * cb->mass / 0.016683351f;
            RVec3 f;
            f.x = cb->f25A4 * k;
            f.y = cb->f25A8 * k;
            f.z = cb->f25AC * k;
            ((RigidBody*)p)->AddForceAt((const RVec3*)&f, (const RVec3*)(p + 0x60));
        }
    }
    if (cb->airborne && cb->f2618 > 0.35f) {
        HVec right = *(HVec*)(p + 0x3C);
        f32 slide = -PSVECDotProduct((const Vec*)&right, (const Vec*)(p + 0x118));
        f32 t = Clamp01((cb->f2618 - 0.35f) / 1.15f);
        f32 lt = LerpT(t, 0.0f, 0.5f);
        f32 d;
        if (slide > 0.0f) {
            d = MaxF(slide - 2.2222223f, 0.0f);
        } else {
            d = MinF(2.2222223f + slide, 0.0f);
        }
        f32 mass2 = cb->mass;
        f32 k = lt * d * mass2 / 0.016683351f;
        RVec3 f;
        f.x = right.x * k;
        f.y = right.y * k;
        f.z = right.z * k;
        f32 n = -PSVECDotProduct((const Vec*)(p + 0x25A4), (const Vec*)&f);
        f.x = f.x + cb->f25A4 * n;
        f.y = f.y + cb->f25A8 * n;
        f.z = f.z + cb->f25AC * n;
        ((RigidBody*)p)->AddForceAt((const RVec3*)&f, (const RVec3*)(p + 0x60));
    }
    if (NearZero(cb->f2690) && cb->f2618 > 0.15f &&
        NearZero(cb->f2650)) {
        f32 stall = MinF(1.0f, 10.0f * cb->f264C);
        f32 pitch = cb->f2600;
        pitch += 0.75f * stall * fn_80129DC4(4, -1.0f, 1.0f);
        f32 roll = cb->f2604;
        HVec fl = *(HVec*)(p + 0x118);
        fl.y = 0.0f;
        f32 m = MaxF(1.0f, PSVECMag((const Vec*)&fl));
        f32 inv = 1.0f / m;
        fl.x = fl.x * inv;
        fl.y = fl.y * inv;
        fl.z = fl.z * inv;
        if (PSVECDotProduct((const Vec*)&fl, (const Vec*)(p + 0x54)) < -0.2f) {
            CbScaleSelf(&fl, -1.0f);
        }
        f32 nose = PSVECDotProduct((const Vec*)&fl, (const Vec*)(p + 0x48));
        f32 wantR = -roll - cb->mtx.rot.x.y * (1.0f - AbsNarrow(cb->mtx.rot.z.y));
        f32 wantP = 0.25f * pitch - nose;
        f32 wp = PSVECDotProduct((const Vec*)(p + 0x124), (const Vec*)(p + 0x3C));
        f32 wr = PSVECDotProduct((const Vec*)(p + 0x124), (const Vec*)(p + 0x54));
        f32 gainP = cb->f2680 ? 1.0f : 0.25f;
        f32 gainR = cb->f2680 ? 1.0f : 0.25f;
        f32 lead = 5.0f * LerpT(Clamp01((cb->f2618 - 0.15f) / 0.35f), 0.0f, 0.5f);
        if (!cb->f2680) {
            lead = 0.0f;
        }
        f32 tp = gainP * (cb->inertia.x * ((wantP - wp) + lead * (pitch - nose))) / 0.016683351f;
        RVec3 t;
        t.x = cb->mtx.rot.x.x * tp;
        t.y = cb->mtx.rot.x.y * tp;
        t.z = cb->mtx.rot.x.z * tp;
        f32 tr = gainR * (cb->inertia.z * (wantR - wr)) / 0.016683351f;
        t.x = t.x + cb->mtx.rot.z.x * tr;
        t.y = t.y + cb->mtx.rot.z.y * tr;
        t.z = t.z + cb->mtx.rot.z.z * tr;
        ((RigidBody*)p)->AddTorque((const RVec3*)&t);
    }
    if (cb->f2648 == 7) {
        RVec3 side;
        UpCross(&side, (const Vec*)(p + 0x54));
        side.y = 0.0f;
        f32 k = 0.5f * (-PSVECDotProduct((const Vec*)(p + 0x118), (const Vec*)&side) * cb->mass);
        RVec3 f;
        f.x = side.x * k;
        f.y = side.y * k;
        f.z = side.z * k;
        ((RigidBody*)p)->AddForceAt((const RVec3*)&f, (const RVec3*)(p + 0x60));
    }
    f32 stalled = cb->f264C;
    cb->f264C = 0.0f;
    if (cb->f2648 == 7) {
        if (!cb->f2624 && cb->f260D && cb->f2598 < 0.15f && cb->f2618 >= -0.1f &&
            cb->f2618 <= 0.25f && cb->speed > 20.0f) {
            cb->f2624 = 1;
        }
        if (cb->f2624 && cb->f2618 >= 0.3f && (cb->f2624 = 0, cb->vel.y > 2.2222223f) &&
            (f32)__fabs(cb->f2588) <= __float_epsilon[0] && NearZero(cb->f2690)) {
            cb->f2625 = 1;
            s32 id = cb->index;
            fn_801796EC(13, &id, 4);
            ((CbFx*)((CarBody*)p)->f2584)->v28();
            fn_8014E61C(cb->index, 6, 0.0f);
            fn_80161A38(1, cb->index, 0);
            KickUp(p, cb->mass);
        }
        f32 top = 0.44444445f * fn_80181A48(p);
        cb->f261C = MinF(cb->f261C, 0.9f * top);
        f32 flat = sqrt(Sq(cb->vel.x) + Sq(cb->vel.z));
        int nosedive = 0;
        f32 dive = -cb->f2600;
        if (lbl_8055A3AC && U32(p, 0x10) == 'PCAR' && dive > 0.0f && dive > 0.9f) {
            cb->f264C = MinF(0.016683351f + stalled, 1.0f);
            dive = LerpT(Ramp(cb->f264C, 0.5f, 1.0f), 0.9f, -0.5f);
            nosedive = 1;
        }
        f32 lean = MinF(1.0f, 6.0f * dive);
        lean = lean > -1.0f ? lean : -1.0f;
        if (cb->f2618 > 1.0f) {
            CbLeanEvent ev;
            ev.id = cb->index;
            ev.lean = lean;
            ev.dive = nosedive;
            fn_801796CC(15, &ev, sizeof(ev));
        }
        f32 target;
        if (lean < 0.0f) {
            target = LerpT(Ramp(lean, 0.0f, -0.35f), cb->f261C, 0.9f * cb->f261C);
            airLift = airLift + lbl_8055A3B0 * -lean;
        } else {
            target = LerpT(Ramp(lean, 0.0f, 0.35f), cb->f261C, MinF(1.1f * cb->f261C, top));
        }
        f32 want = target - 8.888889f * cb->f25E4;
        HVec dir = *(HVec*)(p + 0x118);
        dir.y = 0.0f;
        f32 m = MaxF(0.01f, PSVECMag((const Vec*)&dir));
        f32 one = 1.0f;
        f32 inv = one / m;
        dir.x = dir.x * inv;
        dir.y = dir.y * inv;
        dir.z = dir.z * inv;
        if (want < flat) {
            one = one + 4.0f * cb->f25E4;
        }
        f32 k = one * ((want - flat) * cb->mass);
        RVec3 f;
        f.x = dir.x * k;
        f.y = dir.y * k;
        f.z = dir.z * k;
        ((RigidBody*)p)->AddForceAt((const RVec3*)&f, (const RVec3*)(p + 0x60));
    }
    if (U32(p, 0x10) == 'PCAR') {
        f32 x = cb->mtx.pos.x;
        f32 z = cb->mtx.pos.z;
        f32 floor = MaxF(0.0f, fn_801DDC8C(p + 0x6C, cb->mtx.pos.x, cb->mtx.pos.z));
        cb->f2688 = MaxF(0.0f, cb->f2688 - 0.033366702f);
        int high = floor > 6.6666665f;
        if (high) {
            RVec3 f;
            if (cb->mtx.pos.y - 0.915f < fn_801DCF48(&f, x, z) && cb->vel.y < floor) {
                f32 push = 24.525002f + (floor - cb->vel.y);
                f.y = push * cb->mass / 0.016683351f;
                f.x = 0.0f;
                f.z = 0.0f;
                ((RigidBody*)p)->AddForceAt((const RVec3*)&f, (const RVec3*)(p + 0x60));
                fn_802E30C8("%d: terrain morph pants-kick: %.2f MPH, terr %.2f MPH", lbl_8055E3C4, push / 0.44444445f, floor / 0.44444445f);
                if ((f32)__fabs(cb->f2688) <= __float_epsilon[0]) {
                    fn_8014E61C(cb->index, 8, 0.0f);
                }
                cb->f2688 = 1.0f;
            }
            if (!((f32)__fabs(cb->f2688) <= __float_epsilon[0])) {
                cb->f2688 = 1.0f;
            }
        } else if (!((f32)__fabs(cb->f2688) <= __float_epsilon[0])) {
            if (cb->vel.y > 0.8888889f) {
                cb->f2688 = 1.0f;
            }
            if (cb->airborne == 4 || !NearZero(cb->f2690)) {
                cb->f2688 = 0.0f;
            }
        }
    }
    if (cb->f258C > __float_epsilon[0] && !cb->f259D && cb->f2618 < 0.1f &&
        cb->speed > 4.4444447f) {
        s32 id = cb->index;
        fn_801796CC(14, &id, 4);
    }
    cb->f2588 = MaxF(0.0f, cb->f2588 - 0.016683351f);
    cb->f2590 = MaxF(0.0f, cb->f2590 - 0.016683351f);
    cb->f258C = MaxF(-3.0f, cb->f258C - 0.016683351f);
    if (cb->f25BC > __float_epsilon[0]) {
        cb->f258C = MaxF(cb->f258C, cb->f25BC);
        cb->f25BC = MaxF(cb->f25BC - 0.016683351f, 0.0f);
        if (!fn_8014E708(cb->index)) {
            s32 id = cb->index;
            fn_801796CC(16, &id, 4);
        }
    }
    if (cb->airborne > 0 && cb->f2618 > 0.75f && lbl_8055E4F4 && cb->f1F0 == 0x50) {
        fn_801DF074(5, 1);
    }
    if (cb->airborne == 0) {
        cb->f2618 += 0.016683351f;
    } else {
        cb->f2618 = 0.0f;
        sqrt(Sq(cb->vel.x) + Sq(cb->vel.z));
        cb->f261C = cb->speed;
    }
    int sunk = 0;
    if (fn_802BF4F0()) {
        sunk = 1;
        for (int i = 0; i < 4; i++) {
            CbWheel c = cb->wheels[i];
            RVec3 pt;
            ObRotateRows(&pt, (const f32*)(p + 0x3C), c.local);
            pt.x = pt.x + cb->mtx.pos.x;
            pt.y = pt.y + cb->mtx.pos.y;
            pt.z = pt.z + cb->mtx.pos.z;
            if (pt.y > fn_802BF590()) {
                sunk = 0;
                break;
            }
        }
    }
    if (sunk) {
        cb->f2588 = 0.0f;
    }
    int deep = 0;
    if (fn_802BF4F0() && cb->mtx.pos.y < 0.25f + fn_802BF590()) {
        deep = 1;
    }
    if (cb->f25C4 > 0.01f || (cb->airborne > 0 && fn_802B04D4(p + 0x60) && deep)) {
        if ((f32)__fabs(cb->f25C4) <= __float_epsilon[0]) {
            ((CbFx*)((CarBody*)p)->f2584)->v30();
            if (NearZero(cb->f2690)) {
                fn_8018E330(p);
            }
        }
        f32 mass7 = cb->mass;
        f32 k = Mul(-2.0f, mass7);
        RVec3 f;
        cb->f25C4 = CbAdd(cb->f25C4, 0.016683351f);
        f.x = cb->vel.x * k;
        f.y = cb->vel.y * k;
        f.z = cb->vel.z * k;
        ((RigidBody*)p)->AddForceAt((const RVec3*)&f, (const RVec3*)(p + 0x60));
        f32 surf = 0.35f + fn_802BF590();
        f32 grow = LerpT(RampW(cb->f25C4, 0.7f, 0.8f), 1.0f, 0.2f);
        RVec3 front;
        CbAddScaled(&front, &cb->mtx.pos, &cb->mtx.rot.z, 2.0f);
        f32 df = Ramp(front.y, surf, surf - 0.4f);
        f32 lf = LerpT(df, 0.0f, 1.0f);
        f32 tf = RampW(cb->f25C4, 0.0f, 0.3f);
        f32 mass8 = cb->mass;
        RVec3 ff(0.0f, lf * (LerpT(tf, 0.2f, 1.0f) * (0.5f * grow)) * mass8 / 0.016683351f, 0.0f);
        ((RigidBody*)p)->AddForceAt((const RVec3*)&ff, (const RVec3*)&front);
        RVec3 back;
        CbAddScaled(&back, &cb->mtx.pos, &cb->mtx.rot.z, -2.0f);
        f32 db = Ramp(back.y, surf, surf - 0.4f);
        f32 lb = LerpT(db, 0.0f, 1.0f);
        f32 tb = Clamp01((cb->f25C4 - 0.2f) / 0.6f);
        f32 mass9 = cb->mass;
        RVec3 fb(0.0f, lb * (LerpT(tb, 0.1f, 1.0f) * (0.3f * grow)) * mass9 / 0.016683351f, 0.0f);
        ((RigidBody*)p)->AddForceAt((const RVec3*)&fb, (const RVec3*)&back);
    } else {
        cb->f25C4 = 0.0f;
    }
    f32 cool;
    if (cb->f2618 < 0.4f) {
        if (sunk) {
            cool = heatTune.v[2];
        } else {
            cool = heatTune.v[0];
        }
    } else {
        cool = heatTune.v[1];
    }
    f32 heat = MaxF(0.0f, cb->f25A0 - 0.016683351f * cool);
    cb->f25A0 = heat;
    cb->f259C = 0;
    if (lbl_8055E4F4) {
        if (!NearZero(cb->f2690) && U32(p, 0x10) == 'PCAR') {
            cb->f25A0 = MinF(cb->f25A0, heatTune.v[7]);
        }
        if (cb->f258C < 0.1f && cb->f260D && cb->f2618 < 0.2f &&
            cb->f25A0 < 1.001f && cb->f2588 < 0.001f &&
            NearZero(cb->f2690)) {
            cb->f25A0 = MinF(1.0f, cb->f25A0 + 0.016683351f * (cb->setupA.f6C * (heatTune.v[3] * cb->f2D4)));
            cb->f259C = 1;
        }
        if (cb->airborne >= 3) {
            int boosting = 1;
            if (!CarHot(p) && !cb->f259C) {
                boosting = 0;
            }
            if (!boosting || !CarOnGas(p)) {
                if (cb->speed > cb->f5A8) {
                    f32 over = Ramp(cb->speed, cb->f5A8, 2.2222223f + cb->f5A8);
                    f32 mass10 = cb->mass;
                    f32 k = heatTune.v[12] * (9.81f * (-LerpT(over, 0.0f, 1.0f) * mass10)) / cb->speed;
                    RVec3 f;
                    f.x = cb->vel.x * k;
                    f.y = cb->vel.y * k;
                    f.z = cb->vel.z * k;
                    if (U32(p, 0x10) == 'PCAR') {
                        ((RigidBody*)p)->AddForceAt((const RVec3*)&f, (const RVec3*)(p + 0x60));
                    }
                }
            }
        }
        if (cb->f25A0 > 0.99f && cb->f2588 < 0.01f) {
            cb->f25A0 = heatTune.v[6];
            cb->f2588 = heatTune.v[8];
            fn_801DF074(6, 1);
            ((CbFx*)((CarBody*)p)->f2584)->v14();
        }
    }
    f32 vol = fn_801B8950(cb->index);
    int loud = 0;
    if (CarOnGas(p) && CarHot(p)) {
        loud = 1;
    }
    f32 want = loud ? 1.0f : 0.0f;
    f32 dvc = MaxF(MinF(0.033366702f, want - vol), -0.033366702f);
    fn_801B8964(cb->index, vol + dvc);
    int trick = 0;
    if (NearZero(cb->f2690) && NearZero(cb->f2650)) {
        f32 yaw = 0.33333334f * (cb->f25D8 * (cb->f25F0 * cb->f5B0));
        f32 sp = PSVECMag((const Vec*)(p + 0x118));
        f32 m = MaxF(sp, 1.0f);
        RVec3 t;
        RVec3 dir;
        f32 inv = 1.0f / m;
        dir.x = cb->vel.x * inv;
        dir.y = cb->vel.y * inv;
        dir.z = cb->vel.z * inv;
        if (cb->f2680) {
            RVec3 d2;
            d2.y = 0.25f * dir.y;
            d2.x = dir.x;
            d2.z = dir.z;
            RVec3 side;
            PSVECCrossProduct((const Vec*)&d2, (const Vec*)(p + 0x54), (Vec*)&side);
            f32 g = (s32)fn_8018B06C(p) < 0 ? 0.0f : 1.0f;
            f32 st = cb->f25F0 * g;
            f32 lat = PSVECDotProduct((const Vec*)&side, (const Vec*)(p + 0x48));
            f32 want = 3.0f * (20.0f * (0.6f * st - lat));
            f32 t = RampW(cb->speed, 22.222223f, 48.888893f);
            yaw = LerpT(t, yaw, want);
        }
        f32 now = lbl_8055E3C8;
        if (cb->f2638 == 0 && cb->f2644 > 0 &&
            (now - cb->f2634 > 0.6f || cb->f2618 < 0.1f)) {
            fn_8014E61C(cb->index, 7, (f32)cb->f2644);
            cb->f2644 = 0;
            trick = 1;
        }
        if (cb->f2618 < 0.5f) {
            cb->f263C = 0;
            if (cb->f2638) {
                cb->f2638 = 0;
                if (cb->f2644 > 1) {
                    fn_8014E61C(cb->index, 7, (f32)(cb->f2644 - 1));
                    trick = 1;
                }
            }
            cb->f2644 = 0;
        }
        if (cb->f2638) {
            CbSpinEvent ev;
            ev.id = cb->index;
            ev.left = cb->f2638 == 1;
            fn_801796CC(17, &ev, sizeof(ev));
        }
        if (lbl_8055A3B4 && U32(p, 0x10) == 'PCAR' && cb->f2618 > 0.5f && cb->speed > 13.333333f &&
            !cb->f2638) {
            int held = 0;
            if (cb->f25E4 > 0.2f && cb->f260D) {
                held = 1;
            }
            if (cb->f25F0 < -0.75f) {
                cb->f2628 = now;
            }
            if (cb->f25F0 > 0.75f) {
                cb->f262C = now;
            }
            if (!held) {
                cb->f2630 = now;
            }
            if (cb->f2644 == 0 && !held) {
                cb->f2628 = -1000.0f;
                cb->f262C = -1000.0f;
            }
            f32 l = cb->f2628;
            f32 r = cb->f262C;
            f32 gap = AbsNarrow(l - r);
            r = MinF(l, r);
            int side = cb->f2628 > cb->f262C ? 1 : 2;
            if (gap < 0.6f && now - r < 1.0f && held &&
                (cb->f263C == 0 || side == cb->f263C)) {
                cb->f2638 = side;
                cb->f263C = side;
                cb->f2640 = 0;
                f32 spin = PSVECDotProduct((const Vec*)(p + 0x124), (const Vec*)(p + 0x48));
                f32 sgn = cb->f2638 == 1 ? -1.0f : 1.0f;
                if (sgn * spin < -0.1f && cb->f2644 == 0) {
                    cb->f2638 = 0;
                } else {
                    cb->f2628 = now;
                    cb->f262C = 0.0f;
                    cb->f2644 = cb->f2644 + 1;
                }
            }
        } else {
            if (cb->f2638) {
                f32 sgn = cb->f2638 == 1 ? -1.0f : 1.0f;
                yaw = 62.831856f * sgn;
                PlainRVec3 fwd = cb->mtx.rot.z;
                fwd.y = 0.0f;
                PlainRVec3 v = dir;
                v.y = 0.0f;
                int behind = PSVECDotProduct((const Vec*)&fwd, (const Vec*)&v) < 0.0f;
                if (behind) {
                    cb->f2640 = 1;
                }
                if (!behind && cb->f2640) {
                    cb->f2638 = 0;
                    cb->f2628 = 0.0f;
                    cb->f262C = 0.0f;
                    cb->f2634 = now;
                }
            }
        }
        f32 spin = PSVECDotProduct((const Vec*)(p + 0x124), (const Vec*)(p + 0x48));
        f32 sk = LerpT(RampW(cb->speed, 6.6666665f, 24.444445f), 1.0f, 0.1f);
        f32 e = cb->f5E4;
        f32 m2 = Mul(30000.0f, Sq(e));
        f32 lt = LerpT(RampW(cb->f2618, 0.1f, 0.20000002f), 1.0f, 0.2f);
        f32 k = m2 * lt * (yaw * sk - spin);
        t.x = cb->mtx.rot.y.x * k;
        t.y = cb->mtx.rot.y.y * k;
        t.z = cb->mtx.rot.y.z * k;
        ((RigidBody*)p)->AddTorque((const RVec3*)&t);
    }
    int spins = cb->f2644 - (cb->f2638 != 0);
    if (spins > 0 && !trick) {
        fn_8014E61C(cb->index, 9, (f32)spins);
    }
    if (cb->airborne < 3) {
        f32 grav = cb->f5DC;
        f32 gravUp = cb->f5E0;
        if (cb->airborne == 0) {
            f32 pitch = cb->f2600;
            if (pitch < -0.01f) {
                f32 ap = AbsNarrow(pitch);
                grav = LerpT(ap, grav, 0.5f * grav);
                gravUp = LerpT(ap, gravUp, 0.5f * gravUp);
            } else if (pitch > 0.01f) {
                f32 ap = AbsNarrow(pitch);
                grav = LerpT(ap, grav, 0.5f + grav);
                gravUp = LerpT(ap, gravUp, 0.5f + gravUp);
            }
        }
        if (!cb->f2625 && cb->f2618 >= 0.3f) {
            f32 s = LerpT(cb->f5F0, 1.0f, lbl_8055E4D8);
            grav = grav * s;
            gravUp = gravUp * s;
        }
        airLift = airLift + (cb->vel.y > 0.0f ? grav : gravUp);
        LiftUp(p, airLift * (9.81f * -cb->mass));
        cb->f2684 = 9.81f * (1.0f + airLift);
    }
    cb->f26D0 = fn_801DCEC0(*((CollVolume*)p)->hint, cb->mtx.pos.x, cb->mtx.pos.z);
    cb->f26CC = LerpT(0.1f, cb->f26CC, cb->f26D0);
    if (cb->speed < 0.5f && cb->f25F4 < 0.1f && cb->f25E4 > 0.9f) {
        cb->f26A8 += 0.016683351f;
    } else {
        cb->f26A8 = 0.0f;
    }
    u32 stuck = cb->f26A8 > 2.0f;
    if (stuck != cb->f26AC) {
        cb->f26AC = stuck;
        fn_801BF1B8(PTR(p, 0x1EC), stuck);
    }
    if (cb->f26E0) {
        RVec3 a;
        CbSpring3(&a, (RVec3*)&cb->f26E4, -30.0f, (RVec3*)&cb->f26F0, -3.0f);
        if (cb->airborne > 0) {
            f32 s = cb->f2604;
            cb->f26FC = cb->f2600;
            a.x = a.x + 2.0f * s;
        }
        a.y = a.y + 10.0f * cb->f26FC * (1.0f - cb->f25F0 * cb->f25F0);
        RVec3 acc;
        ObUnrotateRows(&acc, (const f32*)(p + 0x3C), (const f32*)(p + 0x16C));
        CbAddScaled(&a, &a, &acc, -0.02f);
        CbAddScaled((RVec3*)&cb->f26F0, (RVec3*)&cb->f26F0, &a, 0.016683351f);
        CbAddScaled((RVec3*)&cb->f26E4, (RVec3*)&cb->f26E4, (RVec3*)&cb->f26F0, 0.016683351f);
        if (cb->f26FC > 0.3f) {
            f32 k = 2.0f * (9.81f * (cb->f26F4 * cb->mass));
            RVec3 f;
            f.x = cb->mtx.rot.y.x * k;
            f.y = cb->mtx.rot.y.y * k;
            f.z = cb->mtx.rot.y.z * k;
            ((RigidBody*)p)->AddForceAt((const RVec3*)&f, (const RVec3*)(p + 0x60));
        }
    }
}

u32 DeityCar_vf58(u8* p) { return 0; }

}  // extern "C"

void CarBody::vf5C(void) {
    u8* p = (u8*)this; fn_8018A8A8(p); }


extern "C" {

// One physics step for every live car: run each body's pre-step (fn_80183E7C), feed contacts, accumulated forces
// and the body state into the solver records, step the solver, then per car apply drag, boost, lift and
// air-control forces, read the solved pose and velocities back, rebuild the four wheels' contact state, and work
// out the throttle/brake/steer limits (water, offroad, airborne, boost) before the drive model update.
// MATCHING-AID: invented helpers (the target materialises these compares as values, mfcr); unproven.

void fn_80186DE8(void) {
    CbSolMass* mass;
    CbSolContact* contact;
    CbSolCross* cross;
    CbSolCx* cx;
    s32 n;
    s32 i;
    fn_80190E6C();
    if (fn_801C5B60() == 0) {
        fn_80196A9C();
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
        ((CarBody*)car)->b1E9 = 1;
        if (((CarBody*)car)->mtx.rot.y.y < 0.0f) {
            Corner* c;
            for (c = ((RigidBody*)car)->contacts; c; c = c->next) {
                c->Hit((RigidBody*)car);
            }
        }
        RVec3 f;
        RVec3 t;
        ((RigidBody*)car)->Flush((RVec3*)&f, (RVec3*)&t);
        if (((CarBody*)car)->f26AC) {
            f.x = 0.0f;
            f.y = 0.0f;
            f.z = 0.0f;
            t.x = 0.0f;
            t.y = 0.0f;
            t.z = 0.0f;
        }
        RVec3 F(m->force.x, m->force.y, m->force.z);
        F.x += f.x;
        F.y += f.y;
        F.z += f.z;
        RVec3 T(m->torque.x + t.x, m->torque.y + t.y, m->torque.z + t.z);
        m->force = CbSolVec4(F.x, F.y, F.z);
        m->torque = CbSolVec4(T.x, T.y, T.z);
        if (PSVECDotProduct((const Vec*)(car + 0x124), (const Vec*)(car + 0x124)) > 100.0f) {
            PSVECNormalize((const Vec*)(car + 0x124), (Vec*)(car + 0x124));
            CbScaleTo(&((CarBody*)car)->avel.x, &((CarBody*)car)->avel.x, 10.0f);
        }
        m->pos = CbSolVec4(((CarBody*)car)->mtx.pos.x, ((CarBody*)car)->mtx.pos.y, ((CarBody*)car)->mtx.pos.z);
        m->vel = CbSolVec4(((CarBody*)car)->vel.x, ((CarBody*)car)->vel.y, ((CarBody*)car)->vel.z);
        m->avel = CbSolVec4(((CarBody*)car)->avel.x, ((CarBody*)car)->avel.y, ((CarBody*)car)->avel.z);
    }
    fn_801BEDD4();
    for (i = 0; i < n; i++) {
        CbSolMass* m;
        f32 steer;
        f32 lim;
        CbVec4 a;
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
        f32 hold;
        u8 player;
        f32 slide;
        int surf;
        CbSolCx* e;
        f32 s6;
        int landing;
        int wet2;
        f32 cut;
        e = &cx[i];
        CbVec4 grip4 = CbVec4(x->vB0);
        air = 1.0f - ((CarBody*)car)->speed;
        if (1.0f < air) {
            air = 1.0f;
        }
        if (air > 0.0f) {
            air = air;
        } else {
            air = 0.0f;
        }
        f32 ds;
        f32 grip;
        airK = 1.0f + 3.0f * air;
        surf = 0;
        for (int k = 0; k < 4; k++) {
            if (e->surface[k] > surf) {
                surf = e->surface[k];
            }
        }
        f32 wet;
        f32 hb;
        st = fn_801D04A8(surf);
        f32 bk;
        if (!NearZero(((CarBody*)car)->f2650)) {
            f32 d = (4.0f - ((CarBody*)car)->f2650 - 0.25f) / 0.25f;
            if (1.0f < d) {
                d = 1.0f;
            }
            if (d > 0.0f) {
                d = d;
            } else {
                d = 0.0f;
            }
            wet = LerpT(d, 0.1f, 1.0f);
        } else {
            wet = 1.0f;
        }
        f32 slip;
        grip = st->grip * wet;
        slip = st->slip * wet;
        f32 gas;
        f32 steerK;
        landing = 0;
        if (lbl_8055E3C8 - ((CarBody*)car)->f26DC < 0.5f && ((CarBody*)car)->speed < 8.888889f) {
            landing = 1;
        }
        if (((CarBody*)car)->airborne >= 3 && !landing) {
            f32 k;
            f32 drag;
            f32 speed = PSVECMag((const Vec*)(car + 0x118));
            speed = speed > 1.0f ? speed : 1.0f;
            drag = 9.81f * (-st->drag * ((CarBody*)car)->mass) / speed;
            f32 force[3];
            force[0] = ((CarBody*)car)->vel.x * drag;
            force[1] = ((CarBody*)car)->vel.y * drag;
            force[2] = ((CarBody*)car)->vel.z * drag;
            fn_801769CC();
            f32 top = 0.44444445f * fn_80181A48(car);
            if (CarHot(car) && ((CarBody*)car)->speed < top && ((CarBody*)car)->f25E4 < 0.1f &&
                (s32)fn_8018B06C(car) > 0) {
                f32 k = 9.81f * fn_80181A14(car) * ((CarBody*)car)->f25E0 * ((CarBody*)car)->mass;
                AddScaled(force, 0x54 + car, k);
            }
            if (((CarBody*)car)->f5EC > 0.01f && (s32)fn_8018B06C(car) > 0 && ((CarBody*)car)->f25E4 < 0.1f &&
                ((CarBody*)car)->f25E0 > 0.5f) {
                f32 k = 9.81f * (lbl_8055E4E8[1] * (((CarBody*)car)->f25E0 * ((CarBody*)car)->mass));
                AddScaled(force, car + 0x54, k);
            }
            if (((CarBody*)car)->f258C > -2.0f || ((CarBody*)car)->f25E4 > 0.25f || ((CarBody*)car)->f5EC > 0.01f) {
                f32 fwd = PSVECDotProduct((const Vec*)(car + 0x3C), (const Vec*)(car + 0x118));
                f32 b = MaxF(2.0f + ((CarBody*)car)->f258C, ((CarBody*)car)->f25E4);
                f32 k = 0.55181247f * (-fwd * ((CarBody*)car)->mass) * Clamp01(b);
                AddScaled(force, car + 0x3C, k);
            }
            speed = PSVECMag((const Vec*)(car + 0x118));
            speed = speed > 1.0f ? speed : 1.0f;
            k = 9.81f * (-1.5f * ((CarBody*)car)->mass) * ((CarBody*)car)->f25E4 / speed;
            force[0] = force[0] + ((CarBody*)car)->vel.x * k;
            force[1] = force[1] + ((CarBody*)car)->vel.y * k;
            force[2] = force[2] + ((CarBody*)car)->vel.z * k;
            if (((CarBody*)car)->f259C && ((CarBody*)car)->speed < top) {
                f32 t = (((CarBody*)car)->speed - 26.666666f) / 35.555557f;
                f32 boost = (heatTune.v[5] * ((CarBody*)car)->f2D8 * ((CarBody*)car)->setupA.f70) * 9.81f;
                f32 lo = 2.4f * boost;
                k = (LerpT(Clamp01(t), lo, 0.6f * boost)) * ((CarBody*)car)->f25E0 * ((CarBody*)car)->mass;
                AddScaled(force, car + 0x54, k);
            }
            ((RigidBody*)car)->AddForceAt((const RVec3*)force, (const RVec3*)(car + 0x60));
        }
        CbVec4 v = CbVec4(x->pts[6]);
        f32 brake;
        s6 = v.w;
        if (((CarBody*)car)->f2618 > 0.25f) {
            s6 = s6 * heatTune.v[4];
        }
        v.x = s6;
        x->pts[6] = v.Sol();
        grip4.x = grip4.x * grip;
        grip4.y = grip4.y * grip;
        grip4.z = grip4.z * (airK * slip);
        grip4.w = grip4.w * (slip * airK);
        x->vA0 = grip4.Sol();
        wh = ((CarBody*)car)->f26B0;
        if (wh >= 0) {
            f32 a;
            f32 b;
            a = F32(car, wh * 0x74 + 0x23B4);
            f32 s1[4] = {0};
            f32 s0[4] = {0};
            s1[wh] = (a - 6.2831855f) * (a - 3.1415927f) * (0.09829f * a) * 0.08726646f;
            b = fmod(1.5707964f + a, 6.2831854820251465);
            x->v130 = CbVec4(s1[0], s1[1], s1[2], s1[3]).Sol();
            s0[wh] = -((b - 6.2831855f) * (b - 3.1415927f) * (0.09829f * b)) * 0.08726646f;
            x->v140 = CbVec4(s0[0], s0[1], s0[2], s0[3]).Sol();
        }
        player = 0;
        if (U32(car, 0x10) == 'PCAR' && ((*(CbFlagsV**)(car + 0x270C))->Flags() & 0x20)) {
            player = 1;
        }
        RMtx fr;
        fr.pos.Set(m->pos.x, m->pos.y, m->pos.z);
        fr.rot.x.Set(m->rot[0].x, m->rot[0].y, m->rot[0].z);
        fr.rot.y.Set(m->rot[1].x, m->rot[1].y, m->rot[1].z);
        fr.rot.z.Set(m->rot[2].x, m->rot[2].y, m->rot[2].z);
        *(RMtx*)(car + 0x3C) = fr;
        ((CollVolume*)car)->Transform();
        *((CollVolume*)car)->hint = e->id;
        HVec o = *(HVec*)(car + 0x16C);
        RVec3 vel(m->vel.x, m->vel.y, m->vel.z);
        RVec3 avel(m->avel.x, m->avel.y, m->avel.z);
        f32 throttle;
        int boosting;
        f32 vmax;
        f32 out;
        ((CarBody*)car)->accel.x = 59.94f * vel.x + ((CarBody*)car)->vel.x * -59.94f;
        ((CarBody*)car)->accel.y = vel.y * 59.94f + ((CarBody*)car)->vel.y * -59.94f;
        ((CarBody*)car)->accel.z = vel.z * 59.94f + ((CarBody*)car)->vel.z * -59.94f;
        ((CarBody*)car)->aaccel.x = avel.x * 59.94f + ((CarBody*)car)->avel.x * -59.94f;
        ((CarBody*)car)->aaccel.y = avel.y * 59.94f + ((CarBody*)car)->avel.y * -59.94f;
        ((CarBody*)car)->aaccel.z = avel.z * 59.94f + ((CarBody*)car)->avel.z * -59.94f;
        ((CarBody*)car)->vel.x = vel.x;
        ((CarBody*)car)->vel.y = vel.y;
        ((CarBody*)car)->vel.z = vel.z;
        ((CarBody*)car)->avel.x = avel.x;
        ((CarBody*)car)->avel.y = avel.y;
        ((CarBody*)car)->avel.z = avel.z;
        ((CarBody*)car)->push.x = ((CarBody*)car)->accel.x - o.x;
        ((CarBody*)car)->push.y = ((CarBody*)car)->accel.y - o.y;
        ((CarBody*)car)->push.z = ((CarBody*)car)->accel.z - o.z;
        CbVec4 w0(c->v30);
        CbVec4 w1(c->v10);
        CbVec4 w2(c->v0);
        CbVec4 w4(x->v120);
        CbVec4 w3(x->v130);
        CbVec4 w5(x->v140);
        CbVec4 ctl(c->v190);  // never read: retail copies c->v190 here too (dead stores in its frame); name unproven
        CbVec4 rpm(c->pts[4]);
        CbVec4 w6(x->v70);
        CbWheel* w;
        ((CarBody*)car)->f25D0 = rpm.x / 0.10471976f;
        ((CarBody*)car)->speed = PSVECMag((const Vec*)(0x118 + car));
        ((CarBody*)car)->f25D8 = PSVECDotProduct((const Vec*)(car + 0x118), (const Vec*)(car + 0x54));
        ((CarBody*)car)->f25CC = rpm.y;
        steer = ((CarBody*)car)->f25F0;
        f32* water;
        w = (CbWheel*)(car + 0x23AC);
        grounded = 0;
        for (int j = 0; j < 4; j++, w = (CbWheel*)((u8*)w + 0x74)) {
            f32 load = w0[j];
            w->load = load;
            f32 spin = w1[j];
            w->spin = spin;
            w->loadN = load / 0.10471976f;
            w1[j] = fmod(6.2831855f + spin, 6.2831854820251465);
            w->prevSlip = w->slip;
            w->slip = w2[j];
            w->steer = asin(steer * w4[j] + w3[j]);
            w->v18 = w5[j];
            w->contact[0] = c->pts[j].x;
            w->contact[1] = c->pts[j].y;
            f32 nrm[3];
            f32 nrm_2 = c->pts[9 + j].z;
            f32 nrm_1 = c->pts[9 + j].y;
            w->contact[2] = c->pts[j].z;
            nrm[0] = c->pts[9 + j].x;
            nrm[2] = nrm_2;
            nrm[1] = nrm_1;
            w->local[0] = x->pts[j].x;
            w->local[2] = x->pts[j].z;
            w->local[1] = x->pts[j].y - w6[j];
            u32 fl;
            f32 up = PSVECDotProduct((const Vec*)nrm, (const Vec*)(car + 0x54));
            w->plane.nRight = PSVECDotProduct((const Vec*)nrm, (const Vec*)(car + 0x3C));
            w->plane.nFwd = up;
            w->grounded = nrm[1] > 0.01f;
            f32 comp = c->pts[5 + j].w;
            if (comp > 20000.0f) {
                c->pts[5 + j] = CbSolVec4(0.0f, 0.0f, 0.0f);
                CbVec4 half = CbVec4(c->v30);
                half[j] = Mul(half[j], 0.5f);
                c->v30 = half.Sol();
            }
            fl = w->flags & 0xE;
            if (w->grounded) {
                f32 d = comp - 1.4f;
                grounded++;
                w->comp = d > 0.0f ? d : 0.0f;
                int sid = e->surface[j];
                w->groundH = fn_802B03C0(w->plane.ground);
                w->sound = fn_801D04A8(sid)->sound;
            } else {
                w->comp = 0.0f;
            }
            w->flags = fl;
        }
        ((CarBody*)car)->airborne = grounded;
        c->v10 = w1.Sol();
        ds = ((CarBody*)car)->f25DC - ((CarBody*)car)->f25F0;
        lim = ds * ((CarBody*)car)->f25F0 > 0.0f ? 0.12512514f : 0.25025028f;
        if (lim < ds) {
            ds = lim;
        }
        ds = MaxF(ds, -lim);
        ((CarBody*)car)->f25F0 = ((CarBody*)car)->f25F0 + ds;
        water = (f32*)(lbl_8055E918 + i * 0x9C);
        f32* ground = &lbl_8055E908[i].f0;
        throttle = Clamp01(water[1] + ((CarBody*)car)->f25E0);
        if (!(fn_801769CC()->type & 0x4000) && !NearZero(((CarBody*)car)->f2690)) {
            throttle = 0.0f;
            ((CarBody*)car)->f25E4 = 1.0f;
        }
        if (((CarBody*)car)->f2588 > 0.001f) {
            U32(car, 0x268C) = 10;
            if (((CarBody*)car)->speed > 0.44444445f * heatTune.v[11]) {
                ((CarBody*)car)->f25E4 = MaxF(((CarBody*)car)->f25E4, heatTune.v[10]);
                throttle = ((CarBody*)car)->f25E0 < heatTune.v[9] ? ((CarBody*)car)->f25E0 : heatTune.v[9];
            }
        } else if ((fn_801769CC()->type & 0x80000) && ((CarBody*)car)->f25A0 > 0.625f) {
            U32(car, 0x268C) = 6;
        } else {
            U32(car, 0x268C) = 0;
        }
        int down = ((CarBody*)car)->airborne >= 3;
        steerK = ((CarBody*)car)->f25F0;
        if (player) {
            f32 sp;
            f32 cl;
            f32 t;
            sp = ((CarBody*)car)->speed;
            if (sp > 15.555555f) {
                cl = sp;
                cl = 44.444447f < cl ? 44.444447f : cl;
                steerK = steerK * (LerpT(((CarBody*)car)->setupA.f124, 1.0f, 1.2692307f + -0.017307691f * cl));
            }
            if (sp > 0.0f) {
                t = Clamp01((sp / 20.0f) * 0.44444445f);
                steerK = steerK * (1.0f - 0.5f * t) * (1.0f - 0.6f * t);
            }
        }
        slide = (f32)__fabs(((CarBody*)car)->f25CC) - 1.3f * AbsNarrow(((CarBody*)car)->f25D8) - 3.0f;
        if (0.0f > slide) {
            slide = 0.0f;
        }
        cut = 1.0f;
        if (player) {
            cut = 0.0f;
        } else {
            if (((CarBody*)car)->f267C) {
                f32 s = 4.0f * steerK - 0.6f * ((CarBody*)car)->avel.y;
                if (cut < s) {
                    s = cut;
                }
                steerK = MaxF(s, -1.0f);
            }
        }
        gas = throttle;
        if (slide > 0.0f && down) {
            gas = throttle < 1.0f - cut ? throttle : 1.0f - cut;
        }
        brake = ((CarBody*)car)->f25E8;
        f32 rpmT = rpm.x / 0.10471976f;
        throttle = MaxF(gas, 0.0016666667f * (1800.0f - rpmT));
        if (rpmT > ground[12]) {
            throttle = 0.0f;
        }
        hold = ((CarBody*)car)->f25EC;
        bk = Clamp01((rpmT - 1000.0f - 2000.0f * throttle) / 2000.0f);
        brake = bk < brake ? bk : brake;
        if (((CarBody*)car)->speed > 4.4444447f && ((CarBody*)car)->f25CC > 4.4444447f + 1.6f * ((CarBody*)car)->speed) {
            throttle = 0.0f;
        }
        if (!NearZero(((CarBody*)car)->f2650)) {
            throttle = 0.0f;
        }
        if ((f32)!((f32)__fabs(water[6]) <= __float_epsilon[0]) != 0.0f) {  // a bool converted to float
            brake = 0.0f;
        }
        boosting = 1;
        ((CarBody*)car)->f25FC = throttle;
        vmax = ((CarBody*)car)->f5A8;
        if (!CarHot(car) && !((CarBody*)car)->f259C) {
            boosting = 0;
        }
        if (boosting && CarOnGas(car)) {
            vmax = 0.44444445f * fn_80181A48(car) - 4.4444447f;
        }
        vmax = vmax + 0.44444445f * (((CarBody*)car)->f5EC * lbl_8055E4E8[0]);
        off = fn_802B02C8(car + 0x60);
        wet2 = 0;
        if (fn_802BF4F0() && ((CarBody*)car)->wheels[3].plane.ground[1] < fn_802BF590()) {
            wet2 = 1;
        }
        if (wet2) {
            vmax = vmax * lbl_8055A3A8;
        } else if (off) {
            vmax = vmax * lbl_8055A3A4;
        }
        if (((CarBody*)car)->speed > vmax && brake > 0.5f && U32(car, 0x10) == 'PCAR') {
            out = 1.0f - 0.225f * (((CarBody*)car)->speed - vmax);
            if (0.0f > out) {
                out = 0.0f;
            }
            if (throttle < out) {
                out = throttle;
            }
            boosting = 1;
            if (!CarHot(car) && !((CarBody*)car)->f259C) {
                boosting = 0;
            }
            if (!boosting) {
                throttle = LerpT(Clamp01(1.0f + ((CarBody*)car)->f258C), out, throttle);
            } else {
                throttle = out;
            }
        }
        out = throttle;
        ((CarBody*)car)->f25F4 = out;
        ((CarBody*)car)->f25F8 = brake;
        a = CbVec4(x->pts[6]);
        a.w = ((CarBody*)car)->setupA.f20 - ((CarBody*)car)->setupA.f24;
        if (!NearZero(((CarBody*)car)->f2650)) {
            f32 off = 0.0f;
            a.w = a.w * off;
        }
        x->pts[6] = a.Sol();
        rpm.x = rpm.x > 52.35988f ? rpm.x : 52.35988f;
        rpm.w = ground[0];
        c->pts[4] = rpm.Sol();
        out = rpmT < 9500.0f ? out : 0.0f;
        c->v190 = CbSolVec4(steerK, out, brake, water[0]);
        bk2 = ((CarBody*)car)->f25E4;
        hb = 8.0f * hold;
        if (bk2 > hb) {
            hb = bk2;
        }
        c->v1A0 = CbSolVec4(MaxF(0.0f, bk2), MaxF(0.0f, bk2), MaxF(0.0f, hb),
                             MaxF(0.0f, hb));
        fn_8018CB3C(car);
        ((CbStepV*)((u8*&)((CarBody*)car)->src))->Step();
        ((CbTickV*)car)->Tick();
    }
    fn_801BF1B4();
}




// o = a + b * k
static inline void AddScaled(RVec3* o, const RVec3* a, const RVec3* b, f32 k) {
    o->x = a->x + b->x * k;
    o->y = a->y + b->y * k;
    o->z = a->z + b->z * k;
}

// v times the body rotation (rows at 0x3C).
static inline void SwayRotate(RVec3* out, const RVec3* v, const f32* m) {
    if (out == v) {
        f32 x = Dot3(v->x, m[0], v->y, m[1], v->z, m[2]);
        f32 y = Dot3(v->x, m[3], v->y, m[4], v->z, m[5]);
        f32 z = Dot3(v->x, m[6], v->y, m[7], v->z, m[8]);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(v->x, m[0], v->y, m[1], v->z, m[2]);
        out->y = Dot3(v->x, m[3], v->y, m[4], v->z, m[5]);
        out->z = Dot3(v->x, m[6], v->y, m[7], v->z, m[8]);
    }
}

// Per-frame body update: the effect parts, the settle timer, and the body sway, a damped spring
// (velocity 0x2654, angle 0x2660) driven by the filtered body-space acceleration (0x266C); only the
// pitch is kept, held between -2.5 and 15 degrees.
}  // extern "C"

void CarBody::vf20(void) {
    u8* p = (u8*)this;
    RVec3 a;
    RVec3 f;
    ((CollVolume*)p)->CollVolume::vf20();
    if ((CbPart*)((CarBody*)p)->f257C) {
        ((CbPart*)((CarBody*)p)->f257C)->Update(((DeityCar*)((CarBody*)p)->src)->Held());
    }
    if ((CbFx*)((CarBody*)p)->f2580) {
        ((CbFx*)((CarBody*)p)->f2580)->Update();
    }
    if ((CbFx*)((CarBody*)p)->f2584) {
        ((CbFx*)((CarBody*)p)->f2584)->Update();
    }
    if (F32(p, 0x269C) < 0.5f) {
        F32(p, 0x269C) += 0.016683351f;
    } else {
        F32(p, 0x2694) = MinF(F32(p, 0x2690), 0.008341676f + F32(p, 0x2694));
    }
    RVec3* vel = (RVec3*)(p + 0x2654);
    RVec3* ang = (RVec3*)(p + 0x2660);
    RVec3* acc = (RVec3*)(p + 0x266C);
    SwayRotate(&a, (RVec3*)(p + 0x16C), (f32*)(p + 0x3C));
    a.x *= -1.0f;
    a.y *= -1.0f;
    a.z *= -1.0f;
    AddScaled(&f, &a, acc, -1.5f);
    AddScaled(&f, &f, ang, -100.0f);
    AddScaled(&f, &f, vel, -5.0f);
    AddScaled(vel, vel, &f, 0.016683351f);
    AddScaled(ang, ang, vel, 0.016683351f);
    Scale(acc, acc, 0.95f);
    AddScaled(acc, acc, &a, 0.050000012f);
    acc->z = 0.0f;
    acc->y = 0.0f;
    if (ang->z < -0.04363323f) {
        ang->z = -0.04363323f;
        vel->z = MaxF(vel->z, 0.0f);
    }
    ang->z = MinF(0.2617994f, ang->z);
}

extern "C" {

u32 DeityCar_vf7C(u8* p) { return 0; }


extern f32 lbl_8055E460;  // render interpolation factor between the last two physics steps
extern s32 lbl_8055E3C4;  // frame counter
void fn_8018DA5C(u8* p, u8* m);
u32 fn_8018B06C(u8* p);
void fn_8018A9F0(u8* p, u8* m);
double sqrt(double);
static inline void LerpSnapshotMatrix(SnapPose* out, const SnapPose* previous, const SnapPose* current, f32 t) {
    f32* o = (f32*)out;
    const f32* prev = (const f32*)previous;
    const f32* cur = (const f32*)current;
    for (int i = 0; i < 12; i++) {
        *o++ = LerpT(t, *prev++, *cur++);
    }
}

// Fills this car's render snapshot (0xFA8 bytes at buf + 0x8): the body pose interpolated between the last
// two physics steps, state flags and gauges, and the four wheels (interpolated hub positions, suspension and
// spin). If a wheel sinks below its contact height, the whole body is lifted by the deepest one.
}  // extern "C"

void CarBody::vf28(u8* buf) {
    SnapPose tmp;
    u8* m = buf + this->a8;
    memset(m, 0, 0xFA8);
    SnapPose* pose = (SnapPose*)(buf + this->a8);
    if ((f32)__fabs(lbl_8055E460 - 1.0f) <= __float_epsilon[0]) {
        *pose = *(SnapPose*)&this->world;
    } else {
        LerpSnapshotMatrix(&tmp, (const SnapPose*)&this->prev, (const SnapPose*)&this->world, lbl_8055E460);
        *pose = tmp;
    }
    ((CarDrawState*)pose)->hint = *(s16*)&this->hint;
    u16 flags = 0;
    if (((DeityCar*)this->src)->Held()) {
        flags |= 0x40;
    }
    if (!NearZero(this->f2690)) {
        flags |= 0x20;
    }
    ((CarDrawState*)m)->f34 = this->f25D0;
    const f32 speedZ = this->vel.z;
    const f32 zz = speedZ * speedZ;
    const f32& speedX = this->vel.x;
    const f32 xx = speedX * speedX;
    ((CarDrawState*)m)->speedXZ = sqrt(xx + zz);
    ((CarDrawState*)m)->f3C = this->f25F0;
    ((CarDrawState*)m)->f38 = this->f25F4;
    ((CarDrawState*)m)->f40 = this->f25E4;
    ((CarDrawState*)m)->f44 = this->f25F8;
    ((CarDrawState*)m)->f48 = fn_8018B06C(((u8*)this));
    ((CarDrawState*)m)->f50 = 100.0f;
    if (lbl_8055E4F5 && this->nAdded == 0) {
        ((CarDrawState*)m)->blink = (lbl_8055E3C4 & 4) ? 0.0f : 1.0f;
    } else {
        ((CarDrawState*)m)->blink = 1.0f;
    }
    ((CarDrawState*)m)->flags = flags;
    ((CarDrawState*)m)->vel[0] = this->vel.x;
    ((CarDrawState*)m)->vel[1] = this->vel.y;
    ((CarDrawState*)m)->vel[2] = this->vel.z;
    ((CarDrawState*)m)->speed = this->speed;
    ((CarDrawState*)m)->overheat = this->f2588;
    ((CarDrawState*)m)->damage[0] = this->f26E0 ? MaxF(this->f2690, 0.01f) : this->f2690;
    ((CarDrawState*)m)->damage[1] = this->f2694;
    ((CarDrawState*)m)->damage[2] = this->f2698;
    ((CarDrawState*)m)->f298 = this->a268C;
    f32 v = this->f25BC;
    if (Positive(v)) {
        ((CarDrawState*)m)->f2D4 = v;
    } else {
        ((CarDrawState*)m)->f2D4 = 0.0f;
    }
    ((CarDrawState*)m)->heat = this->f25A0;
    ((CarDrawState*)m)->f29C = this->f5A0;
    if (this->f25E4 > 0.1f && !(flags & 0x20)) {
        ((u32&)((CarDrawState*)m)->f29C) |= 0x20000000;
    } else {
        ((u32&)((CarDrawState*)m)->f29C) |= 0x10000000;
    }
    ((CarDrawState*)m)->f238 = this->f2660;
    ((CarDrawState*)m)->f23C = this->f2664;
    ((CarDrawState*)m)->f240 = this->f2668;
    f32 sink = 0.0f;
    u8* wheels = (u8*)&((CarDrawState*)m)->wheels;
    u8* w = ((u8*)this) + 0x23AC;
    u8* o = wheels;
    for (int i = 0; i < 4; i++) {
        HVec a = *(HVec*)(w + 0x30);
        const f32 offsetY = a.y - this->offset.y;
        a.x = a.x - this->offset.x;
        a.y = offsetY;
        a.z = a.z - this->offset.z;
        HVec b = a;
        a.y = a.y + ((CbWheel*)w)->slip;
        b.y = b.y + ((CbWheel*)w)->prevSlip;
        HVec c;
        const f32& priorX = b.x;
        const f32 tWheel = lbl_8055E460;
        const f32 wheelZ = LerpT(tWheel, b.z, a.z);
        const f32 wheelY = LerpT(tWheel, b.y, a.y);
        const f32 wheelX = LerpT(tWheel, priorX, a.x);
        c.x = wheelX;
        c.y = wheelY;
        c.z = wheelZ;
        ((CarDrawWheel*)o)->pos[0] = c.x;
        ((CarDrawWheel*)o)->pos[1] = c.y;
        ((CarDrawWheel*)o)->pos[2] = c.z;
        sink = MaxF(sink, c.y - ((CbWheel*)w)->v40);
        ((CarDrawWheel*)o)->ground[0] = ((CbWheel*)w)->plane.ground[0];
        ((CarDrawWheel*)o)->ground[1] = ((CbWheel*)w)->plane.ground[1];
        ((CarDrawWheel*)o)->ground[2] = ((CbWheel*)w)->plane.ground[2];
        ((CarDrawWheel*)o)->normal[0] = ((CbWheel*)w)->normal[0];
        ((CarDrawWheel*)o)->normal[1] = ((CbWheel*)w)->normal[1];
        ((CarDrawWheel*)o)->normal[2] = ((CbWheel*)w)->normal[2];
        ((CarDrawWheel*)o)->slip = ((CbWheel*)w)->slip;
        ((CarDrawWheel*)o)->steer = ((CbWheel*)w)->steer;
        ((CarDrawWheel*)o)->v18 = ((CbWheel*)w)->v18;
        ((CarDrawWheel*)o)->load = ((CbWheel*)w)->load;
        ((CarDrawWheel*)o)->spin = ((CbWheel*)w)->spin + 0.016683351f * (((CbWheel*)w)->load * (lbl_8055E460 - 1.0f));
        ((CarDrawWheel*)o)->comp = ((CbWheel*)w)->comp;
        ((CarDrawWheel*)o)->nRight = ((CbWheel*)w)->plane.nRight;
        ((CarDrawWheel*)o)->nFwd = ((CbWheel*)w)->plane.nFwd;
        ((CarDrawWheel*)o)->v10 = ((CbWheel*)w)->v10;
        ((CarDrawWheel*)o)->f48[0] = 0.0f;
        ((CarDrawWheel*)o)->f48[3] = 0.0f;
        ((CarDrawWheel*)o)->f48[2] = 0.0f;
        ((CarDrawWheel*)o)->f48[1] = 0.0f;
        ((CarDrawWheel*)o)->f48[4] = 0.0f;
        ((CarDrawWheel*)o)->f48[5] = 0.0f;
        ((CarDrawWheel*)o)->flags = S32(w, 0x70);
        ((CarDrawWheel*)o)->grounded = ((CbWheel*)w)->grounded;
        ((CarDrawWheel*)o)->groundH = ((CbWheel*)w)->groundH;
        w += 0x74;
        o += 0x6C;
    }
    ((CarDrawState*)m)->lapDist = ((DeityCar*)this->src)->Ideal()->travel;
    ((CarDrawState*)m)->f2CC = this->f26CC;
    ((CarDrawState*)m)->f2D0 = this->f5E4;
    ((CarDrawState*)m)->f2D8 = this->f26D4;
    fn_8018DA5C(((u8*)this), m);
    if (sink > 0.01f) {
        ((CarDrawState*)m)->pose.pos.v[0] = ((CarDrawState*)m)->pose.pos.v[0] + ((CarDrawState*)m)->pose.rot.m[3] * sink;
        ((CarDrawState*)m)->pose.pos.v[1] = ((CarDrawState*)m)->pose.pos.v[1] + ((CarDrawState*)m)->pose.rot.m[4] * sink;
        ((CarDrawState*)m)->pose.pos.v[2] = ((CarDrawState*)m)->pose.pos.v[2] + ((CarDrawState*)m)->pose.rot.m[5] * sink;
        for (int i = 0; i < 4; i++) {
            F32(wheels + i * 0x6C, 0x4) = MaxF(F32(wheels + i * 0x6C, 0x4) - sink, -0.6096f);
        }
        u8* e = (u8*)&((CarDrawState*)m)->points;
        for (int i = 0; i < ((CarDrawState*)m)->nPoints; i++) {
            ((CarDrawPoint*)e)->pos[0] = ((CarDrawPoint*)e)->pos[0] + ((CarDrawState*)m)->pose.rot.m[3] * sink;
            ((CarDrawPoint*)e)->pos[1] = ((CarDrawPoint*)e)->pos[1] + ((CarDrawState*)m)->pose.rot.m[4] * sink;
            ((CarDrawPoint*)e)->pos[2] = ((CarDrawPoint*)e)->pos[2] + ((CarDrawState*)m)->pose.rot.m[5] * sink;
            e += 0x34;
        }
    }
    if (S32(((u8*)this), 0x26C0)) {
        fn_8018A9F0(((u8*)this), m);
    } else {
        ((CarDrawState*)m)->fFA4 = 0;
    }
}

extern "C" {

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


void fn_8018E3EC(u8* p, const HVec* imp, const HVec* at, int a, s32 id, int kind, u8* other, int c);

// Collision response: scales the impulse `imp` at the world point `at` by the mass ratio of the two cars
// for a car-on-car hit (and drops it when this car is the heavier one hit from the front), by the body
// stiffness otherwise, and passes it on in centre-of-mass space when it is large enough.
/* MATCHING-AID: this CarBody member is written on the raw object under its mangled name (extern "C"); as a
   member body, MWCC keeps this+K in an extra register and the code differs. */
void OnImpulse__7CarBodyFPC5RVec3PC5RVec3iPvPv(u8* p, const HVec* imp, const HVec* at, int kind, u8* other, int c) {
    CarBody* cb = (CarBody*)p;
    fn_80189C64(p);
    f32 mag = PSVECMag((const Vec*)imp);
    int carHit = other && kind == 0x6D;
    f32 scale = 1.0f;
    s32 otherId = -1;
    if (kind == 0x6D && other) {
        u32 t = U32(other, 0x10);
        if (t == 'CAR ' || t == 'PCAR' || t == 'ACAR' || t == 'NCAR' || t == 'TRLR') {
            otherId = S32(other, 0x1EC);
            HVec dir = *imp;
            if (mag > 0.1f) {
                PSVECNormalize((const Vec*)&dir, (Vec*)&dir);
            }
            double a_d = __fabs(PSVECDotProduct((const Vec*)&dir, (const Vec*)((u8*)&cb->vel)));
            f32 a = a_d;
            f32 b = __fabs(PSVECDotProduct((const Vec*)&dir, (const Vec*)(other + 0x118)));
            f32 d = b - a;
            if (d > 6.6666665f && b > 1.5f * a) {
                scale = MinF(b / MaxF(a, 0.001f), 5.0f);
            } else if (d < -6.6666665f && a > 1.5f * b) {
                scale = MaxF(b / MaxF(a, 0.001f), 0.2f);
                HVec local;
                local.x = at->x - cb->world.pos.x;
                local.y = at->y - cb->world.pos.y;
                local.z = at->z - cb->world.pos.z;
                f32 x = Dot3(local.x, cb->world.rot.x.x, local.y, cb->world.rot.x.y, local.z, cb->world.rot.x.z);
                f32 y = Dot3(local.x, cb->world.rot.y.x, local.y, cb->world.rot.y.y, local.z, cb->world.rot.y.z);
                f32 z = Dot3(local.x, cb->world.rot.z.x, local.y, cb->world.rot.z.y, local.z, cb->world.rot.z.z);
                local.x = x;
                local.y = y;
                local.z = z;
                if (local.z < -0.508f && PSVECDotProduct((const Vec*)imp, (const Vec*)((u8*)&cb->world.rot.z)) > 0.0f) {
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
    f32 k = cb->f5F4;
    if (carHit) {
        k *= scale;
    } else if (kind != 0x70) {
        k *= 0.4f * cb->f318;
    } else {
        k *= 7.0f;
    }
    if (!(fn_801769CC()->type & 0x4000)) {
        k *= 10.0f;
    }
    hit.x *= k;
    hit.y *= k;
    hit.z *= k;
    if (mag * k > 741.6667f) {
        const HRot* rot = (const HRot*)&cb->mtx;
        const HVec* org = (const HVec*)&cb->mtx.pos;
        const HVec* com = (const HVec*)&cb->offset;
        HFrame fr(*rot);
        if (&fr.pos == com) {
            f32 x = Dot3(com->x, rot->rows0.x, com->y, rot->rows1.x, com->z, rot->rows2.x);
            f32 y = Dot3(com->x, rot->rows0.y, com->y, rot->rows1.y, com->z, rot->rows2.y);
            f32 z = Dot3(com->x, rot->rows0.z, com->y, rot->rows1.z, com->z, rot->rows2.z);
            fr.pos.x = x;
            fr.pos.y = y;
            fr.pos.z = z;
        } else {
            fr.pos.x = Dot3(com->x, rot->rows0.x, com->y, rot->rows1.x, com->z, rot->rows2.x);
            fr.pos.y = Dot3(com->x, rot->rows0.y, com->y, rot->rows1.y, com->z, rot->rows2.y);
            fr.pos.z = Dot3(com->x, rot->rows0.z, com->y, rot->rows1.z, com->z, rot->rows2.z);
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
        f32 lp_y = Dot3(dx, fr.rot.rows1.x, dy, fr.rot.rows1.y, dz, fr.rot.rows1.z);
        lp.x = Dot3(dx, fr.rot.rows0.x, dy, fr.rot.rows0.y, dz, fr.rot.rows0.z);
        lp.y = lp_y;
        lp.z = Dot3(dx, fr.rot.rows2.x, dy, fr.rot.rows2.y, dz, fr.rot.rows2.z);
        HVec li;
        if (&li == &hit) {
            f32 x = Dot3(hit.x, fr.rot.rows0.x, hit.y, fr.rot.rows0.y, hit.z, fr.rot.rows0.z);
            f32 y = Dot3(hit.x, fr.rot.rows1.x, hit.y, fr.rot.rows1.y, hit.z, fr.rot.rows1.z);
            f32 z = Dot3(hit.x, fr.rot.rows2.x, hit.y, fr.rot.rows2.y, hit.z, fr.rot.rows2.z);
            li.x = x;
            li.y = y;
            li.z = z;
        } else {
            li.x = Dot3(hit.x, fr.rot.rows0.x, hit.y, fr.rot.rows0.y, hit.z, fr.rot.rows0.z);
            li.y = Dot3(hit.x, fr.rot.rows1.x, hit.y, fr.rot.rows1.y, hit.z, fr.rot.rows1.z);
            li.z = Dot3(hit.x, fr.rot.rows2.x, hit.y, fr.rot.rows2.y, hit.z, fr.rot.rows2.z);
        }
        fn_8018E3EC(p, &li, &lp, 0, otherId, kind, other, c);
    }
}

int fn_80189BDC(u8* p) {
    CarBody* cb = (CarBody*)p;
    if (U32(p, 0x10) != 'TRLR') {
        u8* o = ((VG*)((u8*&)cb->src))->v10();
        if (o && (S32(o, 0x24) == 1 || S32(o, 0x24) == 3)) {
            return 0;
        }
    }
    return fn_801B83A0()->sCC(cb->index);
}


void fn_80189C64(u8* p) {
    CarBody* cb = (CarBody*)p;
    if (cb->f26AC) {
        cb->f26AC = 0;
        fn_801BF1B8(PTR(p, 0x1EC), 0);
    }
    cb->f26A8 = 0.0f;
}

// A collision shape of the body (slots at 0x14, count at 0x24).
class CbShape {
public:
    virtual ~CbShape(void);  // 0x08 (a CollShape: slot 0 is __dt__9CollShape)
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


static inline void PlaceRotate(RVec3* out, const RVec3* v, const RMtx* m) {
    if (out == v) {
        f32 x = Dot3(v->x, m->rot.x.x, v->y, m->rot.y.x, v->z, m->rot.z.x);
        f32 y = Dot3(v->x, m->rot.x.y, v->y, m->rot.y.y, v->z, m->rot.z.y);
        f32 z = Dot3(v->x, m->rot.x.z, v->y, m->rot.y.z, v->z, m->rot.z.z);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(v->x, m->rot.x.x, v->y, m->rot.y.x, v->z, m->rot.z.x);
        out->y = Dot3(v->x, m->rot.x.y, v->y, m->rot.y.y, v->z, m->rot.z.y);
        out->z = Dot3(v->x, m->rot.x.z, v->y, m->rot.y.z, v->z, m->rot.z.z);
    }
}

f32 fn_801E9DD0(s16* surface, f32 x, f32 z);
void fn_801BE400(s32 car, RMtx* frame, f32 a);
void fn_801B30C0(s32 car, s32 reset, void* start);
void fn_8026A394(u8* car);
void fn_8018A750(u8* p);

// Puts the car down at `s` (a frame for the body origin, the velocity at 0x30, the start point at
// 0x3C and the surface at 0x40) at rest: clears the impulses, wheel loads and gear state.
/* MATCHING-AID: this CarBody member is written on the raw object under its mangled name (extern "C"); as a
   member body, MWCC keeps this+K in an extra register and the code differs. */
void SetState__7CarBodyFPC9BodyState(u8* p, const u8* s) {
    RMtx frame;
    RVec3 off;
    RVec3 rotated;
    ((CarBody*)p)->force.x = 0.0f;
    ((CarBody*)p)->force.y = 0.0f;
    ((CarBody*)p)->force.z = 0.0f;
    ((CarBody*)p)->torque.x = 0.0f;
    ((CarBody*)p)->torque.y = 0.0f;
    ((CarBody*)p)->torque.z = 0.0f;
    ((RigidBody*)p)->ClearImpulses();
    fn_80189C64(p);
    frame = *(RMtx*)s;
    Scale(&off, (RVec3*)(p + 0x30), -1.0f);
    PlaceRotate(&rotated, &off, &frame);
    frame.pos.x = frame.pos.x + rotated.x;
    frame.pos.y = frame.pos.y + rotated.y;
    frame.pos.z = frame.pos.z + rotated.z;
    *(RMtx*)(p + 0x3C) = frame;
    ((CollVolume*)p)->Transform();
    f32 vz;
    f32 vy;
    f32 vx;
    vx = F32(s, 0x30);
    vy = F32(s, 0x34);
    vz = F32(s, 0x38);
    ((CarBody*)p)->block.vel[0] = vx;
    ((CarBody*)p)->block.vel[1] = vy;
    ((CarBody*)p)->block.vel[2] = vz;
    ((CarBody*)p)->vel.x = vx;
    ((CarBody*)p)->vel.y = vy;
    ((CarBody*)p)->vel.z = vz;
    ((CarBody*)p)->avel.x = 0.0f;
    ((CarBody*)p)->avel.y = 0.0f;
    ((CarBody*)p)->avel.z = 0.0f;
    ((CarBody*)p)->f25D8 = ((CarBody*)p)->f25CC = PSVECDotProduct((const Vec*)(p + 0x118), (const Vec*)(p + 0x54));
    ((CarBody*)p)->f261C = ((CarBody*)p)->speed = PSVECMag((const Vec*)(p + 0x118));
    ((CarBody*)p)->accel.x = 0.0f;
    ((CarBody*)p)->accel.y = 0.0f;
    ((CarBody*)p)->accel.z = 0.0f;
    ((CarBody*)p)->push.x = 0.0f;
    ((CarBody*)p)->push.y = 0.0f;
    ((CarBody*)p)->push.z = 0.0f;
    *((CollVolume*)p)->hint = *(s16*)(s + 0x40);
    fn_801E9DD0(((CollVolume*)p)->hint, frame.pos.x, frame.pos.z);
    ((CarGear*)(lbl_8055E918 + ((CarBody*)p)->index * 0x9C))->Clear(((CarBody*)p)->index);
    f32 speed = ((CarBody*)p)->f25CC;
    fn_801BE400(((CarBody*)p)->index, &frame, speed);
    fn_801B30C0(((CarBody*)p)->index, 0, PTR(s, 0x3C));
    for (int i = 0; i < S32(p, 0x24); i++) {
        ShapeAt(p, i)->Reset();
    }
    for (int w = 0; w < 4; w++) {
        F32(p, 0x23AC + w * 0x74) = 0.0f;
        F32(p, 0x2414 + w * 0x74) = 0.0f;
    }
    fn_8018A750(p);
    ((CarBody*)p)->f2638 = 0;
    ((CarBody*)p)->f2628 = 0.0f;
    ((CarBody*)p)->f262C = 0.0f;
    ((CarBody*)p)->f2640 = 0;
    ((CarBody*)p)->f26D4++;
    fn_8026A394(p);
}

// CollShape::vf48 (empty; slot 0x48 of the sphcoll shape tables), by its linker name: carbody only forward-declares CollShape.
extern "C" void vf48__9CollShapeFv(void) {}

void fn_8018A08C(u8* p) { F32(p, 0x26D8) = lbl_8055E3C8; }

void fn_8018A098(u8* p, f32 t) {
    CarBody* cb = (CarBody*)p;
    cb->f259D = 1;
    f32 m = cb->f2CC * fn_80181AEC();
    cb->f258C = cb->setupA.f74 * (LerpT(t, m, driveTune.v[0]));
}

int fn_8018A100(u8* p) { return F32(p, 0x2618) > driveTune.v[1]; }

void fn_8018A120(u8* p) {
    CarBody* cb = (CarBody*)p;
    if (!fn_801B2F2C(PTR(p, 0x1EC))) {
        cb->f25BC = lbl_8055E4E0.a;
        ((VR*)cb->f2584)->v38();
    }
}

void fn_8018A174(u8* p) { F32(p, 0x2650) = 4.0f; }

}  // extern "C"




extern CarElem* lbl_8055E908;  // carelem.cpp: one per car

extern "C" {
void fn_8018F2C0(u8* p, u8* q);
void fn_8018AF04(u8* p);
void fn_802E30C8(const char* fmt, ...);
void fn_801BDA74(s32 id, u8* mass);

// Re-applies the tuned body setup (mass, inertia, elements, gearing) after a change.
/* Helper name unproven: unused, so MWCC drops it but keeps its string. */
static void KickReport(int id, f32 mph, f32 terr) {
    fn_802E30C8("%d: terrain morph pants-kick: %.2f MPH, terr %.2f MPH", id, mph, terr);
}

void fn_8018A180(u8* p) {
    CarBody* cb = (CarBody*)p;
    if (cb->f328) {
        fn_80279B28(cb->f328, (u8*)&cb->state);
        fn_8018F2C0(p, (u8*)&cb->state);
        if (cb->state.x298 < 9 && ((DeityCar*)cb->src)->Held()) {
            fn_802E30C8("car: unimpairing car %d", cb->index);
            ((DeityCar*)cb->src)->Release();
            fn_8018AF04(p);
        }
    }
    cb->offset.x = cb->setupA.modelOffset.x;
    cb->offset.y = cb->setupA.modelOffset.y;
    cb->offset.z = cb->setupA.modelOffset.z;
    ((CollVolume*)p)->CollVolume::vf20();
    ((RigidBody*)p)->SetMass(cb->setupA.mass / 0.4545f, cb->setupA.f04 / 0.042279866f, cb->setupA.f08 / 0.042279866f,
                             cb->setupA.f0C / 0.042279866f);
    cb->mass = cb->setupA.mass;
    cb->invMass = 1.0f / cb->setupA.mass;
    fn_801BDA74(cb->index, (u8*)&cb->setupA);
    lbl_8055E908[cb->index].Fetch(cb->index);
    ((CarGear*)(lbl_8055E918 + cb->index * 0x9C))->Fetch(cb->index);
}

void DeityCar_vf78(u8* p) {}

// Physics tuning sliders: a sink taking (label, value, units, min, max, step).
struct TuneSink {
    virtual void Add(const char* name, void* value, const char* units, float lo, float hi, float step);
};


// Registers the body's tunables (mass, moments, power, geometry, springs, damping, tires) with a tuning menu.
void fn_8018A2C0(u8* p, TuneSink* t) {
    u8* body = p + 0x32C;
    const char* m = " m";
    const char* kg = " kg";
    const char* kgm2 = " kg m2";
    const char* nm = " N/m";
    const char* nsm = " N-s/m";
    t->Add("Mass", body, kg, 100.0f, 5000.0f, 50.0f);
    t->Add("Yaw Moment", body + 8, kgm2, 100.0f, 50000.0f, 50.0f);
    t->Add("Pitch Moment", body + 4, kgm2, 100.0f, 50000.0f, 50.0f);
    t->Add("Roll Moment", body + 0xC, kgm2, 100.0f, 50000.0f, 50.0f);
    t->Add("Engine Power", p + 0x2DC, "", 0.1f, 3.0f, 0.05f);
    t->Add("Final Drive", body + 0x30, "", 0.5f, 6.0f, 0.05f);
    t->Add("Speed Limiter", p + 0x5A8, " m/s", 10.0f, 200.0f, 1.0f);
    t->Add("Wheelbase", body + 0x18, m, 0.1f, 5.0f, 0.01f);
    t->Add("Front Track", body + 0x10, m, 0.1f, 5.0f, 0.01f);
    t->Add("Rear Track", body + 0x14, m, 0.1f, 5.0f, 0.01f);
    t->Add("CG Height", body + 0x20, m, -1.0f, 1.0f, 0.01f);
    t->Add("Front Height", body + 0x80, m, 0.0f, 1.0f, 0.01f);
    t->Add("Rear Height", body + 0xC8, m, 0.0f, 1.0f, 0.01f);
    t->Add("Front Springs", body + 0x84, nm, 1000.0f, 50000.0f, 500.0f);
    t->Add("Rear Springs", body + 0xCC, nm, 1000.0f, 50000.0f, 500.0f);
    t->Add("Front Damping", body + 0x8C, nsm, 500.0f, 10000.0f, 50.0f);
    t->Add("Rear Damping", body + 0xD4, nsm, 500.0f, 10000.0f, 50.0f);
    t->Add("Tire Grip", body + 0x110, " G", 0.1f, 3.0f, 0.1f);
    t->Add("Tire Slip", body + 0x114, "", 0.1f, 6.0f, 0.1f);
}

void fn_8018A654(u8* p, const u8* cfg) {
    CarBody* cb = (CarBody*)p;
    if (*(s8*)(cfg + 0xEC)) {  // CarDesc::part (typed access reuses the address: lbzu)
        u8* m = fn_801D2008((u8*)cfg + 0xEC);
        cb->f26C0 = m;
        if (S32(m, 0x10) > 0) {
            S32(p, 0x26C4) = fn_801C1C1C(m);
        } else {
            S32(p, 0x26C4) = 0;
        }
        s32 n = S32(cb->f26C0, 0xC8);
        if (n > 0) {
            S32(p, 0x26C8) = fn_801C1BA4(n * 2, n, n);
        } else {
            S32(p, 0x26C8) = 0;
        }
    } else {
        cb->f26C0 = 0;
        S32(p, 0x26C4) = 0;
        S32(p, 0x26C8) = 0;
    }
}

void fn_8018A6F4(u8* p) {
    CarBody* cb = (CarBody*)p;
    if (cb->f26C8) {
        fn_801C1C7C(cb->f26C8);
    }
    if (cb->f26C4) {
        fn_801C1C7C(cb->f26C4);
    }
    if (cb->f26C0) {
        fn_801D21B4(cb->f26C0);
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
    CarBody* cb = (CarBody*)p;
    if (cb->f26C4) {
        fn_801C2170(cb->f26C4, cb->f26C0, (u8*)&cb->world);
    }
    if (cb->f26C8) {
        SprVert verts[8];
        SprEdge edges[4];
        SprPin pins[4];
        int n = S32(cb->f26C0, 0xC8);
        int nv = n * 2;
        int i = 0;
        if (n > 0) {
            for (; i < S32(cb->f26C0, 0xC8); i++) {
                SprVert* v = &verts[i * 2];
                v->pos.v[0] = cb->world.pos.x;
                v->pos.v[1] = cb->world.pos.y;
                v->pos.v[2] = cb->world.pos.z;
                v->w34 = 1.0f;
                v->w30 = 0.5f;
                v[1] = v[0];
                edges[i].a = i * 2;
                edges[i].b = i * 2 + 1;
                edges[i].extra = 0.0f;
                edges[i].stiff = 1.0f - F32(cb->f26C0 + i * 8, 0xA8);
                pins[i].idx = i * 2;
                pins[i].mtx = (u8*)&cb->world;
                pins[i].at = 0;
            }
        }
        fn_801C1CBC(cb->f26C8, nv, verts, n, edges, n, pins);
    }
}

}  // extern "C"

extern "C" {
double fmod(double x, double y);
void fn_801C27D8(u8* sim, RVec3* gravity, RVec3* up, RVec3* force, RVec3* torque);

// Feeds the flapping parts (flags/aerials at 0x26C4/0x26C8) their wind and motion input.
void fn_8018A8A8(u8* p) {
    CarBody* cb = (CarBody*)p;
    RVec3 gravity(0.0f, -10000.0f, 0.0f);
    RVec3 up(0.0f, 1.0f, 0.0f);
    RVec3 zero(0.0f, 0.0f, 0.0f);
    if (cb->f26C4) {
        f32 t = lbl_8055E3C8;
        RVec3 wind(1681.65f * ((f32)fmod(t, 0.16f) - 0.08f), 1954.35f * ((f32)fmod(t, 0.14f) - 0.07f),
                  1363.5f * ((f32)fmod(t, 0.2f) - 0.1f));
        fn_801C27D8(cb->f26C4, &gravity, &up, &wind, &zero);
    }
    if (cb->f26C8) {
        RVec3 v;
        Scale(&v, (RVec3*)&cb->vel, -227.25f);
        fn_801C27D8(cb->f26C8, &gravity, &up, &v, &zero);
    }
}

void fn_801C2BDC(u8* sim, s32* n, RVec3** pos, RVec3** old);
void fn_801BEB30(s32 car, u8** out);


// v in the body frame (rotation rows at 0x70, position at 0x94), in place.
static inline void ToBody(u8* p, RVec3* o, const RVec3* v) {
    o->x = v->x - F32(p, 0x94);
    o->y = v->y - F32(p, 0x98);
    o->z = v->z - F32(p, 0x9C);
    f32 x = Dot3(o->x, F32(p, 0x70), o->y, F32(p, 0x74), o->z, F32(p, 0x78));
    f32 y = Dot3(o->x, F32(p, 0x7C), o->y, F32(p, 0x80), o->z, F32(p, 0x84));
    f32 z = Dot3(o->x, F32(p, 0x88), o->y, F32(p, 0x8C), o->z, F32(p, 0x90));
    o->x = x;
    o->y = y;
    o->z = z;
}

// Copies the flapping parts' simulated points (0x26C4 flag, 0x26C8 aerial) into the car model's point
// list `m` (0xE24) in body space, then pins the four wheel points at the suspension heights.
void fn_8018A9F0(u8* p, u8* m) {
    CarBody* cb = (CarBody*)p;
    if (cb->f26C4) {
        RVec3* pos = 0;
        RVec3* old = 0;
        s32 n = 0;
        fn_801C2BDC(cb->f26C4, &n, &pos, &old);
        if (S32(cb->f26C0, 0xA4) > 0) {
            for (int i = 0; i < S32(cb->f26C0, 0xA4); i++) {
                u8* d = cb->f26C0;
                ToBody(p, (RVec3*)(m + 0xE24) + (S32(d, 0xA0) + i), &pos[((s32*)(d + 0x20))[i]]);
            }
        }
    }
    if (cb->f26C8) {
        RVec3* pos = 0;
        RVec3* old = 0;
        s32 n = 0;
        fn_801C2BDC(cb->f26C8, &n, &pos, &old);
        s32 i = 0;
        if (S32(cb->f26C0, 0xC8) > 0) {
            for (; i < S32(cb->f26C0, 0xC8); i++) {
                u8* d = cb->f26C0;
                ToBody(p, (RVec3*)(m + 0xE24) + ((s32*)(d + 0xAC))[i * 2], &pos[i * 2 + 1]);
            }
        }
    }
    u8* x = 0;
    fn_801BEB30(cb->index, &x);
    CbVec4 h = *(CbSolVec*)(x + 0x70);
    for (int i = 0; i < 4; i++) {
        RVec3* o = (RVec3*)(m + 0xE24) + ((s32*)(cb->f26C0 + 0xCC))[i];
        o->Set(0.0f, cb->f5AC + (F32((u8*)&((CarDrawState*)m)->wheels + i * 0x6C, 0x4) + h[i]), 0.0f);
    }
    ((CarDrawState*)m)->fFA4 = S32(cb->f26C0, 0xDC);
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

f32 fn_8018AE3C(u8*) { return 0.0f; }

f32 fn_8018AE44(u8*) { return 0.0f; }

f32 fn_8018AE4C(u8*) { return 0.0f; }

f32 fn_8018AE54(u8*) { return 0.0f; }

// Shared body-tuning block (lbl_80434990), built on first use.
struct BodyTune {
    BodyTune(void) {}
    u8 data[0x10];
};

BodyTune* fn_8018AE5C(u8*) {
    static BodyTune s;
    return &s;
}

f32 fn_8018AE7C(u8* p) {
    CarBody* cb = (CarBody*)p;
    if (fn_801769CC()->type == 0x180304) {
        return 0.15f;
    }
    if (Positive(cb->f25BC)) {
        return 0.15f;
    }
    if (cb->mass > 1363.5f) {
        return 0.15f;
    }
    return 0.5f;
}

}  // extern "C"

void CarBody::vf58(void) {
    u8* p = (u8*)this;}



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
// Clears a wheel's detached flag and returns its saved load. /* Helper name unproven */
static inline f32 CbReattach(u8* w) {
    S32(w, 0x0) &= ~8;
    return F32(w, -0x34);
}
void fn_8018AF04(u8* p) {
    CarBody* cb = (CarBody*)p;
    fn_8018F2C0(p, !fn_801CF064() ? (u8*)&cb->state : (u8*)&cb->saved);
    CVec4 load;
    u8* w = p + 0x241C;
    for (int i = 0; i < 4; i++, w += 0x74) {
        (&load.x)[i] = CbReattach(w);
    }
    u8* x;
    fn_801BEB30(cb->index, &x);
    SetVec4(x + 0x70, load);
    SetVec4(x + 0x130, CVec4(0.0f, 0.0f, 0.0f, 0.0f));
    SetVec4(x + 0x140, CVec4(0.0f, 0.0f, 0.0f, 0.0f));
    if (cb->f26B0 >= 0) {
        cb->f26B0 = -1;
    }
    cb->f1E40 = 0;
    if (*fn_801E1F44() == 1) {
        fn_8018F6F8(p);
    }
    cb->f2650 = 0.0f;
    cb->f264C = 0.0f;
    cb->f25C4 = 0.0f;
}

u32 fn_8018B06C(u8* p) { return U32(lbl_8055E918 + S32(p, 0x1EC) * 0x9C, 0x14); }

u32 fn_8018B084(u8* p) { return U32(lbl_8055E918 + S32(p, 0x1EC) * 0x9C, 0x20); }

}  // extern "C"

void CarBody::OnHit(const RVec3* a, const RVec3* b, int c, void* d) {
    u8* p = (u8*)this;
    ((RigidBody*)p)->RigidBody::OnHit(a, b, c, d);
    F32(p, 0x26DC) = lbl_8055E3C8;
}

extern "C" {

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
static inline f32 PkUnpack(u32 v, f32 q, f32 lo, f32 span) { f32 r = PkNorm(v, 0.0f, q); return lo + r * span; }

static inline void PkCopy(void* dst, const void* src, int n) {
    u8* d = (u8*)dst;
    const u8* s = (const u8*)src;
    while (--n >= 0) {
        *d++ = *s++;
    }
}

void fn_8018ADB4(u8* p, f32 v);
void fn_8018ADBC(u8* p, f32 v);
f32 fn_801E9DD0(s16* surface, f32 x, f32 z);

// CarBody::vf30: applies a state packet interpolated between `cur` and `prev` (t = 0 is cur): the rigid body
// part, engine speed, steering, pedals, gear and the four wheels (suspension, spin with 2 pi wrap, slip, skid,
// flags), then the ground lookup at prev's position. Both sizes read are 0x44 bytes.
/* MATCHING-AID: this CarBody member is written on the raw object under its mangled name (extern "C"); as a
   member body, MWCC keeps this+K in an extra register and the code differs. */
void vf30__7CarBodyFfP7RPacketPC7RPacketPlPl(u8* p, f32 t, const CbPacket* cur, const CbPacket* prev, s32* na, s32* nb) {
    CarBody* cb = (CarBody*)p;
    int teleport = (prev->flags >> 9) & 1;
    if (teleport) {
        ((RigidBody*)p)->RigidBody::vf30(t, (RPacket*)cur, (const RPacket*)cur, na, nb);
    } else {
        ((RigidBody*)p)->RigidBody::vf30(t, (RPacket*)cur, (const RPacket*)prev, na, nb);
    }
    cb->speed = cb->f25CC = cb->f25D8 = PSVECMag((const Vec*)((u8*)&cb->vel));
    f32 v0 = PkUnpack(prev->rpm, 65535.0f, -2000.0f, 14000.0f);
    f32 v1 = PkUnpack(cur->rpm, 65535.0f, -2000.0f, 14000.0f);
    cb->f25D0 = LerpT(t, v1, v0);
    v0 = PkUnpack(prev->steer, 255.0f, -1.2f, 2.4f);
    v1 = PkUnpack(cur->steer, 255.0f, -1.2f, 2.4f);
    cb->f25DC = cb->f25F0 = LerpT(t, v1, v0);
    if (cur->boost) {
        f32 boost;
        if (teleport) {
            boost = PkUnpack(cur->boost, 255.0f, 0.0f, 1.0f);
        } else {
            v0 = PkUnpack(prev->boost, 255.0f, 0.0f, 1.0f);
            v1 = PkUnpack(cur->boost, 255.0f, 0.0f, 1.0f);
            boost = LerpT(t, v1, v0);
        }
    }
    f32 steer = cb->f25F0 * cb->f5B0;
    cb->wheels[2].steer = 0.0f;
    cb->wheels[3].steer = 0.0f;
    cb->f25F8 = 1.0f;
    cb->wheels[1].steer = cb->wheels[0].steer = steer;
    cb->f25E8 = 1.0f;
    v0 = PkUnpack(prev->throttle, 255.0f, 0.0f, 1.0f);
    v1 = PkUnpack(cur->throttle, 255.0f, 0.0f, 1.0f);
    fn_8018ADB4(p, LerpT(t, v1, v0));
    v0 = PkUnpack(prev->brake, 255.0f, 0.0f, 1.0f);
    v1 = PkUnpack(cur->brake, 255.0f, 0.0f, 1.0f);
    fn_8018ADBC(p, LerpT(t, v1, v0));
    U32(p, 0x4) = cur->flags | prev->flags;
    cb->f260D = (cur->flags >> 8) & 1;
    u8* w = (u8*)&cb->wheels[0];
    const u8* c = cur->wheel[0];
    const u8* q = prev->wheel[0];
    for (int i = 0; i < 4; i++) {
        f32 s0 = PkUnpack(q[2], 255.0f, 0.0f, 2.0f);
        f32 s1 = PkUnpack(c[2], 255.0f, 0.0f, 2.0f);
        ((CbWheel*)w)->prevSlip = ((CbWheel*)w)->slip = LerpT(t, s1, s0);
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
        ((CbWheel*)w)->spin = LerpT(t, a1, a0);
        f32 l1;
        f32 l0 = PkUnpack(q[1], 255.0f, -100.0f, 200.0f);
        l1 = PkUnpack(c[1], 255.0f, -100.0f, 200.0f);
        ((CbWheel*)w)->load = LerpT(t, l1, l0);
        f32 k0 = PkUnpack(q[3], 255.0f, 0.0f, 10.0f);
        f32 k1 = PkUnpack(c[3], 255.0f, 0.0f, 10.0f);
        u8 f = c[4];
        ((CbWheel*)w)->comp = LerpT(t, k1, k0);
        S32(w, 0x70) = f;
        U8(w, 0x6C) = 1;
        ((CbWheel*)w)->grounded = f & 1;
        w += 0x74;
        c += 5;
        q += 5;
    }
    u8* rec = lbl_8055E918 + cb->index * 0x9C;
    S32(rec, 0x14) = cur->gear;
    S32(rec, 0x8) = cur->gear;
    cb->f2678 = cur->w40;
    *nb = 0x44;
    *na = 0x44;
    f32 x, z;
    PkCopy(&x, prev->x, 4);
    PkCopy(&z, prev->z, 4);
    fn_801E9DD0((s16*)&cb->hint, x, z);
}

}  // extern "C"

static inline f32 PackClamp01(f32 x) {
    x = 1.0f < x ? 1.0f : x;
    return x > 0.0f ? x : 0.0f;
}


// Quantises x from [lo, hi] to 0..range (rounded).
static inline int Pack(f32 x, f32 lo, f32 hi, f32 range) {
    return (int)(0.5f + Lerp(0.0f, range, MaxF(MinF(1.0f, (x - lo) / (hi - lo)), 0.0f)));
}

extern "C" {
s32 fn_801363A0(u8* seg, u8* pos);

// Writes the car's state into a network/replay packet: engine and control values, then per wheel
// (steer angle, suspension, spin, slip) and its contact flags.
}  // extern "C"

s32 CarBody::vf34(RPacket* pk_) {
    ((RigidBody*)((u8*)this))->RigidBody::vf34((RPacket*)((u8*)pk_));
    U16(((u8*)pk_), 0x20) = Pack(this->f25D0, -2000.0f, 12000.0f, 65535.0f);
    U8(((u8*)pk_), 0x22) = Pack(this->f25F0, -1.2f, 1.2f, 255.0f);
    U8(((u8*)pk_), 0x23) = Pack(this->f25F4, 0.0f, 1.0f, 255.0f);
    const int control = Pack(this->f25E4, 0.0f, 1.0f, 255.0f);
    U8(((u8*)pk_), 0x24) = control;
    f32 k = 1.0f;
    const int variation = (int)(0.5f + Lerp(0.0f, 255.0f, k));
    U8(((u8*)pk_), 0x3C) = variation;
    u8* seg = (u8*)this->src->Ideal();
    S32(((u8*)pk_), 0x40) = fn_801363A0(seg, seg + 4);
    if (U8(((u8*)pk_), 0x3C) == 0) {
        U8(((u8*)pk_), 0x3C) = 1;
    }
    U16(((u8*)pk_), 0x3A) = this->a4;
    U8(((u8*)pk_), 0x25) = S32(lbl_8055E918 + this->index * 0x9C, 0x14);
    u8* w = ((u8*)this) + 0x23AC;
    u8* out = ((u8*)pk_) + 0x26;
    for (int i = 0; i < 4; i++) {
        u32 flags = ((CbWheel*)w)->flags;
        if (((CbWheel*)w)->grounded) {
            flags |= 1;
        }
        out[2] = Pack(((CbWheel*)w)->slip, 0.0f, 2.0f, 255.0f);
        out[0] = Pack(((CbWheel*)w)->spin, -6.2831855f, 6.2831855f, 255.0f);
        out[1] = Pack(((CbWheel*)w)->load, -100.0f, 100.0f, 255.0f);
        out[3] = Pack(((CbWheel*)w)->comp, 0.0f, 10.0f, 255.0f);
        out[4] = flags;
        w += 0x74;
        out += 5;
    }
    return 0x44;
}


void CarBody::Notify(const RVec3* v, const RVec3* at, int kind, void* c, void* by) {
    u8* p = (u8*)this;
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

extern "C" {

void fn_8018BE54(u8* p, f32 s) {
    CarBody* cb = (CarBody*)p;
    if ((f32)__fabs(cb->f5E4 - s) <= __float_epsilon[0]) {
        return;
    }
    cb->f5E4 = s;
    fn_801A1474(cb->f268, cb->f5E4);
    f32 k = cb->f5E4;
    f32 k2 = k * k;
    cb->setupA.f04 = cb->setupB.f04 * k2;
    cb->setupA.f08 = cb->setupB.f08 * k2;
    cb->setupA.f0C = cb->setupB.f0C * k2;
    cb->setupA.f18 = cb->setupB.f18 * k;
    cb->setupA.f10 = cb->setupB.f10 * k;
    cb->setupA.f14 = cb->setupB.f14 * k;
    cb->setupA.f20 = cb->setupB.f20 * k;
    F32(p, 0x3AC) = k * F32(p, 0x4E4);
    F32(p, 0x3D0) = k * F32(p, 0x508);
    F32(p, 0x3F4) = k * F32(p, 0x52C);
    F32(p, 0x418) = k * F32(p, 0x550);
    fn_8018A180(p);
}

void fn_8018BF44(u32* p) { *p = 0x0FF807FF; }

u32 fn_8018BF54(u8* p) { return 0; }

void fn_8018BF5C(u8* p) {}

}  // extern "C"

void CarBody::Update(void) {
    u8* p = (u8*)this;}


s32 CarBody::vf2C(RPacket* pk) { return 0x44; }

