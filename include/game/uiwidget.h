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

// widgets.cpp's image drawers (lbl_8033F998 / 9BC / 9E0). uiapi's make_image builds the first two.
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

#endif
