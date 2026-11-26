// Racer slots (0x8026A600-0x8026B118): the four per-racer records built at start-up, the slot
// constructor and the distance-along-axis helper. The large update routine is still a stub. Names
// are guesses.
#include "game/common.h"
#include "game/camblend.h"
#include "game/raceopts.h"
#include <revolution/MTX/vec.h>
#include "game/racer_camera_slot.h"
#include "game/camrec.h"
#include "game/carbody.h"
#include "macros.h"
#include <string.h>
#include <math.h>
#include "game/mathutil.h"
#include "game/tadds.h"
#include "game/tscale.h"
// Passed to fn_80176A24 (3.0f; name unproven). A named constant with external linkage defined before any code:
// retail keeps it as the FIRST .sdata2 entry, separate from the pooled 3.0f literal used further down.
extern const f32 lbl_80563D08;
const f32 lbl_80563D08 = 3.0f;

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

extern s32 lbl_8055E3C4;  // frame counter
extern f32 __float_epsilon[];

extern "C" {
int fn_801CF064(void);
void fn_80176A00(int mode);
void fn_80176A24(f32 t);
int fn_80178CA4(void);
int fn_801B8570(int car);
int fn_8026A38C(void);
int fn_801784AC(const ValVec3* from, const ValVec3* to, ValVec3* hit, ValVec3* n, void* a, void* b, int flags);
int fn_801EA03C(u8* ground, ValVec3* p);
}


// dst = m * src (column-major rotation), with the aliasing guard of the inline.
static inline void MulVec(const Mat3& m, const ValVec3& src, ValVec3* dst) {
    if (dst == &src) {
        f32 x = Dot3(src.x, m.m[0][0], src.y, m.m[1][0], src.z, m.m[2][0]);
        f32 y = Dot3(src.x, m.m[0][1], src.y, m.m[1][1], src.z, m.m[2][1]);
        f32 z = Dot3(src.x, m.m[0][2], src.y, m.m[1][2], src.z, m.m[2][2]);
        dst->x = x;
        dst->y = y;
        dst->z = z;
    } else {
        dst->x = Dot3(src.x, m.m[0][0], src.y, m.m[1][0], src.z, m.m[2][0]);
        dst->y = Dot3(src.x, m.m[0][1], src.y, m.m[1][1], src.z, m.m[2][1]);
        dst->z = Dot3(src.x, m.m[0][2], src.y, m.m[1][2], src.z, m.m[2][2]);
    }
}


// The car's controller; slot 0x14 returns its option flags.
class Ctl {
public:
    virtual ~Ctl(void);  // 0x08 (the car's Controller (0x270C, Hybrid/IDriver): slot 0 is its destructor)
    virtual void v0C(void);
    virtual void v10(void);
    virtual u32 Flags(void);
};

ModuleTag lbl_8055F4B0;
CameraRacerSlot lbl_80499E60[4];

