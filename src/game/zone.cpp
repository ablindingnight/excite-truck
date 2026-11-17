// Gate challenge ("zone") mode (0x8013CB10-0x801444C4): ZoneMaster drives the timed ring/gate
// challenge - levels of gates read from the event's zones file, crossing tests per car, the timer,
// time bonuses, the HUD layouts and the pre-race and results overlays. Names are guesses apart from
// the allocation tags, keys and messages.
//
// ZoneHud and ZoneMaster live in an anonymous namespace and ZoneMaster's virtuals from vf0C on are inline:
// CodeWarrior emits them at the end of the unit after the ZoneSubTask/ZoneMaster vtables, with their
// literals pooled behind them (the original .data layout; see betarace.cpp).
#include <math.h>
#include "game/common.h"
#include "game/pathset.h"
#include "game/glow.h"
#include "game/idealline.h"
#include "game/deity.h"
#include "game/centerline.h"
#include "game/gamemode.h"
#include "game/gamemodector.h"
#include "game/sound.h"
#include "game/ilayout.h"
#include "game/textelem.h"
#include "game/artelemcopy.h"
#include "game/v2.h"
#include "game/locstring.h"
#include "game/raceopts.h"
#include "game/racesetup.h"
#include "game/raceprog.h"
#include "game/gamedb.h"
#include <revolution/MTX/vec.h>
#include "game/strcopy.h"
#include "game/mathutil.h"
#include "game/clamp01.h"

extern "C" {
int strcmp(const char* a, const char* b);
int sprintf(char* s, const char* fmt, ...);
void* memset(void* d, int c, size_t n);
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
int fn_801345D8(char* buf, u32 size, const char* fmt, ...);  // snprintf
ValueTable* fn_80131810(const char* name);  // load value file, quiet
void fn_80131818(ValueTable* t);
}  // extern "C"
const char* fn_8000AC2C(void);
extern "C" {
f32 fn_80204CFC(const char* name);  // tuning value
void fn_802049DC(const char* name, f32* v, f32 lo, f32 hi, f32 step);  // debug tweak
void fn_801331C4(int type, const char* name, f32* out);
int fn_802C6FC0(const char* name, int a, int b);
void mrModelUnload(int model);
s32 fn_802CAC28(const char* name, int flags);  // load texture
void gxForgetTexture(s32 tex);
int fn_80176A1C(void);
void fn_801E9DC4(s16* h);
void* fn_80171698(void);
}

extern "C" {
}

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);
extern "C" f32 fn_8013CB3C(void);
extern u8 lbl_8055E1BC;  // timed-out-of-zone mode

extern f32 lbl_8055E3C8;     // current race time
extern f32 __float_epsilon[];  // FLT_EPSILON (MSL)

extern "C" ILayout* fn_80145E70(const char* name);

static inline void StopSound(GameSound* s) {
    if (s) {
        s->stop = 1;
        s->restart = 0;
    }
}
extern "C" GameSound* fn_80207848(const char* name, int flags);

class TruckPath {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual struct TrackPos* GetPos(void);  // 0x18
};

struct TrackPos {
    u8 pad0[0x48];
    f32 dist;  // offset 0x48
};

struct Truck {
    u8 pad0[0x54];
    ZoneVec3 vel;  // offset 0x54
    ZoneVec3 p60;  // offset 0x60
    u8 pad6C[0x94 - 0x6C];
    ZoneVec3 pos;  // offset 0x94
    u8 padA0[0x118 - 0xA0];
    f32 dirX;  // offset 0x118
    u8 pad11C[0x4];
    f32 dirZ;  // offset 0x120
    u8 pad124[0x270 - 0x124];
    TruckPath* path;  // offset 0x270
    u8 pad274[0x59C - 0x274];
    s32 model;  // offset 0x59C
    u8 pad5A0[0x5BC - 0x5A0];
    f32 dt;  // offset 0x5BC
    u8 pad5C0[0x2690 - 0x5C0];
    f32 boost;  // offset 0x2690
};
extern "C" Truck* fn_8017960C(int index);

static inline ValPoint GetPt(ValueNode* n, const char* k) { return n->GetPoint(k); }
static inline void ScaleV(ZoneVec3* d, const ZoneVec3* s, f32 k) {
    d->x = s->x * k;
    d->y = s->y * k;
    d->z = s->z * k;
}
static inline void Perp(ValPoint* v) {
    f32 t = v->x;
    v->x = v->y;
    v->y = -t;
}
static inline int Round(f32 x) { return x < 0.0f ? x - 0.5f : x + 0.5f; }
static inline int ClampI(int x, int lo, int hi) {
    if (x > lo) {
        lo = x;
    }
    if (lo < hi) {
        hi = lo;
    }
    return hi;
}
inline f32 Max(f32 a, f32 b) { return a > b ? a : b; }
inline f32 Min(f32 a, f32 b) { return a < b ? a : b; }
inline f32 Floor(f32 x) { return floor(x); }
static inline f32 LerpDiffFirst(f32 a, f32 b, f32 t) { return a + (b - a) * t; }
static inline f32 Sum(f32 a, f32 b) { return a + b; }

// One gate: two posts on the ground plane, an optional ring height.
// The end points: retail copies them as one 24-byte block in Gate::operator=, so they form one struct.
struct GateSeg {
    ZoneVec3 a;     // offset 0x0
    ZoneVec3 b;     // offset 0xC
};

class Gate : public GateSeg {
public:
    Gate(void);
    void Init(ValueNode* node, f32 w);
    void Scale(f32 f);
    Gate& operator=(const Gate& o);
    ZoneVec3 Center(void) const { return ZoneVec3(LerpDiffFirst(a.x, b.x, 0.5f), LerpDiffFirst(a.y, b.y, 0.5f), LerpDiffFirst(a.z, b.z, 0.5f)); }

    f32 t;      // offset 0x18
    s16 h;      // offset 0x1C
    u8 ring;    // offset 0x1E
    f32 ringY;  // offset 0x20
};

struct ZoneLevel {
    ~ZoneLevel(void);

    s32 n;         // offset 0x0
    Gate* gates;   // offset 0x4
    f32 timeScale;  // offset 0x8
    s32 mult;      // offset 0xC
    s32 score;     // offset 0x10
};

struct ZoneLevels {
    ZoneLevels(ValueTable* t);
    ~ZoneLevels(void) {
        delete[] levels;
        levels = 0;
    }

    s32 n;              // offset 0x0
    ZoneLevel* levels;  // offset 0x4
};

struct Tune {
    f32 wideOpenSpeedF;      // offset 0x0
    s32 levelupGates;        // offset 0x4
    f32 lowTimeWarnT;        // offset 0x8
    s32 awesomeComboX;       // offset 0xC
    f32 wideOpenIncreaseT;   // offset 0x10
    s32 highestLevel;        // offset 0x14
    f32 bigLifeTimer;        // offset 0x18
    f32 timeBonus;           // offset 0x1C
    f32 timeBonusLen;        // offset 0x20
    f32 timeBonusStartT;     // offset 0x24
    f32 lifeBoostFactor;     // offset 0x28
    s32 basePerfectScore;    // offset 0x2C
    f32 secondaryTimerLen;   // offset 0x30
    f32 gatePenalty;         // offset 0x34
    f32 ringTimeBonus;       // offset 0x38

    f32 BonusAt(f32 t) const {
        f32 v = timeBonus;
        return LerpDiffFirst(v, 0.0f, Clamp01(InvLerp(0.0f, timeBonusLen, t - timeBonusStartT)));
    }
};

// Interval timer on the race clock (0x8001DAE0..): length, start time.
#include "game/timer.h"

template <typename T, int N>
class FixedArray {
public:
    FixedArray(void) {
        data = storage;
        max = N;
        count = 0;
    }
    ~FixedArray(void) {
        data = 0;
        count = 0;
        max = 0;
    }
    void Add(const T& v) {
        T* p = &data[count++];
        *p = v;
    }
    void DeleteAll(void) {
        int i;
        for (i = 0; i < count; i++) {
            delete data[i];
        }
    }
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 max;    // offset 0x8
    T storage[N];
};

extern Tune lbl_8042FB3C;
extern "C" void* fn_802C6C34(int id);
struct Shape {
    u8 pad0[0x2C];
    f32 left;   // offset 0x2C
    f32 right;  // offset 0x30
};

struct Stats {
    s32 mult;     // offset 0x0
    s32 one;      // offset 0x4
    s32 one2;     // offset 0x8
    s32 rating;   // offset 0xC
    s32 perfect;  // offset 0x10
    s32 score;    // offset 0x14
    s32 bonus;    // offset 0x18
};

inline int NearZero(f32 x) {
    f64 absValue = __fabs(x);
    f32 value = (f32)absValue;
    return value <= __float_epsilon[0];
}


extern "C" const f32 lbl_8056149C;  // 5.0: seconds a gate adds

// A car's gate-run state in the zone mode (0x12C); class name unproven. Was named CarState here, the name of
// carstate.cpp's 0x2B4 physics state.
struct ZoneCar {
    ZoneCar(void);
    f32 BonusScale(void) {
        if (tC8.Active()) {
            return LerpDiffFirst(0.0f, 1.0f, Clamp01(InvLerp(0.5f, 1.0f, tC8.Progress())));
        }
        return 1.0f;
    }
    void EndCombo(void) {
        if (combo > 0) {
            lastCombo = combo;
            comboEndT = fn_8013CB3C();
            combo = 0;
        }
    }
    void EndStreak(void) {
        if (streak > 0) {
            tD0.Start();
        }
        streak = 0;
    }
    int AddTime(f32 dt) {
        int r = 1;
        if (lbl_8055E1BC) {
            f32 old = timeLeft;
            timeLeft = Clamp01(old + dt / lbl_8056149C);
            if (old < timeLeft) {
                gainT = timeLeft - old;
            } else if (NearZero(timeLeft) && !(FAbs(old) <= __float_epsilon[0])) {
                EndCombo();
                EndStreak();
                tA8.Start();
                if (!lbl_8055E1BC) {
                    refill = 1;
                }
                r = 0;
            }
        }
        return r;
    }
    void ResetGates(void) {
        gates = 0;
        gate = 0;
        streak = 0;
    }
    void OnGate(void) {
        combo++;
        gates++;
        maxCombo = maxCombo > combo ? maxCombo : combo;
        if (refill) {
            refill = 0;
            AddTime(lbl_8056149C);
        }
        if (combo % lbl_8042FB3C.awesomeComboX == 0) {
            tB8.Start();
        }
    }
    void Miss(void) {
        tA0.Start();
        EndCombo();
        EndStreak();
        if (!lbl_8055E1BC) {
            AddTime(lbl_8056149C);
        }
    }
    void SetWidth(Truck* t) {
        Shape* s = (Shape*)fn_802C6C34(t->model);
        gateWidth = s->right - s->left;
    }

    s32 rating;  // offset 0x0
    s32 i4;      // offset 0x4
    f32 f8;      // offset 0x8
    s32 hits[4];  // offset 0xC
    s32 i1C;      // offset 0x1C
    s32 i20[10];  // offset 0x20
    Stats stats;  // offset 0x48
    Stats prev;   // offset 0x64
    s32 bonus;   // offset 0x80
    f32 bonusT;  // offset 0x84
    s32 total;   // offset 0x88
    s32 i8C;     // offset 0x8C
    u8 b90;      // offset 0x90
    u8 b91;      // offset 0x91
    u8 b92;      // offset 0x92
    f32 timeLeft;  // offset 0x94
    Timer t98;  // offset 0x98
    Timer tA0;  // offset 0xA0
    Timer tA8;  // offset 0xA8
    Timer tB0;  // offset 0xB0
    Timer tB8;  // offset 0xB8
    Timer tC0;  // offset 0xC0
    Timer tC8;  // offset 0xC8
    Timer tD0;  // offset 0xD0
    Timer tD8;  // offset 0xD8
    s32 maxCombo;   // offset 0xE0
    s32 combo;      // offset 0xE4
    s32 gates;      // offset 0xE8
    f32 gainT;      // offset 0xEC
    f32 comboEndT;  // offset 0xF0
    s32 lastCombo;  // offset 0xF4
    u8 refill;      // offset 0xF8
    s32 perfectLevels;  // offset 0xFC
    s32 streak;     // offset 0x100
    Gate* gate;  // offset 0x104
    f32 clock;   // offset 0x108
    s32 score;   // offset 0x10C
    s32 finalScore;  // offset 0x110
    s32 timeBonus;   // offset 0x114
    bool human;  // offset 0x118
    s32 done;    // offset 0x11C
    f32 doneT;   // offset 0x120
    s32 lives;   // offset 0x124
    f32 gateWidth;  // offset 0x128
};

