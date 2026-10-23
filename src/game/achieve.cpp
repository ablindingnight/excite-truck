// Achievements (0x8001EF38-0x8001FD7C): eight categories with level schedules from
// achievements.val, per-profile totals saved under 'ACHV', and race/session accumulators with
// per-model stats. The "Achievements"/"AchievementsGetValue" prefixes come from its messages; the
// other names are guesses.
#include <string.h>
#include "game/common.h"

extern "C" {
int strcasecmp(const char* a, const char* b);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
const char* fn_80131234(const char* key);  // localize
ValueTable* fn_80131810(const char* name);
void fn_80131818(ValueTable* table);
void fn_802E6BD8(u32 code, int, void* buf, u32 size, void (*cb)(int));
int fn_801E8838(void);
}

const char* GetBaseChassisName(const char* chassis);
void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

int IdentGetCurrentSlot(void);
void IdentMarkSlotUsed(void);
void IdentRegisterCallback(void (*cb)(int slot, int event));
s32 GetMaxExcitePoints(void);

#define NUM_CATEGORIES 8
#define NUM_MODELS 50
#define NUM_PROFILES 4

struct AchLevel {
    s32 threshold;
    const char* model;
};

class AchSchedule {
public:
    AchSchedule(void) {
        levels = NULL;
        count = 0;
    }
    ~AchSchedule(void) {
        if (levels != NULL) {
            delete[] levels;
            levels = NULL;
        }
        count = 0;
    }

    AchLevel* levels;
    s32 count;
};

struct AchCategory {
    const char* name;
    const char* loc_key;
    const char* loc_name;
    AchSchedule* schedule;
};

struct AchModelStat {
    char name[0x20];
    s32 values[2];
};

struct AchStats {
    s32 totals[NUM_CATEGORIES];       // offset 0x0
    u8 unk20[0x30];                   // offset 0x20
    AchModelStat models[NUM_MODELS];  // offset 0x50
};

struct AchQuery {
    s32 category;
    s32 profile;
    s32 total;
};

ModuleTag lbl_8055D608;
ValueTable* lbl_8055D610;
u8 lbl_8055D614;
s32 lbl_8055D618;
static AchStats s_profiles[NUM_PROFILES];
static AchStats s_session;
static AchStats s_race;

static AchCategory lbl_802FDF30[NUM_CATEGORIES] = {
    {"score", "Achievements:LIFETIME_SCORE", NULL, NULL},
    {"win", "Achievements:LIFETIME_WINS", NULL, NULL},
    {"AAA", "Achievements:LIFETIME_AAAS", NULL, NULL},
    {"air", "Achievements:AIR", NULL, NULL},
    {"tree_run", "Achievements:TREE_RUN", NULL, NULL},
    {"drift", "Achievements:DRIFT", NULL, NULL},
    {"rings", "Achievements:RINGS", NULL, NULL},
    {"crash", "Achievements:CRASH", NULL, NULL},
};

void AchRegister(void);
void AchLoad(void);
void AchAddValue(int category, s32 value);
s32 AchGetValue(int category, int slot);
const char* AchGetCategoryName(int category);
int AchGetLevel(int category, s32 value);
s32 AchGetLevelThreshold(int category, int level);
s32 AchGetLevelCount(int category);
const char* AchGetLevelModel(int category, int level);
void AchQueryValue(AchQuery* q);
void AchCommitRace(void);
void AchClearRace(void);
void AchCommitSession(void);
s32 AchGetModelStat(int category, const char* model);
void AchAddModelStat(int category, const char* model, s32 value);
void AchClearSession(void);
void AchFree(void);

void AchievementsInit(void) {
    AchRegister();
    AchLoad();
}

void AchievementsAdd(int category, s32 value) {
    AchAddValue(category, value);
}

s32 AchievementsGetForSlot(int category, int slot);

s32 AchievementsGet(int category) {
    return AchievementsGetForSlot(category, IdentGetCurrentSlot());
}

s32 AchievementsGetForSlot(int category, int slot) {
    return AchGetValue(category, slot);
}

const char* AchievementsGetCategoryName(int category) {
    return AchGetCategoryName(category);
}

int AchievementsGetLevel(int category, s32 value) {
    return AchGetLevel(category, value);
}

s32 AchievementsGetLevelThreshold(int category, int level) {
    return AchGetLevelThreshold(category, level);
}

s32 AchievementsGetLevelCount(int category) {
    return AchGetLevelCount(category);
}

const char* AchievementsGetLevelModel(int category, int level) {
    return AchGetLevelModel(category, level);
}

void AchievementsQuery(AchQuery* q) {
    AchQueryValue(q);
}

void AchievementsCommitRace(void) {
    AchCommitRace();
}

void AchievementsClearRace(void) {
    AchClearRace();
}

void AchievementsCommitSession(void) {
    AchCommitSession();
}

s32 AchievementsGetModelStat(int category, const char* model) {
    return AchGetModelStat(category, model);
}

void AchievementsAddModelStat(int category, const char* model, s32 value) {
    AchAddModelStat(category, model, value);
}

