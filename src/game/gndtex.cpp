// Ground texture renderer (0x802DB2D4-0x802DD4FC): the TEV/texgen setups for the ground layers (up to
// three bound layer textures blended by mode), the indexed triangle-strip emitter the ground builder
// draws through, the per-instance model batcher that lights small objects from the ground lightmap
// (a CPU texel fetch from the 512x512 RGBA8 layer or a per-model texture matrix) and the indirect
// ripple pass. Names are guesses apart from the strings.
#include "game/common.h"
#include <revolution/GX/GXTypes.h>
#include <revolution/GX/GXAttr.h>
#include <revolution/GX/GXBump.h>
#include <revolution/GX/GXDisplayList.h>
#include <revolution/GX/GXGeometry.h>
#include <revolution/GX/GXLight.h>
#include <revolution/GX/GXPixel.h>
#include <revolution/GX/GXTev.h>
#include <revolution/GX/GXTexture.h>
#include <revolution/GX/GXTransform.h>
#include <revolution/MTX/mtx.h>
#include <revolution/OS/OSCache.h>
#include "game/plainvec.h"

extern "C" {
void fn_802E317C(const char* fmt, ...);  // fatal
double log2(double x);
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

// GXTexObj: the SDK's.

// GXColor: the SDK's.

void* GXGetTexObjData(const GXTexObj* obj);
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

struct GXf {
    f32 m[12];
};

extern BVec lbl_804DB068[81];
extern u8 lbl_8055FD24;
extern u32 lbl_8055FD30;
extern "C" GXTex* fn_802D94DC(void);

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
    GXSetAlphaCompare((GXCompare)4, (u8)t->alphaRef, (GXAlphaOp)1, (GXCompare)0, (u8)0);
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
    int r = 0xFF;
    if (mode == 2) {
        if (i == 0) {
            r = 4;
        }
    }
    return r;
}

void fn_802DB394(void) {
    GXSetTevOp((GXTevStageID)0, (GXTevMode)3);
    GXSetTevOp((GXTevStageID)1, (GXTevMode)3);
    GXSetTevAlphaOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)1, (GXTevAlphaArg)0, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7);
}

void fn_802DB3FC(void) {
    GXSetTevOp((GXTevStageID)0, (GXTevMode)3);
    GXSetTevOp((GXTevStageID)1, (GXTevMode)4);
    GXSetTevAlphaOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)1, (GXTevAlphaArg)4, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7);
    GXSetTevOp((GXTevStageID)2, (GXTevMode)0);
    GXSetTevAlphaOp((GXTevStageID)2, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)2, (GXTevAlphaArg)0, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7);
    GXSetTevOp((GXTevStageID)3, (GXTevMode)4);
    GXSetTevAlphaOp((GXTevStageID)3, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)3, (GXTevAlphaArg)7, (GXTevAlphaArg)0, (GXTevAlphaArg)4, (GXTevAlphaArg)7);
}

void fn_802DB4E4(void) {
    f32 a[2][4] = {0};
    a[0][0] = lbl_804DB068[1].x;
    a[1][2] = lbl_804DB068[1].y;
    f32 b[2][4] = {0};
    b[0][0] = lbl_804DB068[2].x;
    b[1][2] = lbl_804DB068[2].y;
    GXLoadTexMtxImm((const f32 (*)[4])(a), (u32)0x1E, (GXMtxType)1);
    GXLoadTexMtxImm((const f32 (*)[4])(b), (u32)0x21, (GXMtxType)1);
}

void fn_802DB58C(void) {
    f32 s = lbl_804DB068[1].x;
    f32 m[2][4] = {{0.0f, 0.0f, 0.0f, 0.5f}, {0.0f, 0.0f, 0.0f, 0.5f}};
    m[0][0] = s;
    m[1][2] = s;
    GXLoadTexMtxImm((const f32 (*)[4])(m), (u32)0x1E, (GXMtxType)1);
}

