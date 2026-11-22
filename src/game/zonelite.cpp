// Zone lights (0x802A8FEC-0x802A96E8): the lap banner over the start/finish line in lap races
// ("start.mod", "lap2.mod", "lap3.mod", "finish.mod"), placed across the line at ground height and switched
// to the current lap of the viewed car, with a short hold after each lap change. Names are guesses apart
// from the strings.
#include "game/common.h"
#include "game/raceprog.h"
#include "game/raceopts.h"
#include <revolution/MTX/vec.h>

extern "C" {
f32 fn_801EA0E8(f32 x, f32 z);  // ground height
void fn_802A1120(void* pos, void* dir);
int fn_8012C33C(const char* name);
s32 fn_802C6FC0(const char* name, int a, int b);  // load a model
void mrModelUnload(s32 model);
void fn_802C744C(s32 model, const void* frame);
void fn_802C77F0(s32 model, f32* box);
s32 fn_802CAC28(const char* name, int flags);
void gxForgetTexture(s32 tex);
int fn_801E85B4(void);
int fn_801E866C(int a);
f32 fn_801E7E24(void);
}

struct ZV2 {
    f32 x;
    f32 y;
};

// ZFrame: common.h's PlainValMatrix (ZVec: valvec3.h's ValVec3).
struct LapModel {
    LapModel(const char* n) : name(n), model(-1) {}
    ~LapModel(void) {
        if (model != -1) {
            mrModelUnload(model);
            model = -1;
        }
    }
    const char* name;  // offset 0x0
    s32 model;         // offset 0x4
};

struct LapState {
    s32 car;   // offset 0x0
    s32 shown; // offset 0x4
    s32 lap;   // offset 0x8
    f32 hold;  // offset 0xC
};

struct CarLap {
    u8 pad0[0x1C];
    s32 lap;  // offset 0x1C
};

extern "C" {
CarLap* GetCarInfo(int car);
}

ModuleTag lbl_8055F8E0;
UnkStaticFFFF lbl_8055F8E8;
UnkStaticFFFF lbl_8055F8EC;
u8 lbl_8055F8F0;  // on
u8 lbl_8055F8F1;  // free laps
ZV2 lbl_8055F8F8;  // line position
ZV2 lbl_8055F900;  // line normal
ZV2 lbl_8055F908;  // line direction
s32 lbl_8055CC58 = -2;  // light texture

static ValVec3 lbl_804BD6F8;
static ValVec3 lbl_804BD704;
static LapModel lbl_804BD740[4] = {
    LapModel("start.mod"),
    LapModel("lap2.mod"),
    LapModel("lap3.mod"),
    LapModel("finish.mod"),
};
static PlainValMatrix lbl_804BD760;
static LapState lbl_804BD790[2];

static inline void Perp(ZV2* v) {
    f32 t = v->x;
    v->x = v->y;
    v->y = -t;
}

