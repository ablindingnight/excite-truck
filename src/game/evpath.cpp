// Career event path screen (0x8027E908-0x8028777C): the career map ("EventPath:..."), unlock
// announcements ("unlocked_career", "CAREER_DIFFICULTY_%d_UNLOCKED", "CONGRATULATIONS"), the
// bracket/contest selection lists and the animation wait list ("add_anim_to_wait_for"). Written
// against raw field offsets; the large screen routines are still stubs. Names are guesses apart from
// the strings.
#include "game/common.h"
#include "game/menu.h"
#include "game/logomover.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

// The screen's image/font/sound set (shared UIObjects implementation, see uiobjs.cpp).
class UIObjects {
public:
    virtual ~UIObjects(void);
    virtual void AddImage(const char* name, const char* file, int frame);
    virtual void AddFont(const char* name, const char* file);
    virtual void AddSound(const char* name, const char* file, u8 loop);
    virtual void DrawImage(const char* name, f32 x, f32 y, int z, int frame);                // 0x18
    virtual void DrawImageFade(const char* name, f32 x, f32 y, f32 a, int z, int frame);     // 0x1C
    virtual void s20(void);
    virtual void s24(void);
    virtual const char* GetFile(const char* name);  // 0x28
    virtual int GetFrame(const char* name);         // 0x2C
    virtual void s30(void);
    virtual void s34(void);
    virtual void PlaySound(const char* name);  // 0x38
    virtual void StopSound(const char* name);  // 0x3C
};

class VR {
public:
    virtual ~VR(void);
    virtual const char* v0C(void);  // display name
    virtual const char* v10(void);  // id
    virtual void v14(void);
    virtual int v18(void);
    virtual u8* v1C(s32 x);
    virtual u8* v20(void);
    virtual int v24(void);
    virtual int v28(void);
    virtual void v2C(void);
    virtual void v30(int a);
    virtual void v34(void);
    virtual void v38(const char* s);
    virtual int v3C(void);
    virtual int v40(void);
    virtual void v44(void);
    virtual int v48(void);
    virtual int v4C(void);
};

#define VR(p) ((VR*)(p))

// A list with an inline buffer; the base destructor clears the header.
template <typename T>
class ArrayBase {
public:
    ~ArrayBase(void) {
        data = 0;
        count = 0;
        cap = 0;
    }
    int Count(void) const { return count; }
    void Add(T v) {
        int n = count++;
        data[n] = v;
    }
    template <typename V>
    int Find(V* v) {
        for (int i = 0; i < Count(); i++) {
            if ((V*)data[i] == v) {
                return i;
            }
        }
        return -1;
    }
    template <typename V>
    void Remove(V* v) {
        int i = Find(v);
        if (i == -1) {
            return;
        }
        if (i >= 0 && i < count) {
            int last = count - 1;
            if (i != last) {
                data[i] = data[last];
            }
            count--;
        }
    }
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
};

template <typename T, int N>
class FixedArray : public ArrayBase<T> {
public:
    FixedArray(void) {
        this->data = buf;
        this->cap = N;
        this->count = 0;
    }
    T buf[N];  // offset 0xC
};

const char* GetEventTypeName(u32 type);

extern "C" {
void fn_80236914(void);
void fn_80216D28(void);
void fn_802E30C8(const char* fmt, ...);
void fn_8025C204(u8* p, s32 a);
f32 fn_8023659C(void);
void fn_80214A9C(int s);
void fn_80214AF0(int s);
void fn_8027E908(void);
UIObjects* fn_80258354(void);
void fn_80286A34(u8* p);
void fn_80287238(u8* p);
}

void operator delete(void* p);


// Copies at most n characters, always terminating.
static inline void CopyName(int n, char* d, const char* s) {
    while ((*d++ = *s++) != 0) {
        if (--n <= 0) {
            d[-1] = 0;
            break;
        }
    }
}

struct AnimName {
    char name[30];
};

ModuleTag lbl_8055F670;
s32 lbl_8055F678;
u8 lbl_8055F67C;
UIObjects* lbl_8055F680;
bool lbl_8055F684;
u8 lbl_8055F685;
s32 lbl_8055F688;
s32 lbl_8055F68C;
s32 lbl_8055F690;
s32 lbl_8055F694;
u32 lbl_8055F698;
s32 lbl_8055F69C;
u8 lbl_8055F6A0;  // first showing of the cup screen: advance to the next cup

// The "cup unlocked" flash shown in the unlock message box (menu item type 5).
class EventUnlockFx : public MenuState {
public:
    EventUnlockFx(void) { start = 0.0f; }
    virtual void Enter(void);
    virtual void Update(void);
    virtual int Key(int key);
    f32 start;  // offset 0x18
    u8 done;    // offset 0x1C
};

static UnkStatic123s lbl_804B2FF8;            // this unit's end-of-menu tag
static EventUnlockFx lbl_804B3018;            // unlock flash widget
static FixedArray<u32, 5> lbl_804B3048;       // events of the current path
// Index of a cup bar in the current path's list (the bar is held as an s32 handle).
static inline int BarIndex(const s32& bar) { return lbl_804B3048.Find((u8*)bar); }
// Menu slot of an event: the bonus slot (5) or its place in the registry.
static inline int EventSlot(u8* e) { return U8(e, 0x64) ? 5 : lbl_804B3048.Find(e); }
static char lbl_804B3068[100];                // the selected cup's help text
static FixedArray<u8*, 5> lbl_804B30D8;       // the event browser's events
static char sBrowseTitle[32];                 // lbl_804B30D8 + 0x20
static char sBrowseDesc[32];                  // lbl_804B30D8 + 0x40
static char sBrowseBest[20];                  // lbl_804B30D8 + 0x60

// Career path interfaces (slots used here; see pathcareer.cpp).
class EvContest {
public:
    virtual ~EvContest(void) {}
    virtual const char* TrackName(void) = 0;  // 0xC
    virtual const char* Name(void) = 0;       // 0x10
    virtual const char* TypeName(void) = 0;   // 0x14
    virtual const char* TypeDisplayName(void) = 0;  // 0x18
    virtual const char* FriendlyName(void) = 0;     // 0x1C
    virtual int EventId(void) = 0;            // 0x20
    virtual int Difficulty(void) = 0;         // 0x24
    virtual int Best(void) const = 0;         // 0x28
    virtual int BestGrade(void) = 0;          // 0x2C
    virtual int GradeOf(int score) = 0;       // 0x30
    virtual int Threshold(int grade) = 0;     // 0x34
    virtual int Passing(void) = 0;            // 0x38
    virtual int PassScore(void) = 0;          // 0x3C
    virtual int Unlocked(void) = 0;           // 0x40: passed
    virtual int v44(void) = 0;
    virtual int Perfected(void) = 0;          // 0x48
    virtual int v4C(void) = 0;
};

class EvBracket {
public:
    virtual ~EvBracket(void) {}
    virtual const char* Name(void) = 0;    // 0xC
    virtual const char* Id(void) = 0;      // 0x10
    virtual int Difficulty(void) = 0;      // 0x14
    virtual int Count(void) = 0;           // 0x18
    virtual EvContest* Get(int i) = 0;     // 0x1C
    virtual int Unlocked(void) = 0;        // 0x20
    virtual int Open(void) = 0;            // 0x24
    virtual int AllDone(void) = 0;         // 0x28
    virtual int Gated(void) = 0;           // 0x2C
    virtual const char* Text(void) = 0;    // 0x30
};

class EvPath {
public:
    virtual ~EvPath(void);
    virtual int Count(void);           // 0xC
    virtual EvBracket* Get(int i);     // 0x10
};

namespace {

// A cup on the event path screen (vtable __vt__EvCupBar, 0xD0 bytes): eases into place, shows the
// cup's icon, its progress and a star per event. Every live marker is listed in lbl_804B3048.
class EvCupBar : public LogoMover {
public:
    EvCupBar(void) : LogoMover(0.25f) {
        cup = 0;
        still = 0;
        style = 0;
        progress[0] = 0;
        lbl_804B3048.Add((u32)this);
        title[0] = 0;
        frame = 0;
    }
    virtual ~EvCupBar(void) {
        if ((u32)lbl_8055F688 == (u32)this) {
            lbl_8055F688 = 0;
        }
        lbl_804B3048.Remove(this);
    }
    virtual void Enter(void);
    virtual void Update(void);
    virtual void Draw(void);

    // Shows bracket c; `still` skips the ease-in.
    void SetCup(EvBracket* c, u8 s) {
        cup = c;
        still = s;
        for (int i = 0; i < cup->Count(); i++) {
            fill[i] = cup->Get(i)->Unlocked() ? 1.0f : 0.0f;
        }
        perfect = cup->AllDone();
    }

    EvBracket* Cup(void) const { return cup; }

