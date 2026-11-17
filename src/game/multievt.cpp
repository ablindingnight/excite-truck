// Multiplayer event menu (0x8024958C-0x8024BE04, "multi_evt.lyt" / "multi_country.lyt" /
// "multi_course.lyt"): choosing the location, course, event and difficulty for a multiplayer race,
// remembering the last choices ("last_mp_event_count", ...), and the stats table beside the course list.
// The pickers and pages are real classes (names unproven): their vtables, the inline members emitted after
// fn_8024974C and at the end of the unit, and the two this-adjust thunk pairs (0x8024BDDC..) are the compiler's.
#include "game/common.h"
#include "game/gamedb.h"
#include "game/menu.h"
#include "game/menuwidget_inl.h"
#include "game/menustate.h"
#include "game/uitable.h"
#include <string.h>
#include "game/strcopy.h"

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

const char* lbl_8055BF10 = "last_mp_event_count";
const char* lbl_8055BF14 = "last_mp_eve";
const char* lbl_8055BF18 = "last_mp_difficulty";
char lbl_8055BF1C[] = "Easy";
char lbl_8055BF24[] = "Medium";
char lbl_8055BF2C[] = "Hard";

// A set of named textures, one of which is drawn at a time (gart.cpp).
#include "game/dynotex.h"

// The course stats table (mstats.cpp).
#include "game/statstable.h"

struct NamedValue {
    const char* name;
    s32 value;
};

struct NameEntry {
    s32 id;
    const char* name;
};


class VN {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual const char* v14(const char* key);
};

extern "C" {
int strcasecmp(const char* a, const char* b);
u32 strlen(const char* s);
int strncmp(const char* a, const char* b, u32 n);
int fn_801345D8(char* buf, int n, const char* fmt, ...);
void fn_8023E52C(u32 a);
s32 fn_80214D04(void);
void fn_80214CB4(s32 a);
int fn_802411D4(void);
int fn_8024974C(s32* track, s32* eve, s32* diff, int testing);
void fn_80214AF0(int s);
void fn_80214A9C(int s);
void fn_80214B44(int i);
}

void operator delete(void* p);

// The difficulty names (Difficulty picker page and the replay movie path).
extern const NameEntry lbl_802FA4B0[3] = {{0, lbl_8055BF1C}, {1, lbl_8055BF24}, {2, lbl_8055BF2C}};
extern char lbl_804989AC[0x24];  // defined after lbl_80498998 (.bss order)

// Per-car multiplayer unlock flags.
struct UnlockEntry {
    s32 id;            // offset 0x0
    const char* name;  // offset 0x4
    u8 pad8[0xC];      // offset 0x8
    u8 unlocked;     // offset 0x14
    u8 pad15[0x3];   // offset 0x15
};

struct UnlockList {
    UnlockEntry* entries;  // offset 0x0
    s32 count;             // offset 0x4
};

extern UnlockList* lbl_8055F28C;

static inline int FindUnlock(EventInfo* ev, const UnlockList* list) {
    for (int i = 0; i < list->count; i++) {
        s32 id = list->entries[i].id;
        if (id == ev->id) {
            return i;
        }
    }
    return -1;
}

extern u8 lbl_8055F290;
extern char* lbl_8055F294;

// Copies at most n characters, always terminating.

ModuleTag lbl_8055F280;
UnkStatic123s lbl_80498998;
char lbl_804989AC[0x24];
UnkStatic7F lbl_8055F288;
UnlockList* lbl_8055F28C;
u8 lbl_8055F290;
u8 lbl_8055F291;
char* lbl_8055F294;
u8 lbl_8055F298;
s32 lbl_8055BF34 = -1;

extern "C" {

// fn_802494F8..fn_80249588 are popwarn.cpp's ValueLabel/PopWarnValue inline methods (moved 2026-11-02).


/* Helper name unproven */
static inline int FindUnlockByOffset(EventInfo* ev) {
    UnlockList* l = lbl_8055F28C;
    unsigned offset;
    int i;
    for (i = 0, offset = 0; i < l->count; offset += sizeof(UnlockEntry), i++) {
        UnlockEntry* entry = (UnlockEntry*)((u8*)l->entries + offset);
        if (entry->id == ev->id) {
            return i;
        }
    }
    return -1;
}

int fn_8024958C(EventInfo* ev, const char* track) {
    u32 ok = ev->type == 0x180302;
    if (ok) {
        ok = FindUnlock(ev, lbl_8055F28C) != -1;
    }
    if (ok) {
        ok = lbl_8055F28C->entries[FindUnlockByOffset(ev)].unlocked;
    }
    return ok;
}

int fn_80249660(EventInfo* ev, const char* track) { return 1; }

}  // extern "C"