void fn_802DB620(void) {
    f32 m[2][4] = {{0.0f, 0.0f, 0.0f, 0.5f}, {0.0f, 0.0f, 0.0f, 0.5f}};
    m[0][0] = lbl_8055D148;
    m[1][2] = lbl_8055D148;
    GXLoadTexMtxImm((const f32 (*)[4])(m), (u32)0x1E, (GXMtxType)1);
}

int fn_802DB69C(GXTex** layers, int mode) {
    GXTex** p = layers;
    int stage = 0;
    int map = 0;
    GXTex* t;
    for (int i = 0; i < lbl_8055FEA4; i++) {
        t = *p;
        int base = map;
        GXLoadTexObj((const GXTexObj*)&t->obj, (GXTexMapID)(map++));
        if (t->hasAlpha) {
            int amap = map;
            GXLoadTexObj((const GXTexObj*)&t->obj2, (GXTexMapID)(map++));
            GXSetTevOrder((GXTevStageID)(stage++), (GXTexCoordID)i, (GXTexMapID)amap, (GXChannelID)0xFF);
        }
        int s = stage++;
        GXSetTevOrder((GXTevStageID)s, (GXTexCoordID)i, (GXTexMapID)base, (GXChannelID)(fn_802DB378(mode, i)));
        p++;
    }
    GXSetNumTexGens((u8)lbl_8055FEA4);
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
            GXSetTevColorOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
            GXSetTevColorIn((GXTevStageID)0, (GXTevColorArg)0xF, (GXTevColorArg)0xA, (GXTevColorArg)8, (GXTevColorArg)0xF);
            GXSetTevAlphaOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
            GXSetTevAlphaIn((GXTevStageID)0, (GXTevAlphaArg)5, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7);
            if (stages >= 2) {
                GXSetTevColorOp((GXTevStageID)1, (GXTevOp)8, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
                GXSetTevColorIn((GXTevStageID)1, (GXTevColorArg)4, (GXTevColorArg)8, (GXTevColorArg)5, (GXTevColorArg)0);
                GXSetTevAlphaOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
                GXSetTevAlphaIn((GXTevStageID)1, (GXTevAlphaArg)0, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7);
            }
            break;
        }
        GXSetNumTevStages((u8)stages);
        switch (mode) {
        case 0:
            GXSetTexCoordGen2((GXTexCoordID)0, (GXTexGenType)1, (GXTexGenSrc)0, (u32)0x1E, (GXBool)0, (u32)0x7D);
            break;
        case 1:
            GXSetTexCoordGen2((GXTexCoordID)0, (GXTexGenType)1, (GXTexGenSrc)0, (u32)0x24, (GXBool)0, (u32)0x7D);
            GXSetTexCoordGen2((GXTexCoordID)1, (GXTexGenType)1, (GXTexGenSrc)0, (u32)0x1E, (GXBool)0, (u32)0x7D);
            GXSetTexCoordGen2((GXTexCoordID)2, (GXTexGenType)1, (GXTexGenSrc)0, (u32)0x24, (GXBool)0, (u32)0x7D);
            break;
        case 2:
            fn_802DB620();
            GXSetTexCoordGen2((GXTexCoordID)0, (GXTexGenType)1, (GXTexGenSrc)4, (u32)0x3C, (GXBool)0, (u32)0x7D);
            GXSetTexCoordGen2((GXTexCoordID)1, (GXTexGenType)1, (GXTexGenSrc)0, (u32)0x1E, (GXBool)0, (u32)0x7D);
            break;
        }
        GXSetBlendMode((GXBlendMode)1, (GXBlendFactor)4, (GXBlendFactor)5, (GXLogicOp)5);
    }
}

void fn_802DB9DC(int first) {
    if (lbl_8055FEBA) {
        return;
    }
    if (first) {
        GXSetBlendMode((GXBlendMode)1, (GXBlendFactor)4, (GXBlendFactor)0, (GXLogicOp)5);
    } else {
        GXSetBlendMode((GXBlendMode)1, (GXBlendFactor)4, (GXBlendFactor)1, (GXLogicOp)5);
    }
}

