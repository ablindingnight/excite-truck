// Code 0x8026B844-0x8026E224, imported verbatim from the upstream project (github.com/orangedude27/excite-truck,
// CC0; file src/auto/text_8026B840_.c), where its functions match exactly. Unit name is provisional.
// CLEANUP: automated-decompiler source with raw offsets and placeholder types; rewrite with real types
// before the PC port. Tricks are tagged MATCHING-AID.
// Compiled as C++ (2026-10-31): the original copies the pad struct member by member, which MWCC does only in C++.
// fn_8026C014 (the debug camera) is a draft, not yet matching; it reaches both .bss objects from one base register,
// which is still unexplained.
#include "types.h"
#include "game/common.h"
#include <revolution/MTX/vec.h>
#include "game/math_types.h"
#include "game/mathutil.h"

// The unit's module tag, registered by its static initializer (the 8-byte function after the code, formerly the
// separate split unk_8026E224).
ModuleTag lbl_8055F4D8;

extern "C" {

extern void *lbl_8055F4E0;
extern void *lbl_8055F4E4;
extern void *lbl_8055F4E8;


void fn_8026B840(void) {}

f32 fn_8026B844(f32 f) {
    f32 c = 0.0f;
    if (f > c) {
        return MaxF(c, (f - 0.2f) / 0.8f);
    }
    return MinF(c, (0.2f + f) / 1.2f);
}

// Camera frames (camframe.cpp's CamFrame: three axes and a position).
typedef struct CamVec {
    union {
        f32 v[3];
        struct {
            f32 x, y, z;
        };
    };
} CamVec;
typedef struct CamRot {
    CamVec axes[3];
} CamRot;
typedef struct CamFrame {
    CamRot rot;  // offset 0x0
    CamVec pos;  // offset 0x24
} CamFrame;
typedef struct CamView {
    u8 pad0[0x58];
    CamFrame frame;       // offset 0x58
    u8 pad88[0xBC - 0x88];
    f32 fov;              // offset 0xBC
    u8 padC0[0x120 - 0xC0];
    f32 shake[3];         // offset 0x120
    u8 pad12C[0x138 - 0x12C];
    u8 track;             // offset 0x138
    u8 pad139[0x194 - 0x139];
    f32 trackFov;         // offset 0x194
} CamView;
typedef struct CamCar {
    u8 pad0[0x3C];
    CamFrame frame;  // offset 0x3C
} CamCar;

extern CamFrame lbl_80499050;  // the frame saved while tracking
extern f32 lbl_8055F47C;       // its field of view
extern s32 lbl_8055F4F0;       // the car the replay camera follows (-1: none)
s32 fn_8026A344(s32 i);
CamCar* fn_8017960C(s32 car);
void CalculateCameraMatrix(CamFrame* out, const CamVec* eye, const CamVec* target, const CamVec* up);
void fn_801BFD34(CamVec* out, const CamVec* v, const CamFrame* m);

// The pad as ReadJoyStatus fills it (achview's ViewPad, keybd's KeyState): sticks, the analog trigger at 0x1C,
// buttons, d-pad bits.
typedef struct FreePad {
    f32 x0, y0, x1, y1, x2, y2, x3, y3;  // offset 0x0 (y3: the trigger)
    u8 keys[0x20];   // offset 0x20
    s32 w40;         // offset 0x40 (d-pad bits)
    s32 w44[6];      // offset 0x44
    s32 w5C;         // offset 0x5C
    u8 valid;        // offset 0x60

    FreePad(void) {}
} FreePad;
typedef int (*CamGate)(s32 x, s32 y);

extern u32 lbl_8055F4FC;        // the free camera's last update time (ms)
extern f32 lbl_8055F4F4;        // its yaw
extern f32 lbl_8055F4F8;        // its pitch
extern s32 lbl_8055CFF0;        // screen width
extern s32 lbl_8055CFF4;        // screen height
void fn_80127A84(s32* x, s32* y, s32* buttons);
int ReadJoyStatus(FreePad* pad, int chan);
u32 fn_802E2C20(void);
int fn_802E4A1C(int key);
f32 fn_801EA0E8(f32 x, f32 z);

static __inline void CamMulVec(CamVec* out, const CamVec* in, const CamRot* m) {
    if (out == in) {
        f32 x = Dot3(in->x, m->axes[0].x, in->y, m->axes[1].x, in->z, m->axes[2].x);
        f32 y = Dot3(in->x, m->axes[0].y, in->y, m->axes[1].y, in->z, m->axes[2].y);
        f32 z = Dot3(in->x, m->axes[0].z, in->y, m->axes[1].z, in->z, m->axes[2].z);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, m->axes[0].x, in->y, m->axes[1].x, in->z, m->axes[2].x);
        out->y = Dot3(in->x, m->axes[0].y, in->y, m->axes[1].y, in->z, m->axes[2].y);
        out->z = Dot3(in->x, m->axes[0].z, in->y, m->axes[1].z, in->z, m->axes[2].z);
    }
}

// The free (debug) camera: stick yaw/pitch and movement, trigger boost, keys up/down, the pointer at a screen
// edge pans (faster the higher the camera is), and the camera keeps its height above the ground.
/* Helper name unproven: stick look and move of the free camera; the boundary is needed (in place -1.58) */
static inline void ReadFlyPad(FreePad& pad, f32 dt, f32 turn, f32 speed, CamVec& in, CamVec& move) {
    f32 x;
    f32 z;
    lbl_8055F4F4 += turn * (dt * fn_8026B844(pad.x0));
    if (lbl_8055F4F4 > 6.2831855f) {
        lbl_8055F4F4 -= 6.2831855f;
    } else if (lbl_8055F4F4 < -6.2831855f) {
        lbl_8055F4F4 += 6.2831855f;
    }
    lbl_8055F4F8 = MaxF(MinF(1.0471976f, lbl_8055F4F8 + turn * (dt * fn_8026B844(pad.y0))), -1.0471976f);
    z = speed * (dt * -fn_8026B844(pad.x1));
    x = speed * (dt * fn_8026B844(pad.y1));
    in.x = x;
    in.y = 0.0f;
    in.z = z;
    const CamRot* rot = &lbl_80499050.rot;
    CamMulVec(&move, &in, rot);
    lbl_80499050.pos.x += move.x;
    lbl_80499050.pos.y += move.y;
    lbl_80499050.pos.z += move.z;
}

