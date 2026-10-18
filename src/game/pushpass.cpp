// AI push/pass tuning (0x8013A264-0x8013A660): default tuning from the event's "ai" values and per-track
// "exception" ranges (on_lon..off_lon) that override it; ranges that wrap past the lap end are split in
// two. Names are guesses apart from the value keys.
#include "game/common.h"

extern "C" {
int strcasecmp(const char* a, const char* b);
void fn_802E30C8(const char* fmt, ...);  // debug print
f32 fn_80134B58(const char* key, f32 def);
}

void* operator new(size_t size, const char* tag);

struct AITune {
    f32 onT;        // offset 0x0
    f32 moveout;    // offset 0x4
    f32 passout;    // offset 0x8
    f32 pushSens;   // offset 0xC
    f32 lookahead;  // offset 0x10
    f32 checks;     // offset 0x14
    f32 landing;    // offset 0x18
};

struct AIException {
    f32 onLon;          // offset 0x0
    f32 offLon;         // offset 0x4
    AITune tune;        // offset 0x8
    AIException* next;  // offset 0x24
};

class AIExceptions {
public:
    AIExceptions(void);
    ~AIExceptions(void);
    void Load(ValueNode* node);
    AITune* Find(f32 lon);

    AIException* list;  // offset 0x0
    AITune tune;        // offset 0x4
};

static inline const char* Plural(int n) { return n != 1 ? "s" : ""; }

ModuleTag lbl_8055E150;

AIExceptions::AIExceptions(void) {
    tune.passout = fn_80134B58("pushpass_factor", 1.0f);
    tune.moveout = fn_80134B58("moveout_factor", 1.0f);
    tune.pushSens = fn_80134B58("push_sensitivity", 1.0f);
    tune.checks = fn_80134B58("checks", 10.0f);
    tune.lookahead = 1.0f;
    tune.landing = 1.0f;
    tune.onT = 3.0f;
    list = 0;
}

void AIExceptions::Load(ValueNode* node) {
    if (node) {
        ValueNode* c;
        int i;
        AIException* e;
        for (i = 0; (c = node->GetChildAt(i)) != 0; i++) {
            if (!strcasecmp(c->GetName(), "exception")) {
                e = (AIException*)operator new(sizeof(AIException), "AIException");
                e->onLon = c->GetFloat("on_lon");
                e->offLon = c->GetFloat("off_lon");
                e->tune.onT = c->GetFloat("on_t", tune.onT);
                e->tune.moveout = c->GetFloat("moveout", tune.moveout);
                e->tune.passout = c->GetFloat("passout", tune.passout);
                e->tune.pushSens = c->GetFloat("push_sensitivity", tune.pushSens);
                e->tune.lookahead = c->GetFloat("lookahead_f", tune.lookahead);
                e->tune.checks = c->GetFloat("checks", tune.checks);
                e->tune.landing = c->GetFloat("landing_f", tune.landing);
                e->next = list;
                list = e;
                if (e->onLon > e->offLon) {
                    AIException* w = new ("AIException") AIException(*e);
                    e->offLon = 10000.0f;
                    w->onLon = 0.0f;
                    w->tune.onT = 0.1f;
                    w->next = list;
                    list = w;
                }
            }
        }
        fn_802E30C8("ai: track has %d ai-exception%s", i, Plural(i));
    }
}

AIExceptions::~AIExceptions(void) {
    while (list) {
        AIException* e = list;
        list = list->next;
        delete e;
    }
}

AITune* AIExceptions::Find(f32 lon) {
    AIException* e;
    for (e = list; e; e = e->next) {
        bool in = false;
        if (lon >= e->onLon && lon <= e->offLon) {
            in = true;
        }
        if (in) {
            return &e->tune;
        }
    }
    return &tune;
}
