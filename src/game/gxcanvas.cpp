// gx canvases (0x802C2CAC-0x802C5130): software pixel buffers in 16-bit (1 = ARGB4444, 3 = RGB555,
// 4 = RGB565) or 32-bit (5 = ARGB8888) formats, with a clip rect, a current-canvas pointer for point/
// rect/line drawing, format-converting copies and alpha blits between canvases, "CANV" resources, and
// RLE-encoded "STMP" stamps drawn through a 256-entry palette. Colours are passed as a pair of 16-bit
// values (565 in the high half, 555 in the low half). Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
void* fn_801284E8(const char* tag, s32 size);  // MemAlloc
void fn_80128658(void* p);                     // MemFree
void* fn_8012C370(const char* name, u32 tag, u32* version, u32* size, int flags, u8* swapped);
void fn_8012C4D8(void* res);
void fn_802DB2C8(void* src, int x, int y);
void fn_802E317C(const char* fmt, ...);  // fatal
void* memset(void* dst, int c, u32 n);
u32 strlen(const char* s);
int abs(int x);
}

struct gxCanvas {
    s8 fmt;     // offset 0x0
    u8 flags;   // offset 0x1
    void* data; // offset 0x4
    s32 w;      // offset 0x8
    s32 h;      // offset 0xC
    s32 stride; // offset 0x10
    s32 cx0;    // offset 0x14
    s32 cy0;    // offset 0x18
    s32 cx1;    // offset 0x1C
    s32 cy1;    // offset 0x20
};

// An RLE stamp: per frame and row, a byte stream of runs (bit 7 set = opaque run of palette indices).
struct gxStamp {
    s32 w;            // offset 0x0
    s32 h;            // offset 0x4
    s32 ox;           // offset 0x8
    s32 oy;           // offset 0xC
    s32 frames;       // offset 0x10
    u16 pal555[256];  // offset 0x14
    u16 pal565[256];  // offset 0x214
    u32 pal8888[256]; // offset 0x414
    u8** rows;        // offset 0x814
    u8* runs;         // offset 0x818
};

static inline int gxIs16(s8 fmt) { return fmt != 5 && fmt != 6; }
static inline int gxBpp(s8 fmt) { return fmt == 5 ? 4 : 2; }

static inline void gxSwap32(void* p) {
    u8 t;
    t = ((u8*)p)[0];
    ((u8*)p)[0] = ((u8*)p)[3];
    ((u8*)p)[3] = t;
    t = ((u8*)p)[1];
    ((u8*)p)[1] = ((u8*)p)[2];
    ((u8*)p)[2] = t;
}

static inline u16 gxPack555(int r, int g, int b) { return ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3); }
static inline u16 gxPack565(int r, int g, int b) { return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3); }
static inline u32 gxColor(u32 r, u32 g, u32 b) { return (gxPack565(r, g, b) << 16) | gxPack555(r, g, b); }
static inline u16 gxShort(int v) { return v; }

ModuleTag lbl_8055FBB0;
u16 lbl_8055FBB8 = gxShort(0x100);
u16 lbl_8055FBBA = gxShort(0x405);
u16 lbl_8055FBBC = gxShort(0x401);
u32 lbl_8055FBC0 = gxColor(0, 0, 0);
u32 lbl_8055FBC4 = gxColor(0, 0, 0xFF);
u32 lbl_8055FBC8 = gxColor(0, 0x7F, 0);
u32 lbl_8055FBCC = gxColor(0, 0x7F, 0x7F);
u32 lbl_8055FBD0 = gxColor(0x7F, 0, 0);
u32 lbl_8055FBD4 = gxColor(0x7F, 0, 0x7F);
u32 lbl_8055FBD8 = gxColor(0x7F, 0x7F, 0);
u32 lbl_8055FBDC = gxColor(0x7F, 0x7F, 0x7F);
u32 lbl_8055FBE0 = gxColor(0x40, 0x40, 0x40);
u32 lbl_8055FBE4 = gxColor(0, 0, 0xFF);
u32 lbl_8055FBE8 = gxColor(0, 0xFF, 0);
u32 lbl_8055FBEC = gxColor(0, 0xFF, 0xFF);
u32 lbl_8055FBF0 = gxColor(0xFF, 0, 0);
u32 lbl_8055FBF4 = gxColor(0xFF, 0, 0xFF);
u32 lbl_8055FBF8 = gxColor(0xFF, 0xFF, 0);
u32 lbl_8055FBFC = gxColor(0xFF, 0xFF, 0xFF);
gxCanvas* lbl_8055FC00;

