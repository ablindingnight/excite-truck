// Standard race HUD overlays (0x8001DA5C-0x8001EF38, "stdoverlay" in its debug message): score, overheat
// warning, grade, lap title, continue prompt and out-of-time text, drawn from the betalyt/betares/laptitle
// layout sets. Class and function names are guesses.
#include <math.h>
#include "game/common.h"
#include "macros.h"

void* operator new(size_t size, const char* tag);

// One element of a layout set (0x20 bytes); copied, modified and drawn.
struct LayoutItem {
    u32 unk0;   // offset 0x0
    struct {
        u32 a;
        u32 b;
    } unk4;  // offset 0x4
    u32 unkC;   // offset 0xC
    f32 scale;  // offset 0x10
    u32 unk14;  // offset 0x14
    u32 color;  // offset 0x18
    u32 unk1C;  // offset 0x1C
};

class LayoutArt;

// A loaded layout set (.val).
class Layout {
public:
    virtual ~Layout(void);
    virtual LayoutItem* Find(const char* name);    // 0xC
    virtual LayoutArt* FindArt(const char* name);  // 0x10
};

struct Racer {
    u8 pad0[0x1C];
    s32 lap;  // offset 0x1C
};

struct LapInfo {
    u8 pad0[0x8];
    s32 total;  // offset 0x8
};

struct Truck {
    u8 pad0[0x2588];
    f32 heat;  // offset 0x2588
    u8 pad258C[0x25A0 - 0x258C];
    f32 heatRate;  // offset 0x25A0
};

class Sound {
public:
    virtual ~Sound(void);
};


extern "C" {
double sin(double x);
double fabs(double x);
void fn_802E30C8(const char* fmt, ...);  // debug print
int fn_801345D8(char* buf, u32 size, const char* fmt, ...);  // snprintf
const char* fn_80131234(const char* key);  // localize
Layout* fn_80145E70(const char* name);
s32 fn_802CAC28(const char* name, int flags);  // load texture
void fn_802CAC54(s32 tex);
Sound* fn_80207848(const char* name, int flags);
int fn_801E85A4(void);
int fn_80204CF0(void);
void fn_8014784C(Layout* layout, const char* name, const char* set, int index);
void fn_801476D8(LayoutArt* art, u32 color, s32 tex);
int fn_801609C4(int score);
void fn_801476C4(LayoutItem* item);
int fn_80160A90(void);
void fn_801308F4(char* buf, int size, int value);
void fn_801308F8(char* buf, int size, int value);
void fn_801473F8(Layout* layout, const char* name, const char* fmt, ...);
void fn_80147230(LayoutItem* item, const char* fmt, ...);
Truck* fn_8017960C(int player);
void fn_80147CEC(LayoutItem* out, LayoutItem* a, LayoutItem* b, f32 t);
u32 fn_80147C50(u32 a, u32 b, f32 t);
void fn_80232928(u32 color);
void fn_80232930(void);
Racer* fn_801EA9A0(int player);
f32 fn_801E7E18(void);
LapInfo* fn_801E7E3C(void);
void fn_80147ABC(LayoutItem* a, LayoutItem* b, const char* fmt, f32 t, ...);
}

int GetNumPlayers(void);
const char* GetGradeDisplayName(int grade);

extern s32 lbl_8055E3C4;  // frame counter
extern f32 lbl_8055E3C8;  // current time
extern f32 lbl_80558DC4[];

static inline s32 LoadScoreTex(void) {
    return fn_802CAC28("betatex2.tex", 0);
}

static inline Sound* LoadGradeSound(void) {
    return fn_80207848("gradeup.sfx", 1);
}

namespace {

class Timer {
public:
    Timer(f32 duration);
    f32 Now(void);
    f32 Progress(void);

