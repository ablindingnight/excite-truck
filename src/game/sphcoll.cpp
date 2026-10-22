// Collision volumes (0x80198F64-0x801A520C): spheres, flat spheres, sphere groups ("SphereGroupPart")
// and their parts, the mesh-backed track collision ("track.msh"), bounds and debug drawing, and
// the per-type collide dispatch ("Unsupported collision type: Sphere-%x"). Written against raw
// field offsets; the large collide routines are still stubs. Names are guesses apart from the
// strings.
#include "game/common.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

struct RVec3 {
    f32 x, y, z;
};

struct W3 {
    u32 a, b, c;
};

struct Phob;

class RigidBody {
public:
    void Push(Phob* who, void* a, const RVec3* b, const RVec3* c, int d, const RVec3* e, void* f, f32 g);
};

class VGroup {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(u8* a);
};

extern "C" void fn_801A4A10(u8* p, const RVec3* c, void* f, const RVec3* b);

// A rotation plus a translation.
struct Mat3x3 {
    f32 m[9];
};

struct Xform {
    Mat3x3 rot;
    RVec3 pos;
};

extern void* lbl_80333968[];

static inline void Swap32(u8* b) {
    u8 b0 = b[0];
    u8 b1 = b[1];
    u8 b2 = b[2];
    u8 b3 = b[3];
    b[0] = b3;
    b[3] = b0;
    b[1] = b2;
    b[2] = b1;
}

class VEx {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(const char* a, const char* b);
    virtual void v1C(void);
    virtual void v20(void);
    virtual void v24(void);
    virtual void v28(void);
    virtual void v2C(void);
    virtual void v30(void);
    virtual void v34(u8* q);
    virtual void v38(void);
    virtual void v3C(void);
    virtual void v40(void);
    virtual void v44(void);
    virtual void v48(void);
    virtual void v4C(void);
    virtual u8* v50(void);
};

#define VE(p) ((VEx*)(p))

class SpringCar {
public:
    void HitWall(void* w);
};

extern "C" {
f32 PSVECDotProduct(const void* a, const void* b);
double sqrt(double x);
void fn_801E3FC8(u8* c, u32 color, f32 r);
void fn_801E44F4(u8* m, u32 color, f32 a, f32 b, f32 c);
void fn_801CFA48(u8* p);
void fn_801CFF14(u8* p);
void fn_801CFF18(u8* p, u32 a);
u8* fn_801CFEAC(const char* name);
int fn_801CF828(u8* m, u32 a, u32 b, u32 c, u32 d);
void fn_8012C4E4(void* a, void* fn, int b);
void fn_801796EC(int kind, void* msg, int size);
void fn_801D0650(u8* p);
void fn_801D06C4(u8* p);
void fn_8019A820(u8* p, u8* c, u32 a, u32 b, u32 d, f32 r);
void fn_8019AA34(u8* p, u8* c, u8* q, u8* d, u32 a, u32 b, f32 r, f32 s);
void fn_8019CCCC(u8* p, u8* c, u32 a, f32 r);
void fn_8019AC44(u8* p, u8* c, u8* d, f32 r);
void fn_8019DDFC(u32 a, u32 b, u32 c, u32 d, u8* e, f32 r);
void fn_8019ECC4(u8* p, u32 a, u32 b);
u8* fn_801A1520(u8* p, u32 a, u32 swap);
void fn_8019F1A4(u8* p);
void fn_8019EBC0(u8* p);
void fn_8019F03C(u8* p);
void fn_801A49FC(u8* p);
void fn_801A49E8(u8* p);
void fn_801A44D4(u8* l);
void fn_801A4868(u8* p, u32 v);
}

void* operator new(size_t size, const char* tag);
void operator delete(void* p);

extern void* lbl_803339B8[];
extern void* lbl_80333918[];
extern void* lbl_80333728[];

ModuleTag lbl_8055E590;
UnkStaticFFFF lbl_8055E598;
UnkStaticFFFF lbl_8055E59C;
u8* lbl_8055E5A0;  // track collision mesh

