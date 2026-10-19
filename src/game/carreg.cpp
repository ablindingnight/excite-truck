// A registry of up to 12 objects looked up by id (0x8027ABF8-0x8027AD4C), plus small accessors on
// the registered objects. Names are guesses.
#include "game/common.h"

struct RegEntry {
    s32 id;  // offset 0x0
};

struct RegPair {
    s32 key;    // offset 0x0
    s32 value;  // offset 0x4
};

struct RegMap {
    s32 Find(s32 key) const {
        int i;
        for (i = 0; i < count; i++) {
            if (key == pairs[i].key) {
                return pairs[i].value;
            }
        }
        return 0;
    }
    RegPair pairs[11];  // offset 0x0
    s32 count;          // offset 0x58
};

struct RegUser {
    u8 pad0[0x30];
    RegMap map;  // offset 0x30
    u8 pad8C[0xE8 - 0x8C];
    s32 regId;   // offset 0xE8
    u8 padEC[0xF0 - 0xEC];
    int IsActive(void) const { return active != 0; }
    s32 active;  // offset 0xF0
    u8 padF4[0xF8 - 0xF4];
    s32 slot;    // offset 0xF8
};

struct RegTable {
    RegTable(void) { count = 0; }
    ~RegTable(void) {}
    RegEntry* items[12];  // offset 0x0
    s32 count;            // offset 0x30
};

ModuleTag lbl_8055F640;
RegTable lbl_804A433C;

extern "C" {

RegEntry* fn_8027ACBC(RegTable* table, s32 id);

RegEntry* fn_8027ABF8(s32 id) { return fn_8027ACBC(&lbl_804A433C, id); }

s32 fn_8027AC08(RegUser* u, s32 key) { return u->map.Find(key); }

int fn_8027AC50(RegUser* u) { return u->active != 0; }

s32 fn_8027AC64(RegUser* u) {
    if (u->active == 0) {
        return -1;
    } else {
        return u->slot;
    }
}

int fn_8027AC80(const RegUser* u) { return fn_8027ACBC(&lbl_804A433C, u->regId) != 0; }

RegEntry* fn_8027ACBC(RegTable* table, s32 id) {
    int i;
    for (i = 0; i < table->count; i++) {
        if (id == table->items[i]->id) {
            return table->items[i];
        }
    }
    return 0;
}

}  // extern "C"
