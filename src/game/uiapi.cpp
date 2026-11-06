// UI API layer (0x802115AC-0x80215CF4): UIShowGroup/UIHideGroup/UIMoveGroup/UIGetMenuIndex... on the
// active window, plus menu-state glue. Imported verbatim from the upstream project
// (github.com/orangedude27/excite-truck, CC0), where its functions match exactly.
// CLEANUP: raw offsets and placeholder classes from an automated decompiler; replace with real types
// (Window, Widget, MenuState) before the PC port. Tricks are tagged MATCHING-AID.

#include <types.h>
#include "game/menustate.h"
#include "game/uilists.h"
#include "game/menu.h"

extern "C" s32 fn_80218C14(s32, s32, s32);
extern "C" s32 fn_802E317C(const char *, ...);
extern "C" void fn_80214934(const void *, s32, void *, s32);
extern "C" s32 fn_80029004(const char *, s32);
extern "C" s32 fn_801345D8(void *, s32, const char *, ...);
extern "C" unsigned long strlen(const char *);
extern "C" char lbl_8055B808[8];
extern "C" s32 fn_802C4F14(s32);
extern "C" s32 fn_802C4C90(s32, s32, s32, s32, s32);
extern "C" void fn_8021481C(const char*);
extern "C" char lbl_8033D388[];
extern "C" char lbl_8033D268[];
extern "C" char lbl_8055B810[];

extern "C" s32 fn_802168E4(void);
extern "C" s32 fn_802168DC(s32, s32);
extern "C" s32 fn_802168D4(s32);
extern "C" s32 fn_802168CC(s32, float);
extern "C" s32 fn_802168E0(s32, s32, s32);
extern "C" char lbl_8033D2E0[];
extern "C" char lbl_8033D328[];
extern "C" char lbl_8033D2A0[];
extern "C" char lbl_8033D274[];

extern "C" s32 fn_802180E0(s32, s32);
extern "C" char lbl_8033D3B0[];

extern "C" s32 fn_802168DC(s32, s32);
extern "C" s32 fn_802168D4(s32);
extern "C" s32 fn_802168CC(s32, float);
extern "C" s32 fn_802168E0(s32, s32, s32);
extern "C" char lbl_8033D2E0[];
extern "C" char lbl_8033D328[];
extern "C" char lbl_8033D2A0[];
extern "C" char lbl_8033D274[];

class VfObj {
public:
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void vf5();
    virtual void vf6();
    virtual void vf7();
    virtual void vf8();
    virtual void vf9();     // vtable 0x24
    virtual void vf10();    // vtable 0x28
    virtual void vf11();    // vtable 0x2C
    virtual void vf12();
    virtual void vf13();
    virtual void vf14();
    virtual void vf15();
      virtual void vf16();
  virtual void vf17();
  virtual void vf18();
  virtual void vf19();
  virtual void vf20();
  virtual void vf21();    // vtable 0x5C
};

extern "C" void fn_8021915C(void *, unsigned int);

extern "C" void fn_80221E7C(void);
extern "C" void* fn_80215270(int, int);
extern "C" void fn_8022E4BC(s32);
extern "C" void fn_8022E960(s32);
s32 fn_80211678(u8* arg0);
extern "C" char lbl_8055B750[4];
extern "C" int sprintf(char *, const char *, ...);
extern "C" int stricmp(const char *, const char *);
extern "C" void *memcpy(void *, const void *, unsigned long);
// Name of the last layout UIBegin saw (first .data object of the unit).
extern "C" char lbl_8033CAE8[100];
char lbl_8033CAE8[100] = "__OOPS_NO_LAST_LAYOUT__";
extern "C" void fn_80214E10(const char* title, const char* message, int (*poll)(int*));
extern "C" int fn_80214FE0(const char* title, const char* message, int (*poll)(int*), int defaultNo);
extern "C" s32 fn_80206430(void);
extern "C" s32 fn_8022BAD0(s32, s32, s32, s32, float);
extern "C" s32 fn_8022AC1C(s32, s32, s32, s32, s32, s32, s32, s32);
extern "C" float fn_80209028(void);
extern "C" int lbl_8055EF5C;
extern "C" char *lbl_8055B6B0;

extern "C" s32 fn_80128668(void);
extern "C" s32 (*lbl_8055EF0C)(s32 *);
extern "C" s32 fn_802168BC(s32);
extern "C" void *fn_802115AC(void *arg0, s32 arg1) {
    if (arg0 != 0) {
        if (arg0 != 0) {
            *(s32 *)arg0 = 0;
            *((s32 *)arg0 + 1) = 0;
            *((s32 *)arg0 + 2) = 0;
        }
        if (arg1 > 0)
            ((void (*)(void *))fn_80128668)(arg0);
    }
    return arg0;
}

extern "C" void *fn_80211604(void *arg0, s32 arg1) {
    if (arg0 != 0) {
        if (arg0 != 0) {
            *(s32 *)arg0 = 0;
            *((s32 *)arg0 + 1) = 0;
            *((s32 *)arg0 + 2) = 0;
        }
        if (arg1 > 0)
            ((void (*)(void *))fn_80128668)(arg0);
    }
    return arg0;
}

extern "C" void* fn_8021165C(int dir, int set) {
    return fn_80215270(dir, set);
}

extern s32 lbl_8055EF1C;
extern s32 lbl_8055EF20;
// The lite-mode blinker art (0 = left, else right).
s32 fn_80211660(s32 arg0) {
    if (arg0 == 0) {
        return lbl_8055EF1C;
    } else {
        return lbl_8055EF20;
    }
}

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
struct UISoundSlot {
    const char* bank;
    float volume;
    UIVoice* voices[4];
};
class UITable {
public:
    UITable(int rows, int cols, const int* widths);
    ~UITable(void);
    // The callers pass the flag unconverted (no extsb), so it is an int at the call site; uitable.cpp
    // still declares it char (same callee code, symbol name kept for objdiff pairing).
    void Set(int row, int col, const char* s, int flag);

    char* buf;      // 0x00
    s32 cols;       // 0x04
    s32 used;       // 0x08
    s32 rows;       // 0x0C
    s32 visible;    // 0x10
    s32 rowSize;    // 0x14
    s32* widths;    // 0x18
    s32* offsets;   // 0x1C
    s32 page;       // 0x20
    s32 changes;    // 0x24
    u32 magic;      // 0x28
    s32 sel;        // 0x2C
};
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
extern "C" void fn_80235380(s32);
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
extern "C" s32 fn_8023512C(const char*);
extern "C" void fn_8022A5E4(void);
extern "C" void fn_80216710(void);
extern "C" void* fn_8022D228(void*, int, int);
extern "C" void fn_8022D350(void*, const char*, int);
extern "C" UIVoice* fn_80207848(const char*, int);
extern "C" int lbl_8055EF08;
extern "C" int lbl_8055EF3C;
extern "C" void* lbl_8055EF24;
extern "C" void* lbl_8055EF28;
extern void *lbl_8055EF38;
void LoadTimerProgress(void);
void LoadTimerEnd(const char* name, int);
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
            lbl_8055EF2C->Set(i, 0, entry, i % 3);
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
        LoadTimerEnd("-UIBegin", 0);
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