class ISubTaskMgr {
public:
    virtual ~ISubTaskMgr(void) = 0;
    virtual int vf0C(void) = 0;
    virtual int vf10(void) = 0;
    virtual int vf14(int car) = 0;
    virtual void vf18(void) = 0;
    virtual int vf1C(int x) = 0;
    virtual void vf20(void) = 0;
    virtual void vf24(void) = 0;
    virtual int vf28(void) = 0;
};

inline ISubTaskMgr::~ISubTaskMgr(void) {}

class CSubTaskMgr : public ISubTaskMgr {
public:
    virtual ~CSubTaskMgr(void) {}
    virtual int vf0C(void);
    virtual int vf10(void);
    virtual int vf14(int car);
    virtual void vf18(void);
    virtual int vf1C(int x);
    virtual void vf20(void);
    virtual void vf24(void);
    virtual int vf28(void);
};

class Deletable {
public:
    virtual ~Deletable(void);
};

struct GateHit {
    Gate* gate;
    f32 dist;
};

// Plays a list of art animations (artanim.cpp); allocated with the "ArtAnimManager" tag.
struct ArtXform;  // artanim.cpp
#include "game/artplayer.h"

// Result panel animation item.
class AnimItem {
public:
    virtual ~AnimItem(void);
    u8 pad4[0x19 - 0x4];
    u8 active;  // offset 0x19
    u8 pad1A[0x49 - 0x1A];
    u8 play;  // offset 0x49
    u8 b4A;   // offset 0x4A
    u8 b4B;   // offset 0x4B
};

extern "C" const char* fn_80131234(const char* key);


static inline void InitText(char* s) {
    memset(s, 'z', 3);
    s[3] = 0;
}

// Results panel state owned by the master (0x134 in ZoneMaster).
namespace {
struct ZoneHud {
    ZoneHud(void) {
        anim = 0;
        timeBonusAnim = 0;
        noTimeBonusAnim = 0;
        ringAnim = 0;
        outro = 0;
        complete1 = 0;
        complete2 = 0;
        InitText(score);
        InitText(bonus);
        InitText(bonusTitle);
        InitText(finalText);
        InitText(totalText);
        InitText(grade);
        InitText(noBonusText);
        InitText(timeLeft);
        InitText(textC6);
        InitText(textDA);
        InitText(failText);
    }
    ~ZoneHud(void) { Clear(); }
    void Clear(void) {
        if (anim) {
            delete anim;
            anim = 0;
        }
        if (timeBonusAnim) {
            delete timeBonusAnim;
            timeBonusAnim = 0;
        }
        if (noTimeBonusAnim) {
            delete noTimeBonusAnim;
            noTimeBonusAnim = 0;
        }
        if (ringAnim) {
            delete ringAnim;
            ringAnim = 0;
        }
        if (outro) {
            delete outro;
            outro = 0;
        }
        if (complete1) {
            delete complete1;
            complete1 = 0;
        }
        if (complete2) {
            delete complete2;
            complete2 = 0;
        }
    }
    static void Play(GameSound* it) {
        if (it) {
            it->start = 1;
            it->loop = 0;
            it->stop = 0;
        }
    }
    static void Show(GameSound* it) {
        if (it && !it->playing) {
            it->start = 1;
            it->loop = 0;
            it->stop = 0;
        }
    }
    int Update(ZoneCar* c);
    void SetComplete(void) {
        SafeStrCopy(textC6, fn_80131234("ResultsOverlay:CHALLENGE_COMPLETE_1"), 0x14);
        SafeStrCopy(textDA, fn_80131234("ResultsOverlay:CHALLENGE_COMPLETE_2"), 0x14);
        SafeStrCopy(failText, "", 0x14);
    }
    void SetTimedOut(void) {
        SafeStrCopy(failText, fn_80131234("ResultsOverlay:TIMED_OUT"), 0x14);
        SafeStrCopy(textC6, "", 0x14);
        SafeStrCopy(textDA, "", 0x14);
    }
    void ResetState(void) {
        state = 0;
        t = -1.0f;
        b8 = 1;
        b9 = 0;
        noBonus = 0;
        i10C = 0;
        i110 = 0;
        i114 = 0;
        i118 = 0;
        if (anim) {
            ((ArtPlayer*)(anim))->StopAll();
        }
        StopSound(timeBonusAnim);
        StopSound(noTimeBonusAnim);
        StopSound(ringAnim);
        ringShown = 0;
        StopSound(outro);
        StopSound(complete1);
        StopSound(complete2);
    }
    void Reset(void);
    void Init(void);

    u32 state;  // offset 0x0 (0x134)
    // const reader returning int: state is re-read and compared signed here. The name is ours.
    int State(void) const { return state; }
    f32 t;      // offset 0x4
    u8 b8;      // offset 0x8
    u8 b9;      // offset 0x9
    u8 padA[0x2];
    ArtPlayer* anim;  // offset 0xC (0x140)
    u8 noBonus;            // offset 0x10
    u8 pad11[0x3];
    GameSound* timeBonusAnim;    // offset 0x14
    GameSound* noTimeBonusAnim;  // offset 0x18
    GameSound* ringAnim;         // offset 0x1C
    u8 ringShown;               // offset 0x20
    u8 pad21[0x3];
    GameSound* outro;      // offset 0x24
    GameSound* complete1;  // offset 0x28
    GameSound* complete2;  // offset 0x2C
    char score[0x14];        // offset 0x30
    char bonus[0x14];        // offset 0x44
    char bonusTitle[0x14];   // offset 0x58
    char finalText[0x14];    // offset 0x6C
    char totalText[0x14];    // offset 0x80
    char grade[0xA];         // offset 0x94
    char noBonusText[0x14];  // offset 0x9E
    char timeLeft[0x14];     // offset 0xB2
    char textC6[0x14];       // offset 0xC6
    char textDA[0x14];       // offset 0xDA
    char failText[0x14];     // offset 0xEE
    u8 pad102[0x104 - 0x102];
    s32 level;   // offset 0x104
    s32 animId;  // offset 0x108
    s32 i10C;    // offset 0x10C
    s32 i110;    // offset 0x110
    s32 i114;    // offset 0x114
    s32 i118;    // offset 0x118
};
}

// Ring of recent events (0xC bytes each).
struct EventRing {
    EventRing(void) {
        tail = 0;
        cap = 8;
        data = storage;
        head = 0;
    }
    ~EventRing(void) { data = 0; }
    struct Entry {
        int Expired(f32 now) { return now - time >= 1.0f; }
        f32 Age(f32 now, f32 life) { return (now - time) / life; }
        f32 time;   // offset 0x0
        f32 bonus;  // offset 0x4
        Gate* gate;  // offset 0x8
    };
    int Count(void) const { return (cap + (tail - head)) % cap; }
    Entry* Get(int i) const {
        if (i >= Count()) {
            return 0;
        }
        return &data[(head + i + cap) % cap];
    }
    Entry* Push(void) {
        Entry* e = &data[tail];
        tail = (tail + 1) % cap;
        if (tail == head) {
            head = (head + 1) % cap;
        }
        return e;
    }
    Entry* Pop(void) {
        Entry* e = Get(0);
        if (e) {
            head = (head + 1) % cap;
        }
        return e;
    }
    Entry* data;  // offset 0x0
    s32 cap;      // offset 0x4
    s32 tail;     // offset 0x8
    s32 head;     // offset 0xC
    Entry storage[8];
};

template <typename T>
struct Wrap {
    T a;
};


extern "C" int fn_801B8514(void);  // amaster.cpp: nonzero while the race is running

extern "C" Gate* fn_8013D148(ZoneLevel* l, f32 x);
extern "C" u32 fn_8013D18C(u32 i);


#include "game/zoneresults.h"

// Ideal-path walker from the track map unit (0x801352B8..).
struct CenterLine;
extern "C" void fn_801B8048(Respawn* r, LineFollower* it, f32 a, f32 b);  // placecar.cpp

static inline int Boosting(Truck* t) { return !NearZero(t->boost); }

namespace {
class ZoneMaster : public GameMode {
public:
    void MarkHitGates(FixedArray<GateHit, 8>& hits);
public:
    ZoneMaster(RaceSetup* race);
    virtual ~ZoneMaster(void);
    virtual int vf20(int car) const;
    virtual int vf24(void) const;
    virtual void vf44(void);
    virtual void vf4C(int p) const;
    virtual int vf5C(int p) const;
    virtual int vf68(void) const;
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf50(int p);
    virtual void vfB8(ZoneResults* r);
    virtual void vfC0(void);
    virtual void vfC4(void);
    virtual int vf54(int p);
    virtual void* vfE8(void);
    virtual int vfA0(Respawn* r, int idx, int c);
    virtual void vfAC(int car);
    virtual void vfA8(int car, int b);
    virtual int vf8C(int p, int key);
    virtual int vf3C(int p);
    virtual f32 vf38(void);
    virtual int vf64(void);
    virtual void vfF8(void);
    virtual void vfFC(void);

    Gate* NextGate(Gate* g) const {
        Gate* first = CurLevel()->gates;
        if (g == CurLevel()->gates + CurLevel()->n - 1) {
            g = first;
        } else {
            g++;
        }
        return g;
    }
    void DrawNextGate(void);
    void DrawBonus(int p, ZoneCar* c) const;
    void DrawLevel(int p, ZoneCar* c) const;
    void DrawMissed(int p, ZoneCar* c) const;
    void DrawGateStar(int p, ZoneCar* c) const;
    void DrawHud(int p) const;
    void DrawNothing(int p, ZoneCar* c) const;
    void DrawTimer(int p, ZoneCar* c) const;
    void DrawScore(int p, ZoneCar* c) const;
    void DrawMissedFlag(int p, ZoneCar* c) const;
    void DrawGateBonus(int p, ZoneCar* c) const;
    void DrawRing(const Gate* g, int c) const;
    ZoneCar* GetCar(int i) { return &cars[i]; }
    int Paused(f32 now) { return 0; }
    ZoneLevel* CurLevel(void) const { return &levels->levels[level]; }
    Gate* FirstGate(int car) const {
        Truck* t = fn_8017960C(car);
        f32 d = t->path->GetPos()->dist;
        return NextGate(fn_8013D148(CurLevel(), d));
    }
    int NextLevel(void) {
        int r = 0;
        if (level < levels->n - 1) {
            level++;
            t6C.Start();
            i288 = fn_8013D18C(level);
            r = 1;
        }
        return r;
    }
    int MaxBonus(int lvl) const {
        f32 bonus = lbl_8042FB3C.timeBonus * levels->levels[lvl].score;
        return Round(bonus * levels->levels[lvl].mult);
    }
    int TimeBonusPoints(ZoneCar* c) {
        if (lbl_8042FB3C.BonusAt(fn_8013CB3C() - c->bonusT) > 0.0f) {
            return Round(lbl_8042FB3C.timeBonus);
        }
        return 0;
    }
    void ScoreGate(ZoneCar* c) {
        int pts;
        int mx;
        int bonus;
        c->tB0.Start();
        pts = TimeBonusPoints(c);
        mx = MaxBonus(level);
        if (pts == mx) {
            c->rating = 3;
        } else if (pts > 0) {
            c->rating = 2;
        } else {
            c->rating = 1;
        }
        c->stats.score = CurLevel()->score;
        c->stats.mult = CurLevel()->mult;
        c->stats.one = 1;
        if (c->streak == lbl_8042FB3C.levelupGates) {
            c->stats.perfect = (level + 1) * lbl_8042FB3C.basePerfectScore;
        } else {
            c->stats.perfect = 0;
        }
        c->stats.one2 = 1;
        c->stats.rating = c->rating;
        bonus = TimeBonusPoints(c);
        c->stats.bonus = bonus;
        c->bonus = c->stats.mult * c->stats.one * (c->stats.score * c->stats.one2) + (bonus + c->stats.perfect);
        c->bonusT = fn_8013CB3C();
        c->tD8.Start();
        c->tC8.Start();
        c->score += c->bonus;
        c->t98.Start();
        c->AddTime(CurLevel()->timeScale);
        c->hits[c->stats.rating]++;
    }
    void SetTotal(int car) { cars[car].total = cars[car].score; }
    void DrawHits(FixedArray<GateHit, 8>* hits);

