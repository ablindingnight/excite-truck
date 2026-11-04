// Records screen (0x8025C028-0x8025CE78): the records/achievements layout ("records.lyt",
// "achievements", "tricks", "times", "track") with the car icon textures ("caricons0.tex", ...).
// Written against raw field offsets; the large screen routines are still stubs. Names are guesses
// apart from the strings.
#include "game/common.h"
#include "game/menu.h"
#include "game/menustate.h"

#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))

extern "C" {
s32 fn_80214D04(void);
void fn_80214CB4(s32 a);
void fn_80235380(u8* p);
}

void operator delete(void* p);

extern s32 lbl_8055C390;

ModuleTag lbl_8055F3A8;
UnkStatic123s lbl_80498EB8;
UnkStatic7F lbl_8055F3B0;

extern "C" {

void fn_8025C028(void) { lbl_8055C390 = fn_80214D04(); }

void fn_8025C04C(void) { fn_80214CB4(lbl_8055C390); }

void MenuDoAchievementsViewer(void);
void fn_8025CE78(void);
void fn_8025E034(void);
extern char lbl_8055B860[8];
extern char lbl_8055BAC0[4];  // ""
extern char lbl_8055BABC[4];  // ""

// Records menu: achievements, tricks or times until backed out.
void fn_8025C054(void) {
    MenuItem items[] = {
        MenuItem(lbl_8055BAC0, 8, (const void*)fn_8025C04C, 0, 0),
        MenuItem(lbl_8055BABC, 8, (const void*)fn_8025C028, 1, 0),
        MenuItem("achievements", 0, 0, 0, 0),
        MenuItem("tricks", 0, 0, 1, 0),
        MenuItem("times", 0, 0, 2, 0),
        MenuItem(lbl_8055B860, 6, 0, -1, 27),
        MenuItem(lbl_80498EB8),
    };
    MenuDesc desc = {"records.lyt", 0, 0, 0, 0};
    desc.items = items;
    int run = 1;
    while (run) {
        switch (fn_802146BC(&desc, -999, -999, 1)) {
        case 0:
            MenuDoAchievementsViewer();
            break;
        case 1:
            fn_8025CE78();
            break;
        case 2:
            fn_8025E034();
            break;
        case -1:
        default:
            run = 0;
            break;
        }
    }
}

}  // extern "C"

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

// One car icon's texture rectangles.
struct IconEntry {
    char name[0x20];  // offset 0x0, chassis
    u8* tex;          // offset 0x20
    u8* overlay;      // offset 0x24
    f32 u0;           // offset 0x28
    f32 v0;           // offset 0x2C
    f32 u1;           // offset 0x30
    f32 v1;           // offset 0x34
};

struct IconList {
    IconEntry* data;  // offset 0x0
    s32 count;        // offset 0x4
};

// The shared car icon table (caricons.val; also frname's and carpick's): loaded on first use, reference
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
extern "C" int strcasecmp(const char* a, const char* b);
extern "C" void gxForgetTexture(u8* tex);
extern "C" u8* fn_8023512C(const char* name);

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

