// Crash recovery (0x801DF6FC-0x801E06E4): after a crash the player shakes the remote to pump a boost bar
// (crashpump/crashbst/crashfail sounds); a full bar relaunches the car with a boost, otherwise it
// times out. Also scores the crash by the damage taken (crash.val "crash_scores") and draws the bar,
// the shake prompt and the white-out flash with crashlyt.val. Names are guesses apart from the
// strings.
#include "game/common.h"

struct V2 {
    f32 x;
    f32 y;
};

struct V3 {
    f32 x;
    f32 y;
    f32 z;
};

struct ElemBase {
    const char* name;  // offset 0x0
    V2 pos;            // offset 0x4
    s32 elemJust;      // offset 0xC
};

struct TextElem : ElemBase {
    f32 scale;         // offset 0x10
    u32 just;          // offset 0x14
    u32 color;         // offset 0x18
    const char* text;  // offset 0x1C
};

struct ArtElem : ElemBase {
    ArtElem(const ArtElem& o);
    f32 w;   // offset 0x10
    f32 h;   // offset 0x14
    f32 u0;  // offset 0x18
    f32 v0;  // offset 0x1C
    f32 u1;  // offset 0x20
    f32 v1;  // offset 0x24
};

struct GartElem : ElemBase {
    f32 w;      // offset 0x10
    f32 h;      // offset 0x14
    u32 color;  // offset 0x18
};

class ILayout {
public:
    virtual ~ILayout(void);
    virtual TextElem* Find(const char* name) = 0;
    virtual ArtElem* FindArt(const char* name) = 0;
    virtual GartElem* FindBox(const char* name) = 0;
    virtual int FindSet(const char* name) = 0;
    virtual void Edit(void) = 0;
};

class Sound2D {
public:
    virtual ~Sound2D(void);
    void Play(void) {
        start = 1;
        loop = 0;
        stop = 0;
    }
    u8 pad4[0x45];
    u8 start;  // offset 0x49
    u8 stop;   // offset 0x4A
    u8 loop;   // offset 0x4B
};

class RaceCarRec {
public:
    f32 F2690(void);
    u8 pad0[0x118];
    V3 vel;     // offset 0x118
    V3 angvel;  // offset 0x124
    u8 pad130[0xC0];
    s32 surface;  // offset 0x1F0
    u8 pad1F4[0x2398];
    f32 boost;    // offset 0x258C
    u8 pad2590[0xD];
    u8 boosting;  // offset 0x259D
    u8 pad259E[0x36];
    f32 f25D4;    // offset 0x25D4
    u8 pad25D8[0x36];
    u8 shaking;   // offset 0x260E
    u8 pad260F[0x81];
    f32 crashed;  // offset 0x2690
    u8 pad2694[0xC];
    f32 crashTime;  // offset 0x26A0
    s32 f26A4;      // offset 0x26A4
};

class GameUI {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void vf18(void);
    virtual void vf1C(void);
    virtual void vf20(void);
    virtual void vf24(void);
    virtual void vf28(void);
    virtual void vf2C(void);
    virtual void vf30(void);
    virtual void vf34(void);
    virtual void vf38(void);
    virtual void vf3C(void);
    virtual void vf40(void);
    virtual void vf44(void);
    virtual void vf48(void);
    virtual void vf4C(void);
    virtual void vf50(void);
    virtual void vf54(void);
    virtual void vf58(void);
    virtual void vf5C(void);
    virtual void vf60(void);
    virtual void vf64(void);
    virtual void vf68(void);
    virtual void vf6C(void);
    virtual void vf70(void);
    virtual void vf74(void);
    virtual void vf78(void);
    virtual void vf7C(void);
    virtual void vf80(void);
    virtual void vf84(void);
    virtual void vf88(void);
    virtual void vf8C(void);
    virtual void vf90(void);
    virtual void vf94(void);
    virtual void vf98(void);
    virtual void vf9C(void);
    virtual void vfA0(void);
    virtual void vfA4(void);
    virtual void vfA8(int car, s32 a);
};

