// 16K scratch buffer (0x801293B4-0x80129400): a shared save/restore area. Names are guesses.
#include "game/common.h"

extern "C" {
void* memcpy(void* dst, const void* src, size_t n);
}

ModuleTag lbl_8055DFB8;
u8 lbl_8041E180[0x4000];
s32 lbl_8055DFC0;
u8* lbl_8055DFC4;

extern "C" {

void fn_801293B4(void) { lbl_8055DFC4 = lbl_8041E180; }

void fn_801293C4(void) {}

void fn_801293C8(s32 owner) { lbl_8055DFC0 = owner; }

void fn_801293D0(void) { lbl_8055DFC0 = 0; }

void fn_801293DC(void* dst) { memcpy(dst, lbl_8055DFC4, 0x4000); }

void fn_801293E8(const void* src) { memcpy(lbl_8055DFC4, src, 0x4000); }

}  // extern "C"
