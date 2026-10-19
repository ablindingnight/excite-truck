// Stunt race ("betarace", 0x80149588-0x801561E8): BetaMaster is the GameMode behind the normal
// excite races - per-car scores and stunts, coins and star pickups, the time limit, the place bonus
// read from place_bonus.val, the HUD layouts (betalyt*.val), the pre-race overlay and the results
// screen with its finish animation. Names are guesses apart from the allocation tags, keys and
// messages.
#include <math.h>
#include "game/common.h"

#define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))

extern "C" {
void* memset(void* d, int c, size_t n);
int fn_801345D8(char* buf, u32 size, const char* fmt, ...);  // snprintf
void fn_802E30C8(const char* fmt, ...);                      // debug print
void fn_802E317C(const char* fmt, ...);                      // warning print
const char* fn_80131234(const char* key);                    // localize
const char* fn_80130FE0(s32 value);                          // ordinal suffix
void fn_80130980(char* buf, s32 size, int fields, f32 secs);  // format a time
void fn_801308F4(char* buf, s32 size, s32 value);             // format a score
void fn_801308F8(char* buf, s32 size, s32 value);
int fn_80179650(void);
int fn_801609C4(s32 score);  // grade for a score
}

// The results-screen art animation player.
class ArtAnimMan {
public:
    ArtAnimMan(int n);
    u8 pad0[0x8];
};

extern "C" {
void fn_802AE168(ArtAnimMan* anim);
void fn_802ADFF4(ArtAnimMan* anim, s32 arg);
void fn_802AE02C(ArtAnimMan* anim, s32 arg);
void fn_802AE0B8(ArtAnimMan* anim, s32 arg);
void fn_802AE0E0(ArtAnimMan* anim);
void fn_802ADE60(ArtAnimMan* anim, int arg);
}

const char* GetGradeDisplayName(int grade);

extern s32 lbl_8055E3C4;  // frame counter
extern f32 lbl_8055E3C8;  // race clock

ModuleTag lbl_8055E228;
UnkStaticFFFF lbl_8055E230;
static UnkStatic123 lbl_8042FC70;
UnkStatic7F lbl_8055E234;

struct TextElem;
extern "C" void fn_801474EC(TextElem* e, const char* fmt, ...);

// A loaded sound effect; playback flags are polled by the sound system.
class Sound {
public:
    virtual ~Sound(void);
    void Play(void) {
        play = 1;
        b4B = 0;
        b4A = 0;
    }

    f32 pitch;  // offset 0x4
    u8 pad8[0x19 - 0x8];
    u8 active;  // offset 0x19
    u8 pad1A[0x49 - 0x1A];
    u8 play;  // offset 0x49
    u8 b4A;   // offset 0x4A
    u8 b4B;   // offset 0x4B
    u8 b4C;   // offset 0x4C
};

static inline void StopSound(Sound* s) {
    if (s) {
        s->b4A = 1;
        s->b4C = 0;
    }
}

template <typename T>
static inline void SafeDelete(T*& p) {
    if (p) {
        delete p;
        p = 0;
    }
}

static inline void StrCopyN(char* d, int n, const char* s) {
    char* p = d;
    int k = n;
    const char* q = s;
    while ((*p++ = *q++) != 0) {
        if (--k <= 0) {
            p[-1] = 0;
            break;
        }
    }
}

// A fixed-size string; new ones read "zzz" until they are filled in.
template <int N>
struct FixedStr {
    FixedStr(void) {
        memset(s, 'z', 3);
        s[3] = 0;
    }
    void Set(const char* t) { StrCopyN(s, N, t); }
    char s[N];
};

static inline f32 Clamp01(f32 x) {
    f32 r = 1.0f < x ? 1.0f : x;
    return r > 0.0f ? r : 0.0f;
}

static inline f32 Ramp(f32 x, f32 lo, f32 hi) { return Clamp01((x - lo) / (hi - lo)); }

static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

struct V2 {
    f32 x;
    f32 y;
};

// Layout elements (see ovlayout.cpp).
struct ElemBase {
    ElemBase(void) {}
    const char* name;  // offset 0x0
    V2 pos;            // offset 0x4
    s32 elemJust;      // offset 0xC
};

struct BoxElem : ElemBase {
    f32 w;  // offset 0x10
    f32 h;  // offset 0x14
};

struct TextElem : ElemBase {
    f32 scale;            // offset 0x10
    u32 just;             // offset 0x14
    u32 color;            // offset 0x18
    const char* defText;  // offset 0x1C
};

struct ArtElem : BoxElem {
    f32 u0;  // offset 0x18
    f32 v0;  // offset 0x1C
    f32 u1;  // offset 0x20
    f32 v1;  // offset 0x24
};

struct GartElem : BoxElem {
    u32 color;  // offset 0x18
};

class ILayout {
public:
    virtual ~ILayout(void) {}
    virtual TextElem* Find(const char* name) = 0;
    virtual ArtElem* FindArt(const char* name) = 0;
    virtual GartElem* FindBox(const char* name) = 0;
    virtual int FindSet(const char* name) = 0;
    virtual void Edit(void) = 0;
};

extern "C" {
void fn_801476C4(TextElem* e);
void fn_801476D8(const ArtElem* e, u32 color, s32 tex);
void fn_80147230(TextElem* e, const char* fmt, ...);
void fn_801473F8(ILayout* l, const char* name, const char* fmt, ...);
void fn_8014784C(ILayout* l, const char* box, const char* set, int part);
void fn_80147940(ILayout* l, GartElem* b, const char* set, int part);
void fn_801479A8(const TextElem* a, const TextElem* b, const char* fmt, f32 t, ...);
ILayout* fn_80145E70(const char* name);
}

// The base of every race mode; the defaults are shared weak copies.
class GameMode {
public:
    virtual ~GameMode(void) {}
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void) {}
    virtual void vf18(void) {}
    virtual int vf1C(int car) { return vf20(car); }
    virtual int vf20(int car) = 0;
    virtual int vf24(void);
    virtual void vf28(void);
    virtual void vf2C(void);
    virtual int vf30(void) { return 0; }
    virtual int vf34(void) { return 0; }
    virtual f32 vf38(void);
    virtual int vf3C(int p);
    virtual void vf40(void) {}
    virtual void vf44(void);
    virtual void vf48(void) {}
    virtual void vf4C(int p);
    virtual void vf50(int p);
    virtual int vf54(void);
    virtual int vf58(void);
    virtual int vf5C(int p);
    virtual void vf60(int view);
    virtual int vf64(void);
    virtual int vf68(void);
    virtual void vf6C(void) {}
    virtual void vf70(void) {}
    virtual void vf74(void) {}
    virtual void vf78(void) {}
    virtual int vf7C(void) { return 0; }
    virtual int vf80(void) { return 0; }
    virtual int vf84(void) { return 0; }
    virtual int vf88(void) { return 0; }
    virtual int vf8C(int p, int key);
    virtual void vf90(struct RaceCarRec* a, struct RaceCarRec* b, void* p, const f32* hit) {}
    virtual void vf94(struct RaceCarRec* rc) {}
    virtual void vf98(void);
    virtual void vf9C(struct RaceCarRec* rc, int type) {}
    virtual int vfA0(void* cam);
    virtual void vfA4(struct RaceCarRec* rc) {}
    virtual void vfA8(int car);
    virtual void vfAC(int car);
    virtual int vfB0(void) { return 0; }
    virtual void vfB4(void) {}
    virtual void vfB8(void* r) = 0;
    virtual int vfBC(void) { return 0; }
    virtual void vfC0(void);
    virtual void vfC4(void);
    virtual void vfC8(void) {}
    virtual int vfCC(void) { return 1; }
    virtual int vfD0(void) { return 0; }
    virtual int vfD4(int car);
    virtual int vfD8(void) { return 0; }
    virtual int vfDC(void) { return 1; }
    virtual int vfE0(void) { return 0; }
    virtual int vfE4(void) { return 0; }
    virtual void* vfE8(void);
    virtual int vfEC(void);
    virtual int vfF0(void) { return 0; }
    virtual void vfF4(void) {}
    virtual void vfF8(void);
    virtual void vfFC(void);

    void FillResults(void* r);  // 0x801B8A7C
};

struct ResultEntry {
    u8 pad0[0x10];
    s32 score;  // offset 0x10
};

struct RaceResults {
    u8 pad0[0x8];
    ResultEntry e[8];  // offset 0x8
};

extern s32 lbl_8055E4D0;  // cars in the race

// Race event listeners BetaMaster registers itself with.
class IRaceListener {
public:
    virtual void a0(void) = 0;
    virtual void a1(void) = 0;
    virtual void a2(void) = 0;
    virtual void a3(void) = 0;
    virtual void a4(void) = 0;
};

class IStuntListener {
public:
    virtual void b0(void) = 0;
    virtual void b1(void) = 0;
    virtual void b2(void) = 0;
};

class IPickupListener {
public:
    virtual void c0(void) = 0;
    virtual void c1(void) = 0;
    virtual void c2(void) = 0;
    virtual void c3(void) = 0;
};

template <typename T>
struct ArrayRef {
    ~ArrayRef(void) {
        data = 0;
        count = 0;
        cap = 0;
    }
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
};

template <typename T, int N>
struct FixedList {
    ~FixedList(void) {}
    FixedList(void) {
        a.data = storage;
        a.cap = N;
        a.count = 0;
    }
    void Push(T v) {
        T* p = &a.data[a.count++];
        *p = v;
    }
    T* Add(const T& v);
    ArrayRef<T> a;  // offset 0x0
    T storage[N];   // offset 0xC
};

struct CoinCar {
    u8 active;     // offset 0x0
    s32 count;     // offset 0x4
    f32 flash[4];  // offset 0x8
    u32 next;      // offset 0x18
};


extern "C" void fn_80167EEC(void);

struct Coins {
    Coins(void);
    ~Coins(void) {
        if (enabled) {
            if (obj) {
                delete obj;
            }
            fn_80167EEC();
            enabled = 0;
        }
    }
    s32 Count(int id) { return enabled ? cars[id].count : 0; }

    u8 enabled;       // offset 0x0
    CoinCar cars[8];  // offset 0x4
    ILayout* obj;     // offset 0xE4
    s32 value;        // offset 0xE8
};


extern "C" {

void fn_80149588(TextElem* e, f32 secs) {
    char buf[0x20];
    fn_80130980(buf, sizeof(buf), 3, secs);
    fn_801474EC(e, "%s", buf);
}

f32 fn_801495D4(int period) {
    f32 s = sin(6.2831855f * (f32)(lbl_8055E3C4 % period) / (f32)period);
    return 0.5f + 0.5f * s;
}

f32 fn_80149650(void) { return fn_801495D4(64); }

}  // extern "C"

class BetaMaster;

struct PlaceInfo {
    u8 pad0[0x4];
    s32 place;  // offset 0x4
};

class PlaceSource {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual PlaceInfo* Get(void);
};

struct RaceCarRec {
    u8 pad0[0x10];
    u32 model;            // offset 0x10
    u8 pad14[0x60 - 0x14];
    f32 pos[3];           // offset 0x60
    s16 heading;          // offset 0x6C
    u8 pad6E[0x118 - 0x6E];
    f32 vel[3];           // offset 0x118
    u8 pad124[0x1EC - 0x124];
    s32 index;            // offset 0x1EC
    s32 kind;             // offset 0x1F0
    u8 pad1F4[0x270 - 0x1F4];
    PlaceSource* places;  // offset 0x270
    u8 pad274[0x25A0 - 0x274];
    f32 f25A0;            // offset 0x25A0
    u8 pad25A4[0x25BC - 0x25A4];
    f32 turbo;            // offset 0x25BC
    u8 pad25C0[0x25D4 - 0x25C0];
    f32 speed;            // offset 0x25D4
    u8 pad25D8[0x2620 - 0x25D8];
    s32 i2620;            // offset 0x2620
    u8 pad2624[0x2690 - 0x2624];
    f32 f2690;            // offset 0x2690

