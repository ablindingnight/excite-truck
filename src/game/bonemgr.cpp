// Bone manager (0x802D9C5C-0x802DACD4): bone pose commands built in a 100K stack allocator ("StackAlloc",
// each block framed by a used marker and its size) - a set of vectors or matrices for a range of bone slots,
// optionally with a list of extra words - and applied by copying them into the shared bone vector / matrix /
// word tables. Applying no command applies the default pose (80 zero vectors). Names are guesses apart from
// the strings.
#include "game/common.h"

extern "C" {
void* memset(void* p, int c, u32 n);
void* memcpy(void* d, const void* s, u32 n);
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
}

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);
void operator delete(void* p);
void operator delete[](void* p);

class AllocBuf {
public:
    AllocBuf(u8* b) { buf = b; }
    virtual ~AllocBuf(void) { buf = 0; }
    u8* buf;  // offset 0x4
};

class StackAlloc : public AllocBuf {
public:
    StackAlloc(const char* n, u8* b, s32 s) : AllocBuf(b) {
        name = n;
        used = 0;
        size = s;
    }
    virtual ~StackAlloc(void) { delete[] buf; }
    void* Alloc(s32 n) {
        s32 a = (n + 3) & ~3;
        if (a + used + 8 > size) {
            fn_802E317C("StackAlloc(%s:%d)::insufficient memory (max %dK)", name, n, size / 1024);
            return 0;
        }
        s32* h = (s32*)(buf + used);
        *h = 1;
        void* p = h + 1;
        *(s32*)(buf + used + (a + 4)) = a;
        used = a + used + 8;
        return p;
    }
    const char* name;  // offset 0x8
    volatile s32 size;  // offset 0xC
    volatile s32 used;  // offset 0x10
};

struct BVec {
    BVec(void);
    f32 x;
    f32 y;
    f32 z;
};

struct BMtx {
    BMtx(void);
    void Identity(void) {
        m[0][0] = 1.0f;
        m[0][1] = 0.0f;
        m[0][2] = 0.0f;
        m[1][0] = 0.0f;
        m[1][1] = 1.0f;
        m[1][2] = 0.0f;
        m[2][0] = 0.0f;
        m[2][1] = 0.0f;
        m[2][2] = 1.0f;
        m[3][0] = 0.0f;
        m[3][1] = 0.0f;
        m[3][2] = 0.0f;
    }
    f32 m[4][3];
};

struct BMtx33 {
    void Identity(void) {
        m[0][0] = 1.0f;
        m[0][1] = 0.0f;
        m[0][2] = 0.0f;
        m[1][0] = 0.0f;
        m[1][1] = 1.0f;
        m[1][2] = 0.0f;
        m[2][0] = 0.0f;
        m[2][1] = 0.0f;
        m[2][2] = 1.0f;
    }
    f32 m[3][3];
};

// Extra words carried by a command (0x8 + 4 * count).
struct BoneWords {
    s32 start;    // offset 0x0
    s32 count;    // offset 0x4
    u32 data[1];  // offset 0x8
};

// A pose command; the vectors or matrices follow the header.
struct BoneCmd {
    u8* Data(void) { return (u8*)(this + 1); }
    s32 type;          // offset 0x0
    s32 start;         // offset 0x4
    s32 count;         // offset 0x8
    s32 mtx;           // offset 0xC
    u8 flag;           // offset 0x10
    BoneWords* words;  // offset 0x14
};

typedef void (*BoneCopy)(int start, const void* src, int n);

class BoneMgr {
public:
    BoneMgr(BoneCopy v, BoneCopy m, BoneCopy w) : alloc("BoneHandleAlloc", new ("StackAlloc::buf") u8[0x19000], 0x19000) {
        copyVec = v;
        copyMtx = m;
        copyWords = w;
    }
    ~BoneMgr(void) {}
    BoneCmd* NewCmd(int start, const void* src, int n, int size, int mtx, BoneWords* words) {
        u8 flag = dirty;
        dirty = 0;
        BoneCmd* c = (BoneCmd*)alloc.Alloc(sizeof(BoneCmd) + size);
        c->type = 7;
        c->start = start;
        c->count = n;
        c->mtx = mtx;
        c->words = words;
        c->flag = flag;
        memcpy(c->Data(), src, size);
        return c;
    }
    StackAlloc alloc;    // offset 0x0
    BoneCmd* Rest(void) {
        if (rest == (BoneCmd*)-1) {
            f32 zero[80][3];
            memset(zero, 0, sizeof(zero));
            dirty = 1;
            rest = NewCmd(0, zero, 80, sizeof(zero), 0, 0);
        }
        return rest;
    }
    BoneCopy copyVec;    // offset 0x14
    BoneCopy copyMtx;    // offset 0x18
    BoneCopy copyWords;  // offset 0x1C
    BoneCmd* rest;       // offset 0x20
    u8 flag;             // offset 0x24
    u8 dirty;            // offset 0x25
};

