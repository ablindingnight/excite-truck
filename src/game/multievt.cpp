// Multiplayer event menu (0x802494F8-0x8024BDDC, "multi_evt.lyt" / "multi_country.lyt" /
// "multi_course.lyt"): choosing the location, course, event and difficulty for a multiplayer race,
// remembering the last choices ("last_mp_event_count", ...), and the stats table beside the course list.
// Written against raw field offsets; the large screen routines are still stubs. Names are guesses
// apart from the strings.
#include "game/common.h"
#include "game/menu.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

const char* lbl_8055BF10 = "last_mp_event_count";
const char* lbl_8055BF14 = "last_mp_eve";
const char* lbl_8055BF18 = "last_mp_difficulty";

// A list widget: the base data comes first, the vptr after it.
class VC {
public:
    u8 b0;
    s32 f4;
    u8 b8;
    virtual void s08(void);
    virtual void s0C(void);
    virtual void s10(void);
    virtual void s14(void);
    virtual void s18(void);
    virtual void s1C(void);
    virtual void s20(void);
    virtual void s24(void);
    virtual void s28(void);
    virtual void s2C(void);
    virtual void s30(u32 a, const char* b);
    virtual void s34(void);
    virtual void s38(s32* a, s32* b);
};

class StatsTable {
public:
    void Show(s32 event);
};

struct NamedValue {
    const char* name;
    s32 value;
};

struct NameEntry {
    s32 id;
    const char* name;
};

void* FindTrackEvent(int track, int eve);
u8* GetEvent(int i);

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
void __dt__7UITableFv(void* p, int del);
void __dt__7DynoTexFv(void* p, int del);
void fn_8024974C(u32 a, u32 b, u32 c, int d);
void fn_8024B260(u8* p);
void fn_80214AF0(int s);
void fn_80214A9C(int s);
void fn_80214B44(int i);
void fn_8024B6AC(u8* p);
}

void operator delete(void* p);

extern const NameEntry lbl_802FA4B0[3];
extern char lbl_804989AC[];

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

static inline int FindUnlock(u8* car) {
    UnlockList* l = lbl_8055F28C;
    for (int i = 0; i < l->count; i++) {
        if (S32(car, 0x0) == l->entries[i].id) {
            return i;
        }
    }
    return -1;
}

extern char* lbl_8055F294;
extern u8 lbl_8055F290;

// Copies at most n characters, always terminating.
static inline void CopyName(const char* s, int n, char* d) {
    while ((*d++ = *s++) != 0) {
        if (--n <= 0) {
            d[-1] = 0;
            break;
        }
    }
}

ModuleTag lbl_8055F280;
UnkStatic123s lbl_80498998;
UnkStatic7F lbl_8055F288;
s32 lbl_8055BF34 = -1;

