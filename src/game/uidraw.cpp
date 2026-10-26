// UI drawing helpers (0x80230090-0x80231504): UI start-up and shut-down, screen-wrapped rectangles and
// text, filled and textured quads, colour fading by the current UI context, the shared pulse/flash
// timer, TV safe-area margins and number formatting with thousands separators. Names are guesses.
#include "game/common.h"
#include <math.h>

struct UIXform {
    f32 m[3][3];  // offset 0x0
    f32 pos[3];   // offset 0x24
};

class UIContext {
public:
    static void operator delete(void* p);

    UIXform xf;  // offset 0x0
    f32 alpha;   // offset 0x30
    f32 unk34;   // offset 0x34
    f32 scale;   // offset 0x38
};

struct UIVert {
    f32 x;      // offset 0x0
    f32 y;      // offset 0x4
    f32 z;      // offset 0x8
    u32 color;  // offset 0xC
    f32 u;      // offset 0x10
    f32 v;      // offset 0x14
};

void UIContextBegin(void);
void UIContextEnd(void);

extern "C" {
void fn_80129C64(int a);
UIContext* fn_8022EDE4(const UIXform* xf);
void fn_8022ED00(void);
void fn_8022EDB8(void);
UIContext* fn_8022EDC4(void);
void fn_802321D8(void);
void fn_802324D8(void);
void fn_8022F248(void);
void fn_8022F294(void);
void fn_8023509C(void);
void fn_802350BC(void);
void fn_802365D8(void);
void fn_80236728(void);
void fn_80236774(void);
f32 fn_8023293C(s32 font, const char* text, f32 scale);
int fn_80232B70(s32 font);
void fn_80232BA4(const char* text, s32 font, f32 x, f32 y, u32 color, int align, f32 scale, f32 a, f32 b);
void fn_80233354(const char* text, s32 font, u32 color, int align, f32 x, f32 y, f32 scale, f32 a);
void fn_802C8B98(s32 tex, const void* m);
void fn_802C8DB8(UIVert* v, int nv, const u16* idx, int ni);
void fn_802C8D74(void);
void fn_802C87B4(int x, int y, int w, int h);
void fn_802C88BC(int a);
void fn_802C8530(void);
void fn_802C8638(const void* m);
void fn_802C974C(f32 a, f32 b, f32 c);
void fn_802C855C(void);
void fn_802C2B48(void);
int fn_802C9A68(void);
int fn_802C9A60(void);
int fn_802C9A58(void);
s32 fn_802E2C20(void);  // milliseconds
f32 fn_8023659C(void);  // seconds
extern s32 lbl_8055CFF0;  // screen width
extern s32 lbl_8055CFF4;  // screen height
extern s32 lbl_8055FB9C;  // layout width
extern s32 lbl_8055FBA0;  // layout height
extern f32 lbl_8055D020;  // aspect
}

ModuleTag lbl_8055F018;
UnkStatic7F lbl_8055F020;
static UnkStatic123 lbl_80497EF0;
s32 lbl_8055F024;
UIContext* lbl_8055F028;  // the root UI context
f32 lbl_8055F02C;         // pulse phase offset
f32 lbl_8055F030;         // pulse start time
u8 lbl_8055B998 = 1;      // pulsing enabled

