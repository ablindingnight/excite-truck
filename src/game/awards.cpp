// Race awards (0x801CB2D8-0x801CEC80): the "StatsModule" that watches a race's statistics. One StatHandler
// per tracked stat type (int or real) holds the player's record plus the best other event's record; pushing
// a value that beats them queues a "live update" pop-up (awards_lyt.val). It also takes the place, score,
// grade and race time once per race, feeds achievements and the high-score table, and runs the records
// animation (awards_pr.val / awardsart.art). Single-player only (Off() = more than one player). The records
// animation update/draw and the pop-up draw are placeholders. Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
void* memset(void* dst, int c, size_t n);
}  // extern "C"
void UpdateContinueOverlay(void);
extern "C" {
void* fn_80145E70(const char* name);  // layout
int fn_801345D8(char* dst, int n, const char* fmt, ...);
int fn_80160AB4(void);
int fn_80161C38(void);
void fn_80235380(void* art);
void* fn_8023512C(const char* name);
void* fn_80207848(const char* name, int a);
int fn_801EA994(void);
void fn_802E317C(const char* fmt, ...);  // fatal
}

int OptE3Demo(void);
int GetNumPlayers(void);
int GetNumEvents(void);
int IdentGetCurrentSlot(void);
u32 StatsGetTypeFromIndex(u32 i);
void AchievementsAdd(int category, long value);
void AchievementsAddModelStat(int category, const char* model, long value);
s32 StatsGetHighScoreRank(int slot, long event, long difficulty, long score, f32 time);
void StatsAddHighScore(int slot, long event, long difficulty, const char* name, long score, long grade, f32 time);
void* operator new(size_t size, const char* name);
void operator delete(void* p);

struct HighScore {
    u8 pad0[0x8];
    s32 grade;  // offset 0x8
};

HighScore* StatsGetHighScore(int slot, long event, long difficulty, int a);

struct RaceSlot {
    s32 event;  // offset 0x0
    s32 a4;
    s32 difficulty;  // offset 0x8
};

struct CarInfo {
    u8 active;  // offset 0x0
    u8 pad1[0x44 - 0x1];
    s32 kind;   // offset 0x44
    char name[0x40];   // offset 0x48
    char model[0x2C];  // offset 0x88
    u8 slot;    // offset 0xB4
};

extern "C" {
RaceSlot* fn_801769CC(void);
CarInfo* fn_801EA9A0(int car);
}

class Statistic {
public:
    Statistic(int slot, u32 type, long event, int difficulty);
    int GetType(void);
    int HasValue(void);
    int IsBetterThan(const Statistic* o) const;
    void Revert(void);
    void Commit(void);
    void Format(char* buf, int n);
    u8 pad[0x24];
};

class IntStatistic : public Statistic {
public:
    IntStatistic(int slot, u32 type, long event, int difficulty);
    int IsNewRecord(long v);
    void SetValue(long v);
};

class RealStatistic : public Statistic {
public:
    RealStatistic(int slot, u32 type, long event, int difficulty);
    int IsNewRecord(f32 v);
    void SetValue(f32 v);
};

class ArtPlayer {
public:
    ~ArtPlayer(void);
    void StopAll(void);
    void Draw(int a);
};

class ILayout {
public:
    virtual ~ILayout(void);
};

template <class T> void SafeDelete(T*& p) {
    if (p) {
        delete p;
        p = 0;
    }
}

struct StatDesc {
    s32 event;
    s32 difficulty;
};

struct StatHolder {
    Statistic* stat;  // offset 0x0
};

class StatHandler : public StatHolder {
public:
    StatHandler(Statistic* s) { stat = s; }
    virtual ~StatHandler(void) { SafeDelete(stat); }
    virtual int PushInt(long v);
    virtual int PushReal(f32 v);
};

class IntStatHandler : public StatHandler {
public:
    IntStatHandler(IntStatistic* s) : StatHandler(s) {
        best = 0;
        best2 = 0;
    }
    virtual ~IntStatHandler(void) {
        SafeDelete(best);
        SafeDelete(best2);
    }
    virtual int PushInt(long v) {
        IntStatistic* s = (IntStatistic*)stat;
        if (s->IsNewRecord(v)) {
            s->SetValue(v);
            if (!best || best->IsNewRecord(v)) {
                if (!best2 || best2->IsNewRecord(v)) {
                    return 1;
                }
            }
        }
        return 0;
    }
    IntStatistic* best;   // offset 0x8
    IntStatistic* best2;  // offset 0xC
};

