// Stunt trackers (0x80157278-0x8015EC50): every car's stunt meter is an array of trackers (air, drift, close
// call, 360, car throw, jump combo, car hit, crash, ring) that time or count a stunt, rank it against the
// thresholds in stunts.val / stuntdef.val, award points and drive the HUD's stunt pop-up. Names are guesses
// apart from the strings.
#include "game/common.h"
#include "game/mathutil.h"
#include "game/sound.h"
#include "game/arrayref.h"
#include "game/raceopts.h"
#include <revolution/MTX/vec.h>
#include "game/carbody.h"

inline f32 RaceCarRec::Speed(void) const { return speed; }

inline s32 RaceCarRec::Air(void) const { return airborne; }

#define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))

extern RaceCarRec* lbl_804348E8[];

// The HUD's stunt pop-up and a results line (StuntDisplay), and what hands them out (StuntFactory: the HUD,
// the results list).
#include "game/stuntdisp.h"

// Something told about every awarded stunt (achievements, statistics).
#include "game/stuntwatch.h"
#include "game/clamp01.h"

extern "C" {
void* memset(void* d, int c, size_t n);
int strcasecmp(const char* a, const char* b);
int fn_801345D8(char* buf, u32 size, const char* fmt, ...);  // snprintf
void fn_802E30C8(const char* fmt, ...);                      // debug print
ValueTable* fn_80131810(const char* name);
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* t);
const char* fn_80131234(const char* key);  // localize
GameSound* fn_802076A0(const char* name, int flags, int car, int pan);
f32 fn_80204CFC(const char* name);  // tuning value
char* fn_8023139C(char* buf, int size, u32 value);  // format a score
void fn_801CB6E4(u32 type, int car, long v);  // stop a rumble effect (awards.cpp)
void fn_801CB750(u32 type, int car, f32 v);  // awards.cpp
// The weak out-of-line copy of NearZero, which retail calls in some trackers while others expand it (the macro
// below), as (NearZero)(x) so the macro does not expand: an inline NearZero function expands at every call
// (asmcmp: 11 functions change).
extern "C++" int NearZero(f32 x);
int fn_80179650(void);
int fn_801CF064(void);
int fn_801B2F2C(int id);
int fn_801B8570(int id);
int fn_80161BB8(void);
int fn_80161C4C(void);
}

void AchievementsAdd(int category, s32 value);

extern f32 __float_epsilon[];  // FLT_EPSILON (MSL)
#define NearZero(x) ((f32)__fabs(x) <= __float_epsilon[0])
// SpinTracker::Rate tests its angle through the float abs inline FAbs (mathutil.h; the call copy gives fabs its own register);
// the other trackers use the macro above. Exact original spelling unknown.

template <typename T, int N>
struct FixedList {
    FixedList(void) : a(storage, N) {}
    s32 Count(void) const { return a.count; }
    T* At(int i) { return &a.data[i]; }
    int Find(const T& v) {
        int i;
        for (i = 0; i < a.count; i++) {
            if (a.data[i] == v) {
                return i;
            }
        }
        return -1;
    }
    void Remove(int k) {
        if (k >= 0 && k < Count()) {
            int last = Count() - 1;
            if (k != last) {
                a.data[k] = a.data[last];
            }
            a.count--;
        }
    }
    T* Push(const T& v) {
        T* p = &a.data[a.count++];
        *p = v;
        return p;
    }
    ArrayRef<T> a;
    T storage[N];
};

const char* lbl_8032E540[15] = {
    "air", "nice_landing", "offroad", "drift", "close_call", "turbo_close_call", "high_speed", "360",
    "roll", "car_throw", "jump_combo", "car_hit_car", "car_hit_car", "crash", "ring",
};

f32 lbl_8055A02C = 100.0f;  // close-call speed

const char* lbl_8032E668[16] = {
    "Stunt:AIR",       "Stunt:LANDING_BOOST", "Stunt:OFFROAD",   "Stunt:DRIFT",       "Stunt:TREE_RUN",
    "Stunt:TURBO_TREE_RUN", "Stunt:HISPEED",  "Stunt:360",       "Stunt:ROLL",        "Stunt:CAR_THROW",
    "Stunt:JUMP_COMBO", "Stunt:CAR_HIT_CAR",  "Stunt:CAR_WRECK", "Stunt:CRASH",       "Stunt:RING",
    0,
};

const char* lbl_8032E7F8[15] = {
    "Stunt:SUPER_AIR",        "Stunt:SUPER_LANDING_BOOST", "Stunt:SUPER_OFFROAD",
    "Stunt:SUPER_DRIFT",      "Stunt:SUPER_TREE_RUN",      "Stunt:SUPER_TURBO_TREE_RUN",
    "Stunt:SUPER_HISPEED",    "Stunt:SUPER_360",           "Stunt:SUPER_ROLL",
    "Stunt:SUPER_CAR_THROW",  "Stunt:SUPER_JUMP_COMBO",    "Stunt:SUPER_CAR_HIT_CAR",
    "Stunt:SUPER_CAR_WRECK",  "Stunt:SUPER_CRASH",         "Stunt:SUPER_RING",
};

const char* lbl_8032E9E0[15] = {
    "Stunt:FINAL_STATS_AIR",        "Stunt:FINAL_STATS_LANDING_BOOST", "Stunt:FINAL_STATS_OFFROAD",
    "Stunt:FINAL_STATS_DRIFT",      "Stunt:FINAL_STATS_TREE_RUN",      "Stunt:FINAL_STATS_TURBO_TREE_RUN",
    "Stunt:FINAL_STATS_HISPEED",    "Stunt:FINAL_STATS_360",           "Stunt:FINAL_STATS_ROLL",
    "Stunt:FINAL_STATS_CAR_THROW",  "Stunt:FINAL_STATS_JUMP_COMBO",    "Stunt:FINAL_STATS_CAR_HIT_CAR",
    "Stunt:FINAL_STATS_CAR_WRECK",  "Stunt:FINAL_STATS_CRASH",         "Stunt:FINAL_STATS_RING",
};

ModuleTag lbl_8055E280;
UnkStatic7F lbl_8055E288;
f32 lbl_8042FD08[8];  // per-car score scale
static FixedList<StuntWatcher*, 8> lbl_8042FD34;


// One car's stunt meter (betarace's Meter): the tracker array below and every tracker implement it.
class Meter {
public:
    Meter(void) {}
    virtual ~Meter(void) {}                       // 0x08
    virtual void Init(void) = 0;                  // 0x0C
    virtual void Reset(void) {}                   // 0x10
    virtual f32 Rate(const RaceCarRec* rc) = 0;         // 0x14
    virtual void Dump(void) = 0;                  // 0x18
    virtual void SetActive(u8 on) {}              // 0x1C
    virtual void Hit(f32 mag, int other) {}       // 0x20
    virtual void Bump(const RaceCarRec* rc, int kind, void* other) {}  // 0x24
    virtual void Stop(const RaceCarRec* rc) {}          // 0x28
    virtual void Award(f32 v, RaceCarRec* rc, int kind) {}  // 0x2C
    virtual void Start(const RaceCarRec* rc) {}         // 0x30
    virtual void Land(const RaceCarRec* rc) {}          // 0x34
    virtual s32 Total(void) const { return 0; }         // 0x38
    virtual void Crash(const RaceCarRec* rc) = 0;       // 0x3C
    virtual int Pending(f32* out) const { return 0; }   // 0x40
    virtual int Count(void) const { return 0; }         // 0x44
    virtual void Fill(StuntFactory* l) const = 0;          // 0x48
};

struct Rank {
    Rank(void) { memset(this, 0, 4); }
    s32 count;       // offset 0x0
    s32 scored;      // offset 0x4
    s32 natural;     // offset 0x8
    f32 total;       // offset 0xC
    f32 pts;         // offset 0x10
    s32 thresh;      // offset 0x14
    char adj[0x20];  // offset 0x18
};

extern "C" {
void fn_801584D4(s32 type, int car, int rank);
}

