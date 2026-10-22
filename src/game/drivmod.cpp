// Driver model (0x801FFB08-0x80202218): the rider skeleton helpers ("butt", "wheel", "Lhand",
// "Rhand", "hand", "neck", "drivmod: can't find helper %s in %s"), pose blending and teardown. Written
// against raw field offsets; the large routines are still stubs. Names are guesses apart from the
// strings.
#include "game/common.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

extern "C" {
int fn_801E2CCC(u8* skel, const char* name, u8** bone);
void fn_802E30C8(const char* fmt, ...);
void fn_801E323C(u8* p);
void fn_801C1C7C(u8* p);
void fn_801E598C(u8* p);
void fn_802C6CF8(u8* p);
}

void operator delete(void* p);

// A position that starts at the origin.
struct ZeroPos {
    f32 x, y, z;
    ZeroPos(void) {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
    }
};

extern const char* lbl_8055ECA8;

ModuleTag lbl_8055ECA0;
ZeroPos lbl_804888A8;

extern "C" {

f32* fn_801FFB08(u8* skel, const char* name) {
    u8* bone;
    if (!fn_801E2CCC(skel, name, &bone)) {
        fn_802E30C8("drivmod: can't find helper %s in %s\n", name, lbl_8055ECA8);
        return &lbl_804888A8.x;
    }
    return (f32*)(bone + 0x24);
}

void fn_801FFB6C(u8* p) {}

void fn_801FFB70(u8* p) {}

void fn_801FFB74(u8* p) {}  // stub (472)

void fn_802002D4(u8* p) {}  // stub (418)

void fn_8020095C(u8* p) {
    if (p) {
        fn_801E323C(PTR(p, 0x1C));
        fn_801E323C(PTR(p, 0x20));
        fn_801E323C(PTR(p, 0x24));
        fn_801E323C(PTR(p, 0x28));
        fn_801C1C7C(PTR(p, 0x0));
        fn_801E598C(PTR(p, 0xC));
        fn_802C6CF8(PTR(p, 0x8));
        operator delete(p);
    }
}

void fn_802009CC(u8* p) {}  // stub (403)

void fn_80201018(u8* p) {}  // stub (1137)

}  // extern "C"
