#ifndef GAME_RPACKET_H
#define GAME_RPACKET_H

#include "game/common.h"
#include "game/rvec.h"

// A body's state record in the physics packet stream: CollVolume::vf34 writes the packed rotation and position
// (vf2C sizes it 0x14), RigidBody adds its velocity after it. Names are guesses.
struct RPacket {
    u16 rot[3];  // offset 0x0
    u8 pad6[2];
    RVec3 pos;   // offset 0x8
    RVec3 vel;   // offset 0x14 (RigidBody)
};

#endif
