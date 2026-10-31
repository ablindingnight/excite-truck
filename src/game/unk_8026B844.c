// Code 0x8026B844-0x8026E224, imported verbatim from the upstream project (github.com/orangedude27/excite-truck,
// CC0; file src/auto/text_8026B840_.c), where its functions match exactly. Unit name is provisional.
// CLEANUP: automated-decompiler source with raw offsets and placeholder types; rewrite with real types
// before the PC port. Tricks are tagged MATCHING-AID.
#include "types.h"

extern void *lbl_8055F4E0;
extern void *lbl_8055F4E4;
extern void *lbl_8055F4E8;
extern const f32 lbl_80563D78;
extern const f32 lbl_80563D7C;
extern const f32 lbl_80563D80;
extern const f32 lbl_80563D84;

void fn_8026B840(void) {}

f32 fn_8026B844(f32 f) {
    f32 c = lbl_80563D78;
    if (f > c) {
        f32 t = (f - lbl_80563D7C) / lbl_80563D80;
        if (c > t) c = t;
        return c;
    }
    {
        f32 t = (lbl_80563D7C + f) / lbl_80563D84;
        if (c < t) c = t;
    }
    return c;
}

// Camera frames (camframe.cpp's CamFrame: three axes and a position).
typedef struct CamVec {
    f32 x, y, z;
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
extern CamFrame lbl_8049A1C0;  // the frame last applied
extern f32 lbl_8055F47C;       // its field of view
extern s32 lbl_8055F4F0;       // the car the replay camera follows (-1: none)
extern const f32 lbl_80563D94;       // 1.0f
s32 fn_8026A344(s32 i);
CamCar* fn_8017960C(s32 car);
void fn_8026E688(CamFrame* out, const CamVec* eye, const CamVec* target, const CamVec* up);
void fn_801BFD34(CamVec* out, const CamVec* v, const CamFrame* m);
void Mat3Mul__FP4Mat3PC4Mat3PC4Mat3(CamRot* out, const CamRot* a, const CamRot* b);

// The pad as fn_801298E8 fills it (achview's ViewPad): sticks, then the analog trigger at 0x1C.
typedef struct FreePad {
    f32 stick[4];        // offset 0x0
    u8 pad10[0xC];
    f32 trigger;         // offset 0x1C
    u8 pad20[0x44];
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

static __inline f32 CamMin(f32 a, f32 b) { return a < b ? a : b; }
static __inline f32 CamMax(f32 a, f32 b) { return a > b ? a : b; }
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
        f32 t = CamMax(CamMin(lbl_80563D94, (pad.trigger - lbl_80563D98) * lbl_80563D9C), lbl_80563D78);
        f32 speed = lbl_80563D94 * (lbl_80563DA4 * (lbl_80563D94 + t * lbl_80563DA0));
        f32 x;
        f32 z;
        lbl_8055F4F4 += turn * (dt * fn_8026B844(pad.stick[0]));
        if (lbl_8055F4F4 > lbl_80563DA8) {
            lbl_8055F4F4 -= lbl_80563DA8;
        } else if (lbl_8055F4F4 < lbl_80563DAC) {
            lbl_8055F4F4 += lbl_80563DA8;
        }
        lbl_8055F4F8 = CamMax(CamMin(lbl_80563DB0, lbl_8055F4F8 + turn * (dt * fn_8026B844(pad.stick[1]))), lbl_80563DB4);
        z = speed * (dt * -fn_8026B844(pad.stick[2]));
        x = speed * (dt * fn_8026B844(pad.stick[3]));
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
    CopyFrame(&lbl_8049A1C0, &c->frame);
}

void* fn_8026DDD0(void) {
    return lbl_8055F4E0;
}

typedef struct CamQuat {
    f32 x, y, z, w;
} CamQuat;
// The blimp camera: a fixed offset behind and above the car it follows.
typedef struct BlimpCam {
    CamVec side;     // offset 0x0
    CamVec up;       // offset 0xC
    CamVec dir;      // offset 0x18
    CamVec offset;   // offset 0x24
    CamQuat rot;     // offset 0x30
    CamVec pos;      // offset 0x40
    s16 hint;        // offset 0x4C
    s32 car;         // offset 0x50
} BlimpCam;

extern BlimpCam lbl_8049A168;
extern const f32 lbl_80563D98;  // -1.0f
extern const f32 lbl_80563D9C;  // 0.5f
extern const f32 lbl_80563DA0;  // 5.0f
extern const f32 lbl_80563E18;  // -10.0f
double sqrt(double x);
void PSVECNormalize(const CamVec* in, CamVec* out);
void PSVECCrossProduct(const CamVec* a, const CamVec* b, CamVec* out);
void fn_802E30C8(const char* fmt, ...);
void fn_801E9DC4(s16* hint);

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