// Helper name unproven: 1 + t * k; as an inline its constant read is numbered above the clamp it follows (retail f2/f3).
static inline f32 CamBlend(f32 t, f32 k) { return 1.0f + t * k; }
// The constants this unit reads are its own literals (its .sdata2 is created in retail order by the forms below).
// Helpers unproven (names and existence): each is the form that reproduces retail where a plain literal does not.
// CamUnit: v mapped from lo..hi to 0..1; retail creates 1.0 before lo and 1/(hi-lo).
static inline f32 CamUnit(f32 v, f32 lo, f32 hi) { return MaxF(MinF(1.0f, (v - lo) / (hi - lo)), 0.0f); }
// CamMulF: a product whose constant arrives as an inline parameter. A literal written in the multiply is always
// emitted as the first operand; retail has the variable first at these sites.
// CamSpeed: retail keeps the multiply by a scale of 1.0 (a literal 1.0f * x is folded away).
static inline f32 CamSpeed(f32 t, f32 scale) { return scale * (100.0f * CamBlend(t, 5.0f)); }
void fn_8026B8A0(void) {
    FreePad pad;
    Mat3 yawM;
    Mat3 pitchM;
    CamVec right;
    CamVec fwd;
    CamVec in;
    CamVec in2;
    Vec3 yawE;
    Vec3 pitchE;
    CamVec move;
    CamVec move2;
    s32 mx;
    s32 my;
    s32 buttons;
    int free;
    f32 turn;
    f32 dt;
    f32 speed;
    f32 h;
    CamGate gate;

    fn_80127A84(&mx, &my, &buttons);
    gate = (CamGate)lbl_8055F4E8;
    free = 1;
    if (gate && !gate(mx, my)) {
        free = 0;
    }
    dt = MinF((f32)(s32)(fn_802E2C20() - lbl_8055F4FC) / 1000.0f, 0.033333335f);
    lbl_8055F4FC = fn_802E2C20();
    right = lbl_80499050.rot.axes[0];
    right.y = 0.0f;
    fwd = lbl_80499050.rot.axes[2];
    fwd.y = 0.0f;
    if (ReadJoyStatus(&pad, 0)) {
        turn = 3.1415927f;
        f32 t = CamUnit(pad.y3, -1.0f, 1.0f);
        speed = CamSpeed(t, 1.0f);
        ReadFlyPad(pad, dt, turn, speed, in, move);
    }
    {
        f32 rate = 100.0f;
        f32 side = 0.0f;
        f32 vert = side;
        if (fn_802E4A1C(0x1E)) {
            vert = 1.0f;
        } else if (fn_802E4A1C(0x2C)) {
            vert = -1.0f;
        }
        in2.x = dt * (side * rate);
        in2.y = 0.0f;
        in2.z = dt * (vert * rate);
        const CamRot* rot2 = &lbl_80499050.rot;
        CamMulVec(&move2, &in2, rot2);
        lbl_80499050.pos.x += move2.x;
        lbl_80499050.pos.y += move2.y;
        lbl_80499050.pos.z += move2.z;
    }
    h = lbl_80499050.pos.y - fn_801EA0E8(lbl_80499050.pos.x, lbl_80499050.pos.z);
    if (free) {
        f32 t = MaxF(MinF(1.0f, (h - 20.0f) / 480.0f), 0.0f);
        f32 px = 0.0f;
        f32 py = 0.0f;
        f32 pan = 50.0f * CamBlend(t, 19.0f);
        if (mx < 0x50) {
            px = -1.0f + (f32)mx / 80.0f;
        } else if (mx > lbl_8055CFF0 - 0x50) {
            px = (f32)(mx - (lbl_8055CFF0 - 0x50)) / 80.0f;
        }
        if (my < 0x3C) {
            py = -1.0f + (f32)my / 60.0f;
        } else if (my > lbl_8055CFF4 - 0x3C) {
            py = (f32)(my - (lbl_8055CFF4 - 0x3C)) / 60.0f;
        }
        lbl_80499050.pos.x += right.x * (dt * (px * pan));
        lbl_80499050.pos.y += right.y * (dt * (px * pan));
        lbl_80499050.pos.z += right.z * (dt * (px * pan));
        lbl_80499050.pos.x += fwd.x * (dt * (-py * pan));
        lbl_80499050.pos.y += fwd.y * (dt * (-py * pan));
        lbl_80499050.pos.z += fwd.z * (dt * (-py * pan));
    }
    lbl_80499050.pos.y += h - (lbl_80499050.pos.y - fn_801EA0E8(lbl_80499050.pos.x, lbl_80499050.pos.z));
    // The rotation is reset and rebuilt through a Mat3 view. The cast is retail evidence (aids audit 2, 2026-11-16,
    // on the EXACT form): the identity stores keep one resolved base only when they go through a pointer variable
    // obtained by a cast (this view, rot->m[r][c] stores, or a cast pointer inside a helper: all exact); every
    // cast-free form (rot.axes[i] in place, .v[i], CamRot*/CamFrame*/CamVec* locals, CamRot& helper) and the unnamed
    // cast ((Mat3*)&...)->SetScale miss by 6 rows. Same mechanism as the SoundManager ctor. SetScale(1.0f) = identity.
    Mat3* rot = (Mat3*)&lbl_80499050.rot;
    rot->SetScale(1.0f);
    yawE.Set(0.0f, lbl_8055F4F4, 0.0f);
    Mat3FromEuler(&yawM, &yawE);
    Mat3Mul(rot, &yawM, rot);
    pitchE.Set(-lbl_8055F4F8, 0.0f, 0.0f);
    Mat3FromEuler(&pitchM, &pitchE);
    Mat3Mul(rot, &pitchM, rot);
}