s8 lbl_8055CFF8 = 3;

extern "C" {

void fn_802C2CE8(gxCanvas* c, int w, int h, s8 fmt);
void fn_802C3014(gxCanvas* c);
void fn_802C5024(int x, int y, int ch, u32 color);

u32 fn_802C2CAC(u16 c) { return 0xFF000000 | ((c & 0x7C00) << 9) | ((c & 0x3E0) << 6) | ((c & 0x1F) << 3); }

u32 fn_802C2CC4(u16 c) { return 0xFF000000 | ((c & 0xF800) << 8) | ((c & 0x7E0) << 5) | ((c & 0x1F) << 3); }

void fn_802C2CDC(gxCanvas* c, int w, int h) { fn_802C2CE8(c, w, h, lbl_8055CFF8); }

void fn_802C2CE8(gxCanvas* c, int w, int h, s8 fmt) {
    c->fmt = fmt;
    c->flags = 0x82;
    c->data = fn_801284E8("canvas", w * h * gxBpp(fmt));
    c->w = w;
    c->h = h;
    c->stride = w * gxBpp(fmt);
    fn_802C3014(c);
}

void fn_802C2D94(gxCanvas* c) {
    fn_80128658(c->data);
    memset(c, 0, sizeof(gxCanvas));
}

gxCanvas* fn_802C2DD4(const char* name) {
    u32 ver;
    u32 size;
    u8 swapped;
    gxCanvas* c = (gxCanvas*)fn_8012C370(name, 'CANV', &ver, &size, 0, &swapped);
    if (c) {
        if (ver == 1) {
            if (swapped) {
                gxSwap32(&c->data);
                gxSwap32(&c->w);
                gxSwap32(&c->h);
                gxSwap32(&c->stride);
                gxSwap32(&c->cx0);
                gxSwap32(&c->cy0);
                gxSwap32(&c->cx1);
                gxSwap32(&c->cy1);
                u32* p = (u32*)(c + 1);
                c->data = p;
                for (int y = 0; y < c->h; y++) {
                    for (int x = 0; x < c->w; x++) {
                        *p = (*p >> 8) | (*p << 24);
                        p++;
                    }
                }
            }
        } else {
            fn_802E317C("gxCanvasGet(%s): wrong ver (%x != %x)", name, ver, 1);
        }
    } else {
        fn_802E317C("gxCanvasGet(%s) fails", name);
    }
    return c;
}

void fn_802C2FC4(gxCanvas* c) { fn_8012C4D8(c); }

void fn_802C2FC8(gxCanvas* c, int x0, int y0, int x1, int y1) {
    c->cx0 = x0 >= 0 ? x0 : 0;
    c->cy0 = y0 >= 0 ? y0 : 0;
    c->cx1 = x1 < c->w ? x1 : c->w;
    c->cy1 = y1 < c->h ? y1 : c->h;
}

void fn_802C3014(gxCanvas* c) {
    c->cx0 = 0;
    c->cy0 = 0;
    c->cx1 = c->w;
    c->cy1 = c->h;
}

gxCanvas* fn_802C3034(gxCanvas* c) {
    gxCanvas* old = lbl_8055FC00;
    lbl_8055FC00 = c;
    return old;
}

u16* fn_802C3044(gxCanvas* c, int x, int y) { return (u16*)((u8*)c->data + x * 2 + c->stride * y); }

u32* fn_802C3060(gxCanvas* c, int x, int y) { return (u32*)((u8*)c->data + x * 4 + c->stride * y); }

int fn_802C307C(gxCanvas* c, int* x0, int* y0, int* x1, int* y1) {
    if (*x0 >= c->cx1) {
        return 0;
    }
    if (*y0 >= c->cy1) {
        return 0;
    }
    if (*x0 < c->cx0) {
        *x0 = c->cx0;
    }
    if (*y0 < c->cy0) {
        *y0 = c->cy0;
    }
    if (*x1 > c->cx1) {
        *x1 = c->cx1;
    }
    if (*y1 > c->cy1) {
        *y1 = c->cy1;
    }
    if (*x0 >= *x1) {
        return 0;
    }
    return *y0 < *y1;
}

int fn_802C3130(gxCanvas* c, int x, int y) {
    if (x >= c->cx0 && y >= c->cy0 && x < c->cx1 && y < c->cy1) {
        return 1;
    }
    return 0;
}

// Clips a w*h blit to (dx, dy) against the canvas clip rect, adjusting the source position to match.
int fn_802C3170(gxCanvas* c, int* dx, int* dy, int* sx, int* sy, int* w, int* h) {
    if (*dx + *w < c->cx0) {
        return 0;
    }
    if (*dy + *h < c->cy0) {
        return 0;
    }
    if (*dx >= c->cx1) {
        return 0;
    }
    if (*dy >= c->cy1) {
        return 0;
    }
    if (*dx < c->cx0) {
        int d = c->cx0 - *dx;
        *sx += d;
        *w -= d;
        *dx = c->cx0;
    }
    if (*dy < c->cy0) {
        int d = c->cy0 - *dy;
        *sy += d;
        *h -= d;
        *dy = c->cy0;
    }
    if (*dx + *w > c->cx1) {
        *w -= *dx + *w - c->cx1;
    }
    if (*dy + *h > c->cy1) {
        *h -= *dy + *h - c->cy1;
    }
    if (*w <= 0) {
        return 0;
    }
    return *h > 0;
}

void fn_802C3320(int x, int y, u32 color);
void fn_802C33A8(int x, int y, u32 color);

void fn_802C32B8(int x, int y, u32 color) {
    gxCanvas* c = lbl_8055FC00;
    if (c->flags & 4) {
        return;
    }
    if (gxIs16(c->fmt)) {
        fn_802C3320(x, y, color);
    } else if (c->fmt == 5) {
        fn_802C33A8(x, y, color);
    } else {
        fn_802E317C("gxPoint: bad format %d", c->fmt);
    }
}

void fn_802C3320(int x, int y, u32 color) {
    if (fn_802C3130(lbl_8055FC00, x, y)) {
        u16 v = color >> 16;
        if (lbl_8055FC00->fmt == 3) {
            v = color;
        }
        *fn_802C3044(lbl_8055FC00, x, y) = v;
    }
}

static inline u32 gx565To8888(u32 c) { return 0xFF000000 | ((c & 0xF800) << 8) | ((c & 0x7E0) << 5) | ((c & 0x1F) << 3); }

void fn_802C33A8(int x, int y, u32 color) {
    if (fn_802C3130(lbl_8055FC00, x, y)) {
        u32* p = fn_802C3060(lbl_8055FC00, x, y);
        *p = gx565To8888(color >> 16);
    }
}

void fn_802C3478(int x0, int y0, int x1, int y1, u32 color);
void fn_802C356C(int x0, int y0, int x1, int y1, u32 color);
void fn_802C365C(int x0, int y0, int x1, int y1, u32 color);

void fn_802C3424(int x0, int y0, int x1, int y1, u32 color) {
    gxCanvas* c = lbl_8055FC00;
    if (gxIs16(c->fmt)) {
        if (c->fmt != 1) {
            fn_802C3478(x0, y0, x1, y1, color);
        } else {
            fn_802C365C(x0, y0, x1, y1, color);
        }
    } else if (c->fmt == 5) {
        fn_802C356C(x0, y0, x1, y1, color);
    }
}

void fn_802C3478(int x0, int y0, int x1, int y1, u32 color) {
    if (x1 == x0 || y1 == y0) {
        return;
    }
    if (!fn_802C307C(lbl_8055FC00, &x0, &y0, &x1, &y1)) {
        return;
    }
    u16 v = color >> 16;
    int w = x1 - x0;
    int h = y1 - y0;
    if (lbl_8055FC00->fmt == 3) {
        v = color;
    }
    u16* p = fn_802C3044(lbl_8055FC00, x0, y0);
    int skip = (lbl_8055FC00->stride - w * 2) / 2;
    do {
        int n = w;
        do {
            *p++ = v;
        } while (--n > 0);
        p += skip;
    } while (--h > 0);
}

void fn_802C356C(int x0, int y0, int x1, int y1, u32 color) {
    if (x1 == x0 || y1 == y0) {
        return;
    }
    if (!fn_802C307C(lbl_8055FC00, &x0, &y0, &x1, &y1)) {
        return;
    }
    u32 v;
    int w = x1 - x0;
    int h = y1 - y0;
    v = gx565To8888(color >> 16);
    u32* p = fn_802C3060(lbl_8055FC00, x0, y0);
    int skip = (lbl_8055FC00->stride - w * 4) / 4;
    do {
        int n = w;
        do {
            *p++ = v;
        } while (--n > 0);
        p += skip;
    } while (--h > 0);
}

// ARGB4444: replaces the colour and keeps the destination alpha.
void fn_802C365C(int x0, int y0, int x1, int y1, u32 color) {
    if (x1 == x0 || y1 == y0) {
        return;
    }
    if (!fn_802C307C(lbl_8055FC00, &x0, &y0, &x1, &y1)) {
        return;
    }
    u16 v;
    int w = x1 - x0;
    int h = y1 - y0;
    int b = (color & 0x1F) << 3;
    v = (b >> 4) | (((color >> 2) & 0xF0) | ((color >> 3) & 0xF00));
    u16* p = fn_802C3044(lbl_8055FC00, x0, y0);
    int skip = (lbl_8055FC00->stride - w * 2) / 2;
    do {
        int n = w;
        do {
            *p = v | (*p & 0xF000);
            p++;
        } while (--n > 0);
        p += skip;
    } while (--h > 0);
}

void fn_802C3918(gxCanvas* src, int x0, int y0, int x1, int y1, gxCanvas* dst, int dx, int dy);
void fn_802C3C68(gxCanvas* src, int x0, int y0, int x1, int y1, gxCanvas* dst, int dx, int dy);
void fn_802C3D74(gxCanvas* src, int x0, int y0, int x1, int y1, gxCanvas* dst, int dx, int dy);
void fn_802C3F00(gxCanvas* src, int x0, int y0, int x1, int y1, gxCanvas* dst, int dx, int dy);
void fn_802C4108(gxCanvas* src, int x0, int y0, int x1, int y1, gxCanvas* dst, int dx, int dy);
void fn_802C4388(gxCanvas* src, int x0, int y0, int x1, int y1, gxCanvas* dst, int dx, int dy);
void fn_802C4610(gxCanvas* src, int x0, int y0, int x1, int y1, gxCanvas* dst, int dx, int dy);

void fn_802C3754(gxCanvas* src, int x0, int y0, int x1, int y1, gxCanvas* dst, int dx, int dy) {
    if (gxIs16(src->fmt) && gxIs16(dst->fmt)) {
        fn_802C3918(src, x0, y0, x1, y1, dst, dx, dy);
    } else if (src->fmt == 5 && dst->fmt == 5) {
        fn_802C3C68(src, x0, y0, x1, y1, dst, dx, dy);
    } else if (gxIs16(src->fmt) && dst->fmt == 5) {
        fn_802C3D74(src, x0, y0, x1, y1, dst, dx, dy);
    } else if (src->fmt == 5 && gxIs16(dst->fmt)) {
        fn_802C3F00(src, x0, y0, x1, y1, dst, dx, dy);
    } else {
        fn_802E317C("unsupported copy (%d -> %d)", src->fmt, dst->fmt);
    }
}

void fn_802C3850(gxCanvas* src, int x0, int y0, int x1, int y1, gxCanvas* dst, int dx, int dy) {
    if (gxIs16(src->fmt) && gxIs16(dst->fmt)) {
        fn_802C4108(src, x0, y0, x1, y1, dst, dx, dy);
    } else if (src->fmt == 5 && dst->fmt == 5) {
        fn_802C4610(src, x0, y0, x1, y1, dst, dx, dy);
    } else if (src->fmt == 5 && gxIs16(dst->fmt)) {
        fn_802C4388(src, x0, y0, x1, y1, dst, dx, dy);
    } else {
        fn_802E317C("unsupported alpha blit (%d -> %d)", src->fmt, dst->fmt);
    }
}

void fn_802C3918(gxCanvas* src, int x0, int y0, int x1, int y1, gxCanvas* dst, int dx, int dy) {
    int w = x1 - x0;
    int h = y1 - y0;
    if (!fn_802C3170(dst, &dx, &dy, &x0, &y0, &w, &h)) {
        return;
    }
    u16* s = fn_802C3044(src, x0, y0);
    u16* d = fn_802C3044(dst, dx, dy);
    int cw = w;
    int ch = h;
    int sskip = (src->stride - cw * 2) / 2;
    int dskip = (dst->stride - cw * 2) / 2;
    do {
        int n = w;
        do {
            *d++ = *s++;
        } while (--n > 0);
        s += sskip;
        d += dskip;
    } while (--h > 0);
    if (src->fmt == dst->fmt) {
        return;
    }
    if (src->fmt == 4 && dst->fmt == 3) {
        s = fn_802C3044(src, x0, y0);
        d = fn_802C3044(dst, dx, dy);
        w = cw;
        h = ch;
        sskip = (src->stride - cw * 2) / 2;
        dskip = (dst->stride - cw * 2) / 2;
        do {
            int n = w;
            do {
                u16 p = *s++;
                *d++ = (p & 0x1F) | ((p >> 1) & 0x7FE0);
            } while (--n > 0);
            s += sskip;
            d += dskip;
        } while (--h > 0);
    } else if (src->fmt == 3 && dst->fmt == 4) {
        s = fn_802C3044(src, x0, y0);
        d = fn_802C3044(dst, dx, dy);
        w = cw;
        h = ch;
        sskip = (src->stride - cw * 2) / 2;
        dskip = (dst->stride - cw * 2) / 2;
        do {
            int n = w;
            do {
                u16 p = *s++;
                *d++ = ((p << 1) & 0xFFC0) | (p & 0x1F) | (((p << 1) & 0x400) >> 5);
            } while (--n > 0);
            s += sskip;
            d += dskip;
        } while (--h > 0);
    } else if (src->fmt == 1 && dst->fmt == 4) {
        s = fn_802C3044(src, x0, y0);
        d = fn_802C3044(dst, dx, dy);
        w = cw;
        h = ch;
        sskip = (src->stride - cw * 2) / 2;
        dskip = (dst->stride - cw * 2) / 2;
        do {
            int n = w;
            do {
                u16 p = *s++;
                *d++ = ((p & 0xF) << 1) | ((p << 3) & 0x780) | ((p << 4) & 0xF000);
            } while (--n > 0);
            s += sskip;
            d += dskip;
        } while (--h > 0);
    } else {
        fn_802E317C("copy_16_to_16: can't convert from %d to %d", src->fmt, dst->fmt);
    }
}

void fn_802C3C68(gxCanvas* src, int x0, int y0, int x1, int y1, gxCanvas* dst, int dx, int dy) {
    int w = x1 - x0;
    int h = y1 - y0;
    if (!fn_802C3170(dst, &dx, &dy, &x0, &y0, &w, &h)) {
        return;
    }
    u32* s = fn_802C3060(src, x0, y0);
    u32* d = fn_802C3060(dst, dx, dy);
    int sskip = (src->stride - w * 4) / 4;
    int dskip = (dst->stride - w * 4) / 4;
    do {
        int n = w;
        do {
            *d++ = *s++;
        } while (--n > 0);
        s += sskip;
        d += dskip;
    } while (--h > 0);
}

void fn_802C3D74(gxCanvas* src, int x0, int y0, int x1, int y1, gxCanvas* dst, int dx, int dy) {
    int w = x1 - x0;
    int h = y1 - y0;
    if (!fn_802C3170(dst, &dx, &dy, &x0, &y0, &w, &h)) {
        return;
    }
    u16* s = fn_802C3044(src, x0, y0);
    u32* d = fn_802C3060(dst, dx, dy);
    int sskip = (src->stride - w * 2) / 2;
    int dskip = (dst->stride - w * 4) / 4;
    if (src->fmt == 4) {
        do {
            int n = w;
            do {
                *d++ = fn_802C2CC4(*s++);
            } while (--n > 0);
            s += sskip;
            d += dskip;
        } while (--h > 0);
    } else if (src->fmt == 3) {
        do {
            int n = w;
            do {
                *d++ = fn_802C2CAC(*s++);
            } while (--n > 0);
            s += sskip;
            d += dskip;
        } while (--h > 0);
    } else {
        fn_802E317C("Can't copy from %d to %d", src->fmt, dst->fmt);
    }
}

void fn_802C3F00(gxCanvas* src, int x0, int y0, int x1, int y1, gxCanvas* dst, int dx, int dy) {
    int w = x1 - x0;
    int h = y1 - y0;
    if (!fn_802C3170(dst, &dx, &dy, &x0, &y0, &w, &h)) {
        return;
    }
    u32* s = fn_802C3060(src, x0, y0);
    u16* d = fn_802C3044(dst, dx, dy);
    int sskip = (src->stride - w * 4) / 4;
    int dskip = (dst->stride - w * 2) / 2;
    if (dst->fmt == 3) {
        do {
            int n = w;
            do {
                u32 p = *s++;
                int r = (p >> 16) & 0xFF;
                int g = (p >> 8) & 0xFF;
                int b = p & 0xFF;
                *d++ = ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3);
            } while (--n > 0);
            s += sskip;
            d += dskip;
        } while (--h > 0);
    } else if (dst->fmt == 4) {
        do {
            int n = w;
            do {
                u32 p = *s++;
                *d++ = gxPack565((p >> 16) & 0xFF, (p >> 8) & 0xFF, p & 0xFF);
            } while (--n > 0);
            s += sskip;
            d += dskip;
        } while (--h > 0);
    } else {
        fn_802E317C("Unsupported copy format: %d -> %d", src->fmt, dst->fmt);
    }
}

