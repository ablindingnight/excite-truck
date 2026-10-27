// Unlockables menu (0x8024497C-0x80245030): the reputation rating and a scrolling list of the
// unlockable cars and tracks, three at a time, each with its points and a lock icon. Names are
// guesses apart from the layout and element names and the messages.
#include "game/common.h"

struct Unlockable {
    const char* type;  // offset 0x0
    const char* name;  // offset 0x4
    s32 points;        // offset 0x8
    u8 unlocked;       // offset 0xC
    s32 order;         // offset 0x10
};

int GetUnlockableCount(void);
Unlockable* GetUnlockable(int i);

struct MenuItem {
    MenuItem(const char* n, int t, const void* d, int a0, int a1) {
        type = t;
        name = n;
        data = d;
        arg0 = a0;
        arg1 = a1;
    }
    const char* name;
    int type;
    const void* data;
    int arg0;
    int arg1;
};

struct MenuDesc {
    const char* layout;
    MenuItem* items;
    int (*poll)(int* result);
    int unkC;
    int unk10;
};

extern "C" {
int sprintf(char* buf, const char* fmt, ...);
char* strcpy(char* dst, const char* src);
int strcasecmp(const char* a, const char* b);
int fn_802146BC(MenuDesc* desc, int x, int y, int a);
void fn_80236914(void);
void fn_80236900(void);
void fn_802E30C8(const char* fmt, ...);  // debug print
s32 fn_802E2C20(void);                   // milliseconds
int fn_80019888(void);                   // reputation rating
void fn_8013C804(void);
void fn_8013C8B8(void);
s32 fn_8013C8E8(const char* chassis);
const char* fn_8013C9C8(s32 id);
void fn_80230C30(f32* x, f32* y, int a, int b);  // element position
int fn_80232578(const char* name);              // load font
void fn_802328D4(int font);
int fn_80232B70(int font);  // font height
void fn_80232BA4(const char* text, int font, f32 x, f32 y, u32 color, int flags, f32 scale, f32 z, f32 alpha);
void fn_802337A0(int font, u32 color, int flags, const char* fmt, f32 x, f32 y, f32 z, f32 scale,
                 f32 alpha, ...);
int fn_802C5130(const char* name);  // load image
void fn_802C519C(int image);
int fn_802C51C0(int image);  // image width
void fn_802C5310(int image, int frame, f32 x, f32 y, f32 z);
extern char lbl_8055BAB8[4];
extern char lbl_8055B860[8];
}

// The menu state interface. Its all-pure vtable (24 zero words) is what follows each menu state's
// vtable in the target's .data.
class IMenuState {
public:
    IMenuState(void) {}
    virtual ~IMenuState(void) = 0;
    virtual void Enter(void) = 0;
    virtual void Exit(void) = 0;
    virtual void Unk14(void) = 0;
    virtual void Unk18(void) = 0;
    virtual void Update(void) = 0;
    virtual void Unk20(void) = 0;
    virtual void Unk24(void) = 0;
    virtual void Draw(void) = 0;
    virtual void Unk2C(void) = 0;
    virtual void Unk30(void) = 0;
    virtual void Unk34(void) = 0;
    virtual void Unk38(void) = 0;
    virtual void Unk3C(void) = 0;
    virtual void Unk40(void) = 0;
    virtual void Unk44(void) = 0;
    virtual void Unk48(void) = 0;
    virtual void Unk4C(void) = 0;
    virtual void Unk50(void) = 0;
    virtual void Unk54(void) = 0;
    virtual int Key(int key) = 0;
    virtual void Unk5C(void) = 0;
};

inline IMenuState::~IMenuState(void) {}

// A menu state; most slots are empty defaults.
class MenuState : public IMenuState {
public:
    MenuState(void) { unk14 = 0; }
    virtual ~MenuState(void) {}
    virtual void Enter(void);   // 0x0C
    virtual void Exit(void);    // 0x10
    virtual void Unk14(void);
    virtual void Unk18(void);
    virtual void Update(void);  // 0x1C
    virtual void Unk20(void);
    virtual void Unk24(void);
    virtual void Draw(void);    // 0x28
    virtual void Unk2C(void);
    virtual void Unk30(void);
    virtual void Unk34(void);
    virtual void Unk38(void);
    virtual void Unk3C(void);
    virtual void Unk40(void);
    virtual void Unk44(void);
    virtual void Unk48(void);
    virtual void Unk4C(void);
    virtual void Unk50(void);
    virtual void Unk54(void);
    virtual int Key(int key);   // 0x58
    virtual void Unk5C(void);

    int x;      // offset 0x4
    int y;      // offset 0x8
    int width;  // offset 0xC
    int unk10;  // offset 0x10
    int unk14;  // offset 0x14
};