BOOL fn_8026BFF0(s32 x) {
    switch (x) { case 3: case 4: return TRUE; default: return FALSE; }
}



typedef struct CamQuat {
    f32 x, y, z, w;
} CamQuat;
// The blimp camera: a fixed offset behind and above the car it follows. Its axes and offset are laid out as a
// CamFrame (the stick modes fly it like one).
typedef struct BlimpCam {
    CamVec side;     // offset 0x0
    CamVec up;       // offset 0xC
    CamVec dir;      // offset 0x18
    CamVec offset;   // offset 0x24
    CamQuat rot;     // offset 0x30
    CamVec pos;      // offset 0x40
    s16 hint;        // offset 0x4C
    s32 car;         // offset 0x50
    u8 pad54[4];
} BlimpCam;

BlimpCam lbl_8049A168;
// The applied frame and the keyboard camera's motion: three separate objects (with the blimp camera the unit's
// functions reference three or more, so they share one base register as in the original).
CamFrame lbl_8049A1C0;
CamVec lbl_8049A1F0;  // velocity
CamVec lbl_8049A1FC;  // spin

double sqrt(double x);
void fn_802E30C8(const char* fmt, ...);
void fn_801E9DC4(s16* hint);

extern s32 lbl_8055F4EC;        // the debug camera's control mode (0 keyboard, 1 pad, 2 free camera)
extern u8 lbl_8055F500;         // the mode button last frame
extern s8 lbl_8055F501;         // keyboard velocity cleared
extern s8 lbl_8055F502;         // keyboard spin cleared
extern s32 lbl_8055F504;        // the keyboard camera's last update time (ms)
extern u32 lbl_8055F508;        // the orbit camera's last update time (ms)
extern f32 lbl_8055F50C;        // its yaw
extern f32 lbl_8055F510;        // its pitch
extern u32 lbl_8055F514;        // the pad camera's last update time (ms)
int fn_802D3EBC(void);
int fn_80137CFC(int player);
s32* fn_801297F4(void);
int fn_802E4A60(FreePad* out);
void fn_8012F57C(CamQuat* out, CamQuat* a, CamQuat* b, f32 t);
f32 fn_801E9DDC(s16* hint, f32 x, f32 z);
void fn_802E317C(const char* fmt, ...);

static __inline void MtxToQuatAxis(f32 (*m)[3], CamQuat* q) {
    f32* v = &q->x;
    int i = 0;
    int j;
    int k;
    f32 s;
    if (m[1][1] > m[0][0]) {
        i = 1;
    }
    if (m[2][2] > m[i][i]) {
        i = 2;
    }
    j = (i + 1) % 3;
    k = (j + 1) % 3;
    s = sqrt(1.0f + (m[i][i] - (m[j][j] + m[k][k])));
    v[i] = 0.5f * s;
    s = 0.5f / s;
    v[3] = s * (m[j][k] - m[k][j]);
    v[j] = s * (m[i][j] + m[j][i]);
    v[k] = s * (m[i][k] + m[k][i]);
}

static __inline void MtxToQuat(const CamFrame* f, CamQuat* q) {
    const f32(*m)[3] = (const f32(*)[3])f;
    f32 tr = f->rot.axes[2].z + (f->rot.axes[0].x + f->rot.axes[1].y);
    if (tr > 0.0f) {
        f32 s = sqrt(1.0f + tr);
        q->w = 0.5f * s;
        s = 0.5f / s;
        q->x = s * (m[1][2] - m[2][1]);
        q->y = s * (m[2][0] - m[0][2]);
        q->z = s * (m[0][1] - m[1][0]);
    } else {
        MtxToQuatAxis((f32(*)[3])m, q);
    }
}

/* Helper name unproven: MtxToQuat with the axis branch written out (the debug camera's copy: its quaternion
 address numbers with the frame pointer; fn_8026DDD8's copy keeps the nested axis helper). */
static __inline void FrameToQuat(const CamFrame* f, CamQuat* q) {
    const f32(*m)[3] = (const f32(*)[3])f;
    f32 tr = f->rot.axes[2].z + (f->rot.axes[0].x + f->rot.axes[1].y);
    if (tr > 0.0f) {
        f32 s = sqrt(1.0f + tr);
        q->w = 0.5f * s;
        s = 0.5f / s;
        q->x = s * (m[1][2] - m[2][1]);
        q->y = s * (m[2][0] - m[0][2]);
        q->z = s * (m[0][1] - m[1][0]);
    } else {
        f32* v = &q->x;
        int i = 0;
        int j;
        int k;
        f32 s;
        if (m[1][1] > m[0][0]) {
            i = 1;
        }
        if (m[2][2] > m[i][i]) {
            i = 2;
        }
        j = (i + 1) % 3;
        k = (j + 1) % 3;
        s = sqrt(1.0f + (m[i][i] - (m[j][j] + m[k][k])));
        v[i] = 0.5f * s;
        s = 0.5f / s;
        v[3] = s * (m[j][k] - m[k][j]);
        v[j] = s * (m[i][j] + m[j][i]);
        v[k] = s * (m[i][k] + m[k][i]);
    }
}