struct RankTable : FixedList<struct Rank, 10> {
    void Clear(void) {
        int i;
        for (i = 0; i < a.count; i++) {
            a.data[i].count = 0;
            a.data[i].scored = 0;
            a.data[i].natural = 0;
            a.data[i].total = 0.0f;
        }
    }
    s32 First(void) const {
        int i;
        for (i = 0; i < a.count; i++) {
            if (a.data[i].thresh >= 0) {
                return a.data[i].thresh;
            }
        }
        return 0;
    }
    s32 Last(void) const {
        int i = a.count;
        while (i-- > 0) {
            if (a.data[i].thresh >= 0) {
                return a.data[i].thresh;
            }
        }
        return 0;
    }
};

// A stunt kind with its rank table, its HUD pop-up and its share of the score.
struct CountSound;

namespace {
class Tracker : public Meter {
public:
    Tracker(StuntFactory* h, int c) {
        ranks.a.count = 0;
        car = c;
        hud = h;
        evt = 0;
    }
    virtual ~Tracker(void) {}
    virtual void Init(void) { ranks.Clear(); }

    int NumRanks(void) const { return ranks.a.count; }
    virtual int FirstRank(void) const { return 1; }  // 0x4C
    virtual f32 Points(int rank) const {             // 0x50
        if (rank >= FirstRank()) {
            return ranks.a.data[rank].pts;
        }
        return 0.0f;
    }
    virtual int FindRank(s32 v) const { return Find(v); }  // 0x54
    int Find(s32 v) const {
        for (int i = 0; i < ranks.a.count; i++) {
            s32 t = ranks.a.data[i].thresh;
            if (v < t || t < 0) {
                return i;
            }
        }
        return 0;
    }
    virtual s32 Value(void) const = 0;                          // 0x58
    virtual int Rank(void) const { return FindRank(Value()); }  // 0x5C
    virtual f32 Progress(void) const {                          // 0x60
        int r = Rank();
        f32 f = 0.0f;
        if (r < ranks.a.count - 1) {
            s32 lo = r > 0 ? ranks.a.data[r - 1].thresh : 0;
            s32 v = Value();
            f32 cur = (f32)(v - lo);
            f32 span = (f32)(ranks.a.data[r].thresh - lo);
            if (span > 0.0f) {
                f = cur / span;
            }
        }
        return (f32)r + f;
    }
    virtual const char* Label(void) const { return fn_80131234(lbl_8032E668[Type()]); }       // 0x64
    virtual const char* SuperLabel(void) const { return fn_80131234(lbl_8032E7F8[Type()]); }  // 0x68
    virtual const char* StatsLabel(void) const { return fn_80131234(lbl_8032E9E0[Type()]); }  // 0x6C
    virtual const char* Name(void) const { return lbl_8032E540[Type()]; }                     // 0x70
    virtual s32 MinThresh(void) const { return ranks.First(); }  // 0x74
    virtual s32 MaxThresh(void) const { return ranks.Last(); }  // 0x78
    virtual s32 Type(void) const = 0;  // 0x7C
    virtual void Show(void) {    // 0x80
        if (!evt) {
            evt = hud->Alloc();
        }
    }
    virtual void Hide(void) {  // 0x84
        if (evt) {
            evt->Stop();
            evt = 0;
        }
    }
    virtual void Refresh(void) {  // 0x88
        if (evt) {
            evt->Text1("%s", Label());
            evt->SetType(Type());
            evt->SetValue(Progress());
            if (Rank() >= FirstRank()) {
                evt->SetReady(1);
            }
        }
    }
    virtual void Score(f32* pts) {  // 0x8C
        *pts *= fn_80204CFC("score_scale");
        *pts *= lbl_8042FD08[car];
        if (evt && !NearZero(*pts)) {
            char buf[0x10];
            evt->Text2("%s", fn_8023139C(buf, sizeof(buf), *pts));
            evt->SetType(Type());
            evt->SetPoints(*pts);
            evt->Text1("%s", Label());
            evt->Text0("%s", SuperLabel());
            evt->SetValue(Progress());
            if (Rank() >= FirstRank()) {
                evt->SetReady(1);
            }
        }
        {
            f32 p = *pts;
            int r = Rank();
            ranks.a.data[r].count++;
            if (!NearZero(p)) {
                ranks.a.data[r].scored++;
                if (NearZero(p - ranks.a.data[r].pts)) {
                    ranks.a.data[r].natural++;
                }
            }
            ranks.a.data[r].total += p;
            fn_801584D4(Type(), car, Rank());
        }
    }
    void Dump(void) {
        int i;
        int scored;
        f32 total;
        int count;
        struct Rank* r;
        int offset;

        fn_802E30C8("\r\n%s:", Label());
        fn_802E30C8("   Rank         Count   Unscored   Natural   Score");
        fn_802E30C8("--------------------------------------------------");
        total = 0.0f;
        count = 0;
        scored = 0;
        i = 0;
        offset = 0;
        for (; i < ranks.a.count; offset += sizeof(struct Rank), ++i) {
            r = (struct Rank*)((u8*)ranks.a.data + offset);
            fn_802E30C8("%d: %-12.12s %3d     %4d       %3d       %10.0f", i, r->adj, r->count, r->count - r->scored,
                        r->natural, r->total);
            total += r->total;
            count += r->count;
            scored += r->scored;
        }
        fn_802E30C8("Total count: %d (%d unscored) (%.0f points)", count, count - scored, total);
    }
    virtual void Fill(StuntFactory* l) const {
        int n = 0;
        struct {
            int i;
        } rank;
        StuntDisplay* line;
        for (rank.i = 0; rank.i < ranks.a.count; rank.i++) {
            n += ranks.a.data[rank.i].scored;
        }
        line = l->Alloc();
        if (line) {
            f32 total;
            char buf[0x10];
            s32 pts;
            line->Text0("%s", StatsLabel());
            line->Text1("%d", n);
            line->SetType(Type());
            struct {
                int i;
            } points;
            total = 0.0f;
            for (points.i = 0; points.i < ranks.a.count; points.i++) {
                total += ranks.a.data[points.i].total;
            }
            pts = total;
            line->Text2("%s", fn_8023139C(buf, sizeof(buf), pts));
            line->SetPoints(pts);
            line->SetValue(Progress());
            if (Rank() >= FirstRank()) {
                line->SetReady(1);
            }
        }
    }
    virtual void Start(const RaceCarRec* rc) {}
    virtual int InAir(void) const { return 0; }  // 0x90
    virtual s32 Total(void) const { return 0; }

    s32 car;                                 // offset 0x4
    RankTable ranks;                         // offset 0x8
    StuntFactory* hud;                           // offset 0x244
    StuntDisplay* evt;                          // offset 0x248
};
}  // namespace


struct Blip {
    void Clear(void) { x8 = 0; }
    u8 pad0[0x8];
    s32 x8;  // offset 0x8
};

// A stunt that lasts: air time, drift time. Scored when it ends, by its duration.
namespace {
class TimedTracker : public Tracker {
public:
    TimedTracker(StuntFactory* h, int c, f32 min, s32 fx) : Tracker(h, c) {
        effect = fx;
        minT = min;
        mult = 1.0f;
        f254 = 305.0f;
        evt = 0;
        blip = 0;
    }
    virtual ~TimedTracker(void) {}
    virtual void Init(void) {
        Tracker::Init();
        dur = 0.0f;
        f260 = 0.0f;
        since = 0.0f;
        f264 = 0.0f;
        Hide();
    }
    virtual void Stop(const RaceCarRec* rc) {
        if (dur > 0.0f) {
            End();
        }
    }
    void StopEffect(void) {
    if (f260 > minT) {
        if (effect) {
            fn_801CB750(effect, car, f260);
        }
        f260 = 0.0f;
    }
}
    f32 Finish(const RaceCarRec* rc) {
    f32 pts = 0.0f;
    if (dur > minT && Rank() > 0) {
        pts = Points(Rank()) * mult;
        Score(&pts);
        Hide();
    }
    if (!NearZero(pts)) {
        Ended(dur);
    }
    dur = 0.0f;
    f264 = 0.0f;
    since += 0.016683351f;
    return pts;
}
    virtual void Reset(void) {
        Hide();
        dur = 0.0f;
        StopFx();
        f264 = 0.0f;
        since = 0.016683351f;
    }
    virtual s32 Value(void) const { return dur; }
    virtual int Airborne(const RaceCarRec* rc) = 0;  // 0x94
    virtual void Ended(f32 d) {}               // 0x98

