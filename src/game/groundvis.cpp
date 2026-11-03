// Ground visibility (0x802AE5E0-0x802B0628): sets up and tears down the terrain subsystems,
// keeps the active ground set, and walks the terrain quadtree to build per-frame visibility masks.
// Names are guesses apart from the value keys and messages.
#include "game/common.h"
#include <revolution/MTX.h>

struct GroundFrame {
    Vec axes[3];  // offset 0x0
    Vec pos;      // offset 0x24
};

// A quadtree node waiting to be visited.
struct GroundNode {
    s16 x;      // offset 0x0
    s16 y;      // offset 0x2
    s16 size;   // offset 0x4
    s16 unk6;   // offset 0x6
    f32 scale;     // offset 0x8
    u16* edge[4];  // offset 0xC: vertex ids along each side (0xFFFF = not computed, 0xFFFE = skipped)
};

struct GroundStack {
    s32 count;             // offset 0x0
    GroundNode nodes[64];  // offset 0x4
};

struct GroundCell {
    u8 pad0[0x8];
    u32 flags;  // offset 0x8
};

struct GroundLayer {
    u32 a;  // offset 0x0
    u32 b;  // offset 0x4
};

struct GroundSet {
    u8 pad0[0x4];
    u32 terrain;           // offset 0x4
    u32 objects;           // offset 0x8
    GroundLayer layers[4];  // offset 0xC
    u32 sky;               // offset 0x2C
};

struct GroundList {
    void* items;  // offset 0x0
    u32 count;    // offset 0x4
};

// An object scattered over a cell, placed between the cell centre and one of its corners.
struct GroundItem {
    f32 t0;           // offset 0x0
    f32 t1;           // offset 0x4
    u16 kind;         // offset 0x8
    u16 corner;       // offset 0xA
    void* lists[6];   // offset 0xC, per detail level
};

struct GroundBuf {
    u16* data;  // offset 0x0
    u16 count;  // offset 0x4
    u16 cap;    // offset 0x6
    void Add(u16 v) {
        if (count < cap) {
            data[count++] = v;
        }
    }
};

struct ViewFlags {
    u8 pad0[0x10];
    u32 pad : 30;  // offset 0x10
    u32 hires : 1;
    u32 bit0 : 1;
};

extern "C" int fn_802E2C20(void);                  // milliseconds
extern "C" void fn_802E30C8(const char* fmt, ...);  // warning print

// Times a block and reports it when it runs long.
struct ScopeTimer {
    ScopeTimer(const char* n) {
        name = n;
        start = fn_802E2C20();
    }
    ~ScopeTimer(void) {
        int t = fn_802E2C20() - start;
        if (t > 500) {
            fn_802E30C8("%s: took %dms", name, t);
        }
    }
    const char* name;  // offset 0x0
    int start;         // offset 0x4
};

extern "C" {
int fn_8012C33C(const char* name);
f32 fn_801DCF48(int a, f32 x, f32 z);
GroundFrame* fn_801E8718(s32 a);
ViewFlags* fn_801E87A8(void);
f32 fn_801EA0E8(f32 x, f32 z);
void fn_801FA6B8(void);
void fn_801FA738(void);
void fn_80202718(void);
void fn_802879E4(int a);
void fn_80287A34(int a);
void fn_8029790C(int a);
void fn_802B0914(void);
void fn_802B0954(void);
u32 fn_802B127C(u32 t, const Vec* p);
void fn_802B14B4(u32 t);
int fn_802B1C50(u32 t, u32 a, u32 b);
void fn_802B1E28(void);
void fn_802B1FD4(void);
void fn_802B2D94(u32 o);
void fn_802B40F8(void);
void fn_802B4BDC(void);
void fn_802B4E30(void);
void fn_802B4E44(u32 o);
void fn_802B57B8(void);
void fn_802B5808(void);
void fn_802B58E0(void);
GroundSet* fn_802B59A4(int a);
ValueNode* fn_802B59CC(void);
void fn_802B9B68(void);
void fn_802B9D08(void);
void fn_802BA004(void);
void fn_802BA348(void);
void fn_802BAE50(void);
void fn_802BB674(u32 a, int i);
void fn_802BBBC8(void);
void fn_802BBCC4(void);
u32 fn_802BBD38(short x, short z, int layer);  // layer weight (lweight.cpp; defined there returning u8)
void fn_802BDBA8(void);
void fn_802BDC3C(void);
void fn_802BDC60(void);
void fn_802BDD54(void);
void fn_802BDD58(void);
void fn_802BE7F8(void);
void fn_802BE840(void);
GroundCell* fn_802BE844(s16 x, s16 y, int lvl);
void fn_802BEAB0(void);
void fn_802BEB70(void);
void fn_802BF350(void);
void fn_802BF40C(void);
void fn_802C10B8(void);
void fn_802C10C4(void);
void fn_802C10C8(void);
void fn_802C10CC(u32 a);
void fn_802C2138(void);
void fn_802C217C(void);
void fn_802C2298(void);
void fn_802C2410(void);
void fn_802C26F0(void);
void fn_802C2874(void);
void fn_802C29A0(void);
void fn_802C2A34(void);
void fn_802C7A44(void);
void fn_802C7A50(void);
void fn_802C9648(f32 a);
void fn_802C967C(void);
void fn_802C9904(void);
void fn_802C9950(u32 c);
void fn_802C9994(u32 c);
int fn_802C9C58(int a);
void fn_802DBE4C(void);
void fn_802DBECC(void);
double sqrt(double x);
void* memset(void* dst, int c, size_t n);
}

