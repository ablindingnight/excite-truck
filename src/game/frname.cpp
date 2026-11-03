// Friendly name screen (0x8025CE78-0x8025E034): the player name entry ("friendly_name") with the car
// icons ("caricona.art", "caricons0.tex", ...). Written against raw field offsets; the large screen
// routines are still stubs. Names are guesses apart from the strings.
#include "game/common.h"
#include "game/menu.h"

#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

extern "C" {
void fn_80235380(u8* p);
void fn_80214A9C(s32 s);
u8* fn_8023512C(const char* name);
}

void operator delete(void* p);

extern void* lbl_80345718[];
extern char lbl_8055C3E0[1];

// Copies at most n - 1 characters and always terminates.
static inline void CopyName(const char* src, int n, char* dst) {
    char* d = dst;
    int left = n;
    while ((*d++ = *src++) != 0) {
        if (--left <= 0) {
            d[-1] = 0;
            break;
        }
    }
}
extern s32 lbl_8055F3C4;

ModuleTag lbl_8055F3B8;
UnkStatic123s lbl_80498ED0;
UnkStatic7F lbl_8055F3C0;

// One car icon's texture rectangles.
struct IconEntry {
    char name[0x20];  // offset 0x0, chassis
    u8* tex;        // offset 0x20
    u8* overlay;    // offset 0x24
    f32 u0;         // offset 0x28
    f32 v0;         // offset 0x2C
    f32 u1;         // offset 0x30
    f32 v1;         // offset 0x34
};

struct IconList {
    IconEntry* data;  // offset 0x0
    s32 count;        // offset 0x4
};

// One stat record line on the friendly-name screen.
struct RecordLine {
    const char* label;     // offset 0x0
    char value[0x14];      // offset 0x4
    const char* track;     // offset 0x18
    const char* name;      // offset 0x1C
    char text[1000];       // offset 0x20
    s32 difficulty;        // offset 0x408
    u8 pad40C[0x18];
    IconList* icons;       // offset 0x424
    s32 icon;              // offset 0x428
    u8 hasIcon;            // offset 0x42C
    u32 color;             // offset 0x430
    u8 pad434[0x4];
    u8 valid;              // offset 0x438
};

// The shared car icon table (caricons.val; also carpick's CarIconSet): loaded on first use, reference
// counted through lbl_8055F2E8. CLEANUP: the original keeps this class in a shared header.
template<int N> struct IconTail {
    static void CopyTail(const char* s, int n, char* d) {
        while ((*d++ = *s++) != 0) {
            if (--n <= 0) { d[-1] = 0; break; }
        }
    }
    static void Append(char* text, const char* s) {
        char* d = text;
        int n = N;
        while (*d) { n--; d++; }
        if (n > 0) { CopyTail(s, n, d); }
    }
};
void* operator new(size_t n, const char* tag);
void* operator new[](size_t n, const char* tag);
void operator delete[](void* p);
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* t);
extern "C" u8* fn_802CAC28(const char* name, int flags);
extern "C" int strcmp(const char* a, const char* b);
extern "C" void gxForgetTexture(u8* tex);

struct CarIconTable : public IconList {
    CarIconTable(void) {
        ValueTable* t = fn_80131808("caricons.val");
        count = 0;
        data = new ("cicon") IconEntry[50];
        ValueNode* n;
        for (int i = 0; (n = t->GetChild(i)) != 0; i++) {
            if (strcmp(n->GetName(), "Car")) {
                continue;
            }
            for (int k = 0; k < 2; k++) {
                IconEntry* c = &data[count++];
                CopyName(n->GetValue("chassis"), 32, c->name);
                if (k) {
                    IconTail<32>::Append(c->name, "_e");
                }
                c->tex = fn_802CAC28(k ? "caricons0e.tex" : "caricons0.tex", 0);
                c->overlay = fn_802CAC28(k ? "caricons1e.tex" : "caricons1.tex", 0);
                c->u0 = n->GetFloat("u1");
                c->v0 = n->GetFloat("v1");
                c->u1 = n->GetFloat("u2");
                c->v1 = n->GetFloat("v2");
            }
        }
        fn_80131818(t);
    }
    ~CarIconTable(void) {
        for (int i = 0; i < count; i++) {
            IconEntry& e = data[i];
            gxForgetTexture(e.tex);
            gxForgetTexture(e.overlay);
        }
        delete[] data;
    }
};
extern CarIconTable* lbl_8055F2E4;
extern s32 lbl_8055F2E8;

