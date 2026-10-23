// X-ray suspension view (0x801EF190-0x801EFF4C): the car's front suspension drawn through the body. A
// context ("xray_context") holds the chassis model and, per side, a spinning hub model ("spin_l.mod" /
// "spin_r.mod") and the joints of the upper and lower mounts, shock, shaft and spring. Each frame the
// mounts are aimed at the wheel, the shock and shaft follow, and the spring is stretched between its
// ends. Names are guesses apart from the strings.
#include "game/common.h"

void* operator new(size_t size, const char* tag);

struct Mat3 {
    f32 m[3][3];
};

struct Joint;

extern "C" {
int fn_802C6FC0(const char* name, int a, int b);  // load a model
void fn_802C6CF8(int model);                        // release a model
void fn_802C7760(int model, f32* x0, f32* x1, f32* y0, f32* y1, f32* z0, f32* z1);  // bounds
void fn_802C744C(int model, const ValMatrix* m);
void fn_802C70AC(int model, const ValMatrix* m, int a, int b);
int fn_802C7A34(int a, const ValVec3* v, int n);
Joint* fn_801E3004(int model, const char* name, int flags);
Joint* fn_801E31D8(Joint* parent, const char* name, int flags);
void fn_801E323C(Joint* j);
void fn_801E3440(ValVec3* out, const Joint* j, const ValMatrix* root);
void fn_801E34E4(const Joint* j, const ValMatrix* root);
int fn_801E3534(const Joint* j);
void fn_801E353C(Joint* j, f32 angle);
void fn_801E35D4(Joint* j, const Mat3* r);
f32 fn_801F159C(void* w);
void fn_80015A20(Mat3* out, const ValVec3* angles);
f32 fn_80144514(f32 a0, f32 b0, f32 a1, f32 b1, f32 a2, f32 b2);  // a0*b0 + a1*b1 + a2*b2
f32 PSVECDotProduct(const void* a, const void* b);
void PSVECNormalize(const void* in, void* out);
double atan2(double y, double x);
double cos(double x);
double sin(double x);
}

void Mat3Mul(Mat3* out, const Mat3* a, const Mat3* b);

static inline void Mul(Mat3* out, const Mat3* l, const Mat3* r) {
    if (out == l || out == r) {
        f32 m00 = fn_80144514(r->m[0][0], l->m[0][0], r->m[1][0], l->m[0][1], r->m[2][0], l->m[0][2]);
        f32 m10 = fn_80144514(r->m[0][0], l->m[1][0], r->m[1][0], l->m[1][1], r->m[2][0], l->m[1][2]);
        f32 m20 = fn_80144514(r->m[0][0], l->m[2][0], r->m[1][0], l->m[2][1], r->m[2][0], l->m[2][2]);
        f32 m01 = fn_80144514(r->m[0][1], l->m[0][0], r->m[1][1], l->m[0][1], r->m[2][1], l->m[0][2]);
        f32 m11 = fn_80144514(r->m[0][1], l->m[1][0], r->m[1][1], l->m[1][1], r->m[2][1], l->m[1][2]);
        f32 m21 = fn_80144514(r->m[0][1], l->m[2][0], r->m[1][1], l->m[2][1], r->m[2][1], l->m[2][2]);
        f32 m02 = fn_80144514(r->m[0][2], l->m[0][0], r->m[1][2], l->m[0][1], r->m[2][2], l->m[0][2]);
        f32 m12 = fn_80144514(r->m[0][2], l->m[1][0], r->m[1][2], l->m[1][1], r->m[2][2], l->m[1][2]);
        f32 m22 = fn_80144514(r->m[0][2], l->m[2][0], r->m[1][2], l->m[2][1], r->m[2][2], l->m[2][2]);
        out->m[0][0] = m00;
        out->m[1][0] = m10;
        out->m[2][0] = m20;
        out->m[0][1] = m01;
        out->m[1][1] = m11;
        out->m[2][1] = m21;
        out->m[0][2] = m02;
        out->m[1][2] = m12;
        out->m[2][2] = m22;
    } else {
        out->m[0][0] = fn_80144514(r->m[0][0], l->m[0][0], r->m[1][0], l->m[0][1], r->m[2][0], l->m[0][2]);
        out->m[1][0] = fn_80144514(r->m[0][0], l->m[1][0], r->m[1][0], l->m[1][1], r->m[2][0], l->m[1][2]);
        out->m[2][0] = fn_80144514(r->m[0][0], l->m[2][0], r->m[1][0], l->m[2][1], r->m[2][0], l->m[2][2]);
        out->m[0][1] = fn_80144514(r->m[0][1], l->m[0][0], r->m[1][1], l->m[0][1], r->m[2][1], l->m[0][2]);
        out->m[1][1] = fn_80144514(r->m[0][1], l->m[1][0], r->m[1][1], l->m[1][1], r->m[2][1], l->m[1][2]);
        out->m[2][1] = fn_80144514(r->m[0][1], l->m[2][0], r->m[1][1], l->m[2][1], r->m[2][1], l->m[2][2]);
        out->m[0][2] = fn_80144514(r->m[0][2], l->m[0][0], r->m[1][2], l->m[0][1], r->m[2][2], l->m[0][2]);
        out->m[1][2] = fn_80144514(r->m[0][2], l->m[1][0], r->m[1][2], l->m[1][1], r->m[2][2], l->m[1][2]);
        out->m[2][2] = fn_80144514(r->m[0][2], l->m[2][0], r->m[1][2], l->m[2][1], r->m[2][2], l->m[2][2]);
    }
}

