#ifndef GAME_ARRAYREF_H
#define GAME_ARRAYREF_H
// A counted view over caller-owned storage (data, count, cap): the dtor clears it. One declaration for every unit
// (port ODR work, 2026-11-16). Units whose helpers differ in body (Find, Remove, RemoveAt, At...) keep them as
// unit-local free inline functions next to their use, each matched to its unit's code. Names unproven.
#include "game/common.h"

template <class T>
struct ArrayRef {
    ArrayRef(void) {}  // ovlayout's FixedList fills the fields itself
    ArrayRef(T* d, int n) {
        data = d;
        cap = n;
        count = 0;
    }
    ~ArrayRef(void) {
        data = 0;
        count = 0;
        cap = 0;
    }
    int Count(void) const { return count; }
    T* Next(void) { return &data[count++]; }
    T* At(int i) { return &data[i]; }

    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
};

#endif
