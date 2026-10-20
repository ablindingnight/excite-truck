// Race results (0x801AE600-0x801AFAC8): the per-car result table ("RaceResultTable": finish time, place,
// points, best lap, flags), per-car lap tables ("CarLapTable"/"LapResultTable", 501 laps each, only for
// 0x50-type entries), the race entry list copied from the race setup, and the end-of-race pass: mark the
// unfinished cars, sort the entries (by score, or by best time in time trials), hand the leftover points
// to the podium and award bonus points. The record manager hooks ("RecordMgr") are stubs in this build.
// Names are guesses apart from the allocation tags.
#include "game/common.h"

extern "C" {
void* memset(void* p, int c, u32 n);
void qsort(void* base, u32 n, u32 size, int (*cmp)(const void*, const void*));
int fn_801B2F24(void);           // racer count
s32 fn_801B3050(int i);          // racer id
int fn_801B2ED4(void);           // total laps
void fn_801B3208(int idx, int place);
int fn_801EA994(void);           // human count
u8* fn_801EA9A0(int i);          // car info (first byte: still racing)
int fn_801769B8(void);           // game mode
void* fn_80137C4C(void);
int fn_802794AC(s32 type, void* car);
int fn_801B8728(void* s);
int fn_801B2C70(void* s);
void fn_801B8778(void);
void fn_801B878C(void);
}

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);
void operator delete(void* p);
void operator delete[](void* p);

extern f32 lbl_80558DC4[];  // FLT_EPSILON

// A car's race result (0x1C).
struct RaceResult {
    f32 time;    // offset 0x0
    f32 rate;    // offset 0x4
    f32 best;    // offset 0x8
    s16 place;   // offset 0xC
    u16 flags;   // offset 0xE
    s16 x10;     // offset 0x10
    s16 bonus;   // offset 0x12
    s16 points;  // offset 0x14
    s16 score;   // offset 0x16
    s16 laps;    // offset 0x18
    s16 id;      // offset 0x1A
};

struct RaceResultTable {
    static RaceResultTable* Create(void) { return new ("RaceResultTable") RaceResultTable; }
    RaceResultTable(void) {
        n = 8;
        r = new ("record:RaceResults") RaceResult[n];
        Reset();
    }
    ~RaceResultTable(void) {
        delete[] r;
        r = 0;
    }
    void Reset(void) {
        for (int i = 0; i < n; i++) {
            RaceResult* e = &r[i];
            memset(e, 0, sizeof(RaceResult));
            e->best = 86400.0f;
            e->place = -100;
            e->id = (i < fn_801B2F24()) ? fn_801B3050(i) : -1;
        }
    }
    RaceResult* r;  // offset 0x0
    s32 n;          // offset 0x4
};

struct LapResult {
    u8 pad0[0x6];
    s16 x6;  // offset 0x6
};

struct LapResultTable {
    static LapResultTable* Create(void) { return new ("LapResultTable") LapResultTable; }
    LapResultTable(void) {
        n = 501;
        laps = new ("LapResultTable") LapResult[n];
        Reset();
    }
    ~LapResultTable(void) {
        delete[] laps;
        laps = 0;
    }
    void Reset(void) {
        for (int i = 0; i < n; i++) {
            laps[i].x6 = 0;
        }
    }
    LapResult* laps;  // offset 0x0
    s32 n;            // offset 0x4
};

struct CarLapTable {
    static CarLapTable* Create(void) { return new ("CarLapTable") CarLapTable; }
    CarLapTable(void) {
        memset(t, 0, sizeof(t));
        n = 8;
    }
    ~CarLapTable(void) {
        for (u32 i = 0; i < 8; i++) {
            if (t[i]) {
                delete t[i];
                t[i] = 0;
            }
        }
    }
    void Reset(void) {
        for (int i = 0; i < 8; i++) {
            if (t[i]) {
                t[i]->Reset();
            }
        }
    }
    LapResultTable* t[8];  // offset 0x0
    s32 n;                 // offset 0x20
};

// A race entry (0x78), copied from the race setup.
struct RaceEntry {
    RaceEntry(void) {}
    s32 type;        // offset 0x0
    char name[0x40]; // offset 0x4
    char tag[0x20];  // offset 0x44
    s32 x64;         // offset 0x64
    s32 x68;         // offset 0x68
    s32 x6C;         // offset 0x6C
    u8 x70;          // offset 0x70
    s32 x74;         // offset 0x74
};

