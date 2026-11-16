#ifndef GAME_CARREG_H
#define GAME_CARREG_H

#include "game/common.h"

// carreg.cpp's registry of up to 12 objects looked up by id (0x8027ABF8-0x8027AD4C). Names are guesses.

struct RegEntry {
    s32 id;  // offset 0x0
};

struct RegPair {
    s32 key;    // offset 0x0
    s32 value;  // offset 0x4
};

struct RegMap {
    s32 Find(s32 key) const {
        int i;
        for (i = 0; i < count; i++) {
            if (key == pairs[i].key) {
                return pairs[i].value;
            }
        }
        return 0;
    }
    RegPair pairs[11];  // offset 0x0
    s32 count;          // offset 0x58
};

struct RegUser {
    u8 pad0[0x30];
    RegMap map;  // offset 0x30
    u8 pad8C[0xE8 - 0x8C];
    s32 regId;   // offset 0xE8
    u8 padEC[0xF0 - 0xEC];
    int IsActive(void) const { return active != 0; }
    s32 active;  // offset 0xF0
    u8 padF4[0xF8 - 0xF4];
    s32 slot;    // offset 0xF8
};

struct RegTable {
    RegTable(void) { count = 0; }
    ~RegTable(void) {}
    RegEntry* items[12];  // offset 0x0
    s32 count;            // offset 0x30
};

#endif
