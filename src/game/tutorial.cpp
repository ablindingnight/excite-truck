// Tutorial challenges (0x80160CE4-0x801659D0): the challenge list for a tutorial event ("challenge" entries:
// stunt challenges with a list of required stunts, freestyle challenges with a score threshold), their HUD
// pop-ups and results screen, and the save-slot progress through the tutorial events. Names are guesses
// apart from the strings. FIRST DRAFT: the challenge classes' methods are not written yet.
#include "game/common.h"

extern "C" {
int strcasecmp(const char* a, const char* b);
void fn_802E30C8(const char* fmt, ...);  // debug print
int fn_801345D8(char* buf, u32 size, const char* fmt, ...);  // snprintf
s32 fn_802CAC28(const char* name, int flags);  // load texture
void fn_802CAC54(s32 tex);
s32 fn_8023512C(const char* name);
void fn_80235380(s32 art);
void fn_80176734(void);
int fn_80158618(const char* name);  // stunt type by name
void fn_80158558(void* w);          // add stunt watcher
void fn_801332D0(void* slot, const char* key, s32* value);
void fn_80133348(void* slot, const char* key, s32 value);
void fn_800121A4(void);
void fn_80137C78(int a);
void* fn_802E4A10(void);
void fn_80137C84(void* a, int b);
void fn_80137C80(int a);
void fn_8023BEC4(int event, void* out);
void fn_802E7128(void);
void fn_80148070(void* elem, u32 color, void* art);
void fn_80148134(f32 x, f32 y, f32* ox, f32* oy, s32 mode);
void fn_800103B8(f32 x, s32 a, s32 b, f32 y, f32 w, f32 h, const char* fmt, ...);
void fn_801473F8(void* layout, const char* elem, const char* fmt, ...);
void fn_801476C4(void* elem);
}

void* operator new(size_t size, const char* tag);
int IdentGetCurrentSlot(void);
const char* IdentGetSlotName(int slot);
void SetPlayerName(const char* name, int player);
void SetPlayer(int player, long driver, long vehicle, unsigned long color);
void AchievementsClearSession(void);
int GetNumEvents(void);

struct EventDesc {
    s32 event;  // offset 0x0
};

struct Event {
    s32 id;  // offset 0x0
    u8 pad4[0x20 - 0x4];
    ValueNode* vals;  // offset 0x20
};

extern "C" EventDesc* fn_801769CC(void);
Event* GetEvent(int id);

struct DriverInfo {
    u8 pad0[0x4];
    s32 id;  // offset 0x4
    u8 pad8[0x50 - 0x8];
    u32 color;  // offset 0x50
};

struct VehicleInfo {
    u8 pad0[0x4];
    s32 id;  // offset 0x4
    u8 pad8[0x64 - 0x8];
    u32 colors[1];  // offset 0x64
};

DriverInfo* GetDriver(int i);
VehicleInfo* GetDriverVehicle(long driver);
VehicleInfo* FindVehicleNamed(const char* name, int n);
VehicleInfo* FindVehicle(long id);

// A loaded overlay layout (tutovr.val / challist.val).
class Layout {
public:
    virtual ~Layout(void);
    virtual void* Find(const char* name);   // 0x0C
    virtual void* Art(const char* name);    // 0x10
    virtual void* Elem(const char* name);   // 0x14
    virtual void* Image(const char* name);  // 0x18
};

extern "C" Layout* fn_80145E70(const char* name);

struct TextBox {
    u8 pad0[0x4];
    f32 x;     // offset 0x4
    f32 y;     // offset 0x8
    s32 mode;  // offset 0xC
    f32 w;     // offset 0x10
    s32 b;     // offset 0x14
    s32 a;     // offset 0x18
};

// Something told about every awarded stunt. These interfaces come before Task because the
// compiler emits vtables in reverse declaration order (target .data: Task, ISubTaskMgr, StuntWatcher).
class StuntWatcher {
public:
    virtual void Notify(s32 type, int car, int rank) = 0;  // 0x08
};

// The race's per-mode hooks (0x2C vtable, all pure).
class ISubTaskMgr {
public:
    virtual ~ISubTaskMgr(void) = 0;
    virtual int vf0C(int a, int b, int c, int d, u32* flags) = 0;
    virtual int vf10(int a, f32* out, int car) = 0;
    virtual int vf14(void) = 0;
    virtual void vf18(void) = 0;
    virtual int vf1C(void) = 0;
    virtual void vf20(void) = 0;
    virtual void vf24(void) = 0;
    virtual int vf28(void) = 0;
};

inline ISubTaskMgr::~ISubTaskMgr(void) {}

// One challenge (and the list screen): the interface the tutorial drives.
class Task {
public:
    virtual ~Task(void) {}                                // 0x08
    virtual void Init(void) = 0;                          // 0x0C
    virtual void Shutdown(void) = 0;                      // 0x10
    virtual void Load(void) = 0;                          // 0x14
    virtual void Unload(void) = 0;                        // 0x18
    virtual void Update(void) = 0;                        // 0x1C
    virtual void Start(void) = 0;                         // 0x20
    virtual int IsDone(void) const = 0;                         // 0x24
    virtual int IsPassed(void) const = 0;                       // 0x28
    virtual int State(void) = 0;                          // 0x2C
    virtual int Key(int k) = 0;                           // 0x30
    virtual f32 Time(void) = 0;                           // 0x34
    virtual int Get(f32* out, int i) { return 0; }        // 0x38
    virtual int Query(void* car, int i, int c) = 0;       // 0x3C
    virtual void Event(int a, int b, int c) = 0;          // 0x40
    virtual int Draw(int a) = 0;                          // 0x44
    virtual int DrawResults(int a) = 0;                   // 0x48
    virtual void Draw3D(void) = 0;                        // 0x4C
    virtual int AllowPause(void) = 0;                     // 0x50
    virtual int AllowExit(void) = 0;                      // 0x54
    virtual void Camera(void) = 0;                        // 0x58
    virtual const char* Title(void) = 0;                  // 0x5C
};

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
struct FixedArr : ArrayRef<T> {
    FixedArr(void) : ArrayRef<T>(storage, N) {}
    T storage[N];
};

struct TaskList : ArrayRef<Task*> {
    TaskList(void) : ArrayRef<Task*>(storage, 16) {}
    void Push(Task* t) {
        Task** p = &data[count++];
        *p = t;
    }
    Task* storage[16];
};

const char* lbl_8055A0B0 = "tutorial_level";
s32 lbl_8055A0B4 = 1;  // tutorial level reached
s32 lbl_8055A0B8 = 1;  // tutorial level saved
s32 lbl_8055A0BC = -1;
s32 lbl_8055A0C0 = -1;

ModuleTag lbl_8055E2D0;
UnkStatic7F lbl_8055E2D8;
u8 lbl_8055E2DC;     // passed a new level
u8 lbl_8055E2DD;     // skip requested
Layout* lbl_8055E2E0;  // challist.val
Layout* lbl_8055E2E4;  // tutovr.val
s32 lbl_8055E2E8;  // bar1_top.tex
s32 lbl_8055E2EC;  // bar1_bot.tex
s32 lbl_8055E2F0;  // tut_pass.tex
s32 lbl_8055E2F4;  // tut_fail.tex
s32 lbl_8055E2F8;  // tut_timer.tex
s32 lbl_8055E2FC;  // tut_box.art
Task* lbl_8055E300;  // current task
s32 lbl_8055E304;    // current challenge index
TaskList lbl_8042FDEC;

struct TVec3 {
    f32 x;
    f32 y;
    f32 z;
};

struct TVec2 {
    f32 x;
    f32 z;
};

struct TPath {
    s32 count;  // offset 0x0
    u8 pad4[0x8 - 0x4];
    TVec2 pts[1];  // offset 0x8
};

