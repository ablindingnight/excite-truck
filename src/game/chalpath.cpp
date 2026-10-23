// Challenge event path screen (0x8025FE8C-0x80262F8C): the challenge map
// ("EventPath:CHALLENGE_DIFFICULTY_%d_UNLOCKED", "EventPath:CONGRATULATIONS"), its title/message
// popup, the "%s/%s" progress counters and the list descriptors behind it. Written against raw field
// offsets; the large screen routines are still stubs. Names are guesses apart from the strings.
#include "game/common.h"
#include "game/menu.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

class VPage {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(void);
    virtual void v1C(void);
    virtual u8* v20(void);
    virtual s32 v24(void);
};

struct Words3 {
    u32 w[3];
};

struct Words5 {
    u32 w[5];
};

extern "C" {
void fn_80236914(void);
void fn_80214A9C(int s);
void fn_80230E90(int a);
void fn_8025C204(u8* p, s32 a);
void fn_80235380(u8* p);
void fn_802328D4(u8* p);
void fn_80214AF0(int s);
}

void operator delete(void* p);

extern const Words3 lbl_802FA6A8;
extern const Words3 lbl_802FA6B4;
extern u8 lbl_8055B860[8];
extern u8 lbl_8055BAB8[4];
extern u8 lbl_8055BAC0[8];
extern bool lbl_8055F3E9;
extern u8 lbl_8055F3EC;
extern u8 lbl_8055F3EA;
extern u8 lbl_8055F3EB;
extern void* lbl_80345E28[];
extern void* lbl_80345EA0[];
extern u8* lbl_8055F420;

