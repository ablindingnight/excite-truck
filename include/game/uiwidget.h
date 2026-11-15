#ifndef GAME_UIWIDGET_H
#define GAME_UIWIDGET_H

// Self-contained (built-in types only): widgets.cpp has its own u32 typedef, so u32 would mangle differently per unit.

// The menu widget framework shared by uiapi.cpp and widgets.cpp (see handoff/FRAMEWORK_PLAN_2026-11-15.md).
// Class and slot names are unproven; the slot names follow widgets.cpp's WidgetDispatch view. Every widget
// vtable in the two units (uiapi lbl_8033D480/D590, widgets lbl_8033DD20..lbl_8033F90C) derives from UIWidget,
// whose own vtable (lbl_8033FB38) is emitted by widgets.cpp (its destructor, the key function, is defined there).

struct UIWidgetProperty {
    void* value;   // 0x0
    int key;       // 0x4
    int bytes;     // 0x8
    void* buffer;  // 0xC
};

class UIWidget {  /* Class name unproven */
public:
    UIWidget(int left, int top, int right, int bottom, int kind);  // widgets fn_80218F34
    UIWidget(struct WidgetDesc* desc);                              // widgets fn_80218F80
    virtual ~UIWidget(void);                                         // widgets fn_8021915C
    virtual void Slot0C(void);               // 0x0C
    virtual void Slot10(void);               // 0x10
    virtual void Slot14(void);               // 0x14
    virtual int Slot18(void);                // 0x18
    virtual void Slot1C(int x, int y);       // 0x1C: move to (x, y)
    virtual void Slot20(int dx, int dy);     // 0x20: move by (dx, dy)
    virtual void Slot24(void* context);      // 0x24
    virtual void Slot28(void);               // 0x28
    virtual void Slot2C(void);               // 0x2C
    virtual void Slot30(void);               // 0x30
    virtual void Slot34(void);               // 0x34
    virtual void Slot38(void);               // 0x38
    virtual void Slot3C(int a, int b);       // 0x3C
    virtual void Slot40(int a, int b);       // 0x40
    virtual void Slot44(void);               // 0x44
    virtual void Slot48(void);               // 0x48
    virtual void Slot4C(int x, int y);       // 0x4C
    virtual void Slot50(void);               // 0x50
    virtual void Slot54(void);               // 0x54
    virtual void Slot58(void);               // 0x58
    virtual void Slot5C(void);               // 0x5C
    virtual void* Slot60(void);              // 0x60
    virtual int Slot64(int key);             // 0x64
    virtual int Slot68(void);                // 0x68
    virtual int Slot6C(int key);             // 0x6C
    virtual int Slot70(int x, int y);        // 0x70: hit test
    virtual void Slot74(int key, void* value);  // 0x74: property changed
    virtual int Slot78(void);                // 0x78: width
    virtual void Slot7C(void);               // 0x7C: per-frame update
    virtual void Slot80(void);               // 0x80

    int left, top, right, bottom, kind;           // 0x04
    unsigned char flag18, flag19, flag1a, enabled;           // 0x18
    float fadeIn, fadeOut, speedIn, speedOut;       // 0x1C
    void* window;                                 // 0x2C
    UIWidgetProperty properties[32];              // 0x30
    int propertyCount;                            // 0x230
    unsigned char dirty, pad235[3];                          // 0x234
    unsigned int group;                                    // 0x238
    int style;                                    // 0x23C
    void* stylePtr;                               // 0x240
};

// A widget that drags its window by its stored offset (vtable lbl_8033FAB4; no constructor in widgets.cpp, so the
// table is emitted for its key function). Class name unproven; declared before UILabel (its vtable follows UILabel's).
class UIDragArea : public UIWidget {  /* Class name unproven */
public:
    virtual void Slot3C(int x, int y);           // fn_80219510 (implicit dtor fn_802295AC)
    virtual void Slot40(int a, int b) { enabled = 0; }  // inline: TU-end copy fn_80229604
    virtual void Slot4C(int x, int y);           // fn_8021952C
    int offsetX, offsetY;        // 0x244
    unsigned char enabled;       // 0x24C
};

// Text label base of the "choice"/caption widgets (vtable lbl_8033FA28; abstract): draws Slot88()'s text with
// Slot84()'s frame. Class and slot names unproven. The widgets still written as C call it by its linker names.
class UILabel : public UIWidget {  /* Class name unproven */
public:
    UILabel(int x, int y, int width, int height, int dx, int dy);  // fn_80219568
    virtual ~UILabel(void) {}                 // inline: out-of-line copy fn_80219900
    virtual void Slot24(void* context);       // fn_802195DC
    virtual void Slot28(void);                // fn_80219660
    virtual int Slot84(void);                 // fn_80219658: frame
    virtual const char* Slot88(void) = 0;     // text
    int textX, textY, textWidth, textStyle;   // 0x244 (widgets.cpp views: x, y, width, height)
};

// uiapi's key binding widget (lbl_8033D590): calls its mapped action when the key is pressed. Declared before UIImage
// because its vtable follows UIImage's in uiapi .data (vtables come out in reverse declaration order).
class UIHotKey : public UIWidget {  /* Class name unproven */
public:
    virtual ~UIHotKey(void);
    virtual int Slot64(int key);
    unsigned short key;            // 0x244
    void* action;       // 0x248
    void* param;        // 0x24C
    UIHotKey(void* p, void* a, unsigned short k) : UIWidget(0, 0, 0, 0, 0) {
        action = a;
        param = p;
        key = k;
    }
};

