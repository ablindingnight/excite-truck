// UI objects (0x80258354-0x802590F8, "UIObjects_impl"): a menu's named images, text styles and sounds,
// each in a fixed list of 32 entries looked up by case-insensitive name, with drawing at an anchored
// position, text style setup and sound playback. Names are guesses apart from the tags.
#include "game/common.h"
#include "game/gart.h"
#include "game/sound.h"
#include "game/arrayref.h"
#include "game/strcopy.h"

void* operator new(size_t size, const char* tag);

extern "C" {
int strcasecmp(const char* a, const char* b);
struct GTextFont;
GTextFont* fn_80232578(const char* name);  // load font
void fn_802328D4(GTextFont* font);
int fn_80232B70(GTextFont* font);  // font height
f32 fn_80209028(void);      // effects volume
}

extern "C" Sound2D* fn_80207848(const char* name, int flags);

template <typename T, int N>
struct FixedArr : ArrayRef<T> {
    FixedArr(void) : ArrayRef<T>(storage, N) {}
    ~FixedArr(void) {}
    T storage[N];
};

static inline BOOL NameEq(const char* a, const char* b) { return strcasecmp(a, b) == 0; }

// Copies at most n - 1 characters and always terminates.

struct TextStyle {
    TextStyle(const char* file) {
        font = 0;
        color = -1;
        flags = 9;
        scale = 1.0f;
        alpha = 1.0f;
        font = fn_80232578(file);
    }
    ~TextStyle(void) {
        fn_802328D4(font);
        font = 0;
    }
    GTextFont* font;  // offset 0x0
    u32 color;   // offset 0x4
    u32 flags;   // offset 0x8
    f32 scale;   // offset 0xC
    f32 alpha;   // offset 0x10
};

// A named image resource (not uiwidget.h's UIImage widget). Name unproven.
struct UIImageDef {
    char name[0x20];  // offset 0x0
    char file[0x20];  // offset 0x20
    gArtSet* img;     // offset 0x40
    int frame;        // offset 0x44
};

struct UIFont {
    char name[0x20];   // offset 0x0
    TextStyle* style;  // offset 0x20
};

struct UISound {
    char name[0x20];  // offset 0x0
    Sound2D* snd;     // offset 0x20
    u8 loop;          // offset 0x24
    f32 volume;       // offset 0x28
};

class UIObjectsImpl {
public:
    virtual ~UIObjectsImpl(void);
    virtual void AddImage(const char* name, const char* file, int frame);
    virtual void AddFont(const char* name, const char* file);
    virtual void AddSound(const char* name, const char* file, u8 loop);
    virtual void DrawImage(const char* name, u32 flags, int a, f32 x, f32 y);
    virtual void DrawImageScaled(const char* name, u32 flags, int a, f32 x, f32 y, f32 scale);
    virtual f32 GetWidth(const char* name);
    virtual f32 GetHeight(const char* name);
    virtual const char* GetFile(const char* name);
    virtual int GetFrame(const char* name);
    virtual TextStyle* GetStyle(const char* name);
    virtual TextStyle* SetStyle(const char* name, u32 flags, int size, u32 color);
    virtual void PlaySound(const char* name);
    virtual void StopSound(const char* name);
    virtual void SetSoundVolume(const char* name, f32 volume);

    int FindImage(const char* name) {
        for (int i = 0; i < images.count; i++) {
            if (NameEq(images.data[i].name, name)) {
                return i;
            }
        }
        return -1;
    }
    UIImageDef* Image(int i) { return &images.data[i]; }
    UISound* Sound(int i) { return &sounds.data[i]; }
    int FindFont(const char* name) {
        for (int i = 0; i < fonts.count; i++) {
            if (NameEq(fonts.data[i].name, name)) {
                return i;
            }
        }
        return -1;
    }
    int FindSound(const char* name) {
        for (int i = 0; i < sounds.count; i++) {
            if (NameEq(sounds.data[i].name, name)) {
                return i;
            }
        }
        return -1;
    }

    FixedArr<UIImageDef, 32> images;  // offset 0x4
    FixedArr<UIFont, 32> fonts;    // offset 0x910
    FixedArr<UISound, 32> sounds;  // offset 0xD9C
};

ModuleTag lbl_8055F378;
static UnkStatic123 lbl_80498E80;
UnkStatic7F lbl_8055F380;

extern "C" UIObjectsImpl* fn_80258354(void) { return new ("UIObjects_impl") UIObjectsImpl; }

UIObjectsImpl::~UIObjectsImpl(void) {
    for (int i = 0; i < images.count; i++) {
        fn_80235380(images.data[i].img);
    }
    images.count = 0;
    for (int i = 0; i < fonts.count; i++) {
        delete fonts.data[i].style;
    }
    fonts.count = 0;
    for (int i = 0; i < sounds.count; i++) {
        delete sounds.data[i].snd;
    }
    sounds.count = 0;
}

void UIObjectsImpl::AddImage(const char* name, const char* file, int frame) {
    UIImageDef* e = &images.data[images.count++];
    SafeStrCopy(e->name, name, 0x20);
    SafeStrCopy(e->file, file, 0x20);
    e->img = fn_8023512C(file);
    e->frame = frame;
}