    EvBracket* cup;    // offset 0x60
    u8 still;          // offset 0x64: no ease-in
    f32 timer;         // offset 0x68
    s32 f6C;           // offset 0x6C
    s32 style;         // offset 0x70
    char progress[10]; // offset 0x74: "done/total"
    char title[50];    // offset 0x7E: icon file
    s32 frame;         // offset 0xB0: icon frame
    f32 fill[5];       // offset 0xB4: star fill per event
    u8 padC8[4];       // offset 0xC8
    u8 perfect;        // offset 0xCC
    u8 showStars;      // offset 0xCD
};

// An event of the bracket screen (vtable __vt__Q220@unnamed@evpath_cpp@10EvEventBar): eases into place and slides out while it is the
// current one; shows "best/pass" and the best grade and plays the passed / perfected / grade-up
// animations when they change. Every live bar is listed in lbl_804B30D8.
class EvEventBar : public LogoMover {
public:
    EvEventBar(void);
    virtual ~EvEventBar(void);
    void Reset(void);
    virtual void Enter(void);
    virtual void Update(void);

    EvContest* contest;  // offset 0x60
    f32 slide;           // offset 0x64: 0 in place, 1 slid out
    s32 baseX;           // offset 0x68
    char text[30];       // offset 0x6C: "best/pass"
    char grade[10];      // offset 0x8A: best grade's name
    s32 current;         // offset 0x94
    u8 passed;           // offset 0x98
    u8 perfected;        // offset 0x99
    s32 unlocked;        // offset 0x9C
};

}  // namespace

// An event's unlock condition (object at event + 0x60).
class EvCond {
public:
    virtual void v08(void);
    virtual const char* Title(void);  // 0x0C
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(void);
    virtual const char* Desc(void);   // 0x1C
    virtual s32 EventId(void);        // 0x20
    virtual int Difficulty(void);     // 0x24
    virtual void v28(void);
    virtual int Done(void);          // 0x2C
    virtual const char* Name(void);  // 0x30
};

// Makes event i current (none when out of range) and copies its caption.
static inline void SelectEvent(int i) {
    lbl_804B3068[0] = 0;
    if (i >= 0 && i < lbl_804B3048.count) {
        u8* e = (u8*)lbl_804B3048.data[i];
        lbl_8055F688 = (s32)e;
        if (e && ((EvCupBar*)e)->Cup()) {
            CopyName(100, lbl_804B3068, ((EvCond*)PTR(e, 0x60))->Name());
        }
    } else {
        lbl_8055F688 = 0;
    }
}

extern "C" {

// Loads the screen's images, fonts and sounds.
void fn_8027E908(void) {
    lbl_8055F680 = fn_80258354();
    lbl_8055F680->AddImage("meter_unlockable", "ep_ticker.art", 4);
    lbl_8055F680->AddImage("meter_minor", "ep_ticker.art", 3);
    lbl_8055F680->AddImage("meter_major", "ep_ticker.art", 1);
    lbl_8055F680->AddImage("meter_tick", "ep_ticker.art", 2);
    lbl_8055F680->AddImage("meter_finger", "ep_ticker.art", 4);
    lbl_8055F680->AddImage(GetEventTypeName(0x180201), "event_icons.art", 1);
    lbl_8055F680->AddImage(GetEventTypeName(0x180302), "event_icons.art", 2);
    lbl_8055F680->AddImage("Time Attack", "event_icons.art", 1);
    lbl_8055F680->AddImage("bronze", "ep_cupicon1.art", 0);
    lbl_8055F680->AddImage("silver", "ep_cupicon1.art", 1);
    lbl_8055F680->AddImage("gold", "ep_cupicon1.art", 2);
    lbl_8055F680->AddImage("platinum", "ep_cupicon1.art", 3);
    lbl_8055F680->AddImage("diamond", "ep_cupicon1.art", 4);
    lbl_8055F680->AddImage("cup_star", "ep_cupicon1.art", 5);
    lbl_8055F680->AddImage("star_small_empty", "star_ab.art", 1);
    lbl_8055F680->AddImage("star_small_full", "star_ab.art", 0);
    lbl_8055F680->AddImage("silver_unlock", "cup_unlk1.art", 0);
    lbl_8055F680->AddImage("gold_unlock", "cup_unlk1.art", 1);
    lbl_8055F680->AddImage("platinum_unlock", "cup_unlk2.art", 0);
    lbl_8055F680->AddImage("diamond_unlock", "cup_unlk2.art", 1);
    lbl_8055F680->AddImage("nocup_unlock", "bar5.art", 6);
    lbl_8055F680->AddFont("normal", "uifnt.gtx");
    lbl_8055F680->AddFont("outline", "uifnt_o.gtx");
    lbl_8055F680->AddSound("scoreup_tally", "a_tally.sfx", 1);
    lbl_8055F680->AddSound("scoreup_gradeup", "a_grade1.sfx", 0);
    lbl_8055F680->AddSound("scoreup_AAA", "a_grade2.sfx", 0);
    lbl_8055F680->AddSound("scoreup_passevent", "a_starfill.sfx", 0);
    lbl_8055F680->AddSound("scoreup_perfectevent", "a_starfill.sfx", 0);
    lbl_8055F680->AddSound("passed_cup", "a_cheerhit.sfx", 0);
    lbl_8055F680->AddSound("passed_career", "a_grade1.sfx", 0);
    lbl_8055F680->AddSound("unlocked_career", "a_levelup.sfx", 0);
}

void fn_8027ED2C(void) {
    delete lbl_8055F680;
    lbl_8055F680 = 0;
}

UIObjects* fn_8027ED70(void) { return lbl_8055F680; }

}  // extern "C"

extern char lbl_8055B860[8];

extern "C" {

// Shows the "event unlocked" message box.
void fn_8027ED78(const char* title, const char* message) {
    MenuItem items[] = {
        MenuItem("#title", title),
        MenuItem("#message", message),
        MenuItem(lbl_8055B860, 6, 0, -2, 13),
        MenuItem(lbl_804B2FF8),
    };
    MenuDesc desc = {"ep_unlock.lyt", 0, 0, 0, 0};
    desc.items = items;
    fn_802146BC(&desc, -999, -999, 1);
}

void fn_8027EE74(int x) {
    lbl_8055F684 = x == 2;
    fn_80236914();
}

int fn_8027EE88(void) {
    if (!lbl_8055F685 && lbl_8055F684) {
        lbl_8055F685 = 1;
        fn_8027ED70()->PlaySound("unlocked_career");
    }
    return 0;
}

}  // extern "C"

extern char lbl_8055BAB8[4];  // ""
extern char lbl_8055BAC0[4];  // ""
extern "C" const char* fn_80131234(const char* key);
extern "C" int fn_801345D8(char* buf, int n, const char* fmt, ...);

extern "C" {

// Per-difficulty (excite, super, mirror) names and art.
static inline const char* NewModeLayout(int d) {
    const char* names[3] = {"newmode1.lyt", "newmode2.lyt", "newmode3.lyt"};
    return names[d];
}

static inline const char* EventTitle(int d) {
    const char* names[3] = {"EventPath:EXCITE_EVENT", "EventPath:SUPER_EVENT", "EventPath:MIRROR_EVENT"};
    return names[d];
}

static inline const char* CupTitle(int d) {
    const char* names[3] = {"EventPath:EXCITE_CUP", "EventPath:SUPER_CUP", "EventPath:MIRROR_CUP"};
    return names[d];
}

static inline const char* BarBottom(int d) {
    const char* names[3] = {"bar4_bot_e.art", "bar4_bot_s.art", "bar4_bot_m.art"};
    return names[d];
}

static inline const char* BarTop(int d) {
    const char* names[3] = {"bar4_top_e.art", "bar4_top_s.art", "bar4_top_m.art"};
    return names[d];
}

// "Career difficulty N unlocked" box; the layout depends on the new difficulty.
void fn_8027EEE4(int level) {
    char buf[100];
    fn_801345D8(buf, 100, "EventPath:CAREER_DIFFICULTY_%d_UNLOCKED", level);
    MenuItem items[] = {
        MenuItem(0, 5, &lbl_804B3018, 0, 0),
        MenuItem("title", fn_80131234("EventPath:CONGRATULATIONS")),
        MenuItem("message", fn_80131234(buf)),
        MenuItem(lbl_8055B860, 6, 0, -2, 13),
        MenuItem(lbl_8055BAB8, 8, (const void*)fn_8027EE74, 2, 0),
        MenuItem(lbl_804B2FF8),
    };
    MenuDesc desc = {NewModeLayout(level - 1), 0, (int (*)(int*))fn_8027EE88, 0, 0};
    desc.items = items;
    lbl_8055F684 = 0;
    lbl_8055F685 = 0;
    fn_802146BC(&desc, -999, -999, 1);
}

}  // extern "C"


class EvPath;
class EvBracket;
struct EvResult;

