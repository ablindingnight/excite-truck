// Action sequences (0x801D4734-0x801D8FF8): scripted track events from actseq.val (and the event's own
// action_sequence_file). A sequence holds up to 64 events (obstacles, camera shakes, track effects,
// sounds, text, ring masters, world morphs, physics paths, surface changes, embedded effects,
// triggers, lightning), is started forwards or backwards by a trigger, runs until every event is
// done and then resets or stays finished; laps outside its active range park it. A rumble sound
// and an on-screen text line are shared. Names are guesses apart from the strings.
#include "game/common.h"
#include "game/deity.h"
#include "game/sound.h"
#include "game/arrayref.h"
#include "game/raceopts.h"
#include "game/gamedb.h"
#include <revolution/MTX/vec.h>
#include "game/plainvec.h"
#include "game/strcopy.h"
#include "game/mathutil.h"
#include "game/nearzero.h"

void* operator new(size_t size, const char* tag);

extern "C" {
int strcmp(const char* a, const char* b);
int strcasecmp(const char* a, const char* b);
char* strchr(const char* s, int c);
void fn_802E30C8(const char* fmt, ...);  // debug print
ValueTable* fn_80131810(const char* name);
void fn_80131818(ValueTable* table);
u8* fn_8017960C(int car);
int fn_801B2F24(void);  // racer count
}


extern "C" DeityCar* GetDeityCarByIndex(int car);

// Unit-local vector with constructors (port ODR: the SDK and other units define a different Vec).
struct ActVec {
    ActVec(void) {}
    ActVec(f32 x_, f32 y_, f32 z_) {
        x = x_;
        y = y_;
        z = z_;
    }
    void Set(f32 x_, f32 y_, f32 z_) {
        x = x_;
        y = y_;
        z = z_;
    }
    ActVec& operator=(const ValVec3& v) {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }
    f32 x;
    f32 y;
    f32 z;
};

extern "C" Sound3D* fn_802077A4(const char* name, int cls, ActVec* vel, ActVec* pos);

template <typename T, int N>
struct FixedArr : ArrayRef<T> {
    FixedArr(void) : ArrayRef<T>(storage, N) {}
    ~FixedArr(void) {}
    void Push(T v) {
        T* p = &this->data[this->count++];
        *p = v;
    }
    int Find(T v) {
        for (int i = 0; i < this->Count(); i++) {
            if (this->data[i] == v) {
                return i;
            }
        }
        return -1;
    }
    void Remove(T v) {
        int i = Find(v);
        if (i == -1) {
            return;
        }
        if (i >= 0 && i < this->count) {
            int last = this->count - 1;
            if (i != last) {
                this->data[i] = this->data[last];
            }
            this->count--;
        }
    }
    void RemoveAt(int i) {
        if (i >= 0 && i < this->count) {
            if (i != this->count - 1) {
                this->data[i] = this->data[this->count - 1];
            }
            this->count--;
        }
    }
    T storage[N];
};

// Copies at most n - 1 characters and always terminates.

class Body;
class SurfBody;

// Told when surface-changing bodies appear or disappear.
class ISurfaceListener {
public:
    virtual void Added(SurfBody* b) = 0;
    virtual void Removed(SurfBody* b) = 0;
    virtual void Changed(void) = 0;
};

// Told when pickup bodies appear, disappear or are hit.
class IPickupListener {
public:
    virtual void Added(Body* b) = 0;
    virtual void Removed(Body* b) = 0;
    virtual void Changed(void) = 0;
    virtual void Hit(Body* b, const ActVec* pos) = 0;
};

class ActionSequenceEvent {
public:
    virtual ~ActionSequenceEvent(void) {}
    virtual int Load(ValueNode* t) = 0;            // 0x0C
    virtual void Unload(void) {}                   // 0x10
    virtual const char* Name(void) = 0;            // 0x14
    virtual int State(void) = 0;                   // 0x18
    virtual int IsTrigger(void) { return 0; }      // 0x1C
    virtual int Handles(const int* ids, int n) { return 0; }  // 0x20
    virtual int Begin(int reverse) = 0;            // 0x24
    virtual void Reset(void) {}                    // 0x28
    virtual void Update(void) = 0;                 // 0x2C
    virtual void Rearm(void) = 0;                  // 0x30
    virtual void Stop(void) = 0;                   // 0x34
};

extern "C" ActionSequenceEvent* fn_801D7334(ValueNode* t);  // event factory

class ActionSequence {
public:
    ActionSequence(void);
    virtual ~ActionSequence(void);
    int Load(ValueNode* t);
    void Unload(void);
    void Reset(void);
    int Handles(const int* ids, int n);
    void Start(int forward);
    void StartForward(void) { Start(1); }
    void StartReverse(void) { Start(0); }
    void Update(void);
    void Stop(void);

    s32 state;            // offset 0x4: 0 unloaded, 1 ready, 2 running, 3 done, 4 off lap
    char name[0x14];      // offset 0x8
    f32 resetTime;        // offset 0x1C
    u8 autoReset;         // offset 0x20
    u8 triggerResets;     // offset 0x21
    u8 hasRumble;         // offset 0x22
    s32 firstLap;         // offset 0x24
    s32 lastLap;          // offset 0x28
    u8 forward;           // offset 0x2C
    FixedArr<ActionSequenceEvent*, 64> events;  // offset 0x30
};

// Unit-local (port ODR: other units define a different Rumble).
namespace {
struct Rumble {
    Sound3D* snd;  // offset 0x0
    f32 time;      // offset 0x4
    ActVec vel;       // offset 0x8
    ActVec pos;       // offset 0x14
};
}  // namespace

struct ASText {
    ASText(void) {
        start = -999.0f;
        duration = 0.0f;
    }
    f32 start;       // offset 0x0
    f32 duration;    // offset 0x4
    char text[0x40];   // offset 0x8
    char title[0x40];  // offset 0x48
};

extern f32 lbl_8055E3C8;  // race time

ModuleTag lbl_8055E948;
static UnkStatic123 lbl_804422D8;
UnkStatic7F lbl_8055E950;
Rumble lbl_804422F0;
ASText lbl_80442310;
FixedArr<ActionSequence*, 64> lbl_804423A4;
u8 lbl_8055E954;  // started

extern "C" {
void LoadActionSequences(const char* file);
void UpdateAllObjects(void);
void UpdateEngineSound(void);
void StopAllActiveObjects(void);
void ClearActionSequences(void);
void ResetAllObjects(void);
ActionSequence* FindActionSequenceByName(const char* name);
void fn_801D597C(ActionSequence* s, int forward);
int fn_801D5A68(ActionSequence* s);
void UpdateCameraTargetPosition(ActionSequence* s, const ActVec* pos);

void InitializeRumbleSound(void) {
    lbl_8055E954 = 1;
    lbl_804422F0.snd = fn_802077A4("rumble.sfx", 5, &lbl_804422F0.vel, &lbl_804422F0.pos);
    if (lbl_804422F0.snd) {
        lbl_804422F0.snd->radius = 100.0f;
    }
    LoadActionSequences("actseq.val");
    ValueNode* t = ((EventInfo*)GetEvent(fn_801769CC()->event))->node;
    if (t->Has("action_sequence_file")) {
        LoadActionSequences(t->GetValue("action_sequence_file"));
    }
}

void fn_801D47F4(void) { ResetAllObjects(); }

void fn_801D47F8(void) {
    UpdateAllObjects();
    UpdateEngineSound();
}

void fn_801D481C(void) {
    lbl_804422F0.pos.x = 0.0f;
    lbl_804422F0.pos.y = 0.0f;
    lbl_804422F0.pos.z = 0.0f;
    lbl_804422F0.time = 0.0f;
    StopAllActiveObjects();
}

void ResetSoundSystem(void) {
    lbl_8055E954 = 0;
    delete lbl_804422F0.snd;
    ClearActionSequences();
}

ActionSequence* LoadActionSequence(const char* name) { return FindActionSequenceByName(name); }

void fn_801D488C(ActionSequence* s, int a, int rumble, const ActVec* pos) {
    fn_801D597C(s, a);
    if (rumble && fn_801D5A68(s)) {
        UpdateCameraTargetPosition(s, pos);
    }
}

}  // extern "C"

