// AI skill (0x8013A660-0x8013B7B4): reads each contest's skill range and speed factor (directly or
// interpolated between easy/medium/hard by AI strength), then spreads the AI drivers' start and race
// speeds across that range in a shuffled order, with per-driver overrides. Names are guesses apart from
// the value keys and messages.
#include "game/common.h"

extern "C" {
char* strcpy(char* d, const char* s);
char* strcat(char* d, const char* s);
int sscanf(const char* s, const char* fmt, ...);
void qsort(void* base, size_t n, size_t size, int (*cmp)(const void*, const void*));
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
int fn_80129D90(int seed, int lo, int hi);    // random int
f32 fn_80129DC4(int seed, f32 lo, f32 hi);    // random float
int fn_8027AA48(void);
void fn_802049DC(const char* name, f32* v, f32 lo, f32 hi, f32 step);  // debug tweak
f32 fn_80134B58(const char* key, f32 def);
s32 fn_80135108(void);
void fn_801E879C(void);
}

void* GetEvent(int event);
void* GetTrack(int track);
int GetNumDrivers(void);
void* GetDriver(int i);

struct V2 {
    f32 x;
    f32 y;
};

struct Strength {
    s32 event;  // offset 0x0
    u8 pad4[8];
    s32 level;  // offset 0xC
};

extern "C" Strength* fn_801E87A8(void);

struct SkillEntry {
    f32 start;  // offset 0x0
    f32 race;   // offset 0x4
};

struct SkillTable {
    f32 base;              // offset 0x0
    f32 unk4;
    SkillEntry entries[8];  // offset 0x8
    f32 rtt;               // offset 0x48
};

struct DriverBonus {
    s32 id;
    s32 bonus;
};

struct SkillSlot {
    s32 idx;     // offset 0x0
    s32 driver;  // offset 0x4
    s32 score;   // offset 0x8
};

struct StrengthDef {
    s32 level;
    f32 frac;
};

struct ResExt {
    u32 type;
    ResExt* next;
    f32 data[2];
};

class AICar {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void vf18(void);
    virtual u8* GetLine(void);  // offset 0x1C
};

extern "C" AICar* fn_801B2ED0(int i);
extern "C" ResExt* fn_80138B1C(void* res);

static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

ModuleTag lbl_8055E158;
u8 lbl_8055E160;  // active
u8 lbl_8055E161;  // attract mode
f32 lbl_80559C98 = 1.0f;  // drone speed
SkillTable lbl_8042F858;