void fn_802DBA1C(int prepass) {
    f32 a[2][4] = {{1.0f / 2048, 0.0f, 0.0f, 0.5009765625f}, {0.0f, 0.0f, -1.0f / 2048, 0.4990234375f}};
    GXLoadTexMtxImm((const f32 (*)[4])(a), (u32)0x24, (GXMtxType)1);
    f32 b[2][4] = {{1.0f / 2048, 0.0f, 0.0f, 0.5f}, {0.0f, 0.0f, 1.0f / 2048, 0.5f}};
    GXLoadTexMtxImm((const f32 (*)[4])(b), (u32)0x27, (GXMtxType)1);
    GXSetAlphaCompare((GXCompare)7, (u8)0, (GXAlphaOp)1, (GXCompare)7, (u8)0);
    fn_802DB4E4();
    if (prepass == 0) {
        GXLoadTexObj((const GXTexObj*)&lbl_804DB468[0]->obj, (GXTexMapID)0);
        GXLoadTexObj((const GXTexObj*)&lbl_804DB468[1]->obj, (GXTexMapID)1);
        GXLoadTexObj((const GXTexObj*)&lbl_804DB468[2]->obj, (GXTexMapID)2);
        GXLoadTexObj((const GXTexObj*)&lbl_804DB468[2]->obj2, (GXTexMapID)3);
        GXLoadTexObj((const GXTexObj*)&lbl_804DB468[3]->obj, (GXTexMapID)4);
        fn_802DEAC4();
    } else {
        GXLoadTexObj((const GXTexObj*)&lbl_804DB468[0]->obj, (GXTexMapID)0);
        GXLoadTexObj((const GXTexObj*)&lbl_804DB468[1]->obj, (GXTexMapID)1);
        GXLoadTexObj((const GXTexObj*)&lbl_804DB468[1]->obj2, (GXTexMapID)2);
        GXLoadTexObj((const GXTexObj*)&lbl_804DB468[2]->obj, (GXTexMapID)3);
        GXLoadTexObj((const GXTexObj*)&lbl_804DB468[2]->obj2, (GXTexMapID)4);
        fn_802E0684();
        GXSetZMode((GXBool)0, (GXCompare)7, (GXBool)0);
    }
}

void fn_802DBBE4(void* verts, int n) {
    if (lbl_8055FEA8 != verts) {
        lbl_8055FEA8 = verts;
        lbl_8055FEAC = n;
        GXClearVtxDesc();
        GXSetVtxDesc((GXAttr)9, (GXAttrType)3);
        GXSetVtxAttrFmt((GXVtxFmt)0, (GXAttr)9, (GXCompCnt)1, (GXCompType)4, (u8)0);
        GXSetArray((GXAttr)9, (const void*)verts, (u8)0xC);
        DCFlushRange((const void*)verts, (u32)(n * 0xC));
    }
}

void fn_802DBC70(void* verts, int n) {
    if (lbl_8055FEA8 != verts) {
        lbl_8055FEA8 = verts;
        lbl_8055FEAC = n;
        GXClearVtxDesc();
        GXSetVtxDesc((GXAttr)9, (GXAttrType)3);
        GXSetVtxAttrFmt((GXVtxFmt)0, (GXAttr)9, (GXCompCnt)1, (GXCompType)4, (u8)0);
        GXSetArray((GXAttr)9, (const void*)verts, (u8)0xC);
        DCFlushRange((const void*)verts, (u32)(n * 0xC));
    }
}

