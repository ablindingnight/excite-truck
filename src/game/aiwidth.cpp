// AI car spacing (0x80139044-0x801390B0): half the current league's car length plus a one-foot margin.
// Names are guesses.
#include "game/common.h"

#include "game/league.h"
#include "game/makeilpos.h"


ModuleTag lbl_8055E108;

extern "C" {

void fn_80139044(int*) {}

f32 fn_80139048(void) { return 0.305f + fn_80137C54(-1)->carLen * 0.5f; }

}  // extern "C"


u32 lbl_8055E110 = MakeILPos(0xFFFF, 0);
u32 lbl_8055E114 = MakeILPos(0xFFFF, 0);