// An image drawer (the "img"/"art"/"cvs"/"stp" resources and the text references): abstract base of
// uiapi lbl_8033D504/D528/D54C and widgets lbl_8033F998/9BC/9E0; its own vtable is lbl_8033D570 (uiapi).
class UIImage {  /* Class name unproven */
public:
    virtual ~UIImage(void) {}                                                  // fn_802121D4 (inline; its copy follows make_image)
    virtual void DrawAt(int x, int y, int a, int b, int c, int frame);         // fn_80215828: calls Draw
    virtual void Draw(int x, int y, int a, unsigned int frame);                         // fn_80215824
    virtual void Rect(int x, int y, int* left, int* top, int* right, int* bottom) = 0;
    virtual int Hit(int x, int y, int testX, int testY);                       // fn_8021583C: tests Rect
    virtual int Frames(void);                                                  // fn_802154B8
};

struct gArtSet;
struct GTextFont;

// widgets.cpp's image drawers (lbl_8033F998 / 9BC / 9E0). uiapi's make_image builds AnimationPrefix and ImagePrefix.
// Declared in reverse .data order: MWCC emits vtables in reverse class declaration order.
class ResourcePrefix : public UIImage {  /* Class name unproven */
public:
    ResourcePrefix(const char* path, const char* text);
    virtual ~ResourcePrefix(void);
    virtual void Draw(int x, int y, int unused, unsigned int frame);
    virtual void Rect(int first, int second, int* left, int* top, int* right, int* bottom);
    virtual int Frames(void) { return 1; }
    virtual void Extra(void) {}  /* Helper name unproven */

    GTextFont* resource;  // 0x4
    const char* text;     // 0x8
};

class ImagePrefix : public UIImage {  /* Class name unproven */
public:
    ImagePrefix(const char* path);  // "aimg"
    virtual ~ImagePrefix(void);
    virtual void Draw(int x, int y, int unused, unsigned int frame);
    virtual void Rect(int first, int second, int* left, int* top, int* right, int* bottom);
    virtual int Frames(void) { return 1; }
    virtual void Extra(void) {}  /* Helper name unproven */

    gArtSet* image;  // 0x4
};

class AnimationPrefix : public UIImage {  /* Class name unproven */
public:
    AnimationPrefix(const char* path, int width, int height);  // "iimg"
    virtual ~AnimationPrefix(void);
    virtual void Draw(int x, int y, int unused, unsigned int frame);
    virtual void Rect(int first, int second, int* left, int* top, int* right, int* bottom);
    virtual int Hit(int x, int y, int testX, int testY);
    virtual int Frames(void);
    virtual int Playing(void);  /* Helper name unproven */

    void* handle;  // 0x4
    float width;     // 0x8
    float height;    // 0xC
};

// A widget drawn by an owned child image (vtable lbl_8033F90C; class name unproven). Its Slot20/28/70 and its
// subclasses' small slots are inline members defined in widgets.cpp (the TU-end copies).
struct OwnedWidget;
class UIImageButton : public UIWidget {  /* Class name unproven; vtable lbl_8033F90C */
public:
    UIImageButton(int x, int y, int kind, int dx, int dy, OwnedWidget* child);  // fn_80222340
    virtual ~UIImageButton(void);                // fn_802223C4: deletes the child
    virtual void Slot1C(int x, int y);           // fn_80222468
    virtual void Slot20(int dx, int dy);         // inline fn_80229B0C
    virtual void Slot28(void);                   // inline fn_802299FC
    virtual int Slot70(int x, int y);            // inline fn_80229AE4
    virtual void Slot84(void);                   // fn_80222450
    virtual int Slot88(void);                    // fn_8021AB88: the draw state
    OwnedWidget* child;          // 0x244
    int posX, posY, kind;        // 0x248
};

class IButton : public UIImageButton {  /* Class name unproven (allocation tag "IButton"); vtable lbl_8033F880 */
public:
    IButton(void* context, int x, int y, int dx, int dy, OwnedWidget* child, int (*callback)(void*), unsigned char a, unsigned char b);  // fn_802224B4
    virtual ~IButton(void) {}                    // inline: out-of-line copy fn_802266B0
    virtual void Slot3C(int a, int b);           // inline fn_80229628
    virtual void Slot40(int a, int b);           // inline fn_80229640
    virtual int Slot88(void);                    // inline fn_802296C0
    virtual void Slot58(void);                   // inline fn_80229750
    virtual void Slot5C(void);                   // inline fn_80229790
    virtual void Slot7C(void);                   // inline fn_802297D0
    unsigned char active, unknown255, flag256, flag257;  // 0x254
    int (*callback)(void*);      // 0x258
    void* context;               // 0x25C
};

class MenuBtn : public IButton {  /* Class name unproven (allocation tag "MenuBtn"); vtable lbl_8033F7F4 */
public:
    MenuBtn(void* context, int x, int y, int dx, int dy, OwnedWidget* child, int (*callback)(void*));  // fn_80226630 (implicit dtor fn_80229550)
    virtual void Slot14(void);                   // fn_80226708
    virtual void Slot3C(int a, int b);           // fn_80226758
    virtual void Slot40(int a, int b);           // fn_80226768
    virtual int Slot64(int key);                 // fn_802267C4
    virtual int Slot6C(int key);                 // fn_802268A0
    virtual int Slot88(void);                    // fn_802268E8
    int token;                   // 0x260
    int* focus;                  // 0x264
};

// The first UILabel subclass; declared after the image prefixes so its vtable comes out before theirs.
class CheckBox : public UILabel {  /* Class name unproven (allocation tag "CheckBox"); vtable lbl_8033F768 */
public:
    CheckBox(int x, int y, const char* text, unsigned char* flag, int height);  // fn_80219870 (no dtor: implicit fn_802294F4)
    virtual void Slot3C(int a, int b) { *(bool*)flag = !*(bool*)flag; }   // inline: fn_8022984C
    virtual void Slot58(void) { if (flag1a) dirty = 1; flag18 = 1; }      // inline: fn_80229864
    virtual void Slot5C(void) { if (flag1a) dirty = 1; flag18 = 0; }      // inline: fn_80229884
    virtual int Slot64(int key);                                           // fn_80219958
    virtual const char* Slot88(void) { return text; }                      // inline: fn_802298A4
    virtual int Slot84(void) { int mode = 0; if (flag18) mode = 2; return (flag[0] != 0) | mode; }  // inline: fn_802298AC
    unsigned char* flag;         // 0x254
    char text[0x100];            // 0x258
};

