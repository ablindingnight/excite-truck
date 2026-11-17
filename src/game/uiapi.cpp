// UI API layer (0x8021165C-0x802160AC): UIShowGroup/UIHideGroup/UIMoveGroup/UIGetMenuIndex... on the
// active window, plus menu-state glue. The unit ends with __sinit and the weak inline functions it emits after it
// (MenuWidget/UICustomHandler/OptionPair/FloatValue dtors, OptionPair and MenuWidget(Base) virtuals,
// UICustomHandler::Unk24), up to 0x802160AC where widgetwin.cpp starts. Imported verbatim from the upstream project
// (github.com/orangedude27/excite-truck, CC0), where its functions match exactly.
// CLEANUP: raw offsets and placeholder classes from an automated decompiler; replace with real types
// (Window, Widget, MenuState) before the PC port. Tricks are tagged MATCHING-AID.

#include <types.h>
#include "game/gart.h"
// The retail UI API section has only the concrete MenuState table.
#define IMENUSTATE_NO_VTABLE
#include "game/menustate.h"
#undef IMENUSTATE_NO_VTABLE
#include "game/uilists.h"
#include "game/menu.h"
#include "game/menuwidget_inl.h"
#include "game/gxcanvas.h"
// Provisional emission form: UI API owns the four standalone UIImage bodies.
#define UIAPI_IMAGE_METHODS_OUT_OF_LINE
#include "game/uiwidget.h"
#undef UIAPI_IMAGE_METHODS_OUT_OF_LINE
#include "game/locstring.h"

extern "C" s32 fn_80218C14(s32, s32, s32);
extern "C" s32 fn_802E317C(const char *, ...);
extern "C" void fn_80214934(const void *, s32, void *, s32);
extern "C" char* strrchr(const char *, int);
extern "C" s32 fn_801345D8(void *, s32, const char *, ...);
extern "C" unsigned long strlen(const char *);
extern "C" s32 fn_802C4F14(s32);
extern "C" s32 fn_802C4C90(s32, s32, s32, s32, s32);
extern "C" void fn_8021481C(const char*);

extern "C" s32 fn_802168E4(void);
class Window;
extern "C" void fn_802168CC(Window* window, s32 group, f32 time);
extern "C" void fn_802168D0(Window* window, s32 group, f32 time);
extern "C" s32 fn_802168D4(Window* window, s32 group);
extern "C" void fn_802168D8(Window* window, s32 group);
extern "C" void fn_802168DC(Window* window, s32 group);
extern "C" void fn_802168E0(Window* window, s32 group, s32 x, s32 y);

extern "C" s32 fn_802180E0(s32, s32);


class VfObj {
public:
    virtual ~VfObj();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void vf5();
    virtual void vf6();
    virtual void vf7();
    virtual void vf8();
    virtual void vf9();     // vtable 0x24
    virtual void vf10(int x, int y);    // vtable 0x28
    virtual void vf11(int x, int y);    // vtable 0x2C
    virtual void vf12(int x, int y);
    virtual void vf13();
    virtual void vf14();
    virtual void vf15();
      virtual void vf16();
  virtual void vf17();
  virtual void vf18();
  virtual void vf19();
  virtual void vf20();
  virtual int vf21();     // vtable 0x5C
};


extern "C" void fn_80221E7C(void*);
extern "C" void* fn_80215270(int, int);
extern "C" void fn_8022E4BC(s32);
extern "C" void fn_8022E960(s32);
s32 fn_80211678(u8* arg0);
extern "C" int sprintf(char *, const char *, ...);
extern "C" int strcasecmp(const char *, const char *);
extern "C" void *memcpy(void *, const void *, unsigned long);
// Name of the last layout UIBegin saw (first .data object of the unit).
extern "C" char lbl_8033CAE8[100];
char lbl_8033CAE8[100] = "__OOPS_NO_LAST_LAYOUT__";
// Layout names CopyName/strcasecmp compare against: the first layout and the "no layout" name.
extern "C" char* lbl_8055B6B0;
char* lbl_8055B6B0 = "__first";
extern "C" const char* lbl_8055B6BC;
const char* lbl_8055B6BC = "__null";
extern "C" void fn_80214E10(const char* title, const char* message, int (*poll)(int*));
extern "C" int fn_80214FE0(const char* title, const char* message, int (*poll)(int*), int defaultNo);
extern "C" s32 fn_80206430(void);
extern "C" void fn_8022BAD0(s32 style, s32 x, s32 y, s32 z, s32 w, s32 h, const char* text, s32 color, s32 extra, f32 scale);
extern "C" s32 fn_8022AC1C(s32, s32, s32, s32, s32, s32, s32, s32);
extern "C" float fn_80209028(void);
extern "C" int lbl_8055EF5C;

extern "C" s32 (*lbl_8055EF0C)(s32 *);
extern "C" s32 fn_802168BC(s32);
extern "C" void* fn_8021165C(int dir, int set) {
    return fn_80215270(dir, set);
}

extern "C" gArtSet* lbl_8055EF1C;
extern "C" gArtSet* lbl_8055EF20;
// The lite-mode blinker art (0 = left, else right).
// Plain two-return form: the target has no bnelr fold here, which only optimization_level 0 reproduces.
#pragma push
#pragma optimization_level 0
gArtSet* fn_80211660(s32 arg0) {
    if (arg0 == 0) {
        return lbl_8055EF1C;
    } else {
        return lbl_8055EF20;
    }
}
#pragma pop

s32 fn_80211678(u8 *arg0) {
    return ((s8) *arg0 - 0x24) == 0;
}

extern "C" void fn_80211690(int);
extern "C" void fn_80211D24(int);
extern "C" int fn_802E2C20(void);
extern "C" int fn_802ECC10(void);
extern "C" int lbl_8055EF14;
extern "C" float lbl_8055EF10;
extern "C" int lbl_8055EF40;
extern "C" double lbl_80563448;

// One button-sound slot: the sound bank plus four playing voices.
class UIVoice {
public:
    virtual ~UIVoice(void);
};
#include "game/uitable.h"

struct UISoundSlot {
    const char* bank;
    float volume;
    UIVoice* voices[4];
};
// uitable.cpp defines Set(int, int, const char*, char) but this caller passes the flag unconverted (no extsb):
// call retail's symbol directly with an int flag (PC: the member).
#ifdef __MWERKS__
extern "C" void Set__7UITableFiiPCcc(UITable*, int, int, const char*, int);
#else
static inline void Set__7UITableFiiPCcc(UITable* t, int row, int col, const char* s, int flag) { t->Set(row, col, s, (char)flag); }
#endif
extern "C" UISoundSlot lbl_80490558[8];
extern "C" void* lbl_80490528[3][4];
extern "C" void* lbl_8055EF24;
extern "C" void* lbl_8055EF28;
extern "C" UITable* lbl_8055EF2C;
extern "C" CycleList* lbl_8055EF30;
extern "C" NameList* lbl_8055EF34;
extern "C" int lbl_8055EF18;
extern "C" void fn_802ECC18(int);
extern "C" void fn_8022D2BC(void*, int);
extern "C" void fn_8021676C(void);
extern "C" void fn_8022A7D0(void);
extern "C" void fn_8020A550(void);
extern "C" void fn_802C519C(void*);
extern "C" void fn_80238920(void);
extern "C" void fn_8012BF24(const char*);
extern "C" int fn_80128314(int);
extern "C" void fn_802E1058(void);
extern "C" void* memset(void*, int, unsigned long);
extern "C" void fn_802E30C8(const char *, ...);
extern "C" void fn_802E0F98(void);
extern "C" void fn_8012BEF0(const char*);
extern "C" void fn_8020A47C(void);
extern "C" void fn_8023889C(void);
extern "C" void* gxGetImage(const char*);
extern "C" void fn_8022A5E4(void);
extern "C" void fn_80216710(void);
extern "C" void* fn_8022D228(void*, int, int);
extern "C" void fn_8022D350(void*, const char*, int);
extern "C" UIVoice* fn_80207848(const char*, int);
extern "C" int lbl_8055EF08;
extern "C" int lbl_8055EF3C;
extern "C" void* lbl_8055EF24;
extern "C" void* lbl_8055EF28;
extern "C" void *lbl_8055EF38;
void LoadTimerProgress(void);
extern "C" void LoadTimerEnd__FPCc(const char* name, int);
void* operator new(unsigned long size, const char* tag);

static inline void SetSound(UISoundSlot* slot, const char* bank, float volume) {
    slot->bank = bank;
    slot->volume = volume;
    memset(slot->voices, 0, sizeof(slot->voices));
}

// Copies at most n characters, always terminating.
static inline void CopyName(const char* src, int n, char* dst) {
    char* d = dst;
    int left = n;
    while ((*d++ = *src++) != 0) {
        if (--left <= 0) {
            d[-1] = 0;
            break;
        }
    }
}

