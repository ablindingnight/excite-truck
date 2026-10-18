// Event queue (0x80127A24-0x80127B5C): a 16-entry ring of 0x14-byte events filled elsewhere (the
// producers were stripped) and drained here. Names are guesses.
#include "game/common.h"

struct QueuedEvent {
    s32 type;
    s32 a;
    s32 b;
    s32 c;
    s32 d;
};

extern "C" {
void fn_80127464(void);
}

ModuleTag lbl_8055DF48;
QueuedEvent lbl_8041DF58[16];
s32 lbl_8055DF50;  // head
s32 lbl_8055DF54;  // tail
s32 lbl_8055DF58;
s32 lbl_8055DF5C;
s32 lbl_8055DF60;

extern "C" {

int fn_80127AB8(QueuedEvent* out);

void fn_80127A24(void) {
    fn_80127464();
    lbl_8055DF54 = 0;
    lbl_8055DF50 = 0;
}

int fn_80127A50(QueuedEvent* out) {
    fn_80127464();
    return fn_80127AB8(out);
}

void fn_80127A84(s32* a, s32* b, s32* c) {
    *a = lbl_8055DF58;
    *b = lbl_8055DF5C;
    *c = lbl_8055DF60;
}

int fn_80127AA0(void) { return lbl_8055DF50 == lbl_8055DF54; }

int fn_80127AB8(QueuedEvent* out) {
    if (!fn_80127AA0()) {
        *out = lbl_8041DF58[lbl_8055DF50];
        lbl_8055DF50 = (lbl_8055DF50 + 1) % 16;
        return 1;
    }
    return 0;
}

}  // extern "C"