const char* GetTrackEnviron(const char* track);
const char* GetEventDisplayName(int ev);

struct EvPair {
    const char* track;  // offset 0x0
    EventInfo* ev;      // offset 0x4
};

// The event/track pairs the pickers choose from.
struct EvPairs {
    EvPairs();
    ~EvPairs() { items = 0; count = 0; cap = 0; }
    void Trim();
    EvPair* items;  // offset 0x0
    s32 count;      // offset 0x4
    s32 cap;        // offset 0x8
    EvPair* alloc;  // offset 0xC
};

/* Class name unproven */
// Owns the event pairs for one menu run.
struct EvPairsRef {
    EvPairsRef(EvPairs* q) : p(q) {}
    ~EvPairsRef(void) {
        EvPairs* q = p;
        if (q) {
            delete[] q->alloc;
            delete q;
            p = 0;
        }
    }
    EvPairs* p;
};

// One course row of the picker.
struct EvPick {
    s32 track;         // offset 0x0
    s32 eve;           // offset 0x4
    const char* name;  // offset 0x8
};

// A fixed-capacity list over inline storage. Its destructor (and its own base's) clears the header.
#include "game/vecref.h"

/* Class name unproven */
template <class T, int N>
struct FixedVec : VecRef<T> {
    FixedVec(void) {
        this->data = buf;
        this->cap = N;
        this->count = 0;
    }
    T buf[N];  // offset 0xC
};

// The preview movie of a track environment.
inline void MoviePath(char* buf, const char* env);

/* Class name unproven */
// Event picker page: the courses of the chosen location.
class EvPicker : public MenuWidget {
public:
    EvPicker(void) { f18 = -1; }
    virtual ~EvPicker(void) {
        f18 = -1;
        cur = 0;
        env = 0;
    }
    virtual const char* Current(void) { return ents[cur].name; }
    virtual void Prev(void) {
        s32 n = count;
        cur = (n + cur - 1) % n;
    }
    virtual void Next(void) { cur = (cur + 1) % count; }
    virtual void SetEnv(EvPairs** pairs, const char* env);  // 0x30
    virtual void Select(s32 track, s32 eve);               // 0x34
    virtual s32 Get(s32* track, s32* eve) {                 // 0x38
        EvPick* q = &ents[cur];
        s32 v1 = q->track;
        s32 v2 = q->eve;
        *track = v1;
        *eve = v2;
        return v1;
    }

    s32 cur;           // offset 0x10
    s32 count;         // offset 0x14
    s32 f18;           // offset 0x18
    const char* env;   // offset 0x1C
    EvPick ents[64];   // offset 0x20
};

int GetNumTracks(void);
int GetNumEvents(void);
const char* GetTrackFriendlyName(const char* name);

struct EvEntryBuf {
    s32 id;         // offset 0x0
    u8 pad4[0x14];  // offset 0x4
};

// An event list with room for 0x80 entries on the stack.
struct EvListBuf {
    EvListBuf(void) {
        items = buf;
        cap = 0x80;
        count = 0;
    }
    ~EvListBuf(void) {
        items = 0;
        count = 0;
        cap = 0;
    }
    int Find(s32 id) {
        for (int i = 0; i < count; i++) {
            EvEntryBuf& e = items[i];
            if (e.id == id) {
                return i;
            }
        }
        return -1;
    }
    EvEntryBuf* items;      // offset 0x0
    s32 count;              // offset 0x4
    s32 cap;                // offset 0x8
    EvEntryBuf buf[0x80];   // offset 0xC
};

// Location picker page: one row per track environment.
struct LocEntry {
    const char* name;  // offset 0x0, friendly name
    const char* env;   // offset 0x4
};

/* Class name unproven */
class LocPicker : public MenuWidget {
public:
    LocPicker(EvPairs** p, EvPicker* e) {
        pairs = p;
        events = e;
        cur = 0;
    }
    virtual ~LocPicker(void) { cur = 0; }
    virtual const char* Current(void) { return list.data[cur].name; }
    virtual void Prev(void) {
        cur = (list.count + cur - 1) % list.count;
        events->SetEnv(pairs, list.data[cur].env);
        MoviePath(movie, list.data[cur].env);
    }
    virtual void Next(void) {
        cur = (cur + 1) % list.count;
        events->SetEnv(pairs, list.data[cur].env);
        MoviePath(movie, list.data[cur].env);
    }

