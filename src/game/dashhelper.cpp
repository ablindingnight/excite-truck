// Dash helper (0x8000DFA8-0x800123EC, "dashhelper" in its warning): the race HUD toolkit - the left,
// right and centre text-row lists, the 3-2-1-GO countdown, text/rect/line/nine-slice drawing helpers,
// the per-view screen layout and the menu sound player. Names are guesses.
#include <stdarg.h>
#include <math.h>
#include "game/common.h"

void* operator new(size_t size, const char* tag);

struct LayoutItem {
    u32 unk0;  // offset 0x0
    struct {
        u32 a;
        u32 b;
    } unk4;     // offset 0x4
    u32 unkC;   // offset 0xC
    f32 scale;  // offset 0x10
    u32 unk14;  // offset 0x14
    u32 color;  // offset 0x18
    u32 unk1C;  // offset 0x1C
};

class Layout {
public:
    virtual ~Layout(void);
    virtual LayoutItem* Find(const char* name);  // 0xC
};

class Sound {
public:
    virtual ~Sound(void);

    void Play(void) {
        play = 1;
        stop = 0;
        paused = 0;
    }
    void SetVolume(f32 v) {
        if (v != volume) {
            dirty = 1;
            volume = v;
        }
    }

    u8 pad4[0x8 - 0x4];
    f32 volume;  // offset 0x8
    u8 padC[0x49 - 0xC];
    u8 play;    // offset 0x49
    u8 paused;  // offset 0x4A
    u8 stop;    // offset 0x4B
    u8 pad4C;
    u8 dirty;  // offset 0x4D
};

struct Viewport {
    u8 data[0x10];
};

struct RaceProgress {
    u8 pad0[0x10];
    f32 elapsed;  // offset 0x10
};

// One row of a text-row list (0x40 bytes).
struct DashRow {
    char text[0x2C];  // offset 0x0
    s32 a;            // offset 0x2C
    s32 b;            // offset 0x30
    s32 c;            // offset 0x34
    s32 type;         // offset 0x38
    f32 slide;        // offset 0x3C
};

struct DashRowList {
    int art;            // offset 0x0
    u8 wide;            // offset 0x4
    DashRow rows[10];   // offset 0x8
    s32 count;          // offset 0x288
};

struct DashRowType {
    s32 id;
    s32 part;
    s32 part2;
};

struct QueuedRect {
    f32 x;
    f32 y;
    f32 w;
    f32 h;
    u32 color0;
    u32 color1;
};

struct ScreenBox {
    f32 top;     // offset 0x0
    f32 left;    // offset 0x4
    f32 right;   // offset 0x8
    f32 bottom;  // offset 0xC
    f32 width;   // offset 0x10
};

struct ViewLayout {
    f32 top;      // offset 0x0
    f32 left;     // offset 0x4
    f32 right;    // offset 0x8
    f32 bottom;   // offset 0xC
    f32 centerX;  // offset 0x10
    f32 centerY;  // offset 0x14
};

struct Vertex {
    f32 data[6];
};

extern "C" {
int sprintf(char* buf, const char* fmt, ...);
int vsprintf(char* buf, const char* fmt, va_list args);
char* strncpy(char* dst, const char* src, size_t n);
char* strcpy(char* dst, const char* src);
size_t strlen(const char* s);
void* memset(void* p, int c, size_t n);
double fmod(double x, double y);
double ceil(double x);
double sin(double x);
double cos(double x);
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
void fn_802E30C8(const char* fmt, ...);  // debug print
int fn_801345D8(char* buf, u32 size, const char* fmt, ...);
f32 fn_80235EF0(int art, int part);
f32 fn_80235F64(int art, int part);
int fn_80235FD8(int art);
int fn_8023512C(const char* name);
void fn_80235380(int art);
void fn_802353F4(int art, int part, f32 x, f32 y, f32 z);
void fn_802354A0(int art, int part, u32 color, f32 x, f32 y, f32 z, f32 w, f32 h);
f32 fn_801E7E18(void);
RaceProgress* fn_801E7E3C(void);
int fn_801E85A4(void);
int fn_8023441C(void);
void fn_80234470(void);
int fn_80232B70(int font);
int fn_80232578(const char* name);
void fn_802328D4(int font);
f32 fn_8023293C(int font, const char* text, f32 scale);
void fn_802344CC(const char* text, u32 color, int flags, f32 x, f32 y, f32 scale, int unk);
void fn_80232BA4(const char* text, int font, u32 color, int flags, f32 x, f32 y, f32 scale, f32 z,
                 f32 alpha);
void fn_80233010(const char* text, int font, u32 color, int flags, f32 x, f32 y, f32 w, f32 scale,
                 f32 z, f32 alpha, int wrap);
void fn_802337A0(int font, u32 color, int flags, const char* fmt, f32 x, f32 y, f32 z, f32 scale,
                 f32 alpha, ...);
int fn_802C6FC0(const char* name, int a, int b);
void fn_802C6CF8(int model);
Sound* fn_80207848(const char* name, int flags);
Layout* fn_80145E70(const char* name);
void fn_802C86F0(Viewport* vp, int x, int y, int w, int h);
void fn_802C8750(Viewport* vp, int unk);
const char* fn_80131268(const char* key);
void fn_80232928(u32 color);
void fn_80232930(void);
void fn_80147ABC(LayoutItem* a, LayoutItem* b, const char* fmt, f32 t, ...);
int fn_80137C74(void);
void fn_80231380(Vertex* v, u32 color, f32 x, f32 y, f32 z, f32 u, f32 w);
void fn_802C8DB8(Vertex* v, int nv, const u16* idx, int ni);
void fn_802C8BD0(s32 tex);
void fn_802C8D74(void);
int fn_8022EDC4(void);
void fn_802C8B98(s32 tex, int unk);
void fn_801E4F20(int n);
void fn_801E4F3C(u32 color, int back, f32 x, f32 y, f32 z, f32 u, f32 v);
void fn_801E4FF4(void);
void fn_802346C8(void);
void fn_80234AEC(void);
void fn_80231094(int* top, int* bottom, int* left, int* right);
void fn_801331C4(int type, const char* name, f32* out);
void fn_801310B4(char* buf, int size, s32 value);
const char* fn_80130FE0(s32 value);

f32 fn_80011E54(void);
f32 fn_80011DC0(int player);
f32 fn_80011EC8(int player);
f32 fn_80011E34(void);
f32 fn_80011DD4(int player);
f32 fn_80011DEC(int player);
f32 fn_80011E04(int player);
f32 fn_80011E1C(int player);
f32 fn_80011E70(void);
f32 fn_80011E90(void);
f32 fn_80011E60(void);
f32 fn_80011EDC(void);
void fn_80010074(u32 color, int flags, const char* fmt, f32 scale, f32 x, f32 y, ...);
void fn_8000FF88(u32 color, int flags, const char* fmt, f32 scale, f32 x, f32 y, ...);
f32 fn_800108C8(const char* text, f32 scale);
void fn_800109E4(u32 color, f32 x, f32 y, f32 w, f32 h);
void fn_80010A50(f32 x, f32 y, f32 w, f32 h, u32 color0, u32 color1);
void fn_80010B74(void);
void fn_80010B7C(void);
void fn_80010B80(f32 x, f32 y, f32 w, f32 h, u32 color);
void fn_80010C74(f32 x, f32 y, f32 w);
void fn_80010D88(f32 x, f32 y, f32 h);
void fn_80010E94(int art, int part, f32 x, f32 y, f32 w, f32 h);
void fn_80010FDC(const char* text, int type);
int fn_8001084C(int size, const char* text);
int fn_80010964(int size);
f32 fn_800109A0(int size);
void fn_80011280(u32 color, int art, f32 x, f32 y, f32 w, f32 h);
char* fn_800122B4(s32 value, char* out);
void fn_8000F4F0(void);
void fn_80010BEC(void);

extern s32 lbl_8055CFF0;
extern s32 lbl_8055CFF4;
}

