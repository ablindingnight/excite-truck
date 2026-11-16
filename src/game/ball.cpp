// Ball (0x8017D378-0x8017EB80): the "Ball" physics object, a rigid sphere (BallStrings::VolumeName()) parked under the
// track per car. A player drops or throws it (BallStrings::Arming() -> "Press" status), and it explodes on a hard enough
// hit, pushing every body within range away (BallStrings::ExplodeSound()). Also registered as a status/input listener for
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
// Vec2's empty constructor, which retail calls out of line in Drop, by its mangled name (xz stays a plain array:
// PickDrop takes an f32*).
void __ct__4Vec2Fv(f32* v);
void __ct__10CamFrame34Fv(void* m);
void __ct__4VPosFfff(void* v, f32 x, f32 y, f32 z);
}

struct BMtx;
extern "C" void fn_8017E388(BMtx* m);

void* operator new(size_t size, const char* tag);

extern u32 lbl_8055D430;
extern f32 lbl_8055E3C8;    // time
extern f32 lbl_8055E460;    // tick alpha
extern f32 __float_epsilon[];  // FLT_EPSILON

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

struct BGround {
    u8 pad0[0x28];
    f32 y;  // offset 0x28
};

class Vec2;
// A ground position set from an x/z point (aiobst.cpp defines From: y = 0).
struct Spot {
    void From(const Vec2& v);
};

struct RaceCarRec {
    BGround* Mtx(void);  // weak: the car's frame
    u8 pad0[0x10];
    u32 model;  // offset 0x10
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

static inline f32 LerpT(f32 t, f32 a, f32 b) { return a + t * (b - a); }
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
        xz[0] = LerpT(t, pt->x, pt->next->x);
        xz[1] = LerpT(t, pt->z, pt->next->z);
        dir.x = LerpT(t, pt->dx, pt->next->dx);
        dir.z = LerpT(t, pt->dz, pt->next->dz);
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
RaceCarRec* fn_8017960C(int id);
BRacer* fn_801B2EC0(int id);
void* fn_801769CC(void);
}