    FixedVec<LocEntry, 0x20> list;  // offset 0x10
    s32 cur;                        // offset 0x11C
    EvPairs** pairs;                // offset 0x120
    EvPicker* events;               // offset 0x124
    char movie[0x20];               // offset 0x128
};

/* Class name unproven */
// Difficulty picker page.
class DiffPicker : public MenuWidget {
public:
    DiffPicker(void) { cur = 0; }
    virtual ~DiffPicker(void) {}
    virtual void Prev(void) {
        s32 v = cur - 1;
        cur = v > 0 ? v : 0;
    }
    virtual void Next(void) {
        s32 v = cur + 1;
        cur = v > 2 ? 2 : v;
    }
    virtual const char* Current(void) { return lbl_802FA4B0[cur].name; }

    s32 cur;  // offset 0x10
};

/* Class name unproven */
// Stats page: the StatsTable of the chosen course.
class StatsPage : public StatsTable, public MenuState {
public:
    EvPicker* picker;  // offset 0x4C
    StatsPage(EvPicker* p) : picker(p) { text[0] = 0; }
    virtual ~StatsPage(void) {}
    virtual void Update(void) {
        s32 a;
        s32 b;
        picker->Get(&a, &b);
        Show(FindTrackEvent(a, b)->id);
    }
    s32 u54;           // offset 0x54
    char text[0x100];  // offset 0x58
};

/* Class name unproven */
// Course texture page: a DynoTex of the event pictures plus the chosen course.
class CourseTex : public DynoTex, public MenuState {
public:
    CourseTex(int n) : DynoTex(n, 0) {}
    virtual ~CourseTex(void) {}
    virtual void Enter(void) { Pick(); }
    virtual void Update(void) { Pick(); }
    // Takes the chosen course from the event picker.
    void Pick(void) {
        EvPicker* c = picker;
        if (c) {
            s32 track;
            s32 eve;
            c->Get(&track, &eve);
            EventInfo* ev = FindTrackEvent(track, eve);
            id = ev->id;
            const char* name_ = ((VN*)ev->node)->v14("friendly_name");
            SafeStrCopy(name, name_, 100);
        } else {
            id = -1;
            name[0] = 0;
        }
    }

    EvPicker* picker;  // offset 0x30
    s32 id;            // offset 0x34
    char name[100];    // offset 0x38
};

struct EvEntry {
    s32 id;             // offset 0x0
    const char* name;   // offset 0x4
    s32 slot;           // offset 0x8
    const char* group;  // offset 0xC, country/location key
    const char* name2;  // offset 0x10
    u8 unlocked;        // offset 0x14
    u8 flag;            // offset 0x15
    u8 pad16[0x2];
};

struct EvList {
    EvEntry* items;  // offset 0x0
    s32 count;       // offset 0x4
};

// One course row on the course list.
struct CourseSlot {
    char name[50];   // offset 0x0
    char name2[50];  // offset 0x32
    s32 open;        // offset 0x64
};

// One location on the location list.
struct GroupEntry {
    const char* name;  // offset 0x0
    u8 flag;           // offset 0x4
    u8 pad5[0x3];
};

/* Class name unproven */
// Location (country) screen of the multiplayer course choice ("multi_country.lyt").
class CountryScreen : public MenuState {
public:
    CountryScreen(EvList* l);
    virtual ~CountryScreen(void) {}
    virtual void Enter(void);
    virtual void Update(void);
    void Refresh(void);
    void Select(const char* name);
    const char* Name(int i);
    char* Course(int i);
    char* Cup(int i);
    s32* Lock(int i);

    FixedVec<GroupEntry, 10> groups;  // offset 0x18
    EvList* list;                     // offset 0x74
    CourseSlot slots[5];              // offset 0x78
    s32 sel;                          // offset 0x280
    u8 anyNew;                        // offset 0x284
};

/* Class name unproven */
// Course screen of one location ("multi_course.lyt").
class CourseScreen : public MenuState {
public:
    CourseScreen(const char* group, EvList* l);
    virtual ~CourseScreen(void) {}
    virtual void Enter(void);
    virtual void Update(void);
    void Refresh(void);
    void Select(s32 id);
    char* Name(void);
    const char* CourseName(int i);
    s32 CourseId(int i);
    DynoTex* Tex(void);
    s32* IdPtr(void);

    FixedVec<EvEntry*, 10> events;  // offset 0x18
    DynoTex tex;                    // offset 0x4C
    char name[50];                  // offset 0x64
    s32 id;                         // offset 0x98
    s32 sel;                        // offset 0x9C
};

inline void MoviePath(char* buf, const char* env) { fn_801345D8(buf, 0x20, "%s/race.%s", env, "m2v"); }

