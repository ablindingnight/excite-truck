// Code 0x8020CF68-0x8020DE24, imported verbatim from the upstream project (github.com/orangedude27/excite-truck,
// CC0; file src/auto/text_8020CF68_.c), where its functions match exactly. Unit name is provisional.
// CLEANUP: automated-decompiler source with raw offsets and placeholder types; rewrite with real types
// before the PC port. Tricks are tagged MATCHING-AID.

extern int lbl_8055EE74, lbl_8055EE78, lbl_8055EEA0;
extern void *lbl_8055EE7C, *lbl_8055EE80;
extern char lbl_8055EE94;
extern float lbl_8055EE84, lbl_8055EE88, lbl_8055EE8C, lbl_8055EE90, lbl_8055EE98, lbl_8055EE9C;
extern const float lbl_8056331C, lbl_80563320;
extern const float lbl_8056332C;
__declspec(section ".data") extern float lbl_80558DC4;
extern const double lbl_80563360;
extern int fn_802E2C20(void);
extern void fn_80012158(void);
extern void fn_8020CF6C(int);
extern double fabs(double);
extern char lbl_8033C6B0[], lbl_8033C6BC[];
extern void fn_80209190(int, float);
extern void fn_80206454(void);
extern void fn_801769EC(void);
extern void fn_80206444(void);
extern float fn_80209028(void);
extern float fn_8020A5F0(void);
extern int fn_80265D64(void);
extern void __AXDSPInitCallback_801769E0(void);
extern void fn_80230E90(int);
extern void *fn_802090D4(void *);
extern void *fn_80145E70(void *);
extern void fn_80209110(void *);
#pragma cplusplus on
class Dummy_8020D1C8 {
public:
    virtual ~Dummy_8020D1C8();
};
#pragma cplusplus off

int fn_8020CF68(void) {
    return fn_802104D0();
}

extern const float lbl_80563318;
extern void fn_8020A5F8(float);
extern void fn_8020C908(float);
extern void fn_80209030(float);
extern void fn_8013323C(int, char *, float);
extern void fn_800121F0(void);
extern void fn_8000C730(void);
extern void fn_8020D0CC(int);
extern int lbl_8055EE78;
extern float lbl_8055EE8C, lbl_8055EE90, lbl_8055EE98, lbl_8055EE9C;
extern const float lbl_80563320;
extern char lbl_8033C690[], lbl_8033C6A0[];
extern void fn_8000C6E4(void);
#define MAX(a, b) ((a) > (b) ? (a) : (b))

void fn_8020CFEC(int arg0) {
    switch (arg0) {
    case 0:
        lbl_8055EE8C = MAX(lbl_8055EE8C - lbl_80563318, lbl_80563320);
        fn_8020C908(lbl_8055EE8C);
        break;
    case 1:
        lbl_8055EE90 = MAX(lbl_8055EE90 - lbl_80563318, lbl_80563320);
        break;
    case 4:
        fn_8000C730();
        break;
    }
}

void fn_8020D06C(void) {
    fn_8020A5F8(lbl_8055EE8C);
    fn_8020C908(lbl_8055EE8C);
    fn_80209030(lbl_8055EE90);
    fn_8013323C(4, lbl_8033C690, lbl_8055EE8C);
    fn_8013323C(4, lbl_8033C6A0, lbl_8055EE90);
    fn_800121F0();
}

void fn_8020D0CC(int arg0) {
    int value;
    value = (lbl_8055EE78 + arg0 + 3) % 3;
    lbl_8055EE78 = value;
    if (value == 2 && fn_8020CF68() == 0)
        lbl_8055EE78 = (lbl_8055EE78 + arg0 + 3) % 3;
    fn_80230E90(1);
    lbl_8055EE9C = lbl_80563320;
    lbl_8055EE98 = lbl_80563320;
}

void fn_8020D178(void) {
    lbl_8055EE74 = (int)fn_802090D4(lbl_8033C6B0);
    fn_80209190(lbl_8055EE74, lbl_8056331C);
    lbl_8055EE7C = 0;
    lbl_8055EE80 = 0;
    lbl_8055EEA0 = (int)fn_80145E70(lbl_8033C6BC);
}

