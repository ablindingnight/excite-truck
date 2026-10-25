// Player car driver (0x8018FB18-0x8019309C): the PlayerDriver/LocalCar side of the car code - input
// to throttle/steer/brake, replay packet recording ("replaypkt"), force-feedback jolts ("ff_jolt"),
// the hybrid control mode, and the per-car wrapper objects that forward to the physics car. Written
// against raw field offsets; the large routines are still stubs. Names are guesses apart from the
// strings.
#include "game/common.h"
#include <string.h>
#include <revolution/MTX/vec.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define B8(p, o) (*(bool*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

struct RVec3 {
    f32 x, y, z;
};

struct PFlags {
    u8 lvl : 3;
    u8 b4 : 1;
    u8 b3 : 1;
    u8 b2 : 1;
    u8 b1 : 1;
    u8 b0 : 1;
};

#define PF(p) ((PFlags*)((u8*)(p) + 7))

class VPx {
public:
    virtual void v08(void);
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
    virtual void v4C(void);
    virtual void v50(void);
    virtual void v54(void);
    virtual void v58(void);
    virtual void v5C(void);
    virtual void v60(void);
    virtual void v64(void);
    virtual void v68(void);
    virtual void v6C(void);
    virtual u8* v70(void);
    virtual void v74(void);
    virtual void v78(int a);
    virtual void v7C(void);
    virtual void v80(int del);
};

#define VP(p) ((VPx*)(p))

class VDtor {
public:
    virtual ~VDtor(void);
};

extern void* lbl_80333018[];
extern f32 lbl_8055A468;
extern f32 lbl_8055A46C;

static inline void SetMode(u8* p, int on) {
    B8(p, 0x25) = on;
    B8(p, 0x26) = !B8(p, 0x25);
    U8(p, 0x24) = 1;
}

// The same interface viewed with an argument-less slot 0x24.
class VPa {
public:
    virtual void s08(void);
    virtual void s0C(void);
    virtual void s10(void);
    virtual void s14(void);
    virtual void s18(void);
    virtual void s1C(void);
    virtual void s20(void);
    virtual void v24(void);
};

extern const f32 lbl_8056208C;
extern const f32 lbl_80562090;
extern const f32 lbl_80562094;
extern const f32 lbl_805620AC;
extern const f32 lbl_80562138;
#define kZero lbl_8056208C
#define kOne lbl_80562090
#define kNegOne lbl_80562094
#define k255 lbl_805620AC
#define kTwo lbl_80562138
extern const f32 lbl_80562084;
extern const f32 lbl_805620A4;
extern const f32 lbl_805620B8;
#define kNegThree lbl_80562084
#define kSix lbl_805620A4
#define k65535 lbl_805620B8

#define CMIN(a, b) ((a) < (b) ? (a) : (b))
#define CMAX(a, b) ((a) > (b) ? (a) : (b))

static inline f32 Clamp01(f32 x) {
    x = 1.0f < x ? 1.0f : x;
    return x > 0.0f ? x : 0.0f;
}

static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

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
extern void* lbl_80332FC4[];

u8* lbl_804349A0[4];
u8* lbl_804349B0[4];

ModuleTag lbl_8055E508;
UnkStaticFFFF lbl_8055E510;
UnkStaticFFFF lbl_8055E514;
s32 lbl_8055E518;
s32 lbl_8055E51C;
u8 lbl_8055E520;
u8 lbl_8055E521;
u8 lbl_8055E522;
u8 lbl_8055E523;
s32 lbl_8055E524;

extern "C" {

}  // extern "C"

