// Memory card status screens (0x802451DC-0x80246938): the save status layout ("mc_status.lyt"),
// the dialog and progress bar ("mc_dialog.lyt", "mem_bar.art", "progress") and the wireless warning.
// Written against raw field offsets; the large screen routines are still stubs. Names are guesses
// apart from the strings.
#include "game/common.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

class VM {
public:
    virtual void v08(void);
    virtual void v0C(void);
};

extern "C" {
u8 fn_8012C33C(const char* lyt);
void fn_80214D50(int s);
void fn_80214CB4(int a);
void fn_80214AF0(int s);
void fn_80214B44(int i);
void fn_80214A9C(int s);
void fn_80235380(u8* p);
void fn_802C519C(u8* p);
void fn_802328D4(u8* p);
}

void operator delete(void* p);

extern void* lbl_803430E8[];
extern u8* lbl_8055F244;

ModuleTag lbl_8055F248;
UnkStatic123s lbl_804988D8;

extern "C" {

u8* fn_802451DC(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

u8* fn_8024521C(u8* p, int del) {
    if (p) {
        lbl_8055F244 = 0;
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

void fn_80245264(u8* p) {}  // stub (32)

void fn_802452E4(u8* p) {
    if (PTR(p, 0x4)) {
        fn_802C519C(PTR(p, 0x4));
    }
    U32(p, 0x4) = 0;
    if (PTR(p, 0x8)) {
        fn_802C519C(PTR(p, 0x8));
    }
    U32(p, 0x8) = 0;
    fn_802328D4(PTR(p, 0xC));
    fn_802328D4(PTR(p, 0x10));
    U32(p, 0xC) = 0;
    U32(p, 0x10) = 0;
}

void fn_80245360(u8* p) {}  // stub (363)

void fn_8024590C(u8* p) {}  // stub (239)

void fn_80245CC8(u8* p) {
    U8(p, 0x1C) = 1;
    U8(p, 0x1D) = fn_8012C33C("mc_dialog.lyt");
    if (!U8(p, 0x1D)) {
        ((VM*)(p + 4))->v08();
        U8(p, 0x1E) = 1;
    }
}

void fn_80245D2C(u8* p) {
    U8(p, 0x1C) = 0;
    if (!U8(p, 0x1D)) {
        ((VM*)(p + 4))->v0C();
    }
    fn_80214D50(4);
}

void fn_80245D74(u8* p) {}  // stub (279)

u8* fn_802461D0(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_80246210(void) {
    U8(lbl_8055F244, 0x1E) = 0;
    s32 n = S32(PTR(lbl_8055F244, 0x20), 0x14);
    for (int i = 0; i < n; i++) {
        fn_80214AF0(i);
    }
    for (; n < 4; n++) {
        fn_80214B44(n);
        fn_80214A9C(n);
    }
    fn_80214CB4(S32(PTR(lbl_8055F244, 0x20), 0x10));
}

void fn_802462A0(u8* p) {}  // stub (98)

u8* fn_80246428(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_803430E8;
        if (PTR(p, 0x1C)) {
            fn_80235380(PTR(p, 0x1C));
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

void fn_80246498(u8* p) { fn_80214CB4(U8(p, 0x18) == 0); }

void fn_802464A8(u8* p) { U8(p, 0x19) = 0; }

void fn_802464B4(u8* p) {}  // stub (60)

void fn_802465A4(u8* p) {}  // stub (166)

void fn_8024683C(u8* p) {}  // stub (45)

}  // extern "C"
