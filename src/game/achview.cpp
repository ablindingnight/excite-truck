// Achievements viewer (0x80259148-0x8025C028): the trophy cabinet ("trophy", "cnt1".."cnt3",
// "MenuDoAchievementsViewer: bad return value %d ...") with its camera orbit and trophy spin helpers.
// Written against raw field offsets; the large routines and the initialiser are still stubs. Names
// are guesses apart from the strings.
#include "game/common.h"
#include "macros.h"
#include "game/menustate.h"
#include "game/menu.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

// Interface of UIObjectsImpl (uiobjs.cpp).
class UIObjects {
public:
    virtual ~UIObjects(void);
    virtual void AddImage(const char* name, const char* file, int frame);
    virtual void AddFont(const char* name, const char* file);
    virtual void AddSound(const char* name, const char* file, u8 loop);
    virtual void s18(void);
    virtual void s1C(void);
    virtual void s20(void);
    virtual void s24(void);
    virtual void s28(void);
    virtual void s2C(void);
    virtual void s30(void);
    virtual void s34(void);
    virtual void PlaySound(const char* name);               // 0x38
    virtual void StopSound(const char* name);               // 0x3C
    virtual void SetSoundVolume(const char* name, f32 v);   // 0x40
};

class VA {
public:
    virtual void v08(void);
    virtual void v0C(u8* q);
};

extern "C" {
void fn_80236914(void);
void fn_802C6CF8(s32 h);
void fn_802CAC54(u8* p);
void fn_80214AF0(int s);
void fn_80214A9C(int s);
f32 PSVECDotProduct(const f32* a, const f32* b);
double sqrt(double x);
double sin(double x);
}

const char* AchievementsGetLevelModel(int cat, int level);

extern "C" int fn_801345D8(char* buf, int n, const char* fmt, ...);

// Copies at most n characters, always terminating.
static inline void CopyName(const char* src, int n, char* dst) {
    char* d = dst;
    int left = n;
    while ((*d++ = *src++) != 0) {
        if (--left <= 0) {
            d[-1] = 0;
            break;
        }
    }
}

// Shows one trophy: its model name and its level caption.
static inline void SetTrophy(u8* self, s32 cat, s32 level) {
    u8* m = PTR(self, 0x18);
    S32(m, 0x4C) = level;
    const char* name = AchievementsGetLevelModel(cat, level);
    if (name) {
        CopyName(name, 50, (char*)(m + 0x4));
    } else {
        U8(m, 0x4) = 0;
    }
    if (level > 0) {
        fn_801345D8((char*)(m + 0x36), 0x14, "Level %d", level);
    } else {
        U8(m, 0x36) = 0;
    }
}

struct Words9 {
    u32 w[9];
};

extern UIObjects* lbl_8055F394;
extern "C" UIObjects* fn_80258354(void);
extern void* lbl_80345158[];
extern bool lbl_8055F3A0;
extern f32 lbl_8055F3A4;

extern "C" {

void fn_80259148(void) {
    lbl_8055F394 = fn_80258354();
    lbl_8055F394->AddSound("achieve_enter", "a_achieve.sfx", 0);
    lbl_8055F394->AddSound("achieve_trophy", "a_trophy1.sfx", 0);
    lbl_8055F394->AddSound("achieve_tally", "a_tally.sfx", 1);
}

void fn_802591D8(void) {
    delete lbl_8055F394;
    lbl_8055F394 = 0;
}

u32 fn_8025921C(void) { return 0; }

}  // extern "C"

// Achievement categories in viewer order.
struct AchCategoryRef {
    s32 category;
    const char* name;
};
AchCategoryRef lbl_80344EA0[7] = {
    {1, "ach_1st.art"}, {2, "ach_aaa.art"}, {3, "ach_air.art"}, {4, "ach_trun.art"},
    {5, "ach_drft.art"}, {7, "ach_hits.art"}, {6, "ach_ring.art"},
};

void* operator new(unsigned long n, const char* tag);
extern const char lbl_8055BAB8[];  // ""
extern const char lbl_8055B860[];  // ""
s32 AchievementsGetLevelThreshold(int category, int level);
extern "C" {
u8* fn_802CAC28(const char* name, int a);
int fn_801345D8(char* buf, int n, const char* fmt, ...);
}

// Tally speeds by level.
extern const f32 lbl_802FA628[5] = {0.75f, 0.8f, 0.85f, 0.95f, 1.0f};
extern const MenuDesc lbl_802FA63C = {"ep_trophy.lyt", 0, 0, 0, 0};

// The trophy model shown in the achievement viewer (vtable lbl_80345158; deleting dtor fn_80259680).
struct TrophyView {
    void* vt;          // offset 0x0
    char model[50];    // offset 0x4
    char level[20];    // offset 0x36
    u8 shown;          // offset 0x4A
    s32 levelNum;      // offset 0x4C
    s32 handle;        // offset 0x50
    u8 pad54[0x30];
    f32 rot[9];        // offset 0x84
    u8* env;           // offset 0xA8

    void Release(void) {
        vt = lbl_80345158;
        if (handle != -1) {
            fn_802C6CF8(handle);
            handle = -1;
        }
        fn_802CAC54(env);
    }
};

extern void* lbl_803450F8[];
extern void* lbl_80345038[];


