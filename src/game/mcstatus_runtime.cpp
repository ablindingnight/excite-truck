// Wireless privacy warning shown by name entry, and memory card status registration.
// Unit boundary reconstructed (warning + startup). Retail source boundary unverified. User-approved 2026-11-17.
#include "game/common.h"
#include "game/joy.h"

extern "C" {
s32 fn_80129AF4(void);
int ReadJoyStatus(JoyStatus* info, int ch);
void fn_80214FD8(const char* title, const char* msg);
}

ModuleTag lbl_8055F248;
UnkStatic123s lbl_804988D8;

extern "C" void fn_8024683C(void) {
    JoyStatus all;
    JoyStatus one;
    int ch = fn_80129AF4();
    int warn = 0;
    if (ch == -1) {
        for (int i = 0; i < 4; i++) {
            if (ReadJoyStatus(&all, i) && (all.info.b & 1)) {
                warn = 1;
            }
        }
    } else {
        if (ReadJoyStatus(&one, ch) && (one.info.b & 1)) {
            warn = 1;
        }
    }
    if (warn) {
        const char* title = "Wireless Warning";
        const char* message = "Any information you enter might be picked up by another wireless receiver";
        fn_80214FD8(title, message);
    }
}
