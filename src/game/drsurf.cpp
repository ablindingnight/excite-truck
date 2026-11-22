// Surface drawing (0x802D3F34-0x802D7564, "drgc/surf"): draws NDL2 surfaces through their display lists,
// one path per vertex format - plain, two-bone weighted (bone deltas or skin matrices), morph targets,
// "y-morph" wobble, shadow-volume extrusion and the distance-faded environment layer. Large deformed
// meshes are queued and run through the locked cache in one batch. Also loads (byte-swaps and relocates)
// surfaces and builds and caches the per-format vertex descriptor display lists. Names are guesses apart
// from the strings.
#include "game/common.h"
#include "game/surf.h"
#include "game/texstate.h"
#include <revolution/GX/GXAttr.h>
#include <revolution/GX/GXDisplayList.h>
#include <revolution/GX/GXGeometry.h>
#include <revolution/GX/GXPixel.h>
#include <revolution/GX/GXTransform.h>
#include <revolution/MTX/mtx.h>
#include <revolution/MTX/vec.h>
#include <revolution/OS/OSCache.h>
#include "game/math_types.h"
#include "game/plainvec.h"
#include "revolution/GX/GXTypes.h"
#include "game/byteswap.h"
#include "game/mathutil.h"

template <class T>
static inline void SetXYZ(T* v, f32 x, f32 y, f32 z) { /* Helper name unproven */
    v->x = x;
    v->y = y;
    v->z = z;
}

// GXVtxDescList: the SDK's.

// SkinMtx: common.h's PlainValMatrix.
extern "C" {
void* memset(void* dst, int c, size_t n);
void fn_802E317C(const char* fmt, ...);  // fatal
void fn_802E30C8(const char* fmt, ...);  // warning print
void* fn_802CFF64(u32 size);              // frame allocation
void fn_801282EC(int align);
int fn_802DAC64(void);
void fn_802D0E48(int pass);
void fn_802D2614(void);
void fn_802D2B0C(void);
const V3* fn_802D2D04(void);  // light direction
void LCLoadData(void* dst, const void* src, u32 n);
}

void* operator new[](size_t size, const char* tag);
void operator delete[](void* p);

union WGPipe {
    u8 uc;
    u32 u;
    f32 f;
};

#define WGPIPE (*(volatile WGPipe*)0xCC008000)

// Two-bone weighted vertex.
struct SkinVtx {
    f32 x, y, z;  // offset 0x0
    u8 b0;        // offset 0xC
    u8 b1;        // offset 0xD
    u8 w0;        // offset 0xE
    u8 w1;        // offset 0xF
};

// Bone-delta vertex with one weight.
struct DirVtx {
    f32 x, y, z;  // offset 0x0
    f32 w;        // offset 0xC
};

// Two morph targets.
struct MorphVtx {
    f32 x, y, z;    // offset 0x0
    s16 a[3];       // offset 0xC
    s16 b[3];       // offset 0x12
    u8 ia;          // offset 0x18
    u8 ib;          // offset 0x19
    u8 pad1A[2];
};

// Y-morph (wobble) vertex.
struct YMorphVtx {
    f32 x, y, z;  // offset 0x0
    s16 da;       // offset 0xC
    s16 db;       // offset 0xE
    u8 ia;        // offset 0x10
    u8 ib;        // offset 0x11
    u8 pad12[2];
};

struct DirYMorphVtx {
    f32 x, y, z;  // offset 0x0
    f32 w;        // offset 0xC
    s16 da;       // offset 0x10
    s16 db;       // offset 0x12
    u8 ia;        // offset 0x14
    u8 ib;        // offset 0x15
    u8 pad16[2];
};

struct SkinYMorphVtx {
    f32 x, y, z;  // offset 0x0
    u8 b0;        // offset 0xC
    u8 b1;        // offset 0xD
    u8 w0;        // offset 0xE
    u8 w1;        // offset 0xF
    s16 da;       // offset 0x10
    s16 db;       // offset 0x12
    u8 ia;        // offset 0x14
    u8 ib;        // offset 0x15
    u8 pad16[2];
};

struct SkinMorphVtx {
    f32 x, y, z;  // offset 0x0
    u8 b0;        // offset 0xC
    u8 b1;        // offset 0xD
    u8 w0;        // offset 0xE
    u8 w1;        // offset 0xF
    s16 a[3];     // offset 0x10
    s16 b[3];     // offset 0x16
    u8 ia;        // offset 0x1C
    u8 ib;        // offset 0x1D
    u8 pad1E[2];
};

struct DLEntry {
    u32 key;   // offset 0x0
    u8 b;      // offset 0x4
    void* dl;  // offset 0x8
    u32 size;  // offset 0xC
};

struct DeformJob {
    s32 n;      // offset 0x0
    void* src;  // offset 0x4
    void* dst;  // offset 0x8
    s32 pad;    // offset 0xC
};

struct DeformQueue {
    DeformJob jobs[0x400];  // offset 0x0
    s32 count;              // offset 0x4000
};

struct FatMesh {
    s32 n;
    void* p;
};

extern Mat3 lbl_804C5650[1];
extern PlainValMatrix lbl_804C5690;
extern PlainValMatrix lbl_804C56C0;
extern V3 lbl_804C56F0;
extern TexState lbl_804C5700;
extern BVec lbl_804DB068[81];
extern PlainValMatrix lbl_804DAC48[22];
extern u8 lbl_8055FD19;
extern u32 lbl_8055FD1C;
extern u32 lbl_8055FD20;
extern u8 lbl_8055FD25;
extern u32 lbl_8055FD28;

static f32 lbl_804CA308[0x4C];       // morph weights
static DLEntry lbl_804CA438[0x80];   // vertex format display lists
static DeformQueue lbl_804CAC38;     // two-bone
static DeformQueue lbl_804CEC3C;     // bone delta
static DeformQueue lbl_804D2C40;     // y-morph
static DeformQueue lbl_804D6C44;     // bone delta + y-morph

ModuleTag lbl_8055FDD8;
f32 lbl_8055FDE0;   // shadow extrusion
u8 lbl_8055FDE4;    // mirrored
u32 lbl_8055FDE8;   // bound vertex format
u8 lbl_8055FDEC;
s32 lbl_8055FDF0;   // cached formats
u8* lbl_8055FDF4;   // format DL buffer
u8* lbl_8055FDF8;   // format DL cursor
u8 lbl_8055FDFC;
s32 lbl_8055FE00;   // y-morph vertices


template <class Position>
static inline void MulVec(V3* out, const Position* in, const Mat3* m, const void* pos) {
    if (out == pos) {
        f32 x = Dot3(in->x, m->m[0][0], in->y, m->m[1][0], in->z, m->m[2][0]);
        f32 y = Dot3(in->x, m->m[0][1], in->y, m->m[1][1], in->z, m->m[2][1]);
        f32 z = Dot3(in->x, m->m[0][2], in->y, m->m[1][2], in->z, m->m[2][2]);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, m->m[0][0], in->y, m->m[1][0], in->z, m->m[2][0]);
        out->y = Dot3(in->x, m->m[0][1], in->y, m->m[1][1], in->z, m->m[2][1]);
        out->z = Dot3(in->x, m->m[0][2], in->y, m->m[1][2], in->z, m->m[2][2]);
    }
}

