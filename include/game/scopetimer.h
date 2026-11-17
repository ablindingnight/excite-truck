#ifndef GAME_SCOPETIMER_H
#define GAME_SCOPETIMER_H
// ScopeTimer: one declaration for the units that each carried an identical copy (groundvis, xlobj). Port ODR work,
// 2026-11-16 (samesize.py). Names unproven.
#include "game/common.h"

// Times a block and reports it when it runs long.
struct ScopeTimer {
    ScopeTimer(const char* n) {
        name = n;
        start = fn_802E2C20();
    }
    ~ScopeTimer(void) {
        int t = fn_802E2C20() - start;
        if (t > 500) {
            fn_802E30C8("%s: took %dms", name, t);
        }
    }
    const char* name;  // offset 0x0
    int start;         // offset 0x4
};

#endif