namespace {

class UnlockList : public MenuState {
public:
    UnlockList(void) {
        font = fn_80232578("uifnt.gtx");
        image = fn_802C5130("chcklock.img");
        count = GetUnlockableCount();
        top = 0;
        sprintf(title, "Game Reputation Rating: %d", fn_80019888());
        downFlash = 0;
        upFlash = 0;
        fn_802E30C8("UnlockableList: list size is %d", count);
        for (int i = 0; i < count; i++) {
            Unlockable* u = GetUnlockable(i);
            fn_802E30C8("  item %d: %s / %s / %d", i, u->type, u->name, u->points);
        }
    }
    virtual ~UnlockList(void) {
        fn_802328D4(font);
        font = 0;
        fn_802C519C(image);
        image = 0;
    }
    virtual void Enter(void);
    virtual void Exit(void);
    virtual int Key(int key);
    virtual void Update(void);
    virtual void Draw(void);

    int count;       // offset 0x18
    int top;         // offset 0x1C
    char title[0x80];  // offset 0x20
    int upFlash;     // offset 0xA0
    int downFlash;   // offset 0xA4
    s32 upTime;      // offset 0xA8
    s32 downTime;    // offset 0xAC
    int font;        // offset 0xB0
    int image;       // offset 0xB4
};

}  // namespace

ModuleTag lbl_8055F228;
static UnkStatic123 lbl_80498878;
UnkStatic7F lbl_8055F230;

extern "C" int fn_8024497C(void) {
    fn_80236900();
    return 1;
}

extern "C" void fn_802449A0(void) {
    int arrows = 1;
    UnlockList list;
    MenuItem items[] = {
        MenuItem("downarrow", 2, &arrows, 0, 0),
        MenuItem("#rating", 3, list.title, 0x80, 0),
        MenuItem("unlockable_list", 5, &list, 0, 0),
        MenuItem(lbl_8055B860, 6, (void*)fn_8024497C, -1, 27),
        MenuItem(lbl_8055BAB8, 8, (void*)fn_80236914, 2, 0),
        *(MenuItem*)&lbl_80498878,
    };
    MenuDesc desc = {"unlock.lyt", items, NULL, 0, 0};
    fn_802146BC(&desc, -999, -999, 1);
}

void UnlockList::Enter(void) { fn_8013C804(); }

void UnlockList::Exit(void) { fn_8013C8B8(); }

int UnlockList::Key(int key) {
    switch (key) {
    case 0x126:
        upTime = fn_802E2C20();
        top--;
        break;
    case 0x128:
        downTime = fn_802E2C20();
        top++;
        break;
    default:
        return 0;
    }
    int t = top > 0 ? top : 0;
    top = t < count - 3 ? t : count - 3;
    return 1;
}

void UnlockList::Update(void) {
    s32 now = fn_802E2C20();
    upFlash = now <= upTime + 250;
    downFlash = now <= downTime + 250;
}

void UnlockList::Draw(void) {
    f32 px;
    f32 py;
    fn_80230C30(&px, &py, x, y);
    f32 w = width;
    f32 lockX = px + w - fn_802C51C0(image);
    f32 left = px;
    f32 ptsX = lockX - 15.0f;
    f32 textX = left + w / 2.8f;
    f32 nameScale = 22.0f / fn_80232B70(font);
    f32 ptsScale = 20.0f / fn_80232B70(font);
    int rating = fn_80019888();
    f32 rowY = py;
    for (int i = 0; i < 3; i++) {
        if (top + i >= count) {
            break;
        }
        Unlockable* u = GetUnlockable(top + i);
        f32 nameY = rowY - 3.0f;
        f32 ptsY = rowY - 4.0f;
        f32 lockY = 2.0f + rowY;
        char buf[0x40] = {0};
        if (strcasecmp(u->type, "car") == 0) {
            sprintf(buf, "%s Car", fn_8013C9C8(fn_8013C8E8(u->name)));
        } else if (strcasecmp(u->type, "track") == 0) {
            strcpy(buf, "Track");
        } else {
            fn_802E30C8("Unknown unlockable type %s for %s", u->type, u->name);
        }
        fn_80232BA4(u->name, font, left, nameY, 0xFFDFDF20, 9, nameScale, 0.0f, 1.0f);
        int frame = 2;
        if (u->points <= rating || u->unlocked) {
            frame = 3;
        }
        fn_802C5310(image, frame, lockX, lockY, 0.0f);
        fn_802337A0(font, -1, 10, "%d Pts", ptsX, ptsY, 0.0f, ptsScale, 1.0f, u->points);
        fn_80232BA4(buf, font, textX, ptsY, -1, 9, ptsScale, 0.0f, 1.0f);
        rowY -= 33.0f;
    }
}

// NonMatching: fn_802449A0 and Draw number their saved registers differently (65 and 17 diffs, same
// instructions); MenuState's and IMenuState's inline dtors are emitted here as weak copies.