    f32 duration;  // offset 0x0
    f32 start;     // offset 0x4
};

class Overlay {
public:
    virtual ~Overlay(void) {}
    virtual void Update(int a, int b) = 0;  // 0xC
    virtual void Reset(void);               // 0x10
};

class ScoreOverlay : public Overlay {
public:
    ScoreOverlay(void) { tex = LoadScoreTex(); }
    virtual ~ScoreOverlay(void);
    virtual void Update(int player, int score);

    s32 tex;  // offset 0x4
    u32 unk8;
};

struct HeatState {
    f32 start;  // offset 0x0
    f32 level;  // offset 0x4
    f32 rate;   // offset 0x8
};

class OverheatOverlay : public Overlay {
public:
    virtual ~OverheatOverlay(void);
    virtual void Update(int player, int unused);
    virtual void Reset(void);

    HeatState state[8];  // offset 0x4
};

class GradeOverlay : public Overlay {
public:
    GradeOverlay(void) : popTimer(0.8f), fadeTimer(0.5f) {
        sound = LoadGradeSound();
        grades[0] = -1;
        grades[1] = -1;
        grades[2] = -1;
        grades[3] = -1;
        grades[4] = -1;
        grades[5] = -1;
        grades[6] = -1;
        grades[7] = -1;
    }
    virtual ~GradeOverlay(void);
    virtual void Update(int player, int score);
    virtual void Reset(void);

    s32 grades[8];    // offset 0x4
    Sound* sound;     // offset 0x24
    Timer popTimer;   // offset 0x28
    Timer fadeTimer;  // offset 0x30
};

struct LapState {
    s32 lap;    // offset 0x0
    f32 start;  // offset 0x4
};

class LapTitleOverlay : public Overlay {
public:
    virtual ~LapTitleOverlay(void);
    virtual void Update(int player, int unused);
    virtual void Reset(void);

    LapState state[8];  // offset 0x4
};

class ContinueOverlay : public Overlay {
public:
    virtual ~ContinueOverlay(void);
    virtual void Update(int a, int b);
};

class OutOfTimeOverlay : public Overlay {
public:
    virtual ~OutOfTimeOverlay(void);
    virtual void Update(int a, int b);
};

}  // namespace

ModuleTag lbl_8055D5E8;
static UnkStatic123 lbl_80358668;
UnkStatic7F lbl_8055D5F0;

Layout* lbl_8055D5F4;
Layout* lbl_8055D5F8;
Layout* lbl_8055D5FC;
Layout* lbl_8055D600;
Layout* lbl_8055D604;
Overlay* lbl_80358680[6];

static inline f32 InvLerp(f32 x, f32 lo, f32 hi) { return (x - lo) / (hi - lo); }
static inline Layout* GetLayout(void) {
    return fn_801E85A4() == 2 ? lbl_8055D5F8 : lbl_8055D5F4;
}

static inline Layout* GetResLayout(void) {
    return fn_801E85A4() == 2 ? lbl_8055D600 : lbl_8055D5FC;
}

static inline LayoutItem* FindResItem(const char* name) {
    return GetResLayout()->Find(name);
}

static inline u32 WithAlpha(u32 color, s32 alpha) {
    return (color & 0xFFFFFF) | (alpha << 24);
}

static inline f32 Lerp(f32 a, f32 b, f32 t) {
    return a + t * (b - a);
}

static inline f32 Clamp01(f32 x) {
    f32 v = MIN(1.0f, x);
    return MAX(v, 0.0f);
}

static inline void CopyString(int size, char* dst, const char* src) {
    while ((*dst++ = *src++) != 0) {
        if (--size <= 0) {
            dst[-1] = 0;
            break;
        }
    }
}

f32 Pulse(int period) {
    f32 s = sin(6.2831855f * (f32)(lbl_8055E3C4 % period) / (f32)period);
    return 0.5f + 0.5f * s;
}

f32 PulseFast(void) {
    return Pulse(64);
}

Timer::Timer(f32 duration) {
    this->duration = duration;
}

