// Grade schedule (0x8016071C-0x80160B14): the race grades' score thresholds for the current event, from the
// event's own grade table, else grades.val, else built-in defaults. Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
void fn_802E30C8(const char* fmt, ...);  // debug print
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* t);
const char* fn_80204EA0(const char* name);  // tuning string
int fn_8027DED0(void);
}

void* operator new(size_t size, const char* tag);

struct EventDesc {
    s32 event;  // offset 0x0
};

struct Event {
    u8 pad0[0x20];
    ValueNode* vals;  // offset 0x20
};

extern "C" EventDesc* fn_801769CC(void);
Event* GetEvent(int id);
const char* GetGradeDisplayName(int grade);

class Grades {
public:
    Grades(ValueNode* table);
    ~Grades(void) {}
    int Find(int score);
    int GetThreshold(int grade);

    s32 thresholds[5];  // offset 0x0
    s32 passing;        // offset 0x14
};

struct Schedule {
    s32 score[5];
};

const Schedule lbl_802F9490 = {{8000000, 5000000, 2000000, 1000000, 500000}};

ModuleTag lbl_8055E2B0;
UnkStaticFFFF lbl_8055E2B8;
Grades* lbl_8055E2BC;
Schedule lbl_8042FD60;

static inline int HasNode(ValueNode* n, const char* name) { return n->FindChild(name) != 0; }

extern "C" {

void fn_8016071C(ValueNode* t) {
    int i;
    lbl_8055E2BC = new ("gradeschedule") Grades(t);
    for (i = 0; i < 5; i++) {
        lbl_8042FD60.score[i] = t->GetInt(GetGradeDisplayName(i), -1);
    }
}

void fn_801607BC(void) {
    int i;
    for (i = 0; i < 5; i++) {
        lbl_8042FD60.score[i] = lbl_802F9490.score[i];
    }
}

void fn_801607F8(void) {
    if (lbl_8055E2BC) {
        delete lbl_8055E2BC;
        lbl_8055E2BC = 0;
    }
    lbl_8042FD60.score[0] = -1;
    lbl_8042FD60.score[1] = -1;
    lbl_8042FD60.score[2] = -1;
    lbl_8042FD60.score[3] = -1;
    lbl_8042FD60.score[4] = -1;
}

void fn_80160850(void) {
    int ok;
    const char* file;
    const char* key;
    ValueNode* ev;
    const char* name;
    fn_801607F8();
    file = "grades.val";
    ok = 0;
    key = "grades";
    ev = GetEvent(fn_801769CC()->event)->vals;
    name = fn_80204EA0("grade_table");
    if (HasNode(ev, name)) {
        fn_8016071C(ev->FindChild(name));
        ok = 1;
    } else {
        ValueTable* vt = fn_80131808(file);
        if (vt) {
            if (vt->HasChild(key)) {
                fn_8016071C(vt->FindChild(key));
                ok = 1;
            } else {
                fn_802E30C8("GradeSchedule: couldn't find the grade table (\"%s\") in %s", key, file);
            }
            fn_80131818(vt);
        } else {
            fn_802E30C8("GradeSchedule: couldn't load %s", file);
        }
    }
    if (!ok) {
        fn_802E30C8("GradeSchedule: initializing with default schedule");
        fn_801607BC();
    }
}

int fn_80160998(int grade) {
    if (lbl_8055E2BC) {
        return lbl_8055E2BC->GetThreshold(grade);
    }
    return lbl_8042FD60.score[grade];
}

int fn_801609C4(int score) {
    int i;
    if (lbl_8055E2BC) {
        return lbl_8055E2BC->Find(score);
    }
    for (i = 0; i < 5; i++) {
        if (lbl_8042FD60.score[i] >= 0 && score >= lbl_8042FD60.score[i]) {
            return i;
        }
    }
    return 4;
}

int fn_80160A74(void) {
    if (lbl_8055E2BC) {
        return lbl_8055E2BC->passing;
    }
    return 2;
}

int fn_80160AB4(void);

int fn_80160A90(void) { return fn_80160998(fn_80160AB4()); }

int fn_80160AB4(void) {
    int g = fn_8027DED0();
    return g != -1 ? g : fn_80160A74();
}

void fn_80160AE4(void) { fn_801607F8(); }

}  // extern "C"