static __inline void QuatToMtx(const CamQuat* q, CamRot* r) {
    f32 x2 = q->x + q->x;
    f32 y2 = q->y + q->y;
    f32 z2 = q->z + q->z;
    f32 xx = q->x * x2;
    f32 yy = q->y * y2;
    f32 zz = q->z * z2;
    f32 wx = q->w * x2;
    f32 yz = q->y * z2;
    f32 wy = q->w * y2;
    f32 xz = q->x * z2;
    f32 wz = q->w * z2;
    f32 xy = q->x * y2;
    r->axes[0].x = 1.0f - (yy + zz);
    r->axes[1].x = xy - wz;
    r->axes[2].x = xz + wy;
    r->axes[0].y = xy + wz;
    r->axes[1].y = 1.0f - (xx + zz);
    r->axes[2].y = yz - wx;
    r->axes[0].z = xz - wy;
    r->axes[1].z = yz + wx;
    r->axes[2].z = 1.0f - (xx + yy);
}

/* Helper name unproven */
static __inline void QuatCopy(CamQuat* d, const CamQuat* s) {
    d->x = s->x;
    d->y = s->y;
    d->z = s->z;
    d->w = s->w;
}
static __inline void CamSet(CamVec* v, f32 x, f32 y, f32 z) {
    v->x = x;
    v->y = y;
    v->z = z;
}
static __inline void CamScl(CamVec* v, f32 k) {
    v->x *= k;
    v->y *= k;
    v->z *= k;
}
static __inline void CamCopy(CamVec* d, const CamVec* s) {
    d->x = s->x;
    d->y = s->y;
    d->z = s->z;
}
static __inline int CamIsZero(const CamVec* v) {
    return 0.0f == v->x && 0.0f == v->y && 0.0f == v->z;
}
static __inline void CamIdentity(CamRot* r) {
    r->axes[0].x = 1.0f;
    r->axes[0].y = 0.0f;
    r->axes[0].z = 0.0f;
    r->axes[1].x = 0.0f;
    r->axes[1].y = 1.0f;
    r->axes[1].z = 0.0f;
    r->axes[2].x = 0.0f;
    r->axes[2].y = 0.0f;
    r->axes[2].z = 1.0f;
}
// Moves a frame by a vector in its own axes.
static __inline void CamMove(CamFrame* f, const CamVec* local) {
    CamVec w;
    const CamRot* rot = &f->rot;
    CamMulVec(&w, local, rot);
    f->pos.x = f->pos.x + w.x;
    f->pos.y = f->pos.y + w.y;
    f->pos.z = f->pos.z + w.z;
}
// The stick fly modes: turn by the left stick (roll from the d-pad), move by the right stick.
static __inline void CamFly(CamFrame* f, const FreePad* pad, f32 turn, f32 roll, f32 speed, f32 lift) {
    CamRot m;
    CamVec e;
    CamVec v;
    CamSet(&e, turn * -fn_8026B844(pad->y0), turn * fn_8026B844(pad->x0), roll * turn);
    Mat3FromEuler((Mat3*)&m, (const Vec3*)&e);
    Mat3Mul((Mat3*)&f->rot, (const Mat3*)&m, (const Mat3*)&f->rot);
    CamSet(&v, pad->y1 * speed, 0.1f * (lift * speed), -pad->x1 * speed);
    CamMove(f, &v);
}

#define CAM_MAX(a, b) ((a) > (b) ? (a) : (b))

// The debug camera, once a frame: the mode button cycles keyboard / pad / free camera control of the saved
// frame (the pad modes move whichever camera is current), then the replay cameras follow their car.
/* Helper names unproven: one keyboard axis of the free camera from two key bits. The forward/back axis tests its
   plus key first, the strafe axis its minus key first (retail's order of the two tests). */
static inline f32 KeyAxis(FreePad& pad, f32 v, f32 speed, int plus, int minus) {
    if (pad.w40 & plus) {
        v = speed;
    } else if (pad.w40 & minus) {
        v = -speed;
    }
    return v;
}
static inline f32 KeyAxisRev(FreePad& pad, f32 v, f32 speed, int minus, int plus) {
    if (pad.w40 & minus) {
        v = -speed;
    } else if (pad.w40 & plus) {
        v = speed;
    }
    return v;
}

/* Helper name unproven */
static __inline void CamOrthonormal(CamRot& d) {
    CamVec a0;
    CamVec a1;
    CamVec a2;
    CamSet(&a0, d.axes[0].x, d.axes[0].y, d.axes[0].z);
    CamSet(&a2, d.axes[2].x, d.axes[2].y, d.axes[2].z);
    PSVECCrossProduct((const Vec*)&a2, (const Vec*)&a0, (Vec*)&a1);
    PSVECNormalize((const Vec*)&a0, (Vec*)&a0);
    PSVECNormalize((const Vec*)&a1, (Vec*)&a1);
    PSVECCrossProduct((const Vec*)&a0, (const Vec*)&a1, (Vec*)&a2);
    CamCopy(&d.axes[0], &a0);
    CamCopy(&d.axes[1], &a1);
    CamCopy(&d.axes[2], &a2);
}

/* Helper name unproven */
static __inline void CamAddRotated(CamVec& pos, const CamVec& v, const CamRot* rot) {
    CamVec w;
    CamMulVec(&w, &v, rot);
    pos.x = pos.x + w.x;
    pos.y = pos.y + w.y;
    pos.z = pos.z + w.z;
}

/* Helper name unproven: a 4-float vector (retail keeps 16 bytes for the follow camera's up vector) */
struct CamVec4 : CamVec {
    f32 w;
};

/* Helper name unproven: a row of four rates */
struct Rate4 {
    f32 v[4];
};

static const Rate4 mr0 = {{1.0f, 0.1f, 10.0f, 0.01f}};
static const Rate4 tr0 = {{0.05f, 0.0125f, 0.15f, 0.0025f}};

