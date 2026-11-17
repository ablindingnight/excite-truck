#ifndef GAME_STUNTWATCH_H
#define GAME_STUNTWATCH_H
// Something told about every awarded stunt (achievements, statistics, the tutorial): stunts.cpp's trackers
// notify them. Names unproven.
#include "game/common.h"

class StuntWatcher {
public:
    virtual void Notify(s32 type, int car, int rank) = 0;  // 0x08
};

#endif
