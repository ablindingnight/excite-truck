// Replay playback flags (0x801CEFAC-0x801CF080): whether replays are enabled and whether one is playing.
// Names are guesses apart from the debug messages.
#include "game/common.h"

extern "C" {
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_801AED68(void);
void fn_801AED5C(void);
}

u8 lbl_8055AB50 = 1;
ModuleTag lbl_8055E898;
u8 lbl_8055E8A0;
u8 lbl_8055E8A1;

extern "C" {

void fn_801CEFAC(void) {
    lbl_8055AB50 = 1;
    lbl_8055E8A0 = 0;
    lbl_8055E8A1 = 1;
}

void fn_801CEFC4(void) {
    lbl_8055AB50 = 0;
    lbl_8055E8A0 = 0;
}

void fn_801CEFD4(u8 on) {
    lbl_8055AB50 = on;
    if (on) {
        fn_801AED68();
    } else {
        fn_801AED5C();
    }
}

void fn_801CEFE8(void) {
    fn_802E30C8("CSReplayStartReplayPlayback");
    lbl_8055E8A0 = 1;
}

void fn_801CF01C(void) {
    fn_802E30C8("CSReplayEndReplayPlayback");
    lbl_8055E8A0 = 0;
}

int fn_801CF050(void) { return lbl_8055AB50 != 0; }

int fn_801CF064(void) { return lbl_8055E8A0 != 0; }

}  // extern "C"
