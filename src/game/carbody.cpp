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

struct RVec3;

class RigidBody {
public:
    RigidBody(void);
    void OnHit(const RVec3* a, const RVec3* b, int c, void* d);
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

// The whole tuning table, read through raw offsets.
struct TuneTable {
    u8 raw[0xB8];
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
void fn_801D14EC(void);
void fn_801BD808(void);
}

extern TuneTable lbl_804348E8;
extern TuneBlock lbl_80434908;
extern TuneBlock lbl_80434930;
extern TuneBlock lbl_80434950;
extern u8 lbl_8055E4F4;
extern f32 lbl_8055E3C8;
extern f32 lbl_8055E4E0;
extern u8* lbl_8055E918;
extern s32 lbl_8055E4D0;

ModuleTag lbl_8055E4C0;
CarHandle lbl_8055E4CC;
UnkStatic7F lbl_8055E4C8;

extern "C" {

void fn_8018185C(u8* p) {}  // stub (81)

void fn_801819A0(u8* p) {}  // stub (29)

f32 fn_80181A14(u8* p) {
    f32 k;
    if (U8(p, 0x259D)) {
        k = lbl_80434930.v[6];
    } else {
        k = lbl_80434908.v[6];
    }
    return F32(p, 0x2D8) * k;
}

f32 fn_80181A48(u8* p) {
    u8* b = lbl_804348E8.raw;
    f32 k;
    if (U8(p, 0x259D)) {
        b += 0x48;
        k = F32(b, 0x1C);
    } else if (lbl_8055E4F4) {
        b += 0x68;
        k = F32(b, 0x34);
    } else {
        b += 0x20;
        k = F32(b, 0x1C);
    }
    return F32(p, 0x2D0) + k;
}

f32 fn_80181A94(void) {
    if (lbl_8055E4F4) {
        return lbl_80434950.v[14];
    }
    return lbl_80434908.v[2];
}

f32 fn_80181AC0(void) {
    if (lbl_8055E4F4) {
        return lbl_80434950.v[15];
    }
    return lbl_80434908.v[3];
}

f32 fn_80181AEC(void) { return lbl_80434930.v[3]; }

void fn_80181AFC(u8* p) {}  // stub (236)

void fn_80181EAC(u8* p) {}  // stub (33)

void fn_80181F30(u8* p) {}  // stub (37)

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
    ((CarBody**)lbl_804348E8.raw)[lbl_8055E4D0++] = this;
}

extern "C" {

void fn_80182074(u8* p) {}  // stub (864)

void fn_80182DF4(u8* p) {}  // stub (570)

void fn_801836DC(void) {}

}  // extern "C"

CarBody::~CarBody(void) {}  // stub (63)

extern "C" {

u8* fn_801837DC(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_8018381C(u8* p) {}  // stub (43)

void fn_801838C8(u8* p) {}  // stub (332)

void fn_80183DF8(u8* p) {}  // stub (33)

void fn_80183E7C(u8* p) {}  // stub (3032)

u32 fn_80186DDC(u8* p) { return 0; }

void fn_80186DE4(u8* p) { fn_8018A8A8(p); }

void fn_80186DE8(u8* p) {}  // stub (1830)

void fn_80188A80(u8* p) {}  // stub (233)

u32 fn_80188E24(u8* p) { return 0; }

void fn_80188E2C(u8* p) {}  // stub (464)

void fn_8018956C(u8* p) {}  // stub (412)

void fn_80189BDC(u8* p) {}  // stub (34)

void fn_80189C64(u8* p) {
    if (U8(p, 0x26AC)) {
        U8(p, 0x26AC) = 0;
        fn_801BF1B8(PTR(p, 0x1EC), 0);
        F32(p, 0x26A8) = 0.0f;
    }
}

void fn_80189CB4(u8* p) {}  // stub (245)

void fn_8018A088(u8* p) {}

void fn_8018A08C(u8* p) { F32(p, 0x26D8) = lbl_8055E3C8; }

void fn_8018A098(u8* p, f32 t) {
    U8(p, 0x259D) = 1;
    f32 m = F32(p, 0x2CC) * fn_80181AEC();
    F32(p, 0x258C) = F32(p, 0x3A0) * (m + t * (lbl_80434930.v[0] - m));
}

int fn_8018A100(u8* p) { return F32(p, 0x2618) > lbl_80434930.v[1]; }

void fn_8018A120(u8* p) {
    if (!fn_801B2F2C(PTR(p, 0x1EC))) {
        F32(p, 0x25BC) = lbl_8055E4E0;
        ((VR*)PTR(p, 0x2584))->v38();
    }
}

void fn_8018A174(u8* p) { F32(p, 0x2650) = 4.0f; }

void fn_8018A180(u8* p) {}  // stub (79)

void fn_8018A2BC(u8* p) {}

void fn_8018A2C0(u8* p) {}  // stub (229)

void fn_8018A654(u8* p) {}  // stub (40)

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

void fn_8018A750(u8* p) {}  // stub (86)

void fn_8018A8A8(u8* p) {}  // stub (82)

void fn_8018A9F0(u8* p) {}  // stub (237)

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

void fn_8018AE5C(u8* p) {}  // stub (8)

void fn_8018AE7C(u8* p) {}  // stub (33)

void fn_8018AF00(u8* p) {}

void fn_8018AF04(u8* p) {}  // stub (90)

u32 fn_8018B06C(u8* p) { return U32(lbl_8055E918 + S32(p, 0x1EC) * 0x9C, 0x14); }

u32 fn_8018B084(u8* p) { return U32(lbl_8055E918 + S32(p, 0x1EC) * 0x9C, 0x20); }

void fn_8018B09C(u8* p, const RVec3* a, const RVec3* b, int c, void* d) {
    ((RigidBody*)p)->OnHit(a, b, c, d);
    F32(p, 0x26DC) = lbl_8055E3C8;
}

void fn_8018B0D0(u8* p, u32 v) { U32(p, 0x2610) = v; }

void fn_8018B0D8(u8* p) {}  // stub (607)

void fn_8018BA54(u8* p) {}  // stub (230)

void fn_8018BDEC(u8* p) {
    f32 m[12];
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
        fn_801C2BF8(PTR(p, 0x26C4), m);
    }
}

void fn_8018BE54(u8* p) {}  // stub (60)

void fn_8018BF44(u32* p) { *p = 0x0FF807FF; }

u32 fn_8018BF54(u8* p) { return 0; }

void fn_8018BF5C(u8* p) {}

void fn_8018BF60(u8* p) {}

int fn_8018BF64(void) { return 0x44; }

}  // extern "C"
