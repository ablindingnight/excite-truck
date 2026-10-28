// Orbit (0x801659D0-0x80167834): the computer trucks' rubber band around the player. orbit.val (the
// "orbit_table" tuning subtable) lists "role"s, target distances to the player that cycle smoothly between
// min_lon and max_lon, and a "config" block. Each computer truck takes a free role and steers its speed
// factor (and its lane: ideal line or the crazy line) toward that distance; far ahead or behind it gets
// the park / suicide / infinite turbo / tree / low gravity flags. Near the finish the distances freeze,
// and when the player cannot finish in time the "turtle" mode turns it all off. Names are guesses apart
// from the strings.
#include "game/common.h"

extern "C" {
int strcmp(const char* a, const char* b);
char* strchr(const char* s, int c);
double atof(const char* s);
double fmod(double x, double y);
void* memset(void* dst, int c, u32 n);
void fn_802E30C8(const char* fmt, ...);  // debug print
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* t);
f32 fn_80204CFC(const char* name);          // tuning value
const char* fn_80204EA0(const char* name);  // tuning string
f32 fn_80129DC4(int seed, f32 lo, f32 hi);  // random float
f32 fn_8013672C(void);                      // center line length
s32 fn_800081F0(const void* race);                // laps
}  // extern "C"
f32 Max(float, float);
extern "C" {
}  // extern "C"
f32 Remap(float, float, float, float, float);
extern "C" {
}  // extern "C"
f32 Clamp(float, float, float);
extern "C" {
int fn_80179630(int view);
int fn_801B8570(int id);
void fn_80139E18(void* b);
int fn_80139EC4(void* b, f32 x, f32 z);
}

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

#define CLAMP01(x) ((x) = MIN(1.0f, (x)), (x) = MAX((x), 0.0f))

extern f32 lbl_80558DC4[];  // FLT_EPSILON (MSL)
extern f32 lbl_8055E3C8;    // race time

struct EventDesc {
    s32 event;  // offset 0x0
};

struct Event {
    u8 pad0[0x20];
    ValueNode* vals;  // offset 0x20
};

extern "C" EventDesc* fn_801769CC(void);
Event* GetEvent(int id);

struct CarProgress {
    u8 pad0[0x4];
    s32 lap;  // offset 0x4
    u8 pad8[0x10 - 0x8];
    s32 laps;  // offset 0x10
    u8 pad14[0x24 - 0x14];
    s32 done;  // offset 0x24
};

struct CarTrackPos {
    u8 pad0[0x48];
    f32 lon;  // offset 0x48
};

struct CarEngine {
    u8 pad0[0x20];
    f32 topSpeed;  // offset 0x20
    f32 throttle;  // offset 0x24
};

struct CarPhysics {
    u8 pad0[0x4];
    CarEngine* engine;  // offset 0x4
};

class OrbitCar {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual CarProgress* Progress(void);  // 0x10
    virtual void vf14(void);
    virtual CarTrackPos* TrackPos(void);  // 0x18
    virtual CarPhysics* Physics(void);    // 0x1C
};

class LineSet {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void vf18(void);
    virtual void SetCrazy(int car, int on);  // 0x1C
};

struct CarRec {
    u8 pad0[0x60];
    f32 x;  // offset 0x60
    u8 pad64[0x4];
    f32 z;  // offset 0x68
    u8 pad6C[0x25CC - 0x6C];
    f32 speed;  // offset 0x25CC
};

struct RaceDesc {
    u8 pad0[0x4];
    s32 type;  // offset 0x4
};

extern "C" {
OrbitCar* fn_801B2EC0(int id);
LineSet* fn_80156A40(void);
CarRec* fn_8017960C(int id);
}

template <class T>
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

template <class T, int N>
struct FixedArr : ArrayRef<T> {
    FixedArr(void) : ArrayRef<T>(storage, N) {}
    T storage[N];
};