extern "C" {

// "Career difficulty N unlocked" message box.
void fn_8027F09C(int difficulty) {
    char buf[100];
    fn_801345D8(buf, 100, "EventPath:CAREER_DIFFICULTY_%d_UNLOCK_INSTRUCTIONS", difficulty);
    MenuItem items[] = {
        MenuItem(0, 5, &lbl_804B3018, 0, 0),
        MenuItem("title", fn_80131234("EventPath:CHALLENGE")),
        MenuItem("message", fn_80131234(buf)),
        MenuItem(lbl_8055B860, 6, 0, -2, 13),
        MenuItem(lbl_804B2FF8),
    };
    MenuDesc desc = {"unlock_crr_chal.lyt", 0, 0, 0, 0};
    desc.items = items;
    fn_8027ED70()->PlaySound("passed_career");
    fn_802146BC(&desc, -999, -999, 1);
}

s32 lbl_8055C7E0 = -1;  // the browsed event's id (trackmap), -1 for none

int fn_802E499C(u16* keys);

// Key sequences on the path screens (left/right twice; left, left, right, right). Unused.
static inline int TapLeftRight(void) {
    u16 keys[5] = {0x125, 0x127, 0x125, 0x127, 0};
    return fn_802E499C(keys);
}

static inline int TapLeftLeftRightRight(void) {
    u16 keys[5] = {0x125, 0x125, 0x127, 0x127, 0};
    return fn_802E499C(keys);
}

void add_anim_to_wait_for(const char* name) {
    u8* l = (u8*)lbl_8055F69C;
    if (!l) {
        return;
    }
    ArrayBase<AnimName>* anims = (ArrayBase<AnimName>*)(l + 0x40);
    if (anims->Count() + 1 <= anims->cap) {
        CopyName(30, anims->data[anims->count++].name, name);
        return;
    }
    fn_802E30C8("add_anim_to_wait_for: can't add, there are %d already!", anims->Count());
}

s32 fn_802168E4(void);

// "Cup completed" box: congratulations and the next cup (or what this one unlocked), with the cup's
// marker easing in.
void fn_8027F288(EvBracket* cup, EvBracket* next) {
    char msg[100];
    if (next) {
        fn_801345D8(msg, 100, "EventPath:%s_COMPLETED", cup->Id());
    } else if (cup && cup->AllDone()) {
        fn_801345D8(msg, 100, "EventPath:%s_COMPLETED_NONEXT_PERFECTED", cup->Id());
    } else {
        fn_801345D8(msg, 100, "EventPath:%s_COMPLETED_NONEXT_PASSED", cup->Id());
    }
    s32 shown = fn_802168E4();
    if (shown) {
        fn_80214A9C(0x1F);
    }
    EvCupBar bar;
    if (next) {
        bar.SetCup(next, 1);
    }
    char unlock[100] = "nocup_unlock";
    if (next) {
        fn_801345D8(unlock, 100, "%s_unlock", next->Id());
    } else {
        fn_801345D8(unlock, 100, "%s_unlock", cup->Id());
    }
    const char* file = fn_8027ED70()->GetFile(unlock);
    int frame = fn_8027ED70()->GetFrame(unlock);
    MenuItem items[] = {
        MenuItem(0, 5, &lbl_804B3018, 0, 0),
        MenuItem("bg_image", 3, file, 0, 0),
        MenuItem("bg_image", 2, &frame, 0, 0),
        MenuItem("path", 5, &bar, 0, 0),
        MenuItem("cup_name", 3, bar.cup ? bar.cup->Name() : "", 0, 0),
        MenuItem("cup_icon", 3, bar.title, 0, 0),
        MenuItem("cup_icon", 2, &bar.frame, 0, 0),
        MenuItem("title", fn_80131234("EventPath:CONGRATULATIONS")),
        MenuItem("message", fn_80131234(msg)),
        MenuItem(lbl_8055B860, 6, 0, -2, 13),
        MenuItem(lbl_804B2FF8),
    };
    MenuDesc desc = {"ep_completecup.lyt", 0, 0, 0, 0};
    desc.items = items;
    lbl_8055F680->PlaySound("passed_cup");
    fn_802146BC(&desc, -999, -999, 1);
    if (shown) {
        fn_80214AF0(0x1F);
    }
}

int fn_8024739C(int* item);
int fn_80280898(void);
int fn_802808D0(void);
int fn_80280A38(void);
int fn_80280C00(void);
int fn_80280DC8(void);
int fn_80280F7C(void);
void fn_80281138(void);

// A path's cup screen: a marker per cup (at most five) with its name and progress, coloured and titled for
// the path's difficulty. Returns 1 with the picked cup in *sel, 0 when backed out.
int fn_8027F958(s32* sel, EvPath* path) {
    int diff = path->Get(0)->Difficulty();
    EvCupBar bars[5];
    int count = path->Count();
    int n = count < 5 ? count : 5;
    for (int i = 0; i < n; i++) {
        bars[i].SetCup(path->Get(i), 0);
    }
    MenuItem items[] = {
        MenuItem(lbl_8055BAC0, 8, (const void*)fn_80281138, 0, 0),
        MenuItem("cup_help", 3, lbl_804B3068, 0, 0),
        MenuItem("path_1", 5, &bars[0], 0, 0),
        MenuItem("cup_bar_1", 2, &bars[0].style, 0, 0),
        MenuItem("cup_name_1", 3, bars[0].cup ? bars[0].cup->Name() : "", 0, 0),
        MenuItem("cup_events_passed_1", 3, bars[0].progress, 0, 0),
        MenuItem("path_2", 5, &bars[1], 0, 0),
        MenuItem("cup_bar_2", 2, &bars[1].style, 0, 0),
        MenuItem("cup_name_2", 3, bars[1].cup ? bars[1].cup->Name() : "", 0, 0),
        MenuItem("cup_events_passed_2", 3, bars[1].progress, 0, 0),
        MenuItem("path_3", 5, &bars[2], 0, 0),
        MenuItem("cup_bar_3", 2, &bars[2].style, 0, 0),
        MenuItem("cup_name_3", 3, bars[2].cup ? bars[2].cup->Name() : "", 0, 0),
        MenuItem("cup_events_passed_3", 3, bars[2].progress, 0, 0),
        MenuItem("path_4", 5, &bars[3], 0, 0),
        MenuItem("cup_bar_4", 2, &bars[3].style, 0, 0),
        MenuItem("cup_name_4", 3, bars[3].cup ? bars[3].cup->Name() : "", 0, 0),
        MenuItem("cup_events_passed_4", 3, bars[3].progress, 0, 0),
        MenuItem("path_5", 5, &bars[4], 0, 0),
        MenuItem("cup_bar_5", 2, &bars[4].style, 0, 0),
        MenuItem("cup_name_5", 3, bars[4].cup ? bars[4].cup->Name() : "", 0, 0),
        MenuItem("cup_events_passed_5", 3, bars[4].progress, 0, 0),
        MenuItem("#bar4_top_color", 3, BarTop(diff), 0, 0),
        MenuItem("#bar4_bot_color", 3, BarBottom(diff), 0, 0),
        MenuItem("#screen_title", fn_80131234(CupTitle(diff))),
        MenuItem(lbl_8055B860, 6, (const void*)fn_80280F7C, 0, 0x125),
        MenuItem(lbl_8055B860, 6, (const void*)fn_80280DC8, 0, 0x127),
        MenuItem(lbl_8055B860, 6, (const void*)fn_80280C00, 0, 0x128),
        MenuItem(lbl_8055B860, 6, (const void*)fn_80280A38, 0, 0x126),
        MenuItem(lbl_8055B860, 6, (const void*)fn_802808D0, -2, 13),
        MenuItem(lbl_8055B860, 6, (const void*)fn_80280898, -1, 0x1B),
        MenuItem(lbl_8055BAB8, 8, (const void*)fn_8027EE74, 2, 0),
        MenuItem(lbl_804B2FF8),
    };
    MenuDesc desc = {"ep_chcup.lyt", 0, (int (*)(int*))fn_8024739C, 0, 0};
    desc.items = items;
    if (lbl_8055F6A0) {
        lbl_8055F6A0 = 0;
        SelectEvent(*sel);
        if (*sel < path->Count() - 1) {
            (*sel)++;
            lbl_8055F694 = (s32)&bars[*sel];
        }
    } else {
        SelectEvent(*sel);
    }
    int picked = fn_802146BC(&desc, -999, -999, 1) == -2;
    if (picked) {
        *sel = lbl_804B3048.Find((u8*)lbl_8055F688);
    }
    lbl_8055F688 = 0;
    lbl_804B3048.count = 0;
    lbl_8055F68C = 0;
    lbl_8055F690 = 0;
    lbl_804B3068[0] = 0;
    lbl_8055F694 = 0;
    return picked;
}

int fn_80280898(void) {
    if (lbl_8055F688 == 0) {
        fn_80216D28();
        return 0;
    }
    return 1;
}

}  // extern "C"

int IsUnlockAll(void);

extern "C" {
void fn_80236954(const char* name);
void fn_80214D50(int s);

// Moves the cursor to the current event's slot; returns whether it is selectable.
int fn_802808D0(void) {
    int i = lbl_804B3048.Find((u8*)lbl_8055F688);
    if (i >= 0) {
        int ok;
        u8* e = (u8*)lbl_804B3048.data[i];
        ok = 0;
        if (((EvCupBar*)e)->Cup() && (IsUnlockAll() || ((VR*)PTR(e, 0x60))->v24())) {
            ok = 1;
        }
        if (ok) {
            int idx = EventSlot((u8*)lbl_8055F688);
            char buf[0x1E];
            fn_801345D8(buf, 0x1E, "select_%d", idx + 1);
            fn_80236954(buf);
            return 1;
        }
        fn_80216D28();
        fn_80214D50(1);
        return 0;
    }
    fn_80216D28();
    return 0;
}

}  // extern "C"



