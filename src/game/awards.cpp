// Race awards (0x801CB2D8-0x801CEC80): the "StatsModule" that watches a race's statistics. One StatHandler
// per tracked stat type (int or real) holds the player's record plus the best other event's record; pushing
// a value that beats them queues a "live update" pop-up (awards_lyt.val). It also takes the place, score,
// grade and race time once per race, feeds achievements and the high-score table, and runs the records
// animation (awards_pr.val / awardsart.art). Single-player only (Off() = more than one player). The records
// animation update/draw and the pop-up draw are placeholders. Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
void* memset(void* dst, int c, size_t n);
void fn_8001DFCC(void);
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
    u32 GetType(void);
    int HasValue(void);
    int IsBetterThan(Statistic* o);
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

template <class T> void SafeDelete(T*& p);

// Owns one object.
template <class T> struct AutoDel {
    AutoDel(void) : p(0) {}
    AutoDel(T* q) : p(q) {}
    ~AutoDel(void) {
        if (p) {
            delete p;
            p = 0;
        }
    }
    T* p;
};

struct StatDesc {
    s32 event;
    s32 difficulty;
};

class StatHandler : public AutoDel<Statistic> {
public:
    StatHandler(Statistic* s) : AutoDel<Statistic>(s) {}
    virtual ~StatHandler(void) {}
    virtual int PushInt(long v);
    virtual int PushReal(f32 v);
};

class IntStatHandler : public StatHandler {
public:
    IntStatHandler(IntStatistic* s) : StatHandler(s) {}
    virtual int PushInt(long v) {
        IntStatistic* s = (IntStatistic*)p;
        if (s->IsNewRecord(v)) {
            s->SetValue(v);
            if (best.p && !best.p->IsNewRecord(v)) {
                return 0;
            }
            if (best2.p && !best2.p->IsNewRecord(v)) {
                return 0;
            }
            return 1;
        }
        return 0;
    }
    AutoDel<IntStatistic> best;   // offset 0x8
    AutoDel<IntStatistic> best2;  // offset 0xC
};

class RealStatHandler : public StatHandler {
public:
    RealStatHandler(RealStatistic* s) : StatHandler(s) {}
    virtual int PushReal(f32 v) {
        RealStatistic* s = (RealStatistic*)p;
        if (s->IsNewRecord(v)) {
            s->SetValue(v);
            if (best.p && !best.p->IsNewRecord(v)) {
                return 0;
            }
            if (best2.p && !best2.p->IsNewRecord(v)) {
                return 0;
            }
            return 1;
        }
        return 0;
    }
    AutoDel<RealStatistic> best;   // offset 0x8
    AutoDel<RealStatistic> best2;  // offset 0xC
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

struct HandlerVec {
    StatHandler** data;  // offset 0x0
    s32 n;               // offset 0x4
    s32 cap;             // offset 0x8
};

struct Results {
    s32 state;  // offset 0x0
    u8 pad4[0x2B8];
};

struct Vec4 {
    f32 x, y, z, w;
};

struct AwardsMgr {
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
    Popup* head;              // offset 0x29C
    Popup** tail;             // offset 0x2A0
    ILayout* recLayout;       // offset 0x2A4
    s32 place;                // offset 0x2A8
    s32 score;                // offset 0x2AC
    s32 grade;                // offset 0x2B0
    s32 rank;                 // offset 0x2B4
    f32 time;                 // offset 0x2B8
    u8 hasTime;               // offset 0x2BC
    Results results;          // offset 0x2C0 (state), 0x2C4 f32 ...
};

#define AW_OFF(o) (*(f32*)((u8*)(o) + 0x2C4))
#define AW_ART(o) (*(ArtPlayer**)((u8*)(o) + 0x2C8))
#define AW_OBJ(o, i) (*(LayoutObj**)((u8*)(o) + 0x2CC + (i) * 4))
#define AW_VOBJ(o, i) (*(VObj**)((u8*)(o) + 0x2CC + (i) * 4))
#define AW_2D8(o) (*(s32*)((u8*)(o) + 0x2D8))
#define AW_SCALES(o) ((Vec4*)((u8*)(o) + 0x578))

ModuleTag lbl_8055E878;
u8 lbl_8055E880;
AwardsMgr* lbl_8055E884;

static const char* const lbl_802F9930[] = {
    "live_update_value_excite",
    "live_update_value_super",
    "live_update_value_mirror",
};

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

int fn_801CB900(StatHandler* h, u32 type) {
    int r = 0;
    if (h && type == h->p->GetType()) {
        r = 1;
    }
    return r;
}

StatHandler* fn_801CB954(u32 type, const StatDesc* d) {
    switch (type & 0x300) {
    default:
        fn_802E317C("StatHandler::Create: stat type %x has invalid data type flag %x", type);
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
                h->best.p = new ("int_stat") IntStatistic(IdentGetCurrentSlot(), type, bestEv, bestD);
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
                h->best.p = new ("real_stat") RealStatistic(IdentGetCurrentSlot(), type, bestEv, bestD);
            }
        }
        return h;
    }
    }
}

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
void* fn_801CC7A0(Results* r);
void fn_801CC8EC(Popup** q);
void fn_801CC8FC(HandlerVec* v);

