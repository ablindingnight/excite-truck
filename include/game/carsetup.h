#ifndef GAME_CARSETUP_H
#define GAME_CARSETUP_H
// CarSetup: one declaration for the units that each carried an identical copy (carsetup, caruimod, masslist). Port ODR work,
// 2026-11-16 (samesize.py). Names unproven.
#include "game/common.h"
#include "game/valvec3.h"

struct CarSetup {
    f32 mass;             // offset 0x0
    f32 mx;               // offset 0x4
    f32 my;               // offset 0x8
    f32 mz;               // offset 0xC
    f32 ftrack;           // offset 0x10
    f32 rtrack;           // offset 0x14
    f32 wheelbase;        // offset 0x18
    f32 balance;          // offset 0x1C
    f32 collideCmHeight;  // offset 0x20
    f32 cmHeight;         // offset 0x24
    f32 airDrag;          // offset 0x28
    f32 torqueBalance;    // offset 0x2C
    f32 finalDrive;       // offset 0x30
    s32 numGears;         // offset 0x34
    f32 gears[6];         // offset 0x38
    f32 torqueMax;        // offset 0x50
    f32 powerMax;         // offset 0x54
    f32 torqueRpm;        // offset 0x58
    f32 powerRpm;         // offset 0x5C
    f32 drag;             // offset 0x60
    f32 redline;          // offset 0x64
    f32 idleSpeed;        // offset 0x68
    f32 ebHeat;           // offset 0x6C
    f32 ebStrength;       // offset 0x70
    f32 jbTime;           // offset 0x74
    f32 fsway;            // offset 0x78
    f32 rsway;            // offset 0x7C
    WheelSetup wheels[4];  // offset 0x80
    f32 grip;             // offset 0x110
    f32 slip;             // offset 0x114
    f32 loadFactor;       // offset 0x118
    f32 unk11C;           // offset 0x11C
    f32 brake;            // offset 0x120
    f32 speedSteering;    // offset 0x124
    f32 wheelLock;        // offset 0x128
    ValVec3 modelOffset;  // offset 0x12C
};

#endif
