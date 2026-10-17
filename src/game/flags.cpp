// Global game flags word (0x8000D7D8-0x8000D7F0). DirectToTrack (app.cpp) saves and sets bit 0 of
// it around a launch; init clears it. Names are guesses.
#include "game/common.h"

ModuleTag lbl_8055D428;
u32 lbl_8055D430;

void FlagsInit(void) {
    lbl_8055D430 = 0;
}

void FlagsShutdown(void) {}
