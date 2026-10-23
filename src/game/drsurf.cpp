// Surface drawing (0x802D3F34-0x802D7564, "drgc/surf"): draws NDL2 surfaces through their display lists,
// one path per vertex format - plain, two-bone weighted (bone deltas or skin matrices), morph targets,
// "y-morph" wobble, shadow-volume extrusion and the distance-faded environment layer. Large deformed
// meshes are queued and run through the locked cache in one batch. Also loads (byte-swaps and relocates)
// surfaces and builds and caches the per-format vertex descriptor display lists. Names are guesses apart
// from the strings.
#include "game/common.h"

struct GXVtxDescList {
    int attr;
    int type;
};

struct V3 {
    f32 x;
    f32 y;
    f32 z;
};

struct Mat3 {
    f32 m[3][3];
};

// Bone delta (bonemgr).
struct BVec {
    f32 x;
    f32 y;
    f32 z;
};

// Skin matrix (rotation then translation).
struct SkinMtx {
    Mat3 rot;  // offset 0x0
    V3 pos;    // offset 0x24
};

extern "C" {
void* memset(void* dst, int c, size_t n);
void DCInvalidateRange(void* p, u32 n);
void fn_802E317C(const char* fmt, ...);  // fatal
void fn_802E30C8(const char* fmt, ...);  // warning print
void* fn_802CFF64(u32 size);              // frame allocation
void fn_801282EC(int align);
int fn_802DAC64(void);
void fn_802D0E48(int pass);
void fn_802D2614(void);
void fn_802D2B0C(void);
const V3* fn_802D2D04(void);  // light direction
f32 PSVECDotProduct(const void* a, const void* b);
f32 PSVECDistance(const void* a, const void* b);
f32 PSVECSquareDistance(const void* a, const void* b);
void PSMTXTransApply(const void* src, void* dst, f32 x, f32 y, f32 z);
void GXLoadPosMtxImm(const void* m, u32 id);
void GXSetArray(int attr, const void* base, u8 stride);
void GXCallDisplayList(const void* list, u32 size);
void GXSetCullMode(int mode);
void GXSetBlendMode(int type, int src, int dst, int op);
void GXClearVtxDesc(void);
void GXSetVtxDesc(int attr, int type);
void GXSetVtxDescv(const GXVtxDescList* list);
void GXSetVtxAttrFmt(int fmt, int attr, int cnt, int type, int frac);
void GXBegin(int prim, int fmt, u16 n);
void GXBeginDisplayList(void* list, u32 size);
u32 GXEndDisplayList(void);
void LCLoadData(void* dst, const void* src, u32 n);
void LCStoreData(void* dst, const void* src, u32 n);
void LCQueueWait(u32 n);
}

void* operator new[](size_t size, const char* tag);
void operator delete[](void* p);

union WGPipe {
    u8 uc;
    u32 u;
    f32 f;
};

#define WGPIPE (*(volatile WGPipe*)0xCC008000)

struct Surf {
    u32 magic;     // offset 0x0
    u8 pad4[4];
    u8 fmt;        // offset 0x8
    u8 b9;         // offset 0x9
    u8 flags;      // offset 0xA
    u8 dlBlocks;   // offset 0xB
    u32 vfmt;      // offset 0xC
    void* fmtDL;   // offset 0x10
    u32 tex;       // offset 0x14
    u8 pad18[4];
    s32 nverts;    // offset 0x1C
    void* verts;   // offset 0x20
    void* attr;    // offset 0x24
    void* uv;      // offset 0x28
    u32 dlSize;    // offset 0x2C
    void* dl;      // offset 0x30
    u8 pad34[4];
    f32 off0;      // offset 0x38
    f32 off1;      // offset 0x3C
};

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

struct TexState {
    u8 pad0[4];
    s32 mode;   // offset 0x4
    u8 base2;   // offset 0x8
    u8 b9;      // offset 0x9
    u8 padA[6];
};

struct FatMesh {
    s32 n;
    void* p;
};

