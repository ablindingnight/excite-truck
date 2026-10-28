// Gate challenge ("zone") mode (0x8013CB10-0x801444C4): ZoneMaster drives the timed ring/gate
// challenge - levels of gates read from the event's zones file, crossing tests per car, the timer,
// time bonuses, the HUD layouts and the pre-race and results overlays. Names are guesses apart from
// the allocation tags, keys and messages.
#include <math.h>
#include "game/common.h"

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
void fn_802C6CF8(int model);
s32 fn_802CAC28(const char* name, int flags);  // load texture
void fn_802CAC54(s32 tex);
int fn_80176A1C(void);
void fn_801E9DC4(s16* h);
void* fn_80171698(void);
}

struct Vec3 {
    Vec3(void) {}
    Vec3(f32 x, f32 y, f32 z) : x(x), y(y), z(z) {}
    void Set(f32 x, f32 y, f32 z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    Vec3 operator+(const Vec3& b) const { return Vec3(x + b.x, y + b.y, z + b.z); }
    Vec3 operator-(const Vec3& b) const { return Vec3(x - b.x, y - b.y, z - b.z); }
    Vec3 operator*(f32 s) const { return Vec3(x * s, y * s, z * s); }
    f32 x;
    f32 y;
    f32 z;
};

extern "C" {
f32 PSVECDotProduct(const Vec3* a, const Vec3* b);
f32 PSVECDistance(const Vec3* a, const Vec3* b);
f32 PSVECSquareDistance(const Vec3* a, const Vec3* b);
}

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);
extern "C" f32 fn_8013CB3C(void);
extern u8 lbl_8055E1BC;  // timed-out-of-zone mode
void* GetEvent(int event);

extern f32 lbl_8055E3C8;     // current race time
extern f32 lbl_80558DC4[];  // FLT_EPSILON (MSL)

// One text element of a layout set (0x20 bytes).
struct LayoutItem {
    u32 unk0;
    struct {
        f32 x;
        f32 y;
    } pos;      // offset 0x4
    u32 unkC;   // offset 0xC
    f32 scale;  // offset 0x10
    u32 unk14;  // offset 0x14
    u32 color;  // offset 0x18
    u32 unk1C;  // offset 0x1C
};

// One art (textured quad) element of a layout set (0x28 bytes).
struct LayoutArt {
    u32 unk0;
    struct {
        f32 x;
        f32 y;
    } pos;   // offset 0x4
    u32 unkC;
    f32 w;   // offset 0x10
    f32 h;   // offset 0x14
    f32 u0;  // offset 0x18
    f32 v0;  // offset 0x1C
    f32 u1;  // offset 0x20
    f32 v1;  // offset 0x24
};

class Layout {
public:
    virtual ~Layout(void);
    virtual LayoutItem* Find(const char* name);    // 0xC
    virtual LayoutArt* FindArt(const char* name);  // 0x10
    virtual void vf14(void);
    virtual void vf18(void);
    virtual void vf1C(void);
    virtual void vf20(void);
    virtual void vf24(void);
    virtual void vf28(void);
};
extern "C" Layout* fn_80145E70(const char* name);

// A loaded sound effect; playback flags are polled by the sound system.
class Sound {
public:
    virtual ~Sound(void);
    void Play(void) {
        play = 1;
        b4B = 0;
        b4A = 0;
    }
    void SetPitch(f32 f) {
        if (f != pitch) {
            pitch = f;
            dirty = 1;
        }
    }
    int Active(void) { return active; }

    f32 pitch;  // offset 0x4
    u8 pad8[0x19 - 0x8];
    u8 active;  // offset 0x19
    u8 pad1A[0x49 - 0x1A];
    u8 play;   // offset 0x49
    u8 b4A;    // offset 0x4A
    u8 b4B;    // offset 0x4B
    u8 b4C;    // offset 0x4C
    u8 pad4D;
    u8 dirty;  // offset 0x4E
};

static inline void StopSound(Sound* s) {
    if (s) {
        s->b4A = 1;
        s->b4C = 0;
    }
}
extern "C" Sound* fn_80207848(const char* name, int flags);

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
    Vec3 vel;  // offset 0x54
    Vec3 p60;  // offset 0x60
    u8 pad6C[0x94 - 0x6C];
    Vec3 pos;  // offset 0x94
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

struct LocalCarDesc {
    s32 event;  // offset 0x0
};
extern "C" LocalCarDesc* fn_801769CC(void);

struct EventDesc {
    u8 pad0[0x20];
    ValueNode* vals;  // offset 0x20
};

struct RaceCar {
    u8 pad0[0x20];
    s32 kind;  // offset 0x20
    u8 pad24[0x78 - 0x24];
};

struct Race {
    RaceCar cars[8];
    u8 pad3C0[0x3E0 - 0x3C0];
    s32 nCars;  // offset 0x3E0
};

static inline ValPoint GetPt(ValueNode* n, const char* k) { return n->GetPoint(k); }
static inline void ScaleV(Vec3* d, const Vec3* s, f32 k) {
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
f32 Max(f32 a, f32 b) { return a > b ? a : b; }
f32 Min(f32 a, f32 b) { return a < b ? a : b; }
inline f32 Floor(f32 x) { return floor(x); }
static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + (b - a) * t; }
static inline f32 InvLerp(f32 a, f32 b, f32 x) { return (x - a) / (b - a); }
static inline f32 Clamp01(f32 x) {
    f32 r = 1.0f < x ? 1.0f : x;
    return r > 0.0f ? r : 0.0f;
}
static inline f32 Sq(f32 x) { return x * x; }
static inline f32 Sum(f32 a, f32 b) { return a + b; }

struct V2 {
    f32 x;
    f32 y;
};

// One gate: two posts on the ground plane, an optional ring height.
class Gate {
public:
    Gate(void);
    void Init(ValueNode* node, f32 w);
    void Scale(f32 f);
    Gate& operator=(const Gate& o);
    Vec3 Center(void) const { return Vec3(a.x + 0.5f * (b.x - a.x), a.y + 0.5f * (b.y - a.y), a.z + 0.5f * (b.z - a.z)); }

    Vec3 a;     // offset 0x0
    Vec3 b;     // offset 0xC
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
        return Lerp(v, 0.0f, Clamp01(InvLerp(0.0f, timeBonusLen, t - timeBonusStartT)));
    }
};

// Interval timer on the race clock (0x8001DAE0..): length, start time.
class Timer {
public:
    Timer(f32 len);
    f32 Now(void);
    f32 Frac(void);

    void Start(void) { start = Now(); }
    int Active(void) {
        f32 t = Now() - start;
        return t >= 0.0f && t < len;
    }

