// Minimal world (0x802526E0-0x80253258): a tiny rigid-body sandbox where trucks are dropped on a
// ring of radius 325 around the origin and kept circling at a set speed. The slot table mirrors each
// body for the caller. Names are guesses apart from the timeout message.
#include "game/common.h"

struct MWVec2 {
    MWVec2(void) {}
    MWVec2(f32 ax, f32 ay) {
        x = ax;
        y = ay;
    }
    f32 x, y;
};

struct MWVec3 {
    f32 x, y, z;
};

struct MWVec4 {
    MWVec4(void) {}
    MWVec4(f32 ax, f32 ay, f32 az) {
        x = ax;
        y = ay;
        z = az;
    }
    f32 x, y, z, w;
};

struct MWMtx {
    MWVec3 x;    // offset 0x0
    MWVec3 y;    // offset 0xC
    MWVec3 z;    // offset 0x18
    MWVec3 pos;  // offset 0x24
};

// A simulated body (0xB0).
struct MWBody {
    MWVec4 pos;    // offset 0x0
    MWVec4 x;      // offset 0x10
    MWVec4 y;      // offset 0x20
    MWVec4 z;      // offset 0x30
    MWVec4 f40;    // offset 0x40
    MWVec4 vel;    // offset 0x50
    MWVec4 accel;  // offset 0x60
    u8 pad70[0xB0 - 0x70];
};

struct MWWheel {
    MWVec3 pos;  // offset 0x0
    u8 padC[0x10 - 0xC];
};

// Per-body wheel state (0x1F0).
struct MWWheels {
    f32 drop[4];  // offset 0x0
    f32 spin[4];  // offset 0x10
    u8 pad20[0x1F0 - 0x20];
};

struct MWContacts {
    MWWheel w[4];  // offset 0x0
    u8 pad40[0x190 - 0x40];
};

// What the caller sees of each body (0x78).
struct MWSlot {
    MWSlot(void) {}
    MWMtx mtx;     // offset 0x0
    u8 valid;      // offset 0x30
    MWVec3 f34;    // offset 0x34
    f32 h[4];      // offset 0x40
    f32 spin[4];   // offset 0x50
    MWVec3 vel;    // offset 0x60
    MWVec3 avel;   // offset 0x6C
};

struct MWCar {
    u8 pad0[0x12C];
    MWVec3 f12C;  // offset 0x12C
    
};

struct MWSpeed {
    s32 f0;
    f32 speed;  // offset 0x4
};

extern "C" {
double sqrt(double x);
double sin(double x);
double cos(double x);
double fmod(double x, double y);
void* memset(void* p, int c, u32 n);
f32 fn_80129DC4(int seed, f32 lo, f32 hi);  // random float
void fn_801BD790(void);
void fn_801BD808(void);
void fn_801BDA70(MWCar* c);
void fn_801BDA74(int i, const MWCar* c);
void fn_801BE400(int i, MWMtx* m, f32 a);
void fn_801BEDA8(MWBody** bodies, MWWheels** wheels, MWContacts** contacts, void* d, int* n);
void fn_801BEDD4(void);
void fn_801BF1B4(void);
f32 fn_801DCF48(MWVec3* normal, f32 x, f32 z);  // terrain height
void fn_802E30C8(const char* fmt, ...);
f32 PSVECDotProduct(const MWVec4* a, const MWVec4* b);
void PSVECCrossProduct(const MWVec3* a, const MWVec3* b, MWVec3* out);
}

extern f32 lbl_80558DC4[];  // FLT_EPSILON

#define NearZero(x) ((f32)__fabs(x) <= lbl_80558DC4[0])

ModuleTag lbl_8055F2F0;
s32 lbl_8055F2F8;      // body count
MWSpeed lbl_8055F300;  // target speed
u8 lbl_8055F308;
f32 lbl_8055F30C;  // step accumulator
u8 lbl_8055F310;   // timeout reported
MWSlot lbl_80498D00[2];

static inline void Rotate(MWVec2* v, f32 a) {
    f32 c = cos(a);
    f32 s = sin(a);
    f32 x = v->x * c - v->y * s;
    v->y = v->x * s + v->y * c;
    v->x = x;
}

