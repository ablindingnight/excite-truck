#ifndef GAME_FIXEDSTR_H
#define GAME_FIXEDSTR_H
// FixedStr: one declaration for the units that each carried an identical copy (awards, betarace). Port ODR work,
// 2026-11-16 (samesize.py). Names unproven.
#include "game/common.h"

struct FixedStr {
    FixedStr(void) {
        memset(s, 'z', 3);
        s[3] = 0;
    }
    void Set(const char* t) { SafeStrCopy(s, t, N); }
    char s[N];
};

#endif
