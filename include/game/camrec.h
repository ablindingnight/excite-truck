#ifndef GAME_CAMREC_H
#define GAME_CAMREC_H

#include "game/common.h"

// The per-player camera record kept by cammgr (four of them) and copied by namedcam.

// A camera pose: rotation then position.
struct CamMat3 {
    f32 m[9];
};

struct CamVec3 {
    f32 v[3];
};

struct CamPose {
    CamPose(void) {}
    CamMat3 rot;  // offset 0x0
    CamVec3 pos;  // offset 0x24
};

// Mount camera timing (plain data; copied word by word).
struct CamMountTime {
    u32 w[4];   // offset 0x0
    f32 start;  // offset 0x10, when the mount last changed
};

// Per-camera flag that a copy-constructed record leaves alone (its copy constructor copies nothing);
// assignment copies it.
struct CamLink {
    CamLink(void) {}
    CamLink(const CamLink&) {}
    u8 b;
};

// The camera before the last switch, for blending away from it.
struct CamPrev {
    f32 start;     // offset 0x0, when the switch happened
    f32 fov;       // offset 0x4
    CamPose pose;  // offset 0x8
};

// One player's camera record.
struct CamRec {
    CamRec(void);  // fn_8026A600 (out of line, after this unit)
    s32 w0;             // offset 0x0
    s32 w4;             // offset 0x4
    f32 f8;             // offset 0x8
    f32 fC;             // offset 0xC
    s32 mode;           // offset 0x10
    s32 nextMode;       // offset 0x14
    s32 w18;            // offset 0x18
    s32 w1C;            // offset 0x1C
    s32 target;         // offset 0x20
    s32 nextTarget;     // offset 0x24
    u32 w28[3];         // offset 0x28
    u32 w34[3];         // offset 0x34
    CamVec3 eye;        // offset 0x40
    u32 w4C[3];         // offset 0x4C
    CamPose pose;       // offset 0x58
    CamPose lastPose;   // offset 0x88
    s32 lastFrame;      // offset 0xB8
    f32 fov;            // offset 0xBC
    CamPose carFrame;   // offset 0xC0
    u32 wF0[6];         // offset 0xF0
    u32 w108;
    u32 w10C;
    u32 w110;
    u32 w114;
    u32 w118;
    u32 w11C;
    CamVec3 vel;        // offset 0x120
    CamVec3 carVel;     // offset 0x12C
    u8 b138;            // offset 0x138
    u8 b139;            // offset 0x139
    f32 f13C;           // offset 0x13C
    u8* car;            // offset 0x140
    u8* lastCar;        // offset 0x144
    u32 w148;           // offset 0x148
    u32 w14C;           // offset 0x14C
    u32 w150;           // offset 0x150
    f32 f154;           // offset 0x154
    f32 f158;           // offset 0x158
    f32 f15C;           // offset 0x15C
    u32 w160[3];        // offset 0x160
    f32 f16C;           // offset 0x16C
    f32 f170;           // offset 0x170
    f32 f174;           // offset 0x174
    u32 w178[2];        // offset 0x178
    f32 f180;           // offset 0x180
    f32 f184;           // offset 0x184
    CamLink link;       // offset 0x188
    f32 f18C;           // offset 0x18C
    f32 f190;           // offset 0x190
    f32 f194;           // offset 0x194
    f32 f198;           // offset 0x198
    f32 f19C;           // offset 0x19C
    f32 f1A0;           // offset 0x1A0
    u32 w1A4[12];       // offset 0x1A4
    f32 f1D4;           // offset 0x1D4
    CamMountTime mount; // offset 0x1D8
    s16 h1EC;           // offset 0x1EC
    CamPrev prev;       // offset 0x1F0
};

#endif
