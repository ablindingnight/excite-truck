// Player car driver (0x8018FB18-0x8019309C): the PlayerDriver/LocalCar side of the car code - input
// to throttle/steer/brake, replay packet recording ("replaypkt"), force-feedback jolts ("ff_jolt"),
// the hybrid control mode, and the per-car wrapper objects that forward to the physics car. Written
// against raw field offsets; the large routines are still stubs. Names are guesses apart from the
// strings.
#include "game/common.h"
#include "game/axisrow.h"
#include "game/buildquery.h"
#include "game/proxer.h"
#include "game/raceopts.h"
#include "game/rvec.h"
#include "game/rigid.h"
#include "game/carbody.h"
#include "game/deity.h"
#include "game/idealline.h"
#include <string.h>
#include <revolution/MTX/vec.h>
#include "game/mathutil.h"



#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define B8(p, o) (*(bool*)((u8*)(p) + (o)))
// The meter flag at +0x34 is a VOLATILE field (form provisional; user-approved 2026-11-17). Retail keeps a store that
// MWCC's post-schedule peephole deletes unless volatile, and volatile at all 11 accesses changes no other function
// (fable-rules FACTS 2026-11-17; Codex verified). It replaces a one-site volatile cast (fb0251d59).
#define VU8(p, o) (*(volatile u8*)((u8*)(p) + (o)))
#define VB8(p, o) (*(volatile bool*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

struct PFlags {
    u8 lvl : 3;
    u8 b4 : 1;
    u8 b3 : 1;
    u8 b2 : 1;
    u8 b1 : 1;
    u8 b0 : 1;
};

#define PF(p) ((PFlags*)((u8*)(p) + 7))

// The PhysDriver input interface (__vt__10PhysDriver: SetGear at 0x08, destructor LAST at 0x80), as seen
// through the car's replay Recorder (lbl_80332F40 forwards every slot to its PhysDriver source).
class VPx {
public:
    virtual void v08(int gear);  // SetGear (not a destructor)
    virtual void v0C(void);
    virtual void v10(u8* car);
    virtual u32 v14(void);
    virtual int v18(int x);
    virtual void v1C(s32 x, s32 y, s32* dx, s32* dy, f32 t);
    virtual int v20(int x);
    virtual void v24(int a);
    virtual void v28(f32 t);  // SetThrottle
    virtual void v2C(void);
    virtual void v30(void);
    virtual void v34(void);
    virtual int v38(void);
    virtual void v3C(u32 a, u32 b, int c, u32 d);
    virtual f32 v40(void);  // PhysDriver::vf40 returns f32 (x87 stack on the PC port)
    virtual void v44(void);
    virtual void v48(void);
    virtual void v4C(void);
    virtual void v50(f32 a, f32 b, f32 c);  // SetRumbleA
    virtual void SetRumbleB(f32 a, f32 b, f32 c);
    virtual void v58(f32 a, f32 b);  // SetRumbleC
    virtual void v5C(void);
    virtual void v60(void);
    virtual void v64(void);
    virtual void v68(void);
    virtual void v6C(void);
    virtual u8* v70(void);
    virtual void v74(void);
    virtual void v78(int a);
    virtual void v7C(void);
#ifdef __MWERKS__
    virtual void v80(int del);
#else
    virtual ~VPx(void);  // PORT: slot 0x80 is the destructor; on the host a virtual destructor takes two slots
#endif
};

#define VP(p) ((VPx*)(p))

// The player controller (lbl_80333018) and its drivers (IDriver family at +0x8/+0xC, active one at +0x10):
// slot 0x08 is the destructor. Class name unproven.
class VCtl {
public:
    virtual ~VCtl(void);
    virtual void v0C(void);
    virtual void v10(u8* car);
    virtual u32 v14(void);
    virtual int v18(int x);
    virtual void v1C(s32 x, s32 y, s32* dx, s32* dy, f32 t);
    virtual int v20(int x);
    virtual void v24(int a);
    virtual void v28(void);
    virtual void v2C(void);
    virtual void v30(void);
    virtual void v34(void);
    virtual int v38(void);
    virtual void v3C(u32 a, u32 b, int c, u32 d);
    virtual void v40(void);
    virtual void v44(void);
    virtual void v48(void);
};

#define VC(p) ((VCtl*)(p))

class VDtor {
public:
    virtual ~VDtor(void);
};

f32 lbl_8055A468 = 7.5f;
f32 lbl_8055A46C = 12.5f;

static inline void SetMode(u8* p, int on) {
    U8(p, 0x24) = 1;
    B8(p, 0x25) = on;
    B8(p, 0x26) = !B8(p, 0x25);
}

// The same interface viewed with an argument-less slot 0x24.
class VPa {
public:
    virtual ~VPa(void);  // PlayerDriver_vf08 is the destructor
    virtual void s0C(void);
    virtual void s10(void);
    virtual void s14(void);
    virtual void s18(void);
    virtual void s1C(void);
    virtual void s20(void);
    virtual void v24(void);
};

#define kNegOne -1.0f
#define k255 255.0f
#define kTwo 2.0f
#define kNegThree -3.0f
#define kSix 6.0f
#define k65535 65535.0f

#define CMIN(a, b) ((a) < (b) ? (a) : (b))
#define CMAX(a, b) ((a) > (b) ? (a) : (b))

// A raw pad value mapped from [inLo, inHi] onto [outLo, outHi], clamped (helper name unproven).
static inline f32 PadRange(u32 v, f32 inLo, f32 inHi, f32 outLo, f32 outHi) {
    f32 t = CMIN(1.0f, ((f32)v - inLo) / (inHi - inLo));
    t = CMAX(t, 0.0f);
    return outLo + t * (outHi - outLo);
}
static inline f32 PcFade(f32 hi, f32 lo, f32 a, f32 b) { return hi - (hi - lo) * a * b; }
static inline f32 PcAddF(f32 a, f32 b) { return a + b; }
template <class O> static inline void PcScaleTo(O* o, const f32* v, f32 k) { o->x = v[0] * k; o->y = v[1] * k; o->z = v[2] * k; }
template <class O> static inline void PcAddScaled(O* o, const f32* a, const f32* d, f32 k) { o->x = a[0] + d[0] * k; o->y = a[1] + d[1] * k; o->z = a[2] + d[2] * k; }
static inline f32 Clamp01(f32 x) {
    x = 1.0f < x ? 1.0f : x;
    return x > 0.0f ? x : 0.0f;
}


extern "C" {
s32 fn_80137C74(void);
int fn_801B89E0(void);
void fn_801B665C(void);
void fn_801B6664(u8* p);
void fn_8018AE24(u8* car, int v);
void fn_80191084(u8* p, u32 a);
void fn_80191758(u8* p);
int fn_801B526C(u8* p);
int fn_801769F8(void);
void fn_801B6650(u8* p);
void fn_802793B0(u8* p);
void fn_801797A8(u8* p, const char* fmt, ...);
u32 fn_80191100(u8* p);
void fn_801B6660(void);
int fn_801CF050(void);
void fn_801B6658(void);
int fn_801B6668(void);
void fn_801B6654(void);
void fn_801B664C(void);
s32 fn_801293A4(void);
void fn_801331C4(u8* ctx, const char* name, u8* value);
u8* fn_80178C90(void);
s32 fn_801B2F54(u8* p);
f32 fn_8018AE3C(u8* car);
f32 fn_8018AE4C(u8* car);
void fn_802E30C8(const char* fmt, ...);
}

void operator delete(void* p);
void operator delete[](void* p);
void* operator new[](unsigned long n, const char* tag);

extern s32 lbl_8055E3C4;

u8* lbl_804349A0[4];
u8* lbl_804349B0[4];

ModuleTag lbl_8055E508;
UnkStaticFFFF lbl_8055E510;
UnkStaticFFFF lbl_8055E514;
s32 lbl_8055E518;
f32 lbl_8055E51C;
u8 lbl_8055E520;
u8 lbl_8055E521;
u8 lbl_8055E522;
u8 lbl_8055E523;
s32 lbl_8055E524;
u8 lbl_8055E528;
u8 lbl_8055E529;
u8 lbl_8055E52A;
u8 lbl_8055E52B;
u8 lbl_8055E52C;

extern "C" {

}  // extern "C"

class PcGameMode {
public:
    virtual ~PcGameMode(void);  // 0x08 (the arena master GameMode: slot 0 is its destructor)
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
    virtual void v5C(void);
    virtual void v60(void);
    virtual void v64(void);
    virtual void v68(void);
    virtual void v6C(void);
    virtual void v70(void);
    virtual void v74(void);
    virtual void v78(void);
    virtual void v7C(void);
    virtual void v80(void);
    virtual void v84(void);
    virtual void v88(void);
    virtual void v8C(void);
    virtual void v90(void);
    virtual void v94(void);
    virtual void v98(void);
    virtual void v9C(void);
    virtual void vA0(void);
    virtual void vA4(void);
    virtual void vA8(void);
    virtual void vAC(void);
    virtual void vB0(void);
    virtual void vB4(void);
    virtual void vB8(void);
    virtual void vBC(void);
    virtual void vC0(void);
    virtual void vC4(void);
    virtual void vC8(void);
    virtual void vCC(void);
    virtual void vD0(void);
    virtual void vD4(void);
    virtual void vD8(void);
    virtual void vDC(void);
    virtual int vE0(s32 car, bool* out);
    virtual void vE4(void);
    virtual void vE8(void);
    virtual u32 PlayerController(void);  // 0xEC
};

