// Kernel (0x802E1728-0x802E3028): the program entry, system init/shutdown order, stack overrun guard,
// millisecond/microsecond clocks, the reset/power-off path with its veto callbacks, and the overlay that
// draws a planar YUV(A) video frame through indirect texturing (two colour-ramp lookup textures turn the
// chroma planes into RGB). Names are guesses apart from the strings.
#include "game/common.h"
#include <revolution/GX/GXTypes.h>
#include <revolution/GX/GXVert.h>
#include <revolution/GX/GXAttr.h>
#include <revolution/GX/GXBump.h>
#include <revolution/GX/GXGeometry.h>
#include <revolution/GX/GXLight.h>
#include <revolution/GX/GXPixel.h>
#include <revolution/GX/GXTev.h>
#include <revolution/GX/GXTexture.h>
#include <revolution/GX/GXTransform.h>
#include <revolution/OS/OS.h>
#include <revolution/OS/OSCache.h>
#include <revolution/OS/OSError.h>
#include <revolution/OS/OSReset.h>
#include <revolution/OS/OSTime.h>
#include <revolution/VI/vi.h>

extern "C" {
void* radmalloc(u32 size);  // allocate
void radfree(void* p);
void fn_802E30C8(const char* fmt, ...);
void fn_802E317C(const char* fmt, ...);  // fatal
int DVDCancelAll(void);
void __WPADShutdown(void);
s32 WPADGetStatus(void);
void fn_80127C90(void (*cb)(void), const char* name);
void fn_80127D58(void (*cb)(void));
int fn_802D9BF0(void);
int fn_802D9C04(void);

}

// GXColor: the SDK's.

// GXColorS10: the SDK's.

extern "C" {
}

int ParseCommandLine(const char* cmd);
void AppMain(void);



// GXTexObj: the SDK's.

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

namespace {
extern const f32 lbl_802FAE40[2][4];
extern const f32 lbl_802FAE60[2][4];
}

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
        radfree(v->mem);
        v->mem = 0;
    }
}

u32 fn_802E176C(u32 x) { return 1 << (32 - __cntlzw(x)); }

// A colour ramp: column k holds (k - 128, row - 128) in its two channels.
void fn_802E1780(u8* dst, u32 w, u32 pitch) {
    u8* p;
    const u32 columns = w >> 3;
    int a;
    int b;
    int k;
    int row;
    a = 0;
    b = 0;
    for (row = 0; row < 4; row++) {
        for (k = 0; k < columns; k++) {
            p = &dst[(k + row * pitch) * 2];
            p[0] = a - k + 0x80;
            p[1] = b - row + 0x80;
            if (++b == 4) {
                b = 0;
                a++;
            }
        }
    }
}

/* Helper name unproven */
// Copies one 8-byte x 4-row tile.
static inline void CopyTile(u32* d, const u8* s, u32 off, u32 pitch) {
    for (int r = 0; r < 4; r++) {
        const u32* p = (const u32*)(s + off + pitch * r);
        d[r * 2] = p[0];
        d[r * 2 + 1] = p[1];
    }
}

// Swizzles linear rows into 4-row tiles of 8 bytes.
void fn_802E1804(u8* dst, const u8* src, int w, u32 h, int bpp) {
    u32 k;
    u32 y;
    u32 pitch;
    u32 cols;
    u32 rows;
    int out;
    out = 0;
    pitch = w * bpp;
    rows = h >> 2;
    cols = pitch >> 3;
    for (y = 0; y < rows; y++) {
        const u32 row = y * 4;
        for (k = 0; k < cols; k++) {
            CopyTile((u32*)(dst + out), src, row * pitch + k * 8, pitch);
            out += 0x20;
        }
    }
}

