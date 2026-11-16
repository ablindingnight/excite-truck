// GX driver core (0x802CE9FC-0x802D3F34, "gx.cpp"): video and GX init/shutdown, the render mode choice,
// the per-frame begin/end (dynamic frame memory, draw sync, hang watchdog, display copy), lazy state
// (z/blend/alpha flags, fog, lights, projection, viewport, camera and texture-matrix uploads), the
// texture-environment presets, region screenshots, visibility (z peek) tests and a couple of debug
// helpers. Names are guesses apart from the strings.
#include "game/common.h"
#include <revolution/GX/GXTypes.h>
#include <revolution/GX/GXAttr.h>
#include <revolution/GX/GXBump.h>
#include <revolution/GX/GXFifo.h>
#include <revolution/GX/GXFrameBuf.h>
#include <revolution/GX/GXGeometry.h>
#include <revolution/GX/GXInit.h>
#include <revolution/GX/GXLight.h>
#include <revolution/GX/GXMisc.h>
#include <revolution/GX/GXPixel.h>
#include <revolution/GX/GXTev.h>
#include <revolution/GX/GXTexture.h>
#include <revolution/GX/GXTransform.h>
#include <revolution/MTX/mtx.h>
#include <revolution/MTX/mtx44.h>
#include <revolution/MTX/vec.h>
#include <revolution/OS/OSCache.h>
#include <revolution/OS/OSError.h>
#include <revolution/VI/vi.h>
#include "game/math_types.h"
#include "game/plainvec.h"
#include "macros.h"

// GXColor: the SDK's.

// GXTexObj: the SDK's.

// GXLightObj: the SDK's.

// GXFifoObj: the SDK's.

// GXRenderModeObj: the SDK's.

// One matrix row as a block (retail copies Invert's rows as three 12-byte blocks: lfd+lwz each).
struct Row3 {
    f32 v[3];
};

// RMtx: common.h's PlainValMatrix.
extern "C" {
void* memset(void* dst, int c, size_t n);
void* memcpy(void* dst, const void* src, size_t n);
int memcmp(const void* a, const void* b, size_t n);
double tan(double x);
double atan(double x);
s64 fn_802E2C9C(void);  // OSGetTime
f32 __cvt_sll_flt(s64 v);
u32 __cvt_fp2unsigned(f64 v);
void fn_802E317C(const char* fmt, ...);  // fatal
void fn_802E30C8(const char* fmt, ...);  // warning print
int sprintf(char* buf, const char* fmt, ...);
int fn_802E2C20(void);                   // milliseconds
void fn_80127468(void);
void fn_801282EC(int align);
void fn_801282F4(void);
void fn_80127C90(void (*cb)(void), const char* name);
void fn_80127D58(void (*cb)(void));
void fn_802E2E0C(int (*cb)(int), const char* name);
int fn_802E9A1C(void);
void fn_802D9C4C(void);
void fn_802D9C18(void);
void fn_802D9A50(void);
void fn_802D8D1C(void);
void fn_802D8DA0(void);
void fn_802D6810(void);
void fn_802D6860(void);
void fn_802D6C68(void);
void fn_802D6C78(void);
void fn_802D68C4(void* verts, int unused, const u16* idx, int n);
void fn_802DACD4(void);
void fn_802DA2F4(void);
void fn_802DA4C4(void);
void fn_802DA52C(void);
void fn_802DAE18(void);
void fn_802DB168(void);
void fn_802DD4FC(void);
void fn_802DD5F4(void);
void fn_802ECA48(void);
void fn_802D9230(void* tex);
void fn_802D92B8(void* tex);
void fn_802D92B0(void);
void fn_802DFAF4(void);
void fn_802DF21C(void);
void fn_802DF2F0(void);
void fn_802E09EC(void);
void fn_802E0B68(void);
void fn_802DF300(void);
void fn_802DF46C(void);
void fn_802DEFA0(void);
void fn_802DF1B4(void);
void fn_802DF994(void);
void fn_802DF6D0(void);
void fn_802DF938(void);
void fn_802DF4A8(void);
void fn_802DF684(void);
int fn_802E3D68(const char* name);
void fn_802E3734(s32* fd);
void fn_802E3A64(s32 fd, const void* buf, u32 n);
void fn_801BFD34(V3* out, const V3* in, const PlainValMatrix* m);
void fn_802C8588(void* out, const V3* a, const V3* b);

extern GXRenderModeObj GXNtsc480IntDf;
extern GXRenderModeObj GXNtsc480Prog;
extern GXRenderModeObj GXPal528IntDf;
extern GXRenderModeObj GXEurgb60Hz480IntDf;
extern GXRenderModeObj GXMpal480IntDf;
void GXSetGPMetric(int a, int b);
// GXCompare: the SDK's.
// GXFogType: the SDK's.
}

void* operator new[](size_t size, const char* tag);
void operator delete[](void* p);

union WGPipe {
    u8 uc;
    u16 us;
    u32 u;
    f32 f;
};

#define WGPIPE (*(volatile WGPipe*)0xCC008000)

struct TexState {
    u8 dirty;   // offset 0x0
    s32 mode;   // offset 0x4
    u8 base2;   // offset 0x8
    u8 env;     // offset 0x9
    u8 envA;    // offset 0xA
    u8 bB;      // offset 0xB
    u8 lmap;    // offset 0xC
    u8 ind;     // offset 0xD
};

struct Capture {
    u8 pending;     // offset 0x0
    u8 active;      // offset 0x1
    u8 pad2[2];
    s32 x;          // offset 0x4
    s32 y;          // offset 0x8
    s32 w;          // offset 0xC
    s32 h;          // offset 0x10
    u16 data[0x2000];  // offset 0x14
};

struct XFB {
    u16* buf;  // offset 0x0
    u16* cur;  // offset 0x4
    s32 w;     // offset 0x8
    s32 h;     // offset 0xC
    s32 bpp;   // offset 0x10
    u8 hold;   // offset 0x14
    u8 pad15[3];
};

struct Proj {
    u8 dirty;      // offset 0x0
    u8 vpDirty;    // offset 0x1
    u8 pad2[2];
    f32 nearZ;     // offset 0x4
    f32 farZ;      // offset 0x8
    f32 fovy;      // offset 0xC
    f32 tanHalf;   // offset 0x10
    f32 zoom;      // offset 0x14
    f32 aspect;    // offset 0x18
    s32 vx;        // offset 0x1C
    s32 vy;        // offset 0x20
    s32 vw;        // offset 0x24
    s32 vh;        // offset 0x28
    u8 overlayZ;   // offset 0x2C
    u8 overlayC;   // offset 0x2D
    GXColor overlay;  // offset 0x2E
    u8 pad32[6];
};

struct Fog {
    u8 range;      // offset 0x0
    GXColor color; // offset 0x1
    u8 pad5[3];
    f32 start;     // offset 0x8
    f32 end;       // offset 0xC
    f32 depth;     // offset 0x10
    f32 nearZ;     // offset 0x14
    f32 farZ;      // offset 0x18
    u8 dirty;      // offset 0x1C
    u8 pad1D[3];
};

// A direction copied as words (array member).
struct LightDir {
    f32 v[3];
};

// Byte-aligned, so it copies as nine bytes through one pointer (lbzu/stbu on the first).
struct LightColors {
    GXColor amb;
    GXColor color;
    u8 alpha;
};

// Unit-local (port ODR: other units define a different Light).
namespace {
struct Light {
    GXColor amb;    // offset 0x0
    GXColor color;  // offset 0x4
    u8 alpha;       // offset 0x8
    u8 pad9[3];
    V3 dir;         // offset 0xC
    f32 scale;      // offset 0x18
    GXLightObj obj; // offset 0x1C
    u8 dirty;       // offset 0x5C
    u8 pad5D[3];
};
}  // namespace

struct GXLightSet {
    GXLightSet(void);
    PlainValMatrix view;      // offset 0x0
    GXColor amb;    // offset 0x30
    GXColor col;    // offset 0x34
    u8 alpha;       // offset 0x38
    u8 pad39[3];
    V3 dir;         // offset 0x3C
    f32 scale;      // offset 0x48
};

struct ShotBuf {
    void* buf;      // offset 0x0
    GXTexObj obj;   // offset 0x4
    u8 pad24[0x20];
};

struct VisTest {
    s32 x;      // offset 0x0
    s32 y;      // offset 0x4
    u32 z;      // offset 0x8
    u8 used;    // offset 0xC
    u8 hit;     // offset 0xD
    u8 padE[2];
    f32 rate;   // offset 0x10
    f32 vis;    // offset 0x14
};

struct VisTests {
    VisTest tests[0x40];   // offset 0x0
    VisTest* queue[0x40];  // offset 0x600
    s32 count;             // offset 0x700
};

struct PointLight {
    PointLight(void) {}
    V3 pos;         // offset 0x0
    GXColor color;  // offset 0xC
    f32 radius;     // offset 0x10
};

extern s32 lbl_8055CFF0;  // screen width
extern s32 lbl_8055CFF4;  // screen height
extern u8 lbl_8055FE58;
extern void* lbl_8055FE1C;
extern void* lbl_8055FE20;
struct GXTex;
extern GXTex* lbl_8055FE24;  // drtex.cpp's render target (null when drawing to the frame buffer)
extern u8 lbl_8055FE39;
extern u32 lbl_804DB458[4];

f32 lbl_804C5650[4][4];      // loaded position matrix
PlainValMatrix lbl_804C5690;           // view
PlainValMatrix lbl_804C56C0;           // camera
V3 lbl_804C56F0;             // camera side axis
TexState lbl_804C5700;
Capture lbl_804C5710;
GXRenderModeObj lbl_804C9724;
XFB lbl_804C9760;
Proj lbl_804C9778;
Fog lbl_804C97B0;
Light lbl_804C97D0;
char lbl_804C9830[0x100];    // screenshot name
TexState lbl_804C9930;
ShotBuf lbl_804C9940;
VisTests lbl_804C9984;

struct GPMetric {
    s32 id;
    const char* name;
    s32 metric;
    const char* unit;
    s32 kind;
};

f32 lbl_8055D0B0 = 99.0f;
f32 lbl_8055D0B4 = 99.0f;
s32 lbl_8055D0B8 = -1;  // screenshot tile
s32 lbl_8055D0BC = 7;
s32 lbl_8055D0C0 = 7;
u8 lbl_8055D0C4 = 1;

GPMetric lbl_8034BD40[6] = {
    {0x22, "Clocks", 0x10, "Vertex", 1},     {2, "Clip Clocks", 0x15, "Clock", 1},
    {3, "XF Wait In", 0x15, "Clock", 1},     {4, "XF Wait Out", 0x15, "Clock", 1},
    {5, "XF XFrm", 0x15, "Clock", 1},        {6, "XF Lit", 0x15, "Clock", 1},
};

extern s32 __float_epsilon[];  // FLT_EPSILON
#define FEPS (*(f32*)__float_epsilon)

ModuleTag lbl_8055FD00;
static inline u16 BlendKey(int v) { return v; }
extern const u16 lbl_8055FD08 = BlendKey(0x100);
u16 lbl_8055FD0A = BlendKey(0x405);
u16 lbl_8055FD0C = BlendKey(0x401);
PointLight lbl_804CA088[7];
GXLightObj lbl_804CA118[7];
PlainValMatrix lbl_804CA2D8;
s64 lbl_8055FD10;   // draw sync time
u8 lbl_8055FD18;
u8 lbl_8055FD19;
u32 lbl_8055FD1C;   // render flags
u32 lbl_8055FD20;   // dirty flags
u8 lbl_8055FD24;    // alpha ref
u8 lbl_8055FD25;    // lighting on
u32 lbl_8055FD28;   // tev color 2
s32 lbl_8055FD2C;   // last finished frame
s32 lbl_8055FD30;   // frame
u8 lbl_8055FD34;    // waiting for draw done
volatile s32 lbl_8055FD38;  // retraces waited (counted in the retrace callback)
volatile u8 lbl_8055FD3C;   // draw done (set by the draw-done callback)
s32 lbl_8055FD40;
s32 lbl_8055FD44;   // retraces
s32 lbl_8055FD48;   // black frames
s32 lbl_8055FD4C;   // retraces since flip
u8 lbl_8055FD50;    // scanline effect
void* lbl_8055FD54;
void* lbl_8055FD58;
void* lbl_8055FD5C;
u8 lbl_8055FD60;    // dst alpha
GXRenderModeObj* lbl_8055FD64;
void* lbl_8055FD68;  // fifo buffer
GXFifoObj* lbl_8055FD6C;
u8 lbl_8055FD70;    // camera dirty
u32 lbl_8055FD74;   // flags last applied
u8 lbl_8055FD78;
s32 lbl_8055FD7C;
void (*lbl_8055FD80)(void);  // tev cleanup
u8 lbl_8055FD84;
u8 lbl_8055FD85;
u16 lbl_8055FD86;   // blend
u16 lbl_8055FD88;
s32 lbl_8055FD8C;   // light set
u8 lbl_8055FD90;    // screenshot running
u8 lbl_8055FD91;
s32 lbl_8055FD94;   // dynamic data used
s32 lbl_8055FD98;   // max dynamic data
s32 lbl_8055FD9C;
u8* lbl_8055FDA0;   // dynamic data
s32 lbl_8055FDA4;
s32 lbl_8055FDA8;
s32 lbl_8055FDAC;   // light sets
GXLightSet* lbl_8055FDB0;
u8 lbl_8055FDB4;    // shadow alpha pass
u32 lbl_8055FDB8;   // hang count
u32 lbl_8055FDBC;
s32 lbl_8055FDC0;
s32 lbl_8055FDC4;   // screenshot start
s32 lbl_8055FDC8;   // point lights
s32 lbl_8055FDCC;


