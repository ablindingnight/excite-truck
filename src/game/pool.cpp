// Fixed-size object pool (0x8012A13C-0x8012A388): a free list carved from one MemAlloc block (or a
// caller-supplied buffer). Names are guesses.
#include "game/common.h"

extern "C" {
void* fn_801284E8(const char* name, u32 size);  // MemAlloc
void fn_80128658(void* p);                      // MemFree
void fn_802E317C(const char* fmt, ...);         // fatal: print, then exit() -> PPCHalt (never returns)
}

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

ModuleTag lbl_8055DFE8;

Pool::Pool(const char* name, s32 count, s32 size) { Init(name, count, size, 0); }

void Pool::Init(const char* n, s32 c, s32 size, void* buffer) {
    external = buffer != 0;
    panic = 1;
    elemSize = size;
    count = c;
    name = n;
    if (!buffer) {
        mem = (u8*)fn_801284E8(n, c * size);
    } else {
        mem = (u8*)buffer;
    }
    if (mem) {
        free = (void**)mem;
        Reset();
    } else {
        fn_802E317C("Unable to allocate memory for pool");
    }
}

void Pool::Reset(void) {
    u8* end = (u8*)free + (count - 1) * elemSize;
    u8* q;
    for (q = (u8*)free; q < end; q += ElemSize()) {
        *(void**)q = q + ElemSize();
    }
    *(void**)end = 0;
    used = 0;
}

Pool::~Pool(void) {
    if (!external) {
        fn_80128658(mem);
    }
    mem = 0;
}

void* Pool::Alloc(void) {
    void** p = free;
    if (!p) {
        if (panic) {
            fn_802E317C("Panic: out of memory in pool(%s)", name);
        } else {
            return 0;
        }
    }
    if (free) {
        free = (void**)*free;
    }
    used++;
    return p;
}

void Pool::Free(void* p) {
    *(void**)p = free;
    free = (void**)p;
    used--;
}