int fn_802E18D4(u32 w, u32 h, int fmt, int wrap, GXTexObj* obj, void* image) {
    GXInitTexObj((GXTexObj*)obj, (void*)image, (u16)w, (u16)h, (GXTexFmt)fmt, (GXTexWrapMode)wrap, (GXTexWrapMode)wrap, (GXBool)0);
    GXInitTexObjLOD((GXTexObj*)obj, (GXTexFilter)0, (GXTexFilter)0, 0.0f, 0.0f, 0.0f, (GXBool)0, (GXBool)0, (GXAnisotropy)0);
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
    DCFlushRange((const void*)buf, (u32)size);
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
    void* mem = radmalloc(ramps + frame * v->info.count + 0x7F);
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
    int hasAlpha;
    int a;
    if (alpha >= 0.99f) {
        a = 0xFF;
    } else if (alpha <= 0.01f) {
        a = 0;
    } else {
        // A two-statement branch: IRO does not fold this if/else into a ?:, so 0.01f is pooled before 0.5f/255.0f.
        f32 scaled = 0.5f + 255.0f * alpha;
        a = (int)scaled;
    }
    // The target branches (beq; li 1; b; li 0); every if/else or ?: form becomes branch-free (neg/or/srwi) here.
    // A switch with default first reproduces it; the original may have switched on a related format value.
    switch ((u32)v->info.frames[0].a.buf) {
    default:
        hasAlpha = 1;
        break;
    case 0:
        hasAlpha = 0;
        break;
    }
    v->busy[v->info.cur] = 1;
    GXSetVtxAttrFmt((GXVtxFmt)0, (GXAttr)9, (GXCompCnt)1, (GXCompType)3, (u8)0);
    GXSetVtxAttrFmt((GXVtxFmt)0, (GXAttr)11, (GXCompCnt)1, (GXCompType)5, (u8)0);
    GXSetVtxAttrFmt((GXVtxFmt)0, (GXAttr)13, (GXCompCnt)1, (GXCompType)4, (u8)0);
    GXSetVtxAttrFmt((GXVtxFmt)0, (GXAttr)14, (GXCompCnt)1, (GXCompType)4, (u8)0);
    GXClearVtxDesc();
    GXLoadTexObj((const GXTexObj*)&v->tex[v->info.cur].y, (GXTexMapID)0);
    GXLoadTexObj((const GXTexObj*)&v->ramp1, (GXTexMapID)1);
    GXLoadTexObj((const GXTexObj*)&v->ramp2, (GXTexMapID)2);
    GXLoadTexObj((const GXTexObj*)&v->tex[v->info.cur].u, (GXTexMapID)3);
    GXLoadTexObj((const GXTexObj*)&v->tex[v->info.cur].v, (GXTexMapID)4);
    GXSetVtxDesc((GXAttr)9, (GXAttrType)1);
    GXSetVtxDesc((GXAttr)11, (GXAttrType)1);
    GXSetVtxDesc((GXAttr)13, (GXAttrType)1);
    GXSetVtxDesc((GXAttr)14, (GXAttrType)1);
    if (hasAlpha) {
        GXLoadTexObj((const GXTexObj*)&v->tex[v->info.cur].a, (GXTexMapID)5);
    }
    GXSetNumChans((u8)1);
    GXSetNumTexGens((u8)4);
    GXSetNumTevStages((u8)4);
    GXSetNumIndStages((u8)3);
    GXSetIndTexOrder((GXIndTexStageID)0, (GXTexCoordID)1, (GXTexMapID)1);
    GXSetIndTexOrder((GXIndTexStageID)1, (GXTexCoordID)2, (GXTexMapID)2);
    if (hasAlpha) {
        GXSetBlendMode((GXBlendMode)1, (GXBlendFactor)4, (GXBlendFactor)5, (GXLogicOp)0);
    } else if ((u8)a != 0xFF) {
        GXSetBlendMode((GXBlendMode)1, (GXBlendFactor)4, (GXBlendFactor)5, (GXLogicOp)0);
    } else {
        GXSetBlendMode((GXBlendMode)1, (GXBlendFactor)1, (GXBlendFactor)0, (GXLogicOp)0);
    }
    GXSetTevIndirect((GXTevStageID)0, (GXIndTexStageID)1, (GXIndTexFormat)0, (GXIndTexBiasSel)7, (GXIndTexMtxID)1, (GXIndTexWrap)0, (GXIndTexWrap)0, (GXBool)0, (GXBool)0, (GXIndTexAlphaSel)0);
    GXSetTevOrder((GXTevStageID)0, (GXTexCoordID)0, (GXTexMapID)4, (GXChannelID)0xFF);
    GXSetTevColorIn((GXTevStageID)0, (GXTevColorArg)15, (GXTevColorArg)8, (GXTevColorArg)14, (GXTevColorArg)2);
    GXSetTevColorOp((GXTevStageID)0, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)0, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)0, (GXTevAlphaArg)7, (GXTevAlphaArg)4, (GXTevAlphaArg)6, (GXTevAlphaArg)1);
    GXSetTevAlphaOp((GXTevStageID)0, (GXTevOp)1, (GXTevBias)0, (GXTevScale)0, (GXBool)0, (GXTevRegID)0);
    GXSetTevKColorSel((GXTevStageID)0, (GXTevKColorSel)12);
    GXSetTevKAlphaSel((GXTevStageID)0, (GXTevKAlphaSel)0x1C);
    GXSetTevSwapMode((GXTevStageID)0, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevIndirect((GXTevStageID)1, (GXIndTexStageID)1, (GXIndTexFormat)0, (GXIndTexBiasSel)7, (GXIndTexMtxID)1, (GXIndTexWrap)0, (GXIndTexWrap)0, (GXBool)0, (GXBool)0, (GXIndTexAlphaSel)0);
    GXSetTevOrder((GXTevStageID)1, (GXTexCoordID)0, (GXTexMapID)3, (GXChannelID)0xFF);
    GXSetTevColorIn((GXTevStageID)1, (GXTevColorArg)15, (GXTevColorArg)8, (GXTevColorArg)14, (GXTevColorArg)0);
    GXSetTevColorOp((GXTevStageID)1, (GXTevOp)0, (GXTevBias)0, (GXTevScale)1, (GXBool)0, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)1, (GXTevAlphaArg)7, (GXTevAlphaArg)4, (GXTevAlphaArg)6, (GXTevAlphaArg)0);
    GXSetTevAlphaOp((GXTevStageID)1, (GXTevOp)1, (GXTevBias)0, (GXTevScale)0, (GXBool)0, (GXTevRegID)0);
    GXSetTevKColorSel((GXTevStageID)1, (GXTevKColorSel)13);
    GXSetTevKAlphaSel((GXTevStageID)1, (GXTevKAlphaSel)0x1D);
    GXSetTevSwapMode((GXTevStageID)1, (GXTevSwapSel)0, (GXTevSwapSel)0);
    GXSetTevIndirect((GXTevStageID)2, (GXIndTexStageID)0, (GXIndTexFormat)0, (GXIndTexBiasSel)7, (GXIndTexMtxID)1, (GXIndTexWrap)0, (GXIndTexWrap)0, (GXBool)0, (GXBool)0, (GXIndTexAlphaSel)0);
    GXSetTevOrder((GXTevStageID)2, (GXTexCoordID)3, (GXTexMapID)0, (GXChannelID)0xFF);
    GXSetTevColorIn((GXTevStageID)2, (GXTevColorArg)15, (GXTevColorArg)8, (GXTevColorArg)12, (GXTevColorArg)0);
    GXSetTevColorOp((GXTevStageID)2, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevAlphaIn((GXTevStageID)2, (GXTevAlphaArg)4, (GXTevAlphaArg)7, (GXTevAlphaArg)7, (GXTevAlphaArg)0);
    GXSetTevAlphaOp((GXTevStageID)2, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
    GXSetTevSwapMode((GXTevStageID)2, (GXTevSwapSel)0, (GXTevSwapSel)0);
    if (hasAlpha) {
        GXSetTevIndirect((GXTevStageID)3, (GXIndTexStageID)0, (GXIndTexFormat)0, (GXIndTexBiasSel)7, (GXIndTexMtxID)1, (GXIndTexWrap)0, (GXIndTexWrap)0, (GXBool)0, (GXBool)0, (GXIndTexAlphaSel)0);
        GXSetTevOrder((GXTevStageID)3, (GXTexCoordID)3, (GXTexMapID)5, (GXChannelID)4);
        GXSetTevColorIn((GXTevStageID)3, (GXTevColorArg)1, (GXTevColorArg)0, (GXTevColorArg)14, (GXTevColorArg)15);
        GXSetTevColorOp((GXTevStageID)3, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
        GXSetTevAlphaIn((GXTevStageID)3, (GXTevAlphaArg)7, (GXTevAlphaArg)4, (GXTevAlphaArg)5, (GXTevAlphaArg)7);
        GXSetTevAlphaOp((GXTevStageID)3, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
        GXSetTevSwapMode((GXTevStageID)3, (GXTevSwapSel)0, (GXTevSwapSel)0);
        GXSetTevKColorSel((GXTevStageID)3, (GXTevKColorSel)14);
    } else {
        GXSetTevOrder((GXTevStageID)3, (GXTexCoordID)0xFF, (GXTexMapID)0xFF, (GXChannelID)4);
        GXSetTevColorIn((GXTevStageID)3, (GXTevColorArg)1, (GXTevColorArg)0, (GXTevColorArg)14, (GXTevColorArg)15);
        GXSetTevColorOp((GXTevStageID)3, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
        GXSetTevAlphaIn((GXTevStageID)3, (GXTevAlphaArg)7, (GXTevAlphaArg)6, (GXTevAlphaArg)5, (GXTevAlphaArg)7);
        GXSetTevAlphaOp((GXTevStageID)3, (GXTevOp)0, (GXTevBias)0, (GXTevScale)0, (GXBool)1, (GXTevRegID)0);
        GXSetTevSwapMode((GXTevStageID)3, (GXTevSwapSel)0, (GXTevSwapSel)0);
        GXSetTevKColorSel((GXTevStageID)3, (GXTevKColorSel)14);
    }
    GXColorS10 c0 = {-90, 0, -114, 135};
    GXSetTevColorS10((GXTevRegID)1, (GXColorS10)c0);
    GXColor k0 = {0, 0, 0xE2, 0x58};
    GXSetTevKColor((GXTevKColorID)0, (GXColor)k0);
    GXColor k1 = {0xB3, 0, 0, 0xB6};
    GXSetTevKColor((GXTevKColorID)1, (GXColor)k1);
    GXColor k2 = {0xFF, 0, 0xFF, 0x80};
    GXSetTevKColor((GXTevKColorID)2, (GXColor)k2);
    GXSetTevSwapModeTable((GXTevSwapSel)0, (GXTevColorChan)0, (GXTevColorChan)1, (GXTevColorChan)2, (GXTevColorChan)3);
    GXSetIndTexMtx((GXIndTexMtxID)1, (const f32(*)[3])lbl_802FAE40, (s8)4);
    f32 ys = (f32)(v->info.width >> 3) / (f32)v->rampW[0];
    f32 yt = (f32)v->info.height / (f32)v->rampW[1];
    f32 cs = (f32)(v->info.cwidth >> 3) / (f32)v->rampW2[0];
    f32 ct = (f32)v->info.cheight / (f32)v->rampW2[1];
    lbl_804DB740[0][0] = ys;
    lbl_804DB740[1][1] = yt;
    lbl_804DB760[0][0] = cs;
    lbl_804DB760[1][1] = ct;
    GXSetTexCoordGen2((GXTexCoordID)0, (GXTexGenType)1, (GXTexGenSrc)4, (u32)0x1E, (GXBool)0, (u32)0x7D);
    GXSetTexCoordGen2((GXTexCoordID)1, (GXTexGenType)1, (GXTexGenSrc)4, (u32)0x21, (GXBool)0, (u32)0x7D);
    GXSetTexCoordGen2((GXTexCoordID)2, (GXTexGenType)1, (GXTexGenSrc)4, (u32)0x24, (GXBool)0, (u32)0x7D);
    GXSetTexCoordGen2((GXTexCoordID)3, (GXTexGenType)1, (GXTexGenSrc)5, (u32)0x27, (GXBool)0, (u32)0x7D);
    GXLoadTexMtxImm((const f32 (*)[4])(lbl_802FAE60), (u32)0x1E, (GXMtxType)1);
    GXLoadTexMtxImm((const f32 (*)[4])(lbl_804DB740), (u32)0x21, (GXMtxType)1);
    GXLoadTexMtxImm((const f32 (*)[4])(lbl_804DB760), (u32)0x24, (GXMtxType)1);
    GXLoadTexMtxImm((const f32 (*)[4])(lbl_802FAE60), (u32)0x27, (GXMtxType)1);
    GXBegin((GXPrimitive)0x80, (GXVtxFmt)0, (u16)4);
    int x1;
    int x0;
    int y0;
    x0 = (int)x;
    y0 = (int)y;
    x1 = x0 + (int)((f32)w * sx);
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

void fn_802E27A0(Video* v) { DCStoreRange((const void*)(v->info.frames[v->info.cur].y.buf), (u32)v->frameSize); }

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
}

void stack_check(void) {
    u32 here;
#ifdef __MWERKS__
    lbl_804DB780.size = 0x7400;
#else
    lbl_804DB780.size = 0x40000;  // PORT: host (x86) frames are larger than the Wii's; give the overrun check 256K
#endif
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
void StrmEnd(void);
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
    stack_check();
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
    StrmEnd();
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
    if (!lbl_8055FEF8) {
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
    OSTicksToCalendarTime((s64)(OSGetTime()), (OSCalendarTime*)&c);
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
            OSRestart((u32)0);
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

// Indirect-texture matrices share the alignment of the first table.
namespace {
const f32 lbl_802FAE40[2][4] __attribute__((aligned(16))) = {{0.5f, 0.0f, 0.0f, 0.0f}, {0.0625f, 0.0f, 0.0f, 0.0f}};
const f32 lbl_802FAE60[2][4] = {{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}};
}