extern "C" {

void fn_802CF850(void);
void fn_802CED78(void);
void fn_802D3900(void);
void fn_802D0BD8(void);
void fn_802D0C98(void);
void fn_802D1678(void);
void fn_802D14D8(void);
void fn_802D2218(void);
void fn_802D0EEC(void);
void fn_802D18E8(void);
void fn_802D1C04(void);
void fn_802D2684(void);
void fn_802D2640(void);
void fn_802D37E4(void);
void fn_802D3894(void);
void fn_802D0910(s32 fd, int x, int y, int w, int h);
void fn_802D09E4(void);
void fn_802CFF10(void);
void fn_802CFF5C(u32 flags);
void fn_802CFFA4(u16 blend);
void fn_802CFF50(void);
void fn_802CF5D0(void);
void fn_802CF5D4(void);
int fn_802CEC04(int reset);
void fn_802CEC44(void);
void fn_802CECF4(u16 token);
static inline int DarkenPixel(int pixel) {
    int chroma = pixel & 0xFF;
    return chroma > (pixel - 0x400) ? chroma : (pixel - 0x400);
}

void fn_802CF10C(u32 count);

GXColor fn_802CE9FC(u8 r, u8 g, u8 b, u8 a) {
    GXColor c;
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = a;
    return c;
}

GXColor fn_802CEA1C(u32 argb) { return fn_802CE9FC(argb >> 16, argb >> 8, argb, argb >> 24); }

void fn_802CEA34(void) {
    GXRenderModeObj* rm = lbl_8055FD64;
    u32 n = ((rm->fbWidth + 15) & 0xFFF0) * rm->xfbHeight * 2;
    lbl_804C9760.w = rm->fbWidth;
    lbl_804C9760.h = rm->xfbHeight;
    lbl_804C9760.bpp = 2;
    lbl_804C9760.buf = (u16*)new ("xfb.a") u8[n];
    memset(lbl_804C9760.buf, 0, n);
    lbl_804C9760.cur = lbl_804C9760.buf;
}

void fn_802CEAC4(void) {
    switch (VIGetTvFormat()) {
    case 0:
        if (!fn_802E9A1C() || !VIGetDTVStatus()) {
            fn_802E30C8("gx: NTSC 480i");
            lbl_8055FD64 = &GXNtsc480IntDf;
        } else {
            fn_802E30C8("gx: NTSC 480p");
            lbl_8055FD64 = &GXNtsc480Prog;
        }
        break;
    case 1:
        fn_802E30C8("gx: PAL 528");
        lbl_8055FD64 = &GXPal528IntDf;
        break;
    case 5:
        fn_802E30C8("gx: EURO RGB60 480");
        lbl_8055FD64 = &GXEurgb60Hz480IntDf;
        break;
    case 2:
        fn_802E30C8("gx: MPAL 480");
        lbl_8055FD64 = &GXMpal480IntDf;
        break;
    default:
        fn_802E317C("gx: invalid TV format");
        break;
    }
    GXAdjustForOverscan((const GXRenderModeObj*)lbl_8055FD64, (GXRenderModeObj*)&lbl_804C9724, (u16)0, (u16)0x10);
    lbl_8055FD64 = &lbl_804C9724;
    lbl_804C9724.viWidth += 10;
    lbl_804C9724.viXOrigin -= 5;
}

int fn_802CEC04(int reset) {
    if (reset == 1) {
        fn_802D9C4C();
        GXDrawDone();
        VISetBlack((BOOL)1);
        VIFlush();
        VIWaitForRetrace();
    }
    return 0;
}

void fn_802CEC44(void) { lbl_8055FD3C = 1; }

void fn_802CEC50(u16 token) {
    switch (token) {
    case 1:
        lbl_8055FD10 = fn_802E2C9C();
        lbl_8055FD3C = 0;
        break;
    case 2: {
        f32 ms = 0.001f * __cvt_sll_flt(fn_802E2C9C() - lbl_8055FD10);
        lbl_8055D0B0 = ms;
        f32 avg = lbl_8055D0B4;
        lbl_8055D0B4 = ms + 0.95f * (avg - ms);
        fn_802D3900();
        break;
    }
    default:
        fn_802E317C("gx: unhandled draw sync token %x", token);
        break;
    }
}

u16 lbl_8055D0F6 = 2;   // draw sync token

void fn_802CECF4(u16 token) {
    do {
        if (++lbl_8055D0F6 > 2) {
            lbl_8055D0F6 = 1;
        }
        fn_802CEC50(lbl_8055D0F6);
    } while (lbl_8055D0F6 != token);
}

void fn_802CED50(s32 x, s32 y, s32 w, s32 h) {
    lbl_804C5710.x = x;
    lbl_804C5710.y = y;
    lbl_804C5710.w = w;
    lbl_804C5710.h = h;
    lbl_804C5710.pending = 1;
    lbl_804C5710.active = 1;
}

void fn_802CED78(void) {
    u32 d0, c0, b0, a0, d1, c1, b1, a1;
    u8 readIdle, cmdIdle, overhi, underlow, brk;
    GXReadXfRasMetric((u32*)&c0, (u32*)&d0, (u32*)&a0, (u32*)&b0);
    GXReadXfRasMetric((u32*)&c1, (u32*)&d1, (u32*)&a1, (u32*)&b1);
    int clocks = d0 == d1;
    int vtx = c0 == c1;
    int xfWaitIn = (b1 - b0) != 0;
    int xfWaitOut = (a1 - a0) != 0;
    GXGetGPStatus((GXBool*)&overhi, (GXBool*)&underlow, (GXBool*)&readIdle, (GXBool*)&cmdIdle, (GXBool*)&brk);
    OSReport("GP status (%c%c%c%c%c) metric (%d%d%d%d) --> ", overhi ? 'O' : '-', underlow ? 'U' : '-',
             readIdle ? '-' : 'R', cmdIdle ? '-' : 'C', brk ? 'B' : '-', clocks, vtx, xfWaitIn, xfWaitOut);
    if (!vtx && xfWaitIn) {
        OSReport("GP hang due to XF stall bug.\n");
    } else if (!clocks && vtx && xfWaitIn) {
        OSReport("GP hang due to unterminated primitive.\n");
    } else if (!cmdIdle && clocks && vtx && xfWaitIn) {
        OSReport("GP hang due to illegal instruction.\n");
    } else if (readIdle && cmdIdle && clocks && vtx && xfWaitIn && xfWaitOut) {
        OSReport("GP appears to be not hung (waiting for input).\n");
    } else {
        OSReport("GP is in unknown state.\n");
    }
}

void fn_802CEF90(void) {
    u8 overhi;
    u8 dummy;
    if (lbl_8055FD34) {
        lbl_8055FD38++;
    }
    GXGetGPStatus((GXBool*)&overhi, (GXBool*)&dummy, (GXBool*)&dummy, (GXBool*)&dummy, (GXBool*)&dummy);
    u32 n = GXGetOverflowCount();
    if (overhi && n == lbl_8055FDBC) {
        if (++lbl_8055FDB8 >= 10) {
            OSReport("---------WARNING : HANG AT HIGH WATERMARK----------\n");
            fn_802CED78();
            OSPanic("gx.cpp", 0x201, "Halting program");
        }
    } else {
        lbl_8055FDBC = n;
        lbl_8055FDB8 = 0;
    }
}

void fn_802CF048(int on) {
    if (on) {
        GXSetGPMetric(0x23, 0x16);
        WGPIPE.uc = 0x61;
        WGPIPE.u = 0x2402C004;
        WGPIPE.uc = 0x61;
        WGPIPE.u = 0x23000020;
        WGPIPE.uc = 0x10;
        WGPIPE.us = 0;
        WGPIPE.us = 0x1006;
        WGPIPE.u = 0x84400;
    } else {
        WGPIPE.uc = 0x61;
        WGPIPE.u = 0x24000000;
        WGPIPE.uc = 0x61;
        WGPIPE.u = 0x23000000;
        WGPIPE.uc = 0x10;
        WGPIPE.us = 0;
        WGPIPE.us = 0x1006;
        WGPIPE.u = 0;
    }
}

void fn_802CF104(void) { fn_802CF048(1); }

void fn_802CF10C(u32 count) {
    u16* buf;
    fn_802CEF90();
    if (lbl_8055FD44 == 4000) {
        OSReport("gx: hang? or perhaps the disk drive has powered down...\n");
    }
    Capture* cap = &lbl_804C5710;
    lbl_8055FD44++;
    lbl_8055FD4C++;
    if (lbl_8055FD4C < 5 || (!lbl_8055FD50 && !cap->active)) {
        return;
    }
    if (cap->pending) {
        cap->pending = 0;
        DCInvalidateRange((const void*)lbl_804C9760.cur, (u32)(lbl_804C9760.w * lbl_804C9760.h * 2));
        u16* src = lbl_804C9760.cur;
        for (int y = 0; y < cap->h; y++) {
            memcpy(&cap->data[y * cap->w],
                   src + cap->x + lbl_804C9760.w * (y + cap->y), cap->w * 2);
        }
    }
    if (cap->active) {
        int m = lbl_8055FD4C % 60;
        if (m != 0 && m != 45) {
            return;
        }
    } else if (lbl_8055FD4C % 2 != 0) {
        return;
    }
    buf = lbl_804C9760.cur;
    DCInvalidateRange((const void*)buf, (u32)(lbl_804C9760.w * lbl_804C9760.h * 2));
    int y = (lbl_8055FD4C >> 2) & 1;
    if (lbl_8055FD50) {
        for (; y < lbl_804C9760.h; y += 2) {
            for (int x = 0; x < lbl_804C9760.w; x++) {
                int i = y * lbl_804C9760.w + x;
                buf[i] = DarkenPixel(buf[i]);
            }
        }
    }
    if (cap->active) {
        u16* dst = lbl_804C9760.cur;
        if (lbl_8055FD4C % 60 < 30) {
            int w = cap->w;
            for (int y = 0; y < cap->h; y++) {
                u16* d = &dst[lbl_804C9760.w * (cap->y + y) + cap->x];
                u16* s = &cap->data[y * w];
                for (int x = 0; x < w; x++) {
                    d[x] = s[x];
                }
            }
        } else {
            const u16 c = cap->data[0];
            const int w = cap->w;
            for (int y = 0; y < cap->h; y++) {
                u16* d = &dst[lbl_804C9760.w * (cap->y + y) + cap->x];
                int x;
                for (x = 0; x < w; x++) {
                    d[x] = c;
                }
            }
        }
    }
    DCFlushRange((const void*)lbl_804C9760.cur, (u32)(lbl_804C9760.bpp * (lbl_804C9760.w * lbl_804C9760.h)));
}

u8 lbl_8055D0FF = 1;

void fn_802CF5A8(void) {
    if (lbl_8055D0FF) {
        lbl_8055D0FF = 0;
        fn_802E2E0C(fn_802CEC04, "GX");
    }
}

void fn_802CF5D0(void) {}

void fn_802CF5D4(void) {
    lbl_8055FD24 = 0;
    lbl_8055FD91 = 0;
    GXSetAlphaCompare((GXCompare)4, (u8)0, (GXAlphaOp)1, (GXCompare)0, (u8)0);
}

void fn_802CF5F8(u32) {}

void fn_802CF5FC(void) {}

static inline GXLightSet* NewLightSets(void) { return new ("local_flist") GXLightSet[0x800]; }

void fn_802CF600(void) {
    lbl_8055FD7C = 0;
    lbl_8055FD48 = 3;
    lbl_8055FD4C = 0;
    lbl_8055FD50 = 0;
    lbl_8055FD2C = -0x259;
    lbl_8055FD30 = -0x258;
    lbl_8055FD64 = 0;
    lbl_8055FE24 = 0;
    fn_80127468();
    VIInit();
    fn_802CEAC4();
    fn_80127468();
    VIConfigure((const GXRenderModeObj*)lbl_8055FD64);
    fn_802CEA34();
    fn_80127468();
    fn_801282EC(0x20);
    fn_801282F4();
    lbl_8055FDA0 = new ("gc:dyndata") u8[0x177000];
    lbl_8055FD94 = 0;
    lbl_8055FDB0 = NewLightSets();
    lbl_8055FDAC = 0;
    fn_802D8D1C();
    fn_80127468();
    lbl_8055FD68 = new ("fifo_buf") u8[0x300000];
    lbl_8055FD6C = GXInit((void*)lbl_8055FD68, (u32)0x300000);
    fn_802D6810();
    fn_802CF850();
    fn_80127468();
    fn_802E30C8("gx: done with drReset");
    fn_802D0BD8();
    fn_802E30C8("gx: done with before render");
    fn_80127468();
    fn_802CFF10();
    fn_802E30C8("gx: did flip");
    fn_80127468();
    fn_802CF5A8();
    fn_802DACD4();
    fn_802D9C18();
    VISetPreRetraceCallback((VIRetraceCallback)fn_802CF10C);
    fn_80127468();
    fn_802DA2F4();
    fn_802CFF5C(0x17);
    fn_802CFFA4(lbl_8055FD0A);
    lbl_8055FD20 = -1;
    u32 n = GXGetTexBufferSize((u16)0x280, (u16)0x1C0, (u32)1, (GXBool)0, (u8)0);
    fn_801282EC(0x20);
    lbl_804C9940.buf = new ("shadow alpha mem") u8[n];
    GXInitTexObj((GXTexObj*)&lbl_804C9940.obj, (void*)lbl_804C9940.buf, (u16)0x280, (u16)0x1C0, (GXTexFmt)1, (GXTexWrapMode)0, (GXTexWrapMode)0, (GXBool)0);
    GXSetDrawSyncCallback((GXDrawSyncCallback)fn_802CECF4);
    GXSetDrawDoneCallback((GXDrawDoneCallback)fn_802CEC44);
    fn_802D37E4();
    fn_802DD4FC();
    lbl_8055FD40 = -1;
    fn_80127468();
    fn_80127C90(fn_802D9A50, "drIdle");
    fn_802DB168();
    lbl_8055FD48 = 0;
    VISetBlack((BOOL)0);
    VIFlush();
}

inline GXLightSet::GXLightSet(void) {}

int fn_802CF834(void) { return lbl_804C9760.cur != 0; }

void fn_802D0188(u8 r, u8 g, u8 b);
void fn_802D0160(u8 r, u8 g, u8 b);
void fn_802D007C(f32 a);
void fn_802D01B0(const V3* dir, f32 scale);
void fn_802D01E0(s32 x, s32 y, s32 w, s32 h);
void fn_802D0368(f32 n, f32 f, f32 fovy, f32 zoom);
void fn_802D03F4(const PlainValMatrix* cam);
void fn_802D0610(u32 set);

void fn_802CF850(void) {
    fn_802CF104();
    GXSetViewport(0.0f, 0.0f, lbl_8055FD64->fbWidth, lbl_8055FD64->efbHeight, 0.0f, 1.0f);
    GXSetScissor((u32)0, (u32)0, (u32)lbl_8055FD64->fbWidth, (u32)lbl_8055FD64->efbHeight);
    GXSetPixelFmt((GXPixelFmt)1, (GXZFmt16)0);
    GXSetDispCopyGamma((u32)0);
    GXSetCopyClear((GXColor)(fn_802CE9FC(0, 0, 0, 0)), (u32)0xFFFFFF);
    GXSetDither((GXBool)1);
    VIWaitForRetrace();
    VISetNextFrameBuffer((void*)lbl_804C9760.buf);
    lbl_804C9760.cur = lbl_804C9760.buf;
    VIFlush();
    if (lbl_8055FD64->tvInfo & 1) {
        VIWaitForRetrace();
    }
    GXSetNumChans((u8)1);
    GXSetNumTexGens((u8)1);
    GXSetTevOrder((GXTevStageID)0, (GXTexCoordID)0, (GXTexMapID)0, (GXChannelID)4);
    GXSetTevOp((GXTevStageID)0, (GXTevMode)0);
    GXSetBlendMode((GXBlendMode)1, (GXBlendFactor)4, (GXBlendFactor)5, (GXLogicOp)5);
    GXSetTevOrder((GXTevStageID)1, (GXTexCoordID)0, (GXTexMapID)1, (GXChannelID)4);
    GXSetTevOp((GXTevStageID)1, (GXTevMode)2);
    GXSetTevColorIn((GXTevStageID)1, (GXTevColorArg)0, (GXTevColorArg)0, (GXTevColorArg)0xF, (GXTevColorArg)0xF);
    GXSetTevColorOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    fn_802CF5D4();
    GXSetZCompLoc((GXBool)0);
    V3* a;
    V3* b;
    a = (V3*)&lbl_804C56C0;
    a[0].x = 1.0f;
    a[0].y = 0.0f;
    a[0].z = 0.0f;
    a[1].x = 0.0f;
    a[1].y = 1.0f;
    a[1].z = 0.0f;
    a[2].x = 0.0f;
    a[2].y = 0.0f;
    a[2].z = 1.0f;
    a[3].x = 0.0f;
    a[3].y = 0.0f;
    a[3].z = 0.0f;
    b = (V3*)&lbl_804C5690;
    b[0].x = 1.0f;
    b[0].y = 0.0f;
    b[0].z = 0.0f;
    b[1].x = 0.0f;
    b[1].y = 1.0f;
    b[1].z = 0.0f;
    b[2].x = 0.0f;
    b[2].y = 0.0f;
    b[2].z = 1.0f;
    b[3].x = 0.0f;
    b[3].y = 0.0f;
    b[3].z = 0.0f;
    lbl_804C56F0.x = 1.0f;
    lbl_804C56F0.y = 0.0f;
    lbl_804C56F0.z = 0.0f;
    lbl_8055FD70 = 1;
    lbl_804C5700.dirty = 1;
    fn_802D0188(0xFF, 0xFF, 0xFF);
    fn_802D0160(0x20, 0x20, 0x20);
    fn_802D007C(1.0f);
    V3 dir;
    dir.x = 0.0f;
    dir.y = -1.0f;
    dir.z = 0.0f;
    fn_802D01B0(&dir, 1.0f);
    fn_802D01E0(0, 0, lbl_8055CFF0, lbl_8055CFF4);
    fn_802D0368(0.5f, 1000.0f, 1.5707964f, 1.0f);
    PlainValMatrix cam;
    cam.basis[0].x = 1.0f;
    cam.basis[0].y = 0.0f;
    cam.basis[0].z = 0.0f;
    cam.basis[1].x = 0.0f;
    cam.basis[1].y = 1.0f;
    cam.basis[1].z = 0.0f;
    cam.basis[2].x = 0.0f;
    cam.basis[2].y = 0.0f;
    cam.basis[2].z = 1.0f;
    cam.origin.x = 0.0f;
    cam.origin.y = 0.0f;
    cam.origin.z = 0.0f;
    fn_802D03F4(&cam);
    fn_802D0610(0);
    lbl_8055FD85 = 1;
    lbl_8055FD25 = 0;
    lbl_8055FD84 = 1;
    fn_802CFF5C(0x17);
    fn_802CFF50();
    fn_802CFFA4(lbl_8055FD0A);
    if (lbl_8055FE1C) {
        fn_802D9230(lbl_8055FE1C);
    }
    if (lbl_8055FE20) {
        fn_802D92B8(lbl_8055FE20);
    }
    lbl_8055FDB4 = 0;
}

void fn_802CFBA8(void) {
    fn_80127D58(fn_802D9A50);
    fn_802D9C4C();
    VISetPreRetraceCallback((VIRetraceCallback)0);
    fn_802E30C8("gx: max dynamic data used = %d", lbl_8055FD98);
    fn_802DD5F4();
    fn_802D3894();
    fn_802D6860();
    delete[] (u8*)lbl_804C9940.buf;
    lbl_804C9940.buf = 0;
    fn_802DA4C4();
    fn_802D0C98();
    VISetBlack((BOOL)1);
    VIFlush();
    VIWaitForRetrace();
    GXFlush();
    GXDrawDone();
    VIFlush();
    VIWaitForRetrace();
    fn_802DAE18();
    delete[] (u8*)lbl_8055FD68;
    lbl_8055FD68 = 0;
    delete[] (u8*)lbl_804C9760.buf;
    lbl_804C9760.buf = 0;
    fn_802D8DA0();
    delete[] lbl_8055FDB0;
    lbl_8055FDB0 = 0;
    lbl_804C9760.cur = 0;
    delete[] lbl_8055FDA0;
    lbl_8055FDA0 = 0;
}

void fn_802CFCA0(void) {
    GXFifoObj fifo;
    void* rd;
    void* wr;
    fn_802D6C68();
    lbl_8055FE39 = 1;
    lbl_8055FDA4 = lbl_8055FD94;
    lbl_8055FD9C = 0;
    lbl_804C5700.dirty = 1;
    lbl_8055FD8C = -1;
    fn_802DA52C();
    GXSetDrawDone();
    GXFlush();
    GXGetCPUFifo((GXFifoObj*)&fifo);
    GXGetFifoPtrs((const GXFifoObj*)&fifo, (void**)&rd, (void**)&wr);
    GXEnableBreakPt((void*)wr);
    GXFlush();
    GXInvalidateVtxCache();
    GXSetDrawSync((u16)1);
    lbl_8055FD30++;
    if (!lbl_804C97B0.range && !((f32)__fabs(lbl_804C97B0.depth) <= FEPS)) {
        lbl_804C97B0.range = 1;
        lbl_804C97B0.dirty = 1;
    }
}

void fn_802CFD74(void) {
    GXFifoObj fifo;
    GXGetCPUFifo((GXFifoObj*)&fifo);
    void* base = GXGetFifoBase((const GXFifoObj*)&fifo);
    u32 size = GXGetFifoSize((const GXFifoObj*)&fifo);
    GXAbortFrame();
    lbl_8055FD6C = GXInit((void*)base, (u32)size);
    fn_802CF850();
    VIConfigure((const GXRenderModeObj*)lbl_8055FD64);
}

void fn_802CFDE0(void) {
    fn_802ECA48();
    s32 used = lbl_8055FD94 + lbl_8055FD9C;
    lbl_8055FD98 = used > lbl_8055FD98 ? used : lbl_8055FD98;
    fn_802D6C78();
    GXFlush();
    GXSetDrawSync((u16)2);
    lbl_8055FDA8 = lbl_8055FD94;
    if (lbl_8055FDA4 < lbl_8055FD94) {
        DCFlushRange((const void*)(lbl_8055FDA0 + lbl_8055FDA4), (u32)(lbl_8055FD94 - lbl_8055FDA4));
    } else {
        DCFlushRange((const void*)(lbl_8055FDA0 + lbl_8055FDA4), (u32)(0x177000 - lbl_8055FDA4));
        DCFlushRange((const void*)lbl_8055FDA0, (u32)lbl_8055FDA8);
    }
    lbl_8055FD38 = 0;
    lbl_8055FD34 = 1;
    while (!lbl_8055FD3C) {
        if (lbl_8055FD38 > 10) {
            OSReport("---------WARNING : ABORTING FRAME----------\n");
            fn_802CED78();
            fn_802CFD74();
            fn_802CF048(1);
            lbl_8055FD40 = fn_802E2C20();
            break;
        }
    }
    lbl_8055FD34 = 0;
    lbl_8055FD2C = lbl_8055FD30 - 1;
    GXDisableBreakPt();
}

void fn_802CFEEC(void) {
    fn_802CFCA0();
    fn_802CFDE0();
}

#pragma push
#pragma peephole off
void fn_802CFF10(void) {
    lbl_8055FD44 = 0;
    fn_802CF5D0();
    fn_802D0C98();
    fn_802D0BD8();
    lbl_8055FDAC = 0;
    lbl_8055FE39 = 0;
}
#pragma pop

void fn_802CFF50(void) { lbl_8055FD20 = -1; }

void fn_802CFF5C(u32 flags) { lbl_8055FD1C = flags; }

void* fn_802CFF64(u32 size) {
    s32 n = (size + 0x1F) & ~0x1F;
    s32 used = lbl_8055FD94;
    if (used + n <= 0x177000) {
        lbl_8055FD94 = used + n;
        return lbl_8055FDA0 + used;
    }
    lbl_8055FD94 = n;
    return lbl_8055FDA0;
}

void* fn_802CFFA0(u32 size) { return fn_802CFF64(size); }

void fn_802CFFA4(u16 blend) { lbl_8055FD86 = blend; }

void fn_802CFFAC(u8 r, u8 g, u8 b) {
    GXColor col = fn_802CE9FC(r, g, b, 0);
    lbl_804C97B0.color.r = col.r;
    lbl_804C97B0.color.g = col.g;
    lbl_804C97B0.color.b = col.b;
    lbl_804C97B0.color.a = col.a;
    lbl_804C97B0.dirty = 1;
}

void fn_802D0014(f32 start, f32 end, f32 depth) {
    lbl_804C97B0.start = start;
    if (!((f32)__fabs(depth) <= FEPS)) {
        lbl_804C97B0.range = 1;
        lbl_804C97B0.end = start + (end - start) / depth;
    } else {
        lbl_804C97B0.end = end;
        lbl_804C97B0.range = 0;
    }
    lbl_804C97B0.depth = depth;
    lbl_804C97B0.dirty = 1;
}

void fn_802D007C(f32 a) {
    f32 x = MIN(255.0f, (f32)(int)(255.5f * a));
    x = MAX(x, 0.0f);
    GXColor c;
    c.r = c.g = c.b = 0xFF;
    c.a = (int)x;
    lbl_804C97D0.alpha = c.a;
    lbl_804C97D0.dirty = 1;
    GXSetTevColor((GXTevRegID)1, (GXColor)c);
    lbl_804C5700.dirty = 1;
}

void fn_802D0160(u8 r, u8 g, u8 b) {
    lbl_804C97D0.color.r = r;
    lbl_804C97D0.color.g = g;
    lbl_804C97D0.color.b = b;
    lbl_804C97D0.color.a = 0xFF;
    lbl_804C97D0.dirty = 1;
}

void fn_802D0188(u8 r, u8 g, u8 b) {
    lbl_804C97D0.amb.r = r;
    lbl_804C97D0.amb.g = g;
    lbl_804C97D0.amb.b = b;
    lbl_804C97D0.amb.a = 0;
    lbl_804C97D0.dirty = 1;
}

void fn_802D01B0(const V3* dir, f32 scale) {
    lbl_804C97D0.dir.x = dir->x;
    lbl_804C97D0.dir.y = dir->y;
    lbl_804C97D0.dir.z = dir->z;
    lbl_804C97D0.scale = scale;
    lbl_804C97D0.dirty = 1;
}

void fn_802D01E0(s32 x, s32 y, s32 w, s32 h) {
    if (lbl_8055FE24 == 0) {
        y = 0.5f + 448.0f * y / 480.0f;
        h = 0.5f + 448.0f * h / 480.0f;
    }
    fn_802D2218();
    lbl_804C9778.vx = x;
    lbl_804C9778.vy = y;
    lbl_804C9778.vw = w;
    lbl_804C9778.vh = h;
    lbl_804C9778.vpDirty = 1;
    lbl_804C9778.dirty = 1;
    lbl_804C9778.aspect = (f32)w / (f32)h;
}

void fn_802D02F8(u32 color, u8 on) {
    lbl_804C9778.overlayC = on;
    lbl_804C9778.overlayZ = 1;
    GXColor col = fn_802CEA1C(color);
    lbl_804C9778.overlay.r = col.r;
    lbl_804C9778.overlay.g = col.g;
    lbl_804C9778.overlay.b = col.b;
    lbl_804C9778.overlay.a = col.a;
}

void fn_802D0368(f32 n, f32 f, f32 fovy, f32 zoom) {
    lbl_804C9778.dirty = 1;
    lbl_804C9778.nearZ = n;
    lbl_804C9778.farZ = f;
    lbl_804C9778.fovy = fovy;
    lbl_804C9778.zoom = zoom;
    lbl_804C9778.tanHalf = tan(fovy / 2.0f);
    lbl_804C97B0.nearZ = n;
    lbl_804C97B0.farZ = f;
    lbl_804C97B0.dirty = 1;
}

void fn_802D03F4(const PlainValMatrix* cam) {
    fn_802D6C78();
    lbl_804C56C0 = *cam;
    V3 up;
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    PSVECCrossProduct((const Vec*)&up, (const Vec*)&cam->basis[2], (Vec*)&lbl_804C56F0);
    f32 mag = PSVECMag((const Vec*)&lbl_804C56F0);
    mag = mag > 0.05f ? mag : 0.05f;
    f32 inv = 1.0f / mag;
    lbl_8055FD70 = 1;
    lbl_804C56F0.x *= inv;
    lbl_804C56F0.y *= inv;
    lbl_804C56F0.z *= inv;
}

void fn_802D050C(void* p) { lbl_8055FD54 = p; }

s32 fn_802D0514(const PlainValMatrix* view) {
    s32 n = lbl_8055FDAC;
    if (n >= 0x800) {
        return 0;
    }
    GXLightSet* s = &lbl_8055FDB0[n];
    lbl_8055FDAC = n + 1;
    s->view = *view;
    *(LightColors*)&s->amb = *(const LightColors*)&lbl_804C97D0.amb;
    *(LightDir*)&s->dir = *(const LightDir*)&lbl_804C97D0.dir;
    s->scale = lbl_804C97D0.scale;
    return n + 1;
}

void fn_802D0610(u32 set) {
    if (set == lbl_8055FD8C) {
        return;
    }
    if (set != 0) {
        GXLightSet* s = &lbl_8055FDB0[set - 1];
        lbl_804C5690 = s->view;
        lbl_8055FD70 = 1;
        *(LightColors*)&lbl_804C97D0.amb = *(const LightColors*)&s->amb;
        *(LightDir*)&lbl_804C97D0.dir = *(const LightDir*)&s->dir;
        lbl_804C97D0.scale = s->scale;
        lbl_804C97D0.dirty = 1;
    } else {
        PlainValMatrix m;
        m.basis[0].x = 1.0f;
        m.basis[0].y = 0.0f;
        m.basis[0].z = 0.0f;
        m.basis[1].x = 0.0f;
        m.basis[1].y = 1.0f;
        m.basis[1].z = 0.0f;
        m.basis[2].x = 0.0f;
        m.basis[2].y = 0.0f;
        m.basis[2].z = 1.0f;
        m.origin.x = 0.0f;
        m.origin.y = 0.0f;
        m.origin.z = 0.0f;
        lbl_804C5690 = m;
        lbl_8055FD70 = 1;
    }
    lbl_8055FD8C = set;
}

// MATCHING-AID: linker-stripped functions, reconstructed only so the .data string pool keeps the original
// offsets (their literals stay in the pool; see docs/matching_aids.md). Bodies are guesses.
void GXCheckEnvMap(const char* name, int lut, int alpha) {
    if (lut) {
        fn_802E30C8("dr: custom envmap with lut data disallowed (%s)", name);
    }
    if (alpha) {
        fn_802E30C8("dr: custom envmap with alpha obj disallowed (%s)", name);
    }
}

void fn_802D07E4(const char* name, int announce) {
    if (lbl_8055D0B8 >= 0) {
        fn_802E30C8("gx: aborting screenshot...");
        lbl_8055D0B8 = lbl_8055D0BC * lbl_8055D0C0;
        return;
    }
    lbl_8055FD90 = 1;
    int n = 0x100;
    char* d = lbl_804C9830;
    while ((*d++ = *name++) != 0) {
        if (--n <= 0) {
            d[-1] = 0;
            break;
        }
    }
    if (announce) {
        fn_802E30C8("gx: taking %d x %d screenshot!", lbl_8055D0BC * lbl_804C9760.w,
                    lbl_8055D0C0 * lbl_804C9760.h);
        lbl_8055D0B8 = 0;
    }
}

// MATCHING-AID: linker-stripped (see GXCheckEnvMap).
void GXScreenshotNumbered(int n) {
    char name[32];
    sprintf(name, "tmp_screenshot%02d.tga", n);
    fn_802D07E4(name, 1);
}

void fn_802D08B4(s32 tx, s32 ty, u8 raw) {
    lbl_8055FD18 = raw;
    lbl_8055D0BC = tx;
    lbl_8055D0C0 = ty;
}

void fn_802D08C4(u8 on) { lbl_8055FD78 = on; }

void fn_802D08CC(int mode) {
    int m;
    switch (mode) {
    case 1:
        m = 1;
        break;
    case 2:
        m = 2;
        break;
    case 0:
        m = 0;
        break;
    default:
        m = 0;
        break;
    }
    GXSetCullMode((GXCullMode)m);
}

// Writes one screenshot tile (x, y in the full w x h image); empty in this build.
void fn_802D0910(s32 fd, int x, int y, int w, int h) {}

static inline u8 TgaHeader(u8* hdr) {
    u8 t;
    hdr[0] = 0;
    hdr[1] = 0;
    hdr[2] = 2;
    hdr[0x10] = 0x18;
    hdr[0x11] = 0;
    t = hdr[8];
    return t;
}

static inline u8 TgaSwapDims(u8* hdr, u8 t) {
    hdr[8] = hdr[9];
    hdr[9] = t;
    t = hdr[10];
    hdr[10] = hdr[11];
    hdr[11] = t;
    t = hdr[12];
    return t;
}

void fn_802D0914(s32 fd, int w, int h) {
    u8 hdr[0x12];
    memset(hdr, 0, sizeof(hdr));
    *(u16*)(hdr + 8) = 0;
    *(u16*)(hdr + 10) = 0;
    *(u16*)(hdr + 12) = w;
    *(u16*)(hdr + 14) = h;
    u8 t;
    t = TgaHeader(hdr);
    t = TgaSwapDims(hdr, t);
    hdr[12] = hdr[13];
    hdr[13] = t;
    t = hdr[14];
    hdr[14] = hdr[15];
    hdr[15] = t;
    fn_802E3A64(fd, hdr, 0x12);
}

void fn_802D09E4(void) {}

s32 lbl_8055D10C = -1;  // screenshot file

void fn_802D09E8(void) {
    if (!lbl_8055FD90) {
        return;
    }
    if (lbl_8055D0B8 < 0) {
        lbl_8055FD90 = 0;
        fn_802D09E4();
        return;
    }
    if (++lbl_8055FDC0 <= 2) {
        return;
    }
    lbl_8055FDC0 = 0;
    if (lbl_8055D0B8 == 0) {
        lbl_8055D10C = fn_802E3D68(lbl_804C9830);
        if (lbl_8055D10C == -1) {
            fn_802E30C8("gx: can't create %s", lbl_804C9830);
            lbl_8055FD90 = 0;
            lbl_8055D0B8 = -1;
            return;
        }
        lbl_8055FDC4 = fn_802E2C20();
        fn_802E30C8("gx: starting screenshot %s", lbl_804C9830);
        if (!lbl_8055FD18) {
            fn_802D0914(lbl_8055D10C, lbl_804C9760.w * lbl_8055D0BC, lbl_804C9760.h * lbl_8055D0C0);
        }
    }
    s32 tile = lbl_8055D0B8;
    if (tile < lbl_8055D0BC * lbl_8055D0C0) {
        int tx = tile % lbl_8055D0BC;
        int ty = tile / lbl_8055D0BC;
        fn_802E30C8(" tile %d / %d", tile + 1, lbl_8055D0BC * lbl_8055D0C0);
        fn_802D0910(lbl_8055D10C, lbl_804C9760.w * tx, lbl_804C9760.h * ty, lbl_804C9760.w * lbl_8055D0BC,
                    lbl_804C9760.h * lbl_8055D0C0);
        lbl_8055D0B8++;
    }
    if (lbl_8055D0B8 >= lbl_8055D0BC * lbl_8055D0C0) {
        lbl_8055FD90 = 0;
        lbl_8055D0B8 = -1;
        fn_802E3734(&lbl_8055D10C);
        lbl_8055D10C = -1;
        fn_802E30C8("gx: screenshot %s complete (%1.1f s)", lbl_804C9830,
                    0.001f * (fn_802E2C20() - lbl_8055FDC4));
    }
}

// MATCHING-AID: linker-stripped (see GXCheckEnvMap).
void GXPrintRow(const f32* v) { fn_802E30C8(" %5.2f %5.2f %5.2f %5.2f ", v[0], v[1], v[2], v[3]); }

void fn_802D0BD8(void) {
    GXRenderModeObj* rm = lbl_8055FD64;
    if (rm->field_rendering) {
        GXSetViewportJitter(0.0f, 0.0f, rm->fbWidth, rm->efbHeight, 0.0f, 1.0f, (u32)(VIGetNextField()));
    } else {
        GXSetViewport(0.0f, 0.0f, rm->fbWidth, rm->efbHeight, 0.0f, 1.0f);
    }
    GXInvalidateVtxCache();
    GXInvalidateTexAll();
}

void fn_802D0C98(void) {
    lbl_8055FD4C = 0;
    GXSetZMode((GXBool)1, (GXCompare)GX_LEQUAL, (GXBool)1);
    GXSetColorUpdate((GXBool)1);
    fn_802D09E8();
    GXSetColorUpdate((GXBool)lbl_8055D0C4);
    lbl_8055FD74 |= 1;
    GXSetZMode((GXBool)1, (GXCompare)GX_ALWAYS, (GXBool)1);
    lbl_8055FD74 = (lbl_8055FD74 | 2) & ~4;
    u16 h = GXSetDispCopyYScale(GXGetYScaleFactor(lbl_8055FD64->efbHeight, lbl_8055FD64->xfbHeight));
    GXSetDispCopySrc((u16)0, (u16)0, (u16)lbl_8055FD64->fbWidth, (u16)lbl_8055FD64->efbHeight);
    GXSetDispCopyDst((u16)lbl_8055FD64->fbWidth, (u16)h);
    GXSetCopyFilter((GXBool)lbl_8055FD64->aa, lbl_8055FD64->sample_pattern, (GXBool)1, lbl_8055FD64->vfilter);
    GXCopyDisp((void*)lbl_804C9760.cur, (GXBool)1);
    VISetNextFrameBuffer((void*)lbl_804C9760.cur);
    if (lbl_8055FD48 > 0) {
        lbl_8055FD48--;
        VISetBlack((BOOL)1);
    } else {
        lbl_8055FD48 = 0;
        VISetBlack((BOOL)0);
    }
    VIFlush();
    lbl_804C9760.cur = lbl_804C9760.buf;
    fn_802CFF50();
}

void fn_802D0DD4(f32 out[3][4], const PlainValMatrix* m) {
    out[0][0] = m->basis[0].x;
    out[1][0] = m->basis[0].y;
    out[2][0] = -m->basis[0].z;
    out[0][1] = m->basis[1].x;
    out[1][1] = m->basis[1].y;
    out[2][1] = -m->basis[1].z;
    out[0][2] = m->basis[2].x;
    out[1][2] = m->basis[2].y;
    out[2][2] = -m->basis[2].z;
    out[0][3] = m->origin.x;
    out[1][3] = m->origin.y;
    out[2][3] = -m->origin.z;
}

void fn_802D0E48(int pass) {
    if (pass == 0) {
        GXSetAlphaCompare((GXCompare)3, (u8)lbl_8055FD24, (GXAlphaOp)1, (GXCompare)0, (u8)0);
        GXSetZMode((GXBool)1, (GXCompare)GX_LEQUAL, (GXBool)0);
    } else if (pass == 1) {
        GXSetAlphaCompare((GXCompare)4, (u8)lbl_8055FD24, (GXAlphaOp)1, (GXCompare)0, (u8)0);
        GXSetZMode((GXBool)1, (GXCompare)GX_LEQUAL, (GXBool)1);
    }
}

int fn_802D0EC8(int f) {
    switch (f) {
    case 8:
        return 2;
    case 9:
        return 3;
    }
    return f;
}

static inline void ApplyBlend(void) {
    if (lbl_8055FD86 == lbl_8055FD08) {
        GXSetBlendMode((GXBlendMode)0, (GXBlendFactor)0, (GXBlendFactor)0, (GXLogicOp)5);
    } else {
        int hi = lbl_8055FD86 >> 8;
        int lo = lbl_8055FD86 & 0xFF;
        GXSetBlendMode((GXBlendMode)1, (GXBlendFactor)(fn_802D0EC8(hi)), (GXBlendFactor)(fn_802D0EC8(lo)), (GXLogicOp)5);
    }
}

static inline void ScaleV3(V3* out, const V3* v, f32 k) {
    out->x = v->x * k;
    out->y = v->y * k;
    out->z = v->z * k;
}

static inline GXColor ScaleColor(const GXColor& c, f32 s) {
    GXColor out;
    out.r = (int)(s * c.r);
    out.g = (int)(s * c.g);
    out.b = (int)(s * c.b);
    out.a = c.a;
    return out;
}

void fn_802D0EEC(void) {
    if (lbl_8055FDB4) {
        return;
    }
    u32 f = lbl_8055FD1C;
    u32 d = lbl_8055FD20 | (f ^ lbl_8055FD74);
    lbl_8055FD20 = 0;
    if (d) {
        if (d & 6) {
            if (f & 6) {
                int upd = (f >> 1) & 1;
                GXSetZMode((GXBool)1, (GXCompare)(f & 4 ? GX_LEQUAL : GX_ALWAYS), (GXBool)upd);
            } else {
                GXSetZMode((GXBool)0, (GXCompare)GX_ALWAYS, (GXBool)0);
            }
        }
        if (d & 1) {
            GXSetColorUpdate((GXBool)(lbl_8055FD1C & 1));
        }
        if (d & 0x10) {
            GXSetAlphaUpdate((GXBool)((lbl_8055FD1C >> 4) & 1));
        }
        if (d & 0x20) {
            GXSetTevColor((GXTevRegID)2, (GXColor)(*(GXColor*)&lbl_8055FD28));
        }
        if (d & 0x40) {
            if (lbl_8055FD1C & 0x40) {
                GXSetDstAlpha((GXBool)0, (u8)0);
                GXSetAlphaCompare((GXCompare)7, (u8)0, (GXAlphaOp)1, (GXCompare)0, (u8)0);
            } else {
                GXSetDstAlpha((GXBool)1, (u8)lbl_8055FD60);
                fn_802CF5D4();
            }
            lbl_804C5700.dirty = 1;
        }
        if (d & 0x80) {
            ApplyBlend();
            lbl_8055FD88 = lbl_8055FD86;
            lbl_804C5700.dirty = 1;
        }
        if (d & 0x100) {
            lbl_804C5700.dirty = 1;
        }
        lbl_8055FD74 = lbl_8055FD1C;
    }
    if (lbl_8055FD24 != lbl_8055FD91) {
        GXSetAlphaCompare((GXCompare)4, (u8)lbl_8055FD24, (GXAlphaOp)1, (GXCompare)0, (u8)0);
        lbl_8055FD91 = lbl_8055FD24;
    }
    if (lbl_8055FD86 != lbl_8055FD88) {
        ApplyBlend();
        lbl_8055FD88 = lbl_8055FD86;
        lbl_804C5700.dirty = 1;
    }
    if (lbl_8055FE58) {
        GXSetTevColor((GXTevRegID)3, (GXColor)(fn_802CEA1C(lbl_804DB458[1])));
        lbl_8055FE58 = 0;
    }
    if (lbl_804C97B0.dirty) {
        lbl_804C97B0.dirty = 0;
        GXSetFog((GXFogType)(lbl_804C97B0.range ? GX_FOG_PERSP_LIN : GX_FOG_NONE), lbl_804C97B0.color, lbl_804C97B0.start, lbl_804C97B0.end, lbl_804C97B0.nearZ, lbl_804C97B0.farZ);
    }
    if (lbl_804C97D0.dirty) {
        lbl_804C97D0.dirty = 0;
        GXColor w = fn_802CE9FC(0xFF, 0xFF, 0xFF, 0xFF);
        GXColor k = fn_802CE9FC(0, 0, 0, 0xFF);
        V3 p;
        ScaleV3(&p, &lbl_804C97D0.dir, -1048576.0f);
        GXInitLightPos((GXLightObj*)&lbl_804C97D0.obj, p.x, p.y, -p.z);
        GXInitLightColor((GXLightObj*)&lbl_804C97D0.obj, (GXColor)(ScaleColor(lbl_804C97D0.amb, lbl_804C97D0.scale)));
        GXInitLightDistAttn((GXLightObj*)&lbl_804C97D0.obj, 1.0f, 1.0f, (GXDistAttnFn)0);
        GXLoadLightObjImm((const GXLightObj*)&lbl_804C97D0.obj, (GXLightID)1);
        GXSetChanMatColor((GXChannelID)0, (GXColor)w);
        GXSetChanMatColor((GXChannelID)2, (GXColor)(fn_802CE9FC(0, 0, 0, lbl_804C97D0.alpha)));
        GXSetChanAmbColor((GXChannelID)0, (GXColor)lbl_804C97D0.color);
        GXSetChanAmbColor((GXChannelID)2, (GXColor)k);
    }
    if (lbl_8055FD25 != lbl_8055FD85 || lbl_8055FD84) {
        lbl_8055FD85 = lbl_8055FD25;
        lbl_8055FD84 = 0;
        if (lbl_804C5700.mode == 0) {
            lbl_804C5700.dirty = 1;
        }
        GXSetNumChans((u8)1);
        if (lbl_8055FD25) {
            GXSetChanCtrl((GXChannelID)0, (GXBool)1, (GXColorSrc)0, (GXColorSrc)0, (GXLightID)1, (GXDiffuseFn)2, (GXAttnFn)2);
            GXSetChanCtrl((GXChannelID)2, (GXBool)0, (GXColorSrc)0, (GXColorSrc)0, (GXLightID)0, (GXDiffuseFn)0, (GXAttnFn)2);
        } else {
            GXSetChanCtrl((GXChannelID)0, (GXBool)0, (GXColorSrc)1, (GXColorSrc)1, (GXLightID)0, (GXDiffuseFn)0, (GXAttnFn)2);
            GXSetChanCtrl((GXChannelID)2, (GXBool)0, (GXColorSrc)1, (GXColorSrc)1, (GXLightID)0, (GXDiffuseFn)0, (GXAttnFn)2);
        }
    }
}

// MATCHING-AID: machine-extracted inline helper (loopext); it raised the score but nothing proves the original had it.
static inline f32 FovFit(f32 half, f32 a) { return 2.0f * (f32)atan((f32)tan(half) / a); }

static inline void Loop1_fn_802D14D8(f32 k) {
    f32 aspect = lbl_804C9778.aspect * (k / lbl_804C9778.zoom);
    f32 fovy = FovFit(0.5f * lbl_804C9778.fovy, aspect);
    f32 m[4][4];
    C_MTXPerspective((f32 (*)[4])(m), fovy / 0.017453292f, aspect, lbl_804C9778.nearZ, lbl_804C9778.farZ);
    if (lbl_8055FE24 == 0 && lbl_8055D0B8 >= 0 && lbl_8055D0B8 < lbl_8055D0BC * lbl_8055D0C0) {
        int ty = lbl_8055D0B8 / lbl_8055D0BC;
        int tx = lbl_8055D0B8 - ty * lbl_8055D0BC;
        f32 sy, sx;
        sx = 1.0f / lbl_8055D0BC;
        sy = 1.0f / lbl_8055D0C0;
        m[0][0] = m[0][0] / sx;
        m[1][1] = m[1][1] / sy;
        m[0][2] = m[0][2] + (f32)(tx * 2 - lbl_8055D0BC + 1);
        m[1][2] = m[1][2] - (f32)(ty * 2 - lbl_8055D0C0 + 1);
    }
    GXSetProjection((const f32 (*)[4])(m), (GXProjectionType)0);
}

void fn_802D14D8(void) {
    if (!lbl_804C9778.dirty) {
        return;
    }
    lbl_804C9778.dirty = 0;
    f32 k = lbl_8055FE24 ? 1.0f : 0.93333334f;
    Loop1_fn_802D14D8(k);
}

void fn_802D1678(void) {
    if (!lbl_804C9778.vpDirty) {
        return;
    }
    lbl_804C9778.vpDirty = 0;
    if (lbl_8055FD64->field_rendering) {
        GXSetViewportJitter(lbl_804C9778.vx, lbl_804C9778.vy, lbl_804C9778.vw, lbl_804C9778.vh, 0.0f, 1.0f, (u32)(VIGetNextField()));
    } else {
        GXSetViewport(lbl_804C9778.vx, lbl_804C9778.vy, lbl_804C9778.vw, lbl_804C9778.vh, 0.0f, 1.0f);
    }
    GXSetScissor((u32)lbl_804C9778.vx, (u32)lbl_804C9778.vy, (u32)lbl_804C9778.vw, (u32)lbl_804C9778.vh);
    fn_802D6C68();
}

void fn_802D17BC(GXLightObj* l, const V3* pos, const V3* cam, GXColor color, f32 radius) {
    GXInitLightColor((GXLightObj*)l, (GXColor)color);
    V3 d;
    d.x = pos->x + cam->x;
    d.y = pos->y + cam->y;
    d.z = pos->z + cam->z;
    f32 mag = PSVECMag((const Vec*)&d);
    mag = mag > 0.001f ? mag : 0.001f;
    f32 inv = -1.0f / mag;
    d.x *= inv;
    d.y *= inv;
    d.z *= inv;
    GXInitSpecularDirHA((GXLightObj*)l, cam->x, cam->y, -cam->z, d.x, d.y, -d.z);
    GXInitLightAttn((GXLightObj*)l, 0.0f, 0.0f, 1.0f, radius / 2.0f, 0.0f, 1.0f - radius / 2.0f);
}

void fn_802D18E8(void) {
    V3 d;
    d.x = lbl_804C5690.origin.x - lbl_804C56C0.origin.x;
    d.y = lbl_804C5690.origin.y - lbl_804C56C0.origin.y;
    d.z = lbl_804C5690.origin.z - lbl_804C56C0.origin.z;
    f32 mag = PSVECMag((const Vec*)&d);
    mag = mag > 0.001f ? mag : 0.001f;
    f32 inv = 1.0f / mag;
    lbl_8055FDCC = 0;
    d.x *= inv;
    d.y *= inv;
    d.z *= inv;
    if (lbl_8055FDC8 > 0) {
        PointLight* p = lbl_804CA088;
        GXLightObj* o = lbl_804CA118;
        for (int i = 0; i < lbl_8055FDC8; i++) {
            fn_802D17BC(o, &d, &p->pos, p->color, p->radius);
            p++;
            o++;
        }
        lbl_8055FDCC = lbl_8055FDC8;
    } else {
        fn_802D17BC(lbl_804CA118, &d, &lbl_804C97D0.dir, ScaleColor(lbl_804C97D0.amb, lbl_804C97D0.scale), 64.0f);
        lbl_8055FDCC++;
    }
    u32 mask = 0;
    int i;
    GXLightObj* o = lbl_804CA118;
    for (i = 0; i < lbl_8055FDCC; i++) {
        u32 bit = 2 << i;
        GXLoadLightObjImm((const GXLightObj*)o, (GXLightID)bit);
        mask |= bit;
        o++;
    }
    GXSetNumChans((u8)2);
    GXSetChanCtrl((GXChannelID)1, (GXBool)1, (GXColorSrc)0, (GXColorSrc)0, (GXLightID)mask, (GXDiffuseFn)0, (GXAttnFn)0);
    GXSetChanCtrl((GXChannelID)3, (GXBool)0, (GXColorSrc)0, (GXColorSrc)0, (GXLightID)0, (GXDiffuseFn)0, (GXAttnFn)2);
}

void fn_802D1B58(void) {
    f32 m[2][4] = {{0.0f, 0.0f, 0.0f, 0.5f}, {0.0f, 0.0f, 0.0f, 0.5f}};
    m[0][0] = 0.5f * -lbl_804C5690.basis[0].y;
    m[0][1] = 0.5f * -lbl_804C5690.basis[1].y;
    m[0][2] = 0.5f * -lbl_804C5690.basis[2].y;
    GXLoadTexMtxImm((const f32 (*)[4])(m), (u32)0x21, (GXMtxType)1);
}

static inline void MulVec(V3* out, const V3* in, const Mat3* m) {
    if (out == in) {
        f32 x = in->x * m->m[0][0] + in->y * m->m[1][0] + in->z * m->m[2][0];
        f32 y = in->x * m->m[0][1] + in->y * m->m[1][1] + in->z * m->m[2][1];
        f32 z = in->x * m->m[0][2] + in->y * m->m[1][2] + in->z * m->m[2][2];
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = in->x * m->m[0][0] + in->y * m->m[1][0] + in->z * m->m[2][0];
        out->y = in->x * m->m[0][1] + in->y * m->m[1][1] + in->z * m->m[2][1];
        out->z = in->x * m->m[0][2] + in->y * m->m[1][2] + in->z * m->m[2][2];
    }
}

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

static inline void MulVecT(V3* out, const V3* in, const Mat3* const& m) {
    if (out == in) {
        f32 x = Dot3(in->x, m->m[0][0], in->y, m->m[0][1], in->z, m->m[0][2]);
        f32 y = Dot3(in->x, m->m[1][0], in->y, m->m[1][1], in->z, m->m[1][2]);
        f32 z = Dot3(in->x, m->m[2][0], in->y, m->m[2][1], in->z, m->m[2][2]);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, m->m[0][0], in->y, m->m[0][1], in->z, m->m[0][2]);
        out->y = Dot3(in->x, m->m[1][0], in->y, m->m[1][1], in->z, m->m[1][2]);
        out->z = Dot3(in->x, m->m[2][0], in->y, m->m[2][1], in->z, m->m[2][2]);
    }
}