    void StopFx(void) {
        if (f260 > minT) {
            if (effect) {
                fn_801CB750(effect, car, f260);
            }
            f260 = 0.0f;
        }
    }
    f32 End(void) {
        f32 pts = 0.0f;
        if (dur > minT && Rank() > 0) {
            pts = Points(Rank()) * mult;
            Score(&pts);
            Hide();
        }
        if (!(NearZero)(pts)) {
            Ended(dur);
        }
        dur = 0.0f;
        f264 = 0.0f;
        since += 0.016683351f;
        return pts;
    }
    virtual f32 Rate(const RaceCarRec* rc) {
        f32 r = 0.0f;
        if (NearZero(rc->f2690) && Airborne(rc)) {
            if (dur > minT) {
                Show();
            }
            {
                f32 s = rc->speed;
                f32 d = 0.016683351f * s;
                dur += d;
                f260 += d;
                f264 += d;
            }
            Refresh();
            if (dur > minT && Rank() > 0) {
                since = 0.0f;
                if (blip) {
                    blip->x8 = 0;
                }
            }
            r = 0.0f;
        } else if (NearZero(since)) {
            r = End();
        } else {
            Hide();
            since += 0.016683351f;
            StopFx();
        }
        int n = ranks.a.count;
        int k = Rank();
        if (k == n - 1) {
            r = End();
        }
        return r;
    }
    virtual void Crash(const RaceCarRec* rc) { End(); }

    f32 minT;    // offset 0x24C
    f32 mult;    // offset 0x250
    f32 f254;    // offset 0x254
    f32 since;   // offset 0x258
    f32 dur;     // offset 0x25C
    f32 f260;    // offset 0x260
    f32 f264;    // offset 0x264
    s32 effect;  // offset 0x268
    Blip* blip;  // offset 0x26C
};
}  // namespace


// A stunt that is counted: close calls, car hits, rings... Each one extends a short window; the count is
// scored when the window runs out.

extern "C" int fn_80157278(int car);

// Air time. Doubled when both wheels... (flags from the landing), zero in modes that need a boost.
namespace {
class AirTracker : public TimedTracker {
public:
    AirTracker(StuntFactory* h, int c, struct CountSound* cs);
    virtual f32 Points(int rank) const {
        f32 p = Tracker::Points(rank);
        if (fn_801769CC()->type & 0x200000) {
            if (b271) {
                return p * (b270 ? 2.0f : 1.0f);
            }
            return 0.0f;
        }
        return p;
    }
    virtual int Airborne(const RaceCarRec* rc) { return rc->airborne == 0; }
    virtual s32 Type(void) const { return 0; }
    virtual void Ended(f32 d) {
        last = d;
        b272 = 1;
    }
    virtual void Init(void) {
        b273 = 0;
        b270 = 0;
        b271 = 0;
        b272 = 0;
        TimedTracker::Init();
    }
    f32 RateBody(const RaceCarRec* rc);
    virtual f32 Rate(const RaceCarRec* rc);
    virtual void Stop(const RaceCarRec* rc);
    void StopBody(const RaceCarRec* rc);
    virtual void Award(f32 v, RaceCarRec* rc, int kind) {
        if (kind == 1) {
            b270 = 1;
        } else if (kind == 0) {
            b271 = 1;
        }
    }
    virtual int Count(void) const { return Rank() > 0; }
    virtual int Pending(f32* out) const {
        if (b273) {
            if (out) {
                f32 hi, v;
                v = last;
                hi = ((AirTracker*)this)->MaxThresh();
                f32 lo = ((AirTracker*)this)->MinThresh();
                *out = Lerp(0.0f, 1.0f, Clamp01((v - lo) / (hi - lo)));
            }
            return 1;
        }
        return 0;
    }

    u8 b270;   // offset 0x270
    u8 b271;   // offset 0x271
    u8 b272;   // offset 0x272
    u8 b273;   // offset 0x273
    f32 last;  // offset 0x274
    s32 car2;  // offset 0x278
};
}  // namespace

inline void Unused(const RaceCarRec** p) {}
void Unused2(f32** p);


// The out-of-class tracker methods are inline: retail emits them with the in-class ones after fn_80158618, class by
// class in member declaration order, and that order is also the order their constants enter .sdata2. The members
// of each class are declared in retail's text order (new virtuals keep their vtable slots); AirTracker, ThrowTracker
// and CarHitTracker have no destructor of their own (retail emits theirs first, as implicit ones).
// Helper name unproven: shared inline body inferred from retail call and emission order.
inline f32 AirTracker::RateBody(const RaceCarRec* rc) {
    f32 r = 0.0f;
    b272 = 0;
    if ((NearZero)(((RaceCarRec*)rc)->F2690()) && Airborne(rc)) {
        if (dur > minT) {
            Show();
        }
        {
            f32 d = 0.016683351f * rc->Speed();
            dur += d;
            f260 += d;
            f264 += d;
        }
        Refresh();
        if (dur > minT && Rank() > 0) {
            since = 0.0f;
            if (blip) {
                blip->Clear();
            }
        }
        r = 0.0f;
    } else if ((NearZero)(since)) {
        r = Finish(rc);
    } else {
        Hide();
        since += 0.016683351f;
        StopEffect();
    }
    if (Rank() == NumRanks() - 1) {
        r = Finish(rc);
    }
    b273 = b272 && b271;
    b270 = 0;
    b271 = 0;
    return r;
}
inline f32 AirTracker::Rate(const RaceCarRec* rc) { return RateBody(rc); }

// Helper name unproven: shared inline body inferred from retail call and emission order.
inline void AirTracker::StopBody(const RaceCarRec* rc) {
    s32 n;
    const RaceCarRec* p = rc;
    Unused(&p);
    if (dur > 0.0f) {
        Finish(p);
    }
    {
        int last = ranks.a.count - 1;
        n = ranks.a.data[last].scored;
    }
    if (n > 0 && fn_80179650() == 1 && rc->f1F0 == 0x50 && !fn_801CF064()) {
        AchievementsAdd(3, n);
    }
}
inline void AirTracker::Stop(const RaceCarRec* rc) { StopBody(rc); }

// RaceCarVec: valvec3.h's ValVec3.

inline void StopSound(GameSound* s) {
    if (s) {
        s->stop = 1;
        s->restart = 0;
    }
}

inline void PlaySound(GameSound* s) {
    s->start = 1;
    s->loop = 0;
    s->stop = 0;
}

inline void LoopSound(GameSound* s) {
    s->stop = 0;
    s->loop = 1;
    s->start = 1;
}

// Drift time; plays the skid sound while sliding fast enough sideways.
namespace {
class DriftTracker : public TimedTracker {
    f32 DriftAmount(const RaceCarRec* rc, u8 was) const {
        f32 spd = rc->speed;
        f64 magnitude = __fabs(PSVECDotProduct((const Vec*)&rc->vel, (const Vec*)&rc->mtx.rot.x));
        f32 side = (f32)magnitude;
        f32 k = was ? 0.15f : 0.35f;
        f32 fade = 1.0f - 8.0f * ground;
        fade = fade > 0.0f ? fade : 0.0f;
        f32 clamped = Clamp01(5.0f * (side / (spd * k) - 1.0f));
        return clamped * fade;
    }

public:
    DriftTracker(StuntFactory* h, int c, struct CountSound* cs);
    virtual int Airborne(const RaceCarRec* rc) {
        u8 was;
        f32 amount;
        if (rc->airborne == 0) {
            ground += 0.016683351f;
        } else {
            ground = 0.0f;
        }
        amount = 0.0f;
        was = sliding;
        sliding = Sliding(rc, was, rc->speed);
        if (sliding) {
            amount = DriftAmount(rc, was);
        }
        if (snd) {
            if (!NearZero(amount)) {
                snd->SetVolume(0.4f * amount);
                LoopSound(snd);
            } else {
                snd->Stop();
            }
        }
        return sliding;
    }
    virtual s32 Type(void) const { return 3; }
    virtual ~DriftTracker(void) {
        if (snd) {
            delete snd;
        }
    }
    virtual void Reset(void) {
        TimedTracker::Reset();
        StopSound(snd);
    }
    virtual void Init(void) {
        TimedTracker::Init();
        sliding = 1;
        ground = 0.0f;
        StopSound(snd);
    }
    virtual void Stop(const RaceCarRec* rc);
    void StopBody(const RaceCarRec* rc);
    virtual void Start(const RaceCarRec* rc) {
        if (snd) {
            u8 was = sliding;
            if (!Sliding(rc, was, rc->speed)) {
                snd->Stop();
            }
        }
    }