void fn_801CC470(void) {
    AwardsMgr* m = (AwardsMgr*)operator new(0x618, "StatsModule");
    if (m) {
        fn_801CC8FC(&m->handlers);
        fn_801CC8EC(&m->head);
        fn_801CC7A0(&m->results);
        m->layout = (ILayout*)fn_80145E70("awards_lyt.val");
        m->recLayout = (ILayout*)fn_80145E70("awards_pr.val");
        m->paused = 0;
        RaceSlot* s = fn_801769CC();
        m->event = s->event;
        m->difficulty = s->difficulty;
        m->off[0] = 1;
        m->off[1] = 1;
        m->off[2] = 1;
        m->off[3] = 1;
        for (u32 i = 1; i <= 7; i++) {
            StatDesc* d = (StatDesc*)m;
            StatHandler* h = fn_801CB954(StatsGetTypeFromIndex(i), d);
            *fn_801CC784(&m->handlers) = h;
        }
        m->sfx = (LayoutObj*)fn_80207848("levelup.sfx", 1);
        if (m->sfx) {
            fn_801CC768(m->sfx, 0.5f);
        }
        m->art = fn_8023512C("awardsart.art");
        fn_801CC594(m);
    }
    lbl_8055E884 = m;
}

static inline void ResetAwards(AwardsMgr* m) {
    if (!m->paused) {
        for (int i = 0; i < m->handlers.n; i++) {
            if (m->handlers.data[i]) {
                m->handlers.data[i]->p->Revert();
            }
        }
        m->head = 0;
        m->tail = &m->head;
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

struct Initials {
    Initials(void) {
        memset(s, 'z', 3);
        s[3] = 0;
    }
    char s[4];
    u8 pad[0x10];
};

void* fn_801CC7A0(Results* r) {
    u8* b = (u8*)r;
    *(s32*)(b + 0x8) = 0;
    *(s32*)(b + 0xC) = 0;
    *(s32*)(b + 0x10) = 0;
    *(s32*)(b + 0x14) = 0;
    for (char* p = (char*)(b + 0x20); p < (char*)(b + 0x14C); p += 0x14) {
        memset(p, 'z', 3);
        p[3] = 0;
    }
    memset(b + 0x14C, 'z', 3);
    b[0x14F] = 0;
    memset(b + 0x160, 'z', 3);
    b[0x163] = 0;
    memset(b + 0x174, 'z', 3);
    b[0x177] = 0;
    memset(b + 0x188, 'z', 3);
    b[0x18B] = 0;
    memset(b + 0x19C, 'z', 3);
    b[0x19F] = 0;
    memset(b + 0x200, 'z', 3);
    b[0x203] = 0;
    memset(b + 0x264, 'z', 3);
    b[0x267] = 0;
    memset(b + 0x276, 'z', 3);
    b[0x279] = 0;
    return r;
}

void fn_801CC8EC(Popup** q) {
    q[1] = (Popup*)q;
    q[0] = 0;
}

void fn_801CC8FC(HandlerVec* v) {
    v->data = (StatHandler**)(v + 1);
    v->cap = 7;
    v->n = 0;
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
                m->handlers.data[i]->p->Commit();
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
    Popup* p = m->head;
    if (p) {
        m->head = p->next;
        if (!m->head) {
            m->tail = &m->head;
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
        *m->tail = free;
        m->tail = &free->next;
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

// Pop-up draw (placeholder; keeps its strings in the pool).
void fn_801CD970(int car) { IL_Use(0, "live_update_box", "live_update_message", "live_update_title"); }

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
        fn_8001DFCC();
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
    for (int i = 0; i < 3; i++) {
        if (AW_VOBJ(m, i)) {
            delete AW_VOBJ(m, i);
            AW_VOBJ(m, i) = 0;
        }
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
    s32 score = 0;
    s32 grade = -1;
    fn_801CD730(&score, &grade);
    if (grade >= 0 && grade <= need) {
        return 1;
    }
    return 0;
}

}  // extern "C"
