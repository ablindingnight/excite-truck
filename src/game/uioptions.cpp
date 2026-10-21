// Options menu (0x802399AC-0x8023B5E0, "options.lyt" / "options_uni.lyt"): the main options screen
// (sound, screen, controller, extras, soundtracks, credits, aspect ratio, audio, erase data, units,
// stats) and the universal options page with its widgets: aspect ratio and audio mode lists, music and
// sound effect sliders, the track music choice, the icon set and the erase-data button. Names are
// guesses apart from the strings.
#include "game/common.h"
#include <stdarg.h>

void* operator new[](size_t size, const char* tag);

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

    s32 x;      // offset 0x4
    s32 y;      // offset 0x8
    s32 w;      // offset 0xC
    s32 h;      // offset 0x10
    int unk14;  // offset 0x14
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

int IdentGetCurrentSlot(void);
const char* IdentGetSlotName(int slot);
void IdentClearSlot(void);

extern "C" {
int fn_8027B9EC(int a);
int fn_80214A38(int sound);
void fn_80214A9C(int sound);
void fn_80214AF0(int sound);
void fn_80214B44(int sound);
void fn_80214B94(int sound);
int fn_80214D04(void);
void fn_80214CB4(int a);
const char* fn_80131234(const char* key);  // localized string
int fn_8021521C(const char* head, const char* body, int a);
int fn_802146BC(const MenuDesc* desc, int x, int y, int a);
void fn_80236914(void);
void fn_8023FCFC(int slot);
void fn_8023D1C0(void);
void fn_8023C958(void);
void fn_8023C94C(void);
void fn_802E4A10(void);
void fn_8023D4B8(void);
void fn_8023C3D4(void);
void fn_80278A24(void);
void fn_802449A0(void);
void fn_8024C070(void);
void fn_8024C370(void);
void fn_80257284(void);
void fn_802E317C(const char* fmt, ...);
void fn_802E30C8(const char* fmt, ...);
int fn_802104D0(void);
s32 fn_80210570(void);
void fn_80210514(s32 mode);
void fn_80209030(f32 v);
void fn_8020A5F8(f32 v);
void fn_802C82C4(s32 mode);
void fn_802090BC(s32 mode);
s32 fn_802C83C4(void);
s32 fn_802090CC(void);
void fn_801331C4(int section, const char* key, f32* v);
void fn_8013323C(int section, const char* key, f32 v);
void fn_801332D0(int section, const char* key, s32* v);
void fn_80133348(int section, const char* key, s32 v);
u8 fn_8027B8C0(int a);
int fn_802E8D84(void);
void fn_802E6FBC(void);
void fn_8027B77C(void);
void fn_8027B690(void);
void fn_8026311C(void);
void fn_80263F30(int a, int b, int c);
void fn_80230E90(int a);
void fn_802168BC(int a);
void fn_8022BAD0(int id, int x, int y, int z, int w, int h, const char* s, int sel, f32 scale, int extra);
void* fn_80232578(const char* font);
void fn_802328D4(void* font);
int fn_802399DC(int unused);
int fn_802399AC(void);
extern char lbl_8055BAC0[8];
extern char lbl_8055BABC[4];
extern char lbl_8055BAB8[4];
extern char lbl_8055B860[8];
}

// The options values behind the universal page, with the values on entry.
struct OptValues {
    OptValues(void) {}
    s32 aspect;     // offset 0x0
    s32 aspectOld;  // offset 0x4
    s32 audio;      // offset 0x8
    s32 audioOld;   // offset 0xC
    f32 music;      // offset 0x10
    f32 sfx;        // offset 0x14
    s32 track;      // offset 0x18
    s32 trackOld;   // offset 0x1C
    s32 icon;       // offset 0x20
};

struct OptState {
    OptValues cur;    // offset 0x0
    OptValues saved;  // offset 0x24
};

// A list of choices drawn by the layout (vptr after the base data).
class ChoiceBase {
public:
    ChoiceBase(void) {
        b0 = 0;
        f4 = -1;
        b8 = 0;
    }
    u8 b0;
    s32 f4;
    u8 b8;
    virtual void Base08(void);
    virtual void Base0C(void);
    virtual void Base10(void);
    virtual void Base14(void);
};

