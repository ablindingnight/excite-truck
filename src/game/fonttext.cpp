// Bitmap font text (0x801E4A2C-0x801E4E44): string widths, line heights and aligned, formatted
// drawing, with a debug-text fallback when no font is loaded. Names are guesses.
#include "game/common.h"
#include <stdarg.h>

extern "C" {
int vsprintf(char* buf, const char* fmt, va_list args);
char* strncpy(char* dst, const char* src, size_t n);
void fn_802C4C80(s32 tex);
void fn_802C4C90(s32 tex, int x, int y, int glyph, u32 color);
int fn_802C4FF8(const char* s);  // debug text width
int fn_802C501C(void);           // debug text height
void fn_802C4F2C(s32 tex, int* a, int* b);
int fn_802C4EFC(s32 tex);
void fn_802C4F58(int x, int y, const char* s, u32 color);  // debug text
}

extern u32 lbl_8055FBC0;  // debug text shadow color
extern u32 lbl_8055FBFC;  // debug text color

struct Font {
    ~Font(void) { fn_802C4C80(tex); }
    s32 tex;            // offset 0x0
    s16 glyph[256];     // offset 0x4
    s16 advance[256];   // offset 0x204
    s16 xOffset[256];   // offset 0x404
};

ModuleTag lbl_8055EA98;

extern "C" {

void fn_801E4A2C(Font* f) { delete f; }

int fn_801E4A6C(Font* f, const char* s) {
    if (f) {
        const u8* p = (const u8*)s;
        int w = 0;
        for (; *p; p++) {
            w += f->advance[*p];
        }
        return w;
    }
    return fn_802C4FF8(s);
}

int fn_801E4AB0(Font* f, const char* s, int n) {
    if (f) {
        const u8* p = (const u8*)s;
        int w = 0;
        for (; *p && n > 0; n--, p++) {
            w += f->advance[*p];
        }
        return w;
    }
    return fn_802C4FF8(s);
}

int fn_801E4B00(Font* f) {
    if (f) {
        int a;
        int b = 0;
        fn_802C4F2C(f->tex, &a, &b);
        return b;
    }
    return fn_802C501C();
}

int fn_801E4B48(Font* f) {
    if (f) {
        int a;
        int b = 0;
        fn_802C4F2C(f->tex, &a, &b);
        return fn_802C4EFC(f->tex) - b;
    }
    return 0;
}

int fn_801E4BA8(Font* f) { return fn_801E4B00(f) + fn_801E4B48(f); }

void fn_801E4BEC(Font* f, u32 color, u32 flags, int x, int y, const char* fmt, ...) {
    char buf[0x1000];
    va_list args;
    int w;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    w = fn_801E4A6C(f, buf);
    if (flags & 2) {
        x -= w;
    } else if (flags & 4) {
        x -= w / 2;
    }
    if (flags & 8) {
        y += fn_801E4B00(f);
    } else if (flags & 0x10) {
        y += (fn_801E4B00(f) - fn_801E4B48(f)) / 2;
    } else if (flags & 0x40) {
        y -= fn_801E4B48(f);
    }
    if (f) {
        const u8* p;
        for (p = (const u8*)buf; *p;) {
            int g = f->glyph[*p];
            if (g >= 0) {
                fn_802C4C90(f->tex, x + f->xOffset[*p], y, g, color);
            }
            x += f->advance[*p++];
        }
    } else {
        fn_802C4F58(x + 1, y + 1, buf, lbl_8055FBC0);
        fn_802C4F58(x, y, buf, lbl_8055FBFC);
    }
}

void fn_801E4DBC(Font* f, u32 color, u32 flags, int x, int y, const char* s, int n) {
    char buf[0x100];
    strncpy(buf, s, n);
    buf[n] = 0;
    fn_801E4BEC(f, color, flags, x, y, "%s", buf);
}

}  // extern "C"