void fn_8026C014(int player) {
    FreePad pad;
    FreePad keys;
    CamFrame tgt;
    CamVec move;
    CamVec spin;
    Rate4 moveRate;
    Rate4 turnRate;
    CamVec4 up;
    CamQuat q;
    CamQuat qs;
    if (fn_802D3EBC()) {
        return;
    }
    if (ReadJoyStatus(&pad, player ? fn_80137CFC(0) : 0)) {
        s32 b = fn_801297F4()[10];
        if (b >= 0) {
            u8 down = pad.keys[b];
            if (down && !lbl_8055F500) {
                lbl_8055F4EC = (lbl_8055F4EC + 1) % 3;
            }
            lbl_8055F500 = down;
        }
    }
    if (fn_802E4A60(&keys)) {
        pad = keys;
    }
    if (lbl_8055F4EC == 0) {
        lbl_8055F4EC = (lbl_8055F4EC + 1) % 3;
    }
    if (lbl_8055F4EC == 2) {
        fn_8026B8A0();
        return;
    }
    if (lbl_8055F4EC == 0) {
        CamRot m;
        f32 dt;
        int shift;
        int ctrl;
        f32 speed;
        f32 turn;
        move.x = 0.0f;
        move.y = 0.0f;
        move.z = 0.0f;
        spin.x = 0.0f;
        spin.y = 0.0f;
        spin.z = 0.0f;
        if (!lbl_8055F501) {
            CamSet(&lbl_8049A1F0, 0.0f, 0.0f, 0.0f);
            lbl_8055F501 = 1;
        }
        if (!lbl_8055F502) {
            CamSet(&lbl_8049A1FC, 0.0f, 0.0f, 0.0f);
            lbl_8055F502 = 1;
        }
        dt = MinF((f32)(s32)(fn_802E2C20() - lbl_8055F504) / 1000.0f, 0.033333335f);
        lbl_8055F504 = fn_802E2C20();
        CamScl(&lbl_8049A1F0, 0.7f);
        CamScl(&lbl_8049A1FC, 0.7f);
        shift = fn_802E4A1C(0xFD);
        ctrl = fn_802E4A1C(0xFE) || fn_802E4A1C(0x3A);
        {
            moveRate = mr0;
            turnRate = tr0;
            f32 frames = dt / 0.033333335f;
            speed = frames * moveRate.v[(shift << 1) | ctrl];
            turn = frames * turnRate.v[(shift << 1) | ctrl];
        }
        if (fn_802E4A1C(0x1E)) {
            move.z = speed;
        } else if (fn_802E4A1C(0x2C)) {
            move.z = -speed;
        }
        if (fn_802E4A1C(0x51)) {
            move.x = speed;
        } else if (fn_802E4A1C(0x4F)) {
            move.x = -speed;
        }
        if (fn_802E4A1C(0x53)) {
            move.y = Mul(speed, 0.5f);
        } else if (fn_802E4A1C(0x52)) {
            move.y = Mul(-speed, 0.5f);
        }
        if (fn_802E4A1C(0x4D)) {
            if (fn_802E4A1C(0x4F)) {
                spin.y = Mul(turn, 0.5f);
            } else {
                spin.y = turn;
            }
        } else if (fn_802E4A1C(0x4B)) {
            if (fn_802E4A1C(0x51)) {
                spin.y = Mul(-turn, 0.5f);
            } else {
                spin.y = -turn;
            }
        }
        if (fn_802E4A1C(0x48)) {
            spin.x = Mul(turn, 0.5f);
        } else if (fn_802E4A1C(0x50)) {
            spin.x = Mul(-turn, 0.5f);
        }
        if (fn_802E4A1C(0x47)) {
            spin.z = turn;
        } else if (fn_802E4A1C(0x49)) {
            spin.z = -turn;
        }
        {
            f32 step = 0.004363323f;
            f32 lo = 0.2617994f;
            f32 hi = 2.443461f;
            if (fn_802E4A1C(0x37)) {
                lbl_8055F47C += step;
            } else if (fn_802E4A1C(0xB5)) {
                lbl_8055F47C -= step;
            }
            lbl_8055F47C = MaxF(MinF(hi, lbl_8055F47C), lo);
        }
        if (!CamIsZero(&spin)) {
            CamCopy(&lbl_8049A1FC, &spin);
        }
        if (!CamIsZero(&move)) {
            CamCopy(&lbl_8049A1F0, &move);
        }
        const CamRot* rot = &lbl_80499050.rot;
        CamAddRotated(lbl_80499050.pos, lbl_8049A1F0, rot);
        Mat3FromEuler((Mat3*)&m, (const Vec3*)&lbl_8049A1FC);
        Mat3Mul((Mat3*)&lbl_80499050.rot, (const Mat3*)&m, (const Mat3*)&lbl_80499050.rot);
    } else if (pad.w44[3] & 0x10) {
        // Orbit: the left stick turns about the vertical and tilts, the d-pad moves.
        const f32 yawUnit = 0.5235988f;
        CamRot yawM;
        CamRot pitchM;
        CamVec yawE;
        CamVec pitchE;
        CamVec v;
        u32 now = fn_802E2C20();
        u32 last = lbl_8055F508;
        f32 dt;
        f32 maxStep = 0.1f;
        f32 speed;
        f32 yawRate;
        f32 fovRate;
        f32 hi;
        f32 lo;
        f32 yaw;
        f32 s;
        f32 target;
        f32 x;
        f32 z;
        lbl_8055F508 = now;
        dt = CAM_MAX(0.001f * (f32)(now - last), maxStep);
        yawRate = dt * yawUnit;
        speed = Mul(4.0f, Mul(0.25f, Mul(10.0f, dt)));
        fovRate = Mul(dt, 0.034906585f);
        lo = 0.2617994f;
        hi = 2.443461f;
        if (pad.keys[4]) {
            f32 dir;
            speed = 0.0f;
            dir = speed;
            if (pad.w40 & 1) {
                dir = -1.0f;
            }
            if (pad.w40 & 4) {
                dir = 1.0f;
            }
            lbl_8055F47C = MaxF(MinF(hi, lbl_8055F47C + dir * fovRate), lo);
        } else if (pad.keys[5]) {
            speed *= 0.2f;
        } else if (pad.keys[2]) {
            speed *= 5.0f;
        }
        yaw = lbl_8055F50C;
        if (pad.x0 > 0.0f) {
            s = MaxF(MinF(1.0f, 1.2f * pad.x0 - 0.2f), 0.0f);
        } else {
            s = MaxF(MinF(0.0f, 0.2f + 1.2f * pad.x0), -1.0f);
        }
        yaw += yawRate * s;
        target = Mul(pad.y0, 1.0471976f);
        lbl_8055F510 = target + 0.95f * (lbl_8055F510 - target);
        {
            CamVec* b = (CamVec*)&lbl_80499050;
            b[0].x = 1.0f;
            b[0].y = 0.0f;
            b[0].z = 0.0f;
            b[1].x = 0.0f;
            b[1].y = 1.0f;
            b[1].z = 0.0f;
            b[2].x = 0.0f;
            b[2].y = 0.0f;
            b[2].z = 1.0f;
        }
        CamSet(&yawE, 0.0f, yaw, 0.0f);
        Mat3FromEuler((Mat3*)&yawM, (const Vec3*)&yawE);
        Mat3Mul((Mat3*)&lbl_80499050.rot, (const Mat3*)&yawM, (const Mat3*)&lbl_80499050.rot);
        CamSet(&pitchE, -lbl_8055F510, 0.0f, 0.0f);
        Mat3FromEuler((Mat3*)&pitchM, (const Vec3*)&pitchE);
        Mat3Mul((Mat3*)&lbl_80499050.rot, (const Mat3*)&pitchM, (const Mat3*)&lbl_80499050.rot);
        z = 0.0f;
        x = z;
        z = KeyAxis(pad, z, speed, 1, 4);
        x = KeyAxisRev(pad, x, speed, 8, 2);
        CamSet(&v, x, 0.0f, z);
        CamMove(&lbl_80499050, &v);
        lbl_8055F50C = yaw;
    } else {
        // Stick flight of the current camera; the trigger and a button boost it.
        u32 now = fn_802E2C20();
        u32 last = lbl_8055F514;
        f32 dt;
        f32 fovRate;
        f32 speed;
        f32 turn;
        f32 hi;
        f32 lo;
        f32 t;
        f32 boost;
        f32 roll;
        f32 lift;
        lbl_8055F514 = now;
        dt = CAM_MAX(0.001f * (f32)(now - last), 0.1f);
        turn = dt * 0.5235988f;
        speed = 0.25f * (10.0f * dt);
        fovRate = Mul(dt, 0.034906585f);
        lo = 0.2617994f;
        hi = 2.443461f;
        t = CamUnit(pad.y3, -1.0f, 1.0f);
        t = CamBlend(t, 5.0f);
        boost = 1.0f;
        if (pad.keys[6]) {
            boost = 4.0f;
        }
        speed = speed * (boost * t);
        roll = 0.0f;
        if (pad.w40 & 8) {
            roll = speed;
        } else if (pad.w40 & 2) {
            roll = Mul(-1.0f, speed);
        }
        lift = 0.0f;
        if (pad.w40 & 1) {
            lift = speed;
        }
        if (pad.w40 & 4) {
            lift = Mul(-1.0f, speed);
        }
        if (pad.keys[5]) {
            lbl_8055F47C = MaxF(MinF(hi, lbl_8055F47C + pad.y0 * fovRate), lo);
        } else {
            switch ((s32)lbl_8055F4E0) {
            case 0:
            case 4:
                {
                    CamRot m;
                    CamVec e;
                    CamVec v;
                    CamSet(&e, turn * -fn_8026B844(pad.y0), turn * fn_8026B844(pad.x0), roll * turn);
                    Mat3FromEuler((Mat3*)&m, (const Vec3*)&e);
                    Mat3Mul((Mat3*)&(&lbl_80499050)->rot, (const Mat3*)&m, (const Mat3*)&(&lbl_80499050)->rot);
                    CamSet(&v, pad.y1 * speed, 0.1f * (lift * speed), -pad.x1 * speed);
                    CamMove((&lbl_80499050), &v);
                }
                break;
            case 1: {
                CamVec v;
                CamVec v2;
                CamSet(&v, 0.0f, speed * -fn_8026B844(pad.y0), 0.0f);
                CamMove(&lbl_80499050, &v);
                CamSet(&v2, pad.y1 * speed, 0.1f * (lift * speed), -pad.x1 * speed);
                CamMove(&lbl_80499050, &v2);
                break;
            }
            case 3: {
                CamFrame* f = &lbl_8049A1C0;
                f32 s = speed * fn_8026B844(pad.y1);
                lbl_80499050.pos.x += f->rot.axes[0].x * s;
                lbl_80499050.pos.y += f->rot.axes[0].y * s;
                lbl_80499050.pos.z += f->rot.axes[0].z * s;
                lbl_80499050.pos.x += f->rot.axes[1].x * (0.1f * (lift * speed));
                lbl_80499050.pos.y += f->rot.axes[1].y * (0.1f * (lift * speed));
                lbl_80499050.pos.z += f->rot.axes[1].z * (0.1f * (lift * speed));
                s = speed * -fn_8026B844(pad.x1);
                lbl_80499050.pos.x += f->rot.axes[2].x * s;
                lbl_80499050.pos.y += f->rot.axes[2].y * s;
                lbl_80499050.pos.z += f->rot.axes[2].z * s;
                break;
            }
            case 2:
                {
                    CamRot m;
                    CamVec e;
                    CamVec v;
                    CamSet(&e, turn * -fn_8026B844(pad.y0), turn * fn_8026B844(pad.x0), roll * turn);
                    Mat3FromEuler((Mat3*)&m, (const Vec3*)&e);
                    Mat3Mul((Mat3*)&((CamFrame*)&lbl_8049A168)->rot, (const Mat3*)&m, (const Mat3*)&((CamFrame*)&lbl_8049A168)->rot);
                    CamSet(&v, pad.y1 * speed, 0.1f * (lift * speed), -pad.x1 * speed);
                    CamMove(((CamFrame*)&lbl_8049A168), &v);
                }
                break;
            default:
                fn_802E317C("blimpcam: invalid mode %d", lbl_8055F4E0);
                break;
            }
        }
    }
    if ((s32)lbl_8055F4E0 == 1 && player) {
        CamCar* car = fn_8017960C(fn_8026A344(0));
        CamFrame* f;
        if (!car) {
            return;
        }
        f = &lbl_80499050;
        f->rot.axes[2].x = car->frame.pos.x - f->pos.x;
        f->rot.axes[2].y = car->frame.pos.y - f->pos.y;
        f->rot.axes[2].z = car->frame.pos.z - f->pos.z;
        CamSet(&up, 0.0f, 1.0f, 0.0f);
        f32 xx = f->rot.axes[2].x * f->rot.axes[2].x;
        f32 zz = f->rot.axes[2].z * f->rot.axes[2].z;
        if (xx + zz > 0.01f) {
            PSVECCrossProduct((const Vec*)&up, (const Vec*)&f->rot.axes[2], (Vec*)&f->rot.axes[0]);
            PSVECCrossProduct((const Vec*)&f->rot.axes[2], (const Vec*)&f->rot.axes[0], (Vec*)&f->rot.axes[1]);
            CamOrthonormal(f->rot);
        }
    } else if ((s32)lbl_8055F4E0 == 2 && player) {
        s32 id = fn_8026A344(0);
        CamCar* car = fn_8017960C(id);
        if (car && ((s32)lbl_8055F4E4 & 4)) {
            tgt = car->frame;
        } else if (car) {
            FrameToQuat(&car->frame, &q);
            if (id == lbl_8049A168.car) {
                CamVec p;

                fn_8012F57C(&qs, &lbl_8049A168.rot, &q, 0.05f);
                if (!((s32)lbl_8055F4E4 & 2)) {
                    QuatToMtx(&qs, &tgt.rot);
                } else {
                    CamIdentity(&tgt.rot);
                }
                f32 deltaz = car->frame.pos.z - lbl_8049A168.pos.z;
                f32 deltay = car->frame.pos.y - lbl_8049A168.pos.y;
                f32 deltax = car->frame.pos.x - lbl_8049A168.pos.x;
                tgt.pos.z = lbl_8049A168.pos.z + deltaz / 2.0f;
                tgt.pos.y = lbl_8049A168.pos.y + 0.5f * deltay;
                tgt.pos.x = lbl_8049A168.pos.x + 0.5f * deltax;
                p.x = tgt.pos.x;
                p.y = tgt.pos.y;
                p.z = tgt.pos.z;
                QuatCopy(&lbl_8049A168.rot, &qs);
                CamCopy(&lbl_8049A168.pos, &p);
            } else {
                tgt = car->frame;
                lbl_8049A168.rot = q;
                lbl_8049A168.pos.x = car->frame.pos.x;
                lbl_8049A168.pos.y = car->frame.pos.y;
                lbl_8049A168.pos.z = car->frame.pos.z;
            }
        } else {
            CamIdentity(&tgt.rot);
            tgt.pos.x = 0.0f;
            tgt.pos.y = 0.0f;
            tgt.pos.z = 0.0f;
            lbl_8049A168.rot.x = 0.0f;
            lbl_8049A168.rot.y = 0.0f;
            lbl_8049A168.rot.z = 0.0f;
            lbl_8049A168.rot.w = 1.0f;
            lbl_8049A168.pos.x = 0.0f;
            lbl_8049A168.pos.y = 0.0f;
            lbl_8049A168.pos.z = 0.0f;
        }
        fn_801BFD34(&lbl_80499050.pos, &lbl_8049A168.offset, &tgt);
        Mat3Mul((Mat3*)&lbl_80499050.rot, (const Mat3*)(CamRot*)&lbl_8049A168, (const Mat3*)&tgt.rot);
        if ((s32)lbl_8055F4E4 & 1) {
            f32 floor = 1.0f + fn_801E9DDC(&lbl_8049A168.hint, lbl_80499050.pos.x, lbl_80499050.pos.z);
            lbl_80499050.pos.y = MaxF(lbl_80499050.pos.y, floor);
        }
        lbl_8049A168.car = id;
    } else if ((s32)lbl_8055F4E0 == 3 && player && lbl_8055F4F0 >= 0) {
        lbl_8049A168.car = fn_8026A344(0);
    }
}