// One side of the suspension.
struct XraySide {
    int hub;            // offset 0x0: spinning hub model
    Joint* mountU;      // offset 0x4
    Joint* mountL;      // offset 0x8
    Joint* spring;      // offset 0xC
    Joint* shock;       // offset 0x10
    Joint* shaft;       // offset 0x14
    Joint* springEnd;   // offset 0x18
    f32 springLen;      // offset 0x1C
    f32 side;           // offset 0x20: -1 left, 1 right
};

struct XrayContext {
    int model;          // offset 0x0
    XraySide sides[2];  // offset 0x4
};

// The suspension view: carmodel allocates 4 bytes ("xray_model") and calls these by hand.
struct XraySusp {
    XrayContext* ctx;  // offset 0x0
};

struct XrayCar {
    ValMatrix mtx;  // offset 0x0
    u8 pad30[0xE8 - 0x30];
    f32 track;      // offset 0xE8: half the track width
    u8 padEC[0xF0 - 0xEC];
    f32 wheelbase;  // offset 0xF0
};

ModuleTag lbl_8055EBD8;

extern "C" {

// A model's height (0.1 when there is none).
f32 fn_801EF190(int model) {
    if (model == -1) {
        return 0.1f;
    }
    f32 x0;
    f32 y0 = 0.0f;
    f32 z0;
    f32 x1;
    f32 y1 = 0.0f;
    f32 z1;
    fn_802C7760(model, &x0, &x1, &y0, &y1, &z0, &z1);
    return y1 - y0;
}

void fn_801EF53C(XraySide* s, const ValMatrix* root, const ValMatrix* wheel);

}  // extern "C"

