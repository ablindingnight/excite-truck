// Menu window (0x80216D14-0x80218E98): a window holds up to 256 widgets, its background (".img" or
// the plain "dialog" frame), 31 text styles, the pointer press/release/move routing with hover,
// press and keyboard focus, the menu cursor over registered menu items (next/previous/spatial
// moves), widget groups that fade or disable together, and drawing the widgets with their styles.
// Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
char* strstr(const char* s, const char* t);
int strcasecmp(const char* a, const char* b);
void qsort(void* base, u32 n, u32 size, int (*cmp)(const void*, const void*));
int abs(int x);
}

// A text style (0x3C): copied memberwise, so the floats go through lfs/stfs.
struct StyleBody {
    u32 w[12];
};

struct Style {
    Style(void) {}
    StyleBody body;  // offset 0x0
    f32 f30;         // offset 0x30
    f32 f34;         // offset 0x34
    f32 f38;         // offset 0x38
};

class Window;

class Widget {
public:
    virtual ~Widget(void);
    virtual void Open(void);                // 0x0C
    virtual void Close(void);               // 0x10
    virtual void Attach(void);              // 0x14
    virtual int Id(void);                   // 0x18
    virtual void Unk1C(void);
    virtual void Group(int a, int b);       // 0x20
    virtual void Unk24(void);
    virtual void Draw(void);                // 0x28
    virtual void DrawText(void);            // 0x2C
    virtual void Unk30(void);
    virtual void Focus(void);               // 0x34
    virtual void Blur(void);                // 0x38
    virtual void PressA(s32 x, s32 y);      // 0x3C
    virtual void ReleaseA(s32 x, s32 y);    // 0x40
    virtual void PressB(s32 x, s32 y);      // 0x44
    virtual void ReleaseB(s32 x, s32 y);    // 0x48
    virtual void Move(s32 x, s32 y);        // 0x4C
    virtual void Unk50(void);
    virtual void Unk54(void);
    virtual void Hover(void);               // 0x58
    virtual void Unhover(void);             // 0x5C
    virtual int Cursor(void);               // 0x60
    virtual int Key(u32 key);               // 0x64
    virtual int Focusable(void);            // 0x68
    virtual void Unk6C(void);
    virtual int Hit(s32 x, s32 y);          // 0x70
    virtual void Unk74(void);
    virtual s32 Height(void);               // 0x78
    virtual void Tick(void);                // 0x7C
    virtual void Tick2(void);               // 0x80

    s32 x;          // offset 0x4
    s32 y;          // offset 0x8
    s32 fC;         // offset 0xC
    s32 bottom;     // offset 0x10
    u8 pad14[0x1A - 0x14];
    u8 shown;       // offset 0x1A
    u8 enabled;     // offset 0x1B
    f32 fadeIn;     // offset 0x1C
    f32 fadeOut;    // offset 0x20
    u8 pad24[0x2C - 0x24];
    Window* window; // offset 0x2C
    u8 pad30[0x234 - 0x30];
    u8 dirty;       // offset 0x234
    u32 mask;       // offset 0x238
    s32 style;      // offset 0x23C
    s32* stylePtr;  // offset 0x240
};

struct MenuEntry {
    s32 id;     // offset 0x0
    Widget* w;  // offset 0x4
};

class Window {
public:
    Window(const char* bg, s32 x, s32 y, s32 w, s32 h, s32 f490);
    ~Window(void);

    void* f0;                // offset 0x0
    void* f4;                // offset 0x4
    void* image;             // offset 0x8
    u8 fC[0x30 - 0xC];       // offset 0xC
    Widget* widgets[256];    // offset 0x30
    s32 count;               // offset 0x430
    void (*update)(Window* w);  // offset 0x434
    void (*open)(void);      // offset 0x438
    void (*close)(void);     // offset 0x43C
    void (*fade)(int phase, void* fade, f32 t);  // offset 0x440
    void (*fadeStyle)(Style* s, u32 mask, s32 x, s32 y, s32 h, s32 dh, int a, f32 t);  // offset 0x444
    Widget* hover;           // offset 0x448
    Widget* pressed;         // offset 0x44C
    Widget* focus;           // offset 0x450
    s32 x;                   // offset 0x454
    s32 y;                   // offset 0x458
    s32 w;                   // offset 0x45C
    s32 h;                   // offset 0x460
    u32 hiddenGroups;        // offset 0x464
    u32 disabledGroups;      // offset 0x468
    u8 b46C;                 // offset 0x46C
    u8 b46D[0x20];           // offset 0x46D
    u8 pad48D[0x490 - 0x48D];
    s32 f490;                // offset 0x490
    s32 menuCount;           // offset 0x494
    s32 menuCur;             // offset 0x498
    u8 pad49C[0x4A0 - 0x49C];
    MenuEntry menu[64];      // offset 0x4A0
    Style styles[31];        // offset 0x6A0
    u8 noSound;              // offset 0xDE4
};

