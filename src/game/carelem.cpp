// Car element manager (0x801D1184-0x801D14EC): a Drawable that owns one engine-curve record per car and
// refreshes it from the car every frame. Names are guesses apart from the "carelmgr"/"carelem" tags.
#include "game/common.h"
#include "game/carbody.h"
#include "game/mathutil.h"

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

extern "C" {
void fn_801D0DDC(class Updatable* u);  // register
void fn_801D0DFC(void);                // unregister
}

#include "game/updatable.h"

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

extern RaceCarRec* lbl_804348E8[8];
extern s32 lbl_8055E8F8;  // number of cars

extern "C" void fn_801819A0(f32* out, CarCurveSrc* src, f32 a, f32 b, f32 c);

#include "game/carelem.h"

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
    RaceCarRec* c = lbl_804348E8[car];
    CarCurveSrc* src = (CarCurveSrc*)&c->setupA;  // the tuned setup (CbSetup)
    fn_801819A0(&f4, src, 1.25f * c->f2DC, c->f2E4, c->f2E8);
    f30 = src->f64;
}

void CarElem::Clear(int car) { f0 = 0.0f; }

/* Helper name unproven: evaluates the element curve at x (two quadratic pieces) scaled by k. */
inline void CarElem::ApplyCurve(f32 x, f32 k) {
    f32 t;
    if (x > f2C) {
        t = x - f20;
        t = 0.0f > t ? 0.0f : t;
        f32 sq = Sq(x);
        t = fC + (f8 * x + f4 * sq) - f24 * t;
        t = f28 > t ? f28 : t;
    } else {
        f32 sq = Sq(x);
        t = f18 + (f14 * x + f10 * sq);
    }
    f0 = k * t - x * f1C;
}

void CarElem::Compute(int car) {
    RaceCarRec* c = lbl_804348E8[car];
    ApplyCurve(c->f25D0, c->f25F4);  // rpm, load
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

// Every function matches (2026-11-14 audit); not linked yet - run flipcheck.py before flipping it to Matching.
