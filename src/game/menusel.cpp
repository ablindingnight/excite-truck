// Menu selection hook (0x80245080-0x802451DC): the selection callback check, the retry counter and
// the menu sound-bank selection. The static initialiser is still a stub. Names are guesses.
#include "game/common.h"

#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

typedef int (*SelectCheck)(u8* p);

extern "C" {
void fn_80236900(void);
void fn_802E8D7C(u8* bank);
}

extern u8* lbl_8055F244;
extern u8 lbl_804988B0[];

extern "C" {

int fn_80245080(u8* p) {
    int r = 0;
    SelectCheck check = *(SelectCheck*)(PTR(lbl_8055F244, 0x20) + 0xC);
    if (check && check(p) == 1) {
        r = 1;
    }
    return r;
}

int fn_802450D4(void) {
    fn_80236900();
    u8* m = lbl_8055F244;
    if (S32(m, 0x24) > 7) {
        return 1;
    }
    S32(m, 0x24)++;
    return 0;
}

void fn_80245118(void) { fn_802E8D7C(lbl_804988B0); }

void fn_80245124(void) { fn_802E8D7C(0); }

void fn_8024512C(void) {}  // stub (44)

}  // extern "C"