void fn_802C40B8(gxCanvas* src, int x, int y) {
    if (lbl_8055FC00->flags & 4) {
        fn_802DB2C8(src, x, y);
        return;
    }
    fn_802C3754(src, 0, 0, src->w, src->h, lbl_8055FC00, x, y);
}

void fn_802C40E8(gxCanvas* src, int x, int y) { fn_802C3850(src, 0, 0, src->w, src->h, lbl_8055FC00, x, y); }

// ARGB4444 source over a 555/565 destination.
void fn_802C4108(gxCanvas* src, int x0, int y0, int x1, int y1, gxCanvas* dst, int dx, int dy) {
    int w = x1 - x0;
    int h = y1 - y0;
    if (!fn_802C3170(dst, &dx, &dy, &x0, &y0, &w, &h)) {
        return;
    }
    u16* s = fn_802C3044(src, x0, y0);
    u16* d = fn_802C3044(dst, dx, dy);
    int sskip = (src->stride - w * 2) / 2;
    int dskip = (dst->stride - w * 2) / 2;
    if (dst->fmt == 3) {
        do {
            int n = w;
            do {
                u16 p = *s++;
                int a = ((p >> 8) & 0xF0) >> 4;
                u32 ia = 15 - a;
                u8 b = (ia * ((*d & 0x1F) << 3) + a * ((p & 0xF) << 4)) / 15;
                u8 r = (ia * ((*d >> 7) & 0xF8) + a * ((p >> 4) & 0xF0)) / 15;
                u8 g = (ia * ((*d >> 2) & 0xF8) + a * (p & 0xF0)) / 15;
                *d++ = gxPack555(r, g, b);
            } while (--n > 0);
            s += sskip;
            d += dskip;
        } while (--h > 0);
    } else if (dst->fmt == 4) {
        do {
            int n = w;
            do {
                u16 p = *s++;
                int a = ((p >> 8) & 0xF0) >> 4;
                u32 ia = 15 - a;
                u8 b = (ia * ((*d & 0x1F) << 3) + a * ((p & 0xF) << 4)) / 15;
                u8 r = (ia * ((*d >> 8) & 0xF8) + a * ((p >> 4) & 0xF0)) / 15;
                u8 g = (ia * ((*d >> 3) & 0xFC) + a * (p & 0xF0)) / 15;
                *d++ = gxPack565(r, g, b);
            } while (--n > 0);
            s += sskip;
            d += dskip;
        } while (--h > 0);
    } else {
        fn_802E317C("unsupported alpha blit format");
    }
}

