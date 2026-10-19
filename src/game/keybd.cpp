// Debug keyboard (0x802E46AC-0x802E4C14): a small queue of key presses, the last 32 keys for
// matching key sequences, a screenshot hotkey, and the raw input state. The device reads were
// compiled out, leaving only the bookkeeping. Names are guesses apart from the file name pattern.
#include "game/common.h"

extern "C" {
void* memset(void* dst, int c, size_t n);
int sprintf(char* buf, const char* fmt, ...);
s32 fn_802E2C20(void);                         // milliseconds
int fn_802ECC10(void);                         // keys locked
void fn_802D07E4(const char* name, int full);  // save screenshot
}

// The raw input state.
struct KeyState {
    KeyState(void) {}
    f32 x0, y0, x1, y1, x2, y2, x3, y3;  // offset 0x0
    u8 keys[0x20];   // offset 0x20
    s32 w40;         // offset 0x40
    s32 w44[6];      // offset 0x44
    s32 w5C;         // offset 0x5C
    u8 valid;        // offset 0x60
};

ModuleTag lbl_8055FF48;
u16 lbl_8055FF50[4];   // queued keys
u8 lbl_8055FF58;       // state valid
u16 lbl_8055FF5A;      // last key read
s32 lbl_8055FF5C;      // time of the last key
u16 lbl_8055FF60;      // previous key
s32 lbl_8055FF64;      // screenshot number
s32 lbl_8055D1C0[2] = {-1, 0};  // device channel
KeyState lbl_804FC3B0;
u16 lbl_804FC418[0x20];  // key history
u8 lbl_804FC458[0x100];  // keys held

extern "C" {

void fn_802E47E8(void);
void fn_802E4A2C(void);

void fn_802E46AC(void) {}

void fn_802E46B0(void) {
    lbl_8055FF5C = 0;
    fn_802E47E8();
    lbl_8055D1C0[0] = -1;
    fn_802E46AC();
}

void fn_802E46E4(void) { lbl_8055D1C0[0] = -1; }

u16 fn_802E46F0(void) {
    fn_802E4A2C();
    u16 k = lbl_8055FF50[0];
    lbl_8055FF5A = k;
    u32 code = k & ~0xF000;
    if (code) {
        lbl_8055FF50[0] = lbl_8055FF50[1];
        lbl_8055FF50[1] = lbl_8055FF50[2];
        lbl_8055FF50[2] = lbl_8055FF50[3];
        lbl_8055FF50[3] = 0;
    }
    u16 key = code;
    // Two separate compares in the target (an || folds into a range check).
    if (key == 0x17A) {
        goto shot;
    }
    if (key == 0x17B) {
    shot:
        if (lbl_8055FF60 == 0x112) {
            char name[0x20];
            sprintf(name, "shot%02d.tga", lbl_8055FF64);
            fn_802D07E4(name, key == 0x17B);
            lbl_8055FF64++;
            key = 0;
        }
    }
    if (key) {
        lbl_8055FF60 = key;
    }
    return key;
}

int fn_802E47B8(void) {
    fn_802E4A2C();
    return lbl_8055FF50[0] != 0;
}

void fn_802E47E8(void) { memset(lbl_8055FF50, 0, sizeof(lbl_8055FF50)); }

int fn_802E47F8(void) { return 0; }

void fn_802E4800(u16 key) {
    if (fn_802ECC10()) {
        return;
    }
    lbl_8055FF5C = fn_802E2C20();
    for (int i = 0; i < 0x1F; i++) {
        lbl_804FC418[i] = lbl_804FC418[i + 1];
    }
    lbl_804FC418[0x1F] = key & 0xFFF;
    for (int i = 0; i < 4; i++) {
        if (lbl_8055FF50[i] == 0) {
            lbl_8055FF50[i] = key;
            return;
        }
    }
    lbl_8055FF50[0] = lbl_8055FF50[1];
    lbl_8055FF50[1] = lbl_8055FF50[2];
    lbl_8055FF50[2] = lbl_8055FF50[3];
    lbl_8055FF50[3] = key;
}

int fn_802E499C(const u16* seq) {
    const u16* p = seq;
    int n = 0;
    while (*p != 0) {
        n++;
        p++;
    }
    p = &lbl_804FC418[0x20 - n];
    for (int i = 0; i < n; i++) {
        u16 c = *seq;
        if (*p != c && c != 0xFFFF) {
            return 0;
        }
        seq++;
        p++;
    }
    return 1;
}

u32 fn_802E4A10(void) { return (lbl_8055FF5A >> 12) & 0xF; }

u8 fn_802E4A1C(int key) { return lbl_804FC458[key]; }

void fn_802E4A2C(void) {
    if (lbl_8055D1C0[0] >= 0) {
        u8 report[0x100] = {0};
    }
}

int fn_802E4A60(KeyState* out) {
    if (lbl_8055FF58) {
        *out = lbl_804FC3B0;
        return 1;
    }
    return 0;
}

}  // extern "C"
