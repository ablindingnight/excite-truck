// Menu backgrounds (0x8022F248-0x80230090, "gBack"): textured quads drawn in camera space behind the
// menus, kept in a list. A background can slowly spin and drift with random jitter. The state of the
// last freed background is kept so that one created again with the same texture carries on where it
// left off. Names are guesses apart from the strings.
#include "game/common.h"
#include <revolution/MTX/vec.h>
#include "game/math_types.h"

void* operator new(size_t size, const char* tag);


// BMtx: common.h's PlainValMatrix.
extern "C" {
int strcmp(const char* a, const char* b);
int strcasecmp(const char* a, const char* b);
char* strcpy(char* d, const char* s);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
int fn_8012C33C(const char* name);       // file exists
int fn_802CAC28(const char* name, int a);  // load a texture
void gxForgetTexture(int tex);
int fn_80129D90(int seed, int lo, int hi);  // random int
f32 fn_80214D48(void);                      // frame time
const Mat3* GetTopUIContext(void);              // camera
void fn_801BFD34(ValVec3* out, const ValVec3* v, const Mat3* m);
void fn_802C8B98(int tex, const PlainValMatrix* m);
void fn_802C8DB8(const void* verts, int nv, const u16* idx, int ni);
void fn_802C8D74(void);
u32 fn_80230B9C(int color);
}

struct Background {
    PlainValMatrix mtx;         // offset 0x0
    s32 tex;          // offset 0x30
    u32 color;        // offset 0x34
    s32 tint;         // offset 0x38
    char name[0x40];  // offset 0x3C
    f32 dist;         // offset 0x7C
    s32 texScale;     // offset 0x80
    s32 width;        // offset 0x84
    s32 height;       // offset 0x88
    f32 spin;         // offset 0x8C
    u8 pad90[0x94 - 0x90];
    f32 vx;           // offset 0x94
    f32 vy;           // offset 0x98
    u8 spins;         // offset 0x9C
    u8 drifts;        // offset 0x9D
    f32 driftLimit;   // offset 0xA0: turns the drift back beyond this
    f32 spinJitter;   // offset 0xA4
    f32 spinMax;      // offset 0xA8
    f32 driftJitter;  // offset 0xAC
    f32 driftMax;     // offset 0xB0
};

struct BackNode {
    Background* bg;   // offset 0x0
    s32 refs;         // offset 0x4
    BackNode* next;   // offset 0x8
};

ModuleTag lbl_8055F008;
class GBackStatic20 {
public:
    GBackStatic20(void) {
        unk4 = 123;
        unk0 = 0;
        unk8 = 0;
        unkC = 0;
        unk10 = 0;
    }
    u32 unk0, unk4, unk8, unkC, unk10;
};
static GBackStatic20 lbl_80497E28;
UnkStatic7F lbl_8055F010;
BackNode* lbl_8055F014;     // the backgrounds
Background lbl_80497E3C;    // the state of the last freed background