static inline void Invert(PlainValMatrix* out, const PlainValMatrix* m) {
    for (int i = 0; i < 3; i++) {
        *(Row3*)&out->basis[i] = *(const Row3*)&m->basis[i];
    }
    f32 t = out->basis[0].y;
    out->basis[0].y = out->basis[1].x;
    out->basis[1].x = t;
    t = out->basis[0].z;
    out->basis[0].z = out->basis[2].x;
    out->basis[2].x = t;
    t = out->basis[1].z;
    out->basis[1].z = out->basis[2].y;
    out->basis[2].y = t;
    MulVecT(&out->origin, &m->origin, &m->Mat());
    out->origin.x *= -1.0f;
    out->origin.y *= -1.0f;
    out->origin.z *= -1.0f;
}
static inline void InvertR(PlainValMatrix* const& out, const PlainValMatrix* m) {  /* Helper name unproven: Invert with the destination by reference */
    out->Mat() = m->Mat();
    f32 t = out->basis[0].y;
    out->basis[0].y = out->basis[1].x;
    out->basis[1].x = t;
    t = out->basis[0].z;
    out->basis[0].z = out->basis[2].x;
    out->basis[2].x = t;
    t = out->basis[1].z;
    out->basis[1].z = out->basis[2].y;
    out->basis[2].y = t;
    MulVecT(&out->origin, &m->origin, &m->Mat());
    out->origin.x *= -1.0f;
    out->origin.y *= -1.0f;
    out->origin.z *= -1.0f;
}

