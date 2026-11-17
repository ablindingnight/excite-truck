// Memory card status screens (0x802451DC-0x80246938): the save status layout ("mc_status.lyt"),
// the dialog and progress bar ("mc_dialog.lyt", "mem_bar.art", "progress") and the wireless warning.
// Written against raw field offsets; the large screen routines are still stubs. Names are guesses
// apart from the strings.
#include "game/common.h"
#include "game/gart.h"
#include "game/menu.h"
#include "game/menustate.h"
extern "C" {
size_t strlen(const char* s);
char* strcpy(char* d, const char* s);
char* strcat(char* d, const char* s);
}

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

extern "C" {
u8 fn_8012C33C(const char* lyt);
void fn_80214D50(int s);
void fn_80214CB4(int a);
void fn_80216D28(void);
void fn_80230E90(int a);
void fn_80214AF0(int s);
void fn_80214B44(int i);
void fn_80214A9C(int s);
s32 fn_80129AF4(void);
int fn_801298E8(void* info, s32 ch);
void fn_80214FD8(const char* title, const char* msg);
void fn_802C519C(u8* p);
void fn_802328D4(u8* p);
u8* gxGetImage(const char* name);
void fn_802E30C8(const char* fmt, ...);
u8* fn_80232578(const char* name);
void fn_802C8530(void);
void fn_802C87B4(int a, int b, s32 w, s32 h);
void fn_802C88BC(int a);
void fn_802C974C(f32 a, f32 b, f32 c);
void fn_802C5320(u8* img, int a, f32 x, f32 y, f32 z, f32 w, f32 h);
void fn_802C92D0(void);
void fn_802C92DC(int x0, int y0, int x1, int y1, u32 color);
void fn_802C92D8(void);
void fn_80232BA4(const char* text, u8* font, f32 x, f32 y, u32 color, int align, f32 scale, f32 z, f32 xs);
f32 fn_8023293C(u8* font, const char* text, f32 scale);
void fn_802335F0(const char* text, u8* font, f32 x, f32 y);
void fn_802C9A70(int a);
void fn_802C855C(void);
void fn_802C2B48(void);
extern s32 lbl_8055CFF0;
extern s32 lbl_8055CFF4;
extern s32 lbl_8055FB9C;  // screen width
extern s32 lbl_8055FBA0;  // screen height
extern f32 __float_epsilon[];  // FLT_EPSILON
void fn_802C5310(u8* img, int a, f32 x, f32 y, f32 z);
void fn_802337A0(u8* font, u32 color, int flags, const char* fmt, f32 x, f32 y, f32 z, f32 scale, f32 alpha, ...);
s32 fn_802E2C20(void);  // milliseconds
void fn_802E2D74(int ms);
u16 fn_802E46F0(void);  // key
int fn_802E47B8(void);
void fn_80129AFC(void);
double sin(double x);
}

/* Class name unproven */
// The choice dialog's focus state: accept is focused first; without a custom layout the alternative choice
// starts focused too.
class McChoiceState : public MenuState {
public:
    McChoiceState(void) {
        accept = 1;
        alt = 1;
    }
    virtual ~McChoiceState(void) {}
    virtual void Enter(void);
    virtual void Unfocus(void);  // 0x60 /* name unproven */
    virtual int Key(int key);

    u8 accept;  // offset 0x18
    u8 alt;     // offset 0x19
};

/* Helper name unproven */
static inline gArtSet* LoadProgressBarArt(void) { return fn_8023512C("mem_bar.art"); }

// The save progress bar ("mem_bar.art").
class MemBar : public MenuState {
public:
    MemBar(f32 f) {
        frac = f;
        art = LoadProgressBarArt();
    }
    virtual ~MemBar(void) {
        if (art) {
            fn_80235380(art);
        }
    }
    virtual void Draw(void);

    f32 frac;  // offset 0x18
    gArtSet* art;  // offset 0x1C
};

#include "game/mcdialog.h"

// Camera: common.h's PlainValMatrix.
extern "C" {
void fn_80230B5C(PlainValMatrix* cam);
void fn_802C8638(PlainValMatrix* cam);
}

static inline f32 AbsF(f32 v) { return (f32)__fabs(v); }

