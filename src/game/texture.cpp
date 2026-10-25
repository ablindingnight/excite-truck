// Textures (0x802CA0E4-0x802CCBC0, texture.cpp): the 512-slot texture table (reference-counted "TEX "
// tt_texture resources by name, render targets, the null and lightmap textures) and the mip0 texture
// cache that streams the full-resolution level of big textures (".tm0") into a 1.5M SimpleHeap by
// on-screen size, discarding the least useful ones a couple of frames later. Names are guesses apart
// from the strings.
#include "game/common.h"

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

class AsyncResource {
public:
    s32 Span(void);
    void Load(u8* dst, s32 n, s32 offs);
    u8* Poll(s32* sizeOut);
    void Cancel(void);
};

// A tt_texture header ("TEX " resource, version 15).
struct TTHeader {
    u16 w;        // offset 0x0
    u16 h;        // offset 0x2
    s8 fmt;       // offset 0x4
    u8 flags;     // offset 0x5 (0x80: fixed up)
    u8 wrapS;     // offset 0x6
    u8 wrapT;     // offset 0x7
    u8 prio;      // offset 0x8
    u8 prio2;     // offset 0x9
    u8 unkA;      // offset 0xA
    u8 mips;      // offset 0xB
    u16 zhint;    // offset 0xC
    u8 padE[6];
    u32 data;     // offset 0x14
};

// What the GX layer gets to build a texture object (0x34).
struct GXTexDesc {
    s32 format;   // offset 0x0
    s32 mode;     // offset 0x4
    s32 flags;    // offset 0x8
    s32 zhint;    // offset 0xC
    s32 wrapS;    // offset 0x10
    s32 wrapT;    // offset 0x14
    s32 mips;     // offset 0x18
    u8 unk1C;     // offset 0x1C
    s32 fmt;      // offset 0x20
    s32 w;        // offset 0x24
    s32 h;        // offset 0x28
    u32 data;     // offset 0x2C
    s32 user;     // offset 0x30
};

struct TexEntry {
    s32 refs;         // offset 0x0
    char name[32];    // offset 0x4
    s32 key;          // offset 0x24
    void* gx;         // offset 0x28
    TTHeader* res;    // offset 0x2C
    u8 shared;        // offset 0x30 (resource supplied by the caller)
    s32 kind;         // offset 0x34
    f32 size;         // offset 0x38 (smallest on-screen size this frame)
    s32 mip0Bytes;    // offset 0x3C
    void* mip0;       // offset 0x40
    void* mip0Mem;    // offset 0x44
    s32 uses;         // offset 0x48
};

extern "C" {
void* memset(void* p, int c, u32 n);
u32 strlen(const char* s);
char* strcpy(char* d, const char* s);
char* strchr(const char* s, int c);
char* strrchr(const char* s, int c);
int strcasecmp(const char* a, const char* b);
void* fn_8012C370(const char* name, u32 type, u32* version, u32* size, u8* first, u8* swapped);
void fn_8012C4D8(void* res);
AsyncResource* fn_8012C5B8(const char* name, u32 type, u32* version);
void fn_8012C958(AsyncResource* r);
void fn_801282EC(int a);
void fn_801282F4(void);
int fn_802C90E0(void);
void* fn_802D8DCC(const GXTexDesc* d, const char* name);
void* fn_802D8E9C(const GXTexDesc* d, const char* name);
void fn_802D8FB4(void* gx);
int fn_802D9068(void);
void* fn_802D9154(void);
void fn_802D915C(void* gx);
void fn_802D9160(void* gx, int map);
void fn_802D92B8(void* gx);
void fn_802D93E8(void* gx);
void fn_802D9418(void);
void fn_802DB2C0(void* gx);
void fn_802DB2C4(void* gx);
void fn_802DBDE8(void* a, void* gx);
u32 fn_802E2C20(void);
void fn_802E30C8(const char* fmt, ...);
void fn_802E317C(const char* fmt, ...);
}

struct SimpleHeap;

struct HeapBlock {
    u32 magic;         // offset 0x0
    s32 size;          // offset 0x4
    HeapBlock* next;   // offset 0x8
    SimpleHeap* owner; // offset 0xC
};

