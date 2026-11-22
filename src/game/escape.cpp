// In-race pause ("escape") menu (0x8000C7CC-0x8000D7D8): resume/restart/options/quit, the
// confirmation dialog and the "missing controller" popup, drawn from esclyt.val. "escape:" in its
// debug message suggests escape.cpp; function names are guesses.
#include "game/common.h"
#include "game/sound.h"
#include "game/ilayout.h"
#include "game/textelem.h"
#include "game/gart.h"

// Camera: common.h's PlainValMatrix.
extern "C" {
void fn_80230E90(int unk);
void PlayNextSound(void);
void fn_80230B5C(PlainValMatrix* cam);
void fn_802C8638(PlainValMatrix* cam);
void SetupCameraProjection(f32 a, f32 b, f32 c);
void fn_800109E4(u32 color, f32 x, f32 y, f32 w, f32 h);
const char* fn_80131234(const char* key);
void fn_801473F8(ILayout* layout, const char* name, const char* fmt, ...);
int fn_801345D8(char* buf, int size, const char* fmt, ...);
void fn_80230F78(u8* alpha, f32* scale, u32* color, int unk);
void fn_80147940(ILayout* layout, GartElem* item, const char* set, int index);
void fn_80147230(TextElem* item, const char* fmt, ...);
void fn_801476C4(TextElem* item);
void fn_80011A2C(void);
void fn_802C9904(void);
}  // extern "C"
int fn_800075A0(int);
extern "C" {
int fn_80137CFC(int player);
int fn_802E8504(int pad);
int fn_80137C74(void);
void fn_80148070(GartElem* item, u32 color, gArtSet* set);
void fn_80148134(f32* ox, f32* oy, f32 x, f32 y, u32 unk);
void DrawFormattedText(f32 scale, u32 color, u32 flags, f32 x, f32 y, f32 w, const char* fmt, ...);
void* AllocateVolumeHandle(const char* name);
void SetVolumeClamped(void* p, f32 value);
ILayout* fn_80145E70(const char* name);
Sound2D* fn_80207848(const char* name, int flags);
void ClearAudioHandle(void** p);
f32 fn_80209028(void);
int fn_80265D64(void);
void fn_801769E0(void);
void fn_80206444(void);
void fn_80206454(void);
void fn_802E30C8(const char* fmt, ...);
void fn_801769EC(void);
int fn_80161C38(void);
}
int OptE3Demo(void);

static const char* lbl_802FC098[] = {
    "Pause:RESUME",
    "Pause:RESTART",
    "Pause:OPTIONS",
    "Pause:QUIT",
};

ModuleTag lbl_8055D3F0;
static inline u32 DefaultPauseFlags() { return 0xFFFF0000u; }
extern const u32 lbl_8055D3F8 = DefaultPauseFlags();
static UnkStatic123 lbl_8034F610;
static inline u32 DefaultPauseLimit() { return 0x7F000000u; }
extern const u32 lbl_8055D3FC = DefaultPauseLimit();

s32 lbl_8055D400;                  // selected item
u8 lbl_8055D404;                   // menu still active when left
bool lbl_8055D405;                 // confirm dialog up
bool lbl_8055D406;                 // confirm "yes" selected
bool lbl_8055D407;                 // missing controller popup up
int (*lbl_8055D408)(int item);     // item callback
void (*lbl_8055D40C)(void);        // background draw callback
void* lbl_8055D410;
ILayout* lbl_8055D414;
Sound2D* lbl_8055D418;
Sound2D* lbl_8055D41C;
s32 lbl_8055D420;  // missing controller index

static int fn_8000C7CC(void) { return OptE3Demo() == 0; }

static int fn_8000C7F4(int item) {
    if (fn_8000C7CC()) {
        return 1;
    }
    return item != 2;
}

static int fn_8000C840(int item) { return 1; }