struct TCar {
    u8 pad0[0x10];
    u32 model;  // offset 0x10
    u8 pad14[0x18 - 0x14];
    TVec3 fwd;  // offset 0x18
    u8 pad24[0x30 - 0x24];
    TVec3 vel;  // offset 0x30
    u8 pad3C[0x60 - 0x3C];
    TVec3 pos;  // offset 0x60
    u8 pad6C[0x118 - 0x6C];
    TVec3 v118;  // offset 0x118
};

class Sound {
public:
    virtual ~Sound(void);
    void Play(void) {
        play = 1;
        b4B = 0;
        b4A = 0;
    }
    u8 pad4[0x49 - 0x4];
    u8 play;  // offset 0x49
    u8 b4A;   // offset 0x4A
    u8 b4B;   // offset 0x4B
};

class Arena {
public:
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
    virtual void vf40(void);
    virtual void vf44(void);
    virtual void vf48(void);
    virtual void vf4C(void);
    virtual void vf50(void);
    virtual void vf54(void);
    virtual void vf58(void);
    virtual void vf5C(void);
    virtual void vf60(void);
    virtual void vf64(void);
    virtual void vf68(void);
    virtual void vf6C(void);
    virtual void vf70(void);
    virtual void vf74(void);
    virtual void vf78(void);
    virtual void vf7C(void);
    virtual void vf80(void);
    virtual void vf84(void);
    virtual void vf88(void);
    virtual void vf8C(void);
    virtual void vf90(void);
    virtual void Reset(TCar* car, int i);  // 0x94
};


extern "C" {
void fn_80161438(const char* title, const char* msg);
int fn_80160CE4(void);
int fn_80161D50(void);
void fn_8015857C(class StuntWatcher* w);  // remove stunt watcher
void fn_8016D294(int a, int b, int c);
Arena* fn_801B83A0(void);
struct MenuState* fn_801E7E34(void);
void fn_80230E90(int a);
void fn_801796BC(int a);
void fn_80276CB0(const char* name, int a);
int fn_801E8838(void);
void fn_80147230(void* elem, const char* fmt, ...);
void fn_8018ADBC(TCar* car, f32 v);
void fn_8018ADB4(TCar* car, f32 v);
void fn_8018ADA4(TCar* car, f32 v);
void fn_8017CFFC(TCar* car, void* m, TVec3* v, TVec3* w);
void fn_80176780(int a, int b);
void fn_8017691C(void);
void fn_801B7C8C(TCar* car, TVec2* pos, TVec2* dir);
}
const char* GetEventDisplayName(int id);
extern s32 lbl_8055E4D0;  // car count

struct MenuState {
    u8 pad0[0xDB0];
    s32 menu;  // offset 0xDB0
};
extern TCar* lbl_804348E8[];  // race cars
extern f32 lbl_8055E3C8;      // race clock
extern f32 lbl_80558DC4[];    // FLT_EPSILON

extern "C" {
int fn_801C5B60(void);
void fn_801DF074(int a, int b);
void fn_801DF380(const char* title, const char* msg, void (*cb)(const char*, const char*), void* arg);
int fn_801B2F24(void);
int fn_80139B5C(TCar* car, TPath* path);
f32 PSVECSquareDistance(const void* a, const void* b);
void fn_80276DC8(int a);
void fn_801EA140(void* p);
void fn_802A95A8(const void* a, const void* b);
void fn_802A95AC(int a);
void fn_801476D8(void* art, s32 color, s32 tex);
void fn_80130980(char* buf, s32 size, int fields, f32 secs);  // format a time
void fn_801475D0(Layout* l, const char* elem, const char* fmt, ...);
const char* fn_80131234(const char* key);  // localize
s32 fn_802C6FC0(const char* name, int a, int b);
void fn_802C77F0(s32 model, f32* box);
Sound* fn_80207848(const char* name, int flags);
int sscanf(const char* s, const char* fmt, ...);
TPath* fn_801399FC(ValueNode* n);
void PSVECNormalize(const void* a, void* b);
void PSVECCrossProduct(const void* a, const void* b, void* c);
double sqrt(double x);
void fn_80128658(void* p);  // MemFree
void fn_802C6CF8(s32 model);
}  // extern "C"
void fn_8000C630(long, int);
extern "C" {
s32 fn_802C9950(s32 a);
s32 fn_802C9994(s32 a);
void fn_802A96E8(void* pts, int n, u32 color, s32 tex, int a);
ValueTable* fn_80131810(const char* name);
void fn_80131818(ValueTable* t);
Layout* fn_80146C2C(Layout* l, ValueNode* desc);
}

struct CamFrame {
    u8 pad[0x30];
};

void GetTipsCamera(CamFrame* cam, f32* fov);
extern "C" void fn_8026A2CC(CamFrame* cam, f32 fov, int a);

// A help page: its text and its optional overlay layout.
struct HelpPage {
    const char* desc;  // offset 0x0
    Layout* lyt;       // offset 0x4
};

// A token to collect: a point (coin) or a line to cross.
struct EVec3 {
    EVec3(void) {}
    f32 x;
    f32 y;
    f32 z;
};

struct Token {
    Token(void) {}
    TVec3 pos;      // offset 0x0
    EVec3 ends[2];  // offset 0xC
    TVec2 mid;      // offset 0x24
    TVec2 dir;      // offset 0x2C
    TPath* path;    // offset 0x34
    f32 bob;        // offset 0x38
    u8 active;      // offset 0x3C
    u8 crossed[8];  // offset 0x3D
};

// A challenge required stunt: type and minimum rank.
struct StuntReq {
    s32 type;  // offset 0x0
    s32 rank;  // offset 0x4
    u8 done;   // offset 0x8
};

class CSubTaskMgr : public ISubTaskMgr {
public:
    virtual ~CSubTaskMgr(void) {}
    virtual int vf0C(int a, int b, int c, int d, u32* flags);
    virtual int vf10(int a, f32* out, int car);
    virtual int vf14(void);
    virtual void vf18(void);
    virtual int vf1C(void);
    virtual void vf20(void);
    virtual void vf24(void);
    virtual int vf28(void);
};

namespace {

class Challenge;
class StuntChallenge;
class FreestyleChallenge;
static inline void SetupChallenge(Challenge* c, ValueNode* n);
static inline void SetupStunt(StuntChallenge* c, ValueNode* n);
static inline void SetupFreestyle(FreestyleChallenge* c, ValueNode* n);

// The challenges' common part: help pages, the result and place targets.
class ChallengeBase : public Task {
public:
    ChallengeBase(ValueNode* n) { node = n; }
    virtual ~ChallengeBase(void) {}
    virtual void Init(void) {}
    virtual void Shutdown(void) {}
    virtual void Load(void) {
        char buf[0x10];
        int i;
        ValueTable* cmds;
        int n;
        pages.count = 0;
        cmds = fn_80131810("tutcmd.val");
        n = 0;
        do {
            n++;
            fn_801345D8(buf, sizeof(buf), "desc%d", n);
        } while (node->Has(buf));
        for (i = 0; i < n - 1; i++) {
            HelpPage* p = &pages.data[pages.count++];
            int k = i + 1;
            fn_801345D8(buf, sizeof(buf), "desc%d", k);
            p->desc = node->GetValue(buf);
            fn_801345D8(buf, sizeof(buf), "lyt%d", k);
            p->lyt = 0;
            if (node->Has(buf)) {
                const char* name = node->GetValue(buf);
                if (cmds) {
                    ValueNode* d = cmds->FindChild(name);
                    if (d) {
                        p->lyt = fn_80146C2C(lbl_8055E2E4, d);
                    }
                }
            }
        }
        if (cmds) {
            fn_80131818(cmds);
        }
    }
    virtual void Unload(void) {
        int i;
        for (i = 0; i < pages.count; i++) {
            if (pages.data[i].lyt) {
                delete pages.data[i].lyt;
            }
        }
        pages.count = 0;
    }
    virtual void Update(void) {
        if (!fn_801C5B60()) {
            fn_801DF074(8, 0);
        }
    }
    virtual void Start(void) {
        int i;
        result = 0;
        resultSet = 0;
        resultTime = -1000.0f;
        for (i = 0; i < pages.count; i++) {
            fn_801DF380(node->GetString("title", ""), pages.data[i].desc, fn_80161438, pages.data[i].lyt);
        }
    }
    virtual int Key(int k) { return 0; }
    virtual f32 Time(void) { return 5.0f; }
    virtual int Get(f32* out, int i) {
        *out = speed[i];
        return 1;
    }
    virtual int Query(void* car, int i, int c);
    virtual int Draw(int a) { return 0; }
    virtual int DrawResults(int a) { return 0; }
    virtual void Draw3D(void) {}
    virtual int AllowPause(void) { return 1; }
    virtual void Camera(void) {}
    virtual const char* Title(void) { return node->GetString("title", ""); }
    virtual void SetResult(int r) {  // 0x60
        if (!resultSet) {
            result = r;
            resultSet = 1;
            resultTime = lbl_8055E3C8;
        }
    }
    virtual int TimedOut(void) = 0;  // 0x64

