// Friendly name screen (0x8025CE78-0x8025E034): the player name entry ("friendly_name") with the car
// icons ("caricona.art", "caricons0.tex", ...). Written against raw field offsets; the large screen
// routines are still stubs. Names are guesses apart from the strings.
#include "game/common.h"

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

extern "C" {

void fn_8025CE78(u8* p) {}  // stub (701)

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
    int b = (int)(c & 0xFF) * 255 * 255 / 256 / 256;
    int g = (int)((c >> 8) & 0xFF) * 255 * 255 / 256 / 256;
    int r = (int)((c >> 16) & 0xFF) * 255 * 255 / 256 / 256;
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
    fw = (f32)S32(p, 0xC);
    fh = (f32)S32(p, 0x10);
    fn_802354A0(PTR(p, 0x28), U8(p, 0x2C), sx - 3.0f, 3.0f + sy, 0.0f, 6.0f + fw, 6.0f + fh, -1);
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