// Sets up a trophy view (identity rotation, environment map, level 5).
#define TROPHY_VIEW_INIT(v)                      \
    (v).vt = lbl_80345158;                       \
    (v).handle = -1;                             \
    (v).rot[0] = 1.0f;                           \
    (v).rot[1] = 0.0f;                           \
    (v).rot[2] = 0.0f;                           \
    (v).rot[3] = 0.0f;                           \
    (v).rot[4] = 1.0f;                           \
    (v).rot[5] = 0.0f;                           \
    (v).rot[6] = 0.0f;                           \
    (v).rot[7] = 0.0f;                           \
    (v).rot[8] = 1.0f;                           \
    (v).model[0] = 0;                            \
    (v).level[0] = 0;                            \
    (v).env = fn_802CAC28("tr_env.tex", 0);      \
    (v).shown = 0;                               \
    (v).levelNum = 5

// The trophy's menu widget (vtable lbl_803450F8): the viewer, the level and its caption.
struct TrophyController {
    TrophyController(TrophyView* v, s32 cat, s32 lvl) {
        unk14 = 0;
        vt = lbl_803450F8;
        view = v;
        category = cat;
        level = lvl;
        text[0] = 0;
        shownLevel = lvl;
    }
    void* vt;          // offset 0x0
    s32 x, y, w, h;    // offset 0x4
    int unk14;         // offset 0x14
    TrophyView* view;  // offset 0x18
    s32 level;         // offset 0x1C
    char text[0x50];   // offset 0x20
    s32 category;      // offset 0x70
    s32 shownLevel;    // offset 0x74
};

extern UnkStatic123s lbl_80498E98;
const char* AchievementsGetCategoryName(int cat);


