// AI driver (0x8019309C-0x80198F14): the per-car AI behind each computer opponent - tunables
// ("moveout_timer_f", "pass_hys_min", ...), wake/sleep/pit/wreck messages, racing-line following,
// passing and pushing, overheat and turbo management, and the per-frame steering and throttle.
// Written against raw field offsets (the AIDriver object is 0x30C bytes). Names are guesses apart from
// the strings.
#include "game/common.h"
#include "macros.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))
#define VCALL(T, p, slot) ((T)(*(void**)(*(u8**)(p) + (slot))))

struct RVec3 {
    f32 x, y, z;
};

// A vector copied as words (array member).
struct PVec3 {
    f32 v[3];
};

struct V2 {
    f32 x, y;
};

class VIface {
public:
    virtual void v08(void);
    virtual void v0C(int a);
    virtual u8* v10(void);
    virtual void v14(void);
    virtual u8* v18(void);
    virtual void v1C(void);
    virtual void v20(void);
    virtual void v24(void);
    virtual f32 v28(const char* k);
    virtual void v2C(void);
    virtual void v30(int a);
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
    virtual void vAC(s32 a);
};

class VSelf {
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
};

#define VI(p) ((VIface*)(p))

class RigidBody {
public:
    void AddImpulse(f32 s, const RVec3* a, const RVec3* b);
    void AddTorque(const RVec3* t);
    void AddForceAt(const RVec3* f, const RVec3* at);
};

class Proxer {
public:
    void SortCar(int car);
    void SetActive(int car, u8 on);
};

class AIExceptions {
public:
    u8* Find(f32 t);
};

int IsLoadingActive(void);

extern "C" {
void fn_802E317C(const char* fmt, ...);  // fatal
void fn_802E30C8(const char* fmt, ...);  // warning print
f32 PSVECDotProduct(const void* a, const void* b);
f32 PSVECMag(const void* a);
double sqrt(double x);
double fmod(double x, double y);
f32 fn_80129DC4(int seed, f32 lo, f32 hi);  // random float
int fn_80129D18(int seed, int n);           // random int
void fn_8023E52C(void* a);
u8* fn_8017960C(s32 id);
void fn_801352B8(void* p);
void fn_8013530C(void* p, int del);
void fn_80134CD4(s32 car, void* out);
f32* fn_80137C54(int a);
void* fn_801769CC(void);
int fn_801769B8(void);
u32 fn_801769A8(void);
f32 fn_80134B58(void* tune, f32 def);
void fn_8013B6A4(s32 car, f32 v);
s32 fn_8013B578(s32 car);
int fn_801667F0(void);
void fn_801B30D0(s32 car);
int fn_801B8570(s32 car);
f32 fn_801B306C(void);
void fn_801B30E0(s32 car);
void fn_8018ADB4(void* car, f32 v);
void fn_8018ADAC(void* car, f32 v);
void fn_8018ADBC(void* car, f32 v);
void fn_8018AE24(void* car, int v);
void fn_8018AF04(void* car);
void fn_8018A174(void* car);
void* fn_801B83A0(void);
void fn_8013B744(s32 car);
f32 fn_801671FC(s32 car);
int fn_801B89F8(void);
s32 fn_801B2F24(void);
s32 fn_801B2F40(s32 i);
f32 fn_80137850(u8* p);
void* fn_801B2ED0(s32 id);
s32 fn_801671AC(s32 car);
int fn_8015FBCC(void* car, V2* out, int a, s32 o);
int fn_80167544(s32 car);
}

extern Proxer* lbl_8055E0D0;
extern u32 lbl_8055D430;
extern s32 lbl_8055E3C4;
extern f32 lbl_8055E3C8;
extern f32 lbl_80558DC4[];
extern void* lbl_80333348[];  // AIDriver vtable
extern void* lbl_80333390[];  // IDriver vtable

struct AIVecBase {
    ~AIVecBase(void) { data = 0; }
    u8* data;  // offset 0x0
};

struct AIVec : AIVecBase {
    AIVec(void) {
        cap = 0x80;
        data = buf;
        count = 0;
        unkC = 0;
    }
    ~AIVec(void) {}
    s32 cap;   // offset 0x4
    s32 count; // offset 0x8
    s32 unkC;  // offset 0xC
    u8 buf[0x600];
};

u8* lbl_804348E8[8];  // cars
u8* lbl_804349C0[8];  // AI drivers by car
struct DriverList {
    u8* items[8];
    s32 count;
} lbl_804349E0;

f32 lbl_8055A488 = 100000.0f;
f32 lbl_8055A48C = -100000.0f;

ModuleTag lbl_8055E530;
UnkStaticFFFF lbl_8055E538;
UnkStaticFFFF lbl_8055E53C;
s32 lbl_8055E540;  // race kind
f32 lbl_8055E544;
f32 lbl_8055E548;
f32 lbl_8055E54C;
s32 lbl_8055E550;  // live AI drivers
f32 lbl_8055E554;
f32 lbl_8055E558;
f32 lbl_8055E55C;
f32 lbl_8055E560;
f32 lbl_8055E564;
f32 lbl_8055E568;
f32 lbl_8055E56C;
f32 lbl_8055E570;
bool lbl_8055E574;
s32 lbl_8055E578;
f32 lbl_8055E57C;
u8 lbl_8055E580;
f32 lbl_8055E584;
f32 lbl_8055E588;
AIVec lbl_80434A10;

#define CAR(d) PTR(d, 0x294)
#define CARID(d) S32(d, 0x290)

static inline f32 Clamp01(f32 x) {
    x = 1.0f < x ? 1.0f : x;
    return x > 0.0f ? x : 0.0f;
}

