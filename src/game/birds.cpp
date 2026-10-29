// Birds (0x802A81F0-0x802A8FEC): flocks from "birds.val". Each flock has a leader boid that wanders around a
// ground point and up to 32 birds that follow it; a boid turns toward its target at a turn rate scaled by its
// distance band and speeds up the farther it is. Drawn with up to four bird models. Names are guesses apart
// from the strings.
#include "game/common.h"

extern "C" {
void* memset(void* dst, int c, size_t n);
void* memcpy(void* dst, const void* src, size_t n);
int strcasecmp(const char* a, const char* b);
double sqrt(double x);
double cos(double x);
double sin(double x);
f32 PSVECDotProduct(const void* a, const void* b);
void PSVECNormalize(const void* in, void* out);
void PSVECCrossProduct(const void* a, const void* b, void* out);
ValueTable* fn_80131810(const char* name);
void fn_80131818(ValueTable* t);
int fn_801345D8(char* dst, int n, const char* fmt, ...);
void fn_801E9DC4(s16* h);
int fn_801E9E30(s16* h, void* pos);
f32 fn_80287BC8(f32 lo, f32 hi);
void* fn_802C6FC0(const char* name, int a, int b);  // load a model
void fn_802C6CF8(void* model);
void fn_802C744C(void* model, const void* frame);
void fn_802E30C8(const char* fmt, ...);
}

void* operator new(size_t size, const char* name);
void operator delete(void* p);

extern f32 lbl_8055E3C8;    // time
extern f32 lbl_80558DC4[];  // FLT_EPSILON

struct BVec {
    BVec(void) {}
    BVec(f32 a, f32 b, f32 c) {
        x = a;
        y = b;
        z = c;
    }
    f32 x;
    f32 y;
    f32 z;
};

struct BoidDesc {
    f32 radius;    // offset 0x0
    f32 turn;      // offset 0x4
    f32 speedMin;  // offset 0x8
    f32 speedMax;  // offset 0xC
    BVec pos;      // offset 0x10
};

struct Boid {
    BVec pos;      // offset 0x0
    BVec vel;      // offset 0xC
    f32 radius;    // offset 0x18
    f32 turn;      // offset 0x1C
    f32 speedMin;  // offset 0x20
    f32 speedMax;  // offset 0x24
};

struct BirdFrame {
    BVec side;  // offset 0x0
    BVec up;    // offset 0xC
    BVec fwd;   // offset 0x18
    BVec pos;   // offset 0x24
};

struct ModelVec {
    ~ModelVec(void) {
        if (this) {
            data = 0;
            n = 0;
            cap = 0;
        }
    }
    void** data;  // offset 0x0
    s32 n;        // offset 0x4
    s32 cap;      // offset 0x8
};

struct BoidVec {
    ~BoidVec(void) {
        if (this) {
            data = 0;
            n = 0;
            cap = 0;
        }
    }
    Boid* data;  // offset 0x0
    s32 n;       // offset 0x4
    s32 cap;     // offset 0x8
};

struct Flock {
    ~Flock(void);
    Boid lead;           // offset 0x0
    ModelVec models;     // offset 0x28
    void* modelBuf[4];   // offset 0x34
    f32 flockRad;        // offset 0x44
    f32 birdRad;         // offset 0x48
    f32 flockSpeed;      // offset 0x4C
    f32 birdSpeedMin;    // offset 0x50
    f32 birdSpeedMax;    // offset 0x54
    f32 flockTurn;       // offset 0x58
    f32 birdTurn;        // offset 0x5C
    BVec pt;             // offset 0x60
    BoidVec birds;       // offset 0x6C
    Boid birdBuf[32];    // offset 0x78
};

struct FlockList {
    FlockList(void) {
        data = buf;
        cap = 10;
        n = 0;
    }
    ~FlockList(void) {
        if (this) {
            data = 0;
            n = 0;
            cap = 0;
        }
    }
    Flock** data;   // offset 0x0
    s32 n;          // offset 0x4
    s32 cap;        // offset 0x8
    Flock* buf[10]; // offset 0xC
};

ModuleTag lbl_8055F8C8;
FlockList lbl_804BD6C4;
UnkStaticFFFF lbl_8055F8D0;
f32 lbl_8055F8D4;  // frame time
f32 lbl_8055F8D8;  // last time