void operator delete(void* p);

extern "C" {
int fn_802450D4(void);
int fn_80245080(u8* p);
}

// The status screen's menu (copied by MemCardDialogMgr::ShowMsg). Retail's .rodata and .data put it, and its
// layout name, ahead of everything else in the unit.
/* Helper name unproven */
static inline MenuDesc MakeStatusMenu(void) {
    MenuDesc d = {"mc_status.lyt", 0, (int (*)(int*))fn_802450D4, 0, 0};
    return d;
}

// A remote's status block.
struct WpadInfo {
    u8 pad0[0x50];  // offset 0x0
    u32 flags;      // offset 0x50
    u8 pad54[0x10]; // offset 0x54
};

ModuleTag lbl_8055F248;
UnkStatic123s lbl_804988D8;

extern "C" {

}  // extern "C"

// A constructor retained by the compiler but stripped by the linker. It emits the pure dialog vtable.
/* Helper name unproven */
MemCardDialogMgr* CreateMemoryCardDialog(void) { return new MemCardDialogMgr; }

MemCardDialogMgr::~MemCardDialogMgr(void) { lbl_8055F244 = 0; }

/* Helper name unproven */
static inline void LoadDialogBackground(MemCardDialogBg* self) {
    if ((self->image = gxGetImage("*uibig.res/backsplt.img")) == 0) {
        fn_802E30C8("MemCardDialogMgr couldn't get background image backgnd1.img");
    }
    self->image2 = 0;
    self->font2 = fn_80232578("uifnt.gtx");
    self->font = fn_80232578("uifnt.gtx");
    self->fade = lbl_80563830;
}

inline void MemCardDialogBg::Open(void) { LoadDialogBackground(this); }

inline void MemCardDialogBg::Close(void) {
    if (image) {
        fn_802C519C(image);
    }
    image = 0;
    if (image2) {
        fn_802C519C(image2);
    }
    image2 = 0;
    fn_802328D4(font);
    fn_802328D4(font2);
    font = 0;
    font2 = 0;
}

// The "Accept" hint at (x, y): the A button art when loaded, else "A - Accept".
static inline void DrawAccept(MemCardDialogBg* bg, f32 x, f32 y, const char* label) {
    if (bg->image2) {
        fn_802C5310(bg->image2, 0, 4.0f + (x + 0.5f * fn_8023293C(bg->font2, label, 1.0f)), 3.0f + y, lbl_80563830);
        fn_802337A0(bg->font2, 0xFFFFFFFF, 9, "%s", x, y, lbl_80563830, 0.5f, 1.0f, label);
    } else {
        fn_802337A0(bg->font2, 0xFFFFFFFF, 0xC, "%s - Accept", x, y, lbl_80563830, 0.5f, 1.0f, "A");
    }
}

