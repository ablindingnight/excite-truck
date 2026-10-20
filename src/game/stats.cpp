// Stats and records (0x8001BA68-0x8001D7D8): per-identity statistics (best jump, drift, air spin...)
// and per-contest high score lists, kept in one 0x3E7D0-byte memory card chunk ('Stat') whose
// strings live in a small interned pool. Defaults come from stats.val. The "Stats:",
// "RealStatistic" and "IntStatistic" messages give the names; the rest are guesses.
#include <string.h>
#include "game/common.h"

extern "C" {
int strcasecmp(const char* a, const char* b);
void* memmove(void* dst, const void* src, size_t n);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
int fn_801345D8(char* buf, u32 size, const char* fmt, ...);  // snprintf
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* table);
const char* fn_80131234(const char* key);  // localize
void fn_802E6BD8(u32 code, int unk, void* buf, u32 size, void (*cb)(int));
void fn_80130978(char* buf, int size, f32 value);
void fn_801308E4(char* buf, int size, int unk, f32 value);
void fn_800122B4(s32 value, char* buf);
}

void* operator new(size_t size, const char* tag);

struct EventInfo {
    s32 id;           // offset 0x0
    s32 track;        // offset 0x4
    s32 type;         // offset 0x8
    s32 laps;         // offset 0xC
    u8 unk10[0x10];
    ValueNode* node;  // offset 0x20
};

struct VehicleInfo {
    s32 index;  // offset 0x0
};

struct VehicleColor {
    s32 body;  // offset 0x0
    s32 trim;  // offset 0x4
    u8 unk8;   // offset 0x8
};

struct Units {
    u8 unk0[0x10];
    f32 speed_scale;  // offset 0x10
};

EventInfo* GetEvent(int event);
extern "C" Units* lbl_8055E050;

VehicleInfo* GetPlayerVehicle(int slot);
VehicleColor* GetVehicleColor(VehicleInfo* v, int slot);
int IdentGetCurrentSlot(void);
void IdentSelectSlot(int slot);
void IdentMarkSlotUsed(void);
typedef void (*IdentCallback)(int slot, int event);
void IdentRegisterCallback(IdentCallback cb);

#define NUM_IDENTITIES 4
#define MAX_STATS_PER_IDENTITY 640
#define NUM_STAT_SLOTS 3
#define NUM_HS_LISTS 240
#define NUM_HIGH_SCORES 5
#define STRING_POOL_SIZE 0x1000

#define STF_DATA_TYPE_MASK 0x300
#define STF_DATA_TYPE_REAL 0x100
#define STF_DATA_TYPE_INT 0x200
#define STF_UNIT_MASK 0x1FC00
#define STF_COMPARE_MASK 0x60000
#define STF_COMPARE_LOWER 0x20000
#define STF_COMPARE_HIGHER 0x40000

#define COPY_STRING(dst_, src_, size_)                  \
    {                                                   \
        const char* src = (src_);                       \
        int n = (size_);                                \
        char* dst = (dst_);                             \
        while ((*dst++ = *src++) != '\0') {             \
            if (--n <= 0) {                             \
                dst[-1] = '\0';                         \
                break;                                  \
            }                                           \
        }                                               \
    }

struct StatValue {
    u32 value;    // offset 0x0
    u32 choice;   // offset 0x4, packed vehicle/color
    u8 set;       // offset 0x8
};

class StatEntry {
public:
    StatEntry(void) { type = 0; }
    BOOL Matches(u32 type, const char* name);

    u32 type;                         // offset 0x0
    const char* name;                 // offset 0x4
    StatValue values[NUM_STAT_SLOTS]; // offset 0x8
};

struct HighScore {
    const char* name;  // offset 0x0
    s32 score;         // offset 0x4
    s32 grade;         // offset 0x8
    f32 time;          // offset 0xC, tie-break
    u32 choice;        // offset 0x10
};

