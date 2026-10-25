// Loading screen (0x8001FD7C-0x8002194C): the tips camera flythrough from camera3.val, the track
// title bars, a random loading tip, the race goals / records panel and the fade in and out. The
// "tips:" messages give the subject; the names are guesses.
#include <string.h>
#include "game/common.h"

struct Vec {
    Vec(void) {}
    Vec(f32 x_, f32 y_, f32 z_) {
        x = x_;
        y = y_;
        z = z_;
    }
    void Set(f32 x_, f32 y_, f32 z_) {
        x = x_;
        y = y_;
        z = z_;
    }

    f32 x;
    f32 y;
    f32 z;
};

// A camera orientation and position.
extern "C" {
void PSVECCrossProduct(const Vec* a, const Vec* b, Vec* out);
void PSVECNormalize(const Vec* in, Vec* out);
}

struct VecPOD {
    f32 x;
    f32 y;
    f32 z;
};

struct CamFrame {
    VecPOD axes[2];  // offset 0x0: right, up
    Vec fwd;       // offset 0x18
    Vec pos;       // offset 0x24

    void RebuildRight(void) { PSVECCrossProduct((Vec*)&axes[1], &fwd, (Vec*)&axes[0]); }
};

// One leg of the tips camera sequence.
struct TipCam {
    CamFrame start;  // offset 0x0
    CamFrame end;    // offset 0x30
    f32 start_fov;   // offset 0x60
    f32 end_fov;     // offset 0x64
    f32 duration;    // offset 0x68
    u8 hermite;      // offset 0x6C
};

struct RaceGoals {
    f32 time_limit;  // offset 0x0
    s32 stars;       // offset 0x4
    s32 laps;        // offset 0x8
};

struct EventInfo {
    s32 id;           // offset 0x0
    s32 track;        // offset 0x4
    s32 type;         // offset 0x8
    s32 laps;         // offset 0xC
    u8 unk10[0x10];
    ValueNode* node;  // offset 0x20
};

struct Track {
    u8 unk0[0x8];
    char name[0x20];  // offset 0x8
};

struct RecordEntry {
    s32 unk0;
    s32 score;  // offset 0x4
    s32 grade;  // offset 0x8
};

struct DrawContext {
    u8 unk0[0x38];
};

class Grades {
public:
    Grades(ValueNode* table);
    int GetThreshold(int grade);

    s32 thresholds[5];  // offset 0x0
    s32 passing;        // offset 0x14
};

class LocString;