extern Mat3 lbl_804C5650[1];
extern SkinMtx lbl_804C5690;
extern SkinMtx lbl_804C56C0;
extern V3 lbl_804C56F0;
extern TexState lbl_804C5700;
extern BVec lbl_804DB068[81];
extern SkinMtx lbl_804DAC48[22];
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

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

static inline void MulVec(V3* out, const V3* in, const Mat3* m) {
    if (out == in) {
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

static inline void Swap32(u8* p) {
    u8 t = p[0];
    p[0] = p[3];
    p[3] = t;
    t = p[1];
    p[1] = p[2];
    p[2] = t;
}

static inline void Skin2(V3* out, const SkinVtx* v, int n) {
    for (int i = n; i > 0; i--) {
        f32 w0 = (1.0f / 255) * v->w0;
        const BVec* a = &lbl_804DB068[v->b0];
        const BVec* b = &lbl_804DB068[v->b1];
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
    GXSetArray(9, s->verts, 0xC);
    GXSetArray(0xB, s->attr, 2);
    GXSetArray(0xD, s->uv, 4);
    GXCallDisplayList(s->dl, s->dlSize);
}

void fn_802D3F98(Surf* s) {
    GXSetArray(9, s->verts, 0xC);
    GXSetArray(0xB, s->attr, 4);
    GXSetArray(0xD, s->uv, 4);
    GXCallDisplayList(s->dl, s->dlSize);
}

void fn_802D3FFC(Surf* s) {
    GXSetArray(9, s->verts, 0xC);
    GXSetArray(0xD, s->uv, 4);
    GXSetArray(0xE, s->attr, 4);
    GXCallDisplayList(s->dl, s->dlSize);
}

void fn_802D4060(Surf* s) {
    SkinVtx* v;
    int n = s->nverts;
    v = (SkinVtx*)s->verts;
    V3* out = (V3*)fn_802CFF64(n * 0xC);
    if (out) {
        Skin2(out, v, n);
        GXSetArray(9, out, 0xC);
        GXSetArray(0xD, s->uv, 4);
        GXSetArray(0xE, s->attr, 4);
        GXCallDisplayList(s->dl, s->dlSize);
    }
}

void fn_802D41C8(Surf* s) {
    GXSetArray(9, s->verts, 0xC);
    GXSetArray(0xA, s->attr, 3);
    GXSetArray(0xD, s->uv, 4);
    GXCallDisplayList(s->dl, s->dlSize);
}

void fn_802D422C(Surf* s) {
    GXSetArray(9, s->verts, 0xC);
    GXSetArray(0xD, s->uv, 4);
    GXCallDisplayList(s->dl, s->dlSize);
}

void fn_802D4280(Surf* s) {
    GXSetArray(9, s->verts, 0x10);
    GXSetArray(0xA, s->attr, 3);
    GXSetArray(0xD, s->uv, 4);
    GXCallDisplayList(s->dl, s->dlSize);
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
    Skin2(out, v, n);
    GXSetArray(9, out, 0xC);
    GXSetArray(0xA, s->attr, 3);
    GXSetArray(0xD, s->uv, 4);
    GXCallDisplayList(s->dl, s->dlSize);
}

void fn_802D4458(Surf* s) {
    SkinVtx* v;
    int n = s->nverts;
    v = (SkinVtx*)s->verts;
    V3* out = (V3*)fn_802CFF64(n * 0xC);
    if (out) {
        Skin2(out, v, n);
        GXSetArray(9, out, 0xC);
        GXSetArray(0xB, s->attr, 2);
        GXSetArray(0xD, s->uv, 4);
        GXCallDisplayList(s->dl, s->dlSize);
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
        GXSetArray(9, out, 0xC);
        GXSetArray(0xB, s->attr, 2);
        GXSetArray(0xD, s->uv, 4);
        GXCallDisplayList(s->dl, s->dlSize);
    }
}

void fn_802D479C(Surf* s) {
    MorphVtx* v;
    int n = s->nverts;
    v = (MorphVtx*)s->verts;
    V3* out = (V3*)fn_802CFF64(n * 0xC);
    if (out) {
        V3* o = out;
        for (int i = n; i > 0; i--) {
            f32 wa = lbl_804CA308[v->ia];
            f32 wb = lbl_804CA308[v->ib];
            V3 a;
            a.x = (1.0f / 255) * v->a[0];
            a.y = (1.0f / 255) * v->a[1];
            a.z = (1.0f / 255) * v->a[2];
            V3 b;
            b.x = (1.0f / 255) * v->b[0];
            b.y = (1.0f / 255) * v->b[1];
            b.z = (1.0f / 255) * v->b[2];
            o->x = v->x + a.x * wa;
            o->y = v->y + a.y * wa;
            o->z = v->z + a.z * wa;
            o->x = o->x + b.x * wb;
            o->y = o->y + b.y * wb;
            o->z = o->z + b.z * wb;
            v++;
            o++;
        }
        GXSetArray(9, out, 0xC);
        GXSetArray(0xB, s->attr, 2);
        GXSetArray(0xD, s->uv, 4);
        GXCallDisplayList(s->dl, s->dlSize);
    }
}

void fn_802D4988(Surf* s) {
    YMorphVtx* v = (YMorphVtx*)s->verts;
    PSVECDistance(&lbl_804C56C0.pos, v);
    int n = s->nverts;
    lbl_8055FE00 += n;
    V3* out = (V3*)fn_802CFF64(n * 0xC);
    if (out) {
        if (!Queue(&lbl_804D2C40, n, v, out)) {
            YMorphVtx* m = v;
            V3* o = out;
            for (; n > 0; n--) {
                f32 a = (1.0f / 255) * m->da;
                f32 b = (1.0f / 255) * m->db;
                f32 wa = lbl_804CA308[m->ia];
                f32 wb = lbl_804CA308[m->ib];
                m++;
                f32 y = v->y;
                f32 z = v->z;
                o->x = v->x;
                o->y = y + a * wa + b * wb;
                o->z = z;
                v++;
                o++;
            }
        }
        GXSetArray(9, out, 0xC);
        GXSetArray(0xB, s->attr, 2);
        GXSetArray(0xD, s->uv, 4);
        GXCallDisplayList(s->dl, s->dlSize);
    }
}

void fn_802D4C44(Surf* s) {
    SkinMorphVtx* v;
    int n = s->nverts;
    v = (SkinMorphVtx*)s->verts;
    V3* out = (V3*)fn_802CFF64(n * 0xC);
    if (out) {
        V3* o = out;
        for (int i = n; i > 0; i--) {
            f32 w0 = (1.0f / 255) * v->w0;
            f32 w1 = (1.0f / 255) * v->w1;
            const BVec* ba = &lbl_804DB068[v->b0];
            const BVec* bb = &lbl_804DB068[v->b1];
            V3 p;
            p.x = v->x + ba->x * w0 + bb->x * w1;
            p.y = v->y + ba->y * w0 + bb->y * w1;
            p.z = v->z + ba->z * w0 + bb->z * w1;
            f32 wa = lbl_804CA308[v->ia];
            f32 wb = lbl_804CA308[v->ib];
            V3 a;
            a.x = (1.0f / 255) * v->a[0];
            a.y = (1.0f / 255) * v->a[1];
            a.z = (1.0f / 255) * v->a[2];
            V3 b;
            b.x = (1.0f / 255) * v->b[0];
            b.y = (1.0f / 255) * v->b[1];
            b.z = (1.0f / 255) * v->b[2];
            o->x = p.x + a.x * wa + b.x * wb;
            o->y = p.y + a.y * wa + b.y * wb;
            o->z = p.z + a.z * wa + b.z * wb;
            v++;
            o++;
        }
        GXSetArray(9, out, 0xC);
        GXSetArray(0xB, s->attr, 2);
        GXSetArray(0xD, s->uv, 4);
        GXCallDisplayList(s->dl, s->dlSize);
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
            f32 w0 = (1.0f / 255) * v->w0;
            f32 w1 = (1.0f / 255) * v->w1;
            const BVec* ba = &lbl_804DB068[v->b0];
            const BVec* bb = &lbl_804DB068[v->b1];
            V3 p;
            p.x = v->x + ba->x * w0;
            p.y = v->y + ba->y * w0;
            p.z = v->z + ba->z * w0;
            p.x = p.x + bb->x * w1;
            p.y = p.y + bb->y * w1;
            p.z = p.z + bb->z * w1;
            o->x = p.x;
            o->y = p.y + (1.0f / 255) * v->da * lbl_804CA308[v->ia] + (1.0f / 255) * v->db * lbl_804CA308[v->ib];
            o->z = p.z;
            v++;
            o++;
        }
        GXSetArray(9, out, 0xC);
        GXSetArray(0xB, s->attr, 2);
        GXSetArray(0xD, s->uv, 4);
        GXCallDisplayList(s->dl, s->dlSize);
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
                f32 w = v->w;
                V3 p;
                p.x = v->x + lbl_804C56F0.x * w;
                p.y = v->y + lbl_804C56F0.y * w;
                p.z = v->z + lbl_804C56F0.z * w;
                o->x = p.x;
                o->y = p.y + (1.0f / 255) * v->da * lbl_804CA308[v->ia] + (1.0f / 255) * v->db * lbl_804CA308[v->ib];
                o->z = p.z;
                v++;
                o++;
            }
        }
        GXSetArray(9, out, 0xC);
        GXSetArray(0xB, s->attr, 2);
        GXSetArray(0xD, s->uv, 4);
        GXCallDisplayList(s->dl, s->dlSize);
    }
}

