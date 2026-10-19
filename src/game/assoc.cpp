// A small association table (0x801D0BE4-0x801D0CF0): up to 16 (key, key) -> pointer entries with lookup,
// insert and remove-by-value. Names are guesses.
#include "game/common.h"

struct AssocEntry {
    s32 a;        // offset 0x0
    s32 b;        // offset 0x4
    void* value;  // offset 0x8
};

static AssocEntry lbl_80438758[16];
ModuleTag lbl_8055E8D0;
s32 lbl_8055E8D8;
s32 lbl_8055E8DC;

extern "C" {

void fn_801D0BE4(void) {
    lbl_8055E8D8 = 0;
    lbl_8055E8DC = 0;
}

void fn_801D0BF4(void) {}

void* fn_801D0BF8(s32 a, s32 b) {
    int i;
    for (i = 0; i < lbl_8055E8D8; i++) {
        if (a == lbl_80438758[i].a && b == lbl_80438758[i].b) {
            return lbl_80438758[i].value;
        }
    }
    return 0;
}

void fn_801D0C58(void* value, s32 a, s32 b) {
    lbl_80438758[lbl_8055E8D8].b = b;
    lbl_80438758[lbl_8055E8D8].a = a;
    lbl_80438758[lbl_8055E8D8].value = value;
    lbl_8055E8D8++;
}

void fn_801D0C84(void* value) {
    int i = 0;
    while (i < lbl_8055E8D8) {
        if (lbl_80438758[i].value == value) {
            lbl_80438758[i] = lbl_80438758[--lbl_8055E8D8];
        } else {
            i++;
        }
    }
}

}  // extern "C"