// The choice dialog without a layout: draws the message and the choices (the selected one pulses) until
// one is accepted or the check callback reports a result.
static inline int RunChoices(MemCardDialogBg* bg, const McDialogCfg* cfg) {
    int sel = cfg->select;
    f32 ax;
    f32 ay;
    char* p;
    for (;;) {
        f32 y;
        int r;
        PlainValMatrix cam;
        char text[0x200];
        fn_802C8530();
        fn_802C87B4(0, 0, lbl_8055CFF0, lbl_8055CFF4);
        fn_802C88BC(2);
        fn_80230B5C(&cam);
        fn_802C8638(&cam);
        fn_802C974C(1.0f, 1000.0f, 1.4398966f);
        f32 w = lbl_8055FB9C + 1;
        f32 h = lbl_8055FBA0 + 1;
        f32 x0 = -1.0f * lbl_8055FB9C / 2.0f - 1.0f;
        f32 y0 = 1.0f + lbl_8055FBA0 / 2.0f;
        if (bg->image) {
            fn_802C5320(bg->image, 0, x0, y0, lbl_80563830, w, h);
        } else {
            fn_802C92D0();
            fn_802C92DC(0, 0, (int)w, (int)h, 0xFF000000);
            fn_802C92D8();
        }
        const char* title = cfg->title;
        const char* msg = cfg->text;
        y = 145.0f;
        if (strlen(title)) {
            fn_80232BA4(title, bg->font, 2.0f + bg->fade, y - 2.0f, 0xFF000000, 0xC, 1.0f, lbl_80563830, 1.0f);
            fn_80232BA4(title, bg->font, bg->fade, y, 0xFFFFFFFF, 0xC, 1.0f, lbl_80563830, 1.0f);
        }
        y = y - 40.0f;
        strcpy(text, msg);
        p = text;
        while (strlen(p)) {
            char* brk = 0;
            char line[0x200] = {0};
            char* d = line;
            u32 i = 0;
            while (i < strlen(p) && fn_8023293C(bg->font, line, 1.0f) < 480.0f) {
                char* c = &p[i];
                if (*c == ' ') {
                    brk = c;
                } else if (*c == '^') {
                    *c = ' ';
                }
                i++;
                *d++ = *c;
                *d = 0;
            }
            char keep = 0;
            if (i != strlen(p)) {
                if (!brk) {
                    brk = &p[i];
                    keep = p[i];
                }
                *brk = 0;
            } else {
                brk = &p[i] - 1;
            }
            fn_80232BA4(p, bg->font, 2.0f + bg->fade, y - 2.0f, 0xFF000000, 0xC, 0.9f, lbl_80563830, 1.0f);
            fn_80232BA4(p, bg->font, bg->fade, y, 0xFFFFFFFF, 0xC, 0.9f, lbl_80563830, 1.0f);
            if (!keep) {
                p = brk + 1;
            } else {
                *brk = keep;
                p = brk;
            }
            y -= 20.0f;
        }
        y = -70.0f;
        for (int i = 0; i < cfg->count; i++) {
            f32 angle = 3.0f * (f32)fn_802E2C20() / 1000.0f;
            f32 pulse = 255.0f * (0.75f + 0.25f * (f32)sin(angle));
            fn_80232BA4(cfg->entries[i].title, bg->font, 2.0f + bg->fade, y - 2.0f, 0xFF000000, 0xC, 1.0f, lbl_80563830, 1.0f);
            u32 color;
            if (sel == i) {
                const u32 selected = ((u32)(int)pulse << 24) | 0xFFD03F;
                color = selected;
            } else {
                color = 0xC8646464;
            }
            fn_80232BA4(cfg->entries[i].title, bg->font, bg->fade, y, color, 0xC, 1.0f, lbl_80563830, 1.0f);
            y -= 20.0f;
        }
        const char* acceptLabel = "Accept";
        ay = -140.0f;
        ax = 155.0f;
        DrawAccept(bg, ax, ay, acceptLabel);
        fn_802C9A70(1);
        fn_802C855C();
        fn_802C2B48();
        fn_80129AFC();
        if (fn_802E47B8()) {
            switch (fn_802E46F0()) {
            case 0x126:
                if (sel > 0) {
                    fn_80214D50(0);
                    sel--;
                }
                break;
            case 0x128:
                if (sel + 1 < cfg->count) {
                    fn_80214D50(0);
                    sel++;
                }
                break;
            case 0xD:
                fn_80214D50(2);
                return (int)cfg->entries[sel].text;
            }
        }
        int (*check)(u8* p) = cfg->check;
        if (check) {
            r = -1;
            if (check((u8*)&r) == 1) {
                return r;
            }
        }
        fn_802E2D74(5);
    }
}

#pragma push
#pragma opt_lifetimes off  /* Pragma unproven: helps but not exact (PRAGMA_AUDIT_2026-11-15) */
inline int MemCardDialogBg::Setup(const McDialogCfg* cfg) { return RunChoices(this, cfg); }
#pragma pop

/* Helper name unproven */
static inline void ClearProgressDots(char* dots) { strcpy(dots, ""); }
/* Helper name unproven */
static inline void AppendProgressDot(char* dots) { strcat(dots, "."); }

