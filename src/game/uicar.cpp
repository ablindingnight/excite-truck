// Car select widgets (0x8023FE20-0x80243460): the per-player join/leave widget ("JoinLeaveWidget"),
// the car icons ("caricona.art"), colour and stat labels ("UICar:COLOR%d", "UICar:STAT_%i_TEXT_%i")
// and the list selection state. Written against raw field offsets; the large routines are still
// stubs. Names are guesses apart from the strings.
#include "game/common.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

class VU {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual int v14(void);
    virtual void v18(void);
    virtual u8* v1C(int i);
};

extern "C" {
void fn_80235380(u8* p);
u8* fn_80232578(const char* name);
int fn_802E8504(int ch);
void __construct_array(void* p, void* ctor, void* dtor, u32 size, u32 n);
u8* fn_80241098(u8* p);
}

void operator delete(void* p);

// The two players' joined slots.
struct JoinSlots {
    s32 first;     // offset 0x0
    u8 pad4[0xC];  // offset 0x4
    s32 second;    // offset 0x10
};

extern JoinSlots lbl_80498828;
extern void* lbl_803426B0[];
extern void* lbl_80342734[];
extern s32 lbl_8055F1F4;
extern u8 lbl_8055F1FC;
extern u32 lbl_8055F200;

ModuleTag lbl_8055F1E8;
UnkStatic123s lbl_80498810;
UnkStatic7F lbl_8055F1F0;

extern "C" {

void fn_8023FE20(void) {
    JoinSlots* j = &lbl_80498828;
    j->first = -1;
    j->second = -1;
}

int fn_8023FE34(void) {
    JoinSlots* j = &lbl_80498828;
    int n = 0;
    if (j->first >= 0) {
        n = 1;
    }
    if (j->second >= 0) {
        n++;
    }
    return n;
}

void fn_8023FE60(u8* p) {}  // stub (47)

void fn_8023FF1C(u8* p) {}  // stub (549)

u8* fn_802407B0(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

u8* fn_802407F0(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_80240830(u8* p) {}  // stub (32)

u8* fn_802408B0(u8* p, int del) {
    if (p) {
        *(void**)(p + 0xC) = lbl_803426B0;
        *(void**)(p + 0x10) = (u8*)lbl_803426B0 + 0x18;
        lbl_8055F1F4--;
        fn_80235380(PTR(p, 0x44));
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

void fn_80240928(u8* p) {}  // stub (31)

void fn_802409A4(u8* p, u8 v) { U8(p, 0x2E8) = v; }

void fn_802409AC(u8* p, u32 item) {
    s32 n = S32(p, 0x2C);
    *(u32*)(p + 0x18 + n * 4) = item;
    S32(p, 0x2C) = n + 1;
    for (int i = 0; i < S32(p, 0x2C); i++) {
        u8 sel = 0;
        u32 v = *(u32*)PTR(p, 0x34);
        if (U8(p, 0x38) && i == S32(p, 0x30)) {
            sel = 1;
        }
        u8* e = *(u8**)(p + 0x18 + i * 4);
        U8(e, 0x0) = sel;
        U32(e, 0x4) = v;
    }
}

void fn_80240A1C(u8* p) {}  // stub (98)

void fn_80240BA4(u8* p) {}  // stub (283)

void fn_80241010(u8* p, s32 a, s32 b, s32 c) {
    S32(p, 0x24) = b;
    S32(p, 0x1C) = a + 1;
    S32(p, 0x20) = c;
}

u8* fn_80241024(u8* p) {
    __construct_array(p + 0x2C, (void*)fn_80241098, (void*)fn_80240928, 0x28, 5);
    memset(p, 0, 0x2C);
    U8(p, 0xF4) = 0;
    U8(p, 0x158) = 0;
    U8(p, 0x1BC) = 0;
    U8(p, 0x220) = 0;
    U8(p, 0x284) = 0;
    return p;
}

u8* fn_80241098(u8* p) {
    *(void**)p = lbl_80342734;
    U32(p, 0x14) = 0;
    S32(p, 0x1C) = -1;
    U32(p, 0x24) = 0;
    U32(p, 0x20) = 0;
    PTR(p, 0x18) = fn_80232578("uifnt_o.gtx");
    return p;
}

void fn_802410F8(u8* p) {}  // stub (43)

int fn_802411A4(u32* out) {
    if (lbl_8055F1FC) {
        *out = lbl_8055F200;
        return 1;
    }
    return 0;
}

void fn_802411C8(void) { fn_8023FE20(); }

int fn_802411CC(void) { return fn_8023FE34(); }

void fn_802411D0(u8* p) { fn_8023FE60(p); }

int fn_802411D4(void) {
    int r = 0;
    if (fn_802E8504(0) && fn_802E8504(1)) {
        r = 1;
    }
    return r;
}

void fn_80241224(u8* p) {}

void fn_80241228(u8* p) {}

void fn_8024122C(u8* p) {}  // stub (74)

void fn_80241354(u8* p) {}  // stub (307)

void fn_80241820(u8* p) {}  // stub (370)

void fn_80241DE8(u8* p) {
    for (int i = 0; i < S32(p, 0x2C); i++) {
        u8 sel = 0;
        u32 v = *(u32*)PTR(p, 0x34);
        if (U8(p, 0x38) && i == S32(p, 0x30)) {
            sel = 1;
        }
        u8* e = *(u8**)(p + 0x18 + i * 4);
        U8(e, 0x0) = sel;
        U32(e, 0x4) = v;
    }
}

void fn_80241E40(u8* p) {}  // stub (42)

u8* fn_80241EE8(u8* p) { return p + 0x4c; }

void fn_80241EF0(u8* p) {}  // stub (251)

void fn_802422DC(u8* p) {}  // stub (249)

u8* fn_802426C0(u8* p) {
    s32 i = S32(p, 0x28);
    if (i >= 0 && i < ((VU*)PTR(p, 0x24))->v14()) {
        return ((VU*)PTR(p, 0x24))->v1C(i);
    }
    return 0;
}

void fn_80242738(u8* p) {}  // stub (249)

void fn_80242B1C(u8* p) {}  // stub (275)

void fn_80242F68(u8* p) {}  // stub (298)

}  // extern "C"
