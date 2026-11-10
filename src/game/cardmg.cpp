// Car damage (0x8018BFA0-0x8018FB18): the detachable body parts ("right hood hinge", ...), wheel loss
// ("cardmg: wheel %d is already missing from car %d") and the part break-away timers. Written against
// raw field offsets; the large routines and the initialiser are still stubs. Names are guesses apart
// from the strings.
#include "game/common.h"
#include "macros.h"

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

struct EVec {
    f32 x, y, z;
};

struct DMat3 {
    DMat3(void) {}
    f32 m[9];
};

// A part's transform (rotation + position).
struct DFrame {
    DMat3 rot;  // offset 0x0
    EVec pos;   // offset 0x24
};

// One entry of the damageable part table.
struct DamagePart {
    s32 kind;          // offset 0x0
    const char* name;  // offset 0x4
    s32 bit;           // offset 0x8, the part model visibility bit
    EVec axis;         // offset 0xC, hinge axis
    EVec pivot;        // offset 0x18
    EVec anchor;       // offset 0x24
    f32 size;          // offset 0x30
    s32 x34;           // offset 0x34
    s32 x38;           // offset 0x38
    s32 mirror;        // offset 0x3C
};

// The damageable parts: name, model visibility bit, hinge axis (set up by the initialiser), pivot and anchor.
DamagePart lbl_80332840[17] = {
    {0, "right hood hinge", 3, {0.0f, 0.0f, 0.0f}, {-0.508f, 0.1016f, 0.508f}, {-0.508f, 0.1016f, 1.5239999f}, 0.127f, 0, 0, 0},
    {1, "right decklid hinge", 4, {0.0f, 0.0f, 0.0f}, {-0.508f, -0.2032f, -0.508f}, {-0.508f, -0.2032f, -1.5239999f}, 0.0381f, 0, 0, 0},
    {2, "left front bumper", 5, {0.0f, 0.0f, 0.0f}, {0.508f, 0.0f, 0.1016f}, {0.508f, 0.0f, 0.1016f}, 0.0762f, 0, 2, 0},
    {3, "left rear bumper", 6, {0.0f, 0.0f, 0.0f}, {0.508f, 0.0f, -0.1016f}, {0.508f, 0.0f, -0.1016f}, 0.0762f, 0, 0, 0},
    {4, "left front flare", 7, {0.0f, 0.0f, 0.0f}, {0.0762f, 0.254f, 0.6096f}, {0.0762f, 0.254f, 0.6096f}, 0.0762f, 0, 0, 0},
    {5, "right front flare", 8, {0.0f, 0.0f, 0.0f}, {-0.0762f, 0.254f, 0.6096f}, {-0.0762f, 0.254f, 0.6096f}, 0.0762f, 0, 0, 0},
    {6, "left rear flare", 9, {0.0f, 0.0f, 0.0f}, {0.0762f, 0.254f, 0.6096f}, {0.0762f, 0.254f, 0.6096f}, 0.0762f, 0, 0, 0},
    {7, "right rear flare", 10, {0.0f, 0.0f, 0.0f}, {-0.0762f, 0.254f, 0.6096f}, {-0.0762f, 0.254f, 0.6096f}, 0.0762f, 0, 0, 0},
    {8, "left side mirror", 19, {0.0f, 0.0f, 0.0f}, {-0.0762f, 0.254f, -0.508f}, {-0.0762f, 0.254f, -0.508f}, 0.0762f, 0, 0, 1},
    {9, "right side mirror", 20, {0.0f, 0.0f, 0.0f}, {0.0762f, 0.254f, -0.508f}, {0.0762f, 0.254f, -0.508f}, 0.0762f, 0, 0, 1},
    {10, "item 1", 24, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0762f, 0, 2, 0},
    {11, "item 2", 25, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0762f, 0, 2, 0},
    {12, "item 3", 26, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0762f, 0, 2, 0},
    {13, "item 4", 27, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0762f, 0, 2, 0},
    {14, "left wiper", 21, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 10000.0f, 0, 0, 0},
    {15, "right wiper", 22, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 10000.0f, 0, 0, 0},
    {16, "rear wiper", 23, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 10000.0f, 0, 0, 0},
};

// The breakable lights and windows (and the wheel and hood-latch helpers): name, part index, hit radius, body
// point (-1: none).
struct LightPart {
    const char* name;  // offset 0x0
    s32 index;         // offset 0x4
    f32 radius;        // offset 0x8
    s32 point;         // offset 0xC
};

LightPart lbl_80332D08[13] = {
    {"left headlight", 2, 0.0254f, 15},
    {"right headlight", 3, 0.0254f, 16},
    {"left taillight", 0, 0.0254f, 17},
    {"right taillight", 1, 0.0254f, 18},
    {"front window", 4, 0.0254f, 11},
    {"left window", 5, 0.0254f, 13},
    {"right window", 6, 0.0254f, 14},
    {"rear window", 7, 0.0254f, 12},
    {"$W0", 8, 0.2032f, -1},
    {"$W1", 9, 0.2032f, -1},
    {"$W2", 10, 0.2032f, -1},
    {"$W3", 11, 0.2032f, -1},
    {"hood latch", 12, 0.0254f, -1},
};

extern "C" f32 Dot(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz);

static inline void MulM(DMat3* out, const DMat3* a, const DMat3* b) {
    if (out == b || out == a) {
        f32 m0 = Dot(a->m[0], b->m[0], a->m[3], b->m[1], a->m[6], b->m[2]);
        f32 m3 = Dot(a->m[0], b->m[3], a->m[3], b->m[4], a->m[6], b->m[5]);
        f32 m6 = Dot(a->m[0], b->m[6], a->m[3], b->m[7], a->m[6], b->m[8]);
        f32 m1 = Dot(a->m[1], b->m[0], a->m[4], b->m[1], a->m[7], b->m[2]);
        f32 m4 = Dot(a->m[1], b->m[3], a->m[4], b->m[4], a->m[7], b->m[5]);
        f32 m7 = Dot(a->m[1], b->m[6], a->m[4], b->m[7], a->m[7], b->m[8]);
        f32 m2 = Dot(a->m[2], b->m[0], a->m[5], b->m[1], a->m[8], b->m[2]);
        f32 m5 = Dot(a->m[2], b->m[3], a->m[5], b->m[4], a->m[8], b->m[5]);
        f32 m8 = Dot(a->m[2], b->m[6], a->m[5], b->m[7], a->m[8], b->m[8]);
        out->m[0] = m0;
        out->m[3] = m3;
        out->m[6] = m6;
        out->m[1] = m1;
        out->m[4] = m4;
        out->m[7] = m7;
        out->m[2] = m2;
        out->m[5] = m5;
        out->m[8] = m8;
    } else {
        out->m[0] = Dot(a->m[0], b->m[0], a->m[3], b->m[1], a->m[6], b->m[2]);
        out->m[3] = Dot(a->m[0], b->m[3], a->m[3], b->m[4], a->m[6], b->m[5]);
        out->m[6] = Dot(a->m[0], b->m[6], a->m[3], b->m[7], a->m[6], b->m[8]);
        out->m[1] = Dot(a->m[1], b->m[0], a->m[4], b->m[1], a->m[7], b->m[2]);
        out->m[4] = Dot(a->m[1], b->m[3], a->m[4], b->m[4], a->m[7], b->m[5]);
        out->m[7] = Dot(a->m[1], b->m[6], a->m[4], b->m[7], a->m[7], b->m[8]);
        out->m[2] = Dot(a->m[2], b->m[0], a->m[5], b->m[1], a->m[8], b->m[2]);
        out->m[5] = Dot(a->m[2], b->m[3], a->m[5], b->m[4], a->m[8], b->m[5]);
        out->m[8] = Dot(a->m[2], b->m[6], a->m[5], b->m[7], a->m[8], b->m[8]);
    }
}

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

