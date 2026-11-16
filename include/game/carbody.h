#ifndef GAME_CARBODY_H
#define GAME_CARBODY_H

#include "game/common.h"
#include "game/rigid.h"

// The car body (0x2700 bytes; vtable emitted by carbody.cpp): the rigid body every car, net car and trailer is.
// One declaration for carbody, netcar, springcar and trailer (port ODR work, 2026-11-16). Most of carbody.cpp still
// reaches the body through byte offsets; the fields from 0x1EC are the census of carbody's own accesses
// (fXXXX = offset, typed by the accessor; 2026-11-16 offset census) plus the names the other units use. Names unproven.

class StatsSrc;
struct CarLayout;

// The contact plane under a wheel.
struct CbPlane {
    f32 nRight;     // offset 0x0, ground normal . body right
    f32 nFwd;       // offset 0x4, ground normal . body forward
    f32 ground[3];  // offset 0x8, contact plane anchor
};

// A wheel's per-step state in the car body (4 of them at 0x23AC).
struct CbWheel {
    CbWheel(void) {}
    f32 load;        // offset 0x0
    f32 loadN;       // offset 0x4
    f32 spin;        // offset 0x8
    f32 slip;        // offset 0xC
    f32 v10;         // offset 0x10
    f32 steer;       // offset 0x14
    f32 v18;         // offset 0x18
    f32 prevSlip;    // offset 0x1C
    u8 grounded;     // offset 0x20
    f32 contact[3];  // offset 0x24
    f32 local[3];    // offset 0x30, body-space mount point
    f32 v3C;         // offset 0x3C
    f32 v40;         // offset 0x40
    CbPlane plane;   // offset 0x44
    f32 normal[3];   // offset 0x58, contact plane normal
    s32 groundH;     // offset 0x64, fn_802B03C0's result (an int)
    f32 comp;        // offset 0x68
    s8 sound;        // offset 0x6C
    u32 flags;       // offset 0x70
};

// A plain 3-vector (carbody's physics tables).
// Plain 3-vector without constructors: copies move words.
struct HVec {
    f32 x, y, z;
};

// One wheel's setup inside CbSetup.
struct CbWheelSetup {
    s32 w[9];
};

// Tuned car setup (0x138 bytes): read from the car's physics tables, kept twice in the body (0x32C, 0x464).
struct CbSetup {
    f32 mass;                                                           // 0x00
    f32 f04, f08, f0C, f10, f14, f18, f1C, f20, f24, f28, f2C, f30;    // 0x04
    s32 i34;                                                            // 0x34
    s32 i38[6];                                                         // 0x38
    f32 f50, f54, f58, f5C, f60, f64, f68, f6C, f70, f74, f78, f7C;    // 0x50
    CbWheelSetup wheels[4];                                             // 0x80
    f32 f110, f114, f118, f11C, f120, f124, f128;                       // 0x110
    HVec modelOffset;                                                   // 0x12C
};

struct EVec {
    f32 x, y, z;
};

struct DMat3 {
    DMat3(void) {}
    f32 m[9];
};

// A part's transform (rotation + position).
struct DFrame {
    DMat3 rot;  // offset 0x0
    EVec pos;   // offset 0x24
};

// One body joint / detachable part slot (18 of them at 0x152C, 0x80 bytes; cardmg's damage parts). carbody resets
// the two frames as six identity rows.
struct CbJoint {
    DFrame* on;     // offset 0x0, the part's bone frame (0: no such part)
    s32 point;      // offset 0x4
    s32 hub;        // offset 0x8, body point the hinge rides on
    f32 angle;      // offset 0xC
    f32 spin;       // offset 0x10
    f32 limit;      // offset 0x14
    DFrame cur;     // offset 0x18
    DFrame prev;    // offset 0x48
    u8 valid;       // offset 0x78
    s32 state;      // offset 0x7C: 0 attached, 1 hanging, 2/4 flying, 3 gone
};

// The body pose in the render snapshot: rotation rows, then the position (copied block by block).
struct SnapRot {
    f32 m[9];
};
struct SnapPos {
    f32 v[3];
};
struct SnapPose {
    SnapRot rot;
    SnapPos pos;
};

// One wheel in the car's draw state (0x6C bytes).
struct CarDrawWheel {
    f32 pos[3];     // offset 0x0: interpolated mount point
    f32 ground[3];  // offset 0xC
    f32 normal[3];  // offset 0x18
    f32 slip;       // offset 0x24
    f32 steer;      // offset 0x28
    f32 v18;        // offset 0x2C
    f32 load;       // offset 0x30
    f32 spin;       // offset 0x34
    f32 comp;       // offset 0x38
    f32 nRight;     // offset 0x3C
    f32 nFwd;       // offset 0x40
    f32 v10;        // offset 0x44
    f32 f48[6];     // offset 0x48
    s32 flags;      // offset 0x60
    u8 grounded;    // offset 0x64
    u8 pad65[3];
    s32 groundH;    // offset 0x68
};

