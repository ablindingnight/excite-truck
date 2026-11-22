// A two-function unit (0x801EA4B4-0x801EA4C4): forwarders into another module. Names are guesses.
#include "game/common.h"

extern "C" {
void InitializeCarStateBlocks(const s32* ids, int n);
void fn_80279848(void);
}

ModuleTag lbl_8055EB68;

extern "C" {

void fn_801EA4B4(const s32* ids, int n) { InitializeCarStateBlocks(ids, n); }

void fn_801EA4B8(void) { fn_80279848(); }

}  // extern "C"