// A choice list drawn with its own font (the aspect ratio).
class VideoList : public MenuState {
public:
    VideoList(s32* v, const char* s, ...) {
        va_list ap;
        int i;
        font = fn_80232578("uifnt_o.gtx");
        dirty = 0;
        va_start(ap, s);
        count = 0;
        do {
            count++;
        } while (va_arg(ap, const char*));
        strs = new ("VideoListCustom") const char*[count];
        va_start(ap, s);
        i = 0;
        do {
            strs[i++] = s;
            s = va_arg(ap, const char*);
        } while (s);
        value = v;
    }
    virtual ~VideoList(void) {
        delete[] strs;
        fn_802328D4(font);
        font = 0;
    }
    virtual void Draw(void) {}
    virtual void Prev(void) {
        dirty = 1;
        *value = (count + *value - 1) % count;
    }
    virtual void Next(void) {
        dirty = 1;
        *value = (*value + 1) % count;
    }
    virtual int HasPrev(void) { return *value > 0; }
    virtual int HasNext(void) { return *value < count - 1; }
    virtual const char* Get(void) { return strs[*value]; }

    const char** strs;  // offset 0x18
    s32 count;          // offset 0x1C
    s32* value;         // offset 0x20
    void* font;         // offset 0x24
    u8 dirty;           // offset 0x28
};

// A list of localized choices for one value.
class ChoiceList : public ChoiceBase {
public:
    ChoiceList(s32* v, const char* s, ...) {
        va_list ap;
        int i;
        va_start(ap, s);
        count = 0;
        do {
            count++;
        } while (va_arg(ap, const char*));
        strs = new ("StringMultiCustom") const char*[count];
        va_start(ap, s);
        i = 0;
        do {
            strs[i++] = s;
            s = va_arg(ap, const char*);
        } while (s);
        value = v;
    }
    virtual ~ChoiceList(void) { delete[] strs; }
    virtual void Prev(void) { *value = (count + *value - 1) % count; }
    virtual void Next(void) { *value = (*value + 1) % count; }
    virtual int HasPrev(void) { return *value > 0; }
    virtual int HasNext(void) { return *value < count - 1; }
    virtual const char* Get(void) { return strs[*value]; }

    const char** strs;  // offset 0x10
    s32 count;          // offset 0x14
    s32* value;         // offset 0x18
};

// A choice list whose visible length can be cut down (the track music list).
class ChoiceListVar : public ChoiceBase {
public:
    ChoiceListVar(s32* v, const char* s, ...) {
        va_list ap;
        int i;
        va_start(ap, s);
        count = 0;
        do {
            count++;
        } while (va_arg(ap, const char*));
        strs = new ("StringMultiCustom") const char*[count];
        va_start(ap, s);
        i = 0;
        do {
            strs[i++] = s;
            s = va_arg(ap, const char*);
        } while (s);
        total = count;
        value = v;
    }
    virtual ~ChoiceListVar(void) { delete[] strs; }
    virtual void Prev(void) { *value = (count + *value - 1) % count; }
    virtual void Next(void) { *value = (*value + 1) % count; }
    virtual int HasPrev(void) { return *value > 0; }
    virtual int HasNext(void) { return *value < count - 1; }
    virtual const char* Get(void) { return strs[*value]; }
    virtual void SetCount(s32 n) { count = n; }
    virtual void Set(s32 v) { *value = v; }
    virtual s32 Value(void) { return *value; }

    const char** strs;  // offset 0x10
    s32 count;          // offset 0x14
    s32 total;          // offset 0x18
    s32* value;         // offset 0x1C
};

// The erase-data button: armed while the layout's cursor is on it.
class EraseButton : public MenuState {
public:
    EraseButton(void) {
        sel = 0;
        armed = 0;
    }
    virtual void Enter(void) { f18 = -1; }
    virtual void Update(void) {
        u32 on = fn_80214D04() == 4;
        if (armed != on) {
            fn_80230E90(0);
            if (on) {
                sel = 1;
            } else {
                sel = 0;
            }
            armed = on;
        }
    }
    virtual int Key(int key) {
        switch (key) {
        case 0xD:
            if (armed && fn_802399DC(0)) {
                fn_802168BC(6);
                break;
            }
            return 0;
        default:
            return 0;
        }
        return 1;
    }
    virtual void Draw(void) {
        s32 h0 = h;
        s32 w0 = w;
        s32 y0 = y;
        s32 x0 = x;
        fn_8022BAD0(0xCB, x0, y0, 0, w0, h0, fn_80131234("UIOptions:ERASEDATA"), sel, 1.0f, 0);
    }
    virtual int Armed(void);

    s32 f18;   // offset 0x18
    s32 sel;   // offset 0x1C
    u8 armed;  // offset 0x20
};

ModuleTag lbl_8055F100;
static UnkStatic123s lbl_804986F8;
static OptState lbl_8049870C;
s32 lbl_8055BAC8 = -1;
UnkStatic7F lbl_8055F108;
EraseButton* lbl_8055F10C;