void fn_802D5270(Surf* s) {
    SkinVtx* v;
    int n = s->nverts;
    v = (SkinVtx*)s->verts;
    V3* out = (V3*)fn_802CFF64(n * 0xC);
    if (out) {
        const V3* p = (const V3*)v;
        V3* o = out;
        for (int i = n; i > 0; i--) {
            V3 a, b;
            f32 w0 = (1.0f / 255) * v->w0;
            f32 w1 = (1.0f / 255) * v->w1;
            const SkinMtx* ma = &lbl_804DAC48[v->b0 >> 2];
            const SkinMtx* mb = &lbl_804DAC48[v->b1 >> 2];
            MulVec(&a, p, &ma->rot);
            a.x += ma->pos.x;
            a.y += ma->pos.y;
            a.z += ma->pos.z;
            MulVec(&b, p, &mb->rot);
            b.x += mb->pos.x;
            b.y += mb->pos.y;
            b.z += mb->pos.z;
            o->x = a.x * w0;
            o->y = a.y * w0;
            o->z = a.z * w0;
            o->x = o->x + b.x * w1;
            o->y = o->y + b.y * w1;
            o->z = o->z + b.z * w1;
            v++;
            p = (const V3*)((u8*)p + 0x10);
            o++;
        }
        GXSetArray(9, out, 0xC);
        GXSetArray(0xA, s->attr, 3);
        GXSetArray(0xD, s->uv, 4);
        GXCallDisplayList(s->dl, s->dlSize);
    }
}

