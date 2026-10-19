// Line-drawing texture (0x8029EF70-0x8029F03C): loads and frees "efx_line.tex". Names are guesses apart from
// the strings.
#include "game/common.h"

extern "C" {
void fn_802E30C8(const char* fmt, ...);  // debug print
s32 fn_802CAC28(const char* name, int a);  // load texture
void fn_802CAC54(s32 tex);
}

s32 lbl_8055CAE0 = -1;
ModuleTag lbl_8055F800;
UnkStaticFFFF lbl_8055F808;

extern "C" {

int fn_8029EFF0(s32 tex);

void fn_8029EF70(void) {
    lbl_8055CAE0 = fn_802CAC28("efx_line.tex", 0);
    if (!fn_8029EFF0(lbl_8055CAE0)) {
        fn_802E30C8("drawline: Couldn't get the line texture");
    }
}

void fn_8029EFBC(void) {
    if (fn_8029EFF0(lbl_8055CAE0)) {
        fn_802CAC54(lbl_8055CAE0);
    }
}

int fn_8029EFF0(s32 tex) {
    if (!(tex != -1 && tex != -2)) {
        return 0;
    }
    return 1;
}

}  // extern "C"
