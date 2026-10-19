// Stream files (0x802E72B8-0x802E7578): a table of up to 256 named stream files laid out on a virtual
// sector range, with lookup by sector or name and sector reads. Names are guesses apart from the message.
#include "game/common.h"

extern "C" {
void fn_802E30C8(const char* fmt, ...);  // debug print
int strcasecmp(const char* a, const char* b);
s32 fn_802E36D8(const char* name);  // open
void fn_802E3734(s32* f);           // close
s32 fn_802E37C4(s32 f);             // file size
void fn_802E3A14(s32 f, s32 pos);   // seek
void fn_802E3930(s32 f, void* buf, s32 len);  // read
void fn_802E3990(void);
}

struct StrmFile {
    s32 start;  // offset 0x0
    s32 end;    // offset 0x4
    s32 size;   // offset 0x8
    u8 padC[4];
    char name[0x80];  // offset 0x10
};

static StrmFile lbl_804FC978[256];
ModuleTag lbl_8055FFD0;
s32 lbl_8055FFD8;  // count
s32 lbl_8055FFDC;  // sectors used
u8 lbl_8055FFE0;   // open

static inline void StrCopyN(char* dst, const char* src, int len) {
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

extern "C" {

int fn_802E72B8(int bytes) { return (bytes + 0x7FF) / 0x800; }

int fn_802E72C8(s32 sector) {
    int i;
    for (i = 0; i < lbl_8055FFD8; i++) {
        if (sector >= lbl_804FC978[i].start && sector < lbl_804FC978[i].end) {
            return i;
        }
    }
    return -1;
}

void fn_802E7318(void) {
    lbl_8055FFDC = 0;
    lbl_8055FFD8 = 0;
    lbl_8055FFE0 = 1;
}

void fn_802E7330(void) {
    lbl_8055FFE0 = 0;
    fn_802E30C8("StrmEnd: used %d of %d entries, %d sectors", lbl_8055FFD8, 0x100, lbl_8055FFDC);
}

int fn_802E7354(const char* name) {
    int i;
    s32 f;
    int k;
    StrmFile* e;
    s32 size;
    s32 n;
    for (i = 0; i < lbl_8055FFD8; i++) {
        if (strcasecmp(name, lbl_804FC978[i].name) == 0) {
            return i;
        }
    }
    f = fn_802E36D8(name);
    if (f != -1) {
        k = lbl_8055FFD8++;
        e = &lbl_804FC978[k];
        StrCopyN(e->name, name, 0x80);
        size = fn_802E37C4(f);
        n = fn_802E72B8(size);
        e->start = lbl_8055FFDC;
        e->end = lbl_8055FFDC + n;
        lbl_8055FFDC = e->end;
        e->size = size;
        fn_802E3734(&f);
        return k;
    }
    return -1;
}

void fn_802E746C(int idx, s32* start, s32* size) {
    StrmFile* e = &lbl_804FC978[idx];
    *start = e->start;
    *size = e->size;
}

s32 fn_802E748C(s32 sector, void* buf, s32 len) {
    int idx = fn_802E72C8(sector);
    StrmFile* e;
    s32 f;
    s32 off;
    s32 n;
    if (idx < 0) {
        return -1;
    }
    e = &lbl_804FC978[idx];
    f = fn_802E36D8(e->name);
    if (f == -1) {
        return -1;
    }
    off = (sector - e->start) << 11;
    fn_802E3A14(f, off);
    n = (e->size - off + 0x1F) & ~0x1F;
    if (n > len) {
        n = len;
    }
    fn_802E3930(f, buf, n);
    return f;
}

void fn_802E7544(void) { fn_802E3990(); }

void fn_802E7548(s32 f) { fn_802E3734(&f); }

}  // extern "C"