void fn_802D5680(Surf* s) {
    GXSetArray(9, s->verts, 0x10);
    GXSetArray(0xA, s->attr, 3);
    GXCallDisplayList(s->dl, s->dlSize);
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
    Skin2(out, v, n);
    GXSetArray(9, out, 0xC);
    GXSetArray(0xA, s->attr, 3);
    GXCallDisplayList(s->dl, s->dlSize);
}

void fn_802D5838(Surf* s) {
    int n = s->nverts;
    V3* p = (V3*)s->verts;
    V3* nrm = (V3*)s->attr;
    V3* out = (V3*)fn_802CFF64(n * 0xC);
    if (out) {
        V3 l;
        MulVec(&l, fn_802D2D04(), &lbl_804C5690.rot);
        V3* o = out;
        for (int i = 0; i < n; i++) {
            if (PSVECDotProduct(nrm, &l) < 0.0f) {
                o->x = p->x + l.x * lbl_8055FDE0;
                o->y = p->y + l.y * lbl_8055FDE0;
                o->z = p->z + l.z * lbl_8055FDE0;
            } else {
                o->x = p->x;
                o->y = p->y;
                o->z = p->z;
            }
            nrm++;
            p++;
            o++;
        }
        GXSetArray(9, out, 0xC);
        GXSetCullMode(lbl_8055FDE4 ? 2 : 1);
        GXSetBlendMode(1, 1, 1, 5);
        GXCallDisplayList(s->dl, s->dlSize);
        GXSetCullMode(lbl_8055FDE4 ? 1 : 2);
        GXSetBlendMode(3, 1, 1, 5);
        GXCallDisplayList(s->dl, s->dlSize);
    }
}

