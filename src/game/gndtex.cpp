// Ground texture renderer (0x802DB2D4-0x802DD4FC): the TEV/texgen setups for the ground layers (up to
// three bound layer textures blended by mode), the indexed triangle-strip emitter the ground builder
// draws through, the per-instance model batcher that lights small objects from the ground lightmap
// (a CPU texel fetch from the 512x512 RGBA8 layer or a per-model texture matrix) and the indirect
// ripple pass. Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
void DCFlushRange(void* p, u32 n);
void fn_802E317C(const char* fmt, ...);  // fatal
double fn_8002F208(double x);            // log10
double ceil(double x);
double pow(double x, double y);
void fn_802CADD0(s32 tex, int stage);
void* fn_802CFF64(u32 size);  // frame allocation
void fn_802D0610(int a);
void fn_802D68C4(int a, int b, int c, int d);
void fn_802D3360(void);
void fn_802DEAC4(void);
void fn_802E0684(void);
void fn_802DEE1C(void);
void fn_802DFB30(void);
void fn_802DFCC0(void);
void fn_802D2D1C(void);
void fn_802E0218(void);
void fn_802E009C(void);
void fn_802E050C(void);
void fn_802E0420(void);
void fn_802DFE88(void);
void fn_802DFD00(void);
void fn_802D2ED8(const void* xf, int i);
void fn_802D2F70(const void* xf);
void fn_802D3D94(int i, f32 m[2][4]);
void fn_802D0E48(int pass);
s32 fn_802CA294(s32 tex);
void fn_802D3DF8(s32 tex, f32 u, f32 v, int scale);
void fn_802D3E88(void);
void fn_802D6B54(struct GModel* m);
void fn_802D6B78(struct GModel* m);
void fn_802D6B7C(struct GModel* m);
void PSMTXScale(f32 m[3][4], f32 x, f32 y, f32 z);
void GXLoadPosMtxImm(f32 m[3][4], u32 id);

struct GXTexObj {
    u32 dummy[8];
};

struct GXColor {
    u8 r, g, b, a;
};

u16 GXGetTexObjWidth(const GXTexObj* obj);
int GXGetTexObjFmt(const GXTexObj* obj);
void* GXGetTexObjData(const GXTexObj* obj);
void GXLoadTexObj(const GXTexObj* obj, int map);
void GXLoadTexMtxImm(const void* m, u32 id, int type);
void GXSetVtxAttrFmt(int fmt, int attr, int cnt, int type, int frac);
void GXClearVtxDesc(void);
void GXSetVtxDesc(int attr, int type);
void GXSetArray(int attr, const void* base, u8 stride);
void GXSetNumChans(int n);
void GXSetChanCtrl(int chan, u8 enable, int amb, int mat, u32 lights, int diff, int attn);
void GXSetNumTexGens(u8 n);
void GXSetNumTevStages(u8 n);
void GXSetNumIndStages(u8 n);
void GXSetIndTexOrder(int stage, int coord, int map);
void GXSetTevIndirect(int stage, int ind, int fmt, int bias, int mtx, int ws, int wt, u8 addPrev, u8 utcLod,
                      int alpha);
void GXSetTevDirect(int stage);
void GXSetBlendMode(int type, int src, int dst, int op);
void GXSetAlphaCompare(int c0, u8 r0, int op, int c1, u8 r1);
void GXSetZMode(u8 test, int func, u8 update);
void GXSetColorUpdate(u8 on);
void GXSetTevOp(int stage, int mode);
void GXSetTevOrder(int stage, int coord, int map, int color);
void GXSetTevColorIn(int stage, int a, int b, int c, int d);
void GXSetTevColorOp(int stage, int op, int bias, int scale, u8 clamp, int reg);
void GXSetTevAlphaIn(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp(int stage, int op, int bias, int scale, u8 clamp, int reg);
void GXSetTevKAlphaSel(int stage, int sel);
void GXSetTevColor(int reg, GXColor c);
void GXSetTevKColor(int reg, GXColor c);
void GXSetTexCoordGen2(int dst, int type, int src, u32 mtx, u8 norm, u32 pt);
void GXBegin(int prim, int fmt, u16 n);
void GXCallDisplayList(const void* list, u32 size);
}

