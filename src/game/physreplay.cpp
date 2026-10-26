// Replay recording (0x801B45E0-0x801B53DC): a ring of frames (time + data handle) over a packet heap,
// the recorded header packet (unpacked from 16-bit fixed point and blended between two frames), playback
// of every physics object's packets and of the event packets (up to 16 installed handlers, by type).
// Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
void* memset(void* p, int c, u32 n);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
int fn_801B2F24(void);                   // racer count
void fn_801769E0(void);
}

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);
void operator delete(void* p);
void operator delete[](void* p);

class PacketHeap {
public:
    virtual ~PacketHeap(void);
    virtual u8* Lock(s32 handle, int flags);
    virtual void vf10(void);
    virtual void Unlock(s32 handle);
    virtual void Reset(void);
};

extern "C" PacketHeap* fn_802E4CA4(s32 size);

class ReplayObj {
public:
    virtual void vf08(void) = 0;
    virtual void vf0C(void) = 0;
    virtual void vf10(void) = 0;
    virtual void vf14(void) = 0;
    virtual void vf18(void) = 0;
    virtual void vf1C(void) = 0;
    virtual void vf20(void) = 0;
    virtual void vf24(void) = 0;
    virtual void vf28(void) = 0;
    virtual void vf2C(void) = 0;
    virtual void Play(u8* a, u8* b, f32 t, s32* sizeA, s32* sizeB) = 0;
    virtual void vf34(void) = 0;
    virtual void vf38(void) = 0;
    virtual void vf3C(void) = 0;
    virtual int Replays(void) = 0;
};

// One recorded frame (0xC).
struct Frame {
    s32 time;    // offset 0x0
    s32 unk4;    // offset 0x4
    s32 handle;  // offset 0x8
};

struct FramePool {
    PacketHeap* heap;  // offset 0x0
    s32 size;          // offset 0x4
    s32 unk8;          // offset 0x8
    s32 unkC;          // offset 0xC
    s32 unk10;         // offset 0x10
    s32 unk14;         // offset 0x14
    Frame* frames;     // offset 0x18
    u32 count;         // offset 0x1C
    u32 head;          // offset 0x20
    u32 tail;          // offset 0x24
};

struct Replay {
    u8 enabled;      // offset 0x0
    FramePool pool;  // offset 0x4
};

// The header packet of a frame (0xC) and its unpacked form.
struct HeaderPacket {
    s32 unk0;  // offset 0x0
    u8 unk4;   // offset 0x4
    u16 x;     // offset 0x6
    u16 y;     // offset 0x8
    u16 z;     // offset 0xA
};

struct Header {
    s32 unk0;  // offset 0x0
    s32 unk4;  // offset 0x4
    f32 x;     // offset 0x8
    f32 y;     // offset 0xC
    f32 z;     // offset 0x10
};

extern "C" Header* fn_80178C90(void);

typedef s32 (*EventHandler)(u8* data, s32 arg);

struct ReplayEvents {
    u8 unk0[0x200];             // offset 0x0
    s32 unk200;                 // offset 0x200
    s32 unk204;                 // offset 0x204
    EventHandler handlers[16];  // offset 0x208
};

ReplayEvents lbl_80435C70;
u8 lbl_8055E6D0;
s32 lbl_8055E6D4;
Replay* lbl_8055E6D8;
u8* lbl_8055E6DC;
ModuleTag lbl_8055E6C8;

extern s32 lbl_8055E3C4;  // game time

#define RP_MIN(a, b) (((a) < (b)) ? (a) : (b))

static inline f32 Norm(f32 v, f32 lo, f32 hi) { return (v - lo) / (hi - lo); }
static inline f32 Unpack(u16 v, f32 lo, f32 hi) {
    f32 f = RP_MIN(1.0f, Norm(v, 0.0f, 65535.0f));
    f = (f > 0.0f) ? f : 0.0f;
    return lo + f * (hi - lo);
}

static inline int MinI(int a, int b) { return (a < b) ? a : b; }

static inline f32 Lerp(f32 a, f32 b, f32 t) {
    b = t * (b - a);
    return a + b;
}

