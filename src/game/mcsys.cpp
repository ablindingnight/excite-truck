// Memory card system (0x802E6B50-0x802E72B8): the save file built from registered data chunks (each
// with a cookie, version and size), saved behind a checksummed 'MCSH' header and loaded back only
// if every chunk's format still matches. Names are guesses apart from the messages.
#include "game/common.h"

extern "C" {
void* memset(void* dst, int c, size_t n);
void* memcpy(void* dst, const void* src, size_t n);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
u8* fn_801284E8(const char* tag, u32 size);
void fn_80128658(void* p);
u32 fn_80132E7C(const void* data, s32 len);  // checksum
int fn_802E8D8C(const char* name, void* buf, u32 size);  // read save file
int fn_802E967C(const char* name, void* buf, u32 size);  // write save file
}

// A registered chunk; cb is told 0/1 around loading, 2 when it was not loaded, 3/4 around saving.
struct MemCardChunk {
    u32 cookie;          // offset 0x0
    u32 version;         // offset 0x4
    void* data;          // offset 0x8
    s32 size;            // offset 0xC
    void (*cb)(int ev);  // offset 0x10
};

struct ChunkHeader {
    u32 cookie;   // offset 0x0
    u32 version;  // offset 0x4
    s32 size;     // offset 0x8
};

struct MemCardHeader {
    u8 banner[0x2040];       // offset 0x0
    u32 magic;               // offset 0x2040
    u32 version;             // offset 0x2044
    u32 headerSize;          // offset 0x2048
    s32 dataSize;            // offset 0x204C
    u32 checksum;            // offset 0x2050
    s32 count;               // offset 0x2054
    ChunkHeader chunks[10];  // offset 0x2058
    u8 data[1];              // offset 0x20D0
};

ModuleTag lbl_8055FFB8;
s32 lbl_8055FFC0;  // chunks
u8 lbl_8055FFC4;
u8 lbl_8055FFC5;
char lbl_8055FFC8[8];  // cookie as text
const char* lbl_8055D200[2] = {"foo.bar", NULL};  // save file
MemCardChunk lbl_804FC8B0[10];