class PcGameMode {
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

class Controller;
class SpringCar {
public:
    SpringCar(Controller* c);
    u8 data[0x2844];
};

void* operator new(unsigned long n, const char* tag);

extern "C" {
u8* fn_801769CC(void);
PcGameMode* fn_801B83A0(void);
u32 fn_80134E90(void);
u32 fn_80134EC8(void);
u32 fn_80135078(void);
u8* fn_801912B4(u8* p);
u8* fn_8018FD88(u8* p, u32 a, u32 b);

// Builds the local player's car: driver, hybrid controller and the car itself.
SpringCar* fn_8018FB18(void) {
    u8* drv;
    u8* hyb;
    u8* prof = fn_801769CC();
    u32 ctl = fn_801B83A0()->PlayerController();
    lbl_8055E522 = (U32(prof, 0x4) >> 8) & 1;
    if (U32(prof, 0x4) == 0x180304) {
        lbl_8055E522 = 0;
    }
    if (!ctl) {
        if (U32(prof, 0x4) & 0x100) {
            ctl = fn_80134E90();
        } else if (U32(prof, 0x4) == 0x180201) {
            ctl = fn_80134EC8();
        } else {
            ctl = fn_80135078();
        }
    }
    drv = (u8*)operator new(0x44, "PlayerDriver");
    if (drv) {
        drv = fn_801912B4(drv);
    }
    hyb = (u8*)operator new(0x3C, "Hybrid");
    if (hyb) {
        hyb = fn_8018FD88(hyb, ctl, (u32)drv);
    }
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

u8* fn_8018FD88(u8* p, u32 a, u32 b) {
    *(void**)p = lbl_80333018;
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
    f32 t = 0.001f * (f32)fn_801293A4();
    f32 v = t - 5.0f;
    lbl_8055A46C = t;
    lbl_8055A468 = 0.5f > v ? 0.5f : v;
    return p;
}

u8* fn_8018FE8C(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_80333018;
        if (fn_801B6668()) {
            fn_801B6654();
        }
        if (PTR(p, 0x4)) {
            VP(PTR(p, 0x4))->v80(1);
        }
        PTR(p, 0x4) = 0;
        delete (VDtor*)PTR(p, 0x8);
        PTR(p, 0x8) = 0;
        delete (VDtor*)PTR(p, 0xC);
        PTR(p, 0xC) = 0;
        if (--lbl_8055E518 == 0) {
            fn_8018FCC4();
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

u8* fn_8018FF84(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

}  // extern "C"

extern void* lbl_80332F40[];  // replay recorder vtable
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

// Car controller interface (driver/hybrid at +0x8/+0xC, controller at +0x14).
class CarCtl {
public:
    virtual void v08(void);
    virtual void Attach(u8* car, u8* q);
    virtual u8* Info(void);
    virtual void v14(void);
    virtual s32 v18(void);
};

extern "C" {
ReplaySrc* fn_8018108C(s32 id);
CarCtl* fn_801B2EC0(s32 id);
}

// The car's replay recorder (0x24): writes packets into the car's replay buffer.
struct Recorder {
    void* vtbl;
    ReplaySrc* src;
    u8* buf;
    s32 pos;
    u8 pad10;
    u8 cfg[4];
    u8 pad15[3];
    u8* start;
    s32 cap;
    s32 id;

    Recorder(s32 n) {
        vtbl = lbl_80332F40;
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

void fn_8018FFC4(u8* p, u8* car, u8* q) {
    S32(p, 0x28) = U8(car, 0x260) & 3;
    Recorder* r = new ("repl_di") Recorder(S32(p, 0x28));
    PTR(p, 0x4) = (u8*)r;
    PTR(PTR(p, 0xC), 0x38) = (u8*)r;
    B8(p, 0x25) = S32(q, 0x5C) == 0;
    B8(p, 0x26) = S32(q, 0x5C) != 0;
    PTR(p, 0x20) = car;
    S32(p, 0x1C) = S32(car, 0x1EC);
    CarCtl* c = fn_801B2EC0(S32(p, 0x1C));
    PTR(p, 0x14) = (u8*)c;
    S32(p, 0x18) = c->v18();
    ((CarCtl*)PTR(p, 0x8))->Attach(car, q);
    ((CarCtl*)PTR(p, 0xC))->Attach(car, q);
    if (OptProfile()) {
        lbl_8055E528 = 1;
    }
}

void fn_80190184(u8* p, u8* car) {
    PTR(p, 0x20) = car;
    S32(p, 0x1C) = S32(car, 0x1EC);
    VP(PTR(p, 0x8))->v10(car);
    VP(PTR(p, 0xC))->v10(car);
}

u32 fn_801901F0(u8* p) {
    if (!fn_801B526C(p)) {
        if (PTR(p, 0x4) && fn_801769F8()) {
            VP(PTR(p, 0x4))->v78(0);
        }
        if (PTR(p, 0x10)) {
            u32 f = VP(PTR(p, 0x10))->v14();
            if (!U8(p, 0x34)) {
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

void fn_801902A0(u8* p, int a) { VP(PTR(p, 0x4))->v78(a); }

void fn_801902B4(u8* p) {
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
    VP(p)->v30();
}

void fn_80190360(u8* p) { fn_80191084(p, 0); }

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

class RaceCarRec {
public:
    f32 Speed(void) const;
};

// Track position tracker of a car (car + 0x270).
class CarTrackPos {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual u32* Flags(void);
    virtual void v14(void);
    virtual u8* Segment(void);
};

extern f32 lbl_8055E3C8;  // race time
extern u8 lbl_8055E529;
extern f32 lbl_80558DC4[];  // FLT_EPSILON
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

static inline int NearZero(f32 x) { return (f32)__fabs(x) <= lbl_80558DC4[0]; }

void fn_80190388(u8* p) {
    u8* info = ((CarCtl*)PTR(p, 0x14))->Info();
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
        SetV14(rec, fn_80190D38(rec->src->v14(), kZero, kOne));
        SetV10(rec, fn_80190D38(rec->src->v10(), kZero, kOne));
        SetV18(rec, fn_80190CD0(rec->src->v18(), kZero, kOne));
        SetV1C(rec, fn_80190CD0(rec->src->v1C(), kZero, kOne));
        SetGear(rec, rec->src->Gear() + 1);
        SetV34(rec, rec->src->v34());
        SetV38(rec, rec->src->v38());
        SetV3C(rec, rec->src->v3C());
        SetV2C(rec, rec->src->v2C());
        SetV30(rec, rec->src->v30());
        SetV44(rec, fn_80190D38(rec->src->v44(), kNegOne, kOne));
        SetV48(rec, fn_80190D38(rec->src->v48(), kNegOne, kOne));
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
        VP(PTR(p, 0xC))->v48();
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
        if ((B8(p, 0x34) = !off)) {
            F32(p, 0x2C) = CMAX(F32(p, 0x2C), now);
        }
    } else {
        u8* car = PTR(p, 0x20);
        f32 dist = F32(((CarTrackPos*)PTR(car, 0x270))->Segment(), 0x48);
        f32 moved = dist - F32(p, 0x30);
        int stopped;
        int lap = (*((CarTrackPos*)PTR(PTR(p, 0x20), 0x270))->Flags() >> 2) & 1;
        car = PTR(p, 0x20);
        stopped = NearZero(F32(car, 0x2690));
        if (stopped) {
            stopped = now - F32(car, 0x26D8) >= 5.0f;
        }
        if (lap && stopped) {
            U8(p, 0x34) = 0;
        } else if (moved > 106.66667f || moved < -500.0f || !stopped || !fn_802B02C8(car + 0x60)) {
            F32(p, 0x30) = dist;
            F32(p, 0x2C) = CMAX(F32(p, 0x2C), now);
            U8(p, 0x34) = 1;
        } else if (now - F32(p, 0x2C) > kSix) {
            U8(p, 0x34) = 0;
            F32(p, 0x2C) = kOne + (now - kSix);
            F32(p, 0x30) = 17.777779f + (dist - 106.66667f);
        }
    }
    if (fn_801796C4() || !lbl_8055E522) {
        F32(p, 0x2C) = now;
        U8(p, 0x34) = 1;
    }
    if (!U8(p, 0x34) && landed) {
        U8(p, 0x34) = 1;
        F32(p, 0x2C) = CMAX(F32(p, 0x2C), now);
        fn_801B86A4(S32(p, 0x1C));
    }
    if (!U8(p, 0x26)) {
        u8* other = PTR(PTR(p, 0x18), 0x4);
        if ((other ? U8(other, 0x34) : 3) == 0) {
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

int fn_80190B60(u8* p) { return PF(PTR(p, 0x8))->b4 != 0; }

f32 fn_80190B7C(u8* p) {
    f32 t = CMIN(kOne, ((f32)U8(PTR(p, 0x8), 0x2) - kZero) / k255);
    t = CMAX(t, kZero);
    return kZero + t * kOne;
}

f32 fn_80190C04(u8* p) {
    f32 t = CMIN(kOne, ((f32)U8(PTR(p, 0x8), 0x3) - kZero) / k255);
    t = CMAX(t, kZero);
    return kZero + t * kOne;
}

int fn_80190C8C(u8* p) { return PF(PTR(p, 0x8))->b3 != 0; }

void fn_80190CA8(u8* p) { VP(PTR(p, 0x4))->v7C(); }

void fn_80190CBC(u8* p) { VP(PTR(p, 0x4))->v74(); }

int fn_80190CD0(f32 x, f32 lo, f32 hi) { return ((int)(0.5f + Lerp(0.0f, 255.0f, Clamp01((x - lo) / (hi - lo)))) >> 4) & 0xF; }

int fn_80190D38(f32 x, f32 lo, f32 hi) { return (int)(0.5f + Lerp(0.0f, 255.0f, Clamp01((x - lo) / (hi - lo)))); }

int fn_80190D9C(f32 x, f32 lo, f32 hi) { return (int)(0.5f + Lerp(0.0f, 65535.0f, Clamp01((x - lo) / (hi - lo)))); }

u32 fn_80190E00(u8* p) { return U32(p, 0x8); }

void fn_80190E08(void) {
    u8** a = lbl_804349A0;
    for (int i = 0; i < lbl_8055E518; i++) {
        VP(*a)->v48();
        a++;
    }
}

void fn_80190E6C(void) { fn_80190E08(); }

void fn_80190E70(u8* p) {
    u8* o = PTR(p, 0x10);
    if (o) {
        VP(o)->v2C();
    }
    fn_801B665C();
}

void fn_80190EAC(u8* p) {
    int z = 0;
    SetMode(p, z);
    lbl_8055E520 = z;
    u8* o = PTR(p, 0x4);
    if (fn_801CF050()) {
        U32(o, 0xC) = z;
        lbl_8055E524 = z;
    }
    S32(o, 0x8) = S32(o, 0x18);
    VP(PTR(p, 0x8))->v30();
    VP(PTR(p, 0xC))->v30();
    fn_80191084(p, 0);
    fn_80191084(p, fn_80191100(p));
    fn_801B6658();
    U8(p, 0x34) = 0;
    F32(p, 0x30) = 0.0f;
    U8(p, 0x34) = 1;
    F32(p, 0x2C) = 10.0f;
}

void fn_80190F90(u8* p) { fn_80191758(p); }

void fn_80190F94(u8* p) { VP(PTR(p, 0x10))->v38(); }

void fn_80190FA8(u8* p, u32 a, u32 b, int c, u32 d) {
    if (!PTR(p, 0x10)) {
        return;
    }
    if (PTR(p, 0x10) == PTR(p, 0xC) && lbl_8055E523 && !lbl_8055E520 && c != 1) {
        lbl_8055E520 = 1;
        fn_801797A8(PTR(p, 0x28), "Hit object!");
    }
    VP(PTR(p, 0x10))->v3C(a, b, c, d);
}

void fn_80191060(u8* p) { fn_801B6664(p); }

void fn_80191064(u8* p) {
    u8* o = PTR(p, 0x10);
    if (o) {
        VP(o)->v44();
    }
}

// The camera/view slots seen as switchable modes: 0x24 enters, 0x28 leaves.
class VSwitch {
public:
    virtual void v08(void);
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

int fn_80191108(u8* p, int x) {
    int a = VP(PTR(p, 0xC))->v18(x);
    return a + VP(PTR(p, 0x8))->v18(x + a);
}

void fn_80191178(u8* p, s32 x, s32 y, s32* outx, s32* outy, f32 t) {
    s32 ax;
    s32 ay;
    s32 bx;
    s32 by;
    VP(PTR(p, 0xC))->v1C(x, y, &ax, &ay, t);
    VP(PTR(p, 0x8))->v1C(x + ax, y + ay, &bx, &by, t);
    *outx = ax + bx;
    *outy = ay + by;
    fn_801B6660();
    U8(PTR(p, 0x20), 0x260C) = 0;
}

int fn_80191240(u8* p, int x) {
    int a = x + VP(PTR(p, 0xC))->v20(x);
    a = a + VP(PTR(p, 0x8))->v20(a);
    return a - x;
}

u8* fn_801912B4(u8* p) {
    U32(p, 0x34) = 0;
    *(void**)p = lbl_80332FC4;
    U32(p, 0x38) = 0;
    S32(p, 0x8) = -1;
    return p;
}

// Transmission interface at +0x38 of the shifter.
class Gearbox {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual f32 Speed(void);
    virtual void v18(void);
    virtual void v1C(void);
    virtual void v20(void);
    virtual void SetGear(int g);
    virtual void SetRatio(f32 r);
    virtual int Busy(void);
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
    virtual u8 Kind(void);
    virtual int ParamSection(void);
};

extern u32 lbl_8055D430;
int fn_801333E8(int section, const char* name, bool* v);
int fn_8018B06C(u8* body);

static inline BOOL OptHardcore(void) { return (lbl_8055D430 & 1) && (lbl_8055D430 & 4); }

void fn_801912D8(u8* p) {
    Gearbox* gb = (Gearbox*)PTR(p, 0x38);
    U8(PTR(p, 0x34), 0x2680) = gb->Kind();
    S32(PTR(p, 0x34), 0x267C) = 0;
    F32(PTR(p, 0x34), 0x5F0) = kOne;
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

void fn_801914F8(u8* p) { VP(PTR(p, 0x4))->v28(); }

void fn_8019150C(u8* p, int v) {
    VP(PTR(p, 0x4))->v24(v);
    PF(PTR(p, 0x8))->lvl = v + 1;
}

void fn_80191568(u8* p) { VP(PTR(p, 0x4))->v6C(); }

u8 fn_8019157C(u8* p) { return U8(p, 0x14); }

void fn_80191584(u8* p) {
    U8(PTR(p, 0x34), 0x2680) = 0;
    fn_8018AE24(PTR(p, 0x34), 0);
    U8(PTR(p, 0x34), 0x260E) = 0;
    VP(PTR(p, 0x38))->v78(0);
    U8(PTR(p, 0x34), 0x260C) = 0;
    S32(p, 0x8) = -1;
}

void fn_801915FC(u8* p, u8* car) {
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

void fn_80191688(u8* p) { VP(PTR(p, 0x4))->v70(); }

void fn_8019169C(u8* p, u8* car) {
    PTR(p, 0x34) = car;
    S32(p, 0x3C) = S32(car, 0x1EC);
}

u8* fn_801916AC(u8* p, int del) {
    if (p) {
        U32(p, 0x38) = 0;
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

void fn_801916F4(u8* p) {
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
    x->pos[0] = x->pos[1] = x->pos[2] = kZero;
    x->scl[0] = x->scl[1] = x->scl[2] = kOne;
    x->a = kOne;
    x->b = kOne;
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

void fn_80191860(u8* p, FieldSaver* s) {
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
    if (F32(body, 0x2618) > kOne) {
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

int fn_80191A98(u8* p) { return PF(PTR(p, 0x8))->b1 != 0; }

void fn_80191AB4(u8* p) {}  // stub (1100)

void fn_80192BE4(u8* p) {}

void fn_80192BE8(u8* p) { VP(PTR(p, 0x4))->v4C(); }

void fn_80192BFC(u8* p) { VP(PTR(p, 0x4))->v40(); }

void fn_80192C10(u8* p) { VP(PTR(p, 0x4))->v58(); }

void fn_80192C24(u8* p) { VP(PTR(p, 0x4))->v54(); }

void fn_80192C38(u8* p) { VP(PTR(p, 0x4))->v50(); }

int fn_80192C4C(u8* p) { return PF(PTR(p, 0x8))->b2 != 0; }

int fn_80192C68(u8* p) { return PF(PTR(p, 0x8))->lvl - 1; }

// A 4-bit field widened to 8 bits (0xN -> 0xNN).
struct Nibbles {
    u8 hi : 4;
    u8 lo : 4;
};

static inline u8 HiNibble8(u8* p) { return (*p & 0xF0) | ((Nibbles*)p)->hi; }
static inline u8 LoNibble8(u8* p) { return ((*p << 4) & 0xF0) | ((Nibbles*)p)->lo; }

f32 fn_80192C7C(u8* p) {
    f32 t = CMIN(kOne, ((f32)HiNibble8(PTR(p, 0x8) + 0x6) - kZero) / k255);
    t = CMAX(t, kZero);
    return kZero + t * kOne;
}

f32 fn_80192D10(u8* p) {
    f32 t = CMIN(kOne, ((f32)U8(PTR(p, 0x8), 0x5) - kZero) / k255);
    t = CMAX(t, kZero);
    return kNegOne + t * kTwo;
}

f32 fn_80192D98(u8* p) {
    f32 t = CMIN(kOne, ((f32)U8(PTR(p, 0x8), 0x4) - kZero) / k255);
    t = CMAX(t, kZero);
    return kNegOne + t * kTwo;
}

int fn_80192E20(u8* p) { return PF(PTR(p, 0x8))->b0 != 0; }

f32 fn_80192E3C(u8* p) {
    f32 t = CMIN(kOne, ((f32)LoNibble8(PTR(p, 0x8) + 0x6) - kZero) / k255);
    t = CMAX(t, kZero);
    return kZero + t * kOne;
}

f32 fn_80192ED0(u8* p) {
    f32 t = CMIN(kOne, ((f32)*(u16*)PTR(p, 0x8) - kZero) / k65535);
    t = CMAX(t, kZero);
    return kNegThree + t * kSix;
}

void fn_80192F58(u8* p) { VP(PTR(p, 0x4))->v08(); }

u8 fn_80192F6C(u8* p) { return U8(p, 0x12); }

void fn_80192F74(u8* p) { S32(p, 0xC) = lbl_8055E3C4; }

void fn_80192F80(u8* p) {
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

int fn_80193010(void) { return 0x20; }

void fn_80193018(u8* p, const RVec3* v) {
    f32 a = F32(p, 0x10) + v->x;
    f32 b = F32(p, 0x14) + v->y;
    f32 c = F32(p, 0x18) + v->z;
    F32(p, 0x10) = a;
    F32(p, 0x14) = b;
    F32(p, 0x18) = c;
}

void fn_8019304C(u8* p) { F32(p, 0x40) = 0.0f; }

void fn_80193058(u8* p) {}

u8 fn_8019305C(u8* p) { return U8(p, 0x11); }

u8 fn_80193064(u8* p) { return U8(p, 0x13); }



}  // extern "C"