/* Helper name unproven. */
static inline void LoadProjectedTexture(Mat3& rotv, const PlainValMatrix& pose) {
    rotv = pose.Mat();
    f32 t = rotv.m[0][1];
    rotv.m[0][1] = rotv.m[1][0];
    rotv.m[1][0] = t;
    t = rotv.m[0][2];
    rotv.m[0][2] = rotv.m[2][0];
    rotv.m[2][0] = t;
    t = rotv.m[1][2];
    rotv.m[1][2] = rotv.m[2][1];
    rotv.m[2][1] = t;
    f32 m[2][4] = {{0.0f, 0.0f, 0.0f, 0.5f}, {0.0f, 0.0f, 0.0f, 0.5f}};
    m[0][0] = 0.5f * rotv.m[0][0];
    m[0][1] = 0.5f * rotv.m[0][1];
    m[0][2] = 0.5f * rotv.m[0][2];
    m[1][0] = 0.5f * rotv.m[1][0];
    m[1][1] = 0.5f * rotv.m[1][1];
    m[1][2] = 0.5f * rotv.m[1][2];
    GXLoadTexMtxImm((const f32 (*)[4])(m), (u32)0x2A, (GXMtxType)1);
}

void fn_802D1C04(void) {
    if (!lbl_8055FD70) {
        return;
    }
    lbl_8055FD70 = 0;
    PlainValMatrix inv;
    Invert(&inv, &lbl_804C56C0);
    PlainValMatrix w;
    fn_801BFD34(&w.origin, &lbl_804C5690.origin, &inv);
    Mat3Mul(&w.Mat(), &lbl_804C5690.Mat(), &inv.Mat());
    fn_802D0DD4(lbl_804C5650, &w);
    GXLoadPosMtxImm((const f32 (*)[4])(lbl_804C5650), (u32)0);
    if (lbl_8055FD19) {
        lbl_8055FD19 = 0;
        GXSetCurrentMtx((u32)0);
    }
    switch (lbl_804C5700.mode) {
    case 2:
    case 3: {
        // Projected texture: express the model frame in the projector's look-at frame.
        f32 b[3][4];
        PlainValMatrix look;
        PlainValMatrix li;
        PlainValMatrix lw;
        f32 m[3][4];
        f32 t[3][4];
        f32 s[3][4];
        f32 st[3][4];
        fn_802C8588(&look, &lbl_804C56C0.origin, &lbl_804C5690.origin);
        Invert(&li, &look);
        fn_801BFD34(&lw.origin, &lbl_804C5690.origin, &li);
        Mat3Mul(&lw.Mat(), &lbl_804C5690.Mat(), &li.Mat());
        fn_802D0DD4(m, &lw);
        PSMTXInvXpose((const f32 (*)[4])(m), (f32 (*)[4])(b));
        PSMTXScale((f32 (*)[4])(s), 0.5f, -0.5f, 0.0f);
        PSMTXTrans((f32 (*)[4])(t), 0.5f, 0.5f, 1.0f);
        PSMTXConcat((const f32 (*)[4])(t), (const f32 (*)[4])(s), (f32 (*)[4])(st));
        GXLoadTexMtxImm((const f32 (*)[4])(b), (u32)0x2A, (GXMtxType)0);
        GXLoadTexMtxImm((const f32 (*)[4])(st), (u32)0x4C, (GXMtxType)0);
        break;
    }
    case 1:
    case 5: {
        Mat3 rotv;
        LoadProjectedTexture(rotv, lbl_804C5690);
        break;
    }
    case 4:
        fn_802D18E8();
        break;
    }
    f32 n[3][4];
    fn_802D0DD4(n, &lbl_804C5690);
    n[0][3] = n[1][3] = n[2][3] = 0.0f;
    GXLoadNrmMtxImm((const f32 (*)[4])(n), (u32)0);
    if (lbl_804C5700.mode == 2) {
        fn_802D1B58();
    }
}

