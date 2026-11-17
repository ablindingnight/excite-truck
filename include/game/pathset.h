#ifndef GAME_PATHSET_H
#define GAME_PATHSET_H

#include "game/common.h"

struct IdealLineRes;

// The set of lines the AI follows (ilresmgr's RacingLines is the one fn_80156A40 returns). The destructor is the
// LAST slot, so 0x08 is a plain virtual. One declaration for ilresmgr, deity and orbit (port vtable views, 2026-11-16).
class PathSet {
public:
    virtual IdealLineRes* GetLine(int i) = 0;              // 0x08 centre line
    virtual IdealLineRes* GetRacingLine(int i) = 0;        // 0x0C
    virtual IdealLineRes* CenterFor(int car) = 0;          // 0x10
    virtual IdealLineRes* LineFor(int car) = 0;            // 0x14
    virtual void Snap(int car, const f32* pos) = 0;        // 0x18
    // 0x1C: moves `car` to another racing line. orbit passes a second flag (crazy driving on/off) that
    // RacingLines ignores. Parameter name unproven.
    virtual void Switch(int car, int on) = 0;
    virtual void Reset(void) {}                            // 0x20
    virtual void Assign(void) = 0;                         // 0x24
    virtual ~PathSet(void) {}                              // 0x28
};

extern "C" PathSet* fn_80156A40(void);

#endif