static int fn_8000C848(int key) {
    switch (key) {
    case 0x126:
        if (!lbl_8055D406) {
            lbl_8055D406 = 1;
            fn_80230E90(1);
        }
        break;
    case 0x128:
        if (lbl_8055D406) {
            lbl_8055D406 = 0;
            fn_80230E90(1);
        }
        break;
    case 0x1B:
    case 'S':
        lbl_8055D406 = 0;
    case 'X':
    case '\r':
        if (lbl_8055D406) {
            if (fn_8000C840(lbl_8055D400)) {
                lbl_8055D404 = 0;
                if (!lbl_8055D408(lbl_8055D400)) {
                    lbl_8055D404 = 1;
                }
            }
        } else {
            lbl_8055D405 = 0;
        }
        break;
    default:
        return 0;
    }
    PlayNextSound();
    return 1;
}

static void fn_8000C964(void) {
    GartElem* art;
    const char** text;
    int i;
    f32 pulse;
    PlainValMatrix cam;
    char name[32];
    char label[32];
    int selected;
    TextElem item;
    u8 alpha;
    GartElem button;

    fn_80230B5C(&cam);
    fn_802C8638(&cam);
    SetupCameraProjection(1.0f, 1000.0f, 1.4398966f);
    fn_800109E4(0x7F000000, -428.0f, 241.0f, 855.0f, 482.0f);
    if (lbl_8055D414) {
        const char* choices[2] = {"Generic:YES", "Generic:NO"};
        fn_801473F8(lbl_8055D414, "confirm_title", "%s", fn_80131234(lbl_802FC098[lbl_8055D400]));
        fn_801473F8(lbl_8055D414, "confirm_msg", "%s", fn_80131234("Pause:CONFIRM"));
        for (i = 0, text = choices; i < 2; i++, text++) {
            fn_801345D8(name, sizeof(name), "item%d_button", i + 1);
            fn_801345D8(label, sizeof(label), "item%d", i + 1);
            item = *lbl_8055D414->Find(label);
            selected = 0;
            alpha = 0xFF;
            pulse = 0.0f;
            if ((i == 0 && lbl_8055D406) || (i == 1 && !lbl_8055D406)) {
                selected = 1;
            }
            if (selected) {
                fn_80230F78(&alpha, &pulse, &item.color, 1);
            }
            art = lbl_8055D414->FindBox(name);
            if (art) {
                if (!fn_8000C840(i)) {
                    art->color = (art->color & 0xFFFFFF) | 0x80000000;
                }
                fn_80147940(lbl_8055D414, art, "menu_artset", selected ? 1 : 2);
                if (selected) {
                    button = *art;
                    button.pos.x -= 0.5f * button.w * pulse;
                    button.pos.y -= 0.5f * button.h * pulse;
                    button.w *= 1.0f + pulse;
                    button.h *= 1.0f + pulse;
                    button.color = (button.color & 0xFF0000) | (alpha << 24) | (button.color & 0xFFFF);
                    fn_80147940(lbl_8055D414, &button, "menu_artset", 1);
                }
            }
            fn_80147230(&item, "%s", fn_80131234(*text));
        }
        fn_801476C4(lbl_8055D414->Find("cancel_prompt"));
        fn_801476C4(lbl_8055D414->Find("ok_prompt"));
    }
    fn_80011A2C();
    fn_802C9904();
}

static int fn_8000CCB8(void) {
    int i;
    for (i = 0; i < fn_80137C74(); i++) {
        if (fn_800075A0(i) && !fn_802E8504(fn_80137CFC(i))) {
            return i;
        }
    }
    return -1;
}