void fn_802D2218(void) {
    if (!lbl_804C9778.overlayC && !lbl_804C9778.overlayZ) {
        return;
    }
    fn_802D1678();
    PlainValMatrix id;
    PlainValMatrix* const identity = &id;
    identity->basis[0].x = 1.0f;
    identity->basis[0].y = 0.0f;
    identity->basis[0].z = 0.0f;
    identity->basis[1].x = 0.0f;
    identity->basis[1].y = 1.0f;
    identity->basis[1].z = 0.0f;
    identity->basis[2].x = 0.0f;
    identity->basis[2].y = 0.0f;
    identity->basis[2].z = 1.0f;
    identity->origin.x = 0.0f;
    identity->origin.y = 0.0f;
    identity->origin.z = 0.0f;
    f32 pm[3][4];
    fn_802D0DD4(pm, identity);
    GXLoadPosMtxImm((const f32 (*)[4])(pm), (u32)0);
    f32 hw = 0.5f * lbl_804C9778.vw;
    f32 hh = 0.5f * lbl_804C9778.vh;
    f32 o[4][4];
    C_MTXOrtho((f32 (*)[4])(o), hh, -hh, -hw, hw, 1.0f, 2.0f);
    GXSetProjection((const f32 (*)[4])(o), (GXProjectionType)1);
    GXSetDstAlpha((GXBool)0, (u8)0);
    GXSetBlendMode((GXBlendMode)0, (GXBlendFactor)1, (GXBlendFactor)0, (GXLogicOp)5);
    GXSetColorUpdate((GXBool)lbl_804C9778.overlayC);
    GXSetAlphaUpdate((GXBool)lbl_804C9778.overlayC);
    GXSetZMode((GXBool)1, (GXCompare)GX_ALWAYS, (GXBool)lbl_804C9778.overlayZ);
    GXSetFog((GXFogType)GX_FOG_NONE, lbl_804C97B0.color, 0.0f, 0.0f, 0.0f, 0.0f);
    GXSetNumTevStages((u8)1);
    GXSetNumTexGens((u8)0);
    GXSetTevColor((GXTevRegID)1, (GXColor)lbl_804C9778.overlay);
    GXSetTevOrder((GXTevStageID)0, (GXTexCoordID)0xFF, (GXTexMapID)0xFF, (GXChannelID)0xFF);
    GXSetTevColorIn((GXTevStageID)0, (GXTevColorArg)0xF, (GXTevColorArg)0xF, (GXTevColorArg)0xF, (GXTevColorArg)2);
    GXSetTevColorOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)0, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)0, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)1);
    GXSetTevAlphaOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)0, (GXTevRegID)0);
    GXSetCullMode((GXCullMode)0);
    GXClearVtxDesc();
    GXSetVtxDesc((GXAttr)9, (GXAttrType)1);
    GXSetVtxAttrFmt((GXVtxFmt)0, (GXAttr)9, (GXCompCnt)1, (GXCompType)4, (u8)0);
    GXBegin((GXPrimitive)0x98, (GXVtxFmt)0, (u16)4);
    for (int i = 0; i < 4; i++) {
        f32 p[3];
        p[0] = hw * (i & 1 ? -1 : 1);
        p[1] = hh * (i & 2 ? -1 : 1);
        const f32 x = p[0];
        WGPIPE.f = x;
        WGPIPE.f = p[1];
        p[2] = 1.99999f;
        WGPIPE.f = p[2];
    }
    GXSetNumTexGens((u8)1);
    GXSetTevOrder((GXTevStageID)0, (GXTexCoordID)0, (GXTexMapID)0, (GXChannelID)4);
    GXSetTevOp((GXTevStageID)0, (GXTevMode)0);
    GXSetDstAlpha((GXBool)1, (u8)lbl_8055FD60);
    lbl_804C9778.overlayZ = 0;
    lbl_804C9778.overlayC = 0;
    fn_802CFF50();
    lbl_804C9778.dirty = 1;
    fn_802D14D8();
    lbl_804C5700.dirty = 1;
    lbl_8055FD70 = 1;
    lbl_804C97D0.dirty = 1;
    lbl_804C97B0.dirty = 1;
    fn_802D6C68();
}