#include "game/springcar.h"

void* operator new(unsigned long n, const char* tag);

/* Class name unproven */
// The replay Recorder's interface: every slot pure, destructor last (0x80). Only playcar builds a Recorder, so the
// table exists only as this unit's dead weak copy (retail's 0x84 zero block after the IDriver copy).
class RecorderIface {
public:
    virtual void SetGear(int gear) = 0;
    virtual f32 Steer(void) = 0;
    virtual f32 Throttle(void) = 0;
    virtual f32 Brake(void) = 0;
    virtual f32 Grip(void) = 0;
    virtual f32 vf1C(void) = 0;
    virtual int MaxGear(void) = 0;
    virtual void SetMaxGear(int v) = 0;
    virtual void SetThrottle(f32 t) = 0;
    virtual int vf2C(void) = 0;
    virtual int vf30(void) = 0;
    virtual int vf34(void) = 0;
    virtual int vf38(void) = 0;
    virtual int vf3C(void) = 0;
    virtual f32 vf40(void) = 0;
    virtual f32 vf44(void) = 0;
    virtual f32 vf48(void) = 0;
    virtual void vf4C(void) = 0;
    virtual void SetRumbleA(f32 a, f32 b, f32 c) = 0;
    virtual void SetRumbleB(f32 strength, f32 turn, f32 coefficient) = 0;
    virtual void SetRumbleC(f32 a, f32 b) = 0;
    virtual u8 vf5C(void) = 0;
    virtual u8 vf60(void) = 0;
    virtual u8 vf64(void) = 0;
    virtual u8 vf68(void) = 0;
    virtual void vf6C(void) = 0;
    virtual void vf70(void) = 0;
    virtual void Buttons(void) = 0;
    virtual void SetRumble(int a) = 0;
    virtual void Stop(void) = 0;
    virtual ~RecorderIface(void) = 0;  // pure: the dead copy's last slot is null, so no weak dtor stays referenced
};

inline RecorderIface::~RecorderIface(void) {}

#include "game/idriver.h"

class FieldSaver;

/* Class name unproven ("Hybrid" new tag) */
// The local player's controller (retail lbl_80333018): forwards to the drivers at +0x8 / +0xC (active one at +0x10).
class HybridDriver : public IDriver {
public:
    HybridDriver(u32 a, u32 b);
    virtual ~HybridDriver(void);
    virtual void Init(void* car_, void* q_);
    virtual void SetCar(void* car_);
    virtual u32 Flags(void);
    virtual int vf18(int x);
    virtual void vf1C(int x, int y, int* outx, int* outy, f32 t);
    virtual int vf20(int x);
    virtual void Start(void);
    virtual void Stop(void);
    virtual void vf2C(void);
    virtual void vf30(void);
    virtual void vf34(void);
    virtual void vf38(const RVec3* dir, RVec3* b, int kind, u8* other);
    virtual void vf3C(const RVec3* a_, RVec3* b_, int c, u8* d_);
    virtual void vf40(void);
    virtual void vf44(void);
    virtual void v48(void);
    u8 pad04[0x3C - 4];
};

// The player's own driver (retail lbl_80332FC4, "PlayerDriver" new tag).
class PlayerDriver : public IDriver {
public:
    PlayerDriver(void);
    virtual ~PlayerDriver(void);
    virtual u32 Flags(void);
    virtual void Init(void* car_, void* q);
    virtual void SetCar(void* car_);
    virtual void Start(void);
    virtual void Stop(void);
    virtual void vf2C(void);
    virtual void vf30(void);
    virtual void vf38(const RVec3* dir, RVec3* b, int kind, u8* other);
    virtual void vf3C(const RVec3* v, RVec3* imp_, int a_, u8* other_);
    virtual void vf44(void);
    virtual void vf48(void);
    virtual void vf4C(FieldSaver* s);
    virtual void vf50(void);
    u8 pad04[0x44 - 4];
};


extern "C" {
PcGameMode* fn_801B83A0(void);
u32 CreateAIDriver(void);
u32 GetArenaDriverByType(u32);
u32 CreateParkDriver(void);

// Builds the local player's car: driver, hybrid controller and the car itself.
SpringCar* fn_8018FB18(void) {
    u8* drv;
    u8* hyb;
    u8* prof = (u8*)fn_801769CC();
    u32 ctl = fn_801B83A0()->PlayerController();
    lbl_8055E522 = (U32(prof, 0x4) >> 8) & 1;
    if (U32(prof, 0x4) == 0x180304) {
        lbl_8055E522 = 0;
    }
    if (!ctl) {
        if (U32(prof, 0x4) & 0x100) {
            ctl = CreateAIDriver();
        } else if (U32(prof, 0x4) == 0x180201) {
            ctl = GetArenaDriverByType(U32(prof, 0x4));
        } else {
            ctl = CreateParkDriver();
        }
    }
    drv = (u8*)new ("PlayerDriver") PlayerDriver;
    hyb = (u8*)new ("Hybrid") HybridDriver(ctl, (u32)drv);
    return new ("LocalCar (player)") SpringCar((Controller*)hyb);
}

void fn_8018FC38(void) {
    s32 n = fn_80137C74();
    for (int i = 0; i < n; i++) {
        lbl_804349B0[i] = (u8*)operator new[](0x46500, "replaypkt");
    }
    fn_802E30C8("playcar: allocating %dK for replay", (u32)(n * 0x8CA0) * 8 / 1024);
}

void fn_8018FCC4(void) {
    s32 n = fn_80137C74();
    u8** a = lbl_804349B0;
    for (int i = 0; i < n; i++) {
        delete[] * a;
        *a = 0;
        a++;
    }
}

int fn_8018FD38(void) {
    int r = 0;
    if (lbl_8055E3C4 + 1 >= lbl_8055E524 || fn_801B89E0()) {
        r = 1;
    }
    return r;
}

/* Helper name unproven */
#pragma push
#pragma auto_inline on
static f32 TickCountAsFloat(s32 ticks) { return (f32)ticks; }
#pragma pop

}  // extern "C"

HybridDriver::HybridDriver(u32 a, u32 b) {
    u8* p = (u8*)this;
    if (lbl_8055E518 == 0) {
        fn_8018FC38();
    }
    lbl_804349A0[lbl_8055E518++] = p;
    U8(p, 0x38) = 1;
    lbl_8055E520 = 0;
    lbl_8055E523 = 0;
    U32(p, 0x8) = a;
    U32(p, 0xC) = b;
    U32(p, 0x10) = 0;
    U32(p, 0x4) = 0;
    S32(p, 0x28) = 0x10000;
    if (!fn_801B6668()) {
        fn_801B664C();
    }
    f32 t = 0.001f * TickCountAsFloat(fn_801293A4());
    f32 v = t - 5.0f;
    lbl_8055A46C = t;
    lbl_8055A468 = 0.5f > v ? 0.5f : v;
}

extern "C" {

}  // extern "C"

extern u8 lbl_8055E528;
int OptProfile(void);

// Replay data source of a car (fn_8018108C); slots 0x5C-0x68 report its setup.
class ReplaySrc {
public:
    virtual void v08(void);
    virtual f32 Throttle(f32 speed);
    virtual f32 v10(void);
    virtual f32 v14(void);
    virtual f32 v18(void);
    virtual f32 v1C(void);
    virtual int Gear(void);
    virtual void v24(void);
    virtual void v28(void);
    virtual BOOL v2C(void);
    virtual BOOL v30(void);
    virtual BOOL v34(void);
    virtual BOOL v38(void);
    virtual BOOL v3C(void);
    virtual void v40(void);
    virtual f32 v44(void);
    virtual f32 v48(void);
    virtual void v4C(void);
    virtual void v50(void);
    virtual void v54(void);
    virtual void v58(void);
    virtual u8 v5C(void);
    virtual u8 v60(void);
    virtual u8 v64(void);
    virtual u8 v68(void);
};

// The drivers at +0x8/+0xC (IDriver family: destructor at 0x08, Init at 0x0C).
class CarCtl {
public:
    virtual ~CarCtl(void);
    virtual void Attach(u8* car, u8* q);
};

// The car's DeityCar (+0x14, fn_801B2EC0): Reset at 0x08 (not a destructor), State at 0x10, Ideal at 0x18.
class DeityView {
public:
    virtual void Reset(void);
    virtual void Update(void);
    virtual u8* Info(void);
    virtual void v14(void);
    virtual s32 v18(void);
};

extern "C" {
ReplaySrc* fn_8018108C(s32 id);
DeityView* fn_801B2EC0(s32 id);
}

// The car's replay recorder (0x24): writes packets into the car's replay buffer.
struct Recorder : public RecorderIface {
    ReplaySrc* src;
    u8* buf;
    s32 pos;
    u8 pad10;
    u8 cfg[4];
    u8 pad15[3];
    u8* start;
    s32 cap;
    s32 id;

