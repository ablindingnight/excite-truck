// Car placement (0x801B7688-0x801B81E0): puts a car at its start or respawn point ("physics/PlaceCar")
// - on the grid behind the start line, at a distance along the ideal line, or on a given position and
// heading - by sampling the ground under three points around the car to get its up vector and building
// its basis from that. Names are guesses apart from the message.
#include "game/common.h"
#include "game/objteleport.h"
#include "game/pathset.h"
#include "game/deity.h"
#include "game/idealline.h"
#include "game/v2.h"
#include "game/raceopts.h"
#include "game/respawn.h"
#include <revolution/MTX/vec.h>
#include "game/mathutil.h"
// Lane offset of the grid columns (5.0f). A named constant defined before any code: retail keeps it as the FIRST
// .sdata2 entry and negates it at run time, so its value is not folded where it is used. A one-field const struct
// gives exactly that (type and name unproven; a plain `const f32` defined here folds -x into a second literal).
struct GridTune { f32 laneOffset; };
extern const GridTune lbl_80562700;
const GridTune lbl_80562700 = {5.0f};

extern "C" {
double sqrt(double x);
double cos(double x);
double sin(double x);
}

typedef ValVec3 V3;

#include "game/league.h"

struct GroundInfo {
    s32 a;
    s32 b;
};

// Respawn, CarPlace: game/respawn.h.
extern "C" {
void* memset(void* p, int c, u32 n);
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
int fn_801769B8(void);
u32 fn_801769A8(void);
int fn_801796B4(void);
ILPos fn_80135B24(const LineFollower* f, f32 m);
ILPos fn_80136468(LineFollower* f, f32 m);
f32 fn_80139048(void);
DeityCar* fn_801B2ED0(int id);
int fn_801B3050(int car);
PathSet* fn_80156A40(void);
LineFollower* fn_80178C9C(void);
f32 fn_80134C1C(void);
void fn_801E9DC4(s16* h);
int fn_801E9E30(s16* h, const V3* pos);
int fn_801E9E78(s16* h, V3* pos, V3* normal, GroundInfo* info);
int fn_801B8630(Respawn* r, int car, int mode);
void fn_801B7C8C(CarPlace* r, const V2* pos, const V2* dir);
void fn_801B8054(CarPlace* r, LineFollower* f, f32 side, f32 dist);
}

ModuleTag lbl_8055E730;

extern "C" {

int fn_801B7688(int mode) {
    EventDesc* d = fn_801769CC();
    int r = 1;
    if (d->type & 0x100) {
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


void fn_801B7760(CarPlace* r, const ILPos* p, f32 side, f32 angle) {
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

void fn_801B78EC(CarPlace* r, LineFollower* f, int byIlpos, f32 side, f32 dist, f32 angle) {
    ILPos p;
    if (byIlpos) {
        p = fn_80135B24(f, dist);
    } else {
        p = fn_80136468(f, dist);
    }
    fn_801B7760(r, &p, side, angle);
}

f32 fn_801B79AC(void) { return fn_80137C54(-1)->carLen; }


#pragma push
#pragma opt_lifetimes off
void fn_801B79D4(Respawn* r, int car, int mode) {
    LineFollower* f = fn_801B2ED0(car)->Line();
    f32 row = fn_80139048();
    f32 len = fn_801B79AC();
    row = 0.61f + (len + row);
    if (mode != 0x53 && mode != 0x54) {
        car = fn_801B3050(car);
    }
    f32 side;
    if (car & 1) {
        side = lbl_80562700.laneOffset;
    } else {
        side = -lbl_80562700.laneOffset;
    }
    row = -fn_801B79AC() / 2.0f - (f32)(car / 2) * row;
    f32 back = 100.0f;
    IdealPoint* first = ((CenterLine*)fn_80156A40()->GetRacingLine(0))->points;
    IdealPoint* p = first;
    IdealPoint* best = first;
    do {
        if (p->ilpos > best->ilpos) {
            best = p;
        }
        p = p->next;
    } while (p != first);
    back = best->dist - back;
    row = (fn_801769CC()->flags & 1) ? row : back + row;
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
#pragma pop

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
    PSVECCrossProduct((const Vec*)&u, (const Vec*)&f, (Vec*)&rt);
    PSVECCrossProduct((const Vec*)&rt, (const Vec*)&u, (Vec*)&f);
    PSVECNormalize((const Vec*)&rt, (Vec*)&rt);
    PSVECNormalize((const Vec*)&u, (Vec*)&u);
    PSVECNormalize((const Vec*)&f, (Vec*)&f);
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

}
template <class O, class A, class V> static inline void TAddS(O* o, const A* a, const V* v, f32 k) { o->x = a->x + v->x * k; o->y = a->y + v->y * k; o->z = a->z + v->z * k; }  /* Helper name unproven */
extern "C" {
/* Name unproven: the 10.0f of fn_801B7F4C is retail's @335, created BEFORE the literals of fn_801B7C8C, so it is a
   named constant defined here (emitted at its definition point); the one-field struct keeps the use from folding
   into a second pooled 10.0f. Replaces a dead static function that blocked linking (extra .text). */
struct SlotConst {
    f32 v;
};
extern const SlotConst lbl_80562720;
const SlotConst lbl_80562720 = {10.0f};

/* Helper name unproven */
static inline void Add2(V2* o, const V2* a, const V2* v, f32 k) { o->x = a->x + v->x * k; o->y = a->y + v->y * k; }
/* Helper name unproven */
static inline void Flat(V3* o, const V2* v) { o->x = v->x; o->y = 0.0f; o->z = v->y; }

void fn_801B7C8C(CarPlace* r, const V2* pos, const V2* dir) {
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
    V2 b;
    Add2(&b, pos, dir, -0.5f);
    Flat(&pts[0], &b);
    Add2(&b, pos, dir, 0.5f);
    Add2(&b, &b, &n, 0.5f);
    Flat(&pts[1], &b);
    Add2(&b, &b, &n, -1.0f);
    Flat(&pts[2], &b);
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
        PSVECCrossProduct((const Vec*)&e2, (const Vec*)&e1, (Vec*)&up);
        PSVECNormalize((const Vec*)&up, (Vec*)&up);
    } else {
        fn_801E9DC4(&r->hint);
        fn_801E9E78(&r->hint, &m.origin, &up, &info);
    }
    fn_801B7BA8(&m, &up, dir);
    TAddS(&m.origin, &m.origin, &m.basis[1], 0.6286f);
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
        fn_801B8054(r, fn_80178C9C(), 0.0f, (lbl_80562720.v + base) + (f32)slot * spacing);
    } else {
        fn_801B78EC(r, fn_80178C9C(), 0, 0.0f, (f32)slot * spacing, 0.0f);
    }
}

void fn_801B8040(CarPlace* r, const ILPos* p, f32 side) { fn_801B7760(r, p, side, 0.0f); }

void fn_801B8048(CarPlace* r, LineFollower* f, f32 side, f32 dist) { fn_801B78EC(r, f, 0, side, dist, 0.0f); }

void fn_801B8054(CarPlace* r, LineFollower* f, f32 side, f32 dist) { fn_801B78EC(r, f, 1, side, dist, 0.0f); }


void fn_801B8060(CarPlace* r, const ObjTeleport* t) {
    V2 pos;
    V2 dir;
    pos.x = t->mtx.origin.x;
    pos.y = t->mtx.origin.z;
    dir.x = t->mtx.basis[2].x;
    dir.y = t->mtx.basis[2].z;
    f32 inv = 1.0f / MaxF(0.01f, (f32)sqrt(dir.x * dir.x + dir.y * dir.y));
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
