// Car placement (0x801B7688-0x801B81E0): puts a car at its start or respawn point ("physics/PlaceCar")
// - on the grid behind the start line, at a distance along the ideal line, or on a given position and
// heading - by sampling the ground under three points around the car to get its up vector and building
// its basis from that. Names are guesses apart from the message.
#include "game/common.h"

extern "C" {
double sqrt(double x);
double cos(double x);
double sin(double x);
}

struct V2 {
    f32 x;
    f32 y;
};

typedef ValVec3 V3;

struct IdealPoint {
    V2 pos;            // offset 0x0
    V2 dir;            // offset 0x8
    u32 ilpos;         // offset 0x10 (16.16)
    IdealPoint* next;  // offset 0x14
    f32 len;           // offset 0x18
    f32 dist;          // offset 0x1C
};

struct CenterLine {
    u8 pad0[0x1C];
    IdealPoint* points;  // offset 0x1C
    IdealPoint* start;   // offset 0x20
};

class PathSet {
public:
    virtual void vf08(void);
    virtual CenterLine* GetLine(int index);  // offset 0xC
};

struct ILPos {
    ILPos(void) {
        seg = 0;
        mode = 10000;
        t = 0.0f;
    }
    IdealPoint* seg;  // offset 0x0
    f32 t;            // offset 0x4
    s32 mode;         // offset 0x8
};

struct LineFollower {
    u8 pad0[0x2C];
    CenterLine* line;  // offset 0x2C
};

class Racer {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void vf18(void);
    virtual LineFollower* Follower(void);  // 0x1C
};

struct League {
    u8 pad0[4];
    f32 carLen;  // offset 0x4
};

struct LocalCarDesc {
    u32 unk0;
    u32 flags;  // offset 0x4
    u8 pad8[8];
    u32 flags10;  // offset 0x10
};

struct GroundInfo {
    s32 a;
    s32 b;
};

struct Respawn {
    ValMatrix place;  // offset 0x0
    ValVec3 vel;   // offset 0x30
    s32 unk3C;     // offset 0x3C
    s16 hint;      // offset 0x40
    u8 pad42[2];
    s32 mode;      // offset 0x44
    u8 pad48[8];
};

struct ObjTeleport {
    u8 pad0[0x18];
    f32 dx;  // offset 0x18
    u8 pad1C[4];
    f32 dz;  // offset 0x20
    f32 x;   // offset 0x24
    u8 pad28[4];
    f32 z;   // offset 0x2C
};

extern "C" {
void* memset(void* p, int c, u32 n);
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
void PSVECCrossProduct(const V3* a, const V3* b, V3* out);
void PSVECNormalize(const V3* a, V3* out);
LocalCarDesc* fn_801769CC(void);
int fn_801769B8(void);
u32 fn_801769A8(void);
int fn_801796B4(void);
ILPos fn_80135B24(const LineFollower* f, f32 m);
ILPos fn_80136468(LineFollower* f, f32 m);
League* fn_80137C54(s32 i);
f32 fn_80139048(void);
Racer* fn_801B2ED0(int id);
int fn_801B3050(int car);
PathSet* fn_80156A40(void);
LineFollower* fn_80178C9C(void);
f32 fn_80134C1C(void);
void fn_801E9DC4(s16* h);
int fn_801E9E30(s16* h, const V3* pos);
int fn_801E9E78(s16* h, V3* pos, V3* normal, GroundInfo* info);
int fn_801B8630(Respawn* r, int car, int mode);
void fn_801B7C8C(Respawn* r, const V2* pos, const V2* dir);
void fn_801B8054(Respawn* r, LineFollower* f, f32 side, f32 dist);
}

ModuleTag lbl_8055E730;

