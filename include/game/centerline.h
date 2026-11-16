#ifndef GAME_CENTERLINE_H
#define GAME_CENTERLINE_H
// A track's centre line as loaded from its ideal-line resource (idealres.cpp relocates it in place; idealline.cpp
// walks it as CenterLine: "CenterLine::convert_meters_to_ilpos"). The points form a ring; a point's
// ilpos is its 16.16 position along the line. One declaration for every unit (port ODR work, 2026-11-16; names other
// than CenterLine unproven).
#include "game/common.h"
#include "game/v2.h"

struct ResExt;  // idealres.cpp: an extension block chained after the points

struct IdealPoint {
    V2 pos;            // offset 0x0
    V2 dir;            // offset 0x8
    u32 ilpos;         // offset 0x10 (16.16)
    IdealPoint* next;  // offset 0x14
    f32 len;           // offset 0x18
    f32 dist;          // offset 0x1C
    f32 speed;         // offset 0x20
    f32 unk24;         // offset 0x24
    f32 width;         // offset 0x28
    f32 height;        // offset 0x2C (idealres: lateral)
    f32 lon;           // offset 0x30
    u8 type;           // offset 0x34
    u8 section;        // offset 0x35
    u8 pad36[6];
};

// The resource's header: its size and the size of the relocated segment.
struct ResHeader {
    u32 resSize;  // offset 0x0
    u32 segSize;  // offset 0x4
};

// The resource as loaded and relocated (idealres.cpp's messages name it: "ideal: bad IdealLineRes version").
struct IdealLineRes {
    ResHeader hdr;       // offset 0x0
    f32 len[4];          // offset 0x8 (per point type)
    u32 total;           // offset 0x18 (16.16)
    IdealPoint* points;  // offset 0x1C
    IdealPoint* start;   // offset 0x20
    ResExt* ext;         // offset 0x24
    void* blips;         // offset 0x28
    s32 nrelocs;         // offset 0x2C
    u32 relocs[1];       // offset 0x30
};

// The line the followers walk (idealline.cpp: "CenterLine::convert_meters_to_ilpos"): the loaded resource itself.
struct CenterLine : IdealLineRes {
    int Count(void) const { return total >> 16; }  // points in the ring (total = count << 16)
};

#endif