extern u8 lbl_8055F291;
extern "C" char lbl_8055B860[8];
extern char lbl_8055BAC0[8];
extern char lbl_8055BABC[4];
extern char lbl_8055BAB8[4];
void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

extern "C" {
void fn_801332D0(int slot, const char* name, s32* value);
void fn_80133348(int slot, const char* name, s32 value);
void fn_8027BCD0(EvListBuf* list);
void fn_801286A0(void* p, s32 shrink);
void fn_80265DCC(void* base, int n, int size, int (*cmp)(u8*, u8*));
int fn_802146BC(MenuDesc* desc, int x, int y, int modal);
void fn_802E317C(const char* fmt, ...);
void fn_80236914(int which, f32* out, f32 t);
int fn_8024A4B8(const char** a, const char** b);
}

// Stripped from the shipped game; only its message survives in the unit's .data.
// MATCHING-AID: reconstructed so the literal pool keeps the original offsets (see docs/matching_aids.md).
void MultiEvtTestingCheck(void) {
    if (!fn_802411D4()) {
        fn_802E317C("Testing mode only allowed from Single Race path");
    }
}

extern "C" {

int fn_80249668(s32* out) {
    const char* names[3] = {"Select Location", "Select Event", "Select Difficulty"};
    const char* src = names[fn_80214D04()];
    SafeStrCopy(lbl_8055F294, src, 0x100);
    if (lbl_8055F290 && !fn_802411D4()) {
        *out = -1;
        return 1;
    }
    return 0;
}

void fn_80249720(void) { lbl_8055BF34 = fn_80214D04(); }

void fn_80249744(void) { fn_80214CB4(lbl_8055BF34); }

// The multiplayer event menu: location, course, event count and AI strength, with the course picture,
// preview movie and stats. Starts from the remembered choices and remembers the new ones; returns 0 when
// backed out. In testing mode only the unlocked multiplayer events are offered.
inline EvPairs::EvPairs() {
    int n = GetNumEvents();
    EvPair* b = new ("dsa_buf") EvPair[n];
    items = b;
    cap = n;
    count = 0;
    alloc = b;
}

inline void EvPairs::Trim() {
    if (count < cap) {
        fn_801286A0(alloc, (cap - count) * sizeof(EvPair));
        cap = count;
    }
}

int fn_8024974C(s32* track, s32* eve, s32* diff, int testing) {
    if (testing && !fn_802411D4()) {
        return 0;
    }
    lbl_8055F290 = testing;
    lbl_8055F28C = (UnlockList*)new ("sa") EvListBuf;
    fn_8027BCD0((EvListBuf*)lbl_8055F28C);
    lbl_8055F291 = 0;
    *track = -1;
    *eve = 5;
    *diff = 0;
    fn_801332D0(0, lbl_8055BF10, eve);
    fn_801332D0(0, lbl_8055BF14, track);
    EvPairsRef pairs(new ("ci-list") EvPairs);
    for (int i = 0; i < GetNumEvents(); i++) {
        EventInfo* ev = GetEvent(i);
        const char* name = GetTrack(ev->track)->name;
        if ((testing ? fn_8024958C : fn_80249660)(ev, name)) {
            EvPair* d = pairs.p->items;
            int k = pairs.p->count;
            pairs.p->count = k + 1;
            d[k].ev = ev;
            d[k].track = name;
        }
    }
    pairs.p->Trim();
    EvPicker events;
    s32 lastEve = *eve;
    s32 lastTrack = *track;
    LocPicker loc(&pairs.p, &events);
    for (int i = 0; i < (*loc.pairs)->count; i++) {
        const char* name = (*loc.pairs)->items[i].track;
        const char* env = GetTrackEnviron(name);
        int found = 0;
        for (int j = 0; j < loc.list.count; j++) {
            if (!strcasecmp(loc.list.data[j].env, env)) {
                found = 1;
                break;
            }
        }
        if (!found) {
            LocEntry* e = &loc.list.data[loc.list.count++];
            e->name = GetTrackFriendlyName(name);
            e->env = env;
        }
    }
    fn_80265DCC(loc.list.data, loc.list.count, sizeof(LocEntry), (int (*)(u8*, u8*))fn_8024A4B8);
    if (lastTrack >= 0 && lastTrack < GetNumTracks()) {
        const char* env = GetTrackEnviron(GetTrack(lastTrack)->name);
        for (int j = 0; j < loc.list.count; j++) {
            if (!strcasecmp(env, loc.list.data[j].env)) {
                loc.cur = j;
                break;
            }
        }
    }
    loc.events->SetEnv(loc.pairs, loc.list.data[loc.cur].env);
    loc.events->Select(lastTrack, lastEve);
    MoviePath(loc.movie, loc.list.data[loc.cur].env);
    UnlockList* unlocks = lbl_8055F28C;
    CourseTex tex(unlocks->count);
    for (int i = 0; i < unlocks->count; i++) {
        char buf[0x21];
        const char* name = ((VN*)GetEvent(unlocks->entries[i].id)->node)->v14("name");
        const char* prefix = "career_";
        if (!strncmp(name, prefix, strlen(prefix))) {
            name += strlen(prefix);
        }
        fn_801345D8(buf, 0x21, "/%s.tex", name);
        tex.Add(unlocks->entries[i].id, buf);
    }
    tex.picker = &events;
    tex.id = -1;
    tex.name[0] = 0;
    if (!testing) {
        fn_801332D0(0, lbl_8055BF18, diff);
    } else {
        *diff = 0;
    }
    DiffPicker level;
    for (int i = 0; i < 3; i++) {
        if (*diff == lbl_802FA4B0[i].id) {
            level.cur = i;
            break;
        }
    }
    StatsPage stats(&events);
    lbl_8055F294 = stats.text;
    MenuItem items[] = {
        MenuItem(lbl_8055BAC0, 8, (const void*)fn_80249744, 0, 0),
        MenuItem(lbl_8055BABC, 8, (const void*)fn_80249720, 1, 0),
        MenuItem("#track", 0x10, &loc, 0, 0),
        MenuItem("#event_count", 0x10, &events, 0, 0),
        MenuItem("#ai_strength", 0x10, &level, 0, 0),
        MenuItem(lbl_8055B860, 6, 0, -1, 27),
        MenuItem(lbl_8055B860, 6, 0, -2, 13),
        MenuItem("#title", stats.text, CaptionContext(0x100)),
        MenuItem("#course_name", tex.name, (int)NoCaption()),
        MenuItem("#trackmap", 0x12, &tex, 0, 0),
        MenuItem("#trackmap", 2, &tex.id, 0, 0),
        MenuItem(0, 5, (MenuState*)&tex, 0, 0),
        MenuItem("#track_movie", loc.movie, CaptionContext(0x20)),
        MenuItem("Stat", 0xD, &stats, 0, 0),
        MenuItem("Value", 0xD, &stats, 1, 0),
        MenuItem(0, 5, (MenuState*)&stats, 0, 0),
        MenuItem(lbl_8055BAB8, 8, (const void*)fn_80236914, 2, 0),
        MenuItem(lbl_80498998),
    };
    MenuDesc desc = {testing ? "multi_evt.lyt" : "track1.lyt", (MenuItem*)items, (int (*)(int*))fn_80249668, 0, 0};
    int r = fn_802146BC(&desc, -999, -999, 3);
    *track = events.ents[events.cur].track;
    *eve = events.ents[events.cur].eve;
    *diff = lbl_802FA4B0[level.cur].id;
    fn_80133348(0, lbl_8055BF14, *track);
    fn_80133348(0, lbl_8055BF10, *eve);
    fn_80133348(0, lbl_8055BF18, *diff);
    lbl_8055F294 = 0;
    delete (EvListBuf*)lbl_8055F28C;
    lbl_8055F28C = 0;
    if (r == -1) {
        return 0;
    }
    return 1;
}

}  // extern "C"

