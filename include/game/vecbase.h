#ifndef GAME_VECBASE_H
#define GAME_VECBASE_H
// A list header over caller-owned storage (data, count, capacity); the destructor clears it. Every member
// any unit uses (gmat, gndtex, tedash, xlobj); a template only instantiates what a unit calls. Names unproven.
#include "game/common.h"

template <class T>
struct VecBase {
    VecBase(void) {
        data = 0;
        cap = 0;
        count = 0;
    }
    VecBase(T* d, int n) {
        data = d;
        cap = n;
        count = 0;
    }
    ~VecBase(void) {
        data = 0;
        count = 0;
        cap = 0;
    }
    T& Next(void) { return data[count++]; }
    int Size(void) const { return count; }
    int CanAdd(void) const { return count + 1 <= cap; }
    bool Fits(int n) const { return count + n <= cap; }
    int Find(const T& v) const {
        for (int j = 0; j < count; j++) {
            if (data[j] == v) {
                return j;
            }
        }
        return -1;
    }
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
};

#endif