// Art box with eight measured parts (init_box_info).
class BoxInfo {
public:
    BoxInfo(void) { art = 0; }
    ~BoxInfo(void) {
        if (art) {
            fn_80235380(art);
        }
        art = 0;
    }

    void Init(const char* name, f32 m) {
        art = fn_8023512C(name);
        if (!art) {
            fn_802E30C8("init_box_info: art resource %s was not found!", name);
        } else if (fn_80235FD8(art) < 4) {
            fn_802E30C8("init_box_info: art resource %s doesn't have enough parts!", name);
            fn_80235380(art);
        } else {
            int n;
            int i;
            color = 0x66000000;
            n = fn_80235FD8(art);
            i = n < 8 ? n : 8;
            while (i-- > 0) {
                parts[i].w = fn_80235EF0(art, i);
                parts[i].h = fn_80235F64(art, i);
            }
            margin[0] = m;
            margin[1] = m;
            margin[3] = m;
            margin[2] = m;
        }
    }

    f32 margin[4];  // offset 0x0
    struct {
        f32 w;
        f32 h;
    } parts[8];  // offset 0x10
    int art;     // offset 0x50
    u32 color;   // offset 0x54
};

int GetNumPlayers(void);

class DashSoundPlayer {
public:
    DashSoundPlayer(void) {
        int i;
        int j;
        index = 0;
        for (i = 0; i < 3; i++) {
            sounds[0][i] = fn_80207848("rd_press.sfx", 0);
        }
        for (j = 0; j < 3; j++) {
            sounds[1][j] = fn_80207848("rd_accept.sfx", 0);
        }
        UpdateVolume();
    }
    ~DashSoundPlayer(void) {
        int i;
        int j;
        for (i = 0; i < 2; i++) {
            for (j = 0; j < 3; j++) {
                if (sounds[i][j]) {
                    delete sounds[i][j];
                }
            }
        }
    }
    void UpdateVolume(void) {
        f32 vol = 1.0f;
        int i;
        int j;
        fn_801331C4(4, "effects_volume", &vol);
        for (i = 0; i < 2; i++) {
            for (j = 0; j < 3; j++) {
                if (sounds[i][j]) {
                    sounds[i][j]->SetVolume(vol);
                }
            }
        }
    }

    s32 index;            // offset 0x0
    Sound* sounds[2][3];  // offset 0x4
};

extern "C" {

DashRowType lbl_802FC2A8[11] = {
    {0, 0, 4}, {1, 1, 5}, {2, 1, 5}, {3, 1, 5}, {4, 1, 5}, {5, 1, 5},
    {6, 2, 6}, {7, 2, 6}, {8, 2, 6}, {9, 2, 6}, {10, 1, 5},
};

ModuleTag lbl_8055D478;
UnkStatic7F lbl_8055D480;
UnkStatic7F lbl_8055D484;
UnkStatic7F lbl_8055D488;
UnkStatic7F lbl_8055D48C;

s32 lbl_80558A90 = -1;  // text line height
s32 lbl_80558A94 = -1;  // help arrow model
s32 lbl_80558A98 = -1;  // last countdown number
const char* lbl_80558A9C = "center_rows.art";
const char* lbl_80558AA0 = "centerrows2.art";

QueuedRect lbl_8034F6B0[256];
ScreenBox lbl_80350EB0;
ScreenBox lbl_80350EC4;
ScreenBox lbl_80350ED8[4];
ViewLayout lbl_80350F28[4];
DashRowList lbl_80350F88;  // left rows
DashRowList lbl_80351214;  // right rows
DashRowList lbl_803514A0;  // centre rows
BoxInfo lbl_80351738;

int lbl_8055D490;  // font
int lbl_8055D494;  // digit font
int lbl_8055D498;  // big text font
f32 lbl_8055D49C;  // digit width
s32 lbl_8055D4A0;  // number of views
u8 lbl_8055D4A4;
s32 lbl_8055D4A8;  // queued rects
DashSoundPlayer* lbl_8055D4AC;
Layout* lbl_8055D4B0;
Sound* lbl_8055D4B4;
Sound* lbl_8055D4B8;
int lbl_8055D4BC;
int lbl_8055D4C0;
int lbl_8055D4C4;
int lbl_8055D4C8;
int lbl_8055D4CC;
int lbl_8055D4D0;
int lbl_8055D4D4;
f32 lbl_8055D4D8;  // centre text wrap width

static inline f32 Min(f32 a, f32 b) { return a < b ? a : b; }
static inline f32 Max(f32 a, f32 b) { return a > b ? a : b; }
static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }
static inline f32 InvLerp(f32 x, f32 a, f32 b) { return (x - a) / (b - a); }
static inline f32 Clamp01(f32 x) { return Max(Min(1.0f, x), 0.0f); }

static inline DashRow* AddRow(DashRowList* list) { return &list->rows[list->count++]; }

int fn_8000DFA8(int type) { return lbl_802FC2A8[type].part; }

int fn_8000DFC0(int type) { return lbl_802FC2A8[type].part2; }

void fn_8000DFD8(DashRow* row, u32 color, f32 x, f32 y) {
    f32 w = 89.0f;
    f32 h = 17.0f;
    if (row->c > 0 && row->type == 9) {
        fn_800109E4(((int)(2.55f * row->c) << 24) | 0xFDB300, x, y + h * 0.5f, w, h);
    }
    fn_800109E4(-1, x, y + h * 0.5f, row->b * w / 100.0f, h);
    fn_800109E4(color, x, y + h * 0.5f, row->a * w / 100.0f, h);
    fn_80010074(-1, 0x14, "%s", 1.0f, x + w * 0.5f, y, row);
}

void fn_8000E15C(DashRow* row, u32 color, f32 x, f32 y) {
    f32 w = 89.0f;
    f32 h = 17.0f;
    if (row->c > 0 && row->type == 9) {
        fn_800109E4(((int)(2.55f * row->c) << 24) | 0xFDB300, x, y + h * 0.5f, w, h);
    }
    fn_800109E4(-1, x, y + h * 0.5f, row->b * w / 100.0f, h);
    fn_800109E4(color, x, y + h * 0.5f, row->a * w / 100.0f, h);
    fn_80010074(-1, 0x11, "%s", 1.0f, 4.0f + x, y, row);
}

