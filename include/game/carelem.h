#ifndef GAME_CARELEM_H
#define GAME_CARELEM_H
// A car's engine curve (carelem.cpp; lbl_8055E908 holds one per car): carelem fills it from the tuned setup,
// cargear reads the torque. Names unproven.
#include "game/common.h"

// ApplyCurve is inline, defined in carelem.cpp.
struct CarElem {
public:
    void ApplyCurve(f32 x, f32 k);
    CarElem(void) {
        f4 = f8 = fC = f10 = f14 = f18 = 0.0f;
        f1C = 0.0f;
        f20 = 0.0f;
    }
    void Update(int car);
    void Fetch(int car);
    void Clear(int car);
    void Compute(int car);
    f32 Torque(f32 x) {
        f32 t;
        if (x > f2C) {
            f32 sq;
            f32 d = x - f20;
            if (0.0f > d) {
                d = 0.0f;
            }
            sq = x * x;
            t = fC + (f8 * x + f4 * sq) - f24 * d;
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

#endif