template <class T>
struct DynArray {
    DynArray(int n) : a(new ("dsa_buf") T[n], n) { alloc = a.data; }
    ~DynArray(void) { delete[] alloc; }
    T* Add(void) { return &a.data[a.count++]; }
    T* At(int i) { return &a.data[i]; }
    int Count(void) const { return a.count; }
    ArrayRef<T> a;  // offset 0x0
    T* alloc;       // offset 0xC
};

// A value moving linearly from -> to over [t0, t1].
struct Ramp {
    f32 from;  // offset 0x0
    f32 to;    // offset 0x4
    f32 t0;    // offset 0x8
    f32 t1;    // offset 0xC
    Ramp(f32 r) {
        rate = r;
        from = to = 0.0f;
        t0 = t1 = 0.0f;
    }
    f32 rate;  // offset 0x10
};

// A value cycling smoothly between min and max (a full cycle is 4 * half).
struct Osc {
    f32 min;    // offset 0x0
    f32 max;    // offset 0x4
    f32 half;   // offset 0x8
    f32 start;  // offset 0xC
    f32 Period(void) { return 2.0f * half; }
};

struct Role : Osc {
    s32 owner;  // offset 0x10
};

struct OrbitConfig {
    f32 minLon;         // offset 0x0
    f32 maxLon;         // offset 0x4
    f32 minSf;          // offset 0x8
    f32 maxSf;          // offset 0xC
    f32 setFreq;        // offset 0x10
    f32 setSpeed;       // offset 0x14
    f32 minCloseLon;    // offset 0x18
    f32 maxCloseLon;    // offset 0x1C
    f32 minCloseSf;     // offset 0x20
    f32 maxCloseSf;     // offset 0x24
    f32 switchLon;      // offset 0x28
    f32 crazyLon;       // offset 0x2C
    f32 suicideLon;     // offset 0x30
    f32 parkLon;        // offset 0x34
    f32 infTurboLon;    // offset 0x38
    f32 crazyWid;       // offset 0x3C
    f32 treeLon;        // offset 0x40
    f32 lowGravLon;     // offset 0x44
    f32 freezeLon;      // offset 0x48
    u8 holdCycle;       // offset 0x4C
    u8 trackSpeed;      // offset 0x4D
    u8 turboTap;        // offset 0x4E
    f32 throttle;       // offset 0x50
    f32 speedAdjust;    // offset 0x54
    f32 distAdjust;     // offset 0x58
    f32 turtleTime;     // offset 0x5C
    f32 turtleSpeed;    // offset 0x60
    f32 boostLon;       // offset 0x64
    f32 rjLon;          // offset 0x68
};

struct OrbitCarState;

ModuleTag lbl_8055E310;
f32 lbl_8055E318;  // time
f32 lbl_8055E31C;  // player speed factor
f32 lbl_8055E320;  // player total lon
f32 lbl_8055E324;  // race length
u8 lbl_8055E328;   // debug prints
bool lbl_8055E329;
u8 lbl_8055E32A;   // enabled
u8 lbl_8055E32B;   // turtle
DynArray<OrbitCarState>* lbl_8055E32C;
s32 lbl_8055E330;
void* lbl_8055E334;  // boost bounds

OrbitConfig lbl_8042FE38;
FixedArr<Role, 8> lbl_8042FEB0;

struct OrbitCarState {
    OrbitCarState(void);
    void Reset(void) {
        sf.from = sf.to = 1.0f;
        sf.t0 = sf.t1 = 0.0f;
        setTime = lbl_8055E318 + fn_80129DC4(3, 5.0f, 10.0f);
        laneTime = 0.0f;
        factor = 1.0f;
        dist = 0.0f;
        lane.from = lane.to = 0.0f;
        lane.t0 = lane.t1 = 0.0f;
        wander.start = lbl_8055E318 - fn_80129DC4(3, 0.0f, 2.0f * wander.half);
        role = 0;
        suicide = 0;
        infTurbo = 0;
        park = 0;
        close = 0;
        tree = 0;
        lowGrav = 0;
        frozen = 0;
        frozenDist = 0.0f;
        memset(x64, 0, 0x24);
    }
    void Set(int i, bool h) {
        id = i;
        human = h;
    }
    s32 id;          // offset 0x0
    bool human;      // offset 0x4
    u8 close;        // offset 0x5
    Ramp sf;         // offset 0x8
    f32 setTime;     // offset 0x1C
    f32 laneTime;    // offset 0x20
    f32 factor;      // offset 0x24
    f32 dist;        // offset 0x28
    u8 frozen;       // offset 0x2C
    f32 frozenDist;  // offset 0x30
    Ramp lane;       // offset 0x34
    Osc wander;      // offset 0x48
    Role* role;      // offset 0x58
    u8 suicide;      // offset 0x5C
    u8 park;         // offset 0x5D
    u8 infTurbo;     // offset 0x5E
    u8 tree;         // offset 0x5F
    u8 lowGrav;      // offset 0x60
    f32 x64[4];      // offset 0x64
    f32 boost;       // offset 0x74
    f32 x78[4];      // offset 0x78
};


