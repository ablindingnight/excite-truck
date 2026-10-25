// Ball (0x8017D378-0x8017EB80): the "Ball" physics object, a rigid sphere ("ball-volume") parked under the
// track per car. A player drops or throws it ("Arming" -> "Press" status), and it explodes on a hard enough
// hit, pushing every body within range away ("explode.sfx"). Also registered as a status/input listener for
// its car. Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
double sqrt(double x);
f32 PSVECDotProduct(const void* a, const void* b);
void PSVECNormalize(const void* src, void* dst);
f32 fn_80129DC4(int seed, f32 lo, f32 hi);  // random float
void fn_801796EC(int type, const void* data, int size);  // post event
void fn_801E9DC4(s16* h);
int fn_801E9E30(s16* h, const f32* pos);
void fn_801D0C58(void* l, int car, int a);  // register listener
void fn_801D0C84(void* l);                  // unregister listener
void fn_801602B0(void* v);
void fn_80160230(void* dst, const void* xz);
void fn_80144DC8(void* m);
void fn_80144B44(void* v, f32 x, f32 y, f32 z);
}

struct BMtx;
extern "C" void fn_8017E388(BMtx* m);

void* operator new(size_t size, const char* tag);

extern u32 lbl_8055D430;
extern f32 lbl_8055E3C8;    // time
extern f32 lbl_8055E460;    // tick alpha
extern f32 lbl_80558DC4[];  // FLT_EPSILON

inline bool DbgFlag(u32 m) {
    int r = 0;
    if ((lbl_8055D430 & 1) && (lbl_8055D430 & m)) {
        r = 1;
    }
    return r;
}

