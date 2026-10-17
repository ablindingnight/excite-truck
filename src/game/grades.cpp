// Race grades S-D (0x8001D7D8-0x8001DA5C): names, display names, and per-grade score thresholds
// read from a .val table. Names are guesses.
#include <string.h>
#include "game/common.h"

extern "C" int strcasecmp(const char* a, const char* b);

#define NUM_GRADES 5

const char* const lbl_802ED818[NUM_GRADES] = {"S", "A", "B", "C", "D"};
const char* const lbl_802ED82C[NUM_GRADES] = {"\\GRADE_S", "A", "B", "C", "D"};

class Grades {
public:
    Grades(ValueNode* table);
    void Load(ValueNode* table);
    int Find(int score);
    int GetThreshold(int grade);
    void Reset(void);

    s32 thresholds[NUM_GRADES];  // offset 0x0
    s32 passing;                 // offset 0x14
};

ModuleTag lbl_8055D5E0;

const char* GetGradeDisplayName(int grade) {
    return lbl_802ED82C[grade];
}

int FindGrade(const char* name) {
    int i;

    for (i = 0; i < NUM_GRADES; i++) {
        if (lbl_802ED818[i] != NULL && strcmp(lbl_802ED818[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

void Grades::Reset(void) {
    thresholds[0] = -1;
    thresholds[1] = -1;
    thresholds[2] = -1;
    thresholds[3] = -1;
    thresholds[4] = -1;
}

Grades::Grades(ValueNode* table) {
    Load(table);
}

void Grades::Load(ValueNode* table) {
    const char* name;
    int i;

    passing = -1;
    name = table->GetString("passing_grade", "default_bad_grade");
    for (i = 0; i < NUM_GRADES; i++) {
        if (strcasecmp(lbl_802ED818[i], name) == 0) {
            passing = i;
        }
    }
    if (passing == -1) {
        passing = 2;
    }
    i = NUM_GRADES;
    while (i-- > 0) {
        thresholds[i] = table->GetInt(lbl_802ED818[i], -1);
    }
    thresholds[4] = 0;
}

int Grades::Find(int score) {
    int i;

    for (i = 0; i < NUM_GRADES; i++) {
        if (thresholds[i] >= 0 && thresholds[i] <= score) {
            return i;
        }
    }
    return -1;
}

int Grades::GetThreshold(int grade) {
    return thresholds[grade];
}
