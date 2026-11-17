// Sound bank queue (0x801292CC-0x801293A4): loads/unloads the race's .bnk sound bank. Names are guesses.
#include "game/common.h"

extern "C" {
int sprintf(char* buf, const char* fmt, ...);
void fn_802E30C8(const char* fmt, ...);  // debug print
s32 fn_802E75E4(const char* name);
void fn_802E75FC(s32 bank);
void fn_802E765C(s32 bank, s32 a, s32 b);
#ifdef __MWERKS__
void fn_802E7684(f32 volume);
#else
/* PORT: MWCC path keeps retail's float argument, see fn_80129374 */
void fn_802E7684(s32 a);  // strmbank.cpp's definition: an int
#endif
}

ModuleTag lbl_8055DFA0;
s32 lbl_8055DFA8;

extern "C" {

void fn_801292CC(const char* name) {
    char buf[32];
    fn_802E30C8("SpuQueueLoadBank!");
    sprintf(buf, "%s.bnk", name);
    lbl_8055DFA8 = fn_802E75E4(buf);
}

void fn_80129324(void*) {
    fn_802E30C8("SpuQueueUnloadBank!");
    fn_802E75FC(lbl_8055DFA8);
    lbl_8055DFA8 = 0;
}

void fn_80129360(s32 a, s32 b) { fn_802E765C(lbl_8055DFA8, a, b); }

void fn_80129374(int mode, int on) {
    f32 v = mode == 1 ? 1.0f : 0.5f;
#ifdef __MWERKS__
    // RETAIL BUG: strmbank's fn_802E7684 takes an int (r3), but retail passes the volume as a float (f1); the callee
    // reads r3, which still holds `mode`. Harmless on PowerPC (separate registers), garbage on x86.
    fn_802E7684(on ? v : 0.0f);
#else
    /* PORT: MWCC path keeps retail's argument registers, see the comment above */
    (void)on;
    (void)v;
    fn_802E7684(mode);  // what the retail callee actually receives
#endif
}

}  // extern "C"
