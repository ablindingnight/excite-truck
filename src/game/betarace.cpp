// Stunt race ("betarace", 0x80149588-0x801561E8): BetaMaster is the GameMode behind the normal
// excite races - per-car scores and stunts, coins and star pickups, the time limit, the place bonus
// read from place_bonus.val, the HUD layouts (betalyt*.val), the pre-race overlay and the results
// screen with its finish animation. Names are guesses apart from the allocation tags, keys and
// messages.
#include <math.h>
#include "game/common.h"

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
void fn_802AE168(s32 anim);
void fn_802ADFF4(s32 anim, s32 arg);
void fn_802AE02C(s32 anim, s32 arg);
void fn_802AE0B8(s32 anim, s32 arg);
void fn_802AE0E0(s32 anim);
void fn_802ADE60(s32 anim, int arg);
}

const char* GetGradeDisplayName(int grade);

extern s32 lbl_8055E3C4;  // frame counter
extern f32 lbl_8055E3C8;  // race clock

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
    u8 pad8[0x49 - 0x8];
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

struct CoinCar {
    u8 active;     // offset 0x0
    s32 count;     // offset 0x4
    f32 flash[4];  // offset 0x8
    u32 next;      // offset 0x18
};

struct Coins {
    u8 enabled;       // offset 0x0
    CoinCar cars[8];  // offset 0x4
    s32 xE4;          // offset 0xE4
    s32 value;        // offset 0xE8
};

class BetaMaster {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void vf18(void);
    virtual void vf1C(void);
    virtual int vf20(int car);

    u8 pad4[0x8D4 - 0x4];
    f32 clock;          // offset 0x8D4
    f32 finishTimeout;  // offset 0x8D8
    f32 timeLimit;      // offset 0x8DC
    u8 pad8E0[0x8F4 - 0x8E0];
    Coins coins;        // offset 0x8F4
};

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
    s32 pickupFlags;      // offset 0x1EC
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

// Final placing and score of one car, as shown on the results screen.
struct CarResult {
    void Finish(const BetaCar* c);
    void Compute(const BetaCar* c);
    f32 SinceFinish(const BetaCar* c);

    s32 score;        // offset 0x0
    s32 place;        // offset 0x4
    s32* bonusTable;  // offset 0x8
    s32 nBonus;       // offset 0xC
    s32 index;        // offset 0x10
    s32 multiBase;    // offset 0x14
    s32 multiRate;    // offset 0x18
    s32 timeLeft;     // offset 0x1C
    s32 bonus;        // offset 0x20
    s32 total;        // offset 0x24
    f32 clock;        // offset 0x28
    f32 finishTime;   // offset 0x2C
    s32 grade;        // offset 0x30
    u8 dnf;           // offset 0x34
};

struct StuntLine {
    u8 pad0[0x4];
    char label[0xA0];  // offset 0x4
    char value[0x54];  // offset 0xA4
};

struct StuntList {
    u8 pad0[0x4];
    StuntLine* lines;  // offset 0x4
    s32 count;         // offset 0x8
};

// The results-screen animation of one player: place, bonus count-up, grade and stunt list.
class FinishAnimation {
public:
    FinishAnimation(void) {
        anim = 0;
        bonusSnd = 0;
        noBonusSnd = 0;
        transitionSnd = 0;
    }
    ~FinishAnimation(void) {
        if (anim) {
            fn_802ADE60(anim, 1);
            anim = 0;
        }
        SafeDelete(bonusSnd);
        SafeDelete(noBonusSnd);
        SafeDelete(transitionSnd);
    }
    int Update(const CarResult* r, const StuntList* s);

    s32 state;              // offset 0x0
    f32 bonusStart;         // offset 0x4
    s32 anim;               // offset 0x8
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
};

static inline void Log(const FinishAnimation* a, const char* msg) {
    fn_802E30C8("FinishAnimation: %s (tick %d, signal %d/%d/%d)", msg, lbl_8055E3C4, a->signal[0], a->signal[1],
                a->signal[2]);
}

