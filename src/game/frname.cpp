// Friendly name screen (0x8025CE78-0x8025E034): the player name entry ("friendly_name") with the car
// icons ("caricona.art", "caricons0.tex", ...). Written against raw field offsets; the large screen
// routines are still stubs. Names are guesses apart from the strings.
#include "game/common.h"

#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

extern "C" {
void fn_80235380(u8* p);
void fn_80214A9C(s32 s);
u8* fn_8023512C(const char* name);
}

void operator delete(void* p);

extern void* lbl_80345718[];
extern char lbl_8055C3E0[1];

// Copies at most n characters, always terminating.
static inline void CopyName(const char* s, int n, char* d) {
    while ((*d++ = *s++) != 0) {
        if (--n <= 0) {
            d[-1] = 0;
            break;
        }
    }
}
extern s32 lbl_8055F3C4;

ModuleTag lbl_8055F3B8;
UnkStatic123s lbl_80498ED0;
UnkStatic7F lbl_8055F3C0;

extern "C" {

void fn_8025CE78(u8* p) {}  // stub (701)

u8* fn_8025D96C(u8* p, int del) {
    if (p) {
        u8* m = p + 0x18;
        lbl_8055F3C4 = 0;
        if (m) {
            if (m) {
                U32(m, 0x0) = 0;
                U32(m, 0x4) = 0;
                U32(m, 0x8) = 0;
            }
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

u8* fn_8025D9D0(u8* p) {
    U32(p, 0x420) = 0;
    *(void**)(p + 0x40C) = lbl_80345718;
    U32(p, 0x424) = 0;
    U8(p, 0x42C) = 0;
    PTR(p, 0x434) = fn_8023512C("caricona.art");
    PTR(p, 0x0) = (u8*)lbl_8055C3E0;
    CopyName(lbl_8055C3E0, 0x14, (char*)(p + 0x4));
    U32(p, 0x408) = 0;
    PTR(p, 0x1C) = (u8*)lbl_8055C3E0;
    PTR(p, 0x18) = (u8*)lbl_8055C3E0;
    U8(p, 0x20) = 0;
    U8(p, 0x42C) = 0;
    return p;
}

u8* fn_8025DA88(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_80345718;
        fn_80235380(PTR(p, 0x28));
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

void fn_8025DAEC(u8* p) {}  // stub (185)

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
void fn_8025DDD0(u8* p) {
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
    x = sx;
    y = sy;
    u0 = e->u0;
    v0 = e->v0;
    u1 = e->u1;
    v1 = e->v1;
    c = U32(p, 0x24);
    fn_80231220(e->tex, x, y, fw, fh, u0, v0, u1, v1, -1);
    fn_80231220(e->overlay, x, y, fw, fh, u0, v0, u1, v1, DimColor(c));
}

void fn_8025DF80(u8* p) {
    for (int i = 0; i < S32(p, 0x1C); i++) {
        fn_80214A9C(((s32*)PTR(p, 0x18))[i]);
    }
}

}  // extern "C"
