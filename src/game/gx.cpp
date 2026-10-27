// GX driver core (0x802CE9FC-0x802D3F34, "gx.cpp"): video and GX init/shutdown, the render mode choice,
// the per-frame begin/end (dynamic frame memory, draw sync, hang watchdog, display copy), lazy state
// (z/blend/alpha flags, fog, lights, projection, viewport, camera and texture-matrix uploads), the
// texture-environment presets, region screenshots, visibility (z peek) tests and a couple of debug
// helpers. Names are guesses apart from the strings.
#include "game/common.h"

struct GXColor {
    u8 r, g, b, a;
};

struct GXTexObj {
    u32 dummy[8];
};

struct GXLightObj {
    u32 dummy[16];
};

struct GXFifoObj {
    u8 pad[0x80];
};

struct GXRenderModeObj {
    s32 viTVmode;     // offset 0x0
    u16 fbWidth;      // offset 0x4
    u16 efbHeight;    // offset 0x6
    u16 xfbHeight;    // offset 0x8
    u16 viXOrigin;    // offset 0xA
    u16 viYOrigin;    // offset 0xC
    u16 viWidth;      // offset 0xE
    u16 viHeight;     // offset 0x10
    u8 pad12[2];
    s32 xFBmode;      // offset 0x14
    u8 field_rendering;  // offset 0x18
    u8 aa;               // offset 0x19
    u8 sample_pattern[12][2];  // offset 0x1A
    u8 vfilter[7];             // offset 0x32
    u8 pad39[3];
};

struct V3 {
    f32 x, y, z;
};

struct Mat3 {
    f32 m[3][3];
};

// Rigid transform: rotation then position.
struct RMtx {
    Mat3 rot;  // offset 0x0
    V3 pos;    // offset 0x24
};