extern "C" {

FramePool* fn_801B45E0(FramePool* p) {
    p->heap = 0;
    p->size = p->unk8 = p->unkC = p->unk10 = p->unk14 = 0;
    p->frames = 0;
    p->count = p->head = p->tail = 0;
    return p;
}

FramePool* fn_801B4610(FramePool* p, int flags) {
    if (p) {
        delete p->heap;
        p->heap = 0;
        p->size = p->unk8 = p->unkC = p->unk10 = p->unk14 = 0;
        delete[] p->frames;
        p->frames = 0;
        p->count = p->head = p->tail = 0;
        if (flags > 0) {
            operator delete(p);
        }
    }
    return p;
}

void fn_801B4720(FramePool* p);

void fn_801B46BC(FramePool* p, u32 n, s32 size) {
    p->count = n;
    p->frames = new ("FramePool::mFrames") Frame[n];
    p->size = size;
    p->heap = fn_802E4CA4(size);
    fn_801B4720(p);
}

void fn_801B4720(FramePool* p) {
    for (u32 i = 0; i < p->count; i++) {
        p->frames[i].time = -1;
        p->frames[i].unk4 = 0;
        p->frames[i].handle = 0;
    }
    p->unk8 = 0;
    p->unkC = p->size;
    p->unk10 = p->size;
    p->unk14 = 0;
    p->head = 0;
    p->tail = 0;
    p->heap->Reset();
}

Frame* fn_801B479C(FramePool* p, u32 i) { return &p->frames[i]; }

u32 fn_801B47AC(FramePool* p) { return p->head; }

u32 fn_801B47D4(FramePool* p, u32 i);

u32 fn_801B47B4(FramePool* p) { return fn_801B47D4(p, p->tail); }

u32 fn_801B47BC(FramePool* p, u32 i) {
    u32 j = i + 1;
    return (j < p->count) ? j : 0;
}

u32 fn_801B47D4(FramePool* p, u32 i) {
    if (i != 0) {
        return i - 1;
    }
    return p->count - 1;
}

u32 fn_801B47F0(FramePool* p) {
    if (p->head < p->tail) {
        return p->tail - p->head;
    }
    return p->tail + (p->count - p->head);
}

s32 fn_801B4818(Replay* r) { return 0x100000; }

void fn_801B4820(Replay* r, s32* dataSize, s32* frames, u32 avail) {
    int n = avail / ((fn_801B2F24() + 1) * 0x44 + 0xC);
    n = (n < 0x7E90) ? n : 0x7E90;
    *dataSize = 0x5000;
    int secs = n / 9;
    *frames = 50;
    fn_802E30C8("replay: Allocing %d:%2.2ds (%dK) frames and %dK data (%dK avail)", secs / 60, secs % 60,
                *frames * sizeof(Frame) / 1024, *dataSize / 1024, avail / 1024);
}

Replay* fn_801B4904(Replay* r) {
    s32 dataSize;
    s32 frames;
    fn_801B45E0(&r->pool);
    fn_801B4820(r, &dataSize, &frames, fn_801B4818(r));
    fn_801B46BC(&r->pool, frames, dataSize);
    r->enabled = 0;
    return r;
}

Replay* fn_801B496C(Replay* r, int flags) {
    if (r) {
        fn_801B4610(&r->pool, -1);
        if (flags > 0) {
            operator delete(r);
        }
    }
    return r;
}

void fn_801B49C8(Replay* r) { r->enabled = 1; }

int fn_801B4B44(Replay* r);
int fn_801B4C1C(Replay* r, s32 time);
Frame* fn_801B4C58(Replay* r, u32 i);
Frame* fn_801B4C60(Replay* r, u32 i);
u32 fn_801B4C9C(Replay* r, s32 time);
f32 fn_801B4D10(Replay* r, s32 time);

int fn_801B49D4(Replay* r, u8** a, u8** b, f32* t, s32 time) {
    if (!fn_801B4B44(r)) {
        return 0;
    }
    u32 i = fn_801B4C9C(r, time);
    s32 h = fn_801B4C58(r, i)->handle;
    *a = r->pool.heap->Lock(h, 0x2200);
    if (fn_801B4C1C(r, time)) {
        *b = *a;
    } else {
        h = fn_801B4C60(r, i)->handle;
        *b = r->pool.heap->Lock(h, 0x2200);
    }
    *t = fn_801B4D10(r, time);
    return 1;
}

void fn_801B4AC4(Replay* r, s32 time) {
    u32 i = fn_801B4C9C(r, time);
    s32 h = fn_801B4C58(r, i)->handle;
    r->pool.heap->Unlock(h);
    h = fn_801B4C60(r, i)->handle;
    r->pool.heap->Unlock(h);
}

int fn_801B4B44(Replay* r) {
    int ok = 0;
    if (fn_801B47F0(&r->pool) && r->enabled) {
        ok = 1;
    }
    return ok;
}

s32 fn_801B4B9C(Replay* r) { return fn_801B479C(&r->pool, fn_801B47AC(&r->pool))->time; }

s32 fn_801B4BDC(Replay* r) { return fn_801B479C(&r->pool, fn_801B47B4(&r->pool))->time; }

int fn_801B4C1C(Replay* r, s32 time) { return time >= fn_801B4BDC(r); }

Frame* fn_801B4C58(Replay* r, u32 i) { return fn_801B479C(&r->pool, i); }

Frame* fn_801B4C60(Replay* r, u32 i) { return fn_801B479C(&r->pool, fn_801B47BC(&r->pool, i)); }

u32 fn_801B4C9C(Replay* r, s32 time) {
    int d = time - fn_801B4B9C(r);
    u32 i = fn_801B47AC(&r->pool);
    int n = d / 6;
    while (n--) {
        i = fn_801B47BC(&r->pool, i);
    }
    return i;
}

f32 fn_801B4D10(Replay* r, s32 time) { return (f32)((time - fn_801B4B9C(r)) % 6) / 6.0f; }

void fn_801B4D80(Header* h, const HeaderPacket* a, const HeaderPacket* b, f32 t) {
    s32 u0 = a->unk0;
    s32 u4 = a->unk4;
    h->unk0 = u0;
    h->unk4 = u4;
    h->x = Lerp(Unpack(a->x, -300.0f, 300.0f), Unpack(b->x, -300.0f, 300.0f), t);
    h->y = Lerp(Unpack(a->y, -300.0f, 300.0f), Unpack(b->y, -300.0f, 300.0f), t);
    h->z = Lerp(Unpack(a->z, -4.0f, 4.0f), Unpack(b->z, -4.0f, 4.0f), t);
}

void fn_801B5060(const HeaderPacket* a, const HeaderPacket* b, f32 t) { fn_801B4D80(fn_80178C90(), a, b, t); }

void fn_801B50B4(ReplayObj** list, int n) {
    if (lbl_8055E6D8 == 0 || !fn_801B4B44(lbl_8055E6D8)) {
        return;
    }
    u8* a;
    u8* b;
    f32 t;
    if (fn_801B49D4(lbl_8055E6D8, &a, &b, &t, lbl_8055E3C4)) {
        u8* start = a;
        fn_801B5060((HeaderPacket*)a, (HeaderPacket*)b, t);
        a += sizeof(HeaderPacket);
        b += sizeof(HeaderPacket);
        for (int i = 0; i < n; list++, i++) {
            ReplayObj* o = *list;
            if (o && o->Replays()) {
                s32 sizeA = -1;
                s32 sizeB = -1;
                o->Play(a, b, t, &sizeA, &sizeB);
                a += sizeA;
                b += sizeB;
            }
        }
        if (lbl_8055E6DC != start) {
            for (;;) {
                if (*a >= 16) {
                    break;
                }
                EventHandler h = lbl_80435C70.handlers[*a];
                if (!h) {
                    break;
                }
                a += h(a + 1, lbl_8055E6D4) + 1;
            }
        }
        lbl_8055E6DC = start;
        fn_801B4AC4(lbl_8055E6D8, lbl_8055E3C4);
    }
    if (fn_801B4C1C(lbl_8055E6D8, lbl_8055E3C4)) {
        fn_801769E0();
    }
}

u8 fn_801B526C(void) { return lbl_8055E6D0; }

void fn_801B5274(void) {
    memset(lbl_80435C70.handlers, 0, sizeof(lbl_80435C70.handlers));
    lbl_80435C70.unk200 = 0;
    lbl_80435C70.unk204 = 0;
    lbl_8055E6D8 = 0;
    lbl_8055E6D0 = 0;
    lbl_8055E6D4 = 3;
}

void fn_801B52CC(void) {
    fn_802E30C8("replay: done creating");
    Replay* r = (Replay*)operator new(sizeof(Replay), "ReplayPacketQueue::recording");
    if (r) {
        r = fn_801B4904(r);
    }
    lbl_8055E6D8 = r;
}

void fn_801B5318(void* p) {}

void fn_801B531C(void) {
    if (lbl_8055E6D8) {
        fn_801B496C(lbl_8055E6D8, 1);
        lbl_8055E6D8 = 0;
    }
}

void fn_801B5354(void) { fn_801B49C8(lbl_8055E6D8); }

void fn_801B535C(void* list, int n) {}

void fn_801B5360(void) {}

void fn_801B5364(int type, EventHandler h) { lbl_80435C70.handlers[type] = h; }

void fn_801B537C(EventHandler h) {
    for (int i = 0; i < 16; i++) {
        if (lbl_80435C70.handlers[i] == h) {
            lbl_80435C70.handlers[i] = 0;
            return;
        }
    }
    fn_802E317C("PhysReplayUninstallEventHandler: couldn't find handler 0x%x!", h);
}

}  // extern "C"