static inline void MulVec(V3* out, const V3* in, const Mat3* m) {
    MulVec(out, in, m, in);
}

static inline void MulVecR(V3* out, const V3* in, const Mat3* m) {
    if (out == in) {
        f32 x = Dot3(in->x, m->m[0][0], in->y, m->m[0][1], in->z, m->m[0][2]);
        f32 y = Dot3(in->x, m->m[1][0], in->y, m->m[1][1], in->z, m->m[1][2]);
        f32 z = Dot3(in->x, m->m[2][0], in->y, m->m[2][1], in->z, m->m[2][2]);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, m->m[0][0], in->y, m->m[0][1], in->z, m->m[0][2]);
        out->y = Dot3(in->x, m->m[1][0], in->y, m->m[1][1], in->z, m->m[1][2]);
        out->z = Dot3(in->x, m->m[2][0], in->y, m->m[2][1], in->z, m->m[2][2]);
    }
}

static inline int Queue(DeformQueue* q, int n, void* src, void* dst) {
    if (q->count < 0x400) {
        DeformJob* j = &q->jobs[q->count++];
        j->n = n;
        j->src = src;
        j->dst = dst;
        j->pad = 0;
        return 1;
    }
    return 0;
}

static inline void Skin2(const SkinVtx* v, V3* out, int n) {
    for (int i = n; i > 0; i--) {
        const BVec* a = &lbl_804DB068[v->b0];
        const BVec* b = &lbl_804DB068[v->b1];
        f32 w0 = (1.0f / 255) * v->w0;
        f32 w1 = (1.0f / 255) * v->w1;
        out->x = v->x + a->x * w0;
        out->y = v->y + a->y * w0;
        out->z = v->z + a->z * w0;
        v++;
        out->x = out->x + b->x * w1;
        out->y = out->y + b->y * w1;
        out->z = out->z + b->z * w1;
        out++;
    }
}

