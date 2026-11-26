// Fan info (0x802BE30C-0x802BECE4): per-cell terrain summaries on grids of 4..64 units (each cell's
// dominant material, the materials present and a height), loaded from faninfo.fnf or computed from the
// layer weights and terrain heights. Names are guesses apart from the file name and messages.
#include "game/common.h"
#include "game/faninfo.h"
#include "game/ptrint.h"

// World scale 2.0: a named object (first in .sdata2), read by reference so it does not fold.
extern const f32 lbl_80564D50;
const f32 lbl_80564D50 = 2.0f;
static inline const f32& FanScale() { return lbl_80564D50; }

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


void FanCell::Clear(void) {
    head = 0;
    items = 0;
    bits = 0;
}

extern "C" {

int fn_802BE378(short size) {
    int n = 0x400 / (size * 2);
    return n * n;
}

void LoadFanInfoFile(void) {
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
#if defined(__MWERKS__) || defined(GAME_FILE_DATA_NEEDS_SWAP)
            if (swapped) {
#else
            if (0 && swapped) {  // PORT: FINF data is little-endian on disc: already native on a little-endian host
#endif
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
    LoadFanInfoFile();
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

/* Helper name unproven: resets the cell used outside the map. */
static inline void InitDefault(FanCell* def) {
    def->items = (Item**)&lbl_8055FAC8;
    def->head = 0;
    def->bits = 0x200;
    def->bits |= 0x84;
    def->bits |= 0x800000;
}

void fn_802BE7F8(void) {
    fn_802BE694();
    InitDefault((FanCell*)lbl_804C07E4);
}

void fn_802BE840(void) { fn_802BE72C(); }

FanCell* fn_802BE844(short x, short z, int size) {
    if (x < -0x200 || x >= 0x200 || z < -0x200 || z >= 0x200) {
        return (FanCell*)lbl_804C07E4;
    }
    return &lbl_804C06E0[size][fn_802BE7C4(x, z, size)];
}

#pragma push
#pragma opt_dead_assignments off  /* Proven exact here by own scoped ablation; original spelling unproven. */
// PROVISIONAL: accumulate one actual weighted material vote and its presence.
static inline void AccumulateMaterialVote(int& total, int material, int weight, u32& presence) {
    // PROVISIONAL: the original presence-bit type and name are unproven.
    enum MaterialBit { kFirstMaterialBit = 1 };
    MaterialBit materialBit = kFirstMaterialBit;
    total += weight;
    if (weight) {
        presence |= materialBit << material;
    }
}

// PROVISIONAL: one complete child geometry, lookup and material-vote operation.
static inline void AccumulateChildMaterial(short x, short z, int parentSize, int corner,
                                          int (&counts)[6], u32& presence) {
    int childSize = parentSize / 2;
    short px = x + ((corner & 1) ? childSize : -childSize);
    short pz = z + ((corner & 2) ? childSize : -childSize);
    FanCell* cell = fn_802BE844(px, pz, childSize);
    presence |= cell->bits & 0x3F;
    counts[(cell->bits >> 6) & 7]++;
}

// PROVISIONAL: query a real material layer, then apply its weighted vote.
static inline void SampleMaterialLayer(short px, short pz, int layer,
                                      int (&counts)[6], u32& presence) {
    int weight = fn_802BBD38(px, pz, layer);
    AccumulateMaterialVote(counts[layer], layer, weight, presence);
}

void fn_802BE8B4(short x, short z, int size) {
    u32 mask;
    mask = 0;
    int counts[6];
    memset(counts, 0, sizeof(counts));
    if (size == 4) {
        for (short px = x - size; px <= x + size; px += 2) {
            for (short pz = z - size; pz <= z + size; pz += 2) {
                for (int i = 0; i < 6; i++) {
                    SampleMaterialLayer(px, pz, i, counts, mask);
                }
            }
        }
    } else {
        for (int k = 0; k < 4; k++) {
            AccumulateChildMaterial(x, z, size, k, counts, mask);
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
#pragma pop

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

void fn_802BEB70(void) {
    if (!lbl_8055FACC) {
        for (short s = 4; s <= 0x40; s *= 2) {
            for (short x = s - 0x200; x < 0x200; x += s * 2) {
                for (short z = s - 0x200; z < 0x200; z += s * 2) {
                    FanCell* c = fn_802BE844(x, z, s);
                    f32 h = fn_801DCF48(0, (f32)x * FanScale(), (f32)z * FanScale()) - -12.0f;
                    int ih = (int)((h < 0.0f) ? h - 0.5f : 0.5f + h);
                    c->bits = (c->bits & ~0x7FC000) | (ih << 14);
                    c->bits = (c->bits & ~0x3C00) | ((int)((f32)ceil(4.0f / 3.0f) - 1.0f) << 10);
                }
            }
        }
        fn_802BE7F4();
    }
}

}  // extern "C"

// NonMatching: fn_802BE7F8 builds the default cell's bits constant in three steps (li/ori/oris), fn_802BE8B4
// numbers its saved registers differently, and fn_802BEB70 swaps two fmuls operands.
