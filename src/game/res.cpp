// Resource sets (0x8012A388-0x8012F150, "res:" in its messages): .res archives of named, typed,
// CRC-checked resources with a table of contents (stored in the set, or in the tocres.res library),
// loaded synchronously or through a background queue, plus double-buffered streaming of single
// resources. Little-endian headers are byte-swapped on load. Names are guesses.
#include "game/common.h"

extern "C" {
int sprintf(char* s, const char* fmt, ...);
int strcasecmp(const char* a, const char* b);
char* strrchr(const char* s, int c);
char* strstr(const char* s, const char* sub);
void* memset(void* p, int c, size_t n);
void* memcpy(void* d, const void* s, size_t n);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
u32 fn_80132E7C(const void* p, s32 n);   // crc
int fn_802E36D8(const char* path);       // open
s32 fn_802E37C4(int fd);                 // file size
void fn_802E3734(int* fd);               // close
int fn_802E39D0(int fd, void* buf, s32 n);  // read, checked
void fn_802E3A14(int fd, s32 pos);       // seek
void fn_802E3804(int fd, void* buf, s32 n);  // read
int fn_802E3BB4(const char* pattern, char* name, s32 n);  // find first
int fn_802E3C24(int h, char* name, s32 n);                // find next
void fn_802E3CA4(int h);                                  // find close
void fn_802E4CA8(char* dst, const char* src, s32 n);
int fn_802E7354(const char* path);
void fn_802E746C(int h, s32* sector, s32* size);
int fn_802E748C(s32 sector, void* buf, s32 size);  // start read
int fn_802E7544(int h);                            // read done
void fn_802E7548(int h);                           // finish read
void fn_8002B0D8(char* s);                         // lowercase
void fn_801282F4(void);
void fn_801282EC(s32 align);
void* fn_801284F0(const char* name, u32 size);  // MemAlloc
void fn_80128660(void* p);                      // MemFree
void fn_80127464(void);
void fn_8012748C(int code);
u32 fn_802E2C20(void);       // time (ms)
void fn_802E2D74(int ms);    // sleep
}

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

class Pool {
public:
    Pool(const char* name, s32 count, s32 size);
    virtual ~Pool(void);
    void* Alloc(void);
    void Free(void* p);

    u8 pad[0x1C];
};

class BgObject {
public:
    virtual ~BgObject(void) {}
    virtual void Update(void) = 0;
};

extern "C" {
void fn_80127CF4(BgObject* obj, const char* name);
void fn_80127E28(BgObject* obj);
}

struct ResEntry;

struct ResData {
    ResEntry* entry;  // offset 0x0
    u32 magic;        // offset 0x4 ('MGI!')
    u8 body[4];       // offset 0x8
};

struct ResEntry {
    char name[0x20];  // offset 0x0
    u32 type;         // offset 0x20
    u32 info;         // offset 0x24
    s32 size;         // offset 0x28
    s32 offset;       // offset 0x2C
    u32 crc;          // offset 0x30
    u32 flags;        // offset 0x34 (1 got, 2 referenced, 4 extracted, 8 resident)
    s32 refs;         // offset 0x38
    ResData* data;    // offset 0x3C
    void* mem;        // offset 0x40
};

struct ResToc {
    u32 version;          // offset 0x0
    u32 size;             // offset 0x4
    u32 createTime;       // offset 0x8
    u32 count;            // offset 0xC
    u32 loadSize;         // offset 0x10
    ResEntry entries[1];  // offset 0x14
};

struct ResSetHeader {
    u32 magic;         // offset 0x0 ('RST0')
    u32 headerSize;    // offset 0x4
    u32 version;       // offset 0x8
    u32 flags;         // offset 0xC (1: TOC in the library)
    u32 fileSize;      // offset 0x10
    u32 createTime;    // offset 0x14
    s32 baseOffset;    // offset 0x18
    u32 unk1C;
    s32 resCount;      // offset 0x20
    s32 loadSize;      // offset 0x24
    u32 unk28;
    u32 tocCrc;        // offset 0x2C
    u32 unk30;
};

class ResourceSet;

ModuleTag lbl_8055DFF0;
u8 lbl_8055DFF8;           // initialised
u8 lbl_8055DFF9;           // a read failed
class TocLib* lbl_8055DFFC;
ResourceSet* lbl_8055E000;  // loaded sets
const char* lbl_80559984 = "\\";

extern "C" {
void fn_8012A46C(void* p, const char* what);
void fn_8012A7FC(void);
void fn_8012A388(ResEntry* e);
void fn_8012A80C(ResSetHeader* h);
int fn_8012A470(ResourceSet* set, int idx, ResEntry* e, u8* base);
int fn_8012AA60(ResourceSet* set, s32 size);
int fn_8012ACA0(ResourceSet* set, u8* base, s32 size);
int fn_8012AC98(const char* name);
void fn_8012BA1C(ResourceSet* set);
void fn_8012BC24(ResourceSet** list, ResourceSet* set);
ResourceSet* fn_8012BD3C(ResourceSet* list, const char* name);
void* fn_8012C370(const char* name, u32 type, u32* info, s32* size, bool* first, bool* got);
int fn_8012C4D8(void* p);
void fn_8012D908(const char* name);
}

static inline void Swap4(u8* p) {
    u8 t = p[0];
    p[0] = p[3];
    p[3] = t;
    t = p[1];
    p[1] = p[2];
    p[2] = t;
}

static inline void FourCC(char* s, u32 v) {
    *(u32*)s = v;
    Swap4((u8*)s);
    s[4] = 0;
}

static inline void StrNCpy(char* dst, const char* src, int len) {
    char* d = dst;
    int n = len;
    const char* s = src;
    while ((*d++ = *s++) != 0) {
        if (--n <= 0) {
            d[-1] = 0;
            break;
        }
    }
}

static inline void StrNCat(char* d, const char* s, int n) {
    while (*d) {
        n--;
        d++;
    }
    if (n > 0) {
        StrNCpy(d, s, n);
    }
}

inline ResToc* GetToc(const char* name) {
    u32 i;
    char buf[0x80];
    bool swap;
    u32 info;
    ResToc* toc;
    StrNCpy(buf, name, 0x80);
    if (*strrchr(buf, '.')) {
        *strrchr(buf, '.') = 0;
    }
    StrNCat(buf, ".toc", 0x80);
    fn_8002B0D8(buf);
    toc = (ResToc*)fn_8012C370(buf, 'ETOC', &info, 0, 0, &swap);
    if (toc && swap) {
        Swap4((u8*)&toc->version);
        Swap4((u8*)&toc->size);
        Swap4((u8*)&toc->createTime);
        Swap4((u8*)&toc->count);
        Swap4((u8*)&toc->loadSize);
        if (toc->version != 0 || toc->size != 0x14) {
            fn_802E317C("res: Bad external toc version/size (%d != %d || %d != %d)", toc->version, 0,
                        toc->size, 0x14);
        }
        for (i = 0; i < toc->count; i++) {
            fn_8012A388(&toc->entries[i]);
        }
    }
    return toc;
}

