// Named cameras (0x80276068-0x8027820C): the named camera sets ("namedcam", "rddefault", "raceday",
// "house") and the timed camera transitions ("physcam: Clock looped during camera transition!!").
// Written against raw field offsets; the large routines and the initialiser are still stubs. Names
// are guesses apart from the strings.
#include "game/common.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

// A named camera setup ("namedcam" entries in the camera file).
struct NamedCam {
    u32 unk0;       // offset 0x0
    f32 pose[12];   // offset 0x4
    f32 fov;        // offset 0x34
    char name[16];  // offset 0x38
};

extern "C" {
int strcasecmp(const char* a, const char* b);
void* memcpy(void* d, const void* s, u32 n);
void fn_8026A4A0(int i, f32* pose, f32* fov);
}

extern u8 lbl_8049FD58[];
extern f32 lbl_8055E3C8;
extern NamedCam* lbl_8055F598;
extern NamedCam lbl_8049FE98;
extern NamedCam lbl_8049FEE0[];
extern s32 lbl_8055F59C;
extern s32 lbl_8055F5A0;

extern "C" {

int fn_80276068(int mode) {
    switch (mode) {
    case 4:
    case 5:
    case 6:
    case 9:
    case 23:
        return 1;
    }
    return 0;
}

void fn_80276098(f32* dst, f32* src, int a, int b, f32 from, f32 to, f32 t) {}  // stub (309)

void fn_8027656C(u8* p) {}  // stub (310)

NamedCam* fn_80276A44(const char* name) {
    NamedCam* c = lbl_8049FEE0;
    for (int i = 0; i < lbl_8055F5A0; i++) {
        if (!strcasecmp(c->name, name)) {
            return &lbl_8049FEE0[i];
        }
        c++;
    }
    return 0;
}

void fn_80276ACC(u8* p) {}  // stub (48)

void fn_80276B8C(void) {
    u8* e = lbl_8049FD58;
    for (int i = 0; i < 4; i++) {
        memset(e, 0, 0x50);
        e += 0x50;
    }
}

void fn_80276BE4(u8* p) {}  // stub (51)

void fn_80276CB0(const char* name, int blend) {
    NamedCam* c = fn_80276A44(name);
    if (!c) {
        c = lbl_8055F598;
    }
    if (blend) {
        fn_80276098(lbl_8049FE98.pose, c->pose, 0, 0x22, lbl_8049FE98.fov, c->fov, 1.0f);
    } else {
        memcpy(&lbl_8049FE98, c, 0x48);
        lbl_8049FD58[0] = 0;
    }
}

void fn_80276D3C(const char* name, int mode) {
    NamedCam* c = fn_80276A44(name);
    if (mode == 1) {
        f32 pose[12];
        f32 fov;
        fn_8026A4A0(0, pose, &fov);
        fn_80276098(pose, c->pose, 0, 0x22, fov, c->fov, 2.0f);
    } else {
        memcpy(&lbl_8049FE98, c, 0x48);
        lbl_8049FD58[0] = 0;
    }
}

void fn_80276DC8(u8* p) {}  // stub (591)

f32 fn_80277704(int i) {
    u8* e = lbl_8049FD58 + i * 0x50;
    if (e[0]) {
        return (lbl_8055E3C8 - F32(e, 0x48)) / F32(e, 0x44);
    }
    return -1.0f;
}

void fn_80277740(void) {
    lbl_8055F5A0 = 0;
    lbl_8055F598 = 0;
    lbl_8055F59C = 0;
}

void fn_80277754(u8* p) {}  // stub (53)

void fn_80277828(u8* p) {}  // stub (78)

void fn_80277960(u8* p) {}  // stub (45)

void fn_80277A14(u8* p) {}  // stub (348)

void fn_80277F84(u8* p) {}  // stub (121)

void fn_80278168(u8* p) {}  // stub (41)
}  // extern "C"