extern "C" {

int fn_8019309C(u8* d, int car, u8 v) {
    u8* o = lbl_804349C0[car];
    int r = 0;
    if (o && U8(o, 0x2A4)) {
        r = 1;
        U8(o, 0x2FC) = CARID(d);
        U8(d, 0x2FD) = v;
    }
    return r;
}

u8 fn_801930DC(u8* d, int car) {
    u8* o = lbl_804349C0[car];
    if (o && U8(o, 0x2A4)) {
        return U8(o, 0x2FD);
    }
    return 0;
}

int fn_80193110(u8* d, int car) {
    if (U8(d, 0x2FC) == (u8)car) {
        U8(d, 0x2FC) = 0xFF;
        return 1;
    }
    return 0;
}

void fn_80193138(u8* d, int car) {
    u8* o = lbl_804349C0[car];
    if (o) {
        U8(o, 0x304) = 20;
    }
}

void fn_8019315C(u8* d, int car) {
    u8* o = lbl_804349C0[car];
    if (o) {
        S32(o, 0x300)++;
    }
}

u8 fn_80193184(u8* d, int car) {
    u8* o = lbl_804349C0[car];
    if (o) {
        return U8(o, 0x2FE);
    }
    return 0;
}

f32 fn_801931AC(f32 t, f32 a, f32 b, f32 len) {
    f32 k = Clamp01(1.0f - t / len);
    return a + k * (b - a);
}

f32 fn_801931EC(f32 t, f32 len) {
    f32 x = 1.0f - t / len;
    f32 lo = lbl_8055E558;
    x = 1.0f < x ? 1.0f : x;
    return x > lo ? x : lo;
}

void fn_8019321C(V2* out, const V2* in, const u8* m) {
    f32 dx = F32(m, 0x18);
    f32 dz = F32(m, 0x20);
    V2 dir;
    dir.x = dx;
    dir.y = dz;
    f32 len = (f32)sqrt(dx * dx + dz * dz);
    len = 0.01f > len ? 0.01f : len;
    f32 inv = 1.0f / len;
    V2 org;
    org.x = F32(m, 0x24);
    org.y = F32(m, 0x2C);
    dir.x *= inv;
    dir.y *= inv;
    V2 side;
    side.x = dir.y;
    side.y = -dir.x;
    V2 r;
    r.x = dir.y * in->x + dir.x * in->y + org.x;
    r.y = (f32)-dir.x * in->x + dir.y * in->y + org.y;
    out->x = r.x;
    out->y = r.y;
}

void fn_8019330C(V2* out, const V2* in, const u8* m) {
    f32 dx = F32(m, 0x18);
    f32 dz = F32(m, 0x20);
    V2 dir;
    dir.x = dx;
    dir.y = dz;
    f32 len = (f32)sqrt(dx * dx + dz * dz);
    len = 0.01f > len ? 0.01f : len;
    f32 inv = 1.0f / len;
    V2 org;
    org.x = F32(m, 0x24);
    org.y = F32(m, 0x2C);
    dir.x *= inv;
    dir.y *= inv;
    V2 side;
    side.x = dir.y;
    side.y = -dir.x;
    V2 rel;
    rel.x = in->x - org.x;
    rel.y = in->y - org.y;
    out->x = rel.x * dir.y + rel.y * (f32)-dir.x;
    out->y = rel.x * dir.x + rel.y * dir.y;
}

void fn_801933FC(u8* d, V2* v) { fn_8019330C(v, v, CAR(d) + 0x70); }

void fn_8019340C(u8* d, RVec3* pt, RVec3* imp, int a, u8* other) {
    int isCar = 0;
    if (other && U32(other, 0x10) == 'PCAR') {
        isCar = 1;
    }
    if (isCar) {
        F32(d, 0x1F4) = F32(d, 0x26C);
        int on = 0;
        if ((lbl_8055D430 & 1) && (lbl_8055D430 & 0x20)) {
            on = 1;
        }
        if (on) {
            f32 m = PSVECDotProduct(pt, pt);
            if (m > 562500.0f) {
                s32 id = S32(other, 0x1EC);
                s32 cur = S32(d, 0x2C4);
                if (cur == id) {
                    RigidBody* rb = (RigidBody*)fn_8017960C(cur);
                    f32 s = 2000.0f + m / 5000.0f;
                    s = s > 5000.0f ? 5000.0f : s;
                    RVec3 v;
                    v.x = 500.0f;
                    v.y = s;
                    v.z = 500.0f;
                    rb->AddImpulse(1.0f, &v, imp);
                    S32(d, 0x2C4) = -1;
                } else {
                    S32(d, 0x2C4) = id;
                }
            }
        }
    }
}

int fn_80193524(u8* a, u8* b) {
    int r = 0;
    if ((f32)__fabs(PSVECDotProduct(a + 0x54, b + 0x54)) < 0.5f) {
        RVec3 d;
        d.x = F32(b, 0x60) - F32(a, 0x60);
        d.y = F32(b, 0x64) - F32(a, 0x64);
        d.z = F32(b, 0x68) - F32(a, 0x68);
        f32 s = PSVECDotProduct(&d, a + 0x54);
        r = s > -PSVECDotProduct(&d, b + 0x54);
    }
    return r;
}

void fn_801935EC(u8* d) {
    F32(d, 0x224) = 0.0f;
    F32(d, 0x228) = 0.0f;
    F32(d, 0x22C) = lbl_8055E3C8;
}

extern const f32 lbl_80562168;  // 0.1
extern const f32 lbl_80562174;  // 2.0
extern const f32 lbl_8056216C;  // 22.2 (80 km/h)
extern const f32 lbl_80562170;  // 2.2 (8 km/h)

void fn_80193604(u8* d, const RVec3* dir, RVec3* b, int kind, u8* other) {
    if (other) {
        if (U32(other, 0x10) == 'PCAR') {
            RVec3 rel;
            u8* car = CAR(d);
            rel.x = F32(other, 0x60) - F32(car, 0x60);
            rel.y = F32(other, 0x64) - F32(car, 0x64);
            rel.z = F32(other, 0x68) - F32(car, 0x68);
            if (PSVECDotProduct(&rel, CAR(d) + 0x54) > 0.0f) {
                f32 s = PSVECDotProduct(other + 0x118, other + 0x54);
                if (s > 4.4444447f) {
                    if ((f32)__fabs(PSVECDotProduct(other + 0x118, other + 0x3C) / s) > 0.176f) {
                        F32(d, 0x2A8) += lbl_80562168;
                    }
                }
            }
        }
        u32 t = U32(other, 0x10);
        if ((t == 'ACAR' || t == 'PCAR') && fn_80193524(CAR(d), other)) {
            F32(d, 0x2A8) += lbl_80562168;
        }
    }
    if (kind < 1000 && PSVECDotProduct(dir, CAR(d) + 0x88) < 0.0f) {
        RVec3 n = *dir;
        f32 m = PSVECMag(&n);
        m = 0.01f > m ? 0.01f : m;
        f32 inv = 1.0f / m;
        n.x *= inv;
        n.y *= inv;
        n.z *= inv;
        if ((f32)__fabs(PSVECDotProduct(&n, CAR(d) + 0x88)) > 0.5f) {
            f32 v = F32(d, 0x270);
            if (v < lbl_8056216C) {
                if (kind != 0x6D || v < lbl_80562170) {
                    if (S32(d, 0x268) != 1 || kind != 0x6D) {
                        F32(d, 0x2A0) += lbl_80562174;
                    }
                }
            }
        }
    }
    if (kind == 0x3EA) {
        F32(d, 0x2A0) += lbl_80562174;
    }
    switch (kind) {
    case 0x6D:
        U8(d, 0x2FE) = 0;
        break;
    }
}

void fn_80193CE0(u8* d);
void fn_801945F8(u8* d, const char* why);

void fn_8019386C(u8* d) { fn_80193CE0(d); }

void fn_80193870(u8* d) { fn_801945F8(d, "console"); }

void fn_80193878(u8* d) {
    RVec3 at;
    RVec3 imp;
    RVec3 pos;
    pos.x = 0.0f;
    pos.y = 0.0f;
    pos.z = 13349.999f;
    u8* car = CAR(d);
    imp.x = 10.0f;
    imp.y = 0.0f;
    imp.z = 0.0f;
    at.x = F32(car, 0x94) + 10.0f;
    at.y = F32(car, 0x98) + 0.0f;
    at.z = F32(car, 0x9C) + 0.0f;
    ((RigidBody*)CAR(d))->AddImpulse(1.0f, &pos, &at);
}

// AI debug console commands: name and handler (stored as a pointer to member: this delta, vtable slot).
struct AICommand {
    const char* name;
    s32 thisDelta;
    s32 vtblSlot;
    void (*fn)(u8* d);
};

void fn_80193CD4(u8* d);

AICommand lbl_80333158[4] = {
    {"wake", 0, -1, fn_8019386C},
    {"sleep", 0, -1, fn_80193CD4},
    {"pit", 0, -1, fn_80193870},
    {"wreck", 0, -1, fn_80193878},
};


u8* fn_801938F4(u8* d) {
    S32(d, 0x68) = 0;
    *(void***)d = lbl_80333348;
    S32(d, 0x70) = 10000;
    F32(d, 0x6C) = 0.0f;
    S32(d, 0x78) = 0;
    S32(d, 0x90) = -1;
    U8(d, 0x8C) = 0;
    S32(d, 0xA0) = 0;
    S32(d, 0xB8) = -1;
    U8(d, 0xB4) = 0;
    u8* m = d + 0x168;
    fn_801352B8(m);
    fn_801352B8(m + 0x34);
    F32(m, 0x68) = -999.0f;
    VI(m + 0x34)->v0C(0);
    VI(m)->v0C(0);
    CAR(d) = 0;
    CARID(d) = -1000;
    return d;
}

void fn_801942F0(u8* d, int on);
void fn_80193D30(u8* d);
void fn_8019416C(u8* d, const char* why);

void fn_801939C8(u8* d) {
    if (U8(d, 0x2A4)) {
        fn_802E30C8("AIDriver::Connect(): Car %d: already connected!", CARID(d));
        return;
    }
    U8(d, 0x2A4) = 1;
    F32(d, 0x24C) = F32(CAR(d), 0x5B0);
    fn_801942F0(d, 1);
    fn_80193D30(d);
}

void fn_80193A38(u8* d) {
    U8(d, 0x2A4) = 0;
    u8* car = CAR(d);
    S32(car, 0x24) = S32(car, 0x28);
}

u8* fn_80193A50(u8* d, int del) {
    if (d) {
        s32 id = CARID(d);
        if (id != -1000) {
            lbl_804349C0[id] = 0;
        }
        lbl_8055E550--;
        u8* m = d + 0x168;
        if (m) {
            fn_8013530C(d + 0x19C, -1);
            fn_8013530C(d + 0x168, -1);
        }
        if (del > 0) {
            operator delete(d);
        }
    }
    return d;
}

void fn_80193AEC(u8* d, u8* car) {
    CAR(d) = car;
    S32(d, 0x214) = 2;
    U8(d, 0x2A4) = 0;
    CARID(d) = S32(car, 0x1EC);
    fn_80134CD4(CARID(d), d + 0x2AC);
    PTR(d, 0x298) = CAR(d) + 0x23AC;
    lbl_804349C0[CARID(d)] = d;
    u8* x = PTR(CAR(d), 0x270);
    PTR(d, 0x28C) = x;
    PTR(d, 0x164) = VI(x)->v18();
    if (lbl_8055E550++ == 0) {
        lbl_8055A488 = fn_80137C54(-1)[0];
        lbl_8055A48C = fn_80137C54(-1)[1];
        lbl_8055E580 = 1;
        lbl_8055E540 = S32(fn_801769CC(), 4) != 0x180304;
        lbl_8055E554 = F32(PTR(PTR(d, 0x164), 0x2C), 0x8);
        lbl_8055E56C = fn_80134B58((void*)"moveout_timer_f", 1.0f);
        lbl_8055E568 = fn_80134B58((void*)"yellow_moveout_f", 1.0f);
        lbl_8055E564 = fn_80134B58((void*)"pushout_factor", 1.0f);
        lbl_8055E558 = fn_80134B58((void*)"min_temperance", 0.5f);
        fn_80134B58((void*)"moveout_factor", 1.0f);
        lbl_8055E544 = 1.0f;
        lbl_8055E548 = fn_80134B58((void*)"pass_hys_min", 5.0f);
        lbl_8055E54C = fn_80134B58((void*)"pass_hys_max", 13.0f);
        lbl_8055E55C = fn_80134B58((void*)"min_hangout", 0.993f);
        lbl_8055E560 = fn_80134B58((void*)"max_hangout", 0.999f);
        lbl_8055E570 = fn_80134B58((void*)"push_lat_sens", 1.1f);
        lbl_8055E584 = fn_80134B58((void*)"min_overheat_t", 60.0f);
        lbl_8055E588 = fn_80134B58((void*)"max_overheat_t", 240.0f);
        lbl_8055E578 = 0;
    }
    fn_80193D30(d);
}

void fn_80193CD4(u8* d) { U8(d, 0x2A5) = 1; }

void fn_80193CE0(u8* d) {
    ((VSelf*)d)->v30();
    U8(d, 0x2A5) = 0;
}

f32 fn_80193D20(void) { return fn_80129DC4(3, lbl_8055E584, lbl_8055E588); }

void fn_80195D80(u8* d, f32 t);

void fn_80193D30(u8* d) {
    f32 t = lbl_8055E3C8;
    lbl_8055E57C = 0.0f;
    lbl_8055E574 = fn_801769B8() == 2;
    memset(d + 0x8, 0, 0x10);
    memset(d + 0x20, 0, 0x54);
    memset(d + 0x74, 0, 0x78);
    memset(d + 0xEC, 0, 0x30);
    memset(d + 0x11C, 0, 0x48);
    memset(d + 0x1D4, 0, 0x34);
    memset(d + 0x21C, 0, 0x4);
    memset(d + 0x220, 0, 0x1C);
    memset(d + 0x23C, 0, 0xC);
    memset(d + 0x2D4, 0, 0x28);
    S32(d, 0x260) = 0;
    S32(d, 0x2C4) = -1;
    F32(d, 0x2B8) = 0.0f;
    F32(d, 0x2C0) = 0.0f;
    F32(d, 0x264) = 0.0f;
    S32(d, 0x2D0) = 0;
    U8(d, 0x2A7) = 0;
    memset(d + 0x218, 0, 0x4);
    S32(d, 0x210) = 0;
    S32(d, 0x2B4) = lbl_8055E3C4;
    F32(d, 0x1D4) = -1000.0f;
    F32(d, 0x1F0) = -1000.0f;
    F32(d, 0x1E8) = -1000.0f;
    F32(d, 0x1E0) = -1000.0f;
    F32(d, 0x1E4) = -1000.0f;
    F32(d, 0x1EC) = -1.0f;
    F32(d, 0x1F4) = -1000.0f;
    F32(d, 0x1F8) = -1000.0f;
    F32(d, 0x1FC) = -1000.0f;
    F32(d, 0x204) = -1000.0f;
    F32(d, 0x200) = fn_80193D20();
    F32(d, 0x20C) = 0.0f;
    u8 on = 0;
    if ((fn_801769A8() & 0x800) && S32(CAR(d), 0x1F0) != 0x50) {
        on = 1;
    }
    U8(d, 0x2A5) = on;
    U8(d, 0x208) = 0;
    memset(d + 0x26C, 0, 0x20);
    u8* p = PTR(d, 0x28C);
    S32(d, 0x288) = (s32)VI(p)->v10();
    u8* car = CAR(d);
    F32(d, 0x25C) = 0.0f;
    F32(d, 0x144) = 0.0f;
    F32(d, 0x148) = 0.0f;
    F32(d, 0x14C) = 1.0f;
    F32(d, 0x150) = 0.0f;
    S32(d, 0x140) = 0;
    F32(d, 0x1C) = 4.4444447f;
    F32(d, 0x2BC) = 6.0f;
    u8* x = PTR(car, 0x2C0);
    if (x) {
        fn_8013B6A4(CARID(d), VI(x)->v28("speed_factor"));
    }
    S32(d, 0x29C) = fn_8013B578(CARID(d));
    if (fn_801667F0()) {
        fn_8013B6A4(CARID(d), 1.0f);
    }
    F32(d, 0x254) = 0.0f;
    F32(d, 0x258) = 0.5f;
    F32(d, 0x2A8) = 0.0f;
    F32(d, 0x2A0) = 0.0f;
    memset(d + 0x2FC, 0, 0xC);
    U8(d, 0x74) = 0;
    U8(d, 0x250) = 0;
    F32(d, 0xE8) = 0.0f;
    F32(d, 0xE0) = 0.0f;
    F32(d, 0xF0) = 0.0f;
    F32(d, 0xF4) = 0.0f;
    S32(d, 0x118) = -1;
    S32(d, 0x114) = -1;
    F32(d, 0x248) = 0.0f;
    F32(d, 0x234) = -1000.0f;
    F32(d, 0x22C) = -1000.0f;
    F32(d, 0x224) = 0.0f;
    F32(d, 0x228) = 0.0f;
    F32(d, 0x220) = 0.0f;
    F32(d, 0x244) = 0.0f;
    F32(d, 0x240) = 0.0f;
    F32(d, 0x23C) = 0.0f;
    S32(d, 0x13C) = -1;
    U8(d, 0x11C) = 0;
    F32(d, 0x120) = 0.0f;
    S32(d, 0x98) = 0;
    F32(d, 0x94) = -1000.0f;
    S32(d, 0xC0) = 0;
    F32(d, 0xBC) = 1000.0f;
    U8(d, 0x9C) = 0;
    U8(d, 0x74) = 0;
    F32(d, 0xC4) = 0.0f;
    F32(d, 0xD0) = 0.0f;
    F32(d, 0xC8) = 0.0f;
    F32(d, 0xD4) = 0.0f;
    U8(d, 0xFC) = 0;
    S32(d, 0x100) = 0;
    S32(PTR(d, 0x164), 0xC) = 0;
    if (S32(CAR(d), 0x1F0) == 0x50) {
        fn_801B30D0(CARID(d));
        int take = 0;
        if (!fn_801B8570(CARID(d))) {
            f32 l = lbl_8055E3C8;
            if (fn_801B306C() < l) {
                take = 1;
            }
        }
        if (take) {
            fn_802E30C8("aicar: new player-takeover racingline zip performed!");
            fn_801B30E0(CARID(d));
        }
    }
    u8* m = d + 0x168;
    F32(m, 0x68) = -999.0f;
    VI(m + 0x34)->v0C(0);
    VI(m)->v0C(0);
    fn_80195D80(d, t);
    S32(d, 0x268) = 6;
    U8(d, 0x2A6) = 0;
    F32(d, 0x21C) = 1.0f;
    u8* q = PTR(d, 0x2C8);
    F32(d, 0x2E0) = F32(q, 0x44);
    F32(d, 0x2E4) = F32(q, 0x48);
    F32(d, 0x2E8) = F32(q, 0x4C);
    F32(d, 0x2EC) = F32(q, 0x50);
    F32(d, 0x2F0) = F32(q, 0x54);
    F32(d, 0x2F4) = F32(q, 0x58);
    F32(d, 0x2F8) = F32(q, 0x5C);
    PTR(d, 0x2D4) = d + 0x2E0;
    PTR(d, 0x2D8) = d + 0x2E0;
    F32(d, 0x2DC) = t;
    S32(d, 0x4) = CARID(d) * 4;
}

void fn_8019416C(u8* d, const char* why) {}

void fn_80194170(u8* d, f32 v) {
    F32(d, 0x240) = Clamp01(v);
    fn_8018ADB4(CAR(d), F32(d, 0x240));
}

void fn_801941A8(u8* d, f32 v) {
    f32 dv = v - F32(d, 0x23C);
    dv = 0.2f < dv ? 0.2f : dv;
    dv = dv > -0.2f ? dv : -0.2f;
    f32 n = F32(d, 0x23C) + dv;
    f32 k = F32(d, 0x248);
    F32(d, 0x23C) = n;
    if (k < 2.0f) {
        f32 a = n;
        f32 b = F32(CAR(d), 0x25F0);
        if ((*(u32*)&b & 0x80000000) != (*(u32*)&a & 0x80000000)) {
            F32(d, 0x248) = k + 0.1f;
        }
    }
    f32 g = Clamp01(F32(d, 0x248));
    f32 s = F32(d, 0x23C) * (1.0f - g);
    F32(d, 0x23C) = s;
    s = 1.0f < s ? 1.0f : s;
    s = s > -1.0f ? s : -1.0f;
    F32(d, 0x23C) = s;
    fn_8018ADAC(CAR(d), s);
}

void fn_801942B8(u8* d, f32 v) {
    F32(d, 0x244) = Clamp01(v);
    fn_8018ADBC(CAR(d), F32(d, 0x244));
}

void fn_801942F0(u8* d, int on) {
    if ((S32(d, 0x2D0) & 0x20) || on) {
        S32(CAR(d), 0x267C) = 1;
    } else {
        S32(CAR(d), 0x267C) = 0;
    }
}

void fn_80194324(u8* d) {
    if (IsLoadingActive()) {
        S32(d, 0x2D0) |= 0x10;
    }
    int ok = 1;
    f32 lim = 60.0f;
    f32 lo = 8.0f;
    if (!(S32(d, 0x2D0) & 0x10)) {
        ok = F32(CAR(d), 0x25D4) > 6.6666665f;
        if (ok) {
            f32 dt = F32(d, 0x26C) - F32(d, 0x1D0);
            f32 v;
            if (dt < 1.5f) {
                f32 a = F32(d, 0x1B0) * F32(d, 0x1B4) + -F32(d, 0x1AC) * F32(d, 0x1B8);
                f32 b = F32(d, 0x17C) * F32(d, 0x180) + -F32(d, 0x178) * F32(d, 0x184);
                v = b + (dt / 1.5f) * (a - b);
            } else {
                v = F32(d, 0x1B0) * F32(d, 0x1B4) + -F32(d, 0x1AC) * F32(d, 0x1B8);
            }
            ok = (f32)__fabs(v) < lim;
        }
        if (!ok && F32(d, 0x1D8) >= lo) {
            void* w = fn_801B83A0();
            VI(w)->vAC(CARID(d));
            F32(d, 0x1D8) = 0.0f;
        }
    }
    if (!ok) {
        F32(d, 0x1D8) = F32(d, 0x1D8) + 4.2375712f;
    } else {
        f32 v = F32(d, 0x1D8) - 4.2375712f;
        F32(d, 0x1D8) = v > 0.0f ? v : 0.0f;
    }
}

void fn_801945F8(u8* d, const char* why);
void fn_80193CD4(u8* d);

void fn_801944C0(u8* d) {
    u8* s = PTR(d, 0x288);
    u8 w = U8(s, 0x28);
    U8(d, 0x2A6) = w;
    if (w) {
        if (F32(d, 0x1EC) < 0.0f) {
            F32(d, 0x1EC) = F32(d, 0x26C);
        }
        switch (lbl_8055E540) {
        case 2:
            fn_8018AF04(CAR(d));
            break;
        case 1:
            if (!(S32(d, 0x2D0) & 0x200)) {
                fn_801945F8(d, "disabled");
            }
            break;
        case 0:
            fn_80193CD4(d);
            break;
        }
        u8* p = PTR(d, 0x28C);
        VI(p)->v30(-1);
    } else {
        F32(d, 0x1EC) = -1.0f;
    }
}

extern "C" const f32 lbl_8056215C;  // 0.5

void fn_80194590(u8* d) {
    if (S32(d, 0x260) == 0) {
        s32 k = 1;
        S32(d, 0x260) = 1;
        if (S32(d, 0x18) == 1) {
            k = -1;
        }
        F32(d, 0x264) = (f32)k * (F32(d, 0x284) + lbl_8055A488 * lbl_8056215C);
    }
}

void fn_801945F8(u8* d, const char* why) {
    if (S32(CAR(d), 0x1F0) == 0x50) {
        if (S32(PTR(d, 0x288), 0x24) == 3) {
            fn_80194590(d);
        }
    } else {
        fn_80194590(d);
    }
}

static inline f32 Blend(u8* d, u8* a, u8* b, int o) {
    f32 t = Clamp01((F32(d, 0x26C) - F32(d, 0x2DC)) / F32(b, 0x0));
    return F32(a, o) + t * (F32(b, o) - F32(a, o));
}

void fn_80194624(u8* d) {
    u8* e = ((AIExceptions*)(PTR(d, 0x2C8) + 0x40))->Find(F32(d, 0x280));
    if (e != PTR(d, 0x2D8)) {
        PTR(d, 0x2D4) = PTR(d, 0x2D8);
        PTR(d, 0x2D8) = e;
        F32(d, 0x2DC) = F32(d, 0x26C);
    }
    F32(d, 0x2E8) = Blend(d, PTR(d, 0x2D4), PTR(d, 0x2D8), 0x8);
    F32(d, 0x2E4) = Blend(d, PTR(d, 0x2D4), PTR(d, 0x2D8), 0x4);
    F32(d, 0x2EC) = Blend(d, PTR(d, 0x2D4), PTR(d, 0x2D8), 0xC);
    F32(d, 0x2F0) = Blend(d, PTR(d, 0x2D4), PTR(d, 0x2D8), 0x10);
    F32(d, 0x2F4) = Blend(d, PTR(d, 0x2D4), PTR(d, 0x2D8), 0x14);
}

void fn_801958D8(u8* d);
void fn_80195F6C(u8* d);

void fn_80194844(u8* d) {
    fn_801958D8(d);
    U8(d, 0x250) = 0;
    F32(d, 0x270) = F32(CAR(d), 0x25D4);
    F32(d, 0x26C) = lbl_8055E3C8;
    fn_80195F6C(d);
    fn_80194624(d);
    U8(d, 0x14) = 0;
    F32(d, 0x8) = 0.0f;
}


void fn_801948B0(u8* d) {}

static inline V2 LerpXY(const RVec3* a, const RVec3* b, f32 t) {
    V2 r;
    r.y = a->y + t * (b->y - a->y);
    r.x = a->x + t * (b->x - a->x);
    return r;
}

void fn_80194AF4(u8* d, RVec3* out, RVec3* a, RVec3* b, f32 t) {
    V2 c = LerpXY(a, b, t);
    out->x = c.x;
    out->y = c.y;
    out->z = a->z + t * (b->z - a->z);
}

f32 fn_80194B60(f32 a, f32 b, f32 p, f32 x) {
    f32 m = (f32)fmod(x, p);
    if (m > p * 0.5f) {
        m = p - m;
    }
    f32 t = m / (p * 0.5f);
    f32 s;
    if (t < -1.0f) {
        s = -1.0f;
    } else if (t > 1.0f) {
        s = 1.0f;
    } else if (t > 0.0f) {
        s = -2.0f * t * t * t + 3.0f * t * t;
    } else {
        s = -2.0f * t * t * t - 3.0f * t * t;
    }
    return a + s * (b - a);
}

void fn_80194C4C(u8* d, u8* out, f32 t) {}

f32 fn_80194F70(const RVec3* n, const RVec3* a, const RVec3* b) {
    RVec3 v;
    v.x = a->x - b->x;
    v.y = a->y - b->y;
    v.z = a->z - b->z;
    return PSVECDotProduct(n, &v);
}

void fn_80194FC4(u8* d) {}

void fn_80194FC8(u8* d) { fn_8013B744(CARID(d)); }

void fn_80194FD0(u8* d, u8* out, f32 t) {}

void fn_801955F0(u8* d, u8* out, f32 t) {
    fn_80194FD0(d, out, t);
    f32 k = F32(d, 0x2B8);
    F32(out, 0x0) = F32(out, 0x0) + F32(d, 0x58) * k;
    F32(out, 0x4) = F32(out, 0x4) + F32(d, 0x5C) * k;
}

void fn_801971AC(u8* d, V2* io);
void fn_801981A0(u8* d, u8* out);
void fn_8019743C(u8* d, u8* out);
void fn_80197BD0(u8* d, u8* out);
void fn_8019724C(u8* d, V2* io);

struct Pos4 {
    V2 p;
    f32 c;
    f32 d;
};

void fn_80195654(u8* d, u8* out) {
    S32(d, 0x2D0) = 0;
    f32 dt = F32(d, 0x26C) - F32(d, 0x1D0);
    f32 t;
    if (dt < 1.5f) {
        u8* a = PTR(d, 0x1A0);
        u8* b = PTR(d, 0x16C);
        u8* an = PTR(a, 0x14);
        u8* bn = PTR(b, 0x14);
        f32 va = F32(a, 0x24) + F32(d, 0x1A4) * (F32(an, 0x24) - F32(a, 0x24));
        f32 vb = F32(b, 0x24) + F32(d, 0x170) * (F32(bn, 0x24) - F32(b, 0x24));
        t = vb + (dt / 1.5f) * (va - vb);
    } else {
        u8* a = PTR(d, 0x1A0);
        t = F32(a, 0x24) + F32(d, 0x1A4) * (F32(PTR(a, 0x14), 0x24) - F32(a, 0x24));
    }
    if (S32(d, 0x268) == 1) {
        fn_80194C4C(d, out, t);
    } else {
        fn_80194FD0(d, out, t);
        if (F32(d, 0x26C) - F32(d, 0x1E4) < F32(d, 0x2BC)) {
            Pos4 tmp;
            tmp.p = *(V2*)out;
            tmp.c = F32(out, 0x8);
            tmp.d = F32(out, 0xC);
            fn_801955F0(d, (u8*)&tmp, t);
            f32 k = (F32(d, 0x26C) - F32(d, 0x1E4)) / F32(d, 0x2BC);
            fn_80194AF4(d, (RVec3*)out, (RVec3*)&tmp, (RVec3*)out, k * k);
        }
    }
    fn_801971AC(d, (V2*)out);
    F32(d, 0x1C) = F32(out, 0x8);
    fn_801981A0(d, out);
    fn_8019743C(d, out);
    fn_80197BD0(d, out);
    fn_8019724C(d, (V2*)out);
    if (fn_80167544(CARID(d))) {
        S32(d, 0x2D0) |= 0x50;
    }
}

void fn_80195830(u8* d) {
    fn_80195654(d, d + 0x20);
    fn_801933FC(d, (V2*)(d + 0x20));
    f32 x = F32(d, 0x20);
    f32 y = F32(d, 0x24);
    F32(d, 0x40) = x;
    F32(d, 0x44) = y;
    f32 l = (f32)sqrt(x * x + y * y);
    l = 0.01f > l ? 0.01f : l;
    f32 inv = 1.0f / l;
    F32(d, 0x40) = F32(d, 0x40) * inv;
    F32(d, 0x44) = F32(d, 0x44) * inv;
}

void fn_801958C4(u8* d) { F32(d, 0x248) *= 0.9409f; }

void fn_801958D8(u8* d) {
    u8* s = PTR(d, 0x288);
    s32 old = S32(d, 0x268);
    s32 n = S32(s, 0x24);
    if (old == n) {
        return;
    }
    switch (n) {
    case 0:
        F32(d, 0x1E4) = F32(d, 0x26C);
        F32(d, 0x2B8) = F32(s, 0x34);
        S32(d, 0x260) = 0;
        break;
    case 1:
        F32(d, 0x1E8) = -1000.0f;
        break;
    case 3:
    {
        f32 v = F32(d, 0x278);
        F32(d, 0x1DC) = F32(d, 0x26C);
        F32(d, 0x2C0) = v > 0.0f ? 1.0f : -1.0f;
    }
        break;
    default:
        fn_802E317C("AIDriver: Unrecognized flag %d", old);
        break;
    }
    S32(d, 0x268) = n;
}

void fn_80195B28(u8* d, f32 v);
void fn_801961F8(u8* d, const char* why);

void fn_801959AC(u8* d) {
    if (U8(d, 0x2A7) && S32(CAR(d), 0x2620) > 0) {
        U8(d, 0x2A7) = 0;
        fn_8018A174(CAR(d));
    }
    if (!((f32)__fabs(F32(CAR(d), 0x2690)) <= lbl_80558DC4[0])) {
        fn_801961F8(d, "i am damaged");
    }
    fn_80195B28(d, 0.0f);
    fn_801942F0(d, F32(d, 0x26C) - F32(d, 0x1F4) > 1.0f);
    switch (S32(d, 0x260)) {
    case 1:
        f32 off = F32(d, 0x264);
        if ((f32)__fabs(F32(d, 0x278) - off) < 0.61f) {
            S32(d, 0x260) = 2;
        }
        break;
    case 2:
        if (F32(d, 0x270) < 0.44444445f) {
            fn_80193CD4(d);
        }
        break;
    }
    if ((f32)__fabs(F32(d, 0x278)) > 1.0f + F32(d, 0x284) && !(S32(d, 0x2D0) & 8)) {
        fn_8019416C(d, "offroad");
    }
    int z = S32(CAR(d), 0x2620) == 0;
    if (z && !U8(d, 0x208)) {
        F32(d, 0x204) = F32(d, 0x26C);
        U8(d, 0x208) = z;
    }
}

f32 lbl_80333308[9] = {5.0f, 15.0f, 4.0f, 1.0f, 1.5f, 0.9f, 0.3f, 0.8f, 0.4f};

void fn_80195B28(u8* d, f32 v) {
    if (!(F32(d, 0x234) < F32(d, 0x26C)) && v == 0.0f) {
        return;
    }
    if (S32(fn_801769CC(), 4) != 0x180201) {
        int one = S32(d, 0x268) == 1;
        f32* tbl = lbl_80333308;
        if (one) {
            tbl += 3;
        }
        F32(d, 0x22C) = F32(d, 0x26C);
        F32(d, 0x234) = fn_80129DC4(3, tbl[0], tbl[1]);
        if (!one) {
            F32(d, 0x234) *= lbl_8055E56C;
        }
        F32(d, 0x228) = F32(d, 0x220);
        F32(d, 0x234) += F32(d, 0x26C);
        F32(d, 0x230) = tbl[2];
        if (one) {
            F32(d, 0x224) = fn_80129DC4(3, -0.5f, 0.5f);
        } else {
            F32(d, 0x224) = fn_80129DC4(3, -1.0f, 1.0f);
        }
        if (v != 0.0f) {
            F32(d, 0x224) = (v > 0.0f ? 1.0f : -1.0f) * (f32)__fabs(F32(d, 0x224));
        }
    } else {
        f32 t = F32(d, 0x26C);
        F32(d, 0x234) = t;
        F32(d, 0x228) = 0.0f;
        F32(d, 0x224) = 0.0f;
        F32(d, 0x22C) = t;
        F32(d, 0x230) = t;
    }
}

void fn_80195CA8(u8* d) {
    if (!(S32(d, 0x4) & 0x7F)) {
        fn_80194324(d);
        fn_801944C0(d);
    }
    if (F32(d, 0x270) < 6.6666665f && F32(d, 0x2A0) > 100.0f) {
        F32(d, 0x2A0) = -200.0f;
        if (S32(d, 0x260) == 0) {
            s32 m = S32(d, 0x210);
            if (m != 1) {
                if (S32(d, 0x214) == 2) {
                    S32(d, 0x214) = m;
                    S32(d, 0x210) = 1;
                } else {
                    S32(d, 0x210) = 1;
                }
                F32(d, 0x218) = F32(d, 0x26C);
            }
        } else {
            S32(d, 0x260) = 2;
        }
    } else {
        F32(d, 0x2A0) *= 0.984064f;
    }
    S32(d, 0x4)++;
}

}  // extern "C"

