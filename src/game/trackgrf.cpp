// Track graph hooks (0x801FC8A8-0x801FC914): loads "track.grf" and forwards the graph's lifecycle calls.
// Names are guesses apart from the file name.
#include "game/common.h"

extern "C" {
void fn_801EBEF4(f32 scale);
void fn_801EB9FC(const char* name);
void fn_801EBBB4(void);
void fn_801EBD70(void);
void fn_801EBDCC(void);
}

ModuleTag lbl_8055EC80;

extern "C" {

void fn_801FC8A8(void) {
    fn_801EBEF4(2.0f);
    fn_801EB9FC("track.grf");
}

void fn_801FC8D8(void) { fn_801EBBB4(); }

int fn_801FC8DC(void) {
    fn_801FC8A8();
    return 1;
}

void fn_801FC900(void) { fn_801FC8D8(); }

void fn_801FC904(void) { fn_801EBD70(); }

void fn_801FC908(void) { fn_801EBDCC(); }

}  // extern "C"
