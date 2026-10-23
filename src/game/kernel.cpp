// Kernel (0x802E1728-0x802E3028): the program entry, system init/shutdown order, stack overrun guard,
// millisecond/microsecond clocks, the reset/power-off path with its veto callbacks, and the overlay that
// draws a planar YUV(A) video frame through indirect texturing (two colour-ramp lookup textures turn the
// chroma planes into RGB). Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
void* fn_8011FFC4(u32 size);  // allocate
void fn_80120084(void* p);
void DCFlushRange(void* p, u32 n);
void DCStoreRange(void* p, u32 n);
void fn_802E30C8(const char* fmt, ...);
void fn_802E317C(const char* fmt, ...);  // fatal
void OSPanic(const char* file, int line, const char* msg, ...);
s64 OSGetTime(void);
void OSTicksToCalendarTime(s64 ticks, void* td);
void OSInit(void);
void VIInit(void);
void LCEnable(void);
void OSRestart(u32 code);
void OSReturnToMenu(void);
void OSShutdownSystem(void);
int DVDCancelAll(void);
void __WPADShutdown(void);
s32 WPADGetStatus(void);
void fn_80127C90(void (*cb)(void), const char* name);
void fn_80127D58(void (*cb)(void));
int fn_802D9BF0(void);
int fn_802D9C04(void);

void GXInitTexObj(void* obj, void* image, u16 w, u16 h, int fmt, int ws, int wt, int mip);
void GXInitTexObjLOD(void* obj, int minf, int magf, f32 minlod, f32 maxlod, f32 bias, int clamp, int edge,
                     int aniso);
void GXLoadTexObj(void* obj, int id);
void GXSetVtxAttrFmt(int fmt, int attr, int cnt, int type, int frac);
void GXClearVtxDesc(void);
void GXSetVtxDesc(int attr, int type);
void GXSetNumChans(int n);
void GXSetNumTexGens(int n);
void GXSetNumTevStages(int n);
void GXSetNumIndStages(int n);
void GXSetIndTexOrder(int stage, int coord, int map);
void GXSetBlendMode(int type, int src, int dst, int op);
void GXSetTevIndirect(int stage, int ind, int fmt, int bias, int mtx, int ws, int wt, int addPrev, int utcLod,
                      int alpha);
void GXSetTevOrder(int stage, int coord, int map, int color);
void GXSetTevColorIn(int stage, int a, int b, int c, int d);
void GXSetTevColorOp(int stage, int op, int bias, int scale, int clamp, int reg);
void GXSetTevAlphaIn(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp(int stage, int op, int bias, int scale, int clamp, int reg);
void GXSetTevKColorSel(int stage, int sel);
void GXSetTevKAlphaSel(int stage, int sel);
void GXSetTevSwapMode(int stage, int ras, int tex);
void GXSetTevSwapModeTable(int table, int r, int g, int b, int a);
void GXSetIndTexMtx(int id, const f32 m[2][3], s8 exp);
void GXSetTexCoordGen2(int dst, int type, int src, int mtx, int norm, int pt);
void GXLoadTexMtxImm(const void* m, u32 id, int type);
void GXBegin(int prim, int fmt, u16 n);
}

struct GXColor {
    u8 r, g, b, a;
};

struct GXColorS10 {
    s16 r, g, b, a;
};

extern "C" {
void GXSetTevColorS10(int reg, GXColorS10 c);
void GXSetTevKColor(int reg, GXColor c);
}

int ParseCommandLine(const char* cmd);
void AppMain(void);

union WGPipe {
    u8 uc;
    s16 s;
    f32 f;
};

#define WGPIPE (*(volatile WGPipe*)0xCC008000)

static inline void GXPosition3s16(s16 x, s16 y, s16 z) {
    WGPIPE.s = x;
    WGPIPE.s = y;
    WGPIPE.s = z;
}

static inline void GXColor4u8(u8 r, u8 g, u8 b, u8 a) {
    WGPIPE.uc = r;
    WGPIPE.uc = g;
    WGPIPE.uc = b;
    WGPIPE.uc = a;
}