u32 fn_8000E2D8(int kind) {
    switch (kind) {
    case 0:
    default:
        return 0xFFFFFFFF;
    case 1:
        return 0xFFD6003C;
    case 2:
        return 0xFF2DC82D;
    case 3:
        return 0xFF2D2DC8;
    case 4:
        return 0xFFFFFF00;
    }
}

u32 fn_8000E338(int value) {
    if (value <= 15) {
        if ((f32)fmod(fn_801E7E18(), 1.0) > 0.5f) {
            return 0xFFFFFFFF;
        }
    }
    if (value <= 40) {
        return 0xFFF70000;
    }
    return value <= 70 ? 0xFFE5A703 : 0xFF278F27;
}

void fn_8000E3B4(int player) {
    DashRowList* list = &lbl_80350F88;
    f32 top;
    f32 y;
    f32 right;
    f32 pad;
    f32 maxSlide;
    DashRow* row;
    int i;

    if (list->count < 1) {
        return;
    }
    if (player < 0) {
        top = fn_80011E54();
    } else {
        top = fn_80011DC0(player);
    }
    y = Min(0.0f, fn_80011EC8(player) - fn_80235F64(lbl_80350F88.art, fn_8000DFA8(0))) + top - 50.0f;
    if (player < 0) {
        right = fn_80011E34();
    } else {
        right = fn_80011DD4(player);
    }
    pad = lbl_80350F88.wide ? 56.0f : 0.0f;
    maxSlide = 0.0f;
    for (i = 0, row = lbl_80350F88.rows; i < lbl_80350F88.count; i++, row++) {
        char buf[20];
        int part = fn_8000DFA8(row->type);
        int part2 = fn_8000DFC0(row->type);
        f32 w = pad + fn_80235EF0(lbl_80350F88.art, part);
        f32 h = fn_80235F64(lbl_80350F88.art, part);
        f32 x = Lerp(-5.0f, right + w, row->slide) - w;
        f32 x5;
        f32 x10;
        f32 xr;
        f32 ty;
        fn_80235EF0(lbl_80350F88.art, part2);
        x5 = 5.0f + x;
        xr = x + w - 5.0f;
        x10 = 5.0f + x5;
        ty = 1.0f + (y - h * 0.5f);
        switch (row->type) {
        case 1:
            fn_80010074(fn_8000E2D8(row->a), 0x11, "%s", 1.0f, x10, ty, row);
            break;
        case 2:
            {
                f32 w3 = fn_800108C8("000", 1.0f);
                fn_80010074(-1, 0x12, "%d", 1.0f, x10 + w3, ty, row->a);
            }
            {
                f32 w4 = fn_800108C8("000 ", 1.0f);
                fn_80010074(-1, 0x11, "%s", 1.0f, x10 + w4, ty, row);
            }
            break;
        case 3:
            sprintf(buf, "%d", row->a);
            switch (row->b) {
            case 5:
                fn_800122B4(row->a, buf);
                break;
            case 6:
                sprintf(buf, "$%d", row->a);
                break;
            }
            fn_80010074(fn_8000E2D8(row->b), 0x11, "%s", 1.0f, x10, ty, row);
            fn_80010074(fn_8000E2D8(row->b), 0x12, "%s", 1.0f, xr - 4.0f, ty, buf);
            break;
        case 4:
            fn_80010074(-1, 0x11, "%s", 1.0f, x10, ty, row);
            fn_80010074(-1, 0x12, "%d/%d", 1.0f, xr - 4.0f, ty, row->a, row->b);
            break;
        case 5:
            fn_80010074(0xFFFFD03F, 0x11, "%s", 1.0f, x10, ty, row);
            if ((f32)fmod(fn_801E7E18(), 0.5) > 0.1f) {
                fn_80010074(0xFFFFD03F, 0x12, "%d/%d", 1.0f, xr - 4.0f, ty, row->a, row->b);
            }
            break;
        case 6:
            fn_8000DFD8(row, fn_8000E338(row->a), x5 + pad, ty);
            break;
        case 7:
            fn_8000DFD8(row, fn_8000E338(row->a), x5 + pad, ty);
            {
                f32 bx = x5 + pad;
                f32 bw = 89.0f;
                f32 bh = 17.0f;
                f32 fill = bx + row->c * bw / 100.0f;
                fn_800109E4(lbl_8055D48C.value, fill, ty + bh * 0.5f, bw + bx - fill, bh);
            }
            break;
        case 8:
            fn_8000DFD8(row, fn_8000E338(row->a), x5 + pad, ty);
            break;
        case 9:
            fn_8000DFD8(row, 0xFF2D2DC8, x5 + pad, ty);
            break;
        }
        maxSlide = Max(maxSlide, row->slide);
        y -= h - 1.0f;
    }
}

void fn_8000E874(int player) {
    f32 right;
    f32 top;
    f32 y;
    f32 pad;
    f32 maxSlide;
    DashRow* row;
    int i;

    if (lbl_80351214.count < 1) {
        return;
    }
    if (player < 0) {
        right = fn_80011E34();
    } else {
        right = fn_80011DEC(player);
    }
    if (player < 0) {
        top = fn_80011E54();
    } else {
        top = fn_80011DC0(player);
    }
    y = Min(0.0f, fn_80011EC8(player) - fn_80235F64(lbl_80351214.art, fn_8000DFA8(0))) + top;
    pad = lbl_80351214.wide ? 56.0f : 0.0f;
    maxSlide = 0.0f;
    for (i = 0, row = lbl_80351214.rows; i < lbl_80351214.count; i++, row++) {
        char buf[20];
        int part = fn_8000DFA8(row->type);
        int part2 = fn_8000DFC0(row->type);
        f32 w = fn_80235EF0(lbl_80351214.art, part) + pad;
        f32 h = fn_80235F64(lbl_80351214.art, part);
        f32 x = Lerp(5.0f + lbl_8055CFF0, right, row->slide);
        f32 x5;
        f32 x9;
        f32 xr;
        f32 ty;
        fn_80235EF0(lbl_80351214.art, part2);
        x9 = 9.0f + x;
        x5 = 5.0f + x;
        xr = x + w;
        ty = 1.0f + (y - h * 0.5f);
        switch (row->type) {
        case 1:
            fn_8000FF88(fn_8000E2D8(row->a), 0x12, "%s", 1.1f, x9, ty, row);
            break;
        case 2:
            fn_8000FF88(-1, 0x12, "%d %s", 1.4f, x9, ty, row->a, row);
            break;
        case 3:
            sprintf(buf, "%d", row->a);
            switch (row->b) {
            case 5:
                fn_800122B4(row->a, buf);
                break;
            case 6:
                sprintf(buf, "$%d", row->a);
                break;
            }
            fn_8000FF88(fn_8000E2D8(row->b), 0x11, "%s", 1.0f, x9, ty, row);
            fn_8000FF88(fn_8000E2D8(row->b), 0x12, "%s", 1.0f, xr - 4.0f, ty, buf);
            break;
        case 4:
            fn_8000FF88(-1, 0x11, "%s", 1.0f, x9, ty, row);
            fn_8000FF88(-1, 0x12, "%d/%d", 1.0f, xr - 4.0f, ty, row->a, row->b);
            break;
        case 5:
            fn_8000FF88(0xFFFFD03F, 0x11, "%s", 1.0f, x9, ty, row);
            if ((f32)fmod(fn_801E7E18(), 0.5) > 0.1f) {
                fn_8000FF88(0xFFFFD03F, 0x12, "%d/%d", 1.0f, xr - 4.0f, ty, row->a, row->b);
            }
            break;
        case 6:
            fn_8000DFD8(row, fn_8000E338(row->a), x5, ty);
            break;
        case 7:
            fn_8000DFD8(row, fn_8000E338(row->a), x5, ty);
            {
                f32 bw = 89.0f;
                f32 bh = 17.0f;
                fn_800109E4(lbl_8055D48C.value, x5 + row->c * bw / 100.0f - 5.0f, ty + bh * 0.5f, 2.0f, bh);
            }
            break;
        case 8:
            fn_8000DFD8(row, fn_8000E338(row->a), x5 + pad, ty);
            break;
        case 9:
            fn_8000E15C(row, 0xFF2D2DC8, x5, ty);
            break;
        case 10:
            fn_8000FF88(fn_8000E2D8(row->a), 0x12, "%s", 1.0f, x9, ty, row);
            if (row->b >= 0) {
                f32 size = 20.0f;
                fn_80010E94(lbl_8055D4CC, row->b, x9, ty + 0.5f * size, size, size);
            }
            break;
        }
        maxSlide = Max(maxSlide, row->slide);
        y -= h - 1.0f;
    }
    {
        int part = fn_8000DFA8(0);
        int part2 = fn_8000DFC0(0);
        fn_80235EF0(lbl_80351214.art, part);
        fn_80235EF0(lbl_80351214.art, part2);
        fn_80235F64(lbl_80351214.art, part);
        f32 sw = lbl_8055CFF0;
    }
}