// ARGB8888 source over a 555/565 destination.
void fn_802C4388(gxCanvas* src, int x0, int y0, int x1, int y1, gxCanvas* dst, int dx, int dy) {
    int w = x1 - x0;
    int h = y1 - y0;
    if (!fn_802C3170(dst, &dx, &dy, &x0, &y0, &w, &h)) {
        return;
    }
    u32* s = fn_802C3060(src, x0, y0);
    u16* d = fn_802C3044(dst, dx, dy);
    int sskip = (src->stride - w * 4) / 4;
    int dskip = (dst->stride - w * 2) / 2;
    if (dst->fmt == 3) {
        do {
            int n = w;
            do {
                u32 p = *s++;
                u32 a = p >> 24;
                u32 ia = 0xFF - a;
                u8 b = (ia * ((*d & 0x1F) << 3) + a * (p & 0xFF)) / 0xFF;
                u8 r = (ia * ((*d >> 7) & 0xF8) + a * ((p >> 16) & 0xFF)) / 0xFF;
                u8 g = (ia * ((*d >> 2) & 0xF8) + a * ((p >> 8) & 0xFF)) / 0xFF;
                *d++ = gxPack555(r, g, b);
            } while (--n > 0);
            s += sskip;
            d += dskip;
        } while (--h > 0);
    } else if (dst->fmt == 4) {
        do {
            int n = w;
            do {
                u32 p = *s++;
                u8 qb = ((*d & 0x1F) << 3) + 1;
                u8 qr = ((*d >> 8) & 0xF8) + 1;
                u8 qg = ((*d >> 3) & 0xFC) + 1;
                u32 a = p >> 24;
                u32 ia = 0xFF - a;
                u32 b = ((ia * qb + a * (p & 0xFF)) * 0x10101) >> 24;
                u32 r = ((ia * qr + a * ((p >> 16) & 0xFF)) * 0x10101) >> 24;
                u32 g = ((ia * qg + a * ((p >> 8) & 0xFF)) * 0x10101) >> 24;
                *d++ = gxPack565(r, g, b);
            } while (--n > 0);
            s += sskip;
            d += dskip;
        } while (--h > 0);
    } else {
        fn_802E317C("unsupported alpha blit format");
    }
}

