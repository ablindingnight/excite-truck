#ifndef GAME_GAMELISTENER_H
#define GAME_GAMELISTENER_H
// GameListener: one declaration for the units that each carried an identical copy (gamedo, gameloop). Port ODR work,
// 2026-11-16 (samesize.py). Names unproven.
#include "game/common.h"
#include "game/bink_internal.h"
#include "game/raceresultentry.h"

// A race's standings (recordmgr.cpp fills them: its Standings view; not recordmgr's per-car RaceResult).
struct RaceStandings {
    s32 count;   // offset 0x0
    s32 unk4;    // offset 0x4
    s32 races;   // offset 0x8
    u8 unkC;     // offset 0xC
    u8 finished; // offset 0xD
    u8 aborted;  // offset 0xE
    RaceResultEntry entries[8];  // offset 0x10
};

class GameListener {
public:
    virtual void Begin(void);              // 0x8
    virtual void End(RaceStandings* result);  // 0xC
    virtual void Finish(void);             // 0x10
};

#endif