void AchievementsClearSession(void) {
    AchClearSession();
}

void AchievementsShutdown(void) {
    AchFree();
    AchClearSession();
}

int AchFindCategory(const char* name);

void AchLoad(void) {
    AchCategory* cat;
    ValueNode* node;
    ValueNode* child;
    AchSchedule* sched;
    u32 i;
    int j;
    int idx;

    for (i = 0, cat = lbl_802FDF30; i < NUM_CATEGORIES; i++, cat++) {
        cat->loc_name = fn_80131234(cat->loc_key);
    }

    lbl_8055D610 = fn_80131810("achievements.val");
    if (lbl_8055D610 == NULL) {
        return;
    }

    for (i = 0; (node = lbl_8055D610->GetChild(i)) != NULL; i++) {
        if (strcasecmp(node->GetName(), "achievement_schedule") != 0) {
            continue;
        }
        idx = AchFindCategory(node->GetValue("name"));
        if (idx >= 0 && idx < NUM_CATEGORIES) {
            if ((u32)idx < NUM_CATEGORIES) {
                sched = new ("AchievementSchedule") AchSchedule;
                for (j = 0; (child = node->GetChildAt(j)) != NULL; j++) {
                    if (strcasecmp(child->GetName(), "level") == 0) {
                        sched->count++;
                    }
                }
                if (sched->count == 0) {
                    fn_802E317C("Achievements: achievements table %s has no \"level\" tables!", node->GetName());
                }
                sched->levels = new ("schedule_item") AchLevel[sched->count];
                sched->count = 0;
                for (j = 0; (child = node->GetChildAt(j)) != NULL; j++) {
                    if (strcasecmp(child->GetName(), "level") == 0) {
                        sched->levels[sched->count].threshold = child->GetIntValue("threshhold");
                        sched->levels[sched->count].model = child->GetValue("model_name");
                        sched->count++;
                    }
                }
                lbl_802FDF30[idx].schedule = sched;
            }
        } else {
            const char* name = node->GetValue("name");

            fn_802E30C8("Achievements: table %s has invalid category name %s", node->GetName(), name);
        }
    }
}

void AchFree(void) {
    AchCategory* cat;
    u32 i;

    if (lbl_8055D610 == NULL) {
        return;
    }
    for (i = 0, cat = lbl_802FDF30; i < NUM_CATEGORIES; i++, cat++) {
        if (cat->schedule != NULL) {
            delete cat->schedule;
            cat->schedule = NULL;
        }
    }
    fn_80131818(lbl_8055D610);
    lbl_8055D610 = NULL;
}

AchCategory* AchGetCategory(int category) {
    return &lbl_802FDF30[category];
}

const char* AchGetCategoryName(int category) {
    return AchGetCategory(category)->loc_name;
}

const char* AchGetLevelModel(int category, int level) {
    AchSchedule* sched = AchGetCategory(category)->schedule;

    if (sched != NULL) {
        if (level > 0 && level <= sched->count) {
            return sched->levels[level - 1].model;
        }
        return NULL;
    }
    return NULL;
}