extern "C" {
void* memset(void* dst, int c, size_t n);
void* memcpy(void* dst, const void* src, size_t n);
int memcmp(const void* a, const void* b, size_t n);
double tan(double x);
double atan(double x);
void OSReport(const char* fmt, ...);
void OSPanic(const char* file, int line, const char* fmt, ...);
s64 fn_802E2C9C(void);  // OSGetTime
f32 __cvt_sll_flt(s64 v);
u32 __cvt_fp2unsigned(f64 v);
void DCInvalidateRange(void* p, u32 n);
void DCFlushRange(void* p, u32 n);
void fn_802E317C(const char* fmt, ...);  // fatal
void fn_802E30C8(const char* fmt, ...);  // warning print
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
void fn_801BFD34(V3* out, const V3* in, const RMtx* m);
void Mat3Mul__FP4Mat3PC4Mat3PC4Mat3(Mat3* out, const Mat3* a, const RMtx* b);
void fn_802C8588(void* out, const V3* a, const V3* b);
void GXInitLightPos(GXLightObj* l, f32 x, f32 y, f32 z);
void GXInitLightDistAttn(GXLightObj* l, f32 a, f32 b, int f);
void GXInitSpecularDirHA(GXLightObj* l, f32 nx, f32 ny, f32 nz, f32 hx, f32 hy, f32 hz);
void GXInitLightAttn(GXLightObj* l, f32 a, f32 b, f32 c, f32 d, f32 e, f32 f);
void PSVECCrossProduct(const V3* a, const V3* b, V3* out);
f32 PSVECMag(const V3* v);
void PSMTXInvXpose(const void* src, void* dst);
void PSMTXScale(void* m, f32 x, f32 y, f32 z);
void PSMTXTrans(void* m, f32 x, f32 y, f32 z);
void PSMTXConcat(const void* a, const void* b, void* ab);
void C_MTXPerspective(f32 m[4][4], f32 fovy, f32 aspect, f32 n, f32 f);
void C_MTXOrtho(f32 m[4][4], f32 t, f32 b, f32 l, f32 r, f32 n, f32 f);

s32 VIGetTvFormat(void);
s32 VIGetDTVStatus(void);
void VIInit(void);
void VIConfigure(const GXRenderModeObj* rm);
void VISetBlack(int on);
void VIFlush(void);
void VIWaitForRetrace(void);
void VISetNextFrameBuffer(void* fb);
u32 VIGetNextField(void);
void* VISetPreRetraceCallback(void (*cb)(u32));
extern GXRenderModeObj GXNtsc480IntDf;
extern GXRenderModeObj GXNtsc480Prog;
extern GXRenderModeObj GXPal528IntDf;
extern GXRenderModeObj GXEurgb60Hz480IntDf;
extern GXRenderModeObj GXMpal480IntDf;
void GXAdjustForOverscan(const GXRenderModeObj* in, GXRenderModeObj* out, u16 h, u16 v);
GXFifoObj* GXInit(void* base, u32 size);
void GXDrawDone(void);
void GXSetDrawDone(void);
void GXFlush(void);
void GXAbortFrame(void);
void GXGetCPUFifo(GXFifoObj* f);
void* GXGetFifoBase(const GXFifoObj* f);
u32 GXGetFifoSize(const GXFifoObj* f);
void GXGetFifoPtrs(const GXFifoObj* f, void** r, void** w);
void GXEnableBreakPt(void* p);
void GXDisableBreakPt(void);
void GXInvalidateVtxCache(void);
void GXInvalidateTexAll(void);
void GXSetDrawSync(u16 token);
void GXSetDrawSyncCallback(void (*cb)(u16));
void GXSetDrawDoneCallback(void (*cb)(void));
void GXReadXfRasMetric(u32* a, u32* b, u32* c, u32* d);
void GXGetGPStatus(u8* overhi, u8* underlow, u8* readIdle, u8* cmdIdle, u8* brkpt);
u32 GXGetOverflowCount(void);
u32 GXResetOverflowCount(void);
void GXSetGPMetric(int a, int b);
void GXSetViewport(f32 x, f32 y, f32 w, f32 h, f32 n, f32 f);
void GXSetViewportJitter(f32 x, f32 y, f32 w, f32 h, f32 n, f32 f, u32 field);
void GXSetScissor(u32 x, u32 y, u32 w, u32 h);
void GXSetPixelFmt(int pix, int z);
void GXSetDispCopyGamma(int g);
void GXSetCopyClear(GXColor c, u32 z);
void GXSetDither(u8 on);
void GXSetZCompLoc(u8 before);
f32 GXGetYScaleFactor(u16 efb, u16 xfb);
u32 GXSetDispCopyYScale(f32 s);
void GXSetDispCopySrc(u16 l, u16 t, u16 w, u16 h);
void GXSetDispCopyDst(u16 w, u16 h);
void GXSetCopyFilter(u8 aa, const u8 pattern[12][2], u8 vf, const u8 vfilter[7]);
void GXCopyDisp(void* dst, u8 clear);
void GXSetTexCopySrc(u16 l, u16 t, u16 w, u16 h);
void GXSetTexCopyDst(u16 w, u16 h, int fmt, u8 mip);
void GXCopyTex(void* dst, u8 clear);
void GXPixModeSync(void);
u32 GXGetTexBufferSize(u16 w, u16 h, u32 fmt, u8 mip, u8 maxLod);
void GXInitTexObj(GXTexObj* obj, void* image, u16 w, u16 h, int fmt, int ws, int wt, u8 mip);
void GXLoadTexObj(const GXTexObj* obj, int map);
void GXSetNumChans(u8 n);
void GXSetChanCtrl(int chan, u8 enable, int amb, int mat, u32 lights, int diff, int attn);
void GXSetChanMatColor(int chan, GXColor c);
void GXSetChanAmbColor(int chan, GXColor c);
void GXInitLightColor(GXLightObj* l, GXColor c);
void GXLoadLightObjImm(const GXLightObj* l, u32 id);
void GXSetNumTexGens(u8 n);
void GXSetNumTevStages(u8 n);
void GXSetNumIndStages(u8 n);
void GXSetTevDirect(int stage);
void GXSetTevOrder(int stage, int coord, int map, int color);
void GXSetTevOp(int stage, int mode);
void GXSetTevColorIn(int stage, int a, int b, int c, int d);
void GXSetTevColorOp(int stage, int op, int bias, int scale, u8 clamp, int reg);
void GXSetTevAlphaIn(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp(int stage, int op, int bias, int scale, u8 clamp, int reg);
void GXSetTevColor(int reg, GXColor c);
void GXSetTevIndirect(int stage, int ind, int fmt, int bias, int mtx, int ws, int wt, u8 addPrev, u8 utcLod,
                      int alpha);
void GXSetIndTexOrder(int stage, int coord, int map);
void GXSetIndTexMtx(int id, const f32 m[2][3], s8 exp);
void GXSetTexCoordGen2(int dst, int type, int src, u32 mtx, u8 norm, u32 pt);
void GXSetBlendMode(int type, int src, int dst, int op);
void GXSetAlphaCompare(int c0, u8 r0, int op, int c1, u8 r1);
void GXSetZMode(u8 test, int func, u8 update);
void GXSetColorUpdate(u8 on);
void GXSetAlphaUpdate(u8 on);
void GXSetDstAlpha(u8 on, u8 a);
void GXSetCullMode(int mode);
void GXSetFog(int type, f32 start, f32 end, f32 n, f32 f, GXColor c);
void GXSetProjection(const f32 m[4][4], int type);
void GXLoadPosMtxImm(const void* m, u32 id);
void GXLoadNrmMtxImm(const void* m, u32 id);
void GXLoadTexMtxImm(const void* m, u32 id, int type);
void GXSetCurrentMtx(u32 id);
void GXClearVtxDesc(void);
void GXSetVtxDesc(int attr, int type);
void GXSetVtxAttrFmt(int fmt, int attr, int cnt, int type, int frac);
void GXBegin(int prim, int fmt, u16 n);
void GXPeekZ(u16 x, u16 y, u32* z);
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
    u8 pad1[3];
    s32 mode;   // offset 0x4
    u8 base2;   // offset 0x8
    u8 env;     // offset 0x9
    u8 envA;    // offset 0xA
    u8 bB;      // offset 0xB
    u8 lmap;    // offset 0xC
    u8 ind;     // offset 0xD
    u8 padE[2];
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

struct GXLightSet {
    GXLightSet(void);
    RMtx view;      // offset 0x0
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
    u8 color[4];    // offset 0xC
    f32 radius;     // offset 0x10
};

extern s32 lbl_8055CFF0;  // screen width
extern s32 lbl_8055CFF4;  // screen height
extern u8 lbl_8055FE58;
extern void* lbl_8055FE1C;
extern void* lbl_8055FE20;
extern s32 lbl_8055FE24;
extern u8 lbl_8055FE39;
extern u32 lbl_804DB458[4];

f32 lbl_804C5650[4][4];      // loaded position matrix
RMtx lbl_804C5690;           // view
RMtx lbl_804C56C0;           // camera
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
PointLight lbl_804CA088[7];
GXLightObj lbl_804CA118[7];
RMtx lbl_804CA2D8;

struct GPMetric {
    s32 id;
    const char* name;
    s32 metric;
    const char* unit;
    s32 kind;
};

GPMetric lbl_8034BD40[6] = {
    {0x22, "Clocks", 0x10, "Vertex", 1},     {2, "Clip Clocks", 0x15, "Clock", 1},
    {3, "XF Wait In", 0x15, "Clock", 1},     {4, "XF Wait Out", 0x15, "Clock", 1},
    {5, "XF XFrm", 0x15, "Clock", 1},        {6, "XF Lit", 0x15, "Clock", 1},
};

extern s32 lbl_80558DC4[];  // FLT_EPSILON
#define FEPS (*(f32*)lbl_80558DC4)

ModuleTag lbl_8055FD00;
static inline u16 BlendKey(int v) { return v; }
u16 lbl_8055FD08 = BlendKey(0x100);
u16 lbl_8055FD0A = BlendKey(0x405);
u16 lbl_8055FD0C = BlendKey(0x401);
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
s32 lbl_8055FDD0;   // fifo overflows

f32 lbl_8055D0B0 = 99.0f;
f32 lbl_8055D0B4 = 99.0f;
s32 lbl_8055D0B8 = -1;  // screenshot tile
s32 lbl_8055D0BC = 7;
s32 lbl_8055D0C0 = 7;
u8 lbl_8055D0C4 = 1;
u16 lbl_8055D0F6 = 2;   // draw sync token
u8 lbl_8055D0FF = 1;
s32 lbl_8055D10C = -1;  // screenshot file

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
void fn_802D0910(void);
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
void fn_802CF10C(u32 count);

u32 fn_802CE9FC(u8 r, u8 g, u8 b, u8 a) {
    GXColor c;
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = a;
    return *(u32*)&c;
}

u32 fn_802CEA1C(u32 argb) { return fn_802CE9FC(argb >> 16, argb >> 8, argb, argb >> 24); }

void fn_802CEA34(void) {
    GXRenderModeObj* rm = lbl_8055FD64;
    s32 w = rm->fbWidth;
    u32 n = ((u16)(w + 15) & 0xFFF0) * rm->xfbHeight * 2;
    lbl_804C9760.w = w;
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
    GXAdjustForOverscan(lbl_8055FD64, &lbl_804C9724, 0, 0x10);
    lbl_8055FD64 = &lbl_804C9724;
    lbl_804C9724.viWidth += 10;
    lbl_804C9724.viXOrigin -= 5;
}

int fn_802CEC04(int reset) {
    if (reset == 1) {
        fn_802D9C4C();
        GXDrawDone();
        VISetBlack(1);
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
        lbl_8055D0B4 = ms + 0.95f * (lbl_8055D0B4 - ms);
        fn_802D3900();
        break;
    }
    default:
        fn_802E317C("gx: unhandled draw sync token %x", token);
        break;
    }
}

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
    u32 a0, a1, b0, b1, c0, c1, d0, d1;
    u8 brk, readIdle, cmdIdle, underlow, overhi;
    GXReadXfRasMetric(&c0, &d0, &a0, &b0);
    GXReadXfRasMetric(&c1, &d1, &a1, &b1);
    int xfWaitIn = (b1 - b0) != 0;
    int xfWaitOut = (a1 - a0) != 0;
    int clocks = d1 == d0;
    int vtx = c1 == c0;
    GXGetGPStatus(&overhi, &underlow, &readIdle, &cmdIdle, &brk);
    OSReport("GP status (%c%c%c%c%c) metric (%d%d%d%d) --> ", overhi ? 'O' : '-', underlow ? 'U' : '-',
             readIdle ? '-' : 'R', cmdIdle ? '-' : 'C', brk ? 'B' : '-', xfWaitIn, xfWaitOut, clocks, vtx);
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
    GXGetGPStatus(&overhi, &dummy, &dummy, &dummy, &dummy);
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
    fn_802CEF90();
    if (lbl_8055FD44 == 4000) {
        OSReport("gx: hang? or perhaps the disk drive has powered down...\n");
    }
    lbl_8055FD44++;
    lbl_8055FD4C++;
    if (lbl_8055FD4C < 5) {
        return;
    }
    if (!lbl_8055FD50 && !lbl_804C5710.active) {
        return;
    }
    if (lbl_804C5710.pending) {
        lbl_804C5710.pending = 0;
        DCInvalidateRange(lbl_804C9760.cur, lbl_804C9760.w * lbl_804C9760.h * 2);
        u16* src = lbl_804C9760.cur;
        for (int y = 0; y < lbl_804C5710.h; y++) {
            memcpy(&lbl_804C5710.data[y * lbl_804C5710.w],
                   &src[lbl_804C9760.w * (y + lbl_804C5710.y) + lbl_804C5710.x], lbl_804C5710.w * 2);
        }
    }
    if (lbl_804C5710.active) {
        int m = lbl_8055FD4C % 60;
        if (m != 0 && m != 45) {
            return;
        }
    } else if (lbl_8055FD4C % 2 != 0) {
        return;
    }
    u16* buf = lbl_804C9760.cur;
    DCInvalidateRange(buf, lbl_804C9760.w * lbl_804C9760.h * 2);
    if (lbl_8055FD50) {
        for (int y = (lbl_8055FD4C >> 2) & 1; y < lbl_804C9760.h; y += 2) {
            for (int x = 0; x < lbl_804C9760.w; x++) {
                int i = y * lbl_804C9760.w + x;
                int d = buf[i] - 0x400;
                int lo = buf[i] & 0xFF;
                buf[i] = lo > d ? lo : d;
            }
        }
    }
    if (lbl_804C5710.active) {
        u16* dst = lbl_804C9760.cur;
        if (lbl_8055FD4C % 60 < 30) {
            int w = lbl_804C5710.w;
            int k = 0;
            for (int y = 0; y < lbl_804C5710.h; y++) {
                u16* d = &dst[lbl_804C9760.w * (y + lbl_804C5710.y) + lbl_804C5710.x];
                u16* s = &lbl_804C5710.data[k];
                for (int x = 0; x < w; x++) {
                    d[x] = s[x];
                }
                k += w;
            }
        } else {
            u16 c = lbl_804C5710.data[0];
            int w = lbl_804C5710.w;
            for (int y = 0; y < lbl_804C5710.h; y++) {
                u16* d = &dst[lbl_804C9760.w * (y + lbl_804C5710.y) + lbl_804C5710.x];
                for (int x = 0; x < w; x++) {
                    d[x] = c;
                }
            }
        }
    }
    DCFlushRange(lbl_804C9760.cur, lbl_804C9760.bpp * (lbl_804C9760.w * lbl_804C9760.h));
}

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
    GXSetAlphaCompare(4, 0, 1, 0, 0);
}