f32 Timer::Now(void) {
    return lbl_8055E3C8;
}

f32 Timer::Progress(void) {
    return Clamp01((Now() - start) / duration);
}

void InitOverlays(void) {
    lbl_8055D5F4 = fn_80145E70("betalyt.val");
    lbl_8055D5F8 = fn_80145E70("betalyt_2.val");
    lbl_8055D5FC = fn_80145E70("betares.val");
    lbl_8055D600 = fn_80145E70("betares_2.val");
    lbl_8055D604 = fn_80145E70("laptitle.val");
    lbl_80358680[0] = new ("ScoreOverlay") ScoreOverlay;
    lbl_80358680[1] = new ("OverheatOverlay") OverheatOverlay;
    lbl_80358680[2] = new ("GradeOverlay") GradeOverlay;
    lbl_80358680[3] = new ("LapTitleOverlay") LapTitleOverlay;
    lbl_80358680[4] = new ("ContinueOverlay") ContinueOverlay;
    lbl_80358680[5] = new ("OutOfTimeOverlay") OutOfTimeOverlay;
}

void ShutdownOverlays(void) {
    int i;

    delete lbl_8055D604;
    lbl_8055D604 = NULL;
    delete lbl_8055D5FC;
    lbl_8055D5FC = NULL;
    delete lbl_8055D600;
    lbl_8055D600 = NULL;
    delete lbl_8055D5F4;
    lbl_8055D5F4 = NULL;
    delete lbl_8055D5F8;
    lbl_8055D5F8 = NULL;
    for (i = 0; i < 6; i++) {
        delete lbl_80358680[i];
        lbl_80358680[i] = NULL;
    }
}

void ResetOverlays(void) {
    u32 i;

    for (i = 0; i < sizeof(lbl_80358680) / sizeof(lbl_80358680[0]); i++) {
        lbl_80358680[i]->Reset();
    }
}

void Overlay::Reset(void) {}

void UpdateScoreOverlays(int a, int b) {
    lbl_80358680[0]->Update(a, b);
    lbl_80358680[2]->Update(a, b);
}

void UpdateOverheatOverlay(int player) {
    lbl_80358680[1]->Update(player, 0);
}

void UpdateLapTitleOverlay(int player) {
    lbl_80358680[3]->Update(player, 0);
}

void UpdateContinueOverlay(void) {
    lbl_80358680[4]->Update(9, 17);
}

void UpdateOutOfTimeOverlay(void) {
    lbl_80358680[5]->Update(0, 0);
}

inline ScoreOverlay::~ScoreOverlay(void) {
    fn_802CAC54(tex);
}

inline void ScoreOverlay::Update(int player, int score) {
    Layout* layout = GetLayout();
    int diff;
    int art;
    s32 t;
    u32 color;
    LayoutItem* p;
    LayoutItem item;
    char total[20];
    char text[20];

    if (layout) {
        diff = fn_80204CF0();
        switch (diff) {
        case 0:
            art = 0;
            break;
        case 1:
            art = 2;
            break;
        case 2:
            art = 4;
            break;
        default:
            fn_802E30C8("stdoverlay: Couldn't find score art idx for difficulty %d, returning 0", diff);
            art = 0;
            break;
        }
        fn_8014784C(layout, "score_art", "excite_artset", art);
        t = tex;
        color = layout->Find("star_color_1")->color;
        fn_801476D8(layout->FindArt("score_star_art"), color, t);
        fn_801609C4(score);
        p = layout->Find("score");
        if (p) {
            item = *p;
            int multi = GetNumPlayers() > 1;
            if (!multi) {
                fn_801476C4(layout->Find("score_slash"));
                fn_801308F4(total, sizeof(total), fn_80160A90());
                fn_801473F8(layout, "score_denominator", "%s", total);
            }
            fn_801308F8(text, sizeof(text), score);
            fn_80147230(&item, "%s", text);
        }
    }
}