extern "C" {

// When the current event is done, picks the first not-yet-done event as the next unlock.
int fn_80280A38(void) {
    u8* cur = (u8*)lbl_8055F688;
    if (cur && ((EvCupBar*)cur)->Cup() && ((EvCond*)PTR(cur, 0x60))->Done()) {
        lbl_8055F690 = lbl_8055F688;
        if (!lbl_8055F68C) {
            for (int i = 0; i < lbl_804B3048.count; i++) {
                EvCupBar* e = (EvCupBar*)lbl_804B3048.data[i];
                if (e->cup && !((EvCond*)e->cup)->Done()) {
                    lbl_8055F68C = (s32)e;
                    break;
                }
            }
        }
        if (lbl_8055F68C) {
            int i = BarIndex(lbl_8055F68C);
            lbl_804B3068[0] = 0;
            if (i >= 0 && i < lbl_804B3048.count) {
                u8* e = (u8*)lbl_804B3048.data[i];
                lbl_8055F688 = (s32)e;
                if (e && ((EvCupBar*)e)->Cup()) {
                    CopyName(100, lbl_804B3068, ((EvCond*)PTR(e, 0x60))->Name());
                }
            } else {
                lbl_8055F688 = 0;
            }
        }
    }
    return 0;
}

}  // extern "C"


extern "C" {
void fn_80216D28(void);

// While the current event is not done, picks the first done event as the one to show.
int fn_80280C00(void) {
    u8* cur = (u8*)lbl_8055F688;
    if (cur && ((EvCupBar*)cur)->Cup() && !((EvCond*)PTR(cur, 0x60))->Done()) {
        lbl_8055F68C = lbl_8055F688;
        if (!lbl_8055F690) {
            for (int i = 0; i < lbl_804B3048.count; i++) {
                EvCupBar* e = (EvCupBar*)lbl_804B3048.data[i];
                if (e->cup && ((EvCond*)e->cup)->Done()) {
                    lbl_8055F690 = (s32)e;
                    break;
                }
            }
        }
        if (lbl_8055F690) {
            SelectEvent(BarIndex(lbl_8055F690));
        }
    }
    return 0;
}

// Next event in the same state (done/not done) as the current one.
int fn_80280DC8(void) {
    if (lbl_8055F688) {
        int i = BarIndex(lbl_8055F688);
        u8* cur = (u8*)lbl_804B3048.data[i];
        if (((EvCupBar*)cur)->Cup()) {
            u32 done = ((EvCond*)PTR(cur, 0x60))->Done();
            int k;
            for (k = i + 1; k < lbl_804B3048.count; k++) {
                u8* e = (u8*)lbl_804B3048.data[k];
                EvCond* c = (EvCond*)((EvCupBar*)e)->Cup();
                if (c && c->Done() == done) {
                    break;
                }
            }
            if (k < lbl_804B3048.count) {
                SelectEvent(k);
            }
        }
    } else {
        fn_80216D28();
    }
    return 0;
}

// Previous event in the same state as the current one.
int fn_80280F7C(void) {
    if (lbl_8055F688) {
        int i = BarIndex(lbl_8055F688);
        u8* cur = (u8*)lbl_804B3048.data[i];
        if (((EvCupBar*)cur)->Cup()) {
            u32 done = ((EvCond*)PTR(cur, 0x60))->Done();
            int k;
            for (k = i - 1; k >= 0; k--) {
                u8* e = (u8*)lbl_804B3048.data[k];
                EvCond* c = (EvCond*)((EvCupBar*)e)->Cup();
                if (c && c->Done() == done) {
                    break;
                }
            }
            if (k >= 0) {
                SelectEvent(k);
            }
        }
    } else {
        fn_80216D28();
    }
    return 0;
}

int fn_802368EC(void);


// Plays the "back to slot" animation, then the unlock animation for the newly opened event.
void fn_80281138(void) {
    if (lbl_804B3048.Find((u8*)lbl_8055F688) >= 0 && fn_802368EC()) {
        char buf[0x1E];
        fn_801345D8(buf, 0x1E, "back_select_%d", EventSlot((u8*)lbl_8055F688) + 1);
        fn_80236954(buf);
    }
    u8* next = (u8*)lbl_8055F694;
    if (next) {
        lbl_8055F688 = 0;
        char buf[0x1E];
        fn_801345D8(buf, 0x1E, "unlock_%d", EventSlot(next) + 1);
        fn_80236954(buf);
    }
}

}  // extern "C"

// What a finished event changed.
struct EvResult {
    u8 improved;    // offset 0x0
    s32 delta;      // offset 0x4
    s32 event;      // offset 0x8
    u8 newly[7];    // offset 0xC: [4] tier, [5] path done, [6] career done
};

class Statistic {
public:
    int HasValue(void);
    u8 pad[0x24];
};

class RealStatistic : public Statistic {
public:
    RealStatistic(int slot, u32 type, long event, int difficulty);
    f32 GetValue(void);
};

int IdentGetCurrentSlot(void);

extern "C" void fn_80130980(char* buf, s32 size, int fields, f32 secs);  // format a time

// Makes event bar i current (none when out of range or without a condition) and shows its title,
// description and best time.
static inline void SelectBar(int i) {
    if (i >= 0 && i < lbl_804B30D8.Count() && PTR((u8*)lbl_804B30D8.data[i], 0x60)) {
        lbl_8055F698 = (u32)lbl_804B30D8.data[i];
    } else {
        lbl_8055F698 = 0;
    }
    if (i >= 0 && i < lbl_804B30D8.Count() && PTR((u8*)lbl_804B30D8.data[i], 0x60)) {
        u8* e = (u8*)lbl_804B30D8.data[i];
        if (e) {
            CopyName(32, sBrowseTitle, ((EvCond*)PTR(e, 0x60))->Title());
            CopyName(32, sBrowseDesc, ((EvCond*)PTR(e, 0x60))->Desc());
            int difficulty = ((EvCond*)PTR(e, 0x60))->Difficulty();
            s32 event = ((EvCond*)PTR(e, 0x60))->EventId();
            RealStatistic best(IdentGetCurrentSlot(), 0x20507, event, difficulty);
            if (best.HasValue()) {
                fn_80130980(sBrowseBest, 20, 3, best.GetValue());
            } else {
                CopyName(20, sBrowseBest, fn_80131234("EventPath:NO_BEST_TIME"));
            }
            lbl_8055C7E0 = ((EvCond*)PTR(e, 0x60))->EventId();
        } else {
            sBrowseDesc[0] = 0;
            sBrowseTitle[0] = 0;
            lbl_8055C7E0 = -1;
        }
    } else {
        sBrowseDesc[0] = 0;
        sBrowseTitle[0] = 0;
        lbl_8055C7E0 = -1;
    }
}

extern "C" void fn_801308F4(char* buf, int n, int value);  // format a score
const char* GetGradeDisplayName(int grade);


// Copies s to d, at most n bytes in all, always terminating.
static inline void CopyTail(const char* s, int n, char* d) {
    while ((*d++ = *s++) != 0) {
        if (--n <= 0) {
            d[-1] = 0;
            break;
        }
    }
}

// Appends s to the string in d (n bytes in all), always terminating.
static inline void AppendName(const char* s, int n, char* d) {
    while (*d) {
        n--;
        d++;
    }
    if (n > 0) {
        CopyTail(s, n, d);
    }
}

// The score being counted up (object at ScoreAccumulator + 0x20).
struct ScoreTally {
    s32 from;   // offset 0x0
    s32 to;     // offset 0x4
    f32 rate;   // offset 0x8: points per second
    u8 unlocks; // offset 0xC: announce unlocks passed on the way
    f32 acc;    // offset 0x10: counted so far

    ScoreTally(int from, int to) {
        this->from = from;
        this->to = to;
        unlocks = 0;
        rate = 30.0f;
    }
    ~ScoreTally(void) {}

    // The current value, at most the target.
    s32 Value(void) const {
        s32 v = (s32)((f32)from + acc);
        s32 m = to;
        return v < m ? v : m;
    }
};


namespace {

// The finished event's score while it counts up (vtable __vt__ScoreAccumulator; menu item type 5 through its
// MenuState part at +4). It stands in for the event on the bracket screen, reporting the counted value
// as the best; queued animations (add_anim_to_wait_for) are waited for before the follow-up boxes.
class ScoreAccumulator : public EvContest, public MenuState {
public:
    ScoreAccumulator(void) {
        lbl_8055F69C = (s32)this;
        inner = 0;
        tally = 0;
        sel = -1;
        next = -1;
        showCup = 0;
        nextCup = 0;
        cup = 0;
        showUnlock = 0;
        unlockLevel = 0;
        quit = 0;
    }
    virtual ~ScoreAccumulator(void) {
        lbl_8055F69C = 0;
        delete tally;
    }