extern "C" {

f32 fn_8026A63C(const f32* a, const f32* b, const f32* axis) {
    f32 d[3];
    d[0] = a[0] - b[0];
    d[1] = a[1] - b[1];
    d[2] = a[2] - b[2];
    return PSVECDotProduct((const Vec*)d, (const Vec*)axis);
}

// The racer-slot (finish orbit) camera: when the race is over for this car, record its frame, find how far
// the camera can swing left and right before hitting scenery, then orbit the car and blend into the orbit.
// Returns 1 while it owns the camera.
}
extern "C" {

static inline void FollowStep(CameraRacerSlot* s, const ValVec3& d) {  // name unproven
    ValVec3 v;
    f32 zv = s->vel.z;
    f32 blend = 0.2f;
    f32 xv = s->vel.x;
    v.Set(xv + blend * (d.x - xv), s->vel.y + blend * (d.y - s->vel.y), zv + blend * (d.z - zv));
    s->vel = v;
    TAddSTo(&s->pos, &v, 1.0f);
}

// name unproven
static inline void Turn(Mat3* m, f32 x, f32 y, f32 z) {
    f32 e[3];
    e[0] = x;
    e[1] = y;
    e[2] = z;
    Mat3 r;
    Mat3FromEuler(&r, (const Vec3*)e);
    Mat3Mul(m, &r, m);
}

// name unproven
static inline ValVec3 Rotated(const Mat3& m, const ValVec3& v) {
    ValVec3 o;
    MulVec(m, v, &o);
    return o;
}

// name unproven
static inline void AddRotated(ValVec3* o, const Mat3& m, const ValVec3& v) {
    ValVec3 off;
    MulVec(m, v, &off);
    o->x += off.x;
    o->y += off.y;
    o->z += off.z;
}

// name unproven
static inline void Push(PlainValMatrix& f, f32 x, f32 y, f32 z) {
    ValVec3 back;
    back.x = x;
    back.y = y;
    back.z = z;
    AddRotated(&f.origin, f.Mat(), back);
}

struct MatRef { Mat3* m; };  // name unproven

int fn_8026A698(u8* p, int i) {
    f32* r;
    u8* car = ((CamRec*)p)->car;
    CameraRacerSlot* s = &lbl_80499E60[i];
    int ok = 1;
    if (((CarBody*)car)->type == 'PCAR') {
        u8* ctl = PTR(car, 0x270C);
        if (((Ctl*)ctl)->Flags() & 0x20) {
            ok = 0;
        }
    }
    if (fn_801CF064() || ((CamRec*)p)->mode == 0x11 || ((CamRec*)p)->mode == 0x15 || (fn_801769CC()->type & 0x4000) || ok) {
        if (s->active) {
            fn_80176A00(1);
        }
        s->t = 0.0f;
        s->active = 0;
        return 0;
    }
    int n = fn_80178CA4();
    bool multi = n > 1;
    u8* c = ((CamRec*)p)->car;
    int done = 0;
    if (!((f32)__fabs(((CarBody*)c)->f2690) <= __float_epsilon[0]) && fn_801B8570(((CarBody*)c)->index)) {
        done = 1;
    }
    if (!done) {
        s->t = 0.0f;
        if (s->active) {
            s->active = 0;
            if (!multi) {
                ((CamRec*)p)->b138 = 1;
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
        r = room;
        for (int k = 0; k < 2; k++) {
            ValVec3 from = *(const ValVec3*)&((CamRec*)p)->carFrame.pos;
            ValVec3 side;
            side.x = k ? -1.0f : 1.0f;
            side.y = 0.0f;
            side.z = 0.0f;
            ValVec3 dir;
            ValVec3 nrm;
            ValVec3 to;
            ValVec3 hit;
            u32 a;
            u32 b;
            MulVec(*(const Mat3*)&((CamRec*)p)->carFrame, side, &dir);
            TAddSTo(&from, &dir, -0.5f);
            TAddS(&to, &from, &dir, 5.0f);
            if (fn_801784AC(&from, &to, &hit, &nrm, &a, &b, 0)) {
                *r = fn_8026A63C(&hit.x, &from.x, &dir.x);
            }
            r++;
        }
        ((CamRec*)p)->b138 = 1;
        s->active = 1;
        s->frame = *(const PlainValMatrix*)&((CamRec*)p)->lastPose;
        s->fov = ((CamRec*)p)->f194;
        s->yaw = atan2(s->frame.basis[2].x, s->frame.basis[2].z);
        f32 turn = 2.3561945f;
        // p + 0xC0 = CamRec::carFrame (typed, together with the MulVec read above, it grows the frame by 16)
        turn = PSVECDotProduct((const Vec*)&((CarBody*)((CamRec*)p)->car)->vel, (const Vec*)((f32*)(p + 0xC0))) > 0.0f ? turn : -turn;
        s->spin = turn;
        if (MinF(room[0], room[1]) < 10.0f) {
            if (room[0] > room[1]) {
                s->spin = -(f32)__fabs(s->spin);
            } else {
                s->spin = (f32)__fabs(s->spin);
            }
        }
        f32 swing = 2.3561945f;  // 3*pi/4
        if (s->spin > 0.0f) {
            s->yaw += swing;
        } else {
            s->yaw -= swing;
        }
        s->spin *= -0.1f;
        s->target.x = ((CamRec*)p)->carFrame.pos.v[0];
        s->target.y = ((CamRec*)p)->carFrame.pos.v[1];
        s->target.z = ((CamRec*)p)->carFrame.pos.v[2];
        s->pos = s->target;
        s->vel.x = 0.0f;
        s->vel.y = 0.0f;
        s->vel.z = 0.0f;
        s->frameNo = lbl_8055E3C4;
    } else {
        ((CamRec*)p)->b138 = 0;
    }
    s->t += 0.016683351f * (f32)(lbl_8055E3C4 - s->frameNo);
    if (lbl_8055E3C4 != s->frameNo) {
        ValVec3 at = *(const ValVec3*)&((CamRec*)p)->carFrame.pos;
        TAddSTo(&at, (const ValVec3*)&((CamRec*)p)->carFrame.rot.m[3], 0.7f);
        ValVec3 d;
        d.x = at.x - s->pos.x;
        d.y = at.y - s->pos.y;
        d.z = at.z - s->pos.z;
        s->target = at;
        FollowStep(s, d);
    }
    PlainValMatrix f;
    f.origin = s->pos;
    f.basis[0].x = 1.0f;
    f.basis[0].y = 0.0f;
    f.basis[0].z = 0.0f;
    f.basis[1].x = 0.0f;
    f.basis[1].y = 1.0f;
    f.basis[1].z = 0.0f;
    f.basis[2].x = 0.0f;
    f.basis[2].y = 0.0f;
    f.basis[2].z = 1.0f;
    MatRef fr = {&f.Mat()};
    Turn(fr.m, 0.0f, s->yaw + s->t * s->spin, 0.0f);
    Turn(fr.m, 0.2617994f, 0.0f, 0.0f);
    Push(f, 0.0f, 0.0f, -6.0f);
    ValVec3 g = f.origin;
    if (fn_801EA03C((u8*)((CarBody*)((CamRec*)p)->car)->hint, &g)) {
        g.y += 1.0f;
        f32 up = MaxF(0.0f, g.y - f.origin.y);
        f.origin.y += up;
        Turn(&f.Mat(), (f32)atan(up / 6.0f), 0.0f, 0.0f);
    }
    const f32& duration = lbl_80563D08;
    fn_80176A24(duration);
    fn_80176A00(2);
    f32 k = MinF(1.0f, 3000.0f * (3.0f * s->t));
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
    InterpolateCameraMatrix((RMtx*)&((CamRec*)p)->pose, (const RMtx*)&s->frame, (const RMtx*)&f, e);
    ((CamRec*)p)->fov = s->fov + e * (1.3089969f - s->fov);
    ((CamRec*)p)->mode = 5;
    s->frameNo = lbl_8055E3C4;
    return 1;
}

int fn_8026B0C0(int i) { return lbl_80499E60[i].active; }

}  // extern "C"
