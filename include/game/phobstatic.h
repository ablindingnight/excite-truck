#ifndef GAME_PHOBSTATIC_H
#define GAME_PHOBSTATIC_H

#include "game/common.h"
#include "game/collvol.h"

struct BodyDesc;

// A static physics object: a collision volume with no rigid body (collvol.cpp; phob.cpp constructs one). Names
// are guesses.
class PhobStatic : public CollVolume {
public:
    PhobStatic(void);
    virtual void Init(BodyDesc* d);
    virtual ~PhobStatic(void);
    virtual void vf28(u8* buf);
};

#endif
