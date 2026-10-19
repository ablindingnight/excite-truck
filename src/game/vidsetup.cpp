// Video setup (0x802C2A48-0x802C2CAC): picks the video mode, brings up and tears down the
// graphics modules, and keeps the clear color. Names are guesses.
#include "game/common.h"

// A 16-bit value built by an inline constructor.
class UnkStaticS16 {
public:
    UnkStaticS16(s16 v) { val = v; }
    s16 val;
};

inline s16 MakeS16(s16 v) { return v; }

struct VidModeInfo {
    u32 width;
    u32 height;
    u32 bpp;
    u8 field;
    u8 pad[3];
    u32 x10;
};

struct ClearColor {
    f32 r;
    f32 g;
    f32 b;
    u8 on;
};

extern "C" {

VidModeInfo* fn_802CE344(int mode);
void fn_802CF5F8(u32 x);
void fn_802CA0E4(void);
void fn_801E2F00(void);
void fn_802C83CC(void);
void fn_802C990C(u32 color);
void fn_801E6978(void);
void fn_801E69EC(void);
void fn_802C84EC(void);
void fn_801E2F68(void);
void fn_802CA1E0(void);
void fn_802CF5FC(void);
void fn_802CFF10(void);
void fn_80127468(void);
void fn_801279B8(void);
void fn_802E4C18(int x);
void fn_802D0014(f32 a, f32 b, f32 c);
void fn_802DD63C(void);
void fn_802DD738(void);
void fn_802DD750(void);
void fn_802DE774(void);

}  // extern "C"

extern u32 lbl_8055CFF0;  // screen width
extern u32 lbl_8055CFF4;  // screen height
extern u8 lbl_8055CFF8;

ModuleTag lbl_8055FB88;
// Three shorts shared by many units (a common header). The target stores 0x401 before 0x405;
// no declaration form tried yet reproduces that order, so this unit stays NonMatching.
s16 lbl_8055FB90 = MakeS16(0x100);
s16 lbl_8055FB92 = MakeS16(0x405);
s16 lbl_8055FB94 = MakeS16(0x401);
u32 lbl_8055FB98;
s32 lbl_8055FB9C;
s32 lbl_8055FBA0;
s32 lbl_8055FBA4;
u8 lbl_8055FBA8;
ClearColor lbl_804C1328;

extern "C" {

void fn_802C2B78(f32 r, f32 g, f32 b);
u8 fn_802C2C00(u8 on);

void fn_802C2A48(s32 mode) {
    lbl_8055FBA4 = mode;
    VidModeInfo* info = fn_802CE344(mode);
    lbl_8055CFF0 = info->width;
    lbl_8055CFF4 = info->height;
    lbl_8055CFF8 = info->field;
    u32 stride = (info->bpp >> 3) * info->width;
    lbl_8055FB9C = 640;
    lbl_8055FBA0 = 480;
    lbl_8055FB98 = stride;
}

void fn_802C2AA8(void) {
    fn_802C2A48(2);
    fn_802CF5F8(fn_802CE344(2)->x10);
    fn_802CA0E4();
    lbl_8055FBA8 = 0;
    fn_801E2F00();
    fn_802C83CC();
    fn_802C990C(0xFF00FF00);
    fn_802C2B78(0.0f, 1.0f, 0.0f);
    fn_802C2C00(1);
    fn_801E6978();
}

void fn_802C2B18(void) {
    fn_801E69EC();
    fn_802C84EC();
    fn_801E2F68();
    fn_802CA1E0();
    fn_802CF5FC();
}

void fn_802C2B48(void) {
    fn_802CFF10();
    fn_80127468();
    fn_801279B8();
    fn_802E4C18(0);
}

inline void ApplyClear(double b) {
    fn_802D0014(lbl_804C1328.r, lbl_804C1328.g, lbl_804C1328.on ? (f32)b : 0.0f);
}

void fn_802C2B78(f32 r, f32 g, f32 b) {
    lbl_804C1328.r = r;
    lbl_804C1328.g = g;
    lbl_804C1328.b = b;
    ApplyClear(b);
}

void fn_802C2BB8(f32* r, f32* g, f32* b) {
    if (r) {
        *r = lbl_804C1328.r;
    }
    if (g) {
        *g = lbl_804C1328.g;
    }
    if (b) {
        *b = lbl_804C1328.b;
    }
}

u8 fn_802C2C00(u8 on) {
    u8 old = lbl_804C1328.on;
    lbl_804C1328.on = on;
    fn_802D0014(lbl_804C1328.r, lbl_804C1328.g, lbl_804C1328.on ? lbl_804C1328.b : 0.0f);
    return old;
}

void fn_802C2C60(void) { fn_802DD63C(); }

void fn_802C2C64(void) { fn_802DD738(); }

void fn_802C2C68(void) { fn_802DD750(); }

void fn_802C2C6C(void) { fn_802DE774(); }

}  // extern "C"