    f32 len;    // offset 0x0
    f32 start;  // offset 0x4
};

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
    void Add(T v) {
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

static inline int NearZero(f32 x) { return (f32)__fabs(x) <= lbl_80558DC4[0]; }

struct CarState {
    CarState(void);
    f32 BonusScale(void) {
        if (tC8.Active()) {
            return Lerp(0.0f, 1.0f, Clamp01(InvLerp(0.5f, 1.0f, tC8.Frac())));
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
            timeLeft = Clamp01(old + dt / 5.0f);
            if (old < timeLeft) {
                gainT = timeLeft - old;
            } else if (NearZero(timeLeft) && !((f32)__fabs(old) <= lbl_80558DC4[0])) {
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
            AddTime(5.0f);
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
            AddTime(5.0f);
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

struct RacerInfo {
    u8 pad0[0x4];
    s32 place;  // offset 0x4
    u8 pad8[0x10 - 0x8];
    s32 rank;  // offset 0x10
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
class ArtPlayer {
public:
    ArtPlayer(int n);
    ~ArtPlayer(void);
    int Add(const char* name, int bindings);
    void Start(int i);
    void Pause(int i);
    void Resume(int i);
    void StopAll(void);
    void Update(void);
    void Draw(int flags);

    void* list;  // offset 0x0
    f32 t;       // offset 0x4
};

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

static inline void StrCopyN(const char* src, int n, char* dst) {
    char* d = dst;
    int left = n;
    while ((*d++ = *src++) != 0) {
        if (--left <= 0) {
            d[-1] = 0;
            break;
        }
    }
}

static inline void InitText(char* s) {
    memset(s, 'z', 3);
    s[3] = 0;
}

// Results panel state owned by the master (0x134 in ZoneMaster).
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
    static void Play(Sound* it) {
        if (it) {
            it->play = 1;
            it->b4B = 0;
            it->b4A = 0;
        }
    }
    static void Show(Sound* it) {
        if (it && !it->active) {
            it->play = 1;
            it->b4B = 0;
            it->b4A = 0;
        }
    }
    int Update(CarState* c);
    void SetComplete(void) {
        StrCopyN(fn_80131234("ResultsOverlay:CHALLENGE_COMPLETE_1"), 0x14, textC6);
        StrCopyN(fn_80131234("ResultsOverlay:CHALLENGE_COMPLETE_2"), 0x14, textDA);
        StrCopyN("", 0x14, failText);
    }
    void SetTimedOut(void) {
        StrCopyN(fn_80131234("ResultsOverlay:TIMED_OUT"), 0x14, failText);
        StrCopyN("", 0x14, textC6);
        StrCopyN("", 0x14, textDA);
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

    s32 state;  // offset 0x0 (0x134)
    f32 t;      // offset 0x4
    u8 b8;      // offset 0x8
    u8 b9;      // offset 0x9
    u8 padA[0x2];
    ArtPlayer* anim;  // offset 0xC (0x140)
    u8 noBonus;            // offset 0x10
    u8 pad11[0x3];
    Sound* timeBonusAnim;    // offset 0x14
    Sound* noTimeBonusAnim;  // offset 0x18
    Sound* ringAnim;         // offset 0x1C
    u8 ringShown;               // offset 0x20
    u8 pad21[0x3];
    Sound* outro;      // offset 0x24
    Sound* complete1;  // offset 0x28
    Sound* complete2;  // offset 0x2C
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
        f32 time;   // offset 0x0
        f32 bonus;  // offset 0x4
        Gate* gate;  // offset 0x8
    };
    int Count(void) { return (cap + (tail - head)) % cap; }
    Entry* Get(int i) {
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

class Glow {
public:
    Glow(void);
    ~Glow(void);
    void Draw(Vec3* pts, int n, int c);
    s32 lite;  // offset 0x0
    s32 glow;  // offset 0x4
};

class GameMode {
public:
    virtual ~GameMode(void) {}
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(int p) {}
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
    virtual void vf58(void);
    virtual int vf5C(int p);
    virtual void vf60(void);
    virtual int vf64(void);
    virtual int vf68(void);
    virtual void vf6C(void) {}
    virtual void vf70(void) {}
    virtual void vf74(void) {}
    virtual void vf78(void) {}
    virtual int vf7C(int p) { return 0; }
    virtual int vf80(void) { return 0; }
    virtual int vf84(void) { return 0; }
    virtual int vf88(void) { return 0; }
    virtual int vf8C(int p, int key);
    virtual void vf90(int a, int b, int c, int d) {}
    virtual void vf94(int p) {}
    virtual void vf98(void);
    virtual void vf9C(int p, int type) {}
    virtual int vfA0(struct Cam* cam);
    virtual void vfA4(int p) {}
    virtual void vfA8(int car);
    virtual void vfAC(int car);
    virtual int vfB0(int a, int b, int c) { return 0; }
    virtual void vfB4(int car, struct Damage* d) {}
    virtual void vfB8(struct ZoneResults* r) = 0;
    virtual int vfBC(void) { return 0; }
    virtual void vfC0(void);
    virtual void vfC4(void);
    virtual void vfC8(void) {}
    virtual int vfCC(void) { return 1; }
    virtual int vfD0(void) { return 0; }
    virtual int vfD4(int car) {
        int r = 0;
        if (IsRacing(car) && !vf20(car)) {
            r = 1;
        }
        return r;
    }
    virtual int vfD8(void) { return 0; }
    virtual int vfDC(void) { return 1; }
    virtual int vfE0(void) { return 0; }
    virtual int vfE4(void) { return 0; }
    virtual CSubTaskMgr* vfE8(void);
    virtual int vfEC(void);
    virtual int vfF0(void) { return 0; }
    virtual void vfF4(void) {}
    virtual void vfF8(void);
    virtual void vfFC(void);

    int IsRacing(int car);  // 0x801B8514

    u32 unk4;  // offset 0x4
};

extern "C" Gate* fn_8013D148(ZoneLevel* l, f32 x);
extern "C" u32 fn_8013D18C(u32 i);

struct Cam {
    u8 pad0[0x18];
    Vec3 v18;  // offset 0x18
    Vec3 v24;
    Vec3 v30;  // offset 0x30
    u8 pad3C[0x44 - 0x3C];
    s32 i44;  // offset 0x44
};

struct ZoneResults {
    struct Entry {
        u8 pad0[0x4];
        s32 place;  // offset 0x4
        s32 score;  // offset 0x8
        u8 padC[0x10 - 0xC];
        u8 b10;     // offset 0x10
    };
    s32 nCars;    // offset 0x0
    s32 nHumans;  // offset 0x4
    u8 pad8[0x4];
    u8 bC;    // offset 0xC
    u8 done;  // offset 0xD
    u8 padE[0x2];
    Entry entries[8];  // offset 0x10
};

struct Path {
    u8 pad0[0x8];
    f32 len;  // offset 0x8
};

// Ideal-path walker from the track map unit (0x801352B8..).
struct PathIter {
    PathIter(void);
    ~PathIter(void);
    int Set(int index);
    u8 pad0[0x2C];
    Path* path;  // offset 0x2C
    u8 pad30[0x38 - 0x30];
};

class PathSet {
public:
    virtual void vf08(void);
    virtual void* Get(int index);  // 0xC
};

struct RaceClock {
    u8 pad0[0x10];
    f32 t;  // offset 0x10
};

extern "C" void PlaceCamera(Cam* cam, PathIter* it, f32 a, f32 b);

static inline int Boosting(Truck* t) { return !NearZero(t->boost); }

class ZoneMaster : public GameMode {
public:
    ZoneMaster(Race* race);
    virtual ~ZoneMaster(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual int vf20(int car);
    virtual int vf24(void);
    virtual f32 vf38(void);
    virtual int vf3C(int p);
    virtual void vf44(void);
    virtual void vf4C(int p);
    virtual void vf50(int p);
    virtual int vf54(void);
    virtual int vf5C(int p);
    virtual int vf64(void);
    virtual int vf68(void);

    Gate* NextGate(Gate* g) const {
        ZoneLevel* l = &levels->levels[level];
        if (g == l->gates + l->n - 1) {
            return l->gates;
        }
        return g + 1;
    }
    void DrawNextGate(void);
    void DrawBonus(int p, CarState* c);
    void DrawLevel(int p, CarState* c);
    void DrawMissed(int p, CarState* c);
    void DrawGateStar(int p, CarState* c);
    void DrawHud(int p);
    void DrawNothing(int p, CarState* c);
    void DrawTimer(int p, CarState* c);
    void DrawScore(int p, CarState* c);
    void DrawMissedFlag(int p, CarState* c);
    void DrawGateBonus(int p, CarState* c);
    void DrawRing(Gate* g, int c);
    CarState* GetCar(int i) { return &cars[i]; }
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
        ZoneLevel* l = &levels->levels[lvl];
        return Round(lbl_8042FB3C.timeBonus * l->score * l->mult);
    }
    int TimeBonusPoints(CarState* c) {
        if (lbl_8042FB3C.BonusAt(fn_8013CB3C() - c->bonusT) > 0.0f) {
            return Round(lbl_8042FB3C.timeBonus);
        }
        return 0;
    }
    void ScoreGate(CarState* c) {
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
    virtual int vf8C(int p, int key);
    virtual int vfA0(Cam* cam);
    virtual void vfA8(int car);
    virtual void vfAC(int car);
    virtual void vfB8(ZoneResults* r);
    virtual void vfC0(void);
    virtual void vfC4(void);
    virtual CSubTaskMgr* vfE8(void);
    virtual void vfF8(void);
    virtual void vfFC(void);

    Wrap<Wrap<FixedArray<Layout*, 16> > > layouts;  // offset 0x8
    Layout* mainLyt;     // offset 0x54
    Layout* closureLyt;  // offset 0x58
    Layout* resultLyt;   // offset 0x5C
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
    CarState* cars;  // offset 0xB8
    s32 nCars;       // offset 0xBC
    Wrap<EventRing> events;  // offset 0xC0
    s32 nHumans;     // offset 0x130
    ZoneHud hud;     // offset 0x134
    f32 f250;         // offset 0x250
    Sound* prerace;   // offset 0x254
    u8 b258;          // offset 0x258
    u8 pad259[0x268 - 0x259];
    f32 volume;      // offset 0x268
    u8 b26C;         // offset 0x26C
    Sound* fcount;   // offset 0x270
    Sound* ringhit;  // offset 0x274
    Sound* levelup;  // offset 0x278
    Sound* missflag;  // offset 0x27C
    Glow glow;       // offset 0x280
    s32 i288;        // offset 0x288
    Gate* nextGate;  // offset 0x28C
    f32 nextT;       // offset 0x290
    u8 b294;         // offset 0x294
    u8 pad295[0x298 - 0x295];
};

static inline void LoadSound(Sound*& s, const char* name) {
    s = fn_80207848(name, 1);
    if (!s) {
        fn_802E317C("%s does not exist", name);
    }
}

class Color {
public:
    Color(u32 c) { rgba = c; }
    u32 rgba;
};

ModuleTag lbl_8055E1A8;
UnkStaticFFFF lbl_8055E1B0;
UnkStaticFFFF lbl_8055E1B4;
static UnkStatic123s lbl_8042FB28;
UnkStatic7F lbl_8055E1B8;
u8 lbl_8055E1BC;
ZoneMaster* lbl_8055E1C0;
Color lbl_8055E1C4(0xFF2A85F0);
Tune lbl_8042FB3C;

extern "C" {

int fn_8013CB10(int p) { return fn_80176A1C() == 2; }

f32 fn_8013CB3C(void) { return lbl_8055E3C8; }

int fn_8013CB44(const char* name, ValueNode* n) { return strcmp(n->GetName(), name) == 0; }

void fn_8013CB90(LayoutArt* a) {
    a->pos.x -= a->w / 2;
    a->pos.y -= a->h / 2;
}


int fn_8013CBC0(CarState* zm, int car) {
    Gate* g = zm->gate;
    int r = 0;
    Truck* t = fn_8017960C(car);
    Vec3 pos;
    V2 p;
    V2 q;
    V2 a;
    V2 b;
    V2 ab;
    V2 perp;
    V2 pa;
    V2 qa;
    V2 pb;
    V2 qb;
    V2 dir;
    V2 ap;
    V2 bp;
    f32 wa;
    f32 wb;
    f32 len;
    f32 c0;
    f32 c1;
    int nearA;
    pos.x = t->pos.x + t->vel.x * t->dt;
    pos.y = t->pos.y + t->vel.y * t->dt;
    pos.z = t->pos.z + t->vel.z * t->dt;
    p.x = pos.x;
    p.y = pos.z;
    dir.x = t->dirX;
    dir.y = t->dirZ;
    q.x = p.x + dir.x * -0.083416760f;
    q.y = p.y + dir.y * -0.083416760f;
    a.x = g->a.x;
    a.y = g->a.z;
    b.x = g->b.x;
    b.y = g->b.z;
    ab.x = a.x - b.x;
    ab.y = a.y - b.y;
    bp.x = b.x - p.x;
    bp.y = b.y - p.y;
    ap.x = a.x - p.x;
    ap.y = a.y - p.y;
    nearA = ap.x * ap.x + ap.y * ap.y < bp.x * bp.x + bp.y * bp.y;
    wa = nearA ? zm->gateWidth * 0.5f : 0.0f;
    wb = !nearA ? zm->gateWidth * 0.5f : 0.0f;
    len = sqrt(ab.x * ab.x + ab.y * ab.y);
    len = 0.01f > len ? 0.01f : len;
    ab.x *= 1.0f / len;
    ab.y *= 1.0f / len;
    a.x += ab.x * (0.65f + wa);
    a.y += ab.y * (0.65f + wa);
    perp.x = ab.y;
    perp.y = -ab.x;
    b.x += ab.x * -(0.65f + wb);
    b.y += ab.y * -(0.65f + wb);
    pa.x = p.x - a.x;
    pa.y = p.y - a.y;
    qa.x = q.x - a.x;
    qa.y = q.y - a.y;
    c0 = perp.x * pa.x + perp.y * pa.y;
    c1 = perp.x * qa.x + perp.y * qa.y;
    if (c0 * c1 <= 0.0f) {
        r = 1;
        if (perp.y * pa.x + -perp.x * pa.y > 0.0f || perp.y * qa.x + -perp.x * qa.y > 0.0f) {
            pb.x = p.x - b.x;
            pb.y = p.y - b.y;
            qb.x = q.x - b.x;
            qb.y = q.y - b.y;
            if (pb.y * perp.x + -pb.x * perp.y > 0.0f || qb.y * perp.x + -qb.x * perp.y > 0.0f) {
                r = 2;
            }
        }
    }
    if (r == 2 && g->ring) {
        Vec3 mid;
        Vec3 at;
        Vec3 ring;
        f32 half;
        mid.x = g->a.x + (g->b.x - g->a.x) * 0.5f;
        mid.y = g->a.y + (g->b.y - g->a.y) * 0.5f;
        mid.z = g->a.z + (g->b.z - g->a.z) * 0.5f;
        half = PSVECDistance(&g->a, &g->b) * 0.5f;
        ring.x = mid.x;
        ring.y = g->ringY;
        ring.z = mid.z;
        at.x = t->pos.x + t->vel.x * t->dt;
        at.y = t->pos.y + t->vel.y * t->dt;
        at.z = t->pos.z + t->vel.z * t->dt;
        if (PSVECSquareDistance(&at, &ring) > half * half) {
            r = 1;
        }
    }
    return r;
}

struct Watcher {
    u8 pad0[0x18];
    Vec3 dir;  // offset 0x18
    Vec3 pos;  // offset 0x24
};

int fn_8013CFF0(const Gate* g, const Watcher* w, f32* out) {
    Vec3 mid = g->Center();
    Vec3 d(mid.x - w->pos.x, mid.y - w->pos.y, mid.z - w->pos.z);
    f32 dist2 = PSVECDotProduct(&d, &d);
    int r = 0;
    if (dist2 < 275625.0f) {
        r = PSVECDotProduct(&d, &w->dir) > -0.1f;
        if (!r) {
            d.x = g->b.x - w->pos.x;
            d.y = g->b.y - w->pos.y;
            d.z = g->b.z - w->pos.z;
            r = PSVECDotProduct(&d, &w->dir) > -0.1f;
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


class Racer {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual RacerInfo* GetInfo(void);
};
extern "C" Racer* fn_801B2EC0(int i);

// Ring-hit pitch per ring in the current chain.
const f32 lbl_802F93B0[5] = {0.7932f, 0.8904f, 0.9433f, 0.9994f, 1.0f};

int fn_8013D1F0(int i) { return fn_801B2EC0(i)->GetInfo()->rank; }

static inline f32 SignF(f32 x) { return x > 0.0f ? 1.0f : -1.0f; }

extern const f32 lbl_805614A4;  // 0.0
extern const f32 lbl_805614AC;  // 1.0
extern const f32 lbl_805614BC;  // -1.0

// Compare two floats: 0 when (nearly) equal, else the sign of a - b.
int fn_8013D224(f32 a, f32 b) {
    f32 d = a - b;
    f32 m = (f32)__fabs(d);
    if (m <= lbl_80558DC4[0]) {
        return 0;
    }
    return d > lbl_805614A4 ? lbl_805614AC : lbl_805614BC;
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

ZoneMaster::ZoneMaster(Race* race) : t6C(0.8f), t74(0.8f), t7C(1.0f), t84(1.0f), t8C(2.0f) {
    int i;
    int n;
    ValueTable* t;
    ValueTable* conf;
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
    n = race->nCars;
    lbl_8055E1C0 = this;
    nCars = n;
    cars = new ("ZoneMaster::tab") CarState[n];
    nHumans = 0;
    for (i = 0; i < nCars; i++) {
        cars[i].human = race->cars[i].kind == 0x50;
        if (cars[i].human) {
            nHumans++;
        }
    }
    triggers = 0;
    if (((EventDesc*)GetEvent(fn_801769CC()->event))->vals->GetBool("use_triggers", 1)) {
        triggers = (Deletable*)fn_80171698();
    }
    {
        const char* file = ((EventDesc*)GetEvent(fn_801769CC()->event))->vals->GetString("zonefile", "zones");
        fn_801345D8(buf, sizeof(buf), "*%s/%s.va0", fn_8000AC2C(), file);
    }
    t = fn_80131810(buf);
    conf = fn_80131810("zoneconf.val");
    if (t) {
        ValueNode* node = 0;
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
    LoadSound(fcount, "fcount.sfx");
    LoadSound(levelup, "levelup.sfx");
    LoadSound(missflag, "missflag.sfx");
    LoadSound(ringhit, "ringhit.sfx");
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

ZoneLevels::ZoneLevels(ValueTable* t) {
    int i;
    ValueNode* child;
    int lvl;
    n = fn_8013DDD4(t, "ZoneLevel");
    levels = new ("ZoneLevel[]") ZoneLevel[n];
    lvl = 0;
    for (i = 0; (child = t->GetChild(i)) != 0; i++) {
        if (fn_8013CB44("ZoneLevel", child)) {
            ZoneLevel* prev = lvl > 0 ? levels : 0;
            ZoneLevel* l = &levels[lvl];
            int j;
            char buf[0x80];
            if (prev) {
                l->n = prev->n;
                l->gates = new ("zone[]") Gate[l->n];
                {
                    ValPoint pt = child->FindChild("Zone")->GetPoint("tan");
                    f32 scale = fn_80204CFC("zone_gate_scale");
                    f32 w = fn_8013E300((V2*)&pt) * 2.0f * scale;
                    for (j = 0; j < l->n; j++) {
                        l->gates[j] = prev->gates[j];
                        l->gates[j].Scale(w);
                    }
                }
            } else {
                int k;
                f32 w;
                ValueNode* g;
                l->n = fn_8013DE7C(child, "Zone");
                l->gates = new ("Zone[]") Gate[l->n];
                w = child->FindChild("config")->GetFloat("gate_scale", 1.0f);
                w = fn_80204CFC("zone_gate_scale") * w;
                k = 0;
                for (j = 0; (g = child->GetChildAt(j)) != 0; j++) {
                    if (fn_8013CB44("Zone", g)) {
                        l->gates[k++].Init(g, w);
                    }
                }
            }
            for (j = 0; j < l->n; j++) {
                Gate* gt = &l->gates[j];
                if (gt->ring) {
                    int nn = l->n;
                    Gate* a = 0;
                    Gate* b = 0;
                    int k;
                    for (k = 1; k < nn; k++) {
                        a = &l->gates[(nn + (j - k)) % nn];
                        if (!a->ring) {
                            break;
                        }
                    }
                    for (k = 1; k < nn; k++) {
                        b = &l->gates[(j + (nn + k)) % nn];
                        if (!b->ring) {
                            break;
                        }
                    }
                    fn_8013DF24(gt, a, b);
                }
            }
            l->timeScale = fn_80204CFC("zone_time_scale");
            l->mult = 1;
            l->score = 10;
            sprintf(buf, "scoring_level%d", lvl + 1);
            if (t->HasChild(buf)) {
                ValueNode* sn = t->FindChild(buf);
                l = &levels[lvl];
                if (sn) {
                    l->mult = sn->GetInt("m", lvl + 1);
                    l->score = sn->GetInt("flag", 10);
                } else {
                    l->mult = lvl + 1;
                    l->score = 10;
                }
                l->score = l->score * fn_80204CFC("score_scale");
            } else {
                l = &levels[lvl];
                l->mult = lvl + 1;
                l->score = 10;
                l->score = l->score * fn_80204CFC("score_scale");
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
    Vec3* a = &prev->a;
    Vec3* b = &next->a;
    Vec3* p = &g->a;
    int i;
    for (i = 0; i < 2; i++) {
        V2 pa;
        V2 pb;
        V2 d;
        V2 r;
        V2 pp;
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
        pp.x = p->x;
        pp.y = p->z;
        d.x *= inv;
        d.y *= inv;
        t = d.x * (pp.x - pa.x) + d.y * (pp.y - pa.y);
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
    b.y = 0.0f;
    a.y = 0.0f;
    q.y = c.y + n.y * w;
    q.x = c.x + n.x * w;
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
    d = PSVECDistance(&a, &b);
#define s (f / d)
    {
        Vec3 mid(a.x + (b.x - a.x) * 0.5f, a.y + (b.y - a.y) * 0.5f, a.z + (b.z - a.z) * 0.5f);
        a = Vec3(mid.x + s * (a.x - mid.x), mid.y + s * (a.y - mid.y), mid.z + s * (a.z - mid.z));
        b = Vec3(mid.x + s * (b.x - mid.x), mid.y + s * (b.y - mid.y), mid.z + s * (b.z - mid.z));
    }
#undef s
}

// MATCHING-AID: the target copies both end points as one 24-byte block (six lwz, six stw), which means
// a and b were probably one segment struct in the original. GateSeg stands in for that type.
struct GateSeg {
    Vec3 a;
    Vec3 b;
};

Gate& Gate::operator=(const Gate& o) {
    *(GateSeg*)&a = *(const GateSeg*)&o.a;
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

CarState::CarState(void)
    : t98(1.0f), tA0(1.25f), tA8(1.25f), tB0(1.25f), tB8(1.65f), tC0(1.0f), tC8(0.5f), tD0(0.5f), tD8(0.5f) {}

ZoneMaster::~ZoneMaster(void) {
    int i;
    if (prerace) {
        delete prerace;
    }
    fn_802C6CF8(ringModel);
    layouts.a.a.DeleteAll();
    fn_802CAC54(tex[0]);
    fn_802CAC54(tex[1]);
    fn_802CAC54(tex[2]);
    fn_802CAC54(tex[3]);
    fn_802CAC54(tex[4]);
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
    CarState* c = &zm->cars[car];
    if (c->human) {
        c->rating = 1;
    }
}

extern "C" {
int fn_801E866C(int player);
Watcher* fn_802C86DC(void);
void fn_802A9BFC(Vec3* a, Vec3* b, s32 tex, int d, int e, int f, int g, f32 t);
void fn_801E9E30(s16* h, Vec3* p);
f32 fn_801E7E24(void);
void fn_802981F8(Vec3* p);
}  // extern "C"
void UpdateContinueOverlay(void);
extern "C" {
void fn_80147F30(f32 x, f32 y, f32 w, f32 h, u32 color, int segs, s32 tex, f32 start, f32 step, f32 u0, f32 v0,
                 f32 u1, f32 v1);
void fn_80147EE0(LayoutArt* a, f32 s);
void fn_801476D8(LayoutArt* a, u32 color, s32 tex);
u32 fn_80147C50(u32 a, u32 b, f32 t);
void fn_801476C4(LayoutItem* item);
void fn_801479A8(LayoutItem* orig, LayoutItem* item, const char* fmt, f32 alpha, ...);
void fn_80147BD0(LayoutArt* a, LayoutArt* b, int c, int d, s32 tex, f32 alpha);
void fn_8000FF88(u32 color, int size, const char* text, f32 scale, f32 x, f32 y, ...);
}

void ZoneMaster::DrawNextGate(void) {
    f32 t = fn_8013CB3C() - nextT;
    CarState* c = &cars[fn_801E866C(0)];
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
        if (g == nextGate || PSVECSquareDistance(&g->a, &fn_802C86DC()->pos) < 40000.0f) {
            if (nextGate != g) {
                nextGate = g;
                nextT = fn_8013CB3C();
            }
            {
                Vec3 b = g->b;
                Vec3 a = g->a;
                f32 y = (a.y > b.y ? a.y : b.y) + 10.0f;
                int lo;
                int hi;
                b.y = y;
                a.y = y;
                lo = 0;
                hi = 0x48;
                if (level == lbl_8042FB3C.highestLevel - 1) {
                    lo = 0x48;
                    hi = 0x72;
                }
                fn_802A9BFC(&a, &b, tex[1], 0, lo, 0x100, hi, t);
            }
        }
    }
}


void ZoneMaster::vf44(void) {
    GateHit* h;
    FixedArray<GateHit, 8> hits;
    CarState* c = &cars[fn_801E866C(-1)];
    Gate* g = c->gate;
    if (c->human && g) {
        int nPlain = 0;
        int nRing = 0;
        Watcher* w = fn_802C86DC();
        int i;
        for (i = 0; i < 10; i++) {
            int ok = 1;
            f32 d2;
            if (g->ring && nRing < 5) {
                nRing++;
            } else if (!g->ring && nPlain < 2) {
                nPlain++;
            } else {
                ok = 0;
            }
            if (ok && fn_8013CFF0(g, w, &d2)) {
                if (hits.count + 1 > hits.max) {
                    break;
                }
                {
                    int k;
                    Vec3* p = &g->a;
                    for (k = 0; k < 2; k++) {
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
        if (!((f32)__fabs(fn_801E7E24()) <= lbl_80558DC4[0])) {
            int i;
            for (i = 0; i < hits.count; i++) {
                Gate* gg = hits.data[i].gate;
                if (!gg->ring) {
                    Vec3* p = &gg->a;
                    int k;
                    for (k = 0; k < 2; k++) {
                        fn_802981F8(p);
                        p++;
                    }
                }
            }
        }
        DrawHits(&hits);
    }
}

// Stripped by the linker; keeps its constant in place.
extern "C" f32 fn_zone_StubM2(f32 x) { return x * -2.0f; }

extern "C" f32 fn_8013EFB0(f32 x) { return x - 320.0f; }

extern "C" f32 fn_8013EFBC(f32 y) { return 240.0f - y; }

int ZoneMaster::vf5C(int p) {
    if ((f32)__fabs(f250) <= lbl_80558DC4[0]) {
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

int ZoneMaster::vf68(void) { return hud.state == 2; }

void ZoneMaster::vf4C(int p) { DrawHud(p); }

extern "C" void fn_801475D0(Layout* l, const char* name, const char* fmt, ...);

// Stripped by the linker; keep their literals in place.
extern "C" void fn_zone_TextElem(int len, const char* fmt) {
    fn_802E317C(" oversized text elem (len %d, fmt \"%s\")", len, fmt);
}

extern "C" void fn_zone_WideOpen(Layout* l, int mode, int mult, f32* out) {
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

void ZoneMaster::DrawBonus(int p, CarState* c) {
    if (c->gate) {
        f32 k = c->BonusScale();
        f32 a = Lerp(1.0f, 0.0f, Clamp01(InvLerp(0.0f, lbl_8042FB3C.timeBonusLen,
                                                 fn_8013CB3C() - c->bonusT - lbl_8042FB3C.timeBonusStartT)));
        LayoutArt* pie = mainLyt->FindArt("bonus_pie");
        f32 r = pie->w * k;
        f32 x = pie->pos.x;
        f32 y = pie->pos.y;
        int n = a * 360.0f;
        if (n > 0) {
            fn_80147F30(x, y, r, 0.0f, 0xFF00FF00, n, tex[4], 1.5707964f, 0.017453292f, 0.0f, 0.0f, 1.0f, 1.0f);
            fn_80147F30(x, y, r, 0.0f, 0xFF000000, 1, tex[4], 1.5707964f, 0.06981317f, 0.0f, 0.0f, 1.0f, 1.0f);
            fn_80147F30(x, y, r, 0.0f, 0xFF000000, 1, tex[4], 1.5707964f + 0.017453292f * n, 0.06981317f, 0.0f,
                        0.0f, 1.0f, 1.0f);
        }
        mainLyt->FindArt("score_star_art");
        {
            LayoutArt star;
            star = *mainLyt->FindArt("bonus_star_art");
            star.pos.x = pie->pos.x;
            star.pos.y = pie->pos.y;
            fn_8013CB90(&star);
            fn_80147EE0(&star, k);
            fn_801476D8(&star, fn_80147C50(0xFFFFFF, 0xFFFFFFFF, Lerp(1.0f, 0.0f, Clamp01(InvLerp(0.2f, 0.0f, a)))),
                        tex[3]);
        }
    }
}

void ZoneMaster::DrawNothing(int p, CarState* c) {}

extern "C" int fn_8013F438(f32 t) { return Lerp(200.0f, 0.0f, Clamp01(InvLerp(0.2f, 0.8f, t))); }

extern "C" void fn_8013F498(f32 t, V2* a, V2* b) {
    f32 s = Lerp(150.0f, 50.0f, Clamp01(InvLerp(0.0f, 1.0f, t)));
    f32 h = s / 4;
    V2 c;
    V2 d1;
    V2 d2;
    c.x = 0.0f;
    c.y = Lerp(0.0f, 50.0f, Clamp01(InvLerp(0.0f, 1.0f, t * t)));
    d1.x = -s;
    d1.y = h;
    d2.x = s;
    d2.y = -h;
    a->x = c.x + d1.x;
    a->y = c.y + d1.y;
    b->x = c.x + d2.x;
    b->y = c.y + d2.y;
}

// Stripped by the linker; keeps its literals in place.
extern "C" void fn_zone_LevelTitle(Layout* l) {
    l->Find("level_title");
    l->Find("level_fade_in");
    l->Find("level_number");
    l->Find("level_fade_out");
}

void ZoneMaster::DrawLevel(int p, CarState* c) {
    if (level > 0) {
        if (t74.Active()) {
            f32 a = Lerp(0.0f, 1.0f, Clamp01(InvLerp(0.6f, 1.0f, t74.Frac())));
            LayoutItem* it = mainLyt->Find("middle_message");
            if (it) {
                LayoutItem m;
                m = *it;
                m.scale = it->scale * 0.3f;
                m.color = 0xFFFFFF;
                fn_801479A8(it, &m, "PERFECT LEVEL", a);
            }
            {
                LayoutArt* star = mainLyt->FindArt("perfect_star_art");
                LayoutArt* score = mainLyt->FindArt("score_star_art");
                if (star) {
                    LayoutArt s;
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
            f32 a = Lerp(0.0f, 1.0f, Clamp01(InvLerp(0.6f, 1.0f, t6C.Frac())));
            LayoutItem* it = mainLyt->Find("middle_message");
            if (it) {
                LayoutItem m;
                m = *it;
                m.scale = it->scale * 0.3f;
                m.color = 0xFFFFFF;
                fn_801479A8(it, &m, "LEVEL %d", a, level + 1);
            }
        }
    }
}

void ZoneMaster::DrawMissed(int p, CarState* c) {
    if (c->tA8.Active()) {
        f32 t = c->tA8.Frac();
        u32 col = (fn_8013F438(t) << 24) | 0xFF7F00;
        V2 a;
        V2 b;
        f32 s;
        fn_8013F498(t, &a, &b);
        s = Lerp(1.8f, 1.0f, Clamp01(InvLerp(0.4f, 0.9f, t)));
        fn_8000FF88(col, 12, "TIME MISSED", s, 0.0f, fn_8013EFBC(a.y));
    }
    if (c->timeLeft < lbl_8042FB3C.lowTimeWarnT) {
        f32 f = 1.0f - c->timeLeft / lbl_8042FB3C.lowTimeWarnT;
        fn_80147F30(320.0f, 160.0f, 50.0f, 0.0f, ((int)(f * 255.0f) << 24) | 0xFFFFFF, (1.0f - f) * 100.0f, tex[2],
                    1.5707964f, 0.06283186f, 0.0f, 0.0f, 1.0f, 1.0f);
    }
}

void ZoneMaster::DrawGateStar(int p, CarState* c) {
    if (c->gate && !vf24()) {
        f32 k = c->BonusScale();
        LayoutArt* a = mainLyt->FindArt("gate_star_art");
        if (a) {
            LayoutArt l;
            l = *a;
            fn_8013CB90(&l);
            fn_80147EE0(&l, k);
            fn_801476D8(&l, -1, tex[3]);
        }
        fn_801476C4(mainLyt->Find("next_gate"));
    }
}

int ZoneMaster::vf24(void) {
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

int ZoneMaster::vf20(int car) {
    CarState* c = &cars[car];
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
void fn_802C8E00(u32 color, int idx, f32 x, f32 y, f32 z, f32 u, f32 v);
void fn_802C8E48(void);
void fn_802C8D74(void);
void fn_80130980(char* buf, int size, int digits, f32 t);
void fn_8014784C(Layout* l, const char* name, const char* set, int index);
void fn_80147CEC(LayoutItem* out, LayoutItem* a, LayoutItem* b, f32 t);
void fn_801474EC(LayoutItem* item, const char* fmt, ...);
}  // extern "C"
void UpdateScoreOverlays(int, int);
extern "C" {
void fn_80147ABC(LayoutItem* a, LayoutItem* b, const char* fmt, f32 t, ...);
void PSVECNormalize(const Vec3* a, Vec3* b);
void PSVECCrossProduct(const Vec3* a, const Vec3* b, Vec3* c);
void fn_802C70AC(int model, void* mtx, int a, int b);
void fn_802A96E8(Vec3* pts, int n, int c, s32 lite, s32 glow);
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

void ZoneMaster::DrawTimer(int p, CarState* c) {
    f32 age;
    if (lbl_8055E1BC) {
        fn_802C8BD0(tex[0]);
        if (fn_802C8DBC(0x10)) {
            LayoutArt* bar = mainLyt->FindArt("timer_bar");
            f32 x = fn_8013EFB0(bar->pos.x);
            f32 y = fn_8013EFBC(bar->pos.y);
            f32 h = bar->h;
            f32 w = bar->w;
            u32 col;
            fn_802C8E00(-1, 0, x, y, 0.0f, 0.46484375f, 0.98828125f);
            fn_802C8E00(-1, 0, x, y - h, 0.0f, 0.46484375f, 0.98828125f);
            fn_802C8E00(-1, 1, x + w, y, 0.0f, 0.46484375f, 0.98828125f);
            fn_802C8E00(-1, 1, x + w, y - h, 0.0f, 0.46484375f, 0.98828125f);
            col = lbl_8055E1C4.rgba;
            fn_802C8E00(col, 0, x, y, 0.0f, 0.46484375f, 0.98828125f);
            fn_802C8E00(col, 0, x, y - h, 0.0f, 0.46484375f, 0.98828125f);
            fn_802C8E00(col, 1, x + w * c->timeLeft, y, 0.0f, 0.46484375f, 0.98828125f);
            fn_802C8E00(col, 1, x + w * c->timeLeft, y - h, 0.0f, 0.46484375f, 0.98828125f);
            fn_802C8E00(-1, 0, x, y, 0.0f, 0.90625f, 0.92578125f);
            fn_802C8E00(-1, 0, x, y - h, 0.0f, 0.90625f, 1.0f);
            fn_802C8E00(-1, 1, 14.0f + x, y, 0.0f, 0.9609375f, 0.92578125f);
            fn_802C8E00(-1, 1, 14.0f + x, y - h, 0.0f, 0.9609375f, 1.0f);
            fn_802C8E00(-1, 1, x + w - 14.0f, y, 0.0f, 0.9609375f, 0.92578125f);
            fn_802C8E00(-1, 1, x + w - 14.0f, y - h, 0.0f, 0.9609375f, 1.0f);
            fn_802C8E00(-1, 1, x + w, y, 0.0f, 1.0f, 0.92578125f);
            fn_802C8E00(-1, 1, x + w, y - h, 0.0f, 1.0f, 1.0f);
            fn_802C8E48();
        }
        fn_802C8D74();
    } else {
        char buf[0x14];
        int i;
        f32 now;
        EventRing::Entry* e;
        fn_80130980(buf, 0x14, 3, c->clock);
        fn_801475D0(mainLyt, "big_timer", "%s", buf);
        fn_8014784C(mainLyt, "stop_watch_art", "stop_watch_artset", 0);
        if (t7C.Active()) {
            f32 t = Ease(t7C.Frac());
            LayoutItem* a = mainLyt->Find("timer_increase_0");
            LayoutItem item;
            fn_80147CEC(&item, a, mainLyt->Find("timer_increase_1"), t);
            fn_801474EC(&item, "+%.0f", f98);
        }
        if (t84.Active()) {
            f32 t = Ease(t84.Frac());
            LayoutItem* a = mainLyt->Find("timer_decrease_0");
            LayoutItem item;
            fn_80147CEC(&item, a, mainLyt->Find("timer_decrease_1"), t);
            fn_801474EC(&item, "-%.0f", f9C);
        }
        now = fn_8013CB3C();
        for (i = 0; events.a.Get(i); i++) {
            age = (now - events.a.Get(i)->time) / 1.0f;
            LayoutArt* s0 = mainLyt->FindArt("ring_bonus_star_0");
            LayoutArt* s1 = mainLyt->FindArt("ring_bonus_star_1");
            LayoutArt* s2 = mainLyt->FindArt("score_star_art");
            f32 keys[3] = {0.0f, 0.7f, 1.0f};
            LayoutArt* arts[3];
            u32 col = -1;
            int k;
            f32 u;
            arts[0] = s0;
            arts[1] = s1;
            arts[2] = s2;
            for (k = 1; age > keys[k] && k < 2; k++) {
            }
            if (k == 2) {
                col = 0xFFFFFF;
            }
            u = (age - keys[k - 1]) / (keys[k] - keys[k - 1]);
            if (1.0f < u) {
                u = 1.0f;
            }
            u = u > 0.0f ? u : 0.0f;
            fn_80147BD0(arts[k - 1], arts[k], -1, col, tex[3], Ease(Lerp(0.0f, 1.0f, u)));
        }
    }
}

void ZoneMaster::DrawScore(int p, CarState* c) {
    int score = c->score - c->timeBonus;
    if (c->t98.Active() && c->bonus > 50) {
        f32 f = 1.0f - c->t98.Frac();
        score = c->score - (int)(c->bonus * f);
    }
    UpdateScoreOverlays(fn_801E866C(p), score);
}

void ZoneMaster::DrawMissedFlag(int p, CarState* c) {
    if (!c->tA8.Active() && c->tA0.Active()) {
        LayoutItem* a = mainLyt->Find("missed_start");
        LayoutItem* b = mainLyt->Find("missed_end");
        f32 t = c->tA0.Frac();
        fn_80147ABC(a, b, (const char*)a->unk1C, t * t);
    }
}

void ZoneMaster::DrawGateBonus(int p, CarState* c) {
    if (c->tD8.Active()) {
        LayoutArt* a0;
        LayoutArt* a1;
        LayoutArt* a2;
        c->tD8.Frac();
        a0 = mainLyt->FindArt("bonus_star_art");
        a1 = mainLyt->FindArt("gate_star_art");
        a2 = mainLyt->FindArt("score_star_art");
        if (a0 && a1 && a2) {
            LayoutArt s0;
            LayoutArt s1;
            LayoutArt s2;
            s0 = *a0;
            s1 = *a1;
            s2 = *a2;
            fn_8013CB90(&s1);
            fn_8013CB90(&s0);
            if (c->stats.bonus > 0) {
                fn_80147BD0(&s0, &s2, -1, 0xFFFFFF, tex[3],
                            Lerp(0.0f, 1.0f, Clamp01(InvLerp(0.0f, c->tD8.len / 2, fn_8013CB3C() - c->tD8.start))));
                fn_80147BD0(&s1, &s2, -1, 0xFFFFFF, tex[3],
                            Lerp(0.0f, 1.0f,
                                 Clamp01(InvLerp(c->tD8.len / 2, c->tD8.len, fn_8013CB3C() - c->tD8.start))));
            } else {
                fn_80147BD0(&s1, &s2, -1, 0xFFFFFF, tex[3], Clamp01(c->tD8.Frac() * 2.0f));
            }
        }
    }
}

void ZoneMaster::DrawHud(int p) {
    int i = fn_801E866C(p);
    CarState* c = &cars[i];
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
            LayoutArt* a = mainLyt->FindArt("sudden_death");
            if (a) {
                fn_801476D8(a, -1, tex[0]);
            }
        }
    }
}

struct RingMtx {
    Vec3 x;
    Vec3 y;
    Vec3 z;
    Vec3 pos;
};

void ZoneMaster::DrawRing(Gate* g, int c) {
    RingMtx m;
    f32 s;
    {
        Vec3 mid = g->Center();
        m.pos = mid;
    }
    m.x = Vec3(g->a.x - g->b.x, g->a.y - g->b.y, g->a.z - g->b.z);
    PSVECNormalize(&m.x, &m.x);
    m.y = Vec3(0.0f, 1.0f, 0.0f);
    PSVECCrossProduct(&m.x, &m.y, &m.z);
    s = PSVECDistance(&g->a, &g->b) / 20.0f;
    m.x = m.x * s;
    m.y = m.y * s;
    m.z = m.z * s;
    m.pos.y = g->ringY;
    fn_802C70AC(ringModel, &m, -1, c);
}

void ZoneMaster::DrawHits(FixedArray<GateHit, 8>* hits) {
    FixedArray<Vec3, 32> pts;
    int i;
    DrawNextGate();
    for (i = hits->count - 1; i >= 0; i--) {
        Gate* g = hits->data[i].gate;
        if (g->ring) {
            if (pts.count > 0) {
                glow.Draw(pts.data, pts.count, i288);
                pts.count = 0;
            }
            DrawRing(hits->data[i].gate, 2);
        } else {
            pts.Add(g->a);
            pts.Add(hits->data[i].gate->b);
        }
    }
    if (pts.count > 0) {
        glow.Draw(pts.data, pts.count, i288);
        pts.count = 0;
    }
    for (i = 0; events.a.Get(i); i++) {
        DrawRing(events.a.Get(i)->gate, 4);
    }
}

extern "C" {
size_t strlen(const char* s);
char* fn_801308F4(char* buf, s32 size, s32 value);
int fn_801609C4(int score);
}
const char* GetGradeDisplayName(int grade);

extern u32 lbl_8055E044;  // localization version
class LocString;
extern "C" void fn_80131318(LocString* s);

class LocString {
public:
    LocString(const char* key);
    const char* Get(void) {
        if (version != lbl_8055E044) {
            fn_80131318(this);
        }
        return text;
    }

    const char* key;   // offset 0x0
    const char* text;  // offset 0x4
    u32 version;       // offset 0x8
};


int ZoneHud::Update(CarState* c) {
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
        shown = Lerp(0.0f, tb + 0.5f, u);
        left = tb - shown;
    }
    fn_801345D8(bonus, 0x14, "%d", shown);
    if (noBonus) {
        StrCopyN("", 0x14, timeLeft);
        StrCopyN("", 0x14, bonusTitle);
    } else {
        static LocString caps("ResultsOverlay:TIME_BONUS_CAPS");
        fn_80130980(timeLeft, 0x14, 2, left);
        StrCopyN(caps.Get(), 0x14, bonusTitle);
    }
    fn_801345D8(finalText, 0x14, "%d", final);
    fn_801308F4(totalText, 0x14, total);
    fn_801308F4(noBonusText, 0x14, total - tb);
    {
        int g = fn_801609C4(total);
        if (g != -1) {
            StrCopyN(GetGradeDisplayName(g), 10, grade);
        } else {
            StrCopyN("-", 10, grade);
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
void fn_802AE118(ArtPlayer* a);
int fn_80204CF0(void);
void* fn_80156A40(void);
void fn_801352B8(void* it);
int fn_80135D60(void* it, int index);
void fn_8013530C(void* it, int unk);
void fn_802E30C8(const char* fmt, ...);  // debug print
void* fn_801E7E3C(void);
void fn_8000FBE4(void);
int fn_80161C38(void);
int fn_80160A74(void);
int fn_80160998(int x);
f32 fn_80147204(LayoutItem* item, const char* text);
void fn_80130978(char* buf, int size, f32 t);
void qsort(void* base, size_t n, size_t size, int (*cmp)(const void*, const void*));
extern s32 lbl_8055E3C4;
}

struct ILPos {
    struct Seg {
        u8 pad0[0x18];
        f32 scale;  // offset 0x18
        f32 base;   // offset 0x1C
    }* seg;     // offset 0x0
    f32 t;      // offset 0x4
    s32 mode;   // offset 0x8
};
extern "C" ILPos fn_80136594(void* f, V2* p);

extern "C" void fn_80141454(void) {}

extern "C" void fn_80141458(void) {}

extern "C" GameMode* fn_8014145C(Race* race) { return new ("ZoneMaster") ZoneMaster(race); }

Glow::Glow(void) {
    lite = fn_802CAC28("zonelite.tex", 0);
    glow = fn_802CAC28("zoneglow.tex", 0);
}

Glow::~Glow(void) {
    fn_802CAC54(lite);
    fn_802CAC54(glow);
}

void Glow::Draw(Vec3* pts, int n, int c) { fn_802A96E8(pts, n, c, lite, glow); }


// Nothing is inlined into the per-frame update in the original build: every helper it uses is a call to
// a weak copy emitted right after it.
#pragma dont_inline on
void ZoneMaster::vf0C(void) {
    if (!b258 && fn_801B306C() < 6.75f) {
        b258 = 1;
        if (prerace && !fn_80009C84()) {
            prerace->play = 1;
            prerace->b4B = 0;
            prerace->b4A = 0;
        }
    }
    if (!vf24()) {
        int i;
        for (i = 0; i < nCars; i++) {
            CarState* c = GetCar(i);
            f32 now = fn_8013CB3C();
            if (c->human && c->done != 1 && fn_801B306C() <= 0.0f) {
                int finished = 0;
                if (!Paused(now) && !c->gate && fn_8013D1F0(i) > 0) {
                    c->gate = FirstGate(i);
                    c->SetWidth(fn_8017960C(i));
                }
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
                            ringhit->SetPitch(lbl_802F93B0[ClampI(events.a.Count() - 1, 0, 4)]);
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
                                int mx = MaxBonus(level);
                                if (TimeBonusPoints(c) == mx) {
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
                    if (!c->AddTime(-0.016683351f) && !missflag->Active()) {
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
        if ((f32)__fabs(f250) <= lbl_80558DC4[0]) {
            hud.Update(&cars[fn_80179630(0)]);
        }
    }
}

#pragma dont_inline reset

void ZoneMaster::vf10(void) {
    int lv;
    int i;
    lbl_8055E1BC = 0;
    b294 = 0;
    nextGate = 0;
    for (lv = 0; lv < levels->n; lv++) {
        ZoneLevel* l = &levels->levels[lv];
        int j;
        for (j = 0; j < l->n; j++) {
            Gate* g = &l->gates[j];
            Vec3* p = &g->a;
            int k;
            for (k = 0; k < 2; k++) {
                fn_801E9E30(&g->h, p);
                p->y += 0.5f;
                p++;
            }
            {
                Vec3 mid = g->Center();
                V2 q;
                ILPos pos;
                q.x = mid.x;
                q.y = mid.z;
                pos = fn_80136594(fn_80178C9C(), &q);
                g->t = pos.seg->base + pos.seg->scale * pos.t;
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
    for (i = 0; i < nCars; i++) {
        CarState* c = &cars[i];
        memset(c->hits, 0, sizeof(c->hits));
        c->tC0.start = -999.0f;
        c->rating = 1;
        c->i4 = 1;
        c->f8 = 0.0f;
        c->i1C = 0;
        c->refill = 0;
        c->bonus = 0;
        c->bonusT = -999.9f;
        c->i20[0] = 4;
        c->i20[1] = 4;
        c->i20[2] = 4;
        c->i20[3] = 4;
        c->i20[4] = 4;
        c->i20[5] = 4;
        c->i20[6] = 4;
        c->i20[7] = 4;
        c->i20[8] = 4;
        c->i20[9] = 4;
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

void ZoneMaster::vf50(int p) { vf4C(p); }

void ZoneMaster::vfB8(ZoneResults* r) {
    int i;
    r->nCars = nCars;
    r->nHumans = nHumans;
    r->done = vf24();
    r->bC = 0;
    for (i = 0; i < nCars; i++) {
        ZoneResults::Entry* e = &r->entries[i];
        e->b10 = 0;
        e->place = fn_801B2EC0(i)->GetInfo()->place;
        e->score = cars[i].score;
    }
}

void ZoneMaster::vfC0(void) { b26C = 1; }

void ZoneMaster::vfC4(void) { b26C = 0; }

int ZoneMaster::vf54(void) { return 0x1008; }

CSubTaskMgr* ZoneMaster::vfE8(void) { return fn_8013D288(); }

int ZoneMaster::vfA0(Cam* cam) {
    PathIter it;
    Path* path;
    it.Set((int)((PathSet*)fn_80156A40())->Get(0));
    path = it.path;
    PlaceCamera(cam, &it, 0.0f, path->len - 25.0f - 35.0f);
    ScaleV(&cam->v30, &cam->v18, 4.4444447f);
    cam->i44 = 1;
    return 1;
}

void ZoneMaster::vfAC(int car) {
    CarState* c = &cars[car];
    if (c->human && vf20(car)) {
        c->gate = 0;
        c->EndCombo();
        c->EndStreak();
    }
    GameMode::vfAC(car);
}

void ZoneMaster::vfA8(int car) { vfAC(car); }

int ZoneMaster::vf8C(int p, int key) {
    int i;
    if (key == 's') {
        for (i = 0; i < layouts.a.a.count; i++) {
            layouts.a.a.data[i]->vf28();
        }
    } else if (key == 'e') {
        for (i = 0; i < layouts.a.a.count; i++) {
            layouts.a.a.data[i]->vf20();
        }
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
        half = PSVECDistance(&levels->levels->gates->a, &levels->levels->gates->b) * 0.5f;
        len = sqrt(tan.x * tan.x + tan.y * tan.y);
        if (!((f32)__fabs(len) <= lbl_80558DC4[0])) {
            tan.x *= half / len;
            tan.y *= half / len;
        }
        fn_802E30C8("\ttan %.2f, %.2f", tan.x, tan.y);
        fn_802E30C8("end");
    }
    if (key == 'X') {
        if (vf68()) {
            if (hud.state == 2) {
                hud.b9 = 1;
            }
        } else if (hud.state != 0) {
            f32 now = lbl_8055E3C8;
            if (hud.state == 1 && !hud.noBonus && hud.t > 0.0f && now - hud.t < 1.0f) {
                hud.noBonus = 1;
                if (hud.anim) {
                    fn_802AE118(hud.anim);
                }
                StopSound(hud.timeBonusAnim);
                StopSound(hud.noTimeBonusAnim);
                StopSound(hud.complete1);
                StopSound(hud.complete2);
                hud.b8 = 0;
                hud.t = now - 1.0f;
            }
        }
    }
    return 0;
}

int ZoneMaster::vf3C(int p) {
    f32 t = ((RaceClock*)fn_801E7E3C())->t;
    f32 t0 = 6.75f;
    f32 t1 = t0 - 0.8f;
    f32 t2 = t1 - 0.3f;
    f32 t3 = t2 - 0.15f;
    f32 t4 = t3 - 0.8f;
    f32 t5 = t4 - 0.3f;
    LayoutItem* h0 = mainLyt->Find("intro_head_0");
    LayoutItem* h1 = mainLyt->Find("intro_head_1");
    if (t < t5) {
        fn_8000FBE4();
    }
    if (!fn_80161C38()) {
        if (t < 6.75f) {
            f32 a = Lerp(0.0f, 1.0f, Clamp01(InvLerp(t1, t2, t)));
            LayoutItem* b0 = mainLyt->Find("star_body_0");
            LayoutItem* b1 = mainLyt->Find("star_body_1");
            UpdateScoreOverlays(fn_801E866C(p), 0);
            if (h0 && h1 && b0 && b1) {
                char buf[0x14];
                LayoutItem it;
                fn_801479A8(h0, h1, fn_80131234("PreRaceOverlay:STARS_NEEDED"), Ease(a));
                fn_801308F4(buf, 0x14, fn_80160998(fn_80160A74()));
                it = *b0;
                it.pos.x -= fn_80147204(&it, buf) * 0.5f;
                fn_801479A8(&it, b1, buf, Ease(a));
            }
        }
        if (t < t3) {
            f32 a;
            LayoutItem* tb;
            LayoutItem* bt;
            fn_8014784C(mainLyt, "stop_watch_art", "stop_watch_artset", 0);
            a = Lerp(0.0f, 1.0f, Clamp01(InvLerp(t4, t5, t)));
            tb = mainLyt->Find("time_body_0");
            bt = mainLyt->Find("big_timer");
            if (h0 && h1 && tb && bt) {
                char buf[0x14];
                LayoutItem it;
                fn_801479A8(h0, h1, fn_80131234("PreRaceOverlay:TIME_LIMIT"), Ease(a));
                fn_80130978(buf, 0x14, cars[fn_801E866C(0)].clock);
                it = *tb;
                it.pos.x += fn_80147204(&it, buf) * 0.5f;
                fn_801479A8(&it, bt, buf, Ease(a));
            }
        }
    }
    return 1;
}

f32 ZoneMaster::vf38(void) { return 7.75f; }

int ZoneMaster::vf64(void) { return 1; }

void ZoneMaster::vfF8(void) {
    hud.anim = new ("ArtAnimManager") ArtPlayer(1);
    hud.Init();
    hud.Reset();
}

void ZoneHud::Reset(void) { ResetState(); }

struct HudBinding {
    HudBinding(s32 type, const char* name, void* ptr) : type(type), name(name), ptr(ptr) {}
    s32 type;
    const char* name;
    void* ptr;
};
extern const HudBinding lbl_802FAA40;  // list terminator

void ZoneHud::Init(void) {
    HudBinding b[] = {
        HudBinding(3, "time_left", &i10C),
        HudBinding(3, "time_bonus", &i110),
        HudBinding(3, "grade", &i114),
        HudBinding(3, "final_score", &i118),
        HudBinding(1, "gate_count", score),
        HudBinding(1, "time_bonus", bonus),
        HudBinding(1, "time_bonus_title_caps", bonusTitle),
        HudBinding(1, "ring_bonus", finalText),
        HudBinding(1, "final_score", totalText),
        HudBinding(1, "grade", grade),
        HudBinding(1, "final_score_no_time_bonus", noBonusText),
        HudBinding(1, "time_left", timeLeft),
        HudBinding(1, "challenge_complete_1", textC6),
        HudBinding(1, "challenge_complete_2", textDA),
        HudBinding(1, "out_of_time_message", failText),
        HudBinding(2, "difficulty_level", &level),
        lbl_802FAA40,
    };
    StrCopyN("100", 0x14, noBonusText);
    StrCopyN("50", 0x14, bonus);
    StrCopyN("150", 0x14, totalText);
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

void ZoneMaster::vfFC(void) { hud.Clear(); }