    int Sliding(const RaceCarRec* rc, u8 was, f32 spd) {
        if (NearZero(rc->f2690) && spd > 5.0f && ground < 0.5f) {
            double d = __fabs(PSVECDotProduct((const Vec*)&rc->vel, (const Vec*)&rc->mtx.rot.x));
            f32 side = d;
            return side > spd * (was ? 0.15f : 0.35f);
        }
        return 0;
    }

    f32 ground;  // offset 0x270
    u8 sliding;  // offset 0x274
    GameSound* snd;  // offset 0x278
};
}  // namespace

// Helper name unproven: shared inline body inferred from retail call and emission order.
inline void DriftTracker::StopBody(const RaceCarRec* rc) {
    s32 n;
    const RaceCarRec* p = rc;
    Unused(&p);
    if (dur > 0.0f) {
        Finish(p);
    }
    {
        int last = ranks.a.count - 1;
        n = ranks.a.data[last].scored;
    }
    if (n > 0 && fn_80179650() == 1 && rc->f1F0 == 0x50 && !fn_801CF064()) {
        AchievementsAdd(5, n);
    }

}
inline void DriftTracker::Stop(const RaceCarRec* rc) { StopBody(rc); StopSound(snd); }

struct CloseCallSlot {
    void* other;  // offset 0x0
    f32 time;     // offset 0x4
    u8 done;      // offset 0x8
};

extern f32 lbl_8055E3C8;  // race clock
extern f32 lbl_8055A02C;  // close-call speed

f32 Max(f32 a, f32 b);  // out-of-line max (0x801429D0)

// Near misses with other cars ("close call", "turbo close call" while boosting).
namespace {
class CountTracker : public Tracker {
public:
    f32 RateCount(const RaceCarRec* rc);
    CountTracker(StuntFactory* h, int c, f32 w, s32 fx) : Tracker(h, c) {
        effect = fx;
        window = w;
        blip = 0;
        evt = 0;
    }
    virtual ~CountTracker(void) {}
    virtual void Init(void) {
        Tracker::Init();
        count = 0;
        playing = 0;
        timer = 0.0f;
        mult = 1.0f;
        Hide();
    }
    virtual f32 Rate(const RaceCarRec* rc) {
        f32 r;
        int n;
        int full;
        {
            f32 t = timer - 0.016683351f;
            timer = 0.0f > t ? 0.0f : t;
        }
        if (rc->airborne == 0 && !Grounded()) {
            timer = timer > 0.033366702f ? timer : 0.033366702f;
        }
        r = 0.0f;
        full = 0;
        if (Capped()) {
            n = ranks.a.count;
            int k = Rank();
            if (k == n - 1) {
                full = 1;
            }
        }
        if (NearZero(timer) || full) {
            if (count > 0) {
                r = Take();
            }
            if (!full && playing > 0) {
                if (effect) {
                    fn_801CB6E4(effect, car, playing);
                }
                playing = 0;
            }
        } else if (count > 0) {
            Refresh();
        }
        return r;
    }
    void StopEffect(void) {
    if (playing > 0) {
        if (effect) {
            fn_801CB6E4(effect, car, playing);
        }
        playing = 0;
    }
}
    f32 Collect(const RaceCarRec* rc) {
    f32 pts = Points(Rank()) * mult;
    Score(&pts);
    Hide();
    count = 0;
    mult = 1.0f;
    return pts;
}
    virtual void Stop(const RaceCarRec* rc) {
        if (count > 0) {
            Take();
        }
    }
    virtual int Grounded(void) const { return 1; }  // 0x94
    virtual s32 Value(void) const { return count; }
    virtual s32 Shown(void) const { return count; }  // 0x98
    virtual int Capped(void) const { return 1; }    // 0x9C

    f32 Take(void) {
        f32 pts = Points(Rank()) * mult;
        Score(&pts);
        Hide();
        count = 0;
        mult = 1.0f;
        return pts;
    }
    virtual void Reset(void) {
        s32 p = playing;
        count = 0;
        if (p > 0) {
            if (effect) {
                fn_801CB6E4(effect, car, p);
            }
            playing = 0;
        }
        timer = 0.0f;
        mult = 1.0f;
        Hide();
    }
    virtual void Crash(const RaceCarRec* rc) { Take(); }

    u8 pad24C[0x250 - 0x24C];
    f32 window;   // offset 0x250
    f32 timer;    // offset 0x254
    s32 effect;   // offset 0x258
    f32 mult;     // offset 0x25C
    s32 playing;  // offset 0x260
    s32 count;    // offset 0x264
    Blip* blip;   // offset 0x268
};
// Helper name unproven: shared inline body inferred from retail call and emission order.
inline f32 CountTracker::RateCount(const RaceCarRec* rc) {
    f32 r;
    int full;
    timer = Max(0.0f, timer - 0.016683351f);
    if (!rc->Air() && !Grounded()) {
        timer = Max(timer, 0.033366702f);
    }
    r = 0.0f;
    full = 0;
    if (Capped() && Rank() == NumRanks() - 1) {
        full = 1;
    }
    if ((NearZero)(timer) || full) {
        if (count > 0) {
            r = Collect(rc);
        }
        if (!full) {
            StopEffect();
        }
    } else if (count > 0) {
        Refresh();
    }
    return r;
}

}  // namespace

namespace {
class CloseCallTracker : public CountTracker {
public:
    CloseCallTracker(StuntFactory* h, int c, struct CountSound* cs);
    virtual const char* Label(void) const { return fn_80131234(lbl_8032E668[Type()]); }
    virtual s32 Type(void) const { return 4; }
    virtual ~CloseCallTracker(void) {
        if (snd) {
            delete snd;
        }
    }
    virtual s32 Total(void) const { return turbo; }
    virtual f32 Rate(const RaceCarRec* rc);
    virtual void Init(void) {
        int i;
        CountTracker::Init();
        for (i = 0; i < 8; i++) {
            slots[i].other = 0;
            slots[i].time = -1000.0f;
            slots[i].done = 1;
        }
        flags = 0;
        turbo = 0;
        turboTime = -100.0f;
    }
    virtual void Stop(const RaceCarRec* rc);
    void StopBody(const RaceCarRec* rc);
    virtual void Bump(const RaceCarRec* rc, int kind, void* other);
    virtual void Land(const RaceCarRec* rc) {
        if (!turbo && (rc->f25BC > __float_epsilon[0]) != 0) {
            turbo = 1;
            turboTime = lbl_8055E3C8;
        }
    }

    CloseCallSlot slots[8];  // offset 0x26C
    s32 flags;               // offset 0x2CC
    u8 b2D0;                 // offset 0x2D0
    u8 turbo;                // offset 0x2D1
    f32 turboTime;           // offset 0x2D4
    GameSound* snd;              // offset 0x2D8
};
}  // namespace