    s32 unk4;                                         // offset 0x4
    Wrap<Wrap<FixedArray<ILayout*, 16> > > layouts;  // offset 0x8
    void SaveLayouts(void);
    void ToggleLayoutEdit(void);
    ILayout* mainLyt;     // offset 0x54
    ILayout* closureLyt;  // offset 0x58
    ILayout* resultLyt;   // offset 0x5C
    Deletable* triggers;  // offset 0x60
    ZoneLevels* levels;  // offset 0x64
    s32 level;           // offset 0x68
    Timer t6C;  // offset 0x6C
    Timer t74;  // offset 0x74
    Timer t7C;  // offset 0x7C
    Timer t84;  // offset 0x84
    Timer t8C;  // offset 0x8C
    f32 f94;  // offset 0x94
    f32 f98;  // offset 0x98
    f32 f9C;  // offset 0x9C
    s32 tex[5];      // offset 0xA0
    s32 ringModel;   // offset 0xB4
    ZoneCar* cars;  // offset 0xB8
    s32 nCars;       // offset 0xBC
    Wrap<EventRing> events;  // offset 0xC0
    s32 nHumans;     // offset 0x130
    ZoneHud hud;     // offset 0x134
    f32 f250;         // offset 0x250
    GameSound* prerace;   // offset 0x254
    u8 b258;          // offset 0x258
    u8 pad259[0x268 - 0x259];
    f32 volume;      // offset 0x268
    u8 b26C;         // offset 0x26C
    GameSound* fcount;   // offset 0x270
    GameSound* ringhit;  // offset 0x274
    GameSound* levelup;  // offset 0x278
    GameSound* missflag;  // offset 0x27C
    Glow glow;       // offset 0x280
    s32 i288;        // offset 0x288
    Gate* nextGate;  // offset 0x28C
    f32 nextT;       // offset 0x290
    u8 b294;         // offset 0x294
    u8 pad295[0x298 - 0x295];
};
}

static inline void LoadSound(GameSound*& s, const char* name) {
    s = fn_80207848(name, 1);
    if (!s) {
        fn_802E317C("%s does not exist", name);
    }
}

ModuleTag lbl_8055E1A8;
static inline u32 MakeILPos(u32 seg, u32 frac) { return (seg << 16) | frac; }

u32 lbl_8055E1B0 = MakeILPos(0xFFFF, 0);
u32 lbl_8055E1B4 = MakeILPos(0xFFFF, 0);
static UnkStatic123s lbl_8042FB28;
extern const u32 lbl_8055E1B8 = MakeILPos(0x7F00, 0);
u8 lbl_8055E1BC;
ZoneMaster* lbl_8055E1C0;
static inline u32 DefaultTimerColor() { return 0xFF2A85F0; }
extern const u32 lbl_8055E1C4 = DefaultTimerColor();
Tune lbl_8042FB3C;

extern "C" {

int fn_8013CB10(int p) { return fn_80176A1C() == 2; }

f32 fn_8013CB3C(void) { return lbl_8055E3C8; }

int fn_8013CB44(const char* name, ValueNode* n) { return strcmp(n->GetName(), name) == 0; }

void fn_8013CB90(ArtElem* a) {
    a->pos.x -= a->w / 2;
    a->pos.y -= a->h / 2;
}


/* The 2D helpers below are invented (names unproven); their SHAPES are what retail's code fixes:
   scalar parameters passed by value (arguments evaluated right to left, each expression argument is its own
   object) for the sum of two products and for the squared length, and a V2C built from two differences inside
   the distance helper. See handoff/FACTS.md, toughcases 2026-11-16 (fn_8013CBC0). */
static inline f32 SumProd(f32 a, f32 b, f32 c, f32 d) { return a * b + c * d; }
static inline f32 Dot(const V2& a, const V2& b) { return SumProd(a.x, b.x, a.y, b.y); }
/* Dot of a turned a quarter (a.y, -a.x) with b. */
static inline f32 Cross(const V2& a, const V2& b) { return SumProd(a.y, b.x, -a.x, b.y); }
static inline f32 DistSq(const V2& a, const V2& b) {
    V2C d(a.x - b.x, a.y - b.y);
    return d.x * d.x + d.y * d.y;
}
static inline void AddScaled(V2& d, const V2& v, f32 s) {
    d.x += v.x * s;
    d.y += v.y * s;
}
static inline void Scale(V2& v, f32 s) {
    v.x *= s;
    v.y *= s;
}
static inline void Perp(V2& v) {
    f32 t = v.x;
    v.x = v.y;
    v.y = -t;
}
static inline void Offset(V2& q, const V2& p, const V2& d, f32 s) {
    q.x = p.x + d.x * s;
    q.y = p.y + d.y * s;
}
/* The point a little behind the truck along its heading. */
static inline void TailPoint(V2& q, const V2& p, Truck* t) {
    V2C dir(t->dirX, t->dirZ);
    Offset(q, p, dir, -0.083416760f);
}
static inline void CenterOf(ZoneVec3& out, const Gate* g) {
    out.Set(LerpDiffFirst(g->a.x, g->b.x, 0.5f), LerpDiffFirst(g->a.y, g->b.y, 0.5f), LerpDiffFirst(g->a.z, g->b.z, 0.5f));
}

// Seconds a gate adds (declared extern above). Defined here, between the first function that pools 0.5f and
// fn_8013CBC0: retail holds it as the second .sdata2 entry, separate from the pooled 5.0f literal of the tuning
// loader. Defining it above its users would let the compiler fold it into them.
const f32 lbl_8056149C = 5.0f;

// Did the truck's step (p = predicted position, q = a little behind it) cross the car's gate line?
// 0 = no, 1 = crossed the line outside the posts (or missed the ring), 2 = through the gate.
int fn_8013CBC0(ZoneCar* zm, int car) {
    Gate* g = zm->gate;
    int r = 0;
    Truck* t = fn_8017960C(car);
    ZoneVec3 mid;  // declared ahead of pos: retail's frame has it above
    ZoneVec3 pos;
    V2 p;
    V2 q;
    V2 a;
    V2 b;
    V2 ab;
    f32 dt = t->dt;
    pos.x = t->pos.x + t->vel.x * dt;
    pos.y = t->pos.y + t->vel.y * dt;
    pos.z = t->pos.z + t->vel.z * dt;
    p.x = pos.x;
    p.y = pos.z;
    TailPoint(q, p, t);
    a.x = g->a.x;
    a.y = g->a.z;
    b.x = g->b.x;
    b.y = g->b.z;
    ab.x = a.x - b.x;
    ab.y = a.y - b.y;
    int nearA = DistSq(a, p) < DistSq(b, p);
    f32 wa = nearA ? zm->gateWidth * 0.5f : 0.0f;
    f32 wb = !nearA ? zm->gateWidth * 0.5f : 0.0f;
    f32 len = sqrt(Len2(ab.x, ab.y));
    len = 0.01f > len ? 0.01f : len;
    Scale(ab, 1.0f / len);
    AddScaled(a, ab, 0.65f + wa);
    AddScaled(b, ab, -(0.65f + wb));
    V2 n = ab;
    Perp(n);
    V2 pa;
    pa.x = p.x - a.x;
    pa.y = p.y - a.y;
    V2 qa;
    qa.x = q.x - a.x;
    qa.y = q.y - a.y;
    if (Dot(n, pa) * Dot(n, qa) <= 0.0f) {
        r = 1;
        if (Cross(n, pa) > 0.0f || Cross(n, qa) > 0.0f) {
            V2 pb;
            pb.x = p.x - b.x;
            pb.y = p.y - b.y;
            V2 qb;
            qb.x = q.x - b.x;
            qb.y = q.y - b.y;
            if (Cross(pb, n) > 0.0f || Cross(qb, n) > 0.0f) {
                r = 2;
            }
        }
    }
    if (r == 2 && g->ring) {
        CenterOf(mid, g);
        ZoneVec3 at;
        ZoneVec3 ring;
        f32 half = PSVECDistance((const Vec*)&g->a, (const Vec*)&g->b) / 2.0f;
        ring.y = g->ringY;
        f32 zv = mid.z;
        ring.x = mid.x;
        ring.z = zv;
        f32 dtv = t->dt;
        at.x = t->pos.x + t->vel.x * dtv;
        at.y = t->pos.y + t->vel.y * dtv;
        at.z = t->pos.z + t->vel.z * dtv;
        if (PSVECSquareDistance((const Vec*)&at, (const Vec*)&ring) > half * half) {
            r = 1;
        }
    }
    return r;
}


struct Watcher {
    u8 pad0[0x18];
    ZoneVec3 dir;  // offset 0x18
    ZoneVec3 pos;  // offset 0x24
};

int fn_8013CFF0(const Gate* g, const Watcher* w, f32* out) {
    ZoneVec3 mid = g->Center();
    ZoneVec3 d;
    d.x = mid.x - w->pos.x;
    d.y = mid.y - w->pos.y;
    d.z = mid.z - w->pos.z;
    f32 dist2 = PSVECDotProduct((const Vec*)&d, (const Vec*)&d);
    int r = 0;
    if (dist2 < 275625.0f) {
        r = PSVECDotProduct((const Vec*)&d, (const Vec*)&w->dir) > -0.1f;
        if (!r) {
            d.x = g->b.x - w->pos.x;
            d.y = g->b.y - w->pos.y;
            d.z = g->b.z - w->pos.z;
            r = PSVECDotProduct((const Vec*)&d, (const Vec*)&w->dir) > -0.1f;
        }
        if (r && out) {
            *out = dist2;
        }
    }
    return r;
}

Gate* fn_8013D148(ZoneLevel* l, f32 x) {
    Gate* r = l->gates + l->n - 1;
    int i;
    for (i = 0; i < l->n; i++) {
        if (l->gates[i].t > x) {
            return r;
        }
        r = &l->gates[i];
    }
    return r;
}

u32 fn_8013D18C(u32 i) {
    u32 colors[6] = {0xFF00FF00, 0xFFFF4040, 0xFF00FFFF, 0xFFFFFF00, 0xFFFF00FF, 0xFFFF7F00};
    return colors[i % 6];
}


extern "C" DeityCar* fn_801B2EC0(int i);

// Ring-hit pitch per ring in the current chain.
const f32 lbl_802F93B0[5] = {0.7932f, 0.8904f, 0.9433f, 0.9994f, 1.0f};

int fn_8013D1F0(int i) { return fn_801B2EC0(i)->State()->lap; }

static inline f32 SignF(f32 x) { return x > 0.0f ? 1.0f : -1.0f; }


// Compare two floats: 0 when (nearly) equal, else the sign of a - b.
int fn_8013D224(f32 a, f32 b) {
    f32 d = a - b;
    f32 m = (f32)__fabs(d);
    if (m <= __float_epsilon[0]) {
        return 0;
    }
    return d > 0.0f ? 1.0f : -1.0f;
}

int fn_8013D27C(Gate* a, Gate* b) { return fn_8013D224(a->t, b->t); }

}  // extern "C"

class ZoneSubTask : public CSubTaskMgr {
public:
    virtual ~ZoneSubTask(void) {}
    virtual int vf0C(void) { return 0; }
    virtual int vf10(void) { return 0; }
    virtual int vf14(int car) { return fn_8013D1F0(car) < 10; }
    virtual int vf1C(int x) { return x; }
};

extern "C" CSubTaskMgr* fn_8013D288(void) { return new ("zstm") ZoneSubTask; }

extern Tune lbl_8042FB3C;
extern "C" void fn_8013E3A8(Tune* t, ValueNode* n);