f32 fn_8000EDC0(int type) {
    if (fn_801E85A4() > 2) {
        return 1.0f;
    }
    switch (type) {
    case 11:
        break;
    case 12:
        return 1.5f;
    case 13:
        return 1.8f;
    case 14:
        return 1.0f;
    default:
        fn_802E317C("get_center_item_scale(%d) - not a center (DCIT_) item!", type);
        break;
    }
    return 1.0f;
}

u32 fn_8000EE54(int type) {
    switch (type) {
    case 11:
        break;
    case 12:
        return 0xFFFFAC00;
    case 13:
        return 0xFFFFFFFF;
    case 14:
        return 0xFFFFAC00;
    default:
        fn_802E317C("get_center_item_color(%d) - not a center (DCIT_) item!", type);
        break;
    }
    return 0xFFFFFFFF;
}

f32 fn_8000EEC8(int player, f32 height) {
    f32 y;
    if (fn_801E85A4() == 1) {
        if (player < 0) {
            y = fn_80011E54();
        } else {
            y = fn_80011DC0(player);
        }
        return y - 100.0f;
    }
    if (player < 0) {
        y = fn_80011E90();
    } else {
        y = 0.8f * fn_80011E1C(player) + 0.2f * fn_80011DC0(player);
    }
    return y + 0.5f * height;
}

void fn_8000EF74(int player) {
    f32 x;
    DashRow* row;
    f32 total;
    int j;
    f32 y;
    int i;

    if (lbl_803514A0.count == 0) {
        return;
    }
    if (player < 0) {
        x = fn_80011E70();
    } else {
        x = fn_80011E04(player);
    }
    total = 0.0f;
    for (i = 0; i < lbl_803514A0.count; i++) {
        total += fn_8000EDC0(lbl_803514A0.rows[i].type);
    }
    total *= lbl_80558A90;
    y = fn_8000EEC8(player, total);
    for (j = 0, row = lbl_803514A0.rows; j < lbl_803514A0.count; row++, j++) {
        f32 scale = fn_8000EDC0(row->type);
        f32 lineH = scale * lbl_80558A90;
        f32 ty = y - 0.5f * lineH;
        fn_80010074(fn_8000EE54(row->type), 0x14, row->text, scale, x, ty);
        y -= lineH;
    }
}

void fn_8000F0E4(void) {
    lbl_8055D490 = fn_8023441C();
    lbl_80558A90 = 0.73333335f * fn_80232B70(lbl_8055D490);
    lbl_8055D49C = fn_80011EDC();
    lbl_8055D498 = fn_80232578("bigtext.gtx");
}

void fn_8000F158(void) {
    fn_802328D4(lbl_8055D498);
    fn_80234470();
    lbl_8055D490 = 0;
}

void fn_8000F188(const char* name) { lbl_80558A94 = fn_802C6FC0(name, 0, 0); }

void fn_8000F1B4(void) {
    fn_802C6CF8(lbl_80558A94);
    lbl_80558A94 = -1;
}

void fn_8000F1E0(const char* name) {
    lbl_8055D4BC = fn_8023512C(name);
    lbl_8055D4B4 = fn_80207848("321low.sfx", 1);
    lbl_8055D4B8 = fn_80207848("321high.sfx", 1);
}

void fn_8000F22C(void) {
    if (lbl_8055D4BC) {
        fn_80235380(lbl_8055D4BC);
    }
    lbl_8055D4BC = 0;
    if (lbl_8055D4B4) {
        delete lbl_8055D4B4;
    }
    if (lbl_8055D4B8) {
        delete lbl_8055D4B8;
    }
}

void fn_8000F2A8(const char* left, const char* right) {
    lbl_80350F88.art = fn_8023512C(left);
    lbl_80351214.art = fn_8023512C(right);
}

void fn_8000F2EC(void) {
    if (lbl_80350F88.art) {
        fn_80235380(lbl_80350F88.art);
    }
    if (lbl_80351214.art) {
        fn_80235380(lbl_80351214.art);
    }
}

void fn_8000F330(const char* art, const char* font, const char* font2) {
    lbl_8055D4C0 = fn_8023512C(art);
    lbl_8055D4C4 = fn_80232578(font);
    lbl_8055D4C8 = fn_80232578(font2);
}

void fn_8000F384(void) {
    if (lbl_8055D4C0) {
        fn_80235380(lbl_8055D4C0);
        lbl_8055D4C0 = 0;
    }
    if (lbl_8055D4C4) {
        fn_802328D4(lbl_8055D4C4);
        lbl_8055D4C4 = 0;
    }
    if (lbl_8055D4C8) {
        fn_802328D4(lbl_8055D4C8);
        lbl_8055D4C8 = 0;
    }
}

void fn_8000F3E8(const char* name) { lbl_8055D4D0 = fn_8023512C(name); }

void fn_8000F40C(void) {
    if (lbl_8055D4D0) {
        fn_80235380(lbl_8055D4D0);
        lbl_8055D4D0 = 0;
    }
}

void fn_8000F440(const char* name) { lbl_8055D4CC = fn_8023512C(name); }

void fn_8000F464(void) {
    if (lbl_8055D4CC) {
        fn_80235380(lbl_8055D4CC);
        lbl_8055D4CC = 0;
    }
}