class ResourceSet {
public:
    ResourceSet(void) { loaded = 0; }
    ResourceSet(const char* n, const char* p, ResSetHeader* h, int f) {
        loaded = 0;
        Init(n, p, h, f);
    }
    ~ResourceSet(void);

    void Init(const char* n, const char* p, ResSetHeader* h, int f) {
        StrNCpy(name, n, sizeof(name));
        StrNCpy(path, p, sizeof(path));
        next = 0;
        entries = 0;
        if (h) {
            hdr = *h;
        } else {
            memset(&hdr, 0, sizeof(hdr));
        }
        refs = 1;
        mem = 0;
        fd = f;
        loaded = 1;
        toc = 0;
        pending = 0;
    }

    void Unload(void) {
        int i;
        for (i = 0; i < hdr.resCount; i++) {
            if (entries[i].flags & 4) {
                fn_802E317C("res: extracted resource memory not freed until set unload. (%s)", &entries[i]);
            }
            entries[i].flags &= ~0xF;
        }
        if (toc) {
            fn_8012C4D8(toc);
        }
        if (mem) {
            fn_80128660(mem);
        }
        loaded = 0;
    }

    char name[0x20];     // offset 0x0
    char path[0x40];     // offset 0x20
    ResEntry* entries;   // offset 0x60
    ResToc* toc;         // offset 0x64
    ResSetHeader hdr;    // offset 0x68
    s32 refs;            // offset 0x9C
    void* mem;           // offset 0xA0
    s32 fd;              // offset 0xA4 (first sector)
    ResourceSet* next;   // offset 0xA8
    u8 loaded;           // offset 0xAC
    u8 pending;          // offset 0xAD
};

extern "C" {
void fn_8012BEF0(const char* name);
void fn_8012BF24(const char* name);
}

class TocLib {
public:
    TocLib(void) { fn_8012BEF0("tocres.res"); }
    ~TocLib(void) { fn_8012BF24("tocres.res"); }
};

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
    u8* Poll(s32* sizeOut);
    void Cancel(void);

    u32 magic;          // offset 0x0
    ResEntry* entry;    // offset 0x4
    ResourceSet* set;   // offset 0x8
    s32 handle;         // offset 0xC
    u8* buf;            // offset 0x10
    s32 size;           // offset 0x14
};

s32 lbl_8055E004;  // AsyncResource count
Pool* lbl_8055E008;

extern "C" {

void fn_8012A388(ResEntry* e) {
    Swap4((u8*)&e->type);
    Swap4((u8*)&e->info);
    Swap4((u8*)&e->size);
    Swap4((u8*)&e->offset);
    Swap4((u8*)&e->crc);
    e->refs = 0;
    e->flags = 0;
    e->info &= 0xFFFF;
}

void fn_8012A46C(void* p, const char* what) {}

int fn_8012A470(ResourceSet* set, int idx, ResEntry* e, u8* base) {
    int ok;
    e->data = (ResData*)(base + e->offset - 8);
    fn_8012A46C(e->data->body, "toc fixup");
    ok = (e->crc - fn_80132E7C(e->data->body, e->size)) == 0;
    if (ok) {
        if (e->flags & 8) {
            fn_802E30C8("*************************************");
            fn_802E30C8("** DID YOU KNOW? Only non-resident **");
            fn_802E30C8("**   resources may be extracted!   **");
            fn_802E30C8("*************************************");
            fn_802E317C("%s[%d]/%s was already resident!", set, idx, e);
        }
        e->flags |= 8;
    } else {
        fn_802E30C8("res: crc failure on %s[%d]/%s", set, idx, e);
    }
    return ok;
}

}  // extern "C"

void* AsyncResource::operator new(size_t size, const char* name) {
    void* p;
    if (lbl_8055E004 == 0) {
        lbl_8055E008 = new ("AsyncResource::pool") Pool("AsyncResource::pool", 32, sizeof(AsyncResource));
    }
    p = lbl_8055E008->Alloc();
    if (p) {
        lbl_8055E004++;
    }
    return p;
}

void AsyncResource::operator delete(void* p) {
    lbl_8055E008->Free(p);
    if (--lbl_8055E004 == 0) {
        delete lbl_8055E008;
        lbl_8055E008 = 0;
    }
}