#pragma cplusplus on
extern "C" void fn_8020D1C8(void) {
    Dummy_8020D1C8 *obj = (Dummy_8020D1C8 *)lbl_8055EEA0;
    if (obj != 0) {
        delete obj;
        lbl_8055EEA0 = 0;
    }
    fn_80209110(&lbl_8055EE74);
    lbl_8055EE7C = 0;
    lbl_8055EE80 = 0;
}
#pragma cplusplus off
void fn_8020D224(void) {
    float value;
    lbl_8055EE94 = 1;
    value = fn_80209028();
    lbl_8055EE90 = value;
    lbl_8055EE88 = value;
    value = fn_8020A5F0();
    lbl_8055EE8C = value;
    lbl_8055EE84 = value;
    if (fn_80265D64() == 0)
        __AXDSPInitCallback_801769E0();
    fn_80206444();
    fn_80209190(lbl_8055EE74, lbl_80563320);
    lbl_8055EE78 = 0;
    lbl_8055EE9C = lbl_80563320;
    lbl_8055EE98 = lbl_80563320;
    fn_80230E90(1);
}

void fn_8020D29C(void) {
    lbl_8055EE94 = 0;
    fn_80209190(lbl_8055EE74, lbl_8056331C);
    fn_80206454();
    fn_801769EC();
}

typedef struct SoundMenuLabels {
    const char *names[3];
} SoundMenuLabels;

// A text box from the layout (0x20 bytes); color at 0x18.
typedef struct LayoutVec {
    float x;
    float y;
    float z;
} LayoutVec;

typedef struct LayoutText {
    long unk00;
    LayoutVec pos;
    float unk10;
    long unk14;
    unsigned long color;
    long unk1C;
} LayoutText;

// A picture pane from the layout (0x1C bytes); color at 0x18.
typedef struct LayoutPane {
    long unk00;
    LayoutVec pos;
    float w;
    float h;
    unsigned long color;
} LayoutPane;

extern const SoundMenuLabels lbl_802F9E58;
extern char lbl_8055B630[6], lbl_8055B638[3], lbl_8055B63C[7];
extern const float lbl_80563324, lbl_80563328, lbl_80563330, lbl_80563334, lbl_80563338, lbl_8056333C;
extern const float lbl_80563340, lbl_80563344, lbl_80563348, lbl_8056334C, lbl_80563350;
extern const float lbl_80563354, lbl_80563358, lbl_8056335C;
extern void fn_80230B5C(void *);
extern void fn_802C8638(void *);
extern void fn_802C974C(float, float, float);
extern void fn_800109E4(unsigned long, float, float, float, float);
extern const char *fn_80131234(const char *);
extern void fn_801473F8(int, const char *, const char *, ...);
extern void fn_801345D8(char *, int, const char *, ...);
extern int fn_8000C69C(void);
extern void fn_80230F78(unsigned char *, float *, unsigned long *, int);
extern void fn_80147940(int, void *, const char *, int);
extern void fn_80147230(void *, const char *, ...);
extern void fn_801476C4(void *);
extern void fn_80011A2C(void);
extern void fn_802C9904(void);

#pragma cplusplus on
// The sound menu layout: slot 0x0C finds a text box, slot 0x14 a picture pane.
class SoundMenuLayout {
public:
    virtual void vf08(void);
    virtual LayoutText *FindText(const char *name);
    virtual void vf10(void);
    virtual LayoutPane *FindPane(const char *name);
};

static inline unsigned long HalveAlpha(unsigned long color) {
    return (color & 0x00FFFFFF) | ((color >> 1) & 0x7F000000);
}