    virtual const char* TrackName(void) { return inner->TrackName(); }
    virtual const char* Name(void) { return inner->Name(); }
    virtual const char* TypeName(void) { return inner->TypeName(); }
    virtual const char* TypeDisplayName(void) { return inner->TypeDisplayName(); }
    virtual const char* FriendlyName(void) { return inner->FriendlyName(); }
    virtual int EventId(void) { return inner->EventId(); }
    virtual int Difficulty(void) { return inner->Difficulty(); }
    virtual int Best(void) const;
    virtual int BestGrade(void) { return GradeOf(Best()); }
    virtual int GradeOf(int score) { return inner->GradeOf(score); }
    virtual int Threshold(int grade) { return inner->Threshold(grade); }
    virtual int Passing(void) { return inner->Passing(); }
    virtual int PassScore(void) { return inner->PassScore(); }
    virtual int Unlocked(void) {
        int a = PassScore();
        return a <= Best();
    }
    virtual int v44(void) { return inner->v44(); }
    virtual int Perfected(void) {
        int a = v4C();
        return a <= Best();
    }
    virtual int v4C(void) { return inner->v4C(); }
    virtual void Enter(void);
    virtual void Update(void);
    virtual int Key(int key);

    EvContest* inner;          // offset 0x1C
    ScoreTally* tally;         // offset 0x20
    s32 sel;                   // offset 0x24
    u8 perfect;                // offset 0x28
    u8 counted;                // offset 0x29
    u8 shown;                  // offset 0x2A
    u8 showCup;                // offset 0x2B: then the cup-completed box
    EvBracket* cup;            // offset 0x2C
    EvBracket* nextCup;        // offset 0x30
    u8 showUnlock;             // offset 0x34: then the difficulty-unlocked box
    s32 unlockLevel;           // offset 0x38
    u8 quit;                   // offset 0x3C: then leave the screen
    FixedArray<AnimName, 10> anims;  // offset 0x40
    f32 wait;                  // offset 0x178
    s32 next;                  // offset 0x17C: event to select afterwards
};

// The bar's caption without animations: "best/pass", whether it is passed, its best grade.
inline void EvEventBar::Reset(void) {
    if (contest) {
        fn_801308F4(text, 30, contest->Best());
        AppendName("/", 30, text);
        char pass[20];
        fn_801308F4(pass, 20, contest->PassScore());
        AppendName(pass + 1, 30, text);
        if (contest->Unlocked()) {
            unlocked = 1;
        } else {
            unlocked = 0;
        }
        int g = contest->BestGrade();
        if (g >= 0 && g < 5) {
            CopyName(10, grade, GetGradeDisplayName(g));
        }
        current = (u8*)this == (u8*)lbl_8055F698;
    } else {
        text[0] = 0;
        grade[0] = 0;
        current = 0;
        passed = 0;
        perfected = 0;
    }
}

inline EvEventBar::EvEventBar(void) : LogoMover(0.2f) {
    m.ease = 1;
    contest = 0;
    lbl_804B30D8.Add((u8*)this);
    Reset();
}

inline EvEventBar::~EvEventBar(void) {
    if ((u32)lbl_8055F698 == (u32)this) {
        lbl_8055F698 = 0;
    }
    lbl_804B30D8.Remove(this);
}

// The bracket as the event screen shows it (vtable __vt__Q220@unnamed@evpath_cpp@11BracketView): event `index` is replaced by the score
// accumulator while it counts.
class BracketView : public EvBracket {
public:
    BracketView(EvBracket* b, EvContest* e, int i) {
        br = b;
        stand = e;
        index = i;
    }

    virtual const char* Name(void) { return br->Name(); }
    virtual const char* Id(void) { return br->Id(); }
    virtual int Difficulty(void) { return br->Difficulty(); }
    virtual int Count(void) { return br->Count(); }
    virtual EvContest* Get(int i) {
        EvContest* r = stand;
        if (r && i == index) {
            return r;
        }
        return br->Get(i);
    }
    virtual int Unlocked(void) {
        for (int i = 0; i < Count(); i++) {
            if (!Get(i)->Unlocked()) {
                return 0;
            }
        }
        return 1;
    }
    virtual int Open(void) { return br->Open(); }
    virtual int AllDone(void) {
        for (int i = 0; i < Count(); i++) {
            if (!Get(i)->Perfected()) {
                return 0;
            }
        }
        return 1;
    }
    virtual int Gated(void) { return br->Gated(); }
    virtual const char* Text(void) { return br->Text(); }

    EvBracket* br;     // offset 0x4
    EvContest* stand;  // offset 0x8
    s32 index;         // offset 0xC
};

}  // namespace

// A set of named textures, one of which is drawn at a time (gart.cpp).
class DynoTex {
public:
    DynoTex(int max, u8 flip);
    virtual ~DynoTex(void);
    void Add(int id, const char* name);

    void* entries;  // offset 0x4
    int max;        // offset 0x8
    int count;      // offset 0xC
    s32 current;    // offset 0x10
    u8 flip;        // offset 0x14
};

extern "C" int fn_802054EC(const char* key, int level, int event);

class ValueNodeV {
public:
    virtual const char* GetName(void);
    virtual int Has(const char* key);
    virtual int GetPair(int index, const char** key, const char** value);
    virtual const char* GetValue(const char* key);
};

struct EventDesc {
    u8 pad[0x20];
    ValueNodeV* table;  // offset 0x20
};

void* GetEvent(int event);

// s without a leading `prefix`.
static inline const char* SkipPrefix(const char* s, const char* prefix) {
    if (!strncmp(s, prefix, strlen(prefix))) {
        s += strlen(prefix);
    }
    return s;
}
void* operator new(unsigned long n, const char* tag);