ZoneMaster::ZoneMaster(RaceSetup* race) : t6C(0.8f), t74(0.8f), t7C(1.0f), t84(1.0f), t8C(2.0f) {
    ValueTable* conf;
    int i;
    int n;
    ValueNode* node;
    ValueTable* t;
    const char* file;

    char buf[0x100];
    mainLyt = fn_80145E70("zlmain.val");
    layouts.a.a.Add(mainLyt);
    closureLyt = fn_80145E70("zlclosure.val");
    layouts.a.a.Add(closureLyt);
    resultLyt = fn_80145E70("zlresult.val");
    layouts.a.a.Add(resultLyt);
    tex[0] = fn_802CAC28("zonetex.tex", 0);
    tex[1] = fn_802CAC28("zonetex2.tex", 0);
    tex[2] = fn_802CAC28("zonelowt.tex", 0);
    tex[3] = fn_802CAC28("betatex2.tex", 0);
    tex[4] = fn_802CAC28("zonetex5.tex", 0);
    n = race->cars.count;
    lbl_8055E1C0 = this;
    nCars = n;
    cars = new ("ZoneMaster::tab") ZoneCar[n];
    nHumans = 0;
    for (i = 0; i < nCars; i++) {
        cars[i].human = race->cars.defs[i].type == 0x50;
        if (cars[i].human) {
            nHumans++;
        }
    }
    triggers = 0;
    if (((EventInfo*)GetEvent(fn_801769CC()->event))->node->GetBool("use_triggers", 1)) {
        triggers = (Deletable*)fn_80171698();
    }
    {
        file = ((EventInfo*)GetEvent(fn_801769CC()->event))->node->GetString("zonefile", "zones");
        fn_801345D8(buf, sizeof(buf), "*%s/%s.va0", fn_8000AC2C(), file);
    }
    t = fn_80131810(buf);
    conf = fn_80131810("zoneconf.val");
    if (t) {
        node = 0;
        if (conf && conf->HasChild("config")) {
            node = conf->FindChild("config");
        }
        fn_8013E3A8(&lbl_8042FB3C, node);
        levels = new ("zonelevels") ZoneLevels(t);
        if (conf) {
            fn_80131818(conf);
        }
        fn_80131818(t);
    } else {
        fn_802E317C("zone: no zones file (%s)", buf);
        levels = 0;
    }
    fn_802049DC("Time-Bonus Freeze", &lbl_8042FB3C.timeBonusStartT, 0.5f, 4.0f, 0.1f);
    fn_802049DC("Time-Bonus Fadeout", &lbl_8042FB3C.timeBonusLen, 0.1f, 2.0f, 0.1f);
    fn_802049DC("Time-Bonus X", &lbl_8042FB3C.timeBonus, 0.1f, 3.0f, 0.1f);
    fn_802049DC("Start Time", &lbl_8042FB3C.bigLifeTimer, 2.0f, 60.0f, 2.0f);
    volume = 1.0f;
    fn_801331C4(4, "effects_volume", &volume);
    {
        const char* name = "fcount.sfx";
        LoadSound(fcount, name);
    }
    {
        const char* name = "levelup.sfx";
        LoadSound(levelup, name);
    }
    {
        const char* name = "missflag.sfx";
        LoadSound(missflag, name);
    }
    {
        const char* name = "ringhit.sfx";
        LoadSound(ringhit, name);
    }
    i288 = -1;
    b26C = 0;
    ringModel = fn_802C6FC0("unitring.mod", 0, 0);
    prerace = fn_80207848("chal_prerace.sfx", 1);
    b258 = 0;
}

extern "C" {

int fn_8013DDD4(ValueTable* t, const char* name);
int fn_8013DE7C(ValueNode* t, const char* name);
void fn_8013DF24(Gate* g, Gate* prev, Gate* next);
f32 fn_8013E300(const V2* v);

}  // extern "C"

/* Helper name unproven */
static inline void CopyLevel(ZoneLevel* l, ZoneLevel* prev, ValueNode* child) {
    s32 j;
    l->n = prev->n;
    l->gates = new ("zone[]") Gate[l->n];
    {
        ValPoint pt = child->FindChild("Zone")->GetPoint("tan");
        f32 w = fn_8013E300((V2*)&pt) * 2.0f * fn_80204CFC("zone_gate_scale");
        for (j = 0; j < l->n; j++) {
            l->gates[j] = prev->gates[j];
            l->gates[j].Scale(w);
        }
    }
}

/* Helper name unproven */
static inline void LinkRings(ZoneLevel* l) {
    s32 j;
    for (j = 0; j < l->n; j++) {
        Gate* gt = &l->gates[j];
        if (gt->ring) {
            int nn = l->n;
            Gate* a = 0;
            int k;
            for (k = 1; k < nn; k++) {
                a = &l->gates[(nn + (j - k)) % nn];
                if (!a->ring) {
                    break;
                }
            }
            Gate* b = 0;
            for (k = 1; k < nn; k++) {
                b = &l->gates[(j + (nn + k)) % nn];
                if (!b->ring) {
                    break;
                }
            }
            fn_8013DF24(gt, a, b);
        }
    }
}

/* Helper names unproven (BuildLevel, ReadScore, DefaultScore): the level pointer is each helper's parameter, passed as
   &levels[lvl], so it is numbered as an inline object instead of a function local (retail r22 / r18). */
static inline void BuildLevel(ZoneLevel* l, ZoneLevel* prev, ValueNode* child) {
    s32 j;
    if (prev) {
        CopyLevel(l, prev, child);
    } else {
        int k;
        f32 w;
        ValueNode* g;
        l->n = fn_8013DE7C(child, "Zone");
        l->gates = new ("Zone[]") Gate[l->n];
        w = fn_80204CFC("zone_gate_scale") * child->FindChild("config")->GetFloat("gate_scale", 1.0f);
        k = 0;
        for (j = 0; child->GetChildAt(j) != 0; j++) {
            g = child->GetChildAt(j);
            if (fn_8013CB44("Zone", g)) {
                l->gates[k].Init(g, w);
                k++;
            }
        }
    }
    LinkRings(l);
    l->timeScale = fn_80204CFC("zone_time_scale");
    l->mult = 1;
    l->score = 10;
}
static inline void ReadScore(ZoneLevel* l, ValueNode* sn, int lvl) {
    if (sn) {
        l->mult = sn->GetInt("m", lvl + 1);
        l->score = sn->GetInt("flag", 10);
    } else {
        l->mult = lvl + 1;
        l->score = 10;
    }
    l->score = l->score * fn_80204CFC("score_scale");
}
static inline void DefaultScore(ZoneLevel* l, int lvl) {
    l->mult = lvl + 1;
    l->score = 10;
    l->score = l->score * fn_80204CFC("score_scale");
}
ZoneLevels::ZoneLevels(ValueTable* t) {
    int i;
    int lvl;
    ValueNode* child;
    n = fn_8013DDD4(t, "ZoneLevel");
    levels = new ("ZoneLevel[]") ZoneLevel[n];
    i = 0;
    lvl = 0;
    for (; t->GetChild(i) != 0; i++) {
        child = t->GetChild(i);
        if (fn_8013CB44("ZoneLevel", child)) {
            char buf[32];
            BuildLevel(&levels[lvl], lvl > 0 ? levels : 0, child);
            sprintf(buf, "scoring_level%d", lvl + 1);
            if (t->HasChild(buf)) {
                ReadScore(&levels[lvl], t->FindChild(buf), lvl);
            } else {
                DefaultScore(&levels[lvl], lvl);
            }
            lvl++;
        }
    }
}

extern "C" {

int fn_8013DDD4(ValueTable* t, const char* name) {
    int n = 0;
    int i;
    for (i = 0; t->GetChild(i) != 0; i++) {
        if (fn_8013CB44(name, t->GetChild(i))) {
            n++;
        }
    }
    return n;
}

int fn_8013DE7C(ValueNode* t, const char* name) {
    int n = 0;
    int i;
    for (i = 0; t->GetChildAt(i) != 0; i++) {
        if (fn_8013CB44(name, t->GetChildAt(i))) {
            n++;
        }
    }
    return n;
}

void fn_8013DF24(Gate* g, Gate* prev, Gate* next) {
    ZoneVec3* a = &prev->a;
    ZoneVec3* b = &next->a;
    ZoneVec3* p = &g->a;
    u32 i;
    for (i = 0; i < 2; i++) {
        V2 pa;
        V2 pb;
        V2 d;
        V2 r;
        V2 e;
        f32 len;
        f32 inv;
        f32 t;
        pa.x = a->x;
        pa.y = a->z;
        pb.x = b->x;
        pb.y = b->z;
        d.x = pb.x - pa.x;
        d.y = pb.y - pa.y;
        len = sqrt(d.x * d.x + d.y * d.y);
        inv = 1.0f / len;
        d.x *= inv;
        d.y *= inv;
        r.x = p->x;
        r.y = p->z;
        e.x = r.x - pa.x;
        e.y = r.y - pa.y;
        t = d.x * e.x + d.y * e.y;
        r.x = pa.x + d.x * t;
        r.y = pa.y + d.y * t;
        p->x = r.x;
        p->y = 0.0f;
        p->z = r.y;
        p++;
        a++;
        b++;
    }
}

}  // extern "C"

void Gate::Init(ValueNode* node, f32 w) {
    ValPoint c = GetPt(node, "center");
    ValPoint n = GetPt(node, "tan");
    ValPoint q;
    Perp(&n);
    q.x = c.x + n.x * w;
    q.y = c.y + n.y * w;
    b.y = 0.0f;
    a.y = 0.0f;
    b.x = q.x;
    b.z = q.y;
    q.x = c.x + n.x * -w;
    q.y = c.y + n.y * -w;
    a.x = q.x;
    a.z = q.y;
    fn_801E9DC4(&h);
    ring = node->Has("ring_y");
    if (ring) {
        ringY = node->GetFloat("ring_y");
    }
}

void Gate::Scale(f32 f) {
    f32 d;
    b.y = 0.0f;
    a.y = 0.0f;
    d = PSVECDistance((const Vec*)&a, (const Vec*)&b);
#define s (f / d)
    {
        f32 ay;
        f32 az;
        f32 ax;
        f32 bx;
        f32 by;
        f32 bz;
        bz = b.z;
        az = a.z;
        by = b.y;
        ay = a.y;
        bx = b.x;
        ax = a.x;
        f32 mz = az + (bz - az) * 0.5f;
        f32 my = ay + (by - ay) * 0.5f;
        f32 mx = ax + (bx - ax) * 0.5f;
        ZoneVec3 mid(mx, my, mz);
        a = ZoneVec3(mid.x + s * (ax - mid.x), mid.y + s * (ay - mid.y), mid.z + s * (az - mid.z));
        b = ZoneVec3(mid.x + s * (bx - mid.x), mid.y + s * (by - mid.y), mid.z + s * (bz - mid.z));
    }
#undef s
}

Gate& Gate::operator=(const Gate& o) {
    GateSeg::operator=(o);
    t = o.t;
    h = o.h;
    ring = o.ring;
    ringY = o.ringY;
    return *this;
}

static inline f32 Len2D(f32 x, f32 y) { return sqrt(x * x + y * y); }
extern "C" f32 fn_8013E300(const V2* v) { return Len2D(v->x, v->y); }

Gate::Gate(void) {}

ZoneLevel::~ZoneLevel(void) {
    delete[] gates;
    gates = 0;
}

