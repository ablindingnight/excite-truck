#ifndef GAME_FLAGGUARD_H
#define GAME_FLAGGUARD_H
// FlagGuard: one declaration for the units that each carried an identical copy (app, popwarn). Port ODR work,
// 2026-11-16 (samesize.py). Names unproven.
#include "game/common.h"

// Self-contained for the PC build (port ODR check, 2026-11-17).
extern u32 lbl_8055D430;  // debug flags (the units that include this declare it the same way)

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