extern "C" {
extern u32 lbl_8055E044;  // localization version
extern s32 lbl_8055CFF0;  // screen width
extern s32 lbl_8055CFF4;  // screen height
int strcmp(const char* a, const char* b);
double cos(double x);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
int fn_801345D8(char* buf, u32 size, const char* fmt, ...);  // snprintf
ValueTable* fn_80131810(const char* name);
void fn_80131818(ValueTable* table);
const char* fn_80131234(const char* key);  // localize
int fn_80131264(const char* key);          // localized key exists
void fn_80131318(LocString* s);
void fn_8012E81C(const char* name);
void* fn_80232578(const char* name);  // load font
void fn_802328D4(void* font);
void* fn_8023512C(const char* name);  // load image
void fn_80235380(void* image);
s32 fn_802CAC28(const char* name, int flags);  // load texture
void fn_802CAC54(s32 tex);
u32 fn_802E2C20(void);  // milliseconds
void fn_80176734(void);
void fn_80232BA4(const char* text, void* font, u32 color, int align, f32 x, f32 y, f32 scale, f32 a, f32 b);
void fn_8023293C(void* font, const char* text, f32 scale);
void fn_80233010(const char* text, void* font, u32 color, int align, int unk, f32 x, f32 y, f32 width,
                 f32 scale, f32 a, f32 b);
void fn_80231220(f32 x, f32 y, f32 w, f32 h, s32 tex, u32 color, f32 u0, f32 v0, f32 u1, f32 v1);
void fn_802354A0(void* image, int unk, u32 color, f32 x, f32 y, f32 z);
void fn_80011280(u32 color, void* image, f32 x, f32 y, f32 width);
void fn_80148134(f32 x, f32 y, f32* out_x, f32* out_y, int mode);
int fn_80277960(const char* name, CamFrame* frame, f32* fov);
void fn_8026E22C(CamFrame* out, const CamFrame* a, const CamFrame* b, f32 t);
void fn_802A1120(f32* center, f32* dir);
f32 fn_801EA0E8(f32 x, f32 z);
void fn_8026A2CC(CamFrame* cam, int unk, f32 fov);
}  // extern "C"
void fn_8000C630(long, int);
extern "C" {
}  // extern "C"
void fn_8000B3DC(unsigned char);
extern "C" {
int fn_801E8838(void);
int fn_802E4A10(void);
int fn_802C9A58(void);
int fn_8020DE1C(void);
void fn_80230B5C(DrawContext* ctx);
void fn_80230AD8(DrawContext* ctx);
void fn_80230B38(void);
void fn_80130FCC(char* buf, int size, s32 value);
void fn_80130980(char* buf, int size, int digits, f32 time);
const char* fn_80205268(const char* fmt, s32 difficulty, s32 event);
int fn_8027DED0(void);
f32 fn_8014E7DC(s32 event, s32 difficulty);
int fn_80129D18(int lo, int hi);
}
Track* GetTrack(int track);
const char* GetEventDisplayName(int event);
EventInfo* GetEvent(int event);

class LocString {
public:
    LocString(const char* key);
    const char* Get(void) {
        if (version != lbl_8055E044) {
            fn_80131318(this);
        }
        return text;
    }

    const char* key;   // offset 0x0
    const char* text;  // offset 0x4
    u32 version;       // offset 0x8
};

int IdentGetCurrentSlot(void);
u32 StatsGetValue(int identity, u32 type, s32 event, int slot, u32 def);
char* StatsFormatValue(u32 type, u32 value, char* buf, int size);
const char* StatsGetLabel(u32 type);
RecordEntry* StatsGetHighScore(int identity, s32 event, s32 key, int index);
int GetNumPlayers(void);
const char* GetGradeDisplayName(int grade);
const char* GetTrackFriendlyName(const char* name);
const char* GetTrackEnviron(const char* name);
BOOL OptDirectToTrack(void);
BOOL OptCarImage(void);
BOOL OptCarIcon(void);

#define MAX_TIP_CAMS 8
#define NO_TEXTURE -2

#define COPY_STRING(dst_, src_, size_)                  \
    {                                                   \
        const char* src = (src_);                       \
        int n = (size_);                                \
        char* dst = (dst_);                             \
        while ((*dst++ = *src++) != '\0') {             \
            if (--n <= 0) {                             \
                dst[-1] = '\0';                         \
                break;                                  \
            }                                           \
        }                                               \
    }


s32 lbl_80558D78 = NO_TEXTURE;  // bar1_top.tex
s32 lbl_80558D7C = NO_TEXTURE;  // bar1_bot.tex
s32 lbl_80558D80 = NO_TEXTURE;  // bar1bktop.tex
s32 lbl_80558D84 = NO_TEXTURE;  // bar1bkbot.tex
const char* lbl_80558D8C = "---";