extern "C" {

XraySusp* fn_801EF1F0(XraySusp* x, const char* model) {
    XrayContext*& ctx = x->ctx;
    ctx = new ("xray_context") XrayContext;
    ctx->model = fn_802C6FC0(model, 0, 0);
    ctx->sides[0].side = -1.0f;
    ctx->sides[0].hub = fn_802C6FC0("spin_l.mod", 0, 0);
    ctx->sides[1].hub = fn_802C6FC0("spin_r.mod", 0, 0);
    ctx->sides[0].mountU = fn_801E3004(ctx->model, "mount_ul", -1);
    ctx->sides[0].mountL = fn_801E3004(ctx->model, "mount_ll", -1);
    ctx->sides[0].shaft = fn_801E31D8(ctx->sides[0].mountL, "shaft", -1);
    ctx->sides[0].spring = fn_801E31D8(ctx->sides[0].mountL, "spring", -1);
    ctx->sides[0].shock = fn_801E3004(ctx->model, "shock_ul", -1);
    ctx->sides[0].springEnd = fn_801E3004(ctx->model, "springl", -1);
    ctx->sides[0].springLen = fn_801EF190(fn_801E3534(ctx->sides[0].spring));
    ctx->sides[1].side = 1.0f;
    ctx->sides[1].mountU = fn_801E3004(ctx->model, "mount_ur", -1);
    ctx->sides[1].mountL = fn_801E3004(ctx->model, "mount_lr", -1);
    ctx->sides[1].shaft = fn_801E31D8(ctx->sides[1].mountL, "shaft", -1);
    ctx->sides[1].spring = fn_801E31D8(ctx->sides[1].mountL, "spring", -1);
    ctx->sides[1].shock = fn_801E3004(ctx->model, "shock_ur", -1);
    ctx->sides[1].springEnd = fn_801E3004(ctx->model, "springr", -1);
    ctx->sides[1].springLen = fn_801EF190(fn_801E3534(ctx->sides[1].spring));
    return x;
}

XraySusp* fn_801EF428(XraySusp* x, int del) {
    if (x) {
        XrayContext*& ctx = x->ctx;
    fn_801E323C(ctx->sides[0].mountU);
    fn_801E323C(ctx->sides[0].mountL);
    fn_801E323C(ctx->sides[0].shaft);
    fn_801E323C(ctx->sides[0].spring);
    fn_801E323C(ctx->sides[0].shock);
    fn_801E323C(ctx->sides[0].springEnd);
    fn_802C6CF8(ctx->sides[0].hub);
    fn_801E323C(ctx->sides[1].mountU);
    fn_801E323C(ctx->sides[1].mountL);
    fn_801E323C(ctx->sides[1].shaft);
    fn_801E323C(ctx->sides[1].spring);
    fn_801E323C(ctx->sides[1].shock);
    fn_801E323C(ctx->sides[1].springEnd);
    fn_802C6CF8(ctx->sides[1].hub);
    fn_802C6CF8(ctx->model);
    delete ctx;
    ctx = 0;
        if (del > 0) {
            delete x;
        }
    }
    return x;
}

}  // extern "C"

static inline void Madd(ValVec3* out, const ValVec3* a, const ValVec3* b, f32 k) {
    out->x = a->x + b->x * k;
    out->y = a->y + b->y * k;
    out->z = a->z + b->z * k;
}