inline f32 CloseCallTracker::Rate(const RaceCarRec* rc) {
    f32 now = lbl_8055E3C8;
    f32 r;
    int full;
    int any;
    CloseCallSlot* s;
    int n;
    if (turbo) {
        f32 turboLimit = 10.0f;
        if ((now - turboTime) > turboLimit || (rc->f25BC > __float_epsilon[0]) == 0) {
            turbo = 0;
        }
    }
    if (count == 0) {
        int boost;
        flags = 0;
        b2D0 = 1;
        boost = 1;
        if ((rc->f258C > 0.001f) == 0 && rc->f259C == 0) {
            boost = 0;
        }
        if (boost || rc->airborne == 0) {
            flags |= 2;
        } else {
            flags &= ~2;
        }
    } else if (rc->airborne > 0) {
        for (int i = 0; i < 4; i++) {
            if (((CarBody*)rc)->wheels[i].groundH == 0) {  // carbody.h: the car body's wheel records
                b2D0 = 0;
                break;
            }
        }
    }
    n = 0;
    any = 0;
    s = slots;
    for (int i = 0; i < 8; i++, s++) {
        if (!s->done && now - s->time > 0.1f) {
            s->done = 1;
            if (!any) {
                if (snd) {
                    snd->SetVolume(0.8f);
                    PlaySound(snd);
                }
                count++;
                playing++;
                timer = window;
                mult = 1.0f;
                Show();
                if (evt && blip) {
                    blip->x8 = 0;
                }
            }
            any = 1;
            n++;
        }
    }
    if (n > 0) {
        int boost = 1;
        if ((rc->f258C > 0.001f) == 0 && rc->f259C == 0) {
            boost = 0;
        }
        if (!boost && rc->airborne > 0) {
            flags &= ~2;
        }
    }
    if (count > 0 && Rank() > 0) {
        flags |= 1;
    }
    r = CountTracker::RateCount(rc);
    n = ranks.a.count;
    int rankValue = Rank();
    if (rankValue == n - 1) {
        flags = 0;
    }
    return r;
}

// Helper name unproven: shared inline body inferred from retail call and emission order.
inline void CloseCallTracker::StopBody(const RaceCarRec* rc) {
    s32 n;
    const RaceCarRec* p = rc;
    Unused(&p);
    if (count > 0) {
        Collect(p);
    }
    {
        int last = ranks.a.count - 1;
        n = ranks.a.data[last].scored;
    }
    if (n > 0 && fn_80179650() == 1 && rc->f1F0 == 0x50 && !fn_801CF064()) {
        AchievementsAdd(4, n);
    }
}
inline void CloseCallTracker::Stop(const RaceCarRec* rc) { StopBody(rc); }

inline void CloseCallTracker::Bump(const RaceCarRec* rc, int kind, void* other) {
    if (rc->speed > 0.44444445f * lbl_8055A02C && kind == 1003 && !turbo) {
        f32 now = lbl_8055E3C8;
        int recent = 0;
        CloseCallSlot* oldest;
        for (int i = 0; i < 8; i++) {
            CloseCallSlot* s = &slots[i];
            if (NearZero(s->time - now)) {
                recent = 1;
            }
            if (s->other == other && now - s->time < 5.0f) {
                return;
            }
        }
        oldest = 0;
        for (int i = 0; i < 8; i++) {
            CloseCallSlot* s = &slots[i];
            if (!oldest || s->time < oldest->time) {
                oldest = s;
            }
        }
        oldest->time = now;
        oldest->done = recent;
        oldest->other = other;
    }
}

extern "C" {
RaceCarRec* fn_8017960C(int id);
void fn_80198EA0(int car);
int fn_801D48FC(s32 handle);
int fn_801D4908(s32 handle, f32* pos, int n);
void fn_801E9DC4(void* q);
f32 fn_801DDC8C(void* q, f32 x, f32 z);
int fn_801DDA84(void* q, f32 x, f32 z, f32* out, f32 eps, int n);
f32 fn_801E7E24(void);
}  // extern "C"
int fn_80009C84(void);
extern "C" {
void fn_80206750(GameSound* s, int car, int pan);
}

inline s32 Round(f32 x) { return x < 0.0f ? x - 0.5f : x + 0.5f; }

// One-shot stunts reported with a value (crashes).
struct ThrowTarget {
    ThrowTarget(void) {
        state = 0;
        prev = 0;
    }
    s32 state;  // offset 0x0
    s32 prev;   // offset 0x4
    s32 car;    // offset 0x8
    f32 timer;  // offset 0xC
};

// Throwing another car with a tree/object.
namespace {
class ThrowTracker : public CountTracker {
public:
    void RateTarget(int i, int alive);
public:
    ThrowTracker(StuntFactory* h, int c);
    virtual s32 Type(void) const { return 9; }
    virtual void Init(void) {
        CountTracker::Init();
        handle = 0;
        active = 0;
    }
    virtual f32 Rate(const RaceCarRec* rc);
    virtual void Award(f32 v, RaceCarRec* rc, int kind) {
        if (kind == 3) {
            int i;
            f32 t = v;
            active = 1;
            handle = *(s32*)&t;
            for (i = 0; i < 8; i++) {
                if (i == rc->index) {
                    targets[i].car = -1;
                    targets[i].state = 0;
                    targets[i].prev = 0;
                } else {
                    targets[i].car = i;
                    targets[i].state = 1;
                    targets[i].prev = 1;
                }
            }
        }
    }

    s32 handle;              // offset 0x26C
    ThrowTarget targets[8];  // offset 0x270
    u8 active;               // offset 0x2F0
};
}  // namespace

namespace {
class JumpComboTracker : public CountTracker {
public:
    JumpComboTracker(StuntFactory* h, int c);
    virtual s32 Type(void) const { return 10; }
    virtual ~JumpComboTracker(void) {}
    virtual f32 Rate(const RaceCarRec* rc);
    virtual void Reset(void) {
        CountTracker::Reset();
        jumped = 0;
        ground = 0.0f;
        jumps = 0;
        if (effects > 0) {
            fn_801CB6E4(0x50204, car, effects);
            effects = 0;
        }
    }
    virtual void Init(void) {
        CountTracker::Init();
        jumped = 1;
        ground = 0.0f;
        jumps = 0;
        effects = 0;
    }
    virtual void Award(f32 v, RaceCarRec* rc, int kind) {
        if (kind == 6) {
            jumped = 1;
        }
    }

    u8 jumped;    // offset 0x26C
    f32 ground;   // offset 0x270
    s32 jumps;    // offset 0x274
    s32 effects;  // offset 0x278
};
}  // namespace


struct CarHit {
    CarHit(void) { timer = 0.0f; }
    bool Active(void) const { return timer > __float_epsilon[0]; }
    f32 timer;  // offset 0x0
    f32 mag;    // offset 0x4
};

// Being rammed by other cars into a wreck.
namespace {
class CarHitTracker : public Tracker {
public:
    CarHitTracker(StuntFactory* h, int c);
    virtual s32 Value(void) const {
        if (wrecked) {
            return 0x7FFFFFFF;
        }
        if (f290 > __float_epsilon[0]) {
            return f290;
        }
        return f28C;
    }
    virtual s32 Type(void) const { return 12; }
    virtual void Init(void) {
        int i;
        Tracker::Init();
        Hide();
        wrecked = 0;
        f290 = 0.0f;
        f28C = 0.0f;
        for (i = 0; i < 8; i++) {
            hits[i].timer = 0.0f;
        }
    }
    virtual f32 Rate(const RaceCarRec* rc) {
        f32 r;
        int i;
        wrecked = 0;
        f290 = 0.0f;
        f28C = 0.0f;
        for (i = 0; i < 8; i++) {
            if (hits[i].Active()) {
                int w;
                f28C = MaxF(f28C, hits[i].mag);
                if (hits[i].Active()) {
                    hits[i].timer -= 0.016683351f;
                }
                w = Wrecked(i);
                if (w || !hits[i].Active()) {
                    hits[i].timer = 0.0f;
                    wrecked = w;
                    f290 = MaxF(f290, hits[i].mag);
                }
            }
        }
        r = 0.0f;
        if (f28C > __float_epsilon[0]) {
            Show();
            Refresh();
            if (f290 > __float_epsilon[0]) {
                f32 pts = Points(Rank());
                Score(&pts);
                Hide();
                r = pts;
            }
        }
        return r;
    }
    virtual void Stop(const RaceCarRec* rc) {
        s32 n;
        if (f28C > __float_epsilon[0]) {
            f32 pts = Points(Rank());
            Score(&pts);
            Hide();
        }
        {
            int last = ranks.a.count - 1;
            n = ranks.a.data[last].scored;
        }
        if (n > 0 && fn_80179650() == 1 && rc->f1F0 == 0x50 && !fn_801CF064()) {
            AchievementsAdd(7, n);
        }
    }
    virtual void Crash(const RaceCarRec* rc) {
        f32 pts = Points(Rank());
        Score(&pts);
        Hide();
    }
    virtual void Hit(f32 mag, int other) {
        if (!Wrecked(other)) {
            CarHit* h = &hits[other];
            if (NearZero(h->timer)) {
                h->mag = mag;
            } else {
                h->mag = mag > h->mag ? mag : h->mag;
            }
            h->timer = 0.3f;
        }
    }