extern "C" {
void fn_802368E0(void);
void fn_80216CA8(void);
void fn_80259740(int x, f32 v);

// The trophy viewer for one achievement row: its model, level and the five level thresholds.
int fn_80259224(u8* p) {
    TrophyView view;
    TROPHY_VIEW_INIT(view);
    TrophyController* ctl;
    if (fn_8025921C()) {
        ctl = new ("trophycontroller") TrophyController(&view, S32(p, 0x18), 5);
    } else {
        ctl = new ("trophycontroller") TrophyController(&view, S32(p, 0x18), S32(p, 0xC8));
    }
    s32 level = S32(p, 0xC8);
    view.levelNum = level;
    const char* model = AchievementsGetLevelModel(S32(p, 0x18), level);
    if (model) {
        CopyName(model, 50, view.model);
    } else {
        view.model[0] = 0;
    }
    if (level > 0) {
        fn_801345D8(view.level, 20, "Level %d", level);
    } else {
        view.level[0] = 0;
    }
    view.shown = 1;
    char counts[32][5];
    for (int i = 1; i <= 5; i++) {
        fn_801345D8(counts[i - 1], 0x20, "%d", AchievementsGetLevelThreshold(S32(p, 0x18), i));
    }
    MenuItem items[] = {
        MenuItem(lbl_8055BAB8, 8, (const void*)fn_80259740, 2, 0),
        MenuItem(0, 5, ctl, 0, 0),
        MenuItem("trophy", 0x13, &view, 0, 0),
        MenuItem("trophy", ctl->text),
        MenuItem("achievement_title", AchievementsGetCategoryName(S32(p, 0x18))),
        MenuItem("achievement_icon", lbl_80344EA0[S32(p, 0x24)].name),
        MenuItem("cnt1", counts[0]),
        MenuItem("cnt2", counts[1]),
        MenuItem("cnt3", counts[2]),
        MenuItem("cnt4", counts[3]),
        MenuItem("cnt5", counts[4]),
        MenuItem(lbl_8055B860, 6, 0, -1, 27),
        MenuItem(lbl_80498E98),
    };
    MenuDesc desc = lbl_802FA63C;
    desc.items = items;
    fn_802368E0();
    fn_80216CA8();
    int r = fn_802146BC(&desc, -999, -999, 1);
    if (ctl) {
        delete (MenuState*)ctl;
    }
    view.Release();
    return r;
}

u8* fn_80259680(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_80345158;
        if (S32(p, 0x50) != -1) {
            fn_802C6CF8(S32(p, 0x50));
            S32(p, 0x50) = -1;
        }
        fn_802CAC54(PTR(p, 0xA8));
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

u8* fn_80259700(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_80259740(int x, f32 v) {
    fn_80236914();
    lbl_8055F3A4 = v;
    lbl_8055F3A0 = x == 2;
}

}  // extern "C"

// The two-sided trophy's menu widget (vtable lbl_80345038): a view and the row it shows.
struct TrophyFace {
    TrophyFace(TrophyView* v, u8* row) {
        unk14 = 0;
        vt = lbl_80345038;
        view = v;
        owner = row;
        category = S32(row, 0x18);
        level = S32(row, 0xC8);
    }
    void* vt;          // offset 0x0
    s32 x, y, w, h;    // offset 0x4
    int unk14;         // offset 0x14
    TrophyView* view;  // offset 0x18
    u8* owner;         // offset 0x1C
    s32 category;      // offset 0x20
    s32 level;         // offset 0x24
};

// Shows the row's current trophy model and level on a view.
#define TROPHY_VIEW_SHOW(v, row)                                                 \
    {                                                                            \
        s32 lvl = S32(row, 0xC8);                                                \
        (v).levelNum = lvl;                                                      \
        const char* model = AchievementsGetLevelModel(S32(row, 0x18), lvl);      \
        if (model) {                                                             \
            CopyName(model, 50, (v).model);                                      \
        } else {                                                                 \
            (v).model[0] = 0;                                                    \
        }                                                                        \
        if (lvl > 0) {                                                           \
            fn_801345D8((v).level, 20, "Level %d", lvl);                         \
        } else {                                                                 \
            (v).level[0] = 0;                                                    \
        }                                                                        \
    }

struct AchQuery3 {
    s32 category;  // offset 0x0
    s32 value;     // offset 0x4
    s32 best;      // offset 0x8
};
void AchievementsQuery(AchQuery3* q);
int AchievementsGetLevel(int category, s32 value);

extern "C" {
void fn_80236910(const char* name);
const char* fn_80131234(const char* key);

// The achievement viewer for one row (with the level-up "new" effect while tallying): the trophy front and
// back, the progress towards the next level and every category's level bar.
int fn_8025978C(u8* p) {
    s32 cat;
    lbl_8055F3A0 = 0;
    TrophyView front;
    TROPHY_VIEW_INIT(front);
    TrophyView back;
    TROPHY_VIEW_INIT(back);
    back.shown = !U8(p, 0xC4);
    TrophyFace frontFace(&front, p);
    TrophyFace backFace(&back, p);
    TROPHY_VIEW_SHOW(front, p);
    TROPHY_VIEW_SHOW(back, p);
    char toNext[0x20];
    fn_801345D8(toNext, 0x20, "%d %s", S32(p, 0x20) - S32(p, 0x1C), fn_80131234("Achievements:UP") + 1);
    s32* vals[7];
    s32 barColor[7];
    s32 levels[7];
    s32 iconColor[7];
    for (u32 i = 0; i < 7; i++) {
        cat = lbl_80344EA0[i].category;
        if (cat == S32(p, 0x18)) {
            iconColor[i] = 9;
            barColor[i] = 8;
            vals[i] = (s32*)(p + 0xC8);
        } else {
            iconColor[i] = 10;
            barColor[i] = 11;
            AchQuery3 q;
            q.category = cat;
            AchievementsQuery(&q);
            levels[i] = AchievementsGetLevel(cat, S32(p, 0x18) > cat ? q.best : q.value);
            vals[i] = &levels[i];
        }
    }
    if (U8(p, 0xC4) && U8(p, 0xC5)) {
        fn_80236910("a_ep_achieve_new.val");
    }
    MenuItem items[] = {
        MenuItem(lbl_8055BAB8, 8, (const void*)fn_80259740, 2, 0),
        MenuItem("achievement_custom", 5, p, 0, 0),
        MenuItem(0, 5, &frontFace, 0, 0),
        MenuItem(0, 5, &backFace, 0, 0),
        MenuItem("trophy", 0x13, &front, 0, 0),
        MenuItem("trophy", (const char*)(frontFace.owner + 0x60)),
        MenuItem("back_trophy", 0x13, &back, 0, 0),
        MenuItem("back_trophy", (const char*)(backFace.owner + 0x60)),
        MenuItem("achievement_icon", lbl_80344EA0[S32(p, 0x24)].name),
        MenuItem("achievement_title", AchievementsGetCategoryName(S32(p, 0x18))),
        MenuItem("achievement_value", (const char*)(p + 0x2C)),
        MenuItem("next_level_value", (const char*)(p + 0x40)),
        MenuItem("increment_amt", toNext),
        MenuItem("back_1", 0x14, &barColor[0], 0, 0),
        MenuItem("icon_1", 0x14, &iconColor[0], 0, 0),
        MenuItem("bars_1", 2, vals[0], 0, 0),
        MenuItem("icon_1", lbl_80344EA0[0].name),
        MenuItem("title_1", AchievementsGetCategoryName(lbl_80344EA0[0].category)),
        MenuItem("back_2", 0x14, &barColor[1], 0, 0),
        MenuItem("icon_2", 0x14, &iconColor[1], 0, 0),
        MenuItem("bars_2", 2, vals[1], 0, 0),
        MenuItem("icon_2", lbl_80344EA0[1].name),
        MenuItem("title_2", AchievementsGetCategoryName(lbl_80344EA0[1].category)),
        MenuItem("back_3", 0x14, &barColor[2], 0, 0),
        MenuItem("icon_3", 0x14, &iconColor[2], 0, 0),
        MenuItem("bars_3", 2, vals[2], 0, 0),
        MenuItem("icon_3", lbl_80344EA0[2].name),
        MenuItem("title_3", AchievementsGetCategoryName(lbl_80344EA0[2].category)),
        MenuItem("back_4", 0x14, &barColor[3], 0, 0),
        MenuItem("icon_4", 0x14, &iconColor[3], 0, 0),
        MenuItem("bars_4", 2, vals[3], 0, 0),
        MenuItem("icon_4", lbl_80344EA0[3].name),
        MenuItem("title_4", AchievementsGetCategoryName(lbl_80344EA0[3].category)),
        MenuItem("back_5", 0x14, &barColor[4], 0, 0),
        MenuItem("icon_5", 0x14, &iconColor[4], 0, 0),
        MenuItem("bars_5", 2, vals[4], 0, 0),
        MenuItem("icon_5", lbl_80344EA0[4].name),
        MenuItem("title_5", AchievementsGetCategoryName(lbl_80344EA0[4].category)),
        MenuItem("back_6", 0x14, &barColor[5], 0, 0),
        MenuItem("icon_6", 0x14, &iconColor[5], 0, 0),
        MenuItem("bars_6", 2, vals[5], 0, 0),
        MenuItem("icon_6", lbl_80344EA0[5].name),
        MenuItem("title_6", AchievementsGetCategoryName(lbl_80344EA0[5].category)),
        MenuItem("back_7", 0x14, &barColor[6], 0, 0),
        MenuItem("icon_7", 0x14, &iconColor[6], 0, 0),
        MenuItem("bars_7", 2, vals[6], 0, 0),
        MenuItem("icon_7", lbl_80344EA0[6].name),
        MenuItem("title_7", AchievementsGetCategoryName(lbl_80344EA0[6].category)),
        MenuItem(lbl_80498E98),
    };
    MenuDesc desc = {U8(p, 0xC4) ? "ep_achieve.lyt" : "ach_viewer.lyt", items, 0, 0, 0};
    fn_802368E0();
    if (U8(p, 0xC4)) {
        fn_80216CA8();
    }
    int r = fn_802146BC(&desc, -999, -999, 1);
    int wait = 1;
    while (wait) {
        if (r == -2) {
            if (!(F32(p, 0xCC) < 1.5f && U8(p, 0xC5))) {
                wait = 0;
            }
        } else {
            wait = 0;
        }
    }
    back.Release();
    front.Release();
    return r;
}

u8* fn_8025A1C4(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

}  // extern "C"

struct AchQuery {
    s32 category;  // offset 0x0
    s32 value;     // offset 0x4
    s32 best;      // offset 0x8
};

void AchievementsQuery(AchQuery* q);
int AchievementsGetLevel(int category, s32 value);
s32 AchievementsGetLevelThreshold(int category, int level);
int AchievementsGetLevelCount(int category);
extern "C" void fn_80130FCC(char* buf, s32 size, s32 value);
extern f32 lbl_80558DC4[];  // FLT_EPSILON
int strcasecmp(const char* a, const char* b);
void fn_80236954(const char* name);
void fn_80236958(const char* name);
f32 fn_80214D48(void);
void fn_802168BC(int);
extern u8 lbl_8055F67C;     // demo: show every category one step ahead
extern f32 lbl_8055F398;     // trophy spin angle
extern f32 lbl_80498EAC[3];  // smoothed view direction

// One achievement category shown in the viewer (vtable lbl_80345098).
// Fixed-capacity name buffer (original class name unknown).
// CopyFrom is a bounded copy that always terminates.
template<int N> struct NameBuffer {
    char text[N];
    operator char*() { return text; }
    operator const char*() const { return text; }
    void CopyFrom(const char* s) {
        char* d = text;
        int n = N;
        while ((*d++ = *s++) != 0) {
            if (--n <= 0) { d[-1] = 0; break; }
        }
    }
};
typedef char AchievementNameSize20[(sizeof(NameBuffer<20>) == 20) ? 1 : -1];
typedef char AchievementNameSize50[(sizeof(NameBuffer<50>) == 50) ? 1 : -1];

class AchView : public MenuState {
public:
    AchView(int idx, int cat, f32 rate) {
        index = idx;
        q.category = cat;
        tallyRate = rate;
        tally = 0.0f;
        AchievementsQuery(&q);
        if (lbl_8055F67C) {
            q.best = q.value + 50;
        }
        int bestLevel = AchievementsGetLevel(q.category, q.best);
        int currentLevel = AchievementsGetLevel(q.category, q.value);
        levelUp = currentLevel != bestLevel;
        Refresh();
    }
    virtual ~AchView(void) {}
    virtual void Enter(void);
    virtual void Exit(void);
    virtual void Update(void);

    void Refresh(void) {
        shown = (f32)q.value;
        ShowValue();
    }

    // The level-up marker flashes for a moment after the trophy is awarded.
    bool Flashing(void) { return tally < 1.5f && levelUp; }

    // Shows the tallied value, the next level's threshold and the trophy model.
    void ShowValue(void) {
        s32 v;
        int show;
        v = (s32)(shown + lbl_80558DC4[0]);
        fn_80130FCC(valueText, 0x14, v);
        show = 1;
        if (counting && (started || !levelUp)) {
            show = 0;
        }
        level = AchievementsGetLevel(q.category, v);
        s32 next = AchievementsGetLevelThreshold(q.category, level + 1);
        if (next >= 0) {
            fn_80130FCC(nextText, 0x14, next);
        } else {
            nextText.CopyFrom("");
        }
        const char* model = AchievementsGetLevelModel(q.category, level);
        if (model && show) {
            modelName.CopyFrom(model);
        } else {
            modelName.CopyFrom("");
        }
    }

    AchQuery q;          // offset 0x18
    s32 index;           // offset 0x24
    f32 shown;           // offset 0x28
    char valueText[20];  // offset 0x2C
    NameBuffer<20> nextText;   // offset 0x40
    f32 tallyRate;       // offset 0x54
    u8 started;          // offset 0x58
    u8 pad59[0x7];
    NameBuffer<50> modelName;  // offset 0x60
    NameBuffer<50> previousModelName;
    u8 counting;         // offset 0xC4
    u8 levelUp;          // offset 0xC5
    u8 padC6[0x2];
    s32 level;           // offset 0xC8
    f32 tally;           // offset 0xCC
};


extern "C" {
void fn_80259148(void);
void fn_802591D8(void);
int fn_8025978C(u8* p);
int fn_80259224(u8* p);
void fn_802E317C(const char* fmt, ...);

// Tally run: every category that changed since the last visit counts up in turn.
void fn_8025A204(void) {
    fn_80259148();
    lbl_80498EAC[0] = 0.0f;
    lbl_80498EAC[1] = -1.0f;
    lbl_80498EAC[2] = 0.0f;
    lbl_8055F398 = 0.31415927f;
    for (int i = 0; i < 7; i++) {
        AchView v(i, lbl_80344EA0[i].category, lbl_80344EA0[i].category == 0 ? 500.0f : 12.0f);
        v.counting = 1;
        if (v.q.best != v.q.value) {
            lbl_8055F394->PlaySound("achieve_enter");
            fn_8025978C((u8*)&v);
        }
    }
    fn_802591D8();
}

u8* fn_8025A4CC(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

// Browse run: left/right page through the categories until the player backs out.
void fn_8025A50C(void) {
    fn_80259148();
    lbl_80498EAC[0] = 0.0f;
    lbl_80498EAC[1] = -1.0f;
    lbl_80498EAC[2] = 0.0f;
    lbl_8055F398 = 0.31415927f;
    int i = 0;
    int run = 1;
    while (run) {
        AchView v(i, lbl_80344EA0[i].category, 0.0f);
        v.counting = 0;
        int r = fn_8025978C((u8*)&v);
        switch (r) {
        case 1:
            i = (i + 6) % 7;
            break;
        case 2:
            i = (i + 1) % 7;
            break;
        case 3:
            while (fn_80259224((u8*)&v) != -1) {
            }
            break;
        case -1:
            run = 0;
            break;
        default:
            fn_802E317C("MenuDoAchievementsViewer: bad return value %d from do_accumulate_single_achievement", r);
            break;
        }
    }
    fn_802591D8();
}

void fn_8025A83C(u8* p) {
    F32(p, 0x84) = 1.0f;
    F32(p, 0x88) = 0.0f;
    F32(p, 0x8C) = 0.0f;
    F32(p, 0x90) = 0.0f;
    F32(p, 0x94) = 1.0f;
    F32(p, 0x98) = 0.0f;
    F32(p, 0x9C) = 0.0f;
    F32(p, 0xA0) = 0.0f;
    F32(p, 0xA4) = 1.0f;
    ((VA*)p)->v0C(p + 4);
}

void fn_8025A87C(u8* p) {
    F32(p, 0x84) = 1.0f;
    F32(p, 0x88) = 0.0f;
    F32(p, 0x8C) = 0.0f;
    F32(p, 0x90) = 0.0f;
    F32(p, 0x94) = 1.0f;
    F32(p, 0x98) = 0.0f;
    F32(p, 0x9C) = 0.0f;
    F32(p, 0xA0) = 0.0f;
    F32(p, 0xA4) = 1.0f;
    U8(p, 0x4) = 0;
    U8(p, 0x36) = 0;
}

}  // extern "C"

// Appends s to d (n bytes in all), always terminating.
static inline void CatName(const char* s, int n, char* d) {
    while (*d) {
        n--;
        d++;
    }
    if (n > 0) {
        CopyName(s, n, d);
    }
}

extern char lbl_80345208[];  // "*uiaction.res/"

extern "C" {
char* strrchr(const char* s, int c);
int fn_8012C33C(const char* name);
s32 fn_802C6FC0(const char* name, int a, int b);

// Loads the trophy model "*uiaction.res/<name>.mo0" (replacing the old one) and resets its rotation.
void fn_8025A8B8(u8* p, const char* name) {
    if (S32(p, 0x50) != -1) {
        fn_802C6CF8(S32(p, 0x50));
        S32(p, 0x50) = -1;
    }
    if (*name && strlen(name)) {
        char buf[64];
        CopyName(lbl_80345208, 64, buf);
        CatName(name, 64, buf);
        if (strrchr(buf, '.')) {
            char* dot = strrchr(buf, '.');
            *dot = 0;
        }
        CatName(".mo0", 64, buf);
        if (fn_8012C33C(buf)) {
            S32(p, 0x50) = fn_802C6FC0(buf, 0, 0);
        }
    }
    F32(p, 0x84) = 1.0f;
    F32(p, 0x88) = 0.0f;
    F32(p, 0x8C) = 0.0f;
    F32(p, 0x90) = 0.0f;
    F32(p, 0x94) = 1.0f;
    F32(p, 0x98) = 0.0f;
    F32(p, 0x9C) = 0.0f;
    F32(p, 0xA0) = 0.0f;
    F32(p, 0xA4) = 1.0f;
}

}  // extern "C"

// Pad state as read by fn_801298E8 (partial).
struct ViewPad {
    f32 x;            // offset 0x0
    f32 y;            // offset 0x4
    u8 pad8[0x38];
    u32 hold;         // offset 0x40
    u8 pad44[0xC];
    u32 flags;        // offset 0x50, 0x20: pointer direction valid
    f32 dir[3];       // offset 0x54
    u8 pad60[0x4];
};

struct Mat3 {
    f32 m[9];
};

f32 Max(f32 a, f32 b);
f32 Remap(f32 lo, f32 hi, f32 a, f32 b, f32 x);
void Mat3RotY(Mat3* m, f32 a);
void Mat3Mul(Mat3* out, const Mat3* a, const Mat3* b);

extern f32 lbl_8055F398;     // trophy spin angle
extern f32 lbl_80498EAC[3];  // smoothed view direction

extern "C" {
int fn_801298E8(ViewPad* pad, int chan);
void StatsUpdate__Fv(f32* v);  // vector default constructor (empty, out of line)
void fn_80144DAC(f32* dst, const f32* src);
f32 fn_80144CC8(f32 x);
void fn_801BFEBC(f32* v, f32 x, f32 y, f32 z);
void fn_801F3198(f32* v);
void fn_80144B44(f32* v, f32 x, f32 y, f32 z);
f32 fn_8017F70C(f32 x);
void fn_801C0E4C(f32* out, const f32* a, const f32* b);
Mat3* fn_802C86DC(void);
void fn_8025AD3C(f32* out, const f32* a, const f32* b, f32 t);
f32 fn_8025AD88(const f32* a, const f32* b);
void fn_8025AD8C(u8* self, f32* out, const f32* a, const f32* b, f32 t);
f32 fn_8025ADF4(f32 x);

static inline f32 LerpF(f32 a, f32 b, f32 t) { return a + t * (b - a); }

// Turns the trophy: the +/- buttons spin it, the stick or pointer tilts it, then the frame at p + 0x84
// (right, up, forward) is rebuilt around the tilted up vector.
void fn_8025AA90(u8* p, f32 dt) {
    ViewPad pad;
    Mat3 rot;
    f32 r[3];
    f32 c[3];
    f32 b[3];
    f32 a[3];
    f32 fwd[3];
    f32 side[3];
    f32 v[3];
    if (S32(p, 0x50) == -1 || !U8(p, 0x4A)) {
        return;
    }
    if (fn_801298E8(&pad, 0)) {
        f32 plus = (pad.hold & 2) ? 1.0f : 0.0f;
        f32 minus = (pad.hold & 8) ? -1.0f : 0.0f;
        lbl_8055F398 += dt * (3.1415927f * (minus + plus));
        StatsUpdate__Fv(v);
        if (pad.flags & 0x20) {
            fn_80144DAC(v, pad.dir);
        } else {
            fn_801BFEBC(v, pad.x, -fn_8025ADF4(Max(0.0f, 1.0f - fn_80144CC8(pad.x) + fn_80144CC8(pad.y))), pad.y);
        }
        fn_8025AD8C(p + 0x84, lbl_80498EAC, lbl_80498EAC, v, dt);
        fn_801BFEBC((f32*)(p + 0x90), lbl_80498EAC[0], -lbl_80498EAC[1], lbl_80498EAC[2]);
        fn_801F3198((f32*)(p + 0x90));
    }
    fn_80144B44(side, 1.0f, 0.0f, 0.0f);
    fn_80144B44(fwd, 0.0f, 0.0f, 1.0f);
    f32 tilt = fn_8017F70C(fn_8025AD88(side, (f32*)(p + 0x90)));
    fn_8017F70C(fn_8025AD88((f32*)(p + 0x90), fwd));
    if (tilt > 0.707f) {
        StatsUpdate__Fv(a);
        StatsUpdate__Fv(b);
        StatsUpdate__Fv(c);
        fn_801C0E4C(c, side, (f32*)(p + 0x90));
        fn_801C0E4C(a, (f32*)(p + 0x90), c);
        fn_801C0E4C(b, (f32*)(p + 0x90), fwd);
        fn_8025AD3C(r, a, b, Remap(0.707f, 0.996f, 0.0f, 1.0f, tilt));
        fn_801C0E4C((f32*)(p + 0x9C), r, (f32*)(p + 0x90));
        fn_801F3198((f32*)(p + 0x9C));
        fn_801C0E4C((f32*)(p + 0x84), (f32*)(p + 0x90), (f32*)(p + 0x9C));
    } else {
        fn_801C0E4C((f32*)(p + 0x9C), side, (f32*)(p + 0x90));
        fn_801F3198((f32*)(p + 0x9C));
        fn_801C0E4C((f32*)(p + 0x84), (f32*)(p + 0x90), (f32*)(p + 0x9C));
    }
    Mat3RotY(&rot, lbl_8055F398);
    Mat3Mul((Mat3*)(p + 0x84), &rot, (Mat3*)(p + 0x84));
    Mat3Mul((Mat3*)(p + 0x84), (Mat3*)(p + 0x84), fn_802C86DC());
}

static inline void SetVec3(f32* v, f32 x, f32 y, f32 z) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
}
void fn_8025AD3C(f32* out, const f32* a, const f32* b, f32 t) {
    SetVec3(out, LerpF(a[0], b[0], t), LerpF(a[1], b[1], t), LerpF(a[2], b[2], t));
}

f32 fn_8025AD88(const f32* a, const f32* b) { return PSVECDotProduct(a, b); }

void fn_8025AD8C(u8* self, f32* out, const f32* a, const f32* b, f32 t) {
    f32 s = 4.1f * t;
    f32 k = 1.0f < s ? 1.0f : s;
    out[0] = LerpF(a[0], b[0], k);
    out[1] = LerpF(a[1], b[1], k);
    out[2] = LerpF(a[2], b[2], k);
}

f32 fn_8025ADF4(f32 x) { return (f32)sqrt(x); }

}  // extern "C"

struct Perspective {
    Perspective(f32 near, f32 far, f32 fov);
    ~Perspective(void);
    f32 near;  // offset 0x0
    f32 far;   // offset 0x4
    f32 fov;   // offset 0x8
};

struct LightDir {
    f32 x, y, z;
};

extern f32 lbl_8055F39C;  // trophy view field of view

extern "C" {
u8* fn_8025B134(u8* p);
void fn_8025B0E8(Words9* d, const Words9* s);
f32 fn_8025B0A8(f32* r);
f32 fn_8025B0C4(f32 x);
u8* fn_8022EDC4(void);
void fn_80144D80(Mat3* m, f32 x, f32 y, f32 z);
void fn_802C77F0(s32 model, f32* bounds);
void fn_801C2B9C(f32* out, const f32* pos, const f32* dir, f32 d);
void fn_801E67B4(f32* out, const f32* v, const u8* m);
void fn_801C0E60(f32* out, const f32* a, const f32* b);
void fn_801BFE7C(f32* out, const f32* a, const f32* b);
void fn_802E30C8(const char* fmt, ...);
u32 fn_802C9950(u32 c);
u32 fn_802C9994(u32 c);
f32 fn_802C99E8(f32 v);
LightDir* fn_802C9A40(void);
void fn_802C9A1C(const LightDir* d);
void fn_802D7554(int on);
void fn_802CA294(s32 tex);
void fn_802D9230(void);
void fn_802C744C(s32 model, u8* m);

// Trophy size per level.
struct TrophySizes {
    f32 s[5];
};

static const TrophySizes kTrophySizes = {0.75f, 0.8f, 0.85f, 0.95f, 1.0f};

// Draws the trophy model: scaled to the view, placed in front of the camera so its size class
// fills the screen, lit from a fixed direction.
// Moves the model so its bounding-box centre sits at the view point.
static inline void TrophyCenterOffset(u8* p, f32* bounds, u8* cam) {
    f32 c[3];
    StatsUpdate__Fv(c);
    fn_801BFEBC(c, fn_8025B0A8(bounds), fn_8025B0A8(bounds + 2), fn_8025B0A8(bounds + 4));
    fn_801E67B4(c, c, p + 0x54);
    fn_801C0E60((f32*)(p + 0x78), (f32*)(p + 0x78), c);
    fn_801BFE7C((f32*)(p + 0x78), (f32*)(p + 0x78), (f32*)(cam + 0x24));
}

void fn_8025AE18(u8* p) {
    Mat3 scale;
    TrophySizes sizes;
    f32 bounds[6];
    LightDir dir;
    if (S32(p, 0x50) == -1 || !U8(p, 0x4A)) {
        return;
    }
    fn_8025B0E8((Words9*)(p + 0x54), (Words9*)fn_8025B134(p + 0x84));
    u8* cam = fn_8022EDC4();
    fn_80144D80(&scale, F32(cam, 0x38), F32(cam, 0x38), F32(cam, 0x38));
    Mat3Mul((Mat3*)(p + 0x54), (Mat3*)(p + 0x54), &scale);
    fn_802C77F0(S32(p, 0x50), bounds);
    sizes = kTrophySizes;
    int level = S32(p, 0x4C);
    if (level <= 0) {
        fn_802E30C8("AchievementTrophyModel: bad level value %d", level);
    } else {
        f32 r = 4.3f * sizes.s[level - 1] / 2.0f;
        f32 d = r / fn_8025B0C4(0.5f * lbl_8055F39C);
        f32 nearD = 0.1f + r;
        u8* m = (u8*)fn_802C86DC();
        fn_801C2B9C((f32*)(p + 0x78), (f32*)(m + 0x24), (f32*)(m + 0x18), Max(d, nearD));
        TrophyCenterOffset(p, bounds, cam);
    }
    dir.x = 0.2f;
    dir.y = -0.7f;
    dir.z = 0.7f;
    u32 oldAmb = fn_802C9950(0xFF646464);
    u32 oldDiff = fn_802C9994(0xFFB4B4B4);
    f32 oldSpec = fn_802C99E8(1.0f);
    LightDir saved = *fn_802C9A40();
    fn_802C9A1C(&dir);
    fn_802D7554(1);
    fn_802CA294(S32(p, 0xA8));
    fn_802D9230();
    Perspective persp(0.1f, 100.0f, lbl_8055F39C);
    fn_802C744C(S32(p, 0x50), p + 0x54);
    fn_802D7554(0);
    fn_802C9A1C(&saved);
    fn_802C99E8(oldSpec);
    fn_802C9994(oldDiff);
    fn_802C9950(oldAmb);
}

extern const f32 lbl_80563B1C;  // 0.5

f32 fn_8025B0A8(f32* r) {
    f32 d = r[1] - r[0];
    f32 out = r[0];
    out += d * lbl_80563B1C;
    return out;
}

f32 fn_8025B0C4(f32 x) { return (f32)sin(x); }

void fn_8025B0E8(Words9* d, const Words9* s) { *d = *s; }

u8* fn_8025B134(u8* p) { return p; }

void fn_8025B138(u8* p) {
    s32 level = S32(p, 0x1C);
    S32(p, 0x74) = level;
    SetTrophy(p, S32(p, 0x70), level);
    fn_801345D8((char*)(p + 0x20), 0x50, "%s", AchievementsGetLevelModel(S32(p, 0x70), S32(p, 0x74)));
}

void fn_8025B22C(u8* p) {
    for (int i = 1; i <= 5; i++) {
        if (S32(p, 0x74) == i) {
            fn_80214AF0(i + 5);
        } else {
            fn_80214A9C(i + 5);
        }
        if (i <= S32(p, 0x1C)) {
            fn_80214AF0(i);
            fn_80214A9C(i + 10);
        } else {
            fn_80214AF0(i + 10);
            fn_80214A9C(i);
        }
    }
}

}  // extern "C"

extern "C" void fn_80216D28(void);
extern "C" void fn_80214D50(int s);

static inline int MaxI(int a, int b) { return a > b ? a : b; }
static inline int MinI(int a, int b) { return a < b ? a : b; }

// Moves the shown level by d within [1, unlocked]; a bump sound when it can't move.
static inline void StepLevel(u8* p, int d) {
    int old = S32(p, 0x74);
    S32(p, 0x74) = old + d;
    S32(p, 0x74) = MinI(MaxI(S32(p, 0x74), 1), S32(p, 0x1C));
    if (old != S32(p, 0x74)) {
        SetTrophy(p, S32(p, 0x70), S32(p, 0x74));
        fn_801345D8((char*)(p + 0x20), 0x50, "%s", AchievementsGetLevelModel(S32(p, 0x70), S32(p, 0x74)));
    } else {
        fn_80216D28();
        fn_80214D50(1);
    }
}

extern "C" {

int fn_8025B2BC(u8* p, int key) {
    switch (key) {
    case 0x128:
        StepLevel(p, -1);
        return 1;
    case 0x126:
        StepLevel(p, 1);
        return 1;
    }
    return 0;
}

}  // extern "C"

int AchievementsGetLevel(int category, s32 value);
s32 AchievementsGetLevelThreshold(int category, int level);

extern "C" {
void fn_80130FCC(char* buf, s32 size, s32 value);
extern f32 lbl_80558DC4[];  // FLT_EPSILON

// An achievement row: its value, level, next threshold and trophy model name.
void fn_8025B520(u8* p) {
    ((AchView*)p)->Refresh();
    U8(p, 0x58) = 1;
    U8(p, 0x59) = 0;
    S32(p, 0x5C) = -1000;
    if (U8(p, 0xC4)) {
        lbl_8055F394->SetSoundVolume("achieve_tally", 0.0f);
        lbl_8055F394->PlaySound("achieve_tally");
    }
    fn_80214A9C(0x10);
    fn_80214A9C(0xD);
    ((AchView*)p)->previousModelName.CopyFrom((const char*)(p + 0x60));
}

void fn_8025B784(u8* p) {}

// Per-frame row update: swaps in a new trophy, counts the value up towards its best (with the tally
// sound), then shows the level arrows and markers.
void fn_8025B788(u8* p) {
    AchView* v = (AchView*)p;
    if (strcasecmp(v->previousModelName, v->modelName) != 0 && v->modelName.text[0]) {
        fn_80236954("trophy_present");
        v->previousModelName.CopyFrom(v->modelName);
    }
    if (lbl_8055F3A0 && v->tally >= 0.8f) {
        f32 next = v->shown + v->tallyRate * fn_80214D48();
        f32 best = v->q.best;
        v->shown = best < next ? best : next;
    }
    u8 wasStarted = v->started;
    v->started = (s32)(v->shown + lbl_80558DC4[0]) < v->q.best;
    v->tally += fn_80214D48();
    if (v->started) {
        f32 x = lbl_8055F3A4;
        f32 vol = 1.0f < x ? 1.0f : x;
        lbl_8055F394->SetSoundVolume("achieve_tally", vol);
    } else if (wasStarted) {
        lbl_8055F394->StopSound("achieve_tally");
        if (v->counting && v->levelUp) {
            lbl_8055F394->PlaySound("achieve_trophy");
            fn_80214AF0(0x10);
            v->tally = 0.0f;
        }
    }
    if (U8(p, 0x59)) {
        fn_80236958("trophy_present");
        fn_802168BC(S32(p, 0x5C));
    }
    v->ShowValue();
    if (v->level >= AchievementsGetLevelCount(v->q.category)) {
        fn_80214AF0(0x15);
        fn_80214A9C(0x14);
    } else {
        fn_80214AF0(0x14);
        fn_80214A9C(0x15);
    }
    if (v->level == 0 || v->counting) {
        fn_80214A9C(0xF);
    } else {
        fn_80214AF0(0xF);
    }
    if (v->counting) {
        fn_80214A9C(0xE);
    } else {
        fn_80214AF0(0xE);
    }
    if (v->level < 2 || v->counting) {
        fn_80214A9C(0x11);
    } else {
        fn_80214AF0(0x11);
    }
    if (v->started || v->Flashing()) {
        fn_80214A9C(0xD);
    } else {
        fn_80214AF0(0xD);
    }
}

}  // extern "C"

// True while the tally animation (f_cc) still runs.
static inline bool Tallying(u8* p) { return F32(p, 0xCC) < 1.5f && U8(p, 0xC5); }

extern "C" {
void fn_80216CA8(void);

int fn_8025BBFC(u8* p, int key) {
    if (U8(p, 0x58) || Tallying(p)) {
        fn_80216D28();
    }
    switch (key) {
    case 0xD:
        if (U8(p, 0xC4)) {
            if (U8(p, 0x58)) {
                F32(p, 0x28) = (f32)S32(p, 0x20);
            } else if (!Tallying(p)) {
                U8(p, 0x59) = 1;
                S32(p, 0x5C) = -2;
            }
        } else if ((S32(p, 0xC8) > 1 || fn_8025921C()) && !Tallying(p)) {
            S32(p, 0x5C) = 3;
            U8(p, 0x59) = 1;
        } else {
            fn_80214D50(1);
            return 0;
        }
        break;
    case 0x1B:
        if (!U8(p, 0xC4)) {
            U8(p, 0x59) = 1;
            S32(p, 0x5C) = -1;
        } else {
            return 0;
        }
        break;
    case 0x126:
        if (!U8(p, 0xC4)) {
            U8(p, 0x59) = 1;
            S32(p, 0x5C) = 1;
            fn_80216CA8();
        } else {
            return 0;
        }
        break;
    case 0x128:
        if (!U8(p, 0xC4)) {
            U8(p, 0x59) = 1;
            S32(p, 0x5C) = 2;
            fn_80216CA8();
        } else {
            return 0;
        }
        break;
    default:
        return 0;
    }
    return 1;
}

void fn_8025BE0C(u8* p) {
    s32 level = S32(p, 0x24);
    SetTrophy(p, S32(p, 0x20), level);
}

void fn_8025BED0(u8* p) {
    u8* o = PTR(p, 0x1C);
    if (U8(o, 0xC5) && !U8(o, 0x58)) {
        s32 level = S32(o, 0xC8);
        S32(p, 0x24) = level;
        s32 cat = S32(p, 0x20) = S32(o, 0x18);
        SetTrophy(p, cat, level);
        U8(PTR(p, 0x18), 0x4A) = 1;
    }
}

}  // extern "C"

ModuleTag lbl_8055F388;
UnkStatic123s lbl_80498E98;
UnkStatic7F lbl_8055F390;
extern const f32 lbl_80563B38;
f32 lbl_8055F39C = lbl_80563B38;