void fn_8000F498(const char* name) { lbl_8055D4D4 = fn_8023512C(name); }

void fn_8000F4BC(void) {
    if (lbl_8055D4D4) {
        fn_80235380(lbl_8055D4D4);
        lbl_8055D4D4 = 0;
    }
}

static inline void CopyString(const char* src, int size, char* dst) {
    char* d = dst;
    int n = size;
    const char* s = src;
    while ((*d++ = *s++) != 0) {
        if (--n <= 0) {
            d[-1] = 0;
            break;
        }
    }
}

static inline u32 WithAlpha(u32 color, s32 alpha) { return (color & 0xFFFFFF) | (alpha << 24); }

void fn_8000F4F0(void) {
    Viewport vp;
    f32 t;
    int num;
    u8 alpha;
    f32 phase;

    fn_802C86F0(&vp, 0, 0, lbl_8055CFF0, lbl_8055CFF4);
    t = fn_801E7E3C()->elapsed;
    num = -1;
    char text[32] = {0};
    alpha = 255;
    phase = 0.0f;
    if (t > 3.0f) {
    } else if (t > 0.01f && t <= 3.0f) {
        char key[20];
        num = (f32)ceil(t);
        if (num != lbl_80558A98) {
            lbl_8055D4B4->Play();
        }
        phase = num - t;
        fn_801345D8(key, sizeof(key), "BigText:321_GO_%d", num);
        CopyString(fn_80131268(key), sizeof(text), text);
    } else if (-t < 1.5f) {
        f32 left;
        num = 0;
        if (lbl_80558A98 != 0) {
            lbl_8055D4B8->Play();
        }
        phase = Lerp(0.0f, 1.0f, Clamp01(InvLerp(-t, 0.5f, 1.5f)));
        left = 1.5f - -t;
        if (left < 0.7f) {
            alpha = alpha * (left / 0.7f);
        }
        CopyString(fn_80131268("BigText:321_GO_GO"), sizeof(text), text);
    }
    lbl_80558A98 = num;
    if (lbl_8055D4B0) {
        LayoutItem* in = lbl_8055D4B0->Find("321GO_in");
        LayoutItem* out = lbl_8055D4B0->Find("321GO_out");
        if (in && out) {
            LayoutItem a;
            LayoutItem b;
            a = *in;
            b = *out;
            a.color = (alpha << 24) | (a.color & 0xFFFFFF);
            b.color = a.color;
            fn_80232928(-1);
            fn_80147ABC(&a, &b, "%s", phase, text);
            fn_80232930();
        }
    }
    fn_802C8750(&vp, -1);
}


static inline f32 ArcU(f32 c) { return Clamp01((c - -1.0f) * 0.5f); }

static inline f32 ArcV(f32 s) { return Clamp01((s - 1.0f) / -2.0f); }

// Stripped from ET; only its constants remain.
void GetArcUV(f32 c, f32 s, f32* u, f32* v) {
    *u = ArcU(c);
    *v = ArcV(s);
}

void fn_8000F884(void) {
    int n;
    fn_8000F188("hlparrow.mod");
    fn_8000F2A8("left_rows.art", "right_rows.art");
    fn_8000F1E0("321go.art");
    fn_8000F330("interlude.art", "uifnt.gtx", "uifnt.gtx");
    fn_8000F440("replayxb.art");
    fn_8000F3E8("uielems.art");
    fn_8000F498("nicebox.art");
    lbl_8055D494 = fn_80232578("digiface.gtx");
    lbl_8055D4B0 = fn_80145E70("laptitle.val");
    n = GetNumPlayers();
    if (n > 1) {
        lbl_80351738.Init(lbl_80558AA0, 13.0f);
    } else {
        lbl_80351738.Init(lbl_80558A9C, 23.0f);
    }
    lbl_8055D4D8 = n > 2 ? 128.0f : 240.0f;
    if (n == 1) {
        lbl_802FC2A8[0].part = 0;
        lbl_802FC2A8[0].part2 = 4;
    } else {
        lbl_802FC2A8[0].part = 3;
        lbl_802FC2A8[0].part2 = 7;
    }
    lbl_80558A98 = -1;
}

void fn_8000FB44(void) {
    if (lbl_80351738.art) {
        fn_80235380(lbl_80351738.art);
    }
    lbl_80351738.art = 0;
    fn_802328D4(lbl_8055D494);
    lbl_8055D494 = 0;
    if (lbl_8055D4B0) {
        delete lbl_8055D4B0;
    }
    lbl_8055D4B0 = 0;
    fn_8000F40C();
    fn_8000F464();
    fn_8000F384();
    fn_8000F1B4();
    fn_8000F2EC();
    fn_8000F22C();
    fn_8000F4BC();
}

void fn_8000FBE4(void) { fn_8000F4F0(); }

void fn_8000FBE8(f32 x, f32 y, f32 scale) {
    f32 y1 = y + 1.0f;
    f32 h = fn_80235F64(lbl_8055D4CC, 4) * scale;
    f32 size = 10.0f + h;
    f32 bx = x - 5.0f;
    f32 by = 5.0f + (y1 + h * 0.5f);
    fn_80010E94(lbl_8055D4D0, 4, bx, by, size, size);
    fn_80010E94(lbl_8055D4D0, 5, bx + size, by, 640.0f, size);
}

void fn_8000FC8C(const char* text) {
    f32 scale = 35.0f / fn_80232B70(lbl_8055D4C4);
    f32 x = fn_80011E70();
    f32 y = fn_80011E54();
    if (lbl_8055D4C0) {
        f32 w0 = fn_80235EF0(lbl_8055D4C0, 0);
        f32 bx = x - 0.5f * (w0 + fn_80235EF0(lbl_8055D4C0, 1));
        f32 by = y - 17.5f + 0.5f * fn_80235F64(lbl_8055D4C0, 0);
        fn_80010E94(lbl_8055D4C0, 0, bx, by, 0.0f, 0.0f);
        fn_80010E94(lbl_8055D4C0, 1, bx + w0, by, 0.0f, 0.0f);
    }
    fn_802337A0(lbl_8055D4C4, 0xFF000000, 12, "%s", 2.0f + x, y - 2.0f, 0.0f, scale, 1.0f, text);
    fn_802337A0(lbl_8055D4C4, -1, 12, "%s", x, y, 0.0f, scale, 1.0f, text);
}

void fn_8000FDF0(const char* text) {
    f32 scale = 27.0f / fn_80232B70(lbl_8055D4C8);
    f32 x = fn_80011E70();
    f32 y = 13.0f + fn_80011E60();
    f32 w = scale * fn_8023293C(lbl_8055D4C8, text, 1.0f);
    f32 bx = x - 0.5f * w;
    fn_800109E4(0x66000000, bx - 7.0f, 13.0f + y, 14.0f + w, 26.0f);
    if (lbl_8055D4C0) {
        f32 by = y + 0.5f * fn_80235F64(lbl_8055D4C0, 2);
        fn_80010E94(lbl_8055D4C0, 2, bx - fn_80235EF0(lbl_8055D4C0, 2) - 7.0f, by, 0.0f, 0.0f);
        fn_80010E94(lbl_8055D4C0, 2, 7.0f + (bx + w), by, 0.0f, 0.0f);
    }
    {
        f32 ty = 2.0f + y;
        fn_802337A0(lbl_8055D4C8, 0xFF000000, 0x14, "%s", 1.0f + x, ty - 1.0f, 0.0f, scale, 1.0f, text);
        fn_802337A0(lbl_8055D4C8, -1, 0x14, "%s", x, ty, 0.0f, scale, 1.0f, text);
    }
}

