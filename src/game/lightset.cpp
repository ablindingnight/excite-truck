// Light sets (0x80279370-0x80279464): four sets of four light presets, reset to their defaults.
// Names are guesses.
#include "game/common.h"

struct LightPreset {
    f32 v[3];  // offset 0x0
    f32 w[3];  // offset 0xC
    f32 c;     // offset 0x18
    f32 d;     // offset 0x1C
};

struct LightSet {
    u8 on;                  // offset 0x0
    u8 pad1[0x1F];
    LightPreset preset[4];  // offset 0x20
};

LightSet lbl_804A16E8[4];
ModuleTag lbl_8055F5F8;

extern "C" {

void fn_802793B0(int i);

void fn_80279370(void) {
    int i;
    for (i = 0; i < 4; i++) {
        fn_802793B0(i);
    }
}

void fn_802793AC(void) {}

void fn_802793B0(int i) {
    LightSet* s = &lbl_804A16E8[i];
    int k;
    s->on = 0;
    for (k = 0; k < 4; k++) {
        s->preset[k].v[0] = s->preset[k].v[1] = s->preset[k].v[2] = 287.55554f;
        s->preset[k].w[0] = s->preset[k].w[1] = s->preset[k].w[2] = 1.0f;
        s->preset[k].c = 25.0f;
        s->preset[k].d = 35.0f;
    }
}

}  // extern "C"