extern "C" {

void fn_8013E3A8(Tune* t, ValueNode* n) {
    t->awesomeComboX = n ? n->GetInt("awesome_combo_x", 25) : 25;
    t->levelupGates = n ? n->GetInt("levelup_gates", 10) : 10;
    t->wideOpenSpeedF = n ? n->GetFloat("wide_open_speed_f", 0.95f) : 0.95f;
    t->lowTimeWarnT = n ? n->GetFloat("low_time_warn_t", 0.4f) : 0.4f;
    t->wideOpenIncreaseT = n ? n->GetFloat("wide_open_increase_t", 5.0f) : 5.0f;
    t->highestLevel = n ? n->GetInt("highest_level", 10) : 10;
    t->bigLifeTimer = n ? n->GetFloat("big_life_timer", 40.0f) : 40.0f;
    t->timeBonus = n ? n->GetFloat("time_bonus", 1.0f) : 1.0f;
    t->timeBonusStartT = n ? n->GetFloat("time_bonus_start_t", 2.22f) : 2.22f;
    t->timeBonusLen = n ? n->GetFloat("time_bonus_len", 1.0f) : 1.0f;
    t->lifeBoostFactor = n ? n->GetFloat("life_boost_factor", 0.7f) : 0.7f;
    t->secondaryTimerLen = n ? n->GetFloat("secondary_timer_len", 5.0f) : 5.0f;
    t->basePerfectScore = n ? n->GetInt("base_perfect_score", 100000) : 100000;
    t->ringTimeBonus = n ? n->GetFloat("ring_time_bonus", 1.0f) : 1.0f;
    t->gatePenalty = fn_80204CFC("zone_gate_penalty");
    t->basePerfectScore = t->basePerfectScore * fn_80204CFC("score_scale");
    t->timeBonusStartT = -100.0f;
}

}  // extern "C"

ZoneCar::ZoneCar(void)
    : t98(1.0f), tA0(1.25f), tA8(1.25f), tB0(1.25f), tB8(1.65f), tC0(1.0f), tC8(0.5f), tD0(0.5f), tD8(0.5f) {}

ZoneMaster::~ZoneMaster(void) {
    int i;
    if (prerace) {
        delete prerace;
    }
    mrModelUnload(ringModel);
    layouts.a.a.DeleteAll();
    gxForgetTexture(tex[0]);
    gxForgetTexture(tex[1]);
    gxForgetTexture(tex[2]);
    gxForgetTexture(tex[3]);
    gxForgetTexture(tex[4]);
    delete levels;
    levels = 0;
    delete missflag;
    missflag = 0;
    delete levelup;
    levelup = 0;
    delete fcount;
    fcount = 0;
    delete ringhit;
    ringhit = 0;
    if (triggers) {
        delete triggers;
        triggers = 0;
    }
    delete[] cars;
    cars = 0;
    lbl_8055E1C0 = 0;
}

extern "C" void fn_8013EB0C(ZoneMaster* zm, int car) {
    ZoneCar* c = &zm->cars[car];
    if (c->human) {
        c->rating = 1;
    }
}

extern "C" {
int fn_801E866C(int player);
Watcher* fn_802C86DC(void);
void fn_802A9BFC(PlainVec3* a, PlainVec3* b, s32 tex, f32 t, int d, int e, int f, int g);
void fn_801E9E30(s16* h, ZoneVec3* p);
f32 fn_801E7E24(void);
void fn_802981F8(ZoneVec3* p);
}  // extern "C"
void UpdateContinueOverlay(void);
extern "C" {
void fn_80147F30(f32 x, f32 y, f32 w, f32 h, u32 color, int segs, f32 start, f32 step, s32 tex, f32 u0,
                 f32 v0, f32 u1, f32 v1);
void fn_80147EE0(ArtElem* a, f32 s);
void fn_801476D8(ArtElem* a, u32 color, s32 tex);
u32 fn_80147C50(u32 a, u32 b, f32 t);
void fn_801476C4(TextElem* item);
void fn_801479A8(TextElem* orig, TextElem* item, const char* fmt, f32 alpha, ...);
void fn_80147BD0(ArtElem* a, ArtElem* b, int c, int d, s32 tex, f32 alpha);
void fn_8000FF88(u32 color, int size, const char* text, f32 scale, f32 x, f32 y, ...);
}

/* Helper name unproven: the watcher's position. */
static inline ZoneVec3* WatcherPos(void) { return &fn_802C86DC()->pos; }

/* Helper name unproven: the bar drawn above the next gate (endpoints copied as plain vectors, level and texture by value). */
static inline void DrawGateBar(const PlainVec3& from, const PlainVec3& to, const s32& level, const s32& tex, f32 t) {
    PlainVec3 a = from;
    PlainVec3 b = to;
    f32 y = (a.y > b.y ? a.y : b.y) + 10.0f;
    const s32 currentLevel = level;
    int lo;
    int hi;
    b.y = y;
    a.y = y;
    lo = 0;
    hi = 0x48;
    if (currentLevel == lbl_8042FB3C.highestLevel - 1) {
        lo = 0x48;
        hi = 0x72;
    }
    fn_802A9BFC(&a, &b, tex, t, 0, lo, 0x100, hi);
}

void ZoneMaster::DrawNextGate(void) {
    f32 t = fn_8013CB3C() - nextT;
    ZoneCar* c = GetCar(fn_801E866C(0));
    Gate* g = c->gate;
    if (g && lbl_8042FB3C.levelupGates - c->gates < 3) {
        int i;
        while (g->ring) {
            g = NextGate(g);
        }
        for (i = c->gates + 1; i < lbl_8042FB3C.levelupGates; i++) {
            do {
                g = NextGate(g);
            } while (g->ring);
        }
        if (g == nextGate || PSVECSquareDistance((const Vec*)&g->a, (const Vec*)(WatcherPos())) < 40000.0f) {
            if (nextGate != g) {
                nextGate = g;
                nextT = fn_8013CB3C();
            }
            DrawGateBar(g->a, g->b, level, tex[1], t);
        }
    }
}


/* Helper name unproven: handles the gates hit this frame. */
inline void ZoneMaster::MarkHitGates(FixedArray<GateHit, 8>& hits) {
    Gate* gg;
    ZoneVec3* p;
    if (!((f32)__fabs(fn_801E7E24()) <= __float_epsilon[0])) {
        int i;
        for (i = 0; i < hits.count; i++) {
            gg = hits.data[i].gate;
            if (!gg->ring) {
                p = &gg->a;
                u32 k;
                for (k = 0; k < 2; k++) {
                    fn_802981F8(p);
                    p++;
                }
            }
        }
    }
    DrawHits(&hits);
}

void ZoneMaster::vf44(void) {
    ZoneCar* c;
    struct { u32 k; int ok; } sp_;  /* Struct unproven: fields of a small local (MWCC numbers struct fields as temps; structpair.py) */
    int i;
    f32 d2;
    Watcher* w;
    int nRing;
    GateHit* h;
    int nPlain;
    ZoneVec3* p;
    Gate* g;

    FixedArray<GateHit, 8> hits;
    int ci = fn_801E866C(-1);
    c = &cars[ci];
    g = c->gate;
    if (c->human && g) {

        nPlain = 0;
        nRing = 0;
        w = fn_802C86DC();

        for (i = 0; i < 10; i++) {
            sp_.ok = 1;

            if (g->ring && nRing < 5) {
                nRing++;
            } else if (!g->ring && nPlain < 2) {
                nPlain++;
            } else {
                sp_.ok = 0;
            }
            if (sp_.ok && fn_8013CFF0(g, w, &d2)) {
                if (hits.count + 1 > hits.max) {
                    break;
                }
                {

                    p = &g->a;
                    for (sp_.k = 0; sp_.k < 2; sp_.k++) {
                        fn_801E9E30(&g->h, p);
                        p->y += 0.5f;
                        p++;
                    }
                }
                {
                    h = &hits.data[hits.count++];
                    h->gate = g;
                    h->dist = sqrt(d2);
                }
            }
            g = NextGate(g);
        }
    }
    if (hits.count > 0) {
        MarkHitGates(hits);
    }
}

// Stripped by the linker; keeps its constant in place.
extern "C" f32 fn_zone_StubM2(f32 x) { return x * -2.0f; }

extern "C" f32 fn_8013EFB0(f32 x) { return x - 320.0f; }

extern "C" f32 fn_8013EFBC(f32 y) { return 240.0f - y; }

int ZoneMaster::vf5C(int p) const {
    if ((f32)__fabs(f250) <= __float_epsilon[0]) {
        if (hud.b8) {
            vf4C(p);
        }
        if (hud.anim) {
            ((ArtPlayer*)(hud.anim))->Draw(0);
        }
    } else {
        vf4C(p);
    }
    if (vf68()) {
        UpdateContinueOverlay();
    }
    return hud.state != 4;
}

int ZoneMaster::vf68(void) const { return hud.state == 2; }

void ZoneMaster::vf4C(int p) const { DrawHud(p); }

extern "C" void fn_801475D0(ILayout* l, const char* name, const char* fmt, ...);

// Stripped by the linker; keep their literals in place.
extern "C" void fn_zone_TextElem(int len, const char* fmt) {
    fn_802E317C(" oversized text elem (len %d, fmt \"%s\")", len, fmt);
}

extern "C" void fn_zone_WideOpen(ILayout* l, int mode, int mult, f32* out) {
    out[0] = out[1] * 0.25f;
    out[2] = out[3] * 255.0f;
    const char* msg;
    if (mode == 0) {
        msg = "HURRY UP!";
    } else if (mode == 1) {
        msg = "GREAT SPEED";
    } else {
        msg = "WIDE OPEN";
    }
    fn_801475D0(l, "wideopen_msg", "%s", msg);
    fn_801475D0(l, "wideopen_mult", "%d", mult);
}

void ZoneMaster::DrawBonus(int p, ZoneCar* c) const {
    if (c->gate) {
        f32 k = c->BonusScale();
        f32 a = LerpDiffFirst(1.0f, 0.0f, Clamp01(InvLerp(0.0f, lbl_8042FB3C.timeBonusLen,
                                                 fn_8013CB3C() - c->bonusT - lbl_8042FB3C.timeBonusStartT)));
        ArtElem* pie = mainLyt->FindArt("bonus_pie");
        f32 r = pie->w * k;
        f32 x = pie->pos.x;
        f32 y = pie->pos.y;
        int n = a * 360.0f;
        if (n > 0) {
            const f32 degree = 0.017453292f;
            const f32 startAngle = 1.5707964f;
            fn_80147F30(x, y, r, 0.0f, 0xFF00FF00, n, startAngle, degree, tex[4], 0.0f, 0.0f, 1.0f, 1.0f);
            fn_80147F30(x, y, r, 0.0f, 0xFF000000, 1, startAngle, 0.06981317f, tex[4], 0.0f, 0.0f, 1.0f,
                        1.0f);
            f32 angularSpan = degree * n;
            fn_80147F30(x, y, r, 0.0f, 0xFF000000, 1, startAngle + angularSpan, 0.06981317f, tex[4], 0.0f, 0.0f, 1.0f, 1.0f);
        }
        mainLyt->FindArt("score_star_art");
        {
            ArtElem star;
            star = *mainLyt->FindArt("bonus_star_art");
            star.pos.x = pie->pos.x;
            star.pos.y = pie->pos.y;
            fn_8013CB90(&star);
            fn_80147EE0(&star, k);
            fn_801476D8(&star, fn_80147C50(0xFFFFFF, 0xFFFFFFFF, LerpDiffFirst(1.0f, 0.0f, Clamp01(InvLerp(0.2f, 0.0f, a)))),
                        tex[3]);
        }
    }
}

void ZoneMaster::DrawNothing(int p, ZoneCar* c) const {}

extern "C" int fn_8013F438(f32 t) { return LerpDiffFirst(200.0f, 0.0f, Clamp01(InvLerp(0.2f, 0.8f, t))); }

extern "C" void fn_8013F498(f32 t, V2* a, V2* b) {
    f32 s = LerpDiffFirst(150.0f, 50.0f, Clamp01(InvLerp(0.0f, 1.0f, t)));
    f32 h = s / 4;
    V2 c;
    V2 d1;
    V2 d2;
    f32 y = LerpDiffFirst(0.0f, 50.0f, Clamp01(InvLerp(0.0f, 1.0f, t * t)));
    c.x = 0.0f;
    c.y = y;
    d1.x = -s;
    d1.y = h;
    d2.x = s;
    a->x = c.x + d1.x;
    a->y = c.y + d1.y;
    d2.y = -h;
    b->x = c.x + d2.x;
    b->y = c.y + d2.y;
}

// Stripped by the linker; keeps its literals in place.
extern "C" void fn_zone_LevelTitle(ILayout* l) {
    l->Find("level_title");
    l->Find("level_fade_in");
    l->Find("level_number");
    l->Find("level_fade_out");
}

