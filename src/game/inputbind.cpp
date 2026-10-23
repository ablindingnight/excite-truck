// Input bindings (0x801A75B4-0x801A9660): named controls ("Joystick Up", "Key_SPACE", ...) for the
// pads, keyboard and mouse, their current and previous-frame values, edge detection, reading and
// writing them as option strings and finding the control the player is pressing. Names are guesses
// apart from the strings.
#include "game/common.h"

extern "C" {
char* strncpy(char* d, const char* s, u32 n);
int strcasecmp(const char* a, const char* b);
void* memset(void* p, int c, u32 n);
}

class LocString {
public:
    LocString(const char* key);
    u8 pad[0xC];
};

struct JoyInfo {
    u32 hold;  // offset 0x0
    u32 bits;  // offset 0x4
    s32 a;     // offset 0x8
    s32 b;     // offset 0xC
};

struct JoyStatus {
    f32 axes[8];     // offset 0x0
    u8 buttons[32];  // offset 0x20
    u32 dir;         // offset 0x40
    JoyInfo info;    // offset 0x44
    s32 f54;         // offset 0x54
    s32 f58;         // offset 0x58
    u32 f5C;         // offset 0x5C
    u8 connected;    // offset 0x60
};

struct Mouse {
    f32 x;  // offset 0x0
    f32 y;  // offset 0x4
    u8 b0;  // offset 0x8
    u8 b1;  // offset 0x9
    u8 b2;  // offset 0xA
};

// One control: kind (1 key, 2 stick axis, 3 button, 4 direction bit, 5 mouse axis, 6 mouse button), the
// pad it reads and the control index.
struct Binding {
    s32 type;  // offset 0x0
    s32 pad;   // offset 0x4
    union {
        s32 code;
        u8 key;
    };           // offset 0x8
    u8 latched;  // offset 0xC
};

struct BindEntry {
    const char* name;   // offset 0x0
    const char* label;  // offset 0x4
    s32 type;           // offset 0x8
    s32 code;           // offset 0xC
    u8 key;             // offset 0x10
    u8 pad11[0x14 - 0x11];
    u8 used;  // offset 0x14
    u8 pad15[0x18 - 0x15];
};

struct NamePair {
    const char* name;
    const char* text;
};

extern "C" {
u8 fn_802E47F8(int key);
int fn_802E4A1C(int key);
void fn_80127A84(s32* x, s32* y, s32* buttons);
int fn_80129AE8(JoyStatus* out, int chan);
void fn_801334F4(int section, const char* key, char* out, int len);
void fn_8013358C(int section, const char* key, const char* s);
void fn_802E317C(const char* fmt, ...);
extern s32 lbl_8055CFF0;  // screen width
extern s32 lbl_8055CFF4;  // screen height
extern char lbl_8055A850[4];
extern char lbl_80333D18[];
extern NamePair lbl_802F9588[21];
extern BindEntry lbl_803342D0[0x7A];
}

ModuleTag lbl_8055E5C0;
u8 lbl_8055E5C8;          // keyboard disabled
BindEntry* lbl_8055E5CC;  // the bindable controls
s32 lbl_8055E5D0;         // how many
s8 lbl_8055E5D4;          // key codes filled in

static JoyStatus lbl_80435050[8];  // current pads, then last frame's
static Mouse lbl_80435370[2];      // current mouse, then last frame's