void fn_802D5ACC(Surf* s) {
    f32 m[3][4];
    GXSetArray(9, s->verts, 0xC);
    GXSetArray(0xB, s->attr, 2);
    GXSetArray(0xD, s->uv, 4);
    PSMTXTransApply(lbl_804C5650, m, s->off0, 0.0f, 0.0f);
    GXLoadPosMtxImm(m, 3);
    PSMTXTransApply(lbl_804C5650, m, s->off1, 0.0f, 0.0f);
    GXLoadPosMtxImm(m, 6);
    GXCallDisplayList(s->dl, s->dlSize);
    lbl_8055FD19 = 1;
}


void fn_802D5B8C(Surf* s) {
    int n = s->nverts;
    u32* col = (u32*)fn_802CFF64(n * 4);
    f32* uv = (f32*)fn_802CFF64(n * 8);
    if (!col || !uv) {
        return;
    }
    V3* p = (V3*)s->verts;
    u32* src = (u32*)s->attr;
    f32* suv = (f32*)s->uv;
    V3 d;
    d.x = lbl_804C56C0.pos.x - lbl_804C5690.pos.x;
    d.y = lbl_804C56C0.pos.y - lbl_804C5690.pos.y;
    d.z = lbl_804C56C0.pos.z - lbl_804C5690.pos.z;
    MulVec(&d, &d, &lbl_804C5690.rot);
    f32 u0 = lbl_804DB068[1].x;
    f32 v0 = lbl_804DB068[1].y;
    f32 k = lbl_804DB068[1].z;
    u32* c = col;
    f32* t = uv;
    for (int i = 0; i < n; i++) {
        f32 f = 1.0f - PSVECSquareDistance(p, &d) * k;
        u32 a = *src;
        int x = MAX((int)((u8)a * f), 0);
        *c = (a & ~0xFF) | (x < 0xFF ? x : 0xFF);
        p++;
        src++;
        c++;
        t[0] = suv[0] + u0;
        t[1] = suv[1] + v0;
        suv += 2;
        t += 2;
    }
    GXSetArray(9, s->verts, 0xC);
    GXSetArray(0xB, col, 4);
    GXSetArray(0xD, uv, 8);
    GXCallDisplayList(s->dl, s->dlSize);
}

void fn_802D5DE8(u32 fmt) {
    GXVtxDescList list[27];
    memset(list, 0, sizeof(list));
    GXVtxDescList* p = list;
    if (fmt & 1) {
        p->attr = 0;
        p->type = 1;
        p++;
    }
    if (fmt & 2) {
        p->attr = 9;
        p->type = 2;
        p++;
    } else if (fmt & 4) {
        p->attr = 9;
        p->type = 3;
        p++;
    }
    if (fmt & 8) {
        p->attr = 0xA;
        p->type = 2;
        p++;
    } else if (fmt & 0x10) {
        p->attr = 0xA;
        p->type = 3;
        p++;
    }
    if (fmt & 0x20) {
        p->attr = 0xB;
        p->type = 2;
        p++;
    } else if (fmt & 0x40) {
        p->attr = 0xB;
        p->type = 3;
        p++;
    }
    if (fmt & 0x80) {
        p->attr = 0xD;
        p->type = 2;
        p++;
    } else if (fmt & 0x100) {
        p->attr = 0xD;
        p->type = 3;
        p++;
    }
    if (fmt & 0x200) {
        p->attr = 0xE;
        p->type = 2;
        p++;
    } else if (fmt & 0x400) {
        p->attr = 0xE;
        p->type = 3;
        p++;
    }
    p->attr = 0xFF;
    p->type = 0;
    GXClearVtxDesc();
    GXSetVtxDescv(list);
}