class RealStatHandler : public StatHandler {
public:
    RealStatHandler(RealStatistic* s) : StatHandler(s) {
        best = 0;
        best2 = 0;
    }
    virtual ~RealStatHandler(void) {
        SafeDelete(best);
        SafeDelete(best2);
    }
    virtual int PushReal(f32 v) {
        RealStatistic* s = (RealStatistic*)stat;
        if (s->IsNewRecord(v)) {
            s->SetValue(v);
            if (!best || best->IsNewRecord(v)) {
                if (!best2 || best2->IsNewRecord(v)) {
                    return 1;
                }
            }
        }
        return 0;
    }
    RealStatistic* best;   // offset 0x8
    RealStatistic* best2;  // offset 0xC
};

// A queued "live update" pop-up.
struct Popup {
    u8 used;          // offset 0x0
    u8 shown;         // offset 0x1
    f32 time;         // offset 0x4
    StatHandler* h;   // offset 0x8
    s32 aC;           // offset 0xC
    Popup* next;      // offset 0x10
};

struct LayoutObj {
    u8 pad0[0x8];
    f32 speed;   // offset 0x8
    u8 padC[0x49 - 0xC];
    u8 b49;      // offset 0x49
    u8 b4A;      // offset 0x4A
    u8 b4B;      // offset 0x4B
    u8 b4C;      // offset 0x4C
    u8 dirty;    // offset 0x4D
};

class VObj {
public:
    virtual ~VObj(void);
};

// The handler list; its storage follows it in AwardsMgr (handlerBuf).
struct HandlerVec {
    HandlerVec(void);
    StatHandler** data;  // offset 0x0
    s32 n;               // offset 0x4
    s32 cap;             // offset 0x8
};

// Three-letter initials, "zzz" when empty.
struct Initials {
    char s[4];
};

// One high-score row: initials then score data.
struct ScoreRow {
    Initials name;  // offset 0x0
    u8 pad4[0x10];
};

// The results screen's state (0x2C0 in AwardsMgr).
struct Results {
    Results(void);
    s32 state;          // offset 0x0
    s32 f4;             // offset 0x4
    s32 f8;             // offset 0x8
    s32 fC;             // offset 0xC
    s32 f10;            // offset 0x10
    s32 f14;            // offset 0x14
    u8 pad18[8];
    ScoreRow rows[15];  // offset 0x20
    ScoreRow best[5];   // offset 0x14C
    u8 pad1B0[0x50];
    Initials i200;      // offset 0x200
    u8 pad204[0x60];
    Initials i264;      // offset 0x264
    u8 pad268[0xE];
    Initials i276;      // offset 0x276
    u8 pad27A[0x42];
};

// The popup queue: head and the link to append at.
struct PopupQueue {
    PopupQueue(void);
    Popup* head;   // offset 0x0
    Popup** tail;  // offset 0x4
};

struct Vec4 {
    f32 x, y, z, w;
};

namespace {

struct AwardsMgr {
    AwardsMgr(void);
    s32 event;                // offset 0x0
    s32 difficulty;           // offset 0x4
    LayoutObj* sfx;           // offset 0x8
    void* art;                // offset 0xC
    u8 paused;                // offset 0x10
    u8 off[4];                // offset 0x11
    HandlerVec handlers;      // offset 0x18
    StatHandler* handlerBuf[7];  // offset 0x24
    ILayout* layout;          // offset 0x40
    Popup popups[30];         // offset 0x44
    PopupQueue q;             // offset 0x29C
    ILayout* recLayout;       // offset 0x2A4
    s32 place;                // offset 0x2A8
    s32 score;                // offset 0x2AC
    s32 grade;                // offset 0x2B0
    s32 rank;                 // offset 0x2B4
    f32 time;                 // offset 0x2B8
    u8 hasTime;               // offset 0x2BC
    Results results;          // offset 0x2C0 (state), 0x2C4 f32 ...
    u8 pad57C[0x9C];
};

}  // namespace

#define AW_OFF(o) (*(f32*)((u8*)(o) + 0x2C4))
#define AW_ART(o) (*(ArtPlayer**)((u8*)(o) + 0x2C8))
#define AW_OBJ(o, i) (*(LayoutObj**)((u8*)(o) + 0x2CC + (i) * 4))
#define AW_VOBJ(o, i) (*(VObj**)((u8*)(o) + 0x2CC + (i) * 4))
#define AW_2D8(o) (*(s32*)((u8*)(o) + 0x2D8))
#define AW_SCALES(o) ((Vec4*)((u8*)(o) + 0x578))