class BSound {
public:
    virtual ~BSound(void);  // Sound family: destructor at 0x08
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

class GameSound {
public:
    virtual ~GameSound(void);
};

class Sound3D : public GameSound {
public:
    virtual ~Sound3D(void) {}
    static void operator delete(void* p);
};

extern "C" BSound* fn_802077A4(const char* name, int prio, const BVec3* vel, const BVec3* pos);

class Phob;
extern "C" void fn_8019EECC(Phob* p, BVec3* v);

class CollShape {
public:
    CollShape(Phob* o, const BMtx* m, u32 t, f32 k, f32 s) {
        mtx = m;
        owner = o;
        a8 = 0;
        type = t;
        stiff = k;
        skin = s;
        a20 = 100;
        a1C = 0;
    }
    virtual ~CollShape(void);
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

class SphereShape : public CollShape {
public:
    SphereShape(Phob* o, const BMtx* m, f32 k, f32 s, f32 r, const BVec3& c) : CollShape(o, m, 0x53504852, k, s), center(c) {
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
    virtual ~SphereShape(void);
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

struct BodyDesc {
    u8 pad0[0x8];
    f32 mass;
    f32 ix, iy, iz;
};

struct BallDesc : public BodyDesc {
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
    void AddShape(CollShape* s);
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

struct RVec3;  // rigid.cpp vector (same layout as BVec3)

struct RPacket;
struct RVec3;

class CollVolume {
public:
    virtual void Init(BodyDesc* d);
    virtual ~CollVolume(void);
    virtual void Reset(void);
    virtual void vf14(void);
    virtual void vf18(void);
    virtual void Update(void);
    virtual void vf20(void);
    virtual void vf24(void);
    virtual void vf28(u8* buf);
    virtual s32 vf2C(void);
    virtual void vf30(f32 t, RPacket* a, RPacket* b, s32* na, s32* nb);
    virtual s32 vf34(RPacket* p);
    virtual void vf38(u8* out);
    virtual void vf3C(void);
    virtual int vf40(void);
    virtual int vf44(void);
    void AddShape(CollShape* s);
    s32 a4;
    s32 a8;
    u8 padC[0x3C - 0xC];
    BMtx mtx;
    s16 hint[2];
    BMtx world;
    BMtx prev;
};

class RigidBody : public CollVolume {
public:
    RigidBody(void);
    virtual void Init(BodyDesc* d);
    virtual void Reset(void);
    virtual void vf18(void);
    virtual void Update(void);
    virtual s32 vf2C(void);
    virtual void vf30(f32 t, RPacket* a, RPacket* b, s32* na, s32* nb);
    virtual s32 vf34(RPacket* p);
    virtual void vf3C(void);
    virtual int vf40(void);
    virtual void OnHit(const RVec3* force, const RVec3* at, int kind, void* contact);
    virtual void OnImpulse(const RVec3* force, const RVec3* at, int kind, void* contact, void* by);
    virtual void OnImpulses(RVec3* force, RVec3* torque);
    void Push(Phob* a, void* b, const RVec3* imp, const RVec3* at, int kind, const RVec3* c, void* by, f32 k);
    u8 padD0[0x118 - 0xD0];
    BVec3 vel;
    BVec3 avel;
    u8 pad130[0x1E8 - 0x130];
    u8 b1E8;
    u8 b1E9;
};

class IBallListener {
public:
    virtual void Input(Phob* src, u8* pressed) = 0;
    virtual void Status(const char** text, u8* flash) = 0;
    virtual bool GetPos(f32* xz, f32* vxz) = 0;
};

struct BallStrings {
    static inline const char* ExplodeSound() { return "explode.sfx"; }
    static inline const char* RollSound() { return "chkroll.sfx"; }
    static inline const char* StartSound() { return "chkstart.sfx"; }
    static inline const char* VolumeName() { return "ball-volume"; }
    static inline const char* Exploded() { return "Exploded"; }
    static inline const char* Unknown() { return "UNK"; }
    static inline const char* Pressed() { return "Press  "; }
    static inline const char* Arming() { return "Arming"; }
};

class Ball : public RigidBody, public IBallListener {
public:
    void PredictPos(BVec3& p, RigidBody* b, f32 k);
public:
    Ball(void) {}
    virtual void Init(BodyDesc* d);
    virtual ~Ball(void);
    virtual void Reset(void);
    void Rehome(void);
    void CheckBounds(void);
    void TryDrop(void);
    void Drop(void);
    virtual void Update(void);
    virtual void vf28(u8* buf);
    virtual void OnHit(const RVec3* force, const RVec3* at, int kind, void* contact);
    virtual void Status(const char** text, u8* flash);
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

static inline u32 MakeStartupWord(u32 value) { return value; }

ModuleTag lbl_8055E498;
extern const u32 lbl_8055E4A0 = MakeStartupWord(0xFFFF0000);
extern const u32 lbl_8055E4A4 = MakeStartupWord(0xFFFF0000);
static UnkStatic123 lbl_804348C0;
extern const u32 lbl_8055E4A8 = MakeStartupWord(0x7F000000);

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

static inline f32 SqrtF(f32 v) { return (f32)sqrt(v); }

extern "C" void fn_8017D404(f32 r, f32 force, const BVec3* c, RigidBody* self, int car) {
    RaceCarRec* by = fn_8017960C(car);
    struct Node {
        Phob* obj;
        Node* next;
    };
    struct List {
        Node* head;
    };
    List* lists[5];
    struct { Node* nd; } sp_;  /* MATCHING-AID: one-field local struct; the field is numbered as a temp (structpair.py) */
    int n = 5;
    int i;
    void fn_801B619C(List** out, int* n, const BVec3* c);
    fn_801B619C(lists, &n, c);
    for (i = 0; i < n; i++) {
        for (sp_.nd = lists[i]->head; sp_.nd; sp_.nd = sp_.nd->next) {
            RigidBody* b = sp_.nd->obj->vf40() ? (RigidBody*)sp_.nd->obj : 0;
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
                        fall = 1.0f - SqrtF(d2) / r;
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
                    d.y -= 0.15f;
                    imp.x = d.x * (-fall * force);
                    f32 yv = d.y;
                    imp.y = (-fall * force) * yv;
                    imp.z = d.z * (-fall * force);
                    b->Push(0, 0, (const RVec3*)&imp, (const RVec3*)&at, 1001, 0, by, 1.0f);
                }
            }
        }
    }
}

extern "C" Ball* fn_8017D688(void) { return new ("Ball") Ball; }

void Ball::Init(BodyDesc* desc) {
    SphereShape* s;
    BallDesc* d = static_cast<BallDesc*>(desc);
    RigidBody::Init(desc);
    f32 kk = d->stiffK;
    f32 skin = d->skin;
    f32 radius = 0.0254f * d->radius;
    f32 k = 4.45f * d->stiff / 0.0254f;
    f32 stiff = k * (kk * kk);
    BVec3 c(0.0f, 0.0f, 0.0f);
    car = d->car;
    mtx.pos = Home(car);
    CollVolume::Update();
    explodable = d->explodable;
    launch = d->launch;
    s = new (BallStrings::VolumeName()) SphereShape(reinterpret_cast<Phob*>(this), &mtx, stiff, skin, radius, c);
    s->SetId(1001);
    AddShape(s);
    b1E8 = 1;
    b1E9 = 0;
    resetPending = 1;
    active = 0;
    fn_801D0C58((IBallListener*)this, car, 0);
    startPos = mtx.pos;
    explPos = mtx.pos;
    sndStart = fn_802077A4(BallStrings::StartSound(), 5, &vel, &startPos);
    sndRoll = fn_802077A4(BallStrings::RollSound(), 8, &vel, &mtx.pos);
    sndExpl = fn_802077A4(BallStrings::ExplodeSound(), 5, &vel, &explPos);
}

Ball::~Ball(void) {
    fn_801D0C84((IBallListener*)this);
    if (sndExpl) {
        delete (Sound3D*)sndExpl;
    }
    if (sndRoll) {
        delete (Sound3D*)sndRoll;
    }
    if (sndStart) {
        delete (Sound3D*)sndStart;
    }
}

void Ball::Reset(void) {
    RigidBody::Reset();
    mtx.pos = Home(car);
    CollVolume::Update();
    b1E9 = 0;
    if (DbgFlag(8)) {
        state = 0;
        timer = fn_8017D378() + 1.5f;
    }
    resetPending = 0;
    active = 0;
}

void Ball::OnHit(const RVec3* force, const RVec3* at, int kind, void* contact) {
    if (active && explodable) {
        if ((kind >= 100 || DbgFlag(0x90)) && kind < 1000) {
            struct {
                BVec3 pos;
                BVec3 vel;
            } ev;
            ev.pos = mtx.pos;
            ev.vel = vel;
            fn_801796EC(8, &ev, sizeof(ev));
            fn_8017D404(5.0f, 17500.0f, &mtx.pos, static_cast<RigidBody*>(contact), car);
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
    RigidBody::OnHit(force, at, kind, contact);
}

inline void Ball::Drop(void) {
    s16 h;
    f32 xz[2];
    BVec3 v;
    BMtx m;
    f32 up;
    __ct__4Vec2Fv(xz);
    PickDrop(xz, &h);
    __ct__10CamFrame34Fv(&m);
    fn_8017E388(&m);
    ((Spot*)&m.pos)->From(*(const Vec2*)xz);
    up = DropHeight();
    f32 base = fn_8017960C(car)->Mtx()->y;
    m.pos.y = base + up;
    __ct__4VPosFfff(&v, 0.0f, 0.0f, 0.0f);
    Launch(&m, &v, h, 0.0f);
}

inline void Ball::TryDrop(void) {
    if (!active && DbgFlag(0x80)) {
        if (fn_801B2EC0(car)->vf10()->f24 == 0) {
            Drop();
        }
    }
}

inline void Ball::CheckBounds(void) {
    struct P {
        f32 v[3];
    } p = *(P*)&mtx.pos;
    if (!fn_801E9E30(hint, p.v)) {
        resetPending = 1;
    }
}

inline void Ball::Rehome(void) {
    mtx.pos = Home(car);
    CollVolume::Update();
}

void Ball::Update(void) {
    if (resetPending) {
        RigidBody::Reset();
        Rehome();
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
        CollVolume::Update();
        vel.x = 0.0f;
        vel.y = 0.0f;
        vel.z = 0.0f;
    }
    TryDrop();
    CheckBounds();
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

// Helper names unproven: vector operations written as inlines, as the target evaluates them.
static inline void AddSB(BVec3* o, const BVec3* a, f32 s) {
    o->x += a->x * s;
    o->y += a->y * s;
    o->z += a->z * s;
}
static inline void ScaleB(BVec3* o, const BVec3* a, f32 s) {
    o->x = a->x * s;
    o->y = a->y * s;
    o->z = a->z * s;
}
static inline void AccB(BVec3* o, const BVec3* a) {
    o->x += a->x;
    o->y += a->y;
    o->z += a->z;
}
void Ball::Launch(const BMtx* in, const BVec3* v, s16 h, f32 k) {
    BMtx m;
    BVec3 p;
    fn_8017D378();
    avel.x = 0.0f;
    avel.y = 0.0f;
    avel.z = 0.0f;
    hint[0] = h;
    b1E9 = 1;
    m = *in;
    Rotate(&p, &m, &launch);
    p += m.pos;
    m.pos = p;
    mtx = m;
    CollVolume::Update();
    if (DbgFlag(0x80)) {
        vel.x = 0.0f;
        vel.y = -19.62f;
        vel.z = 0.0f;
    } else if (DbgFlag(0x10)) {
        BVec3 d;
        ScaleB(&d, &m.rot[2], 0.707f);
        AddSB(&d, &m.rot[1], 0.707f * 0.7f);
        ScaleB(&vel, &d, (128.0f / 3.6f) * k);
        AccB(&vel, v);
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

/* Helper name unproven: position k seconds ahead along the body velocity. */
inline void Ball::PredictPos(BVec3& p, RigidBody* b, f32 k) {
    p.x = b->mtx.pos.x + b->vel.x * k;
    p.y = b->mtx.pos.y + b->vel.y * k;
    p.z = b->mtx.pos.z + b->vel.z * k;
}

void Ball::PickDrop(f32* xz, s16* h) {
    if (*(u32*)((u8*)fn_801769CC() + 4) & 0x10000) {
        RigidBody* b = (RigidBody*)fn_8017960C(car);
        BVec3 p;
        f32 c;
        PredictPos(p, b, 2.0f);
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
        f32 ppT = pp.t;
        w = pp.pt->width + ppT * (pp.pt->next->width - pp.pt->width);
        off = fn_80129DC4(4, -w / 2.0f, w / 2.0f);
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

static inline void LerpMtx(BMtx* out, const BMtx* a, const BMtx* b, f32 t) {
    f32* d = (f32*)out;
    const f32* x = (const f32*)a;
    const f32* y = (const f32*)b;
    for (int i = 0; i < 12; i++) {
        *d++ = LerpT(t, *x++, *y++);
    }
}

void Ball::vf28(u8* buf) {
    u8* o = buf + a8;
    BMtx* out = (BMtx*)o;
    if ((f32)__fabs(lbl_8055E460 - 1.0f) <= __float_epsilon[0]) {
        *out = world;
    } else {
        BMtx t;
        LerpMtx(&t, &prev, &world, lbl_8055E460);
        *out = t;
    }
    *(s16*)(o + 0x30) = hint[0];
    buf[a8 + 0x32] = active;
}

bool Ball::GetPos(f32* xz, f32* vxz) {
    if (active) {
        xz[0] = mtx.pos.x;
        xz[1] = mtx.pos.z;
        vxz[0] = vel.x;
        vxz[1] = vel.z;
        return true;
    }
    return false;
}

void Ball::Status(const char** text, u8* flash) {
    switch (state) {
    case 0:
        *text = BallStrings::Arming();
        *flash = 0;
        return;
    case 3:
        *text = BallStrings::Exploded();
        *flash = 0;
        return;
    case 1:
        *text = 0;
        *flash = 0;
        return;
    case 2:
        *text = BallStrings::Pressed();
        *flash = 1;
        return;
    default:
        *text = BallStrings::Unknown();
        *flash = 0;
        return;
    }
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
