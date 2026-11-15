#ifndef GAME_VEC3_INIT_H
#define GAME_VEC3_INIT_H

#include <types.h>

// The retail default constructor leaves all three components uninitialized.
struct Vec3Storage {
    Vec3Storage(void);

    void Set(f32 a, f32 b, f32 c) {
        x = a;
        y = b;
        z = c;
    }

    f32 x, y, z;
};

#ifdef __MWERKS__
// Call the same constructor for other views of existing vector storage.
extern "C" void __ct__11Vec3StorageFv(void* storage);
inline void DefaultConstructVec3(void* storage) {
    __ct__11Vec3StorageFv(storage);
}
#else
inline void DefaultConstructVec3(void*) {}
#endif

#endif