    int Wrecked(int i) {
        RaceCarRec* o = fn_8017960C(i);
        return o ? !NearZero(o->f2690) : 0;
    }

    CarHit hits[8];  // offset 0x24C
    f32 f28C;        // offset 0x28C
    f32 f290;        // offset 0x290
    u8 wrecked;      // offset 0x294
    s32 car2;        // offset 0x298
};
}  // namespace

namespace {
class ValueTracker : public Tracker {
public:
    ValueTracker(StuntFactory* h, int c) : Tracker(h, c) { car2 = c; }
    virtual ~ValueTracker(void) {}
    virtual void Refresh(void) {
        Tracker::Refresh();
        if (evt) {
            evt->Text1("%s", Label());
        }
    }
    virtual void Score(f32* pts) {
        Tracker::Score(pts);
        if (evt && !NearZero(*pts)) {
            evt->Text1("%s", Label());
        }
    }
    virtual s32 Value(void) const { return Round(value); }
    virtual int Kind(void) const = 0;            // 0x94
    virtual void Set(f32 v) { value = v; }  // 0x98
    virtual void Init(void) {
        Tracker::Init();
        Hide();
        value = -999.0f;
    }
    virtual void Reset(void) {}
    virtual f32 Rate(const RaceCarRec* rc) {
        f32 pts = 0.0f;
        if (value > 0.0f) {
            Show();
            Refresh();
            pts = Points(Rank());
            Score(&pts);
            Hide();
        }
        value = -999.0f;
        return pts;
    }
    virtual void Crash(const RaceCarRec* rc) {}
    virtual void Award(f32 v, RaceCarRec* rc, int kind) {
        if (car2 == rc->index && kind == Kind()) {
            value = v;
        }
    }

    s32 car2;   // offset 0x24C
    f32 value;  // offset 0x250
};
}  // namespace

// Spins and rolls.
namespace {
class SpinTracker : public CountTracker {
public:
    SpinTracker(StuntFactory* h, int c);
    virtual s32 Type(void) const { return 7; }
    virtual void Score(f32* pts) {
        int deg;
        Tracker::Score(pts);
        deg = spins * 360;
        if (evt && !NearZero(*pts)) {
            evt->Text1("%s %d", Label(), deg);
            evt->Text0("%s %d", SuperLabel(), deg);
        }
    }
    virtual int Capped(void) const { return 0; }
    virtual ~SpinTracker(void) {}
    virtual void Init(void) {
        CountTracker::Init();
        spins = 0;
        rolls = 0;
        angle = 0.0f;
        f27C = 0.0f;
    }
    virtual void Reset(void) {
        CountTracker::Reset();
        if (spins > 0) {
            fn_801CB6E4(0x48203, car, spins);
            spins = 0;
        }
        rolls = 0;
        angle = 0.0f;
        f27C = 0.0f;
    }
    virtual f32 Rate(const RaceCarRec* rc);
    virtual void Award(f32 v, RaceCarRec* rc, int kind) {
        if (car2 == rc->index && (kind == 7 || kind == 9)) {
            angle = v;
            if (kind == 7) {
                rolls = Round(angle);
            }
        }
    }
    virtual void Crash(const RaceCarRec* rc);
    void CrashBody(const RaceCarRec* rc);
    virtual void Stop(const RaceCarRec* rc) {
        spins = Round(angle);
        Crash(rc);
        if (spins > 0) {
            if (spins > 0) {
                fn_801CB6E4(0x48203, car, spins);
                spins = 0;
            }
            angle = 0.0f;
        }
    }

    s32 spins;  // offset 0x26C
    s32 car2;   // offset 0x270
    s32 rolls;  // offset 0x274
    f32 angle;  // offset 0x278
    f32 f27C;   // offset 0x27C
};
}  // namespace

// Crashing (into the scenery).
namespace {
class CrashTracker : public ValueTracker {
public:
    CrashTracker(StuntFactory* h, int c);
    virtual int Kind(void) const { return 4; }
    virtual s32 Type(void) const { return 13; }
    virtual void Refresh(void) {
        ValueTracker::Refresh();
        if (evt) {
            evt->Text1("%s", Label());
        }
    }
    virtual void Score(f32* pts) {
        ValueTracker::Score(pts);
        if (evt && !NearZero(*pts)) {
            evt->Text1("%s", Label());
        }
    }
    virtual ~CrashTracker(void) {}
    virtual int InAir(void) const { return 1; }
};
}  // namespace

static const f32 lbl_802F9478[6] = {0.7932f, 0.8904f, 0.9433f, 0.9994f, 1.0f, 0.0f};

// Flying through rings.
namespace {
class RingTracker : public CountTracker {
public:
    RingTracker(StuntFactory* h, int c);
    virtual s32 Type(void) const { return 14; }
    virtual ~RingTracker(void) {
        if (snd) {
            delete snd;
        }
    }
    virtual void Init(void) {
        int pan;
        int mono;
        CountTracker::Init();
        pending = 0;
        queued = 0;
        StopSound(snd);
        mono = fn_80009C84();
        pan = 2;
        if (mono) {
            pan = 0;
        }
        if (snd) {
            fn_80206750(snd, car2, pan);
        }
    }
    virtual f32 Rate(const RaceCarRec* rc);
    virtual void Stop(const RaceCarRec* rc);
    void StopBody(const RaceCarRec* rc);
    virtual void Award(f32 v, RaceCarRec* rc, int kind) {
        if (kind == 5) {
            pending += Round(v);
        }
    }

    s32 pending;  // offset 0x26C
    s32 queued;   // offset 0x270
    s32 car2;     // offset 0x274
    GameSound* snd;   // offset 0x278
};
}  // namespace

inline f32 JumpComboTracker::Rate(const RaceCarRec* rc) {
    f32 r;
    int full;
    int n;
    if (rc->airborne > 0) {
        ground += 0.016683351f;
    } else {
        ground = 0.0f;
    }
    if (jumped) {
        int i;
        jumps++;
        effects++;
        jumped = 0;
        for (i = 0; i < jumps; i++) {
            count++;
            playing++;
            timer = window;
            mult = 1.0f;
            Show();
            if (evt && blip) {
                blip->x8 = 0;
            }
        }
    }
    n = ranks.a.count;
    int rank = Rank();
    if (rank == n - 1) {
        jumps = 1;
    }
    if (ground > 0.8f) {
        jumps = 0;
        if (effects > 0) {
            fn_801CB6E4(0x50204, car, effects);
            effects = 0;
        }
    }
    jumped = 0;
    r = CountTracker::RateCount(rc);
    return r;
}

