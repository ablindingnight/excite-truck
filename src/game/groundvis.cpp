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
    f32 scale;  // offset 0x8
    u32 a;      // offset 0xC
    u32 b;      // offset 0x10
    u32 d;      // offset 0x14
    u32 c;      // offset 0x18
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
f32 lbl_8055F96C = QuarterPi();
GroundSet* lbl_8055F970;   // active ground set
u32 lbl_8055F974;          // terrain
u32 lbl_8055F978;          // objects
s32 lbl_8055F97C;          // water cell value
u8 lbl_8055F980;           // has a swell map
u8 lbl_8055F981;           // crystal mode
s32 lbl_8055CD78 = -1;     // water layer
s16* lbl_8055F984;         // visibility masks
s32 lbl_8055F988;          // view quadrant
f32 lbl_8055F98C;

static GroundStack lbl_804BD8E8;
static s32 lbl_804BDFF0[40];  // per-level mask offsets
static struct {
    GroundFrame cam;
    f32 vals[14];
} lbl_804BE090;

extern "C" {

void fn_802AE64C(s16** p);

void fn_802AE5E0(int n) {
    s32* p = &lbl_804BDFF0[1];
    int ofs = 0;
    for (int s = 2; s <= n; s *= 2) {
        *p = ofs;
        ofs += (s + 1) * 4;
        p++;
    }
    lbl_8055F984 = new ("wtab") s16[ofs];
}

void fn_802AE644(void) { fn_802AE64C(&lbl_8055F984); }

void fn_802AE64C(s16** p) {
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
    return sqrt(dx * dx + dz * dz);
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
    f32* out = &lbl_804BE090.vals[1];
    int s = 2;
    for (int i = 0; i < 7; i++) {
        *out = sum;
        f32 d = diag * (f32)(s * 3);
        sum += d > dist[i + 1] ? d : dist[i + 1];
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

// NonMatching: the quadtree walk is not written yet.
void fn_802AEB3C(const GroundFrame* cam) {
    fn_802AEAAC();
    lbl_804BE090.cam = *cam;
    fn_802AE690(cam);
}

void fn_802AFE64(GroundStack* s, s16 x, s16 y, s16 size, s16 unk6, f32 scale, u32 a, u32 b, u32 c, u32 d) {
    GroundNode* n = &s->nodes[s->count++];
    n->x = x;
    n->y = y;
    n->size = size;
    n->unk6 = unk6;
    n->scale = scale;
    n->a = a;
    n->b = b;
    n->d = d;
    n->c = c;
}

void fn_802AFEA4(GroundStack* s, s16 size) {
    GroundNode* n = &s->nodes[s->count++];
    n->size = size;
}

s16* fn_802AFEC0(int n) {
    int lvl;
    if (n >= 2) {
        union {
            f32 f;
            u32 u;
        } v;
        v.f = n;
        lvl = ((v.u >> 23) & 0x7F) + 1;
    } else {
        lvl = 0;
    }
    s16* p = lbl_8055F984 + lbl_804BDFF0[lvl];
    memset(p, -1, (n + 1) * 8);
    return p;
}

u32 fn_802AFF54(GroundList* l) { return l->count; }

void* fn_802AFF5C(GroundList* l, int i) { return (u8*)l->items + i * 0x24; }

void fn_802AFF6C(GroundBuf* b) { b->data[0] = b->count - 1; }

void fn_802AFF80(GroundBuf* b, u16 v) { b->Add(v); }

u16 fn_802AFFA8(int x, int y) {
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

void fn_802B009C(Vec* out, int a, int x, int y) {
    out->x = x * lbl_80564AF0;
    out->z = y * lbl_80564AF0;
    out->y = fn_801DCF48(a, out->x, out->z);
}

void fn_802B0114(void) {}

u32 fn_802B0118(const GroundCell* c) { return (c->flags >> 6) & 7; }

u32 fn_802B0124(const GroundCell* c) { return (c->flags >> 23) & 1; }

int fn_802B0130(const GroundNode* n) { return n->size / 2; }

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
