// Career path classes and the career runner (0x8027D304-0x8027E908): the concrete Path / Bracket / Contest
// classes behind the abstract event-path interfaces (their constructors, destructors and accessors; the
// loading code is in eventpath.cpp), the per-slot career points save block, the vehicle list offered for an
// event, and the loop that plays career contests and reports what a run unlocked. Names are guesses apart
// from the strings.
#include "game/common.h"

extern "C" {
void* memset(void* dst, int c, size_t n);
int strcasecmp(const char* a, const char* b);
void fn_802E30C8(const char* fmt, ...);
void fn_802E6BD8(u32 tag, int version, void* data, int size, void (*reset)(int));
void fn_802840F4(void);
void fn_802840F8(void);
int fn_802E4A10(void);
void fn_80137C84(int a, int b);
void fn_80284580(s32 a, s32 b, void* list, s32* x);
void fn_8025A204(void);
void fn_802E7128(void);
}

class Grades {
public:
    void Reset(void);
    int Find(int score);
    int GetThreshold(int grade);
    s32 thresholds[5];  // offset 0x0
    s32 passing;        // offset 0x14
};

struct EventInfo {
    s32 id;      // offset 0x0
    s32 track;   // offset 0x4
    u32 type;    // offset 0x8
    u8 padC[0x20 - 0xC];
    ValueNode* node;  // offset 0x20
};

struct TrackInfo {
    u8 pad0[0x8];
    char name[0x68];   // offset 0x8
    s32 events;        // offset 0x70
    u8 pad74[0x80 - 0x74];
    ValueNode* node;   // offset 0x80
};

struct TrackEvent {
    s32 event;  // offset 0x0
};

struct Unlockable {
    const char* type;  // offset 0x0
    const char* name;  // offset 0x4
    s32 points;        // offset 0x8
};

struct VehicleInfo {
    u8 pad0[0x4];
    s32 a4;          // offset 0x4
    s32 a8;          // offset 0x8
    u8 padC[0x2C - 0xC];
    char name[0x74];  // offset 0x2C
    u32 flags;        // offset 0xA0
};

struct DriverInfo {
    u8 pad0[0x4];
    s32 id;       // offset 0x4
    u8 pad8[0x4C - 0x8];
    s32 vehicle;  // offset 0x4C
    s32 a50;      // offset 0x50
};

class VehicleList {
public:
    virtual ~VehicleList(void);                                    // 0x8
    virtual void Add(const char* name, int unlocked, int special);  // 0xC
    virtual void SetReason(int i, const char* reason);             // 0x10
    virtual int Count(void);                                       // 0x14
};

struct AchQuery {
    s32 a0;
    s32 a4;
    s32 a8;
};

EventInfo* GetEvent(int i);
TrackInfo* GetTrack(int i);
TrackEvent* FindTrackEvent(int track, int i);
const char* GetTrackFriendlyName(const char* name);
const char* GetEventTypeName(u32 type);
const char* GetEventTypeDisplayName(u32 type);
int IdentGetCurrentSlot(void);
const char* IdentGetSlotName(int slot);
void IdentRegisterCallback(void (*cb)(int, int));
int IsUnlockAll(void);
int GetMaxExcitePoints(void);
Unlockable* GetUnlockable(int i);
int GetUnlockableCount(void);
int CountVehiclesNamed(const char* name);
VehicleInfo* FindVehicleNamed(const char* name, int a);
VehicleInfo* FindVehicle(long id);
int IsCarUnlocked(const char* name);
int IsSpecialChassisUnlocked(const char* name);
int IsExtremeChassisUnlocked(void);
int IsCareerComplete(void);
int AllExtremeCarsUnlocked2(void);
const char* GetAllExtremesChassis(void);
const char* GetCareerCompletedChassis(void);
const char* GetSpecialChassis(const char* name);
const char* ShowUnlockReason(const char* name);
const char* ShowExtremeUnlockReason(void);
int GetEveMode(void);
void UnloadEves(void);
void LoadModeEves(int mode);
void SetNumPlayers(int n);
DriverInfo* GetDriver(int i);
VehicleInfo* GetVehicle(int i);
void SetPlayer(int i, long driver, unsigned long vehicle, long x);
void SetPlayerName(const char* name, int i);
void ClearPlayer(int i);
void AchSetScoreLimiter(unsigned char on);
void AchievementsQuery(AchQuery* q);
void AchievementsCommitSession(void);