struct HighScoreList {
    const char* name;                      // offset 0x0
    s32 key;                               // offset 0x4
    HighScore entries[NUM_HIGH_SCORES];    // offset 0x8
    s32 count;                             // offset 0x6C
    u8 used;                               // offset 0x70
};

struct IdentityStats {
    StatEntry stats[MAX_STATS_PER_IDENTITY];  // offset 0x0
    s32 count;                                // offset 0x6E00
    HighScoreList lists[NUM_HS_LISTS];        // offset 0x6E04
};

struct StatsChunk {
    IdentityStats identities[NUM_IDENTITIES];  // offset 0x0
    HighScoreList lists[NUM_HS_LISTS];         // offset 0x36B10
    char strings[STRING_POOL_SIZE];            // offset 0x3D7D0
};

class StatList {
public:
    StatList(StatEntry* e, int n) {
        entries = e;
        max = n;
        count = 0;
    }
    ~StatList(void) {
        entries = NULL;
        count = 0;
        max = 0;
    }

    s32 Size(void) const { return count; }
    StatEntry* Add(void) { return &entries[count++]; }
    StatEntry* At(s32 i) { return &entries[i]; }

    StatEntry* entries;  // offset 0x0
    s32 count;           // offset 0x4
    s32 max;             // offset 0x8
};

class StringPool {
public:
    StringPool(char* buf, int n) {
        data = buf;
        size = n;
    }
    char* Intern(const char* s) {
        char* p;

        unk8 = 0;
        for (p = data; *p != '\0' && p - data < size; p++) {
            if (strcasecmp(p, s) == 0) {
                return p;
            }
            while (*p != '\0') {
                p++;
            }
        }
        {
            char* dst = p;
            const char* src = s;
            int n = size - (p - data) - 1;
            while ((*dst++ = *src++) != '\0') {
                if (--n <= 0) {
                    dst[-1] = '\0';
                    break;
                }
            }
        }
        return p;
    }

    char* data;  // offset 0x0
    s32 size;    // offset 0x4
    u8 unk8;     // offset 0x8
    u8 unk9[0xB];
};

struct StatTypeInfo {
    s32 type;             // offset 0x0
    const char* key;      // offset 0x4, default_stats key
    const char* loc_key;  // offset 0x8
};

struct VehicleChoice {
    s32 vehicle;  // offset 0x0
    s32 color;    // offset 0x4
    bool flag;    // offset 0x8
};

class Statistic {
public:
    Statistic(int identity, u32 type, s32 event, int slot);
    s32 GetEvent(void);
    u32* GetDefaultPtr(void);
    u32* GetValuePtr(void);
    void SetRaw(u32 value);
    BOOL HasValue(void);
    u32 GetType(void);
    BOOL HasDefault(void);
    char* Format(char* buf, int size);
    VehicleChoice GetChoice(void);
    BOOL IsBetterThan(Statistic* other);
    void Commit(void);
    void Revert(void);

    s32 identity;       // offset 0x0
    u32 type;           // offset 0x4
    s32 event;          // offset 0x8
    s32 slot;           // offset 0xC
    u8 has_default;     // offset 0x10
    u32 default_value;  // offset 0x14
    StatEntry* entry;   // offset 0x18
    u32 value;          // offset 0x1C
    u8 dirty;           // offset 0x20
};

class RealStatistic : public Statistic {
public:
    RealStatistic(int identity, u32 type, s32 event, int slot);
    f32 GetDefault(void);
    f32 GetValue(void);
    void SetValue(f32 v);
    BOOL IsNewRecord(f32 v);
};

class IntStatistic : public Statistic {
public:
    IntStatistic(int identity, u32 type, s32 event, int slot);
    s32 GetDefault(void);
    s32 GetValue(void);
    void SetValue(s32 v);
    BOOL IsNewRecord(s32 v);
};