static inline void MulVec(EVec* out, const EVec* in, const DMat3* m) {
    if (out == in) {
        f32 x = Dot3(in->x, m->m[0], in->y, m->m[3], in->z, m->m[6]);
        f32 y = Dot3(in->x, m->m[1], in->y, m->m[4], in->z, m->m[7]);
        f32 z = Dot3(in->x, m->m[2], in->y, m->m[5], in->z, m->m[8]);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, m->m[0], in->y, m->m[3], in->z, m->m[6]);
        out->y = Dot3(in->x, m->m[1], in->y, m->m[4], in->z, m->m[7]);
        out->z = Dot3(in->x, m->m[2], in->y, m->m[5], in->z, m->m[8]);
    }
}

// A detachable part's live state (0x80 bytes from car + 0x152C).
struct DmgSlot {
    DFrame* on;     // offset 0x0, the part's bone frame (0: no such part)
    s32 point;      // offset 0x4
    s32 hub;        // offset 0x8, body point the hinge rides on
    f32 angle;      // offset 0xC
    f32 spin;       // offset 0x10
    f32 limit;      // offset 0x14
    DFrame cur;     // offset 0x18
    DFrame prev;    // offset 0x48
    u8 valid;       // offset 0x78
    s32 state;      // offset 0x7C: 0 attached, 1 hanging, 2/4 flying, 3 gone
};

// A point of frame f in its parent's space.
static inline void ToWorld(EVec* out, const EVec* in, const DFrame* f) {
    MulVec(out, in, &f->rot);
    out->x += f->pos.x;
    out->y += f->pos.y;
    out->z += f->pos.z;
}

extern "C" {

// Rotates frame a by b's rotation about b's position: out = a * b, keeping b's pivot where a puts it.
void fn_8018BFA0(DFrame* out, const DFrame* a, const DFrame* b) {
    f32 v[3];
    MulM(&out->rot, &a->rot, &b->rot);
    MulVec((EVec*)v, &b->pos, &a->rot);
    out->pos.x = a->pos.x + v[0];
    out->pos.y = a->pos.y + v[1];
    out->pos.z = a->pos.z + v[2];
    MulVec((EVec*)v, &b->pos, &out->rot);
    out->pos.x = out->pos.x - v[0];
    out->pos.y = out->pos.y - v[1];
    out->pos.z = out->pos.z - v[2];
}

void fn_8018C50C(u8* p) {}

void fn_8018C510(u8* p) {}

f32 PSVECDistance(const f32* a, const f32* b);
extern const f32 lbl_80561F70;  // large distance

// Index of the damage point nearest to pos (entry 0 is skipped).
int fn_8018C514(u8* p, const f32* pos) {
    f32 best = lbl_80561F70;
    int idx = 0;
    for (int i = 1; i < *(s32*)PTR(p, 0x26C); i++) {
        f32 d = PSVECDistance(pos, (f32*)(PTR(p, 0x26C) + i * 0x34 + 0x20));
        if (d < best) {
            best = d;
            idx = i;
        }
    }
    return idx;
}

}  // extern "C"

// The car's paint/flag state block (carstate.cpp).
struct CarColors {
    u8 c[164][4];
};

struct CarFlags {
    u8 f[18];
};

struct CarState {
    CarColors col;   // offset 0x0
    f32 time;        // offset 0x290
    f32 duration;    // offset 0x294
    s32 x298;        // offset 0x298
    s32 x29C;        // offset 0x29C
    CarFlags flags;  // offset 0x2A0
};

void* GetEvent(int event);

struct EventDesc {
    u8 pad[0x20];
    ValueNode* table;  // offset 0x20
};

extern "C" {
void* memset(void* p, int c, unsigned long n);
int fn_801E2CCC(s32 model, const char* name, DFrame** bone);
void fn_802E317C(const char* fmt, ...);
void fn_80279B28(u8* block, CarState* out);
void fn_80279CD8(CarState* s);
u8* fn_801769CC(void);
}
extern const f32 lbl_80561F74;  // 0.0
extern const f32 lbl_80561F78;  // pi/2
extern const f32 lbl_80561F7C;  // 1818.0
extern const f32 lbl_80561F80;  // 0.4545
extern const f32 lbl_80561F84;  // -0.0001
extern const f32 lbl_80561F88;  // 1.0
extern const f32 lbl_80561F8C;  // 0.5

extern "C" {

// Sets the car's damage up: finds each part's bone and the body points it rides on (its anchor and its
// origin), the dent stiffness from the car's mass and bounce (scaled by the event's player_damage_tweak),
// the paint state, and the body point each light, window and helper sits nearest to.
void fn_8018C5A0(u8* p, u8* desc) {
    memset(p + 0x152C, 0, 0x900);
    DamagePart* d = lbl_80332840;
    DmgSlot* s = (DmgSlot*)(p + 0x152C);
    for (int i = 0; i < 17; i++, d++, s++) {
        if (fn_801E2CCC(S32(p, 0x59C), d->name, &s->on)) {
            EVec v;
            ToWorld(&v, &d->anchor, s->on);
            s->point = fn_8018C514(p, &v.x);
            EVec o;
            o.x = lbl_80561F74;
            o.y = lbl_80561F74;
            o.z = lbl_80561F74;
            ToWorld(&v, &o, s->on);
            s->hub = fn_8018C514(p, &v.x);
            s->limit = lbl_80561F78;
        }
    }
    S32(p, 0x5F8) = *(s32*)PTR(p, 0x26C);
    F32(p, 0x5F4) = lbl_80561F7C / (lbl_80561F80 * F32(desc, 0x8));
    if (F32(p, 0x314) >= lbl_80561F84) {
        F32(p, 0x5F4) = F32(p, 0x5F4) * F32(p, 0x314);
    } else {
        F32(p, 0x5F4) = (f32)__fabs(F32(p, 0x314));
    }
    if (S32(p, 0x1F0) == 0x50) {
        ValueNode* t = ((EventDesc*)GetEvent(*(s32*)fn_801769CC()))->table;
        F32(p, 0x5F4) = F32(p, 0x5F4) * t->GetFloat("player_damage_tweak", lbl_80561F88);
    }
    F32(p, 0x5F4) = F32(p, 0x5F4) * lbl_80561F8C;
    if (PTR(p, 0x328)) {
        fn_80279B28(PTR(p, 0x328), (CarState*)(p + 0x1E44));
    } else {
        fn_80279CD8((CarState*)(p + 0x1E44));
    }
    *(CarState*)(p + 0x20F8) = *(CarState*)(p + 0x1E44);
    for (int i = 0; i < 16; i++) {
        U8(p, 0x1E2C + i) = 0xFF;
    }
    LightPart* l = lbl_80332D08;
    for (int i = 0; i < 13; i++, l++) {
        DFrame* bone = 0;
        u8* q = p + l->index;
        if (l->name[0] == '$') {
            if (l->name[1] == 'W') {
                q[0x1E2C] = fn_8018C514(p, (f32*)(p + (l->name[2] - '0') * 0x74 + 0x23DC));
            } else {
                fn_802E317C("cardmg: unknown special helper %s", l->name);
            }
        }
        if (fn_801E2CCC(S32(p, 0x59C), l->name, &bone)) {
            q[0x1E2C] = fn_8018C514(p, &bone->pos.x);
        }
    }
}

}  // extern "C"


extern "C" f32 PSVECDotProduct(const f32* a, const f32* b);

struct RVec3;
class RigidBody {
public:
    void VelAt(RVec3* out, const RVec3* at);
    void VelAt2(RVec3* out, const RVec3* at);
};

struct Mat3;
struct Vec3;
void Mat3FromEuler(Mat3* m, const Vec3* e);
void Mat3Mul(Mat3* out, const Mat3* a, const Mat3* b);

// A rotation plus position, filled from euler angles (modeinl.cpp).
struct Mtx33 {
    void SetEuler(f32 y, f32 x, f32 z);
    DMat3 rot;  // offset 0x0
    EVec pos;   // offset 0x24
};