// Draws the sound menu: music volume, effects volume and the third item, the selection highlight,
// and the left/right arrows that fade out at the ends of a slider.
extern "C" void fn_8020D2D4(void) {
    const char *pool = lbl_8033C690;
    float camera[12];
    float now;
    SoundMenuLabels labels;
    unsigned long i;

    if (lbl_8055EE80 != 0) {
        ((void (*)(void))lbl_8055EE80)();
    }
    fn_80230B5C(camera);
    fn_802C8638(camera);
    fn_802C974C(lbl_8056331C, lbl_80563324, lbl_80563328);
    now = lbl_8056332C * (float)fn_802E2C20();
    fn_800109E4(0x7F000000, lbl_80563330, lbl_80563334, lbl_80563338, lbl_8056333C);
    if (lbl_8055EEA0 != 0) {
        fn_801473F8(lbl_8055EEA0, lbl_8055B630, lbl_8055B638, fn_80131234(pool + 0x68));
    }
    labels = lbl_802F9E58;
    if (lbl_8055EEA0 != 0) {
        for (i = 0; i < 3; i++) {
            char value[0x20] = {0};
            char paneName[0x20];
            char textName[0x20];
            const char *text = fn_80131234(labels.names[i]);
            long selected;
            LayoutText box;
            LayoutPane *pane;
            unsigned char alpha;
            float scale;

            switch (i) {
            case 0:
                fn_801345D8(value, 0x20, pool + 0x78, text, (int)(lbl_80563340 + lbl_80563344 * lbl_8055EE8C));
                break;
            case 1:
                fn_801345D8(value, 0x20, pool + 0x78, text, (int)(lbl_80563340 + lbl_80563344 * lbl_8055EE90));
                break;
            case 4:
                fn_801345D8(value, 0x20, pool + 0x84, fn_8000C69C());
                break;
            default: {
                char *dest = value;
                int left = 0x20;
                while ((*dest++ = *text++) != 0) {
                    if (--left <= 0) {
                        dest[-1] = 0;
                        break;
                    }
                }
                break;
            }
            }
            fn_801345D8(paneName, 0x20, pool + 0x90, i);
            fn_801345D8(textName, 0x20, lbl_8055B63C, i);
            selected = i == lbl_8055EE78;
            box = *((SoundMenuLayout *)lbl_8055EEA0)->FindText(textName);
            alpha = 0xFF;
            scale = lbl_80563320;
            if (selected) {
                fn_80230F78(&alpha, &scale, &box.color, 1);
            }
            pane = ((SoundMenuLayout *)lbl_8055EEA0)->FindPane(paneName);
            if (pane != 0) {
                LayoutPane back = *pane;
                if (i == 2 && fn_8020CF68() == 0) {
                    back.color = HalveAlpha(back.color);
                }
                fn_80147940(lbl_8055EEA0, &back, pool + 0xA0, selected ? 1 : 2);
                if (selected) {
                    LayoutPane glow = *pane;
                    float grow = lbl_8056331C + scale;
                    glow.w = pane->w * grow;
                    glow.h = pane->h * grow;
                    glow.pos.x = glow.pos.x - lbl_80563340 * pane->w * scale;
                    glow.pos.y = glow.pos.y - lbl_80563340 * pane->h * scale;
                    glow.color = (pane->color & 0x00FFFFFF) | ((unsigned long)alpha << 24);
                    fn_80147940(lbl_8055EEA0, &glow, pool + 0xA0, 1);
                }
            }
            if (selected && i >= 0 && i < 5) {
                int canDec = 1;
                int canInc = 1;
                char arrowName[0x20];
                unsigned long leftColor;
                unsigned long rightColor;
                LayoutPane *arrow;
                float t;

                switch (i) {
                case 0:
                    if ((float)__fabs(lbl_8055EE8C) <= lbl_80558DC4) canDec = 0;
                    if (lbl_8056331C == lbl_8055EE8C) canInc = 0;
                    break;
                case 1:
                    if ((float)__fabs(lbl_8055EE90) <= lbl_80558DC4) canDec = 0;
                    if (lbl_8056331C == lbl_8055EE90) canInc = 0;
                    break;
                case 2:
                    canInc = 0;
                    canDec = 0;
                    break;
                }
                leftColor = 0xFFFFFFFF;
                rightColor = 0xFFFFFFFF;
                if (!canDec) {
                    t = (now - lbl_8055EE98 - lbl_80563320) / lbl_80563348;
                    t = lbl_8056331C < t ? lbl_8056331C : t;
                    t = t > lbl_80563320 ? t : lbl_80563320;
                    leftColor = ((unsigned long)(int)(lbl_8056334C + t * lbl_80563350) << 24) | 0x00FFFFFF;
                }
                if (!canInc) {
                    t = (now - lbl_8055EE9C - lbl_80563320) / lbl_80563348;
                    t = lbl_8056331C < t ? lbl_8056331C : t;
                    t = t > lbl_80563320 ? t : lbl_80563320;
                    rightColor = ((unsigned long)(int)(lbl_8056334C + t * lbl_80563350) << 24) | 0x00FFFFFF;
                }
                fn_801345D8(arrowName, 0x20, pool + 0xAC, textName);
                arrow = ((SoundMenuLayout *)lbl_8055EEA0)->FindPane(arrowName);
                if (arrow != 0) {
                    LayoutPane a = *arrow;
                    t = (now - lbl_8055EE98 - lbl_80563354) / lbl_80563358;
                    t = lbl_8056331C < t ? lbl_8056331C : t;
                    t = t > lbl_80563320 ? t : lbl_80563320;
                    a.color = leftColor;
                    a.pos.x = a.pos.x - (lbl_80563320 + t * lbl_8056335C);
                    fn_80147940(lbl_8055EEA0, &a, pool + 0xBC, 1);
                }
                fn_801345D8(arrowName, 0x20, pool + 0xCC, textName);
                arrow = ((SoundMenuLayout *)lbl_8055EEA0)->FindPane(arrowName);
                if (arrow != 0) {
                    LayoutPane a = *arrow;
                    t = (now - lbl_8055EE9C - lbl_80563354) / lbl_80563358;
                    t = lbl_8056331C < t ? lbl_8056331C : t;
                    t = t > lbl_80563320 ? t : lbl_80563320;
                    a.color = rightColor;
                    a.pos.x = a.pos.x + (lbl_80563320 + t * lbl_8056335C);
                    fn_80147940(lbl_8055EEA0, &a, pool + 0xE0, 1);
                }
            }
            if (i == 2 && fn_8020CF68() == 0) {
                box.color = HalveAlpha(box.color);
            }
            fn_80147230(&box, lbl_8055B638, value);
        }
        fn_801476C4(((SoundMenuLayout *)lbl_8055EEA0)->FindText(pool + 0xF0));
        if (lbl_8055EE78 == 2) {
            fn_801476C4(((SoundMenuLayout *)lbl_8055EEA0)->FindText(pool + 0x100));
        }
    }
    fn_80011A2C();
    fn_802C9904();
}
#pragma cplusplus off