static void fn_8000CD24(void) {
    GartElem* box;
    TextElem* msg;
    f32 x;
    f32 y;
    if (lbl_8055D414) {
        box = lbl_8055D414->FindBox("popup_box");
        if (box) {
            fn_80148070(box, 0x96FFFFFF, lbl_8055D414->FindSet("popup_box_art"));
        }
        fn_801476C4(lbl_8055D414->Find("missing_ctrl_title"));
        msg = lbl_8055D414->Find("missing_ctrl_msg");
        if (msg) {
            const char* names[2] = {"Pause:MISSING_CTRL_0", "Pause:MISSING_CTRL_1"};
            fn_80148134(&x, &y, msg->pos.x, msg->pos.y, msg->elemJust);
            DrawFormattedText(msg->scale, msg->color, msg->just, x, y, box->w - 34.0f, "%s",
                        fn_80131234(names[lbl_8055D420]));
            fn_80011A2C();
        }
    }
}

void fn_8000CE60(void) {
    lbl_8055D404 = 0;
    lbl_8055D405 = 0;
    lbl_8055D407 = 0;
    lbl_8055D400 = 0;
    lbl_8055D408 = 0;
    lbl_8055D40C = 0;
    lbl_8055D410 = AllocateVolumeHandle("escape-menu");
    SetVolumeClamped(lbl_8055D410, 1.0f);
    lbl_8055D414 = fn_80145E70("esclyt.val");
    lbl_8055D418 = fn_80207848("pauseon.sfx", 0);
    lbl_8055D41C = fn_80207848("pauseoff.sfx", 0);
    lbl_8055D420 = -1;
}

void fn_8000CEF0(void) {
    if (lbl_8055D418) {
        delete lbl_8055D418;
        lbl_8055D418 = 0;
    }
    if (lbl_8055D41C) {
        delete lbl_8055D41C;
        lbl_8055D41C = 0;
    }
    if (lbl_8055D414) {
        delete lbl_8055D414;
        lbl_8055D414 = 0;
    }
    ClearAudioHandle(&lbl_8055D410);
    lbl_8055D404 = 0;
    lbl_8055D405 = 0;
    lbl_8055D407 = 0;
}

void fn_8000D000(int (*callback)(int), void (*draw)(void), int item) {
    lbl_8055D408 = callback;
    lbl_8055D404 = 1;
    lbl_8055D405 = 0;
    lbl_8055D406 = 0;
    lbl_8055D407 = 0;
    lbl_8055D40C = draw;
    lbl_8055D400 = item;
    lbl_8055D420 = fn_8000CCB8();
}

void fn_8000D048(int (*callback)(int), void (*draw)(void), int item) {
    fn_8000D000(callback, draw, item);
    lbl_8055D407 = 1;
}

void fn_8000D070(void) {
    if (lbl_8055D418) {
        lbl_8055D418->SetVolume(fn_80209028());
        lbl_8055D418->Play();
    }
    if (!fn_80265D64()) {
        fn_801769E0();
    }
    fn_80206444();
    lbl_8055D400 = 0;
    SetVolumeClamped(lbl_8055D410, 0.0f);
    fn_80230E90(1);
}

void fn_8000D100(void) {
    if (lbl_8055D41C) {
        lbl_8055D41C->SetVolume(fn_80209028());
        lbl_8055D41C->Play();
    }
    SetVolumeClamped(lbl_8055D410, 1.0f);
    fn_80206454();
    if (lbl_8055D404) {
        fn_802E30C8("escape: leave with still active!");
        lbl_8055D404 = 0;
    } else {
        fn_801769EC();
    }
}

void fn_8000D19C(void) {
    if (lbl_8055D407) {
        if ((lbl_8055D420 = fn_8000CCB8()) == -1) {
            lbl_8055D407 = 0;
        }
    }
}