    ValueNode* node;           // offset 0x4
    s32 result;                // offset 0x8
    f32 resultTime;            // offset 0xC
    u8 resultSet;              // offset 0x10
    f32 speed[8];              // offset 0x14
    FixedArr<HelpPage, 8> pages;  // offset 0x34
};

// A timed challenge: collect the tokens (and, in the subclasses, do the stunts) before the time runs out.
class Challenge : public ChallengeBase {
public:
    Challenge(ValueNode* n) : ChallengeBase(n) { SetupChallenge(this, n); }
    virtual ~Challenge(void) {}
    virtual void Init(void);
    virtual void Shutdown(void);
    virtual void Update(void) {
        ChallengeBase::Update();
        Collect();
    }
    virtual void Start(void) {
        int i;
        ChallengeBase::Start();
        got = 0;
        time = 0.0f;
        passed = 0;
        endTime = 0.0f;
        cont = 0;
        crashed = 0;
        ready = 1;
        for (i = 0; i < tokens.count; i++) {
            Token* t = &tokens.data[i];
            int j;
            fn_801EA140(t);
            t->pos.y += t->bob;
            if (t->path) {
                for (j = 0; j < sizeof(t->ends) / sizeof(t->ends[0]); j++) {
                    fn_801EA140(&t->ends[j]);
                }
            }
            t->active = 1;
            if (t->path) {
                fn_802A95A8(&t->mid, &t->dir);
                fn_802A95AC(1);
            }
        }
    }
    virtual int IsDone(void) const { return result != 0; }
    virtual int IsPassed(void) const { return result == 1; }
    virtual int State(void) {
        int r = 0;
        if (IsDone() && cont) {
            r = 1;
        }
        return r;
    }
    virtual int Key(int k) {
        if (endTime > 0.5f) {
            if (k == 'X') {
                if (cont) {
                    lbl_8055E2DD = 1;
                }
                cont = 1;
                return 1;
            }
            if (k == 0x1Bu) {
                lbl_8055E2DD = 1;
                return 1;
            }
        }
        return 0;
    }
    virtual void Event(int a, int b, int c) {
        if (!IsDone()) {
            switch (a) {
            case 0:
                if (lbl_804348E8[b]->model == 'PCAR') {
                    SetResult(2);
                    crashed = 1;
                }
                break;
            }
        }
    }
    virtual int Draw(int a) {
        f32 now = lbl_8055E3C8;
        if (now > Time() && !IsPassed()) {
            char buf[0x14];
            fn_801476D8(lbl_8055E2E0->Art("timer_art"), -1, lbl_8055E2F8);
            fn_801476C4(lbl_8055E2E0->Find("timer_title"));
            fn_80130980(buf, sizeof(buf), 3, limit - time);
            fn_801475D0(lbl_8055E2E0, "timer", "%s", buf);
        }
        return 0;
    }
    virtual int DrawResults(int a) {
        if (endTime > 0.5f) {
            fn_801476C4(lbl_8055E2E0->Find("continue"));
        }
        return 1;
    }
    virtual void Draw3D(void);
    virtual int AllowExit(void) { return endTime < 0.5f; }
    virtual void Camera(void);
    virtual int TimedOut(void) { return time > limit; }

    void Collect(void);

    s32 needed;      // offset 0x80
    f32 limit;       // offset 0x84
    u8 passed;       // offset 0x88
    f32 endTime;     // offset 0x8C
    u8 cont;         // offset 0x90
    u8 crashed;      // offset 0x91
    s32 got;         // offset 0x94
    f32 time;        // offset 0x98
    u8 ready;        // offset 0x9C
    s32 model;       // offset 0xA0
    u8 padA4[0xCC - 0xA4];
    f32 radius;      // offset 0xCC
    s32 tex;         // offset 0xD0
    u32 color;       // offset 0xD4
    u8 pillars;      // offset 0xD8
    Sound* snd;      // offset 0xDC
    FixedArr<Token, 16> tokens;  // offset 0xE0
};

// A stunt challenge: also do the listed stunts (only stunts: the stunts alone pass it).
class StuntChallenge : public Challenge, public StuntWatcher {
public:
    StuntChallenge(ValueNode* n) : Challenge(n) { SetupStunt(this, n); }
    virtual ~StuntChallenge(void) { fn_8015857C(this); }
    virtual void Update(void) {
        int all = AllStunts();
        if (all) {
            ready = 1;
            stuntTime += 0.016683351f;
            if (onlyStunts && stunts.count > 0) {
                SetResult(1);
            }
        } else {
            ready = 0;
        }
        Challenge::Update();
        if (!all && got >= needed) {
            SetResult(4);
        }
    }
    virtual void Start(void) {
        int i;
        Challenge::Start();
        for (i = 0; i < stunts.count; i++) {
            stunts.data[i].done = 0;
        }
        done = 0;
        stuntTime = 0.0f;
    }
    virtual int IsDone(void) const {
        f32 delay = finishDelay;
        if (IsPassed()) {
            return stuntTime > delay;
        }
        return result != 0;
    }
    virtual int IsPassed(void) const { return result == 1; }
    virtual void Event(int a, int b, int c) {
        if (!IsDone()) {
            switch (a) {
            case 2:
                Notify(0x10, b, 5);
                break;
            case 1:
                Notify(0x11, b, 5);
                break;
            case 3:
                Notify(0x12, b, 5);
                break;
            case 4:
                Notify(0x13, b, 5);
                break;
            }
        }
        Challenge::Event(a, b, c);
    }
    virtual int Draw(int a);
    virtual int DrawResults(int a);
    virtual void Notify(s32 type, int car, int rank) {
        if (lbl_804348E8[car]->model == 'PCAR' && done < stunts.count && !IsDone()) {
            int i;
            for (i = 0; i < stunts.count; i++) {
                StuntReq* r = &stunts.data[i];
                if (!r->done && (r->type == type || (r->type == 4 && type == 5) || (r->type == 11 && type == 12)) &&
                    r->rank <= rank) {
                    r->done = 1;
                    done++;
                }
            }
        }
    }

    int AllStunts(void) {
        int i;
        for (i = 0; i < stunts.count; i++) {
            if (!stunts.data[i].done) {
                return 0;
            }
        }
        return 1;
    }