extern "C" void *fn_801284F8(s32, const char *);
extern "C" s32 fn_8012C33C(s32);
extern "C" s32 fn_802C49E8(s32);
extern "C" s32 fn_802C2DD4(s32);
extern "C" void *fn_802228B0(void *, const char *, s32, s32);
extern "C" void *fn_802226D4(void *, const char *);
extern "C" void *fn_8021203C(char *arg0, s32 arg1, s32 arg2, s32 arg3) {
    extern int lbl_8033D528[];
    extern char lbl_8033D504[];
    extern const char lbl_8055B720[4], lbl_8055B724[4], lbl_8055B728[5], lbl_8055B730[5];
    extern const char lbl_8055B738[4], lbl_8055B73C[5], lbl_8055B744[4], lbl_8055B748[5];
    struct ObjA { void *vtable; void *value; };
    struct ObjB { void *vtable; void *value; s32 count; s32 divisor; };
    char *name = (char *)fn_80029004(arg0, 0x2e);
    ObjA *a;
    ObjB *b;
    void *c;
    if (stricmp(name + 1, lbl_8055B724) == 0) {
        a = (ObjA *)fn_801284F8(8, lbl_8055B728);
        if (a) { a->vtable = lbl_8033D528; a->value = (void *)fn_802C49E8((s32)arg0); }
        return a;
    }
    if (stricmp(name + 1, lbl_8055B720) == 0) {
        b = (ObjB *)fn_801284F8(16, lbl_8055B730);
        if (b) {
            b->vtable = lbl_8033D504;
            if (fn_8012C33C((s32)arg0)) {
                c = (void *)fn_802C2DD4((s32)arg0); b->value = c;
                b->count = *(s32 *)((char *)c + 12) / arg1;
            } else { b->value = 0; b->count = 0; }
            b->divisor = arg1;
        }
        return b;
    }
    if (stricmp(name + 1, lbl_8055B738) == 0) {
        c = fn_801284F8(16, lbl_8055B73C);
        if (c) c = fn_802228B0(c, arg0, arg2, arg3);
        return c;
    }
    if (stricmp(name + 1, lbl_8055B744) == 0) {
        c = fn_801284F8(8, lbl_8055B748);
        if (c) c = fn_802226D4(c, arg0);
        return c;
    }
    fn_802E317C("make_image: unknown extension %s", name + 1);
    return 0;
}

