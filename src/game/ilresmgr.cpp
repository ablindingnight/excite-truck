// Racing line resource manager (0x801561E8-0x80157278): loads the event's centre line ("<line_basename>track.ild")
// and either a single ghost line or a "racingline_set" of lines with per-car switching between them, inside
// "legal_transition_regions", every min_change_t..max_change_t seconds. Names are guesses apart from the strings.
#include "game/common.h"

#define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))

ModuleTag lbl_8055E270;
UnkStaticFFFF lbl_8055E278;

struct IdealLineRes;
struct Region;
struct EventInfo {
    u8 pad0[0x20];
    ValueNode* vals;  // offset 0x20
};

struct Strength {
    s32 event;  // offset 0x0
};

struct RacerInfo {
    u8 pad0[0x34];
    f32 side;  // offset 0x34
};

class Racer {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual RacerInfo* GetInfo(void);  // 0x10
};

struct RaceCarRec {
    u8 pad0[0x60];
    f32 pos[3];  // offset 0x60
};

struct V2 {
    f32 x;
    f32 y;
};

EventInfo* GetEvent(int event);

extern "C" {
char* strrchr(const char* s, int c);
int fn_801345D8(char* buf, u32 size, const char* fmt, ...);  // snprintf
void fn_802E30C8(const char* fmt, ...);                      // debug print
void fn_802E317C(const char* fmt, ...);                      // fatal: print, then exit() -> PPCHalt (never returns)
void fn_8012E81C(const char* res);
Strength* fn_801E87A8(void);
const char* fn_801E879C(void);  // track name
IdealLineRes* fn_80138968(const char* name);
void fn_80138AD8(IdealLineRes** pr);
void fn_80138BA0(IdealLineRes* a, IdealLineRes* b);
Region* fn_801399FC(ValueNode* n);
int fn_80139B70(Region* r, f32 x, f32 z);
void fn_80128658(Region* r);
int fn_80129D90(int seed, int lo, int hi);  // random int
f32 fn_80129DC4(int seed, f32 lo, f32 hi);  // random float
RaceCarRec* fn_8017960C(int id);
f32 fn_801B306C(void);
int fn_801B2F24(void);
Racer* fn_801B2ED0(int i);
V2* fn_80135420(V2* p, int a, IdealLineRes* line);  // closest point on a line
}

void* operator new(size_t size, const char* tag);

extern f32 lbl_8055E3C8;  // race clock

static inline void StrCopyN(char* d, int n, const char* s) {
    char* p = d;
    int k = n;
    const char* q = s;
    while ((*p++ = *q++) != 0) {
        if (--k <= 0) {
            p[-1] = 0;
            break;
        }
    }
}

static inline void StrCatN(char* d, int n, const char* s) {
    char* p = d;
    int k = n;
    while (*p != 0) {
        k--;
        p++;
    }
    if (k > 0) {
        const char* q = s;
        while ((*p++ = *q++) != 0) {
            if (--k <= 0) {
                p[-1] = 0;
                break;
            }
        }
    }
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
    int Full(void) const { return count + 1 > cap; }
    T* Add(void) { return &data[count++]; }
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
};

template <class T, int N>
struct FixedList {
    FixedList(void) : a(storage, N) {}
    ArrayRef<T> a;
    T storage[N];
};

extern "C" {

void fn_801561E8(char* s) {
    char* p = strrchr(s, '.');
    if (p) {
        *p = 0;
    }
}

ValueNode* fn_8015621C(void) { return GetEvent(fn_801E87A8()->event)->vals; }

char* fn_80156248(char* buf, int size) {
    const char* base = fn_8015621C()->GetString("line_basename", "");
    fn_801345D8(buf, size, "%strack.ild", base);
    return buf;
}

char* fn_801562BC(char* buf, int size, const char* name, int ghost) {
    char track[0x20];
    StrCopyN(track, sizeof(track), fn_801E879C());
    fn_801561E8(track);
    if (ghost) {
        fn_801345D8(buf, size, "*ghostcar.res/%s%s%s.il0", name, *name ? "_" : "", track);
    } else {
        fn_801345D8(buf, size, "%s.ili", *name ? name : track);
    }
    return buf;
}

}  // extern "C"