static inline u8 gxBlend8(u32 ia, u32 dc, u32 a, u32 sc) {
    int t = ((ia * dc + a * sc) >> 8) & 0xFF;
    return t + (t >> 7);
}

void fn_802C4610(gxCanvas* src, int x0, int y0, int x1, int y1, gxCanvas* dst, int dx, int dy) {
    int w = x1 - x0;
    int h = y1 - y0;
    if (!fn_802C3170(dst, &dx, &dy, &x0, &y0, &w, &h)) {
        return;
    }
    u32* s = fn_802C3060(src, x0, y0);
    u32* d = fn_802C3060(dst, dx, dy);
    int sskip = (src->stride - w * 4) / 4;
    int dskip = (dst->stride - w * 4) / 4;
    do {
        int n = w;
        do {
            u32 p = *s++;
            u32 q = *d;
            u32 a = p >> 24;
            if (a == 0) {
            } else if (a == 0xFF) {
                q = (q & 0xFF000000) | (p & 0xFFFFFF);
            } else {
                u32 ia = 0xFF - a;
                u8 r = gxBlend8(ia, (q >> 16) & 0xFF, a, (p >> 16) & 0xFF);
                u8 g = gxBlend8(ia, (q >> 8) & 0xFF, a, (p >> 8) & 0xFF);
                u8 b = gxBlend8(ia, q & 0xFF, a, p & 0xFF);
                q = (r << 16 | g << 8) | ((q & 0xFF000000) | b);
            }
            *d++ = q;
        } while (--n > 0);
        s += sskip;
        d += dskip;
    } while (--h > 0);
}