extern "C" {

void fn_80198F64(u8* p) {}  // stub (470)

void fn_801996BC(u8* p) {}  // stub (109)

void fn_80199870(u8* p, void* w) {
    u32 t = U32(p, 0x10);
    if (t == 'PCAR' || t == 'ACAR') {
        ((SpringCar*)p)->HitWall(w);
    }
}

void fn_80199894(u8* p) {}  // stub (955)

u32 fn_8019A780(u8* p) { return U32(p, 0x8); }

void fn_8019A788(u8* p) {}  // stub (38)

void fn_8019A820(u8* p, u8* c, u32 a, u32 b, u32 d, f32 r) {}

void fn_8019AA18(u8* p, u32 a, u32 b, u32 c) { fn_8019A820(p, p + 0x34, a, b, c, F32(p, 0x24)); }

void fn_8019AA34(u8* p, u8* c, u8* q, u8* d, u32 a, u32 b, f32 r, f32 s) {}

void fn_8019AC44(u8* p, u8* c, u8* d, f32 r) {}

void fn_8019B068(u8* p, u8* q, u32 a, u32 b) { fn_8019AA34(p, p + 0x34, q, q + 0x34, a, b, F32(p, 0x24), F32(q, 0x24)); }

void fn_8019B08C(u8* p) {}  // stub (275)

void fn_8019B4D8(u8* p) {}  // stub (541)

u32 fn_8019BD4C(u8* p) { return U32(p, 0xc); }

void fn_8019BD54(u8* p, u8* q, int a, int b) {}  // stub (704)

void fn_8019C854(u8* p) {}  // stub (286)

void fn_8019CCCC(u8* p, u8* c, u32 a, f32 r) {}

void fn_8019CE3C(u8* p, u32 a) { fn_8019CCCC(p, p + 0x34, a, F32(p, 0x24)); }

void fn_8019CE4C(u8* p) {}  // stub (69)

void fn_8019CF60(u8* p) {}  // stub (67)

void fn_8019D06C(u8* p, u8* q, int a, int b) {}  // stub (76)

void fn_8019D19C(u8* p) {}  // stub (119)

void fn_8019D378(u8* p) {}  // stub (67)

void fn_8019D484(u8* p) {}  // stub (101)

void fn_8019D618(u8* p) {}  // stub (294)

void fn_8019DAB0(u8* p) {}  // stub (85)

void fn_8019DC04(u8* p) {}  // stub (122)

f32 fn_8019DDEC(u8* p) { return F32(p, 0x24) + F32(p, 0x28); }

void fn_8019DDFC(u32 a, u32 b, u32 c, u32 d, u8* e, f32 r) {}

void fn_8019DF88(u8* p) {}  // stub (155)

void fn_8019E1F4(u8* p) {}  // stub (55)

void fn_8019E2D0(u8* p) {}  // stub (140)

void fn_8019E500(u8* p) {}  // stub (205)

u8* fn_8019E834(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_8019E874(u8* p) {}  // stub (130)

void fn_8019EA7C(u8* p, u8* q) {}  // stub (38)

void fn_8019EB14(u8* tree, int i) {
    u8* n = tree + i * 8;
    u8 t;
    t = n[0];
    n[0] = n[1];
    n[1] = t;
    t = n[2];
    n[2] = n[3];
    n[3] = t;
    t = n[4];
    n[4] = n[5];
    n[5] = t;
    t = n[6];
    n[6] = n[7];
    n[7] = t;
    if (*(u16*)(n + 4)) {
        for (int k = 0; k < 4; k++) {
            fn_8019EB14(tree, *(u16*)(n + 4) + k);
        }
    }
}

void fn_8019EBC0(u8* p) {
    u8* o = PTR(p, 0x4);
    if (o) {
        VE(o)->v48();
    }
}

void fn_8019EBE0(u8* p, void* a, const RVec3* b, const RVec3* c, int d, const RVec3* e, void* f, f32 g) {
    if (PTR(p, 0x4)) {
        ((RigidBody*)PTR(p, 0x4))->Push((Phob*)p, a, b, c, d, e, f, g);
    } else if (PTR(p, 0x8)) {
        fn_801A4A10(PTR(p, 0x8), c, f, b);
    }
}

void fn_8019EC84(u8* p) {
    u8* o = PTR(p, 0x4);
    if (o) {
        VE(o)->v4C();
    }
}

void fn_8019ECA4(u8* p, u8* q) {
    PTR(p, 0x8) = q;
    VE(p)->v34(q + 4);
}

void fn_8019ECBC(u8* p, u32 v) { U32(p, 0xc) = v; }

void fn_8019ECC4(u8* p, u32 a, u32 b) {}

void fn_8019EDB8(u8* p) {}  // stub (69)

void fn_8019EECC(u8* p, RVec3* v) {
    v->x += F32(p, 0x30);
    v->y += F32(p, 0x34);
    v->z += F32(p, 0x38);
}

void fn_8019EF00(u8* p) {}  // stub (79)

void fn_8019F03C(u8* p) {
    U8(p, 0x4C) = 0;
    F32(p, 0x40) = 0.0f;
    F32(p, 0x44) = 0.0f;
    F32(p, 0x48) = 0.0f;
    F32(p, 0x34) = 0.0f;
    F32(p, 0x38) = 0.0f;
    F32(p, 0x3C) = 0.0f;
}

void fn_8019F064(u8* p) {}  // stub (80)

void fn_8019F1A4(u8* p) { fn_8019AC44(p, p + 0x34, p + 0x40, F32(p, 0x24)); }

void fn_8019F1B4(u8* p, u32 a, u32 b, u32 c, u32 d) { fn_8019DDFC(a, b, c, d, p + 0x34, F32(p, 0x24)); }

void fn_8019F1D4(u8* p, RVec3* lo, RVec3* hi) {
    f32 r = F32(p, 0x24);
    RVec3 e;
    e.x = r;
    e.y = r;
    e.z = r;
    RVec3 c;
    c.x = F32(p, 0x34);
    c.y = F32(p, 0x38);
    c.z = F32(p, 0x3C);
    lo->x = c.x + r * -1.0f;
    lo->y = c.y + r * -1.0f;
    lo->z = c.z + r * -1.0f;
    hi->x = c.x + r * 1.0f;
    hi->y = c.y + r * 1.0f;
    hi->z = c.z + r * 1.0f;
}

void fn_8019F248(u8* p) {
    f32 r = F32(p, 0x24);
    r = r > 0.2f ? r : 0.2f;
    fn_801E3FC8(p + 0x34, lbl_8055E598.value, r);
}

void fn_8019F26C(u8* p) {}  // stub (89)

void fn_8019F3D0(u8* p) {}

u8* fn_8019F3D4(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_8019F414(u8* p) {}  // stub (48)

void fn_8019F4D4(u8* p) {}  // stub (35)

void fn_8019F560(u8* p) { fn_8019F1A4(p); }

void fn_8019F564(u8* p) {}  // stub (99)

void fn_8019F6F0(u8* p) {}  // stub (36)

void fn_8019F780(u8* p) {}  // stub (514)

void fn_8019FF88(u8* p) { fn_8019EBC0(p); }

void fn_8019FF8C(u8* p) {
    fn_8019F03C(p);
    F32(p, 0x28) = F32(p, 0x54);
    F32(p, 0x2C) = F32(p, 0x58);
    F32(p, 0x30) = F32(p, 0x5C);
    U8(p, 0x78) = 0;
}

u8* fn_8019FFD8(u8* p, u32 a, u32 b) {
    U32(p, 0xC) = a;
    U32(p, 0x4) = b;
    U32(p, 0x8) = 0;
    U32(p, 0x18) = 'GRUP';
    F32(p, 0x10) = 0.0f;
    F32(p, 0x14) = 0.0f;
    U32(p, 0x20) = 100;
    U32(p, 0x1C) = 0;
    *(void**)p = lbl_803339B8;
    F32(p, 0x170) = 0.0f;
    F32(p, 0x178) = 0.0f;
    F32(p, 0x16C) = 0.0f;
    F32(p, 0x168) = 0.0f;
    F32(p, 0x164) = 0.0f;
    U32(p, 0x174) = 0;
    U32(p, 0x17C) = 0;
    S32(p, 0x180) = -1;
    return p;
}

void fn_801A0040(u8* p) {}  // stub (51)

u8* fn_801A010C(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_801A014C(u8* p) { VE(PTR(p, 0x17C))->v0C(); }

void fn_801A0160(u8* p) {}  // stub (32)

void fn_801A01E0(u8* p, RVec3* lo, RVec3* hi) {
    f32 r = F32(p, 0x178);
    u8* o = PTR(p, 0xC);
    RVec3 e;
    e.x = r;
    e.y = r;
    e.z = r;
    RVec3 c;
    c.x = F32(o, 0x24);
    c.y = F32(o, 0x28);
    c.z = F32(o, 0x2C);
    lo->x = c.x + r * -1.0f;
    lo->y = c.y + r * -1.0f;
    lo->z = c.z + r * -1.0f;
    hi->x = c.x + r * 1.0f;
    hi->y = c.y + r * 1.0f;
    hi->z = c.z + r * 1.0f;
}

void fn_801A0258(u8* p) {}  // stub (72)

void fn_801A0378(u8* p) {}  // stub (51)

void fn_801A0444(u8* p, u8* q) {
    PTR(p, 0x17C) = q;
    F32(p, 0x164) = F32(q, 0x24);
    F32(p, 0x168) = F32(q, 0x28);
    F32(p, 0x16C) = F32(q, 0x2C);
}

void fn_801A0464(u8* p) {}  // stub (683)

void fn_801A0F10(u8* p) {}  // stub (142)

void fn_801A1148(u8* p) {}  // stub (66)

void fn_801A1250(u8* p) {
    u8* q = p;
    for (int i = 0; i < S32(p, 0x174); i++) {
        VE(PTR(q, 0x24))->v28();
        q += 4;
    }
    S32(p, 0x180) = -1;
}

void fn_801A12C4(u8* p) {}  // stub (37)

void fn_801A1358(u8* p, u32 v) { U32(p, 0x1c) = v; }

void fn_801A1360(u8* p) {}  // stub (37)

void fn_801A13F4(u8* p) {}  // stub (32)

void fn_801A1474(u8* p) {}  // stub (43)

u8* fn_801A1520(u8* p, u32 a, u32 swap) {
    fn_8019ECC4(p, a, swap);
    *(void**)p = lbl_80333968;
    if (swap) {
        Swap32(p + 0x24);
        Swap32(p + 0x28);
        if (S32(p, 0x20) == 0x3EB) {
            F32(p, 0x24) = F32(p, 0x24) + 3.35f;
        }
    }
    return p;
}

void fn_801A15CC(u8* p) {}  // stub (123)

void fn_801A17B8(u8* p) {}  // stub (56)

u8* fn_801A1898(u8* p, u32 a, u32 swap) {
    fn_801A1520(p, a, swap);
    *(void**)p = lbl_80333918;
    return p;
}

void fn_801A18D4(u8* p) {}  // stub (122)

void fn_801A1ABC(u8* p) {}  // stub (56)

void fn_801A1B9C(u8* p) {}  // stub (65)

void fn_801A1CA0(u8* p) {
    if (S32(p, 0x20) < 1000) {
        fn_801E44F4(PTR(p, 0xC), 0x40FFFF00, F32(p, 0x24), F32(p, 0x24), F32(p, 0x28));
    } else {
        fn_801E44F4(PTR(p, 0xC), 0x4000FF80, F32(p, 0x24), F32(p, 0x24), F32(p, 0x28));
    }
}

void fn_801A1CE4(u8* p) {}  // stub (60)

void fn_801A1DD4(u8* a, u8* obj) {
    switch (S32(obj, 0x18)) {
    case 'FLSR':
        fn_8019D06C(obj, a, 0, 0);
        break;
    case 'SPHR':
        fn_8019BD54(obj, a, 0, 0);
        break;
    case 'TUBE':
        fn_8019EA7C(obj, a);
        break;
    case 'GRUP':
        ((VGroup*)obj)->v10(a);
        break;
    }
}

void fn_801A1E7C(u8* p) {}  // stub (50)

void fn_801A1F44(RVec3* out, u8* q) {
    u8* o = VE(q)->v50();
    *(W3*)out = *(W3*)(o + 0x24);
}

void fn_801A1F98(u8* p) {}  // stub (195)

void fn_801A22A4(u8* p) {
    Xform x = *(Xform*)((VEx*)p)->v50();
    fn_801E44F4((u8*)&x, 0x50000000, F32(p, 0x24), F32(p, 0x28), F32(p, 0x2C));
}

void fn_801A2354(u8* p) {}  // stub (56)

u8* fn_801A2434(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_801A2474(u8* p) {}  // stub (97)

void fn_801A25F8(u8* p) {}  // stub (31)

void fn_801A2674(u8* p, f32 r, u32 a, u32 b) {
    F32(p, 0x24) = r;
    U32(p, 0x28) = a;
    U32(p, 0x2C) = b;
}

void fn_801A2684(u8* p, RVec3* lo, RVec3* hi) {
    f32 r = F32(p, 0x24);
    u8* o = PTR(p, 0xC);
    f32 k = 1.0f;
    RVec3 e;
    e.x = k;
    e.y = k;
    e.z = k;
    f32 a = k * -r;
    f32 b = k * r;
    lo->x = F32(o, 0x24) + a;
    lo->y = F32(o, 0x28) + a;
    lo->z = F32(o, 0x2C) + a;
    hi->x = F32(o, 0x24) + b;
    hi->y = F32(o, 0x28) + b;
    hi->z = F32(o, 0x2C) + b;
}

void fn_801A26FC(u8* p) {}  // stub (360)

u8* fn_801A2C9C(u8* p) { return p + 0x44; }

f32 fn_801A2CA4(u8* p) { return F32(p, 0x178); }

void fn_801A2CAC(u8* p) { fn_801E3FC8(PTR(p, 0xC) + 0x24, lbl_8055E598.value, F32(p, 0x24)); }

void fn_801A2CC0(u8* p) {}  // stub (36)

void fn_801A2D50(u8* p) {}  // stub (94)

void fn_801A2EC8(u8* p) {}

void fn_801A2ECC(u8* p) {}  // stub (170)

void fn_801A3174(u8* p) { fn_801E3FC8(PTR(p, 0xC) + 0x24, lbl_8055E598.value, F32(p, 0x24) + F32(p, 0x28)); }

void fn_801A3190(u8* p) {}  // stub (47)

void fn_801A324C(u8* p) {}  // stub (161)

void fn_801A34D0(u8* p) {}  // stub (236)

f32 fn_801A3880(u8* p) {
    f32 a = PSVECDotProduct(p + 0x30, p + 0x30);
    f32 b = PSVECDotProduct(p + 0x24, p + 0x24);
    return (f32)sqrt(b > a ? b : a);
}

void fn_801A38E4(u8* p) {}  // stub (191)

void fn_801A3BE0(u8* p) {}  // stub (115)

void fn_801A3DAC(u8* p) {}  // stub (108)

u8* fn_801A3F5C(u8* p, u32 a, u32 b) {
    fn_8019ECC4(p, a, b);
    *(void**)p = lbl_80333728;
    if (b) {
        PTR(p, 0x24) = p + 0x40;
        fn_801CFA48(p + 0x40);
    }
    return p;
}

u8* fn_801A3FB8(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_80333728;
        fn_801CFF14(PTR(p, 0x24));
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

void fn_801A401C(u8* p) {}  // stub (55)

void fn_801A40F8(u8* p) {}  // stub (205)

void fn_801A442C(u8* p, RVec3* a, RVec3* b) {
    *a = *(RVec3*)(p + 0x28);
    *b = *(RVec3*)(p + 0x34);
}

f32 fn_801A4460(u8* p) {
    f32 a = PSVECDotProduct(p + 0x34, p + 0x34);
    f32 b = PSVECDotProduct(p + 0x28, p + 0x28);
    return (f32)sqrt(b > a ? b : a);
}

void fn_801A44C4(u8* p) { fn_801CFF18(PTR(p, 0x24), U32(p, 0xC)); }

void fn_801A44D4(u8* l) {
    for (int i = 0; i < S32(l, 0x0); i++) {
        fn_801A49FC(((u8**)PTR(l, 0x4))[i]);
    }
}

void fn_801A4538(void* a) { fn_8012C4E4(a, (void*)fn_801A44D4, 0); }

void fn_801A4548(u8* p) {}  // stub (136)

void fn_801A4768(s32* p, s32 base) {
    if (*p) {
        *p += base;
    }
}

void fn_801A4780(s32* p, s32 base) {
    if (*p) {
        *p += base;
    }
}

void fn_801A4798(s32* p, s32 base) {
    if (*p) {
        *p += base;
    }
}

void fn_801A47B0(s32* p, s32 base) {
    if (*p) {
        *p += base;
    }
}

void fn_801A47C8(u8* l) {
    for (int i = 0; i < S32(l, 0x0); i++) {
        U8(((u8**)PTR(l, 0x4))[i], 0x40) = 1;
    }
}

void fn_801A47FC(u8* l) {
    if (l) {
        for (int i = 0; i < S32(l, 0x0); i++) {
            fn_801A49E8(((u8**)PTR(l, 0x4))[i]);
        }
    }
}

struct OnMsg {
    u32 id;
    u8 on;
};

void fn_801A4868(u8* p, u32 v) {
    if (U8(p, 0x40) != v) {
        OnMsg m;
        m.id = U32(p, 0x38);
        m.on = v;
        fn_801796EC(0xC, &m, 8);
        U8(p, 0x40) = v;
    }
}

void fn_801A48C8(u8* p) { VE(PTR(p, 0x48))->v18("dyno", "unknown"); }

void fn_801A48E4(u8* p) {}  // stub (65)

void fn_801A49E8(u8* p) {
    if (PTR(p, 0x48)) {
        fn_801D0650(p);
    }
}

void fn_801A49FC(u8* p) {
    if (PTR(p, 0x48)) {
        fn_801D06C4(p);
    }
}

void fn_801A4A10(u8* p, const RVec3* c, void* f, const RVec3* b) {}  // stub (31)

void fn_801A4A8C(u8* p) {}  // stub (52)

void fn_801A4B5C(u8* p) {}  // stub (179)

void fn_801A4E28(void) { lbl_8055E5A0 = fn_801CFEAC("track.msh"); }

void fn_801A4E54(void) {
    if (lbl_8055E5A0) {
        fn_801CFF14(lbl_8055E5A0);
        lbl_8055E5A0 = 0;
    }
}

int fn_801A4E88(u32 a, u32 b, u32 c, u32 d, s32* out) {
    if (lbl_8055E5A0 && fn_801CF828(lbl_8055E5A0, a, b, c, d)) {
        *out = 0x6E;
        return 1;
    }
    return 0;
}

u8* fn_801A4F00(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

u8* fn_801A4F40(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

u8* fn_801A4F80(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

u8* fn_801A4FC0(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

u8* fn_801A5000(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

u8* fn_801A5040(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

u8* fn_801A5080(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

u8* fn_801A50C0(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

f32 fn_801A5100(u8* p) { return F32(p, 0x24); }

void fn_801A5108(u8* p) { S32(p, 0x180) = -1; }

f32 fn_801A5114(u8* p) { return F32(p, 0x30); }

f32 fn_801A511C(u8* p) { return F32(p, 0x24); }

f32 fn_801A5124(u8* p) { return F32(p, 0x28); }

u32 fn_801A512C(u8* p) { return 0; }

u8* fn_801A5134(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

u32 fn_801A5174(u8* p) { return U32(p, 0x48); }

u8* fn_801A517C(u8* p) { return p + 0x28; }

u8 fn_801A5184(u8* p) { return U8(p, 0x40); }

void fn_801A518C(u8* p, u32 v) { fn_801A4868(p, v); }

void fn_801A5190(u8* p) {
    u8* o = PTR(p, 0x3C);
    if (o) {
        VE(o)->v40();
    }
}

s32 fn_801A51B0(u8* p) {
    u8* o = PTR(p, 0x3C);
    if (o) {
        return S32(o, 0x20);
    }
    return -1;
}

u32 fn_801A51CC(u8* p) { return U32(p, 0x34); }

void fn_801A51D4(u8* p, u32 v) { U32(p, 0x34) = v; }



}  // extern "C"
