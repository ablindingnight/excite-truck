#ifndef GAME_DEITY_H
#define GAME_DEITY_H
// The race director's per-car record (deity.cpp's DeityCar): the car's race state (DState, returned by State()),
// its line position on the ideal and racing lines, its followers and its car. One declaration for every unit (port
// ODR work, 2026-11-16). Names unproven.
#include "game/common.h"
#include "game/centerline.h"
#include "game/ilpos.h"

class LineFollower;
class IdealFollower;
class DCar;

// Grid slot: the car in front in the same column and the column offset.
struct DGridSlot {
    DCar* ahead;  // offset 0x0
    f32 lane;     // offset 0x4
};

// A car's race state (0x38), DeityCar's bytes 0x4-0x3B.
struct DState {
    u32 flags;   // offset 0x0
    s32 place;   // offset 0x4
    s32 wC;      // offset 0x8
    f32 f10;     // offset 0xC
    s32 lap;     // offset 0x10
    f32 f18;     // offset 0x14
    f32 best;    // offset 0x18
    f32 last;    // offset 0x1C
    f32 qual;    // offset 0x20
    s32 mode;    // offset 0x24
    u8 hold;     // offset 0x28
    f32 holdTime;  // offset 0x2C
    DGridSlot grid;  // offset 0x30
};

// The car a DeityCar oversees (offsets only).
class DCar {
public:
    virtual void v08(void);
    virtual ~DCar(void);  // 0x0C (CarBody family: Init at 0x08, destructor at 0x0C)
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(void);
    virtual void v1C(void);
    virtual void v20(void);
    virtual void v24(void);
    virtual void v28(void);
    virtual void v2C(void);
    virtual void v30(void);
    virtual void v34(void);
    virtual void v38(void);
    virtual void v3C(void);
    virtual void v40(void);
    virtual void v44(void);
    virtual void v48(void);
    virtual void v4C(void);
    virtual void v50(void);
    virtual void Teleport(void* info);  // 0x54
    virtual void Lap(void);             // 0x58
    u8 pad4[0x10 - 0x4];
    u32 type;  // offset 0x10
    u8 pad14[0x54 - 0x14];
    f32 fwd[3];  // offset 0x54
    u8 pad60[0x94 - 0x60];
    f32 pos[3];  // offset 0x94
    u8 padA0[0x1EC - 0xA0];
    s32 il;      // offset 0x1EC
    u8 pad1F0[0x5BC - 0x1F0];
    f32 ahead;   // offset 0x5BC
    u8 pad5C0[0x5EC - 0x5C0];
    f32 boost;   // offset 0x5EC: catch-up
};

class DeityCar {
public:
    DeityCar(void);
    virtual void Reset(void);             // 0x08
    virtual void Update(void);            // 0x0C
    virtual DState* State(void);          // 0x10
    virtual int v14(void);                // 0x14
    virtual IdealFollower* Ideal(void);   // 0x18
    virtual LineFollower* Line(void);     // 0x1C
    virtual DCar* Car(void);              // 0x20
    virtual void v24(void);               // 0x24
    virtual void v28(void* a, void* b, s32* out);  // 0x28
    virtual void v2C(void* a, void* b, s32* out);  // 0x2C
    virtual void Hold(int who);           // 0x30
    virtual void Release(void);           // 0x34
    virtual int Held(void);               // 0x38 (a u8 flag; callers read r3 whole)
    virtual ~DeityCar(void);              // 0x3C
    virtual DState* State2(void);         // 0x40
    void SetRank(s32 r) {
        if (!locked) {
            rank = r;
        }
    }
    u8 state[0x38];        // offset 0x4
    LineFollower* line;    // offset 0x3C
    ILPos ideal;           // offset 0x40
    ILPos racing;          // offset 0x4C
    IdealFollower* follower;  // offset 0x58
    DCar* car;             // offset 0x5C
    s32 index;             // offset 0x60
    s32 stuck;             // offset 0x64
    s32 finishLap;         // offset 0x68
    s32 start;             // offset 0x6C
    f32 finishTime;        // offset 0x70
    s32 rank;              // offset 0x74
    u8 locked;             // offset 0x78
    u8 finished;           // offset 0x79
    s32 ai;                // offset 0x7C
};

#endif