class IContest {
public:
    virtual ~IContest(void) {}
    virtual const char* TrackName(void) = 0;      // 0xC
    virtual const char* Name(void) = 0;           // 0x10
    virtual const char* TypeName(void) = 0;       // 0x14
    virtual const char* TypeDisplayName(void) = 0;  // 0x18
    virtual const char* FriendlyName(void) = 0;   // 0x1C
    virtual int EventId(void) = 0;                // 0x20
    virtual int Difficulty(void) = 0;             // 0x24
    virtual int Best(void) = 0;                   // 0x28
    virtual int BestGrade(void) = 0;              // 0x2C
    virtual int GradeOf(int score) = 0;           // 0x30
    virtual int Threshold(int grade) = 0;         // 0x34
    virtual int Passing(void) = 0;                // 0x38
    virtual int PassScore(void) = 0;              // 0x3C
    virtual int Unlocked(void) = 0;               // 0x40
    virtual int Open(void) = 0;                   // 0x44
    virtual int Done(void) = 0;                   // 0x48
    virtual int MaxGrade(void) = 0;               // 0x4C
};

class IBracket {
public:
    virtual ~IBracket(void) {}
    virtual const char* Name(void) = 0;          // 0xC
    virtual const char* Id(void) = 0;            // 0x10
    virtual int Difficulty(void) = 0;            // 0x14
    virtual int Count(void) = 0;                 // 0x18
    virtual IContest* Get(int i) = 0;            // 0x1C
    virtual int Unlocked(void) = 0;              // 0x20
    virtual int Open(void) = 0;                  // 0x24
    virtual int AllDone(void) = 0;               // 0x28
    virtual int Gated(void) = 0;                 // 0x2C
    virtual const char* Text(void) = 0;          // 0x30
};

class IPath {
public:
    virtual ~IPath(void) {}
    virtual int Count(void) = 0;            // 0xC
    virtual IBracket* Get(int i) = 0;       // 0x10
    virtual int IsCareer(void) = 0;         // 0x14
};

template <class T> struct FixedVec {
    FixedVec(T* d, int c) {
        data = d;
        cap = c;
        n = 0;
    }
    ~FixedVec(void) {
        data = 0;
        n = 0;
        cap = 0;
    }
    T* data;  // offset 0x0
    s32 n;    // offset 0x4
    s32 cap;  // offset 0x8
};

template <class T, int N> struct InlineVec : FixedVec<T> {
    InlineVec(void) : FixedVec<T>(buf, N) {}
    T buf[N];
};

class Contest : public IContest {
public:
    Contest(void) {
        valid = 0;
        grades.Reset();
    }
    ~Contest(void);
    const char* TrackName(void);
    const char* Name(void);
    const char* TypeName(void);
    const char* TypeDisplayName(void);
    const char* FriendlyName(void);
    int EventId(void);
    int Difficulty(void);
    int Best(void);
    int BestGrade(void);
    int GradeOf(int score);
    int Threshold(int grade);
    int Passing(void);
    int PassScore(void);
    int Unlocked(void);
    int Open(void);
    int Done(void);
    int MaxGrade(void);
    u8 valid;          // offset 0x4
    ValueNode* node;   // offset 0x8
    s32 event;         // offset 0xC
    u8 unlocked;       // offset 0x10
    u8 open;           // offset 0x11
    u8 done;           // offset 0x12
    Grades grades;     // offset 0x14
    s32 bestGrade;     // offset 0x2C
    s32 difficulty;    // offset 0x30
    s32 best;          // offset 0x34
    u8 hasBest;        // offset 0x38
};

