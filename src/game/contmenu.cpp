// Controller options menu (0x80263080-0x80263F30, "contunified.lyt"): a rumble and a speaker toggle
// for each of two controllers, saved to the "force_feedback" / "controller_speaker" options, and a
// "decide" button. The items are menu states. Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
const char* fn_80131234(const char* key);  // localize
void fn_8022BAD0(int style, int x, int y, int z, int w, int h, const char* text, int color, f32 scale, int a);
}

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

    void DrawText(int style, int dx, int dy, const char* key, int color) {
        int x, y, w, h;
        h = this->h;
        w = this->w;
        y = this->y;
        x = this->x;
        fn_8022BAD0(style, x + dx, y + dy, 0, w, h, fn_80131234(key), color, 1.0f, 0);
    }

    s32 x;      // offset 0x4
    s32 y;      // offset 0x8
    s32 w;      // offset 0xC
    s32 h;      // offset 0x10
    int unk14;  // offset 0x14
};

class Sound {
public:
    virtual ~Sound(void);
    void Play(void) {
        play = 1;
        b4B = 0;
        b4A = 0;
    }
    void SetMask(u32 m) {
        mask = m;
        b4F = 1;
    }
    void SetVolume(f32 v) {
        if (v != vol) {
            volDirty = 1;
            vol = v;
        }
    }

    f32 pitch;  // offset 0x4
    f32 vol;    // offset 0x8
    u8 padC[0x14 - 0xC];
    u32 mask;   // offset 0x14
    u8 pad18[0x49 - 0x18];
    u8 play;      // offset 0x49
    u8 b4A;       // offset 0x4A
    u8 b4B;       // offset 0x4B
    u8 b4C;       // offset 0x4C
    u8 volDirty;  // offset 0x4D
    u8 b4E;       // offset 0x4E
    u8 b4F;       // offset 0x4F
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

extern "C" {
int fn_802146BC(MenuDesc* desc, int x, int y, int a);
void fn_80230E90(int a);
s32 fn_80232578(const char* name);  // load font
void fn_802328D4(s32 font);
Sound* fn_80207848(const char* name, int flags);
void fn_801333E8(int section, const char* name, bool* v);
void fn_80133460(int section, const char* name, bool v);
s32 fn_802E2C20(void);  // milliseconds
void fn_802E8528(int ch, f32 a, f32 b);
int fn_802E8D84(void);
int fn_802E6FBC(void);
void fn_802E8518(int ch);
int fn_80129890(void);
void fn_802E317C(const char* fmt, ...);
extern char lbl_8055B860[8];
}

ModuleTag lbl_8055F428;
static UnkStatic123 lbl_80498F40;
UnkStatic7F lbl_8055F430;
int lbl_8055F434;      // the selected row
int lbl_8055C440 = 4;  // the last row

// The "decide" row: confirms the menu.
class DecideItem : public MenuState {
public:
    DecideItem(void) { selected = 0; }
    virtual ~DecideItem(void) {}
    virtual void Exit(void) {}
    virtual void Update(void) {
        if (lbl_8055F434 == lbl_8055C440) {
            selected = 1;
        } else {
            selected = 0;
        }
    }
    virtual void Draw(void) { DrawText(0x12, 0, 0, "UIOptions:DECIDE", selected); }

    int selected;  // offset 0x18
};

// An on/off option row.
class OptionItem : public MenuState {
public:
    OptionItem(void) {
        selected = 0;
        value = 1;
        focused = 0;
        font = fn_80232578("uifnt_o.gtx");
    }
    virtual ~OptionItem(void) {
        fn_802328D4(font);
        font = 0;
    }
    virtual void Unk60(void) {}
    virtual void Exit(void) {}
    virtual void Save(void) {}
    virtual int Key(int key) {
        if (focused) {
            switch (key) {
            case 0x125:
                if (!value) {
                    value = 1;
                }
                break;
            case 0x127:
                if (value) {
                    value = 0;
                }
                break;
            }
        }
        return 0;
    }
    virtual void Update(void) {
        if (lbl_8055F434 == index) {
            selected = 1;
            focused = 1;
        } else {
            selected = 0;
            focused = 0;
        }
    }
    virtual void Draw(void) {
        int on;
        int off;
        if (focused) {
            if (value) {
                on = 3;
                off = 0;
            } else {
                on = 0;
                off = 3;
            }
        } else {
            if (value) {
                on = 2;
                off = 0;
            } else {
                on = 0;
                off = 2;
            }
        }
        if (index % 2 != 0) {
            DrawText(0xC9, 0, 0, "UIOptions:SPEAKER", selected);
        } else {
            DrawText(0xC9, 0, 0, "UIOptions:RUMBLE", selected);
        }
        DrawText(0xCA, 345, 2, "UIOptions:ENABLE", on);
        DrawText(0xCA, 445, 2, "UIOptions:DISABLE", off);
    }