void fn_802CF5F8(void) {}

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
    VIConfigure(lbl_8055FD64);
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
    lbl_8055FD6C = GXInit(lbl_8055FD68, 0x300000);
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
    VISetPreRetraceCallback(fn_802CF10C);
    fn_80127468();
    fn_802DA2F4();
    fn_802CFF5C(0x17);
    fn_802CFFA4(lbl_8055FD0A);
    lbl_8055FD20 = -1;
    u32 n = GXGetTexBufferSize(0x280, 0x1C0, 1, 0, 0);
    fn_801282EC(0x20);
    lbl_804C9940.buf = new ("shadow alpha mem") u8[n];
    GXInitTexObj(&lbl_804C9940.obj, lbl_804C9940.buf, 0x280, 0x1C0, 1, 0, 0, 0);
    GXSetDrawSyncCallback(fn_802CECF4);
    GXSetDrawDoneCallback(fn_802CEC44);
    fn_802D37E4();
    fn_802DD4FC();
    lbl_8055FD40 = -1;
    fn_80127468();
    fn_80127C90(fn_802D9A50, "drIdle");
    fn_802DB168();
    lbl_8055FD48 = 0;
    VISetBlack(0);
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
void fn_802D03F4(const RMtx* cam);
void fn_802D0610(s32 set);