extern "C" {

void fn_8012A648(void) { lbl_8055E008->Free(AsyncResource::operator new(sizeof(AsyncResource), "baz")); }

void fn_8012A67C(void) {
    lbl_8055E004 = 0;
    delete lbl_8055E008;
    lbl_8055E008 = 0;
}

int fn_8012A6C8(ResSetHeader* h, const char* name, u32 fileSize) {
    u32 magic;
    fn_80132E7C(h, 0x30);
    magic = h->magic;
    if (magic != 'RST0') {
        char s[5];
        FourCC(s, 'RST0');
        fn_802E30C8("res: bad set header on %s: bad cookie '0x%x' != '0x%x'", name, magic, s);
    } else if (h->version != 8) {
        fn_802E30C8("res: bad set header on %s: bad version '%d' != '%d'", name, h->version, 8);
    } else if (h->headerSize != 0x34) {
        fn_802E30C8("res: bad set header on %s: bad header size '%d' != '%d'", name, h->headerSize, 0x34);
    } else if (h->fileSize != fileSize && fileSize != 0) {
        fn_802E30C8("res: bad set header on %s: bad file size '%d' != '%d'", name, h->fileSize, fileSize);
    } else {
        return 1;
    }
    return 0;
}

void fn_8012A7FC(void) {
    lbl_8055DFF9 = 1;
    fn_802E2D74(50);
}

void fn_8012A80C(ResSetHeader* h) {
    Swap4((u8*)&h->magic);
    Swap4((u8*)&h->headerSize);
    Swap4((u8*)&h->version);
    Swap4((u8*)&h->flags);
    Swap4((u8*)&h->fileSize);
    Swap4((u8*)&h->createTime);
    Swap4((u8*)&h->baseOffset);
    Swap4((u8*)&h->unk1C);
    Swap4((u8*)&h->resCount);
    Swap4((u8*)&h->loadSize);
    Swap4((u8*)&h->unk28);
    Swap4((u8*)&h->tocCrc);
    Swap4((u8*)&h->unk30);
}

int fn_8012AA60(ResourceSet* set, s32 size) {
    ResEntry* e = set->entries;
    s32 count = set->hdr.resCount;
    u8* base = (u8*)e;
    u32 crc = set->hdr.tocCrc;
    int ok = crc == fn_80132E7C(e, count * sizeof(ResEntry));
    if (ok) {
        int i;
        for (i = 0; i < count; e++, i++) {
            fn_8012A388(e);
            if (e->offset + e->size <= size && !fn_8012A470(set, i, e, base)) {
                ok = 0;
                break;
            }
        }
    } else {
        fn_802E30C8("res: Bad TOC crc on %s", set);
    }
    return ok;
}

int fn_8012AB34(int fd, ResSetHeader* h, const char* name, u32 fileSize) {
    int ok = 0;
    if (fn_802E39D0(fd, h, sizeof(ResSetHeader))) {
        fn_8012A80C(h);
        ok = fn_8012A6C8(h, name, fileSize);
    }
    return ok;
}

int fn_8012ABB0(const char* path, ResSetHeader* h) {
    int fd = fn_802E36D8(path);
    if (fd != -1 && !fn_8012AB34(fd, h, path, fn_802E37C4(fd))) {
        fn_802E3734(&fd);
        fd = -1;
    }
    return fd;
}

int fn_8012AC28(const char* name, s32* sizeOut) {
    char path[0x104];
    s32 sector = -1;
    s32 size;
    int h;
    fn_802E4CA8(path, name, sizeof(path));
    h = fn_802E7354(path);
    if (h != -1) {
        fn_802E746C(h, &sector, &size);
        if (sizeOut) {
            *sizeOut = size;
        }
    }
    return sector;
}

int fn_8012AC98(const char* name) { return fn_8012AC28(name, 0); }

int fn_8012ACA0(ResourceSet* set, u8* base, s32 size) {
    ResToc* toc;
    int ok;
    ResEntry* e;
    u32 i;
    if (!lbl_8055DFFC) {
        fn_802E317C("res(%s): TOC library unavailable, can't load.", set);
    }
    toc = GetToc(set->name);
    if (!toc) {
        fn_802E317C("res(%s): Library has no TOC for set.", set);
    }
    if (set->hdr.createTime != toc->createTime) {
        fn_802E317C("res(%s): TOC mismatch (create_time) (%d != %d)", set, set->hdr.createTime, toc->createTime);
    }
    if (set->hdr.resCount != toc->count) {
        fn_802E317C("res(%s): TOC mismatch (res_count) (%d != %d)", set, set->hdr.resCount, toc->count);
    }
    if (set->hdr.loadSize != toc->loadSize) {
        fn_802E317C("res(%s): TOC mismatch (load_size) (%d != %d)", set, set->hdr.loadSize, toc->loadSize);
    }
    e = toc->entries;
    ok = 1;
    for (i = 0; i < toc->count; i++, e++) {
        if (e->offset + e->size <= size && !fn_8012A470(set, i, e, base)) {
            ok = 0;
            break;
        }
    }
    set->entries = toc->entries;
    set->toc = toc;
    return ok;
}

ResourceSet* fn_8012AFE0(const char* path, const char* name) {
    ResSetHeader hdr;
    ResourceSet* set = 0;
    int fd = fn_8012ABB0(path, &hdr);
    if (fd != -1) {
        s32 size;
        void* mem;
        int ok;
        set = new ("res:ResourceSetNode") ResourceSet(name, path, &hdr, fn_8012AC98(path));
        size = hdr.loadSize;
        fn_801282F4();
        fn_801282EC(0x80);
        mem = fn_801284F0(name, size);
        fn_802E3A14(fd, set->hdr.baseOffset);
        fn_802E3804(fd, mem, size);
        set->mem = mem;
        if (hdr.flags & 1) {
            set->entries = 0;
            ok = fn_8012ACA0(set, (u8*)mem, size);
        } else {
            set->entries = (ResEntry*)set->mem;
            ok = fn_8012AA60(set, size);
        }
        if (!ok) {
            fn_8012A7FC();
            if (set) {
                if (set->loaded) {
                    set->Unload();
                }
                operator delete(set);
            }
            set = 0;
        }
        fn_802E3734(&fd);
    }
    return set;
}

// Stripped from ET; only their strings remain.
void ResCacheSets(void) {
    fn_802E30C8("tracks,tracks2,");
    fn_802E30C8("car,car2,");
    fn_802E30C8("uicar.res");
}

void ResCacheReport(const char* name, s32 bytes, s32 ms, f32 kps) {
    fn_802E30C8("rescache: %s: copied %d bytes in %d ms (%.1f K/s)", name, bytes, ms, kps);
}

char* fn_8012B2C4(char* buf, const char* name, int i) {
    char* path = 0;
    if (i == 0) {
        sprintf(buf, "%s%s", lbl_80559984, name);
        path = buf;
    }
    return path;
}

char* fn_8012B314(const char* name, char* buf, int i) { return fn_8012B2C4(buf, name, i); }

void fn_8012B324(const char* path) {}

void fn_8012B328(ResourceSet* set) {}

ResourceSet* fn_8012B32C(const char* name) {
    char buf[0x40];
    ResourceSet* set = 0;
    int i;
    char* path;
    fn_802E30C8("res: loading %s", name);
    for (i = 0;; i++) {
        path = fn_8012B314(name, buf, i);
        if (!path) {
            break;
        }
        set = fn_8012AFE0(path, name);
        if (set) {
            fn_8012B328(set);
            break;
        }
        fn_8012B324(path);
    }
    return set;
}

void fn_8012B3D4(char* setName, int setLen, char* resName, int resLen, const char* src) {
    char* p;
    StrNCpy(setName, src, setLen);
    p = strrchr(setName, '/');
    if (p) {
        *p = 0;
        StrNCpy(resName, p + 1, resLen);
    } else {
        *setName = 0;
        StrNCpy(resName, src, resLen);
    }
}

const char* fn_8012B4E4(const char* name, bool* star) {
    *star = *name == '*';
    if (*name == '*') {
        name++;
    }
    return name;
}

int fn_8012B50C(ResEntry* a, ResEntry* b) {
    if ((a->flags & 0x20) && (b->flags & 0x10)) {
        return 1;
    }
    return 0;
}

ResourceSet* fn_8012B534(const char* name, int* idx, int flags) {
    bool star;
    char resName[0x100];
    char setName[0x100];
    int any = flags & 1;
    int quiet = (flags >> 1) & 1;
    ResourceSet* found;
    ResourceSet* set;
    name = fn_8012B4E4(name, &star);
    fn_8012B3D4(setName, sizeof(setName), resName, sizeof(resName), name);
    if (star && !setName[0]) {
        fn_802E30C8("res: *%s is a meaningless resource name.", name);
    }
    *idx = -1;
    found = 0;
    for (set = lbl_8055E000; set; set = set->next) {
        if (!setName[0] || !strcasecmp(set->name, setName)) {
            int i;
            for (i = 0; i < set->hdr.resCount; i++) {
                if (!strcasecmp(set->entries[i].name, resName)) {
                    if (*idx != -1) {
                        fn_8012B50C(&found->entries[*idx], &set->entries[i]);
                    } else {
                        found = set;
                        *idx = i;
                    }
                }
            }
        }
    }
    if (found && found->pending && !quiet) {
        fn_802E317C("res(%s/%s): Get-Before-Wait failure! Resource set still in flight.",
                    found ? found->name : "?", name);
    }
    if (!any && found && !star && !(found->entries[*idx].flags & 8)) {
        found = 0;
    }
    if (!found && !quiet) {
        fn_8012D908(name);
    }
    return found;
}

int fn_8012B6FC(ResourceSet* set, int idx) {
    ResEntry* e = &set->entries[idx];
    if (!(e->flags & 8)) {
        s32 size = e->size + 0x80;
        u8* mem;
        fn_801282F4();
        fn_801282EC(0x80);
        mem = (u8*)fn_801284F0(e->name, size);
        if (mem) {
            u32 start = fn_802E2C20();
            int fd = fn_802E36D8(set->path);
            if (fd != -1) {
                ResData* d = (ResData*)(mem + 0x78);
                d->entry = e;
                d->magic = 'MGI!';
                fn_802E3A14(fd, e->offset + set->hdr.baseOffset);
                fn_802E3804(fd, d->body, e->size);
                if (e->crc == fn_80132E7C(d->body, e->size)) {
                    e->flags |= 0xC;
                    e->data = d;
                    e->mem = mem;
                    fn_8012A46C(d->body, "extracted res");
                    fn_802E30C8("EXTRACTION: LOADED %s/%s %d msec", set, e, fn_802E2C20() - start);
                } else {
                    fn_802E30C8("res: crc failure on extraction: %s", e);
                    fn_8012A7FC();
                }
                fn_802E3734(&fd);
            }
            if (!(e->flags & 8)) {
                fn_80128660(mem);
            }
        }
    }
    return (e->flags >> 3) & 1;
}

void* fn_8012B85C(const char* name, u32 type, u32* info, s32* sizeOut, bool* first, bool* got) {
    int idx;
    void* data = 0;
    ResourceSet* set = fn_8012B534(name, &idx, 0);
    if (set) {
        if (fn_8012B6FC(set, idx)) {
            ResEntry* e = &set->entries[idx];
            if (type == e->type) {
                e->flags |= 2;
                if (got) {
                    u32 f = e->flags;
                    e->flags = f | 1;
                    *got = !(f & 1);
                }
                if (sizeOut) {
                    *sizeOut = e->size;
                }
                if (first) {
                    *first = e->refs == 0;
                }
                if (e->refs++ == 0) {
                    e->data->magic = 'MGI!';
                    e->data->entry = e;
                }
                *info = e->info;
                data = e->data->body;
                fn_8012A46C(data, "ResourceGet");
            } else {
                char want[5];
                char have[5];
                FourCC(want, type);
                FourCC(have, e->type);
                fn_802E30C8("res: ResourceGet(\"%s\"): Expected type '%s', got type '%s'", name, want, have);
            }
        } else {
            fn_802E30C8("res: Can't make %s resident.", name);
        }
    }
    return data;
}

void fn_8012BA1C(ResourceSet* set) {
    int i;
    for (i = 0; i < set->hdr.resCount; i++) {
        ResEntry* e = &set->entries[i];
        if (e->refs > 0) {
            fn_802E30C8("ResourceSetUnload(\"%s\"): \"%s\" still used by %d", set, e, e->refs);
        }
    }
    if (strstr(set->name, ".trk") && strcasecmp(set->name, "uitrack.trk")) {
        for (i = 0; i < set->hdr.resCount; i++) {
            ResEntry* e = &set->entries[i];
            if ((e->flags & 0xA) == 8) {
                fn_802E30C8("res: %s: never referenced %s", set, e);
            }
        }
    }
}

int fn_8012BB18(ResourceSet* set) { return 1; }

void fn_8012BB20(ResourceSet** list, ResourceSet* set) {
    if (fn_8012BB18(set)) {
        fn_8012BA1C(set);
        fn_8012BC24(list, set);
        if (set) {
            if (set->loaded) {
                set->Unload();
            }
            operator delete(set);
        }
    }
}

void fn_8012BC24(ResourceSet** list, ResourceSet* set) {
    ResourceSet* s;
    while ((s = *list) != 0) {
        if (s == set) {
            *list = set->next;
            return;
        }
        list = &s->next;
    }
}

int fn_8012BC50(void* data, void (*cb)(void*, void*), void* arg) {
    ResEntry* e = ((ResData*)((u8*)data - 8))->entry;
    if (!e) {
        return 0;
    }
    if (e->refs) {
        e->refs--;
    } else {
        fn_802E30C8("Someone tried to forget \"%s\" too many times!", e);
    }
    if (e->refs == 0) {
        if (cb) {
            cb(data, arg);
        }
        if (e->flags & 4) {
            fn_80128660(e->mem);
            e->flags &= ~0xC;
            e->mem = 0;
            e->data = 0;
            e->flags &= ~1;
        }
    }
    return e->refs == 0;
}

ResourceSet* fn_8012BD3C(ResourceSet* list, const char* name) {
    ResourceSet* s = list;
    while (s) {
        if (!strcasecmp(name, s->name)) {
            break;
        }
        s = s->next;
    }
    return s;
}

ResourceSet* fn_8012BD98(const char* name, ResourceSet** list) {
    ResourceSet* set = fn_8012BD3C(*list, name);
    if (!set) {
        int tries;
        for (tries = 0; tries < 10; tries++) {
            lbl_8055DFF9 = 0;
            set = fn_8012B32C(name);
            if (set) {
                set->next = *list;
                *list = set;
            } else if (lbl_8055DFF9) {
                fn_802E30C8("get_resource_set(%s): BAD READ, retrying", name);
                fn_80127464();
            }
            if (!lbl_8055DFF9 || set) {
                break;
            }
        }
        if (!set) {
            fn_802E30C8("Can't load resource set \"%s\"", name);
            fn_8012748C(2);
        }
    } else {
        set->refs++;
    }
    return set;
}

void fn_8012A648(void);
void fn_8012DBD0(void);

int fn_8012BE90(void) {
    fn_8012A648();
    lbl_8055DFF8 = 1;
    lbl_8055DFFC = new ("toclib") TocLib;
    fn_8012DBD0();
    return 1;
}

void fn_8012DDA8(const char* name);
void fn_8012E81C(const char* name);
void fn_8012E498(const char* name);

void fn_8012BEF0(const char* name) {
    fn_8012DDA8(name);
    fn_8012E81C(name);
}

void fn_8012BF24(const char* name) { fn_8012E498(name); }

void fn_8012BF28(void) {
    if (lbl_8055DFFC) {
        const char* patterns[] = {"*.res", "*.trk", "*.car"};
        u32 k;
        for (k = 0; k < sizeof(patterns) / sizeof(patterns[0]); k++) {
            char name[0x80];
            int h = fn_802E3BB4(patterns[k], name, sizeof(name));
            if (h != -1) {
                do {
                    int fd;
                    ResToc th;
                    *strrchr(name, '.') = 0;
                    StrNCat(name, ".toc", sizeof(name));
                    fd = fn_802E36D8(name);
                    if (fd != -1) {
                        fn_802E3A14(fd, 0x14);
                        fn_802E3804(fd, &th, 0x14);
                        if (th.version == 0) {
                            ResToc* toc = GetToc(name);
                            if (toc) {
                                if (th.createTime != toc->createTime) {
                                    if (toc->count == th.count) {
                                        fn_802E30C8("res(%s): reloading TOC! ", name);
                                        toc->createTime = th.createTime;
                                        toc->loadSize = th.loadSize;
                                        fn_802E3804(fd, toc->entries, th.count * sizeof(ResEntry));
                                    } else {
                                        fn_802E30C8("res(%s): Can't reload TOC, res_count mismatch! (new %d old %d)",
                                                    name, th.count, toc->count);
                                    }
                                }
                                fn_8012C4D8(toc);
                            } else {
                                fn_802E30C8("reload: no eth for %s", name);
                            }
                        } else {
                            fn_802E30C8("reload: not a set, afaict %s", name);
                        }
                        fn_802E3734(&fd);
                    } else {
                        fn_802E30C8("reload: failed even to open %s", name);
                    }
                } while (fn_802E3C24(h, name, sizeof(name)));
                fn_802E3CA4(h);
            }
        }
    }
}

int fn_8012C33C(const char* name) {
    int idx;
    return fn_8012B534(name, &idx, 0) != 0;
}

void* fn_8012C370(const char* name, u32 type, u32* info, s32* size, bool* first, bool* got) {
    void* data;
    int tries;
    for (tries = 0; tries < 10; tries++) {
        lbl_8055DFF9 = 0;
        data = fn_8012B85C(name, type, info, size, first, got);
        if (!lbl_8055DFF9 || data) {
            break;
        }
        fn_802E30C8("ResourceGet(%s): BAD READ, retrying", name);
        fn_80127464();
    }
    return data;
}

void fn_8012C41C(int (*cb)(ResEntry*, u32, ResourceSet*, void*), void* arg, const char* setName) {
    ResourceSet* set;
    for (set = lbl_8055E000; set; set = set->next) {
        if (!setName || !strcasecmp(set->name, setName)) {
            int i;
            for (i = 0; i < set->hdr.resCount; i++) {
                ResEntry* e = &set->entries[i];
                if ((e->flags & 8) && !cb(e, e->type, set, arg)) {
                    return;
                }
            }
        }
    }
}

int fn_8012C4D8(void* p) { return fn_8012BC50(p, 0, 0); }

int fn_8012C4E4(void* p, void (*cb)(void*, void*), void* arg) { return fn_8012BC50(p, cb, arg); }

void fn_8012DD00(void);

void fn_8012C4E8(void) {
    ResourceSet* s;
    fn_8012DD00();
    fn_8012A67C();
    delete lbl_8055DFFC;
    lbl_8055DFFC = 0;
    lbl_8055DFF8 = 0;
    if ((s = lbl_8055E000) != 0) {
        for (; s; s = s->next) {
            fn_802E30C8("res: set never unloaded: %s with %d refs", s, s->refs);
        }
        fn_802E317C("ResourceEnd: sets remain loaded.");
    }
}

const char* fn_8012C58C(void* data) {
    const char* name = "res:unknown";
    if (data && ((u32*)data)[-1] == 'MGI!') {
        name = ((ResEntry**)data)[-2]->name;
    }
    return name;
}

AsyncResource* fn_8012C5B8(const char* name, u32 type, u32* info) {
    int idx;
    AsyncResource* r = 0;
    ResourceSet* set = fn_8012B534(name, &idx, 1);
    if (set) {
        ResEntry* e = &set->entries[idx];
        if (e->type == type) {
            *info = e->info;
            r = new ("AsyncResource") AsyncResource;
            r->entry = e;
            r->set = set;
        } else {
            char want[5];
            char have[5];
            FourCC(want, type);
            FourCC(have, e->type);
            fn_802E30C8("ResourceGetStream: type mismatch (%s != %s)", want, have);
        }
    }
    return r;
}

}  // extern "C"

