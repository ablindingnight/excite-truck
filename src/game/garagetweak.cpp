// Garage tweak menu (0x801DDE78-0x801DE7E8): a debug page that lists the tunable values of the first
// local player's car (collected through the car's tweak visitor), lets left/right step them within
// their range, up/down pick one and "RESTORE DEFAULTS" reset them, and draws the list with the
// garage.val layout, fading out a few seconds after the last input. Names are guesses apart from the
// strings.
#include "game/common.h"

struct V2 {
    f32 x;
    f32 y;
};

struct Camera {
    u8 pad[0x30];
};

struct ElemBase {
    ElemBase(void) {}
    const char* name;  // offset 0x0
    V2 pos;            // offset 0x4
    s32 elemJust;      // offset 0xC
};

// A text element of a layout (0x20).
struct TextElem : ElemBase {
    f32 scale;         // offset 0x10
    u32 just;          // offset 0x14
    u32 color;         // offset 0x18
    const char* text;  // offset 0x1C
};

class ILayout {
public:
    virtual ~ILayout(void);
    virtual TextElem* Find(const char* name) = 0;
    virtual void* FindArt(const char* name) = 0;
    virtual void* FindBox(const char* name) = 0;
    virtual int FindSet(const char* name) = 0;
    virtual void Edit(void) = 0;
};

// A tunable value of the car.
struct Tweak {
    const char* name;  // offset 0x0
    const char* unit;  // offset 0x4
    f32* value;        // offset 0x8
    f32 def;           // offset 0xC
    f32 min;           // offset 0x10
    f32 max;           // offset 0x14
    f32 step;          // offset 0x18
};

class TweakVisitor {
public:
    virtual void Add(const char* name, f32* value, const char* unit, f32 min, f32 max, f32 step) = 0;
};

struct Params {
    Params& operator=(const Params& o) {
        a = o.a;
        b = o.b;
        c = o.c;
        d = o.d;
        e = o.e;
        f = o.f;
        g = o.g;
        h = o.h;
        i = o.i;
        return *this;
    }
    f32 a, b, c, d, e, f, g, h, i;
};

struct CarTune {
    u8 pad0[0x80];
    Params cur0;    // offset 0x80
    Params saved0;  // offset 0xA4
    Params cur1;    // offset 0xC8
    Params saved1;  // offset 0xEC
};

struct RaceCar {
    u8 pad0[0x32C];
    CarTune tune;  // offset 0x32C
};

extern "C" {
ILayout* fn_80145E70(const char* name);  // load layout
void fn_80147230(TextElem* e, const char* fmt, ...);
int fn_802E2C20(void);  // milliseconds
int fn_80179630(int view);
void fn_8018A2C0(RaceCar* car, TweakVisitor* v);  // visit the car's tweaks
void fn_8018A180(RaceCar* car);                   // apply the car's tweaks
void fn_80230B5C(Camera* c);
void fn_802C8638(Camera* c);
void fn_802C974C(f32 nearZ, f32 farZ, f32 fov);
void fn_80011A2C(void);
extern RaceCar* lbl_804348E8[8];
void fn_801DDED0(const char* name, f32* value, const char* unit, f32 min, f32 max, f32 step);
}

ModuleTag lbl_8055E9B0;
UnkStatic123 lbl_80486F18;
UnkStatic7F lbl_8055E9B8;
s32 lbl_8055E9BC;      // selected row (-1: restore defaults)
ILayout* lbl_8055E9C0;  // garage.val
s32 lbl_8055E9C4;      // tweak count
RaceCar* lbl_8055E9C8;  // car the list was built for
s32 lbl_8055E9CC;      // time of the last input
Tweak lbl_80486F30[32];

class Collector : public TweakVisitor {
public:
    virtual void Add(const char* name, f32* value, const char* unit, f32 min, f32 max, f32 step) {
        fn_801DDED0(name, value, unit, min, max, step);
    }
};

static inline int Round(f32 x) { return (int)(x < 0.0f ? x - 0.5f : 0.5f + x); }

