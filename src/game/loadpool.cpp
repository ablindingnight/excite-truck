// Load pool (0x8013420C-0x801345D8): fixed-size buffer slots filled by asynchronous resource streams
// (at most three in flight), with a completion callback per slot. Names are guesses.
#include "game/common.h"

class AsyncResource;

extern "C" {
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
AsyncResource* fn_8012C5B8(const char* name, u32 type, u32* info);  // ResourceGetStream
void fn_8012C958(AsyncResource* r);                                 // ResourceStreamClose
}

class AsyncResource {
public:
    s32 Span(void);
    void Load(u8* dst, s32 n, s32 offs);
    u8* Poll(s32* sizeOut);
    void Cancel(void);
};

void* operator new[](size_t size, const char* tag);

typedef void (*LoadCallback)(u8* data, s32 size, void* arg);

struct LoadSlot {
    s32 state;           // offset 0x0 (0 free, 1 loading, 2 done)
    u8* buf;             // offset 0x4
    AsyncResource* res;  // offset 0x8
    LoadCallback cb;     // offset 0xC
    void* arg;           // offset 0x10
    s32 size;            // offset 0x14
};

class LoadPool {
public:
    ~LoadPool(void);

    u8* buf;          // offset 0x0
    s32 count;        // offset 0x4
    s32 slotSize;     // offset 0x8
    LoadSlot* slots;  // offset 0xC
};

ModuleTag lbl_8055E088;

extern "C" {

LoadPool* fn_8013420C(LoadPool* pool, s32 count, s32 slotSize) {
    int i;
    pool->count = count;
    pool->slotSize = slotSize;
    pool->buf = new ("loadbuf") u8[count * slotSize];
    pool->slots = new ("loadlost") LoadSlot[pool->count];
    for (i = 0; i < pool->count; i++) {
        LoadSlot* s = &pool->slots[i];
        s->buf = pool->buf + i * pool->slotSize;
        s->state = 0;
        s->res = 0;
    }
    return pool;
}

}  // extern "C"

LoadPool::~LoadPool(void) {
    int i;
    for (i = 0; i < count; i++) {
        LoadSlot* s = &slots[i];
        if (s->state != 0 && s->state == 1) {
            fn_802E30C8("loadpool: canceling stream...");
            s->res->Cancel();
            fn_8012C958(s->res);
        }
    }
    delete[] slots;
    delete[] buf;
}

extern "C" {

void fn_8013436C(LoadPool* pool) {
    int i;
    for (i = 0; i < pool->count; i++) {
        LoadSlot* s = &pool->slots[i];
        if (s->state == 1) {
            s32 size = 0;
            u8* data = s->res->Poll(&size);
            if (data) {
                s->state = 2;
                fn_8012C958(s->res);
                s->res = 0;
                if (s->cb) {
                    s->cb(data, size, s->arg);
                }
            }
        }
    }
}

int fn_80134424(LoadPool* pool, const char* name, u32 type, u32* info, LoadCallback cb, void* arg, s32 offset,
                s32 size) {
    int i;
    int active = 0;
    for (i = 0; i < pool->count; i++) {
        if (pool->slots[i].state == 1) {
            active++;
        }
    }
    if (active >= 3) {
        return -1;
    }
    for (i = 0; i < pool->count; i++) {
        LoadSlot* s = &pool->slots[i];
        if (s->state == 0) {
            s32 total;
            s->state = 1;
            s->res = fn_8012C5B8(name, type, info);
            if (!s->res) {
                fn_802E317C("loadpool: can't load %s", name);
            }
            s->cb = cb;
            s->arg = arg;
            total = s->res->Span();
            if (size <= 0) {
                size = total;
            }
            if (size > pool->slotSize) {
                fn_802E317C("loadpool: resource %s too big (%d > %d)", name, size, pool->slotSize);
            }
            if (offset + size > total) {
                fn_802E317C("loadpool: requesting more than %d bytes at %d+%d", total, offset, size);
            }
            s->size = size;
            s->res->Load(s->buf, size, offset);
            return i;
        }
    }
    return -1;
}

int fn_801345A4(LoadPool* pool, int i) {
    if (pool->slots[i].state == 1) {
        return 0;
    }
    pool->slots[i].state = 0;
    return 1;
}

}  // extern "C"
