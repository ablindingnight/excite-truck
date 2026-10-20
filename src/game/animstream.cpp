// Skeletal animation streams (0x801E68D4-0x801E76A8): "Anim" .an0 resources of packed 64-bit
// keyframes (bone, time, 12-bit quaternion or 16-bit root translation), resident or streamed through
// eight 16K buffers, played on a model's bone set by blending each bone between its last two keys.
// Names are guesses apart from the strings.
#include "game/common.h"

void* operator new(size_t size, const char* tag);

struct AQuat {
    AQuat(void) {}
    f32 x, y, z, w;
};

// One bone's interpolation state (0x28).
struct BoneKey {
    AQuat q0;  // offset 0x0
    AQuat q1;  // offset 0x10
    f32 t0;    // offset 0x20
    f32 t1;    // offset 0x24
};

// A decoded keyframe.
struct AnimKey {
    s32 bone;  // offset 0x0
    f32 t;     // offset 0x4
    AQuat q;   // offset 0x8
};

struct PackedKey {
    u32 hi;  // offset 0x0
    u32 lo;  // offset 0x4
};

// An .an0 resource.
struct AnimRes {
    s32 numFrames;        // offset 0x0
    s32 resident;         // offset 0x4
    s32 numBones;         // offset 0x8
    f32 duration;         // offset 0xC
    char name[16];        // offset 0x10
    PackedKey frames[1];  // offset 0x20
};

struct AnimInst {
    s32 model;            // offset 0x0
    void* skel;           // offset 0x4
    s32 numBones;         // offset 0x8
    u8 loop;              // offset 0xC
    u8 streaming;         // offset 0xD
    AnimRes* res;         // offset 0x10
    void* buf;            // offset 0x14
    s32 frame;            // offset 0x18
    s32 lastFrame;        // offset 0x1C
    AnimKey cur;          // offset 0x20
    f32 time;             // offset 0x38
    BoneKey bones[32];    // offset 0x3C
    AQuat pose[32];       // offset 0x53C
};

extern "C" {
void* memset(void* p, int c, u32 n);
char* strcpy(char* d, const char* s);
char* strcat(char* d, const char* s);
char* strrchr(const char* s, int c);
double sqrt(double x);
void* fn_8012C370(const char* name, u32 type, u32* version, u32* size, u8* first, u8* swapped);
void fn_8012C4D8(void* res);
void* fn_8012EC90(u32 size);
void fn_8012ED20(void* b);
int fn_8012EDA8(void* b, const char* name, u32 type, u32* version);
s32 fn_8012EE78(void* b);
void fn_8012EE80(void* b);
int fn_8012EFB0(void* b, void* dst, u32 n);
void* fn_801E592C(s32 model);
void fn_801E598C(void* skel);
s32 fn_801E5990(void* skel);
s32 fn_801E5E24(void* skel, const AQuat* pose, s32 n, ValMatrix* out);
void fn_801BFD34(ValVec3* out, const ValVec3* v, const ValMatrix* m);
void fn_80015B20(ValMatrix* out, const ValMatrix* a, const ValMatrix* b);
s32 fn_802C6FC0(const char* name, int a, int b);
void fn_802C6CF8(s32 model);
void fn_802C70AC(s32 model, const ValMatrix* m, s32 a, int b);
void fn_802E30C8(const char* fmt, ...);
void fn_802E317C(const char* fmt, ...);
}

#define AMIN(a, b) ((a) < (b) ? (a) : (b))

ModuleTag lbl_8055EAC0;
u8 lbl_8055EAC8[8];  // buffer in use
s32 lbl_8055EAD0;    // load error throttle
void* lbl_80487CC0[8];

