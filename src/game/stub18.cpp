// A stripped unit (0x802789A8-0x80278A1C): 3 empty functions and its statics. Names are guesses.
#include "game/common.h"

class UnkStaticS16 {
public:
    UnkStaticS16(s16 v) { val = v; }
    s16 val;
};

inline s16 MakeS16(s16 v) { return v; }

ModuleTag lbl_8055F5D0;
static UnkStatic123 lbl_804A16D0;
s16 lbl_8055F5D8 = MakeS16(0x100);
s16 lbl_8055F5DA = MakeS16(0x405);
s16 lbl_8055F5DC = MakeS16(0x401);
UnkStatic7F lbl_8055F5E0;

extern "C" {

void fn_802789A8(void) {}

void fn_802789AC(void) {}

void fn_802789B0(void) {}

}  // extern "C"

// NonMatching: the sinit is one of the epilogue-scheduled kind.
