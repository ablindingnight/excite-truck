#ifndef GAME_COLLSHAPE_H
#define GAME_COLLSHAPE_H

#include "game/common.h"

// The collision shape base class (vtable 0x80333B98, emitted by sphcoll.cpp). Every unit that builds or uses
// shapes includes this one declaration (port ODR work, 2026-11-16).
//
// Why this form (retail evidence):
// - The interface base is novtable outside sphcoll (see below).
// - The empty slots (vf0C, vf14, vf28, vf48, vf4C), SetSurface and the dtor are inline: retail's only copies sit
//   in the first linked unit whose vtable needs them (collvol, rigid, carbody, phob, betarace), as weak copies do.
// - vf18 is the first non-inline virtual and sphcoll defines it, so sphcoll emits the vtable, as retail does.

struct RVec3;
struct RMtx;  // game/rmtx.h (the shape's frame; was CFrame)
class CollVolume;

// The all-pure interface the shape base derives from (its table is the 14 zero words after CollShape's, 0x80333BE8,
// in sphcoll's .data). Every polymorphic base makes each unit that constructs a shape emit the base's table as weak
// DATA, which the linker keeps (harness 2026-11-16); retail mutphob, obstacles, ball, wheel and collvol have no such
// bytes, so for them the interface is novtable (no vptr stores, no table). sphcoll, the one unit that emits the
// table, defines COLLSHAPE_IFACE_DECL empty before including this header (MATCHING-AID: retail's sphcoll saw the
// interface without novtable). On a PC build both spellings mean the same class. Class and method names unproven.
#ifndef COLLSHAPE_IFACE_DECL
#define COLLSHAPE_IFACE_DECL __declspec(novtable)
#endif

// The root of the shapes: only a virtual destructor. Retail's only table for it is a weak 0xC one in betarace
// (__vt__13CollShapeBase, with the weak __dt__13CollShapeBaseFv), the one unit that builds a shape without the
// parameter constructor; so it is novtable everywhere else, and betarace defines COLLSHAPE_BASE_DECL empty
// (MATCHING-AID, as COLLSHAPE_IFACE_DECL in sphcoll). Class name unproven.
#ifndef COLLSHAPE_BASE_DECL
#define COLLSHAPE_BASE_DECL __declspec(novtable)
#endif
class COLLSHAPE_BASE_DECL CollShapeBase {
public:
    virtual ~CollShapeBase(void) {}
};

class COLLSHAPE_IFACE_DECL CollShapeIface : public CollShapeBase {
public:
    virtual ~CollShapeIface(void) = 0;
    virtual void vf0C(void) = 0;
    virtual void vf10(u8* other) = 0;
    virtual void vf14(void) = 0;
    virtual int vf18(const RVec3* from, const RVec3* dir, f32* t, RVec3* n) = 0;
    virtual void vf1C(void) = 0;
    virtual void vf20(void* a, const RVec3* b, const RVec3* c, int d, const RVec3* e, void* f, f32 g) = 0;
    virtual void vf24(const RVec3* v, const RVec3* at, int kind, void* c, void* by) = 0;
    virtual void vf28(void) = 0;
    virtual void vf2C(u8* data) = 0;
    virtual u32 vf30(void) = 0;
    virtual void vf34(RMtx* frame) = 0;
};
inline CollShapeIface::~CollShapeIface(void) {}

class CollShape : public CollShapeIface {
public:
    CollShape(void) {}  // fields set by the builder (betarace's spin box)
    CollShape(u32 data, u32 swap);  // from file data, byte-swapped when `swap`
    // Parameter form, inlined into every builder; store order as mutphob's copy (the only order that keeps
    // SphereGroup and PolyShape identical).
    CollShape(const RMtx* frame_, CollVolume* owner_, s32 type_, f32 k, f32 s) {
        frame = (RMtx*)frame_;
        owner = owner_;
        data = 0;
        type = type_;
        stiff = k;
        skin = s;
        surface = 100;
        x1C = 0;
    }
    virtual ~CollShape(void) {}
    virtual void vf0C(void) {}
    virtual void vf10(u8* other) = 0;
    virtual void vf14(void) {}
    virtual int vf18(const RVec3* from, const RVec3* dir, f32* t, RVec3* n);
    virtual void vf1C(void);
    virtual void vf20(void* a, const RVec3* b, const RVec3* c, int d, const RVec3* e, void* f, f32 g);
    virtual void vf24(const RVec3* v, const RVec3* at, int kind, void* c, void* by);  // impulse notice, forwarded to the owner (sphcoll)
    virtual void vf28(void) {}
    virtual void vf2C(u8* data);
    virtual u32 vf30(void);
    virtual void vf34(RMtx* frame);
    virtual void vf38(RVec3* lo, RVec3* hi) const = 0;
    virtual f32 Radius(void) = 0;
    virtual void SetSurface(int m) { surface = m; }
    virtual void vf44(u32 v);
    virtual void vf48(void) {}
    virtual void vf4C(void) {}
    CollVolume* owner;  // 0x04
    u8* data;           // 0x08
    RMtx* frame;        // 0x0C
    f32 stiff;          // 0x10
    f32 skin;           // 0x14
    s32 type;           // 0x18
    u32 x1C;            // 0x1C
    s32 surface;        // 0x20
};

#endif