struct FormattedChoiceDescriptor;
struct ObservedValueProvider;
class UITable;

// Animated arrow/step button (vtable lbl_8033F6E0; abstract: Slot84 is the press action). Class name unproven.
// Its in-class-declared Slot3C/40/58/5C/70 are inline, defined in widgets.cpp (TU-end copies fn_80229CB4..D3C).
class UIArrowButton : public UIWidget {  /* Class name unproven; vtable lbl_8033F6E0 */
public:
    UIArrowButton(int x, int y, void* handle);   // fn_8021AC24
    virtual ~UIArrowButton(void) {}              // inline: out-of-line copy fn_8021AF9C
    virtual void Slot24(void* context);          // fn_8021ACC0
    virtual void Slot28(void);                   // fn_8021AD10
    virtual void Slot3C(int a, int b);           // inline fn_80229CB4
    virtual void Slot40(int a, int b);           // inline fn_80229CD8
    virtual void Slot58(void);                   // inline fn_80229CF8
    virtual void Slot5C(void);                   // inline fn_80229D1C
    virtual int Slot70(int x, int y);            // inline fn_80229D3C
    virtual void Slot7C(void);                   // fn_8021AE18
    virtual void Slot84(void) = 0;               // the press action
    unsigned char active, pad245[3];             // 0x244
    float elapsed, period;                       // 0x248
    void* handle;                                // 0x250
};

class ArrowBtn : public UIArrowButton {  /* Class name unproven (allocation tag "ArrowBtn"); vtable lbl_8033F658 */
public:
    ArrowBtn(int x, int y, void* watched, int option, int mode, void* handle, float low, float high);  // fn_8021AF0C (implicit dtor fn_80229498)
    virtual void Slot84(void);                   // fn_8021AFF4
    void* watched;               // 0x254
    float low, high;             // 0x258
    int option, mode;            // 0x260
};

class IntArrowBtn : public UIArrowButton {  /* Class name unproven (allocation tag "IntArrowBtn"); vtable lbl_8033F5D0 */
public:
    IntArrowBtn(int x, int y, int* watched, int low, int high, int step, void* handle);  // fn_8021B0CC (implicit dtor fn_8022943C)
    virtual void Slot84(void);                   // fn_8021B134
    int* watched;                // 0x254
    int low, high, step;         // 0x258
};

struct ObservedScrollState;
class ScrollBtn : public UIArrowButton {  /* Class name unproven (allocation tag "ScrollBtn"); vtable lbl_8033F548 */
public:
    ScrollBtn(int x, int y, ObservedScrollState* state, int direction, void* handle);  // fn_8021B1C4 (implicit dtor fn_802293E0)
    virtual void Slot84(void);                   // fn_8021B224
    int direction;               // 0x254
    ObservedScrollState* state;  // 0x258
};

class gText : public UIImageButton {  /* Class name unproven (allocation tags "gText"/"imgstamproll"); vtable lbl_8033F4B8 */
public:
    gText(int x, int y, int width, OwnedWidget* value, void* watched);  // fn_8021995C
    virtual ~gText(void);                        // fn_802199D0 (empty)
    virtual int Slot88(void);                    // inline fn_802298DC
    virtual void Slot8C(void) = 0;               // 0x8C: pure in retail's table (meaning unknown)
    void* watched;               // 0x254
};

class ArtWidget : public UIWidget {  /* Class name unproven (allocation tag "ArtWidget"); vtable lbl_8033F434 */
public:
    ArtWidget(struct WidgetDesc* desc, const char* text, int value);  // fn_80219A28
    virtual ~ArtWidget(void);                    // fn_80219E44
    virtual void Slot24(void* context) {}        // inline: TU-end copy fn_802298F8
    virtual void Slot28(void);                   // fn_80219FE0
    virtual void Slot7C(void);                   // fn_80219F14
    unsigned char fields244[0x40];  // 0x244, read through widgets.cpp's SourceResourcePrefix
};

class BackWidget : public UIWidget {  /* Class name unproven (allocation tag "BackWidget"); vtable lbl_8033F3B0 */
public:
    BackWidget(const char* textureName, unsigned char a, unsigned char b, int option, float amount);  // fn_8021A5B8
    virtual ~BackWidget(void);                   // fn_8021A650 (empty: no vptr store)
    virtual void Slot0C(void);                   // fn_8021A6EC
    virtual void Slot10(void);                   // fn_8021A744
    virtual void Slot24(void* context) {}        // inline: TU-end copy fn_802298FC
    virtual void Slot28(void);                   // fn_8021A6A8
    unsigned char fields244[0x14];  // 0x244, read through widgets.cpp's BackgroundWidgetPrefix
};

class RadioButton : public UIImageButton {  /* Class name unproven (allocation tags "RadioButton"/"BRadioButton"); vtable lbl_8033F324 */
public:
    RadioButton(int x, int y, int kind, void* watched, OwnedWidget* child);  // fn_8021A77C (implicit dtor fn_80229388)
    virtual void Slot0C(void);                   // inline fn_80229900
    virtual void Slot58(void);                   // inline fn_80229918
    virtual void Slot5C(void);                   // inline fn_80229958
    virtual void Slot3C(int a, int b);           // inline fn_80229998
    virtual int Slot64(int key);                 // fn_8021A804
    virtual int Slot88(void);                    // inline fn_802299B0
    void* watched;               // 0x254
    int value;                   // 0x258
};