void fn_8000FF88(u32 color, int flags, const char* fmt, f32 scale, f32 x, f32 y, ...) {
    va_list args;
    char buf[0x200];
    va_start(args, y);
    vsprintf(buf, fmt, args);
    va_end(args);
    fn_802344CC(buf, color, flags, x, y, 0.73333335f * scale, 0);
}

void fn_80010074(u32 color, int flags, const char* fmt, f32 scale, f32 x, f32 y, ...) {
    va_list args;
    char buf[0x200];
    va_start(args, y);
    vsprintf(buf, fmt, args);
    va_end(args);
    fn_802344CC(buf, color & 0xFF000000, flags, 2.0f + x, y - 2.0f, 0.73333335f * scale, 0);
    fn_802344CC(buf, color, flags, x, y, 0.73333335f * scale, 0);
}

void fn_80010188(u32 color, int flags, const char* fmt, f32 scale, f32 x, f32 y, ...) {
    va_list args;
    char buf[0x200];
    va_start(args, y);
    vsprintf(buf, fmt, args);
    va_end(args);
    fn_80232BA4(buf, lbl_8055D490, color, flags, x, y, 0.73333335f * scale, 0.0f, 1.0f);
}

void fn_8001027C(u32 color, int flags, const char* fmt, f32 scale, f32 x, f32 y, ...) {
    va_list args;
    char buf[0x200];
    if (lbl_8055D494) {
        va_start(args, y);
        vsprintf(buf, fmt, args);
        va_end(args);
        if (strlen(buf) >= 0x200) {
            fn_802E317C("dashhelper: oversized text elem (len %d, fmt \"%s\")", strlen(buf), fmt);
        }
        fn_802337A0(lbl_8055D494, color, flags, "%s", x, y, 0.0f, scale, 1.0f, buf);
    }
}

void fn_800103B8(u32 color, int flags, const char* fmt, f32 scale, f32 x, f32 y, f32 w, ...) {
    va_list args;
    char buf[0x200];
    va_start(args, w);
    vsprintf(buf, fmt, args);
    va_end(args);
    fn_80233010(buf, lbl_8055D490, color, flags, x, y, w, 0.73333335f * scale, 0.0f, 1.0f, 1);
}

void fn_800104B0(u32 color, int flags, const char* fmt, f32 scale, f32 x, f32 y, ...) {
    va_list args;
    char buf[0x200];
    va_start(args, y);
    vsprintf(buf, fmt, args);
    va_end(args);
    fn_80232BA4(buf, lbl_8055D498, color, flags, x, y, scale, 0.0f, 1.0f);
}

void fn_800105A0(const char* text, int player) {
    int many = fn_80137C74() > 2;
    f32 scale = many ? 1.0f : 1.2f;
    f32 w = 10.0f + fn_800108C8(text, scale);
    f32 h = many ? 23.0f : 26.0f;
    f32 x = -w / 2 + fn_80011E04(player);
    f32 cx = fn_80011E04(player);
    f32 y = (many ? 0 : -40) + fn_80011E1C(player);
    fn_80010B80(x, y, w, h, 0x90000000);
    fn_80010C74(x, 1.0f + y, 1.0f + w);
    fn_80010D88(x + w, y, h);
    fn_80010C74(x, y - h, 1.0f + w);
    fn_80010D88(x, y, h);
    fn_8000FF88(-1, 0xC, text, scale, cx, y);
}

void fn_8001071C(const char* text) { fn_80010FDC(text, 13); }

void fn_80010724(int x, int y, const char* text) {
    f32 w = 14.0f + fn_8001084C(16, text);
    f32 h = 6.0f + fn_80010964(16);
    f32 bx = (x - 320.0f) - w / 2;
    f32 by = (240.0f - y) + h / 2;
    fn_80010B80(bx, by, w, h, 0x80000000);
    fn_8000FF88(-1, 0x14, "%s", fn_800109A0(lbl_80558A90), bx, by, text);
}

int fn_8001084C(int size, const char* text) {
    return size * fn_8023293C(lbl_8055D490, text, 1.0f) / lbl_80558A90;
}

f32 fn_800108C8(const char* text, f32 scale) {
    return 0.73333335f * (scale * fn_8023293C(lbl_8055D490, text, 1.0f));
}

f32 fn_8001090C(f32 scale) { return 0.73333335f * (scale * fn_80232B70(lbl_8055D490)); }

int fn_80010964(int size) { return size * fn_80232B70(lbl_8055D490) / lbl_80558A90; }

f32 fn_800109A0(int size) { return (f32)size / (f32)lbl_80558A90; }

void fn_800109E4(u32 color, f32 x, f32 y, f32 w, f32 h) {
    fn_80010B74();
    fn_80010A50(x, y, w, h, color, color);
    fn_80010B7C();
}

void fn_80010A50(f32 x, f32 y, f32 w, f32 h, u32 color0, u32 color1) {
    Vertex v[4] __attribute__((aligned(16)));
    fn_80231380(&v[0], color0, x, y, 1.0f, 0.0f, 0.0f);
    fn_80231380(&v[1], color1, x + w, y, 1.0f, 0.0f, 0.0f);
    fn_80231380(&v[2], color0, x, y - h, 1.0f, 0.0f, 0.0f);
    fn_80231380(&v[3], color1, x + w, y - h, 1.0f, 0.0f, 0.0f);
    u16 idx[6] = {0, 1, 3, 0, 3, 2};
    fn_802C8DB8(v, 4, idx, 6);
}

void fn_80010B74(void) { fn_802C8BD0(-1); }

void fn_80010B7C(void) { fn_802C8D74(); }

void fn_80010B80(f32 x, f32 y, f32 w, f32 h, u32 color) {
    QueuedRect r;
    r.x = x;
    r.y = y;
    r.w = w;
    r.h = h;
    r.color0 = color;
    r.color1 = color;
    if (lbl_8055D4A8 < 256) {
        lbl_8034F6B0[lbl_8055D4A8++] = r;
    }
}

void fn_80010BEC(void) {
    int i;
    QueuedRect* r;
    if (lbl_8055D4A8 > 0) {
        fn_80010B74();
        for (i = 0; i < lbl_8055D4A8; i++) {
            r = &lbl_8034F6B0[i];
            fn_80010A50(r->x, r->y, r->w, r->h, r->color0, r->color1);
        }
        fn_80010B7C();
        lbl_8055D4A8 = 0;
    }
}

