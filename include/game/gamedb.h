#ifndef GAME_GAMEDB_H
#define GAME_GAMEDB_H
// The game database records (events, tracks, unlockables, vehicles, drivers) and their getters, defined in
// pathcareer.cpp. One declaration for every unit (port ODR work, 2026-11-16): other units still declare the
// getters with local views (Eve, Track, TrackDesc, void*) and join as they are converted. Field names unproven.
#include "game/common.h"

// An event (also what FindTrackEvent returns: the track's i-th event record).
struct EventInfo {
    s32 id;      // offset 0x0
    s32 track;   // offset 0x4
    u32 type;    // offset 0x8
    u8 padC[0x20 - 0xC];
    ValueNode* node;  // offset 0x20
};

struct TrackInfo {
    u8 pad0[0x8];
    char name[0x68];   // offset 0x8
    s32 events;        // offset 0x70: number of events on the track
    u8 pad74[0x80 - 0x74];
    ValueNode* node;   // offset 0x80
};

struct Unlockable {
    const char* type;  // offset 0x0
    const char* name;  // offset 0x4
    s32 points;        // offset 0x8
};

struct VehicleInfo {
    u8 pad0[0x4];
    s32 a4;          // offset 0x4
    s32 a8;          // offset 0x8
    u8 padC[0x2C - 0xC];
    char name[0x74];  // offset 0x2C
    u32 flags;        // offset 0xA0
};

struct DriverInfo {
    u8 pad0[0x4];
    s32 id;       // offset 0x4
    u8 pad8[0x48 - 0x8];
    s32 bonus;    // offset 0x48: skill bonus
    s32 vehicle;  // offset 0x4C
    s32 a50;      // offset 0x50
    u8 pad54[0x58 - 0x54];
    ValueNode* node;  // offset 0x58: the driver's settings ("override_skill", ...)
};

EventInfo* GetEvent(int i);
TrackInfo* GetTrack(int i);
EventInfo* FindTrackEvent(int track, int i);
DriverInfo* GetDriver(int i);

#endif