void fn_802D2614(void) {
    fn_802D14D8();
    fn_802D1678();
    fn_802D2218();
    fn_802D0EEC();
}

void fn_802D2640(void) {
    GXSetNumIndStages((u8)0);
    for (int i = 0; i < 4; i++) {
        GXSetTevDirect((GXTevStageID)i);
    }
}

#pragma push
#pragma opt_strength_reduction off
void fn_802D2684(void) {
    u8 env = lbl_804C5700.env;
    u8 envA;
    u8 base2;
    base2 = lbl_804C5700.base2;
    envA = lbl_804C5700.envA;
    int stage = 0;
    int alphaOnly = (lbl_8055FD1C >> 6) & 1;
    int coord = 0;
    GXSetTexCoordGen2((GXTexCoordID)0, (GXTexGenType)1, (GXTexGenSrc)4, (u32)(lbl_804C5700.lmap ? 0x1E : 0x3C), (GXBool)0, (u32)0x7D);
    GXSetTevOrder((GXTevStageID)0, (GXTexCoordID)0, (GXTexMapID)0, (GXChannelID)4);
    if (alphaOnly) {
        GXSetTevOp((GXTevStageID)0, (GXTevMode)3);
    } else if (env) {
        GXSetTevOp((GXTevStageID)0, (GXTevMode)3);
        coord = 1;
        GXSetTexCoordGen2((GXTexCoordID)1, (GXTexGenType)1, (GXTexGenSrc)5, (u32)0x3C, (GXBool)0, (u32)0x7D);
        stage = 1;
        GXSetTevOrder((GXTevStageID)1, (GXTexCoordID)1, (GXTexMapID)3, (GXChannelID)4);
        GXSetTevOp((GXTevStageID)1, (GXTevMode)0);
    } else {
        int in[4];
        GXSetTevColorOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
        GXSetTevColorIn((GXTevStageID)0, (GXTevColorArg)0xF, (GXTevColorArg)8, (GXTevColorArg)0xA, (GXTevColorArg)0xF);
        int n = 0;
        if (0x100 & lbl_8055FD1C) {
            in[0] = 4;
            n = 1;
        }
        if (lbl_804C97D0.alpha < 0xFF) {
            in[n++] = 1;
        }
        if (!lbl_8055FD25 && n < 2) {
            in[n++] = 5;
        }
        for (; n < 2; n++) {
            in[n] = 6;
        }
        GXSetTevAlphaOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
        GXSetTevAlphaIn((GXTevStageID)0, (GXTevAlphaArg)7, (GXTevAlphaArg)(in[0]), (GXTevAlphaArg)(in[1]), (GXTevAlphaArg)7);
    }
    if (base2) {
        stage++;
        GXSetTevOrder((GXTevStageID)stage, (GXTexCoordID)0, (GXTexMapID)1, (GXChannelID)4);
        GXSetTevColorOp((GXTevStageID)stage, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
        GXSetTevColorIn((GXTevStageID)stage, (GXTevColorArg)0, (GXTevColorArg)0xF, (GXTevColorArg)0xF, (GXTevColorArg)0xF);
        GXSetTevAlphaOp((GXTevStageID)stage, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
        GXSetTevAlphaIn((GXTevStageID)stage, (GXTevAlphaArg)7, (GXTevAlphaArg)0, (GXTevAlphaArg)4, (GXTevAlphaArg)7);
    }
    if (envA) {
        coord++;
        GXSetTexCoordGen2((GXTexCoordID)coord, (GXTexGenType)1, (GXTexGenSrc)0, (u32)0x2D, (GXBool)0, (u32)0x7D);
        stage++;
        GXSetTevOrder((GXTevStageID)stage, (GXTexCoordID)coord, (GXTexMapID)5, (GXChannelID)0xFF);
        GXSetTevColorIn((GXTevStageID)stage, (GXTevColorArg)0, (GXTevColorArg)8, (GXTevColorArg)9, (GXTevColorArg)0xF);
        GXSetTevColorOp((GXTevStageID)stage, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
        GXSetTevAlphaIn((GXTevStageID)stage, (GXTevAlphaArg)0, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7);
        GXSetTevAlphaOp((GXTevStageID)stage, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    }
    GXSetNumTevStages((u8)(stage + 1));
    GXSetNumTexGens((u8)(coord + 1));
    if (lbl_804C5700.ind) {
        GXSetBlendMode((GXBlendMode)1, (GXBlendFactor)4, (GXBlendFactor)5, (GXLogicOp)5);
        GXSetNumIndStages((u8)1);
        GXSetTevIndirect((GXTevStageID)0, (GXIndTexStageID)0, (GXIndTexFormat)0, (GXIndTexBiasSel)3, (GXIndTexMtxID)1, (GXIndTexWrap)0, (GXIndTexWrap)0, (GXBool)0, (GXBool)0, (GXIndTexAlphaSel)0);
        GXSetIndTexOrder((GXIndTexStageID)0, (GXTexCoordID)1, (GXTexMapID)2);
        GXSetNumTexGens((u8)2);
        GXSetTexCoordGen2((GXTexCoordID)1, (GXTexGenType)1, (GXTexGenSrc)0, (u32)0x21, (GXBool)0, (u32)0x7D);
        lbl_8055FD80 = fn_802D2640;
    }
}
#pragma pop

void fn_802D2AC4(void) {
    GXSetNumChans((u8)1);
    GXSetChanCtrl((GXChannelID)1, (GXBool)0, (GXColorSrc)0, (GXColorSrc)0, (GXLightID)0, (GXDiffuseFn)0, (GXAttnFn)2);
    fn_802DFAF4();
}

static inline bool TexChanged(void) { return memcmp(&lbl_804C9930, &lbl_804C5700, sizeof(TexState)) != 0; }

void fn_802D2B0C(void) {
    if (!lbl_8055FDB4 && TexChanged()) {
        if (lbl_8055FD80) {
            lbl_8055FD80();
            lbl_8055FD80 = 0;
        }
        switch (lbl_804C5700.mode) {
        case 1:
            lbl_8055FD70 = 1;
            fn_802DF21C();
            lbl_8055FD80 = fn_802DF2F0;
            break;
        case 5:
            lbl_8055FD70 = 1;
            fn_802E09EC();
            lbl_8055FD80 = fn_802E0B68;
            break;
        case 2:
            lbl_8055FD70 = 1;
            if (lbl_804C5700.envA) {
                fn_802DF300();
                lbl_8055FD80 = fn_802DF46C;
            } else {
                fn_802DEFA0();
                lbl_8055FD80 = fn_802DF1B4;
            }
            break;
        case 4:
            fn_802D18E8();
            fn_802DF994();
            lbl_8055FD80 = fn_802D2AC4;
            lbl_8055FD70 = 1;
            break;
        case 3:
            if (lbl_8055FD86 == lbl_8055FD0C) {
                fn_802D2684();
            } else {
                if (lbl_804C5700.base2) {
                    fn_802DF6D0();
                    lbl_8055FD80 = fn_802DF938;
                } else {
                    fn_802DF4A8();
                    lbl_8055FD80 = fn_802DF684;
                }
                lbl_8055FD70 = 1;
            }
            break;
        case 0:
            fn_802D2684();
            break;
        }
        lbl_804C5700.dirty = 0;
        lbl_804C9930 = lbl_804C5700;
    }
    fn_802D1C04();
}

V3* fn_802D2D04(void) { return &lbl_804C97D0.dir; }

GXRenderModeObj* fn_802D2D14(void) { return lbl_8055FD64; }

void fn_802D2D1C(void) {
    const PlainValMatrix* camera = &lbl_804C56C0;
    PlainValMatrix* const world = &lbl_804CA2D8;
    InvertR(world, camera);
    GXSetCurrentMtx((u32)0);
}

void fn_802D2ED8(const PlainValMatrix* m, int slot) {
    if (slot == 0) {
        lbl_8055FD70 = 1;
        lbl_8055FD8C = -1;
    }
    PlainValMatrix t;
    fn_801BFD34(&t.origin, &m->origin, &lbl_804CA2D8);
    Mat3Mul(&t.Mat(), &m->Mat(), &lbl_804CA2D8.Mat());
    fn_802D0DD4(lbl_804C5650, &t);
    GXLoadPosMtxImm((const f32 (*)[4])(lbl_804C5650), (u32)(slot * 3));
}

void fn_802D2F70(const PlainValMatrix* m) { fn_802D2ED8(m, 0); }

u16* fn_802D2F78(s32* w, s32* h) {
    GXDrawDone();
    VIFlush();
    lbl_804C9760.hold = 1;
    *w = lbl_804C9760.w;
    *h = lbl_804C9760.h;
    return lbl_804C9760.cur;
}

void fn_802D2FD8(void) {
    lbl_804C9760.hold = 0;
    DCFlushRange((const void*)lbl_804C9760.cur, (u32)(lbl_804C9760.bpp * (lbl_804C9760.w * lbl_804C9760.h)));
    VISetNextFrameBuffer((void*)lbl_804C9760.cur);
    VIFlush();
    VIWaitForRetrace();
    VISetBlack((BOOL)0);
    lbl_804C9760.cur = lbl_804C9760.buf;
}

void fn_802D3050(int on) { lbl_8055FD1C = on ? lbl_8055FD1C | 0x10 : lbl_8055FD1C & ~0x10; }

void fn_802D306C(u8 a) {
    lbl_8055FD60 = a;
    GXSetDstAlpha((GXBool)1, (u8)a);
}

void fn_802D307C(void) {
    f32 m[2][3] = {{0.5f, 0.0f, 0.0f}, {0.0f, 0.5f, 0.0f}};
    GXSetIndTexMtx((GXIndTexMtxID)1, m, (s8)1);
}

void fn_802D30DC(void* p) { lbl_8055FD5C = p; }

void* fn_802D30E4(void) { return (u8*)lbl_8055FD54 + 0x24; }

void* fn_802D30F0(void) { return (u8*)lbl_8055FE1C + 0x24; }

void* fn_802D30FC(void) { return fn_802D30F0(); }

void* fn_802D3100(void) { return fn_802D30F0(); }

void* fn_802D3104(void) {
    fn_802D307C();
    return (u8*)lbl_8055FD5C + 0x24;
}

void* fn_802D312C(void) { return fn_802D3104(); }

void* fn_802D3130(void) { return fn_802D3104(); }

void* fn_802D3134(void) { return fn_802D3104(); }

void* fn_802D3138(void) { return (u8*)lbl_8055FD58 + 0x24; }

void fn_802D3144(void) {
    ApplyBlend();
    lbl_8055FD88 = lbl_8055FD86;
    lbl_804C5700.dirty = 1;
}

int fn_802D31D8(int) {
    lbl_8055FDB4 = 1;
    GXSetColorUpdate((GXBool)0);
    GXSetAlphaUpdate((GXBool)1);
    GXSetDstAlpha((GXBool)0, (u8)0);
    fn_802CF5D4();
    lbl_8055FD1C &= ~0x8;
    GXSetNumTevStages((u8)1);
    GXSetNumTexGens((u8)0);
    GXSetZMode((GXBool)1, (GXCompare)GX_GEQUAL, (GXBool)0);
    GXSetTevOrder((GXTevStageID)0, (GXTexCoordID)0xFF, (GXTexMapID)0xFF, (GXChannelID)4);
    GXSetTevAlphaIn((GXTevStageID)0, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)1);
    GXSetTevAlphaOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)0, (GXTevRegID)0);
    GXSetTevColorOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevColorIn((GXTevStageID)0, (GXTevColorArg)0xF, (GXTevColorArg)0xF, (GXTevColorArg)0xF, (GXTevColorArg)0xF);
    GXSetTevColor((GXTevRegID)1, (GXColor)(fn_802CE9FC(0, 0, 0, 4)));
    return 1;
}

void fn_802D3304(void) {
    lbl_804C5700.dirty = 1;
    fn_802CFF50();
    lbl_8055FDB4 = 0;
    GXSetCullMode((GXCullMode)0);
    GXSetTevOrder((GXTevStageID)0, (GXTexCoordID)0, (GXTexMapID)0, (GXChannelID)4);
    GXSetTevOp((GXTevStageID)0, (GXTevMode)0);
}

void fn_802D3360(void) {
    fn_802CF5D4();
    lbl_8055FD84 = 1;
    lbl_804C5700.dirty = 1;
    fn_802CFF50();
    GXSetTexCoordGen2((GXTexCoordID)0, (GXTexGenType)1, (GXTexGenSrc)4, (u32)0x3C, (GXBool)0, (u32)0x7D);
    for (int i = 0; i < 4; i++) {
        GXSetTexCoordGen2((GXTexCoordID)i, (GXTexGenType)1, (GXTexGenSrc)4, (u32)0x3C, (GXBool)0, (u32)0x7D);
    }
    fn_802D2640();
    if (lbl_8055FE1C) {
        fn_802D92B0();
    }
    GXSetTevOrder((GXTevStageID)0, (GXTexCoordID)0, (GXTexMapID)0, (GXChannelID)4);
    GXSetTevOp((GXTevStageID)0, (GXTevMode)0);
    GXSetCurrentMtx((u32)0);
}

void fn_802D3420(void) {
    PlainValMatrix id;
    PlainValMatrix* const identity = &id;
    identity->basis[0].x = 1.0f;
    identity->basis[0].y = 0.0f;
    identity->basis[0].z = 0.0f;
    identity->basis[1].x = 0.0f;
    identity->basis[1].y = 1.0f;
    identity->basis[1].z = 0.0f;
    identity->basis[2].x = 0.0f;
    identity->basis[2].y = 0.0f;
    identity->basis[2].z = 1.0f;
    identity->origin.x = 0.0f;
    identity->origin.y = 0.0f;
    identity->origin.z = 0.0f;
    f32 pm[3][4];
    fn_802D0DD4(pm, identity);
    GXLoadPosMtxImm((const f32 (*)[4])(pm), (u32)0);
    f32 o[4][4];
    C_MTXOrtho((f32 (*)[4])(o), 0.0f, 480.0f, 0.0f, 640.0f, 1.0f, 2.0f);
    GXSetProjection((const f32 (*)[4])(o), (GXProjectionType)1);
    GXSetCullMode((GXCullMode)0);
    GXClearVtxDesc();
    GXSetVtxDesc((GXAttr)9, (GXAttrType)1);
    GXSetVtxDesc((GXAttr)0xD, (GXAttrType)1);
    GXSetVtxAttrFmt((GXVtxFmt)0, (GXAttr)9, (GXCompCnt)1, (GXCompType)4, (u8)0);
    GXSetVtxAttrFmt((GXVtxFmt)0, (GXAttr)0xD, (GXCompCnt)1, (GXCompType)4, (u8)0);
    f32 u = (f32)lbl_804C9778.vw / 640.0f;
    f32 v = u * (f32)lbl_804C9778.vh / 448.0f;
    GXBegin((GXPrimitive)0x80, (GXVtxFmt)0, (u16)4);
    WGPIPE.f = 0.0f;
    WGPIPE.f = 0.0f;
    WGPIPE.f = 1.5f;
    WGPIPE.f = 0.0f;
    WGPIPE.f = 0.0f;
    WGPIPE.f = 640.0f;
    WGPIPE.f = 0.0f;
    WGPIPE.f = 1.5f;
    WGPIPE.f = u;
    WGPIPE.f = 0.0f;
    WGPIPE.f = 640.0f;
    WGPIPE.f = 480.0f;
    WGPIPE.f = 1.5f;
    WGPIPE.f = u;
    WGPIPE.f = v;
    WGPIPE.f = 0.0f;
    WGPIPE.f = 480.0f;
    WGPIPE.f = 1.5f;
    WGPIPE.f = 0.0f;
    WGPIPE.f = v;
    fn_802D6C68();
    lbl_804C9778.dirty = 1;
    lbl_8055FD70 = 1;
}

void fn_802D362C(u32 color) {
    GXSetTexCopySrc((u16)lbl_804C9778.vx, (u16)lbl_804C9778.vy, (u16)lbl_804C9778.vw, (u16)lbl_804C9778.vh);
    GXSetTexCopyDst((u16)lbl_804C9778.vw, (u16)lbl_804C9778.vh, (GXTexFmt)0x27, (GXBool)0);
    GXCopyTex((void*)lbl_804C9940.buf, (GXBool)0);
    GXPixModeSync();
    GXSetColorUpdate((GXBool)1);
    GXSetAlphaUpdate((GXBool)0);
    GXSetDstAlpha((GXBool)0, (u8)0);
    GXSetNumTevStages((u8)1);
    GXSetNumTexGens((u8)1);
    GXLoadTexObj((const GXTexObj*)&lbl_804C9940.obj, (GXTexMapID)0);
    GXSetTevOrder((GXTevStageID)0, (GXTexCoordID)0, (GXTexMapID)0, (GXChannelID)4);
    GXSetTevColorOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevColorIn((GXTevStageID)0, (GXTevColorArg)0xF, (GXTevColorArg)0xF, (GXTevColorArg)0xF, (GXTevColorArg)2);
    GXSetTevAlphaOp((GXTevStageID)0, (GXTevOp)0xE, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)0, (GXTevAlphaArg)4, (GXTevAlphaArg)7, (GXTevAlphaArg)1, (GXTevAlphaArg)7);
    GXSetBlendMode((GXBlendMode)1, (GXBlendFactor)4, (GXBlendFactor)5, (GXLogicOp)3);
    GXSetZMode((GXBool)0, (GXCompare)GX_LEQUAL, (GXBool)0);
    GXSetTexCoordGen2((GXTexCoordID)0, (GXTexGenType)1, (GXTexGenSrc)0xC, (u32)0x3C, (GXBool)0, (u32)0x7D);
    GXSetTevColor((GXTevRegID)1, (GXColor)(fn_802CEA1C(color)));
    fn_802D3420();
}

