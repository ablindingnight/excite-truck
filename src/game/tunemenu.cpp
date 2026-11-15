// Tuning menu (0x80257C10-0x80257FB8): the garage screen's sliders for a car's handling scales.
// Names are guesses apart from the layout and element names.
#include "game/common.h"
#include "game/menu.h"

// Handling scales edited by the garage sliders.
struct TuneScales {
    f32 mass;        // offset 0x0
    f32 massMoment;  // offset 0x4
    f32 power;       // offset 0x8
    f32 gearRatio;   // offset 0xC
    f32 bump;        // offset 0x10
    f32 rebound;     // offset 0x14
    f32 spring;      // offset 0x18
    f32 grip;        // offset 0x1C
    f32 slip;        // offset 0x20
};

static inline int FloatBits(f32 f) { return *(int*)&f; }

extern "C" {
TuneScales* fn_8027A798(const char*);  // the current car's scales
extern char lbl_8055B860[8];
}

ModuleTag lbl_8055F358;
static UnkStatic123 lbl_80498E50;

extern "C" void fn_80257C10(const char* chassis) {
    TuneScales* t = fn_8027A798(chassis);
    int steps = 30;
    f32 lo = 0.25f;
    f32 hi = 1.75f;
    MenuItem items[] = {
        MenuItem("mass", 9, &t->mass, FloatBits(lo), FloatBits(hi)),
        MenuItem("mass", 2, &steps, 0, 0),
        MenuItem("mass_moment", 9, &t->massMoment, FloatBits(lo), FloatBits(hi)),
        MenuItem("mass_moment", 2, &steps, 0, 0),
        MenuItem("power", 9, &t->power, FloatBits(lo), FloatBits(hi)),
        MenuItem("power", 2, &steps, 0, 0),
        MenuItem("gear_ratio", 9, &t->gearRatio, FloatBits(lo), FloatBits(hi)),
        MenuItem("gear_ratio", 2, &steps, 0, 0),
        MenuItem("spring", 9, &t->spring, FloatBits(lo), FloatBits(hi)),
        MenuItem("spring", 2, &steps, 0, 0),
        MenuItem("bump", 9, &t->bump, FloatBits(lo), FloatBits(hi)),
        MenuItem("bump", 2, &steps, 0, 0),
        MenuItem("rebound", 9, &t->rebound, FloatBits(lo), FloatBits(hi)),
        MenuItem("rebound", 2, &steps, 0, 0),
        MenuItem("grip", 9, &t->grip, FloatBits(lo), FloatBits(hi)),
        MenuItem("grip", 2, &steps, 0, 0),
        MenuItem("slip", 9, &t->slip, FloatBits(lo), FloatBits(hi)),
        MenuItem("slip", 2, &steps, 0, 0),
        MenuItem(lbl_8055B860, 6, NULL, -1, 27),
        MenuItem(lbl_8055B860, 6, NULL, -2, 13),
        MenuItem(lbl_80498E50),
    };
    MenuDesc desc = {"$garage.lyt", items, NULL, 0, 0};
    fn_802146BC(&desc, -999, -999, 1);
}

// NonMatching: the item stores are scheduled and register-allocated differently (the target keeps
// every float-bit temp in r14-r31), and "garage.lyt" comes first in the target's string pool.
