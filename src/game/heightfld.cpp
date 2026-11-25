// Track heightfield (0x801DB348-0x801DDE78, heightfld.cpp): the "track.gnd" ground grid - 32x32 tiles of
// 16-bit heights placed on a 96x96 tile index, each with run-length surface materials and a chain of
// weighted displacement layers - sampled with Catmull-Rom splines for height and normal; plus a
// procedural fake heightfield used when there is no track, and deformation records that track how the
// layer weights move a point. Names are guesses apart from the strings.
#include "game/common.h"
#include <revolution/MTX/vec.h>
#include "game/byteswap.h"
#include "game/mathutil.h"
#include "game/round.h"
// World units per height-field cell. A named constant with external linkage, defined before any code: retail keeps
// it as the FIRST .sdata2 entry, separate from the pooled 2.0f literal of the spline helpers (name unproven).
extern const f32 lbl_80562B60;
const f32 lbl_80562B60 = 2.0f;

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

extern "C" {
void* memset(void* p, int c, u32 n);
double floor(double x);
double sqrt(double x);
double sin(double x);
double cos(double x);
}

// GVec: valvec3.h's ValVec3.
// A 32x32 tile of the ground (0x894).
struct GTile {
    f32 base;        // offset 0x0
    f32 scale;       // offset 0x4
    u16 h[32][32];   // offset 0x8
    s32 x;           // offset 0x808
    s32 z;           // offset 0x80C
    u32 rows[32];    // offset 0x810 (material run offsets per z)
    s32 layer;       // offset 0x890
};

// A weighted displacement layer over one tile (0x810).
struct GLayer {
    f32 base;        // offset 0x0
    f32 scale;       // offset 0x4
    u16 h[32][32];   // offset 0x8
    s32 weight;      // offset 0x808
    s32 next;        // offset 0x80C
};

struct GItem {
    u32 w[4];
};

struct GroundHeader {
    f32 offX;        // offset 0x0
    f32 offZ;        // offset 0x4
    f32 scaleX;      // offset 0x8
    f32 scaleZ;      // offset 0xC
    s32 tiled;       // offset 0x10
    s32 numTiles;    // offset 0x14
    GTile* tiles;    // offset 0x18
    s32 numLayers;   // offset 0x1C
    GLayer* layers;  // offset 0x20
    s32 numItems;    // offset 0x24
    GItem* items;    // offset 0x28
    u32 unk2C;       // offset 0x2C
    u8* runs;        // offset 0x30
};

struct FakeField {
    s32 w;       // offset 0x0
    s32 h;       // offset 0x4
    f32* data;   // offset 0x8
    s32 padC;
};

// A point whose height follows the layer weights (0x20).
struct Deform {
    f32* out;        // offset 0x0
    f32 base;        // offset 0x4
    struct {
        s32 weight;  // offset 0x8
        f32 k;       // offset 0xC
    } terms[3];
};

extern "C" {
void* fn_8012C370(const char* name, u32 type, u32* version, u32* size, u8* first, u8* swapped);
void fn_8012C4D8(void* res);
void fn_8012E96C(const char* name);
void LoadSurfaceTypes(void);
void fn_801D049C(void);
void fn_802E30C8(const char* fmt, ...);
void fn_802E317C(const char* fmt, ...);
}

extern f32 __float_epsilon[];  // FLT_EPSILON

f32 lbl_804424B0[75];          // layer weights
f32 lbl_804425DC[75];          // layer weights for the second query
u16 lbl_80442708[96][96];      // tile index (0xFFFF: none)
FakeField lbl_80446F08;
Deform lbl_80446F18[0x2000];

ModuleTag lbl_8055E988;
GroundHeader* lbl_8055E990;
void (*lbl_8055E994)(void*);   // releases the header
s32 lbl_8055E998;
s32 lbl_8055E99C;              // tile origin x
s32 lbl_8055E9A0;              // tile origin z
u8 lbl_8055E9A4;               // loaded
s32 lbl_8055E9A8[2];           // deform count