union WGPipe {
    u8 uc;
    u16 us;
    f32 f;
};

#define WGPIPE (*(volatile WGPipe*)0xCC008000)

struct GXTex {
    char name[32];  // offset 0x0
    u8 used;        // offset 0x20
    GXTexObj obj;   // offset 0x24
    u8 pad44[0x34];
    GXTexObj obj2;  // offset 0x78
    u8 pad98[0x34];
    u8 hasAlpha;    // offset 0xCC
    u8 padCD[0x13];
    u32 flags;      // offset 0xE0
    u8 alphaRef;    // offset 0xE4
    void* heap;     // offset 0xE8
    u32 lastFrame;  // offset 0xEC
    u32 uses;       // offset 0xF0
};

struct GModel {
    u8 pad0[8];
    u8 type;     // offset 0x8
    u8 pad9[0x17];
    void* pos;   // offset 0x20
    u8 pad24[4];
    void* uv;    // offset 0x28
    u32 dlSize;  // offset 0x2C
    void* dl;    // offset 0x30
};

struct BVec {
    f32 x;
    f32 y;
    f32 z;
};

struct GXf {
    f32 m[12];
};

extern BVec lbl_804DB068[81];
extern u8 lbl_8055FD24;
extern u32 lbl_8055FD30;
GXTex* fn_802D94DC(void);

template <class T>
struct VecBase {
    VecBase(T* d, int n) {
        data = d;
        cap = n;
        count = 0;
    }
    ~VecBase(void) {
        data = 0;
        count = 0;
        cap = 0;
    }
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
};

template <class T, int N>
struct InlineVec : VecBase<T> {
    InlineVec(void) : VecBase<T>(buf, N) {}
    ~InlineVec(void) {}
    T buf[N];  // offset 0xC
};

namespace {

struct Inst {
    GXf xf;     // offset 0x0
    f32 alpha;  // offset 0x30
};

struct Batch {
    ~Batch(void) {}
    InlineVec<Inst, 10> list;  // offset 0x0
    GModel* model;             // offset 0x214
    u8 pending;                // offset 0x218
};

}  // namespace

static inline u16 TexShort(int v) { return v; }

static inline int PMax(int a, int b) { return a > b ? a : b; }
static inline int PMin(int a, int b) { return a < b ? a : b; }

static inline GXColor Rgba(u8 r, u8 g, u8 b, u8 a) {
    GXColor c;
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = a;
    return c;
}

static inline int Round(f32 x) { return x < 0.0f ? x - 0.5f : 0.5f + x; }

ModuleTag lbl_8055FE90;
u16 lbl_8055FE98 = TexShort(0x100);
u16 lbl_8055FE9A = TexShort(0x405);
u16 lbl_8055FE9C = TexShort(0x401);
u8 lbl_8055FE9E;   // tev setup dirty
s32 lbl_8055FEA0;  // last tev alpha
s32 lbl_8055FEA4;  // bound layers
void* lbl_8055FEA8;  // bound vertex array
s32 lbl_8055FEAC;
u8 lbl_8055FEB0;      // two-pass models
GModel* lbl_8055FEB4;  // last model
u8 lbl_8055FEB8;
u8 lbl_8055FEB9;  // cpu lighting
u8 lbl_8055FEBA;  // depth prepass
u8 lbl_8055FEBB;  // batched models
u8* lbl_8055FEBC;  // lightmap texels
u8 lbl_8055FEC0;   // ripple on
f32 lbl_8055D148 = 1.0f;
GXTex* lbl_804DB468[3];
Batch lbl_804DB480;