/* Helper name unproven */
static inline void DrawDialog(MemCardDialogBg* bg, const McMsg* m, char*& p) {
    PlainValMatrix cam;
    char text[0x200];
    fn_802C8530();
    fn_802C87B4(0, 0, lbl_8055CFF0, lbl_8055CFF4);
    fn_802C88BC(2);
    fn_80230B5C(&cam);
    fn_802C8638(&cam);
    fn_802C974C(1.0f, 1000.0f, 1.4398966f);
    f32 h;
    f32 w;
    f32 x0;
    f32 y0;
    w = lbl_8055FB9C + 1;
    h = lbl_8055FBA0 + 1;
    x0 = -1.0f * lbl_8055FB9C / 2.0f - 1.0f;
    y0 = 1.0f + lbl_8055FBA0 / 2.0f;
    if (bg->image) {
        fn_802C5320(bg->image, 0, x0, y0, lbl_80563830, w, h);
    } else {
        fn_802C92D0();
        fn_802C92DC(0, 0, (int)w, (int)h, 0xFF000000);
        fn_802C92D8();
    }
    const char* title = m->title;
    const char* msg = m->text;
    f32 y = 145.0f;
    if (strlen(title)) {
        fn_80232BA4(title, bg->font, 2.0f + bg->fade, y - 2.0f, 0xFF000000, 0xC, 1.0f, lbl_80563830, 1.0f);
        fn_80232BA4(title, bg->font, bg->fade, y, 0xFFFFFFFF, 0xC, 1.0f, lbl_80563830, 1.0f);
    }
    y -= 40.0f;
    strcpy(text, msg);
    p = text;
    while (strlen(p)) {
        struct { char* brk; } wrap = { 0 };
        char line[0x200] = {0};
        char* d = line;
        u32 i = 0;
        while (i < strlen(p) && fn_8023293C(bg->font, line, 1.0f) < 480.0f) {
            char* c = &p[i];
            if (*c == ' ') {
                wrap.brk = c;
            } else if (*c == '^') {
                *c = ' ';
            }
            i++;
            *d++ = *c;
            *d = 0;
        }
        char keep = 0;
        if (i != strlen(p)) {
            if (!wrap.brk) {
                wrap.brk = &p[i];
                keep = p[i];
            }
            *wrap.brk = 0;
        } else {
            wrap.brk = &p[(int)i - 1];
        }
        fn_80232BA4(p, bg->font, 2.0f + bg->fade, y - 2.0f, 0xFF000000, 0xC, 0.9f, lbl_80563830, 1.0f);
        fn_80232BA4(p, bg->font, bg->fade, y, 0xFFFFFFFF, 0xC, 0.9f, lbl_80563830, 1.0f);
        if (!keep) {
            p = wrap.brk + 1;
        } else {
            *wrap.brk = keep;
            p = wrap.brk;
        }
        y -= 20.0f;
    }
}

// Draws the plain dialog and its progress dots.
inline void MemCardDialogBg::ShowMsg(const McMsg* m) {
    char* p;
    DrawDialog(this, m, p);
    char dots[0x40];
    f32 frac = m->frac;
    if (!(AbsF(frac) <= __float_epsilon[0])) {
        ClearProgressDots(dots);
        for (int n = 0; n < (int)(30.0f * frac); n++) {
            AppendProgressDot(dots);
        }
        fn_802335F0(dots, this->font2, -270.0f, -138.0f);
    }
    fn_802C9A70(1);
    fn_802C855C();
    fn_802C2B48();
}

/* Helper name unproven */
static inline void OpenDialog(MemCardDialogMgr* self) {
    self->open = 1;
    self->layout = fn_8012C33C("mc_dialog.lyt");
    if (!self->layout) {
        self->bg.Open();
    }
    self->bgShown = 1;
}

inline void MemCardDialogMgr::Open(void) { OpenDialog(this); }

inline void MemCardDialogMgr::Close(void) {
    open = 0;
    if (!layout) {
        bg.Close();
    }
    fn_80214D50(4);
}

extern UnkStatic123s lbl_80498890;
extern "C" void fn_80216CA8(void);
extern const char lbl_8055BAC0[8];  // ""
extern "C" inline void fn_80246210(void);

// Runs a memory card choice dialog (up to four options) on the layout, or on the plain background.
/* Helper name unproven */
static inline const char* FirstDialogItem(void) { return "__first"; }
/* Helper name unproven */
static inline const char* DialogTitle(void) { return "#title"; }
/* Helper name unproven */
static inline const char* DialogMessage(void) { return "#msg"; }