extern "C" {
void DrawFormattedString(u32 color, int flags, const char* fmt, f32 scale, f32 x, f32 y, ...);
f32 GetActionSequenceResetTime(ActionSequence* s);
u8 fn_801D592C(ActionSequence* s);
int fn_801D5A14(ActionSequence* s);
int fn_801D5A1C(ActionSequence* s);
int fn_801D5A24(ActionSequence* s);
int FindActionSequenceHandle(ActionSequence* s, const int* ids, int n);

f32 fn_801D48F4(ActionSequence* s) { return GetActionSequenceResetTime(s); }

u8 fn_801D48F8(ActionSequence* s) { return fn_801D592C(s); }

int fn_801D48FC(ActionSequence* s) { return fn_801D5A14(s); }

int fn_801D4900(ActionSequence* s) { return fn_801D5A1C(s); }

int fn_801D4904(ActionSequence* s) { return fn_801D5A24(s); }

int fn_801D4908(ActionSequence* s, const int* ids, int n) { return FindActionSequenceHandle(s, ids, n); }

}  // extern "C"


/* Helper name unproven */
inline void DrawTitle(u32 color, const char* s) { DrawFormattedString(color, 12, "%s", 1.2f, 0.0f, 130.0f, s); }
/* Helper name unproven */
inline void DrawLine(u32 color, f32 y, const char* s) { DrawFormattedString(color, 12, "%s", 1.0f, 0.0f, y, s); }

extern "C" void fn_801D490C(void) {
    f32 dur = lbl_80442310.duration;
    f32 start = lbl_80442310.start;
    f32 now = lbl_8055E3C8;
    if (now < start + dur) {
        f32 a = 1.0f;
        if (dur > 0.5f) {
            f32 in = now - start;
            if (in < 0.25f) {
                a = Lerp(0.0f, 1.0f, MaxF(MinFRev(in / ((start + 0.25f) - start), 1.0f), 0.0f));
            } else {
                f32 out = (start + dur) - 0.25f;
                if (now > out) {
                    a = Lerp(1.0f, 0.0f, MaxF(MinFRev((now - out) / ((start + dur) - out), 1.0f), 0.0f));
                }
            }
        }
        char* line;
        u32 color = (((int)(255.0f * a) << 24) | 0xFFFFFF) & ~0xFF;
        DrawTitle(color, lbl_80442310.title);
        char buf[0x40];
        SafeStrCopy(buf, lbl_80442310.text, 0x40);
        f32 y = 100.0f;
        line = buf;
        while (line && *line) {
            char* nl = strchr(line, '^');
            if (nl) {
                *nl = 0;
            }
            DrawLine(color, y, line);
            line = nl ? nl + 1 : NULL;
            y -= 15.0f;
        }
    }
}

ActionSequence::ActionSequence(void) : state(0) {}

ActionSequence::~ActionSequence(void) {
    if (state) {
        int i = events.count;
        state = 0;
        while (i-- > 0) {
            ActionSequenceEvent* e = events.data[i];
            events.RemoveAt(i);
            delete e;
        }
    }
    SafeStrCopy(name, "UNLOADED_SEQUENCE", 0x14);
}

int ActionSequence::Load(ValueNode* t) {
    if (!t->Has("name")) {
        fn_802E30C8("ActionSequence: required value \"name\" is not present in the table");
        return 0;
    }
    SafeStrCopy(name, t->GetValue("name"), 0x14);
    ValueNode* c;
    for (int i = 0; (c = t->GetChildAt(i)) != 0; i++) {
        if (strcasecmp(c->GetName(), "event") == 0) {
            ActionSequenceEvent* e = fn_801D7334(c);
            if (e) {
                events.Push(e);
            }
        } else {
            fn_802E30C8("ActionSequence::Load: sequence \"%s\" has unknown sub-table \"%s\"", name, c->GetName());
        }
    }
    autoReset = t->GetBool("auto_reset", 0);
    resetTime = t->GetFloat("reset_time", 10.0f);
    triggerResets = t->GetBool("trigger_resets", 1);
    hasRumble = t->GetBool("has_rumble", 1);
    firstLap = t->GetInt("first_active_lap", -1);
    lastLap = t->GetInt("last_active_lap", -1);
    state = 1;
    return 1;
}

void ActionSequence::Unload(void) {
    for (int i = 0; i < events.count; i++) {
        events.data[i]->Unload();
    }
}

void ActionSequence::Reset(void) {
    for (int i = 0; i < events.count; i++) {
        events.data[i]->Reset();
    }
}

int ActionSequence::Handles(const int* ids, int n) {
    for (int i = 0; i < events.count; i++) {
        if (events.data[i]->Handles(ids, n)) {
            return 1;
        }
    }
    return 0;
}

extern "C" void fn_801D50A8(ActionSequence* s) { s->StartForward(); }

extern "C" void fn_801D50B0(ActionSequence* s) { s->StartReverse(); }

void ActionSequence::Update(void) {
    if (state == 2) {
        int busy = 0;
        for (int i = 0; i < events.count; i++) {
            ActionSequenceEvent* e = events.data[i];
            if (e->State() == 2) {
                busy = 1;
                e->Update();
                e->State();
            }
        }
        if (!busy) {
            if (autoReset) {
                for (int i = 0; i < events.count; i++) {
                    events.data[i]->Rearm();
                }
                state = 1;
            } else {
                state = 3;
            }
        }
    } else {
        int lap = -1;
        for (int i = 0; i < fn_801B2F24(); i++) {
            if (*(s32*)(fn_8017960C(i) + 0x1F0) == 0x50) {
                int l = GetDeityCarByIndex(i)->State()->lap;
                if (l > lap) {
                    lap = l;
                }
            }
        }
        if (lap >= 0) {
            if (state == 1) {
                if ((firstLap >= 0 && lap < firstLap) || (lastLap >= 0 && lap > lastLap)) {
                    state = 4;
                }
            } else if (state == 4) {
                if ((firstLap < 0 || lap >= firstLap) && (lastLap < 0 || lap <= lastLap)) {
                    state = 1;
                }
            }
        }
    }
}

void ActionSequence::Stop(void) {
    if (state) {
        for (int i = 0; i < events.count; i++) {
            events.data[i]->Stop();
        }
        state = 1;
    }
}

void ActionSequence::Start(int fwd) {
    if (state == 1) {
        forward = fwd;
        int reverse = forward == 0;
        int started = 0;
        for (int i = 0; i < events.count; i++) {
            ActionSequenceEvent* e = events.data[i];
            if (!e->IsTrigger() && e->Begin(reverse)) {
                started = 1;
            }
        }
        if (started) {
            for (int i = 0; i < events.count; i++) {
                ActionSequenceEvent* e = events.data[i];
                if (e->IsTrigger()) {
                    e->Begin(reverse);
                }
            }
        }
        state = 2;
    }
}