// UIBegin: mode 0 = lite (only the two lite arrows), 1 = arcade-style sounds, else race-day sounds.
extern "C" void fn_80211690(int mode) {
    bool lite;
    int i;
    fn_802E0F98();
    lbl_8055EF40 = mode;
    lite = mode == 0;
    bool timer = !lite;
    lbl_8055EF18 = fn_80128314(0);
    lbl_8055EF08 = 1;
    if (!lite) {
        fn_8012BEF0("uiaction.res");
        if (timer) {
            LoadTimerProgress();
        }
    }
    fn_8020A47C();
    if (!lite) {
        fn_8023889C();
    }
    const char* art[3][4] = {
        {"a_lslide.img", "a_rslide.img", "a_uscroll.img", "a_dscroll.img"},
        {"blinkerlt.img", "blinkerrt.img", "blinkerup.img", "blinkerdn.img"},
        {"blinkerlt.img", "blinkerrt.img", "blinkerup.img", "blinkerdn.img"},
    };
    static const char* sItemNames[8] = {"Item 0", "Item 1", "Item 2", "Item 3",
                                        "Item 4", "Item 5", "Item 6", "Item 7"};
    static long sItemValues[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
    for (i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            const char* name = art[i][j];
            if (lite) {
                if (i == 2 && (j == 0 || j == 1)) {
                    lbl_80490528[i][j] = gxGetImage(name);
                } else {
                    lbl_80490528[i][j] = 0;
                }
            } else {
                lbl_80490528[i][j] = gxGetImage(name);
            }
        }
    }
    if (!lite) {
        lbl_8055EF1C = fn_8023512C("blinkerlt2.art");
        lbl_8055EF20 = fn_8023512C("blinkerrt2.art");
    }
    lbl_8055EF10 = 0.0f;
    fn_8022A5E4();
    fn_80216710();
    if (!lite) {
        void* list = operator new(0x14, "generic_list");
        if (list) list = fn_8022D228(list, 0x20, 0x40);
        lbl_8055EF24 = list;
        list = operator new(0x14, "generic_menu_list");
        if (list) list = fn_8022D228(list, 8, 0x40);
        lbl_8055EF28 = list;
        for (i = 0; i < 32; i++) {
            char name[0x40];
            sprintf(name, "Generic %d", i + 1);
            int value = i % 4 - 1;
            if (value < 0) {
                value = 0;
            }
            fn_8022D350(lbl_8055EF24, name, value);
        }
        for (i = 0; i < 8; i++) {
            char option[0x20];
            sprintf(option, "Option%d", i);
            fn_8022D350(lbl_8055EF28, option, i * 4);
        }
        int widths[1] = {32};
        lbl_8055EF2C = new ("generic_table") UITable(20, 1, widths);
        for (i = 0; i < lbl_8055EF2C->rows; i++) {
            char entry[0x20];
            sprintf(entry, "Entry %d", i + 1);
            Set__7UITableFiiPCcc(lbl_8055EF2C, i, 0, entry, i % 3);
        }
        lbl_8055EF30 = new ("generic_button_list") CycleList(sItemNames, sItemValues, 8, 0);
        const char* tabs[3] = {"Menu1", "Menu2", "Menu3"};
        lbl_8055EF34 = new ("generic_tab_list") NameList(tabs, 3, 0);
    } else {
        lbl_8055EF24 = 0;
        lbl_8055EF28 = 0;
    }
    memset(lbl_80490558, 0, sizeof(lbl_80490558));
    if (mode == 1) {
        const char* accept = "a_accept.sfx";
        SetSound(&lbl_80490558[0], "a_press.sfx", 0.8f);
        SetSound(&lbl_80490558[2], accept, 0.8f);
        SetSound(&lbl_80490558[3], "a_back.sfx", 0.8f);
        SetSound(&lbl_80490558[1], "a_error.sfx", 0.8f * 0.6f);
        SetSound(&lbl_80490558[4], "a_unprsnt.sfx", 0.8f);
        SetSound(&lbl_80490558[5], "a_popup.sfx", 0.8f * 0.6f);
        SetSound(&lbl_80490558[6], accept, 0.8f);
        SetSound(&lbl_80490558[7], accept, 0.8f);
    } else {
        SetSound(&lbl_80490558[0], "rd_press.sfx", 0.8f);
        SetSound(&lbl_80490558[2], "rd_accept.sfx", 0.8f);
        SetSound(&lbl_80490558[3], "rd_accept.sfx", 0.8f);
        SetSound(&lbl_80490558[1], "rd_accept.sfx", 0.8f);
        SetSound(&lbl_80490558[4], "rd_unprsnt.sfx", 0.8f);
        SetSound(&lbl_80490558[5], 0, 0.8f);
        SetSound(&lbl_80490558[6], "repair.sfx", 0.8f);
        SetSound(&lbl_80490558[7], "register.sfx", 0.8f);
    }
    for (i = 0; i < 8; i++) {
        if (lbl_80490558[i].bank) {
            for (int j = 0; j < 4; j++) {
                lbl_80490558[i].voices[j] = fn_80207848(lbl_80490558[i].bank, 0);
            }
        }
    }
    lbl_8055EF3C = -1;
    lbl_8055EF38 = (void*)-1;
    if (timer) {
        LoadTimerEnd__FPCc("-UIBegin", 0);
    }
    CopyName(lbl_8055B6B0, 100, lbl_8033CAE8);
    const char* label = lite ? "UIBeginLite" : "UIBegin";
    fn_802E30C8("%s: overhead = %dK", label, (fn_80128314(0) - lbl_8055EF18) / 1024);
    fn_802ECC18(1);
}

// UIEnd: tears down what UIBegin set up (mode 0 = lite: only the two lite arrows were loaded).
extern "C" void fn_80211D24(int mode) {
    int j;
    bool lite;
    int i;
    fn_802ECC18(0);
    lite = mode == 0;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 4; j++) {
            if (lbl_80490558[i].voices[j]) {
                delete lbl_80490558[i].voices[j];
            }
        }
    }
    memset(lbl_80490558, 0, sizeof(lbl_80490558));
    if (lbl_8055EF24) {
        fn_8022D2BC(lbl_8055EF24, 1);
        lbl_8055EF24 = 0;
    }
    if (lbl_8055EF28) {
        fn_8022D2BC(lbl_8055EF28, 1);
        lbl_8055EF28 = 0;
    }
    if (lbl_8055EF2C) {
        delete lbl_8055EF2C;
        lbl_8055EF2C = 0;
    }
    if (lbl_8055EF30) {
        delete lbl_8055EF30;
        lbl_8055EF30 = 0;
    }
    if (lbl_8055EF34) {
        delete lbl_8055EF34;
        lbl_8055EF34 = 0;
    }
    fn_8021676C();
    fn_8022A7D0();
    fn_8020A550();
    if (!lite) {
        if (lbl_8055EF1C) {
            fn_80235380(lbl_8055EF1C);
        }
        lbl_8055EF1C = 0;
        if (lbl_8055EF20) {
            fn_80235380(lbl_8055EF20);
        }
        lbl_8055EF20 = 0;
        for (i = 0; i < 3; i++) {
            for (j = 0; j < 4; j++) {
                fn_802C519C(lbl_80490528[i][j]);
                lbl_80490528[i][j] = 0;
            }
        }
        fn_80238920();
        fn_8012BF24("uiaction.res");
    } else {
        fn_802C519C(lbl_80490528[2][0]);
        lbl_80490528[2][0] = 0;
        fn_802C519C(lbl_80490528[2][1]);
        lbl_80490528[2][1] = 0;
    }
    int mem = fn_80128314(0);
    if (mem != lbl_8055EF18) {
        fn_802E30C8("UI overlap: %d K vs %d K", mem, lbl_8055EF18);
    }
    fn_802E1058();
}

extern "C" void fn_80211F44(void) { fn_80211690(1); }

extern "C" void fn_80211F4C(void) { fn_80211D24(1); }

extern "C" int fn_80211F54(void) { return lbl_8055EF40 == 0; }

extern "C" void fn_80211F64(void) {
    int now = fn_802E2C20();
    if (fn_802ECC10() == 0) {
        // frame time in seconds, capped at a quarter second
        float dt = 0.001f * (float)(now - lbl_8055EF14);
        lbl_8055EF10 = 0.25f < dt ? 0.25f : dt;
    } else {
        lbl_8055EF10 = 0.0f;
    }
    lbl_8055EF14 = now;
}

extern "C" void fn_80211FF0(void) {
    s32 sp8;
    s32 (*cb)(s32 *);
    cb = lbl_8055EF0C;
    sp8 = -2;
    if (cb != 0) {
        if (cb(&sp8) != 0) {
            fn_802168BC(sp8);
        }
    }
}

extern "C" s32 fn_8012C33C(s32);
extern "C" s32 fn_802C49E8(s32);
extern "C" s32 fn_802C2DD4(s32);
// Image/text reference handed to buttons ("StyleImage", "rimg", "timg").
class UIImageRef : public UIImage {  /* Class name unproven */
public:
    UIImageRef(const char* n, int f) {
        flags = f;
        name = n;
    }
    virtual ~UIImageRef(void);
    virtual void DrawAt(int x, int y, int a, int b, int c, int frame);
    virtual void Rect(int x, int y, int* left, int* top, int* right, int* bottom);
    virtual int Frames(void);
    virtual void Extra(void);  /* Helper name unproven */

    const char* name;  // 0x4
    int flags;         // 0x8
};

// make_image's drawers (class names unproven): "stp" sprite tables and "cvs" canvases.
class StpImage : public UIImage {
public:
    StpImage(s32 name) { value = fn_802C49E8(name); }
    virtual ~StpImage(void);
    virtual void Rect(int x, int y, int* left, int* top, int* right, int* bottom);
    virtual int Frames(void);
    virtual void DrawFrame(int x, int y, int frame);
    s32 value;  // 0x4
};

class CvsImage : public UIImage {
public:
    CvsImage(char* name, s32 div) {
        void* c;
        if (fn_8012C33C((s32)name)) {
            c = (void*)fn_802C2DD4((s32)name);
            value = c;
            count = *(s32*)((char*)c + 12) / div;
        } else {
            value = 0;
            count = 0;
        }
        divisor = div;
    }
    virtual ~CvsImage(void);
    virtual void Rect(int x, int y, int* left, int* top, int* right, int* bottom);
    virtual int Frames(void);
    virtual void DrawFrame(int x, int y, unsigned int frame);
    void* value;  // 0x4
    s32 count;    // 0x8
    s32 divisor;  // 0xC
};

extern "C" void *fn_8021203C(char *arg0, s32 arg1, s32 arg2, s32 arg3) {
    char *name = strrchr(arg0, 0x2e);
    static char canvasSuffix[] = "cvs";
    if (strcasecmp(name + 1, "stp") == 0) {
        return new ("simg") StpImage((s32)arg0);
    }
    if (strcasecmp(name + 1, canvasSuffix) == 0) {
        return new ("cimg") CvsImage(arg0, arg1);
    }
    if (strcasecmp(name + 1, "img") == 0) {
        return new ("iimg") AnimationPrefix(arg0, arg2, arg3);
    }
    if (strcasecmp(name + 1, "art") == 0) {
        return new ("aimg") ImagePrefix(arg0);
    }
    fn_802E317C("make_image: unknown extension %s", name + 1);
    return 0;
}



extern s32 (*lbl_8055EF0C)(s32 *);
extern "C" s32 fn_80212214(s32 arg0) {
    s32 value;
    memcpy(&value, &arg0, 4);
    return value;
}

extern "C" float fn_80212248(s32 arg0) {
    float value;
    memcpy(&value, &arg0, 4);
    return value;
}

extern "C" void fn_8022E4BC(s32);
extern "C" void fn_8022E960(s32);
extern "C" int sprintf(char *, const char *, ...);
extern "C" int strcasecmp(const char *, const char *);
extern "C" void *fn_8021227C(void *entry, const char *name, s32 arg2) {
    char buf[0x100];
    if (*name == '#') {
        name = name + 1;
    }
    sprintf(buf, "#%s", name);
    while (((MenuItem *)entry)->type != 0x7B) {
        if (((MenuItem *)entry)->type == arg2 && ((MenuItem *)entry)->name != 0 &&
            (strcasecmp(((MenuItem *)entry)->name, name) == 0 ||
             strcasecmp(((MenuItem *)entry)->name, buf) == 0)) return entry;
        entry = (char *)entry + 0x14;
    }
    return 0;
}

extern "C" s32 fn_80212338(void) { return 0; }

extern "C" void fn_80212340(const void *arg0) {
    s32 first = *(s32 *)((char *)arg0 + 0x10);
    s32 *p = (s32 *)arg0;
    if (first != 0) {
        fn_8022E4BC(first);
        p[4] = 0;
    }
    u8 *name = *(u8 **)arg0;
    if (name != 0) {
        if (fn_80211678(name) == 0) {
            fn_8022E4BC(p[3]);
        } else {
            fn_8022E960(p[3]);
        }
        p[3] = 0;
    }
}