static inline void InitBoid(Boid* b, const BoidDesc* d) {
    memcpy(&b->radius, d, 0x10);
    b->pos.x = d->pos.x;
    b->pos.y = d->pos.y;
    b->pos.z = d->pos.z;
    f32 z = fn_80287BC8(-10.0f, 10.0f);
    b->vel.x = fn_80287BC8(-10.0f, 10.0f);
    b->vel.y = 0.0f;
    b->vel.z = z;
    if ((f32)__fabs(PSVECDotProduct(&b->vel, &b->vel)) <= lbl_80558DC4[0]) {
        b->vel.x = 1.0f;
    }
    PSVECNormalize(&b->vel, &b->vel);
    f32 s = b->speedMin;
    b->vel.x *= s;
    b->vel.y *= s;
    b->vel.z *= s;
}

extern "C" {

void fn_802A8A4C(Boid* b, const BVec* target);
s32 fn_802A8A34(BoidVec* v);
Boid* fn_802A8A3C(BoidVec* v, int i);

void fn_802A81F0(void) {
    int i;
    lbl_804BD6C4.n = 0;
    ValueTable* t = fn_80131810("birds.val");
    if (t) {
        int next = 0;
        ValueNode* n = t->GetChild(next++);
        int count = 0;
        while (n) {
            if (!strcasecmp(n->GetName(), "flock")) {
                Flock** slot = &lbl_804BD6C4.data[lbl_804BD6C4.n++];
                Flock* f = (Flock*)operator new(sizeof(Flock), "flock");
                if (f) {
                    f->models.data = f->modelBuf;
                    f->models.cap = 4;
                    f->birds.data = f->birdBuf;
                    f->birds.cap = 32;
                    f->birds.n = 0;
                    f->models.n = 0;
                    for (i = 0; i < f->models.cap; i++) {
                        char key[64];
                        fn_801345D8(key, 64, "model%d", i);
                        if (n->Has(key)) {
                            f->models.data[f->models.n++] = fn_802C6FC0(n->GetValue(key), 0, 0);
                        }
                    }
                    f->birds.n = 0;
                    int nb = n->GetIntValue("birds");
                    for (int j = 0; j < nb; j++) {
                        f->birds.n++;
                    }
                    ValPoint p = n->GetPoint("pt");
                    f->pt.x = p.x;
                    f->pt.y = 0.0f;
                    f->pt.z = p.y;
                    f32 hit = n->GetFloat("hit");
                    s16 h[2];
                    fn_801E9DC4(h);
                    fn_801E9E30(h, &f->pt);
                    f->pt.y = f->pt.y + hit;
                    f->flockRad = n->GetFloat("flock_rad", 100.0f);
                    f->birdRad = n->GetFloat("bird_rad", 30.0f);
                    f->flockSpeed = n->GetFloat("flock_speed", 20.0f);
                    f->birdSpeedMin = n->GetFloat("bird_speed_min", 30.0f);
                    f->birdSpeedMax = n->GetFloat("bird_speed_max", 70.0f);
                    f->flockTurn = n->GetFloat("flock_turn", 45.0f);
                    f->birdTurn = n->GetFloat("bird_turn", 180.0f);
                }
                *slot = f;
                count++;
            }
            n = t->GetChild(next++);
        }
        fn_802E30C8("Birds: loaded %d flocks", count);
        fn_80131818(t);
    }
}

inline Flock::~Flock(void) {
    for (int j = 0; j < models.n; j++) {
        fn_802C6CF8(models.data[j]);
    }
    models.n = 0;
    birds.n = 0;
}

void fn_802A85C8(void) {
    for (int i = 0; i < lbl_804BD6C4.n; i++) {
        delete lbl_804BD6C4.data[i];
    }
    lbl_804BD6C4.n = 0;
}

void fn_802A86A4(void) {
    for (int i = 0; i < lbl_804BD6C4.n; i++) {
        Flock* f = lbl_804BD6C4.data[i];
        BoidDesc d;
        memset(&d, 0, 0);
        d.pos.x = f->pt.x;
        d.pos.y = f->pt.y;
        d.pos.z = f->pt.z;
        d.turn = f->flockTurn;
        d.radius = f->flockRad;
        d.speedMax = f->flockSpeed;
        d.speedMin = f->flockSpeed;
        InitBoid(&f->lead, &d);
        d.turn = f->birdTurn;
        d.radius = f->birdRad;
        d.speedMax = f->birdSpeedMax;
        d.speedMin = f->birdSpeedMin;
        f32 r = f->birdRad * 0.25f;
        for (int j = 0; j < f->birds.n; j++) {
            Boid* b = &f->birds.data[j];
            BVec off(fn_80287BC8(-r, r), fn_80287BC8(-r, r), fn_80287BC8(-r, r));
            d.pos.x = f->lead.pos.x + off.x;
            d.pos.y = f->lead.pos.y + off.y;
            d.pos.z = f->lead.pos.z + off.z;
            InitBoid(b, &d);
        }
    }
}

void fn_802A8978(void) {
    int j;
    f32 now = lbl_8055E3C8;
    lbl_8055F8D4 = now - lbl_8055F8D8;
    if (now < lbl_8055F8D8) {
        lbl_8055F8D4 = 1.0f / 59.94f;
    }
    lbl_8055F8D8 = now;
    for (int i = 0; i < lbl_804BD6C4.n; i++) {
        Flock* f = lbl_804BD6C4.data[i];
        fn_802A8A4C(&f->lead, &f->pt);
        for (j = 0; j < fn_802A8A34(&f->birds); j++) {
            fn_802A8A4C(fn_802A8A3C(&f->birds, j), &f->lead.pos);
        }
    }
}

s32 fn_802A8A34(BoidVec* v) { return v->n; }

Boid* fn_802A8A3C(BoidVec* v, int i) { return &v->data[i]; }

static inline f32 Clamp01(f32 x) {
    f32 r = 1.0f;
    if (r >= x) {
        r = x;
    }
    if (r <= 0.0f) {
        r = 0.0f;
    }
    return r;
}

void fn_802A8A4C(Boid* b, const BVec* target) {
    f32 dir[2];
    dir[0] = b->vel.x;
    dir[1] = b->vel.z;
    f32 inv = 1.0f / (f32)sqrt(dir[0] * dir[0] + dir[1] * dir[1]);
    dir[0] *= inv;
    dir[1] *= inv;
    f32 to[2];
    to[0] = target->x - b->pos.x;
    to[1] = target->z - b->pos.z;
    f32 d2 = to[0] * to[0] + to[1] * to[1];
    if (!((f32)__fabs(d2) <= lbl_80558DC4[0])) {
        f32 k = 1.0f / (f32)sqrt(d2);
        to[0] *= k;
        to[1] *= k;
    }
    f32 dot = to[0] * dir[0] + to[1] * dir[1];
    f32 steer = fn_80287BC8(0.0f, 1.0f);
    f32 half = b->radius / 2.0f;
    if (d2 > half * half && dot > (f32)cos(0.05235988f)) {
        steer = 0.0f;
    }
    f32 r = b->radius;
    f32 third = r / 3.0f;
    third *= third;
    f32 t = Clamp01((d2 - third) / (r * r - third));
    f32 ang = lbl_8055F8D4 * ((0.0f + t * (steer - 0.0f)) * (0.017453292f * b->turn));
    f32 c = cos(ang);
    f32 s = sin(ang);
    f32 m[9];
    m[0] = c;
    m[1] = 0.0f;
    m[2] = -s;
    m[3] = 0.0f;
    m[4] = 1.0f;
    m[5] = 0.0f;
    m[6] = s;
    m[7] = 0.0f;
    m[8] = c;
    BVec* v = &b->vel;
    f32 x = v->x * m[0] + v->y * m[3] + v->z * m[6];
    f32 y = v->x * m[1] + v->y * m[4] + v->z * m[7];
    f32 z = v->x * m[2] + v->y * m[5] + v->z * m[8];
    v->x = x;
    v->y = y;
    v->z = z;
    PSVECNormalize(&b->vel, &b->vel);
    f32 u = Clamp01((d2 - 0.0f) / (r * r - 0.0f));
    f32 spd = b->speedMin + u * (b->speedMax - b->speedMin);
    b->vel.x *= spd;
    b->vel.y *= spd;
    b->vel.z *= spd;
    f32 dt = lbl_8055F8D4;
    b->pos.x += b->vel.x * dt;
    b->pos.y += b->vel.y * dt;
    b->pos.z += b->vel.z * dt;
}

void fn_802A8DE8(void) {
    for (int i = 0; i < lbl_804BD6C4.n; i++) {
        Flock* f = lbl_804BD6C4.data[i];
        for (int j = 0; j < f->birds.n; j++) {
            Boid* b = &f->birds.data[j];
            BirdFrame m;
            m.pos.x = b->pos.x;
            m.pos.y = b->pos.y;
            m.pos.z = b->pos.z;
            m.fwd.x = b->vel.x;
            m.fwd.y = b->vel.y;
            m.fwd.z = b->vel.z;
            if ((f32)__fabs(PSVECDotProduct(&m.fwd, &m.fwd)) <= lbl_80558DC4[0]) {
                m.fwd.x = 0.0f;
                m.fwd.y = 0.0f;
                m.fwd.z = 1.0f;
            }
            PSVECNormalize(&m.fwd, &m.fwd);
            m.up.x = 0.0f;
            m.up.y = 1.0f;
            m.up.z = 0.0f;
            PSVECCrossProduct(&m.up, &m.fwd, &m.side);
            fn_802C744C(f->models.data[j % f->models.n], &m);
        }
    }
}

}  // extern "C"
