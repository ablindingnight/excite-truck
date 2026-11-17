#ifndef GAME_IDEALLINE_H
#define GAME_IDEALLINE_H

#include "game/common.h"
#include "game/v2.h"
#include "game/centerline.h"
#include "game/ilpos.h"

// The track's ideal/centre line and its followers (idealline.cpp), shared by its users (port: one definition of each
// class). Moved verbatim from idealline.cpp, ObstaclesVtable 2026-11-16.

class LineFollower {
public:
    LineFollower(void);
    virtual ~LineFollower(void);
    virtual int SetLine(CenterLine* line);
    virtual void Attach(CenterLine* line, V2* pos, V2* dir) {
        if (SetLine(line) == 1) {
            SetPosition(pos, dir, 0);
        }
    }

    void Clear(void);
    void Init(void);
    int Update(V2* pos);
    void SetPosition(V2* pos, V2* dir, IdealPoint* start);
    void Locate(V2* pos, V2* dir, ILPos* out, int unused) const;
    f32 Project(IdealPoint* s, f32 t, V2* p);
    int Step(V2* pos);

    ILPos il;      // offset 0x4
    V2 dir;        // offset 0x10
    V2 off;        // offset 0x18
    V2 pos;        // offset 0x20
    u8 wide;       // offset 0x28
    CenterLine* line;  // offset 0x2C
    LineFollower* link;  // offset 0x30
};

struct TimeArray {
    TimeArray(void) {
        cap = 0;
        n = 0;
        data = 0;
    }
    ~TimeArray(void) {
        if (data) {
            delete[] data;
        }
        data = 0;
    }
    void Init(s32 count, s32 extra);

    f32* data;  // offset 0x0
    s32 n;      // offset 0x4
    s32 cap;    // offset 0x8
};

class IdealFollower : public LineFollower {
    bool IsTrackJump(bool moved, ILPos& old, bool jump);

public:
    IdealFollower(int withTimes, CenterLine* line);
    // No user destructor: retail's is implicit (no vptr store where it is inlined; emitted before the in-class
    // Attach pair at the end of idealline.cpp). Aids audit 2, 2026-11-17.
    virtual int SetLine(CenterLine* line);
    virtual void Attach(CenterLine* line, V2* pos, V2* dir) {
        if (SetLine(line) == 1) {
            Start(pos, dir, 1, 0);
        }
    }

    void Reset(void);
    void Start(V2* pos, V2* dir, int resetTimes, IdealPoint* start);
    void ClearFlag(void);
    int Track(V2* pos, f32* progress);
    void UpdatePerp(void);

    u32 Rel(u32 x) const {
        u32 b = base;
        if (b) {
            return (x - b + line->total) % line->total;
        }
        return x;
    }

    u32 base;       // offset 0x34
    V2 perp;        // offset 0x38
    f32 lateral;    // offset 0x40
    f32 latSpeed;   // offset 0x44
    f32 travel;     // offset 0x48
    f32 speed;      // offset 0x4C
    TimeArray times;  // offset 0x50
    IdealPoint* last;  // offset 0x5C
    IdealPoint* prev;  // offset 0x60
    u8 flag;        // offset 0x64
};

#endif