void fn_802D37E4(void) {
    for (int i = 0; i < 0x40; i++) {
        lbl_804C9984.tests[i].used = 0;
        lbl_804C9984.tests[i].hit = 0;
    }
    lbl_804C9984.count = 0;
}

void fn_802D3894(void) {
    VisTest* t = lbl_804C9984.tests;
    for (int i = 0; i < 0x40; i++) {
        if (t->used) {
            fn_802E317C("visibility test still in use");
        }
        t++;
    }
}

void fn_802D3900(void) {
    for (int i = 0; i < lbl_804C9984.count; i++) {
        VisTest* t = lbl_804C9984.queue[i];
        u32 z;
        GXPeekZ((u16)t->x, (u16)t->y, (u32*)&z);
        t->hit = t->z < z;
    }
    lbl_804C9984.count = 0;
}

VisTest* fn_802D39A4(void*, f32 rate) {
    VisTest* t = 0;
    for (int i = 0; i < 0x40; i++) {
        if (!lbl_804C9984.tests[i].used) {
            t = &lbl_804C9984.tests[i];
            break;
        }
    }
    if (!t) {
        fn_802E317C("exceeded visibility test limit");
    }
    t->used = 1;
    t->hit = 0;
    t->rate = rate;
    t->vis = 0.0f;
    return t;
}

