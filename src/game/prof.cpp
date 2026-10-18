// Profiler (0x801274C8-0x80127A24, "prof:" in its messages). Most of it was stripped from the release
// build; what remains resets the per-frame counters, keeps the running time total and dumps the trace
// list on shutdown. Names are guesses.
#include "game/common.h"

template <typename T, int N>
class FixedArray {
public:
    FixedArray(void) {
        data = storage;
        max = N;
        count = 0;
    }
    T* GetFromEnd(int i) {
        if (count - i - 1 >= 0) {
            return &data[count] - i - 1;
        }
        return 0;
    }

    T* data;       // offset 0x0
    s32 max;       // offset 0x4
    s32 count;     // offset 0x8
    T storage[N];  // offset 0xC
};

struct ProfSlot {
    u8 pad0[0x10];
    s32 counters[10];  // offset 0x10
    u8 pad38[0x41 - 0x38];
    u8 used;    // offset 0x41
    u8 active;  // offset 0x42
    u8 pad43[0x48 - 0x43];
};

struct ProfWatch {
    u8 active;  // offset 0x0
    u8 pad1[0x24 - 0x1];
    s32* target;  // offset 0x24
    u8 pad28[0x2C - 0x28];
};

typedef void (*ProfCallback)(int);

struct ProfHooks {
    ProfCallback callbacks[4];  // offset 0x0
    ProfWatch watches[16];      // offset 0x10
};

struct ProfTrace {
    const char* file;  // offset 0x0
    s32 line;          // offset 0x4
    s32 time;          // offset 0x8
};

extern "C" {
void* memset(void* p, int c, size_t n);
void fn_802E30C8(const char* fmt, ...);  // debug print
u32 fn_802E2C20(void);
u64 fn_802E2C9C(void);
void fn_80127C90(void (*fn)(void), const char* name);
void fn_80127D58(void (*fn)(void));
}

ModuleTag lbl_8055DF10;
ProfSlot lbl_8041D790[16];
ProfHooks lbl_8041DC10;
FixedArray<s32, 16> lbl_8041DEE0;
ProfTrace lbl_8041DF30[3];

u64 lbl_8055DF18;  // total time
u64 lbl_8055DF20;  // last time
s32 lbl_8055DF28;
s32 lbl_8055DF2C;
s32 lbl_8055DF30;
s32 lbl_8055DF34;
s32 lbl_8055DF38;
s32 lbl_8055DF3C;
u32 lbl_8055DF40;  // frame start time
s32 lbl_8055DF44;  // trace count

extern "C" {

void fn_80127968(void);

void fn_801274C8(void) {
    int i;
    s32* p;
    for (i = 0; (p = lbl_8041DEE0.GetFromEnd(i)) != 0; i++) {
        lbl_8041D790[*p].active = 1;
    }
}

// Stripped from ET; only their strings remain.
void ProfAddCounter(const char* name) { fn_802E30C8("prof: out of custom counters (%s)", name); }

void ProfAddCallback(const char* name) { fn_802E30C8("prof: out of room for callbacks (%s)", name); }

void ProfRemoveCallback(const char* name) { fn_802E30C8("prof: can't remove callback %s", name); }

void ProfReportHR(const char* name, s32 usec) { fn_802E30C8("hrprof: %7d usec avg for %s", usec, name); }

void ProfBegin(const char* name) { fn_802E30C8("prof: out of profs for %s", name); }

void ProfRestartTrace(void) { fn_802E30C8("prof: restarting trace system!"); }

int fn_80127538(void) {
    memset(lbl_8041D790, 0, sizeof(lbl_8041D790));
    memset(lbl_8041DC10.callbacks, 0, sizeof(lbl_8041DC10.callbacks));
    fn_80127968();
    fn_80127C90(fn_80127968, "ProfUpdate");
    lbl_8055DF18 = 0;
    lbl_8055DF30 = 0;
    lbl_8055DF34 = 0;
    return 1;
}

void fn_801275AC(void) {
    int i;
    fn_80127D58(fn_80127968);
    if (lbl_8055DF44) {
        fn_802E30C8("=======================================================");
        fn_802E30C8("prof: trace list %d", lbl_8055DF44);
        for (i = 0; i < lbl_8055DF44; i++) {
            fn_802E30C8(" %08d : %8d : %s:%d", lbl_8041DF30[i].time, lbl_8041DF30[i + 1].time - lbl_8041DF30[i].time,
                        lbl_8041DF30[i].file, lbl_8041DF30[i].line);
        }
        fn_802E30C8("=======================================================");
    }
}

// Stripped from ET; only their strings remain.
void ProfPrintLine(const char* indent, int width, const char* name, const char* label, f32 pct) {
    fn_802E30C8("  %s%-*.*s  %32.32s %10.2f%%*", indent, width, width, name, label, pct);
}

const char* ProfGetName(int index) { return index < 0 ? "Not Profiled" : 0; }

void fn_8012766C(void) {
    int i;
    for (i = 0; i < 16; i++) {
        ProfSlot* s = &lbl_8041D790[i];
        s->used = 0;
        s->counters[9] = 0;
        s->counters[8] = 0;
        s->counters[7] = 0;
        s->counters[6] = 0;
        s->counters[5] = 0;
        s->counters[4] = 0;
        s->counters[3] = 0;
        s->counters[2] = 0;
        s->counters[1] = 0;
        s->counters[0] = 0;
    }
    lbl_8055DF18 = 0;
    lbl_8055DF30 = 0;
    lbl_8055DF34 = 0;
    lbl_8055DF2C = 0;
    lbl_8055DF28 = 0;
    lbl_8055DF3C = 0;
    lbl_8055DF38 = 0;
    for (i = 0; i < sizeof(lbl_8041DC10.watches) / sizeof(lbl_8041DC10.watches[0]); i++) {
        if (lbl_8041DC10.watches[i].active) {
            *lbl_8041DC10.watches[i].target = 0;
        }
    }
    lbl_8055DF40 = fn_802E2C20();
    for (i = 0; i < 4; i++) {
        if (lbl_8041DC10.callbacks[i]) {
            lbl_8041DC10.callbacks[i](0);
        }
    }
}

void fn_80127930(void) {
    fn_8012766C();
    fn_801274C8();
}

u32 fn_80127954(void) { return fn_802E2C20(); }

void fn_80127958(void) {}

int fn_8012795C(void) { return 0; }

void fn_80127964(void) {}

void fn_80127968(void) {
    u64 now = fn_802E2C9C();
    lbl_8055DF18 += now - lbl_8055DF20;
    lbl_8055DF20 = now;
}

void fn_801279B8(void) {
    if (++lbl_8055DF34 == 0) {
        fn_80127954();
    }
}

s32 fn_801279D0(void) { return lbl_8055DF30++; }

}  // extern "C"
