// Difficulty (0x80204AA4-0x802057A8): the easy/medium/hard levels and their values from
// difficulty.val, with per-event overrides (the event's own node, then "<mode>_<key>" keys).
// Names are guesses apart from the file names, level names and messages.
#include "game/common.h"

extern "C" {
int strcasecmp(const char* a, const char* b);
double atof(const char* s);
int fn_801345D8(char* buf, int size, const char* fmt, ...);  // snprintf
void fn_802E317C(const char* fmt, ...);                      // fatal: print, then exit() -> PPCHalt (never returns)
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* table);
}

struct Event {
    s32 index;        // offset 0x0
    s32 track;        // offset 0x4
    u32 type;         // offset 0x8
    s32 laps;         // offset 0xC
    s32 drivers;      // offset 0x10
    u32 terrain;      // offset 0x14
    s32 rounds;       // offset 0x18
    s32 purse;        // offset 0x1C
    ValueNode* node;  // offset 0x20
};

Event* GetEvent(int i);
int GetNumEvents(void);

struct DebugFlags {
    s32 event;   // offset 0x0
    u8 pad4[0x4];
    s32 level;   // offset 0x8
};

extern "C" DebugFlags* fn_801E87A8(void);

void* operator new(size_t size, const char* tag);

struct DiffLevel {
    const char* name;     // offset 0x0
    const char* display;  // offset 0x4
    const char* node;     // offset 0x8
};

extern "C" {
const char* fn_80204AA4(int event);
const DiffLevel* fn_80204B18(int level);
}

static inline bool HasChild(ValueNode* n, const char* name) { return n->FindChild(name) != NULL; }

class Difficulty {
public:
    Difficulty(int level, int event) {
        this->level = level;
        table = NULL;
        levelNode = NULL;
        eventNode = NULL;
        prefix = NULL;
        table = fn_80131808("difficulty.val");
        const DiffLevel* d = fn_80204B18(level);
        levelNode = table->FindChild(d->node);
        if (event >= 0 && event < GetNumEvents()) {
            ValueNode* ev = GetEvent(event)->node;
            if (HasChild(ev, d->node)) {
                eventNode = ev->FindChild(d->node);
            }
            prefix = fn_80204AA4(event);
        }
    }
    ~Difficulty(void) {
        prefix = NULL;
        levelNode = NULL;
        eventNode = NULL;
        fn_80131818(table);
        table = NULL;
    }
    const char* Find(const char* key) {
        if (eventNode && eventNode->Has(key)) {
            return eventNode->GetValue(key);
        }
        if (levelNode->Has(key)) {
            return levelNode->GetValue(key);
        }
        return NULL;
    }
    const char* Get(const char* key) {
        const char* v = NULL;
        if (prefix) {
            char buf[100];
            fn_801345D8(buf, sizeof(buf), "%s_%s", prefix, key);
            v = Find(buf);
        }
        if (!v) {
            v = Find(key);
        }
        if (!v) {
            fn_802E317C("Difficulty: \"%s\" was not found", key);
        }
        return v;
    }

    int level;              // offset 0x0
    ValueTable* table;      // offset 0x4
    ValueNode* levelNode;   // offset 0x8
    ValueNode* eventNode;   // offset 0xC
    const char* prefix;     // offset 0x10
};

ModuleTag lbl_8055ED50;
u8 lbl_8055ED58;               // initialized
Difficulty* lbl_8055ED5C;      // current difficulty

const DiffLevel lbl_802F9D68[3] = {
    {"easy", "Easy", "easy_difficulty"},
    {"medium", "Medium", "medium_difficulty"},
    {"hard", "Hard", "hard_difficulty"},
};

extern "C" {

const char* fn_80204AA4(int event) {
    switch (GetEvent(event)->type) {
    case 0x180302:
        return "excite";
    case 0x180201:
        return "zone";
    case 0x180303:
        return "fs";
    }
    return NULL;
}

const DiffLevel* fn_80204B18(int level) { return &lbl_802F9D68[level]; }

int fn_80204B2C(const char* name) {
    for (u32 i = 0; i < 3; i++) {
        if (strcasecmp(name, lbl_802F9D68[i].name) == 0) {
            return i;
        }
    }
    fn_802E317C("Difficulty: can't find a difficulty level named '%s'", name);
    return 0;
}

int fn_80204BB4(const char* name) { return fn_80204B2C(name); }

void fn_80204BB8(void) {
    lbl_8055ED58 = 1;
    lbl_8055ED5C = NULL;
    DebugFlags* f = fn_801E87A8();
    lbl_8055ED5C = new ("difficulty") Difficulty(f->level, f->event);
}

int fn_80204CF0(void) { return lbl_8055ED5C->level; }

f32 fn_80204CFC(const char* key) { return atof(lbl_8055ED5C->Get(key)); }

const char* fn_80204EA0(const char* key) { return lbl_8055ED5C->Get(key); }

int fn_8020503C(const char* key) {
    const char* v = lbl_8055ED5C->Get(key);
    int b = strcasecmp(v, "yes") == 0 || strcasecmp(v, "true") == 0;
    return b;
}

void fn_80205208(void) {
    lbl_8055ED58 = 0;
    delete lbl_8055ED5C;
}

const char* fn_80205268(const char* key, int level, int event) {
    Difficulty d(level, event);
    return d.Get(key);
}

int fn_802054EC(const char* key, int level, int event) {
    Difficulty d(level, event);
    const char* v = d.Get(key);
    int b = 1;
    if (strcasecmp(v, "yes") && strcasecmp(v, "true")) {
        b = 0;
    }
    return b;
}

}  // extern "C"

// NonMatching: fn_80204CFC, fn_80204EA0 and fn_8020503C swap the registers of the inlined Get's
// object and result (55 diffs, registers only).