void ZoneMaster::DrawLevel(int p, ZoneCar* c) const {
    if (level > 0) {
        if (t74.Active()) {
            f32 a = LerpDiffFirst(0.0f, 1.0f, Clamp01(InvLerp(0.6f, 1.0f, t74.Progress())));
            TextElem* it = mainLyt->Find("middle_message");
            if (it) {
                TextElem m;
                m = *it;
                m.scale *= 0.3f;
                m.color = 0xFFFFFF;
                fn_801479A8(it, &m, "PERFECT LEVEL", a);
            }
            {
                ArtElem* star = mainLyt->FindArt("perfect_star_art");
                ArtElem* score = mainLyt->FindArt("score_star_art");
                if (star) {
                    ArtElem s;
                    int i;
                    s = *star;
                    s.pos.x -= s.w / 2 * c->prev.perfect;
                    for (i = 0; i < c->prev.perfect; i++) {
                        fn_80147BD0(&s, score, -1, -1, tex[3], a);
                        s.pos.x += s.w;
                    }
                }
            }
        } else if (t6C.Active()) {
            f32 a = LerpDiffFirst(0.0f, 1.0f, Clamp01(InvLerp(0.6f, 1.0f, t6C.Progress())));
            TextElem* it = mainLyt->Find("middle_message");
            if (it) {
                TextElem m;
                m = *it;
                m.scale *= 0.3f;
                m.color = 0xFFFFFF;
                fn_801479A8(it, &m, "LEVEL %d", a, level + 1);
            }
        }
    }
}

extern const f32 lbl_80561564;
extern const f32 lbl_80561568;

void ZoneMaster::DrawMissed(int p, ZoneCar* c) const {
    if (c->tA8.Active()) {
        f32 t = c->tA8.Progress();
        u32 col = (fn_8013F438(t) << 24) | 0xFF7F00;
        V2 a;
        V2 b;
        f32 s;
        fn_8013F498(t, &a, &b);
        s = LerpDiffFirst(1.8f, 1.0f, Clamp01(InvLerp(0.4f, 0.9f, t)));
        fn_8000FF88(col, 12, "TIME MISSED", s, 0.0f, fn_8013EFBC(a.y));
    }
    if (c->timeLeft < lbl_8042FB3C.lowTimeWarnT) {
        f32 f = 1.0f - c->timeLeft / lbl_8042FB3C.lowTimeWarnT;
        const f32 alpha = f * 255.0f;
        const f32 remaining = 1.0f - f;
        fn_80147F30(320.0f, 160.0f, 50.0f, 0.0f, ((int)alpha << 24) | 0xFFFFFF, lbl_80561568 * remaining,
                    1.5707964f, lbl_80561564, tex[2], 0.0f, 0.0f, 1.0f, 1.0f);
    }
}

// Emit the time-warning arc constants after the function, in retail pool order.
const f32 lbl_80561564 = 0.06283186f;
const f32 lbl_80561568 = 100.0f;

void ZoneMaster::DrawGateStar(int p, ZoneCar* c) const {
    if (c->gate && !vf24()) {
        f32 k = c->BonusScale();
        ArtElem* a = mainLyt->FindArt("gate_star_art");
        if (a) {
            ArtElem l;
            l = *a;
            fn_8013CB90(&l);
            fn_80147EE0(&l, k);
            fn_801476D8(&l, -1, tex[3]);
        }
        fn_801476C4(mainLyt->Find("next_gate"));
    }
}

int ZoneMaster::vf24(void) const {
    int n = 0;
    int i;
    for (i = 0; i < nCars; i++) {
        if (cars[i].human && !vf20(i)) {
            if (++n >= nHumans) {
                return 1;
            }
        }
    }
    return 0;
}

int ZoneMaster::vf20(int car) const {
    ZoneCar* c = &cars[car];
    int r = 1;
    if (c->done) {
        if (!t74.Active()) {
            r = 0;
        }
    }
    return r;
}

extern "C" {
void fn_802C8BD0(s32 tex);
int fn_802C8DBC(int n);
void fn_802C8E00(f32 x, f32 y, f32 z, f32 u, f32 v, u32 color, int idx);
void fn_802C8E48(void);
void fn_802C8D74(void);
void fn_80130980(char* buf, int size, int digits, f32 t);
void fn_8014784C(ILayout* l, const char* name, const char* set, int index);
void fn_80147CEC(TextElem* out, TextElem* a, TextElem* b, f32 t);
void fn_801474EC(TextElem* item, const char* fmt, ...);
}  // extern "C"
void UpdateScoreOverlays(int, int);
extern "C" {
void fn_80147ABC(TextElem* a, TextElem* b, const char* fmt, f32 t, ...);
void fn_802C70AC(int model, void* mtx, int a, int b);
void fn_802A96E8(ZoneVec3* pts, int n, int c, s32 lite, s32 glow);
}


static inline f32 Ease(f32 x) {
    if (x < -1.0f) {
        return -1.0f;
    }
    if (x > 1.0f) {
        return 1.0f;
    }
    if (x > 0.0f) {
        return x * (x * -2.0f * x) + x * 3.0f * x;
    }
    return x * (x * -2.0f * x) - x * 3.0f * x;
}

static inline void DrawClock(const ZoneMaster* z, f32 t) {
    char buf[0x14];
    fn_80130980(buf, 0x14, 3, t);
    fn_801475D0(z->mainLyt, "big_timer", "%s", buf);
    fn_8014784C(z->mainLyt, "stop_watch_art", "stop_watch_artset", 0);
}

void ZoneMaster::DrawTimer(int p, ZoneCar* c) const {
    f32 age;
    if (lbl_8055E1BC) {
        fn_802C8BD0(tex[0]);
        if (fn_802C8DBC(0x10)) {
            ArtElem* bar = mainLyt->FindArt("timer_bar");
            f32 x = fn_8013EFB0(bar->pos.x);
            f32 y = fn_8013EFBC(bar->pos.y);
            f32 h = bar->h;
            f32 w = bar->w;
            u32 col;
            fn_802C8E00(x, y, 0.0f, 0.46484375f, 0.98828125f, -1, 0);
            fn_802C8E00(x, y - h, 0.0f, 0.46484375f, 0.98828125f, -1, 0);
            fn_802C8E00(x + w, y, 0.0f, 0.46484375f, 0.98828125f, -1, 1);
            fn_802C8E00(x + w, y - h, 0.0f, 0.46484375f, 0.98828125f, -1, 1);
            col = lbl_8055E1C4;
            fn_802C8E00(x, y, 0.0f, 0.46484375f, 0.98828125f, col, 0);
            fn_802C8E00(x, y - h, 0.0f, 0.46484375f, 0.98828125f, col, 0);
            fn_802C8E00(x + w * c->timeLeft, y, 0.0f, 0.46484375f, 0.98828125f, col, 1);
            fn_802C8E00(x + w * c->timeLeft, y - h, 0.0f, 0.46484375f, 0.98828125f, col, 1);
            fn_802C8E00(x, y, 0.0f, 0.90625f, 0.92578125f, -1, 0);
            fn_802C8E00(x, y - h, 0.0f, 0.90625f, 1.0f, -1, 0);
            fn_802C8E00(14.0f + x, y, 0.0f, 0.9609375f, 0.92578125f, -1, 1);
            fn_802C8E00(14.0f + x, y - h, 0.0f, 0.9609375f, 1.0f, -1, 1);
            fn_802C8E00(x + w - 14.0f, y, 0.0f, 0.9609375f, 0.92578125f, -1, 1);
            fn_802C8E00(x + w - 14.0f, y - h, 0.0f, 0.9609375f, 1.0f, -1, 1);
            fn_802C8E00(x + w, y, 0.0f, 1.0f, 0.92578125f, -1, 1);
            fn_802C8E00(x + w, y - h, 0.0f, 1.0f, 1.0f, -1, 1);
            fn_802C8E48();
        }
        fn_802C8D74();
    } else {
        int i;
        f32 now;
        EventRing::Entry* e;
        DrawClock(this, c->clock);
        if (t7C.Active()) {
            f32 t = Ease(t7C.Progress());
            TextElem* a = mainLyt->Find("timer_increase_0");
            TextElem item;
            fn_80147CEC(&item, a, mainLyt->Find("timer_increase_1"), t);
            fn_801474EC(&item, "+%.0f", f98);
        }
        if (t84.Active()) {
            f32 t = Ease(t84.Progress());
            TextElem* a = mainLyt->Find("timer_decrease_0");
            TextElem item;
            fn_80147CEC(&item, a, mainLyt->Find("timer_decrease_1"), t);
            fn_801474EC(&item, "-%.0f", f9C);
        }
        now = fn_8013CB3C();
        for (i = 0; events.a.Get(i); i++) {
            age = events.a.Get(i)->Age(now, 1.0f);
            ArtElem* s0 = mainLyt->FindArt("ring_bonus_star_0");
            ArtElem* s1 = mainLyt->FindArt("ring_bonus_star_1");
            ArtElem* s2 = mainLyt->FindArt("score_star_art");
            ArtElem* arts[3];
            arts[0] = s0;
            arts[1] = s1;
            arts[2] = s2;
            f32 keys[3] = {0.0f, 0.7f, 1.0f};
            u32 col = -1;
            u32 k;
            f32 u;
            for (k = 1; age > keys[k] && k < 2; k++) {
            }
            if (k == 2) {
                col = 0xFFFFFF;
            }
            fn_80147BD0(arts[k - 1], arts[k], -1, col, tex[3], Ease(LerpDiffFirst(0.0f, 1.0f, Clamp01(InvLerp(keys[k - 1], keys[k], age)))));
        }
    }
}

void ZoneMaster::DrawScore(int p, ZoneCar* c) const {
    int score = c->score - c->timeBonus;
    if (c->t98.Active() && c->bonus > 50) {
        f32 f = 1.0f - c->t98.Progress();
        score = c->score - (int)(c->bonus * f);
    }
    UpdateScoreOverlays(fn_801E866C(p), score);
}

void ZoneMaster::DrawMissedFlag(int p, ZoneCar* c) const {
    if (!c->tA8.Active() && c->tA0.Active()) {
        TextElem* a = mainLyt->Find("missed_start");
        TextElem* b = mainLyt->Find("missed_end");
        f32 t = c->tA0.Progress();
        fn_80147ABC(a, b, a->defText, t * t);
    }
}

void ZoneMaster::DrawGateBonus(int p, ZoneCar* c) const {
    if (c->tD8.Active()) {
        ArtElem* a0;
        ArtElem* a1;
        ArtElem* a2;
        c->tD8.Progress();
        a0 = mainLyt->FindArt("bonus_star_art");
        a1 = mainLyt->FindArt("gate_star_art");
        a2 = mainLyt->FindArt("score_star_art");
        if (a0 && a1 && a2) {
            ArtElem s0;
            ArtElem s1;
            ArtElem s2;
            s0 = *a0;
            s1 = *a1;
            s2 = *a2;
            fn_8013CB90(&s1);
            fn_8013CB90(&s0);
            if (c->stats.bonus > 0) {
                fn_80147BD0(&s0, &s2, -1, 0xFFFFFF, tex[3],
                            LerpDiffFirst(0.0f, 1.0f, Clamp01(InvLerp(0.0f, c->tD8.len / 2, fn_8013CB3C() - c->tD8.start))));
                fn_80147BD0(&s1, &s2, -1, 0xFFFFFF, tex[3],
                            LerpDiffFirst(0.0f, 1.0f,
                                 Clamp01(InvLerp(c->tD8.len / 2, c->tD8.len, fn_8013CB3C() - c->tD8.start))));
            } else {
                fn_80147BD0(&s1, &s2, -1, 0xFFFFFF, tex[3], Clamp01(c->tD8.Progress() * 2.0f));
            }
        }
    }
}