s32 AsyncResource::Span(void) {
    s32 start = entry->offset + set->hdr.baseOffset;
    return ((start + entry->size + 0x7FF) & ~0x7FF) - (start & ~0x7FF);
}

void AsyncResource::Load(u8* dst, s32 n, s32 offs) {
    if (!(entry->flags & 8)) {
        s32 pos = set->hdr.baseOffset + (offs + entry->offset);
        handle = fn_802E748C(set->fd + pos / 0x800, dst, n);
        buf = dst + (pos & 0x7FF);
        size = n;
    } else {
        fn_802E30C8("ResourceStreamLoad(%s): resource already resident", entry);
        memcpy(dst, entry->data->body, entry->size);
        buf = dst;
        handle = -1;
        size = n;
    }
}

u8* AsyncResource::Poll(s32* sizeOut) {
    ResEntry* e;
    u8* data = 0;
    if (handle != -1 && fn_802E7544(handle)) {
        fn_802E7548(handle);
        handle = -1;
    }
    if (handle == -1 && buf) {
        e = entry;
        if (size < e->size || fn_80132E7C(buf, e->size) == e->crc) {
            data = buf;
            if (sizeOut) {
                *sizeOut = e->size;
            }
        } else {
            fn_802E30C8("AsyncResource: Bad crc on %s", e);
            Load(buf - ((entry->offset + set->hdr.baseOffset) & 0x7FF), size, 0);
        }
    }
    return data;
}