struct SimpleHeap {
    SimpleHeap(void* mem, s32 n) {
        used = 0;
        align = 0x7F;
        HeapBlock* b = (HeapBlock*)mem;
        b->magic = 0xABCFEED0;
        b->next = 0;
        b->size = n - ((align + 0x10) & ~align);
        b->owner = this;
        free = b;
        strict = 1;
    }
    ~SimpleHeap(void) {
        if (used) {
            Dump("~SimpleHeap");
            fn_802E317C("SimpleHeap Destroyed with pending allocations");
        }
    }
    void* Alloc(s32 n);
    void Dump(const char* who) {
        fn_802E30C8("SimpleHeap dump: %s", who);
        fn_802E30C8("Allocated:");
        u32 total = 0;
        for (HeapBlock* b = used; b; b = b->next) {
            fn_802E30C8("\tuser ptr %8.8p: %d", b + 1, b->size);
            total += b->size;
        }
        fn_802E30C8("\t%dK total", total >> 10);
        fn_802E30C8("Free:");
        total = 0;
        for (HeapBlock* b = free; b; b = b->next) {
            fn_802E30C8("\tuser ptr %8.8p: %d", b + 1, b->size);
            total += b->size;
        }
        fn_802E30C8("\t%dK total", total >> 10);
    }
    void Free(void* p) {
        HeapBlock* b = (HeapBlock*)((u8*)p - (align + 1));
        HeapBlock** pp;
        HeapBlock* q;
        for (pp = &used; (q = *pp) != 0; pp = &q->next) {
            if (q == b) {
                *pp = b->next;
                break;
            }
        }
        b->next = free;
        free = b;
        HeapBlock** bp = &free;
        int merged;
        do {
            merged = 0;
            for (pp = &free; *pp; pp = &(*pp)->next) {
                HeapBlock* m = *bp;
                HeapBlock* o = *pp;
                int adj = 1;
                if ((u8*)m + align + m->size + 1 != (u8*)o && (u8*)o + align + o->size + 1 != (u8*)m) {
                    adj = 0;
                }
                if (adj) {
                    HeapBlock** first = bp;
                    if ((u8*)o + align + o->size + 1 == (u8*)m) {
                        first = pp;
                        pp = bp;
                    }
                    (*first)->size += (*pp)->size + ((align + 0x10) & ~align);
                    *pp = (*pp)->next;
                    bp = first;
                    merged = 1;
                    break;
                }
            }
        } while (merged);
    }

    u8 strict;         // offset 0x0
    s32 align;         // offset 0x4
    HeapBlock* free;   // offset 0x8
    HeapBlock* used;   // offset 0xC
};

namespace {

// A streaming load of one mip0 (0x18).
struct MipLoad {
    AsyncResource* res; // offset 0x0
    s32 tex;            // offset 0x4
    void* mem;          // offset 0x8
    void* gx;           // offset 0xC
    s32 wait;           // offset 0x10 (-1 loading, else discards still pending)
    MipLoad* next;      // offset 0x14
};

// A mip0 discarded but kept until the GPU is done with it (0x14).
struct MipDiscard {
    const char* name;   // offset 0x0
    void* gx;           // offset 0x4
    s32 frame;          // offset 0x8
    MipLoad* load;      // offset 0xC
    MipDiscard* next;   // offset 0x10
};

struct MipSlot {
    void Init(s32 i) { tex = i; }
    s32 tex;         // offset 0x0
    s32 state;       // offset 0x4 (0 none, 1 loading, 2 resident)
    f32 score;       // offset 0x8
    s32 frame;       // offset 0xC
    MipSlot* next;   // offset 0x10
};

struct MipHeap {
    void* mem;          // offset 0x0
    SimpleHeap* heap;   // offset 0x4
    s32 size;           // offset 0x8
    s32 unkC;           // offset 0xC
    s32 unk10;          // offset 0x10
    s32 unk14;          // offset 0x14
};

struct TexCache {
    TexCache(void);
    ~TexCache(void) {
        delete[] loadPool;
        delete[] discardPool;
        heapRef = 0;
        delete mip.heap;
        delete[] (u8*)mip.mem;
        mip.mem = 0;
        mip.heap = 0;
        mip.size = 0;
    }

    MipLoad* loads;          // offset 0x0
    MipLoad* freeLoads;      // offset 0x4
    MipLoad* loadPool;       // offset 0x8
    MipDiscard* discards;    // offset 0xC
    MipDiscard* freeDiscards; // offset 0x10
    MipDiscard* discardPool; // offset 0x14
    s32 loadedBytes;         // offset 0x18
    u32 startTime;           // offset 0x1C
    MipHeap mip;             // offset 0x20
    MipHeap* heapRef;        // offset 0x38
    MipSlot slots[512];      // offset 0x3C
    MipSlot* resident;       // offset 0x283C
    MipSlot* cut;            // offset 0x2840
    MipSlot* best;           // offset 0x2844
    s32 frame;               // offset 0x2848
};


}  // namespace

static inline void StrCopy(char* d, const char* s, int n) {
    char* p = d;
    while ((*p++ = *s++) != 0) {
        if (--n <= 0) {
            p[-1] = 0;
            break;
        }
    }
}

static inline u16 TexShort(int v) { return v; }

ModuleTag lbl_8055FCA0;
u16 lbl_8055FCA8 = TexShort(0x100);
u16 lbl_8055FCAA = TexShort(0x405);
u16 lbl_8055FCAC = TexShort(0x401);
TexEntry* lbl_8055FCB0;
s32 lbl_8055FCB4;     // null texture
s32 lbl_8055FCB8;     // bound texture
TexCache* lbl_8055FCBC;
s32 lbl_8055FCC0;
f32 lbl_8055FCC4;     // size scale

const char* lbl_8055D038 = "null.tex";
s32 lbl_8055D03C = -1;  // lightmap
char lbl_8055D050[] = "mip-0";
const char* lbl_8055D058 = lbl_8055D050;
s32 lbl_8055D06C = -2;
u8 lbl_8055D07D = 1;    // mip0 streaming on