    FixedArr<StuntReq, 4> stunts;  // offset 0x570
    s32 done;                   // offset 0x5AC
    u8 onlyStunts;              // offset 0x5B0
    f32 stuntTime;              // offset 0x5B4
    f32 finishDelay;            // offset 0x5B8
};

// A freestyle challenge: reach a score in the time.
class FreestyleChallenge : public Challenge {
public:
    FreestyleChallenge(ValueNode* n) : Challenge(n) { SetupFreestyle(this, n); }
    virtual ~FreestyleChallenge(void) {}
    virtual void Update(void) {
        if (OutOfTime()) {
            ended = 1;
        }
        Challenge::Update();
    }
    int OutOfTime(void) { return time > limit; }
    virtual void Start(void) {
        Challenge::Start();
        passedScore = 0;
        ended = 0;
        f574 = 0.0f;
        if (lbl_8055E304 > 0) {
            fn_8016D294(0, 0, lbl_8055E304 - 1);
        }
    }
    virtual int IsDone(void) const { return ended; }
    virtual int IsPassed(void) const { return passedScore; }
    virtual int State(void) { return ended; }
    virtual void Event(int a, int b, int c) {
        if (a == 5 && !ended) {
            if (c >= threshold) {
                passedScore = 1;
            }
            ended = 1;
        }
    }
    virtual int Draw(int a) {
        if (!ended) {
            return Challenge::Draw(a);
        }
        return 0;
    }
    virtual int DrawResults(int a);

    u8 passedScore;  // offset 0x56C
    u8 ended;        // offset 0x56D
    s32 threshold;   // offset 0x570
    f32 f574;        // offset 0x574
};

// The challenge list screen.
class ChalList : public Task {
public:
    ChalList(void) {}
    virtual ~ChalList(void) {}
    virtual void Init(void) {
        pick = 0;
        sel = 1;
        done = 0;
        exit = 0;
        told = 0;
    }
    virtual void Shutdown(void) {}
    virtual void Load(void) {}
    virtual void Unload(void) {}
    virtual void Update(void);
    virtual void Start(void);
    virtual int IsDone(void) const { return done; }
    virtual int IsPassed(void) const { return done; }
    virtual int State(void) { return done; }
    virtual int Key(int k);
    virtual f32 Time(void) { return 0.0f; }
    virtual int Query(void* car, int i, int c) { return 0; }
    virtual void Event(int a, int b, int c) {}
    virtual int Draw(int a);
    virtual int DrawResults(int a) { return 1; }
    virtual void Draw3D(void) {}
    virtual int AllowPause(void) { return 0; }
    virtual int AllowExit(void) { return 0; }
    virtual void Camera(void) {
        CamFrame cam;
        f32 fov;
        GetTipsCamera(&cam, &fov);
        fn_8026A2CC(&cam, fov, 0);
        fn_8000C630(0x15, -1);
    }
    virtual const char* Title(void) { return "Challenge List"; }

    u8 pick;  // offset 0x4
    u8 done;  // offset 0x5
    u8 exit;  // offset 0x6
    u8 told;  // offset 0x7
    s32 sel;  // offset 0x8
};


extern "C" int fn_80161BB8(void);

class ChalSubTaskMgr : public CSubTaskMgr {
public:
    virtual ~ChalSubTaskMgr(void) {}
    virtual int vf0C(int a, int b, int c, int d, u32* flags) {
        if (fn_80161BB8()) {
            *flags |= 0x1150;
            return 1;
        }
        return 0;
    }
    virtual int vf10(int a, f32* out, int car) {
        if (lbl_804348E8[car]) {
            *out = 0.0f;
            lbl_8042FDEC.data[lbl_8055E304]->Get(out, car);
        }
        return 0;
    }
    virtual int vf14(void) { return 1; }
    virtual int vf1C(void) { return 0; }
};

}  // namespace

namespace {

static inline void SetupChallenge(Challenge* c, ValueNode* n) {
    c->needed = 0;
    c->limit = n->GetFloat("time_limit", 10.0f);
    c->radius = 5.5f;
}

static inline void SetupStunt(StuntChallenge* c, ValueNode* n) {
    int i;
    ValueNode* s;
    c->onlyStunts = n->GetBool("only_stunts", 0);
    i = 0;
    for (s = n->GetChildAt(0); s; s = n->GetChildAt(++i)) {
        if (strcasecmp(s->GetName(), "stunt") == 0) {
            int k0 = c->stunts.count++;
            StuntReq* r = &c->stunts.data[k0];
            const char* names[4] = {"nice_landing", "turbo_jump", "pow_item", "morph_trigger"};
            const char* type = s->GetValue("type");
            int k;
            r->type = -1;
            for (k = 0; k < 4; k++) {
                if (strcasecmp(type, names[k]) == 0) {
                    r->type = k + 0x10;
                }
            }
            if (r->type == -1) {
                r->type = fn_80158618(type);
            }
            r->rank = s->GetIntValue("rank");
        }
    }
    c->finishDelay = n->GetFloat("finish_delay", 1.0f);
    fn_80158558(c ? (StuntWatcher*)c : 0);

}

static inline void SetupFreestyle(FreestyleChallenge* c, ValueNode* n) {
    int i;
    ValueNode* s;
    c->threshold = 0;
    for (i = 0; (s = n->GetChildAt(i)) != 0; i++) {
        if (strcasecmp(s->GetName(), "stunt") == 0) {
            c->threshold = s->GetIntValue("score_threshold");
        }
    }

}

}  // namespace

