#ifndef GAME_ZONERESULTS_H
#define GAME_ZONERESULTS_H

#include "game/common.h"

// A zone challenge's results table (zone, crushmaster, ringchal): car count, humans, flags and one entry per car
// (car, place, score, flag). Names are guesses.
struct ZoneResults {
    struct Entry {
        s32 car;    // offset 0x0
        s32 place;  // offset 0x4
        s32 score;  // offset 0x8
        u8 padC[0x10 - 0xC];
        bool b10;   // offset 0x10
        u8 pad11[3];
    };
    s32 nCars;    // offset 0x0
    s32 nHumans;  // offset 0x4
    u8 pad8[0x4];
    u8 bC;    // offset 0xC
    u8 done;  // offset 0xD
    u8 bE;    // offset 0xE
    u8 padF;
    Entry entries[8];  // offset 0x10
};

#endif
