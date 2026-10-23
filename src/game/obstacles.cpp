// Track obstacles (0x801A9660-0x801ADB44): the obstacle factory ("fencepst", "obst", "DynamicObstacle"),
// static and dynamic obstacle rigid bodies, the extents/plane/polyvol/pmshvol readers and the
// fence-pole binding ("fencepole: binding to %p"). Written against raw field offsets; the large
// routines are still stubs. Names are guesses apart from the strings.
#include "game/common.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

class VO {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(const char* a, const char* b);
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
    virtual void v48(const f32* push, const f32* at, int force, int flags);
};

// A 12-word body pose.
struct Pose12 {
    u32 w[12];
};

extern s32 lbl_8055E3C4;

struct ObVec3 {
    f32 x, y, z;
};

// A visitor called for each obstacle in a linked chain.
class VVisit {
public:
    virtual void Visit(u8* o);
};

class CollVolume {
public:
    void Update(void);
};

class RigidBody {
public:
    void Reset(void);
};

// A handle whose constructor marks it unused.
struct ObHandle {
    s32 v;
    ObHandle(void) { v = 0xFFFF0000; }
};

extern "C" {
u8* __ct__9RigidBodyFv(u8* p);
void __dt__10CollVolumeFv(void* p, int del);
void vf20__10CollVolumeFv(u8* p);
s32 fn_801C1BA4(s32 a, s32 b);
void fn_801D08C0(u8* p);
void fn_801D0A24(u8* p, int a);
void fn_801A9AB0(u8* p, u8* q);
void fn_801C1C7C(u8* p);
void fn_802E30C8(const char* fmt, ...);
f32 PSVECSquareDistance(const u8* a, const u8* b);
void fn_801AC240(u8* p);
void fn_801ABFF8(u8* p, VVisit* f);
void fn_801ACF94(u8* p);
void fn_801AD8C8(u8* p, const Pose12* pose, const ObVec3* lo, const ObVec3* hi);
void fn_801AD99C(u8* p, f32* v, f32* off);
void fn_801D09A8(u8* p, s32 id);
}

void operator delete(void* p);
void* operator new(unsigned long n, const char* tag);

extern void* lbl_8033585C[];
extern void* lbl_80335808[];
extern u8* lbl_80435628[];

// Cars, player cars, AI cars and network cars all carry a car tag.
static inline int IsCar(u8* o) {
    u32 t = U32(o, 0x10);
    return t == 'CAR ' || t == 'PCAR' || t == 'ACAR' || t == 'NCAR';
}
extern void* lbl_80335744[];
extern void* lbl_80335940[];
extern f32 lbl_8055E3C8;

ModuleTag lbl_8055E5D8;
ObHandle lbl_8055E5E0;
s32 lbl_8055E5E4;
s32 lbl_8055E5E8;
s32 lbl_8055E5EC;
s32 lbl_8055E5F0;