extern "C" {
int fn_802839D0(void);
int fn_80283A50(void);
int fn_80283AC8(void);
int fn_80283B58(void);
int fn_80283E28(void);

// The bracket's event screen: a bar per event (at most five) with best/pass and grade, the bracket's cup
// marker, the selected event's title, best time and map; after a result the improved event's score counts
// up first. Returns 1 when an event was picked (its index in *event).
int fn_802812B4(s32* event, EvBracket* br, EvResult* res, EvPath* path) {
    int diff = br->Difficulty();
    ScoreAccumulator acc;
    if (res->newly[2]) {
        int i;
        for (i = 0; i < path->Count(); i++) {
            if (path->Get(i) == br) {
                break;
            }
        }
        EvBracket* next = i < path->Count() - 1 ? path->Get(i + 1) : 0;
        acc.cup = br;
        acc.nextCup = next;
        acc.showCup = 1;
        acc.quit = 1;
        lbl_8055F6A0 = next != 0;
    }
    if (res->newly[4]) {
        acc.quit = 1;
    }
    if (res->newly[5]) {
        acc.quit = 1;
    }
    EvEventBar bars[5];
    int count = br->Count();
    int n = count < 5 ? count : 5;
    DynoTex maps(n, fn_802054EC("mirror_mode", br->Difficulty(), -1));
    int start = -1;
    int cur = *event;
    if (res->improved) {
        int done;
        if (br->Unlocked()) {
            done = br->Get(cur)->Perfected();
        } else {
            done = br->Get(cur)->Unlocked();
        }
        if (done && cur < n - 1) {
            cur++;
        }
    }
    for (int i = 0; i < n; i++) {
        EvContest* c = br->Get(i);
        if (res->improved && c->EventId() == res->event) {
            int delta = res->delta;
            start = i;
            acc.inner = c;
            acc.sel = i;
            if (delta > 0) {
                int best = c->Best();
                ScoreTally* t = new ("ScoreAccumulator") ScoreTally(best - delta, best);
                acc.tally = t;
                t->acc = 0.0f;
            }
            acc.next = cur;
            bars[i].contest = &acc;
            bars[i].Reset();
        } else {
            bars[i].contest = c;
            bars[i].Reset();
        }
        char file[33];
        fn_801345D8(file, 33, "/%s.tex", SkipPrefix(((EventDesc*)GetEvent(c->EventId()))->table->GetValue("name"), "career_"));
        maps.Add(c->EventId(), file);
    }
    BracketView view(br, res->improved ? &acc : 0, start);
    EvCupBar cup;
    cup.SetCup(&view, 1);
    MenuItem items[] = {
        MenuItem(0, 5, (MenuState*)&acc, 0, 0),
        MenuItem("path_1", 5, &cup, 0, 0),
        MenuItem("cup_name_1", 3, cup.cup ? cup.cup->Name() : "", 0, 0),
        MenuItem("cup_events_passed_1", 3, cup.progress, 0, 0),
        MenuItem("cup_icon", 3, cup.title, 0, 0),
        MenuItem("cup_icon", 2, &cup.frame, 0, 0),
        MenuItem("#course_name", 3, sBrowseDesc, 0, 0),
        MenuItem("#best_time", 3, sBrowseBest, 0, 0),
        MenuItem("#trackmap", 0x12, &maps, 0, 0),
        MenuItem("#trackmap", 2, &lbl_8055C7E0, 0, 0),
        MenuItem("event_1", 5, &bars[0], 0, 0),
        MenuItem("event_bar_1", 2, &bars[0].current, 0, 0),
        MenuItem("event_name_1", 3, bars[0].contest ? bars[0].contest->TrackName() : "", 0, 0),
        MenuItem("event_points_1", 3, bars[0].text, 0, 0),
        MenuItem("event_grade_1", 3, bars[0].grade, 0, 0),
        MenuItem("event_grade_1", 2, &bars[0].unlocked, 0, 0),
        MenuItem("event_2", 5, &bars[1], 0, 0),
        MenuItem("event_bar_2", 2, &bars[1].current, 0, 0),
        MenuItem("event_name_2", 3, bars[1].contest ? bars[1].contest->TrackName() : "", 0, 0),
        MenuItem("event_points_2", 3, bars[1].text, 0, 0),
        MenuItem("event_grade_2", 3, bars[1].grade, 0, 0),
        MenuItem("event_grade_2", 2, &bars[1].unlocked, 0, 0),
        MenuItem("event_3", 5, &bars[2], 0, 0),
        MenuItem("event_bar_3", 2, &bars[2].current, 0, 0),
        MenuItem("event_name_3", 3, bars[2].contest ? bars[2].contest->TrackName() : "", 0, 0),
        MenuItem("event_points_3", 3, bars[2].text, 0, 0),
        MenuItem("event_grade_3", 3, bars[2].grade, 0, 0),
        MenuItem("event_grade_3", 2, &bars[2].unlocked, 0, 0),
        MenuItem("event_4", 5, &bars[3], 0, 0),
        MenuItem("event_bar_4", 2, &bars[3].current, 0, 0),
        MenuItem("event_name_4", 3, bars[3].contest ? bars[3].contest->TrackName() : "", 0, 0),
        MenuItem("event_points_4", 3, bars[3].text, 0, 0),
        MenuItem("event_grade_4", 3, bars[3].grade, 0, 0),
        MenuItem("event_grade_4", 2, &bars[3].unlocked, 0, 0),
        MenuItem("event_5", 5, &bars[4], 0, 0),
        MenuItem("event_bar_5", 2, &bars[4].current, 0, 0),
        MenuItem("event_name_5", 3, bars[4].contest ? bars[4].contest->TrackName() : "", 0, 0),
        MenuItem("event_points_5", 3, bars[4].text, 0, 0),
        MenuItem("event_grade_5", 3, bars[4].grade, 0, 0),
        MenuItem("event_grade_5", 2, &bars[4].unlocked, 0, 0),
        MenuItem("#bar4_top_color", 3, BarTop(diff), 0, 0),
        MenuItem("#bar4_bot_color", 3, BarBottom(diff), 0, 0),
        MenuItem("#screen_title", fn_80131234(EventTitle(diff))),
        MenuItem(lbl_8055B860, 6, (const void*)fn_80283E28, 0, 0x126),
        MenuItem(lbl_8055B860, 6, (const void*)fn_80283B58, 0, 0x128),
        MenuItem(lbl_8055B860, 6, (const void*)fn_80283AC8, -2, 13),
        MenuItem(lbl_8055B860, 6, (const void*)fn_80283A50, -1, 0x1B),
        MenuItem(lbl_8055B860, 6, (const void*)fn_802839D0, 0, 0x121),
        MenuItem(lbl_8055BAB8, 8, (const void*)fn_8027EE74, 2, 0),
        MenuItem(lbl_804B2FF8),
    };
    MenuDesc desc = {0, 0, (int (*)(int*))fn_8024739C, 0, 0};
    desc.layout = "ep_chevent.lyt";
    desc.items = items;
    if (res->improved) {
        SelectBar(*event);
    } else {
        SelectBar(cur);
    }
    int picked = fn_802146BC(&desc, -999, -999, 1) == -2;
    *event = lbl_804B30D8.Find((u8*)lbl_8055F698);
    lbl_8055F698 = 0;
    lbl_804B30D8.count = 0;
    sBrowseTitle[0] = 0;
    sBrowseDesc[0] = 0;
    sBrowseBest[0] = 0;
    lbl_8055C7E0 = -1;
    return picked;
}

int fn_802839D0(void) {
    if (lbl_8055F684 && lbl_8055F698) {
        s32 a = VR(PTR((u8*)lbl_8055F698, 0x60))->v24();
        fn_8025C204(VR(PTR((u8*)lbl_8055F698, 0x60))->v20(), a);
    } else {
        fn_80216D28();
    }
    return 0;
}

int fn_80283A50(void) {
    if (lbl_804B30D8.Find((u8*)lbl_8055F698) >= 0) {
        return 1;
    }
    fn_80216D28();
    return 0;
}

int fn_80283AC8(void) {
    int i = lbl_804B30D8.Find((u8*)lbl_8055F698);
    if (i >= 0 && PTR((u8*)lbl_804B30D8.data[i], 0x60)) {
        return 1;
    }
    fn_80216D28();
    return 0;
}

}  // extern "C"


extern "C" {

// Selects the next event that has an unlock condition and shows its title, description and best time.
int fn_80283B58(void) {
    int cur = lbl_804B30D8.Find((u8*)lbl_8055F698);
    if (cur >= 0) {
        int i = cur;
        do {
            if (++i >= lbl_804B30D8.Count()) {
                i = 0;
            }
        } while (!PTR((u8*)lbl_804B30D8.data[i], 0x60) && i != cur);
        SelectBar(i);
    } else {
        fn_80216D28();
    }
    return 0;
}

// Selects the previous event that has an unlock condition and shows its title, description and best time.
int fn_80283E28(void) {
    int cur = lbl_804B30D8.Find((u8*)lbl_8055F698);
    if (cur >= 0) {
        int i = cur;
        do {
            if (--i < 0) {
                i += lbl_804B30D8.Count();
            }
        } while (!PTR((u8*)lbl_804B30D8.data[i], 0x60) && i != cur);
        SelectBar(i);
    } else {
        fn_80216D28();
    }
    return 0;
}

void fn_802840F4(u8* p) { fn_8027E908(); }

void fn_802840F8(void) { fn_8027ED2C(); }

}  // extern "C"

struct Unlockable {
    const char* type;  // offset 0x0
    const char* name;  // offset 0x4
    s32 points;        // offset 0x8
};

int GetUnlockableCount(void);
Unlockable* GetUnlockable(int i);

class VehicleList {
public:
    virtual ~VehicleList(void);                                    // 0x8
    virtual void Add(const char* name, int unlocked, int special);  // 0xC
    virtual void SetReason(int i, const char* reason);             // 0x10
    virtual int Count(void);                                       // 0x14
};

// Extra vehicles an event offers on top of the unlockables.
class ExtraVehicles {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual int Count(void);            // 0x14
    virtual const char* Name(int i);    // 0x18
    virtual void v1C(void);
    virtual void v20(void);
    virtual void v24(void);
    virtual const char* Reason(int i);  // 0x28
};



const char* GetCareerCompletedChassis(void);
const char* ShowCareerCompletedReason(void);

extern "C" {
VehicleList* fn_8025007C(int n);
void fn_80263F30(int lastTier, int finalTier, int a);
int fn_8027F958(s32* bracket, EvPath* path);
int fn_802812B4(s32* event, EvBracket* br, EvResult* res, EvPath* path);
void fn_80284580(int lo, int hi, ExtraVehicles* extra, u32 arg);

// The career event browser: starts on the selected event (or the first one not yet unlocked), then lets the
// player pick brackets and events; plays the tier/path/career rewards the last result earned. Returns 1 with the
// chosen event in *sel, 0 when backed out.
int fn_802840FC(s32* sel, EvPath* path, const EvResult* res) {
    lbl_8055F67C = 0;
    EvResult r = *res;
    int fresh = *sel == -1;
    s32 b = -1;
    s32 e = -1;
    if (fresh) {
        for (int i = 0; i < path->Count(); i++) {
            EvBracket* br = path->Get(i);
            if (!br->Unlocked()) {
                b = i;
                for (int j = 0; j < br->Count(); j++) {
                    if (!br->Get(j)->Unlocked()) {
                        e = j;
                        break;
                    }
                }
                break;
            }
        }
        if (b < 0) {
            b = 0;
        }
        if (e < 0) {
            e = 0;
        }
    } else {
        for (int i = 0; i < path->Count(); i++) {
            EvBracket* br = path->Get(i);
            for (int j = 0; j < br->Count(); j++) {
                if (*sel == br->Get(j)->EventId()) {
                    b = i;
                    e = j;
                }
            }
        }
    }
    int ok = 0;
    s32 prev = b;
    for (int skip = !fresh; skip || fn_8027F958(&b, path); skip = 0) {
        EvBracket* br = path->Get(b);
        if (b != prev) {
            e = 0;
        }
        prev = b;
        int go = fn_802812B4(&e, br, &r, path);
        if (r.newly[5]) {
            fn_8027EEE4(br->Difficulty() + 1);
        }
        if (r.newly[6]) {
            VehicleList* list = fn_8025007C(1);
            list->Add(GetCareerCompletedChassis(), 1, 0);
            list->SetReason(0, ShowCareerCompletedReason());
            fn_80284580(0, 0, (ExtraVehicles*)list, 0);
            delete list;
        }
        if ((r.newly[4] && br->Difficulty() == 1) || (r.newly[5] && br->Difficulty() == 2)) {
            int last = !r.newly[6];
            fn_80263F30(last, br->Difficulty() == 2, 1);
        }
        if (r.newly[5]) {
            return 0;
        }
        if (r.newly[4] && br->Difficulty() < 2) {
            fn_8027F09C(br->Difficulty() + 1);
        }
        memset(&r, 0, sizeof(r));
        if (go) {
            *sel = br->Get(e)->EventId();
            ok = 1;
            break;
        }
    }
    return ok;
}

}  // extern "C"

