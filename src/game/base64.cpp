// Base64 decoding (0x8012F7E0-0x8012FAB8) with a URL-ish alphabet ending in ",.": a lazily built
// reverse table, 4-character groups, and whole-string decode. Names are guesses.
#include "game/common.h"

extern "C" {
void* memset(void* p, int c, size_t n);
size_t strlen(const char* s);
}

ModuleTag lbl_8055E030;
char lbl_8032AFE0[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.";
u8 lbl_80422F40[256];

extern "C" {

u8 fn_8012F7E0(char c) {
    static u8 init = 1;
    if (init) {
        int i;
        init = 0;
        memset(lbl_80422F40, 0xFF, sizeof(lbl_80422F40));
        for (i = 0; i < 64; i++) {
            lbl_80422F40[lbl_8032AFE0[i]] = i;
        }
    }
    return lbl_80422F40[c];
}

u32 fn_8012F968(const char* s) {
    const char* p = s;
    u32 v = 0;
    int i;
    for (i = 0; i < 4; i++, p++) {
        u8 d;
        v <<= 6;
        d = fn_8012F7E0(*p);
        if (!(d & 0xC0)) {
            v |= d;
        } else {
            v = 0xFF000000;
            break;
        }
    }
    return v;
}

int fn_8012F9E8(u8* out, s32 size, const char* in) {
    s32 len = strlen(in);
    int ok = 1;
    s32 i;
    s32 n;
    if (len & 3) {
        return 0;
    }
    for (i = 0, n = 0; i < len && n < size; i += 4) {
        u32 v = fn_8012F968(in + i);
        int shift;
        if (!(v & 0xFF000000)) {
            for (shift = 16; n < size && shift >= 0; shift -= 8) {
                out[n++] = v >> shift;
            }
        } else {
            ok = 0;
            break;
        }
    }
    return ok;
}

}  // extern "C"