const char* GetStatLabel(u32 type);
char* FormatStatValue(u32 type, s32 value, char* buf, int size);
u32 GetStatTypeFromIndex(u32 index);
void StatsInitImpl(void);
void StatsShutdownImpl(void);
u32 GetStatValue(int identity, u32 type, const char* name, int slot, u32 def);
s32 GetHighScoreCount(int identity, const char* name, s32 key);
HighScore* GetHighScore(int identity, const char* name, s32 key, int index);
int AddHighScore(int identity, s32 event, s32 key, const char* player, s32 score, s32 grade, f32 time);
int GetHighScoreRank(int identity, s32 event, s32 key, s32 score, f32 time);
void UnpackChoice(u32 packed, s32* vehicle, s32* color, bool* flag);
const char* GetContestName(s32 event);
StatEntry* GetOrCreateStat(int identity, u32 type, const char* name);
BOOL GetDefaultStat(u32 type, const char* name, u32* out);
u32 PackChoice(s32 vehicle, s32 color, s32 flag);
void MarkIdentityUsed(int identity);
int CompareStatValues(StatEntry* a, int a_slot, StatEntry* b, int b_slot);
void StatsSaveCallback(int event);
void StatsIdentityCallback(int identity, int event);
void StatsFirstLoad(void);

ModuleTag lbl_8055D5C0;
u8 lbl_8055D5C8;              // initialized
StatsChunk* lbl_8055D5CC;     // memory card chunk
StringPool* lbl_8055D5D0;     // string pool
ValueTable* lbl_8055D5D4;     // stats.val
u8 lbl_8055D5D8;              // first load pending
StatEntry lbl_80358628;       // returned when a stat has no contest
StatList* lbl_80358658[NUM_IDENTITIES];

void StatsUpdate(void) {}

const char* StatsGetLabel(u32 type) {
    return GetStatLabel(type);
}

char* StatsFormatValue(u32 type, u32 value, char* buf, int size) {
    return FormatStatValue(type, value, buf, size);
}

u32 StatsGetTypeFromIndex(u32 index) {
    return GetStatTypeFromIndex(index);
}

void StatsInit(void) {
    lbl_8055D5C8 = 1;
    StatsInitImpl();
}

void StatsShutdown(void) {
    lbl_8055D5C8 = 0;
    StatsShutdownImpl();
}

u32 StatsGetValue(int identity, u32 type, s32 event, int slot, u32 def) {
    const char* name = GetContestName(event);

    if (name != NULL) {
        return GetStatValue(identity, type, name, slot, def);
    }
    return def;
}

s32 StatsGetHighScoreCount(int identity, s32 event, s32 key) {
    return GetHighScoreCount(identity, GetContestName(event), key);
}

HighScore* StatsGetHighScore(int identity, s32 event, s32 key, int index) {
    return GetHighScore(identity, GetContestName(event), key, index);
}

int StatsAddHighScore(int identity, s32 event, s32 key, const char* player, s32 score, s32 grade, f32 time) {
    return AddHighScore(identity, event, key, player, score, grade, time);
}

int StatsGetHighScoreRank(int identity, s32 event, s32 key, s32 score, f32 time) {
    return GetHighScoreRank(identity, event, key, score, time);
}

VehicleChoice StatsUnpackChoice(u32 packed) {
    VehicleChoice c;

    UnpackChoice(packed, &c.vehicle, &c.color, &c.flag);
    return c;
}

s32 Statistic::GetEvent(void) {
    return event;
}

u32* Statistic::GetDefaultPtr(void) {
    return &default_value;
}

u32* Statistic::GetValuePtr(void) {
    return &value;
}

void Statistic::SetRaw(u32 v) {
    value = v;
    dirty = 1;
}

Statistic::Statistic(int identity, u32 type, s32 event, int slot) {
    const char* name;

    this->identity = identity;
    this->slot = slot;
    this->type = type;
    this->event = event;
    name = GetContestName(event);
    if (name != NULL) {
        entry = GetOrCreateStat(this->identity, this->type, name);
        has_default = GetDefaultStat(this->type, name, &default_value);
    } else {
        entry = &lbl_80358628;
        has_default = 0;
    }
    value = entry->values[this->slot].value;
    dirty = 0;
}

BOOL Statistic::HasValue(void) {
    return entry->values[slot].set || dirty;
}

