// Surface types (0x801D02B0-0x801D0590): per-surface grip/slip/drag/sound/effect, overridden from
// trksurf.val or surftype.val, and the surface-name to id lookup. Names are guesses apart from keys.
#include "game/common.h"

extern "C" {
int sprintf(char* s, const char* fmt, ...);
int strcasecmp(const char* a, const char* b);
void fn_802E317C(const char* fmt, ...);     // fatal: print, then exit() -> PPCHalt (never returns)
ValueTable* fn_80131808(const char* name);  // load value file
ValueTable* fn_80131810(const char* name);  // load value file, quiet
void fn_80131818(ValueTable* t);
}

struct SurfType {
    f32 grip;      // offset 0x0
    f32 slip;      // offset 0x4
    f32 drag;      // offset 0x8
    s8 sound;      // offset 0xC
    u8 effect;     // offset 0xD
    s32 soundType;  // offset 0x10
};

struct SurfName {
    const char* name;
    s32 id;
};

SurfType lbl_80338098[8] = {
    {1.0f, 1.0f, 0.0f, 1, 0, 0}, {1.0f, 1.0f, 0.0f, 0, 0, 0}, {1.0f, 1.0f, 0.0f, 1, 0, 0},
    {1.0f, 1.0f, 0.0f, 1, 0, 0}, {1.0f, 1.0f, 0.0f, 1, 0, 0}, {0.5f, 0.5f, 0.0f, 1, 0, 0},
    {1.0f, 1.0f, 1.0f, 1, 0, 0}, {1.0f, 1.0f, 1.0f, 1, 0, 0},
};

ModuleTag lbl_8055E8B8;
u8 lbl_8055E8C0;  // loaded

extern "C" {

void fn_801D02B0(void) {
    SurfType* s;
    ValueTable* def;
    ValueTable* trk;
    int i;
    lbl_8055E8C0 = 1;
    def = fn_80131808("surftype.val");
    trk = fn_80131810("trksurf.val");
    s = lbl_80338098;
    for (i = 0; i < 8; i++, s++) {
        char key[64];
        ValueNode* n;
        sprintf(key, "surface_%d", i);
        n = 0;
        if (trk && trk->HasChild(key)) {
            n = trk->FindChild(key);
        } else if (def->HasChild(key)) {
            n = def->FindChild(key);
        }
        if (n) {
            s->grip = n->GetFloat("grip", s->grip);
            s->slip = n->GetFloat("slip", s->slip);
            s->drag = n->GetFloat("drag", s->drag);
            s->sound = n->GetInt("sound", s->sound);
            s->effect = n->GetInt("effect", s->effect);
            s->soundType = n->GetInt("sound_surftype", i);
        }
    }
    fn_80131818(def);
    if (trk) {
        fn_80131818(trk);
    }
}

void fn_801D049C(void) { lbl_8055E8C0 = 0; }

SurfType* fn_801D04A8(int i) {
    if (i < 0 || i >= 8) {
        i = 0;
    }
    return &lbl_80338098[i];
}

s32 fn_801D04D0(const char* name) {
    SurfName names[13] = {
        {"default", 100}, {"concrete", 101}, {"metal", 105},  {"plastic", 106},        {"rubber", 107},
        {"wood", 108},    {"wall", 110},     {"hay", 111},    {"powerup", 1000},       {"magic_concrete", 114},
        {"tree", 115},    {"tree_fragile", 116}, {"spring", 1002},
    };
    u32 i;
    for (i = 0; i < 13; i++) {
        if (!strcasecmp(name, names[i].name)) {
            return names[i].id;
        }
    }
    fn_802E317C("surftype: invalid surface type '%s'", name);
    return 100;
}

}  // extern "C"