class TabButton : public UIImageButton {  /* Class name unproven (allocation tags "TabButton"/"BTabButton"); vtable lbl_8033F298 */
public:
    TabButton(int x, int y, int kind, void* watched, OwnedWidget* child);  // fn_8021A808 (implicit dtor fn_80229330)
    virtual void Slot0C(void);                   // fn_8021A8A8
    virtual void Slot28(void);                   // fn_8021AA18
    virtual void Slot34(void);                   // inline fn_80229A94
    virtual void Slot3C(int a, int b);           // inline fn_80229AC4
    virtual int Slot64(int key);                 // fn_8021AB90
    virtual int Slot68(void);                    // inline fn_80229ADC
    virtual void Slot74(int key, void* data);    // fn_8021A910
    virtual int Slot88(void);                    // fn_8021A9A4
    void* watched;               // 0x254
    int value;                   // 0x258
};

class StaticText : public UILabel {  /* Class name unproven (allocation tag "StaticText"); vtable lbl_8033F20C */
public:
    StaticText(int x, int y, const char* label, int height, int* value, int width);  // fn_8021B2B0
    virtual ~StaticText(void) {}                            // inline: out-of-line copy fn_8021B404
    virtual void Slot7C(void);                              // fn_8021B32C
    virtual int Slot84(void);                               // fn_8021B3A0
    virtual const char* Slot88(void) { return label; }      // inline: fn_80229B70
    const char* label;           // 0x254
    unsigned int unknown258, unknown25C, unknown260;
    int* value;                  // 0x264
    char text[0x100];            // 0x268
};

struct RotatingChoiceDescriptorNew;
class CustomText : public StaticText {  /* Class name unproven (allocation tag "CustomText"); vtable lbl_8033F180 */
public:
    CustomText(RotatingChoiceDescriptorNew* desc, const char* label, int* value);  // fn_8021B460
    virtual ~CustomText(void);                              // fn_8021B8AC
    virtual void Slot28(void);                              // fn_8021B944
    virtual int Slot78(void) { return 0; }                  // inline: fn_80229B68
    int resource;                // 0x368
    unsigned char mode;          // 0x36C
    unsigned char pad36D[3];
    void* pose;                  // 0x370
};

// Concrete widgets defined in widgets.cpp and built by uiapi's factory. Names are the retail allocation tags
// passed to operator new (class names unproven). Fields are still read through widgets.cpp's observed views,
// so the storage here is opaque; declared in reverse retail .data order (vtables come out reversed).
class Column : public UIWidget {  /* Class name unproven (allocation tag "Column"); vtable lbl_8033F0FC */
public:
    Column(FormattedChoiceDescriptor* desc, UITable* provider, int column);  // fn_8021B9AC
    virtual ~Column(void);  // fn_8021BBA0
    virtual void Slot28(void);  // fn_8021BC20
    unsigned char fields244[0x20];  // 0x244, read through widgets.cpp views
};

class ColorRect : public UIWidget {  /* Class name unproven; vtable lbl_8033F078 (no constructor in widgets.cpp) */
public:
    virtual void Slot24(void* context);          // fn_8021B3BC (implicit dtor fn_802292D8)
    int rectX, rectY, rectW, rectH;              // 0x244
    int color;                                   // 0x254
};

class RichCaption : public UIWidget {  /* Class name unproven; vtable lbl_8033EFF4 (no constructor in widgets.cpp) */
public:
    inline virtual ~RichCaption(void);           // defined in widgets.cpp: TU-end copy fn_80229B78
    virtual void Slot24(void* context);          // fn_8021BD48
    inline virtual void Slot7C(void);            // defined in widgets.cpp: TU-end copy fn_80229BE8
    const char* label;                           // 0x244
    char text[256];                              // 0x248
    void* renderer;                              // 0x348 (BitmapFont*)
};

class Numeric : public UILabel {  /* Class name unproven (allocation tag "Numeric"); vtable lbl_8033EF68 */
public:
    Numeric(int x, int y, float* value, float scale, const char* format, float bias, int height);  // fn_8021BD98 (implicit dtor fn_8022927C)
    virtual void Slot74(int key, void* data);              // fn_8021BEBC
    virtual const char* Slot88(void) { return text; }      // inline: fn_80229C9C
    float scale, bias;           // 0x254
    float* value;                // 0x25C
    const char* format;          // 0x260
    char text[0x50];             // 0x264
};

struct FullFormatDispatch;
class SpinCtl : public UILabel {  /* Class name unproven (allocation tag "SpinCtl"); vtable lbl_8033EEDC */
public:
    SpinCtl(int x, int y, FullFormatDispatch* target, int height, int left, int right, int lineY);  // fn_8021BEC0 (implicit dtor fn_80229220)
    virtual void Slot14(void);                             // fn_8021BF90
    virtual void Slot28(void);                             // fn_8021BFEC
    virtual int Slot64(int key);                           // fn_8021C18C
    virtual void Slot74(int key, void* data);              // fn_8021C128
    virtual int Slot84(void);                              // fn_8021C318
    virtual const char* Slot88(void) { return text; }      // inline: fn_80229CA4
    char text[80];               // 0x254
    FullFormatDispatch* target;  // 0x2A4
    int* focus;                  // 0x2A8
    int token, left, right, lineY;  // 0x2AC
};