void fn_802C47B8(int x0, int y0, int x1, int y1, u32 color) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    if (dx == 0) {
        if (dy == 0) {
            return;
        }
        int step = y0 < y1 ? 1 : -1;
        for (; y0 * step < step * y1; y0 += step) {
            fn_802C32B8(x0, y0, color);
        }
    } else if (dy == 0) {
        int step = x0 < x1 ? 1 : -1;
        for (; x0 * step < step * x1; x0 += step) {
            fn_802C32B8(x0, y0, color);
        }
    } else if (abs(dy) > abs(dx)) {
        int step = y0 < y1 ? 1 : -1;
        float slope = (float)step * (float)dx / (float)dy;
        float x = (float)x0;
        for (; y0 != y1; y0 += step) {
            fn_802C32B8((int)x, y0, color);
            x += slope;
        }
    } else {
        int step = x0 < x1 ? 1 : -1;
        float slope = (float)step * (float)dy / (float)dx;
        float y = (float)y0;
        for (; x0 != x1; x0 += step) {
            fn_802C32B8(x0, (int)y, color);
            y += slope;
        }
    }
}

gxStamp* fn_802C49E8(const char* name) {
    u8 swapped = 0;
    u32 size = 0;
    u32 ver;
    gxStamp* st = (gxStamp*)fn_8012C370(name, 'STMP', &ver, &size, 0, &swapped);
    if (st) {
        if (ver != 3) {
            fn_802E317C("Wrong stamp version in \"%s\": %x (expecting %x)", name, ver, 3);
        }
        if (swapped) {
            gxSwap32(&st->w);
            gxSwap32(&st->h);
            gxSwap32(&st->ox);
            gxSwap32(&st->oy);
            gxSwap32(&st->frames);
            gxSwap32(&st->rows);
            gxSwap32(&st->runs);
            u8** rows = (u8**)(st + 1);
            st->rows = rows;
            st->runs = (u8*)rows + st->h * st->frames * 4;
            for (int i = 0; i < st->h * st->frames; i++) {
                gxSwap32(&st->rows[i]);
                st->rows[i] += (u32)st->runs;
            }
            for (int i = 0; i < 256; i++) {
                gxSwap32(&st->pal8888[i]);
            }
        }
        return st;
    }
    return 0;
}