void AsyncResource::Cancel(void) {
    if (handle != -1) {
        fn_802E7548(handle);
        handle = -1;
        buf = 0;
    }
}

extern "C" void fn_8012C958(AsyncResource* r) {
    if (r->handle != -1) {
        r->Cancel();
    }
    delete r;
}

class AsyncResourceSet {
public:
    AsyncResourceSet(s32 size, const char* name) {
        state = 1;
        handle = -1;
        bufSize = (size + 0x7FF) & ~0x7FF;
        fn_801282F4();
        fn_801282EC(0x80);
        buf = (u8*)fn_801284F0(name, bufSize);
        fileSize = 0;
    }
    ~AsyncResourceSet(void) {
        Update();
        Reset();
        fn_80128660(buf);
        state = 0;
    }
    static void* operator new(size_t size, const char* name);
    static void operator delete(void* p);

    void Stop(void) {
        switch (state) {
        case 2:
            if (handle != -1) {
                fn_802E7548(handle);
                handle = -1;
            }
            set.Unload();
            break;
        case 3:
            fn_8012BA1C(&set);
            fn_8012BC24(&lbl_8055E000, &set);
            set.Unload();
            break;
        }
        state = 1;
    }
    int Begin(const char* name) {
        char path0[0x40];
        int ok = 0;
        do {
            s32 size;
            int sector;
            char* path = fn_8012B314(name, path0, tries);
            if (!path) {
                break;
            }
            ok = 0;
            sector = fn_8012AC28(path, &size);
            if (sector != -1) {
                fileSize = bufSize;
                if (fileSize <= bufSize) {
                    handle = fn_802E748C(sector, buf, fileSize);
                    if (handle != -1) {
                        state = 2;
                        set.Init(name, path, 0, sector);
                        ok = 1;
                        deadline = fn_802E2C20() + fileSize / 3000 + 500;
                    }
                }
            }
            tries++;
        } while (!ok);
        return ok;
    }

