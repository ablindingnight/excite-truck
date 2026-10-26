// UI text overlay (0x8023875C-0x80238BF4): screen-space text and rectangles in pixel coordinates,
// drawn through the SWF text path when available, else with the uifnt bitmap font. Names are guesses.
#include "game/common.h"
#include <stdarg.h>

struct Camera {
    u8 data[0x2C];
    f32 dist;  // offset 0x2C
};

extern "C" {
size_t strlen(const char* s);
int vsprintf(char* buf, const char* fmt, va_list args);
int fn_8012C33C(const char* name);
int fn_802344B0(void);
void fn_802344C4(int on);
void fn_802344CC(const char* text, u32 color, int flags, f32 x, f32 y, f32 scale, int unk);
void fn_802346C8(void);
void fn_80232BA4(const char* text, int font, f32 x, f32 y, u32 color, int flags, f32 scale, f32 z, f32 alpha);
s32 fn_80232578(const char* name);  // load font
void fn_802328D4(s32 font);
int fn_80232B70(int font);
void fn_80230B5C(Camera* cam);
void fn_802C8638(Camera* cam);
void fn_802C974C(f32 a, f32 b, f32 c);
void fn_802C8BD0(s32 tex);
void fn_802C92DC(int x0, int y0, int x1, int y1, u32 color);
void fn_802C8D74(void);
}

extern s32 lbl_8055CFF0;  // screen width
extern s32 lbl_8055CFF4;  // screen height
extern f32 lbl_8055FC88;

ModuleTag lbl_8055F0C8;
UnkStatic7F lbl_8055F0D0;
s32 lbl_8055F0D4;  // font
u8 lbl_8055F0D8;
u8 lbl_8055F0D9;

extern "C" {

void fn_8023875C(int x, int y, const char* text, u32 color, int flags) {
    if (fn_802344B0() && strlen(text) + 1 < 64) {
        fn_802344CC(text, color, flags, x - lbl_8055CFF0 / 2, lbl_8055CFF4 / 2 - y, 0.4f, 0);
    } else {
        fn_80232BA4(text, lbl_8055F0D4, x - lbl_8055CFF0 / 2, lbl_8055CFF4 / 2 - y, color, flags, 0.4f, 0.0f, 1.0f);
    }
}

void fn_8023889C(void) {
    u32 i;
    lbl_8055F0D9 = 1;
    const char* names[2] = {"uifnt.gtx", "uifnt_o.gtx"};
    for (i = 0; i < 2; i++) {
        if (fn_8012C33C(names[i])) {
            lbl_8055F0D4 = fn_80232578(names[i]);
            break;
        }
    }
}

void fn_80238920(void) {
    lbl_8055F0D9 = 0;
    if (lbl_8055F0D4) {
        fn_802328D4(lbl_8055F0D4);
    }
    lbl_8055F0D4 = 0;
}

void fn_8023895C(void) {
    Camera cam;
    fn_802C974C(1.0f, 1000.0f, 1.4398966f);
    fn_80230B5C(&cam);
    cam.dist = -(lbl_8055CFF0 / 2) / lbl_8055FC88;
    fn_802C8638(&cam);
    lbl_8055F0D8 = 1;
    fn_802344C4(1);
}

void fn_802389E4(void) {
    if (fn_802344B0()) {
        fn_802346C8();
    }
    lbl_8055F0D8 = 0;
    fn_802344C4(0);
}

int fn_80238A20(void) { return 0.4f * fn_80232B70(lbl_8055F0D4); }

void fn_80238A74(int x, int y, const char* text, u32 color) { fn_8023875C(x, y - 3, text, color, 9); }

void fn_80238A80(int x, int y, u32 color, const char* fmt, ...) {
    char buf[0x100];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    fn_80238A74(x, y, buf, color);
}

void fn_80238B44(int x0, int y0, int x1, int y1, u32 color) {
    fn_802C8BD0(-1);
    fn_802C92DC(x0 - lbl_8055CFF0 / 2, lbl_8055CFF4 / 2 - y0, x1 - lbl_8055CFF0 / 2, lbl_8055CFF4 / 2 - y1,
                color);
    fn_802C8D74();
}

}  // extern "C"