extern "C" {
int strcasecmp(const char* a, const char* b);
double fmod(double x, double y);
f32 PSVECMag(const V3* v);
f32 PSVECDotProduct(const V3* a, const V3* b);
ValueTable* fn_80131808(const char* name);  // load value file
void fn_80131818(ValueTable* t);
const char* fn_80131234(const char* key);  // localize
ILayout* fn_80145E70(const char* name);    // load layout
s32 fn_802CAC28(const char* name, int flags);
void fn_802CAC54(s32 tex);
Sound2D* fn_80207848(const char* name, int flags);
void fn_801476D8(const ArtElem* a, u32 color, s32 tex);
void fn_8014C72C(ILayout* l, const char* name);
void fn_8014C75C(V2* d, const V2* s);
void fn_8014C7B4(ArtElem* d, const ArtElem* s);
f32 fn_8014C8B4(f32 x0, f32 x1, f32 y0, f32 y1, f32 x);  // remap
u32 fn_80144C68(u8 r, u8 g, u8 b, u8 a);
f32 fn_80144BD8(f32 x);  // ease in/out
u32 fn_80147C50(u32 a, u32 b, f32 t);
void fn_801479A8(const TextElem* a, const TextElem* b, const char* fmt, ...);
void fn_80148100(GartElem* e);
void fn_8014E61C(int id, int kind, f32 v);
int fn_80141F7C(f32 x);  // near zero
f32 fn_80176A2C(void);
f32 fn_801E7E18(void);
f32 fn_802C9A50(void);
int fn_80009C84(void);
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
extern f32 lbl_80558DC4[];
}

struct LocalCarDesc {
    s32 event;  // offset 0x0
    u32 flags;  // offset 0x4
};

extern "C" LocalCarDesc* fn_801769CC(void);

f32 Clamp(f32 x, f32 lo, f32 hi);
f32 Max(f32 a, f32 b);
V2 LerpV2(const V2& a, const V2& b, f32 t);

void* operator new(size_t size, const char* tag);

class Updatable {
public:
    Updatable(void) { fn_801D0DDC(this); }
    virtual ~Updatable(void) { fn_801D0DFC(this); }
    virtual void vf0C(void) = 0;
    virtual void vf10(void) = 0;
    virtual void vf14(void) = 0;
    virtual void vf18(void) = 0;
};

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
static inline int NearZero(f32 x) { return (f32)__fabs(x) <= lbl_80558DC4[0]; }
static inline int IsTutorialCrash(void) { return fn_801769CC()->flags == 0x180201; }
static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

inline RaceCarRec* Car(int i);
inline void DrawArt(ArtElem* e, s32 tex);
inline void DrawArt(ILayout* l, const char* name, s32 tex);
inline f32 Fmod(f32 a, f32 b);
inline void DrawArt(ILayout* l, const char* name, u32 color, s32 tex);