extern "C" s32 fn_80128668(void);
extern "C" s32 fn_802121D4(s32 arg0, s32 arg1) {
    if (arg0 != 0 && arg1 > 0) {
        fn_80128668();
    }
    return arg0;
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
extern "C" char lbl_8055B750[4];
extern "C" int sprintf(char *, const char *, ...);
extern "C" int stricmp(const char *, const char *);
extern "C" void *fn_8021227C(void *arg0, char *arg1, s32 arg2) {
    char buf[0x100];
    void *entry = arg0;
    char *name = arg1;
    if (*arg1 == '#') {
        name = arg1 + 1;
        sprintf(buf, lbl_8055B750, name);
    }
    for (;;) {
        s32 id = *(s32 *)((char *)entry + 4);
        if (id == 0x7B) return 0;
        if (id == arg2 && *(char **)entry != 0 &&
            (stricmp(*(char **)entry, name) == 0 ||
             stricmp(*(char **)entry, buf) == 0)) return entry;
        entry = (char *)entry + 0x14;
    }
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

extern "C" void* lbl_8033D54C[];
extern "C" void* lbl_8033D480[];
extern "C" void* lbl_8033D590[];
extern "C" UnkStatic123 lbl_80490510;  // this unit's 0x7B end-of-list item
extern "C" f32 lbl_80558DC4[];  // FLT_EPSILON
extern "C" void* lbl_8055EF24;
extern "C" void* lbl_8055EF28;
extern void *lbl_8055EF38;
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
extern "C" void fn_802E30C8(const char *, ...);
extern "C" int fn_80129BC4(void);
extern "C" unsigned int fn_8022D1B4(const char*);
extern "C" void fn_802168EC(void*, WidgetFragment*, int);
extern "C" void fn_80215298(s32 *, s32 *, s32 *, s32 *, s32, s32, s32);
extern "C" WidgetFragment* fn_8021B2B0(WidgetFragment*, int, int, const char*, int, void*, int);
extern "C" WidgetFragment* fn_8021D044(WidgetFragment*, int, int, void*, const char*, int, int, int, int);
extern "C" WidgetFragment* fn_8021D6A0(WidgetFragment*, int, int, void*, void*, const char*, int, int, int);
extern "C" WidgetFragment* fn_8021B460(WidgetFragment*, UIDesc*, const char*, void*);
extern "C" void* fn_80222530(void*, const char*, const char*);
extern "C" WidgetFragment* fn_8021995C(WidgetFragment*, int, int, int, void*, void*);
extern "C" WidgetFragment* fn_8021B9AC(WidgetFragment*, UIDesc*, void*, void*);
extern "C" WidgetFragment* fn_802224B4(WidgetFragment*, void*, int, int, int, int, void*, void*, int, int);
extern "C" WidgetFragment* fn_80226630(WidgetFragment*, void*, int, int, int, int, void*, void*);
extern "C" WidgetFragment* fn_8022435C(WidgetFragment*, void*, UIDesc*, void*, void*);
extern "C" WidgetFragment* fn_8021A5B8(WidgetFragment*, const char*, u8, u8, int, float);
extern "C" WidgetFragment* fn_80219A28(WidgetFragment*, UIDesc*, const char*, void*);
extern "C" WidgetFragment* fn_80219870(WidgetFragment*, int, int, const char*, void*, int);
extern "C" WidgetFragment* fn_8021A77C(WidgetFragment*, int, int, int, void*, void*);
extern "C" WidgetFragment* fn_8021B0CC(WidgetFragment*, int, int, void*, void*, void*, int, void*);
extern "C" WidgetFragment* fn_80220778(WidgetFragment*, int, int, int, int, void*, void*, void*);
extern "C" WidgetFragment* fn_8021F678(WidgetFragment*, int, int, int, int, float, float, void*, int);
extern "C" WidgetFragment* fn_8021AF0C(WidgetFragment*, int, int, void*, int, int, void*, float, float);
extern "C" WidgetFragment* fn_8021BD98(WidgetFragment*, int, int, void*, const char*, int, float, float);
extern "C" WidgetFragment* fn_8021CEEC(WidgetFragment*, int, int, void*, const char*, int, float, float);
extern "C" WidgetFragment* fn_8021BEC0(WidgetFragment*, int, int, void*, int, int, int, int);
extern "C" WidgetFragment* fn_8021CA0C(WidgetFragment*, int, int, int, int, void*);
extern "C" WidgetFragment* fn_8021C97C(WidgetFragment*, int, int, void*, int, void*);
extern "C" WidgetFragment* fn_8021C344(WidgetFragment*, int, int, void*, int, int, int, int, const char*, int);
extern "C" WidgetFragment* fn_8021D7C8(WidgetFragment*, int, int, int, int, int, void*);
extern "C" WidgetFragment* fn_8021A808(WidgetFragment*, int, int, int, void*, void*);
extern "C" WidgetFragment* fn_8021EA64(WidgetFragment*, int, int, int, int, void*, void*, int, int, int);
extern "C" WidgetFragment* fn_80218F34(WidgetFragment*, int, int, int, int, int);
extern "C" WidgetFragment* fn_80222BDC(WidgetFragment*, UIDesc*, void*);
extern "C" WidgetFragment* fn_8021E0C0(WidgetFragment*, int, int, int, int, void*, void*, void*);
extern "C" WidgetFragment* fn_80220E48(WidgetFragment*, int, int, int, int, void*, int);
extern "C" WidgetFragment* fn_8021B1C4(WidgetFragment*, int, int, void*, int, void*);
extern "C" WidgetFragment* fn_8021F394(WidgetFragment*, int, int, const char*, void*, int);
extern "C" WidgetFragment* fn_802230A4(WidgetFragment*, int, int, void*, void*);
extern "C" WidgetFragment* fn_80223800(WidgetFragment*, int, int, void*, void*);
extern "C" WidgetFragment* fn_80223AB4(WidgetFragment*, UIDesc*, void*);
extern "C" WidgetFragment* fn_80223CF0(WidgetFragment*, UIDesc*, void*);
extern "C" WidgetFragment* fn_8022886C(WidgetFragment*, UIDesc*, void*, void*);
extern "C" WidgetFragment* fn_802286E4(WidgetFragment*, UIDesc*, void*);
extern "C" WidgetFragment* fn_802241C8(WidgetFragment*, UIDesc*);
extern "C" WidgetFragment* fn_80224B54(WidgetFragment*, void*, UIDesc*);
extern "C" WidgetFragment* fn_8022553C(WidgetFragment*, void*, UIDesc*);
extern "C" WidgetFragment* fn_802269A8(WidgetFragment*, UIDesc*);
extern "C" WidgetFragment* fn_80226CBC(WidgetFragment*, UIDesc*);
extern "C" WidgetFragment* fn_802270DC(WidgetFragment*, UIDesc*);
extern "C" WidgetFragment* fn_80227E84(WidgetFragment*, void*, UIDesc*, void*);
extern "C" WidgetFragment* fn_80227D00(WidgetFragment*, UIDesc*);
extern "C" WidgetFragment* fn_8021FF34(WidgetFragment*, UIDesc*, void*, int, float, float);
extern "C" WidgetFragment* fn_80228418(WidgetFragment*, UIDesc*, void*);
extern "C" WidgetFragment* fn_8022855C(WidgetFragment*, UIDesc*, void*, void*);

// Image/text reference handed to buttons ("StyleImage", "rimg", "timg").
struct UIImageRef {
    void* vtable;
    const char* name;
    int flags;
    UIImageRef(const char* n, int f) {
        vtable = lbl_8033D54C;
        flags = f;
        name = n;
    }
};

// Handler object a "Custom" widget reports its rectangle to.
struct UICustomHandler {
    void* vtable;
    int x, y, dx, dy;
    void* owner;        // 0x14
};
struct UICustom {
    u8 base[0x244];
    UICustomHandler* handler;
    UICustom(int x, int y, int dx, int dy, int kind, UICustomHandler* h) {
        fn_80218F34((WidgetFragment*)this, x, y, x + dx, y + dy, kind);
        *(void**)this = lbl_8033D480;
        handler = h;
        h->owner = this;
        h->x = x;
        h->y = y;
        h->dx = dx;
        h->dy = dy;
    }
};

static u8 sUiBss0[0x160];
static u8 sDefaultSpin[0x2c];
static u8 sDefaultMulti[0x14];
static char sDefaultInput[0x40];
static u8 sDefaultControl[0x20];
static UICustomHandler sCustomHandlers[16];
static u8 sDefaultView[0x20];

static inline UIStyle* FindStyle(void* styles, const char* name, s32 kind) {
    return (UIStyle*)fn_8021227C(styles, (char*)name, kind);
}

extern "C" void fn_802123B8(void* window, UIDesc* d, void* styles) {
    int count = 0;
    WidgetFragment* widgets[4] = {0, 0, 0, 0};
    WidgetFragment* p;
    lbl_8055EF38 = (void*)d->group;
    switch (d->type) {
    case 5: {
        UIStyle* text = FindStyle(styles, d->style, 3);
        if (text) {
            UIStyle* value = FindStyle(styles, d->style, 2);
            p = (WidgetFragment*)operator new(0x368, "StaticText");
            if (p) p = fn_8021B2B0(p, d->x, d->y, (const char*)text->value, d->flags, value ? value->value : 0, 0);
            widgets[count++] = p;
            break;
        }
        UIStyle* multi = FindStyle(styles, d->style, 7);
        if (multi) {
            p = (WidgetFragment*)operator new(0x2c4, "Multi");
            if (p) p = fn_8021D044(p, d->x, d->y, multi->value, (const char*)multi->arg1, d->flags, 0, 0, 0);
            widgets[count++] = p;
            break;
        }
        UIStyle* value = FindStyle(styles, d->style, 2);
        p = (WidgetFragment*)operator new(0x368, "StaticText");
        if (p) p = fn_8021B2B0(p, d->x, d->y, d->name, d->flags, value ? value->value : 0, 0);
        widgets[count++] = p;
        break;
    }
    case 28: {
        UIStyle* value;
        UIStyle* text;
        if (d->name[0] == '#') {
            text = FindStyle(styles, d->name + 1, 3);
            if (!text) {
                text = FindStyle(styles, d->name + 1, 7);
                if (text) {
                    int first = fn_80212214(d->field9c);
                    int second = fn_80212214(d->fielda0);
                    p = (WidgetFragment*)operator new(0x2c8, "CustomMulti");
                    if (p) p = fn_8021D6A0(p, d->x, d->y, text->value, text->arg1, d->style, d->flags, first, second);
                    widgets[count++] = p;
                    break;
                }
            }
            value = FindStyle(styles, d->name + 1, 2);
        } else {
            text = 0;
        }
        if (!text || !text->value || !strstr((const char*)text->value, ".gtx")) {
            p = (WidgetFragment*)operator new(0x374, "CustomText");
            if (p) p = fn_8021B460(p, d, (text && text->value) ? (const char*)text->value : d->name, value ? value->value : 0);
            widgets[count++] = p;
            break;
        }
        void* image = operator new(0xc, "gTextImage");
        if (image) image = fn_80222530(image, d->style, (const char*)text->value);
        p = (WidgetFragment*)operator new(0x258, "gText");
        if (p) p = fn_8021995C(p, d->x, d->y, d->kind, image, 0);
        widgets[count++] = p;
        break;
    }
    case 31: {
        UIStyle* column = FindStyle(styles, d->name, 0xd);
        p = (WidgetFragment*)operator new(0x264, "Column");
        if (p) p = fn_8021B9AC(p, d, column ? column->value : (void*)lbl_8055EF2C, column ? column->arg1 : 0);
        widgets[count++] = p;
        break;
    }
    case 2: {
        UIStyle* button = FindStyle(styles, d->style, 0);
        UIStyle* text = d->name[0] == '#' ? FindStyle(styles, d->name + 1, 3) : 0;
        void* image;
        if (button) {
            image = button->arg1;
        } else {
            image = 0;
        }
        const char* label = text ? (const char*)text->value : d->name;
        void* handler = button ? button->value : (void*)fn_80212338;
        UIImageRef* ref = new ("StyleImage") UIImageRef(label, d->flags);
        if (d->field9c == 1 || !fn_80129BC4()) {
            p = (WidgetFragment*)operator new(0x260, "IButton");
            if (p) p = fn_802224B4(p, image, d->x, d->y, d->dx, d->dy, ref, handler, 0, d->field9c != 0);
            widgets[count++] = p;
            break;
        }
        p = (WidgetFragment*)operator new(0x268, "MenuBtn");
        if (p) p = fn_80226630(p, image, d->x, d->y, d->dx, d->dy, ref, handler);
        widgets[count++] = p;
        break;
    }
    case 42: {
        UIStyle* button = FindStyle(styles, d->style, 0);
        UIStyle* value = FindStyle(styles, d->style, 2);
        void* image;
        if (button) {
            image = button->arg1;
        } else {
            image = 0;
        }
        void* handler;
        if (button) {
            handler = button->value;
        } else {
            handler = (void*)fn_80212338;
        }
        void* watched = value ? value->value : 0;
        p = (WidgetFragment*)operator new(0x28c, "FadeButton");
        if (p) p = fn_8022435C(p, image, d, handler, watched);
        widgets[count++] = p;
        break;
    }
    case 3:
        fn_802E30C8("BButtons aren't supported anymore.  If you want one, talk to Chris");
        break;
    case 34:
        p = (WidgetFragment*)operator new(0x258, "BackWidget");
        if (p) p = fn_8021A5B8(p, d->name, 1, 1, 1, d->kind);
        widgets[count++] = p;
        break;
    case 17:
        fn_802E30C8("Stamprolls are not supported anymore, talk to Chris if you think you want one.");
    case 33: {
        int useStyle = d->flags == 2 || d->flags == 4 || d->flags == 5;
        UIStyle* value = FindStyle(styles, d->style, 2);
        UIStyle* text = FindStyle(styles, d->style, 3);
        const char* path = text ? (const char*)text->value : d->name;
        void* watched = useStyle ? (void*)d->style : (value ? value->value : 0);
        if (strstr(path, ".img")) {
            p = (WidgetFragment*)operator new(0x258, "imgstamproll");
            if (p) {
                void* image = operator new(0x10, "imgimg");
                if (image) image = fn_802228B0(image, path, d->dx, d->dy);
                p = fn_8021995C(p, d->x, d->y, d->kind, image, watched);
            }
            widgets[count++] = p;
            break;
        }
        p = (WidgetFragment*)operator new(0x284, "ArtWidget");
        if (p) p = fn_80219A28(p, d, path, watched);
        widgets[count++] = p;
        break;
    }
    case 13: {
        UIStyle* flag = FindStyle(styles, d->style, 1);
        p = (WidgetFragment*)operator new(0x358, "CheckBox");
        if (p) p = fn_80219870(p, d->x, d->y, d->name, flag ? flag->value : &lbl_8055EF48, d->flags);
        widgets[count++] = p;
        break;
    }
    case 14: {
        UIStyle* value = FindStyle(styles, d->style, 2);
        p = (WidgetFragment*)operator new(0x25c, "RadioButton");
        if (p) {
            void* watched = value ? value->value : &lbl_8055EF4C;
            p = fn_8021A77C(p, d->x, d->y, d->field9c, watched, new ("rimg") UIImageRef(d->name, d->flags));
        }
        widgets[count++] = p;
        break;
    }
    case 15: {
        UIStyle* value = FindStyle(styles, d->style, 2);
        p = (WidgetFragment*)operator new(0x25c, "BRadioButton");
        if (p) {
            void* watched = value ? value->value : &lbl_8055EF4C;
            p = fn_8021A77C(p, d->x, d->y, d->field9c, watched, fn_8021203C(d->name, 2, -1, -1));
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
        p = (WidgetFragment*)operator new(0x264, "IntArrowBtn");
        if (p) p = fn_8021B0CC(p, d->x, d->y + 1, value, low, high, -1, fn_80215270(0, 1));
        widgets[count++] = p;
        if (d->dx - 37 > 10) {
            p = (WidgetFragment*)operator new(0x264, "IntSlider");
            if (p) p = fn_80220778(p, d->x + 21, d->y, d->dx - 37, d->dy, value, low, high);
            widgets[count++] = p;
        }
        p = (WidgetFragment*)operator new(0x264, "IntArrowBtn");
        if (p) p = fn_8021B0CC(p, d->x + d->dx - 14, d->y + 1, value, low, high, 1, fn_80215270(1, 1));
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
        if ((f32)__fabs(low - high) <= lbl_80558DC4[0]) {
            break;
        }
        if (!d->field9c) {
            break;
        }
        if (d->dx - 37 > 10) {
            p = (WidgetFragment*)operator new(0x27c, "Slider");
            if (p) p = fn_8021F678(p, d->x + 21, d->y, d->dx - 37, d->dy, low, high,
                                   range ? range->value : &lbl_8055EF50, d->field9c);
            widgets[count++] = p;
        }
        p = (WidgetFragment*)operator new(0x268, "ArrowBtn");
        if (p) {
            void* value = range ? range->value : &lbl_8055EF50;
            p = fn_8021AF0C(p, d->x, d->y + 1, value, abs((int)d->field9c), -1, fn_80215270(0, 1), low, high);
        }
        widgets[count++] = p;
        p = (WidgetFragment*)operator new(0x268, "ArrowBtn");
        if (p) {
            void* value = range ? range->value : &lbl_8055EF50;
            p = fn_8021AF0C(p, d->x + d->dx - 14, d->y + 1, value, abs((int)d->field9c), 1, fn_80215270(1, 1), low, high);
        }
        widgets[count++] = p;
        break;
    }
    case 6: {
        UIStyle* value = FindStyle(styles, d->style, 9);
        p = (WidgetFragment*)operator new(0x2b4, "Numeric");
        if (p) p = fn_8021BD98(p, d->x, d->y, value ? value->value : &lbl_8055EF50, d->name, d->flags, 100.0f, 0.0f);
        widgets[count++] = p;
        break;
    }
    case 7: {
        UIStyle* value = FindStyle(styles, d->style, 2);
        p = (WidgetFragment*)operator new(0x2b4, "IntNumeric");
        if (p) p = fn_8021CEEC(p, d->x, d->y, value ? value->value : &lbl_8055EF4C, d->name, d->flags, 1.0f, 0.0f);
        widgets[count++] = p;
        break;
    }
    case 9: {
        UIStyle* spin = FindStyle(styles, d->style, 0xc);
        void* target = spin ? spin->value : (void*)sDefaultSpin;
        s32 left, x, right, line;
        switch (d->flags) {
        case 0:
        default:
            fn_80215298(&left, &x, &right, &line, d->x, d->y, d->dx);
            p = (WidgetFragment*)operator new(0x2bc, "SpinCtl");
            if (p) p = fn_8021BEC0(p, x, d->y, target, 15, left, right, line);
            widgets[count++] = p;
            break;
        case 1:
            fn_80215298(&left, &x, &right, &line, d->x, d->y, d->dx);
            p = (WidgetFragment*)operator new(0x2bc, "SpinCtl");
            if (p) p = fn_8021BEC0(p, x, d->y, target, 20, left, right, line);
            widgets[count++] = p;
            break;
        case 4:
            p = (WidgetFragment*)operator new(0x25c, "SpinSldr");
            if (p) p = fn_8021CA0C(p, d->x + 15, d->y, d->dx - 30, d->dy, target);
            widgets[count++] = p;
            p = (WidgetFragment*)operator new(0x25c, "SpinArrow");
            if (p) p = fn_8021C97C(p, d->x + 1, d->y + 2, fn_80215270(0, 2), -1, target);
            widgets[count++] = p;
            p = (WidgetFragment*)operator new(0x25c, "SpinArrow");
            if (p) p = fn_8021C97C(p, d->x + d->dx - 15, d->y + 2, fn_80215270(1, 2), 1, target);
            widgets[count++] = p;
            break;
        case 5:
            fn_80215298(&left, &x, &right, &line, d->x, d->y, d->dx);
            p = (WidgetFragment*)operator new(0x2d4, "SpinText");
            if (p) p = fn_8021C344(p, x, d->y, target, 15, left, right, line, d->name, d->field9c);
            widgets[count++] = p;
            break;
        }
        break;
    }
    case 49: {
        UIStyle* list = FindStyle(styles, d->style, 0x10);
        s32 left, x, right, line;
        fn_80215298(&left, &x, &right, &line, d->x, d->y, d->dx);
        p = (WidgetFragment*)operator new(0x26c, "MutableMulti");
        if (p) p = fn_8021D7C8(p, d->x, d->y, d->dx, d->dy, d->flags,
                               list ? list->arg1 : (void*)sDefaultMulti);
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
            p = (WidgetFragment*)operator new(0x2c4, "Multi");
            if (p) p = fn_8021D044(p, x, d->y, value, list, d->flags, left, right, line);
            widgets[count++] = p;
            break;
        }
        p = (WidgetFragment*)operator new(0x2c4, "Multi");
        if (p) p = fn_8021D044(p, d->x, d->y, value, list, d->flags, 0, 0, 0);
        widgets[count++] = p;
        break;
    }
    case 16: {
        UIStyle* value = FindStyle(styles, d->style, 2);
        p = (WidgetFragment*)operator new(0x25c, "TabButton");
        if (p) {
            void* watched = value ? value->value : &lbl_8055EF54;
            p = fn_8021A808(p, d->x, d->y, d->field9c, watched, new ("timg") UIImageRef(d->name, d->flags));
        }
        widgets[count++] = p;
        break;
    }
    case 27: {
        UIStyle* value = FindStyle(styles, d->style, 2);
        p = (WidgetFragment*)operator new(0x25c, "BTabButton");
        if (p) {
            void* watched = value ? value->value : &lbl_8055EF54;
            p = fn_8021A808(p, d->x, d->y, d->field9c, watched, fn_8021203C(d->name, 1, -1, -1));
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
        p = (WidgetFragment*)operator new(0xa64, "Input");
        if (p) p = fn_8021EA64(p, d->x, d->y, d->dx, 10, buffer, capacity, 1, d->flags, 0);
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
                         &sCustomHandlers[lbl_8055EF58]);
        }
        break;
    }
    case 35: {
        UIStyle* list = FindStyle(styles, d->style, 0xa);
        p = (WidgetFragment*)operator new(0x258, "CircleList");
        if (p) p = fn_80222BDC(p, d, list ? list->value : lbl_8055EF24);
        widgets[count++] = p;
        break;
    }
    case 23: {
        UIStyle* selected = FindStyle(styles, d->style, 2);
        UIStyle* list = FindStyle(styles, d->style, 0xa);
        UIStyle* view = FindStyle(styles, d->style, 0xb);
        p = (WidgetFragment*)operator new(0x26c, "ListBox");
        if (p) p = fn_8021E0C0(p, d->x, d->y, d->dx, d->dy, list ? list->value : lbl_8055EF24,
                               selected ? selected->value : &lbl_8055EF4C,
                               view ? view->value : (void*)sDefaultView);
        widgets[count++] = p;
        break;
    }
    case 20: {
        UIStyle* view = FindStyle(styles, d->style, 0xb);
        p = (WidgetFragment*)operator new(0x260, "VScrollBar");
        if (p) p = fn_80220E48(p, d->x - 3, d->y + 21, d->dx, d->dy - 40,
                               view ? view->value : (void*)sDefaultView, 0);
        widgets[count++] = p;
        p = (WidgetFragment*)operator new(0x25c, "ScrollBtn");
        if (p) {
            void* state;
            if (view) {
                state = view->value;
            } else {
                state = (void*)sDefaultView;
            }
            p = fn_8021B1C4(p, d->x, d->y, state, -1, fn_80215270(2, 0));
        }
        widgets[count++] = p;
        p = (WidgetFragment*)operator new(0x25c, "ScrollBtn");
        if (p) {
            void* state;
            if (view) {
                state = view->value;
            } else {
                state = (void*)sDefaultView;
            }
            p = fn_8021B1C4(p, d->x, d->y + d->dy - 18, state, 1, fn_80215270(3, 0));
        }
        widgets[count++] = p;
        break;
    }
    case 21: {
        UIStyle* view = FindStyle(styles, d->style, 0xb);
        p = (WidgetFragment*)operator new(0x260, "HScrollBar");
        if (p) p = fn_80220E48(p, d->x + 21, d->y, d->dx - 40, d->dy,
                               view ? view->value : (void*)sDefaultView, 1);
        widgets[count++] = p;
        p = (WidgetFragment*)operator new(0x25c, "ScrollBtn");
        if (p) {
            void* state;
            if (view) {
                state = view->value;
            } else {
                state = (void*)sDefaultView;
            }
            p = fn_8021B1C4(p, d->x, d->y + 3, state, -1, fn_80215270(0, 0));
        }
        widgets[count++] = p;
        p = (WidgetFragment*)operator new(0x25c, "ScrollBtn");
        if (p) {
            void* state;
            if (view) {
                state = view->value;
            } else {
                state = (void*)sDefaultView;
            }
            p = fn_8021B1C4(p, d->x + d->dx - 18, d->y + 3, state, 1, fn_80215270(1, 0));
        }
        widgets[count++] = p;
        break;
    }
    case 12: {
        UIStyle* control = FindStyle(styles, d->style, 4);
        p = (WidgetFragment*)operator new(0x274, "CDetect");
        if (p) p = fn_8021F394(p, d->x, d->y, control ? (const char*)control->arg1 : "Unmapped control",
                               control ? control->value : (void*)sDefaultControl, 6);
        widgets[count++] = p;
        break;
    }
    case 36: {
        UIStyle* selected = FindStyle(styles, d->style, 2);
        UIStyle* list = FindStyle(styles, d->style, 0xa);
        p = (WidgetFragment*)operator new(0x420, "BottomBar");
        if (p) p = fn_802230A4(p, d->x, d->y, list ? list->value : lbl_8055EF28,
                               selected ? selected->value : &lbl_8055EF4C);
        widgets[count++] = p;
        break;
    }
    case 37: {
        UIStyle* selected = FindStyle(styles, d->style, 2);
        UIStyle* list = FindStyle(styles, d->style, 0xa);
        p = (WidgetFragment*)operator new(0x424, "BarTab");
        if (p) p = fn_80223800(p, d->x, d->y, list ? list->value : lbl_8055EF28,
                               selected ? selected->value : &lbl_8055EF54);
        widgets[count++] = p;
        break;
    }
    case 38: {
        UIStyle* menu = FindStyle(styles, d->style, 0xe);
        p = (WidgetFragment*)operator new(0x268, "MenuImage");
        if (p) p = fn_80223AB4(p, d, menu ? menu->value : (void*)lbl_8055EF30);
        widgets[count++] = p;
        break;
    }
    case 39: {
        UIStyle* movie = FindStyle(styles, d->style, 3);
        if (movie) {
            p = (WidgetFragment*)operator new(0x268, "MovieWidget");
            if (p) p = fn_80223CF0(p, d, movie->value);
            widgets[count++] = p;
        } else {
            p = (WidgetFragment*)operator new(0x268, "MovieWidget");
            if (p) p = fn_80223CF0(p, d, 0);
            widgets[count++] = p;
        }
        break;
    }
    case 54: {
        UIStyle* model = FindStyle(styles, d->style, 0x13);
        UIStyle* name = FindStyle(styles, d->style, 3);
        if (model) {
            p = (WidgetFragment*)operator new(0x280, "ModelWidget");
            if (p) p = fn_8022886C(p, d, model->value, name ? name->value : 0);
            widgets[count++] = p;
        } else {
            p = (WidgetFragment*)operator new(0x280, "ModelWidget");
            if (p) p = fn_802286E4(p, d, name ? name->value : 0);
            widgets[count++] = p;
        }
        break;
    }
    case 41:
        p = (WidgetFragment*)operator new(0x258, "DynamicLoadBackground");
        if (p) p = fn_802241C8(p, d);
        widgets[count++] = p;
        break;
    case 43: {
        UIStyle* text = FindStyle(styles, d->style, 3);
        p = (WidgetFragment*)operator new(0x2a0, "TextBox");
        if (p) p = fn_80224B54(p, text ? text->value : 0, d);
        widgets[count++] = p;
        break;
    }
    case 44: {
        UIStyle* menu = FindStyle(styles, d->style, 0xe);
        p = (WidgetFragment*)operator new(0x284, "ButtonList");
        if (p) p = fn_8022553C(p, menu ? menu->value : (void*)lbl_8055EF30, d);
        widgets[count++] = p;
        break;
    }
    case 45:
        p = (WidgetFragment*)operator new(0x260, "RotImage");
        if (p) p = fn_802269A8(p, d);
        widgets[count++] = p;
        break;
    case 46:
        p = (WidgetFragment*)operator new(0x264, "ControlButton");
        if (p) p = fn_80226CBC(p, d);
        widgets[count++] = p;
        break;
    case 47:
        p = (WidgetFragment*)operator new(0x274, "ColorBackground");
        if (p) p = fn_802270DC(p, d);
        widgets[count++] = p;
        break;
    case 48: {
        UIStyle* tabs = FindStyle(styles, d->style, 0xf);
        UIStyle* text = FindStyle(styles, d->style, 3);
        void* first = tabs ? tabs->value : (void*)lbl_8055EF34;
        p = (WidgetFragment*)operator new(0x25c, "TopTabDisplay");
        if (p) p = fn_80227E84(p, first, d, text ? text->value : 0);
        widgets[count++] = p;
        break;
    }
    case 51:
        p = (WidgetFragment*)operator new(0x248, "TabBox");
        if (p) p = fn_80227D00(p, d);
        widgets[count++] = p;
        break;
    case 50: {
        UIStyle* range = FindStyle(styles, d->style, 9);
        UIStyle* steps = FindStyle(styles, d->style, 2);
        p = (WidgetFragment*)operator new(0x268, "SimpleSlider");
        if (p) p = fn_8021FF34(p, d, range ? range->value : 0, steps ? *(int*)steps->value : 10,
                               range ? fn_80212248((s32)range->arg1) : 0.0f,
                               range ? fn_80212248((s32)range->arg2) : 1.0f);
        widgets[count++] = p;
        break;
    }
    case 52: {
        UIStyle* movie = FindStyle(styles, d->style, 0x11);
        p = (WidgetFragment*)operator new(0x24c, "UISW2Movie");
        if (p) p = fn_80228418(p, d, movie ? movie->value : 0);
        widgets[count++] = p;
        break;
    }
    case 53: {
        UIStyle* texture = FindStyle(styles, d->style, 0x12);
        UIStyle* frame = FindStyle(styles, d->style, 2);
        p = (WidgetFragment*)operator new(0x24c, "DynoTexWidget");
        if (p) p = fn_8022855C(p, d, texture ? texture->value : 0, frame ? frame->value : 0);
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
class Window {
public:
    Window(const char* name, long x, long y, long w, long h, long flags);

    u8 pad0[0x434];
    void* update;       // 0x434 per-frame dialog update
    void* funcs[4];     // 0x438 mapped UI functions (0x440 = back)
    u8 pad448[0xDE8 - 0x448];
};
// Key binding widget: calls its mapped action when the key is pressed.
struct UIHotKey {
    u8 base[0x244];
    u16 key;            // 0x244
    void* action;       // 0x248
    void* param;        // 0x24C
    UIHotKey(void* p, void* a, u16 k) {
        fn_80218F34((WidgetFragment*)this, 0, 0, 0, 0, 0);
        *(void**)this = lbl_8033D590;
        param = p;
        action = a;
        key = k;
    }
};

extern "C" UILayout* fn_8022E0EC(const char*);
extern "C" UILayout* fn_8022E4C0(UIMapping*);
extern "C" void fn_80276D3C(const char*, int);
extern "C" int fn_802C51C0(void*);
extern "C" int fn_802C5220(void*);
extern "C" Window* fn_80216770(const char*, int, int, int, int, int);
extern "C" void fn_80236914(void);
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
        lbl_8055EF44 = map->name;
        map->layout = layout;
    } else {
        layout = map->layout;
    }
    map->back = 0;
    if (layout->hasCamera) {
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
            UIHotKey* key = new ("HotKey") UIHotKey(s->arg1, s->value, (u16)(u32)s->arg2);
            fn_802168EC(win, (WidgetFragment*)key, -1);
            lbl_8055EF38 = (void*)-1;
            break;
        }
        case 8:
            switch ((int)s->arg1) {
            case 0:
                win->funcs[0] = s->value;
                break;
            case 1:
                win->funcs[1] = s->value;
                break;
            case 2:
                haveBack = 1;
                win->funcs[2] = s->value;
                break;
            case 3:
                win->funcs[3] = s->value;
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
        win->funcs[2] = (void*)fn_80236914;
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
extern "C" const char* lbl_8055B6BC;

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
    win->update = (void*)fn_80211FF0;
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
extern "C" s32 lbl_802F9EA4[];
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
    s32 end = fn_80029004((const char *)arg1, 0x2e);
    if (end == 0) {
        end = (s32)((const char *)arg1 + strlen((const char *)arg1));
    }
    fn_801345D8(arg2, arg3, lbl_8055B808, (s32)arg0, end - arg1, (const char *)arg1);
}

extern "C" void fn_802149C4(s32 arg0) {
    char sp8[0x64];
    fn_80214934("transin", arg0, sp8, 0x64);
    fn_8021481C(sp8);
}
extern "C" s32 fn_80218C14(s32, s32, s32);
extern "C" void fn_80214934(const void*, s32, void*, s32);
extern "C" char lbl_8033D388[];
extern "C" char lbl_8033D268[];
extern "C" char lbl_8055B810[];
extern "C" void fn_802149FC(s32 arg0) {
    char sp8[0x64];
    fn_80214934(lbl_8033D268, arg0, sp8, 0x64);
    fn_8021481C(sp8);
}

extern "C" s32 fn_80214A38(s32 arg0) {
    s32 result = 0;
    s32 check = fn_802168E4();
    if (check != 0) result = ((s32 (*)(s32, s32))fn_802168D4)(check, arg0);
    else ((s32 (*)(const char *, ...))fn_802E317C)(lbl_8033D274);
    return result;
}

extern "C" void fn_802192AC(void);
extern "C" void fn_80219354(void);
extern "C" void fn_80214A9C(s32 arg0) {
    s32 temp = fn_802168E4();
    if (temp != 0) {
        ((s32 (*)(s32, s32, float))fn_802168CC)(temp, arg0, 0.0f);
    } else {
        ((s32 (*)(const char *, ...))fn_802E317C)(lbl_8033D2A0);
    }
}
#pragma scheduling on
extern "C" s32 fn_802168D0(s32, s32, float);
extern "C" char lbl_8033D2C0[];
extern "C" void fn_80214AF0(s32 arg0) {
    s32 temp = fn_802168E4();
    if (temp != 0) {
        ((s32 (*)(s32, s32, float))fn_802168D0)(temp, arg0, 0.0f);
    } else {
        ((s32 (*)(const char *, ...))fn_802E317C)(lbl_8033D2C0);
    }
}

extern "C" void fn_80214B44(s32 arg0) {
    s32 temp = fn_802168E4();
    if (temp != 0) {
        ((s32 (*)(s32, s32))fn_802168DC)(temp, arg0);
    } else {
        ((s32 (*)(const char *, ...))fn_802E317C)(lbl_8033D2E0);
    }
}
#pragma scheduling on
extern "C" s32 fn_802168D8(s32);
extern "C" char lbl_8033D304[];
extern "C" void fn_80214B94(s32 arg0) {
    s32 temp = fn_802168E4();
    if (temp != 0) {
        ((s32 (*)(s32, s32))fn_802168D8)(temp, arg0);
    } else {
        ((s32 (*)(const char *, ...))fn_802E317C)(lbl_8033D304);
    }
}

extern "C" char lbl_8033D328[];
// UIMoveGroup
extern "C" void fn_80214BE4(s32 group, s32 x, s32 y) {
    s32 temp = fn_802168E4();
    if (temp != 0) {
        ((s32 (*)(s32, s32, s32, s32))fn_802168E0)(temp, group, x, y);
    } else {
        ((s32 (*)(const char *, ...))fn_802E317C)(lbl_8033D328);
    }
}

extern "C" void fn_80214C54(s32 arg0, s32 arg1) {
    s32 w = fn_802168E4();
    if (w != 0) {
        fn_80218C14(w, arg0, arg1);
    } else {
        fn_802E317C(lbl_8033D388);
    }
}

extern "C" void fn_80214CB4(s32 arg0) {
    s32 temp = fn_802168E4();
    if (temp != 0) {
        ((s32 (*)(s32, s32))fn_802180E0)(temp, arg0);
    } else {
        ((s32 (*)(const char *, ...))fn_802E317C)(lbl_8033D3B0);
    }
}
#pragma dont_inline reset

extern "C" s32 fn_802168E4(void);
extern "C" s32 fn_80218120(s32);
extern "C" s32 fn_802E317C(const char *, ...);
extern "C" char lbl_8033D3D4[];
extern "C" s32 fn_80214D04(void) {
    s32 w = fn_802168E4();
    if (w != 0) {
        return fn_80218120(w);
    }
    fn_802E317C(lbl_8033D3D4);
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
extern "C" void* lbl_8033D420[];

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
struct UIYesNoHandler {
    void* vtable;
    int x, y, dx, dy;
    void* owner;
    u8 defaultNo;
    UIYesNoHandler(u8 d) {
        vtable = lbl_8033D420;
        owner = 0;
        defaultNo = d;
    }
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
        MenuItem("title", title, 4),
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

extern "C" s32 fn_80215194(s32 arg0, s32 arg1) {
    if (arg0 != 0 && arg1 > 0) {
        fn_80128668();
    }
    return arg0;
}

extern "C" s32 fn_802151D4(s32 arg0, s32 arg1) {
    if (arg0 != 0 && arg1 > 0) {
        fn_80128668();
    }
    return arg0;
}

extern "C" void fn_80215214(const char* title, const char* message, int (*poll)(int*)) {
    fn_80214FE0(title, message, poll, 1);
}

extern "C" void fn_8021521C(const char* title, const char* message, int defaultNo) {
    fn_80214FE0(title, message, 0, defaultNo);
}

// Was the last layout UIBegin/UIDoMappedDialog saw the given one?
extern "C" int fn_80215228(void) { return stricmp(lbl_8033CAE8, lbl_8055B6B0) == 0; }

extern void *lbl_8055EF38;
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
#pragma dont_inline reset

extern "C" s32 fn_802192A4(void *, s32);
extern "C" void fn_802152C8(void);
#pragma dont_inline reset
extern "C" s32 fn_802152CC(void) {
    fn_802152C8();
    return 0;
}
extern "C" void *fn_802152F0(void *arg0, int arg1) {
    if (arg0 != 0) {
        fn_8021915C(arg0, 0);
        if (arg1 > 0) ((s32 (*)(void *))fn_80128668)(arg0);
    }
    return arg0;
}

extern "C" s32 fn_80215348(void *arg0, s32 arg1) {
    if (arg0 != 0) {
        fn_8021915C(arg0, 0);
        if (arg1 > 0) {
            ((s32 (*)(void *))fn_80128668)(arg0);
        }
    }
    return (s32)arg0;
}

void MenuState::Enter(void) {
}

extern "C" s32 fn_802168BC(s32);
extern "C" void fn_8021915C(void *, unsigned int);
int MenuState::Key(int key) {
    return 0;
}

extern "C" s32 fn_802153AC(void *arg0, u32 arg1) {
    s32 (*callback)(s32);
    if (arg1 == *(u16 *)((char *)arg0 + 0x244)) {
        callback = *(s32 (**)(s32))((char *)arg0 + 0x248);
        if (callback == 0 || callback(*(s32 *)((char *)arg0 + 0x24C)) != 0) {
            fn_802168BC(*(s32 *)((char *)arg0 + 0x24C));
        }
        return 1;
    }
    return 0;
}

extern "C" void fn_80215414(void) {

}

extern "C" void fn_80215418(void) {

}

extern "C" void fn_8021541C(void) {

}

extern "C" void fn_80215420(void) {

}

extern "C" void fn_80215424(void) {

}

extern "C" void fn_80215428(void *arg0) {
    *(u8 *)((char *)arg0 + 0x19) = 1;
    if (*(u8 *)((char *)arg0 + 0x1A) != 0) {
        *(u8 *)((char *)arg0 + 0x234) = 1;
    }
}

extern "C" void fn_80215444(void *arg0) {
    *(u8 *)((char *)arg0 + 0x19) = 0;
    if (*(u8 *)((char *)arg0 + 0x1A) != 0) {
        *(u8 *)((char *)arg0 + 0x234) = 1;
    }
}

extern "C" void fn_80215464(void) {

}

extern "C" void fn_80215468(void) {

}

extern "C" void fn_8021546C(void) {

}

extern "C" void fn_80215470(void) {

}

extern "C" void fn_80215474(void) {

}

extern "C" void fn_80215478(void* arg0) {
    *(u8*)((char*)arg0 + 0x18) = 1;
}

extern "C" void fn_80215484(void* arg0) {
    *(u8*)((char*)arg0 + 0x18) = 0;
}

extern "C" s32 fn_80215490(void) {
    return 0;
}

extern "C" s32 fn_80215498(void) {
    return 0;
}

extern "C" void fn_802154A0(void *arg0) {
    if (*(u8 *)((char *)arg0 + 0x1A) != 0) {
        *(u8 *)((char *)arg0 + 0x234) = 1;
    }
}

extern "C" u32 fn_802154B8(void) { return 1; }

extern "C" s32 fn_802154C0(s32 arg0, s32 arg1) {
    if (arg0 != 0 && arg1 > 0) {
        fn_80128668();
    }
    return arg0;
}

extern "C" void fn_80215500(void) {

}

extern "C" void fn_80215504(void *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6) {
    fn_8022BAD0(*(s32 *)((char *)arg0 + 8), 0, *(s32 *)((char *)arg0 + 4), arg6, 1.0f);
}

extern "C" void fn_80215544(void *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6) {
    fn_8022AC1C(*(s32 *)((char *)arg0 + 8), *(s32 *)((char *)arg0 + 4), arg1, arg2, arg3, arg4, arg5, arg6);
}

extern "C" s32 fn_802155AC(void) {
    return 4;
}

extern "C" void fn_802C4C80(void*);
extern "C" int lbl_8033D528[];
extern "C" void* fn_802155B4(void* arg0, s32 arg1) {
    if (arg0 != 0) {
        *(void**)arg0 = &lbl_8033D528;
        fn_802C4C80(((void**)arg0)[1]);
        if (arg1 > 0) {
            ((s32 (*)(void*))fn_80128668)(arg0);
        }
    }
    return arg0;
}

extern "C" s32 fn_80029004(const char *, s32);
extern "C" s32 fn_801345D8(void *, s32, const char *, ...);
extern "C" unsigned long strlen(const char *);
extern "C" char lbl_8055B808[8];
extern "C" s32 fn_802C4F14(s32);
extern "C" s32 fn_802C4C90(s32, s32, s32, s32, s32);
extern "C" void fn_80215618(void *arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 rem = arg3 % fn_802C4F14(*(s32 *)((char *)arg0 + 4));
    fn_802C4C90(*(s32 *)((char *)arg0 + 4), arg1, arg2, rem, 0);
}

void fn_802C4F2C(int, int *, int *);
int fn_802C4EE4(int);
int fn_802C4EFC(int);
// Screen rectangle of an image drawn at (x, y), allowing for the image's origin.
extern "C" void fn_8021568C(void* self, s32 x, s32 y, s32* left, s32* top, s32* right, s32* bottom) {
    int ox = 0;
    int oy = 0;
    fn_802C4F2C(((s32*)self)[1], &ox, &oy);
    *left = x - ox;
    *top = y - oy;
    *right = *left + fn_802C4EE4(((s32*)self)[1]);
    *bottom = *top + fn_802C4EFC(((s32*)self)[1]);
}

extern "C" s32 fn_80215730(void *arg0) {
    return fn_802C4F14(*(s32*)((u8*)arg0 + 4));
}

  void fn_802C2FC4(int);
  s32 fn_80128668(void *);
extern "C" void fn_802C40E8(int, int, int);
void fn_802C4F2C(int, int *, int *);
int fn_802C4EE4(int);
int fn_802C4EFC(int);
extern "C" char lbl_8033D504[];
extern "C" s32 fn_802C2FC4(s32);
extern "C" void *fn_80215738(void *arg0, s32 arg1) {
    s32 old;
    if (arg0 != 0) {
        old = *(s32 *)((char *)arg0 + 4);
        *(char **)arg0 = lbl_8033D504;
        if (old != 0) fn_802C2FC4(old);
        if (arg1 > 0) ((s32 (*)(void *))fn_80128668)(arg0);
    }
    return arg0;
}

void fn_802157A8(int arg0, int arg1, int arg2, unsigned int arg3) {
    unsigned int r7 = *(unsigned int *)(arg0 + 0xC);
    int r8 = *(int *)(arg0 + 4);
    unsigned int rem = arg3 % r7;
    if (r8 != 0) fn_802C40E8(r8, arg1, arg2 - *(int *)(arg0 + 8) * rem);
}

void fn_802157DC(void *arg0, int arg1, int arg2, int *arg3, int *arg4, int *arg5, int *arg6) {
    int var_r0;
    *arg3 = arg1;
    *arg4 = arg2;
    int *temp = *(int **)((char *)arg0 + 4);
    var_r0 = temp != 0 ? temp[2] : 0;
    *arg5 = *arg3 + var_r0;
    *arg6 = *arg4 + *(int *)((char *)arg0 + 8);
}

extern "C" s32 fn_8021581C(void *arg0) {
    return ((s32 *)arg0)[0xC / 4];
}

extern "C" void fn_80215824(void) {

}

extern "C" void fn_80215828(void *self, void *arg1, void *arg2, void *arg3, void *arg4, void *arg5, void *arg6) {
    struct Obj {
        virtual void v0();
        virtual void v1();
        virtual void v2(void *, void *, void *, void *);
    };
    ((Obj *)self)->v2(arg1, arg2, arg3, arg6);
}

extern "C" s32 fn_8021583C(void *arg0, s32 unused1, s32 unused2, s32 arg1, s32 arg2) {
    struct Local {
        virtual void f0();
        virtual void f1();
        virtual void f2();
        virtual void call(s32, s32, s32*, s32*, s32*, s32*);
    };
    s32 sp14;
    s32 sp10;
    s32 spC;
    s32 sp8;
    s32 result;
    ((Local*)arg0)->call(unused1, unused2, &sp14, &sp10, &spC, &sp8);
    result = 0;
    if (arg1 >= sp14 && arg1 < spC && arg2 >= sp10 && arg2 < sp8) result=1;
    return result;
}

extern "C" void fn_802158C8(void *arg0) {
    VfObj *obj = *(VfObj **)((char *)arg0 + 0x244);
    obj->vf1();
}

class Vf4Obj {
public:
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
};

extern "C" void fn_802158DC(void *arg0) {
    Vf4Obj *obj = *(Vf4Obj **)((char *)arg0 + 0x244);
    obj->vf2();
}

extern "C" void fn_802158F0(void *arg0) {
    VfObj *obj = *(VfObj **)((char *)arg0 + 0x244);
    obj->vf3();
}

extern "C" void fn_80215904(void *arg0) {
    fn_802192AC();
    ((VfObj *)*(void **)((char *)arg0 + 0x244))->vf5();
}

extern "C" void fn_80215944(void *arg0) {
    fn_80219354();
    ((VfObj *)*(void **)((char *)arg0 + 0x244))->vf6();
}

extern "C" void fn_80215984(void *arg0) {
    ((VfObj *)*(void **)((char *)arg0 + 0x244))->vf10();
}

extern "C" void fn_80215998(void *arg0) {
    ((VfObj *)*(void **)((char *)arg0 + 0x244))->vf11();
}

extern "C" void fn_802159AC(void *arg0) {
    ((VfObj *)*(void **)((char *)arg0 + 0x244))->vf13();
}

extern "C" void fn_802159C0(void *arg0) {
    ((VfObj *)*(void **)((char *)arg0 + 0x244))->vf14();
}

extern "C" void fn_802159D4(void *arg0) {
    VfObj *obj = *(VfObj **)((char *)arg0 + 0x244);
    obj->vf19();
}

extern "C" void fn_802159E8(void *arg0) {
    VfObj *obj = *(VfObj **)((char *)arg0 + 0x244);
    obj->vf4();
}

extern "C" void fn_802159FC(void *arg0) {
    VfObj *obj = *(VfObj **)((char *)arg0 + 0x244);
    obj->vf12();
}

extern "C" void fn_80215A10(void *arg0) {
    *(u8 *)((u8 *)arg0 + 0x18) = 1;
    ((VfObj *)*(void **)((u8 *)arg0 + 0x244))->vf15();
}

extern "C" void fn_80215A2C(void *arg0) {
    *(u8 *)((u8 *)arg0 + 0x18) = 0;
    ((VfObj *)*(void **)((u8 *)arg0 + 0x244))->vf16();
}

extern "C" void fn_80215A48(void *arg0) {
    *((u8 *)arg0 + 0x19) = 1;
    if ((*((u8 *)arg0 + 0x1A)) != 0) *((u8 *)arg0 + 0x234) = 1;
    void *obj = *(void **)((u8 *)arg0 + 0x244);
    void **vt = *(void ***)obj;
    ((void (*)(void *))*(void **)((u8 *)vt + 0x4C))(obj);
}

#pragma dont_inline reset
extern "C" void fn_80215A74(void *arg0) {
    u8 *self = (u8 *)arg0;
    self[0x19] = 0;
    if (self[0x1A]) {
        self[0x234] = 1;
    }
    (*(MenuState **)(self + 0x244))->Unk50();
}
extern "C" s32 fn_80215AA4(void *arg0, s32 arg1) {
    class Callback {
    public:
        virtual void f0(); virtual void f1(); virtual void f2(); virtual void f3();
        virtual void f4(); virtual void f5(); virtual void f6(); virtual void f7();
        virtual void f8(); virtual void f9(); virtual void f10(); virtual void f11();
        virtual void f12(); virtual void f13(); virtual void f14(); virtual void f15();
        virtual void f16(); virtual void f17(); virtual void f18(); virtual void f19();
        virtual s32 call();
    };
    Callback *obj = *(Callback **)((u8 *)arg0 + 0x244);
    if (obj->call() != 0) return 1;
    return fn_802192A4(arg0, arg1);
}

extern "C" void fn_80215B08(void *arg0) {
    VfObj *obj = *(VfObj **)((char *)arg0 + 0x244);
    obj->vf21();
}

extern "C" void fn_80215B1C(void *arg0) {
    VfObj *obj = *(VfObj **)((char *)arg0 + 0x244);
    obj->vf7();
}

extern "C" void fn_80215B30(void *arg0) {
    VfObj *obj = *(VfObj **)((char *)arg0 + 0x244);
    obj->vf8();
}

extern "C" void fn_80215B44(void *arg0) {
    VfObj *obj = *(VfObj **)((char *)arg0 + 0x244);
    obj->vf9();
}

extern "C" void fn_802191F0(void);
extern "C" void fn_80215B58(void *arg0, s32 arg1, s32 arg2) {
    fn_802191F0();
    ((void **)*(void ***)((char *)arg0 + 0x244))[1] = (void *)arg1;
    ((void **)*(void ***)((char *)arg0 + 0x244))[2] = (void *)arg2;
}

extern "C" s32 fn_80215BAC(void) {
    return -1;
}

extern "C" void fn_80215BB4(void) {

}

extern "C" void fn_80215BB8(void) {

}

extern "C" void fn_80215BBC(void) {

}

extern "C" s32 fn_80215BC0(void) {
    return 0;
}

extern "C" void fn_80214CB4(s32);
extern "C" void fn_80230E90(s32);
extern "C" s32 fn_80215BC8(void *arg0, s32 arg1, s32 arg2) {
    s32 var_r6;

    var_r6 = 0;
    if (arg1 >= *(s32 *)((char *)arg0 + 4) && arg1 < *(s32 *)((char *)arg0 + 0xC) &&
        arg2 >= *(s32 *)((char *)arg0 + 8) && arg2 < *(s32 *)((char *)arg0 + 0x10)) {
        var_r6 = 1;
    }
    return var_r6;
}

extern "C" s32 fn_80215C08(void *arg0) {
    return *(s32 *)((char *)arg0 + 0xC) - *(s32 *)((char *)arg0 + 4);
}

extern "C" void fn_80215C18(void *arg0) {
    fn_80214CB4(*(u8 *)((char *)arg0 + 0x18) == 0);
}

extern "C" s32 fn_80215C28(s32 arg0, s32 arg1) {
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

void MenuState::Unk18(void) {
	fn_80221E7C();
}

void MenuState::Update(void) {

}

void MenuState::Unk20(void) {

}

void MenuState::Unk24(void) {

}

void MenuState::Draw(void) {

}

void MenuState::Unk2C(void) {

}

void MenuState::Unk30(void) {

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

void MenuState::Unk34(void) {

}