void* operator new[](size_t size, const char* name);

static inline f32 QuarterPi(void) { return 0.7853982f; }

extern const f32 lbl_80564AF0;  // cell size

ModuleTag lbl_8055F960;
UnkStaticFFFF lbl_8055F968;
extern const f32 lbl_8055F96C = QuarterPi();
GroundSet* lbl_8055F970;   // active ground set
u32 lbl_8055F974;          // terrain
u32 lbl_8055F978;          // objects
s32 lbl_8055F97C;          // water cell value
u8 lbl_8055F980;           // has a swell map
u8 lbl_8055F981;           // crystal mode
s32 lbl_8055CD78 = -1;     // water layer
u16* lbl_8055F984;         // visibility masks
s32 lbl_8055F988;          // view quadrant
f32 lbl_8055F98C;

static GroundStack lbl_804BD8E8;
static s32 lbl_804BDFF0[40];  // per-level mask offsets
// The camera as the quadtree walk sees it.
struct GroundView {
    GroundFrame cam;  // offset 0x0
    f32 tanV;         // offset 0x30
    f32 secV;         // offset 0x34
    f32 tanH;         // offset 0x38
    f32 secH;         // offset 0x3C
    f32 znear;        // offset 0x40
    f32 zfar;         // offset 0x44
};

static f32 lbl_804BE090[8];      // LOD switch distances
static GroundView lbl_804BE0B0;

