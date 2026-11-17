#ifndef GAME_STRCAT_H
#define GAME_STRCAT_H
// Bounded append: walks to dst's terminator counting n down, then SafeStrCopy's the rest (carmodel, keyboard, locale,
// res, tevbink, uianime carried identical copies as StrCat/StrNCat). One definition for the port (aids audit 3,
// 2026-11-17: each unit converted only where code-identical).
#include "game/strcopy.h"

/* Helper name unproven */
static inline void StrNCat(char* d, const char* s, int n) {
    while (*d) {
        n--;
        d++;
    }
    if (n > 0) {
        SafeStrCopy(d, s, n);
    }
}

#endif