/* Helper name unproven */
static inline const char* DefaultDialogLayout(void) { return "mc_dialog.lyt"; }
/* Helper name unproven */
static inline const char* DialogOption1(void) { return "#option_1"; }
/* Helper name unproven */
static inline const char* DialogOption2(void) { return "#option_2"; }
/* Helper name unproven */
static inline const char* DialogOption3(void) { return "#option_3"; }
/* Helper name unproven */
static inline const char* DialogOption4(void) { return "#option_4"; }

// cfg is a pointer to const (retail loads through it float above the stores to this->cfg and the choice copies).
inline int MemCardDialogMgr::Setup(const McDialogCfg* cfg) {
    if (!layout) {
        return bg.Setup(cfg);
    }
    this->cfg = cfg;
    McChoice choices[4];
    s32 i;
    for (i = 0; i < cfg->count; i++) {
        choices[i] = cfg->entries[i];
    }
    for (; i < 4; i++) {
        choices[i].title = 0;
        choices[i].text = 0;
    }
    McChoiceState state;
    if (cfg->layout) {
        state.alt = 0;
    }
    MenuItem items[] = {
        MenuItem(FirstDialogItem(), 5, &state, 0, 0),
        MenuItem(lbl_8055BAC0, 8, (const void*)fn_80246210, 0, 0),
        MenuItem(DialogTitle(), cfg->title, CaptionContext(strlen(cfg->title))),
        MenuItem(DialogMessage(), cfg->text, CaptionContext(strlen(cfg->text))),
        MenuItem(DialogOption1(), 0, 0, (int)choices[0].text, 0),
        MenuItem(DialogOption1(), choices[0].title),
        MenuItem(DialogOption2(), 0, 0, (int)choices[1].text, 0),
        MenuItem(DialogOption2(), choices[1].title),
        MenuItem(DialogOption3(), 0, 0, (int)choices[2].text, 0),
        MenuItem(DialogOption3(), choices[2].title),
        MenuItem(DialogOption4(), 0, 0, (int)choices[3].text, 0),
        MenuItem(DialogOption4(), choices[3].title),
        MenuItem(lbl_80498890),
    };
    MenuDesc desc = {0, 0, (int (*)(int*))fn_80245080, 0, 0};
    desc.layout = cfg->layout ? cfg->layout : DefaultDialogLayout();
    desc.items = items;
    fn_80216CA8();
    if (cfg->layout) {
        fn_80214D50(5);
    }
    int result = fn_802146BC(&desc, -999, -999, 1);
    this->cfg = 0;
    return result;
}

extern "C" {

inline void fn_80246210(void) {
    lbl_8055F244->bgShown = 0;
    s32 n = lbl_8055F244->cfg->count;
    for (int i = 0; i < n; i++) {
        fn_80214AF0(i);
    }
    for (; n < 4; n++) {
        fn_80214B44(n);
        fn_80214A9C(n);
    }
    fn_80214CB4(lbl_8055F244->cfg->select);
}

}  // extern "C"

extern const MenuDesc lbl_802FA350;  // "mc_dialog.lyt"
extern UnkStatic123s lbl_80498890;  // menu end tag (menusel.cpp)

extern "C" {
void fn_80216CA8(void);
int fn_802450D4(void);
}

// Shows a memory card message with its progress bar (on the plain background without the layout).
/* Helper name unproven */
static inline const char* DialogProgressName(void) { return "progress"; }

inline void MemCardDialogMgr::ShowMsg(const McMsg* m) {
    if (!layout) {
        bg.ShowMsg(m);
        return;
    }
    MemBar bar(m->frac);
    MenuItem items[] = {
        MenuItem(DialogTitle(), m->title, CaptionContext(strlen(m->title))),
        MenuItem(DialogMessage(), m->text, CaptionContext(strlen(m->text))),
        MenuItem(DialogProgressName(), 5, &bar, 0, 0),
        MenuItem(lbl_80498890),
    };
    retries = 0;
    MenuDesc desc = MakeStatusMenu();
    desc.items = items;
    fn_80216CA8();
    fn_802146BC(&desc, -999, -999, 1);
}

