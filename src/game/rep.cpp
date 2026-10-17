// Registers a 12-byte block under the four-character code 'Rep0' (0x80019864-0x80019898). What
// fn_802E6BD8 does with it is not known yet (a save-data or replay chunk is a guess).
#include "game/common.h"

extern "C" void fn_802E6BD8(u32 code, int, void* buf, int size, int);

ModuleTag lbl_8055D590;
u8 lbl_803582B0[0xC];

void RepInit(void) {
    fn_802E6BD8('Rep0', 0, lbl_803582B0, sizeof(lbl_803582B0), 0);
}

void RepShutdown(void) {}

extern "C" int fn_80019888(void) {
    return 0;
}
