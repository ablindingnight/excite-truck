#ifndef GAME_GAMEDB_H
#define GAME_GAMEDB_H
// The game database records (events, tracks, unlockables, vehicles, drivers) and their getters (events and
// tracks: pathcareer.cpp; vehicles and drivers: driver.cpp). One declaration for every unit (port ODR work,
// 2026-11-16); fields are the union of what the units read. Field names unproven.
#include "game/common.h"

// An event (also what FindTrackEvent returns: the track's i-th event record).
struct EventInfo {
    s32 id;           // offset 0x0
    s32 track;        // offset 0x4
    u32 type;         // offset 0x8
    s32 laps;         // offset 0xC
    s32 drivers;      // offset 0x10: number of racers
    u32 terrain;      // offset 0x14
    s32 rounds;       // offset 0x18
    s32 purse;        // offset 0x1C
    ValueNode* node;  // offset 0x20: the event's settings
};

struct TrackInfo {
    s32 index;         // offset 0x0
    s32 league;        // offset 0x4
    char name[0x20];   // offset 0x8: the track's file name (GetTrackFriendlyName/GetTrackEnviron key)
    char title[0x40];  // offset 0x28
    s32 sort_major;    // offset 0x68
    s32 sort_minor;    // offset 0x6C
    s32 events;        // offset 0x70: number of events on the track
    u8 pad74[0x80 - 0x74];
    ValueNode* node;   // offset 0x80: the track's settings
};

struct Unlockable {
    const char* type;  // offset 0x0
    const char* name;  // offset 0x4
    s32 points;        // offset 0x8
};

// A vehicle (driver.cpp's table).
struct VehicleInfo {
    s32 index;              // offset 0x0
    s32 id;                 // offset 0x4
    s32 base;               // offset 0x8: id of the non-extreme version
    char display_name[0x20];  // offset 0xC
    char chassis[0x20];     // offset 0x2C
    char texture[0x10];     // offset 0x4C
    char number[8];         // offset 0x5C
    u32 colors[12];         // offset 0x64
    s32 default_color;      // offset 0x94
    s32 num_colors;         // offset 0x98
    s32 unk9C;              // offset 0x9C
    u32 flags;              // offset 0xA0: 2 extreme, 4 shiny
    u32 groups;             // offset 0xA4
};

struct DriverInfo {
    s32 index;        // offset 0x0
    s32 id;           // offset 0x4
    char name[0x40];  // offset 0x8
    s32 rank;         // offset 0x48 (aiskill adds it to the driver's skill score)
    s32 vehicle;      // offset 0x4C
    u32 color;        // offset 0x50
    s32 unk54;        // offset 0x54
    ValueNode* node;  // offset 0x58: the driver's settings ("override_skill", ...)
};

EventInfo* GetEvent(int i);
TrackInfo* GetTrack(int i);
EventInfo* FindTrackEvent(int track, int i);
DriverInfo* GetDriver(int i);

#endif