u32 Statistic::GetType(void) {
    return type;
}

BOOL Statistic::HasDefault(void) {
    return has_default;
}

char* Statistic::Format(char* buf, int size) {
    return FormatStatValue(type, value, buf, size);
}

VehicleChoice Statistic::GetChoice(void) {
    return StatsUnpackChoice(entry->values[slot].choice);
}

BOOL Statistic::IsBetterThan(Statistic* other) {
    return CompareStatValues(entry, slot, other->entry, other->slot) < 0;
}

void Statistic::Commit(void) {
    if (dirty && entry != &lbl_80358628) {
        VehicleInfo* v;
        VehicleColor* c;

        entry->values[slot].value = value;
        entry->values[slot].set = 1;
        v = GetPlayerVehicle(0);
        c = GetVehicleColor(v, 0);
        entry->values[slot].choice = PackChoice(v->index, c->trim, c->unk8);
        MarkIdentityUsed(identity);
    }
    dirty = 0;
}

void Statistic::Revert(void) {
    value = entry->values[slot].value;
    dirty = 0;
}

RealStatistic::RealStatistic(int identity, u32 type, s32 event, int slot) : Statistic(identity, type, event, slot) {}

f32 RealStatistic::GetDefault(void) {
    return *(f32*)GetDefaultPtr();
}

f32 RealStatistic::GetValue(void) {
    return *(f32*)GetValuePtr();
}

void RealStatistic::SetValue(f32 v) {
    SetRaw(*(u32*)&v);
}

BOOL RealStatistic::IsNewRecord(f32 v) {
    if (HasValue() || HasDefault()) {
        f32 best;
        u32 compare;

        if (HasValue()) {
            best = GetValue();
        } else {
            best = GetDefault();
        }
        u32 type = GetType();

        compare = type & STF_COMPARE_MASK;
        if (type == 0x20507) {
            s32 ev = GetEvent();
            if (ev >= 0) {
                EventInfo* e = ::GetEvent(ev);
                compare = STF_COMPARE_HIGHER;
                if (e->type == 0x180302) {
                    compare = STF_COMPARE_LOWER;
                }
            }
        }
        switch (compare) {
        default:
            fn_802E317C("RealStatistic: stat type %x has invalid compare type %x!", GetType(),
                        GetType() & STF_COMPARE_MASK);
        case STF_COMPARE_HIGHER:
            return v > best;
        case STF_COMPARE_LOWER:
            return v < best;
        }
    }
    return 1;
}

IntStatistic::IntStatistic(int identity, u32 type, s32 event, int slot) : Statistic(identity, type, event, slot) {}

s32 IntStatistic::GetDefault(void) {
    return *(s32*)GetDefaultPtr();
}

s32 IntStatistic::GetValue(void) {
    return *(s32*)GetValuePtr();
}

void IntStatistic::SetValue(s32 v) {
    SetRaw(v);
}

BOOL IntStatistic::IsNewRecord(s32 v) {
    if (HasValue() || HasDefault()) {
        s32 best;

        if (HasValue()) {
            best = GetValue();
        } else {
            best = GetDefault();
        }
        switch (GetType() & STF_COMPARE_MASK) {
        default:
            fn_802E317C("IntStatistic: stat type %x has invalid compare type %x!", GetType(),
                        GetType() & STF_COMPARE_MASK);
        case STF_COMPARE_HIGHER:
            return v > best;
        case STF_COMPARE_LOWER:
            return v < best;
        }
    }
    return 1;
}

BOOL MatchContestName(const char* a, const char* b) {
    BOOL match = 0;

    if (strcasecmp(a, b) == 0 || strcasecmp(a, "*") == 0 || strcasecmp(b, "*") == 0) {
        match = 1;
    }
    return match;
}

BOOL StatEntry::Matches(u32 type, const char* name) {
    return this->type == type && MatchContestName(this->name, name);
}