extern "C" {

int fn_80160CE4(void) { return lbl_8055A0B8 - lbl_8055A0C0; }

void fn_80160CF4(void) {
    Event* e = GetEvent(fn_801769CC()->event);
    int i = 1;
    ValueNode* n;
    lbl_8055E300 = 0;
    lbl_8042FDEC.Push(new ("ChalList") ChalList);
    for (n = e->vals->GetChildAt(0); n; n = e->vals->GetChildAt(i++)) {
        if (strcasecmp(n->GetName(), "challenge") == 0) {
            const char* type = n->GetValue("type");
            if (strcasecmp(type, "stunt") == 0) {
                lbl_8042FDEC.Push(new ("StuntChallenge") StuntChallenge(n));
            } else if (strcasecmp(type, "freestyle") == 0) {
                lbl_8042FDEC.Push(new ("FreestyleChallenge") FreestyleChallenge(n));
            }
        }
    }
}

int fn_80161C38(void);

int fn_801613E0(int key) {
    if (fn_80161C38()) {
        switch (key) {
        case 3:
            lbl_8055E304 = 0;
            fn_80176734();
            return 1;
        }
    }
    return 0;
}

void fn_80161438(const char* title, const char* msg) {
    if (lbl_8055E2E0) {
        void* box = lbl_8055E2E0->Elem("popup_box");
        TextBox* t;
        f32 y;
        f32 x;
        if (box) {
            fn_80148070(box, 0x96FFFFFF, lbl_8055E2E0->Image("popup_box_art"));
        }
        t = (TextBox*)lbl_8055E2E0->Elem("popup_title");
        if (t) {
            fn_80148070(t, -1, lbl_8055E2E0->Image("popup_box_art"));
        }
        t = (TextBox*)lbl_8055E2E0->Find("popup_msg");
        fn_80148134(t->x, t->y, &y, &x, t->mode);
        fn_800103B8(t->w, t->a, t->b, y, x, ((TextBox*)box)->w - 34.0f, "%s", msg);
        fn_801473F8(lbl_8055E2E0, "popup_title_elem", "%s", title);
        fn_801476C4(lbl_8055E2E0->Find("continue"));
    }
}

Event* fn_80161E40(int n);
int fn_8016208C(int event);

int fn_801615B4(int n) {
    int total = 0;
    int i;
    for (i = 0; i < n; i++) {
        total += fn_8016208C((int)fn_80161E40(i));
    }
    return total;
}

void fn_80160CF4(void);

void fn_80161618(void) {
    int i;
    u8* slot = (u8*)IdentGetCurrentSlot();
    lbl_8055A0B8 = 1;
    fn_801332D0(slot + 0xC, lbl_8055A0B0, &lbl_8055A0B8);
    lbl_8055E2DC = 0;
    lbl_8055A0B4 = lbl_8055A0B8;
    lbl_8055E2DD = 0;
    fn_80160CF4();
    if (lbl_8042FDEC.count > 1) {
        lbl_8055E304 = 0;
        lbl_8055E300 = lbl_8042FDEC.data[0];
    } else {
        lbl_8055E300 = 0;
        lbl_8055E304 = -1;
    }
    for (i = 0; i < lbl_8042FDEC.count; i++) {
        lbl_8042FDEC.data[i]->Init();
    }
}

void fn_801616FC(void) {
    int i;
    fn_80133348((u8*)IdentGetCurrentSlot() + 0xC, lbl_8055A0B0, lbl_8055A0B8);
    for (i = 0; i < lbl_8042FDEC.count; i++) {
        lbl_8042FDEC.data[i]->Shutdown();
        delete lbl_8042FDEC.data[i];
    }
    lbl_8042FDEC.count = 0;
    lbl_8055E300 = 0;
}

void fn_801617BC(void) {
    int i;
    lbl_8055E2E0 = fn_80145E70("challist.val");
    lbl_8055E2E4 = fn_80145E70("tutovr.val");
    lbl_8055E2E8 = fn_802CAC28("bar1_top.tex", 0);
    lbl_8055E2EC = fn_802CAC28("bar1_bot.tex", 0);
    lbl_8055E2F0 = fn_802CAC28("tut_pass.tex", 0);
    lbl_8055E2F4 = fn_802CAC28("tut_fail.tex", 0);
    lbl_8055E2F8 = fn_802CAC28("tut_timer.tex", 0);
    lbl_8055E2FC = fn_8023512C("tut_box.art");
    for (i = 0; i < lbl_8042FDEC.count; i++) {
        lbl_8042FDEC.data[i]->Load();
    }
}

void fn_801618B4(void) {
    int i;
    fn_802CAC54(lbl_8055E2E8);
    fn_802CAC54(lbl_8055E2EC);
    fn_802CAC54(lbl_8055E2F0);
    fn_802CAC54(lbl_8055E2F4);
    fn_802CAC54(lbl_8055E2F8);
    fn_80235380(lbl_8055E2FC);
    if (lbl_8055E2E0) {
        delete lbl_8055E2E0;
    }
    if (lbl_8055E2E4) {
        delete lbl_8055E2E4;
    }
    for (i = 0; i < lbl_8042FDEC.count; i++) {
        lbl_8042FDEC.data[i]->Unload();
    }
}

void fn_801619A8(void) {
    if (lbl_8055E304 >= 0 && lbl_8055E304 < lbl_8042FDEC.count) {
        lbl_8055E300 = lbl_8042FDEC.data[lbl_8055E304];
    }
    if (lbl_8055E300) {
        lbl_8055E300->Start();
    }
    lbl_8055E2DD = 0;
}

void fn_80161A18(void) {
    if (lbl_8055E300) {
        lbl_8055E300->Update();
    }
}

void fn_80161A38(int a, int b, int c) {
    if (lbl_8055E300) {
        lbl_8055E300->Event(a, b, c);
    }
}

int fn_80161A70(void) {
    if (lbl_8055E300) {
        if (lbl_8055E300->IsDone()) {
            return lbl_8055E300->DrawResults(0);
        }
        return lbl_8055E300->Draw(0);
    }
    return 0;
}

void fn_80161AEC(void) {
    if (lbl_8055E300) {
        lbl_8055E300->Draw3D();
    }
}

int fn_80161B0C(int unused, int key) {
    if (lbl_8055E300 && lbl_8055E300->Key(key)) {
        if (!lbl_8055E2DD) {
            fn_800121A4();
        }
        lbl_8055E2DD = 0;
        return 1;
    }
    return 0;
}

f32 fn_80161B70(void) {
    if (lbl_8055E300) {
        return lbl_8055E300->Time();
    }
    return 0.0f;
}

int fn_80161B94(void) {
    if (lbl_8055E300) {
        return lbl_8055E300->State();
    }
    return 0;
}

int fn_80161BB8(void) {
    if (lbl_8055E300) {
        return lbl_8055E300->IsDone();
    }
    return 0;
}

int fn_80161BDC(int a, int b, int c) {
    if (lbl_8055E300) {
        return lbl_8055E300->Query((void*)a, b, c);
    }
    return 0;
}

void fn_80161C18(void) {
    if (lbl_8055E300) {
        lbl_8055E300->Camera();
    }
}

int fn_80161C38(void) { return lbl_8055E300 != 0; }

int fn_80161C4C(void) {
    if (lbl_8055E300) {
        return lbl_8055E300->AllowPause();
    }
    return 1;
}

int fn_80161C70(void) {
    if (lbl_8055E300) {
        return lbl_8055E300->AllowExit();
    }
    return 1;
}

}  // extern "C"

static inline int HasNode(ValueNode* n, const char* name) { return n->FindChild(name) != 0; }

extern "C" {

CSubTaskMgr* fn_80161C94(void) {
    ValueNode* v = GetEvent(fn_801769CC()->event)->vals;
    if (HasNode(v, "challenge")) {
        return new ("chalsubtaskmgr") ChalSubTaskMgr;
    }
    return 0;
}

s32 fn_80161D14(void) {
    s32 v;
    u8* slot = (u8*)IdentGetCurrentSlot();
    v = 1;
    fn_801332D0(slot + 0xC, lbl_8055A0B0, &v);
    return v;
}

int fn_80161ECC(void);

int fn_80161D50(void) {
    int n = fn_80161ECC();
    int total = 0;
    int i;
    for (i = 0; i < n; i++) {
        Event* e = GetEvent((int)fn_80161E40(i));
        if (e->vals->GetBool("mandatory", 0)) {
            total += fn_8016208C(e->id);
        }
    }
    return total;
}

void fn_80161DE4(void) {
    int n;
    int bonus;
    n = fn_801615B4(fn_80161ECC() - 1);
    bonus = fn_8016208C((int)fn_80161E40(fn_80161ECC() - 1));
    bonus += n;
    fn_80133348((u8*)IdentGetCurrentSlot() + 0xC, lbl_8055A0B0, bonus + 1);
}

Event* fn_80161E40(int n) {
    int i;
    for (i = 0; i < GetNumEvents(); i++) {
        if (n == GetEvent(i)->vals->GetInt("tutorial", -999)) {
            return (Event*)i;
        }
    }
    return (Event*)-1;
}

int fn_80161ECC(void) {
    int n = 0;
    while ((int)fn_80161E40(n) >= 0) {
        n++;
    }
    return n;
}

void fn_80161F10(int n) {
    Event* e;
    u32 color;
    int ev;
    s32 driver;
    s32 vehicle;
    u8 buf[0xB8];
    fn_80161D14();
    ev = (int)fn_80161E40(n);
    e = GetEvent(ev);
    fn_80137C78(1);
    fn_80137C84(fn_802E4A10(), 0);
    driver = GetDriver(0)->id;
    vehicle = GetDriverVehicle(driver)->id;
    if (e->vals->Has("player_car")) {
        vehicle = FindVehicleNamed(e->vals->GetValue("player_car"), 0)->id;
    }
    color = GetDriver(driver)->color;
    if (e->vals->Has("player_color")) {
        s32 c = e->vals->GetIntValue("player_color");
        color = FindVehicle(vehicle)->colors[c];
    }
    SetPlayerName(IdentGetSlotName(IdentGetCurrentSlot()), 0);
    SetPlayer(0, driver, vehicle, color);
    lbl_8055A0BC = n;
    lbl_8055A0C0 = fn_801615B4(n);
    fn_8023BEC4(ev, buf);
    lbl_8055A0BC = -1;
    lbl_8055A0C0 = -1;
    fn_80137C80(0);
    AchievementsClearSession();
    fn_80161D14();
    fn_802E7128();
}

int fn_8016208C(int event) {
    Event* e = GetEvent(event);
    int count = 0;
    int i = 0;
    ValueNode* n;
    for (;;) {
        n = e->vals->GetChildAt(i++);
        if (!n) {
            break;
        }
        if (strcasecmp(n->GetName(), "challenge") == 0) {
            count++;
        }
    }
    return count;
}

int fn_8016212C(int key) { return fn_801613E0(key); }


}  // extern "C"

