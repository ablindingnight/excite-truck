// Car rivals (0x8027AA08-0x8027ABF8): when enabled, each racing car looks through the other cars of
// the current race and picks the one it rates lowest in its registry map; past a threshold the pair
// is reported (to a hook that is empty in the shipped game). The unit's other data is unreferenced
// here (its users were stripped). Names are guesses.
#include "game/common.h"
#include "game/racesetup.h"
#include "game/carreg.h"

// RegPair: game/carreg.h.
// RegMap: game/carreg.h.
// RegUser: game/carreg.h.
// RegEntry: game/carreg.h.
// RegTable: game/carreg.h.
// Stripped from the shipped game: its delete[] makes the compiler emit RegTable's weak dtor right here, at the
// start of the unit (0x8027AA08), before fn_8027AA48.
void fn_RegTableUser(RegTable* t) { delete[] t; }  /* Helper name unproven */

extern "C" {
RegUser* fn_8027ABF8(s32 id);
s32 fn_8027AC08(RegUser* u, s32 key);
void fn_8013CA3C(int a, int b);  // empty in the shipped game
}

// One car of the race setup (8 of them from 0x20): world.cpp's CarDef (0x78); was named RaceCar here, the
// 0x25F8 car of carelem/cargear.
struct RivalRace {
    u8 pad0[0xC];
    RaceSetup setup;  // offset 0xC
    u8 pad404[0x450 - 0x404];
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

int fn_8027AA48(void) { return lbl_8055F634; }

void fn_8027AA50(void) {
    if (lbl_8055F634) {
        for (int i = 0; i < lbl_804A3EE0.setup.cars.count; i++) {
            RegUser* u = fn_8027ABF8(lbl_804A3EE0.setup.cars.defs[i].driver);
            s32 best = -1;
            int bestIdx = -1;
            for (int j = 0; j < lbl_804A3EE0.setup.cars.count; j++) {
                if (j == i) {
                    continue;
                }
                s32 id = lbl_804A3EE0.setup.cars.defs[j].driver;
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
