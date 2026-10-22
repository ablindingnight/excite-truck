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

void fn_8025D9D0(u8* p) {}  // stub (46)

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

void fn_8025DDD0(u8* p) {}  // stub (108)

void fn_8025DF80(u8* p) {
    for (int i = 0; i < S32(p, 0x1C); i++) {
        fn_80214A9C(((s32*)PTR(p, 0x18))[i]);
    }
}

}  // extern "C"
