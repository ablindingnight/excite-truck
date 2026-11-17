#ifndef GAME_RACEINFO_H
#define GAME_RACEINFO_H

#include "game/common.h"

struct RaceOpts;      // game/raceopts.h
struct RaceProgress;  // game/raceprog.h
struct CarInfo;       // game/carinfo.h
struct CarDrawState;  // game/carbody.h
struct CarParams;     // game/carinfo.h

// The current race's overlay info (overlay.cpp's lbl_8034EE84, reached through lbl_8055D350): the event, its
// progress and the viewed player's car record (racer = fn_801EA9A0's CarInfo, truck = its draw state, stats = its
// params). One declaration for overlay and racing. Names are guesses.
struct RaceInfo {
    u8 split;             // offset 0x0
    u8 active;            // offset 0x1
    s32 mode;             // offset 0x4
    RaceOpts* event;      // offset 0x8
    RaceProgress* info;   // offset 0xC
    f32 time;             // offset 0x10
    s32 numPlayers;       // offset 0x14
    s32 numViews;         // offset 0x18
    s32 player;           // offset 0x1C
    u32 flags;            // offset 0x20
    s32 view;             // offset 0x24
    CarInfo* racer;       // offset 0x28
    CarDrawState* truck;  // offset 0x2C
    CarParams* stats;     // offset 0x30
};

#endif