    virtual void SetGear(int gear);
    virtual f32 Steer(void);
    virtual f32 Throttle(void);
    virtual f32 Brake(void);
    virtual f32 Grip(void);
    virtual f32 vf1C(void);
    virtual int MaxGear(void);
    virtual void SetMaxGear(int v);
    virtual void SetThrottle(f32 t);
    virtual int vf2C(void);
    virtual int vf30(void);
    virtual int vf34(void);
    virtual int vf38(void);
    virtual int vf3C(void);
    virtual f32 vf40(void);
    virtual f32 vf44(void);
    virtual f32 vf48(void);
    virtual void vf4C(void);
    virtual void SetRumbleA(f32 a, f32 b, f32 c);
    virtual void SetRumbleB(f32 strength, f32 turn, f32 coefficient);
    virtual void SetRumbleC(f32 a, f32 b);
    virtual u8 vf5C(void);
    virtual u8 vf60(void);
    virtual u8 vf64(void);
    virtual u8 vf68(void);
    virtual void vf6C(void);
    virtual void vf70(void);
    virtual void Buttons(void);
    virtual void SetRumble(int a);
    virtual void Stop(void);
    virtual ~Recorder(void);

    Recorder(s32 n) {
        id = n;
        fn_80137C74();
        cap = 36000;
        buf = start = lbl_804349B0[n];
        src = fn_8018108C(n);
        pos = 0;
        lbl_8055E524 = 0;
        if (fn_801CF050()) {
            cfg[0] = src->v5C();
            cfg[1] = src->v64();
            cfg[2] = src->v60();
            cfg[3] = src->v68();
        }
    }
};

extern "C" {
}  // extern "C"

HybridDriver::~HybridDriver(void) {
    u8* p = (u8*)this;
    if (fn_801B6668()) {
        fn_801B6654();
    }
    if (PTR(p, 0x4)) {
#ifdef __MWERKS__
        VP(PTR(p, 0x4))->v80(1);
#else
        delete VP(PTR(p, 0x4));  // PORT: the deleting destructor (MWCC's v80(1))
#endif
    }
    PTR(p, 0x4) = 0;
    delete (VDtor*)PTR(p, 0x8);
    PTR(p, 0x8) = 0;
    delete (VDtor*)PTR(p, 0xC);
    PTR(p, 0xC) = 0;
    if (--lbl_8055E518 == 0) {
        fn_8018FCC4();
    }
}

Recorder::~Recorder(void) {}


extern "C" {

#define p ((u8*)this)  // the parameter itself: a named u8* copy of this recolours it (retail registers)
void HybridDriver::Init(void* car_, void* q_) {
    S32(p, 0x28) = U8(((u8*)car_), 0x260) & 3;
    Recorder* r = new ("repl_di") Recorder(S32(p, 0x28));
    PTR(p, 0x4) = (u8*)r;
    PTR(PTR(p, 0xC), 0x38) = (u8*)r;
    B8(p, 0x25) = S32(((u8*)q_), 0x5C) == 0;
    B8(p, 0x26) = S32(((u8*)q_), 0x5C) != 0;
    PTR(p, 0x20) = ((u8*)car_);
    S32(p, 0x1C) = S32(((u8*)car_), 0x1EC);
    DeityView* c = fn_801B2EC0(S32(p, 0x1C));
    PTR(p, 0x14) = (u8*)c;
    S32(p, 0x18) = c->v18();
    ((CarCtl*)PTR(p, 0x8))->Attach(((u8*)car_), ((u8*)q_));
    ((CarCtl*)PTR(p, 0xC))->Attach(((u8*)car_), ((u8*)q_));
    if (OptProfile()) {
        lbl_8055E528 = 1;
    }
}
#undef p

void HybridDriver::SetCar(void* car_) {
    u8* p = (u8*)this;
    u8* car = (u8*)car_;
    PTR(p, 0x20) = car;
    S32(p, 0x1C) = S32(car, 0x1EC);
    VC(PTR(p, 0x8))->v10(car);
    VC(PTR(p, 0xC))->v10(car);
}

u32 HybridDriver::Flags(void) {
    u8* p = (u8*)this;
    if (!fn_801B526C(p)) {
        if (PTR(p, 0x4) && fn_801769F8()) {
            VP(PTR(p, 0x4))->v78(0);
        }
        if (PTR(p, 0x10)) {
            u32 f = VC(PTR(p, 0x10))->v14();
            if (!VU8(p, 0x34)) {
                f |= 0x10;
            }
            if (lbl_8055E521) {
                f |= 0x100;
            }
            return f;
        }
        return 4;
    }
    return 0;
}

void Recorder::SetRumble(int a) {
    u8* p = (u8*)this;
    VP(PTR(p, 0x4))->v78(a); }

#define p ((u8*)this)  // the parameter itself: a named u8* copy of this recolours it (retail registers)
void HybridDriver::Start(void) {
    fn_80191084(p, 0);
    fn_801B6650(PTR(p, 0x20));
    u32 cur = fn_80191100(p);
    if (cur != U32(p, 0x8)) {
        fn_80191084(p, U32(p, 0x8));
    } else {
        fn_80191084(p, U32(p, 0xC));
    }
    fn_80191084(p, cur);
    if (U8(p, 0x38)) {
        U8(p, 0x38) = 0;
        fn_802793B0(PTR(p, 0x28));
    }
    VC(p)->v30();
}
#undef p

void HybridDriver::Stop(void) {
    u8* p = (u8*)this;
    fn_80191084(p, 0); }

// Modes 0 and 1 (meaning not known yet) are compared as an enum.
enum PlayMode { PLAY_MODE_0, PLAY_MODE_1 };

int fn_80190368(int x) {
    int r = 0;
    if ((PlayMode)x == PLAY_MODE_1 || (PlayMode)x == PLAY_MODE_0) {
        r = 1;
    }
    return r;
}

}  // extern "C"

// One recorded replay frame (8 bytes).
struct ReplayPkt {
    u16 throttle;
    u8 v10;
    u8 v14;
    u8 v44;
    u8 v48;
    u8 v18 : 4;
    u8 v1C : 4;
    u8 gear : 3;
    u8 v34 : 1;
    u8 v38 : 1;
    u8 v3C : 1;
    u8 v2C : 1;
    u8 v30 : 1;
};

extern "C" u32 fn_80190E00(u8* p);

static inline ReplayPkt* CurPkt(Recorder* r) { return (ReplayPkt*)fn_80190E00((u8*)r); }
static inline void SetThrottle(Recorder* r, int v) { CurPkt(r)->throttle = v; }
static inline void SetV10(Recorder* r, int v) { CurPkt(r)->v10 = v; }
static inline void SetV14(Recorder* r, int v) { CurPkt(r)->v14 = v; }
static inline void SetV18(Recorder* r, int v) { CurPkt(r)->v18 = v; }
static inline void SetV1C(Recorder* r, int v) { CurPkt(r)->v1C = v; }
static inline void SetGear(Recorder* r, u8 v) { CurPkt(r)->gear = v; }
static inline void SetV34(Recorder* r, u8 v) { CurPkt(r)->v34 = v; }
static inline void SetV38(Recorder* r, u8 v) { CurPkt(r)->v38 = v; }
static inline void SetV3C(Recorder* r, u8 v) { CurPkt(r)->v3C = v; }
static inline void SetV2C(Recorder* r, u8 v) { CurPkt(r)->v2C = v; }
static inline void SetV30(Recorder* r, u8 v) { CurPkt(r)->v30 = v; }
static inline void SetV44(Recorder* r, int v) { CurPkt(r)->v44 = v; }
static inline void SetV48(Recorder* r, int v) { CurPkt(r)->v48 = v; }

extern f32 lbl_8055E3C8;  // race time
extern u8 lbl_8055E529;
extern f32 __float_epsilon[];  // FLT_EPSILON
BOOL OptPaTest(void);

