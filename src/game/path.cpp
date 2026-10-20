// Recorded paths (0x801D9CC4-0x801DA6EC): "PPth" path files (named paths of timed position + rotation
// keys, byte-swapped on load when needed) and a follower that steps along one path in time and
// evaluates the pose, velocity and angular velocity between two keys. Names are guesses apart from
// the messages.
#include "game/common.h"

extern "C" {
double sqrt(double x);
double acos(double x);
double sin(double x);
int strcmp(const char* a, const char* b);
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
void* fn_8012C370(const char* name, u32 magic, s32* version, int a, int b, u8* swapped);  // load resource
void fn_8012C4D8(void* p);                                                               // free resource
extern f32 lbl_80558DC4[];
}

struct V3 {
    f32 x;
    f32 y;
    f32 z;
};

struct Quat {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
};

// One key of a path (0x20).
struct PathKey {
    f32 t;     // offset 0x0
    V3 pos;    // offset 0x4
    Quat rot;  // offset 0x10
};

// A named path (0x18).
struct Path {
    char name[0x10];  // offset 0x0
    s32 count;        // offset 0x10
    PathKey* keys;    // offset 0x14
};

struct PathFile {
    s32 count;      // offset 0x0
    Path paths[1];  // offset 0x4
};

struct PathMtx {
    f32 m[3][3];  // offset 0x0
    V3 pos;       // offset 0x24
};

static inline void Swap32(void* v) {
    u8* p = (u8*)v;
    u8 t = p[0];
    p[0] = p[3];
    p[3] = t;
    t = p[1];
    p[1] = p[2];
    p[2] = t;
}

static inline void SwapKey(PathKey* k) {
    Swap32(&k->t);
    Swap32(&k->pos.x);
    Swap32(&k->pos.y);
    Swap32(&k->pos.z);
    Swap32(&k->rot.x);
    Swap32(&k->rot.y);
    Swap32(&k->rot.z);
    Swap32(&k->rot.w);
}

class PathFollower {
public:
    PathFollower(void);
    ~PathFollower(void);
    PathFile* file;  // offset 0x0
    Path* path;      // offset 0x4
    s32 idx;         // offset 0x8
    f32 t;           // offset 0xC
};

ModuleTag lbl_8055E970;

extern "C" {

PathFile* fn_801D9CC4(const char* name) {
    s32 version = 0;
    u8 swapped = 0;
    PathFile* f = (PathFile*)fn_8012C370(name, 'PPth', &version, 0, 0, &swapped);
    if (!f) {
        fn_802E317C("path: can't find %s", name);
        return 0;
    }
    if (version != 0) {
        fn_802E317C("path: version mismatch on %s (%x != %x)", name, version, 0);
        return 0;
    }
    if (swapped) {
        Swap32(&f->count);
        for (int i = 0; i < f->count; i++) {
            Path* p = &f->paths[i];
            Swap32(&p->count);
            Swap32(&p->keys);
            p->keys = (PathKey*)((u8*)f + (u32)p->keys);
            for (int j = 0; j < p->count; j++) {
                PathKey* k = &p->keys[j];
                Swap32(&k->t);
                Swap32(&k->pos.x);
                Swap32(&k->pos.y);
                Swap32(&k->pos.z);
                Swap32(&k->rot.x);
                Swap32(&k->rot.y);
                Swap32(&k->rot.z);
                Swap32(&k->rot.w);
            }
        }
    }
    return f;
}

void fn_801D9F24(PathFile* f) { fn_8012C4D8(f); }

}  // extern "C"

PathFollower::PathFollower(void) {
    file = 0;
    path = 0;
    idx = 0;
    t = 0.0f;
}

PathFollower::~PathFollower(void) {}

static inline Path* GetPath(PathFile* f, int i) { return &f->paths[i]; }
static inline int Match(Path* p, const char* name) { return strcmp(p->name, name) == 0; }
static inline PathKey* Last(Path* p) { return &p->keys[p->count - 1]; }
static inline void Normalize(Quat& q) {
    f32 inv = 1.0f / (f32)sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    q.x *= inv;
    q.y *= inv;
    q.z *= inv;
    q.w *= inv;
}
static inline void ToMatrix(PathMtx* m, const Quat& q) {
    f32 x2 = q.x + q.x;
    f32 y2 = q.y + q.y;
    f32 z2 = q.z + q.z;
    f32 xx = q.x * x2;
    f32 yy = q.y * y2;
    f32 zz = q.z * z2;
    f32 xy = q.x * y2;
    f32 xz = q.x * z2;
    f32 yz = q.y * z2;
    f32 wx = q.w * x2;
    f32 wy = q.w * y2;
    f32 wz = q.w * z2;
    m->m[0][0] = 1.0f - (yy + zz);
    m->m[0][1] = xy + wz;
    m->m[0][2] = xz - wy;
    m->m[1][0] = xy - wz;
    m->m[1][1] = 1.0f - (xx + zz);
    m->m[1][2] = yz + wx;
    m->m[2][0] = xz + wy;
    m->m[2][1] = yz - wx;
    m->m[2][2] = 1.0f - (xx + yy);
}
static inline int MinI(int a, int b) { return (b < a) ? b : a; }
static inline f32 MinF(f32 a, f32 b) { return (a < b) ? a : b; }
static inline f32 MaxF(f32 a, f32 b) { return (a > b) ? a : b; }