// Widget factory: builds the widget(s) for one parsed UI descriptor and adds them to the window.
// Constructors live in widgets.cpp (plain functions returning the object); styles are looked up by name and kind
// in the window's style table (fn_8021227C). Cross-checked against the PS2 Test Drive: Eve of Destruction twin.
struct WidgetFragment;
struct UIDesc {
    u32 type;           // 0x00 widget kind (switch below)
    int flags;          // 0x04
    int x, y;           // 0x08
    int kind;           // 0x10
    int dx, dy;         // 0x14 size
    float scale;        // 0x1C
    char style[0x20];   // 0x20 style name
    char name[0x48];    // 0x40 text / resource name ('#' = style reference)
    int window;         // 0x88
    int group;          // 0x8C
    int field90, field94, field98;
    u32 field9c;        // 0x9C
    int fielda0;        // 0xA0
    u8 rest[0x90];      // 0xA4 (descriptor stride 0x134)
};
struct UIStyle {
    const char* name;
    int kind;
    void* value;        // 0x08
    void* arg1;         // 0x0C
    void* arg2;         // 0x10
};
struct UIWidgetView {
    u8 pad[0x240];
    void* style;        // 0x240
};

extern "C" UnkStatic123 lbl_80490510;  // this unit's 0x7B end-of-list item
extern "C" f32 __float_epsilon[];  // FLT_EPSILON
extern "C" void* lbl_8055EF24;
extern "C" void* lbl_8055EF28;
extern "C" void *lbl_8055EF38;
extern "C" int lbl_8055EF48;
extern "C" int lbl_8055EF4C;
extern "C" int lbl_8055EF50;
extern "C" int lbl_8055EF54;
extern "C" int lbl_8055EF58;
// Default option list (NUL-separated) for Multi widgets without a style.
extern "C" const char* lbl_8055B754;
const char* lbl_8055B754 = "MIN\0LOW\0MEDIUM\0HIGH\0MAX\0";

void* operator new(unsigned long size, const char* tag);
extern "C" char* strstr(const char*, const char*);
extern "C" int abs(int);
extern "C" void* fn_80215270(int, int);
// MATCHING-AID (transitional): the factory still calls these constructors by linker name. ArrowBtn, RadioButton and
// TabButton: retail evaluates the watched value into a named local after the allocation check (a new-expression swaps
// saved registers); gText: its table has a pure 0x8C slot, and MWCC refuses a new-expression on an abstract class.
extern "C" WidgetFragment* __ct__11RadioButtonFiiiPvP11OwnedWidget(WidgetFragment*, int, int, int, void*, void*);
extern "C" WidgetFragment* __ct__9TabButtonFiiiPvP11OwnedWidget(WidgetFragment*, int, int, int, void*, void*);
extern "C" WidgetFragment* __ct__5gTextFiiiP11OwnedWidgetPv(WidgetFragment*, int, int, int, void*, void*);
extern "C" WidgetFragment* __ct__8ArrowBtnFiiPviiPvff(WidgetFragment*, int, int, void*, int, int, void*, float, float);
extern "C" void fn_802E30C8(const char *, ...);
extern "C" int fn_80129BC4(void);
extern "C" unsigned int fn_8022D1B4(const char*);
extern "C" void fn_802168EC(void*, WidgetFragment*, int);
extern "C" void fn_80215298(s32 *, s32 *, s32 *, s32 *, s32, s32, s32);
// Float parameter positions follow the call site's evaluation order (scale is read after the value pointer).


// The placeholder two-choice widget ("OPTION 1" / "OPTION 2").
class OptionPair : public MenuWidget {
public:
    OptionPair(void) { value = 0; }
    virtual ~OptionPair(void) {}
    virtual const char* Current(void) {
        if (value) {
            return "OPTION 2";
        }
        return "OPTION 1";
    }
    virtual void Prev(void) { value ^= 1; }
    virtual void Next(void) { value ^= 1; }
    u32 value;  // offset 0x10
};

// Handler state a "Custom" widget reports its rectangle to: x, y, w, h, with the widget in unk14.
// Its Unk24 fills that rectangle on the canvas in the custom-widget colour.
extern "C" const u32 lbl_8055FBDC;
extern "C" void fn_802C3424(s32 x0, s32 y0, s32 x1, s32 y1, s32 a);
class UICustomHandler : public MenuState {
public:
    UICustomHandler(void) {}
    virtual ~UICustomHandler(void) {}
    virtual void Unk24(gxCanvas* canvas) {
        fn_802C3034(canvas);
        fn_802C3424(x, y, x + w, y + h, lbl_8055FBDC);
    }
};

class UICustom : public UIWidget {  /* Class name unproven */
public:
    virtual ~UICustom(void);
    virtual void Slot0C(void);
    virtual void Slot10(void);
    virtual void Slot14(void);
    virtual void Slot1C(int x, int y);
    virtual void Slot24(void* context);
    virtual void Slot28(void);
    virtual void Slot30(void);
    virtual void Slot34(void);
    virtual void Slot38(void);
    virtual void Slot3C(int a, int b);
    virtual void Slot40(int a, int b);
    virtual void Slot44(void);
    virtual void Slot48(void);
    virtual void Slot4C(int x, int y);
    virtual void Slot58(void);
    virtual void Slot5C(void);
    virtual void* Slot60(void);
    virtual int Slot64(int key);
    virtual int Slot68(void);
    virtual void Slot74(int key, void* value);
    virtual void Slot7C(void);
    virtual void Slot80(void);

    UICustomHandler* handler;  // 0x244
    UICustom(int x, int y, int dx, int dy, int kind, UICustomHandler* h) : UIWidget(x, y, x + dx, y + dy, kind) {
        handler = h;
        h->unk14 = (int)this;
        h->x = x;
        h->y = y;
        h->w = dx;
        h->h = dy;
    }
};

#include "game/optionvalue.h"

#include "game/floatvalue.h"

class UIBox {
public:
    UIBox(void) {
        unk4 = 0;
        unkC = 0;
    }
    u32 unk0;
    u32 unk4;
    u32 unk8;
    u8 unkC;
};

class UITriple {
public:
    UITriple(void) {
        a = 0;
        b = 0;
        c = 0;
    }
    u32 a, b, c;
    u32 pad[5];
};

ModuleTag lbl_8055EEF8;
UnkStaticFFFF lbl_8055EF00;
UnkStatic123 lbl_80490510;
UnkStatic7F lbl_8055EF04;
void* lbl_80490528[3][4];
UISoundSlot lbl_80490558[8];
LocString sOk("UI:Ok");
LocString sCancel("UI:Cancel");
LocString sYes("UI:Yes");
LocString sNo("UI:No");
LocString sDrat("UI:Drat");
LocString sReturn("UI:Return");
static FloatValue sDefaultSpin((f32*)&lbl_8055EF50, 0.0f, 100.0f, 1.0f, 0.0f, 1.0f);
static OptionPair sDefaultMulti;
static char sDefaultInput[0x40];
static UIBox sDefaultControl;
UICustomHandler lbl_80490710[16];
static UITriple sDefaultView;

static inline UIStyle* FindStyle(void* styles, const char* name, s32 kind) {
    return (UIStyle*)fn_8021227C(styles, (char*)name, kind);
}

static inline WidgetFragment* CreateImageRadioButton(UIDesc* d, void* styles) {
    UIStyle* style = FindStyle(styles, d->style, 2);
    WidgetFragment* memory = (WidgetFragment*)operator new(0x25c, "RadioButton");
    if (memory) {
        if (style) style = (UIStyle*)style->value;
        else style = (UIStyle*)&lbl_8055EF4C;
        memory = __ct__11RadioButtonFiiiPvP11OwnedWidget(memory, d->x, d->y, d->field9c, style, new ("rimg") UIImageRef(d->name, d->flags));
    }
    return memory;
}

static inline WidgetFragment* CreateImageTabButton(UIDesc* d, void* styles) {
    UIStyle* style = FindStyle(styles, d->style, 2);
    WidgetFragment* memory = (WidgetFragment*)operator new(0x25c, "TabButton");
    if (memory) {
        if (style) style = (UIStyle*)style->value;
        else style = (UIStyle*)&lbl_8055EF54;
        memory = __ct__9TabButtonFiiiPvP11OwnedWidget(memory, d->x, d->y, d->field9c, style, new ("timg") UIImageRef(d->name, d->flags));
    }
    return memory;
}

