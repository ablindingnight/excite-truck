// The "swlmap" top-down map texture (0x802C26F0-0x802C2914): loads swlmap.tex, or makes a
// 512x512 render target to draw it into. Names are guesses.
#include "game/common.h"

extern "C" {

int fn_8012C33C(const char* name);
s32 fn_802CAC28(const char* name, int flags);  // load texture
int fn_802CB128(int w, int h, int a, int b);
void fn_802CB274(int rt);
void fn_802C9294(int rt, int unk);
void fn_802C8638(ValMatrix* cam);
void fn_802C87B4(int x, int y, int w, int h);
void fn_802C974C(f32 a, f32 b, f32 c);
void fn_802C86E8(u32 color);
void fn_802C88BC(int unk);
int fn_802C2C00(int on);
void fn_801D9B58(void);
void fn_802CB228(int rt);
void fn_802CAC54(s32 tex);

}  // extern "C"

extern const f32 lbl_80564EB0;

ModuleTag lbl_8055FB60;
UnkStaticFFFF lbl_8055FB68;
s32 lbl_8055FB6C;  // render target
s32 lbl_8055FB70;  // loaded texture
// The target computes this at run time; a const defined after it keeps MWCC from folding it.
// The target sinit also schedules its epilogue into the stores (see the handoff), so NonMatching.
f32 lbl_8055FB74 = 1024.0f * lbl_80564EB0;

const f32 lbl_80564EB0 = 2.0f;

extern "C" {

void fn_802C26F0(void) {
    const char* name = "swlmap.tex";
    s32 tex;
    if (fn_8012C33C(name)) {
        tex = fn_802CAC28(name, 0);
    } else {
        tex = -2;
    }
    lbl_8055FB70 = tex;
    lbl_8055FB6C = tex == -2 ? fn_802CB128(0x200, 0x200, 0, 1) : -2;
}

void fn_802C276C(void) {
    ValMatrix cam;
    if (lbl_8055FB70 == -2) {
        fn_802CB274(lbl_8055FB6C);
        fn_802C9294(lbl_8055FB6C, 0);
        cam.origin.x = 0.0f;
        cam.origin.y = 512.0f * (lbl_8055FB74 / 512.0f) / 2;
        cam.origin.z = 0.0f;
        cam.basis[0].x = 1.0f;
        cam.basis[0].y = 0.0f;
        cam.basis[0].z = 0.0f;
        cam.basis[1].x = 0.0f;
        cam.basis[1].y = 0.0f;
        cam.basis[1].z = -1.0f;
        cam.basis[2].x = 0.0f;
        cam.basis[2].y = -1.0f;
        cam.basis[2].z = 0.0f;
        fn_802C8638(&cam);
        fn_802C87B4(0, 0, 0x200, 0x200);
        fn_802C974C(1.0f, 100.0f + cam.origin.y, 1.5707964f);
        fn_802C86E8(0xFF00FF00);
        fn_802C88BC(-1);
        int old = fn_802C2C00(0);
        fn_801D9B58();
        fn_802C2C00(old);
        fn_802C9294(-1, 0);
    }
}

void fn_802C2874(void) {
    if (lbl_8055FB6C != -2) {
        fn_802CB228(lbl_8055FB6C);
        lbl_8055FB6C = -2;
    }
    if (lbl_8055FB70 != -2) {
        fn_802CAC54(lbl_8055FB70);
        lbl_8055FB70 = -2;
    }
}

s32 fn_802C28C0(void) {
    if (lbl_8055FB70 != -2) {
        return lbl_8055FB70;
    }
    return lbl_8055FB6C;
}

}  // extern "C"