extern "C" {

void fn_801DF6FC(void) { new ("crashmode") CrashMode(); }

int fn_801DF93C(int view) {
    ArtElem* up;
    int car;
    CrashMode* m;
    CrashState* s;
    f32 dur;
    f32 left;
    f32 fade;
    f32 fill;
    m = lbl_8055EA04;
    car = fn_801E866C(view);
    if (fn_80141F7C(Car(car)->F2690())) {
        return 0;
    }
    if (fn_801E85A4() > 1) {
        fn_8014C72C(m->layout, "crashed");
        return 1;
    }
    s = &m->cars[car];
    m->layout->Edit();
    fill = Clamp(s->pump, 0.0f, 1.0f);
    dur = lbl_8055AE40 / fn_80176A2C();
    fade = dur / 7.0f;
    left = Max(0.0f, dur - (fn_801E7E18() - s->time));
    if (left > dur - fade || !s->boosted) {
        u32 c0 = fn_80144C68(0xFF, 0x1E, 0x64, 0xF0);
        u32 c1 = fn_80144C68(0xFF, 0xF0, 0x32, 0);
        fn_80147C50(c0, c1, fill);
        DrawArt(m->layout, "bar_left", -1, m->boostTex);
        DrawArt(m->layout, "bar_right", -1, m->boostTex);
        ArtElem* fl = m->layout->FindArt("bar_fill_left");
        ArtElem* fr = m->layout->FindArt("bar_fill_right");
        if (fl && fr) {
            f32 split = fl->w / (fl->w + fr->w);
            f32 a = fn_8014C8B4(0.0f, split, 0.0f, 1.0f, fill);
            f32 b = fn_8014C8B4(split, 1.0f, 0.0f, 1.0f, fill);
            ArtElem e(*fl);
            e.w *= a;
            e.u1 *= a;
            fn_801476D8(&e, -1, m->boostTex);
            fn_8014C7B4(&e, fr);
            e.w *= b;
            e.u1 *= b;
            fn_801476D8(&e, -1, m->boostTex);
        }
        fn_80144C68(255.0f * fill, 0xFF, 0xFF, 0xFF);
        if (!s->boosted) {
            fn_8014C72C(m->layout, "boost_text");
        }
        if (fn_80141F7C(left)) {
            f32 t = Fmod(4.0f * fn_801E7E18() * fn_80176A2C(), 1.0f);
            f32 k;
            if (t < 0.5f) {
                k = fn_8014C8B4(0.0f, 0.5f, 0.0f, 1.0f, t);
            } else {
                k = fn_8014C8B4(0.5f, 1.0f, 1.0f, 0.0f, t);
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
            ArtElem* dn = m->layout->FindArt("arrow_dn");
            if (up && dn) {
                ArtElem e(*up);
                V2 p = LerpV2(up->pos, dn->pos, k);
                fn_8014C75C(&e.pos, &p);
                DrawArt(&e, m->tapTex);
            }
            fn_8014C72C(m->layout, "press_text");
        }
    }
    if (!fn_80141F7C(left) && s->boosted) {
        f32 d = lbl_8055AE40 / fn_80176A2C();
        f32 k = fn_80144BD8(fn_8014C8B4(d - fade, d - d * 0.5f, 0.0f, 1.0f, left));
        TextElem* a = m->layout->Find("boost_text");
        TextElem* b = m->layout->Find("boost_out");
        if (a && b) {
            fn_801479A8(a, b, fn_80131234("Crash:BOOST"), k);
        }
    }
    fn_801769CC();
    return 1;
}

}  // extern "C"

inline void DrawArt(ArtElem* e, s32 tex) { fn_801476D8(e, -1, tex); }

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
    s->damage = 20.0f * (lbl_8055AE30 * PSVECMag(&rc->vel)) / 53.333332f;
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
        if (!((f32)__fabs(left) <= lbl_80558DC4[0])) {
            e = m->layout->FindBox("whiteout");
            f32 half = 0.5f;
            f32 t = Max1(Min1((left - dur * half) / (0.1f - dur * half), 1.0f), 0.0f);
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
    fn_802CAC54(betaTex);
    fn_802CAC54(tapTex);
    fn_802CAC54(zoneTex);
    fn_802CAC54(boostTex);
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

void CrashMode::vf14(void) {
    RaceCarRec* rc;
    int i;
    int done;
    CrashState* s;
    if (fn_801769CC()->flags & 0x4000) {
        return;
    }
    for (i = 0; i < lbl_8055E4D0; i++) {
        rc = lbl_804348E8[i];
        s = &cars[i];
        if ((f32)__fabs(rc->crashed) <= lbl_80558DC4[0]) {
            continue;
        }
        s->elapsed += 0.016683352f;
        if (!s->scored) {
            f32 spin = PSVECMag(&rc->angvel);
            f32 speed = PSVECMag(&rc->vel);
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
        f32 now = lbl_8055E3C8;
        f32 since = now - rc->crashTime;
        f32 window = 1.5f;
        if (rc->f25D4 > 3.5555556f || PSVECDotProduct(&rc->angvel, &rc->angvel) > 0.04f) {
            window *= 1.5f;
        }
        done = 0;
        if (s->time < 0.0f && !s->boosted) {
            if (rc->shaking && since < window && fn_80179650() == 1) {
                s->pump += 0.1f;
                if (pumpSnd[pumpIdx] && rc->surface == 0x50 && !fn_80009C84()) {
                    pumpSnd[pumpIdx]->Play();
                    pumpIdx = (pumpIdx + 1) % 3;
                }
            }
            if (s->pump >= 0.9f) {
                s->pump = 1.0f;
                s->boosted = 1;
                s->time = now;
                if (boostSnd && rc->surface == 0x50 && !fn_80009C84()) {
                    boostSnd->Play();
                }
            } else if (since >= window) {
                s->time = now;
                if (failSnd && rc->surface == 0x50 && !fn_80009C84()) {
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
                    rc->boost = 4.5f;
                    rc->boosting = 0;
                }
            }
        }
        if (done) {
            fn_801B83A0()->vfA8(i, rc->f26A4);
        }
    }
}