    f32 F2690(void) { return f2690; }
};

class BetaCar;

extern "C" RaceCarRec* fn_8017960C(int id);

inline int Round(f32 x) { return x < 0.0f ? x - 0.5f : x + 0.5f; }

extern "C" {
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* t);
}
void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

// Points for each finishing place, read from place_bonus.val (multiplayer: base + rate per second).
struct PlaceBonus;
static inline void LoadPlaceBonus(PlaceBonus* p);

struct PlaceBonus {
    PlaceBonus(void) {
        table = 0;
        n = 0;
        index = -1;
        multiBase = 1;
        multiRate = 1;
        LoadPlaceBonus(this);
    }
    ~PlaceBonus(void) {
        if (table) {
            delete[] table;
            table = 0;
            n = 0;
        }
    }
    s32 Get(void) const {
        if (index >= 0 && index < n) {
            return table[index];
        }
        return 0;
    }

    s32* table;     // offset 0x0
    s32 n;          // offset 0x4
    s32 index;      // offset 0x8
    s32 multiBase;  // offset 0xC
    s32 multiRate;  // offset 0x10
};

static inline void LoadPlaceBonus(PlaceBonus* p) {
    ValueTable* t = fn_80131808("place_bonus.val");
    if (t) {
        int ok = 0;
        ValueNode* nd = t->FindChild("place_bonus");
        if (nd) {
            int i;
            char buf[100];
            ok = 1;
            p->n = 1;
            for (;;) {
                fn_801345D8(buf, sizeof(buf), "bonus_%d", p->n);
                if (!nd->Has(buf)) {
                    break;
                }
                p->n++;
            }
            p->table = new ("bonus_values") s32[p->n + 1];
            for (i = 0; i < p->n - 1; i++) {
                fn_801345D8(buf, sizeof(buf), "bonus_%d", i + 1);
                p->table[i] = nd->GetIntValue(buf);
            }
            {
                s32 last = p->n - 1;
                p->table[last] = 0;
            }
            p->multiBase = nd->GetInt("multi_base", p->multiBase);
            p->multiRate = nd->GetInt("multi_rate", p->multiRate);
        }
        if (ok) {
            fn_802E30C8("betarace: loading place bonus from place_bonus.val");
        }
        fn_80131818(t);
    }
    if (!p->table) {
        fn_802E30C8("betarace: place bonus not loaded");
        p->table = new ("bonus_values") s32[1];
        p->table[0] = 0;
        p->n = 1;
    }
}

// Final placing and score of one car, as shown on the results screen.
struct CarResult {
    void Finish(const BetaCar* c);
    void Compute(const BetaCar* c);
    f32 SinceFinish(const BetaCar* c);

    s32 score;         // offset 0x0
    s32 place;         // offset 0x4
    PlaceBonus table;  // offset 0x8
    s32 timeLeft;      // offset 0x1C
    s32 bonus;         // offset 0x20
    s32 total;         // offset 0x24
    f32 clock;         // offset 0x28
    f32 finishTime;    // offset 0x2C
    s32 grade;         // offset 0x30
    u8 dnf;            // offset 0x34
};

// One line of the stunt summary on the results screen.
class StuntLine {
public:
    virtual ~StuntLine(void) {}
    char label[0xA0];  // offset 0x4
    char value[0x54];  // offset 0xA4
};

class StuntList {
public:
    virtual ~StuntList(void) {}
    FixedList<StuntLine, 16> lines;  // offset 0x4
};

// The results-screen animation of one player: place, bonus count-up, grade and stunt list.
class FinishAnimation {
public:
    FinishAnimation(void) : anim(0), bonusSnd(0), noBonusSnd(0), transitionSnd(0) {}
    ~FinishAnimation(void) { Release(); }
    void Release(void) {
        if (anim) {
            fn_802ADE60(anim, 1);
            anim = 0;
        }
        SafeDelete(bonusSnd);
        SafeDelete(noBonusSnd);
        SafeDelete(transitionSnd);
    }
    int Update(const CarResult* r, const StuntList* s, s32 coins);
    void Setup(void);

    s32 state;              // offset 0x0
    f32 bonusStart;         // offset 0x4
    ArtAnimMan* anim;       // offset 0x8
    u8 go;                  // offset 0xC
    Sound* bonusSnd;        // offset 0x10
    Sound* noBonusSnd;      // offset 0x14
    Sound* transitionSnd;   // offset 0x18
    s32 animArg;            // offset 0x1C
    s32 signal[3];          // offset 0x20
    FixedStr<0x14> place;   // offset 0x2C
    FixedStr<5> suffix;     // offset 0x40
    FixedStr<0x14> bonus;   // offset 0x45
    FixedStr<0x14> base;    // offset 0x59
    FixedStr<0x14> total;   // offset 0x6D
    FixedStr<10> grade;     // offset 0x81
    FixedStr<10> grade2;    // offset 0x8B
    FixedStr<0x14> stuntLabel[9];  // offset 0x95
    FixedStr<0x14> stuntValue[9];  // offset 0x149
    FixedStr<0x28> title;   // offset 0x1FD
    FixedStr<0x14> count;   // offset 0x225
    s32 x23C;               // offset 0x23C
};

static inline void Log(const FinishAnimation* a, const char* msg) {
    fn_802E30C8("FinishAnimation: %s (tick %d, signal %d/%d/%d)", msg, lbl_8055E3C4, a->signal[0], a->signal[1],
                a->signal[2]);
}

int FinishAnimation::Update(const CarResult* r, const StuntList* s, s32 coins) {
    int i;
    s32 b;
    s32 shown;
    if (!anim) {
        return 0;
    }
    {
        s32 p = r->place;
        fn_801345D8(place.s, 0x14, "%d", p);
        suffix.Set(fn_80130FE0(p));
    }
    if (fn_80179650() == 1) {
        char buf[0x32];
        fn_801345D8(buf, sizeof(buf), "ResultsOverlay:FINISH_BONUS_%d", r->place);
        title.Set(fn_80131234(buf));
    } else {
        title.Set(fn_80131234("ResultsOverlay:WINNING_BONUS"));
    }
    b = r->bonus;
    shown = 0;
    if (b && bonusStart > 0.0f) {
        f32 t = Ramp(lbl_8055E3C8 - bonusStart, 0.0f, 1.0f);
        shown = Lerp(0.0f, 0.5f + b, t);
    }
    fn_801308F4(bonus.s, 0x14, shown);
    {
        s32 tot = r->total;
        s32 basePts = tot - b;
        fn_801308F4(base.s, 0x14, basePts);
        fn_801308F8(total.s, 0x14, tot);
        fn_801345D8(count.s, 0x14, r->timeLeft ? "%d" : "-", r->timeLeft);
        int g = fn_801609C4(basePts);
        if (g != -1) {
            grade.Set(GetGradeDisplayName(g));
        } else {
            grade.Set("");
        }
    }
    if (r->grade != -1) {
        grade2.Set(GetGradeDisplayName(r->grade));
    } else {
        grade2.Set("");
    }
    for (i = 0; i < s->lines.a.count; i++) {
        const StuntLine* l = &s->lines.a.data[i];
        stuntLabel[i].Set(l->label);
        stuntValue[i].Set(l->value);
    }
    for (; i < ARRAY_COUNT(stuntLabel); i++) {
        stuntLabel[i].Set("");
        stuntValue[i].Set("");
    }
    fn_802AE168(anim);
    switch (state) {
    case 0:
        Log(this, "initial state - starting animation");
        fn_802ADFF4(anim, animArg);
        if (b > 0) {
            if (bonusSnd) {
                bonusSnd->Play();
            }
        } else if (noBonusSnd) {
            noBonusSnd->Play();
        }
        state = 1;
        break;
    case 1:
        if (signal[0] && bonusStart < 0.0f) {
            Log(this, "first anim - starting place bonus");
            bonusStart = lbl_8055E3C8;
        }
        if (signal[1]) {
            Log(this, "first anim - moving to wait state");
            fn_802AE02C(anim, animArg);
            state = 2;
        }
        break;
    case 2:
        if (go) {
            Log(this, "waiting - starting second animation");
            fn_802AE0B8(anim, animArg);
            if (transitionSnd) {
                transitionSnd->Play();
            }
            state = 3;
        }
        break;
    case 3:
        if (signal[2]) {
            Log(this, "second anim - ending animation");
            fn_802AE0E0(anim);
            state = 4;
        }
        break;
    case 4:
        break;
    }
    return state != 4;
}


// ---------------------------------------------------------------------------------------------------
// Stunt events queued on a car's HUD, an intrusive list of them and an iterator.

extern "C" {
int fn_80176A1C(void);
f32 fn_80176A2C(void);
}

static inline int SlowMotion(void) { return fn_80176A1C() == 2; }
static inline f32 FrameDt(void) { return (SlowMotion() ? fn_80176A2C() : 1.0f) * 0.016683351f; }

inline f32 Floor(f32 x) { return floor(x); }
extern f32 lbl_80558DC4[];  // FLT_EPSILON (MSL)
inline int NearZero(f32 x) { return (f32)__fabs(x) <= lbl_80558DC4[0]; }

class StuntEvent {
public:
    virtual ~StuntEvent(void) {}
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void SetValue(f32 v) { value = v; }
    void Tick(void) {
        if (running) {
            t0C += FrameDt();
        }
    }
    int IsRunning(void) { return running; }
    int IsReady(void) { return ready; }
    f32 Value(void) { return value; }
    s32 Type(void) { return type; }
    void Start(void) { start = lbl_8055E3C8; }
    void Age(void) { age += FrameDt(); }
    f32 GetAge(void) { return age; }
    s32 Points(void) { return points; }

    u8 running;         // offset 0x4
    u8 ready;           // offset 0x5
    f32 age;            // offset 0x8
    f32 t0C;            // offset 0xC
    f32 start;          // offset 0x10
    u8 pad14[0x104 - 0x14];
    f32 value;          // offset 0x104
    s32 points;         // offset 0x108
    s32 type;           // offset 0x10C
    u8 pad110[0x11C - 0x110];
    StuntEvent* next;   // offset 0x11C
    StuntEvent* prev;   // offset 0x120
};


struct StuntQueue {
    void Unlink(StuntEvent* n) {
        if (n->prev) {
            n->prev->next = n->next;
        } else {
            head = n->next;
        }
        if (n->next) {
            n->next->prev = n->prev;
        } else {
            tail = n->prev;
        }
        n->prev = 0;
        n->next = 0;
    }
    void PushBack(StuntEvent* n) {
        n->next = 0;
        n->prev = tail;
        if (tail) {
            tail->next = n;
        }
        tail = n;
        if (!head) {
            head = n;
        }
    }
    StuntEvent* head;  // offset 0x0
    StuntEvent* tail;  // offset 0x4
};