StatEntry* FindStat(int identity, u32 type, const char* name) {
    int i;

    for (i = 0; i < lbl_80358658[identity]->Size(); i++) {
        StatEntry* e = lbl_80358658[identity]->At(i);
        if (e->Matches(type, name)) {
            return e;
        }
    }
    return NULL;
}

StatEntry* CreateStat(int identity, u32 type, const char* name) {
    StatEntry* e;

    if (lbl_80358658[identity]->count + 1 > lbl_80358658[identity]->max) {
        fn_802E317C("Stats: create_stat couldn't add a stat to identity %d; try increasing MAX_STATS_PER_IDENTITY "
                    "from %d",
                    identity, MAX_STATS_PER_IDENTITY);
    }
    e = lbl_80358658[identity]->Add();
    e->type = type;
    e->name = lbl_8055D5D0->Intern(name);
    e->values[0].set = 0;
    e->values[1].set = 0;
    e->values[2].set = 0;
    lbl_8055D5CC->identities[identity].count = lbl_80358658[identity]->count;
    return e;
}

void StatsCheck(void) {}

void StatsCheck2(void) {
    StatsCheck();
}

void StatsCheck3(void) {
    StatsCheck2();
}

static const StatTypeInfo lbl_802ED7B8[] = {
    {0x40901, "jump", "Records:JUMP"},
    {0x40902, "drift", "Records:DRIFT"},
    {0x48203, "air_spin", "Records:AIR_SPIN"},
    {0x50204, "jump_combo", "Records:JUMP_COMBO"},
    {0x22205, NULL, "Records:FINISH"},
    {0x41206, "close_call", "Records:TREE_RUN"},
    {0x20507, NULL, "Records:RACE_TIME"},
    {0, NULL, NULL},
};

const StatTypeInfo* GetStatTypeInfo(s32 type) {
    const StatTypeInfo* info;

    for (info = lbl_802ED7B8; info->type != 0; info++) {
        if (info->type == type) {
            return info;
        }
    }
    fn_802E317C("Stats: get_stat_type_info called with invalid type %d", type);
    return NULL;
}

HighScoreList* FindHighScoreList(int identity, const char* name, s32 key, BOOL create) {
    HighScoreList* empty;
    int i;
    HighScoreList* list;

    if (identity < 0) {
        list = lbl_8055D5CC->lists;
    } else {
        list = lbl_8055D5CC->identities[identity].lists;
    }
    empty = NULL;
    for (i = 0; i < NUM_HS_LISTS; i++, list++) {
        if (!list->used) {
            empty = list;
        } else if (list->key == key && list->name != NULL && strcasecmp(name, list->name) == 0) {
            return list;
        }
    }
    if (!create) {
        return NULL;
    }
    fn_802E30C8("Stats: creating a new high score list for %s", name);
    if (empty == NULL) {
        fn_802E317C("Stats: didn't find an empty slot for %s!", name);
    }
    memset(empty, 0, sizeof(HighScoreList));
    empty->name = lbl_8055D5D0->Intern(name);
    empty->key = key;
    empty->used = 1;
    return empty;
}

const char* GetStatLabel(u32 type) {
    return fn_80131234(GetStatTypeInfo(type)->loc_key);
}

