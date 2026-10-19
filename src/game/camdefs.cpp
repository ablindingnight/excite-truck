// A one-function unit (0x8027953C-0x802795E8): resets a 0x64-byte parameter block to its defaults.
// Names are guesses.
#include "game/common.h"

extern "C" void* memset(void* dst, int c, u32 n);

struct ParamBlock {
    u8 on;     // offset 0x0
    u8 pad1[3];
    f32 f[24];  // offset 0x4
};

ModuleTag lbl_8055F608;

extern "C" void fn_8027953C(ParamBlock* p) {
    memset(p, 0, 0x64);
    p->on = 1;
    p->f[0] = 0.0f;
    p->f[1] = 1.0f;
    p->f[2] = 0.0f;
    p->f[3] = 1.0f;
    p->f[4] = 1.0f;
    p->f[5] = 1.0f;
    p->f[6] = 1.0f;
    p->f[7] = 1.0f;
    p->f[8] = 1.0f;
    p->f[9] = 1.0f;
    p->f[10] = 1.0f;
    p->f[11] = 1.0f;
    p->f[12] = 1.0f;
    p->f[13] = 1.0f;
    p->f[14] = 0.0f;
    p->f[15] = 0.0f;
    p->f[16] = 0.0f;
    p->f[17] = 0.0f;
    p->f[18] = 1.0f;
    p->f[19] = 1.0f;
    p->f[20] = 1.0f;
    p->f[21] = 1.0f;
    p->f[22] = 1.0f;
    p->f[23] = 1.0f;
}