extern "C" void fn_802123B8(void* window, UIDesc* d, void* styles) {
    UIStyle* value = 0;
    int count = 0;
    WidgetFragment* widgets[4] = {0, 0, 0, 0};
    WidgetFragment* p;
    lbl_8055EF38 = (void*)d->group;
    switch (d->type) {
    case 5: {
        UIStyle* text = FindStyle(styles, d->style, 3);
        if (text) {
            value = FindStyle(styles, d->style, 2);
            p = (WidgetFragment*)new ("StaticText") StaticText(d->x, d->y, (const char*)text->value, d->flags, (int*)(value ? value->value : 0), 0);
            widgets[count++] = p;
            break;
        }
        UIStyle* multi = FindStyle(styles, d->style, 7);
        if (multi) {
            p = (WidgetFragment*)new ("Multi") Multi(d->x, d->y, (int*)multi->value, (const char*)multi->arg1, d->flags, 0, 0, 0);
            widgets[count++] = p;
            break;
        }
        value = FindStyle(styles, d->style, 2);
        p = (WidgetFragment*)new ("StaticText") StaticText(d->x, d->y, d->name, d->flags, (int*)(value ? value->value : 0), 0);
        widgets[count++] = p;
        break;
    }
    case 28: {
        UIStyle* text;
        if (d->name[0] == '#') {
            text = FindStyle(styles, d->name + 1, 3);
            if (!text) {
                text = FindStyle(styles, d->name + 1, 7);
                if (text) {
                    int first = fn_80212214(d->field9c);
                    int second = fn_80212214(d->fielda0);
                    p = (WidgetFragment*)new ("CustomMulti") CustomMulti(d->x, d->y, (int*)text->value, (const char*)text->arg1, d->style, d->flags, first, second);
                    widgets[count++] = p;
                    break;
                }
            }
            value = FindStyle(styles, d->name + 1, 2);
        } else {
            text = 0;
        }
        if (!text || !text->value || !strstr((const char*)text->value, ".gtx")) {
            p = (WidgetFragment*)new ("CustomText") CustomText((RotatingChoiceDescriptorNew*)d, (text && text->value) ? (const char*)text->value : d->name, (int*)(value ? value->value : 0));
            widgets[count++] = p;
            break;
        }
        void* image = new ("gTextImage") ResourcePrefix(d->style, (const char*)text->value);
        p = (WidgetFragment*)operator new(0x258, "gText");
        if (p) p = __ct__5gTextFiiiP11OwnedWidgetPv(p, d->x, d->y, d->kind, image, 0);
        widgets[count++] = p;
        break;
    }
    case 31: {
        UIStyle* column = FindStyle(styles, d->name, 0xd);
        p = (WidgetFragment*)new ("Column") Column((FormattedChoiceDescriptor*)d, (UITable*)(column ? column->value : (void*)lbl_8055EF2C), (int)(column ? column->arg1 : 0));
        widgets[count++] = p;
        break;
    }
    case 2: {
        UIStyle* button = FindStyle(styles, d->style, 0);
        UIStyle* text = d->name[0] == '#' ? FindStyle(styles, d->name + 1, 3) : 0;
        void* image = button ? button->arg1 : 0;
        const char* label = text ? (const char*)text->value : d->name;
        void* handler = button ? button->value : (void*)fn_80212338;
        OwnedWidget* ref = (OwnedWidget*)new ("StyleImage") UIImageRef(label, d->flags);
        if (d->field9c == 1 || !fn_80129BC4()) {
            p = (WidgetFragment*)new ("IButton") IButton(image, d->x, d->y, d->dx, d->dy, ref, (int (*)(void*))handler, 0, d->field9c != 0);
            widgets[count++] = p;
            break;
        }
        p = (WidgetFragment*)new ("MenuBtn") MenuBtn(image, d->x, d->y, d->dx, d->dy, ref, (int (*)(void*))handler);
        widgets[count++] = p;
        break;
    }
    case 42: {
        UIStyle* button = FindStyle(styles, d->style, 0);
        value = FindStyle(styles, d->style, 2);
        void* image = button ? button->arg1 : 0;
        void* handler;
        if (button) handler = button->value;
        else handler = (void*)fn_80212338;
        void* watched = value ? value->value : 0;
        p = (WidgetFragment*)new ("FadeButton") FadeButton(image, (WidgetDesc*)d, (int (*)(void*))handler, (int*)watched);
        widgets[count++] = p;
        break;
    }
    case 3:
        fn_802E30C8("BButtons aren't supported anymore.  If you want one, talk to Chris");
        break;
    case 34:
        p = (WidgetFragment*)new ("BackWidget") BackWidget(d->name, 1, 1, 1, d->kind);
        widgets[count++] = p;
        break;
    case 17:
        fn_802E30C8("Stamprolls are not supported anymore, talk to Chris if you think you want one.");
    case 33: {
        int useStyle = d->flags == 2 || d->flags == 4 || d->flags == 5;
        value = FindStyle(styles, d->style, 2);
        UIStyle* text = FindStyle(styles, d->style, 3);
        const char* path = text ? (const char*)text->value : d->name;
        void* watched = useStyle ? (void*)d->style : (value ? value->value : 0);
        if (strstr(path, ".img")) {
            p = (WidgetFragment*)operator new(0x258, "imgstamproll");
            if (p) {
                void* image = new ("imgimg") AnimationPrefix(path, d->dx, d->dy);
                p = __ct__5gTextFiiiP11OwnedWidgetPv(p, d->x, d->y, d->kind, image, watched);
            }
            widgets[count++] = p;
            break;
        }
        p = (WidgetFragment*)new ("ArtWidget") ArtWidget((WidgetDesc*)d, path, (int)watched);
        widgets[count++] = p;
        break;
    }
    case 13: {
        UIStyle* flag = FindStyle(styles, d->style, 1);
        p = (WidgetFragment*)new ("CheckBox") CheckBox(d->x, d->y, d->name, (unsigned char*)(flag ? flag->value : &lbl_8055EF48), d->flags);
        widgets[count++] = p;
        break;
    }
    case 14: {
        p = CreateImageRadioButton(d, styles);
        widgets[count++] = p;
        break;
    }
    case 15: {
        UIStyle* style = FindStyle(styles, d->style, 2);
        p = (WidgetFragment*)operator new(0x25c, "BRadioButton");
        if (p) {
            if (style) style = (UIStyle*)style->value;
            else style = (UIStyle*)&lbl_8055EF4C;
            p = __ct__11RadioButtonFiiiPvP11OwnedWidget(p, d->x, d->y, d->field9c, style, fn_8021203C(d->name, 2, -1, -1));
        }
        widgets[count++] = p;
        break;
    }
    case 8: {
        void* value;
        void* low;
        void* high;
        UIStyle* range = FindStyle(styles, d->style, 2);
        if (range) {
            value = range->value;
            low = range->arg1;
            high = range->arg2;
        } else {
            UIStyle* list = FindStyle(styles, d->style, 7);
            if (list) {
                value = list->value;
                low = 0;
                high = (void*)fn_8022D1B4((const char*)list->arg1);
            } else {
                value = &lbl_8055EF4C;
                low = 0;
                high = (void*)5;
            }
        }
        p = (WidgetFragment*)new ("IntArrowBtn") IntArrowBtn(d->x, d->y + 1, (int*)value, (int)low, (int)high, -1, fn_80215270(0, 1));
        widgets[count++] = p;
        if (d->dx - 37 > 10) {
            p = (WidgetFragment*)new ("IntSlider") IntSlider(d->x + 21, d->y, d->dx - 37, d->dy, (int*)value, (int)low, (int)high);
            widgets[count++] = p;
        }
        p = (WidgetFragment*)new ("IntArrowBtn") IntArrowBtn(d->x + d->dx - 14, d->y + 1, (int*)value, (int)low, (int)high, 1, fn_80215270(1, 1));
        widgets[count++] = p;
        break;
    }
    case 18: {
        UIStyle* range = FindStyle(styles, d->style, 9);
        float low = 0.0f;
        float high = 1.0f;
        if (range) {
            low = fn_80212248((s32)range->arg1);
            high = fn_80212248((s32)range->arg2);
        }
        if ((f32)__fabs(low - high) <= __float_epsilon[0]) {
            break;
        }
        if (!d->field9c) {
            break;
        }
        if (d->dx - 37 > 10) {
            p = (WidgetFragment*)new ("Slider") Slider(d->x + 21, d->y, d->dx - 37, d->dy,
                                   (float*)(range ? range->value : &lbl_8055EF50), low, d->field9c, high);
            widgets[count++] = p;
        }
        p = (WidgetFragment*)operator new(0x268, "ArrowBtn");
        if (p) {
            void* value;
            if (range) value = range->value;
            else value = &lbl_8055EF50;
            p = __ct__8ArrowBtnFiiPviiPvff(p, d->x, d->y + 1, value, abs((int)d->field9c), -1, fn_80215270(0, 1), low, high);
        }
        widgets[count++] = p;
        p = (WidgetFragment*)operator new(0x268, "ArrowBtn");
        if (p) {
            void* value;
            if (range) value = range->value;
            else value = &lbl_8055EF50;
            p = __ct__8ArrowBtnFiiPviiPvff(p, d->x + d->dx - 14, d->y + 1, value, abs((int)d->field9c), 1, fn_80215270(1, 1), low, high);
        }
        widgets[count++] = p;
        break;
    }
    case 6: {
        value = FindStyle(styles, d->style, 9);
        p = (WidgetFragment*)new ("Numeric") Numeric(d->x, d->y, (float*)(value ? value->value : &lbl_8055EF50), 100.0f, d->name, 0.0f, d->flags);
        widgets[count++] = p;
        break;
    }
    case 7: {
        value = FindStyle(styles, d->style, 2);
        p = (WidgetFragment*)new ("IntNumeric") IntNumeric(d->x, d->y, (int*)(value ? value->value : &lbl_8055EF4C), 1.0f, d->name, 0.0f, d->flags);
        widgets[count++] = p;
        break;
    }
    case 9: {
        UIStyle* spin = FindStyle(styles, d->style, 0xc);
        void* target = spin ? spin->value : (void*)&sDefaultSpin;
        s32 left, x, right, line;
        switch (d->flags) {
        case 0:
        default:
            fn_80215298(&left, &x, &right, &line, d->x, d->y, d->dx);
            p = (WidgetFragment*)new ("SpinCtl") SpinCtl(x, d->y, (FullFormatDispatch*)target, 15, left, right, line);
            widgets[count++] = p;
            break;
        case 1:
            fn_80215298(&left, &x, &right, &line, d->x, d->y, d->dx);
            p = (WidgetFragment*)new ("SpinCtl") SpinCtl(x, d->y, (FullFormatDispatch*)target, 20, left, right, line);
            widgets[count++] = p;
            break;
        case 4:
            p = (WidgetFragment*)new ("SpinSldr") SpinSldr(d->x + 15, d->y, d->dx - 30, d->dy, (ObservedValueProvider*)target);
            widgets[count++] = p;
            p = (WidgetFragment*)new ("SpinArrow") SpinArrow(d->x + 1, d->y + 2, fn_80215270(0, 2), -1, (FormatTarget*)target);
            widgets[count++] = p;
            p = (WidgetFragment*)new ("SpinArrow") SpinArrow(d->x + d->dx - 15, d->y + 2, fn_80215270(1, 2), 1, (FormatTarget*)target);
            widgets[count++] = p;
            break;
        case 5:
            fn_80215298(&left, &x, &right, &line, d->x, d->y, d->dx);
            p = (WidgetFragment*)new ("SpinText") SpinText(x, d->y, (FullFormatDispatch*)target, 15, left, right, line, d->name, d->field9c);
            widgets[count++] = p;
            break;
        }
        break;
    }
    case 49: {
        UIStyle* list = FindStyle(styles, d->style, 0x10);
        s32 left, x, right, line;
        fn_80215298(&left, &x, &right, &line, d->x, d->y, d->dx);
        p = (WidgetFragment*)new ("MutableMulti") MutableMulti(d->x, d->y, d->dx, d->dy, d->flags,
                               (ChoiceProviderDispatch*)(list ? list->arg1 : (void*)&sDefaultMulti));
        widgets[count++] = p;
        break;
    }
    case 10: {
        UIStyle* multi = FindStyle(styles, d->style, 7);
        void* value = &lbl_8055EF4C;
        const char* list = lbl_8055B754;
        if (multi) {
            value = multi->value;
            list = (const char*)multi->arg1;
        }
        if (d->dx > 0) {
            fn_8022D1B4(list);
            if (d->dx - 37 <= 10) {
                break;
            }
            s32 left, x, right, line;
            fn_80215298(&left, &x, &right, &line, d->x, d->y, d->dx);
            p = (WidgetFragment*)new ("Multi") Multi(x, d->y, (int*)value, (const char*)list, d->flags, left, right, line);
            widgets[count++] = p;
            break;
        }
        p = (WidgetFragment*)new ("Multi") Multi(d->x, d->y, (int*)value, (const char*)list, d->flags, 0, 0, 0);
        widgets[count++] = p;
        break;
    }
    case 16: {
        p = CreateImageTabButton(d, styles);
        widgets[count++] = p;
        break;
    }
    case 27: {
        UIStyle* style = FindStyle(styles, d->style, 2);
        p = (WidgetFragment*)operator new(0x25c, "BTabButton");
        if (p) {
            if (style) style = (UIStyle*)style->value;
            else style = (UIStyle*)&lbl_8055EF54;
            p = __ct__9TabButtonFiiiPvP11OwnedWidget(p, d->x, d->y, d->field9c, style, fn_8021203C(d->name, 1, -1, -1));
        }
        widgets[count++] = p;
        break;
    }
    case 11: {
        UIStyle* input = FindStyle(styles, d->style, 3);
        void* buffer = sDefaultInput;
        void* capacity = (void*)0x40;
        if (input) {
            buffer = input->value;
            capacity = input->arg1;
        }
        p = (WidgetFragment*)new ("Input") Input(d->x, d->y, d->dx, 10, (char*)buffer, (int)capacity, 1, d->flags, 0);
        widgets[count++] = p;
        break;
    }
    case 26: {
        UIStyle* custom = FindStyle(styles, d->style, 5);
        if (custom) {
            widgets[count++] = (WidgetFragment*)new ("Custom")
                UICustom(d->x, d->y, d->dx, d->dy, d->kind, (UICustomHandler*)custom->value);
        } else {
            lbl_8055EF58 = (lbl_8055EF58 + 1) & 0xF;
            widgets[count++] = (WidgetFragment*)new ("Custom")
                UICustom(d->x, d->y, d->dx, d->dy, d->kind,
                         &lbl_80490710[lbl_8055EF58]);
        }
        break;
    }
    case 35: {
        UIStyle* list = FindStyle(styles, d->style, 0xa);
        p = (WidgetFragment*)new ("CircleList") CircleList((WidgetDesc*)d, list ? list->value : lbl_8055EF24);
        widgets[count++] = p;
        break;
    }
    case 23: {
        UIStyle* selected = FindStyle(styles, d->style, 2);
        UIStyle* list = FindStyle(styles, d->style, 0xa);
        UIStyle* view = FindStyle(styles, d->style, 0xb);
        p = (WidgetFragment*)new ("ListBox") ListBox(d->x, d->y, d->dx, d->dy, (StringList*)(list ? list->value : lbl_8055EF24),
                               (int*)(selected ? selected->value : &lbl_8055EF4C),
                               (ObservedListViewport*)(view ? view->value : (void*)&sDefaultView));
        widgets[count++] = p;
        break;
    }
    case 20: {
        UIStyle* view = FindStyle(styles, d->style, 0xb);
        p = (WidgetFragment*)new ("VScrollBar") ScrollBar(d->x - 3, d->y + 21, d->dx, d->dy - 40,
                               view ? view->value : (void*)&sDefaultView, 0);
        widgets[count++] = p;
        p = (WidgetFragment*)new ("ScrollBtn") ScrollBtn(d->x, d->y, (ObservedScrollState*)(view ? view->value : (void*)&sDefaultView), -1, fn_80215270(2, 0));
        widgets[count++] = p;
        p = (WidgetFragment*)new ("ScrollBtn") ScrollBtn(d->x, d->y + d->dy - 18, (ObservedScrollState*)(view ? view->value : (void*)&sDefaultView), 1, fn_80215270(3, 0));
        widgets[count++] = p;
        break;
    }
    case 21: {
        UIStyle* view = FindStyle(styles, d->style, 0xb);
        p = (WidgetFragment*)new ("HScrollBar") ScrollBar(d->x + 21, d->y, d->dx - 40, d->dy,
                               view ? view->value : (void*)&sDefaultView, 1);
        widgets[count++] = p;
        p = (WidgetFragment*)new ("ScrollBtn") ScrollBtn(d->x, d->y + 3, (ObservedScrollState*)(view ? view->value : (void*)&sDefaultView), -1, fn_80215270(0, 0));
        widgets[count++] = p;
        p = (WidgetFragment*)new ("ScrollBtn") ScrollBtn(d->x + d->dx - 18, d->y + 3, (ObservedScrollState*)(view ? view->value : (void*)&sDefaultView), 1, fn_80215270(1, 0));
        widgets[count++] = p;
        break;
    }
    case 12: {
        UIStyle* control = FindStyle(styles, d->style, 4);
        p = (WidgetFragment*)new ("CDetect") CDetect(d->x, d->y, (int)(control ? (const char*)control->arg1 : "Unmapped control"),
                               (Binding*)(control ? control->value : (void*)&sDefaultControl), 6);
        widgets[count++] = p;
        break;
    }
    case 36: {
        UIStyle* selected = FindStyle(styles, d->style, 2);
        UIStyle* list = FindStyle(styles, d->style, 0xa);
        p = (WidgetFragment*)new ("BottomBar") BottomBar(d->x, d->y, (const StringList*)(list ? list->value : lbl_8055EF28),
                               (int*)(selected ? selected->value : &lbl_8055EF4C));
        widgets[count++] = p;
        break;
    }
    case 37: {
        UIStyle* selected = FindStyle(styles, d->style, 2);
        UIStyle* list = FindStyle(styles, d->style, 0xa);
        p = (WidgetFragment*)new ("BarTab") BarTab(d->x, d->y, list ? list->value : lbl_8055EF28,
                               (int*)(selected ? selected->value : &lbl_8055EF54));
        widgets[count++] = p;
        break;
    }
    case 38: {
        UIStyle* menu = FindStyle(styles, d->style, 0xe);
        p = (WidgetFragment*)new ("MenuImage") MenuImage((WidgetDesc*)d, menu ? menu->value : (void*)lbl_8055EF30);
        widgets[count++] = p;
        break;
    }
    case 39: {
        UIStyle* movie = FindStyle(styles, d->style, 3);
        if (movie) {
            p = (WidgetFragment*)new ("MovieWidget") MovieWidget((WidgetDesc*)d, (const char*)movie->value);
            widgets[count++] = p;
        } else {
            p = (WidgetFragment*)new ("MovieWidget") MovieWidget((WidgetDesc*)d, (const char*)0);
            widgets[count++] = p;
        }
        break;
    }
    case 54: {
        UIStyle* model = FindStyle(styles, d->style, 0x13);
        UIStyle* name = FindStyle(styles, d->style, 3);
        if (model) {
            p = (WidgetFragment*)new ("ModelWidget") ModelWidget((WidgetDesc*)d, (MenuCustomView*)model->value, (const char*)(name ? name->value : 0));
            widgets[count++] = p;
        } else {
            p = (WidgetFragment*)new ("ModelWidget") ModelWidget((WidgetDesc*)d, (const char*)(name ? name->value : 0));
            widgets[count++] = p;
        }
        break;
    }
    case 41:
        p = (WidgetFragment*)new ("DynamicLoadBackground") DynamicLoadBackground((WidgetDesc*)d);
        widgets[count++] = p;
        break;
    case 43: {
        UIStyle* text = FindStyle(styles, d->style, 3);
        p = (WidgetFragment*)new ("TextBox") TextBox((int)(text ? text->value : 0), (WidgetDesc*)d);
        widgets[count++] = p;
        break;
    }
    case 44: {
        UIStyle* menu = FindStyle(styles, d->style, 0xe);
        p = (WidgetFragment*)new ("ButtonList") ButtonList((RollingListData*)(menu ? menu->value : (void*)lbl_8055EF30), (WidgetDesc*)d);
        widgets[count++] = p;
        break;
    }
    case 45:
        p = (WidgetFragment*)new ("RotImage") RotImage((WidgetDesc*)d);
        widgets[count++] = p;
        break;
    case 46:
        p = (WidgetFragment*)new ("ControlButton") ControlButton((WidgetDesc*)d);
        widgets[count++] = p;
        break;
    case 47:
        p = (WidgetFragment*)new ("ColorBackground") ColorBackground((WidgetDesc*)d);
        widgets[count++] = p;
        break;
    case 48: {
        UIStyle* tabs = FindStyle(styles, d->style, 0xf);
        UIStyle* text = FindStyle(styles, d->style, 3);
        void* first = tabs ? tabs->value : (void*)lbl_8055EF34;
        p = (WidgetFragment*)new ("TopTabDisplay") TopTabDisplay(first, (WidgetDesc*)d, text ? text->value : 0);
        widgets[count++] = p;
        break;
    }
    case 51:
        p = (WidgetFragment*)new ("TabBox") TabBox((WidgetDesc*)d);
        widgets[count++] = p;
        break;
    case 50: {
        UIStyle* range = FindStyle(styles, d->style, 9);
        UIStyle* steps = FindStyle(styles, d->style, 2);
        p = (WidgetFragment*)new ("SimpleSlider") SimpleSlider((WidgetDesc*)d, (float*)(range ? range->value : 0), steps ? *(int*)steps->value : 10,
                               range ? fn_80212248((s32)range->arg1) : 0.0f,
                               range ? fn_80212248((s32)range->arg2) : 1.0f);
        widgets[count++] = p;
        break;
    }
    case 52: {
        UIStyle* movie = FindStyle(styles, d->style, 0x11);
        p = (WidgetFragment*)new ("UISW2Movie") UISW2Movie((WidgetDesc*)d, (SwfAnim*)(movie ? movie->value : 0));
        widgets[count++] = p;
        break;
    }
    case 53: {
        UIStyle* texture = FindStyle(styles, d->style, 0x12);
        UIStyle* frame = FindStyle(styles, d->style, 2);
        p = (WidgetFragment*)new ("DynoTexWidget") DynoTexWidget((WidgetDesc*)d, (DynoTex*)(texture ? texture->value : 0), (int*)(frame ? frame->value : 0));
        widgets[count++] = p;
        break;
    }
    default:
        ((s32 (*)(const char *, ...))fn_802E317C)("Unknown type: %d", d->type);
        break;
    }
    UIStyle* group = FindStyle(styles, d->style, 0x14);
    if (!group) {
        group = FindStyle(styles, d->name, 0x14);
    }
    for (int i = 0; i < count; i++) {
        if (group) {
            ((UIWidgetView*)widgets[i])->style = group->value;
        }
        fn_802168EC(window, widgets[i], d->window);
    }
    lbl_8055EF38 = (void*)-1;
}

