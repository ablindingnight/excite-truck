// Records screen (0x8025C028-0x8025CE78): the records/achievements layout ("records.lyt",
// "achievements", "tricks", "times", "track") with the car icon textures ("caricons0.tex", ...).
// Written against raw field offsets; the large screen routines are still stubs. Names are guesses
// apart from the strings.
#include "game/common.h"
#include "game/gart.h"
#include "game/caricontable.h"
#include "game/gamedb.h"
#include "game/highscore.h"
#include "game/menu.h"
#include "game/menustate.h"
#include "game/buildquery.h"

#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))

extern "C" {
s32 fn_80214D04(void);
void fn_80214CB4(s32 a);
}

void operator delete(void* p);

s32 lbl_8055C390 = -1;

ModuleTag lbl_8055F3A8;
UnkStatic123s lbl_80498EB8;
UnkStatic7F lbl_8055F3B0;

extern "C" {

void fn_8025C028(void) { lbl_8055C390 = fn_80214D04(); }

void fn_8025C04C(void) { fn_80214CB4(lbl_8055C390); }

void MenuDoAchievementsViewer(void);
void InitializeStuntRecordsScreen(void);
void InitializeCarSelectionScreen(void);
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
            InitializeStuntRecordsScreen();
            break;
        case 2:
            InitializeCarSelectionScreen();
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

void* operator new(size_t n, const char* tag);
void* operator new[](size_t n, const char* tag);
void operator delete[](void* p);
extern "C" ValueTable* fn_80131808(const char* name);
extern "C" void fn_80131818(ValueTable* t);
extern "C" u8* fn_802CAC28(const char* name, int flags);
extern "C" int strcmp(const char* a, const char* b);
extern "C" int strcasecmp(const char* a, const char* b);
extern "C" void gxForgetTexture(u8* tex);

extern s32 lbl_8055F2E8;

// Index of the car icon for chassis `name`, or -1.
static inline int FindIcon(CarIconTable* icons, const char* name) {
    for (int i = 0; i < icons->count; i++) {
        if (strcasecmp(icons->icons[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

// A car icon in a records row: a menu state (vtable lbl_803453B8 = __vt__10RecordIcon, followed in .data by the
// zero-filled space of this unit's weak IMenuState copy). Constructed and destroyed inline as the stack array in
// the records screen; the deleting dtor (fn_8025CC14) is the weak out-of-line copy its vtable needs.
namespace {
#include "game/recordicon.h"
#include "game/strcopy.h"
#include "game/mathutil.h"
}  // namespace

// One high score row: score, grade and time text.
struct ScoreRow {
    char score[20];     // offset 0x0
    const char* grade;  // offset 0x14
    char time[20];      // offset 0x18
};

// HighScore: game/highscore.h.
#include "game/vehchoice.h"

#include "game/statistic.h"


int IdentGetCurrentSlot(void);
HighScore* StatsGetHighScore(int slot, long event, long difficulty, int rank);
VehicleChoice StatsUnpackChoice(u32 packed);
const char* GetGradeDisplayName(int grade);
const char* GetTrackFriendlyName(const char* name);
extern "C" const char* fn_80131234(const char* key);
extern f32 __float_max[];  // FLT_MAX (MSL)

extern "C" {
void FormatIntegerBuffer(char* buf, s32 size, s32 value);
void fn_80130980(char* buf, s32 size, int fields, f32 secs);
int fn_801345D8(char* buf, int n, const char* fmt, ...);
s32 fn_802168E4(void);
void fn_80214A9C(s32 s);
void fn_80214AF0(s32 s);

// Whether a record row shows its car icon. MWCC: an enum-typed local that holds a constant is not propagated
// by the front end, so it stays a variable and the back end gives it its own `li` (literals share one register:
// FACTS 2026-11-17 G5 run 3). Retail loads a fresh zero for the five stores below. Names unproven.
enum IconShow {
    kIconHidden,  // 0
    kIconShown
};

// The event records screen ("ep_records.lyt"): the five best scores of one event and difficulty with
// their grades, times and cars, the event's best time and its location.
void fn_8025C204(int event, int difficulty) {
    CarIconTable::Acquire();
    CarIconTable* set = lbl_8055F2E4;
    ScoreRow rows[5];
    RecordIcon icons[5];
    IconShow hidden = kIconHidden;
    // Helper name unproven; PROVISIONAL (game/buildquery.h): retail starts a new scheduling block after the last
    // inlined ctor (the icons address and the hidden flag are formed there) with no instruction left of a test.
    // Open: this spelling also leaves a join before the loop (one extra li of the loop zero).
    if (BuildMode() == kBuildRetail) {
        icons[0].icons = set;
        icons[0].visible = hidden;
        icons[1].icons = set;
        icons[1].visible = hidden;
        icons[2].icons = set;
        icons[2].visible = hidden;
        icons[3].icons = set;
        icons[3].visible = hidden;
        icons[4].icons = set;
        icons[4].visible = hidden;
    }
    for (int i = 0; i < 5; i++) {
        SafeStrCopy(rows[i].score, "-", 20);
        rows[i].grade = "-";
        f32 time = -__float_max[0];
        HighScore* hs = StatsGetHighScore(IdentGetCurrentSlot(), event, difficulty, i);
        if (hs) {
            FormatIntegerBuffer(rows[i].score, 20, hs->score);
            if (hs->grade != -1) {
                rows[i].grade = GetGradeDisplayName(hs->grade);
            }
            time = hs->time;
            icons[i].visible = 1;
            VehicleChoice c = StatsUnpackChoice(hs->choice);
            int color = c.color;
            icons[i].valid = c.flag;
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
    int extra = GetEvent(event)->type != 0x180302;
    const char* head = fn_80131234("Records:TIME");
    if (extra) {
        head = fn_80131234("Records:EXTRA_TIME");
    }
    char best[20];
    RealStatistic stat(IdentGetCurrentSlot(), 0x20507, event, difficulty);
    if (stat.HasValue()) {
        fn_80130980(best, 20, 3, stat.GetValue());
    } else {
        SafeStrCopy(best, fn_80131234("EventPath:NO_BEST_TIME"), 20);
    }
    MenuItem items[] = {
        MenuItem("location", location, NoCaption()),
        MenuItem("best_time", best, NoCaption()),
        MenuItem("time_head", head, NoCaption()),
        MenuItem("#score_1", rows[0].score, NoCaption()),
        MenuItem("#grade_1", rows[0].grade, NoCaption()),
        MenuItem("#time_1", rows[0].time, NoCaption()),
        MenuItem("car_icon_1", 5, &icons[0], 0, 0),
        MenuItem("#score_2", rows[1].score, NoCaption()),
        MenuItem("#grade_2", rows[1].grade, NoCaption()),
        MenuItem("#time_2", rows[1].time, NoCaption()),
        MenuItem("car_icon_2", 5, &icons[1], 0, 0),
        MenuItem("#score_3", rows[2].score, NoCaption()),
        MenuItem("#grade_3", rows[2].grade, NoCaption()),
        MenuItem("#time_3", rows[2].time, NoCaption()),
        MenuItem("car_icon_3", 5, &icons[2], 0, 0),
        MenuItem("#score_4", rows[3].score, NoCaption()),
        MenuItem("#grade_4", rows[3].grade, NoCaption()),
        MenuItem("#time_4", rows[3].time, NoCaption()),
        MenuItem("car_icon_4", 5, &icons[3], 0, 0),
        MenuItem("#score_5", rows[4].score, NoCaption()),
        MenuItem("#grade_5", rows[4].grade, NoCaption()),
        MenuItem("#time_5", rows[4].time, NoCaption()),
        MenuItem("car_icon_5", 5, &icons[4], 0, 0),
        MenuItem(lbl_8055B860, 6, 0, -2, 27),
        MenuItem(lbl_80498EB8),
    };
    MenuDesc desc = {"ep_records.lyt", items, NULL, 0, 0};
    s32 music = fn_802168E4();
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
    return ARGB(0xFF, r, g, b);
}

extern "C" {
void fn_80230C30(f32* x, f32* y, s32 lx, s32 ly);
void fn_80231220(f32 x, f32 y, f32 w, f32 h, u8* tex, u32 color, f32 u0, f32 v0, f32 u1, f32 v1);

// The chosen car icon in a frame, the overlay tinted with the player's colour.
void RecordIcon::Draw(void) {
    if (!visible) {
        return;
    }
    u32 c;
    CarIcon* e;
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
    e = &icons->icons[icon];
    y = sy;
    x = sx;
    u0 = e->u1;
    v0 = e->v1;
    u1 = e->u2;
    v1 = e->v2;
    c = color;
    fn_80231220(x, y, fw, fh, e->tex, -1, u0, v0, u1, v1);
    fn_80231220(x, y, fw, fh, e->overlay, DimColor(c), u0, v0, u1, v1);
}


}  // extern "C"
