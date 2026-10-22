// Career event path screen (0x8027E908-0x8028777C): the career map ("EventPath:..."), unlock
// announcements ("unlocked_career", "CAREER_DIFFICULTY_%d_UNLOCKED", "CONGRATULATIONS"), the
// bracket/contest selection lists and the animation wait list ("add_anim_to_wait_for"). Written
// against raw field offsets; the large screen routines are still stubs. Names are guesses apart from
// the strings.
#include "game/common.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

class VR {
public:
    virtual ~VR(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(void);
    virtual u8* v1C(s32 x);
    virtual void v20(void);
    virtual void v24(void);
    virtual int v28(void);
    virtual void v2C(void);
    virtual void v30(int a);
    virtual void v34(void);
    virtual void v38(const char* s);
    virtual int v3C(void);
    virtual void v40(void);
    virtual void v44(void);
    virtual void v48(void);
    virtual int v4C(void);
};

#define VR(p) ((VR*)(p))

struct U32List {
    u32* data;  // offset 0x0
    s32 count;  // offset 0x4
};

extern "C" {
void fn_80236914(void);
void fn_80216D28(void);
f32 fn_8023659C(void);
void fn_80214A9C(int s);
void fn_80214AF0(int s);
void fn_8027E908(u8* p);
void fn_80286A34(u8* p);
void fn_80287238(u8* p);
void fn_80283938(u8* p);
}

void operator delete(void* p);

extern U32List lbl_804B30D8;

ModuleTag lbl_8055F670;
s32 lbl_8055F678;
u8 lbl_8055F67C;
VR* lbl_8055F680;
u8 lbl_8055F684;
u8 lbl_8055F685;
s32 lbl_8055F688;
s32 lbl_8055F68C;
s32 lbl_8055F690;
s32 lbl_8055F694;
u32 lbl_8055F698;
s32 lbl_8055F69C;

extern "C" {

void fn_8027E908(u8* p) {}  // stub (265)

void fn_8027ED2C(void) {
    delete lbl_8055F680;
    lbl_8055F680 = 0;
}

VR* fn_8027ED70(void) { return lbl_8055F680; }

void fn_8027ED78(u8* p) {}  // stub (63)

void fn_8027EE74(int x) {
    lbl_8055F684 = x == 2;
    fn_80236914();
}

int fn_8027EE88(void) {
    if (!lbl_8055F685 && lbl_8055F684) {
        lbl_8055F685 = 1;
        fn_8027ED70()->v38("unlocked_career");
    }
    return 0;
}

void fn_8027EEE4(u8* p) {}  // stub (110)

void fn_8027F09C(u8* p) {}  // stub (90)

void fn_8027F204(u8* p) {}  // stub (33)

void fn_8027F288(u8* p) {}  // stub (363)

void fn_8027F834(u8* p) {}  // stub (73)

void fn_8027F958(u8* p) {}  // stub (976)

int fn_80280898(void) {
    if (lbl_8055F688 == 0) {
        fn_80216D28();
        return 0;
    }
    return 1;
}

void fn_802808D0(u8* p) {}  // stub (90)

void fn_80280A38(u8* p) {}  // stub (114)

void fn_80280C00(u8* p) {}  // stub (114)

void fn_80280DC8(u8* p) {}  // stub (109)

void fn_80280F7C(u8* p) {}  // stub (111)

void fn_80281138(u8* p) {}  // stub (95)

void fn_802812B4(u8* p) {}  // stub (2376)

u8* fn_802837D4(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_80283814(u8* p) {}  // stub (73)

void fn_80283938(u8* p) {}  // stub (38)

void fn_802839D0(u8* p) {}  // stub (32)

int fn_80283A50(void) {
    int i;
    u32 key = lbl_8055F698;
    u32* d = lbl_804B30D8.data;
    for (i = 0; i < lbl_804B30D8.count; i++) {
        if (*d == key) {
            goto found;
        }
        d++;
    }
    i = -1;
found:
    if (i >= 0) {
        return 1;
    }
    fn_80216D28();
    return 0;
}

void fn_80283AC8(u8* p) {}  // stub (36)

void fn_80283B58(u8* p) {}  // stub (180)

void fn_80283E28(u8* p) {}  // stub (179)

void fn_802840F4(u8* p) { fn_8027E908(p); }

void fn_802840F8(void) { fn_8027ED2C(); }

void fn_802840FC(u8* p) {}  // stub (289)

void fn_80284580(u8* p) {}  // stub (120)

void fn_80284760(u8* p) {}  // stub (162)

void fn_802849E8(u8* p) {}  // stub (702)

void fn_802854E0(u8* p) {}  // stub (178)

void fn_802857A8(u8* p) {}  // stub (324)

void fn_80285CB8(u8* p) {}  // stub (667)

void fn_80286724(u8* p) { VR(PTR(p, 0x1C))->v0C(); }

void fn_80286738(u8* p) { VR(PTR(p, 0x1C))->v10(); }

void fn_8028674C(u8* p) { VR(PTR(p, 0x1C))->v14(); }

void fn_80286760(u8* p) { VR(PTR(p, 0x1C))->v18(); }

void fn_80286774(u8* p, int a) { VR(PTR(p, 0x1C))->v1C(a); }

void fn_80286788(u8* p) { VR(PTR(p, 0x1C))->v20(); }

void fn_8028679C(u8* p) { VR(PTR(p, 0x1C))->v24(); }

void fn_802867B0(u8* p) {}  // stub (33)

void fn_80286834(u8* p) { VR(p)->v30(VR(p)->v28()); }

void fn_80286884(u8* p, int a) { VR(PTR(p, 0x1C))->v30(a); }

void fn_80286898(u8* p) { VR(PTR(p, 0x1C))->v34(); }

void fn_802868AC(u8* p, const char* a) { VR(PTR(p, 0x1C))->v38(a); }

void fn_802868C0(u8* p) { VR(PTR(p, 0x1C))->v3C(); }

int fn_802868D4(u8* p) {
    int a = VR(p)->v3C();
    return a <= VR(p)->v28();
}

void fn_8028693C(u8* p) { VR(PTR(p, 0x1C))->v44(); }

int fn_80286950(u8* p) {
    int a = VR(p)->v4C();
    return a <= VR(p)->v28();
}

void fn_802869B8(u8* p) { VR(PTR(p, 0x1C))->v4C(); }

int fn_802869CC(u8* p, int key) {
    switch (key) {
    case 0xD: {
        u8* o = PTR(p, 0x20);
        if (o && !U8(p, 0x29)) {
            F32(o, 0x10) = (f32)(S32(o, 0x4) - S32(o, 0x0));
            return 1;
        }
        break;
    }
    }
    return 0;
}

void fn_80286A34(u8* p) {}  // stub (513)

void fn_80287238(u8* p) {}  // stub (41)

void fn_802872DC(u8* p) { VR(PTR(p, 0x4))->v0C(); }

void fn_802872F0(u8* p) { VR(PTR(p, 0x4))->v10(); }

void fn_80287304(u8* p) { VR(PTR(p, 0x4))->v14(); }

void fn_80287318(u8* p) { VR(PTR(p, 0x4))->v18(); }

u8* fn_8028732C(u8* p, s32 x) {
    if (PTR(p, 0x8) && x == S32(p, 0xC)) {
        return PTR(p, 0x8);
    }
    return VR(PTR(p, 0x4))->v1C(x);
}

void fn_80287360(u8* p) {}

void fn_80287364(u8* p) {}  // stub (37)

void fn_802873F8(u8* p) { VR(PTR(p, 0x4))->v24(); }

void fn_8028740C(u8* p) {}  // stub (37)

void fn_802874A0(u8* p) { VR(PTR(p, 0x4))->v2C(); }

void fn_802874B4(u8* p, int a) { VR(PTR(p, 0x4))->v30(a); }


u8* fn_802875B8(u8* p, int del) {
    if (p) {
        if (p) {
            U32(p, 0x0) = 0;
            U32(p, 0x4) = 0;
            U32(p, 0x8) = 0;
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

u8* fn_80287610(u8* p, int del) {
    if (p) {
        if (p) {
            U32(p, 0x0) = 0;
            U32(p, 0x4) = 0;
            U32(p, 0x8) = 0;
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

u8* fn_80287668(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_802876A8(u8* p) {
    F32(p, 0x18) = fn_8023659C();
    U8(p, 0x1C) = 0;
    fn_80214A9C(0x1D);
}

void fn_802876E8(u8* p) {
    f32 t = fn_8023659C();
    if (!(t < F32(p, 0x18)) && t - F32(p, 0x18) > 1.5f) {
        U8(p, 0x1C) = 1;
    }
    if (U8(p, 0x1C)) {
        fn_80214AF0(0x1D);
    }
}

int fn_8028774C(u8* p) { return U8(p, 0x1C) == 0; }

int fn_8028775C(u8* p, int k) { return fn_802869CC(p - 4, k); }

void fn_80287764(u8* p) { fn_80286A34(p - 4); }

void fn_8028776C(u8* p) { fn_80287238(p - 4); }

void fn_80287774(u8* p) { fn_80283938(p - 4); }

}  // extern "C"