static inline GTile* TileAt(GroundHeader* g, int tx, int tz) {
    int ix = tx - lbl_8055E99C;
    int iz = tz - lbl_8055E9A0;
    if (ix < 0 || iz < 0) {
        return 0;
    }
    if (ix >= 96 || iz >= 96) {
        return 0;
    }
    u16 i = lbl_80442708[ix][iz];
    if (i == 0xFFFF) {
        return 0;
    }
    return &g->tiles[i];
}

static inline GLayer* LayerAt(s32 i) { return i >= 0 ? &lbl_8055E990->layers[i] : 0; }

extern "C" int fn_801DB348(int x, int z, f32* out, s32* mat, u16* unused);

// TileH: name unproven.
static inline u16 TileH(const GTile* t, int lx, int lz) { return t->h[lx][lz]; }

static inline int Sample(int x, int z, f32* out, s32* mat, u16* unused) {
    if (x < 0 || z < 0) {
        return 0;
    }
    GroundHeader* g = lbl_8055E990;
    if (!g->tiled) {
        return fn_801DB348(x, z, out, mat, unused);
    }
    int tx = x / 32;
    int tz = z / 32;
    int lx = x - tx * 32;
    int lz = z - tz * 32;
    GTile* t = TileAt(g, tx, tz);
    if (!t) {
        return 0;
    }
    f32 h = t->base + t->scale * TileH(t, lx, lz);
    s32 li = t->layer;
    if (li >= 0) {
        GroundHeader* gg = lbl_8055E990;
        for (GLayer* l = &gg->layers[li]; l; l = l->next >= 0 ? &gg->layers[l->next] : 0) {
            f32 v = l->scale * l->h[lx][lz];
            h += lbl_804424B0[l->weight] * (l->base + v);
        }
    }
    u8* r = g->runs + t->rows[lz];
    int n = 0;
    int m;
    for (;;) {
        // PROVISIONAL: run-length local ownership and spelling.
        u8 span = r[1];
        if (n + span > lx) {
            m = r[0];
            break;
        }
        n += span;
        r += 2;
    }
    if (m != 0xFF) {
        *out = h;
        if (mat) {
            *mat = m;
        }
        return 1;
    }
    return 0;
}

// HeightSquare: name unproven.
static inline f32 HeightSquare(const f32& value) { return value * value; }

static inline f32 SplineD(f32 p0, f32 p1, f32 p2, f32 p3, f32 t, f32* deriv) {
    f32 t2 = t * t;
    f32 t3 = Mul(t2, t);
    f32 half = 0.5f;
    f32 m1 = half * (p2 - p0);
    f32 d = p2 - p1;
    f32 m2 = half * (p3 - p1);
    f32 c3 = m1 + m2 - 2.0f * d;
    f32 c2 = 3.0f * d - 2.0f * m1 - m2;
    if (deriv) {
        *deriv = m1 + (3.0f * c3 * t2 + 2.0f * c2 * t);
    }
    return p1 + (m1 * t + (c3 * t3 + c2 * t2));
}