extern "C" {

void UpdateCameraTargetPosition(ActionSequence* s, const ActVec* pos) {
    lbl_804422F0.pos = *pos;
    lbl_804422F0.time += 3.0f;
}

void UpdateEngineSound(void) {
    Sound3D* s = lbl_804422F0.snd;
    if (!s) {
        return;
    }
    if (lbl_804422F0.time > 0.0f) {
        lbl_804422F0.time -= 0.016683351f;
        lbl_804422F0.snd->SetVolume(MinFRev(lbl_804422F0.time, 1.0f));
        lbl_804422F0.snd->PlayLoop();
    } else {
        s->Stop();
    }
}

void LoadActionSequences(const char* file) {
    ValueTable* t = fn_80131810(file);
    if (!t) {
        fn_802E30C8("ActionSequence: can't load value file %s", file);
        return;
    }
    ValueNode* n;
    for (int i = 0; (n = t->GetChild(i)) != 0; i++) {
        if (strcasecmp(n->GetName(), "action_sequence") == 0) {
            ActionSequence* s = new ("ActionSequence") ActionSequence;
            if (s->Load(n)) {
                lbl_804423A4.Push(s);
            } else {
                delete s;
            }
        }
    }
    fn_80131818(t);
}

void UpdateAllObjects(void) {
    for (int i = 0; i < lbl_804423A4.count; i++) {
        lbl_804423A4.data[i]->Update();
    }
}

void StopAllActiveObjects(void) {
    for (int i = 0; i < lbl_804423A4.count; i++) {
        lbl_804423A4.data[i]->Stop();
    }
}

void ClearActionSequences(void) {
    int i = lbl_804423A4.count;
    while (i-- > 0) {
        ActionSequence* s = lbl_804423A4.data[i];
        s->Unload();
        lbl_804423A4.RemoveAt(i);
        delete s;
    }
}

void ResetAllObjects(void) {
    for (int i = 0; i < lbl_804423A4.count; i++) {
        lbl_804423A4.data[i]->Reset();
    }
}

ActionSequence* FindActionSequenceByName(const char* name) {
    ActionSequence** p = lbl_804423A4.data;
    int n = lbl_804423A4.count;
    for (int i = 0; i < n; p++, i++) {
        ActionSequence* s = *p;
        if (strcmp(s->name, name) == 0) {
            return s;
        }
    }
    return NULL;
}


f32 GetActionSequenceResetTime(ActionSequence* s) {
    if (!s) {
        return 10.0f;
    }
    for (int i = 0; i < lbl_804423A4.count; i++) {
        if (lbl_804423A4.data[i] == s) {
            return s->resetTime;
        }
    }
    return 10.0f;
}

u8 fn_801D592C(ActionSequence* s) {
    if (!s) {
        return 0;
    }
    for (int i = 0; i < lbl_804423A4.count; i++) {
        if (lbl_804423A4.data[i] == s) {
            return s->triggerResets;
        }
    }
    return 0;
}

void fn_801D597C(ActionSequence* s, int forward) {
    if (!s) {
        return;
    }
    for (int i = 0; i < lbl_804423A4.count; i++) {
        if (lbl_804423A4.data[i] == s) {
            if (forward) {
                fn_801D50A8(s);
            } else {
                fn_801D50B0(s);
            }
            return;
        }
    }
}

int CheckActionSequenceState(ActionSequence* s, int state) {
    for (int i = 0; i < lbl_804423A4.count; i++) {
        if (lbl_804423A4.data[i] == s) {
            return state == s->state;
        }
    }
    return 0;
}

int fn_801D5A14(ActionSequence* s) { return CheckActionSequenceState(s, 2); }

int fn_801D5A1C(ActionSequence* s) { return CheckActionSequenceState(s, 1); }

int fn_801D5A24(ActionSequence* s) { return CheckActionSequenceState(s, 4); }

int FindActionSequenceHandle(ActionSequence* s, const int* ids, int n) {
    for (int i = 0; i < lbl_804423A4.count; i++) {
        if (lbl_804423A4.data[i] == s) {
            return s->Handles(ids, n);
        }
    }
    return 0;
}

int fn_801D5A68(ActionSequence* s) {
    for (int i = 0; i < lbl_804423A4.count; i++) {
        if (lbl_804423A4.data[i] == s) {
            return s->hasRumble;
        }
    }
    return 0;
}

}  // extern "C"

extern "C" {
f32 fn_80129DC4(int seed, f32 lo, f32 hi);  // random float
void fn_801796CC(u8 type, const void* data, u32 n);
int fn_801345D8(char* buf, int size, const char* fmt, ...);  // snprintf
}

extern s32 lbl_8055E3C4;  // frame counter
extern f32 __float_epsilon[];  // FLT_EPSILON


// A thrown or knocked object on the track.
#include "game/body.h"
#include "game/clamp01.h"

extern "C" {
void AddRaceListener(IPickupListener* l);
void fn_801D0824(IPickupListener* l);
}

// A collision message: where, how fast, the surface and the effect to play.
struct HitMsg {
    ActVec pos;     // offset 0x0
    ActVec vel;     // offset 0xC
    s16 surf;    // offset 0x18
    s32 effect;  // offset 0x1C
};

struct ObstacleEffect {
    s32 id;     // offset 0x0
    f32 minV2;  // offset 0x4: squared speed needed
};

namespace {

class Obstacle_ASE : public ActionSequenceEvent, public IPickupListener {
public:
    Obstacle_ASE(void);
    virtual ~Obstacle_ASE(void);
    virtual int Load(ValueNode* t);
    virtual const char* Name(void) { return "Obstacle_ASE"; }
    virtual int State(void) { return state; }
    virtual void Stop(void) {
        if (state) {
            state = 1;
        }
    }
    virtual void Rearm(void) { Stop(); }
    virtual void Added(Body* b);
    virtual void Removed(Body* b);
    virtual void Changed(void) {}
    virtual int Begin(int reverse);
    virtual void Update(void);
    virtual void Hit(Body* b, const ActVec* pos);

    int Matches(Body* b) {
        ValueNode* d = b->Desc();
        if (groupId >= 0 && d) {
            int g = d->GetInt("group_id", -1);
            if (g == groupId) {
                return 1;
            }
        }
        return 0;
    }

    s32 state;                             // offset 0x8
    s32 groupId;                           // offset 0xC
    FixedArr<ObstacleEffect, 4> effects;   // offset 0x10
    ActVec dir1;                              // offset 0x3C
    ActVec dir2;                              // offset 0x48
    ActVec throwPt;                           // offset 0x54
    f32 speed1;                            // offset 0x60
    f32 speed2;                            // offset 0x64
    s32 resetTimeout;                      // offset 0x68
    s32 startFrame;                        // offset 0x6C
    FixedArr<Body*, 64> bodies;            // offset 0x70
};

inline void Obstacle_ASE::Removed(Body* b) { bodies.Remove(b); }

inline void Obstacle_ASE::Added(Body* b) {
    if (Matches(b)) {
        bodies.Push(b);
    }
}

}  // namespace

extern "C" {
void fn_80128658(void* p);  // MemFree
void* ParseBoundPoints(ValueNode* t);  // load a bound volume
int fn_80139B5C(u8* car, void* bound);  // car inside bound
void fn_80267E40(u8* car, f32 amount);  // shake the camera
void fn_801E9DC4(s16* h);
}

extern "C" {
int fn_8016A15C(const char* name);  // switch a ring set on
void fn_8016A240(const char* name);  // switch a ring set off
}

