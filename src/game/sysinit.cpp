// System init/shutdown pair (0x80129BD4-0x80129C24): brings two subsystems up and down. Names are guesses.
#include "game/common.h"

extern "C" {
void fn_80129C24(void);
void fn_8012BE90(void);
void fn_8012C4E8(void);
void fn_80129C60(void);
}

ModuleTag lbl_8055DFD8;

extern "C" {

void fn_80129BD4(void) {
    fn_80129C24();
    fn_8012BE90();
}

void fn_80129BF8(void) {
    fn_8012C4E8();
    fn_80129C60();
}

}  // extern "C"