extern "C" {

void ResetLapStates(void);

void fn_802A8FEC(ValVec3* out, const ZV2* p) {
    f32 z = p->y;
    out->y = fn_801EA0E8(p->x, z);
    out->x = p->x;
    out->z = z;
}

bool fn_802A9048(void) {
    u32 f = fn_801E87A8()->type;
    bool r = false;
    if ((f & 0x100) && f != 0x180303 && f != 0x180304) {
        r = true;
    }
    return r;
}

static inline f32 ZMax(const f32& a, const f32& b) { return a > b ? a : b; }

/* Helper name unproven: the start-line frame from the two post points; the boundary is needed (in place -6.6, 50 forms) */
static inline void BuildLineFrame(const ValVec3& vb, PlainValMatrix* f) {
    lbl_804BD704 = vb;
    f->origin = vb;
    f32 yBase = lbl_804BD6F8.y;
    f->origin.x = lbl_804BD6F8.x + f->origin.x;
    f->origin.y = yBase + f->origin.y;
    f->origin.z = lbl_804BD6F8.z + f->origin.z;
    f->origin.x *= 0.5f;
    f->origin.y *= 0.5f;
    f->origin.z *= 0.5f;
    f32 y = ZMax(lbl_804BD6F8.y, vb.y);
    f->origin.y = y;
    f->basis[0].x = lbl_804BD704.x - lbl_804BD6F8.x;
    f->basis[0].y = lbl_804BD704.y - lbl_804BD6F8.y;
    f->basis[0].z = lbl_804BD704.z - lbl_804BD6F8.z;
    f->basis[0].y = 0.0f;
    f->basis[1].x = 0.0f;
    f->basis[1].y = 1.0f;
    f->basis[1].z = 0.0f;
    PSVECCrossProduct((const Vec*)&f->basis[0], (const Vec*)&f->basis[1], (Vec*)&f->basis[2]);
    PSVECNormalize((const Vec*)&f->basis[0], (Vec*)&f->basis[0]);
    PSVECNormalize((const Vec*)&f->basis[2], (Vec*)&f->basis[2]);
    PSVECCrossProduct((const Vec*)&f->basis[2], (const Vec*)&f->basis[0], (Vec*)&f->basis[1]);
    if (fn_801E87A8()->flags & 2) {
        f->basis[0].x *= -1.0f;
        f->basis[0].y *= -1.0f;
        f->basis[0].z *= -1.0f;
    }
}

void fn_802A9090(const ZV2* pos, const ZV2* dir) {
    ZV2 n = *dir;
    Perp(&n);
    f32 half;
    if (lbl_804BD740[0].model != -1) {
        f32 box[6];
        fn_802C77F0(lbl_804BD740[0].model, box);
        half = box[1] - box[0];
    } else {
        half = 20.0f;
    }
    ZV2 a;
    ZV2 b;
    a.x = pos->x + n.x * -half;
    a.y = pos->y + n.y * -half;
    b.x = pos->x + n.x * half;
    b.y = pos->y + n.y * half;
    ValVec3 va;
    fn_802A8FEC(&va, &a);
    lbl_804BD6F8 = va;
    ValVec3 vb;
    fn_802A8FEC(&vb, &b);
    BuildLineFrame(vb, &lbl_804BD760);
}

void fn_802A92CC(void) {
    if ((lbl_8055F8F0 = fn_802A9048()) != 0) {
        fn_802A1120(&lbl_8055F8F8, &lbl_8055F908);
        lbl_8055F900 = lbl_8055F908;
        Perp(&lbl_8055F900);
        LapModel* m = lbl_804BD740;
        for (u32 i = 0; i < 4; i++, m++) {
            if (fn_8012C33C(m->name)) {
                m->model = fn_802C6FC0(m->name, 0, 0);
            }
        }
        fn_802A9090(&lbl_8055F8F8, &lbl_8055F908);
        lbl_8055F8F1 = 0;
        lbl_8055CC58 = fn_802CAC28("zonelite.tex", 0);
        ResetLapStates();
    }
}

void UnloadLapModels(void) {
    if (lbl_8055F8F0) {
        LapModel* m = lbl_804BD740;
        for (u32 i = 0; i < 4; i++, m++) {
            if (m->model != -1) {
                mrModelUnload(m->model);
                m->model = -1;
            }
        }
        gxForgetTexture(lbl_8055CC58);
        lbl_8055CC58 = -2;
    }
}

/* Helper name unproven */
static inline LapState* LapAt(int view) {
    return &lbl_804BD790[view];
}

/* Helper name unproven */
static inline s32 LapShown(int view) {
    return lbl_804BD790[view].shown;
}

void UpdateLapDisplay(void) {
    if (!lbl_8055F8F0) {
        return;
    }
    s32 shown;
    LapState* s;
    int which = -1;
    int view = fn_801E85B4();
    int car = fn_801E866C(-1);
    s32 lap = GetCarInfo(car)->lap;
    s = LapAt(view);
    if (car != s->car || lap < s->lap) {
        lbl_804BD790[view].hold = 0.0f;
        s->car = car;
        lbl_804BD790[view].shown = lap;
        lbl_804BD790[view].lap = lap;
    } else if (lap != s->lap) {
        s->shown = s->lap;
        s->lap = lap;
        s->hold = 3.0f;
    } else if (s->hold < 0.001f) {
        s->shown = s->lap;
    } else {
        s->hold -= fn_801E7E24();
    }
    shown = LapShown(view);
    s32 laps = fn_801E7E3C()->laps;
    if (lbl_8055F8F1 || shown >= laps) {
        which = 3;
    } else if ((u32)(shown + 1) < 4) {
        which = shown;
    }
    if (which >= 0 && lbl_804BD740[which].model != -1) {
        fn_802C744C(lbl_804BD740[which].model, &lbl_804BD760);
    }
}

void ResetLapStates(void) {
    // A counted-down loop over both views (the compiler unrolls it; the stores stay in source order).
    LapState* s = lbl_804BD790;
    int n = 2;
    while (n--) {
        s->lap = -1;
        s->shown = -1;
        s->car = -1;
        s->hold = 0.0f;
        s++;
    }
}

void fn_802A95A8(const ZV2* pos, const ZV2* dir) { fn_802A9090(pos, dir); }

void fn_802A95AC(u8 on) { lbl_8055F8F1 = on; }

}  // extern "C"
