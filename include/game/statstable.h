#ifndef GAME_STATSTABLE_H
#define GAME_STATSTABLE_H
// The course stats table (mstats.cpp; multievt shows one). Names unproven.
#include "game/common.h"
#include "game/uitable.h"

class StatsTable : public UITable {
public:
    StatsTable(void);
    ~StatsTable(void) {}
    void Refresh(void);
    void Show(s32 event);

    s32 event;  // offset 0x30
};

#endif