extern "C" {
void PSVECCrossProduct(const f32* a, const f32* b, f32* out);
s32 fn_80129D18(int a, int b);
u8* fn_801769CC(void);
}
extern const f32 lbl_80561F74;  // 0.0
extern const f32 lbl_80561F88;  // 1.0
extern const f32 lbl_80561F90;  // 4.905
extern const f32 lbl_80561F94;  // 0.4
extern const f32 lbl_80561F98;  // 0.3
extern const f32 lbl_80561F9C;  // 5.0
extern const f32 lbl_80561FA0;  // -5.0
extern const f32 lbl_80561FA4;  // 0.0002
extern const f32 lbl_80561FA8;  // 1/59.94
extern const f32 lbl_80561FAC;  // 20.0
extern const f32 lbl_80561FB0;  // -20.0
extern const f32 lbl_80561FB4;  // 0.045
extern const f32 lbl_80561FB8;  // 2000.0
extern const f32 lbl_80561FBC;  // -0.045
extern const f32 lbl_80561FC0;  // 2.0
extern const f32 lbl_80561FC4;  // -1.0

extern "C" {

static inline f32 MinF(f32 v, f32 hi) { return hi < v ? hi : v; }
static inline f32 MaxF(f32 v, f32 lo) { return v > lo ? v : lo; }
static inline f32 ClampF(f32 v, f32 lo, f32 hi) { return MaxF(MinF(v, hi), lo); }
static inline f32 Lower(f32 a, f32 b) { return a < b ? a : b; }

static inline void Scale(EVec* out, const EVec* v, f32 k) {
    out->x = v->x * k;
    out->y = v->y * k;
    out->z = v->z * k;
}

// Turns m by the euler angles e (applied first).
static inline void TurnEuler(DMat3* m, const EVec* e) {
    DMat3 r;
    Mat3FromEuler((Mat3*)&r, (const Vec3*)e);
    Mat3Mul((Mat3*)m, (Mat3*)&r, (Mat3*)m);
}

// Swings the hinged parts (hood, decklid, ...). A hanging part (2) is driven by the torque of the
// wind and car motion about its hinge, damped and limited, and may snap shut and reattach; a bouncing
// part (4) swings freely between its stops. Both rebuild the part frame on the car each tick.
void fn_8018CB3C(u8* p) {
    DmgSlot* s = (DmgSlot*)(p + 0x152C);
    const DamagePart* d = lbl_80332840;
    for (int i = 0; i < 17; i++, s++, d++) {
        if (s->on && s->state == 2) {
            DFrame f = *s->on;
            EVec ang;
            Scale(&ang, &d->axis, s->angle);
            TurnEuler(&f.rot, &ang);
            EVec hinge;
            MulVec(&hinge, &d->pivot, &f.rot);
            hinge.x += f.pos.x;
            hinge.y += f.pos.y;
            hinge.z += f.pos.z;
            EVec at;
            MulVec(&at, &s->on->pos, (DMat3*)(p + 0x3C));
            at.x += F32(p, 0x60);
            at.y += F32(p, 0x64);
            at.z += F32(p, 0x68);
            EVec h;
            MulVec(&h, &hinge, (DMat3*)(p + 0x3C));
            h.x += F32(p, 0x60);
            h.y += F32(p, 0x64);
            h.z += F32(p, 0x68);
            EVec v1;
            ((RigidBody*)p)->VelAt((RVec3*)&v1, (RVec3*)&h);
            EVec v2;
            ((RigidBody*)p)->VelAt2((RVec3*)&v2, (RVec3*)&at);
            EVec arm;
            arm.x = h.x - at.x;
            arm.y = h.y - at.y;
            arm.z = h.z - at.z;
            v2.y += lbl_80561F90;
            Scale(&v2, &v2, lbl_80561F94);
            if (fn_80129D18(4, 4) < 3) {
                v2.x += v1.x * lbl_80561F98;
                v2.y += v1.y * lbl_80561F98;
                v2.z += v1.z * lbl_80561F98;
            }
            EVec torque;
            PSVECCrossProduct(&arm.x, &v2.x, &torque.x);
            EVec ax;
            MulVec(&ax, &d->axis, &s->on->rot);
            EVec axw;
            MulVec(&axw, &ax, (DMat3*)(p + 0x3C));
            f32 t = -PSVECDotProduct(&torque.x, &axw.x);
            t = ClampF(t, lbl_80561FA0, lbl_80561F9C);
            f32 w = s->spin + t;
            s->spin = w - lbl_80561FA4 * (lbl_80561FA8 * w);
            s->spin = ClampF(s->spin, lbl_80561FB0, lbl_80561FAC);
            if (s->angle > lbl_80561F88) {
                s->angle = lbl_80561F88;
                s->spin = Lower(s->spin, lbl_80561F74);
            } else if (s->angle < lbl_80561FB4) {
                if ((U32(fn_801769CC(), 0x4) & 0x4000) && s->spin < lbl_80561F74 && fn_80129D18(4, 10) < 1) {
                    s->angle = lbl_80561F74;
                    s->spin = lbl_80561F74;
                    s->state = 0;
                } else {
                    s->spin = s->spin + lbl_80561FA8 * (lbl_80561FB8 * (lbl_80561FB4 - s->angle));
                }
            }
            s->angle = s->angle + lbl_80561FA8 * s->spin;
            s->angle = MaxF(s->angle, lbl_80561FBC);
            Mtx33 m;
            m.pos = s->on->pos;
            EVec* hub = (EVec*)(p + 0xD94) + s->hub;
            m.pos.x += hub->x;
            m.pos.y += hub->y;
            m.pos.z += hub->z;
            EVec e;
            Scale(&e, &d->axis, s->angle);
            EVec er;
            MulVec(&er, &e, &s->on->rot);
            m.SetEuler(er.y, er.x, er.z);
            s->prev = s->cur;
            fn_8018BFA0(&s->cur, (DFrame*)(p + 0x70), (DFrame*)&m);
            if (!s->valid) {
                s->valid = 1;
                s->prev = s->cur;
            }
            s->spin = lbl_80561F74;
        } else if (s->on && s->state == 4) {
            s->angle = s->angle + lbl_80561FA8 * s->spin;
            if (s->angle < lbl_80561F74 || s->angle > s->limit) {
                s->angle = lbl_80561FC0 * ClampF(s->angle, lbl_80561F74, s->limit) - s->angle;
                s->spin = -s->spin;
            }
            Mtx33 m;
            m.pos = s->on->pos;
            EVec* hub = (EVec*)(p + 0xD94) + s->hub;
            m.pos.x += hub->x;
            m.pos.y += hub->y;
            m.pos.z += hub->z;
            EVec e;
            Scale(&e, &d->axis, s->angle);
            EVec er;
            MulVec(&er, &e, &s->on->rot);
            m.SetEuler(er.y, er.x, er.z);
            s->prev = s->cur;
            if (U32(fn_801769CC(), 0x10) & 2) {
                DFrame cf = *(DFrame*)(p + 0x70);
                cf.rot.m[0] *= lbl_80561FC4;
                cf.rot.m[1] *= lbl_80561FC4;
                cf.rot.m[2] *= lbl_80561FC4;
                fn_8018BFA0(&s->cur, &cf, (DFrame*)&m);
                s->cur.rot.m[0] *= lbl_80561FC4;
                s->cur.rot.m[1] *= lbl_80561FC4;
                s->cur.rot.m[2] *= lbl_80561FC4;
            } else {
                fn_8018BFA0(&s->cur, (DFrame*)(p + 0x70), (DFrame*)&m);
            }
            if (!s->valid) {
                s->valid = 1;
                s->prev = s->cur;
            }
        }
    }
}


// A loose part handed to the renderer.
struct DmgDraw {
    DFrame frame;  // offset 0x0
    u32 mask;      // offset 0x30
};

// Views of the car (deformation points) and its draw model used here.
struct DmgCarView {
    u8 pad0[0x5F8];
    s32 nPoints;      // offset 0x5F8
    EVec goal[162];   // offset 0x5FC
    EVec ease[1];     // offset 0xD94
};

struct DmgPose {
    u8 pad0[0x2E0];
    EVec pt[1];  // offset 0x2E0
};

extern f32 lbl_8055E460;    // tick alpha
extern f32 lbl_80558DC4[];  // FLT_EPSILON

static inline void SetE(EVec* d, const EVec* s) {
    d->x = s->x;
    d->y = s->y;
    d->z = s->z;
}
static inline f32 DmgMax(f32 a, f32 b) { return a > b ? a : b; }
static inline f32 LerpF(f32 a, f32 b, f32 t) { return a + t * (b - a); }

// Fills the model's damage pose: eases the wheel offsets toward their targets, bends the body points by the
// lean/twist (when the deformation is on), hides the parts that are gone and lists the flying ones with their
// frame interpolated to the tick.
// MATCHING-AID: machine-extracted inline helper (loopext); it raised the score but nothing proves the original had it.
static inline void Loop1_fn_8018DA5C(DmgSlot* s, u8* m, DamagePart* tp, f32 t) {
    if (s->state == 3) {
        U32(m, 0x29C) &= ~(1 << tp->bit);
    }
    if (s->state == 2 || s->state == 4) {
        DmgDraw* d = (DmgDraw*)(m + 0xA7C) + S32(m, 0xA78)++;
        if ((f32)__fabs(t - 1.0f) <= lbl_80558DC4[0]) {
            d->frame = s->cur;
        } else {
            DFrame f;
            f32* o = (f32*)&f;
            f32* a = (f32*)&s->prev;
            f32* b = (f32*)&s->cur;
            for (int j = 0; j < 12; j++) {
                *o++ = LerpF(*a++, *b++, t);
            }
            d->frame = f;
        }
        d->mask = 1 << tp->bit;
    }
}

static inline void EaseV(EVec* e, const EVec* g, f32 a, f32 b) {
    e->x *= a;
    e->y *= a;
    e->z *= a;
    e->x += g->x * b;
    e->y += g->y * b;
    e->z += g->z * b;
}

static inline f32 AbsFlt(f32 v) { return (f32)__fabs(v); }

void fn_8018DA5C(u8* p, u8* m) {
    DmgCarView* car = (DmgCarView*)p;
    DmgPose* mdl = (DmgPose*)m;
    EVec* e = car->ease;
    for (int i = 0; i < car->nPoints; i++) {
        EaseV(&car->ease[i], &car->goal[i], 0.8f, 1.0f - 0.8f);
        SetE(&mdl->pt[i], e + i);
    }
    if (U8(p, 0x26E0)) {
        f32 lean = F32(p, 0x26E8);
        f32 twist = 0.2f * F32(p, 0x26E4);
        f32 roll = 0.1f * F32(p, 0x26EC);
        lean = lean > -0.25f ? lean : -0.25f;
        for (int i = 1; i < car->nPoints; i++) {
            u8* pt = PTR(p, 0x26C) + i * 0x34;
            EVec v;
            v.x = 0.0f;
            v.y = 0.0f;
            v.z = 0.0f;
            f32 z = F32(pt, 0x28);
            f32 az = (f32)__fabs(z);
            v.x += roll * (-0.3f * (F32(pt, 0x20) * (az - 0.7f)));
            v.y += roll * (-0.3f * (F32(pt, 0x24) * (az - 0.7f)));
            v.z += -0.5f * z * roll;
            f32 w = DmgMax(0.0f, 3.0f - az);
            v.y += 0.5f * lean * (F32(pt, 0x24) * w);
            v.x += twist * DmgMax(0.0f, 3.0f - AbsFlt(F32(pt, 0x28)));
            mdl->pt[i] = v;
        }
    }
    U32(m, 0x29C) |= U32(p, 0x1E40);
    DamagePart* tp = lbl_80332840;
    DmgSlot* s = (DmgSlot*)(p + 0x152C);
    f32 t = lbl_8055E460;
    S32(m, 0xA78) = 0;
    for (int i = 0; i < 17; i++) {
        if (s->on && s->state != 0 && s->state != 1) {
            Loop1_fn_8018DA5C(s, m, tp, t);
        }
        tp++;
        s++;
    }
}

// A deformable point of the car body (0x34 bytes, from the model at car + 0x26C).
struct DentVec {
    void AddScaled(const DentVec& v, f32 s) {
        x += v.x * s;
        y += v.y * s;
        z += v.z * s;
    }
    f32 x, y, z;
};

struct DentPoint {
    s32 unk0;     // offset 0x0
    s32 goal;     // offset 0x4: index into the car's spring goals (car + 0x5FC)
    DentVec lo;   // offset 0x8
    DentVec hi;   // offset 0x14
    DentVec pos;  // offset 0x20
    f32 unk2C;    // offset 0x2C
    f32 soft;     // offset 0x30
};

// A spring link between two dent points (0x14 bytes).
struct DentLink {
    s32 a;        // offset 0x0
    s32 b;        // offset 0x4
    f32 along;    // offset 0x8: share of the push passed along the link
    f32 across;   // offset 0xC: share of the push passed across it
    f32 lift;     // offset 0x10
};

static inline f32 DentClamp(f32 v, f32 lo, f32 hi) {
    v = hi < v ? hi : v;
    return v > lo ? v : lo;
}

f32 PSVECMag(const f32* v);
void PSVECNormalize(const f32* v, f32* out);
f32 PSVECDotProduct(const f32* a, const f32* b);

// Dents point i of the body by `push`: moves its spring goal (pushing out only along the point's outward
// direction is cancelled), clamps it to the point's box, then passes the push on to linked points (not back to
// `from`), up to three links deep.
void fn_8018DE94(u8* p, int i, const DentVec* push, int from, int depth) {
    DentPoint* pt = (DentPoint*)PTR(p, 0x26C) + i;
    f32 k = 10.0f * (0.0000068539325f * pt->soft);
    f32 mag = PSVECMag(&push->x);
    DentVec d;
    d.x = push->x * k;
    d.y = push->y * k;
    d.z = push->z * k;
    d.y = d.y < 0.0f ? d.y : 0.0f;
    DentVec* g = (DentVec*)(p + 0x5FC) + pt->goal;
    g->AddScaled(d, 2.0f);
    if (PSVECDotProduct(&g->x, &pt->pos.x) > 0.0f) {
        DentVec n = pt->pos;
        PSVECNormalize(&n.x, &n.x);
        f32 out = -PSVECDotProduct(&g->x, &n.x);
        g->x += n.x * out;
        g->y += n.y * out;
        g->z += n.z * out;
    }
    g->x = DentClamp(g->x, pt->lo.x, pt->hi.x);
    g->y = DentClamp(g->y, pt->lo.y, pt->hi.y);
    g->z = DentClamp(g->z, pt->lo.z, pt->hi.z);
    if (depth <= 2) {
        int other;
        DentVec* gi = (DentVec*)(p + 0x5FC) + i;
        DentVec at;
        at.x = pt->pos.x + gi->x;
        at.y = pt->pos.y + gi->y;
        at.z = pt->pos.z + gi->z;
        for (int n = 0; n < S32(PTR(p, 0x26C), 0x20F0); n++) {
            DentLink* l = (DentLink*)PTR(PTR(p, 0x26C), 0x20EC) + n;
            other = l->a;
            if ((other == i && l->b != from) || (l->b == i && other != from)) {
                if (other == i) {
                    other = l->b;
                }
                DentPoint* o = (DentPoint*)PTR(p, 0x26C) + other;
                DentVec* go = (DentVec*)(p + 0x5FC) + other;
                DentVec op;
                // MATCHING-AID: y read ahead of x (single-use reads); the target adds y first but stores x first.
                f32 yv = go->y;
                f32 y2 = o->pos.y;
                op.x = o->pos.x + go->x;
                op.y = y2 + yv;
                op.z = o->pos.z + go->z;
                DentVec dir;
                dir.x = op.x - at.x;
                dir.y = op.y - at.y;
                dir.z = op.z - at.z;
                f32 len = PSVECMag(&dir.x);
                DentVec f;
                if (len > 0.01f) {
                    f32 inv = 1.0f / len;
                    dir.x *= inv;
                    dir.y *= inv;
                    dir.z *= inv;
                    f32 t = -PSVECDotProduct(&dir.x, &push->x);
                    f.x = push->x + dir.x * t;
                    f.y = push->y + dir.y * t;
                    f.z = push->z + dir.z * t;
                    f32 c = l->across;
                    f.x *= c;
                    f.y *= c;
                    f.z *= c;
                    f32 s = l->along * PSVECDotProduct(&push->x, &dir.x);
                    f.x += dir.x * s;
                    f.y += dir.y * s;
                    f.z += dir.z * s;
                    DentVec up;
                    up.x = 0.0f;
                    up.y = 1.0f;
                    up.z = 0.0f;
                    f32 m = mag * l->lift;
                    f.x += up.x * m;
                    f.y += up.y * m;
                    f.z += up.z * m;
                } else {
                    f32 a = l->along;
                    f.x = push->x * a;
                    f.y = push->y * a;
                    f.z = push->z * a;
                }
                fn_8018DE94(p, other, &f, i, depth + 1);
            }
        }
    }
}

}  // extern "C"

