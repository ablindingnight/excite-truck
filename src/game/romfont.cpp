// ROM font text (0x802DACD4-0x802DB2A0): loads the console's ROM font and draws boot-screen
// messages with it straight onto a cleared screen. Names are guesses apart from the messages.
#include "game/common.h"

extern "C" {
u16 OSGetFontEncode(void);
int OSInitFont(void* font);
const char* OSGetFontTexture(const char* str, void** texOut, u32* xOut, u32* yOut, u32* widthOut);
const char* OSGetFontWidth(const char* str, u32* widthOut);
void* memset(void* dst, int c, size_t n);
char* strcpy(char* dst, const char* src);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_801282EC(int align);
void* fn_801284E8(const char* tag, s32 size);  // MemAlloc
void fn_80128658(void* p);                     // MemFree
}

class UnkStaticS16 {
public:
    UnkStaticS16(s16 v) { val = v; }
    s16 val;
};

inline s16 MakeS16(s16 v) { return v; }

struct TexImage {
    u32 unk0;
    void* data;  // offset 0x4
    u8 pad8[0x18];
};

struct TexDesc {
    u32 format;  // offset 0x0
    u32 unk4;
    u8 pad8[0x18];
    u32 unk20;
    u32 width;   // offset 0x24
    u32 height;  // offset 0x28
    TexImage* image;  // offset 0x2C
    u8 pad30[0x4];
};

struct ScreenDesc {
    u8 format;  // offset 0x0
    u8 alpha;   // offset 0x1
    u8 pad2[2];
    u8* pixels;  // offset 0x4
    int width;   // offset 0x8
    int height;  // offset 0xC
    int stride;  // offset 0x10
    u8 pad14[0x8];
    int width2;   // offset 0x1C
    int height2;  // offset 0x20
};

extern "C" {
void* fn_802D8E9C(TexDesc* desc, const char* name);
void fn_802D8FB4(void* tex);
u8* fn_802D2F78(int* width, int* height);
void fn_802D2FD8(void);
void fn_802C3034(ScreenDesc* desc);
void fn_802C32B8(int x, int y, u32 color);
void fn_802CED50(int x, int y, int w, int h);
}

ModuleTag lbl_8055FE68;
s16 lbl_8055FE70 = MakeS16(0x100);
s16 lbl_8055FE72 = MakeS16(0x405);
s16 lbl_8055FE74 = MakeS16(0x401);
void* lbl_8055FE78;  // font data
void* lbl_8055FE7C;  // font sheet
void* lbl_8055FE80;  // font texture

static inline u8* ClearScreen(void) {
    int height;
    int width;
    width = 0;
    height = 0;
    u8* pixels = fn_802D2F78(&width, &height);
    int x;
    int y;
    u8* p;
    p = pixels;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            p[0] = 0x10;
            p[1] = 0x80;
            p += 2;
        }
    }
    ScreenDesc desc;
    memset(&desc, 0, sizeof(desc));
    desc.format = 4;
    desc.alpha = 0x80;
    desc.pixels = pixels;
    desc.width = width;
    desc.height = height;
    desc.stride = width * 2;
    desc.width2 = width;
    desc.height2 = height;
    fn_802C3034(&desc);
    return pixels;
}

static inline void Replace(char* s, char from, char to) {
    for (; *s; s++) {
        if (*s == from) {
            *s = to;
        }
    }
}