extern "C" {

void FreeBackground(Background* bg);
int CountBackNodes(void);
void InitializeBackgroundMatrix(Background* bg);

void InitializeBackgroundSystem(void) {
    if (lbl_8055F014) {
        fn_802E317C("gBack system started twice.  Next time just call gBackBegin once.");
    }
    strcpy(lbl_80497E3C.name, "");
}

void DeallocateBackgrounds(void) {
    if (CountBackNodes()) {
        fn_802E30C8("gBack: Some backgrounds still set, deallocating now:");
        BackNode* t = lbl_8055F014;
        BackNode* n = t;
        while (t) {
            FreeBackground(n->bg);
            n = n->next;
            t = 0;
            lbl_8055F014 = n;
        }
    }
}

BackNode* FindBackgroundNodeByName(const char* name) {
    for (BackNode* n = lbl_8055F014; n; n = n->next) {
        if (!strcasecmp(n->bg->name, name)) {
            return n;
        }
    }
    return 0;
}

void InitializeBackgroundTracker(Background* bg) {
    if (bg) {
        BackNode* n = lbl_8055F014;
        if (!n) {
            lbl_8055F014 = (BackNode*)operator new(sizeof(BackNode), "gBack tracker");
            lbl_8055F014->bg = bg;
            lbl_8055F014->refs = 1;
            lbl_8055F014->next = 0;
            return;
        }
        while (n->next) {
            n = n->next;
        }
        BackNode* m = (BackNode*)operator new(sizeof(BackNode), "gBack tracker");
        n->next = m;
        m->bg = bg;
        m->refs = 1;
        m->next = 0;
    }
}

// Remembers bg's state.
void SaveBackgroundState(Background* bg) {
    lbl_80497E3C.mtx = bg->mtx;
    lbl_80497E3C.tex = -1;
    lbl_80497E3C.color = bg->color;
    lbl_80497E3C.tint = bg->tint;
    strcpy(lbl_80497E3C.name, bg->name);
    lbl_80497E3C.dist = bg->dist;
    lbl_80497E3C.texScale = bg->texScale;
    lbl_80497E3C.width = bg->width;
    lbl_80497E3C.height = bg->height;
    lbl_80497E3C.spin = bg->spin;
    *(f32*)lbl_80497E3C.pad90 = *(f32*)bg->pad90;
    lbl_80497E3C.vx = bg->vx;
    lbl_80497E3C.vy = bg->vy;
    lbl_80497E3C.spins = bg->spins;
    lbl_80497E3C.drifts = bg->drifts;
    lbl_80497E3C.driftLimit = bg->driftLimit;
    lbl_80497E3C.spinJitter = bg->spinJitter;
    lbl_80497E3C.spinMax = bg->spinMax;
    lbl_80497E3C.driftJitter = bg->driftJitter;
    lbl_80497E3C.driftMax = bg->driftMax;
}

// Restores the remembered state into bg.
void RestoreBackgroundState(Background* bg) {
    const Background* c = &lbl_80497E3C;
    bg->mtx = c->mtx;
    bg->color = c->color;
    bg->tint = c->tint;
    bg->dist = c->dist;
    bg->texScale = c->texScale;
    bg->width = c->width;
    bg->height = c->height;
    bg->spin = c->spin;
    *(f32*)bg->pad90 = *(f32*)c->pad90;
    bg->vx = c->vx;
    bg->vy = c->vy;
    bg->spins = c->spins;
    bg->drifts = c->drifts;
    bg->driftLimit = c->driftLimit;
    bg->spinJitter = c->spinJitter;
    bg->spinMax = c->spinMax;
    bg->driftJitter = c->driftJitter;
    bg->driftMax = c->driftMax;
}

Background* CreateBackgroundWithTexture(const char* tex, u8 spins, u8 drifts, s32 texScale, f32 dist) {
    if (!tex) {
        return 0;
    }
    FindBackgroundNodeByName(tex);
    Background* bg = (Background*)operator new(sizeof(Background), "gBack");
    if (fn_8012C33C(tex)) {
        bg->tex = fn_802CAC28(tex, 0);
        bg->texScale = texScale;
    } else {
        bg->tex = -1;
        fn_802E30C8("gBack: Texture %s not found for background.", tex);
        bg->texScale = 0;
    }
    strcpy(bg->name, tex);
    if (strcmp(tex, lbl_80497E3C.name)) {
        InitializeBackgroundMatrix(bg);
        bg->drifts = drifts;
        bg->spins = spins;
        bg->driftLimit = 30.0f;
        bg->dist = dist;
        bg->driftJitter = 0.18f;
        bg->driftMax = 4.5f;
        bg->spinJitter = 0.0015f;
        bg->spinMax = 0.035f;
        bg->width = 720;
        bg->height = 480;
    } else {
        RestoreBackgroundState(bg);
    }
    InitializeBackgroundTracker(bg);
    return bg;
}

void RemoveBackgroundNode(Background* bg) {
    BackNode* prev = 0;
    BackNode* n = lbl_8055F014;
    while (n->bg != bg) {
        if (!n) {
            fn_802E317C("Asked to forget background %s that I don't remeber.", bg->name);
        }
        prev = n;
        n = n->next;
    }
    if (!prev) {
        lbl_8055F014 = n->next;
    } else {
        prev->next = n->next;
    }
    n->refs = 0;
    n->next = 0;
    n->bg = 0;
    operator delete(n);
}

void FreeBackground(Background* bg) {
    SaveBackgroundState(bg);
    gxForgetTexture(bg->tex);
    RemoveBackgroundNode(bg);
    operator delete(bg);
}

// m in camera space.
void TransformCameraSpaceMatrix(PlainValMatrix* out, const PlainValMatrix* m) {
    const Mat3* cam = GetTopUIContext();
    fn_801BFD34(&out->origin, &m->origin, cam);
    Mat3Mul(&out->Mat(), &m->Mat(), cam);
}

struct BackXY {
    f32 x;
    f32 y;
} __attribute__((aligned(8)));

struct BackVert {
    BackXY xy;
    u32 a8;
    u32 aC;
    f32 u;
    f32 v;
} __attribute__((aligned(8)));

extern "C" BackVert CreateUIVertex(u32 color, f32 x, f32 y, f32 z, f32 u, f32 v);


void DrawBackgroundLayer(Background* bg) {
    f32 scale = bg->texScale;
    f32 dist = bg->dist;
    u32 color = fn_80230B9C(bg->tint);
    f32 w = bg->width;
    f32 h = bg->height;
    BackVert verts[4] __attribute__((aligned(16)));
    verts[0] = CreateUIVertex(color, -w, h, dist, 0.0f, 0.0f);
    verts[1] = CreateUIVertex(color, w, h, dist, scale, 0.0f);
    verts[2] = CreateUIVertex(color, -w, -h, dist, 0.0f, scale);
    verts[3] = CreateUIVertex(color, w, -h, dist, scale, scale);
    u16 idx[6] = {0, 1, 3, 0, 3, 2};
    PlainValMatrix cm;
    PlainValMatrix m = bg->mtx;
    const PlainValMatrix* in = &m;
    PlainValMatrix* out = &cm;
    TransformCameraSpaceMatrix(out, in);
    fn_802C8B98(bg->tex, &cm);
    fn_802C8DB8(verts, 4, idx, 6);
    fn_802C8D74();
}

static inline f32 Jitter(f32 amount) {
    f32 scale = 10000000.0f;
    int n = amount * scale;
    return fn_80129D90(2, -n, n + 1) / scale;
}

void UpdateBackgroundMotion(Background* bg) {
    if (bg->spins) {
        bg->spin += Jitter(bg->spinJitter);
        if (bg->spin > bg->spinMax) {
            bg->spin = bg->spinMax;
        } else if (bg->spin < -bg->spinMax) {
            bg->spin = -bg->spinMax;
        }
    }
    if (bg->drifts) {
        if (bg->mtx.origin.x > bg->driftLimit) {
            bg->vx -= bg->driftJitter;
        } else if (bg->mtx.origin.x < -bg->driftLimit) {
            bg->vx += bg->driftJitter;
        }
        if (bg->mtx.origin.y > bg->driftLimit) {
            bg->vy -= bg->driftJitter;
        } else if (bg->mtx.origin.y < -bg->driftLimit) {
            bg->vy += bg->driftJitter;
        }
        bg->vx += Jitter(bg->driftJitter);
        if (bg->vx > bg->driftMax) {
            bg->vx = bg->driftMax;
        } else if (bg->vx < -bg->driftMax) {
            bg->vx = -bg->driftMax;
        }
        bg->vy += Jitter(bg->driftJitter);
        if (bg->vy > bg->driftMax) {
            bg->vy = bg->driftMax;
        } else if (bg->vy < -bg->driftMax) {
            bg->vy = -bg->driftMax;
        }
    }
}

void ApplyEulerAngles(Mat3* out, const ValVec3* a);


static inline void Orthonormalize(Mat3* m) {
    ValVec3 x;
    ValVec3 y;
    ValVec3 z;
    x.Set(m->m[0][0], m->m[0][1], m->m[0][2]);
    z.Set(m->m[2][0], m->m[2][1], m->m[2][2]);
    PSVECCrossProduct((const Vec*)&z, (const Vec*)&x, (Vec*)&y);
    PSVECNormalize((const Vec*)&x, (Vec*)&x);
    PSVECNormalize((const Vec*)&y, (Vec*)&y);
    PSVECCrossProduct((const Vec*)&x, (const Vec*)&y, (Vec*)&z);
    *(ValVec3*)m->m[0] = x;
    *(ValVec3*)m->m[1] = y;
    *(ValVec3*)m->m[2] = z;
}

void UpdateBackgroundMatrix(Background* bg, f32 dt) {
    ValVec3 turn;
    turn.x = 0.0f;
    turn.y = 0.0f;
    turn.z = bg->spin * dt;
    if (bg->spins) {
        Mat3 r;
        ApplyEulerAngles(&r, &turn);
        Mat3Mul(&bg->mtx.Mat(), &bg->mtx.Mat(), &r);
    }
    ValVec3 v;
    v.x = bg->vx * dt;
    v.y = bg->vy * dt;
    v.z = 0.0f;
    if (bg->drifts) {
        bg->mtx.origin.x += v.x;
        bg->mtx.origin.y += v.y;
        bg->mtx.origin.z += v.z;
    }
    Orthonormalize(&bg->mtx.Mat());
}

void ApplyEulerAngles(Mat3* out, const ValVec3* a) {
    f32 ry = a->y;
    f32 rx = a->x;
    f32 rz = a->z;
    Mat3 t;
    Mat3RotY(out, ry);
    Mat3RotX(&t, rx);
    Mat3Mul(out, out, &t);
    Mat3RotZ(&t, rz);
    Mat3Mul(out, out, &t);
}

void InitializeBackgroundMatrix(Background* bg) {
    bg->mtx.basis[0].x = 1.0f;
    bg->mtx.basis[0].y = 0.0f;
    bg->mtx.basis[0].z = 0.0f;
    bg->mtx.basis[1].x = 0.0f;
    bg->mtx.basis[1].y = 1.0f;
    bg->mtx.basis[1].z = 0.0f;
    bg->mtx.basis[2].x = 0.0f;
    bg->mtx.basis[2].y = 0.0f;
    bg->mtx.basis[2].z = 1.0f;
    bg->mtx.origin.x = 0.0f;
    bg->mtx.origin.y = 0.0f;
    bg->mtx.origin.z = 0.0f;
    bg->color = 0xFF000000;
    bg->tint = -1;
    bg->spin = 0.0f;
    bg->vx = 0.0f;
    bg->vy = 0.0f;
}

void UpdateBackgroundWidgets(Background* bg) {
    UpdateBackgroundMotion(bg);
    UpdateBackgroundMatrix(bg, fn_80214D48());
}

int CountBackNodes(void) {
    int n = 0;
    for (BackNode* p = lbl_8055F014; p; p = p->next) {
        n++;
    }
    return n;
}

}  // extern "C"
