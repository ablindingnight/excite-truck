// Debug log (0x802E3028-0x802E321C): the log file, the debug print (compiled out in this build), and the
// fatal paths (ABEND / PANIC). Names are guesses apart from the strings.
#include "game/common.h"
#include <stdarg.h>

extern "C" {
s32 fn_802E367C(const char* name);  // open file
void fn_802E3734(s32* f);           // close file
void exit(int code);
}

const char* GetBuildString(void);

s32 lbl_8055D178 = -1;
ModuleTag lbl_8055FF08;

extern "C" {

void fn_802E3028(const char* fmt, va_list args, const char* prefix) {}

void fn_802E30C8(const char* fmt, ...);

int fn_802E302C(void) {
    lbl_8055D178 = fn_802E367C("log.log");
    fn_802E30C8("========================================================");
    fn_802E30C8("%s", GetBuildString());
    fn_802E30C8("========================================================");
    return 1;
}

void fn_802E3090(void) {
    if (lbl_8055D178 != -1) {
        fn_802E3734(&lbl_8055D178);
        lbl_8055D178 = -1;
    }
}

void fn_802E30C8(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fn_802E3028(fmt, args, 0);
    va_end(args);
}

void fn_802E3150(const char* msg) {
    fn_802E30C8("ABEND: %s", msg);
    *(volatile int*)0 = 0;
    for (;;) {
    }
}

void fn_802E317C(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fn_802E3028(fmt, args, "PANIC: ");
    fn_802E3150("PANIC");
    exit(-1);
    va_end(args);
}

}  // extern "C"
