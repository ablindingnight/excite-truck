// A one-function unit (0x80265DCC-0x80265DD8): forwards to qsort. Names are guesses.
#include "game/common.h"

extern "C" void qsort(void* base, u32 n, u32 size, int (*cmp)(const void*, const void*));

ModuleTag lbl_8055F468;

extern "C" void fn_80265DCC(void* base, u32 n, u32 size, int (*cmp)(const void*, const void*)) {
    qsort(base, n, size, cmp);
}