extern "C" {

void fn_802CB424(void);
void fn_802CB6B4(void);
void fn_802CB82C(s32 id);
s32 fn_802CAC28(const char* name, s32 key);
void fn_802CAC54(s32 id);
void fn_802CAD28(s32 id);
TexEntry* fn_802CB018(void);
TTHeader* fn_802CB2FC(u16 w, u16 h, u8 a, u8 b, u16 c, u8 d);
void fn_802CB3F4(void* gx);
void fn_802CB3F8(const char* name, const char* what);
void fn_802CBB9C(MipLoad** to, MipLoad** from, MipLoad* l);
void fn_802CBBE8(MipLoad** list, MipLoad* l);
void fn_802CBD30(MipDiscard** to, MipDiscard** from, MipDiscard* d);
void fn_802CBD7C(MipDiscard** list, MipDiscard* d);
void fn_802CBFA0(TexCache* c, MipLoad* l);
void fn_802CBFCC(TexCache* c, MipLoad* l);
void fn_802CC03C(TexCache* c, MipLoad* l, s32 id);
static int fn_802CC10C(TexCache* c, s32 id, s32 frame, MipLoad* l);
SimpleHeap* fn_802CC404(TexCache* c);

// Stripped from the DOL; only their strings remain.
int TexBitsPerTexel(int fmt) {
    fn_802E317C("texture: get_bits_per_texel: unknown tex type %d", fmt);
    return 0;
}

void fn_802CA0E4(void) {
    lbl_8055FCB0 = new ("texlist") TexEntry[512];
    fn_802CB424();
    for (int i = 0; i < 512; i++) {
        memset(&lbl_8055FCB0[i], 0, sizeof(TexEntry));
    }
    lbl_8055FCB8 = -2;
    lbl_8055FCB4 = fn_802CAC28(lbl_8055D038, 0);
    if (lbl_8055FCB4 == -1) {
        fn_802E317C("texture: could not load %s!!", lbl_8055D038);
    }
    lbl_8055D03C = fn_802CAC28("gclmap.tex", 0);
    if (lbl_8055D03C != -1) {
        fn_802E30C8("texture: loaded lightmap");
        fn_802D92B8(lbl_8055FCB0[lbl_8055D03C].gx);
    } else {
        fn_802E30C8("texture: can't load lightmap (pre-genesis)");
    }
}

void fn_802CA1E0(void) {
    if (lbl_8055D03C != -1) {
        fn_802CAC54(lbl_8055D03C);
        lbl_8055D03C = -1;
    }
    fn_802CAC54(lbl_8055FCB4);
    lbl_8055FCB4 = -1;
    for (int i = 0; i < 512; i++) {
        if (lbl_8055FCB0[i].refs) {
            fn_802E317C("gxTexture: texture %d (%s) still in use", i, lbl_8055FCB0[i].name);
        }
    }
    fn_802CB6B4();
    delete[] lbl_8055FCB0;
    lbl_8055FCB0 = 0;
}

void* fn_802CA294(s32 id) { return lbl_8055FCB0[id].gx; }

// name.ext -> name.tm0
char* fn_802CA2A8(char* dst, int n, const char* name) {
    StrCopy(dst, name, n);
    char* dot = strrchr(dst, '.');
    *dot = 0;
    char* p = dst;
    while (*p) {
        n--;
        p++;
    }
    if (n > 0) {
        StrCopy(p, ".tm0", n);
    }
    return dst;
}

s32 fn_802CA380(const char* name) {
    char buf[32];
    u32 ver;
    s32 span = 0;
    fn_802CA2A8(buf, 32, name);
    AsyncResource* r = fn_8012C5B8(buf, 'TEX ', &ver);
    if (r) {
        span = r->Span();
        fn_8012C958(r);
    }
    return span;
}

void fn_802CA3F4(GXTexDesc* d, const TTHeader* h, s32 mode, u32 data, s32 user) {
    memset(d, 0, sizeof(GXTexDesc));
    d->flags = h->mips & 1;
    d->format = h->flags & 0x7F;
    d->wrapT = h->wrapT;
    d->wrapS = h->wrapS;
    d->zhint = h->zhint;
    d->mips = h->mips;
    d->unk1C = h->unkA;
    d->mode = mode;
    d->fmt = h->fmt;
    d->w = h->w;
    d->h = h->h;
    d->data = h->data;
    d->user = data;
    if (user == 2) {
        d->flags |= 1;
    } else if (user == 6) {
        d->flags |= 2;
    } else if (user == 5) {
        d->flags |= 4;
    }
}

void* fn_802CA4E0(const TTHeader* h, const char* name, int rt, u32 data, s32 kind) {
    GXTexDesc d;
    if (!fn_802C90E0() && name != lbl_8055D058) {
        fn_802E30C8("texture: create %s in scene", name);
    }
    fn_802CA3F4(&d, h, rt == 1 ? 3 : 1, data, kind);
    return fn_802D8E9C(&d, name);
}

static inline void Swap16(void* p) {
    u8* b = (u8*)p;
    u8 t = b[0];
    b[0] = b[1];
    b[1] = t;
}

static inline void Swap32(void* p) {
    u8* b = (u8*)p;
    u8 t = b[0];
    b[0] = b[3];
    b[3] = t;
    t = b[1];
    b[1] = b[2];
    b[2] = t;
}

void fn_802CA57C(TTHeader* h, const char* name, u32 data, s32 kind) {
    if (!(h->flags & 0x80)) {
        GXTexDesc d;
        Swap32(&h->data);
        Swap16(&h->w);
        Swap16(&h->h);
        Swap16(&h->zhint);
        h->data = (u32)((u8*)h + h->data);  // offset -> pointer
        fn_802CA3F4(&d, h, 0, data, kind);
        fn_802D8DCC(&d, name);
        h->flags |= 0x80;
    }
}

TTHeader* fn_802CA658(const char* name, s32 kind) {
    u32 ver;
    u32 size;
    u8 first = 0;
    u8 swapped = 0;
    TTHeader* h = (TTHeader*)fn_8012C370(name, 'TEX ', &ver, &size, &first, &swapped);
    if (h) {
        if ((u32)h & 0x7F) {
            fn_802E317C("tt_texture(%s) misaligned (0x%x)", name, h);
        }
        if (ver != 15) {
            fn_802E317C("tt_texture(%s) bad version (%d)", name, ver);
        }
        if (swapped) {
            fn_802CA57C(h, name, 0, kind);
        }
    }
    return h;
}

const char* fn_802CA72C(u32 type) {
    switch (type) {
    case 0:
        return "MMT_BASELAYER";
    case 1:
        return "MMT_BASELAYER_WITH_SIMPLE_SPECULAR";
    case 2:
        return "MMT_CHROME";
    case 3:
        return "MMT_ENVLAYER";
    case 4:
        return "MMT_DECAL_LAYER";
    case 5:
        return "MMT_DECAL_ENVLAYER";
    case 6:
        return "MMT_ZWRITE";
    case 7:
        return "MMT_GLASS";
    case 8:
        return "MMT_GLASS_ENVLAYER";
    case 9:
        return "MMT_GLASS_DECAL";
    case 10:
        return "MMT_GLASS_DECAL_ENVLAYER";
    case 11:
        return "MMT_DIRT_LAYER";
    case 12:
        return "MMT_INVALID";
    case 13:
        return "MMT_UNTEXTURED_BASELAYER";
    case 14:
        return "MMT_UNKNOWN";
    }
    return "<unknown>";
}

int fn_802CA7D4(const char* name, s32 kind, s32 cur) { return 1; }

int fn_802CA7DC(const char* name) { return *name == '*'; }

int fn_802CA7F4(const char* name) { return strchr(name, '/') != 0; }

const char* fn_802CA824(const char* name) {
    if (fn_802CA7DC(name)) {
        const char* p = strchr(name, '/');
        if (!p) {
            fn_802E317C("texture: bad extraction name %s", name);
        }
        return p + 1;
    }
    return name;
}

s32 fn_802CA898(const char* name, s32 key, TTHeader* res, s32 kind) {
    if (strlen(name) >= 16 && !fn_802CA7DC(name) && !fn_802CA7F4(name)) {
        fn_802E317C("texture: \"%s\" is too long (%d >= %d)", name, strlen(name), 16);
    }
    if (strlen(name) >= 32) {
        fn_802E317C("texture: \"%s\" is really too long (%d >= %d)", name, strlen(name), 32);
    }
    if (*name == 0 && kind == 1) {
        name = "white.tex";
    }
    int shared = res != 0;
    if (*name == 0 || *name == '.' || *name == '#') {
        return -1;
    }
    int slot = -1;
    int i;
    for (i = 0; i < 512; i++) {
        TexEntry* t = &lbl_8055FCB0[i];
        if (t->refs > 0) {
            if (strcasecmp(t->name, fn_802CA824(name)) == 0 && t->key == key && !shared) {
                if (!fn_802CA7D4(name, kind, lbl_8055FCB0[i].kind)) {
                    fn_802E30C8("texture: disabling caching on %s", name);
                    lbl_8055FCB0[i].mip0Bytes = 0;
                    lbl_8055FCB0[i].kind = 14;
                }
                t->refs++;
                return i;
            }
        } else if (slot < 0) {
            slot = i;
        }
    }
    if (slot < 0) {
        fn_802E317C("Out of textures (%s)", name);
    }
    if (!shared) {
        res = fn_802CA658(name, kind);
        if (!res) {
            fn_802E30C8("texture: missing texture %s", name);
            res = fn_802CA658("badtex.tex", 14);
        }
    } else {
        fn_802CA57C(res, name, 0, kind);
    }
    if (!res) {
        return -1;
    }
    TexEntry* t = &lbl_8055FCB0[slot];
    t->shared = shared;
    t->refs = 1;
    t->uses = 0;
    t->key = key;
    if (!fn_802CA7DC(name)) {
        StrCopy(t->name, name, 32);
        s32 span = shared ? 0 : fn_802CA380(name);
        t->mip0Bytes = span;
        if (span < 16000) {
            t->mip0Bytes = 0;
        }
    } else {
        fn_802E30C8("texture: WARNING: extraction texture %s", name);
        const char* src = fn_802CA824(name);
        StrCopy(t->name, src, 32);
        t->mip0Bytes = 0;
    }
    t->mip0 = 0;
    t->mip0Mem = 0;
    t->res = res;
    t->kind = kind;
    t->gx = fn_802CA4E0(res, name, 0, 0, kind);
    lbl_8055FCB8 = -2;
    return slot;
}

s32 fn_802CAC28(const char* name, s32 key) { return fn_802CA898(name, key, 0, 14); }

s32 fn_802CAC34(const char* name, TTHeader* res) { return fn_802CA898(name, 0, res, 14); }

s32 fn_802CAC44(const char* name, s32 kind) { return fn_802CA898(name, 0, 0, kind); }

void fn_802CAC54(s32 id) {
    if (id == -1) {
        return;
    }
    if (id >= 0 && id < 512 && lbl_8055FCB0[id].refs <= 0) {
        fn_802E317C("gxForgetTexture: \"%s\" refcount = %d", lbl_8055FCB0[id].name, lbl_8055FCB0[id].refs);
    }
    TexEntry* t = &lbl_8055FCB0[id];
    if (t->shared) {
        fn_802CAD28(id);
        return;
    }
    if (--t->refs == 0 && t->res) {
        fn_802CB82C(id);
        fn_8012C4D8(t->res);
        t->res = 0;
        fn_802D8FB4(t->gx);
    }
}

void fn_802CAD28(s32 id) {
    if (id == -1) {
        return;
    }
    TexEntry* t = &lbl_8055FCB0[id];
    if (--t->refs == 0 && t->res) {
        t->res = 0;
        fn_802D8FB4(t->gx);
    }
}

void fn_802CAD68(void) { lbl_8055FCB8 = -2; }

void fn_802CAD74(s32 id) {
    if (id == lbl_8055FCB8) {
        return;
    }
    lbl_8055FCB8 = id;
    if (id == -1) {
        fn_802CAD74(lbl_8055FCB4);
        return;
    }
    TexEntry* t = &lbl_8055FCB0[id];
    t->uses++;
    if (t->mip0) {
        fn_802D9160(t->mip0, 0);
    } else {
        fn_802D9160(t->gx, 0);
    }
}

void fn_802CADD0(s32 id, void* a) {
    if (id == -1) {
        fn_802DBDE8(a, 0);
    } else {
        fn_802DBDE8(a, lbl_8055FCB0[id].gx);
    }
}

int fn_802CAE00(s32 id) {
    int ok = 0;
    if (id >= 0 && id < 512 && lbl_8055FCB0[id].refs > 0) {
        ok = 1;
    }
    return ok;
}

const char* fn_802CAE34(s32 id) {
    if (id == -1) {
        return "TEXTURE_NULL";
    }
    if (fn_802CAE00(id)) {
        return lbl_8055FCB0[id].name;
    }
    return "(INVALID)";
}

void fn_802CAE98(s32 id, const char* name) {
    if (fn_802CAE00(id)) {
        StrCopy(lbl_8055FCB0[id].name, name, 32);
    }
}

void fn_802CAF1C(s32 id, u8 a, u8 b) {
    if (fn_802CAE00(id)) {
        lbl_8055FCB0[id].res->prio = a;
        lbl_8055FCB0[id].res->prio2 = b;
    }
}

int fn_802CAF8C(s32 id) {
    if (id == -1) {
        return 0;
    }
    u16* z = &lbl_8055FCB0[id].res->zhint;
    return *z != lbl_8055FCA8;
}

u8 fn_802CAFC8(s32 id) {
    if (id == -1) {
        return 5;
    }
    return lbl_8055FCB0[id].res->prio;
}

u8 fn_802CAFF0(s32 id) {
    if (id == -1) {
        return 50;
    }
    return lbl_8055FCB0[id].res->prio2;
}

void TexSummary(void) {
    fn_802E30C8("gxTextureSummary:");
    for (int i = 0; i < 512; i++) {
        if (lbl_8055FCB0[i].refs) {
            fn_802E30C8("%16s : %4d", lbl_8055FCB0[i].name, lbl_8055FCB0[i].uses);
        }
    }
}

TexEntry* fn_802CB018(void) {
    int slot = -1;
    for (int i = 1; i < 512; i++) {
        if (lbl_8055FCB0[i].refs == 0) {
            slot = i;
            break;
        }
    }
    if (slot < 0) {
        fn_802E317C("tex: Out of texture slots");
    }
    return &lbl_8055FCB0[slot];
}

void TexGrab(void) { fn_802E317C("gxGrabTexture not supported"); }

void TexGrabOpaque(s32 id) {
    if (id == -1) {
        fn_802E317C("gxGrabOpaqueTexture: grabbing NULL texture");
    }
}

void TexCopy(void) { fn_802E317C("gxCopyTexture not supported"); }

s32 fn_802CB128(u16 w, u16 h, int unused, u32 flags) {
    u16 z = lbl_8055FCAA;
    const char* name = "rtarg";
    u8 wrap = 3;
    if (flags & 0x20) {
        wrap = 2;
    }
    if (flags & 0x40) {
        wrap &= ~2;
    }
    u8 fmt = 2;
    if (flags & 1) {
        fmt = 3;
    }
    TexEntry* t = fn_802CB018();
    memset(t, 0, sizeof(TexEntry));
    t->refs = 1;
    t->key = 0;
    t->mip0Bytes = 0;
    t->mip0 = 0;
    t->mip0Mem = 0;
    t->kind = 14;
    strcpy(t->name, name);
    t->res = fn_802CB2FC(w, h, 1, fmt, z, wrap);
    t->gx = fn_802CA4E0(t->res, "<rt>", 1, 0, t->kind);
    return t - lbl_8055FCB0;
}

void fn_802CB228(s32 id) {
    TexEntry* t = &lbl_8055FCB0[id];
    fn_802D8FB4(t->gx);
    delete t->res;
    t->res = 0;
    t->refs = 0;
}

void fn_802CB274(s32 id) { fn_802DB2C0(lbl_8055FCB0[id].gx); }

void fn_802CB288(s32 id) { fn_802DB2C4(lbl_8055FCB0[id].gx); }

void fn_802CB29C(s32 id) {
    if (id != -1 && id != -2) {
        fn_802D93E8(lbl_8055FCB0[id].gx);
        lbl_8055D06C = id;
    } else {
        fn_802D9418();
        lbl_8055D06C = -2;
    }
}

TTHeader* fn_802CB2FC(u16 w, u16 h, u8 fmt, u8 flags, u16 zhint, u8 wrapT) {
    TTHeader* t = new ("tt tex") TTHeader;
    t->w = w;
    t->h = h;
    t->fmt = fmt;
    t->flags = flags;
    t->zhint = zhint;
    t->wrapS = 0;
    t->prio = 5;
    t->prio2 = 50;
    t->wrapT = wrapT;
    t->mips = 2;
    t->data = 0;
    t->unkA = 0;
    return t;
}

void* fn_802CB38C(TTHeader* h, u32 data, s32 kind) {
    fn_802CA57C(h, "mip0", 0, kind);
    return fn_802CA4E0(h, lbl_8055D058, 0, data, kind);
}

void fn_802CB3F4(void* gx) { fn_802D8FB4(gx); }

void fn_802CB3F8(const char* name, const char* what) {}

s32 fn_802CB3FC(s32 id) {
    TTHeader* h = lbl_8055FCB0[id].res;
    s32 w = h->w;
    s32 hh = h->h;
    if (hh > w) {
        return hh;
    }
    return w;
}

}  // extern "C"

