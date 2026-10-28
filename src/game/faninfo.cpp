// Fan info (0x802BE30C-0x802BECE4): per-cell terrain summaries on grids of 4..64 units (each cell's
// dominant material, the materials present and a height), loaded from faninfo.fnf or computed from the
// layer weights and terrain heights. Names are guesses apart from the file name and messages.
#include "game/common.h"

extern "C" {
void* memset(void* dst, int c, size_t n);
double ceil(double x);
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
void fn_8012E96C(const char* name);
void* fn_8012C370(const char* name, u32 tag, u32* version, u32* size, int flags, u8* swapped);
void fn_8012C4D8(void* res);
int fn_802BBD38(short x, short z, int layer);  // layer weight
f32 fn_801DCF48(int a, f32 x, f32 z);         // terrain height
}

void* operator new[](size_t size, const char* tag);

// bits: 0-5 materials present, 6-8 dominant material, 10-13 ?, 14-22 height.
struct FanData {
    ~FanData(void) {
        a = 0;
        b = 0;
        bits = 0;
    }

    u32 a;     // offset 0x0
    u32 b;     // offset 0x4
    u32 bits;  // offset 0x8
};

struct FanCell : FanData {
    ~FanCell(void);
    void Clear(void);
};

ModuleTag lbl_8055FAC0;
u32 lbl_8055FAC8;

u8 lbl_8055FACC;            // loaded from faninfo.fnf
FanCell* lbl_804C06E0[0x41];  // grids by cell size
u32 lbl_804C07E4[3];        // cell outside the map (a FanCell)

static inline void SetMaterial(FanCell* c, u32 mask, int best) {
    c->bits = (((c->bits & ~0x3F) | mask) & ~0x1C0) | ((best << 6) & 0x1C0);
}

extern "C" {
void fn_802BE780(FanCell** grid);
int fn_802BE7C4(int x, int z, int size);
}

FanCell::~FanCell(void) {}

void FanCell::Clear(void) {
    b = 0;
    a = 0;
    bits = 0;
}

extern "C" {

int fn_802BE378(short size) {
    int n = 0x400 / (size * 2);
    return n * n;
}

void fn_802BE38C(void) {
    const char* name = "faninfo.fnf";
    fn_8012E96C(name);
    lbl_8055FACC = 0;
    u32 version;
    u32 size;
    u8 swapped;
    u32* data = (u32*)fn_8012C370(name, 'FINF', &version, &size, 0, &swapped);
    if (data) {
        if (version == 2) {
            u32* rec = data;
            if (swapped) {
                u8* p = (u8*)data;
                for (u32 i = 0; i < size / 4; i++) {
                    u8 t = p[0];
                    p[0] = p[3];
                    p[3] = t;
                    t = p[1];
                    p[1] = p[2];
                    p[2] = t;
                    p += 4;
                }
            }
            while (*rec != 0) {
                int grid = *(s32*)rec;
                int n = rec[1];
                rec += 2;
                for (int i = 0; i < n; i++) {
                    FanCell* c = &lbl_804C06E0[grid][i];
                    c->bits = (c->bits & 0xFF800200) | (*rec++ & 0x7FFDFF);
                }
            }
            lbl_8055FACC = 1;
        } else {
            fn_802E317C("faninfo: Bad finfo ver %d", version);
        }
        fn_8012C4D8(data);
    }
}

void fn_802BE694(void) {
    for (u32 s = 4; s < 0x41; s *= 2) {
        int n = fn_802BE378(s);
        lbl_804C06E0[s] = (FanCell*)new ("FanInfo[]") u8[n * sizeof(FanCell)];
        for (int i = 0; i < n; i++) {
            lbl_804C06E0[s][i].Clear();
        }
    }
    fn_802BE38C();
}

void fn_802BE72C(void) {
    for (u32 s = 4; s < 0x41; s *= 2) {
        fn_802BE780(&lbl_804C06E0[s]);
    }
}

void fn_802BE780(FanCell** grid) {
    if (*grid) {
        delete[] (u8*)*grid;
        *grid = NULL;
    }
}

int fn_802BE7C4(int x, int z, int size) {
    int row = (z - size + 0x200) / (size * 2);
    return (x - size + 0x200) / (size * 2) + row * (0x400 / (size * 2));
}

void fn_802BE7F4(void) {}

void fn_802BE7F8(void) {
    fn_802BE694();
    FanCell* def = (FanCell*)lbl_804C07E4;
    def->a = (u32)&lbl_8055FAC8;
    u32 b = 0x200;
    b |= 0x84;
    b |= 0x800000;
    def->bits = b;
    def->b = 0;
}

void fn_802BE840(void) { fn_802BE72C(); }

FanCell* fn_802BE844(short x, short z, int size) {
    if (x < -0x200 || x >= 0x200 || z < -0x200 || z >= 0x200) {
        return (FanCell*)lbl_804C07E4;
    }
    return &lbl_804C06E0[size][fn_802BE7C4(x, z, size)];
}

void fn_802BE8B4(short x, short z, int size) {
    int k;
    u32 mask = 0;
    int counts[6];
    memset(counts, 0, sizeof(counts));
    if (size == 4) {
        for (short px = x - size; px <= x + size; px += 2) {
            for (short pz = z - size; pz <= z + size; pz += 2) {
                for (int i = 0; i < 6; i++) {
                    int w = fn_802BBD38(px, pz, i);
                    counts[i] += w;
                    if (w) {
                        mask |= 1 << i;
                    }
                }
            }
        }
    } else {
        int h = size / 2;
        for (k = 0; k < 4; k++) {
            short px = x + ((k & 1) ? h : -h);
            short pz = z + ((k & 2) ? h : -h);
            FanCell* c = fn_802BE844(px, pz, h);
            mask |= c->bits & 0x3F;
            counts[(c->bits >> 6) & 7]++;
        }
    }
    int best = 0;
    for (int i = 1; i < 6; i++) {
        if (counts[best] < counts[i]) {
            best = i;
        }
    }
    SetMaterial(fn_802BE844(x, z, size), mask, best);
}

void fn_802BEAB0(void) {
    if (!lbl_8055FACC) {
        for (short s = 4; s <= 0x40; s *= 2) {
            for (short x = s - 0x200; x < 0x200; x += s * 2) {
                for (short z = s - 0x200; z < 0x200; z += s * 2) {
                    fn_802BE844(x, z, s);
                    SetMaterial(fn_802BE844(x, z, s), 2, 1);
                }
            }
        }
    }
}

extern const f32 lbl_80564D50;  // 2.0

void fn_802BEB70(void) {
    if (!lbl_8055FACC) {
        for (short s = 4; s <= 0x40; s *= 2) {
            for (short x = s - 0x200; x < 0x200; x += s * 2) {
                for (short z = s - 0x200; z < 0x200; z += s * 2) {
                    FanCell* c = fn_802BE844(x, z, s);
                    f32 h = fn_801DCF48(0, (f32)x * lbl_80564D50, (f32)z * lbl_80564D50) - -12.0f;
                    int ih = (int)((h < 0.0f) ? h - 0.5f : 0.5f + h);
                    c->bits = (c->bits & ~0x7FC000) | (ih << 14);
                    c->bits = (c->bits & ~0x3C00) | ((int)((f32)ceil(1.3333333333333333) - 1.0f) << 10);
                }
            }
        }
        fn_802BE7F4();
    }
}

}  // extern "C"

// NonMatching: fn_802BE7F8 builds the default cell's bits constant in three steps (li/ori/oris), fn_802BE8B4
// numbers its saved registers differently, and fn_802BEB70 swaps two fmuls operands.