ModuleTag lbl_8055E878;
u8 lbl_8055E880;
AwardsMgr* lbl_8055E884;


#define Disabled() (OptE3Demo() || fn_80161C38())

static inline int CarSlot(int car) {
    CarInfo* c = fn_801EA9A0(car);
    return c->active ? (c->slot & 3) : -1;
}

static inline u8 CarOff(AwardsMgr* m, int car) {
    int s = CarSlot(car);
    return s >= 0 ? m->off[s] : 1;
}

extern "C" {

void fn_801CC470(void);
void fn_801CC918(void);
void fn_801CCAEC(int car);
void fn_801CCB48(int car);
void fn_801CCBA4(void);
void fn_801CCBB4(void);
void fn_801CCBC4(void);
void fn_801CCCAC(void);
void fn_801CCDAC(u32 type, int car, long v);
void fn_801CD130(u32 type, int car, f32 v);
void fn_801CD4CC(int car, long place);
void fn_801CD5B4(int car, long score, long grade);
void fn_801CD7BC(int car, int valid, f32 t);
void fn_801CD970(int car);
void fn_801CDDCC(void);
void fn_801CE6F8(void);
int fn_801CE75C(void);
void fn_801CE7A0(void);
int fn_801CE874(int car);
int fn_801CE898(void);
int fn_801CE8CC(void);
void fn_801CB3F8(void);
void fn_801CBD48(Results* r, AwardsMgr* m);
void fn_801CDE00(Results* r);

void fn_801CB2D8(void) {
    if (Disabled()) {
        return;
    }
    lbl_8055E880 = 1;
    fn_801CC470();
    fn_801CB3F8();
}

void fn_801CB320(void) {
    if (Disabled()) {
        return;
    }
    fn_801CC918();
    fn_801CB3F8();
}

void fn_801CB360(int car) {
    if (Disabled()) {
        return;
    }
    fn_801CCAEC(car);
}

void fn_801CB3AC(int car) {
    if (Disabled()) {
        return;
    }
    fn_801CCB48(car);
}

void fn_801CB3F8(void) {
    if (Disabled()) {
        return;
    }
    int n = fn_801EA994();
    for (int i = 0; i < n; i++) {
        fn_801CCAEC(i);
    }
}

void fn_801CB464(void) {
    if (Disabled()) {
        return;
    }
    fn_801CCBA4();
}

void fn_801CB4A0(void) {
    if (Disabled()) {
        return;
    }
    fn_801CCBB4();
}

void fn_801CB4DC(void) {
    if (Disabled()) {
        return;
    }
    fn_801CCBC4();
}

void fn_801CB518(void) {
    if (Disabled()) {
        return;
    }
    lbl_8055E880 = 0;
    fn_801CCCAC();
}

void fn_801CB55C(int car, long place) {
    if (Disabled()) {
        return;
    }
    if (fn_801CE874(car)) {
        fn_801CCDAC(0x22205, car, place);
        fn_801CD4CC(car, place);
    }
}

void fn_801CB5DC(int car, long score, long grade) {
    if (Disabled()) {
        return;
    }
    fn_801CD5B4(car, score, grade);
}

void fn_801CB648(int car, int valid, f32 t) {
    if (Disabled()) {
        return;
    }
    fn_801CD7BC(car, valid, t);
}

void fn_801CB6B4(void) {
    if (Disabled()) {
        return;
    }
}

void fn_801CB6E4(u32 type, int car, long v) {
    if (Disabled()) {
        return;
    }
    fn_801CCDAC(type, car, v);
}

void fn_801CB750(u32 type, int car, f32 v) {
    if (Disabled()) {
        return;
    }
    fn_801CD130(type, car, v);
}

void fn_801CB7BC(int car) {
    if (Disabled()) {
        return;
    }
    fn_801CD970(car);
}

void fn_801CB808(void) {
    if (Disabled()) {
        return;
    }
    fn_801CDDCC();
}

void fn_801CB844(void) {
    if (Disabled()) {
        return;
    }
    fn_801CE6F8();
}

int fn_801CB880(void) {
    if (Disabled()) {
        return 1;
    }
    return fn_801CE75C();
}

void fn_801CB8C0(void) {
    if (Disabled()) {
        return;
    }
    fn_801CE7A0();
}

int fn_801CB8FC(void) { return fn_801CE8CC(); }

int fn_801CB900(StatHandler* h, int type) {
    int r = 0;
    if (h && type == h->stat->GetType()) {
        r = 1;
    }
    return r;
}

StatHandler* fn_801CB954(u32 type, const StatDesc* d) {
    switch (type & 0x300) {
    default:
        fn_802E317C("StatHandler::Create: stat type %x has invalid data type flag %x", type, type & 0x300);
    case 0x200: {
        IntStatHandler* h =
            new ("award_int") IntStatHandler(new ("int_stat") IntStatistic(IdentGetCurrentSlot(), type, d->event, d->difficulty));
        if (h) {
            int bestEv = -1;
            int bestD = -1;
            for (int diff = 0; diff < 3; diff++) {
                for (int ev = 0; ev < GetNumEvents(); ev++) {
                    if (ev != d->event || diff != d->difficulty) {
                        Statistic s(IdentGetCurrentSlot(), type, ev, diff);
                        if (s.HasValue()) {
                            if (bestEv < 0) {
                                bestEv = ev;
                                bestD = diff;
                            } else {
                                Statistic b(IdentGetCurrentSlot(), type, bestEv, bestD);
                                if (s.IsBetterThan(&b)) {
                                    bestEv = ev;
                                    bestD = diff;
                                }
                            }
                        }
                    }
                }
            }
            if (bestEv >= 0) {
                h->best = new ("int_stat") IntStatistic(IdentGetCurrentSlot(), type, bestEv, bestD);
            }
        }
        return h;
    }
    case 0x100: {
        RealStatHandler* h = new ("award_real")
            RealStatHandler(new ("real_stat") RealStatistic(IdentGetCurrentSlot(), type, d->event, d->difficulty));
        if (h) {
            int bestEv = -1;
            int bestD = -1;
            for (int diff = 0; diff < 3; diff++) {
                for (int ev = 0; ev < GetNumEvents(); ev++) {
                    if (ev != d->event || diff != d->difficulty) {
                        Statistic s(IdentGetCurrentSlot(), type, ev, diff);
                        if (s.HasValue()) {
                            if (bestEv < 0) {
                                bestEv = ev;
                                bestD = diff;
                            } else {
                                Statistic b(IdentGetCurrentSlot(), type, bestEv, bestD);
                                if (s.IsBetterThan(&b)) {
                                    bestEv = ev;
                                    bestD = diff;
                                }
                            }
                        }
                    }
                }
            }
            if (bestEv >= 0) {
                h->best = new ("real_stat") RealStatistic(IdentGetCurrentSlot(), type, bestEv, bestD);
            }
        }
        return h;
    }
    }
}

}  // extern "C"

