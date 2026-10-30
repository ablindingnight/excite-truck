// Player profile slots ("IdentInfo", 0x8000D884-0x8000DFA8). The block is registered with the
// save system under 'Idnt'; other modules register extra per-slot fields that live after it, and
// get notified when the current slot changes. Names are guesses.
#include <string.h>
#include "game/common.h"

extern "C" {
void fn_802E30C8(const char* fmt, ...);  // debug print
int fn_801345D8(char* buf, u32 size, const char* fmt, ...);  // snprintf
const char* fn_80131234(const char* key);  // localize
void* fn_801284E8(const char* tag, u32 size);
void fn_80128658(void* p);
void fn_802E6BD8(u32 code, int, void* buf, u32 size, void (*cb)(int));
void fn_80247624(void);
}

typedef void (*IdentCallback)(int slot, int event);
typedef void (*IdentFieldClear)(void);

#define NUM_SLOTS 4
#define NAME_LEN 0x12

struct IdentSlot {
    s32 state;            // offset 0x0 (0 empty, 1 loaded, 2 in use)
    char name[NAME_LEN];  // offset 0x4
};

struct IdentInfo {
    s32 current;                          // offset 0x0
    IdentSlot slots[NUM_SLOTS];           // offset 0x4
    char default_names[NUM_SLOTS][NAME_LEN];  // offset 0x64
    s32 unkAC[NUM_SLOTS];                 // offset 0xAC
};

struct IdentField {
    s32 unk0;
    s32 unk4;
    void** ptr;              // offset 0x8, set to this slot's copy
    u32 size;                // offset 0xC
    IdentFieldClear clear;   // offset 0x10
    u32 offset;              // offset 0x14
};

ModuleTag lbl_8055D458;
s32 lbl_8055D460;
s32 lbl_8055D464;
void* lbl_8055D468;
IdentInfo* lbl_8055D46C;
u8 lbl_8055D470;
u32 lbl_80558A88 = sizeof(IdentInfo);
IdentCallback lbl_8034F628[10];
IdentField lbl_8034F650[4];

void IdentResetAll(void);
void IdentSelectSlot(int slot);
void IdentClearSlot(void);
void IdentNotify(int event);
void IdentSaveEvent(int event);

void IdentRegisterField(s32 a, s32 b, void** ptr, u32 size, IdentFieldClear clear) {
    IdentField* f = &lbl_8034F650[lbl_8055D464];

    f->unk0 = a;
    f->unk4 = b;
    f->ptr = ptr;
    f->size = size;
    f->clear = clear;
    lbl_8055D464++;
    f->offset = lbl_80558A88;
    lbl_80558A88 += size * NUM_SLOTS;
}

void IdentInit(void) {
    int i;

    lbl_8055D470 = 1;
    lbl_8055D468 = fn_801284E8("IdentInfo", lbl_80558A88);
    fn_802E6BD8('Idnt', 2, lbl_8055D468, lbl_80558A88, IdentSaveEvent);
    lbl_8055D46C = (IdentInfo*)lbl_8055D468;
    IdentResetAll();
    for (i = NUM_SLOTS - 1; i >= 0; i--) {
        IdentSelectSlot(i);
        IdentClearSlot();
    }
}

void IdentShutdown(void) {
    lbl_8055D46C = NULL;
    fn_80128658(lbl_8055D468);
    lbl_8055D468 = NULL;
}

void IdentSelectSlot(int slot) {
    IdentField* f;
    int i;

    IdentNotify(2);
    lbl_8055D46C->current = slot;
    for (i = 0, f = lbl_8034F650; i < lbl_8055D464; i++, f++) {
        *f->ptr = (u8*)lbl_8055D468 + f->offset + f->size * lbl_8055D46C->current;
    }
    IdentNotify(3);
}

int IdentGetCurrentSlot(void) {
    return lbl_8055D46C->current;
}