// A deformable body point in the draw state (0x34 bytes; cardmg's damage points).
struct CarDrawPoint {
    u8 pad0[0x24];
    f32 pos[3];  // offset 0x24
    u8 pad30[0x34 - 0x30];
};

// The car's draw state: what CarBody::vf28 writes into the frame buffer at its a8 offset (0xFA8 bytes) for the
// renderer and damage code. Field names unproven.
struct CarDrawState {
    SnapPose pose;            // offset 0x0
    s16 hint;                 // offset 0x30
    u8 pad32[0x34 - 0x32];
    f32 f34;                  // offset 0x34
    f32 f38;                  // offset 0x38
    f32 f3C;                  // offset 0x3C
    f32 f40;                  // offset 0x40
    f32 f44;                  // offset 0x44
    u32 f48;                  // offset 0x48
    f32 speedXZ;              // offset 0x4C
    f32 f50;                  // offset 0x50
    u8 pad54[0x64 - 0x54];
    f32 speed;                // offset 0x64
    u8 pad68[0x6E - 0x68];
    u16 flags;                // offset 0x6E
    f32 vel[3];               // offset 0x70
    CarDrawWheel wheels[4];   // offset 0x7C
    f32 blink;                // offset 0x22C
    u8 pad230[0x238 - 0x230];
    f32 f238;                 // offset 0x238
    f32 f23C;                 // offset 0x23C
    f32 f240;                 // offset 0x240
    u8 pad244[0x28C - 0x244];
    f32 f28C;                 // offset 0x28C
    f32 f290;                 // offset 0x290
    f32 f294;                 // offset 0x294
    s32 f298;                 // offset 0x298
    s32 f29C;                 // offset 0x29C (flag bits 0x10000000/0x20000000)
    u8 pad2A0[0x2A8 - 0x2A0];
    f32 f2A8;                 // offset 0x2A8
    u8 pad2AC[0x2C4 - 0x2AC];
    f32 f2C4;                 // offset 0x2C4
    f32 f2C8;                 // offset 0x2C8
    f32 f2CC;                 // offset 0x2CC
    f32 f2D0;                 // offset 0x2D0
    f32 f2D4;                 // offset 0x2D4
    s32 f2D8;                 // offset 0x2D8
    u8 pad2DC[0xA78 - 0x2DC];
    s32 nPoints;              // offset 0xA78
    u8 points[0xFA4 - 0xA7C]; // offset 0xA7C: CarDrawPoint records
    s32 fFA4;                 // offset 0xFA4
};
typedef char CarDrawStateSize[sizeof(CarDrawState) == 0xFA8 ? 1 : -1];

// The state handed to a body's SetState (vtable 0x54).
struct BodyState {
    RMtx mtx;   // offset 0x0
    RVec3 vel;  // offset 0x30
    s32 a3C;    // offset 0x3C
    s16 hint;   // offset 0x40
};

class CarBody : public RigidBody {
public:
    CarBody(void);
    // The destructor is declared first: MWCC emits the vtable where the first-declared non-inline virtual is
    // defined, and Init's body is an extern "C" aid in carbody.cpp. (Overrides keep their base slots in any order.)
    virtual ~CarBody(void);           // 0x0C
    virtual void Init(BodyDesc* d);   // 0x08
    virtual void Reset(void);         // 0x10
    virtual void vf14(void);          // 0x14
    virtual void Update(void);        // 0x1C (empty)
    virtual void vf20(void);          // 0x20
    virtual void Notify(const RVec3* v, const RVec3* at, int kind, void* c, void* by);  // 0x24
    virtual void vf28(u8* buf);       // 0x28
    virtual s32 vf2C(RPacket* pk);    // 0x2C: 0x44 bytes
    virtual void vf30(f32 t, RPacket* a, const RPacket* b, s32* na, s32* nb);  // 0x30
    virtual s32 vf34(RPacket* pk);    // 0x34
    virtual void OnHit(const RVec3* a, const RVec3* b, int c, void* d);  // 0x48
    virtual void OnImpulse(const RVec3* v, const RVec3* at, int kind, void* c, void* by);  // 0x4C
    virtual void SetState(const BodyState* s);  // 0x54: places the car at a pose
    virtual void vf58(void);          // 0x58
    virtual void vf5C(void);          // 0x5C

