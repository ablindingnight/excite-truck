// Racer slots (0x8026A600-0x8026B118): the four per-racer records built at start-up, the slot
// constructor and the distance-along-axis helper. The large update routine is still a stub. Names
// are guesses.
#include "game/common.h"
#include "macros.h"
#include <string.h>
#include <math.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

struct Mat3 {
    f32 m[9];
};
struct Vec3;
void Mat3Mul(Mat3* out, const Mat3* a, const Mat3* b);
void Mat3FromEuler(Mat3* out, const Vec3* euler);

struct SVec {
    f32 x, y, z;
};

// Rotation + position (a camera frame).
struct SFrame {
    Mat3 rot;
    SVec pos;
};

extern s32 lbl_8055E3C4;  // frame counter
extern f32 lbl_80558DC4[];

extern "C" {
int fn_801CF064(void);
void* fn_801769CC(void);
void fn_80176A00(int mode);
void fn_80176A24(f32 t);
int fn_80178CA4(void);
int fn_801B8570(int car);
int fn_8026A38C(void);
int fn_801784AC(const SVec* from, const SVec* to, SVec* hit, SVec* n, void* a, void* b, int flags);
int fn_801EA03C(u8* ground, SVec* p);
void fn_8026E22C(u8* out, SFrame* from, SFrame* to, f32 t);
}

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

// dst = m * src (column-major rotation), with the aliasing guard of the inline.
static inline void MulVec(const Mat3& m, const SVec& src, SVec* dst) {
    if (dst == &src) {
        f32 x = Dot3(src.x, m.m[0], src.y, m.m[3], src.z, m.m[6]);
        f32 y = Dot3(src.x, m.m[1], src.y, m.m[4], src.z, m.m[7]);
        f32 z = Dot3(src.x, m.m[2], src.y, m.m[5], src.z, m.m[8]);
        dst->x = x;
        dst->y = y;
        dst->z = z;
    } else {
        dst->x = Dot3(src.x, m.m[0], src.y, m.m[3], src.z, m.m[6]);
        dst->y = Dot3(src.x, m.m[1], src.y, m.m[4], src.z, m.m[7]);
        dst->z = Dot3(src.x, m.m[2], src.y, m.m[5], src.z, m.m[8]);
    }
}

extern "C" {
void fn_80265DD8(u8* p);
void fn_80266120(u8* p);
f32 PSVECDotProduct(const f32* a, const f32* b);
}

// One racer's record; its constructor lives in the next unit.
struct RacerSlot {
    u8 active;          // offset 0x0
    SFrame frame;       // offset 0x4: the camera frame when the orbit started
    f32 fov;            // offset 0x34
    f32 yaw;            // offset 0x38
    SVec target;        // offset 0x3C
    SVec pos;           // offset 0x48
    SVec vel;           // offset 0x54
    f32 t;              // offset 0x60
    f32 spin;           // offset 0x64
    s32 frameNo;        // offset 0x68
    RacerSlot(void);
};

// The car's controller; slot 0x14 returns its option flags.
class Ctl {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual u32 Flags(void);
};

// A camera record (cammgr.cpp builds four); its constructor is the first function of this unit.
struct CamRec {
    CamRec(void);
    u8 data[0x228];
};

CamRec::CamRec(void) {
    fn_80265DD8(data + 0x160);
    fn_80266120(data + 0x178);
}

ModuleTag lbl_8055F4B0;
RacerSlot lbl_80499E60[4];

