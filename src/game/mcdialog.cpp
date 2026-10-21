// Memory card dialogs (0x8023E5A8-0x8023FA50, "mem3line.lyt", "msaving.lyt"): the saving spinner, the
// three-line status/error box and the yes/no prompts shown around memory card access. Names are guesses
// apart from the strings.
#include "game/common.h"
#include <string.h>

// One menu widget binding: the layout element's name, its kind and kind-specific data.
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

struct MenuStrList {
    u8 pad0[0x14];
};

// The memory card operation being waited on.
class CardTask {
public:
    virtual ~CardTask(void);
    virtual const char* Title(void);  // 0x0C
    virtual int Done(void);           // 0x10
    virtual void Step(void);          // 0x14
};

extern "C" {
void* __nw__FUlPCc(u32 size, const char* tag);
MenuStrList* fn_8022D228(MenuStrList* l, int count, int len);
void fn_8022D2BC(MenuStrList* l, int flags);
void fn_8022D350(MenuStrList* l, const char* s, int a);
int fn_802146BC(MenuDesc* desc, int x, int y, int a);
void fn_80214A9C(int sound);
void fn_80214AF0(int sound);
void fn_80236914(void);
s32 fn_802E2C20(void);  // milliseconds
int fn_802E8D84(void);
void fn_802E30C8(const char* fmt, ...);  // debug print
int fn_80215214(const char* title, const char* msg, int a);
extern char lbl_8055BAC0[8];
extern char lbl_8055BABC[4];
extern char lbl_8055BAB8[4];
extern char lbl_8055B860[8];
}

ModuleTag lbl_8055F1A0;
UnkStatic7F lbl_8055F1A8;
static UnkStatic123 lbl_804987E0;
u8 lbl_8055F1AC[3];       // which circles are lit
CardTask* lbl_8055F1B0;   // the task the saving spinner waits on
s32 lbl_8055F1B4;         // when the spinner may close
s32 lbl_8055F1B8;         // when the task is first polled
u8 lbl_8055F1BC;          // the task is done
void (*lbl_8055F1C0)(void);  // called every poll
u8 lbl_8055F1C4;          // the card was removed
s32 lbl_8055F1C8;         // when the status box closes