// Index of the car icon for chassis `name`, or -1.
static inline int FindIcon(IconList* icons, const char* name) {
    for (int i = 0; i < icons->count; i++) {
        if (strcasecmp(icons->data[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

// A car icon in a records row: a menu state (vtable lbl_803453B8 = __vt__10RecordIcon, followed in .data by the
// zero-filled space of this unit's weak IMenuState copy). Constructed and destroyed inline as the stack array in
// the records screen; the deleting dtor (fn_8025CC14) is the weak out-of-line copy its vtable needs.
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

// One high score row: score, grade and time text.
struct ScoreRow {
    char score[20];     // offset 0x0
    const char* grade;  // offset 0x14
    char time[20];      // offset 0x18
};

struct HighScore {
    u8 pad0[0x4];
    s32 score;   // offset 0x4
    s32 grade;   // offset 0x8
    f32 time;    // offset 0xC
    u32 choice;  // offset 0x10
};

struct VehicleChoice {
    s32 vehicle;  // offset 0x0
    s32 color;    // offset 0x4
    u8 valid;     // offset 0x8
};

struct EventInfo {
    u8 pad0[0x4];
    s32 track;        // offset 0x4
    u32 id;           // offset 0x8
    u8 padC[0x14];
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

class Statistic {
public:
    Statistic(int slot, u32 type, long event, int difficulty);
    int HasValue(void);
    u8 pad[0x24];
};

class RealStatistic : public Statistic {
public:
    RealStatistic(int slot, u32 type, long event, int difficulty);
    f32 GetValue(void);
};

int IdentGetCurrentSlot(void);
HighScore* StatsGetHighScore(int slot, long event, long difficulty, int rank);
VehicleChoice StatsUnpackChoice(u32 packed);
const char* GetGradeDisplayName(int grade);
EventInfo* GetEvent(int i);
TrackInfo* GetTrack(int track);
VehicleInfo* GetVehicle(int i);
const char* GetTrackFriendlyName(const char* name);
const char* fn_80131234(const char* key);
extern f32 lbl_80558DC0[];  // FLT_MAX (MSL)

extern "C" {
void fn_801308F4(char* buf, s32 size, s32 value);
void fn_80130980(char* buf, s32 size, int fields, f32 secs);
int fn_801345D8(char* buf, int n, const char* fmt, ...);
s32 fn_802168E4(void);
void fn_80214A9C(s32 s);
void fn_80214AF0(s32 s);

// The event records screen ("ep_records.lyt"): the five best scores of one event and difficulty with
// their grades, times and cars, the event's best time and its location.
void fn_8025C204(int event, int difficulty) {
    if (lbl_8055F2E8 == 0) {
        lbl_8055F2E4 = new ("cimgr") CarIconTable;
    }
    lbl_8055F2E8++;
    CarIconTable* set = lbl_8055F2E4;
    ScoreRow rows[5];
    RecordIcon icons[5];
    icons[0].icons = set;
    icons[0].visible = 0;
    icons[1].icons = set;
    icons[1].visible = 0;
    icons[2].icons = set;
    icons[2].visible = 0;
    icons[3].icons = set;
    icons[3].visible = 0;
    icons[4].icons = set;
    icons[4].visible = 0;
    for (int i = 0; i < 5; i++) {
        CopyName("", 20, rows[i].score);
        rows[i].grade = "";
        f32 time = -lbl_80558DC0[0];
        HighScore* hs = StatsGetHighScore(IdentGetCurrentSlot(), event, difficulty, i);
        if (hs) {
            fn_801308F4(rows[i].score, 20, hs->score);
            if (hs->grade != -1) {
                rows[i].grade = GetGradeDisplayName(hs->grade);
            }
            time = hs->time;
            icons[i].visible = 1;
            VehicleChoice c = StatsUnpackChoice(hs->choice);
            int color = c.color;
            icons[i].valid = c.valid;
            VehicleInfo* v = GetVehicle(c.vehicle);
            icons[i].icon = FindIcon(icons[i].icons, v->chassis);
            icons[i].color = v->colors[color];
        }
        fn_80130980(rows[i].time, 20, 3, time);
    }
    EventInfo* e = GetEvent(event);
    const char* track;
    if (e->node->Has("track")) {
        track = GetTrackFriendlyName(e->node->GetValue("track"));
    } else {
        track = GetTrackFriendlyName(GetTrack(e->track)->name);
    }
    char location[1000];
    fn_801345D8(location, 1000, "%s - %s", track, e->node->GetValue("friendly_name") + 1);
    int extra = GetEvent(event)->id != 0x180302;
    const char* head = fn_80131234("Records:TIME");
    if (extra) {
        head = fn_80131234("Records:EXTRA_TIME");
    }
    char best[20];
    RealStatistic stat(IdentGetCurrentSlot(), 0x20507, event, difficulty);
    if (stat.HasValue()) {
        fn_80130980(best, 20, 3, stat.GetValue());
    } else {
        CopyName(fn_80131234("EventPath:NO_BEST_TIME"), 20, best);
    }
    MenuItem items[] = {
        MenuItem("location", location),
        MenuItem("best_time", 3, best, 0, 0),
        MenuItem("time_head", 3, head, 0, 0),
        MenuItem("#score_1", 3, rows[0].score, 0, 0),
        MenuItem("#grade_1", 3, rows[0].grade, 0, 0),
        MenuItem("#time_1", rows[0].time),
        MenuItem("car_icon_1", 5, &icons[0], 0, 0),
        MenuItem("#score_2", 3, rows[1].score, 0, 0),
        MenuItem("#grade_2", rows[1].grade),
        MenuItem("#time_2", rows[1].time),
        MenuItem("car_icon_2", 5, &icons[1], 0, 0),
        MenuItem("#score_3", rows[2].score),
        MenuItem("#grade_3", rows[2].grade),
        MenuItem("#time_3", rows[2].time),
        MenuItem("car_icon_3", 5, &icons[2], 0, 0),
        MenuItem("#score_4", rows[3].score),
        MenuItem("#grade_4", rows[3].grade),
        MenuItem("#time_4", rows[3].time),
        MenuItem("car_icon_4", 5, &icons[3], 0, 0),
        MenuItem("#score_5", rows[4].score),
        MenuItem("#grade_5", rows[4].grade),
        MenuItem("#time_5", rows[4].time),
        MenuItem("car_icon_5", 5, &icons[4], 0, 0),
        MenuItem(lbl_8055B860, 6, 0, -2, 27),
        MenuItem(lbl_80498EB8),
    };
    s32 music = fn_802168E4();
    MenuDesc desc = {"ep_records.lyt", items, NULL, 0, 0};
    if (music) {
        fn_80214A9C(0x1F);
    }
    fn_802146BC(&desc, -999, -999, 1);
    if (music) {
        fn_80214AF0(0x1F);
    }
    if (--lbl_8055F2E8 == 0) {
        delete lbl_8055F2E4;
        lbl_8055F2E4 = 0;
    }
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
    fw = (f32)w;
    fh = (f32)h;
    fn_802354A0(art, valid, sx - 3.0f, 3.0f + sy, 0.0f, 6.0f + fw, 6.0f + fh, -1);
    e = &(*(IconEntry**)icons)[icon];
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


}  // extern "C"