extern "C" Sound3D* fn_80207848(const char* name, int flags);  // 2D sound

extern "C" {
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
void* fn_801D9A54(const char* name);     // find morph group
void fn_801D9AB8(void* group, f32 t);    // set morph group position
int fn_801D9B28(void* group, int index);  // morph group contains index
void fn_80178DBC(int index, f32 t);      // set world morph position
}

static inline f32 Ramp(f32 x, f32 lo, f32 hi) { return Clamp01((x - lo) / (hi - lo)); }

namespace {

// Moves a world morph (by index, or a named morph group) from position_1 to position_2 over "length".
class WorldMorph_ASE : public ActionSequenceEvent {
public:
    WorldMorph_ASE(void) { state = 0; }
    virtual ~WorldMorph_ASE(void) { state = 0; }
    virtual int Handles(const int* ids, int n) {
        for (int i = 0; i < n; i++) {
            if (group) {
                if (fn_801D9B28(group, ids[i])) {
                    return 1;
                }
            } else if (index == ids[i]) {
                return 1;
            }
        }
        return 0;
    }
    virtual int Load(ValueNode* t) {
        group = 0;
        index = -1;
        if (t->Has("mgroup_name")) {
            const char* n = t->GetValue("mgroup_name");
            group = fn_801D9A54(n);
            if (!group) {
                fn_802E30C8("WorldMorphActionSequenceEvent: MorphGroup \"%s\" does not exist!", n);
                return 0;
            }
        } else {
            if (!t->Has("morph_index")) {
                fn_802E30C8("WorldMorphActionSequenceEvent: value morph_index was not found in the source table!");
                return 0;
            }
            index = t->GetIntValue("morph_index");
            if (index < 0) {
                fn_802E30C8("WorldMorphActionSequenceEvent: morph index should be non-negative, is %d", index);
                return 0;
            }
            if (index >= 75) {
                fn_802E30C8("WorldMorphActionSequenceEvent: morph index %d exceeds the maximum of %d", index, 75);
                return 0;
            }
        }
        f32 len = t->GetFloat("length", -1.0f);
        if (len < 0.0f) {
            return 0;
        }
        length = (int)(59.94f * len);
        delay = (int)(59.94f * t->GetFloat("delay", 0.0f));
        pos1 = t->GetFloat("start_t", t->GetFloat("position_1", 0.0f));
        pos2 = t->GetFloat("end_t", t->GetFloat("position_2", 1.0f));
        state = 1;
        Stop();
        return 1;
    }
    virtual const char* Name(void) { return "WorldMorph_ASE"; }
    virtual int State(void) { return state; }
    virtual int Begin(int dir) {
        if (state == 1) {
            startFrame = lbl_8055E3C4;
            switch (dir) {
            case 0:
                from = pos1;
                to = pos2;
                break;
            case 1:
                from = pos2;
                to = pos1;
                break;
            default:
                fn_802E317C("bad ActionSequenceEvent::Direction '%d' in Begin!", dir);
                break;
            }
            state = 2;
            return 1;
        }
        return 0;
    }
    /* Helper name unproven */
    void SetInterpolatedMorph(int t, f32 a, f32 b) {
        Set(Lerp(a, b, Ramp(t, 0.0f, length)));
    }
    virtual void Update(void) {
        if (state == 2) {
            int t = lbl_8055E3C4 - startFrame - delay;
            if (t >= 0) {
                SetInterpolatedMorph(t, from, to);
            }
            if (t >= length) {
                state = 3;
            }
        }
    }
    virtual void Rearm(void) {
        if (state) {
            state = 1;
        }
    }
    virtual void Stop(void) {
        if (state) {
            if (delay == 0) {
                Set(pos1);
            }
            state = 1;
        }
    }

    void Set(f32 t) {
        if (group) {
            fn_801D9AB8(group, t);
        } else {
            fn_80178DBC(index, t);
        }
    }

    void* group;     // offset 0x4
    s32 index;       // offset 0x8
    s32 length;      // offset 0xC
    s32 delay;       // offset 0x10
    f32 pos1;        // offset 0x14
    f32 pos2;        // offset 0x18
    s32 startFrame;  // offset 0x1C
    f32 from;        // offset 0x20
    f32 to;          // offset 0x24
    s32 state;       // offset 0x28
};

}  // namespace

#include "game/pathmover.h"

#include "game/gamemode.h"
#include "game/round.h"

extern "C" {
PathMover* fn_801DA8C8(const char* object, const char* path);
int fn_80179630(int i);  // car of a local player
int fn_80179650(void);   // local player count
GameMode* fn_801B83A0(void);
}

namespace {

// Runs an object along a path; cars inside the kill bound at the start are respawned.
class PhysPath_ASE : public ActionSequenceEvent {
public:
    PhysPath_ASE(void) { state = 0; }
    virtual ~PhysPath_ASE(void) {
        state = 0;
        if (mover) {
            delete mover;
        }
        if (killBound) {
            fn_80128658(killBound);
        }
    }
    virtual int Load(ValueNode* t) {
        if (!t->Has("path")) {
            fn_802E30C8("PhysPathActionSequenceEvent: no \"path\" was found in the table!");
            return 0;
        }
        SafeStrCopy(path, t->GetValue("path"), 0x14);
        SafeStrCopy(object, t->GetString("object", "*"), 0x14);
        ValueNode* kb = t->FindChild("kill_bound");
        killBound = !kb ? 0 : ParseBoundPoints(kb);
        delay = (int)(59.94f * t->GetFloat("delay", 0.0f));
        dir = 0;
        mover = 0;
        state = 1;
        fn_802E30C8("PhysPathActionSequenceEvent: initialized with object \"%s\", path \"%s\"", object, path);
        return 1;
    }
    virtual const char* Name(void) { return "PhysPath_ASE"; }
    virtual int State(void) { return state; }
    virtual int Begin(int d) {
        if (state == 1) {
            startFrame = lbl_8055E3C4;
            dir = d;
            state = 2;
            if (killBound) {
                int cp;
                int car;
                int i;
                int inside;
                for (i = 0; i < fn_80179650(); i++) {
                    car = fn_80179630(i);
                    inside = fn_80139B5C(fn_8017960C(car), killBound);
                    if (inside) {
                        cp = GetDeityCarByIndex(car)->State()->place;
                        fn_801B83A0()->vfA8(car, cp);  // respawn the car at checkpoint cp
                    }
                    fn_802E30C8("Car %d was %s the kill bound.", car, inside ? "inside" : "outside");
                }
            }
            return 1;
        }
        return 0;
    }
    virtual void Reset(void) {
        if (state) {
            mover = fn_801DA8C8(object, path);
        }
    }
    virtual void Update(void) {
        int elapsed = lbl_8055E3C4 - (startFrame + delay);
        if (state == 2 && elapsed >= 0) {
            if (mover) {
                mover->Start(dir == 0 ? 1.0f : -1.0f);
            }
            state = 3;
        }
    }
    virtual void Rearm(void) {
        if (state) {
            state = 1;
        }
    }
    virtual void Stop(void) {
        if (state) {
            if (mover) {
                mover->Reset();
            }
            state = 1;
        }
    }

    char object[0x14];  // offset 0x4
    char path[0x14];    // offset 0x18
    PathMover* mover;   // offset 0x2C
    s32 delay;          // offset 0x30
    s32 startFrame;     // offset 0x34
    s32 dir;            // offset 0x38
    void* killBound;    // offset 0x3C
    s32 state;          // offset 0x40
};

}  // namespace

