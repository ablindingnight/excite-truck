// Random numbers (0x80129C24-0x8012A13C): seven independent Marsaglia "RANMAR" generators (the
// 177/169/178/53 seeding and a 97-entry lagged table), with integer and float range helpers.
// Names are guesses.
#include "game/common.h"

struct Ranmar {
    u8 ready;    // offset 0x0
    f32 u[97];   // offset 0x4
    f32 c;       // offset 0x188
    f32 cd;      // offset 0x18C
    f32 cm;      // offset 0x190
    s32 i97;     // offset 0x194
    s32 j97;     // offset 0x198
};

extern "C" {
double fmod(double x, double y);
void fn_802E30C8(const char* fmt, ...);  // debug print
}

int OptDirectToTrack(void);

ModuleTag lbl_8055DFE0;
Ranmar lbl_804223F8[7];

extern "C" {

void fn_80129C64(int index);
void fn_80129E34(Ranmar* r, int ij, int kl);
f32 fn_8012A080(Ranmar* r);

void fn_80129C24(void) {
    int i;
    for (i = 0; i < 7; i++) {
        fn_80129C64(i);
    }
}

void fn_80129C60(void) {}

void fn_80129C64(int index) { fn_80129E34(&lbl_804223F8[index], 23234, 17371); }

void fn_80129C80(int index, int ij, int kl) {
    if (OptDirectToTrack()) {
        ij = 2721883;
        kl = 1727101;
    } else {
        fn_802E30C8("seed[%d]: %d, %d", index, ij, kl);
    }
    fn_80129E34(&lbl_804223F8[index], ij, kl);
}

int fn_80129D18(int index, int n) {
    int r = n * fn_8012A080(&lbl_804223F8[index]);
    return r < n - 1 ? r : n - 1;
}

int fn_80129D90(int index, int lo, int hi) { return lo + fn_80129D18(index, hi - lo); }

f32 fn_80129DC4(int index, f32 lo, f32 hi) {
    f32 range;
    int r = fn_80129D18(index, 100000);
    range = hi - lo;
    return lo + range * (r / 99999.0f);
}

// Stripped from ET; only its constant remains.
f32 RandomZero(void) { return 0.0f; }

void fn_80129E34(Ranmar* r, int ij, int kl) {
    f32 t;
    f32 s;
    int i = (int)(f32)fmod(ij / 177.0f, 177.0) + 2;
    int j = (int)(f32)fmod((f32)ij, 177.0) + 2;
    int k = (int)(f32)fmod(kl / 169.0f, 178.0) + 1;
    int l = (int)(f32)fmod((f32)kl, 169.0);
    int ii;
    int jj;
    for (ii = 0; ii <= 96; ii++) {
        s = 0.0f;
        t = 0.5f;
        for (jj = 0; jj <= 23; jj++) {
            int m = (f32)fmod((f32)fmod((f32)(i * j), 179.0) * k, 179.0);
            i = j;
            j = k;
            k = m;
            l = (f32)fmod(53.0f * l + 1.0f, 169.0);
            if ((f32)fmod((f32)(l * m), 64.0) >= 32.0f) {
                s += t;
            }
            t = 0.5f * t;
        }
        r->u[ii] = s;
    }
    r->c = 362436.0f / 16777216.0f;
    r->cd = 7654321.0f / 16777216.0f;
    r->cm = 16777213.0f / 16777216.0f;
    r->i97 = 96;
    r->j97 = 32;
    r->ready = 1;
}

f32 fn_8012A080(Ranmar* r) {
    f32 uni = r->u[r->i97] - r->u[r->j97];
    if (uni < 0.0f) {
        uni = 1.0f + uni;
    }
    r->u[r->i97] = uni;
    if (--r->i97 < 0) {
        r->i97 = 96;
    }
    if (--r->j97 < 0) {
        r->j97 = 96;
    }
    r->c -= r->cd;
    if (r->c < 0.0f) {
        r->c += r->cm;
    }
    {
        f32 res = uni - r->c;
        if (res < 0.0f) {
            res = 1.0f + res;
        }
        return res;
    }
}

}  // extern "C"