// The running game mode (only the slot used here is named).
class GameMode {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(void);
    virtual void v1C(void);
    virtual void v20(void);
    virtual void v24(void);
    virtual void v28(void);
    virtual void v2C(void);
    virtual void v30(void);
    virtual void v34(void);
    virtual void v38(void);
    virtual void v3C(void);
    virtual void v40(void);
    virtual void v44(void);
    virtual void v48(void);
    virtual void v4C(void);
    virtual void v50(void);
    virtual void v54(void);
    virtual void v58(void);
    virtual void v5C(void);
    virtual void v60(void);
    virtual void v64(void);
    virtual void v68(void);
    virtual void v6C(void);
    virtual void v70(void);
    virtual void v74(void);
    virtual void v78(void);
    virtual void v7C(void);
    virtual void v80(void);
    virtual void v84(void);
    virtual void v88(void);
    virtual void v8C(void);
    virtual void v90(void);
    virtual void v94(void);
    virtual void v98(void);
    virtual void v9C(void);
    virtual void vA0(void);
    virtual void ResetCar(u8* car);  // 0xA4
};

class DmgModel {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual u8* Current(void);  // 0x10
    virtual void v14(void);
    virtual void v18(void);
    virtual void v1C(void);
    virtual void v20(void);
    virtual void v24(void);
    virtual void v28(void);
    virtual void v2C(void);
    virtual void Impair(s32 by);  // 0x30
    virtual void v34(void);
    virtual int Busy(void);       // 0x38
};

