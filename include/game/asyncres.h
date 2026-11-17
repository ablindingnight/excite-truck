#ifndef GAME_ASYNCRES_H
#define GAME_ASYNCRES_H
// A resource streamed from a resource set (res.cpp; ResourceGetStream opens one): loadpool and texture read
// through it. AsyncResource is the game's own name ("AsyncResource::pool",
// "AsyncResource: Bad crc on %s"); field names are guesses.
#include "game/common.h"

struct ResEntry;
class ResourceSet;

class AsyncResource {
public:
    AsyncResource(void) { Clear(); }
    ~AsyncResource(void) { Clear(); }
    static void* operator new(size_t size, const char* name);
    static void operator delete(void* p);

    void Clear(void) {
        handle = -1;
        buf = 0;
        entry = 0;
        set = 0;
        size = 0;
        magic = 0x19AFEE2A;
    }
    s32 Span(void);
    void Load(u8* dst, s32 n, s32 offs);
    s32 Start(void) const;  // file offset of the resource (inline, res.cpp)
    u8* Poll(s32* sizeOut);
    void Cancel(void);

    u32 magic;          // offset 0x0
    ResEntry* entry;    // offset 0x4
    ResourceSet* set;   // offset 0x8
    s32 handle;         // offset 0xC
    u8* buf;            // offset 0x10
    s32 size;           // offset 0x14
};

#endif
