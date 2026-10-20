// Car rivals (0x8027AA08-0x8027ABF8): when enabled, each racing car looks through the other cars of
// the current race and picks the one it rates lowest in its registry map; past a threshold the pair
// is reported (to a hook that is empty in the shipped game). The unit's other data is unreferenced
// here (its users were stripped). Names are guesses.
#include "game/common.h"

struct RegPair {
    s32 key;    // offset 0x0
    s32 value;  // offset 0x4
};

struct RegMap {
    RegPair pairs[11];  // offset 0x0
    s32 count;          // offset 0x58
};

struct RegUser {
    u8 pad0[0x30];
    RegMap map;  // offset 0x30
};

struct RegEntry {
    s32 id;  // offset 0x0
};

struct RegTable {
    RegTable(void) { count = 0; }
    ~RegTable(void) {}
    RegEntry* items[12];  // offset 0x0
    s32 count;            // offset 0x30
};

extern "C" {
RegUser* fn_8027ABF8(s32 id);
s32 fn_8027AC08(RegUser* u, s32 key);
void fn_8013CA3C(int a, int b);  // empty in the shipped game
void fn_801E952C(void* setup);   // race setup init
}

struct RaceCar {
    u8 pad0[0x68];
    s32 id;  // offset 0x68
    u8 pad6C[0xC];
};

struct RaceSetup {
    RaceSetup(void) { fn_801E952C(this); }
    char track[0x20];  // offset 0x0
    RaceCar cars[8];   // offset 0x20
    s32 numCars;       // offset 0x3E0
    u8 pad3E4[0x60];
};

struct RivalRace {
    u8 pad0[0xC];
    RaceSetup setup;  // offset 0xC
};

ModuleTag lbl_8055F628;
static UnkStatic123 lbl_804A21E8;
UnkStatic7F lbl_8055F630;
u8 lbl_804A2200[0x580];
RegTable lbl_804A278C;
RegTable lbl_804A27CC;
u8 lbl_804A2800[0x16E0];
RivalRace lbl_804A3EE0;
u8 lbl_8055F634;   // rivals enabled
s32 lbl_8055F638;  // rating threshold
u8 lbl_8055F63C;

extern "C" {

u8 fn_8027AA48(void) { return lbl_8055F634; }

void fn_8027AA50(void) {
    if (lbl_8055F634) {
        for (int i = 0; i < lbl_804A3EE0.setup.numCars; i++) {
            RegUser* u = fn_8027ABF8(lbl_804A3EE0.setup.cars[i].id);
            s32 best = -1;
            int bestIdx = -1;
            for (int j = 0; j < lbl_804A3EE0.setup.numCars; j++) {
                if (j == i) {
                    continue;
                }
                s32 id = lbl_804A3EE0.setup.cars[j].id;
                if (best == -1 || fn_8027AC08(u, id) < fn_8027AC08(u, best)) {
                    best = id;
                    bestIdx = j;
                }
            }
            if (best != -1 && fn_8027AC08(u, best) > lbl_8055F638) {
                fn_8013CA3C(bestIdx, i);
            }
        }
    }
}

}  // extern "C"