struct RaceSetup {
    u8 pad0[0x20];
    RaceEntry e[8];  // offset 0x20
    s32 n;           // offset 0x3E0
    u8 car[1];       // offset 0x3E4
};

struct Lap2 {
    f32 a;  // offset 0x0
    f32 b;  // offset 0x4
};

struct RaceEntries {
    s32 Count(void) { return n; }
    RaceEntry e[8];      // offset 0x0
    s32 n;               // offset 0x3C0
    u8 pad3C4[0x4];
    s32 order[8];        // offset 0x3C8
    u8 misc[4][0x10];    // offset 0x3E8
};


class RecordMgr {
public:
    RecordMgr(int a);
    ~RecordMgr(void);
    void Laps(LapResult* laps, int n);
    void Results(RaceResult* r, int n);
    void Final(RaceResult* r, int n);
    void Bonus(int place);
    s32 x0;
};

ModuleTag lbl_8055E618;
CarLapTable* lbl_8055E620;
RaceResultTable* lbl_8055E624;
RecordMgr* lbl_8055E628;
s32 lbl_8055E62C;  // total laps
f32 lbl_8055E630;  // time scale
u8 lbl_8055E634;
u8 lbl_8055E635;
Lap2 lbl_80435740[8];
RaceEntries lbl_80435780;

// The best-time table and the entry list as one block (the end-of-race pass addresses both from one base).
struct RaceState {
    Lap2 best[8];         // offset 0x0
    RaceEntries entries;  // offset 0x40
};

struct Leaders {
    s32 n;
    s32 idx[8];
    s32 best;
};