extern "C" {

f32 fn_8026A63C(const f32* a, const f32* b, const f32* axis) {
    f32 d[3];
    d[0] = a[0] - b[0];
    d[1] = a[1] - b[1];
    d[2] = a[2] - b[2];
    return PSVECDotProduct(d, axis);
}

// The racer-slot (finish orbit) camera: when the race is over for this car, record its frame, find how far
// the camera can swing left and right before hitting scenery, then orbit the car and blend into the orbit.
// Returns 1 while it owns the camera.
int fn_8026A698(u8* p, int i) {
    u8* car = PTR(p, 0x140);
    RacerSlot* s = &lbl_80499E60[i];
    int ok = 1;
    if (U32(car, 0x10) == 'PCAR') {
        u8* ctl = PTR(car, 0x270C);
        if (((Ctl*)ctl)->Flags() & 0x20) {
            ok = 0;
        }
    }
    if (fn_801CF064() || S32(p, 0x10) == 0x11 || S32(p, 0x10) == 0x15 || (S32(fn_801769CC(), 4) & 0x4000) || ok) {
        if (s->active) {
            fn_80176A00(1);
        }
        s->t = 0.0f;
        s->active = 0;
        return 0;
    }
    int n = fn_80178CA4();
    int multi = n > 1;
    int done = 0;
    u8* c = PTR(p, 0x140);
    if (!((f32)__fabs(F32(c, 0x2690)) <= lbl_80558DC4[0]) && fn_801B8570(S32(c, 0x1EC))) {
        done = 1;
    }
    if (!done) {
        s->t = 0.0f;
        if (s->active) {
            s->active = 0;
            if (!multi) {
                U8(p, 0x138) = 1;
            }
            fn_80176A00(1);
        }
        return 0;
    }
    if (multi) {
        s->active = 1;
        return 0;
    }
    if (fn_8026A38C()) {
        s->active = 0;
        fn_80176A00(1);
        return 0;
    }
    if (!s->active) {
        f32 room[2] = {1000.0f, 1000.0f};
        f32* r = room;
        for (int k = 0; k < 2; k++) {
            SVec from = *(const SVec*)(p + 0xE4);
            SVec side;
            side.x = k ? -1.0f : 1.0f;
            side.y = 0.0f;
            side.z = 0.0f;
            SVec dir;
            SVec nrm;
            SVec to;
            SVec hit;
            u32 a;
            u32 b;
            MulVec(*(const Mat3*)(p + 0xC0), side, &dir);
            from.x += dir.x * -0.5f;
            from.y += dir.y * -0.5f;
            from.z += dir.z * -0.5f;
            to.x = from.x + dir.x * 5.0f;
            to.y = from.y + dir.y * 5.0f;
            to.z = from.z + dir.z * 5.0f;
            if (fn_801784AC(&from, &to, &hit, &nrm, &a, &b, 0)) {
                *r = fn_8026A63C(&hit.x, &from.x, &dir.x);
            }
            r++;
        }
        U8(p, 0x138) = 1;
        s->active = 1;
        s->frame = *(const SFrame*)(p + 0x88);
        s->fov = F32(p, 0x194);
        f32 turn = 2.3561945f;
        s->yaw = atan2(s->frame.rot.m[6], s->frame.rot.m[8]);
        turn = PSVECDotProduct((f32*)(PTR(p, 0x140) + 0x118), (f32*)(p + 0xC0)) > 0.0f ? turn : -turn;
        s->spin = turn;
        if (MIN(room[0], room[1]) < 10.0f) {
            if (room[0] > room[1]) {
                s->spin = -(f32)__fabs(s->spin);
            } else {
                s->spin = (f32)__fabs(s->spin);
            }
        }
        if (s->spin > 0.0f) {
            s->yaw += 2.3561945f;
        } else {
            s->yaw -= 2.3561945f;
        }
        s->spin *= -0.1f;
        s->target.x = F32(p, 0xE4);
        s->target.y = F32(p, 0xE8);
        s->target.z = F32(p, 0xEC);
        s->pos = s->target;
        s->vel.x = 0.0f;
        s->vel.y = 0.0f;
        s->vel.z = 0.0f;
        s->frameNo = lbl_8055E3C4;
    } else {
        U8(p, 0x138) = 0;
    }
    s->t += 0.016683351f * (f32)(lbl_8055E3C4 - s->frameNo);
    if (lbl_8055E3C4 != s->frameNo) {
        SVec at = *(const SVec*)(p + 0xE4);
        at.x += F32(p, 0xCC) * 0.7f;
        at.y += F32(p, 0xD0) * 0.7f;
        at.z += F32(p, 0xD4) * 0.7f;
        s->target = at;
        SVec d;
        d.x = at.x - s->pos.x;
        d.y = at.y - s->pos.y;
        d.z = at.z - s->pos.z;
        s->vel.x = s->vel.x + 0.2f * (d.x - s->vel.x);
        s->vel.y = s->vel.y + 0.2f * (d.y - s->vel.y);
        s->vel.z = s->vel.z + 0.2f * (d.z - s->vel.z);
        SVec v = s->vel;
        s->pos.x += v.x * 1.0f;
        s->pos.y += v.y * 1.0f;
        s->pos.z += v.z * 1.0f;
    }
    SFrame f;
    f.pos = s->pos;
    f.rot.m[0] = 1.0f;
    f.rot.m[1] = 0.0f;
    f.rot.m[2] = 0.0f;
    f.rot.m[3] = 0.0f;
    f.rot.m[4] = 1.0f;
    f.rot.m[5] = 0.0f;
    f.rot.m[6] = 0.0f;
    f.rot.m[7] = 0.0f;
    f.rot.m[8] = 1.0f;
    f32 orbit[3];
    orbit[0] = 0.0f;
    orbit[1] = s->yaw + s->t * s->spin;
    orbit[2] = 0.0f;
    Mat3 m1;
    Mat3FromEuler(&m1, (const Vec3*)orbit);
    Mat3Mul(&f.rot, &m1, &f.rot);
    f32 tilt[3];
    tilt[0] = 0.2617994f;
    tilt[1] = 0.0f;
    tilt[2] = 0.0f;
    Mat3 m2;
    Mat3FromEuler(&m2, (const Vec3*)tilt);
    Mat3Mul(&f.rot, &m2, &f.rot);
    SVec back;
    back.x = 0.0f;
    back.y = 0.0f;
    back.z = -6.0f;
    SVec off;
    MulVec(f.rot, back, &off);
    f.pos.x += off.x;
    f.pos.y += off.y;
    f.pos.z += off.z;
    SVec g = f.pos;
    if (fn_801EA03C(PTR(p, 0x140) + 0x6C, &g)) {
        g.y += 1.0f;
        f32 up = MAX(0.0f, g.y - f.pos.y);
        f.pos.y += up;
        f32 euler[3];
        euler[0] = (f32)atan(up / 6.0f);
        euler[1] = 0.0f;
        euler[2] = 0.0f;
        Mat3 m3;
        Mat3FromEuler(&m3, (const Vec3*)euler);
        Mat3Mul(&f.rot, &m3, &f.rot);
    }
    fn_80176A24(3.0f);
    fn_80176A00(2);
    f32 k = MIN(1.0f, 3000.0f * (3.0f * s->t));
    f32 e;
    if (k < -1.0f) {
        e = -1.0f;
    } else if (k > 1.0f) {
        e = 1.0f;
    } else if (k > 0.0f) {
        e = k * (-2.0f * k * k) + 3.0f * k * k;
    } else {
        e = k * (-2.0f * k * k) - 3.0f * k * k;
    }
    fn_8026E22C(p + 0x58, &s->frame, &f, e);
    S32(p, 0x10) = 5;
    F32(p, 0xBC) = s->fov + e * (1.3089969f - s->fov);
    s->frameNo = lbl_8055E3C4;
    return 1;
}

u8 fn_8026B0C0(int i) { return lbl_80499E60[i].active; }

}  // extern "C"