extern "C" {
u32 fn_80190E00(u8* p);
int fn_80190D9C(f32 x, f32 lo, f32 hi);
int fn_80190D38(f32 x, f32 lo, f32 hi);
int fn_80190CD0(f32 x, f32 lo, f32 hi);
int fn_801CF064(void);
void fn_801769E0(void);
void fn_80176A34(s32 frame);
int fn_801B851C(s32 car);
int fn_8026B7B4(s32 car);
int fn_802B02C8(u8* p);
int fn_801796C4(void);
void fn_801B86A4(s32 car);

static inline int NearZero(f32 x) { return (f32)__fabs(x) <= __float_epsilon[0]; }

#define p ((u8*)this)  // the parameter itself: a named u8* copy of this recolours it (retail registers)
void HybridDriver::v48(void) {
    u8* info = ((DeityView*)PTR(p, 0x14))->Info();
    f32 now = lbl_8055E3C8;
    Recorder* rec = (Recorder*)PTR(p, 0x4);
    f32 speed = ((RaceCarRec*)PTR(p, 0x20))->Speed();
    s32 frame = lbl_8055E3C4;
    if (frame >= rec->cap) {
        frame = rec->cap - 1;
    }
    rec->buf = rec->start + frame * 8;
    if (fn_801CF050()) {
        if (frame >= rec->pos) {
            lbl_8055E524 = rec->pos = frame + 1;
        }
        SetThrottle(rec, fn_80190D9C(rec->src->Throttle(speed), kNegThree, 3.0f));
        SetV14(rec, fn_80190D38(rec->src->v14(), 0.0f, 1.0f));
        SetV10(rec, fn_80190D38(rec->src->v10(), 0.0f, 1.0f));
        SetV18(rec, fn_80190CD0(rec->src->v18(), 0.0f, 1.0f));
        SetV1C(rec, fn_80190CD0(rec->src->v1C(), 0.0f, 1.0f));
        SetGear(rec, rec->src->Gear() + 1);
        SetV34(rec, rec->src->v34());
        SetV38(rec, rec->src->v38());
        SetV3C(rec, rec->src->v3C());
        SetV2C(rec, rec->src->v2C());
        SetV30(rec, rec->src->v30());
        SetV44(rec, fn_80190D38(rec->src->v44(), kNegOne, 1.0f));
        SetV48(rec, fn_80190D38(rec->src->v48(), kNegOne, 1.0f));
    }
    if (fn_801CF064() && frame >= rec->pos - 1) {
        fn_801769E0();
        fn_80176A34(lbl_8055E3C4);
    }
    VP(PTR(p, 0x4))->v74();
    if (S32(PTR(p, 0x20), 0x2620)) {
        VP(PTR(p, 0x4))->v7C();
    }
    if (!lbl_8055E529) {
        bool late = lbl_8055E3C8 - F32(PTR(p, 0x20), 0x26D8) < 0.4f;
        if (S32(info, 0x24) == 3 || lbl_8055E528 || OptPaTest() || late) {
            fn_80191084(p, U32(p, 0x8));
        } else if (U8(p, 0x25)) {
            if (!U8(p, 0x26)) {
                fn_80191084(p, U32(p, 0xC));
            } else {
                fn_80191084(p, U32(p, 0x8));
            }
        } else if (S32(info, 0x24) == 1) {
            fn_80191084(p, U32(p, 0x8));
        } else if (!fn_801B851C(S32(p, 0x1C)) || fn_8026B7B4(S32(p, 0x1C))) {
            fn_80191084(p, U32(p, 0x8));
        } else {
            fn_80191084(p, U32(p, 0xC));
        }
    } else {
        fn_80191084(p, U32(p, 0xC));
    }
    if (PTR(p, 0x10) == PTR(p, 0xC)) {
        VC(PTR(p, 0xC))->v48();
    } else {
        VP(PTR(p, 0x4))->v78(0);
    }
    int landed = 0;
    if (VP(PTR(p, 0x4))->v38()) {
        if (!U8(p, 0x24)) {
            landed = 1;
        }
        U8(p, 0x24) = 1;
    } else {
        U8(p, 0x24) = 0;
    }
    bool off = false;
    if (fn_801B83A0()->vE0(S32(p, 0x1C), &off)) {
        if ((VB8(p, 0x34) = !off)) {
            F32(p, 0x2C) = CMAX(F32(p, 0x2C), now);
        }
    } else {
        u8* car = PTR(p, 0x20);
        f32 dist = ((RaceCarRec*)car)->src->Ideal()->travel;
        f32 moved = dist - F32(p, 0x30);
        int stopped;
        int lap = (((RaceCarRec*)PTR(p, 0x20))->src->State()->flags >> 2) & 1;
        car = PTR(p, 0x20);
        stopped = NearZero(F32(car, 0x2690));
        if (stopped) {
            stopped = now - F32(car, 0x26D8) >= 5.0f;
        }
        if (lap && stopped) {
            VU8(p, 0x34) = 0;
        } else if (moved > 106.66667f || moved < -500.0f || !stopped || !fn_802B02C8(car + 0x60)) {
            F32(p, 0x30) = dist;
            F32(p, 0x2C) = CMAX(F32(p, 0x2C), now);
            VU8(p, 0x34) = 1;
        } else if (now - F32(p, 0x2C) > kSix) {
            VU8(p, 0x34) = 0;
            F32(p, 0x2C) = 1.0f + (now - kSix);
            F32(p, 0x30) = 17.777779f + (dist - 106.66667f);
        }
    }
    if (fn_801796C4() || !lbl_8055E522) {
        F32(p, 0x2C) = now;
        VU8(p, 0x34) = 1;
    }
    if (!VU8(p, 0x34) && landed) {
        VU8(p, 0x34) = 1;
        F32(p, 0x2C) = CMAX(F32(p, 0x2C), now);
        fn_801B86A4(S32(p, 0x1C));
    }
    if (!U8(p, 0x26)) {
        u8* other = PTR(PTR(p, 0x18), 0x4);
        if ((other ? VU8(other, 0x34) : 3) == 0) {
            U8(p, 0x26) = 1;
            S32(PTR(p, 0x18), 0xC) = 0;
        }
    }
    if (PTR(p, 0x10) == PTR(p, 0xC) && lbl_8055E523 && !lbl_8055E520) {
        int off = 0;
        for (int i = 0; i < 4; i++) {
            if (!fn_80190368(S32(PTR(p, 0x20) + i * 0x74, 0x2410))) {
                off = 1;
            }
        }
        if (off) {
            lbl_8055E520 = 1;
            fn_801797A8(PTR(p, 0x28), "\x01" "Don't leave the road!");
        }
    }
}
#undef p

int Recorder::vf34(void) {
    u8* p = (u8*)this;
    return PF(PTR(p, 0x8))->b4 != 0; }

f32 Recorder::Throttle(void) {
    u8* p = (u8*)this;
    return PadRange(U8(PTR(p, 0x8), 0x2), 0.0f, 255.0f, 0.0f, 1.0f);
}

f32 Recorder::Brake(void) {
    u8* p = (u8*)this;
    return PadRange(U8(PTR(p, 0x8), 0x3), 0.0f, 255.0f, 0.0f, 1.0f);
}

int Recorder::vf38(void) {
    u8* p = (u8*)this;
    return PF(PTR(p, 0x8))->b3 != 0; }

void Recorder::Stop(void) {
    u8* p = (u8*)this;
    VP(PTR(p, 0x4))->v7C(); }

void Recorder::Buttons(void) {
    u8* p = (u8*)this;
    VP(PTR(p, 0x4))->v74(); }

int fn_80190CD0(f32 x, f32 lo, f32 hi) { return ((int)(0.5f + Lerp(0.0f, 255.0f, Clamp01((x - lo) / (hi - lo)))) >> 4) & 0xF; }

int fn_80190D38(f32 x, f32 lo, f32 hi) { return (int)(0.5f + Lerp(0.0f, 255.0f, Clamp01((x - lo) / (hi - lo)))); }

int fn_80190D9C(f32 x, f32 lo, f32 hi) { return (int)(0.5f + Lerp(0.0f, 65535.0f, Clamp01((x - lo) / (hi - lo)))); }

u32 fn_80190E00(u8* p) { return U32(p, 0x8); }

void fn_80190E08(void) {
    u8** a = lbl_804349A0;
    for (int i = 0; i < lbl_8055E518; i++) {
        VC(*a)->v48();
        a++;
    }
}

void fn_80190E6C(void) { fn_80190E08(); }

void HybridDriver::vf2C(void) {
    u8* p = (u8*)this;
    u8* o = PTR(p, 0x10);
    if (o) {
        VC(o)->v2C();
    }
    fn_801B665C();
}

#define p ((u8*)this)  // the parameter itself: a named u8* copy of this recolours it (retail registers)
void HybridDriver::vf30(void) {
    int z = 0;
    SetMode(p, z);
    lbl_8055E520 = z;
    u8* o = PTR(p, 0x4);
    if (fn_801CF050()) {
        U32(o, 0xC) = z;
        lbl_8055E524 = z;
    }
    S32(o, 0x8) = S32(o, 0x18);
    VC(PTR(p, 0x8))->v30();
    VC(PTR(p, 0xC))->v30();
    fn_80191084(p, 0);
    fn_80191084(p, fn_80191100(p));
    fn_801B6658();
    VU8(p, 0x34) = 0;
    F32(p, 0x30) = 0.0f;
    VU8(p, 0x34) = 1;
    F32(p, 0x2C) = 10.0f;
}
#undef p

void HybridDriver::vf34(void) {
    u8* p = (u8*)this;
    fn_80191758(p); }

void HybridDriver::vf38(const RVec3* dir, RVec3* b, int kind, u8* other) {
    u8* p = (u8*)this;
    VC(PTR(p, 0x10))->v38(); }

#define p ((u8*)this)  // the parameter itself: a named u8* copy of this recolours it (retail registers)
void HybridDriver::vf3C(const RVec3* a_, RVec3* b_, int c, u8* d_) {
    if (!PTR(p, 0x10)) {
        return;
    }
    if (PTR(p, 0x10) == PTR(p, 0xC) && lbl_8055E523 && !lbl_8055E520 && c != 1) {
        lbl_8055E520 = 1;
        fn_801797A8(PTR(p, 0x28), "Hit object!");
    }
    VC(PTR(p, 0x10))->v3C(((u32)a_), ((u32)b_), c, ((u32)d_));
}
#undef p

void HybridDriver::vf40(void) {
    u8* p = (u8*)this;
    fn_801B6664(p); }

void HybridDriver::vf44(void) {
    u8* p = (u8*)this;
    u8* o = PTR(p, 0x10);
    if (o) {
        VC(o)->v44();
    }
}

// The camera/view slots seen as switchable modes: 0x24 enters, 0x28 leaves.
class VSwitch {
public:
    virtual ~VSwitch(void);  // 0x08 (the Hybrid's IDriver: slot 0 is its destructor)
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(void);
    virtual void v1C(void);
    virtual void v20(void);
    virtual void Enter(void);  // 0x24
    virtual void Leave(void);  // 0x28
};

// Makes `next` the active view (offset 0x10): the old one leaves, the new one enters.
void fn_80191084(u8* p, u32 next) {
    VSwitch* cur = (VSwitch*)PTR(p, 0x10);
    if ((VSwitch*)next != cur) {
        if (cur) {
            cur->Leave();
        }
        PTR(p, 0x10) = (u8*)next;
        if (next) {
            ((VSwitch*)next)->Enter();
        }
    }
}

u32 fn_80191100(u8* p) { return U32(p, 0xc); }

int HybridDriver::vf18(int x) {
    u8* p = (u8*)this;
    int a = VC(PTR(p, 0xC))->v18(x);
    return a + VC(PTR(p, 0x8))->v18(x + a);
}

void HybridDriver::vf1C(int x, int y, int* outx, int* outy, f32 t) {
    u8* p = (u8*)this;
    s32 ax;
    s32 ay;
    s32 bx;
    s32 by;
    VC(PTR(p, 0xC))->v1C(x, y, &ax, &ay, t);
    VC(PTR(p, 0x8))->v1C(x + ax, y + ay, &bx, &by, t);
    *outx = ax + bx;
    *outy = ay + by;
    fn_801B6660();
    U8(PTR(p, 0x20), 0x260C) = 0;
}

int HybridDriver::vf20(int x) {
    u8* p = (u8*)this;
    int a = x + VC(PTR(p, 0xC))->v20(x);
    a = a + VC(PTR(p, 0x8))->v20(a);
    return a - x;
}

PlayerDriver::PlayerDriver(void) {
    u8* p = (u8*)this;
    U32(p, 0x34) = 0;
    U32(p, 0x38) = 0;
    S32(p, 0x8) = -1;
}

// Transmission interface at +0x38 of the shifter.
class Gearbox {
public:
    virtual void SetDir(int d);     // 0x08
    virtual f32 Steer(f32 speed);   // 0x0C
    virtual f32 Throttle(void);     // 0x10
    virtual f32 Speed(void);        // 0x14: brake
    virtual f32 v18(void);
    virtual f32 Clutch(void);       // 0x1C
    virtual int v20(void);
    virtual void SetGear(int g);
    virtual void SetRatio(f32 r);
    virtual int Busy(void);
    virtual u8 v30(void);
    virtual int v34(void);
    virtual void v38(void);
    virtual int v3C(void);
    virtual f32 v40(void);  // PhysDriver::vf40 returns f32 (x87 stack on the PC port)
    virtual f32 v44(void);
    virtual f32 v48(void);
    virtual u8 v4C(void);
    virtual void v50(f32 a, f32 b, f32 c);
    virtual void SetRumbleB(f32 a, f32 b, f32 c);
    virtual void v58(f32 a, f32 b);
    virtual void v5C(void);
    virtual void v60(void);
    virtual int v64(void);
    virtual u8 Kind(void);
    virtual int ParamSection(void);
    virtual void v70(void);
    virtual void v74(void);
    virtual void v78(int on);
};

extern u32 lbl_8055D430;
int fn_801333E8(int section, const char* name, bool* v);
int fn_8018B06C(u8* body);

static inline BOOL OptHardcore(void) { return (lbl_8055D430 & 1) && (lbl_8055D430 & 4); }

void PlayerDriver::Start(void) {
    u8* p = (u8*)this;
    Gearbox* gb = (Gearbox*)PTR(p, 0x38);
    U8(PTR(p, 0x34), 0x2680) = gb->Kind();
    S32(PTR(p, 0x34), 0x267C) = 0;
    F32(PTR(p, 0x34), 0x5F0) = 1.0f;
    bool autoShift = true;
    B8(p, 0x4) = autoShift;
    fn_801333E8(((Gearbox*)PTR(p, 0x38))->ParamSection(), "auto_shifting", (bool*)(p + 0x4));
    B8(p, 0x5) = true;
    fn_801333E8(((Gearbox*)PTR(p, 0x38))->ParamSection(), "auto_clutch", (bool*)(p + 0x5));
    if (B8(p, 0x4)) {
        B8(p, 0x5) = true;
    }
    if (!B8(p, 0x4)) {
        autoShift = false;
    }
    int mode = 0;
    if (autoShift) {
        mode = 4;
    }
    S32(p, 0x30) = mode;
    bool stability = true;
    bool braking = true;
    bool hardcore = false;
    fn_801333E8(((Gearbox*)PTR(p, 0x38))->ParamSection(), "arc_stability_help", &stability);
    fn_801333E8(((Gearbox*)PTR(p, 0x38))->ParamSection(), "arcade_braking_help", &braking);
    fn_801333E8(((Gearbox*)PTR(p, 0x38))->ParamSection(), "hardcore_mode", &hardcore);
    if (OptHardcore()) {
        hardcore = true;
    }
    if (hardcore) {
        stability = false;
        braking = false;
    }
    ((Gearbox*)PTR(p, 0x38))->SetGear(fn_8018B06C(PTR(p, 0x34)));
    ((Gearbox*)PTR(p, 0x38))->SetRatio(F32(PTR(p, 0x34), 0x25F4));
    S32(p, 0x8) = S32(PTR(p, 0x38), 0x20);
    F32(p, 0x10) = 0.0f;
    F32(p, 0x14) = 0.0f;
    F32(p, 0x18) = 0.0f;
    memset(p + 0x20, 0, 0x10);
}

void Recorder::SetThrottle(f32 t) {
    u8* p = (u8*)this;
    VP(PTR(p, 0x4))->v28(t); }

void Recorder::SetMaxGear(int v) {
    u8* p = (u8*)this;
    VP(PTR(p, 0x4))->v24(v);
    PF(PTR(p, 0x8))->lvl = v + 1;
}

void Recorder::vf6C(void) {
    u8* p = (u8*)this;
    VP(PTR(p, 0x4))->v6C(); }

u8 Recorder::vf68(void) {
    u8* p = (u8*)this;
    return U8(p, 0x14); }

void PlayerDriver::Stop(void) {
    u8* p = (u8*)this;
    U8(PTR(p, 0x34), 0x2680) = 0;
    fn_8018AE24(PTR(p, 0x34), 0);
    U8(PTR(p, 0x34), 0x260E) = 0;
    VP(PTR(p, 0x38))->v78(0);
    U8(PTR(p, 0x34), 0x260C) = 0;
    S32(p, 0x8) = -1;
}

void PlayerDriver::Init(void* car_, void* q) {
    u8* p = (u8*)this;
    u8* car = (u8*)car_;
    PTR(p, 0x34) = car;
    S32(p, 0x3C) = S32(car, 0x1EC);
    ((VPa*)p)->v24();
    F32(p, 0x10) = 0.0f;
    F32(p, 0x14) = 0.0f;
    F32(p, 0x18) = 0.0f;
    memset(p + 0x20, 0, 0x10);
    F32(p, 0x1C) = 1.0f;
    fn_801331C4(VP(PTR(p, 0x38))->v70(), "ff_jolt", p + 0x1C);
}

void Recorder::vf70(void) {
    u8* p = (u8*)this;
    VP(PTR(p, 0x4))->v70(); }

void PlayerDriver::SetCar(void* car_) {
    u8* p = (u8*)this;
    u8* car = (u8*)car_;
    PTR(p, 0x34) = car;
    S32(p, 0x3C) = S32(car, 0x1EC);
}

PlayerDriver::~PlayerDriver(void) {
    u8* p = (u8*)this;
    U32(p, 0x38) = 0;
}

void PlayerDriver::vf30(void) {
    u8* p = (u8*)this;
    VP(PTR(p, 0x38))->v24(0);
    if (S32(p, 0x30)) {
        S32(p, 0x30) = 4;
    }
    S32(p, 0xC) = -1;
    F32(p, 0x40) = 0.0f;
}

// Per-player body-part transforms (lbl_804A16E8).
struct PartXf {
    f32 pos[3];  // offset 0x0
    f32 scl[3];  // offset 0xC
    f32 a;       // offset 0x18
    f32 b;       // offset 0x1C
};

struct PlayerRec {
    char name[0x20];   // offset 0x0
    PartXf parts[4];   // offset 0x20
};

extern PlayerRec lbl_804A16E8[];
char* strcpy(char* d, const char* s);

static inline void ResetPart(PartXf* x) {
    x->pos[0] = x->pos[1] = x->pos[2] = 0.0f;
    x->scl[0] = x->scl[1] = x->scl[2] = 1.0f;
    x->a = 1.0f;
    x->b = 1.0f;
}

void fn_80191758(u8* p) {
    strcpy(lbl_804A16E8[S32(p, 0x28)].name, (const char*)(PTR(p, 0x20) + 0x234));
    ResetPart(&lbl_804A16E8[S32(p, 0x28)].parts[0]);
    ResetPart(&lbl_804A16E8[S32(p, 0x28)].parts[1]);
    ResetPart(&lbl_804A16E8[S32(p, 0x28)].parts[2]);
    ResetPart(&lbl_804A16E8[S32(p, 0x28)].parts[3]);
}

}  // extern "C"

