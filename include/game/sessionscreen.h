#ifndef GAME_SESSIONSCREEN_H
#define GAME_SESSIONSCREEN_H

#include "game/common.h"

// A list screen's data source (garage.cpp's chassis/event lists implement it; gamedo deletes one). Slot names are
// guesses.
class SessionScreen {
public:
    virtual ~SessionScreen(void) {}
    virtual void Add(const char* name, u8 unlocked, u8 extreme) = 0;  // 0x0C
    virtual void SetValue(int i, s32 v) = 0;          // 0x10
    virtual int Count(void) = 0;                      // 0x14
    virtual const char* Name(int i) = 0;              // 0x18
    virtual int Unlocked(int i) = 0;                  // 0x1C
    virtual const char* ExtremeName(int i) = 0;       // 0x20
    virtual int IsExtreme(int i) = 0;                 // 0x24
    virtual const char* Title(int i) = 0;             // 0x28
};

#endif