class SpinText : public UILabel {  /* Class name unproven (allocation tag "SpinText"); vtable lbl_8033EE50 */
public:
    SpinText(int x, int y, FullFormatDispatch* target, int height, int left, int right, int lineY, const char* label, int alignment);  // fn_8021C344 (implicit dtor fn_802291C4)
    virtual void Slot14(void);                             // fn_8021C478
    virtual void Slot28(void);                             // fn_8021C4D4
    virtual int Slot64(int key);                           // fn_8021C730
    virtual void Slot74(int key, void* data);              // fn_8021C6CC
    virtual int Slot84(void);                              // fn_8021C974
    virtual const char* Slot88(void) { return text; }      // inline: fn_80229CAC
    char text[80];               // 0x254
    FullFormatDispatch* target;  // 0x2A4
    int* focus;                  // 0x2A8
    int token, left, right, lineY;  // 0x2AC
    int labelLeft, labelRight, font;  // 0x2BC
    const char* label;           // 0x2C8
    int side;                    // 0x2CC
    bool showArrows;             // 0x2D0
};

struct FormatTarget;
class SpinArrow : public UIArrowButton {  /* Class name unproven (allocation tag "SpinArrow"); vtable lbl_8033EDC8 */
public:
    SpinArrow(int x, int y, void* handle, int direction, FormatTarget* target);  // fn_8021C97C (implicit dtor fn_80229168)
    virtual void Slot84(void);                   // fn_8021C9D8
    FormatTarget* target;        // 0x254
    int direction;               // 0x258
};

class SpinSldr : public UIWidget {  /* Class name unproven (allocation tag "SpinSldr"); vtable lbl_8033ED44 */
public:
    SpinSldr(int x, int y, int dx, int dy, ObservedValueProvider* provider);  // fn_8021CA0C
    virtual ~SpinSldr(void);  // fn_8021CAD4
    virtual void Slot24(void* context);  // fn_8021CB4C
    virtual void Slot28(void);  // fn_8021CBBC
    virtual void Slot3C(int a, int b);  // fn_8021CD04
    virtual void Slot40(int a, int b);  // fn_8021CEAC
    virtual void Slot4C(int x, int y);  // fn_8021CE8C
    unsigned char fields244[0x18];  // 0x244, read through widgets.cpp views
};

class IntNumeric : public UILabel {  /* Class name unproven (allocation tag "IntNumeric"); vtable lbl_8033ECB8 */
public:
    IntNumeric(int x, int y, int* value, float scale, const char* format, float bias, int height);  // fn_8021CEEC (implicit dtor fn_8022910C)
    virtual void Slot74(int key, void* data);              // fn_8021D040
    virtual const char* Slot88(void) { return text; }      // inline: fn_80229D5C
    float scale, bias;           // 0x254
    int* value;                  // 0x25C
    const char* format;          // 0x260
    char text[0x50];             // 0x264
};

struct ChoiceProviderDispatch;
class MutableMulti : public UILabel {  /* Class name unproven (allocation tag "MutableMulti"); vtable lbl_8033EC2C */
public:
    MutableMulti(int x, int y, int dx, int dy, int height, ChoiceProviderDispatch* provider);  // fn_8021D7C8
    virtual ~MutableMulti(void) {}                         // inline: TU-end copy fn_80229D64
    virtual void Slot14(void);                             // fn_8021D878
    virtual void Slot28(void);                             // fn_8021DB58
    virtual int Slot64(int key);                           // fn_8021D97C
    virtual int Slot84(void);                              // fn_8021D8D4
    virtual const char* Slot88(void);                      // fn_8021D864
    ChoiceProviderDispatch* provider;  // 0x254
    int* focus;                  // 0x258
    int token;                   // 0x25C
    float previousTime, nextTime;  // 0x260
    int fontResource;            // 0x268
};

class Multi : public UILabel {  /* Class name unproven (allocation tag "Multi"); vtable lbl_8033EBA0 */
public:
    Multi(int x, int y, int* value, const char* list, int height, int first, int second, int style);  // fn_8021D044
    virtual ~Multi(void);                                   // fn_8021D300
    virtual void Slot14(void);                              // fn_8021D154
    virtual void Slot24(void* context);                     // fn_8021D568
    virtual void Slot28(void);                              // fn_8021D378
    virtual void Slot3C(int a, int b);                      // fn_8021D614
    virtual int Slot64(int key);                            // fn_8021D1F0
    virtual void Slot74(int key, void* data);               // fn_8021D618
    virtual int Slot84(void);                               // fn_8021D1B0
    virtual const char* Slot88(void) { return text; }       // inline: fn_80229DC0
    int* value;                  // 0x254
    int count;                   // 0x258
    int first, second, style;    // 0x25C
    char text[0x50];             // 0x268
    const char** choices;        // 0x2B8
    int* focus;                  // 0x2BC
    int token;                   // 0x2C0
};

class CustomMulti : public Multi {  /* Class name unproven (allocation tag "CustomMulti"); vtable lbl_8033EB14 */
public:
    CustomMulti(int x, int y, int* value, const char* list, const char* first, int second, int third, int fourth);  // fn_8021D6A0
    virtual ~CustomMulti(void);                             // fn_8021D758
    int resource;                // 0x2C4
};

class StringList;
struct ObservedListViewport;
class ListBox : public UIWidget {  /* Class name unproven (allocation tag "ListBox"); vtable lbl_8033EA90 */
public:
    ListBox(int x, int y, int width, int height, StringList* list, int* selected, ObservedListViewport* view);  // fn_8021E0C0
    virtual ~ListBox(void) {}                    // inline: out-of-line copy fn_8021E884
    virtual void Slot24(void* context);          // fn_8021E3BC
    virtual void Slot28(void);                   // fn_8021E51C
    virtual void Slot3C(int x, int y);           // inline fn_80229DC8
    virtual void Slot40(int x, int y);           // inline fn_80229EB4
    virtual void Slot4C(int x, int y);           // fn_8021E1C8
    virtual void Slot50(void);                   // inline fn_80229E74
    virtual void Slot54(void);                   // inline fn_80229E34
    virtual int Slot64(int key);                 // fn_8021E620
    virtual int Slot68(void);                    // inline fn_80229EC4
    virtual void Slot7C(void);                   // fn_8021E2A0
    StringList* list;            // 0x244
    ObservedListViewport* view;  // 0x248
    int unknown24C, columns, rows, rowHeight;  // 0x24C
    int* selected;               // 0x25C
    int direction;               // 0x260
    float elapsed;               // 0x264
    unsigned char active;        // 0x268
};

