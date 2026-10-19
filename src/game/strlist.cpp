// Double-null-terminated string lists (0x8022D0D8-0x8022D228): clear, append and count. Names are guesses.
#include "game/common.h"

extern "C" u32 strlen(const char* s);

ModuleTag lbl_8055EFD0;
static UnkStatic123 lbl_80497DE0;

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

void fn_8022D0D8(char* list) {
    list[1] = 0;
    list[0] = 0;
}

void fn_8022D0E8(char* list0, int size0, const char* s) {
    char* list;
    int size;
    int len;
    list = list0;
    size = size0;
    while (*list != 0) {
        do {
            size--;
        } while (*++list != 0);
        list++;
        size--;
    }
    if (size > 0) {
        StrCopyN(list, s, size);
        len = strlen(list) + 1;
        size -= len;
        if (size >= 1) {
            list[len] = 0;
        } else {
            (list + len)[-2] = 0;
        }
    }
}

int fn_8022D1B4(const char* list) {
    int n = 0;
    do {
        n++;
        while (*list++ != 0) {
        }
    } while (*list != 0);
    return n;
}

}  // extern "C"