#define NearEq(a, b) ((f32)__fabs((a) - (b)) <= lbl_80558DC4[0])

static inline f32 Unlerp(f32 a, f32 b, f32 x) {
    f32 t = (x - a) / (b - a);
    CLAMP01(t);
    return t;
}
static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

static inline f32 RampValue(Ramp* r) {
    f32 t1 = r->t1;
    f32 t0 = r->t0;
    if (NearEq(t0, t1)) {
        return r->from;
    } else {
        f32 u = (lbl_8055E318 - t0) / (t1 - t0);
        f32 to = r->to;
        f32 from = r->from;
        CLAMP01(u);
        return from + u * (to - from);
    }
}

static inline f32 Smooth(f32 t) {
    if (t < -1.0f) {
        return -1.0f;
    } else if (t > 1.0f) {
        return 1.0f;
    } else if (t > 0.0f) {
        return t * (-2.0f * t * t) + 3.0f * t * t;
    } else {
        return t * (-2.0f * t * t) - 3.0f * t * t;
    }
}

static inline f32 OscValue(Osc* o) {
    f32 t = fmod(lbl_8055E318 - o->start, o->Period());
    if (t > o->half) {
        t = 2.0f * o->half - t;
    }
    t /= o->half;
    return o->min + Smooth(t) * (o->max - o->min);
}

