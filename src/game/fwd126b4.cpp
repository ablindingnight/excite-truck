// Two forwarding wrappers (0x800126B4-0x800126CC); the first ignores its first argument (probably
// a member function's this). Purpose unknown.
#include "game/common.h"

extern "C" {
void fn_801EA4B4(void* p, s32 n);
void fn_801EA4B8(void);
}

struct Unk_800126B4 {
    u8 unk0[0x24];
    s32 unk24;  // offset 0x24
    u8 unk28[4];  // offset 0x28
};

ModuleTag lbl_8055D4F0;

extern "C" void fn_800126B4(void*, Unk_800126B4* obj) {
    fn_801EA4B4(obj->unk28, obj->unk24);
}

extern "C" void fn_800126C0(void) {
    fn_801EA4B8();
}
