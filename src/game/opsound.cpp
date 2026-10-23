// Sound options menu (0x8023CA18-0x8023D4B8, "opsound.lyt" / "sound.lyt"): music, sound effect,
// announcer, tyre and video volumes, the sound quality and race music choices and the speaker mixer,
// saved to option section 4. Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
char* strcpy(char* d, const char* s);
}

class MenuState {
public:
    MenuState(void) { unk14 = 0; }
    virtual ~MenuState(void) {}
    virtual void Enter(void);   // 0x0C
    virtual void Exit(void);    // 0x10
    virtual void Unk14(void);
    virtual void Changed(int item);  // 0x18
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
    void Stop(void) {
        b4A = 1;
        b4C = 0;
    }
    void SetVolume(f32 v) {
        if (v != vol) {
            volDirty = 1;
            vol = v;
        }
    }

    f32 pitch;  // offset 0x4
    f32 vol;    // offset 0x8
    u8 padC[0x49 - 0xC];
    u8 play;      // offset 0x49
    u8 b4A;       // offset 0x4A
    u8 b4B;       // offset 0x4B
    u8 b4C;       // offset 0x4C
    u8 volDirty;  // offset 0x4D
};

class Mixer {
public:
    virtual ~Mixer(void);
    virtual const char* Name(void);  // 0x0C
};

// One menu widget binding: the layout element's name, its kind and kind-specific data.
struct MenuItem {
    MenuItem(const char* n, int t, const void* d, int a0, int a1) {
        type = t;
        name = n;
        data = d;
        arg0 = a0;
        arg1 = a1;
    }
    MenuItem(const char* n, f32* v, const struct MenuRange& r);
    const char* name;
    int type;
    const void* data;
    int arg0;
    int arg1;
};

struct MenuRange {
    MenuRange(void) {}
    MenuRange(f32 a, f32 b) {
        lo = a;
        hi = b;
    }
    f32 lo, hi;
};

inline MenuItem::MenuItem(const char* n, f32* v, const MenuRange& r) {
    name = n;
    type = 9;
    data = v;
    *(MenuRange*)&arg0 = r;
}

// A slider: the range is stored in the item's argument words.
struct SliderItem {
    SliderItem(const char* n, f32* v, MenuRange r) {
        name = n;
        type = 9;
        data = v;
        range = r;
    }
    const char* name;
    int type;
    const void* data;
    MenuRange range;
};

struct MenuDesc {
    const char* layout;
    MenuItem* items;
    int (*poll)(int* result);
    int unkC;
    int unk10;
};

struct MenuStrList {
    u8 pad0[0x14];
};

extern "C" {
MenuStrList* fn_8022D228(MenuStrList* l, int count, int len);
void fn_8022D2BC(MenuStrList* l, int flags);
void fn_8022D350(MenuStrList* l, const char* s, int a);
int fn_802146BC(const MenuDesc* desc, int x, int y, int a);
void fn_80214A9C(int sound);
void fn_80236900(void);
void fn_80236914(void);
void fn_801330E8(int section);
void fn_801331C4(int section, const char* key, f32* v);
void fn_8013323C(int section, const char* key, f32 v);
void fn_801332D0(int section, const char* key, s32* v);
void fn_80133348(int section, const char* key, s32 v);
void fn_801333E8(int section, const char* key, bool* v);
void fn_80133460(int section, const char* key, bool v);
s32 fn_80208D3C(void);  // mixer count
Mixer* fn_80208D4C(int i);
void fn_80208D60(s32 i);
s32 fn_80208D70(void);
void fn_80209030(f32 v);
void fn_80209090(s32 q);
void fn_8020A5F8(f32 v);
void fn_80221EA0(MenuState* m, int item, void* value, int kind);
extern char lbl_8055BAC0[8];
extern char lbl_8055BABC[4];
extern char lbl_8055BAB8[4];
extern char lbl_8055B860[8];
}

