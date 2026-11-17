#ifndef GAME_VECREF_H
#define GAME_VECREF_H
// A list header over inline storage; its destructor clears the header (multievt, popwarn). Names unproven.
#include "game/common.h"

template <class T>
struct VecRef {
    ~VecRef(void) {
        data = 0;
        count = 0;
        cap = 0;
    }
    int Size() const { return count; }
    T* Next() { return &data[count++]; }
    void Push(T e) {
        T* d = data;
        int n = count;
        count = n + 1;
        d[n] = e;
    }
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
};

#endif