    s32 index;                   // offset 0x1EC
    s32 f1F0;                    // offset 0x1F0
    u8 pad1F4[0x25C - 0x1F4];
    s32 f25C;                    // offset 0x25C
    u8 f260;                     // offset 0x260
    u8 pad261[0x268 - 0x261];
    u8* f268;                    // offset 0x268
    CarLayout* layout;           // offset 0x26C (carlayout.cpp; cardmg walks its deformable points)
    StatsSrc* src;               // offset 0x270
    u8 pad274[0x2A4 - 0x274];
    f32 f2A4;                    // offset 0x2A4
    f32 f2A8;                    // offset 0x2A8
    f32 f2AC;                    // offset 0x2AC
    u8 pad2B0[0x2C8 - 0x2B0];
    f32 f2C8;                    // offset 0x2C8
    f32 f2CC;                    // offset 0x2CC
    f32 f2D0;                    // offset 0x2D0
    f32 f2D4;                    // offset 0x2D4
    f32 f2D8;                    // offset 0x2D8
    u8 pad2DC[0x2EC - 0x2DC];
    f32 f2EC;                    // offset 0x2EC
    u8 pad2F0[0x2F4 - 0x2F0];
    f32 f2F4;                    // offset 0x2F4
    f32 f2F8;                    // offset 0x2F8
    f32 f2FC;                    // offset 0x2FC
    u8 pad300[0x314 - 0x300];
    f32 f314;                    // offset 0x314
    f32 f318;                    // offset 0x318
    u8 pad31C[0x328 - 0x31C];
    u8* f328;                    // offset 0x328
    CbSetup setupA;              // offset 0x32C
    CbSetup setupB;              // offset 0x464
    s32 model;                   // offset 0x59C
    s32 f5A0;                    // offset 0x5A0
    s32 f5A4;                    // offset 0x5A4
    f32 f5A8;                    // offset 0x5A8
    f32 f5AC;                    // offset 0x5AC
    f32 f5B0;                    // offset 0x5B0
    f32 f5B4;                    // offset 0x5B4
    f32 f5B8;                    // offset 0x5B8
    f32 f5BC;                    // offset 0x5BC
    f32 f5C0;                    // offset 0x5C0
    f32 f5C4;                    // offset 0x5C4
    f32 f5C8;                    // offset 0x5C8
    f32 f5CC;                    // offset 0x5CC
    f32 f5D0;                    // offset 0x5D0
    f32 f5D4;                    // offset 0x5D4
    f32 f5D8;                    // offset 0x5D8
    f32 f5DC;                    // offset 0x5DC
    f32 f5E0;                    // offset 0x5E0
    f32 f5E4;                    // offset 0x5E4
    s32 f5E8;                    // offset 0x5E8
    f32 f5EC;                    // offset 0x5EC
    f32 f5F0;                    // offset 0x5F0
    f32 f5F4;                    // offset 0x5F4
    s32 f5F8;                    // offset 0x5F8
    u8 pad5FC[0x152C - 0x5FC];
    CbJoint joints[18];          // offset 0x152C
    u8 pad1E2C[0x1E3C - 0x1E2C];
    s32 f1E3C;                   // offset 0x1E3C
    s32 f1E40;                   // offset 0x1E40
    u8 pad1E44[0x20DC - 0x1E44];
    s32 f20DC;                   // offset 0x20DC
    u8 pad20E0[0x23AC - 0x20E0];
    CbWheel wheels[4];           // offset 0x23AC
    u8* f257C;                   // offset 0x257C
    u8* f2580;                   // offset 0x2580
    u8* f2584;                   // offset 0x2584
    f32 f2588;                   // offset 0x2588
    f32 f258C;                   // offset 0x258C
    f32 f2590;                   // offset 0x2590
    f32 f2594;                   // offset 0x2594
    f32 f2598;                   // offset 0x2598
    u8 f259C;                    // offset 0x259C
    u8 f259D;                    // offset 0x259D
    u8 pad259E[0x25A0 - 0x259E];
    f32 f25A0;                   // offset 0x25A0
    f32 f25A4;                   // offset 0x25A4
    f32 f25A8;                   // offset 0x25A8
    f32 f25AC;                   // offset 0x25AC
    u8 pad25B0[0x25BC - 0x25B0];
    f32 f25BC;                   // offset 0x25BC
    u8 f25C0;                    // offset 0x25C0
    u8 pad25C1[0x25C4 - 0x25C1];
    f32 f25C4;                   // offset 0x25C4
    u8 f25C8;                    // offset 0x25C8
    u8 pad25C9[0x25CC - 0x25C9];
    f32 f25CC;                   // offset 0x25CC
    f32 f25D0;                   // offset 0x25D0
    f32 speed;                   // offset 0x25D4
    f32 f25D8;                   // offset 0x25D8
    f32 f25DC;                   // offset 0x25DC
    f32 f25E0;                   // offset 0x25E0
    f32 f25E4;                   // offset 0x25E4
    f32 f25E8;                   // offset 0x25E8
    f32 f25EC;                   // offset 0x25EC
    f32 f25F0;                   // offset 0x25F0
    f32 f25F4;                   // offset 0x25F4
    f32 f25F8;                   // offset 0x25F8
    f32 f25FC;                   // offset 0x25FC
    f32 f2600;                   // offset 0x2600
    f32 f2604;                   // offset 0x2604
    f32 f2608;                   // offset 0x2608
    u8 f260C;                    // offset 0x260C
    u8 f260D;                    // offset 0x260D
    u8 f260E;                    // offset 0x260E
    u8 pad260F[0x2610 - 0x260F];
    s32 link;                    // offset 0x2610
    f32 f2614;                   // offset 0x2614
    f32 f2618;                   // offset 0x2618
    f32 f261C;                   // offset 0x261C
    s32 airborne;                // offset 0x2620
    u8 f2624;                    // offset 0x2624
    u8 f2625;                    // offset 0x2625
    u8 pad2626[0x2628 - 0x2626];
    f32 f2628;                   // offset 0x2628
    f32 f262C;                   // offset 0x262C
    f32 f2630;                   // offset 0x2630
    f32 f2634;                   // offset 0x2634
    s32 f2638;                   // offset 0x2638
    s32 f263C;                   // offset 0x263C
    u8 f2640;                    // offset 0x2640
    u8 pad2641[0x2644 - 0x2641];
    s32 f2644;                   // offset 0x2644
    s32 f2648;                   // offset 0x2648
    f32 f264C;                   // offset 0x264C
    f32 f2650;                   // offset 0x2650
    f32 f2654;                   // offset 0x2654
    f32 f2658;                   // offset 0x2658
    f32 f265C;                   // offset 0x265C
    f32 f2660;                   // offset 0x2660
    f32 f2664;                   // offset 0x2664
    f32 f2668;                   // offset 0x2668
    f32 f266C;                   // offset 0x266C
    f32 f2670;                   // offset 0x2670
    f32 f2674;                   // offset 0x2674
    s32 f2678;                   // offset 0x2678
    s32 f267C;                   // offset 0x267C
    u8 f2680;                    // offset 0x2680
    u8 pad2681[0x2684 - 0x2681];
    f32 f2684;                   // offset 0x2684
    f32 f2688;                   // offset 0x2688
    s32 a268C;                   // offset 0x268C
    f32 f2690;                   // offset 0x2690
    f32 f2694;                   // offset 0x2694
    f32 f2698;                   // offset 0x2698
    f32 f269C;                   // offset 0x269C
    f32 f26A0;                   // offset 0x26A0
    s32 f26A4;                   // offset 0x26A4
    f32 f26A8;                   // offset 0x26A8
    u8 f26AC;                    // offset 0x26AC
    u8 pad26AD[0x26B0 - 0x26AD];
    s32 f26B0;                   // offset 0x26B0
    RVec3 push;                  // offset 0x26B4
    u8* f26C0;                   // offset 0x26C0
    u8* f26C4;                   // offset 0x26C4
    u8* f26C8;                   // offset 0x26C8
    f32 f26CC;                   // offset 0x26CC
    f32 f26D0;                   // offset 0x26D0
    s32 f26D4;                   // offset 0x26D4
    f32 f26D8;                   // offset 0x26D8
    f32 f26DC;                   // offset 0x26DC
    u8 f26E0;                    // offset 0x26E0
    u8 pad26E1[0x26E4 - 0x26E1];
    f32 f26E4;                   // offset 0x26E4
    f32 f26E8;                   // offset 0x26E8
    f32 f26EC;                   // offset 0x26EC
    f32 f26F0;                   // offset 0x26F0
    f32 f26F4;                   // offset 0x26F4
    f32 f26F8;                   // offset 0x26F8
    f32 f26FC;                   // offset 0x26FC
};

// The car body as the race sees it (fn_8017960C(id) returns it): the same object, with the accessors other units
// call on it. A shared view so every unit uses one declaration; the mangled names keep RaceCarRec (no symbol churn).
// The accessors are inline, defined in the unit retail keeps their weak copy in (Mtx: aiobst, F2690: betarace,
// Rings: ringchal; Speed/Air: stunts). Names unproven.
struct RaceCarRec : CarBody {
    RMtx* Mtx(void);           // the car's frame (&mtx)
    f32 F2690(void);           // f2690 (non-const in retail)
    f32 Speed(void) const;     // speed
    s32 Air(void) const;       // airborne (0 while airborne)
    void* Rings(void);         // the ring record at 0x1EC (ringchal)
};

#endif