char* FormatStatValue(u32 type, s32 value, char* buf, int size) {
    union {
        s32 i;
        f32 f;
    } u;

    u.i = value;
    switch (type & STF_DATA_TYPE_MASK) {
    case STF_DATA_TYPE_REAL: {
        f32 v = u.f;

        switch (type & STF_UNIT_MASK) {
        case 0x400:
            fn_80130978(buf, size, v);
            break;
        case 0x800:
            fn_801308E4(buf, size, 1, v);
            break;
        default:
            fn_802E317C("stats: unit type (%x) is not value with data type STF_DATA_TYPE_REAL", type & STF_UNIT_MASK);
        case 0x4000:
            fn_801345D8(buf, size, "%.1f", v);
            break;
        }
        break;
    }
    case STF_DATA_TYPE_INT:
        switch (type & STF_UNIT_MASK) {
        case 0x1000:
            fn_801345D8(buf, size, "%d %s", value, value == 1 ? "Time" : "Times");
            break;
        case 0x2000:
            fn_800122B4(value, buf);
            break;
        case 0x8000:
            fn_801345D8(buf, size, "%d%s", value * 360, fn_80131234("Records:AIR_SPIN_UNITS") + 1);
            break;
        case 0x10000:
            fn_801345D8(buf, size, "%d %s", value, fn_80131234("Records:JUMP_COMBO_UNITS") + 1);
            break;
        default:
            fn_802E317C("stats: unit type (%x) is not value with data type STF_DATA_TYPE_INT", type & STF_UNIT_MASK);
        case 0x4000:
            fn_801345D8(buf, size, "%d", value);
            break;
        }
        break;
    default:
        fn_802E317C("stats: unknown data type field (%x)", type & STF_DATA_TYPE_MASK);
        fn_801345D8(buf, size, "INVALID_DATA_TYPE");
        break;
    }
    return buf;
}

u32 GetStatTypeFromIndex(u32 index) {
    const StatTypeInfo* info;

    for (info = lbl_802ED7B8; info->type != 0; info++) {
        if ((info->type & 0xFF) == index) {
            return *(u32*)&info->type;
        }
    }
    fn_802E317C("Stats: get_from_index called with invalid index %d", index);
    return 0;
}

void StatsInitImpl(void) {
    int i;

    fn_802E30C8("Stats: allocating %d bytes for memcard chunk", sizeof(StatsChunk));
    lbl_8055D5CC = (StatsChunk*)operator new(sizeof(StatsChunk), "StatsMC");
    memset(lbl_8055D5CC, 0, sizeof(StatsChunk));
    for (i = 0; i < NUM_IDENTITIES; i++) {
        lbl_80358658[i] =
            new ("stats_array") StatList(lbl_8055D5CC->identities[i].stats, MAX_STATS_PER_IDENTITY);
    }
    lbl_8055D5D0 = new ("stats_strings") StringPool(lbl_8055D5CC->strings, STRING_POOL_SIZE);
    fn_802E6BD8('Stat', 7, lbl_8055D5CC, sizeof(StatsChunk), StatsSaveCallback);
    IdentRegisterCallback(StatsIdentityCallback);
    lbl_8055D5D4 = fn_80131808("stats.val");
    lbl_8055D5D8 = 1;
}

void StatsShutdownImpl(void) {
    int i;

    fn_80131818(lbl_8055D5D4);
    delete lbl_8055D5D0;
    for (i = 0; i < NUM_IDENTITIES; i++) {
        StatList* list = lbl_80358658[i];
        if (list != NULL) {
            list->~StatList();
            operator delete(list);
        }
    }
    delete lbl_8055D5CC;
    lbl_8055D5CC = NULL;
}

BOOL GetDefaultStat(u32 type, const char* name, u32* out) {
    const StatTypeInfo* info;
    ValueNode* node;
    int i;

    if (name == NULL) {
        return 0;
    }
    info = GetStatTypeInfo(type);
    if (info->key == NULL) {
        return 0;
    }
    for (i = 0; (node = lbl_8055D5D4->GetChild(i)) != NULL; i++) {
        if (strcasecmp(node->GetName(), "default_stats") == 0 &&
            strcasecmp(node->GetString("contest_name", ""), name) == 0 && node->Has(info->key)) {
            break;
        }
    }
    if (node == NULL) {
        node = lbl_8055D5D4->FindChild("default_default_stats");
    }
    if (out != NULL) {
        switch (type & STF_DATA_TYPE_MASK) {
        case STF_DATA_TYPE_INT:
            *out = node->GetIntValue(info->key);
            break;
        case STF_DATA_TYPE_REAL: {
            f32 v = node->GetFloat(info->key);
            switch (type & STF_UNIT_MASK) {
            case 0x800:
                v = v / lbl_8055E050->speed_scale;
                break;
            }
            *out = *(u32*)&v;
            break;
        }
        }
    }
    return 1;
}