extern "C" {
GameMode* fn_801B83A0(void);
void fn_80161A38(int a, u8* car, int b);
void fn_801DFF1C(u8* car);
extern const f32 lbl_80561F74;
extern const f32 lbl_80561F8C;
extern f32 lbl_8055E3C8;

void fn_8018E330(u8* p) {
    fn_801B83A0()->ResetCar(p);
    fn_80161A38(0, PTR(p, 0x1EC), 0);
    fn_801DFF1C(PTR(p, 0x1EC));
    f32 zero = lbl_80561F74;
    f32 k = lbl_80561F8C;
    F32(p, 0x258C) = zero;
    F32(p, 0x2690) = k;
    F32(p, 0x25BC) = zero;
    F32(p, 0x26A0) = lbl_8055E3C8;
    S32(p, 0x26A4) = S32(((DmgModel*)PTR(p, 0x270))->Current(), 0x4);
    F32(p, 0x2588) = lbl_80561F74;
    F32(p, 0x25A0) = lbl_80561F74;
    U8(p, 0x259D) = 0;
    U8(p, 0x259C) = 0;
    S32(p, 0x2638) = 0;
    S32(p, 0x2644) = 0;
    F32(p, 0x2630) = lbl_80561F74;
    F32(p, 0x262C) = lbl_80561F74;
    F32(p, 0x2628) = lbl_80561F74;
}

struct DVec {
    f32 x, y, z;
};

// The car's spring goals, one per body point.
struct DentCar {
    u8 pad0[0x5FC];
    DentVec goal[1];  // offset 0x5FC
};

// A vector as the collision code passes it around (copied word by word).
struct AVec {
    f32 v[3];
};

// A body point viewed from its spring-goal index on (the point array starts 4 bytes into the model).
struct DentNode {
    s32 goal;    // offset 0x0
    DentVec lo;  // offset 0x4
    DentVec hi;  // offset 0x10
    DentVec pos; // offset 0x1C
    f32 unk28;   // offset 0x28
    f32 soft;    // offset 0x2C
    u8 fixed;    // offset 0x30: never dented
    u8 pad31[0x3];
};

struct BoneBox {
    DVec lo;  // offset 0x0
    DVec hi;  // offset 0xC
};

// Car model (partial).
struct CarModel {
    u8 pad0[0x2138];
    BoneBox boxes[1];  // offset 0x2138, one per bone
};

// A breaking-glass event: where, how fast, which car.
struct GlassMsg {
    EVec pos;  // offset 0x0
    EVec vel;  // offset 0xC
    s16 car;   // offset 0x18
};

void fn_801BF2A4(u8* car, u32 mask, DVec* center, DVec* lo, DVec* hi, s32 kind, u8* fx);
extern const f32 lbl_80561FE4;  // -0.5
extern const f32 lbl_80561F88;  // 1.0
extern const f32 lbl_80561F98;  // 0.3
extern const f32 lbl_80561FC4;  // -1
extern const f32 lbl_80561FD0;  // 0.2
extern const f32 lbl_80561FD4;  // 0.1
extern const f32 lbl_80561FE8;  // 3
extern const f32 lbl_80561FEC;  // 10
extern const f32 lbl_80561FF4;  // 0.01
extern const f32 lbl_80561FF8;  // -0.1
extern const f32 lbl_80561FFC;  // 0.001
extern const f32 lbl_80562000;  // 1500
extern const f32 lbl_80562004;  // 15000
extern const f32 lbl_80562008;  // 30000
extern const f32 lbl_8056200C;  // 13.333333
extern const f32 lbl_80562010;  // 300000
extern const f32 lbl_80562014;  // 100000
extern const f32 lbl_80562018;  // 1.8
extern const f32 lbl_8056201C;  // 2.25
extern const f32 lbl_80562020;  // 300
extern const f32 lbl_80562024;  // 0.35
extern const f32 lbl_80562028;  // 1/150000
extern const f32 lbl_8056202C;  // 0.6
extern const f32 lbl_80562030;  // 0.03
extern const f32 lbl_80562034;  // 0.9
extern const f32 lbl_8055A460;  // 4
extern f32 lbl_80558DC4[];      // FLT_EPSILON
int fn_80189BDC(u8* p);
u8* fn_8017960C(s32 car);
void fn_8018A174(u8* p);
void fn_801796CC(int kind, void* msg, int size);
void fn_801B8610(s32 car, s32 by);
void fn_8018F76C(u8* p, int i);
void fn_802E30C8(const char* fmt, ...);
f32 PSVECSquareDistance(const f32* a, const f32* b);

static inline int NearZero(f32 x) { return (f32)__fabs(x) <= lbl_80558DC4[0]; }
static inline int Boosting(const u8* car) { return F32(car, 0x25BC) > lbl_80558DC4[0]; }
static inline int HitTooSoon(const u8* car) { return lbl_8055E3C8 < lbl_80561FE8 + F32(car, 0x26D8); }

// Which corner of the car a body-space point is nearest: 0/1 front left/right, 3/2 rear left/right.
static inline int HitCorner(const AVec* at) {
    return at->v[2] > lbl_80561F74 ? !(at->v[0] < lbl_80561F74) : (at->v[0] < lbl_80561F74 ? 3 : 2);
}

// A collision impulse `imp` at body point `at` (negated when `flip`) from car `other` (-1: the world) of
// surface kind `kind`: accumulates the impulse, decides whether the hit is hard enough to crash the car, dents
// the nearest body point and spreads the dent, breaks off parts and lights pushed past their limits, and
// raises the car's impairment.
static inline double AbsDbl(double v) { return __fabs(v); }

void fn_8018E3EC(u8* p, const AVec* imp, const AVec* at, int flip, s32 other, int kind) {
    GlassMsg msg;
    AVec di;
    AVec da;
    AVec h;
    DentVec cand;
    AVec g0;
    DentVec nd;
    DentVec dg;
    DentVec c0;
    DentVec w;
    DentVec r;
    DVec c;
    f32 inv = lbl_80561FF8 / F32(p, 0xD4);
    F32(p, 0x26F0) += imp->v[0] * inv;
    F32(p, 0x26F4) += imp->v[1] * inv;
    F32(p, 0x26F8) += imp->v[2] * inv;
    if (!fn_80189BDC(p)) {
        return;
    }
    if (NearZero(F32(p, 0x2690))) {
        if ((F32(p, 0x4C) < lbl_80561F74 && kind != 0x6D) || kind == 0x3E7) {
            fn_8018E330(p);
        }
    }
    if (kind == 0x73) {
        int fast = U8(p, 0x25C8) || Boosting(p);
        if (fast) {
            return;
        }
    }
    if (kind == 0x74) {
        return;
    }
    di = *imp;
    if (flip) {
        di.v[0] *= lbl_80561FC4;
        di.v[1] *= lbl_80561FC4;
        di.v[2] *= lbl_80561FC4;
    }
    da = *at;
    f32 mag = PSVECMag(di.v);
    if (!(U32(fn_801769CC(), 0x4) & 0x4000) && F32(p, 0x2690) < lbl_80561FFC) {
        if (mag < lbl_80562000) {
            return;
        }
        if (HitTooSoon(p)) {
            return;
        }
        if (other >= 0 && Boosting(p)) {
            return;
        }
        h = di;
        h.v[1] = h.v[1] > lbl_80561F74 ? lbl_80561F74 : h.v[1];
        f32 hm = PSVECMag(h.v);
        if (other >= 0) {
            hm *= lbl_80561F98;
            if (S32(p, 0x1F0) != 0x50 && !Boosting(p)) {
                if (Boosting(fn_8017960C(other)) || hm > lbl_80562004) {
                    if (F32(p, 0x25D4) < F32(fn_8017960C(other), 0x25D4)) {
                        fn_8018A174(p);
                    }
                }
            }
        } else if (kind == 0x73) {
            hm *= lbl_8055A460;
        }
        if (other < 0 || !Boosting(fn_8017960C(other))) {
            if (hm < lbl_80562008) {
                return;
            }
            if (other < 0 && F32(p, 0x25D4) < lbl_8056200C && hm < lbl_80562010) {
                return;
            }
        }
        if (U32(fn_801769CC(), 0x4) == 0x180304 && S32(p, 0x1F0) == 0x50 && other >= 0) {
            return;
        }
        fn_8018E330(p);
    }
    DentNode* best = 0;
    f32 bestD = lbl_80562014;
    for (int i = 1; i < S32(p, 0x5F8); i++) {
        DentNode* d = (DentNode*)(PTR(p, 0x26C) + 4) + i;
        if (!d->fixed) {
            cand.x = d->pos.x + ((DentCar*)p)->goal[i].x * lbl_80562018;
            cand.y = d->pos.y + ((DentCar*)p)->goal[i].y * lbl_80562018;
            cand.z = d->pos.z + ((DentCar*)p)->goal[i].z * lbl_80562018;
            f32 dd = PSVECSquareDistance(&cand.x, da.v);
            if (dd < lbl_8056201C && dd < bestD) {
                best = d;
                bestD = dd;
            }
        }
    }
    if (best) {
        g0 = *(AVec*)((DentVec*)(p + 0x5FC) + best->goal);
        fn_8018DE94(p, best->goal, (DentVec*)&di, -1, 0);
        nd.x = di.v[0] * lbl_80561FC4;
        nd.y = di.v[1] * lbl_80561FC4;
        nd.z = di.v[2] * lbl_80561FC4;
        PSVECNormalize(&nd.x, &nd.x);
        DentVec* gb = (DentVec*)(p + 0x5FC) + best->goal;
        dg.x = g0.v[0] - gb->x;
        dg.y = g0.v[1] - gb->y;
        dg.z = g0.v[2] - gb->z;
        f32 moved = (f32)AbsDbl(PSVECDotProduct(&nd.x, &dg.x));
        gb = (DentVec*)(p + 0x5FC) + best->goal;
        c0.x = best->pos.x + gb->x;
        c0.y = best->pos.y + gb->y;
        c0.z = best->pos.z + gb->z;
        int count = S32(p, 0x5F8);
        for (int i = 1; i < count; i++) {
            DentPoint* q = (DentPoint*)PTR(p, 0x26C) + i;
            DentVec* g = (DentVec*)(p + 0x5FC) + q->goal;
            w.x = q->pos.x + g->x;
            w.y = q->pos.y + g->y;
            w.z = q->pos.z + g->z;
            r.x = w.x - c0.x;
            r.y = w.y - c0.y;
            r.z = w.z - c0.z;
            f32 t = PSVECDotProduct(&nd.x, &r.x);
            f32 a = t < moved ? t : moved;
            if (a > lbl_80561F74) {
                f32 r2 = PSVECDotProduct(&r.x, &r.x);
                if (lbl_80561FD4 > r2) {
                    r2 = lbl_80561FD4;
                }
                f32 s = lbl_80561FD0 / r2;
                if (lbl_80561F88 < s) {
                    s = lbl_80561F88;
                }
                f32 m = -a * s;
                g->x += nd.x * m;
                g->y += nd.y * m;
                g->z += nd.z * m;
                g->x = DentClamp(g->x, q->lo.x, q->hi.x);
                g->y = DentClamp(g->y, q->lo.y, q->hi.y);
                g->z = DentClamp(g->z, q->lo.z, q->hi.z);
            }
        }
        LightPart* l;
        DmgSlot* sl = (DmgSlot*)(p + 0x152C);
        DamagePart* t = lbl_80332840;
        for (int i = 0; i < 17; i++) {
            if ((sl->on && sl->point > 0 && !sl->state) || sl->state == 2) {
                DentVec* g = (DentVec*)(p + 0x5FC) + sl->point;
                if (PSVECDotProduct(&g->x, &g->x) > t->size * t->size) {
                    if (!((f32)__fabs(PSVECDotProduct(&t->axis.x, &t->axis.x)) <= lbl_80558DC4[0])) {
                        if ((u32)t->kind <= 1) {
                            if (sl->state != 2) {
                                sl->state = 2;
                                sl->valid = 0;
                            }
                        } else {
                            int roll = fn_80129D18(4, 10);
                            if (sl->state == 2) {
                                if (roll < 2) {
                                    sl->state = 3;
                                    sl->valid = 0;
                                }
                            } else {
                                if (roll < 7) {
                                    sl->state = 2;
                                } else if (roll < 8) {
                                    sl->state = 3;
                                } else {
                                    sl->state = 1;
                                }
                                sl->valid = 0;
                            }
                            if (sl->state == 2) {
                                sl->angle = sl->angle > lbl_80561FF4 ? sl->angle : lbl_80561FF4;
                                sl->spin = sl->spin > lbl_80561FD0 ? sl->spin : lbl_80561FD0;
                            }
                        }
                    } else {
                        sl->state = 3;
                        sl->valid = 0;
                    }
                    if (sl->state == 3) {
                        DVec* lo = &((CarModel*)PTR(p, 0x26C))->boxes[t->bit].lo;
                        DVec* hi = lo + 1;
                        c.x = lo->x + (lo + 1)->x;
                        c.y = lo->y + hi->y;
                        c.z = lo->z + (lo + 1)->z;
                        c.x *= lbl_80561FE4;
                        c.y *= lbl_80561FE4;
                        c.z *= lbl_80561FE4;
                        fn_801BF2A4(p, 1 << t->bit, &c, lo, lo + 1, t->x34, (u8*)t + 0x38);
                    }
                }
            }
            sl++;
            t++;
        }
        l = lbl_80332D08;
        for (int i = 0; i < 13; i++) {
            int idx = l->index;
            u32 bit = 1 << idx;
            u8 pt = U8(p, 0x1E2C + idx);
            if (pt != 0xFF && !(U32(p, 0x1E3C) & bit)) {
                DentVec* g = (DentVec*)(p + 0x5FC) + pt;
                if (PSVECDotProduct(&g->x, &g->x) > l->radius * l->radius) {
                    U32(p, 0x1E3C) |= bit;
                    if (l->point > 0) {
                        U32(p, 0x1E40) |= 1 << l->point;
                    }
                    if (idx >= 2 && idx <= 7) {
                        DentPoint* q = (DentPoint*)PTR(p, 0x26C) + U8(p, 0x1E2C + idx);
                        ToWorld(&msg.pos, (EVec*)&q->pos, (DFrame*)(p + 0x3C));
                        ((RigidBody*)p)->VelAt((RVec3*)&msg.vel, (RVec3*)&msg.pos);
                        msg.car = *(s16*)(p + 0x6C);
                        fn_801796CC(7, &msg, sizeof(GlassMsg));
                    }
                    if (idx >= 8 && idx <= 11 && fn_80129D18(4, 10) < 5) {
                        fn_8018F76C(p, idx - 8);
                    }
                    if (idx == 12) {
                        if (fn_80129D18(4, 10) < 5) {
                            S32(p, 0x268C) = 6;
                        } else {
                            S32(p, 0x268C) = 3;
                        }
                        DmgSlot* hood = (DmgSlot*)(p + 0x152C);
                        if (hood->state != 3) {
                            hood->state = 2;
                            hood->valid = 0;
                            hood->spin = lbl_80561FEC;
                            hood->angle = hood->angle > lbl_80561F8C ? hood->angle : lbl_80561F8C;
                        }
                    }
                }
            }
            l++;
        }
    }
    f32 was = F32(p, 0x2690);
    if (U32(fn_801769CC(), 0x4) & 0x4000) {
        int hit = 0;
        s32 st = S32(p, 0x268C);
        if (st == 0) {
            if (mag > lbl_80562020) {
                f32 cap = kind != 0x70 ? lbl_80561FD4 : lbl_80562024;
                f32 add = lbl_80562028 * mag;
                add = add < cap ? add : cap;
                f32 v = F32(p, 0x2690) += add;
                if (v > lbl_8056202C || (v > lbl_80561F8C && add > lbl_80562030)) {
                    if (fn_80129D18(4, 2) == 0) {
                        S32(p, 0x268C) = 2;
                    } else {
                        S32(p, 0x268C) = 3;
                    }
                    if (S32(p, 0x26B0) < 0 && fn_80129D18(4, 6) == 0) {
                        S32(p, 0x26B0) = HitCorner(&da);
                    }
                }
            }
        } else if (st >= 1 && st <= 4) {
            if (mag > lbl_80562020) {
                f32 cap = kind != 0x70 ? lbl_80561FD4 : lbl_80562024;
                f32 add = lbl_80562028 * mag;
                add = add < cap ? add : cap;
                f32 v = F32(p, 0x2690) += add;
                if (kind == 0x70 && other == S32(p, 0x1EC) && v > lbl_80562034) {
                    F32(p, 0x2690) = was > lbl_80562034 ? was : lbl_80562034;
                }
                if (F32(p, 0x2690) > lbl_80561F88) {
                    F32(p, 0x2690) = lbl_80561F88;
                    switch (fn_80129D18(4, 3)) {
                    case 0:
                        S32(p, 0x268C) = 10;
                        break;
                    case 1:
                        S32(p, 0x268C) = 11;
                        break;
                    default:
                        S32(p, 0x268C) = 12;
                        break;
                    }
                    if (fn_80129D18(4, 4) == 0) {
                        int corner = HitCorner(&da);
                        if (!(U32(p + corner * 0x74, 0x241C) & 8)) {
                            fn_8018F76C(p, corner);
                        }
                    }
                    fn_802E30C8("car %d: impaired", S32(p, 0x1EC));
                    hit = 1;
                    fn_801B8610(S32(p, 0x1EC), other);
                }
            }
        } else if (st >= 9) {
            if (!((DmgModel*)PTR(p, 0x270))->Busy()) {
                fn_802E30C8("car %d: re-impairing?", S32(p, 0x1EC));
                hit = 1;
            }
        }
        if (hit) {
            ((DmgModel*)PTR(p, 0x270))->Impair(other);
        }
    }
    F32(p, 0x269C) = lbl_80561F74;
}


}  // extern "C"

