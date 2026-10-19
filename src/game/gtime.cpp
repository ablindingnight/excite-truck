// Global game time (0x80236550-0x80236728): tick bookkeeping with pause/resume, a seconds accessor and an
// auto-reset before the float conversion loses precision. Names are guesses apart from the message.
#include "game/common.h"

extern "C" {
void fn_802E30C8(const char* fmt, ...);  // debug print
s32 fn_802E2C20(void);                   // ticks
int fn_802ECC10(void);
}

u8 lbl_8055BA00 = 1;
ModuleTag lbl_8055F088;
static UnkStatic123 lbl_80498638;
UnkStatic7F lbl_8055F090;
s32 lbl_8055F094;  // base tick
s32 lbl_8055F098;  // previous tick
s32 lbl_8055F09C;  // current tick
u8 lbl_8055F0A0;   // paused

extern "C" {

void fn_802365D8(void);
void fn_80236674(void);

void fn_80236550(void) {
    if (lbl_8055F09C - lbl_8055F094 > 10000000) {
        fn_802E30C8("gTime: auto-reset, running out of float precision");
        fn_802365D8();
    }
}

f32 fn_8023659C(void) { return (f32)(lbl_8055F09C - lbl_8055F094) / 1000.0f; }

void fn_802365D8(void) {
    lbl_8055F094 = fn_802E2C20();
    fn_80236674();
}

void fn_80236600(void) { lbl_8055F0A0 = 1; }

void fn_8023660C(void) {
    lbl_8055BA00 = 0;
    if (lbl_8055F0A0) {
        s32 d = fn_802E2C20() - lbl_8055F09C;
        d = d + lbl_8055F094 - 16;
        lbl_8055F094 = d > 1 ? d : 1;
        lbl_8055F0A0 = 0;
    }
    fn_80236674();
}

void fn_80236674(void) {
    if (!lbl_8055F0A0) {
        s32 now;
        fn_80236550();
        now = fn_802E2C20();
        if (fn_802ECC10()) {
            lbl_8055F094 += now - lbl_8055F09C;
        }
        lbl_8055F098 = now;
        lbl_8055F09C = now;
    }
}

}  // extern "C"
