// Event paths (0x8027AD4C-0x8027D304): the career and challenge progressions. Each save slot holds three
// career paths and two challenge paths (ep_career.val / ep_chal.val) of brackets, each a list of contests
// ("event_goal" entries naming an event by contest_name). A reference-counted "EventPathOracle" indexes
// every bracket and contest of both files so lock/unlock state and the path texts can be looked up by name;
// contests are passed by a grade (career) or a passing grade (challenge). Names are guesses apart from the
// strings.
#include "game/common.h"

extern "C" {
int strcmp(const char* a, const char* b);
int strcasecmp(const char* a, const char* b);
ValueTable* fn_80131808(const char* name);
ValueTable* fn_80131810(const char* name);
void fn_80131818(ValueTable* t);
const char* fn_80131234(const char* key);  // localized text
int fn_801345D8(char* dst, int n, const char* fmt, ...);
int fn_80204BB4(const char* difficulty);
const char* fn_80205268(const char* key, int difficulty, int event);
void fn_802E30C8(const char* fmt, ...);
void fn_802E317C(const char* fmt, ...);  // fatal
}

class Grades {
public:
    Grades(ValueNode* table);
    void Load(ValueNode* table);
    int GetThreshold(int grade);
    s32 thresholds[5];  // offset 0x0
    s32 passing;        // offset 0x14
};

struct EventInfo {
    s32 id;  // offset 0x0
    u8 pad4[0x20 - 0x4];
    ValueNode* node;  // offset 0x20
};

struct HighScore {
    u8 pad0[0x4];
    s32 score;  // offset 0x4
    s32 grade;  // offset 0x8
};

EventInfo* GetEvent(int i);
int GetNumEvents(void);
int FindGrade(const char* name);
int IdentGetCurrentSlot(void);
void IdentSelectSlot(int slot);
HighScore* StatsGetHighScore(int slot, long event, long difficulty, int a);
void StatsAddHighScore(int slot, long event, long difficulty, const char* name, long score, long grade, f32 time);
int GetEveMode(void);
void UnloadEves(void);
void LoadModeEves(int mode);
int OptDirectToTrack(void);
void SetPlayerDriver(long a, int b);
void ClearPlayer(int i);
void* operator new(size_t size, const char* name);
void operator delete(void* p);

// A fixed-capacity array over storage the owner provides.
template <class T> struct FixedVec {
    ~FixedVec(void) {
        if (this) {
            data = 0;
            n = 0;
            cap = 0;
        }
    }
    FixedVec& operator=(const FixedVec& o) {
        if (&o != this) {
            int c = o.n;
            if (cap < c) {
                c = cap;
            }
            n = c;
            for (int i = 0; i < n; i++) {
                data[i] = o.data[i];
            }
        }
        return *this;
    }
    void RemoveAt(int k) {
        volatile s32& c = n;
        int cnt;
        if (k >= 0 && k < (cnt = c)) {
            if (k != cnt - 1) {
                data[k] = data[cnt - 1];
            }
            c = c - 1;
        }
    }
    T* data;  // offset 0x0
    s32 n;    // offset 0x4
    s32 cap;  // offset 0x8
};