struct StuntIter {
    StuntIter(StuntQueue* l, int rev) {
        list = l;
        if (!rev) {
            cur = l->head;
            if (cur) {
                next = cur->next;
                prev = cur->prev;
            } else {
                prev = 0;
                next = 0;
            }
        } else {
            cur = l->tail;
            if (cur) {
                next = cur->next;
                prev = cur->prev;
            } else {
                prev = 0;
                next = 0;
            }
        }
    }
    StuntEvent* Valid(void) { return cur; }
    int Next(void) {
        cur = next;
        if (cur) {
            next = cur->next;
            prev = cur->prev;
        } else {
            prev = 0;
            next = 0;
        }
        return cur != 0;
    }
    StuntEvent* Get(void) { return cur; }
    StuntEvent* operator->(void) { return cur; }
    void Remove(void) { list->Unlink(cur); }

    StuntQueue* list;  // offset 0x0
    StuntEvent* cur;   // offset 0x4
    StuntEvent* next;  // offset 0x8
    StuntEvent* prev;  // offset 0xC
};

class CarHud {
public:
    virtual ~CarHud(void) {
        if (bigSnd) {
            delete bigSnd;
        }
        if (bonusSnd) {
            delete bonusSnd;
        }
    }
    StuntEvent pool[16];   // offset 0x4
    StuntQueue shown;      // offset 0x1244
    StuntQueue pending;    // offset 0x124C
    StuntEvent* current;   // offset 0x1254
    Sound* bonusSnd;       // offset 0x1258
    Sound* bigSnd;         // offset 0x125C
    u8 played;             // offset 0x1260
    u8 pad1261[0x1268 - 0x1261];
    s32 racer;             // offset 0x1268
};

class Meter {
public:
    virtual ~Meter(void);
    virtual void vf0C(void);
    virtual void Reset(void);                             // 0x10
    virtual f32 Rate(RaceCarRec* rc);                     // 0x14
    virtual void vf18(void);
    virtual void SetActive(int on);                       // 0x1C
    virtual void Hit(f32 mag, int other);                 // 0x20
    virtual void Bump(void);                              // 0x24
    virtual void Stop(void);                              // 0x28
    virtual void Award(RaceCarRec* rc, f32 v, int kind);  // 0x2C
    virtual void Start(RaceCarRec* rc);                   // 0x30
    virtual void vf34(void);
    virtual s32 Total(void);                              // 0x38
    virtual void vf3C(void);
    virtual int Pending(f32* out);                        // 0x40
    virtual int Count(void);                              // 0x44
    virtual void Fill(StuntList* l);                      // 0x48
};

struct RacerState {
    u32 flags;  // offset 0x0
    s32 place;  // offset 0x4
    u8 pad8[0x10 - 0x8];
    s32 rank;   // offset 0x10
    u8 pad14[0x24 - 0x14];
    s32 state;  // offset 0x24
    u8 out;     // offset 0x28
};

class Racer {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual RacerState* GetInfo(void);
};

struct CoinFrame {
    s32 a;      // offset 0x0
    s32 count;  // offset 0x4
    s32 c[4];
};

extern "C" {
void fn_802CAC54(s32 tex);
void fn_802C6CF8(s32 model);
}

struct TexRef {
    ~TexRef(void) { fn_802CAC54(h); }
    s32 h;
};

extern s32 lbl_8055E258;  // pickups alive
extern s32 lbl_80559ECC;
extern s32 lbl_80559ED0;
extern s32 lbl_80559ED4;
extern s32 lbl_80559ED8;
extern s32 lbl_80559EDC;
extern s32 lbl_8055E254;

// A star or coin lying on the track; the models are shared by all of them.
struct Pickup {
    Pickup(ValueNode* n);
    ~Pickup(void) {
        if (--lbl_8055E258 == 0) {
            fn_802CAC54(lbl_80559EDC);
            lbl_80559EDC = -1;
            fn_802C6CF8(lbl_80559ED4);
            fn_802C6CF8(lbl_80559ED8);
            fn_802C6CF8(lbl_80559ECC);
            fn_802C6CF8(lbl_80559ED0);
            fn_802C6CF8(lbl_8055E254);
            lbl_80559ED0 = -1;
            lbl_80559ECC = -1;
            lbl_8055E254 = -1;
        }
    }
    f32 pos[3];            // offset 0x0
    void* objs[2];         // offset 0xC
    s32 which;             // offset 0x14
    f32 switchLeft;        // offset 0x18
    f32 switchTime;        // offset 0x1C
    TexRef tex;            // offset 0x20
    f32 mid[3];            // offset 0x24
    s16 track[2];          // offset 0x30
    f32 respawn;           // offset 0x34
    s32 kind;              // offset 0x38
    u32 colors[15];        // offset 0x3C
    u8 pad78[0x8C - 0x78];
    u32 glow[10];          // offset 0x8C
    class BoxVolume* vol;  // offset 0xB4
    s32 hB8;               // offset 0xB8
    f32 angle;             // offset 0xBC
    s32 hC0;               // offset 0xC0
    u8 hit[8];             // offset 0xC4
    u8 big;                // offset 0xCC
    const char* name;      // offset 0xD0
};

struct Puddle {
    Puddle(const f32* p, f32 rad, u8 fromFile) {
        x = p[0];
        y = p[1];
        z = p[2];
        r = rad;
        b10 = fromFile;
    }
    f32 x;   // offset 0x0
    f32 y;   // offset 0x4
    f32 z;   // offset 0x8
    f32 r;   // offset 0xC
    u8 b10;  // offset 0x10
};

struct PickupList {
    Pickup** items;  // offset 0x0
    s32 count;       // offset 0x4
};

struct PuddleList {
    Puddle** items;  // offset 0x0
    s32 count;       // offset 0x4
};

struct PickupEvent {
    f32 pos[3];   // offset 0x0
    f32 vel[3];   // offset 0xC
    s16 heading;  // offset 0x18
    s32 sfx;      // offset 0x1C
};

struct V3 {
    f32 x, y, z;
};

struct BumpObj;

extern "C" {
CoinFrame fn_801683F4(void);
int fn_801B8514(RaceCarRec* rc);
Racer* fn_801B2EC0(int id);
Racer* fn_801B2ED0(int id);
void fn_801DF074(int a, int b);
void fn_801796EC(int type, void* data, int size);
f32 fn_801D48F4(void* obj);
void fn_80161A38(int a, int id, int b);
void fn_801D488C(void* obj, int a, int b, V3* pos);
void fn_8014E61C(int id, int kind, f32 v);
void fn_801E1250(int id, int n);
void fn_8018A098(RaceCarRec* rc, f32 v);
f32 PSVECSquareDistance(const void* a, const void* b);
}

extern f32 lbl_80559EB8;
extern s32 lbl_80559EC0;

// What the race keeps about one car (0xC0, eight of them in BetaMaster).
class BetaCar {
public:
    void Update(PickupList* stars, PuddleList* puddles);
    void DrawHud(ILayout* l, int view);
    void DrawResults(ILayout* l, int view);
    void StopBonus(void);
    int Results(FinishAnimation* a);
    void SetResultsView(FinishAnimation* a);
    void DrawWinner(ILayout* l) const;
    void DrawBonusStar(ILayout* l, int n);
    void Retire(void);
    void Finish(void);

    s32 id;              // offset 0x0
    s32 mode;            // offset 0x4
    CarHud* hud;         // offset 0x8
    StuntList* stunts;   // offset 0xC
    Meter* meterObj;     // offset 0x10
    f32 score;           // offset 0x14
    f32 shown;           // offset 0x18
    f32 meter;           // offset 0x1C
    f32 meterSmooth;     // offset 0x20
    s32 shownInt;        // offset 0x24
    u8 moving;           // offset 0x28
    f32 fade;            // offset 0x2C
    f32 fullTime;        // offset 0x30
    s32 boosts;          // offset 0x34
    s32 maxBoosts;       // offset 0x38
    f32 fullFlash;       // offset 0x3C
    f32 flash[5];        // offset 0x40
    f32 countUp;         // offset 0x54
    f32 f58;             // offset 0x58
    u8 pad5C[0x64 - 0x5C];
    f32 overtime;        // offset 0x64
    BetaMaster* master;  // offset 0x68
    CarResult result;    // offset 0x6C
    void* star;          // offset 0xA4
    u8 bA8;              // offset 0xA8
    u8 bA9;              // offset 0xA9
    u8 bAA;              // offset 0xAA
    u8 bAB;              // offset 0xAB
    Sound* starSnd;      // offset 0xAC
    Sound* fullSnd;      // offset 0xB0
    s32 tex[3];          // offset 0xB4
};

struct TrackDesc {
    u8 pad0[0x8];
    char name[0x20];  // offset 0x8
};

struct EventInfo {
    u8 pad0[0x4];
    s32 track;        // offset 0x4
    u8 pad8[0xC - 0x8];
    s32 laps;         // offset 0xC
    u8 pad10[0x20 - 0x10];
    ValueNode* vals;  // offset 0x20
};


struct LocalCarDesc {
    s32 event;  // offset 0x0
};

struct Settings {
    u8 pad0[0x20];
    ValueNode* vals;  // offset 0x20
};

extern "C" {
LocalCarDesc* fn_801769CC(void);
f32 fn_80204CFC(const char* name);  // tuning value
Sound* fn_80207848(const char* name, int flags);
Settings* fn_8000AC38(void);
void fn_801679F0(void);
}
EventInfo* GetEvent(int event);
TrackDesc* GetTrack(int track);

static inline void InitCoins(Coins* c) {
    ValueNode* v;
    c->enabled = 0;
    v = fn_8000AC38()->vals;
    if (v->Has("coin_file")) {
        fn_801679F0();
        c->obj = 0;
        c->value = v->GetInt("coin_value", 3);
        c->enabled = 1;
    }
}

inline Coins::Coins(void) { InitCoins(this); }

struct RaceTimer;
static inline void InitTimer(RaceTimer* t);

// The race clock: time limit from the event, the finish timeout and the warning sounds.
struct RaceTimer {
    RaceTimer(void) { InitTimer(this); }
    ~RaceTimer(void) {
        if (tick) {
            delete tick;
        }
        if (timeout) {
            delete timeout;
        }
    }

    f32 now;            // offset 0x0
    f32 finishTimeout;  // offset 0x4
    f32 limit;          // offset 0x8
    f32 limit0;         // offset 0xC
    u8 b10;             // offset 0x10
    Sound* tick;        // offset 0x14
    Sound* timeout;     // offset 0x18
    u8 b1C;             // offset 0x1C
};

static inline void InitTimer(RaceTimer* t) {
    t->limit = GetEvent(fn_801769CC()->event)->vals->GetFloat("time_limit", 300.0f);
    t->limit0 = t->limit;
    t->finishTimeout = fn_80204CFC("finish_timeout");
    t->tick = fn_80207848("clocktick.sfx", 1);
    t->timeout = fn_80207848("timeout.sfx", 1);
    t->now = 0.0f;
    t->b10 = 0;
        StopSound(t->tick);
        StopSound(t->timeout);
    t->b1C = 0;
    t->limit = t->limit0;
}