extern "C" {

void* fn_801E68D4(void) {
    for (int i = 0; i < 8; i++) {
        if (!lbl_8055EAC8[i]) {
            lbl_8055EAC8[i] = 1;
            return lbl_80487CC0[i];
        }
    }
    return 0;
}

void fn_801E6924(void* b) {
    for (int i = 0; i < 8; i++) {
        if (lbl_80487CC0[i] == b) {
            lbl_8055EAC8[i] = 0;
            return;
        }
    }
    fn_802E317C("anim: release_buffer: unknown buffer %p", b);
}

void fn_801E6978(void) {
    for (int i = 0; i < 8; i++) {
        lbl_80487CC0[i] = fn_8012EC90(0x4000);
        lbl_8055EAC8[i] = 0;
    }
}

void fn_801E69EC(void) {
    for (int i = 0; i < 8; i++) {
        fn_8012ED20(lbl_80487CC0[i]);
    }
}

AnimInst* fn_801E6A3C(const char* name, int streaming, int flags) {
    AnimInst* a;
    s32 model = fn_802C6FC0(name, flags, 0);
    void* skel = fn_801E592C(model);
    if (!skel) {
        fn_802E317C("anim: %s has no bone set", name);
    }
    a = new ("mranm") AnimInst;
    memset(a, 0, sizeof(AnimInst));
    a->numBones = fn_801E5990(skel);
    a->skel = skel;
    a->model = model;
    a->time = 0.0f;
    a->res = 0;
    a->frame = 0;
    a->lastFrame = -1;
    a->streaming = 0;
    if (streaming == 1) {
        a->streaming = 1;
    }
    a->buf = 0;
    a->loop = 0;
    for (int i = 0; i < a->numBones + 1; i++) {
        BoneKey* b = &a->bones[i];
        b->q0.x = 0.0f;
        b->q0.y = 0.0f;
        b->q0.z = 0.0f;
        b->q0.w = 1.0f;
        b->q1.x = 0.0f;
        b->q1.y = 0.0f;
        b->q1.z = 0.0f;
        b->q1.w = 1.0f;
        b->t1 = 0.0f;
        b->t0 = 0.0f;
        a->pose[i].x = 0.0f;
        a->pose[i].y = 0.0f;
        a->pose[i].z = 0.0f;
        a->pose[i].w = 1.0f;
    }
    return a;
}

void fn_801E6B98(AnimInst* a) {
    if (a->buf) {
        fn_801E6924(a->buf);
    }
    fn_801E598C(a->skel);
    fn_802C6CF8(a->model);
    delete a;
}

AnimRes* fn_801E6BEC(const char* name) {
    u32 size = 0;
    u8 first = 0;
    u8 swapped = 0;
    u32 ver;
    AnimRes* r = (AnimRes*)fn_8012C370(name, 'Anim', &ver, &size, &first, &swapped);
    if (!r) {
        fn_802E317C("anim: can't get %s", name);
    }
    if (ver != 2) {
        fn_802E317C("anim: %s is wrong version (%x != %x)", name, ver, 2);
    }
    strcpy(r->name, name);
    char* dot = strrchr(r->name, '.');
    if (dot) {
        *dot = 0;
    }
    strcat(r->name, ".an0");
    return r;
}

void fn_801E6CC4(AnimRes* r) { fn_8012C4D8(r); }

void fn_801E6CC8(AnimInst* a, u8 loop) { a->loop = loop; }

void fn_801E6CD0(AnimInst* a, AnimRes* r, f32 t) {
    a->res = r;
    a->frame = 0;
    a->lastFrame = -1;
    a->time = -t;
    if (r && a->numBones < r->numBones) {
        fn_802E317C("anim: bone-count mismatch:  %d (model) != %d (stream)", a->numBones, r->numBones);
    }
    for (int i = 0; i < a->numBones + 1; i++) {
        BoneKey* b = &a->bones[i];
        b->q0 = b->q1 = a->pose[i];
        b->t1 = -t;
        b->t0 = -t;
    }
    if (r && r->numFrames > r->resident) {
        u32 ver;
        if (!a->streaming) {
            fn_802E317C("anim: tried to play nonresident anim stream with non-streaming model");
        }
        if (!a->buf) {
            a->buf = fn_801E68D4();
        }
        if (!a->buf) {
            a->res = 0;
            return;
        }
        if (!fn_8012EDA8(a->buf, r->name, 'Anm0', &ver)) {
            fn_802E317C("anim: can't get nonresident stream %s", r->name);
        }
        if (ver != 2) {
            fn_802E317C("anim: %s is wrong version (%x != %x)", r->name, 2);
        }
    } else if (a->buf) {
        fn_801E6924(a->buf);
        a->buf = 0;
    }
}

static inline f32 Lerp(f32 lo, f32 range, f32 t) { return lo + t * range; }

static inline f32 Dequant(u32 v, f32 lo, f32 range) { return ((f32)v - lo) / range; }

void fn_801E6E9C(AnimInst* a, f32 dt) {
    AnimRes* r = a->res;
    f32 time = a->time;
    if (r) {
        if (a->buf) {
            fn_8012EE80(a->buf);
        }
        while (a->frame < r->numFrames) {
            AnimKey k;
            if (a->lastFrame != a->frame) {
                PackedKey pk;
                if (a->frame < r->resident) {
                    pk = r->frames[a->frame];
                } else if (!fn_8012EFB0(a->buf, &pk, 8)) {
                    if (lbl_8055EAD0 == 0) {
                        fn_802E30C8("anim: failed loading frame %d @ 0x%x", a->frame, fn_8012EE78(a->buf));
                    }
                    if (++lbl_8055EAD0 > 200) {
                        lbl_8055EAD0 = 0;
                    }
                    break;
                }
                u64 v = ((u64)pk.hi << 32) | pk.lo;
                u32 b6 = pk.lo & 0x3F;
                k.bone = b6 - 1;
                u32 tv = ((u32)(v >> 6) & 0x3FF) << 6;
                f32 c = AMIN(1.0f, Dequant(tv, 0.0f, 65535.0f));
                c = c > 0.0f ? c : 0.0f;
                k.t = Lerp(0.0f, 17.05f, c);
                if (b6 != 0) {
                    u32 qx = ((u32)(v >> 16) & 0xFFF) << 4;
                    u32 qy = ((u32)(v >> 28) & 0xFFF) << 4;
                    u32 qz = (pk.hi >> 4) & 0xFFF0;
                    u32 qw = (pk.hi >> 16) & 0xFFF0;
                    c = AMIN(1.0f, Dequant(qx, 0.0f, 65535.0f));
                    c = c > 0.0f ? c : 0.0f;
                    k.q.x = Lerp(-1.0f, 2.0f, c);
                    c = AMIN(1.0f, Dequant(qy, 0.0f, 65535.0f));
                    c = c > 0.0f ? c : 0.0f;
                    k.q.y = Lerp(-1.0f, 2.0f, c);
                    c = AMIN(1.0f, Dequant(qz, 0.0f, 65535.0f));
                    c = c > 0.0f ? c : 0.0f;
                    k.q.z = Lerp(-1.0f, 2.0f, c);
                    c = AMIN(1.0f, Dequant(qw, 0.0f, 65535.0f));
                    c = c > 0.0f ? c : 0.0f;
                    k.q.w = Lerp(-1.0f, 2.0f, c);
                } else {
                    u32 px = (u32)(v >> 16) & 0xFFFF;
                    u32 py = pk.hi & 0xFFFF;
                    u32 pz = pk.hi >> 16;
                    c = AMIN(1.0f, Dequant(px, 0.0f, 65535.0f));
                    c = c > 0.0f ? c : 0.0f;
                    k.q.x = 20.0f * (Lerp(-1.0f, 2.0f, c));
                    c = AMIN(1.0f, Dequant(py, 0.0f, 65535.0f));
                    c = c > 0.0f ? c : 0.0f;
                    k.q.y = 20.0f * (Lerp(-1.0f, 2.0f, c));
                    c = AMIN(1.0f, Dequant(pz, 0.0f, 65535.0f));
                    c = c > 0.0f ? c : 0.0f;
                    k.q.z = 20.0f * (Lerp(-1.0f, 2.0f, c));
                }
                a->cur = k;
                a->lastFrame = a->frame;
            } else {
                k = a->cur;
            }
            BoneKey* b = &a->bones[k.bone + 1];
            if (!(time >= b->t1)) {
                break;
            }
            b->q0 = b->q1;
            b->q1 = k.q;
            b->t0 = b->t1;
            b->t1 = k.t + (b->t1 > 0.0f ? b->t1 : 0.0f);
            a->frame++;
        }
        a->time = time + dt;
        if (a->loop && a->frame >= r->numFrames && a->time >= r->duration) {
            fn_801E6CD0(a, r, 0.0f);
        }
    }
    for (int i = 0; i < a->numBones + 1; i++) {
        BoneKey* b = &a->bones[i];
        f32 t0 = b->t0;
        if (t0 >= b->t1 - 0.001f) {
            t0 = b->t1 - 0.001f;
        }
        f32 u = (time - t0) / (b->t1 - t0);
        if (1.0f < u) {
            u = 1.0f;
        }
        u = u > 0.0f ? u : 0.0f;
        AQuat* p = &a->pose[i];
        if (i > 0) {
            f32 s = 1.0f - u;
            p->x = b->q0.x * s + b->q1.x * u;
            p->y = b->q0.y * s + b->q1.y * u;
            p->z = b->q0.z * s + b->q1.z * u;
            p->w = b->q0.w * s + b->q1.w * u;
            f32 n = 1.0f / (f32)sqrt(p->x * p->x + (p->y * p->y + (p->z * p->z + p->w * p->w)));
            p->x *= n;
            p->y *= n;
            p->z *= n;
            p->w *= n;
        } else {
            f32 s = 1.0f - u;
            p->x = b->q0.x * s + b->q1.x * u;
            p->y = b->q0.y * s + b->q1.y * u;
            p->z = b->q0.z * s + b->q1.z * u;
            p->w = 0.0f;
        }
    }
}

void fn_801E75C8(AnimInst* a, const ValMatrix* m) {
    ValMatrix mat;
    ValMatrix out;
    s32 r = fn_801E5E24(a->skel, a->pose, a->numBones, &mat);
    fn_801BFD34(&out.origin, &mat.origin, m);
    fn_80015B20(&out, &mat, m);
    fn_802C70AC(a->model, &out, r, -1);
}

int fn_801E7650(AnimInst* a) {
    AnimRes* r = a->res;
    if (r) {
        int done = 0;
        if (a->frame >= r->numFrames && a->time >= r->duration) {
            done = 1;
        }
        return done;
    }
    return 0;
}

int fn_801E7698(void) { return 8; }

}  // extern "C"