inline OverheatOverlay::~OverheatOverlay(void) {}

inline void OverheatOverlay::Update(int player, int unused) {
    Truck* truck = fn_8017960C(player);
    LayoutItem* a;
    s32 alpha;
    LayoutItem warn;
    int hot;
    HeatState* s = &state[player];
    LayoutItem* b;
    Layout* layout;
    LayoutItem item;

    hot = truck->heat > 0.0f;
    if (hot) {
        if (!((f32)fabs(s->level - 1.0f) <= lbl_80558DC4[0])) {
            s->start = lbl_8055E3C8;
        }
        s->level = 1.0f;
    } else {
        s->level = s->level - 0.033366702f;
    }
    s->rate = truck->heatRate;
    layout = GetLayout();
    if (layout) {
        if (s->level > 0.01f) {
            alpha = 255.0f * s->level;
            a = layout->Find("overheat_text");
            b = layout->Find("overheat_text_end");
            if (a && b) {
                fn_80147CEC(&item, a, b, Lerp(0.0f, 1.0f, Clamp01((lbl_8055E3C8 - s->start - 1.0f) / 0.39999998f)));
                item.color = WithAlpha(fn_80147C50(a->color, b->color, PulseFast()), alpha);
                fn_80232928(WithAlpha(fn_80147C50(b->color, a->color, PulseFast()), alpha));
                fn_80147230(&item, "%s", fn_80131234("RaceOverlay:OVERHEAT"));
                fn_80232930();
            }
        } else if (s->rate > 0.66f) {
            a = layout->Find("overheat_warn");
            if (a) {
                warn = *a;
                warn.color = fn_80147C50(0xC0FFFFFF, 0xFFFF2020, PulseFast());
                fn_801476C4(&warn);
            }
        }
    }
}

inline void OverheatOverlay::Reset(void) {
    for (int i = 0; i < sizeof(state) / sizeof(state[0]); i++) {
        state[i].start = -999.0f;
        state[i].level = 0.0f;
        state[i].rate = 0.0f;
    }
}

inline GradeOverlay::~GradeOverlay(void) {
    if (sound) {
        delete sound;
    }
}

inline void GradeOverlay::Update(int player, int score) {
    int grade = fn_801609C4(score);
    Layout* layout = GetLayout();
    f32 scale;
    LayoutItem* p;
    const char* name;
    u32 color;
    int popping;
    f32 t;
    LayoutItem title;
    LayoutItem item;

    if (layout) {
        fn_8014784C(layout, "grade_circle", "grade_circle_artset", 0);
        if (grade <= 4 && grade >= 0) {
            if (grade != grades[player]) {
                if (grade == 4) {
                    fadeTimer.start = fadeTimer.Now();
                } else {
                    popTimer.start = popTimer.Now();
                }
            }
            scale = 2.0f - fadeTimer.Progress();
            p = layout->Find("grade_title");
            if (p) {
                title = *p;
                title.scale *= scale;
                color = title.color;
                title.color = fn_80147C50(color & 0xFFFFFF, color, fadeTimer.Progress());
                fn_801476C4(&title);
            }
            if (grade >= 0) {
                p = layout->Find("grade");
                if (p) {
                    item = *p;
                    name = GetGradeDisplayName(grade);
                    t = popTimer.Now() - popTimer.start;
                    popping = 0;
                    if (t >= 0.0f && t < popTimer.duration) {
                        popping = 1;
                    }
                    if (popping) {
                        name = GetGradeDisplayName(grade);
                        if (popTimer.Progress() > 0.5f) {
                            item.scale *= Lerp(1.0f, 1.7f, Clamp01((popTimer.Progress() - 1.0f) / -0.5f));
                        } else {
                            item.scale *= Lerp(1.7f, 1.0f, Clamp01((popTimer.Progress() - 0.5f) / -0.5f));
                        }
                    } else {
                        item.scale *= scale;
                        color = item.color;
                        item.color = fn_80147C50(color & 0xFFFFFF, color, fadeTimer.Progress());
                    }
                    fn_80147230(&item, name);
                }
            }
        }
    }
    grades[player] = grade;
}

