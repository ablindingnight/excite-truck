#ifndef GAME_FLAGGUARD_H
#define GAME_FLAGGUARD_H
// FlagGuard: one declaration for the units that each carried an identical copy (app, popwarn). Port ODR work,
// 2026-11-16 (samesize.py). Names unproven.
#include "game/common.h"

class FlagGuard {
public:
    FlagGuard(void) {
        saved = lbl_8055D430 & 1;
        lbl_8055D430 |= 1;
    }
    ~FlagGuard(void) { lbl_8055D430 = (lbl_8055D430 & ~1) | saved; }
    u32 saved;
};

#endif
