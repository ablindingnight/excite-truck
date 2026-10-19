// Stunt trackers (0x80157278-0x8015EC50): every car's stunt meter is an array of trackers (air, drift, close
// call, 360, car throw, jump combo, car hit, crash, ring) that time or count a stunt, rank it against the
// thresholds in stunts.val / stuntdef.val, award points and drive the HUD's stunt pop-up. Names are guesses
// apart from the strings.
#include "game/common.h"

#define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))

struct RaceCarVec3 {
    f32 x;
    f32 y;
    f32 z;
};

struct Wheel {
    s32 contact;  // offset 0x0
    u8 pad4[0x74 - 0x4];
};

struct RaceCarRec {
    u8 pad0[0x10];
    u32 model;         // offset 0x10
    u8 pad14[0x3C - 0x14];
    RaceCarVec3 f3c;   // offset 0x3C
    u8 pad48[0x60 - 0x48];
    f32 pos[3];        // offset 0x60
    u8 pad6C[0x118 - 0x6C];
    RaceCarVec3 f118;  // offset 0x118
    u8 pad124[0x1EC - 0x124];
    s32 id;     // offset 0x1EC
    s32 kind;   // offset 0x1F0
    u8 pad1F4[0x2410 - 0x1F4];
    s32 w2410;  // offset 0x2410 (first of four 0x74-byte wheel records)
    u8 pad2414[0x258C - 0x2414];
    f32 f258C;  // offset 0x258C
    u8 pad2590[0x259C - 0x2590];
    u8 b259C;   // offset 0x259C
    u8 pad259D[0x25BC - 0x259D];
    f32 turbo;  // offset 0x25BC
    u8 pad25C0[0x25D4 - 0x25C0];
    f32 speed;  // offset 0x25D4
    u8 pad25D8[0x2620 - 0x25D8];
    s32 air;    // offset 0x2620 (0 while airborne)
    u8 pad2624[0x2690 - 0x2624];
    f32 f2690;  // offset 0x2690
    f32 F2690(void) { return f2690; }
    f32 Speed(void) { return speed; }
    s32 Air(void) { return air; }
    Wheel* Wheels(void) { return (Wheel*)&w2410; }
};

struct LocalCarDesc {
    u8 pad0[0x4];
    u32 flags;  // offset 0x4
};

extern RaceCarRec* lbl_804348E8[];

class Sound {
public:
    virtual ~Sound(void);
    void SetPitch(f32 v) {
        if (v != pitch) {
            pitchDirty = 1;
            pitch = v;
        }
    }
    void SetVolume(f32 v) {
        if (v != vol) {
            volDirty = 1;
            vol = v;
        }
    }
    f32 pitch;  // offset 0x4
    f32 vol;    // offset 0x8
    u8 padC[0x19 - 0xC];
    u8 b19;         // offset 0x19
    u8 pad1A[0x49 - 0x1A];
    u8 play;        // offset 0x49
    u8 b4A;         // offset 0x4A
    u8 b4B;         // offset 0x4B
    u8 b4C;         // offset 0x4C
    u8 volDirty;    // offset 0x4D
    u8 pitchDirty;  // offset 0x4E
};

// The HUD's stunt pop-up (StuntEvent in betarace) and a results line share these slots.
class StuntText {
public:
    virtual ~StuntText(void);
    virtual void Text0(const char* fmt, ...);  // 0x0C
    virtual void Text1(const char* fmt, ...);  // 0x10
    virtual void Text2(const char* fmt, ...);  // 0x14
    virtual void SetValue(f32 v);              // 0x18
    virtual void SetPoints(s32 p);             // 0x1C
    virtual void SetType(s32 t);               // 0x20
    virtual void Stop(void);                   // 0x24
    virtual void SetReady(u8 r);               // 0x28
};

class StuntHud {
public:
    virtual ~StuntHud(void);
    virtual StuntText* Alloc(void);  // 0x0C
};

class StuntList {
public:
    virtual ~StuntList(void);
    virtual StuntText* Add(void);  // 0x0C
};

// Something told about every awarded stunt (achievements, statistics).
class StuntWatcher {
public:
    virtual void Notify(s32 type, int car, int rank);  // 0x08
};

extern "C" {
void* memset(void* d, int c, size_t n);
int strcasecmp(const char* a, const char* b);
int fn_801345D8(char* buf, u32 size, const char* fmt, ...);  // snprintf
void fn_802E30C8(const char* fmt, ...);                      // debug print
ValueTable* fn_80131810(const char* name);
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* t);
const char* fn_80131234(const char* key);  // localize
Sound* fn_802076A0(const char* name, int flags, int car, int pan);
f32 fn_80204CFC(const char* name);  // tuning value
char* fn_8023139C(char* buf, int size, u32 value);  // format a score
void fn_801CB6E4(s32 effect, int car);  // stop a rumble effect
void fn_801CB750(s32 effect, int car);
int fn_80141F7C(f32 x);  // near zero
LocalCarDesc* fn_801769CC(void);
int fn_80179650(void);
int fn_801CF064(void);
int fn_801B2F2C(int id);
int fn_801B8570(int id);
int fn_80161BB8(void);
int fn_80161C4C(void);
}

void AchievementsAdd(int category, s32 value);

extern f32 lbl_80558DC4[];  // FLT_EPSILON (MSL)
inline int NearZero(f32 x) { return (f32)__fabs(x) <= lbl_80558DC4[0]; }
inline f32 Clamp01(f32 x) {
    f32 r = 1.0f < x ? 1.0f : x;
    return r > 0.0f ? r : 0.0f;
}
inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

