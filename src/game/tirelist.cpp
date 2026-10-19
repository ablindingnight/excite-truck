// Tire list (0x801AFAC8-0x801AFC28): a pool of 32 tire records allocated at startup and freed at shutdown.
// Names are guesses apart from the "tire_list" allocation tag.
#include "game/common.h"

void* operator new[](size_t size, const char* tag);

struct Tire {
    Tire(void) {}
    u8 pad0[0x50];
    s32 f50;  // offset 0x50
    u8 pad54[0x64 - 0x54];
};

ModuleTag lbl_8055E638;
Tire* lbl_8055E640;

extern "C" {

void fn_801AFAC8(void) {
    int i;
    lbl_8055E640 = new ("tire_list") Tire[32];
    for (i = 0; i < 32; i++) {
        lbl_8055E640[i].f50 = 0;
    }
}

void fn_801AFBE8(void) {
    delete[] lbl_8055E640;
    lbl_8055E640 = 0;
}

}  // extern "C"