class Bracket : public IBracket {
public:
    Bracket(void) { valid = 0; }
    ~Bracket(void);
    const char* Name(void);
    const char* Id(void);
    int Difficulty(void);
    int Count(void);
    IContest* Get(int i);
    int Unlocked(void);
    int Open(void);
    int AllDone(void);
    int Gated(void);
    const char* Text(void);
    u8 valid;                    // offset 0x4
    ValueNode* node;             // offset 0x8
    const char* id;              // offset 0xC
    const char* name;            // offset 0x10
    s32 difficulty;              // offset 0x14
    InlineVec<Contest, 6> contests;  // offset 0x18
    char text[100];              // offset 0x18C
    u8 unlocked;                 // offset 0x1F0
    u8 open;                     // offset 0x1F1
    u8 allDone;                  // offset 0x1F2
    u8 gated;                    // offset 0x1F3
};

class Path : public IPath {
public:
    Path(void) : table(0) {}
    ~Path(void);
    int Count(void);
    IBracket* Get(int i);
    int IsCareer(void);
    ValueTable* table;           // offset 0x4
    InlineVec<Bracket, 6> brackets;  // offset 0x8
    u8 career;                   // offset 0xBCC
};

struct SlotPaths {
    SlotPaths(void);
    Path career[3];  // offset 0x0
    Path chal[2];    // offset 0x2370
};

struct CareerSave {
    s32 points;   // offset 0x0
    s32 percent;  // offset 0x4
};

struct CareerResult {
    u8 improved;    // offset 0x0
    s32 delta;      // offset 0x4
    s32 event;      // offset 0x8
    u8 newly[7];    // offset 0xC
};

struct CareerRun {
    s32 track;    // offset 0x0
    s32 index;    // offset 0x4
    s32 which;    // offset 0x8
    void* out;    // offset 0xC
};

extern "C" {
void fn_8027AF94(Contest* c);
void fn_8027B288(Bracket* b);
void fn_8027B60C(Path* p);
void fn_8027B690(void);
void fn_8027B77C(void);
Path* fn_8027B834(int i);
int fn_8027B94C(int i);
int fn_8027BAB8(IPath* p);
void fn_8027BCC0(int difficulty, s32* total, s32* max);
void fn_8027BCC8(int difficulty, s32* total, s32* max);
void fn_8027BF38(void);
VehicleList* fn_8025007C(int n);
int fn_8024DAE0(s32* vehicle, s32* x, int a, VehicleList* list, int b);
void fn_8023BF4C(CareerRun* r);
int fn_802840FC(s32* sel, IPath* path, CareerResult* res);
}

Path::~Path(void) { fn_8027B60C(this); }

Bracket::~Bracket(void) { fn_8027B288(this); }

Contest::~Contest(void) { fn_8027AF94(this); }

SlotPaths::SlotPaths(void) {}

const char* Contest::TrackName(void) {
    EventInfo* e = GetEvent(event);
    if (e->node->Has("track")) {
        return GetTrackFriendlyName(e->node->GetValue("track"));
    }
    return GetTrackFriendlyName(GetTrack(e->track)->name);
}

const char* Contest::Name(void) { return GetEvent(event)->node->GetValue("name"); }

const char* Contest::TypeName(void) { return GetEventTypeName(GetEvent(event)->type); }

const char* Contest::TypeDisplayName(void) { return GetEventTypeDisplayName(GetEvent(event)->type); }

const char* Contest::FriendlyName(void) { return GetEvent(event)->node->GetValue("friendly_name"); }

int Contest::Best(void) { return best; }

int Contest::BestGrade(void) {
    if (hasBest) {
        return GradeOf(best);
    }
    return -1;
}

int Contest::GradeOf(int score) { return grades.Find(score); }

int Contest::Threshold(int grade) { return grades.GetThreshold(grade); }

int Contest::EventId(void) { return event; }

int Contest::Difficulty(void) { return difficulty; }

int Contest::Passing(void) { return grades.passing; }

int Contest::PassScore(void) { return grades.GetThreshold(Passing()); }

int Contest::MaxGrade(void) { return bestGrade; }

const char* Bracket::Id(void) { return id; }

const char* Bracket::Name(void) { return name; }