// Where the car is on the race line (copied whole into the AI state each frame).
struct LinePos {
    f32 pos[3];  // offset 0x0
    u32 w[6];    // offset 0xC
    u8 flag;   // offset 0x24
    u8* seg;   // offset 0x28
    u32 w2C;   // offset 0x2C
};

class VPosSrc {
public:
    virtual void s08(void);
    virtual void s0C(void);
    virtual void s10(void);
    virtual void s14(void);
    virtual void s18(void);
    virtual u8* Pos(void);  // 0x1C
};

class LineFollower {
public:
    void Update(V2* p);
};

extern f32 lbl_805621C0;  // 1.5f

extern "C" {
u32 fn_801388F4(u32 a);

// Tracks the car's race-line position; re-aims the line follower at most every 1.5 s.
void fn_80195D80(u8* d, f32 t) {
    u8* pos = ((VPosSrc*)PTR(d, 0x28C))->Pos();
    F32(d, 0x2CC) = F32(PTR(pos, 0x2C), 0x14);
    U32(d, 0x2C8) = U32(PTR(pos, 0x2C), 0x28);
    U32(d, 0x18) = fn_801388F4(U32(pos, 0x4));
    u8* car = CAR(d);
    bool changed = PTR(d, 0x1C8) && PTR(d, 0x1C8) != PTR(pos, 0x2C);
    if (changed) {
        *(LinePos*)(d + 0x16C) = *(LinePos*)(d + 0x1A0);
        F32(d, 0x1D0) = lbl_8055E3C8;
    }
    *(LinePos*)(d + 0x1A0) = *(LinePos*)(pos + 0x4);
    if (t - F32(d, 0x1D0) < lbl_805621C0) {
        f32 s = F32(car, 0x5BC);
        RVec3 p;
        p.x = F32(car, 0x94) + F32(car, 0x54) * s;
        p.y = F32(car, 0x98) + F32(car, 0x58) * s;
        p.z = F32(car, 0x9C) + F32(car, 0x5C) * s;
        V2 q;
        q.x = p.x;
        q.y = p.z;
        ((LineFollower*)(d + 0x168))->Update(&q);
    }
}

void fn_80195F6C(u8* d) {}

void fn_801961F8(u8* d, const char* why) {}

void fn_801961FC(u8* d) {
    F32(d, 0x28) = F32(d, 0x28) * fn_801671FC(CARID(d));
    if (fn_801B89F8()) {
        S32(d, 0x2D0) |= 0x1150;
    }
}

void fn_80196250(f32* outA, f32* outB, s32* sign, f32 a, f32 b, f32 c) {
    f32 x = (1.3333334f + (a - b)) / c;
    if ((f32)__fabs(a) < 0.1f) {
        *outB = 1.0f;
        *outA = 0.0f;
        *sign = 0;
        return;
    }
    if ((f32)__fabs(b) < 0.44444445f || (*(u32*)&x & 0x80000000) == (*(u32*)&b & 0x80000000)) {
        *outA = (f32)__fabs(x) / 4.905f;
        *outB = 0.0f;
    } else {
        *outA = 0.0f;
        *outB = (f32)__fabs(x) / 11.772001f;
    }
    *sign = a > 0.0f ? 1 : -1;
}

void fn_80196328(u8* d) {
    fn_8018ADBC(CAR(d), 1.0f);
    fn_8018ADB4(CAR(d), 0.0f);
    fn_8018ADAC(CAR(d), 0.0f);
    fn_8018AE24(CAR(d), 0);
    lbl_8055E0D0->SortCar(CARID(d));
    lbl_8055E0D0->SetActive(CARID(d), 0);
}

void fn_8019639C(u8* d) { fn_80194844(d); }

void fn_801963A0(u8* d) {
    fn_801961F8(d, 0);
    f32 v = F32(CAR(d), 0x25D4);
    lbl_8055E57C += 0.033366702f * v;
    lbl_8055E0D0->SetActive(CARID(d), 1);
    fn_801958C4(d);
    fn_801959AC(d);
    fn_80195CA8(d);
}

void fn_80196DC4(u8* d);

void fn_80196414(u8* d) { fn_80196DC4(d); }

void fn_80196418(u8* d) {
    switch (S32(d, 0x210)) {
    case 0:
        fn_80195830(d);
        break;
    case 1:
        break;
    }
}

int fn_8019643C(int i) { return U32(lbl_804348E8[i], 0x10) == 'PCAR'; }

int fn_80196468(int i) {
    s32 n = fn_801B2F24();
    for (; i <= n; i++) {
        if (fn_8019643C(fn_801B2F40(i - 1))) {
            return 1;
        }
    }
    return 0;
}

void fn_801964D0(u8* d) {
    u8* p = PTR(d, 0x28C);
    if (fn_80196468(S32(VI(p)->v10(), 4))) {
        U8(d, 0x2A7) = 1;
        if (fn_80129D18(3, 10) < 5) {
            fn_8018A174(CAR(d));
            f32 s = fn_80129D18(3, 2) ? -1.0f : 1.0f;
            u8* car = CAR(d);
            f32 v = F32(car, 0xD4);
            f32 k = 59.94f * (1000.0f * v) * s;
            RVec3 t;
            t.x = F32(car, 0x3C) * k;
            t.y = F32(car, 0x40) * k;
            t.z = F32(car, 0x44) * k;
            ((RigidBody*)CAR(d))->AddTorque(&t);
        }
    }
}

void fn_801EA18C(RVec3* pos, const RVec3* dir, int a);
extern const f32 lbl_80562140;  // 1.0f
extern const f32 lbl_80562144;  // 0.0f
extern const f32 lbl_80562248;  // -1.1f
void fn_8018AE2C(u8* car, f32 pitch);
void fn_8018AE34(u8* car, f32 roll);

// Steers the truck's attitude: in the air level it against up; on the ground pitch from height and speed.
void fn_801965AC(u8* d) {
    if (S32(CAR(d), 0x2620) != 0) {
        return;
    }
    RVec3 up;
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    PVec3 pos = *(PVec3*)(CAR(d) + 0x60);
    fn_801EA18C((RVec3*)&pos, &up, 0);
    f32 h = MAX(0.0f, F32(CAR(d), 0x64) - pos.v[1]);
    f32 vy = MIN(F32(CAR(d), 0x11C), -0.01f);
    bool air = -h / vy < 1.0f;
    f32 gain = 5.0f;
    if (air) {
        f32 a = gain * PSVECDotProduct(&up, CAR(d) + 0x54);
        a = MIN(1.0f, a);
        fn_8018AE2C(CAR(d), MAX(a, -1.0f));
        a = gain * PSVECDotProduct(&up, CAR(d) + 0x3C);
        a = MIN(1.0f, a);
        fn_8018AE34(CAR(d), MAX(a, -1.0f));
    } else {
        f32 k = MIN(1.0f, ((f32)__fabs(F32(d, 0x23C)) - lbl_80562144) / 1.0f);
        k = MAX(k, 0.0f);
        f32 a = gain * (F32(CAR(d), 0x58) - (0.7f + k * lbl_80562248));
        a = MIN(1.0f, a);
        fn_8018AE2C(CAR(d), MAX(a, -1.0f));
        fn_8018AE34(CAR(d), 0.0f);
    }
}

void fn_80198530(u8* d);
void fn_80196AA0(u8* d);

void fn_801967A4(u8* d) {
    switch (S32(d, 0x210)) {
    case 0:
        fn_80198530(d);
        fn_801965AC(d);
        break;
    case 1:
        fn_80196AA0(d);
        break;
    }
}

void fn_801967FC(u8* d) {
    S32(d, 0x300) = 0;
    if (U8(d, 0x250)) {
        F32(d, 0x254) *= 0.96040004f;
        return;
    }
    f32 v = 0.01f + F32(d, 0x254);
    F32(d, 0x254) = 1.0f < v ? 1.0f : v;
}

}  // extern "C"