// One saved option: where it lives, its size and its key.
struct SaveField {
    void* data;        // offset 0x0
    s32 size;          // offset 0x4
    const char* name;  // offset 0x8
};

class FieldSaver {
public:
    virtual void Save(SaveField* fields);  // 0x08
};

extern "C" {
int fn_8018B06C(u8* body);

void PlayerDriver::vf4C(FieldSaver* s) {
    u8* p = (u8*)this;
    SaveField f[3] = {
        {p + 4, 1, "auto_shifting"},
        {p + 5, 1, "auto_clutch"},
        {0, 0, 0},
    };
    VP(PTR(p, 0x38))->v24(fn_8018B06C(PTR(p, 0x34)));
    s->Save(f);
}

void fn_80191928(u8* p) {
    if (S32(p, 0x30) == 0) {
        return;
    }
    u8* body = PTR(p, 0x34);
    if (F32(body, 0x2618) > 1.0f) {
        S32(p, 0x30) = 4;
        return;
    }
    f32 speed = ((Gearbox*)PTR(p, 0x38))->Speed();
    int busy = ((Gearbox*)PTR(p, 0x38))->Busy();
    bool moving = speed > 0.1f;
    body = PTR(p, 0x34);
    f32 d = PSVECDotProduct((Vec*)(body + 0x54), (Vec*)(body + 0x118));
    int steady;
    if (S32(p, 0x30) == 4) {
        steady = moving && d < 0.8888889f;
    } else {
        steady = moving && d < 2.6666667f;
    }
    if (!steady || busy) {
        S32(p, 0x30) = 4;
    } else if (S32(p, 0x30) == 4) {
        S32(p, 0x30) = 5;
    } else if (S32(p, 0x30) >= 5) {
        if (++S32(p, 0x30) - 5 == 29) {
            S32(p, 0x30) = 3;
        }
        if (d < -1.3333334f) {
            S32(p, 0x30) = 3;
        }
    }
}

int Recorder::vf2C(void) {
    u8* p = (u8*)this;
    return PF(PTR(p, 0x8))->b1 != 0; }

}  // extern "C"