// The stunt race mode.
class BetaMaster : public GameMode, public IRaceListener, public IStuntListener, public IPickupListener {
public:
    BetaMaster(void);
    virtual ~BetaMaster(void);
    BetaCar* CarFor(RaceCarRec* rc) { return &cars[rc->index]; }
    int AllDone(int n) {
        int i;
        for (i = 0; i < n; i++) {
            if (anims[i].state != 4) {
                return 0;
            }
        }
        return 1;
    }
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual int vf20(int car) {
        RacerState* s = fn_801B2ED0(car)->GetInfo();
        int racing = 0;
        int r = 0;
        if (s->state != 3 && !s->out) {
            racing = 1;
        }
        if (racing && !(timer.now > timer.limit)) {
            r = 1;
        }
        return r;
    }
    virtual int vf24(void);
    virtual f32 vf38(void);
    virtual int vf3C(int p);
    virtual void vf40(void);
    virtual void vf44(void);
    virtual void vf48(void);
    virtual void vf4C(int p);
    virtual void vf50(int p);
    virtual int vf54(void);
    virtual int vf58(void);
    virtual int vf5C(int p);
    virtual void vf60(int view);
    virtual int vf64(void);
    virtual int vf68(void);
    virtual int vf88(void);
    virtual int vf8C(int p, int key);
    virtual void vf90(RaceCarRec* a, RaceCarRec* b, void* p, const f32* hit);
    virtual void vf94(RaceCarRec* rc);
    virtual void vf9C(RaceCarRec* rc, int type);
    virtual int vfA0(void* cam);
    virtual void vfA4(RaceCarRec* rc);
    virtual void vfA8(int car);
    virtual void vfB8(void* r) {
        RaceResults* res = (RaceResults*)r;
        int i;
        GameMode::FillResults(r);
        fn_802E30C8("BetaRace: FillResults");
        for (i = 0; i < lbl_8055E4D0; i++) {
            s32 t = cars[i].result.total;
            res->e[i].score = t;
            fn_802E30C8("  car %d scored %d", i, t);
        }
    }
    virtual void vfC0(void);
    virtual void vfC4(void);
    virtual int vfDC(void);
    virtual void* vfE8(void);
    virtual void vfF4(void);
    virtual void vfF8(void) {
        int i;
        for (i = 0; i < ARRAY_COUNT(anims); i++) {
            anims[i].anim = new ("artanim_man") ArtAnimMan(2);
            anims[i].Setup();
        }
        coins.obj = fn_80145E70("bonanza.val");
    }
    virtual void vfFC(void) {
        int i;
        SafeDelete(coins.obj);
        for (i = 1; i >= 0; i--) {
            anims[i].Release();
        }
    }
    virtual void a0(void);
    virtual void a4(void);
    virtual void a1(void);
    virtual void a2(void);
    virtual void a3(void);
    virtual void b0(void);
    virtual void b1(void);
    virtual void b2(void);
    virtual void c0(void);
    virtual void c1(void);
    virtual void c2(void);
    virtual void c3(void);

    f32 skipAt;                      // offset 0x10
    FixedList<Pickup*, 0x80> stars;  // offset 0x14
    FixedList<Puddle*, 0x40> puddles;  // offset 0x220
    FixedList<BumpObj*, 0x20> bumps;   // offset 0x32C
    FixedList<BumpObj*, 0x20> bumps2;  // offset 0x3B8
    Sound* coinSnd;                  // offset 0x444
    Sound* starSnd;                  // offset 0x448
    Sound* preraceSnd;               // offset 0x44C
    u8 b450;                         // offset 0x450
    FinishAnimation anims[2];        // offset 0x454
    RaceTimer timer;                 // offset 0x8D4
    Coins coins;                     // offset 0x8F4
    u8 b9E0;                         // offset 0x9E0
    BetaCar cars[8];                 // offset 0x9E4
    s32 nCars;                       // offset 0xFE4
};

void CarResult::Finish(const BetaCar* c) {
    s32 p;
    clock = c->master->timer.now;
    p = fn_8017960C(c->id)->places->Get()->place;
    place = p;
    table.index = p - 1;
    Compute(c);
}

void CarResult::Compute(const BetaCar* c) {
    f32 left;
    score = dnf ? 0 : Round(c->score);
    if (fn_80179650() > 1) {
        f32 t = SinceFinish(c);
        s32 b = table.index == 0 ? table.multiBase + (s32)(table.multiRate * t) : 0;
        if (b != bonus && c->hud && c->hud->bonusSnd) {
            c->hud->bonusSnd->Play();
        }
        bonus = b;
    } else {
        bonus = table.Get();
    }
    left = c->master->timer.limit - c->master->timer.now;
    left = 0.0f > left ? 0.0f : left;
    total = score + bonus;
    timeLeft = left;
    grade = fn_801609C4(total);
}

f32 CarResult::SinceFinish(const BetaCar* c) {
    f32 d;
    if (finishTime < 0.0f) {
        return 0.0f;
    }
    d = c->master->timer.now - finishTime;
    if (0.0f > d) {
        return 0.0f;
    }
    return d;
}

#pragma dont_inline on
void BetaCar::Update(PickupList* stars, PuddleList* puddles) {
    s32 gain;
    f32 add;
    int wasFull;
    int boosting;
    int over;
    RaceCarRec* rc;
    PickupEvent ev;
    V3 lastPos;
    Coins* co = &master->coins;
    if (co->enabled && co->cars[id].active) {
        CoinCar* cc = &co->cars[id];
        CoinFrame f = fn_801683F4();
        int i;
        for (i = 0; i < 4; i++) {
            if (cc->flash[i] > 0.0f) {
                cc->flash[i] -= 0.016683351f;
            }
        }
        gain = f.count * co->value;
        cc->count += gain;
        for (i = 0; i < f.count; i++) {
            cc->flash[cc->next] = 0.6f;
            cc->next = (cc->next + 1) % 4;
        }
    } else {
        gain = 0;
    }
    {
        CarHud* h = hud;
        StuntIter it(&h->pending, 0);
        while (it.Valid()) {
            it->Tick();
            if (!it->IsRunning()) {
                if (it->IsReady()) {
                    it->SetValue(Floor(it->Value()));
                    if (!h->current) {
                        if (NearZero(fn_8017960C(h->racer)->F2690()) || it->Type() == 13) {
                            h->played = 0;
                            h->current = it.Get();
                            if (Round(h->current->Value()) == 5 && h->bigSnd) {
                                h->bigSnd->Play();
                            }
                            it.Remove();
                            h->current->Start();
                        }
                    }
                } else {
                    it.Remove();
                    h->shown.PushBack(it.Get());
                }
            }
            it.Next();
        }
        add = 0.0f;
        if (h->current) {
            h->current->Age();
            if (!h->played && h->current->GetAge() > 0.2f) {
                h->played = 1;
                if (h->bonusSnd) {
                    h->bonusSnd->Play();
                }
            }
            if (h->current->GetAge() > 1.25f) {
                add = h->current->Points();
                h->shown.PushBack(h->current);
                h->current = 0;
            }
        }
    }
    score += add + gain;
    {
        int m = !((f32)__fabs(score - shown) <= lbl_80558DC4[0]);
        moving = m;
    }
    {
        f32 t;
        t = flash[0] - 0.016683351f;
        flash[0] = t > 0.0f ? t : 0.0f;
        t = flash[1] - 0.016683351f;
        flash[1] = t > 0.0f ? t : 0.0f;
        t = flash[2] - 0.016683351f;
        flash[2] = t > 0.0f ? t : 0.0f;
        t = flash[3] - 0.016683351f;
        flash[3] = t > 0.0f ? t : 0.0f;
        t = flash[4] - 0.016683351f;
        flash[4] = t > 0.0f ? t : 0.0f;
    }
    if (moving) {
        f32 t = 0.06673341f + fade;
        fade = 1.0f < t ? 1.0f : t;
    } else {
        f32 t = fade - 0.06673341f;
        fade = 0.0f > t ? 0.0f : t;
    }
    {
        f32 d = score - shown;
        if (d > 16.683352f) {
            d = 16.683352f + 0.5f * (d - 16.683352f);
        }
        shown += d;
        if ((s32)(shown < 0.0f ? shown - 0.5f : shown + 0.5f) > shownInt) {
            countUp += 0.016683351f;
        }
    }
    {
        Sound* snd = fullSnd;
        if (snd) {
            snd->b4A = 1;
            snd->b4C = 0;
        }
    }
    rc = fn_8017960C(id);
    if (fn_801B8514(rc) && !master->vf20(id)) {
        shown = score;
        result.Compute(this);
        return;
    }
    over = master->timer.now > master->timer.limit;
    if (over && overtime < 0.0f) {
        overtime = lbl_8055E3C8;
    }
    wasFull = (mode == 0 ? 0 : mode == 1 ? 1 : (f32)__fabs(meter - 1.0f) <= lbl_80558DC4[0]);
    boosting = 0;
    if (fn_801B2EC0(id)->GetInfo()->flags & 4) {
        meterObj->Start(rc);
        meterObj->Reset();
        boosting = 1;
    }
    meterObj->SetActive(!bAA && !boosting);
    if (!boosting && !bAA) {
        f32 k = rc->model == 'ACAR' ? lbl_80559EB8 : 1.0f;
        f32 m = meter + 0.0004f * (meterObj->Rate(rc) * k);
        meter = 1.0f < m ? 1.0f : m;
    }
    meterSmooth += 0.1f * (meter - meterSmooth);
    {
        f32 t = fullFlash - 0.016683351f;
        fullFlash = 0.0f > t ? 0.0f : t;
    }
    if (mode == 0 ? 0 : mode == 1 ? 1 : (f32)__fabs(meter - 1.0f) <= lbl_80558DC4[0]) {
        int i;
        int j;
        int busy;
        f32* pos;
        if (!wasFull) {
            if (rc->kind == 0x50) {
                fn_801DF074(2, 1);
            }
            fullFlash = 2.0f;
        }
        fullTime += 0.016683351f;
        busy = stars->count != 0;
        if (fullTime > 3.0f && !busy) {
            boosts++;
            meter = 0.0f;
            meterSmooth = 0.0f;
            maxBoosts = boosts > maxBoosts ? boosts : maxBoosts;
        }
        j = 0;
        if (puddles->count > 0) {
            for (; j < puddles->count; j++) {
                Puddle* p = puddles->items[j];
                f32 dx = rc->pos[0] - p->x;
                f32 dz = rc->pos[2] - p->z;
                if (dx * dx + dz * dz < p->r * p->r ? rc->i2620 > 0 : 0) {
                    f32 v = rc->f25A0 - 0.1f;
                    rc->f25A0 = 0.0f > v ? 0.0f : v;
                }
            }
        }
        pos = rc->pos;
        for (i = 0; i < stars->count; i++) {
            Pickup* e = stars->items[i];
            int got;
            if (e->kind == 3) {
                u8* mine;
                if ((f32)__fabs(e->respawn) <= lbl_80558DC4[0] && (mine = &e->hit[rc->index])[0]) {
                    if (e->big) {
                        ev.pos[0] = e->pos[0];
                        ev.pos[1] = e->pos[1];
                        ev.pos[2] = e->pos[2];
                        ev.vel[0] = rc->vel[0];
                        ev.vel[1] = rc->vel[1];
                        ev.vel[2] = rc->vel[2];
                        ev.heading = rc->heading;
                        ev.sfx = 0x97;
                        fn_801796EC(10, &ev, sizeof(ev));
                    }
                    e->respawn = fn_801D48F4(e->objs[e->which]);
                    got = 1;
                    mine[0] = 0;
                } else {
                    got = 0;
                }
            } else if ((f32)__fabs(e->respawn) <= lbl_80558DC4[0] && PSVECSquareDistance(pos, e->pos) < 25.0f) {
                e->respawn = fn_801D48F4(e->objs[e->which]);
                got = 1;
            } else {
                got = 0;
            }
            if (got) {
                if (e->big) {
                    Sound* snd = starSnd;
                    if (snd) {
                        snd->play = 1;
                        snd->b4B = 0;
                        snd->b4A = 0;
                    }
                }
                lastPos.x = e->pos[0];
                lastPos.y = e->pos[1];
                lastPos.z = e->pos[2];
                star = e->objs[e->which];
                meter = 0.0f;
                fn_80161A38(4, id, 0);
            }
        }
    } else {
        fullTime = 0.0f;
    }
    if (star) {
        if (rc->kind == 0x50) {
            fn_801DF074(3, 1);
        }
        fn_801D488C(star, 1, 1, &lastPos);
        fn_8014E61C(id, 3, *(f32*)&star);
        boosts++;
        star = 0;
        maxBoosts = boosts > maxBoosts ? boosts : maxBoosts;
    }
    if (fn_801B2EC0(id)->GetInfo()->state == 0 && rc->kind == 0x50 && rc->speed > 44.444447f) {
        fn_801DF074(4, 1);
    }
    f58 = 0.0f;
    fn_801E1250(id, meterObj->Count());
    {
        f32 v = 0.0f;
        if (meterObj->Pending(&v)) {
            fn_8018A098(rc, v);
        }
    }
    {
        int turbo = rc->turbo > lbl_80558DC4[0];
        if (lbl_80559EC0 < 0 || lbl_80559EC0 >= 4) {
            lbl_80559EC0 = 0;
        }
        bA8 = 0;
        bA9 = 1;
        if (turbo) {
            if (lbl_80559EC0 == 3 && !bAB) {
                bA8 = 1;
                bA9 = 0;
            } else if ((u32)(lbl_80559EC0 - 1) <= 1) {
                bA8 = 1;
                bA9 = 0;
            } else if (lbl_80559EC0 == 0) {
                bA9 = 0;
            }
        } else {
            bAA = 0;
            bAB = 0;
        }
    }
}
#pragma dont_inline reset

