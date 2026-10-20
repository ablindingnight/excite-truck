// Background tasks (0x80127B5C-0x801281A0, "bg:" in its messages): up to ten per-frame hooks, each a
// plain function or an object with an Update method, run once a frame (or when 16 ticks have passed);
// removals during a run are deferred. Names are guesses.
#include "game/common.h"

class BgObject {
public:
    virtual void vf08(void);
    virtual void Update(void);  // 0xC
};

typedef void (*BgFunc)(void);

struct BgTask {
    BgTask(void) {
        name = 0;
        fn = 0;
        obj = 0;
        dead = 0;
    }

    BgFunc fn;         // offset 0x0
    BgObject* obj;     // offset 0x4
    const char* name;  // offset 0x8
    u8 dead;           // offset 0xC

    int IsFunc(BgFunc f) { return fn == f && obj == 0; }
    int IsObj(BgObject* o) { return fn == 0 && obj == o; }
};

class BgTasks {
public:
    BgTasks(void) {
        running = 0;
        count = 0;
    }
    void RemoveAt(int i) {
        if (i < 0 || i >= count) {
            return;
        }
        if (running) {
            tasks[i].dead = 1;
        } else {
            tasks[i] = tasks[--count];
        }
    }

    void RemoveFunc(BgFunc fn) {
        int i;
        for (i = 0; i < count; i++) {
            if (tasks[i].IsFunc(fn)) {
                RemoveAt(i);
                return;
            }
        }
    }
    void RemoveObj(BgObject* obj) {
        int i;
        for (i = 0; i < count; i++) {
            if (tasks[i].IsObj(obj)) {
                RemoveAt(i);
                return;
            }
        }
    }

    u8 running;        // offset 0x0
    s32 count;         // offset 0x4
    BgTask tasks[10];  // offset 0x8
};

extern "C" {
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
void fn_802E30C8(const char* fmt, ...);  // debug print
u32 fn_802E2C20(void);
}

ModuleTag lbl_8055DF68;
BgTasks lbl_8041E098;
s32 lbl_8055DF70;  // last run time

extern "C" {

void fn_80127B5C(u32 time) {
    BgTask* t;
    int i;
    lbl_8041E098.running = 1;
    for (i = 0, t = lbl_8041E098.tasks; i < lbl_8041E098.count; t++, i++) {
        if (t->fn) {
            t->fn();
        } else if (t->obj) {
            t->obj->Update();
        }
    }
    lbl_8041E098.running = 0;
    for (i = 0; i < lbl_8041E098.count;) {
        if (lbl_8041E098.tasks[i].dead) {
            lbl_8041E098.RemoveAt(i);
        } else {
            i++;
        }
    }
    lbl_8055DF70 = time;
}

void fn_80127C8C(void) {}

void fn_80127C90(BgFunc fn, const char* name) {
    bool ok = lbl_8041E098.count < 10;
    if (ok) {
        BgTask* t = &lbl_8041E098.tasks[lbl_8041E098.count++];
        t->fn = fn;
        t->obj = 0;
        t->name = name;
        t->dead = 0;
    }
    if (!ok) {
        fn_802E317C("bg: too many tasks (adding %s)", name);
    }
}

void fn_80127CF4(BgObject* obj, const char* name) {
    bool ok = lbl_8041E098.count < 10;
    if (ok) {
        BgTask* t = &lbl_8041E098.tasks[lbl_8041E098.count++];
        t->fn = 0;
        t->obj = obj;
        t->name = name;
        t->dead = 0;
    }
    if (!ok) {
        fn_802E317C("bg: too many task objects (adding %s)", name);
    }
}

void fn_80127D58(BgFunc fn) { lbl_8041E098.RemoveFunc(fn); }

void fn_80127E28(BgObject* obj) { lbl_8041E098.RemoveObj(obj); }

void fn_80127EF8(void) {
    int i;
    for (i = 0; i < lbl_8041E098.count; i++) {
        fn_802E30C8("bg: Unfreed hook! \"%s\"", lbl_8041E098.tasks[i].name);
    }
}

void fn_80127F70(void) { fn_80127B5C(fn_802E2C20()); }

void fn_80127F94(void) {
    s32 now = fn_802E2C20();
    if (now - lbl_8055DF70 > 16) {
        fn_80127B5C(now);
    }
}

}  // extern "C"
