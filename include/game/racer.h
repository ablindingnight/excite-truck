#ifndef GAME_RACER_H
#define GAME_RACER_H
// What a car model is built from (0x11C): world.cpp's fn_801E8F10 builds one per car on the stack and hands it to
// CarModel/PlayerCarModel (carmodel.cpp), which fill in the tail. Names unproven (no retail
// symbols; Racer is the name our CarModel constructors use).
#include "game/common.h"
#include "game/racesetup.h"

struct Racer {
    Racer(const CarDef& d, int i);  // inline, defined in world.cpp

    s32 tag;          // offset 0x0: 'PCAR' for the player's car (AnimObj::Bind name)
    s32 model;        // offset 0x4
    u8 pad8[0x10];
    u8 zero[0x50];    // offset 0x18
    CarDef def;       // offset 0x68
    s32 index;        // offset 0xE0
    s32 lodModel;     // offset 0xE4
    s32 color;        // offset 0xE8
    char xrayMesh[32]; // offset 0xEC
    f32 wheelSize[4]; // offset 0x10C
};

#endif
