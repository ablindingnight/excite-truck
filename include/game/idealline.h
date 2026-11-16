#ifndef GAME_IDEALLINE_H
#define GAME_IDEALLINE_H

#include "game/common.h"

// The track's ideal/centre line and its followers (idealline.cpp), shared by its users (port: one definition of each
// class). Moved verbatim from idealline.cpp, ObstaclesVtable 2026-11-16.

struct V2 {
    V2(void) {}
    V2(f32 ax, f32 ay) {
        x = ax;
        y = ay;
    }
    f32 x;
    f32 y;
};

struct IdealPoint {
    V2 pos;            // offset 0x0
    V2 dir;            // offset 0x8
    u32 ilpos;         // offset 0x10 (16.16)
    IdealPoint* next;  // offset 0x14
    f32 len;           // offset 0x18
    f32 dist;          // offset 0x1C
    f32 speed;         // offset 0x20
    u8 pad24[4];
    f32 width;         // offset 0x28
    u8 pad2C[8];
    u8 type;           // offset 0x34
    u8 section;        // offset 0x35
    u8 pad36[6];
};

struct CenterLine {
    u8 pad0[8];
    f32 len[4];          // offset 0x8 (per point type)
    u32 total;           // offset 0x18 (16.16)
    IdealPoint* points;  // offset 0x1C
    IdealPoint* start;   // offset 0x20
};

static inline f32 ILerpT(f32 t, f32 a, f32 b) { return a + t * (b - a); }

struct ILPos {
    ILPos(void) {
        seg = 0;
        mode = 10000;
        t = 0.0f;
    }
    ILPos(IdealPoint* s, f32 f, s32 m) {
        seg = s;
        t = f;
        mode = m;
    }
    // Point and heading on the line at this position (method names unproven).
    void Pos(V2* o) const {
        o->x = ILerpT(t, seg->pos.x, seg->next->pos.x);
        o->y = ILerpT(t, seg->pos.y, seg->next->pos.y);
    }
    void Dir(V2* o) const {
        o->x = ILerpT(t, seg->dir.x, seg->next->dir.x);
        o->y = ILerpT(t, seg->dir.y, seg->next->dir.y);
    }
    IdealPoint* seg;  // offset 0x0
    f32 t;            // offset 0x4
    s32 mode;         // offset 0x8
};

class LineFollower {
public:
    LineFollower(void);
    virtual ~LineFollower(void);
    virtual int SetLine(CenterLine* line);
    virtual void Attach(CenterLine* line, V2* pos, V2* dir);

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
    s32 unk30;     // offset 0x30
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
    virtual ~IdealFollower(void);
    virtual int SetLine(CenterLine* line);
    virtual void Attach(CenterLine* line, V2* pos, V2* dir);

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
