// Network car (0x801A6370-0x801A75B4): a CarBody driven by position packets from the other machine.
// The last three packets ("netcar:ptab") give a position extrapolation and a velocity from the
// derivative of the quadratic through them, and an orientation slerped between them; the car is
// blended towards that each frame and snapped when a packet asks for it. Names are guesses apart
// from the allocation tag.
#include "game/common.h"

void* operator new[](size_t size, const char* tag);

struct TVec {
    TVec(void) {}
    TVec(f32 ax, f32 ay, f32 az) {
        x = ax;
        y = ay;
        z = az;
    }
    f32 x, y, z;
};

struct TQuat {
    TQuat(void) {}
    f32 x, y, z, w;
};

struct TMtx {
    TVec x;    // offset 0x0
    TVec y;    // offset 0xC
    TVec z;    // offset 0x18
    TVec pos;  // offset 0x24
};

struct ZVec {
    ZVec(void);
    f32 x, y, z;
};

struct PVec {
    PVec(void) {}
    f32 v[3];
};

struct PQuat {
    PQuat(void) {}
    f32 v[4];
};

#pragma pack(push, 1)
// One position packet (0x4D, packed).
struct Packet {
    s32 sent;     // offset 0x0
    s32 frame;    // offset 0x4
    u8 b8;        // offset 0x8
    f32 steer;    // offset 0x9
    f32 throttle; // offset 0xD
    f32 brake;    // offset 0x11
    s32 gear;     // offset 0x15
    f32 f19;      // offset 0x19
    u8 b1D;       // offset 0x1D
    PVec pos;     // offset 0x1E
    PQuat rot;    // offset 0x2A
    u8 pad3A[0x48 - 0x3A];
    u8 snap;      // offset 0x48
    s32 f49;      // offset 0x49
};
#pragma pack(pop)

// The state handed to a body's SetState (vtable 0x54).
struct BodyState {
    TMtx mtx;   // offset 0x0
    TVec vel;   // offset 0x30
    s32 a3C;    // offset 0x3C
    s16 hint;   // offset 0x40
};

struct Blend {
    f32 k;    // offset 0x0
    TVec at;  // offset 0x4
};

struct LinkState {
    u8 pad0[0x24];
    s32 busy;  // offset 0x24
};

class Link {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual LinkState* State(void);  // 0x10
    virtual void vf14(void);
    virtual void vf18(void);
    virtual void vf1C(void);
    virtual void vf20(void);
    virtual void Ready(void);  // 0x24
};

class Proxer {
public:
    void SortCar(int i);
};

class CarBody {
public:
    CarBody(void);
    virtual void Init(void* desc);   // 0x08
    virtual ~CarBody(void);          // 0x0C
    virtual void Reset(void);        // 0x10
    virtual void vf14(void);
    virtual void vf18(void);
    virtual void Update(void);       // 0x1C
    virtual void vf20(void);
    virtual void vf24(void);
    virtual void vf28(void);
    virtual void vf2C(void);
    virtual void vf30(void);
    virtual void vf34(void);
    virtual void vf38(void);
    virtual void vf3C(void);
    virtual void vf40(void);
    virtual void vf44(void);
    virtual void vf48(void);
    virtual void vf4C(void);
    virtual void vf50(void);
    virtual void SetState(const BodyState* s);  // 0x54
    virtual void vf58(void);
    virtual void vf5C(void);

    u8 pad4[0x3C - 0x4];
    TMtx mtx;    // offset 0x3C
    u8 pad6C[0x118 - 0x6C];
    TVec vel;    // offset 0x118
    TVec avel;   // offset 0x124
    u8 pad130[0x1E9 - 0x130];
    u8 b1E9;     // offset 0x1E9
    u8 pad1EA[0x1EC - 0x1EA];
    s32 index;   // offset 0x1EC
    u8 pad1F0[0x2610 - 0x1F0];
    s32 link;    // offset 0x2610
    u8 pad2614[0x267C - 0x2614];
    s32 f267C;   // offset 0x267C
    u8 pad2680[0x2700 - 0x2680];
};

class NetCar : public CarBody {
public:
    NetCar(void);
    virtual void Init(void* desc);
    virtual ~NetCar(void);
    virtual void Reset(void);
    virtual void Update(void);
    void Clear(void);
    int Lost(void) const;
    void Start(void);
    void Extrapolate(Blend* b);
    void Snap(void);
    void Rotate(Blend* b);
    void Predict(void);
    void GetBlend(Blend* b);

