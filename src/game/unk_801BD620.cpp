// Code 0x801BA088-0x801BD628 with its literal pool (.sdata2 0x805627A8-0x80562838) and module tag (.sbss 0x8055E778).
// fn_801BD620 was imported verbatim from the upstream project (github.com/orangedude27/excite-truck, CC0; file
// src/auto/text_801BD620_.c), where it matches exactly. Unit name is provisional.
// 2026-11-02: the unit was widened to its whole original extent. fn_801BA088 (0x3598 bytes, 8 KB stack frame) fills the
// masslist MVec4 table (masslist.cpp calls fn_801BA088(lbl_80436D48, 0, 0xB, 0x2A)); it is a PLACEHOLDER awaiting a
// real body (the target asm is in build/REXE01/asm/auto_03_801BA088_text.s before the split, game/unk_801BD620.s after).
// CLEANUP: automated-decompiler source with raw offsets and placeholder types; rewrite with real types
// before the PC port. Tricks are tagged MATCHING-AID.

typedef float f32;

typedef struct MVec4 {
    f32 x, y, z, w;
} MVec4;

char lbl_8055E778[8];  // the unit's ModuleTag (registered by the static initialiser below)
extern void RegisterModuleTag__FP9ModuleTag(void *queue);

void fn_801BA088(MVec4* t, int a, int b, int c) {
}

void fn_801BD620(void) {
    RegisterModuleTag__FP9ModuleTag(lbl_8055E778);
}