template <typename T>
struct ArrayRef {
    ArrayRef(T* d, int n) {
        data = d;
        cap = n;
        count = 0;
    }
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
    FixedList(void) : a(storage, N) {}
    s32 Count(void) { return a.count; }
    T* At(int i) { return &a.data[i]; }
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
static FixedList<StuntWatcher*, 8> lbl_8042FD34;
UnkStatic7F lbl_8055E288;

f32 lbl_8042FD08[8];  // per-car score scale


// One car's stunt meter (betarace's Meter): the tracker array below and every tracker implement it.
class Meter {
public:
    Meter(void) {}
    virtual ~Meter(void) {}                       // 0x08
    virtual void Init(void) = 0;                  // 0x0C
    virtual void Reset(void) {}                   // 0x10
    virtual f32 Rate(RaceCarRec* rc) = 0;         // 0x14
    virtual void Dump(void) = 0;                  // 0x18
    virtual void SetActive(u8 on) {}              // 0x1C
    virtual void Hit(f32 mag, int other) {}       // 0x20
    virtual void Bump(RaceCarRec* rc, int kind, void* other) {}  // 0x24
    virtual void Stop(RaceCarRec* rc) {}          // 0x28
    virtual void Award(f32 v, RaceCarRec* rc, int kind) {}  // 0x2C
    virtual void Start(RaceCarRec* rc) {}         // 0x30
    virtual void Land(RaceCarRec* rc) {}          // 0x34
    virtual s32 Total(void) { return 0; }         // 0x38
    virtual void Crash(RaceCarRec* rc) = 0;       // 0x3C
    virtual int Pending(f32* out) { return 0; }   // 0x40
    virtual int Count(void) { return 0; }         // 0x44
    virtual void Fill(StuntList* l) = 0;          // 0x48
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

// A stunt kind with its rank table, its HUD pop-up and its share of the score.
class Tracker : public Meter {
public:
    Tracker(StuntHud* h, int c) {
        car = c;
        ranks.a.count = 0;
        hud = h;
        evt = 0;
    }
    virtual ~Tracker(void) {}
    virtual void Init(void) {
        int i;
        for (i = 0; i < ranks.a.count; i++) {
            ranks.a.data[i].count = 0;
            ranks.a.data[i].scored = 0;
            ranks.a.data[i].natural = 0;
            ranks.a.data[i].total = 0.0f;
        }
    }
    virtual void Dump(void) {
        int count = 0;
        int scored = 0;
        f32 total = 0.0f;
        int i;
        fn_802E30C8("\n%s:", Label());
        fn_802E30C8("   Rank         Count   Unscored   Natural   Score");
        fn_802E30C8("--------------------------------------------------");
        for (i = 0; i < ranks.a.count; i++) {
            struct Rank* r = &ranks.a.data[i];
            fn_802E30C8("%d: %-12.12s %3d     %4d       %3d       %10.0f", i, r->adj, r->count, r->count - r->scored,
                        r->natural, r->total);
            total += r->total;
            count += r->count;
            scored += r->scored;
        }
        fn_802E30C8("Total count: %d (%d unscored) (%.0f points)", count, count - scored, total);
    }
    virtual void Start(RaceCarRec* rc) {}
    virtual s32 Total(void) { return 0; }
    virtual void Fill(StuntList* l) {
        int n = 0;
        int i;
        StuntText* line;
        for (i = 0; i < ranks.a.count; i++) {
            n += ranks.a.data[i].scored;
        }
        line = l->Add();
        if (line) {
            f32 total;
            char buf[0x10];
            s32 pts;
            line->Text0("%s", StatsLabel());
            line->Text1("%d", n);
            line->SetType(Type());
            total = 0.0f;
            for (i = 0; i < ranks.a.count; i++) {
                total += ranks.a.data[i].total;
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
    virtual int FirstRank(void) { return 1; }  // 0x4C
    virtual f32 Points(int rank) {             // 0x50
        if (rank < FirstRank()) {
            return 0.0f;
        }
        return ranks.a.data[rank].pts;
    }
    virtual int FindRank(s32 v) {  // 0x54
        int i;
        for (i = 0; i < ranks.a.count; i++) {
            s32 t = ranks.a.data[i].thresh;
            if (v < t || t < 0) {
                return i;
            }
        }
        return 0;
    }
    virtual s32 Value(void) = 0;                          // 0x58
    virtual int Rank(void) { return FindRank(Value()); }  // 0x5C
    virtual f32 Progress(void) {                          // 0x60
        int r = Rank();
        f32 f = 0.0f;
        if (r < ranks.a.count - 1) {
            s32 lo = r > 0 ? ranks.a.data[r - 1].thresh : 0;
            s32 v = Value();
            f32 span = (f32)(ranks.a.data[r].thresh - lo);
            f32 cur = (f32)(v - lo);
            if (span > 0.0f) {
                f = cur / span;
            }
        }
        return (f32)r + f;
    }
    virtual const char* Label(void) { return fn_80131234(lbl_8032E668[Type()]); }       // 0x64
    virtual const char* SuperLabel(void) { return fn_80131234(lbl_8032E7F8[Type()]); }  // 0x68
    virtual const char* StatsLabel(void) { return fn_80131234(lbl_8032E9E0[Type()]); }  // 0x6C
    virtual const char* Name(void) { return lbl_8032E540[Type()]; }                     // 0x70
    virtual s32 MinThresh(void) {                                                       // 0x74
        int i;
        for (i = 0; i < ranks.a.count; i++) {
            if (ranks.a.data[i].thresh >= 0) {
                return ranks.a.data[i].thresh;
            }
        }
        return 0;
    }
    virtual s32 MaxThresh(void) {  // 0x78
        int i;
        for (i = ranks.a.count; i > 0; i--) {
            if (ranks.a.data[i].thresh >= 0) {
                return ranks.a.data[i].thresh;
            }
        }
        return 0;
    }
    virtual s32 Type(void) = 0;  // 0x7C
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
    virtual int InAir(void) { return 0; }  // 0x90

    int NumRanks(void) { return ranks.a.count; }

    s32 car;                                 // offset 0x4
    FixedList<struct Rank, 10> ranks;        // offset 0x8
    StuntHud* hud;                           // offset 0x244
    StuntText* evt;                          // offset 0x248
};

inline int IsZero(f32 x);

struct Blip {
    void Clear(void) { x8 = 0; }
    u8 pad0[0x8];
    s32 x8;  // offset 0x8
};

// A stunt that lasts: air time, drift time. Scored when it ends, by its duration.
class TimedTracker : public Tracker {
public:
    TimedTracker(StuntHud* h, int c, f32 min, s32 fx) : Tracker(h, c) {
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
    virtual void Reset(void) {
        Hide();
        dur = 0.0f;
        StopEffect();
        f264 = 0.0f;
        since = 0.016683351f;
    }
    virtual f32 Rate(RaceCarRec* rc) {
        f32 r;
        if (NearZero(rc->f2690) && Airborne(rc)) {
            if (dur > minT) {
                Show();
            }
            {
                f32 d = 0.016683351f * rc->speed;
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
        } else if (NearZero(since)) {
            r = Finish(rc);
        } else {
            Hide();
            since += 0.016683351f;
            StopEffect();
        }
        if (NumRanks() - 1 == Rank()) {
            r = Finish(rc);
        }
        return r;
    }
    virtual void Stop(RaceCarRec* rc) {
        if (dur > 0.0f) {
            Finish(rc);
        }
    }
    virtual void Crash(RaceCarRec* rc) { Finish(rc); }
    virtual s32 Value(void) { return dur; }
    virtual int Airborne(RaceCarRec* rc) = 0;  // 0x94
    virtual void Ended(f32 d) {}               // 0x98

    void StopEffect(void) {
        if (f260 > minT) {
            if (effect) {
                fn_801CB750(effect, car);
            }
            f260 = 0.0f;
        }
    }
    f32 Finish(RaceCarRec* rc) {
        f32 pts = 0.0f;
        if (dur > minT && Rank() > 0) {
            pts = Points(Rank()) * mult;
            Score(&pts);
            Hide();
        }
        if (!IsZero(pts)) {
            Ended(dur);
        }
        since += 0.016683351f;
        dur = 0.0f;
        f264 = 0.0f;
        return pts;
    }

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

inline int IsZero(f32 x) { return NearZero(x); }

// A stunt that is counted: close calls, car hits, rings... Each one extends a short window; the count is
// scored when the window runs out.
class CountTracker : public Tracker {
public:
    CountTracker(StuntHud* h, int c, f32 w, s32 fx) : Tracker(h, c) {
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
    virtual void Reset(void) {
        count = 0;
        if (playing > 0) {
            if (effect) {
                fn_801CB6E4(effect, car);
            }
            playing = 0;
        }
        timer = 0.0f;
        mult = 1.0f;
        Hide();
    }
    virtual f32 Rate(RaceCarRec* rc) {
        f32 r;
        int full;
        {
            f32 t = timer - 0.016683351f;
            timer = 0.0f > t ? 0.0f : t;
        }
        if (rc->air == 0 && !Grounded()) {
            timer = timer > 0.033366702f ? timer : 0.033366702f;
        }
        r = 0.0f;
        full = 0;
        if (Capped() && ranks.a.count - 1 == Rank()) {
            full = 1;
        }
        if (NearZero(timer) || full) {
            if (count > 0) {
                r = Collect(rc);
            }
            if (!full && playing > 0) {
                if (effect) {
                    fn_801CB6E4(effect, car);
                }
                playing = 0;
            }
        } else if (count > 0) {
            Refresh();
        }
        return r;
    }
    virtual void Stop(RaceCarRec* rc) {
        if (count > 0) {
            Collect(rc);
        }
    }
    virtual void Crash(RaceCarRec* rc) { Collect(rc); }
    virtual s32 Value(void) { return count; }
    virtual int Grounded(void) { return 1; }  // 0x94
    virtual s32 Shown(void) { return count; }  // 0x98
    virtual int Capped(void) { return 1; }    // 0x9C

    f32 Collect(RaceCarRec* rc) {
        f32 pts = Points(Rank()) * mult;
        Score(&pts);
        Hide();
        count = 0;
        mult = 1.0f;
        return pts;
    }
    void StopEffect(void) {
        if (playing > 0) {
            if (effect) {
                fn_801CB6E4(effect, car);
            }
            playing = 0;
        }
    }

    u8 pad24C[0x250 - 0x24C];
    f32 window;   // offset 0x250
    f32 timer;    // offset 0x254
    s32 effect;   // offset 0x258
    f32 mult;     // offset 0x25C
    s32 playing;  // offset 0x260
    s32 count;    // offset 0x264
    Blip* blip;   // offset 0x268
};

extern "C" int fn_80157278(int car);

// Air time. Doubled when both wheels... (flags from the landing), zero in modes that need a boost.
class AirTracker : public TimedTracker {
public:
    AirTracker(StuntHud* h, int c) : TimedTracker(h, c, 30.5f, 0x40901) { car2 = c; }
    virtual ~AirTracker(void) {}
    virtual void Init(void) {
        b273 = 0;
        b270 = 0;
        b271 = 0;
        b272 = 0;
        TimedTracker::Init();
    }
    virtual f32 Rate(RaceCarRec* rc);
    virtual void Stop(RaceCarRec* rc);
    virtual void Award(f32 v, RaceCarRec* rc, int kind) {
        if (kind == 1) {
            b270 = 1;
        } else if (kind == 0) {
            b271 = 1;
        }
    }
    virtual int Pending(f32* out) {
        if (b273) {
            if (out) {
                f32 v = last;
                f32 hi = MaxThresh();
                f32 lo = MinThresh();
                *out = Lerp(0.0f, 1.0f, Clamp01((v - lo) / (hi - lo)));
            }
            return 1;
        }
        return 0;
    }
    virtual int Count(void) { return Rank() > 0; }
    virtual f32 Points(int rank) {
        f32 p = Tracker::Points(rank);
        if (fn_801769CC()->flags & 0x200000) {
            if (b271) {
                return p * (b270 ? 2.0f : 1.0f);
            }
            return 0.0f;
        }
        return p;
    }
    virtual s32 Type(void) { return 0; }
    virtual int Airborne(RaceCarRec* rc) { return rc->air == 0; }
    virtual void Ended(f32 d) {
        last = d;
        b272 = 1;
    }

    u8 b270;   // offset 0x270
    u8 b271;   // offset 0x271
    u8 b272;   // offset 0x272
    u8 b273;   // offset 0x273
    f32 last;  // offset 0x274
    s32 car2;  // offset 0x278
};

inline void Unused(RaceCarRec** p) {}
inline void Unused2(f32** p) {}

#pragma dont_inline on
f32 AirTracker::Rate(RaceCarRec* rc) {
    f32 r = 0.0f;
    b272 = 0;
    if (IsZero(rc->F2690()) && Airborne(rc)) {
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
    } else if (IsZero(since)) {
        r = Finish(rc);
    } else {
        Hide();
        since += 0.016683351f;
        StopEffect();
    }
    if (NumRanks() - 1 == Rank()) {
        r = Finish(rc);
    }
    b273 = b272 && b271;
    b270 = 0;
    b271 = 0;
    return r;
}

void AirTracker::Stop(RaceCarRec* rc) {
    s32 n;
    Unused(&rc);
    if (dur > 0.0f) {
        Finish(rc);
    }
    n = ranks.a.data[ranks.a.count - 1].scored;
    if (n > 0 && fn_80179650() == 1 && rc->kind == 0x50 && !fn_801CF064()) {
        AchievementsAdd(3, n);
    }
}
#pragma dont_inline reset

struct RaceCarVec {
    f32 x;
    f32 y;
    f32 z;
};

extern "C" f32 PSVECDotProduct(const void* a, const void* b);

inline void StopSound(Sound* s) {
    if (s) {
        s->b4A = 1;
        s->b4C = 0;
    }
}

inline void PlaySound(Sound* s) {
    s->play = 1;
    s->b4B = 0;
    s->b4A = 0;
}

inline void LoopSound(Sound* s) {
    s->b4A = 0;
    s->b4B = 1;
    s->play = 1;
}

// Drift time; plays the skid sound while sliding fast enough sideways.
class DriftTracker : public TimedTracker {
public:
    DriftTracker(StuntHud* h, int c) : TimedTracker(h, c, 15.0f, 0x40902) {
        snd = 0;
        if (snd) {
            snd->SetVolume(1.0f);
        }
        blip = 0;
    }
    virtual ~DriftTracker(void) {
        if (snd) {
            delete snd;
        }
    }
    virtual void Init(void) {
        TimedTracker::Init();
        sliding = 1;
        ground = 0.0f;
        StopSound(snd);
    }
    virtual void Reset(void) {
        TimedTracker::Reset();
        StopSound(snd);
    }
    virtual void Stop(RaceCarRec* rc);
    virtual void Start(RaceCarRec* rc) {
        if (snd) {
            if (!Sliding(rc, sliding, rc->speed)) {
                StopSound(snd);
            }
        }
    }
    virtual s32 Type(void) { return 3; }
    virtual int Airborne(RaceCarRec* rc) {
        u8 was;
        f32 amount;
        if (rc->air == 0) {
            ground += 0.016683351f;
        } else {
            ground = 0.0f;
        }
        amount = 0.0f;
        was = sliding;
        sliding = Sliding(rc, was, rc->speed);
        if (sliding) {
            f32 spd = rc->speed;
            f32 side = (f32)__fabs(PSVECDotProduct(&rc->f118, &rc->f3c));
            f32 k = was ? 0.15f : 0.35f;
            f32 fade = 1.0f - 8.0f * ground;
            fade = fade > 0.0f ? fade : 0.0f;
            amount = Clamp01(5.0f * (side / (spd * k) - 1.0f)) * fade;
        }
        if (snd) {
            if (!NearZero(amount)) {
                snd->SetVolume(0.4f * amount);
                LoopSound(snd);
            } else {
                StopSound(snd);
            }
        }
        return sliding;
    }

    int Sliding(RaceCarRec* rc, u8 was, f32 spd) {
        return NearZero(rc->f2690) && spd > 5.0f && ground < 0.5f &&
               (f32)__fabs(PSVECDotProduct(&rc->f118, &rc->f3c)) > spd * (was ? 0.15f : 0.35f);
    }

    f32 ground;  // offset 0x270
    u8 sliding;  // offset 0x274
    Sound* snd;  // offset 0x278
};

#pragma dont_inline on
void DriftTracker::Stop(RaceCarRec* rc) {
    s32 n;
    Unused(&rc);
    if (dur > 0.0f) {
        Finish(rc);
    }
    n = ranks.a.data[ranks.a.count - 1].scored;
    if (n > 0 && fn_80179650() == 1 && rc->kind == 0x50 && !fn_801CF064()) {
        AchievementsAdd(5, n);
    }
    StopSound(snd);
}
#pragma dont_inline reset

struct CloseCallSlot {
    void* other;  // offset 0x0
    f32 time;     // offset 0x4
    u8 done;      // offset 0x8
};

extern f32 lbl_8055E3C8;  // race clock
extern f32 lbl_8055A02C;  // close-call speed

extern "C" f32 fn_801429D0(f32 a, f32 b);  // max

// Near misses with other cars ("close call", "turbo close call" while boosting).
class CloseCallTracker : public CountTracker {
public:
    CloseCallTracker(StuntHud* h, int c) : CountTracker(h, c, 2.5f, 0x41206) {
        snd = fn_802076A0("missflag.sfx", 1, car, 0);
        flags = 0;
    }
    virtual ~CloseCallTracker(void) {
        if (snd) {
            delete snd;
        }
    }
    virtual void Init(void) {
        int i;
        CountTracker::Init();
        for (i = 0; i < ARRAY_COUNT(slots); i++) {
            slots[i].other = 0;
            slots[i].time = -1000.0f;
            slots[i].done = 1;
        }
        flags = 0;
        turbo = 0;
        turboTime = -100.0f;
    }
    virtual f32 Rate(RaceCarRec* rc);
    virtual void Bump(RaceCarRec* rc, int kind, void* other);
    virtual void Stop(RaceCarRec* rc);
    virtual void Land(RaceCarRec* rc) {
        if (!turbo && rc->turbo > lbl_80558DC4[0]) {
            turbo = 1;
            turboTime = lbl_8055E3C8;
        }
    }
    virtual s32 Total(void) { return turbo; }
    virtual const char* Label(void) { return fn_80131234(lbl_8032E668[Type()]); }
    virtual s32 Type(void) { return 4; }

    CloseCallSlot slots[8];  // offset 0x26C
    s32 flags;               // offset 0x2CC
    u8 b2D0;                 // offset 0x2D0
    u8 turbo;                // offset 0x2D1
    f32 turboTime;           // offset 0x2D4
    Sound* snd;              // offset 0x2D8
};

#pragma dont_inline on
f32 CloseCallTracker::Rate(RaceCarRec* rc) {
    f32 now = lbl_8055E3C8;
    f32 r;
    int full;
    int any;
    int n;
    int i;
    if (turbo) {
        if (now - turboTime > 10.0f || !(rc->turbo > lbl_80558DC4[0])) {
            turbo = 0;
        }
    }
    if (count == 0) {
        int boost;
        flags = 0;
        b2D0 = 1;
        boost = rc->f258C > 0.001f || rc->b259C;
        if (boost || rc->air == 0) {
            flags |= 2;
        } else {
            flags &= ~2;
        }
    } else if (rc->air > 0) {
        for (i = 0; i < 4; i++) {
            if (rc->Wheels()[i].contact == 0) {
                b2D0 = 0;
                break;
            }
        }
    }
    n = 0;
    any = 0;
    for (i = 0; i < ARRAY_COUNT(slots); i++) {
        CloseCallSlot* s = &slots[i];
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
                    blip->Clear();
                }
            }
            any = 1;
            n++;
        }
    }
    if (n > 0) {
        int boost = rc->f258C > 0.001f || rc->b259C;
        if (!boost && rc->air > 0) {
            flags &= ~2;
        }
    }
    if (count > 0 && Rank() > 0) {
        flags |= 1;
    }
    timer = fn_801429D0(0.0f, timer - 0.016683351f);
    if (!rc->Air() && !Grounded()) {
        timer = fn_801429D0(timer, 0.033366702f);
    }
    r = 0.0f;
    full = 0;
    if (Capped() && NumRanks() - 1 == Rank()) {
        full = 1;
    }
    if (IsZero(timer) || full) {
        if (count > 0) {
            r = Collect(rc);
        }
        if (!full) {
            StopEffect();
        }
    } else if (count > 0) {
        Refresh();
    }
    if (ranks.a.count - 1 == Rank()) {
        flags = 0;
    }
    return r;
}

void CloseCallTracker::Stop(RaceCarRec* rc) {
    s32 n;
    Unused(&rc);
    if (count > 0) {
        Collect(rc);
    }
    n = ranks.a.data[ranks.a.count - 1].scored;
    if (n > 0 && fn_80179650() == 1 && rc->kind == 0x50 && !fn_801CF064()) {
        AchievementsAdd(4, n);
    }
}
#pragma dont_inline reset

void CloseCallTracker::Bump(RaceCarRec* rc, int kind, void* other) {
    if (rc->speed > 0.44444445f * lbl_8055A02C && kind == 1003 && !turbo) {
        f32 now = lbl_8055E3C8;
        int recent = 0;
        CloseCallSlot* oldest;
        int i;
        for (i = 0; i < ARRAY_COUNT(slots); i++) {
            CloseCallSlot* s = &slots[i];
            if (NearZero(s->time - now)) {
                recent = 1;
            }
            if (s->other == other && now - s->time < 5.0f) {
                return;
            }
        }
        oldest = 0;
        for (i = 0; i < ARRAY_COUNT(slots); i++) {
            if (!oldest || slots[i].time < oldest->time) {
                oldest = &slots[i];
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
int fn_80009C84(void);
void fn_80206750(Sound* s, int car, int pan);
}

inline s32 Round(f32 x) { return x < 0.0f ? x - 0.5f : x + 0.5f; }

// One-shot stunts reported with a value (crashes).
class ValueTracker : public Tracker {
public:
    ValueTracker(StuntHud* h, int c) : Tracker(h, c) { car2 = c; }
    virtual ~ValueTracker(void) {}
    virtual void Init(void) {
        Tracker::Init();
        Hide();
        value = -999.0f;
    }
    virtual void Reset(void) {}
    virtual f32 Rate(RaceCarRec* rc) {
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
    virtual void Award(f32 v, RaceCarRec* rc, int kind) {
        if (car2 == rc->id && kind == Kind()) {
            value = v;
        }
    }
    virtual void Crash(RaceCarRec* rc) {}
    virtual s32 Value(void) { return Round(value); }
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
    virtual int Kind(void) = 0;            // 0x94
    virtual void Set(f32 v) { value = v; }  // 0x98

    s32 car2;   // offset 0x24C
    f32 value;  // offset 0x250
};

struct CarHit {
    CarHit(void) { timer = 0.0f; }
    f32 timer;  // offset 0x0
    f32 mag;    // offset 0x4
};

// Being rammed by other cars into a wreck.
class CarHitTracker : public Tracker {
public:
    CarHitTracker(StuntHud* h, int c) : Tracker(h, c) {
        int i;
        hits[0].timer = 0.0f;
        car2 = c;
        f290 = 0.0f;
        f28C = 0.0f;
        wrecked = 0;
        for (i = 0; i < ARRAY_COUNT(hits); i++) {
            hits[i].timer = 0.0f;
        }
    }
    virtual ~CarHitTracker(void) {}
    virtual void Init(void) {
        int i;
        Tracker::Init();
        Hide();
        wrecked = 0;
        f290 = 0.0f;
        f28C = 0.0f;
        for (i = 0; i < ARRAY_COUNT(hits); i++) {
            hits[i].timer = 0.0f;
        }
    }
    virtual f32 Rate(RaceCarRec* rc) {
        f32 r;
        int i;
        wrecked = 0;
        f290 = 0.0f;
        f28C = 0.0f;
        for (i = 0; i < ARRAY_COUNT(hits); i++) {
            CarHit* h = &hits[i];
            if (h->timer > lbl_80558DC4[0]) {
                int w;
                f28C = f28C > h->mag ? f28C : h->mag;
                if (h->timer > lbl_80558DC4[0]) {
                    h->timer -= 0.016683351f;
                }
                w = Wrecked(i);
                if (w || !(h->timer > lbl_80558DC4[0])) {
                    h->timer = 0.0f;
                    wrecked = w;
                    f290 = f290 > h->mag ? f290 : h->mag;
                }
            }
        }
        r = 0.0f;
        if (f28C > lbl_80558DC4[0]) {
            Show();
            Refresh();
            if (f290 > lbl_80558DC4[0]) {
                f32 pts = Points(Rank());
                Score(&pts);
                Hide();
                r = pts;
            }
        }
        return r;
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
    virtual void Stop(RaceCarRec* rc) {
        s32 n;
        if (f28C > lbl_80558DC4[0]) {
            f32 pts = Points(Rank());
            Score(&pts);
            Hide();
        }
        n = ranks.a.data[ranks.a.count - 1].scored;
        if (n > 0 && fn_80179650() == 1 && rc->kind == 0x50 && !fn_801CF064()) {
            AchievementsAdd(7, n);
        }
    }
    virtual void Crash(RaceCarRec* rc) {
        f32 pts = Points(Rank());
        Score(&pts);
        Hide();
    }
    virtual s32 Value(void) {
        if (wrecked) {
            return 0x7FFFFFFF;
        }
        if (f290 > lbl_80558DC4[0]) {
            return f290;
        }
        return f28C;
    }
    virtual s32 Type(void) { return 12; }

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

// Several jumps in a row.
class JumpComboTracker : public CountTracker {
public:
    JumpComboTracker(StuntHud* h, int c) : CountTracker(h, c, 0.001f, 0) {}
    virtual ~JumpComboTracker(void) {}
    virtual void Init(void) {
        CountTracker::Init();
        jumped = 1;
        ground = 0.0f;
        jumps = 0;
        effects = 0;
    }
    virtual void Reset(void) {
        CountTracker::Reset();
        jumped = 0;
        ground = 0.0f;
        jumps = 0;
        if (effects > 0) {
            fn_801CB6E4(0x50204, car);
            effects = 0;
        }
    }
    virtual f32 Rate(RaceCarRec* rc);
    virtual void Award(f32 v, RaceCarRec* rc, int kind) {
        if (kind == 6) {
            jumped = 1;
        }
    }
    virtual s32 Type(void) { return 10; }

    u8 jumped;    // offset 0x26C
    f32 ground;   // offset 0x270
    s32 jumps;    // offset 0x274
    s32 effects;  // offset 0x278
};

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
class ThrowTracker : public CountTracker {
public:
    ThrowTracker(StuntHud* h, int c) : CountTracker(h, c, 0.5f, 0) {
        handle = 0;
        active = 0;
    }
    virtual ~ThrowTracker(void) {}
    virtual void Init(void) {
        CountTracker::Init();
        handle = 0;
        active = 0;
    }
    virtual f32 Rate(RaceCarRec* rc);
    virtual void Award(f32 v, RaceCarRec* rc, int kind) {
        if (kind == 3) {
            int i;
            active = 1;
            handle = *(s32*)&v;
            for (i = 0; i < ARRAY_COUNT(targets); i++) {
                if (i == rc->id) {
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
    virtual s32 Type(void) { return 9; }

    s32 handle;              // offset 0x26C
    ThrowTarget targets[8];  // offset 0x270
    u8 active;               // offset 0x2F0
};

// Spins and rolls.
class SpinTracker : public CountTracker {
public:
    SpinTracker(StuntHud* h, int c) : CountTracker(h, c, 999.0f, 0) {
        car2 = c;
        f27C = 0.0f;
    }
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
            fn_801CB6E4(0x48203, car);
            spins = 0;
        }
        rolls = 0;
        angle = 0.0f;
        f27C = 0.0f;
    }
    virtual f32 Rate(RaceCarRec* rc);
    virtual void Stop(RaceCarRec* rc) {
        spins = Round(angle);
        Crash(rc);
        if (spins > 0) {
            if (spins > 0) {
                fn_801CB6E4(0x48203, car);
                spins = 0;
            }
            angle = 0.0f;
        }
    }
    virtual void Award(f32 v, RaceCarRec* rc, int kind) {
        if (car2 == rc->id && (kind == 7 || kind == 9)) {
            angle = v;
            if (kind == 7) {
                rolls = Round(v);
            }
        }
    }
    virtual void Crash(RaceCarRec* rc);
    virtual s32 Type(void) { return 7; }
    virtual void Score(f32* pts) {
        int deg;
        Tracker::Score(pts);
        deg = spins * 360;
        if (evt && !NearZero(*pts)) {
            evt->Text1("%s %d", Label(), deg);
            evt->Text0("%s %d", SuperLabel(), deg);
        }
    }
    virtual int Capped(void) { return 0; }

    s32 spins;  // offset 0x26C
    s32 car2;   // offset 0x270
    s32 rolls;  // offset 0x274
    f32 angle;  // offset 0x278
    f32 f27C;   // offset 0x27C
};

// Crashing (into the scenery).
class CrashTracker : public ValueTracker {
public:
    CrashTracker(StuntHud* h, int c) : ValueTracker(h, c) {}
    virtual ~CrashTracker(void) {}
    virtual s32 Type(void) { return 13; }
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
    virtual int InAir(void) { return 1; }
    virtual int Kind(void) { return 4; }
};

static const f32 lbl_802F9478[6] = {0.7932f, 0.8904f, 0.9433f, 0.9994f, 1.0f, 0.0f};

// Flying through rings.
class RingTracker : public CountTracker {
public:
    RingTracker(StuntHud* h, int c) : CountTracker(h, c, 2.0f, 0) {
        car2 = c;
        if (!fn_80157278(c)) {
            snd = fn_802076A0("ringhit.sfx", 1, c, 2);
        } else {
            snd = 0;
        }
    }
    virtual ~RingTracker(void) {
        if (snd) {
            delete snd;
        }
    }
    virtual void Init(void) {
        int pan;
        CountTracker::Init();
        pending = 0;
        queued = 0;
        StopSound(snd);
        pan = fn_80009C84() ? 0 : 2;
        if (snd) {
            fn_80206750(snd, car2, pan);
        }
    }
    virtual f32 Rate(RaceCarRec* rc);
    virtual void Stop(RaceCarRec* rc);
    virtual void Award(f32 v, RaceCarRec* rc, int kind) {
        if (kind == 5) {
            pending += Round(v);
        }
    }
    virtual s32 Type(void) { return 14; }

    s32 pending;  // offset 0x26C
    s32 queued;   // offset 0x270
    s32 car2;     // offset 0x274
    Sound* snd;   // offset 0x278
};

#pragma dont_inline on
f32 JumpComboTracker::Rate(RaceCarRec* rc) {
    f32 r;
    int full;
    if (rc->air > 0) {
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
                blip->Clear();
            }
        }
    }
    if (ranks.a.count - 1 == Rank()) {
        jumps = 1;
    }
    if (ground > 0.8f) {
        jumps = 0;
        if (effects > 0) {
            fn_801CB6E4(0x50204, car);
            effects = 0;
        }
    }
    jumped = 0;
    timer = fn_801429D0(0.0f, timer - 0.016683351f);
    if (!rc->Air() && !Grounded()) {
        timer = fn_801429D0(timer, 0.033366702f);
    }
    r = 0.0f;
    full = 0;
    if (Capped() && NumRanks() - 1 == Rank()) {
        full = 1;
    }
    if (IsZero(timer) || full) {
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

f32 ThrowTracker::Rate(RaceCarRec* rc) {
    f32 r;
    int full;
    if (active) {
        int i;
        int alive = fn_801D48FC(handle);
        active = alive;
        for (i = 0; i < ARRAY_COUNT(targets); i++) {
            ThrowTarget* t = &targets[i];
            s32 h = handle;
            int hit;
            t->prev = t->state;
            if (t->state == 0) {
                hit = 0;
            } else {
                RaceCarRec* o = fn_8017960C(t->car);
                if (!o) {
                    hit = 0;
                } else {
                    int air = o->air > 0;
                    switch (t->state) {
                    case 1:
                        if (o) {
                            t->state = air ? 3 : 2;
                        } else {
                            t->state = 0;
                        }
                        break;
                    case 2:
                        if (air) {
                            t->state = 3;
                        }
                        break;
                    case 3:
                        if (!air) {
                            u8 q[8];
                            f32 at[3];
                            t->state = 2;
                            fn_801E9DC4(q);
                            {
                                f32 g = fn_801DDC8C(q, o->pos[0], o->pos[2]);
                                if (g > 4.4444447f && o->f118.y - g > 0.44444445f) {
                                    int n;
                                    fn_801E9DC4(q);
                                    n = fn_801DDA84(q, o->pos[0], o->pos[2], at, 1e-7f, 10);
                                    if (fn_801D4908(h, at, n)) {
                                        t->state = 4;
                                        t->timer = 1.0f;
                                    }
                                }
                            }
                        }
                        break;
                    case 4:
                        t->timer -= 0.016683351f;
                        if (t->timer < 0.0f) {
                            t->state = 5;
                        } else if (air) {
                            t->state = 3;
                        }
                        break;
                    }
                    hit = alive || t->state == 4;
                }
            }
            active = active | hit;
            if (t->state == 5 && t->prev != 5) {
                count++;
                playing++;
                timer = window;
                mult = 1.0f;
                Show();
                if (evt && blip) {
                    blip->Clear();
                }
                fn_80198EA0(i);
            }
        }
        if (!active) {
            handle = 0;
        }
    }
    timer = fn_801429D0(0.0f, timer - 0.016683351f);
    if (!rc->Air() && !Grounded()) {
        timer = fn_801429D0(timer, 0.033366702f);
    }
    r = 0.0f;
    full = 0;
    if (Capped() && NumRanks() - 1 == Rank()) {
        full = 1;
    }
    if (IsZero(timer) || full) {
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

f32 SpinTracker::Rate(RaceCarRec* rc) {
    f32 r;
    int full;
    if (!NearZero(angle)) {
        spins += rolls;
        if (Round(angle) > 0) {
            int add = rolls ? 0 : Round(angle);
            count = playing = spins + add;
            timer = window;
            mult = 1.0f;
            Show();
            if (evt && blip) {
                blip->Clear();
            }
        }
    }
    timer = fn_801429D0(0.0f, timer - 0.016683351f);
    if (!rc->Air() && !Grounded()) {
        timer = fn_801429D0(timer, 0.033366702f);
    }
    r = 0.0f;
    full = 0;
    if (Capped() && NumRanks() - 1 == Rank()) {
        full = 1;
    }
    if (IsZero(timer) || full) {
        if (count > 0) {
            r = Collect(rc);
        }
        if (!full) {
            StopEffect();
        }
    } else if (count > 0) {
        Refresh();
    }
    if (rolls > 0) {
        Crash(rc);
    }
    if (rolls > 0 || rc->air > 0) {
        f27C += fn_801E7E24();
        if (rolls > 0 || f27C > 0.75f) {
            if (spins > 0) {
                fn_801CB6E4(0x48203, car);
                spins = 0;
            }
            angle = 0.0f;
        }
    } else if (rc->air == 0) {
        f27C = 0.0f;
    }
    rolls = 0;
    return r;
}

void SpinTracker::Crash(RaceCarRec* rc) {
    if (spins > 0) {
        Collect(rc);
    }
}

f32 RingTracker::Rate(RaceCarRec* rc) {
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
            blip->Clear();
        }
    }
    pending = 0;
    if (play && !snd->b19) {
        queued--;
        PlaySound(snd);
    }
    timer = fn_801429D0(0.0f, timer - 0.016683351f);
    if (!rc->Air() && !Grounded()) {
        timer = fn_801429D0(timer, 0.033366702f);
    }
    r = 0.0f;
    full = 0;
    if (Capped() && NumRanks() - 1 == Rank()) {
        full = 1;
    }
    if (IsZero(timer) || full) {
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

void RingTracker::Stop(RaceCarRec* rc) {
    s32 n;
    Unused(&rc);
    if (count > 0) {
        Collect(rc);
    }
    n = ranks.a.data[ranks.a.count - 1].scored;
    if (n > 0 && fn_80179650() == 1 && rc->kind == 0x50 && !fn_801CF064()) {
        AchievementsAdd(6, n);
    }
}
#pragma dont_inline reset

// The two count-up sounds of the score counter.
struct CountSound {
    CountSound(const char* a, const char* b, int car) {
        s1 = 0;
        s0 = 0;
        if (!fn_80157278(car)) {
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
                s0->b4A = 0;
                s0->b4B = 1;
                s0->play = 1;
            } else {
                if (s1 && wasOn == 0) {
                    s1->play = 1;
                    s1->b4B = 0;
                    s1->b4A = 0;
                }
                s0->b4A = 1;
                s0->b4C = 0;
            }
        }
        wasOn = on;
        on = 1;
    }
    Sound* s0;  // offset 0x0
    Sound* s1;  // offset 0x4
    s32 on;     // offset 0x8
    s32 wasOn;  // offset 0xC
};

// All of a car's trackers behind one Meter.
class TrackerArray : public Meter {
public:
    TrackerArray(void) {}
    virtual ~TrackerArray(void) {
        int i;
        for (i = 0; i < list.a.count; i++) {
            if (list.a.data[i]) {
                delete list.a.data[i];
            }
        }
        if (cnt) {
            delete cnt;
        }
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
    virtual f32 Rate(RaceCarRec* rc) {
        f32 sum = 0.0f;
        if (active && !fn_801B2F2C(rc->id) && fn_801B8570(rc->id) && !fn_80161BB8() && fn_80161C4C()) {
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
    virtual void Bump(RaceCarRec* rc, int kind, void* other) {
        if (active) {
            int i;
            for (i = 0; i < list.a.count; i++) {
                list.a.data[i]->Bump(rc, kind, other);
            }
        }
    }
    virtual void Crash(RaceCarRec* rc) {
        int i;
        for (i = 0; i < list.a.count; i++) {
            list.a.data[i]->Crash(rc);
        }
    }
    virtual void Stop(RaceCarRec* rc) {
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
    virtual void Start(RaceCarRec* rc) {
        int i;
        for (i = 0; i < list.a.count; i++) {
            list.a.data[i]->Start(rc);
        }
    }
    virtual int Count(void) {
        int i;
        for (i = 0; i < list.a.count; i++) {
            if (list.a.data[i]->Count()) {
                return 1;
            }
        }
        return 0;
    }
    virtual int Pending(f32* out) {
        int i;
        for (i = 0; i < list.a.count; i++) {
            if (list.a.data[i]->Pending(out)) {
                return 1;
            }
        }
        return 0;
    }
    virtual void Fill(StuntList* l) {
        int i;
        for (i = 0; i < list.a.count; i++) {
            list.a.data[i]->Fill(l);
        }
    }
    virtual void Land(RaceCarRec* rc) {
        int i;
        for (i = 0; i < list.a.count; i++) {
            list.a.data[i]->Land(rc);
        }
    }
    virtual s32 Total(void) {
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

void* operator new(size_t size, const char* tag);

extern "C" {

int fn_80157278(int car) { return lbl_804348E8[car]->model == 'ACAR'; }

void fn_80157560(Tracker* t);

#pragma dont_inline on
Meter* fn_801572A4(StuntHud* hud, int car, f32* hispeed) {
    TrackerArray* m;
    lbl_8042FD08[car] = 1.0f;
    m = new ("arraytracker") TrackerArray;
    if (m) {
        int i;
        Unused2(&hispeed);
        m->cnt = new ("cntsound") CountSound("counter.sfx", "fcount.sfx", car);
        m->list.Push(new ("airtracker") AirTracker(hud, car));
        m->list.Push(new ("drifttracker") DriftTracker(hud, car));
        m->list.Push(new ("ccalltracker") CloseCallTracker(hud, car));
        m->list.Push(new ("360tracker") SpinTracker(hud, car));
        m->list.Push(new ("throwtracker") ThrowTracker(hud, car));
        m->list.Push(new ("jmpcmbtracker") JumpComboTracker(hud, car));
        m->list.Push(new ("carhittracker") CarHitTracker(hud, car));
        m->list.Push(new ("crashtracker") CrashTracker(hud, car));
        m->list.Push(new ("ringtracker") RingTracker(hud, car));
        for (i = 0; i < m->list.Count(); i++) {
            fn_80157560(*m->list.At(i));
        }
        m->active = 1;
        m->car = car;
    }
    return m;
}
#pragma dont_inline reset

void fn_80157560(Tracker* t) {
    ValueTable* vt = fn_80131810("stunts.val");
    ValueTable* def = fn_80131808("stuntdef.val");
    const char* name;
    ValueNode* node;
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
        int i = 0;
        ValueNode* r;
        for (r = node->GetChildAt(0); r; r = node->GetChildAt(++i)) {
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

void fn_801584D4(s32 type, int car, int rank) {
    int i;
    for (i = 0; i < lbl_8042FD34.a.count; i++) {
        lbl_8042FD34.a.data[i]->Notify(type, car, rank);
    }
}

void fn_80158558(StuntWatcher* w) { lbl_8042FD34.a.data[lbl_8042FD34.a.count++] = w; }

void fn_8015857C(StuntWatcher* w) {
    int i;
    int k;
    for (i = 0; i < lbl_8042FD34.a.count; i++) {
        if (lbl_8042FD34.a.data[i] == w) {
            goto found;
        }
    }
    i = -1;
found:
    k = i;
    if (k == -1) {
        return;
    }
    if (k >= 0 && k < lbl_8042FD34.a.count) {
        if (k != lbl_8042FD34.a.count - 1) {
            lbl_8042FD34.a.data[k] = lbl_8042FD34.a.data[lbl_8042FD34.a.count - 1];
        }
        lbl_8042FD34.a.count--;
    }
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