StatEntry* GetOrCreateStat(int identity, u32 type, const char* name) {
    StatEntry* e;

    StatsCheck2();
    e = FindStat(identity, type, name);
    if (e == NULL) {
        e = CreateStat(identity, type, name);
    }
    return e;
}

u32 GetStatValue(int identity, u32 type, const char* name, int slot, u32 def) {
    StatEntry* e;

    StatsCheck3();
    e = FindStat(identity, type, name);
    if (e == NULL || !e->values[slot].set) {
        return def;
    }
    return e->values[slot].value;
}

s32 GetHighScoreCount(int identity, const char* name, s32 key) {
    if (name != NULL) {
        HighScoreList* list = FindHighScoreList(identity, name, key, 0);
        if (list != NULL) {
            return list->count;
        }
    }
    return 0;
}

HighScore* GetHighScore(int identity, const char* name, s32 key, int index) {
    if (name != NULL) {
        HighScoreList* list = FindHighScoreList(identity, name, key, 0);
        if (list != NULL && index < list->count) {
            return &list->entries[index];
        }
    }
    return NULL;
}

int FindHighScorePos(HighScoreList* list, s32 event, s32 score, f32 time) {
    EventInfo* ev = GetEvent(event);
    int i;

    for (i = 0; i < list->count; i++) {
        if (list->entries[i].name == NULL) {
            break;
        }
        if (list->entries[i].score < score) {
            break;
        }
        if (score == list->entries[i].score) {
            if (ev->type == 0x180302) {
                if (list->entries[i].time > time) {
                    break;
                }
            } else if (list->entries[i].time < time) {
                break;
            }
        }
    }
    return i;
}

int GetHighScoreRank(int identity, s32 event, s32 key, s32 score, f32 time) {
    const char* name = GetContestName(event);

    if (name != NULL) {
        return FindHighScorePos(FindHighScoreList(identity, name, key, 1), event, score, time);
    }
    return NUM_HIGH_SCORES;
}

int AddHighScore(int identity, s32 event, s32 key, const char* player, s32 score, s32 grade, f32 time) {
    const char* name = GetContestName(event);

    if (name != NULL) {
        HighScoreList* list = FindHighScoreList(identity, name, key, 1);
        int pos = FindHighScorePos(list, event, score, time);

        if (pos < NUM_HIGH_SCORES) {
            HighScore hs;
            VehicleInfo* v;
            VehicleColor* c;

            hs.name = lbl_8055D5D0->Intern(player);
            hs.score = score;
            hs.grade = grade;
            hs.time = time;
            v = GetPlayerVehicle(0);
            c = GetVehicleColor(v, 0);
            hs.choice = PackChoice(v->index, c->trim, c->unk8);
            list->count = list->count > NUM_HIGH_SCORES - 1 ? NUM_HIGH_SCORES - 1 : list->count;
            memmove(&list->entries[pos] + 1, &list->entries[pos], (list->count - pos) * sizeof(HighScore));
            list->entries[pos] = hs;
            list->count++;
            return 1;
        }
        return 0;
    }
    return 0;
}

u32 PackChoice(s32 vehicle, s32 color, s32 flag) {
    return (flag << 16) | (color << 8) | vehicle;
}

void UnpackChoice(u32 packed, s32* vehicle, s32* color, bool* flag) {
    *vehicle = packed & 0xFF;
    *color = (packed >> 8) & 0xFF;
    *flag = ((packed >> 16) & 0xFF) != 0;
}

const char* GetContestName(s32 event) {
    return GetEvent(event)->node->GetString("name", NULL);
}

void MarkIdentityUsed(int identity) {
    int cur = IdentGetCurrentSlot();

    IdentSelectSlot(identity);
    IdentMarkSlotUsed();
    IdentSelectSlot(cur);
}

