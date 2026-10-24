// Menu selection hook (0x80245080-0x802451DC): the memory card dialog's selection check and retry
// counter, installing/removing it as the active dialog and the dialog manager instance. Names are guesses.
#include "game/common.h"
#include "game/mcdialog.h"

extern "C" {
void fn_80236900(void);
void fn_802E8D7C(MemCardDialogMgr* dialog);  // sets the active memory card dialog
}

ModuleTag lbl_8055F238;
UnkStatic123s lbl_80498890;  // menu end tag
UnkStatic7F lbl_8055F240;
MemCardDialogMgr* lbl_8055F244;
static MemCardDialogMgr lbl_804988B0;

extern "C" {

int fn_80245080(u8* p) {
    int r = 0;
    int (*check)(u8* p) = lbl_8055F244->cfg->check;
    if (check && check(p) == 1) {
        r = 1;
    }
    return r;
}

int fn_802450D4(void) {
    fn_80236900();
    MemCardDialogMgr* m = lbl_8055F244;
    if (m->retries > 7) {
        return 1;
    }
    m->retries++;
    return 0;
}

void fn_80245118(void) { fn_802E8D7C(&lbl_804988B0); }

void fn_80245124(void) { fn_802E8D7C(0); }

}  // extern "C"