namespace {

// Shakes every car's camera (inside the bound, if any) for "length" seconds after "delay".
class CameraShake_ASE : public ActionSequenceEvent {
public:
    CameraShake_ASE(void);
    virtual ~CameraShake_ASE(void);
    virtual int Load(ValueNode* t);
    virtual const char* Name(void);
    virtual int State(void);
    virtual int IsTrigger(void) { return !runSolo; }
    virtual int Begin(int reverse);
    virtual void Update(void);
    virtual void Rearm(void) { Stop(); }
    virtual void Stop(void);

    s32 state;       // offset 0x4
    s32 delay;       // offset 0x8
    s32 length;      // offset 0xC
    f32 amount;      // offset 0x10
    void* bound;     // offset 0x14
    u8 runSolo;      // offset 0x18
    s32 startFrame;  // offset 0x1C
};

}  // namespace

namespace {

// Sends a track effect message (position, velocity, effect id) once "delay" has passed.
class TrackEffect_ASE : public ActionSequenceEvent {
public:
    TrackEffect_ASE(void);
    virtual ~TrackEffect_ASE(void);
    virtual int Load(ValueNode* t);
    virtual const char* Name(void);
    virtual int State(void);
    virtual int IsTrigger(void) { return !runSolo; }
    virtual int Begin(int reverse);
    virtual void Update(void);
    virtual void Rearm(void) { Stop(); }
    virtual void Stop(void);

    s32 state;       // offset 0x4
    s32 length;      // offset 0x8
    s32 delay;       // offset 0xC
    s32 startFrame;  // offset 0x10
    ActVec loc;         // offset 0x14
    ActVec v;           // offset 0x20
    s32 effectId;    // offset 0x2C
    u8 runSolo;      // offset 0x30
};

}  // namespace

extern "C" int fn_8029F3BC(const char* name);  // embedded effect id

namespace {

// Fires an embedded track effect once "delay" has passed.
class EmbeddedTrackEffect_ASE : public ActionSequenceEvent {
public:
    EmbeddedTrackEffect_ASE(void) { state = 0; }
    virtual ~EmbeddedTrackEffect_ASE(void) { state = 0; }
    virtual int Load(ValueNode* t) {
        f32 len = t->GetFloat("length", 0.0f);
        f32 dly = t->GetFloat("delay", 0.0f);
        length = (int)(59.94f * len);
        delay = (int)(59.94f * dly);
        effect = fn_8029F3BC(t->GetValue("effect"));
        if (effect < 0) {
            return 0;
        }
        state = 1;
        return 1;
    }
    virtual const char* Name(void) { return "EmbeddedTrackEffect_ASE"; }
    virtual int State(void) { return state; }
    virtual int Begin(int reverse) {
        if (state == 1) {
            startFrame = lbl_8055E3C4;
            state = 2;
            return 1;
        }
        return 0;
    }
    virtual void Update(void) {
        int elapsed = lbl_8055E3C4 - (startFrame + delay);
        if (state == 2 && elapsed > 0) {
            s32 m = effect;
            fn_801796CC(11, &m, 4);
            if (elapsed >= length) {
                state = 3;
            }
        }
    }
    virtual void Rearm(void) { Stop(); }
    virtual void Stop(void) {
        if (state) {
            state = 1;
        }
    }

    s32 state;       // offset 0x4
    s32 length;      // offset 0x8
    s32 delay;       // offset 0xC
    s32 startFrame;  // offset 0x10
    s32 effect;      // offset 0x14
};

}  // namespace

namespace {

// Plays a sound (2D, or 3D at "loc" moving at "v") once "delay" has passed, looped for "length".
class SoundEffect_ASE : public ActionSequenceEvent {
public:
    SoundEffect_ASE(void);
    virtual ~SoundEffect_ASE(void);
    virtual int Load(ValueNode* t);
    virtual void Unload(void);
    virtual const char* Name(void);
    virtual int State(void);
    virtual int IsTrigger(void) { return !runSolo; }
    virtual int Begin(int reverse);
    virtual void Update(void);
    virtual void Rearm(void) {
        if (state) {
            state = 1;
        }
    }
    virtual void Stop(void);

    s32 state;        // offset 0x4
    s32 length;       // offset 0x8
    s32 delay;        // offset 0xC
    s32 startFrame;   // offset 0x10
    u8 runSolo;       // offset 0x14
    u8 looped;        // offset 0x15
    u8 forceMaxVol;   // offset 0x16
    ActVec loc;          // offset 0x18
    ActVec v;            // offset 0x24
    Sound3D* snd;     // offset 0x30
};

}  // namespace

namespace {

// Shows a title and a text line for "length" seconds.
class Text_ASE : public ActionSequenceEvent {
public:
    Text_ASE(void);
    virtual ~Text_ASE(void);
    virtual int Load(ValueNode* t);
    virtual const char* Name(void);
    virtual int State(void);
    virtual int IsTrigger(void) { return 0; }
    virtual int Begin(int reverse);
    virtual void Update(void) {}
    virtual void Rearm(void) { Stop(); }
    virtual void Stop(void);

    s32 state;          // offset 0x4
    f32 length;         // offset 0x8
    char title[0x40];   // offset 0xC
    char text[0x40];    // offset 0x4C
};

}  // namespace

// A body whose surface can be changed (a driveable track object).
class SurfBody {
public:
    virtual ValueNode* Desc(void);    // 0x08
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void vf18(void);
    virtual void SetSurface(int s);   // 0x1C
    virtual int Surface(void);        // 0x20
    virtual int Mode(void);           // 0x24
    virtual void SetMode(int m);      // 0x28
};

extern "C" {
int fn_801D04D0(const char* name);  // surface type by name
void fn_801D0590(ISurfaceListener* l);
void RemoveCarListener(ISurfaceListener* l);
void fn_801E1F78(void);  // lightning flash
}

struct SurfEntry {
    SurfBody* obj;  // offset 0x0
    s32 surf;       // offset 0x4
    s32 mode;       // offset 0x8
};

// PROVISIONAL: actual insertion context and saved-state address input ownership/API are inferred.
static inline void AppendSurfaceEntry(FixedArr<SurfEntry, 32>& entries, SurfBody* body, const SurfEntry* const& state) {
    SurfEntry* p = &entries.data[entries.count++];
    p->obj = body;
    p->surf = state->surf;
    p->mode = state->mode;
}

namespace {

// Changes the surface of every body in a group after "delay" seconds, and back after "length".
class SurfaceChange_ASE : public ActionSequenceEvent, public ISurfaceListener {
public:
    virtual int Load(ValueNode* t) {
        delay = t->GetFloat("delay", 0.0f);
        length = t->GetFloat("length");
        groupId = t->GetIntValue("group_id");
        surftype = fn_801D04D0(t->GetValue("surftype"));
        fn_801D0590(this);
        state = 1;
        start = 0.0f;
        return 1;
    }
    virtual void Unload(void) { RemoveCarListener(this); }
    virtual const char* Name(void) { return "SurfaceChange_ASE"; }
    virtual int State(void) { return state; }
    virtual int IsTrigger(void) { return 0; }
    virtual int Begin(int reverse) {
        state = 2;
        phase = 0;
        start = lbl_8055E3C8;
        return 0;
    }
    virtual void Update(void) {
        if (state == 2) {
            f32 now = lbl_8055E3C8;
            if (!phase) {
                if (now - start > delay) {
                    SurfEntry* e;
                    int off;
                    int i = 0;
                    for (off = 0; i < entries.count; off += sizeof(SurfEntry), ++i) {
                        e = (SurfEntry*)((u8*)entries.data + off);
                        e->obj->SetSurface(surftype);
                        e->obj->SetMode(e->mode);
                    }
                    phase = 1;
                }
            } else if (now - start - delay > length) {
                int i;
                int off;
                for (i = 0, off = 0; i < entries.count; off += sizeof(SurfEntry), i++) {
                    SurfEntry* e = (SurfEntry*)((u8*)entries.data + off);
                    e->obj->SetSurface(e->surf);
                    e->obj->SetMode(-1);
                }
                state = 1;
            }
        }
    }
    virtual void Rearm(void) { Stop(); }
    virtual void Stop(void) {
        state = 1;
        for (int i = 0; i < entries.count; i++) {
            entries.data[i].obj->SetMode(-1);
        }
    }
    virtual void Added(SurfBody* b);
    virtual void Removed(SurfBody* b);
    virtual void Changed(void) {}

