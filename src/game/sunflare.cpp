// Sun and lens flare (0x80291784-0x802924E8, "flare.val", "burst.tex"): a sun sprite and up to 8 flare
// sprites strung along a rod through the screen centre, shown only on clear days. Names are guesses.
#include "game/common.h"
#include <math.h>

struct VecP {
    void Set(f32 x_, f32 y_, f32 z_) {
        x = x_;
        y = y_;
        z = z_;
    }
    f32 x;
    f32 y;
    f32 z;
};

struct Rot {
    VecP right;  // offset 0x0
    VecP up;     // offset 0xC
    VecP fwd;    // offset 0x18
};

struct Camera {
    Rot rot;   // offset 0x0
    VecP pos;  // offset 0x24
};

struct WeatherDef {
    s32 precip;  // offset 0x0 (0 none, 1 rain, 2 snow)
    u8 night;    // offset 0x4
    int Clear(void) { return night == 0 && precip == 0; }
};

// A camera-facing sprite; pos holds its projected screen position.
struct Sprite {
    VecP pos;    // offset 0x0
    s32 tex;     // offset 0xC
    void* obj;   // offset 0x10
    u8 visible;  // offset 0x14
    u32 color;   // offset 0x18
    f32 radius;  // offset 0x1C
};

struct Flare : Sprite {
    f32 slide;  // offset 0x20
};

struct FlareSet {
    Flare flares[8];  // offset 0x0
    s32 count;        // offset 0x120
    f32 pivotDepth;   // offset 0x124
    f32 rodLength;    // offset 0x128
    f32 maxRadius;    // offset 0x12C
    VecP sunPos;      // offset 0x130
    VecP screen;      // offset 0x13C
    u8 visible;       // offset 0x148
};

static inline f32 Mul(f32 a, f32 b) { return a * b; }

extern "C" {
void PSVECNormalize(const VecP* src, VecP* dst);
f32 fn_80129DC4(int seed, f32 lo, f32 hi);  // random float
ValueTable* fn_80131810(const char* name);  // load value file, quiet
void fn_80131818(ValueTable* t);
ValueNode* fn_802AE414(const char* name);   // track effect table lookup
WeatherDef* fn_801E1F44(void);
f32 fn_801EC114(void);
int fn_802C2C00(int on);
Camera* fn_802C86DC(void);
void fn_802C8C04(int a);
void fn_802C8D74(void);
void fn_802C8E70(const VecP* p, VecP* out);
int fn_802C8FF4(const VecP* p, f32 r);
void fn_802C98DC(f32* fov, f32* znear, f32* zfar, f32* aspect);
void* fn_802CA294(s32 tex);
s32 fn_802CAC28(const char* name, s32 key);
void fn_802CAC54(s32 id);
void* fn_802D39A4(void* tex, f32 scale);
void fn_802D3A50(void* obj);
int fn_802D3A5C(void* obj, const VecP* p);
void fn_802D3BB0(void* obj);
void fn_802D3BB4(void* obj, f32 radius, const VecP* p, const VecP* u, const VecP* v, u32 color);
extern s32 lbl_8055CFF0;  // screen width
}

ModuleTag lbl_8055F750;
UnkStaticFFFF lbl_8055F758;
static u8 lbl_8055F75C;
u8 lbl_8055C920 = 1;
FlareSet lbl_804B3EC0;
Sprite lbl_804B4010;