class DropdownList : public ListBox {  /* Class name unproven; vtable lbl_8033EA0C (no constructor in widgets.cpp) */
public:
    virtual void Slot24(void* context);          // fn_8021E8DC (implicit dtor fn_802290B0)
    virtual void Slot3C(int x, int y);           // inline fn_80229DE8
    virtual void Slot4C(int x, int y);           // inline fn_80229E1C
    int width, height;           // 0x26C
};

class Input : public UIWidget {  /* Class name unproven (allocation tag "Input"); vtable lbl_8033E988 */
public:
    Input(int x, int y, int width, int height, char* buffer, int capacity, int stride, int font, bool clear);  // fn_8021EA64
    virtual ~Input(void);                        // fn_8021EB9C
    virtual void Slot24(void* context);          // fn_8021EC1C
    virtual void Slot28(void);                   // fn_8021EDA4
    virtual void Slot3C(int x, int y);           // fn_8021EFE0
    virtual int Slot68(void);                    // inline fn_80229ECC
    virtual void Slot58(void);                   // inline fn_80229ED4
    virtual void Slot5C(void);                   // inline fn_80229EF4
    virtual void* Slot60(void);                  // inline fn_80229F14
    virtual int Slot64(int key);                 // fn_8021F06C
    virtual void Slot74(int key, void* data);    // fn_8021F044
    int state, capacity, stride, cursor;  // 0x244
    char unknown254[0x800];      // 0x254
    char* buffer;                // 0xA54
    int font;                    // 0xA58
    void* first;                 // 0xA5C
    void* second;                // 0xA60
};

struct Binding;
class CDetect : public UILabel {  /* Class name unproven (allocation tag "CDetect"); vtable lbl_8033E8FC */
public:
    CDetect(int x, int y, int parameter, Binding* value, int height);  // fn_8021F394 (implicit dtor fn_80229054)
    virtual void Slot40(int x, int y);           // fn_8021F4F0
    virtual void Slot58(void);                   // inline fn_80229F1C
    virtual void Slot5C(void);                   // inline fn_80229F38
    virtual void Slot74(int key, void* data);    // fn_8021F42C
    virtual const char* Slot88(void);            // inline fn_80229F58
    virtual int Slot84(void);                    // inline fn_80229F60
    char buffer[24];             // 0x254
    int parameter;               // 0x26C
    Binding* value;              // 0x270
};

extern "C" void fn_802C519C(void*);
class Slider : public UIWidget {  /* Class name unproven (allocation tag "Slider"); vtable lbl_8033E878 */
public:
    Slider(int x, int y, int width, int height, float* value, float minimum, int rate, float maximum);  // fn_8021F678
    virtual ~Slider(void) { fn_802C519C(background); fn_802C519C(marker); }  // inline: TU-end copy fn_80229F78
    virtual void Slot14(void);                        // fn_8021F7EC
    virtual void Slot24(void* context);               // fn_8021FC40
    virtual void Slot28(void);                        // fn_8021FD68
    virtual void Slot3C(int x, int y) { dragging = 1; Slot4C(x, y); }  // inline: fn_80229FF0
    virtual void Slot40(int a, int b) { dragging = 0; }                // inline: fn_8022A008
    virtual void Slot4C(int x, int y);                // fn_8021FA98
    virtual void Slot58(void) { if (flag1a) dirty = 1; flag18 = 1; }   // inline: fn_8022A014
    virtual void Slot5C(void) { if (flag1a) dirty = 1; flag18 = 0; }   // inline: fn_8022A034
    virtual int Slot64(int key);                      // fn_8021F848
    bool dragging;               // 0x244
    unsigned char gap245[3];
    float minimum, maximum;      // 0x248
    float* value;                // 0x250
    int rate;                    // 0x254
    bool negative;               // 0x258
    unsigned char gap259[3];
    void* background;            // 0x25C
    void* marker;                // 0x260
    int oldX, oldY, oldWidth, oldHeight;  // 0x264
    int* focus;                  // 0x274
    int token;                   // 0x278
};

class SimpleSlider : public UIWidget {  /* Class name unproven (allocation tag "SimpleSlider"); vtable lbl_8033E7F4 */
public:
    SimpleSlider(struct WidgetDesc* desc, float* value, int divisions, float minimum, float maximum);  // fn_8021FF34
    virtual ~SimpleSlider(void);   // fn_8022003C
    virtual void Slot14(void);     // fn_8022071C
    virtual void Slot28(void);     // fn_802200AC
    virtual int Slot64(int key);   // fn_802205AC
    unsigned char fields244[0x24];  // 0x244, read through widgets.cpp's SteppedFloatPrefix
};

class ScrollBar : public UIWidget {  /* Class name unproven (allocation tags "VScrollBar"/"HScrollBar"); vtable lbl_8033E770 */
public:
    ScrollBar(int x, int y, int width, int height, void* view, int vertical);  // fn_80220E48
    virtual ~ScrollBar(void) { fn_802C519C(track); fn_802C519C(marker); }  // inline: TU-end copy fn_8022A054
    virtual void Slot24(void* context);  // fn_80221544
    virtual void Slot28(void);           // fn_80221970
    virtual void Slot3C(int a, int b);   // fn_80220F90
    virtual void Slot40(int a, int b);   // fn_80221538
    virtual void Slot4C(int x, int y);   // fn_802213A4
    void* view;                  // 0x244
    void* track;                 // 0x248
    void* marker;                // 0x24C
    unsigned char rest250[0x10]; // 0x250, read through widgets.cpp's ScrollbarPrefix
};