struct BVec3 {
    BVec3(void) {}
    BVec3(f32 a, f32 b, f32 c) {
        x = a;
        y = b;
        z = c;
    }
    BVec3& operator+=(const BVec3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    f32 x;
    f32 y;
    f32 z;
};

struct BMtx {
    BVec3 rot[3];  // offset 0x0
    BVec3 pos;     // offset 0x24
};

struct BTrackStart {
    u8 pad0[0x1C];
    f32* xz;  // offset 0x1C
};

struct BTrack {
    u8 pad0[0x2C];
    BTrackStart* start;  // offset 0x2C
};

struct BCarRec {
    u8 pad0[0x10];
    u32 model;  // offset 0x10
};

struct BGround {
    u8 pad0[0x28];
    f32 y;  // offset 0x28
};

struct BRacerInfo {
    u8 pad0[0x24];
    s32 f24;  // offset 0x24
};

class BRacer {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual BRacerInfo* vf10(void);
    virtual void vf14(void);
    virtual struct BPath* vf18(void);
};

struct BPathPt {
    f32 x;   // offset 0x0
    f32 z;   // offset 0x4
    f32 dx;  // offset 0x8
    f32 dz;  // offset 0xC
    u8 pad10[0x4];
    BPathPt* next;  // offset 0x14
    f32 len;        // offset 0x18
    u8 pad1C[0xC];
    f32 width;  // offset 0x28
};

struct BVec2 {
    BVec2(void) {}
    BVec2(f32 a, f32 b) {
        x = a;
        z = b;
    }
    f32 x;
    f32 z;
};

struct BPathPos {
    BPathPos(const BPathPos& o) {
        pt = o.pt;
        t = o.t;
        f8 = o.f8;
    }
    BPathPt* pt;  // offset 0x0
    f32 t;        // offset 0x4
    s32 f8;       // offset 0x8
    void Advance(f32 d) {
        while (d > pt->len) {
            d -= pt->len;
            pt = pt->next;
        }
    }
    f32 Width(void) const { return pt->width + t * (pt->next->width - pt->width); }
    void Place(f32* xz, f32 off) const {
        BVec2 dir;
        f32 inv;
        xz[0] = pt->x + t * (pt->next->x - pt->x);
        xz[1] = pt->z + t * (pt->next->z - pt->z);
        dir.x = pt->dx + t * (pt->next->dx - pt->dx);
        dir.z = pt->dz + t * (pt->next->dz - pt->dz);
        inv = 1.0f / (f32)sqrt(dir.x * dir.x + dir.z * dir.z);
        dir.x *= inv;
        dir.z *= inv;
        {
            f32 t = dir.x;
            dir.x = dir.z;
            dir.z = -t;
        }
        xz[0] += dir.x * off;
        xz[1] += dir.z * off;
    }
};

struct BPath {
    u8 pad0[0x4];
    BPathPos pos;  // offset 0x4
};

extern "C" {
BTrack* fn_80178C9C(void);
BCarRec* fn_8017960C(int id);
BRacer* fn_801B2EC0(int id);
BGround* fn_801602B4(BCarRec* c);
void* fn_801769CC(void);
}

class BSound {
public:
    virtual void vf08(void);
    u8 pad4[0x4];
    f32 vol;  // offset 0x8
    u8 padC[0x49 - 0xC];
    u8 playing;  // offset 0x49
    u8 stop;     // offset 0x4A
    u8 loop;     // offset 0x4B
    u8 f4C;      // offset 0x4C
    u8 dirty;    // offset 0x4D
    void SetVol(f32 v) {
        if (v != vol) {
            dirty = 1;
            vol = v;
        }
    }
    void Play(void) {
        playing = 1;
        loop = 0;
        stop = 0;
    }
    void Loop(void) {
        stop = 0;
        loop = 1;
        playing = 1;
    }
    void Stop(void) {
        stop = 1;
        f4C = 0;
    }
};

class BSoundBase {
public:
    virtual ~BSoundBase(void);
    static void operator delete(void* p);
};

class BSfx : public BSoundBase {
public:
    virtual ~BSfx(void) {}
};

extern "C" BSound* fn_802077A4(const char* name, int prio, const BVec3* vel, const BVec3* pos);

class Phob;
extern "C" void fn_8019EECC(Phob* p, BVec3* v);

class BShape {
public:
    BShape(Phob* o, const BMtx* m, u32 t, f32 k, f32 s) {
        mtx = m;
        owner = o;
        a8 = 0;
        type = t;
        stiff = k;
        skin = s;
        a20 = 100;
        a1C = 0;
    }
    virtual ~BShape(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void vf18(void);
    virtual void vf1C(void);
    virtual void vf20(void);
    virtual void vf24(void);
    virtual void vf28(void);
    virtual void vf2C(void);
    virtual void vf30(void);
    virtual void vf34(void);
    virtual void vf38(void) = 0;
    virtual f32 Radius(void);    // 0x3C
    virtual void SetId(int id);  // 0x40
    Phob* owner;      // offset 0x4
    s32 a8;           // offset 0x8
    const BMtx* mtx;  // offset 0xC
    f32 stiff;        // offset 0x10
    f32 skin;         // offset 0x14
    u32 type;         // offset 0x18
    s32 a1C;          // offset 0x1C
    s32 a20;          // offset 0x20
};

class BSphere : public BShape {
public:
    BSphere(Phob* o, const BMtx* m, f32 k, f32 s, f32 r, const BVec3& c) : BShape(o, m, 0x53504852, k, s) {
        radius = r;
        f34 = 0.0f;
        f38 = 0.0f;
        f3C = 0.0f;
        f40 = 0.0f;
        f44 = 0.0f;
        f48 = 0.0f;
        b4C = 0;
        f50 = 0.0f;
        center.x = c.x;
        center.y = c.y;
        center.z = c.z;
        if (o) {
            fn_8019EECC(o, &center);
        }
    }
    virtual ~BSphere(void);
    virtual void vf38(void);
    f32 radius;    // offset 0x24
    BVec3 center;  // offset 0x28
    f32 f34;       // offset 0x34
    f32 f38;       // offset 0x38
    f32 f3C;       // offset 0x3C
    f32 f40;       // offset 0x40
    f32 f44;       // offset 0x44
    f32 f48;       // offset 0x48
    u8 b4C;        // offset 0x4C
    f32 f50;       // offset 0x50
};

struct BallDesc {
    u8 pad0[0x18];
    s32 car;        // offset 0x18
    u8 pad1C[0x4];
    f32 radius;     // offset 0x20
    f32 stiff;      // offset 0x24
    f32 stiffK;     // offset 0x28
    f32 skin;       // offset 0x2C
    u8 explodable;  // offset 0x30
    u8 pad31[3];
    BVec3 launch;   // offset 0x34
};

class Phob {
public:
    virtual void Init(BallDesc* d);   // 0x8
    virtual ~Phob(void);              // 0xC
    virtual void Reset(void);         // 0x10
    virtual void vf14(void);          // 0x14
    virtual void vf18(void);          // 0x18
    virtual void Update(void);        // 0x1C
    virtual void vf20(void);          // 0x20
    virtual void vf24(void);          // 0x24
    virtual void vf28(u8* buf);       // 0x28
    virtual void vf2C(void);          // 0x2C
    virtual void vf30(void);          // 0x30
    virtual void vf34(void);          // 0x34
    virtual void vf38(void);          // 0x38
    virtual void vf3C(void);          // 0x3C
    virtual int vf40(void);           // 0x40
    virtual void vf44(void);          // 0x44
    void Xform(void);
    void AddShape(BShape* s);
    const s16* Hint(void);
    const BVec3* Vel(void);
    const BMtx* World(void);
    s32 a4;       // offset 0x4
    s32 a8;       // offset 0x8
    u8 padC[0x3C - 0xC];
    BMtx mtx;     // offset 0x3C
    s16 hint[2];  // offset 0x6C
    BMtx world;   // offset 0x70
    BMtx prev;    // offset 0xA0
    u8 padD0[0x118 - 0xD0];
    BVec3 vel;    // offset 0x118
    BVec3 avel;   // offset 0x124
    u8 pad130[0x1E8 - 0x130];
};

class RigidBody : public Phob {
public:
    RigidBody(void);
    virtual void Init(BallDesc* d);
    virtual void Reset(void);
    virtual void Update(void);
    virtual void vf48(int a, int b, int kind, RigidBody* other);  // 0x48
    virtual void vf4C(void);
    virtual void vf50(void);
    void Push(int a, int b, const BVec3* imp, const BVec3* at, int kind, int c, BCarRec* by, f32 k);
    u8 b1E8;  // offset 0x1E8
    u8 b1E9;  // offset 0x1E9
};

class IBallListener {
public:
    virtual void Status(const char** text, u8* flash) = 0;
    virtual void Input(Phob* src, u8* pressed) = 0;
    virtual bool GetPos(f32* xz, f32* vxz) = 0;
};

class Ball : public RigidBody, public IBallListener {
public:
    Ball(void) {}
    virtual void Init(BallDesc* d);
    virtual ~Ball(void);
    virtual void Reset(void);
    virtual void Update(void);
    virtual void vf28(u8* buf);
    virtual void vf48(int a, int b, int kind, RigidBody* other);
    virtual void Status(const char** text, u8* flash) {
        switch (state) {
        case 0:
            *text = "Arming";
            *flash = 0;
            return;
        case 3:
            *text = "Exploded";
            *flash = 0;
            return;
        case 1:
            *text = 0;
            *flash = 0;
            return;
        case 2:
            *text = "Press  ";
            *flash = 1;
            return;
        default:
            *text = "UNK";
            *flash = 0;
            return;
        }
    }
    virtual void Input(Phob* src, u8* pressed);
    virtual bool GetPos(f32* xz, f32* vxz);
    void Launch(const BMtx* m, const BVec3* v, s16 h, f32 k);
    f32 DropHeight(void);
    void PickDrop(f32* xz, s16* h);
    s32 state;          // offset 0x1F0
    f32 timer;          // offset 0x1F4
    s32 car;            // offset 0x1F8
    u8 resetPending;    // offset 0x1FC
    u8 explodable;      // offset 0x1FD
    u8 active;          // offset 0x1FE
    BVec3 startPos;     // offset 0x200
    BVec3 explPos;      // offset 0x20C
    BSound* sndStart;   // offset 0x218
    BSound* sndRoll;    // offset 0x21C
    BSound* sndExpl;    // offset 0x220
    BVec3 launch;       // offset 0x224
};

ModuleTag lbl_8055E498;
UnkStaticFFFF lbl_8055E4A0;
static UnkStatic123 lbl_804348C0;
UnkStaticFFFF lbl_8055E4A4;
UnkStatic7F lbl_8055E4A8;

extern "C" f32 fn_8017D378(void) { return lbl_8055E3C8; }

BVec3 Home(int car) {
    BTrack* t = fn_80178C9C();
    f32* p = t->start->xz;
    BVec3 v;
    v.x = p[0];
    v.y = 0.0f;
    v.z = p[1];
    v.y = -1000.0f + (f32)(car * 2);
    return v;
}

extern "C" void fn_8017D404(f32 r, f32 force, const BVec3* c, RigidBody* self, int car) {
    BCarRec* by = fn_8017960C(car);
    struct Node {
        Phob* obj;
        Node* next;
    };
    struct List {
        Node* head;
    };
    List* lists[5];
    int n = 5;
    int i;
    void fn_801B619C(List** out, int* n, const BVec3* c);
    fn_801B619C(lists, &n, c);
    for (i = 0; i < n; i++) {
        Node* nd;
        for (nd = lists[i]->head; nd; nd = nd->next) {
            RigidBody* b = nd->obj->vf40() ? (RigidBody*)nd->obj : 0;
            if (b) {
                BVec3 d;
                f32 d2;
                d.x = c->x - b->mtx.pos.x;
                d.y = c->y - b->mtx.pos.y;
                d.z = c->z - b->mtx.pos.z;
                d2 = PSVECDotProduct(&d, &d);
                if (d2 < r * r && d2 > 0.0001f) {
                    f32 fall;
                    BVec3 at;
                    BVec3 imp;
                    f32 k;
                    f32 rnd;
                    if (b == self) {
                        fall = 1.0f;
                    } else {
                        fall = 1.0f - (f32)sqrt(d2) / r;
                    }
                    PSVECNormalize(&d, &d);
                    at.x = b->mtx.pos.x + d.x;
                    at.y = b->mtx.pos.y + d.y;
                    at.z = b->mtx.pos.z + d.z;
                    rnd = fn_80129DC4(3, -0.2f, 0.2f);
                    at.x += b->mtx.rot[0].x * rnd;
                    at.y += b->mtx.rot[0].y * rnd;
                    at.z += b->mtx.rot[0].z * rnd;
                    rnd = fn_80129DC4(3, -0.2f, 0.2f);
                    at.x += b->mtx.rot[2].x * rnd;
                    at.y += b->mtx.rot[2].y * rnd;
                    at.z += b->mtx.rot[2].z * rnd;
                    k = -fall * force;
                    d.y -= 0.15f;
                    imp.x = d.x * k;
                    imp.y = d.y * k;
                    imp.z = d.z * k;
                    b->Push(0, 0, &imp, &at, 1001, 0, by, 1.0f);
                }
            }
        }
    }
}

extern "C" Ball* fn_8017D688(void) { return new ("Ball") Ball; }

inline void Ball::Init(BallDesc* d) {
    BSphere* s;
    RigidBody::Init(d);
    f32 kk = d->stiffK;
    f32 k = 4.45f * d->stiff / 0.0254f;
    f32 skin = d->skin;
    f32 radius = 0.0254f * d->radius;
    BVec3 c(0.0f, 0.0f, 0.0f);
    car = d->car;
    f32 stiff = k * (kk * kk);
    mtx.pos = Home(car);
    Xform();
    explodable = d->explodable;
    launch = d->launch;
    s = new ("ball-volume") BSphere(this, &mtx, stiff, skin, radius, c);
    s->SetId(1001);
    AddShape(s);
    b1E8 = 1;
    b1E9 = 0;
    resetPending = 1;
    active = 0;
    fn_801D0C58((IBallListener*)this, car, 0);
    startPos = mtx.pos;
    explPos = mtx.pos;
    sndStart = fn_802077A4("chkstart.sfx", 5, &vel, &startPos);
    sndRoll = fn_802077A4("chkroll.sfx", 8, &vel, &mtx.pos);
    sndExpl = fn_802077A4("explode.sfx", 5, &vel, &explPos);
}

inline Ball::~Ball(void) {
    fn_801D0C84((IBallListener*)this);
    if (sndExpl) {
        delete (BSfx*)sndExpl;
    }
    if (sndRoll) {
        delete (BSfx*)sndRoll;
    }
    if (sndStart) {
        delete (BSfx*)sndStart;
    }
}

inline void Ball::Reset(void) {
    RigidBody::Reset();
    mtx.pos = Home(car);
    Xform();
    b1E9 = 0;
    if (DbgFlag(8)) {
        state = 0;
        timer = fn_8017D378() + 1.5f;
    }
    resetPending = 0;
    active = 0;
}

inline void Ball::vf48(int a, int b, int kind, RigidBody* other) {
    if (active && explodable) {
        if ((kind >= 100 || DbgFlag(0x90)) && kind < 1000) {
            struct {
                BVec3 pos;
                BVec3 vel;
            } ev;
            ev.pos = mtx.pos;
            ev.vel = vel;
            fn_801796EC(8, &ev, sizeof(ev));
            fn_8017D404(5.0f, 17500.0f, &mtx.pos, other, car);
            if (sndRoll) {
                sndRoll->Stop();
            }
            if (sndExpl) {
                explPos = mtx.pos;
                sndExpl->SetVol(1.0f);
                sndExpl->Play();
            }
            active = 0;
            resetPending = 1;
        }
    }
    RigidBody::vf48(a, b, kind, other);
}

inline void Ball::Update(void) {
    if (resetPending) {
        RigidBody::Reset();
        mtx.pos = Home(car);
        Xform();
        b1E9 = 0;
        resetPending = 0;
    }
    if (state == 0 && timer < fn_8017D378()) {
        u32 model = fn_8017960C(car)->model;
        int st = 1;
        if (model == 0x41434152) {
            st = 2;
        }
        state = st;
    }
    if (!DbgFlag(0x88)) {
        mtx.pos = Home(car);
        Xform();
        vel.x = 0.0f;
        vel.y = 0.0f;
        vel.z = 0.0f;
    }
    if (!active && DbgFlag(0x80)) {
        if (fn_801B2EC0(car)->vf10()->f24 == 0) {
            s16 h;
            f32 xz[2];
            BVec3 v;
            BMtx m;
            f32 up;
            fn_801602B0(xz);
            PickDrop(xz, &h);
            fn_80144DC8(&m);
            fn_8017E388(&m);
            fn_80160230(&m.pos, xz);
            up = DropHeight();
            m.pos.y = fn_801602B4(fn_8017960C(car))->y + up;
            fn_80144B44(&v, 0.0f, 0.0f, 0.0f);
            Launch(&m, &v, h, 0.0f);
        }
    }
    {
        struct P {
            f32 v[3];
        } p = *(P*)&mtx.pos;
        if (!fn_801E9E30(hint, p.v)) {
            resetPending = 1;
        }
    }
    RigidBody::Update();
}

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

inline void Rotate(BVec3* out, const BMtx* m, const BVec3* v) {
    if (out == v) {
        f32 x = Dot3(v->x, m->rot[0].x, v->y, m->rot[1].x, v->z, m->rot[2].x);
        f32 y = Dot3(v->x, m->rot[0].y, v->y, m->rot[1].y, v->z, m->rot[2].y);
        f32 z = Dot3(v->x, m->rot[0].z, v->y, m->rot[1].z, v->z, m->rot[2].z);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(v->x, m->rot[0].x, v->y, m->rot[1].x, v->z, m->rot[2].x);
        out->y = Dot3(v->x, m->rot[0].y, v->y, m->rot[1].y, v->z, m->rot[2].y);
        out->z = Dot3(v->x, m->rot[0].z, v->y, m->rot[1].z, v->z, m->rot[2].z);
    }
}

void Ball::Launch(const BMtx* in, const BVec3* v, s16 h, f32 k) {
    BMtx m;
    BVec3 p;
    fn_8017D378();
    m = *in;
    avel.x = 0.0f;
    avel.y = 0.0f;
    avel.z = 0.0f;
    hint[0] = h;
    b1E9 = 1;
    Rotate(&p, &m, &launch);
    p += m.pos;
    m.pos = p;
    mtx = m;
    Xform();
    if (DbgFlag(0x80)) {
        vel.x = 0.0f;
        vel.y = -19.62f;
        vel.z = 0.0f;
    } else if (DbgFlag(0x10)) {
        BVec3 d;
        f32 s;
        d.x = m.rot[2].x * 0.707f + m.rot[1].x * 0.4949f;
        d.y = m.rot[2].y * 0.707f + m.rot[1].y * 0.4949f;
        s = 35.5556f * k;
        d.z = m.rot[2].z * 0.707f + m.rot[1].z * 0.4949f;
        vel.x = d.x * s;
        vel.y = d.y * s;
        vel.z = d.z * s;
        vel.x += v->x;
        vel.y += v->y;
        vel.z += v->z;
    } else {
        f32 s = 40.0f * k;
        vel.x = v->x + mtx.rot[2].x * s;
        vel.y = v->y + mtx.rot[2].y * s;
        vel.z = v->z + mtx.rot[2].z * s;
        if (DbgFlag(8)) {
            state = 0;
            timer = fn_8017D378() + 1.5f;
        }
    }
    active = 1;
    resetPending = 0;
    if (sndStart) {
        startPos = p;
        sndStart->SetVol(1.0f);
        sndStart->Play();
    }
    if (sndRoll) {
        sndRoll->SetVol(1.0f);
        sndRoll->Loop();
    }
}

f32 Ball::DropHeight(void) { return 60.0f; }

extern "C" void fn_8017E388(BMtx* m) {
    m->rot[0].x = 1.0f;
    m->rot[0].y = 0.0f;
    m->rot[0].z = 0.0f;
    m->rot[1].x = 0.0f;
    m->rot[1].y = 1.0f;
    m->rot[1].z = 0.0f;
    m->rot[2].x = 0.0f;
    m->rot[2].y = 0.0f;
    m->rot[2].z = 1.0f;
    m->pos.x = 0.0f;
    m->pos.y = 0.0f;
    m->pos.z = 0.0f;
}

void Ball::PickDrop(f32* xz, s16* h) {
    if (*(u32*)((u8*)fn_801769CC() + 4) & 0x10000) {
        RigidBody* b = (RigidBody*)fn_8017960C(car);
        BVec3 p;
        f32 c;
        f32 k = 2.0f;
        p.x = b->mtx.pos.x + b->vel.x * k;
        p.y = b->mtx.pos.y + b->vel.y * k;
        p.z = b->mtx.pos.z + b->vel.z * k;
        c = 10.0f;
        p += BVec3(fn_80129DC4(4, -c, c), 0.0f, fn_80129DC4(4, -c, c));
        xz[0] = p.x;
        xz[1] = p.z;
        fn_801E9DC4(h);
    } else {
        BPath* path = fn_801B2EC0(car)->vf18();
        f32 d = fn_80129DC4(4, 50.0f, 80.0f);
        BPathPos pp = path->pos;
        f32 w;
        f32 off;
        while (d > pp.pt->len) {
            d -= pp.pt->len;
            pp.pt = pp.pt->next;
        }
        w = pp.pt->width + pp.t * (pp.pt->next->width - pp.pt->width);
        off = fn_80129DC4(4, -w * 0.5f, w * 0.5f);
        pp.Place(xz, off);
        {
            BVec3 p;
            p.x = xz[0];
            p.y = 0.0f;
            p.z = xz[1];
            fn_801E9DC4(h);
            if (!fn_801E9E30(h, &p.x)) {
                pp.Place(xz, 0.0f);
                fn_801E9DC4(h);
            }
        }
    }
}

inline void Ball::vf28(u8* buf) {
    u8* o = buf + a8;
    BMtx* out = (BMtx*)o;
    f32 k = lbl_8055E460;
    if ((f32)__fabs(k - 1.0f) <= lbl_80558DC4[0]) {
        *out = world;
    } else {
        BMtx t;
        int i;
        for (i = 0; i < 4; i++) {
            BVec3* d = &t.rot[i];
            const BVec3* a = &world.rot[i];
            const BVec3* b = &prev.rot[i];
            d->x = b->x + k * (a->x - b->x);
            d->y = b->y + k * (a->y - b->y);
            d->z = b->z + k * (a->z - b->z);
        }
        *out = t;
    }
    *(s16*)(o + 0x30) = hint[0];
    buf[a8 + 0x32] = active;
}

inline bool Ball::GetPos(f32* xz, f32* vxz) {
    if (active) {
        xz[0] = mtx.pos.x;
        xz[1] = mtx.pos.z;
        vxz[0] = vel.x;
        vxz[1] = vel.z;
        return true;
    }
    return false;
}

void Ball::Input(Phob* src, u8* pressed) {
    switch (state) {
    case 1:
        if (!*pressed) {
            state = 2;
        }
        break;
    case 2:
        if (*pressed) {
            Launch(src->World(), src->Vel(), *src->Hint(), 1.0f);
            if (DbgFlag(8)) {
                state = 0;
                timer = fn_8017D378() + 1.5f;
            }
        }
        break;
    }
}

inline const s16* Phob::Hint(void) { return hint; }
inline const BVec3* Phob::Vel(void) { return &vel; }
inline const BMtx* Phob::World(void) { return &world; }