extern const f32 lbl_80562038;  // 255
extern const f32 lbl_80561F88;  // 1.0
extern const f32 lbl_8056203C;  // -0.508
extern const f32 lbl_80562040;  // 1.016
static inline f32 FMin(f32 a, f32 b) { return b < a ? b : a; }
static inline f32 FMax(f32 a, f32 b) { return a > b ? a : b; }

// A dent offset (-0.508..0.508) packed into a byte.
static inline u8 PackDent(f32 v) {
    f32 t = FMax(FMin((v - -0.508f) / 1.016f, 1.0f), lbl_80561F74);
    return lbl_80561F74 + t * lbl_80562038 + 0.5f;
}

#define DMG_MIN(a, b) ((a) < (b) ? (a) : (b))

// A packed dent byte back to its offset.
static inline f32 UnpackDent(u8 b) {
    f32 t = DMG_MIN(lbl_80561F88, (b - lbl_80561F74) / lbl_80562038);
    t = FMax(t, lbl_80561F74);
    return lbl_8056203C + t * lbl_80562040;
}

extern "C" {

// Restores the damage state saved by fn_8018F538.
void fn_8018F2C0(u8* p, u8* q) {
    f32 h = F32(q, 0x290);
    F32(p, 0x2690) = h;
    F32(p, 0x2694) = h;
    F32(p, 0x2698) = F32(q, 0x294);
    S32(p, 0x268C) = S32(q, 0x298);
    u8* o = q;
    u8* d = p;
    for (int i = 0; i < S32(p, 0x5F8); o += 4, i++, d += 0xC) {
        F32(d, 0x5FC) = UnpackDent(o[0]);
        F32(d, 0x600) = UnpackDent(o[1]);
        F32(d, 0x604) = UnpackDent(o[2]);
        F32(d, 0xD94) = F32(d, 0x5FC);
        F32(d, 0xD98) = F32(d, 0x600);
        F32(d, 0xD9C) = F32(d, 0x604);
    }
    S32(p, 0x15A8) = U8(q, 0x2A0);
    U8(p, 0x15A4) = 0;
    S32(p, 0x1628) = U8(q, 0x2A1);
    U8(p, 0x1624) = 0;
    S32(p, 0x16A8) = U8(q, 0x2A2);
    U8(p, 0x16A4) = 0;
    S32(p, 0x1728) = U8(q, 0x2A3);
    U8(p, 0x1724) = 0;
    S32(p, 0x17A8) = U8(q, 0x2A4);
    U8(p, 0x17A4) = 0;
    S32(p, 0x1828) = U8(q, 0x2A5);
    U8(p, 0x1824) = 0;
    S32(p, 0x18A8) = U8(q, 0x2A6);
    U8(p, 0x18A4) = 0;
    S32(p, 0x1928) = U8(q, 0x2A7);
    U8(p, 0x1924) = 0;
    S32(p, 0x19A8) = U8(q, 0x2A8);
    U8(p, 0x19A4) = 0;
    S32(p, 0x1A28) = U8(q, 0x2A9);
    U8(p, 0x1A24) = 0;
    S32(p, 0x1AA8) = U8(q, 0x2AA);
    U8(p, 0x1AA4) = 0;
    S32(p, 0x1B28) = U8(q, 0x2AB);
    U8(p, 0x1B24) = 0;
    S32(p, 0x1BA8) = U8(q, 0x2AC);
    U8(p, 0x1BA4) = 0;
    S32(p, 0x1C28) = U8(q, 0x2AD);
    U8(p, 0x1C24) = 0;
    S32(p, 0x1CA8) = U8(q, 0x2AE);
    U8(p, 0x1CA4) = 0;
    S32(p, 0x1D28) = U8(q, 0x2AF);
    U8(p, 0x1D24) = 0;
    S32(p, 0x1DA8) = U8(q, 0x2B0);
    U8(p, 0x1DA4) = 0;
    S32(p, 0x1E3C) = S32(q, 0x29C);
}

}  // extern "C"