inline void McChoiceState::Enter(void) { fn_80214CB4(accept == 0); }

inline void McChoiceState::Unfocus(void) { alt = 0; }

inline int McChoiceState::Key(int key) {
    switch (key) {
    case 0x125:
        if (!alt) {
            fn_80214CB4(0);
            fn_80230E90(0);
        } else {
            fn_80216D28();
        }
        break;
    case 0x127:
        if (!alt) {
            fn_80214CB4(1);
            fn_80230E90(0);
        } else {
            fn_80216D28();
        }
        break;
    case 0x126:
        if (alt) {
            fn_80214CB4(0);
            fn_80230E90(0);
        } else {
            fn_80216D28();
        }
        break;
    case 0x128:
        if (alt) {
            fn_80214CB4(1);
            fn_80230E90(0);
        } else {
            fn_80216D28();
        }
        break;
    default:
        return 0;
    }
    return 1;
}

extern "C" {
void fn_80230C30(f32* x, f32* y, s32 lx, s32 ly);
void fn_80230924(u32 color, f32 x0, f32 y0, f32 x1, f32 y1, f32 z);
}

// Left cap, stretched middle and right cap, the filled part drawn over them; a plain bar without art.
inline void MemBar::Draw(void) {
    f32 sx;
    f32 sy;
    fn_80230C30(&sx, &sy, x, y);
    f32 fw = (f32)w;
    f32 fh = (f32)h;
    if (art) {
        f32 lw = fn_80235EF0(art, 0);
        f32 rw = fn_80235EF0(art, 2);
        f32 mid = fw - lw - rw;
        f32 total = rw + (lw + mid);
        f32 ah = fn_80235F64(art, 0);
        sx = (sx + 0.5f * fw) - 0.5f * total;
        sy = (sy + 0.5f * fh) - 0.5f * ah;
        f32 fill = total * frac;
        if (fill < lw) {
            fn_80235758(art, 3, sx, sy, fill, ah, lbl_80563830, lbl_80563830, fill / lw, 1.0f, -1);
        } else if (fill < lw + mid) {
            fn_802353F4(art, 3, sx, sy, lbl_80563830);
            fn_80235758(art, 4, sx + lw, sy, fill - lw, ah, lbl_80563830, lbl_80563830, (fill - lw) / mid, 1.0f, -1);
        } else {
            fn_802353F4(art, 3, sx, sy, lbl_80563830);
            fn_802354A0(art, 4, sx + lw, sy, lbl_80563830, mid, ah, -1);
            fn_80235758(art, 5, mid + (sx + lw), sy, (fill - lw) - mid, ah, lbl_80563830, lbl_80563830, ((fill - lw) - mid) / rw, 1.0f,
                        -1);
        }
        fn_802353F4(art, 0, sx, sy, lbl_80563830);
        fn_802354A0(art, 1, sx + lw, sy, lbl_80563830, mid, ah, -1);
        fn_802353F4(art, 2, mid + (sx + lw), sy, lbl_80563830);
    } else {
        fn_80230924(-1, sx, sy, sx + fw * frac, sy - fh, lbl_80563830);
    }
}

extern "C" {

inline void fn_8024683C(void) {
    WpadInfo all;
    WpadInfo one;
    s32 ch = fn_80129AF4();
    int warn = 0;
    if (ch == -1) {
        for (int i = 0; i < 4; i++) {
            if (fn_801298E8(&all, i) && (all.flags & 1)) {
                warn = 1;
            }
        }
    } else {
        if (fn_801298E8(&one, ch) && (one.flags & 1)) {
            warn = 1;
        }
    }
    if (warn) {
        const char* message = "Any information you enter might be picked up by another wireless receiver";
        const char* title = "Wireless Warning";
        fn_80214FD8(title, message);
    }
}

typedef void (*RemoteWarningCallback)(void);
// This reference emits the out-of-line copy of the inline warning for nameent.cpp.
/* Helper name unproven */
RemoteWarningCallback GetRemoteWarningCallback(void) { return fn_8024683C; }


}  // extern "C"

const f32 lbl_80563830 = 0.0f;  // the dialog fade start (declared in mcdialog.h; menusel reads it too)