static inline void GXTexCoord2f32(f32 s, f32 t) {
    WGPIPE.f = s;
    WGPIPE.f = t;
}

struct GXTexObj {
    u32 dummy[8];
};

// Textures of one frame buffer: luma, the two chroma planes and alpha.
struct VidTexSet {
    GXTexObj y;  // offset 0x0
    GXTexObj u;  // offset 0x20
    GXTexObj v;  // offset 0x40
    GXTexObj a;  // offset 0x60
};

struct VidPlane {
    s32 on;     // offset 0x0
    void* buf;  // offset 0x4
    u32 width;  // offset 0x8
};

struct VidFrame {
    VidPlane y;  // offset 0x0
    VidPlane u;  // offset 0xC
    VidPlane v;  // offset 0x18
    VidPlane a;  // offset 0x24
};

struct VidInfo {
    s32 count;       // offset 0x0
    u32 width;       // offset 0x4
    u32 height;      // offset 0x8
    u32 cwidth;      // offset 0xC
    u32 cheight;     // offset 0x10
    s32 cur;         // offset 0x14
    VidFrame frames[2];  // offset 0x18
};

struct Video {
    VidTexSet tex[2];   // offset 0x0
    VidInfo info;       // offset 0x100
    void* mem;          // offset 0x178
    u32 frameSize;      // offset 0x17C
    u32 rampW[2];       // offset 0x180
    u32 rampW2[2];      // offset 0x188
    GXTexObj ramp1;     // offset 0x190
    GXTexObj ramp2;     // offset 0x1B0
    s32 busy[2];        // offset 0x1D0
};

static inline int AlphaByte(f32 alpha) {
    if (alpha >= 0.99f) {
        return 0xFF;
    } else if (alpha <= 0.01f) {
        return 0;
    }
    return (int)(0.5f + 255.0f * alpha);
}

static inline bool VidHasAlpha(Video* v) {
    if (v->info.frames[0].a.buf) {
        return true;
    }
    return false;
}

struct KDate {
    u16 year;   // offset 0x0
    u16 month;  // offset 0x2
    u16 day;    // offset 0x4
    u16 hour;   // offset 0x6
    u16 min;    // offset 0x8
    u16 sec;    // offset 0xA
    s32 msec;   // offset 0xC
    s32 wday;   // offset 0x10
};

struct KCalendar {
    s32 sec, min, hour, mday, mon, year, wday, yday, msec, usec;
};

struct KStack {
    s32 size;     // offset 0x0
    u32* top;     // offset 0x4
    u32* bottom;  // offset 0x8
};

struct KError {
    u16 code;
    const char* name;
    void (*handler)(void);
};

static KError lbl_8034CCE0[] = {
    {2, "DSI", 0},
};

static const f32 lbl_802FAE40[2][4] = {{0.5f, 0.0f, 0.0f, 0.0f}, {0.0625f, 0.0f, 0.0f, 0.0f}};
static const f32 lbl_802FAE60[2][4] = {{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}};

ModuleTag lbl_8055FEF0;
s64 lbl_8055FEF8;  // clock start
s32 lbl_8055FF00;  // reset callbacks
static f32 lbl_804DB740[2][4];
static f32 lbl_804DB760[2][4];
static KStack lbl_804DB780;
static int (*lbl_804DB790[16])(int, int);
static const char* lbl_804DB7D0[16];

