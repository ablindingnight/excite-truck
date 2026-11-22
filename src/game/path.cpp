// Recorded paths (0x801D9CC4-0x801DA6EC): "PPth" path files (named paths of timed position + rotation
// keys, byte-swapped on load when needed) and a follower that steps along one path in time and
// evaluates the pose, velocity and angular velocity between two keys. Names are guesses apart from
// the messages.
#include "game/common.h"
#include "game/recpath.h"
#include "game/plainvec.h"
#include "game/byteswap.h"
#include "game/mathutil.h"

extern "C" {
double sqrt(double x);
double acos(double x);
double sin(double x);
int strcmp(const char* a, const char* b);
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
void* fn_8012C370(const char* name, u32 magic, s32* version, int a, int b, u8* swapped);  // load resource
void fn_8012C4D8(void* p);                                                               // free resource
extern f32 __float_epsilon[];
}

#include "game/quat.h"

// One key of a path (0x20).
struct PathKey {
    f32 t;     // offset 0x0
    V3 pos;    // offset 0x4
    Quat rot;  // offset 0x10
};

// PathMtx: common.h's PlainValMatrix.
static inline void SwapKey(PathKey* k) {
    for (unsigned w = 0; w < 2; ++w) {
        u8* words = (u8*)k + w * 16;
        SwapWord(words + 0);
        SwapWord(words + 4);
        SwapWord(words + 8);
        SwapWord(words + 12);
    }
}

ModuleTag lbl_8055E970;