void ZoneMaster::DrawHud(int p) const {
    int i = fn_801E866C(p);
    ZoneCar* c = &cars[i];
    if (!fn_8013CB10(p)) {
        DrawBonus(p, c);
        DrawNothing(p, c);
        DrawLevel(p, c);
        DrawGateStar(p, c);
        DrawTimer(p, c);
        DrawMissed(p, c);
        DrawScore(p, c);
        DrawGateBonus(p, c);
        DrawMissedFlag(p, c);
        if (lbl_8055E1BC) {
            ArtElem* a = mainLyt->FindArt("sudden_death");
            if (a) {
                fn_801476D8(a, -1, tex[0]);
            }
        }
    }
}

struct RingMtx {
    ZoneVec3 x;
    ZoneVec3 y;
    ZoneVec3 z;
    ZoneVec3 pos;
};

void ZoneMaster::DrawRing(const Gate* g, int c) const {
    RingMtx m;
    f32 s;
    {
        ZoneVec3 mid = g->Center();
        m.pos = mid;
    }
    m.x.x = g->a.x - g->b.x;
    m.x.y = g->a.y - g->b.y;
    m.x.z = g->a.z - g->b.z;
    PSVECNormalize((const Vec*)&m.x, (Vec*)&m.x);
    m.y.Set(0.0f, 1.0f, 0.0f);
    PSVECCrossProduct((const Vec*)&m.x, (const Vec*)&m.y, (Vec*)&m.z);
    f32 size = 20.0f;
    s = PSVECDistance((const Vec*)&g->a, (const Vec*)&g->b) / size;
    m.x.x *= s;
    m.x.y *= s;
    m.x.z *= s;
    m.y.x *= s;
    m.y.y *= s;
    m.y.z *= s;
    m.z.x *= s;
    m.z.y *= s;
    m.z.z *= s;
    m.pos.y = g->ringY;
    fn_802C70AC(ringModel, &m, -1, c);
}

void ZoneMaster::DrawHits(FixedArray<GateHit, 8>* __restrict hits) {
    DrawNextGate();
    FixedArray<ZoneVec3, 32> pts;
    int i = hits->count - 1;
    for (; i >= 0; i--) {
        GateHit* data = hits->data;
        Gate* g;
        g = data[i].gate;
        if (g->ring) {
            if (pts.count > 0) {
                glow.Draw(pts.data, pts.count, i288);
                pts.count = 0;
            }
            DrawRing(data[i].gate, 2);
        } else {
            pts.Add(g->a);
            pts.Add(data[i].gate->b);
        }
    }
    if (pts.count > 0) {
        glow.Draw(pts.data, pts.count, i288);
        pts.count = 0;
    }
    for (int j = 0; events.a.Get(j); j++) {
        DrawRing(events.a.Get(j)->gate, 4);
    }
}

extern "C" {
size_t strlen(const char* s);
char* fn_801308F4(char* buf, s32 size, s32 value);
int fn_801609C4(int score);
}
const char* GetGradeDisplayName(int grade);

class LocString;


int ZoneHud::Update(ZoneCar* c) {
    int shown;
    int left;
    int final;
    int total;
    int tb;
    if (!anim) {
        return 0;
    }
    tb = c->timeBonus;
    shown = 0;
    final = c->finalScore;
    left = tb;
    total = c->total;
    int hits = 0;
    for (int k = 1; k < 4; k++) {
        hits += c->hits[k];
    }
    fn_801345D8(score, 0x14, "%d", hits);
    if (tb && t > 0.0f) {
        f32 now = lbl_8055E3C8;
        f32 u = Clamp01(InvLerp(0.0f, 1.0f, now - t));
        shown = LerpDiffFirst(0.0f, tb + 0.5f, u);
        left = tb - shown;
    }
    fn_801345D8(bonus, 0x14, "%d", shown);
    if (noBonus) {
        SafeStrCopy(timeLeft, "", 0x14);
        SafeStrCopy(bonusTitle, "", 0x14);
    } else {
        static LocString caps("ResultsOverlay:TIME_BONUS_CAPS");
        fn_80130980(timeLeft, 0x14, 2, left);
        SafeStrCopy(bonusTitle, caps.Get(), 0x14);
    }
    fn_801345D8(finalText, 0x14, "%d", final);
    fn_801308F4(totalText, 0x14, total);
    fn_801308F4(noBonusText, 0x14, total - tb);
    {
        int g = fn_801609C4(total);
        if (g != -1) {
            SafeStrCopy(grade, GetGradeDisplayName(g), 10);
        } else {
            SafeStrCopy(grade, "-", 10);
        }
    }
    if (i10C) {
        b8 = 0;
    }
    switch (state) {
    case 0:
        ((ArtPlayer*)(anim))->Start(animId);
        if (strlen(failText)) {
            Show(complete2);
        } else {
            Show(complete1);
        }
        if (tb > 0) {
            Play(timeBonusAnim);
        } else {
            Play(noTimeBonusAnim);
        }
        state = 1;
        break;
    case 1:
        if (i110 && t < 0.0f) {
            t = lbl_8055E3C8;
        }
        if (i110 == 2 && !ringShown) {
            ringShown = 1;
            Play(ringAnim);
        }
        if (i114) {
            ((ArtPlayer*)(anim))->Pause(animId);
            state = 2;
        }
        break;
    case 2:
        if (b9) {
            ((ArtPlayer*)(anim))->Resume(animId);
            Play(outro);
            state = 3;
        }
        break;
    case 3:
        if (i118) {
            ((ArtPlayer*)(anim))->Pause(animId);
            state = 4;
        }
        break;
    case 4:
        break;
    }
    ((ArtPlayer*)(anim))->Update();
    return state != 4;
}

extern "C" {
f32 fn_801B306C(void);
}  // extern "C"
int fn_80009C84(void);
extern "C" {
int fn_80179630(int i);
void fn_801CB5DC(int car, int score, int grade);
void fn_801CB648(int car, int a, f32 t);
void* fn_80178C9C(void);
void* fn_802C6C34(int id);
int fn_80204CF0(void);
void fn_801352B8(void* it);
int fn_80135D60(void* it, int index);
void fn_8013530C(void* it, int unk);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_8000FBE4(void);
int fn_80161C38(void);
int fn_80160A74(void);
int fn_80160998(int x);
f32 fn_80147204(TextElem* item, const char* text);
void fn_80130978(char* buf, int size, f32 t);
void qsort(void* base, size_t n, size_t size, int (*cmp)(const void*, const void*));
extern s32 lbl_8055E3C4;
}

extern "C" ILPos fn_80136594(void* f, V2* p);

extern "C" void fn_80141454(void) {}

extern "C" void fn_80141458(void) {}

extern "C" GameMode* fn_8014145C(RaceSetup* race) { return new ("ZoneMaster") ZoneMaster(race); }

Glow::Glow(void) {
    lite = fn_802CAC28("zonelite.tex", 0);
    glow = fn_802CAC28("zoneglow.tex", 0);
}

Glow::~Glow(void) {
    gxForgetTexture(lite);
    gxForgetTexture(glow);
}

void Glow::Draw(ZoneVec3* pts, int n, int c) { fn_802A96E8(pts, n, c, lite, glow); }


// Nothing is inlined into the per-frame update in the original build: every helper it uses is a call to
// a weak copy emitted right after it.
#pragma dont_inline on
inline void ZoneMaster::vf0C(void) {
    f32 t = fn_801B306C();
    if (!b258 && t < 6.75f) {
        b258 = 1;
        if (prerace && !fn_80009C84()) {
            GameSound* p = prerace;
            p->start = 1;
            p->loop = 0;
            p->stop = 0;
        }
    }
    if (!vf24()) {
        int finished;
        ZoneCar* c;
        int i;
        for (i = 0; i < nCars; i++) {
            c = GetCar(i);
            f32 now = fn_8013CB3C();
            if (c->human && c->done != 1 && fn_801B306C() <= 0.0f) {
                if (!Paused(now) && !c->gate && fn_8013D1F0(i) > 0) {
                    c->gate = FirstGate(i);
                    c->SetWidth(fn_8017960C(i));
                }
                finished = 0;
                if (c->gate) {
                    if (c->bonusT < 0.0f) {
                        c->bonusT = now;
                    }
                    if (c->gate->ring) {
                        switch (fn_8013CBC0(c, i)) {
                        case 0:
                            break;
                        case 1:
                            c->gate = NextGate(c->gate);
                            break;
                        case 2: {
                            EventRing::Entry* e = events.a.Push();
                            e->bonus = lbl_8042FB3C.ringTimeBonus;
                            e->time = now;
                            e->gate = c->gate;
                            c->finalScore++;
                            c->score++;
                            c->gate = NextGate(c->gate);
                            int k = ClampI(events.a.Count() - 1, 0, 4);
                            ringhit->SetPitch(lbl_802F93B0[k]);
                            ringhit->Play();
                            break;
                        }
                        }
                    } else {
                        switch (fn_8013CBC0(c, i)) {
                        case 0:
                            break;
                        case 1:
                            c->Miss();
                            c->clock = Max(Min(c->clock, 1.25f), c->clock - lbl_8042FB3C.gatePenalty);
                            c->gate = NextGate(c->gate);
                            c->hits[0]++;
                            if (!c->tA8.Active()) {
                                missflag->Play();
                            }
                            if (t84.Active()) {
                                f9C += lbl_8042FB3C.gatePenalty;
                            } else {
                                t84.Start();
                                f9C = lbl_8042FB3C.gatePenalty;
                            }
                            break;
                        case 2: {
                            f32 add;
                            if (c->gates == c->streak) {
                                if (TimeBonusPoints(c) == MaxBonus(level)) {
                                    c->streak++;
                                    goto counted;
                                }
                            }
                            if (c->streak > 0) {
                                c->tD0.Start();
                            }
                            c->streak = 0;
                        counted:
                            c->OnGate();
                            ScoreGate(c);
                            if (c->gates == lbl_8042FB3C.levelupGates) {
                                int idx = c->gate - CurLevel()->gates;
                                finished = !NextLevel();
                                if (c->streak == lbl_8042FB3C.levelupGates) {
                                    c->perfectLevels++;
                                    t74.Start();
                                }
                                c->prev = c->stats;
                                levelup->Play();
                                c->ResetGates();
                                c->gate = &CurLevel()->gates[idx];
                                c->gate = NextGate(c->gate);
                            } else {
                                c->gate = NextGate(c->gate);
                                fcount->Play();
                            }
                            add = lbl_8042FB3C.lifeBoostFactor * CurLevel()->timeScale;
                            c->clock += add;
                            if (t7C.Active()) {
                                f98 += add;
                            } else {
                                t7C.Start();
                                f98 = add;
                            }
                            break;
                        }
                        }
                    }
                    c->clock = Max(Boosting(fn_8017960C(i)) ? 0.5f : 0.0f, c->clock - 0.016683351f);
                    if (!c->AddTime(-0.016683351f) && !missflag->Playing()) {
                        missflag->Play();
                    }
                    if (!lbl_8055E1BC && NearZero(c->clock)) {
                        c->timeLeft = 0.0f;
                    }
                    if (events.a.Get(0)) {
                        while (events.a.Get(0) && events.a.Get(0)->Expired(now)) {
                            f94 += events.a.Get(0)->bonus;
                            events.a.Pop();
                        }
                        if (f94 > 0.0f) {
                            t8C.Start();
                        }
                    } else if (!t8C.Active()) {
                        f94 = 0.0f;
                    }
                }
                if (t74.Active() && t6C.Active()) {
                    t6C.Start();
                }
                if (finished || NearZero(c->timeLeft) || lbl_8042FB3C.highestLevel == level || c->lives < 0) {
                    if (NearZero(c->timeLeft)) {
                        hud.SetTimedOut();
                    } else {
                        hud.SetComplete();
                    }
                    c->done = 1;
                    c->doneT = now;
                    c->timeBonus = Floor(c->clock);
                    c->score += c->timeBonus;
                    fn_801CB5DC(i, c->score, fn_801609C4(c->score));
                    fn_801CB648(i, 1, c->clock);
                    if (c->human) {
                        SetTotal(i);
                    }
                }
            }
            fn_8013EB0C(this, i);
        }
    } else {
        f32 t = f250 - 0.016683351f;
        f250 = 0.0f > t ? 0.0f : t;
        if ((f32)__fabs(f250) <= __float_epsilon[0]) {
            int car = fn_80179630(0);
            hud.Update(&cars[car]);
        }
    }
}

