// Name entry (0x8023FA50-0x8023FE20): the on-screen keyboard menu that asks for a player name and
// stores it as the identity's default name. Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
char* strcat(char* dst, const char* src);
char* strcpy(char* dst, const char* src);
char* strstr(const char* s, const char* sub);
int strcmp(const char* a, const char* b);
}

void* operator new(size_t size, const char* tag);
char* IdentGetDefaultName(int slot);
void IdentSetDefaultName(const char* name, int slot);

// The keyboard (keyboard.cpp).
class NameChooser {
public:
    NameChooser(char* buf, int max);
    virtual ~NameChooser(void);
    int IsDone(void);
    u8 pad4[0x34];
};

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

extern "C" {
void fn_80214A9C(int sound);
void fn_80214AF0(int sound);
void fn_8024683C(void);
void fn_80236914(void);
void fn_8022D228(MenuStrList* l, int count, int len);
void fn_8022D2BC(MenuStrList* l, int flags);
void fn_8022D350(MenuStrList* l, const char* s, int a);
int fn_802146BC(MenuDesc* desc, int x, int y, int a);
extern char lbl_8055BAC0[8];
extern char lbl_8055BAB8[4];
extern char lbl_8055B860[8];
}

ModuleTag lbl_8055F1D0;
static UnkStatic123 lbl_804987F8;
UnkStatic7F lbl_8055F1D8;
UnkStaticFFFF lbl_8055F1DC;
NameChooser* lbl_8055F1E0;

extern "C" {

int fn_8023FA50(int* result) {
    if (lbl_8055F1E0->IsDone()) {
        *result = -2;
        return 1;
    }
    return 0;
}

void fn_8023FA9C(void) {
    fn_80214A9C(12);
    fn_8024683C();
    fn_80214A9C(31);
    fn_80214AF0(12);
}

int fn_8023FAD4(char* name, int maxLen) {
    strcat(name, "_");
    lbl_8055F1E0 = new ("name_chooser") NameChooser(name, maxLen);
    MenuStrList list;
    fn_8022D228(&list, 10, 32);
    for (int i = 0; i < 10; i++) {
        fn_8022D350(&list, "ENTER NAME", 0);
    }
    MenuItem items[] = {
        MenuItem(lbl_8055BAC0, 8, (void*)fn_8023FA9C, 0, 0),
        MenuItem("player", 3, "", 4, 0),
        MenuItem("namechooser", 5, lbl_8055F1E0, 0, 0),
        MenuItem("circle_list", 10, &list, 0, 0),
        MenuItem(lbl_8055BAB8, 8, (void*)fn_80236914, 2, 0),
        MenuItem(lbl_8055B860, 6, NULL, -1, 27),
        *(MenuItem*)&lbl_804987F8,
    };
    MenuDesc desc = {"namenter.lyt", items, fn_8023FA50, 0, 0};
    int ok = fn_802146BC(&desc, -999, -999, 1) != -1;
    delete lbl_8055F1E0;
    lbl_8055F1E0 = NULL;
    fn_8022D2BC(&list, -1);
    return ok;
}

void fn_8023FCFC(int slot) {
    char name[32];
    strcpy(name, IdentGetDefaultName(slot));
    if (strstr(name, "Player-") || strstr(name, "Guest") || strstr(name, "Driver-")) {
        strcpy(name, "");
    }
    if (fn_8023FAD4(name, 19) && strcmp(IdentGetDefaultName(slot), name) && strcmp(name, "")) {
        IdentSetDefaultName(name, slot);
    }
}

}  // extern "C"

// NonMatching: one item's stores are scheduled differently and the sinit is one of the
// epilogue-scheduled kind.