extern "C" {

/* Helper name unproven. The count uses separate byte temporaries. */
static inline void SwapPathCount(RecPath* path) {
#if !defined(__MWERKS__) && !defined(GAME_FILE_DATA_NEEDS_SWAP)
    return;  // PC: the little-endian file data is already native (see byteswap.h)
#endif
    u8* p = (u8*)&path->count;
    u8 first = p[0];
    p[0] = p[3];
    p[3] = first;
    u8 second = p[1];
    p[1] = p[2];
    p[2] = second;
}

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
        RecPath* p;
        PathKey* k;
        int i;
        int j;
        SwapWord(&f->count);
        u8* cursor = (u8*)f->paths;
        for (i = 0; i < f->count; cursor += sizeof(RecPath), ++i) {
            p = (RecPath*)cursor;
            SwapPathCount(p);
            SwapWord(&p->keys);
            p->keys = (PathKey*)((u8*)f + (u32)p->keys);
            for (j = 0; j < p->count; j++) {
                k = &p->keys[j];
                SwapKey(k);
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

static inline RecPath* GetPath(PathFile* f, int i) { return &f->paths[i]; }
static inline int Match(RecPath* p, const char* name) { return strcmp(p->name, name) == 0; }
static inline PathKey* Last(RecPath* p) { return &p->keys[p->count - 1]; }
static inline void Normalize(Quat& q) {
    f32 inv = 1.0f / (f32)sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    q.x *= inv;
    q.y *= inv;
    q.z *= inv;
    q.w *= inv;
}
static inline void ToMatrix(PlainValMatrix* m, const Quat& q) {
    f32 y2 = q.y + q.y;
    f32 z2 = q.z + q.z;
    f32 x2 = q.x + q.x;
    f32 yy = q.y * y2;
    f32 zz = q.z * z2;
    f32 xx = q.x * x2;
    f32 wx = q.w * x2;
    f32 yz = q.y * z2;
    f32 wy = q.w * y2;
    f32 xz = q.x * z2;
    f32 xy = q.x * y2;
    f32 wz = q.w * z2;
    m->basis[0].x = 1.0f - (yy + zz);
    m->basis[1].y = 1.0f - (xx + zz);
    m->basis[2].x = xz + wy;
    m->basis[1].x = xy - wz;
    m->basis[0].y = xy + wz;
    m->basis[2].y = yz - wx;
    m->basis[0].z = xz - wy;
    m->basis[1].z = yz + wx;
    m->basis[2].z = 1.0f - (xx + yy);
}

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
    int n;
    PathFile* f = p->file;
    int i;
    n = f->count;
    for (i = 0; i < n; i++) {
        if (strcmp((char*)f + i * (int)sizeof(RecPath) + 4, name) == 0) {  // signed stride from the file, then the header
            return 1;
        }
    }
    return 0;
}

RecPath* fn_801DA03C(PathFollower* p, int i) {
    PathFile* f = p->file;
    if (i < f->count) {
        return &f->paths[i];
    }
    return 0;
}

int fn_801DA064(PathFollower* p, const char* name) {
    PathFile* f = p->file;
    int n = f->count;
    RecPath* paths = f->paths;
    for (int i = 0; i < n; i++) {
        if (strcmp((char*)paths + i * (int)sizeof(RecPath), name) == 0) {  // signed stride: a plain paths[i] walks a pointer instead of an offset
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
        int i = MinIRev(p->idx, p->path->count - 2);
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

// Helper names unproven: vector steps as the target evaluates them.
static inline void AddV3(void* o_, const void* a_, const void* b_) {
    f32* o = (f32*)o_;
    const f32* a = (const f32*)a_;
    const f32* b = (const f32*)b_;
    o[0] = a[0] + b[0];
    o[1] = a[1] + b[1];
    o[2] = a[2] + b[2];
}
static inline void SubV3(void* o_, const void* a_, const void* b_) {
    f32* o = (f32*)o_;
    const f32* a = (const f32*)a_;
    const f32* b = (const f32*)b_;
    o[0] = a[0] - b[0];
    o[1] = a[1] - b[1];
    o[2] = a[2] - b[2];
}
static inline void LerpPos(PlainValMatrix* m, const PathKey* a, const PathKey* b, f32 f) {
    V3 pos;
    pos.z = Lerp(a->pos.z, b->pos.z, f);
    f32 y2 = b->pos.y;
    f32 y_ = a->pos.y;
    f32 xv;
    pos.y = Lerp(y_, y2, f);
    xv = b->pos.x;
    f32 x_ = a->pos.x;
    pos.x = Lerp(x_, xv, f);
    m->origin.z = pos.z;
    m->origin.y = pos.y;
    m->origin.x = pos.x;
}
static inline void ScaleQ(Quat* o, const Quat* a, f32 k) {
    o->x = a->x * k;
    o->y = a->y * k;
    o->z = a->z * k;
    o->w = a->w * k;
}

static inline void LerpQ(Quat* q, const Quat* a, const Quat* b, f32 f) {  /* Helper name unproven */
    f32 g = 1.0f - f;
    q->w = a->w * g + b->w * f;
    q->z = a->z * g + b->z * f;
    q->y = a->y * g + b->y * f;
    q->x = a->x * g + b->x * f;
}

int fn_801DA294(PathFollower* p, PlainValMatrix* m, V3* vel, V3* angvel) {
    PathKey* a = 0;
    PathKey* b = 0;
    f32 f = 0.0f;
    if (fn_801DA1F8(p, &a, &b, &f)) {
        LerpPos(m, a, b, f);
        Quat q;
        LerpQ(&q, &a->rot, &b->rot, f);
        f32 inv = 1.0f / (f32)sqrt(q.x * q.x + (q.y * q.y + (Len2(q.w, q.z))));
        q.y *= inv;
        q.z *= inv;
        q.x *= inv;
        q.w *= inv;
        ToMatrix(m, q);
        f32 rate = 1.0f / MaxF(0.00001f, b->t - a->t);
        V3 d;
        SubV3(&d.x, &b->pos.x, &a->pos.x);
        vel->x = d.x * rate;
        vel->y = d.y * rate;
        vel->z = d.z * rate;
        Quat na;
        ScaleQ(&na, &a->rot, -1.0f);
        Quat dq;
        AddV3(&dq.x, &b->rot.x, &na.x);
        dq.w = b->rot.w + na.w;
        f32 ang = acos(dq.w);
        f32 s = sin(ang);
        V3 w;
        if ((f32)__fabs(s) > __float_epsilon[0]) {
            f32 k = 2.0f * ang / s;
            w.x = dq.x * k;
            w.y = dq.y * k;
            w.z = dq.z * k;
        } else {
            w.x = 0.0f;
            w.y = 0.0f;
            w.z = 0.0f;
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