extern "C" {

// Saves the damage state (dents packed to bytes, part states) into a snapshot.
void fn_8018F538(u8* p, u8* q) {
    F32(q, 0x290) = F32(p, 0x2690);
    F32(q, 0x294) = F32(p, 0x2698);
    S32(q, 0x298) = S32(p, 0x268C);
    u8* d = p;
    u8* o = q;
    for (int i = 0; i < S32(p, 0x5F8); d += 0xC, i++, o += 4) {
        o[0] = PackDent(F32(d, 0x5FC));
        o[1] = PackDent(F32(d, 0x600));
        o[2] = PackDent(F32(d, 0x604));
    }
    U8(q, 0x2A0) = S32(p, 0x15A8);
    U8(q, 0x2A1) = S32(p, 0x1628);
    U8(q, 0x2A2) = S32(p, 0x16A8);
    U8(q, 0x2A3) = S32(p, 0x1728);
    U8(q, 0x2A4) = S32(p, 0x17A8);
    U8(q, 0x2A5) = S32(p, 0x1828);
    U8(q, 0x2A6) = S32(p, 0x18A8);
    U8(q, 0x2A7) = S32(p, 0x1928);
    U8(q, 0x2A8) = S32(p, 0x19A8);
    U8(q, 0x2A9) = S32(p, 0x1A28);
    U8(q, 0x2AA) = S32(p, 0x1AA8);
    U8(q, 0x2AB) = S32(p, 0x1B28);
    U8(q, 0x2AC) = S32(p, 0x1BA8);
    U8(q, 0x2AD) = S32(p, 0x1C28);
    U8(q, 0x2AE) = S32(p, 0x1CA8);
    U8(q, 0x2AF) = S32(p, 0x1D28);
    U8(q, 0x2B0) = S32(p, 0x1DA8);
    S32(q, 0x29C) = S32(p, 0x1E3C);
}

void fn_8018F6F8(u8* p) {
    DamagePart* t = lbl_80332840;
    u8* w = p + 0x152C;
    for (int i = 0; i < 17; i++) {
        if (U32(w, 0x0) && !S32(w, 0x7C) && (u32)(t->kind - 14) <= 2) {
            S32(w, 0x7C) = 4;
            U8(w, 0x78) = 0;
            F32(w, 0xC) = 0.0f;
            F32(w, 0x10) = F32(w, 0x14) / 0.75f;
        }
        t++;
        w += 0x80;
    }
}

}  // extern "C"