void fn_802DBCFC(void* verts, int n) {
    if (lbl_8055FEA8 != verts) {
        lbl_8055FEA8 = verts;
        lbl_8055FEAC = n;
        GXClearVtxDesc();
        GXSetVtxDesc((GXAttr)9, (GXAttrType)3);
        GXSetVtxAttrFmt((GXVtxFmt)0, (GXAttr)9, (GXCompCnt)1, (GXCompType)4, (u8)0);
        GXSetArray((GXAttr)9, (const void*)verts, (u8)0xC);
        DCFlushRange((const void*)verts, (u32)(n * 0xC));
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
    GXSetZMode((GXBool)1, (GXCompare)3, (GXBool)1);
    GXSetNumChans((u8)1);
    GXSetChanCtrl((GXChannelID)4, (GXBool)0, (GXColorSrc)1, (GXColorSrc)1, (GXLightID)0, (GXDiffuseFn)0, (GXAttnFn)2);
    lbl_8055FEB8 = 1;
}

void fn_802DBECC(void) {
    fn_802D3360();
    GXSetTevKAlphaSel((GXTevStageID)0, (GXTevKAlphaSel)0);
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

void fn_802DBF54(const u16* idx, int n) {
    while (n > 0) {
        u16 c = *idx++;
        GXBegin((GXPrimitive)0xA0, (GXVtxFmt)0, (u16)c);
        n -= c + 1;
        for (int i = 0; i < c; i++) {
            WGPIPE.us = *idx++;
        }
    }
}

void fn_802DC078(void* verts, int nverts, const u16* idx, int n, int first, int layer) {
    fn_802DBA1C(lbl_8055FEBA);
    fn_802DB9DC(first);
    fn_802DBBE4(verts, nverts);
    fn_802DBF54(idx, n);
    if (lbl_8055FEBA) {
        GXSetZMode((GXBool)1, (GXCompare)3, (GXBool)1);
        GXSetAlphaCompare((GXCompare)7, (u8)0, (GXAlphaOp)1, (GXCompare)7, (u8)0);
        GXSetColorUpdate((GXBool)0);
        fn_802DBF54(idx, n);
        GXSetColorUpdate((GXBool)1);
    }
}

void fn_802DC130(void) { fn_802DBD88(); }

void fn_802DC134(void) { fn_802DBDD8(); }

void fn_802DC138(void* verts, int nverts, const u16* idx, int n) {
    fn_802DB778(0);
    fn_802DBCFC(verts, nverts);
    const u16* p = idx;
    int left = n;
    while (left > 0) {
        u16 c = *p++;
        GXBegin((GXPrimitive)0xA0, (GXVtxFmt)0, (u16)c);
        left -= c + 1;
        for (int i = 0; i < c; i++) {
            WGPIPE.us = *p++;
        }
    }
}

void fn_802DC278(void) { fn_802DBD88(); }

void fn_802DC27C(void) { fn_802DBDD8(); }

void fn_802DC280(void* verts, int nverts, const u16* idx, int n) {
    fn_802DB778(1);
    fn_802DBC70(verts, nverts);
    const u16* p = idx;
    int left = n;
    while (left > 0) {
        u16 c = *p++;
        GXBegin((GXPrimitive)0xA0, (GXVtxFmt)0, (u16)c);
        left -= c + 1;
        for (int i = 0; i < c; i++) {
            WGPIPE.us = *p++;
        }
    }
}

void fn_802DC3C0(s32 tex) {
    fn_802DBD88();
    fn_802DB58C();
    fn_802CADD0(tex, 1);
    GXLoadTexObj((const GXTexObj*)&lbl_804DB468[0]->obj, (GXTexMapID)0);
    GXLoadTexObj((const GXTexObj*)&lbl_804DB468[1]->obj, (GXTexMapID)1);
    GXSetAlphaCompare((GXCompare)7, (u8)0, (GXAlphaOp)1, (GXCompare)7, (u8)0);
    fn_802DFB30();
}

void fn_802DC440(void) {
    fn_802DBDD8();
    fn_802DFCC0();
}

void fn_802DC464(void* verts, int nverts, const u16* idx, int n) {
    fn_802DBBE4(verts, nverts);
    f32 opacity = lbl_804DB068[1].z;
    u8 a = PMin(PMax(Round(255.0f * opacity), 0), 0xFF);
    if (a != lbl_8055FEA0) {
        GXColor c;
        c.a = a;
        c.b = 0xFF;
        c.g = 0xFF;
        c.r = 0xFF;
        lbl_8055FEA0 = a;
        GXSetTevColor((GXTevRegID)1, (GXColor)c);
    }
    const u16* p = idx;
    int left = n;
    while (left > 0) {
        u16 c = *p++;
        GXBegin((GXPrimitive)0xA0, (GXVtxFmt)0, (u16)c);
        left -= c + 1;
        for (int i = 0; i < c; i++) {
            WGPIPE.us = *p++;
        }
    }
}

GModel* fn_802DC628(GModel* m) { return m; }

void fn_802DC62C(void) {
    GXTex* t = lbl_804DB468[0];
    if (GXGetTexObjFmt((const GXTexObj*)&t->obj) != 6) {
        fn_802E317C("I only cpu-light from 8888 textures");
    }
    if (GXGetTexObjWidth((const GXTexObj*)&t->obj) != 0x200) {
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
    const GXTex* t = lbl_804DB468[1];
    u8 hasAlpha = t->hasAlpha;
    lbl_8055FEBB = fn_802DC628(model)->type == 0x1D ? 1 : 0;
    lbl_8055FEB9 = GXGetTexObjWidth((const GXTexObj*)&lbl_804DB468[0]->obj) == 0x200 ? 1 : 0;
    GXLoadTexObj((const GXTexObj*)&lbl_804DB468[0]->obj, (GXTexMapID)0);
    GXLoadTexObj((const GXTexObj*)&t->obj, (GXTexMapID)1);
    if (hasAlpha) {
        GXLoadTexObj((const GXTexObj*)&t->obj2, (GXTexMapID)2);
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
    GXSetAlphaCompare((GXCompare)4, (u8)t->alphaRef, (GXAlphaOp)1, (GXCompare)0, (u8)0);
    lbl_8055FD24 = t->alphaRef;
    lbl_804DB480.list.count = 0;
    lbl_804DB480.model = 0;
    lbl_804DB480.pending = 1;
    GXSetBlendMode((GXBlendMode)1, (GXBlendFactor)4, (GXBlendFactor)5, (GXLogicOp)5);
}

struct BatchUV {
    f32 u, v;
};

static inline void FlushBatch(void) {
    u32* colors = (u32*)fn_802CFF64(0x28);
    BatchUV* uvs = (BatchUV*)fn_802CFF64(0x50);
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
            uvs[i].u = 0.5f + e->xf.m[9] / 2048.0f;
            uvs[i].v = 0.5f + e->xf.m[11] / 2048.0f;
            fn_802D2ED8(e, i);
        }
        if (i < 10) {
            f32 zero[3][4];
            PSMTXScale((f32 (*)[4])(zero), 0.0f, 0.0f, 0.0f);
            for (; i < 10; i++) {
                GXLoadPosMtxImm((const f32 (*)[4])(zero), (u32)(i * 3));
            }
        }
        if (lbl_804DB480.pending) {
            GXSetArray((GXAttr)9, (const void*)m->pos, (u8)0xC);
            GXSetArray((GXAttr)0xD, (const void*)m->uv, (u8)4);
        }
        GXSetArray((GXAttr)0xB, (const void*)colors, (u8)4);
        GXSetArray((GXAttr)0xE, (const void*)uvs, (u8)8);
        GXCallDisplayList((void*)m->dl, (u32)m->dlSize);
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
        GXSetTevKColor((GXTevKColorID)1, (GXColor)k);
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
            GXColor k;
            k.a = a;
            k.r = k.g = k.b = 0xFF;
            GXSetTevKColor((GXTevKColorID)1, (GXColor)k);
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
        GXCallDisplayList((void*)model->dl, (u32)model->dlSize);
        fn_802D0E48(1);
        GXCallDisplayList((void*)model->dl, (u32)model->dlSize);
    } else {
        GXCallDisplayList((void*)model->dl, (u32)model->dlSize);
    }
}

static inline f32 Log2(f32 x) { return log2(x); }

#pragma push
#pragma opt_lifetimes off
void fn_802DCFCC(void* ctx, f32 scale, f32 uvScale, s32 ind, s32 tex, u32) {
    lbl_8055D148 = uvScale;
    fn_802DBD88();
    if (tex != -2) {
        fn_802CADD0(tex, 1);
    }
    fn_802DB778(2);
    if (ind != -2) {
        lbl_8055FEC0 = 1;
        scale = 5.0f * scale;
        f32 l = Log2(scale);
        f32 d = Log2(128.0f) - l;
        f32 e = ceil(0.1f + (f32)__fabs(d));
        f32 sign = d > 0.0f ? 1.0f : -1.0f;
        int s = sign * e;
        s = PMin(PMax(s, -46), 17);
        f32 p = pow(2.0, (f32)s);
        scale = (scale / 128.0f) / p;
        fn_802D3DF8(fn_802CA294(ind), scale, scale, s);
        GXSetNumIndStages((u8)1);
        GXSetTevIndirect((GXTevStageID)0, (GXIndTexStageID)0, (GXIndTexFormat)0, (GXIndTexBiasSel)3, (GXIndTexMtxID)1, (GXIndTexWrap)0, (GXIndTexWrap)0, (GXBool)0, (GXBool)0, (GXIndTexAlphaSel)0);
        GXSetIndTexOrder((GXIndTexStageID)0, (GXTexCoordID)lbl_8055FEA4, (GXTexMapID)2);
        GXSetNumTexGens((u8)(lbl_8055FEA4 + 1));
        GXSetTexCoordGen2((GXTexCoordID)lbl_8055FEA4, (GXTexGenType)1, (GXTexGenSrc)0, (u32)0x24, (GXBool)0, (u32)0x7D);
    }
    GXClearVtxDesc();
    GXSetVtxDesc((GXAttr)9, (GXAttrType)3);
    GXSetVtxDesc((GXAttr)0xB, (GXAttrType)3);
    GXSetVtxDesc((GXAttr)0xD, (GXAttrType)3);
    GXSetVtxAttrFmt((GXVtxFmt)0, (GXAttr)9, (GXCompCnt)1, (GXCompType)4, (u8)0);
    GXSetVtxAttrFmt((GXVtxFmt)0, (GXAttr)0xB, (GXCompCnt)1, (GXCompType)5, (u8)0);
    GXSetVtxAttrFmt((GXVtxFmt)0, (GXAttr)0xD, (GXCompCnt)1, (GXCompType)4, (u8)0);
    GXColor c;
    c.r = 0xC0;
    c.g = 0xC0;
    c.b = 0xC0;
    c.a = 0x14;
    GXSetTevColor((GXTevRegID)2, (GXColor)c);
}
#pragma pop

void fn_802DD228(void) {
    if (lbl_8055FEC0) {
        fn_802D3E88();
        lbl_8055FEC0 = 0;
    }
    fn_802DBDD8();
    fn_802CADD0(-1, 1);
    GXSetNumIndStages((u8)0);
    GXSetTevDirect((GXTevStageID)0);
}

void fn_802DD27C(u8* verts, int n, const u16* idx, int count) {
    GXSetArray((GXAttr)9, (const void*)verts, (u8)0x18);
    GXSetArray((GXAttr)0xB, (const void*)(verts + 0xC), (u8)0x18);
    GXSetArray((GXAttr)0xD, (const void*)(verts + 0x10), (u8)0x18);
    DCFlushRange((const void*)verts, (u32)(n * 0x18));
    GXBegin((GXPrimitive)0x90, (GXVtxFmt)0, (u16)count);
    for (int i = 0; i < count; i++) {
        u16 k = idx[i];
        WGPIPE.us = k;
        WGPIPE.us = k;
        WGPIPE.us = k;
    }
}

void* fn_802DD42C(u32 size) { return fn_802CFF64(size); }

}  // extern "C"
