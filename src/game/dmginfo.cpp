// Recent-damage records per car (0x801CEC80-0x801CEFAC): four slots per car that age out, copied
// into the race's per-player display data. Names are guesses.
#include "game/common.h"

void* operator new[](size_t size, const char* tag);

struct DmgSlot {
    void Update(void) {
        if (id >= 0) {
            age += 0.016683351f;
            if (age > 1.5f) {
                id = -1;
            } else if (age > 0.5f) {
                f32 a = amount - 0.0083416756f;
                if (0.0f > a) {
                    a = 0.0f;
                }
                amount = a;
            }
        }
    }
    s32 id;      // offset 0x0
    f32 age;     // offset 0x4
    f32 amount;  // offset 0x8
};

struct DmgCar {
    void Reset(void) {
        int j;
        for (j = 0; j < 4; j++) {
            slots[j].id = -1;
            slots[j].amount = 0.0f;
            slots[j].age = 0.0f;
        }
    }
    void Update(void) {
        int j;
        for (j = 0; j < 4; j++) {
            slots[j].Update();
        }
    }
    DmgSlot slots[4];
};

struct DmgCarRec {
    u8 pad0[0x268C];
    s32 x268C;  // offset 0x268C
    f32 x2690;  // offset 0x2690
    f32 x2694;  // offset 0x2694
};

struct DmgOut {
    s32 id;      // offset 0x0
    f32 age;     // offset 0x4
    f32 x8;      // offset 0x8
    f32 xC;      // offset 0xC
    f32 amount;  // offset 0x10
    s32 x14;     // offset 0x14
};

struct DmgPlayer {
    u8 pad0[0x60];
    s32 car;  // offset 0x60
    u8 pad64[0x74 - 0x64];
    DmgOut out[4];  // offset 0x74
    u8 padD4[0xE0 - 0xD4];
};

struct DmgRace {
    u8 pad0[4];
    DmgPlayer players[4];  // offset 0x4
    u8 pad384[0x394 - 0x384];
    s32 numPlayers;  // offset 0x394
};

extern "C" {
DmgCarRec* fn_8017960C(int id);
void fn_801B916C(DmgRace* r);
}

ModuleTag lbl_8055E888;
DmgCar* lbl_8055E890;

extern "C" {

void fn_801CECE0(void);

void fn_801CEC80(void) {
    lbl_8055E890 = new ("dmg_car_info") DmgCar[8];
    fn_801CECE0();
}

void fn_801CECB4(void) {
    delete[] lbl_8055E890;
    lbl_8055E890 = 0;
}

void fn_801CECE0(void) {
    int i;
    int j;
    for (i = 0; i < 8; i++) {
        DmgCar* c = &lbl_8055E890[i];
        for (j = 0; j < 4; j++) {
            c->slots[j].id = -1;
            c->slots[j].amount = 0.0f;
            c->slots[j].age = 0.0f;
        }
    }
}

void fn_801CEDDC(void) {
    int i;
    int j;
    DmgCar* c;
    DmgSlot* s;
    for (i = 0; i < 8; i++) {
        c = &lbl_8055E890[i];
        for (j = 0; j < 4; j++) {
            s = &c->slots[j];
            if (s->id >= 0) {
                s->age += 0.016683351f;
                if (s->age > 1.5f) {
                    s->id = -1;
                } else if (s->age > 0.5f) {
                    f32 a = s->amount - 0.0083416756f;
                    if (0.0f > a) {
                        a = 0.0f;
                    }
                    s->amount = a;
                }
            }
        }
    }
}

void fn_801CEEC8(DmgRace* r) {
    DmgPlayer* p;
    DmgSlot* s;
    DmgOut* o;
    int i;
    int j;
    for (i = 0; i < r->numPlayers; i++) {
        p = &r->players[i];
        o = p->out;
        s = lbl_8055E890[p->car].slots;
        for (j = 0; j < 4; j++, s++, o++) {
            o->id = s->id;
            o->age = s->age;
            o->amount = s->amount;
            if (o->id >= 0) {
                DmgCarRec* c = fn_8017960C(o->id);
                o->x8 = c->x2690;
                o->xC = c->x2694;
                o->x14 = c->x268C;
            } else {
                o->x8 = 0.0f;
                o->x14 = 0;
            }
        }
    }
    fn_801B916C(r);
}

}  // extern "C"