class CanvasWidget : public UIWidget {  /* Class name unproven; vtable lbl_8033E6EC (no constructor in widgets.cpp) */
public:
    virtual void Slot24(void* context);          // fn_80220E08 (implicit dtor fn_80228FFC)
    void* canvas;                                // 0x244
};

class IntSlider : public UIWidget {  /* Class name unproven (allocation tag "IntSlider"); vtable lbl_8033E668 */
public:
    IntSlider(int x, int y, int width, int height, int* value, int minimum, int maximum);  // fn_80220778
    virtual ~IntSlider(void);                    // fn_80220874
    virtual void Slot14(void);                   // fn_802208EC
    virtual void Slot24(void* context);          // fn_80220B5C
    virtual void Slot28(void);                   // fn_80220C54
    virtual void Slot3C(int x, int y);           // fn_80220A50
    virtual void Slot40(int x, int y);           // fn_80220AFC
    virtual void Slot4C(int x, int y);           // fn_80220B3C
    virtual int Slot64(int key);                 // fn_80220948
    int* value;                  // 0x244
    void* background;            // 0x248
    void* marker;                // 0x24C
    int minimum, maximum;        // 0x250
    bool dragging;               // 0x258
    unsigned char gap259[3];
    int* focus;                  // 0x25C
    int token;                   // 0x260
};

class MenuGrid : public UIWidget {  /* Class name unproven; vtable lbl_8033E5E4 (no constructor in widgets.cpp) */
public:
    virtual void Slot24(void* context);          // fn_80221F9C (implicit dtor fn_80228FA4)
    virtual void Slot3C(int x, int y);           // fn_80222120
    virtual void Slot40(int x, int y);           // fn_80222178
    virtual void Slot4C(int x, int y);           // fn_80222234
    unsigned char fields244[0x10];  // 0x244, read through widgets.cpp's MenuSelectionPrefix
};

class CircleList : public UIWidget {  /* Class name unproven (allocation tag "CircleList"); vtable lbl_8033E560 */
public:
    CircleList(struct WidgetDesc* desc, void* context);  // fn_80222BDC
    virtual ~CircleList(void);                   // fn_80222C8C
    virtual void Slot28(void);                   // fn_80222CFC
    void* context;               // 0x244
    unsigned char rest248[0x10]; // 0x248, read through widgets.cpp's UnsignedAssetPrefix
};

class StringList;
class BottomBar : public UIWidget {  /* Class name unproven (allocation tag "BottomBar"); vtable lbl_8033E4DC */
public:
    BottomBar(int x, int y, const StringList* list, int* selected);  // fn_802230A4
    virtual ~BottomBar(void);      // fn_80223208
    virtual void Slot28(void);     // fn_80223280
    virtual int Slot64(int key);   // fn_80223760
    virtual void Slot7C(void);     // fn_80223728
    const char* labels[33];        // 0x244
    int count;                     // 0x2C8
    int* selected;                 // 0x2CC
    unsigned char rest2D0[0x150];  // 0x2D0, read through widgets.cpp's SelectionFullPrefix
};

class BarTab : public BottomBar {  /* Class name unproven (allocation tag "BarTab"); vtable lbl_8033E458 */
public:
    BarTab(int x, int y, void* model, int* watched);  // fn_80223800 (implicit dtor fn_80228F4C)
    virtual void Slot0C(void);                   // fn_80223880
    virtual void Slot28(void);                   // fn_80223A58
    virtual int Slot64(int key);                 // fn_802239C8
    virtual int Slot68(void);                    // fn_80223AAC
    virtual void Slot74(int key, void* data);    // fn_802238D0
    int* watched;                // 0x420
};

class MenuImage : public UIWidget {  /* Class name unproven (allocation tag "MenuImage"); vtable lbl_8033E3D4 */
public:
    MenuImage(WidgetDesc* desc, void* context);  // fn_80223AB4
    virtual ~MenuImage(void);  // fn_80223B94
    virtual void Slot28(void);  // fn_80223C20
    virtual void Slot7C(void);  // fn_80223CC4
    unsigned char fields244[0x24];  // 0x244, read through widgets.cpp views
};
class MovieWidget : public UIWidget {  /* Class name unproven (allocation tag "MovieWidget"); vtable lbl_8033E350 */
public:
    MovieWidget(struct WidgetDesc* desc, const char* name);  // fn_80223CF0
    virtual ~MovieWidget(void);                  // fn_80223FAC
    virtual void Slot28(void);                   // fn_8022419C
    virtual void Slot7C(void);                   // fn_80224028
    virtual void Slot80(void);                   // fn_80224160
    unsigned char fields244[0x24];  // 0x244, read through widgets.cpp's LargeImage355/RenderRequestPrefix views
};

class DynamicLoadBackground : public UIWidget {  /* Class name unproven (allocation tag "DynamicLoadBackground"); vtable lbl_8033E2CC */
public:
    DynamicLoadBackground(WidgetDesc* desc);  // fn_802241C8
    virtual ~DynamicLoadBackground(void);  // fn_802242A4
    virtual void Slot28(void);  // fn_80224328
    unsigned char fields244[0x14];  // 0x244, read through widgets.cpp views
};
class FadeButton : public UIWidget {  /* Class name unproven (allocation tag "FadeButton"); vtable lbl_8033E248 */
public:
    FadeButton(void* context, WidgetDesc* desc, int (*callback)(void*) , int* frame);  // fn_8022435C
    virtual ~FadeButton(void);  // fn_802244C8
    virtual void Slot14(void);  // fn_8022455C
    virtual void Slot28(void);  // fn_802248FC
    virtual int Slot64(int key);  // fn_80224A38
    virtual int Slot6C(int key);  // fn_80224B0C
    virtual void Slot74(int key, void* value);  // fn_80224864
    virtual void Slot7C(void);  // fn_802245C8
    unsigned char fields244[0x48];  // 0x244, read through widgets.cpp views
};