static LocString lbl_80435388(lbl_80333D18 + 0x126C);
static LocString lbl_80435394(lbl_80333D18 + 0x1288);
static LocString lbl_804353A0(lbl_80333D18 + 0x12A4);
static LocString lbl_804353AC(lbl_80333D18 + 0x12C0);
static LocString lbl_804353B8(lbl_80333D18 + 0x12DC);
static LocString lbl_804353C4(lbl_80333D18 + 0x12F8);
static LocString lbl_804353D0(lbl_80333D18 + 0x1314);
static LocString lbl_804353DC(lbl_80333D18 + 0x1330);
static LocString lbl_804353E8(lbl_80333D18 + 0x1350);
static LocString lbl_804353F4(lbl_80333D18 + 0x1368);
static LocString lbl_80435400(lbl_80333D18 + 0x1384);
static LocString lbl_8043540C(lbl_80333D18 + 0x13A0);
static LocString lbl_80435418(lbl_80333D18 + 0x13B8);
static LocString lbl_80435424(lbl_80333D18 + 0x13D0);
static LocString lbl_80435430(lbl_80333D18 + 0x13EC);
static LocString lbl_8043543C(lbl_80333D18 + 0x1408);
static LocString lbl_80435448(lbl_80333D18 + 0x1420);
static LocString lbl_80435454(lbl_80333D18 + 0x1438);
static LocString lbl_80435460(lbl_80333D18 + 0x1450);
static LocString lbl_8043546C(lbl_80333D18 + 0x1468);
static LocString lbl_80435478(lbl_80333D18 + 0x1480);
static LocString lbl_80435484(lbl_80333D18 + 0x1498);
static LocString lbl_80435490(lbl_80333D18 + 0x14B0);
static LocString lbl_8043549C(lbl_80333D18 + 0x14C8);
static LocString lbl_804354A8(lbl_80333D18 + 0x14E0);
static LocString lbl_804354B4(lbl_80333D18 + 0x14F8);
static LocString lbl_804354C0(lbl_80333D18 + 0x1510);
static LocString lbl_804354CC(lbl_80333D18 + 0x1528);
static LocString lbl_804354D8(lbl_80333D18 + 0x1540);
static LocString lbl_804354E4(lbl_80333D18 + 0x1558);
static LocString lbl_804354F0(lbl_80333D18 + 0x1570);
static LocString lbl_804354FC(lbl_80333D18 + 0x1588);
static LocString lbl_80435508(lbl_80333D18 + 0x15A0);
static LocString lbl_80435514(lbl_80333D18 + 0x15B8);
static LocString lbl_80435520(lbl_80333D18 + 0x15D0);
static LocString lbl_8043552C(lbl_80333D18 + 0x15E8);
static LocString lbl_80435538(lbl_80333D18 + 0x1600);
static LocString lbl_80435544(lbl_80333D18 + 0x1618);
static LocString lbl_80435550(lbl_80333D18 + 0x1630);
static LocString lbl_8043555C(lbl_80333D18 + 0x164C);
static LocString lbl_80435568(lbl_80333D18 + 0x1668);
static LocString lbl_80435574(lbl_80333D18 + 0x1684);
static LocString lbl_80435580(lbl_80333D18 + 0x16A4);
static LocString lbl_8043558C(lbl_80333D18 + 0x16C8);
static LocString lbl_80435598(lbl_80333D18 + 0x16E8);
static LocString lbl_804355A4(lbl_80333D18 + 0x1700);
static LocString lbl_804355B0(lbl_80333D18 + 0x1718);
static LocString lbl_804355BC(lbl_80333D18 + 0x1738);
static LocString lbl_804355C8(lbl_80333D18 + 0x1750);
static LocString lbl_804355D4(lbl_80333D18 + 0x1768);
static LocString lbl_804355E0(lbl_80333D18 + 0x1780);
static LocString lbl_804355EC(lbl_80333D18 + 0x1798);
static LocString lbl_804355F8(lbl_80333D18 + 0x17B0);
static LocString lbl_80435604(lbl_80333D18 + 0x17C8);
static LocString lbl_80435610(lbl_80333D18 + 0x17E0);
static LocString lbl_8043561C(lbl_80333D18 + 0x17F8);

static inline f32 MaxZero(f32 v) { return 0.0f > v ? 0.0f : v; }

static inline void StrNCopy(char* dst, const char* src, int len) {
    char* d = dst;
    int n = len;
    const char* s = src;
    while ((*d++ = *s++) != 0) {
        if (--n <= 0) {
            d[-1] = 0;
            break;
        }
    }
}

