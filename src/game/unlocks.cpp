// Unlockables (0x80018B70-0x80019864): the "schedule" of things unlocked by Excite points and the
// special chassis unlocked by races run / AAA grades, from common.res/unlock.val, plus the event
// paths that explain an unlock. Names are guesses except where the messages give them.
#include <string.h>
#include "game/common.h"

extern "C" {
int strcasecmp(const char* a, const char* b);
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* table);
void fn_80131234(const char* event_path);
void fn_80265DCC(void* base, int num, int size, int (*cmp)(const void*, const void*));
s32 fn_8027DEAC(void);
s32 fn_8001EF60(int);
s32 fn_8001EFC0(int kind, const char* chassis);
int fn_8027DE68(int);
}

// A counted array over storage it does not own; clearing it on destruction.
template <class T>
class ArrayRef {
public:
    ~ArrayRef(void) {
        data = NULL;
        count = 0;
        capacity = 0;
    }
    s32 Size(void) const { return count; }
    T* Add(void) { return &data[count++]; }

    T* data;       // offset 0x0
    s32 count;     // offset 0x4
    s32 capacity;  // offset 0x8
};

template <class T, int N>
class FixedArray : public ArrayRef<T> {
public:
    FixedArray(void) {
        this->data = storage;
        this->capacity = N;
        this->count = 0;
    }
    ~FixedArray(void) {}

    T storage[N];  // offset 0xC
};

struct Unlockable {
    const char* type;  // offset 0x0
    const char* name;  // offset 0x4
    s32 points;        // offset 0x8
    u8 unlocked;       // offset 0xC
    s32 order;         // offset 0x10
};

struct UnlockReq {
    s32 races_run;        // offset 0x0
    s32 aaas;             // offset 0x4
    const char* special;  // offset 0x8
};

struct ChassisUnlock {
    const char* name;  // offset 0x0
    UnlockReq req;     // offset 0x4
};

ModuleTag lbl_8055D580;
u8 lbl_8055D588;
ValueTable* lbl_8055D58C;
// Both lists live in wrapper objects; the registered dtors are the wrappers' implicit ones.
struct UnlockableTable {
    FixedArray<Unlockable, 32> list;
};
struct ChassisTableInner {
    FixedArray<ChassisUnlock, 32> list;
};
struct ChassisTable {
    ChassisTableInner inner;
};
UnlockableTable lbl_80357E0C;
ChassisTable lbl_803580A4;

s32 GetExcitePoints(void) {
    return fn_8027DEAC();
}

s32 GetCarPoints(void) {
    return fn_8001EF60(0);
}

int CompareUnlockables(const void* a, const void* b) {
    int d = ((const Unlockable*)a)->points - ((const Unlockable*)b)->points;

    if (d == 0) {
        return ((const Unlockable*)a)->order - ((const Unlockable*)b)->order;
    }
    return d;
}

void LoadUnlockables(void) {
    Unlockable* u;
    int order;
    int i;
    ValueNode* node;

    order = 0;
    for (i = 0;; i++) {
        node = lbl_8055D58C->GetChild(i);
        if (node == NULL) {
            break;
        }
        if (strcmp(node->GetName(), "schedule") == 0) {
            u = &lbl_80357E0C.list.data[lbl_80357E0C.list.count++];
            u->type = node->GetValue("type");
            u->name = node->GetValue("name");
            u->points = node->GetIntValue("points");
            u->unlocked = 0;
            u->order = order++;
        }
    }
    fn_80265DCC(lbl_80357E0C.list.data, lbl_80357E0C.list.count, sizeof(Unlockable), CompareUnlockables);
}

