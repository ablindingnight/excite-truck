// Updatable registry (0x801D0CF0-0x801D0FA4): objects register themselves in a fixed list and get their
// per-frame hooks called in registration order. Names are guesses.
#include "game/common.h"

class Updatable {
public:
    virtual ~Updatable(void);
    virtual void vf0C(void) = 0;
    virtual void vf10(void) = 0;
    virtual void vf14(void) = 0;
    virtual void vf18(void) = 0;
};

static Updatable* lbl_80438818[16];
ModuleTag lbl_8055E8E0;
s32 lbl_8055E8E8;

extern "C" {

void fn_801D0CF0(void) { lbl_8055E8E8 = 0; }

void fn_801D0CFC(void) {
    int i;
    for (i = lbl_8055E8E8 - 1; i >= 0; i--) {
        delete lbl_80438818[i];
    }
    lbl_8055E8E8 = 0;
}

void fn_801D0DFC(void);

}  // extern "C"

Updatable::~Updatable(void) { fn_801D0DFC(); }

extern "C" {

void fn_801D0DDC(Updatable* u) { lbl_80438818[lbl_8055E8E8++] = u; }

void fn_801D0DFC(void) { lbl_8055E8E8--; }

void fn_801D0E0C(void) {
    int i;
    for (i = 0; i < lbl_8055E8E8; i++) {
        lbl_80438818[i]->vf0C();
    }
}

void fn_801D0E70(void) {
    int i;
    for (i = 0; i < lbl_8055E8E8; i++) {
        lbl_80438818[i]->vf18();
    }
}

void fn_801D0ED4(void) {
    int i;
    for (i = 0; i < lbl_8055E8E8; i++) {
        lbl_80438818[i]->vf10();
    }
}

void fn_801D0F38(void) {
    int i;
    for (i = 0; i < lbl_8055E8E8; i++) {
        lbl_80438818[i]->vf14();
    }
}

}  // extern "C"
