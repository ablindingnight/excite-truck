// Crash recovery (0x801DF6FC-0x801E06E4): after a crash the player shakes the remote to pump a boost bar
// (crashpump/crashbst/crashfail sounds); a full bar relaunches the car with a boost, otherwise it
// times out. Also scores the crash by the damage taken (crash.val "crash_scores") and draws the bar,
// the shake prompt and the white-out flash with crashlyt.val. Names are guesses apart from the
// strings.
#include "game/common.h"
#include "game/gamemode.h"
#include "game/sound.h"
#include "game/ilayout.h"
#include "game/textelem.h"
#include "game/v2.h"
#include "game/raceopts.h"
#include "game/gart.h"
#include <revolution/MTX/vec.h>
#include "game/carbody.h"
#include "game/plainvec.h"

extern "C" {
int strcasecmp(const char* a, const char* b);
double fmod(double x, double y);
ValueTable* fn_80131808(const char* name);  // load value file
void fn_80131818(ValueTable* t);
const char* fn_80131234(const char* key);  // localize
ILayout* fn_80145E70(const char* name);    // load layout
s32 fn_802CAC28(const char* name, int flags);
void gxForgetTexture(s32 tex);
Sound2D* fn_80207848(const char* name, int flags);
void fn_801476D8(const ArtElem* a, u32 color, s32 tex);
u32 fn_80144C68(u8 r, u8 g, u8 b, u8 a);
f32 fn_80144BD8(f32 x);  // ease in/out
u32 fn_80147C50(u32 a, u32 b, f32 t);
void fn_801479A8(const TextElem* a, const TextElem* b, const char* fmt, ...);
void fn_80148100(GartElem* e);
void fn_8014E61C(int id, int kind, f32 v);
f32 fn_80176A2C(void);
f32 fn_801E7E18(void);
f32 fn_802C9A50(void);
int fn_80161BB8(void);
int fn_80179650(void);
int fn_801B8570(int car);
int fn_801769F8(void);
int fn_801E85A4(void);
int fn_801E866C(int view);
GameUI* fn_801B83A0(void);
void fn_801D0DDC(class Updatable* u);  // register
void fn_801D0DFC(class Updatable* u);  // unregister
extern RaceCarRec* lbl_804348E8[8];
extern s32 lbl_8055E4D0;  // number of cars
extern f32 lbl_8055E3C8;  // race time
extern s32 lbl_8055CFF0;  // screen width
extern f32 __float_epsilon[];
}

// Out-of-line copies of shared inline helpers (defined in other units; called, not inlined, by retail).
int fn_80009C84(void);
int NearZero(f32 x);
void DrawText(ILayout* l, const char* name);
void SetV2(V2& d, const V2& s);
f32 Remap(f32 x0, f32 x1, f32 y0, f32 y1, f32 x);


f32 Clamp(f32 x, f32 lo, f32 hi);
f32 Max(f32 a, f32 b);
V2 LerpV2(const V2& a, const V2& b, f32 t);

void* operator new(size_t size, const char* tag);

class Updatable {
public:
    Updatable(void) { fn_801D0DDC(this); }
    virtual ~Updatable(void) = 0;  // pure (retail vtable slot is 0) but defined below
    virtual void vf0C(void) = 0;
    virtual void vf10(void) = 0;
    virtual void vf14(void) = 0;
    virtual void vf18(void) = 0;
};

inline Updatable::~Updatable(void) { fn_801D0DFC(this); }

namespace {
class CrashMode;
}
extern CrashMode* lbl_8055EA04;
extern f32 lbl_8055AE30;
extern f32 lbl_8055AE34;
extern f32 lbl_8055AE38;
extern f32 lbl_8055AE3C;
extern f32 lbl_8055AE40;

struct CrashState {
    f32 pump;      // offset 0x0
    f32 time;      // offset 0x4: when the boost (or the timeout) happened
    u8 boosted;    // offset 0x8
    f32 elapsed;   // offset 0xC
    f32 damage;    // offset 0x10
    u8 scored;     // offset 0x14
    s32 score;     // offset 0x18
};

struct CrashScore {
    f32 thresh;     // offset 0x0
    char name[32];  // offset 0x4
    f32 score;      // offset 0x24
};

static inline void StrCopy(char* d, const char* s, int n) {
    char* p = d;
    while ((*p++ = *s++) != 0) {
        if (--n <= 0) {
            p[-1] = 0;
            break;
        }
    }
}