    s32 groupId;                      // offset 0x8
    f32 delay;                        // offset 0xC
    f32 start;                        // offset 0x10
    f32 length;                       // offset 0x14
    s32 state;                        // offset 0x18
    s32 surftype;                     // offset 0x1C
    s32 phase;                        // offset 0x20
    FixedArr<SurfEntry, 32> entries;  // offset 0x24
};

inline void SurfaceChange_ASE::Removed(SurfBody* b) {
    for (int i = 0; i < entries.Count(); i++) {
        if (entries.data[i].obj == b) {
            entries.RemoveAt(i);
        }
    }
}
// PROVISIONAL: descriptor-result and saved-state input ownership are inferred.
inline void SurfaceChange_ASE::Added(SurfBody* b) {
    ValueNode* d = b->Desc();
    if (!d) { return; }
    s32 group = d->GetInt("group_id", -1);
    if (group == groupId) {
        // Only surf/mode are saved-state inputs; insertion supplies the body.
        SurfEntry e;
        e.surf = b->Surface();
        e.mode = b->Mode();
        AppendSurfaceEntry(entries, b, &e);
        b->SetMode(-1);
    }
}

}  // namespace

namespace {

// Switches a named ring set on after "delay" seconds, and off again after "length" (if not negative).
class RingMaster_ASE : public ActionSequenceEvent {
public:
    RingMaster_ASE(void);
    virtual ~RingMaster_ASE(void);
    virtual int Load(ValueNode* t);
    virtual void Unload(void);
    virtual const char* Name(void);
    virtual int State(void);
    virtual int IsTrigger(void) { return length < 0.0f; }
    virtual int Begin(int reverse);
    virtual void Update(void);
    virtual void Rearm(void) { Stop(); }
    virtual void Stop(void);

    void Activate(void);

    s32 state;        // offset 0x4
    char name[0x20];  // offset 0x8
    f32 delay;        // offset 0x28
    f32 start;        // offset 0x2C
    f32 length;       // offset 0x30
    u8 active;        // offset 0x34
    Sound3D* snd;     // offset 0x38
};

}  // namespace

namespace {

// An event that waits "delay" seconds and then runs for "length" seconds, with hooks for each stage.
class TimedEvent_ASE : public ActionSequenceEvent {
public:
    TimedEvent_ASE(void) { state = 0; }
    virtual int Load(ValueNode* t) {
        state = 1;
        start = 0.0f;
        delay = t->GetFloat("delay", 0.0f);
        length = t->GetFloat("length", 5.0f);
        return 1;
    }
    virtual int State(void) { return state; }
    virtual int Begin(int reverse) {
        if (state == 1) {
            state = 2;
            start = lbl_8055E3C8;
            OnStart();
            return 1;
        }
        return 0;
    }
    virtual void Update(void) {
        if (state == 2) {
            f32 d = delay;
            f32 t = lbl_8055E3C8 - start;
            if (t < d) {
                OnWait();
            } else if (t < d + length) {
                OnUpdate(t - d);
            } else {
                OnEnd();
                state = 3;
            }
        }
    }
    virtual void Rearm(void) { Stop(); }
    virtual void Stop(void) {
        if (state) {
            state = 1;
        }
    }
    virtual void OnStart(void) {}
    virtual void OnWait(void) {}
    virtual void OnUpdate(f32 t) {}
    virtual void OnEnd(void) {}

    s32 state;   // offset 0x4
    f32 delay;   // offset 0x8
    f32 length;  // offset 0xC
    f32 start;   // offset 0x10
};

}  // namespace


namespace {

// Flashes lightning every "interval" seconds while running.
class Lightning_ASE : public TimedEvent_ASE {
public:
    virtual int Load(ValueNode* t) {
        interval = t->GetFloat("interval", 999.0f);
        return TimedEvent_ASE::Load(t);
    }
    virtual void OnStart(void) { last = -1; }
    virtual void OnWait(void) {}
    virtual void OnUpdate(f32 t) {
        int n = Round(t / interval);
        if (n != last) {
            fn_801E1F78();
            last = n;
        }
    }
    virtual void OnEnd(void) {}
    virtual const char* Name(void) { return "LightningASE"; }

    f32 interval;  // offset 0x14
    s32 last;      // offset 0x18
};

}  // namespace

namespace {

// Starts another action sequence (by name) after "delay" seconds.
class Trigger_ASE : public ActionSequenceEvent {
public:
    Trigger_ASE(void) { state = 0; }
    virtual ~Trigger_ASE(void) { state = 0; }
    virtual int Load(ValueNode* t) {
        SafeStrCopy(seq, t->GetString("actseq", ""), 0x14);
        if (!seq[0]) {
            fn_802E30C8("TriggerActionSequenceEvent: no actseq value found");
            return 0;
        }
        delay = t->GetFloat("delay", 0.0f);
        start = 0.0f;
        state = 1;
        return 1;
    }
    virtual const char* Name(void) { return "Trigger_ASE"; }
    virtual int State(void) { return state; }
    virtual int Begin(int reverse) {
        if (state == 1) {
            state = 2;
            start = lbl_8055E3C8;
            return 1;
        }
        return 0;
    }
    virtual void Update(void) {
        if (state == 2 && lbl_8055E3C8 - start >= delay) {
            ActionSequence* s = LoadActionSequence(seq);
            if (!s) {
                fn_802E30C8("TriggerActionSequenceEvent: no action sequence found with name %s", seq);
            } else {
                ActVec zero(0.0f, 0.0f, 0.0f);
                fn_801D488C(s, 1, 0, &zero);
            }
            state = 3;
        }
    }
    virtual void Rearm(void) { Stop(); }
    virtual void Stop(void) {
        if (state) {
            state = 1;
        }
    }

    s32 state;        // offset 0x4
    char seq[0x14];   // offset 0x8
    f32 delay;        // offset 0x1C
    f32 start;        // offset 0x20
};

}  // namespace

Obstacle_ASE::Obstacle_ASE(void) : state(0) {}

Obstacle_ASE::~Obstacle_ASE(void) {
    if (state) {
        state = 0;
        fn_801D0824(this);
        groupId = -1;
    }
}