inline void GradeOverlay::Reset(void) {
    fadeTimer.start = -999.0f;
    popTimer.start = -999.0f;
}

inline LapTitleOverlay::~LapTitleOverlay(void) {}

inline void LapTitleOverlay::Update(int player, int unused) {
    const char* str;
    char name[32];
    Racer* racer = fn_801EA9A0(player);
    f32 now = fn_801E7E18();
    f32 t;
    LapInfo* info;
    char leftText[32];
    LayoutItem** items;
    char rightText[32];
    int seg;
    int i;
    char key[30];
    int side;
    LapState* s = &state[player];

    if (racer->lap > s->lap) {
        s->start = now;
        s->lap = racer->lap;
    }
    Layout* layout = lbl_8055D604;
    f32 elapsed = now - state[player].start;
    f32 keys[4] = {0.0f, 0.15f, 0.65f, 0.8f};
    if (layout && elapsed < keys[3]) {
        LayoutItem* left[4];
        LayoutItem* right[4];

        for (side = 0; side < 2; side++) {
            items = side == 0 ? left : right;
            for (i = 0; i < 4; i++) {
                fn_801345D8(name, sizeof(name), "%s_%d", side == 0 ? "left" : "right", i);
                *items++ = layout->Find(name);
            }
        }
        info = fn_801E7E3C();
        if (s->lap != 1 && s->lap <= 3) {
            if (s->lap < info->total) {
                str = fn_80131234("BigText:LAP_LEFT");
                CopyString(32, leftText, str);
                fn_801345D8(key, sizeof(key), "BigText:LAP_RIGHT_%d", s->lap);
                str = fn_80131234(key);
                CopyString(32, rightText, str);
            } else if (info->total == s->lap) {
                str = fn_80131234("BigText:FINAL_LEFT");
                CopyString(32, leftText, str);
                str = fn_80131234("BigText:FINAL_RIGHT");
                CopyString(32, rightText, str);
            } else if (info->total + 1 == s->lap) {
                return;
            } else {
                return;
            }
            seg = 0;
            if (elapsed < keys[1]) {
                seg = 0;
            } else if (elapsed < keys[2]) {
                seg = 1;
            } else if (elapsed < keys[3]) {
                seg = 2;
            }
            t = Lerp(0.0f, 1.0f, Clamp01(InvLerp(elapsed, keys[seg], keys[seg + 1])));
            fn_80232928(0xFFFFFFFF);
            fn_80147ABC(left[seg], left[seg + 1], "%s", t, leftText);
            fn_80147ABC(right[seg], right[seg + 1], "%s", t, rightText);
            fn_80232930();
        }
    }
}

inline void LapTitleOverlay::Reset(void) {
    state[0].lap = 0;
    state[0].start = -999.0f;
    state[1].lap = 0;
    state[1].start = -999.0f;
    state[2].lap = 0;
    state[2].start = -999.0f;
    state[3].lap = 0;
    state[3].start = -999.0f;
    state[4].lap = 0;
    state[4].start = -999.0f;
    state[5].lap = 0;
    state[5].start = -999.0f;
    state[6].lap = 0;
    state[6].start = -999.0f;
    state[7].lap = 0;
    state[7].start = -999.0f;
}

inline ContinueOverlay::~ContinueOverlay(void) {}

inline void ContinueOverlay::Update(int a, int b) {
    fn_801476C4(FindResItem("continue_prompt"));
}

inline OutOfTimeOverlay::~OutOfTimeOverlay(void) {}

inline void OutOfTimeOverlay::Update(int a, int b) {
    fn_801476C4(FindResItem("timed_out"));
    fn_801476C4(FindResItem("disqualified"));
}