void fn_8020DABC(void) {
    fn_8020C908(lbl_8055EE8C);
    fn_80209030(lbl_8055EE90);
    fn_800121F0();
    if (lbl_8055EE78 == 2 && fn_8020CF68() == 0)
        fn_8020D0CC(-1);
}

int fn_8020DB0C(int arg0) {
    int var_r31 = 1;

    switch (arg0) {
    case 0x126:
        fn_8020D0CC(-1);
        break;
    case 0x128:
        fn_8020D0CC(1);
        break;
    case 0x125:
        switch (lbl_8055EE78) {
        case 0:
            if (!(__fabsf(lbl_8055EE8C) <= lbl_80558DC4)) {
                lbl_8055EE98 = lbl_8056332C * (float)fn_802E2C20();
            } else {
                var_r31 = 0;
            }
            break;
        case 1:
            if (!(__fabsf(lbl_8055EE90) <= lbl_80558DC4)) {
                lbl_8055EE98 = lbl_8056332C * (float)fn_802E2C20();
            } else {
                var_r31 = 0;
            }
            break;
        case 4:
            lbl_8055EE98 = lbl_8056332C * (float)fn_802E2C20();
            break;
        default:
            var_r31 = 0;
            break;
        }
        fn_8020CFEC(lbl_8055EE78);
        break;
    case 0x127:
        switch (lbl_8055EE78) {
        case 0:
            if (lbl_8055EE8C < lbl_8056331C) {
                lbl_8055EE9C = lbl_8056332C * (float)fn_802E2C20();
            } else {
                var_r31 = 0;
            }
            break;
        case 1:
            if (lbl_8055EE90 < lbl_8056331C) {
                lbl_8055EE9C = lbl_8056332C * (float)fn_802E2C20();
            } else {
                var_r31 = 0;
            }
            break;
        case 4:
            lbl_8055EE9C = lbl_8056332C * (float)fn_802E2C20();
            break;
        default:
            var_r31 = 0;
            break;
        }
        fn_8020CF6C(lbl_8055EE78);
        break;
    case 0x58:
    case 0xD:
        if (lbl_8055EE7C != 0 && lbl_8055EE78 == 2) {
            ((void (*)(int, int))lbl_8055EE7C)(lbl_8055EE78, arg0);
            fn_8020D06C();
            break;
        }
        return 0;
    case 0x53:
        if (lbl_8055EE7C != 0) {
            ((void (*)(int, int))lbl_8055EE7C)(lbl_8055EE78, arg0);
            fn_8020D06C();
        }
        break;
    case 0x1B:
    case 0x113:
        var_r31 = 0;
        break;
    default:
        return 0;
    }
    if (var_r31 != 0) {
        fn_80012158();
    }
    return 1;
}
extern void *lbl_8055EE7C;
extern void *lbl_8055EE80;
void fn_8020DE10(void *arg0, void *arg1) {
    lbl_8055EE7C = arg0;
    lbl_8055EE80 = arg1;
}

extern char lbl_8055EE94;
char fn_8020DE1C(void) {
    return lbl_8055EE94;
}