extern "C" {
u32 fn_80216D14(int i);
void fn_80216D28(void);
void fn_80216D34(void);
void fn_802C2CDC(void* p, int a, int b);
void fn_802C2D94(void* p);
int fn_8012C33C(const char* name);
void* fn_802C5130(const char* name);
void fn_802E30C8(const char* fmt, ...);
const Style* fn_8022EE9C(void);
void fn_8022EE98(void);
void fn_802C4C80(void* p);
void fn_802C2FC4(void* p);
void fn_802C519C(void* p);
void fn_802368B4(s32 style);
int fn_80218214(Window* w, s32 x, s32 y);
void fn_80218198(Window* w, Widget* wd);
void fn_80218E08(Window* w, int key);
int fn_802182F0(Window* w, Widget* wd);
void fn_80217C4C(Window* w);
void fn_80217D84(Window* w);
void fn_80217EC0(Window* w, int horizontal, int negative);
void fn_80230C30(f32* x, f32* y, int a, int b);
void fn_802C5310(void* p, int a, f32 x, f32 y, f32 z);
int fn_80215228(void);
void fn_802C86E8(u32 color);
void fn_802C88BC(int a);
void fn_8022EDB8(void);
void fn_8022ED00(const Style* s);
const Style* fn_8022EDC4(void);
void fn_802193C8(Widget* w, f32 t);
void fn_80219358(Widget* w, f32 t);
void fn_80214D50(int sound);
}

ModuleTag lbl_8055EFA0;
static UnkStatic123 lbl_80490908;
UnkStatic7F lbl_8055EFA8;
u8 lbl_8055EFAC;  // keys muted

static inline BOOL Visible(const Widget* w) {
    BOOL r = FALSE;
    if (w->shown || w->fadeOut >= 0.0f) {
        r = TRUE;
    }
    return r;
}

static inline s32 StyleOf(const Widget* w) { return w->stylePtr ? *w->stylePtr : w->style; }

static inline f32 Alpha(const Widget* w) {
    if (w->fadeOut >= 0.0f) {
        return 1.0f - w->fadeOut;
    }
    if (w->fadeIn >= 0.0f) {
        return w->fadeIn;
    }
    if (w->shown) {
        return 1.0f;
    }
    return 0.0f;
}

static inline int Sign(int negative) { return negative ? -1 : 1; }

static inline int Max0(int x) { return x < 0 ? 0 : x; }

extern "C" {

u32 fn_80216D14(int i) { return i < 0 ? 0 : 1 << i; }

void fn_80216D28(void) { lbl_8055EFAC = 1; }

void fn_80216D34(void) { lbl_8055EFAC = 0; }
}

Window::Window(const char* bg, s32 x, s32 y, s32 w, s32 h, s32 f) {
    const Style* s;
    int i;
    this->x = x;
    f490 = f;
    this->y = y;
    this->w = w;
    this->h = h;
    b46C = 1;
    count = 0;
    update = 0;
    open = 0;
    close = 0;
    fade = 0;
    fadeStyle = 0;
    hover = 0;
    pressed = 0;
    focus = 0;
    disabledGroups = 0;
    hiddenGroups = 0;
    fn_802C2CDC(fC, 4, 4);
    noSound = 0;
    f0 = 0;
    f4 = 0;
    image = 0;
    if (bg && *bg) {
        if (strstr(bg, ".img")) {
            if (fn_8012C33C(bg)) {
                image = fn_802C5130(bg);
            } else {
                fn_802E30C8("UI: missing background %s", bg);
            }
        } else if (strcasecmp(bg, "dialog") == 0) {
            noSound = 1;
        }
    }
    for (i = 0; i < 0x20; i++) {
        b46D[i] = 0;
    }
    menuCount = 0;
    menuCur = -1;
    s = fn_8022EE9C();
    for (i = 0; i < 31; i++) {
        styles[i] = *s;
    }
    fn_8022EE98();
}

