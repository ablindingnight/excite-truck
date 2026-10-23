// Records screen (0x8025C028-0x8025CE78): the records/achievements layout ("records.lyt",
// "achievements", "tricks", "times", "track") with the car icon textures ("caricons0.tex", ...).
// Written against raw field offsets; the large screen routines are still stubs. Names are guesses
// apart from the strings.
#include "game/common.h"
#include "game/menu.h"

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

void fn_8025CC78(u8* p) {}  // stub (108)

}  // extern "C"