extern "C" {

int fn_801B7688(int mode) {
    LocalCarDesc* d = fn_801769CC();
    int r = 1;
    if (d->flags & 0x100) {
        int a = fn_801769B8();
        int z = (a == 0);
        switch (mode) {
        case 0x51:
            if (z || a == 1) {
                r = 0;
            }
            break;
        case 0x50:
        case 0x52:
            if (z) {
                r = 0;
            }
            if (a == 1) {
                r = 3;
            }
            if (fn_801769A8() & 0x400) {
                r = 4;
            }
            break;
        case 0x53:
            break;
        }
    } else {
        r = 2;
    }
    if (fn_801796B4() && (u32)(mode - 0x50) <= 1) {
        r = 0;
    }
    return r;
}

static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

void fn_801B7760(Respawn* r, const ILPos* p, f32 side, f32 angle) {
    f32 nx;
    IdealPoint* s = p->seg;
    f32 t = p->t;
    V2 pos;
    pos.x = Lerp(s->pos.x, s->next->pos.x, t);
    pos.y = Lerp(s->pos.y, s->next->pos.y, t);
    V2 dir;
    dir.x = Lerp(s->dir.x, s->next->dir.x, t);
    dir.y = Lerp(s->dir.y, s->next->dir.y, t);
    f32 inv = 1.0f / (f32)sqrt(dir.x * dir.x + dir.y * dir.y);
    nx = dir.x * inv;
    dir.x = nx;
    dir.y *= inv;
    V2 n = dir;
    f32 tmp = n.x;
    n.x = n.y;
    n.y = -tmp;
    pos.x += n.x * side;
    pos.y += n.y * side;
    f32 c = cos(angle);
    f32 sn = sin(angle);
    f32 x = dir.x * c - dir.y * sn;
    dir.y = nx * sn + dir.y * c;
    dir.x = x;
    fn_801B7C8C(r, &pos, &dir);
    r->unk3C = 0;
}

void fn_801B78EC(Respawn* r, LineFollower* f, int byIlpos, f32 side, f32 dist, f32 angle) {
    ILPos p;
    if (byIlpos) {
        p = fn_80135B24(f, dist);
    } else {
        p = fn_80136468(f, dist);
    }
    fn_801B7760(r, &p, side, angle);
}

f32 fn_801B79AC(void) { return fn_80137C54(-1)->carLen; }

void fn_801B79D4(Respawn* r, int car, int mode) {
    LineFollower* f = fn_801B2ED0(car)->Follower();
    f32 row = fn_80139048();
    f32 len = fn_801B79AC();
    row = 0.61f + (len + row);
    if (mode != 0x53 && mode != 0x54) {
        car = fn_801B3050(car);
    }
    f32 side;
    if (car & 1) {
        side = 5.0f;
    } else {
        side = -5.0f;
    }
    row = 0.5f * -fn_801B79AC() - (f32)(car / 2) * row;
    f32 back = 100.0f;
    IdealPoint* first = fn_80156A40()->GetLine(0)->points;
    IdealPoint* p = first;
    IdealPoint* best = first;
    do {
        if (p->ilpos > best->ilpos) {
            best = p;
        }
        p = p->next;
    } while (p != first);
    back = best->dist - back;
    row = (fn_801769CC()->flags10 & 1) ? row : back + row;
    fn_801B78EC(r, f, 0, side, row, 0.0f);
    if (r->mode == 1) {
        f32 v = fn_80134C1C();
        r->vel.x = r->place.basis[2].x * v;
        r->vel.y = r->place.basis[2].y * v;
        r->vel.z = r->place.basis[2].z * v;
    } else {
        r->vel.x = 0.0f;
        r->vel.y = 0.0f;
        r->vel.z = 0.0f;
    }
}

void fn_801B7BA8(ValMatrix* m, const V3* up, const V2* d) {
    V3 u;
    u.x = up->x;
    u.y = up->y;
    u.z = up->z;
    V3 f;
    f.x = d->x;
    f.y = 0.0f;
    f.z = d->y;
    V3 rt;
    PSVECCrossProduct(&u, &f, &rt);
    PSVECCrossProduct(&rt, &u, &f);
    PSVECNormalize(&rt, &rt);
    PSVECNormalize(&u, &u);
    PSVECNormalize(&f, &f);
    m->basis[0].x = rt.x;
    m->basis[0].y = rt.y;
    m->basis[0].z = rt.z;
    m->basis[1].x = u.x;
    m->basis[1].y = u.y;
    m->basis[1].z = u.z;
    m->basis[2].x = f.x;
    m->basis[2].y = f.y;
    m->basis[2].z = f.z;
}

void fn_801B7C8C(Respawn* r, const V2* pos, const V2* dir) {
    V2 n = *dir;
    ValMatrix m;
    V3 up;
    V3 pts[3];
    m.origin.x = pos->x;
    m.origin.y = 0.0f;
    m.origin.z = pos->y;
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    f32 tmp = n.x;
    n.x = n.y;
    n.y = -tmp;
    pts[0].x = pos->x + dir->x * -0.5f;
    pts[0].y = 0.0f;
    pts[0].z = pos->y + dir->y * -0.5f;
    f32 ax = pos->x + dir->x * 0.5f + n.x * 0.5f;
    f32 ay = pos->y + dir->y * 0.5f + n.y * 0.5f;
    pts[1].x = ax;
    pts[1].y = 0.0f;
    pts[1].z = ay;
    V2 b;
    b.x = ax + n.x * -1.0f;
    b.y = ay + n.y * -1.0f;
    pts[2].x = b.x;
    pts[2].y = 0.0f;
    pts[2].z = b.y;
    GroundInfo info;
    fn_801E9DC4(&r->hint);
    int fail = 0;
    for (int i = 0; i < 3; i++) {
        if (!fn_801E9E30(&r->hint, &pts[i])) {
            fail = 1;
            break;
        }
    }
    if (!fail) {
        fn_801E9E78(&r->hint, &m.origin, &up, &info);
        V3 e1;
        e1.x = pts[1].x - pts[0].x;
        e1.y = pts[1].y - pts[0].y;
        e1.z = pts[1].z - pts[0].z;
        V3 e2;
        e2.x = pts[2].x - pts[0].x;
        e2.y = pts[2].y - pts[0].y;
        e2.z = pts[2].z - pts[0].z;
        PSVECCrossProduct(&e2, &e1, &up);
        PSVECNormalize(&up, &up);
    } else {
        fn_801E9DC4(&r->hint);
        fn_801E9E78(&r->hint, &m.origin, &up, &info);
    }
    fn_801B7BA8(&m, &up, dir);
    f32 h = 0.6286f;
    m.origin.x += m.basis[1].x * h;
    m.origin.y += m.basis[1].y * h;
    m.origin.z += m.basis[1].z * h;
    r->place = m;
    r->unk3C = 0;
    r->vel.x = 0.0f;
    r->vel.y = 0.0f;
    r->vel.z = 0.0f;
}

void fn_801B7F4C(Respawn* r, int slot, int mode) {
    f32 spacing = 20.0f;
    if (fn_80178C9C()->line->start) {
        IdealPoint* st = fn_80178C9C()->line->start;
        f32 base = 200.0f + st->dist;
        fn_801B8054(r, fn_80178C9C(), 0.0f, (10.0f + base) + (f32)slot * spacing);
    } else {
        fn_801B78EC(r, fn_80178C9C(), 0, 0.0f, (f32)slot * spacing, 0.0f);
    }
}

void fn_801B8040(Respawn* r, const ILPos* p, f32 side) { fn_801B7760(r, p, side, 0.0f); }

void fn_801B8048(Respawn* r, LineFollower* f, f32 side, f32 dist) { fn_801B78EC(r, f, 0, side, dist, 0.0f); }

void fn_801B8054(Respawn* r, LineFollower* f, f32 side, f32 dist) { fn_801B78EC(r, f, 1, side, dist, 0.0f); }

static inline f32 Max(f32 a, f32 b) { return (a > b) ? a : b; }

void fn_801B8060(Respawn* r, const ObjTeleport* t) {
    V2 pos;
    V2 dir;
    pos.x = t->x;
    pos.y = t->z;
    dir.x = t->dx;
    dir.y = t->dz;
    f32 inv = 1.0f / Max(0.01f, (f32)sqrt(dir.x * dir.x + dir.y * dir.y));
    dir.x *= inv;
    dir.y *= inv;
    fn_801B7C8C(r, &pos, &dir);
}

void fn_801B8100(Respawn* r, int car, int mode) {
    memset(r, 0, sizeof(Respawn));
    if (fn_801796B4() || !fn_801B8630(r, car, mode)) {
        int m = fn_801B7688(mode);
        r->mode = m;
        switch (m) {
        case 0:
        case 3:
        case 4:
            fn_801B7F4C(r, car, mode);
            break;
        case 1:
        case 2:
            fn_801B79D4(r, car, mode);
            break;
        default:
            fn_802E317C("physics/PlaceCar: unknown start mode %d", m);
            break;
        }
    }
}

}  // extern "C"
