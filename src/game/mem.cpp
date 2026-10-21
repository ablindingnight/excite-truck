// Memory (0x801281A0-0x801292CC): MemAlloc/MemFree and the global operator new/delete on top of two
// "stackmem" heaps (MEM1/MEM2) with cookie-checked block headers, plus the light stack allocator.
// Names are guesses.
#include "game/common.h"

#define STACK_COOKIE 0x53544B41  // 'STKA'
#define FILL_BYTE 0xE3

extern "C" {
void* memset(void* p, int c, size_t n);
char* strcpy(char* dst, const char* src);
int strcmp(const char* a, const char* b);
int sprintf(char* buf, const char* fmt, ...);
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E3DF8(int arena, void** base, u32* size);
void fn_802E2D74(int unk);
void fn_80127964(void);
}

struct StackBlock {
    u32 cookie;          // offset 0x0
    char name[0x20];     // offset 0x4
    u8 inUse;            // offset 0x24
    u32 size;            // offset 0x28
    u32 request;         // offset 0x2C
    s32 type;            // offset 0x30
    StackBlock* prev;    // offset 0x34
    u8 pad38[0x4];
    u32 cookie2;  // offset 0x3C
    u8 data[];    // offset 0x40
};

namespace {

class Allocator {
public:
    Allocator(void) {}
    virtual void Init(void* base, u32 size) = 0;                   // 0x8
    virtual void Shutdown(void) = 0;                               // 0xC
    virtual void* Alloc(s32 size, const char* name, int type) = 0; // 0x10
    virtual void Free(void* p, int type) = 0;                      // 0x14
    virtual void* Avail(u32 size) = 0;                             // 0x18
    virtual int Contains(void* p) = 0;                             // 0x1C
    virtual u32 GetUsed(void) = 0;                                 // 0x20
    virtual u32 GetFree(void) = 0;                                 // 0x24
    virtual u32 GetSize(void) = 0;                                 // 0x28
    virtual void Verify(void) = 0;                                 // 0x2C
    virtual void vf30(void) = 0;                                   // 0x30
    virtual void vf34(void) = 0;                                   // 0x34
    virtual void Dump(int flags) = 0;                              // 0x38
    virtual void Align(s32 align) = 0;                             // 0x3C
    virtual const char* GetName(void* p) = 0;                      // 0x40
};

class LightStack : public Allocator {
public:
    virtual void Init(void* base, u32 size);
    virtual void Shutdown(void);
    virtual void* Alloc(s32 size, const char* name, int type);
    virtual void Free(void* p, int type);
    virtual void* Avail(u32 size);
    virtual int Contains(void* p) { return (u8*)p >= base && (u8*)p < base + size; }
    virtual u32 GetUsed(void) { return used; }
    virtual u32 GetFree(void) { return size - used; }
    virtual u32 GetSize(void) { return size; }
    virtual void Verify(void) {}
    virtual void vf30(void) {}
    virtual void vf34(void) {}
    virtual void Dump(int flags);
    virtual void Align(s32 align);
    virtual const char* GetName(void* p);

    void Collapse(void);

    u8* base;  // offset 0x4
    s32 used;  // offset 0x8
    s32 size;  // offset 0xC
    s32 peak;  // offset 0x10
};

class StackHeap : public Allocator {
public:
    virtual void Init(void* base, u32 size);
    virtual void Shutdown(void);
    virtual void* Alloc(s32 size, const char* name, int type);
    virtual void Free(void* p, int type);
    virtual void* Avail(u32 size);
    virtual int Contains(void* p) { return (u8*)p >= base && (u8*)p < base + size; }
    virtual u32 GetUsed(void) { return used; }
    virtual u32 GetFree(void) { return size - used; }
    virtual u32 GetSize(void) { return size; }
    virtual void Verify(void);
    virtual void vf30(void) {}
    virtual void vf34(void) {}
    virtual void Dump(int flags);
    virtual void Align(s32 align) {
        if (align > 64) {
            u32 top = (u32)base + used;
            if ((top + 64) & (align - 1)) {
                u8* p = (u8*)Alloc((align - ((top + 128) & (align - 1))) & (align - 1), "internal", 3);
                ((StackBlock*)p - 1)->inUse = 0;
            }
        }
    }
    virtual const char* GetName(void* p);

    StackBlock* FindFree(u32 size);

    s32 used;           // offset 0x4
    s32 size;           // offset 0x8
    u8* base;           // offset 0xC
    StackBlock* top;    // offset 0x10
    s32 peak;           // offset 0x14
    u8* lowest;         // offset 0x18
    u8* highest;        // offset 0x1C
};

}  // namespace