class Contest {
public:
    virtual void v08(void);
    virtual int v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(void);
    virtual int v1C(void);
    virtual int EventId(void);  // 0x20
    virtual void v24(void);
    virtual void v28(void);
    virtual void v2C(void);
    virtual void v30(void);
    virtual void v34(void);
    virtual void v38(void);
    virtual void v3C(void);
    virtual void v40(void);
    virtual int v44(void);
    virtual int v48(void);
    virtual int MaxScore(void);  // 0x4C
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

class Bracket {
public:
    virtual void v08(void);
    virtual int v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual int Count(void);         // 0x18
    virtual Contest* Get(int i);     // 0x1C
    virtual int v20(void);
    virtual int v24(void);
    virtual int v28(void);
    virtual int v2C(void);
    u8 valid;                  // offset 0x4
    ValueNode* node;           // offset 0x8
    const char* id;            // offset 0xC
    const char* name;          // offset 0x10
    s32 difficulty;            // offset 0x14
    FixedVec<Contest> contests;  // offset 0x18
    Contest buf[6];            // offset 0x24
    char text[100];            // offset 0x18C
    u8 unlocked;               // offset 0x1F0
    u8 open;                   // offset 0x1F1
    u8 allDone;                // offset 0x1F2
    u8 gated;                  // offset 0x1F3
};

class Path {
public:
    ~Path(void);
    virtual void v08(void);
    virtual int Count(void);       // 0xC
    virtual Bracket* Get(int i);   // 0x10
    ValueTable* table;             // offset 0x4
    FixedVec<Bracket> brackets;    // offset 0x8
    Bracket buf[6];                // offset 0x14
    u8 career;                     // offset 0xBCC
};

struct SlotPaths {
    SlotPaths(void);
    Path career[3];  // offset 0x0
    Path chal[2];    // offset 0x2370
};

// The oracle's index entries.
struct ContestRef {
    s32 type;          // offset 0x0: 0 = career, 1 = challenge
    s32 event;         // offset 0x4
    const char* bracket;  // offset 0x8
    const char* name;  // offset 0xC
    ValueNode* node;   // offset 0x10
};

struct BracketRef {
    s32 type;          // offset 0x0
    const char* name;  // offset 0x4
    ValueNode* node;   // offset 0x8
};

struct Oracle {
    FixedVec<ContestRef> contests;     // offset 0x0
    ContestRef contestBuf[0x8C0];      // offset 0xC
    FixedVec<BracketRef> brackets;     // offset 0xAF0C
    BracketRef bracketBuf[0x14];       // offset 0xAF18
    FixedVec<ValueTable*> tables;      // offset 0xB008
    ValueTable* tableBuf[2];           // offset 0xB014
    s32 difficulty;                    // offset 0xB01C
};

// One distinct event of the career, as the event list shows it.
struct PathEvent {
    s32 event;     // offset 0x0
    s32 bracket;   // offset 0x4
    s32 index;     // offset 0x8
    s32 a0C;       // offset 0xC
    s32 a10;       // offset 0x10
    u8 unlocked;   // offset 0x14
    u8 b15;        // offset 0x15
};

struct PathEventList {
    int Find(int event) {
        for (int i = 0; i < n; i++) {
            PathEvent* e = &data[i];
            if (e->event == event) {
                return i;
            }
        }
        return -1;
    }
    PathEvent* data;  // offset 0x0
    s32 n;            // offset 0x4
};

ModuleTag lbl_8055F648;
SlotPaths lbl_804A4380[4];
u8 lbl_8055F650;
Oracle* lbl_8055F654;
s32 lbl_8055F658;

static inline bool StrEq(const char* a, const char* b);

static inline int FindContestIdx(Oracle* o, const char* name) {
    for (int i = 0; i < o->contests.n; i++) {
        if (StrEq(name, o->contests.data[i].name)) {
            return i;
        }
    }
    return -1;
}

static inline bool HasChild(ValueNode* n, const char* name) { return n->FindChild(name) != 0; }

static inline bool StrEq(const char* a, const char* b) { return strcasecmp(a, b) == 0; }

static inline void StrCopyN(char* d, const char* s, int n) {
    while ((*d++ = *s++) != '\0') {
        if (--n <= 0) {
            d[-1] = '\0';
            break;
        }
    }
}

static inline int EnterCareerEves(void) {
    int mode = GetEveMode();
    if (mode != 0x130) {
        if (mode != 0x134) {
            UnloadEves();
        }
        LoadModeEves(0x130);
    }
    return mode;
}

static inline void LeaveEves(int mode) {
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

extern "C" {

s32 fn_8027D1F4(void);
s32 fn_8027CFB4(const char* name);
int fn_8027CEC4(const char* name);
int fn_8027CE4C(const char* name);
int fn_8027CF3C(const char* name);
int fn_8027D0E0(const char* name);
int fn_8027D068(const char* name);
void fn_8027D158(const char* name, char* out, int n);
int fn_8027BA24(Path* p);
int fn_8027BAB8(Path* p);

int fn_8027AD4C(Contest* c, ValueNode* n) {
    c->node = n;
    c->valid = 1;
    if (!strcasecmp(n->GetName(), "event_goal")) {
        const char* name = n->GetValue("contest_name");
        c->difficulty = fn_8027D1F4();
        int found = 0;
        for (int i = 0; i < GetNumEvents(); i++) {
            EventInfo* e = GetEvent(i);
            if (e->node->Has("name") && !strcasecmp(e->node->GetValue("name"), name)) {
                if (found) {
                    fn_802E30C8("EventPathEvent: found two contests named %s, using the first one", name);
                } else {
                    found = 1;
                    s32 id = e->id;
                    c->event = id;
                    const char* table = fn_80205268("grade_table", c->difficulty, id);
                    if (HasChild(e->node, table)) {
                        c->grades.Load(e->node->FindChild(table));
                    } else {
                        fn_802E30C8("contest table %s has no table called \"%s\"", name, table);
                    }
                }
            }
        }
        if (!found) {
            fn_802E317C("EventPathEvent: there is no contest with \"name %s\"", name);
            return 0;
        }
        c->bestGrade = fn_8027CFB4(name);
        c->unlocked = fn_8027CEC4(name);
        c->open = fn_8027CE4C(name);
        c->done = fn_8027CF3C(name);
        HighScore* h = StatsGetHighScore(IdentGetCurrentSlot(), c->event, c->difficulty, 0);
        if (h) {
            c->best = h->score;
            c->hasBest = 1;
        } else {
            c->best = 0;
            c->hasBest = 0;
        }
    } else {
        c->valid = 0;
    }
    return c->valid;
}

u8 fn_8027AF84(Contest* c) { return c->open; }

u8 fn_8027AF8C(Contest* c) { return c->unlocked; }

void fn_8027AF94(Contest* c) { c->valid = 0; }

int fn_8027AFA0(Bracket* b, ValueNode* n) {
    b->node = n;
    b->valid = 1;
    if (n) {
        int diff = 0;
        if (n->Has("minimum_difficulty")) {
            diff = fn_80204BB4(n->GetValue("minimum_difficulty"));
            if (fn_8027D1F4() < diff) {
                b->valid = 0;
            }
        }
        if (b->valid) {
            b->name = n->GetValue("bracket_name");
            b->id = n->GetValue("identifier");
            b->difficulty = fn_8027D1F4();
            ValueNode* c;
            for (int i = 0; (c = n->GetChildAt(i)) != 0; i++) {
                if (!fn_8027AD4C(&b->contests.data[b->contests.n++], c)) {
                    b->contests.RemoveAt(b->contests.n - 1);
                }
            }
            if (b->contests.n > 0) {
                b->unlocked = fn_8027D0E0(b->name);
                b->open = fn_8027D068(b->name);
                b->allDone = 1;
                for (int j = 0; j < b->contests.n; j++) {
                    b->allDone = (b->allDone & b->contests.data[j].v48()) != 0;
                }
                fn_8027D158(b->name, b->text, 100);
                b->gated = diff > 0;
            } else {
                b->valid = 0;
            }
        }
    } else {
        b->valid = 0;
    }
    return b->valid;
}

u8 fn_8027B280(Contest* c) { return c->done; }

void fn_8027B288(Bracket* b) {
    for (int i = 0; i < b->contests.n; i++) {
        fn_8027AF94(&b->contests.data[i]);
    }
    b->contests.n = 0;
    b->name = 0;
}

void fn_8027B2F8(Path* p, const char* file, u8 career) {
    p->career = career;
    p->table = fn_80131810(file);
    if (p->table) {
        ValueNode* c;
        for (int i = 0; (c = p->table->GetChild(i)) != 0; i++) {
            if (!strcmp(c->GetName(), "bracket")) {
                if (!fn_8027AFA0(&p->brackets.data[p->brackets.n++], c)) {
                    p->brackets.RemoveAt(p->brackets.n - 1);
                }
            }
        }
        if (p->brackets.n <= 0) {
            fn_80131818(p->table);
            p->table = 0;
        }
    }
}

void fn_8027B60C(Path* p) {
    if (p->table) {
        for (int i = 0; i < p->brackets.n; i++) {
            fn_8027B288(&p->brackets.data[i]);
        }
        p->brackets.n = 0;
        fn_80131818(p->table);
        p->table = 0;
    }
}

void fn_8027CCB4(Path* p, int slot, int difficulty);
void fn_8027CD80(Path* p, int slot, int difficulty);

void fn_8027B690(void) {
    int mode = EnterCareerEves();
    for (int s = 0; s < 4; s++) {
        SlotPaths* sp = &lbl_804A4380[s];
        for (u32 i = 0; i < 3; i++) {
            fn_8027CCB4(&sp->career[i], s, i);
        }
        for (u32 i = 0; i < 2; i++) {
            fn_8027CD80(&sp->chal[i], s, i);
        }
    }
    lbl_8055F650 = 1;
    LeaveEves(mode);
}

void fn_8027B77C(void) {
    if (OptDirectToTrack() && !lbl_8055F650) {
        return;
    }
    for (u32 s = 0; s < 4; s++) {
        SlotPaths* sp = &lbl_804A4380[s];
        for (u32 i = 0; i < 3; i++) {
            fn_8027B60C(&sp->career[i]);
        }
        for (u32 i = 0; i < 2; i++) {
            fn_8027B60C(&sp->chal[i]);
        }
    }
    lbl_8055F650 = 0;
}

Path* fn_8027B834(int i) { return &lbl_804A4380[IdentGetCurrentSlot()].career[i]; }

Path* fn_8027B878(int i) { return &lbl_804A4380[IdentGetCurrentSlot()].chal[i]; }

int fn_8027B8C0(int i) { return fn_8027BA24(fn_8027B834(i)); }

int fn_8027B8E4(int i, int slot) {
    int cur = IdentGetCurrentSlot();
    IdentSelectSlot(slot);
    int r = fn_8027BAB8(fn_8027B834(i));
    IdentSelectSlot(cur);
    return r;
}

int fn_8027B94C(int i) { return fn_8027B8E4(i, IdentGetCurrentSlot()); }

int fn_8027B984(int i, int slot) {
    int cur = IdentGetCurrentSlot();
    IdentSelectSlot(slot);
    int r = fn_8027BAB8(fn_8027B878(i));
    IdentSelectSlot(cur);
    return r;
}

int fn_8027B9EC(int i) { return fn_8027B984(i, IdentGetCurrentSlot()); }

int fn_8027BA24(Path* p) {
    for (int i = 0; i < p->Count(); i++) {
        if (!p->Get(i)->v20()) {
            return 0;
        }
    }
    return 1;
}

int fn_8027BAB8(Path* p) {
    for (int i = 0; i < p->Count(); i++) {
        if (!p->Get(i)->v28()) {
            return 0;
        }
    }
    return 1;
}

void fn_8027BB4C(int difficulty, s32* total, s32* max, int clamp) {
    s32 sum = 0;
    s32 most = 0;
    Path* p = fn_8027B834(0);
    for (int i = 0; i < p->Count(); i++) {
        Bracket* b = p->Get(i);
        for (int j = 0; j < b->Count(); j++) {
            Contest* c = b->Get(j);
            HighScore* h = StatsGetHighScore(IdentGetCurrentSlot(), c->EventId(), difficulty, 0);
            if (h) {
                if (clamp) {
                    s32 m = c->MaxScore();
                    s32 s = h->score;
                    if (m < s) {
                        s = m;
                    }
                    sum += s;
                } else {
                    sum += h->score;
                }
            }
            most += c->MaxScore();
        }
    }
    if (total) {
        *total = sum;
    }
    if (max) {
        *max = most;
    }
}

void fn_8027BCC0(int difficulty, s32* total, s32* max) { fn_8027BB4C(difficulty, total, max, 0); }

void fn_8027BCC8(int difficulty, s32* total, s32* max) { fn_8027BB4C(difficulty, total, max, 1); }

void fn_8027BCD0(PathEventList* list) {
    for (int k = 0; k < 3; k++) {
        Path* p = fn_8027B834(k);
        for (int i = 0; i < p->Count(); i++) {
            Bracket* b = p->Get(i);
            for (int j = 0; j < b->Count(); j++) {
                Contest* c = b->Get(j);
                int idx = list->Find(c->EventId());
                if (idx == -1) {
                    PathEvent* e = &list->data[list->n++];
                    e->event = c->EventId();
                    e->bracket = b->v0C();
                    e->index = i;
                    e->a0C = c->v0C();
                    e->a10 = c->v1C();
                    int u = 0;
                    if (b->v24() && c->v44()) {
                        u = 1;
                    }
                    e->unlocked = u;
                    e->b15 = b->v2C();
                } else {
                    PathEvent* e = &list->data[idx];
                    int u = 0;
                    if (b->v24() && c->v44()) {
                        u = 1;
                    }
                    e->unlocked = (e->unlocked | u) != 0;
                }
            }
        }
    }
}

void fn_8027BF38(void) {
    int mode = EnterCareerEves();
    SetPlayerDriver(0, 0);
    for (int d = 0; d < 3; d++) {
        for (int ev = 0; ev < GetNumEvents(); ev++) {
            StatsAddHighScore(IdentGetCurrentSlot(), ev, d, "SNEAKER", 500, 0, 1.1f);
        }
    }
    ClearPlayer(0);
    LeaveEves(mode);
}

void fn_8027C014(ContestRef* r, s32 type, const char* bracket, ValueNode* n) {
    r->type = type;
    r->bracket = bracket;
    r->node = n;
    r->name = n->GetValue("contest_name");
    for (int i = 0; i < GetNumEvents(); i++) {
        EventInfo* e = GetEvent(i);
        if (!strcasecmp(e->node->GetString("name", ""), r->name)) {
            r->event = e->id;
        }
    }
}

void fn_8027C0D0(BracketRef* r, s32 type, ValueNode* n) {
    r->type = type;
    r->node = n;
    r->name = n->GetValue("bracket_name");
}

ContestRef* fn_8027C120(Oracle* o, const char* name) {
    int idx = FindContestIdx(o, name);
    if (idx < 0) {
        return 0;
    }
    return &o->contests.data[idx];
}

static inline int FindBracketIdx(Oracle* o, const char* name) {
    for (int i = 0; i < o->brackets.n; i++) {
        if (StrEq(name, o->brackets.data[i].name)) {
            return i;
        }
    }
    return -1;
}
int fn_8027C1C8(Oracle* o, const char* name) {
    return FindBracketIdx(o, name);
}

BracketRef* fn_8027C258(Oracle* o, int i) { return &o->brackets.data[i]; }

BracketRef* fn_8027C26C(Oracle* o, const char* name) {
    int i = fn_8027C1C8(o, name);
    if (i < 0) {
        return 0;
    }
    return &o->brackets.data[i];
}

void fn_8027C2B8(Oracle* o, s32 type, const char* file) {
    BracketRef* br;
    ValueNode* b;
    ValueTable* t = fn_80131808(file);
    if (t) {
        ValueTable** slot = &o->tables.data[o->tables.n++];
        *slot = t;
        for (int i = 0; (b = t->GetChild(i)) != 0; i++) {
            if (!strcasecmp(b->GetName(), "bracket")) {
                br = &o->brackets.data[o->brackets.n++];
                fn_8027C0D0(br, type, b);
                ValueNode* c;
                for (int j = 0; (c = b->GetChildAt(j)) != 0; j++) {
                    if (!strcasecmp(c->GetName(), "event_goal")) {
                        fn_8027C014(&o->contests.data[o->contests.n++], type, br->name, c);
                    }
                }
            }
        }
    }
}

Oracle* fn_8027C41C(Oracle* o, s32 difficulty) {
    o->contests.data = o->contestBuf;
    o->contests.n = 0;
    o->contests.cap = 0x8C0;
    o->brackets.data = o->bracketBuf;
    o->brackets.n = 0;
    o->brackets.cap = 0x14;
    o->tables.data = o->tableBuf;
    o->tables.n = 0;
    o->tables.cap = 2;
    o->difficulty = difficulty;
    fn_8027C2B8(o, 0, "ep_career.val");
    fn_8027C2B8(o, 1, "ep_chal.val");
    return o;
}

Oracle* fn_8027C4B4(Oracle* o, short del) {
    if (o) {
        o->contests.n = 0;
        o->brackets.n = 0;
        for (int i = 0; i < o->tables.n; i++) {
            fn_80131818(o->tables.data[i]);
        }
        o->tables.n = 0;
        o->tables.~FixedVec<ValueTable*>();
        o->brackets.~FixedVec<BracketRef>();
        o->contests.~FixedVec<ContestRef>();
        if (del > 0) {
            operator delete(o);
        }
    }
    return o;
}

void fn_8027C59C(s32 difficulty) {
    if (!lbl_8055F654) {
        Oracle* o = (Oracle*)operator new(sizeof(Oracle), "EventPathOracle");
        if (o) {
            o = fn_8027C41C(o, difficulty);
        }
        lbl_8055F654 = o;
        lbl_8055F658 = 1;
    } else {
        lbl_8055F658++;
    }
}

void fn_8027C610(void) {
    if (--lbl_8055F658 == 0) {
        fn_8027C4B4(lbl_8055F654, 1);
        lbl_8055F654 = 0;
    }
}

int fn_8027C650(ContestRef* r) {
    HighScore* h = StatsGetHighScore(IdentGetCurrentSlot(), r->event, fn_8027D1F4(), 0);
    if (h) {
        const char* table = fn_80205268("grade_table", lbl_8055F654->difficulty, -1);
        Grades g(GetEvent(r->event)->node->FindChild(table));
        return h->grade <= g.passing;
    }
    return 0;
}

int fn_8027C71C(ContestRef* r) {
    HighScore* h = StatsGetHighScore(IdentGetCurrentSlot(), r->event, fn_8027D1F4(), 0);
    int done = 0;
    if (h && h->grade == 0) {
        done = 1;
    }
    return done;
}

int fn_8027C788(BracketRef* b) {
    ValueNode* c;
    for (int i = 0; (c = b->node->GetChildAt(i)) != 0; i++) {
        if (!strcasecmp(c->GetName(), "event_goal")) {
            if (!fn_8027C650(fn_8027C120(lbl_8055F654, c->GetValue("contest_name")))) {
                return 0;
            }
        }
    }
    return 1;
}

int fn_8027C850(const BracketRef* b) {
    const char* name = b->name;
    int i = fn_8027C1C8(lbl_8055F654, name);
    if (i == 0) {
        return 1;
    }
    return fn_8027C788(fn_8027C26C(lbl_8055F654, fn_8027C258(lbl_8055F654, i - 1)->name));
}

int fn_8027C8B0(ContestRef* r) { return 1; }

void fn_8027C8B8(BracketRef* b, char* out, int n) {
    if (fn_8027C788(b)) {
        StrCopyN(out, fn_80131234("EventPath:COMPLETED"), n);
        out[0] = '\0';
    } else if (fn_8027C850(b)) {
        StrCopyN(out, fn_80131234("EventPath:CAREER_UNLOCKED_PATH"), n);
        out[0] = '\0';
    } else {
        char buf[100];
        fn_801345D8(buf, 100, "EventPath:CAREER_LOCKED_PATH_%s", b->node->GetValue("identifier"));
        StrCopyN(out, fn_80131234(buf), n);
    }
}

int fn_8027CA1C(ContestRef* r) {
    HighScore* h = StatsGetHighScore(IdentGetCurrentSlot(), r->event, fn_8027D1F4(), 0);
    int passed = 0;
    if (h) {
        if (h->grade <= FindGrade(r->node->GetValue("passing_grade"))) {
            passed = 1;
        }
    }
    return passed;
}

int fn_8027CAB4(ContestRef* r) {
    HighScore* h = StatsGetHighScore(IdentGetCurrentSlot(), r->event, fn_8027D1F4(), 0);
    int done = 0;
    if (h && h->grade == 0) {
        done = 1;
    }
    return done;
}

int fn_8027CB20(ContestRef* r) { return 1; }

int fn_8027CB28(BracketRef* b) {
    ValueNode* c;
    for (int i = 0; (c = b->node->GetChildAt(i)) != 0; i++) {
        if (!strcasecmp(c->GetName(), "event_goal")) {
            if (!fn_8027CA1C(fn_8027C120(lbl_8055F654, c->GetValue("contest_name")))) {
                return 0;
            }
        }
    }
    return 1;
}

int fn_8027CBF0(BracketRef* b) { return 1; }

void fn_8027CBF8(BracketRef* b, char* out, int n) {
    if (fn_8027CB28(b)) {
        StrCopyN(out, fn_80131234("EventPath:COMPLETED"), n);
    } else {
        StrCopyN(out, fn_80131234("EventPath:CHALLENGE_UNLOCKED_PATH"), n);
    }
}

void fn_8027CCB4(Path* p, int slot, int difficulty) {
    int mode = EnterCareerEves();
    int cur = IdentGetCurrentSlot();
    if (cur != slot) {
        IdentSelectSlot(slot);
    }
    fn_8027C59C(difficulty);
    fn_8027B2F8(p, "ep_career.val", 1);
    if (cur != slot) {
        IdentSelectSlot(cur);
    }
    fn_8027C610();
    LeaveEves(mode);
}

void fn_8027CD80(Path* p, int slot, int difficulty) {
    int mode = EnterCareerEves();
    int cur = IdentGetCurrentSlot();
    if (cur != slot) {
        IdentSelectSlot(slot);
    }
    fn_8027C59C(difficulty);
    fn_8027B2F8(p, "ep_chal.val", 1);
    if (cur != slot) {
        IdentSelectSlot(cur);
    }
    fn_8027C610();
    LeaveEves(mode);
}

int fn_8027CE4C(const char* name) {
    ContestRef* r = fn_8027C120(lbl_8055F654, name);
    switch (r->type) {
    case 0:
        return fn_8027C8B0(r);
    case 1:
        return fn_8027CB20(r);
    default:
        fn_802E317C("eventpath: contest %s has unknown event-path type %d", name, r->type);
        return 1;
    }
}

int fn_8027CEC4(const char* name) {
    ContestRef* r = fn_8027C120(lbl_8055F654, name);
    switch (r->type) {
    case 0:
        return fn_8027C650(r);
    case 1:
        return fn_8027CA1C(r);
    default:
        fn_802E317C("eventpath: contest %s has unknown event-path type %d", name, r->type);
        return 1;
    }
}

int fn_8027CF3C(const char* name) {
    ContestRef* r = fn_8027C120(lbl_8055F654, name);
    switch (r->type) {
    case 0:
        return fn_8027C71C(r);
    case 1:
        return fn_8027CAB4(r);
    default:
        fn_802E317C("eventpath: contest %s has unknown event-path type %d", name, r->type);
        return 1;
    }
}

s32 fn_8027CFB4(const char* name) {
    ContestRef* r = fn_8027C120(lbl_8055F654, name);
    const char* table = fn_80205268("grade_table", lbl_8055F654->difficulty, -1);
    Grades g(GetEvent(r->event)->node->FindChild(table));
    for (int i = 0; i < 5; i++) {
        int t = g.GetThreshold(i);
        if (t >= 0) {
            return t;
        }
    }
    return 0;
}

int fn_8027D068(const char* name) {
    BracketRef* b = fn_8027C26C(lbl_8055F654, name);
    switch (b->type) {
    case 0:
        return fn_8027C850(b);
    case 1:
        return fn_8027CBF0(b);
    default:
        fn_802E317C("eventpath: bracket %s has unknown event-path type %d", name, b->type);
        return 1;
    }
}

int fn_8027D0E0(const char* name) {
    BracketRef* b = fn_8027C26C(lbl_8055F654, name);
    switch (b->type) {
    case 0:
        return fn_8027C788(b);
    case 1:
        return fn_8027CB28(b);
    default:
        fn_802E317C("eventpath: bracket %s has unknown event-path type %d", name, b->type);
        return 1;
    }
}

void fn_8027D158(const char* name, char* out, int n) {
    BracketRef* b = fn_8027C26C(lbl_8055F654, name);
    switch (b->type) {
    case 0:
        fn_8027C8B8(b, out, n);
        break;
    case 1:
        fn_8027CBF8(b, out, n);
        break;
    default:
        fn_802E317C("eventpath: contest %s has unknown event-path type %d", name, b->type);
        break;
    }
}

s32 fn_8027D1F4(void) { return lbl_8055F654->difficulty; }

}  // extern "C"