ModuleTag lbl_8055D620;
UnkStatic7F lbl_8055D628;
TipCam lbl_8035B758[MAX_TIP_CAMS];
RaceGoals lbl_8035BAD8;
s32 lbl_8055D62C;  // 1 while loading, 2 when done
s32 lbl_8055D630;  // tip cameras
s32 lbl_8055D634;  // current tip camera
u32 lbl_8055D638;  // tip camera start time
u32 lbl_8055D63C;  // loading start time
u32 lbl_8055D640;  // skip time
bool lbl_8055D644;  // active
bool lbl_8055D645;  // skipping
u8 lbl_8055D646;    // tutorial
bool lbl_8055D647;  // show tip
bool lbl_8055D648;  // fading out
void* lbl_8055D64C;  // uifnt_o.gtx
void* lbl_8055D650;  // digiface.gtx
void* lbl_8055D654;  // loadback.img
void* lbl_8055D658;  // tips_box.art
const char* lbl_8055D65C;  // tip text
const char* lbl_8055D660;  // title
const char* lbl_8055D664;  // subtitle
s32 lbl_8055D668;  // event
s32 lbl_8055D66C;  // difficulty

void LoadingInit(void) {
    fn_8012E81C("race.res");
    lbl_8055D64C = fn_80232578("uifnt_o.gtx");
    lbl_8055D650 = fn_80232578("digiface.gtx");
    lbl_8055D654 = fn_8023512C("loadback.img");
    lbl_8055D658 = fn_8023512C("tips_box.art");
    lbl_80558D78 = fn_802CAC28("bar1_top.tex", 0);
    lbl_80558D7C = fn_802CAC28("bar1_bot.tex", 0);
    lbl_80558D80 = fn_802CAC28("bar1bktop.tex", 0);
    lbl_80558D84 = fn_802CAC28("bar1bkbot.tex", 0);
}

void LoadingShutdown(void) {
    lbl_8055D62C = 0;
    fn_802CAC54(lbl_80558D84);
    lbl_80558D84 = NO_TEXTURE;
    fn_802CAC54(lbl_80558D80);
    lbl_80558D80 = NO_TEXTURE;
    fn_802CAC54(lbl_80558D78);
    lbl_80558D78 = NO_TEXTURE;
    fn_802CAC54(lbl_80558D7C);
    lbl_80558D7C = NO_TEXTURE;
    fn_802328D4(lbl_8055D64C);
    lbl_8055D64C = NULL;
    fn_802328D4(lbl_8055D650);
    lbl_8055D650 = NULL;
    fn_80235380(lbl_8055D654);
    lbl_8055D654 = NULL;
    fn_80235380(lbl_8055D658);
    lbl_8055D658 = NULL;
}

void LoadingSkip(void) {
    lbl_8055D645 = 1;
    lbl_8055D640 = fn_802E2C20();
    fn_80176734();
}

void LoadingResetTimers(void) {
    lbl_8055D638 = lbl_8055D63C = fn_802E2C20();
    lbl_8055D640 = 0;
    lbl_8055D644 = 1;
    lbl_8055D645 = 0;
}

void LoadingStop(void) {
    lbl_8055D644 = 0;
    lbl_8055D645 = 0;
}

void DrawLoadingText(u32 color) {
    static LocString s_loading("Tips:LOADING");

    fn_80232BA4(s_loading.Get(), lbl_8055D64C, color, 0x12, 230.0f, -194.0f, 0.73333335f, 0.0f, 1.0f);
}

void GetTipKey(int idx, s32 type, char* buf, u32 size) {
    switch (type) {
    case 0x180303:
        fn_801345D8(buf, size, "TipsRing:T%d_DESC", idx + 1);
        break;
    case 0x180201:
        fn_801345D8(buf, size, "TipsZone:T%d_DESC", idx + 1);
        break;
    case 0x180304:
        fn_801345D8(buf, size, "TipsCrush:T%d_DESC", idx + 1);
        break;
    default:
        fn_801345D8(buf, size, "Tips:T%d_DESC", idx + 1);
        break;
    }
}

