// Car element manager (0x801D1184-0x801D14EC): a Drawable that owns one engine-curve record per car and
// refreshes it from the car every frame. Names are guesses apart from the "carelmgr"/"carelem" tags.
#include "game/common.h"

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

extern "C" {
void fn_801D0DDC(class Updatable* u);  // register
void fn_801D0DFC(void);                // unregister
}

class Updatable {
public:
    virtual ~Updatable(void);
    virtual void vf0C(void) = 0;
    virtual void vf10(void) = 0;
    virtual void vf14(void) = 0;
    virtual void vf18(void) = 0;
};

class Drawable : public Updatable {
public:
    Drawable(void);
    virtual ~Drawable(void);
    virtual void vf1C(s32 a) = 0;
    virtual void vf20(s32 a) = 0;
};

struct CarCurveSrc {
    u8 pad0[0x64];
    f32 f64;  // offset 0x64
};

struct RaceCar {
    u8 pad0[0x2DC];
    f32 f2DC;            // offset 0x2DC
    u8 pad2E0[4];
    f32 f2E4;            // offset 0x2E4
    f32 f2E8;            // offset 0x2E8
    u8 pad2EC[0x32C - 0x2EC];
    CarCurveSrc curve;   // offset 0x32C
    u8 pad394[0x25D0 - 0x394];
    f32 rpm;             // offset 0x25D0
    u8 pad25D4[0x25F4 - 0x25D4];
    f32 load;            // offset 0x25F4
};

extern RaceCar* lbl_804348E8[8];
extern s32 lbl_8055E8F8;  // number of cars

extern "C" void fn_801819A0(f32* out, CarCurveSrc* src, f32 a, f32 b, f32 c);

struct CarElem {
    CarElem(void) {
        f4 = f8 = fC = f10 = f14 = f18 = 0.0f;
        f1C = 0.0f;
        f20 = 0.0f;
    }
    void Update(int car);
    void Fetch(int car);
    void Clear(int car);
    void Compute(int car);
    f32 f0;   // offset 0x0
    f32 f4;   // offset 0x4
    f32 f8;   // offset 0x8
    f32 fC;   // offset 0xC
    f32 f10;  // offset 0x10
    f32 f14;  // offset 0x14
    f32 f18;  // offset 0x18
    f32 f1C;  // offset 0x1C
    f32 f20;  // offset 0x20
    f32 f24;  // offset 0x24
    f32 f28;  // offset 0x28
    f32 f2C;  // offset 0x2C
    f32 f30;  // offset 0x30
};

class CarElemMgr : public Drawable {
public:
    CarElemMgr(void) {}
    virtual ~CarElemMgr(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void vf18(void);
    virtual void vf1C(s32 car);
    virtual void vf20(s32 a);
};

ModuleTag lbl_8055E900;
CarElem* lbl_8055E908;

extern "C" void fn_801D1184(void) { new ("carelmgr") CarElemMgr; }

void CarElem::Update(int car) { Fetch(car); }

void CarElem::Fetch(int car) {
    RaceCar* c = lbl_804348E8[car];
    CarCurveSrc* src = &c->curve;
    fn_801819A0(&f4, src, 1.25f * c->f2DC, c->f2E4, c->f2E8);
    f30 = src->f64;
}

void CarElem::Clear(int car) { f0 = 0.0f; }

void CarElem::Compute(int car) {
    RaceCar* c = lbl_804348E8[car];
    f32 x = c->rpm;
    f32 k = c->load;
    f32 t;
    if (x > f2C) {
        t = x - f20;
        t = 0.0f > t ? 0.0f : t;
        f32 sq = x * x;
        t = fC + (f8 * x + f4 * sq) - f24 * t;
        t = f28 > t ? f28 : t;
    } else {
        f32 sq = x * x;
        t = f18 + (f14 * x + f10 * sq);
    }
    f0 = k * t - x * f1C;
}

CarElemMgr::~CarElemMgr(void) {}

void CarElemMgr::vf18(void) {
    delete[] lbl_8055E908;
    lbl_8055E908 = 0;
}

void CarElemMgr::vf20(s32 a) {}

void CarElemMgr::vf14(void) {
    int i;
    for (i = 0; i < lbl_8055E8F8; i++) {
        lbl_8055E908[i].Compute(i);
    }
}

void CarElemMgr::vf10(void) {
    int i;
    for (i = 0; i < lbl_8055E8F8; i++) {
        lbl_8055E908[i].Clear(i);
    }
}

void CarElemMgr::vf1C(s32 car) { lbl_8055E908[car].Update(car); }

void CarElemMgr::vf0C(void) { lbl_8055E908 = new ("carelem") CarElem[lbl_8055E8F8]; }

// NonMatching: Compute colors its float registers differently (18 diffs, same instructions).
