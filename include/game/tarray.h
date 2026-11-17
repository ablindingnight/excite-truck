#ifndef GAME_TARRAY_H
#define GAME_TARRAY_H
// A list header over caller-owned storage (cenvmap, partemit, tree): every helper any unit uses; a template
// only instantiates what a unit calls. Names unproven.
#include "game/common.h"

template <class T>
struct TArray {
    TArray(T* d, s32 c) {
        data = d;
        cap = c;
        n = 0;
    }
    ~TArray(void) {
        data = 0;
        n = 0;
        cap = 0;
    }
    void Push(const T& v) { data[n++] = v; }
    T& operator[](int i) { return data[i]; }  /* Helper name unproven */
    T* Add(void) { return &data[n++]; }       /* Helper name unproven */
    T* At(int i) { return &data[i]; }

    T* data;  // offset 0x0
    s32 n;    // offset 0x4
    s32 cap;  // offset 0x8
};

#endif