// The music/sound/icon caption widgets share this base.
class OptCaption : public MenuState {
public:
    OptCaption(void) { sel = 0; }
    virtual void Enter(void) {}
    virtual void Exit(void) {}
    virtual void Update(void) {}
    virtual void Draw(void) {}
    virtual int Key(int key) { return 0; }

    s32 sel;  // offset 0x18
};

class SfxCaption : public OptCaption {
public:
    virtual void Update(void) {
        if (!lbl_8055F10C->Armed()) {
            switch (fn_80214D04()) {
            case 1:
                sel = 1;
                break;
            default:
                sel = 0;
                break;
            }
        }
    }
    virtual void Draw(void) {
        s32 h0 = h;
        s32 w0 = w;
        s32 y0 = y;
        s32 x0 = x;
        fn_8022BAD0(0xC9, x0, y0, 0, w0, h0, fn_80131234("UIOptions:SFXVOLUME"), sel, 1.0f, 0);
    }
};

class MusicCaption : public OptCaption {
public:
    virtual void Update(void) {
        if (!lbl_8055F10C->Armed()) {
            switch (fn_80214D04()) {
            case 0:
                sel = 1;
                break;
            default:
                sel = 0;
                break;
            }
        }
    }
    virtual void Draw(void) {
        s32 h0 = h;
        s32 w0 = w;
        s32 y0 = y;
        s32 x0 = x;
        fn_8022BAD0(0xC9, x0, y0, 0, w0, h0, fn_80131234("UIOptions:MUSICVOLUME"), sel, 1.0f, 0);
    }
};

class IconCaption : public OptCaption {
public:
    IconCaption(void) { special = 0; }
    virtual void Enter(void) {
        if (fn_802399AC() || special) {
            fn_80214B94(6);
            fn_80214AF0(6);
        } else {
            fn_80214B44(6);
            fn_80214A9C(6);
        }
    }
    virtual void Update(void) {
        if (!lbl_8055F10C->Armed()) {
            switch (fn_80214D04()) {
            case 3:
                sel = 1;
                break;
            default:
                sel = 0;
                break;
            }
        }
    }
    virtual void Draw(void) {
        s32 h0 = h;
        s32 w0 = w;
        s32 y0 = y;
        s32 x0 = x;
        fn_8022BAD0(0xC9, x0, y0, 0, w0, h0, fn_80131234("UIOptions:ICONS"), sel, 1.0f, 0);
    }

    u8 special;  // offset 0x1C
};

// The race music caption: hides the track music choice while custom soundtracks are off.
class TrackCaption : public MenuState {
public:
    TrackCaption(ChoiceListVar* l) {
        list = l;
        enabled = 1;
    }
    virtual void Enter(void) {
        s32 m = fn_80210570();
        if (m == 2) {
            list->SetCount(3);
        } else {
            list->SetCount(2);
        }
        list->Set(m);
    }
    virtual void Exit(void) {}
    virtual int Key(int key) { return 0; }
    virtual void Update(void) {
        u32 en = fn_802104D0();
        if (en == 1) {
            fn_80214B94(3);
        } else {
            list->Set(0);
            if (fn_80214D04() == 2) {
                fn_80214CB4(1);
            }
            fn_80214B44(3);
        }
        if (en == 0) {
            sel = 4;
        } else {
            switch (fn_80214D04()) {
            case 2:
                sel = 1;
                break;
            default:
                sel = 0;
                break;
            }
        }
        if (enabled != en && en != 0) {
            list->Set(fn_80210570());
        }
        enabled = en;
    }
    virtual void Draw(void) {
        s32 h0 = h;
        s32 w0 = w;
        s32 y0 = y;
        s32 x0 = x;
        fn_8022BAD0(0xC9, x0, y0, 0, w0, h0, fn_80131234("UIOptions:RACE_MUSIC"), sel, 1.0f, 0);
    }

    s32 sel;              // offset 0x18
    ChoiceListVar* list;  // offset 0x1C
    u8 enabled;           // offset 0x20
};

class SoundCaption;
SoundCaption* lbl_8055F110;

// Plays the "sound on" cue on entry; Dolby when the audio mode allows it.
class SoundCaption : public MenuState {
public:
    SoundCaption(void) {
        lbl_8055F110 = this;
        dolby = fn_8027B8C0(1);
    }
    virtual ~SoundCaption(void) { lbl_8055F110 = 0; }
    virtual void Enter(void) {
        if (dolby) {
            fn_80214AF0(0x1D);
        } else {
            fn_80214A9C(0x1D);
        }
    }
    static int Dolby(void) {
        if (lbl_8055F110 && lbl_8055F110->dolby) {
            return 1;
        }
        return 0;
    }