ModuleTag lbl_8055FE50;
u8 lbl_8055FE58;
BoneMgr* lbl_8055FE5C;
BoneCmd* lbl_8055FE60;
BMtx lbl_804DAC48[22];
BVec lbl_804DB068[81];
BMtx33 lbl_804DB434;
u32 lbl_804DB458[4];

extern "C" {

void fn_802D9C5C(int start, const BVec* src, int n) {
    for (int i = 0; i < n; i++) {
        lbl_804DB068[start + i] = src[i];
    }
}

void fn_802D9DE4(int start, const BMtx* src, int n) {
    for (int i = 0; i < n; i++) {
        lbl_804DAC48[start + i] = src[i];
    }
}

void fn_802DA1F4(int start, const u32* src, int n) {
    for (int i = 0; i < n; i++) {
        lbl_804DB458[start + i] = src[i];
    }
    lbl_8055FE58 = 1;
}

void fn_802DA2F4(void) {
    lbl_8055FE5C = new ("bonemgr") BoneMgr((BoneCopy)fn_802D9C5C, (BoneCopy)fn_802D9DE4, (BoneCopy)fn_802DA1F4);
    for (int i = 0; i < 22; i++) {
        lbl_804DAC48[i].Identity();
    }
    lbl_804DB434.Identity();
}

void fn_802DA4C4(void) {
    delete lbl_8055FE5C;
    lbl_8055FE5C = 0;
}

void fn_802DA52C(void) {
    BoneMgr* m = lbl_8055FE5C;
    m->alloc.used = 0;
    m->rest = (BoneCmd*)-1;
    m->flag = 1;
    m->dirty = 0;
}

BoneCmd* fn_802DA550(int start, const BVec* v, int n, int wstart, const u32* w, int wn) {
    BoneMgr* m = lbl_8055FE5C;
    BoneWords* words = (BoneWords*)m->alloc.Alloc(sizeof(BoneWords) - 4 + wn * 4);
    words->start = wstart;
    words->count = wn;
    memcpy(words->data, w, wn * 4);
    return m->NewCmd(start, v, n, n * sizeof(BVec), 0, words);
}

BoneCmd* fn_802DA704(int start, const BVec* v, int n) {
    BoneMgr* m = lbl_8055FE5C;
    return m->NewCmd(start, v, n, n * sizeof(BVec), 0, 0);
}

BoneCmd* fn_802DA80C(int start, const BMtx* v, int n) {
    BoneMgr* m = lbl_8055FE5C;
    return m->NewCmd(start, v, n, n * sizeof(BMtx), 1, 0);
}

BoneCmd* fn_802DA918(f32 a, f32 b, f32 c) {
    f32 v[3];
    v[0] = a;
    v[1] = b;
    v[2] = 1.0f / (c * c);
    BoneMgr* m = lbl_8055FE5C;
    return m->NewCmd(1, v, 1, sizeof(v), 0, 0);
}

void fn_802DAA18(BoneCmd* c) {
    lbl_8055FE60 = c;
    BoneMgr* m = lbl_8055FE5C;
    if (!c) {
        if (m->rest == (BoneCmd*)-1) {
            f32 zero[80][3];
            memset(zero, 0, sizeof(zero));
            m->dirty = 1;
            m->rest = m->NewCmd(0, zero, 80, sizeof(zero), 0, 0);
        }
        c = m->rest;
    }
    if (c->mtx == 0) {
        m->copyVec(c->start, c->Data(), c->count);
    } else {
        m->copyMtx(c->start, c->Data(), c->count);
    }
    BoneWords* w = c->words;
    if (w) {
        m->copyWords(w->start, w->data, w->count);
    }
    m->flag = c->flag;
}

void fn_802DABA4(const f32* p) {
    f32 v[12];
    v[0] = p[0] * 10.0f;
    v[1] = p[1] * 10.0f;
    v[2] = p[2] * 10.0f;
    v[3] = p[0] * -10.0f;
    v[4] = p[1] * -10.0f;
    v[5] = p[2] * -10.0f;
    v[6] = p[3] * 10.0f;
    v[7] = p[4] * 10.0f;
    v[8] = p[5] * 10.0f;
    v[9] = p[3] * -10.0f;
    v[10] = p[4] * -10.0f;
    v[11] = p[5] * -10.0f;
    fn_802DAA18(fn_802DA704(0x4C, (const BVec*)v, 4));
}

void fn_802DAC54(void) { lbl_8055FE5C->dirty = 1; }

u8 fn_802DAC64(void) { return lbl_8055FE5C->flag; }

}  // extern "C"