struct CVec4 {
    CVec4(void) {}
    CVec4(const CVec4& o) {
        x = o.x;
        y = o.y;
        z = o.z;
        w = o.w;
    }
    f32 x, y, z, w;
};

struct WVec3 {
    WVec3(void) {}
    WVec3(const WVec3& o) {
        x = o.x;
        y = o.y;
        z = o.z;
    }
    f32 x, y, z;
};

struct PVec3 {
    f32 x, y, z;
};

struct PFrame {
    PVec3 basis[3];
};

struct PColor {
    f32 r, g, b, a;
};

struct WFrame {
    PVec3 basis[3];
};

extern "C" {
void fn_802E30C8(const char* fmt, ...);
void fn_801BEB30(s32 car, u8** out);
void fn_801AFC44(s32 car, int wheel, WFrame* m, WVec3* vel, WVec3* imp);

// Knocks wheel i off the car: dims its channel in the car's cross-link colour, then launches it from
// the hub with the car's velocity plus its own spin.
void fn_8018F76C(u8* p, int i) {
    u8* w = p + i * 0x74;
    if (U32(w, 0x241C) & 8) {
        fn_802E30C8("cardmg: wheel %d is already missing from car %d", i, S32(p, 0x1EC));
    }
    u8* x;
    fn_801BEB30(S32(p, 0x1EC), &x);
    CVec4 c;
    PColor pc = *(PColor*)(x + 0x70);
    c = *(CVec4*)&pc;
    (&c.x)[i] *= 0.15f;
    *(CVec4*)(x + 0x70) = c;
    WFrame m = *(WFrame*)(p + 0x3C);
    WVec3 hub = *(WVec3*)(w + 0x23D0);
    hub.x += m.basis[1].x * 0.3f;
    hub.y += m.basis[1].y * 0.3f;
    f32 zv = m.basis[1].z;
    hub.z += zv * 0.3f;
    WVec3 vel;
    ((RigidBody*)p)->VelAt((RVec3*)&vel, (RVec3*)&hub);
    f32 s = F32(w, 0x23AC);
    if ((f32)__fabs(s) < lbl_80561F9C) {
        s = s < lbl_80561F74 ? lbl_80561FA0 : lbl_80561F9C;
    }
    f32 spin = F32(w, 0x23AC);
    WVec3 imp;
    imp.x = m.basis[0].x * spin + F32(p, 0x124);
    imp.y = m.basis[0].y * spin + F32(p, 0x128);
    imp.z = m.basis[0].z * spin + F32(p, 0x12C);
    fn_801AFC44(S32(p, 0x1EC), i, &m, &vel, &imp);
    U32(w, 0x241C) |= 8;
}

// Live state of one damageable part (17 of them at car + 0x152C).
struct PartState {
    u32 attached;    // offset 0x0
    s32 hp;          // offset 0x4
    u8 pad8[0x70];   // offset 0x8
    u8 flag78;       // offset 0x78
    u8 pad79[0x3];
    s32 state;       // offset 0x7C
};


// Breaks off every part that is detached (or flagged to break) at its bone box centre.
void fn_8018F9B4(u8* p) {
    PartState* w = (PartState*)(p + 0x152C);
    DamagePart* t = lbl_80332840;
    for (int i = 0; i < 17; i++) {
        if ((w->attached && w->hp > 0 && !w->state) || w->state == 2) {
            s32 bone = *(s32*)((u8*)t + 0x8);
            DVec* lo = &((CarModel*)PTR(p, 0x26C))->boxes[bone].lo;
            DVec* hi = lo + 1;
            DVec c;
            c.x = lo->x + (lo + 1)->x;
            c.y = lo->y + hi->y;
            c.z = lo->z + (lo + 1)->z;
            c.x *= lbl_80561FE4;
            c.y *= lbl_80561FE4;
            c.z *= lbl_80561FE4;
            fn_801BF2A4(p, 1 << bone, &c, lo, lo + 1, *(s32*)((u8*)t + 0x34), (u8*)t + 0x38);
            w->state = 3;
            w->flag78 = 0;
        }
        w++;
        t++;
    }
}

}  // extern "C"

class ModuleTag;
void RegisterModuleTag(ModuleTag* tag);
extern ModuleTag lbl_8055E500;  // this unit's module tag

extern "C" {

extern const f32 lbl_80562058;
extern const f32 lbl_8056205C;
extern const f32 lbl_80562060;
extern const f32 lbl_80562064;

// Unit start-up: register the module tag and fill the damage table's computed entries.
void fn_8018FACC(void) {
    RegisterModuleTag(&lbl_8055E500);
    lbl_80332840[0].axis.x = lbl_80562058;
    lbl_80332840[1].axis.x = lbl_8056205C;
    lbl_80332840[8].axis.y = lbl_80562060;
    lbl_80332840[9].axis.y = lbl_80562064;
}

}  // extern "C"