static const char* const lbl_802F9930[] = {
    "live_update_value_excite",
    "live_update_value_super",
    "live_update_value_mirror",
};

// Layout elements as the awards layouts return them (see betarace's TextElem/GartElem).
struct AwPos {
    f32 x, y;
};

struct AwText {
    const char* name;     // offset 0x0
    AwPos pos;            // offset 0x4
    s32 elemJust;         // offset 0xC
    f32 scale;            // offset 0x10
    u32 just;             // offset 0x14
    u32 color;            // offset 0x18
    const char* defText;  // offset 0x1C
};

struct AwBox {
    const char* name;  // offset 0x0
    AwPos pos;         // offset 0x4
    s32 elemJust;      // offset 0xC
    f32 w;             // offset 0x10
    f32 h;             // offset 0x14
    u32 color;         // offset 0x18
};

class AwLayout {
public:
    virtual ~AwLayout(void);
    virtual AwText* Find(const char* name);     // 0xC
    virtual void* FindArt(const char* name);    // 0x10
    virtual AwBox* FindBox(const char* name);   // 0x14
};

const char* StatsGetLabel(u32 type);

extern "C" {
f32 fn_801E7E24(void);
int fn_8026B0C0(int player);
int fn_80204CF0(void);
void fn_80148070(AwBox* box, u32 color, void* art);
void fn_80147230(AwText* e, const char* fmt, ...);
void fn_801476C4(AwText* e);
}