class TextBox : public UIWidget {  /* Class name unproven (allocation tag "TextBox"); vtable lbl_8033E1C4 */
public:
    TextBox(int context, WidgetDesc* desc);  // fn_80224B54
    virtual ~TextBox(void);  // fn_80224D40
    virtual void Slot28(void);  // fn_80224E74
    unsigned char fields244[0x5C];  // 0x244, read through widgets.cpp views
};

struct RollingListData;
class ButtonList : public UIWidget {  /* Class name unproven (allocation tag "ButtonList"); vtable lbl_8033E140 */
public:
    ButtonList(RollingListData* list, struct WidgetDesc* desc);  // fn_8022553C
    virtual ~ButtonList(void);     // fn_80225828
    virtual void Slot14(void);     // fn_802258C8
    virtual void Slot28(void);     // fn_80225B6C
    virtual int Slot64(int key);   // fn_802263CC
    virtual int Slot6C(int key);   // fn_80226528
    virtual void Slot7C(void);     // fn_80225918
    RollingListData* list;         // 0x244
    unsigned char rest248[0x3C];   // 0x248, read through widgets.cpp's RollingListFullPrefix
};

class RotImage : public UIWidget {  /* Class name unproven (allocation tag "RotImage"); vtable lbl_8033E0BC */
public:
    RotImage(WidgetDesc* desc);  // fn_802269A8
    virtual ~RotImage(void);  // fn_80226AD0
    virtual void Slot28(void);  // fn_80226B6C
    unsigned char fields244[0x1C];  // 0x244, read through widgets.cpp views
};
class ControlButton : public UIWidget {  /* Class name unproven (allocation tag "ControlButton"); vtable lbl_8033E038 */
public:
    ControlButton(struct WidgetDesc* desc);  // fn_80226CBC
    virtual ~ControlButton(void);            // fn_80226F00
    virtual void Slot28(void);               // fn_80226F94
    unsigned char fields244[0x20];  // 0x244, read through widgets.cpp views
};

class ColorBackground : public UIWidget {  /* Class name unproven (allocation tag "ColorBackground"); vtable lbl_8033DFB4 */
public:
    ColorBackground(WidgetDesc* desc);  // fn_802270DC
    virtual ~ColorBackground(void);  // fn_80227244
    virtual void Slot28(void);  // fn_802272D4
    unsigned char fields244[0x30];  // 0x244, read through widgets.cpp views
};
class TopTabDisplay : public UIWidget {  /* Class name unproven (allocation tag "TopTabDisplay"); vtable lbl_8033DF30 */
public:
    TopTabDisplay(void* first, struct WidgetDesc* desc, void* second);  // fn_80227E84
    virtual ~TopTabDisplay(void);  // fn_80227F5C
    virtual void Slot28(void);     // fn_80228010
    bool flag;                     // 0x244
    unsigned char gap245[3];
    void* first;                   // 0x248
    void* second;                  // 0x24C
    unsigned char rest250[0xC];    // 0x250, read through widgets.cpp views
};

class TabBox : public UIWidget {  /* Class name unproven (allocation tag "TabBox"); vtable lbl_8033DEAC */
public:
    TabBox(WidgetDesc* desc);  // fn_80227D00
    virtual ~TabBox(void);  // fn_80227D64
    virtual void Slot28(void);  // fn_80227DE8
    unsigned char fields244[0x4];  // 0x244, read through widgets.cpp views
};

class SwfAnim;
class DynoTex;
class UISW2Movie : public UIWidget {  /* Class name unproven (allocation tag "UISW2Movie"); vtable lbl_8033DE28 */
public:
    UISW2Movie(struct WidgetDesc* desc, SwfAnim* movie);  // fn_80228418
    virtual ~UISW2Movie(void);   // fn_802284BC
    virtual void Slot28(void);   // fn_8022854C
    virtual void Slot7C(void);   // fn_80228554
    SwfAnim* animation;          // 0x244
    unsigned char owned;         // 0x248
};

class DynoTexWidget : public UIWidget {  /* Class name unproven (allocation tag "DynoTexWidget"); vtable lbl_8033DDA4 */
public:
    DynoTexWidget(struct WidgetDesc* desc, DynoTex* texture, int* frame);  // fn_8022855C
    virtual ~DynoTexWidget(void) {}  // inline: its copy fn_8022A0CC follows the TU's code
    virtual void Slot28(void);       // fn_802285B8
    DynoTex* texture;                // 0x244
    int* frame;                      // 0x248
};

class MenuCustomView;
class ModelWidget : public UIWidget {  /* Class name unproven (allocation tag "ModelWidget"); vtable lbl_8033DD20 */
public:
    ModelWidget(struct WidgetDesc* desc, const char* name);                          // fn_802286E4: builds a default view
    ModelWidget(struct WidgetDesc* desc, MenuCustomView* target, const char* name);  // fn_8022886C
    virtual ~ModelWidget(void);  // fn_802288E0
    virtual void Slot0C(void);   // fn_8022897C
    virtual void Slot10(void);   // fn_80228DAC
    virtual void Slot28(void);   // fn_80228A84
    virtual void Slot2C(void);   // fn_80228A6C
    virtual int Slot64(int key); // fn_80228D90
    virtual void Slot7C(void);   // fn_80228994
    virtual void Slot80(void);   // fn_80228A38
    const char* name;            // 0x244
    char lastName[32];           // 0x248
    MenuCustomView* target;      // 0x268
    unsigned char owned;         // 0x26C
    unsigned char pad26D[3];
    int viewLeft, viewTop, viewRight, viewBottom;  // 0x270
};

#endif