ModuleTag lbl_8055DF78;
StackHeap lbl_8041E140;  // MEM1
StackHeap lbl_8041E160;  // MEM2
u8 lbl_8055DF80;   // enabled
u8 lbl_8055DF81;   // next alloc from MEM2
const char* lbl_8055DF84;  // default new name
u8 lbl_8055DF88;   // warn when disabled
s32 lbl_8055DF8C;  // next alloc alignment
s32 lbl_8055DF90;
void* lbl_8055DF94;
void* lbl_8055DF98;

static inline u32 AlignUp(u32 x, u32 a);

extern "C" {

const char* fn_801281A0(int type) {
    const char* table[4] = {"new[]", "MemAlloc", "MemAllocPhys", "new"};
    if (type >= 0 && type < sizeof(table) / sizeof(table[0])) {
        return table[type];
    }
    return "unknown";
}

u32 fn_801281F4(u32 x, u32 a) { return (x + a - 1) & ~(a - 1); }

void fn_80128208(const char* name) {}

int fn_8012820C(void) {
    void* base0;
    void* base1;
    u32 size0;
    u32 size1;
    fn_80128208("MemBegin");
    lbl_8055DF8C = 0;
    lbl_8055DF90 = 1;
    base1 = 0;
    size1 = 0;
    fn_802E3DF8(0, &base0, &size0);
    fn_802E3DF8(1, &base1, &size1);
    lbl_8041E140.Init(base0, size0);
    lbl_8041E160.Init(base1, size1);
    lbl_8055DF94 = base0;
    return 1;
}

void fn_80128710(void* p);

void fn_801282A0(void) {
    lbl_8041E140.Shutdown();
    lbl_8041E160.Shutdown();
    fn_80128710(lbl_8055DF94);
    lbl_8055DF94 = 0;
    lbl_8055DF90 = 0;
}

// Stripped from ET; only its string remains.
void MemVerifyAll(void) { fn_80128208("MemVerify"); }

void fn_801282E8(void) {}

void fn_801282EC(s32 align) { lbl_8055DF8C = align; }

void fn_801282F4(void) { lbl_8055DF81 = lbl_8041E160.size > 0; }

s32 fn_80128314(int arena) {
    if (arena == 0) {
        return lbl_8041E140.used;
    }
    if (arena == 1) {
        return lbl_8041E160.used;
    }
    return 0;
}

void* fn_8012834C(const char* name, s32 size, int type) {
    void* p;
    fn_80127964();
    if (!lbl_8055DF80 && lbl_8055DF88) {
        fn_802E317C("MemAlloc(%s): Subsystem is disabled.", name);
    }
    if (lbl_8055DF8C) {
        if (!lbl_8055DF81) {
            lbl_8041E140.Align(lbl_8055DF8C);
        } else {
            lbl_8041E160.Align(lbl_8055DF8C);
        }
        lbl_8055DF8C = 0;
    }
    if (lbl_8055DF81) {
        p = lbl_8041E160.Alloc(size, name, type);
    } else {
        p = lbl_8041E140.Alloc(size, name, type);
    }
    lbl_8055DF81 = 0;
    return p;
}

void* fn_801284E8(const char* name, u32 size) { return fn_8012834C(name, size, 1); }

void* fn_801284F0(const char* name, u32 size) { return fn_8012834C(name, size, 2); }

}  // extern "C"

void* operator new(size_t size, const char* name) { return fn_8012834C(name, size, 3); }

void* operator new[](size_t size, const char* name) { return fn_8012834C(name, size, 0); }

void* operator new(size_t size) {
    if (lbl_8055DF80) {
        return fn_8012834C(lbl_8055DF84 ? lbl_8055DF84 : "<unknown>", size, 3);
    }
    fn_802E317C("new called with no name! (size == %d", size);
    return lbl_8055DF98;
}

extern "C" {

void fn_80128584(void* p, int type) {
    if (lbl_8055DF80 && !p) {
        return;
    }
    fn_80127964();
    if (!lbl_8055DF80 && lbl_8055DF88) {
        fn_802E317C("MemFree: Subsystem is disabled.");
    }
    if (lbl_8041E140.Contains(p)) {
        lbl_8041E140.Free(p, type);
    } else {
        lbl_8041E160.Free(p, type);
    }
}

void fn_80128658(void* p) { fn_80128584(p, 1); }

void fn_80128660(void* p) { fn_80128584(p, 2); }

}  // extern "C"

void operator delete(void* p) { fn_80128584(p, 3); }

void operator delete[](void* p) { fn_80128584(p, 0); }

