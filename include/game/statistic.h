#ifndef GAME_STATISTIC_H
#define GAME_STATISTIC_H

#include "game/common.h"
#include "game/vehchoice.h"

class StatEntry;  // stats.cpp

// A tracked statistic (stats.cpp): one identity/type/event/slot record with a default and a committed value; Real
// and Int variants. awards, carpick, evpath, frname, mstats and recscreen use it. Names are guesses.
class Statistic {
public:
    Statistic(int identity, u32 type, s32 event, int slot);
    s32 GetEvent(void);
    u32* GetDefaultPtr(void);
    u32* GetValuePtr(void);
    void SetRaw(u32 value);
    BOOL HasValue(void);
    u32 GetType(void);
    BOOL HasDefault(void);
    char* Format(char* buf, int size);
    VehicleChoice GetChoice(void);
    BOOL IsBetterThan(const Statistic* other) const;
    void Commit(void);
    void Revert(void);

    s32 identity;       // offset 0x0
    u32 type;           // offset 0x4
    s32 event;          // offset 0x8
    s32 slot;           // offset 0xC
    u8 has_default;     // offset 0x10
    u32 default_value;  // offset 0x14
    StatEntry* entry;   // offset 0x18
    u32 value;          // offset 0x1C
    u8 dirty;           // offset 0x20
};

class RealStatistic : public Statistic {
public:
    RealStatistic(int identity, u32 type, s32 event, int slot);
    f32 GetDefault(void);
    f32 GetValue(void);
    void SetValue(f32 v);
    BOOL IsNewRecord(f32 v);
};

class IntStatistic : public Statistic {
public:
    IntStatistic(int identity, u32 type, s32 event, int slot);
    s32 GetDefault(void);
    s32 GetValue(void);
    void SetValue(s32 v);
    BOOL IsNewRecord(s32 v);
};

#endif