    s32 font;      // offset 0x18
    int selected;  // offset 0x1C
    int index;     // offset 0x20
    int pad;       // offset 0x24 (the controller)
    bool value;    // offset 0x28
    bool focused;  // offset 0x29
};

class RumbleItem : public OptionItem {
public:
    RumbleItem(void) { start = -1000.0f; }
    virtual void Load(int pad) {
        this->pad = pad;
        index = pad * 2;
        fn_801333E8(pad + 8, "force_feedback", &value);
    }
    virtual void Save(void) { fn_80133460(pad + 8, "force_feedback", value); }
    virtual int Key(int key) {
        if (focused) {
            switch (key) {
            case 0x125:
                if (!value) {
                    value = 1;
                    start = (f32)fn_802E2C20() / 1000.0f;
                }
                break;
            case 0x127:
                if (value) {
                    value = 0;
                }
                break;
            }
        }
        return OptionItem::Key(key);
    }
    virtual void Update(void) {
        if ((f32)fn_802E2C20() / 1000.0f - start < 0.5f) {
            fn_802E8528(pad, 1.0f, 0.0f);
        } else {
            fn_802E8528(pad, 0.0f, 0.0f);
        }
        OptionItem::Update();
    }

    f32 start;  // offset 0x2C
};

class SpeakerItem : public OptionItem {
public:
    SpeakerItem(void) {
        changed = 0;
        mask = 0;
        sound = fn_80207848("a_accept.sfx", 0);
        if (sound) {
            sound->SetVolume(1.0f);
        } else {
            fn_802E317C("Invalid sound file in SpeakerControl");
        }
    }
    virtual ~SpeakerItem(void) {
        if (sound) {
            delete sound;
        }
    }
    virtual void Load(int pad) {
        this->pad = pad;
        index = pad * 2 + 1;
        fn_801333E8(pad + 8, "controller_speaker", &value);
        mask |= 1 << this->pad;
    }
    virtual void Save(void) { fn_80133460(pad + 8, "controller_speaker", value); }
    virtual int Key(int key) {
        if (focused) {
            switch (key) {
            case 0x125:
                if (!value) {
                    value = 1;
                    changed = 1;
                }
                break;
            case 0x127:
                if (value) {
                    value = 0;
                }
                break;
            }
        }
        return OptionItem::Key(key);
    }
    virtual void Update(void) {
        if (changed) {
            changed = 0;
            sound->SetMask(mask);
            sound->Play();
        }
        OptionItem::Update();
    }

    bool changed;  // offset 0x2A
    u32 mask;      // offset 0x2C
    Sound* sound;  // offset 0x30
};

extern "C" {

int fn_80263080(void) {
    if (--lbl_8055F434 < 0) {
        lbl_8055F434 = 0;
    }
    fn_80230E90(0);
    return 0;
}

int fn_802630C0(void) {
    if (++lbl_8055F434 > lbl_8055C440) {
        lbl_8055F434 = lbl_8055C440;
    }
    fn_80230E90(0);
    return 0;
}

int fn_80263104(void) { return lbl_8055F434 == lbl_8055C440; }

void fn_8026311C(void) {
    RumbleItem rumble[2];
    SpeakerItem speaker[2];
    for (int i = 0; i < 2; i++) {
        rumble[i].Load(i);
        speaker[i].Load(i);
    }
    DecideItem decide;
    MenuItem items[] = {
        MenuItem("c1rumble", 5, &rumble[0], 0, 0),
        MenuItem("c1speaker", 5, &speaker[0], 0, 0),
        MenuItem("c2rumble", 5, &rumble[1], 0, 0),
        MenuItem("c2speaker", 5, &speaker[1], 0, 0),
        MenuItem("decide", 5, &decide, 0, 0),
        MenuItem(lbl_8055B860, 6, (void*)fn_80263080, 0, 0x126),
        MenuItem(lbl_8055B860, 6, (void*)fn_802630C0, 0, 0x128),
        MenuItem(lbl_8055B860, 6, (void*)fn_80263104, -2, 13),
        MenuItem(lbl_8055B860, 6, NULL, -1, 27),
        *(MenuItem*)&lbl_80498F40,
    };
    MenuDesc desc = {"contunified.lyt", items, NULL, 0, 0};
    switch (fn_802146BC(&desc, -999, -999, 1)) {
    case -2:
        for (int i = 0; i < 2; i++) {
            rumble[i].Save();
            speaker[i].Save();
        }
        if (fn_802E8D84()) {
            fn_802E6FBC();
        }
        for (int i = 0; i < fn_80129890(); i++) {
            fn_802E8518(i);
        }
    case -1:
        lbl_8055F434 = 0;
        break;
    default:
        fn_802E317C("MenuDoControlUnified: invalid return value from dialog!");
        break;
    }
}

}  // extern "C"