// The set of lines the AI follows.
class PathSet {
public:
    virtual IdealLineRes* GetLine(int i) = 0;              // 0x08 centre line
    virtual IdealLineRes* GetRacingLine(int i) = 0;        // 0x0C
    virtual IdealLineRes* CenterFor(int car) = 0;          // 0x10
    virtual IdealLineRes* LineFor(int car) = 0;            // 0x14
    virtual void Snap(int car, const f32* pos) = 0;        // 0x18
    virtual void Switch(int car) = 0;                      // 0x1C
    virtual void Reset(void) {}                            // 0x20
    virtual void Assign(void) = 0;                         // 0x24
    virtual ~PathSet(void) {}                              // 0x28
};

static inline int RightSide(Racer* r) {
    if (r->GetInfo()->side > 0.0f) {
        return 1;
    }
    return 0;
}

// One source of lines (centre line, one racing line or a set of them).
class LineSource {
public:
    virtual ~LineSource(void) {}                           // 0x08
    virtual IdealLineRes* GetLine(int i) = 0;              // 0x0C
    virtual IdealLineRes* Get(int car) = 0;                // 0x10
    virtual void Reset(void) = 0;                          // 0x14
    virtual void Snap(int car, const f32* pos) {}          // 0x18
    virtual void Switch(int car) {}                        // 0x1C
    virtual void Assign(void) {}                           // 0x20
};

extern "C" IdealLineRes* fn_801563BC(const char* name, LineSource* center);

class CenterLine;
static inline void InitCenter(CenterLine* c);

class CenterLine : public LineSource {
public:
    CenterLine(void) { InitCenter(this); }
    virtual ~CenterLine(void) { fn_80138AD8(&line); }
    virtual IdealLineRes* GetLine(int i) { return line; }
    virtual IdealLineRes* Get(int car) { return line; }
    virtual void Reset(void) {}

    IdealLineRes* line;  // offset 0x4
};

class SingleLine;
static inline void InitSingle(SingleLine* s, LineSource* center);

class SingleLine : public LineSource {
public:
    SingleLine(LineSource* center) { InitSingle(this, center); }
    virtual ~SingleLine(void) { fn_80138AD8(&line); }
    virtual IdealLineRes* GetLine(int i) {
        if (i == 0) {
            return line;
        }
        return 0;
    }
    virtual IdealLineRes* Get(int car) { return line; }
    virtual void Reset(void) {}

    IdealLineRes* line;  // offset 0x4
};

struct LineEntry {
    IdealLineRes* line;  // offset 0x0
    s32 side;            // offset 0x4 (0 left, 1 right, 2 either)
};

struct CarLine {
    f32 next;            // offset 0x0 (race time of the next change)
    IdealLineRes* line;  // offset 0x4
};

class LineSet;
static inline void InitSet(LineSet* s, ValueNode* set, LineSource* center);