int CompareStatValues(StatEntry* a, int a_slot, StatEntry* b, int b_slot) {
    StatValue* av = &a->values[a_slot];

    if (av->set) {
        StatValue* bv = &b->values[b_slot];
        if (bv->set) {
            switch (a->type & STF_DATA_TYPE_MASK) {
            case STF_DATA_TYPE_REAL:
                switch (a->type & STF_COMPARE_MASK) {
                case STF_COMPARE_LOWER:
                    if (*(f32*)&av->value < *(f32*)&bv->value) {
                        return -1;
                    }
                    return 1;
                case STF_COMPARE_HIGHER:
                    if (*(f32*)&av->value > *(f32*)&bv->value) {
                        return -1;
                    }
                    return 1;
                default:
                    fn_802E317C("compare_stats: invalid stat compare type %x", a->type & STF_COMPARE_MASK);
                    return 0;
                }
            case STF_DATA_TYPE_INT:
                switch (a->type & STF_COMPARE_MASK) {
                case STF_COMPARE_LOWER:
                    return (s32)av->value < (s32)bv->value ? -1 : 1;
                case STF_COMPARE_HIGHER:
                    return (s32)av->value > (s32)bv->value ? -1 : 1;
                default:
                    fn_802E317C("compare_stats: invalid stat compare type %x", a->type & STF_COMPARE_MASK);
                    return 0;
                }
            default:
                fn_802E317C("compare_stats: invalid stat data type %x", a->type & STF_DATA_TYPE_MASK);
                return 0;
            }
        }
        return -1;
    }
    return b->values[b_slot].set;
}

u32 PoolOffsetToPtr(u32 offset) {
    return offset + (u32)lbl_8055D5D0->data;
}

u32 PoolPtrToOffset(u32 ptr) {
    return ptr - (u32)lbl_8055D5D0->data;
}

typedef u32 (*NameFixup)(u32 name);

void FixupHighScoreLists(HighScoreList* lists, int n, NameFixup fix) {
    int i;

    for (i = 0; i < n; i++) {
        if (lists[i].used) {
            int j;

            lists[i].name = (const char*)fix((u32)lists[i].name);
            for (j = 0; j < lists[i].count; j++) {
                lists[i].entries[j].name = (const char*)fix((u32)lists[i].entries[j].name);
            }
        }
    }
}

void FixupNames(NameFixup fix) {
    int i;

    for (i = 0; i < NUM_IDENTITIES; i++) {
        IdentityStats* id = &lbl_8055D5CC->identities[i];
        int j;

        for (j = 0; j < id->count; j++) {
            id->stats[j].name = (const char*)fix((u32)id->stats[j].name);
        }
        FixupHighScoreLists(id->lists, NUM_HS_LISTS, fix);
    }
    FixupHighScoreLists(lbl_8055D5CC->lists, NUM_HS_LISTS, fix);
}

void StatsNamesToPtrs(void) {
    FixupNames(PoolOffsetToPtr);
}

void StatsNamesToOffsets(void) {
    FixupNames(PoolPtrToOffset);
}

static inline void SyncStatCounts(void) {
    int i;

    for (i = 0; i < NUM_IDENTITIES; i++) {
        lbl_80358658[i]->count = lbl_8055D5CC->identities[i].count;
    }
}

void StatsSaveCallback(int event) {
    switch (event) {
    case 0:
        break;
    case 1:
        StatsNamesToPtrs();
        SyncStatCounts();
        if (lbl_8055D5D8) {
            StatsFirstLoad();
            lbl_8055D5D8 = 0;
        }
        break;
    case 2:
        memset(lbl_8055D5CC, 0, sizeof(StatsChunk));
        SyncStatCounts();
        break;
    case 3:
        StatsNamesToOffsets();
        break;
    case 4:
    case 5:
        StatsNamesToPtrs();
        break;
    }
}

void StatsIdentityCallback(int identity, int event) {
    switch (event) {
    case 0:
        fn_802E30C8("Stats: Identity called to say that %d was cleared", identity);
        memset(&lbl_8055D5CC->identities[identity], 0, sizeof(IdentityStats));
        lbl_80358658[identity]->count = 0;
        break;
    }
}

void StatsFirstLoad(void) {}