extern "C" {

u8* fn_802494F8(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

u8* fn_80249538(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_80249578(u8* p) { fn_8023E52C(*(u32*)PTR(p, 0x10)); }

void fn_80249584(u8* p) {}

void fn_80249588(u8* p) {}

int fn_8024958C(u8* car) {
    u32 ok = S32(car, 0x8) == 0x180302;
    if (ok) {
        ok = FindUnlock(car) != -1;
    }
    if (ok) {
        ok = lbl_8055F28C->entries[FindUnlock(car)].unlocked;
    }
    return ok;
}

int fn_80249660(u8* p) { return 1; }

int fn_80249668(s32* out) {
    const char* names[3] = {"Select Location", "Select Event", "Select Difficulty"};
    const char* src = names[fn_80214D04()];
    CopyName(src, 0x100, lbl_8055F294);
    if (lbl_8055F290 && !fn_802411D4()) {
        *out = -1;
        return 1;
    }
    return 0;
}

void fn_80249720(void) { lbl_8055BF34 = fn_80214D04(); }

void fn_80249744(void) { fn_80214CB4(lbl_8055BF34); }

void fn_8024974C(u32 a, u32 b, u32 c, int d) {}

u8* fn_8024A110(u8* p, int del) {
    if (p) {
        if (p) {
            __dt__7UITableFv(p, 0);
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

u8* fn_8024A16C(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

u8* fn_8024A1AC(u8* p, int del) {
    if (p) {
        __dt__7DynoTexFv(p, 0);
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

u8* fn_8024A204(u8* p, int del) {
    if (p) {
        U32(p, 0x11C) = 0;
        u8* m = p + 0x10;
        if (m) {
            if (m) {
                U32(p, 0x10) = 0;
                U32(p, 0x14) = 0;
                U32(p, 0x18) = 0;
            }
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

u8* fn_8024A268(u8* p, int del) {
    if (p) {
        S32(p, 0x18) = -1;
        U32(p, 0x10) = 0;
        U32(p, 0x1C) = 0;
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

void fn_8024A2BC(u8* p, s32 a, s32 b) {
    u8* q = p;
    for (int i = 0; i < S32(p, 0x14); i++) {
        if (a == S32(q, 0x20) && b == S32(q, 0x24)) {
            S32(p, 0x10) = i;
            return;
        }
        q += 0xC;
    }
}

}  // extern "C"

// A track record as GetTrack returns it.
struct MpTrack {
    u8 pad0[0x70];  // offset 0x0
    s32 numEves;    // offset 0x70
};

MpTrack* GetTrack(int track);
const char* GetTrackEnviron(const char* track);
const char* GetEventDisplayName(int ev);

struct EvPair {
    const char* track;  // offset 0x0
    u8* ev;             // offset 0x4
};

struct EvPairs {
    EvPair* items;  // offset 0x0
    s32 count;      // offset 0x4
};

// One course row of the picker.
struct EvPick {
    s32 track;         // offset 0x0
    s32 eve;           // offset 0x4
    const char* name;  // offset 0x8
};

struct EvPicker {
    u8 pad0[0x10];     // offset 0x0
    s32 cur;           // offset 0x10
    s32 count;         // offset 0x14
    u8 pad18[0x4];     // offset 0x18
    const char* env;   // offset 0x1C
    EvPick ents[1];    // offset 0x20
};

// The unlock-table row for event ev, or -1.
static inline int FindUnlockEv(u8* ev) {
    UnlockList* l = lbl_8055F28C;
    for (int i = 0; i < l->count; i++) {
        if (l->entries[i].id == S32(ev, 0x0)) {
            return i;
        }
    }
    return -1;
}

// The event's index within its track, or -1.
static inline int EveIndex(u8* ev) {
    int n = GetTrack(S32(ev, 0x4))->numEves;
    for (int k = 0; k < n; k++) {
        if (ev == FindTrackEvent(S32(ev, 0x4), k)) {
            return k;
        }
    }
    return -1;
}

extern "C" {

// Fills the course list with the events at tracks in environment env.
void fn_8024A304(EvPicker* p, EvPairs** pairs, const char* env) {
    u8* ev;
    int i;
    int j;
    p->env = env;
    p->cur = 0;
    p->count = 0;
    for (i = 0; i < (*pairs)->count; i++) {
        if (!strcasecmp(GetTrackEnviron((*pairs)->items[i].track), p->env)) {
            ev = (*pairs)->items[i].ev;
            p->ents[p->count].track = S32(ev, 0x4);
            p->ents[p->count].eve = EveIndex(ev);
            j = FindUnlockEv(ev);
            if (lbl_8055F290 && j >= 0) {
                p->ents[p->count].name = lbl_8055F28C->entries[j].name;
            } else {
                p->ents[p->count].name = ((ValueNode*)PTR(ev, 0x20))->GetString("friendly_name", GetEventDisplayName(S32(ev, 0x0)));
            }
            p->count++;
        }
    }
}

int fn_8024A4B8(const char** a, const char** b) { return strcasecmp(*a, *b); }

void fn_8024A4C4(u32 a, u32 b, u32 c) { fn_8024974C(a, b, c, 0); }

}  // extern "C"

int GetNumTracks(void);

struct EvEntryBuf {
    s32 id;         // offset 0x0
    u8 pad4[0x14];  // offset 0x4
};

// An event list with room for 0x80 entries on the stack.
struct EvListBuf {
    EvListBuf(void) {
        count = 0;
        items = buf;
        cap = 0x80;
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


extern "C" {
void fn_801332D0(int slot, const char* name, s32* value);
void fn_80133348(int slot, const char* name, s32 value);
void fn_8027BCD0(EvListBuf* list);
int fn_8024B070(s32* key, u8* list);

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
        MpTrack* t = GetTrack(*track);
        if (*eve >= 0 && *eve < t->numEves) {
            s32 id = S32(FindTrackEvent(*track, *eve), 0x0);
            if (list.Find(id) >= 0) {
                key = id;
            }
        }
    }
    if (fn_8024B070(&key, (u8*)&list)) {
        *track = S32(GetEvent(key), 0x4);
        *eve = -1;
        int k = GetTrack(*track)->numEves;
        while (k-- > 0) {
            if (key == S32(FindTrackEvent(*track, k), 0x0)) {
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

struct EvList;
u8* fn_8024B3E8(u8* p, EvList* list);
void fn_8024B544(u8* p, const char* name);
const char* fn_8024B5D4(u8* p, int i);
u8* fn_8024B5F8(u8* p, int i);
u8* fn_8024B608(u8* p, int i);
u8* fn_8024B618(u8* p, int i);
extern char lbl_8055B860[8];

// Location menu: on a choice stores the location name in *sel and returns 1.
int fn_8024A6EC(const char** sel, u8* list) {
    if (!fn_802411D4()) {
        return 0;
    }
    u32 buf[0xA2];
    fn_8024B3E8((u8*)buf, (EvList*)list);
    fn_8024B544((u8*)buf, *sel);
    MenuItem items[] = {
        MenuItem("__first", 5, (u8*)buf, 0, 0),
        MenuItem("#country_1", fn_8024B5D4((u8*)buf, 0)),
        MenuItem("#country_1", 0, 0, 1, 0),
        MenuItem("#country_2", fn_8024B5D4((u8*)buf, 1)),
        MenuItem("#country_2", 0, 0, 2, 0),
        MenuItem("#country_3", fn_8024B5D4((u8*)buf, 2)),
        MenuItem("#country_3", 0, 0, 3, 0),
        MenuItem("#country_4", fn_8024B5D4((u8*)buf, 3)),
        MenuItem("#country_4", 0, 0, 4, 0),
        MenuItem("#country_5", fn_8024B5D4((u8*)buf, 4)),
        MenuItem("#country_5", 0, 0, 5, 0),
        MenuItem("#country_6", fn_8024B5D4((u8*)buf, 5)),
        MenuItem("#country_6", 0, 0, 6, 0),
        MenuItem("#country_7", fn_8024B5D4((u8*)buf, 6)),
        MenuItem("#country_7", 0, 0, 7, 0),
        MenuItem("#cup_1", (const char*)fn_8024B608((u8*)buf, 0)),
        MenuItem("#course_1", (const char*)fn_8024B5F8((u8*)buf, 0)),
        MenuItem("#lock_1", 2, fn_8024B618((u8*)buf, 0), 0, 0),
        MenuItem("#cup_2", (const char*)fn_8024B608((u8*)buf, 1)),
        MenuItem("#course_2", (const char*)fn_8024B5F8((u8*)buf, 1)),
        MenuItem("#lock_2", 2, fn_8024B618((u8*)buf, 1), 0, 0),
        MenuItem("#cup_3", (const char*)fn_8024B608((u8*)buf, 2)),
        MenuItem("#course_3", (const char*)fn_8024B5F8((u8*)buf, 2)),
        MenuItem("#lock_3", 2, fn_8024B618((u8*)buf, 2), 0, 0),
        MenuItem("#cup_4", (const char*)fn_8024B608((u8*)buf, 3)),
        MenuItem("#course_4", (const char*)fn_8024B5F8((u8*)buf, 3)),
        MenuItem("#lock_4", 2, fn_8024B618((u8*)buf, 3), 0, 0),
        MenuItem("#cup_5", (const char*)fn_8024B608((u8*)buf, 4)),
        MenuItem("#course_5", (const char*)fn_8024B5F8((u8*)buf, 4)),
        MenuItem("#lock_5", 2, fn_8024B618((u8*)buf, 4), 0, 0),
        MenuItem(lbl_8055B860, 6, 0, -1, 27),
        MenuItem(lbl_80498998),
    };
    MenuDesc desc = {"multi_country.lyt", 0, (int (*)(int*))fn_8024A6A4, 0, 0};
    desc.items = items;
    int r = fn_802146BC(&desc, -999, -999, 1);
    if (r != -1) {
        *sel = fn_8024B5D4((u8*)buf, r - 1);
        U32(buf, 0x18) = 0;
        U32(buf, 0x1C) = 0;
        U32(buf, 0x20) = 0;
        return 1;
    }
    U32(buf, 0x18) = 0;
    U32(buf, 0x1C) = 0;
    U32(buf, 0x20) = 0;
    return 0;
}

u8* fn_8024AC54(u8* p, int del) {
    if (p) {
        u8* m = p + 0x18;
        if (m) {
            if (m) {
                U32(p, 0x18) = 0;
                U32(p, 0x1C) = 0;
                U32(p, 0x20) = 0;
            }
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

struct EvList;
u8* fn_8024B73C(u8* p, const char* name, EvList* list);
void fn_8024B830(u8* p, s32 id);
u8* fn_8024B874(u8* p);
const char* fn_8024B87C(u8* p, int i);
s32 fn_8024B8A4(u8* p, int i);
u8* fn_8024B8CC(u8* p);
u8* fn_8024B8D4(u8* p);
void __dt__7DynoTexFv(void* p, int del);
extern char lbl_8055B860[8];

// Tears down the course list built by fn_8024B73C.
static inline void CourseListFree(u8* m) {
    __dt__7DynoTexFv(m + 0x4C, -1);
    U32(m, 0x18) = 0;
    U32(m, 0x1C) = 0;
    U32(m, 0x20) = 0;
}

// Course menu of location sel; on a choice stores the event in *key and returns 1.
int fn_8024ACB4(s32* key, const char* sel, u8* list) {
    if (!fn_802411D4()) {
        return 0;
    }
    u32 buf[0x28];
    fn_8024B73C((u8*)buf, sel, (EvList*)list);
    fn_8024B830((u8*)buf, *key);
    MenuItem items[] = {
        MenuItem("__first", 5, (u8*)buf, 0, 0),
        MenuItem("#course_1", fn_8024B87C((u8*)buf, 0)),
        MenuItem("#course_1", 0, 0, fn_8024B8A4((u8*)buf, 0), 0),
        MenuItem("#course_2", fn_8024B87C((u8*)buf, 1)),
        MenuItem("#course_2", 0, 0, fn_8024B8A4((u8*)buf, 1), 0),
        MenuItem("#course_3", fn_8024B87C((u8*)buf, 2)),
        MenuItem("#course_3", 0, 0, fn_8024B8A4((u8*)buf, 2), 0),
        MenuItem("#course_4", fn_8024B87C((u8*)buf, 3)),
        MenuItem("#course_4", 0, 0, fn_8024B8A4((u8*)buf, 3), 0),
        MenuItem("#course_5", fn_8024B87C((u8*)buf, 4)),
        MenuItem("#course_5", 0, 0, fn_8024B8A4((u8*)buf, 4), 0),
        MenuItem("#trackmap", 0x12, fn_8024B8CC((u8*)buf), 0, 0),
        MenuItem("#trackmap", 2, fn_8024B8D4((u8*)buf), 0, 0),
        MenuItem("#course_name", (const char*)fn_8024B874((u8*)buf)),
        MenuItem(lbl_8055B860, 6, 0, -1, 27),
        MenuItem(lbl_80498998),
    };
    MenuDesc desc = {"multi_course.lyt", 0, (int (*)(int*))fn_8024A6A4, 0, 0};
    desc.items = items;
    int r = fn_802146BC(&desc, -999, -999, 1);
    if (r != -1) {
        *key = r;
        CourseListFree((u8*)buf);
        return 1;
    }
    CourseListFree((u8*)buf);
    return 0;
}

u8* fn_8024AFF8(u8* p, int del) {
    if (p) {
        __dt__7DynoTexFv(p + 0x4C, -1);
        u8* m = p + 0x18;
        if (m) {
            if (m) {
                U32(p, 0x18) = 0;
                U32(p, 0x1C) = 0;
                U32(p, 0x20) = 0;
            }
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

int fn_80215228(void);
extern u8 lbl_8055F298;

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

static inline int FindById(EvList* list, s32* key) {
    for (int i = 0; i < list->count; i++) {
        EvEntry& e = list->items[i];
        if (e.id == *key) {
            return i;
        }
    }
    return -1;
}

int fn_8024B070(s32* key, u8* list) {
    int i = FindById((EvList*)list, key);
    const char* sel = i >= 0 ? ((EvList*)list)->items[i].group : 0;
    if (fn_80215228()) {
        lbl_8055F298 = 0;
    }
    while (lbl_8055F298 || fn_8024A6EC(&sel, list)) {
        lbl_8055F298 = 0;
        if (fn_8024ACB4(key, sel, list)) {
            lbl_8055F298 = 1;
            break;
        }
    }
    return lbl_8055F298;
}

const char* fn_8024B178(int ev) {
    const char* name = ((VN*)PTR(GetEvent(ev), 0x20))->v14("name");
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

// One course row on the course list.
struct CourseSlot {
    char name[50];   // offset 0x0
    char name2[50];  // offset 0x32
    s32 open;        // offset 0x64
};

const char* fn_80131234(const char* key);

void fn_8024B260(u8* p) {
    EvEntry* e;
    const char* group;
    int i;
    int k;
    S32(p, 0x280) = fn_80214D04();
    group = ((const char**)PTR(p, 0x18))[S32(p, 0x280) * 2];
    memset(p + 0x78, 0, 5 * sizeof(CourseSlot));
    for (k = 0; k < 5; k++) {
        ((CourseSlot*)(p + 0x78))[k].open = 1;
    }
    for (i = 0; i < ((EvList*)PTR(p, 0x74))->count; i++) {
        e = &((EvList*)PTR(p, 0x74))->items[i];
        if (!strcasecmp(group, e->group)) {
            CourseSlot* c = &((CourseSlot*)(p + 0x78))[e->slot];
            CopyName(e->name, 50, c->name);
            if (e->unlocked) {
                CopyName(e->name2, 50, c->name2);
                c->open = 1;
            } else {
                const char* locked = fn_80131234("UIMulti:LOCKED_COURSE_NAME");
                CopyName(locked, 50, c->name2);
                c->open = 0;
            }
        }
    }
}

// One location on the location list.
struct GroupEntry {
    const char* name;  // offset 0x0
    u8 flag;           // offset 0x4
    u8 pad5[0x3];
};

struct GroupVec {
    GroupEntry* data;  // offset 0x0
    s32 count;         // offset 0x4
    s32 cap;           // offset 0x8
};

extern void* lbl_80343BD0[];
void fn_80265DCC(void* base, int n, int size, int (*cmp)(u8*, u8*));

u8* fn_8024B3E8(u8* p, EvList* list) {
    PTR(p, 0x74) = (u8*)list;
    S32(p, 0x14) = 0;
    *(void**)p = lbl_80343BD0;
    PTR(p, 0x18) = p + 0x24;
    S32(p, 0x20) = 10;
    S32(p, 0x1C) = 0;
    U8(p, 0x284) = 0;
    for (int i = 0; i < list->count; i++) {
        EvEntry* e = &list->items[i];
        int isNew = 1;
        for (int j = 0; j < S32(p, 0x1C); j++) {
            if (!e->unlocked || !strcasecmp(((GroupEntry*)PTR(p, 0x18))[j].name, e->group)) {
                isNew = 0;
            }
        }
        if (isNew) {
            GroupEntry* g = &((GroupVec*)(p + 0x18))->data[((GroupVec*)(p + 0x18))->count++];
            g->name = e->group;
            g->flag = e->flag;
        }
        if (e->unlocked && e->flag) {
            U8(p, 0x284) = 1;
        }
    }
    fn_80265DCC(PTR(p, 0x18), S32(p, 0x1C), 8, fn_8024B214);
    memset(p + 0x78, 0, 0x208);
    S32(p, 0x280) = -1;
    return p;
}

void fn_8024B544(u8* p, const char* name) {
    S32(p, 0x280) = 0;
    if (name) {
        for (int i = 0; i < S32(p, 0x1C); i++) {
            if (!strcasecmp(((const NamedValue*)PTR(p, 0x18))[i].name, name)) {
                S32(p, 0x280) = i;
            }
        }
    }
}

const char* fn_8024B5D4(u8* p, int i) {
    if (i < S32(p, 0x1C)) {
        return ((const char**)PTR(p, 0x18))[i * 2];
    }
    return "";
}

u8* fn_8024B5F8(u8* p, int i) { return p + i * 0x68 + 0xAA; }

u8* fn_8024B608(u8* p, int i) { return p + i * 0x68 + 0x78; }

u8* fn_8024B618(u8* p, int i) { return p + i * 0x68 + 0xDC; }

void fn_8024B628(u8* p) {
    fn_80214CB4(S32(p, 0x280));
    for (int i = S32(p, 0x1C); i < S32(p, 0x20); i++) {
        fn_80214A9C(i + 1);
    }
    if (!U8(p, 0x284)) {
        fn_80214A9C(10);
    } else {
        fn_80214AF0(10);
    }
    fn_8024B260(p);
}

void fn_8024B6A8(u8* p) { fn_8024B260(p); }

void fn_8024B6AC(u8* p) {
    S32(p, 0x9C) = fn_80214D04();
    CopyName((const char*)PTR(((u8**)PTR(p, 0x18))[S32(p, 0x9C)], 0x10), 50, (char*)(p + 0x64));
    U32(p, 0x98) = U32(((u8**)PTR(p, 0x18))[S32(p, 0x9C)], 0x0);
}

extern void* lbl_80343B70[];

// Fixed-capacity pointer list with inline storage.
struct PtrVec {
    EvEntry** data;  // offset 0x0
    s32 count;       // offset 0x4
    s32 cap;         // offset 0x8

    void Push(EvEntry* e) {
        EvEntry** d = data;
        int n = count;
        count = n + 1;
        d[n] = e;
    }
};
void __ct__7DynoTexFiUc(void* p, int n, u8 f);
void Add__7DynoTexFiPCc(void* p, int id, const char* name);

u8* fn_8024B73C(u8* p, const char* name, EvList* list) {
    EvEntry* e;
    int i;
    S32(p, 0x14) = 0;
    *(void**)p = lbl_80343B70;
    PTR(p, 0x18) = p + 0x24;
    S32(p, 0x20) = 10;
    S32(p, 0x1C) = 0;
    __ct__7DynoTexFiUc(p + 0x4C, 10, 0);
    for (i = 0; i < list->count; i++) {
        e = &list->items[i];
        if (!strcasecmp(name, e->group)) {
            ((PtrVec*)(p + 0x18))->Push(e);
            Add__7DynoTexFiPCc(p + 0x4C, e->id, fn_8024B178(e->id));
        }
    }
    U8(p, 0x64) = 0;
    S32(p, 0x98) = -1;
    S32(p, 0x9C) = -1;
    return p;
}

void fn_8024B830(u8* p, s32 id) {
    S32(p, 0x9C) = -1;
    for (int i = 0; i < S32(p, 0x1C); i++) {
        if (id == S32(((u8**)PTR(p, 0x18))[i], 0x0)) {
            S32(p, 0x9C) = i;
        }
    }
}

u8* fn_8024B874(u8* p) { return p + 0x64; }

const char* fn_8024B87C(u8* p, int i) {
    if (i < S32(p, 0x1C)) {
        return *(const char**)(((u8**)PTR(p, 0x18))[i] + 0x4);
    }
    return "";
}

s32 fn_8024B8A4(u8* p, int i) {
    if (i < S32(p, 0x1C)) {
        return S32(((u8**)PTR(p, 0x18))[i], 0x0);
    }
    return -1;
}

u8* fn_8024B8CC(u8* p) { return p + 0x4c; }

u8* fn_8024B8D4(u8* p) { return p + 0x98; }

void fn_8024B8DC(u8* p) {
    fn_80214CB4(S32(p, 0x9C));
    for (int i = 0; i < S32(p, 0x20); i++) {
        if (i < S32(p, 0x1C)) {
            if (!U8(((u8**)PTR(p, 0x18))[i], 0x14)) {
                fn_80214B44(i + 1);
            }
        } else {
            fn_80214A9C(i + 1);
        }
    }
}

void fn_8024B970(u8* p) { fn_8024B6AC(p); }

u32 fn_8024B974(u8* p) { return U32(p + S32(p, 0x10) * 0xC, 0x28); }

void fn_8024B988(u8* p) {
    s32 n = S32(p, 0x14);
    S32(p, 0x10) = (n + S32(p, 0x10) - 1) % n;
}

void fn_8024B9AC(u8* p) { S32(p, 0x10) = (S32(p, 0x10) + 1) % S32(p, 0x14); }

void fn_8024B9CC(u8* p, s32* a, s32* b) {
    u8* q = p + S32(p, 0x10) * 0xC;
    *a = S32(q, 0x20);
    *b = S32(q, 0x24);
}

u32 fn_8024B9EC(u8* p) { return ((u32*)PTR(p, 0x10))[S32(p, 0x11C) * 2]; }

void fn_8024BA00(u8* p) {
    S32(p, 0x11C) = (S32(p, 0x14) + S32(p, 0x11C) - 1) % S32(p, 0x14);
    ((VC*)PTR(p, 0x124))->s30(U32(p, 0x120), ((const NameEntry*)PTR(p, 0x10))[S32(p, 0x11C)].name);
    fn_801345D8((char*)(p + 0x128), 0x20, "%s/race.%s", ((const NameEntry*)PTR(p, 0x10))[S32(p, 0x11C)].name, "m2v");
}

void fn_8024BAA0(u8* p) {
    S32(p, 0x11C) = (S32(p, 0x11C) + 1) % S32(p, 0x14);
    ((VC*)PTR(p, 0x124))->s30(U32(p, 0x120), ((const NameEntry*)PTR(p, 0x10))[S32(p, 0x11C)].name);
    fn_801345D8((char*)(p + 0x128), 0x20, "%s/race.%s", ((const NameEntry*)PTR(p, 0x10))[S32(p, 0x11C)].name, "m2v");
}

void fn_8024BB3C(u8* p) {
    s32 v = S32(p, 0x10) - 1;
    S32(p, 0x10) = v > 0 ? v : 0;
}

void fn_8024BB5C(u8* p) {
    s32 v = S32(p, 0x10) + 1;
    S32(p, 0x10) = v > 2 ? 2 : v;
}

const char* fn_8024BB7C(u8* p) { return lbl_802FA4B0[S32(p, 0x10)].name; }

void fn_8024BB98(u8* p) {
    s32 a;
    s32 b;
    ((VC*)PTR(p, 0x4C))->s38(&a, &b);
    ((StatsTable*)p)->Show(*(s32*)FindTrackEvent(a, b));
}

void fn_8024BBF4(u8* p) {
    VC* c = (VC*)PTR(p, 0x30);
    if (c) {
        s32 eve;
        s32 track;
        c->s38(&track, &eve);
        u8* ev = (u8*)FindTrackEvent(track, eve);
        S32(p, 0x34) = S32(ev, 0x0);
        const char* name = ((VN*)PTR(ev, 0x20))->v14("friendly_name");
        CopyName(name, 100, (char*)(p + 0x38));
    } else {
        S32(p, 0x34) = -1;
        U8(p, 0x38) = 0;
    }
}

void fn_8024BCC0(u8* p) {
    VC* c = (VC*)PTR(p, 0x30);
    if (c) {
        s32 eve;
        s32 track;
        c->s38(&track, &eve);
        u8* ev = (u8*)FindTrackEvent(track, eve);
        S32(p, 0x34) = S32(ev, 0x0);
        const char* name = ((VN*)PTR(ev, 0x20))->v14("friendly_name");
        CopyName(name, 100, (char*)(p + 0x38));
    } else {
        S32(p, 0x34) = -1;
        U8(p, 0x38) = 0;
    }
}



}  // extern "C"