class LineSet : public LineSource {
public:
    LineSet(ValueNode* set, LineSource* center) { InitSet(this, set, center); }
    virtual ~LineSet(void) {
        int i;
        for (i = 0; i < lines.a.count; i++) {
            fn_80138AD8(&lines.a.data[i].line);
        }
        lines.a.count = 0;
        for (i = 0; i < regions.a.count; i++) {
            fn_80128658(regions.a.data[i]);
        }
        regions.a.count = 0;
    }
    virtual IdealLineRes* GetLine(int i) {
        if (i < lines.a.count) {
            return lines.a.data[i].line;
        }
        return 0;
    }
    virtual IdealLineRes* Get(int car) {
        CarLine* c = &cars[car];
        f32 now = lbl_8055E3C8;
        if (now >= c->next) {
            if (InRegion(fn_8017960C(car))) {
                int k = fn_80129D90(4, 0, lines.a.count);
                IdealLineRes* l = lines.a.data[k].line;
                if (!l) {
                    l = lines.a.data[(k + 1) % lines.a.count].line;
                }
                c->next = now + fn_80129DC4(4, minT, maxT);
                cars[car].line = l;
            } else {
                c->next += 1.0f;
            }
        }
        return fn_801B306C() > 0.0f ? cars[0].line : cars[car].line;
    }
    virtual void Reset(void);
    virtual void Snap(int car, const f32* pos) {
        V2 p;
        f32 best;
        int bi;
        int i;
        p.x = pos[0];
        p.y = pos[2];
        best = 2.5e7f;
        bi = -1;
        for (i = 0; i < lines.a.count; i++) {
            V2* q = fn_80135420(&p, 0, lines.a.data[i].line);
            V2 d;
            f32 dd;
            d.y = p.y - q->y;
            d.x = p.x - q->x;
            dd = d.x * d.x + d.y * d.y;
            if (dd < best) {
                bi = i;
                best = dd;
            }
        }
        {
            IdealLineRes* l = lines.a.data[bi].line;
            f32 now = lbl_8055E3C8;
            cars[car].next = now + fn_80129DC4(4, minT, maxT);
            cars[car].line = l;
        }
    }
    virtual void Switch(int car) {
        IdealLineRes* l;
        IdealLineRes* cur;
        f32 now = lbl_8055E3C8;
        cur = cars[car].line;
        int k = fn_80129D90(4, 0, lines.a.count);
        l = lines.a.data[k].line;
        if (l == cur) {
            l = lines.a.data[(k + 1) % lines.a.count].line;
        }
        cars[car].next = now + fn_80129DC4(4, minT, maxT);
        cars[car].line = l;
    }
    virtual void Assign(void) {
        int n = fn_801B2F24();
        f32 now = lbl_8055E3C8;
        int i;
        for (i = 0; i < n; i++) {
            FixedList<IdealLineRes*, 8> cand;
            int right;
            if (fn_801B2ED0(i)->GetInfo()->side > 0.0f) {
                right = 1;
            } else {
                right = 0;
            }
            int j;
            for (j = 0; j < lines.a.count && !cand.a.Full(); j++) {
                LineEntry* e = &lines.a.data[j];
                if (right == e->side) {
                    *cand.a.Add() = e->line;
                }
            }
            if (cand.a.count > 0) {
                IdealLineRes* l = cand.a.data[i % cand.a.count];
                cars[i].next = now + fn_80129DC4(4, minT, maxT);
                cars[i].line = l;
            }
        }
    }

    int InRegion(RaceCarRec* rc) {
        int i;
        for (i = 0; i < regions.a.count; i++) {
            if (fn_80139B70(regions.a.data[i], rc->pos[0], rc->pos[2])) {
                return 1;
            }
        }
        return regions.a.count == 0;
    }

    FixedList<LineEntry, 16> lines;  // offset 0x4
    FixedList<Region*, 16> regions;  // offset 0x90
    f32 minT;                        // offset 0xDC
    f32 maxT;                        // offset 0xE0
    CarLine cars[8];                 // offset 0xE4
};

// The manager behind fn_80156A40: forwards to the centre line and the racing line source.
class RacingLines : public PathSet {
public:
    RacingLines(LineSource* r, LineSource* c) {
        center = c;
        racing = r;
    }
    virtual IdealLineRes* GetLine(int i) { return center->GetLine(i); }
    virtual IdealLineRes* GetRacingLine(int i) { return racing->GetLine(i); }
    virtual IdealLineRes* CenterFor(int car) { return center->Get(car); }
    virtual IdealLineRes* LineFor(int car) { return racing->Get(car); }
    virtual void Snap(int car, const f32* pos) { racing->Snap(car, pos); }
    virtual void Switch(int car) { racing->Switch(car); }
    virtual void Reset(void) {
        center->Reset();
        racing->Reset();
    }
    virtual void Assign(void) { racing->Assign(); }
    virtual ~RacingLines(void) {
        delete center;
        delete racing;
    }

    LineSource* racing;  // offset 0x4
    LineSource* center;  // offset 0x8
};

static PathSet* lbl_8055E27C;