// Sound options state behind the menu items.
class SoundOptions : public MenuState {
public:
    SoundOptions(f32* fx, f32* music, s32* quality, s32* mixer, char* mixerName, f32* announcer, f32* tire,
                 f32* video) {
        this->fx = fx;
        this->music = music;
        this->quality = quality;
        this->mixer = mixer;
        this->mixerName = mixerName;
        this->announcer = announcer;
        this->tire = tire;
        this->video = video;
        sound = 0;
    }
    virtual ~SoundOptions(void) {
        if (sound) {
            delete sound;
            sound = 0;
        }
    }
    virtual void Enter(void);
    virtual void Changed(int item);
    virtual void Unk24(void);
    void UpdateMixerName(void) {
        s32 i = *mixer;
        if (i >= 0 && i < fn_80208D3C()) {
            strcpy(mixerName, fn_80208D4C(i)->Name());
        } else {
            strcpy(mixerName, "N/A");
        }
    }

    f32* fx;          // offset 0x18
    f32* music;       // offset 0x1C
    s32* quality;     // offset 0x20
    s32* mixer;       // offset 0x24
    char* mixerName;  // offset 0x28
    f32* announcer;   // offset 0x2C
    f32* tire;        // offset 0x30
    f32* video;       // offset 0x34
    Sound* sound;     // offset 0x38
};

s32 lbl_8055BB68 = 2;  // how the menu was opened

ModuleTag lbl_8055F158;
bool lbl_8055F160;  // music_enabled
f32 lbl_8055F164;   // music_volume
f32 lbl_8055F168;   // spotter_volume
f32 lbl_8055F16C;   // effects_volume
f32 lbl_8055F170;   // tire_volume
f32 lbl_8055F174;   // video_volume
s32 lbl_8055F178;   // race_music_enabled
s32 lbl_8055F17C;   // quality
static UnkStatic123 lbl_804987A0;