int Bracket::Difficulty(void) { return difficulty; }

int Bracket::Count(void) { return contests.n; }

IContest* Bracket::Get(int i) { return &contests.data[i]; }

int Bracket::Unlocked(void) { return unlocked; }

int Bracket::Open(void) { return open; }

int Bracket::AllDone(void) { return allDone; }

int Bracket::Gated(void) { return gated; }

const char* Bracket::Text(void) { return text; }

int Path::Count(void) { return brackets.n; }

IBracket* Path::Get(int i) { return &brackets.data[i]; }

int Path::IsCareer(void) { return career; }

struct PathInfo {
    PathInfo(void) {
        a0 = 0;
        a4 = 0x7B;
        a8 = 0;
        aC = 0;
        a10 = 0;
    }
    s32 a0, a4, a8, aC, a10;
};

struct PathLimit {
    PathLimit(void) { v = 0x7F000000; }
    s32 v;
};

static inline int PMax(int a, int b) { return a > b ? a : b; }
static inline int PMin(int a, int b) { return a < b ? a : b; }

ModuleTag lbl_8055F660;
PathInfo lbl_804B2FC0;
static CareerSave lbl_804B2FD8[4];
PathLimit lbl_8055F668;
u8 lbl_8055F66C;  // running
s32 lbl_8055C790 = -1;