inline TexCache::TexCache(void) {
    mip.mem = 0;
    mip.heap = 0;
    mip.unkC = 0;
    mip.unk10 = 0;
    mip.unk14 = 0;
    mip.size = 0x177000;
    fn_801282EC(0x80);
    fn_801282F4();
    mip.mem = new ("mip0-memory") u8[0x177000];
    mip.heap = new ("mip0-allocator") SimpleHeap(mip.mem, 0x177000);
    mip.heap->strict = 0;
    heapRef = &mip;
    loadPool = new ("mip0-ic[]") MipLoad[2];
    freeLoads = loadPool;
    loads = 0;
    loadPool[0].next = &loadPool[1];
    loadPool[1].next = 0;
    discardPool = new ("og[]") MipDiscard[10];
    freeDiscards = discardPool;
    discards = 0;
    int i;
    for (i = 0; i < 9; i++) {
        discardPool[i].next = &discardPool[i + 1];
    }
    discardPool[i].next = 0;
    memset(&loadedBytes, 0, 8);
    best = 0;
    memset(slots, 0, sizeof(slots));
    s32 n = 0;
    for (i = 0; i < 512; i++) {
        slots[i].tex = n++;
    }
    frame = 0;
}

extern "C" {

void fn_802CB424(void) {
    lbl_8055FCBC = new ("texcache") TexCache;
}



void fn_802CB6B4(void) {
    delete lbl_8055FCBC;
    lbl_8055FCBC = 0;
}

void fn_802CB82C(s32 id) {
    TexCache* c = lbl_8055FCBC;
    MipSlot* s = &c->slots[id];
    if (s->state == 2) {
        TexEntry* t = &lbl_8055FCB0[id];
        fn_802CB3F4(t->mip0);
        t->mip0 = 0;
        c->heapRef->heap->Free(t->mip0Mem);
        t->mip0Mem = 0;
        fn_802CB3F8(t->name, "quick-unload");
    } else if (s->state == 1) {
        for (MipLoad* l = c->loads; l; l = l->next) {
            if (l->tex == id) {
                for (MipDiscard* d = c->discards; d; d = d->next) {
                    if (d->load == l) {
                        d->load = 0;
                    }
                }
                if (l->wait == -1) {
                    l->res->Cancel();
                }
                fn_8012C958(l->res);
                l->res = 0;
                fn_802D915C(l->gx);
                l->gx = 0;
                c->heapRef->heap->Free(l->mem);
                l->mem = 0;
                fn_802CBB9C(&c->freeLoads, &c->loads, l);
                break;
            }
        }
    }
    s->state = 0;
}

void fn_802CBB9C(MipLoad** to, MipLoad** from, MipLoad* l) {
    fn_802CBBE8(from, l);
    l->next = *to;
    *to = l;
}

void fn_802CBBE8(MipLoad** list, MipLoad* l) {
    int found = 0;
    MipLoad* p;
    while ((p = *list) != 0) {
        if (p == l) {
            *list = l->next;
            l->next = 0;
            found = 1;
            break;
        }
        list = &p->next;
    }
    if (!found) {
        fn_802E317C("tex:unlink_node: can't find node!");
    }
}

void fn_802CBC3C(TexCache* c, s32 frame) {
    MipLoad* l;
    MipDiscard* d = c->discards;
    while (d) {
        if (d->frame <= frame) {
            fn_802CB3F8(d->name, "DISCARDED");
            fn_802CB3F4(d->gx);
            d->gx = 0;
            fn_802CBD30(&c->freeDiscards, &c->discards, d);
            l = d->load;
            if (l) {
                if (--l->wait == 0) {
                    l->wait = -1;
                    l->res->Load((u8*)l->mem, lbl_8055FCB0[l->tex].mip0Bytes, 0);
                    fn_802CB3F8(lbl_8055FCB0[l->tex].name, "LOADING");
                }
                d->load = 0;
            }
            d = c->discards;
        } else {
            d = d->next;
        }
    }
}

void fn_802CBD30(MipDiscard** to, MipDiscard** from, MipDiscard* d) {
    fn_802CBD7C(from, d);
    d->next = *to;
    *to = d;
}

void fn_802CBD7C(MipDiscard** list, MipDiscard* d) {
    int found = 0;
    MipDiscard* p;
    while ((p = *list) != 0) {
        if (p == d) {
            *list = d->next;
            d->next = 0;
            found = 1;
            break;
        }
        list = &p->next;
    }
    if (!found) {
        fn_802E317C("tex:unlink_node: can't find node!");
    }
}

// Starts loading the best candidate mip0, evicting resident ones that score worse if needed.
// Slot a scores lower than b (so a can be evicted for b).
static inline int Below(MipSlot* a, MipSlot* b) { return a->score < b->score; }

void fn_802CBDD0(TexCache* c) {
    if (c->best && c->freeLoads) {
        MipSlot* s;
        MipSlot** pp;
        for (pp = &c->resident; (s = *pp) != 0; pp = &s->next) {
            if (Below(s, c->best)) {
                c->cut = s;
                *pp = 0;
                break;
            }
        }
        s32 id = c->best->tex;
        int started = 0;
        MipSlot* victim = c->resident;
        MipLoad* l = c->freeLoads;
        if (l) {
            if (fn_802D9068()) {
                TexEntry* t = &lbl_8055FCB0[id];
                l->wait = 0;
                for (;;) {
                    l->mem = fn_802CC404(c)->Alloc(t->mip0Bytes);
                    if (l->mem || !victim) {
                        break;
                    }
                    if (!fn_802CC10C(c, victim->tex, victim->frame + 2, l)) {
                        break;
                    }
                    victim->state = 0;
                    victim = victim->next;
                    l->wait++;
                }
                if (l->mem) {
                    fn_802CC03C(c, l, id);
                    if (l->wait == 0) {
                        fn_802CBFCC(c, l);
                    }
                    started = 1;
                } else {
                    fn_802CBFA0(c, l);
                    l->wait = 0;
                }
            } else if (victim && lbl_8055FCB0[victim->tex].mip0Mem) {
                if (fn_802CC10C(c, victim->tex, victim->frame + 2, 0)) {
                    victim->state = 0;
                }
            }
        }
        if (started) {
            c->best->state = 1;
        }
    }
}

void fn_802CBFA0(TexCache* c, MipLoad* l) {
    for (MipDiscard* d = c->discards; d; d = d->next) {
        if (d->load == l) {
            d->load = 0;
        }
    }
}

void fn_802CBFCC(TexCache* c, MipLoad* l) {
    l->wait = -1;
    l->res->Load((u8*)l->mem, lbl_8055FCB0[l->tex].mip0Bytes, 0);
    fn_802CB3F8(lbl_8055FCB0[l->tex].name, "LOADING");
}

void fn_802CC03C(TexCache* c, MipLoad* l, s32 id) {
    char buf[32];
    u32 ver;
    l->res = fn_8012C5B8(fn_802CA2A8(buf, 32, lbl_8055FCB0[id].name), 'TEX ', &ver);
    l->tex = id;
    l->gx = fn_802D9154();
    if (!l->res) {
        fn_802E317C("tex: Can't load %s...how did tm0_bytes get set to nonzero?", buf);
    }
    if (ver != 15) {
        fn_802E317C("tex: bad tex version on %s (%d != %d)", buf, ver, 15);
    }
    fn_802CBB9C(&c->loads, &c->freeLoads, l);
}

static inline int fn_802CC10C(TexCache* c, s32 id, s32 frame, MipLoad* l) {
    int ok = 0;
    MipDiscard* d = c->freeDiscards;
    if (d) {
        TexEntry* t = &lbl_8055FCB0[id];
        d->gx = t->mip0;
        d->frame = frame;
        d->load = l;
        d->name = t->name;
        fn_802CBD30(&c->discards, &c->freeDiscards, d);
        t->mip0 = 0;
        c->heapRef->heap->Free(t->mip0Mem);
        t->mip0Mem = 0;
        ok = 1;
        fn_802CB3F8(t->name, "DISCARDING");
    }
    return ok;
}


void TexCacheInflate(s32 k) { fn_802E30C8("texcache: won't inflate cache to %dK", k); }

void TexZHint(s32 id) {
    fn_802E30C8("the z hint is %x", lbl_8055FCB0[id].res->zhint);
    fn_802E317C("%s", "texture.cpp");
}

}  // extern "C"

