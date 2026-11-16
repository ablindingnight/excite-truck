#ifndef GAME_RACESETUP_H
#define GAME_RACESETUP_H
// The race setup (0x3F8; world.cpp keeps the current one, fn_801E879C): the track, the eight racers and the race
// options (fn_801E87A8 returns &opts). One declaration for every unit (port ODR work, 2026-11-16); units used to
// spell it with RacerSlot/RacerList, RaceEntry or padding. CarDef keeps its name: it is in the mangled
// __ct__8CarModelFP5RacerPC6CarDef. Names unproven.
#include "game/common.h"
#include "game/raceopts.h"

// A float kept in a one-member struct (copied as a struct).
struct CarDefF {
    f32 v;
};

// One racer of the setup (0x78): who drives what.
struct CarDef {
    CarDef(void) {}
    s32 type;                 // offset 0x0
    char driverName[0x40];    // offset 0x4
    char vehicleName[0x20];   // offset 0x44
    CarDefF unk64;         // offset 0x64
    s32 driver;               // offset 0x68
    s32 vehicle;              // offset 0x6C
    u8 flags;                 // offset 0x70 (low two bits: player)
    s32 unk74;                // offset 0x74
};

struct RaceCars {
    CarDef defs[8];      // offset 0x0
    s32 count;           // offset 0x3C0
};

// The setup's data. gameloop copies a setup into a local of this type: retail does not run RaceSetup's
// constructor there.
struct RaceSetupData {
    char track[0x20];    // offset 0x0
    RaceCars cars;       // offset 0x20
    RaceOpts opts;       // offset 0x3E4
};

struct RaceSetup : RaceSetupData {
    RaceSetup(void);     // world.cpp: zero-fills
};

typedef char RaceSetupSize[sizeof(RaceSetup) == 0x3F8 ? 1 : -1];

#endif
