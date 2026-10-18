// AI car spacing (0x80139044-0x801390B0): half the current league's car length plus a one-foot margin.
// Names are guesses.
#include "game/common.h"

struct League {
    f32 carWid;  // offset 0x0
    f32 carLen;  // offset 0x4
};

extern "C" League* fn_80137C54(s32 i);

ModuleTag lbl_8055E108;

extern "C" {

void fn_80139044(void) {}

f32 fn_80139048(void) { return 0.305f + fn_80137C54(-1)->carLen * 0.5f; }

}  // extern "C"

static inline u32 MakeILPos(u32 seg, u32 frac) { return (seg << 16) | frac; }

u32 lbl_8055E110 = MakeILPos(0xFFFF, 0);
u32 lbl_8055E114 = MakeILPos(0xFFFF, 0);