/* Helper name unproven: updates and scores one throw target. */
inline void ThrowTracker::RateTarget(int i, int alive) {
    s32 h = handle;
    int hit;
    targets[i].prev = targets[i].state;
    if (targets[i].state == 0) {
        hit = 0;
    } else {
        RaceCarRec* o = fn_8017960C(targets[i].car);
        if (!o) {
            hit = 0;
        } else {
            int air = o->airborne > 0;
            switch (targets[i].state) {
            case 1:
                if (o) {
                    int st = 2;
                    if (air) {
                        st = 3;
                    }
                    targets[i].state = st;
                } else {
                    targets[i].state = 0;
                }
                break;
            case 2:
                if (air) {
                    targets[i].state = 3;
                }
                break;
            case 3:
                if (!air) {
                    u8 q[8];
                    f32 at[7];
                    targets[i].state = 2;
                    fn_801E9DC4(q);
                    {
                        f32 g = fn_801DDC8C(q, o->mtx.pos.x, o->mtx.pos.z);
                        if (g > 4.4444447f && o->vel.y - g > 0.44444445f) {
                            int n;
                            fn_801E9DC4(q);
                            n = fn_801DDA84(q, o->mtx.pos.x, o->mtx.pos.z, at, 1e-7f, 10);
                            if (fn_801D4908(h, at, n)) {
                                targets[i].state = 4;
                                targets[i].timer = 1.0f;
                            }
                        }
                    }
                }
                break;
            case 4:
                targets[i].timer -= 0.016683351f;
                if (targets[i].timer < 0.0f) {
                    targets[i].state = 5;
                } else if (air) {
                    targets[i].state = 3;
                }
                break;
            case 5:
                break;
            }
            hit = alive || targets[i].state == 4;
        }
    }
    active = (active | hit) != 0;
}

inline f32 ThrowTracker::Rate(const RaceCarRec* rc) {
    f32 r;
    int full;
    if (active) {
        int i;
        int alive = fn_801D48FC(handle);
        active = alive;
        for (i = 0; i < 8; i++) {
            RateTarget(i, alive);
            int fire = targets[i].state == 5 && targets[i].prev != 5;
            if (fire) {
                count++;
                playing++;
                timer = window;
                mult = 1.0f;
                Show();
                if (evt && blip) {
                    blip->x8 = 0;
                }
                fn_80198EA0(i);
            }
        }
        if (!active) {
            handle = 0;
        }
    }
    r = CountTracker::RateCount(rc);
    return r;
}

inline f32 SpinTracker::Rate(const RaceCarRec* rc) {
    f32 r;
    int full;
    if (!(FAbs(angle) <= __float_epsilon[0])) {
        spins += rolls;
        if (Round(angle) > 0) {
            int add = rolls ? 0 : Round(angle);
            playing = count = spins + add;
            timer = window;
            mult = 1.0f;
            Show();
            if (evt && blip) {
                blip->x8 = 0;
            }
        }
    }
    r = CountTracker::RateCount(rc);
    if (rolls > 0) {
        Crash(rc);
    }
    if (rolls > 0 || rc->airborne > 0) {
        f27C += fn_801E7E24();
        if (rolls > 0 || f27C > 0.75f) {
            if (spins > 0) {
                fn_801CB6E4(0x48203, car, spins);
                spins = 0;
            }
            angle = 0.0f;
        }
    } else if (rc->airborne == 0) {
        f27C = 0.0f;
    }
    rolls = 0;
    return r;
}

// Helper name unproven: shared inline body inferred from retail call and emission order.
inline void SpinTracker::CrashBody(const RaceCarRec* rc) {
    if (spins > 0) {
        Collect(rc);
    }
}
inline void SpinTracker::Crash(const RaceCarRec* rc) { CrashBody(rc); }

inline f32 RingTracker::Rate(const RaceCarRec* rc) {
    f32 r;
    int full;
    int play = 0;
    int i;
    if (snd && (queued += pending) > 0) {
        int n = count > 0 ? count : 0;
        n = n < 4 ? n : 4;
        snd->SetPitch(lbl_802F9478[n]);
        play = 1;
    }
    for (i = 0; i < pending; i++) {
        count++;
        playing++;
        timer = window;
        mult = 1.0f;
        Show();
        if (evt && blip) {
            blip->x8 = 0;
        }
    }
    pending = 0;
    if (play && !snd->playing) {
        queued--;
        PlaySound(snd);
    }
    r = CountTracker::RateCount(rc);
    return r;
}

// Helper name unproven: shared inline body inferred from retail call and emission order.
inline void RingTracker::StopBody(const RaceCarRec* rc) {
    s32 n;
    const RaceCarRec* p = rc;
    Unused(&p);
    if (count > 0) {
        Collect(p);
    }
    {
        int last = ranks.a.count - 1;
        n = ranks.a.data[last].scored;
    }
    if (n > 0 && fn_80179650() == 1 && rc->f1F0 == 0x50 && !fn_801CF064()) {
        AchievementsAdd(6, n);
    }
}
inline void RingTracker::Stop(const RaceCarRec* rc) { StopBody(rc); }

// The two count-up sounds of the score counter.
struct CountSound {
    CountSound(const char* a, const char* b, int car);
    ~CountSound(void) {
        if (s1) {
            delete s1;
        }
        if (s0) {
            delete s0;
        }
    }
    void Update(void) {
        if (s0) {
            if (on == 0) {
                s0->PlayLoop();
            } else {
                if (s1 && wasOn == 0) {
                    s1->Play();
                }
                s0->Stop();
            }
        }
        wasOn = on;
        on = 1;
    }
    GameSound* s0;  // offset 0x0
    GameSound* s1;  // offset 0x4
    s32 on;     // offset 0x8
    s32 wasOn;  // offset 0xC
};

void* operator new(size_t size, const char* tag);


// All of a car's trackers behind one Meter.
// The tracker constructors, the CountSound one, Unused2 and the per-tracker setup are inlines. The TrackerArray
// constructor, which uses them all, is over the 512-unit nested-inline budget (FACTS 2026-11-17 11:16), so inside
// fn_801572A4 every one of them stays a call and their copies follow it in reverse first-use order. That is
// the retail order to the function (Count, the setup, Name, At, Ring ... Drift, Push, Air, CountSound, Unused2,
// the list and Meter constructors); defined after the factory as ordinary functions they needed
// #pragma inline_depth(1) and still came out in another order. The setup is static: its strings are
// ordinary strings of the unit.
inline void Unused2(f32** p) {}

inline RingTracker::RingTracker(StuntFactory* h, int c) : CountTracker(h, c, 2.0f, 0) {
    int ok;
    car2 = c;
    ok = !fn_80157278(c);
    if (ok) {
        snd = fn_802076A0("ringhit.sfx", 1, c, 2);
    } else {
        snd = 0;
    }
}

inline CrashTracker::CrashTracker(StuntFactory* h, int c) : ValueTracker(h, c) {}

inline CarHitTracker::CarHitTracker(StuntFactory* h, int c) : Tracker(h, c) {
    int i;
    hits[0].timer = 0.0f;
    car2 = c;
    f290 = 0.0f;
    f28C = 0.0f;
    wrecked = 0;
    for (i = 0; i < 8; i++) {
        hits[i].timer = 0.0f;
    }
}

inline JumpComboTracker::JumpComboTracker(StuntFactory* h, int c) : CountTracker(h, c, 0.001f, 0) {}

inline ThrowTracker::ThrowTracker(StuntFactory* h, int c) : CountTracker(h, c, 0.5f, 0) {
    handle = 0;
    active = 0;
}

inline SpinTracker::SpinTracker(StuntFactory* h, int c) : CountTracker(h, c, 999.0f, 0) {
    car2 = c;
    f27C = 0.0f;
}

inline CloseCallTracker::CloseCallTracker(StuntFactory* h, int c, CountSound* cs) : CountTracker(h, c, 2.5f, 0x41206) {
    snd = fn_802076A0("missflag.sfx", 1, car, 0);
    flags = 0;
}

inline DriftTracker::DriftTracker(StuntFactory* h, int c, CountSound* cs) : TimedTracker(h, c, 15.0f, 0x40902) {
    snd = 0;
    if (snd) {
        snd->SetVolume(f254);
    }
    blip = 0;
}

inline AirTracker::AirTracker(StuntFactory* h, int c, CountSound* cs) : TimedTracker(h, c, 30.5f, 0x40901) { car2 = c; }

inline CountSound::CountSound(const char* a, const char* b, int car) {
    int ok = !fn_80157278(car);
    s1 = 0;
    s0 = 0;
    if (ok) {
        s0 = fn_802076A0(a, 1, car, 0);
        if (s0) {
            s0->SetPitch(1.2f);
            s0->SetVolume(1.0f);
        }
        s1 = fn_802076A0(b, 1, car, 0);
        if (s1) {
            s1->SetPitch(1.2f);
            s1->SetVolume(1.0f);
        }
    }
    on = 1;
}

