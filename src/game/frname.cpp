// Friendly name screen (0x8025CE78-0x8025E034): the player name entry ("friendly_name") with the car
// icons ("caricona.art", "caricons0.tex", ...). Written against raw field offsets; the large screen
// routines are still stubs. Names are guesses apart from the strings.
#include "game/common.h"
#include "game/menu.h"
#include "game/menustate.h"

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

extern "C" u8* fn_8023512C(const char* name);
extern "C" void fn_80235380(u8* p);

// A record line's car icon: a menu state (vtable lbl_80345718, followed in .data by the zero-filled space of this
// unit's weak IMenuState copy). Same shape as recscreen's. Declared before FriendlyNames: MWCC emits vtables in
// reverse declaration order.
namespace {
class RecordIcon : public MenuState {
public:
    RecordIcon(void) {
        icons = 0;
        visible = 0;
        art = fn_8023512C("caricona.art");
    }
    virtual ~RecordIcon(void) { fn_80235380(art); }
    virtual void Draw(void);

    IconList* icons;  // offset 0x18
    s32 icon;         // offset 0x1C
    u8 visible;       // offset 0x20
    u32 color;        // offset 0x24
    u8* art;          // offset 0x28
    u8 valid;         // offset 0x2C
};
}  // namespace

// One stat record line on the friendly-name screen.
struct RecordLine {
    RecordLine(void);

    const char* label;     // offset 0x0
    char value[0x14];      // offset 0x4
    const char* track;     // offset 0x18
    const char* name;      // offset 0x1C
    char text[1000];       // offset 0x20
    s32 difficulty;        // offset 0x408
    RecordIcon widget;     // offset 0x40C
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

// The HUD groups the stunt-records screen hides, a menu state built and torn down on the screen's stack
// (lbl_8055F3C4 points at it).
class FriendlyNames : public MenuState {
public:
    FriendlyNames(void) {
        names.data = storage;
        names.count = 0;
        names.cap = 30;
        lbl_8055F3C4 = (s32)this;
    }
    virtual ~FriendlyNames(void) { lbl_8055F3C4 = 0; }
    virtual void Enter(void);
    u32& Next(void) { return names.data[names.count++]; }

    SimpleArray<u32> names;  // offset 0x18
    u32 storage[30];         // offset 0x24
};

int GetEveMode(void);
void UnloadEves(void);
void LoadModeEves(int mode);
extern "C" void fn_8025DAEC(RecordLine* r, u32 type);
extern "C" u8 lbl_8055B860;

// Hides the HUD groups of the two difficulties a stunt record was not set on.
static inline void HideOtherDifficulties(int line, RecordLine* l) {
    for (int d = 0; d < 3; d++) {
        if (d != l->difficulty) {
            FriendlyNames* h = (FriendlyNames*)lbl_8055F3C4;
            h->Next() = line + 1 + d * 10;
        }
    }
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
    RecordLine lines[7];
    if (lbl_8055F2E8 == 0) {
        lbl_8055F2E4 = new ("cimgr") CarIconTable;
    }
    lbl_8055F2E8++;
    lines[0].widget.icons = lbl_8055F2E4;
    lines[1].widget.icons = lbl_8055F2E4;
    lines[2].widget.icons = lbl_8055F2E4;
    lines[3].widget.icons = lbl_8055F2E4;
    lines[4].widget.icons = lbl_8055F2E4;
    lines[5].widget.icons = lbl_8055F2E4;
    lines[6].widget.icons = lbl_8055F2E4;
    FriendlyNames hider;
    fn_8025DAEC(&lines[0], 0x40901);
    HideOtherDifficulties(0, &lines[0]);
    fn_8025DAEC(&lines[1], 0x40902);
    HideOtherDifficulties(1, &lines[1]);
    fn_8025DAEC(&lines[2], 0x41206);
    HideOtherDifficulties(2, &lines[2]);
    fn_8025DAEC(&lines[3], 0x48203);
    HideOtherDifficulties(3, &lines[3]);
    fn_8025DAEC(&lines[4], 0x50204);
    HideOtherDifficulties(4, &lines[4]);
    MenuItem items[] = {
        MenuItem(NULL, 5, &hider, 0, 0),
        MenuItem("stunt_name_1", 3, lines[0].label, 0, 0),
        MenuItem("stunt_record_1", 3, lines[0].value, 0, 0),
        MenuItem("stunt_location_1", 3, lines[0].text, 0, 0),
        MenuItem("car_icon_1", 5, &lines[0].widget, 0, 0),
        MenuItem("stunt_name_2", 3, lines[1].label, 0, 0),
        MenuItem("stunt_record_2", lines[1].value),
        MenuItem("stunt_location_2", lines[1].text),
        MenuItem("car_icon_2", 5, &lines[1].widget, 0, 0),
        MenuItem("stunt_name_3", lines[2].label),
        MenuItem("stunt_record_3", lines[2].value),
        MenuItem("stunt_location_3", lines[2].text),
        MenuItem("car_icon_3", 5, &lines[2].widget, 0, 0),
        MenuItem("stunt_name_4", lines[3].label),
        MenuItem("stunt_record_4", lines[3].value),
        MenuItem("stunt_location_4", lines[3].text),
        MenuItem("car_icon_4", 5, &lines[3].widget, 0, 0),
        MenuItem("stunt_name_5", lines[4].label),
        MenuItem("stunt_record_5", lines[4].value),
        MenuItem("stunt_location_5", lines[4].text),
        MenuItem("car_icon_5", 5, &lines[4].widget, 0, 0),
        MenuItem("stunt_name_6", lines[5].label),
        MenuItem("stunt_record_6", lines[5].value),
        MenuItem("stunt_location_6", lines[5].text),
        MenuItem("car_icon_6", 5, &lines[5].widget, 0, 0),
        MenuItem("stunt_name_7", lines[6].label),
        MenuItem("stunt_record_7", lines[6].value),
        MenuItem("stunt_location_7", lines[6].text),
        MenuItem("car_icon_7", 5, &lines[6].widget, 0, 0),
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
}

}  // extern "C"


extern "C" {

}  // extern "C"

RecordLine::RecordLine(void) {
    label = lbl_8055C3E0;
    CopyName(lbl_8055C3E0, 0x14, value);
    difficulty = 0;
    char* empty;
    name = (empty = lbl_8055C3E0);
    track = empty;
    text[0] = 0;
    widget.visible = 0;
}

extern "C" {

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
        r->widget.valid = c.valid;
        VehicleInfo* v = GetVehicle(c.vehicle);
        r->widget.icon = FindIcon(r->widget.icons, v->chassis);
        r->widget.color = v->colors[color];
        r->widget.visible = 1;
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
void RecordIcon::Draw(void) {
    if (!visible) {
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
    fn_80230C30(&sx, &sy, this->x, this->y);
    c = valid;
    fw = (f32)w;
    fh = (f32)h;
    f32 shadowY = 3.0f + sy;
    fn_802354A0(art, c, sx - 3.0f, shadowY, 0.0f, 6.0f + fw, 6.0f + fh, -1);
    e = &icons->data[icon];
    y = sy;
    x = sx;
    u0 = e->u0;
    v0 = e->v0;
    u1 = e->u1;
    v1 = e->v1;
    c = color;
    fn_80231220(x, y, fw, fh, e->tex, -1, u0, v0, u1, v1);
    fn_80231220(x, y, fw, fh, e->overlay, DimColor(c), u0, v0, u1, v1);
}

void FriendlyNames::Enter(void) {
    for (int i = 0; i < names.count; i++) {
        fn_80214A9C(names.data[i]);
    }
}

}  // extern "C"
