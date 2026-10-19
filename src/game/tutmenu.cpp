// Tutorial menu (0x80257FB8-0x80258354): picks a tutorial set, highlighting the sets already
// passed, the current one and the locked ones. Names are guesses apart from the element names.
#include "game/common.h"

int IdentGetCurrentSlot(void);

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

// The highlighted set, remembered per identity slot.
struct TutorialChoice {
    TutorialChoice(void) {
        index = -1;
        slot = -1;
    }
    int index;
    int slot;
};

extern "C" {
int fn_802146BC(MenuDesc* desc, int x, int y, int a);
void fn_80236914(void);
int fn_80161D14(void);       // lessons passed
int fn_80161ECC(void);       // set count
void* fn_80161E40(int set);
int fn_8016208C(void* set);  // lessons in a set
void fn_80161F10(int set);
void fn_80214A9C(int id);
void fn_80214AF0(int id);
void fn_80214B44(int id);
void fn_80214CB4(int index);
int fn_80214D04(void);
extern char lbl_8055BAC0[8];
extern char lbl_8055BAB8[4];
extern char lbl_8055B860[8];
}

ModuleTag lbl_8055F360;
static UnkStatic123 lbl_80498E68;
int lbl_8055F368;  // set holding the next lesson
TutorialChoice lbl_8055F370;

extern "C" {

// Returns the set that holds the first lesson not yet passed.
int fn_80257FB8(void) {
    int passed = fn_80161D14();
    int count = fn_80161ECC();
    int total = 0;
    for (int i = 0; i < count; i++) {
        total += fn_8016208C(fn_80161E40(i));
        if (passed <= total) {
            return i;
        }
    }
    return 0;
}

void fn_80258040(void) {
    fn_80214CB4(lbl_8055F370.index);
    int passed = fn_80161D14();
    int count = fn_80161ECC();
    int total = 0;
    for (int i = 0; i < count; i++) {
        int n = fn_8016208C(fn_80161E40(i));
        fn_80214A9C(i + 10);
        fn_80214A9C(i + 14);
        fn_80214A9C(i + 18);
        if (passed <= total) {
            fn_80214B44(i + 1);
            fn_80214AF0(i + 18);
        } else if (passed > total + n) {
            fn_80214AF0(i + 10);
        } else {
            fn_80214AF0(i + 14);
        }
        total += n;
    }
}

int fn_80258104(int* result) {
    lbl_8055F370.index = fn_80214D04();
    return 0;
}

void fn_8025812C(void) {
    lbl_8055F368 = fn_80257FB8();
    int slot = IdentGetCurrentSlot();
    if (lbl_8055F370.slot != slot) {
        lbl_8055F370.slot = slot;
        lbl_8055F370.index = lbl_8055F368;
    }
    MenuItem items[] = {
        MenuItem(lbl_8055BAC0, 8, (void*)fn_80258040, 0, 0),
        MenuItem("set1", 0, NULL, 0, 0),
        MenuItem("set2", 0, NULL, 1, 0),
        MenuItem("set3", 0, NULL, 2, 0),
        MenuItem("set4", 0, NULL, 3, 0),
        MenuItem(lbl_8055B860, 6, NULL, -1, 27),
        MenuItem(lbl_8055BAB8, 8, (void*)fn_80236914, 2, 0),
        *(MenuItem*)&lbl_80498E68,
    };
    MenuDesc desc = {"tutorial.lyt", items, fn_80258104, 0, 0};
    int result;
    do {
        result = fn_802146BC(&desc, -999, -999, 1);
        int before = fn_80257FB8();
        if (result != -1) {
            fn_80161F10(result);
        }
        int after = fn_80257FB8();
        if (after != before) {
            lbl_8055F370.index = after;
        }
    } while (result != -1);
}

}  // extern "C"
