// Options (0x80133010-0x8013420C): named typed settings grouped in sections ([GLOBAL], [GX], ...,
// [PLAYER0-3]), read from text files ("version 2", "name value" lines) inside BOPT resources, kept in a
// 'OPTs' save block, with change callbacks and per-identity clearing. Names are guesses.
#include "game/common.h"
#include <ctype.h>

extern "C" {
int sprintf(char* s, const char* fmt, ...);
int sscanf(const char* s, const char* fmt, ...);
char* strcpy(char* d, const char* s);
char* strncpy(char* d, const char* s, size_t n);
char* strchr(const char* s, int c);
size_t strlen(const char* s);
int strcasecmp(const char* a, const char* b);
int strncasecmp(const char* a, const char* b, size_t n);
int atoi(const char* s);
double atof(const char* s);
void* memmove(void* d, const void* s, size_t n);
void qsort(void* base, size_t n, size_t size, int (*cmp)(const void*, const void*));
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
void* fn_8012C370(const char* name, u32 type, u32* info, s32* size, bool* first, bool* got);
int fn_8012C4D8(void* p);
void fn_802E6BD8(u32 code, int, void* buf, u32 size, void (*cb)(int));
}
void IdentRegisterCallback(void (*cb)(int slot, int event));
void* operator new(size_t size, const char* tag);

#define OPT_SECTION_NONE 18
#define OPT_MAX_ITEMS 500
#define OPT_MAX_CALLBACKS 8

struct OptValue {
    bool b;       // offset 0x0
    s32 i;        // offset 0x4
    f32 f;        // offset 0x8
    char s[16];   // offset 0xC
};

struct OptItem {
    s32 type;        // offset 0x0 (0 file, 1 bool, 2 int, 3 string, 4 float)
    s32 section;     // offset 0x4
    OptValue value;  // offset 0x8
    char name[20];   // offset 0x24
};

struct OptionsFile {
    u32 unk0;
    s32 count;                     // offset 0x4
    OptItem items[OPT_MAX_ITEMS];  // offset 0x8
};

struct OptCallback {
    const char* name;
    void (*fn)(void);
};

extern "C" {
void fn_80133F3C(int section);
void fn_80133FCC(int event);
void fn_801341B4(int slot, int cleared);
int fn_80133BA0(const char* name, int quiet, int section);
const char* fn_80133AD8(int section);
int fn_80133E10(OptItem** out, int section, const char* name);
void fn_8013323C(int section, const char* name, f32 v);
void fn_80133348(int section, const char* name, s32 v);
void fn_80133460(int section, const char* name, bool v);
void fn_8013358C(int section, const char* name, const char* s);
bool fn_80133844(const char* s);
s32 fn_801338D8(const char* s);
f32 fn_80133960(const char* s);
}

ModuleTag lbl_8055E070;

// Stripped from ET; only its string remains.
extern "C" void OptionsSave(void) { fn_802E30C8("options.dat"); }

const char* lbl_80559B58 = "default.opt";
s32 lbl_8055E078;           // initialised
OptionsFile* lbl_8055E07C;
s32 lbl_8055E080;           // patch-up count
OptItem lbl_8042F558[8];    // patch-ups re-applied after a save loads
OptCallback lbl_8042F718[OPT_MAX_CALLBACKS];

