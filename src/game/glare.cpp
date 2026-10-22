// Screen glare (0x802DD4FC-0x802DEAC4): copies the frame into a half-size "blur_tex", then redraws it
// as up to four soft-edged quads (one per screen quadrant or half) at growing scales over five passes
// to make the bloom/glare streaks. Names are guesses apart from the strings.
#include "game/common.h"

struct GXTexObj {
    u32 dummy[8];
};

struct GXRenderModeObj {
    s32 viTVmode;     // offset 0x0
    u16 fbWidth;      // offset 0x4
    u16 efbHeight;    // offset 0x6
};

struct GXVtxDescList {
    int attr;
    int type;
};

struct GXVtxAttrFmtList {
    int attr;
    int cnt;
    int type;
    u8 frac;
};

extern "C" {
GXRenderModeObj* fn_802D2D14(void);
void* fn_802CFF64(u32 size);  // frame allocation
void fn_802DEE88(void);
void fn_802DEF70(void);
void fn_802D3360(void);
void GXInitTexObj(GXTexObj* obj, void* image, u16 w, u16 h, int fmt, int ws, int wt, u8 mip);
void GXInitTexObjFilter(GXTexObj* obj, int minf, int magf);
void GXLoadTexObj(const GXTexObj* obj, int map);
void GXSetTexCopySrc(u16 l, u16 t, u16 w, u16 h);
void GXSetTexCopyDst(u16 w, u16 h, int fmt, u8 mip);
void GXCopyTex(void* dst, u8 clear);
void GXClearVtxDesc(void);
void GXSetVtxDescv(const GXVtxDescList* list);
void GXSetVtxAttrFmtv(int fmt, const GXVtxAttrFmtList* list);
void GXSetViewport(f32 x, f32 y, f32 w, f32 h, f32 n, f32 f);
void GXSetProjection(const f32 m[4][4], int type);
void GXLoadPosMtxImm(const f32 m[3][4], u32 id);
void GXSetCurrentMtx(u32 id);
void GXSetNumChans(u8 n);
void GXSetChanCtrl(int chan, u8 enable, int amb, int mat, u32 lights, int diff, int attn);
void GXSetColorUpdate(u8 on);
void GXSetZMode(u8 test, int func, u8 update);
void GXSetAlphaCompare(int c0, u8 r0, int op, int c1, u8 r1);
void GXSetArray(int attr, const void* base, u8 stride);
void GXBegin(int prim, int fmt, u16 n);
}

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);
void operator delete(void* p);
void operator delete[](void* p);

#define WGPIPE (*(volatile u8*)0xCC008000)

