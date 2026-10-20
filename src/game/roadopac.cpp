// Road opacity map (0x802C2074-0x802C2298): the "roadopac.tex" texture (falling back to "df_rdopc.tex")
// and, when an editable copy exists, writing grey values into it at world positions (mapped to texels around
// the centre) and saving it as "roadopac.tga". Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
int fn_8012C33C(const char* name);         // resource exists
s32 fn_802CAC28(const char* name, int a);  // texture handle
void fn_802CAC54(s32 tex);
void* fn_802CA294(s32 handle);
void fn_802D9628(void* tex, int x, int y, u32 color);
}

// An editable 32-bit image.
struct OpacImage {
    s32 w;      // offset 0x0
    s32 h;      // offset 0x4
    u32* data;  // offset 0x8
};

extern "C" {
void fn_802B52B4(OpacImage* img);
void fn_802B52C0(const char* name, OpacImage* img);
}

static inline void Plot(OpacImage* im, int x, int y, u32 c) { im->data[x + (im->h - 1 - y) * im->w] = c; }

ModuleTag lbl_8055FB48;
s32 lbl_8055FB50;
OpacImage* lbl_8055FB54;

extern "C" {

s32 fn_802C2074(const char* name) {
    int ok = 0;
    if (name && fn_8012C33C(name)) {
        ok = 1;
    }
    if (ok) {
        return fn_802CAC28(name, 0);
    }
    return -2;
}

void fn_802C20DC(s32* tex) {
    if (*tex != -2) {
        fn_802CAC54(*tex);
        *tex = -2;
    }
}

s16 fn_802C2120(int x) { return (x << 8) / 1024 + 0x80; }

void fn_802C2138(void) {
    lbl_8055FB50 = fn_802C2074("roadopac.tex");
    if (lbl_8055FB50 == -2) {
        lbl_8055FB50 = fn_802C2074("df_rdopc.tex");
    }
}

void fn_802C217C(void) {
    fn_802C20DC(&lbl_8055FB50);
    if (lbl_8055FB54) {
        fn_802B52B4(lbl_8055FB54);
        lbl_8055FB54 = 0;
    }
}

s32 fn_802C21B8(void) { return lbl_8055FB50; }

void fn_802C21C0(void) {
    if (lbl_8055FB54) {
        fn_802B52C0("roadopac.tga", lbl_8055FB54);
    }
}

void fn_802C21DC(int x, int z, u32 v) {
    if (lbl_8055FB54) {
        u32 c = ((v & 0xFF) | ((v & 0xFF) << 8)) | ((v << 24) | ((v & 0xFF) << 16));
        s16 px = fn_802C2120(x);
        s16 pz = fn_802C2120(z);
        OpacImage* im = lbl_8055FB54;
        int row = im->h - 1 - pz;
        im->data[px + row * im->w] = c;
        fn_802D9628(fn_802CA294(lbl_8055FB50), px, pz, c);
    }
}

}  // extern "C"