extern "C" {

int fn_801AE600(s32 type) { return type == 0x50; }

int fn_801AE610(void* p, int place) { return 0; }

int fn_801AE618(u32 flags) { return flags & 1; }

int fn_801AE620(RaceResult* r) { return fn_801AE618(r->flags); }

void fn_801AE628(void* p) {
    Leaders l;
    l.best = -1;
    l.n = 0;
    for (int i = 0; i < lbl_80435780.n; i++) {
        RaceResult* r = &lbl_8055E624->r[i];
        if (fn_801AE620(r) && r->points >= l.best) {
            if (r->points > l.best) {
                l.n = 0;
            }
            l.idx[l.n++] = i;
            l.best = r->points;
        }
    }
    for (int i = 0; i < lbl_80435780.n; i++) {
        RaceResult* r = &lbl_8055E624->r[i];
        if (fn_801AE620(r)) {
            r->x10 = fn_801AE610(p, r->place);
            r->bonus = 0;
            if (r->points != 0) {
                r->bonus += 5;
            }
            for (int j = 0; j < l.n; j++) {
                if (i == l.idx[j]) {
                    r->bonus += 5;
                }
            }
        }
    }
}

int fn_801AE790(RaceEntry* e) { return e->x70 & 3; }

RaceResult* fn_801AF8D0(int i);

void fn_801AE79C(int mode) {
    int ok = 0;
    if (mode == 2 && lbl_80435780.n > 10) {
        ok = 1;
    }
    if (ok) {
        RaceEntry* e = lbl_80435780.e;
        int best = 0;
        for (int i = 0; i < lbl_80435780.n; e++, i++) {
            if (e->type == 0x50 && fn_801AE790(e) == 0) {
                RaceResult* r = fn_801AF8D0(i);
                if (r) {
                    best = r->place;
                }
            }
        }
        if (best >= 1) {
            lbl_8055E628->Bonus(best);
        }
    }
}

void fn_801AE874(u16* flags, int b) {
    *flags |= 1;
    if (b) {
        *flags |= 4;
    }
}

void fn_801AE894(RaceResult* r) {
    if (r->laps > 0 && r->time > 1.0f) {
        r->rate = lbl_8055E630 * r->laps / r->time;
        r->rate *= (f32)r->laps / lbl_8055E62C;
    } else {
        r->rate = -1.0f;
    }
}

void fn_801AE928(void) {
    int n = fn_801EA994();
    for (int i = 0; i < n; i++) {
        if (i < lbl_8055E624->n) {
            RaceResult* r = &lbl_8055E624->r[i];
            if (!fn_801AE620(r)) {
                fn_801AE874(&r->flags, 1);
                r->flags |= 8;
                r->time = -1.0f;
                fn_801AE894(r);
            }
        }
    }
}

f32 fn_801AE9E0(int i, f32* rate) {
    if (rate) {
        *rate = -1.0f;
    }
    RaceResult* r = &lbl_8055E624->r[i];
    if (fn_801AE620(r)) {
        if (rate) {
            *rate = lbl_8055E630 / r->best;
        }
        return r->best;
    }
    return 86400.0f;
}

int fn_801AEA64(f32 a, f32 b) {
    if ((f32)__fabs(a - b) <= lbl_80558DC4[0]) {
        return 0;
    }
    if (b < a) {
        return 1;
    }
    return -1;
}

int fn_801AEAA4(const void* pa, const void* pb) {
    s32 ia = *(const s32*)pa;
    s32 ib = *(const s32*)pb;
    RaceResult* a = fn_801AF8D0(ia);
    RaceResult* b = fn_801AF8D0(ib);
    if (a && b) {
        if (((a->flags | b->flags) & 8) == 0) {
            if (a->score == b->score) {
                return fn_801AEA64(a->time, b->time);
            }
            return a->score - b->score;
        }
        int r = -1;
        if (a->flags & 8) {
            r = 1;
        }
        return r;
    }
    int r = -1;
    if (!a) {
        r = 1;
    }
    return r;
}

f32 fn_801AF9EC(int i, f32* b);

int fn_801AEB58(const void* pa, const void* pb) {
    s32 ia = *(const s32*)pa;
    s32 ib = *(const s32*)pb;
    f32 a = fn_801AF9EC(ia, 0);
    f32 b = fn_801AF9EC(ib, 0);
    if (a < 1.0f) {
        a = 10000.0f;
    }
    if (b < 1.0f) {
        b = 10000.0f;
    }
    return fn_801AEA64(a, b);
}

void fn_801AEBD0(void) {
    int left = lbl_8055E62C;
    for (int i = 0; i < lbl_80435780.n; i++) {
        left -= lbl_8055E624->r[i].points;
    }
    int share = left / 3;
    int rest = left % 3;
    for (int i = 0; i < lbl_80435780.n; i++) {
        RaceResult* r = &lbl_8055E624->r[i];
        s16 place = r->place;
        if (place < 4) {
            r->points += share;
            if (place == 1) {
                lbl_8055E624->r[i].points += rest;
            }
        }
    }
}

void fn_801AED54(void) {}

void fn_801AED58(void) {}

void fn_801AED5C(void) {
    lbl_8055E634 = 1;
    fn_801B8778();
}

void fn_801AED68(void) {
    lbl_8055E634 = 0;
    fn_801B878C();
}

void fn_801AED74(void) {
    if (!lbl_8055E634) {
        lbl_8055E624->Reset();
        lbl_8055E620->Reset();
    }
    lbl_8055E62C = fn_801B2ED4();
}

RecordMgr* fn_801AFA20(RaceSetup* s);

void fn_801AEF28(RaceSetup* s) {
    RaceState* st = (RaceState*)lbl_80435740;
    lbl_8055E635 = 1;
    lbl_8055E620 = CarLapTable::Create();
    lbl_8055E624 = RaceResultTable::Create();
    lbl_8055E628 = fn_801AFA20(s);
    RaceEntries* t = &st->entries;
    t->n = 0;
    for (int i = 0; i < s->n; i++) {
        s32 type = s->e[i].type;
        if (!fn_802794AC(type, s->car)) {
            break;
        }
        t->e[i] = s->e[i];
        t->order[i] = i;
        t->n++;
        if (fn_801AE600(type)) {
            lbl_8055E620->t[i] = LapResultTable::Create();
        }
    }
    for (int i = 0; i < st->entries.n; i++) {
        st->best[i].a = -1.0f;
        st->best[i].b = 1.0f;
    }
    lbl_8055E634 = 0;
    lbl_8055E630 = 1000.0f;
    lbl_8055E62C = -1;
    for (int i = 0; i < 4; i++) {
        memset(st->entries.misc[i], 0, 0x10);
    }
}

void fn_801AFA5C(RecordMgr* m);

void fn_801AF598(void) {
    lbl_8055E635 = 0;
    fn_801AFA5C(lbl_8055E628);
    lbl_8055E628 = 0;
    delete lbl_8055E620;
    lbl_8055E620 = 0;
    delete lbl_8055E624;
    lbl_8055E624 = 0;
}

void fn_801AF65C(void) {
    for (int i = 0; i < lbl_80435780.n; i++) {
        RaceResult* r = &lbl_8055E624->r[i];
        if (fn_801AE620(r) && *fn_801EA9A0(i)) {
            r->flags |= 2;
        }
    }
}

void fn_801AF6FC(void) {
    RaceState* st = (RaceState*)lbl_80435740;
    RaceEntries* t = &st->entries;
    int mode = fn_801769B8();
    int notRace = mode != 2;
    if (!notRace) {
        fn_801AE928();
    }
    fn_801AF65C();
    for (int i = 0; i < lbl_8055E620->n; i++) {
        LapResultTable* t = lbl_8055E620->t[i];
        if (t) {
            lbl_8055E628->Laps(t->laps, t->n);
        }
    }
    if (!notRace) {
        lbl_8055E628->Results(lbl_8055E624->r, lbl_8055E624->n);
    }
    if (mode == 1) {
        for (int i = 0; i < t->n; i++) {
            st->best[i].a = fn_801AE9E0(i, &st->best[i].b);
        }
    }
    qsort(t->order, t->n, 4, (mode == 1) ? fn_801AEB58 : fn_801AEAA4);
    for (int i = 0; i < t->n; i++) {
        lbl_8055E624->r[t->order[i]].place = i + 1;
    }
    fn_801AEBD0();
    fn_801AE628(fn_80137C4C());
    if (!notRace) {
        lbl_8055E628->Final(lbl_8055E624->r, lbl_8055E624->n);
    }
    fn_801AE79C(mode);
    if (mode == 1) {
        for (int i = 0; i < t->n; i++) {
            int idx = t->order[i];
            fn_801AF9EC(idx, 0);
            fn_801B3208(idx, i);
        }
    }
}

RaceResult* fn_801AF8D0(int i) {
    RaceResult* r = 0;
    if (i < lbl_8055E624->n) {
        r = &lbl_8055E624->r[i];
        if (!fn_801AE620(r)) {
            r = 0;
        }
    }
    return r;
}

struct StandingRow {
    u8 pad0[0x4];
    s32 place;  // offset 0x4
    s32 x8;     // offset 0x8
    u8 padC[0x4];
    u8 x10;     // offset 0x10
};

struct Standings {
    s32 n;     // offset 0x0
    s32 x4;    // offset 0x4
    u8 pad8[0x4];
    u8 xC;     // offset 0xC
    u8 xD;     // offset 0xD
    u8 padE[0x2];
    StandingRow rows[8];  // offset 0x10
};

void fn_801AF92C(Standings* s) {
    if (fn_801B8728(s) == 0 && fn_801B2C70(s) == 0) {
    StandingRow* row;
    int n = lbl_80435780.n;
    s->n = n;
    s->x4 = 0;
    s->xC = 0;
    s->xD = 1;
    row = s->rows;
    for (int i = 0; i < n; row++, i++) {
        RaceResult* r = fn_801AF8D0(i);
        if (r) {
            row->place = r->place;
            row->x8 = 0;
            row->x10 = 0;
        } else {
            row->place = -1;
            row->x8 = 0;
            row->x10 = 0;
        }
    }
    }
}

f32 fn_801AF9EC(int i, f32* b) {
    if (b) {
        *b = lbl_80435740[i].b;
    }
    return lbl_80435740[i].a;
}

RecordMgr* fn_801AFA20(RaceSetup* s) { return new ("RecordMgr") RecordMgr(-1); }

void fn_801AFA5C(RecordMgr* m) { delete m; }

}  // extern "C"

RecordMgr::RecordMgr(int a) { x0 = 0; }

RecordMgr::~RecordMgr(void) {}

void RecordMgr::Laps(LapResult* laps, int n) {}

void RecordMgr::Results(RaceResult* r, int n) {}

void RecordMgr::Final(RaceResult* r, int n) {}

void RecordMgr::Bonus(int place) {}
