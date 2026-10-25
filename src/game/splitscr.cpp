// Split-screen overlay (0x802057A8-0x802062E8): with more than one player view, a divider down the
// middle of the screen, a progress bar per player beside it (filled by race distance, with a car
// icon), and the START/FINISH labels at the ends of the bars. Names are guesses apart from the
// keys and the texture name.
#include "game/common.h"

// A vertex position (constructed in place).
struct VtxPos {
    VtxPos(f32 x_, f32 y_, f32 z_) {
        x = x_;
        y = y_;
        z = z_;
    }
    f32 x;
    f32 y;
    f32 z;
};

struct VecP {
    f32 x;
    f32 y;
    f32 z;
};

struct Rot {
    VecP right;  // offset 0x0
    VecP up;     // offset 0xC
    VecP fwd;    // offset 0x18
};

struct Camera {
    Rot rot;   // offset 0x0
    VecP pos;  // offset 0x24
};

class LocString;
extern "C" {
extern u32 lbl_8055E044;  // localization version
void fn_80131318(LocString* s);
}

// A localized string looked up by key and refreshed when the language changes.
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

struct RaceSetupHdr {
    u8 pad0[8];
    s32 laps;  // offset 0x8
};

struct CarBody {
    u8 pad0[0x2C8];
    f32 dist;  // offset 0x2C8: distance along this lap
};

struct CarProgress {
    u8 pad0[8];
    CarBody* body;  // offset 0x8
    u8 padC[0x10];
    s32 lap;        // offset 0x1C
};

extern "C" {
extern s32 lbl_8055CFF0;  // screen width
extern s32 lbl_8055CFF4;  // screen height
void fn_802C8CBC(int tex);
int fn_802C8DBC(int n);
void fn_802C8E00(u32 color, int idx, f32 x, f32 y, f32 z, f32 u, f32 v);
void fn_802C8E48(void);
void fn_802C8D74(void);
Camera* fn_802C86DC(void);
void fn_802C88FC(Camera* m);
void fn_802C8638(Camera* cam);
void fn_80010188(u32 color, int flags, const char* fmt, f32 scale, f32 x, f32 y, ...);
s32 fn_802CAC28(const char* name, int flags);  // load texture
void fn_802CAC54(s32 tex);
int fn_802C2C00(int on);
int fn_801E85A4(void);       // player views
int fn_801E866C(int view);  // car of a view
int fn_801B8570(int car);   // car is racing
CarProgress* fn_801EA9A0(int car);
f32 fn_8013672C(void);      // lap length
RaceSetupHdr* fn_801E7E3C(void);
}

static inline f32 Min(f32 a, f32 b) { return (b < a) ? b : a; }
static inline f32 Max(f32 a, f32 b) { return (a > b) ? a : b; }
static inline f32 Clamp01(f32 x) { return Max(Min(x, 1.0f), 0.0f); }
static inline f32 Ramp(f32 x, f32 lo, f32 hi) { return Clamp01((x - lo) / (hi - lo)); }
static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }
static inline f32 Unit(f32 x, f32 lo, f32 hi) { return (x - lo) / (hi - lo); }

ModuleTag lbl_8055ED60;
UnkStaticFFFF lbl_8055ED68;
s32 lbl_8055ED6C;  // car icon texture

