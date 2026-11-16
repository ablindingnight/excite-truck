// A registry of up to 12 objects looked up by id (0x8027ABF8-0x8027AD4C), plus small accessors on
// the registered objects. Names are guesses.
#include "game/common.h"
#include "game/carreg.h"

// RegEntry: game/carreg.h.
// RegPair: game/carreg.h.
// RegMap: game/carreg.h.
// RegUser: game/carreg.h.
// RegTable: game/carreg.h.
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
