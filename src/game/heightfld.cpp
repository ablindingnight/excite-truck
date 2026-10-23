// Track heightfield (0x801DB348-0x801DDE78, heightfld.cpp): the "track.gnd" ground grid - 32x32 tiles of
// 16-bit heights placed on a 96x96 tile index, each with run-length surface materials and a chain of
// weighted displacement layers - sampled with Catmull-Rom splines for height and normal; plus a
// procedural fake heightfield used when there is no track, and deformation records that track how the
// layer weights move a point. Names are guesses apart from the strings.
#include "game/common.h"

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

extern "C" {
void* memset(void* p, int c, u32 n);
double floor(double x);
double sqrt(double x);
double sin(double x);
double cos(double x);
}

struct GVec {
    f32 x, y, z;
};

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
void fn_801D02B0(void);
void fn_801D049C(void);
void fn_802E30C8(const char* fmt, ...);
void fn_802E317C(const char* fmt, ...);
void PSVECNormalize(const GVec* src, GVec* dst);
}

extern f32 lbl_80558DC4[];  // FLT_EPSILON

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

static inline void Swap16(void* p) {
    u8 t = ((u8*)p)[0];
    ((u8*)p)[0] = ((u8*)p)[1];
    ((u8*)p)[1] = t;
}

static inline void Swap32(void* p) {
    u8 t = ((u8*)p)[0];
    ((u8*)p)[0] = ((u8*)p)[3];
    ((u8*)p)[3] = t;
    t = ((u8*)p)[1];
    ((u8*)p)[1] = ((u8*)p)[2];
    ((u8*)p)[2] = t;
}

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

extern "C" int fn_801DB348(int x, int z, f32* out, s32* mat, int unused);