extern "C" {

// One vertex at (x, y) in bar space: x in [-1, 1] across the divider, y in [0, 1] down the screen.
void fn_802057A8(u32 color, int idx, f32 x, f32 y, f32 u, f32 v) {
    f32 px = (f32)lbl_8055CFF0 * Lerp(0.49f, 0.51f, Unit(x, -1.0f, 1.0f));
    f32 py = (f32)lbl_8055CFF4 * Lerp(0.93f, 0.07f, y);
    VtxPos p(px, py, 0.0f);
    fn_802C8E00(color, idx, p.x, p.y, p.z, u, v);
}

void fn_80205850(void) {
    fn_802C8CBC(-1);
    fn_802C8DBC(0x20);
    fn_802057A8(-1, 0, -1.0f, 0.0f, 0.0f, 0.0f);
    fn_802057A8(-1, 0, 1.0f, 0.0f, 0.0f, 0.0f);
    fn_802057A8(-1, 1, -1.0f, 0.005f, 0.0f, 0.0f);
    fn_802057A8(-1, 1, 1.0f, 0.005f, 0.0f, 0.0f);
    fn_802057A8(-1, 0, -1.0f, 0.995f, 0.0f, 0.0f);
    fn_802057A8(-1, 0, 1.0f, 0.995f, 0.0f, 0.0f);
    fn_802057A8(-1, 1, -1.0f, 1.0f, 0.0f, 0.0f);
    fn_802057A8(-1, 1, 1.0f, 1.0f, 0.0f, 0.0f);
    fn_802C8E48();
    fn_802C8D74();
}

int fn_80205964(void) {
    for (int i = 0; i < fn_801E85A4(); i++) {
        if (fn_801B8570(fn_801E866C(i))) {
            return 1;
        }
    }
    return 0;
}

void fn_802059C0(void) {
    f32 lapLen = fn_8013672C();
    f32 total = lapLen * (f32)fn_801E7E3C()->laps;
    int n = fn_801E85A4();
    for (int i = 0; i < n; i++) {
        int car = fn_801E866C(i);
        if (fn_801B8570(car)) {
            CarProgress* cp = fn_801EA9A0(car);
            int lap = cp->lap;
            f32 dist = cp->body->dist;
            if (lap == 0) {
                lap = 1;
                dist = 0.0f;
            }
            f32 p = (dist + (f32)(lap - 1) * lapLen) / total;
            p = Min(p, 1.0f);
            int a = (int)Lerp(0.0f, 255.0f, Ramp(p, 0.0f, 0.02f));
            u32 color = (a << 24) | ((i == 0 ? 0xFFFF0000 : 0xFF0000FF) & 0xFFFFFF);
            u32 dark = color & 0xFF7F7F7F;
            f32 side = (i == 0) ? -1.0f : 1.0f;
            f32 iconScale = 3.0f;
            f32 iconX = side * iconScale;
            f32 margin = 2.0f * (0.01f * iconScale);
            f32 y = Lerp(margin, 1.0f, Ramp(p, 0.0f, 1.0f));
            if ((u8)a) {
                fn_802C8CBC(-1);
                fn_802C8DBC(6);
                fn_802057A8(color & 0xFFFFFF, 0, side, y, 0.0f, 0.0f);
                fn_802057A8(dark & 0xFFFFFF, 0, 0.0f, y, 0.0f, 0.0f);
                f32 y2 = Max(0.0f, y - 0.05f);
                fn_802057A8(color, 1, side, y2, 0.0f, 0.0f);
                fn_802057A8(dark, 1, 0.0f, y2, 0.0f, 0.0f);
                y2 = Max(0.0f, y2 - Lerp(0.07f, 0.25f, Ramp(y2, 0.1f, 0.5f)));
                fn_802057A8(color & 0xFFFFFF, 1, side, y2, 0.0f, 0.0f);
                fn_802057A8(dark & 0xFFFFFF, 1, 0.0f, y2, 0.0f, 0.0f);
                fn_802C8E48();
                fn_802C8D74();
            }
            if (lbl_8055ED6C != -2) {
                fn_802C8CBC(lbl_8055ED6C);
                fn_802C8DBC(4);
                fn_802057A8(-1, 0, iconX, y, 0.0f, 1.0f);
                fn_802057A8(-1, 0, 0.0f, y, 1.0f, 1.0f);
                y -= margin;
                fn_802057A8(-1, 1, iconX, y, 0.0f, 0.0f);
                fn_802057A8(-1, 1, 0.0f, y, 0.0f, 1.0f);
                fn_802C8E48();
                fn_802C8D74();
            }
        }
    }
}

static inline void DividerVertex(u32 color, int idx, f32 x, f32 y) {
    VtxPos p(x, y, 0.0f);
    fn_802C8E00(color, idx, p.x, p.y, p.z, 0.0f, 0.0f);
}

void fn_80205D5C(void) {
    fn_802C8CBC(-1);
    fn_802C8DBC(0x10);
    f32 ys[4] = {0.0f, 0.08f, 0.92f, 1.0f};
    f32 w = 0.5f * (0.01f * (f32)lbl_8055CFF0);
    f32 ws[4] = {1.5f * w, w, w, 1.5f * w};
    f32 cx = (f32)lbl_8055CFF0 * 0.5f;
    for (int i = 0; i < 4; i++) {
        DividerVertex(0x00404040, i > 0, cx + ws[i], (f32)lbl_8055CFF4 * ys[i]);
        DividerVertex(0xFF404040, i > 0, cx, (f32)lbl_8055CFF4 * ys[i]);
    }
    for (int i = 0; i < 4; i++) {
        DividerVertex(0x00404040, i > 0, cx - ws[i], (f32)lbl_8055CFF4 * ys[i]);
        DividerVertex(0xFF404040, i > 0, cx, (f32)lbl_8055CFF4 * ys[i]);
    }
    fn_802C8E48();
    fn_802C8D74();
}

void fn_80205FCC(void) {
    Camera saved = *fn_802C86DC();
    Camera m;
    fn_802C88FC(&m);
    m.rot.up.x *= -1.0f;
    m.rot.up.y *= -1.0f;
    m.rot.up.z *= -1.0f;
    fn_802C8638(&m);
    static LocString finish("SplitScreen:FINISH");
    static LocString start("SplitScreen:START");
    fn_80010188(-1, 0xC, start.Get(), 0.75f, 0.5f * (f32)lbl_8055CFF0, 3.0f + 0.07f * (f32)lbl_8055CFF4);
    f32 y = 0.93f * (f32)lbl_8055CFF4 + 4.0f;
    fn_80010188(-1, 0x24, finish.Get(), 0.75f, 0.5f * (f32)lbl_8055CFF0, y);
    fn_802C8638(&saved);
}

void fn_802061F8(void) { lbl_8055ED6C = fn_802CAC28("mpov_car.tex", 0); }

void fn_80206228(void) {
    if (lbl_8055ED6C != -2) {
        fn_802CAC54(lbl_8055ED6C);
        lbl_8055ED6C = -2;
    }
}

void fn_8020625C(void) {
    if (fn_801E85A4() > 1) {
        int old = fn_802C2C00(0);
        fn_80205D5C();
        if (fn_80205964()) {
            fn_80205850();
            fn_80205FCC();
        }
        fn_802059C0();
        fn_802C2C00((u8)old);
    }
}

}  // extern "C"