    Packet* ptab;     // offset 0x2700
    s32 state;        // offset 0x2704
    ZVec v[3];        // offset 0x2708
    ZVec acc;         // offset 0x272C
    f32 t;            // offset 0x2738
    f32 dt;           // offset 0x273C
    u8 waiting;       // offset 0x2740
    u8 first;         // offset 0x2741
    TMtx home;        // offset 0x2744
    Link* linkObj;    // offset 0x2774
    s32 lastLink;     // offset 0x2778
    s32 time;         // offset 0x277C
};

extern "C" {
void* memset(void* p, int c, u32 n);
double sqrt(double x);
void fn_8012F57C(TQuat* out, const TQuat* a, const TQuat* b, f32 t);  // slerp
void fn_80182074(CarBody* b, void* desc);  // CarBody::Init
void fn_8018381C(CarBody* b);              // CarBody::Reset
void fn_8018ADA4(CarBody* b, f32 v);
void fn_8018ADB4(CarBody* b, f32 v);
void fn_8018ADBC(CarBody* b, f32 v);
void fn_8018ADC4(CarBody* b, f32 v);
void fn_8018ADCC(CarBody* b, f32 v);
void fn_8018ADD4(CarBody* b, s32 v);
Link* fn_801B2EC0(int car);
void fn_801BE168(int car, TMtx* m, TVec* vel, TVec* avel);
int fn_801E89EC(void);
void fn_801E9DC4(s16* h);
void fn_80265D40(CarBody* b, s32 i);
void fn_80265D44(CarBody* b);
Blend* fn_80265D98(void);
s32 fn_802E2C20(void);  // milliseconds
f32 PSVECDotProduct(const TVec* a, const TVec* b);
void Update__10CollVolumeFv(CarBody* b);
}

extern s32 lbl_8055E3C4;  // frame counter
extern Proxer* lbl_8055E0D0;

ModuleTag lbl_8055E5B8;

extern "C" {

f32 fn_801A6370(f32 p, f32 v, f32 a, f32 t) { return p + v * t + t * (0.5f * a * t); }

f32 fn_801A6390(s32 a, s32 b) { return 0.016683351f * (f32)(a - b); }

f32 fn_801A63C4(s32 a, s32 b) { return 0.0192f * fn_801A6390(a, b) / 0.016683351f; }

void fn_801A63F4(TVec* out, const TVec* a, const TVec* b, const TVec* t) {
    out->z = a->z + t->z * (b->z - a->z);
    out->y = a->y + t->y * (b->y - a->y);
    out->x = a->x + t->x * (b->x - a->x);
}
}

ZVec::ZVec(void) { memset(this, 0, sizeof(*this)); }

NetCar::NetCar(void) { ptab = new ("netcar:ptab") Packet[3]; }

void NetCar::Reset(void) {
    fn_8018381C(this);
    Clear();
}

void NetCar::Clear(void) {
    lastLink = 4;
    time = fn_802E2C20();
    dt = 1.0f;
    t = 1.0f;
    state = 0;
    acc.x = 0.0f;
    acc.y = 0.0f;
    acc.z = 0.0f;
    v[0].x = 0.0f;
    v[0].y = 0.0f;
    v[0].z = 0.0f;
    v[1].x = 0.0f;
    v[1].y = 0.0f;
    v[1].z = 0.0f;
    v[2].x = 0.0f;
    v[2].y = 0.0f;
    v[2].z = 0.0f;
    memset(ptab, 0, sizeof(Packet) * 3);
    if (waiting) {
        Start();
    }
}

void NetCar::Init(void* desc) {
    fn_80182074(this, desc);
    linkObj = fn_801B2EC0(index);
    waiting = 0;
    first = 1;
    Clear();
    home.x.x = 1.0f;
    home.x.y = 0.0f;
    home.x.z = 0.0f;
    home.y.x = 0.0f;
    home.y.y = 1.0f;
    home.y.z = 0.0f;
    home.z.x = 0.0f;
    home.z.y = 0.0f;
    home.z.z = 1.0f;
    home.pos.x = 0.0f;
    home.pos.y = 0.0f;
    home.pos.z = 0.0f;
    fn_80265D40(this, index);
    f267C = 1;
}

NetCar::~NetCar(void) {
    fn_80265D44(this);
    delete[] ptab;
    ptab = 0;
}