inline void EvPicker::Select(s32 track, s32 eve) {
    for (int i = 0; i < count; i++) {
        if (track == ents[i].track && eve == ents[i].eve) {
            cur = i;
            return;
        }
    }
}


// The name an event is listed under. The u8* parameter is retail's (the string label is
// @STRING@EventFriendlyName__FPUc).
inline const char* EventFriendlyName(u8* ev) {
    return ((EventInfo*)ev)->node->GetString("friendly_name", GetEventDisplayName(((EventInfo*)ev)->id));
}

// The event's index within its track, or -1.
static inline int EveIndex(EventInfo* ev) {
    int n = GetTrack(ev->track)->events;
    for (int k = 0; k < n; k++) {
        if (ev == FindTrackEvent(ev->track, k)) {
            return k;
        }
    }
    return -1;
}

// Fills the course list with the events at tracks in environment env.
inline void EvPicker::SetEnv(EvPairs** pairs, const char* env_) {
    EvPicker* p = this;
    EventInfo* ev;
    int i;
    int j;
    p->env = env_;
    p->cur = 0;
    p->count = 0;
    for (i = 0; i < (*pairs)->count; i++) {
        if (!strcasecmp(GetTrackEnviron((*pairs)->items[i].track), p->env)) {
            ev = (*pairs)->items[i].ev;
            p->ents[p->count].track = ev->track;
            p->ents[p->count].eve = EveIndex(ev);
            j = FindUnlock(ev, lbl_8055F28C);
            if (lbl_8055F290 && j >= 0) {
                p->ents[p->count].name = lbl_8055F28C->entries[j].name;
            } else {
                p->ents[p->count].name = EventFriendlyName((u8*)ev);
            }
            p->count++;
        }
    }
}