extern "C" {
void fn_8001DFF0(void);
void fn_801E7E18(void);
void fn_802AE220(ArtAnimMan* anim, const f32* rect);
}
int GetNumPlayers(void);

void BetaCar::StopBonus(void) {
    if (result.dnf) {
        fn_8001DFF0();
    }
}

int BetaCar::Results(FinishAnimation* a) {
    stunts->lines.a.count = 0;
    meterObj->Fill(stunts);
    return a->Update(&result, stunts, master->coins.Count(id));
}

void BetaCar::SetResultsView(FinishAnimation* a) {
    fn_801E7E18();
    if (!result.dnf) {
        if (GetNumPlayers() == 2) {
            ArtAnimMan* h = a->anim;
            f32 r[4];
            r[0] = 1.5f;
            r[1] = 8.5f;
            r[2] = 39.0f;
            r[3] = 0.0f;
            if (h) {
                fn_802AE220(h, r);
            }
        } else if (a->anim) {
            fn_802AE220(a->anim, 0);
        }
        if (overtime >= 0.0f) {
            return;
        }
    }
}

void BetaCar::DrawWinner(ILayout* l) const {
    int best = -1;
    const BetaMaster* m = master;
    int i;
    for (i = 0; i < m->nCars; i++) {
        const BetaCar* c = &m->cars[i];
        if (best < 0) {
            best = i;
        } else {
            const BetaCar* b = &m->cars[best];
            if (b->result.total < c->result.total) {
                best = i;
            } else if (b->result.total == c->result.total && b->result.place > c->result.place) {
                best = i;
            }
        }
    }
    if (id == best) {
        fn_801476C4(l->Find("winner"));
    } else {
        fn_801476C4(l->Find("loser"));
    }
}

extern "C" {
u32 fn_80147C50(u32 a, u32 b, f32 t);
double fmod(double x, double y);
void fn_801CB648(int id, f32 t, int done);
void fn_801CB5DC(int id, s32 total, s32 grade);
}

void BetaCar::DrawBonusStar(ILayout* l, int n) {
    f32 left = master->timer.limit - master->timer.now;
    left = 0.0f > left ? 0.0f : left;
    if (!(left < 1.0f)) {
        char buf[100];
        GartElem* a;
        GartElem* b;
        f32 t;
        fn_801345D8(buf, sizeof(buf), "bonus_star_start_%d", n);
        a = l->FindBox(buf);
        fn_801345D8(buf, sizeof(buf), "bonus_star_end_%d", n);
        b = l->FindBox(buf);
        t = result.SinceFinish(this);
        if (a && b) {
            GartElem e;
            f32 ph = fmod(t, 1.875);
            if (ph < 0.5f) {
                e = *a;
                e.color = fn_80147C50(e.color & 0xFFFFFF, e.color, Lerp(0.0f, 1.0f, Ramp(ph, 0.0f, 0.25f)));
            } else {
                f32 f = 2.0f * ph - 1.0f;
                f32 q = f * f;
                f32 s = 1.0f - (1.0f - f) * (1.0f - f);
                e = *a;
                e.pos.x = a->pos.x + s * (b->pos.x - a->pos.x);
                e.pos.y = a->pos.y + q * (b->pos.y - a->pos.y);
                e.w = a->w + q * (b->w - a->w);
                e.h = a->h + q * (b->h - a->h);
                e.color = fn_80147C50(a->color, b->color, f);
            }
            fn_80147940(l, &e, "star_art", 0);
        }
    }
}

void BetaCar::Retire(void) {
    f32 left;
    result.dnf = 1;
    result.Finish(this);
    left = master->timer.limit - master->timer.now;
    left = 0.0f > left ? 0.0f : left;
    fn_801CB648(id, master->timer.limit - left, 0);
}

void BetaCar::Finish(void) {
    f32 add;
    CarHud* h;
    meterObj->Stop();
    h = hud;
    add = 0.0f;
    if (h->current) {
        add += h->current->points;
        h->shown.PushBack(h->current);
        h->current = 0;
    }
    {
        StuntIter it(&h->pending, 0);
        for (; it.Valid(); it.Next()) {
            if (!it->running) {
                if (it->ready) {
                    add += it->points;
                }
                it.Remove();
                h->shown.PushBack(it.Get());
            }
        }
    }
    score += add;
    overtime = lbl_8055E3C8;
    result.Finish(this);
    if (!result.dnf) {
        s32 total = result.total;
        s32 grade = result.grade;
        f32 left;
        fn_802E30C8("betarace: car %d finished at %d points", id, total);
        fn_801CB5DC(id, total, grade);
        left = master->timer.limit - master->timer.now;
        left = 0.0f > left ? 0.0f : left;
        fn_801CB648(id, master->timer.limit - left, 1);
    }
}

extern "C" {
int fn_802C9A58(void);  // widescreen
void fn_801777CC(IRaceListener* l);
void fn_801D0590(IStuntListener* l);
void fn_801D0800(IPickupListener* l);
s32 fn_80232578(const char* name);  // load font
void fn_80167834(void);
void fn_8016A0B0(void);
}

extern BetaMaster* lbl_8055E268;
extern ILayout* lbl_8055E238;
extern ILayout* lbl_8055E23C;
extern ILayout* lbl_8055E240;
extern ILayout* lbl_8055E244;
extern ILayout* lbl_8055E248;
extern s32 lbl_8055E250;

static inline void InitBeta(BetaMaster* m) {
    lbl_8055E268 = m;
    lbl_8055E238 = fn_80145E70("betalyt.val");
    lbl_8055E23C = fn_80145E70("betalyt_2.val");
    lbl_8055E240 = fn_80145E70(fn_802C9A58() ? "beta_multi_fs_ws.val" : "beta_multi_fs.val");
    lbl_8055E244 = fn_80145E70("betares.val");
    lbl_8055E248 = fn_80145E70("betares_2.val");
    m->b9E0 = 0;
    m->nCars = 0;
    fn_801777CC(m);
    fn_801D0590(m);
    fn_801D0800(m);
    m->coinSnd = fn_80207848("pickup_coin.sfx", 1);
    m->starSnd = fn_80207848("pickup_star.sfx", 1);
    m->preraceSnd = fn_80207848("prerace.sfx", 1);
    m->b450 = 0;
    lbl_8055E250 = fn_80232578("digiface.gtx");
    fn_80167834();
    fn_8016A0B0();
}

inline BetaMaster::BetaMaster(void) { InitBeta(this); }


extern "C" GameMode* fn_8014DE50(void) { return new ("betamaster") BetaMaster; }

static inline void AwardCar(BetaCar* c, int id, int kind, f32 v) {
    switch (kind) {
    case 8:
        c->flash[3] = 1.5f;
        break;
    case 1:
        c->flash[1] = 1.5f;
        if (fn_8017960C(id)->kind == 0x50) {
            fn_801DF074(1, 1);
        }
        break;
    case 6:
        c->flash[2] = 1.5f;
        break;
    case 0:
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
    case 9:
        break;
    }
    c->meterObj->Award(fn_8017960C(id), v, kind);
}

extern "C" {

void fn_8014E61C(int id, int kind, f32 v) {
    BetaMaster* m = lbl_8055E268;
    if (m) {
        BetaCar* c = &m->cars[id];
        if (c) {
            AwardCar(c, id, kind, v);
        }
    }
}

void fn_8014E704(void) {}

s32 fn_8014E708(int id) {
    BetaMaster* m = lbl_8055E268;
    if (m) {
        return m->cars[id].meterObj->Total();
    }
    return 0;
}

s32 fn_8014E738(void) { return 0; }

void fn_8014E740(const f32* pos, f32 r) {
    BetaMaster* m = lbl_8055E268;
    if (m) {
        m->puddles.Push(new ("puddle") Puddle(pos, r, 0));
    }
}

void fn_8014E7D8(void) {}

}  // extern "C"

extern f32 lbl_80559EB0;

extern "C" {
void fn_8012E81C(const char* res);
const char* fn_80205268(const char* key, int a, int event);
int fn_8012C33C(const char* name);
ValueTable* fn_80131810(const char* name);
double ceil(double x);

f32 fn_8014E7DC(int event, int a) {
    char buf[0x104];
    EventInfo* e;
    TrackDesc* t;
    const char* key;
    fn_8012E81C("ghostcar.res");
    e = GetEvent(event);
    t = GetTrack(e->track);
    key = fn_80205268("time_limit_override_key", a, event);
    if (!e->vals->Has(key)) {
        f32 lim = e->vals->GetFloat("time_limit", 300.0f);
        const char* res = e->vals->GetString("resource", "xxx");
        ValueTable* vt;
        fn_8012E81C("ghostcar.res");
        fn_801345D8(buf, sizeof(buf), "%sa_%s.val", res, t->name);
        if (!fn_8012C33C(buf)) {
            fn_801345D8(buf, sizeof(buf), "%s_%s.val", res, t->name);
        }
        vt = fn_80131810(buf);
        if (vt) {
            f32 lap = vt->FindChild("info")->GetFloat("lap_time");
            fn_80131818(vt);
            lim = 15.0f * (f32)ceil(lbl_80559EB0 * (lap * e->laps) / 15.0f);
        } else {
            fn_802E30C8("betarace: can't open %s", buf);
        }
        return lim;
    }
    return e->vals->GetFloat(key);
}

}  // extern "C"

extern "C" {
void fn_802328D4(s32 font);
void fn_801D0824(IPickupListener* l);
void fn_801D05B4(IStuntListener* l);
void fn_801777D0(IRaceListener* l);
void fn_80167838(void);
void fn_8016A68C(void);
}