extern "C" {
int strcasecmp(const char* a, const char* b);
VehicleList* fn_8025007C(int n);
void fn_8024FBD8(VehicleList* list, u32 arg);

// Shows the cars unlocked between two point totals (plus the event's extra vehicles).
void fn_80284580(int lo, int hi, ExtraVehicles* extra, u32 arg) {
    VehicleList* list = fn_8025007C(GetUnlockableCount() + (extra ? extra->Count() : 0));
    if (extra) {
        for (int i = 0; i < extra->Count(); i++) {
            list->Add(extra->Name(i), 1, 0);
            list->SetReason(list->Count() - 1, extra->Reason(i));
        }
    }
    for (int i = 0; i < GetUnlockableCount(); i++) {
        Unlockable* u = GetUnlockable(i);
        if (!strcasecmp(u->type, "CAR") && lo < u->points && hi >= u->points) {
            list->Add(u->name, 1, 0);
        }
    }
    if (list->Count() > 0) {
        fn_8024FBD8(list, arg);
    }
    delete list;
}

int fn_8023695C(const char* name);

// Event screen setup: progress text ("%d/%d" of the cup's parts) and the event's title.
}  // extern "C"

inline void EvCupBar::Enter(void) {
    u8* p = (u8*)this;
    if (!U8(p, 0x64)) {
        U8(p, 0x18) = 1;
        S32(p, 0x1C) = fn_80221EA8(p);
    }
    if (PTR(p, 0x60)) {
        S32(p, 0x20) = 0;
        U8(p, 0x50) = 1;
        s32 y;
        s32 x = S32(p, 0x4);
        y = S32(p, 0x8);
        S32(p, 0x48) = x;
        S32(p, 0x4C) = y;
        S32(p, 0x6C) = x;
        F32(p, 0x68) = 0.0f;
        if (PTR(p, 0x60)) {
            if (VR(PTR(p, 0x60))->v28() && !U8(p, 0xCC)) {
                fn_80236954("cup_perfected");
                add_anim_to_wait_for("cup_perfected");
            }
            int show = 0;
            S32(p, 0x70) = p == (u8*)lbl_8055F688 ? 6 : 7;
            if (VR(PTR(p, 0x60))->v24()) {
                if (!VR(PTR(p, 0x60))->v28() || fn_8023695C("cup_perfected")) {
                    show = 1;
                }
            }
            U8(p, 0xCD) = show;
            if (show) {
                int i;
                int done;
                int n = VR(PTR(p, 0x60))->v18();
                done = 0;
                for (i = 0; i < n; i++) {
                    if (VR(VR(PTR(p, 0x60))->v1C(i))->v40()) {
                        done++;
                    }
                }
                fn_801345D8((char*)p + 0x74, 10, "%d/%d", done, n);
            } else {
                U8(p, 0x74) = 0;
            }
        }
        const char* title = fn_8027ED70()->GetFile(VR(PTR(p, 0x60))->v10());
        if (title) {
            CopyName(50, (char*)p + 0x7E, title);
        }
        S32(p, 0xB0) = fn_8027ED70()->GetFrame(VR(PTR(p, 0x60))->v10());
    } else {
        S32(p, 0x20) = 1;
    }
}

extern "C" {

void fn_80236958(const char* name);

static inline f32 Clamp01F(f32 v) {
    f32 t = 1.0f < v ? 1.0f : v;
    return t > 0.0f ? t : 0.0f;
}

// Menu slot of a cup marker: the bonus slot (5) or its place in the registry.
static inline int CupSlot(EvCupBar* b) { return b->still ? 5 : lbl_804B3048.Find(b); }

// Can the event be picked: it has a bracket that is open (or everything is unlocked).
static inline int Selectable(EvCupBar* e) { return e->Cup() && (IsUnlockAll() || e->cup->Open()); }

// Refreshes the cup's caption: the "perfected" animation once, its style (6 when current), its "done/total"
// progress when it is open, and its icon.
static inline void RefreshCup(EvCupBar* b) {
    if (b->cup) {
        if (b->cup->AllDone() && !b->perfect) {
            fn_80236954("cup_perfected");
            add_anim_to_wait_for("cup_perfected");
        }
        b->style = (u8*)b == (u8*)lbl_8055F688 ? 6 : 7;
        int show = b->cup->Open() && (!b->cup->AllDone() || fn_8023695C("cup_perfected"));
        b->showStars = show;
        if (show) {
            int n = b->cup->Count();
            int done = 0;
            for (int i = 0; i < n; i++) {
                if (b->cup->Get(i)->Unlocked()) {
                    done++;
                }
            }
            fn_801345D8(b->progress, 10, "%d/%d", done, n);
        } else {
            b->progress[0] = 0;
        }
    }
    const char* title = fn_8027ED70()->GetFile(b->cup->Id());
    if (title) {
        CopyName(50, b->title, title);
    }
    b->frame = fn_8027ED70()->GetFrame(b->cup->Id());
}

// The cup marker's frame: motion (unless still), caption, its menu items' visibility, the stars easing
// toward done/not done, the highlight fade, and the end of its unlock animation.
}  // extern "C"

inline void EvCupBar::Update(void) {
    u8* p = (u8*)this;
    EvCupBar* b = (EvCupBar*)p;
    if (!b->still) {
        b->LogoMover::Update();
    }
    if (b->cup) {
        RefreshCup(b);
        if (b->cup->Open()) {
            fn_80214AF0(CupSlot(b) + 11);
        } else {
            fn_80214A9C(CupSlot(b) + 11);
        }
        if (b->perfect) {
            fn_80214AF0(CupSlot(b) + 21);
        } else {
            fn_80214A9C(CupSlot(b) + 21);
        }
        b->perfect = b->cup->AllDone();
        f32 step = fn_80214D48() / 0.5f;
        for (int i = 0; i < b->cup->Count(); i++) {
            f32* f = &b->fill[i];
            *f = Clamp01F(*f + (b->cup->Get(i)->Unlocked() ? step : -step));
        }
        f32 fade = fn_80214D48() / 0.1f;
        if ((u8*)b == (u8*)lbl_8055F688) {
            f32 t = b->timer + fade;
            b->timer = 1.0f < t ? 1.0f : t;
        } else {
            f32 t = b->timer - fade;
            b->timer = 0.0f > t ? 0.0f : t;
        }
        if ((u8*)b == (u8*)lbl_8055F694) {
            char buf[30];
            fn_801345D8(buf, 30, "unlock_%d", CupSlot(b) + 1);
            if (fn_8023695C(buf) == 2) {
                fn_80236958(buf);
                lbl_8055F694 = 0;
                SelectEvent(lbl_804B3048.Find(b));
            }
        }
    } else {
        fn_80214A9C(CupSlot(b) + 11);
        fn_80214A9C(CupSlot(b) + 21);
        fn_80214A9C(CupSlot(b) + 1);
    }
    if (CupSlot(b) == 0) {
        EvCupBar* cur = (EvCupBar*)lbl_8055F688;
        if (cur && Selectable(cur)) {
            fn_80214AF0(0x1F);
        } else {
            fn_80214A9C(0x1F);
        }
    }
}


extern f32 lbl_80558DC4[];  // FLT_EPSILON
extern "C" double cos(double x);
extern "C" double sin(double x);

extern "C" {
void fn_80230C30(f32* x, f32* y, s32 lx, s32 ly);

// The fill level of each of an event icon's stars.
// Draws the event's stars on an arc under its icon: full, empty, or empty with the fill fading in.
}  // extern "C"

inline void EvCupBar::Draw(void) {
    if (!cup || !showStars) {
        return;
    }
    f32 x;
    f32 y;
    fn_80230C30(&x, &y, this->x, this->y);
    f32 cx = x + 0.5f * w;
    f32 cy = y - 0.5f * h;
    f32 radius = 0.3f * w;
    f32 step = cup->Count() == 4 ? 0.6981317f : 0.61086524f;
    f32 start = 4.712389f;  // 3pi/2: straight down
    for (int i = 0; i < cup->Count(); i++) {
        f32 a = start + step * (i - 0.5f * (cup->Count() - 1));
        x = cx + radius * (f32)cos(a);
        y = cy + radius * (f32)sin(a);
        if (cup->Get(i)->Unlocked()) {
            if (!((f32)__fabs(fill[i] - 1.0f) <= lbl_80558DC4[0])) {
                fn_8027ED70()->DrawImage("star_small_empty", x, y, 20, -1);
                fn_8027ED70()->DrawImageFade("star_small_full", x, y, 2.0f - fill[i], 20, -1);
            } else {
                fn_8027ED70()->DrawImage("star_small_full", x, y, 20, -1);
            }
        } else {
            fn_8027ED70()->DrawImage("star_small_empty", x, y, 20, -1);
        }
    }
}

