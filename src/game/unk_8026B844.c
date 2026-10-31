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

BOOL fn_8026BFF0(s32 x) {
    switch (x) { case 3: case 4: return TRUE; default: return FALSE; }
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

