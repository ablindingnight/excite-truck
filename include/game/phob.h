#ifndef GAME_PHOB_H
#define GAME_PHOB_H

#include "game/common.h"

struct PhobMsg;
struct RPacket;
struct PhobCar;

// A physics object (phob.cpp): pose matrices plus the virtual interface the physics loop drives. ringchal's
// teleport check takes one (retail CheckTeleport__...ObjSetFP4Phob) and reads its position, mtx.origin. Slot and
// field names are unproven unless noted.
class Phob {
public:
    virtual void Init(PhobMsg* m);       // 0x8
    virtual ~Phob(void);                 // 0xC
    virtual void Reset(void);            // 0x10
    virtual void vf14(void);             // 0x14
    virtual void vf18(void);             // 0x18
    virtual void Update(void);           // 0x1C
    virtual void vf20(void);             // 0x20
    virtual void vf24(void);             // 0x24
    virtual void vf28(u8* buf);          // 0x28
    virtual s32 vf2C(RPacket* p);        // 0x2C
    virtual void vf30(void);             // 0x30
    virtual void vf34(void);             // 0x34
    virtual void vf38(void);             // 0x38
    virtual PhobCar* vf3C(void);         // 0x3C
    virtual int vf40(void);              // 0x40
    virtual int vf44(void);              // 0x44
    s32 a4;               // offset 0x4
    s32 a8;               // offset 0x8
    u8 padC[0x3C - 0xC];
    PlainValMatrix mtx;   // offset 0x3C
    s16 hint[2];          // offset 0x6C
    PlainValMatrix world; // offset 0x70
    PlainValMatrix prev;  // offset 0xA0
};

#endif