extern "C" {

void fn_8013A660(ValueNode* t, const char* key, const char* table) {
    if (!t->Has(key)) {
        fn_802E317C("AI: Warning: table \"%s\" has no element \"%s\"", table, key);
    }
}

// Stripped by the linker; keeps its message in place.
f32 fn_8013A660_ghost(void* ghost) {
    if (!ghost) {
        fn_802E317C("AI: Warning: No ghost data.");
    }
    return 60.0f;
}

int fn_8013A6C4(const void* a, const void* b) { return ((SkillSlot*)a)->score - ((SkillSlot*)b)->score; }

const char* fn_8013A6D4(int i) {
    const char* names[3] = {"easy_", "medium_", "hard_"};
    return names[i];
}

char* fn_8013A708(char* buf, int lvl, const char* key) {
    strcpy(buf, fn_8013A6D4(lvl));
    strcat(buf, key);
    return buf;
}

int fn_8013A760(ValueNode* t, const char* key) {
    char buf[32];
    int i;
    for (i = 0; i < 3; i++) {
        if (t->Has(fn_8013A708(buf, i, key))) {
            return 1;
        }
    }
    return 0;
}

void fn_8013A7E8(ValueNode* t, const char* key, const char* table) {
    char buf[32];
    int i;
    for (i = 0; i < 3; i++) {
        fn_8013A660(t, fn_8013A708(buf, i, key), table);
    }
}

int fn_8013A860(V2* out, ValueNode* t, const char* table) {
    int r = t->Has("speed_factor");
    if (r) {
        out->x = t->GetFloat("skill_range");
        out->y = t->GetFloat("speed_factor");
    }
    return r;
}

f32 fn_8013A90C(ValueNode* t, const char* key, int lvl, f32 frac) {
    char lo[32];
    char hi[32];
    return Lerp(t->GetFloat(fn_8013A708(lo, lvl, key)), t->GetFloat(fn_8013A708(hi, lvl + 1, key)), frac);
}

void fn_8013B6C0(s32* lvl, f32* frac, s32 strength);

int fn_8013A9C0(V2* out, ValueNode* t, Strength* s, const char* table) {
    int r = t->Has("easy_skill_range");
    if (r) {
        s32 lvl;
        f32 frac;
        fn_8013B6C0(&lvl, &frac, s->level);
        out->x = fn_8013A90C(t, "skill_range", lvl, frac);
        out->y = fn_8013A90C(t, "speed_factor", lvl, frac);
    }
    return r;
}

f32 fn_8013AA68(ValueNode* t, const char* key, const char* table, Strength* s) {
    f32 v = 0.0f;
    if (fn_8013A760(t, key)) {
        s32 lvl;
        f32 frac;
        fn_8013B6C0(&lvl, &frac, s->level);
        fn_8013A7E8(t, key, table);
        v = fn_8013A90C(t, key, lvl, frac);
    }
    return v;
}

void fn_8013AB08(V2* out, Strength* s) {
    u8* ev = (u8*)GetEvent(s->event);
    u8* trk = (u8*)GetTrack(*(s32*)(ev + 4));
    if (!fn_8013A860(out, *(ValueNode**)(ev + 0x20), "Contest Table") &&
        !fn_8013A860(out, *(ValueNode**)(trk + 0x80), "Eve Table") &&
        !fn_8013A9C0(out, *(ValueNode**)(ev + 0x20), s, "Contest Table") &&
        !fn_8013A9C0(out, *(ValueNode**)(trk + 0x80), s, "Eve Table")) {
        out->x = 0.05f;
        out->y = 1.5f;
        fn_802E30C8("skill: !! !! no skill for this contest !! !!");
    }
    if (!fn_8027AA48()) {
        out->x += fn_8013AA68(*(ValueNode**)(ev + 0x20), "srd", "Contest Table", s);
        out->y += fn_8013AA68(*(ValueNode**)(ev + 0x20), "sfd", "Contest Table", s);
    }
}

f32 fn_8013AC2C(const Strength* s) {
    u8* ev = (u8*)GetEvent(s->event);
    u8* trk = (u8*)GetTrack(*(s32*)(ev + 4));
    return (*(ValueNode**)(trk + 0x80))->GetFloat("rtt_speed_factor", 1.0f);
}

void fn_8013AC78(V2* ovr, s32* cars) {
    DriverBonus bonus[50];
    SkillSlot slots[8];
    V2 sk;
    Strength* s = fn_801E87A8();
    s32 n = fn_80135108();
    int num;
    int nd;
    int i;
    int j;
    f32 span;
    fn_801E879C();
    if (!ovr) {
        fn_8013AB08(&sk, s);
    } else {
        fn_802E30C8("skill: override %.2f/%.2f", ovr->y, ovr->x);
        sk.x = ovr->x;
        sk.y = ovr->y;
    }
    lbl_8042F858.rtt = fn_8013AC2C(s);
    lbl_8042F858.base = sk.y;
    num = GetNumDrivers();
    for (nd = 0; nd < num; nd++) {
        u8* d = (u8*)GetDriver(nd);
        bonus[nd].id = *(s32*)(d + 4);
        bonus[nd].bonus = *(s32*)(d + 0x48);
    }
    for (i = 0; i < n; i++) {
        SkillSlot* e = &slots[i];
        e->idx = i;
        e->driver = cars[i];
        e->score = fn_80129D90(3, -2, 2);
        for (j = 0; j < nd; j++) {
            if (e->driver == bonus[j].id) {
                e->score += bonus[j].bonus;
                break;
            }
        }
    }
    qsort(slots, n, sizeof(SkillSlot), fn_8013A6C4);
    span = 1.0f;
    if ((f32)(n - 1) > span) {
        span = (f32)(n - 1);
    }
    for (i = 0; i < n; i++) {
        lbl_8042F858.entries[slots[i].idx].start = lbl_8042F858.base - (f32)i / span * sk.x;
    }
    for (i = 0; i < n; i++) {
        slots[i].score += fn_80129D90(3, -2, 2);
    }
    qsort(slots, n, sizeof(SkillSlot), fn_8013A6C4);
    for (i = 0; i < n; i++) {
        lbl_8042F858.entries[slots[i].idx].race = lbl_8042F858.base - (f32)i / span * sk.x;
    }
    for (i = 0; i < n; i++) {
        SkillSlot* e = &slots[i];
        u8* d = 0;
        int idx;
        int hasStart;
        for (j = 0; j < nd; j++) {
            if (e->driver == bonus[j].id) {
                d = (u8*)GetDriver(j);
            }
        }
        idx = e->idx;
        hasStart = (*(ValueNode**)(d + 0x58))->Has("override_start_skill");
        if (hasStart) {
            lbl_8042F858.entries[idx].start = sk.y * (*(ValueNode**)(d + 0x58))->GetFloat("override_start_skill");
        }
        if ((*(ValueNode**)(d + 0x58))->Has("override_skill")) {
            SkillEntry* se = &lbl_8042F858.entries[idx];
            se->race = sk.y * (*(ValueNode**)(d + 0x58))->GetFloat("override_skill");
            if (!hasStart) {
                se->start = se->race;
            }
        }
    }
}

void fn_8013B490(const char* fmt, ...) {}

// Stripped by the linker (the "skill" debug command); keeps its messages in place.
void fn_8013B490_cmd(const char* arg) {
    f32 a;
    f32 b;
    int on;
    if (sscanf(arg, "set %f %f", &a, &b) == 2) {
        fn_802E30C8("skill: set to %.2f / %.2f", a, b);
    } else if (sscanf(arg, "attract %d", &on) == 1) {
        fn_802E30C8("skill: attract mode is %s", arg);
    } else {
        fn_802E30C8("skill: expected \"skill set [speed_factor] [range]\"");
        fn_802E30C8("OR \"skill attract 0|1\"");
    }
}

struct AIGridCar {
    u8 pad0[0x68];
    s32 driver;  // offset 0x68
    u8 pad6C[0xC];
};

struct AIGrid {
    AIGridCar cars[8];  // offset 0x0
    s32 count;          // offset 0x3C0
};

void fn_8013B4E0(AIGrid* g) {
    s32 cars[8];
    int i;
    for (i = 0; i < g->count; i++) {
        cars[i] = g->cars[i].driver;
    }
    fn_8013AC78(0, cars);
    lbl_80559C98 = 1.0f;
    fn_802049DC("Drone Speed", &lbl_80559C98, 0.2f, 1.4f, 0.05f);
    lbl_8055E160 = 1;
    lbl_8055E161 = 0;
}

void fn_8013B56C(void) { lbl_8055E160 = 0; }

SkillEntry* fn_8013B578(int i) { return &lbl_8042F858.entries[i]; }

f32 fn_8013B590(int car) {
    ResExt* e = fn_80138B1C(*(void**)(fn_801B2ED0(car)->GetLine() + 0x2C));
    if (e) {
        return e->data[1];
    }
    return 60.0f;
}

f32 fn_8013B5DC(int car, int seed) {
    SkillEntry* e = fn_8013B578(car);
    f32 v = fn_8013B590(car) / e->race;
    return v + v * fn_80129DC4(seed, -0.005f, 0.005f);
}

f32 fn_8013B650(int car) {
    f32 bias = fn_80134B58("qualify_bias", 0.0f);
    f32 v = fn_8013B5DC(car, 3);
    return v + bias;
}

void fn_8013B6A4(int car, f32 v) { lbl_8042F858.entries[car].start = lbl_8042F858.entries[car].race = v; }

void fn_8013B6C0(s32* lvl, f32* frac, s32 strength) {
    static const StrengthDef defs[5] = {{0, 0.0f}, {0, 0.5f}, {0, 1.0f}, {1, 0.5f}, {1, 1.0f}};
    if (strength <= 4) {
        *lvl = defs[strength].level;
        *frac = defs[strength].frac;
    } else {
        fn_802E317C("Unrecognized AI Strength %d", strength);
        *lvl = 0;
        *frac = 0.0f;
    }
}

f32 fn_8013B744(int car) {
    if (lbl_8055E161) {
        return 1.0f - 0.01f * car;
    }
    {
        f32 r = fn_8013B578(car)->race;
        return r * lbl_80559C98;
    }
}

}  // extern "C"
