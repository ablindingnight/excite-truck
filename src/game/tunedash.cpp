// Tune dashboard (0x80204330-0x80204AA4): a debug list of tweakable floats and switches drawn with
// tunedash.val, edited with the arrow keys and faded out after a few seconds without input. Names
// are guesses apart from the file names, element names and messages.
#include "game/common.h"

struct V2 {
    f32 x;
    f32 y;
};

struct TextElem {
    TextElem(void) {}
    const char* name;  // offset 0x0
    V2 pos;            // offset 0x4
    s32 elemJust;      // offset 0xC
    f32 scale;         // offset 0x10
    u32 just;          // offset 0x14
    u32 color;         // offset 0x18
    u32 unk1C;         // offset 0x1C
};

struct Box {
    const char* name;  // offset 0x0
    V2 pos;            // offset 0x4
    s32 c;             // offset 0xC
    f32 w;             // offset 0x10
    f32 h;             // offset 0x14
};

class ILayout {
public:
    virtual ~ILayout(void);
    virtual TextElem* Find(const char* name) = 0;
    virtual void* FindArt(const char* name) = 0;
    virtual Box* FindBox(const char* name) = 0;
    virtual int FindSet(const char* name) = 0;
    virtual void Edit(void) = 0;
};

struct Camera {
    u8 data[0x30];
};

struct PadState {
    u8 pad0[0x20];
    u8 buttons[8];  // offset 0x20
    u8 pad28[0x68 - 0x28];
};

extern "C" {
s32 fn_802E2C20(void);  // milliseconds
ILayout* fn_80145E70(const char* name);
void fn_801476C4(TextElem* e);
void fn_80147230(TextElem* e, const char* fmt, ...);
void fn_801473F8(ILayout* l, const char* name, const char* fmt, ...);
void fn_80230B5C(Camera* cam);
void fn_802C8638(Camera* cam);
void fn_802C974C(f32 a, f32 b, f32 c);
int fn_801298E8(PadState* pad, int chan);
void fn_80011A2C(void);
}

static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

static inline int Min(int a, int b) { return (b < a) ? b : a; }

// A tweakable value: a float stepped between min and max, or an on/off switch.
struct TuneItem {
    const char* name;  // offset 0x0
    int type;          // offset 0x4, 1 = float, 0 = switch
    void* value;       // offset 0x8
    f32 min;           // offset 0xC
    f32 max;           // offset 0x10
    f32 step;          // offset 0x14
};

ModuleTag lbl_8055ED30;
static UnkStatic123 lbl_80488BB8;
UnkStatic7F lbl_8055ED38;
int lbl_8055ED3C;            // item count
int lbl_8055ED40;            // selected item
ILayout* lbl_8055ED44;       // layout
s32 lbl_8055ED48;            // time of the last input
TuneItem lbl_80488BD0[0x40];

