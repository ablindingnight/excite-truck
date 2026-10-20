// Localization (0x8012FAB8-0x8013144C): language string tables (lang.lng inside usenglish.res /
// japanese.res), metric/imperial unit systems tunable from units.val, LocString (cached lookups
// keyed by a global version), and number/time/ordinal formatting. Names are guesses.
#include "game/common.h"
#include <ctype.h>

extern "C" {
int strcasecmp(const char* a, const char* b);
int strncasecmp(const char* a, const char* b, size_t n);
size_t strlen(const char* s);
char* strchr(const char* s, int c);
void* memset(void* p, int c, size_t n);
void* memcpy(void* d, const void* s, size_t n);
int atoi(const char* s);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
int fn_801345D8(char* buf, u32 size, const char* fmt, ...);  // snprintf
void* fn_8012C370(const char* name, u32 type, u32* info, s32* size, bool* first, bool* got);
int fn_8012C4D8(void* p);
void fn_8012BEF0(const char* name);  // ResourceSetLoad
void fn_8012BF24(const char* name);  // ResourceSetUnload
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* table);
}

extern f32 lbl_80558DC0[];  // FLT_MAX (MSL)
extern u32 lbl_8055E044;

struct Unit {
    const char* name;   // offset 0x0
    const char* sname;  // offset 0x4
    f32 conv;           // offset 0x8
    s32 precis;         // offset 0xC
};

struct UnitSystem {
    Unit s_dist;    // offset 0x0
    Unit l_dist;    // offset 0x10
    Unit speed;     // offset 0x20
    Unit liquid;    // offset 0x30
    Unit eng;       // offset 0x40
    Unit mass;      // offset 0x50
    Unit ui;        // offset 0x60
    bool metric;    // offset 0x70
};

struct CurUnits {
    char sep;        // offset 0x0 (decimal separator)
    u8 pad[7];
    UnitSystem sys;  // offset 0x8
};

struct LangInfo {
    const char* res;
    const char* name;
    const char* flag;
};

struct LangEntry {
    const char* key;
    const char* text;
};

struct LangFile {
    u8 header[0x40];
    s32 count;            // offset 0x40
    LangEntry entries[1];  // offset 0x44
};

struct LangState {
    s32 cur;          // offset 0x0
    LangFile* file;   // offset 0x4
    u8 buf[50000];    // offset 0x8
};

struct DigitGlyph {
    char c;
    const char* esc;
};

// Same 123-tag pattern as common.h's UnkStatic123, but only five words.
struct UnkStatic5 {
    UnkStatic5(void) {
        unk4 = 123;
        unk0 = 0;
        unk8 = 0;
        unkC = 0;
        unk10 = 0;
    }
    u32 unk0;
    u32 unk4;
    u32 unk8;
    u32 unkC;
    u32 unk10;
};

struct Unk40 {
    Unk40(void) { v = 0x7F000000; }
    u32 v;
};

class LocString {
public:
    LocString(const char* key);
    const char* Get(void);

    const char* key;   // offset 0x0
    const char* text;  // offset 0x4
    u32 version;       // offset 0x8
};

extern "C" {
void fn_80131318(LocString* s);
const char* fn_801302E0(const char* key);
const char* fn_80131234(const char* key);
void fn_8013111C(int lang);
}

inline const char* LocString::Get(void) {
    if (version != lbl_8055E044) {
        fn_80131318(this);
    }
    return text;
}

ModuleTag lbl_8055E038;
static UnkStatic5 s_unk;
Unk40 lbl_8055E040;
u32 lbl_8055E044;  // localization version
u8 lbl_8055E048;    // initialised
s32 lbl_8055E04C;   // language index
static CurUnits s_cur;
CurUnits& lbl_8055E050 = s_cur;
static LangState s_lang;

const LangInfo lbl_802F8E60[] = {
    {"usenglish.res", "English (US)", "flag_usa.art"},
    {"japanese.res", "Japanese", "flag_jp.art"},
};

