#ifndef GAME_ARRAYBASE_H
#define GAME_ARRAYBASE_H

#include "game/common.h"

// A data/count/capacity array view (evpath, frname, gtext); members are instantiated only where used. Names
// unproven.
template <typename T>
class ArrayBase {
public:
    ~ArrayBase(void) {
        data = 0;
        count = 0;
        cap = 0;
    }
    int Count(void) const { return count; }
    void Add(T v) {
        int n = count++;
        data[n] = v;
    }
    T& AppendSlot(void) {
        return data[count++];
    }
    template <typename V>
    int Find(V* key) {
        V* v = key;
        for (int i = 0; i < Count(); i++) {
            if ((V*)data[i] == v) {
                return i;
            }
        }
        return -1;
    }
    // MWCC: the key is a const REFERENCE. A destructor that passes `this` here gets a real object for `this`
    // when it is inlined, so its `this != 0` tests survive for stack objects (evpath, FACTS 2026-11-17 G5).
    template <typename V>
    void Remove(V* const& v) {
        int i = Find(v);
        if (i == -1) {
            return;
        }
        if (i >= 0 && i < count) {
            int last = count - 1;
            if (i != last) {
                data[i] = data[last];
            }
            count--;
        }
    }
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
};

#endif