void fn_80010C74(f32 x, f32 y, f32 w) {
    if (lbl_8055D4A8 < 255) {
        QueuedRect a = {x, y, w, 1.0f, lbl_8055D484.value, lbl_8055D484.value};
        QueuedRect b = {1.0f + x, y - 1.0f, w, 1.0f, lbl_8055D488.value, lbl_8055D488.value};
        lbl_8034F6B0[lbl_8055D4A8++] = a;
        lbl_8034F6B0[lbl_8055D4A8++] = b;
    }
}

void fn_80010D88(f32 x, f32 y, f32 h) {
    if (lbl_8055D4A8 < 255) {
        QueuedRect a = {x, y, 1.0f, h, lbl_8055D484.value, lbl_8055D484.value};
        QueuedRect b = {1.0f + x, y - 1.0f, 1.0f, h, lbl_8055D488.value, lbl_8055D488.value};
        lbl_8034F6B0[lbl_8055D4A8++] = a;
        lbl_8034F6B0[lbl_8055D4A8++] = b;
    }
}

void fn_80010E94(int art, int part, f32 x, f32 y, f32 w, f32 h) {
    if (0.0f == w && 0.0f == h) {
        fn_802353F4(art, part, x, y, 1.0f);
    } else {
        fn_802354A0(art, part, -1, x, y, 1.0f, w, h);
    }
}

void fn_80010EF0(void);
void fn_80010F08(void);
void fn_80010F20(void);

void fn_80010EC8(void) {
    fn_80010EF0();
    fn_80010F08();
    fn_80010F20();
}

void fn_80010EF0(void) {
    lbl_80350F88.count = 0;
    lbl_80350F88.wide = 0;
}

void fn_80010F08(void) {
    lbl_80351214.count = 0;
    lbl_80351214.wide = 0;
}

void fn_80010F20(void) {
    lbl_803514A0.count = 0;
    lbl_803514A0.wide = 0;
}

void fn_80010F38(const char* text, s32 a, s32 b, s32 type, f32 slide) {
    DashRow* row = AddRow(&lbl_80350F88);
    strncpy(row->text, text, 40);
    row->text[40] = 0;
    row->a = a;
    row->b = b;
    row->c = 0;
    row->type = type;
    row->slide = slide;
}

void fn_80010FDC(const char* text, int type) {
    char buf[0x200];
    int n;
    int space;
    DashRow* row;
    f32 wrap = lbl_8055D4D8;
    f32 scale = fn_8000EDC0(type);
    if (fn_800108C8(text, scale) > wrap) {
        while (*text) {
            memset(buf, 0, sizeof(buf));
            n = 0;
            space = -1;
            while (*text && *text != '\n' && fn_800108C8(buf, scale) < wrap) {
                if (*text == ' ') {
                    space = n;
                }
                buf[n++] = *text++;
            }
            if (*text == '\n') {
                text++;
            } else if (*text && space >= 0) {
                buf[space] = 0;
                text -= n - space - 1;
            }
            row = AddRow(&lbl_803514A0);
            strncpy(row->text, buf, 40);
            row->text[40] = 0;
            row->a = 0;
            row->b = 0;
            row->c = 0;
            row->type = type;
            row->slide = 1.0f;
        }
    } else {
        row = AddRow(&lbl_803514A0);
        strncpy(row->text, text, 40);
        row->text[40] = 0;
        row->a = 0;
        row->b = 0;
        row->c = 0;
        row->type = type;
        row->slide = 1.0f;
    }
}

void fn_800111A4(const char* text, s32 a, s32 b, s32 c, s32 type, f32 slide) {
    DashRow* row = AddRow(&lbl_80350F88);
    strncpy(row->text, text, 40);
    row->text[40] = 0;
    row->a = a;
    row->b = b;
    row->type = type;
    row->slide = slide;
    row->c = c;
}

void fn_8001123C(int player) {
    fn_8000E3B4(player);
    fn_8000E874(player);
    fn_8000EF74(player);
}

void fn_80011278(u32 color, f32 x, f32 y, f32 w, f32 h) { fn_80011280(color, lbl_8055D4D4, x, y, w, h); }

struct Vec2 {
    f32 x;
    f32 y;
};

void fn_80011280(u32 color, int art, f32 x, f32 y, f32 w, f32 h) {
    Vec2 p;
    f32 pw;
    f32 a;
    f32 b;

    pw = fn_80235EF0(art, 0);
    b = fn_80235F64(art, 0);
    p.x = x;
    p.y = y;
    fn_802354A0(art, 0, color, p.x, p.y, 0.0f, pw, b);

    pw = fn_80235EF0(art, 1);
    p.y = y;
    p.x = x + w - pw;
    fn_802354A0(art, 1, color, p.x, p.y, 0.0f, pw, fn_80235F64(art, 1));

    pw = fn_80235EF0(art, 2);
    b = fn_80235F64(art, 2);
    p.x = x;
    p.y = b + (y - h);
    fn_802354A0(art, 2, color, p.x, p.y, 0.0f, pw, fn_80235F64(art, 2));

    pw = fn_80235EF0(art, 3);
    p.x = x + w - pw;
    b = fn_80235F64(art, 3);
    p.y = b + (y - h);
    fn_802354A0(art, 3, color, p.x, p.y, 0.0f, pw, fn_80235F64(art, 3));

    a = fn_80235EF0(art, 0);
    p.y = y;
    p.x = x + a;
    pw = w - (a + fn_80235EF0(art, 1));
    fn_802354A0(art, 4, color, p.x, p.y, 0.0f, pw, fn_80235F64(art, 0));

    a = fn_80235EF0(art, 2);
    p.x = x + a;
    pw = w - (a + fn_80235EF0(art, 3));
    b = fn_80235F64(art, 2);
    p.y = b + (y - h);
    fn_802354A0(art, 5, color, p.x, p.y, 0.0f, pw, b);

    pw = fn_80235EF0(art, 0);
    a = fn_80235F64(art, 0);
    p.x = x;
    p.y = y - a;
    fn_802354A0(art, 6, color, p.x, p.y, 0.0f, pw, h - (a + fn_80235F64(art, 2)));

    pw = fn_80235EF0(art, 1);
    p.x = x + w - pw;
    a = fn_80235F64(art, 1);
    p.y = y - a;
    fn_802354A0(art, 7, color, p.x, p.y, 0.0f, pw, h - (a + fn_80235F64(art, 3)));

    a = fn_80235EF0(art, 0);
    p.x = x + a;
    pw = w - a - fn_80235EF0(art, 1);
    b = fn_80235F64(art, 0);
    p.y = y - b;
    fn_802354A0(art, 8, color, p.x, p.y, 0.0f, pw, h - b - fn_80235F64(art, 2));
}

static inline void Rotate(Vec2* v, f32 angle) {
    f32 c = cos(angle);
    f32 s = sin(angle);
    f32 x = v->x;
    f32 y = v->y;
    v->y = x * s + y * c;
    v->x = x * c - y * s;
}