// A car's damage model (car + 0x270) as the driver sees it.
class DmgView {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual u8* Current(void);  // 0x10
    virtual void v14(void);
    virtual void v18(void);
    virtual void v1C(void);
    virtual void v20(void);
    virtual void v24(void);
    virtual void Inputs(f32* a, f32* b, f32* c);  // 0x28
};

// A rumble/force-feedback channel (fn_801D0BF8).
class Rumble {
public:
    virtual void Play(u8* car, void* msg);  // 0x08
};

struct RumbleMsg {
    u8 kind;
    u8 pad;
};

extern Proxer* lbl_8055E0D0;
extern u8 lbl_8055E52A;
extern u8 lbl_8055E52B;
extern u8 lbl_8055E52C;
extern f32 lbl_8055E3C8;  // race time
extern f32 __float_epsilon[];  // FLT_EPSILON

extern "C" {
f32* fn_8018AE5C(u8* car);
int fn_8018B084(u8* car);
void fn_8018ADA4(u8* car, f32 v);
void fn_8018ADB4(u8* car, f32 v);
void fn_8018ADBC(u8* car, f32 v);
void fn_8018ADC4(u8* car, f32 v);
void fn_8018ADCC(u8* car, f32 v);
void fn_8018ADD4(u8* car, int v);
void fn_8018ADEC(u8* car);
void fn_8018AE2C(u8* car, f32 v);
void fn_8018AE34(u8* car, f32 v);
f32 fn_8018AE54(u8* car);
int fn_801796B4(void);
Rumble* fn_801D0BF8(s32 car, int a);
double fmod(double x, double y);
double sin(double x);
double cos(double x);

static inline f32 VDot(const void* a, const void* b) { return PSVECDotProduct((const Vec*)a, (const Vec*)b); }
static inline f32 VMag(const void* v) { return PSVECMag((const Vec*)v); }
static inline BOOL OptRumble(void) { return (lbl_8055D430 & 1) && (lbl_8055D430 & 8); }

// The player driver's frame: reads the controller into steering, throttle, brake and clutch, applies the
// arcade assists (in-air pitch and roll control, a nose-down push while boosting) and the replay/rumble hooks,
// then works out the force-feedback wobble from the wheels and the car's spin.

/* Helper name unproven */
static inline void SpinXform(f32* out, const f32* in, const f32* m) {
    if (out == in) {
        f32 x = Dot3(in[0], m[0], in[1], m[3], in[2], m[6]);
        f32 y = Dot3(in[0], m[1], in[1], m[4], in[2], m[7]);
        f32 z = Dot3(in[0], m[2], in[1], m[5], in[2], m[8]);
        out[0] = x;
        out[1] = y;
        out[2] = z;
    } else {
        out[0] = Dot3(in[0], m[0], in[1], m[3], in[2], m[6]);
        out[1] = Dot3(in[0], m[1], in[1], m[4], in[2], m[7]);
        out[2] = Dot3(in[0], m[2], in[1], m[5], in[2], m[8]);
    }
}
/* PROVISIONAL (game/buildquery.h, FACTS 2026-11-17 02:20 and 13:40). Between the lean read and the first vector
   statement of the nose-down push retail had a test that only the back end folds, after its first scheduling pass:
   the lean read is scheduled in a block of its own, air is still live there, and air takes f28 instead of sharing
   lean's f30 (every saved FPR after it follows). 1100/1100, same opcodes as retail. What the original tested is
   unknown. Native proof: handoff/dossier/native_lane/playcar_vf48.md. */
/* Helper name unproven: the nose-down push while boosting, as its own inline. Retail colours `lean` second of all
   saved FPRs (f30, before dir): an inline's objects number above the caller's locals, and lean is the first one
   declared here. The -3000 * air factor is written in each product: retail's factor is a code-generation temporary
   (f2), not a named value. Open: retail also keeps `air` alive up to the lean load (native replay, see
   handoff/dossier/native_lane/playcar_vf48.md). */
static inline void NoseDown(u8* p, f32 air, f32 fwd, RVec3& f, RVec3& at2) {
    // Coefficient name unproven: this named scale preserves retail's first-pass load order.
    f32 forceScale = -3000.0f;
    f32 lean;
    f32 q;
    u8* c3;
    c3 = PTR(p, 0x34);
    lean = F32(c3, 0x25F0);
    if (BuildMode() == kBuildRetail) {
        PcAddScaled(&f, &F32(c3, 0x54), &F32(c3, 0x48), 0.0f);
        f32 b;
        f.x *= forceScale * air;
        f.y *= forceScale * air;
        f.z *= forceScale * air;
        b = ((Gearbox*)PTR(p, 0x38))->Speed();
        q = lean * lean;
        f.x *= b;
        f.y *= b;
        f.z *= b;
        q = 1.0f - q;
        f.x *= q;
        f.y *= q;
        f.z *= q;
        PcAddScaled(&at2, &F32(c3, 0x60), &F32(c3, 0x54), -2.0f);
        if (fwd > -3.5555556f) {
            ((RigidBody*)PTR(p, 0x34))->AddForceAt(&f, &at2);
        }
    }
}
// Helper name unproven: a by-value double parameter owns the wheel-sample abs result;
// narrowing its return preserves retail's FRSP temporaries in the real four-sample loop.
static inline f32 NarrowSample(f64 absolute) { return absolute; }
void PlayerDriver::vf48(void) {
    u8* p = (u8*)this;
    RVec3 rx;
    f32 cl;
    RVec3 at;
    int i;
    RVec3 f;
    f32 dir;
    f32 c;
    RVec3 at2;
    f32 grip;
    f32 spin[9];
    f32 side;
    f32 st;
    f32 air;
    f32 sn;
    f32 s;
    u8* car;
    RVec3 f2;
    f32 wob;
    f32 ax;
    f32 ay;
    f32 az;
    RVec3 w;
    f32 lim;
    f32 ib;
    RVec3 u;
    int hold;
    f32 yaw;
    fn_80191928(p);
    f32 fwd;
    f32 ia[2];
    lbl_8055E51C = VMag(fn_8018AE5C(PTR(p, 0x34)));
    f32 ic;
    car = PTR(p, 0x34);
    f32 rough;
    fwd = VDot(car + 0x118, car + 0x54);
    f32 sp;
    f32 ga;
    f32 engaged = ((Gearbox*)PTR(p, 0x38))->v64() ? 1.0f : 0.0f;
    f32 sum;
    if (!((fn_801769CC()->flags & 2))) {
        dir = 1.0f;
    } else {
        dir = kNegOne;
    }
    ((Gearbox*)PTR(p, 0x38))->SetDir(fn_8018B084(PTR(p, 0x34)));
    lbl_8055E0D0->SortCar(S32(p, 0x3C));
    car = PTR(p, 0x34);
    double speed = __fabs(VDot(car + 0x118, car + 0x54));
    sp = (f32)speed;
    f32 mag;
    u8* c4;
    f32 initialSteer = 0.0f;
    st = ((Gearbox*)PTR(p, 0x38))->Steer(sp);
    fn_8018ADA4(PTR(p, 0x34), dir * (initialSteer + st));
    if (((Gearbox*)PTR(p, 0x38))->Clutch() < 0.25f) {
        fn_8018ADB4(PTR(p, 0x34), ((Gearbox*)PTR(p, 0x38))->Throttle());
    } else {
        fn_8018ADB4(PTR(p, 0x34), 0.0f);
    }
    U8(PTR(p, 0x34), 0x260E) = ((Gearbox*)PTR(p, 0x38))->v30();
    if (fn_801769CC()->type & 0x20000) {
        air = 1.0f;
    } else {
        air = 0.5f;
    }
    if (lbl_8055E3C4 - S32(p, 0xC) > 10 && F32(car = PTR(p, 0x34), 0x25F4) > 0.9f && fwd < 4.4444447f &&
        S32(car, 0x2620) >= 3 && !lbl_8055E52A) {
        f32 roll;
        f32 boost;
        f32 m;
        PcAddScaled(&rx, &F32(car, 0x54), &F32(car, 0x48), 0.0f);
        f32 g;
        f32 steer;
        f32 k;
        steer = MaxF(MinF(1.0f, F32(PTR(p, 0x34), 0x25F0)), kNegOne);
        u8* c2;
        c2 = PTR(p, 0x34);
        boost = MinF(1.0f, (F32(c2, 0x25D4) - 1.7777778f) / 3.5555556f);
        boost = MaxF(boost, 0.0f);
        f32 low;
        f32 high;
        high = 1.0f;
        low = 0.0f;
        f32 range = high - low;
        f32 pr = steer * -F32(c2, 0x128);
        roll = MaxF(MinF(1.0f, pr * -20.0f), 0.0f);
        k = 1.0f - (range * roll) * (steer * steer);
        rx.x *= k;
        rx.y *= k;
        f32 slow;
        rx.z *= k;
        slow = MaxF(MinF(1.0f, 1.0f - F32(PTR(p, 0x34), 0x25D4) / 17.777779f), 0.0f);
        g = slow * (-0.1f * (steer * FAbs(steer)));
        rx.x += F32(car, 0x3C) * g;
        rx.y = rx.y + F32(car, 0x40) * g;
        rx.z = rx.z + F32(car, 0x44) * g;
        f32 tr = F32(PTR(p, 0x34), 0x25F8);
        m = 30000.0f * air * tr;
        rx.x *= m;
        rx.y *= m;
        rx.z *= m;
        PcAddScaled(&at, &F32(car, 0x60), &F32(car, 0x54), -2.0f);
        if (fn_8018B06C(PTR(p, 0x34)) < 0) {
            if ((f32)__fabs(fwd) < 4.4444447f) {
                rx.x *= kNegOne;
                rx.y *= kNegOne;
                rx.z *= kNegOne;
            } else {
                rx.x *= 0.0f;
                rx.y *= 0.0f;
                rx.z *= 0.0f;
            }
        }
        ((RigidBody*)PTR(p, 0x34))->AddForceAt(&rx, &at);
    }
    if (S32(PTR(p, 0x34), 0x2620) >= 3 && ((Gearbox*)PTR(p, 0x38))->Speed() > 0.1f) {
        NoseDown(p, air, fwd, f, at2);
    }
    if (S32(p, 0x30) != 3) {
        fn_8018ADBC(PTR(p, 0x34), ((Gearbox*)PTR(p, 0x38))->Speed());
    } else {
        fn_8018ADBC(PTR(p, 0x34), 0.0f);
    }
    fn_8018ADCC(PTR(p, 0x34), ((Gearbox*)PTR(p, 0x38))->Clutch());
    fn_8018AE2C(PTR(p, 0x34), ((Gearbox*)PTR(p, 0x38))->v44());
    fn_8018AE34(PTR(p, 0x34), dir * ((Gearbox*)PTR(p, 0x38))->v48());
    cl = ((Gearbox*)PTR(p, 0x38))->v18();
    if (((Gearbox*)PTR(p, 0x38))->v34() || S32(p, 0x30) == 3) {
        fn_8018ADD4(PTR(p, 0x34), -1);
        car = PTR(p, 0x34);
        if (VDot(car + 0x118, 0x54 + car) < 6.6666665f ||
            ((Gearbox*)PTR(p, 0x38))->Throttle() > 0.6f) {
            fn_8018ADB4(PTR(p, 0x34), MaxF(1.0f, ((Gearbox*)PTR(p, 0x38))->Throttle()));
        } else {
            fn_8018ADBC(PTR(p, 0x34), 1.0f);
            cl = 0.0f;
            if (F32(PTR(p, 0x34), 0x25D0) < 3500.0f) {
                fn_8018ADB4(PTR(p, 0x34), MaxF(1.0f, ((Gearbox*)PTR(p, 0x38))->Throttle()));
            }
        }
    } else if (B8(p, 0x4)) {
        if (F32(PTR(p, 0x34), 0x25CC) > 0.0f || fn_8018B06C(PTR(p, 0x34)) > 0 ||
            ((Gearbox*)PTR(p, 0x38))->Throttle() > 0.5f) {
            fn_8018ADEC(PTR(p, 0x34));
        }
    } else {
        fn_8018ADD4(PTR(p, 0x34), ((Gearbox*)PTR(p, 0x38))->v20());
        ((Gearbox*)PTR(p, 0x38))->SetGear(fn_8018B06C(PTR(p, 0x34)));
    }
    c = 1.0f - ((Gearbox*)PTR(p, 0x38))->Clutch();
    fn_8018ADC4(PTR(p, 0x34), MaxF(cl, c));
    if (lbl_8055E52B && ((Gearbox*)PTR(p, 0x38))->v3C()) {
        int calm;
        calm = (f32)__fabs(F32(PTR(p, 0x34), 0x2690)) <= __float_epsilon[0];
        if (!lbl_8055E52C && (lbl_8055E52B || calm)) {
            fn_801B86A4(S32(p, 0x3C));
        }
        lbl_8055E52C = 1;
    } else {
        lbl_8055E52C = 0;
    }
    hold = ((Gearbox*)PTR(p, 0x38))->Busy();
    if (fn_801796B4() || fn_801796C4()) {
        fn_8018ADBC(PTR(p, 0x34), 1.0f);
        fn_8018ADB4(PTR(p, 0x34), 0.0f);
        fn_8018ADA4(PTR(p, 0x34), 0.0f);
        if (!B8(p, 0x4)) {
            fn_8018ADD4(PTR(p, 0x34), 0);
            ((Gearbox*)PTR(p, 0x38))->SetGear(0);
        }
        fn_8018AE24(PTR(p, 0x34), 0);
    } else {
        fn_8018AE24(PTR(p, 0x34), hold);
        if (OptRumble()) {
            Rumble* r;
            r = fn_801D0BF8(S32(p, 0x3C), 0);
            if (r) {
                RumbleMsg msg = {0};
                msg.kind = ((Gearbox*)PTR(p, 0x38))->Busy();
                if (!((DmgView*)PTR(PTR(p, 0x34), 0x270))->Current()[0x28]) {
                    r->Play(PTR(p, 0x34), &msg);
                }
            }
        }
    }
    if (((DmgView*)PTR(PTR(p, 0x34), 0x270))->Current()[0x28]) {
        fn_8018ADBC(PTR(p, 0x34), 1.0f);
        fn_8018ADB4(PTR(p, 0x34), 0.0f);
        fn_8018ADA4(PTR(p, 0x34), 0.0f);
    }
    yaw = -VDot(p + 0x10, PTR(p, 0x34) + 0x3C);
    side = (f32)__fabs(VDot(0x10 + p, PTR(p, 0x34) + 0x54));
    if (yaw < 0.0f) {
        s = kNegOne;
    } else {
        s = 1.0f;
    }
    sum = 0.0f;
    mag = 0.0f;
    ((f32*)(0x20 + p))[lbl_8055E3C4 % 4] = (yaw + s * (0.4f * side)) * 0.1f;
    for (i = 0; i < 4; i++) {
        f32 h;
        h = ((f32*)(p + 0x20))[i];
        sum = sum + h;
        mag = mag + NarrowSample(__fabs(h));
    }
    wob = (sum > 0.0f ? 1.0f : kNegOne) * mag;
    u8* c5;
    int odd;
    f32 gb;
    wob *= F32(p, 0x1C);
    wob *= 0.009f;
    c4 = PTR(p, 0x34);
    f32 gr = F32(c4, 0x2570);
    f32 gl = F32(c4, 0x24FC);
    grip = gl + gr;
    rough = VMag(c4 + 0x26B4);
    lim = MinF(1.0f, Mul(10.0f, F32(PTR(p, 0x34), 0x264C)));
    ga = 0.033000004f * rough + lim;
    F32(p, 0x40) = 0.8f * F32(p, 0x40) + 0.0039600004f * rough;
    gb = F32(p, 0x40) + 0.2f * MaxF(MinF(1.0f, grip), 0.0f);
    fn_8018AE54(PTR(p, 0x34));
    odd = (lbl_8055E3C4 >> 1) & 1;
    if (!fn_801796B4() && fn_801CF050() && !fn_801796C4() &&
        NearZero(F32(PTR(p, 0x34), 0x2690))) {
        const f32* v;
        f32 buzz;
        f32 str;
        f32 sr;
        f32 airt;
        c5 = PTR(p, 0x34);
        fmod(lbl_8055E3C8, 1.0);
        buzz = (0.25f * ga + 0.25f * gb) * (odd ? kNegOne : 1.0f);
        ((Gearbox*)PTR(p, 0x38))->v50(0.0f, fn_8018AE4C(c5) * 0.25f, wob + buzz);
        f32 turn;
        c5 = PTR(p, 0x34);
        str = 1.0f;
        sr = F32(c5, 0x25D4) / 4.4444447f;
        if (str < sr) {
            sr = str;
        }
        str *= MaxF(sr, 0.0f);
        PcScaleTo(&w, &F32(c5, 0x124), -0.05f);
        /* The spin vector's length reuses mag (the wobble sum above is finished): a second lifetime of a reused
           variable is numbered above every named local, which is where retail colours it (f24, obtained before
           cl / grip / yaw / initialSteer take it). */
        mag = VMag(&w);
        f32 slip;
        if (mag > 0.0001f) {
            f32 inv;
            inv = 1.0f / mag;
            f32 t;
            f32 yDiag;
            f32 zDiag;
            f32 xx;
            f32 yy;
            f32 zz;
            f32 cs;
            ax = inv * w.x;
            ay = inv * w.y;
            az = inv * w.z;
            sn = sin(mag);
            cs = cos(mag);
            /* The axis-angle rows in rigid's exact form (0fccfbd8b): squares named, t*ax and t*ay written in each
               entry, the y and z diagonals computed into named locals ahead of their rows. The rows still go
               through AxisRow: binding spin's elements by reference is what makes spin a referenced object, so
               SpinXform's matrix reads become parameter objects (loaded before the vector reads, as in retail). */
            xx = ax * ax;
            yy = ay * ay;
            zz = az * az;
            t = 1.0f - cs;
            AxisRow(spin[0], spin[1], spin[2], cs + t * xx, ay * (t * ax) + sn * az, az * (t * ax) - sn * ay);
            yDiag = cs + t * yy;
            AxisRow(spin[3], spin[4], spin[5], ay * (t * ax) - sn * az, yDiag, az * (t * ay) + sn * ax);
            zDiag = cs + t * zz;
            AxisRow(spin[6], spin[7], spin[8], az * (t * ax) + sn * ay, az * (t * ay) - sn * ax, zDiag);
        } else {
            spin[0] = 1.0f;
            spin[1] = 0.0f;
            spin[2] = 0.0f;
            spin[3] = 0.0f;
            spin[4] = 1.0f;
            spin[5] = 0.0f;
            spin[6] = 0.0f;
            spin[7] = 0.0f;
            spin[8] = 1.0f;
        }
        f32 sl;
        f32 ref;
        v = (const f32*)(PTR(p, 0x34) + 0x3C);
        u8* c6;
        SpinXform(&u.x, v, spin);
        f32 sr2 = F32(c5, 0x2488);
        f32 sl2 = F32(c5, 0x2414);
        slip = MaxF(0.0f, 1.0f - 0.5f * (sl2 + sr2));
        str *= slip;
        ref = 0.1f;
        f32 spd = F32(PTR(p, 0x34), 0x25D4);
        ref = ref > spd ? ref : spd;
        turn = VDot(0x118 + PTR(p, 0x34), &u) / ref;
        turn = MaxF(MinF(0.9f, 10.0f * turn), -0.9f);
        c6 = PTR(p, 0x34);
        sl = MaxF(MinF(1.0f, (f32)__fabs(F32(c6, 0x25F0)) / 0.25f), 0.0f);
        turn *= sl;
        f32 coefficient = 0.9f;
        airt = MaxF(F32(c6, 0x23B8), 0.0f) + MaxF(F32(c6, 0x242C), 0.0f);
        str *= MinF(19.68504f * airt, 1.0f);
        ((Gearbox*)PTR(p, 0x38))->SetRumbleB(str, turn, coefficient);
        ((Gearbox*)PTR(p, 0x38))->v58(ga, gb);
        ((Gearbox*)PTR(p, 0x38))->v78(1);
    } else {
        ((Gearbox*)PTR(p, 0x38))->v78(0);
    }
    ((Gearbox*)PTR(p, 0x38))->v40();
    U8(PTR(p, 0x34), 0x260C) = ((Gearbox*)PTR(p, 0x38))->v4C();
    F32(p, 0x10) = 0.0f;
    F32(p, 0x14) = 0.0f;
    F32(p, 0x18) = 0.0f;
    ia[0] = 0.0f;
    ia[1] = 0.0f;
    ib = 0.0f;
    ((DmgView*)PTR(PTR(p, 0x34), 0x270))->Inputs(ia, &ib, &ic);
}


void DeityCar_vf6C(u8* p) {}

void Recorder::vf4C(void) {
    u8* p = (u8*)this;
    VP(PTR(p, 0x4))->v4C(); }

f32 Recorder::vf40(void) {
    u8* p = (u8*)this;
    return VP(PTR(p, 0x4))->v40(); }

void Recorder::SetRumbleC(f32 a, f32 b) {
    u8* p = (u8*)this;
    VP(PTR(p, 0x4))->v58(a, b); }

void Recorder::SetRumbleB(f32 strength, f32 turn, f32 coefficient) {
    u8* p = (u8*)this;
    VP(PTR(p, 0x4))->SetRumbleB(strength, turn, coefficient);
}

void Recorder::SetRumbleA(f32 a, f32 b, f32 c) {
    u8* p = (u8*)this;
    VP(PTR(p, 0x4))->v50(a, b, c); }

int Recorder::vf3C(void) {
    u8* p = (u8*)this;
    return PF(PTR(p, 0x8))->b2 != 0; }

int Recorder::MaxGear(void) {
    u8* p = (u8*)this;
    return PF(PTR(p, 0x8))->lvl - 1; }

f32 Recorder::Grip(void) {
    u8* p = (u8*)this;
    u8 b = *(PTR(p, 0x8) + 0x6);
    return PadRange((u8)((b & 0xF0) | ((b >> 4) & 0xF)), 0.0f, 255.0f, 0.0f, 1.0f);
}

f32 Recorder::vf48(void) {
    u8* p = (u8*)this;
    return PadRange(U8(PTR(p, 0x8), 0x5), 0.0f, 255.0f, -1.0f, 1.0f);
}

f32 Recorder::vf44(void) {
    u8* p = (u8*)this;
    return PadRange(U8(PTR(p, 0x8), 0x4), 0.0f, 255.0f, -1.0f, 1.0f);
}

int Recorder::vf30(void) {
    u8* p = (u8*)this;
    return PF(PTR(p, 0x8))->b0 != 0; }

f32 Recorder::vf1C(void) {
    u8* p = (u8*)this;
    u8 b = *(PTR(p, 0x8) + 0x6);
    return PadRange((u8)(((b << 4) & 0xF0) | (b & 0xF)), 0.0f, 255.0f, 0.0f, 1.0f);
}

f32 Recorder::Steer(void) {
    u8* p = (u8*)this;
    return PadRange(*(u16*)PTR(p, 0x8), 0.0f, 65535.0f, -3.0f, 3.0f);
}

void Recorder::SetGear(int gear) {
    u8* p = (u8*)this;
    VP(PTR(p, 0x4))->v08(gear); }

u8 Recorder::vf64(void) {
    u8* p = (u8*)this;
    return U8(p, 0x12); }

void PlayerDriver::vf38(const RVec3* dir, RVec3* b, int kind, u8* other) {
    u8* p = (u8*)this;
    S32(p, 0xC) = lbl_8055E3C4; }

void PlayerDriver::vf2C(void) {
    u8* p = (u8*)this;
    U32(fn_80178C90(), 0x0) = 0;
    s32 v = fn_801B2F54(PTR(p, 0x3C));
    S32(fn_80178C90(), 0x4) = v;
    f32 a = F32(PTR(p, 0x34), 0x25CC);
    F32(fn_80178C90(), 0x8) = a;
    f32 b = fn_8018AE3C(PTR(p, 0x34));
    F32(fn_80178C90(), 0xC) = b;
    f32 c = fn_8018AE4C(PTR(p, 0x34));
    F32(fn_80178C90(), 0x10) = c;
}

u32 PlayerDriver::Flags(void) { return 0x20; }

void PlayerDriver::vf3C(const RVec3* v, RVec3* imp_, int a_, u8* other_) {
    u8* p = (u8*)this;
    f32 a = F32(p, 0x10) + v->x;
    f32 b = F32(p, 0x14) + v->y;
    f32 c = F32(p, 0x18) + v->z;
    F32(p, 0x10) = a;
    F32(p, 0x14) = b;
    F32(p, 0x18) = c;
}

void PlayerDriver::vf44(void) {
    u8* p = (u8*)this;
    F32(p, 0x40) = 0.0f; }

void PlayerDriver::vf50(void) {}

u8 Recorder::vf5C(void) {
    u8* p = (u8*)this;
    return U8(p, 0x11); }

u8 Recorder::vf60(void) {
    u8* p = (u8*)this;
    return U8(p, 0x13); }



}  // extern "C"