void IdentSetSlotName(const char* name) {
    int n;
    char* dst;

    lbl_8055D46C->slots[lbl_8055D46C->current].state = 2;
    n = NAME_LEN;
    dst = lbl_8055D46C->slots[lbl_8055D46C->current].name;
    while ((*dst++ = *name++) != '\0') {
        if (--n <= 0) {
            dst[-1] = '\0';
            return;
        }
    }
}

void IdentClearSlot(void) {
    IdentField* f;
    int i;

    lbl_8055D46C->slots[lbl_8055D46C->current].state = 0;
    IdentNotify(0);
    for (i = 0, f = lbl_8034F650; i < lbl_8055D464; f++, i++) {
        memset(*f->ptr, 0, f->size);
        if (f->clear != NULL) {
            f->clear();
        }
    }
}

void IdentMarkSlotUsed(void) {
    lbl_8055D46C->slots[lbl_8055D46C->current].state = 2;
}

const char* IdentGetSlotName(int slot) {
    IdentSlot* s;

    fn_80247624();
    s = &lbl_8055D46C->slots[slot];
    if (s->state == 0) {
        return fn_80131234("UIProfile:EMPTY_SLOT");
    }
    return s->name;
}

BOOL IdentIsSlotUsed(int slot) {
    return lbl_8055D46C->slots[slot].state == 2;
}

BOOL IdentIsSlotEmpty(int slot) {
    return lbl_8055D46C->slots[slot].state == 0;
}

void IdentRegisterCallback(IdentCallback cb) {
    int i;

    for (i = 0; i < lbl_8055D460; i++) {
        if (lbl_8034F628[i] == cb) {
            fn_802E30C8("tried to register callback at %p more than once!", cb);
            return;
        }
    }
    lbl_8034F628[lbl_8055D460++] = cb;
}

void IdentSetDefaultName(const char* name, int slot) {
    int n = NAME_LEN;
    char* dst = lbl_8055D46C->default_names[slot];

    while ((*dst++ = *name++) != '\0') {
        if (--n <= 0) {
            dst[-1] = '\0';
            return;
        }
    }
}

char* IdentGetDefaultName(int slot) {
    return lbl_8055D46C->default_names[slot];
}

void IdentSaveEvent(int event) {
    int i;

    switch (event) {
    case 1:
        IdentSelectSlot(lbl_8055D46C->current);
        break;
    case 2:
        IdentResetAll();
        break;
    case 3:
        IdentNotify(1);
    case 4:
        for (i = 0; i < NUM_SLOTS; i++) {
            if (lbl_8055D46C->slots[i].state != 0) {
                lbl_8055D46C->slots[i].state = 1;
            }
        }
        break;
    case 5:
        for (i = 0; i < NUM_SLOTS; i++) {
            if (lbl_8055D46C->slots[i].state != 0) {
                lbl_8055D46C->slots[i].state = 2;
            }
        }
        break;
    }
}

void IdentResetAll(void) {
    int n;
    const char* src;
    int i;

    lbl_8055D46C->current = 0;
    for (i = 0; i < NUM_SLOTS; i++) {
        src = "";
        n = NAME_LEN;
        char* dst;

        lbl_8055D46C->slots[i].state = 0;
        dst = lbl_8055D46C->slots[i].name;
        while ((*dst++ = *src++) != '\0') {
            if (--n <= 0) {
                dst[-1] = '\0';
                break;
            }
        }
    }
    for (i = 0; i < NUM_SLOTS; i++) {
        const char* src;
        int n;
        char* dst;

        fn_801345D8(lbl_8055D46C->default_names[i], NAME_LEN, "PlayerName:%d", i + 1);
        src = fn_80131234(lbl_8055D46C->default_names[i]);
        n = NAME_LEN;
        dst = lbl_8055D46C->default_names[i];
        while ((*dst++ = *src++) != '\0') {
            if (--n <= 0) {
                dst[-1] = '\0';
                break;
            }
        }
        lbl_8055D46C->unkAC[i] = -1;
    }
}

void IdentNotify(int event) {
    IdentCallback* cb;
    int i;

    for (i = 0, cb = lbl_8034F628; i < lbl_8055D460; cb++, i++) {
        (*cb)(lbl_8055D46C->current, event);
    }
}