extern "C" {

void fn_802D3F34(Surf* s) {
    GXSetArray((GXAttr)9, (const void*)s->verts, (u8)0xC);
    GXSetArray((GXAttr)0xB, (const void*)s->attr, (u8)2);
    GXSetArray((GXAttr)0xD, (const void*)s->uv, (u8)4);
    GXCallDisplayList((void*)s->dl, (u32)s->dlSize);
}

void fn_802D3F98(Surf* s) {
    GXSetArray((GXAttr)9, (const void*)s->verts, (u8)0xC);
    GXSetArray((GXAttr)0xB, (const void*)s->attr, (u8)4);
    GXSetArray((GXAttr)0xD, (const void*)s->uv, (u8)4);
    GXCallDisplayList((void*)s->dl, (u32)s->dlSize);
}

void fn_802D3FFC(Surf* s) {
    GXSetArray((GXAttr)9, (const void*)s->verts, (u8)0xC);
    GXSetArray((GXAttr)0xD, (const void*)s->uv, (u8)4);
    GXSetArray((GXAttr)0xE, (const void*)s->attr, (u8)4);
    GXCallDisplayList((void*)s->dl, (u32)s->dlSize);
}

void fn_802D4060(Surf* s) {
    SkinVtx* v;
    int n = s->nverts;
    v = (SkinVtx*)s->verts;
    V3* out = (V3*)fn_802CFF64(n * 0xC);
    if (out) {
        Skin2(v, out, n);
        GXSetArray((GXAttr)9, (const void*)out, (u8)0xC);
        GXSetArray((GXAttr)0xD, (const void*)s->uv, (u8)4);
        GXSetArray((GXAttr)0xE, (const void*)s->attr, (u8)4);
        GXCallDisplayList((void*)s->dl, (u32)s->dlSize);
    }
}

void fn_802D41C8(Surf* s) {
    GXSetArray((GXAttr)9, (const void*)s->verts, (u8)0xC);
    GXSetArray((GXAttr)0xA, (const void*)s->attr, (u8)3);
    GXSetArray((GXAttr)0xD, (const void*)s->uv, (u8)4);
    GXCallDisplayList((void*)s->dl, (u32)s->dlSize);
}

void fn_802D422C(Surf* s) {
    GXSetArray((GXAttr)9, (const void*)s->verts, (u8)0xC);
    GXSetArray((GXAttr)0xD, (const void*)s->uv, (u8)4);
    GXCallDisplayList((void*)s->dl, (u32)s->dlSize);
}

void fn_802D4280(Surf* s) {
    GXSetArray((GXAttr)9, (const void*)s->verts, (u8)0x10);
    GXSetArray((GXAttr)0xA, (const void*)s->attr, (u8)3);
    GXSetArray((GXAttr)0xD, (const void*)s->uv, (u8)4);
    GXCallDisplayList((void*)s->dl, (u32)s->dlSize);
}

void fn_802D42E4(Surf* s) {
    SkinVtx* v;
    int n = s->nverts;
    v = (SkinVtx*)s->verts;
    V3* out = (V3*)fn_802CFF64(n * 0xC);
    if (!out) {
        fn_802D4280(s);
        return;
    }
    Skin2(v, out, n);
    GXSetArray((GXAttr)9, (const void*)out, (u8)0xC);
    GXSetArray((GXAttr)0xA, (const void*)s->attr, (u8)3);
    GXSetArray((GXAttr)0xD, (const void*)s->uv, (u8)4);
    GXCallDisplayList((void*)s->dl, (u32)s->dlSize);
}

void fn_802D4458(Surf* s) {
    SkinVtx* v;
    int n = s->nverts;
    v = (SkinVtx*)s->verts;
    V3* out = (V3*)fn_802CFF64(n * 0xC);
    if (out) {
        Skin2(v, out, n);
        GXSetArray((GXAttr)9, (const void*)out, (u8)0xC);
        GXSetArray((GXAttr)0xB, (const void*)s->attr, (u8)2);
        GXSetArray((GXAttr)0xD, (const void*)s->uv, (u8)4);
        GXCallDisplayList((void*)s->dl, (u32)s->dlSize);
    }
}

void fn_802D45C0(Surf* s) {
    DirVtx* v;
    int n = s->nverts;
    v = (DirVtx*)s->verts;
    V3* out = (V3*)fn_802CFF64(n * 0xC);
    if (out) {
        if (!Queue(&lbl_804CEC3C, n, v, out)) {
            V3* o = out;
            for (; n > 0; n--) {
                f32 w = v->w;
                o->x = v->x + lbl_804C56F0.x * w;
                o->y = v->y + lbl_804C56F0.y * w;
                o->z = v->z + lbl_804C56F0.z * w;
                v++;
                o++;
            }
        }
        GXSetArray((GXAttr)9, (const void*)out, (u8)0xC);
        GXSetArray((GXAttr)0xB, (const void*)s->attr, (u8)2);
        GXSetArray((GXAttr)0xD, (const void*)s->uv, (u8)4);
        GXCallDisplayList((void*)s->dl, (u32)s->dlSize);
    }
}


void fn_802D479C(Surf* s) {
    MorphVtx* v;
    int n = s->nverts;
    v = (MorphVtx*)s->verts;
    V3* out = (V3*)fn_802CFF64(n * 0xC);
    if (out) {
        MorphVtx* m = v;
        V3* o = out;
        for (int i = n; i > 0; i--) {
            f32 wb = lbl_804CA308[m->ib];
            f32 wa = lbl_804CA308[m->ia];
            V3 a;
            a.Set((1.0f / 255) * m->a[0], (1.0f / 255) * m->a[1], Mul((1.0f / 255), m->a[2]));
            V3 b;
            b.Set((1.0f / 255) * m->b[0], (1.0f / 255) * m->b[1], (1.0f / 255) * m->b[2]);
            m++;
            f32 z = v->z + a.z * wa;
            f32 x = v->x + a.x * wa;
            f32 y = v->y + a.y * wa;
            o->x = x + b.x * wb;
            o->y = y + b.y * wb;
            o->z = z + b.z * wb;
            v++;
            o++;
        }
        GXSetArray((GXAttr)9, (const void*)out, (u8)0xC);
        GXSetArray((GXAttr)0xB, (const void*)s->attr, (u8)2);
        GXSetArray((GXAttr)0xD, (const void*)s->uv, (u8)4);
        GXCallDisplayList((void*)s->dl, (u32)s->dlSize);
    }
}

// PROVISIONAL: actual morph-height input/return API is inferred.
static inline f32 MorphHeight(const f32& y, f32 a, f32 u, f32 b, f32 w) {
    return y + a * u + b * w;
}

void fn_802D4988(Surf* s) {
    YMorphVtx* v = (YMorphVtx*)s->verts;
    PSVECDistance((const Vec*)&lbl_804C56C0.origin, (const Vec*)v);
    int n = s->nverts;
    lbl_8055FE00 += n;
    V3* out = (V3*)fn_802CFF64(n * 0xC);
    if (out) {
        if (!Queue(&lbl_804D2C40, n, v, out)) {
            YMorphVtx* m = v;
            V3* o = out;
            for (; n > 0; n--) {
                f32 vx;
                f32 b;
                f32 wb;
                f32 wa;
                f32 z;
                f32 a;
                wa = lbl_804CA308[m->ia];
                wb = lbl_804CA308[m->ib];
                a = (1.0f / 255) * m->da;
                b = (1.0f / 255) * m->db;
                m++;
                z = v->z;
                vx = v->x;
                f32 height = MorphHeight(v->y, a, wa, b, wb);
                o->x = vx;
                o->y = height;
                o->z = z;
                v++;
                o++;
            }
        }
        GXSetArray((GXAttr)9, (const void*)out, (u8)0xC);
        GXSetArray((GXAttr)0xB, (const void*)s->attr, (u8)2);
        GXSetArray((GXAttr)0xD, (const void*)s->uv, (u8)4);
        GXCallDisplayList((void*)s->dl, (u32)s->dlSize);
    }
}

// Actual per-vertex skinned morph operation.
/* Helper name unproven */
// Actual two-target morph of the skinned base point.
/* Helper name unproven */
// Decode the actual signed16 morph displacement unit.
/* Helper name unproven */
static inline f32 DecodeMorphDelta(s16 raw) { return (1.0f / 255) * raw; }

static inline void ApplySkinMorph(V3* o, const V3& p, const SkinMorphVtx* v) {
    f32 wa = lbl_804CA308[v->ia];
    f32 wb = lbl_804CA308[v->ib];
    V3 a;
    a.Set(DecodeMorphDelta(v->a[0]), DecodeMorphDelta(v->a[1]), DecodeMorphDelta(v->a[2]));
    V3 b;
    b.Set(DecodeMorphDelta(v->b[0]), DecodeMorphDelta(v->b[1]), DecodeMorphDelta(v->b[2]));
    o->x = p.x + a.x * wa;
    o->y = p.y + a.y * wa;
    o->z = p.z + a.z * wa;
    o->x = o->x + b.x * wb;
    o->y = o->y + b.y * wb;
    o->z = o->z + b.z * wb;

}

static inline void SkinMorphVertex(V3* o, const SkinMorphVtx* v) {

    const BVec* ba = &lbl_804DB068[v->b0];
    const BVec* bb = &lbl_804DB068[v->b1];
    f32 w0 = (1.0f / 255) * v->w0;
    f32 w1 = (1.0f / 255) * v->w1;
    V3 p;
    p.x = v->x + ba->x * w0;
    p.y = v->y + ba->y * w0;
    p.z = v->z + ba->z * w0;
    p.x = p.x + bb->x * w1;
    p.y = p.y + bb->y * w1;
    p.z = p.z + bb->z * w1;
    ApplySkinMorph(o, p, v);

}

void fn_802D4C44(Surf* s) {
    SkinMorphVtx* v;
    int n = s->nverts;
    v = (SkinMorphVtx*)s->verts;
    V3* out = (V3*)fn_802CFF64(n * 0xC);
    if (out) {
        V3* o = out;
        for (int i = n; i > 0; i--) {
            SkinMorphVertex(o, v);
            v++;
            o++;
        }
        GXSetArray((GXAttr)9, (const void*)out, (u8)0xC);
        GXSetArray((GXAttr)0xB, (const void*)s->attr, (u8)2);
        GXSetArray((GXAttr)0xD, (const void*)s->uv, (u8)4);
        GXCallDisplayList((void*)s->dl, (u32)s->dlSize);
    }
}

void fn_802D4ECC(Surf* s) {
    SkinYMorphVtx* v;
    int n = s->nverts;
    v = (SkinYMorphVtx*)s->verts;
    V3* out = (V3*)fn_802CFF64(n * 0xC);
    if (out) {
        V3* o = out;
        for (int i = n; i > 0; i--) {
            V3 p;
            f32 w0, w1, wa, b, a, wb;
            const BVec* ba = &lbl_804DB068[v->b0];
            const BVec* bb = &lbl_804DB068[v->b1];
            w0 = (1.0f / 255) * v->w0;
            w1 = (1.0f / 255) * v->w1;
            p.y = v->y + ba->y * w0;
            p.x = v->x + ba->x * w0;
            p.z = v->z + ba->z * w0;
            p.x = p.x + bb->x * w1;
            p.y = p.y + bb->y * w1;
            p.z = p.z + bb->z * w1;
            a = (1.0f / 255) * v->da;
            b = (1.0f / 255) * v->db;
            wa = lbl_804CA308[v->ia];
            wb = lbl_804CA308[v->ib];
            o->x = p.x;
            o->y = AddScaledF(p.y, wa, a) + b * wb;
            o->z = p.z;
            v++;
            o++;
        }
        GXSetArray((GXAttr)9, (const void*)out, (u8)0xC);
        GXSetArray((GXAttr)0xB, (const void*)s->attr, (u8)2);
        GXSetArray((GXAttr)0xD, (const void*)s->uv, (u8)4);
        GXCallDisplayList((void*)s->dl, (u32)s->dlSize);
    }
}

void fn_802D50B4(Surf* s) {
    DirYMorphVtx* v;
    int n = s->nverts;
    v = (DirYMorphVtx*)s->verts;
    V3* out = (V3*)fn_802CFF64(n * 0xC);
    if (out) {
        if (!Queue(&lbl_804D6C44, n, v, out)) {
            V3* o = out;
            for (int i = n; i > 0; i--) {
                f32 a;
                f32 wa;
                f32 wb;
                f32 w;
                f32 b;
                V3 p;
                w = v->w;
                p.x = v->x + lbl_804C56F0.x * w;
                p.y = v->y + lbl_804C56F0.y * w;
                p.z = v->z + lbl_804C56F0.z * w;
                const int ia = v->ia;
                wa = lbl_804CA308[ia];
                const int ib = v->ib;
                wb = lbl_804CA308[ib];
                a = (1.0f / 255) * v->da;
                b = (1.0f / 255) * v->db;
                o->x = p.x;
                o->y = p.y + a * wa + b * wb;
                o->z = p.z;
                v++;
                o++;
            }
        }
        GXSetArray((GXAttr)9, (const void*)out, (u8)0xC);
        GXSetArray((GXAttr)0xB, (const void*)s->attr, (u8)2);
        GXSetArray((GXAttr)0xD, (const void*)s->uv, (u8)4);
        GXCallDisplayList((void*)s->dl, (u32)s->dlSize);
    }
}

void fn_802D5270(Surf* s) {
    int n;
    const SkinVtx* position;  // Position address used by the transform overlap check.
    V3* out;
    const PlainValMatrix* ma;
    V3* o;
    const SkinVtx* v = (const SkinVtx*)s->verts;
    n = s->nverts;
    out = (V3*)fn_802CFF64(0xC * n);
    if (out) {
        position = v;
        o = out;
        int i;
        for (i = n; i > 0; i--) {
            f32 w1;
            f32 w0;
            V3 b, a;
            const PlainValMatrix* mb;
            ma = &lbl_804DAC48[v->b0 >> 2];
            mb = &lbl_804DAC48[v->b1 >> 2];
            w0 = v->w0 * (1.0f / 255);
            w1 = v->w1 * (1.0f / 255);
            MulVec(&a, v, &ma->Mat(), position);
            a.x += ma->origin.x;
            a.y = a.y + ma->origin.y;
            a.z = a.z + ma->origin.z;
            MulVec(&b, v, &mb->Mat(), position);
            b.x = b.x + mb->origin.x;
            b.y = b.y + mb->origin.y;
            b.z += mb->origin.z;
            o->x = a.x * w0;
            o->y = a.y * w0;
            o->z = a.z * w0;
            o->x += b.x * w1;
            o->y = o->y + b.y * w1;
            o->z = o->z + b.z * w1;
            o++;
            v++;
            position++;
        }
        GXSetArray((GXAttr)9, (const void*)out, (u8)0xC);
        GXSetArray((GXAttr)0xA, (const void*)s->attr, (u8)3);
        GXSetArray((GXAttr)0xD, (const void*)s->uv, (u8)4);
        GXCallDisplayList((void*)s->dl, (u32)s->dlSize);
    }
}

void fn_802D5680(Surf* s) {
    GXSetArray((GXAttr)9, (const void*)s->verts, (u8)0x10);
    GXSetArray((GXAttr)0xA, (const void*)s->attr, (u8)3);
    GXCallDisplayList((void*)s->dl, (u32)s->dlSize);
}

void fn_802D56D4(Surf* s) {
    SkinVtx* v;
    int n = s->nverts;
    v = (SkinVtx*)s->verts;
    V3* out = (V3*)fn_802CFF64(n * 0xC);
    if (!out) {
        fn_802D5680(s);
        return;
    }
    Skin2(v, out, n);
    GXSetArray((GXAttr)9, (const void*)out, (u8)0xC);
    GXSetArray((GXAttr)0xA, (const void*)s->attr, (u8)3);
    GXCallDisplayList((void*)s->dl, (u32)s->dlSize);
}

void fn_802D5838(Surf* s) {
    V3* o;
    V3* p = (V3*)s->verts;
    V3* nrm = (V3*)s->attr;
    int n = s->nverts;
    V3* out = (V3*)fn_802CFF64(n * 0xC);
    if (out) {
        V3 l;
        const V3* dir = fn_802D2D04();
        const Mat3* rot = &lbl_804C5690.Mat();
        MulVecR(&l, dir, rot);
        o = out;
        for (int i = 0; i < n; i++) {
            if (PSVECDotProduct((const Vec*)nrm, (const Vec*)&l) < 0.0f) {
                f32 e = lbl_8055FDE0;
                o->x = p->x + l.x * e;
                o->y = p->y + l.y * e;
                o->z = p->z + l.z * e;
            } else {
                o->x = p->x;
                o->y = p->y;
                o->z = p->z;
            }
            nrm++;
            p++;
            o++;
        }
        GXSetArray((GXAttr)9, (const void*)out, (u8)0xC);
        GXSetCullMode((GXCullMode)(lbl_8055FDE4 ? GX_CULL_BACK : GX_CULL_FRONT));
        GXSetBlendMode((GXBlendMode)1, (GXBlendFactor)1, (GXBlendFactor)1, (GXLogicOp)5);
        GXCallDisplayList((void*)s->dl, (u32)s->dlSize);
        GXSetCullMode((GXCullMode)(lbl_8055FDE4 ? GX_CULL_FRONT : GX_CULL_BACK));
        GXSetBlendMode((GXBlendMode)3, (GXBlendFactor)1, (GXBlendFactor)1, (GXLogicOp)5);
        GXCallDisplayList((void*)s->dl, (u32)s->dlSize);
    }
}

void fn_802D5ACC(Surf* s) {
    f32 m[3][4];
    GXSetArray((GXAttr)9, (const void*)s->verts, (u8)0xC);
    GXSetArray((GXAttr)0xB, (const void*)s->attr, (u8)2);
    GXSetArray((GXAttr)0xD, (const void*)s->uv, (u8)4);
    PSMTXTransApply((const f32 (*)[4])(lbl_804C5650), (f32 (*)[4])(m), s->off0, 0.0f, 0.0f);
    GXLoadPosMtxImm((const f32 (*)[4])(m), (u32)3);
    PSMTXTransApply((const f32 (*)[4])(lbl_804C5650), (f32 (*)[4])(m), s->off1, 0.0f, 0.0f);
    GXLoadPosMtxImm((const f32 (*)[4])(m), (u32)6);
    GXCallDisplayList((void*)s->dl, (u32)s->dlSize);
    lbl_8055FD19 = 1;
}


// A vertex colour with its low byte (the fog amount) as a field: the target inserts it with rlwimi.
union FogColor {
    u32 w;
    struct {
        u32 rgb : 24;
        u32 a : 8;
    } f;
};

// Helper name unproven: the body of fn_802D5B8C once its two buffers exist (the target numbers n, col and uv above
// the surface parameter, as arguments of an inline are).
static inline void SurfFog(int n, u32* col, f32* uv, const Surf* s) {
    int i;
    f32* t;
    f32* suv;
    u32* c;
    u32* src;
    V3* p;
    p = (V3*)s->verts;
    src = (u32*)s->attr;
    c = col;
    V3 d;
    // Both frames read as twelve floats (three axis rows, then the position); the offset is rotated by the ROWS.
    const f32* g = (const f32*)lbl_804DB068;
    const f32* a = (const f32*)&lbl_804C56C0;
    const f32* m = (const f32*)&lbl_804C5690;
    d.y = a[10] - m[10];
    f32 dx = a[9] - m[9];
    d.z = a[11] - m[11];
    {
        f32 rx = m[0];
        f32 x = d.z * m[2] + (dx * rx + d.y * m[1]);
        f32 ux = m[3];
        f32 y = Mul(d.z, m[5]) + (dx * ux + d.y * m[4]);
        f32 vx = m[6];
        f32 z = d.z * m[8] + (dx * vx + d.y * m[7]);
        d.x = x;
        d.y = y;
        d.z = z;
    }
    f32 off[2];
    off[0] = g[3];
    off[1] = g[4];
    f32 k = g[5];
    suv = (f32*)s->uv;
    t = uv;
    for (i = 0; i < n;) {
        f32 dist = PSVECSquareDistance((const Vec*)p, (const Vec*)&d);
        u32 v = *src;
        int x = MAX((int)((u8)v * (1.0f - dist * k)), 0);
        int lo = x < 0xFF ? x : 0xFF;
        FogColor o;
        o.w = v & ~0xFF;
        o.f.a = lo;
        *c = o.w;
        p++;
        src++;
        c++;
        i++;
        t[0] = suv[0] + off[0];
        t[1] = suv[1] + off[1];
        suv += 2;
        t += 2;
    }
    GXSetArray((GXAttr)9, (const void*)s->verts, (u8)0xC);
    GXSetArray((GXAttr)0xB, (const void*)col, (u8)4);
    GXSetArray((GXAttr)0xD, (const void*)uv, (u8)8);
    GXCallDisplayList((void*)s->dl, (u32)s->dlSize);
}

void fn_802D5B8C(const Surf* s) {
    int n = s->nverts;
    u32* col = (u32*)fn_802CFF64(n * 4);
    f32* uv = (f32*)fn_802CFF64(n * 8);
    if (!col || !uv) {
        return;
    }
    SurfFog(n, col, uv, s);
}

void fn_802D5DE8(u32 fmt) {
    GXVtxDescList list[27];
    memset(list, 0, sizeof(list));
    GXVtxDescList* p = list;
    if (fmt & 1) {
        p->attr = (GXAttr)0;
        p->type = (GXAttrType)1;
        p++;
    }
    if (fmt & 2) {
        p->attr = (GXAttr)9;
        p->type = (GXAttrType)2;
        p++;
    } else if (fmt & 4) {
        p->attr = (GXAttr)9;
        p->type = (GXAttrType)3;
        p++;
    }
    if (fmt & 8) {
        p->attr = (GXAttr)0xA;
        p->type = (GXAttrType)2;
        p++;
    } else if (fmt & 0x10) {
        p->attr = (GXAttr)0xA;
        p->type = (GXAttrType)3;
        p++;
    }
    if (fmt & 0x20) {
        p->attr = (GXAttr)0xB;
        p->type = (GXAttrType)2;
        p++;
    } else if (fmt & 0x40) {
        p->attr = (GXAttr)0xB;
        p->type = (GXAttrType)3;
        p++;
    }
    if (fmt & 0x80) {
        p->attr = (GXAttr)0xD;
        p->type = (GXAttrType)2;
        p++;
    } else if (fmt & 0x100) {
        p->attr = (GXAttr)0xD;
        p->type = (GXAttrType)3;
        p++;
    }
    if (fmt & 0x200) {
        p->attr = (GXAttr)0xE;
        p->type = (GXAttrType)2;
        p++;
    } else if (fmt & 0x400) {
        p->attr = (GXAttr)0xE;
        p->type = (GXAttrType)3;
        p++;
    }
    p->attr = (GXAttr)0xFF;
    p->type = (GXAttrType)0;
    GXClearVtxDesc();
    GXSetVtxDescv((const GXVtxDescList*)list);
}

void fn_802D5F88(const Surf* s) {
    u32 key = s->vfmt & ~0x300000;
    u32 b = s->b9;
    if (key != lbl_8055FDE8) {
        GXCallDisplayList((void*)s->fmtDL, (u32)(s->dlBlocks << 5));
        lbl_8055FDE8 = key;
        lbl_8055FDEC = b;
    }
}

void fn_802D5FE4(Surf* s) {
    fn_802D5F88(s);
    switch (s->fmt) {
    case 0xA:
        fn_802D41C8(s);
        break;
    case 0xE:
        if (fn_802DAC64()) {
            fn_802D4280(s);
        } else {
            fn_802D42E4(s);
        }
        break;
    case 0x13:
        fn_802D4458(s);
        break;
    case 0x1A:
        fn_802D45C0(s);
        break;
    case 0x1B:
        fn_802D50B4(s);
        break;
    case 0xF:
        fn_802D5270(s);
        break;
    case 0xB:
        fn_802D3F34(s);
        break;
    case 0x1C:
        fn_802D3F98(s);
        break;
    case 0xC:
        fn_802D3FFC(s);
        break;
    case 0xD:
        fn_802D4060(s);
        break;
    case 0x10:
    case 0x12:
        if (fn_802DAC64()) {
            fn_802D5680(s);
        } else {
            fn_802D56D4(s);
        }
        break;
    case 0x11:
        fn_802D5838(s);
        break;
    case 0x14:
        fn_802D5ACC(s);
        break;
    case 0x15:
        fn_802D479C(s);
        break;
    case 0x18:
        fn_802D4988(s);
        break;
    case 0x16:
        fn_802D4C44(s);
        break;
    case 0x19:
        fn_802D4ECC(s);
        break;
    case 0x17:
        fn_802D5B8C(s);
        break;
    case 0x1D:
        fn_802E317C("drgc/surf: Can't draw VF_DXL_NMATRIX_LMSURF conventionally.");
        break;
    case 0x1E:
        fn_802D422C(s);
        break;
    default:
        fn_802E317C("drDrawSurface (DL): unhandled vertex fmt %d", s->fmt);
        break;
    }
}

void fn_802D6170(Surf* s) {
    fn_802D0E48(0);
    fn_802D5FE4(s);
    fn_802D0E48(1);
    GXCallDisplayList((void*)s->dl, (u32)s->dlSize);
}

void fn_802D61BC(Surf* s) {
    lbl_8055FD25 = (s->flags >> 3) & 1;
    lbl_804C5700.env = (s->flags >> 4) & 1;
    lbl_804C5700.mode = 6;
    switch (s->fmt) {
    case 0x10:
        lbl_804C5700.mode = 2;
        break;
    case 0x12:
        lbl_804C5700.mode = 1;
        break;
    default:
        if (s->flags & 1) {
            lbl_804C5700.mode = 3;
        } else if (s->flags & 2) {
            if (lbl_8055FDFC) {
                lbl_804C5700.mode = 5;
            } else {
                lbl_804C5700.mode = 4;
            }
        } else {
            lbl_804C5700.mode = 0;
        }
        break;
    }
    if (s->flags & 4) {
        if (lbl_8055FD28 != s->tex) {
            lbl_8055FD20 |= 0x20;
        }
        lbl_8055FD28 = s->tex;
    }
    fn_802D2614();
    fn_802D2B0C();
}

Surf* fn_802D62E4(Surf* s) {
    if (s->magic == 'NDL2') {
        return s;
    }
    return 0;
}

/* Helper name unproven: accessor for the surface vertex format (a getter; the call-site value goes through it). */
static inline u32 SurfVFmt(const Surf* s) {
    return s->vfmt;
}

void fn_802D62FC(const Surf* s, void** dl, u32* size) {
    u32 key = SurfVFmt(s);
    DLEntry* e = lbl_804CA438;
    const u8 b = s->b9;
    for (int i = lbl_8055FDF0; i > 0; i--) {
        if (e->key == key && e->b == b) {
            *dl = e->dl;
            *size = e->size;
            return;
        }
        e++;
    }
    e = &lbl_804CA438[lbl_8055FDF0++];
    e->key = key;
    e->b = b;
    GXBeginDisplayList((void*)lbl_8055FDF8, (u32)(lbl_8055FDF4 + 0x4000 - lbl_8055FDF8));
    fn_802D5DE8(key);
    u32 n = GXEndDisplayList();
    e->dl = lbl_8055FDF8;
    e->size = n;
    lbl_8055FDF8 += n;
    *dl = e->dl;
    *size = e->size;
}

void fn_802D657C(u32* p, u32 base);

static inline void SwapHeader(u8* const& b) {
    SwapWordRef(b + 0x1C);
    SwapWordRef(b + 0x20);
    SwapWordRef(b + 0x24);
    SwapWordRef(b + 0x28);
    SwapWordRef(b + 0x18);
    SwapWordRef(b + 0xC);
    SwapWordRef(b + 0x10);
}

#ifndef __MWERKS__
// PORT: model files carry their vertex arrays big-endian (GX-ready; the CPU-side header fields are little-endian and
// the Wii swaps those). Skinned, morphing and wind-swayed surfaces are read by the CPU (Skin2, morph and bone-delta
// loops), so a little-endian host converts those surfaces' source vertices to host order at load; GX only reads
// them through the CPU-built output arrays, or directly as positions. nf = floats from offset 0, then ns s16s from
// offset so (the u8 bone indices/weights and morph indices need nothing).
static void PortHostOrderVerts(Surf* s, int stride, int nf, int so, int ns) {
    u8* p = (u8*)s->verts;
    if (!p) {
        return;
    }
    for (int i = 0; i < s->nverts; i++, p += stride) {
        for (int k = 0; k < nf; k++) {
            u8* w = p + k * 4;
            u8 t0 = w[0], t1 = w[1];
            w[0] = w[3];
            w[1] = w[2];
            w[2] = t1;
            w[3] = t0;
        }
        for (int k = 0; k < ns; k++) {
            u8* h = p + so + k * 2;
            u8 t = h[0];
            h[0] = h[1];
            h[1] = t;
        }
    }
}

static void PortHostOrderSurf(Surf* s) {
    switch (s->fmt) {
    case 0xD: case 0xE: case 0xF: case 0x10: case 0x12: case 0x13:
        PortHostOrderVerts(s, sizeof(SkinVtx), 3, 0, 0);
        break;
    case 0x1A:
        PortHostOrderVerts(s, sizeof(DirVtx), 4, 0, 0);
        break;
    case 0x15:
        PortHostOrderVerts(s, sizeof(MorphVtx), 3, 0xC, 6);
        break;
    case 0x18:
        PortHostOrderVerts(s, sizeof(YMorphVtx), 3, 0xC, 2);
        break;
    case 0x16:
        PortHostOrderVerts(s, sizeof(SkinMorphVtx), 3, 0x10, 6);
        break;
    case 0x19:
        PortHostOrderVerts(s, sizeof(SkinYMorphVtx), 3, 0x10, 2);
        break;
    case 0x1B:
        PortHostOrderVerts(s, sizeof(DirYMorphVtx), 4, 0x10, 2);
        break;
    }
}
#endif

void fn_802D63FC(Surf* s, u32 base) {  // one surface: header swap, file offsets to pointers, format DL
    SwapHeader((u8*)s);
    fn_802D657C((u32*)&s->verts, base);
    fn_802D657C((u32*)&s->attr, base);
    fn_802D657C((u32*)&s->uv, base);
#ifndef __MWERKS__
    PortHostOrderSurf(s);  // PORT: CPU-read vertex arrays to host byte order
#endif
    u32 size;
    fn_802D62FC(s, &s->fmtDL, &size);
    size >>= 5;
    s->dlBlocks = size;
}

void fn_802D657C(u32* p, u32 base) {
    if (*p) {
        *p += base;
    }
}

// A surface vertex as far as the facing conversion goes: the word at 0xC holds the colour tag bytes (0x4C/0x4D,
// 0, weight) until it is rewritten as the signed weight. Name unproven.
struct SurfVert {
    f32 pos[3];  // offset 0x0
    f32 w;       // offset 0xC
};

void fn_802D6594(u8* s) {
    u8 a = s[0x34];
    u8* p = s + 0x38;
    u8 b = s[0x35];
    int i;
    u8 c;
    s[0x34] = s[0x37];
    s[0x37] = a;
    s[0x35] = s[0x36];
    s[0x36] = b;
    for (i = 0; i < ((Surf*)s)->nfacing; i++) {
        a = p[0];
        c = p[3];
        p[0] = c;
        p[3] = a;
        a = p[1];
        c = p[2];
        p[1] = c;
        p[2] = a;
        p += 4;
    }
}

// Vertex format 0x1A: colour-facing surfaces (0x13 ones are converted to it below).
enum SurfFmt { SURF_FMT_FACING = 0x1A };

void fn_802D65F8(Surf* s, u32 base, const char* name) {
    fn_802D63FC(s, base);
    SwapWordRef((u8*)s + 0x2C);
    SwapWordRef((u8*)s + 0x30);
    fn_802D657C((u32*)&s->dl, base);
    if ((SurfFmt)s->fmt == SURF_FMT_FACING) {
        fn_802D6594((u8*)s);
    }
    if (s->fmt == 0x13) {
        u8* v = (u8*)s->verts;
        for (int i = s->nverts; i > 0; i--) {
            if (v[0xC] != 0x4C && v[0xC] != 0x4D) {
                return;
            }
            if (v[0xD] != 0) {
                return;
            }
            v += 0x10;
        }
        fn_802E317C("surf: a color facing surf got through!");
        s->fmt = 0x1A;
        v = (u8*)s->verts;
        for (int i = 0; i < s->nverts; i++) {
            f32 w = 10.0f * v[0xE] / 255.0f;
            if (v[0xC] == 0x4C) {
                ((SurfVert*)v)->w = w;
            } else {
                ((SurfVert*)v)->w = -w;
            }
            v += 0x10;
        }
    }
    if (s->fmt == 0x19) {
        u8* v = (u8*)s->verts;
        for (int i = s->nverts; i > 0; i--) {
            if (v[0xC] != 0x4C && v[0xC] != 0x4D) {
                return;
            }
            if (v[0xD] != 0) {
                return;
            }
            v += 0x18;
        }
        s->fmt = 0x1B;
        v = (u8*)s->verts;
        for (int i = 0; i < s->nverts; i++) {
            f32 w = 10.0f * v[0xE] / 255.0f;
            if (v[0xC] == 0x4C) {
                ((SurfVert*)v)->w = w;
            } else {
                ((SurfVert*)v)->w = -w;
            }
            v += 0x18;
        }
    }
}

void fn_802D6C68(void);

void fn_802D6810(void) {
    lbl_8055FDF0 = 0;
    fn_801282EC(0x20);
    lbl_8055FDF8 = lbl_8055FDF4 = new ("vertex_format_dl_buf") u8[0x4000];
    DCInvalidateRange((const void*)lbl_8055FDF4, (u32)0x4000);
    fn_802D6C68();
}

void fn_802D6860(void) {
    fn_802E30C8("dr: vertex format dl took %d bytes for %d formats", lbl_8055FDF8 - lbl_8055FDF4, lbl_8055FDF0);
    delete[] lbl_8055FDF4;
    lbl_8055FDF4 = 0;
    lbl_8055FDF8 = 0;
    fn_802E30C8("total ymorph verts = %d", lbl_8055FE00);
}

// ColPos: valvec3.h's ValVec3.
struct ColVtx {
    ValVec3 pos;  // offset 0x0
    u32 color;    // offset 0xC
    f32 u, v;     // offset 0x10
};

void fn_802D68C4(ColVtx* __restrict verts, int unused, u16* __restrict idx, int n) {
    lbl_8055FD25 = 0;
    lbl_804C5700.env = 0;
    lbl_804C5700.mode = 0;
    fn_802D2614();
    fn_802D2B0C();
    GXClearVtxDesc();
    GXSetVtxDesc((GXAttr)9, (GXAttrType)1);
    GXSetVtxDesc((GXAttr)0xB, (GXAttrType)1);
    GXSetVtxDesc((GXAttr)0xD, (GXAttrType)1);
    GXSetVtxAttrFmt((GXVtxFmt)0, (GXAttr)9, (GXCompCnt)1, (GXCompType)4, (u8)0);
    GXSetVtxAttrFmt((GXVtxFmt)0, (GXAttr)0xB, (GXCompCnt)1, (GXCompType)5, (u8)0);
    GXSetVtxAttrFmt((GXVtxFmt)0, (GXAttr)0xD, (GXCompCnt)1, (GXCompType)4, (u8)0);
    GXBegin((GXPrimitive)0x90, (GXVtxFmt)0, (u16)n);
    for (int i = 0; i < n; i++) {
        ColVtx v;
        f32 x;
        f32 y;
        f32 z;
        v = verts[idx[i]];
        x = v.pos.x;
        y = v.pos.y;
        z = v.pos.z;
        WGPIPE.f = x;
        WGPIPE.f = y;
        WGPIPE.f = z;
        WGPIPE.u = (v.color << 8) | (v.color >> 24);
        WGPIPE.f = v.u;
        WGPIPE.f = v.v;
    }
    fn_802D6C68();
}

void fn_802D6B08(Surf* s) {
    fn_802D61BC(s);
    if (lbl_8055FD1C & 8) {
        fn_802D6170(s);
    } else {
        fn_802D5FE4(s);
    }
}

void fn_802D6B54(Surf* s) { fn_802D5F88(fn_802D62E4(s)); }

void fn_802D6B78(Surf* s) { fn_802D5FE4(s); }

void fn_802D6B7C(Surf* s) { fn_802D6170(s); }

void fn_802D6B80(Surf* s, u32 base, const char* name) {
    SwapWordRef((u8*)s);
    Surf* t = fn_802D62E4(s);
    if (t) {
        fn_802D65F8(t, base, name);
    } else {
        char type[5];
        *(u32*)type = s->magic;
        SwapWordV(type);
        type[4] = 0;
        fn_802E317C("gx: %s has bad surf type %s", name, type);
    }
}

void fn_802D6C44(void*) {}

void fn_802D6C48(void*) {}

void fn_802D6C4C(f32 x) { lbl_8055FDE0 = x; }

void fn_802D6C54(int i, f32 w) { lbl_804CA308[i] = w; }

void fn_802D6C68(void) {
    lbl_8055FDE8 = 0;
    lbl_8055FDEC = 0;
}

static inline void SetGQR6(void) {
#ifdef __MWERKS__
    register u32 v = 0x08070807;
    asm { mtspr 0x396, v }
#endif
}

#define LC_BUF(i) ((u8*)0xE0000000 + ((i) << 13))

// Helper name unproven: the two-bone queue. Each vertex indexes the job's source and destination (the target reloads
// both every time).
static inline void FlushSkin(DeformQueue* q) {
    for (int k = 0; k < q->count; k++) {
        DeformJob* j = &q->jobs[k];
        int n = j->n;
        for (int i = 0; i < n; i++) {
            const SkinVtx* v = &((const SkinVtx*)j->src)[i];
            V3* o = &((V3*)j->dst)[i];
            const BVec* a = &lbl_804DB068[v->b0];
            const BVec* b = &lbl_804DB068[v->b1];
            f32 w0 = (1.0f / 255) * v->w0;
            f32 w1 = (1.0f / 255) * v->w1;
            o->x = v->x + a->x * w0;
            o->y = v->y + a->y * w0;
            o->z = v->z + a->z * w0;
            o->x = o->x + b->x * w1;
            o->y = o->y + b->y * w1;
            o->z = o->z + b->z * w1;
        }
    }
    q->count = 0;
}

// Helper name unproven: the y-morph queue (the target numbers its locals in ascending declaration order, as the
// locals of an inline are).
static inline void FlushYMorph(DeformQueue* q) {
    if (q->count) {
        int left;
        DeformJob* next;
        int n;
        YMorphVtx* src;
        u8* dst;
        int c;
        int buf;
        u8* cur;
        int pc;
        u8* pdst;
        f32* pout;
        u8* in;
        next = q->jobs;
        left = q->count - 1;
        n = next->n;
        src = (YMorphVtx*)next->src;
        dst = (u8*)next->dst;
        next++;
        c = n > 0x100 ? 0x100 : n;
        cur = LC_BUF(0);
        buf = 1;
        LCLoadData(LC_BUF(0), src, c * 0x14);
        n -= c;
        src += c;
        pc = 0;
        pdst = 0;
        pout = 0;
        for (;;) {
            LCQueueWait((u32)0);
            if (pc > 0) {
                LCStoreData((void*)pdst, (const void*)pout, (u32)((pc * 0xC + 0x1F) & ~0x1F));
            }
            pdst = dst;
            pc = c;
            in = cur;
            dst += c * 0xC;
            if (n == 0 && left > 0) {
                n = next->n;
                left--;
                src = (YMorphVtx*)next->src;
                dst = (u8*)next->dst;
                next++;
            }
            c = n > 0x100 ? 0x100 : n;
            if (n > 0) {
                cur = LC_BUF(buf);
                buf = 1 - buf;
                LCLoadData(cur, src, (c * 0x14 + 0x1F) & ~0x1F);
                src += c;
                n -= c;
            }
            if (pc == 0) {
                break;
            }
            pout = (f32*)(LC_BUF(buf) + 0x1400);
            f32* out = pout;
            for (int i = 0; i < pc; i++) {
                SetXYZ((V3*)out, ((f32*)in)[0], ((f32*)in)[1], ((f32*)in)[2]);
                in += 0x14;
                out += 3;
            }
        }
        LCQueueWait((u32)0);
        lbl_804D2C40.count = 0;
    }
}

void fn_802D6C78(void) {
    DeformQueue* q;
    FlushSkin(&lbl_804CAC38);
    SetGQR6();
    SetGQR6();
    FlushYMorph(&lbl_804D2C40);
    SetGQR6();
    q = &lbl_804D6C44;
    if (q->count) {
        DirYMorphVtx* v;
        f32* pout;
        u8* pdst;
        int pc;
        u8* cur;
        int buf;
        int c;
        u8* dst;
        DirYMorphVtx* src;
        int n;
        DeformJob* next;
        int left;
        left = q->count - 1;
        next = q->jobs;
        n = next->n;
        src = (DirYMorphVtx*)next->src;
        dst = (u8*)next->dst;
        next++;
        c = n > 0xE0 ? 0xE0 : n;
        cur = LC_BUF(0);
        buf = 1;
        LCLoadData(LC_BUF(0), src, c * 0x18);
        n -= c;
        src += c;
        pc = 0;
        pdst = 0;
        pout = 0;
        for (;;) {
            LCQueueWait((u32)0);
            if (pc > 0) {
                LCStoreData((void*)pdst, (const void*)pout, (u32)((pc * 0xC + 0x1F) & ~0x1F));
            }
            pdst = dst;
            pc = c;
            v = (DirYMorphVtx*)cur;
            dst += c * 0xC;
            if (n == 0 && left > 0) {
                n = next->n;
                left--;
                src = (DirYMorphVtx*)next->src;
                dst = (u8*)next->dst;
                next++;
            }
            c = n > 0xE0 ? 0xE0 : n;
            if (n > 0) {
                cur = LC_BUF(buf);
                buf = 1 - buf;
                LCLoadData(cur, src, (c * 0x18 + 0x1F) & ~0x1F);
                src += c;
                n -= c;
            }
            if (pc == 0) {
                break;
            }
            pout = (f32*)(LC_BUF(buf) + 0x1500);
            V3* o = (V3*)pout;
            for (int i = 0; i < pc; i++) {
                f32 a = (1.0f / 255) * v->da;
                f32 b = (1.0f / 255) * v->db;
                V3 d = lbl_804C56F0;
                f32 w = v->w;
                V3 p;
                p.y = v->y + d.y * w;
                f32 ta = a * lbl_804CA308[v->ia];
                p.x = v->x + d.x * w;
                p.z = v->z + d.z * w;
                o->Set(p.x, p.y + ta + b * lbl_804CA308[v->ib], p.z);
                v++;
                o++;
            }
        }
        LCQueueWait((u32)0);
        lbl_804D6C44.count = 0;
    }
    SetGQR6();
    q = &lbl_804CEC3C;
    if (q->count) {
        u8* in;
        f32* pout;
        u8* pdst;
        int pc;
        u8* cur;
        int buf;
        int c;
        u8* dst;
        DirVtx* src;
        int n;
        DeformJob* next;
        int left;
        next = q->jobs;
        left = q->count - 1;
        n = next->n;
        src = (DirVtx*)next->src;
        dst = (u8*)next->dst;
        next++;
        c = n > 0x100 ? 0x100 : n;
        cur = LC_BUF(0);
        buf = 1;
        LCLoadData(LC_BUF(0), src, c * 0x10);
        n -= c;
        src += c;
        pc = 0;
        pdst = 0;
        pout = 0;
        for (;;) {
            LCQueueWait((u32)0);
            if (pc > 0) {
                LCStoreData((void*)pdst, (const void*)pout, (u32)((pc * 0xC + 0x1F) & ~0x1F));
            }
            pdst = dst;
            pc = c;
            in = cur;
            dst += c * 0xC;
            if (n == 0 && left > 0) {
                n = next->n;
                left--;
                src = (DirVtx*)next->src;
                dst = (u8*)next->dst;
                next++;
            }
            c = n > 0x100 ? 0x100 : n;
            if (n > 0) {
                cur = LC_BUF(buf);
                buf = 1 - buf;
                LCLoadData(cur, src, (c * 0x10 + 0x1F) & ~0x1F);
                src += c;
                n -= c;
            }
            if (pc == 0) {
                break;
            }
            pout = (f32*)(LC_BUF(buf) + 0x1000);
            f32* out = pout;
            for (int i = 0; i < pc; i++) {
                SetXYZ((V3*)out, ((f32*)in)[0], ((f32*)in)[1], ((f32*)in)[2]);
                in += 0x10;
                out += 3;
            }
        }
        LCQueueWait((u32)0);
        lbl_804CEC3C.count = 0;
    }
}

void fn_802D754C(u8 on) { lbl_8055FDE4 = on; }

void fn_802D7554(u8 on) { lbl_8055FDFC = on; }

}  // extern "C"
