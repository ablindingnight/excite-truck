// String helpers (0x801345D8-0x8013471C, "strfunc:" in its message): a bounded sprintf that warns on
// overflow, and a bounded copy that turns spaces into underscores. Names are guesses.
#include "game/common.h"
#include <stdarg.h>

extern "C" {
int vsprintf(char* s, const char* fmt, va_list args);
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
}

ModuleTag lbl_8055E090;

extern "C" {

void fn_801345D8(char* buf, int size, const char* fmt, ...) {
    va_list args;
    int n;
    va_start(args, fmt);
    n = vsprintf(buf, fmt, args);
    if (n >= size) {
        fn_802E317C("strfunc: buffer overflow (%d > %d)", n + 1, size);
    }
    buf[size - 1] = 0;
}

void fn_801346B4(char* dst, int size, const char* src) {
    int i;
    for (i = 0; i < size && src[i]; i++) {
        dst[i] = src[i] == ' ' ? '_' : src[i];
    }
    dst[size - 1 < i ? size - 1 : i] = 0;
}

}  // extern "C"
