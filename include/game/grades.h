#ifndef GAME_GRADES_H
#define GAME_GRADES_H
// An event's grade thresholds (grades.cpp): five score thresholds and the passing grade, loaded from the event's
// grades table. One declaration for grades, gradesched, eventpath, loading and pathcareer.h (port ODR).
// Names unproven.
#include "game/common.h"

struct ValueNode;

class Grades {
public:
    Grades(void) {}
    Grades(ValueNode* table);
    ~Grades(void) {}
    void Load(ValueNode* table);
    int Find(int score);
    int GetThreshold(int grade);
    void Reset(void);
    s32 thresholds[5];  // offset 0x0
    s32 passing;        // offset 0x14
};

#endif
