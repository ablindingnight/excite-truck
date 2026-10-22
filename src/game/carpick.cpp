// Car pick screen (0x8025E034-0x8025FE8C): the car icon grid ("caricona.art", "caricons0.tex", ...)
// with the chassis/uv fields. Written against raw field offsets; the large screen routines are still
// stubs. Names are guesses apart from the strings.
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

extern void* lbl_80345A30[];

ModuleTag lbl_8055F3C8;
UnkStatic123s lbl_80498EE8;
UnkStatic7F lbl_8055F3D0;

extern "C" {

void fn_8025E034(u8* p) {}  // stub (376)

u8* fn_8025E614(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_80345A30;
        fn_80235380(PTR(p, 0x2C));
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

void fn_8025E678(u8* p) {}  // stub (401)

void fn_8025ECBC(u8* p) {}  // stub (366)

void fn_8025F274(u8* p) {}  // stub (126)

void fn_8025F46C(u8* p) {}  // stub (339)

void fn_8025F9B8(u8* p, u32 v) { U32(p, 0x18) = v; }

u8* fn_8025F9C0(u8* p) {
    U32(p, 0x14) = 0;
    *(void**)p = lbl_80345A30;
    U32(p, 0x18) = 0;
    U8(p, 0x25) = 0;
    PTR(p, 0x2C) = fn_8023512C("caricona.art");
    return p;
}

void fn_8025FA18(u8* p) {}  // stub (157)

void fn_8025FC8C(u8* p) {}  // stub (108)

}  // extern "C"
