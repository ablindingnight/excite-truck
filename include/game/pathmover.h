#ifndef GAME_PATHMOVER_H
#define GAME_PATHMOVER_H
// A path follower made by the path manager (pathman.cpp; actseq starts and resets them). Names unproven.
#include "game/common.h"

class PathMover {
public:
    virtual ~PathMover(void) {}
    virtual f32 Length(void) = 0;
    virtual f32 Time(void) = 0;
    virtual void Start(f32 speed) = 0;
    virtual int Done(void) = 0;
    virtual void Reset(void) = 0;
};

#endif
