// Player car driver (0x8018FB18-0x8019309C): the PlayerDriver/LocalCar side of the car code - input
// to throttle/steer/brake, replay packet recording ("replaypkt"), force-feedback jolts ("ff_jolt"),
// the hybrid control mode, and the per-car wrapper objects that forward to the physics car. Written
// against raw field offsets; the large routines are still stubs. Names are guesses apart from the
// strings.
#include "game/common.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
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
    virtual void v38(void);
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
};

#define VP(p) ((VPx*)(p))

static inline void SetMode(u8* p, int on) {
    U8(p, 0x25) = on;
    U8(p, 0x26) = on == 0;
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
void fn_80191084(u8* p, int a);
void fn_80191758(u8* p);
int fn_801B526C(u8* p);
int fn_801769F8(void);
void fn_801B6650(u8* p);
void fn_802793B0(u8* p);
void fn_801797A8(u8* p, const char* fmt, ...);
u32 fn_80191100(u8* p);
void fn_801B6660(void);
int fn_801CF050(u8* p, int a);
void fn_801B6658(void);
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

void fn_8018FB18(u8* p) {}  // stub (72)

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

void fn_8018FD88(u8* p) {}  // stub (65)

void fn_8018FE8C(u8* p) {}  // stub (62)

u8* fn_8018FF84(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_8018FFC4(u8* p) {}  // stub (112)

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

int fn_80190368(int x) {
    int r = 0;
    switch (x) {
    case 1:
    case 0:
        r = 1;
        break;
    }
    return r;
}

void fn_80190388(u8* p) {}  // stub (502)

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
        fn_801B665C();
    }
}

void fn_80190EAC(u8* p) {
    int z = 0;
    SetMode(p, z);
    lbl_8055E520 = z;
    u8* o = PTR(p, 0x4);
    if (fn_801CF050(p, 1)) {
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

void fn_80191084(u8* p, int a) {}

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

void fn_801912D8(u8* p) {}  // stub (136)

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

void fn_80191758(u8* p) {}  // stub (66)

void fn_80191860(u8* p) {}  // stub (50)

void fn_80191928(u8* p) {}  // stub (92)

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

void fn_80192C7C(u8* p) {}  // stub (37)

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

void fn_80192E3C(u8* p) {}  // stub (37)

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