extern "C" {

f32 fn_801659D0(const void* race) {
    f32 len = fn_8013672C();
    return len * fn_800081F0(race);
}

void fn_80165A30(ValueNode* table) {
    int i;
    ValueNode* n;
    lbl_8042FEB0.count = 0;
    for (i = 0; (n = table->GetChildAt(i)) != 0; i++) {
        if (strcmp(n->GetName(), "role") == 0) {
            Role* r = &lbl_8042FEB0.data[lbl_8042FEB0.count++];
            f32 cycle = n->GetFloat("cycle_t");
            f32 max = n->GetFloat("max_lon");
            r->min = n->GetFloat("min_lon");
            r->max = max;
            r->half = cycle / 2.0f;
            r->owner = -1;
            r->start = lbl_8055E318 - fn_80129DC4(3, 0.0f, 2.0f * r->half);
        }
    }
}

void fn_80165B78(ValueNode* table) {
    const char* s;
    f32 v;
    ValueNode* sf;
    ValueNode* c = table->FindChild("config");
    lbl_8042FE38.switchLon = c->GetFloat("switchline_lon");
    lbl_8042FE38.crazyLon = c->GetFloat("crazyline_lon");
    lbl_8042FE38.crazyWid = c->GetFloat("crazyline_wid", 20.0f);
    lbl_8042FE38.parkLon = c->GetFloat("park_lon");
    lbl_8042FE38.infTurboLon = c->GetFloat("infinite_turbo_lon", -100.0f);
    lbl_8042FE38.suicideLon = c->GetFloat("suicide_lon", lbl_8042FE38.crazyLon);
    lbl_8042FE38.lowGravLon = c->GetFloat("low_gravity_lon", -1000.0f);
    lbl_8042FE38.treeLon = c->GetFloat("tree_destroy_lon", -1000.0f);
    lbl_8042FE38.freezeLon = c->GetFloat("freeze_orbit_lon");
    lbl_8042FE38.boostLon = c->GetFloat("speedboost_plon", -250.0f);
    lbl_8042FE38.rjLon = c->GetFloat("rj_plon", -250.0f);
    lbl_8042FE38.throttle = c->GetFloat("turbo_enabled_throttle", 0.99f);
    s = c->GetValue("freeze_orbit_lon");
    if (strchr(s, 'L')) {
        f32 len = fn_8013672C();
        f32 a = atof(s);
        v = a * len;
    } else {
        v = atof(s);
    }
    lbl_8042FE38.freezeLon = lbl_8055E324 - v;
    lbl_8042FE38.holdCycle = c->GetBool("hold_cycle_when_deviant", 1);
    lbl_8042FE38.trackSpeed = c->GetBool("track_player_speed", 1);
    lbl_8042FE38.turboTap = c->GetBool("turbo_tap_enabled", 0);
    sf = c->FindChild("speedfactor");
    lbl_8042FE38.minLon = sf->GetFloat("min_lon");
    lbl_8042FE38.minSf = sf->GetFloat("min_sf");
    lbl_8042FE38.maxLon = sf->GetFloat("max_lon");
    lbl_8042FE38.maxSf = sf->GetFloat("max_sf");
    lbl_8042FE38.setFreq = sf->GetFloat("set_freq");
    lbl_8042FE38.setSpeed = sf->GetFloat("set_speed");
    lbl_8042FE38.minCloseLon = sf->GetFloat("min_close_lon", -100.0f);
    lbl_8042FE38.maxCloseLon = sf->GetFloat("max_close_lon", 300.0f);
    lbl_8042FE38.minCloseSf = sf->GetFloat("min_close_sf", 0.85f);
    lbl_8042FE38.maxCloseSf = sf->GetFloat("max_close_sf", 1.25f);
    lbl_8042FE38.speedAdjust = fn_80204CFC("orbit_speed_adjust");
    lbl_8042FE38.distAdjust = fn_80204CFC("orbit_player_dist_adjust");
    lbl_8042FE38.turtleTime = c->GetFloat("turtle_enable_time", 45.0f);
    lbl_8042FE38.turtleSpeed = c->GetFloat("turtle_player_speed_mph", 200.0f);
    lbl_8042FE38.turtleSpeed *= 0.44444445f;
}

void fn_80165FAC(void) {
    ValueTable* t = fn_80131808("orbit.val");
    const char* name = fn_80204EA0("orbit_table");
    ValueNode* n;
    fn_802E30C8("orbit: loading from subtable \"%s\"", name);
    n = t->FindChild(name);
    fn_80165A30(n);
    fn_80165B78(n);
    fn_80131818(t);
}

Role* fn_80166040(s32 id) {
    Role* r = 0;
    int i;
    for (i = 0; i < lbl_8042FEB0.count; i++) {
        if (lbl_8042FEB0.data[i].owner == -1) {
            r = &lbl_8042FEB0.data[i];
            r->owner = id;
            break;
        }
    }
    return r;
}

OrbitCar* fn_80166098(void) { return fn_801B2EC0(fn_80179630(-1)); }

f32 fn_801660C0(void) {
    OrbitCar* p = fn_80166098();
    int lap = p->Progress()->laps - 1;
    f32 len = fn_8013672C();
    f32 v = lap * len + p->TrackPos()->lon;
    return MAX(0.0f, v);
}

int fn_8016616C(void) { return lbl_8055E320 > lbl_8042FE38.freezeLon; }

f32 fn_8016618C(OrbitCar* car) {
    OrbitCar* p = fn_80166098();
    f32 plon = p->TrackPos()->lon;
    f32 clon = car->TrackPos()->lon;
    f32 d;
    if (p->Progress()->lap < car->Progress()->lap) {
        if (plon < clon) {
            plon += fn_8013672C();
        }
        d = clon - plon;
    } else {
        if (clon < plon) {
            clon += fn_8013672C();
        }
        d = clon - plon;
    }
    return d + lbl_8042FE38.distAdjust;
}

f32 fn_80166274(void) {
    CarPhysics* ph = fn_801B2EC0(fn_80179630(-1))->Physics();
    CarRec* rec = fn_8017960C(fn_80179630(-1));
    f32 top = ph->engine->topSpeed;
    f32 speed = rec->speed;
    f32 m = 1.0f;
    m = MAX(m, top);
    f32 r = speed / m;
    return MAX(0.7f, r);
}

void fn_801662FC(void) {
    if ((lbl_8055E329 = (u8)lbl_8055E330++ == 0xFF) && lbl_8055E328) {
        if (!NearEq(lbl_8055E31C, 1.0f)) {
            fn_802E30C8("G: g_speed_factor: %.2f, p_total_lon: %.2f (hold at %.2f) (player %.3f)", lbl_8055E31C,
                        lbl_8055E320, lbl_8042FE38.freezeLon, fn_80166274());
        } else {
            fn_802E30C8("G: plon %.2f hold @%.2f, pspeed_f: %.3f", lbl_8055E320, lbl_8042FE38.freezeLon,
                        fn_80166274());
        }
    }
}

struct CarSetup {
    s32 type;  // offset 0x0
    u8 pad4[0x78 - 0x4];
};

void fn_801663C8(int n, const CarSetup* cars, const RaceDesc* race) {
    int i;
    lbl_8055E324 = fn_801659D0(race);
    fn_80165FAC();
    lbl_8042FE38.maxSf = GetEvent(fn_801769CC()->event)->vals->GetFloat("orbit_max_sf", lbl_8042FE38.maxSf);
    fn_802E30C8("orbit: %.2f->%.2f on %.2f->%.2f", lbl_8042FE38.minSf, lbl_8042FE38.maxSf, lbl_8042FE38.maxLon,
                lbl_8042FE38.minLon);
    lbl_8055E32C = new ("orbit-ci") DynArray<OrbitCarState>(n);
    lbl_8055E32C->a.count = 0;
    for (i = 0; i < n; i++) {
        lbl_8055E32C->Add()->Set(i, cars[i].type == 'P');
    }
    lbl_8055E32A = race->type != 0x180304;
    lbl_8055E32B = 0;
    lbl_8055E334 = 0;
}

}  // extern "C"