extern "C" {

f32 fn_80291784(void) {
    f32 fov, znear, zfar, aspect;
    fn_802C98DC(&fov, &znear, &zfar, &aspect);
    return zfar - 1.0f;
}

void fn_802917C0(FlareSet* fs, ValueNode* node) {
    f32 maxR = 0.0f;
    fs->count = 0;
    while (fs->count < 8) {
        ValueNode* c = node->GetChildAt(fs->count);
        if (c == NULL) {
            break;
        }
        f32 radius = fn_80129DC4(0, 8.0f, 15.0f);
        f32 alpha = fn_80129DC4(0, 0.1f, 0.5f);
        f32 slide = fn_80129DC4(0, -0.5f, 0.5f);
        alpha = c->GetFloat("alpha", alpha);
        Flare* f = &fs->flares[fs->count];
        f->pos.x = 0.0f;
        f->pos.y = 0.0f;
        f->pos.z = 0.0f;
        f->tex = fn_802CAC28(c->GetValue("texture"), 0);
        f->obj = NULL;
        f->visible = 0;
        f->color = ((int)(255.0f * alpha) << 24) | 0xFFFFFF;
        f->radius = c->GetFloat("radius", radius);
        f->slide = c->GetFloat("slide", slide);
        maxR = (maxR > f->radius) ? maxR : f->radius;
        fs->count++;
    }
    fs->pivotDepth = node->GetFloat("pivot_depth");
    fs->rodLength = node->GetFloat("rod_length");
    fs->maxRadius = maxR;
}

void fn_802919AC(FlareSet* fs) {
    Flare* f = fs->flares;
    for (int i = 0; i < fs->count; f++, i++) {
        fn_802CAC54(f->tex);
        f->tex = -1;
    }
}

void fn_80291A1C(void) {
    f32 radius = 300.0f;
    ValueNode* n = fn_802AE414("sun");
    if (n != NULL) {
        radius = n->GetFloat("radius", 300.0f);
    }
    Sprite* s = &lbl_804B4010;
    s->pos.x = 0.0f;
    s->pos.y = 0.0f;
    s->pos.z = 0.0f;
    s->tex = fn_802CAC28("burst.tex", 0);
    s->obj = fn_802D39A4(fn_802CA294(s->tex), 0.1f);
    s->visible = 0;
    s->color = 0xFFFFFFFF;
    s->radius = radius;
}

void fn_80291AC8(void) {
    Sprite* s = &lbl_804B4010;
    fn_802CAC54(s->tex);
    s->tex = -1;
    fn_802D3A50(s->obj);
    s->obj = NULL;
}

void fn_80291B14(const VecP* p, f32 depth) {
    fn_802C8E70(p, &lbl_804B4010.pos);
    lbl_804B4010.pos.z = depth;
    u8 vis = 0;
    if (fn_802C8FF4(&lbl_804B4010.pos, lbl_804B4010.radius) && fn_802D3A5C(lbl_804B4010.obj, &lbl_804B4010.pos)) {
        vis = 1;
    }
    lbl_804B4010.visible = vis;
}

void fn_80291B9C(FlareSet* fs, const VecP* p, f32 depth) {
    fs->sunPos = *p;
    fn_802C8E70(p, &fs->screen);
    fs->screen.z = depth;
    if (fn_802C8FF4(&fs->screen, fs->maxRadius)) {
        fs->visible = fs->count > 0;
        Flare* f = fs->flares;
        for (int i = 0; i < fs->count; f++, i++) {
            f->visible = 1;
        }
    } else {
        fs->visible = 0;
        Flare* f = fs->flares;
        for (int i = 0; i < fs->count; f++, i++) {
            f->visible = 0;
        }
    }
}

void fn_80291C7C(Sprite* s, int sun) {
    VecP p = s->pos;
    VecP u;
    VecP v;
    p.z = 0.0f;
    f32 x = (f32)fabs(p.x - 0.5f * lbl_8055CFF0);
    f32 edge = 0.2f * lbl_8055CFF0;
    f32 t = (x - edge) / (0.5f * lbl_8055CFF0 - edge);
    t = (1.0f < t) ? 1.0f : t;
    t = (t > 0.0f) ? t : 0.0f;
    int alpha = Mul(1.0f + Mul(t, -0.75f), s->color >> 24);
    if (alpha > 0) {
        u32 color = (alpha << 24) | (s->color & 0xFFFFFF);
        if (!sun) {
            f32 sn;
            f32 a = fmod((p.x - p.y) / 50.0f, 6.2831855f);
            sn = sin(a);
            f32 cs = cos(a);
            u.Set(cs, sn, 0.0f);
            v.Set(sn, -cs, 0.0f);
        } else {
            u.x = -0.70710677f;
            u.y = 0.70710677f;
            u.z = 0.0f;
            v.x = 0.70710677f;
            v.y = 0.70710677f;
            v.z = 0.0f;
        }
        fn_802D3BB4(lbl_804B4010.obj, s->radius, &p, &u, &v, color);
    }
}

void fn_80291E64(FlareSet* fs) {
    Camera* c = fn_802C86DC();
    VecP pivot;
    VecP dir;
    pivot.x = c->pos.x + c->rot.fwd.x * fs->pivotDepth;
    pivot.y = c->pos.y + c->rot.fwd.y * fs->pivotDepth;
    pivot.z = c->pos.z + c->rot.fwd.z * fs->pivotDepth;
    dir.x = pivot.x - fs->sunPos.x;
    dir.y = pivot.y - fs->sunPos.y;
    dir.z = pivot.z - fs->sunPos.z;
    PSVECNormalize(&dir, &dir);
    f32 back = -(fs->rodLength - fs->pivotDepth);
    VecP start;
    VecP end;
    start.x = pivot.x + dir.x * back;
    start.y = pivot.y + dir.y * back;
    start.z = pivot.z + dir.z * back;
    end.x = start.x + dir.x * fs->rodLength;
    end.y = start.y + dir.y * fs->rodLength;
    end.z = start.z + dir.z * fs->rodLength;
    f32 slideK = fs->screen.x / lbl_8055CFF0;
    f32 step = 1.0f / fs->count;
    Flare* f = fs->flares;
    for (int i = 0; i < fs->count; f++, i++) {
        f32 t = i * step + f->slide * slideK;
        t = (1.0f < t) ? 1.0f : t;
        t = (t > 0.0f) ? t : 0.0f;
        VecP p;
        p.Set(start.x + t * (end.x - start.x), start.y + t * (end.y - start.y), start.z + t * (end.z - start.z));
        fn_802C8E70(&p, &f->pos);
        if (f->visible && fn_802C8FF4(&f->pos, 0.0f)) {
            fn_802C8C04(f->tex);
            fn_80291C7C(f, 0);
            fn_802C8D74();
        }
    }
}

void fn_802920D0(void) {
    Sprite* s = &lbl_804B4010;
    fn_802C8C04(s->tex);
    fn_80291C7C(s, 1);
    fn_802C8D74();
}


void fn_80292114(void) {
    WeatherDef* w = fn_801E1F44();
    u8 on = 0;
    if (w->Clear()) {
        on = 1;
    }
    lbl_8055F75C = on;
    if (lbl_8055F75C) {
        ValueTable* t = fn_80131810("flare.val");
        if (t != NULL) {
            if (t->HasChild("sun")) {
                fn_802917C0(&lbl_804B3EC0, t->FindChild("sun"));
            }
            fn_80131818(t);
        } else {
            lbl_804B3EC0.count = 0;
        }
        fn_80291A1C();
    }
}

void fn_802921F4(void) {
    if (lbl_8055F75C) {
        fn_80291AC8();
        fn_802919AC(&lbl_804B3EC0);
    }
}

void fn_8029222C(void) {
    if (lbl_8055F75C) {
        Flare* f = lbl_804B3EC0.flares;
        for (int i = 0; i < lbl_804B3EC0.count; f++, i++) {
            f->visible = 0;
        }
        lbl_804B4010.visible = 0;
    }
}

void fn_8029227C(void) {
    if (!lbl_8055F75C || !lbl_8055C920) {
        return;
    }
    {
        int old = fn_802C2C00(0);
        if (lbl_804B4010.visible) {
            fn_802D3BB0(lbl_804B4010.obj);
        }
        if (lbl_804B4010.visible) {
            fn_802920D0();
        }
        if (lbl_804B3EC0.visible) {
            fn_80291E64(&lbl_804B3EC0);
        }
        fn_802C2C00(old);
    }
}

void fn_8029231C(const VecP* dir) {
    if (lbl_8055F75C) {
        f32 dist = fn_801EC114();
        Camera* c = fn_802C86DC();
        VecP p;
        p.x = c->pos.x + dir->x * dist;
        p.y = c->pos.y + dir->y * dist;
        p.z = c->pos.z + dir->z * dist;
        f32 depth = fn_80291784();
        fn_80291B14(&p, depth);
        fn_80291B9C(&lbl_804B3EC0, &p, depth);
    }
}

void fn_802923C4(u32 rgb) {
    if (lbl_8055F75C) {
        lbl_804B4010.color = (lbl_804B4010.color & 0xFF000000) | (rgb & 0xFFFFFF);
    }
}

void fn_802923EC(f32 a) {
    if (lbl_8055F75C) {
        a = (1.0f < a) ? 1.0f : a;
        a = (a > 0.0f) ? a : 0.0f;
        lbl_804B4010.color = ((int)(255.0f * a) << 24) | (lbl_804B4010.color & 0xFFFFFF);
        Flare* f = lbl_804B3EC0.flares;
        for (int i = 0; i < lbl_804B3EC0.count; f++, i++) {
            f->color = ((int)(a * (f->color >> 24)) << 24) | (f->color & 0xFFFFFF);
        }
    }
}

}  // extern "C"
