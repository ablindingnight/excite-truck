// Racer slots (0x8026A600-0x8026B118): the four per-racer records built at start-up, the slot
// constructor and the distance-along-axis helper. The large update routine is still a stub. Names
// are guesses.
#include "game/common.h"

extern "C" {
void fn_80265DD8(u8* p);
void fn_80266120(u8* p);
f32 PSVECDotProduct(const f32* a, const f32* b);
}

// One racer's record; its constructor lives in the next unit.
struct RacerSlot {
    u8 active;          // offset 0x0
    u8 data[0x6B];      // offset 0x1
    RacerSlot(void);
};

// A camera record (cammgr.cpp builds four); its constructor is the first function of this unit.
struct CamRec {
    CamRec(void);
    u8 data[0x228];
};

CamRec::CamRec(void) {
    fn_80265DD8(data + 0x160);
    fn_80266120(data + 0x178);
}

ModuleTag lbl_8055F4B0;
RacerSlot lbl_80499E60[4];

extern "C" {

f32 fn_8026A63C(const f32* a, const f32* b, const f32* axis) {
    f32 d[3];
    d[0] = a[0] - b[0];
    d[1] = a[1] - b[1];
    d[2] = a[2] - b[2];
    return PSVECDotProduct(d, axis);
}

void fn_8026A698(u8* p) {}  // stub (650)

u8 fn_8026B0C0(int i) { return lbl_80499E60[i].active; }

}  // extern "C"