extern "C" {

extern "C" void IL_Use(int k, ...);

// Records animation update (placeholder; keeps its strings in the pool).
void fn_801CBD48(Results* r, AwardsMgr* m) {
    IL_Use(0, "RecordsAnim:NEW_TIME_RECORD", "RecordsAnim:NEW_EXTRA_TIME_RECORD", "RecordsAnim:TIME_RECORD",
           "RecordsAnim:EXTRA_TIME_RECORD", "RecordsAnim:TIME", "RecordsAnim:EXTRA_TIME",
           "RecordsAnimation: invalid state %d");
}

void fn_801CC594(AwardsMgr* m);
void fn_801CC768(LayoutObj* l, f32 speed);
StatHandler** fn_801CC784(HandlerVec* v);

inline AwardsMgr::AwardsMgr(void) {
    layout = (ILayout*)fn_80145E70("awards_lyt.val");
    recLayout = (ILayout*)fn_80145E70("awards_pr.val");
    paused = 0;
    RaceSlot* s = fn_801769CC();
    event = s->event;
    difficulty = s->difficulty;
    off[0] = 1;
    off[1] = 1;
    off[2] = 1;
    off[3] = 1;
    for (int i = 1; i <= 7; i++) {
        StatDesc* d = (StatDesc*)this;
        StatHandler* h = fn_801CB954(StatsGetTypeFromIndex(i), d);
        *fn_801CC784(&handlers) = h;
    }
    sfx = (LayoutObj*)fn_80207848("levelup.sfx", 1);
    if (sfx) {
        fn_801CC768(sfx, 0.5f);
    }
    art = fn_8023512C("awardsart.art");
    fn_801CC594(this);
}

void fn_801CC470(void) {
    lbl_8055E884 = new ("StatsModule") AwardsMgr;
}

static inline void ResetAwards(AwardsMgr* m) {
    if (!m->paused) {
        for (int i = 0; i < m->handlers.n; i++) {
            if (m->handlers.data[i]) {
                m->handlers.data[i]->stat->Revert();
            }
        }
        m->q.head = 0;
        m->q.tail = &m->q.head;
        memset(m->popups, 0, sizeof(m->popups));
        m->place = 0;
        m->score = 0;
        m->grade = -1;
        m->rank = -1;
        m->time = -999.0f;
        m->hasTime = 0;
        m->results.state = 0;
        AW_OFF(m) = 0.0f;
        AW_2D8(m) = 0;
        Vec4* v = AW_SCALES(m);
        for (int i = 0; i < 10; i++) {
            v[i].x = 1.0f;
            v[i].y = 0.0f;
            v[i].z = 0.0f;
            v[i].w = 0.0f;
        }
        if (AW_ART(m)) {
            AW_ART(m)->StopAll();
        }
        for (int i = 0; i < 3; i++) {
            LayoutObj* o = AW_OBJ(m, i);
            if (o) {
                o->b4A = 1;
                o->b4C = 0;
            }
        }
    }
}

void fn_801CC594(AwardsMgr* m) { ResetAwards(m); }

void fn_801CC768(LayoutObj* l, f32 speed) {
    if (speed != l->speed) {
        l->speed = speed;
        l->dirty = 1;
    }
}

StatHandler** fn_801CC784(HandlerVec* v) { return &v->data[v->n++]; }

Results::Results(void) {
    f8 = 0;
    fC = 0;
    f10 = 0;
    f14 = 0;
    for (ScoreRow* p = rows; p < rows + 15; p++) {
        memset(p->name.s, 'z', 3);
        p->name.s[3] = 0;
    }
    memset(best[0].name.s, 'z', 3);
    best[0].name.s[3] = 0;
    memset(best[1].name.s, 'z', 3);
    best[1].name.s[3] = 0;
    memset(best[2].name.s, 'z', 3);
    best[2].name.s[3] = 0;
    memset(best[3].name.s, 'z', 3);
    best[3].name.s[3] = 0;
    memset(best[4].name.s, 'z', 3);
    best[4].name.s[3] = 0;
    memset(i200.s, 'z', 3);
    i200.s[3] = 0;
    memset(i264.s, 'z', 3);
    i264.s[3] = 0;
    memset(i276.s, 'z', 3);
    i276.s[3] = 0;
}

PopupQueue::PopupQueue(void) {
    tail = &head;
    head = 0;
}

HandlerVec::HandlerVec(void) {
    data = (StatHandler**)(this + 1);
    cap = 7;
    n = 0;
}

void fn_801CC918(void) {
    AwardsMgr* m = lbl_8055E884;
    ResetAwards(m);
}

void fn_801CCAEC(int car) {
    AwardsMgr* m = lbl_8055E884;
    int s = CarSlot(car);
    if (s >= 0) {
        m->off[s] = 1;
    }
}

void fn_801CCB48(int car) {
    AwardsMgr* m = lbl_8055E884;
    int s = CarSlot(car);
    if (s >= 0) {
        m->off[s] = 0;
    }
}

void fn_801CCBA4(void) { lbl_8055E884->paused = 1; }

void fn_801CCBB4(void) { lbl_8055E884->paused = 0; }

void fn_801CCBC4(void) {
    AwardsMgr* m = lbl_8055E884;
    if (!m->paused) {
        for (int i = 0; i < m->handlers.n; i++) {
            if (m->handlers.data[i]) {
                m->handlers.data[i]->stat->Commit();
            }
        }
        if (!fn_801CE898() && m->score != 0) {
            for (int i = 0; i < fn_801EA994(); i++) {
                CarInfo* c = fn_801EA9A0(i);
                if (c->kind == 0x50) {
                    StatsAddHighScore(IdentGetCurrentSlot(), m->event, m->difficulty, c->name, m->score, m->grade,
                                      m->time);
                    break;
                }
            }
        }
    }
}

void fn_801CCCAC(void) {
    AwardsMgr* m = lbl_8055E884;
    if (m) {
        if (m->art) {
            fn_80235380(m->art);
            m->art = 0;
        }
        if (m->sfx) {
            delete (VObj*)m->sfx;
        }
        SafeDelete<ILayout>(m->layout);
        SafeDelete<ILayout>(m->recLayout);
        for (int i = 0; i < m->handlers.n; i++) {
            if (m->handlers.data[i]) {
                delete m->handlers.data[i];
            }
        }
        m->handlers.n = 0;
        HandlerVec* v = &m->handlers;
        if (v) {
            v->data = 0;
            v->n = 0;
            v->cap = 0;
        }
        operator delete(m);
    }
    lbl_8055E884 = 0;
}

static inline int FindHandler(AwardsMgr* m, u32 type) {
    int i;
    for (i = 0; i < m->handlers.n; i++) {
        if (fn_801CB900(m->handlers.data[i], type)) {
            goto found;
        }
    }
    i = -1;
found:
    if (i < 0) {
        fn_802E317C("Awards: can't find stat info for type %d!", type);
    }
    return i;
}

static inline Popup* PopQueue(AwardsMgr* m) {
    Popup* p = m->q.head;
    if (p) {
        m->q.head = p->next;
        if (!m->q.head) {
            m->q.tail = &m->q.head;
        }
        p->next = 0;
    }
    return p;
}

static inline void ShowPopup(AwardsMgr* m, StatHandler* h) {
    Popup* found = 0;
    Popup* free = 0;
    for (int i = 0; i < 30; i++) {
        Popup* p = &m->popups[i];
        if (!p->used) {
            free = p;
        } else if (p->h == h) {
            found = &m->popups[i];
            break;
        }
    }
    if (found) {
        found->time = 3.0f;
        if (found->shown) {
            found->time = found->time - 0.25f;
        }
    } else {
        if (!free) {
            free = PopQueue(m);
        }
        free->used = 1;
        free->shown = 0;
        free->time = 3.0f;
        free->h = h;
        if (m->sfx) {
            m->sfx->b49 = 1;
            m->sfx->b4B = 0;
            m->sfx->b4A = 0;
        }
        *m->q.tail = free;
        m->q.tail = &free->next;
        free->next = 0;
    }
}

void fn_801CCDAC(u32 type, int car, long v) {
    if (fn_801CE898()) {
        return;
    }
    AwardsMgr* m = lbl_8055E884;
    if (m->paused) {
        return;
    }
    if (CarOff(m, car)) {
        return;
    }
    StatHandler* h = m->handlers.data[FindHandler(m, type)];
    if (h && h->PushInt(v)) {
        ShowPopup(m, h);
    }
}

}  // extern "C"