extern "C" {

void fn_80204330(void) { lbl_8055ED48 = fn_802E2C20(); }

s32 fn_80204354(void) { return fn_802E2C20() - lbl_8055ED48; }

int fn_8020437C(void) { return fn_80204354() < 3500; }

void fn_802043B0(u32* color) {
    f32 t = ((f32)fn_80204354() - 2500.0f) / 1000.0f;
    t = MIN(1.0f, t);
    t = MAX(t, 0.0f);
    *color &= 0x00FFFFFF;
    *color |= (int)Lerp(255.0f, 0.0f, t) << 24;
}

void fn_80204460(void) {
    lbl_8055ED3C = 0;
    lbl_8055ED40 = 0;
    lbl_8055ED48 = 0;
    lbl_8055ED44 = fn_80145E70("tunedash.val");
}

void fn_8020449C(void) {
    lbl_8055ED3C = 0;
    delete lbl_8055ED44;
    lbl_8055ED44 = NULL;
}

int fn_802044E8(void) {
    Camera cam;
    fn_80230B5C(&cam);
    fn_802C8638(&cam);
    fn_802C974C(1.0f, 1000.0f, 1.4396f);
    lbl_8055ED44->Edit();
    int active = fn_8020437C();
    if (active) {
        if (lbl_8055ED40 >= 0 && lbl_8055ED40 < lbl_8055ED3C) {
            TextElem* n1 = lbl_8055ED44->Find("name1");
            TextElem* n2 = lbl_8055ED44->Find("name2");
            TextElem* v1 = lbl_8055ED44->Find("value1");
            Box* list = lbl_8055ED44->FindBox("listbox");
            if (n1 && n2 && v1 && list) {
                u32 normal = n1->color;
                u32 selected = n2->color;
                fn_802043B0(&normal);
                fn_802043B0(&selected);
                f32 rowH = n2->pos.y - n1->pos.y;
                int end;
                TuneItem* it;
                int rows = (int)(list->h / rowH);
                int first = lbl_8055ED40 - lbl_8055ED40 % rows;
                TextElem name = *n1;
                TextElem value = *v1;
                end = first + rows;
                it = &lbl_80488BD0[first];
                for (int i = first; i < end && i < lbl_8055ED3C; it++, i++) {
                    if (i == lbl_8055ED40) {
                        name.color = value.color = selected;
                    } else {
                        name.color = value.color = normal;
                    }
                    fn_80147230(&name, "%s", it->name);
                    switch (it->type) {
                    case 1:
                        fn_80147230(&value, "%1.2f", *(f32*)it->value);
                        break;
                    case 0:
                        fn_80147230(&value, "%s", *(u8*)it->value ? "on" : "off");
                        break;
                    }
                    name.pos.y += rowH;
                    value.pos.y += rowH;
                }
                if (first >= rows) {
                    fn_801473F8(lbl_8055ED44, "up_arrow", "^");
                }
                if (end < lbl_8055ED3C) {
                    fn_801473F8(lbl_8055ED44, "down_arrow", "v");
                }
            }
        } else {
            fn_801473F8(lbl_8055ED44, "name1", "Nothing to edit");
        }
    } else {
        fn_801476C4(lbl_8055ED44->Find("inactivity_message"));
        PadState pad;
        if (fn_801298E8(&pad, 0) && pad.buttons[5]) {
            fn_80204330();
        }
    }
    fn_80011A2C();
    return active;
}

int fn_80204870(u32 key) {
    if (fn_8020437C()) {
        switch (key) {
        case 0x126:
            lbl_8055ED40 = MAX(0, lbl_8055ED40 - 1);
            break;
        case 0x128:
            lbl_8055ED40 = Min(lbl_8055ED40 + 1, lbl_8055ED3C - 1);
            break;
        case 0x125:
        case 0x127:
            if (lbl_8055ED40 >= 0 && lbl_8055ED40 < lbl_8055ED3C) {
                TuneItem* it = &lbl_80488BD0[lbl_8055ED40];
                switch (it->type) {
                case 1:
                    if (key == 0x127) {
                        *(f32*)it->value = MIN(it->max, *(f32*)it->value + it->step);
                    } else {
                        *(f32*)it->value = MAX(it->min, *(f32*)it->value - it->step);
                    }
                    break;
                case 0:
                    *(bool*)it->value = !*(bool*)it->value;
                    break;
                }
            }
            break;
        default:
            return 0;
        }
        fn_80204330();
        return 1;
    }
    return 0;
}

void fn_802049DC(const char* name, f32* value, f32 min, f32 max, f32 step) {
    if (lbl_8055ED3C < 0x40) {
        TuneItem* it = &lbl_80488BD0[lbl_8055ED3C++];
        it->name = name;
        it->type = 1;
        it->value = value;
        it->min = min;
        it->max = max;
        it->step = step;
    }
}

void fn_80204A1C(const char* name, u8* value) {
    if (lbl_8055ED3C < 0x40) {
        TuneItem* it = &lbl_80488BD0[lbl_8055ED3C++];
        it->name = name;
        it->type = 0;
        it->value = value;
    }
}

void fn_80204A50(void) { fn_80204330(); }

}  // extern "C"

// NonMatching: fn_802044E8 colors its saved registers differently (36 diffs, registers only).