extern "C" {

void fn_8023CA18(void) {
    if (lbl_8055BB68 == 0) {
        fn_80214A9C(0x1F);
    }
    fn_80214A9C(1);
}

void fn_8023CA50(void) {}

void fn_8023CA54(void) {
    lbl_8055F160 = 0;
    fn_801333E8(4, "music_enabled", &lbl_8055F160);
    lbl_8055F164 = 1.0f;
    fn_801331C4(4, "music_volume", &lbl_8055F164);
    lbl_8055F168 = 1.0f;
    fn_801331C4(4, "spotter_volume", &lbl_8055F168);
    lbl_8055F16C = 1.0f;
    fn_801331C4(4, "effects_volume", &lbl_8055F16C);
    fn_80209030(lbl_8055F16C);
    lbl_8055F17C = 0;
    fn_801332D0(4, "quality", &lbl_8055F17C);
    fn_80209090(lbl_8055F17C);
    lbl_8055F170 = 1.0f;
    fn_801331C4(4, "tire_volume", &lbl_8055F170);
    lbl_8055F174 = 1.0f;
    fn_801331C4(4, "video_volume", &lbl_8055F174);
    lbl_8055F178 = 1;
    fn_801332D0(4, "race_music_enabled", &lbl_8055F178);
}

int fn_8023CB54(void) {
    fn_801330E8(4);
    fn_8023CA54();
    return 0;
}

int fn_8023CB80(void) {
    fn_80236900();
    return 1;
}

static const MenuDesc lbl_802FA090 = {"imgtest.lyt", 0, 0, 0, 0};

void fn_8023CBA4(void) {
    MenuItem items[] = {
        MenuItem(lbl_8055B860, 6, 0, -1, 0x1B),
        MenuItem(lbl_8055B860, 6, 0, -2, 0xD),
        *(MenuItem*)&lbl_804987A0,
    };
    MenuDesc desc = lbl_802FA090;
    desc.items = items;
    fn_802146BC(&desc, -999, -999, 1);
}

void fn_8023CC84(const char* layout) {
    static char qualities[] = "Low\0Medium\0High\0";
    static char raceMusic[] = "In-Game Music Off\0In-Game Music On\0\0";
    fn_8023CA54();
    if (!lbl_8055F160) {
        lbl_8055F164 = 0.0f;
    }
    {
        s32 mixer = fn_80208D70();
        char mixerName[0x100];
        SoundOptions opts(&lbl_8055F16C, &lbl_8055F164, &lbl_8055F17C, &mixer, mixerName, &lbl_8055F168,
                          &lbl_8055F170, &lbl_8055F174);
        MenuStrList list;
        int i;
        fn_8022D228(&list, 0x11, 0x20);
        for (i = 0; i < 0x11; i++) {
            fn_8022D350(&list, "SOUND", 0);
        }
        {
            MenuItem items[] = {
                MenuItem(lbl_8055BAC0, 8, (void*)fn_8023CA18, 0, 0),
                MenuItem(0, 5, &opts, 0, 0),
                MenuItem("default", 0, (void*)fn_8023CB54, 0, 0),
                MenuItem("video_volume", &lbl_8055F174, MenuRange(0.0f, 1.0f)),
                MenuItem("sfx_volume", &lbl_8055F16C, MenuRange(0.0f, 1.0f)),
                MenuItem("announcer_volume", &lbl_8055F168, MenuRange(0.0f, 1.0f)),
                MenuItem("music_volume", &lbl_8055F164, MenuRange(0.0f, 1.0f)),
                MenuItem("tire_volume", &lbl_8055F170, MenuRange(0.0f, 1.0f)),
                MenuItem("back", 0, 0, -1, 0),
                MenuItem("quality", 7, &lbl_8055F17C, (int)qualities, 0),
                MenuItem("race_music", 7, &lbl_8055F178, (int)raceMusic, 0),
                MenuItem("mixer", 2, &mixer, 0, 0),
                MenuItem("mixer_name", 3, mixerName, 0x100, 0),
                MenuItem("circle_list", 10, &list, 0, 0),
                MenuItem(lbl_8055B860, 6, (void*)fn_8023CB80, -1, 0x1B),
                MenuItem(lbl_8055BABC, 8, (void*)fn_8023CA50, 1, 0),
                MenuItem(lbl_8055BAB8, 8, (void*)fn_80236914, 2, 0),
                *(MenuItem*)&lbl_804987A0,
            };
            MenuDesc desc = {layout, items, 0, 0, 0};
            if (fn_802146BC(&desc, -999, -999, 1) == -2) {
                fn_8023CBA4();
            }
        }
        lbl_8055F160 = lbl_8055F164 > 0.01f;
        fn_8013323C(4, "music_volume", lbl_8055F164);
        fn_80133460(4, "music_enabled", lbl_8055F160);
        fn_8013323C(4, "spotter_volume", lbl_8055F168);
        fn_8013323C(4, "tire_volume", lbl_8055F170);
        fn_8013323C(4, "video_volume", lbl_8055F174);
        fn_80133348(4, "race_music_enabled", lbl_8055F178);
        fn_8022D2BC(&list, -1);
    }
}
}

extern "C" void fn_8023D1C0(void) {
    lbl_8055BB68 = 0;
    fn_8023CC84("sound.lyt");
    lbl_8055BB68 = 2;
}

void SoundOptions::Unk24(void) {}

void SoundOptions::Enter(void) {
    fn_80221EA0(this, 2, mixer, 4);
    fn_80221EA0(this, 0, fx, 4);
    fn_80221EA0(this, 1, quality, 4);
    fn_80221EA0(this, 3, music, 4);
    fn_80221EA0(this, 4, announcer, 4);
    fn_80221EA0(this, 5, tire, 4);
    fn_80221EA0(this, 6, video, 4);
    UpdateMixerName();
}

void SoundOptions::Changed(int item) {
    switch (item) {
    case 2:
        if (*mixer >= fn_80208D3C()) {
            *mixer = fn_80208D3C() - 1;
        }
        fn_80208D60(*mixer);
        UpdateMixerName();
        break;
    case 3:
        fn_8020A5F8(*music);
        break;
    case 0:
        fn_80209030(*fx);
        break;
    case 1:
        fn_80209090(*quality);
        break;
    case 4:
        if (sound) {
            sound->SetVolume(*announcer);
            sound->Stop();
            sound->Play();
        }
        break;
    case 5:
    case 6:
        break;
    }
}
