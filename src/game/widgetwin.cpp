// Widget windows (0x80216054-0x80216D14, "WidgetWindow"): the table of up to 16 open menu windows,
// the pointer position in 640x480 screen space and its button dispatch to the active window,
// drawing every window with the pointer on top, and the modal run loop that fades a window in,
// runs it until it reports a result and fades it out again. Names are guesses apart from the strings.
#include "game/common.h"

void* operator new(size_t size, const char* tag);
void OptE3Demo(void);

struct VecP {
    f32 x, y, z;
};

struct UIXform {
    f32 m[3][3];  // offset 0x0
    f32 pos[3];   // offset 0x24
};

// Fade timing a window reports for its open and close.
struct WinFade {
    f32 in;   // offset 0x0
    f32 out;  // offset 0x4
};

struct PadPos {
    s32 button;  // offset 0x0
    s32 x;       // offset 0x4
    s32 y;       // offset 0x8
    s32 fC;      // offset 0xC
    s32 f10;     // offset 0x10
    s32 f14;     // offset 0x14
};

class Window;

class Window {
public:
    Window(const char* bg, s32 x, s32 y, s32 w, s32 h, s32 f);
    ~Window(void);

    u8 pad0[0x434];
    void (*update)(Window* w);  // offset 0x434
    u8 pad438[0x454 - 0x438];
    s32 x;                      // offset 0x454
    s32 y;                      // offset 0x458
    u8 pad45C[0xDE4 - 0x45C];
    u8 noSound;                 // offset 0xDE4
    u8 padDE5[0xDE8 - 0xDE5];
};

struct WinRect {
    u8 pad0[4];
    s32 x;  // offset 0x4
    s32 y;  // offset 0x8
    s32 w;  // offset 0xC
    s32 h;  // offset 0x10
};

extern "C" {
void fn_802C3034(int color);
void fn_802C3424(s32 x0, s32 y0, s32 x1, s32 y1, s32 a);
Window* fn_8021832C(Window* w);
void fn_802C974C(f32 nearPlane, f32 farPlane, f32 fov);
void fn_80230B5C(UIXform* m);
void fn_802C8638(UIXform* m);
void fn_802C5310(Window* w, int a, f32 x, f32 y, f32 z);
void fn_802166B8(void);
void fn_80218464(Window* w);
void fn_802184D0(Window* w);
void fn_802C86E8(u32 color);
void fn_802C87B4(s32 x, s32 y, s32 w, s32 h);
void fn_802C88BC(int a);
void fn_8023108C(int active);
void fn_80218574(Window* w);
int fn_80127A50(PadPos* p);
void fn_80217590(Window* w, s32 x, s32 y);
void fn_802176D4(Window* w, s32 x, s32 y);
void fn_8021749C(Window* w, s32 x, s32 y);
void fn_802175F0(Window* w, s32 x, s32 y);
void fn_80217734(Window* w, s32 x, s32 y);
void fn_80217834(Window* w, s32 x, s32 y);
void fn_80217854(Window* w, s32 x, s32 y);
int fn_802E47B8(void);
u32 fn_802E46F0(void);
void fn_80217874(Window* w, u16 key);
void fn_80211F64(void);
void fn_80129AFC(void);
void fn_80218370(Window* w);
void fn_802E97B4(void);
void fn_802C8530(void);
void fn_80216244(void);
void fn_802160AC(void);
int fn_80211F54(void);
void fn_80230DA0(void);
void fn_802C855C(void);
void fn_802C2B48(void);
void fn_802E317C(const char* fmt, ...);
int fn_8021889C(Window* w);
int fn_80218968(Window* w);
int fn_8021885C(Window* w);
int fn_80218A34(Window* w);
int fn_80218ADC(Window* w);
int fn_80218B88(Window* w);
int fn_80218C98(Window* w);
int fn_8012742C(void);
void fn_80214D50(int sound);
void fn_8023660C(void);
void fn_802D3E9C(int a);
void fn_80127A84(s32* x, s32* y, s32* b);
void fn_80236780(void);
void fn_8021734C(Window* w);
void fn_8021747C(Window* w, int phase, WinFade* fade, f32 t);
void fn_801282E8(void);
void fn_80236674(void);
void fn_80230E90(int a);
void fn_8021662C(void);
f32 fn_80214D48(void);
void fn_80236900(void);
void fn_802368D0(void);
void fn_80216194(void);
void fn_802173F8(Window* w);
void fn_80217478(Window* w);
void fn_80236848(void);
void fn_80236600(void);
double fabs(double x);
extern s32 lbl_8055CFF0;
extern s32 lbl_8055CFF4;
extern s32 lbl_8055FBDC;
extern u8 lbl_8055F024;
}