void fn_8000D1DC(void) {
    u8 alpha;
    PlainValMatrix cam;
    const char** text;
    char name[32];
    f32 pulse;
    int i;
    bool selected;
    char label[32];
    TextElem item;
    GartElem button;
    GartElem* art;

    if (lbl_8055D405) {
        fn_8000C964();
        return;
    }
    if (lbl_8055D40C) {
        lbl_8055D40C();
    }
    fn_80230B5C(&cam);
    fn_802C8638(&cam);
    SetupCameraProjection(1.0f, 1000.0f, 1.4398966f);
    fn_800109E4(0x7F000000, -428.0f, 241.0f, 855.0f, 482.0f);
    if (lbl_8055D407) {
        fn_8000CD24();
        return;
    }
    if (lbl_8055D414) {
        fn_801473F8(lbl_8055D414, "title", "%s", fn_80131234("Pause:TITLE"));
        for (i = 0, text = lbl_802FC098; i < 4; i++, text++) {
            if (fn_8000C7F4(i)) {
                fn_801345D8(name, sizeof(name), "item%d_button", i);
                fn_801345D8(label, sizeof(label), "item%d", i);
                selected = i == lbl_8055D400;
                item = *lbl_8055D414->Find(label);
                alpha = 0xFF;
                pulse = 0.0f;
                if (selected) {
                    fn_80230F78(&alpha, &pulse, &item.color, 1);
                }
                if (!fn_8000C840(i)) {
                    item.color = (item.color & 0xFFFFFF) | 0x80000000;
                }
                art = lbl_8055D414->FindBox(name);
                if (art) {
                    if (!fn_8000C840(i)) {
                        art->color = (art->color & 0xFFFFFF) | 0x80000000;
                    }
                    fn_80147940(lbl_8055D414, art, "menu_artset", selected ? 1 : 2);
                    if (selected) {
                        button = *art;
                        button.pos.x -= 0.5f * button.w * pulse;
                        button.pos.y -= 0.5f * button.h * pulse;
                        button.w *= 1.0f + pulse;
                        button.h *= 1.0f + pulse;
                        button.color = (button.color & 0xFF0000) | (alpha << 24) | (button.color & 0xFFFF);
                        fn_80147940(lbl_8055D414, &button, "menu_artset", 1);
                    }
                }
                fn_80147230(&item, "%s", fn_80131234(*text));
            }
        }
        fn_801476C4(lbl_8055D414->Find("cancel_prompt"));
        fn_801476C4(lbl_8055D414->Find("ok_prompt"));
    }
    fn_80011A2C();
    fn_802C9904();
}

int fn_8000D540(int key) {
    if (lbl_8055D405) {
        return fn_8000C848(key);
    }
    if (lbl_8055D407) {
        return 0;
    }
    switch (key) {
    case 0x1B:
        lbl_8055D404 = 0;
        if (!lbl_8055D408(0)) {
            lbl_8055D404 = 1;
        }
        break;
    case 0x126:
        lbl_8055D400 = (lbl_8055D400 + 3) % 4;
        while (!fn_8000C7F4(lbl_8055D400) || !fn_8000C840(lbl_8055D400)) {
            lbl_8055D400 = (lbl_8055D400 + 3) % 4;
        }
        fn_80230E90(1);
        break;
    case 0x128:
        lbl_8055D400 = (lbl_8055D400 + 1) % 4;
        while (!fn_8000C7F4(lbl_8055D400) || !fn_8000C840(lbl_8055D400)) {
            lbl_8055D400 = (lbl_8055D400 + 1) % 4;
        }
        fn_80230E90(1);
        break;
    case 'S':
        lbl_8055D400 = 0;
    case 'X':
    case '\r':
        if (lbl_8055D400 == 3 && !fn_80161C38()) {
            lbl_8055D405 = 1;
            lbl_8055D406 = 0;
            break;
        }
        if (fn_8000C840(lbl_8055D400)) {
            lbl_8055D404 = 0;
            if (!lbl_8055D408(lbl_8055D400)) {
                lbl_8055D404 = 1;
            }
        }
        break;
    default:
        return 0;
    }
    PlayNextSound();
    return 1;
}

int fn_8000D75C(void) { return lbl_8055D404; }

int fn_8000D764(void) {
    if (lbl_8055D404) {
        return -1;
    }
    return fn_8000CCB8();
}
