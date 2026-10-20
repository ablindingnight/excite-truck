// Car classes (0x8013C748-0x8013C9F4): the SUV/Truck/Featherweight/Heavyweight/Extreme classes, each
// bound to its table in carclass.val, and the lookup of a chassis's class. Names are guesses.
#include "game/common.h"

extern "C" {
int strcasecmp(const char* a, const char* b);
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* t);
}

struct CarClass {
    const char* name;  // offset 0x0
    s32 id;            // offset 0x4
    ValueNode* node;   // offset 0x8
};

ModuleTag lbl_8055E180;
ValueTable* lbl_8055E188;  // carclass.val

CarClass lbl_8032C478[] = {
    {"SUV", 0, 0}, {"Truck", 1, 0}, {"Featherweight", 2, 0}, {"Heavyweight", 3, 0}, {"Extreme", 4, 0}, {0, 6, 0},
};

extern "C" {

void fn_8013C748(void) {
    CarClass* c;
    for (c = lbl_8032C478; c->name; c++) {
        c->node = 0;
    }
}

CarClass* fn_8013C770(const char* name) {
    CarClass* c;
    for (c = lbl_8032C478; c->name; c++) {
        if (!strcasecmp(name, c->name)) {
            break;
        }
    }
    return c;
}

CarClass* fn_8013C7D0(s32 id) {
    CarClass* c;
    for (c = lbl_8032C478; c->name; c++) {
        if (id == c->id) {
            break;
        }
    }
    return c;
}

CarClass* fn_8013C800(s32 id) { return fn_8013C7D0(id); }

void fn_8013C804(void) {
    fn_8013C748();
    lbl_8055E188 = fn_80131808("carclass.val");
    if (lbl_8055E188) {
        ValueNode* n;
        int i;
        for (i = 0; (n = lbl_8055E188->GetChild(i)) != 0; i++) {
            CarClass* c = fn_8013C770(n->GetName());
            if (c->id != 6) {
                c->node = n;
            }
        }
    } else {
        fn_802E317C("carclass: no class file?");
    }
}

void fn_8013C8B8(void) {
    fn_8013C748();
    fn_80131818(lbl_8055E188);
    lbl_8055E188 = 0;
}

s32 fn_8013C8E8(const char* chassis) {
    int i;
    for (i = 0; i < 5; i++) {
        CarClass* c = fn_8013C800(i);
        ValueNode* n = c->node->FindChild("chassis");
        int j;
        const char* key;
        const char* val;
        for (j = 0; n->GetPair(j, &key, &val); j++) {
            if (!strcasecmp(val, chassis)) {
                return c->id;
            }
        }
    }
    return 6;
}

s32 fn_8013C9A4(const char* name) { return fn_8013C770(name)->id; }

const char* fn_8013C9C8(s32 id) { return fn_8013C800(id)->name; }

}  // extern "C"