void fn_802D3A50(VisTest* t) { t->used = 0; }

static inline f32 VisLerp(f32 a, f32 b, f32 k) { return a + k * (b - a); }

int fn_802D3A5C(VisTest* t, const V3* p) {
    int x = p->x;
    int y = 448.0f * p->y / 480.0f;
    f32 target = t->hit ? 1.0f : 0.0f;
    if (x >= 0 && x < 0x280 && y >= 0 && y < 0x1C0) {
        t->x = x;
        t->y = y;
        f32 f = lbl_804C9778.farZ;
        f32 n = lbl_804C9778.nearZ;
        f32 z = -p->z;
        f32 range = f - n;
        t->z = __cvt_fp2unsigned(16777215.0f + 16777215.0f * ((z * (-n / range) - f * n / range) / -z));
        if (lbl_804C9984.count < 0x40) {
            lbl_804C9984.queue[lbl_804C9984.count] = t;
            lbl_804C9984.count++;
        }
    } else {
        t->hit = 0;
    }
    t->vis = VisLerp(t->vis, target, t->rate);
    return t->vis >= 0.007843138f;
}

void fn_802D3BB0(void*) {}

struct QuadVtx {
    V3 pos;
    u32 color;
    f32 u, v;
};

void fn_802D3BB4(VisTest* t, f32 r, const V3* c, const V3* a, const V3* b, u32 color) {
    int alpha = t->vis * (color >> 24);
    u32 col = (alpha << 24) | (color & 0xFFFFFF);
    u16 i[6] = {0, 1, 2, 1, 2, 3};
    QuadVtx v[4];
    v[0].pos.x = c->x + a->x * r;
    v[0].pos.y = c->y + a->y * r;
    v[0].pos.z = c->z + a->z * r;
    v[1].pos.x = c->x + b->x * r;
    v[1].pos.y = c->y + b->y * r;
    v[1].pos.z = c->z + b->z * r;
    v[2].pos.x = c->x + b->x * -r;
    v[2].pos.y = c->y + b->y * -r;
    v[2].pos.z = c->z + b->z * -r;
    v[3].pos.x = c->x + a->x * -r;
    v[3].pos.y = c->y + a->y * -r;
    v[3].pos.z = c->z + a->z * -r;
    v[0].color = v[1].color = v[2].color = v[3].color = col;
    v[0].u = v[2].u = 0.0f;
    v[1].u = v[3].u = 1.0f;
    v[0].v = v[1].v = 0.0f;
    v[2].v = v[3].v = 1.0f;
    fn_802D68C4(v, 4, i, 6);
}

void fn_802D3D50(void) {
    lbl_804C97B0.range = 0;
    lbl_804C97B0.dirty = 1;
}

void fn_802D3D6C(void) { lbl_804C5700.lmap = 1; }

void fn_802D3D80(void) { lbl_804C5700.lmap = 0; }

void fn_802D3D94(int i, const f32 m[2][4]) { GXLoadTexMtxImm((const f32 (*)[4])(m), (u32)(i * 3 + 0x1E), (GXMtxType)1); }

void fn_802D3DAC(void) {
    static s32 overflows;
    if ((overflows & 0x3F) == 0) {
        OSReport((const char*)(" gx: overflowed GX FIFO! (%d)\n"), overflows);
    }
    overflows++;
    GXResetOverflowCount();
    GXDisableBreakPt();
}

void fn_802D3DF8(GXTexObj* tex, f32 u, f32 v, s8 scale) {
    lbl_804C5700.ind = 1;
    GXLoadTexObj((const GXTexObj*)((GXTexObj*)((u8*)tex + 0x24)), (GXTexMapID)2);
    f32 m[2][3] = {{u, 0.0f, 0.0f}, {0.0f, v, 0.0f}};
    GXSetIndTexMtx((GXIndTexMtxID)1, m, (s8)scale);
}

void fn_802D3E88(void) { lbl_804C5700.ind = 0; }

void fn_802D3E9C(u8 on) {
    lbl_8055FD50 = on;
    lbl_8055FD4C = 0;
    lbl_804C5710.active = 0;
    lbl_804C5710.pending = 0;
}

int fn_802D3EBC(void) { return lbl_8055D0B8 >= 0; }

void fn_802D3ECC(u16 blend) { lbl_8055FD86 = blend; }

}  // extern "C"


struct HeapBlock {
    u32 magic;        // offset 0x0
    s32 size;         // offset 0x4
    HeapBlock* next;  // offset 0x8
    void* heap;       // offset 0xC
};

struct SimpleHeap {
    ~SimpleHeap(void) {
        if (used) {
            Dump("~SimpleHeap");
            fn_802E317C("SimpleHeap Destroyed with pending allocations");
        }
    }
    void* Alloc(u32 size) {
        s32 n = (size + align) & ~align;
        HeapBlock** best = 0;
        s32 bestSize = 0x7FFFFFFF;
        void* res = 0;
        for (HeapBlock** l = &free; *l; l = &(*l)->next) {
            HeapBlock* b = *l;
            if (b->size >= n && bestSize > b->size) {
                best = l;
                bestSize = b->size;
            }
        }
        if (best) {
            HeapBlock* b = *best;
            HeapBlock* split = 0;
            HeapBlock* next = b->next;
            s32 rest = b->size - n;
            s32 hdr = (align + 0x10) & ~align;
            if (rest > hdr + 0x20) {
                HeapBlock* s = (HeapBlock*)((u8*)b + align + n + 1);
                s->magic = 0xABCFEED0;
                s->next = 0;
                s->size = rest - ((align + 0x10) & ~align);
                s->heap = this;
                b->size = n;
                split = s;
            }
            if (split) {
                split->next = next;
                *best = split;
            } else {
                *best = next;
            }
            b->next = used;
            used = b;
            res = (u8*)b + align + 1;
        }
        if (!res && active) {
            fn_802E317C("SimpleHeap: out of memory for %d byte allocation.", n);
        }
        return res;
    }
    void Dump(const char* who) {
        fn_802E30C8("SimpleHeap dump: %s", who);
        fn_802E30C8("Allocated:");
        u32 total = 0;
        for (HeapBlock* b = used; b; b = b->next) {
            fn_802E30C8("	user ptr %8.8p: %d", b + 1, b->size);
            total += b->size;
        }
        fn_802E30C8("	%dK total", total >> 10);
        u32 nfree = 0;
        for (HeapBlock* b = free; b; b = b->next) {
            fn_802E30C8("	user ptr %8.8p: %d", b + 1, b->size);
            nfree += b->size;
        }
        fn_802E30C8("	%dK total", nfree >> 10);
    }
    u8 active;        // offset 0x0
    u32 align;        // offset 0x4
    HeapBlock* free;  // offset 0x8
    HeapBlock* used;  // offset 0xC
};

/* Helper names unproven. gx saw the SimpleHeap class (same as tevbink.cpp) and two unused static users: MWCC
   drops the dead functions but keeps the strings of the inlined methods at the end of .data. */
static void HeapDelete(SimpleHeap* h) { delete h; }
static void* HeapAlloc(SimpleHeap* h, u32 n) { return h->Alloc(n); }