int StatHandler::PushInt(long v) {
    fn_802E317C("StatHandler: data type \"int\" is not accepted here!");
    return 0;
}

extern "C" {

void fn_801CD130(u32 type, int car, f32 v) {
    if (fn_801CE898()) {
        return;
    }
    AwardsMgr* m = lbl_8055E884;
    if (m->paused) {
        return;
    }
    if (CarOff(m, car)) {
        return;
    }
    StatHandler* h = m->handlers.data[FindHandler(m, type)];
    if (h && h->PushReal(v)) {
        ShowPopup(m, h);
    }
}

}  // extern "C"

int StatHandler::PushReal(f32 v) {
    fn_802E317C("StatHandler: data type \"real\" is not accepted here!");
    return 0;
}

static inline void UpdateRank(AwardsMgr* m) {
    if (m->rank < 0 && m->hasTime) {
        s32 s = m->score;
        if (s >= 0) {
            m->rank = StatsGetHighScoreRank(IdentGetCurrentSlot(), m->event, m->difficulty, s, m->time);
        }
    }
}

extern "C" {

void fn_801CD4CC(int car, long place) {
    if (fn_801CE898()) {
        return;
    }
    AwardsMgr* m = lbl_8055E884;
    if (m->paused) {
        return;
    }
    if (CarOff(m, car)) {
        return;
    }
    if (!fn_801EA9A0(car)->active) {
        return;
    }
    if (m->place == 0) {
        m->place = place;
        if (place == 1) {
            AchievementsAdd(1, 1);
        }
    } else {
        fn_802E317C("Awards: place was pushed to me twice!");
    }
}

void fn_801CD5B4(int car, long score, long grade) {
    if (fn_801CE898()) {
        return;
    }
    AwardsMgr* m = lbl_8055E884;
    if (m->paused) {
        return;
    }
    if (CarOff(m, car)) {
        return;
    }
    if (!fn_801EA9A0(car)->active) {
        return;
    }
    if (m->score == 0) {
        m->score = score;
        m->grade = grade;
        AchievementsAdd(0, score);
        if (m->grade == 0) {
            AchievementsAdd(2, 1);
        }
        const char* model = fn_801EA9A0(car)->model;
        AchievementsAddModelStat(0, model, 1);
        if (grade == 0) {
            AchievementsAddModelStat(1, model, 1);
        }
        UpdateRank(m);
    } else {
        fn_802E317C("Awards: score and grade were pushed to me twice!");
    }
}

void fn_801CD730(s32* score, s32* grade) {
    if (fn_801CE898()) {
        if (score) {
            *score = 0;
        }
        if (grade) {
            *grade = -1;
        }
    } else {
        if (score) {
            *score = lbl_8055E884->score;
        }
        if (grade) {
            *grade = lbl_8055E884->grade;
        }
    }
}

void fn_801CD7BC(int car, int valid, f32 t) {
    if (fn_801CE898()) {
        return;
    }
    AwardsMgr* m = lbl_8055E884;
    if (m->paused) {
        return;
    }
    if (CarOff(m, car)) {
        return;
    }
    if (!fn_801EA9A0(car)->active) {
        return;
    }
    if (!valid) {
        return;
    }
    if (m->time < 0.0f) {
        m->handlers.data[FindHandler(m, 0x20507)]->PushReal(t);
        m->time = t;
        m->hasTime = 1;
        UpdateRank(m);
    } else {
        fn_802E317C("Awards: race time was pushed to me twice: first %.2f, then %.2f!", m->time, t);
    }
}

// Live-update pop-up draw: the head of the queue slides in from the right for its first quarter second and
// out for its last, showing the stat's label, its value (the art for the difficulty) and the title.
void fn_801CD970(int car) {
    if (fn_801CE898()) {
        return;
    }
    AwardsMgr* m = lbl_8055E884;
    if (!m->layout || !m->q.head) {
        return;
    }
    f32 dt = fn_801E7E24();
    Popup* p;
    for (;;) {
        p = m->q.head;
        if (!p) {
            p = 0;
            break;
        }
        if (fn_8026B0C0(car)) {
            if (p->shown) {
                if (p->time < 0.25f) {
                    PopQueue(m);
                    p->time = -1.0f;
                    p->used = 0;
                } else {
                    p->time = 2.75f;
                }
            }
            p = 0;
            break;
        }
        p->shown = 1;
        p->time -= dt;
        if (!(p->time < 0.0f)) {
            break;
        }
        PopQueue(m);
        p->used = 0;
    }
    if (!p) {
        return;
    }
    AwLayout* l = (AwLayout*)m->layout;
    AwBox* box = l->FindBox("live_update_box");
    if (!box) {
        return;
    }
    f32 t = p->time < 0.25f ? p->time : 3.0f - p->time;
    f32 out = box->w + (640.0f - (box->pos.x + box->w));
    f32 k = (t - 0.0f) / 0.25f;
    k = k > 1.0f ? 1.0f : k;
    k = k > 0.0f ? k : 0.0f;
    f32 slide = out + k * (0.0f - out);
    if (box) {
        AwBox b = *box;
        b.pos.x += slide;
        fn_80148070(&b, -1, m->art);
    }
    AwText* msg = l->Find("live_update_message");
    AwText* val = l->Find(lbl_802F9930[fn_80204CF0()]);
    AwText* title = l->Find("live_update_title");
    if (msg && val && title) {
        char num[0x14];
        char label[0x40];
        char value[0x10];
        AwText e = *msg;
        e.pos.x += slide;
        Statistic* s = p->h->stat;
        s->Format(num, sizeof(num));
        fn_801345D8(label, sizeof(label), "%s", StatsGetLabel(s->GetType()));
        fn_801345D8(value, sizeof(value), "%s", num);
        fn_80147230(&e, "%s", label);
        e = *val;
        e.pos.x += slide;
        fn_80147230(&e, "%s", value);
        e = *title;
        e.pos.x += slide;
        fn_801476C4(&e);
    }
}

void fn_801CDDCC(void) {
    if (fn_801CE898()) {
        return;
    }
    fn_801CDE00(&lbl_8055E884->results);
}

// Records animation draw (placeholder; keeps its strings in the pool).
void fn_801CDE00(Results* r) {
    IL_Use(0, "records_anim", "player_score", "record_score_1", "record_grade_1", "record_time_1", "record_score_2",
           "record_grade_2", "record_time_2", "record_score_3", "record_grade_3", "record_time_3", "record_score_4",
           "record_grade_4", "record_time_4", "record_score_5", "record_grade_5", "record_time_5", "player_grade",
           "player_time", "time_record_excite", "time_record_super", "time_record_mirror", "time_record_title",
           "new_time_record_title", "time_heading", "player_name", "player_bar", "a_records.val",
           "pr_recordmove.sfx", "pr_recordland.sfx", "pr_fail.sfx");
}

struct Cmd {
    s32 op;
    s32 a;
    s32 b;
};

Cmd fn_801CE670(s32 a, s32 b) {
    Cmd c;
    c.op = 5;
    c.a = a;
    c.b = b;
    return c;
}

void fn_801CE698(void) {}

void fn_801CE69C(void) {}

void fn_801CE6A0(void) {}

Cmd fn_801CE6A4(s32 a, s32 b) {
    Cmd c;
    c.op = 1;
    c.a = a;
    c.b = b;
    return c;
}

void fn_801CE6CC(void) {}

Cmd fn_801CE6D0(s32 a, s32 b) {
    Cmd c;
    c.op = 3;
    c.a = a;
    c.b = b;
    return c;
}

void fn_801CE6F8(void) {
    if (fn_801CE898()) {
        return;
    }
    AwardsMgr* m = lbl_8055E884;
    fn_801CBD48(&m->results, m);
    if (AW_ART(m)) {
        AW_ART(m)->Draw((int)((u8*)m + 0x608));
    }
    if (m->results.state == 3) {
        UpdateContinueOverlay();
    }
}

int fn_801CE75C(void) {
    if (fn_801CE898()) {
        return 1;
    }
    return lbl_8055E884->results.state == 3;
}

void fn_801CE7A0(void) {
    if (fn_801CE898()) {
        return;
    }
    AwardsMgr* m = lbl_8055E884;
    if (AW_ART(m)) {
        delete AW_ART(m);
        AW_ART(m) = 0;
    }
    if (AW_VOBJ(m, 0)) {
        delete AW_VOBJ(m, 0);
        AW_VOBJ(m, 0) = 0;
    }
    if (AW_VOBJ(m, 1)) {
        delete AW_VOBJ(m, 1);
        AW_VOBJ(m, 1) = 0;
    }
    if (AW_VOBJ(m, 2)) {
        delete AW_VOBJ(m, 2);
        AW_VOBJ(m, 2) = 0;
    }
}

int fn_801CE874(int car) { return fn_801EA9A0(car)->active; }

int fn_801CE898(void) { return GetNumPlayers() > 1; }

int fn_801CE8CC(void) {
    if (fn_801CE898()) {
        return 1;
    }
    int need = fn_80160AB4();
    HighScore* h = StatsGetHighScore(IdentGetCurrentSlot(), fn_801769CC()->event, fn_801769CC()->difficulty, 0);
    if (h && h->grade >= 0 && h->grade <= need) {
        return 1;
    }
    s32 grade = -1;
    s32 score = 0;
    fn_801CD730(&score, &grade);
    if (grade >= 0 && grade <= need) {
        return 1;
    }
    return 0;
}

}  // extern "C"