extern f32 lbl_80558DC4[];  // FLT_EPSILON

ModuleTag lbl_8055EF60;
UnkStaticFFFF lbl_8055EF68;
static UnkStatic123 lbl_804908B0;
UnkStatic7F lbl_8055EF6C;
static Window* lbl_804908C8[16];
u8 lbl_8055EF70;   // the active window reported a result
s32 lbl_8055EF74;  // that result
s32 lbl_8055EF78;  // run depth
Window* lbl_8055EF7C;
s32 lbl_8055EF80;  // pointer x
s32 lbl_8055EF84;  // pointer y
Window* lbl_8055EF88;  // the active window
void (*lbl_8055EF8C)(void);
void (*lbl_8055EF90)(void);
f32 lbl_8055EF94;  // fade
u8 lbl_8055EF98;
u8 lbl_8055EF99;

static inline void XformVec(const UIXform* m, const VecP* in, VecP* out) {
    if (in == out) {
        VecP t;
        t.x = in->z * m->m[2][0] + (in->x * m->m[0][0] + in->y * m->m[1][0]);
        t.y = in->z * m->m[2][1] + (in->x * m->m[0][1] + in->y * m->m[1][1]);
        t.z = in->z * m->m[2][2] + (in->x * m->m[0][2] + in->y * m->m[1][2]);
        *out = t;
    } else {
        out->x = in->z * m->m[2][0] + (in->x * m->m[0][0] + in->y * m->m[1][0]);
        out->y = in->z * m->m[2][1] + (in->x * m->m[0][1] + in->y * m->m[1][1]);
        out->z = in->z * m->m[2][2] + (in->x * m->m[0][2] + in->y * m->m[1][2]);
    }
}

static inline f32 FadeMax(f32 a, f32 b) { return a > b ? a : b; }
static inline f32 FadeMin(f32 a, f32 b) { return a < b ? a : b; }

