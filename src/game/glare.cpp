// Screen glare (0x802DD4FC-0x802DEAC4): copies the frame into a half-size "blur_tex", then redraws it
// as up to four soft-edged quads (one per screen quadrant or half) at growing scales over five passes
// to make the bloom/glare streaks. Names are guesses apart from the strings.
#include "game/common.h"
#include <revolution/GX/GXTypes.h>
#include <revolution/GX/GXAttr.h>
#include <revolution/GX/GXFrameBuf.h>
#include <revolution/GX/GXGeometry.h>
#include <revolution/GX/GXLight.h>
#include <revolution/GX/GXPixel.h>
#include <revolution/GX/GXTev.h>
#include <revolution/GX/GXTexture.h>
#include <revolution/GX/GXTransform.h>

// Helper name unproven: arguments evaluate b, a, t, which sets the load order of the expanded form.
static inline f32 LerpT(f32 t, const f32& a, f32 b) { return a + t * (b - a); }

// GXTexObj: the SDK's.

// GXRenderModeObj: the SDK's.

// GXVtxDescList: the SDK's.

// GXVtxAttrFmtList: the SDK's.

extern "C" {
GXRenderModeObj* fn_802D2D14(void);
void* fn_802CFF64(u32 size);  // frame allocation
void fn_802DEE88(void);
void fn_802DEF70(void);
void fn_802D3360(void);
}

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);
void operator delete(void* p);
void operator delete[](void* p);

#define WGPIPE (*(volatile u8*)0xCC008000)

struct BlurTex {
    BlurTex(int fw, int fh, u8 half) {
        mip = half;
        if (mip) {
            w = fw / 2;
            h = fh / 2;
        } else {
            w = fw;
            h = fh;
        }
        data = new ("gc_texture") u8[h * (w * 2)];
        GXInitTexObj((GXTexObj*)&obj, (void*)data, (u16)w, (u16)h, (GXTexFmt)4, (GXTexWrapMode)0, (GXTexWrapMode)0, (GXBool)0);
        GXInitTexObjFilter((GXTexObj*)&obj, (GXTexFilter)1, (GXTexFilter)1);
        valid = 0;
    }
    GXTexObj obj;  // offset 0x0
    u8 pad20[0x20];
    u8* data;      // offset 0x40
    s32 w;         // offset 0x44
    s32 h;         // offset 0x48
    u8 mip;        // offset 0x4C
    u8 valid;      // offset 0x4D
    u8 pad4E[2];
};

struct Streak {
    f32 x0, x1;    // offset 0x0
    f32 y0, y1;    // offset 0x8
    f32 u0, u1;    // offset 0x10
    f32 v0, v1;    // offset 0x18
    f32 power;     // offset 0x20
};

struct Streaks {
    Streaks(void) { count = 0; }
    Streak s[4];   // offset 0x0
    s32 count;     // offset 0x90
    u8 pad94[0xC];
};

struct GVtx {
    f32 x, y, z;  // offset 0x0
    f32 u, v;     // offset 0xC
    u32 color;    // offset 0x14
};

ModuleTag lbl_8055FEC8;
s32 lbl_8055FED0;  // frame width
s32 lbl_8055FED4;  // frame height
BlurTex* lbl_8055FED8;
Streaks lbl_804DB6A0;