void fn_802CF850(void) {
    fn_802CF104();
    GXSetViewport(0.0f, 0.0f, lbl_8055FD64->fbWidth, lbl_8055FD64->efbHeight, 0.0f, 1.0f);
    GXSetScissor(0, 0, lbl_8055FD64->fbWidth, lbl_8055FD64->efbHeight);
    GXSetPixelFmt(1, 0);
    GXSetDispCopyGamma(0);
    u32 c = fn_802CE9FC(0, 0, 0, 0);
    GXSetCopyClear(*(GXColor*)&c, 0xFFFFFF);
    GXSetDither(1);
    VIWaitForRetrace();
    VISetNextFrameBuffer(lbl_804C9760.buf);
    lbl_804C9760.cur = lbl_804C9760.buf;
    VIFlush();
    if (lbl_8055FD64->viTVmode & 1) {
        VIWaitForRetrace();
    }
    GXSetNumChans(1);
    GXSetNumTexGens(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevOp(0, 0);
    GXSetBlendMode(1, 4, 5, 5);
    GXSetTevOrder(1, 0, 1, 4);
    GXSetTevOp(1, 2);
    GXSetTevColorIn(1, 0, 0, 0xF, 0xF);
    GXSetTevColorOp(1, 0, 0, 0, 1, 0);
    fn_802CF5D4();
    GXSetZCompLoc(0);
    RMtx* m = &lbl_804C56C0;
    m->rot.m[0][0] = 1.0f;
    m->rot.m[0][1] = 0.0f;
    m->rot.m[0][2] = 0.0f;
    m->rot.m[1][0] = 0.0f;
    m->rot.m[1][1] = 1.0f;
    m->rot.m[1][2] = 0.0f;
    m->rot.m[2][0] = 0.0f;
    m->rot.m[2][1] = 0.0f;
    m->rot.m[2][2] = 1.0f;
    m->pos.x = 0.0f;
    m->pos.y = 0.0f;
    m->pos.z = 0.0f;
    m = &lbl_804C5690;
    m->rot.m[0][0] = 1.0f;
    m->rot.m[0][1] = 0.0f;
    m->rot.m[0][2] = 0.0f;
    m->rot.m[1][0] = 0.0f;
    m->rot.m[1][1] = 1.0f;
    m->rot.m[1][2] = 0.0f;
    m->rot.m[2][0] = 0.0f;
    m->rot.m[2][1] = 0.0f;
    m->rot.m[2][2] = 1.0f;
    m->pos.x = 0.0f;
    m->pos.y = 0.0f;
    m->pos.z = 0.0f;
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
    RMtx cam;
    cam.rot.m[0][0] = 1.0f;
    cam.rot.m[0][1] = 0.0f;
    cam.rot.m[0][2] = 0.0f;
    cam.rot.m[1][0] = 0.0f;
    cam.rot.m[1][1] = 1.0f;
    cam.rot.m[1][2] = 0.0f;
    cam.rot.m[2][0] = 0.0f;
    cam.rot.m[2][1] = 0.0f;
    cam.rot.m[2][2] = 1.0f;
    cam.pos.x = 0.0f;
    cam.pos.y = 0.0f;
    cam.pos.z = 0.0f;
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
    VISetPreRetraceCallback(0);
    fn_802E30C8("gx: max dynamic data used = %d", lbl_8055FD98);
    fn_802DD5F4();
    fn_802D3894();
    fn_802D6860();
    delete[] (u8*)lbl_804C9940.buf;
    lbl_804C9940.buf = 0;
    fn_802DA4C4();
    fn_802D0C98();
    VISetBlack(1);
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
    GXGetCPUFifo(&fifo);
    GXGetFifoPtrs(&fifo, &rd, &wr);
    GXEnableBreakPt(wr);
    GXFlush();
    GXInvalidateVtxCache();
    GXSetDrawSync(1);
    lbl_8055FD30++;
    if (!lbl_804C97B0.range && !((f32)__fabs(lbl_804C97B0.depth) <= FEPS)) {
        lbl_804C97B0.range = 1;
        lbl_804C97B0.dirty = 1;
    }
}

void fn_802CFD74(void) {
    GXFifoObj fifo;
    GXGetCPUFifo(&fifo);
    void* base = GXGetFifoBase(&fifo);
    u32 size = GXGetFifoSize(&fifo);
    GXAbortFrame();
    lbl_8055FD6C = GXInit(base, size);
    fn_802CF850();
    VIConfigure(lbl_8055FD64);
}

void fn_802CFDE0(void) {
    fn_802ECA48();
    s32 used = lbl_8055FD94 + lbl_8055FD9C;
    lbl_8055FD98 = used > lbl_8055FD98 ? used : lbl_8055FD98;
    fn_802D6C78();
    GXFlush();
    GXSetDrawSync(2);
    lbl_8055FDA8 = lbl_8055FD94;
    if (lbl_8055FDA4 < lbl_8055FD94) {
        DCFlushRange(lbl_8055FDA0 + lbl_8055FDA4, lbl_8055FD94 - lbl_8055FDA4);
    } else {
        DCFlushRange(lbl_8055FDA0 + lbl_8055FDA4, 0x177000 - lbl_8055FDA4);
        DCFlushRange(lbl_8055FDA0, lbl_8055FDA8);
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

void fn_802CFF10(void) {
    lbl_8055FD44 = 0;
    fn_802CF5D0();
    fn_802D0C98();
    fn_802D0BD8();
    lbl_8055FDAC = 0;
    lbl_8055FE39 = 0;
}

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
    u32 c = fn_802CE9FC(r, g, b, 0);
    GXColor col = *(GXColor*)&c;
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
    int v = 255.5f * a;
    f32 x = (f32)v < 255.0f ? (f32)v : 255.0f;
    if (x <= 0.0f) {
        x = 0.0f;
    }
    GXColor c;
    c.r = 0xFF;
    c.g = 0xFF;
    c.b = 0xFF;
    c.a = (int)x;
    lbl_804C97D0.alpha = c.a;
    lbl_804C97D0.dirty = 1;
    GXSetTevColor(1, c);
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
    u32 c = fn_802CEA1C(color);
    GXColor col = *(GXColor*)&c;
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
    lbl_804C9778.tanHalf = tan(fovy * 0.5f);
    lbl_804C97B0.nearZ = n;
    lbl_804C97B0.farZ = f;
    lbl_804C97B0.dirty = 1;
}

void fn_802D03F4(const RMtx* cam) {
    fn_802D6C78();
    lbl_804C56C0 = *cam;
    V3 up;
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    PSVECCrossProduct(&up, (const V3*)cam->rot.m[2], &lbl_804C56F0);
    f32 mag = PSVECMag(&lbl_804C56F0);
    mag = mag > 0.05f ? mag : 0.05f;
    f32 inv = 1.0f / mag;
    lbl_8055FD70 = 1;
    lbl_804C56F0.x *= inv;
    lbl_804C56F0.y *= inv;
    lbl_804C56F0.z *= inv;
}

void fn_802D050C(void* p) { lbl_8055FD54 = p; }

s32 fn_802D0514(const RMtx* view) {
    s32 n = lbl_8055FDAC;
    if (n >= 0x800) {
        return 0;
    }
    GXLightSet* s = &lbl_8055FDB0[n];
    lbl_8055FDAC = n + 1;
    s->view = *view;
    s->amb = lbl_804C97D0.amb;
    s->col = lbl_804C97D0.color;
    s->alpha = lbl_804C97D0.alpha;
    s->dir = lbl_804C97D0.dir;
    s->scale = lbl_804C97D0.scale;
    return n + 1;
}

void fn_802D0610(s32 set) {
    if (set == lbl_8055FD8C) {
        return;
    }
    if (set != 0) {
        GXLightSet* s = &lbl_8055FDB0[set - 1];
        lbl_804C5690 = s->view;
        lbl_8055FD70 = 1;
        lbl_804C97D0.amb = s->amb;
        lbl_804C97D0.color = s->col;
        lbl_804C97D0.alpha = s->alpha;
        lbl_804C97D0.dir = s->dir;
        lbl_804C97D0.scale = s->scale;
        lbl_804C97D0.dirty = 1;
    } else {
        RMtx m;
        m.rot.m[0][0] = 1.0f;
        m.rot.m[0][1] = 0.0f;
        m.rot.m[0][2] = 0.0f;
        m.rot.m[1][0] = 0.0f;
        m.rot.m[1][1] = 1.0f;
        m.rot.m[1][2] = 0.0f;
        m.rot.m[2][0] = 0.0f;
        m.rot.m[2][1] = 0.0f;
        m.rot.m[2][2] = 1.0f;
        m.pos.x = 0.0f;
        m.pos.y = 0.0f;
        m.pos.z = 0.0f;
        lbl_804C5690 = m;
        lbl_8055FD70 = 1;
    }
    lbl_8055FD8C = set;
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
    GXSetCullMode(m);
}

void fn_802D0910(void) {}

void fn_802D0914(s32 fd, int w, int h) {
    u8 hdr[0x12];
    memset(hdr, 0, sizeof(hdr));
    *(u16*)(hdr + 8) = 0;
    *(u16*)(hdr + 10) = 0;
    *(u16*)(hdr + 12) = w;
    *(u16*)(hdr + 14) = h;
    u8 t;
    hdr[0] = 0;
    hdr[1] = 0;
    hdr[2] = 2;
    hdr[0x10] = 0x18;
    hdr[0x11] = 0;
    t = hdr[8];
    hdr[8] = hdr[9];
    hdr[9] = t;
    t = hdr[10];
    hdr[10] = hdr[11];
    hdr[11] = t;
    t = hdr[12];
    hdr[12] = hdr[13];
    hdr[13] = t;
    t = hdr[14];
    hdr[14] = hdr[15];
    hdr[15] = t;
    fn_802E3A64(fd, hdr, 0x12);
}

void fn_802D09E4(void) {}

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
        int ty = tile / lbl_8055D0BC;
        int tx = tile - ty * lbl_8055D0BC;
        fn_802E30C8(" tile %d / %d", tile + 1, lbl_8055D0BC * lbl_8055D0C0);
        fn_802D0910();
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

void fn_802D0BD8(void) {
    GXRenderModeObj* rm = lbl_8055FD64;
    if (rm->field_rendering) {
        GXSetViewportJitter(0.0f, 0.0f, rm->fbWidth, rm->efbHeight, 0.0f, 1.0f, VIGetNextField());
    } else {
        GXSetViewport(0.0f, 0.0f, rm->fbWidth, rm->efbHeight, 0.0f, 1.0f);
    }
    GXInvalidateVtxCache();
    GXInvalidateTexAll();
}

void fn_802D0C98(void) {
    lbl_8055FD4C = 0;
    GXSetZMode(1, 3, 1);
    GXSetColorUpdate(1);
    fn_802D09E8();
    GXSetColorUpdate(lbl_8055D0C4);
    lbl_8055FD74 |= 1;
    GXSetZMode(1, 7, 1);
    lbl_8055FD74 = (lbl_8055FD74 | 2) & ~4;
    u16 h = GXSetDispCopyYScale(GXGetYScaleFactor(lbl_8055FD64->efbHeight, lbl_8055FD64->xfbHeight));
    GXSetDispCopySrc(0, 0, lbl_8055FD64->fbWidth, lbl_8055FD64->efbHeight);
    GXSetDispCopyDst(lbl_8055FD64->fbWidth, h);
    GXSetCopyFilter(lbl_8055FD64->aa, lbl_8055FD64->sample_pattern, 1, lbl_8055FD64->vfilter);
    GXCopyDisp(lbl_804C9760.cur, 1);
    VISetNextFrameBuffer(lbl_804C9760.cur);
    if (lbl_8055FD48 > 0) {
        lbl_8055FD48--;
        VISetBlack(1);
    } else {
        lbl_8055FD48 = 0;
        VISetBlack(0);
    }
    VIFlush();
    lbl_804C9760.cur = lbl_804C9760.buf;
    fn_802CFF50();
}

void fn_802D0DD4(f32 out[3][4], const RMtx* m) {
    out[0][0] = m->rot.m[0][0];
    out[1][0] = m->rot.m[0][1];
    out[2][0] = -m->rot.m[0][2];
    out[0][1] = m->rot.m[1][0];
    out[1][1] = m->rot.m[1][1];
    out[2][1] = -m->rot.m[1][2];
    out[0][2] = m->rot.m[2][0];
    out[1][2] = m->rot.m[2][1];
    out[2][2] = -m->rot.m[2][2];
    out[0][3] = m->pos.x;
    out[1][3] = m->pos.y;
    out[2][3] = -m->pos.z;
}

void fn_802D0E48(int pass) {
    if (pass == 0) {
        GXSetAlphaCompare(3, lbl_8055FD24, 1, 0, 0);
        GXSetZMode(1, 3, 0);
    } else if (pass == 1) {
        GXSetAlphaCompare(4, lbl_8055FD24, 1, 0, 0);
        GXSetZMode(1, 3, 1);
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
        GXSetBlendMode(0, 0, 0, 5);
    } else {
        int hi = lbl_8055FD86 >> 8;
        int d = fn_802D0EC8(lbl_8055FD86 & 0xFF);
        GXSetBlendMode(1, fn_802D0EC8(hi), d, 5);
    }
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
                GXSetZMode(1, f & 4 ? 3 : 7, (f >> 1) & 1);
            } else {
                GXSetZMode(0, 7, 0);
            }
        }
        if (d & 1) {
            GXSetColorUpdate(lbl_8055FD1C & 1);
        }
        if (d & 0x10) {
            GXSetAlphaUpdate((lbl_8055FD1C >> 4) & 1);
        }
        if (d & 0x20) {
            GXSetTevColor(2, *(GXColor*)&lbl_8055FD28);
        }
        if (d & 0x40) {
            if (lbl_8055FD1C & 0x40) {
                GXSetDstAlpha(0, 0);
                GXSetAlphaCompare(7, 0, 1, 0, 0);
            } else {
                GXSetDstAlpha(1, lbl_8055FD60);
                fn_802CF5D4();
            }
            lbl_804C5700.dirty = 1;
        }
        if (d & 0x80) {
            ApplyBlend();
            lbl_804C5700.dirty = 1;
            lbl_8055FD88 = lbl_8055FD86;
        }
        if (d & 0x100) {
            lbl_804C5700.dirty = 1;
        }
        lbl_8055FD74 = lbl_8055FD1C;
    }
    if (lbl_8055FD24 != lbl_8055FD91) {
        GXSetAlphaCompare(4, lbl_8055FD24, 1, 0, 0);
        lbl_8055FD91 = lbl_8055FD24;
    }
    if (lbl_8055FD86 != lbl_8055FD88) {
        ApplyBlend();
        lbl_804C5700.dirty = 1;
        lbl_8055FD88 = lbl_8055FD86;
    }
    if (lbl_8055FE58) {
        u32 c = fn_802CEA1C(lbl_804DB458[1]);
        GXSetTevColor(3, *(GXColor*)&c);
        lbl_8055FE58 = 0;
    }
    if (lbl_804C97B0.dirty) {
        lbl_804C97B0.dirty = 0;
        GXSetFog(lbl_804C97B0.range ? 2 : 0, lbl_804C97B0.start, lbl_804C97B0.end, lbl_804C97B0.nearZ,
                 lbl_804C97B0.farZ, lbl_804C97B0.color);
    }
    if (lbl_804C97D0.dirty) {
        lbl_804C97D0.dirty = 0;
        u32 w = fn_802CE9FC(0xFF, 0xFF, 0xFF, 0xFF);
        u32 k = fn_802CE9FC(0, 0, 0, 0xFF);
        f32 s = -1048576.0f;
        GXInitLightPos(&lbl_804C97D0.obj, lbl_804C97D0.dir.x * s, lbl_804C97D0.dir.y * s, -(lbl_804C97D0.dir.z * s));
        GXColor c;
        c.r = (int)(lbl_804C97D0.scale * lbl_804C97D0.amb.r);
        c.g = (int)(lbl_804C97D0.scale * lbl_804C97D0.amb.g);
        c.b = (int)(lbl_804C97D0.scale * lbl_804C97D0.amb.b);
        c.a = lbl_804C97D0.amb.a;
        GXInitLightColor(&lbl_804C97D0.obj, c);
        GXInitLightDistAttn(&lbl_804C97D0.obj, 1.0f, 1.0f, 0);
        GXLoadLightObjImm(&lbl_804C97D0.obj, 1);
        GXSetChanMatColor(0, *(GXColor*)&w);
        u32 a = fn_802CE9FC(0, 0, 0, lbl_804C97D0.alpha);
        GXSetChanMatColor(2, *(GXColor*)&a);
        GXSetChanAmbColor(0, lbl_804C97D0.color);
        GXSetChanAmbColor(2, *(GXColor*)&k);
    }
    if (lbl_8055FD25 != lbl_8055FD85 || lbl_8055FD84) {
        lbl_8055FD85 = lbl_8055FD25;
        lbl_8055FD84 = 0;
        if (lbl_804C5700.mode == 0) {
            lbl_804C5700.dirty = 1;
        }
        GXSetNumChans(1);
        if (lbl_8055FD25) {
            GXSetChanCtrl(0, 1, 0, 0, 1, 2, 2);
            GXSetChanCtrl(2, 0, 0, 0, 0, 0, 2);
        } else {
            GXSetChanCtrl(0, 0, 1, 1, 0, 0, 2);
            GXSetChanCtrl(2, 0, 1, 1, 0, 0, 2);
        }
    }
}

