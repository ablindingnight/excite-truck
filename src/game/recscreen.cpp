// Records screen (0x8025C028-0x8025CE78): the records/achievements layout ("records.lyt",
// "achievements", "tricks", "times", "track") with the car icon textures ("caricons0.tex", ...).
// Written against raw field offsets; the large screen routines are still stubs. Names are guesses
// apart from the strings.
#include "game/common.h"

#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

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

void fn_8025C054(u8* p) {}  // stub (108)

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

void fn_8025CC78(u8* p) {}  // stub (108)

}  // extern "C"