static inline void Sub(ValVec3* out, const ValVec3* a, const ValVec3* b) {
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

extern "C" {

// Poses side 's': its hub at the wheel and the suspension joints aimed at it.
void fn_801EF53C(XraySide* s, const ValMatrix* root, const ValMatrix* wheel) {
    ValMatrix w;
    ValMatrix hub;
    Mat3 rot;
    Mat3 scale;
    Mat3 m;
    Mat3 r;
    ValVec3 d;
    ValVec3 q;
    ValVec3 p;
    ValVec3 turn;
    w = *wheel;
    f32 k = 0.2159f * -s->side;
    w.origin.x += root->basis[0].x * k;
    w.origin.y += root->basis[0].y * k;
    w.origin.z += root->basis[0].z * k;
    hub = w;
    if (s->side > 0.0f) {
        turn.x = 0.0f;
        turn.y = 3.1415927f;
        turn.z = 0.0f;
        fn_80015A20(&r, &turn);
        Mat3Mul((Mat3*)&hub, &r, (Mat3*)&hub);
    }
    fn_802C744C(s->hub, &hub);
    Madd(&p, &w.origin, &w.basis[1], 0.3f);
    f32 quarter = 0.7853982f;
    fn_801E3440(&q, s->mountU, root);
    Sub(&d, &p, &q);
    PSVECNormalize(&d, &d);
    f32 a = (f32)atan2(PSVECDotProduct(&d, &root->basis[1]), s->side * PSVECDotProduct(&d, &root->basis[0]));
    fn_801E353C(s->mountU, s->side * a - s->side * quarter);
    Madd(&p, &w.origin, &w.basis[1], -0.2f);
    fn_801E3440(&q, s->mountL, root);
    Sub(&d, &p, &q);
    PSVECNormalize(&d, &d);
    a = (f32)atan2(PSVECDotProduct(&d, &root->basis[1]), s->side * PSVECDotProduct(&d, &root->basis[0]));
    f32 lower = s->side * a;
    fn_801E353C(s->mountL, lower);
    fn_801E3440(&q, s->shaft, root);
    fn_801E3440(&p, s->shock, root);
    Sub(&d, &p, &q);
    PSVECNormalize(&d, &d);
    f32 shock = -s->side * (f32)atan2(s->side * PSVECDotProduct(&d, &root->basis[0]), PSVECDotProduct(&d, &root->basis[1]));
    fn_801E353C(s->shaft, shock - lower);
    fn_801E353C(s->shock, shock);
    fn_801E3440(&q, s->spring, root);
    fn_801E3440(&p, s->springEnd, root);
    Sub(&d, &p, &q);
    f32 c;
    f32 stretch = PSVECDotProduct(&d, &root->basis[1]) / s->springLen;
    c = cos(-lower);
    f32 sn = sin(-lower);
    rot.m[0][0] = c;
    rot.m[0][1] = sn;
    rot.m[0][2] = 0.0f;
    rot.m[1][0] = -sn;
    rot.m[1][1] = c;
    rot.m[1][2] = 0.0f;
    rot.m[2][0] = 0.0f;
    rot.m[2][1] = 0.0f;
    rot.m[2][2] = 1.0f;
    scale.m[0][0] = 1.0f;
    scale.m[0][1] = 0.0f;
    scale.m[0][2] = 0.0f;
    scale.m[1][0] = 0.0f;
    scale.m[1][1] = stretch;
    scale.m[1][2] = 0.0f;
    scale.m[2][0] = 0.0f;
    scale.m[2][1] = 0.0f;
    scale.m[2][2] = 1.0f;
    Mul(&m, &scale, &rot);
    fn_801E35D4(s->spring, &m);
    fn_801E34E4(s->mountU, root);
    fn_801E34E4(s->mountL, root);
    fn_801E34E4(s->shaft, root);
    fn_801E34E4(s->spring, root);
    fn_801E34E4(s->shock, root);
}

}  // extern "C"

struct XrayWheel {
    u8 pad0[0x18];
    ValMatrix mtx;  // offset 0x18
};

extern "C" void fn_801EFC90(XraySusp* x, const ValMatrix* carMtx, void** wheels) {
    XrayContext*& ctx = x->ctx;
    const XrayCar* car = (const XrayCar*)carMtx;
    f32 back = car->wheelbase - 1.397f;
    ValMatrix m = car->mtx;
    f32 up = fn_801F159C(wheels[1]) - 0.3556f;
    Madd(&m.origin, &m.origin, &m.basis[2], back);
    Madd(&m.origin, &m.origin, &m.basis[1], up);
    f32 half = car->track - 0.76835f;
    ValMatrix left = m;
    Madd(&left.origin, &m.origin, &m.basis[0], -half);
    ValMatrix right = m;
    Madd(&right.origin, &m.origin, &m.basis[0], half);
    ValVec3 ends[2];
    ends[0].x = half;
    ends[0].y = 0.0f;
    ends[0].z = 0.0f;
    ends[1].x = -half;
    ends[1].y = 0.0f;
    ends[1].z = 0.0f;
    fn_802C70AC(ctx->model, &m, fn_802C7A34(1, ends, 2), -1);
    fn_801EF53C(&ctx->sides[0], &left, &((XrayWheel*)wheels[0])->mtx);
    fn_801EF53C(&ctx->sides[1], &right, &((XrayWheel*)wheels[1])->mtx);
}
