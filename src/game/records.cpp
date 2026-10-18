// Best-score records (0x80144E38-0x80144F40): a per-level table of 8 scores and the lookup that also
// reports whether a slot holds the level's best. The table pointer and flag are never set by any other
// unit. Names are guesses.
#include "game/common.h"

struct Records {
    u8 pad0[0x1514];
    s32 score[1][8];  // offset 0x1514
};

ModuleTag lbl_8055E1E0;
u8 lbl_8055E1E8;
Records* lbl_8055E1EC;

extern "C" {

s32 fn_80144E38(int i, int level, u8* best) {
    Records* r = lbl_8055E1EC;
    if (best) {
        *best = 1;
    }
    if (best) {
        int k;
        *best = 1;
        for (k = 0; k < 8; k++) {
            if (r->score[level][k] > r->score[level][i]) {
                *best = 0;
            }
        }
    }
    return r->score[level][i];
}

u8 fn_80144F2C(void) { return lbl_8055E1E8; }

s32 fn_80144F34(int i, int level, u8* best) { return fn_80144E38(i, level, best); }

}  // extern "C"