// Layout + mapping a window is built from (UIDoMappedDialog and friends).
struct UILayout {
    u8 pad0[4];
    char name[0x20];    // 0x04 layout / background image name
    s8 hasBack;         // 0x24 also build back.lyt
    s8 hasCamera;       // 0x25
    u8 pad26[2];
    char camera[0x3C];  // 0x28
    UIDesc* descs;      // 0x64 widget descriptors, terminated by type 0
};
struct UIMapping {
    const char* name;   // 0x00 layout name ('$' = built in)
    UIStyle* styles;    // 0x04 style table, terminated by kind 0x7B
    int field8;
    UILayout* layout;   // 0x0C
    UILayout* back;     // 0x10
};
#include "game/window.h"

extern "C" UILayout* fn_8022E0EC(const char*);
extern "C" UILayout* fn_8022E4C0(UIMapping*);
extern "C" void fn_80276D3C(const char*, int);
extern "C" int fn_802C51C0(void*);
extern "C" int fn_802C5220(void*);
extern "C" Window* fn_80216770(const char*, int, int, int, int, int);
extern "C" void fn_80236914(int which, f32* out, f32 t);
extern "C" int strcmp(const char*, const char*);
extern "C" const char* lbl_8055EF44;
extern "C" int lbl_8055CFF0;
extern "C" int lbl_8055CFF4;