extern "C" {

void fn_802E1728(Video* v) {
    if (v->mem) {
        fn_80120084(v->mem);
        v->mem = 0;
    }
}

u32 fn_802E176C(u32 x) { return 1 << (32 - __cntlzw(x)); }

// A colour ramp: column k holds (k - 128, row - 128) in its two channels.
void fn_802E1780(u8* dst, u32 w, int pitch) {
    int a = 0;
    int b = 0;
    for (int row = 0; row < 4; row++) {
        for (u32 k = 0; k < w >> 3; k++) {
            u8* p = &dst[(k + row * pitch) * 2];
            p[0] = a - k + 0x80;
            p[1] = b - row + 0x80;
            if (++b == 4) {
                b = 0;
                a++;
            }
        }
    }
}

// Swizzles linear rows into 4-row tiles of 8 bytes.
void fn_802E1804(u8* dst, const u8* src, int w, u32 h, int bpp) {
    u32 pitch = w * bpp;
    int out = 0;
    for (u32 y = 0; y < (h >> 2); y++) {
        u32 off = y * 4 * pitch;
        for (u32 k = 0; k < (pitch >> 3); k++, off += 8) {
            const u8* s = src + off;
            u32* d = (u32*)(dst + out);
            d[0] = *(u32*)(src + off);
            d[1] = *(u32*)(s + 4);
            d[2] = *(u32*)(s + pitch);
            d[3] = *(u32*)(s + pitch + 4);
            d[4] = *(u32*)(s + pitch * 2);
            d[5] = *(u32*)(s + pitch * 2 + 4);
            d[6] = *(u32*)(s + pitch * 3);
            d[7] = *(u32*)(s + pitch * 3 + 4);
            out += 0x20;
        }
    }
}

int fn_802E18D4(u32 w, u32 h, int fmt, int wrap, GXTexObj* obj, void* image) {
    GXInitTexObj(obj, image, w, h, fmt, wrap, wrap, 0);
    GXInitTexObjLOD(obj, 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    return 1;
}

u32 fn_802E1954(u32 n) { return (n + 0x7F) & ~0x7F; }

u32 fn_802E1960(u32 w) {
    u32 n = ((w >> 3) + 3) & ~3;
    if (n & (n - 1)) {
        return fn_802E176C(n);
    }
    return n;
}

u32 fn_802E1980(u8* buf, GXTexObj* obj, u32* outW, u32* outH, u32 w) {
    u8 tmp[0x800];
    u32 n = fn_802E1960(w);
    *outW = n;
    *outH = 4;
    u32 size = fn_802E1954(n << 3);
    fn_802E1780(tmp, w, n);
    fn_802E1804(buf, tmp, n, 4, 2);
    DCFlushRange(buf, size);
    fn_802E18D4(n, 4, 3, 1, obj, buf);
    return size;
}

void fn_802E1A38(Video* v, u8* buf) {
    u32 size = fn_802E1980(buf, &v->ramp1, &v->rampW[0], &v->rampW[1], v->info.width);
    fn_802E1980(buf + size, &v->ramp2, &v->rampW2[0], &v->rampW2[1], v->info.cwidth);
}

void fn_802E1A9C(VidInfo* i, u32* offU, u32* offV, u32* offA, u32* frame, u32* ramps, u32* lumaW, u32* chromaW) {
    u32 wy;
    if (i->frames[0].y.on) {
        wy = i->width;
    } else {
        wy = 0;
    }
    u32 wa;
    if (i->frames[0].a.on) {
        wa = i->width;
    } else {
        wa = 0;
    }
    u32 wc;
    if (i->frames[0].u.on) {
        wc = i->cwidth;
    } else {
        wc = 0;
    }
    u32 h = i->height;
    u32 ch = i->cheight;
    u32 sy = fn_802E1954(wy * h);
    u32 sa = fn_802E1954(wa * h);
    u32 sc = fn_802E1954(wc * ch);
    u32 r2 = fn_802E1954(fn_802E1960(i->cwidth) << 3);
    u32 r1 = fn_802E1954(fn_802E1960(i->width) << 3);
    *ramps = r1 + r2;
    u32 ya = sy + sa;
    *offA = sy;
    *offU = ya;
    *offV = sc + ya;
    *frame = ya + sc * 2;
    *lumaW = wy | wa;
    *chromaW = wc;
}

int fn_802E1BB4(Video* v) {
    u32 offU, offV, offA, frame, ramps, lumaW, chromaW;
    fn_802E1A9C(&v->info, &offU, &offV, &offA, &frame, &ramps, &lumaW, &chromaW);
    void* mem = fn_8011FFC4(ramps + frame * v->info.count + 0x7F);
    if (mem) {
        v->mem = mem;
        u8* p = (u8*)(((u32)mem + 0x7F) & ~0x7F);
        for (int k = 0; k < v->info.count; k++) {
            if (v->info.frames[k].y.on) {
                v->info.frames[k].y.buf = p;
                v->info.frames[k].y.width = lumaW;
                if (!fn_802E18D4(lumaW, v->info.height, 1, 0, &v->tex[k].y, p)) {
                    goto fail;
                }
            }
            if (v->info.frames[k].u.on) {
                v->info.frames[k].u.buf = p + offU;
                v->info.frames[k].u.width = chromaW;
                if (!fn_802E18D4(chromaW, v->info.cheight, 1, 0, &v->tex[k].u, p + offU)) {
                    goto fail;
                }
            }
            if (v->info.frames[k].v.on) {
                v->info.frames[k].v.buf = p + offV;
                v->info.frames[k].v.width = chromaW;
                if (!fn_802E18D4(chromaW, v->info.cheight, 1, 0, &v->tex[k].v, p + offV)) {
                    goto fail;
                }
            }
            if (v->info.frames[k].a.on) {
                v->info.frames[k].a.buf = p + offA;
                v->info.frames[k].a.width = lumaW;
                if (!fn_802E18D4(lumaW, v->info.height, 1, 0, &v->tex[k].a, p + offA)) {
                    goto fail;
                }
            }
            p += frame;
        }
        v->frameSize = frame;
        fn_802E1A38(v, p);
        return 1;
    }
fail:
    fn_802E1728(v);
    return 0;
}

void fn_802E1DA4(Video* v, u32 w, u32 h, f32 x, f32 y, f32 sx, f32 sy, f32 alpha) {
    int a;
    if (alpha >= 0.99f) {
        a = 0xFF;
    } else if (alpha <= 0.01f) {
        a = 0;
    } else {
        a = (int)(0.5f + 255.0f * alpha);
    }
    int hasAlpha;
    if (v->info.frames[0].a.buf) {
        hasAlpha = 1;
    } else {
        hasAlpha = 0;
    }
    v->busy[v->info.cur] = 1;
    GXSetVtxAttrFmt(0, 9, 1, 3, 0);
    GXSetVtxAttrFmt(0, 11, 1, 5, 0);
    GXSetVtxAttrFmt(0, 13, 1, 4, 0);
    GXSetVtxAttrFmt(0, 14, 1, 4, 0);
    GXClearVtxDesc();
    GXLoadTexObj(&v->tex[v->info.cur].y, 0);
    GXLoadTexObj(&v->ramp1, 1);
    GXLoadTexObj(&v->ramp2, 2);
    GXLoadTexObj(&v->tex[v->info.cur].u, 3);
    GXLoadTexObj(&v->tex[v->info.cur].v, 4);
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(11, 1);
    GXSetVtxDesc(13, 1);
    GXSetVtxDesc(14, 1);
    if (hasAlpha) {
        GXLoadTexObj(&v->tex[v->info.cur].a, 5);
    }
    GXSetNumChans(1);
    GXSetNumTexGens(4);
    GXSetNumTevStages(4);
    GXSetNumIndStages(3);
    GXSetIndTexOrder(0, 1, 1);
    GXSetIndTexOrder(1, 2, 2);
    if (hasAlpha) {
        GXSetBlendMode(1, 4, 5, 0);
    } else if ((u8)a != 0xFF) {
        GXSetBlendMode(1, 4, 5, 0);
    } else {
        GXSetBlendMode(1, 1, 0, 0);
    }
    GXSetTevIndirect(0, 1, 0, 7, 1, 0, 0, 0, 0, 0);
    GXSetTevOrder(0, 0, 4, 0xFF);
    GXSetTevColorIn(0, 15, 8, 14, 2);
    GXSetTevColorOp(0, 0, 0, 0, 0, 0);
    GXSetTevAlphaIn(0, 7, 4, 6, 1);
    GXSetTevAlphaOp(0, 1, 0, 0, 0, 0);
    GXSetTevKColorSel(0, 12);
    GXSetTevKAlphaSel(0, 0x1C);
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevIndirect(1, 1, 0, 7, 1, 0, 0, 0, 0, 0);
    GXSetTevOrder(1, 0, 3, 0xFF);
    GXSetTevColorIn(1, 15, 8, 14, 0);
    GXSetTevColorOp(1, 0, 0, 1, 0, 0);
    GXSetTevAlphaIn(1, 7, 4, 6, 0);
    GXSetTevAlphaOp(1, 1, 0, 0, 0, 0);
    GXSetTevKColorSel(1, 13);
    GXSetTevKAlphaSel(1, 0x1D);
    GXSetTevSwapMode(1, 0, 0);
    GXSetTevIndirect(2, 0, 0, 7, 1, 0, 0, 0, 0, 0);
    GXSetTevOrder(2, 3, 0, 0xFF);
    GXSetTevColorIn(2, 15, 8, 12, 0);
    GXSetTevColorOp(2, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(2, 4, 7, 7, 0);
    GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
    GXSetTevSwapMode(2, 0, 0);
    if (hasAlpha) {
        GXSetTevIndirect(3, 0, 0, 7, 1, 0, 0, 0, 0, 0);
        GXSetTevOrder(3, 3, 5, 4);
        GXSetTevColorIn(3, 1, 0, 14, 15);
        GXSetTevColorOp(3, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn(3, 7, 4, 5, 7);
        GXSetTevAlphaOp(3, 0, 0, 0, 1, 0);
        GXSetTevSwapMode(3, 0, 0);
        GXSetTevKColorSel(3, 14);
    } else {
        GXSetTevOrder(3, 0xFF, 0xFF, 4);
        GXSetTevColorIn(3, 1, 0, 14, 15);
        GXSetTevColorOp(3, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn(3, 7, 6, 5, 7);
        GXSetTevAlphaOp(3, 0, 0, 0, 1, 0);
        GXSetTevSwapMode(3, 0, 0);
        GXSetTevKColorSel(3, 14);
    }
    GXColorS10 c0 = {-90, 0, -114, 135};
    GXSetTevColorS10(1, c0);
    GXColor k0 = {0, 0, 0xE2, 0x58};
    GXSetTevKColor(0, k0);
    GXColor k1 = {0xB3, 0, 0, 0xB6};
    GXSetTevKColor(1, k1);
    GXColor k2 = {0xFF, 0, 0xFF, 0x80};
    GXSetTevKColor(2, k2);
    GXSetTevSwapModeTable(0, 0, 1, 2, 3);
    GXSetIndTexMtx(1, (const f32(*)[3])lbl_802FAE40, 4);
    lbl_804DB740[0][0] = (f32)(v->info.width >> 3) / (f32)v->rampW[0];
    lbl_804DB740[1][1] = (f32)v->info.height / (f32)v->rampW[1];
    lbl_804DB760[0][0] = (f32)(v->info.cwidth >> 3) / (f32)v->rampW2[0];
    lbl_804DB760[1][1] = (f32)v->info.cheight / (f32)v->rampW2[1];
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    GXSetTexCoordGen2(1, 1, 4, 0x21, 0, 0x7D);
    GXSetTexCoordGen2(2, 1, 4, 0x24, 0, 0x7D);
    GXSetTexCoordGen2(3, 1, 5, 0x27, 0, 0x7D);
    GXLoadTexMtxImm(lbl_802FAE60, 0x1E, 1);
    GXLoadTexMtxImm(lbl_804DB740, 0x21, 1);
    GXLoadTexMtxImm(lbl_804DB760, 0x24, 1);
    GXLoadTexMtxImm(lbl_802FAE60, 0x27, 1);
    GXBegin(0x80, 0, 4);
    s16 x0 = (int)x;
    s16 y0 = (int)y;
    s16 x1 = x0 + (int)((f32)w * sx);
    s16 y1 = y0 + (int)((f32)h * sy);
    GXPosition3s16(x0, y0, 0);
    GXColor4u8(a, a, a, a);
    GXTexCoord2f32(0.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3s16(x1, y0, 0);
    GXColor4u8(a, a, a, a);
    GXTexCoord2f32(1.0f, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3s16(x1, y1, 0);
    GXColor4u8(a, a, a, a);
    GXTexCoord2f32(1.0f, 1.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3s16(x0, y1, 0);
    GXColor4u8(a, a, a, a);
    GXTexCoord2f32(0.0f, 1.0f);
    GXTexCoord2f32(0.0f, 1.0f);
}

void fn_802E27A0(Video* v) { DCStoreRange(v->info.frames[v->info.cur].y.buf, v->frameSize); }

void fn_802E27B8(Video* v) {
    int next = v->info.cur + 1;
    if (next >= v->info.count) {
        next = 0;
    }
    if (v->busy[next]) {
        v->busy[next] = 0;
    }
}

// Only its strings survive in the game.
void KernelErrorHandler(u16 err) {
    fn_802E30C8("error_handler(%x)", err);
    fn_802E30C8("%s", lbl_8034CCE0[0].name);
}

void fn_802E27F0(void) {
    u32 here;
    lbl_804DB780.size = 0x7400;
    lbl_804DB780.top = &here;
    fn_802E30C8("stack_check: hoping stack size %dK from 0x%p", lbl_804DB780.size / 1024, &here);
    int n = lbl_804DB780.size / 4;
    for (int i = 0; i < n; i++) {
        lbl_804DB780.top[-i] = 0x53FFEEDD;
    }
    lbl_804DB780.bottom = &lbl_804DB780.top[-(n - 1)];
}

void fn_802E29AC(void) {
    u32* top = lbl_804DB780.top;
    int size = lbl_804DB780.size;
    for (int i = size / 4 - 1; i >= 0; i--) {
        if (top[-i] != 0x53FFEEDD) {
            fn_802E30C8("stack usage: %d/%dK\n", (i * 4) / 1024, size / 1024);
            return;
        }
    }
}

void fn_802E2A1C(void) {
    if (lbl_804DB780.bottom && *lbl_804DB780.bottom != 0x53FFEEDD) {
        fn_802E317C("STACK OVERRUN!");
    }
}

void fn_802E2A54(void) {}

void fn_802E2A58(void) {}

void fn_802E77A8(void);
void fn_802E3498(void);
void fn_801293B4(void);
void fn_802E302C(void);
void fn_8012820C(u32 size);
void fn_802E98D0(void);
void fn_802E7318(void);
void fn_802E46B0(void);
void fn_80127C8C(void);
void fn_802E8CA8(void);
void fn_80127538(void);
void fn_80129790(void);
void fn_802E5478(void);
void fn_802E7578(void);
void fn_802EBE2C(void);
void fn_802CF600(void);
void fn_802EC614(void);
void fn_802E2CFC(KDate* d);
void fn_802EC7AC(void);
void fn_802CFBA8(void);
void fn_802EC038(void);
void fn_802E759C(void);
void fn_802E56F0(void);
void fn_80129864(void);
void fn_801275AC(void);
void fn_802E8D2C(void);
void fn_80127EF8(void);
void fn_802E46E4(void);
void fn_802E7330(void);
void fn_802E99A8(void);
void fn_801282A0(void);
void fn_802E3090(void);
void fn_801293C4(void);
void fn_802E34DC(void);
void fn_802E77AC(void);

int fn_802E2A5C(void) {
    OSInit();
    VIInit();
    LCEnable();
    fn_802E77A8();
    fn_802E3498();
    fn_801293B4();
    fn_802E302C();
    fn_802E2A54();
    fn_8012820C(0x2000000);
    fn_802E98D0();
    fn_802E7318();
    fn_802E46B0();
    fn_80127C8C();
    fn_802E8CA8();
    fn_80127538();
    fn_80129790();
    fn_802E5478();
    fn_802E7578();
    fn_802EBE2C();
    fn_802CF600();
    fn_802EC614();
    KDate d;
    fn_802E2CFC(&d);
    fn_802E30C8("date: %d/%d/%d (%d)", d.month, d.day, d.year, d.wday);
    fn_802E30C8("time: %d:%02d:%02d", d.hour, d.min, d.sec);
    fn_802E27F0();
    fn_80127C90(fn_802E2A1C, "check_stack");
    return 1;
}

void fn_802E2B34(void) {
    fn_80127D58(fn_802E2A1C);
    fn_802E29AC();
    fn_802EC7AC();
    fn_802CFBA8();
    fn_802EC038();
    fn_802E759C();
    fn_802E56F0();
    fn_80129864();
    fn_801275AC();
    fn_802E8D2C();
    fn_80127EF8();
    fn_802E46E4();
    fn_802E7330();
    fn_802E99A8();
    fn_801282A0();
    fn_802E2A58();
    fn_802E3090();
    fn_801293C4();
    fn_802E34DC();
    fn_802E77AC();
}

int main(void) {
    char cmd[0x100] = {0};
    if (ParseCommandLine(cmd) && fn_802E2A5C()) {
        AppMain();
        fn_802E2B34();
    }
    OSPanic("kernel.cpp", 210, "END");
    return 0;
}

#define OS_BUS_CLOCK (*(u32*)0x800000F8)

s32 fn_802E2C20(void) {
    s64 t = OSGetTime();
    if (lbl_8055FEF8 == 0) {
        lbl_8055FEF8 = t;
    }
    return (t - lbl_8055FEF8) / ((OS_BUS_CLOCK / 4) / 1000);
}

s64 fn_802E2C9C(void) {
    s64 t = OSGetTime();
    return ((t - lbl_8055FEF8) * 8) / ((OS_BUS_CLOCK / 4) / 125000);
}

void fn_802E2CFC(KDate* d) {
    KCalendar c;
    OSTicksToCalendarTime(OSGetTime(), &c);
    d->sec = c.sec;
    d->min = c.min;
    d->hour = c.hour;
    d->day = c.mday;
    d->month = c.mon + 1;
    d->year = c.year;
    d->wday = c.wday;
    d->msec = c.msec;
}

void fn_802E2D74(int ms) {
    s64 wait = (s64)ms * 1000;
    s64 start = fn_802E2C9C();
    s64 now;
    do {
        now = fn_802E2C9C();
    } while (now - start < wait && now >= start);
}

void fn_802E2E0C(int (*cb)(int, int), const char* name) {
    int i = lbl_8055FF00++;
    lbl_804DB790[i] = cb;
    lbl_804DB7D0[i] = name;
}

void fn_802E2E38(int mode) {
    fn_802E30C8("resetting system...");
    for (int i = 0; i < lbl_8055FF00; i++) {
        if (lbl_804DB790[i](0, mode) == 1) {
            fn_802E30C8(" reset cancelled by %s", lbl_804DB7D0[i]);
            return;
        }
    }
    fn_802E30C8("kernel: reset is okay");
    for (int i = lbl_8055FF00 - 1; i >= 0; i--) {
        lbl_804DB790[i](1, mode);
    }
    fn_802E30C8("kernel: cancelling DVD...");
    if (DVDCancelAll()) {
        fn_802E30C8("DVDCancelAll fails");
    }
    fn_802E30C8("doing reset %d", mode);
    if (mode == 0) {
        if (!fn_802D9BF0() || fn_802D9C04()) {
            fn_802E30C8("kernel: doing restart");
            OSRestart(0);
        } else {
            fn_802E30C8("kernel: restart during disk error (go to menu)");
            OSReturnToMenu();
        }
    } else if (mode == 2) {
        fn_802E30C8("kernel: doing power down");
        __WPADShutdown();
        fn_802E30C8("kernel: waiting for WPAD to shut down");
        while (WPADGetStatus() == 4) {
        }
        fn_802E30C8("kernel: WPAD has shut down");
        OSShutdownSystem();
    } else {
        fn_802E30C8("kernel: doing return to menu");
        OSReturnToMenu();
    }
}

void fn_802E300C(int code) { fn_802E317C("PlatExitProcess(%d)", code); }

}  // extern "C"