int NetCar::Lost(void) const {
    int r;
    if (state > 0) {
        r = fn_801A6390(lbl_8055E3C4, ptab[0].frame) > 2.0f;
    } else {
        r = 0;
        if (fn_801E89EC() == 1 && fn_802E2C20() - time > 2000) {
            r = 1;
        }
    }
    return r;
}

void NetCar::Start(void) {
    state = 1;
    vel.y = -1.0f;
}

void NetCar::Update(void) {
    s32 l = link;
    b1E9 = 0;
    if (l != lastLink) {
        time = fn_802E2C20();
    }
    if (!Lost()) {
        if (waiting && linkObj->State()->busy == 0) {
            waiting = 0;
            first = 1;
        }
    } else {
        state = 0;
        waiting = 1;
        mtx = home;
        Update__10CollVolumeFv(this);
        vel.x = 0.0f;
        vel.y = 0.0f;
        vel.z = 0.0f;
        avel.x = 0.0f;
        avel.y = 0.0f;
        avel.z = 0.0f;
        fn_8018ADBC(this, 0.0f);
        fn_8018ADB4(this, 0.0f);
        fn_8018ADC4(this, 0.0f);
        linkObj->Ready();
        fn_801BE168(index, &mtx, &vel, &avel);
    }
    if (!waiting && state == 3) {
        fn_8018ADA4(this, ptab[0].throttle);
        fn_8018ADCC(this, 0.0f);
        if (ptab[0].gear != -1) {
            fn_8018ADD4(this, ptab[0].gear);
        }
        fn_8018ADC4(this, 0.0f);
        fn_8018ADBC(this, ptab[0].steer);
        fn_8018ADB4(this, ptab[0].brake);
        lbl_8055E0D0->SortCar(index);
        if ((l != 2 || lastLink != 0) && l != 0) {
            Predict();
        }
        first = 0;
    }
    lastLink = l;
}

static inline double DL(double t, double ti, double tj, double tk) { return ((t + t - tj) - tk) / ((ti - tj) * (ti - tk)); }

void NetCar::Extrapolate(Blend* b) {
    s32 now = lbl_8055E3C4;
    f32 s = fn_801A63C4(now, ptab[0].frame);
    TVec p;
    TVec d;
    TVec r;
    p.x = fn_801A6370(ptab[0].pos.v[0], v[0].x, acc.x, s);
    p.y = fn_801A6370(ptab[0].pos.v[1], v[0].y, acc.y, s);
    p.z = fn_801A6370(ptab[0].pos.v[2], v[0].z, acc.z, s);
    {
        f32 t1 = fn_801A63C4(ptab[1].frame, ptab[2].frame);
        f32 t0 = fn_801A63C4(ptab[0].frame, ptab[2].frame);
        f32 tn = fn_801A63C4(now, ptab[2].frame);
        double b1 = DL(tn, t1, 0.0, t0);
        double b2 = DL(tn, 0.0, t1, t0);
        double b0 = DL(tn, t0, 0.0, t1);
        d.x = ptab[0].pos.v[0] * b0 + (ptab[2].pos.v[0] * b2 + ptab[1].pos.v[0] * b1);
        d.y = ptab[0].pos.v[1] * b0 + (ptab[2].pos.v[1] * b2 + ptab[1].pos.v[1] * b1);
        d.z = ptab[0].pos.v[2] * b0 + (ptab[2].pos.v[2] * b2 + ptab[1].pos.v[2] * b1);
    }
    fn_801A63F4(&r, &mtx.pos, &p, &b->at);
    mtx.pos = r;
    Update__10CollVolumeFv(this);
    fn_801A63F4(&r, &vel, &d, &b->at);
    vel = r;
}

static inline void QuatToMtx(TMtx* m, const TQuat* q) {
    f32 x = q->x;
    f32 y = q->y;
    f32 z = q->z;
    f32 w = q->w;
    f32 x2 = x + x;
    f32 y2 = y + y;
    f32 z2 = z + z;
    f32 xx = x * x2;
    f32 yy = y * y2;
    f32 zz = z * z2;
    f32 wx = w * x2;
    f32 yz = y * z2;
    f32 wy;
    f32 xz;
    f32 wz;
    f32 xy;
    m->x.x = 1.0f - (yy + zz);
    m->y.y = 1.0f - (xx + zz);
    m->z.z = 1.0f - (xx + yy);
    m->z.y = yz - wx;
    m->y.z = yz + wx;
    wy = w * y2;
    xz = x * z2;
    wz = w * z2;
    xy = x * y2;
    m->z.x = xz + wy;
    m->x.z = xz - wy;
    m->y.x = xy - wz;
    m->x.y = xy + wz;
}