struct BlurTex {
    BlurTex(int fw, int fh, u8 half) {
        mip = half;
        if (half) {
            w = fw / 2;
            h = fh / 2;
        } else {
            w = fw;
            h = fh;
        }
        data = new ("gc_texture") u8[h * (w * 2)];
        GXInitTexObj(&obj, data, w, h, 4, 0, 0, 0);
        GXInitTexObjFilter(&obj, 1, 1);
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
    Streak* s = lbl_804DB6A0.s;
    for (int i = 0; i < lbl_804DB6A0.count; i++) {
        int c = lbl_804DB6A0.count;
        if (c < 3) {
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
        if (c == 1) {
            s->y0 = 1.0f;
            s->y1 = -1.0f;
            s->v0 = 0.0f;
            s->v1 = 1.0f;
        } else if (i < (c + 1) / 2) {
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
    if (AnyGlare()) {
        fn_802DE788();
        for (int pass = 0; pass < 5; pass++) {
            BlurTex* t = lbl_8055FED8;
            GXSetTexCopySrc(0, 0, lbl_8055FED0, lbl_8055FED4);
            GXSetTexCopyDst(t->w, t->h, 4, t->mip);
            GXCopyTex(t->data, 0);
            t->valid = 1;
            GXLoadTexObj(&lbl_8055FED8->obj, 0);
            f32 scale = (f32)(1 << pass);
            Streak* s = lbl_804DB6A0.s;
            for (int j = 0; j < lbl_804DB6A0.count; j++) {
                fn_802DD898(s, pass, scale, 0.5f);
                s++;
            }
        }
        fn_802DEA6C();
    }
}

static const f32 kFrac[24][2] = {
    {0.0f, 0.0f},   {0.09f, 0.0f},  {0.0f, 0.3f},   {0.09f, 0.3f},  {0.0f, 0.7f},   {0.09f, 0.7f},
    {0.0f, 1.0f},   {0.09f, 1.0f},  {0.91f, 0.0f},  {1.0f, 0.0f},   {0.91f, 0.3f},  {1.0f, 0.3f},
    {0.91f, 0.7f},  {1.0f, 0.7f},   {0.91f, 1.0f},  {1.0f, 1.0f},   {0.3f, 0.09f},  {0.7f, 0.09f},
    {0.3f, 0.91f},  {0.7f, 0.91f},  {0.3f, 0.0f},   {0.7f, 0.0f},   {0.3f, 0.97f},  {0.7f, 0.97f},
};

void fn_802DD898(Streak* s, int pass, f32 scale, f32 k) {
    int a = (int)(k * (255.0f * s->power));
    a = a > 0 ? a : 0;
    u32 color = (a < 0xFF ? a : 0xFF) | 0xFFFFFF00;
    f32 du = (0.01f * (0.5f * (scale * (s->u1 - s->u0)))) / 1.01f;
    f32 dv = (0.01f * (0.5f * (scale * (s->v1 - s->v0)))) / 1.01f;
    GVtx* buf = (GVtx*)fn_802CFF64(0x240);
    int n = 0;
    for (int i = 0; i < 24; i++) {
        GVtx* v = &buf[n++];
        f32 fx = kFrac[i][0];
        f32 fy = kFrac[i][1];
        v->x = s->x0 + fx * (s->x1 - s->x0);
        v->y = s->y0 + fy * (s->y1 - s->y0);
        v->z = 0.5f;
        f32 u0 = s->u0 + du;
        v->u = u0 + fx * ((s->u1 - du) - u0);
        f32 v0 = s->v0 + dv;
        v->v = v0 + fy * ((s->v1 - dv) - v0);
        v->color = (fx == 0.0f || fx == 1.0f || fy == 0.0f || fy == 1.0f) ? 0 : color;
    }
    GXSetArray(9, buf, 0x18);
    GXSetArray(0xD, &buf->u, 0x18);
    GXSetArray(0xB, &buf->color, 0x18);
    GXBegin(0x98, 0, 0x18);
    for (int i = 0; i < n; i++) {
        WGPIPE = i;
        WGPIPE = i;
        WGPIPE = i;
    }
}

void fn_802DE774(void) { lbl_804DB6A0.count = 0; }

void fn_802DE788(void) {
    GXVtxDescList desc[4] = {{9, 2}, {0xD, 2}, {0xB, 2}, {0xFF, 0}};
    GXClearVtxDesc();
    GXSetVtxDescv(desc);
    GXVtxAttrFmtList fmt[4] = {{9, 1, 4, 0}, {0xD, 1, 4, 0}, {0xB, 1, 5, 0}, {0xFF, 0, 0, 0}};
    GXSetVtxAttrFmtv(0, fmt);
    GXSetViewport(0.0f, 0.0f, lbl_8055FED0, lbl_8055FED4, 0.0f, 1.0f);
    f32 proj[4][4] = {{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, -0.5f},
                      {0.0f, 0.0f, 0.0f, 1.0f}};
    GXSetProjection(proj, 1);
    f32 m[3][4] = {{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0f}};
    GXLoadPosMtxImm(m, 0);
    GXSetCurrentMtx(0);
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 1, 1, 0, 0, 2);
    GXSetChanCtrl(5, 0, 1, 1, 0, 0, 2);
    fn_802DEE88();
    GXSetColorUpdate(1);
    GXSetZMode(0, 7, 0);
    GXSetAlphaCompare(4, 0, 0, 7, 0xFF);
}

void fn_802DEA6C(void) {
    fn_802DEF70();
    fn_802D3360();
}

}  // extern "C"
