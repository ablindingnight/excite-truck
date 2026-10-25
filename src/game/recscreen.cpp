// Records screen (0x8025C028-0x8025CE78): the records/achievements layout ("records.lyt",
// "achievements", "tricks", "times", "track") with the car icon textures ("caricons0.tex", ...).
// Written against raw field offsets; the large screen routines are still stubs. Names are guesses
// apart from the strings.
#include "game/common.h"
#include "game/menu.h"

#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))

extern "C" {
s32 fn_80214D04(void);
void fn_80214CB4(s32 a);
void fn_80235380(u8* p);
}

void operator delete(void* p);

extern void* lbl_803453B8[];
extern s32 lbl_8055C390;

ModuleTag lbl_8055F3A8;
UnkStatic123s lbl_80498EB8;
UnkStatic7F lbl_8055F3B0;

extern "C" {

void fn_8025C028(void) { lbl_8055C390 = fn_80214D04(); }

void fn_8025C04C(void) { fn_80214CB4(lbl_8055C390); }

void fn_8025A50C(void);
void fn_8025CE78(void);
void fn_8025E034(void);
extern char lbl_8055B860[8];
extern char lbl_8055BAC0[4];  // ""
extern char lbl_8055BABC[4];  // ""

// Records menu: achievements, tricks or times until backed out.
void fn_8025C054(void) {
    MenuItem items[] = {
        MenuItem(lbl_8055BAC0, 8, (const void*)fn_8025C04C, 0, 0),
        MenuItem(lbl_8055BABC, 8, (const void*)fn_8025C028, 1, 0),
        MenuItem("achievements", 0, 0, 0, 0),
        MenuItem("tricks", 0, 0, 1, 0),
        MenuItem("times", 0, 0, 2, 0),
        MenuItem(lbl_8055B860, 6, 0, -1, 27),
        MenuItem(lbl_80498EB8),
    };
    MenuDesc desc = {"records.lyt", 0, 0, 0, 0};
    desc.items = items;
    int run = 1;
    while (run) {
        switch (fn_802146BC(&desc, -999, -999, 1)) {
        case 0:
            fn_8025A50C();
            break;
        case 1:
            fn_8025CE78();
            break;
        case 2:
            fn_8025E034();
            break;
        case -1:
        default:
            run = 0;
            break;
        }
    }
}

void fn_8025C204(u8* p) {}  // stub (644)

u8* fn_8025CC14(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_803453B8;
        fn_80235380(PTR(p, 0x28));
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

}  // extern "C"

// One car icon's texture rectangles.
struct IconEntry {
    u8 pad0[0x20];  // offset 0x0
    u8* tex;        // offset 0x20
    u8* overlay;    // offset 0x24
    f32 u0;         // offset 0x28
    f32 v0;         // offset 0x2C
    f32 u1;         // offset 0x30
    f32 v1;         // offset 0x34
};

// Scales each colour channel by 255*255/65536, alpha forced opaque.
static inline u32 DimColor(u32 c) {
    int b = (int)(c & 0xFF) * 255 * 255 / 256 / 256;
    int g = (int)((c >> 8) & 0xFF) * 255 * 255 / 256 / 256;
    int r = (int)((c >> 16) & 0xFF) * 255 * 255 / 256 / 256;
    return 0xFF000000 | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
}

extern "C" {
void fn_80230C30(f32* x, f32* y, s32 lx, s32 ly);
void fn_802354A0(u8* art, int part, f32 x, f32 y, f32 z, f32 w, f32 h, u32 color);
void fn_80231220(u8* tex, f32 x, f32 y, f32 w, f32 h, f32 u0, f32 v0, f32 u1, f32 v1, u32 color);

// The chosen car icon in a frame, the overlay tinted with the player's colour.
void fn_8025CC78(u8* p) {
    if (!U8(p, 0x20)) {
        return;
    }
    u32 c;
    IconEntry* e;
    f32 sx;
    f32 sy;
    f32 x;
    f32 y;
    f32 v1;
    f32 u1;
    f32 v0;
    f32 u0;
    f32 fw;
    f32 fh;
    fn_80230C30(&sx, &sy, S32(p, 0x4), S32(p, 0x8));
    fw = (f32)S32(p, 0xC);
    fh = (f32)S32(p, 0x10);
    fn_802354A0(PTR(p, 0x28), U8(p, 0x2C), sx - 3.0f, 3.0f + sy, 0.0f, 6.0f + fw, 6.0f + fh, -1);
    e = &((IconEntry*)*(u8**)PTR(p, 0x18))[S32(p, 0x1C)];
    y = sy;
    x = sx;
    u0 = e->u0;
    v0 = e->v0;
    u1 = e->u1;
    v1 = e->v1;
    c = U32(p, 0x24);
    fn_80231220(e->tex, x, y, fw, fh, u0, v0, u1, v1, -1);
    fn_80231220(e->overlay, x, y, fw, fh, u0, v0, u1, v1, DimColor(c));
}


}  // extern "C"