extern "C" {

IdealLineRes* fn_801563BC(const char* name, LineSource* center) {
    char buf[0x40];
    IdealLineRes* r;
    fn_801562BC(buf, sizeof(buf), name, 0);
    r = fn_80138968(buf);
    if (!r) {
        fn_802E30C8("ilresmgr: Can't load racingline \"%s\", using ghostcar.res", buf);
        fn_801562BC(buf, sizeof(buf), name, 1);
        r = fn_80138968(buf);
    }
    if (!r) {
        fn_802E30C8("ilresmgr: Can't load racingline \"%s\", using centerline", buf);
        r = fn_80138968(fn_80156248(buf, sizeof(buf)));
    } else {
        fn_80138BA0(r, center->GetLine(0));
    }
    return r;
}

}  // extern "C"

static inline void InitCenter(CenterLine* c) {
    char buf[0x40];
    c->line = fn_80138968(fn_80156248(buf, sizeof(buf)));
    if (!c->line) {
        fn_802E317C("ilresmgr: Can't load \"%s\"", buf);
    }
}

static inline void InitSingle(SingleLine* s, LineSource* center) {
    ValueNode* v = fn_8015621C();
    s->line = fn_801563BC(v->GetString("ghostline_basename", v->GetString("line_basename", "")), center);
}

static inline void InitSet(LineSet* s, ValueNode* set, LineSource* center) {
    const char* prefix = "line";
    int i;
    for (i = 1;; i++) {
        char key[0x10];
        char sideKey[0x20];
        const char* name;
        LineEntry* e;
        const char* side;
        fn_801345D8(key, sizeof(key), "%s%d", prefix, i);
        name = set->GetString(key, 0);
        if (!name || s->lines.a.Full()) {
            break;
        }
        e = s->lines.a.Add();
        e->line = fn_801563BC(name, center);
        StrCopyN(sideKey, sizeof(sideKey), key);
        StrCatN(sideKey, sizeof(sideKey), "_side");
        side = set->GetString(sideKey, "");
        {
            int sd;
            switch (*side) {
            case 'r':
                sd = 1;
                break;
            case 'l':
                sd = 0;
                break;
            default:
                sd = 2;
                break;
            }
            e->side = sd;
        }
    }
    if (s->lines.a.count == 0) {
        fn_802E317C("ilresmgr: I expected at least \"line1\" in the racing line set");
    }
    {
        ValueNode* regs = set->FindChild("legal_transition_regions");
        if (regs) {
            ValueNode* r;
            for (i = 0; (r = regs->GetChildAt(i)) != 0 && !s->regions.a.Full(); i++) {
                s->regions.a.data[s->regions.a.count++] = fn_801399FC(r);
            }
        }
    }
    s->minT = set->GetFloat("min_change_t", 15.0f);
    s->maxT = set->GetFloat("max_change_t", 60.0f);
    fn_802E30C8("ilresmgr: %d line%s, %d region%s, on (%.1f->%.1f) cycle", s->lines.a.count,
                s->lines.a.count == 1 ? "" : "s", s->regions.a.count, s->regions.a.count == 1 ? "" : "s", s->minT, s->maxT);
    s->Reset();
}

extern "C" {

void fn_801564B0(void) {
    CenterLine* center;
    LineSource* racing;
    ValueNode* set;
    fn_8012E81C("ghostcar.res");
    center = new ("clp") CenterLine;
    set = fn_8015621C()->FindChild("racingline_set");
    if (set) {
        racing = new ("rlp") LineSet(set, center);
    } else {
        racing = new ("rlp") SingleLine(center);
    }
    lbl_8055E27C = new ("ilresmgr") RacingLines(racing, center);
}

}  // extern "C"

void LineSet::Reset(void) {
    IdealLineRes* l;
    f32 now = lbl_8055E3C8;
    int i;
    for (i = 0; i < ARRAY_COUNT(cars); i++) {
        l = lines.a.data[i % lines.a.count].line;
        cars[i].next = now + fn_80129DC4(4, minT, maxT);
        cars[i].line = l;
    }
}

extern "C" {

void fn_801569FC(void) {
    delete lbl_8055E27C;
    lbl_8055E27C = 0;
}

PathSet* fn_80156A40(void) { return lbl_8055E27C; }

}  // extern "C"