int AchFindCategory(const char* name) {
    AchCategory* cat;
    u32 i;

    for (i = 0, cat = lbl_802FDF30; i < NUM_CATEGORIES; i++, cat++) {
        if (strcasecmp(cat->name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void AchSaveEvent(int event) {
    switch (event) {
    case 0:
    case 2:
        memset(s_profiles, 0, sizeof(s_profiles));
        break;
    case 3:
        break;
    }
}

void AchIdentEvent(int slot, int event) {
    switch (event) {
    case 0:
        fn_802E30C8("Achievements: Identity called to say that %d was cleared", slot);
        memset(&s_profiles[slot], 0, sizeof(AchStats));
        break;
    }
}

void AchRegister(void) {
    fn_802E6BD8('ACHV', 3, s_profiles, sizeof(s_profiles), AchSaveEvent);
    IdentRegisterCallback(AchIdentEvent);
    AchClearSession();
}

void AchClearSession(void) {
    memset(&s_session, 0, sizeof(AchStats));
    memset(&s_race, 0, sizeof(AchStats));
}

void AchMerge(AchStats* dst, AchStats* src) {
    AchModelStat* s;
    AchModelStat* d;
    u32 i;
    u32 j;
    int found;
    int empty;

    for (i = 0; i < NUM_CATEGORIES; i++) {
        dst->totals[i] += src->totals[i];
    }
    for (i = 0, s = src->models; i < NUM_MODELS; i++, s++) {
        found = FALSE;
        empty = -1;
        for (j = 0, d = dst->models; !found && j < NUM_MODELS; j++, d++) {
            if (empty < 0 && d->name[0] == '\0') {
                empty = j;
            } else if (strcasecmp(d->name, s->name) == 0) {
                found = TRUE;
                d->values[0] += s->values[0];
                d->values[1] += s->values[1];
            }
        }
        if (!found) {
            memcpy(&dst->models[empty], s, sizeof(AchModelStat));
        }
    }
}

void AchCommitRace(void) {
    AchMerge(&s_session, &s_race);
    memset(&s_race, 0, sizeof(AchStats));
}

void AchClearRace(void) {
    memset(&s_race, 0, sizeof(AchStats));
}

void AchCommitSession(void) {
    AchStats* p = &s_profiles[IdentGetCurrentSlot()];

    AchMerge(p, &s_session);
    AchMerge(p, &s_race);
    AchClearSession();
    IdentMarkSlotUsed();
}

BOOL AchIsRecording(void);

void AchAddValue(int category, s32 value) {
    if (AchIsRecording()) {
        if (category == 0 && lbl_8055D614) {
            s32 total = AchGetValue(category, IdentGetCurrentSlot());
            s32 limited = value;

            if (lbl_8055D618 - total < value) {
                limited = lbl_8055D618 - total;
                fn_802E30C8("Achievements: limiting value %d to %d due to scoring limiter", value, limited);
            }
            value = limited;
        }
        s_race.totals[category] += value;
    }
}

s32 AchGetValue(int category, int slot) {
    if (slot >= 0 && (u32)slot < NUM_PROFILES) {
        return s_session.totals[category] + s_profiles[slot].totals[category];
    }
    fn_802E30C8("AchievementsGetValue: identity %d is out of range", slot);
    return 0;
}

void AchQueryValue(AchQuery* q) {
    int slot = IdentGetCurrentSlot();

    if (slot >= 0 && (u32)slot < NUM_PROFILES) {
        q->profile = s_profiles[slot].totals[q->category];
    } else {
        q->profile = 0;
    }
    q->total = q->profile + s_session.totals[q->category] + s_race.totals[q->category];
}

int AchGetLevel(int category, s32 value) {
    AchSchedule* sched;
    int i;

    if (category >= 0 && (u32)category < NUM_CATEGORIES) {
        sched = lbl_802FDF30[category].schedule;
        if (sched != NULL) {
            if (sched->levels == NULL) {
                return 0;
            }
            for (i = 0; i < sched->count; i++) {
                if (sched->levels[i].threshold > value) {
                    return i;
                }
            }
            return sched->count;
        }
    }
    return 0;
}

s32 AchGetLevelThreshold(int category, int level) {
    AchSchedule* sched;

    if (category >= 0 && (u32)category < NUM_CATEGORIES) {
        sched = lbl_802FDF30[category].schedule;
        if (sched != NULL && sched->levels != NULL && level <= sched->count) {
            if (level <= 0) {
                return -1;
            }
            return sched->levels[level - 1].threshold;
        }
    }
    return 0;
}

s32 AchGetLevelCount(int category) {
    AchSchedule* sched;

    if (category >= 0 && (u32)category < NUM_CATEGORIES) {
        sched = lbl_802FDF30[category].schedule;
        if (sched != NULL) {
            return sched->count;
        }
    }
    return 0;
}

static inline s32 AchSumModel(AchStats* stats, int category, const char* name) {
    s32 total = 0;
    u32 i;

    for (i = 0; i < NUM_MODELS; i++) {
        if (strcasecmp(stats->models[i].name, name) == 0) {
            total += stats->models[i].values[category];
        }
    }
    return total;
}

s32 AchGetModelStat(int category, const char* model) {
    const char* name = GetBaseChassisName(model);
    s32 total = 0;
    AchStats* p = &s_profiles[IdentGetCurrentSlot()];
    u32 i;

    for (i = 0; i < NUM_MODELS; i++) {
        if (strcasecmp(p->models[i].name, name) == 0) {
            total += p->models[i].values[category];
        }
    }
    for (i = 0; i < NUM_MODELS; i++) {
        if (strcasecmp(s_session.models[i].name, name) == 0) {
            total += s_session.models[i].values[category];
        }
    }
    for (i = 0; i < NUM_MODELS; i++) {
        if (strcasecmp(s_race.models[i].name, name) == 0) {
            total += s_race.models[i].values[category];
        }
    }
    return total;
}

void AchAddModelStat(int category, const char* model, s32 value) {
    const char* name;
    int found;
    int empty;
    u32 i;

    if (!AchIsRecording()) {
        return;
    }
    name = GetBaseChassisName(model);
    found = -1;
    empty = -1;
    for (i = 0; i < NUM_MODELS; i++) {
        if (empty < 0 && s_race.models[i].name[0] == '\0') {
            empty = i;
        } else if (strcasecmp(s_race.models[i].name, name) == 0) {
            found = i;
        }
    }
    if (found < 0) {
        char* dst;
        int n;
        dst = s_race.models[empty].name;
        n = sizeof(s_race.models[empty].name);

        while ((*dst++ = *name++) != '\0') {
            if (--n <= 0) {
                dst[-1] = '\0';
                break;
            }
        }
        s_race.models[empty].values[category] = value;
    } else {
        s_race.models[found].values[category] += value;
    }
}

BOOL AchIsRecording(void) {
    return fn_801E8838() == 0;
}

void AchSetScoreLimiter(u8 enable) {
    lbl_8055D614 = enable;
    lbl_8055D618 = GetMaxExcitePoints();
}
