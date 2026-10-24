// Memory card status screens (0x802451DC-0x80246938): the save status layout ("mc_status.lyt"),
// the dialog and progress bar ("mc_dialog.lyt", "mem_bar.art", "progress") and the wireless warning.
// Written against raw field offsets; the large screen routines are still stubs. Names are guesses
// apart from the strings.
#include "game/common.h"
#include "game/mcdialog.h"
#include "game/menu.h"
#include "game/menustate.h"
#include <string.h>

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
void fn_80235380(u8* p);
s32 fn_80129AF4(void);
int fn_801298E8(void* info, s32 ch);
void fn_80214FD8(const char* title, const char* msg);
void fn_802C519C(u8* p);
void fn_802328D4(u8* p);
u8* fn_802C5130(const char* name);
void fn_802E30C8(const char* fmt, ...);
u8* fn_80232578(const char* name);
u8* fn_8023512C(const char* art);
}

void operator delete(void* p);

extern char lbl_80342FC0[];  // string pool
extern const f32 lbl_80563830;

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

MemCardDialogMgr::~MemCardDialogMgr(void) { lbl_8055F244 = 0; }

void MemCardDialogBg::Load(void) {
    const char* str = lbl_80342FC0;
    if ((image = fn_802C5130(str + 0x60)) == 0) {
        fn_802E30C8(str + 0x78);
    }
    image2 = 0;
    font2 = fn_80232578(str + 0xB4);
    font = fn_80232578(str + 0xB4);
    fade = lbl_80563830;
}

void MemCardDialogBg::Unload(void) {
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

void MemCardDialogBg::Setup(McDialogCfg* cfg) {}  // stub (363)

void MemCardDialogBg::Show(const McMsg* m) {}  // stub (239)

void MemCardDialogMgr::Open(void) {
    open = 1;
    layout = fn_8012C33C("mc_dialog.lyt");
    if (!layout) {
        bg.Load();
    }
    bgShown = 1;
}

void MemCardDialogMgr::Close(void) {
    open = 0;
    if (!layout) {
        bg.Unload();
    }
    fn_80214D50(4);
}

void MemCardDialogMgr::Setup(McDialogCfg* cfg) {}  // stub (279)

extern "C" {

u8* fn_802461D0(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

void fn_80246210(void) {
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

// The save progress bar ("mem_bar.art"; vtable lbl_803430E8, dtor fn_80246428).
class MemBar : public MenuState {
public:
    MemBar(f32 f) {
        frac = f;
        art = fn_8023512C("mem_bar.art");
    }
    virtual ~MemBar(void) {
        if (art) {
            fn_80235380(art);
        }
    }
    virtual void Draw(void);

    f32 frac;  // offset 0x18
    u8* art;   // offset 0x1C
};

extern const MenuDesc lbl_802FA350;  // "mc_dialog.lyt"
extern UnkStatic123s lbl_80498890;  // menu end tag (menusel.cpp)

extern "C" {
void fn_80216CA8(void);
int fn_802450D4(void);
}

// Shows a memory card message with its progress bar (on the plain background without the layout).
void MemCardDialogMgr::ShowMsg(const McMsg* m) {
    if (!layout) {
        bg.Show(m);
        return;
    }
    MemBar bar(m->frac);
    MenuItem items[] = {
        MenuItem("#title", m->title, CaptionContext(strlen(m->title))),
        MenuItem("#msg", m->text, CaptionContext(strlen(m->text))),
        MenuItem("progress", 5, &bar, 0, 0),
        MenuItem(lbl_80498890),
    };
    retries = 0;
    MenuDesc desc = {"mc_status.lyt", 0, (int (*)(int*))fn_802450D4, 0, 0};
    desc.items = items;
    fn_80216CA8();
    fn_802146BC(&desc, -999, -999, 1);
}

extern "C" {


void fn_80246498(u8* p) { fn_80214CB4(U8(p, 0x18) == 0); }

void fn_802464A8(u8* p) { U8(p, 0x19) = 0; }

int fn_802464B4(u8* p, int key) {
    switch (key) {
    case 0x125:
        if (!U8(p, 0x19)) {
            fn_80214CB4(0);
            fn_80230E90(0);
        } else {
            fn_80216D28();
        }
        break;
    case 0x127:
        if (!U8(p, 0x19)) {
            fn_80214CB4(1);
            fn_80230E90(0);
        } else {
            fn_80216D28();
        }
        break;
    case 0x126:
        if (U8(p, 0x19)) {
            fn_80214CB4(0);
            fn_80230E90(0);
        } else {
            fn_80216D28();
        }
        break;
    case 0x128:
        if (U8(p, 0x19)) {
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

}  // extern "C"

extern "C" {
void fn_80230C30(f32* x, f32* y, s32 lx, s32 ly);
f32 fn_80235EF0(u8* art, int part);
f32 fn_80235F64(u8* art, int part);
void fn_802353F4(u8* art, int part, f32 x, f32 y, f32 z);
void fn_802354A0(u8* art, int part, f32 x, f32 y, f32 z, f32 w, f32 h, u32 color);
void fn_80235758(u8* art, int part, f32 x, f32 y, f32 w, f32 h, f32 u0, f32 v0, f32 u1, f32 v1, u32 color);
void fn_80230924(u32 color, f32 x0, f32 y0, f32 x1, f32 y1, f32 z);
}

// Left cap, stretched middle and right cap, the filled part drawn over them; a plain bar without art.
void MemBar::Draw(void) {
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
            fn_80235758(art, 3, sx, sy, fill, ah, 0.0f, 0.0f, fill / lw, 1.0f, -1);
        } else if (fill < lw + mid) {
            fn_802353F4(art, 3, sx, sy, 0.0f);
            fn_80235758(art, 4, sx + lw, sy, fill - lw, ah, 0.0f, 0.0f, (fill - lw) / mid, 1.0f, -1);
        } else {
            fn_802353F4(art, 3, sx, sy, 0.0f);
            fn_802354A0(art, 4, sx + lw, sy, 0.0f, mid, ah, -1);
            fn_80235758(art, 5, mid + (sx + lw), sy, (fill - lw) - mid, ah, 0.0f, 0.0f, ((fill - lw) - mid) / rw, 1.0f,
                        -1);
        }
        fn_802353F4(art, 0, sx, sy, 0.0f);
        fn_802354A0(art, 1, sx + lw, sy, 0.0f, mid, ah, -1);
        fn_802353F4(art, 2, mid + (sx + lw), sy, 0.0f);
    } else {
        fn_80230924(-1, sx, sy, sx + fw * frac, sy - fh, 0.0f);
    }
}

extern "C" {

void fn_8024683C(void) {
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
        fn_80214FD8("Wireless Warning", "Any information you enter might be picked up by another wireless receiver");
    }
}

}  // extern "C"