extern "C" {

int fn_801DB348(int x, int z, f32* out, s32* mat, u16* unused) {
    if (mat) {
        *mat = 0;
    }
    x &= lbl_80446F08.w - 1;
    z &= lbl_80446F08.h - 1;
    *out = lbl_80446F08.data[z * lbl_80446F08.w + x];
    return 1;
}

/* Helper name unproven */
static inline f32 SplineP(const f32* p, int s, f32 t) {
    f32 d, t2, t3, p3, p2, p1, p0, k, m1, m2, c3, c2;
    k = 0.5f;
    p2 = p[s * 2]; p0 = p[0]; p1 = p[s]; p3 = p[s * 3];
    d = p2 - p1;
    t2 = t * t;
    t3 = t2 * t;
    m1 = k * (p2 - p0);
    m2 = k * (p3 - p1);
    c3 = m1 + m2 - 2.0f * d;
    c2 = 3.0f * d - 2.0f * m1 - m2;
    return p1 + (m1 * t + (c3 * t3 + c2 * t2));
}

f32 fn_801DB398(const f32 g[4][4], f32 fx, f32 fz) {
    f32 col[4];
    for (int i = 0; i < 4; i++) {
        col[i] = SplineP(&g[0][i], 4, fx);
    }
    return SplineP(col, 1, fz);
}

/* Helper name unproven */
static inline f32 SplinePD(const f32* p, int s, f32 t, f32* deriv) {
    f32 d, t2, t3, p3, p2, p1, p0, k, m1, m2, c3, c2;
    k = 0.5f;
    p2 = p[s * 2]; p0 = p[0]; p1 = p[s]; p3 = p[s * 3];
    d = p2 - p1;
    t2 = HeightSquare(t);
    t3 = Mul(t2, t);
    m1 = k * (p2 - p0);
    m2 = k * (p3 - p1);
    c3 = m1 + m2 - 2.0f * d;
    c2 = 3.0f * d - 2.0f * m1 - m2;
    if (deriv) {
        *deriv = m1 + (2.0f * c2 * t + 3.0f * c3 * t2);
    }
    return p1 + (m1 * t + (c3 * t3 + c2 * t2));
}

f32 fn_801DB514(const f32 g[4][4], ValVec3* normal, f32 fx, f32 fz) {
    f32 col[4];
    f32 dcol[4];
    for (int i = 0; i < 4; i++) {
        f32 p1 = g[1][i];
        f32 p2 = g[2][i];
        f32 p3 = g[3][i];
        col[i] = SplineD(g[0][i], p1, p2, p3, fx, &dcol[i]);
    }
    f32 dz;
    f32 h = SplinePD(col, 1, fz, &dz);
    ValVec3 n;
    f32 n_x = -SplineP(dcol, 1, fz);
    n.y = 1.0f / lbl_8055E990->scaleX;
    n.z = -dz;
    n.x = n_x;
    PSVECNormalize((const Vec*)&n, (Vec*)&n);
    *normal = n;
    return h;
}

int fn_801DB74C(int x, int z, f32* out, s32* mat, u16* unused, ValVec3* normal, f32 fx, f32 fz) {
    f32 h0;
    if (!Sample(x, z, &h0, mat, unused) && !Sample(x + 1, z, &h0, mat, unused) && !Sample(x, z + 1, &h0, mat, unused) &&
        !Sample(x + 1, z + 1, &h0, mat, unused)) {
        return 0;
    }
    f32 g[4][4];
    for (int j = 0; j < 4; j++) {
        int i;
        for (i = 0; i < 4; i++) {
            if (!Sample(x + i - 1, z + j - 1, &g[i][j], 0, 0)) {
                g[i][j] = h0;
            }
        }
    }
    if (normal) {
        *out = fn_801DB514(g, normal, fx, fz);
    } else {
        *out = fn_801DB398(g, fx, fz);
    }
    return 1;
}

int fn_801DC258(f32* out, s32* mat, u16* unused, ValVec3* normal, f32 x, f32 z) {
    GroundHeader* g = lbl_8055E990;
    f32 fx = g->scaleX * (x + g->offX);
    f32 ix;
    f32 fz = g->scaleZ * (z + g->offZ);
    ix = floor(fx);
    f32 iz = floor(fz);
    return fn_801DB74C((int)ix, (int)iz, out, mat, unused, normal, fx - ix, fz - iz);
}

/* The target keeps the 32x32 height swaps as plain loops (no unrolled copies): opt_unroll_loops off
   makes the structure exact; only the row pointer / address registers differ in the two inner loops. */
#pragma push
#pragma opt_unroll_loops off
/* Helper name unproven */
}
template <class T>
static inline T& AtR(T* p, int i) {
    return p[i];
}
extern "C" {

void fn_801DC32C(GroundHeader* g) {
    u32* w = (u32*)g;
    for (int k = 0; k < 13; k++) {
        SwapWord(&w[k]);
    }
    g->tiles = (GTile*)((u8*)g + (u32)g->tiles);
    g->layers = (GLayer*)((u8*)g + (u32)g->layers);
    g->items = (GItem*)((u8*)g + (u32)g->items);
    g->runs = (u8*)g + (u32)g->runs;
    memset(lbl_80442708, -1, sizeof(lbl_80442708));
    lbl_8055E9A0 = 0;
    lbl_8055E99C = 0;
    int minZ = 0;
    int minX = 0;
    int maxX = 0;
    int maxZ = 0;
    int i;
    for (i = 0; i < g->numTiles; i++) {
        GTile* t = &g->tiles[i];
        SwapWord(&t->base);
        SwapWord(&t->scale);
        for (int a = 0; a < 32; a++) {
            for (int b = 0; b < 32; b++) {
                SwapHalfV(&AtR(t->h, b)[a]);
            }
        }
        SwapWord(&t->x);
        SwapWord(&t->z);
        for (int r = 0; r < 32; r++) {
            SwapWord(&t->rows[r]);
        }
        SwapWord(&t->layer);
        if (i == 0) {
            lbl_8055E99C = minX = maxX = t->x;
            lbl_8055E9A0 = minZ = maxZ = t->z;
        } else {
            minX = lbl_8055E99C = t->x < lbl_8055E99C ? t->x : lbl_8055E99C;
            minZ = lbl_8055E9A0 = t->z < lbl_8055E9A0 ? t->z : lbl_8055E9A0;
            maxX = t->x > maxX ? t->x : maxX;
            maxZ = t->z > maxZ ? t->z : maxZ;
        }
    }
    fn_802E30C8("ground: grid %d,%d - %d,%d (%d x %d)", minX, minZ, maxX, maxZ, maxX - minX, maxZ - minZ);
    for (i = 0; i < g->numTiles; i++) {
        GTile* t = &g->tiles[i];
        if (t->x < lbl_8055E99C || t->z < lbl_8055E9A0 || t->x - lbl_8055E99C >= 96 || t->z - lbl_8055E9A0 >= 96) {
            fn_802E317C("ground: out of range tile %d, %d (%d x %d)", t->x - lbl_8055E99C, t->z - lbl_8055E9A0, 96, 96);
        }
        lbl_80442708[t->x - lbl_8055E99C][t->z - lbl_8055E9A0] = i;
    }
    for (i = 0; i < g->numLayers; i++) {
        GLayer* l = &g->layers[i];
        SwapWord(&l->base);
        SwapWord(&l->scale);
        for (int a = 0; a < 32; a++) {
            for (int b = 0; b < 32; b++) {
                SwapHalfV(&AtR(l->h, b)[a]);
            }
        }
        SwapWord(&l->weight);
        SwapWord(&l->next);
    }
    for (i = 0; i < g->numItems; i++) {
        GItem* it = &g->items[i];
        for (int k = 0; k < 4; k++) {
            SwapWord(&it->w[k]);
        }
    }
}
#pragma pop

GroundHeader* LoadGroundHeader(void) {
    u32 ver = 0;
    u8 swapped;
    const char* name = "track.gnd";
    fn_8012E96C(name);
    GroundHeader* g = (GroundHeader*)fn_8012C370(name, 'Grnd', &ver, 0, 0, &swapped);
    if (g) {
        if (ver != 0) {
            fn_802E317C("ground: version mismatch on %s (%x != %x)", name, ver, 0);
        }
        if (swapped) {
            fn_801DC32C(g);
        }
    }
    return g;
}

void fn_801DCA5C(void* p) { fn_8012C4D8(p); }


static inline f32 SqI(int a) { return (f32)a * (f32)a; }

GroundHeader* GenerateHeightField(void) {
    lbl_80446F08.h = 256;
    lbl_80446F08.w = 256;
    int w;
    GroundHeader* g = (GroundHeader*)operator new(sizeof(GroundHeader), "fake-GroundHeader");
    memset(g, 0, sizeof(GroundHeader));
    int z;
    f32 size = 1024.0f;
    size *= lbl_80562B60;
    g->offZ = g->offX = size / 2.0f;
    g->scaleX = g->scaleZ = lbl_80446F08.w / size;
    lbl_80446F08.data = new ("heightfield") f32[lbl_80446F08.w * lbl_80446F08.h];
    for (z = 0; z < lbl_80446F08.w; z++) {
        int dz = z - 128;
        for (int x = 0; x < lbl_80446F08.h; x++) {
            int dx = x - 128;
            // Stand-in: the sum lives in some local struct's field (frsp on its first forwarded read); the real struct is unknown.
            struct { f32 v; } h;
            h.v = 0.0f;
            f32 k = 1.0f;
            f32 r2 = SqI(dz) + SqI(dx);
            if (r2 > 1314.0625f && r2 < 2025.0f) {
                f32 r = sqrt(r2);
                if (r > 40.625f) {
                    r = 40.625f - (r - 40.625f);
                }
                k = LerpT(MaxF(MinF(1.0f, (r - 36.25f) / 1.875f), 0.0f), 1.0f, 0.8f);
            }

            f32 a = 15.0f;
            w = lbl_80446F08.w;
            for (int n = w / 2; n > 16; n /= 2) {
                h.v = h.v + a * (1.0f + (f32)sin(6.2831855f * ((f32)(z * 2) / n)));
                h.v = h.v + a * (1.0f + (f32)cos(6.2831855f * ((f32)(x * 2) / n)));
                a = a * (0.6f * k);
            }
            lbl_80446F08.data[w * x + z] = h.v;
        }
    }
    return g;
}

void fn_801DCD24(void* p) { operator delete(p); }

void load_ground(int fake) {
    lbl_8055E998 = 0;
    if (!fake) {
        lbl_8055E990 = LoadGroundHeader();
        fn_802E30C8("load_ground: %p", lbl_8055E990);
        lbl_8055E994 = fn_801DCA5C;
    } else {
        lbl_8055E990 = GenerateHeightField();
        lbl_8055E994 = fn_801DCD24;
    }
    lbl_8055E9A4 = 1;
    LoadSurfaceTypes();
}

void fn_801DCDA0(void) {}

void fn_801DCDA4(void) {
    if (lbl_80446F08.data) {
        delete[] lbl_80446F08.data;
        lbl_80446F08.data = 0;
    }
    lbl_8055E994(lbl_8055E990);
    lbl_8055E994 = 0;
    lbl_8055E990 = 0;
    lbl_8055E9A4 = 0;
    fn_801D049C();
}

void fn_801DCE0C(int fake) { load_ground(fake); }

void fn_801DCE10(void) {}

int fn_801DCE14(u16* flags, const ValVec3* in, ValVec3* out, ValVec3* normal, s32* mat) {
    if (flags) {
        *flags = 0;
    }
    f32 h;
    s32 m = 0xFF;
    f32 x = in->x;
    f32 z = in->z;
    if (fn_801DC258(&h, &m, flags, normal, x, z)) {
        if (mat) {
            *mat = m;
        }
        f32 inZ = in->z;
        out->y = h;
        out->x = in->x;
        out->z = inZ;
        return 1;
    }
    return 0;
}

f32 fn_801DCEC0(s16, f32, f32) { return 1.0f; }  // carbody passes (hint id, x, z); retail ignores them

void fn_801DCEC8(int i, f32 w) { lbl_804424B0[i] = w; }

void fn_801DCEDC(int i, f32 w) { lbl_804425DC[i] = w; }

void fn_801DCEF0(int) {}

void fn_801DCEF4(void) { lbl_8055E9A8[0] = 0; }

void fn_801DCF00(void) { lbl_8055E9A8[0] = 0; }

f32 fn_801DCF0C(f32 x, f32 z) {
    f32 h = -1000.0f;
    fn_801DC258(&h, 0, 0, 0, x, z);
    return h;
}


/* Helper name unproven: height and optional normal inside one tile; the boundary is needed (in place -19.0) */
static inline f32 TileHeight(GroundHeader* g, int tx, int tz, ValVec3* normal, int lx, int lz) {
    f32 h;
    GTile* t = TileAt(g, tx, tz);
    if (!t) {
        if (normal) {
            normal->x = 0.0f;
            normal->y = 1.0f;
            normal->z = 0.0f;
        }
        return 0.0f;
    }
    if (normal) {
        int ox = lx ^ 1;
        int oz = lz ^ 1;
        h = t->base + t->scale * TileH(t, lx, lz);
        f32 hz;
        f32 hx = t->base + t->scale * TileH(t, ox, lz);
        hz = t->base + t->scale * TileH(t, lx, oz);
        if (t->layer >= 0) {
            GroundHeader* gg = lbl_8055E990;
            for (GLayer* l = &gg->layers[t->layer]; l; l = l->next >= 0 ? &gg->layers[l->next] : 0) {
                f32 w = lbl_804424B0[l->weight];
                h += w * (l->base + l->scale * l->h[lx][lz]);
                hx += w * (l->base + l->scale * l->h[ox][lz]);
                hz += w * (l->base + l->scale * l->h[lx][oz]);
            }
        }
        hz -= h;
        hx -= h;
        f32 sx = (lx & 1 ? -1.0f : 1.0f) / g->scaleX;
        f32 sz = (lz & 1 ? -1.0f : 1.0f) / g->scaleZ;
        normal->Set(-sx * hx, __fabs(sx * sz), -hz * sz);
        PSVECNormalize((const Vec*)normal, (Vec*)normal);
        return h;
    }
    h = t->base + t->scale * TileH(t, lx, lz);
    if (t->layer >= 0) {
        GroundHeader* gg = lbl_8055E990;
        for (GLayer* l = &gg->layers[t->layer]; l; l = l->next >= 0 ? &gg->layers[l->next] : 0) {
            f32 v = l->scale * l->h[lx][lz];
            h += lbl_804424B0[l->weight] * (l->base + v);
        }
    }
    return h;
}

f32 fn_801DCF48(ValVec3* normal, f32 x, f32 z) {
    GroundHeader* g = lbl_8055E990;
    f32 h;
    if (!g->tiled) {
        h = 0.0f;
        if (!fn_801DC258(&h, 0, 0, normal, x, z)) {
            h = 0.0f;
            if (normal) {
                normal->x = 0.0f;
                normal->y = 1.0f;
                normal->z = 0.0f;
            }
        }
        return h;
    }
    f32 fx = g->scaleX * (x + g->offX);
    f32 fz = g->scaleZ * (z + g->offZ);
    int ix = Round(fx);
    int iz = Round(fz);
    int tx = ix / 32;
    int tz = iz / 32;
    int lz = iz - tz * 32;
    int lx = ix - tx * 32;
    return TileHeight(g, tx, tz, normal, lx, lz);
}


void fn_801DD3BC(f32* out, int fixed, f32 x, f32 z) {
    Deform* d = &lbl_80446F18[lbl_8055E9A8[0]++];
    d->out = out;
    f32 base = fn_801DCF0C(x, z);
    f32 h0;
    f32 saved[75];
    for (int i = 0; i < 75; i++) {
        saved[i] = lbl_804424B0[i];
        lbl_804424B0[i] = 0.0f;
    }
    h0 = fn_801DCF0C(x, z);
    if (!fixed) {
        base = h0;
    }
    int n = 0;
    d->base = h0 + (*out - base);
    d->terms[0].weight = 0;
    d->terms[0].k = 0.0f;
    d->terms[1].weight = 0;
    d->terms[1].k = 0.0f;
    d->terms[2].weight = 0;
    d->terms[2].k = 0.0f;
    for (int i = 0; i < 75; i++) {
        int k;
        f32 m;
        f32 dh;
        int j;
        lbl_804424B0[i] = 1.0f;
        dh = fn_801DCF0C(x, z) - h0;
        if (!(FAbs(dh) <= __float_epsilon[0])) {
            if (n >= 3) {
                k = -1;
                m = 0.0f;
                for (j = 0; j < 3; j++) {
                    if (k < 0 || (float)__fabs(d->terms[j].k) < (float)__fabs(m)) {
                        k = j;
                        m = d->terms[j].k;
                    }
                }
                if (FAbs(d->terms[k].k) < FAbs(dh)) {
                    d->terms[k].k = dh;
                    d->terms[k].weight = i;
                }
            } else if (n < 3) {
                d->terms[n].weight = i;
                n++;
                d->terms[n - 1].k = dh;
            }
        }
        lbl_804424B0[i] = 0.0f;
    }
    for (int i = 0; i < 75; i++) {
        lbl_804424B0[i] = saved[i];
    }
    int any = 0;
    for (int i = 0; i < 3; i++) {
        if (!((float)__fabs(d->terms[i].k) <= __float_epsilon[0])) {
            any = 1;
        }
    }
    if (!any) {
        lbl_8055E9A8[0]--;
    }
}

void fn_801DD808(void) {
    Deform* d = lbl_80446F18;
    for (int i = 0; i < lbl_8055E9A8[0]; i++, d++) {
        f32 v = d->base;
        for (int j = 0; j < 3; j++) {
            v += d->terms[j].k * lbl_804424B0[d->terms[j].weight];
        }
        *d->out = v;
    }
}

f32 fn_801DD88C(f32 x, f32 z) {
    f32 saved[75];
    int i;
    for (i = 0; i < 75; i++) {
        saved[i] = lbl_804424B0[i];
        lbl_804424B0[i] = 0.0f;
    }
    f32 h = fn_801DCF0C(x, z);
    for (i = 0; i < 75; i++) {
        lbl_804424B0[i] = saved[i];
    }
    return h;
}

int fn_801DDA84(void* q, f32 x, f32 z, s32* out, f32 thr, int max) {
    GroundHeader* g = lbl_8055E990;
    if (!g->tiled) {
        return 0;
    }
    f32 fx = g->scaleX * (x + g->offX);
    f32 fz = g->scaleZ * (z + g->offZ);
    int ix = Round(fx);
    int iz = Round(fz);
    if (ix < 0 || iz < 0) {
        return 0;
    }
    int tx = ix / 32;
    int tz = iz / 32;
    int lx = ix - tx * 32;
    int n;
    int lz = iz - tz * 32;
    GTile* t = TileAt(g, tx, tz);
    if (!t) {
        return 0;
    }
    n = 0;
    s32 li = t->layer;
    if (li >= 0) {
        GLayer* l = &lbl_8055E990->layers[li];
        for (;;) {
            if ((f32)__fabs(l->base + l->scale * l->h[lx][lz]) > thr && n < max) {
                out[n++] = l->weight;
            }
            if (l->next < 0) {
                break;
            }
            l = &lbl_8055E990->layers[l->next];
        }
    }
    return n;
}

f32 fn_801DDC8C(void* q, f32 x, f32 z) {  // q: the caller passes body+0x6C (unused here; retail r3)
    f32 h;
    f32 fz;
    GroundHeader* g;
    g = lbl_8055E990;
    if (!g->tiled) {
        return 0.0f;
    }
    f32 fx;
    int iz;
    int tx;
    int lz;
    int ix;
    fx = g->scaleX * (x + g->offX);
    int lx;
    fz = (z + g->offZ) * g->scaleZ;
    ix = Round(fx);
    iz = Round(fz);
    int tz;
    if (ix < 0 || iz < 0) {
        return 0.0f;
    }
    tx = ix / 32;
    GTile* t;
    tz = iz / 32;
    lx = ix - tx * 32;
    lz = iz - 32 * tz;
    t = TileAt(g, tx, tz);
    if (!t) {
        return 0.0f;
    }
    h = 0.0f;
    s32 li = t->layer;
    if (li >= 0) {
        GLayer* l;
        l = &lbl_8055E990->layers[li];
        for (;;) {
            f32 v = l->base + l->scale * l->h[lx][lz];
            h += v * lbl_804425DC[l->weight];
            if (l->next < 0) {
                break;
            }
            l = &lbl_8055E990->layers[l->next];
        }
    }
    return h;
}

}  // extern "C"