    int Restart(const char* name);
    int StartLoad(const char* name);
    void Reset(void);
    static void CopyHeader(ResSetHeader* d, const ResSetHeader* s);
    int Ready(void);
    void Finish(void);
    void Update(void);
    int Status(void);
    int Reload(const char* name);

    s32 deadline;     // offset 0x0
    ResourceSet set;  // offset 0x4
    s32 handle;       // offset 0xB4
    s32 state;        // offset 0xB8
    s32 tries;        // offset 0xBC
    s32 fileSize;     // offset 0xC0
    s32 unkC4;
    u8* buf;          // offset 0xC8
    s32 bufSize;      // offset 0xCC
};

s32 lbl_8055E00C;  // AsyncResourceSet count
Pool* lbl_8055E010;

void* AsyncResourceSet::operator new(size_t size, const char* name) {
    void* p;
    if (lbl_8055E00C == 0) {
        lbl_8055E010 = new ("AsyncResourceSet::pool") Pool("AsyncResourceSet::pool", 32, sizeof(AsyncResourceSet));
    }
    p = lbl_8055E010->Alloc();
    if (p) {
        lbl_8055E00C++;
    }
    return p;
}

void AsyncResourceSet::operator delete(void* p) {
    lbl_8055E010->Free(p);
    if (--lbl_8055E00C == 0) {
        delete lbl_8055E010;
        lbl_8055E010 = 0;
    }
}

extern "C" AsyncResourceSet* fn_8012CAA0(s32 size, const char* name) {
    return new ("arsb") AsyncResourceSet(size, name);
}

ResourceSet::~ResourceSet(void) {
    if (loaded) {
        Unload();
    }
}

extern "C" int fn_8012CC20(AsyncResourceSet* s, const char* name) { return s->Restart(name); }

int AsyncResourceSet::Restart(const char* name) {
    if (state == 2 && fn_802E7544(handle) && Ready()) {
        int ok;
        fn_802E7548(handle);
        handle = -1;
        CopyHeader(&set.hdr, (ResSetHeader*)buf);
        fn_8012A80C(&set.hdr);
        set.mem = 0;
        if (set.hdr.baseOffset + set.hdr.loadSize > bufSize) {
            fn_802E317C("res: resident size exceeded buf! (%d > %d)", set.hdr.baseOffset + set.hdr.loadSize, bufSize);
        }
        if (set.hdr.flags & 1) {
            set.entries = 0;
            ok = fn_8012ACA0(&set, buf + set.hdr.baseOffset, fileSize - set.hdr.baseOffset);
        } else {
            set.entries = (ResEntry*)(buf + set.hdr.baseOffset);
            ok = fn_8012AA60(&set, fileSize - set.hdr.baseOffset);
        }
        if (ok) {
            fn_8012B328(&set);
            set.next = lbl_8055E000;
            lbl_8055E000 = &set;
            state = 3;
        } else {
            Reset();
            fn_8012B324(set.path);
            tries++;
            StartLoad(set.name);
        }
    }
    Stop();
    tries = 0;
    return Begin(name);
}

int AsyncResourceSet::StartLoad(const char* name) { return Begin(name); }

void AsyncResourceSet::Reset(void) { Stop(); }

void AsyncResourceSet::CopyHeader(ResSetHeader* d, const ResSetHeader* s) { *d = *s; }

int AsyncResourceSet::Ready(void) { return 1; }

void AsyncResourceSet::Finish(void) {
    Update();
    Reset();
}

void AsyncResourceSet::Update(void) {
    if (state == 2 && fn_802E7544(handle)) {
        int ok;
        fn_802E7548(handle);
        handle = -1;
        set.hdr = *(ResSetHeader*)buf;
        fn_8012A80C(&set.hdr);
        set.mem = 0;
        if (set.hdr.baseOffset + set.hdr.loadSize > bufSize) {
            fn_802E317C("res: resident size exceeded buf! (%d > %d)", set.hdr.baseOffset + set.hdr.loadSize, bufSize);
        }
        if (set.hdr.flags & 1) {
            set.entries = 0;
            ok = fn_8012ACA0(&set, buf + set.hdr.baseOffset, fileSize - set.hdr.baseOffset);
        } else {
            set.entries = (ResEntry*)(buf + set.hdr.baseOffset);
            ok = fn_8012AA60(&set, fileSize - set.hdr.baseOffset);
        }
        if (ok) {
            fn_8012B328(&set);
            set.next = lbl_8055E000;
            lbl_8055E000 = &set;
            state = 3;
        } else {
            Stop();
            fn_8012B324(set.path);
            tries++;
            Begin(set.name);
        }
    }
}

int AsyncResourceSet::Status(void) {
    Update();
    switch (state) {
    case 1:
        return 0;
    case 2:
        return 1;
    case 3:
        return 2;
    }
    return 0;
}

extern "C" void fn_8012D8A8(AsyncResourceSet* s) { delete s; }

struct QueueNode {
    QueueNode* link;          // offset 0x0 (pool lists)
    QueueNode* next;          // offset 0x4 (pending list)
    s32 refs;                 // offset 0x8
    char name[0x40];          // offset 0xC
    AsyncResourceSet* async;  // offset 0x4C

    void Init(const char* n, s32 size);
    void Free(void);
};

struct NodePool {
    QueueNode* Alloc(void);
    QueueNode* Used(void);
    void Free(QueueNode* n);

    QueueNode* nodes;  // offset 0x0
    s32 cap;           // offset 0x4
    QueueNode* used;   // offset 0x8
    QueueNode* free;   // offset 0xC
};

struct NodeList {
    void Append(QueueNode* n);
    QueueNode* PopFront(void);
    QueueNode* First(void);

    QueueNode* head;   // offset 0x0
    QueueNode** tail;  // offset 0x4
};

struct TocSizer {
    s32 LoadSize(const char* name);
};

class AsyncSetQueue : public BgObject {
public:
    AsyncSetQueue(void) {
        int i;
        pool.cap = 32;
        pool.nodes = storage;
        pool.free = 0;
        pool.used = 0;
        for (i = 0; i < pool.cap - 1; i++) {
            *(QueueNode**)&pool.nodes[i] = &pool.nodes[i + 1];
        }
        *(QueueNode**)&pool.nodes[pool.cap - 1] = 0;
        pool.free = pool.nodes;
        pending.head = 0;
        pending.tail = &pending.head;
        fn_80127CF4(this, "AsyncSetQueue");
        busy = 0;
    }
    virtual ~AsyncSetQueue(void) { fn_80127E28(this); }
    virtual void Update(void);