#pragma dont_inline reset

inline void ZoneMaster::vf10(void) {
    ZoneCar* c;
    void* tr;
    u32 k;
    ZoneVec3* p;
    int lv;
    ZoneLevel* l;
    int j;
    int loff;
    Gate* g;
    lbl_8055E1BC = 0;
    b294 = 0;
    nextGate = 0;
    for (lv = 0, loff = 0; lv < levels->n; loff += sizeof(ZoneLevel), lv++) {
        l = (ZoneLevel*)((u8*)levels->levels + loff);
        for (j = 0; j < l->n; j++) {
            g = &l->gates[j];
            p = &g->a;
            for (k = 0; k < 2; k++) {
                fn_801E9E30(&g->h, p);
                p->y += 0.5f;
                p++;
            }
            {
                ZoneVec3 mid = g->Center();
                V2 q;
                tr = fn_80178C9C();
                q.y = mid.z;
                q.x = mid.x;
                g->t = fn_80136594(tr, &q).Dist();
            }
        }
        qsort(l->gates, l->n, sizeof(Gate), (int (*)(const void*, const void*))fn_8013D27C);
    }
    level = 0;
    t6C.start = -999.0f;
    t74.start = -999.0f;
    t7C.start = -999.0f;
    t84.start = -999.0f;
    t8C.start = -999.0f;
    i288 = fn_8013D18C(0);
    for (int i = 0, off = 0; i < nCars; off += sizeof(cars[0]), i++) {
        c = (ZoneCar*)((u8*)cars + off);
        memset(c->hits, 0, sizeof(c->hits));
        c->tC0.start = -999.0f;
        c->rating = 1;
        c->i4 = 1;
        c->f8 = 0.0f;
        c->i1C = 0;
        c->refill = 0;
        c->bonus = 0;
        c->bonusT = -999.9f;
        for (int n = 0; n < 10; n++) {
            c->i20[n] = 4;
        }
        c->tA0.start = -999.0f;
        c->tA8.start = -999.0f;
        c->tB0.start = -999.0f;
        c->tB8.start = -999.0f;
        c->t98.start = -999.0f;
        c->tC8.start = -999.0f;
        c->tD8.start = -999.0f;
        c->gainT = 0.0f;
        c->total = -1;
        c->i8C = -1;
        c->timeLeft = 1.0f;
        c->combo = 0;
        c->maxCombo = 0;
        c->gates = 0;
        c->gate = 0;
        c->comboEndT = -999.9f;
        c->streak = 0;
        c->perfectLevels = 0;
        c->clock = lbl_8042FB3C.bigLifeTimer;
        c->b92 = 0;
        c->b91 = 0;
        c->b90 = 0;
        c->done = 0;
        c->score = 0;
        c->finalScore = 0;
        c->timeBonus = 0;
        c->lives = 3;
        c->doneT = -999.9f;
    }
    events.a.tail = 0;
    events.a.head = 0;
    hud.ResetState();
    f250 = 1.5f;
    StopSound(prerace);
    b258 = 0;
}

inline void ZoneMaster::vf50(int p) { vf4C(p); }

inline void ZoneMaster::vfB8(ZoneResults* r) {
    int i;
    r->nCars = nCars;
    r->nHumans = nHumans;
    r->done = vf24();
    r->bC = 0;
    for (i = 0; i < nCars; i++) {
        ZoneResults::Entry* e = &r->entries[i];
        e->b10 = 0;
        e->place = fn_801B2EC0(i)->State()->place;
        e->score = cars[i].score;
    }
}

inline void ZoneMaster::vfC0(void) { b26C = 1; }

inline void ZoneMaster::vfC4(void) { b26C = 0; }

inline int ZoneMaster::vf54(int p) { return 0x1008; }

inline void* ZoneMaster::vfE8(void) { return fn_8013D288(); }

inline int ZoneMaster::vfA0(Respawn* r, int idx, int c) {
    LineFollower it;
    CenterLine* path;
    it.SetLine((CenterLine*)fn_80156A40()->GetRacingLine(0));
    path = it.line;
    fn_801B8048(r, &it, 0.0f, path->len[0] - 25.0f - 35.0f);
    ScaleV((ZoneVec3*)&r->vel, (const ZoneVec3*)&r->place.basis[2], 4.4444447f);
    r->mode = 1;
    return 1;
}

inline void ZoneMaster::vfAC(int car) {
    ZoneCar* c = &cars[car];
    if (c->human && vf20(car)) {
        c->gate = 0;
        c->EndCombo();
        c->EndStreak();
    }
    GameMode::vfAC(car);
}

inline void ZoneMaster::vfA8(int car, int b) { vfAC(car); }

inline void ZoneMaster::SaveLayouts(void) {
    for (int i = 0; i < layouts.a.a.count; i++) {
        layouts.a.a.data[i]->Save();
    }
}

inline void ZoneMaster::ToggleLayoutEdit(void) {
    for (int i = 0; i < layouts.a.a.count; i++) {
        layouts.a.a.data[i]->ToggleEdit();
    }
}

inline int ZoneMaster::vf8C(int p, int key) {
    if (key == 's') {
        SaveLayouts();
    } else if (key == 'e') {
        ToggleLayoutEdit();
    } else if (key == 9) {
        unk4 = (unk4 + 1) % 2;
    }
    if (vf68() && key == 'X') {
        b294 = 1;
    }
    if (key == 'r') {
        Truck* t = fn_8017960C(0);
        V2 tan;
        f32 half;
        f32 len;
        fn_802E30C8(";;;ring loc @ %d", lbl_8055E3C4);
        fn_802E30C8("table Zone");
        fn_802E30C8("\tring_y %.2f", t->p60.y);
        fn_802E30C8("\tcenter %.2f,%.2f", t->p60.x, t->p60.z);
        tan.x = t->dirX;
        tan.y = t->dirZ;
        half = PSVECDistance((const Vec*)&levels->levels->gates->a, (const Vec*)&levels->levels->gates->b) * 0.5f;
        f32 tanY = tan.y;
        len = sqrt(tan.x * tan.x + tanY * tan.y);
        f32 mag = (f32)__fabs(len);
        if (!(mag <= __float_epsilon[0])) {
            tan.x *= half / len;
            tan.y *= half / len;
        }
        fn_802E30C8("\ttan %.2f, %.2f", tan.x, tan.y);
        fn_802E30C8("end");
    }
    if (key == 'X') {
        if (vf68()) {
            if (hud.State() == 2) {
                hud.b9 = 1;
            }
        } else if (hud.state != 0) {
            f32 now = lbl_8055E3C8;
            if (hud.State() == 1 && !hud.noBonus && hud.t > 0.0f && now - hud.t < 1.0f) {
                hud.noBonus = 1;
                if (hud.anim) {
                    hud.anim->Rewind();
                }
                StopSound(hud.timeBonusAnim);
                StopSound(hud.noTimeBonusAnim);
                StopSound(hud.complete1);
                StopSound(hud.complete2);
                hud.t = lbl_8055E3C8 - 1.0f;
                hud.b8 = 0;
            }
        }
    }
    return 0;
}

inline int ZoneMaster::vf3C(int p) {
    f32 t = fn_801E7E3C()->time;
    f32 t0 = 6.75f;
    f32 t1 = t0 - 0.8f;
    f32 t2 = t1 - 0.3f;
    f32 t3 = t2 - 0.15f;
    f32 t4 = t3 - 0.8f;
    f32 t5 = t4 - 0.3f;
    TextElem* h0 = mainLyt->Find("intro_head_0");
    TextElem* h1 = mainLyt->Find("intro_head_1");
    if (t < t5) {
        fn_8000FBE4();
    }
    if (!fn_80161C38()) {
        if (t < t0) {
            f32 a = LerpDiffFirst(0.0f, 1.0f, Clamp01(InvLerp(t1, t2, t)));
            TextElem* b0 = mainLyt->Find("star_body_0");
            TextElem* b1 = mainLyt->Find("star_body_1");
            UpdateScoreOverlays(fn_801E866C(p), 0);
            if (h0 && h1 && b0 && b1) {
                char buf[0x14];
                TextElem it;
                fn_801479A8(h0, h1, fn_80131234("PreRaceOverlay:STARS_NEEDED"), Ease(a));
                fn_801308F4(buf, 0x14, fn_80160998(fn_80160A74()));
                it = *b0;
                it.pos.x -= fn_80147204(&it, buf) * 0.5f;
                fn_801479A8(&it, b1, buf, Ease(a));
            }
        }
        if (t < t3) {
            f32 a;
            TextElem* tb;
            TextElem* bt;
            fn_8014784C(mainLyt, "stop_watch_art", "stop_watch_artset", 0);
            a = LerpDiffFirst(0.0f, 1.0f, Clamp01(InvLerp(t4, t5, t)));
            tb = mainLyt->Find("time_body_0");
            bt = mainLyt->Find("big_timer");
            if (h0 && h1 && tb && bt) {
                char buf[0x14];
                TextElem it;
                fn_801479A8(h0, h1, fn_80131234("PreRaceOverlay:TIME_LIMIT"), Ease(a));
                f32 t = GetCar(fn_801E866C(0))->clock;
                fn_80130978(buf, 0x14, t);
                it = *tb;
                it.pos.x += fn_80147204(&it, buf) * 0.5f;
                fn_801479A8(&it, bt, buf, Ease(a));
            }
        }
    }
    return 1;
}

inline f32 ZoneMaster::vf38(void) { return 7.75f; }

inline int ZoneMaster::vf64(void) { return 1; }

#pragma dont_inline on
inline void ZoneMaster::vfF8(void) {
    hud.anim = new ("ArtAnimManager") ArtPlayer(1);
    hud.Init();
    hud.Reset();
}
#pragma dont_inline reset

inline void ZoneHud::Reset(void) { ResetState(); }

struct HudBinding {
    s32 type;
    const char* name;
    void* ptr;
};
// A HUD value binding for ArtPlayer::Add, built by value: the array is zero-filled and each entry copied in.
/* Helper name unproven: the by-value builder is what the target shows (EXACT 2026-11-08); its name is a guess. */
static inline HudBinding Bind(s32 type, const char* name, void* ptr) {
    HudBinding b;
    b.type = type;
    b.name = name;
    b.ptr = ptr;
    return b;
}
extern const HudBinding lbl_802FAA40;  // list terminator

inline void ZoneHud::Init(void) {
    HudBinding b[] = {
        Bind(3, "time_left", &i10C),
        Bind(3, "time_bonus", &i110),
        Bind(3, "grade", &i114),
        Bind(3, "final_score", &i118),
        Bind(1, "gate_count", score),
        Bind(1, "time_bonus", bonus),
        Bind(1, "time_bonus_title_caps", bonusTitle),
        Bind(1, "ring_bonus", finalText),
        Bind(1, "final_score", totalText),
        Bind(1, "grade", grade),
        Bind(1, "final_score_no_time_bonus", noBonusText),
        Bind(1, "time_left", timeLeft),
        Bind(1, "challenge_complete_1", textC6),
        Bind(1, "challenge_complete_2", textDA),
        Bind(1, "out_of_time_message", failText),
        Bind(2, "difficulty_level", &level),
        lbl_802FAA40,
    };
    SafeStrCopy(noBonusText, "100", 0x14);
    SafeStrCopy(bonus, "50", 0x14);
    SafeStrCopy(totalText, "150", 0x14);
    fn_80130980(timeLeft, 0x14, 2, 15.0f);
    level = fn_80204CF0() + 1;
    animId = ((ArtPlayer*)(anim))->Add("a_zone_finish.val", (int)(b));
    timeBonusAnim = fn_80207848("pr_gatechal1.sfx", 1);
    noTimeBonusAnim = fn_80207848("pr_gatechal1_nobonus.sfx", 1);
    ringAnim = fn_80207848("pr_gatechal2.sfx", 1);
    outro = fn_80207848("pr_transition.sfx", 1);
    complete1 = fn_80207848("levelup.sfx", 1);
    complete2 = fn_80207848("timeout.sfx", 1);
}

inline void ZoneMaster::vfFC(void) { hud.Clear(); }
