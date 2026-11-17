#ifndef GAME_TFIXED_H
#define GAME_TFIXED_H
// A TArray over its own inline storage (cenvmap, partemit; tree on PC). Names unproven.
#include "game/tarray.h"

template <class T, int N>
struct TFixed : TArray<T> {
    TFixed(void) : TArray<T>(items, N) {}
    T items[N];  // offset 0xC
};

#endif