extern "C" {

void fn_801D9F84(PathFollower* p, PathFile* f) {
    p->file = f;
    p->idx = 0;
    p->t = 0.0f;
    if (f && f->count > 0) {
        p->path = &f->paths[0];
    } else {
        p->path = 0;
    }
}

int fn_801D9FC4(PathFollower* p, const char* name) {
    PathFile* f = p->file;
    int n = f->count;
    for (int i = 0; i < n; i++) {
        if (strcmp(f->paths[i].name, name) == 0) {
            return 1;
        }
    }
    return 0;
}

Path* fn_801DA03C(PathFollower* p, int i) {
    PathFile* f = p->file;
    if (i < f->count) {
        return &f->paths[i];
    }
    return 0;
}

int fn_801DA064(PathFollower* p, const char* name) {
    PathFile* f = p->file;
    int n = f->count;
    Path* paths = f->paths;
    for (int i = 0; i < n; i++) {
        if (strcmp(paths[i].name, name) == 0) {
            p->path = &paths[i];
            return 1;
        }
    }
    return 0;
}

void fn_801DA0EC(PathFollower* p, f32 dt) {
    if (p->path) {
        p->t += dt;
        if (dt >= 0.0f) {
            while (p->idx + 1 < p->path->count && p->t > p->path->keys[p->idx + 1].t) {
                p->idx++;
            }
        } else {
            while (p->idx > 0 && p->path->keys[p->idx].t > p->t) {
                p->idx--;
            }
        }
    }
}

void fn_801DA198(PathFollower* p, f32 t) {
    p->t = t;
    p->idx = 0;
    if (p->path) {
        while (p->idx + 1 < p->path->count) {
            if (p->path->keys[p->idx + 1].t > p->t) {
                return;
            }
            p->idx++;
        }
    }
}

int fn_801DA1F8(PathFollower* p, PathKey** a, PathKey** b, f32* frac) {
    if (p->path) {
        int i = MinI(p->idx, p->path->count - 2);
        PathKey* k1 = &p->path->keys[i + 1];
        PathKey* k0 = &p->path->keys[i];
        f32 t = MinF(p->t, k1->t);
        f32 len = MaxF(k1->t - k0->t, 0.0001f);
        f32 d = t - k0->t;
        *a = k0;
        *b = k1;
        *frac = d / len;
        return 1;
    }
    return 0;
}

int fn_801DA294(PathFollower* p, PathMtx* m, V3* vel, V3* angvel) {
    PathKey* a = 0;
    PathKey* b = 0;
    f32 f = 0.0f;
    if (fn_801DA1F8(p, &a, &b, &f)) {
        V3 pos;
        pos.z = a->pos.z + f * (b->pos.z - a->pos.z);
        pos.y = a->pos.y + f * (b->pos.y - a->pos.y);
        pos.x = a->pos.x + f * (b->pos.x - a->pos.x);
        m->pos.z = pos.z;
        m->pos.y = pos.y;
        m->pos.x = pos.x;
        f32 g = 1.0f - f;
        Quat q;
        q.w = a->rot.w * g + b->rot.w * f;
        q.z = a->rot.z * g + b->rot.z * f;
        q.y = a->rot.y * g + b->rot.y * f;
        q.x = a->rot.x * g + b->rot.x * f;
        f32 inv = 1.0f / (f32)sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
        q.x *= inv;
        q.y *= inv;
        q.z *= inv;
        q.w *= inv;
        ToMatrix(m, q);
        f32 rate = 1.0f / MaxF(b->t - a->t, 0.00001f);
        V3 d;
        d.x = b->pos.x - a->pos.x;
        d.y = b->pos.y - a->pos.y;
        d.z = b->pos.z - a->pos.z;
        vel->x = d.x * rate;
        vel->y = d.y * rate;
        vel->z = d.z * rate;
        Quat na;
        na.x = a->rot.x * -1.0f;
        na.y = a->rot.y * -1.0f;
        na.z = a->rot.z * -1.0f;
        na.w = a->rot.w * -1.0f;
        Quat dq;
        dq.x = b->rot.x + na.x;
        dq.y = b->rot.y + na.y;
        dq.z = b->rot.z + na.z;
        dq.w = b->rot.w + na.w;
        f32 ang = acos(dq.w);
        f32 s = sin(ang);
        V3 w;
        if ((f32)__fabs(s) > lbl_80558DC4[0]) {
            f32 k = 2.0f * ang / s;
            w.x = dq.x * k;
            w.y = dq.y * k;
            w.z = dq.z * k;
        } else {
            w.x = w.y = w.z = 0.0f;
        }
        angvel->x = w.x * rate;
        angvel->y = w.y * rate;
        angvel->z = w.z * rate;
        return 1;
    }
    return 0;
}

int fn_801DA648(PathFollower* p) {
    if (p->path) {
        return p->t >= Last(p->path)->t;
    }
    return 0;
}

int fn_801DA688(PathFollower* p) {
    if (p->path) {
        return p->t <= 0.0f;
    }
    return 0;
}

f32 fn_801DA6B8(PathFollower* p) {
    if (p->path) {
        return Last(p->path)->t;
    }
    return 0.0f;
}

}  // extern "C"