extern "C" {

int fn_8024A4B8(const char** a, const char** b) { return strcasecmp(*a, *b); }

int fn_8024A4C4(s32* track, s32* eve, s32* diff) { return fn_8024974C(track, eve, diff, 0); }

}  // extern "C"

int GetNumTracks(void);



extern "C" {
void fn_801332D0(int slot, const char* name, s32* value);
void fn_80133348(int slot, const char* name, s32 value);
void fn_8027BCD0(EvListBuf* list);
int fn_8024B070(s32* key, const u8* list);

// Multiplayer event choice: starts from the remembered track/event, runs the pickers and remembers the
// result. Returns 0 when backed out.
int fn_8024A4CC(s32* track, s32* eve, s32* extra) {
    *track = -1;
    *eve = 0;
    *extra = 0;
    fn_801332D0(0, lbl_8055BF14, track);
    fn_801332D0(0, lbl_8055BF10, eve);
    EvListBuf list;
    fn_8027BCD0(&list);
    s32 key = -1;
    if (*track >= 0 && *track < GetNumTracks()) {
        TrackInfo* t = GetTrack(*track);
        if (*eve >= 0 && *eve < t->events) {
            s32 id = FindTrackEvent(*track, *eve)->id;
            if (list.Find(id) >= 0) {
                key = id;
            }
        }
    }
    if (fn_8024B070(&key, (u8*)&list)) {
        *track = GetEvent(key)->track;
        *eve = -1;
        int k = GetTrack(*track)->events;
        while (k-- > 0) {
            if (key == FindTrackEvent(*track, k)->id) {
                *eve = k;
                break;
            }
        }
        fn_80133348(0, lbl_8055BF14, *track);
        fn_80133348(0, lbl_8055BF10, *eve);
        return 1;
    }
    return 0;
}

int fn_8024A6A4(s32* p) {
    if (fn_802411D4() == 0) {
        *p = -1;
        return 1;
    }
    return 0;
}

extern char lbl_8055B860[8];

// Location menu: on a choice stores the location name in *sel and returns 1.
int fn_8024A6EC(const char** sel, u8* list) {
    if (!fn_802411D4()) {
        return 0;
    }
    CountryScreen screen((EvList*)list);
    screen.Select(*sel);
    MenuItem items[] = {
        MenuItem("__first", 5, &screen, 0, 0),
        MenuItem("#country_1", screen.Name(0), (int)NoCaption()),
        MenuItem("#country_1", 0, 0, 1, 0),
        MenuItem("#country_2", screen.Name(1), (int)NoCaption()),
        MenuItem("#country_2", 0, 0, 2, 0),
        MenuItem("#country_3", screen.Name(2), (int)NoCaption()),
        MenuItem("#country_3", 0, 0, 3, 0),
        MenuItem("#country_4", screen.Name(3), (int)NoCaption()),
        MenuItem("#country_4", 0, 0, 4, 0),
        MenuItem("#country_5", screen.Name(4), (int)NoCaption()),
        MenuItem("#country_5", 0, 0, 5, 0),
        MenuItem("#country_6", screen.Name(5), (int)NoCaption()),
        MenuItem("#country_6", 0, 0, 6, 0),
        MenuItem("#country_7", screen.Name(6), (int)NoCaption()),
        MenuItem("#country_7", 0, 0, 7, 0),
        MenuItem("#cup_1", screen.Cup(0), (int)NoCaption()),
        MenuItem("#course_1", screen.Course(0), (int)NoCaption()),
        MenuItem("#lock_1", 2, screen.Lock(0), 0, 0),
        MenuItem("#cup_2", screen.Cup(1), (int)NoCaption()),
        MenuItem("#course_2", screen.Course(1), (int)NoCaption()),
        MenuItem("#lock_2", 2, screen.Lock(1), 0, 0),
        MenuItem("#cup_3", screen.Cup(2), (int)NoCaption()),
        MenuItem("#course_3", screen.Course(2), (int)NoCaption()),
        MenuItem("#lock_3", 2, screen.Lock(2), 0, 0),
        MenuItem("#cup_4", screen.Cup(3), (int)NoCaption()),
        MenuItem("#course_4", screen.Course(3), (int)NoCaption()),
        MenuItem("#lock_4", 2, screen.Lock(3), 0, 0),
        MenuItem("#cup_5", screen.Cup(4), (int)NoCaption()),
        MenuItem("#course_5", screen.Course(4), (int)NoCaption()),
        MenuItem("#lock_5", 2, screen.Lock(4), 0, 0),
        MenuItem(lbl_8055B860, 6, 0, -1, 27),
        MenuItem(lbl_80498998),
    };
    MenuDesc desc = {"multi_country.lyt", 0, (int (*)(int*))fn_8024A6A4, 0, 0};
    desc.items = items;
    int r = fn_802146BC(&desc, -999, -999, 1);
    if (r != -1) {
        *sel = screen.Name(r - 1);
        return 1;
    }
    return 0;
}

extern char lbl_8055B860[8];

// Course menu of location sel; on a choice stores the event in *key and returns 1.
int fn_8024ACB4(s32* key, const char* sel, u8* list) {
    if (!fn_802411D4()) {
        return 0;
    }
    CourseScreen screen(sel, (EvList*)list);
    screen.Select(*key);
    MenuItem items[] = {
        MenuItem("__first", 5, &screen, 0, 0),
        MenuItem("#course_1", screen.CourseName(0), (int)NoCaption()),
        MenuItem("#course_1", 0, 0, screen.CourseId(0), 0),
        MenuItem("#course_2", screen.CourseName(1), (int)NoCaption()),
        MenuItem("#course_2", 0, 0, screen.CourseId(1), 0),
        MenuItem("#course_3", screen.CourseName(2), (int)NoCaption()),
        MenuItem("#course_3", 0, 0, screen.CourseId(2), 0),
        MenuItem("#course_4", screen.CourseName(3), (int)NoCaption()),
        MenuItem("#course_4", 0, 0, screen.CourseId(3), 0),
        MenuItem("#course_5", screen.CourseName(4), (int)NoCaption()),
        MenuItem("#course_5", 0, 0, screen.CourseId(4), 0),
        MenuItem("#trackmap", 0x12, screen.Tex(), 0, 0),
        MenuItem("#trackmap", 2, screen.IdPtr(), 0, 0),
        MenuItem("#course_name", screen.Name(), (int)NoCaption()),
        MenuItem(lbl_8055B860, 6, 0, -1, 27),
        MenuItem(lbl_80498998),
    };
    MenuDesc desc = {"multi_course.lyt", 0, (int (*)(int*))fn_8024A6A4, 0, 0};
    desc.items = items;
    int r = fn_802146BC(&desc, -999, -999, 1);
    if (r != -1) {
        *key = r;
        return 1;
    }
    return 0;
}

int fn_80215228(void);

static inline int FindById(const EvList* list, const s32* key) {
    for (int i = 0; i < list->count; i++) {
        const EvEntry& e = list->items[i];
        if (e.id == *key) {
            return i;
        }
    }
    return -1;
}

int fn_8024B070(s32* key, const u8* list) {
    int i = FindById((const EvList*)list, key);
    const char* sel = i >= 0 ? ((EvList*)list)->items[i].group : 0;
    if (fn_80215228()) {
        lbl_8055F298 = 0;
    }
    while (lbl_8055F298 || fn_8024A6EC(&sel, (u8*)list)) {
        lbl_8055F298 = 0;
        if (fn_8024ACB4(key, sel, (u8*)list)) {
            lbl_8055F298 = 1;
            break;
        }
    }
    return lbl_8055F298;
}

const char* fn_8024B178(int ev) {
    const char* name = ((VN*)GetEvent(ev)->node)->v14("name");
    const char* prefix = "career_";
    if (!strncmp(name, prefix, strlen(prefix))) {
        name += strlen(prefix);
    }
    fn_801345D8(lbl_804989AC, 0x21, "/%s.tex", name);
    return lbl_804989AC;
}

int fn_8024B214(u8* a, u8* b) {
    u8 fa = U8(a, 0x4);
    if (fa && !U8(b, 0x4)) {
        return 1;
    }
    if (U8(b, 0x4) && !fa) {
        return -1;
    }
    return strcasecmp(*(const char**)a, *(const char**)b);
}

const char* fn_80131234(const char* key);

}  // extern "C"