extern "C" {

int fn_80133010(void) { return lbl_8055E07C != 0; }

void fn_80133024(void) {
    int i;
    lbl_8055E078 = 1;
    lbl_8055E07C = new ("OptionsFile") OptionsFile;
    lbl_8055E07C->count = 0;
    lbl_8055E080 = 0;
    for (i = 0; i < OPT_MAX_CALLBACKS; i++) {
        lbl_8042F718[i].fn = 0;
    }
    if (!fn_80133BA0(lbl_80559B58, 0, OPT_SECTION_NONE)) {
        fn_802E317C("Can't load %s", lbl_80559B58);
    }
    fn_802E6BD8('OPTs', 6, lbl_8055E07C, sizeof(OptionsFile), fn_80133FCC);
    IdentRegisterCallback(fn_801341B4);
}

// Stripped from ET; only its string remains.
void OptionsLoadFile(const char* name) { fn_802E317C("Couldn't load %s", name); }

void fn_801330E8(int section) {
    if (!fn_80133BA0(lbl_80559B58, 0, section)) {
        fn_802E317C("OptionsDefault: can't load %s", fn_80133AD8(section));
    }
}

void fn_80133144(void) {
    int i;
    for (i = 0; i < OPT_MAX_CALLBACKS; i++) {
        if (lbl_8042F718[i].fn) {
            fn_802E317C("opt: callback %s (%x) was never removed", lbl_8042F718[i].name, lbl_8042F718[i].fn);
        }
    }
    delete lbl_8055E07C;
    lbl_8055E07C = 0;
}

void fn_801331C4(int section, const char* name, f32* v) {
    OptItem* it;
    if (fn_80133E10(&it, section, name)) {
        *v = it->value.f;
    } else {
        fn_8013323C(section, name, *v);
    }
}

void fn_8013323C(int section, const char* name, f32 v) {
    OptItem* it;
    fn_80133E10(&it, section, name);
    it->value.f = v;
    it->value.i = (int)v;
    it->value.b = 0.0f != v;
    sprintf(it->value.s, "%f", v);
    it->type = 4;
}

void fn_801332D0(int section, const char* name, s32* v) {
    OptItem* it;
    if (fn_80133E10(&it, section, name)) {
        *v = it->value.i;
    } else {
        fn_80133348(section, name, *v);
    }
}

void fn_80133348(int section, const char* name, s32 v) {
    OptItem* it;
    fn_80133E10(&it, section, name);
    it->value.i = v;
    it->value.f = v;
    it->value.b = v != 0;
    sprintf(it->value.s, "%d", v);
    it->type = 2;
}

void fn_801333E8(int section, const char* name, bool* v) {
    OptItem* it;
    if (fn_80133E10(&it, section, name)) {
        *v = it->value.b;
    } else {
        fn_80133460(section, name, *v);
    }
}

void fn_80133460(int section, const char* name, bool v) {
    OptItem* it;
    fn_80133E10(&it, section, name);
    it->value.b = v;
    it->value.i = v;
    it->value.f = v;
    strcpy(it->value.s, v ? "yes" : "no");
    it->type = 1;
}

void fn_801334F4(int section, const char* name, char* buf, int size) {
    OptItem* it;
    if (fn_80133E10(&it, section, name)) {
        strncpy(buf, it->value.s, size);
        buf[size - 1] = 0;
    } else {
        fn_8013358C(section, name, buf);
    }
}

void fn_8013358C(int section, const char* name, const char* s) {
    OptItem* it;
    fn_80133E10(&it, section, name);
    strncpy(it->value.s, s, sizeof(it->value.s));
    it->value.s[15] = 0;
    it->value.i = fn_801338D8(it->value.s);
    it->value.f = fn_80133960(it->value.s);
    it->value.b = fn_80133844(it->value.s);
    it->type = 3;
    if (strlen(s) > 15) {
        fn_802E30C8("OPTIONS WARNING: truncating \"%s\" ->\"%s\"", s, it->value.s);
    }
}

int fn_8013365C(const char* name, void (*fn)(void)) {
    int i;
    for (i = 0; i < OPT_MAX_CALLBACKS; i++) {
        if (!lbl_8042F718[i].fn) {
            lbl_8042F718[i].fn = fn;
            lbl_8042F718[i].name = name;
            return i;
        }
    }
    fn_802E317C("opt: too many callbacks (%s)", name);
    return -1;
}

void fn_801336E0(int i, void (*fn)(void)) {
    if (!lbl_8042F718[i].fn) {
        fn_802E317C("opt: removing non-existent callback %d", i);
    }
    if (lbl_8042F718[i].fn != fn) {
        fn_802E317C("opt: removing callback that doesn't match %s (%p != %p)", lbl_8042F718[i].name,
                    lbl_8042F718[i].fn, fn);
    }
    lbl_8042F718[i].fn = 0;
}

// Stripped from ET; only its string remains.
void OptionsAddPatchup(const char* section, const char* name) {
    fn_802E317C("OptionsAddPatchup: can't find %s : %s", section, name);
}

int fn_80133780(const void* a, const void* b) {
    const OptItem* x = (const OptItem*)a;
    const OptItem* y = (const OptItem*)b;
    int d = x->section - y->section;
    switch (d) {
    case 0:
        d = strncasecmp(x->name, y->name, 19);
        break;
    }
    return d;
}

void fn_801337C8(void) { qsort(lbl_8055E07C->items, lbl_8055E07C->count, sizeof(OptItem), fn_80133780); }

const char* fn_801337E4(const char* s) {
    while (*s && isspace(*s)) {
        s++;
    }
    return s;
}

bool fn_80133844(const char* s) {
    s = fn_801337E4(s);
    if (isdigit(*s)) {
        return atoi(s) != 0;
    }
    return !strcasecmp(s, "yes");
}

s32 fn_801338D8(const char* s) {
    s = fn_801337E4(s);
    if (isdigit(*s) || *s == '-') {
        return atoi(s);
    }
    return fn_80133844(s);
}

f32 fn_80133960(const char* s) {
    s = fn_801337E4(s);
    if (isdigit(*s) || *s == '-' || *s == '.') {
        return atof(s);
    }
    return fn_80133844(s);
}

int fn_80133A08(const char** pp, s32 avail, char* buf, s32 size) {
    const char* p = *pp;
    char* start = buf;
    const char* end = p + avail;
    char* last = buf + size - 1;
    while (p < end && isspace(*p)) {
        p++;
    }
    while (p < end && buf < last && *p != '\r' && *p != '\n') {
        *buf++ = *p++;
    }
    *buf = 0;
    *pp = p;
    return buf != start;
}

const char* fn_80133AD8(int section) {
    const char* names[] = {
        "GLOBAL",   "GX",       "AI",       "MULTI",   "SOUND",   "GAME",    "PHYSICS", "TRKEDIT", "CONTROL",
        "CONTROL1", "CONTROL2", "CONTROL3", "PLAYER0", "PLAYER1", "PLAYER2", "PLAYER3", "PLAYER_CAREER",
    };
    return names[section];
}

int fn_80133B24(const char* name) {
    int i;
    for (i = 0; i < 17; i++) {
        if (!strcasecmp(name, fn_80133AD8(i))) {
            return i;
        }
    }
    fn_802E30C8("Can't get section for %s", name);
    return OPT_SECTION_NONE;
}

int fn_80133BA0(const char* name, int quiet, int section) {
    s32 size = 0;
    u32 info;
    s32 version;
    const char* cur;
    char line[0x100];
    OptItem* it;
    const char* data = (const char*)fn_8012C370(name, 'BOPT', &info, &size, 0, 0);
    int ok = 0;
    if (data) {
        int curSection;
        int versionOk;
        cur = data;
        versionOk = 0;
        ok = 1;
        curSection = 0;
        while (ok && fn_80133A08(&cur, size - (cur - data), line, sizeof(line))) {
            char c = line[0];
            if (c == '[') {
                char* close = strchr(line, ']');
                int sec = OPT_SECTION_NONE;
                if (close) {
                    *close = 0;
                    sec = fn_80133B24(line + 1);
                }
                if (sec != OPT_SECTION_NONE) {
                    curSection = sec;
                } else {
                    fn_802E30C8("opt: bad section on line %d", cur);
                }
            } else if (c == ';' || c == 0 || c == '#') {
            } else if (versionOk) {
                if (lbl_8055E07C->count < OPT_MAX_ITEMS) {
                    char* sp = strchr(line, ' ');
                    if (sp && *sp) {
                        *sp = 0;
                        if (section == OPT_SECTION_NONE || section == curSection) {
                            fn_80133E10(&it, curSection, line);
                            strcpy(it->value.s, sp + 1);
                            it->value.i = fn_801338D8(it->value.s);
                            it->value.f = fn_80133960(it->value.s);
                            it->value.b = fn_80133844(it->value.s);
                            it->type = 0;
                        }
                    } else {
                        fn_802E30C8("Item %s:%s has no value", fn_80133AD8(curSection), line);
                    }
                }
            } else if ((sscanf(line, "version %d", &version) == 1 && version == 2) || quiet) {
                versionOk = 1;
            } else {
                fn_802E30C8("%s: Wrong version", name);
                ok = 0;
            }
        }
        fn_8012C4D8((void*)data);
    } else {
        fn_802E30C8("Couldn't open options file: %s", name);
    }
    return ok;
}

int fn_80133E10(OptItem** out, int section, const char* name) {
    int i;
    for (i = 0; i < lbl_8055E07C->count; i++) {
        OptItem* it = &lbl_8055E07C->items[i];
        if (section == it->section && !strncasecmp(name, it->name, 19)) {
            *out = &lbl_8055E07C->items[i];
            return 1;
        }
    }
    if (lbl_8055E07C->count < OPT_MAX_ITEMS) {
        OptItem* it = &lbl_8055E07C->items[lbl_8055E07C->count];
        lbl_8055E07C->count++;
        it->section = section;
        strncpy(it->name, name, sizeof(it->name));
        it->name[19] = 0;
        if (strlen(name) >= sizeof(it->name)) {
            fn_802E30C8("OPTIONS WARNING: truncating option name \"%s\" to \"%s\"", name, it->name);
        }
        it->value.s[0] = 0;
        it->value.b = 0;
        it->value.i = 0;
        it->value.f = 0.0f;
        *out = it;
    } else {
        *out = lbl_8055E07C->items;
    }
    return 0;
}

void fn_80133F3C(int section) {
    int i;
    for (i = 0; i < lbl_8055E07C->count; i++) {
        if (lbl_8055E07C->items[i].section == section) {
            lbl_8055E07C->count--;
            memmove(&lbl_8055E07C->items[i], &lbl_8055E07C->items[i] + 1,
                    (lbl_8055E07C->count - i) * sizeof(OptItem));
            i--;
        }
    }
}

void fn_80133FCC(int event) {
    if (event == 1) {
        int k;
        for (k = 0; k < lbl_8055E080; k++) {
            OptItem* p = &lbl_8042F558[k];
            int i;
            for (i = 0; i < lbl_8055E07C->count; i++) {
                OptItem* it = &lbl_8055E07C->items[i];
                if (p->section == it->section && !strcasecmp(p->name, it->name)) {
                    *it = *p;
                }
            }
        }
        for (k = 0; k < OPT_MAX_CALLBACKS; k++) {
            if (lbl_8042F718[k].fn) {
                lbl_8042F718[k].fn();
            }
        }
    } else if (event == 2) {
        fn_802E30C8("opt: loading error not handled!");
    }
    if (event == 3) {
        fn_801337C8();
    }
}

void fn_801341B4(int slot, int cleared) {
    switch (cleared) {
    case 0:
        fn_802E30C8("Opt: Identity called to say that %d was cleared", slot);
        fn_80133F3C(slot + 12);
        break;
    }
}

}  // extern "C"