extern "C" {

void fn_802DB2D4(void) {
    lbl_8055FEB0 = 0;
    GXTex* t = lbl_804DB468[0];
    lbl_8055FD24 = t->alphaRef;
    GXSetAlphaCompare(4, t->alphaRef, 1, 0, 0);
}

void fn_802DB304(GXTex** out, int mode) {
    switch (mode) {
    case 0:
        out[0] = lbl_804DB468[0];
        break;
    case 2:
        out[0] = lbl_804DB468[0];
        out[1] = lbl_804DB468[1];
        break;
    case 1:
        out[0] = lbl_804DB468[0];
        out[1] = lbl_804DB468[1];
        out[2] = lbl_804DB468[2];
        break;
    }
}

int fn_802DB378(int mode, int i) {
    if (mode == 2 && i == 0) {
        return 4;
    }
    return 0xFF;
}

void fn_802DB394(void) {
    GXSetTevOp(0, 3);
    GXSetTevOp(1, 3);
    GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(1, 0, 7, 7, 7);
}

void fn_802DB3FC(void) {
    GXSetTevOp(0, 3);
    GXSetTevOp(1, 4);
    GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(1, 4, 7, 7, 7);
    GXSetTevOp(2, 0);
    GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(2, 0, 7, 7, 7);
    GXSetTevOp(3, 4);
    GXSetTevAlphaOp(3, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(3, 7, 0, 4, 7);
}

void fn_802DB4E4(void) {
    f32 a[2][4] = {0};
    f32 b[2][4] = {0};
    a[0][0] = lbl_804DB068[1].x;
    a[1][1] = lbl_804DB068[1].y;
    b[0][0] = lbl_804DB068[2].x;
    b[1][1] = lbl_804DB068[2].y;
    GXLoadTexMtxImm(a, 0x1E, 1);
    GXLoadTexMtxImm(b, 0x21, 1);
}

void fn_802DB58C(void) {
    f32 m[2][4] = {{0.0f, 0.0f, 0.0f, 0.5f}, {0.0f, 0.0f, 0.0f, 0.5f}};
    m[0][0] = lbl_804DB068[1].x;
    m[1][1] = lbl_804DB068[1].x;
    GXLoadTexMtxImm(m, 0x1E, 1);
}

void fn_802DB620(void) {
    f32 m[2][4] = {{0.0f, 0.0f, 0.0f, 0.5f}, {0.0f, 0.0f, 0.0f, 0.5f}};
    m[0][0] = lbl_8055D148;
    m[1][2] = lbl_8055D148;
    GXLoadTexMtxImm(m, 0x1E, 1);
}

int fn_802DB69C(GXTex** layers, int mode) {
    int map = 0;
    int stage = 0;
    for (int i = 0; i < lbl_8055FEA4; i++) {
        GXTex* t = *layers;
        int base = map;
        GXLoadTexObj(&t->obj, map++);
        if (t->hasAlpha) {
            int amap = map;
            GXLoadTexObj(&t->obj2, map++);
            GXSetTevOrder(stage++, i, amap, 0xFF);
        }
        int s = stage++;
        GXSetTevOrder(s, i, base, fn_802DB378(mode, i));
        layers++;
    }
    GXSetNumTexGens(lbl_8055FEA4);
    return stage;
}

void fn_802DB778(int mode) {
    if (lbl_8055FE9E) {
        lbl_8055FE9E = 0;
        fn_802DB2D4();
        GXTex* layers[3];
        fn_802DB304(layers, mode);
        int stages = fn_802DB69C(layers, mode);
        switch (mode) {
        case 0:
            fn_802DB394();
            break;
        case 1:
            fn_802DB3FC();
            break;
        case 2:
            GXSetTevColorOp(0, 0, 0, 0, 1, 0);
            GXSetTevColorIn(0, 0xF, 0xA, 8, 0xF);
            GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
            GXSetTevAlphaIn(0, 5, 7, 7, 7);
            if (stages >= 2) {
                GXSetTevColorOp(1, 8, 0, 0, 1, 0);
                GXSetTevColorIn(1, 4, 8, 5, 0);
                GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
                GXSetTevAlphaIn(1, 0, 7, 7, 7);
            }
            break;
        }
        GXSetNumTevStages(stages);
        switch (mode) {
        case 0:
            GXSetTexCoordGen2(0, 1, 0, 0x1E, 0, 0x7D);
            break;
        case 1:
            GXSetTexCoordGen2(0, 1, 0, 0x24, 0, 0x7D);
            GXSetTexCoordGen2(1, 1, 0, 0x1E, 0, 0x7D);
            GXSetTexCoordGen2(2, 1, 0, 0x24, 0, 0x7D);
            break;
        case 2:
            fn_802DB620();
            GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
            GXSetTexCoordGen2(1, 1, 0, 0x1E, 0, 0x7D);
            break;
        }
        GXSetBlendMode(1, 4, 5, 5);
    }
}

void fn_802DB9DC(int first) {
    if (lbl_8055FEBA) {
        return;
    }
    if (first) {
        GXSetBlendMode(1, 4, 0, 5);
    } else {
        GXSetBlendMode(1, 4, 1, 5);
    }
}

void fn_802DBA1C(int prepass) {
    f32 a[2][4] = {{1.0f / 2048, 0.0f, 0.0f, 0.5f}, {0.0f, 0.0f, -1.0f / 2048, 0.4990234375f}};
    GXLoadTexMtxImm(a, 0x24, 1);
    f32 b[2][4] = {{1.0f / 2048, 0.0f, 0.0f, 0.5f}, {0.0f, 0.0f, 1.0f / 2048, 0.5f}};
    GXLoadTexMtxImm(b, 0x27, 1);
    GXSetAlphaCompare(7, 0, 1, 7, 0);
    fn_802DB4E4();
    if (prepass == 0) {
        GXLoadTexObj(&lbl_804DB468[0]->obj, 0);
        GXLoadTexObj(&lbl_804DB468[1]->obj, 1);
        GXLoadTexObj(&lbl_804DB468[2]->obj, 2);
        GXLoadTexObj(&lbl_804DB468[2]->obj2, 3);
        GXLoadTexObj(&lbl_804DB468[3]->obj, 4);
        fn_802DEAC4();
    } else {
        GXLoadTexObj(&lbl_804DB468[0]->obj, 0);
        GXLoadTexObj(&lbl_804DB468[1]->obj, 1);
        GXLoadTexObj(&lbl_804DB468[1]->obj2, 2);
        GXLoadTexObj(&lbl_804DB468[2]->obj, 3);
        GXLoadTexObj(&lbl_804DB468[2]->obj2, 4);
        fn_802E0684();
        GXSetZMode(0, 7, 0);
    }
}

void fn_802DBBE4(void* verts, int n) {
    if (lbl_8055FEA8 != verts) {
        lbl_8055FEA8 = verts;
        lbl_8055FEAC = n;
        GXClearVtxDesc();
        GXSetVtxDesc(9, 3);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetArray(9, verts, 0xC);
        DCFlushRange(verts, n * 0xC);
    }
}

void fn_802DBC70(void* verts, int n) {
    if (lbl_8055FEA8 != verts) {
        lbl_8055FEA8 = verts;
        lbl_8055FEAC = n;
        GXClearVtxDesc();
        GXSetVtxDesc(9, 3);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetArray(9, verts, 0xC);
        DCFlushRange(verts, n * 0xC);
    }
}

void fn_802DBCFC(void* verts, int n) {
    if (lbl_8055FEA8 != verts) {
        lbl_8055FEA8 = verts;
        lbl_8055FEAC = n;
        GXClearVtxDesc();
        GXSetVtxDesc(9, 3);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetArray(9, verts, 0xC);
        DCFlushRange(verts, n * 0xC);
    }
}

void fn_802DBD88(void) {
    lbl_8055FEA8 = 0;
    lbl_8055FEAC = 0;
    lbl_8055FE9E = 1;
    lbl_8055FEA0 = -1;
    lbl_804DB468[0] = fn_802D94DC();
    lbl_8055FEA4 = 1;
}

void fn_802DBDD8(void) {
    lbl_8055FEA8 = 0;
    lbl_8055FEAC = 0;
}

void fn_802DBDE8(int i, GXTex* t) {
    if (t) {
        t->lastFrame = lbl_8055FD30;
        t->uses++;
    }
    lbl_804DB468[i] = t;
    for (lbl_8055FEA4 = 0; lbl_8055FEA4 < 3 && lbl_804DB468[lbl_8055FEA4]; lbl_8055FEA4++) {
    }
    lbl_8055FE9E = 1;
}

void fn_802DBE4C(void) {
    fn_802D0610(0);
    fn_802D68C4(0, 0, 0, 0);
    GXSetZMode(1, 3, 1);
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 1, 1, 0, 0, 2);
    lbl_8055FEB8 = 1;
}

void fn_802DBECC(void) {
    fn_802D3360();
    GXSetTevKAlphaSel(0, 0);
    lbl_8055FEB8 = 0;
}

void fn_802DBF00(u8 prepass) {
    fn_802DBD88();
    lbl_8055FEBA = prepass;
}

void fn_802DBF30(void) {
    fn_802DBDD8();
    fn_802DEE1C();
}

static inline void DrawStrips(const u16* idx, int n) {
    const u16* p = idx;
    int left = n;
    while (left > 0) {
        u16 c = *p++;
        GXBegin(0xA0, 0, c);
        left -= c + 1;
        for (int i = 0; i < c; i++) {
            WGPIPE.us = *p++;
        }
    }
}

void fn_802DBF54(const u16* idx, int n) { DrawStrips(idx, n); }

void fn_802DC078(void* verts, int nverts, const u16* idx, int n, int first, int layer) {
    fn_802DBA1C(lbl_8055FEBA);
    fn_802DB9DC(first);
    fn_802DBBE4(verts, nverts);
    fn_802DBF54(idx, n);
    if (lbl_8055FEBA) {
        GXSetZMode(1, 3, 1);
        GXSetAlphaCompare(7, 0, 1, 7, 0);
        GXSetColorUpdate(0);
        fn_802DBF54(idx, n);
        GXSetColorUpdate(1);
    }
}

void fn_802DC130(void) { fn_802DBD88(); }

void fn_802DC134(void) { fn_802DBDD8(); }

void fn_802DC138(void* verts, int nverts, const u16* idx, int n) {
    fn_802DB778(0);
    fn_802DBCFC(verts, nverts);
    DrawStrips(idx, n);
}

void fn_802DC278(void) { fn_802DBD88(); }

void fn_802DC27C(void) { fn_802DBDD8(); }

void fn_802DC280(void* verts, int nverts, const u16* idx, int n) {
    fn_802DB778(1);
    fn_802DBC70(verts, nverts);
    DrawStrips(idx, n);
}

void fn_802DC3C0(s32 tex) {
    fn_802DBD88();
    fn_802DB58C();
    fn_802CADD0(tex, 1);
    GXLoadTexObj(&lbl_804DB468[0]->obj, 0);
    GXLoadTexObj(&lbl_804DB468[1]->obj, 1);
    GXSetAlphaCompare(7, 0, 1, 7, 0);
    fn_802DFB30();
}

void fn_802DC440(void) {
    fn_802DBDD8();
    fn_802DFCC0();
}

void fn_802DC464(void* verts, int nverts, const u16* idx, int n) {
    fn_802DBBE4(verts, nverts);
    u8 a = PMin(PMax(Round(255.0f * lbl_804DB068[1].z), 0), 0xFF);
    if (a != lbl_8055FEA0) {
        GXColor c = Rgba(0xFF, 0xFF, 0xFF, a);
        lbl_8055FEA0 = a;
        GXSetTevColor(1, c);
    }
    DrawStrips(idx, n);
}

GModel* fn_802DC628(GModel* m) { return m; }

void fn_802DC62C(void) {
    GXTex* t = lbl_804DB468[0];
    if (GXGetTexObjFmt(&t->obj) != 6) {
        fn_802E317C("I only cpu-light from 8888 textures");
    }
    if (GXGetTexObjWidth(&t->obj) != 0x200) {
        fn_802E317C("I only cpu-light from 512x512 textures");
    }
    lbl_8055FEBC = (u8*)GXGetTexObjData(&t->obj) + 0x80000000;
}

u32 fn_802DC6AC(int x, int y) {
    u16* p = (u16*)(lbl_8055FEBC + ((x / 4) + (y / 4) * 128) * 64 + ((x % 4) + (y % 4) * 4) * 16 / 8);
    return (p[0] << 16) | p[0x10];
}

void fn_802DC71C(GModel* model, s32 tex) {
    fn_802DBD88();
    lbl_8055FEB4 = 0;
    fn_802CADD0(tex, 1);
    fn_802D2D1C();
    GXTex* t = lbl_804DB468[1];
    u8 hasAlpha = t->hasAlpha;
    lbl_8055FEBB = fn_802DC628(model)->type == 0x1D ? 1 : 0;
    lbl_8055FEB9 = GXGetTexObjWidth(&lbl_804DB468[0]->obj) == 0x200 ? 1 : 0;
    GXLoadTexObj(&lbl_804DB468[0]->obj, 0);
    GXLoadTexObj(&t->obj, 1);
    if (hasAlpha) {
        GXLoadTexObj(&t->obj2, 2);
    }
    if (lbl_8055FEBB) {
        if (hasAlpha) {
            fn_802E0218();
        } else {
            fn_802E009C();
        }
        lbl_8055FEB0 = 0;
    } else {
        if (lbl_8055FEB9) {
            if (hasAlpha) {
                fn_802E050C();
            } else {
                fn_802E0420();
            }
            fn_802DC62C();
        } else if (hasAlpha) {
            fn_802DFE88();
        } else {
            fn_802DFD00();
        }
        lbl_8055FEB0 = (t->flags >> 3) & 1;
    }
    GXSetAlphaCompare(4, t->alphaRef, 1, 0, 0);
    lbl_8055FD24 = t->alphaRef;
    lbl_804DB480.list.count = 0;
    lbl_804DB480.model = 0;
    lbl_804DB480.pending = 1;
    GXSetBlendMode(1, 4, 5, 5);
}

static inline void FlushBatch(void) {
    u32* colors = (u32*)fn_802CFF64(0x28);
    f32* uvs = (f32*)fn_802CFF64(0x50);
    if (colors && uvs) {
        if (lbl_804DB480.pending) {
            fn_802D6B54(lbl_804DB480.model);
        }
        GModel* m = fn_802DC628(lbl_804DB480.model);
        int n = lbl_804DB480.list.count;
        int i;
        for (i = 0; i < n; i++) {
            Inst* e = &lbl_804DB480.list.data[i];
            u8 a = 255.0f * e->alpha;
            colors[i] = 0xFFFFFF00 | (u8)PMin(PMax(a, 0), 0xFF);
            uvs[i * 2] = 0.5f + e->xf.m[9] / 2048.0f;
            uvs[i * 2 + 1] = 0.5f + e->xf.m[11] / 2048.0f;
            fn_802D2ED8(e, i);
        }
        if (i < 10) {
            f32 zero[3][4];
            PSMTXScale(zero, 0.0f, 0.0f, 0.0f);
            for (; i < 10; i++) {
                GXLoadPosMtxImm(zero, i * 3);
            }
        }
        if (lbl_804DB480.pending) {
            GXSetArray(9, m->pos, 0xC);
            GXSetArray(0xD, m->uv, 4);
        }
        GXSetArray(0xB, colors, 4);
        GXSetArray(0xE, uvs, 8);
        GXCallDisplayList(m->dl, m->dlSize);
        lbl_804DB480.pending = 0;
    }
    lbl_804DB480.list.count = 0;
    lbl_804DB480.model = 0;
}

void fn_802DC8B4(void) {
    if (lbl_804DB480.list.count > 0) {
        FlushBatch();
    }
    fn_802CADD0(-1, 1);
    fn_802DBDD8();
}

void fn_802DCAE4(GModel* model, const GXf* xf, f32 alpha) {
    if (lbl_8055FEBB) {
        lbl_804DB480.model = model;
        Inst* e = &lbl_804DB480.list.data[lbl_804DB480.list.count++];
        e->xf = *xf;
        e->alpha = alpha;
        if (lbl_804DB480.list.count + 1 > lbl_804DB480.list.cap) {
            FlushBatch();
        }
        return;
    }
    if (lbl_8055FEB9) {
        int a = PMin(PMax(Round(255.0f * alpha), 0), 0xFF);
        u32 c = fn_802DC6AC((int)xf->m[9] / 4 + 0x100, (int)xf->m[11] / 4 + 0x100);
        GXColor k;
        k.r = c >> 16;
        k.g = c >> 8;
        k.b = c;
        k.a = a;
        GXSetTevKColor(1, k);
    } else {
        f32 m[2][4];
        m[0][0] = 0.0f;
        m[0][1] = 0.0f;
        m[0][2] = 0.0f;
        m[0][3] = 0.5f + xf->m[9] / 2048.0f;
        m[1][0] = 0.0f;
        m[1][1] = 0.0f;
        m[1][2] = 0.0f;
        m[1][3] = 0.5f + xf->m[11] / 2048.0f;
        fn_802D3D94(0, m);
        u8 a = PMin(PMax(Round(255.0f * alpha), 0), 0xFF);
        if (a != lbl_8055FEA0) {
            GXColor k = Rgba(0xFF, 0xFF, 0xFF, a);
            GXSetTevKColor(1, k);
        }
    }
    fn_802D2F70(xf);
    if (lbl_8055FEB4 != model) {
        lbl_8055FEB4 = model;
        if (lbl_8055FEB0) {
            fn_802D6B7C(model);
        } else {
            fn_802D6B78(model);
        }
    } else if (lbl_8055FEB0) {
        fn_802D0E48(0);
        GXCallDisplayList(model->dl, model->dlSize);
        fn_802D0E48(1);
        GXCallDisplayList(model->dl, model->dlSize);
    } else {
        GXCallDisplayList(model->dl, model->dlSize);
    }
}

static inline f32 Log10(f32 x) { return fn_8002F208(x); }

void fn_802DCFCC(void* ctx, s32 ind, s32 tex, f32 scale, f32 uvScale) {
    lbl_8055D148 = uvScale;
    fn_802DBD88();
    if (tex != -2) {
        fn_802CADD0(tex, 1);
    }
    fn_802DB778(2);
    if (ind != -2) {
        lbl_8055FEC0 = 1;
        scale = 5.0f * scale;
        f32 l = Log10(scale);
        f32 d = Log10(128.0f) - l;
        f32 e = ceil(0.1f + (f32)__fabs(d));
        int s = (d > 0.0f ? 1.0f : -1.0f) * e;
        s = PMin(PMax(s, -46), 17);
        f32 p = pow(2.0, (f32)s);
        scale = (scale * 0.0078125f) / p;
        fn_802D3DF8(fn_802CA294(ind), scale, scale, s);
        GXSetNumIndStages(1);
        GXSetTevIndirect(0, 0, 0, 3, 1, 0, 0, 0, 0, 0);
        GXSetIndTexOrder(0, lbl_8055FEA4, 2);
        GXSetNumTexGens(lbl_8055FEA4 + 1);
        GXSetTexCoordGen2(lbl_8055FEA4, 1, 0, 0x24, 0, 0x7D);
    }
    GXClearVtxDesc();
    GXSetVtxDesc(9, 3);
    GXSetVtxDesc(0xB, 3);
    GXSetVtxDesc(0xD, 3);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    GXColor c = Rgba(0xC0, 0xC0, 0xC0, 0x14);
    GXSetTevColor(2, c);
}

void fn_802DD228(void) {
    if (lbl_8055FEC0) {
        fn_802D3E88();
        lbl_8055FEC0 = 0;
    }
    fn_802DBDD8();
    fn_802CADD0(-1, 1);
    GXSetNumIndStages(0);
    GXSetTevDirect(0);
}

void fn_802DD27C(u8* verts, int n, const u16* idx, int count) {
    GXSetArray(9, verts, 0x18);
    GXSetArray(0xB, verts + 0xC, 0x18);
    GXSetArray(0xD, verts + 0x10, 0x18);
    DCFlushRange(verts, n * 0x18);
    GXBegin(0x90, 0, count);
    for (int i = 0; i < count; i++) {
        u16 k = idx[i];
        WGPIPE.us = k;
        WGPIPE.us = k;
        WGPIPE.us = k;
    }
}

void* fn_802DD42C(u32 size) { return fn_802CFF64(size); }

}  // extern "C"