inline void* SimpleHeap::Alloc(s32 n) {
    void* ret = 0;
    HeapBlock** best = 0;
    s32 bestSize = 0x7FFFFFFF;
    HeapBlock** pp = &free;
    HeapBlock* b;
    n = (n + align) & ~align;
    for (; (b = *pp) != 0; pp = &b->next) {
        if (b->size >= n && bestSize > b->size) {
            best = pp;
            bestSize = b->size;
        }
    }
    if (best) {
        b = *best;
        s32 rest = b->size - n;
        HeapBlock* next = b->next;
        HeapBlock* split = 0;
        if (rest > ((align + 0x10) & ~align) + 0x20) {
            split = (HeapBlock*)((u8*)b + align + n + 1);
            split->magic = 0xABCFEED0;
            split->next = 0;
            split->size = rest - ((align + 0x10) & ~align);
            split->owner = this;
            b->size = n;
        }
        if (split) {
            split->next = next;
            *best = split;
        } else {
            *best = next;
        }
        b->next = used;
        used = b;
        ret = (u8*)b + align + 1;
    }
    if (!ret && strict) {
        fn_802E317C("SimpleHeap: out of memory for %d byte allocation.", n);
    }
    return ret;
}

extern "C" {

SimpleHeap* fn_802CC404(TexCache* c) { return c->heapRef->heap; }

void fn_802CC410(TexCache* c) {
    for (;;) {
        s32 done = -2;
        for (MipLoad* l = c->loads; l; l = l->next) {
            if (l->wait == -1) {
                s32 n;
                TTHeader* h = (TTHeader*)l->res->Poll(&n);
                if (h) {
                    TexEntry* t = &lbl_8055FCB0[l->tex];
                    t->mip0Mem = l->mem;
                    t->mip0 = fn_802CB38C(h, (u32)l->gx, lbl_8055FCB0[l->tex].kind);
                    l->gx = 0;
                    fn_8012C958(l->res);
                    l->res = 0;
                    l->mem = 0;
                    fn_802CBB9C(&c->freeLoads, &c->loads, l);
                    fn_802CB3F8(t->name, "LOADED");
                    c->loadedBytes += t->mip0Bytes;
                    done = l->tex;
                    break;
                }
            }
        }
        if (done == -2) {
            break;
        }
        c->slots[done].state = 2;
    }
}

void fn_802CC518(int w, f32 fov) { lbl_8055FCC4 = 2.0f * fov / w; }

void fn_802CC550(s32 id, f32 dist, f32 scale) {
    if (id == -1) {
        return;
    }
    TexEntry* t = &lbl_8055FCB0[id];
    if (!lbl_8055D07D) {
        return;
    }
    if (!t->mip0Bytes) {
        return;
    }
    f32 d = 1.0f > dist ? 1.0f : dist;
    f32 v = scale * (lbl_8055FCC4 * d);
    t->size = t->size < v ? t->size : v;
}

void fn_802CC5B8(s32 id) {
    if (id != -1) {
        fn_802CC550(id, 1.0f, 0.00001f);
        lbl_8055FCB0[id].uses = 1000000;
    }
}

void fn_802CC60C(void) {
    for (int i = 0; i < 512; i++) {
        lbl_8055FCB0[i].size = 1e12f;
        lbl_8055FCB0[i].uses = 0;
    }
    TexCache* c = lbl_8055FCBC;
    c->best = 0;
    c->resident = 0;
    c->cut = 0;
    if (!(u8)lbl_8055FCC0++) {
        c->startTime = fn_802E2C20();
    }
}

void fn_802CC73C(u8 on) { lbl_8055D07D = on; }

u8 fn_802CC744(void) { return lbl_8055D07D; }

int fn_802CC74C(void) {
    TexCache* c = lbl_8055FCBC;
    bool full = c && !c->freeLoads;
    if (full) {
        return 0;
    }
    return 1;
}

static inline int ScoreLess(const MipSlot* a, const MipSlot* b) { return a->score < b->score; }

static inline void Rank(TexCache* c, MipSlot* s, f32 score) {
    if (s->state == 0 && score < 0.75f && (!c->best || ScoreLess(s, c->best))) {
        c->best = s;
    }
    if (s->state == 2) {
        MipSlot** pp = &c->resident;
        MipSlot* q;
        while ((q = *pp) != 0) {
            if (ScoreLess(q, s)) {
                break;
            }
            pp = &q->next;
        }
        s->next = q;
        *pp = s;
    }
}

void fn_802CC770(void) {
    for (int i = 0; i < 512; i++) {
        TexEntry* t = &lbl_8055FCB0[i];
        int stream = t->mip0Bytes != 0;
        if (t->refs > 0 && stream) {
            if (t->uses > 0) {
                s32 dim = fn_802CB3FC(i);
                TexCache* c = lbl_8055FCBC;
                MipSlot* s = &c->slots[i];
                f32 score = (f32)(dim * 2) * t->size - 1.0f;
                s->score = score;
                if (s->state == 2) {
                    s->frame = c->frame;
                }
                Rank(c, s, score);
            } else {
                TexCache* c = lbl_8055FCBC;
                MipSlot* s = &c->slots[i];
                s->score = 4.0f;
                Rank(c, s, 4.0f);
            }
        }
    }
    if (!lbl_8055D07D) {
        TexCache* c = lbl_8055FCBC;
        for (MipSlot* s = c->resident; s; s = s->next) {
            if (s->state == 2) {
                if (fn_802CC10C(c, s->tex, s->frame + 2, 0)) {
                    s->state = 0;
                }
                break;
            }
        }
    }
    TexCache* c = lbl_8055FCBC;
    fn_802CC410(c);
    fn_802CBDD0(c);
    fn_802CBC3C(c, c->frame);
    c->frame++;
    lbl_8055FCB8 = -2;
}

}  // extern "C"