// The group list the stunt-records screen hides (FriendlyNames' layout, built and torn down in place).
struct GroupHider {
    void* vtable;
    u8 pad4[0x10];
    void* owner;      // offset 0x14
    u32* groups;      // offset 0x18
    int count;        // offset 0x1C
    int cap;          // offset 0x20
    u32 storage[30];  // offset 0x24
    u32& Next(void) { return groups[count++]; }
};
extern void* lbl_803456B8[];

int GetEveMode(void);
void UnloadEves(void);
void LoadModeEves(int mode);
extern "C" u8* fn_8025D9D0(u8* p);
extern "C" void fn_8025DAEC(RecordLine* r, u32 type);
extern "C" u8 lbl_8055B860;

// Hides the HUD groups of the two difficulties a stunt record was not set on.
static inline void HideOtherDifficulties(int line, RecordLine* l) {
    for (int d = 0; d < 3; d++) {
        if (d != l->difficulty) {
            GroupHider* h = (GroupHider*)lbl_8055F3C4;
            h->Next() = line + 1 + d * 10;
        }
    }
}

// Releases one record line (its car icon widget).
static inline void FreeRecordLine(RecordLine* l) {
    *(void**)l->pad40C = lbl_80345718;
    fn_80235380(*(u8**)((u8*)l + 0x434));
}

extern "C" {

// The stunt records screen ("tricks.lyt"): the best of five stunt stats, each with where it was set and
// the car it was set with; the difficulty columns that did not set a record are hidden.
void fn_8025CE78(void) {
    int mode = GetEveMode();
    if (mode != 0x130) {
        UnloadEves();
        LoadModeEves(0x130);
    }
    RecordLine l6;
    RecordLine l5;
    RecordLine l4;
    RecordLine l3;
    RecordLine l2;
    RecordLine l1;
    RecordLine l0;
    fn_8025D9D0((u8*)&l0);
    fn_8025D9D0((u8*)&l1);
    fn_8025D9D0((u8*)&l2);
    fn_8025D9D0((u8*)&l3);
    fn_8025D9D0((u8*)&l4);
    fn_8025D9D0((u8*)&l5);
    fn_8025D9D0((u8*)&l6);
    if (lbl_8055F2E8 == 0) {
        lbl_8055F2E4 = new ("cimgr") CarIconTable;
    }
    lbl_8055F2E8++;
    l0.icons = lbl_8055F2E4;
    l1.icons = lbl_8055F2E4;
    l2.icons = lbl_8055F2E4;
    l3.icons = lbl_8055F2E4;
    l4.icons = lbl_8055F2E4;
    l5.icons = lbl_8055F2E4;
    l6.icons = lbl_8055F2E4;
    GroupHider hider;
    hider.vtable = lbl_803456B8;
    hider.owner = 0;
    hider.groups = hider.storage;
    hider.count = 0;
    hider.cap = 30;
    lbl_8055F3C4 = (s32)&hider;
    fn_8025DAEC(&l0, 0x40901);
    HideOtherDifficulties(0, &l0);
    fn_8025DAEC(&l1, 0x40902);
    HideOtherDifficulties(1, &l1);
    fn_8025DAEC(&l2, 0x41206);
    HideOtherDifficulties(2, &l2);
    fn_8025DAEC(&l3, 0x48203);
    HideOtherDifficulties(3, &l3);
    fn_8025DAEC(&l4, 0x50204);
    HideOtherDifficulties(4, &l4);
    MenuItem items[] = {
        MenuItem(NULL, 5, &hider, 0, 0),
        MenuItem("stunt_name_1", l0.label),
        MenuItem("stunt_record_1", l0.value),
        MenuItem("stunt_location_1", l0.text),
        MenuItem("car_icon_1", 5, l0.pad40C, 0, 0),
        MenuItem("stunt_name_2", l1.label),
        MenuItem("stunt_record_2", l1.value),
        MenuItem("stunt_location_2", l1.text),
        MenuItem("car_icon_2", 5, l1.pad40C, 0, 0),
        MenuItem("stunt_name_3", l2.label),
        MenuItem("stunt_record_3", l2.value),
        MenuItem("stunt_location_3", l2.text),
        MenuItem("car_icon_3", 5, l2.pad40C, 0, 0),
        MenuItem("stunt_name_4", l3.label),
        MenuItem("stunt_record_4", l3.value),
        MenuItem("stunt_location_4", l3.text),
        MenuItem("car_icon_4", 5, l3.pad40C, 0, 0),
        MenuItem("stunt_name_5", l4.label),
        MenuItem("stunt_record_5", l4.value),
        MenuItem("stunt_location_5", l4.text),
        MenuItem("car_icon_5", 5, l4.pad40C, 0, 0),
        MenuItem("stunt_name_6", l5.label),
        MenuItem("stunt_record_6", l5.value),
        MenuItem("stunt_location_6", l5.text),
        MenuItem("car_icon_6", 5, l5.pad40C, 0, 0),
        MenuItem("stunt_name_7", l6.label),
        MenuItem("stunt_record_7", l6.value),
        MenuItem("stunt_location_7", l6.text),
        MenuItem("car_icon_7", 5, l6.pad40C, 0, 0),
        MenuItem((const char*)&lbl_8055B860, 6, NULL, -1, 0x1B),
        MenuItem(lbl_80498ED0),
    };
    MenuDesc desc = {"tricks.lyt", items, NULL, 0, 0};
    fn_802146BC(&desc, -999, -999, 1);
    if (--lbl_8055F2E8 == 0) {
        delete lbl_8055F2E4;
        lbl_8055F2E4 = 0;
    }
    if (mode != 0x130) {
        UnloadEves();
        LoadModeEves(mode);
    }
    lbl_8055F3C4 = 0;
    hider.groups = 0;
    hider.count = 0;
    hider.cap = 0;
    FreeRecordLine(&l6);
    FreeRecordLine(&l5);
    FreeRecordLine(&l4);
    FreeRecordLine(&l3);
    FreeRecordLine(&l2);
    FreeRecordLine(&l1);
    FreeRecordLine(&l0);
}

}  // extern "C"