    u8 dolby;  // offset 0x18
};

static const MenuDesc lbl_802FA040 = {"options.lyt", 0, 0, 0, 0};

extern "C" {

int fn_802399AC(void) { return fn_8027B9EC(1) != 0; }

int fn_802399DC(int unused) {
    int a = fn_80214A38(0x1E);
    int b = fn_80214A38(0x1D);
    const char* body;
    int r;
    if (a) {
        fn_80214A9C(0x1E);
    }
    if (b) {
        fn_80214A9C(0x1D);
    }
    body = fn_80131234("UIOptions:CONFIRM_ERASE_BODY");
    r = fn_8021521C(IdentGetSlotName(IdentGetCurrentSlot()), body, 0);
    if (b) {
        fn_80214AF0(0x1D);
    }
    if (a) {
        fn_80214AF0(0x1E);
    }
    return r;
}

void fn_80239A94(void) { lbl_8055BAC8 = fn_80214D04(); }

void fn_80239AB8(void) { fn_80214CB4(lbl_8055BAC8); }

void fn_80239AC0(void) {
    int done = 0;
    MenuDesc desc;
    do {
        MenuItem items[] = {
            MenuItem(lbl_8055BAC0, 8, (void*)fn_80239AB8, 0, 0),
            MenuItem(lbl_8055BABC, 8, (void*)fn_80239A94, 1, 0),
            MenuItem(lbl_8055B860, 6, 0, -1, 0x1B),
            MenuItem("volume", 0, 0, 0, 0),
            MenuItem("screen", 0, 0, 8, 0),
            MenuItem("controller", 0, 0, 3, 0),
            MenuItem("extras", 0, 0, 10, 0),
            MenuItem("soundtracks", 0, 0, 11, 0),
            MenuItem("credits", 0, 0, 16, 0),
            MenuItem("aspectratio", 0, 0, 4, 0),
            MenuItem("audioquality", 0, 0, 5, 0),
            MenuItem("erasedata", 0, (void*)fn_802399DC, 6, 0),
            MenuItem("units", 0, 0, 12, 0),
            MenuItem("stats", 0, 0, 13, 0),
            MenuItem(lbl_8055BAB8, 8, (void*)fn_80236914, 2, 0),
            *(MenuItem*)&lbl_804986F8,
        };
        desc = lbl_802FA040;
        desc.items = items;
        switch (fn_802146BC(&desc, -999, -999, 1)) {
        case 15:
            fn_8023FCFC(IdentGetCurrentSlot());
            break;
        case 0:
            fn_8023D1C0();
            break;
        case 4:
            fn_8023C958();
            break;
        case 6:
            IdentClearSlot();
            done = 1;
            break;
        case 5:
            fn_8023C94C();
            break;
        case 3:
            fn_802E4A10();
            fn_8023D4B8();
            break;
        case 8:
            fn_8023C3D4();
            break;
        case 16:
            fn_80278A24();
            break;
        case 9:
            fn_802449A0();
            break;
        case 12:
            fn_8024C070();
            break;
        case 11:
            fn_8024C370();
            break;
        case 13:
            fn_80257284();
            break;
        case -2:
        case -1:
            done = 1;
            break;
        case 1:
        case 2:
        case 7:
        case 10:
        case 14:
            break;
        default:
            fn_802E317C("bad options return");
            break;
        }
    } while (!done);
}

int fn_80239DE4(void) {
    if (lbl_8049870C.cur.track != lbl_8049870C.cur.trackOld && fn_802104D0()) {
        const char* body = fn_80131234("TrackMusic:CONFIRM_CHANGE_BODY");
        return fn_8021521C(fn_80131234("TrackMusic:CONFIRM_CHANGE_HEAD"), body, 0);
    }
    return 1;
}

void fn_80239E5C(const OptValues* v, int force) {
    fn_80209030(v->sfx);
    fn_8020A5F8(v->music);
    if (force || v->aspect != v->aspectOld) {
        fn_802E30C8("WSM: %d", v->aspect);
        fn_802C82C4(v->aspect);
    }
    if (force || v->audio != v->audioOld) {
        fn_802090BC(v->audio);
    }
}

int fn_80239EF0(int* result) {
    fn_80239E5C(&lbl_8049870C.cur, 0);
    lbl_8049870C.cur.aspectOld = lbl_8049870C.cur.aspect;
    lbl_8049870C.cur.audioOld = lbl_8049870C.cur.audio;
    if (lbl_8055F10C->Armed()) {
        fn_80214AF0(0x1E);
    } else {
        fn_80214A9C(0x1E);
    }
    return 0;
}
}