void OrthonormalizeFrame(CamFrame* f) {
    Vec up(0.0f, 1.0f, 0.0f);
    Vec fwd;
    Vec newUp;
    Vec right;

    PSVECCrossProduct(&up, &f->fwd, (Vec*)&f->axes[0]);
    PSVECCrossProduct(&f->fwd, (Vec*)&f->axes[0], (Vec*)&f->axes[1]);
    right.Set(f->axes[0].x, f->axes[0].y, f->axes[0].z);
    fwd.Set(f->fwd.x, f->fwd.y, f->fwd.z);
    PSVECCrossProduct(&fwd, &right, &newUp);
    PSVECNormalize(&right, &right);
    PSVECNormalize(&newUp, &newUp);
    PSVECCrossProduct(&right, &newUp, &fwd);
    f->axes[0] = *(VecPOD*)&right;
    f->axes[1] = *(VecPOD*)&newUp;
    f->fwd = fwd;
}

void LoadingExit(void) {
    fn_8000B3DC(0);
}

int GetLoadingFade(void) {
    s32 t = ((s32)(fn_802E2C20() - lbl_8055D63C) - 300) * 256 / 1000;

    t = MAX(t, 0);
    return 255 - MIN(t, 255);
}

void LoadTipsSequence(void) {
    ValueTable* table;
    ValueNode* seq;
    ValueNode* node;
    int i;

    LoadingResetTimers();
    lbl_8055D630 = 0;
    lbl_8055D634 = 0;
    table = fn_80131810("camera3.val");
    if (table != NULL) {
        if (table->HasChild("tips_sequence")) {
            seq = table->FindChild("tips_sequence");
            fn_802E30C8("tips: got tips sequence");
            for (i = 0; (node = seq->GetChildAt(i)) != NULL; i++) {
                if (strcmp(node->GetName(), "camera") == 0) {
                    const char* cam1 = node->GetValue("start_camera");
                    const char* cam2 = node->GetValue("end_camera");
                    TipCam* tc = &lbl_8035B758[lbl_8055D630++];

                    if (!fn_80277960(cam1, &tc->start, &tc->start_fov)) {
                        fn_802E317C("tips: can't find camera1 %s", cam1);
                    }
                    if (!fn_80277960(cam2, &tc->end, &tc->end_fov)) {
                        fn_802E317C("tips: can't find camera2 %s", cam1);
                    }
                    tc->duration = node->GetFloat("duration");
                    tc->hermite = node->GetBool("hermite", 0);
                }
            }
        } else {
            fn_802E30C8("tips: camera3 does not contain tips sequence");
        }
        fn_80131818(table);
    }
    lbl_8055D648 = 0;
    lbl_8055D644 = !(OptCarImage() || OptCarIcon() || OptDirectToTrack());
    if (lbl_8055D646) {
        lbl_8055D644 = 0;
        lbl_8055D648 = 1;
    }
}

void LoadingStop2(void) {
    LoadingStop();
}

static inline f32 SCurve(f32 t) {
    if (t < -1.0f) {
        return -1.0f;
    }
    if (t > 1.0f) {
        return 1.0f;
    }
    if (t > 0.0f) {
        return t * (-2.0f * t * t) + 3.0f * t * t;
    }
    return t * (-2.0f * t * t) - 3.0f * t * t;
}

static inline f32 LinStep(f32 x, f32 x0, f32 x1, f32 y0, f32 y1) {
    f32 t = (x - x0) / (x1 - x0);
    t = MIN(1.0f, t);
    t = MAX(t, 0.0f);
    return y0 + t * (y1 - y0);
}