static __inline void CopyFrame(CamFrame* d, const CamFrame* s) {
    d->rot = s->rot;
    d->pos = s->pos;
}

void fn_8026DAA8(CamView* c) {
    if (c->track && !fn_8026BFF0((s32)lbl_8055F4E0)) {
        CopyFrame(&lbl_80499050, &c->frame);
        lbl_8055F47C = c->trackFov;
    }
    switch ((s32)lbl_8055F4E0) {
    case 3:
        if (lbl_8055F4F0 >= 0) {
            s32 idx;
            CamCar* a;
            CamCar* b;
            CamFrame f;
            CamVec up;
            idx = fn_8026A344(0);
            a = fn_8017960C(lbl_8055F4F0);
            b = fn_8017960C(idx);
            CopyFrame(&f, &a->frame);
            f.pos.x += lbl_80499050.pos.x;
            f.pos.y += lbl_80499050.pos.y;
            f.pos.z += lbl_80499050.pos.z;
            up.x = 0.0f;
            up.y = 1.0f;
            up.z = 0.0f;
            CalculateCameraMatrix(&c->frame, &f.pos, &b->frame.pos, &up);
        } else {
            c->frame.rot.axes[0].x = 1.0f;
            c->frame.rot.axes[0].y = 0.0f;
            c->frame.rot.axes[0].z = 0.0f;
            c->frame.rot.axes[1].x = 0.0f;
            c->frame.rot.axes[1].y = 1.0f;
            c->frame.rot.axes[1].z = 0.0f;
            c->frame.rot.axes[2].x = 0.0f;
            c->frame.rot.axes[2].y = 0.0f;
            c->frame.rot.axes[2].z = 1.0f;
            c->frame.pos.x = 0.0f;
            c->frame.pos.y = 0.0f;
            c->frame.pos.z = 0.0f;
        }
        break;
    case 4: {
        CamFrame* car = &fn_8017960C(fn_8026A344(0))->frame;
        fn_801BFD34(&c->frame.pos, &lbl_80499050.pos, car);
        Mat3Mul((Mat3*)&c->frame.rot, (const Mat3*)&lbl_80499050.rot, (const Mat3*)&car->rot);
        break;
    }
    default:
        CopyFrame(&c->frame, &lbl_80499050);
        break;
    }
    c->fov = lbl_8055F47C;
    c->shake[0] = 0.0f;
    c->shake[1] = 0.0f;
    c->shake[2] = 0.0f;
    CopyFrame(&lbl_8049A1C0, &c->frame);
}