namespace {

struct Delta2 {
    f32 x;
    f32 z;
};

inline void Challenge::Collect(void) {
    if (!IsDone()) {
        int i;
        TCar* car;
        Token* t;
        int j;
        for (i = 0; i < fn_801B2F24(); i++) {
            car = lbl_804348E8[i];
            if (car->model == 'PCAR') {
                for (j = 0; j < tokens.count; j++) {
                    t = &tokens.data[j];
                    if (t->active) {
                        if (t->path) {
                            if (fn_80139B5C(car, t->path)) {
                                if (!t->crossed[i]) {
                                    Delta2 v;
                                    v.x = car->v118.x;
                                    v.z = car->v118.z;
                                    if (v.x * v.x + v.z * v.z > 0.001f) {
                                        f32 inv = 1.0f / (f32)sqrt(v.x * v.x + v.z * v.z);
                                        v.x *= inv;
                                        v.z *= inv;
                                        if (v.x * t->dir.x + v.z * t->dir.z > 0.1f) {
                                            got++;
                                            t->active = 0;
                                            snd->Play();
                                        }
                                    }
                                }
                                t->crossed[i] = 1;
                                break;
                            } else if (t->active) {
                                t->crossed[i] = 0;
                            }
                        } else if (PSVECSquareDistance(t, &car->pos) < radius * radius) {
                            got++;
                            t->active = 0;
                            snd->Play();
                            break;
                        }
                    }
                }
            }
        }
        time += 0.016683351f;
        if (time >= limit) {
            SetResult(3);
        }
        if (got >= needed && ready) {
            SetResult(1);
            passed = 1;
        }
    } else {
        if ((f32)__fabs(endTime) <= lbl_80558DC4[0]) {
            int i;
            fn_80276DC8(0);
            for (i = 0; i < fn_801B2F24(); i++) {
                fn_801B83A0()->Reset(lbl_804348E8[i], i);
            }
        }
        endTime += 0.016683351f;
        if (cont) {
            if (IsPassed() && fn_80160CE4() == lbl_8055E304) {
                lbl_8055E2DC = 1;
                lbl_8055A0B4 = lbl_8055A0B8;
                lbl_8055A0B8++;
            } else {
                lbl_8055E2DC = 0;
            }
            lbl_8055E304 = 0;
            if (lbl_8042FDEC.count > 0) {
                fn_80176734();
            }
        }
    }
}

inline void Challenge::Init(void) {
    const char* name = "growcoin.mod";
    model = fn_802C6FC0(name, 0, 0);
    if (model == -1) {
        fn_802E30C8("Couldn't load challenge token model %s", name);
    } else {
        f32 box[6];
        f32 m;
        f32 w;
        fn_802C77F0(model, box);
        m = box[5] - box[4];
        w = box[3] - box[2];
        m = w > m ? w : m;
        w = box[1] - box[0];
        m = w > m ? w : m;
        radius = m * 0.5f;
    }
    tex = fn_802CAC28("zonelite.tex", 0);
    snd = fn_80207848("levelup.sfx", 1);
    pillars = node->GetBool("draw_pillars", 0);
    if (sscanf(node->GetString("pillar_color", "#ffffff00"), "#%x", &color) != 1) {
        color = -1;
    }
    {
        ValueNode* n = node;
        tokens.count = 0;
        if (n && HasNode(n, "end")) {
            Token* t = &tokens.data[tokens.count++];
            TPath* p;
            int best = -1;
            f32 max = -999.0f;
            int i;
            t->path = p = fn_801399FC(n->FindChild("end"));
            for (i = 1; i <= p->count; i++) {
                TVec2* c = &p->pts[i % p->count];
                TVec2* b = &p->pts[(i - 1) % p->count];
                Delta2 d;
                d.x = b->x - c->x;
                d.z = b->z - c->z;
                if (d.x * d.x + d.z * d.z > max) {
                    max = d.x * d.x + d.z * d.z;
                    best = i;
                }
            }
            {
                TVec2* a = &t->path->pts[(best - 1) % t->path->count];
                TVec2* b = &t->path->pts[best % t->path->count];
                TVec2* c = &t->path->pts[(best + 1) % t->path->count];
                TVec3 e1;
                TVec3 e2;
                TVec3 n3;
                t->ends[0].x = a->x;
                t->ends[0].y = 0.0f;
                t->ends[0].z = a->z;
                t->ends[1].x = b->x;
                t->ends[1].y = 0.0f;
                t->ends[1].z = b->z;
                t->mid.x = (a->x + b->x) * 0.5f;
                t->mid.z = (a->z + b->z) * 0.5f;
                e1.x = c->x - b->x;
                e1.y = 0.0f;
                e1.z = c->z - b->z;
                e2.x = a->x - b->x;
                e2.y = 0.0f;
                e2.z = a->z - b->z;
                PSVECNormalize(&e1, &e1);
                PSVECNormalize(&e2, &e2);
                PSVECCrossProduct(&e1, &e2, &n3);
                if (n3.y > 0.0f) {
                    t->dir.x = a->x - t->mid.x;
                    t->dir.z = a->z - t->mid.z;
                } else {
                    t->dir.x = b->x - t->mid.x;
                    t->dir.z = b->z - t->mid.z;
                }
            }
            {
                f32 x = -t->dir.z;
                f32 z = t->dir.x;
                f32 inv;
                t->dir.x = x;
                t->dir.z = z;
                inv = 1.0f / (f32)sqrt(x * x + z * z);
                t->dir.x *= inv;
                t->dir.z *= inv;
            }
            for (i = 0; i < 8; i++) {
                t->crossed[i] = 0;
            }
            t->active = 1;
            needed++;
        }
    }
}

inline void Challenge::Shutdown(void) {
    int i;
    for (i = 0; i < tokens.count; i++) {
        if (tokens.data[i].path) {
            fn_80128658(tokens.data[i].path);
        }
    }
    delete snd;
    fn_802CAC54(tex);
    fn_802C6CF8(model);
}

inline void Challenge::Camera(void) {
    if (IsDone()) {
        if (lbl_8055E3C8 - resultTime > 4.0f) {
            CamFrame cam;
            f32 fov;
            GetTipsCamera(&cam, &fov);
            fn_8026A2CC(&cam, fov, 0);
            fn_8000C630(0x15, -1);
        } else {
            fn_8000C630(0x20, -1);
        }
    }
}

inline void Challenge::Draw3D(void) {
    s32 z = fn_802C9950(-0x100);
    s32 c = fn_802C9994(0);
    if (model != -1) {
        int i;
        for (i = 0; i < tokens.count; i++) {
            Token* t = &tokens.data[i];
            if (t->active && t->path && pillars) {
                fn_802A96E8(t->ends, 2, color, tex, 0);
            }
        }
    }
    fn_802C9950(z);
    fn_802C9994(c);
}

static inline void DrawBars(void) {
    fn_801476D8(lbl_8055E2E0->Art("bar_top"), -1, lbl_8055E2E8);
    fn_801476D8(lbl_8055E2E0->Art("bar_bot"), -1, lbl_8055E2EC);
}

inline int StuntChallenge::Draw(int a) { return Challenge::Draw(a); }

inline int StuntChallenge::DrawResults(int a) {
    const char* body;
    TextBox* t;
    TextBox* art;
    DrawBars();
    if (IsPassed()) {
        fn_801476D8(lbl_8055E2E0->Art("results_art"), -1, lbl_8055E2F0);
        fn_801473F8(lbl_8055E2E0, "results1", fn_80131234("Tutorial:PASSED_HEAD"));
        body = fn_80131234("Tutorial:PASSED_BODY");
    } else {
        fn_801476D8(lbl_8055E2E0->Art("results_art"), -1, lbl_8055E2F4);
        fn_801473F8(lbl_8055E2E0, "results1", fn_80131234("Tutorial:FAILED_HEAD"));
        if (result == 4) {
            body = fn_80131234("Tutorial:FAILED_BODY_STUNT");
        } else if (result == 2) {
            body = fn_80131234("Tutorial:FAILED_BODY_CRASH");
        } else {
            body = fn_80131234("Tutorial:FAILED_BODY_TIME");
        }
    }
    t = (TextBox*)lbl_8055E2E0->Find("results2");
    art = (TextBox*)lbl_8055E2E0->Art("results_art");
    if (t && art) {
        f32 px;
        f32 py;
        fn_80148134(t->x, t->y, &px, &py, t->mode);
        fn_800103B8(t->w, t->a, t->b, px, py, art->w - 100.0f, "%s", body);
    }
    if (endTime > 0.5f) {
        fn_801476C4(lbl_8055E2E0->Find("continue"));
    }
    return 1;
}

inline int FreestyleChallenge::DrawResults(int a) {
    const char* body;
    TextBox* t;
    TextBox* art;
    DrawBars();
    if (IsPassed()) {
        fn_801476D8(lbl_8055E2E0->Art("results_art"), -1, lbl_8055E2F0);
        fn_801473F8(lbl_8055E2E0, "results1", fn_80131234("Tutorial:PASSED_HEAD"));
        body = fn_80131234("Tutorial:PASSED_BODY");
    } else {
        fn_801476D8(lbl_8055E2E0->Art("results_art"), -1, lbl_8055E2F4);
        fn_801473F8(lbl_8055E2E0, "results1", fn_80131234("Tutorial:FAILED_HEAD"));
        if (OutOfTime()) {
            body = fn_80131234("Tutorial:FAILED_BODY_TIME");
        } else if (crashed) {
            body = fn_80131234("Tutorial:FAILED_BODY_CRASH");
        } else {
            body = fn_80131234("Tutorial:FAILED_BODY_STUNT");
        }
    }
    t = (TextBox*)lbl_8055E2E0->Find("results2");
    art = (TextBox*)lbl_8055E2E0->Art("results_art");
    if (t && art) {
        f32 px;
        f32 py;
        fn_80148134(t->x, t->y, &px, &py, t->mode);
        fn_800103B8(t->w, t->a, t->b, px, py, art->w - 100.0f, "%s", body);
    }
    if (endTime > 0.5f) {
        fn_801476C4(lbl_8055E2E0->Find("continue"));
    }
    return 1;
}

inline int ChalList::Key(int k) {
    if (fn_801E7E34()->menu == 0) {
        if (!exit) {
            switch (k) {
            case 0x126: {
                int s = --sel;
                if (s < 1) {
                    sel = lbl_8042FDEC.count;
                } else if (s > lbl_8042FDEC.count) {
                    sel = 1;
                } else if (s > fn_80160CE4() && s < lbl_8042FDEC.count) {
                    sel = fn_80160CE4();
                }
                fn_80230E90(1);
                return 1;
            }
            case 0x128: {
                int s = ++sel;
                if (s < 1) {
                    sel = lbl_8042FDEC.count;
                } else if (s > lbl_8042FDEC.count) {
                    sel = 1;
                } else if (s > fn_80160CE4() && s < lbl_8042FDEC.count) {
                    sel = lbl_8042FDEC.count;
                }
                fn_80230E90(1);
                return 1;
            }
            case 'X':
                pick = 1;
                return 1;
            case 0x1B:
                lbl_8055E2DD = 1;
                return 1;
            }
        } else {
            switch (k) {
            case 'X':
                sel = lbl_8042FDEC.count;
                pick = 1;
                return 1;
            case 'S':
                exit = 0;
                return 1;
            }
        }
    }
    return 0;
}

inline void ChalList::Start(void) {
    TVec2 b;
    TVec2 a;
    fn_801796BC(1);
    {
        int n = fn_80160CE4();
        if (lbl_8055E2DC && n <= lbl_8042FDEC.count) {
            sel = n;
        }
    }
    if (sel >= lbl_8042FDEC.count) {
        sel = lbl_8042FDEC.count;
    }
    pick = 0;
    exit = 0;
    done = 0;
    b.x = 9999.0f;
    b.z = 9999.0f;
    a.x = 1.0f;
    a.z = 0.0f;
    fn_802A95A8(&b, &a);
    fn_80276CB0("challist", 0);
    {
        fn_801DF380(fn_80131234("Tutorial:FINISHED_HEAD"), (fn_80131234("Tutorial:FINISHED_BODY")), fn_80161438, 0);
    }
}

struct CarState {
    TVec3 basis[3];  // offset 0x0
    TVec3 origin;    // offset 0x24
};

inline void ChalList::Update(void) {
    int i;
    if (lbl_8055A0B4 <= fn_80161D50() && lbl_8055A0B8 > fn_80161D50() && !told) {
        fn_801DF074(8, 1);
        told = 1;
    }
    for (i = 0; i < lbl_8055E4D0; i++) {
        TCar* car = lbl_804348E8[i];
        CarState m;
        TVec3 v;
        TVec3 w;
        fn_8018ADBC(car, 1.0f);
        fn_8018ADB4(car, 0.0f);
        fn_8018ADA4(car, 0.0f);
        m = *(CarState*)&car->fwd;
        m.origin.x = 0.0f;
        m.origin.y = 999.0f + 20.0f * (f32)i;
        m.origin.z = 0.0f;
        w.x = 0.0f;
        w.y = 0.0f;
        w.z = 0.0f;
        v.x = 0.0f;
        v.y = 0.0f;
        v.z = 0.0f;
        fn_8017CFFC(car, &m, &v, &w);
    }
    if (pick) {
        fn_801796BC(0);
        pick = 0;
        done = 1;
        lbl_8055E304 = sel;
        if (sel < lbl_8042FDEC.count) {
            fn_80176780(0x8002, 0);
        } else {
            fn_8017691C();
        }
    }
}

struct ElemPos {
    f32 x;
    f32 y;
};

struct ElemHead {
    ElemHead(void) {}
    const char* name;  // offset 0x0
    ElemPos pos;       // offset 0x4
    s32 just;          // offset 0xC
};

struct TextElem : ElemHead {
    f32 f10;     // offset 0x10
    u32 w14;    // offset 0x14
    u32 color;  // offset 0x18
    u32 w1C;    // offset 0x1C
};

struct GartElem : ElemHead {
    f32 w;        // offset 0x10
    f32 h;      // offset 0x14
    u32 color;  // offset 0x18
};

extern "C" {
void fn_80147940(Layout* l, GartElem* e, const char* set, int part);
void fn_80230F78(u8* alpha, f32* pulse, u32* color, int a);
}

static u8 lbl_8055E308;  // warned about the elems

inline int ChalList::Draw(int a) {
    if (exit) {
        fn_80161438(fn_80131234("Tutorial:EXIT_HEAD"), fn_80131234("Tutorial:EXIT_BODY"));
    } else {
        ::Event* e;
        TextElem* e1;
        TextElem* e2;
        GartElem* but;
        GartElem* star;
        if (fn_801E8838() > 100) {
            fn_80230E90(1);
        }
        DrawBars();
        e = GetEvent(fn_801769CC()->event);
        fn_801476C4(lbl_8055E2E0->Find("mode_title"));
        fn_801473F8(lbl_8055E2E0, "list_title", "%s", e->vals->GetString("friendly_name", GetEventDisplayName(e->id)));
        fn_801476C4(lbl_8055E2E0->Find("continue"));
        e1 = (TextElem*)lbl_8055E2E0->Find("elem1");
        e2 = (TextElem*)lbl_8055E2E0->Find("elem2");
        but = (GartElem*)lbl_8055E2E0->Elem("sel_but");
        star = (GartElem*)lbl_8055E2E0->Elem("pass_star");
        if (e1 && e2) {
            int i;
            for (i = 1; i <= lbl_8042FDEC.count; i++) {
                char name[0xA];
                TextElem* el;
                fn_801345D8(name, sizeof(name), "elem%d", i);
                el = (TextElem*)lbl_8055E2E0->Find(name);
                if (el) {
                    GartElem b;
                    TextElem te = *el;
                    f32 dx = te.pos.x - e1->pos.x;
                    f32 dy = te.pos.y - e1->pos.y;
                    b = *but;
                    int off = i != sel;
                    u8 alpha;
                    f32 pulse;
                    if (i > fn_80160CE4() && i != lbl_8042FDEC.count) {
                        b.color = 0x55FFFFFF;
                    }
                    b.pos.y += dy;
                    b.pos.x += dx;
                    fn_80147940(lbl_8055E2E0, &b, "tutbutton_art", off);
                    if (i > fn_80160CE4() && i < lbl_8042FDEC.count) {
                        te.color = e2->color;
                    } else {
                        te.color = e1->color;
                    }
                    alpha = 0xFF;
                    pulse = 0.0f;
                    if (i == sel) {
                        fn_80230F78(&alpha, &pulse, &te.color, 1);
                        b.pos.x -= 0.5f * b.w * pulse;
                        b.pos.y -= 0.5f * b.h * pulse;
                        b.w *= 1.0f + pulse;
                        b.h *= 1.0f + pulse;
                        b.color = (alpha << 24) | (b.color & 0xFFFFFF);
                        fn_80147940(lbl_8055E2E0, &b, "tutbutton_art", off);
                    }
                    if (i != lbl_8042FDEC.count) {
                        if (i < fn_80160CE4()) {
                            GartElem d = *star;
                            d.pos.x += dx;
                            d.pos.y += dy;
                            fn_80147940(lbl_8055E2E0, &d, "pass_dot_art", 2);
                        } else {
                            int done = i <= fn_80160CE4();
                            GartElem d = *star;
                            d.pos.x += dx;
                            d.pos.y += dy;
                            if (!done) {
                                d.color = 0x55FFFFFF;
                            }
                            fn_80147940(lbl_8055E2E0, &d, "pass_dot_art", 3);
                        }
                    }
                    if (i < lbl_8042FDEC.count) {
                        fn_80147230(&te, "%s", lbl_8042FDEC.data[i]->Title());
                    } else {
                        fn_80147230(&te, "%s", fn_80131234("Tutorial:EXIT_HEAD"));
                    }
                } else if (!lbl_8055E308) {
                    lbl_8055E308 = 1;
                    fn_802E30C8("%s", "Challenge: ran out of elems, have someone check challist.txt");
                }
            }
        }
    }
    return 1;
}

struct PathIter {
    u8 pad[0x58];
};

struct LineNode {
    f32 x;   // offset 0x0
    f32 z;   // offset 0x4
    f32 dx;  // offset 0x8
    f32 dz;  // offset 0xC
    u8 pad10[0x14 - 0x10];
    LineNode* next;  // offset 0x14
};

struct LineHit {
    LineNode* node;  // offset 0x0
    f32 t;           // offset 0x4
    s32 max;         // offset 0x8
};

class PathSet {
public:
    virtual void vf08(void);
    virtual int GetLine(int i);  // 0x0C
};

extern "C" {
void fn_801352B8(PathIter* it);
int fn_80135D60(PathIter* it, int line);
void fn_801355C0(PathIter* it, TVec2* pos, int a, LineHit* hit, int b);
void fn_8013530C(PathIter* it, int unk);
PathSet* fn_80156A40(void);
}

inline int ChallengeBase::Query(void* carp, int idx, int c) {
    TCar* car = (TCar*)carp;
    speed[idx] = 0.0f;
    if (node) {
        char buf[0x10];
        ValueNode* place;
        fn_801345D8(buf, sizeof(buf), "place_%d", idx);
        place = node->FindChild(buf);
        if (place) {
            TVec2 pts[3];
            TVec2 pos;
            TVec2 dir;
            int k;
            int best;
            f32 max;
            f32 spd;
            pos.x = 0.0f;
            pos.z = 0.0f;
            dir.x = 0.0f;
            dir.z = 0.0f;
            for (k = 0; k < 3; k++) {
                char key[0x10];
                ValPoint p;
                fn_801345D8(key, sizeof(key), "bound%d", k);
                p = place->GetPoint(key);
                pts[k].x = p.x;
                pts[k].z = p.y;
            }
            {
                Delta2 d;
                f32 len;
                d.x = pts[2].x - pts[0].x;
                d.z = pts[2].z - pts[0].z;
                best = 2;
                max = d.x * d.x + d.z * d.z;
                d.x = pts[0].x - pts[1].x;
                d.z = pts[0].z - pts[1].z;
                len = d.x * d.x + d.z * d.z;
                if (len > max) {
                    max = len;
                    best = 0;
                }
                d.x = pts[1].x - pts[2].x;
                d.z = pts[1].z - pts[2].z;
                if (d.x * d.x + d.z * d.z > max) {
                    best = 1;
                }
            }
            {
                TVec2* a = &pts[best];
                TVec2* b = &pts[(best + 1) % 3];
                TVec2* f = &pts[(best + 2) % 3];
                f32 inv;
                pos.x = a->x + 0.5f * (b->x - a->x);
                pos.z = a->z + 0.5f * (b->z - a->z);
                dir.x = f->x - pos.x;
                dir.z = f->z - pos.z;
                inv = 1.0f / (f32)sqrt(dir.x * dir.x + dir.z * dir.z);
                dir.x *= inv;
                dir.z *= inv;
            }
            if (place->GetBool("on_racingline", 1)) {
                PathIter it;
                LineHit h;
                LineNode* n;
                f32 inv;
                fn_801352B8(&it);
                fn_80135D60(&it, fn_80156A40()->GetLine(0));
                h.node = 0;
                h.max = 10000;
                h.t = 0.0f;
                fn_801355C0(&it, &pos, 0, &h, 0);
                n = h.node;
                pos.x = n->x + h.t * (n->next->x - n->x);
                pos.z = n->z + h.t * (n->next->z - n->z);
                dir.x = n->dx + h.t * (n->next->dx - n->dx);
                dir.z = n->dz + h.t * (n->next->dz - n->dz);
                inv = 1.0f / (f32)sqrt(dir.x * dir.x + dir.z * dir.z);
                dir.x *= inv;
                dir.z *= inv;
                fn_8013530C(&it, -1);
            }
            fn_801B7C8C(car, &pos, &dir);
            spd = 0.44444445f * place->GetFloat("speed", 0.0f);
            car->vel.x = car->fwd.x * spd;
            car->vel.y = car->fwd.y * spd;
            car->vel.z = car->fwd.z * spd;
            speed[idx] = spd;
            return 1;
        }
    }
    return 0;
}

}  // namespace
