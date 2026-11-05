#ifndef GAME_PATHCAREER_H
#define GAME_PATHCAREER_H
// Shared by pathcareer.cpp and pathclass.cpp (KC prototype: the classes' accessors are in-class inlines,
// deferred to the end of pathclass.cpp in declaration order).
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
    virtual const char* TrackName(void) const = 0;      // 0xC
    virtual const char* Name(void) const = 0;           // 0x10
    virtual const char* TypeName(void) const = 0;       // 0x14
    virtual const char* TypeDisplayName(void) const = 0;  // 0x18
    virtual const char* FriendlyName(void) const = 0;   // 0x1C
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
    const char* TrackName(void) const {
        EventInfo* e = GetEvent(event);
        if (e->node->Has("track")) {
            return GetTrackFriendlyName(e->node->GetValue("track"));
        }
        return GetTrackFriendlyName(GetTrack(e->track)->name);
    }
    const char* Name(void) const { return GetEvent(event)->node->GetValue("name"); }
    const char* TypeName(void) const { return GetEventTypeName(GetEvent(event)->type); }
    const char* TypeDisplayName(void) const { return GetEventTypeDisplayName(GetEvent(event)->type); }
    const char* FriendlyName(void) const { return GetEvent(event)->node->GetValue("friendly_name"); }
    int Best(void) { return best; }
    int BestGrade(void) {
        if (hasBest) {
            return GradeOf(best);
        }
        return -1;
    }
    int GradeOf(int score) { return grades.Find(score); }
    int Threshold(int grade) { return grades.GetThreshold(grade); }
    int EventId(void) { return event; }
    int Difficulty(void) { return difficulty; }
    int Passing(void) { return grades.passing; }
    int PassScore(void) { return grades.GetThreshold(Passing()); }
    int MaxGrade(void) { return bestGrade; }
    int Unlocked(void);
    int Open(void);
    int Done(void);
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
    const char* Id(void) { return id; }
    const char* Name(void) { return name; }
    int Difficulty(void) { return difficulty; }
    int Count(void) { return contests.n; }
    IContest* Get(int i) { return &contests.data[i]; }
    int Unlocked(void) { return unlocked; }
    int Open(void) { return open; }
    int AllDone(void) { return allDone; }
    int Gated(void) { return gated; }
    const char* Text(void) { return text; }
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
    int Count(void) { return brackets.n; }
    IBracket* Get(int i) { return &brackets.data[i]; }
    int IsCareer(void) { return career; }
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

#endif