UnitSystem lbl_8032B0C8 = {
    {"Meters", "m", 1.0f, 2},
    {"Kilometers", "km", 0.001f, 1},
    {"Kilometers/Hr", "km/h", 3.6f, 0},
    {"Liters", "L", 1.0f, 2},
    {"Cubic Centimeters", "cc", 1.0f, 2},
    {"Kilograms", "kg", 1.0f, 2},
    {"UI_UNITS", "uiW", 50.0f, 0},
    true,
};

UnitSystem lbl_8032B14C = {
    {"Feet", "'", 3.2786884f, 2},
    {"Miles", "mi", 0.000625f, 1},
    {"MPH", "MPH", 2.25f, 0},
    {"Gallons", "gal", 0.264f, 2},
    {"Cubic Inches", "ci", 0.0610237494f, 2},
    {"Pounds", "lbs", 2.20022f, 2},
    {"UI_UNITS", "uiW", 0.0f, 0},
    false,
};

static LocString s_metersAbbr("Units:MetersAbbreviation");
static LocString s_kmAbbr("Units:KilometersAbbreviation");
static LocString s_kmhAbbr("Units:KilometersPerHourAbbreviation");
static LocString s_litersAbbr("Units:LitersAbbreviation");
static LocString s_ccAbbr("Units:CubicCentimetersAbbreviation");
static LocString s_kgAbbr("Units:KilogramsAbbreviation");
static LocString s_feetAbbr("Units:FeetAbbreviation");
static LocString s_milesAbbr("Units:MilesAbbreviation");
static LocString s_mphAbbr("Units:MilesPerHourAbbreviation");
static LocString s_gallonsAbbr("Units:GallonsAbbreviation");
static LocString s_ciAbbr("Units:CubicInchesAbbreviation");
static LocString s_lbsAbbr("Units:PoundsAbbreviation");

static UnitSystem s_custom = lbl_8032B14C;

static inline void StrNCpy(char* dst, const char* src, int len) {
    char* d = dst;
    int n = len;
    const char* s = src;
    while ((*d++ = *s++) != 0) {
        if (--n <= 0) {
            d[-1] = 0;
            break;
        }
    }
}

static inline void StrNCat(char* d, const char* s, int n) {
    while (*d) {
        n--;
        d++;
    }
    if (n > 0) {
        StrNCpy(d, s, n);
    }
}

static inline void Swap4(u8* p) {
    u8 t = p[0];
    p[0] = p[3];
    p[3] = t;
    t = p[1];
    p[1] = p[2];
    p[2] = t;
}

static inline void ReadUnit(ValueNode* n, Unit* u) {
    u->name = n->GetString("name", u->name);
    u->sname = n->GetString("sname", u->name);
    u->conv = n->GetFloat("conv", u->conv);
    u->precis = n->GetInt("precis", u->precis);
}

