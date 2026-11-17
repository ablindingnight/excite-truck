#ifndef GAME_OBJDESC_H
#define GAME_OBJDESC_H
// A track object class read from a value table (trackobj.cpp; terrpick.cpp defines the empty Props/Draw
// defaults) and its shadow description. Names unproven.
#include "game/common.h"
#include "game/oframe.h"
#include "game/plainvec.h"

struct PlacedObj;

struct ShadowGroup {
    PVec normal;  // offset 0x0
    s32 id;       // offset 0xC
};

struct ShadowDesc {
    const char* model;      // offset 0x0
    f32 extend;             // offset 0x4
    f32 cutoff;             // offset 0x8
    ShadowGroup groups[8];  // offset 0xC
    s32 count;              // offset 0x8C
};

// An object class read from a value table (0x10C).
class ObjDesc {
public:
    char name[0x20];    // offset 0x0
    ObjDesc* next;      // offset 0x20: next class
    ObjDesc* sibling;   // offset 0x24: next part of a composite
    f32 drop;           // offset 0x28
    u8 forbidden;       // offset 0x2C
    OFrame frame;       // offset 0x30
    u8 hasFrame;        // offset 0x60
    char model[0x10];   // offset 0x61
    void* handle;       // offset 0x74
    ShadowDesc shadow;  // offset 0x78

    ObjDesc(void);
    ObjDesc(int);  // inline, defined in trackobj.cpp
    virtual ~ObjDesc(void) {}
    virtual void Read(PlacedObj* o, ValueNode* vn);
    virtual void Create(const PlacedObj* o) = 0;
    virtual void Draw(const OFrame* f) const;
    virtual void Props(void* p, PlacedObj* o);
    virtual f32 vf1C(void);
    virtual void vf20(void);
    virtual void vf24(void);
};

#endif