void GetTipsCamera(CamFrame* out, f32* out_fov) {
    CamFrame cam;
    f32 center[2];
    f32 dir[2];
    f32 k;
    f32 fov;
    s32 t;

    cam.axes[0].x = 1.0f;
    cam.axes[0].y = 0.0f;
    cam.axes[0].z = 0.0f;
    cam.axes[1].x = 0.0f;
    cam.axes[1].y = 1.0f;
    cam.axes[1].z = 0.0f;
    cam.fwd.x = 0.0f;
    cam.fwd.y = 0.0f;
    cam.fwd.z = 1.0f;
    cam.pos.x = 0.0f;
    cam.pos.y = 0.0f;
    cam.pos.z = 0.0f;
    fn_802A1120(center, dir);
    cam.pos.x = center[0];
    cam.pos.y = 0.0f;
    cam.pos.z = center[1];
    cam.fwd.x = dir[0];
    cam.fwd.y = 0.0f;
    cam.fwd.z = dir[1];
    cam.RebuildRight();
    t = fn_802E2C20() - lbl_8055D63C;
    k = 5.0f + 20.0f * (1.0f + -(f32)cos(6.2831855f * ((t % 50000) / 50000.0f)));
    cam.pos.x += cam.fwd.x * k;
    cam.pos.y += cam.fwd.y * k;
    cam.pos.z += cam.fwd.z * k;
    cam.pos.y = 10.0f + fn_801EA0E8(cam.pos.x, cam.pos.z);
    fov = 1.5707964f;
    if (lbl_8055D630 > 0) {
        TipCam* tc = &lbl_8035B758[lbl_8055D634];
        f32 s = 0.001f * (s32)(fn_802E2C20() - lbl_8055D638);

        if (s > tc->duration) {
            lbl_8055D634 = (lbl_8055D634 + 1) % lbl_8055D630;
            lbl_8055D638 = fn_802E2C20();
        }
        s = LinStep(s, 0.0f, tc->duration, 0.0f, 1.0f);
        if (tc->hermite) {
            s = SCurve(s);
        }
        fn_8026E22C(&cam, &tc->start, &tc->end, s);
        OrthonormalizeFrame(&cam);
        fov = tc->start_fov + s * (tc->end_fov - tc->start_fov);
    }
    *out = cam;
    *out_fov = fov;
}

void LoadingUpdate(void) {
    if (lbl_8055D644 && !lbl_8055D645) {
        fn_802E2C20();
    }
    if (lbl_8055D644 && lbl_8055D645) {
        if (fn_801E8838() > 0xF0 || (s32)(fn_802E2C20() - lbl_8055D640) > 1200) {
            lbl_8055D645 = 0;
            lbl_8055D644 = 0;
        }
    }
    if (lbl_8055D644) {
        CamFrame cam;
        f32 fov;

        GetTipsCamera(&cam, &fov);
        fn_8026A2CC(&cam, 0, fov);
        fn_8000C630(0x15, -1);
    }
}

void DrawLoadingBackground(int alpha) {
    f32 width = (1.7777778f * lbl_8055CFF0) / 1.3333334f;
    f32 height = lbl_8055CFF4;

    fn_802354A0(lbl_8055D654, 0, (alpha << 24) | 0xFFFFFF, -0.5f * width, 0.5f * height, 0.0f);
}


void GetRecordString(u32 kind, char* buf, int size) {
    u32 value = StatsGetValue(IdentGetCurrentSlot(), kind, lbl_8055D668, lbl_8055D66C, -1);

    if (value != 0xFFFFFFFF) {
        StatsFormatValue(kind, value, buf, size);
    } else {
        COPY_STRING(buf, lbl_80558D8C, size);
    }
}

static inline f32 ScreenLeft(void) {
    return -0.5f * lbl_8055CFF0;
}

static inline f32 ScreenTop(void) {
    return 0.5f * lbl_8055CFF4;
}