extern "C" {

void fn_8012FAB8(const char* valName) {
    lbl_8032B0C8.s_dist.name = s_metersAbbr.Get();
    lbl_8032B0C8.l_dist.name = s_kmAbbr.Get();
    lbl_8032B0C8.speed.name = s_kmhAbbr.Get();
    lbl_8032B0C8.liquid.name = s_litersAbbr.Get();
    lbl_8032B0C8.eng.name = s_ccAbbr.Get();
    lbl_8032B0C8.mass.name = s_kgAbbr.Get();
    lbl_8032B14C.s_dist.name = s_feetAbbr.Get();
    lbl_8032B14C.l_dist.name = s_milesAbbr.Get();
    lbl_8032B14C.speed.name = s_mphAbbr.Get();
    lbl_8032B14C.liquid.name = s_gallonsAbbr.Get();
    lbl_8032B14C.eng.name = s_ciAbbr.Get();
    lbl_8032B14C.mass.name = s_lbsAbbr.Get();
    if (valName) {
        ValueTable* t = fn_80131808(valName);
        if (t) {
            int i = 0;
            ValueNode* n;
            for (n = t->GetChild(i++); n; n = t->GetChild(i++)) {
                if (!strcasecmp("s_dist", n->GetName())) {
                    ReadUnit(n, &s_custom.s_dist);
                }
                if (!strcasecmp("l_dist", n->GetName())) {
                    ReadUnit(n, &s_custom.l_dist);
                }
                if (!strcasecmp("speed", n->GetName())) {
                    ReadUnit(n, &s_custom.speed);
                }
                if (!strcasecmp("liquid_volume", n->GetName())) {
                    ReadUnit(n, &s_custom.liquid);
                }
                if (!strcasecmp("eng_disp", n->GetName())) {
                    ReadUnit(n, &s_custom.eng);
                }
                if (!strcasecmp("mass", n->GetName())) {
                    ReadUnit(n, &s_custom.mass);
                }
                if (!strcasecmp("ui_width", n->GetName())) {
                    ReadUnit(n, &s_custom.ui);
                }
            }
            fn_80131818(t);
        }
    }
}

// Stripped from ET; only their strings remain.
void LocaleLoadGenesis(const char* name) {
    fn_802E30C8("Bad version on %s", name);
    fn_802E30C8("Can't load %s", name);
    fn_802E30C8("genesis.res");
}

void LocaleFormatDate(char* buf, u32 size, int d, int m, int y) {
    fn_801345D8(buf, size, "%d/%d/%d", d, m, y);
    fn_801345D8(buf, size, "%d", y);
}

void fn_801301E0(LangFile* f) {
    int i;
    Swap4((u8*)&f->count);
    for (i = 0; i < f->count; i++) {
        Swap4((u8*)&f->entries[i].key);
        Swap4((u8*)&f->entries[i].text);
        f->entries[i].key += (u32)f;
        f->entries[i].text += (u32)f;
    }
}

void fn_8013028C(void) {
    if (s_lang.cur >= 0 && s_lang.cur < sizeof(lbl_802F8E60) / sizeof(lbl_802F8E60[0])) {
        fn_8012BF24(lbl_802F8E60[s_lang.cur].res);
    }
    s_lang.cur = -1;
}

const char* fn_801302E0(const char* key) {
    if (s_lang.file) {
        int i;
        for (i = 0; i < s_lang.file->count; i++) {
            if (!strcasecmp(key, s_lang.file->entries[i].key)) {
                return s_lang.file->entries[i].text;
            }
        }
    }
    return 0;
}

const char* fn_80130390(void) { return lbl_802F8E60[lbl_8055E04C].res; }

void fn_801303A8(void) {
    lbl_8055E048 = 1;
    s_lang.cur = -1;
    fn_8013111C(lbl_8055E04C);
    s_cur.sys = s_custom;
    s_cur.sep = s_cur.sys.metric ? ',' : '.';
}

void fn_80130518(void) {
    fn_8013028C();
    s_lang.file = 0;
    memset(s_lang.buf, 0, sizeof(s_lang.buf));
    lbl_8055E048 = 0;
}

void fn_80130568(char* s) {
    if (lbl_8055E050.sep != '.') {
        char* p;
        while ((p = strchr(s, '.')) != 0) {
            *p = lbl_8055E050.sep;
        }
    }
}

void fn_801305C4(char* buf, s32 size, Unit* u, int mode, f32 value) {
    if (u->precis == 0) {
        fn_801345D8(buf, size, "%c%d", 1, (int)(value * u->conv));
    } else {
        fn_801345D8(buf, size, "%c%.*f", 1, u->precis, value * u->conv);
    }
    switch (mode) {
    case 0:
        break;
    case 1:
        StrNCat(buf, u->sname + 1, size);
        break;
    case 2:
        StrNCat(buf, u->name + 1, size);
        break;
    }
}

char* fn_8013072C(char* buf, s32 size, s32 value) {
    int neg = value < 0;
    s32 groups;
    s32 n;
    s32 rest;
    s32 lead;
    char* p;
    n = value < 0 ? -value : value;
    groups = 0;
    rest = n / 1000;
    while (rest > 0) {
        n = rest;
        rest /= 1000;
        groups++;
    }
    if (n < 10) {
        lead = 1;
    } else {
        lead = n < 100 ? 2 : 3;
    }
    p = &buf[groups * 4 + lead + neg];
    p[1] = 0;
    if (value == 0) {
        *p-- = '0';
    } else {
        n = value < 0 ? -value : value;
        while (n > 0) {
            if (n >= 1000) {
                p[0] = n % 10 + '0';
                n /= 10;
                p[-1] = n % 10 + '0';
                n /= 10;
                p[-2] = n % 10 + '0';
                n /= 10;
                p[-3] = ',';
                p -= 4;
            } else {
                *p-- = n % 10 + '0';
                n /= 10;
            }
        }
        if (neg) {
            *p-- = '-';
        }
    }
    *p = 1;
    return p;
}

void fn_801308E4(char* buf, s32 size, int mode, f32 value) { fn_801305C4(buf, size, &lbl_8055E050.sys.s_dist, mode, value); }

char* fn_801308F4(char* buf, s32 size, s32 value) { return fn_8013072C(buf, size, value); }

void fn_801308F8(char* buf, s32 size, s32 value) {
    if (value >= 1000) {
        StrNCpy(buf, fn_80131234("Locale:OUT_OF_BOUNDS_SCORE"), size);
    } else {
        fn_8013072C(buf, size, value);
    }
}

void fn_80130980(char* buf, s32 size, int fields, f32 secs);

void fn_80130978(char* buf, s32 size, f32 secs) { fn_80130980(buf, size, 3, secs); }

void fn_80130980(char* buf, s32 size, int fields, f32 secs) {
    char out[100];
    char hBuf[10];
    char mBuf[10];
    char sBuf[10];
    char tBuf[10];
    char cBuf[10];
    char sep[8];
    s32 hundredths;
    s32 tenths;
    s32 seconds;
    s32 minutes;
    s32 hours;
    int started;
    char* p;
    if (secs == -lbl_80558DC0[0]) {
        StrNCpy(buf, fn_80131234("Locale:EMPTY_TIME_STRING"), size);
        return;
    }
    hundredths = (int)(100.0f * secs) % 10;
    tenths = (int)(10.0f * secs) % 10;
    seconds = (int)secs % 60;
    minutes = (int)secs / 60 % 60;
    hours = (int)secs / 3600;
    out[0] = 0;
    started = 0;
    if (fields >= 0 && hours > 0) {
        fn_801345D8(hBuf, sizeof(hBuf), "%d", hours);
        StrNCat(out, hBuf, sizeof(out));
        started = 1;
    }
    if (fields >= 1 && (minutes > 0 || started)) {
        if (started) {
            fn_801345D8(mBuf, sizeof(mBuf), "%s%0*d", ":", 2, minutes);
        } else {
            fn_801345D8(mBuf, sizeof(mBuf), "%d", minutes);
        }
        StrNCat(out, mBuf, sizeof(out));
        started = 1;
    }
    if (fields >= 2) {
        if (started) {
            fn_801345D8(sBuf, sizeof(sBuf), "%s%0*d", ":", 2, seconds);
        } else {
            fn_801345D8(sBuf, sizeof(sBuf), "%d", seconds);
        }
        StrNCat(out, sBuf, sizeof(out));
        started = 1;
    }
    if (fields >= 3) {
        fn_801345D8(sep, sizeof(sep), "%c", lbl_8055E050.sep);
        if (started) {
            fn_801345D8(tBuf, sizeof(tBuf), "%s%0*d", sep, 1, tenths);
        } else {
            fn_801345D8(tBuf, sizeof(tBuf), "%d", tenths);
        }
        StrNCat(out, tBuf, sizeof(out));
        started = 1;
    }
    if (fields >= 4) {
        if (started) {
            fn_801345D8(cBuf, sizeof(cBuf), "%s%0*d", "", 1, hundredths);
        } else {
            fn_801345D8(cBuf, sizeof(cBuf), "%d", hundredths);
        }
        StrNCat(out, cBuf, sizeof(out));
        started = 1;
    }
    fn_801345D8(buf, size, "%c", 1);
    {
        const DigitGlyph glyphs[] = {
            {'0', "\\d0"}, {'1', "\\d1"}, {'2', "\\d2"}, {'3', "\\d3"}, {'4', "\\d4"}, {'5', "\\d5"}, {'6', "\\d6"},
            {'7', "\\d7"}, {'8', "\\d8"}, {'9', "\\d9"}, {'-', "\\d-"}, {':', "\\d:"}, {'.', "\\d."},
        };
    for (p = out; *p; p++) {
        int found = 0;
        u32 j;
        for (j = 0; !found && j < sizeof(glyphs) / sizeof(glyphs[0]); j++) {
            if (*p == glyphs[j].c) {
                StrNCat(buf, glyphs[j].esc, size);
                found = 1;
            }
        }
        if (!found) {
            char one[2] = "";
            one[0] = *p;
            StrNCat(buf, one, size);
        }
    }
    }
}

int fn_80130FCC(char* buf, s32 size, s32 value) { return fn_801345D8(buf, size, "%c%d", 1, value); }

const char* fn_80130FE0(int n) {
    const char* ords[] = {
        "Ordinal:ZEROTH", "Ordinal:FIRST",   "Ordinal:SECOND", "Ordinal:THIRD",  "Ordinal:FOURTH",
        "Ordinal:FIFTH",  "Ordinal:SIXTH",   "Ordinal:SEVENTH", "Ordinal:EIGHTH", "Ordinal:NINTH",
    };
    if (n / 10 == 1) {
        return fn_80131234("Ordinal:TENTH");
    }
    return fn_80131234(ords[n % 10]);
}

int fn_801310B4(char* buf, s32 size, int n) { return fn_801345D8(buf, size, "%d%s", n, fn_80130FE0(n) + 1); }

void fn_80131308(void);

void fn_8013111C(int lang) {
    if (lang >= 0 && lang < sizeof(lbl_802F8E60) / sizeof(lbl_802F8E60[0])) {
        fn_8013028C();
        fn_80131308();
        fn_8012BEF0(lbl_802F8E60[lang].res);
        {
            const char* name = "lang.lng";
            s32 size = 0;
            u32 version;
            bool swap;
            void* data = fn_8012C370(name, 'LANG', &version, &size, 0, &swap);
            if (data) {
                memcpy(s_lang.buf, data, size);
                fn_8012C4D8(data);
                if (version == 1) {
                    fn_801301E0((LangFile*)s_lang.buf);
                } else {
                    fn_802E30C8("Bad version on %s", name);
                }
                s_lang.file = (LangFile*)s_lang.buf;
            } else {
                fn_802E30C8("Can't load %s", name);
            }
        }
        fn_8012FAB8("units.val");
        s_lang.cur = lang;
    }
}

const char* fn_80131234(const char* key) {
    const char* s = fn_801302E0(key);
    return s ? s : "???";
}

const char* fn_80131264(const char* key) { return fn_801302E0(key); }

const char* fn_80131268(const char* key) { return fn_80131234(key); }

char* fn_8013126C(char* dst, const char* src) {
    fn_802E317C("LocaleUnXlator - really shouldn't use this in non-DEBUG mode");
    StrNCpy(dst, src, 300);
    return dst;
}

}  // extern "C"

LocString::LocString(const char* k) {
    key = k;
    text = 0;
    version = ~lbl_8055E044;
}

extern "C" {

void fn_80131308(void) { lbl_8055E044++; }

void fn_80131318(LocString* s) {
    s->text = fn_801302E0(s->key);
    if (!s->text) {
        s->text = s->key;
    } else {
        s->version = lbl_8055E044;
    }
}

void fn_80131368(const char* s) {
    if (isdigit(*s)) {
        lbl_8055E04C = atoi(s);
    } else {
        u32 i;
        for (i = 0; i < sizeof(lbl_802F8E60) / sizeof(lbl_802F8E60[0]); i++) {
            if (!strncasecmp(s, lbl_802F8E60[i].res, strlen(s))) {
                lbl_8055E04C = i;
                break;
            }
        }
    }
}

}  // extern "C"