int Obstacle_ASE::Load(ValueNode* t) {
    int ok = 0;
    ObstacleEffect* e;
    int i;
    groupId = t->GetInt("group_id", -1);
    if (groupId >= 0) {
        for (i = 0; i < 4; i++) {
            char key[0x14];
            fn_801345D8(key, 0x14, "effect_id_%d", i);
            if (t->Has(key)) {
                e = &effects.data[effects.count++];
                e->id = t->GetIntValue(key);
                fn_801345D8(key, 0x14, "effect_min_v_%d", i);
                f32 v = t->GetFloat(key, 0.0f);
                e->minV2 = v * v;
            }
        }
        speed1 = t->GetFloat("speed1", 0.0f);
        speed2 = t->GetFloat("speed2", speed1);
        dir1 = (t)->GetVec3Or("dir1", ValV3(0.0f, 0.0f, 0.0f));
        // The by-value default is a copy of dir1 seen as the constructor vector class (a plain ValVec3 lvalue
        // schedules the copy differently; ActVec is not a ValVec3 subclass yet).
        dir2 = (t)->GetVec3Or("dir2", *(ValV3*)&dir1);
        throwPt = (t)->GetVec3Or("throw_pt", ValV3(0.0f, 0.0f, 0.0f));
        resetTimeout = (int)(59.94f * t->GetFloat("reset_timeout", -1.0f));
        AddRaceListener(this);
        state = 1;
        ok = 1;
    } else {
        fn_802E30C8("ObstacleActionSequenceEvent: required value \"group_id\" was negative or not present");
    }
    return ok;
}


int Obstacle_ASE::Begin(int reverse) {
    if (state == 1) {
        for (int i = 0; i < bodies.count; i++) {
            f32 ts = fn_80129DC4(4, 0.0f, 1.0f);
            f32 td = fn_80129DC4(4, 0.0f, 1.0f);
            ActVec dir;
            dir.Set(Lerp(dir1.x, dir2.x, td), Lerp(dir1.y, dir2.y, td), Lerp(dir1.z, dir2.z, td));
            f32 speed = Lerp(speed1, speed2, ts);
            ActVec vel(0.0f, 0.0f, 0.0f);
            if (FAbs(PSVECDotProduct((const Vec*)&dir, (const Vec*)&dir)) <= __float_epsilon[0] ||
                FAbs(speed) <= __float_epsilon[0]) {
                bodies.data[i]->SetActive(1);
            } else {
                PSVECNormalize((const Vec*)&dir, (Vec*)&dir);
                vel.x = dir.x * speed;
                vel.y = dir.y * speed;
                vel.z = dir.z * speed;
                bodies.data[i]->Throw((const f32*)&vel, (const f32*)&throwPt);
            }
        }
        if (resetTimeout > 0) {
            state = 2;
            startFrame = lbl_8055E3C4;
        } else {
            state = 3;
        }
        return 1;
    }
    return 0;
}

void Obstacle_ASE::Update(void) {
    if (state == 2 && lbl_8055E3C4 - startFrame >= resetTimeout) {
        for (int i = 0; i < bodies.count; i++) {
            bodies.data[i]->Respawn();
        }
        state = 3;
    }
}

void Obstacle_ASE::Hit(Body* b, const ActVec* pos) {
    if (effects.count > 0 && Matches(b)) {
        HitMsg m;
        m.pos = *pos;
        m.vel = *(ActVec*)b->Vel();
        m.surf = b->Surface();
        f32 v2 = PSVECDotProduct((const Vec*)&m.vel, (const Vec*)&m.vel);
        for (int i = 0; i < effects.count; i++) {
            ObstacleEffect* e = &effects.data[i];
            if (v2 < e->minV2) {
                return;
            }
            m.effect = e->id;
            fn_801796CC(10, &m, 0x20);
        }
    }
}



CameraShake_ASE::CameraShake_ASE(void) : state(0) { bound = 0; }

CameraShake_ASE::~CameraShake_ASE(void) {
    state = 0;
    if (bound) {
        fn_80128658(bound);
    }
}

int CameraShake_ASE::Load(ValueNode* t) {
    if (!t->Has("length")) {
        fn_802E30C8("CameraShakeActionSequenceEvent: required value \"length\" is missing");
        return 0;
    }
    f32 len = t->GetFloat("length");
    if (len < 0.0f) {
        fn_802E30C8("CameraShakeActionSequenceEvent: value \"length\" should be non-negative, is %f", len);
        return 0;
    }
    length = (int)(59.94f * len);
    delay = (int)(59.94f * t->GetFloat("delay", 0.0f));
    if (!t->Has("amount")) {
        fn_802E30C8("CameraShakeActionSequenceEvent: required value \"amount\" is missing");
        return 0;
    }
    amount = t->GetFloat("amount", 0.5f);
    if (amount < 0.0f) {
        fn_802E30C8("CameraShakeActionSequenceEvent: value \"amount\" should be non-negative, is %f", amount);
        return 0;
    }
    bound = 0;
    ValueNode* b = t->FindChild("bound");
    if (b) {
        bound = ParseBoundPoints(b);
    }
    runSolo = t->GetBool("run_solo", 0);
    state = 1;
    return 1;
}

const char* CameraShake_ASE::Name(void) { return "CameraShake_ASE"; }

int CameraShake_ASE::State(void) { return state; }

int CameraShake_ASE::Begin(int reverse) {
    if (state == 1) {
        startFrame = lbl_8055E3C4;
        state = 2;
        return 1;
    }
    return 0;
}

void CameraShake_ASE::Update(void) {
    if (state == 2) {
        if (lbl_8055E3C4 - startFrame >= delay) {
            u8* car;
            for (int i = 0; (car = fn_8017960C(i)) != 0; i++) {
                if (!bound || fn_80139B5C(car, bound)) {
                    fn_80267E40(car, amount);
                }
            }
        }
        if (lbl_8055E3C4 - startFrame >= delay + length) {
            state = 3;
        }
    }
}

void CameraShake_ASE::Stop(void) {
    if (state) {
        state = 1;
    }
}


TrackEffect_ASE::TrackEffect_ASE(void) : state(0) {}

TrackEffect_ASE::~TrackEffect_ASE(void) { state = 0; }

int TrackEffect_ASE::Load(ValueNode* t) {
    f32 len = t->GetFloat("length", 0.0f);
    f32 dly = t->GetFloat("delay", 0.0f);
    length = (int)(59.94f * len);
    delay = (int)(59.94f * dly);
    loc = t->GetVec3("loc");
    v = t->GetVec3("v");
    effectId = t->GetIntValue("effect_id");
    runSolo = t->GetBool("run_solo", 0);
    state = 1;
    return 1;
}

const char* TrackEffect_ASE::Name(void) { return "TrackEffect_ASE"; }

int TrackEffect_ASE::State(void) { return state; }

int TrackEffect_ASE::Begin(int reverse) {
    if (state == 1) {
        startFrame = lbl_8055E3C4;
        state = 2;
        return 1;
    }
    return 0;
}

void TrackEffect_ASE::Update(void) {
    int elapsed = lbl_8055E3C4 - (startFrame + delay);
    if (state == 2 && elapsed > 0) {
        HitMsg m;
        m.pos = loc;
        m.vel = v;
        fn_801E9DC4(&m.surf);
        m.effect = effectId;
        fn_801796CC(10, &m, 0x20);
        if (elapsed >= length) {
            state = 3;
        }
    }
}

void TrackEffect_ASE::Stop(void) {
    if (state) {
        state = 1;
    }
}



SoundEffect_ASE::SoundEffect_ASE(void) : state(0) {}

SoundEffect_ASE::~SoundEffect_ASE(void) { state = 0; }

