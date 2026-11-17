#ifndef GAME_STATEPACKET_H
#define GAME_STATEPACKET_H

#include "game/common.h"

// One car's slot in the physics state packet (0xE0).
struct CarStateOut {
    u8 pad0[0x6C];
    s32 count;  // offset 0x6C
    u32 id;     // offset 0x70
    u8 pad74[0xE0 - 0x74];
};

struct PhysStats {
    s32 a;  // offset 0x0
    s32 b;  // offset 0x4
    f32 c;  // offset 0x8
    f32 d;  // offset 0xC
    f32 e;  // offset 0x10
};

// The physics state packet (phob.cpp allocates it as "PhysMessage"; fn_80179824 / PhysicsGetStatePacked hand it
// out): per-car state, the views, stats, forces and the message/event/phob streams. physics.cpp reads fade.
// Names are guesses.
struct StatePacket {
    f32 time;                  // offset 0x0
    CarStateOut cars[4];       // offset 0x4
    s32 views[4];              // offset 0x384
    s32 nViews;                // offset 0x394
    s32 serial;                // offset 0x398
    u8 pad39C[0x57C - 0x39C];
    PhysStats stats;           // offset 0x57C
    u8 pad590[0xDB0 - 0x590];
    s32 fade;                  // offset 0xDB0
    f32 forces[75];            // offset 0xDB4
    u8 msgs[0x1004];           // offset 0xEE0
    u8 events[0x1004];         // offset 0x1EE4
    u8 phobs[1];               // offset 0x2EE8
};

#endif
