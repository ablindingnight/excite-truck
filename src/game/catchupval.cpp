// AI catch-up (0x8013B7B4-0x8013BC90): reads catchup.val per league difficulty (begin_t, effect_t,
// speed_factor, compare_car_pct), blends it by AI strength, and every eighth frame measures the gap
// between the leader and the compared car to scale the AI's speed. Names are guesses apart from the keys.
#include "game/common.h"

extern "C" {
int strcasecmp(const char* a, const char* b);
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* t);
void fn_8013B6C0(s32* lvl, f32* frac, s32 strength);
int fn_80179630(int i);
int fn_801B2F24(void);
int fn_801B2F40(int i);
double fn_801B2F90(int a, int b);
}

struct Strength {
    s32 event;
    u8 pad4[8];
    s32 level;  // offset 0xC
};

extern "C" Strength* fn_801E87A8(void);

struct Racer {
    u8 pad0[4];
    s32 place;  // offset 0x4
};

class AICar {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual Racer* GetRacer(void);  // offset 0x10
};

extern "C" AICar* fn_801B2ED0(int i);

struct Catchup {
    f32 begin;    // offset 0x0
    f32 effect;   // offset 0x4
    f32 speed;    // offset 0x8
    f32 compare;  // offset 0xC
};

struct CatchupState {
    f32 dist;   // offset 0x0
    u8 active;  // offset 0x4
    f32 speed;  // offset 0x8
    s32 car;    // offset 0xC
};

static inline f32 Lerp(f32 t, f32 a, f32 b) { return a + t * (b - a); }
static inline f32 MinF(f32 a, f32 b) { return a < b ? a : b; }
static inline f32 MaxF(f32 a, f32 b) { return a > b ? a : b; }
static inline f32 Round(f32 f) { return f < 0.0f ? f - 0.5f : 0.5f + f; }

ModuleTag lbl_8055E168;
s32 lbl_8055E170;  // compared car rank
s32 lbl_8055E174;  // frame counter
CatchupState lbl_8042F8A8;
Catchup lbl_8042F8B8;

extern "C" {

void fn_8013B7B4(Catchup* tbl, s32 n) {
    Catchup* out;
    ValueTable* t;
    ValueNode* c;
    int i;
    t = fn_80131808("catchup.val");
    const char* name;
    u32 j;
    if (t) {
        for (i = 0; (c = t->GetChild(i)) != 0; i++) {
            name = c->GetName();
            const char* names[3] = {"Easy", "Medium", "Hard"};
            out = tbl;
            for (j = 0; j < 3; j++, out++) {
                if (!strcasecmp(names[j], name)) {
                    out->begin = c->GetFloat("begin_t");
                    out->effect = c->GetFloat("effect_t");
                    out->speed = c->GetFloat("speed_factor");
                    out->compare = n * c->GetFloat("compare_car_pct");
                }
            }
        }
        fn_80131818(t);
    } else {
        fn_802E317C("no catchup.val?");
    }
}

int fn_8013BC70(f32* dist);

void fn_8013B950(void) {
    int leader;
    f32 dist;
    lbl_8042F8A8.active = 0;
    leader = fn_80179630(0);
    if (leader >= 0) {
        Racer* found;
        int k;
        int i;
        int id;
        k = lbl_8055E170;
        found = 0;
        id = -1;
        for (i = fn_801B2F24() - 1; i >= 0; i--) {
            id = fn_801B2F40(i);
            found = fn_801B2ED0(id)->GetRacer();
            if (--k <= 0) {
                break;
            }
        }
        if (found && fn_801B2ED0(leader)->GetRacer()->place > found->place) {
            double d = fn_801B2F90(leader, id);
            lbl_8042F8A8.dist = d;
            lbl_8042F8A8.car = id;
            lbl_8042F8A8.active = lbl_8042F8A8.dist < 1000.0f;
        }
    }
    if (fn_8013BC70(&dist)) {
        f32 t = (dist - lbl_8042F8B8.begin) / ((lbl_8042F8B8.begin + lbl_8042F8B8.effect) - lbl_8042F8B8.begin);
        lbl_8042F8A8.speed = Lerp(MaxF(MinF(1.0f, t), 0.0f), 1.0f, lbl_8042F8B8.speed);
    } else {
        lbl_8042F8A8.speed = 1.0f;
    }
}

void fn_8013BAE4(s32 n) {
    Catchup tbl[3];
    s32 lvl;
    f32 frac;
    int cnt;
    lbl_8042F8A8.active = 0;
    fn_8013B7B4(tbl, n);
    fn_8013B6C0(&lvl, &frac, fn_801E87A8()->level);
    lbl_8042F8B8.begin = Lerp(frac, tbl[lvl].begin, tbl[lvl + 1].begin);
    lbl_8042F8B8.effect = Lerp(frac, tbl[lvl].effect, tbl[lvl + 1].effect);
    lbl_8042F8B8.speed = Lerp(frac, tbl[lvl].speed, tbl[lvl + 1].speed);
    lbl_8042F8B8.compare = Lerp(frac, tbl[lvl].compare, tbl[lvl + 1].compare);
    cnt = (int)Round(lbl_8042F8B8.compare);
    cnt = cnt > 1 ? cnt : 1;
    lbl_8055E170 = cnt < n - 1 ? cnt : n - 1;
    if (lbl_8042F8B8.speed >= 1.0f) {
        lbl_8042F8B8.speed = 1.0f;
    }
}

void fn_8013BC34(void) {
    lbl_8042F8A8.speed = 1.0f;
    lbl_8055E174 = 0;
    lbl_8042F8A8.active = 0;
}

void fn_8013BC54(void) {
    if ((++lbl_8055E174 & 7) == 0) {
        fn_8013B950();
    }
}

int fn_8013BC70(f32* dist) {
    *dist = lbl_8042F8A8.dist;
    return lbl_8042F8A8.active;
}

}  // extern "C"