extern "C" {

void fn_802AE64C(u16** p);

void fn_802AE5E0(int n) {
    s32* p = &lbl_804BDFF0[1];
    int ofs = 0;
    for (int s = 2; s <= n; s *= 2) {
        *p = ofs;
        ofs += (s + 1) * 4;
        p++;
    }
    lbl_8055F984 = new ("wtab") u16[ofs];
}

void fn_802AE644(void) { fn_802AE64C(&lbl_8055F984); }

void fn_802AE64C(u16** p) {
    if (*p) {
        delete[] *p;
        *p = 0;
    }
}

void fn_802AE690(const GroundFrame* cam) {
    f32 x = cam->axes[2].x;
    if (x * cam->axes[2].z > 0.0f) {
        lbl_8055F988 = x > 0.0f ? 3 : 0;
        return;
    }
    lbl_8055F988 = x > 0.0f ? 1 : 2;
}

f32 fn_802AE6E0(const Vec* a, const Vec* b) {
    f32 dz = a->z - b->z;
    f32 dx = a->x - b->x;
    f32 z2 = dz * dz;
    f32 x2 = dx * dx;
    return sqrt(x2 + z2);
}

void fn_802AE728(void) {
    fn_802C7A44();
    fn_802C9648(1.1f);
    fn_80287A34(0);
    fn_802C967C();
    fn_802879E4(0);
    fn_801FA6B8();
    fn_802C7A50();
    fn_802C9904();
    fn_801FA738();
}

int fn_802B05E4(void);

void fn_802AE774(void) {
    fn_802BA004();
    if (!fn_802B05E4()) {
        fn_802BA348();
    }
    fn_802C29A0();
    fn_802BAE50();
}

int fn_802AE7AC(void) { return lbl_8055F984 != 0; }

void fn_802AE7C0(void) {
    fn_802C9950(0x555766);
    fn_802C9994(0xFFF5D9);
}

void fn_802AE7F4(void) {
    lbl_8055F980 = fn_8012C33C("swlmap.tex");
    fn_802AE5E0(0x400);
    fn_802BE7F8();
    fn_802B40F8();
    fn_802B0914();
    fn_802C26F0();
    fn_802B1E28();
    fn_802B57B8();
    fn_802BBBC8();
    fn_802C2138();
    fn_802B9B68();
    fn_802BDBA8();
    fn_802C2298();
    if (!lbl_8055F980) {
        fn_802BF350();
    }
    fn_802B58E0();
    lbl_8055CD78 = fn_802B59CC()->GetInt("water_layer", -1);
    lbl_8055F981 = fn_802B59CC()->GetBool("crystal_mode", 0);
    f32 dist[8];
    dist[0] = 10.0f * (2.0f * lbl_80564AF0);
    dist[1] = 4.0f * (4.0f * lbl_80564AF0);
    dist[2] = 4.0f * (8.0f * lbl_80564AF0);
    dist[3] = 10.0f * (16.0f * lbl_80564AF0);
    dist[4] = 4.0f * (32.0f * lbl_80564AF0);
    dist[5] = 3.0f * (64.0f * lbl_80564AF0);
    dist[6] = 2.0f * (128.0f * lbl_80564AF0);
    dist[7] = 256.0f * lbl_80564AF0;
    f32 diag = 1.4142f * (0.5f * lbl_80564AF0);
    f32 sum = dist[0];
    f32* out = &lbl_804BE090[1];
    int s = 2;
    for (int i = 1; i < 8; i++) {
        *out = sum;
        f32 d = diag * (f32)(s * 2 + s);
        sum += d > dist[i] ? d : dist[i];
        s *= 2;
        out++;
    }
    fn_802BEAB0();
    lbl_8055F97C = 0xFA;
}

void fn_802AE9D0(void) {
    lbl_8055CD78 = -1;
    fn_802C10C4();
    if (!fn_802B05E4()) {
        fn_802BF40C();
    }
    fn_802C2410();
    fn_802BDC60();
    fn_802B9D08();
    fn_802C2A34();
    fn_802B5808();
    fn_802C217C();
    fn_802B1FD4();
    fn_802C2874();
    fn_802B0954();
    fn_802B4BDC();
    fn_802BBCC4();
    fn_802BE840();
    fn_802AE644();
    lbl_8055F970 = 0;
}

void fn_802AEA44(void) {
    fn_802C10B8();
    {
        ScopeTimer t("FanInfoSetVisibilityMasks");
        fn_802BEB70();
    }
    fn_802AE774();
}

void fn_802AEAA8(void) { fn_802BDC3C(); }

void fn_802B0288(GroundSet* g);

void fn_802AEAAC(void) {
    if (!lbl_8055F970) {
        fn_802B0288(fn_802B59A4(0));
    }
    lbl_8055F974 = lbl_8055F970->terrain;
    lbl_8055F978 = lbl_8055F970->objects;
    for (int i = 0; i < 4; i++) {
        fn_802BB674(lbl_8055F970->layers[i].a, i);
    }
    fn_802C10CC(lbl_8055F970->sky);
}

// Mask level for a node of size n: log2(n) for powers of two, from the float exponent.
static inline int MaskLevel(int n) {
    if (n >= 2) {
        union {
            f32 f;
            u32 u;
        } v;
        v.f = n;
        return ((v.u >> 23) & 0x7F) + 1;
    }
    return 0;
}

void fn_802AFE64(GroundStack* s, s16 x, s16 y, s16 size, s16 unk6, f32 scale, u16* e0, u16* e1, u16* e3, u16* e2);
void fn_802AFEA4(GroundStack* s, s16 size);
u16* fn_802AFEC0(int n);
u32 fn_802AFF54(GroundList* l);
void* fn_802AFF5C(GroundList* l, int i);
void fn_802AFF6C(GroundBuf* b);
void fn_802AFF80(GroundBuf* b, u16 v);
u16 fn_802AFFA8(s16 x, s16 y);
void fn_802B0024(GroundBuf* b, u16 v);
u16 fn_802B0070(const Vec* p);
void fn_802B009C(Vec* out, int a, s16 x, s16 y);
void fn_802B0114(Vec* v);
u32 fn_802B0118(const GroundCell* c);
u32 fn_802B0124(const GroundCell* c);
s16 fn_802B0130(const GroundNode* n);

void StatsUpdate__Fv(Vec* v);  // empty Vec constructor (folded)
f32 PSVECDotProduct(const Vec* a, const Vec* b);
double atan(double x);
double tan(double x);
double sin(double x);
double fabs(double x);
f32 fn_80203FF0(void);
f32 fn_802C8FEC(void);
f32 fn_802C973C(void);
void fn_802C98DC(f32* fov, f32* znear, f32* zfar, f32* aspect);
void fn_802B11BC(u32 t);
void fn_802B2D24(u32 o);
int fn_802B12E0(u32 t, GroundBuf* out, GroundCell* cell, s16 half);
void fn_802B13F8(u32 t, GroundCell* cell, GroundBuf* b, s16 half, f32 alpha);
void fn_802B1BF0(u32 t, Vec* out, int i);
void fn_802BDFBC(GroundCell* cell, s16 lod, f32 blend);
GroundList* fn_802B4F10(s32* used, s16 x, s16 y, s16 size, int lod);
void fn_80144DAC(Vec* dst, const Vec* src);
void fn_8025AD3C(Vec* out, const Vec* a, const Vec* b, f32 t);
void fn_801C0E60(Vec* out, const Vec* a, const Vec* b);
void fn_801C2B9C(Vec* out, const Vec* a, const Vec* b, f32 t);
void fn_802B3E00(void* list, const Vec* pos, u16 kind, f32 scale);
}