void fn_802C4C80(gxStamp* st) {
    if (st) {
        fn_8012C4D8(st);
    }
}

void fn_802C4C90(gxStamp* st, int x, int y, int frame, u16* pal) {
    gxCanvas* c = lbl_8055FC00;
    if (c->flags & 4) {
        return;
    }
    if (!st) {
        return;
    }
    if (frame >= st->frames) {
        return;
    }
    int oy = y - st->oy;
    int rowEnd = st->h < c->cy1 - oy ? st->h : c->cy1 - oy;
    int row = c->cy0 - oy < 0 ? 0 : c->cy0 - oy;
    int ox = x - st->ox;
    int colEnd = st->w < c->cx1 - ox ? st->w : c->cx1 - ox;
    int colStart = c->cx0 - ox < 0 ? 0 : c->cx0 - ox;
    u16* lut;
    if (pal) {
        if (c->fmt == 3) {
            lut = pal;
        } else {
            lut = pal + 0x100;
        }
    } else if (c->fmt == 3) {
        lut = st->pal555;
    } else {
        lut = st->pal565;
    }
    for (; row < rowEnd; row++) {
        u8* run = st->rows[st->h * frame + row];
        u16* d = fn_802C3044(lbl_8055FC00, ox, row + oy);
        int col = 0;
        while (col < colEnd) {
            int n = *run++;
            int opaque;
            if (n & 0x80) {
                n &= ~0x80;
                opaque = 1;
            } else {
                opaque = 0;
            }
            if (colEnd - col < n) {
                n = colEnd - col;
            }
            if (opaque && col < colStart) {
                int skip = colStart - col;
                if (n < skip) {
                    skip = n;
                }
                n -= skip;
                run += skip;
                d += skip;
                col += skip;
            }
            if (opaque) {
                for (u8 k = n; k != 0; k--) {
                    *d++ = lut[*run++];
                    col++;
                }
            } else {
                d += n;
                col += n;
            }
        }
    }
}

s32 fn_802C4EE4(gxStamp* st) {
    if (st) {
        return st->w;
    }
    return 0;
}

s32 fn_802C4EFC(gxStamp* st) {
    if (st) {
        return st->h;
    }
    return 0;
}

s32 fn_802C4F14(gxStamp* st) {
    if (st) {
        return st->frames;
    }
    return 0;
}

void fn_802C4F2C(gxStamp* st, s32* ox, s32* oy) {
    if (!st) {
        *oy = 0;
        *ox = 0;
        return;
    }
    *ox = st->ox;
    *oy = st->oy;
}

void fn_802C4F58(int x, int y, const char* str, u32 color) {
    char ch;
    while ((ch = *str++) != 0) {
        if (ch < ' ' || (u8)ch > 0x7F) {
            fn_802C5024(x, y, '#', color);
        } else {
            fn_802C5024(x, y, ch, color);
        }
        x += 8;
    }
}

int fn_802C4FF8(const char* str) { return strlen(str) * 8; }

int fn_802C501C(void) { return 8; }

void fn_802C5024(int x, int y, int ch, u32 color) {}

}  // extern "C"