static inline int FindChassisUnlock(const char* name) {
    int i;

    for (i = 0; i < lbl_803580A4.inner.list.count; i++) {
        if (strcasecmp(lbl_803580A4.inner.list.data[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

static inline UnlockReq* FindUnlockReq(const char* name) {
    int i = FindChassisUnlock(name);

    return i < 0 ? NULL : &lbl_803580A4.inner.list.data[i].req;
}

void LoadChassisUnlocks(void) {
    ChassisUnlock* c;
    int idx;
    const char* base;
    ValueNode* node;
    int i;

    for (i = 0; (node = lbl_8055D58C->GetChild(i)) != NULL; i++) {
        if (strcasecmp(node->GetName(), "special_chassis") == 0) {
            base = node->GetValue("base_chassis");
            idx = FindChassisUnlock(base);
            if (idx < 0) {
                idx = lbl_803580A4.inner.list.Size();
                lbl_803580A4.inner.list.Add()->name = base;
            }
            c = &lbl_803580A4.inner.list.data[idx];
            c->req.races_run = node->GetIntValue("races_run");
            c->req.aaas = node->GetIntValue("AAAs");
            c->req.special = node->GetValue("special_chassis");
        }
    }
}

s32 GetCarPoints2(void) {
    return GetCarPoints();
}

s32 GetExcitePoints2(void) {
    return GetExcitePoints();
}

BOOL IsUnlocked(const char* type, const char* name) {
    s32 points;
    int i;

    if (lbl_8055D588) {
        return TRUE;
    }
    if (strcasecmp(type, "car") == 0) {
        points = GetCarPoints2();
    } else {
        points = GetExcitePoints2();
    }
    for (i = 0; i < lbl_80357E0C.list.count; i++) {
        if (strcasecmp(name, lbl_80357E0C.list.data[i].name) == 0 && strcasecmp(type, lbl_80357E0C.list.data[i].type) == 0) {
            Unlockable* u = &lbl_80357E0C.list.data[i];

            {
                BOOL unlocked = TRUE;

                if (u->points > points && !u->unlocked) {
                    unlocked = FALSE;
                }
                return unlocked;
            }
        }
    }
    return TRUE;
}

void UnlocksInit(void) {
    lbl_8055D58C = fn_80131808("common.res/unlock.val");
    LoadUnlockables();
    LoadChassisUnlocks();
    lbl_8055D588 = 0;
}

void UnlocksShutdown(void) {
    fn_80131818(lbl_8055D58C);
    lbl_8055D58C = NULL;
}

BOOL IsCarUnlocked(const char* name) {
    return IsUnlocked("car", name);
}

s32 GetMaxExcitePoints(void) {
    const char* type = "MAX_EXCITE_POINTS";
    int i;

    for (i = 0; i < lbl_80357E0C.list.count; i++) {
        if (strcasecmp(lbl_80357E0C.list.data[i].type, type) == 0) {
            return lbl_80357E0C.list.data[i].points;
        }
    }
    fn_802E317C("Unlockable: didn't find \"%s\" unlockable!", type);
    return 0;
}

Unlockable* GetUnlockable(int i) {
    if (i >= 0 && i < lbl_80357E0C.list.count) {
        return &lbl_80357E0C.list.data[i];
    }
    return NULL;
}

int GetUnlockableCount(void) {
    return lbl_80357E0C.list.count;
}

const char* GetSpecialChassis(const char* name) {
    UnlockReq* req = FindUnlockReq(name);

    if (req != NULL) {
        return req->special;
    }
    return NULL;
}

BOOL IsSpecialChassisUnlocked(const char* name) {
    UnlockReq* req = FindUnlockReq(name);
    BOOL races;
    BOOL grades;

    if (req != NULL) {
        if (lbl_8055D588) {
            return TRUE;
        }
        races = fn_8001EFC0(0, name) >= req->races_run;
        grades = fn_8001EFC0(1, name) >= req->aaas;
        if (races || grades) {
            return TRUE;
        }
        return FALSE;
    }
    return FALSE;
}

BOOL GetUnlockReq(const char* name, s32* races_run, s32* aaas) {
    UnlockReq* req = FindUnlockReq(name);

    if (req != NULL) {
        *races_run = req->races_run;
        *aaas = req->aaas;
        return TRUE;
    }
    return FALSE;
}

void ShowUnlockReason(const char* name) {
    UnlockReq* req = FindUnlockReq(name);
    BOOL races;

    if (req != NULL) {
        races = fn_8001EFC0(0, name) >= req->races_run;
        if (fn_8001EFC0(1, name) >= req->aaas) {
            fn_80131234("EventPath:CAR_UNLOCK_REASON_S_GRADES");
        } else if (races) {
            fn_80131234("EventPath:CAR_UNLOCK_REASON_RACES_RUN");
        } else {
            fn_80131234("EventPath:CONGRATULATIONS");
        }
    }
}

extern "C" BOOL fn_80019430(void) {
    return FALSE;
}

BOOL AllExtremeCarsUnlocked(void) {
    Unlockable* u;
    int i;

    for (i = 0; i < GetUnlockableCount(); i++) {
        u = GetUnlockable(i);
        if (strcasecmp(u->type, "CAR") == 0 && GetSpecialChassis(u->name) != NULL && !IsSpecialChassisUnlocked(u->name)) {
            return FALSE;
        }
    }
    return TRUE;
}

BOOL AllExtremeCarsUnlocked2(void) {
    return AllExtremeCarsUnlocked();
}

extern "C" BOOL fn_80019430(void);
BOOL IsCareerComplete(void);

BOOL IsExtremeChassisUnlocked(void) {
    BOOL unlocked;

    if (lbl_8055D588) {
        unlocked = TRUE;
    } else {
        unlocked = FALSE;
        if (fn_80019430() || AllExtremeCarsUnlocked2() || IsCareerComplete()) {
            unlocked = TRUE;
        }
    }
    return unlocked;
}

const char* GetAllExtremesChassis(void) {
    if (lbl_8055D58C != NULL && lbl_8055D58C->HasChild("other_unlockables")) {
        return lbl_8055D58C->FindChild("other_unlockables")->GetString("all_extremes_chassis", NULL);
    }
    return NULL;
}

void ShowExtremeUnlockReason(void) {
    if (AllExtremeCarsUnlocked()) {
        fn_80131234("EventPath:CAR_UNLOCK_REASON_ALL_EXTREME_CARS_UNLOCKED");
    } else {
        fn_80131234("EventPath:CAR_UNLOCK_REASON_CAREER_COMPLETED");
    }
}

BOOL IsCareerComplete(void) {
    BOOL complete;

    if (lbl_8055D588) {
        complete = TRUE;
    } else {
        complete = FALSE;
        if (fn_80019430() || fn_8027DE68(2)) {
            complete = TRUE;
        }
    }
    return complete;
}

const char* GetCareerCompletedChassis(void) {
    if (lbl_8055D58C != NULL && lbl_8055D58C->HasChild("other_unlockables")) {
        return lbl_8055D58C->FindChild("other_unlockables")->GetString("career_completed_chassis", NULL);
    }
    return NULL;
}

void ShowCareerCompletedReason(void) {
    fn_80131234("EventPath:CAR_UNLOCK_REASON_CAREER_COMPLETED");
}

void UnlockAll(void) {
    lbl_8055D588 = 1;
}

void ClearUnlockAll(void) {
    lbl_8055D588 = 0;
}

u8 IsUnlockAll(void) {
    return lbl_8055D588;
}