extern "C" {

void fn_801A9660(u8* p) {}  // stub (40)

u8* fn_801A9700(u8* p) {
    __ct__9RigidBodyFv(p);
    *(void**)p = lbl_8033585C;
    return p;
}

u8* fn_801A973C(u8* p, int del) {
    if (p) {
        if (p) {
            __dt__10CollVolumeFv(p, 0);
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

void fn_801A9798(u8* p) {}  // stub (198)

void fn_801A9AB0(u8* p, u8* q) {}  // stub (2089)

void fn_801ABB54(u8* p) {}  // stub (53)

void fn_801ABC28(u8* p) {}  // stub (243)

void fn_801ABFF4(u8* p) { vf20__10CollVolumeFv(p); }

void fn_801ABFF8(u8* p, VVisit* f) {
    u8* o = PTR(p, 0x224);
    if (o && U8(p, 0x223) && lbl_8055E3C4 > 1) {
        while (o != p) {
            U8(o, 0x223) = 0;
            f->Visit(o);
            o = PTR(o, 0x224);
        }
        U8(p, 0x223) = 0;
    }
    fn_801AC240(p);
}

void fn_801AC0A0(u8* p, int a, int b, int c, u8* car) {}  // stub (57)

void fn_801AC184(u8* p) {
    fn_801AC240(p);
    *(Pose12*)(p + 0x3C) = *(Pose12*)(p + 0x1EC);
    ((CollVolume*)p)->Update();
    ((RigidBody*)p)->Reset();
    U8(p, 0x223) = 1;
    U8(p, 0x234) = U8(p, 0x235);
}

void fn_801AC240(u8* p) {
    U8(p, 0x220) = 1;
    F32(p, 0x21C) = lbl_8055E3C8;
}

void fn_801AC254(u8* p) {
    if (U8(p, 0x220)) {
        U8(p, 0x220) = 0;
    }
}

u8* fn_801AC26C(void) {
    u8* o = (u8*)operator new(0x244, "DynamicObstacle");
    if (o) {
        fn_801A9700(o);
        *(void**)o = lbl_80335744;
        *(void**)(o + 0x23C) = (u8*)lbl_80335744 + 0x54;
    }
    return o;
}

u8* fn_801AC2C8(u8* p, int del) {
    if (p) {
        lbl_8055E5E4 = 0;
        if (p) {
            if (p) {
                __dt__10CollVolumeFv(p, 0);
            }
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

u8* fn_801AC330(u8* p) {
    fn_801A9700(p);
    *(void**)p = lbl_80335808;
    if (!lbl_8055E5EC) {
        lbl_8055E5F0 = 0;
    }
    S32(p, 0x250) = lbl_8055E5EC++;
    lbl_80435628[S32(p, 0x250)] = p;
    S32(p, 0x254) = -1;
    return p;
}

u8* fn_801AC3AC(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_80335808;
        if (--lbl_8055E5EC == 0 && lbl_8055E5F0) {
            fn_801C1C7C((u8*)lbl_8055E5F0);
            lbl_8055E5F0 = 0;
        }
        if (p) {
            lbl_8055E5E4 = 0;
            if (p) {
                if (p) {
                    __dt__10CollVolumeFv(p, 0);
                }
            }
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

void fn_801AC454(u8* p, u8* q) {
    U32(p, 0x23C) = 0;
    U32(p, 0x240) = 0;
    fn_801A9AB0(p, q);
    s32 idx = S32(p, 0x250);
    F32(p, 0x248) = F32(q, 0x5C);
    F32(p, 0x244) = 0.0f;
    F32(p, 0x24C) = 0.0f;
    U32(p, 0x23C) = 0;
    if (idx > 0) {
        u8* prev = lbl_80435628[idx - 1];
        if (PSVECSquareDistance(prev + 0x60, p + 0x60) < 16.0f) {
            PTR(p, 0x23C) = prev;
            fn_802E30C8("fencepole: binding to %p", prev);
        }
    }
}

void fn_801AC508(void) {
    s32 n = lbl_8055E5EC;
    if (n > 0) {
        s32 t = n * 5;
        lbl_8055E5F0 = fn_801C1BA4(t, t - 1);
    }
}

void fn_801AC544(u8* p) {}  // stub (321)

void fn_801ACA48(u8* p) {}  // stub (58)

void fn_801ACB30(u8* p) {}  // stub (281)

void fn_801ACF94(u8* p) {}  // stub (83)

void fn_801AD0E0(u8* p, const f32* a, s32 id, const f32* b, f32 x, f32 y) {
    f32 t = 0.02f * x;
    S32(p, 0xC) = 0;
    F32(p, 0x28) = 0.0f;
    F32(p, 0x2C) = 0.0f;
    F32(p, 0x0) = a[0];
    F32(p, 0x4) = a[1];
    F32(p, 0x8) = a[2];
    S32(p, 0xC) = id;
    F32(p, 0x10) = b[0];
    F32(p, 0x14) = b[1];
    F32(p, 0x18) = b[2];
    F32(p, 0x1C) = x;
    F32(p, 0x24) = y;
    F32(p, 0x20) = 0.1f * t;
}

void fn_801AD148(u8* p) {}  // stub (81)

void fn_801AD28C(u8* p) {}  // stub (66)

void fn_801AD394(u8* p) {}  // stub (150)

void fn_801AD5EC(void) { fn_801AC508(); }

void fn_801AD5F0(u8* p) { fn_801ACF94(p); }

void fn_801AD5F4(u8* p) {}  // stub (47)

void fn_801AD6B0(u8* p) {}  // stub (40)

void fn_801AD750(u8* p, u8* q) {
    fn_801A9AB0(p, q);
    U32(p, 0x240) = U32(q, 0x80);
    u8* b = p;
    if (b) {
        b += 0x23C;
    }
    fn_801D08C0(b);
}

void fn_801AD7A4(u8* p, int a, int b, int c) {
    if (c < 8) {
        if (p) {
            p += 0x23C;
        }
        fn_801D0A24(p, b);
    }
}

void fn_801AD7C4(u8* p, int a, int b, int c, u8* car) {
    fn_801AC0A0(p, a, b, c, car);
    if (car && IsCar(car)) {
        s32 id = S32(car, 0x1EC);
        if (p) {
            p += 0x23C;
        }
        fn_801D09A8(p, id);
    }
}

u8 fn_801AD860(u8* p) { return U8(p, 0x220); }

u32 fn_801AD868(u8* p) { return 0; }

void fn_801AD870(u8* p, int a, int b, s32* c, s32* d) {
    *d = 0;
    *c = 0;
}

u32 fn_801AD880(u8* p) { return 0; }

void fn_801AD888(u8* p) {
    ((VO*)p)->v10();
    fn_801AC254(p);
}

void fn_801AD8C8(u8* p, const Pose12* pose, const ObVec3* lo, const ObVec3* hi) {
    *(Pose12*)(p + 0x3C) = *pose;
    ((CollVolume*)p)->Update();
    F32(p, 0x118) = lo->x;
    F32(p, 0x11C) = lo->y;
    F32(p, 0x120) = lo->z;
    F32(p, 0x124) = hi->x;
    F32(p, 0x128) = hi->y;
    F32(p, 0x12C) = hi->z;
    fn_801AC240(p);
}

void fn_801AD99C(u8* p, f32* v, f32* off) {
    f32 push[3];
    f32 at[3];
    f32 s = F32(p, 0xD4) / 0.01668335f;
    push[0] = v[0] * s;
    push[1] = v[1] * s;
    push[2] = v[2] * s;
    at[0] = F32(p, 0x60) + off[0];
    at[1] = F32(p, 0x64) + off[1];
    at[2] = F32(p, 0x68) + off[2];
    ((VO*)p)->v48(push, at, 100, 0);
}

void fn_801ADA38(u8* p, int on) {
    if (on) {
        void* f = lbl_80335940;
        fn_801ABFF8(p, (VVisit*)&f);
    } else {
        U8(p, 0x220) = 0;
    }
}

u8 fn_801ADA7C(u8* p) { return U8(p, 0x220); }

s32 fn_801ADA84(u8* p) { return *(s16*)(p + 0x6C); }

u8* fn_801ADA8C(u8* p) { return p + 0x118; }

u8* fn_801ADA94(u8* p) { return p + 0x60; }

void fn_801ADA9C(u8* p) { ((VO*)PTR(p, 0x240))->v18("dyno", "unknown"); }

u32 fn_801ADAB8(u8* p) { return U32(p, 0x240); }

void fn_801ADAC0(u8* self, u8* p) { fn_801AC240(p); }



void fn_801ADAF4(u8* p) { fn_801AD888(p - 0x23C); }

void fn_801ADAFC(u8* p, const Pose12* pose, const ObVec3* lo, const ObVec3* hi) { fn_801AD8C8(p - 0x23C, pose, lo, hi); }

void fn_801ADB04(u8* p, f32* v, f32* off) { fn_801AD99C(p - 0x23C, v, off); }

void fn_801ADB0C(u8* p, int on) { fn_801ADA38(p - 0x23C, on); }

void fn_801ADB14(u8* p) { fn_801ADA7C(p - 0x23C); }

void fn_801ADB1C(u8* p) { fn_801ADA84(p - 0x23C); }

void fn_801ADB24(u8* p) { fn_801ADA8C(p - 0x23C); }

void fn_801ADB2C(u8* p) { fn_801ADA94(p - 0x23C); }

void fn_801ADB34(u8* p) { fn_801ADA9C(p - 0x23C); }

void fn_801ADB3C(u8* p) { fn_801ADAB8(p - 0x23C); }
}  // extern "C"