void DrawLoadingScreen(int alpha, f32 t) {
    f32 offset;
    f32 x0;
    f32 x1;
    f32 y0;
    f32 y1;
    f32 y2;
    u32 color;
    u32 bar_colors[3];
    u32 bar_color;
    f32 title_x;
    f32 title_y;
    f32 sub_x;
    f32 sub_y;
    BOOL show_goals;

    offset = fn_802C9A58() ? -107.0f : 0.0f;
    x0 = -0.5f * lbl_8055CFF0 + offset;
    y0 = 0.5f * lbl_8055CFF4;
    y1 = y0 - 256.0f;
    x1 = 256.0f + x0;
    y2 = y1 - 256.0f;
    color = (alpha << 24) | 0xFFFFFF;
    bar_colors[0] = 0xFF259327;
    bar_colors[1] = 0xFFB71B00;
    bar_colors[2] = 0xFF003FA8;
    bar_color = bar_colors[lbl_8055D66C];
    if (lbl_8055D646) {
        bar_color = 0;
    }
    fn_80231220(x0, y0, x1 - x0, y0 - y1, lbl_80558D80, bar_color, 0.0f, 0.0f, 1.0f, 1.0f);
    fn_80231220(x0, y1, x1 - x0, y1 - y2, lbl_80558D84, bar_color, 0.0f, 0.0f, 1.0f, 1.0f);
    fn_80231220(x0, y0, x1 - x0, y0 - y1, lbl_80558D78, color, 0.0f, 0.0f, 1.0f, 1.0f);
    fn_80231220(x0, y1, x1 - x0, y1 - y2, lbl_80558D7C, color, 0.0f, 0.0f, 1.0f, 1.0f);
    fn_80148134(65.0f, 40.0f, &title_x, &title_y, 2);
    fn_80148134(65.0f, 84.0f, &sub_x, &sub_y, 2);
    fn_80232BA4(lbl_8055D660, lbl_8055D64C, color, 9, title_x, title_y, 1.1733334f, 0.0f, 1.0f);
    fn_80232BA4(lbl_8055D664, lbl_8055D64C, color, 9, sub_x, sub_y, 0.88f, 0.0f, 1.0f);
    show_goals = !lbl_8055D647;
    if (!lbl_8055D646 && lbl_8055D647) {
        f32 slide = 4.0f * SCurve(t);
        f32 box_h = 140.0f;
        f32 box_y = 0.5f * box_h - 80.0f;
        f32 box_x = lbl_8055CFF0 * slide - 225.0f;

        fn_80011280((((alpha * 192) / 255) << 24) | 0xFFFFFF, lbl_8055D658, box_x, box_y, 450.0f);
        static LocString s_title("Tips:TITLE");
        fn_8023293C(lbl_8055D64C, s_title.Get(), 1.0f);
        fn_80233010(lbl_8055D65C, lbl_8055D64C, -1, 0x11, 0, 20.0f + box_x, box_y - 50.0f, 410.0f, 0.51333332f,
                    0.0f, 1.0f);
    }
    if (!lbl_8055D646 && show_goals && GetNumPlayers() == 1) {
        f32 slide = 4.0f * SCurve(t);
        f32 panel_x = lbl_8055CFF0 * slide - 160.0f;
        int i;

        fn_80011280((((alpha * 192) / 255) << 24) | 0xFFFFFF, lbl_8055D658, panel_x, 40.0f, 320.0f);
        int rows[3] = {10, 8, 9};
        for (i = 0; i < sizeof(rows) / sizeof(rows[0]); i++) {
            int kind = rows[i];
            const char* label = "";
            char value[0x40] = {0};
            f32 y;

            switch (kind) {
            case 4:
                label = StatsGetLabel(0x22205);
                GetRecordString(0x22205, value, sizeof(value));
                break;
            case 0:
                label = StatsGetLabel(0x40901);
                GetRecordString(0x40901, value, sizeof(value));
                break;
            case 1:
                label = StatsGetLabel(0x40902);
                GetRecordString(0x40902, value, sizeof(value));
                break;
            case 2:
                label = StatsGetLabel(0x41206);
                GetRecordString(0x41206, value, sizeof(value));
                break;
            case 5:
                label = StatsGetLabel(0x20507);
                GetRecordString(0x20507, value, sizeof(value));
                break;
            case 3:
                value[0] = '\0';
                label = "";
                break;
            case 9:
                label = fn_80131234("Loading:LAPS");
                fn_80130FCC(value, sizeof(value), lbl_8035BAD8.laps);
                break;
            case 10:
                if (GetNumPlayers() == 1) {
                    label = fn_80131234("Loading:STARS");
                    fn_80130FCC(value, sizeof(value), lbl_8035BAD8.stars);
                } else {
                    value[0] = '\0';
                    label = "";
                }
                break;
            case 8:
                if (GetNumPlayers() == 1) {
                    label = fn_80131234("Loading:TIME_LIMIT");
                    fn_80130980(value, sizeof(value), 2, lbl_8035BAD8.time_limit);
                } else {
                    value[0] = '\0';
                    label = "";
                }
                break;
            case 6: {
                RecordEntry* rec = StatsGetHighScore(IdentGetCurrentSlot(), lbl_8055D668, lbl_8055D66C, 0);
                if (rec != NULL && rec->grade >= 0) {
                    COPY_STRING(value, GetGradeDisplayName(rec->grade), sizeof(value));
                } else {
                    COPY_STRING(value, lbl_80558D8C, sizeof(value));
                }
                label = fn_80131234("Records:LOAD_GRADE");
                break;
            }
            case 7: {
                RecordEntry* rec = StatsGetHighScore(IdentGetCurrentSlot(), lbl_8055D668, lbl_8055D66C, 0);
                if (rec != NULL && rec->score >= 0) {
                    fn_80130FCC(value, sizeof(value), rec->score);
                } else {
                    COPY_STRING(value, lbl_80558D8C, sizeof(value));
                }
                label = fn_80131234("Records:LOAD_SCORE");
                break;
            }
            }
            y = -5.0f - 36.0f * i;
            fn_80232BA4(label, lbl_8055D64C, color, 0x11, 20.0f + panel_x, y, 0.6673333f, 0.0f, 1.0f);
            if (kind != 8) {
                fn_80232BA4(value, lbl_8055D64C, color, 0x12, 320.0f + panel_x - 20.0f, y, 0.6673333f, 0.0f,
                            1.0f);
            } else {
                fn_80232BA4(value, lbl_8055D650, color, 0x12, 5.0f + (320.0f + panel_x - 20.0f), 1.0f + y,
                            0.9009f, 0.0f, 1.0f);
            }
        }
    }
}