void UIObjectsImpl::AddFont(const char* name, const char* file) {
    UIFont* e = &fonts.data[fonts.count++];
    SafeStrCopy(e->name, name, 0x20);
    e->style = new ("gText") TextStyle(file);
}

void UIObjectsImpl::AddSound(const char* name, const char* file, u8 loop) {
    UISound* e = &sounds.data[sounds.count++];
    SafeStrCopy(e->name, name, 0x20);
    e->snd = fn_80207848(file, 0);
    e->volume = 1.0f;
    e->loop = loop;
}

void UIObjectsImpl::DrawImage(const char* name, u32 flags, int a, f32 x, f32 y) {
    int i = FindImage(name);
    if (i >= 0) {
        UIImageDef* e = Image(i);
        f32 w = fn_80235EF0(e->img, e->frame);
        f32 h = GetArtHeight(e->img, e->frame);
        if (flags & 2) {
            x -= w;
        } else if (flags & 4) {
            x -= 0.5f * w;
        }
        if (flags & 0x40) {
            y += h;
        } else if (flags & 0x10) {
            y += 0.5f * h;
        }
#ifdef __MWERKS__
        // Retail: this unit's prototype put idx (frame) after the floats; on PowerPC it reaches
        // fn_802354A0(s, x, y, z, idx, w, h, color) unchanged (separate int/float registers).
        ((void (*)(gArtSet*, f32, f32, f32, f32, f32, int, int))fn_802354A0)(e->img, x, y, 0.0f, w, h, e->frame, a);
#else
        /* PORT: MWCC path keeps retail's argument order (registers), see the comment above */
        fn_802354A0(e->img, x, y, 0.0f, e->frame, w, h, a);
#endif
    }
}

void UIObjectsImpl::DrawImageScaled(const char* name, u32 flags, int a, f32 x, f32 y, f32 scale) {
    int i = FindImage(name);
    if (i >= 0) {
        UIImageDef* e = Image(i);
        f32 w = scale * fn_80235EF0(e->img, e->frame);
        f32 h = scale * GetArtHeight(e->img, e->frame);
        if (flags & 2) {
            x -= w;
        } else if (flags & 4) {
            x -= 0.5f * w;
        }
        if (flags & 0x40) {
            y += h;
        } else if (flags & 0x10) {
            y += 0.5f * h;
        }
#ifdef __MWERKS__
        // Retail: this unit's prototype put idx (frame) after the floats; on PowerPC it reaches
        // fn_802354A0(s, x, y, z, idx, w, h, color) unchanged (separate int/float registers).
        ((void (*)(gArtSet*, f32, f32, f32, f32, f32, int, int))fn_802354A0)(e->img, x, y, 0.0f, w, h, e->frame, a);
#else
        /* PORT: MWCC path keeps retail's argument order (registers), see the comment above */
        fn_802354A0(e->img, x, y, 0.0f, e->frame, w, h, a);
#endif
    }
}

f32 UIObjectsImpl::GetWidth(const char* name) {
    int i = FindImage(name);
    if (i >= 0) {
        UIImageDef* e = Image(i);
        return fn_80235EF0(e->img, e->frame);
    }
    return 0.0f;
}

f32 UIObjectsImpl::GetHeight(const char* name) {
    int i = FindImage(name);
    if (i >= 0) {
        UIImageDef* e = Image(i);
        return GetArtHeight(e->img, e->frame);
    }
    return 0.0f;
}

const char* UIObjectsImpl::GetFile(const char* name) {
    int i = FindImage(name);
    if (i >= 0) {
        return images.data[i].file;
    }
    return NULL;
}

int UIObjectsImpl::GetFrame(const char* name) {
    int i = FindImage(name);
    if (i >= 0) {
        return images.data[i].frame;
    }
    return 0;
}

TextStyle* UIObjectsImpl::GetStyle(const char* name) {
    int i = FindFont(name);
    if (i >= 0) {
        return fonts.data[i].style;
    }
    return NULL;
}

TextStyle* UIObjectsImpl::SetStyle(const char* name, u32 flags, int size, u32 color) {
    int i = FindFont(name);
    if (i >= 0) {
        TextStyle* s = fonts.data[i].style;
        s->flags = (s->flags & ~7) | (flags & 7);
        s->flags = (s->flags & ~0x58) | (flags & 0x78);
        int h = fn_80232B70(s->font);
        s->scale = (f32)size / (f32)h;
        s->color = color;
        return s;
    }
    return NULL;
}

void UIObjectsImpl::PlaySound(const char* name) {
    int i = FindSound(name);
    if (i >= 0) {
        UISound* e = Sound(i);
        if (e->snd) {
            e->snd->SetVolume(e->volume * fn_80209028());
            if (e->loop) {
                e->snd->PlayLoop();
            } else {
                if (e->snd->playing) {
                    e->snd->Restart();
                }
                e->snd->Play();
            }
        }
    }
}

void UIObjectsImpl::StopSound(const char* name) {
    int i = FindSound(name);
    if (i >= 0) {
        Sound2D* s = sounds.data[i].snd;
        if (s) {
            s->Stop();
        }
    }
}

void UIObjectsImpl::SetSoundVolume(const char* name, f32 volume) {
    int i = FindSound(name);
    if (i >= 0) {
        sounds.data[i].volume = volume;
    }
}