extern "C" {

int fn_8025FE8C(void) {
    if (lbl_8055F3E9 && !lbl_8055F3EC) {
        u8* o = PTR(lbl_8055F420, 0x14);
        U8(o, 0x49) = 1;
        U8(o, 0x4B) = 0;
        U8(o, 0x4A) = 0;
        lbl_8055F3EC = 1;
    }
    return 0;
}

void fn_8025FECC(u8* p) {}  // stub (92)

void fn_8026003C(u8* p) {}  // stub (482)

void fn_802607C4(u8* p) {
    U8(p, 0x1D) = 1;
}

void fn_802607D0(u8* p) {
    U8(p, 0x1E) = 1;
}

void fn_802607DC(u8* p) {
    U8(p, 0x1E) = 0;
    U8(p, 0x1C) = 0;
}

void fn_802607EC(u8* p) {
    U8(p, 0x1D) = 0;
}

void fn_802607F8(u8* p) {
    if (!U8(p, 0x1C)) {
        U8(p, 0x1C) = 1;
        U8(p, 0x1F) = 1;
    }
}

void fn_80260814(u8* p) {
    U8(p, 0x1D) = 0;
}

void fn_80260820(u8* p) {
    if (!U8(p, 0x1C)) {
        U8(p, 0x1E) = 1;
        U8(p, 0x1C) = 1;
    }
}

void fn_8026083C(u8* p) {
    U8(p, 0x1C) = 0;
}

void fn_80260848(u8* p) {
    U8(p, 0x1D) = 1;
}

u8* fn_80260854(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_80345E28;
        fn_80235380(PTR(p, 0x24));
        U32(p, 0x24) = 0;
        fn_802328D4(PTR(p, 0x28));
        U32(p, 0x28) = 0;
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

u8* fn_802608D4(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_80345EA0;
        fn_80235380(PTR(p, 0x20));
        U32(p, 0x20) = 0;
        fn_802328D4(PTR(p, 0x24));
        U32(p, 0x24) = 0;
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

void fn_80260954(u8* p) {}  // stub (284)

u8* fn_80260DC4(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_80260E04(Words5* d, const Words5* s) { *d = *s; }

void fn_80260E30(u8* p) {}  // stub (166)

int fn_802610C8(void) {
    if (lbl_8055F3EB) {
        lbl_8055F3EA = 1;
        fn_80214A9C(0x1E);
        s32 a = (*(VPage**)(lbl_8055F420 + S32(lbl_8055F420, 0x0) * 4 + 0x60))->v24();
        fn_8025C204((*(VPage**)(lbl_8055F420 + S32(lbl_8055F420, 0x0) * 4 + 0x60))->v20(), a);
        fn_80214AF0(0x1E);
        fn_80230E90(0);
        lbl_8055F3EA = 0;
    }
    return 0;
}

void fn_80261178(u8* p) {}  // stub (154)

void fn_802613E0(u8* p) {}  // stub (170)

void fn_80261688(u8* p, s32 a, s32 b, s32 c) {
    S32(p, 0x8) = c;
    S32(p, 0x4) = 6;
    PTR(p, 0x0) = lbl_8055B860;
    S32(p, 0xC) = b;
    S32(p, 0x10) = a;
}

void fn_802616A8(u8* p) {}  // stub (168)

u32 fn_80261948(s32* p) {
    Words3 t = lbl_802FA6A8;
    return t.w[*p];
}

void fn_80261980(u8* p, s32 a, s32 b, s32 c) {
    S32(p, 0x0) = a;
    S32(p, 0x4) = 3;
    S32(p, 0x8) = b;
    S32(p, 0xC) = c;
    S32(p, 0x10) = 0;
}

u32 fn_802619A0(s32* p) {
    Words3 t = lbl_802FA6B4;
    return t.w[*p];
}

u8* fn_802619D8(u8* p, int i) { return PTR(p, 0x0) + i * 4; }

void fn_802619E8(u8* p, s32 a, s32 b) {
    S32(p, 0x0) = a;
    S32(p, 0x4) = 5;
    S32(p, 0x8) = b;
    S32(p, 0xC) = 0;
    S32(p, 0x10) = 0;
}

u8* fn_80261A08(u8* p, int i) { return PTR(p, 0x0) + i * 4; }

void fn_80261A18(u8* p, s32 a) {
    S32(p, 0x8) = a;
    S32(p, 0x4) = 8;
    PTR(p, 0x0) = lbl_8055BAB8;
    S32(p, 0xC) = 2;
    S32(p, 0x10) = 0;
}

void fn_80261A40(int x) {
    fn_80236914();
    lbl_8055F3E9 = x == 2;
}

void fn_80261A7C(u8* p, s32 a) {
    S32(p, 0x8) = a;
    S32(p, 0x4) = 8;
    PTR(p, 0x0) = lbl_8055BAC0;
    S32(p, 0xC) = 0;
    S32(p, 0x10) = 0;
}

void fn_80261AA0(void) {
    for (int i = 0; i < S32(lbl_8055F420, 0xC); i++) {
        fn_80214A9C(i + 10);
    }
}

void fn_80261AE8(u8* p, u32 v) { U32(p, 0x0) = v; }

}  // extern "C"

extern u8 lbl_80498F20[];          // unlock animation widget
extern UnkStatic123 lbl_80498F00;  // this unit's end-of-menu tag
extern "C" const char* fn_80131234(const char* key);
extern "C" int fn_801345D8(char* buf, int n, const char* fmt, ...);

extern "C" {

// "Challenge difficulty N unlocked" message box.
void fn_80261AF0(u8* p, int difficulty) {
    char buf[100];
    fn_801345D8(buf, 100, "EventPath:CHALLENGE_DIFFICULTY_%d_UNLOCKED", difficulty);
    MenuItem items[] = {
        MenuItem(0, 5, lbl_80498F20, 0, 0),
        MenuItem("title", fn_80131234("EventPath:CONGRATULATIONS")),
        MenuItem("message", fn_80131234(buf)),
        MenuItem((const char*)lbl_8055B860, 6, 0, -2, 13),
        MenuItem((const char*)lbl_8055BAB8, 8, (const void*)fn_80261A40, 2, 0),
        MenuItem(lbl_80498F00),
    };
    MenuDesc desc = {"newmode1.lyt", 0, 0, 0, 0};
    desc.items = items;
    fn_802146BC(&desc, -999, -999, 1);
}

void fn_80261C68(u8* p) {}  // stub (302)

void fn_80262120(u8* p) {
    PTR(p, 0x0) = p + 0xC;
    S32(p, 0x8) = 3;
    S32(p, 0x4) = 0;
}

void fn_8026213C(u8* p) {
    PTR(p, 0x0) = p + 0xC;
    S32(p, 0x8) = 6;
    S32(p, 0x4) = 0;
}

void fn_80262158(u8* p) {}

void fn_8026215C(u8* p) {}

u32 fn_80262160(u8* p) { return 0; }

void fn_80262168(u8* p) {
    if (U8(p, 0x1E) && lbl_8055F3E9 && !lbl_8055F3EA) {
        fn_80230E90(0);
        U8(p, 0x1E) = 0;
    }
    if (lbl_8055F3EB && !lbl_8055F3EA) {
        fn_80214AF0(0x1E);
    } else {
        fn_80214A9C(0x1E);
    }
}

void fn_802621F0(u8* p) {}  // stub (225)

void fn_80262574(u8* p) {}

void fn_80262578(u8* p) {}

u32 fn_8026257C(u8* p) { return 0; }

void fn_80262584(u8* p) {
    if (U8(p, 0x1F) && lbl_8055F3E9) {
        fn_80230E90(0);
        U8(p, 0x1F) = 0;
    }
}

void fn_802625D4(u8* p) {}  // stub (585)

void fn_80262EF8(u8* p) {}  // stub (37)
}  // extern "C"
