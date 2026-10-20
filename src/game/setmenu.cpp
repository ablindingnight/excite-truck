// Settings menus (0x8023C2B4-0x8023C9C8): the display and audio menus (each a pair of option
// cyclers, a screen adjust entry and back/exit), the screen adjust page, and the option cycler
// itself, which steps an int through a NULL-terminated list of labels. Names are guesses apart
// from the layout names and labels.
#include <stdarg.h>
#include "game/common.h"

void* operator new[](size_t size, const char* tag);

struct MenuItem {
    MenuItem(const char* n, int t, const void* d, int a0, int a1) {
        type = t;
        name = n;
        data = d;
        arg0 = a0;
        arg1 = a1;
    }
    const char* name;
    int type;
    const void* data;
    int arg0;
    int arg1;
};

struct MenuDesc {
    const char* layout;
    MenuItem* items;
    int (*poll)(int* result);
    int unkC;
    int unk10;
};

// A menu widget (menu.cpp).
class MenuWidget {
public:
    MenuWidget(void) {
        flag0 = 0;
        id = -1;
        flag8 = 0;
    }

    u8 flag0;  // offset 0x0
    s32 id;    // offset 0x4
    u8 flag8;  // offset 0x8

    virtual u8 vf08(void);
    virtual u8 vf0C(void);
    virtual int vf10(void);
    virtual int vf14(void);
    virtual ~MenuWidget(void) {}
    virtual const char* Current(void) = 0;
    virtual void Prev(void) = 0;
    virtual void Next(void) = 0;
    virtual int vf28(void);
    virtual int vf2C(void);
};

// Steps *value through a NULL-terminated list of labels.
class StringMulti : public MenuWidget {
public:
    StringMulti(int* value, const char* label, ...) {
        va_list args;
        va_start(args, label);
        count = 0;
        do {
            count++;
        } while (va_arg(args, const char*) != NULL);
        va_end(args);
        labels = new ("StringMulti") const char*[count];
        va_start(args, label);
        int i = 0;
        do {
            labels[i++] = label;
            label = va_arg(args, const char*);
        } while (label != NULL);
        va_end(args);
        this->value = value;
    }
    virtual ~StringMulti(void) { delete[] labels; }
    virtual void Prev(void) { *value = (*value + count - 1) % count; }
    virtual void Next(void) { *value = (*value + 1) % count; }
    virtual const char* Current(void) { return labels[*value]; }

    const char** labels;  // offset 0x10
    int count;            // offset 0x14
    int* value;           // offset 0x18
};

extern "C" {
int fn_802146BC(MenuDesc* desc, int x, int y, int a);
void fn_80236914(void);
void fn_80236900(void);
int fn_8027AA48(void);
void fn_80214A9C(int id);
void fn_80214BE4(int sound, int a);
int fn_80214D04(void);
const char* fn_80131234(const char* key);  // localize
void fn_802E30C8(const char* fmt, ...);    // debug print
int fn_802C83C4(void);                     // widescreen mode
void fn_802C82C4(int mode);
int fn_802090CC(void);                     // sound mode
void fn_802090BC(int mode);
extern char lbl_8055BAC0[8];
extern char lbl_8055BAB8[4];
extern char lbl_8055B860[8];
}

ModuleTag lbl_8055F138;
static UnkStatic123 lbl_80498788;
UnkStatic7F lbl_8055F140;
int lbl_8055F144;  // sound mode
int lbl_8055F148;  // widescreen mode
int lbl_8055F14C;  // sound mode applied
int lbl_8055F150;  // widescreen mode applied
int lbl_8055F154;  // screen adjust

extern "C" {

void fn_8023C2B4(void) {
    if (!fn_8027AA48()) {
        fn_80214A9C(0x1F);
    }
}

int fn_8023C2E4(void) {
    fn_80236900();
    return 1;
}

int fn_8023C308(int* result) {
    if (result) {
        int left = fn_80214D04() == 2;
        int adjust = lbl_8055F154;
        int v;
        if (left) {
            v = adjust - 5;
            v = v < 0 ? 0 : v;
        } else {
            v = adjust + 5;
            v = v > 70 ? 70 : v;
        }
        int d = v - adjust;
        lbl_8055F154 += d;
        if (d) {
            fn_80214BE4(7, 0);
        }
    }
    if (lbl_8055F148 != lbl_8055F150) {
        lbl_8055F150 = lbl_8055F148;
        fn_802E30C8("WSM: %d", lbl_8055F148);
        fn_802C82C4(lbl_8055F148);
    }
    if (lbl_8055F144 != lbl_8055F14C) {
        lbl_8055F14C = lbl_8055F144;
        fn_802090BC(lbl_8055F144);
    }
    return 0;
}

int fn_8023C3CC(void) { return 0; }

void fn_8023C3D4(void) {
    MenuItem items[] = {
        MenuItem(lbl_8055B860, 6, (void*)fn_8023C2E4, -1, 27),
        MenuItem(lbl_8055B860, 6, (void*)fn_8023C3CC, -10, 0x126),
        MenuItem(lbl_8055B860, 6, (void*)fn_8023C3CC, 10, 0x128),
        MenuItem(lbl_8055B860, 6, (void*)fn_8023C3CC, -1, 0x125),
        MenuItem(lbl_8055B860, 6, (void*)fn_8023C3CC, 1, 0x127),
        MenuItem(lbl_8055BAB8, 8, (void*)fn_80236914, 2, 0),
        MenuItem(lbl_8055BAC0, 8, (void*)fn_8023C2B4, 0, 0),
        *(MenuItem*)&lbl_80498788,
    };
    MenuDesc desc = {"s_adjust.lyt", items, NULL, 0, 0};
    fn_802146BC(&desc, -999, -999, 1);
}

void fn_8023C55C(const char* layout) {
    lbl_8055F150 = lbl_8055F148 = fn_802C83C4();
    StringMulti display(&lbl_8055F148, fn_80131234("Display:STANDARD"), fn_80131234("Display:WIDESCREEN"), NULL);
    lbl_8055F14C = lbl_8055F144 = fn_802090CC();
    StringMulti audio(&lbl_8055F144, fn_80131234("Audio:MONO"), fn_80131234("Audio:STEREO"),
                      fn_80131234("Audio:DOLBY_PRO_LOGIC_II"), NULL);
    MenuItem items[] = {
        MenuItem("display", 16, NULL, (int)&display, 0),
        MenuItem("audio", 16, NULL, (int)&audio, 0),
        MenuItem(lbl_8055B860, 6, (void*)fn_8023C2E4, -1, 27),
        MenuItem("adjust", 0, NULL, 3, 0),
        MenuItem(lbl_8055BAB8, 8, (void*)fn_80236914, 2, 0),
        MenuItem(lbl_8055BAC0, 8, (void*)fn_8023C2B4, 0, 0),
        *(MenuItem*)&lbl_80498788,
    };
    MenuDesc desc = {layout, items, fn_8023C308, 0, 0};
    for (;;) {
        lbl_8055F154 = 0;
        if (fn_802146BC(&desc, -999, -999, 1) != 3) {
            break;
        }
        fn_8023C3D4();
    }
    fn_8023C308(NULL);
}

void fn_8023C94C(void) { fn_8023C55C("audio.lyt"); }

void fn_8023C958(void) { fn_8023C55C("display.lyt"); }

}  // extern "C"

// NonMatching: fn_8023C308 keeps the step in a different register (2 diffs); MenuWidget's inline dtor is
// emitted here as a weak copy that the target keeps in menu.cpp; the 0x48 zero bytes after the vtable
// (also after other menu widgets' vtables) are not reproduced.
