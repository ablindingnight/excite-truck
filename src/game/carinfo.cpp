// Car info table (0x801EA4C4-0x801EAB2C): per-slot car setup and driver parameters for up to eight
// cars, plus the race order. Names are guesses apart from the allocation tag.
#include "game/common.h"

extern "C" {
void* memset(void* p, int c, size_t n);
int fn_80265D7C(int car);
int fn_802794AC(u32 type, void* car);
}

void* operator new(size_t size, const char* tag);

struct CarSetup {
    CarSetup(void) {}
    s32 type;        // offset 0x0
    char name[0x20];   // offset 0x4
    char model[0x20];  // offset 0x24
    char paint[0x20];  // offset 0x44
    u32 x64;         // offset 0x64
    u32 x68;         // offset 0x68
    u32 x6C;         // offset 0x6C
    u8 x70;          // offset 0x70
    u32 x74;         // offset 0x74
};

struct CarParams {
    CarParams(void) {}
    u32 x0;   // offset 0x0
    s32 pos;  // offset 0x4
    u32 x8;   // offset 0x8
    f32 xC;   // offset 0xC
    u32 x10;  // offset 0x10
    f32 x14;  // offset 0x14
    f32 x18;  // offset 0x18
    f32 x1C;  // offset 0x1C
    f32 x20;  // offset 0x20
    u32 x24;  // offset 0x24
    u8 x28;   // offset 0x28
    f32 x2C;  // offset 0x2C
    u32 x30;  // offset 0x30
    u32 x34;  // offset 0x34
};

struct CarInfo {
    u8 active;         // offset 0x0
    s32 index;         // offset 0x4
    s32 x8;            // offset 0x8
    CarParams params;  // offset 0xC
    CarSetup setup;    // offset 0x44
};

struct CarMgr {
    CarInfo cars[8];  // offset 0x0
    s32 count;        // offset 0x5E0
    s32 humans;       // offset 0x5E4
    s32 order[8];     // offset 0x5E8
};

static inline int IsRemote(int car, s32 type) {
    int r = 0;
    if (type == 'R' && fn_80265D7C(car)) {
        r = 1;
    }
    return r;
}

static inline int IsDriven(int car, s32 type) {
    int r = 1;
    if (type != 'P' && !IsRemote(car, type)) {
        r = 0;
    }
    return r;
}

ModuleTag lbl_8055EB70;
CarMgr* lbl_8055EB78;
s32 lbl_8055EB7C;

extern "C" {

void fn_801EA4C4(void) {
    int i;
    lbl_8055EB7C = 1;
    lbl_8055EB78 = (CarMgr*)operator new(sizeof(CarMgr), "CarMgr::info");
    memset(lbl_8055EB78, 0, sizeof(CarMgr));
    for (i = 0; i < 8; i++) {
        lbl_8055EB78->order[i] = i;
    }
}

void fn_801EA56C(void) {
    delete lbl_8055EB78;
    lbl_8055EB78 = 0;
    lbl_8055EB7C = 0;
}

void fn_801EA59C(int idx, s32 x8, const CarSetup* setup) {
    CarInfo* c = &lbl_8055EB78->cars[idx];
    s32 type = setup->type;
    c->setup = *setup;
    c->active = IsDriven(idx, type);
    c->x8 = x8;
    c->index = idx;
    if (idx >= lbl_8055EB78->count) {
        lbl_8055EB78->count = idx + 1;
    }
    if (fn_802794AC(setup->type, 0)) {
        if (idx >= lbl_8055EB78->humans) {
            lbl_8055EB78->humans = idx + 1;
        }
    }
}

s32 fn_801EA994(void) { return lbl_8055EB78->humans; }

CarInfo* fn_801EA9A0(int i) { return &lbl_8055EB78->cars[i]; }

CarInfo* fn_801EA9B0(int i) { return &lbl_8055EB78->cars[lbl_8055EB78->order[i]]; }

CarInfo* fn_801EA9CC(int i) { return &lbl_8055EB78->cars[i]; }

void fn_801EA9DC(int idx, const CarParams* p) {
    int pos;
    lbl_8055EB78->cars[idx].params = *p;
    pos = p->pos - 1;
    if (pos < 0) {
        pos = lbl_8055EB78->count - 1;
    }
    if (lbl_8055EB78->count - 1 < pos) {
        pos = lbl_8055EB78->count - 1;
    }
    lbl_8055EB78->order[pos] = idx;
}

void fn_801EAA94(int idx, s32 type) {
    CarInfo* c = &lbl_8055EB78->cars[idx];
    c->setup.type = type;
    c->active = IsDriven(idx, type);
}

void fn_801EAB10(int idx, s32 x8) { lbl_8055EB78->cars[idx].x8 = x8; }

}  // extern "C"