BetaMaster::~BetaMaster(void) {
    int i;
    fn_802328D4(lbl_8055E250);
    lbl_8055E250 = 0;
    delete lbl_8055E238;
    lbl_8055E238 = 0;
    delete lbl_8055E23C;
    lbl_8055E23C = 0;
    delete lbl_8055E240;
    lbl_8055E240 = 0;
    delete lbl_8055E244;
    lbl_8055E244 = 0;
    delete lbl_8055E248;
    lbl_8055E248 = 0;
    for (i = 0; i < nCars; i++) {
        delete cars[i].meterObj;
        if (cars[i].starSnd) {
            delete cars[i].starSnd;
        }
        if (cars[i].fullSnd) {
            delete cars[i].fullSnd;
        }
        fn_802CAC54(cars[i].tex[2]);
        fn_802CAC54(cars[i].tex[1]);
        fn_802CAC54(cars[i].tex[0]);
        delete cars[i].stunts;
        delete cars[i].hud;
        cars[i].hud = 0;
    }
    for (i = 0; i < stars.a.count; i++) {
        delete stars.a.data[i];
    }
    for (i = 0; i < puddles.a.count; i++) {
        delete puddles.a.data[i];
    }
    for (i = 0; i < bumps.a.count; i++) {
        delete bumps.a.data[i];
    }
    for (i = 0; i < bumps2.a.count; i++) {
        delete bumps2.a.data[i];
    }
    if (preraceSnd) {
        delete preraceSnd;
    }
    if (coinSnd) {
        delete coinSnd;
    }
    if (starSnd) {
        delete starSnd;
    }
    fn_801D0824(this);
    fn_801D05B4(this);
    fn_801777D0(this);
    lbl_8055E268 = 0;
    fn_80167838();
    fn_8016A68C();
}

// One named value the results animation can show: 1 = text, 2 = int, 3 = signal, 4 = model.
struct AnimBinding {
    s32 type;         // offset 0x0
    const char* name;  // offset 0x4
    const void* ptr;   // offset 0x8
};

extern const AnimBinding lbl_802FAA40;  // end of a binding table

static inline const char* OptNot(int off, const char* str) { return off ? "" : str; }
static inline const char* Opt(int on, const char* str) { return on ? str : ""; }

static inline AnimBinding Bind(s32 t, const char* n, const void* p) {
    AnimBinding b;
    b.type = t;
    b.name = n;
    b.ptr = p;
    return b;
}

extern "C" {
s32 fn_80204CF0(void);  // difficulty
s32 fn_802ADF34(ArtAnimMan* anim, const char* file, const AnimBinding* table);
}

void FinishAnimation::Setup(void) {
    int multi = fn_80179650() > 1;
    int ws = fn_802C9A58();
    int i;
    AnimBinding table[40] = {
        Bind(3, "place_bonus", &signal[0]),
        Bind(3, "grade", &signal[1]),
        Bind(3, "total_score", &signal[2]),
        Bind(1, "place", Opt(!multi && !ws, place.s)),
        Bind(1, "place_suffix", Opt(!multi && !ws, suffix.s)),
        Bind(1, "place_ws", Opt(!multi && ws, place.s)),
        Bind(1, "place_suffix_ws", Opt(!multi && ws, suffix.s)),
        Bind(1, "place_mp", Opt(multi && !ws, place.s)),
        Bind(1, "place_suffix_mp", Opt(multi && !ws, suffix.s)),
        Bind(1, "place_ws_mp", Opt(multi && ws, place.s)),
        Bind(1, "place_suffix_ws_mp", Opt(multi && ws, suffix.s)),
        Bind(1, "place_bonus", bonus.s),
        Bind(1, "total_score_no_bonus", base.s),
        Bind(1, "total_score", total.s),
        Bind(1, "grade_no_bonus", grade.s),
        Bind(1, "grade", OptNot(multi, grade2.s)),
        Bind(1, "grade_title", multi ? "" : fn_80131234("ResultsOverlay:GRADE")),
        Bind(1, "stunt_label_1", stuntLabel[0].s),
        Bind(1, "stunt_value_1", stuntValue[0].s),
        Bind(1, "stunt_label_2", stuntLabel[1].s),
        Bind(1, "stunt_value_2", stuntValue[1].s),
        Bind(1, "stunt_label_3", stuntLabel[2].s),
        Bind(1, "stunt_value_3", stuntValue[2].s),
        Bind(1, "stunt_label_4", stuntLabel[3].s),
        Bind(1, "stunt_value_4", stuntValue[3].s),
        Bind(1, "stunt_label_5", stuntLabel[4].s),
        Bind(1, "stunt_value_5", stuntValue[4].s),
        Bind(1, "stunt_label_6", stuntLabel[5].s),
        Bind(1, "stunt_value_6", stuntValue[5].s),
        Bind(1, "stunt_label_7", stuntLabel[6].s),
        Bind(1, "stunt_value_7", stuntValue[6].s),
        Bind(1, "stunt_label_8", stuntLabel[7].s),
        Bind(1, "stunt_value_8", stuntValue[7].s),
        Bind(1, "stunt_label_9", stuntLabel[8].s),
        Bind(1, "stunt_value_9", stuntValue[8].s),
        Bind(1, "time_bonus_value", count.s),
        Bind(1, "finish_bonus_label", title.s),
        Bind(4, "background_box", multi ? "pr_box1_mp.mod" : "pr_box1.mod"),
        Bind(2, "difficulty_level", &x23C),
        lbl_802FAA40,
    };
    place.Set("1");
    suffix.Set("st");
    bonus.Set("50");
    base.Set("150");
    total.Set("200");
    grade.Set(GetGradeDisplayName(3));
    grade2.Set(GetGradeDisplayName(2));
    title.Set("3rd Bonus");
    for (i = 0; i < 9; i++) {
        stuntLabel[i].Set("BIG AIR!!!11");
        stuntValue[i].Set("33");
    }
    x23C = fn_80204CF0() + 1;
    animArg = fn_802ADF34(anim, "a_beta_finish.val", table);
    bonusSnd = fn_80207848("pr_score.sfx", 1);
    noBonusSnd = fn_80207848("pr_score_nobonus.sfx", 1);
    transitionSnd = fn_80207848("pr_transition.sfx", 1);
}

template <typename T, int N>
T* FixedList<T, N>::Add(const T& v) {
    T* p = &a.data[a.count++];
    *p = v;
    return p;
}

struct TuneRef {
    ValueNode* node;  // offset 0x0
    s32 x4;           // offset 0x4
};

class Trigger;

extern f32 lbl_8055E24C;  // high-speed threshold
extern f32 lbl_80559EB4;  // score factor

extern "C" {
void fn_8013A0B0(TuneRef* r, const char* table, const char* entry);
void fn_8013A198(TuneRef* r, int flag);
s32 fn_80204BB4(const char* s);
f32 fn_801EA0E8(f32 x, f32 z);  // ground height
double sqrt(double x);
int sprintf(char* s, const char* fmt, ...);
}

void BetaMaster::vfF4(void) {
    TuneRef ref;
    ValueTable* t;
    int loaded;
    int i;
    lbl_8055E24C = 57.77778f;
    fn_8013A0B0(&ref, "betarace", "info");
    {
        ValueNode* n = ref.node;
        if (n) {
            lbl_8055E24C = 0.44444445f * n->GetFloat("hispeed_thresh", 130.0f);
            lbl_80559EB4 = n->GetFloat("score_factor", 1.0f);
            fn_802E30C8("betarace: score_factor = %1.2f", lbl_80559EB4);
        }
    }
    fn_8013A198(&ref, -1);
    t = fn_80131810(GetEvent(fn_801769CC()->event)->vals->GetString("trigger_file", "triggers.val"));
    if (t) {
        ValueNode* n;
        for (i = 0; (n = t->GetChild(i)) != 0; i++) {
            int d = fn_80204CF0();
            int nd = n->Has("difficulty_level") ? fn_80204BB4(n->GetValue("difficulty_level")) : d;
            if (nd == d) {
                Pickup* p = new ("trigger") Pickup(n);
                stars.Add(p);
            }
        }
        fn_80131818(t);
        loaded = 1;
    } else {
        loaded = 0;
    }
    if (!loaded) {
        ValueNode* ev = GetEvent(fn_801769CC()->event)->vals;
        ValueNode* n;
        for (i = 0; (n = ev->GetChildAt(i)) != 0; i++) {
            if (n->Has("bound0")) {
                Pickup* p = new ("trigger") Pickup(n);
                stars.Add(p);
            }
        }
    }
    t = fn_80131810("puddles.val");
    if (t) {
        ValueNode* n;
        for (i = 0; (n = t->GetChild(i)) != 0; i++) {
            V2 pts[32];
            char buf[0x20];
            f32 pos[3];
            V2 c;
            V2 d;
            int k;
            int j;
            f32 maxd;
            f32 r;
            f32 h;
            c.x = 0.0f;
            c.y = 0.0f;
            for (k = 0;; k++) {
                sprintf(buf, "bound%d", k);
                if (!n->Has(buf)) {
                    break;
                }
                ValPoint pt = n->GetPoint(buf);
                pts[k].x = pt.x;
                pts[k].y = pt.y;
                c.x += pt.x;
                c.y += pt.y;
            }
            c.x *= 1.0f / k;
            c.y *= 1.0f / k;
            maxd = 0.0f;
            for (j = 0; j < k; j++) {
                f32 dd;
                d.x = pts[j].x - c.x;
                d.y = pts[j].y - c.y;
                dd = d.x * d.x + d.y * d.y;
                maxd = maxd > dd ? maxd : dd;
            }
            r = sqrt(maxd);
            h = fn_801EA0E8(c.x, c.y);
            pos[0] = c.x;
            pos[1] = h;
            pos[2] = c.y;
            puddles.Push(new ("puddle") Puddle(pos, r, 1));
        }
        fn_80131818(t);
    }
}

// A physics box around a spin trigger.
class BoxVolume {
public:
    BoxVolume(f32 x, f32 y, f32 z) {
        x4 = 0;
        x8 = 0;
        xC = 0;
        x10 = 0.0f;
        x14 = 0.0f;
        type = 'BOX ';
        x1C = 0;
        x20 = 100;
        half[0] = x;
        half[1] = y;
        half[2] = z;
        x34 = 0;
        radius = 0.2f + (f32)sqrt(x * x + (y * y + z * z));
    }
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
    virtual void SetMass(int m);  // 0x40

    s32 x4;       // offset 0x4
    s32 x8;       // offset 0x8
    s32 xC;       // offset 0xC
    f32 x10;      // offset 0x10
    f32 x14;      // offset 0x14
    u32 type;     // offset 0x18
    s32 x1C;      // offset 0x1C
    s32 x20;      // offset 0x20
    f32 half[3];  // offset 0x24
    f32 radius;   // offset 0x30
    s32 x34;      // offset 0x34
};

struct Model {
    s32 nMats;  // offset 0x0
    u8* mats;   // offset 0x4
};

struct TexOverride {
    char mat[0x10];  // offset 0x0
    s32 flag;        // offset 0x10
    char tex[0x10];  // offset 0x14
    u8 pad24[0x2C - 0x24];
};

struct Seg {
    u8 pad0[0x8];
    f32 x;      // offset 0x8
    f32 z;      // offset 0xC
    u8 pad10[0x14 - 0x10];
    Seg* next;  // offset 0x14
};

struct TrackSample {
    Seg* seg;  // offset 0x0
    f32 t;     // offset 0x4
};