Window::~Window(void) {
    int i;
    for (i = count - 1; i >= 0; i--) {
        delete widgets[i];
    }
    fn_802C2D94(fC);
    if (f0) {
        fn_802C4C80(f0);
    }
    if (f4) {
        fn_802C2FC4(f4);
    }
    if (image) {
        fn_802C519C(image);
    }
}

extern "C" {

void fn_8021734C(Window* win) {
    int i;
    for (i = 0; i < win->count; i++) {
        win->widgets[i]->Open();
        if (win->fade) {
            fn_802368B4(StyleOf(win->widgets[i]));
        }
    }
    if (win->open) {
        win->open();
    }
}

void fn_802173F8(Window* win) {
    int i;
    if (win->close) {
        win->close();
    }
    for (i = win->count - 1; i >= 0; i--) {
        win->widgets[i]->Close();
    }
    fn_80216D28();
}

void fn_80217478(Window* win) {}

void fn_8021747C(Window* win, int phase, void* fade, f32 t) {
    if (win->fade) {
        win->fade(phase, fade, t);
    }
}

void fn_8021749C(Window* win, s32 x, s32 y) {
    if (!win->pressed) {
        s32 lx = x - win->x;
        s32 ly = y - win->y;
        int i = fn_80218214(win, lx, ly);
        Widget* wd = 0;
        if (i >= 0) {
            wd = win->widgets[i];
        }
        win->pressed = wd;
        if (wd && wd->Focusable()) {
            if (win->focus != wd) {
                fn_80218198(win, wd);
            }
        } else {
            fn_80218198(win, 0);
        }
        if (wd) {
            wd->PressA(lx, ly);
        }
    }
}

void fn_80217590(Window* win, s32 x, s32 y) {
    Widget* wd = win->pressed;
    x -= win->x;
    y -= win->y;
    if (wd) {
        wd->ReleaseA(x, y);
    }
    win->pressed = 0;
}

void fn_802175F0(Window* win, s32 x, s32 y) {
    if (!win->pressed) {
        s32 lx = x - win->x;
        s32 ly = y - win->y;
        int i = fn_80218214(win, lx, ly);
        Widget* wd = 0;
        if (i >= 0) {
            wd = win->widgets[i];
        }
        win->pressed = wd;
        if (wd && win->focus != wd && wd->Focusable()) {
            fn_80218198(win, wd);
        }
        if (wd) {
            wd->PressB(lx, ly);
        }
    }
}

void fn_802176D4(Window* win, s32 x, s32 y) {
    Widget* wd = win->pressed;
    x -= win->x;
    y -= win->y;
    if (wd) {
        wd->ReleaseB(x, y);
    }
    win->pressed = 0;
}

void fn_80217734(Window* win, s32 x, s32 y) {
    s32 lx = x - win->x;
    s32 ly = y - win->y;
    int i = fn_80218214(win, lx, ly);
    Widget* wd = 0;
    if (i >= 0) {
        wd = win->widgets[i];
    }
    if (win->pressed && wd != win->pressed) {
        wd = 0;
    }
    if (wd != win->hover) {
        if (win->hover) {
            win->hover->Unhover();
        }
        win->hover = wd;
        if (wd) {
            wd->Hover();
        }
    }
    if (!wd) {
        wd = win->pressed;
    }
    if (wd) {
        wd->Move(lx, ly);
    }
}

void fn_80217834(Window* win) {
    if (win->hover) {
        win->hover->Unk50();
    }
}

void fn_80217854(Window* win) {
    if (win->hover) {
        win->hover->Unk54();
    }
}

void fn_80217874(Window* win, u32 key) {
    int i;
    fn_80216D34();
    if (!win->pressed) {
        if (win->focus && win->focus->Key(key)) {
            fn_80218E08(win, key);
            return;
        }
        for (i = 0; i < win->count; i++) {
            Widget* wd = win->widgets[i];
            if (wd != win->focus && Visible(wd) && wd->enabled && wd->Key(key)) {
                fn_80218E08(win, key);
                return;
            }
        }
    }
}

void fn_802179A0(Window* win) {
    int i = win->focus ? fn_802182F0(win, win->focus) : 0;
    int n;
    if (i >= 0) {
        for (n = 0; n < win->count - 1; n++) {
            Widget* wd;
            i++;
            wd = win->widgets[i % win->count];
            if (Visible(wd) && wd->Focusable() && wd->enabled) {
                fn_80218198(win, wd);
                return;
            }
        }
    } else {
        fn_802E30C8("Couldn't find widget");
    }
}

void fn_80217AC0(Window* win) {
    int i = win->focus ? fn_802182F0(win, win->focus) : 0;
    int n;
    for (n = 0; n < win->count - 1; n++) {
        Widget* wd;
        i--;
        wd = win->widgets[(i + win->count) % win->count];
        if (Visible(wd) && wd->Focusable() && wd->enabled) {
            fn_80218198(win, wd);
            return;
        }
    }
}

int fn_80217BC8(const void* a, const void* b) { return *(const s32*)a - *(const s32*)b; }

s32* fn_80217BD8(Window* win, s32 id, Widget* wd) {
    win->menu[win->menuCount].id = id;
    win->menu[win->menuCount].w = wd;
    win->menuCount++;
    qsort(win->menu, win->menuCount, sizeof(MenuEntry), fn_80217BC8);
    win->menuCur = win->menu[0].id;
    return &win->menuCur;
}

void fn_80217C4C(Window* win) {
    int n = win->menuCount;
    int i;
    int cur;
    int vis;
    int next;
    if (n < 1) {
        return;
    }
    cur = -1;
    vis = 0;
    for (i = 0; i < n; i++) {
        Widget* wd;
        if (win->menuCur == win->menu[i].id) {
            cur = i;
        }
        wd = win->menu[i].w;
        if (Visible(wd) && wd->enabled) {
            vis++;
        }
    }
    next = -1;
    for (i = 1; i < n; i++) {
        int idx = (cur + i) % n;
        Widget* wd = win->menu[idx].w;
        if (Visible(wd) && wd->enabled) {
            next = idx;
            break;
        }
    }
    if (next < 0) {
        return;
    }
    if (next <= cur && vis <= 3) {
        return;
    }
    win->menuCur = win->menu[next].id;
}

void fn_80217D84(Window* win) {
    int n = win->menuCount;
    int cur;
    int vis;
    int i;
    int next;
    if (n < 1) {
        return;
    }
    cur = -1;
    vis = 0;
    i = n;
    do {
        Widget* wd;
        if (win->menuCur == win->menu[i].id) {
            cur = i;
        }
        wd = win->menu[i].w;
        if (Visible(wd) && wd->enabled) {
            vis++;
        }
    } while (i-- > 0);
    next = -1;
    for (i = 1; i < n; i++) {
        int idx = (cur + n - i) % n;
        Widget* wd = win->menu[idx].w;
        if (Visible(wd) && wd->enabled) {
            next = idx;
            break;
        }
    }
    if (next < 0) {
        return;
    }
    if (next >= cur && vis <= 3) {
        return;
    }
    win->menuCur = win->menu[next].id;
}

void fn_80217EC0(Window* win, int horizontal, int negative) {
    int cur;
    int n = win->menuCount;
    int best;
    int bestScore;
    int i;
    Widget* cw;
    s32 cx;
    s32 cy;
    if (n < 1) {
        return;
    }
    cur = -1;
    for (i = 0; i < n; i++) {
        if (win->menuCur == win->menu[i].id) {
            cur = i;
        }
    }
    cw = win->menu[cur].w;
    best = cur;
    cx = cw->x;
    cy = cw->y;
    bestScore = 100000;
    for (i = 0; i < n; i++) {
        Widget* wd;
        s32 dx;
        s32 dy;
        int score;
        if (i == cur) {
            continue;
        }
        wd = win->menu[i].w;
        if (!Visible(wd) || !wd->enabled) {
            continue;
        }
        dx = cx - wd->x;
        dy = cy - wd->y;
        if (horizontal) {
            if (dx * Sign(negative) <= 10) {
                continue;
            }
        } else {
            if (dy * Sign(negative) <= 10) {
                continue;
            }
        }
        if (horizontal) {
            score = dx * Sign(negative) + Max0(abs(dy) - 10) * 300;
        } else {
            score = dy * Sign(negative) + Max0(abs(dx) - 10) * 300;
        }
        if (score < bestScore) {
            best = i;
            bestScore = score;
        }
    }
    if (best != -1) {
        win->menuCur = win->menu[best].id;
    }
}

void fn_802180B0(Window* win) { fn_80217EC0(win, 1, 0); }

void fn_802180BC(Window* win) { fn_80217EC0(win, 1, 1); }

void fn_802180C8(Window* win) { fn_80217EC0(win, 0, 0); }

void fn_802180D4(Window* win) { fn_80217EC0(win, 0, 1); }

void fn_802180E0(Window* win, int i) {
    if (i < 0 || i >= win->menuCount) {
        fn_802E30C8("SetMenuIndex(%d) out of range (%d)", i, win->menuCount);
        return;
    }
    win->menuCur = win->menu[i].id;
}

int fn_80218120(Window* win) {
    int i;
    for (i = 0; i < win->menuCount; i++) {
        if (win->menuCur == win->menu[i].id) {
            return i;
        }
    }
    fn_802E30C8("GetMenuIndex fails");
    return 0;
}

void fn_8021818C(Window* win) { win->menuCur = -1; }

void fn_80218198(Window* win, Widget* wd) {
    if (wd != win->focus) {
        if (win->focus) {
            win->focus->Blur();
        }
        win->focus = wd;
        if (wd) {
            wd->Focus();
        }
    }
}

int fn_80218214(Window* win, s32 x, s32 y) {
    int i;
    for (i = win->count - 1; i >= 0; i--) {
        Widget* wd = win->widgets[i];
        if (wd->enabled && Visible(wd) && wd->Hit(x, y)) {
            return i;
        }
    }
    return -1;
}

int fn_802182F0(Window* win, Widget* wd) {
    int i;
    for (i = 0; i < win->count; i++) {
        if (win->widgets[i] == wd) {
            return i;
        }
    }
    return -1;
}

int fn_8021832C(Window* win) {
    if (win->pressed) {
        return win->pressed->Cursor();
    }
    if (win->hover) {
        return win->hover->Cursor();
    }
    return 0;
}

void fn_80218370(Window* win) {
    int i;
    Widget* wd;
    BOOL found;
    for (i = 0; i < win->count; i++) {
        win->widgets[i]->Tick();
    }
    found = 0;
    for (i = 0; i < win->menuCount; i++) {
        if (win->menuCur == win->menu[i].id) {
            wd = win->menu[i].w;
            if (wd->enabled && Visible(wd)) {
                found = 1;
                break;
            }
        }
    }
    if (!found) {
        fn_80217D84(win);
    }
}

void fn_80218464(Window* win) {
    int i;
    for (i = 0; i < win->count; i++) {
        win->widgets[i]->Tick2();
    }
}

void fn_802184D0(Window* win) {
    int i;
    for (i = 0; i < win->count; i++) {
        if (Visible(win->widgets[i])) {
            win->widgets[i]->DrawText();
        }
    }
}

void fn_80218574(Window* win) {
    int style;
    int i;
    if (win->image) {
        f32 x;
        f32 y;
        fn_80230C30(&x, &y, 0, 0);
        fn_802C5310(win->image, 0, x, y, 0.0f);
    } else if (fn_80215228() && !win->noSound) {
        fn_802C86E8(0xFF000000);
        fn_802C88BC(3);
    }
    style = -1;
    for (i = 0; i < win->count; i++) {
        Widget* wd = win->widgets[i];
        int s;
        BOOL pushed;
        if (!Visible(wd)) {
            continue;
        }
        s = StyleOf(wd);
        if (s != style) {
            if (style != -1) {
                fn_8022EDB8();
            }
            style = s;
            fn_8022ED00(&win->styles[s]);
        }
        f32 al = Alpha(wd);
        pushed = 0;
        if (al < 0.9f && win->fadeStyle) {
            Style tmp;
            f32 a;
            pushed = 1;
            tmp = *fn_8022EDC4();
            a = Alpha(wd);
            {
                s32 x;
                u32 mask;
                s32 y;
                y = wd->y;
                x = wd->x;
                mask = wd->mask;
                win->fadeStyle(&tmp, mask, x, y, wd->Height(), wd->bottom - y, 0, a);
            }
            fn_8022ED00(&tmp);
        }
        wd->Draw();
        if (pushed) {
            fn_8022EDB8();
        }
    }
    if (style != -1) {
        fn_8022EDB8();
    }
}

int fn_8021885C(Window* win, int g) { return (win->hiddenGroups & fn_80216D14(g)) == 0; }

void fn_8021889C(Window* win, int g, f32 t) {
    if (!(win->hiddenGroups & fn_80216D14(g))) {
        int n;
        int i;
        win->hiddenGroups |= fn_80216D14(g);
        n = 0;
        for (i = 0; i < win->count; i++) {
            if (win->hiddenGroups & win->widgets[i]->mask) {
                fn_802193C8(win->widgets[i], t);
                n++;
            }
        }
        if (n > 8) {
            win->b46C = 1;
        }
    }
}

void fn_80218968(Window* win, int g, f32 t) {
    if (win->hiddenGroups & fn_80216D14(g)) {
        int n = 0;
        int i;
        win->hiddenGroups &= ~fn_80216D14(g);
        for (i = 0; i < win->count; i++) {
            if (!(win->hiddenGroups & win->widgets[i]->mask)) {
                fn_80219358(win->widgets[i], t);
                n++;
            }
        }
        if (n >= 8) {
            win->b46C = 1;
        }
    }
}

void fn_80218A34(Window* win, int g) {
    if (win->disabledGroups & fn_80216D14(g)) {
        Widget* wd;
        int i;
        win->disabledGroups &= ~fn_80216D14(g);
        for (i = 0; i < win->count; i++) {
            wd = win->widgets[i];
            if (!(win->disabledGroups & wd->mask)) {
                wd->enabled = 1;
                if (wd->shown) {
                    wd->dirty = 1;
                }
            }
        }
    }
}

void fn_80218ADC(Window* win, int g) {
    if (!(win->disabledGroups & fn_80216D14(g))) {
        Widget* wd;
        int i;
        win->disabledGroups |= fn_80216D14(g);
        for (i = 0; i < win->count; i++) {
            wd = win->widgets[i];
            if (win->disabledGroups & wd->mask) {
                wd->enabled = 0;
                if (wd->shown) {
                    wd->dirty = 1;
                }
            }
        }
    }
}

void fn_80218B88(Window* win, int g, int a, int b) {
    u32 m = fn_80216D14(g);
    int i;
    for (i = 0; i < win->count; i++) {
        if (m & win->widgets[i]->mask) {
            win->widgets[i]->Group(a, b);
        }
    }
}

void fn_80218C14(Window* win, int i, const Style* s) { win->styles[i] = *s; }

void fn_80218C98(Window* win, Widget* wd, int g) {
    win->widgets[win->count++] = wd;
    if (wd->Focusable() && !win->focus && win->noSound) {
        fn_80218198(win, wd);
    }
    wd->mask = fn_80216D14(g);
    wd->window = win;
    wd->Attach();
}

void fn_80218D54(Window* win, int id) {
    int i;
    for (i = 0; i < win->count; i++) {
        if (win->widgets[i]->Focusable() && id == win->widgets[i]->Id()) {
            fn_80218198(win, win->widgets[i]);
            return;
        }
    }
}

void fn_80218E08(Window* win, int key) {
    if (lbl_8055EFAC) {
        return;
    }
    if (key == 0xD || (u32)key == 0x113) {
        fn_80214D50(2);
    } else if ((u32)key == 0x1B) {
        fn_80214D50(3);
    } else {
        fn_80214D50(0);
    }
}
}