static inline int Sample(int x, int z, f32* out, s32* mat, int unused) {
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
    f32 h = t->base + t->scale * t->h[lx][lz];
    if (t->layer >= 0) {
        GroundHeader* gg = lbl_8055E990;
        for (GLayer* l = &gg->layers[t->layer]; l; l = l->next >= 0 ? &gg->layers[l->next] : 0) {
            h += lbl_804424B0[l->weight] * (l->base + l->scale * l->h[lx][lz]);
        }
    }
    u8* r = g->runs + t->rows[lz];
    int n = 0;
    int m;
    for (;;) {
        if (n + r[1] > lx) {
            m = r[0];
            break;
        }
        n += r[1];
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

static inline f32 Spline(f32 p0, f32 p1, f32 p2, f32 p3, f32 t, f32 t2, f32 t3) {
    f32 m1 = 0.5f * (p2 - p0);
    f32 m2 = 0.5f * (p3 - p1);
    f32 d = p2 - p1;
    f32 c2 = 3.0f * d - 2.0f * m1 - m2;
    f32 c3 = m1 + m2 - 2.0f * d;
    return p1 + (m1 * t + (c3 * t3 + c2 * t2));
}

static inline f32 SplineD(f32 p0, f32 p1, f32 p2, f32 p3, f32 t, f32 t2, f32 t3, f32* deriv) {
    f32 m1 = 0.5f * (p2 - p0);
    f32 m2 = 0.5f * (p3 - p1);
    f32 d = p2 - p1;
    f32 c3 = m1 + m2 - 2.0f * d;
    f32 c2 = 3.0f * d - 2.0f * m1 - m2;
    if (deriv) {
        *deriv = m1 + (3.0f * c3 * t2 + 2.0f * c2 * t);
    }
    return p1 + (m1 * t + (c3 * t3 + c2 * t2));
}

extern "C" {

int fn_801DB348(int x, int z, f32* out, s32* mat, int unused) {
    if (mat) {
        *mat = 0;
    }
    *out = lbl_80446F08.data[(z & (lbl_80446F08.h - 1)) * lbl_80446F08.w + (x & (lbl_80446F08.w - 1))];
    return 1;
}

f32 fn_801DB398(const f32 g[4][4], f32 fx, f32 fz) {
    f32 t2 = fx * fx;
    f32 t3 = t2 * fx;
    f32 col[4];
    for (int i = 0; i < 4; i++) {
        col[i] = Spline(g[0][i], g[1][i], g[2][i], g[3][i], fx, t2, t3);
    }
    f32 s2 = fz * fz;
    return Spline(col[0], col[1], col[2], col[3], fz, s2, s2 * fz);
}

f32 fn_801DB514(const f32 g[4][4], GVec* normal, f32 fx, f32 fz) {
    f32 t2 = fx * fx;
    f32 t3 = t2 * fx;
    f32 col[4];
    f32 dcol[4];
    for (int i = 0; i < 4; i++) {
        col[i] = SplineD(g[0][i], g[1][i], g[2][i], g[3][i], fx, t2, t3, &dcol[i]);
    }
    f32 s2 = fz * fz;
    f32 s3 = s2 * fz;
    f32 dz;
    f32 h = SplineD(col[0], col[1], col[2], col[3], fz, s2, s3, &dz);
    GVec n;
    n.y = 1.0f / lbl_8055E990->scaleX;
    n.z = -dz;
    n.x = -Spline(dcol[0], dcol[1], dcol[2], dcol[3], fz, s2, s3);
    PSVECNormalize(&n, &n);
    *normal = n;
    return h;
}

int fn_801DB74C(int x, int z, f32* out, s32* mat, int unused, GVec* normal, f32 fx, f32 fz) {
    f32 h0;
    if (!Sample(x, z, &h0, mat, unused) && !Sample(x + 1, z, &h0, mat, unused) && !Sample(x, z + 1, &h0, mat, unused) &&
        !Sample(x + 1, z + 1, &h0, mat, unused)) {
        return 0;
    }
    f32 g[4][4];
    for (int j = 0; j < 4; j++) {
        int sz = z + j - 1;
        for (int i = 0; i < 4; i++) {
            if (!Sample(x + i - 1, sz, &g[i][j], 0, 0)) {
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

int fn_801DC258(f32* out, s32* mat, int unused, GVec* normal, f32 x, f32 z) {
    GroundHeader* g = lbl_8055E990;
    f32 fx = g->scaleX * (x + g->offX);
    f32 fz = g->scaleZ * (z + g->offZ);
    f32 ix = floor(fx);
    f32 iz = floor(fz);
    return fn_801DB74C((int)ix, (int)iz, out, mat, unused, normal, fx - ix, fz - iz);
}

void fn_801DC32C(GroundHeader* g) {
    u32* w = (u32*)g;
    for (int k = 0; k < 13; k++) {
        u8 t = ((u8*)&w[k])[0];
        ((u8*)&w[k])[0] = ((u8*)&w[k])[3];
        ((u8*)&w[k])[3] = t;
        t = ((u8*)&w[k])[1];
        ((u8*)&w[k])[1] = ((u8*)&w[k])[2];
        ((u8*)&w[k])[2] = t;
    }
    g->tiles = (GTile*)((u8*)g + (u32)g->tiles);
    g->layers = (GLayer*)((u8*)g + (u32)g->layers);
    g->items = (GItem*)((u8*)g + (u32)g->items);
    g->runs = (u8*)g + (u32)g->runs;
    memset(lbl_80442708, -1, sizeof(lbl_80442708));
    lbl_8055E9A0 = 0;
    lbl_8055E99C = 0;
    int minX = 0;
    int minZ = 0;
    int maxX = 0;
    int maxZ = 0;
    int i;
    for (i = 0; i < g->numTiles; i++) {
        GTile* t = &g->tiles[i];
        Swap32(&t->base);
        Swap32(&t->scale);
        for (int a = 0; a < 32; a++) {
            for (int b = 0; b < 32; b++) {
                Swap16(&t->h[b][a]);
            }
        }
        Swap32(&t->x);
        Swap32(&t->z);
        for (int r = 0; r < 32; r++) {
            Swap32(&t->rows[r]);
        }
        Swap32(&t->layer);
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
        Swap32(&l->base);
        Swap32(&l->scale);
        for (int a = 0; a < 32; a++) {
            for (int b = 0; b < 32; b++) {
                Swap16(&l->h[b][a]);
            }
        }
        Swap32(&l->weight);
        Swap32(&l->next);
    }
    for (i = 0; i < g->numItems; i++) {
        GItem* it = &g->items[i];
        Swap32(&it->w[0]);
        Swap32(&it->w[1]);
        Swap32(&it->w[2]);
        Swap32(&it->w[3]);
    }
}

GroundHeader* fn_801DC9B0(void) {
    u32 ver = 0;
    u8 swapped;
    fn_8012E96C("track.gnd");
    GroundHeader* g = (GroundHeader*)fn_8012C370("track.gnd", 'Grnd', &ver, 0, 0, &swapped);
    if (g) {
        if (ver != 0) {
            fn_802E317C("ground: version mismatch on %s (%x != %x)", "track.gnd", ver, 0);
        }
        if (swapped) {
            fn_801DC32C(g);
        }
    }
    return g;
}

void fn_801DCA5C(void* p) { fn_8012C4D8(p); }

GroundHeader* fn_801DCA60(void) {
    lbl_80446F08.h = 256;
    lbl_80446F08.w = 256;
    GroundHeader* g = (GroundHeader*)operator new(sizeof(GroundHeader), "fake-GroundHeader");
    memset(g, 0, sizeof(GroundHeader));
    f32 size = 1024.0f * 2.0f;
    g->offZ = g->offX = size * 0.5f;
    g->scaleZ = g->scaleX = lbl_80446F08.w / size;
    lbl_80446F08.data = new ("heightfield") f32[lbl_80446F08.w * lbl_80446F08.h];
    for (int z = 0; z < lbl_80446F08.w; z++) {
        int dz = z - 128;
        for (int x = 0; x < lbl_80446F08.h; x++) {
            int dx = x - 128;
            f32 k = 1.0f;
            f32 r2 = (f32)dz * (f32)dz + (f32)dx * (f32)dx;
            if (r2 > 1314.0625f && r2 < 2025.0f) {
                f32 r = sqrt(r2);
                if (r > 40.625f) {
                    r = 40.625f - (r - 40.625f);
                }
                r = (r - 36.25f) / 1.875f;
                r = 1.0f < r ? 1.0f : r;
                r = r > 0.0f ? r : 0.0f;
                k = 1.0f + r * -0.19999999f;
            }
            f32 h = 0.0f;
            f32 a = 15.0f;
            for (int n = lbl_80446F08.w / 2; n > 16; n /= 2) {
                h = h + a * (1.0f + (f32)sin(6.2831855f * ((f32)(z * 2) / n)));
                h = h + a * (1.0f + (f32)cos(6.2831855f * ((f32)(x * 2) / n)));
                a = a * (0.6f * k);
            }
            lbl_80446F08.data[x * lbl_80446F08.w + z] = h;
        }
    }
    return g;
}

void fn_801DCD24(void* p) { operator delete(p); }

void fn_801DCD28(int fake) {
    lbl_8055E998 = 0;
    if (!fake) {
        lbl_8055E990 = fn_801DC9B0();
        fn_802E30C8("load_ground: %p", lbl_8055E990);
        lbl_8055E994 = fn_801DCA5C;
    } else {
        lbl_8055E990 = fn_801DCA60();
        lbl_8055E994 = fn_801DCD24;
    }
    lbl_8055E9A4 = 1;
    fn_801D02B0();
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

void fn_801DCE0C(int fake) { fn_801DCD28(fake); }

void fn_801DCE10(void) {}

int fn_801DCE14(u16* flags, const GVec* in, GVec* out, GVec* normal, s32* mat) {
    if (flags) {
        *flags = 0;
    }
    s32 m = 0xFF;
    f32 h;
    if (fn_801DC258(&h, &m, (int)flags, normal, in->x, in->z)) {
        if (mat) {
            *mat = m;
        }
        out->y = h;
        out->x = in->x;
        out->z = in->z;
        return 1;
    }
    return 0;
}

f32 fn_801DCEC0(void) { return 1.0f; }

void fn_801DCEC8(int i, f32 w) { lbl_804424B0[i] = w; }

void fn_801DCEDC(int i, f32 w) { lbl_804425DC[i] = w; }

void fn_801DCEF0(void) {}

void fn_801DCEF4(void) { lbl_8055E9A8[0] = 0; }

void fn_801DCF00(void) { lbl_8055E9A8[0] = 0; }

f32 fn_801DCF0C(f32 x, f32 z) {
    f32 h = -1000.0f;
    fn_801DC258(&h, 0, 0, 0, x, z);
    return h;
}

static inline int Round(f32 v) { return (int)(v < 0.0f ? v - 0.5f : 0.5f + v); }

f32 fn_801DCF48(GVec* normal, f32 x, f32 z) {
    GroundHeader* g = lbl_8055E990;
    f32 h;
    if (!g->tiled) {
        h = 0.0f;
        if (!fn_801DC258(&h, 0, 0, normal, x, z)) {
            h = 0.0f;
            if (normal) {
                normal->x = h;
                normal->y = 1.0f;
                normal->z = h;
            }
        }
        return h;
    }
    f32 fx = g->scaleX * (x + g->offX);
    f32 fz = g->scaleZ * (z + g->offZ);
    int ix = Round(fx);
    int iz = Round(fz);
    int tx = ix / 32;
    int lx = ix - tx * 32;
    int tz = iz / 32;
    int lz = iz - tz * 32;
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
        h = t->base + t->scale * t->h[lx][lz];
        f32 hz = t->base + t->scale * t->h[lx][oz];
        f32 hx = t->base + t->scale * t->h[ox][lz];
        for (GLayer* l = LayerAt(t->layer); l; l = LayerAt(l->next)) {
            f32 w = lbl_804424B0[l->weight];
            h += w * (l->base + l->scale * l->h[lx][lz]);
            hx += w * (l->base + l->scale * l->h[ox][lz]);
            hz += w * (l->base + l->scale * l->h[lx][oz]);
        }
        hx -= h;
        hz -= h;
        f32 sx = (lx & 1 ? -1.0f : 1.0f) / g->scaleX;
        f32 sz = (lz & 1 ? -1.0f : 1.0f) / g->scaleZ;
        normal->x = -hx * sx;
        normal->z = -hz * sz;
        normal->y = __fabs(sx * sz);
        PSVECNormalize(normal, normal);
        return h;
    }
    h = t->base + t->scale * t->h[lx][lz];
    for (GLayer* l = LayerAt(t->layer); l; l = LayerAt(l->next)) {
        h += lbl_804424B0[l->weight] * (l->base + l->scale * l->h[lx][lz]);
    }
    return h;
}

void fn_801DD3BC(f32* out, int fixed, f32 x, f32 z) {
    Deform* d = &lbl_80446F18[lbl_8055E9A8[0]++];
    d->out = out;
    f32 base = fn_801DCF0C(x, z);
    f32 saved[75];
    int i;
    for (i = 0; i < 75; i++) {
        saved[i] = lbl_804424B0[i];
        lbl_804424B0[i] = 0.0f;
    }
    f32 h0 = fn_801DCF0C(x, z);
    if (!fixed) {
        base = h0;
    }
    d->base = h0 + (*out - base);
    d->terms[0].weight = 0;
    d->terms[0].k = 0.0f;
    d->terms[1].weight = 0;
    d->terms[1].k = 0.0f;
    d->terms[2].weight = 0;
    d->terms[2].k = 0.0f;
    int n = 0;
    for (i = 0; i < 75; i++) {
        lbl_804424B0[i] = 1.0f;
        f32 dh = fn_801DCF0C(x, z) - h0;
        if (!(__fabs(dh) <= lbl_80558DC4[0])) {
            if (n >= 3) {
                int k = -1;
                f32 m = 0.0f;
                if (k < 0 || __fabs(d->terms[0].k) < __fabs(m)) {
                    k = 0;
                    m = d->terms[0].k;
                }
                if (k < 0 || __fabs(d->terms[1].k) < __fabs(m)) {
                    k = 1;
                    m = d->terms[1].k;
                }
                if (k < 0 || __fabs(d->terms[2].k) < __fabs(m)) {
                    k = 2;
                }
                if (__fabs(d->terms[k].k) < __fabs(dh)) {
                    d->terms[k].k = dh;
                    d->terms[k].weight = i;
                }
            } else {
                d->terms[n].weight = i;
                n++;
                d->terms[n - 1].k = dh;
            }
        }
        lbl_804424B0[i] = 0.0f;
    }
    for (i = 0; i < 75; i++) {
        lbl_804424B0[i] = saved[i];
    }
    int any = 0;
    if (!(__fabs(d->terms[0].k) <= lbl_80558DC4[0])) {
        any = 1;
    }
    if (!(__fabs(d->terms[1].k) <= lbl_80558DC4[0])) {
        any = 1;
    }
    if (!(__fabs(d->terms[2].k) <= lbl_80558DC4[0])) {
        any = 1;
    }
    if (!any) {
        lbl_8055E9A8[0]--;
    }
}

void fn_801DD808(void) {
    Deform* d = lbl_80446F18;
    for (int i = 0; i < lbl_8055E9A8[0]; i++, d++) {
        *d->out = d->base + d->terms[0].k * lbl_804424B0[d->terms[0].weight] + d->terms[1].k * lbl_804424B0[d->terms[1].weight] +
                  d->terms[2].k * lbl_804424B0[d->terms[2].weight];
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

int fn_801DDA84(f32 x, f32 z, s32* out, int max, f32 thr) {
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
    int lz = iz - tz * 32;
    int lx = ix - tx * 32;
    GTile* t = TileAt(g, tx, tz);
    if (!t) {
        return 0;
    }
    int n = 0;
    s32 li = t->layer;
    if (li < 0) {
        return 0;
    }
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
    return n;
}

f32 fn_801DDC8C(f32 x, f32 z) {
    GroundHeader* g = lbl_8055E990;
    if (!g->tiled) {
        return 0.0f;
    }
    f32 fx = g->scaleX * (x + g->offX);
    f32 fz = g->scaleZ * (z + g->offZ);
    int ix = Round(fx);
    int iz = Round(fz);
    if (ix < 0 || iz < 0) {
        return 0.0f;
    }
    int tx = ix / 32;
    int tz = iz / 32;
    int lz = iz - tz * 32;
    int lx = ix - tx * 32;
    GTile* t = TileAt(g, tx, tz);
    if (!t) {
        return 0.0f;
    }
    f32 h = 0.0f;
    if (t->layer < 0) {
        return h;
    }
    GLayer* l = &g->layers[t->layer];
    for (;;) {
        h += (l->base + l->scale * l->h[lx][lz]) * lbl_804425DC[l->weight];
        if (l->next < 0) {
            break;
        }
        l = &g->layers[l->next];
    }
    return h;
}

}  // extern "C"
