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

// A slider item (type 9): the range travels as the two floats' bit patterns in the int argument words.
// PROVISIONAL (user 2026-11-16). Aids audit 2026-11-16: retail stores every row's lo/hi to its own stack slot and
// reads them back with lwz (one float object per row = by-value parameters of an inline). Every non-cast pun
// compiles to the same instructions with other registers: union locals 91.11, a union parameter with a float
// constructor (by value or const&) 91.36, a MenuItem(n, v, f32 lo, f32 hi) constructor doing the cast 91.36;
// only the pointer cast on the inline's own parameters is exact. JP build: identical code. opsound's volume rows
// show the same stack-slot pattern (and rise with the same helper).
/* Helper name unproven */
static inline MenuItem Slider(const char* n, f32* v, f32 lo, f32 hi) { return MenuItem(n, 9, v, *(int*)&lo, *(int*)&hi); }

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
        Slider("mass", &t->mass, lo, hi),
        MenuItem("mass", 2, &steps, 0, 0),
        Slider("mass_moment", &t->massMoment, lo, hi),
        MenuItem("mass_moment", 2, &steps, 0, 0),
        Slider("power", &t->power, lo, hi),
        MenuItem("power", 2, &steps, 0, 0),
        Slider("gear_ratio", &t->gearRatio, lo, hi),
        MenuItem("gear_ratio", 2, &steps, 0, 0),
        Slider("spring", &t->spring, lo, hi),
        MenuItem("spring", 2, &steps, 0, 0),
        Slider("bump", &t->bump, lo, hi),
        MenuItem("bump", 2, &steps, 0, 0),
        Slider("rebound", &t->rebound, lo, hi),
        MenuItem("rebound", 2, &steps, 0, 0),
        Slider("grip", &t->grip, lo, hi),
        MenuItem("grip", 2, &steps, 0, 0),
        Slider("slip", &t->slip, lo, hi),
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