extern "C" {

void fn_802526E0(MWMtx* m) {
    f32 a = fn_80129DC4(0, 0.0f, 6.2831855f);
    MWVec2 d(1.0f, 0.0f);
    Rotate(&d, a);
    m->pos.x = d.x;
    m->pos.y = 0.0f;
    m->pos.z = d.y;
    m->pos.x = m->pos.x * 325.0f;
    m->pos.y = m->pos.y * 325.0f;
    m->pos.z = m->pos.z * 325.0f;
    m->pos.y = 4.0f + fn_801DCF48(0, m->pos.x, m->pos.z);
    m->x.x = d.x;
    m->x.y = 0.0f;
    m->x.z = d.y;
    {
        f32 t = d.x;
        d.x = -d.y;
        d.y = t;
    }
    m->z.x = d.x;
    m->z.y = 0.0f;
    m->z.z = d.y;
    PSVECCrossProduct(&m->z, &m->x, &m->y);
}

void fn_8025280C(MWBody* b, f32 speed) {
    MWVec4 p = b->pos;
    MWVec2 d(p.x, p.z);
    MWVec2 e;
    MWVec2 t;
    f32 len;
    if (d.x * d.x + d.y * d.y < 1.0f) {
        d.x = 1.0f;
        d.y = 0.0f;
    }
    len = sqrt(d.x * d.x + d.y * d.y);
    if (!NearZero(len)) {
        f32 k = 325.0f / len;
        d.x = d.x * k;
        d.y = d.y * k;
    }
    p.x = d.x;
    p.z = d.y;
    b->pos = MWVec4(p.x, p.y, p.z);
    e = d;
    {
        f32 k = 1.0f / (f32)sqrt(e.x * e.x + e.y * e.y);
        e.x = e.x * k;
        e.y = e.y * k;
    }
    t = e;
    {
        f32 tmp = t.y;
        t.y = t.x;
        t.x = -tmp;
    }
    len = sqrt(t.x * t.x + t.y * t.y);
    if (!NearZero(len)) {
        f32 k = speed / len;
        t.x = t.x * k;
        t.y = t.y * k;
    }
    {
        MWVec4 up;
        MWVec4 n;
        MWVec4 a;
        MWVec4 fwd;
        f32 radial = -2.5f * (b->z.x * e.x + b->z.z * e.y);
        f32 h;
        f32 depth;
        b->vel = MWVec4(t.x, b->vel.y, t.y);
        fwd = b->z;
        up = b->x;
        a = b->accel;
        a.y = radial;
        h = fn_801DCF48((MWVec3*)&n, p.x, p.z);
        depth = p.y - h;
        depth = 0.0f > depth ? 0.0f : depth;
        if (depth > 1.5f) {
            f32 u = PSVECDotProduct(&n, &fwd);
            f32 v = -PSVECDotProduct(&n, &up);
            f32 u3 = u * u * u;
            f32 v3 = v * v * v;
            a.x = (a.x + up.x * u3 + fwd.x * v3) * 0.98f;
            a.y = (a.y + up.y * u3 + fwd.y * v3) * 0.98f;
            a.z = (a.z + up.z * u3 + fwd.z * v3) * 0.98f;
        }
        b->accel = MWVec4(a.x, a.y, a.z);
    }
}

void fn_80252B88(int i) {
    MWMtx m;
    fn_802526E0(&m);
    fn_801BE400(i, &m, 30.0f);
}

void fn_80252BC8(void) {
    fn_801BD790();
    lbl_8055F2F8 = 0;
}

void fn_80252BF0(void) {
    int i = lbl_8055F2F8++;
    fn_801BD808();
    {
        MWCar c;
        fn_801BDA70(&c);
        fn_801BDA74(i, &c);
    }
    memset(&lbl_80498D00[i], 0, sizeof(MWSlot));
    fn_80252B88(i);
    lbl_80498D00[i].valid = 1;
}

void fn_80252C78(void) {
    lbl_8055F308 = 1;
    lbl_8055F2F8--;
}

void fn_80252C90(void) {
    MWBody* bodies;
    MWWheels* wheels;
    MWContacts* contacts;
    s32 d;
    int n;
    MWSlot* s;
    int i;
    fn_801BEDA8(&bodies, &wheels, &contacts, &d, &n);
    for (i = 0; i < n; i++) {
        fn_8025280C(&bodies[i], lbl_8055F300.speed);
    }
    fn_801BEDD4();
    s = lbl_80498D00;
    for (i = 0; i < n; i++, s++) {
        MWBody* b = &bodies[i];
        MWWheels* w = &wheels[i];
        MWWheel* c = contacts[i].w;
        int j;
        s->mtx.pos.x = b->pos.x;
        s->mtx.pos.y = b->pos.y;
        s->mtx.pos.z = b->pos.z;
        s->mtx.x.x = b->x.x;
        s->mtx.x.y = b->x.y;
        s->mtx.x.z = b->x.z;
        s->mtx.y.x = b->y.x;
        s->mtx.y.y = b->y.y;
        s->mtx.y.z = b->y.z;
        s->mtx.z.x = b->z.x;
        s->mtx.z.y = b->z.y;
        s->mtx.z.z = b->z.z;
        s->vel.x = b->vel.x;
        s->vel.y = b->vel.y;
        s->vel.z = b->vel.z;
        s->avel.x = b->accel.x;
        s->avel.y = b->accel.y;
        s->avel.z = b->accel.z;
        for (j = 0; j < 4; j++) {
            s->spin[j] = w->spin[j];
        }
        for (j = 0; j < 4; j++) {
            s->spin[j] = fmod(6.2831855f + s->spin[j], 6.283185307179586);
            s->h[j] = c[j].pos.y + w->drop[j];
        }
        for (j = 0; j < 4; j++) {
            w->spin[j] = s->spin[j];
        }
        s->valid = 1;
    }
    fn_801BF1B4();
}

void fn_80252F50(f32 dt) {
    int n = 0;
    lbl_8055F30C = lbl_8055F30C + 1.5f * dt;
    while (lbl_8055F30C > 0.0f) {
        fn_80252C90();
        lbl_8055F30C = lbl_8055F30C - 0.0192f;
        if (++n > 10) {
            if (!lbl_8055F310) {
                lbl_8055F310 = 1;
                fn_802E30C8("%s", "minworld: loop timeout!");
            }
            lbl_8055F30C = 0.0f;
            break;
        }
    }
}

void fn_80253004(int i, MWSlot* out) { *out = lbl_80498D00[i]; }

void fn_80253104(int i, const MWCar* car) {
    lbl_80498D00[i].f34 = car->f12C;
    fn_801BDA74(i, car);
    if (!lbl_80498D00[i].valid) {
        fn_80252B88(i);
    } else {
        MWMtx m = lbl_80498D00[i].mtx;
        m.pos.y = 3.0f + fn_801DCF48(0, m.pos.x, m.pos.z);
        fn_801BE400(i, &m, 30.0f);
    }
}
}