extern "C" {

void fn_802E6B50(void) {
    memset(lbl_804FC8B0, 0, sizeof(lbl_804FC8B0));
    lbl_8055FFC0 = 0;
    lbl_8055FFC4 = 1;
    lbl_8055FFC5 = 0;
}

void fn_802E6B94(void) {}

const char* fn_802E6B98(u32 cookie) {
    lbl_8055FFC8[4] = 0;
    memcpy(lbl_8055FFC8, &cookie, 4);
    return lbl_8055FFC8;
}

void fn_802E6BD8(u32 cookie, u32 version, void* data, s32 size, void (*cb)(int)) {
    fn_802E30C8("chunk: %s (%x) %d bytes", fn_802E6B98(cookie), version, size);
    for (int i = 0; i < lbl_8055FFC0; i++) {
        if (cookie == lbl_804FC8B0[i].cookie) {
            fn_802E317C("MemCardSystem: cookie %x already added", cookie);
        }
    }
    MemCardChunk* c = &lbl_804FC8B0[lbl_8055FFC0];
    c->cookie = cookie;
    c->version = version;
    c->data = data;
    c->size = size;
    lbl_8055FFC0++;
    c->cb = cb;
}

void fn_802E6CA4(void) {}

u32 fn_802E7130(void);
int fn_802E721C(u32 cookie, u32 version, s32 size);
int fn_802E6FBC(void);

int fn_802E6CA8(void) {
    u8* p;
    MemCardHeader* h;
    int ok;
    int failed;
    int i;
    s32 dataSize;
    u32 total;
    u8 loaded[10];
    total = fn_802E7130();
    dataSize = total - 0x20D0;
    h = (MemCardHeader*)fn_801284E8("mcs_data", total);
    memset(h, 0, 0x20D0);
    memset((u8*)h + 0x20D0, 0, dataSize);
    for (i = 0; i < 10; i++) {
        loaded[i] = 0;
    }
    ok = 0;
    failed = 0;
    if (!fn_802E8D8C(lbl_8055D200[0], h, total)) {
        fn_802E30C8("MemCardSystemLoad fails");
        failed = 1;
    } else if (h->dataSize != dataSize) {
        fn_802E30C8("memcard: size mismatch %d vs %d", h->dataSize, dataSize);
    } else if (h->magic != 'MCSH' || h->version != 4 || h->headerSize != 0x20D0 ||
               (u32)h->dataSize != (u32)dataSize) {
        fn_802E30C8("MemCardSystemLoad: bad/corrupt header");
        fn_802E30C8("(%x, %x, %d, %d)", h->magic, h->version, h->headerSize, h->dataSize);
        fn_802E30C8("data_size = %d", dataSize);
    } else {
        u32 sum = fn_80132E7C(h->data, h->dataSize);
        if (sum != h->checksum) {
            fn_802E30C8("Memcard: bad checksum (%x != %x)", sum, h->checksum);
        } else {
            ChunkHeader* ch = h->chunks;
            p = h->data;
            ok = 1;
            ChunkHeader* c = ch;
            for (i = 0; i < h->count; c++, i++) {
                if (fn_802E721C(c->cookie, c->version, c->size) < 0) {
                    ok = 0;
                }
            }
            if (ok) {
                for (i = 0; i < h->count; i++, ch++) {
                    int idx = fn_802E721C(ch->cookie, ch->version, ch->size);
                    if (idx >= 0) {
                        loaded[idx] = 1;
                        MemCardChunk* m = &lbl_804FC8B0[idx];
                        if (m->cb) {
                            m->cb(0);
                        }
                        memcpy(m->data, p, m->size);
                        if (m->cb) {
                            m->cb(1);
                        }
                    } else {
                        ok = 0;
                    }
                    p += ch->size;
                }
            }
        }
    }
    fn_80128658(h);
    if (!ok) {
        int missing = 0;
        for (i = 0; i < lbl_8055FFC0; i++) {
            if (!loaded[i]) {
                missing = 1;
            }
        }
        if (missing) {
            for (i = 0; i < lbl_8055FFC0; i++) {
                if (lbl_804FC8B0[i].cb) {
                    lbl_804FC8B0[i].cb(2);
                }
            }
        }
        if (!failed) {
            fn_802E30C8("Loading error...");
        } else {
            fn_802E6FBC();
        }
    }
    return 0;
}

int fn_802E6FBC(void) {
    MemCardChunk* m;
    ChunkHeader* ch;
    int i;
    u32 total;
    MemCardHeader* h;
    u8* p;
    total = fn_802E7130();
    h = (MemCardHeader*)fn_801284E8("mcs_data", total);
    memset(h, 0, 0x20D0);
    h->magic = 'MCSH';
    h->version = 4;
    h->headerSize = 0x20D0;
    h->dataSize = total - 0x20D0;
    h->count = lbl_8055FFC0;
    memset((u8*)h + 0x20D0, 0, total - 0x20D0);
    p = h->data;
    m = lbl_804FC8B0;
    ch = h->chunks;
    for (i = 0; i < lbl_8055FFC0; m++, ch++, i++) {
        ch->cookie = m->cookie;
        ch->version = m->version;
        ch->size = m->size;
        if (m->cb) {
            m->cb(3);
        }
        memcpy(p, m->data, m->size);
        if (m->cb) {
            m->cb(4);
        }
        p += m->size;
    }
    h->checksum = fn_80132E7C(h->data, h->dataSize);
    fn_802E30C8("memcard: writing file (%d blocks, %d bytes)", (s32)(total + 0x1FFF) / 0x2000, total);
    int r = fn_802E967C(lbl_8055D200[0], h, total);
    fn_80128658(h);
    return r;
}

int fn_802E7128(void) { return fn_802E6FBC(); }

void fn_802E712C(void) {}

u32 fn_802E7130(void) {
    s32 total = 0x20D0;
    for (int i = 0; i < lbl_8055FFC0; i++) {
        total += lbl_804FC8B0[i].size;
    }
    return (total + 0x1FFF) & ~0x1FFF;
}

int fn_802E721C(u32 cookie, u32 version, s32 size) {
    MemCardChunk* c = lbl_804FC8B0;
    for (int i = 0; i < lbl_8055FFC0; c++, i++) {
        if (c->cookie == cookie) {
            if (c->version != version || c->size != size) {
                fn_802E30C8("MemCardSystem: bad chunk format for %x", cookie);
                return -1;
            }
            return i;
        }
    }
    return -1;
}

}  // extern "C"

// NonMatching: fn_802E6CA8 numbers its saved registers differently (69 diffs, registers only).
