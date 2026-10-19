// Drawable registry (0x801D0FA4-0x801D1184): Updatables that also draw; they register in a second list
// and get their draw hooks called in order (or reverse order). Names are guesses.
#include "game/common.h"

extern "C" {
void fn_801D0DDC(class Updatable* u);  // register
void fn_801D0DFC(void);                // unregister
}

class Updatable {
public:
    Updatable(void) { fn_801D0DDC(this); }
    virtual ~Updatable(void);
    virtual void vf0C(void) = 0;
    virtual void vf10(void) = 0;
    virtual void vf14(void) = 0;
    virtual void vf18(void) = 0;
};

inline Updatable::~Updatable(void) { fn_801D0DFC(); }

class Drawable;

static Drawable* lbl_80438858[16];
ModuleTag lbl_8055E8F0;
s32 lbl_8055E8F8;
s32 lbl_8055E8FC;

class Drawable : public Updatable {
public:
    Drawable(void);
    virtual ~Drawable(void);
    virtual void vf1C(s32 a) = 0;
    virtual void vf20(s32 a) = 0;
};

Drawable::Drawable(void) { lbl_80438858[lbl_8055E8FC++] = this; }

Drawable::~Drawable(void) { lbl_80438858[--lbl_8055E8FC] = 0; }

extern "C" {

void fn_801D108C(s32 a) {
    int i;
    for (i = 0; i < lbl_8055E8FC; i++) {
        lbl_80438858[i]->vf1C(a);
    }
}

void fn_801D1100(s32 a) {
    int i;
    for (i = lbl_8055E8FC - 1; i >= 0; i--) {
        lbl_80438858[i]->vf20(a);
    }
}

}  // extern "C"