// Builds a window from a mapping: layout widgets, the optional back layout, hot keys and mapped functions.
extern "C" void fn_802141A0(Window** out, UIMapping* map, int x, int y, int flags, int widgetWindow) {
    UIStyle* styles = map->styles;
    UILayout* layout;
    if (map->name) {
        if (fn_80211678((u8*)map->name)) {
            layout = fn_8022E4C0(map);
        } else {
            layout = fn_8022E0EC(map->name);
        }
        const char* name = map->name;
        lbl_8055EF44 = name;
        map->layout = layout;
    } else {
        layout = map->layout;
    }
    s8 camera = layout->hasCamera;
    map->back = 0;
    if (camera) {
        if (!lbl_8055EF40) {
            fn_80276D3C(layout->camera, 1);
        } else {
            fn_802E30C8("UI: Not in Lite Mode, can't use camera %s", layout->camera);
        }
    }
    int w = lbl_8055CFF0;
    int h = lbl_8055CFF4;
    const char* background = map->layout->name;
    if (background && strstr(background, ".img") && fn_8012C33C((s32)background)) {
        void* image = gxGetImage(background);
        if (fn_802C51C0(image) > 0 && fn_802C5220(image) > 0) {
            w = fn_802C51C0(image);
            h = fn_802C5220(image);
        }
        fn_802C519C(image);
    }
    if (x == -999) {
        x = (lbl_8055CFF0 - w) / 2;
    }
    if (y == -999) {
        y = (lbl_8055CFF4 - h) / 2;
    }
    Window* win;
    if (widgetWindow) {
        win = new ("WidgetTWindow") Window("", x, y, w, h, flags);
    } else {
        win = fn_80216770(map->layout->name, x, y, w, h, flags);
    }
    *out = win;
    UIDesc* d;
    for (d = layout->descs; d->type; d++) {
    }
    if (layout->hasBack) {
        map->back = fn_8022E0EC("back.lyt");
        for (d = map->back->descs; d->type; d++) {
        }
    }
    int i;
    for (i = 0; styles[i].kind != 0x7B; i++) {
        UIStyle* s = &styles[i];
        switch (s->kind) {
        case 5:
            if (s->name && !strcmp(s->name, "__first")) {
                lbl_8055EF38 = 0;
                UICustom* custom = new ("Custom") UICustom(0, 0, 0, 0, 0, (UICustomHandler*)s->value);
                fn_802168EC(win, (WidgetFragment*)custom, -1);
                lbl_8055EF38 = (void*)-1;
            }
            break;
        }
    }
    if (layout->hasBack) {
        for (d = map->back->descs; d->type; d++) {
            fn_802123B8(win, d, styles);
        }
    }
    for (d = layout->descs; d->type; d++) {
        fn_802123B8(win, d, styles);
    }
    int haveBack = 0;
    for (i = 0; styles[i].kind != 0x7B; i++) {
        UIStyle* s = &styles[i];
        switch (s->kind) {
        case 6: {
            lbl_8055EF38 = 0;
            UIHotKey* key = new ("HotKey") UIHotKey(s->arg1, (u16)(u32)s->arg2, s->value);
            fn_802168EC(win, (WidgetFragment*)key, -1);
            lbl_8055EF38 = (void*)-1;
            break;
        }
        case 8:
            switch ((int)s->arg1) {
            case 0:
                win->open = (void (*)(void))s->value;
                break;
            case 1:
                win->close = (void (*)(void))s->value;
                break;
            case 2:
                haveBack = 1;
                win->fade = (void (*)(int, void*, f32))s->value;
                break;
            case 3:
                win->fadeStyle = (void (*)(Style*, u32, s32, s32, s32, s32, int, f32))s->value;
                break;
            default:
                ((s32 (*)(const char *, ...))fn_802E317C)("UIMAP_FUNC: bad func %d", s->arg1);
                break;
            }
            break;
        case 5:
            if (!s->name) {
                lbl_8055EF38 = 0;
                UICustom* custom = new ("Custom") UICustom(0, 0, 0, 0, 0, (UICustomHandler*)s->value);
                fn_802168EC(win, (WidgetFragment*)custom, -1);
                lbl_8055EF38 = (void*)-1;
            }
            break;
        case 7:
            break;
        }
    }
    if (!haveBack) {
        win->fade = (void (*)(int, void*, f32))fn_80236914;
    }
}

extern "C" void fn_802D08C4(int);
extern "C" void fn_80127A24(void);
extern "C" void fn_802E47E8(void);
extern "C" void fn_80218D54(Window*, int);
extern "C" void fn_80236918(const char*);
extern "C" int fn_802168F0(Window*);
extern "C" void fn_802E3D00(void);
extern "C" void fn_80236950(void);
extern "C" void fn_8021683C(Window*);

// UIDoMappedDialog: builds the mapped window, runs it until it returns a result, then tears it down.
extern "C" int fn_802146BC(MenuDesc* desc, int x, int y, int modal) {
    int result;
    fn_802D08C4(1);
    int mem = fn_80128314(0);
    Window* win;
    fn_802141A0(&win, (UIMapping*)desc, x, y, modal, 0);
    fn_80127A24();
    fn_802E47E8();
    s32 (*poll)(s32*) = lbl_8055EF0C;
    fn_80218D54(win, 0);
    lbl_8055EF0C = (s32 (*)(s32*))desc->poll;
    const char* layout = desc->layout;
    win->update = (void (*)(Window*))fn_80211FF0;
    fn_80236918(layout);
    result = fn_802168F0(win);
    if (lbl_8055EF40) {
        fn_802E3D00();
    }
    fn_80236950();
    fn_8021683C(win);
    lbl_8055EF0C = poll;
    fn_80212340(desc);
    int now = fn_80128314(0);
    if (now != mem) {
        fn_802E30C8("UIDoMappedDialog overlap %s, %d != %d", desc->layout, now, mem);
    }
    fn_802D08C4(0);
    CopyName(desc->layout ? desc->layout : lbl_8055B6BC, 100, lbl_8033CAE8);
    return result;
}

extern "C" s32 fn_802ECEEC(void);
extern "C" void fn_802E30C8(const char *, ...);
extern "C" UnkStatic123 lbl_80490510;  // this unit's 0x7B end-of-list item
extern "C" u8 lbl_8055B860;
// Plays a movie inside the playvid layout (skipped while a movie cannot play).
extern "C" void fn_8021481C(const char* movie) {
    if (fn_802ECEEC() == 0) {
        fn_802E30C8("UI: playing video '%s' in a layout", movie);
        MenuItem items[] = {
            MenuItem("#movie_file", 3, movie, 0, 0),
            MenuItem((const char*)&lbl_8055B860, 6, NULL, -2, 13),
            MenuItem(lbl_80490510),
        };
        MenuDesc desc = {"playvid.lyt", items, NULL, 0, 0};
        fn_802146BC(&desc, -999, -999, 1);
        fn_802E30C8("UI: finished playing video '%s'", movie);
    }
}

extern "C" void fn_80214934(const void *arg0, s32 arg1, void *arg2, s32 arg3) {
    s32 end = (s32)strrchr((const char *)arg1, 0x2e);
    if (end == 0) {
        end = (s32)((const char *)arg1 + strlen((const char *)arg1));
    }
    fn_801345D8(arg2, arg3, "%s\\%.*s", (s32)arg0, end - arg1, (const char *)arg1);
}

extern "C" void fn_802149C4(s32 arg0) {
    char sp8[0x64];
    fn_80214934("transin", arg0, sp8, 0x64);
    fn_8021481C(sp8);
}
extern "C" s32 fn_80218C14(s32, s32, s32);
extern "C" void fn_80214934(const void*, s32, void*, s32);
extern "C" void fn_802149FC(s32 arg0) {
    char sp8[0x64];
    fn_80214934("transout", arg0, sp8, 0x64);
    fn_8021481C(sp8);
}

extern "C" s32 fn_80214A38(s32 arg0) {
    s32 result = 0;
    s32 check = fn_802168E4();
    if (check != 0) result = fn_802168D4((Window*)check, arg0);
    else ((s32 (*)(const char *, ...))fn_802E317C)("UIGroupIsVisible(): No active window");
    return result;
}

extern "C" void fn_80214A9C(s32 arg0) {
    s32 temp = fn_802168E4();
    if (temp != 0) {
        fn_802168CC((Window*)temp, arg0, 0.0f);
    } else {
        ((s32 (*)(const char *, ...))fn_802E317C)("UIHideGroup(): No active window");
    }
}
extern "C" void fn_80214AF0(s32 arg0) {
    s32 temp = fn_802168E4();
    if (temp != 0) {
        fn_802168D0((Window*)temp, arg0, 0.0f);
    } else {
        ((s32 (*)(const char *, ...))fn_802E317C)("UIShowGroup(): No active window");
    }
}

extern "C" void fn_80214B44(s32 arg0) {
    s32 temp = fn_802168E4();
    if (temp != 0) {
        fn_802168DC((Window*)temp, arg0);
    } else {
        ((s32 (*)(const char *, ...))fn_802E317C)("UIDisableGroup(): No active window");
    }
}
extern "C" void fn_80214B94(s32 arg0) {
    s32 temp = fn_802168E4();
    if (temp != 0) {
        fn_802168D8((Window*)temp, arg0);
    } else {
        ((s32 (*)(const char *, ...))fn_802E317C)("UIEnableGroup(): No active window");
    }
}

// UIMoveGroup
extern "C" void fn_80214BE4(s32 group, s32 x, s32 y) {
    s32 temp = fn_802168E4();
    if (temp != 0) {
        fn_802168E0((Window*)temp, group, x, y);
    } else {
        ((s32 (*)(const char *, ...))fn_802E317C)("UIMoveGroup(): No active window");
    }
}

// UIUnfocus / UIRefocus: nothing calls them, so the linker dropped their code and only their messages remain in
// .data (between UIMoveGroup's and UIUpdateAnimeContext's). What they did with the window is unknown; the
// no-window path is all that is kept here.
static void UIUnfocus(void) {
    if (fn_802168E4() == 0) {
        fn_802E317C("UIUnfocus(): No active window");
    }
}

static void UIRefocus(void) {
    if (fn_802168E4() == 0) {
        fn_802E317C("UIRefocus(): No active window");
    }
}