int LoadingDraw(void) {
    int fade;
    f32 t;

    if (!lbl_8055D644) {
        return 0;
    }
    fade = GetLoadingFade();
    if (fade > 0) {
        DrawLoadingBackground(fade);
    }
    if (fade > 0) {
        t = 0.0f;
    } else {
        t = fn_801E8838() / 255.0f;
    }
    DrawLoadingScreen(0xFF, t);
    if (!fn_8020DE1C()) {
        static LocString s_continue("ResultsOverlay:PRESS_TO_CONTINUE");
        static LocString s_options("ResultsOverlay:PRESS_FOR_OPTIONS");

        fn_80232BA4(s_options.Get(), lbl_8055D64C, -1, 0x14, -107.0f, -190.0f, 0.73333335f, 0.0f, 1.0f);
        fn_80232BA4(s_continue.Get(), lbl_8055D64C, -1, 0x14, 107.0f, -190.0f, 0.73333335f, 0.0f, 1.0f);
    }
    return 1;
}

int LoadingHandleKey(int key) {
    if (lbl_8055D644) {
        if (!lbl_8055D645 && !fn_801E8838() && !fn_802E4A10()) {
            switch (key) {
            case 0x58:
                LoadingSkip();
                break;
            case 0x1B:
                LoadingExit();
                break;
            }
        }
        return 1;
    }
    return 0;
}

s32 lbl_80558DA4 = -1;  // last tip shown

