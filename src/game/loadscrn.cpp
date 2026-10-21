// Loading screen state (0x802445FC-0x8024497C): the menu state shown while a race loads; it fades
// the music and the screen over its duration. Names are guesses apart from the layout name.
#include "game/common.h"

class EventInfo;

EventInfo* FindTrackEvent(int track, int eve);
void LoadingBegin(EventInfo* info, long difficulty);
void LoadingStop3(void);
void LoadingFinish(void);
void LoadingNop(void);
void DrawLoadingFull(void);
void DrawLoadingTextOnly(void);

struct LoadView {
    u8 pad0[0x38];
};

extern "C" {
f32 fn_8020A5F0(void);
void fn_8020A5F8(f32 volume);
void fn_8020A634(void);
void fn_8012E81C(const char* name);
s32 fn_802E2C20(void);
int fn_802ECC10(void);
void fn_802C942C(u32 color);
void fn_80230B5C(LoadView* v);
void fn_80230AD8(LoadView* v);
void fn_80230B38(void);
}

// A menu state; most slots are empty defaults.
class MenuState {
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
    void Finish(int result);

    u8 pad4[0x10];
    int unk14;
};

class LoadScreenState : public MenuState {
public:
    LoadScreenState(EventInfo* event, int difficulty) {
        this->event = event;
        this->difficulty = difficulty;
        start = 0;
        duration = 1000;
        volume = fn_8020A5F0();
    }
    virtual ~LoadScreenState(void) {}
    virtual void Enter(void);
    virtual void Exit(void);
    virtual void Update(void);
    virtual void Draw(void);

    EventInfo* event;  // offset 0x18
    int difficulty;    // offset 0x1C
    s32 start;         // offset 0x20
    s32 duration;      // offset 0x24
    f32 volume;        // offset 0x28
};

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

extern "C" int fn_802146BC(MenuDesc* desc, int x, int y, int a);

ModuleTag lbl_8055F218;
static UnkStatic123 lbl_80498860;
UnkStatic7F lbl_8055F220;

extern "C" void fn_802445FC(int track, int eve, int difficulty) {
    LoadScreenState state(FindTrackEvent(track, eve), difficulty);
    MenuItem items[] = {
        MenuItem(NULL, 5, &state, 0, 0),
        *(MenuItem*)&lbl_80498860,
    };
    MenuDesc desc = {"loadksk_gc.lyt", items, NULL, 0, 0};
    fn_802146BC(&desc, -999, -999, 1);
}

extern "C" void fn_80244738(void) {
    LoadingFinish();
    LoadView view;
    fn_80230B5C(&view);
    fn_80230AD8(&view);
    fn_802C942C(0xFF000000);
    DrawLoadingTextOnly();
    fn_80230B38();
    LoadingNop();
}

// Puts "race.res" after the vtable in the string pool, as in the original.
struct LoadResources {
    static void Unload(void) { fn_8012E81C("race.res"); }
};

void LoadScreenState::Enter(void) {
    LoadResources::Unload();
    LoadingBegin(event, difficulty);
    start = fn_802E2C20();
}

void LoadScreenState::Exit(void) {
    LoadingStop3();
    fn_8020A634();
    fn_8020A5F8(volume);
}

void LoadScreenState::Update(void) {
    s32 elapsed = fn_802E2C20() - start;
    f32 f = 1.0f - (f32)elapsed / (f32)duration;
    fn_8020A5F8(volume * ((0.0f > f) ? 0.0f : f));
    if (elapsed > duration && !fn_802ECC10()) {
        Finish(0);
    }
}

void LoadScreenState::Draw(void) {
    DrawLoadingFull();
    int a = 255 - ((fn_802E2C20() - start) * 255) / (duration - 50);
    u8 alpha = (a > 0) ? a : 0;
    if (alpha) {
        fn_802C942C(alpha << 24);
    }
}

// NonMatching: every function matches, but the target emits the vtable mid-.data (after the first
// function's string) and our compiler always emits it at the end of the unit.