extern "C" void fn_80214C54(s32 arg0, s32 arg1) {
    s32 w = fn_802168E4();
    if (w != 0) {
        fn_80218C14(w, arg0, arg1);
    } else {
        fn_802E317C("UIUpdateAnimeContext: No active Window");
    }
}

extern "C" void fn_80214CB4(s32 arg0) {
    s32 temp = fn_802168E4();
    if (temp != 0) {
        ((s32 (*)(s32, s32))fn_802180E0)(temp, arg0);
    } else {
        ((s32 (*)(const char *, ...))fn_802E317C)("UISetMenuIndex: No active window");
    }
}

extern "C" s32 fn_802168E4(void);
extern "C" s32 fn_80218120(s32);
extern "C" s32 fn_802E317C(const char *, ...);
extern "C" s32 fn_80214D04(void) {
    s32 w = fn_802168E4();
    if (w != 0) {
        return fn_80218120(w);
    }
    fn_802E317C("UIGetMenuIndex: No active window");
    return 0;
}

extern "C" f32 fn_80214D48(void) {
    return lbl_8055EF10;
}

extern "C" void fn_80214D50(s32 arg0) {
    s32 index;
    void *obj;
    float value;
    if (fn_80206430() == 0) return;
    index = lbl_8055EF5C + 1;
    lbl_8055EF5C = index;
    if (index >= 4) { index = 0; lbl_8055EF5C = 0; }
    obj = lbl_80490558[arg0].voices[index];
    if (obj != 0) {
        value = lbl_80490558[arg0].volume * fn_80209028();
        if (value != *(float *)((char *)obj + 8)) {
            *((u8 *)obj + 0x4D) = 1;
            *(float *)((char *)obj + 8) = value;
        }
        *((u8 *)obj + 0x49) = 1;
        *((u8 *)obj + 0x4B) = 0;
        *((u8 *)obj + 0x4A) = 0;
    }
}

extern "C" void fn_80214A9C(s32);
extern "C" void fn_80214AF0(s32);
extern "C" s32 fn_802152CC(void);

// UIOkBox: title + message with a single OK button (group 31 is hidden while it shows).
#pragma push
#pragma global_optimizer off
extern "C" void fn_80214E10(const char* title, const char* message, int (*poll)(int*)) {
    int result = 2;
    const char* caption = "Button - OK";
    Window* active = (Window*)fn_802168E4();
    if (active) {
        fn_80214A9C(0x1F);
    }
    MenuItem items[] = {
        MenuItem("ok", 2, &result, 0, 0),
        MenuItem("ok", 0, NULL, -2, 0),
        MenuItem((const char*)&lbl_8055B860, 6, NULL, -1, 27),
        MenuItem((const char*)&lbl_8055B860, 6, NULL, -2, 13),
        MenuItem("buttonok", caption, 4),
        MenuItem("title", title, 0x20),
        MenuItem("message", message, 4),
        MenuItem(lbl_80490510),
    };
    MenuDesc desc = {"ok.lyt", items, NULL, 0, 0};
    if (!poll) {
        poll = (int (*)(int*))fn_802152CC;
    }
    desc.poll = poll;
    fn_802146BC(&desc, -999, -999, 1);
    if (active) {
        fn_80214AF0(0x1F);
    }
}
#pragma pop

extern "C" void fn_80214FD8(const char* title, const char* message) {
    fn_80214E10(title, message, 0);
}

// Custom handler of the yes/no box: remembers which answer starts selected.
class UIYesNoHandler : public MenuState {  /* Class name unproven */
public:
    UIYesNoHandler(u8 d) { defaultNo = d; }
    virtual ~UIYesNoHandler(void) {}
    virtual void Enter(void);
    virtual int Key(int key);
    u8 defaultNo;  // 0x18
};

// UIYesNoBox: returns 1 for yes.
extern "C" int fn_80214FE0(const char* title, const char* message, int (*poll)(int*), int defaultNo) {
    Window* active = (Window*)fn_802168E4();
    if (active) {
        fn_80214A9C(0x1F);
    }
    UIYesNoHandler handler(defaultNo);
    MenuItem items[] = {
        MenuItem("__first", 5, &handler, 0, 0),
        MenuItem(CaptionContext(4), "title", title),
        MenuItem("message", message, 4),
        MenuItem("yes", 0, NULL, -2, 0),
        MenuItem("no", 0, NULL, -3, 0),
        MenuItem((const char*)&lbl_8055B860, 6, NULL, -3, 27),
        MenuItem(lbl_80490510),
    };
    MenuDesc desc = {"yesno.lyt", items, poll, 0, 0};
    int yes = fn_802146BC(&desc, -999, -999, 1) == -2;
    if (active) {
        fn_80214AF0(0x1F);
    }
    return yes;
}



// 0x802151D4 is MenuState's deleting dtor (the in-class ~MenuState in menustate.h, slot 2 of __vt__9MenuState);
// our compiler emits that weak copy after the last function instead of here.

extern "C" void fn_80215214(const char* title, const char* message, int (*poll)(int*)) {
    fn_80214FE0(title, message, poll, 1);
}

extern "C" void fn_8021521C(const char* title, const char* message, int defaultNo) {
    fn_80214FE0(title, message, 0, defaultNo);
}

// Was the last layout UIBegin/UIDoMappedDialog saw the given one?
extern "C" int fn_80215228(void) { return strcasecmp(lbl_8033CAE8, lbl_8055B6B0) == 0; }

extern "C" void *lbl_8055EF38;
extern "C" void* fn_8021525C(void) {
    return lbl_8055EF38;
}

extern "C" void *fn_80215264(void) {
    return lbl_8033CAE8;
}

// Arrow/scroll button art: [set][direction], sets 0-2 (out-of-range sets fall back to set 1).
extern "C" void* fn_80215270(int dir, int set) {
    if (set >= 3) {
        set = 1;
    }
    return ((void* (*)[4])lbl_80490528)[set][dir];
}

extern "C" void fn_80215298(s32 *arg0, s32 *arg1, s32 *arg2, s32 *arg3, s32 arg4, s32 arg5, s32 arg6) {
    *arg0 = arg4;
    *arg2 = arg4 + arg6 - 0x1A;
    *arg1 = arg4 + arg6 / 2;
    *arg3 = arg5 + 4;
}

extern "C" void fn_802152C8(void) {
}

extern "C" void fn_802152C8(void);
extern "C" s32 fn_802152CC(void) {
    fn_802152C8();
    return 0;
}
UICustom::~UICustom(void) {}

UIHotKey::~UIHotKey(void) {}

void MenuState::Enter(void) {
}

extern "C" s32 fn_802168BC(s32);
int MenuState::Key(int key) {
    return 0;
}

int UIHotKey::Slot64(int p_arg1) {
    UIHotKey* arg0 = this;
    u32 arg1 = (u32)p_arg1;
    s32 (*callback)(s32);
    if (arg1 == arg0->key) {
        callback = (s32 (*)(s32))arg0->action;
        if (callback == 0 || callback((s32)arg0->param) != 0) {
            fn_802168BC((s32)arg0->param);
        }
        return 1;
    }
    return 0;
}

void UIWidget::Slot0C(void) {

}

void UIWidget::Slot10(void) {

}

void UIWidget::Slot14(void) {

}

void UIWidget::Slot28(void) {

}

void UIWidget::Slot30(void) {

}

void UIWidget::Slot34(void) {
    UIWidget* arg0 = this;
    arg0->flag19 = 1;
    if (arg0->flag1a != 0) {
        arg0->dirty = 1;
    }
}

void UIWidget::Slot38(void) {
    UIWidget* arg0 = this;
    arg0->flag19 = 0;
    if (arg0->flag1a != 0) {
        arg0->dirty = 1;
    }
}

void UIWidget::Slot3C(int, int) {

}

void UIWidget::Slot40(int, int) {

}

void UIWidget::Slot44(void) {

}

void UIWidget::Slot48(void) {

}

void UIWidget::Slot4C(int, int) {

}

void UIWidget::Slot58(void) {
    UIWidget* arg0 = this;
    arg0->flag18 = 1;
}

void UIWidget::Slot5C(void) {
    UIWidget* arg0 = this;
    arg0->flag18 = 0;
}

void* UIWidget::Slot60(void) {
    return 0;
}

int UIWidget::Slot68(void) {
    return 0;
}

void UIWidget::Slot74(int, void*) {
    UIWidget* arg0 = this;
    if (arg0->flag1a != 0) {
        arg0->dirty = 1;
    }
}



#ifdef __MWERKS__
int UIImage::Frames(void) { return 1; }
#endif

UIImageRef::~UIImageRef(void) {}

void UIImageRef::Extra(void) {

}

void UIImageRef::DrawAt(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6) {
    UIImageRef* arg0 = this;
    fn_8022BAD0(arg0->flags, arg1, arg2, 0, arg4, arg5, arg0->name, arg6, 0, 1.0f);
}

void UIImageRef::Rect(int arg1, int arg2, int* p_arg3, int* p_arg4, int* p_arg5, int* p_arg6) {
    UIImageRef* arg0 = this;
    s32 arg3 = (s32)p_arg3;
    s32 arg4 = (s32)p_arg4;
    s32 arg5 = (s32)p_arg5;
    s32 arg6 = (s32)p_arg6;
    fn_8022AC1C(arg0->flags, (s32)arg0->name, arg1, arg2, arg3, arg4, arg5, arg6);
}

int UIImageRef::Frames(void) {
    return 4;
}

extern "C" void fn_802C4C80(void*);
StpImage::~StpImage(void) {
    fn_802C4C80((void*)value);
}

extern "C" char* strrchr(const char *, int);
extern "C" s32 fn_801345D8(void *, s32, const char *, ...);
extern "C" unsigned long strlen(const char *);
extern "C" s32 fn_802C4F14(s32);
extern "C" s32 fn_802C4C90(s32, s32, s32, s32, s32);
void StpImage::DrawFrame(int arg1, int arg2, int arg3) {
    StpImage* arg0 = this;
    s32 rem = arg3 % fn_802C4F14(arg0->value);
    fn_802C4C90(arg0->value, arg1, arg2, rem, 0);
}

extern "C" void fn_802C4F2C(int, int *, int *);
extern "C" int fn_802C4EE4(int);
extern "C" int fn_802C4EFC(int);
// Screen rectangle of an image drawn at (x, y), allowing for the image's origin.
void StpImage::Rect(int x, int y, int* p_left, int* p_top, int* p_right, int* p_bottom) {
    StpImage* self = this;
    s32* left = (s32*)p_left;
    s32* top = (s32*)p_top;
    s32* right = (s32*)p_right;
    s32* bottom = (s32*)p_bottom;
    int ox = 0;
    int oy = 0;
    fn_802C4F2C(self->value, &ox, &oy);
    *left = x - ox;
    *top = y - oy;
    *right = *left + fn_802C4EE4(self->value);
    *bottom = *top + fn_802C4EFC(self->value);
}