void fn_802D14D8(void) {
    if (!lbl_804C9778.dirty) {
        return;
    }
    lbl_804C9778.dirty = 0;
    f32 k = lbl_8055FE24 ? 1.0f : 0.93333334f;
    f32 aspect = lbl_804C9778.aspect * (k / lbl_804C9778.zoom);
    f32 fovy = 2.0f * (f32)atan((f32)tan(0.5f * lbl_804C9778.fovy) / aspect);
    f32 m[4][4];
    C_MTXPerspective(m, 2.0f * fovy / 0.017453292f, aspect, lbl_804C9778.nearZ, lbl_804C9778.farZ);
    if (lbl_8055FE24 == 0 && lbl_8055D0B8 >= 0 && lbl_8055D0B8 < lbl_8055D0BC * lbl_8055D0C0) {
        int ty = lbl_8055D0B8 / lbl_8055D0BC;
        int tx = lbl_8055D0B8 - ty * lbl_8055D0BC;
        f32 sx = 1.0f / lbl_8055D0BC;
        f32 sy = 1.0f / lbl_8055D0C0;
        m[0][0] = m[0][0] / sx;
        m[1][1] = m[1][1] / sy;
        m[0][2] = m[0][2] + (f32)(tx * 2 - lbl_8055D0BC + 1);
        m[1][2] = m[1][2] - (f32)(ty * 2 - lbl_8055D0C0 + 1);
    }
    GXSetProjection(m, 0);
}