extern "C" {

// Stripped from ET; only their strings remain.
void* malloc(size_t size) {
    fn_802E317C("Do not use malloc()");
    return 0;
}

void free(void* p) { fn_802E317C("Do not use free()"); }

void fn_80128678(void) { lbl_8055DF88 = 1; }

void fn_80128684(void) { lbl_8055DF88 = 0; }

int fn_80128690(void) { return lbl_8055DF88 == 0; }

void fn_801286A0(void* p, u32 amount) {
    StackBlock* b = (StackBlock*)p - 1;
    amount &= ~63;
    b->size -= amount;
    b->request -= amount;
    lbl_8041E140.used -= amount;
    *(u32*)((u8*)p + fn_801281F4(b->request, 4)) = STACK_COOKIE;
}

void fn_80128710(void* p) {}

}  // extern "C"

static inline u32 AlignUp(u32 x, u32 a) { return fn_801281F4(x, a); }

void StackHeap::Init(void* b, u32 s) {
    size = s;
    used = 0;
    top = 0;
    base = (u8*)b;
    if (b) {
        fn_802E30C8("stackmem: base == 0x%x, size = %d", b, s);
        fn_802E30C8("stackmem: range == (0x%x --> 0x%x)", base, base + size);
        highest = 0;
        peak = 0;
        lowest = (u8*)0x02000000;
    } else if (s == 0) {
        highest = 0;
        peak = 0;
    } else {
        fn_802E317C("Can't alloc %dK", (s32)s / 1024);
    }
}

void StackHeap::Shutdown(void) {
    fn_802E30C8("stackmem: Max alloc: %dK  Highest address: 0x%x (%dK)", peak / 1024, highest,
                (u32)highest >> 10);
    fn_802E30C8("stackmem: Lowest address: 0x%x (%dK)  Range: %dK", lowest, (u32)lowest >> 10,
                (u32)(highest - lowest) >> 10);
    fn_802E30C8("stackmem: Unused: %dK", (size - peak) / 1024);
    if (used) {
        Dump(1);
        fn_802E317C("stackmem: Memory not freed: stack_top != 0 (%d)", used);
    }
    base = 0;
}

StackBlock* StackHeap::FindFree(u32 size) { return 0; }

void* StackHeap::Alloc(s32 size, const char* name, int type) {
    StackBlock* b;
    s32 orig = size;
    size = AlignUp(size, 64);
    b = FindFree(size);
    if (b) {
        sprintf(b->name, "?%s (%d)", name, size);
        b->inUse = 1;
        return b->data;
    }
    if (size + used + 64 >= this->size) {
        Dump(1);
        fn_802E317C("MemAlloc(%s, %d): exceeded stack", name, size);
    }
    b = (StackBlock*)(base + used);
    memset(b, 0, sizeof(StackBlock));
    strcpy(b->name, name);
    b->inUse = 1;
    b->type = type;
    b->size = size;
    b->request = orig;
    b->cookie = STACK_COOKIE;
    b->prev = top;
    b->cookie2 = STACK_COOKIE;
    top = b;
    memset(b->data, FILL_BYTE, size);
    {
        u8* data = b->data;
        u8* q;
        for (q = data + b->request; q < data + AlignUp(b->request, 4); q++) {
            *q = FILL_BYTE;
        }
    }
    used = size + used + 64;
    if (used < this->size) {
        *(u32*)&b->data[AlignUp(b->request, 4)] = STACK_COOKIE;
    }
    peak = peak < used ? used : peak;
    {
        u8* end = (u8*)b + size + 64;
        u8* data = b->data;
        if (end >= highest) {
            highest = end;
        }
        if (data < lowest) {
            lowest = data;
        }
    }
    return b->data;
}

void StackHeap::Free(void* p, int type) {
    StackBlock* b = (StackBlock*)p - 1;
    u8* q;
    if (b->cookie != STACK_COOKIE) {
        fn_802E317C("MemFree: bad cookie at %x", b);
    }
    if (*(u32*)((u8*)p + AlignUp(b->request, 4)) != STACK_COOKIE) {
        fn_802E317C("MemFree: overrun: footer cookie corruption in %s", b->name);
    }
    for (q = (u8*)p + b->request; q < (u8*)p + AlignUp(b->request, 4); q++) {
        if (*q != FILL_BYTE) {
            fn_802E317C("MemFree: overrun: footer cookie corruption (%x:@EOB+%d) in %s (%2.2x != %2.2x)", q,
                        q - ((u8*)p + b->request), b->name, *q, FILL_BYTE);
        }
    }
    if (b->cookie2 != STACK_COOKIE) {
        fn_802E317C("MemFree: underrun: header cookie corruption in %s (0x%x)", b->name, b->cookie2);
    }
    if (!b->inUse) {
        fn_802E317C("MemFree: %s not in use", b->name);
    }
    if (b->type != type) {
        fn_802E317C("MemFree: %s was allocated %s, freed %s", b->name, fn_801281A0(b->type), fn_801281A0(type));
    }
    b->inUse = 0;
    while (top && !top->inUse) {
        StackBlock* t = top;
        top = t->prev;
        t->prev = 0;
    }
    if (top) {
        used = (u8*)top - base + top->size + 64;
    } else {
        used = 0;
    }
}