void fn_80011630(u32 color, int segs, s32 tex, f32 x, f32 y, f32 w, f32 h, f32 start, f32 step,
                 f32 u0, f32 v0, f32 u1, f32 v1) {
    Vec2 p;
    Vec2 d;
    f32 ratio;
    int i;

    if (segs <= 0) {
        return;
    }
    fn_802C8B98(tex, fn_8022EDC4());
    fn_801E4F20(segs * 4);
    d.y = sin(start);
    d.x = cos(start);
    ratio = h / w;
    p.y = d.y * w;
    p.x = d.x * w;
    fn_801E4F3C(color, 0, x + p.x, y + p.y, 0.0f, Lerp(u0, u1, ArcU(d.x)),
                Lerp(v0, v1, ArcV(d.y)));
    fn_801E4F3C(color, 0, x + ratio * p.x, y + ratio * p.y, 0.0f,
                Lerp(u0, u1, ArcU(ratio * d.x)),
                Lerp(v0, v1, ArcV(ratio * d.y)));
    for (i = 0; i < segs; i++) {
        Rotate(&p, step);
        Rotate(&d, step);
        fn_801E4F3C(color, 1, x + p.x, y + p.y, 0.0f, Lerp(u0, u1, ArcU(d.x)),
                    Lerp(v0, v1, ArcV(d.y)));
        fn_801E4F3C(color, 1, x + ratio * p.x, y + ratio * p.y, 0.0f,
                    Lerp(u0, u1, ArcU(ratio * d.x)),
                    Lerp(v0, v1, ArcV(ratio * d.y)));
    }
    fn_801E4FF4();
    fn_802C8D74();
}

void fn_80011A2C(void) {
    fn_80010BEC();
    fn_802346C8();
}

void fn_80011A50(void) {
    lbl_8055D4A8 = 0;
    fn_80234AEC();
}

void fn_80011A5C(int n) {
    int left;
    int right;
    int top;
    int bottom;
    ScreenBox views[4];
    f32 l;
    f32 r;
    f32 t;
    f32 b;
    int i;
    int rows;
    int cols;

    lbl_8055D4A0 = n;
    fn_80231094(&top, &bottom, &left, &right);
    l = left;
    r = right;
    t = top;
    b = bottom;
    if (n == 1) {
        views[0].top = t;
        views[0].bottom = b;
        views[0].left = l;
        views[0].right = r;
        views[0].width = 320.0f;
    } else if (n == 2) {
        views[0].top = t;
        views[0].bottom = 0.0f;
        views[0].left = l;
        views[0].right = r;
        views[1].top = 0.0f;
        views[1].bottom = b;
        views[1].left = l;
        views[1].right = r;
        views[0].width = 320.0f;
        views[1].width = 320.0f;
    } else {
        views[0].top = t;
        views[0].bottom = 0.0f;
        views[0].left = l;
        views[0].right = 5.0f;
        views[1].top = t;
        views[1].bottom = 0.0f;
        views[1].left = 5.0f;
        views[1].right = r;
        views[2].top = 0.0f;
        views[2].bottom = b;
        views[2].left = l;
        views[2].right = 5.0f;
        views[3].top = 0.0f;
        views[3].bottom = b;
        views[3].left = 5.0f;
        views[3].right = r;
        views[0].width = (5.0f - l) / 2;
        views[1].width = (r - 5.0f) / 2;
        views[2].width = (5.0f - l) / 2;
        views[3].width = (r - 5.0f) / 2;
    }
    lbl_80350EC4.top = 240.0f - t;
    lbl_80350EC4.bottom = -240.0f + b;
    lbl_80350EC4.left = -320.0f + l;
    lbl_80350EC4.right = 320.0f - r;
    if (n < 3) {
        lbl_80350EB0 = lbl_80350EC4;
    } else {
        lbl_80350EB0.top = 120.0f - t;
        lbl_80350EB0.bottom = -120.0f + b;
        lbl_80350EB0.left = -160.0f + l;
        lbl_80350EB0.right = 160.0f - r;
    }
    rows = ((lbl_8055D4A0 > 1) + 1) * 2;
    cols = ((lbl_8055D4A0 > 2) + 1) * 2;
    for (i = 0; i < lbl_8055D4A0; i++) {
        lbl_80350ED8[i] = views[i];
        lbl_80350F28[i].top = 480.0f / rows - views[i].top;
        lbl_80350F28[i].bottom = -480.0f / rows + views[i].bottom;
        lbl_80350F28[i].left = -640.0f / cols + views[i].left;
        lbl_80350F28[i].right = 640.0f / cols - views[i].right;
        lbl_80350F28[i].centerX = (lbl_80350F28[i].left + lbl_80350F28[i].right) / 2;
        lbl_80350F28[i].centerY = (lbl_80350F28[i].top + lbl_80350F28[i].bottom) / 2;
    }
    lbl_8055D4A4 = 1;
}

f32 fn_80011DC0(int player) { return lbl_80350F28[player].top; }

f32 fn_80011DD4(int player) { return lbl_80350F28[player].left; }

f32 fn_80011DEC(int player) { return lbl_80350F28[player].right; }

f32 fn_80011E04(int player) { return lbl_80350F28[player].centerX; }

f32 fn_80011E1C(int player) { return lbl_80350F28[player].centerY; }

f32 fn_80011E34(void) { return lbl_80350EC4.left; }

f32 fn_80011E44(void) { return lbl_80350EC4.right; }

f32 fn_80011E54(void) { return lbl_80350EC4.top; }

f32 fn_80011E60(void) { return lbl_80350EC4.bottom; }

f32 fn_80011E70(void) { return (lbl_80350EC4.left + lbl_80350EC4.right) / 2; }

f32 fn_80011E90(void) { return (lbl_80350EC4.top + lbl_80350EC4.bottom) / 2; }

f32 fn_80011EB0(int player) { return lbl_80350ED8[player].right; }

f32 fn_80011EC8(int player) { return lbl_80350ED8[player].top; }

f32 fn_80011EDC(void) {
    f32 max = 0.0f;
    int i;
    for (i = 0; i < 10; i++) {
        char buf[8];
        f32 w;
        sprintf(buf, "%c", (char)('0' + i));
        w = 0.73333335f * fn_8023293C(lbl_8055D490, buf, 1.0f);
        if (w > max) {
            max = w;
        }
    }
    return max;
}

void fn_80011F64(void) { lbl_8055D4AC = new ("DashSoundPlayer") DashSoundPlayer; }

void fn_800120BC(void) {
    delete lbl_8055D4AC;
    lbl_8055D4AC = 0;
}

void fn_80012158(void) {
    DashSoundPlayer* p = lbl_8055D4AC;
    Sound* s = p->sounds[0][p->index];
    if (s) {
        s->Play();
        if (++p->index >= 3) {
            p->index = 0;
        }
    }
}

void fn_800121A4(void) {
    DashSoundPlayer* p = lbl_8055D4AC;
    Sound* s = p->sounds[1][p->index];
    if (s) {
        s->Play();
        if (++p->index >= 3) {
            p->index = 0;
        }
    }
}

void fn_800121F0(void) { lbl_8055D4AC->UpdateVolume(); }

char* fn_800122B4(s32 value, char* out) {
    struct {
        char text[20];
        const char* suffix;
    } num;
    fn_801310B4(num.text, sizeof(num.text), value);
    num.suffix = fn_80130FE0(value);
    strcpy(out, num.text);
    return out;
}

}  // extern "C"