template <class T>
class ArrayBase {
public:
    ~ArrayBase(void) {
        data = 0;
        count = 0;
        cap = 0;
    }
    T* data;    // offset 0x0
    int count;  // offset 0x4
    int cap;    // offset 0x8
};

template <class T>
class SimpleArray : public ArrayBase<T> {};

// The friendly-name list (one per screen; lbl_8055F3C4 is cleared when it goes away).
class FriendlyNames {
public:
    ~FriendlyNames(void);
    u8 pad0[0x18];
    SimpleArray<u32> names;  // offset 0x18
};

FriendlyNames::~FriendlyNames(void) { lbl_8055F3C4 = 0; }

extern "C" {

u8* fn_8025D9D0(u8* p) {
    U32(p, 0x420) = 0;
    *(void**)(p + 0x40C) = lbl_80345718;
    U32(p, 0x424) = 0;
    U8(p, 0x42C) = 0;
    PTR(p, 0x434) = fn_8023512C("caricona.art");
    PTR(p, 0x0) = (u8*)lbl_8055C3E0;
    CopyName(lbl_8055C3E0, 0x14, (char*)(p + 0x4));
    U32(p, 0x408) = 0;
    char* empty;
    PTR(p, 0x1C) = (u8*)(empty = lbl_8055C3E0);
    PTR(p, 0x18) = (u8*)empty;
    U8(p, 0x20) = 0;
    U8(p, 0x42C) = 0;
    return p;
}

u8* fn_8025DA88(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_80345718;
        fn_80235380(PTR(p, 0x28));
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

}  // extern "C"


// The car and colour a record was set with.
struct StatChoice {
    s32 vehicle;  // offset 0x0
    s32 color;    // offset 0x4
    u8 valid;     // offset 0x8
};

class Statistic {
public:
    Statistic(int slot, u32 type, long event, int difficulty);
    int HasValue(void);
    int IsBetterThan(const Statistic* o) const;
    void Format(char* buf, int n);
    StatChoice GetChoice(void);
    u8 pad[0x24];
};

struct EventInfo {
    u8 pad0[0x4];
    s32 track;        // offset 0x4
    u8 pad8[0x18];
    ValueNode* node;  // offset 0x20
};

struct TrackInfo {
    u8 pad0[0x8];
    char name[1];  // offset 0x8
};

struct VehicleInfo {
    u8 pad0[0x2C];
    char chassis[0x20];  // offset 0x2C
    u8 pad4C[0x18];
    u32 colors[12];      // offset 0x64
};

int IdentGetCurrentSlot(void);
int GetNumEvents(void);
EventInfo* GetEvent(int i);
TrackInfo* GetTrack(int track);
VehicleInfo* GetVehicle(int i);
const char* StatsGetLabel(u32 type);
const char* GetTrackFriendlyName(const char* name);

extern "C" {
int fn_801345D8(char* buf, int n, const char* fmt, ...);
int strcasecmp(const char* a, const char* b);


// Index of the car icon for chassis `name`, or -1.
static inline int FindIcon(IconList* icons, const char* name) {
    for (int i = 0; i < icons->count; i++) {
        if (strcasecmp(icons->data[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

// Fills one record line for stat `type`: the best value over every event and difficulty, where it was
// set ("%s - %s": track, friendly name) and the car icon and colour it was set with.
void fn_8025DAEC(RecordLine* r, u32 type) {
    int bestEv;
    int bestD;
    int diff;
    int ev;
    bestEv = -1;
    bestD = -1;
    for (diff = 0; diff < 3; diff++) {
        for (ev = 0; ev < GetNumEvents(); ev++) {
            Statistic s(IdentGetCurrentSlot(), type, ev, diff);
            if (s.HasValue()) {
                if (bestEv < 0) {
                    bestEv = ev;
                    bestD = diff;
                } else {
                    Statistic b(IdentGetCurrentSlot(), type, bestEv, bestD);
                    if (s.IsBetterThan(&b)) {
                        bestEv = ev;
                        bestD = diff;
                    }
                }
            }
        }
    }
    r->label = StatsGetLabel(type);
    if (bestEv < 0) {
        CopyName("-", 0x14, r->value);
        CopyName("---", 1000, r->text);
        r->name = "-";
        bestD = 0;
        r->track = "-";
    } else {
        Statistic s(IdentGetCurrentSlot(), type, bestEv, bestD);
        s.Format(r->value, 0x14);
        EventInfo* e = GetEvent(bestEv);
        if (e->node->Has("track")) {
            r->track = GetTrackFriendlyName(e->node->GetValue("track"));
        } else {
            r->track = GetTrackFriendlyName(GetTrack(e->track)->name);
        }
        r->name = e->node->GetValue("friendly_name");
        fn_801345D8(r->text, 1000, "%s - %s", r->track, r->name + 1);
        StatChoice c = s.GetChoice();
        int color = c.color;
        r->valid = c.valid;
        VehicleInfo* v = GetVehicle(c.vehicle);
        r->icon = FindIcon(r->icons, v->chassis);
        r->color = v->colors[color];
        r->hasIcon = 1;
    }
    r->difficulty = bestD;
}

}  // extern "C"


// Scales each colour channel by 255*255/65536, alpha forced opaque.
static inline u32 DimColor(u32 c) {
    int b = (u8)c * 255 * 255 / 256 / 256;
    int g = (u8)(c >> 8) * 255 * 255 / 256 / 256;
    int r = (u8)(c >> 16) * 255 * 255 / 256 / 256;
    return 0xFF000000 | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
}

extern "C" {
void fn_80230C30(f32* x, f32* y, s32 lx, s32 ly);
void fn_802354A0(u8* art, int part, f32 x, f32 y, f32 z, f32 w, f32 h, u32 color);
void fn_80231220(f32 x, f32 y, f32 w, f32 h, u8* tex, u32 color, f32 u0, f32 v0, f32 u1, f32 v1);

// The chosen car icon in a frame, the overlay tinted with the player's colour.
void fn_8025DDD0(u8* p) {
    if (!U8(p, 0x20)) {
        return;
    }
    u32 c;
    IconEntry* e;
    f32 sx;
    f32 sy;
    f32 x;
    f32 y;
    f32 v1;
    f32 u1;
    f32 v0;
    f32 u0;
    f32 fw;
    f32 fh;
    fn_80230C30(&sx, &sy, S32(p, 0x4), S32(p, 0x8));
    c = U8(p, 0x2C);
    fw = (f32)S32(p, 0xC);
    fh = (f32)S32(p, 0x10);
    f32 shadowY = 3.0f + sy;
    fn_802354A0(PTR(p, 0x28), c, sx - 3.0f, shadowY, 0.0f, 6.0f + fw, 6.0f + fh, -1);
    e = &((IconEntry*)*(u8**)PTR(p, 0x18))[S32(p, 0x1C)];
    y = sy;
    x = sx;
    u0 = e->u0;
    v0 = e->v0;
    u1 = e->u1;
    v1 = e->v1;
    c = U32(p, 0x24);
    fn_80231220(x, y, fw, fh, e->tex, -1, u0, v0, u1, v1);
    fn_80231220(x, y, fw, fh, e->overlay, DimColor(c), u0, v0, u1, v1);
}

void fn_8025DF80(u8* p) {
    for (int i = 0; i < S32(p, 0x1C); i++) {
        fn_80214A9C(((s32*)PTR(p, 0x18))[i]);
    }
}

}  // extern "C"
