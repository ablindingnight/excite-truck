// Zone lights (0x802A8FEC-0x802A96E8): the lap banner over the start/finish line in lap races
// ("start.mod", "lap2.mod", "lap3.mod", "finish.mod"), placed across the line at ground height and switched
// to the current lap of the viewed car, with a short hold after each lap change. Names are guesses apart
// from the strings.
#include "game/common.h"

extern "C" {
void PSVECNormalize(const void* in, void* out);
void PSVECCrossProduct(const void* a, const void* b, void* out);
f32 fn_801EA0E8(f32 x, f32 z);  // ground height
u32* fn_801E87A8(void);           // race options
void fn_802A1120(void* pos, void* dir);
int fn_8012C33C(const char* name);
s32 fn_802C6FC0(const char* name, int a, int b);  // load a model
void fn_802C6CF8(s32 model);
void fn_802C744C(s32 model, const void* frame);
void fn_802C77F0(s32 model, f32* box);
s32 fn_802CAC28(const char* name, int flags);
void fn_802CAC54(s32 tex);
int fn_801E85B4(void);
int fn_801E866C(int a);
f32 fn_801E7E24(void);
}

struct ZV2 {
    f32 x;
    f32 y;
};

struct ZVec {
    f32 x;
    f32 y;
    f32 z;
};

struct ZFrame {
    ZVec rot[3];  // offset 0x0
    ZVec pos;     // offset 0x24
};

struct LapModel {
    LapModel(const char* n) : name(n), model(-1) {}
    ~LapModel(void) {
        if (model != -1) {
            fn_802C6CF8(model);
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

struct RaceLaps {
    u8 pad0[0x8];
    s32 laps;  // offset 0x8
};

extern "C" {
CarLap* fn_801EA9A0(int car);
RaceLaps* fn_801E7E3C(void);
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

static ZVec lbl_804BD6F8[2];
static LapModel lbl_804BD740[4] = {
    LapModel("start.mod"),
    LapModel("lap2.mod"),
    LapModel("lap3.mod"),
    LapModel("finish.mod"),
};
static ZFrame lbl_804BD760;
static LapState lbl_804BD790[2];

static inline void Perp(ZV2* v) {
    f32 t = v->x;
    v->x = v->y;
    v->y = -t;
}

extern "C" {

void fn_802A9574(void);

void fn_802A8FEC(ZVec* out, const ZV2* p) {
    f32 z = p->y;
    out->y = fn_801EA0E8(p->x, z);
    out->x = p->x;
    out->z = z;
}

bool fn_802A9048(void) {
    u32 f = fn_801E87A8()[1];
    bool r = false;
    if ((f & 0x100) && f != 0x180303 && f != 0x180304) {
        r = true;
    }
    return r;
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
    ZVec va;
    fn_802A8FEC(&va, &a);
    lbl_804BD6F8[0] = va;
    ZVec vb;
    fn_802A8FEC(&vb, &b);
    ZFrame* f = &lbl_804BD760;
    f->pos.x = lbl_804BD6F8[0].x + vb.x;
    f->pos.y = lbl_804BD6F8[0].y + vb.y;
    f->pos.z = lbl_804BD6F8[0].z + vb.z;
    lbl_804BD6F8[1] = vb;
    f->pos.x *= 0.5f;
    f->pos.y *= 0.5f;
    f->pos.z *= 0.5f;
    f32 y = lbl_804BD6F8[0].y;
    if (y > vb.y) {
        y = vb.y;
    }
    f->rot[0].x = lbl_804BD6F8[1].x - lbl_804BD6F8[0].x;
    f->rot[0].y = lbl_804BD6F8[1].y - lbl_804BD6F8[0].y;
    f->rot[0].z = lbl_804BD6F8[1].z - lbl_804BD6F8[0].z;
    f->pos.y = y;
    f->rot[0].y = 0.0f;
    f->rot[1].x = 0.0f;
    f->rot[1].y = 1.0f;
    f->rot[1].z = 0.0f;
    PSVECCrossProduct(&f->rot[0], &f->rot[1], &f->rot[2]);
    PSVECNormalize(&f->rot[0], &f->rot[0]);
    PSVECNormalize(&f->rot[2], &f->rot[2]);
    PSVECCrossProduct(&f->rot[2], &f->rot[0], &f->rot[1]);
    if (fn_801E87A8()[4] & 2) {
        f->rot[0].x *= -1.0f;
        f->rot[0].y *= -1.0f;
        f->rot[0].z *= -1.0f;
    }
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
        fn_802A9574();
    }
}

void fn_802A93A4(void) {
    if (lbl_8055F8F0) {
        LapModel* m = lbl_804BD740;
        for (u32 i = 0; i < 4; i++, m++) {
            if (m->model != -1) {
                fn_802C6CF8(m->model);
                m->model = -1;
            }
        }
        fn_802CAC54(lbl_8055CC58);
        lbl_8055CC58 = -2;
    }
}

void fn_802A9428(void) {
    if (!lbl_8055F8F0) {
        return;
    }
    int which = -1;
    int view = fn_801E85B4();
    int car = fn_801E866C(-1);
    s32 lap = fn_801EA9A0(car)->lap;
    LapState* s = &lbl_804BD790[view];
    if (car == s->car && lap < s->lap) {
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
    s32 shown = lbl_804BD790[view].shown;
    RaceLaps* r = fn_801E7E3C();
    if (!lbl_8055F8F1) {
        if (shown < r->laps) {
            which = shown;
        } else {
            which = 3;
        }
    } else if ((u32)(shown + 1) < 4) {
        which = shown;
    }
    if (which >= 0 && lbl_804BD740[which].model != -1) {
        fn_802C744C(lbl_804BD740[which].model, &lbl_804BD760);
    }
}

void fn_802A9574(void) {
    LapState* s = lbl_804BD790;
    s[0].lap = -1;
    s[0].shown = -1;
    s[0].car = -1;
    s[0].hold = 0.0f;
    s[1].lap = -1;
    s[1].shown = -1;
    s[1].car = -1;
    s[1].hold = 0.0f;
}

void fn_802A95A8(const ZV2* pos, const ZV2* dir) { fn_802A9090(pos, dir); }

void fn_802A95AC(u8 on) { lbl_8055F8F1 = on; }

}  // extern "C"
