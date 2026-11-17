// Car gearboxes (0x801D14EC-0x801D1C68): a Drawable that owns one gearbox per car. Each gearbox takes
// its ratios from the car, finds the upshift speeds by sweeping the engine curve (carelem) until the
// next gear pulls harder, and shifts automatically with a short delay between shifts. Names are
// guesses apart from the "carelmgr"/"carelem" tags.
#include "game/common.h"
#include "game/carbody.h"
#include "game/mathutil.h"

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

extern "C" {
void* memset(void* dst, int c, size_t n);
int abs(int x);
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

struct GearSrc {
    u8 pad0[0x2C];
    f32 f2C;        // offset 0x2C
    f32 wheel;      // offset 0x30
    s32 count;      // offset 0x34
    f32 ratio[7];   // offset 0x38
};

extern RaceCarRec* lbl_804348E8[8];
extern s32 lbl_8055E8F8;  // number of cars

#include "game/carelem.h"

extern CarElem* lbl_8055E908;

#include "game/cargear.h"


class CarGearMgr : public Drawable {
public:
    CarGearMgr(void) {}
    virtual ~CarGearMgr(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void vf18(void);
    virtual void vf1C(s32 car);
    virtual void vf20(s32 a);
};

ModuleTag lbl_8055E910;
CarGear* lbl_8055E918;

extern "C" void fn_801D14EC(void) { new ("carelmgr") CarGearMgr; }

void CarGear::Update(int car) { Fetch(car); }


void CarGear::Fetch(int car) {
    RaceCarRec* c = lbl_804348E8[car];
    CarElem* e = &lbl_8055E908[car];
    f28 = ((GearSrc*)&c->setupA)->wheel;
    f24 = 0.10471976f * Lerp(c->wheels[0].v3C, c->wheels[3].v3C, ((GearSrc*)&c->setupA)->f2C) / f28;
    GearSrc* src = (GearSrc*)&c->setupA;  // the tuned setup (CbSetup)
    f32 max = e->f30;
    memset(gears, 0, sizeof(gears));
    count = src->count;
    int i;
    for (i = 0; i < src->count; i++) {
        gears[i].ratio = src->ratio[i];
    }
    for (i = 0; i < count; i++) {
        Gear* g = &gears[i];
        f32 r = g->ratio;
        if (i < count - 1) {
            f32 next = g[1].ratio;
            f32 x;
            for (x = 3500.0f; x < max; x += 50.0f) {
                f32 s = x / r;
                f32 y = s * next;
                f32 now = r * (e->Torque(x) - e->f1C * x);
                if (next * (e->Torque(y) - e->f1C * y) > now) {
                    break;
                }
            }
            g->up = x / r;
        } else {
            g->up = 100000.0f;
        }
        if (i - 1 >= 0) {
            f32 v = gears[i - 1].up;
            f32 top = max / r;
            if (v > top) {
                v = top;
            }
            g->down0 = 0.85f * v;
            g->down1 = 0.9f * v;
        } else {
            g->down0 = 0.0f;
            g->down1 = 0.0f;
        }
    }
}

void CarGear::Clear(int car) {
    f0 = 0.0f;
    f4 = 0.0f;
    sinceShift = 0.0f;
    shiftTime = 0.0f;
    prev = 0;
    target = 0;
    gear = 0;
    request = 0;
}

void CarGear::Compute(int car) {
    RaceCarRec* c = lbl_804348E8[car];
    f32 load = c->f25F4;
    f4 = 0.0f;
    if (request == -10) {
        if (gear < 1) {
            gear = 1;
            target = 1;
        }
    } else {
        gear = request;
    }
    int g = target;
    if (request != -10) {
        target = request;
        g = request;
    } else if (g == gear) {
        if (g < 1) {
            g = 1;
            gear = 1;
        }
        Gear* cur = &gears[g - 1];
        f32 hi = c->f25D8;
        f32 top = 1.12f * hi;
        f32 lo = c->f25CC;
        f32 speed = (lo + 0.15f * (top - lo)) / f24;
        f32 down = Lerp(cur->down1, cur->down0, load);
        if (g > 0) {
            if (speed > cur->up) {
                if (g < count) {
                    g++;
                }
            } else if (speed < down) {
                if (g > 1) {
                    g--;
                }
            }
        }
        target = g;
    }
    if (g != gear) {
        int back = 0;
        if (g == prev && g < gear) {
            back = 1;
        }
        f32 wait = back ? 2.5f : 0.5f;
        if (g >= 1 && gear >= 1 && !(sinceShift > wait)) {
            g = gear;
            target = gear;
        }
    }
    sinceShift += 0.016683351f;
    if (gear != g) {
        shiftTime += 0.016683351f;
        if (g < 1 || gear < 1 || shiftTime > 0.25f) {
            prev = gear;
            gear = g;
            shiftTime = 0.0f;
            sinceShift = 0.0f;
        } else if (g > 1 && gear > g) {
            f4 = 0.8f;
        } else if (gear < g && gear != 0) {
            f4 = -1.0f;
        }
    } else {
        shiftTime = 0.0f;
    }
    f0 = g ? f28 * gears[abs(g) - 1].ratio : 0.0f;
    if (g < 0) {
        f0 *= -0.67f;
    }
}

CarGearMgr::~CarGearMgr(void) {}

void CarGearMgr::vf18(void) {
    delete[] lbl_8055E918;
    lbl_8055E918 = 0;
}

void CarGearMgr::vf20(s32 a) {}

void CarGearMgr::vf14(void) {
    int i;
    for (i = 0; i < lbl_8055E8F8; i++) {
        lbl_8055E918[i].Compute(i);
    }
}

void CarGearMgr::vf10(void) {
    int i;
    for (i = 0; i < lbl_8055E8F8; i++) {
        lbl_8055E918[i].Clear(i);
    }
}

void CarGearMgr::vf1C(s32 car) { lbl_8055E918[car].Update(car); }

void CarGearMgr::vf0C(void) { lbl_8055E918 = new ("carelem") CarGear[lbl_8055E8F8]; }

// NonMatching: Fetch and Compute pick different float registers (19 and 10 diffs, same instructions).