void* fn_8026DDD0(void) {
    return lbl_8055F4E0;
}

// Helper unproven: o = v * s. Retail multiplies the stored offset by -1.0 at run time (literals would fold).
static inline void CamScaleTo(CamVec* o, const CamVec* v, f32 s) { o->x = v->x * s; o->y = v->y * s; o->z = v->z * s; }
void fn_8026DDD8(s32 mode) {
    if (mode == 3 || (s32)lbl_8055F4E0 == mode) {
        s32 first = fn_8026A344(0);
        if (!fn_8017960C(++lbl_8055F4F0)) {
            lbl_8055F4F0 = 0;
        }
        if (lbl_8055F4F0 == first) {
            if (!fn_8017960C(++lbl_8055F4F0)) {
                lbl_8055F4F0 = 0;
            }
        }
        fn_802E30C8("blimpcam: car %d look at car %d", lbl_8055F4F0, first);
    }
    if ((s32)lbl_8055F4E0 == mode) {
        return;
    }
    if (mode == 2) {
        CamCar* car;
        lbl_8049A168.offset.x = 0.0f;
        lbl_8049A168.offset.y = 5.0f;
        lbl_8049A168.offset.z = -10.0f;
        lbl_8049A168.side.x = 1.0f;
        lbl_8049A168.side.y = 0.0f;
        lbl_8049A168.side.z = 0.0f;
        CamScaleTo(&lbl_8049A168.dir, &lbl_8049A168.offset, -1.0f);
        PSVECNormalize((const Vec*)&lbl_8049A168.dir, (Vec*)&lbl_8049A168.dir);
        PSVECCrossProduct((const Vec*)&lbl_8049A168.dir, (const Vec*)&lbl_8049A168.side, (Vec*)&lbl_8049A168.up);
        lbl_8049A168.car = fn_8026A344(0);
        car = fn_8017960C(lbl_8049A168.car);
        if (car) {
            const CamFrame* f = &car->frame;
            MtxToQuat(f, &lbl_8049A168.rot);
            lbl_8049A168.pos.x = car->frame.pos.x;
            lbl_8049A168.pos.y = car->frame.pos.y;
            lbl_8049A168.pos.z = car->frame.pos.z;
        } else {
            lbl_8049A168.rot.x = 0.0f;
            lbl_8049A168.rot.y = 0.0f;
            lbl_8049A168.rot.z = 0.0f;
            lbl_8049A168.rot.w = 1.0f;
            lbl_8049A168.pos.x = 0.0f;
            lbl_8049A168.pos.y = 0.0f;
            lbl_8049A168.pos.z = 0.0f;
        }
    } else if (fn_8026BFF0(mode) && !fn_8026BFF0((s32)lbl_8055F4E0)) {
        CamVec* b = (CamVec*)&lbl_80499050;
        b[0].x = 1.0f;
        b[0].y = 0.0f;
        b[0].z = 0.0f;
        b[1].x = 0.0f;
        b[1].y = 1.0f;
        b[1].z = 0.0f;
        b[2].x = 0.0f;
        b[2].y = 0.0f;
        b[2].z = 1.0f;
        b[3].x = 0.0f;
        b[3].y = 0.0f;
        b[3].z = 0.0f;
    } else if (!fn_8026BFF0(mode) && fn_8026BFF0((s32)lbl_8055F4E0)) {
        CamCar* car = fn_8017960C(fn_8026A344(0));
        if (car) {
            CopyFrame(&lbl_80499050, &car->frame);
        }
    }
    lbl_8055F4E0 = (void*)mode;
    fn_801E9DC4(&lbl_8049A168.hint);
}

void fn_8026E204(void* hi) {
    lbl_8055F4E4 = hi;
}

extern void *lbl_8055F4E4;
void* fn_8026E20C(void) {
    return lbl_8055F4E4;
}

void* fn_8026E214(void* value) {
    void* old = lbl_8055F4E8;
    lbl_8055F4E8 = value;
    return old;
}

extern void *lbl_8055F4E8;

}  // extern "C"

// .sbss owned by this unit (0x8055F4E0-0x8055F518), in address order.
void *lbl_8055F4E0;
void *lbl_8055F4E4;
void *lbl_8055F4E8;
s32 lbl_8055F4EC;
s32 lbl_8055F4F0;
f32 lbl_8055F4F4;
f32 lbl_8055F4F8;
u32 lbl_8055F4FC;
u8 lbl_8055F500;
s8 lbl_8055F501;
s8 lbl_8055F502;
s8 lbl_8055F503;
s32 lbl_8055F504;
u32 lbl_8055F508;
f32 lbl_8055F50C;
f32 lbl_8055F510;
u32 lbl_8055F514;
