// Network car (0x801A6370-0x801A75B4): a CarBody driven by position packets from the other machine.
// The last three packets ("netcar:ptab") give a position extrapolation and a velocity from the
// derivative of the quadratic through them, and an orientation slerped between them; the car is
// blended towards that each frame and snapped when a packet asks for it. Names are guesses apart
// from the allocation tag.
#include "game/common.h"
#include "game/collvol.h"
#include "game/rigid.h"
#include "game/carbody.h"

void* operator new[](size_t size, const char* tag);

struct TQuat {
    TQuat(void) {}
    f32 x, y, z, w;
};

struct ZVec {
    ZVec(void);
    f32 x, y, z;
};

#pragma pack(push, 1)
struct PVec {
    PVec(void) {}
    f32 v[3];
};

struct PQuat {
    PQuat(void) {}
    f32 v[4];
};

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
    // The packet copy groups the state byte, position, and orientation.
    struct Pose {
        u8 b1D;       // offset 0x1D
        PVec pos;     // offset 0x1E
        PQuat rot;    // offset 0x2A
    } pose;
    u8 pad3A[0x48 - 0x3A];
    u8 snap;      // offset 0x48
    s32 f49;      // offset 0x49
};
#pragma pack(pop)

struct Blend {
    f32 k;    // offset 0x0
    RVec3 at;  // offset 0x4
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

struct RVec3;
struct RPacket;
struct BodyState;
struct BodyDesc;

class NetCar : public CarBody {
public:
    NetCar(void);
    virtual void Init(BodyDesc* desc);
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
    RMtx home;        // offset 0x2744
    Link* linkObj;    // offset 0x2774
    s32 lastLink;     // offset 0x2778
    s32 time;         // offset 0x277C
};

extern "C" {
void* memset(void* p, int c, u32 n);
double sqrt(double x);
void fn_8012F57C(TQuat* out, const TQuat* a, const TQuat* b, f32 t);  // slerp
void fn_8018ADA4(CarBody* b, f32 v);
void fn_8018ADB4(CarBody* b, f32 v);
void fn_8018ADBC(CarBody* b, f32 v);
void fn_8018ADC4(CarBody* b, f32 v);
void fn_8018ADCC(CarBody* b, f32 v);
void fn_8018ADD4(CarBody* b, s32 v);
Link* fn_801B2EC0(int car);
void fn_801BE168(int car, RMtx* m, RVec3* vel, RVec3* avel);
int fn_801E89EC(void);
void fn_801E9DC4(s16* h);
void fn_80265D40(CarBody* b, s32 i);
void fn_80265D44(CarBody* b);
Blend* fn_80265D98(void);
s32 fn_802E2C20(void);  // milliseconds
f32 PSVECDotProduct(const RVec3* a, const RVec3* b);
void Transform__10CollVolumeFv(CarBody* b);
}

extern s32 lbl_8055E3C4;  // frame counter
extern Proxer* lbl_8055E0D0;

ModuleTag lbl_8055E5B8;

extern "C" {

f32 fn_801A6370(f32 p, f32 v, f32 a, f32 t) { return p + v * t + t * (0.5f * a * t); }

f32 fn_801A6390(s32 a, s32 b) { return 0.016683351f * (f32)(a - b); }

f32 fn_801A63C4(s32 a, s32 b) { return 0.0192f * fn_801A6390(a, b) / 0.016683351f; }

static inline f32 LerpF(f32 t, f32 a, f32 b) { return a + t * (b - a); } /* Helper name unproven */
void fn_801A63F4(RVec3* out, const RVec3* a, const RVec3* b, const RVec3* t) {
    f32 startX;
    f32 endX;
    f32 xWeight;
    f32 startY;
    f32 endY;
    f32 yWeight;
    f32 startZ;
    f32 endZ;
    f32 zWeight;
    endZ = b->z;
    startZ = a->z;
    endY = b->y;
    startY = a->y;
    zWeight = t->z;
    endX = b->x;
    startX = a->x;
    yWeight = t->y;
    xWeight = t->x;
    out->z = LerpF(zWeight, startZ, endZ);
    out->y = LerpF(yWeight, startY, endY);
    out->x = LerpF(xWeight, startX, endX);
}
}

ZVec::ZVec(void) { memset(this, 0, sizeof(*this)); }

NetCar::NetCar(void) { ptab = new ("netcar:ptab") Packet[3]; }

void NetCar::Reset(void) {
    CarBody::Reset();
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

void NetCar::Init(BodyDesc* desc) {
    CarBody::Init(desc);
    linkObj = fn_801B2EC0(index);
    waiting = 0;
    first = 1;
    Clear();
    home.rot.x.x = 1.0f;
    home.rot.x.y = 0.0f;
    home.rot.x.z = 0.0f;
    home.rot.y.x = 0.0f;
    home.rot.y.y = 1.0f;
    home.rot.y.z = 0.0f;
    home.rot.z.x = 0.0f;
    home.rot.z.y = 0.0f;
    home.rot.z.z = 1.0f;
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
        Transform__10CollVolumeFv(this);
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

static inline double DL(double t, double ti, double tj, double tk) {
    double den = (ti - tj) * (ti - tk);
    return ((t + t - tj) - tk) / den;
}

void NetCar::Extrapolate(Blend* b) {
    s32 now = lbl_8055E3C4;
    f32 s = fn_801A63C4(now, ptab[0].frame);
    RVec3 p;
    RVec3 d;
    RVec3 r;
    RVec3 r2;
    p.x = fn_801A6370(ptab[0].pose.pos.v[0], v[0].x, acc.x, s);
    p.y = fn_801A6370(ptab[0].pose.pos.v[1], v[0].y, acc.y, s);
    p.z = fn_801A6370(ptab[0].pose.pos.v[2], v[0].z, acc.z, s);
    {
        f32 t1 = fn_801A63C4(ptab[1].frame, ptab[2].frame);
        f32 t0 = fn_801A63C4(ptab[0].frame, ptab[2].frame);
        f32 tn = fn_801A63C4(now, ptab[2].frame);
        double b0 = DL(tn, t0, 0.0, t1);
        double b1 = DL(tn, t1, 0.0, t0);
        double b2 = DL(tn, 0.0, t1, t0);
        f32 x2 = ptab[2].pose.pos.v[0];
        f32 x1 = ptab[1].pose.pos.v[0];
        f32 x0 = ptab[0].pose.pos.v[0];
        d.x = x0 * b0 + (x2 * b2 + x1 * b1);
        f32 y2 = ptab[2].pose.pos.v[1];
        f32 y1 = ptab[1].pose.pos.v[1];
        f32 y0 = ptab[0].pose.pos.v[1];
        d.y = y0 * b0 + (y2 * b2 + y1 * b1);
        f32 z2 = ptab[2].pose.pos.v[2];
        f32 z1 = ptab[1].pose.pos.v[2];
        f32 z0 = ptab[0].pose.pos.v[2];
        d.z = z0 * b0 + (z2 * b2 + z1 * b1);
    }
    fn_801A63F4(&r, &mtx.pos, &p, &b->at);
    mtx.pos = r;
    Transform__10CollVolumeFv(this);
    fn_801A63F4(&r2, &vel, &d, &b->at);
    vel = r2;
}

static inline void QuatToMtx(RMtx* m, const TQuat* q) {
    f32 x2 = q->x + q->x;
    f32 y2 = q->y + q->y;
    f32 z2 = q->z + q->z;
    f32 xx = q->x * x2;
    f32 yy = q->y * y2;
    f32 zz = q->z * z2;
    f32 wx = q->w * x2;
    f32 yz = q->y * z2;
    f32 wy = q->w * y2;
    f32 xz = q->x * z2;
    f32 wz = q->w * z2;
    f32 xy = q->x * y2;
    m->rot.x.x = 1.0f - (yy + zz);
    m->rot.y.x = xy - wz;
    m->rot.z.x = xz + wy;
    m->rot.x.y = xy + wz;
    m->rot.y.y = 1.0f - (xx + zz);
    m->rot.z.y = yz - wx;
    m->rot.x.z = xz - wy;
    m->rot.y.z = yz + wx;
    m->rot.z.z = 1.0f - (xx + yy);
}

#pragma push
#pragma opt_propagation off /* Pragma unproven: keeps the retail BodyState address across hint initialization. */
void NetCar::Snap(void) {
    BodyState s;
    BodyState* state = &s;
    state->hint = -1;
    QuatToMtx(&state->mtx, (const TQuat*)&ptab[0].pose.rot);
    const PVec* p = &ptab[0].pose.pos;
    state->mtx.pos.x = p->v[0];
    state->mtx.pos.y = p->v[1];
    state->mtx.pos.z = p->v[2];
    state->vel.x = 0.0f;
    state->vel.y = 0.0f;
    state->vel.z = 0.0f;
    state->a3C = 0;
    fn_801E9DC4(&s.hint);
    SetState(state);
    ptab[1] = ptab[2] = ptab[0];
    {
        // MATCHING-AID: retail's zero vector stores y, z, then x (the order of netcar's old TVec constructor).
        RVec3 z;
        z.y = 0.0f;
        z.z = 0.0f;
        z.x = 0.0f;
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
#pragma pop

extern "C" f32 fn_801A70EC(TQuat* out, const Packet* a, const Packet* b, s32 frame) {
    f32 d = fn_801A63C4(b->frame, a->frame);
    f32 k = fn_801A63C4(frame, a->frame) / d;
    fn_8012F57C(out, (const TQuat*)&a->pose.rot, (const TQuat*)&b->pose.rot, k);
    return k;
}

// Helper name unproven. Convert using the largest diagonal when the trace is nonpositive.
// Helper name unproven. Convert using the largest diagonal when the trace is nonpositive.
static inline void NetMtxToQuatAxis(const f32 (*m)[3], TQuat* q) {
    f32* c = &q->x;
    int i = 0;
    int j;
    int l;
    f32 s;
    if (m[1][1] > m[0][0]) {
        i = 1;
    }
    if (m[2][2] > m[i][i]) {
        i = 2;
    }
    j = (i + 1) % 3;
    l = (j + 1) % 3;
    s = sqrt(1.0f + (m[i][i] - (m[j][j] + m[l][l])));
    c[i] = 0.5f * s;
    s = 0.5f / s;
    q->w = s * (m[j][l] - m[l][j]);
    c[j] = s * (m[i][j] + m[j][i]);
    c[l] = s * (m[i][l] + m[l][i]);
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
    k = b->k;
    f32 nextTime = t + dt;
    t = nextTime;
    tr = mtx.rot.x.x + mtx.rot.y.y + mtx.rot.z.z;
    f32(*m)[3] = (f32(*)[3])&mtx;
    if (tr > 0.0f) {
        f32 s = sqrt(1.0f + tr);
        f32 inv = 0.5f / s;
        cur.w = 0.5f * s;
        cur.x = inv * (m[1][2] - m[2][1]);
        cur.y = inv * (m[2][0] - m[0][2]);
        cur.z = inv * (m[0][1] - m[1][0]);
    } else {
        NetMtxToQuatAxis(m, &cur);
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