extern "C" {
s32 fn_802CAC28(const char* name, int flags);  // load texture
void* fn_801D4888(const char* name);
Model* fn_802C6B44(const char* name);
const char* fn_802C9C24(void* mat);
void fn_802C7888(TexOverride* o, int on);
void fn_802C6C20(Model* m);
s32 fn_802C6FC0(const char* name, int a, int b);
void fn_801EA140(Pickup* p);
void fn_801EA1E0(f32 x, f32 z, f32* y, int a);
void fn_801E9DC4(s16* h);
void fn_801E9E30(s16* h, const f32* pos);
s32 fn_80178C9C(void);
void fn_802C77F0(s32 model, f32* box);
void fn_80136594(TrackSample* out, s32 track, const f32* xz);
void PSVECCrossProduct(const f32* a, const f32* b, f32* out);
double acos(double x);
s32 fn_801FB1F4(int a);
s32 fn_801FA8E0(BoxVolume* v, const f32* m, s32 h);
}

static inline void SatModel(const char* name) {
    Model* m = fn_802C6B44(name);
    TexOverride o;
    int i;
    memset(&o, 0, sizeof(o));
    StrCopyN(o.tex, 0x10, "sat.tex");
    o.flag = 1;
    for (i = 0; i < m->nMats; i++) {
        StrCopyN(o.mat, 0x10, fn_802C9C24(m->mats + i * 0x10));
        fn_802C7888(&o, 1);
    }
    fn_802C6C20(m);
}

Pickup::Pickup(ValueNode* n) {
    f32 c[2];
    char buf[0x20];
    int k;
    s32 trk;
    tex.h = fn_802CAC28("trigglow.tex", 0);
    name = n->GetName();
    objs[0] = fn_801D4888(n->GetName());
    kind = n->Has("type") ? n->GetEnum("type", "bank\0jump\0misc\0spin\0multi") : 2;
    big = n->GetBool("visible", 1);
    kind = 3;
    if (kind == 4) {
        if (n->Has("second_actseq")) {
            objs[1] = fn_801D4888(n->GetValue("second_actseq"));
        } else {
            fn_802E30C8("!! Warning: Morph trigger %s is marked as a 'multi' trigger, but has no \"second_actseq\" value.\n",
                        n->GetName());
            objs[1] = objs[0];
        }
        switchTime = n->GetFloat("switch_time", 2.0f);
    }
    if (lbl_8055E258++ == 0) {
        const char* bar = "morphbar.mod";
        const char* ring = "morphring1.mod";
        const char* qst = "morphqst.mod";
        SatModel(bar);
        SatModel(ring);
        SatModel(qst);
        lbl_8055E254 = fn_802C6FC0(ring, 1, 0);
        lbl_80559ED0 = fn_802C6FC0(bar, 0, 0);
        lbl_80559ECC = fn_802C6FC0(qst, 0, 0);
        lbl_80559ED4 = fn_802C6FC0("morph_bubble.mod", 0, 0);
        lbl_80559ED8 = fn_802C6FC0("morph_qmark.mod", 0, 0);
        lbl_80559EDC = fn_802CAC28("zdraw.tex", 0);
    }
    c[0] = 0.0f;
    c[1] = 0.0f;
    for (k = 0;;) {
        sprintf(buf, "bound%d", k);
        if (!n->Has(buf)) {
            break;
        }
        k++;
        {
            ValPoint p = n->GetPoint(buf);
            c[0] += p.x;
            c[1] += p.y;
        }
    }
    pos[1] = 0.0f;
    c[0] *= 1.0f / k;
    pos[0] = c[0];
    c[1] *= 1.0f / k;
    pos[2] = c[1];
    fn_801EA140(this);
    fn_801EA1E0(pos[0], pos[2], &pos[1], 1);
    mid[0] = c[0];
    mid[1] = 0.0f;
    mid[2] = c[1];
    fn_801E9DC4(track);
    fn_801E9E30(track, mid);
    angle = 0.0f;
    colors[0] = 0xFF00FF00;
    colors[1] = 0xFF64FF64;
    colors[2] = 0x6000FF00;
    colors[3] = 0x6064FF64;
    colors[4] = 0xFF00D200;
    colors[5] = 0xFFFF0000;
    colors[6] = 0xFFFF6464;
    colors[7] = 0x60FF0000;
    colors[8] = 0x60FF6464;
    colors[9] = 0xFFD20000;
    colors[10] = 0xFF0000FF;
    colors[11] = 0xFF6464FF;
    colors[12] = 0x600000FF;
    colors[13] = 0x606464FF;
    colors[14] = 0xFF0000D2;
    glow[0] = 0xFF4000C4;
    glow[1] = 0xFF4064C4;
    glow[2] = 0x604000C4;
    glow[3] = 0xFF4064C4;
    glow[4] = 0xFF4000C4;
    glow[5] = 0xFF4000C4;
    glow[6] = 0xFF4064C4;
    glow[7] = 0x604000C4;
    glow[8] = 0xFF4064C4;
    glow[9] = 0xFF4000C4;
    hB8 = 0;
    hC0 = 0;
    hit[0] = 0;
    hit[1] = 0;
    hit[2] = 0;
    hit[3] = 0;
    hit[4] = 0;
    hit[5] = 0;
    hit[6] = 0;
    hit[7] = 0;
    trk = fn_80178C9C();
    if (kind == 3) {
        f32 box[6];
        f32 m[12];
        f32 xz[2];
        TrackSample ts;
        f32 dir[2];
        f32 fwd[2];
        f32 a[3];
        f32 b[3];
        f32 cr[3];
        f32 hx;
        f32 hy;
        f32 hz;
        fn_802C77F0(lbl_80559ED4, box);
        m[0] = 1.0f;
        m[1] = 0.0f;
        m[2] = 0.0f;
        m[3] = 0.0f;
        m[4] = 1.0f;
        m[5] = 0.0f;
        m[6] = 0.0f;
        m[7] = 0.0f;
        m[8] = 1.0f;
        m[9] = 0.0f;
        m[10] = 0.0f;
        m[11] = 0.0f;
        m[9] = pos[0];
        m[10] = pos[1];
        m[11] = pos[2];
        xz[0] = pos[0];
        xz[1] = pos[2];
        fn_80136594(&ts, trk, xz);
        {
            Seg* s0 = ts.seg;
            f32 len;
            dir[0] = s0->x + ts.t * (s0->next->x - s0->x);
            dir[1] = s0->z + ts.t * (s0->next->z - s0->z);
            len = sqrt(dir[0] * dir[0] + dir[1] * dir[1]);
            fwd[0] = m[6];
            fwd[1] = m[8];
            dir[0] *= 1.0f / len;
            dir[1] *= 1.0f / len;
        }
        a[0] = dir[0];
        a[1] = 0.0f;
        a[2] = dir[1];
        b[0] = fwd[0];
        b[1] = 0.0f;
        b[2] = fwd[1];
        PSVECCrossProduct(b, a, cr);
        if (cr[1] > 0.0f) {
            angle = acos(dir[0] * fwd[0] + dir[1] * fwd[1]);
        } else {
            angle = -(f32)acos(dir[0] * fwd[0] + dir[1] * fwd[1]);
        }
        hx = 1.0f + 1.5f * (box[1] - box[0]) * 0.5f;
        hz = 1.0f + 1.5f * (box[5] - box[4]) * 0.5f;
        hy = 1.0f + (box[3] - box[2]);
        vol = new ("spinvol") BoxVolume(hx, hy, hx);
        vol->SetMass(1000);
        hC0 = fn_801FB1F4(1);
        hB8 = fn_801FA8E0(vol, m, hC0);
        vol->half[0] = hx;
        vol->half[1] = hy;
        vol->half[2] = hz;
    }
}

extern "C" {
void fn_8016783C(void);
void fn_801688FC(int a);
void fn_80168020(void);
void fn_8016A0D4(void);
}

void BetaMaster::vf10(void) {
    fn_8016783C();
    if (coins.enabled) {
        memset(coins.cars, 0, sizeof(coins.cars));
        fn_801688FC(1);
        fn_80168020();
    }
    fn_8016A0D4();
    StopSound(preraceSnd);
    b450 = 0;
}

// Physics body of a spin trigger.
struct SpinBody {
    f32 rot[9];  // offset 0x0
    f32 pos[3];  // offset 0x24
};

class ReadyFlag {
public:
    virtual int Ready(void);  // 0x8
};

class Body {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void vf18(void);
    virtual void vf1C(void);
    virtual void vf20(void);
    virtual void vf24(void);
    virtual void SetState(const f32* m, const f32* vel, const f32* spin);  // 0x28
};

struct BumpObj {
    Body* body;         // offset 0x0
    u8 active;          // offset 0x4
    f32 pos[3];         // offset 0x8
    f32 start;          // offset 0x14
    ReadyFlag* ready;   // offset 0x18
};

struct ViewOpts {
    u8 pad0[0x10];
    u32 flags;  // offset 0x10
};

extern "C" {
int fn_801D48F8(void* seq);
int fn_801D4904(void* seq);
int fn_801D4900(void* seq);
f32 fn_801B306C(void);
int fn_80161C38(void);
int fn_80009C84(void);
void fn_80168328(void);
int fn_801B2F2C(int id);
void fn_801688B4(int a);
void fn_80167840(RaceCarRec* rc);
s32 fn_8016A30C(int id);
int fn_80179630(int view);
ViewOpts* fn_801E87A8(void);
void fn_80167684(f32 left);
double floor(double x);
double cos(double x);
}

extern s32 lbl_8055E4D0;  // cars in the race
extern f32 lbl_80559EC8;  // spin trigger wobble speed

static inline void UpdateBumps(FixedList<BumpObj*, 0x20>* l) {
    int i;
    for (i = 0; i < l->a.count; i++) {
        BumpObj* b = l->a.data[i];
        if (b->active) {
            f32 m[12];
            f32 vel[3];
            f32 spin[3];
            f32 a;
            f32 c;
            f32 s;
            m[0] = 1.0f;
            m[1] = 0.0f;
            m[2] = 0.0f;
            m[3] = 0.0f;
            m[4] = 1.0f;
            m[5] = 0.0f;
            m[6] = 0.0f;
            m[7] = 0.0f;
            m[8] = 1.0f;
            m[9] = 0.0f;
            m[10] = 0.0f;
            m[11] = 0.0f;
            m[9] = b->pos[0];
            m[10] = b->pos[1];
            m[11] = b->pos[2];
            a = fmod(3.1415927f * lbl_8055E3C8, 2.3926990816987);
            c = cos(a);
            s = sin(a);
            m[0] = c;
            m[1] = 0.0f;
            m[2] = -s;
            m[3] = 0.0f;
            m[4] = 1.0f;
            m[5] = 0.0f;
            m[6] = s;
            m[7] = 0.0f;
            m[8] = c;
            if (fn_801E87A8()->flags & 2) {
                m[0] = -m[0];
                m[1] = -m[1];
                m[2] = -m[2];
            }
            spin[0] = 0.0f;
            spin[1] = 0.0f;
            spin[2] = 0.0f;
            vel[0] = 0.0f;
            vel[1] = 0.0f;
            vel[2] = 0.0f;
            b->body->SetState(m, vel, spin);
        } else if (b->ready->Ready() && lbl_8055E3C8 > b->start) {
            f32 m[12];
            f32 vel[3];
            f32 spin[3];
            b->active = 1;
            m[0] = 1.0f;
            m[1] = 0.0f;
            m[2] = 0.0f;
            m[3] = 0.0f;
            m[4] = 1.0f;
            m[5] = 0.0f;
            m[6] = 0.0f;
            m[7] = 0.0f;
            m[8] = 1.0f;
            m[9] = 0.0f;
            m[10] = 0.0f;
            m[11] = 0.0f;
            m[9] = b->pos[0];
            m[10] = b->pos[1];
            m[11] = b->pos[2];
            spin[0] = 0.0f;
            spin[1] = 0.0f;
            spin[2] = 0.0f;
            vel[0] = 0.0f;
            vel[1] = 0.0f;
            vel[2] = 0.0f;
            b->body->SetState(m, vel, spin);
        }
    }
}

