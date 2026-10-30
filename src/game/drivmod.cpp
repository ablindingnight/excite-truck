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

}  // extern "C"

struct BVec {
    f32 x, y, z;
};

// A bone's rotation, as three rows.
struct BRot {
    BVec r[3];
};

// A posed bone: rotation and position in the model's frame.
struct BonePose {
    BRot rot;  // offset 0x0
    BVec pos;  // offset 0x24
};

// One bone of the rider: which pose slot, the skeleton point it sits on and the child point it aims at.
struct BoneLink {
    s32 index;  // offset 0x0
    s32 point;  // offset 0x4
    s32 child;  // offset 0x8, -1: none
};

f32 Dot(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz);
double sin(double x);
double cos(double x);
extern "C" {
f32 PSVECMag(const f32* v);
void PSVECCrossProduct(const f32* a, const f32* b, f32* out);
}
extern const f32 lbl_80563058;  // 1.0
extern const f32 lbl_8056305C;  // 0.0
extern const f32 lbl_80563068;  // 0.001
extern const f32 lbl_8056306C;  // -1.0
extern const f32 lbl_80563070;  // 0.0001

static inline void MulM(f32* out, const f32* a, const f32* b) {
    if (out == b || out == a) {
        f32 m0 = Dot(a[0], b[0], a[3], b[1], a[6], b[2]);
        f32 m3 = Dot(a[0], b[3], a[3], b[4], a[6], b[5]);
        f32 m6 = Dot(a[0], b[6], a[3], b[7], a[6], b[8]);
        f32 m1 = Dot(a[1], b[0], a[4], b[1], a[7], b[2]);
        f32 m4 = Dot(a[1], b[3], a[4], b[4], a[7], b[5]);
        f32 m7 = Dot(a[1], b[6], a[4], b[7], a[7], b[8]);
        f32 m2 = Dot(a[2], b[0], a[5], b[1], a[8], b[2]);
        f32 m5 = Dot(a[2], b[3], a[5], b[4], a[8], b[5]);
        f32 m8 = Dot(a[2], b[6], a[5], b[7], a[8], b[8]);
        out[0] = m0;
        out[3] = m3;
        out[6] = m6;
        out[1] = m1;
        out[4] = m4;
        out[7] = m7;
        out[2] = m2;
        out[5] = m5;
        out[8] = m8;
    } else {
        out[0] = Dot(a[0], b[0], a[3], b[1], a[6], b[2]);
        out[3] = Dot(a[0], b[3], a[3], b[4], a[6], b[5]);
        out[6] = Dot(a[0], b[6], a[3], b[7], a[6], b[8]);
        out[1] = Dot(a[1], b[0], a[4], b[1], a[7], b[2]);
        out[4] = Dot(a[1], b[3], a[4], b[4], a[7], b[5]);
        out[7] = Dot(a[1], b[6], a[4], b[7], a[7], b[8]);
        out[2] = Dot(a[2], b[0], a[5], b[1], a[8], b[2]);
        out[5] = Dot(a[2], b[3], a[5], b[4], a[8], b[5]);
        out[8] = Dot(a[2], b[6], a[5], b[7], a[8], b[8]);
    }
}

static inline void SetIdentity(f32* m) {
    m[0] = lbl_80563058;
    m[1] = lbl_8056305C;
    m[2] = lbl_8056305C;
    m[3] = lbl_8056305C;
    m[4] = lbl_80563058;
    m[5] = lbl_8056305C;
    m[6] = lbl_8056305C;
    m[7] = lbl_8056305C;
    m[8] = lbl_80563058;
}

extern "C" {

// Poses the rider's bones on the skeleton points: each bone sits on its point (in the model's frame f)
// and, when it has a child, is turned from its bind direction to aim at the child.
void fn_802009CC(BonePose* out, const BVec* pts, const BoneLink* links, int n, const u8* bind, const BonePose* f) {
    for (int i = 0; i < n; i++, links++) {
        BonePose* o = &out[links->index];
        const BRot* b = (const BRot*)(bind + links->index * 0x38);
        BVec v;
        v.x = pts[links->point].x - f->pos.x;
        v.y = pts[links->point].y - f->pos.y;
        v.z = pts[links->point].z - f->pos.z;
        {
            f32 x = v.z * f->rot.r[0].z + (v.x * f->rot.r[0].x + v.y * f->rot.r[0].y);
            f32 y = v.z * f->rot.r[1].z + (v.x * f->rot.r[1].x + v.y * f->rot.r[1].y);
            f32 z = v.z * f->rot.r[2].z + (v.x * f->rot.r[2].x + v.y * f->rot.r[2].y);
            v.x = x;
            v.y = y;
            v.z = z;
        }
        o->pos = v;
        if (links->child >= 0) {
            BVec c;
            c.x = pts[links->child].x - f->pos.x;
            c.y = pts[links->child].y - f->pos.y;
            c.z = pts[links->child].z - f->pos.z;
            {
                f32 x = c.z * f->rot.r[0].z + (c.x * f->rot.r[0].x + c.y * f->rot.r[0].y);
                f32 y = c.z * f->rot.r[1].z + (c.x * f->rot.r[1].x + c.y * f->rot.r[1].y);
                f32 z = c.z * f->rot.r[2].z + (c.x * f->rot.r[2].x + c.y * f->rot.r[2].y);
                c.x = x;
                c.y = y;
                c.z = z;
            }
            BVec d;
            d.x = c.x - v.x;
            d.y = c.y - v.y;
            d.z = c.z - v.z;
            f32 len = PSVECMag(&d.x);
            BRot m;
            if (len > lbl_80563068) {
                f32 k = lbl_8056306C / len;
                BVec u;
                u.x = d.x * k;
                u.y = d.y * k;
                u.z = d.z * k;
                BVec axis;
                PSVECCrossProduct(&b->r[0].x, &u.x, &axis.x);
                f32 a = PSVECMag(&axis.x);
                BRot r;
                if (a > lbl_80563070) {
                    f32 inv = lbl_80563058 / a;
                    f32 ax = inv * axis.x;
                    f32 ay = inv * axis.y;
                    f32 az = inv * axis.z;
                    f32 sn = sin(a);
                    f32 cs = cos(a);
                    f32 t = lbl_80563058 - cs;
                    r.r[0].x = cs + t * (ax * ax);
                    r.r[0].y = ay * (t * ax) + sn * az;
                    r.r[0].z = az * (t * ax) - sn * ay;
                    r.r[1].x = ay * (t * ax) - sn * az;
                    r.r[1].y = cs + t * (ay * ay);
                    r.r[1].z = az * (t * ay) + sn * ax;
                    r.r[2].x = az * (t * ax) + sn * ay;
                    r.r[2].y = az * (t * ay) - sn * ax;
                    r.r[2].z = cs + t * (az * az);
                } else {
                    SetIdentity(&r.r[0].x);
                }
                MulM(&m.r[0].x, &b->r[0].x, &r.r[0].x);
            } else {
                SetIdentity(&m.r[0].x);
            }
            o->rot = m;
        } else {
            o->rot.r[0] = b->r[0];
            o->rot.r[1] = b->r[1];
            o->rot.r[2] = b->r[2];
        }
    }
}

void fn_80201018(u8* p) {}  // stub (1137)

}  // extern "C"
