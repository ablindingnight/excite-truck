// Image helpers (0x802C5130-0x802C5390): null-safe wrappers around the gx image module.
// Names are guesses.
#include "game/common.h"

extern "C" {

size_t strlen(const char* s);
void fn_802E30C8(const char* fmt, ...);  // debug print
void* fn_8023512C(const char* name);
void fn_80235380(void* img);
int fn_80235FD8(void* img);
f32 fn_80235EF0(void* img, int level);
f32 fn_80235F64(void* img, int level);
void fn_802353F4(void* img);
void fn_802354A0(void* img, int a, int b);
void fn_80235758(void* img, int a, int b, f32 f1, f32 f2, f32 f3, f32 f4, f32 f5, f32 f6, f32 f7, f32 f8);

}  // extern "C"

ModuleTag lbl_8055FC08;
UnkStatic7F lbl_8055FC10;
u8 lbl_8055FC14;

extern "C" {

void* fn_802C5130(const char* name) {
    if (strlen(name) > 15) {
        if (!lbl_8055FC14) {
            fn_802E30C8("image: gxGetImage(\"%s\"): Name is too long. Won't report again.", name);
        }
        lbl_8055FC14 = 1;
        return 0;
    }
    return fn_8023512C(name);
}

void fn_802C519C(void* img) {
    if (img) {
        fn_80235380(img);
    }
}

int fn_802C51AC(void* img) {
    if (img) {
        return fn_80235FD8(img);
    }
    return 1;
}

int fn_802C51C0(void* img) {
    if (img) {
        f32 f = fn_80235EF0(img, 0);
        return f < 0.0f ? f - 0.5f : 0.5f + f;
    }
    return 1;
}

int fn_802C5220(void* img) {
    if (img) {
        f32 f = fn_80235F64(img, 0);
        return f < 0.0f ? f - 0.5f : 0.5f + f;
    }
    return 1;
}

int fn_802C5280(void* img, int x, int y) {
    int r;
    if (img) {
        r = 0;
        if (x >= 0 && y >= 0 && x < fn_802C51C0(img) && y < fn_802C5220(img)) {
            r = 1;
        }
    } else {
        r = 0;
    }
    return r;
}

void fn_802C5310(void* img) {
    if (img) {
        fn_802353F4(img);
    }
}

void fn_802C5320(void* img, int a) {
    if (img) {
        fn_802354A0(img, a, -1);
    }
}

void fn_802C5334(void* img, int a, f32 f1, f32 f2, f32 unused, f32 f4, f32 f5, f32 f6, f32 f7, f32 f8, f32 f9) {
    if (img) {
        fn_80235758(img, a, -1, f1, f2, f4, f5, f6, f7, f8, f9);
    }
}

}  // extern "C"