void fn_802D1678(void) {
    if (!lbl_804C9778.vpDirty) {
        return;
    }
    lbl_804C9778.vpDirty = 0;
    if (lbl_8055FD64->field_rendering) {
        GXSetViewportJitter(lbl_804C9778.vx, lbl_804C9778.vy, lbl_804C9778.vw, lbl_804C9778.vh, 0.0f, 1.0f,
                            VIGetNextField());
    } else {
        GXSetViewport(lbl_804C9778.vx, lbl_804C9778.vy, lbl_804C9778.vw, lbl_804C9778.vh, 0.0f, 1.0f);
    }
    GXSetScissor(lbl_804C9778.vx, lbl_804C9778.vy, lbl_804C9778.vw, lbl_804C9778.vh);
    fn_802D6C68();
}

void fn_802D17BC(GXLightObj* l, const V3* pos, const V3* cam, const u8* color, f32 radius) {
    GXColor c;
    c.r = color[0];
    c.g = color[1];
    c.b = color[2];
    c.a = color[3];
    GXInitLightColor(l, c);
    V3 d;
    d.x = pos->x + cam->x;
    d.y = pos->y + cam->y;
    d.z = pos->z + cam->z;
    f32 mag = PSVECMag(&d);
    mag = mag > 0.001f ? mag : 0.001f;
    f32 inv = -1.0f / mag;
    d.x *= inv;
    d.y *= inv;
    d.z *= inv;
    GXInitSpecularDirHA(l, cam->x, cam->y, -cam->z, d.x, d.y, -d.z);
    GXInitLightAttn(l, 0.0f, 0.0f, 1.0f - 0.5f * radius, 0.0f, 0.0f, 0.0f);
}

void fn_802D18E8(void) {
    V3 d;
    d.x = lbl_804C5690.pos.x - lbl_804C56C0.pos.x;
    d.y = lbl_804C5690.pos.y - lbl_804C56C0.pos.y;
    d.z = lbl_804C5690.pos.z - lbl_804C56C0.pos.z;
    f32 mag = PSVECMag(&d);
    mag = mag > 0.001f ? mag : 0.001f;
    f32 inv = 1.0f / mag;
    lbl_8055FDCC = 0;
    d.y *= inv;
    d.z *= inv;
    d.x *= inv;
    if (lbl_8055FDC8 > 0) {
        PointLight* p = lbl_804CA088;
        GXLightObj* o = lbl_804CA118;
        for (int i = 0; i < lbl_8055FDC8; i++) {
            u8 c[4];
            c[0] = p->color[0];
            c[1] = p->color[1];
            c[2] = p->color[2];
            c[3] = p->color[3];
            fn_802D17BC(o, &d, &p->pos, c, p->radius);
            p++;
            o++;
        }
        lbl_8055FDCC = lbl_8055FDC8;
    } else {
        u8 c[4];
        c[0] = (int)(lbl_804C97D0.scale * lbl_804C97D0.amb.r);
        c[1] = (int)(lbl_804C97D0.scale * lbl_804C97D0.amb.g);
        c[2] = (int)(lbl_804C97D0.scale * lbl_804C97D0.amb.b);
        c[3] = lbl_804C97D0.amb.a;
        fn_802D17BC(lbl_804CA118, &d, &lbl_804C97D0.dir, c, 64.0f);
        lbl_8055FDCC++;
    }
    u32 mask = 0;
    GXLightObj* o = lbl_804CA118;
    for (int i = 0; i < lbl_8055FDCC; i++) {
        u32 bit = 2 << i;
        GXLoadLightObjImm(o, bit);
        mask |= bit;
        o++;
    }
    GXSetNumChans(2);
    GXSetChanCtrl(1, 1, 0, 0, mask, 0, 0);
    GXSetChanCtrl(3, 0, 0, 0, 0, 0, 2);
}