int FinishAnimation::Update(const CarResult* r, const StuntList* s) {
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
    for (i = 0; i < s->count; i++) {
        const StuntLine* l = &s->lines[i];
        stuntLabel[i].Set(l->label);
        stuntValue[i].Set(l->value);
    }
    for (; i < 9; i++) {
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

static inline f32 FrameDt(void) { return (fn_80176A1C() == 2 ? fn_80176A2C() : 1.0f) * 0.016683351f; }

inline f32 Floor(f32 x) { return floor(x); }
extern f32 lbl_80558DC4[];  // FLT_EPSILON (MSL)
inline int NearZero(f32 x) { return (f32)__fabs(x) <= lbl_80558DC4[0]; }

class StuntEvent {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void SetShown(f32 v);
    void Tick(void) {
        if (active) {
            t0C += FrameDt();
        }
    }
    int IsDead(void) { return dead; }
    int IsActive(void) { return active; }
    f32 Value(void) { return value; }
    void SetValue(f32 v) { value = v; }
    s32 Type(void) { return type; }
    void Start(void) { start = lbl_8055E3C8; }
    void Age(void) { age += FrameDt(); }
    f32 GetAge(void) { return age; }
    s32 Points(void) { return points; }

    u8 dead;            // offset 0x4
    u8 active;          // offset 0x5
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
    void Remove(void) {
        StuntEvent* n = cur;
        StuntQueue* l = list;
        if (n->prev) {
            n->prev->next = n->next;
        } else {
            l->head = n->next;
        }
        if (n->next) {
            n->next->prev = n->prev;
        } else {
            l->tail = n->prev;
        }
        n->prev = 0;
        n->next = 0;
    }

    StuntQueue* list;  // offset 0x0
    StuntEvent* cur;   // offset 0x4
    StuntEvent* next;  // offset 0x8
    StuntEvent* prev;  // offset 0xC
};

struct CarHud {
    u8 pad0[0x1244];
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
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual void Reset(void);                             // 0x10
    virtual f32 Rate(RaceCarRec* rc);                     // 0x14
    virtual void vf18(void);
    virtual void SetActive(int on);                       // 0x1C
    virtual void vf20(void);
    virtual void vf24(void);
    virtual void vf28(void);
    virtual void Award(RaceCarRec* rc, f32 v, int kind);  // 0x2C
    virtual void Start(RaceCarRec* rc);                   // 0x30
    virtual void vf34(void);
    virtual void vf38(void);
    virtual void vf3C(void);
    virtual int Pending(f32* out);                        // 0x40
    virtual int Count(void);                              // 0x44
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

// A star or coin lying on the track.
struct Pickup {
    f32 pos[3];     // offset 0x0
    void* objs[2];  // offset 0xC
    s32 which;      // offset 0x14
    u8 pad18[0x34 - 0x18];
    f32 respawn;    // offset 0x34
    s32 kind;       // offset 0x38
    u8 pad3C[0xCC - 0x3C];
    u8 big;         // offset 0xCC
};

struct Puddle {
    f32 x;  // offset 0x0
    f32 y;  // offset 0x4
    f32 z;  // offset 0x8
    f32 r;  // offset 0xC
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

extern "C" {
CoinFrame fn_801683F4(void);
int fn_801B8514(RaceCarRec* rc);
Racer* fn_801B2EC0(int id);
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

    s32 id;              // offset 0x0
    s32 mode;            // offset 0x4
    CarHud* hud;         // offset 0x8
    u8 padC[0x10 - 0xC];
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
};

void CarResult::Finish(const BetaCar* c) {
    s32 p;
    clock = c->master->clock;
    p = fn_8017960C(c->id)->places->Get()->place;
    place = p;
    index = p - 1;
    Compute(c);
}

void CarResult::Compute(const BetaCar* c) {
    f32 left;
    score = dnf ? 0 : Round(c->score);
    if (fn_80179650() > 1) {
        f32 t = SinceFinish(c);
        s32 b = index == 0 ? multiBase + (s32)(multiRate * t) : 0;
        if (b != bonus && c->hud && c->hud->bonusSnd) {
            c->hud->bonusSnd->Play();
        }
        bonus = b;
    } else {
        s32 b;
        if (index >= 0 && index < nBonus) {
            b = bonusTable[index];
        } else {
            b = 0;
        }
        bonus = b;
    }
    left = c->master->timeLimit - c->master->clock;
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
    d = c->master->clock - finishTime;
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
            if (!it->IsDead()) {
                if (it->IsActive()) {
                    it->SetShown(Floor(it->Value()));
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
    over = master->clock > master->timeLimit;
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
                if ((f32)__fabs(e->respawn) <= lbl_80558DC4[0] && (mine = (u8*)e + rc->pickupFlags)[0xC4]) {
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
                    mine[0xC4] = 0;
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