int StpImage::Frames(void) {
    StpImage* arg0 = this;
    return fn_802C4F14(arg0->value);
}

extern "C" void fn_802C40E8(int, int, int);
extern "C" void fn_802C4F2C(int, int *, int *);
extern "C" int fn_802C4EE4(int);
extern "C" int fn_802C4EFC(int);
extern "C" s32 fn_802C2FC4(s32);
CvsImage::~CvsImage(void) {
    if (value != 0) fn_802C2FC4((s32)value);
}

void CvsImage::DrawFrame(int arg1, int arg2, unsigned int arg3) {
    CvsImage* arg0 = this;
    unsigned int r7 = (unsigned int)arg0->divisor;
    int r8 = (int)arg0->value;
    unsigned int rem = arg3 % r7;
    if (r8 != 0) fn_802C40E8(r8, arg1, arg2 - arg0->count * rem);
}

void CvsImage::Rect(int arg1, int arg2, int* arg3, int* arg4, int* arg5, int* arg6) {
    CvsImage* arg0 = this;
    int var_r0;
    *arg3 = arg1;
    *arg4 = arg2;
    int *temp = (int *)arg0->value;
    var_r0 = temp != 0 ? temp[2] : 0;
    *arg5 = *arg3 + var_r0;
    *arg6 = *arg4 + arg0->count;
}

int CvsImage::Frames(void) {
    CvsImage* arg0 = this;
    return arg0->divisor;
}




#ifdef __MWERKS__
void UIImage::Draw(int x, int y, int a, unsigned int frame) {}

void UIImage::DrawAt(int x, int y, int a, int b, int c, int frame) { Draw(x, y, a, frame); }

int UIImage::Hit(int x, int y, int testX, int testY) {                      // fn_8021583C: tests Rect
    int left, top, right, bottom;
    Rect(x, y, &left, &top, &right, &bottom);
    int result = 0;
    if (testX >= left && testX < right && testY >= top && testY < bottom) result = 1;
    return result;
}
#endif

void UICustom::Slot0C(void) {
    UICustom* arg0 = this;
    VfObj *obj = (VfObj *)arg0->handler;
    obj->vf1();
}

class Vf4Obj {
public:
    virtual ~Vf4Obj();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
};

void UICustom::Slot10(void) {
    UICustom* arg0 = this;
    Vf4Obj *obj = (Vf4Obj *)arg0->handler;
    obj->vf2();
}

void UICustom::Slot14(void) {
    UICustom* arg0 = this;
    VfObj *obj = (VfObj *)arg0->handler;
    obj->vf3();
}

void UICustom::Slot7C(void) {
    UICustom* arg0 = this;
    UIWidget::Slot7C();
    ((VfObj *)(void *)arg0->handler)->vf5();
}

void UICustom::Slot80(void) {
    UICustom* arg0 = this;
    UIWidget::Slot80();
    ((VfObj *)(void *)arg0->handler)->vf6();
}

void UICustom::Slot3C(int x, int y) {
    UICustom* arg0 = this;
    ((VfObj *)(void *)arg0->handler)->vf10(x, y);
}

void UICustom::Slot40(int x, int y) {
    UICustom* arg0 = this;
    ((VfObj *)(void *)arg0->handler)->vf11(x, y);
}

void UICustom::Slot44(void) {
    UICustom* arg0 = this;
    ((VfObj *)(void *)arg0->handler)->vf13();
}

void UICustom::Slot48(void) {
    UICustom* arg0 = this;
    ((VfObj *)(void *)arg0->handler)->vf14();
}

void* UICustom::Slot60(void) {
    UICustom* arg0 = this;
    VfObj *obj = (VfObj *)arg0->handler;
    obj->vf19();
}

void UICustom::Slot74(int, void*) {
    UICustom* arg0 = this;
    VfObj *obj = (VfObj *)arg0->handler;
    obj->vf4();
}

void UICustom::Slot4C(int x, int y) {
    UICustom* arg0 = this;
    VfObj *obj = (VfObj *)arg0->handler;
    obj->vf12(x, y);
}

void UICustom::Slot58(void) {
    UICustom* arg0 = this;
    arg0->flag18 = 1;
    ((VfObj *)(void *)arg0->handler)->vf15();
}

void UICustom::Slot5C(void) {
    UICustom* arg0 = this;
    arg0->flag18 = 0;
    ((VfObj *)(void *)arg0->handler)->vf16();
}

void UICustom::Slot34(void) {
    UICustom* arg0 = this;
    u8 *self = (u8 *)arg0;
    self[0x19] = 1;
    if (self[0x1A]) {
        self[0x234] = 1;
    }
    ((MenuState*)arg0->handler)->Unk4C();
}

void UICustom::Slot38(void) {
    UICustom* arg0 = this;
    u8 *self = (u8 *)arg0;
    self[0x19] = 0;
    if (self[0x1A]) {
        self[0x234] = 1;
    }
    ((MenuState*)arg0->handler)->Unk50();
}
int UICustom::Slot64(int arg1) {
    UICustom* arg0 = this;
    class Callback {
    public:
        virtual ~Callback(); virtual void f1(); virtual void f2(); virtual void f3();
        virtual void f4(); virtual void f5(); virtual void f6(); virtual void f7();
        virtual void f8(); virtual void f9(); virtual void f10(); virtual void f11();
        virtual void f12(); virtual void f13(); virtual void f14(); virtual void f15();
        virtual void f16(); virtual void f17(); virtual void f18(); virtual void f19();
        virtual s32 call(int key);  // 0x58: the handler's MenuState::Key
    };
    Callback *obj = (Callback *)arg0->handler;
    if (obj->call(arg1) != 0) return 1;
    return UIWidget::Slot64(arg1);
}

int UICustom::Slot68(void) {
    UICustom* arg0 = this;
    VfObj *obj = (VfObj *)arg0->handler;
    return obj->vf21();
}

void UICustom::Slot24(void*) {
    UICustom* arg0 = this;
    VfObj *obj = (VfObj *)arg0->handler;
    obj->vf7();
}

void UICustom::Slot28(void) {
    UICustom* arg0 = this;
    VfObj *obj = (VfObj *)arg0->handler;
    obj->vf8();
}

void UICustom::Slot30(void) {
    UICustom* arg0 = this;
    VfObj *obj = (VfObj *)arg0->handler;
    obj->vf9();
}

void UICustom::Slot1C(int arg1, int arg2) {
    UICustom* arg0 = this;
    UIWidget::Slot1C(arg1, arg2);
    ((void **)arg0->handler)[1] = (void *)arg1;
    ((void **)arg0->handler)[2] = (void *)arg2;
}

int UIWidget::Slot18(void) {
    return -1;
}

void UIWidget::Slot2C(void) {

}

void UIWidget::Slot50(void) {

}

void UIWidget::Slot54(void) {

}

int UIWidget::Slot6C(int) {
    return 0;
}

extern "C" void fn_80214CB4(s32);
extern "C" void fn_80230E90(s32);
int UIWidget::Slot70(int arg1, int arg2) {
    UIWidget* arg0 = this;
    s32 var_r6;

    var_r6 = 0;
    if (arg1 >= arg0->left && arg1 < arg0->right &&
        arg2 >= arg0->top && arg2 < arg0->bottom) {
        var_r6 = 1;
    }
    return var_r6;
}

int UIWidget::Slot78(void) {
    UIWidget* arg0 = this;
    return arg0->right - arg0->left;
}

void UIYesNoHandler::Enter(void) {
    UIYesNoHandler* arg0 = this;
    fn_80214CB4(arg0->defaultNo == 0);
}

int UIYesNoHandler::Key(int arg1) {
    switch (arg1) {
    case 0x125:
        fn_80214CB4(0);
        fn_80230E90(0);
        goto done;
    case 0x127:
        fn_80214CB4(1);
        fn_80230E90(0);
        goto done;
    case 0x126:
    case 0x128:
        goto done;
    default:
        return 0;
    }
done:
    return 1;
}

void MenuState::Exit(void) {

}

void MenuState::Unk14(void) {

}

void MenuState::Changed(int key, void* value) {
	fn_80221E7C(this);
}

void MenuState::Update(void) {

}

void MenuState::Unk20(void) {

}

void MenuState::Unk24(gxCanvas*) {

}

void MenuState::Draw(void) {

}

void MenuState::Unk2C(void) {

}

void MenuState::Unk30(void) {

}

void MenuState::Unk34(void) {

}

void MenuState::Unk38(void) {

}

void MenuState::Unk3C(void) {

}

void MenuState::Unk40(void) {

}

void MenuState::Unk44(void) {

}

void MenuState::Unk48(void) {

}

void MenuState::Unk4C(void) {

}

void MenuState::Unk50(void) {

}

int MenuState::Unk54(void) {
    return 0;
}

int MenuState::Unk5C(void) {
    return 0;
}

/* .sbss owned by this unit (0x8055EEF8-0x8055EF60), in address order. */
extern "C" {
int lbl_8055EF08;
s32 (*lbl_8055EF0C)(s32 *);
float lbl_8055EF10;
int lbl_8055EF14;
int lbl_8055EF18;
gArtSet* lbl_8055EF1C;
gArtSet* lbl_8055EF20;
void* lbl_8055EF24;
void* lbl_8055EF28;
UITable* lbl_8055EF2C;
CycleList* lbl_8055EF30;
NameList* lbl_8055EF34;
void * lbl_8055EF38;
int lbl_8055EF3C;
int lbl_8055EF40;
const char* lbl_8055EF44;
int lbl_8055EF48;
int lbl_8055EF4C;
int lbl_8055EF50;
int lbl_8055EF54;
int lbl_8055EF58;
int lbl_8055EF5C;
}

// MenuState slots the hand-built tables reuse (referenced by their linker names).
extern "C" {
void Draw__9MenuStateFv(void);
void Exit__9MenuStateFv(void);
void Unk14__9MenuStateFv(void);
void Changed__9MenuStateFiPv(void);
void Unk20__9MenuStateFv(void);
void Unk24__9MenuStateFP8gxCanvas(void);
void Unk2C__9MenuStateFv(void);
void Unk30__9MenuStateFv(void);
void Unk34__9MenuStateFv(void);
void Unk38__9MenuStateFv(void);
void Unk3C__9MenuStateFv(void);
void Unk40__9MenuStateFv(void);
void Unk44__9MenuStateFv(void);
void Unk48__9MenuStateFv(void);
void Unk4C__9MenuStateFv(void);
void Unk50__9MenuStateFv(void);
void Unk54__9MenuStateFv(void);
void Unk5C__9MenuStateFv(void);
void Update__9MenuStateFv(void);
}

// Virtual tables of the raw-offset handler/widget objects above (vptrs stored by hand), in .data order.
// CLEANUP: become real classes once the slot functions are methods.
extern "C" {
}