extern "C" {

const char* fn_8023E5A8(void) { return "mem3line.lyt"; }

void fn_8023F870(void);
void fn_8023F874(void);
int fn_8023F878(int* result);
void fn_8023EC94(void);
int fn_8023ECF8(int* result);
int fn_8023F91C(int* result);
int fn_8023F964(int* result);
int fn_8023F9E8(void);

void fn_8023E5B4(CardTask* task) {
    s32 now = fn_802E2C20();
    lbl_8055F1B0 = task;
    lbl_8055F1B4 = now + 1500;
    lbl_8055F1BC = 0;
    lbl_8055F1B8 = now + 1000;
    MenuStrList* list = (MenuStrList*)__nw__FUlPCc(0x14, "string_list");
    if (list) {
        list = fn_8022D228(list, 14, 32);
    }
    for (int i = 0; i < 14; i++) {
        fn_8022D350(list, "SAVING", 0);
    }
    MenuItem items[] = {
        MenuItem(lbl_8055BAC0, 8, (void*)fn_8023F874, 0, 0),
        MenuItem(lbl_8055BABC, 8, (void*)fn_8023F870, 1, 0),
        MenuItem("title", 3, task->Title(), strlen(task->Title()), 0),
        MenuItem("circle_list", 10, list, 0, 0),
        MenuItem(lbl_8055BAB8, 8, (void*)fn_80236914, 2, 0),
        *(MenuItem*)&lbl_804987E0,
    };
    MenuDesc desc = {"msaving.lyt", items, fn_8023F878, 0, 0};
    fn_802146BC(&desc, -999, -999, 1);
    fn_8022D2BC(list, 1);
}

void fn_8023E7A8(int status) {
    char title[32] = {0};
    char line1[128] = {0};
    char line2[128] = {0};
    char line3[128] = {0};
    switch (status) {
    case 0:
        strcpy(title, "Loading...");
        break;
    case 10:
        strcpy(title, "Checking...");
        break;
    case 3:
        strcpy(title, "Saving...");
        break;
    case 2:
        strcpy(title, "Loading");
        break;
    case 5:
        strcpy(title, "Saving");
        break;
    case 7:
        strcpy(title, "Formating...");
        break;
    case 9:
        strcpy(title, "Formating");
        break;
    case 1:
        strcpy(title, "Loading Error!");
        strcpy(line1, "An error loading your game");
        strcpy(line2, "from the memory card");
        strcpy(line3, "occurred");
        break;
    case 4:
        strcpy(title, "Saving Error!");
        strcpy(line1, "An error occurred while saving");
        strcpy(line2, "your game on the memory card");
        strcpy(line3, "");
        break;
    case 6:
        strcpy(title, "Saving Error!");
        strcpy(line1, "There is insufficient free space");
        strcpy(line2, "on your memory card");
        strcpy(line3, "(280KB required)");
        break;
    case 8:
        strcpy(title, "Formating Error!");
        strcpy(line1, "An error occured while formating");
        strcpy(line2, "your memory card");
        strcpy(line3, "");
        break;
    default:
        fn_802E30C8("mmemcard: Unknown status passed from memory card controller");
        strcpy(title, "Unknown error!");
        strcpy(line1, "An unknown error occured. Please");
        strcpy(line2, "insert another memory card");
        strcpy(line3, "into MEMORY CARD slot 1");
        break;
    }
    lbl_8055F1AC[0] = 0;
    lbl_8055F1AC[2] = 0;
    lbl_8055F1AC[1] = 0;
    int circle1 = 1;
    int circle2 = 0;
    int circle3 = 0;
    switch (status) {
    case 2:
        circle3 = 1;
    case 0:
    case 1:
        circle2 = 1;
    case 10:
        lbl_8055F1AC[0] = 1;
        lbl_8055F1AC[2] = 1;
        lbl_8055F1AC[1] = 1;
        break;
    case 5:
        circle3 = 1;
    case 3:
    case 4:
    case 6:
        circle2 = 1;
        circle1 = 0;
        lbl_8055F1AC[2] = 1;
        lbl_8055F1AC[1] = 1;
        break;
    case 9:
        circle2 = 1;
    case 7:
    case 8:
        lbl_8055F1AC[0] = 1;
        break;
    }
    lbl_8055F1C8 = fn_802E2C20() + 100;
    MenuItem items[] = {
        MenuItem(lbl_8055BAC0, 8, (void*)fn_8023EC94, 0, 0),
        MenuItem("circle1", 2, &circle1, 0, 0),
        MenuItem("circle2", 2, &circle2, 0, 0),
        MenuItem("circle3", 2, &circle3, 0, 0),
        MenuItem("title", 3, title, 32, 0),
        MenuItem("line1", 3, line1, 128, 0),
        MenuItem("line2", 3, line2, 128, 0),
        MenuItem("line3", 3, line3, 128, 0),
        MenuItem("line4", 3, "", 4, 0),
        MenuItem("line5", 3, "", 4, 0),
        MenuItem("lbutton", 3, "", 4, 0),
        MenuItem("rbutton", 3, "", 4, 0),
        *(MenuItem*)&lbl_804987E0,
    };
    MenuDesc desc = {"mem3line.lyt", items, fn_8023ECF8, 0, 0};
    int show;
    if (status == 3) {
        show = 1;
    } else {
        show = 1;
    }
    if (show) {
        fn_802146BC(&desc, -999, -999, 1);
    }
}

void fn_8023EC94(void) {
    u8* lit = lbl_8055F1AC;
    for (int i = 0; i < 3; i++, lit++) {
        if (*lit) {
            fn_80214AF0(i + 1);
        } else {
            fn_80214A9C(i + 1);
        }
    }
}

int fn_8023ECF8(int* result) { return lbl_8055F1C8 < fn_802E2C20(); }

void fn_8023ED30(void) {
    MenuItem items[] = {
        MenuItem(lbl_8055B860, 6, NULL, -2, 13),
        MenuItem("title", 3, "Settings are corrupt", 4, 0),
        MenuItem("line1", 3, "Unable to load settings.", 4, 0),
        MenuItem("line2", 3, "Press A to continue.", 4, 0),
        MenuItem("line3", 3, "", 4, 0),
        MenuItem("line4", 3, "", 4, 0),
        MenuItem("line5", 3, "", 4, 0),
        MenuItem("rbutton", 3, "A - Continue", 4, 0),
        MenuItem("lbutton", 3, "", 4, 0),
        *(MenuItem*)&lbl_804987E0,
    };
    MenuDesc desc = {NULL, NULL, fn_8023F964, 0, 0};
    lbl_8055F1C4 = 0;
    desc.layout = fn_8023E5A8();
    desc.items = items;
    fn_802146BC(&desc, -999, -999, 1);
}

int fn_8023EEF8(void) {
    MenuItem items[] = {
        MenuItem(lbl_8055B860, 6, NULL, -2, 13),
        MenuItem(lbl_8055B860, 6, NULL, -1, 27),
        MenuItem("title", 3, "Out of Room", 4, 0),
        MenuItem("line1", 3, "Your Xbox does not have enough", 4, 0),
        MenuItem("line2", 3, "free blocks to store settings.", 4, 0),
        MenuItem("line3", 3, "Press A to continue without saving", 4, 0),
        MenuItem("line4", 3, "or B to free more blocks.", 4, 0),
        MenuItem("line5", 3, "", 4, 0),
        MenuItem("rbutton", 3, "A - Continue", 4, 0),
        MenuItem("lbutton", 3, "B - Free More Blocks", 4, 0),
        *(MenuItem*)&lbl_804987E0,
    };
    MenuDesc desc = {NULL, NULL, fn_8023F964, 0, 0};
    lbl_8055F1C4 = 0;
    desc.layout = fn_8023E5A8();
    desc.items = items;
    return fn_802146BC(&desc, -999, -999, 1) == -2;
}

int fn_8023F0E8(void) {
    MenuItem items[] = {
        MenuItem(lbl_8055B860, 6, NULL, -2, 13),
        MenuItem(lbl_8055B860, 6, (void*)fn_8023F9E8, -1, 27),
        MenuItem("title", 3, "Format", 4, 0),
        MenuItem("line1", 3, "Format your memory card  ?", 4, 0),
        MenuItem("line2", 3, "This will erase any saved games", 4, 0),
        MenuItem("line3", 3, "", 4, 0),
        MenuItem("line4", 3, "", 4, 0),
        MenuItem("line5", 3, "", 4, 0),
        MenuItem("rbutton", 3, "FORMAT", 4, 0),
        MenuItem("lbutton", 3, "DON'T SAVE", 4, 0),
        *(MenuItem*)&lbl_804987E0,
    };
    MenuDesc desc = {NULL, NULL, NULL, 0, 0};
    desc.layout = fn_8023E5A8();
    desc.items = items;
    return fn_802146BC(&desc, -999, -999, 1) == -2;
}

int fn_8023F2C4(void) {
    MenuItem items[] = {
        MenuItem(lbl_8055B860, 6, NULL, -2, 13),
        MenuItem(lbl_8055B860, 6, (void*)fn_8023F9E8, -1, 27),
        MenuItem("title", 3, "Memory Card Not Found", 4, 0),
        MenuItem("line1", 3, "Please insert a memory card", 4, 0),
        MenuItem("line2", 3, "with at least 280KB of free space", 4, 0),
        MenuItem("line3", 3, "into MEMORY CARD slot 1", 4, 0),
        MenuItem("line4", 3, "", 4, 0),
        MenuItem("line5", 3, "", 4, 0),
        MenuItem("rbutton", 3, "RETRY", 4, 0),
        MenuItem("lbutton", 3, "DON'T SAVE", 4, 0),
        *(MenuItem*)&lbl_804987E0,
    };
    MenuDesc desc = {NULL, NULL, fn_8023F91C, 0, 0};
    desc.layout = fn_8023E5A8();
    desc.items = items;
    return fn_802146BC(&desc, -999, -999, 1) == -2;
}

int fn_8023F4B8(void) {
    MenuItem items[] = {
        MenuItem(lbl_8055B860, 6, NULL, -2, 13),
        MenuItem(lbl_8055B860, 6, (void*)fn_8023F9E8, -1, 27),
        MenuItem("title", 3, "Save Here?", 4, 0),
        MenuItem("line1", 3, "Do you wish to save your game", 4, 0),
        MenuItem("line2", 3, "to the memory card in", 4, 0),
        MenuItem("line3", 3, "MEMORY CARD slot 1?", 4, 0),
        MenuItem("line4", 3, "", 4, 0),
        MenuItem("line5", 3, "", 4, 0),
        MenuItem("rbutton", 3, "YES", 4, 0),
        MenuItem("lbutton", 3, "DON'T SAVE", 4, 0),
        *(MenuItem*)&lbl_804987E0,
    };
    MenuDesc desc = {NULL, NULL, NULL, 0, 0};
    desc.layout = fn_8023E5A8();
    desc.items = items;
    return fn_802146BC(&desc, -999, -999, 1) == -2;
}

int fn_8023F694(void) {
    MenuItem items[] = {
        MenuItem(lbl_8055B860, 6, NULL, -2, 13),
        MenuItem(lbl_8055B860, 6, (void*)fn_8023F9E8, -1, 27),
        MenuItem("title", 3, "Overwrite Old Game?", 4, 0),
        MenuItem("line1", 3, "There is a saved game on this memory", 4, 0),
        MenuItem("line2", 3, "card. Saving now will overwrite", 4, 0),
        MenuItem("line3", 3, "your previous saved game.", 4, 0),
        MenuItem("line4", 3, "", 4, 0),
        MenuItem("line5", 3, "", 4, 0),
        MenuItem("rbutton", 3, "SAVE", 4, 0),
        MenuItem("lbutton", 3, "DON'T SAVE", 4, 0),
        *(MenuItem*)&lbl_804987E0,
    };
    MenuDesc desc = {NULL, NULL, NULL, 0, 0};
    desc.layout = fn_8023E5A8();
    desc.items = items;
    return fn_802146BC(&desc, -999, -999, 1) == -2;
}

void fn_8023F870(void) {}

void fn_8023F874(void) {}

int fn_8023F878(int* result) {
    if (lbl_8055F1C0) {
        lbl_8055F1C0();
    }
    s32 now = fn_802E2C20();
    if (!lbl_8055F1BC) {
        if (now > lbl_8055F1B8) {
            if (lbl_8055F1B0->Done()) {
                lbl_8055F1BC = 1;
            } else {
                lbl_8055F1B0->Step();
            }
        }
    } else if (now > lbl_8055F1B4) {
        return 1;
    }
    return 0;
}

int fn_8023F91C(int* result) {
    if (fn_802E8D84()) {
        *result = -2;
        return 1;
    }
    return 0;
}

int fn_8023F964(int* result) {
    if (lbl_8055F1C0) {
        lbl_8055F1C0();
    }
    if (!fn_802E8D84()) {
        lbl_8055F1C4 = 1;
        return 0;
    }
    if (lbl_8055F1C4 && fn_802E8D84()) {
        *result = -2;
        return 1;
    }
    return 0;
}

// Stripped from ET; only its strings remain.
void McDialogStrippedImages(void) {
    fn_802E30C8("*uibig.res/mc_check.img");
    fn_802E30C8("*uibig.res/mc_notfound.img");
    fn_802E30C8("*uibig.res/mc_full.img");
}

int fn_8023F9E8(void) { return fn_80215214("Don't Ever Save", "Are you sure?", 0); }

// Stripped from ET; only its string remains.
void McDialogStrippedImageError(void) { fn_802E30C8("mmemcard: image %s not found"); }

}  // extern "C"
