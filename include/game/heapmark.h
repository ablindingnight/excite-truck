#ifndef GAME_HEAPMARK_H
#define GAME_HEAPMARK_H
// HeapMark: one declaration for the units that each carried an identical copy (gamedo, garage). Port ODR work,
// 2026-11-16 (samesize.py). Names unproven.
#include "game/common.h"

// Self-contained for the PC build (port ODR check, 2026-11-17).
extern "C" int fn_80128314(int heap);  // heap use
extern "C" int fn_80265D58(void);

// Heap use at the start of an event, for leak checks.
class HeapMark {
public:
    HeapMark(const char* name) {
        this->name = name;
        heap0 = fn_80128314(0);
        heap1 = fn_80128314(1);
        other = fn_80265D58();
    }
    ~HeapMark(void) {}

    const char* name;
    int heap0;
    int heap1;
    int other;
};

#endif