void fn_802D1B58(void) {
    f32 m[2][4] = {{0.0f, 0.0f, 0.0f, 0.5f}, {0.0f, 0.0f, 0.0f, 0.5f}};
    m[0][0] = 0.5f * -lbl_804C5690.rot.m[0][1];
    m[0][1] = 0.5f * -lbl_804C5690.rot.m[1][1];
    m[0][2] = 0.5f * -lbl_804C5690.rot.m[2][1];
    GXLoadTexMtxImm(m, 0x21, 1);
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

static inline void Invert(RMtx* out, const RMtx* m) {
    out->rot = m->rot;
    f32 t = out->rot.m[0][1];
    out->rot.m[0][1] = out->rot.m[1][0];
    out->rot.m[1][0] = t;
    t = out->rot.m[0][2];
    out->rot.m[0][2] = out->rot.m[2][0];
    out->rot.m[2][0] = t;
    t = out->rot.m[1][2];
    out->rot.m[1][2] = out->rot.m[2][1];
    out->rot.m[2][1] = t;
    MulVec(&out->pos, &m->pos, &out->rot);
    out->pos.x *= -1.0f;
    out->pos.y *= -1.0f;
    out->pos.z *= -1.0f;
}

void fn_802D1C04(void) {
    if (!lbl_8055FD70) {
        return;
    }
    lbl_8055FD70 = 0;
    RMtx inv;
    Invert(&inv, &lbl_804C56C0);
    V3 p;
    fn_801BFD34(&p, &lbl_804C5690.pos, &inv);
    Mat3 r;
    Mat3Mul__FP4Mat3PC4Mat3PC4Mat3(&r, &lbl_804C5690.rot, &inv);
    fn_802D0DD4(lbl_804C5650, (RMtx*)&r);
    GXLoadPosMtxImm(lbl_804C5650, 0);
    if (lbl_8055FD19) {
        lbl_8055FD19 = 0;
        GXSetCurrentMtx(0);
    }
    switch (lbl_804C5700.mode) {
    case 2:
    case 3: {
        // Projected texture: express the model frame in the projector's look-at frame.
        f32 m[3][4];
        f32 b[3][4];
        f32 s[3][4];
        f32 t[3][4];
        f32 st[3][4];
        RMtx look;
        fn_802C8588(&look, &lbl_804C56C0.pos, &lbl_804C5690.pos);
        RMtx li;
        Invert(&li, &look);
        V3 lp;
        fn_801BFD34(&lp, &lbl_804C5690.pos, &li);
        Mat3 lr;
        Mat3Mul__FP4Mat3PC4Mat3PC4Mat3(&lr, &lbl_804C5690.rot, &li);
        fn_802D0DD4(m, (RMtx*)&lr);
        PSMTXInvXpose(m, b);
        PSMTXScale(s, 0.5f, -0.5f, 0.0f);
        PSMTXTrans(t, 0.5f, 0.5f, 1.0f);
        PSMTXConcat(t, s, st);
        GXLoadTexMtxImm(b, 0x2A, 0);
        GXLoadTexMtxImm(st, 0x4C, 0);
        break;
    }
    case 1:
    case 5: {
        f32 m[2][4] = {{0.0f, 0.0f, 0.0f, 0.5f}, {0.0f, 0.0f, 0.0f, 0.5f}};
        m[0][0] = 0.5f * lbl_804C5690.rot.m[0][0];
        m[0][1] = 0.5f * lbl_804C5690.rot.m[1][0];
        m[0][2] = 0.5f * lbl_804C5690.rot.m[2][0];
        m[1][0] = 0.5f * lbl_804C5690.rot.m[0][1];
        m[1][1] = 0.5f * lbl_804C5690.rot.m[1][1];
        m[1][2] = 0.5f * lbl_804C5690.rot.m[2][1];
        GXLoadTexMtxImm(m, 0x2A, 1);
        break;
    }
    case 4:
        fn_802D18E8();
        break;
    }
    f32 n[3][4];
    fn_802D0DD4(n, &lbl_804C5690);
    GXLoadNrmMtxImm(n, 0);
    if (lbl_804C5700.mode == 2) {
        fn_802D1B58();
    }
}

void fn_802D2218(void) {
    if (!lbl_804C9778.overlayC && !lbl_804C9778.overlayZ) {
        return;
    }
    fn_802D1678();
    RMtx id;
    id.rot.m[0][0] = 1.0f;
    id.rot.m[0][1] = 0.0f;
    id.rot.m[0][2] = 0.0f;
    id.rot.m[1][0] = 0.0f;
    id.rot.m[1][1] = 1.0f;
    id.rot.m[1][2] = 0.0f;
    id.rot.m[2][0] = 0.0f;
    id.rot.m[2][1] = 0.0f;
    id.rot.m[2][2] = 1.0f;
    id.pos.x = 0.0f;
    id.pos.y = 0.0f;
    id.pos.z = 0.0f;
    f32 pm[3][4];
    fn_802D0DD4(pm, &id);
    GXLoadPosMtxImm(pm, 0);
    f32 w = lbl_804C9778.vw;
    f32 h = lbl_804C9778.vh;
    f32 o[4][4];
    C_MTXOrtho(o, 0.0f, h, 0.0f, w, 0.0f, 1.0f);
    GXSetProjection(o, 1);
    GXSetDstAlpha(0, 0);
    GXSetBlendMode(0, 1, 0, 5);
    GXSetColorUpdate(lbl_804C9778.overlayC);
    GXSetAlphaUpdate(lbl_804C9778.overlayC);
    GXSetZMode(1, 7, lbl_804C9778.overlayZ);
    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, lbl_804C97B0.color);
    GXSetNumTevStages(1);
    GXSetNumTexGens(0);
    GXSetTevColor(1, lbl_804C9778.overlay);
    GXSetTevOrder(0, 0xFF, 0xFF, 0xFF);
    GXSetTevColorIn(0, 0xF, 0xF, 0xF, 2);
    GXSetTevColorOp(0, 0, 0, 0, 0, 0);
    GXSetTevAlphaIn(0, 7, 7, 7, 1);
    GXSetTevAlphaOp(0, 0, 0, 0, 0, 0);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXBegin(0x98, 0, 4);
    for (int i = 0; i < 4; i++) {
        WGPIPE.f = (i & 1 ? -1 : 1) * w;
        WGPIPE.f = (i & 2 ? -1 : 1) * h;
        WGPIPE.f = -0.5f;
    }
    GXSetNumTexGens(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevOp(0, 0);
    GXSetDstAlpha(1, lbl_8055FD60);
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
    GXSetNumIndStages(0);
    for (int i = 0; i < 4; i++) {
        GXSetTevDirect(i);
    }
}

void fn_802D2684(void) {
    u8 env = lbl_804C5700.env;
    int alphaOnly = (lbl_8055FD1C >> 6) & 1;
    int stage = 0;
    u8 base2 = lbl_804C5700.base2;
    int coord = 0;
    u8 envA = lbl_804C5700.envA;
    GXSetTexCoordGen2(0, 1, 4, lbl_804C5700.lmap ? 0x1E : 0x3C, 0, 0x7D);
    GXSetTevOrder(0, 0, 0, 4);
    if (alphaOnly) {
        GXSetTevOp(0, 3);
    } else if (env) {
        GXSetTevOp(0, 3);
        coord = 1;
        GXSetTexCoordGen2(1, 1, 5, 0x3C, 0, 0x7D);
        stage = 1;
        GXSetTevOrder(1, 1, 3, 4);
        GXSetTevOp(1, 0);
    } else {
        int in[4];
        int n = 0;
        GXSetTevColorOp(0, 0, 0, 0, 1, 0);
        GXSetTevColorIn(0, 0xF, 8, 0xA, 0xF);
        if (lbl_8055FD1C & 0x100) {
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
        GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn(0, 7, in[0], in[1], 7);
    }
    if (base2) {
        stage++;
        GXSetTevOrder(stage, 0, 1, 4);
        GXSetTevColorOp(stage, 0, 0, 0, 1, 0);
        GXSetTevColorIn(stage, 0, 0xF, 0xF, 0xF);
        GXSetTevAlphaOp(stage, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn(stage, 7, 0, 4, 7);
    }
    if (envA) {
        coord++;
        GXSetTexCoordGen2(coord, 1, 0, 0x2D, 0, 0x7D);
        stage++;
        GXSetTevOrder(stage, coord, 5, 0xFF);
        GXSetTevColorIn(stage, 0, 8, 9, 0xF);
        GXSetTevColorOp(stage, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn(stage, 0, 7, 7, 7);
        GXSetTevAlphaOp(stage, 0, 0, 0, 1, 0);
    }
    GXSetNumTevStages(stage + 1);
    GXSetNumTexGens(coord + 1);
    if (lbl_804C5700.ind) {
        GXSetBlendMode(1, 4, 5, 5);
        GXSetNumIndStages(1);
        GXSetTevIndirect(0, 0, 0, 3, 1, 0, 0, 0, 0, 0);
        GXSetIndTexOrder(0, 1, 2);
        GXSetNumTexGens(2);
        GXSetTexCoordGen2(1, 1, 0, 0x21, 0, 0x7D);
        lbl_8055FD80 = fn_802D2640;
    }
}

void fn_802D2AC4(void) {
    GXSetNumChans(1);
    GXSetChanCtrl(1, 0, 0, 0, 0, 0, 2);
    fn_802DFAF4();
}

void fn_802D2B0C(void) {
    if (!lbl_8055FDB4 && memcmp(&lbl_804C9930, &lbl_804C5700, sizeof(TexState)) != 0) {
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
    Invert(&lbl_804CA2D8, &lbl_804C56C0);
    GXSetCurrentMtx(0);
}

void fn_802D2ED8(const RMtx* m, int slot) {
    if (slot == 0) {
        lbl_8055FD70 = 1;
        lbl_8055FD8C = -1;
    }
    RMtx t;
    fn_801BFD34(&t.pos, &m->pos, &lbl_804CA2D8);
    Mat3Mul__FP4Mat3PC4Mat3PC4Mat3(&t.rot, &m->rot, &lbl_804CA2D8);
    fn_802D0DD4(lbl_804C5650, &t);
    GXLoadPosMtxImm(lbl_804C5650, slot * 3);
}

void fn_802D2F70(const RMtx* m) { fn_802D2ED8(m, 0); }

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
    DCFlushRange(lbl_804C9760.cur, lbl_804C9760.bpp * (lbl_804C9760.w * lbl_804C9760.h));
    VISetNextFrameBuffer(lbl_804C9760.cur);
    VIFlush();
    VIWaitForRetrace();
    VISetBlack(0);
    lbl_804C9760.cur = lbl_804C9760.buf;
}

void fn_802D3050(int on) { lbl_8055FD1C = on ? lbl_8055FD1C | 0x10 : lbl_8055FD1C & ~0x10; }

void fn_802D306C(u8 a) {
    lbl_8055FD60 = a;
    GXSetDstAlpha(1, a);
}

void fn_802D307C(void) {
    f32 m[2][3] = {{0.5f, 0.0f, 0.0f}, {0.0f, 0.5f, 0.0f}};
    GXSetIndTexMtx(1, m, 1);
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

int fn_802D31D8(void) {
    lbl_8055FDB4 = 1;
    GXSetColorUpdate(0);
    GXSetAlphaUpdate(1);
    GXSetDstAlpha(0, 0);
    fn_802CF5D4();
    lbl_8055FD1C &= ~0x10;
    GXSetNumTevStages(1);
    GXSetNumTexGens(0);
    GXSetZMode(1, 6, 0);
    GXSetTevOrder(0, 0xFF, 0xFF, 4);
    GXSetTevAlphaIn(0, 7, 7, 7, 1);
    GXSetTevAlphaOp(0, 0, 0, 0, 0, 0);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 0xF, 0xF, 0xF);
    u32 c = fn_802CE9FC(0, 0, 0, 4);
    GXSetTevColor(1, *(GXColor*)&c);
    return 1;
}

void fn_802D3304(void) {
    lbl_804C5700.dirty = 1;
    fn_802CFF50();
    lbl_8055FDB4 = 0;
    GXSetCullMode(0);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevOp(0, 0);
}

void fn_802D3360(void) {
    fn_802CF5D4();
    lbl_8055FD84 = 1;
    lbl_804C5700.dirty = 1;
    fn_802CFF50();
    GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
    for (int i = 0; i < 4; i++) {
        GXSetTexCoordGen2(i, 1, 4, 0x3C, 0, 0x7D);
    }
    fn_802D2640();
    if (lbl_8055FE1C) {
        fn_802D92B0();
    }
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevOp(0, 0);
    GXSetCurrentMtx(0);
}

void fn_802D3420(void) {
    RMtx id;
    id.rot.m[0][0] = 1.0f;
    id.rot.m[0][1] = 0.0f;
    id.rot.m[0][2] = 0.0f;
    id.rot.m[1][0] = 0.0f;
    id.rot.m[1][1] = 1.0f;
    id.rot.m[1][2] = 0.0f;
    id.rot.m[2][0] = 0.0f;
    id.rot.m[2][1] = 0.0f;
    id.rot.m[2][2] = 1.0f;
    id.pos.x = 0.0f;
    id.pos.y = 0.0f;
    id.pos.z = 0.0f;
    f32 pm[3][4];
    fn_802D0DD4(pm, &id);
    GXLoadPosMtxImm(pm, 0);
    f32 o[4][4];
    C_MTXOrtho(o, 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f);
    GXSetProjection(o, 1);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    f32 u = (f32)lbl_804C9778.vw / 640.0f;
    f32 v = (f32)lbl_804C9778.vh / 448.0f;
    GXBegin(0x80, 0, 4);
    WGPIPE.f = -1.0f;
    WGPIPE.f = 1.0f;
    WGPIPE.f = -0.5f;
    WGPIPE.f = 0.0f;
    WGPIPE.f = 0.0f;
    WGPIPE.f = 1.0f;
    WGPIPE.f = 1.0f;
    WGPIPE.f = -0.5f;
    WGPIPE.f = u;
    WGPIPE.f = 0.0f;
    WGPIPE.f = 1.0f;
    WGPIPE.f = -1.0f;
    WGPIPE.f = -0.5f;
    WGPIPE.f = u;
    WGPIPE.f = v;
    WGPIPE.f = -1.0f;
    WGPIPE.f = -1.0f;
    WGPIPE.f = -0.5f;
    WGPIPE.f = 0.0f;
    WGPIPE.f = v;
    fn_802D6C68();
    lbl_804C9778.dirty = 1;
    lbl_8055FD70 = 1;
}

void fn_802D362C(u32 color) {
    GXSetTexCopySrc(lbl_804C9778.vx, lbl_804C9778.vy, lbl_804C9778.vw, lbl_804C9778.vh);
    GXSetTexCopyDst(lbl_804C9778.vw, lbl_804C9778.vh, 0x27, 0);
    GXCopyTex(lbl_804C9940.buf, 0);
    GXPixModeSync();
    GXSetColorUpdate(1);
    GXSetAlphaUpdate(0);
    GXSetDstAlpha(0, 0);
    GXSetNumTevStages(1);
    GXSetNumTexGens(1);
    GXLoadTexObj(&lbl_804C9940.obj, 0);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 0xF, 0xF, 2);
    GXSetTevAlphaOp(0, 0xE, 0, 0, 1, 0);
    GXSetTevAlphaIn(0, 4, 7, 1, 7);
    GXSetBlendMode(1, 4, 5, 3);
    GXSetZMode(0, 3, 0);
    GXSetTexCoordGen2(0, 1, 0xC, 0x3C, 0, 0x7D);
    u32 c = fn_802CEA1C(color);
    GXSetTevColor(1, *(GXColor*)&c);
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
    VisTest** q = lbl_804C9984.queue;
    for (int i = 0; i < lbl_804C9984.count; i++) {
        VisTest* t = *q++;
        u32 z;
        GXPeekZ(t->x, t->y, &z);
        t->hit = z >= t->z;
    }
    lbl_804C9984.count = 0;
}

VisTest* fn_802D39A4(f32 rate) {
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
        t->z = __cvt_fp2unsigned(16777215.0f + 16777215.0f * ((z * (-n / (f - n)) - f * n / (f - n)) / -z));
        if (lbl_804C9984.count < 0x40) {
            lbl_804C9984.queue[lbl_804C9984.count] = t;
            lbl_804C9984.count++;
        }
    } else {
        t->hit = 0;
    }
    t->vis = t->vis + t->rate * (target - t->vis);
    return t->vis >= 0.007843138f;
}

void fn_802D3BB0(void) {}

struct QuadVtx {
    V3 pos;
    u32 color;
    f32 u, v;
};

void fn_802D3BB4(const V3* c, f32 r, const V3* a, const V3* b, u32 color, f32 alpha) {
    static const u16 idx[6] = {0, 1, 2, 1, 2, 3};
    u16 i[6];
    for (int k = 0; k < 6; k++) {
        i[k] = idx[k];
    }
    QuadVtx v[4];
    u32 col = (color & 0xFFFFFF) | ((int)(alpha * (color >> 24)) << 24);
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
    v[0].color = col;
    v[1].color = col;
    v[2].color = col;
    v[3].color = col;
    v[0].u = 0.0f;
    v[0].v = 0.0f;
    v[1].u = 1.0f;
    v[1].v = 0.0f;
    v[2].u = 0.0f;
    v[2].v = 1.0f;
    v[3].u = 1.0f;
    v[3].v = 1.0f;
    fn_802D68C4(v, 4, i, 6);
}

void fn_802D3D50(void) {
    lbl_804C97B0.range = 0;
    lbl_804C97B0.dirty = 1;
}

void fn_802D3D6C(void) { lbl_804C5700.lmap = 1; }

void fn_802D3D80(void) { lbl_804C5700.lmap = 0; }

void fn_802D3D94(int i, const f32 m[2][4]) { GXLoadTexMtxImm(m, i * 3 + 0x1E, 1); }

void fn_802D3DAC(void) {
    if ((lbl_8055FDD0 & 0x3F) == 0) {
        OSReport(" gx: overflowed GX FIFO! (%d)\n", lbl_8055FDD0);
    }
    lbl_8055FDD0++;
    GXResetOverflowCount();
    GXDisableBreakPt();
}

void fn_802D3DF8(GXTexObj* tex, f32 u, f32 v, s8 scale) {
    lbl_804C5700.ind = 1;
    GXLoadTexObj((GXTexObj*)((u8*)tex + 0x24), 2);
    f32 m[2][3];
    m[0][0] = 0.0f;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;
    m[1][0] = 0.0f;
    m[1][1] = 0.0f;
    m[1][2] = 0.0f;
    m[0][0] = u;
    m[1][1] = v;
    GXSetIndTexMtx(1, m, scale);
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