static inline EvEntry* EventAt(CountryScreen* s, int i) { return &s->list->items[i]; }

// Fills the five course rows of the selected location.
void CountryScreen::Refresh(void) {
    EvEntry* e;
    const char* group;
    int i;
    int k;
    sel = fn_80214D04();
    group = groups.data[sel].name;
    memset(slots, 0, 5 * sizeof(CourseSlot));
    for (k = 0; k < 5; k++) {
        slots[k].open = 1;
    }
    for (i = 0; i < list->count; i++) {
        e = EventAt(this, i);
        if (!strcasecmp(group, e->group)) {
            CourseSlot* c = &slots[e->slot];
            SafeStrCopy(c->name, e->name, 50);
            if (e->unlocked) {
                SafeStrCopy(c->name2, e->name2, 50);
                c->open = 1;
            } else {
                const char* locked = fn_80131234("UIMulti:LOCKED_COURSE_NAME");
                SafeStrCopy(c->name2, locked, 50);
                c->open = 0;
            }
        }
    }
}

template <class T>
/* Helper name unproven: the address of element i; the accessor boundary is needed (in place: CountryScreen ctor 100 -> 98.62) */
static inline T* ElemAt(T* p, int i) {
    return &p[i];
}

extern "C" int fn_8024B214(u8* a, u8* b);