f32 Remap(f32 lo, f32 hi, f32 outLo, f32 outHi, f32 v);

extern "C" {

static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

// Is the distance of a node from the view plane through this axis inside the view cone?
static inline int InCone(f32 a, f32 depth, f32 tanA, f32 secA, f32 pad, f32 radius, f32 k) {
    f32 t = depth * tanA;
    if (0.0f > t) {
        t = 0.0f;
    }
    return a <= t + (pad + radius * k) * secA;
}

static inline int NodeVisible(const Vec* c, f32 radius, f32 k, f32* dist) {
    Vec d;
    d.x = c->x - lbl_804BE0B0.cam.pos.x;
    d.y = c->y - lbl_804BE0B0.cam.pos.y;
    d.z = c->z - lbl_804BE0B0.cam.pos.z;
    f32 depth = PSVECDotProduct(&d, &lbl_804BE0B0.cam.axes[2]);
    f32 r = radius * k;
    if (depth + r < lbl_804BE0B0.znear || depth - r > lbl_804BE0B0.zfar) {
        return 0;
    }
    f32 ax = fabs(PSVECDotProduct(&d, &lbl_804BE0B0.cam.axes[0]));
    if (InCone(ax, depth, lbl_804BE0B0.tanV, lbl_804BE0B0.secV, 18.0f, radius, k)) {
        f32 ay = fabs(PSVECDotProduct(&d, &lbl_804BE0B0.cam.axes[1]));
        if (InCone(ay, depth, lbl_804BE0B0.tanH, lbl_804BE0B0.secH, 40.0f, radius, k)) {
            *dist = fn_802AE6E0(c, &lbl_804BE0B0.cam.pos);
            return 1;
        }
    }
    return 0;
}

// Walks the terrain quadtree from the camera, building the visible cell batches and their scattered objects.
void fn_802AEB3C(const GroundFrame* cam) {
    fn_802AEAAC();
    fn_802B11BC(lbl_8055F974);
    fn_802B2D24(lbl_8055F978);
    lbl_804BE0B0.cam = *cam;
    fn_802AE690(&lbl_804BE0B0.cam);

    f32 above = -10.0f + lbl_804BE0B0.cam.pos.y - fn_801EA0E8(lbl_804BE0B0.cam.pos.x, lbl_804BE0B0.cam.pos.z);
    lbl_8055F98C = fn_80203FF0() + (0.0f > above ? 0.0f : above);

    f32 aspect = fn_802C8FEC();
    f32 fov = fn_802C973C();
    f32 hfov = 2.0f * (f32)atan((f32)tan(fov * 0.5f) / aspect);
    lbl_804BE0B0.tanV = tan(fov * 0.5f);
    lbl_804BE0B0.secV = 1.0f / (f32)sin(1.5707964f - fov * 0.5f);
    lbl_804BE0B0.tanH = tan(hfov * 0.5f);
    lbl_804BE0B0.secH = 1.0f / (f32)sin(1.5707964f - hfov * 0.5f);
    f32 tmp;
    fn_802C98DC(&tmp, &lbl_804BE0B0.znear, &lbl_804BE0B0.zfar, &tmp);

    u16* root = lbl_8055F984 + lbl_804BDFF0[MaskLevel(0x400)];
    memset(root, -1, (0x400 + 1) * 8);
    GroundNode* top = &lbl_804BD8E8.nodes[lbl_804BD8E8.count++];
    top->x = 0;
    top->y = 0;
    top->size = 0x400;
    top->unk6 = 0;
    top->scale = 1.0f;
    top->edge[0] = root;
    top->edge[1] = root + 0x401;
    top->edge[2] = root + 0xC03;
    top->edge[3] = root + 0x802;

    GroundCell* cell = 0;
    while (lbl_804BD8E8.count != 0) {
        GroundNode* n = &lbl_804BD8E8.nodes[--lbl_804BD8E8.count];
        if (n->size < 0) {
            continue;
        }
        int half = n->size / 2;
        if (half >= 4 && half <= 64) {
            cell = fn_802BE844(n->x, n->y, half);
        }
        f32 dist = 0.0f;
        half = n->size / 2;
        f32 radius = lbl_80564AF0 * half;
        Vec c;
        f32 blend;
        f32 k;
        if (half > 64) {
            c.x = n->x * lbl_80564AF0;
            c.z = n->y * lbl_80564AF0;
            c.y = fn_801DCF48(0, c.x, c.z);
            k = 2.1213f;
        } else {
            c.x = n->x * lbl_80564AF0;
            c.z = n->y * lbl_80564AF0;
            c.y = -12.0f + (f32)((cell->flags >> 14) & 0x1FF);
            k = (f32)(((cell->flags >> 10) & 0xF) + 1) * 1.5f;
            if (half < 4) {
                k *= (f32)(4 / half);
            }
        }
        if (!NodeVisible(&c, radius, k, &dist)) {
            continue;
        }

        blend = -1.0f;
        int draw;
        if (n->size > 128) {
            blend = 1.0f;
            draw = 0;
        } else {
            f32 e = lbl_8055F98C + (dist - 1.4142f * (n->size * lbl_80564AF0 * 0.5f));
            if (0.0f > e) {
                e = 0.0f;
            }
            int lvl = MaskLevel(n->size);
            draw = e >= lbl_804BE090[lvl - 1];
            if (!draw) {
                f32 f = (e - lbl_804BE090[lvl - 1]) / (lbl_804BE090[lvl - 2] - lbl_804BE090[lvl - 1]);
                if (1.0f < f) {
                    f = 1.0f;
                }
                f = f > 0.0f ? f : 0.0f;
                blend = Lerp(0.0f, 1.0f, f);
            }
        }

        if (draw) {
            GroundBuf buf;
            if (!fn_802B12E0(lbl_8055F974, &buf, cell, fn_802B0130(n))) {
                continue;
            }
            // Corner offsets of each side, in half-node steps: start, middle and end.
            s16 endY[4] = {1, -1, -1, 1};
            s16 endX[4] = {-1, -1, 1, 1};
            s16 midY[4] = {1, 0, -1, 0};
            s16 midX[4] = {0, -1, 0, 1};
            s16 startY[4] = {1, 1, -1, -1};
            s16 startX[4] = {1, -1, -1, 1};
            s16 step = fn_802B0130(n);
            if (fn_802B0130(n) >= 4) {
                fn_802BDFBC(cell, fn_802B0130(n), n->scale);
            }
            s32 used = 0;
            GroundList* bag = 0;
            if (!fn_802B0124(cell)) {
                bag = fn_802B4F10(&used, n->x - step, n->y - step, step * 2, fn_802B0118(cell));
            }
            f32 alpha = Remap(0.0f, 0.6f, 0.0f, 1.0f, n->scale);
            Vec corner[4];
            Vec center;
            fn_802B0114(&corner[0]);
            fn_802B0114(&corner[1]);
            fn_802B0114(&corner[2]);
            fn_802B0114(&corner[3]);
            fn_802B0114(&center);
            Vec* centerp = bag ? &center : 0;

            Vec here;
            StatsUpdate__Fv(&here);
            fn_802B009C(&here, 0, n->x, n->y);
            if (centerp) {
                fn_80144DAC(centerp, &here);
            }
            u16 id;
            if (1.0f != alpha) {
                Vec from;
                StatsUpdate__Fv(&from);
                u16* e = n->edge[n->unk6];
                u16 at = n->unk6 < 2 ? 0 : n->size;
                if (e[at] > 0xFFFD) {
                    s16 px[4] = {-1, 1, 1, -1};
                    s16 py[4] = {-1, -1, 1, 1};
                    fn_802B009C(&from, 0, n->x + px[n->unk6] * fn_802B0130(n), n->y + py[n->unk6] * fn_802B0130(n));
                } else {
                    fn_802B1BF0(lbl_8055F974, &from, e[at]);
                }
                Vec mixed;
                fn_8025AD3C(&mixed, &from, &here, alpha);
                id = fn_802B0070(&mixed);
            } else {
                id = fn_802B0070(&here);
            }
            fn_802B0024(&buf, id);

            u16 prev = n->edge[3][0];
            for (int i = 0; i < 4; i++) {
                u16* e = n->edge[i];
                u16* first = i < 2 ? e : e + n->size;
                u16* last = i >= 2 ? e : e + n->size;
                int mid = n->size / 2;
                if (*first == 0xFFFF) {
                    if (prev < 0xFFFD) {
                        *first = prev;
                    } else {
                        *first = fn_802AFFA8(n->x + step * startX[i], n->y + step * startY[i]);
                    }
                }
                if (bag) {
                    if (*first == 0xFFFE) {
                        bag = 0;
                    } else {
                        fn_802B1BF0(lbl_8055F974, &corner[i], *first);
                        corner[i].x = lbl_80564AF0 * (f32)(n->x + (s16)step * startX[i]);
                        corner[i].z = lbl_80564AF0 * (f32)(n->y + (s16)step * startY[i]);
                        fn_801C0E60(&corner[i], &corner[i], &center);
                    }
                }
                if (e[mid] == 0xFFFF) {
                    s16 my;
                    s16 mx = n->x + step * midX[i];
                    my = n->y + step * midY[i];
                    if (1.0f == alpha) {
                        e[mid] = fn_802AFFA8(mx, my);
                    } else {
                        Vec a;
                        StatsUpdate__Fv(&a);
                        if (*first > 0xFFFD) {
                            fn_802B1BF0(lbl_8055F974, &a, *first);
                        } else {
                            fn_802B009C(&a, 0, n->x + step * startX[i], n->y + step * startY[i]);
                        }
                        Vec b;
                        StatsUpdate__Fv(&b);
                        fn_802B009C(&b, 0, mx, my);
                        Vec m;
                        fn_8025AD3C(&m, &a, &b, alpha);
                        e[mid] = fn_802B0070(&m);
                    }
                }
                if (*last == 0xFFFF) {
                    u16 v = 0;
                    switch (i) {
                    case 0:
                        v = n->edge[1][0];
                        break;
                    case 1:
                        v = n->edge[2][n->size];
                        break;
                    case 2:
                        v = n->edge[3][n->size];
                        break;
                    case 3:
                        v = n->edge[0][0];
                        break;
                    }
                    if (v < 0xFFFD) {
                        *last = v;
                    } else {
                        *last = fn_802AFFA8(n->x + step * endX[i], n->y + step * endY[i]);
                    }
                }
                if (i < 2) {
                    for (int j = 0; j < n->size; j++) {
                        if (*e == 0xFFFF) {
                            *e = 0xFFFE;
                        } else if (*e != 0xFFFE) {
                            fn_802AFF80(&buf, *e);
                        }
                        e++;
                    }
                } else {
                    int j = n->size;
                    for (e += j; j > 0; j--) {
                        if (*e == 0xFFFF) {
                            *e = 0xFFFE;
                        } else if (*e != 0xFFFE) {
                            fn_802AFF80(&buf, *e);
                        }
                        e--;
                    }
                }
                prev = *last;
            }
            fn_802AFF80(&buf, prev);
            fn_802AFF6C(&buf);
            fn_802B13F8(lbl_8055F974, cell, &buf, fn_802B0130(n), alpha);

            if (bag && (fn_802B0130(n) < 8 || (fn_802B0130(n) == 8 && n->scale > 0.5f))) {
                int lod = fn_802B0118(cell);
                for (int j = 0; j < fn_802AFF54(bag); j++) {
                    GroundItem* it = (GroundItem*)fn_802AFF5C(bag, j);
                    if (it->lists[lod]) {
                        Vec pos;
                        StatsUpdate__Fv(&pos);
                        fn_801C2B9C(&pos, &center, &corner[it->corner], it->t0);
                        fn_801C2B9C(&pos, &pos, &corner[(it->corner + 1) & 3], it->t1);
                        if (j < used) {
                            fn_802B3E00(it->lists[lod], &pos, it->kind, 1.0f);
                        } else {
                            fn_802B3E00(it->lists[lod], &pos, it->kind, alpha);
                        }
                    }
                }
            }
        } else {
            GroundNode cur;
            int h = n->size / 2;
            if (h == 4) {
                fn_802BDFBC(cell, h, n->scale);
            }
            cur = *n;
            u16* p;
            u16* m1;
            int q = cur.size / 2;
            u16* m2;
            int o = q / 2;
            p = fn_802AFEC0(q);
            m1 = p + (q + 1);
            m2 = p + (q + 1) * 2;
            u16* m3 = p + (q + 1) * 3;
            fn_802AFEA4(&lbl_804BD8E8, -q);
            // Children go on the stack far-to-near for the view quadrant.
            switch (lbl_8055F988) {
            case 3:
                fn_802AFE64(&lbl_804BD8E8, cur.x - o, cur.y - o, q, 2, blend, m3, cur.edge[1] + q, m1, cur.edge[2] + q);
                fn_802AFE64(&lbl_804BD8E8, cur.x + o, cur.y - o, q, 3, blend, m2, m1, cur.edge[3] + q, cur.edge[2]);
                fn_802AFE64(&lbl_804BD8E8, cur.x - o, cur.y + o, q, 1, blend, cur.edge[0] + q, cur.edge[1], p, m3);
                fn_802AFE64(&lbl_804BD8E8, cur.x + o, cur.y + o, q, 0, blend, cur.edge[0], p, cur.edge[3], m2);
                break;
            case 2:
                fn_802AFE64(&lbl_804BD8E8, cur.x + o, cur.y - o, q, 3, blend, m2, m1, cur.edge[3] + q, cur.edge[2]);
                fn_802AFE64(&lbl_804BD8E8, cur.x + o, cur.y + o, q, 0, blend, cur.edge[0], p, cur.edge[3], m2);
                fn_802AFE64(&lbl_804BD8E8, cur.x - o, cur.y - o, q, 2, blend, m3, cur.edge[1] + q, m1, cur.edge[2] + q);
                fn_802AFE64(&lbl_804BD8E8, cur.x - o, cur.y + o, q, 1, blend, cur.edge[0] + q, cur.edge[1], p, m3);
                break;
            case 1:
                fn_802AFE64(&lbl_804BD8E8, cur.x - o, cur.y + o, q, 1, blend, cur.edge[0] + q, cur.edge[1], p, m3);
                fn_802AFE64(&lbl_804BD8E8, cur.x + o, cur.y + o, q, 0, blend, cur.edge[0], p, cur.edge[3], m2);
                fn_802AFE64(&lbl_804BD8E8, cur.x - o, cur.y - o, q, 2, blend, m3, cur.edge[1] + q, m1, cur.edge[2] + q);
                fn_802AFE64(&lbl_804BD8E8, cur.x + o, cur.y - o, q, 3, blend, m2, m1, cur.edge[3] + q, cur.edge[2]);
                break;
            case 0:
                fn_802AFE64(&lbl_804BD8E8, cur.x + o, cur.y + o, q, 0, blend, cur.edge[0], p, cur.edge[3], m2);
                fn_802AFE64(&lbl_804BD8E8, cur.x + o, cur.y - o, q, 3, blend, m2, m1, cur.edge[3] + q, cur.edge[2]);
                fn_802AFE64(&lbl_804BD8E8, cur.x - o, cur.y + o, q, 1, blend, cur.edge[0] + q, cur.edge[1], p, m3);
                fn_802AFE64(&lbl_804BD8E8, cur.x - o, cur.y - o, q, 2, blend, m3, cur.edge[1] + q, m1, cur.edge[2] + q);
                break;
            }
        }
    }
}

void fn_802AFE64(GroundStack* s, s16 x, s16 y, s16 size, s16 unk6, f32 scale, u16* e0, u16* e1, u16* e3, u16* e2) {
    GroundNode* n = &s->nodes[s->count++];
    n->x = x;
    n->y = y;
    n->size = size;
    n->unk6 = unk6;
    n->scale = scale;
    n->edge[0] = e0;
    n->edge[1] = e1;
    n->edge[2] = e2;
    n->edge[3] = e3;
}

void fn_802AFEA4(GroundStack* s, s16 size) {
    GroundNode* n = &s->nodes[s->count++];
    n->size = size;
}

u16* fn_802AFEC0(int n) {
    u16* p = lbl_8055F984 + lbl_804BDFF0[MaskLevel(n)];
    memset(p, -1, (n + 1) * 8);
    return p;
}

u32 fn_802AFF54(GroundList* l) { return l->count; }

void* fn_802AFF5C(GroundList* l, int i) { return (u8*)l->items + i * 0x24; }

void fn_802AFF6C(GroundBuf* b) { b->data[0] = b->count - 1; }

void fn_802AFF80(GroundBuf* b, u16 v) { b->Add(v); }

u16 fn_802AFFA8(s16 x, s16 y) {
    Vec p;
    p.x = x * lbl_80564AF0;
    p.z = y * lbl_80564AF0;
    p.y = fn_801DCF48(0, p.x, p.z);
    return fn_802B127C(lbl_8055F974, &p);
}

void fn_802B0024(GroundBuf* b, u16 v) {
    b->count = 0;
    b->Add(0);
    b->Add(v);
}

u16 fn_802B0070(const Vec* p) { return fn_802B127C(lbl_8055F974, p); }

void fn_802B009C(Vec* out, int a, s16 x, s16 y) {
    out->x = x * lbl_80564AF0;
    out->z = y * lbl_80564AF0;
    out->y = fn_801DCF48(a, out->x, out->z);
}

void fn_802B0114(Vec* v) {}

u32 fn_802B0118(const GroundCell* c) { return (c->flags >> 6) & 7; }

u32 fn_802B0124(const GroundCell* c) { return (c->flags >> 23) & 1; }

s16 fn_802B0130(const GroundNode* n) { return n->size / 2; }

void fn_802B0144(void) { fn_802AEB3C(fn_801E8718(-1)); }

void fn_802B016C(void) {
    fn_802DBE4C();
    fn_802B14B4(lbl_8055F974);
    fn_802BDD54();
    fn_802B2D94(lbl_8055F978);
    fn_802B4E30();
    fn_802DBECC();
}

void fn_802B01A8(void) {
    fn_802DBE4C();
    ViewFlags* v = fn_801E87A8();
    int mode = 1;
    if (v->hires) {
        mode = 2;
    }
    int old = fn_802C9C58(mode);
    fn_802B14B4(lbl_8055F974);
    fn_802C9C58(old);
    fn_802DBECC();
    fn_802AE728();
    fn_80202718();
    fn_8029790C(0);
    fn_802AE7C0();
}

void fn_802B0218(void) {
    if (fn_802AE7AC()) {
        fn_802DBE4C();
        fn_802B2D94(lbl_8055F978);
        fn_802B4E30();
        fn_802DBECC();
    }
}

void fn_802B0254(void) {
    if (fn_802AE7AC()) {
        fn_802DBE4C();
        fn_802BDD58();
        fn_802DBECC();
    }
}

void fn_802B0288(GroundSet* g) {
    if (lbl_8055F970 != g) {
        fn_802B4E44(g->objects);
    }
    lbl_8055F970 = g;
}

static inline int InWorld(const Vec* p) {
    f32 half = 1024.0f * lbl_80564AF0 / 2.0f;
    int in = 0;
    if (p->x > -half && p->x < half && p->z > -half && p->z < half) {
        in = 1;
    }
    return in;
}

static inline GroundCell* CellAt(const Vec* p) {
    int x = p->x / lbl_80564AF0;
    int z = p->z / lbl_80564AF0;
    return fn_802BE844((s16)(x & ~7) + 4, (s16)(z & ~7) + 4, 4);
}

int fn_802B02C8(const Vec* p) {
    if (fn_802AE7AC()) {
        if (InWorld(p)) {
            return !(CellAt(p)->flags & 0x1000000);
        }
        return 1;
    }
    return 0;
}

int fn_802B03C0(const Vec* p) {
    int r = 0;
    if (fn_802AE7AC() && lbl_8055F970) {
        if (InWorld(p)) {
            u32 f = CellAt(p)->flags;
            r = fn_802B1C50(lbl_8055F970->terrain, (f >> 6) & 7, ((f >> 24) & 1) ^ 1);
        }
    }
    return r;
}

int fn_802B04D4(const Vec* p) {
    if (lbl_8055F981) {
        return fn_801EA0E8(p->x, p->z) < -0.95f;
    }
    if (lbl_8055CD78 >= 0) {
        if (InWorld(p)) {
            int x = p->x / lbl_80564AF0;
            int z = p->z / lbl_80564AF0;
            return fn_802BBD38(x, z, lbl_8055CD78) > (u8)lbl_8055F97C;
        }
        return 1;
    }
    return 0;
}

void fn_802B05D8(void) { fn_802C10C8(); }

s32 fn_802B05DC(void) { return lbl_8055CD78; }

int fn_802B05E4(void) { return lbl_8055F980; }

int fn_802B05EC(void) { return lbl_8055F981; }

}  // extern "C"

const f32 lbl_80564AF0 = 2.0f;