void LoadingBegin(EventInfo* info, s32 difficulty) {
    static char s_key[0x40];
    s32 type;
    EventInfo* ev;
    int count;
    int tries;
    int tip;

    lbl_8055D62C = 1;
    LoadingResetTimers();
    lbl_8055D660 = GetTrackFriendlyName(GetTrackEnviron(GetTrack(info->track)->name));
    lbl_8055D664 = info->node->GetString("friendly_name", GetEventDisplayName(info->id));
    lbl_8055D646 = info->node->Has("tutorial");
    if (lbl_8055D646) {
        lbl_8055D660 = fn_80131234("TIPS:TutorialTitle");
    }
    type = info->type;
    lbl_8055D668 = info->id;
    lbl_8055D66C = difficulty;
    lbl_8055D647 = type != 0x180302;
    ev = GetEvent(lbl_8055D668);
    GetTrack(ev->track);
    lbl_8035BAD8.laps = ev->laps;
    {
        ValueNode* table = ev->node->FindChild(fn_80205268("grade_table", lbl_8055D66C, lbl_8055D668));
        if (table != NULL) {
            Grades grades(table);
            int grade = fn_8027DED0();
            if (grade == -1) {
                grade = grades.passing;
            }
            lbl_8035BAD8.stars = grades.GetThreshold(grade);
        }
    }
    lbl_8035BAD8.time_limit = fn_8014E7DC(lbl_8055D668, difficulty);
    for (count = 0;; count++) {
        GetTipKey(count, type, s_key, sizeof(s_key));
        if (!fn_80131264(s_key)) {
            break;
        }
    }
    if (count == 0) {
        count = 1;
    }
    for (tries = 0; tries < 10; tries++) {
        tip = fn_80129D18(1, count);
        if (tip != lbl_80558DA4) {
            break;
        }
    }
    lbl_80558DA4 = tip;
    GetTipKey(tip, type, s_key, sizeof(s_key));
    lbl_8055D65C = fn_80131234(s_key);
}

void LoadingStop3(void) {
    LoadingStop();
}

void DrawLoadingFull(void) {
    DrawLoadingBackground(0xFF);
    DrawLoadingScreen(0xFF, 0.0f);
    DrawLoadingText(-1);
}

void LoadingFinish(void) {
    lbl_8055D62C = 2;
}

void LoadingNop(void) {}

void DrawLoadingTextOnly(void) {
    DrawLoadingText(-1);
}

BOOL IsLoadingActive(void) {
    return lbl_8055D644 && !lbl_8055D645;
}

BOOL IsLoading(void) {
    return lbl_8055D644;
}

BOOL IsLoadingFading(void) {
    if (lbl_8055D644) {
        return GetLoadingFade() > 0;
    }
    return 0;
}

BOOL LoadingFrame(void) {
    if (lbl_8055D62C != 0) {
        DrawContext ctx;

        fn_80230B5C(&ctx);
        fn_80230AD8(&ctx);
        switch (lbl_8055D62C) {
        case 2:
            DrawLoadingTextOnly();
            break;
        case 1:
            DrawLoadingFull();
            break;
        }
        fn_80230B38();
        return 1;
    }
    return 0;
}

BOOL IsLoadingDone(void) {
    if (lbl_8055D648) {
        return 0;
    }
    return !(lbl_8055D644 && !lbl_8055D645);
}

static inline f32 Lerp(f32 a, f32 b, f32 t) {
    return a + t * (b - a);
}

void LoadingFadeOut(void) {
    if (lbl_8055D648) {
        s32 dt = fn_802E2C20() - lbl_8055D63C;

        if (dt < 1500) {
            f32 s = MIN(1.0f, (dt - 500.0f) / 1000.0f);
            f32 a;
            int alpha;

            s = MAX(s, 0.0f);
            a = Lerp(255.0f, 0.0f, s);
            alpha = a < 0.0f ? a - 0.5f : 0.5f + a;

            if (alpha > 0) {
                DrawLoadingBackground(alpha);
                DrawLoadingScreen(alpha, 0.0f);
            }
        } else {
            lbl_8055D648 = 0;
        }
    }
}

void LoadingInitHook(void) {
    LoadingInit();
}

void LoadingShutdownHook(void) {
    LoadingShutdown();
}
