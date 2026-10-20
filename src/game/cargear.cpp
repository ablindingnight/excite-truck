// Car gearboxes (0x801D14EC-0x801D1C68): a Drawable that owns one gearbox per car. Each gearbox takes
// its ratios from the car, finds the upshift speeds by sweeping the engine curve (carelem) until the
// next gear pulls harder, and shifts automatically with a short delay between shifts. Names are
// guesses apart from the "carelmgr"/"carelem" tags.
#include "game/common.h"

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

struct RaceCar {
    u8 pad0[0x32C];
    GearSrc gears;     // offset 0x32C
    u8 pad[0x23E8 - 0x32C - sizeof(GearSrc)];
    f32 f23E8;         // offset 0x23E8
    u8 pad23EC[0x2544 - 0x23EC];
    f32 f2544;         // offset 0x2544
    u8 pad2548[0x25CC - 0x2548];
    f32 f25CC;         // offset 0x25CC
    u8 pad25D0[0x25D8 - 0x25D0];
    f32 f25D8;         // offset 0x25D8
    u8 pad25DC[0x25F4 - 0x25DC];
    f32 load;          // offset 0x25F4
};

extern RaceCar* lbl_804348E8[8];
extern s32 lbl_8055E8F8;  // number of cars

// The engine curve (carelem).
struct CarElem {
    f32 Torque(f32 x) {
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
        return t;
    }
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

extern CarElem* lbl_8055E908;

struct Gear {
    f32 ratio;  // offset 0x0
    f32 up;     // offset 0x4: upshift speed
    f32 down0;  // offset 0x8: downshift speed at full load
    f32 down1;  // offset 0xC: downshift speed at no load
};

struct CarGear {
    CarGear(void) {}
    void Update(int car);
    void Fetch(int car);
    void Clear(int car);
    void Compute(int car);
    f32 f0;         // offset 0x0
    f32 f4;         // offset 0x4
    s32 request;    // offset 0x8: -10 for automatic
    s32 target;     // offset 0xC
    s32 prev;       // offset 0x10
    s32 gear;       // offset 0x14
    f32 shiftTime;  // offset 0x18
    f32 sinceShift; // offset 0x1C
    s32 count;      // offset 0x20
    f32 f24;        // offset 0x24
    f32 f28;        // offset 0x28
    Gear gears[7];  // offset 0x2C
};

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
    RaceCar* c = lbl_804348E8[car];
    CarElem* e = &lbl_8055E908[car];
    f28 = c->gears.wheel;
    f24 = 0.10471976f * (c->f23E8 + c->gears.f2C * (c->f2544 - c->f23E8)) / f28;
    GearSrc* src = &c->gears;
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
    RaceCar* c = lbl_804348E8[car];
    f32 load = c->load;
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
        f32 lo = c->f25CC;
        f32 down = cur->down1 + load * (cur->down0 - cur->down1);
        f32 speed = (lo + 0.15f * (1.12f * hi - lo)) / f24;
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