extern "C" {

void fn_801A7628(void);
f32 fn_801A7F98(Binding* b);
f32 fn_801A85E8(Binding* b);
int fn_801A8A10(Binding* b);
int fn_801A8E38(Binding* b);
int fn_801A8EE4(Binding* b);
int fn_801A8F10(const Binding* b);
void fn_801A90A4(char* out, int len, Binding* b);
void fn_801A9110(Binding* b, const char* name);
void fn_801A91B0(Binding* b, BindEntry* e);
BindEntry* fn_801A9234(Binding* b);

NamePair* fn_801A75B4(const char* name) {
    NamePair* p = lbl_802F9588;
    int i;
    for (i = 0; i < 21; i++, p++) {
        if (!strcasecmp(p->name, name)) {
            return p;
        }
    }
    return 0;
}

void fn_801A7628(void) {
    if (!lbl_8055E5D4) {
        lbl_803342D0[73].key = fn_802E47F8(0x1E);
        lbl_803342D0[74].key = fn_802E47F8(0x30);
        lbl_803342D0[75].key = fn_802E47F8(0x2E);
        lbl_803342D0[76].key = fn_802E47F8(0x20);
        lbl_803342D0[77].key = fn_802E47F8(0x12);
        lbl_803342D0[78].key = fn_802E47F8(0x21);
        lbl_803342D0[79].key = fn_802E47F8(0x22);
        lbl_803342D0[80].key = fn_802E47F8(0x23);
        lbl_803342D0[81].key = fn_802E47F8(0x17);
        lbl_803342D0[82].key = fn_802E47F8(0x24);
        lbl_803342D0[83].key = fn_802E47F8(0x25);
        lbl_803342D0[84].key = fn_802E47F8(0x26);
        lbl_803342D0[85].key = fn_802E47F8(0x32);
        lbl_803342D0[86].key = fn_802E47F8(0x31);
        lbl_803342D0[87].key = fn_802E47F8(0x18);
        lbl_803342D0[88].key = fn_802E47F8(0x10);
        lbl_803342D0[89].key = fn_802E47F8(0x13);
        lbl_803342D0[90].key = fn_802E47F8(0x1F);
        lbl_803342D0[91].key = fn_802E47F8(0x16);
        lbl_803342D0[92].key = fn_802E47F8(0x2F);
        lbl_803342D0[93].key = fn_802E47F8(0x11);
        lbl_803342D0[94].key = fn_802E47F8(0x2D);
        lbl_803342D0[95].key = fn_802E47F8(0x15);
        lbl_803342D0[96].key = fn_802E47F8(0x2C);
        lbl_803342D0[97].key = fn_802E47F8(0x7);
        lbl_803342D0[98].key = fn_802E47F8(0x8);
        lbl_803342D0[99].key = fn_802E47F8(0x9);
        lbl_803342D0[100].key = fn_802E47F8(0xA);
        lbl_803342D0[101].key = fn_802E47F8(0xB);
        lbl_803342D0[102].key = fn_802E47F8(0x1A);
        lbl_803342D0[103].key = fn_802E47F8(0x1B);
        lbl_803342D0[104].key = fn_802E47F8(0x27);
        lbl_803342D0[105].key = fn_802E47F8(0x28);
        lbl_803342D0[106].key = fn_802E47F8(0x33);
        lbl_803342D0[107].key = fn_802E47F8(0x34);
        lbl_803342D0[108].key = fn_802E47F8(0x35);
        lbl_803342D0[109].key = fn_802E47F8(0xC);
        lbl_803342D0[110].key = fn_802E47F8(0xD);
        lbl_8055E5D4 = 1;
    }
    lbl_8055E5CC = lbl_803342D0;
    lbl_8055E5D0 = 0x7A;
}

void fn_801A7844(int reset) {
    s32 x;
    s32 y;
    s32 buttons;
    if (reset) {
        JoyStatus* p = lbl_80435050;
        int i;
        for (i = 0; i < 4; i++, p++) {
            memset(p, 0, sizeof(*p));
            fn_80129AE8(p, i);
        }
    }
    fn_80127A84(&x, &y, &buttons);
    {
        s32 w = lbl_8055CFF0 / 2;
        s32 h = lbl_8055CFF4 / 2;
        lbl_80435370[0].b0 = buttons & 1;
        lbl_80435370[0].b1 = (buttons >> 1) & 1;
        lbl_80435370[0].b2 = (buttons >> 2) & 1;
        lbl_80435370[0].x = (f32)(x - w) / (f32)w;
        lbl_80435370[0].y = (f32)(y - h) / (f32)h;
    }
}

void fn_801A7974(Binding* b, const char* key, int section) {
    char buf[32] = {0};
    fn_801A90A4(buf, sizeof(buf), b);
    fn_801334F4(section, key, buf, sizeof(buf));
    fn_801A9110(b, buf);
}

void fn_801A7A08(Binding* b, const char* key, int section) {
    char buf[32];
    fn_801A90A4(buf, sizeof(buf), b);
    fn_8013358C(section, key, buf);
}

void fn_801A7A5C(void) {
    int i;
    lbl_80435370[0].x = 0.0f;
    lbl_80435370[0].y = 0.0f;
    lbl_80435370[0].b2 = 0;
    lbl_80435370[0].b1 = 0;
    lbl_80435370[0].b0 = 0;
    fn_801A7844(1);
    fn_801A7844(1);
    for (i = 0; i < 4; i++) {
        lbl_80435050[4 + i] = lbl_80435050[i];
    }
    lbl_80435370[1] = lbl_80435370[0];
}

int fn_801A7C8C(Binding* out) {
    int i;
    fn_801A7628();
    for (i = 0; i < lbl_8055E5D0; i++) {
        if (!lbl_8055E5CC[i].used) {
            Binding b;
            b.pad = 0;
            b.latched = 0;
            fn_801A91B0(&b, &lbl_8055E5CC[i]);
            if (b.type != 5 && b.type != 6) {
                if (fn_801A8EE4(&b)) {
                    if (fn_801A8E38(&b) && !fn_801A8A10(&b)) {
                        *out = b;
                        return 1;
                    }
                } else {
                    f32 v = fn_801A7F98(&b);
                    if ((f32)__fabs(fn_801A85E8(&b)) < 0.25 && v > 0.5f) {
                        *out = b;
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

void fn_801A7DFC(const char* key, const char* name, int section) {
    Binding b;
    b.pad = 0;
    b.latched = 0;
    fn_801A9110(&b, name);
    fn_801A7A08(&b, key, section);
}

void fn_801A7E54(char* out, int len, Binding* b) {
    BindEntry* e = fn_801A9234(b);
    if (e) {
        NamePair* p = fn_801A75B4(e->name);
        if (p) {
            StrNCopy(out, p->text, len);
        } else {
            StrNCopy(out, e->label ? e->label : (const char*)&e->key, len);
        }
    } else {
        StrNCopy(out, lbl_8055A850, len);
    }
    out[len - 1] = 0;
}

void fn_801A7F8C(void) { lbl_8055E5C8 = 0; }

f32 fn_801A7F98(Binding* b) {
    JoyStatus* p;
    if (!fn_801A8F10(b)) {
        return 0.0f;
    }
    p = &lbl_80435050[b->pad];
    switch (b->type) {
    case 2:
        switch (b->code) {
        case 0:
            return MaxZero(-p->axes[0]);
        case 1:
            return MaxZero(p->axes[0]);
        case 2:
            return MaxZero(-p->axes[1]);
        case 3:
            return MaxZero(p->axes[1]);
        case 4:
            return MaxZero(-p->axes[3]);
        case 5:
            return MaxZero(p->axes[3]);
        case 6:
            return MaxZero(-p->axes[2]);
        case 7:
            return MaxZero(p->axes[2]);
        case 8:
            return MaxZero(-p->axes[4]);
        case 9:
            return MaxZero(p->axes[4]);
        case 10:
            return MaxZero(-p->axes[5]);
        case 11:
            return MaxZero(p->axes[5]);
        case 12:
            return MaxZero(-p->axes[6]);
        case 13:
            return MaxZero(p->axes[6]);
        case 14:
            return MaxZero(-p->axes[7]);
        case 15:
            return MaxZero(p->axes[7]);
        case 16:
            return 0.5f * (1.0f - p->axes[0]);
        case 17:
            return 0.5f * (1.0f - p->axes[1]);
        case 18:
            return 0.5f * (1.0f - p->axes[3]);
        case 19:
            return 0.5f * (1.0f - p->axes[2]);
        case 20:
            return 0.5f * (1.0f - p->axes[4]);
        case 21:
            return 0.5f * (1.0f - p->axes[5]);
        case 22:
            return 0.5f * (1.0f - p->axes[6]);
        case 23:
            return 0.5f * (1.0f - p->axes[7]);
        case 24:
            return 0.5f * (1.0f + p->axes[0]);
        case 25:
            return 0.5f * (1.0f + p->axes[1]);
        case 26:
            return 0.5f * (1.0f + p->axes[3]);
        case 27:
            return 0.5f * (1.0f + p->axes[2]);
        case 28:
            return 0.5f * (1.0f + p->axes[4]);
        case 29:
            return 0.5f * (1.0f + p->axes[5]);
        case 30:
            return 0.5f * (1.0f + p->axes[6]);
        case 31:
            return 0.5f * (1.0f + p->axes[7]);
        }
        break;
    case 3:
        if (b->code >= 0 && (u32)b->code < 32) {
            return p->buttons[b->code] ? 1.0f : 0.0f;
        }
        break;
    case 4:
        return (p->dir & b->code) ? 1.0f : 0.0f;
    case 5:
        switch (b->code) {
        case 0:
            return MaxZero(-lbl_80435370[0].x);
        case 1:
            return MaxZero(lbl_80435370[0].x);
        case 2:
            return MaxZero(-lbl_80435370[0].y);
        case 3:
            return MaxZero(lbl_80435370[0].y);
        case 4:
            return 0.5f * (1.0f - lbl_80435370[0].x);
        case 6:
            return 0.5f * (1.0f - lbl_80435370[0].y);
        case 5:
            return 0.5f * (1.0f + lbl_80435370[0].x);
        case 7:
            return 0.5f * (1.0f + lbl_80435370[0].y);
        }
        break;
    case 6:
        switch (b->code) {
        case 0:
            return lbl_80435370[0].b0 ? 1.0f : 0.0f;
        case 2:
            return lbl_80435370[0].b1 ? 1.0f : 0.0f;
        case 1:
            return lbl_80435370[0].b2 ? 1.0f : 0.0f;
        }
        break;
    case 1:
        if (!lbl_8055E5C8) {
            return fn_802E4A1C(b->key) ? 1.0f : 0.0f;
        }
        break;
    }
    return 0.0f;
}

f32 fn_801A85E8(Binding* b) {
    if (b->type == 1) {
        return 0.0f;
    } else {
        JoyStatus* cur = &lbl_80435050[b->pad];
        JoyStatus saved = *cur;
        Mouse m = lbl_80435370[0];
        f32 v;
        *cur = lbl_80435050[4 + b->pad];
        lbl_80435370[0] = lbl_80435370[1];
        v = fn_801A7F98(b);
        *cur = saved;
        lbl_80435370[0] = m;
        return v;
    }
}

int fn_801A8A10(Binding* b) {
    if (b->type == 1) {
        return 0;
    } else {
        JoyStatus* cur = &lbl_80435050[b->pad];
        JoyStatus saved = *cur;
        Mouse m = lbl_80435370[0];
        int v;
        *cur = lbl_80435050[4 + b->pad];
        lbl_80435370[0] = lbl_80435370[1];
        v = fn_801A8E38(b);
        *cur = saved;
        lbl_80435370[0] = m;
        return v;
    }
}

int fn_801A8E38(Binding* b) { return fn_801A7F98(b) > 0.2f; }

int fn_801A8E68(Binding* b) {
    switch (b->type) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        if (fn_801A8E38(b)) {
            if (!b->latched) {
                b->latched = 1;
                return 1;
            }
        } else {
            b->latched = 0;
            return 0;
        }
        break;
    }
    return 0;
}

int fn_801A8EE4(Binding* b) {
    s32 t = b->type;
    return t == 1 || t == 3 || t == 4 || t == 6;
}

int fn_801A8F10(const Binding* b) {
    s32 type = b->type;
    JoyInfo info = lbl_80435050[b->pad].info;
    if (type == 4) {
        return (info.hold >> 15) & 1;
    }
    if (type == 3) {
        return (info.bits & (1 << b->code)) != 0;
    }
    if (type == 2) {
        switch (b->code) {
    case 0:
    case 1:
    case 16:
    case 24:
        return (info.hold >> 0) & 1;
    case 2:
    case 3:
    case 17:
    case 25:
        return (info.hold >> 1) & 1;
    case 4:
    case 5:
    case 18:
    case 26:
        return (info.hold >> 3) & 1;
    case 6:
    case 7:
    case 19:
    case 27:
        return (info.hold >> 2) & 1;
    case 8:
    case 9:
    case 20:
    case 28:
        return (info.hold >> 4) & 1;
    case 10:
    case 11:
    case 21:
    case 29:
        return (info.hold >> 5) & 1;
    case 12:
    case 13:
    case 22:
    case 30:
        return (info.hold >> 6) & 1;
    case 14:
    case 15:
    case 23:
    case 31:
        return (info.hold >> 7) & 1;
        default:
            fn_802E317C("ControlExists: unhandled axis %d", b->code);
            return 0;
        }
    }
    return type != 0;
}

int fn_801A902C(void) {
    fn_801A7628();
    return 21;
}

void fn_801A9050(Binding* b, int i) {
    char unused[32] = {0};
    fn_801A9110(b, lbl_802F9588[i].name);
}

void fn_801A90A4(char* out, int len, Binding* b) {
    BindEntry* e = fn_801A9234(b);
    if (e) {
        strncpy(out, e->name, len);
    } else {
        out[0] = 0;
    }
    out[len - 1] = 0;
}

void fn_801A9110(Binding* b, const char* name) {
    int i;
    fn_801A7628();
    for (i = 0; i < lbl_8055E5D0; i++) {
        if (!strcasecmp(lbl_8055E5CC[i].name, name)) {
            fn_801A91B0(b, &lbl_8055E5CC[i]);
            return;
        }
    }
    b->type = 0;
}

void fn_801A91B0(Binding* b, BindEntry* e) {
    b->type = e->type;
    switch (b->type) {
    case 2:
        b->code = e->code;
        break;
    case 3:
        b->code = e->code;
        break;
    case 5:
        b->code = e->code;
        break;
    case 6:
        b->code = e->code;
        break;
    case 4:
        b->code = e->code;
        break;
    case 1:
        b->key = e->code;
        break;
    }
}

BindEntry* fn_801A9234(Binding* b) {
    BindEntry* e;
    int i;
    int n;
    fn_801A7628();
    n = lbl_8055E5D0;
    e = lbl_8055E5CC;
    for (i = 0; i < n; i++, e++) {
        if (b->type == e->type) {
            switch (b->type) {
            case 2:
                if (b->code == e->code) {
                    return &lbl_8055E5CC[i];
                }
                break;
            case 4:
                if (b->code == e->code) {
                    return &lbl_8055E5CC[i];
                }
                break;
            case 3:
                if (b->code == e->code) {
                    return &lbl_8055E5CC[i];
                }
                break;
            case 5:
                if (b->code == e->code) {
                    return &lbl_8055E5CC[i];
                }
                break;
            case 6:
                if (b->code == e->code) {
                    return &lbl_8055E5CC[i];
                }
                break;
            case 1:
                if (b->key == (u8)e->code) {
                    return &lbl_8055E5CC[i];
                }
                break;
            }
        }
    }
    return 0;
}
}