extern "C" {

void fn_8027D96C(int slot) {
    switch (slot) {
    case 1:
        break;
    case 0:
    case 2:
        memset(lbl_804B2FD8, 0, sizeof(lbl_804B2FD8));
        break;
    }
}

void fn_8027D9A4(int slot, int ok) {
    switch (ok) {
    case 0:
        fn_802E30C8("PathCareer: Identity called to say that %d was cleared", slot);
        memset(&lbl_804B2FD8[slot], 0, sizeof(CareerSave));
        break;
    }
}

void fn_8027DA08(void) {
    CareerSave* s = &lbl_804B2FD8[IdentGetCurrentSlot()];
    s->points = 0;
    for (int i = 0; i < 3; i++) {
        s32 total, max;
        fn_8027BCC0(i, &total, &max);
        s->points += total;
    }
    int got = 0;
    int all = 0;
    for (int i = 0; i < 3; i++) {
        s32 total, max;
        fn_8027BCC8(i, &total, &max);
        got += total;
        all += max;
    }
    s->percent = PMin(PMax(got * 100 / all, 0), 100);
}

void fn_8027DAF4(void) {
    fn_802E6BD8('PCRR', 1, lbl_804B2FD8, sizeof(lbl_804B2FD8), fn_8027D96C);
    IdentRegisterCallback(fn_8027D9A4);
}

int fn_8027DE68(int i);

VehicleList* fn_8027DB40(u32 flags) {
    int all = flags & 1;
    int showAll = (flags >> 1) & 1;
    int lockedOut = (flags >> 2) & 1;
    if (IsUnlockAll()) {
        all = 1;
    }
    int maxPoints = GetMaxExcitePoints();
    int limit = 0;
    if (!all && !fn_8027DE68(0)) {
        limit = 1;
    }
    int n = 0;
    for (int i = 0; i < GetUnlockableCount(); i++) {
        Unlockable* u = GetUnlockable(i);
        if (limit && u->points > maxPoints) {
            continue;
        }
        if (u && !strcasecmp(u->type, "CAR") && CountVehiclesNamed(u->name) > 0) {
            n++;
        }
    }
    if (IsExtremeChassisUnlocked()) {
        n++;
    }
    if (IsCareerComplete()) {
        n++;
    }
    VehicleList* list = fn_8025007C(n);
    bool checkSpecial = !showAll;
    for (int i = 0; i < GetUnlockableCount(); i++) {
        Unlockable* u = GetUnlockable(i);
        if (limit && u->points > maxPoints) {
            continue;
        }
        if (u && !strcasecmp(u->type, "CAR") && CountVehiclesNamed(u->name) > 0) {
            VehicleInfo* v = FindVehicleNamed(u->name, 0);
            int unlocked = 1;
            if (!all && !IsCarUnlocked(u->name)) {
                unlocked = 0;
            }
            int special = 0;
            if (!(v->flags & 2) && v->a8 != v->a4) {
                special = 1;
            }
            if (!unlocked && lockedOut) {
                continue;
            }
            int sp = 0;
            if (special && (all || (checkSpecial && IsSpecialChassisUnlocked(u->name)))) {
                sp = 1;
            }
            list->Add(u->name, unlocked, sp);
        }
    }
    if (IsExtremeChassisUnlocked()) {
        list->Add(GetAllExtremesChassis(), 1, 0);
    }
    if (IsCareerComplete()) {
        list->Add(GetCareerCompletedChassis(), 1, 0);
    }
    return list;
}

void fn_8027E0C8(Path* (*getPath)(int), int which);

void fn_8027DDCC(int which) {
    lbl_8055F66C = 1;
    int mode = GetEveMode();
    if (mode != 0x130) {
        if (mode != 0x134) {
            UnloadEves();
        }
        LoadModeEves(0x130);
    }
    fn_8027E0C8(fn_8027B834, which);
    lbl_8055F66C = 0;
    int cur = GetEveMode();
    if (mode != cur) {
        if (cur != 0x134) {
            UnloadEves();
        }
        if (mode != 0x134) {
            LoadModeEves(mode);
        }
    }
}

int fn_8027DE68(int i) { return fn_8027B94C(i); }

void fn_8027DE6C(void) {
    fn_8027B77C();
    fn_8027BF38();
    fn_8027B690();
    fn_8027DA08();
}

s32 fn_8027DE98(int slot) { return lbl_804B2FD8[slot].points; }

s32 fn_8027DEAC(void) { return fn_8027DE98(IdentGetCurrentSlot()); }

s32 fn_8027DED0(void) { return lbl_8055C790; }

void fn_8027DED8(void) {}

static inline bool HasChild(ValueNode* n, const char* name) { return n->FindChild(name) != 0; }

int fn_8027DEDC(s32* vehicle, s32* x, int track, int which) {
    SetNumPlayers(1);
    TrackInfo* t = GetTrack(track);
    const char* def = 0;
    if (t && t->node && HasChild(t->node, "vehicle_map")) {
        def = t->node->FindChild("vehicle_map")->GetString("default_vehicle", 0);
    }
    VehicleList* list;
    if (!def) {
        list = fn_8027DB40(0);
    } else {
        list = fn_8025007C(1);
        list->Add(def, 1, 0);
    }
    int r = fn_8024DAE0(vehicle, x, 0, list, which);
    delete list;
    return r;
}

void fn_8027E024(void* out, int event, int which) {
    CareerRun run;
    run.track = GetEvent(event)->track;
    run.index = -1;
    for (int i = GetTrack(run.track)->events; i-- > 0;) {
        if (event == FindTrackEvent(run.track, i)->event) {
            run.index = i;
        }
    }
    run.which = which;
    run.out = out;
    fn_8023BF4C(&run);
}

void fn_8027E0C8(Path* (*getPath)(int), int which) {
    s32 sel = -1;
    s32 driver = GetDriver(0)->id;
    s32 vehicle = GetVehicle(GetDriver(0)->vehicle)->a4;
    s32 x = GetDriver(0)->a50;
    IPath* path = getPath(which);
    fn_802840F4();
    CareerResult res;
    memset(&res, 0, sizeof(res));
    while (fn_802840FC(&sel, path, &res)) {
        memset(&res, 0, sizeof(res));
        if (!fn_8027DEDC(&vehicle, &x, GetEvent(sel)->track, which)) {
            continue;
        }
        int bi = -1;
        int ci = -1;
        int wasUnlocked = 0;
        int wasOpen = 0;
        int bracketUnlocked = 0;
        int bracketDone = 0;
        int prevDone = 1;
        for (int i = 0; i < path->Count(); i++) {
            IBracket* b = path->Get(i);
            int unl = b->Unlocked();
            int p = prevDone;
            prevDone = 0;
            if (p && unl) {
                prevDone = 1;
            }
            for (int k = 0; k < b->Count(); k++) {
                IContest* c = b->Get(k);
                if (sel == c->EventId()) {
                    bi = i;
                    ci = k;
                    wasUnlocked = c->Unlocked();
                    wasOpen = c->Done();
                    bracketUnlocked = unl;
                    bracketDone = b->AllDone();
                }
            }
        }
        int oldBest = path->Get(bi)->Get(ci)->Best();
        int oldPaths = fn_8027BAB8(path);
        VehicleInfo* v = FindVehicle(vehicle);
        int special = !(v->flags & 2);
        int hadSpecial = 0;
        if (special && IsSpecialChassisUnlocked(v->name)) {
            hadSpecial = 1;
        }
        int hadExtreme = IsExtremeChassisUnlocked();
        int hadCareer = IsCareerComplete();
        fn_802840F8();
        SetPlayer(0, driver, vehicle, x);
        SetPlayerName(IdentGetSlotName(IdentGetCurrentSlot()), 0);
        fn_80137C84(fn_802E4A10(), 0);
        AchSetScoreLimiter(fn_8027DE68(0) == 0);
        if (path->Get(bi)->Unlocked()) {
            lbl_8055C790 = 0;
        } else {
            lbl_8055C790 = path->Get(bi)->Get(ci)->Passing();
        }
        u8 out[0xB0];
        memset(out, 0, sizeof(out));
        fn_8027E024(out, sel, which);
        lbl_8055C790 = -1;
        ClearPlayer(0);
        AchSetScoreLimiter(0);
        path = getPath(which);
        fn_802840F4();
        if (*(s32*)(out + 8) > 0) {
            fn_8027DA08();
            int newBest = path->Get(bi)->Get(ci)->Best();
            int hasSpecial = 0;
            if (special && IsSpecialChassisUnlocked(v->name)) {
                hasSpecial = 1;
            }
            AchQuery q;
            q.a0 = 0;
            q.a4 = 0;
            q.a8 = 0;
            AchievementsQuery(&q);
            VehicleList* list = fn_8025007C(2);
            if (!hadSpecial && hasSpecial) {
                list->Add(GetSpecialChassis(v->name), 1, 0);
                const char* why = ShowUnlockReason(v->name);
                list->SetReason(list->Count() - 1, why);
            }
            if (!hadExtreme && AllExtremeCarsUnlocked2()) {
                list->Add(GetAllExtremesChassis(), 1, 0);
                const char* why = ShowExtremeUnlockReason();
                list->SetReason(list->Count() - 1, why);
            }
            fn_80284580(q.a4, q.a8, list, &x);
            if (list) {
                delete list;
            }
            fn_8025A204();
            AchievementsCommitSession();
            if (path->IsCareer()) {
                int nowUnlocked = path->Get(bi)->Get(ci)->Unlocked();
                int nowOpen = path->Get(bi)->Get(ci)->Done();
                int nowBracketUnlocked = path->Get(bi)->Unlocked();
                int nowBracketDone = path->Get(bi)->AllDone();
                int nowPaths = fn_8027BAB8(path);
                int nowPrev = 1;
                for (int i = 0; i < path->Count(); i++) {
                    int p = nowPrev;
                    nowPrev = 0;
                    if (p && path->Get(i)->Unlocked()) {
                        nowPrev = 1;
                    }
                }
                int nowCareer = IsCareerComplete();
                int d = newBest - oldBest;
                res.delta = d;
                res.event = sel;
                res.improved = d > 0;
                res.newly[0] = !wasUnlocked && nowUnlocked;
                res.newly[1] = !wasOpen && nowOpen;
                res.newly[2] = !bracketUnlocked && nowBracketUnlocked;
                res.newly[3] = !bracketDone && nowBracketDone;
                res.newly[4] = !prevDone && nowPrev;
                res.newly[5] = !oldPaths && nowPaths;
                res.newly[6] = !hadCareer && nowCareer;
            }
        }
        fn_802E7128();
    }
    fn_802840F8();
}

}  // extern "C"