extern "C" {

void fn_802DACD4(void) {
    fn_801282EC(32);
    lbl_8055FE78 = fn_801284E8("OSFontData", OSGetFontEncode() == 0 ? 0x20120 : 0x120F00);
    int ok = OSInitFont(lbl_8055FE78);
    if (OSGetFontEncode() == 0x120F00) {
        fn_802E30C8("bootscrn: using SJIS font encoding");
    }
    if (!ok) {
        fn_802E30C8("dr: can't init ROM font!");
        fn_80128658(lbl_8055FE78);
        lbl_8055FE78 = NULL;
        return;
    }
    u32 width = 0;
    lbl_8055FE7C = NULL;
    u32 x = 0;
    u32 y = 0;
    OSGetFontTexture("A", &lbl_8055FE7C, &x, &y, &width);
    TexDesc desc;
    memset(&desc, 0, sizeof(desc));
    desc.unk4 = 1;
    desc.format = 0x48;
    desc.unk20 = 1;
    desc.width = 512;
    desc.height = 512;
    TexImage image;
    memset(&image, 0, sizeof(image));
    image.unk0 = 0;
    image.data = lbl_8055FE7C;
    desc.image = &image;
    lbl_8055FE80 = fn_802D8E9C(&desc, "rom_tex");
}

void fn_802DAE18(void) {
    if (lbl_8055FE78) {
        fn_802D8FB4(lbl_8055FE80);
        lbl_8055FE80 = NULL;
        fn_80128658(lbl_8055FE78);
        lbl_8055FE78 = NULL;
    }
}

// Draws str with the ROM font, wrapping words past x = 570; a negative x starts at -x.
void fn_802DAE64(int x, int y, const char* str) {
    int py;
    const char* s;
    u8* image;
    int endX;
    const char* p;
    int row;
    int col;
    int cy;
    int curX;
    int sheetX = 0;
    int sheetY = 0;
    int width = 0;
    void* sheet = NULL;
    cy = y;
    if (x >= 0) {
        curX = 70;
    } else {
        int m = x >> 31;
        curX = (m ^ x) - m;
    }
    for (s = str; *s; s++) {
        if (*s == ' ') {
            endX = curX;
            p = s + 1;
            while (*p && *p != ' ') {
                u32 w = 0;
                OSGetFontWidth(p, &w);
                p++;
                endX += w;
            }
            if (endX > 570) {
                curX = 70;
                cy += 32;
                continue;
            }
        }
        OSGetFontTexture(s, &sheet, (u32*)&sheetX, (u32*)&sheetY, (u32*)&width);
        image = (u8*)sheet;
        for (row = 0; row < 24; row++) {
            py = row + cy;
            for (col = 0; col < width; col++) {
                int sy = row + sheetY;
                int sx = col + sheetX;
                int tile = sx / 8 + (sy / 8 * 512) / 8;
                int texel = tile * 64 + sy % 8 * 8 + sx % 8;
                u8 b = image[texel / 2];
                u8 level = !(col & 1) ? ((b >> 4) & 0xF) : (b & 0xF);
                if (level) {
                    u16 c = level * 12 + 16;
                    u16 ia = (c << 8) | 0x80;
                    fn_802C32B8(col + curX, py, (ia << 16) | ia);
                }
            }
        }
        curX += width;
    }
}

void fn_802DB03C(const char* msg) {
    char text[1024];
    ClearScreen();
    strcpy(text, msg);
    Replace(text, '\n', ' ');
    fn_802DAE64(320, 120, text);
    fn_802D2FD8();
}

void fn_802DB168(void) {
    ClearScreen();
    // From the boot-screen functions the linker stripped.
    static const char* unused[] = {
        "%s %d %d",
        "%d:%d:%d",
        "%04d/%02d/%02d   %02d:%02d:%02d",
        "Do you want to use interlaced mode?",
        "drDoBootScreen",
        "version check...",
        "VERSION CHECK",
        "Progressive",
        "Interlaced",
        "bootscrn: has DTV cable (old mode %s)...",
        "dr: interlaced dialog requested!",
        "bootscrn: switching to progressive mode",
        "Screen has been set to progressive mode",
        "bootscrn: switching to interlaced mode",
        "Screen has been set to interlaced mode",
    };
    fn_802DAE64(-450, 380, "LOADING");
    fn_802D2FD8();
    fn_802CED50(450, 380, 120, 30);
}

}  // extern "C"

// NonMatching: a few volatile registers in the glyph address math differ, and the sinit is one
// of the epilogue-scheduled kind.