extern "C" {

int fn_80216054(void) { return 1; }

void fn_8021605C(WinRect* r, int color) {
    fn_802C3034(color);
    fn_802C3424(r->x, r->y, r->x + r->w, r->y + r->h, lbl_8055FBDC);
}

void fn_802160AC(void) {
    Window* w = lbl_8055EF88 ? fn_8021832C(lbl_8055EF88) : 0;
    if (w || lbl_8055EF7C) {
        UIXform m;
        f32 x;
        f32 y;
        fn_802C974C(1.0f, 1000.0f, 1.4399f);
        fn_80230B5C(&m);
        fn_802C8638(&m);
        x = (f32)lbl_8055EF80 - 320.0f;
        y = 240.0f - (f32)lbl_8055EF84;
        if (w) {
            fn_802C5310(w, 0, x, y, 0.0f);
        } else {
            fn_802C5310(lbl_8055EF7C, 0, x, y, 0.0f);
        }
    }
}

void fn_80216194(void) {
    int i = 1;
    do {
        fn_802166B8();
    } while (i++ < 3);
}

void fn_802161CC(int all) {
    int i;
    for (i = 0; i < 16; i++) {
        if (lbl_804908C8[i] && (all || lbl_804908C8[i] != lbl_8055EF88)) {
            fn_80218464(lbl_804908C8[i]);
        }
    }
}

void fn_80216244(void) {
    int i;
    for (i = 0; i < 16; i++) {
        if (lbl_804908C8[i]) {
            fn_802184D0(lbl_804908C8[i]);
        }
    }
    if (lbl_8055EF8C) {
        lbl_8055EF8C();
    }
    fn_802C86E8(0xFF0000FF);
    fn_802C87B4(0, 0, lbl_8055CFF0, lbl_8055CFF4);
    fn_802C88BC(2);
    for (i = 0; i < 16; i++) {
        Window* w = lbl_804908C8[i];
        if (w) {
            UIXform m;
            VecP v;
            VecP t;
            fn_802C87B4(0, 0, lbl_8055CFF0, lbl_8055CFF4);
            fn_80230B5C(&m);
            fn_802C974C(1.0f, 1000.0f, 1.4399f);
            v.x = -(f32)w->x;
            v.y = (f32)w->y;
            v.z = 0.0f;
            XformVec(&m, &v, &t);
            m.pos[0] += t.x;
            m.pos[1] += t.y;
            m.pos[2] += t.z;
            fn_802C8638(&m);
            fn_8023108C(w == lbl_8055EF88);
            fn_80218574(w);
        }
    }
    fn_8023108C(1);
}

void fn_802164D8(void) {
    PadPos p;
    if (fn_80127A50(&p)) {
        lbl_8055EF80 = (s32)(640.0f * (f32)p.x / (f32)lbl_8055CFF0);
        lbl_8055EF84 = (s32)(480.0f * (f32)p.y / (f32)lbl_8055CFF4);
        if (lbl_8055EF88) {
            switch (p.button) {
            case 1:
                fn_80217590(lbl_8055EF88, lbl_8055EF80, lbl_8055EF84);
                break;
            case 3:
                fn_802176D4(lbl_8055EF88, lbl_8055EF80, lbl_8055EF84);
                break;
            case 0:
                fn_8021749C(lbl_8055EF88, lbl_8055EF80, lbl_8055EF84);
                break;
            case 2:
                fn_802175F0(lbl_8055EF88, lbl_8055EF80, lbl_8055EF84);
                break;
            case 8:
                fn_80217734(lbl_8055EF88, lbl_8055EF80, lbl_8055EF84);
                break;
            case 6:
                fn_80217834(lbl_8055EF88, lbl_8055EF80, lbl_8055EF84);
                break;
            case 7:
                fn_80217854(lbl_8055EF88, lbl_8055EF80, lbl_8055EF84);
                break;
            }
        }
    }
    if (fn_802E47B8()) {
        u32 key = fn_802E46F0();
        if (lbl_8055EF88) {
            fn_80217874(lbl_8055EF88, key);
        }
    }
}

void fn_8021662C(void) {
    s32 depth = lbl_8055EF78;
    fn_80211F64();
    if (lbl_8055EF90) {
        lbl_8055EF90();
    }
    fn_802164D8();
    if (lbl_8055EF88) {
        fn_80129AFC();
    }
    fn_802161CC(0);
    if (lbl_8055EF88) {
        fn_80218370(lbl_8055EF88);
    }
    if (depth == lbl_8055EF78) {
        if (!lbl_8055EF70) {
            fn_802166B8();
        }
        fn_802E97B4();
    }
}

void fn_802166B8(void) {
    fn_802C8530();
    fn_80216244();
    fn_802160AC();
    if (!fn_80211F54()) {
        OptE3Demo();
    }
    if (lbl_8055F024) {
        fn_80230DA0();
    }
    fn_802C855C();
    fn_802C2B48();
}

u8* fn_80216708(void) { return &lbl_8055EF98; }

void fn_80216710(void) {
    int i;
    lbl_8055EF88 = 0;
    for (i = 0; i < 16; i++) {
        lbl_804908C8[i] = 0;
    }
    lbl_8055EF7C = 0;
    lbl_8055EF8C = 0;
    lbl_8055EF90 = 0;
}

void fn_8021676C(void) {}

Window* fn_80216770(const char* a, s32 b, s32 c, s32 d, s32 e, s32 f) {
    Window* w = new ("WidgetWindow") Window(a, b, c, d, e, f);
    int i;
    for (i = 0; i < 16; i++) {
        if (!lbl_804908C8[i]) {
            lbl_804908C8[i] = w;
            return w;
        }
    }
    fn_802E317C("Can't allocate another window (increase MAX_WINDOWS)");
    return 0;
}

void fn_8021683C(Window* w) {
    int i;
    for (i = 0; i < 16; i++) {
        if (lbl_804908C8[i] == w) {
            delete w;
            lbl_804908C8[i] = 0;
            return;
        }
    }
    fn_802E317C("Deleting unmanaged window");
}

void fn_802168BC(s32 result) {
    lbl_8055EF74 = result;
    lbl_8055EF70 = 1;
}

int fn_802168CC(Window* w) { return fn_8021889C(w); }

int fn_802168D0(Window* w) { return fn_80218968(w); }

int fn_802168D4(Window* w) { return fn_8021885C(w); }

int fn_802168D8(Window* w) { return fn_80218A34(w); }

int fn_802168DC(Window* w) { return fn_80218ADC(w); }

int fn_802168E0(Window* w) { return fn_80218B88(w); }

Window* fn_802168E4(void) { return lbl_8055EF88; }

int fn_802168EC(Window* w) { return fn_80218C98(w); }

s32 fn_802168F0(Window* w) {
    Window* prev;
    BOOL started;
    f32 one;
    f32 rate;
    f32 t;
    f32 rateOut;
    f32 zero;
    f32 tOut;
    s32 b;
    WinFade fade;
    lbl_8055EF78++;
    if (fn_8012742C()) {
        return -1;
    }
    if (w->noSound) {
        fn_80214D50(5);
    } else {
        fn_8023660C();
    }
    fn_802D3E9C(0);
    lbl_8055EF98 = 1;
    fn_80127A84(&lbl_8055EF80, &lbl_8055EF84, &b);
    fade.in = 0.0f;
    prev = lbl_8055EF88;
    lbl_8055EF88 = w;
    fade.out = 0.0f;
    fn_80236780();
    fn_8021734C(w);
    fn_8021747C(w, 0, &fade, 0.0f);
    fn_801282E8();
    lbl_8055EF70 = 0;
    if (prev) {
        lbl_8055EF74 = 0;
    }
    fn_80217734(lbl_8055EF88, lbl_8055EF80, lbl_8055EF84);
    fn_80211F64();
    fn_80211F64();
    t = 1.0f;
    rate = t / FadeMax(fade.in, 0.01f);
    t = (f32)__fabs(fade.in) <= lbl_80558DC4[0] ? t : 0.0f;
    one = 1.0f;
    started = 0;
    lbl_8055EF94 = t;
    while (!lbl_8055EF70) {
        fn_80236674();
        if (lbl_8055EF88) {
            if (lbl_8055EF88->update) {
                lbl_8055EF88->update(lbl_8055EF88);
            }
            if (t < one) {
                fn_8021747C(lbl_8055EF88, 1, &fade, t);
            } else {
                if (!started) {
                    started = 1;
                    fn_80230E90(0);
                }
                fn_8021747C(lbl_8055EF88, 2, &fade, 1.0f);
            }
        }
        if (t < 0.65f) {
            while (fn_802E47B8()) {
                fn_802E46F0();
            }
        }
        fn_8021662C();
        t += rate * fn_80214D48();
        lbl_8055EF94 = FadeMin(t, one);
        if (fn_8012742C()) {
            lbl_8055EF74 = -1;
            break;
        }
    }
    fn_8012742C();
    if (lbl_8055EF74 == -1) {
        fn_80236900();
    } else {
        fn_802368D0();
    }
    fn_8021747C(w, 0, &fade, 0.0f);
    tOut = 1.0f;
    rateOut = tOut / FadeMax(fade.out, 0.01f);
    if ((f32)__fabs(fade.out) <= lbl_80558DC4[0]) {
        tOut = 0.0f;
    }
    lbl_8055EF94 = tOut;
    if (!lbl_8055EF99) {
        fn_80214D50(4);
    } else {
        lbl_8055EF99 = 0;
    }
    zero = 0.0f;
    for (;;) {
        if (zero > tOut) {
            tOut = zero;
        }
        lbl_8055EF94 = tOut;
        if (fn_8012742C()) {
            lbl_8055EF74 = -1;
            break;
        }
        if (fade.out > zero) {
            fn_80236674();
            if (lbl_8055EF88) {
                fn_8021747C(lbl_8055EF88, 3, &fade, tOut);
            }
        }
        fn_802161CC(1);
        fn_802166B8();
        if (zero == tOut) {
            break;
        }
        tOut -= rateOut * fn_80214D48();
        fn_80211F64();
    }
    fn_80216194();
    lbl_8055EF70 = 0;
    fn_802173F8(w);
    lbl_8055EF88 = prev;
    if (prev) {
        fn_80217734(prev, lbl_8055EF80, lbl_8055EF84);
        if (!w->noSound) {
            fn_80217478(lbl_8055EF88);
        }
    }
    fn_80236848();
    if (!w->noSound) {
        fn_80236600();
    }
    return lbl_8055EF74;
}

void fn_80216CA8(void) { lbl_8055EF99 = 1; }

f32 fn_80216CB4(void) { return lbl_8055EF94; }
}
