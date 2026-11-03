// A stripped unit (0x801E78A0-0x801E78D8): 1 empty function and its statics. Names are guesses.
#include "game/common.h"

ModuleTag lbl_8055EAE0;
static inline u32 MakeILPos(u32 seg, u32 frac) { return (seg << 16) | frac; }

extern const u32 lbl_8055EAE8 = MakeILPos(0xFFFF, 0);
u32 lbl_8055EAEC = MakeILPos(0x7F00, 0);

extern "C" {

void fn_801E78A0(void) {}

}  // extern "C"

// NonMatching: the sinit is one of the epilogue-scheduled kind.
