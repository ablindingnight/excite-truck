#ifndef GAME_CARMODEL_H
#define GAME_CARMODEL_H
// A race car as the world draws it (carmodel.cpp): its LOD models, the dashboard gauges and needle sliders, the
// shadow LODs and the deform/x-ray state; lbl_80488648 holds the eight. One declaration for every unit (port ODR
// work, 2026-11-16). NeedleSlider was carmodel.cpp's Slider (uiwidget.h's UI Slider owns that name). Names
// unproven apart from the mangled CarModel/PlayerCarModel/Gauge.
#include "game/common.h"
#include "game/animobj.h"
#include "game/vecmatrix.h"

struct Racer;
struct CarDef;
struct Joint;
struct CarDrawView;
struct CarRes;
struct AnimSlot;

struct GaugePair {
    f32 val;    // offset 0x0
    f32 angle;  // offset 0x4
};

// A needle bone driven by one float through a piecewise-linear value->angle table.
struct Gauge {
    Gauge(void) {
        bone = 0;
        src = 0;
    }
    ~Gauge(void);

    Joint* bone;           // offset 0x0
    f32 min;               // offset 0x4
    f32 max;               // offset 0x8
    GaugePair pairs[8];    // offset 0xC
    s32 count;             // offset 0x4C
    const f32* src;        // offset 0x50
};

struct NeedleSlider {
    const f32* src;  // offset 0x0
    f32 ky;          // offset 0x4
    f32 by;          // offset 0x8
    f32 kz;          // offset 0xC
    f32 bz;          // offset 0x10
};

struct LodEntry {
    f32 dist;     // offset 0x0
    u8 noMirror;  // offset 0x4
    u8 rider;     // offset 0x5
    u8 wheels;    // offset 0x6
};

struct BoneSurf {
    s32 first;  // offset 0x0
    s32 last;   // offset 0x4
    u32 mask;   // offset 0x8
};

struct ShadowLod {
    s32 model;  // offset 0x0
    f32 dist;   // offset 0x4
};

class CarModel : public AnimObj {
public:
    CarModel(Racer* r, const CarDef* d);
    virtual ~CarModel(void);
    virtual void Draw(void);
    virtual void PostDraw(void);
    virtual void vf1C(void);
    virtual void Set(const AnimSlot* s);
    virtual void vf24(void);

    CarDrawView* car;          // offset 0x3C
    s32 index;              // offset 0x40
    s32 lodModels[16];      // offset 0x44
    s32 drawModels[16];     // offset 0x84
    s32 unkC4;              // offset 0xC4
    LodEntry lodTable[16];  // offset 0xC8
    s32 numLods;            // offset 0x148
    s32 mirrorLod;          // offset 0x14C
    s32 numBoneSurfs;       // offset 0x150
    BoneSurf boneSurfs[2];  // offset 0x154
    s32* models;            // offset 0x16C
    LodEntry* lods;         // offset 0x170
    s32* lodCount;          // offset 0x174
    s32 chassis;            // offset 0x178
    Joint* wheelBone;       // offset 0x17C
    Joint* shiftLight;      // offset 0x180
    void* rider;            // offset 0x184
    CarRes* res;            // offset 0x188
    Gauge speedometer;      // offset 0x18C
    Gauge tachometer;       // offset 0x1E0
    Gauge oilPressure;      // offset 0x234
    Gauge oilTemp;          // offset 0x288
    Gauge waterTemp;        // offset 0x2DC
    Gauge fuel;             // offset 0x330
    Gauge volts;            // offset 0x384
    NeedleSlider speedoSlider;    // offset 0x3D8
    s32 color;              // offset 0x3EC
    void* tire;             // offset 0x3F0
    u8 drawWheel;           // offset 0x3F4
    s32 curLod;             // offset 0x3F8
    s32 minLod;             // offset 0x3FC
    ShadowLod shadows[16];  // offset 0x400
    s32 numShadows;         // offset 0x480
    f32 shadowRadius;       // offset 0x484
    s32 xray;               // offset 0x488
    s32 facingPos;          // offset 0x48C
    s32 facingNeg;          // offset 0x490
    s32 xrayBones;          // offset 0x494
    u32 suspGroup;          // offset 0x498
    u32 xrayMask;           // offset 0x49C
    VecMatrix scaled;           // offset 0x4A0
    f32 scale;              // offset 0x4D0
    void* wheels[4];        // offset 0x4D4
};

class PlayerCarModel : public CarModel {
public:
    PlayerCarModel(Racer* r, CarDef* d);
    virtual ~PlayerCarModel(void);
    virtual void Set(const AnimSlot* s);
    virtual int vf28(void);
};

#endif
