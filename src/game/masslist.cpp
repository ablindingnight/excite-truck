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
    MVec4(f32 x_, f32 y_, f32 z_) {  // w left unset
        x = x_;
        y = y_;
        z = z_;
    }
    MVec4(f32 x_, f32 y_, f32 z_, f32 w_) {
        x = x_;
        y = y_;
        z = z_;
        w = w_;
    }
    f32 x;
    f32 y;
    f32 z;
    f32 w;
};

struct MassSet {
    MassSet(void);
    MVec4 pos;      // offset 0x0
    MVec4 rot[3];   // offset 0x10 (rows)
    MVec4 quat;     // offset 0x40
    MVec4 vel;      // offset 0x50
    MVec4 v60;      // offset 0x60
    u8 pad70[0x30];
    MVec4 vA0;      // offset 0xA0
};

struct ContactPt {
    MVec3 p;
    f32 w;
};

struct ContactSet {
    ContactSet(void);
    u8 pad0[0x30];
    MVec4 v30;            // offset 0x30
    ContactPt pts[0x15];  // offset 0x40
    MVec4 v190;           // offset 0x190
    MVec4 v1A0;           // offset 0x1A0
    u8 pad1B0[0x40];
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

struct ContactOut {
    ContactOut(void) {}
    u8 data[0x158];
};

extern "C" {
void* memset(void* p, int c, size_t n);
double cos(double x);
double sin(double x);
void fn_801E9DC4(CxEntry* e);
void fn_801BDA70(void);
void fn_801BDA74(int car, ContactOut* out);
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

// Adds a car: clears its records and starts it at rest, upright, 2 units up.
void fn_801BD808(void) {
    ContactOut out;
    int car = lbl_8055E78C++;
    MassSet* m = &lbl_8055E794[car];
    ContactSet* c = &lbl_8055E798[car];
    CrossSet* x = &lbl_8055E79C[car];
    CxEntry* e = &lbl_8055E7A4[car];
    memset(m, 0, sizeof(MassSet));
    memset(c, 0, sizeof(ContactSet));
    memset(x, 0, sizeof(CrossSet));
    memset(e, 0, sizeof(CxEntry));
    fn_801E9DC4(e);
    m->pos = MVec4(0.0f, 2.0f, 0.0f);
    m->vel = MVec4(0.0f, 0.0f, 0.0f);
    m->v60 = MVec4(0.0f, 0.0f, 0.0001f);
    m->vA0 = MVec4(0.0f, 0.0f, 0.0f);
    f32 angle = 0.0f;
    m->quat = MVec4(0.0f, 0.0f, sin(0.5f * angle), cos(0.5f * angle));
    m->rot[0] = MVec4(1.0f, 0.0f, 0.0f);
    m->rot[1] = MVec4(0.0f, 1.0f, 0.0f);
    m->rot[2] = MVec4(0.0f, 0.0f, 1.0f);
    c->v30 = MVec4(0.0f, 0.0f, 0.0f, 0.0f);
    c->v190 = MVec4(0.0f, 0.0f, 0.0f, 0.0f);
    c->v1A0 = MVec4(0.0f, 0.0f, 0.0f, 0.0f);
    fn_801BDA70();
    fn_801BDA74(car, &out);
}

void fn_801BDA70(void) { fn_80279D40(); }

// Contact gathering (placeholders).
void fn_801BDA74(int car, ContactOut* out) {}

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