namespace {
class CrashMode : public Updatable {
public:
    void PumpStep(CrashState* s, RaceCarRec* rc, f32 since, f32 window, f32 now, int& done);
public:
    CrashMode(void) {
        lbl_8055EA04 = this;
        layout = fn_80145E70("crashlyt.val");
        boostTex = fn_802CAC28("boost1.tex", 0);
        zoneTex = fn_802CAC28("zonetex.tex", 0);
        tapTex = fn_802CAC28("tapbut.tex", 0);
        betaTex = fn_802CAC28("betatex2.tex", 0);
        boostSnd = fn_80207848("crashbst.sfx", 1);
        for (int i = 0; i < 3; i++) {
            pumpSnd[i] = fn_80207848("crashpump.sfx", 1);
        }
        pumpIdx = 0;
        failSnd = fn_80207848("crashfail.sfx", 1);
        count = 0;
        ValueTable* t = fn_80131808("crash.val");
        if (t) {
            ValueNode* n = t->FindChild("crash_scores");
            ValueNode* c;
            for (int i = 0; (c = n->GetChildAt(i)) != 0; i++) {
                if (strcasecmp(c->GetName(), "score") == 0) {
                    f32 score = c->GetFloat("score");
                    const char* name = c->GetValue("name");
                    f32 thresh = c->GetFloat("damage_thresh");
                    CrashScore* s = &scores[count++];
                    s->thresh = thresh;
                    StrCopy(s->name, name, 0x20);
                    s->score = score;
                }
            }
            fn_80131818(t);
        }
    }
    virtual ~CrashMode(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void vf18(void);

    CrashState cars[8];      // offset 0x4
    ILayout* layout;         // offset 0xE4
    s32 zoneTex;             // offset 0xE8
    s32 tapTex;              // offset 0xEC
    s32 boostTex;            // offset 0xF0
    s32 betaTex;             // offset 0xF4
    Sound2D* boostSnd;       // offset 0xF8
    Sound2D* pumpSnd[3];     // offset 0xFC
    s32 pumpIdx;             // offset 0x108
    Sound2D* failSnd;        // offset 0x10C
    CrashScore scores[4];    // offset 0x110
    s32 count;               // offset 0x1B0
};
}  // namespace

ModuleTag lbl_8055E9F8;
UnkStatic7F lbl_8055EA00;
CrashMode* lbl_8055EA04;
f32 lbl_8055AE30 = 2.0f;
f32 lbl_8055AE34 = 1.0f;
f32 lbl_8055AE38 = 100.0f;
f32 lbl_8055AE3C = 0.1f;   // minimum time before a crash is scored
f32 lbl_8055AE40 = 1.5f;   // crash recovery window

static inline f32 Min1(f32 a, f32 b) { return (b < a) ? b : a; }
static inline f32 Max2(f32 a, f32 b) {
    f32 r = a;
    if (!(r > b)) {
        r = b;
    }
    return r;
}
static inline f32 Max1(f32 a, f32 b) { return (a > b) ? a : b; }
static inline int IsTutorialCrash(void) { return fn_801769CC()->type == 0x180201; }
static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

inline RaceCarRec* Car(int i);
inline void DrawArt(ArtElem* e, s32 tex);
inline void DrawArt(ILayout* l, const char* name, s32 tex);
inline f32 Fmod(f32 a, f32 b);
inline void DrawArt(ILayout* l, const char* name, u32 color, s32 tex);

extern "C" {

void fn_801DF6FC(void) { new ("crashmode") CrashMode(); }

static inline void DrawFill(CrashMode* m, ArtElem* fl, ArtElem* fr, f32 fill) {
    f32 split = fl->w / (fl->w + fr->w);
    f32 a = Remap(0.0f, split, 0.0f, 1.0f, fill);
    f32 b = Remap(split, 1.0f, 0.0f, 1.0f, fill);
    ArtElem e(*fl);
    e.w *= a;
    e.u1 *= a;
    fn_801476D8(&e, -1, m->boostTex);
    e = *fr;
    e.w *= b;
    e.u1 *= b;
    fn_801476D8(&e, -1, m->boostTex);
}

// MATCHING-AID: inline helper with a parameter assigned before it is read: the value only shapes the register allocation.
static inline void LayoutBoost(TextElem* b, CrashMode* m, TextElem* a, f32 k) {
    b = m->layout->Find("boost_out");
    if (a && b) {
        fn_801479A8(a, b, fn_80131234("Crash:BOOST"), k);
    }
}

// Stripped helper (name unproven): retail pools 3.0f FIRST in .sdata2, before this unit's first float user below.
// A never-called static function is dropped by the compiler but keeps its constant in creation order (FACTS:
// strings/constants of linker-stripped functions, romfont); fn_801DFFB4 and vf14 compute the same window inline.
static f32 PumpWindow(void) { return lbl_8055AE40 / 3.0f; }

int fn_801DF93C(int view) {
    ArtElem* fl;
    ArtElem* fr;
    TextElem* a;
    TextElem* b;
    ArtElem* up;
    ArtElem* dn;
    int car;
    CrashMode* m;
    CrashState* s;
    f32 dur;
    f32 left;
    f32 fade;
    f32 fill;
    m = lbl_8055EA04;
    car = fn_801E866C(view);
    if (NearZero(Car(car)->F2690())) {
        return 0;
    }
    if (fn_801E85A4() > 1) {
        DrawText(m->layout, "crashed");
        return 1;
    }
    s = &m->cars[car];
    m->layout->Edit();
    fill = Clamp(s->pump, 0.0f, 1.0f);
    dur = lbl_8055AE40 / fn_80176A2C();
    fade = dur / 7.0f;
    f32 now = fn_801E7E18();
    left = Max(0.0f, dur - (now - s->time));
    if (left > dur - fade || !s->boosted) {
        u32 c0 = fn_80144C68(0xFF, 0x1E, 0x64, 0xF0);
        u32 c1 = fn_80144C68(0xFF, 0xF0, 0x32, 0);
        fn_80147C50(c0, c1, fill);
        DrawArt(m->layout, "bar_left", -1, m->boostTex);
        DrawArt(m->layout, "bar_right", -1, m->boostTex);
        fl = m->layout->FindArt("bar_fill_left");
        fr = m->layout->FindArt("bar_fill_right");
        if (fl && fr) {
            DrawFill(m, fl, fr, fill);
        }
        fn_80144C68(255.0f * fill, 0xFF, 0xFF, 0xFF);
        if (!s->boosted) {
            DrawText(m->layout, "boost_text");
        }
        if (NearZero(left)) {
            f32 k;
            f32 t = Fmod(4.0f * fn_801E7E18() * fn_80176A2C(), 1.0f);
            if (t < 0.5f) {
                k = Remap(0.0f, 0.5f, 0.0f, 1.0f, t);
            } else {
                k = Remap(0.5f, 1.0f, 1.0f, 0.0f, t);
            }
            u32 col = fn_80144C68(255.0f * k, 0xFF, 0xFF, 0xFF);
            DrawArt(m->layout, "impact_left", col, m->tapTex);
            DrawArt(m->layout, "impact_right", col, m->tapTex);
            if (t > 0.25f && t < 0.75f) {
                DrawArt(m->layout, "button_dn", m->tapTex);
            } else {
                DrawArt(m->layout, "button_up", m->tapTex);
            }
            k = fn_80144BD8(k);
            up = m->layout->FindArt("arrow_up");
            dn = m->layout->FindArt("arrow_dn");
            if (up && dn) {
                ArtElem e(*up);
                V2 p = LerpV2(up->pos, dn->pos, k);
                SetV2(e.pos, p);
                DrawArt(&e, m->tapTex);
            }
            DrawText(m->layout, "press_text");
        }
    }
    if (!NearZero(left) && s->boosted) {
        f32 d = lbl_8055AE40 / fn_80176A2C();
        f32 k = fn_80144BD8(Remap(d - fade, d - d / 2.0f, 0.0f, 1.0f, left));
        LayoutBoost(b, m, m->layout->Find("boost_text"), k);
    }
    fn_801769CC();
    return 1;
}

}  // extern "C"

inline void DrawArt(ArtElem* e, s32 tex) { fn_801476D8(e, -1, tex); }

// ArtElem's copy, after its uses as DrawArt above, so this unit calls the out-of-line weak copy (retail's kept copy
// is betarace's).
#include "game/artelemcopy.h"



inline void DrawArt(ILayout* l, const char* name, s32 tex) { fn_801476D8(l->FindArt(name), -1, tex); }

inline f32 Fmod(f32 a, f32 b) { return fmod(a, b); }

inline void DrawArt(ILayout* l, const char* name, u32 color, s32 tex) { fn_801476D8(l->FindArt(name), color, tex); }

inline RaceCarRec* Car(int i) { return lbl_804348E8[i]; }

extern "C" {

void fn_801DFF1C(int car) {
    CrashState* s = &lbl_8055EA04->cars[car];
    RaceCarRec* rc = lbl_804348E8[car];
    s->pump = 0.15f;
    s->elapsed = 0.0f;
    s->time = -100.0f;
    s->boosted = 0;
    s->damage = 20.0f * (lbl_8055AE30 * PSVECMag((const Vec*)((V3*)&rc->vel))) / 53.333332f;
    s->scored = 0;
    s->score = -1;
}

void fn_801DFFB4(void) {
    int car;
    f32 dur;
    GartElem* e;
    f32 left;
    CrashMode* m;
    m = lbl_8055EA04;
    car = fn_801E866C(0);
    if (fn_80179650() == 1 && fn_801B8570(car) && !fn_801769F8() && !fn_80161BB8()) {
        dur = lbl_8055AE40 / 3.0f;
        f32 x = 0.1f + (dur - (fn_801E7E18() - m->cars[car].time));
        left = 0.0f;
        left = (left > x) ? left : x;
        if (!((f32)__fabs(left) <= __float_epsilon[0])) {
            e = m->layout->FindBox("whiteout");
            f32 t = Max1(Min1((left - dur / 2.0f) / (0.1f - dur / 2.0f), 1.0f), 0.0f);
            e->color = ((int)Lerp(0.0f, 255.0f, t) << 24) | 0xFFFFFF;
            e->w = (f32)lbl_8055CFF0 / fn_802C9A50();
            fn_80148100(e);
        }
    }
}

}  // extern "C"

CrashMode::~CrashMode(void) {
    if (failSnd) {
        delete failSnd;
    }
    for (int i = 0; i < 3; i++) {
        if (pumpSnd[i]) {
            delete pumpSnd[i];
        }
    }
    if (boostSnd) {
        delete boostSnd;
    }
    gxForgetTexture(betaTex);
    gxForgetTexture(tapTex);
    gxForgetTexture(zoneTex);
    gxForgetTexture(boostTex);
    lbl_8055EA04 = 0;
    delete layout;
}

void CrashMode::vf0C(void) {}

void CrashMode::vf18(void) {}

void CrashMode::vf10(void) {
    for (int i = 0; i < lbl_8055E4D0; i++) {
        CrashState* s = &cars[i];
        s->boosted = 0;
        s->time = -100.0f;
        s->elapsed = 0.0f;
        s->damage = 0.0f;
        s->scored = 0;
        s->score = -1;
        s->pump = 0.0f;
    }
}

/* Helper name unproven: pump boost while shaking inside the window; returns done. */
inline void CrashMode::PumpStep(CrashState* s, RaceCarRec* rc, f32 since, f32 window, f32 now, int& done) {
    if (s->time < 0.0f && !s->boosted) {
        if (rc->f260E && since < window && fn_80179650() == 1) {
            s->pump += 0.1f;
            if (pumpSnd[pumpIdx] && rc->f1F0 == 0x50 && !fn_80009C84()) {
                pumpSnd[pumpIdx]->Play();
                pumpIdx = (pumpIdx + 1) % 3;
            }
        }
        if (s->pump >= 0.9f) {
            s->pump = 1.0f;
            s->boosted = 1;
            s->time = now;
            if (boostSnd && rc->f1F0 == 0x50 && !fn_80009C84()) {
                boostSnd->Play();
            }
        } else if (since >= window) {
            s->time = now;
            if (failSnd && rc->f1F0 == 0x50 && !fn_80009C84()) {
                failSnd->Play();
            }
        } else {
            s->pump = Max1(s->pump - 0.0027805585f, 0.15f);
        }
    } else {
        if (!s->boosted) {
            s->pump = Max1(s->pump - 0.011122234f, 0.0f);
        }
        if (now - s->time > lbl_8055AE40 / 3.0f) {
            done = 1;
            if (s->boosted) {
                rc->f258C = 4.5f;
                rc->f259D = 0;
            }
        }
    }
}

void CrashMode::vf14(void) {
    int i;
    RaceCarRec* rc;
    int done;
    CrashState* s;
    if (fn_801769CC()->type & 0x4000) {
        return;
    }
    for (i = 0; i < lbl_8055E4D0; i++) {
        rc = Car(i);
        s = &cars[i];
        if ((f32)__fabs(rc->f2690) <= __float_epsilon[0]) {
            continue;
        }
        s->elapsed += 0.016683352f;
        if (!s->scored) {
            f32 spin = PSVECMag((const Vec*)((V3*)&rc->avel));
            f32 speed = PSVECMag((const Vec*)((V3*)&rc->vel));
            s->damage += 0.016683352f * (lbl_8055AE34 * speed + lbl_8055AE38 * spin);
            if (!IsTutorialCrash() && s->elapsed > lbl_8055AE3C) {
                s->score = -1;
                for (int j = 0; j < count; j++) {
                    if (s->damage > scores[j].thresh) {
                        s->score = j;
                    }
                }
                if (s->score >= 0) {
                    fn_8014E61C(i, 4, scores[s->score].score);
                }
                s->scored = 1;
            }
        }
        f32 speed = rc->speed;
        f32 now = lbl_8055E3C8;
        f32 since = now - rc->f26A0;
        f32 window = 1.5f;
        if (speed > 3.5555556f || PSVECDotProduct((const Vec*)((V3*)&rc->avel), (const Vec*)((V3*)&rc->avel)) > 0.2f * 0.2f) {
            window *= 1.5f;
        }
        done = 0;
        PumpStep(s, rc, since, window, now, done);
        if (done) {
            fn_801B83A0()->vfA8(i, rc->f26A4);
        }
    }
}