extern "C" {
static inline void fn_80157560(Tracker* t) {
    ValueTable* vt = fn_80131810("stunts.val");
    ValueTable* def = fn_80131808("stuntdef.val");
    ValueNode* node;
    const char* name;
    if (!vt) {
        vt = def;
    }
    name = t->Name();
    if (vt->HasChild(name)) {
        node = vt->FindChild(name);
    } else if (def->HasChild(name)) {
        node = def->FindChild(name);
    } else {
        node = def->FindChild("default");
    }
    if (node) {
        ValueNode* r;
        int i;
        for (r = node->GetChildAt(0), i = 0; r; r = node->GetChildAt(++i)) {
            s32 rank = r->GetIntValue("rank");
            const char* adj = r->GetString("adj", "");
            s32 thresh = r->GetInt("thresh", 0);
            f32 pts = r->GetFloat("pts", 0.0f);
            struct Rank* e;
            if (pts < 0.0f) {
                pts = def->FindChild("default")->GetChildAt(i)->GetFloat("pts");
            }
            t->ranks.a.count++;
            e = &t->ranks.a.data[rank];
            e->count = 0;
            e->scored = 0;
            e->natural = 0;
            e->total = 0.0f;
            e->pts = pts;
            e->thresh = thresh;
            fn_801345D8(e->adj, sizeof(e->adj), "%s ", adj);
        }
    }
    if (vt != def) {
        fn_80131818(vt);
    }
    fn_80131818(def);
}
}

namespace {
class TrackerArray : public Meter {
public:
    TrackerArray(StuntFactory* hud, int c, f32* hispeed) {
        int i;
        Unused2(&hispeed);
        cnt = new ("cntsound") CountSound("counter.sfx", "fcount.sfx", c);
        list.Push(new ("airtracker") AirTracker(hud, c, cnt));
        list.Push(new ("drifttracker") DriftTracker(hud, c, cnt));
        list.Push(new ("ccalltracker") CloseCallTracker(hud, c, cnt));
        list.Push(new ("360tracker") SpinTracker(hud, c));
        list.Push(new ("throwtracker") ThrowTracker(hud, c));
        list.Push(new ("jmpcmbtracker") JumpComboTracker(hud, c));
        list.Push(new ("carhittracker") CarHitTracker(hud, c));
        list.Push(new ("crashtracker") CrashTracker(hud, c));
        list.Push(new ("ringtracker") RingTracker(hud, c));
        for (i = 0; i < list.Count(); i++) {
            fn_80157560(*list.At(i));
        }
        active = 1;
        car = c;
    }
    virtual ~TrackerArray(void) {
        int i;
        for (i = 0; i < list.a.count; i++) {
            delete list.a.data[i];
        }
        delete cnt;
    }
    virtual void Init(void) {
        int i;
        active = 1;
        lbl_8042FD08[car] = 1.0f;
        for (i = 0; i < list.a.count; i++) {
            list.a.data[i]->Init();
        }
    }
    virtual void Reset(void) {
        int i;
        for (i = 0; i < list.a.count; i++) {
            list.a.data[i]->Reset();
        }
    }
    virtual f32 Rate(const RaceCarRec* rc) {
        f32 sum = 0.0f;
        if (active && !fn_801B2F2C(rc->index) && fn_801B8570(rc->index) && !fn_80161BB8() && fn_80161C4C()) {
            int i;
            for (i = 0; i < list.a.count; i++) {
                if (NearZero(rc->f2690) || list.a.data[i]->InAir()) {
                    sum += list.a.data[i]->Rate(rc);
                }
            }
        }
        cnt->Update();
        return sum;
    }
    virtual void SetActive(u8 on) { active = on; }
    virtual void Hit(f32 mag, int other) {
        if (active) {
            int i;
            for (i = 0; i < list.a.count; i++) {
                list.a.data[i]->Hit(mag, other);
            }
        }
    }
    virtual void Bump(const RaceCarRec* rc, int kind, void* other) {
        if (active) {
            int i;
            for (i = 0; i < list.a.count; i++) {
                list.a.data[i]->Bump(rc, kind, other);
            }
        }
    }
    virtual void Crash(const RaceCarRec* rc) {
        int i;
        for (i = 0; i < list.a.count; i++) {
            list.a.data[i]->Crash(rc);
        }
    }
    virtual void Stop(const RaceCarRec* rc) {
        int i;
        for (i = 0; i < list.a.count; i++) {
            list.a.data[i]->Stop(rc);
        }
        cnt->Update();
    }
    virtual void Award(f32 v, RaceCarRec* rc, int kind) {
        if (active) {
            int i;
            for (i = 0; i < list.a.count; i++) {
                list.a.data[i]->Award(v, rc, kind);
            }
        }
    }
    virtual void Start(const RaceCarRec* rc) {
        int i;
        for (i = 0; i < list.a.count; i++) {
            list.a.data[i]->Start(rc);
        }
    }
    virtual int Count(void) const {
        int i;
        for (i = 0; i < list.a.count; i++) {
            if (list.a.data[i]->Count()) {
                return 1;
            }
        }
        return 0;
    }
    virtual int Pending(f32* out) const {
        int i;
        for (i = 0; i < list.a.count; i++) {
            if (list.a.data[i]->Pending(out)) {
                return 1;
            }
        }
        return 0;
    }
    virtual void Fill(StuntFactory* l) const {
        int i;
        for (i = 0; i < list.a.count; i++) {
            list.a.data[i]->Fill(l);
        }
    }
    virtual void Land(const RaceCarRec* rc) {
        int i;
        for (i = 0; i < list.a.count; i++) {
            list.a.data[i]->Land(rc);
        }
    }
    virtual s32 Total(void) const {
        int i;
        for (i = 0; i < list.a.count; i++) {
            if (list.a.data[i]->Total()) {
                return 1;
            }
        }
        return 0;
    }
    virtual void Dump(void) {
        int i;
        for (i = 0; i < list.a.count; i++) {
            list.a.data[i]->Dump();
        }
    }

    FixedList<Tracker*, 16> list;  // offset 0x4
    CountSound* cnt;               // offset 0x50
    u8 active;                     // offset 0x54
    s32 car;                       // offset 0x58
};
}  // namespace


void* operator new(size_t size, const char* tag);

extern "C" {

int fn_80157278(int car) { return (u32)lbl_804348E8[car]->type == 'ACAR'; }


Meter* fn_801572A4(StuntFactory* hud, int car, f32* hispeed) {
    lbl_8042FD08[car] = 1.0f;
    return new ("arraytracker") TrackerArray(hud, car, hispeed);
}


}  // extern "C"


extern "C" {


void fn_801584D4(s32 type, int car, int rank) {
    int i;
    for (i = 0; i < lbl_8042FD34.a.count; i++) {
        lbl_8042FD34.a.data[i]->Notify(type, car, rank);
    }
}

void fn_80158558(StuntWatcher* w) {
    StuntWatcher** d = lbl_8042FD34.a.data;
    int n = lbl_8042FD34.a.count++;
    d[n] = w;
}

void fn_8015857C(StuntWatcher* w) {
    int k = lbl_8042FD34.Find(w);
    if (k == -1) {
        return;
    }
    lbl_8042FD34.Remove(k);
}

int fn_80158618(const char* name) {
    int i;
    for (i = 0; i < ARRAY_COUNT(lbl_8032E540); i++) {
        if (strcasecmp(lbl_8032E540[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

}  // extern "C"


// Pragma unproven: this unit-local nested-inline limit preserves retail's helper calls
// and reverse first-use emission. All 169 bodies and the source-selected retail DOL match
// for limits 319 through 369; default 512 expands five Stop/Crash paths. Original policy unknown.
#pragma inline_max_size(350)  // interior of the measured 319..369 window (320 and 350 both full-link verified, handoff/codex_stunts_budget350_independent_2026-11-17.md)
