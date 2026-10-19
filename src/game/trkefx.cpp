// Track effect tables (0x802AE2E0-0x802AE5E0): loads trkefx.val, sh_trkefx.val and race.res/efx.val
// on first use and looks entries up across them. Names are guesses.
#include "game/common.h"

extern "C" {
ValueTable* fn_80131810(const char* name);  // load value file, quiet
void fn_80131818(ValueTable* t);
}

template <typename T>
struct ArrayRef {
    ArrayRef(T* d, int n) {
        data = d;
        cap = n;
        count = 0;
    }
    ~ArrayRef(void) {
        data = 0;
        count = 0;
        cap = 0;
    }
    T* Next(void) { return &data[count++]; }
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
};

struct TableList : ArrayRef<ValueTable*> {
    TableList(void) : ArrayRef<ValueTable*>(storage, 4) {}
    ValueTable* storage[4];
};

ModuleTag lbl_8055F950;
u8 lbl_8055F958;  // tables loaded
u8 lbl_8055F959;
TableList lbl_804BD8CC;

extern "C" {

void fn_802AE2E0(const char* name) {
    ValueTable* t = fn_80131810(name);
    if (t) {
        *lbl_804BD8CC.Next() = t;
    }
}

void fn_802AE328(void) {
    fn_802AE2E0("trkefx.val");
    fn_802AE2E0("sh_trkefx.val");
    fn_802AE2E0("race.res/efx.val");
    lbl_8055F958 = 1;
}

void fn_802AE374(void) {
    lbl_8055F958 = 0;
    lbl_8055F959 = 1;
    lbl_804BD8CC.count = 0;
}

void fn_802AE394(void) {
    int i;
    for (i = 0; i < lbl_804BD8CC.count; i++) {
        fn_80131818(lbl_804BD8CC.data[i]);
    }
    lbl_804BD8CC.count = 0;
    lbl_8055F958 = 0;
    lbl_8055F959 = 0;
}

ValueNode* fn_802AE414(const char* name) {
    int i;
    if (!lbl_8055F958) {
        fn_802AE328();
    }
    for (i = 0; i < lbl_804BD8CC.count; i++) {
        if (lbl_804BD8CC.data[i]->HasChild(name)) {
            return lbl_804BD8CC.data[i]->FindChild(name);
        }
    }
    return 0;
}

ValueTable* fn_802AE4C8(int index) {
    if (!lbl_8055F958) {
        fn_802AE328();
    }
    if (index >= 0 && index < lbl_804BD8CC.count) {
        return lbl_804BD8CC.data[index];
    }
    return 0;
}

}  // extern "C"