extern "C" {

RaceCar* fn_801DDE78(void) { return lbl_804348E8[fn_80179630(0)]; }

CarTune* fn_801DDEAC(void) { return &fn_801DDE78()->tune; }

void fn_801DDED0(const char* name, f32* value, const char* unit, f32 min, f32 max, f32 step) {
    Tweak* t = &lbl_80486F30[lbl_8055E9C4++];
    t->name = name;
    t->unit = unit;
    t->value = value;
    t->def = *value;
    t->min = min;
    t->max = max;
    t->step = step;
}

void fn_801DDF0C(void) {
    RaceCar* car = fn_801DDE78();
    if (car != lbl_8055E9C8) {
        lbl_8055E9C8 = car;
        Collector c;
        lbl_8055E9C4 = 0;
        fn_8018A2C0(car, &c);
        for (int i = 0; i < lbl_8055E9C4; i++) {
            Tweak* t = &lbl_80486F30[i];
            *t->value = t->step * (f32)Round(*t->value / t->step);
            t->def = *t->value;
        }
    }
}

void fn_801DDFE8(CarTune* t) {
    t->saved0 = t->cur0;
    t->saved1 = t->cur1;
}

void fn_801DE0A4(void) {
    lbl_8055E9BC = 0;
    lbl_8055E9C8 = 0;
    lbl_8055E9C4 = 0;
    lbl_8055E9C0 = fn_80145E70("garage.val");
    lbl_8055E9CC = fn_802E2C20();
}

void fn_801DE0E8(void) {
    delete lbl_8055E9C0;
    lbl_8055E9C0 = 0;
}

void fn_801DE12C(void) { lbl_8055E9CC = fn_802E2C20(); }

void fn_801DE150(void) {}

void fn_801DE154(void) {
    fn_801DDF0C();
    Camera cam;
    fn_80230B5C(&cam);
    fn_802C8638(&cam);
    fn_802C974C(1.0f, 1000.0f, 1.4396f);
    lbl_8055E9C0->Edit();
    TextElem name = *lbl_8055E9C0->Find("name");
    TextElem name2 = *lbl_8055E9C0->Find("name2");
    V2 step;
    step.x = name2.pos.x - name.pos.x;
    step.y = name2.pos.y - name.pos.y;
    TextElem value = *lbl_8055E9C0->Find("value");
    int t = fn_802E2C20() - lbl_8055E9CC;
    if (t >= 5000) {
        int a = (t - 5000) * 0xC0 / 1000;
        a = a > 0 ? a : 0;
        a = a < 0xC0 ? a : 0xC0;
        name.color -= a << 24;
        name2.color -= a << 24;
        value.color -= a << 24;
    }
    for (int i = -1; i < lbl_8055E9C4; i++) {
        TextElem n = name;
        f32 stepY = step.y;
        n.pos.y += stepY * (f32)i;
        n.pos.x += step.x * (f32)i;
        TextElem v = value;
        v.pos.x += step.x * (f32)i;
        v.pos.y += step.y * (f32)i;
        if (i == lbl_8055E9BC) {
            n.color = v.color = name2.color;
        }
        if (i >= 0) {
            Tweak* tw = &lbl_80486F30[i];
            fn_80147230(&n, "%s", tw->name);
            const char* mark;
            if (Round(*tw->value / tw->step) == Round(tw->def / tw->step)) {
                mark = "";
            } else if (*tw->value > tw->def) {
                mark = " +";
            } else {
                mark = " -";
            }
            fn_80147230(&v, "%1.*f%s%s", tw->step > 0.99f ? 0 : 2, *tw->value, tw->unit, mark);
        } else {
            fn_80147230(&n, "RESTORE DEFAULTS");
            if (i == lbl_8055E9BC) {
                fn_80147230(&v, "(A)");
            }
        }
    }
    fn_80011A2C();
}

static inline int MaxI(int a, int b) { return a > b ? a : b; }
static inline int MinI(int a, int b) { return a < b ? a : b; }
static inline int MinI2(int a, int b) { return b < a ? b : a; }
static inline f32 MaxF(f32 a, f32 b) { return a > b ? a : b; }
static inline f32 MinF(f32 a, f32 b) { return a < b ? a : b; }

int fn_801DE5C4(int key) {
    fn_801DDF0C();
    int handled = 0;
    switch (key) {
    case 0x126:
        handled = 1;
        lbl_8055E9BC = MaxI(lbl_8055E9BC - 1, -1);
        break;
    case 0x128:
        handled = 1;
        lbl_8055E9BC = MinI2(lbl_8055E9C4 - 1, lbl_8055E9BC + 1);
        break;
    case 0x125:
    case 0x127:
        handled = 1;
        if (lbl_8055E9BC >= 0 && lbl_8055E9BC < lbl_8055E9C4) {
            RaceCar* car = fn_801DDE78();
            Tweak* t = &lbl_80486F30[lbl_8055E9BC];
            if ((u32)key == 0x127) {
                *t->value = MinF(t->max, *t->value + t->step);
            } else {
                *t->value = MaxF(t->min, *t->value - t->step);
            }
            fn_801DDFE8(fn_801DDEAC());
            fn_8018A180(car);
        }
        break;
    case 0x58:
        if (lbl_8055E9BC == -1) {
            handled = 1;
            RaceCar* car = fn_801DDE78();
            for (int i = 0; i < lbl_8055E9C4; i++) {
                *lbl_80486F30[i].value = lbl_80486F30[i].def;
            }
            fn_8018A180(car);
            lbl_8055E9BC = 0;
        }
        break;
    }
    if (handled) {
        lbl_8055E9CC = fn_802E2C20();
    }
    return 0;
}

}  // extern "C"