// AI drivers gathered for one update pass.
struct CarList {
    s32 count;   // offset 0x0
    u8* cars[8]; // offset 0x4
    void Add(u8* d) { cars[count++] = d; }
};

extern "C" {
int fn_801796B4(void);
int fn_80134A44(int car);

// Updates every active AI driver: paused/scripted ones only get the base update and path step, the rest
// run the full pipeline; then each listed driver's v48.
void fn_8019684C(void) {
    CarList full;
    CarList hold;
    int paused;
    int k;
    int i;
    u8* d;
    full.count = 0;
    hold.count = 0;
    paused = fn_801796B4();
    for (i = 0; i < 8; i++) {
        d = lbl_804349C0[i];
        if (d && fn_80134A44(i) && U8(d, 0x2A4)) {
            if (paused || U8(d, 0x2A5)) {
                hold.Add(d);
            } else {
                full.Add(d);
            }
        }
    }
    for (k = 0; k < hold.count; k++) {
        fn_8019639C(hold.cars[k]);
    }
    for (k = 0; k < full.count; k++) {
        fn_8019639C(full.cars[k]);
    }
    for (k = 0; k < hold.count; k++) {
        fn_80196328(hold.cars[k]);
    }
    for (k = 0; k < full.count; k++) {
        fn_801963A0(full.cars[k]);
    }
    for (k = 0; k < full.count; k++) {
        fn_80196414(full.cars[k]);
    }
    for (k = 0; k < full.count; k++) {
        fn_80196418(full.cars[k]);
    }
    for (k = 0; k < full.count; k++) {
        fn_801967A4(full.cars[k]);
    }
    for (k = 0; k < full.count; k++) {
        fn_801967FC(full.cars[k]);
    }
    for (k = 0; k < lbl_804349E0.count; k++) {
        VI(lbl_804349E0.items[k])->v48();
    }
}

void fn_80196A9C(u8* d) { fn_8019684C(); }

extern const f32 lbl_80562250[2];  // phase ends: 0.1 s, 1 s
extern const f32 lbl_80562258;     // -44500
extern const f32 lbl_8056225C;     // -31150
extern const f32 lbl_80562160;     // 4.444
void fn_8018ADD4(void* car, int v);
void fn_8018ADC4(void* car, f32 v);

// Start-line launch: a first push, then hold the line until the other car is ready.
void fn_80196AA0(u8* d) {
    f32 t = F32(d, 0x26C) - F32(d, 0x218);
    int phase;
    for (phase = 0; phase < 2; phase++) {
        if (t < lbl_80562250[phase]) {
            break;
        }
    }
    u8* o = PTR(d, 0x298);
    if (!U8(o, 0x17C) || !U8(o, 0x108)) {
        phase = -1;
    }
    switch (phase) {
    case 0: {
        u8* car = CAR(d);
        RVec3 f;
        f.x = F32(car, 0x88) * lbl_80562258;
        f.y = F32(car, 0x8C) * lbl_80562258;
        f.z = F32(car, 0x90) * lbl_80562258;
        (*(RigidBody**)(d + 0x294))->AddForceAt(&f, (RVec3*)(CAR(d) + 0x94));
    }
    case 1: {
        V2 v;
        v.x = 0.0f;
        v.y = 1.0f;
        f32 ax;
        f32 ay;
        ay = F32(d, 0x44);
        ax = F32(d, 0x40);
        f32 s = -(v.y * ax + -v.x * ay);
        s = MIN(1.0f, s);
        s = MAX(s, -1.0f);
        fn_8018ADAC(CAR(d), s);
        fn_8018ADD4(CAR(d), -1);
        fn_8018ADBC(CAR(d), 0.0f);
        fn_8018ADC4(CAR(d), 1.0f);
        if ((f32)__fabs(F32(d, 0x270)) < lbl_80562160) {
            fn_8018ADB4(CAR(d), 1.0f);
        }
        if (F32(d, 0x270) < lbl_80562160) {
            fn_8018ADC4(CAR(d), 0.0f);
            u8* car = CAR(d);
            RVec3 g;
            g.x = F32(car, 0x88) * lbl_8056225C;
            g.y = F32(car, 0x8C) * lbl_8056225C;
            g.z = F32(car, 0x90) * lbl_8056225C;
            (*(RigidBody**)(d + 0x294))->AddForceAt(&g, (RVec3*)(CAR(d) + 0x94));
        }
        break;
    }
    case -1:
        break;
    default:
        S32(d, 0x210) = S32(d, 0x214);
        S32(d, 0x214) = 2;
        break;
    }
}

extern "C" int __fpclassifyf(f32 x);

void fn_80196C90(u8* d) {
    F32(d, 0x144) *= F32(d, 0x14C);
    int any = 0;
    if (U8(d, 0x9C) || U8(d, 0x74)) {
        any = 1;
    }
    if (any) {
        f32 a = F32(d, 0x144);
        f32 b = F32(d, 0xCC);
        if ((*(u32*)&b & 0x80000000) == (*(u32*)&a & 0x80000000)) {
            F32(d, 0x144) = a * 0.80999994f;
        }
    }
    if ((f32)__fabs(F32(d, 0x144)) < (f32)__fabs(F32(d, 0x148))) {
        f32 t = F32(d, 0x150);
        int ok = 0;
        if (!(__fpclassifyf(t) == 1) && __fpclassifyf(t) > 2) {
            ok = 1;
        }
        if (!ok) {
            F32(d, 0x150) = F32(d, 0x26C) + fn_80129DC4(3, lbl_8055E548, lbl_8055E54C);
        }
    }
    F32(d, 0x148) = F32(d, 0x144);
}

void fn_80196DC4(u8* d) {}

int fn_8019700C(int car) {
    u8* o = lbl_804349C0[car];
    int r = 1;
    if (o && U8(o, 0x2A4)) {
        r = 0;
    }
    return r;
}

struct Hit {
    u32 w0;
    u32 w4;
    u32 w8;
    u32 wC;
    u32 w10;
    u8 b14;
    s32 id;
};

int fn_8019703C(u8* d, u8* p) {
    int any = 0;
    f32 z = F32(p, 0x8);
    f32 x = F32(p, 0x4);
    int fast = F32(d, 0x270) > 2.2222223f;
    int near = (f32)__fabs(x) < lbl_8055A488 + 1.1f * lbl_8055A488;
    if (!(S32(d, 0x2D0) & 1) && fast && near) {
        if (x > 0.0f) {
            f32 v = F32(d, 0x278) + x;
            if (v < F32(d, 0xBC)) {
                F32(d, 0xBC) = v;
                *(Hit*)(d + 0xA0) = *(Hit*)p;
                U8(d, 0x9C) = 1;
                any = 1;
            }
        } else {
            f32 v = F32(d, 0x278) + x;
            if (v > F32(d, 0x94)) {
                F32(d, 0x94) = v;
                *(Hit*)(d + 0x78) = *(Hit*)p;
                U8(d, 0x74) = 1;
                any = 1;
            }
        }
    }
    if (any) {
        F32(d, 0xCC) = x;
        F32(d, 0xD8) = z;
        S32(d, 0xDC) = S32(p, 0x18);
        if (x > 0.0f) {
            S32(d, 0xC0)++;
        } else {
            S32(d, 0x98)++;
        }
    }
    return 0;
}

void fn_801971AC(u8* d, V2* io) {
    if (S32(d, 0x260) && S32(d, 0x268) != 1) {
        f32 k = F32(d, 0x25C) + 0.008341676f;
        f32 off = F32(d, 0x264);
        F32(d, 0x25C) = k;
        V2 tgt;
        tgt.x = F32(d, 0x48) + F32(d, 0x58) * off;
        tgt.y = F32(d, 0x4C) + F32(d, 0x5C) * off;
        V2 r;
        r.x = io->x + k * (tgt.x - io->x);
        r.y = io->y + k * (tgt.y - io->y);
        V2 c = r;
        io->x = c.x;
        io->y = c.y;
    }
}

f32 fn_801973FC(const V2* n, const V2* a, const V2* b);

extern const f32 lbl_8056226C;  // 4 / 59.94 (four frames)
extern const f32 lbl_80562268;  // 0.9
extern const f32 lbl_8056222C;  // 40.0

void fn_8019724C(u8* d, V2* io) {
    if (F32(d, 0x270) > 4.4444447f) {
        s32 o = fn_801671AC(CARID(d));
        u8* car = CAR(d);
        V2 hit;
        V2 dir;
        dir.x = F32(car, 0x70);
        dir.y = F32(car, 0x78);
        f32 l = (f32)sqrt(dir.x * dir.x + dir.y * dir.y);
        l = 0.01f > l ? 0.01f : l;
        f32 inv = 1.0f / l;
        dir.x *= inv;
        dir.y *= inv;
        if (fn_8015FBCC(CAR(d), &hit, 0, o)) {
            f32 t = F32(d, 0x26C);
            f32 hi = lbl_8056226C + F32(d, 0x1F8);
            f32 lo = t - lbl_80562268;
            t = t < hi ? t : hi;
            t = t > lo ? t : lo;
            F32(d, 0x1F8) = t;
            F32(d, 0x20C) = fn_801973FC(&dir, &hit, io);
            io->x = hit.x;
            io->y = hit.y;
            if (F32(d, 0x270) > lbl_8056222C) {
                S32(d, 0x2D0) |= 0x40;
            }
            S32(d, 0x2D0) |= 0x800;
        } else {
            f32 dt = F32(d, 0x26C) - F32(d, 0x1F8);
            if (dt < lbl_80562268) {
                f32 k = F32(d, 0x20C) * (1.0f - dt / lbl_80562268);
                io->x = io->x + dir.x * k;
                io->y = io->y + dir.y * k;
            }
        }
    }
}

f32 fn_801973FC(const V2* n, const V2* a, const V2* b) {
    V2 v;
    v.x = a->x - b->x;
    v.y = a->y - b->y;
    return n->x * v.x + n->y * v.y;
}

void fn_8019743C(u8* d, u8* out) {}

void fn_80197AB8(u8* d, s32 id, f32 a, f32 b, f32 c);

int fn_8019799C(u8* d, u8* p) {
    int r = 0;
    if (S32(d, 0x2D0) & 2) {
        return 0;
    }
    int ok = 1;
    f32 v = -F32(p, 0x10);
    s32 id = S32(p, 0x18);
    if (!(v > 4.4444447f)) {
        int same = 0;
        if (id == S32(d, 0x114) && v > 2.2222223f) {
            same = 1;
        }
        if (!same) {
            ok = 0;
        }
    }
    if (ok && id != S32(d, 0x2C4)) {
        f32 x = fn_80137850(p);
        f32 ax = (f32)__fabs(x);
        f32 lim = F32(d, 0x278) + F32(p, 0x4);
        f32 w = 5.0f + F32(d, 0x274);
        ax = ax < lim ? ax : lim;
        if (ax < w) {
            fn_80197AB8(d, id, F32(PTR(p, 0x0), 0x4), x, F32(p, 0x10));
            r = 1;
        }
    }
    return r;
}

void fn_80197AB8(u8* d, s32 id, f32 a, f32 b, f32 c) {
    S32(d, 0x100)++;
    int ok = 1;
    if (U8(d, 0xFC)) {
        ok = F32(d, 0xEC) > a;
    }
    if (ok) {
        F32(d, 0xEC) = a;
        u8* rb = fn_8017960C(id);
        V2 v;
        v.x = F32(rb, 0x88);
        v.y = F32(rb, 0x90);
        u8* o = VI(fn_801B2ED0(id))->v18();
        if (o) {
            U8(d, 0xFC) = 1;
            f32 t = F32(d, 0x27C) * a;
            F32(d, 0xF8) = b;
            S32(d, 0x114) = id;
            F32(d, 0x108) = F32(d, 0x278) + t;
            F32(d, 0x110) = c;
            F32(d, 0x104) = 1.1f * (F32(o, 0x38) * v.x + F32(o, 0x3C) * v.y);
        }
    }
}

void fn_80197BD0(u8* d, u8* out) {}

void fn_8019813C(u8* d, u8* p, f32 a, f32 b);

int fn_80197FE4(u8* d, u8* p) {
    int r = 0;
    if (!(S32(d, 0x2D0) & 4)) {
        int b = 1;
        f32 lo = lbl_8055A48C;
        s32 st = S32(d, 0x268);
        f32 hi = 1.2f * lbl_8055A488;
        f32 lim = lo + 5.0f * lo;
        if (st != 0 && st != 3) {
            b = 0;
        }
        f32 k = b ? 0.5f : 1.0f;
        F32(d, 0x134) = k;
        f32 x = F32(p, 0x8) + k * F32(p, 0x10);
        f32 z = F32(p, 0x4) + k * F32(p, 0xC);
        int c1 = F32(p, 0x8) > 0.5f * lbl_8055A48C;
        int c2 = 0;
        int c3 = 0;
        if (x < lim && x > -5.0f * lbl_8055A48C) {
            c3 = 1;
        }
        if (c3 && c1) {
            c2 = 1;
        }
        int c4 = (f32)__fabs(z) < hi;
        u8* o = lbl_804349C0[S32(p, 0x18)];
        int c5 = 1;
        if (o && U8(o, 0x2A5)) {
            c5 = 0;
        }
        if (c5 && c2 && c4) {
            fn_8019813C(d, p, F32(p, 0x8), x);
            r = 1;
        }
    }
    return r;
}

void fn_8019813C(u8* d, u8* p, f32 a, f32 b) {
    int ok = 1;
    if (U8(d, 0x11C) && a > F32(d, 0x124)) {
        ok = 0;
    }
    if (ok) {
        U8(d, 0x11C) = 1;
        F32(d, 0x128) = b;
        F32(d, 0x124) = a;
        F32(d, 0x12C) = F32(p, 0x4);
        F32(d, 0x130) = (F32(d, 0x270) + F32(p, 0x10)) - 0.8888889f;
        S32(d, 0x138) = S32(p, 0x18);
    }
}

void fn_801981A0(u8* d, u8* out) {}

// MATCHING-AID: volatile forces the second read of each count (found by upstream); the original
// probably re-read it through some accessor. Revisit.
void fn_80198488(u8* d) {
    volatile u8* pa = d + 0x74;
    volatile u8* pb = d + 0x9C;
    u8 b = *pb;
    u8 a = *pa;
    if (a + b + 1 > (int)F32(d, 0x2F4)) {
        int z = S32(d, 0x18) == 0;
        if (z && a) {
            fn_80193138(d, *pa ? S32(d, 0x90) : -1);
        } else if (!z && b) {
            fn_80193138(d, *pb ? S32(d, 0xB8) : -1);
        }
    }
}

void fn_80198530(u8* d) {}

u8* fn_80198DD0(u8* d, int del) {
    if (d) {
        *(void***)d = lbl_80333390;
        for (int i = 0; i < lbl_804349E0.count; i++) {
            if (lbl_804349E0.items[i] == d) {
                lbl_804349E0.items[i] = lbl_804349E0.items[--lbl_804349E0.count];
                break;
            }
        }
        if (del > 0) {
            operator delete(d);
        }
    }
    return d;
}

void fn_80198E7C(int car) {
    u8* o = lbl_804349C0[car];
    if (U8(o, 0x2A4)) {
        fn_801964D0(o);
    }
}

void fn_80198EA0(int car) { fn_80198E7C(car); }

u32 fn_80198EA4(u8* d) { return 0; }

}  // extern "C"
