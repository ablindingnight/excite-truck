// Career event path screen (0x8027E908-0x8028777C): the career map ("EventPath:..."), unlock
// announcements ("unlocked_career", "CAREER_DIFFICULTY_%d_UNLOCKED", "CONGRATULATIONS"), the
// bracket/contest selection lists and the animation wait list ("add_anim_to_wait_for"). Written
// against raw field offsets; the large screen routines are still stubs. Names are guesses apart from
// the strings.
#include "game/common.h"
#include "game/menu.h"
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
    virtual int v18(void);
    virtual u8* v1C(s32 x);
    virtual u8* v20(void);
    virtual int v24(void);
    virtual int v28(void);
    virtual void v2C(void);
    virtual void v30(int a);
    virtual void v34(void);
    virtual void v38(const char* s);
    virtual int v3C(void);
    virtual int v40(void);
    virtual void v44(void);
    virtual int v48(void);
    virtual int v4C(void);
};

#define VR(p) ((VR*)(p))

struct U32List {
    int Count(void) const { return count; }
    template <typename V>
    int Find(V* v) {
        for (int i = 0; i < Count(); i++) {
            if ((V*)data[i] == v) {
                return i;
            }
        }
        return -1;
    }
    template <typename V>
    void Remove(V* v) {
        int i = Find(v);
        if (i == -1) {
            return;
        }
        if (i >= 0 && i < count) {
            int last = count - 1;
            if (i != last) {
                data[i] = data[last];
            }
            count--;
        }
    }
    u32* data;  // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
};

extern "C" {
void fn_80236914(void);
void fn_80216D28(void);
void fn_802E30C8(const char* fmt, ...);
void fn_8025C204(u8* p, s32 a);
f32 fn_8023659C(void);
void fn_80214A9C(int s);
void fn_80214AF0(int s);
void fn_8027E908(u8* p);
void fn_80286A34(u8* p);
void fn_80287238(u8* p);
u8* fn_80283938(u8* p, int del);
}

void operator delete(void* p);

extern U32List lbl_804B30D8;
extern U32List lbl_804B3048;
extern void* lbl_80348678[];

// Copies at most n characters, always terminating.
static inline void CopyName(const char* s, int n, char* d) {
    while ((*d++ = *s++) != 0) {
        if (--n <= 0) {
            d[-1] = 0;
            break;
        }
    }
}

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

}  // extern "C"

extern UnkStatic123 lbl_804B2FF8;  // this unit's end-of-menu tag
extern char lbl_8055B860[8];

extern "C" {

// Shows the "event unlocked" message box.
void fn_8027ED78(const char* title, const char* message) {
    MenuItem items[] = {
        MenuItem("#title", title),
        MenuItem("#message", message),
        MenuItem(lbl_8055B860, 6, 0, -2, 13),
        MenuItem(lbl_804B2FF8),
    };
    MenuDesc desc = {"ep_unlock.lyt", 0, 0, 0, 0};
    desc.items = items;
    fn_802146BC(&desc, -999, -999, 1);
}

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

}  // extern "C"

extern char lbl_80347E50[];  // this unit's string pool
extern u8 lbl_804B3018[];    // unlock animation widget
extern "C" const char* fn_80131234(const char* key);
extern "C" int fn_801345D8(char* buf, int n, const char* fmt, ...);