    void Pump(void) {
        if (!busy) {
            int done;
            busy = 1;
            do {
                QueueNode* n;
                done = 0;
                n = pending.First();
                if (n) {
                    switch (n->async->Status()) {
                    case 0:
                        n->async->Reload(n->name);
                        break;
                    case 1:
                        break;
                    case 2: {
                        ResourceSet* set = fn_8012BD3C(lbl_8055E000, n->name);
                        set->refs = n->refs;
                        set->pending = 1;
                        pending.PopFront();
                        done = 1;
                        break;
                    }
                    }
                }
            } while (done);
            busy = 0;
        }
    }
    void Process(void);
    void Flush(void);
    void Wait(const char* name);
    QueueNode* FindNodeFor(const char* name);

    TocSizer sizer;          // offset 0x4
    NodePool pool;           // offset 0x8
    QueueNode storage[32];   // offset 0x18
    NodeList pending;        // offset 0xA18
    u8 busy;                 // offset 0xA20
};

AsyncSetQueue* lbl_8055E014;

extern "C" void fn_8012D908(const char* name) {
    AsyncSetQueue* q = lbl_8055E014;
    if (q) {
        QueueNode* n = q->pending.head;
        QueueNode* found = 0;
        while (n && !found) {
            ResToc* toc = GetToc(n->name);
            int in = 0;
            u32 i;
            for (i = 0; i < toc->count; i++) {
                if (!strcasecmp(toc->entries[i].name, name)) {
                    in = 1;
                    break;
                }
            }
            if (in) {
                found = n;
            }
            fn_8012C4D8(toc);
            n = n->next;
        }
        if (found) {
            fn_802E317C("res-CRIP(%s/%s): Get-Before-Wait error! Must wait for set.", found->name, name);
        }
    }
}

extern "C" void fn_8012DBD0(void) {
    if (lbl_8055DFFC) {
        lbl_8055E014 = new ("AsyncSetQ") AsyncSetQueue;
    }
}

extern "C" void fn_8012DD00(void) {
    if (lbl_8055E014) {
        delete lbl_8055E014;
        lbl_8055E014 = 0;
    }
}

extern "C" void fn_8012DDA8(const char* name) {
    int found;
    AsyncSetQueue* q;
    if (!fn_8012BD3C(lbl_8055E000, name) && (q = lbl_8055E014) != 0) {
        found = 0;
        QueueNode* n;
        for (n = q->pending.First(); n; n = n->next) {
            if (!strcasecmp(n->name, name)) {
                n->refs++;
                found = 1;
                break;
            }
        }
        if (!found) {
            fn_802E30C8("res: Queueing %s...", name);
            n = q->pool.Alloc();
            if (!n) {
                q->Flush();
                n = q->pool.Alloc();
            }
            if (n) {
                n->Init(name, q->sizer.LoadSize(name));
                q->pending.Append(n);
                q->Process();
                found = 1;
            }
        }
        if (found) {
            return;
        }
    }
    fn_8012BD98(name, &lbl_8055E000);
}

void AsyncSetQueue::Process(void) {
    if (!busy) {
        QueueNode* n;
        int done;
        busy = 1;
        do {
            n = pending.head;
            done = 0;
            if (n) {
                switch (n->async->Status()) {
                case 0:
                    n->async->Restart(n->name);
                    break;
                case 1:
                    break;
                case 2: {
                    ResourceSet* set = fn_8012BD3C(lbl_8055E000, n->name);
                    set->refs = n->refs;
                    set->pending = 1;
                    {
                        QueueNode* h = pending.head;
                        if (h) {
                            pending.head = h->next;
                            if (!pending.head) {
                                pending.tail = &pending.head;
                            }
                            h->next = 0;
                        }
                    }
                    done = 1;
                    break;
                }
                }
            }
        } while (done);
        busy = 0;
    }
}

void NodeList::Append(QueueNode* n) {
    *tail = n;
    tail = &n->next;
    n->next = 0;
}

void QueueNode::Init(const char* n, s32 size) {
    StrNCpy(name, n, sizeof(name));
    async = fn_8012CAA0(size, n);
    refs = 1;
}

s32 TocSizer::LoadSize(const char* name) {
    ResToc* toc = GetToc(name);
    s32 size;
    if (!toc) {
        fn_802E317C("res(%s): No toc entry.", name);
    }
    size = toc->loadSize + 0x80;
    fn_8012C4D8(toc);
    return size;
}

void AsyncSetQueue::Flush(void) {
    QueueNode* n = pending.head;
    if (n) {
        while (n->async->Status() != 2) {
            fn_80127464();
            fn_802E2D74(10);
        }
        Pump();
    }
}

QueueNode* NodeList::PopFront(void) {
    QueueNode* n = head;
    QueueNode* r = 0;
    if (n) {
        r = n;
        head = n->next;
        if (!head) {
            tail = &head;
        }
        n->next = 0;
    }
    return r;
}

int AsyncResourceSet::Reload(const char* name) {
    Update();
    Reset();
    tries = 0;
    return StartLoad(name);
}

QueueNode* NodePool::Alloc(void) {
    QueueNode* n = free;
    QueueNode* r = 0;
    if (n) {
        r = n;
        free = n->link;
        n->link = used;
        used = n;
    }
    return r;
}

QueueNode* NodeList::First(void) { return head; }

extern "C" void fn_8012E498(const char* name) {
    ResourceSet* set;
    fn_8012E81C(name);
    set = fn_8012BD3C(lbl_8055E000, name);
    if (set && --set->refs == 0) {
        AsyncSetQueue* q = lbl_8055E014;
        if (q) {
            int found = 0;
            QueueNode* n;
            for (n = q->pending.First(); n; n = n->next) {
                if (!strcasecmp(n->name, name)) {
                    q->Wait(name);
                    break;
                }
            }
            for (n = q->pending.First(); n; n = n->next) {
                if (!strcasecmp(n->name, name)) {
                    fn_802E317C("res: wtf!  this node should not be in the fifo anymore");
                }
            }
            for (n = q->pool.Used(); n; n = n->link) {
                if (!strcasecmp(n->name, name)) {
                    n->Free();
                    q->pool.Free(n);
                    found = 1;
                    break;
                }
            }
            if (found) {
                return;
            }
        }
        fn_8012BB20(&lbl_8055E000, set);
    }
}

void NodePool::Free(QueueNode* n) {
    int found = 0;
    QueueNode** p = &used;
    while (*p) {
        if (*p == n) {
            *p = *(QueueNode**)*p;
            found = 1;
            break;
        }
        p = (QueueNode**)*p;
    }
    if (found) {
        n->link = free;
        free = n;
    }
}