void BetaMaster::vf0C(void) {
    int i;
    int found;
    f32 lead;
    for (i = 0; i < stars.a.count; i++) {
        Pickup* t = stars.a.data[i];
        SpinBody* body = (SpinBody*)t->hB8;
        if (body) {
            f32 ang = 0.5235988f * (f32)sin(6.2831855f * (f32)fmod(lbl_80559EC8 * lbl_8055E3C8, 1.875)) + t->angle;
            f32 c;
            f32 s;
            body->pos[0] = t->pos[0];
            body->pos[1] = t->pos[1];
            body->pos[2] = t->pos[2];
            body = (SpinBody*)t->hB8;
            c = cos(ang);
            s = sin(ang);
            body->rot[0] = c;
            body->rot[1] = 0.0f;
            body->rot[2] = -s;
            body->rot[3] = 0.0f;
            body->rot[4] = 1.0f;
            body->rot[5] = 0.0f;
            body->rot[6] = s;
            body->rot[7] = 0.0f;
            body->rot[8] = c;
        }
        if (fn_801D48F8(t->objs[t->which]) || fn_801D4904(t->objs[t->which])) {
            f32 r = t->respawn - 0.016683351f;
            if (0.0f > r) {
                r = 0.0f;
            }
            t->respawn = r;
        }
        if (!fn_801D4900(t->objs[t->which])) {
            f32 r = t->respawn;
            t->respawn = r > 0.016683351f ? r : 0.016683351f;
        }
        if (t->kind == 4) {
            t->switchLeft -= 0.016683351f;
            if (t->switchLeft < 0.0f) {
                t->which = 1 - t->which;
                t->switchLeft = t->switchTime;
            }
        }
    }
    found = 0;
    lead = fn_801B306C();
    if (!b450 && lead < 8.0f) {
        b450 = 1;
        if (preraceSnd && !fn_80161C38() && fn_80179650() == 1 && !fn_80009C84()) {
            preraceSnd->Play();
        }
    }
    if (!timer.b10 && lead <= 0.0f && !fn_80161C38()) {
        timer.b10 = 1;
        StopSound(timer.tick);
    }
    if (coins.enabled) {
        fn_80168328();
    }
    for (i = 0; i < lbl_8055E4D0; i++) {
        BetaCar* c = &cars[i];
        c->Update((PickupList*)&stars.a, (PuddleList*)&puddles.a);
        if (!fn_801B2F2C(i)) {
            int over = timer.now > timer.limit;
            if (over) {
                if (!c->result.dnf) {
                    c->Retire();
                }
            } else if (fn_8017960C(i)->kind == 0x50) {
                s32 n;
                found = 1;
                if (fn_801609C4((s32)c->score) == 0 && coins.enabled) {
                    fn_801688B4(1);
                    coins.cars[i].active = 1;
                }
                if (GetNumPlayers() == 1) {
                    fn_80167840(fn_8017960C(i));
                }
                n = fn_8016A30C(i);
                if (n > 0) {
                    fn_8014E61C(i, 5, n);
                }
            }
        } else if (fn_8017960C(i)->kind == 0x50) {
            int j;
            for (j = 0; j < fn_80179650(); j++) {
                if (i == fn_80179630(j)) {
                    if (!c->result.dnf) {
                        c->Results(&anims[j]);
                    }
                    if (anims[j].state == 2 && fn_80179650() > 1) {
                        int ext;
                        int soon = timer.limit - timer.now < timer.finishTimeout;
                        if (!soon) {
                            timer.limit = timer.now + timer.finishTimeout;
                            ext = 1;
                        } else {
                            ext = 0;
                        }
                        if (ext) {
                            if (c->result.finishTime < 0.0f) {
                                c->result.finishTime = timer.now;
                            }
                        } else {
                            f32 left = timer.limit - timer.now;
                            f32 t;
                            left = 0.0f > left ? 0.0f : left;
                            t = timer.limit - ((f32)floor(left) - 0.1f);
                            if (c->result.finishTime < 0.0f) {
                                c->result.finishTime = t;
                            }
                        }
                    }
                }
            }
        }
    }
    UpdateBumps(&bumps);
    UpdateBumps(&bumps2);
    if (skipAt > 0.0f && lbl_8055E3C8 > skipAt) {
        timer.b10 = 1;
        skipAt = -1.0f;
        timer.now = timer.limit - 30.0f;
    }
    if (!found) {
        timer.b10 = 0;
        StopSound(timer.tick);
    }
    if (timer.b10) {
        timer.now += 0.016683351f;
        if (timer.now > timer.limit) {
            StopSound(timer.tick);
            if (timer.timeout && !timer.b1C) {
                timer.timeout->Play();
                timer.b1C = 1;
            }
        } else if (timer.tick && timer.limit - timer.now < timer.finishTimeout && !timer.tick->active) {
            timer.tick->b4A = 0;
            timer.tick->b4B = 1;
            timer.tick->play = 1;
        }
    }
    if (found) {
        f32 left = timer.limit - timer.now;
        fn_80167684(0.0f > left ? 0.0f : left);
    }
}

extern "C" {
f32 PSVECMag(const f32* v);
int fn_801E85A4(void);
int fn_801E86F8(int view);
int fn_801E85B4(void);
int fn_801E866C(int view);
void fn_801E41D8(const V3* a, const V3* b, u32 color, f32 r);
void fn_80167844(void);
void fn_8016A438(void);
}

s32 fn_80151604(void) { return 0; }

int BetaMaster::vf54(void) {
    int f;
    if (fn_80161C38()) {
        return 0x5608;
    }
    f = fn_80179650() > 1 ? 0x160B : 0x564B;
    f &= ~0x40;
    return f;
}

void BetaMaster::vf9C(RaceCarRec* rc, int type) {
    if (type == 1003) {
        BetaCar* c = CarFor(rc);
        if (c) {
            c->meterObj->Bump();
        }
    }
}

void fn_80151688(void) {}

void BetaMaster::vfA4(RaceCarRec* rc) {
    BetaCar* c = CarFor(rc);
    if (c) {
        f32 add = 0.0f;
        CarHud* h;
        c->fullFlash = 0.0f;
        c->meter = 0.5f * c->meter;
        c->star = 0;
        h = c->hud;
        {
            StuntIter it(&h->pending, 0);
            for (; it.Valid(); it.Next()) {
                if (it->running) {
                    it.Remove();
                    h->shown.PushBack(it.Get());
                }
            }
        }
        c->score += add;
        c->meterObj->Reset();
        if (c->boosts > 1) {
            c->flash[0] = 1.5f;
        }
        c->boosts = c->boosts - 1 > 1 ? c->boosts - 1 : 1;
        c->flash[4] = 0.0f;
        c->flash[3] = 0.0f;
        c->flash[2] = 0.0f;
        c->flash[1] = 0.0f;
    }
}

void BetaMaster::vf90(RaceCarRec* a, RaceCarRec* b, void* p, const f32* hit) {
    f32 mag = PSVECMag(hit);
    int fa = 1;
    int fb = 1;
    if (a->kind == 0x50 && b->kind == 0x50) {
        fa = a->speed > b->speed;
        fb = !fa;
    }
    if (fa) {
        BetaCar* c = CarFor(a);
        if (c) {
            c->meterObj->Hit(mag, b->index);
        }
    }
    if (fb) {
        BetaCar* c = CarFor(b);
        if (c) {
            c->meterObj->Hit(mag, a->index);
        }
    }
}

void fn_80151914(void) {}

void BetaMaster::vf94(RaceCarRec* rc) {
    BetaCar* c = CarFor(rc);
    if (c) {
        c->Finish();
    }
    if (coins.enabled) {
        coins.cars[rc->index].active = 0;
    }
}

int BetaMaster::vf8C(int p, int key) {
    if (key == 'X' && vf68()) {
        int n = fn_801E85A4();
        int i;
        for (i = 0; i < n; i++) {
            if (cars[fn_801E86F8(i)].result.dnf || n > 1) {
                anims[i].state = 4;
            } else if (anims[i].state == 2) {
                anims[i].go = 1;
            }
        }
    }
    return 0;
}

void BetaMaster::vf48(void) {
    BetaCar* c = &cars[fn_801E866C(fn_801E85B4())];
    u32 col;
    int i;
    if (c && c->mode != 0) {
        if (c->mode == 1) {
        }
    }
    col = lbl_8055E230.value;
    for (i = 0; i < puddles.a.count; i++) {
        Puddle* p = puddles.a.data[i];
        if (p->b10) {
            V3 a;
            V3 b;
            a = *(V3*)&p->x;
            a.y = a.y - 0.75f;
            b = *(V3*)&p->x;
            b.y = b.y + 0.75f;
            fn_801E41D8(&a, &b, col, p->r);
        }
    }
}

extern "C" {
void fn_801CB7BC(int a);
void fn_8001DFCC(void);
}

void BetaMaster::vf4C(int view) {
    if (b9E0) {
        vf88();
        return;
    }
    (fn_801E85A4() == 2 ? lbl_8055E23C : lbl_8055E238)->Edit();
    {
        BetaCar* c = &cars[fn_801E866C(view)];
        if (c) {
            c->DrawHud(fn_801E85A4() == 2 ? lbl_8055E23C : lbl_8055E238, view);
            fn_801CB7BC(0);
        }
    }
}

void BetaMaster::vf50(int view) {
    BetaCar* c = &cars[fn_801E866C(view)];
    if (c) {
        c->DrawResults(fn_801E85A4() == 2 ? lbl_8055E23C : lbl_8055E238, view);
    }
}

int BetaMaster::vf58(void) { return 0; }

int BetaMaster::vf68(void) {
    int i;
    fn_80179650();
    for (i = 0; i < fn_801E85A4(); i++) {
        if (!cars[fn_801E86F8(i)].result.dnf && anims[i].state != 2) {
            return 0;
        }
    }
    return 1;
}

int BetaMaster::vf64(void) { return 1; }

int BetaMaster::vf5C(int view) {
    int n = fn_80179650();
    BetaCar* mine = &cars[fn_801E86F8(view)];
    if (!mine->result.dnf) {
        BetaCar* c = &cars[fn_801E866C(view)];
        if (c && view >= 0 && view < ARRAY_COUNT(anims)) {
            c->SetResultsView(&anims[view]);
        }
    } else {
        mine->StopBonus();
    }
    if (vf68() && n > 1) {
        int any = 0;
        int i;
        for (i = 0; i < nCars; i++) {
            BetaCar* c = &cars[i];
            if (!c->result.dnf) {
                any = 1;
            }
        }
        if (any) {
            mine->DrawWinner(lbl_8055E248);
        }
    }
    if (vf68() && view == 0) {
        fn_8001DFCC();
    }
    return !AllDone(n);
}

void BetaMaster::vf60(int view) {
    int n = fn_801E85A4();
    if (n > 1) {
        int any = 0;
        int i;
        for (i = 0; i < n; i++) {
            any = (any | vf20(fn_801E866C(i))) != 0;
        }
        if (any) {
            cars[fn_801E86F8(view)].DrawBonusStar(lbl_8055E240, view);
        }
    }
}

int BetaMaster::vf88(void) { return 1; }

f32 BetaMaster::vf38(void) { return GetNumPlayers() == 2 ? 5.0f : 9.0f; }
