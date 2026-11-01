// Image helpers (0x802C5130-0x802C5390): null-safe wrappers around the gx image module.
// Names are guesses.
#include "game/common.h"
#include "game/gart.h"
#include "game/image.h"

extern "C" {

size_t strlen(const char* s);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802354A0(gArtSet* img, int frame, f32 x, f32 y, f32 z, f32 width, f32 height, int color);
void fn_80235758(gArtSet* img, int frame, f32 x, f32 y, f32 width, f32 height,
                 f32 left, f32 top, f32 right, f32 bottom, int color);

}  // extern "C"

ModuleTag lbl_8055FC08;
UnkStatic7F lbl_8055FC10;
u8 lbl_8055FC14;

extern "C" {

void* gxGetImage(const char* name) {
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
        fn_80235380(static_cast<gArtSet*>(img));
    }
}

int fn_802C51AC(void* img) {
    if (img) {
        return fn_80235FD8(static_cast<gArtSet*>(img));
    }
    return 1;
}

int fn_802C51C0(void* img) {
    if (img) {
        f32 f = fn_80235EF0(static_cast<gArtSet*>(img), 0);
        return f < 0.0f ? f - 0.5f : 0.5f + f;
    }
    return 1;
}

int fn_802C5220(void* img) {
    if (img) {
        f32 f = fn_80235F64(static_cast<gArtSet*>(img), 0);
        return f < 0.0f ? f - 0.5f : 0.5f + f;
    }
    return 1;
}

int fn_802C5280(void* img, int x, int y, int) {
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

void fn_802C5310(void* img, int frame, f32 x, f32 y, f32 z) {
    if (img) {
        fn_802353F4(static_cast<gArtSet*>(img), frame, x, y, z);
    }
}

void fn_802C5320(void* img, int frame, f32 x, f32 y, f32 z, f32 width, f32 height) {
    if (img) {
        fn_802354A0(static_cast<gArtSet*>(img), frame, x, y, z, width, height, -1);
    }
}

void fn_802C5334(void* img, int frame, f32 x, f32 y, f32 unusedZ,
                 f32 width, f32 height, f32 left, f32 top, f32 right, f32 bottom) {
    if (img) {
        fn_80235758(static_cast<gArtSet*>(img), frame, x, y, width, height,
                     left, top, right, bottom, -1);
    }
}

}  // extern "C"