extern "C" {

void fn_802DD4FC(void) {
    GXRenderModeObj* rm = fn_802D2D14();
    lbl_8055FED0 = rm->fbWidth;
    lbl_8055FED4 = rm->efbHeight;
    lbl_8055FED8 = new ("blur_tex") BlurTex(lbl_8055FED0, lbl_8055FED4, 1);
}

void fn_802DD5F4(void) {
    BlurTex* t = lbl_8055FED8;
    if (t) {
        delete[] t->data;
        delete t;
    }
    lbl_8055FED8 = 0;
}

void fn_802DD63C(int n) {
    lbl_804DB6A0.count = n;
    int i;
    Streak* s;
    int count;
    s = lbl_804DB6A0.s;
    for (i = 0; i < (count = lbl_804DB6A0.count); i++) {
        if (count < 3) {
            s->x0 = -1.0f;
            s->x1 = 1.0f;
            s->u0 = 0.0f;
            s->u1 = 1.0f;
        } else if (i % 2 == 0) {
            s->x0 = -1.0f;
            s->x1 = 0.0f;
            s->u0 = 0.0f;
            s->u1 = 0.5f;
        } else {
            s->x0 = 0.0f;
            s->x1 = 1.0f;
            s->u0 = 0.5f;
            s->u1 = 1.0f;
        }
        if (count == 1) {
            s->y0 = 1.0f;
            s->y1 = -1.0f;
            s->v0 = 0.0f;
            s->v1 = 1.0f;
        } else if (i < (count + 1) / 2) {
            s->y0 = 1.0f;
            s->y1 = 0.0f;
            s->v0 = 0.0f;
            s->v1 = 0.5f;
        } else {
            s->y0 = 0.0f;
            s->y1 = -1.0f;
            s->v0 = 0.5f;
            s->v1 = 1.0f;
        }
        s->power = 0.0f;
        s++;
    }
    lbl_8055FED8->valid = 0;
}

void fn_802DD738(int i, f32 power) { lbl_804DB6A0.s[i].power = power; }

void fn_802DD750(void) {}

void fn_802DE788(void);
void fn_802DEA6C(void);
void fn_802DD898(Streak* s, int pass, f32 scale, f32 k);

static inline int AnyGlare(void) {
    for (int i = 0; i < lbl_804DB6A0.count; i++) {
        if (lbl_804DB6A0.s[i].power > 0.01f) {
            return 1;
        }
    }
    return 0;
}

void fn_802DD754(void) {
    BlurTex* t;
    int j;
    Streak* s;
    int pass;
    if (AnyGlare()) {
        fn_802DE788();
        for (pass = 0; pass < 5; pass++) {
            t = lbl_8055FED8;
            GXSetTexCopySrc((u16)0, (u16)0, (u16)lbl_8055FED0, (u16)lbl_8055FED4);
            GXSetTexCopyDst((u16)t->w, (u16)t->h, (GXTexFmt)4, (GXBool)t->mip);
            GXCopyTex((void*)t->data, (GXBool)0);
            t->valid = 1;
            GXLoadTexObj((const GXTexObj*)&lbl_8055FED8->obj, (GXTexMapID)0);
            s = lbl_804DB6A0.s;
            for (j = 0; j < lbl_804DB6A0.count; j++) {
                fn_802DD898(s, pass, (f32)(1 << pass), 0.5f);
                s++;
            }
        }
        fn_802DEA6C();
    }
}


// One vertex of a streak quad: position and texture coordinate at fraction (fx, fy) of the streak,
// with the texture inset by (du, dv).
enum GlareVertexForm {
    GlareVertexDirect,
    GlareVertexNamedX,
    GlareVertexNamedVEnd
};

// The named values place the vertex calculations at the same compiler block boundaries as retail.
template <GlareVertexForm Form>
static inline void WriteGlareVtx(GVtx* v, Streak* s, f32 fx, f32 fy, u32 color, f32 du, f32 dv) {
    if (Form == GlareVertexNamedX) {
        const f32 x = LerpT(fx, s->x0, s->x1);
        v->x = x;
    } else {
        v->x = LerpT(fx, s->x0, s->x1);
    }
    v->y = LerpT(fy, s->y0, s->y1);
    v->z = 0.5f;
    f32 u1 = s->u1 - du;
    f32 u0 = s->u0 + du;
    v->u = LerpT(fx, u0, u1);
    f32 v1;
    if (Form == GlareVertexNamedVEnd) {
        const f32 vEnd = s->v1;
        v1 = vEnd - dv;
    } else {
        v1 = s->v1 - dv;
    }
    f32 v0 = s->v0 + dv;
    v->v = LerpT(fy, v0, v1);
    v->color = color;
}

static inline void GlareVtx(GVtx* buf, int* n, Streak* s, f32 fx, f32 fy, u32 color, f32 du, f32 dv) {
    GVtx* v = &buf[(*n)++];
    WriteGlareVtx<GlareVertexDirect>(v, s, fx, fy, color, du, dv);
}

// Helper names unproven. Compute the address before advancing the count.
static inline void GlareVtxAdvance(GVtx* buf, int* n, Streak* s, f32 fx, f32 fy, u32 color, f32 du, f32 dv) {
    GVtx* v = buf + *n;
    ++(*n);
    WriteGlareVtx<GlareVertexDirect>(v, s, fx, fy, color, du, dv);
}

static inline void GlareVtxPair(GVtx* buf, int* n, Streak* s, f32 x0, f32 y0, u32 color,
                                f32 x1, f32 y1, f32 du, f32 dv) {
    GlareVtxAdvance(buf, n, s, x0, y0, color, du, dv);
    GlareVtx(buf, n, s, x1, y1, color, du, dv);
}

template <GlareVertexForm Form>
static inline void GlareVtx(GVtx* buf, int* n, Streak* s, f32 fx, f32 fy, u32 color, f32 du, f32 dv) {
    GVtx* v = &buf[(*n)++];
    WriteGlareVtx<Form>(v, s, fx, fy, color, du, dv);
}

// Draws one light streak as a framed quad: an outer ring (lit at the edge, faded inside) as a strip.
void fn_802DD898(Streak* s, int pass, f32 scale, f32 k) {
    f32 powerv = s->power;
    int a = (int)(k * (255.0f * powerv));
    a = a > 0 ? a : 0;
    u32 color = (a < 0xFF ? a : 0xFF) | 0xFFFFFF00;
    f32 du = (0.01f * (0.5f * (scale * (s->u1 - s->u0)))) / 1.01f;
    f32 dv = (0.01f * (0.5f * (scale * (s->v1 - s->v0)))) / 1.01f;
    int n = 0;
    GVtx* buf = (GVtx*)fn_802CFF64(0x240);
    GlareVtx(buf, &n, s, 0.0f, 0.0f, color, du, dv);
    GlareVtxPair(buf, &n, s, 0.09f, 0.09f, color, 0.0f, 1.0f, du, dv);
    GlareVtxPair(buf, &n, s, 0.09f, 0.97f, color, 1.0f, 1.0f, du, dv);
    GlareVtx(buf, &n, s, 1.0f - 0.09f, 0.97f, color, du, dv);
    GlareVtx(buf, &n, s, 1.0f, 0.0f, color, du, dv);
    GlareVtx(buf, &n, s, 1.0f - 0.09f, 0.09f, color, du, dv);
    GlareVtx(buf, &n, s, 0.0f, 0.0f, color, du, dv);
    GlareVtx(buf, &n, s, 0.09f, 0.09f, color, du, dv);
    GlareVtx(buf, &n, s, 0.09f, 0.09f, color, du, dv);
    GlareVtx(buf, &n, s, 0.3f, 0.3f, 0, du, dv);
    GlareVtx<GlareVertexNamedX>(buf, &n, s, 0.09f, 0.97f, color, du, dv);
    GlareVtx(buf, &n, s, 0.3f, 0.9f, 0, du, dv);
    GlareVtx(buf, &n, s, 1.0f - 0.09f, 0.97f, color, du, dv);
    GlareVtx(buf, &n, s, 0.7f, 0.9f, 0, du, dv);
    GlareVtx(buf, &n, s, 1.0f - 0.09f, 0.09f, color, du, dv);
    GlareVtx<GlareVertexNamedX>(buf, &n, s, 0.7f, 0.3f, 0, du, dv);
    GlareVtx<GlareVertexNamedVEnd>(buf, &n, s, 0.09f, 0.09f, color, du, dv);
    GlareVtx(buf, &n, s, 0.3f, 0.3f, 0, du, dv);
    GlareVtx(buf, &n, s, 0.3f, 0.3f, 0, du, dv);
    GlareVtx(buf, &n, s, 0.3f, 0.9f, 0, du, dv);
    GlareVtx(buf, &n, s, 0.7f, 0.3f, 0, du, dv);
    GlareVtx(buf, &n, s, 0.7f, 0.9f, 0, du, dv);
    GXSetArray((GXAttr)9, (const void*)buf, (u8)0x18);
    GXSetArray((GXAttr)0xD, (const void*)&buf->u, (u8)0x18);
    GXSetArray((GXAttr)0xB, (const void*)&buf->color, (u8)0x18);
    GXBegin((GXPrimitive)0x98, (GXVtxFmt)0, (u16)0x18);
    for (int i = 0; i < n; ++i) {
        WGPIPE = i;
        WGPIPE = i;
        WGPIPE = i;
    }
}

void fn_802DE774(void) { lbl_804DB6A0.count = 0; }

void fn_802DE788(void) {
    GXVtxDescList desc[4] = {{(GXAttr)9, (GXAttrType)2}, {(GXAttr)0xD, (GXAttrType)2}, {(GXAttr)0xB, (GXAttrType)2}, {(GXAttr)0xFF, (GXAttrType)0}};
    GXClearVtxDesc();
    GXSetVtxDescv(desc);
    GXVtxAttrFmtList fmt[4] = {{(GXAttr)9, (GXCompCnt)1, (GXCompType)4, 0}, {(GXAttr)0xD, (GXCompCnt)1, (GXCompType)4, 0}, {(GXAttr)0xB, (GXCompCnt)1, (GXCompType)5, 0}, {(GXAttr)0xFF, (GXCompCnt)0, (GXCompType)0, 0}};
    GXSetVtxAttrFmtv((GXVtxFmt)0, (const GXVtxAttrFmtList*)fmt);
    GXSetViewport(0.0f, 0.0f, lbl_8055FED0, lbl_8055FED4, 0.0f, 1.0f);
    f32 proj[4][4] = {{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, -0.5f},
                      {0.0f, 0.0f, 0.0f, 1.0f}};
    GXSetProjection((const f32 (*)[4])(proj), (GXProjectionType)1);
    f32 m[3][4] = {{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0f}};
    GXLoadPosMtxImm((const f32 (*)[4])(m), (u32)0);
    GXSetCurrentMtx((u32)0);
    GXSetNumChans((u8)1);
    GXSetChanCtrl((GXChannelID)4, (GXBool)0, (GXColorSrc)1, (GXColorSrc)1, (GXLightID)0, (GXDiffuseFn)0, (GXAttnFn)2);
    GXSetChanCtrl((GXChannelID)5, (GXBool)0, (GXColorSrc)1, (GXColorSrc)1, (GXLightID)0, (GXDiffuseFn)0, (GXAttnFn)2);
    fn_802DEE88();
    GXSetColorUpdate((GXBool)1);
    GXSetZMode((GXBool)0, (GXCompare)7, (GXBool)0);
    GXSetAlphaCompare((GXCompare)4, (u8)0, (GXAlphaOp)0, (GXCompare)7, (u8)0xFF);
}

void fn_802DEA6C(void) {
    fn_802DEF70();
    fn_802D3360();
}

}  // extern "C"