extern "C" {

void fn_80230C9C(f32* a, f32* b, f32 lo, f32 hi, f32 range);
void fn_80231380(UIVert* v, u32 color, f32 x, f32 y, f32 z, f32 u, f32 w);
u32 fn_80230B9C(u32 color);

void fn_80230090(void) {
    fn_80129C64(2);
    UIContextBegin();
    UIXform xf;
    xf.m[0][0] = 1.0f;
    xf.m[0][1] = 0.0f;
    xf.m[0][2] = 0.0f;
    xf.m[1][0] = 0.0f;
    xf.m[1][1] = 1.0f;
    xf.m[1][2] = 0.0f;
    xf.m[2][0] = 0.0f;
    xf.m[2][1] = 0.0f;
    xf.m[2][2] = 1.0f;
    xf.pos[0] = 0.0f;
    xf.pos[1] = 0.0f;
    xf.pos[2] = 0.0f;
    lbl_8055F028 = fn_8022EDE4(&xf);
    fn_8022ED00();
    fn_802321D8();
    fn_8022F248();
    fn_8023509C();
    fn_802365D8();
    fn_80236728();
}

void fn_80230114(void) {
    fn_8022EDB8();
    delete lbl_8055F028;
    fn_80236774();
    fn_802350BC();
    fn_8022F294();
    fn_802324D8();
    UIContextEnd();
}

void fn_80230150(u32 color, int flags, f32 x, f32 y, f32 z, f32 w, f32 h, f32 speed, f32 t, f32 margin) {
    f32 x0 = x;
    f32 y0 = y;
    f32 x1 = x0 + w;
    f32 y1 = y0 + h;
    if (flags & 1) {
        y0 = y + t * speed;
        y1 = y0 + h;
    }
    if (flags & 2) {
        x0 = x + t * speed;
        x1 = x0 + w;
    }
    if (flags & 4) {
        z += t * speed;
    }
    f32 l = -320.0f;
    f32 r = 320.0f;
    f32 b = -240.0f;
    f32 tp = 240.0f;
    fn_802C8B98(-1, fn_8022EDC4());
    color = fn_80230B9C(color);
    if (flags & 8) {
        l -= margin;
        r += margin;
        b -= margin;
        tp += margin;
    }
    if ((flags & 8) || (flags & 16)) {
        fn_80230C9C(&x0, &x1, l, r, -l + r);
        fn_80230C9C(&y0, &y1, b, tp, -b + tp);
        f32 wx0 = -1.0f;
        f32 wx1 = wx0;
        f32 wy0 = wx0;
        f32 wy1 = wx0;
        if (x0 < l) {
            wy0 = y0;
            wx1 = r;
            wy1 = y1;
            wx0 = x0 + (-l + r);
        }
        if (x1 > r) {
            wy0 = y0;
            wx0 = l;
            wy1 = y1;
            wx1 = x1 - (-l + r);
        }
        if (y0 < b) {
            wy1 = tp;
            wy0 = y0 + (-b + tp);
            if (wx0 == -1.0f) {
                wx0 = x0;
                wx1 = x1;
            }
        }
        if (y1 > tp) {
            wy0 = b;
            wy1 = y1 - (-b + tp);
            if (wx0 == -1.0f) {
                wx0 = x0;
                wx1 = x1;
            }
        }
        UIVert v[4] __attribute__((aligned(16)));
        fn_80231380(&v[0], color, wx0, wy0, z, 0.0f, 0.0f);
        fn_80231380(&v[1], color, wx1, wy0, z, 1.0f, 0.0f);
        fn_80231380(&v[2], color, wx0, wy1, z, 0.0f, 1.0f);
        fn_80231380(&v[3], color, wx1, wy1, z, 1.0f, 1.0f);
        u16 idx[6] = {0, 3, 1, 0, 2, 3};
        fn_802C8DB8(v, 4, idx, 6);
    }
    UIVert v[4] __attribute__((aligned(16)));
    fn_80231380(&v[0], color, x0, y0, z, 0.0f, 0.0f);
    fn_80231380(&v[1], color, x1, y0, z, 1.0f, 0.0f);
    fn_80231380(&v[2], color, x0, y1, z, 0.0f, 1.0f);
    fn_80231380(&v[3], color, x1, y1, z, 1.0f, 1.0f);
    u16 idx[6] = {0, 3, 1, 0, 2, 3};
    fn_802C8DB8(v, 4, idx, 6);
    fn_802C8D74();
}

void fn_802304EC(const char* text, s32 font, f32 x, f32 y, f32 a, f32 scale, f32 speed, f32 t, f32 margin,
                 u32 color, int align) {
    f32 width = fn_8023293C(font, text, 1.0f);
    f32 x0 = x + t * speed;
    width = scale * width;
    f32 l = -320.0f - margin;
    f32 r = 320.0f + margin;
    f32 x1 = x0 + width;
    fn_80230C9C(&x0, &x1, l, r, -l + r);
    int wrap = 0;
    f32 wx = -320.0f;
    if (x0 < l) {
        wrap = 1;
        wx = x0 + (-l + r);
    }
    if (x1 > r) {
        wrap = 1;
        wx = (x1 - (-l + r)) - width;
    }
    if (wrap) {
        fn_80232BA4(text, font, wx, y, color, align, scale, a, 1.0f);
    }
    fn_80232BA4(text, font, x0, y, color, align, scale, a, 1.0f);
}

void fn_80230650(const char* text, s32 font, f32 x, f32 y, f32 a, f32 scale, f32 speed, f32 t, f32 margin,
                 u32 color, int align) {
    f32 width = fn_8023293C(font, text, 1.0f);
    f32 y0 = y + t * speed;
    width = scale * width;
    f32 l = -244.0f - margin;
    f32 r = 244.0f + margin;
    f32 y1 = y0 - width;
    fn_80230C9C(&y1, &y0, l, r, -l + r);
    int wrap = 0;
    f32 wy = -244.0f;
    if (y1 < l) {
        wrap = 1;
        f32 w0 = y1 + (-l + r);
        wy = w0 + width;
    }
    if (y0 > r) {
        wrap = 1;
        wy = y0 - (-l + r);
    }
    if (wrap) {
        fn_80233354(text, font, color, align, x, wy, scale, a);
    }
    fn_80233354(text, font, color, align, x, y0, scale, a);
}

void fn_802307AC(const char* text, s32 font, f32 x, f32 y, f32 a, f32 scale, f32 speed, f32 t, f32 margin,
                 u32 color, int align) {
    int lines = fn_80232B70(font);
    f32 y0 = y + t * speed;
    f32 height = scale * lines;
    f32 l = -240.0f - margin;
    f32 r = 240.0f + margin;
    f32 y1 = y0 - height;
    fn_80230C9C(&y1, &y0, l, r, -l + r);
    int wrap = 0;
    f32 wy = -240.0f;
    if (y1 < l) {
        wrap = 1;
        f32 w0 = y1 + (-l + r);
        wy = w0 + height;
    }
    if (y0 > r) {
        wrap = 1;
        wy = y0 - (-l + r);
    }
    if (wrap) {
        fn_80232BA4(text, font, x, wy, color, align, scale, a, 1.0f);
    }
    fn_80232BA4(text, font, x, y0, color, align, scale, a, 1.0f);
}

void fn_80230924(u32 color, f32 x0, f32 y0, f32 x1, f32 y1, f32 z) {
    fn_802C8B98(-1, fn_8022EDC4());
    u32 c = fn_80230B9C(color);
    UIVert v[4] __attribute__((aligned(16)));
    fn_80231380(&v[0], c, x0, y0, z, 0.0f, 0.0f);
    fn_80231380(&v[1], c, x1, y0, z, 0.0f, 0.0f);
    fn_80231380(&v[2], c, x0, y1, z, 0.0f, 0.0f);
    fn_80231380(&v[3], c, x1, y1, z, 0.0f, 0.0f);
    u16 idx[6] = {0, 1, 3, 0, 3, 2};
    fn_802C8DB8(v, 4, idx, 6);
    fn_802C8D74();
}

UIVert fn_80230A68(u32 color, f32 x, f32 y, f32 z, f32 u, f32 w) {
    UIVert r;
    r.x = x;
    r.y = y;
    r.z = z;
    r.color = color;
    r.u = u;
    r.v = w;
    return r;
}

void fn_80230AB8(u32 c, u8* a, u8* r, u8* g, u8* b) {
    *a = c >> 24;
    *r = c >> 16;
    *g = c >> 8;
    *b = c;
}

void fn_80230AD8(const void* m) {
    fn_802C87B4(0, 0, lbl_8055CFF0, lbl_8055CFF4);
    fn_802C88BC(3);
    fn_802C8530();
    fn_802C8638(m);
    fn_802C974C(1.0f, 1000.0f, 1.4398966f);
}

void fn_80230B38(void) {
    fn_802C855C();
    fn_802C2B48();
}

// Stripped from ET; only its constant remains.
f32 UIDrawStrippedPeriod(void) { return 2.0f; }

void fn_80230B5C(f32* m) {
    m[0] = 1.0f;
    m[3] = 0.0f;
    m[2] = 0.0f;
    m[1] = 0.0f;
    m[4] = 1.0f;
    m[7] = 0.0f;
    m[6] = 0.0f;
    m[5] = 0.0f;
    m[8] = 1.0f;
    m[9] = 0.0f;
    m[10] = 0.0f;
    m[11] = -365.0f;
}

static inline u32 RGBA(int a, int r, int g, int b) { return (a << 24) | (r << 16) | (g << 8) | b; }

u32 fn_80230B9C(u32 color) {
    u8 a = 0;
    u8 r = 0;
    u8 g = 0;
    u8 b = 0;
    fn_80230AB8(color, &a, &r, &g, &b);
    f32 k = fn_8022EDC4()->alpha;
    return RGBA((int)(a * k), r, g, b);
}

void fn_80230C30(f32* x, f32* y, int lx, int ly) {
    *x = lx - lbl_8055FB9C / 2;
    *y = lbl_8055FBA0 / 2 - ly;
}

void fn_80230C9C(f32* a, f32* b, f32 lo, f32 hi, f32 range) {
    f32 x0 = *a;
    f32 x1 = *b;
    if (x0 < lo && x1 < lo) {
        f32 m = (x0 > x1) ? x0 : x1;
        int n = (int)((lo - m) / range) + 1;
        x0 += n * range;
        x1 += n * range;
    } else if (x0 > hi && x1 > hi) {
        f32 m = (x0 < x1) ? x0 : x1;
        int n = (int)((m - hi) / range) + 1;
        x0 -= n * range;
        x1 -= n * range;
    }
    *a = x0;
    *b = x1;
}

// Stripped from ET; only its constant remains.
f32 UIDrawStrippedRamp(void) { return 0.25f; }

void fn_80230DA0(void) {}

void fn_80230DA4(f32 t, f32 period) {
    lbl_8055F030 = t;
    lbl_8055F02C = fmod(t, period);
    if (lbl_8055F02C < 0.0f) {
        lbl_8055F02C += period;
    }
    lbl_8055F02C = period - lbl_8055F02C;
}

f32 fn_80230DF4(f32 t, f32 period) {
    f32 m = fmod(t + lbl_8055F02C, period);
    if (m < 0.0f) {
        m += period;
    }
    m /= period;
    if (m < 0.5f) {
        m = 1.0f - m;
    }
    return m;
}

f32 fn_80230E54(f32 now, f32 period) {
    period = (period > 0.01f) ? period : 0.01f;
    f32 t = (now - lbl_8055F030) / period;
    if (t < 0.5f) {
        return t;
    }
    return fn_80230DF4(now, period);
}

void fn_80230E90(int real) {
    if (real == 1) {
        fn_80230DA4(fn_802E2C20() / 1000.0f, 2.0f);
    } else {
        fn_80230DA4(fn_8023659C(), 2.0f);
    }
}

f32 fn_80230EF4(int real) {
    if (!lbl_8055B998) {
        return 1.0f;
    }
    if (real == 1) {
        return fn_80230E54(fn_802E2C20() / 1000.0f, 2.0f);
    }
    return fn_80230E54(fn_8023659C(), 2.0f);
}

void fn_80230F6C(u32* color) { *color = 0xFFFFCA44; }

static inline f32 Clamp01(f32 t) {
    t = (1.0f < t) ? 1.0f : t;
    return (t > 0.0f) ? t : 0.0f;
}

static inline f32 Ramp(f32 x, f32 lo, f32 hi) { return Clamp01((x - lo) / (hi - lo)); }
static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

void fn_80230F78(u8* alpha, f32* grow, u32* color, int real) {
    f32 p = fn_80230EF4(real);
    int a = (int)Lerp(255.0f, 0.0f, Ramp(p, 0.0f, 0.25f));
    a = (a > 0) ? a : 0;
    *alpha = (a < 255) ? a : 255;
    *grow = Lerp(0.0f, 0.4f, Ramp(p, 0.0f, 0.25f));
    fn_80230F6C(color);
}

void fn_8023108C(u8 on) { lbl_8055B998 = on; }

static inline int Round(f32 x) { return (int)(x < 0.0f ? x - 0.5f : 0.5f + x); }

void fn_80231094(int* top, int* bottom, int* left, int* right) {
    f32 k = fn_802C9A68() ? 0.9f : 0.85f;
    f32 inv = 1.0f - k;
    int mx = (int)(0.5f + 0.5f * (640.0f * inv));
    int my = (int)(0.5f + 0.5f * (480.0f * inv));
    *left = mx;
    *right = mx;
    *top = my;
    *bottom = my;
    *left = 15;
    *right = 15;
    *top = 24;
    *bottom = 4;
    if (fn_802C9A60() || fn_802C9A58()) {
        *left -= Round(0.5f * (640.0f * (1.0f / lbl_8055D020 - 1.0f)));
        *right -= Round(0.5f * (640.0f * (1.0f / lbl_8055D020 - 1.0f)));
    }
}

void fn_80231220(f32 x, f32 y, f32 w, f32 h, s32 tex, u32 color, f32 u0, f32 v0, f32 u1, f32 v1) {
    fn_802C8B98(tex, fn_8022EDC4());
    u32 c = fn_80230B9C(color);
    f32 z = 0.0f;
    UIVert v[4] __attribute__((aligned(16)));
    fn_80231380(&v[0], c, x, y, z, u0, v0);
    fn_80231380(&v[1], c, x + w, y, z, u1, v0);
    fn_80231380(&v[2], c, x, y - h, z, u0, v1);
    fn_80231380(&v[3], c, x + w, y - h, z, u1, v1);
    u16 idx[6] = {0, 1, 3, 0, 3, 2};
    fn_802C8DB8(v, 4, idx, 6);
    fn_802C8D74();
}

void fn_80231380(UIVert* v, u32 color, f32 x, f32 y, f32 z, f32 u, f32 w) {
    v->x = x;
    v->y = y;
    v->z = z;
    v->color = color;
    v->u = u;
    v->v = w;
}

char* fn_8023139C(char* buf, int size, u32 value) {
    int n = size - 1;
    int digits = n - (n - 1) / 3;
    u32 max = 1;
    int i;
    for (i = 0; i < digits; i++) {
        max *= 10;
    }
    u32 lim = max - 1;
    if (value < lim) {
        lim = value;
    }
    char* p = buf + size;
    p[-1] = 0;
    value = lim;
    int group = 4;
    p -= 2;
    do {
        if (--group == 0) {
            *p-- = ',';
            group = 3;
        }
        *p-- = '0' + value % 10;
        value /= 10;
    } while (value);
    return p + 1;
}

}  // extern "C"
