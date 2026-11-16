#ifndef GAME_POOL_H
#define GAME_POOL_H
// A fixed-size block allocator (pool.cpp; the ctor, Alloc and Free are pool.cpp's inline definitions, emitted weak
// there). One declaration for every unit (port ODR work, 2026-11-16).
#include "game/common.h"

class Pool {
public:
    Pool(const char* name, s32 count, s32 size);
    virtual ~Pool(void);

    void Init(const char* name, s32 count, s32 size, void* buffer);
    void Reset(void);
    void* Alloc(void);
    void Free(void* p);
    s32 ElemSize() const { return elemSize; }

    u8 panic;          // offset 0x4
    u8 external;       // offset 0x5
    const char* name;  // offset 0x8
    u8* mem;           // offset 0xC
    void** free;       // offset 0x10
    s32 elemSize;      // offset 0x14
    s32 count;         // offset 0x18
    s32 used;          // offset 0x1C
};

#endif