void NetCar::Snap(void) {
    BodyState s;
    s.hint = -1;
    QuatToMtx(&s.mtx, (const TQuat*)&ptab[0].rot);
    s.mtx.pos.x = ptab[0].pos.v[0];
    s.mtx.pos.y = ptab[0].pos.v[1];
    s.mtx.pos.z = ptab[0].pos.v[2];
    s.vel.x = 0.0f;
    s.vel.y = 0.0f;
    s.vel.z = 0.0f;
    s.a3C = 0;
    fn_801E9DC4(&s.hint);
    SetState(&s);
    ptab[2] = ptab[0];
    ptab[1] = ptab[2];
    {
        TVec z(0.0f, 0.0f, 0.0f);
        ptab[1].frame--;
        ptab[2].frame -= 2;
        v[2].x = z.x;
        v[2].y = z.y;
        v[2].z = z.z;
        v[1].x = z.x;
        v[1].y = z.y;
        v[1].z = z.z;
        v[0].x = z.x;
        v[0].y = z.y;
        v[0].z = z.z;
    }
}

extern "C" f32 fn_801A70EC(TQuat* out, const Packet* a, const Packet* b, s32 frame) {
    f32 d = fn_801A63C4(b->frame, a->frame);
    f32 k = fn_801A63C4(frame, a->frame) / d;
    fn_8012F57C(out, (const TQuat*)&a->rot, (const TQuat*)&b->rot, k);
    return k;
}

void NetCar::Rotate(Blend* b) {
    s32 now = lbl_8055E3C4;
    TQuat q1;
    TQuat q2;
    TQuat q;
    TQuat cur;
    f32 tr;
    f32 k;
    fn_801A70EC(&q1, &ptab[1], &ptab[0], now);
    fn_801A70EC(&q2, &ptab[2], &ptab[1], now);
    fn_8012F57C(&q, &q2, &q1, t);
    tr = mtx.x.x + mtx.y.y + mtx.z.z;
    t = t + dt;
    k = b->k;
    if (tr > 0.0f) {
        f32 s = sqrt(1.0f + tr);
        f32 inv = 0.5f / s;
        cur.w = 0.5f * s;
        cur.x = inv * (mtx.y.z - mtx.z.y);
        cur.y = inv * (mtx.z.x - mtx.x.z);
        cur.z = inv * (mtx.x.y - mtx.y.x);
    } else {
        f32* m = &mtx.x.x;
        f32* c = &cur.x;
        int i = 0;
        int j;
        int l;
        f32 s;
        f32 inv;
        if (m[4] > m[0]) {
            i = 1;
        }
        if (m[8] > m[i * 3 + i]) {
            i = 2;
        }
        j = (i + 1) % 3;
        l = (j + 1) % 3;
        s = sqrt(1.0f + (m[i * 3 + i] - (m[j * 3 + j] + m[l * 3 + l])));
        inv = 0.5f / s;
        c[i] = 0.5f * s;
        cur.w = inv * (m[j * 3 + l] - m[l * 3 + j]);
        c[j] = inv * (m[i * 3 + j] + m[j * 3 + i]);
        c[l] = inv * (m[i * 3 + l] + m[l * 3 + i]);
    }
    fn_8012F57C(&q, &cur, &q, k);
    QuatToMtx(&mtx, &q);
}

void NetCar::Predict(void) {
    Blend b;
    GetBlend(&b);
    Extrapolate(&b);
    Rotate(&b);
    avel.x = avel.x * (1.0f - b.k);
    avel.y = avel.y * (1.0f - b.k);
    avel.z = avel.z * (1.0f - b.k);
    if (PSVECDotProduct(&vel, &vel) > 4444.4443f) {
        vel.x = 0.0f;
        vel.y = 1.0f;
        vel.z = 0.0f;
    }
    fn_801BE168(index, &mtx, &vel, &avel);
    if (ptab[0].snap) {
        Snap();
        ptab[0].snap = 0;
    }
}

void NetCar::GetBlend(Blend* b) {
    if (link == 0 || first) {
        b->at.x = 1.0f;
        b->at.y = 1.0f;
        b->at.z = 1.0f;
        b->k = 1.0f;
    } else {
        *b = *fn_80265D98();
    }
}
