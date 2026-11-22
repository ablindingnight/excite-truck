// Unlockables menu (0x8024497C-0x80245030): the reputation rating and a scrolling list of the
// unlockable cars and tracks, three at a time, each with its points and a lock icon. Names are
// guesses apart from the layout and element names and the messages.
#include "game/common.h"
#include "game/menustate.h"
#include "game/gamedb.h"
#include "game/menu.h"


// MenuItem: game/menu.h.
// MenuDesc: game/menu.h.
extern "C" {
int sprintf(char* buf, const char* fmt, ...);
char* strcpy(char* dst, const char* src);
int strcasecmp(const char* a, const char* b);
int fn_802146BC(MenuDesc* desc, int x, int y, int a);
void fn_80236914(int which, f32* out, f32 t);
void fn_80236900(void);
void fn_802E30C8(const char* fmt, ...);  // debug print
s32 fn_802E2C20(void);                   // milliseconds
int fn_80019888(void);                   // reputation rating
void LoadCarClassFile(void);
void fn_8013C8B8(void);
s32 FindCarClassByChassis(const char* chassis);
const char* fn_8013C9C8(s32 id);
void CalculateScreenPosition(f32* x, f32* y, int a, int b);  // element position
int fn_80232578(const char* name);              // load font
void fn_802328D4(int font);
int fn_80232B70(int font);  // font height
void DrawColoredText(const char* text, int font, f32 x, f32 y, u32 color, int flags, f32 scale, f32 z, f32 alpha);
void PrintFormattedText(int font, u32 color, int flags, const char* fmt, f32 x, f32 y, f32 z, f32 scale,
                 f32 alpha, ...);
int gxGetImage(const char* name);  // load image
void fn_802C519C(int image);
int fn_802C51C0(int image);  // image width
void DrawImageFrame(int image, int frame, f32 x, f32 y, f32 z);
extern char lbl_8055BAB8[4];
extern char lbl_8055B860[8];
}


namespace {

class UnlockList : public MenuState {
public:
    UnlockList(void) {
        font = fn_80232578("uifnt.gtx");
        image = gxGetImage("chcklock.img");
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
    virtual void Draw(void) {
        f32 textY;
        f32 px;
        f32 py;
        CalculateScreenPosition(&px, &py, x, y);
        f32 w = this->w;
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
            textY = rowY - 4.0f;
            char buf[0x40] = {0};
            if (strcasecmp(u->type, "car") == 0) {
                sprintf(buf, "%s Car", fn_8013C9C8(FindCarClassByChassis(u->name)));
            } else if (strcasecmp(u->type, "track") == 0) {
                strcpy(buf, "Track");
            } else {
                fn_802E30C8("Unknown unlockable type %s for %s", u->type, u->name);
            }
            DrawColoredText(u->name, font, left, nameY, 0xFFDFDF20, 9, nameScale, 0.0f, 1.0f);
            int frame = 2;
            if (u->points <= rating || u->unlocked) {
                frame = 3;
            }
            DrawImageFrame(image, frame, lockX, lockY, 0.0f);
            PrintFormattedText(font, -1, 10, "%d Pts", ptsX, ptsY, 0.0f, ptsScale, 1.0f, u->points);
            DrawColoredText(buf, font, textX, textY, -1, 9, ptsScale, 0.0f, 1.0f);
            rowY -= 33.0f;
        }
    }

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

/* Helper name unproven */
static inline MenuItem MakeRatingLabel(const UnlockList& list) {
    const CaptionContext context(sizeof(list.title));
    return MenuItem(context, "#rating", list.title);
}

#pragma push
#pragma opt_propagation off  /* Pragma unproven */
/* Helper name unproven */
static inline MenuItem MakeListPanel(MenuState& list, int kind = 5) {
    return MenuItem("unlockable_list", kind, &list, 0, 0);
}

/* Helper name unproven */
static inline MenuItem MakeUnlockOption(void) {
    int on = 2;
    return MenuItem(lbl_8055BAB8, 8, (void*)fn_80236914, on, 0);
}

extern "C" void fn_802449A0(void) {
    UnlockList list;
    int arrows = 1;
    MenuItem items[] = {
        MenuItem("downarrow", 2, &arrows, 0, 0),
        MakeRatingLabel(list),
        MakeListPanel(list),
        MenuItem(lbl_8055B860, 6, (void*)fn_8024497C, -1, 27),
        MakeUnlockOption(),
        MenuItem(lbl_80498878),
    };
    MenuDesc desc = {"unlock.lyt", items, NULL, 0, 0};
    fn_802146BC(&desc, -999, -999, 1);
}
#pragma pop

void UnlockList::Enter(void) { LoadCarClassFile(); }

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