CountryScreen::CountryScreen(EvList* l) {
    list = l;
    anyNew = 0;
    for (int i = 0; i < l->count; i++) {
        EvEntry* e = ElemAt(l->items, i);
        int isNew = 1;
        int j;
        for (j = 0; j < groups.Size(); j++) {
            if (!e->unlocked || !strcasecmp(groups.data[j].name, e->group)) {
                isNew = 0;
            }
        }
        if (isNew) {
            GroupEntry* g = groups.Next();
            g->name = e->group;
            g->flag = e->flag;
        }
        if (e->unlocked && e->flag) {
            anyNew = 1;
        }
    }
    fn_80265DCC(groups.data, groups.count, 8, fn_8024B214);
    memset(slots, 0, 0x208);
    sel = -1;
}

void CountryScreen::Select(const char* name) {
    sel = 0;
    if (name) {
        for (int i = 0; i < groups.count; i++) {
            if (!strcasecmp(groups.data[i].name, name)) {
                sel = i;
            }
        }
    }
}

const char* CountryScreen::Name(int i) {
    if (i < groups.count) {
        return groups.data[i].name;
    }
    return "";
}

char* CountryScreen::Course(int i) { return slots[i].name2; }

char* CountryScreen::Cup(int i) { return slots[i].name; }

s32* CountryScreen::Lock(int i) { return &slots[i].open; }

void CountryScreen::Enter(void) {
    fn_80214CB4(sel);
    for (int i = groups.count; i < groups.cap; i++) {
        fn_80214A9C(i + 1);
    }
    if (!anyNew) {
        fn_80214A9C(10);
    } else {
        fn_80214AF0(10);
    }
    Refresh();
}

void CountryScreen::Update(void) { Refresh(); }

void CourseScreen::Refresh(void) {
    sel = fn_80214D04();
    SafeStrCopy(name, events.data[sel]->name2, 50);
    id = events.data[sel]->id;
}

static inline void AddIfInGroup(const char* group, EvEntry* e, CourseScreen* s) {
    if (!strcasecmp(group, e->group)) {
        s->events.Push(e);
        s->tex.Add(e->id, fn_8024B178(e->id));
    }
}

CourseScreen::CourseScreen(const char* group, EvList* l) : tex(10, 0) {
    EvEntry* e;
    int i;
    for (i = 0; i < l->count; i++) {
        AddIfInGroup(group, &l->items[i], this);
    }
    name[0] = 0;
    id = -1;
    sel = -1;
}

void CourseScreen::Select(s32 id_) {
    sel = -1;
    for (int i = 0; i < events.count; i++) {
        if (id_ == events.data[i]->id) {
            sel = i;
        }
    }
}

char* CourseScreen::Name(void) { return name; }

const char* CourseScreen::CourseName(int i) {
    if (i < events.count) {
        return events.data[i]->name;
    }
    return "";
}

s32 CourseScreen::CourseId(int i) {
    if (i < events.count) {
        return events.data[i]->id;
    }
    return -1;
}

DynoTex* CourseScreen::Tex(void) { return &tex; }

s32* CourseScreen::IdPtr(void) { return &id; }

void CourseScreen::Enter(void) {
    fn_80214CB4(sel);
    for (int i = 0; i < events.cap; i++) {
        if (i < events.count) {
            if (!events.data[i]->unlocked) {
                fn_80214B44(i + 1);
            }
        } else {
            fn_80214A9C(i + 1);
        }
    }
}

void CourseScreen::Update(void) { Refresh(); }