int EraseButton::Armed(void) { return armed; }

static const MenuDesc lbl_802FA054 = {"options_uni.lyt", 0, fn_80239EF0, 0, 0};

extern "C" void fn_80239F74(void) {
    OptState* st = &lbl_8049870C;
    int run;
    lbl_8049870C.cur.aspectOld = lbl_8049870C.cur.aspect = fn_802C83C4();
    {
        VideoList aspect(&st->cur.aspect, fn_80131234("Display:STANDARD"), fn_80131234("Display:WIDESCREEN"), 0);
        lbl_8049870C.cur.audioOld = lbl_8049870C.cur.audio = fn_802090CC();
        {
            ChoiceList audio(&st->cur.audio, fn_80131234("Audio:MONO"), fn_80131234("Audio:STEREO"),
                             fn_80131234("Audio:DOLBY_PRO_LOGIC_II"), 0);
            lbl_8049870C.cur.trackOld = lbl_8049870C.cur.track = fn_80210570();
            {
                ChoiceListVar track(&st->cur.track, fn_80131234("TrackMusic:DEFAULT_ALL"),
                                    fn_80131234("TrackMusic:RANDOM_ALL"), fn_80131234("TrackMusic:CUSTOMIZED"), 0);
                lbl_8049870C.cur.music = 1.0f;
                fn_801331C4(4, "music_volume", &st->cur.music);
                lbl_8049870C.cur.sfx = 1.0f;
                fn_801331C4(4, "effects_volume", &st->cur.sfx);
                {
                    EraseButton erase;
                    lbl_8055F10C = &erase;
                    lbl_8049870C.cur.icon = 0;
                    fn_801332D0(IdentGetCurrentSlot() + 0xC, "icon_set", &st->cur.icon);
                    {
                        ChoiceList icons(&st->cur.icon, fn_80131234("IconSet:NORMAL"),
                                         fn_80131234("IconSet:SPECIAL1"), 0);
                        SfxCaption sfx;
                        MusicCaption music;
                        TrackCaption trackCap(&track);
                        IconCaption iconCap;
                        SoundCaption sound;
                        MenuItem items[] = {
                            MenuItem(0, 5, &sound, 0, 0),
                            MenuItem("cvideo", 5, &aspect, 0, 0),
                            MenuItem("music_volume", &st->cur.music, MenuRange(0.0f, 1.0f)),
                            MenuItem("mvcustom", 5, &music, 0, 0),
                            MenuItem("sfx_volume", &st->cur.sfx, MenuRange(0.0f, 1.0f)),
                            MenuItem("sfxcustom", 5, &sfx, 0, 0),
                            MenuItem("audio", 16, 0, (int)&track, 0),
                            MenuItem("ascustom", 5, &trackCap, 0, 0),
                            MenuItem("iconset", 16, 0, (int)&icons, 0),
                            MenuItem("iconcustom", 5, &iconCap, 0, 0),
                            MenuItem("cerase", 5, &erase, 0, 0),
                            MenuItem("decide", 0, 0, -2, 0),
                            MenuItem(lbl_8055B860, 6, (void*)fn_80239DE4, -1, 0x1B),
                            MenuItem(lbl_8055B860, 6, (void*)SoundCaption::Dolby, 0x10, 0x121),
                            *(MenuItem*)&lbl_804986F8,
                        };
                        MenuDesc desc = lbl_802FA054;
                        desc.items = items;
                        lbl_8049870C.saved = lbl_8049870C.cur;
                        run = 1;
                        while (run) {
                            switch (fn_802146BC(&desc, -999, -999, 1)) {
                            case -2:
                            case -1:
                                fn_8013323C(4, "music_volume", st->cur.music);
                                fn_8013323C(4, "effects_volume", st->cur.sfx);
                                fn_80133348(IdentGetCurrentSlot() + 0xC, "icon_set", st->cur.icon);
                                if (fn_802104D0()) {
                                    fn_80210514(st->cur.track);
                                }
                                if (fn_802E8D84()) {
                                    fn_802E6FBC();
                                }
                                run = 0;
                                lbl_8055F10C = 0;
                                break;
                            case 6:
                                fn_8027B77C();
                                IdentClearSlot();
                                fn_8027B690();
                                run = 0;
                                break;
                            case 4:
                                aspect.Next();
                                break;
                            case 5:
                                audio.Next();
                                break;
                            case 3:
                                fn_8026311C();
                                break;
                            case 16:
                                fn_80263F30(0, 1, 0);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}