int SoundEffect_ASE::Load(ValueNode* t) {
    f32 len = t->GetFloat("length", 0.0f);
    f32 dly = t->GetFloat("delay", 0.0f);
    length = (int)(59.94f * len);
    delay = (int)(59.94f * dly);
    loc = t->GetVec3("loc");
    runSolo = t->GetBool("run_solo", 0);
    looped = t->GetBool("looped", 1);
    forceMaxVol = t->GetBool("force_max_vol", 1);
    const char* name = t->GetString("sfx_name", 0);
    if (!name) {
        fn_802E30C8("SoundEffect_ASE: missing required field \"sfx_name\"!");
        return 0;
    }
    v = (t)->GetVec3Or("v", ValV3(0.0f, 0.0f, 0.0f));
    if (t->GetBool("is_3d", 0)) {
        Sound3D* s = fn_802077A4(name, 5, &v, &loc);
        snd = s;
        if (s) {
            s->radius = t->GetFloat("near_distance", 50.0f);
        }
    } else {
        snd = fn_80207848(name, 1);
    }
    if (!snd) {
        fn_802E30C8("SoundEffect_ASE: couldn't find sfx file %s", name);
        return 0;
    }
    state = 1;
    return 1;
}

void SoundEffect_ASE::Unload(void) {
    if (snd) {
        delete snd;
    }
    snd = 0;
}

const char* SoundEffect_ASE::Name(void) { return "SoundEffect_ASE"; }

int SoundEffect_ASE::State(void) { return state; }

int SoundEffect_ASE::Begin(int reverse) {
    if (state == 1) {
        startFrame = lbl_8055E3C4;
        state = 2;
        return 1;
    }
    return 0;
}

void SoundEffect_ASE::Update(void) {
    int elapsed = lbl_8055E3C4 - (startFrame + delay);
    if (state == 2 && elapsed > 0) {
        if (forceMaxVol) {
            snd->SetVolume(1.0f);
        }
        if (looped) {
            snd->PlayLoop();
            if (elapsed >= length) {
                state = 3;
                snd->Stop();
            }
        } else {
            snd->Play();
            state = 3;
        }
    }
}

void SoundEffect_ASE::Stop(void) {
    if (snd && snd->playing) {
        snd->Stop();
    }
    if (state) {
        state = 1;
    }
}


Text_ASE::Text_ASE(void) : state(0) {}

Text_ASE::~Text_ASE(void) { state = 0; }

int Text_ASE::Load(ValueNode* t) {
    length = t->GetFloat("length", 5.0f);
    SafeStrCopy(title, t->GetString("title", ""), 0x40);
    SafeStrCopy(text, t->GetString("text", ""), 0x40);
    state = 1;
    return 1;
}

const char* Text_ASE::Name(void) { return "Text_ASE"; }

int Text_ASE::State(void) { return state; }

int Text_ASE::Begin(int reverse) {
    if (state == 1) {
        lbl_80442310.duration = length;
        const f32 start = lbl_8055E3C8;
        lbl_80442310.start = start;
        SafeStrCopy(lbl_80442310.title, title, 0x40);
        SafeStrCopy(lbl_80442310.text, text, 0x40);
        state = 3;
        return 1;
    }
    return 0;
}

void Text_ASE::Stop(void) {
    if (state) {
        state = 1;
    }
}



void RingMaster_ASE::Activate(void) {
    if (!active) {
        if (fn_8016A15C(name)) {
            snd->Play();
        }
        active = 1;
    }
}

RingMaster_ASE::RingMaster_ASE(void) : state(0) {}

RingMaster_ASE::~RingMaster_ASE(void) { state = 0; }

int RingMaster_ASE::Load(ValueNode* t) {
    if (!t->Has("name")) {
        fn_802E30C8("RingMasterActionSequenceEvent: required value \"name\" is missing");
        return 0;
    }
    SafeStrCopy(name, t->GetValue("name"), 0x1E);
    delay = t->GetFloat("delay", 0.0f);
    length = t->GetFloat("length", -1.0f);
    state = 1;
    start = 0.0f;
    active = 0;
    snd = fn_80207848("ringon.sfx", 1);
    return 1;
}

void RingMaster_ASE::Unload(void) {
    if (snd) {
        delete snd;
    }
    snd = 0;
}

const char* RingMaster_ASE::Name(void) { return "RingMaster_ASE"; }

int RingMaster_ASE::State(void) { return state; }

int RingMaster_ASE::Begin(int reverse) {
    if (state == 1) {
        state = 2;
        start = lbl_8055E3C8;
        return 1;
    }
    return 0;
}

void RingMaster_ASE::Update(void) {
    if (state == 2) {
        if (length < 0.0f) {
            Activate();
            state = 3;
        } else {
            f32 now = lbl_8055E3C8;
            f32 on = start + delay;
            if (length + on < now) {
                fn_8016A240(name);
                state = 3;
            } else if (on < now) {
                Activate();
            }
        }
    }
}

void RingMaster_ASE::Stop(void) {
    fn_8016A240(name);
    state = 1;
    if (snd && snd->playing) {
        snd->Stop();
    }
    active = 0;
}

extern "C" ActionSequenceEvent* fn_801D7334(ValueNode* t) {
    const char* type = t->GetValue("type");
    ActionSequenceEvent* e = 0;
    if (strcasecmp(type, "obstacle") == 0) {
        e = new ("Obstacle_ASE") Obstacle_ASE;
    } else if (strcasecmp(type, "world_morph") == 0) {
        e = new ("WorldMorph_ASE") WorldMorph_ASE;
    } else if (strcasecmp(type, "phys_path") == 0) {
        e = new ("PhysPath_ASE") PhysPath_ASE;
    } else if (strcasecmp(type, "camera_shake") == 0) {
        e = new ("CameraShake_ASE") CameraShake_ASE;
    } else if (strcasecmp(type, "track_effect") == 0) {
        e = new ("TrackEffect_ASE") TrackEffect_ASE;
    } else if (strcasecmp(type, "sound_effect") == 0) {
        e = new ("SoundEffect_ASE") SoundEffect_ASE;
    } else if (strcasecmp(type, "text") == 0) {
        e = new ("Text_ASE") Text_ASE;
    } else if (strcasecmp(type, "surfchange") == 0) {
        e = new ("SurfChange_ASE") SurfaceChange_ASE;
    } else if (strcasecmp(type, "embedded_effect") == 0) {
        e = new ("EmbeddedEffect_ASE") EmbeddedTrackEffect_ASE;
    } else if (strcasecmp(type, "ring_set") == 0) {
        e = new ("RingMaster_ASE") RingMaster_ASE;
    } else if (strcasecmp(type, "trigger") == 0) {
        e = new ("Trigger_ASE") Trigger_ASE;
    } else if (strcasecmp(type, "lightning") == 0) {
        e = new ("Lightning_ASE") Lightning_ASE;
    }
    if (e) {
        if (e->Load(t)) {
            return e;
        }
        delete e;
    }
    return 0;
}

// Debug console command (stripped from the game): restarts every sequence, or starts one by name.
extern "C" void ActionSequenceCommand(const char* name) {
    if (!name) {
        fn_802E30C8("actseqs resetting");
        ResetAllObjects();
        return;
    }
    ActionSequence* s = FindActionSequenceByName(name);
    if (!s) {
        fn_802E30C8("actseq is unknown; known sequences listed below");
        for (int i = 0; i < lbl_804423A4.count; i++) {
            fn_802E30C8(lbl_804423A4.data[i]->name);  // no "%s": retail .sdata has no third "%s" (this function is stripped)
        }
    } else {
        fn_802E30C8("starting actseq %s", name);
        fn_801D597C(s, 1);
    }
}