extern "C" {


// Plays the bar's animation `<anim><slot>` and waits for it.
static inline void PlayBarAnim(EvEventBar* b, const char* anim) {
    char buf[100];
    fn_801345D8(buf, 100, "%s%d", anim, lbl_804B30D8.Find(b) + 1);
    fn_80236954(buf);
    add_anim_to_wait_for(buf);
}

// Refreshes the bar: "best/pass", the passed / perfected / grade-up animations, the grade.
static inline void RefreshEvent(EvEventBar* b) {
    if (b->contest) {
        fn_801308F4(b->text, 30, b->contest->Best());
        AppendName("/", 30, b->text);
        char pass[20];
        fn_801308F4(pass, 20, b->contest->PassScore());
        AppendName(pass + 1, 30, b->text);
        if (!b->passed && b->contest->Unlocked()) {
            fn_8027ED70()->PlaySound("scoreup_passevent");
            PlayBarAnim(b, "event_passed_");
        }
        if (!b->perfected && b->contest->Perfected()) {
            fn_8027ED70()->PlaySound("scoreup_perfectevent");
            PlayBarAnim(b, "event_perfected_");
        }
        if (b->contest->Unlocked()) {
            b->unlocked = 1;
        } else {
            b->unlocked = 0;
        }
        int grade = b->contest->BestGrade();
        if (grade >= 0 && grade < 5) {
            const char* name = GetGradeDisplayName(grade);
            if (strcasecmp(name, b->grade) != 0) {
                if (grade == 0) {
                    fn_8027ED70()->PlaySound("scoreup_AAA");
                    PlayBarAnim(b, "gradeup_S_");
                } else {
                    fn_8027ED70()->PlaySound("scoreup_gradeup");
                    PlayBarAnim(b, "gradeup_");
                }
            }
            CopyName(10, b->grade, name);
        }
        b->current = (u8*)b == (u8*)lbl_8055F698;
    } else {
        b->text[0] = 0;
        b->grade[0] = 0;
        b->current = 0;
        b->passed = 0;
        b->perfected = 0;
    }
}

// Event bar setup: jumps into place and takes the event's passed/perfected state as already seen.
}  // extern "C"

inline void EvEventBar::Enter(void) {
    u8* p = (u8*)this;
    EvEventBar* b = (EvEventBar*)p;
    b->m.active = 1;
    b->m.idx = fn_80221EA8((u8*)b);
    if (b->contest) {
        b->m.vis = 0;
        b->m.jumpOn = 1;
        s32 y;
        s32 x = b->x;
        y = b->y;
        b->m.jumpX = x;
        b->m.jumpY = y;
        b->baseX = x;
        b->slide = 0.0f;
        b->passed = b->contest->Unlocked();
        b->perfected = b->contest->Perfected();
        RefreshEvent(b);
    } else {
        b->m.vis = 1;
    }
}

extern "C" {

// Event bar frame: motion, refresh, slides out while current (its menu items follow), state markers.
}  // extern "C"

inline void EvEventBar::Update(void) {
    u8* p = (u8*)this;
    EvEventBar* b = (EvEventBar*)p;
    b->LogoMover::Update();
    if (b->contest) {
        RefreshEvent(b);
        int was = (int)(-27.0f * b->slide);
        f32 d = fn_80214D48() / 0.1f;
        b->slide = Clamp01F(b->slide + ((u8*)b == (u8*)lbl_8055F698 ? d : -d));
        int dx = (int)(-27.0f * b->slide);
        s32 y = b->y;
        s32 baseX = b->baseX;
        b->m.jumpX = baseX + dx;
        b->m.jumpY = y;
        b->m.jumpOn = 1;
        fn_80214BE4(lbl_804B30D8.Find(b) + 11, dx - was, 0);
        fn_80214BE4(lbl_804B30D8.Find(b) + 21, dx - was, 0);
        if (b->passed) {
            fn_80214AF0(lbl_804B30D8.Find(b) + 11);
        } else {
            fn_80214A9C(lbl_804B30D8.Find(b) + 11);
        }
        if (b->perfected) {
            fn_80214AF0(lbl_804B30D8.Find(b) + 21);
        } else {
            fn_80214A9C(lbl_804B30D8.Find(b) + 21);
        }
        b->passed = b->contest->Unlocked();
        b->perfected = b->contest->Perfected();
    } else {
        fn_80214A9C(lbl_804B30D8.Find(b) + 11);
        fn_80214A9C(lbl_804B30D8.Find(b) + 21);
    }
}


inline int ScoreAccumulator::Best(void) const {
    const u8* p = (const u8*)this;
    u8* o = PTR(p, 0x20);
    if (o) {
        s32 v = (s32)((f32)S32(o, 0x0) + F32(o, 0x10));
        s32 m = S32(o, 0x4);
        return v < m ? v : m;
    }
    return VR(PTR(p, 0x1C))->v28();
}


inline int ScoreAccumulator::Key(int key) {
    u8* p = (u8*)this;
    switch (key) {
    case 0xD: {
        u8* o = PTR(p, 0x20);
        if (o && !U8(p, 0x29)) {
            F32(o, 0x10) = (f32)(S32(o, 0x4) - S32(o, 0x0));
            return 1;
        }
        break;
    }
    }
    return 0;
}

extern "C" {


// While any of the queued animations still plays (or for a moment after), waits.
static inline int AnimsBusy(ScoreAccumulator* a) {
    for (int i = 0; i < a->anims.count; i++) {
        if (fn_8023695C(a->anims.data[i].name)) {
            a->wait = 1.0f;
            return 1;
        }
    }
    f32 t = a->wait - fn_80214D48();
    a->wait = 0.0f > t ? 0.0f : t;
    return a->wait > lbl_80558DC4[0];
}

void fn_802168BC(int a);
void fn_8027ED78(const char* title, const char* msg);

// Score tally frame: counts up (stopping the tally sound at the target), waits for the queued animations,
// then selects the event and shows what the result opened (cup done, difficulty unlocked, career done);
// unlockables whose points the count passes pop up as it goes.
}  // extern "C"

inline void ScoreAccumulator::Update(void) {
    ScoreTally* t = tally;
    if (!t) {
        return;
    }
    if (!counted) {
        s32 v = t->Value();
        if (v == t->to) {
            counted = 1;
            fn_8027ED70()->StopSound("scoreup_tally");
        }
    } else if (!shown && !AnimsBusy(this)) {
        fn_80214AF0(0x1F);
        if (next >= 0) {
            SelectBar(next);
        } else {
            SelectBar(sel);
        }
        shown = 1;
        if (showCup) {
            fn_8027F288(cup, nextCup);
        }
        if (showUnlock) {
            fn_8027F09C(unlockLevel);
        }
        if (quit) {
            fn_802168BC(-1);
        }
    }
    if (lbl_8055F684) {
        f32 dt = fn_80214D48();
        t = tally;
        s32 lo = t->Value();
        if (lo < t->to) {
            t->acc += t->rate * dt;
            s32 hi = t->Value();
            if (t->unlocks) {
                for (int i = 0; i < GetUnlockableCount(); i++) {
                    Unlockable* u = GetUnlockable(i);
                    if (lo < u->points && hi >= u->points) {
                        char title[20];
                        char msg[200];
                        if (!strcasecmp(u->type, "CAR")) {
                            CopyName(20, title, fn_80131234("EventPath:UNLOCK_TITLE"));
                            CopyName(200, msg, u->name);
                        } else if (!strcasecmp(u->type, "RANK")) {
                            CopyName(20, title, fn_80131234("EventPath:RANK_UP_TITLE"));
                            CopyName(200, msg, u->name);
                        }
                        fn_8027ED78(title, msg);
                    }
                }
            }
        }
    }
}


inline void ScoreAccumulator::Enter(void) {
    u8* p = (u8*)this;
    u8* o = PTR(p, 0x20);
    if (o) {
        F32(o, 0x10) = 0.0f;
        U8(p, 0x29) = 0;
        U8(p, 0x2A) = 0;
        U8(p, 0x28) = VR(p)->v40();
        fn_8027ED70()->PlaySound("scoreup_tally");
        fn_80214A9C(0x1F);
        F32(p, 0x178) = 1.0f;
    } else {
        U8(p, 0x29) = 1;
        U8(p, 0x2A) = 0;
        F32(p, 0x178) = 0.0f;
    }
}

extern "C" {

void fn_80287360(u8* p) {}

}  // extern "C"

inline void EventUnlockFx::Enter(void) {
    start = fn_8023659C();
    done = 0;
    fn_80214A9C(0x1D);
}

inline void EventUnlockFx::Update(void) {
    f32 t = fn_8023659C();
    if (t < start || t - start > 1.5f) {
        done = 1;
    }
    if (done) {
        fn_80214AF0(0x1D);
    }
}

inline int EventUnlockFx::Key(int key) { return done == 0; }

extern "C" {


}  // extern "C"