void fn_802D5F88(const Surf* s) {
    u32 key = s->vfmt & ~0x300000;
    u32 b = s->b9;
    if (key != lbl_8055FDE8) {
        GXCallDisplayList(s->fmtDL, s->dlBlocks << 5);
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
    GXCallDisplayList(s->dl, s->dlSize);
}

void fn_802D61BC(Surf* s) {
    lbl_8055FD25 = (s->flags >> 3) & 1;
    lbl_804C5700.mode = 6;
    lbl_804C5700.b9 = (s->flags >> 4) & 1;
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

void fn_802D62FC(Surf* s, void** dl, u32* size) {
    u32 key = s->vfmt;
    DLEntry* e = lbl_804CA438;
    u8 b = s->b9;
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
    GXBeginDisplayList(lbl_8055FDF8, lbl_8055FDF4 + 0x4000 - lbl_8055FDF8);
    fn_802D5DE8(key);
    u32 n = GXEndDisplayList();
    e->dl = lbl_8055FDF8;
    e->size = n;
    lbl_8055FDF8 += n;
    *dl = e->dl;
    *size = e->size;
}

void fn_802D657C(u32* p, u32 base);

void fn_802D63FC(Surf* s, u32 base) {
    u8* b = (u8*)s;
    Swap32(b + 0x18);
    Swap32(b + 0xC);
    Swap32(b + 0x10);
    Swap32(b + 0x1C);
    Swap32(b + 0x20);
    Swap32(b + 0x24);
    Swap32(b + 0x28);
    fn_802D657C((u32*)&s->verts, base);
    fn_802D657C((u32*)&s->attr, base);
    fn_802D657C((u32*)&s->uv, base);
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

void fn_802D6594(u8* s) {
    Swap32(s + 0x34);
    u8* p = s + 0x38;
    for (int i = 0; i < *(s32*)(s + 0x34); i++) {
        Swap32(p);
        p += 4;
    }
}

void fn_802D65F8(Surf* s, u32 base) {
    fn_802D63FC(s, base);
    Swap32((u8*)s + 0x2C);
    Swap32((u8*)s + 0x30);
    fn_802D657C((u32*)&s->dl, base);
    if (s->fmt == 0x1A) {
        fn_802D6594((u8*)s);
    }
    if (s->fmt == 0x13) {
        u8* v = (u8*)s->verts;
        for (int i = s->nverts; i > 0; i--) {
            if ((v[0xC] != 0x4C && v[0xC] != 0x4D) || v[0xD] != 0) {
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
                *(f32*)(v + 0xC) = w;
            } else {
                *(f32*)(v + 0xC) = -w;
            }
            v += 0x10;
        }
    }
    if (s->fmt == 0x19) {
        u8* v = (u8*)s->verts;
        for (int i = s->nverts; i > 0; i--) {
            if ((v[0xC] != 0x4C && v[0xC] != 0x4D) || v[0xD] != 0) {
                return;
            }
            v += 0x18;
        }
        s->fmt = 0x1B;
        v = (u8*)s->verts;
        for (int i = 0; i < s->nverts; i++) {
            f32 w = 10.0f * v[0xE] / 255.0f;
            if (v[0xC] == 0x4C) {
                *(f32*)(v + 0xC) = w;
            } else {
                *(f32*)(v + 0xC) = -w;
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
    DCInvalidateRange(lbl_8055FDF4, 0x4000);
    fn_802D6C68();
}

void fn_802D6860(void) {
    fn_802E30C8("dr: vertex format dl took %d bytes for %d formats", lbl_8055FDF8 - lbl_8055FDF4, lbl_8055FDF0);
    delete[] lbl_8055FDF4;
    lbl_8055FDF4 = 0;
    lbl_8055FDF8 = 0;
    fn_802E30C8("total ymorph verts = %d", lbl_8055FE00);
}

struct ColVtx {
    f32 x, y, z;  // offset 0x0
    u32 color;    // offset 0xC
    f32 u, v;     // offset 0x10
};

void fn_802D68C4(ColVtx* verts, int unused, const u16* idx, int n) {
    lbl_8055FD25 = 0;
    lbl_804C5700.b9 = 0;
    lbl_804C5700.mode = 0;
    fn_802D2614();
    fn_802D2B0C();
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xB, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    GXBegin(0x90, 0, n);
    for (; n > 0; n--) {
        ColVtx v = verts[*idx++];
        WGPIPE.f = v.x;
        WGPIPE.f = v.y;
        WGPIPE.f = v.z;
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
    Swap32((u8*)s);
    if (fn_802D62E4(s)) {
        fn_802D65F8(s, base);
    } else {
        char type[5];
        *(u32*)type = s->magic;
        Swap32((u8*)type);
        type[4] = 0;
        fn_802E317C("gx: %s has bad surf type %s", name, type);
    }
}

void fn_802D6C44(void) {}

void fn_802D6C48(void) {}

void fn_802D6C4C(f32 x) { lbl_8055FDE0 = x; }

void fn_802D6C54(int i, f32 w) { lbl_804CA308[i] = w; }

void fn_802D6C68(void) {
    lbl_8055FDE8 = 0;
    lbl_8055FDEC = 0;
}

static inline void SetGQR6(void) {
    register u32 v = 0x08070807;
    asm { mtspr 0x396, v }
}

#define LC_BUF(i) ((u8*)0xE0000000 + ((i) << 13))

void fn_802D6C78(void) {
    DeformQueue* q = &lbl_804CAC38;
    for (int k = 0; k < q->count; k++) {
        DeformJob* j = &q->jobs[k];
        Skin2((V3*)j->dst, (const SkinVtx*)j->src, j->n);
    }
    q->count = 0;
    SetGQR6();
    SetGQR6();
    q = &lbl_804D2C40;
    if (q->count) {
        DeformJob* next = q->jobs;
        int left = q->count - 1;
        int n = next->n;
        u8* src = (u8*)next->src;
        u8* dst = (u8*)next->dst;
        next++;
        int c = n > 0x100 ? 0x100 : n;
        LCLoadData(LC_BUF(0), src, c * 0x14);
        n -= c;
        src += c * 0x14;
        int pc = 0;
        u8* pdst = 0;
        f32* pout = 0;
        u8* cur = LC_BUF(0);
        int buf = 1;
        for (;;) {
            LCQueueWait(0);
            if (pc > 0) {
                LCStoreData(pdst, pout, (pc * 0xC + 0x1F) & ~0x1F);
            }
            pdst = dst;
            pc = c;
            u8* in = cur;
            dst += c * 0xC;
            if (n == 0 && left > 0) {
                n = next->n;
                left--;
                src = (u8*)next->src;
                dst = (u8*)next->dst;
                next++;
            }
            c = n > 0x100 ? 0x100 : n;
            if (n > 0) {
                cur = LC_BUF(buf);
                buf = 1 - buf;
                LCLoadData(cur, src, (c * 0x14 + 0x1F) & ~0x1F);
                src += c * 0x14;
                n -= c;
            }
            if (pc == 0) {
                break;
            }
            f32* out = (f32*)(LC_BUF(buf) + 0x1400);
            pout = out;
            for (int i = 0; i < pc; i++) {
                out[0] = ((f32*)in)[0];
                out[1] = ((f32*)in)[1];
                out[2] = ((f32*)in)[2];
                in += 0x14;
                out += 3;
            }
        }
        LCQueueWait(0);
        q->count = 0;
    }
    SetGQR6();
    q = &lbl_804D6C44;
    if (q->count) {
        DeformJob* next = q->jobs;
        int left = q->count - 1;
        int n = next->n;
        u8* src = (u8*)next->src;
        u8* dst = (u8*)next->dst;
        next++;
        int c = n > 0xE0 ? 0xE0 : n;
        LCLoadData(LC_BUF(0), src, c * 0x18);
        n -= c;
        src += c * 0x18;
        int pc = 0;
        u8* pdst = 0;
        f32* pout = 0;
        u8* cur = LC_BUF(0);
        int buf = 1;
        for (;;) {
            LCQueueWait(0);
            if (pc > 0) {
                LCStoreData(pdst, pout, (pc * 0xC + 0x1F) & ~0x1F);
            }
            pdst = dst;
            pc = c;
            DirYMorphVtx* v = (DirYMorphVtx*)cur;
            dst += c * 0xC;
            if (n == 0 && left > 0) {
                n = next->n;
                left--;
                src = (u8*)next->src;
                dst = (u8*)next->dst;
                next++;
            }
            c = n > 0xE0 ? 0xE0 : n;
            if (n > 0) {
                cur = LC_BUF(buf);
                buf = 1 - buf;
                LCLoadData(cur, src, (c * 0x18 + 0x1F) & ~0x1F);
                src += c * 0x18;
                n -= c;
            }
            if (pc == 0) {
                break;
            }
            V3* o = (V3*)(LC_BUF(buf) + 0x1500);
            pout = (f32*)o;
            for (int i = pc; i > 0; i--) {
                V3 d = lbl_804C56F0;
                f32 w = v->w;
                V3 p;
                p.x = v->x + d.x * w;
                p.y = v->y + d.y * w;
                p.z = v->z + d.z * w;
                o->x = p.x;
                o->y = p.y + (1.0f / 255) * v->da * lbl_804CA308[v->ia] + (1.0f / 255) * v->db * lbl_804CA308[v->ib];
                o->z = p.z;
                v++;
                o++;
            }
        }
        LCQueueWait(0);
        q->count = 0;
    }
    SetGQR6();
    q = &lbl_804CEC3C;
    if (q->count) {
        DeformJob* next = q->jobs;
        int left = q->count - 1;
        int n = next->n;
        u8* src = (u8*)next->src;
        u8* dst = (u8*)next->dst;
        next++;
        int c = n > 0x100 ? 0x100 : n;
        LCLoadData(LC_BUF(0), src, c * 0x10);
        n -= c;
        src += c * 0x10;
        int pc = 0;
        u8* pdst = 0;
        f32* pout = 0;
        u8* cur = LC_BUF(0);
        int buf = 1;
        for (;;) {
            LCQueueWait(0);
            if (pc > 0) {
                LCStoreData(pdst, pout, (pc * 0xC + 0x1F) & ~0x1F);
            }
            pdst = dst;
            pc = c;
            u8* in = cur;
            dst += c * 0xC;
            if (n == 0 && left > 0) {
                n = next->n;
                left--;
                src = (u8*)next->src;
                dst = (u8*)next->dst;
                next++;
            }
            c = n > 0x100 ? 0x100 : n;
            if (n > 0) {
                cur = LC_BUF(buf);
                buf = 1 - buf;
                LCLoadData(cur, src, (c * 0x10 + 0x1F) & ~0x1F);
                src += c * 0x10;
                n -= c;
            }
            if (pc == 0) {
                break;
            }
            f32* out = (f32*)(LC_BUF(buf) + 0x1000);
            pout = out;
            for (int i = 0; i < pc; i++) {
                out[0] = ((f32*)in)[0];
                out[1] = ((f32*)in)[1];
                out[2] = ((f32*)in)[2];
                in += 0x10;
                out += 3;
            }
        }
        LCQueueWait(0);
        q->count = 0;
    }
}

void fn_802D754C(u8 on) { lbl_8055FDE4 = on; }

void fn_802D7554(u8 on) { lbl_8055FDFC = on; }

}  // extern "C"
