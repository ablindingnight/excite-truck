// A small unit (0x802E7770-0x802E77B8): runs an optional callback, plus empty hooks. Names are guesses.
#include "game/common.h"

ModuleTag lbl_8055FFF8;

extern "C" {

void fn_802E7770(void) {}

int fn_802E7774(void (*fn)(void)) {
    if (fn) {
        fn();
    }
    return 0;
}

void fn_802E77A8(void) {}

void fn_802E77AC(void) {}

}  // extern "C"