void QueueNode::Free(void) {
    async->Finish();
    fn_8012D8A8(async);
    async = 0;
    name[0] = 0;
}

QueueNode* NodePool::Used(void) { return used; }

void AsyncSetQueue::Wait(const char* name) {
    QueueNode* n;
    ResourceSet* set;
    for (n = pending.head; n; n = n->next) {
        if (!strcasecmp(name, n->name)) {
            s32 start = fn_802E2C20();
            s32 next = start + 2000;
            while (n->async->Status() != 2) {
                s32 now;
                fn_80127464();
                fn_802E2D74(10);
                now = fn_802E2C20();
                if (now > next) {
                    fn_802E30C8("res: waiting for %s...", n->name);
                    next = now + 2000;
                }
            }
            Pump();
            fn_802E30C8("res: wait(%s) took %d msec", n->name, fn_802E2C20() - start);
            break;
        }
    }
    set = fn_8012BD3C(lbl_8055E000, name);
    if (set) {
        set->pending = 0;
    } else {
        fn_802E30C8("res: Someone was waiting for %s, but it just wasn't pending!", name);
    }
}

extern "C" void fn_8012E81C(const char* name) {
    AsyncSetQueue* q = lbl_8055E014;
    if (q) {
        QueueNode* n;
        ResourceSet* set;
        for (n = q->pending.First(); n; n = n->next) {
            if (!strcasecmp(name, n->name)) {
                s32 start = fn_802E2C20();
                s32 next = start + 2000;
                while (n->async->Status() != 2) {
                    s32 now;
                    fn_80127464();
                    fn_802E2D74(10);
                    now = fn_802E2C20();
                    if (now > next) {
                        fn_802E30C8("res: waiting for %s...", n->name);
                        next = now + 2000;
                    }
                }
                q->Process();
                fn_802E30C8("res: wait(%s) took %d msec", n->name, fn_802E2C20() - start);
                break;
            }
        }
        set = fn_8012BD3C(lbl_8055E000, name);
        if (set) {
            set->pending = 0;
        } else {
            fn_802E30C8("res: Someone was waiting for %s, but it just wasn't pending!", name);
        }
    }
}

extern "C" int fn_8012E938(const char* name) { return fn_8012BD3C(lbl_8055E000, name) != 0; }

extern "C" void fn_8012E96C(const char* name) {
    AsyncSetQueue* q = lbl_8055E014;
    if (q) {
        int idx;
        ResourceSet* set = fn_8012B534(name, &idx, 3);
        if (!set) {
            QueueNode* n = q->FindNodeFor(name);
            if (n) {
                q->Wait(n->name);
            }
        } else {
            q->Wait(set->name);
        }
    }
}

QueueNode* AsyncSetQueue::FindNodeFor(const char* name) {
    QueueNode* n = pending.head;
    QueueNode* found = 0;
    while (n && !found) {
        ResToc* toc = GetToc(n->name);
        int in = 0;
        u32 i;
        for (i = 0; i < toc->count; i++) {
            if (!strcasecmp(toc->entries[i].name, name)) {
                in = 1;
                break;
            }
        }
        if (in) {
            found = n;
        }
        fn_8012C4D8(toc);
        n = n->next;
    }
    return found;
}

struct StreamSlot {
    u8* buf;             // offset 0x0
    AsyncResource* res;  // offset 0x4
    s32 pos;             // offset 0x8
    s32 state;           // offset 0xC (0 idle, 1 loading, 2 ready)
};

struct StreamBuf {
    s32 size;             // offset 0x0
    StreamSlot slots[2];  // offset 0x4
    s32 total;            // offset 0x24
    s32 pos;              // offset 0x28
};

extern "C" void fn_8012EE80(StreamBuf* b);

extern "C" StreamBuf* fn_8012EC90(s32 size) {
    StreamBuf* b = (StreamBuf*)operator new(sizeof(StreamBuf), "resbuf");
    u8* data;
    memset(b, 0, sizeof(StreamBuf));
    b->size = size;
    fn_801282EC(0x80);
    data = new ("resbufb") u8[size * 2];
    b->slots[0].res = 0;
    b->slots[0].buf = data;
    b->slots[0].state = 0;
    b->slots[1].res = 0;
    b->slots[1].buf = data + size;
    b->slots[1].state = 0;
    b->pos = 0;
    b->total = 0;
    return b;
}

extern "C" void fn_8012ED20(StreamBuf* b) {
    StreamSlot* s = b->slots;
    int i;
    for (i = 0; i < 2; i++, s++) {
        if (s->res) {
            s->res->Cancel();
            fn_8012C958(s->res);
            s->res = 0;
        }
    }
    delete[] b->slots[0].buf;
    operator delete(b);
}

extern "C" int fn_8012EDA8(StreamBuf* b, const char* name, u32 type, u32* info) {
    StreamSlot* s;
    StreamSlot* slots = b->slots;
    int j;
    int i;
    for (s = slots, i = 0; i < 2; i++, s++) {
        if (s->res) {
            s->res->Cancel();
            fn_8012C958(s->res);
            s->res = 0;
            s->state = 0;
        }
    }
    b->pos = 0;
    for (j = 0; j < 2; j++, slots++) {
        slots->res = fn_8012C5B8(name, type, info);
        if (!slots->res) {
            return 0;
        }
        b->total = slots->res->Span();
    }
    fn_8012EE80(b);
    return 1;
}

extern "C" s32 fn_8012EE78(StreamBuf* b) { return b->pos; }

extern "C" void fn_8012EE80(StreamBuf* b) {
    int i;
    StreamSlot* s = b->slots;
    s32 end = b->pos;
    for (i = 0; i < 2; i++, s++) {
        if (s->state) {
            if (s->state == 1 && s->res->Poll(0)) {
                s->state = 2;
            }
            if (s->pos + b->size > end) {
                end = s->pos + b->size;
            }
        }
    }
    s = b->slots;
    for (i = 0; i < 2; i++, s++) {
        if ((s->state == 0 && s->res) || (s->state == 2 && s->pos + b->size < b->pos)) {
            s32 start;
            s32 n;
            s->pos = end;
            start = end;
            n = b->size;
            if (start + n > b->total) {
                n = b->total - start;
            }
            end += b->size;
            if (n > 0) {
                s->res->Load(s->buf, n, start);
                s->state = 1;
            } else {
                s->state = 0;
            }
        }
    }
}

extern "C" int fn_8012EFB0(StreamBuf* b, void* dst, s32 n) {
    int i;
    if (b->pos + n >= b->total) {
        return 0;
    }
    for (i = 0; i < 2; i++) {
        StreamSlot* s = &b->slots[i];
        if (s->state == 2 && b->pos >= s->pos && b->pos < s->pos + b->size) {
            memcpy(dst, s->buf + (b->pos - s->pos), n);
            b->pos += n;
            return 1;
        }
    }
    return 0;
}

void AsyncSetQueue::Update(void) { Pump(); }
