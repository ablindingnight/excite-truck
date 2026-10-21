// Speed units menu (0x8024C070-0x8024C370, "units.lyt"): one option cycler between MPH and KPH, kept
// in the "units" option. Names are guesses apart from the layout, option and labels.
#include "game/common.h"

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

// Steps *out through a table of values, showing the matching label.
class ValueMulti : public MenuWidget {
public:
    ValueMulti(int* out, const char** labels, int* values, int count) {
        index = 0;
        this->values = values;
        this->labels = labels;
        this->count = count;
        this->out = out;
        for (int i = 0; i < count; i++) {
            if (values[i] == *out) {
                index = i;
            }
        }
    }
    virtual ~ValueMulti(void) {}
    virtual void Prev(void) {
        index = (index + count - 1) % count;
        *out = values[index];
    }
    virtual void Next(void) {
        index = (index + 1) % count;
        *out = values[index];
    }
    virtual const char* Current(void) { return labels[index]; }
    virtual int Value(void) { return values[index]; }

    int index;            // offset 0x10
    int* values;          // offset 0x14
    const char** labels;  // offset 0x18
    int count;            // offset 0x1C
    int* out;             // offset 0x20
};

extern "C" {
int fn_802146BC(MenuDesc* desc, int x, int y, int a);
void fn_80236914(void);
void fn_801333E8(int section, const char* name, bool* v);
void fn_80133460(int section, const char* name, bool v);
extern char lbl_8055BAB8[4];
extern char lbl_8055B860[8];
}

ModuleTag lbl_8055F2A8;
static UnkStatic123 lbl_804989D0;
UnkStatic7F lbl_8055F2B0;

extern "C" void fn_8024C070(void) {
    bool kph = false;
    fn_801333E8(5, "units", &kph);
    int units = kph != 0;
    int values[2] = {0, 1};
    const char* labels[2] = {"MPH", "KPH"};
    ValueMulti multi(&units, labels, values, 2);
    MenuItem items[] = {
        MenuItem("units", 16, NULL, (int)&multi, 0),
        MenuItem(lbl_8055BAB8, 8, (void*)fn_80236914, 2, 0),
        MenuItem(lbl_8055B860, 6, NULL, -2, 27),
        *(MenuItem*)&lbl_804989D0,
    };
    MenuDesc desc = {"units.lyt", items, NULL, 0, 0};
    fn_802146BC(&desc, -999, -999, 1);
    kph = units == 1;
    fn_80133460(5, "units", kph);
}