OrbitCarState::OrbitCarState(void) : sf(lbl_8042FE38.setSpeed), lane(0.4f) {
    f32 cycle = fn_80129DC4(3, 8.0f, 15.0f);
    f32 w = lbl_8042FE38.crazyWid;
    wander.max = w;
    wander.min = -w;
    wander.half = cycle / 2.0f;
}

extern "C" {

int fn_801667F0(void) { return lbl_8055E32A && !lbl_8055E32B; }

void fn_80166814(void) {
    if (lbl_8055E334) {
        fn_80139E18(lbl_8055E334);
        lbl_8055E334 = 0;
    }
    delete lbl_8055E32C;
    lbl_8055E32C = 0;
}

void fn_80166898(void) {
    int i;
    lbl_8055E32B = 0;
    lbl_8055E318 = 0.0f;
    lbl_8055E320 = 0.0f;
    lbl_8055E31C = 0.0f;
    for (i = 0; i < lbl_8055E32C->Count(); i++) {
        lbl_8055E32C->At(i)->Reset();
    }
    for (i = 0; i < lbl_8042FEB0.count; i++) {
        Role* r = &lbl_8042FEB0.data[i];
        r->owner = -1;
        r->start = lbl_8055E318 - fn_80129DC4(3, 0.0f, 2.0f * r->half);
    }
}

f32 fn_80166DD4(Ramp* r);
void fn_80166E54(Role* r);
void fn_80166E68(Ramp* r, f32 v);
void fn_80166F3C(f32* t, f32 lo, f32 hi);
int fn_80166F78(f32* t);
f32 fn_80166F90(Role* r);
void fn_8016707C(OrbitCarState* s, OrbitCar* car);

static inline void UpdateCar(OrbitCarState* s, OrbitCar* car) {
    fn_8016707C(s, car);
    if (s->role) {
        f32 d = s->dist = fn_8016618C(car);
        f32 target = fn_80166F90(s->role);
        f32 diff;
        int hold;
        int crazy;
        if (s->frozen) {
            target = s->frozenDist;
        }
        diff = d - target;
        if (fn_80166F78(&s->setTime)) {
            f32 f;
            fn_80166F3C(&s->setTime, Max(0.0f, lbl_8042FE38.setFreq - 0.5f),
                        lbl_8042FE38.setFreq + 0.5f);
            f = Remap(lbl_8042FE38.minLon, lbl_8042FE38.maxLon, lbl_8042FE38.maxSf, lbl_8042FE38.minSf,
                            diff);
            if (s->close = d > lbl_8042FE38.minCloseLon && d < lbl_8042FE38.maxCloseLon) {
                f = Clamp(f, lbl_8042FE38.minCloseSf, lbl_8042FE38.maxCloseSf);
            }
            fn_80166E68(&s->sf, lbl_8042FE38.speedAdjust + f * lbl_8055E31C);
        }
        hold = 0;
        if (lbl_8042FE38.holdCycle && (diff < lbl_8042FE38.minLon || diff > lbl_8042FE38.maxLon)) {
            hold = 1;
        }
        if (hold) {
            fn_80166E54(s->role);
        }
        crazy = diff > lbl_8042FE38.crazyLon;
        if (diff > lbl_8042FE38.switchLon && fn_80166F78(&s->laneTime)) {
            if (crazy) {
                fn_80156A40()->SetCrazy(s->id, 1);
            } else {
                fn_80156A40()->SetCrazy(s->id, 0);
            }
            fn_80166F3C(&s->laneTime, 7.0f, 15.0f);
        }
        if (crazy) {
            fn_80166E68(&s->lane, 1.0f);
        } else {
            fn_80166E68(&s->lane, 0.0f);
        }
        s->factor = fn_80166DD4(&s->sf);
        s->park = diff > lbl_8042FE38.parkLon;
        s->suicide = diff > lbl_8042FE38.suicideLon;
        s->infTurbo = diff < lbl_8042FE38.infTurboLon;
        s->tree = diff < lbl_8042FE38.treeLon;
        s->lowGrav = diff < lbl_8042FE38.lowGravLon;
    } else {
        s->suicide = 0;
        s->infTurbo = 0;
        s->park = 0;
        s->tree = 0;
        s->lowGrav = 0;
        s->factor = 1.0f;
        memset(s->x64, 0, 0x24);
    }
}

void fn_80166A24(void) {
    int i;
    if (fn_801667F0()) {
        int frozen = fn_8016616C();
        lbl_8055E318 = lbl_8055E3C8;
        lbl_8055E31C = 1.0f;
        if (lbl_8042FE38.trackSpeed) {
            lbl_8055E31C *= fn_80166274();
        }
        lbl_8055E320 = fn_801660C0();
        if (fn_8016616C() && !frozen) {
            if (lbl_8055E328) {
                fn_802E30C8("orbit: positions are frozen.");
            }
            for (i = 0; i < lbl_8055E32C->Count(); i++) {
                OrbitCarState* s = lbl_8055E32C->At(i);
                f32 d = fn_8016618C(fn_801B2EC0(s->id));
                s->frozen = 1;
                s->frozenDist = d;
            }
        }
        fn_801662FC();
        for (i = 0; i < lbl_8055E32C->Count(); i++) {
            OrbitCarState* s = lbl_8055E32C->At(i);
            UpdateCar(s, fn_801B2EC0(s->id));
        }
    }
}

f32 fn_80166DD4(Ramp* r) { return RampValue(r); }

void fn_80166E54(Role* r) { r->start += 0.016683351f; }

void fn_80166E68(Ramp* r, f32 v) {
    f32 target = r->to;
    if (!NearEq(v, target)) {
        f32 cur = RampValue(r);
        r->to = v;
        r->from = cur;
        r->t0 = lbl_8055E318;
        r->t1 = lbl_8055E318 + (f32)__fabs((f32)(double)cur - (f32)(double)v) / r->rate;  // FIXME: double operands?
    }
}

void fn_80166F3C(f32* t, f32 lo, f32 hi) { *t = lbl_8055E318 + fn_80129DC4(3, lo, hi); }

int fn_80166F78(f32* t) { return lbl_8055E318 > *t; }

f32 fn_80166F90(Role* r) { return OscValue(r); }

static inline void SetRole(OrbitCarState* s, Role* r) {
    if (s->role) {
        s->role->owner = -1;
    }
    s->role = r;
    s->setTime = lbl_8055E318 + fn_80129DC4(3, 2.0f, 5.0f);
    s->laneTime = lbl_8055E318 + fn_80129DC4(3, 8.0f, 15.0f);
}

void fn_8016707C(OrbitCarState* s, OrbitCar* car) {
    if (!s->human) {
        if (!fn_801B8570(fn_80179630(-1))) {
            SetRole(s, 0);
        } else if (!s->role && !car->Progress()->done) {
            SetRole(s, fn_80166040(s->id));
        }
    }
}

u8 fn_801671AC(int i) {
    if (fn_801667F0()) {
        return lbl_8055E32C->a.data[i].suicide;
    }
    return 0;
}


f32 fn_801671FC(int i) {
    if (fn_801667F0()) {
        OrbitCarState* s = lbl_8055E32C->At(i);
        f32 f = s->factor;
        f32 top = f > lbl_8042FE38.maxSf - 0.01f;
        f32 behind = s->dist < lbl_8042FE38.boostLon;
        f32 boost = 0.0f;
        if (top && behind) {
            if (lbl_8055E334) {
                CarRec* rec = fn_8017960C(i);
                if (lbl_8055E334 && fn_80139EC4(lbl_8055E334, rec->x, rec->z)) {
                    boost = 0.5f;
                }
            } else {
                f32 t = Unlerp(0.0f, 0.002f, fn_801B2EC0(i)->Physics()->engine->throttle);
                boost = Lerp(0.6f, 0.0f, t);
            }
            lbl_8055E32C->a.data[i].boost = boost;
            f += boost;
        } else {
            s->boost = 0.0f;
        }
        return f;
    }
    return 1.0f;
}

f32 fn_801673A4(int i, f32* wander) {
    if (fn_801667F0()) {
        OrbitCarState* s = lbl_8055E32C->At(i);
        *wander = OscValue(&s->wander);
        return RampValue(&s->lane);
    }
    *wander = 0.0f;
    return 0.0f;
}

u8 fn_80167544(int i) {
    if (fn_801667F0()) {
        return lbl_8055E32C->a.data[i].park;
    }
    return 0;
}

u8 fn_80167594(int i) {
    if (fn_801667F0()) {
        return lbl_8055E32C->a.data[i].infTurbo;
    }
    return 0;
}

u8 fn_801675E4(int i) {
    if (fn_801667F0()) {
        return lbl_8055E32C->a.data[i].tree;
    }
    return 0;
}

u8 fn_80167634(int i) {
    if (fn_801667F0()) {
        return lbl_8055E32C->a.data[i].lowGrav;
    }
    return 0;
}

void fn_80167684(f32 left) {
    if (!lbl_8055E32B && left < lbl_8042FE38.turtleTime) {
        f32 need = (lbl_8055E324 - fn_801660C0()) / lbl_8042FE38.turtleSpeed;
        if (need > left) {
            fn_802E30C8("orbit: TURTLE ENABLED: Player cannot possibly finish");
            fn_802E30C8("orbit: %.2f remains, Player won't finish before %.2f", left, need);
            lbl_8055E32B = 1;
        }
    }
}

int fn_80167728(int i) { return lbl_8055E32C->a.data[i].dist < lbl_8042FE38.rjLon; }

u8 fn_80167758(void) { return lbl_8042FE38.turboTap; }

int fn_80167768(f32 throttle) { return throttle > lbl_8042FE38.throttle; }

}  // extern "C"
