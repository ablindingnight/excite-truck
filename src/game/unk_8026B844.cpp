// Code 0x8026B844-0x8026E224, imported verbatim from the upstream project (github.com/orangedude27/excite-truck,
// CC0; file src/auto/text_8026B840_.c), where its functions match exactly. Unit name is provisional.
// CLEANUP: automated-decompiler source with raw offsets and placeholder types; rewrite with real types
// before the PC port. Tricks are tagged MATCHING-AID.
// Compiled as C++ (2026-10-31): the original copies the pad struct member by member, which MWCC does only in C++.
// fn_8026C014 (the debug camera) is a draft, not yet matching; it reaches both .bss objects from one base register,
// which is still unexplained.
#include "types.h"

extern "C" {

extern void *lbl_8055F4E0;
extern void *lbl_8055F4E4;
extern void *lbl_8055F4E8;
extern const f32 lbl_80563D78;
extern const f32 lbl_80563D7C;
extern const f32 lbl_80563D80;
extern const f32 lbl_80563D84;

static __inline f32 CamMin(f32 a, f32 b) { return a < b ? a : b; }
static __inline f32 CamMax(f32 a, f32 b) { return a > b ? a : b; }

void fn_8026B840(void) {}

f32 fn_8026B844(f32 f) {
    f32 c = lbl_80563D78;
    if (f > c) {
        return CamMax(c, (f - lbl_80563D7C) / lbl_80563D80);
    }
    return CamMin(c, (lbl_80563D7C + f) / lbl_80563D84);
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
extern const f32 lbl_80563D94;       // 1.0f
s32 fn_8026A344(s32 i);
CamCar* fn_8017960C(s32 car);
void fn_8026E688(CamFrame* out, const CamVec* eye, const CamVec* target, const CamVec* up);
void fn_801BFD34(CamVec* out, const CamVec* v, const CamFrame* m);
void Mat3Mul__FP4Mat3PC4Mat3PC4Mat3(CamRot* out, const CamRot* a, const CamRot* b);

// The pad as fn_801298E8 fills it (achview's ViewPad, keybd's KeyState): sticks, the analog trigger at 0x1C,
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
extern const f32 lbl_80563D98;  // -1.0f
extern const f32 lbl_80563D9C;  // 0.5f
extern const f32 lbl_80563DA0;  // 5.0f
extern const f32 lbl_80563D94;  // 1.0f
extern const f32 lbl_80563D88;  // 1000.0f
extern const f32 lbl_80563D8C;  // 1/30 s
extern const f32 lbl_80563D90;  // PI
extern const f32 lbl_80563DA4;  // 100.0f
extern const f32 lbl_80563DA8;  // 2 PI
extern const f32 lbl_80563DAC;  // -2 PI
extern const f32 lbl_80563DB0;  // PI / 3
extern const f32 lbl_80563DB4;  // -PI / 3
extern const f32 lbl_80563DB8;  // 20.0f
extern const f32 lbl_80563DBC;  // 480.0f
extern const f32 lbl_80563DC0;  // 19.0f
extern const f32 lbl_80563DC4;  // 50.0f
extern const f32 lbl_80563DC8;  // 80.0f
extern const f32 lbl_80563DCC;  // 60.0f
void fn_80127A84(s32* x, s32* y, s32* buttons);
int fn_801298E8(FreePad* pad, int chan);
u32 fn_802E2C20(void);
int fn_802E4A1C(int key);
f32 fn_801EA0E8(f32 x, f32 z);
void Mat3FromEuler__FP4Mat3PC4Vec3(CamRot* out, const CamVec* euler);

static __inline f32 CamDot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }
static __inline void CamMulVec(CamVec* out, const CamVec* in, const CamRot* m) {
    if (out == in) {
        f32 x = CamDot3(in->x, m->axes[0].x, in->y, m->axes[1].x, in->z, m->axes[2].x);
        f32 y = CamDot3(in->x, m->axes[0].y, in->y, m->axes[1].y, in->z, m->axes[2].y);
        f32 z = CamDot3(in->x, m->axes[0].z, in->y, m->axes[1].z, in->z, m->axes[2].z);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = CamDot3(in->x, m->axes[0].x, in->y, m->axes[1].x, in->z, m->axes[2].x);
        out->y = CamDot3(in->x, m->axes[0].y, in->y, m->axes[1].y, in->z, m->axes[2].y);
        out->z = CamDot3(in->x, m->axes[0].z, in->y, m->axes[1].z, in->z, m->axes[2].z);
    }
}

// The free (debug) camera: stick yaw/pitch and movement, trigger boost, keys up/down, the pointer at a screen
// edge pans (faster the higher the camera is), and the camera keeps its height above the ground.
void fn_8026B8A0(void) {
    FreePad pad;
    CamRot yawM;
    CamRot pitchM;
    CamVec right;
    CamVec fwd;
    CamVec in;
    CamVec in2;
    CamVec yawE;
    CamVec pitchE;
    CamVec move;
    CamVec move2;
    s32 mx;
    s32 my;
    s32 buttons;
    int free;
    f32 dt;
    f32 h;
    CamGate gate;

    fn_80127A84(&mx, &my, &buttons);
    gate = (CamGate)lbl_8055F4E8;
    free = 1;
    if (gate && !gate(mx, my)) {
        free = 0;
    }
    dt = CamMin((f32)(s32)(fn_802E2C20() - lbl_8055F4FC) / lbl_80563D88, lbl_80563D8C);
    lbl_8055F4FC = fn_802E2C20();
    right = lbl_80499050.rot.axes[0];
    right.y = lbl_80563D78;
    fwd = lbl_80499050.rot.axes[2];
    fwd.y = lbl_80563D78;
    if (fn_801298E8(&pad, 0)) {
        f32 turn = lbl_80563D90;
        f32 t = CamMax(CamMin(lbl_80563D94, (pad.y3 - lbl_80563D98) * lbl_80563D9C), lbl_80563D78);
        f32 speed = lbl_80563D94 * (lbl_80563DA4 * (lbl_80563D94 + t * lbl_80563DA0));
        f32 x;
        f32 z;
        lbl_8055F4F4 += turn * (dt * fn_8026B844(pad.x0));
        if (lbl_8055F4F4 > lbl_80563DA8) {
            lbl_8055F4F4 -= lbl_80563DA8;
        } else if (lbl_8055F4F4 < lbl_80563DAC) {
            lbl_8055F4F4 += lbl_80563DA8;
        }
        lbl_8055F4F8 = CamMax(CamMin(lbl_80563DB0, lbl_8055F4F8 + turn * (dt * fn_8026B844(pad.y0))), lbl_80563DB4);
        z = speed * (dt * -fn_8026B844(pad.x1));
        x = speed * (dt * fn_8026B844(pad.y1));
        in.x = x;
        in.y = lbl_80563D78;
        in.z = z;
        CamMulVec(&move, &in, &lbl_80499050.rot);
        lbl_80499050.pos.x += move.x;
        lbl_80499050.pos.y += move.y;
        lbl_80499050.pos.z += move.z;
    }
    {
        f32 side = lbl_80563D78;
        f32 rate = lbl_80563DA4;
        f32 vert = side;
        if (fn_802E4A1C(0x1E)) {
            vert = lbl_80563D94;
        } else if (fn_802E4A1C(0x2C)) {
            vert = lbl_80563D98;
        }
        in2.x = dt * (side * rate);
        in2.y = lbl_80563D78;
        in2.z = dt * (vert * rate);
        CamMulVec(&move2, &in2, &lbl_80499050.rot);
        lbl_80499050.pos.x += move2.x;
        lbl_80499050.pos.y += move2.y;
        lbl_80499050.pos.z += move2.z;
    }
    h = lbl_80499050.pos.y - fn_801EA0E8(lbl_80499050.pos.x, lbl_80499050.pos.z);
    if (free) {
        f32 t = CamMax(CamMin(lbl_80563D94, (h - lbl_80563DB8) / lbl_80563DBC), lbl_80563D78);
        f32 pan = lbl_80563DC4 * (lbl_80563D94 + t * lbl_80563DC0);
        f32 px = lbl_80563D78;
        f32 py = px;
        f32 a;
        f32 b;
        if (mx < 0x50) {
            px = lbl_80563D98 + (f32)mx / lbl_80563DC8;
        } else if (mx > lbl_8055CFF0 - 0x50) {
            px = (f32)(mx - (lbl_8055CFF0 - 0x50)) / lbl_80563DC8;
        }
        if (my < 0x3C) {
            py = lbl_80563D98 + (f32)my / lbl_80563DCC;
        } else if (my > lbl_8055CFF4 - 0x3C) {
            py = (f32)(my - (lbl_8055CFF4 - 0x3C)) / lbl_80563DCC;
        }
        a = dt * (px * pan);
        b = dt * (-py * pan);
        lbl_80499050.pos.x = lbl_80499050.pos.x + right.x * a + fwd.x * b;
        lbl_80499050.pos.y = lbl_80499050.pos.y + right.y * a + fwd.y * b;
        lbl_80499050.pos.z = lbl_80499050.pos.z + right.z * a + fwd.z * b;
    }
    lbl_80499050.pos.y = lbl_80499050.pos.y + (h - (lbl_80499050.pos.y - fn_801EA0E8(lbl_80499050.pos.x, lbl_80499050.pos.z)));
    lbl_80499050.rot.axes[0].x = lbl_80563D94;
    lbl_80499050.rot.axes[0].y = lbl_80563D78;
    lbl_80499050.rot.axes[0].z = lbl_80563D78;
    lbl_80499050.rot.axes[1].x = lbl_80563D78;
    lbl_80499050.rot.axes[1].y = lbl_80563D94;
    lbl_80499050.rot.axes[1].z = lbl_80563D78;
    lbl_80499050.rot.axes[2].x = lbl_80563D78;
    lbl_80499050.rot.axes[2].y = lbl_80563D78;
    lbl_80499050.rot.axes[2].z = lbl_80563D94;
    yawE.x = lbl_80563D78;
    yawE.y = lbl_8055F4F4;
    yawE.z = lbl_80563D78;
    Mat3FromEuler__FP4Mat3PC4Vec3(&yawM, &yawE);
    Mat3Mul__FP4Mat3PC4Mat3PC4Mat3(&lbl_80499050.rot, &yawM, &lbl_80499050.rot);
    pitchE.x = -lbl_8055F4F8;
    pitchE.y = lbl_80563D78;
    pitchE.z = lbl_80563D78;
    Mat3FromEuler__FP4Mat3PC4Vec3(&pitchM, &pitchE);
    Mat3Mul__FP4Mat3PC4Mat3PC4Mat3(&lbl_80499050.rot, &pitchM, &lbl_80499050.rot);
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
// The applied frame and the keyboard camera's motion.
typedef struct CamBody {
    CamFrame frame;  // offset 0x0
    CamVec vel;      // offset 0x30
    CamVec spin;     // offset 0x3C
} CamBody;

BlimpCam lbl_8049A168;
CamBody lbl_8049A1C0;

extern const f32 lbl_80563D98;  // -1.0f
extern const f32 lbl_80563D9C;  // 0.5f
extern const f32 lbl_80563DA0;  // 5.0f
extern const f32 lbl_80563E18;  // -10.0f
double sqrt(double x);
void PSVECNormalize(const CamVec* in, CamVec* out);
void PSVECCrossProduct(const CamVec* a, const CamVec* b, CamVec* out);
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
extern const double lbl_80563DD0;
extern const f32 lbl_80563DD8;  // 0.7f
extern const f32 lbl_80563DDC;  // 0.25 degrees
extern const f32 lbl_80563DE0;  // 15 degrees
extern const f32 lbl_80563DE4;  // 140 degrees
extern const f32 lbl_80563DE8;  // 0.001f
extern const f32 lbl_80563DEC;  // 0.1f
extern const f32 lbl_80563DF0;  // 30 degrees
extern const f32 lbl_80563DF4;  // 4.0f
extern const f32 lbl_80563DF8;  // 0.25f
extern const f32 lbl_80563DFC;  // 10.0f
extern const f32 lbl_80563E00;  // 2 degrees
extern const f32 lbl_80563E04;  // 0.95f
extern const f32 lbl_80563E08;  // 0.01f
extern const f32 lbl_80563E0C;  // 0.05f
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
    s = sqrt(lbl_80563D94 + (m[i][i] - (m[j][j] + m[k][k])));
    v[i] = lbl_80563D9C * s;
    s = lbl_80563D9C / s;
    v[3] = s * (m[j][k] - m[k][j]);
    v[j] = s * (m[i][j] + m[j][i]);
    v[k] = s * (m[i][k] + m[k][i]);
}

static __inline void MtxToQuat(const CamFrame* f, CamQuat* q) {
    const f32(*m)[3] = (const f32(*)[3])f;
    f32 tr = f->rot.axes[2].z + (f->rot.axes[0].x + f->rot.axes[1].y);
    if (tr > lbl_80563D78) {
        f32 s = sqrt(lbl_80563D94 + tr);
        q->w = lbl_80563D9C * s;
        s = lbl_80563D9C / s;
        q->x = s * (m[1][2] - m[2][1]);
        q->y = s * (m[2][0] - m[0][2]);
        q->z = s * (m[0][1] - m[1][0]);
    } else {
        MtxToQuatAxis((f32(*)[3])f, q);
    }
}

static __inline void QuatToMtx(const CamQuat* q, CamRot* r) {
    f32 x2 = q->x + q->x;
    f32 y2 = q->y + q->y;
    f32 z2 = q->z + q->z;
    f32 xx = q->x * x2;
    f32 yy = q->y * y2;
    f32 zz = q->z * z2;
    f32 xy = q->x * y2;
    f32 xz = q->x * z2;
    f32 yz = q->y * z2;
    f32 wx = q->w * x2;
    f32 wy = q->w * y2;
    f32 wz = q->w * z2;
    r->axes[0].x = lbl_80563D94 - (yy + zz);
    r->axes[0].y = xy + wz;
    r->axes[0].z = xz - wy;
    r->axes[1].x = xy - wz;
    r->axes[1].y = lbl_80563D94 - (xx + zz);
    r->axes[1].z = yz + wx;
    r->axes[2].x = xz + wy;
    r->axes[2].y = yz - wx;
    r->axes[2].z = lbl_80563D94 - (xx + yy);
}

static __inline void CamSet(CamVec* v, f32 x, f32 y, f32 z) {
    v->x = x;
    v->y = y;
    v->z = z;
}
static __inline void CamScale(CamVec* v, f32 k) {
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
    return lbl_80563D78 == v->x && lbl_80563D78 == v->y && lbl_80563D78 == v->z;
}
static __inline void CamIdentity(CamRot* r) {
    r->axes[0].x = lbl_80563D94;
    r->axes[0].y = lbl_80563D78;
    r->axes[0].z = lbl_80563D78;
    r->axes[1].x = lbl_80563D78;
    r->axes[1].y = lbl_80563D94;
    r->axes[1].z = lbl_80563D78;
    r->axes[2].x = lbl_80563D78;
    r->axes[2].y = lbl_80563D78;
    r->axes[2].z = lbl_80563D94;
}
// Moves a frame by a vector in its own axes.
static __inline void CamMove(CamFrame* f, const CamVec* local) {
    CamVec w;
    CamMulVec(&w, local, &f->rot);
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
    Mat3FromEuler__FP4Mat3PC4Vec3(&m, &e);
    Mat3Mul__FP4Mat3PC4Mat3PC4Mat3(&f->rot, &m, &f->rot);
    CamSet(&v, pad->y1 * speed, lbl_80563DEC * (lift * speed), -pad->x1 * speed);
    CamMove(f, &v);
}

#define CAM_MAX(a, b) ((a) > (b) ? (a) : (b))

// The debug camera, once a frame: the mode button cycles keyboard / pad / free camera control of the saved
// frame (the pad modes move whichever camera is current), then the replay cameras follow their car.
void fn_8026C014(int player) {
    FreePad pad;
    FreePad keys;
    if (fn_802D3EBC()) {
        return;
    }
    if (fn_801298E8(&pad, player ? fn_80137CFC(0) : 0)) {
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
        CamVec spin;
        CamVec move;
        CamVec w;
        CamRot m;
        f32 dt;
        int shift;
        int ctrl;
        f32 speed;
        f32 turn;
        move.x = lbl_80563D78;
        move.y = lbl_80563D78;
        move.z = lbl_80563D78;
        spin.x = lbl_80563D78;
        spin.y = lbl_80563D78;
        spin.z = lbl_80563D78;
        if (!lbl_8055F501) {
            CamSet(&lbl_8049A1C0.vel, lbl_80563D78, lbl_80563D78, lbl_80563D78);
            lbl_8055F501 = 1;
        }
        if (!lbl_8055F502) {
            CamSet(&lbl_8049A1C0.spin, lbl_80563D78, lbl_80563D78, lbl_80563D78);
            lbl_8055F502 = 1;
        }
        dt = CamMin((f32)(s32)(fn_802E2C20() - lbl_8055F504) / lbl_80563D88, lbl_80563D8C);
        lbl_8055F504 = fn_802E2C20();
        CamScale(&lbl_8049A1C0.vel, lbl_80563DD8);
        CamScale(&lbl_8049A1C0.spin, lbl_80563DD8);
        shift = fn_802E4A1C(0xFD);
        ctrl = 0;
        if (fn_802E4A1C(0xFE) || fn_802E4A1C(0x3A)) {
            ctrl = 1;
        }
        {
            f32 moveRate[4] = {1.0f, 0.1f, 10.0f, 0.01f};
            f32 turnRate[4] = {0.05f, 0.0125f, 0.15f, 0.0025f};
            f32 frames = dt / lbl_80563D8C;
            speed = frames * moveRate[shift * 2 | ctrl];
            turn = frames * turnRate[shift * 2 | ctrl];
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
            move.y = speed * lbl_80563D9C;
        } else if (fn_802E4A1C(0x52)) {
            move.y = -speed * lbl_80563D9C;
        }
        if (fn_802E4A1C(0x4D)) {
            if (fn_802E4A1C(0x4F)) {
                spin.y = turn * lbl_80563D9C;
            } else {
                spin.y = turn;
            }
        } else if (fn_802E4A1C(0x4B)) {
            if (fn_802E4A1C(0x51)) {
                spin.y = -turn * lbl_80563D9C;
            } else {
                spin.y = -turn;
            }
        }
        if (fn_802E4A1C(0x48)) {
            spin.x = turn * lbl_80563D9C;
        } else if (fn_802E4A1C(0x50)) {
            spin.x = -turn * lbl_80563D9C;
        }
        if (fn_802E4A1C(0x47)) {
            spin.z = turn;
        } else if (fn_802E4A1C(0x49)) {
            spin.z = -turn;
        }
        {
            f32 step = lbl_80563DDC;
            f32 lo = lbl_80563DE0;
            f32 hi = lbl_80563DE4;
            if (fn_802E4A1C(0x37)) {
                lbl_8055F47C += step;
            } else if (fn_802E4A1C(0xB5)) {
                lbl_8055F47C -= step;
            }
            lbl_8055F47C = CamMax(CamMin(hi, lbl_8055F47C), lo);
        }
        if (!CamIsZero(&spin)) {
            CamCopy(&lbl_8049A1C0.spin, &spin);
        }
        if (!CamIsZero(&move)) {
            CamCopy(&lbl_8049A1C0.vel, &move);
        }
        CamMulVec(&w, &lbl_8049A1C0.vel, &lbl_80499050.rot);
        lbl_80499050.pos.x = lbl_80499050.pos.x + w.x;
        lbl_80499050.pos.y = lbl_80499050.pos.y + w.y;
        lbl_80499050.pos.z = lbl_80499050.pos.z + w.z;
        Mat3FromEuler__FP4Mat3PC4Vec3(&m, &lbl_8049A1C0.spin);
        Mat3Mul__FP4Mat3PC4Mat3PC4Mat3(&lbl_80499050.rot, &m, &lbl_80499050.rot);
    } else if (pad.w44[3] & 0x10) {
        // Orbit: the left stick turns about the vertical and tilts, the d-pad moves.
        CamRot yawM;
        CamRot pitchM;
        CamVec yawE;
        CamVec pitchE;
        CamVec v;
        u32 now = fn_802E2C20();
        f32 dt = CAM_MAX(lbl_80563DE8 * (f32)(now - lbl_8055F508), lbl_80563DEC);
        f32 speed;
        f32 yawRate;
        f32 fovRate;
        f32 lo;
        f32 hi;
        f32 yaw;
        f32 s;
        f32 target;
        f32 x;
        f32 z;
        lbl_8055F508 = now;
        speed = lbl_80563DF4 * (lbl_80563DF8 * (lbl_80563DFC * dt));
        yawRate = dt * lbl_80563DF0;
        fovRate = dt * lbl_80563E00;
        lo = lbl_80563DE0;
        hi = lbl_80563DE4;
        if (pad.keys[4]) {
            f32 dir;
            speed = lbl_80563D78;
            dir = speed;
            if (pad.w40 & 1) {
                dir = lbl_80563D98;
            }
            if (pad.w40 & 4) {
                dir = lbl_80563D94;
            }
            lbl_8055F47C = CamMax(CamMin(hi, lbl_8055F47C + dir * fovRate), lo);
        } else if (pad.keys[5]) {
            speed *= lbl_80563D7C;
        } else if (pad.keys[2]) {
            speed *= lbl_80563DA0;
        }
        yaw = lbl_8055F50C;
        if (pad.x0 > lbl_80563D78) {
            s = CamMax(CamMin(lbl_80563D94, lbl_80563D84 * pad.x0 - lbl_80563D7C), lbl_80563D78);
        } else {
            s = CamMax(CamMin(lbl_80563D78, lbl_80563D7C + lbl_80563D84 * pad.x0), lbl_80563D98);
        }
        yaw += yawRate * s;
        target = pad.y0 * lbl_80563DB0;
        lbl_8055F510 = target + lbl_80563E04 * (lbl_8055F510 - target);
        CamIdentity(&lbl_80499050.rot);
        CamSet(&yawE, lbl_80563D78, yaw, lbl_80563D78);
        Mat3FromEuler__FP4Mat3PC4Vec3(&yawM, &yawE);
        Mat3Mul__FP4Mat3PC4Mat3PC4Mat3(&lbl_80499050.rot, &yawM, &lbl_80499050.rot);
        CamSet(&pitchE, -lbl_8055F510, lbl_80563D78, lbl_80563D78);
        Mat3FromEuler__FP4Mat3PC4Vec3(&pitchM, &pitchE);
        Mat3Mul__FP4Mat3PC4Mat3PC4Mat3(&lbl_80499050.rot, &pitchM, &lbl_80499050.rot);
        z = lbl_80563D78;
        x = z;
        if (pad.w40 & 1) {
            z = speed;
        } else if (pad.w40 & 4) {
            z = -speed;
        }
        if (pad.w40 & 8) {
            x = -speed;
        } else if (pad.w40 & 2) {
            x = speed;
        }
        CamSet(&v, x, lbl_80563D78, z);
        CamMove(&lbl_80499050, &v);
        lbl_8055F50C = yaw;
    } else {
        // Stick flight of the current camera; the trigger and a button boost it.
        u32 now = fn_802E2C20();
        f32 dt = CAM_MAX(lbl_80563DE8 * (f32)(now - lbl_8055F514), lbl_80563DEC);
        f32 speed;
        f32 turn;
        f32 fovRate;
        f32 lo;
        f32 hi;
        f32 t;
        f32 boost;
        f32 roll;
        f32 lift;
        lbl_8055F514 = now;
        speed = lbl_80563DF8 * (lbl_80563DFC * dt);
        turn = dt * lbl_80563DF0;
        fovRate = dt * lbl_80563E00;
        lo = lbl_80563DE0;
        hi = lbl_80563DE4;
        t = CamMax(CamMin(lbl_80563D94, (pad.y3 - lbl_80563D98) * lbl_80563D9C), lbl_80563D78);
        boost = lbl_80563D94;
        if (pad.keys[6]) {
            boost = lbl_80563DF4;
        }
        speed *= boost * (lbl_80563D94 + t * lbl_80563DA0);
        roll = lbl_80563D78;
        if (pad.w40 & 8) {
            roll = speed;
        } else if (pad.w40 & 2) {
            roll = lbl_80563D98 * speed;
        }
        lift = lbl_80563D78;
        if (pad.w40 & 1) {
            lift = speed;
        }
        if (pad.w40 & 4) {
            lift = lbl_80563D98 * speed;
        }
        if (pad.keys[5]) {
            lbl_8055F47C = CamMax(CamMin(hi, lbl_8055F47C + pad.y0 * fovRate), lo);
        } else {
            switch ((s32)lbl_8055F4E0) {
            case 0:
            case 4:
                CamFly(&lbl_80499050, &pad, turn, roll, speed, lift);
                break;
            case 1: {
                CamVec v;
                CamVec v2;
                CamSet(&v, lbl_80563D78, speed * -fn_8026B844(pad.y0), lbl_80563D78);
                CamMove(&lbl_80499050, &v);
                CamSet(&v2, pad.y1 * speed, lbl_80563DEC * (lift * speed), -pad.x1 * speed);
                CamMove(&lbl_80499050, &v2);
                break;
            }
            case 3: {
                CamFrame* f = &lbl_8049A1C0.frame;
                f32 s = speed * fn_8026B844(pad.y1);
                f32 u;
                lbl_80499050.pos.x = lbl_80499050.pos.x + f->rot.axes[0].x * s;
                lbl_80499050.pos.y = lbl_80499050.pos.y + f->rot.axes[0].y * s;
                lbl_80499050.pos.z = lbl_80499050.pos.z + f->rot.axes[0].z * s;
                u = lbl_80563DEC * (lift * speed);
                lbl_80499050.pos.x = lbl_80499050.pos.x + f->rot.axes[1].x * u;
                lbl_80499050.pos.y = lbl_80499050.pos.y + f->rot.axes[1].y * u;
                lbl_80499050.pos.z = lbl_80499050.pos.z + f->rot.axes[1].z * u;
                s = speed * -fn_8026B844(pad.x1);
                lbl_80499050.pos.x = lbl_80499050.pos.x + f->rot.axes[2].x * s;
                lbl_80499050.pos.y = lbl_80499050.pos.y + f->rot.axes[2].y * s;
                lbl_80499050.pos.z = lbl_80499050.pos.z + f->rot.axes[2].z * s;
                break;
            }
            case 2:
                CamFly((CamFrame*)&lbl_8049A168, &pad, turn, roll, speed, lift);
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
        CamVec up;
        if (!car) {
            return;
        }
        f = &lbl_80499050;
        f->rot.axes[2].x = car->frame.pos.x - f->pos.x;
        f->rot.axes[2].y = car->frame.pos.y - f->pos.y;
        f->rot.axes[2].z = car->frame.pos.z - f->pos.z;
        CamSet(&up, lbl_80563D78, lbl_80563D94, lbl_80563D78);
        if (f->rot.axes[2].x * f->rot.axes[2].x + f->rot.axes[2].z * f->rot.axes[2].z > lbl_80563E08) {
            CamRot r;
            PSVECCrossProduct(&up, &f->rot.axes[2], &f->rot.axes[0]);
            PSVECCrossProduct(&f->rot.axes[2], &f->rot.axes[0], &f->rot.axes[1]);
            r.axes[0] = f->rot.axes[0];
            r.axes[2] = f->rot.axes[2];
            PSVECCrossProduct(&r.axes[2], &r.axes[0], &r.axes[1]);
            PSVECNormalize(&r.axes[0], &r.axes[0]);
            PSVECNormalize(&r.axes[1], &r.axes[1]);
            PSVECCrossProduct(&r.axes[0], &r.axes[1], &r.axes[2]);
            f->rot = r;
        }
    } else if ((s32)lbl_8055F4E0 == 2 && player) {
        s32 id = fn_8026A344(0);
        CamCar* car = fn_8017960C(id);
        CamFrame tgt;
        if (car && ((s32)lbl_8055F4E4 & 4)) {
            tgt = car->frame;
        } else if (car) {
            CamQuat q;
            MtxToQuat(&car->frame, &q);
            if (id == lbl_8049A168.car) {
                CamQuat qs;
                CamVec p;
                fn_8012F57C(&qs, &lbl_8049A168.rot, &q, lbl_80563E0C);
                if (!((s32)lbl_8055F4E4 & 2)) {
                    QuatToMtx(&qs, &tgt.rot);
                } else {
                    CamIdentity(&tgt.rot);
                }
                p.x = lbl_8049A168.pos.x + lbl_80563D9C * (car->frame.pos.x - lbl_8049A168.pos.x);
                p.y = lbl_8049A168.pos.y + lbl_80563D9C * (car->frame.pos.y - lbl_8049A168.pos.y);
                p.z = lbl_8049A168.pos.z + lbl_80563D9C * (car->frame.pos.z - lbl_8049A168.pos.z);
                lbl_8049A168.rot = qs;
                tgt.pos = p;
                lbl_8049A168.pos = p;
            } else {
                tgt = car->frame;
                lbl_8049A168.rot = q;
                lbl_8049A168.pos.x = car->frame.pos.x;
                lbl_8049A168.pos.y = car->frame.pos.y;
                lbl_8049A168.pos.z = car->frame.pos.z;
            }
        } else {
            CamIdentity(&tgt.rot);
            tgt.pos.x = lbl_80563D78;
            tgt.pos.y = lbl_80563D78;
            tgt.pos.z = lbl_80563D78;
            lbl_8049A168.rot.x = lbl_80563D78;
            lbl_8049A168.rot.y = lbl_80563D78;
            lbl_8049A168.rot.z = lbl_80563D78;
            lbl_8049A168.rot.w = lbl_80563D94;
            lbl_8049A168.pos.x = lbl_80563D78;
            lbl_8049A168.pos.y = lbl_80563D78;
            lbl_8049A168.pos.z = lbl_80563D78;
        }
        fn_801BFD34(&lbl_80499050.pos, &lbl_8049A168.offset, &tgt);
        Mat3Mul__FP4Mat3PC4Mat3PC4Mat3(&lbl_80499050.rot, (CamRot*)&lbl_8049A168, &tgt.rot);
        if ((s32)lbl_8055F4E4 & 1) {
            f32 floor = lbl_80563D94 + fn_801E9DDC(&lbl_8049A168.hint, lbl_80499050.pos.x, lbl_80499050.pos.z);
            lbl_80499050.pos.y = CamMax(lbl_80499050.pos.y, floor);
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
            up.x = lbl_80563D78;
            up.y = lbl_80563D94;
            up.z = lbl_80563D78;
            fn_8026E688(&c->frame, &f.pos, &b->frame.pos, &up);
        } else {
            c->frame.rot.axes[0].x = lbl_80563D94;
            c->frame.rot.axes[0].y = lbl_80563D78;
            c->frame.rot.axes[0].z = lbl_80563D78;
            c->frame.rot.axes[1].x = lbl_80563D78;
            c->frame.rot.axes[1].y = lbl_80563D94;
            c->frame.rot.axes[1].z = lbl_80563D78;
            c->frame.rot.axes[2].x = lbl_80563D78;
            c->frame.rot.axes[2].y = lbl_80563D78;
            c->frame.rot.axes[2].z = lbl_80563D94;
            c->frame.pos.x = lbl_80563D78;
            c->frame.pos.y = lbl_80563D78;
            c->frame.pos.z = lbl_80563D78;
        }
        break;
    case 4: {
        CamFrame* car = &fn_8017960C(fn_8026A344(0))->frame;
        fn_801BFD34(&c->frame.pos, &lbl_80499050.pos, car);
        Mat3Mul__FP4Mat3PC4Mat3PC4Mat3(&c->frame.rot, &lbl_80499050.rot, &car->rot);
        break;
    }
    default:
        CopyFrame(&c->frame, &lbl_80499050);
        break;
    }
    c->fov = lbl_8055F47C;
    c->shake[0] = lbl_80563D78;
    c->shake[1] = lbl_80563D78;
    c->shake[2] = lbl_80563D78;
    CopyFrame(&lbl_8049A1C0.frame, &c->frame);
}

void* fn_8026DDD0(void) {
    return lbl_8055F4E0;
}

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
        lbl_8049A168.offset.x = lbl_80563D78;
        lbl_8049A168.offset.y = lbl_80563DA0;
        lbl_8049A168.offset.z = lbl_80563E18;
        lbl_8049A168.side.x = lbl_80563D94;
        lbl_8049A168.side.y = lbl_80563D78;
        lbl_8049A168.side.z = lbl_80563D78;
        lbl_8049A168.dir.x = lbl_80563D78 * lbl_80563D98;
        lbl_8049A168.dir.y = lbl_80563DA0 * lbl_80563D98;
        lbl_8049A168.dir.z = lbl_80563E18 * lbl_80563D98;
        PSVECNormalize(&lbl_8049A168.dir, &lbl_8049A168.dir);
        PSVECCrossProduct(&lbl_8049A168.dir, &lbl_8049A168.side, &lbl_8049A168.up);
        lbl_8049A168.car = fn_8026A344(0);
        car = fn_8017960C(lbl_8049A168.car);
        if (car) {
            const CamFrame* f = &car->frame;
            MtxToQuat(f, &lbl_8049A168.rot);
            lbl_8049A168.pos.x = car->frame.pos.x;
            lbl_8049A168.pos.y = car->frame.pos.y;
            lbl_8049A168.pos.z = car->frame.pos.z;
        } else {
            lbl_8049A168.rot.x = lbl_80563D78;
            lbl_8049A168.rot.y = lbl_80563D78;
            lbl_8049A168.rot.z = lbl_80563D78;
            lbl_8049A168.rot.w = lbl_80563D94;
            lbl_8049A168.pos.x = lbl_80563D78;
            lbl_8049A168.pos.y = lbl_80563D78;
            lbl_8049A168.pos.z = lbl_80563D78;
        }
    } else if (fn_8026BFF0(mode) && !fn_8026BFF0((s32)lbl_8055F4E0)) {
        lbl_80499050.rot.axes[0].x = lbl_80563D94;
        lbl_80499050.rot.axes[0].y = lbl_80563D78;
        lbl_80499050.rot.axes[0].z = lbl_80563D78;
        lbl_80499050.rot.axes[1].x = lbl_80563D78;
        lbl_80499050.rot.axes[1].y = lbl_80563D94;
        lbl_80499050.rot.axes[1].z = lbl_80563D78;
        lbl_80499050.rot.axes[2].x = lbl_80563D78;
        lbl_80499050.rot.axes[2].y = lbl_80563D78;
        lbl_80499050.rot.axes[2].z = lbl_80563D94;
        lbl_80499050.pos.x = lbl_80563D78;
        lbl_80499050.pos.y = lbl_80563D78;
        lbl_80499050.pos.z = lbl_80563D78;
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
extern const f32 lbl_80563D78;
extern const f32 lbl_80563D7C;
extern const f32 lbl_80563D80;
extern const f32 lbl_80563D84;

}  // extern "C"
