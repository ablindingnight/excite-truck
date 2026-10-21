// Car mass lists (0x801BD628-0x801BF214): per-car point-mass tables ("masslist") and the contact lists
// built from them each frame ("cslist", "cclist", "cxlist", "alist"), plus a 256-entry scratch table of
// 4-float records. Only the allocation, accessors and small vector helpers are reconstructed; the mass
// update, contact gathering and post-update passes are placeholders. Names are guesses apart from the
// strings.
#include "game/common.h"

struct MVec3 {
    f32 x;
    f32 y;
    f32 z;
};

struct MVec4 {
    MVec4(void) {}
    f32 x;
    f32 y;
    f32 z;
    f32 w;
};

struct MassSet {
    MassSet(void);
    u8 data[0xB0];
};

struct ContactPt {
    MVec3 p;
    f32 w;
};

struct ContactSet {
    ContactSet(void);
    u8 pad0[0x40];
    ContactPt pts[0x1B];  // offset 0x40
};

struct CrossSet {
    CrossSet(void);
    u8 data[0x190];
};

struct CxEntry {
    u8 data[0x14];
};

ModuleTag lbl_8055E780;
UnkStaticFFFF lbl_8055E788;
s32 lbl_8055E78C;
s32 lbl_8055E790;           // cars
MassSet* lbl_8055E794;      // per car
ContactSet* lbl_8055E798;   // per car
CrossSet* lbl_8055E79C;     // per car
u8* lbl_8055E7A0;           // per-car flags
CxEntry* lbl_8055E7A4;      // per car
u8 lbl_8055E7A8;
MVec4 lbl_80436D48[0x100];

extern "C" {
int fn_801E9E78(s16* h, MVec3* p, MVec3* n);
void fn_80279D40(void);
}

void* operator new[](size_t size, const char* name);
void operator delete[](void* p);

static inline void SetV(MVec3* o, f32 x, f32 y, f32 z) {
    o->x = x;
    o->y = y;
    o->z = z;
}

static inline void SetVyxz(MVec3* o, f32 x, f32 y, f32 z) {
    o->y = y;
    o->x = x;
    o->z = z;
}

extern "C" {

int fn_801BD628(const MVec3* v) {
    int r = 0;
    if (v->x >= -100000.0f && v->x <= 100000.0f && v->y >= -100000.0f && v->y <= 100000.0f &&
        v->z >= -100000.0f && v->z <= 100000.0f) {
        r = 1;
    }
    return r;
}

void fn_801BD694(int n) {
    lbl_8055E790 = n;
    lbl_8055E78C = 0;
    lbl_8055E794 = new ("masslist") MassSet[n];
    lbl_8055E798 = new ("cslist") ContactSet[lbl_8055E790];
    lbl_8055E79C = new ("cclist") CrossSet[lbl_8055E790];
    lbl_8055E7A4 = new ("cxlist") CxEntry[lbl_8055E790];
    lbl_8055E7A0 = new ("alist") u8[lbl_8055E790];
    lbl_8055E7A8 = 1;
}

}  // extern "C"

CrossSet::CrossSet(void) {}

ContactSet::ContactSet(void) {}

MassSet::MassSet(void) {}

extern "C" {

void fn_801BD790(void) {
    delete[] lbl_8055E794;
    delete[] lbl_8055E798;
    delete[] lbl_8055E79C;
    delete[] lbl_8055E7A4;
    delete[] lbl_8055E7A0;
    lbl_8055E794 = 0;
    lbl_8055E78C = 0;
    lbl_8055E790 = 0;
}

// Mass update (placeholder).
void fn_801BD808(void) {}

void fn_801BDA70(void) { fn_80279D40(); }

// Contact gathering (placeholders).
void fn_801BDA74(void) {}

void fn_801BE168(void) {}

void fn_801BE400(void) {}

void fn_801BEAEC(int car, int i, MVec3* out) {
    const MVec3* p = &lbl_8055E798[car].pts[i].p;
    SetV(out, p->x, p->y, p->z);
}

void fn_801BEB1C(int car, ContactSet** out) { *out = &lbl_8055E798[car]; }

void fn_801BEB30(int car, CrossSet** out) { *out = &lbl_8055E79C[car]; }

// Post-update passes (placeholders).
void fn_801BEB44(void) {}

void fn_801BEDA8(MassSet** m, ContactSet** cs, CrossSet** cc, CxEntry** cx, s32* n) {
    *m = lbl_8055E794;
    *cs = lbl_8055E798;
    *cc = lbl_8055E79C;
    *cx = lbl_8055E7A4;
    *n = lbl_8055E78C;
}

void fn_801BEDD4(void) {}

void fn_801BF0C4(MVec3* p, MVec3* n, s16* h) {
    if (!fn_801E9E78(h, p, n)) {
        n->x = 0.0f;
        n->y = 1.0f;
        n->z = 0.0f;
        p->y = -100.0f;
    }
}

void fn_801BF12C(MVec4* out, const MVec4* in) {
    out->x = in->x;
    out->y = in->y;
    out->z = in->z;
    out->w = in->w;
}

void fn_801BF150(MVec4* out, f32 x, f32 y, f32 z, f32 w) {
    out->x = x;
    out->y = y;
    out->z = z;
    out->w = w;
}

void fn_801BF164(MVec4* out, const MVec4* in) {
    out->x = in->x;
    out->y = in->y;
    out->z = in->z;
    out->w = in->w;
}

void fn_801BF188(MVec3* out, f32 x, f32 y, f32 z) {
    out->x = x;
    out->y = y;
    out->z = z;
}

void fn_801BF198(MVec3* out, const MVec3* in) { SetVyxz(out, in->x, in->y, in->z); }

void fn_801BF1B4(void) {}

void fn_801BF1B8(int car, u8 on) { lbl_8055E7A0[car] = on; }

}  // extern "C"