void StackHeap::Dump(int flags) {
    if (flags & 1) {
        s32 off;
        s32 wasted;
        s32 count;
        fn_802E30C8("==================== Alloc Dump =======================");
        off = 0;
        wasted = 0;
        count = 0;
        while (off < used) {
            StackBlock* b = (StackBlock*)(base + off);
            int internal;
            if (b->cookie != STACK_COOKIE) {
                fn_802E317C("MemDump fails %x", b);
            }
            internal = strcmp(b->name, "internal") == 0;
            fn_802E30C8("%s%-16.16s  %10d bytes  %10x", b->inUse && !internal ? "" : "!", b->name, b->size, b);
            if (!b->inUse) {
                wasted += b->size + 64;
            }
            count++;
            off += b->size + 64;
            if (!(off & 31)) {
                fn_802E2D74(1);
            }
        }
        fn_802E30C8("Stack Used: %dK/%dK   Free: %dK   Wasted: %dK", used / 1024, size / 1024,
                    (size - used) / 1024, wasted / 1024);
        fn_802E30C8(" %d allocations", count);
        fn_802E30C8("=======================================================");
    }
}

void StackHeap::Verify(void) {
    s32 off;
    for (off = 0; off < used;) {
        StackBlock* b = (StackBlock*)(base + off);
        if (b->cookie != STACK_COOKIE) {
            fn_802E317C("MemVerifyAll fails %x", b);
        }
        off += b->size + 64;
    }
}

void* StackHeap::Avail(u32 s) {
    s = AlignUp(s, 64);
    if (used < size - (s32)s) {
        return base + size - s;
    }
    return 0;
}

const char* StackHeap::GetName(void* p) {
    StackBlock* b = (StackBlock*)p - 1;
    if (b->cookie != STACK_COOKIE) {
        return "<unknown>";
    }
    return b->name;
}

void LightStack::Init(void* b, u32 s) {
    base = (u8*)b;
    used = 0;
    size = s;
    peak = 0;
}

void LightStack::Align(s32 align) {
    u8* b;
    s32 old = used;
    u32 pad = align - (old & (align - 1));
    if (pad < 0x30) {
        pad += align;
    }
    b = base;
    Alloc(pad - 0x30, "internal", 3);
    *(u32*)(b + old) = 0x06630216;
}

void LightStack::Shutdown(void) { fn_802E30C8("LightStackAllocator: Max alloc %dK/%dK", peak / 1024, (s32)size / 1024); }

void* LightStack::Alloc(s32 s, const char* name, int type) {
    s32 aligned = AlignUp(s, 16);
    u32 total = aligned + 0x20;
    u8* data;
    if (total + used > size) {
        fn_802E317C("LightStackAlloc: Out of room for %s:%dK", name, aligned / 1024);
    }
    {
        u8* blk = base + used;
        *(u32*)blk = 0xAAAA5555;
        ((s32*)blk)[1] = type;
        data = blk + 0x10;
    }
    {
        u8* end = base + used;
        u32* foot = (u32*)(end + aligned);
        foot[4] = 0xFEEDFACE;
        foot[5] = aligned;
    }
    used += total;
    memset(data, FILL_BYTE, aligned);
    peak = used > peak ? used : peak;
    return data;
}

void LightStack::Collapse(void) {
    while (used > 0) {
        u8* blk = base + used;
        blk = blk - 0x20 - *(u32*)(blk - 0xC);
        if (*(u32*)blk == 0xAAAA5555) {
            return;
        }
        used = blk - base;
    }
}

void LightStack::Free(void* p, int type) {
    *(u32*)((u8*)p - 0x10) = 0x06630216;
    Collapse();
}

void* LightStack::Avail(u32 s) {
    s32 a = AlignUp(s, 64);
    if (used < size - a) {
        return base + size - a;
    }
    return 0;
}

void LightStack::Dump(int flags) { fn_802E30C8("Lite Allocator, using %dK/%dK", used / 1024, (s32)size / 1024); }

const char* LightStack::GetName(void* p) { return "<unknown>"; }