extern "C" {

// "Career difficulty N unlocked" message box.
void fn_8027F09C(int difficulty) {
    char buf[100];
    fn_801345D8(buf, 100, lbl_80347E50 + 0x3D8, difficulty);  // "EventPath:CAREER_DIFFICULTY_%d_UNLOCK_INSTRUCTIONS"
    MenuItem items[] = {
        MenuItem(0, 5, lbl_804B3018, 0, 0),
        MenuItem("title", fn_80131234(lbl_80347E50 + 0x40C)),  // "EventPath:CHALLENGE"
        MenuItem("message", fn_80131234(buf)),
        MenuItem(lbl_8055B860, 6, 0, -2, 13),
        MenuItem(lbl_804B2FF8),
    };
    MenuDesc desc = {"unlock_crr_chal.lyt", 0, 0, 0, 0};
    desc.items = items;
    fn_8027ED70()->v38(lbl_80347E50 + 0x214);  // "passed_career"
    fn_802146BC(&desc, -999, -999, 1);
}

void fn_8027F204(const char* name) {
    u8* l = (u8*)lbl_8055F69C;
    if (!l) {
        return;
    }
    if (S32(l, 0x44) + 1 <= S32(l, 0x48)) {
        CopyName(name, 30, (char*)(PTR(l, 0x40) + S32(l, 0x44)++ * 30));
        return;
    }
    fn_802E30C8("add_anim_to_wait_for: can't add, there are %d already!", S32(l, 0x44));
}

void fn_8027F288(u8* p) {}  // stub (363)

extern void* lbl_80348798[];

u8* fn_8027F834(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_80348798;
        if ((u32)lbl_8055F688 == (u32)p) {
            lbl_8055F688 = 0;
        }
        lbl_804B3048.Remove(p);
        if (p) {
            U8(p, 0x18) = 0;
            U8(p, 0x50) = 0;
            U8(p, 0x5C) = 0;
            S32(p, 0x24) = 1;
            S32(p, 0x20) = 0;
            U8(p, 0x44) = 0;
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

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

extern void* lbl_80348738[];

u8* fn_80283814(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_80348738;
        if ((u32)lbl_8055F698 == (u32)p) {
            lbl_8055F698 = 0;
        }
        lbl_804B30D8.Remove(p);
        if (p) {
            U8(p, 0x18) = 0;
            U8(p, 0x50) = 0;
            U8(p, 0x5C) = 0;
            S32(p, 0x24) = 1;
            S32(p, 0x20) = 0;
            U8(p, 0x44) = 0;
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

u8* fn_80283938(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_80348678;
        *(void**)(p + 0x4) = (u8*)lbl_80348678 + 0x50;
        lbl_8055F69C = 0;
        if (PTR(p, 0x20)) {
            operator delete(PTR(p, 0x20));
        }
        u8* m = p + 0x40;
        if (m) {
            if (m) {
                U32(p, 0x40) = 0;
                U32(p, 0x44) = 0;
                U32(p, 0x48) = 0;
            }
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

int fn_802839D0(void) {
    if (lbl_8055F684 && lbl_8055F698) {
        s32 a = VR(PTR((u8*)lbl_8055F698, 0x60))->v24();
        fn_8025C204(VR(PTR((u8*)lbl_8055F698, 0x60))->v20(), a);
    } else {
        fn_80216D28();
    }
    return 0;
}

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

int fn_80283AC8(void) {
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
    if (i >= 0 && PTR((u8*)lbl_804B30D8.data[i], 0x60)) {
        return 1;
    }
    fn_80216D28();
    return 0;
}

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

s32 fn_802867B0(u8* p) {
    u8* o = PTR(p, 0x20);
    if (o) {
        s32 v = (s32)((f32)S32(o, 0x0) + F32(o, 0x10));
        s32 m = S32(o, 0x4);
        return v < m ? v : m;
    }
    return VR(PTR(p, 0x1C))->v28();
}

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

void fn_80287238(u8* p) {
    u8* o = PTR(p, 0x20);
    if (o) {
        F32(o, 0x10) = 0.0f;
        U8(p, 0x29) = 0;
        U8(p, 0x2A) = 0;
        U8(p, 0x28) = VR(p)->v40();
        fn_8027ED70()->v38("scoreup_tally");
        fn_80214A9C(0x1F);
        F32(p, 0x178) = 1.0f;
    } else {
        U8(p, 0x29) = 1;
        U8(p, 0x2A) = 0;
        F32(p, 0x178) = 0.0f;
    }
}

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

int fn_80287364(u8* p) {
    for (int i = 0; i < VR(p)->v18(); i++) {
        if (!VR(VR(p)->v1C(i))->v40()) {
            return 0;
        }
    }
    return 1;
}

void fn_802873F8(u8* p) { VR(PTR(p, 0x4))->v24(); }

int fn_8028740C(u8* p) {
    for (int i = 0; i < VR(p)->v18(); i++) {
        if (!VR(VR(p)->v1C(i))->v48()) {
            return 0;
        }
    }
    return 1;
}

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

u8* fn_80287774(u8* p, int del) { return fn_80283938(p - 4, del); }

}  // extern "C"
