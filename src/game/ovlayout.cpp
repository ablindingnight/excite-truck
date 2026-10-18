// Overlay layouts (0x80144F40-0x80149588): the .val-driven 2D layout sets used by every HUD/overlay -
// text, art and gart (solid box) elements and gart sets, their tabular save format for the in-game
// overlay editor ("OVEdit"), element lookup and the drawing helpers built on them. Names are guesses
// apart from the keys, messages and allocation tags.
#include <stdarg.h>
#include "game/common.h"

extern "C" {
char* strstr(const char* s, const char* t);
char* strchr(const char* s, int c);
int strcasecmp(const char* a, const char* b);
double atof(const char* s);
int sscanf(const char* s, const char* fmt, ...);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E3AD0(void* file, const char* fmt, ...);  // file printf
const char* fn_8013126C(char* buf, const char* s);   // quote for saving
int fn_8023512C(const char* name);                  // load image
int fn_802C9A58(void);  // widescreen
int fn_802C9A60(void);
s32 fn_802C9C38(void);  // screen height
s32 fn_802C9C48(void);  // screen width
void fn_8000FF88(u32 color, int flags, const char* fmt, f32 scale, f32 x, f32 y, ...);
void fn_800104B0(u32 color, int flags, const char* fmt, f32 scale, f32 x, f32 y, ...);
void fn_8001027C(u32 color, int flags, const char* fmt, f32 scale, f32 x, f32 y, ...);
void fn_802354A0(int art, int part, u32 color, f32 x, f32 y, f32 z, f32 w, f32 h);
}

extern f32 lbl_8055D020;  // widescreen x stretch

struct FlagName {
    const char* name;
    u32 flag;
};

struct TextElem;
struct ArtElem;
struct GartElem;
struct GartSet;
extern "C" {
void fn_801455B8(TextElem* e, ValueNode* n);
void fn_80145438(ArtElem* e, ValueNode* n);
void fn_80145538(GartElem* e, ValueNode* n);
void fn_80145670(GartSet* s, ValueNode* n);
void fn_80235380(int handle);
}

// Fields shared by every element type.
struct ElemBase {
    ElemBase(void) {}
    const char* name;  // offset 0x0
    f32 x;             // offset 0x4
    f32 y;             // offset 0x8
    s32 elemJust;      // offset 0xC
};

// Text element (0x20).
struct TextElem : ElemBase {
    void Read(ValueNode* n) { fn_801455B8(this, n); }
    f32 scale;            // offset 0x10
    u32 just;             // offset 0x14
    u32 color;            // offset 0x18
    const char* defText;  // offset 0x1C
};

// Art element (0x28): a textured quad.
struct ArtElem : ElemBase {
    void Read(ValueNode* n) { fn_80145438(this, n); }
    f32 w;             // offset 0x10
    f32 h;             // offset 0x14
    f32 u0;            // offset 0x18
    f32 v0;            // offset 0x1C
    f32 u1;            // offset 0x20
    f32 v1;            // offset 0x24
};

// Gart element (0x1C): a coloured box.
struct GartElem : ElemBase {
    void Read(ValueNode* n) { fn_80145538(this, n); }
    f32 w;             // offset 0x10
    f32 h;             // offset 0x14
    u32 color;         // offset 0x18
};

// Gart set (0xC): a named art resource.
struct GartSet {
    GartSet(void) {
        name = 0;
        resource = 0;
        handle = 0;
    }
    ~GartSet(void) {
        if (handle) {
            fn_80235380(handle);
        }
    }
    void Read(ValueNode* n) { fn_80145670(this, n); }
    const char* name;      // offset 0x0
    const char* resource;  // offset 0x4
    s32 handle;            // offset 0x8
};

template <typename T>
struct ElemList {
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
};

struct IPoint {
    u8 pad0[0x4];
    s32 x;  // offset 0x4
    s32 y;  // offset 0x8
};

struct V2 {
    f32 x;
    f32 y;
};

struct Rect {
    f32 x;  // offset 0x0
    f32 y;  // offset 0x4
    f32 w;  // offset 0x8
    f32 h;  // offset 0xC
};

FlagName lbl_8032CED8[] = {
    {"left", 0x1},   {"right", 0x2},    {"center", 0x4}, {"top", 0x8},
    {"middle", 0x10}, {"baseline", 0x20}, {"bottom", 0x40}, {0, 0},
};
ModuleTag lbl_8055E1F0;
UnkStaticFFFF lbl_8055E1F8;
static UnkStatic123 lbl_8042FBA0;
UnkStatic7F lbl_8055E1FC;
f32 lbl_8055E200;  // x offset
f32 lbl_8055E204;  // y offset
u8 lbl_8055E208;   // element positions are centred

static inline void StrCopyN(char* d, int n, const char* s) {
    while ((*d++ = *s++) != 0) {
        if (--n <= 0) {
            d[-1] = 0;
            break;
        }
    }
}

static inline void StrCat(char* d, int n, const char* s) {
    while (*d) {
        n--;
        d++;
    }
    if (n > 0) {
        StrCopyN(d, n, s);
    }
}

static inline int Round(f32 x) { return x < 0.0f ? x - 0.5f : x + 0.5f; }

extern "C" {

u32 fn_80144F40(const char* s, const FlagName* tbl) {
    u32 r = 0;
    for (; tbl->name; tbl++) {
        if (strstr(s, tbl->name)) {
            r |= tbl->flag;
        }
    }
    return r;
}

void fn_80144FB0(char* buf, int n, u32 flags, const FlagName* tbl) {
    buf[0] = 0;
    for (; tbl->name; tbl++) {
        if ((tbl->flag & flags) == tbl->flag) {
            if (buf[0]) {
                StrCat(buf, n, "_");
            }
            StrCat(buf, n, tbl->name);
        }
    }
}

u8 fn_801450A0(int a, int b, f32 t) {
    f32 f = (f32)a + t * ((f32)b - (f32)a);
    int v = Round(f);
    v = v > 0 ? v : 0;
    return v < 0xFF ? v : 0xFF;
}

int fn_80145140(ValueTable* t, const char* name) {
    int n = 0;
    int i = 0;
    ValueNode* c;
    for (;;) {
        c = t->GetChild(i);
        if (!c) {
            break;
        }
        if (!strcasecmp(c->GetName(), name)) {
            n++;
        }
        i++;
    }
    return n;
}

f32 fn_801451DC(f32 x, int just) {
    x -= 320.0f;
    if (fn_802C9A58() || fn_802C9A60()) {
        if (just == 0) {
            if (x < -180.0f) {
                just = 2;
            } else if (x > 180.0f) {
                just = 1;
            } else {
                just = 3;
            }
        }
        switch (just & ~0x20) {
        case 2:
            x -= 106.0f;
            break;
        case 1:
            x += 106.0f;
            break;
        case 3:
            break;
        }
    }
    x += lbl_8055E200;
    return x;
}

f32 fn_801452A0(f32 v, u32 flags) {
    if (flags & 0x20) {
        v /= lbl_8055D020;
    }
    return v;
}

f32 fn_801452B4(f32 y) { return 240.0f - y + lbl_8055E204; }

f32 fn_801452C8(f32 x) { return x + 320.0f; }

f32 fn_801452D4(f32 y) { return 240.0f - y; }

f32 fn_801452E0(ValueNode* n, const char* key) {
    const char* s = n->GetValue(key);
    if (strchr(s, '/')) {
        f64 d = atof(strchr(s, '/') + 1);
        return atof(s) / d;
    }
    return atof(s);
}

void fn_80145368(ElemBase* e, ValueNode* n) {
    e->name = n->GetValue("name");
    e->x = n->GetFloat("x");
    e->y = n->GetFloat("y");
    if (lbl_8055E208) {
        e->x = fn_801452C8(e->x);
        e->y = fn_801452D4(e->y);
    }
    e->elemJust = n->GetInt("elem_just", 0);
}

void fn_80145438(ArtElem* e, ValueNode* n) {
    fn_80145368(e, n);
    e->w = n->GetFloat("wid");
    e->h = n->GetFloat("hit");
    e->u0 = fn_801452E0(n, "u0");
    e->u1 = fn_801452E0(n, "u1");
    e->v0 = fn_801452E0(n, "v0");
    e->v1 = fn_801452E0(n, "v1");
}

u32 fn_801454E8(ValueNode* n, const char* key) {
    u32 c;
    if (sscanf(n->GetValue(key), "#%x", &c) != 1) {
        c = 0xFFFFFFFF;
    }
    return c;
}

void fn_80145538(GartElem* e, ValueNode* n) {
    fn_80145368(e, n);
    e->w = n->GetFloat("wid");
    e->h = n->GetFloat("hit");
    e->color = fn_801454E8(n, "color");
}

void fn_801455B8(TextElem* e, ValueNode* n) {
    fn_80145368(e, n);
    e->just = fn_80144F40(n->GetValue("just"), lbl_8032CED8);
    e->scale = n->GetFloat("scale");
    e->color = fn_801454E8(n, "color");
    e->defText = n->GetString("default_text", "XXX BADTEXT XXX");
}

void fn_80145670(GartSet* s, ValueNode* n) {
    s->name = n->GetValue("name");
    s->resource = n->GetValue("resource_name");
    s->handle = fn_8023512C(s->resource);
}

void fn_801456E8(ElemList<TextElem>* l, void* f) {
    int i;
    fn_802E3AD0(f, "tabular text\r\n");
    fn_802E3AD0(f, "\tx y elem_just color scale just name default_text\r\n");
    for (i = 0; i < l->count; i++) {
        TextElem* e = &l->data[i];
        char q[0x100];
        char just[0x40];
        const char* dt = fn_8013126C(q, e->defText);
        fn_80144FB0(just, 0x40, e->just, lbl_8032CED8);
        fn_802E3AD0(f, "\t%5.1f %5.1f %d #%8.8X %4.2f %16.16s %s \"%s\"\r\n", e->x, e->y, e->elemJust, e->color,
                    e->scale, just, e->name, dt);
    }
    fn_802E3AD0(f, "end\r\n");
}

void fn_801457D0(ElemList<ArtElem>* l, void* f) {
    int i;
    fn_802E3AD0(f, "tabular art\r\n");
    fn_802E3AD0(f, "\tx y wid hit u0 v0 u1 v1 elem_just name\r\n");
    for (i = 0; i < l->count; i++) {
        ArtElem* e = &l->data[i];
        fn_802E3AD0(f, "\t%5.1f %5.1f %5.1f %5.1f %f %f %f %f %d %s\r\n", e->x, e->y, e->w, e->h, e->u0, e->v0,
                    e->u1, e->v1, e->elemJust, e->name);
    }
    fn_802E3AD0(f, "end\r\n");
}

void fn_8014589C(ElemList<GartElem>* l, void* f) {
    int i;
    fn_802E3AD0(f, "tabular gart\r\n");
    fn_802E3AD0(f, "\tx y wid hit color elem_just name\r\n");
    for (i = 0; i < l->count; i++) {
        GartElem* e = &l->data[i];
        fn_802E3AD0(f, "\t%5.1f %5.1f %5.1f %5.1f #%8.8X %d %s\r\n", e->x, e->y, e->w, e->h, e->color, e->elemJust,
                    e->name);
    }
    fn_802E3AD0(f, "end\r\n");
}

void fn_8014595C(ElemList<GartSet>* l, void* f) {
    int i;
    fn_802E3AD0(f, "tabular gart_set\r\n");
    fn_802E3AD0(f, "\tresource_name name\r\n");
    for (i = 0; i < l->count; i++) {
        GartSet* s = &l->data[i];
        fn_802E3AD0(f, "\t%s %s\r\n", s->resource, s->name);
    }
    fn_802E3AD0(f, "end\r\n");
}

f32 fn_80145A08(f32 a, f32 b, f32 c, f32 d, f32 x) {
    f32 t = (x - a) / (b - a);
    return c + t * (d - c);
}

ValPoint fn_80145A24(const IPoint* p) {
    ValPoint r;
    f32 top;
    r.x = fn_80145A08(0.0f, fn_802C9C48(), 0.0f, 640.0f, p->x);
    top = fn_802C9C38() / 2 - 0.75f * fn_802C9C48() * 0.5f;
    r.y = fn_80145A08(top, top + 0.75f * fn_802C9C48(), 0.0f, 480.0f, p->y);
    return r;
}

void fn_80145B50(Rect* r, int n) {
    r->x = n * 10.0f;
    r->y = 0.0f;
    r->w = 10.0f;
    r->h = 10.0f;
}

void fn_80145B94(const TextElem* e, const char* text) {
    f32 y = fn_801452B4(e->y);
    fn_8000FF88(e->color, e->just, text, e->scale, fn_801451DC(e->x, e->elemJust), y);
}

void fn_80145C08(const TextElem* e, const char* text) {
    f32 y = fn_801452B4(e->y);
    fn_800104B0(e->color, e->just, text, e->scale, fn_801451DC(e->x, e->elemJust), y);
}

void fn_80145C7C(const TextElem* e, const char* text) {
    f32 y = fn_801452B4(e->y);
    fn_8001027C(e->color, e->just, "%s", e->scale, fn_801451DC(e->x, e->elemJust), y, text);
}

void fn_80145CF4(const GartElem* e, int art, int part) {
    f32 w = fn_801452A0(e->w, e->elemJust);
    f32 y = fn_801452B4(e->y);
    fn_802354A0(art, part, e->color, fn_801451DC(e->x, e->elemJust), y, 0.0f, w, e->h);
}

}  // extern "C"

extern "C" {
ValueTable* fn_80131810(const char* name);
void fn_80131818(ValueTable* t);
char* strrchr(const char* s, int c);
char* strcpy(char* d, const char* s);
size_t strlen(const char* s);
int vsprintf(char* buf, const char* fmt, va_list args);
void fn_802E317C(const char* fmt, ...);  // warning print
s32 fn_802E3D68(const char* name);        // open file for writing
void fn_802E3734(s32* f);                // close file
int fn_80127A50(void* ev);               // next mouse event
void fn_802C8BD0(s32 tex);
int fn_802C8DBC(int n);
void fn_802C8E00(u32 color, int idx, f32 x, f32 y, f32 z, f32 u, f32 v);
void fn_802C8E48(void);
void fn_802C8D74(void);
f32 fn_800108C8(const char* text, f32 scale);
void fn_80010B80(f32 x, f32 y, f32 w, f32 h, u32 color);
void fn_80011278(u32 color, f32 x, f32 y, f32 w, f32 h);
void fn_80011280(u32 color, int art, f32 x, f32 y, f32 w, f32 h);
void fn_80011630(u32 color, int segs, s32 tex, f32 x, f32 y, f32 w, f32 h, f32 start, f32 step, f32 u0, f32 v0,
                 f32 u1, f32 v1);
}

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

// A dynamically sized element array: data, count, capacity and the block to free.
template <typename T>
struct ArrayRef {
    ~ArrayRef(void) {
        data = 0;
        count = 0;
        cap = 0;
    }
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
};

template <typename T>
struct DynArray {
    DynArray(int n) {
        a.data = new ("dsa_buf") T[n];
        a.cap = n;
        a.count = 0;
        mem = a.data;
    }
    ~DynArray(void) { delete[] mem; }
    T* Add(void) { return &a.data[a.count++]; }
    ArrayRef<T> a;  // offset 0x0
    T* mem;         // offset 0xC
};

template <typename T>
void SafeDelete(T*& p);

class ILayout {
public:
    ILayout(void);
    virtual ~ILayout(void) {}
    virtual TextElem* Find(const char* name) = 0;
    virtual ArtElem* FindArt(const char* name) = 0;
    virtual GartElem* FindBox(const char* name) = 0;
    virtual int FindSet(const char* name) = 0;
    virtual void Edit(void) = 0;
    virtual void ToggleEdit(void) = 0;
    virtual u8 IsEditing(void) = 0;
    virtual void Save(void) = 0;
};

class Layout;

// Something the overlay editor can drag, click and label.
class EditHandler {
public:
    virtual void Drag(const V2* d, void* target) {}
    virtual u32 HoverColor(void) { return 0xFFFFFFFF; }
    virtual u32 Color(void) { return 0xFF808080; }
    virtual void Click(void) {}
    virtual const char* Label(void* target) { return 0; }
};

class LayoutCmd : public EditHandler {
public:
    LayoutCmd(Layout* o);
    virtual void Drag(const V2* d, void* target) {}
    virtual u32 HoverColor(void) { return 0xC0C0C0C0; }
    virtual u32 Color(void) { return 0x40000000; }
    virtual void Click(void);
    virtual const char* Label(void* target) { return "Save"; }
    Layout* owner;  // offset 0x4
};

// Edit handlers: what dragging a layout element's edit rect does.
class MoveHandler : public EditHandler {
public:
    virtual void Drag(const V2* d, void* target) {
        ElemBase* e = (ElemBase*)target;
        e->x += d->x;
        e->y += d->y;
    }
    virtual u32 Color(void) { return 0xFF00FF00; }
    virtual const char* Label(void* target) { return "Move"; }
};

class ResizeHandler : public EditHandler {
public:
    virtual void Drag(const V2* d, void* target) {
        ArtElem* e = (ArtElem*)target;
        e->x += d->x;
        e->w -= d->x;
        e->y += d->y;
        e->h -= d->y;
    }
    virtual u32 Color(void) { return 0xFF0000FF; }
    virtual const char* Label(void* target) { return "Resize"; }
};

class ScaleHandler : public EditHandler {
public:
    virtual void Drag(const V2* d, void* target) {
        TextElem* e = (TextElem*)target;
        f32 s = e->scale + d->x * 0.01f;
        s = 5.0f >= s ? s : 5.0f;
        e->scale = s <= 0.05f ? 0.05f : s;
    }
    virtual u32 Color(void) { return 0xFFC0D9D9; }
    virtual const char* Label(void* target) { return "Scale"; }
};

template <typename T>
class ChannelHandler : public EditHandler {
public:
    ChannelHandler(void) { idx = 0; }
    virtual void Drag(const V2* d, void* target) {
        u8* c = (u8*)&((T*)target)->color + idx;
        f32 v = *c + d->x;
        v = 255.0f >= v ? v : 255.0f;
        *c = v <= 0.0f ? 0.0f : v;
    }
    virtual u32 HoverColor(void) { return Color() & 0xFF7F7F7F; }
    virtual u32 Color(void) {
        u32 colors[4] = {0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFF000000};
        return colors[idx];
    }
    virtual void Click(void) { idx = (idx + 1) % 4; }
    virtual const char* Label(void* target) {
        const char* names[4] = {"Blue", "Green", "Red", "Alpha"};
        return names[idx];
    }
    s32 idx;  // offset 0x4
};

struct EditRect {
    Rect r;                // offset 0x0
    void* target;          // offset 0x10
    EditHandler* handler;  // offset 0x14
    void Set(int n) { fn_80145B50(&r, n); }
};

struct EditContext {
    EditContext(DynArray<EditRect>* r) {
        hover = 0;
        active = 0;
        rects = r;
    }
    EditRect* active;           // offset 0x0
    EditRect* hover;            // offset 0x4
    u8 moved;                   // offset 0x8
    DynArray<EditRect>* rects;  // offset 0xC
    f32 lastX;                  // offset 0x10
    f32 lastY;                  // offset 0x14
};

extern "C" void fn_80146BD4(char* d, const char* s, int n, int pad);
template <typename T, int N>
struct FixedList {
    FixedList(void) {
        data = storage;
        max = N;
        count = 0;
    }
    ~FixedList(void) {
        data = 0;
        count = 0;
        max = 0;
    }
    int CanAdd(void) { return count + 1 <= max; }
    void Push(T v) {
        T* p = &data[count++];
        *p = v;
    }
    int IndexOf(T v) {
        int i;
        for (i = 0; i < count; i++) {
            if (data[i] == v) {
                return i;
            }
        }
        return -1;
    }
    void RemoveAt(int i) {
        if (i >= 0 && i < count) {
            int last = count - 1;
            if (i != last) {
                data[i] = data[last];
            }
            count--;
        }
    }
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 max;    // offset 0x8
    T storage[N];
};

extern FixedList<Layout*, 40> lbl_8042FBC0;

extern "C" void fn_80145D8C(Layout* l);
extern "C" void fn_80145DD8(Layout* l);
extern "C" Layout* fn_80146490(Layout* l);

template <typename T>
DynArray<T>* LoadElems(ValueTable* t, const char* name);

class Layout : public ILayout {
public:
    Layout(const char* name) : cmd(this) {
        arts = 0;
        texts = 0;
        garts = 0;
        sets = 0;
        rects = 0;
        ctx = 0;
        editing = 0;
        fn_80146BD4(this->name, name, 0x20, 0);
        table = fn_80131810(name);
        if (table) {
            lbl_8055E208 = 1;
            if (table->HasChild("info")) {
                lbl_8055E208 = table->FindChild("info")->GetInt("version", 1) < 1;
            }
            arts = LoadElems<ArtElem>(table, "art");
            texts = LoadElems<TextElem>(table, "text");
            garts = LoadElems<GartElem>(table, "gart");
            sets = LoadElems<GartSet>(table, "gart_set");
            BuildEditRects();
        }
        fn_80145D8C(this);
    }
    virtual ~Layout(void);
    virtual TextElem* Find(const char* name);
    virtual ArtElem* FindArt(const char* name);
    virtual GartElem* FindBox(const char* name);
    virtual int FindSet(const char* name);
    virtual void Edit(void);
    virtual void ToggleEdit(void);
    virtual u8 IsEditing(void);
    virtual void Save(void);
    void BuildEditRects(void);

    bool editing;                  // offset 0x4
    char name[0x23];               // offset 0x5
    ValueTable* table;             // offset 0x28
    DynArray<ArtElem>* arts;       // offset 0x2C
    DynArray<TextElem>* texts;     // offset 0x30
    DynArray<GartElem>* garts;     // offset 0x34
    u8 pad38[0x4];
    DynArray<GartSet>* sets;       // offset 0x3C
    LayoutCmd cmd;                 // offset 0x40
    DynArray<EditRect>* rects;     // offset 0x48
    EditContext* ctx;              // offset 0x4C
};

FixedList<Layout*, 40> lbl_8042FBC0;

extern "C" {

void fn_80145D8C(Layout* l) {
    if (lbl_8042FBC0.CanAdd()) {
        lbl_8042FBC0.Push(l);
    } else {
        fn_802E30C8("OVEdit: too many layouts!");
    }
}

void fn_80145DD8(Layout* l) {
    int i = lbl_8042FBC0.IndexOf(l);
    if (i >= 0) {
        lbl_8042FBC0.RemoveAt(i);
    }
}

Layout* fn_80145E70(const char* name) { return fn_80146490(new ("OverlayLayout") Layout(name)); }

}  // extern "C"

template <typename T>
DynArray<T>* LoadElems(ValueTable* t, const char* name) {
    int n = fn_80145140(t, name);
    DynArray<T>* a = new ("overlay_dsa") DynArray<T>(n);
    int i;
    ValueNode* c;
    for (i = 0;; i++) {
        c = t->GetChild(i);
        if (!c) {
            break;
        }
        if (!strcasecmp(c->GetName(), name)) {
            a->Add()->Read(c);
        }
    }
    return a;
}

extern "C" {

Layout* fn_80146490(Layout* l) {
    if (l && !l->table) {
        delete l;
        l = 0;
    }
    return l;
}

}  // extern "C"

Layout::~Layout(void) {
    fn_80145DD8(this);
    if (table) {
        fn_80131818(table);
        table = 0;
    }
    SafeDelete(ctx);
    SafeDelete(rects);
    SafeDelete(arts);
    SafeDelete(texts);
    SafeDelete(garts);
    SafeDelete(sets);
}

template <typename T>
void SafeDelete(T*& p) {
    if (p) {
        delete p;
        p = 0;
    }
}

MoveHandler lbl_8055E20C;
ResizeHandler lbl_8055E210;
ScaleHandler lbl_8055E214;
ChannelHandler<TextElem> lbl_8055E218;
ChannelHandler<GartElem> lbl_8055E220;

void Layout::BuildEditRects(void) {
    int i;
    EditRect* r;
    rects = new ("edit_oprects") DynArray<EditRect>(arts->a.count * 2 + garts->a.count * 3 + texts->a.count * 3 + 1);
    ctx = new ("EditingContext") EditContext(rects);
    for (i = 0; i < arts->a.count; i++) {
        r = ctx->rects->Add();
        r->Set(0);
        r->target = &arts->a.data[i];
        r->handler = &lbl_8055E20C;
        r = ctx->rects->Add();
        r->Set(1);
        r->target = &arts->a.data[i];
        r->handler = &lbl_8055E210;
    }
    for (i = 0; i < texts->a.count; i++) {
        r = ctx->rects->Add();
        r->Set(0);
        r->target = &texts->a.data[i];
        r->handler = &lbl_8055E20C;
        r = ctx->rects->Add();
        r->Set(1);
        r->target = &texts->a.data[i];
        r->handler = &lbl_8055E214;
        r = ctx->rects->Add();
        r->Set(2);
        r->target = &texts->a.data[i];
        r->handler = &lbl_8055E218;
    }
    for (i = 0; i < garts->a.count; i++) {
        r = ctx->rects->Add();
        r->Set(0);
        r->target = &garts->a.data[i];
        r->handler = &lbl_8055E20C;
        r = ctx->rects->Add();
        r->Set(1);
        r->target = &garts->a.data[i];
        r->handler = &lbl_8055E210;
        r = ctx->rects->Add();
        r->Set(2);
        r->target = &garts->a.data[i];
        r->handler = &lbl_8055E220;
    }
    r = ctx->rects->Add();
    r->handler = &cmd;
    r->target = 0;
    r->r.x = 0.0f;
    r->r.y = 0.0f;
    r->r.w = 40.0f;
    r->r.h = 15.0f;
}

extern "C" void fn_80146BD4(char* d, const char* s, int n, int pad) { StrCopyN(d, n, s); }

LayoutCmd::LayoutCmd(Layout* o) { owner = o; }

ILayout::ILayout(void) {}

void LayoutCmd::Click(void) { owner->Save(); }

struct TextCmd {
    const char* elem;  // offset 0x0
    const char* text;  // offset 0x4
};

struct GartCmd {
    const char* elem;  // offset 0x0
    const char* art;   // offset 0x4
    s32 idx;           // offset 0x8
};

struct BoxCmd {
    const char* elem;  // offset 0x0
    const char* art;   // offset 0x4
    u32 color;         // offset 0x8
};

class IOverlayCmd {
public:
    virtual ~IOverlayCmd(void) {}
    virtual void Draw(void) = 0;
};

// A list of text/gart/box draw commands against a layout, read from a .val node.
class OverlayCmd : public IOverlayCmd {
public:
    virtual ~OverlayCmd(void) {
        SafeDelete(boxes);
        SafeDelete(garts);
        SafeDelete(texts);
        if (ownsLayout) {
            SafeDelete(layout);
        }
    }
    virtual void Draw(void);

    u8 ownsLayout;              // offset 0x4
    ILayout* layout;            // offset 0x8
    DynArray<TextCmd>* texts;   // offset 0xC
    DynArray<GartCmd>* garts;   // offset 0x10
    DynArray<BoxCmd>* boxes;    // offset 0x14
};

static inline int CountChildAt(ValueNode* n, const char* name) {
    int c = 0;
    int i;
    ValueNode* e;
    for (i = 0;; i++) {
        e = n->GetChildAt(i);
        if (!e) {
            break;
        }
        if (!strcasecmp(e->GetName(), name)) {
            c++;
        }
    }
    return c;
}

extern "C" OverlayCmd* fn_80146C2C(ILayout* l, ValueNode* n) {
    OverlayCmd* c = new ("OverlayLayoutCmd") OverlayCmd;
    if (c) {
        int i;
        ValueNode* e;
        DynArray<TextCmd>* t;
        DynArray<GartCmd>* g;
        DynArray<BoxCmd>* b;
        c->ownsLayout = 0;
        c->layout = l;
        c->texts = 0;
        c->garts = 0;
        c->boxes = 0;
        t = new ("overlaycmd_dsa") DynArray<TextCmd>(CountChildAt(n, "text_cmd"));
        for (i = 0;; i++) {
            e = n->GetChildAt(i);
            if (!e) {
                break;
            }
            if (!strcasecmp(e->GetName(), "text_cmd")) {
                TextCmd* tc = t->Add();
                tc->elem = e->GetValue("elem");
                tc->text = e->GetValue("text");
            }
        }
        c->texts = t;
        g = new ("overlaycmd_dsa") DynArray<GartCmd>(CountChildAt(n, "gart_cmd"));
        for (i = 0;; i++) {
            e = n->GetChildAt(i);
            if (!e) {
                break;
            }
            if (!strcasecmp(e->GetName(), "gart_cmd")) {
                GartCmd* gc = g->Add();
                gc->elem = e->GetValue("elem");
                gc->art = e->GetValue("art");
                gc->idx = e->GetIntValue("idx");
            }
        }
        c->garts = g;
        b = new ("overlaycmd_dsa") DynArray<BoxCmd>(CountChildAt(n, "box_cmd"));
        for (i = 0;; i++) {
            e = n->GetChildAt(i);
            if (!e) {
                break;
            }
            if (!strcasecmp(e->GetName(), "box_cmd")) {
                BoxCmd* bc = b->Add();
                bc->elem = e->GetValue("elem");
                bc->art = e->GetValue("art");
                bc->color = fn_801454E8(e, "color");
            }
        }
        c->boxes = b;
    }
    return c;
}

extern "C" {

void fn_801470B0(f32 x, f32 y, f32 w, f32 h, u32 color, s32 tex, int just, f32 u0, f32 v0, f32 u1, f32 v1) {
    fn_802C8BD0(tex);
    if (fn_802C8DBC(4)) {
        fn_802C8E00(color, 0, fn_801451DC(x, just), fn_801452B4(y), 0.0f, u0, v0);
        fn_802C8E00(color, 0, fn_801451DC(x + w, just), fn_801452B4(y), 0.0f, u1, v0);
        fn_802C8E00(color, 1, fn_801451DC(x, just), fn_801452B4(y + h), 0.0f, u0, v1);
        fn_802C8E00(color, 1, fn_801451DC(x + w, just), fn_801452B4(y + h), 0.0f, u1, v1);
        fn_802C8E48();
    }
    fn_802C8D74();
}

f32 fn_80147204(const TextElem* e, const char* text) {
    if (e) {
        if ((u8)*text == 1) {
            text++;
        }
        return fn_800108C8(text, e->scale);
    }
    return 0.0f;
}

void fn_80147230(TextElem* e, const char* fmt, ...) {
    if (e) {
        va_list args;
        char buf[0x200];
        va_start(args, fmt);
        vsprintf(buf, fmt, args);
        va_end(args);
        if (strlen(buf) >= 0x200) {
            fn_802E317C("ovedit: oversized text elem (len %d, fmt \"%s\")", strlen(buf), fmt);
        }
        fn_80145B94(e, buf);
    }
}

void fn_80147314(TextElem* e, const char* fmt, ...) {
    if (e) {
        va_list args;
        char buf[0x200];
        va_start(args, fmt);
        vsprintf(buf, fmt, args);
        va_end(args);
        if (strlen(buf) >= 0x200) {
            fn_802E317C("ovedit: oversized text elem (len %d, fmt \"%s\")", strlen(buf), fmt);
        }
        fn_80145C08(e, buf);
    }
}

void fn_801473F8(ILayout* l, const char* name, const char* fmt, ...) {
    TextElem* e = l->Find(name);
    if (e) {
        va_list args;
        char buf[0x200];
        va_start(args, fmt);
        vsprintf(buf, fmt, args);
        va_end(args);
        if (strlen(buf) >= 0x200) {
            fn_802E317C("ovedit: oversized text elem (len %d, fmt \"%s\")", strlen(buf), fmt);
        }
        fn_80145B94(e, buf);
    }
}

void fn_801474EC(TextElem* e, const char* fmt, ...) {
    if (e) {
        va_list args;
        char buf[0x200];
        va_start(args, fmt);
        vsprintf(buf, fmt, args);
        va_end(args);
        if (strlen(buf) >= 0x200) {
            fn_802E317C("ovedit: oversized text elem (len %d, fmt \"%s\")", strlen(buf), fmt);
        }
        fn_80145C7C(e, buf);
    }
}

void fn_801475D0(ILayout* l, const char* name, const char* fmt, ...) {
    TextElem* e = l->Find(name);
    if (e) {
        va_list args;
        char buf[0x200];
        va_start(args, fmt);
        vsprintf(buf, fmt, args);
        va_end(args);
        if (strlen(buf) >= 0x200) {
            fn_802E317C("ovedit: oversized text elem (len %d, fmt \"%s\")", strlen(buf), fmt);
        }
        fn_80145C7C(e, buf);
    }
}

void fn_801476C4(TextElem* e) {
    if (e) {
        fn_80145B94(e, e->defText);
    }
}

void fn_801476D8(const ArtElem* e, u32 color, s32 tex) {
    if (e) {
        fn_802C8BD0(tex);
        if (fn_802C8DBC(4)) {
            fn_802C8E00(color, 0, fn_801451DC(e->x, e->elemJust), fn_801452B4(e->y), 0.0f, e->u0, e->v0);
            fn_802C8E00(color, 0, fn_801451DC(e->x, e->elemJust) + fn_801452A0(e->w, e->elemJust),
                        fn_801452B4(e->y), 0.0f, e->u1, e->v0);
            fn_802C8E00(color, 1, fn_801451DC(e->x, e->elemJust), fn_801452B4(e->y + e->h), 0.0f, e->u0, e->v1);
            fn_802C8E00(color, 1, fn_801451DC(e->x, e->elemJust) + fn_801452A0(e->w, e->elemJust),
                        fn_801452B4(e->y + e->h), 0.0f, e->u1, e->v1);
            fn_802C8E48();
        }
        fn_802C8D74();
    }
}

void fn_8014784C(ILayout* l, const char* box, const char* set, int part) {
    GartElem* b = l->FindBox(box);
    int art = l->FindSet(set);
    if (b && art) {
        fn_80145CF4(b, art, part);
    }
}

void fn_801478E0(ILayout* l, const char* box, int art, int part) {
    GartElem* b = l->FindBox(box);
    if (b && art) {
        fn_80145CF4(b, art, part);
    }
}

void fn_80147940(ILayout* l, GartElem* b, const char* set, int part) {
    int art = l->FindSet(set);
    if (b && art) {
        fn_80145CF4(b, art, part);
    }
}

void fn_80147CEC(TextElem* out, const TextElem* a, const TextElem* b, f32 t);
void fn_80147DC0(ArtElem* out, const ArtElem* a, const ArtElem* b, f32 t);
u32 fn_80147C50(u32 a, u32 b, f32 t);

void fn_801479A8(const TextElem* a, const TextElem* b, const char* fmt, f32 t, ...) {
    va_list args;
    char buf[0x200];
    TextElem e;
    va_start(args, t);
    vsprintf(buf, fmt, args);
    va_end(args);
    if (strlen(buf) >= 0x200) {
        fn_802E317C("ovedit: oversized text elem (len %d, fmt \"%s\")", strlen(buf), fmt);
    }
    fn_80147CEC(&e, a, b, t);
    fn_80147230(&e, "%s", buf);
}

void fn_80147ABC(const TextElem* a, const TextElem* b, const char* fmt, f32 t, ...) {
    va_list args;
    char buf[0x200];
    TextElem e;
    va_start(args, t);
    vsprintf(buf, fmt, args);
    va_end(args);
    if (strlen(buf) >= 0x200) {
        fn_802E317C("ovedit: oversized text elem (len %d, fmt \"%s\")", strlen(buf), fmt);
    }
    fn_80147CEC(&e, a, b, t);
    fn_80147314(&e, "%s", buf);
}

void fn_80147BD0(const ArtElem* a, const ArtElem* b, u32 c0, u32 c1, s32 tex, f32 t) {
    ArtElem e;
    fn_80147DC0(&e, a, b, t);
    fn_801476D8(&e, fn_80147C50(c0, c1, t), tex);
}

u32 fn_80147C50(u32 a, u32 b, f32 t) {
    u8 c0 = fn_801450A0(a & 0xFF, b & 0xFF, t);
    u8 c1 = fn_801450A0((a >> 8) & 0xFF, (b >> 8) & 0xFF, t);
    u8 c2 = fn_801450A0((a >> 16) & 0xFF, (b >> 16) & 0xFF, t);
    u8 c3 = fn_801450A0(a >> 24, b >> 24, t);
    return (c3 << 24) | (c2 << 16) | (c1 << 8) | c0;
}

void fn_80147CEC(TextElem* out, const TextElem* a, const TextElem* b, f32 t) {
    V2 p;
    f32 s;
    p.x = a->x + t * (b->x - a->x);
    p.y = a->y + t * (b->y - a->y);
    s = a->scale + t * (b->scale - a->scale);
    *out = *a;
    out->x = p.x;
    out->y = p.y;
    out->scale = s;
    out->color = fn_80147C50(a->color, b->color, t);
}

void fn_80147DC0(ArtElem* out, const ArtElem* a, const ArtElem* b, f32 t) {
    *out = *a;
    out->x = a->x + t * (b->x - a->x);
    out->y = a->y + t * (b->y - a->y);
    out->u0 = a->u0 + t * (b->u0 - a->u0);
    out->u1 = a->u1 + t * (b->u1 - a->u1);
    out->v0 = a->v0 + t * (b->v0 - a->v0);
    out->v1 = a->v1 + t * (b->v1 - a->v1);
    out->w = a->w + t * (b->w - a->w);
    out->h = a->h + t * (b->h - a->h);
}

void fn_80147EE0(ArtElem* e, f32 s) {
    f32 w = e->w * s;
    f32 h = e->h * s;
    e->h = h;
    e->w = w;
    e->x = e->x + e->w / s * 0.5f - w * 0.5f;
    e->y = e->y + e->h / s * 0.5f - h * 0.5f;
}

void fn_80147F30(f32 x, f32 y, f32 w, f32 h, u32 color, int segs, s32 tex, f32 start, f32 step, f32 u0, f32 v0,
                 f32 u1, f32 v1) {
    fn_80011630(color, segs, tex, fn_801451DC(x, 0), fn_801452B4(y), w, h, start, step, u0, v0, u1, v1);
}

void fn_80147FF0(const GartElem* e, u32 color) {
    f32 w = fn_801452A0(e->w, e->elemJust);
    fn_80011278(color, fn_801451DC(e->x, e->elemJust), fn_801452B4(e->y), w, e->h);
}

void fn_80148070(const GartElem* e, u32 color, int art) {
    f32 w = fn_801452A0(e->w, e->elemJust);
    fn_80011280(color, art, fn_801451DC(e->x, e->elemJust), fn_801452B4(e->y), w, e->h);
}

void fn_80148100(const GartElem* e) {
    fn_801470B0(e->x, e->y, e->w, e->h, e->color, -1, e->elemJust, 0.0f, 0.0f, 0.0f, 0.0f);
}

void fn_80148134(f32* ox, f32* oy, int just, f32 x, f32 y) {
    if (ox) {
        *ox = fn_801451DC(x, just);
    }
    if (oy) {
        *oy = fn_801452B4(y);
    }
}

}  // extern "C"

// Pushes an offset onto the layout origin for the lifetime of the object.
struct LayoutOffset {
    LayoutOffset(f32 dx, f32 dy);
    ~LayoutOffset(void);
    f32 ox;
    f32 oy;
};

LayoutOffset::LayoutOffset(f32 dx, f32 dy) {
    ox = lbl_8055E200;
    oy = lbl_8055E204;
    lbl_8055E200 = lbl_8055E200 + dx;
    lbl_8055E204 = lbl_8055E204 + dy;
}

LayoutOffset::~LayoutOffset(void) {
    lbl_8055E200 = ox;
    lbl_8055E204 = oy;
}

extern "C" void fn_80148218(void) {}

template <typename T>
T* FindIn(const char* name, DynArray<T>* a) {
    int i;
    for (i = 0; i < a->a.count; i++) {
        if (!strcasecmp(a->a.data[i].name, name)) {
            return &a->a.data[i];
        }
    }
    return 0;
}

TextElem* Layout::Find(const char* name) { return FindIn(name, texts); }

ArtElem* Layout::FindArt(const char* name) { return FindIn(name, arts); }

GartElem* Layout::FindBox(const char* name) { return FindIn(name, garts); }

int Layout::FindSet(const char* name) {
    GartSet* s = FindIn(name, sets);
    return s ? s->handle : 0;
}

void Layout::Save(void) {
    s32 f;
    char* base;
    strcpy(strrchr(name, '.'), ".txt");
    base = name;
    if (strchr(base, '/')) {
        base = strchr(base, '/') + 1;
    }
    f = fn_802E3D68(base);
    if (f != -1) {
        fn_802E3AD0((void*)f, "table info\r\n\tversion\t%d\r\nend\r\n", 1);
        fn_801457D0((ElemList<ArtElem>*)arts, (void*)f);
        fn_801456E8((ElemList<TextElem>*)texts, (void*)f);
        fn_8014589C((ElemList<GartElem>*)garts, (void*)f);
        fn_8014595C((ElemList<GartSet>*)sets, (void*)f);
        fn_802E3AD0((void*)f, "end\r\n");
        fn_802E3734(&f);
    }
}

void Layout::ToggleEdit(void) { editing = !editing; }

u8 Layout::IsEditing(void) { return editing; }

void Layout::Edit(void) {}

void OverlayCmd::Draw(void) {
    int i;
    if (!layout) {
        return;
    }
    for (i = 0; i < boxes->a.count; i++) {
        BoxCmd* c = &boxes->a.data[i];
        int art = layout->FindSet(c->art);
        GartElem* b = layout->FindBox(c->elem);
        if (b && art) {
            fn_80148070(b, c->color, art);
        }
    }
    for (i = 0; i < garts->a.count; i++) {
        GartCmd* c = &garts->a.data[i];
        fn_8014784C(layout, c->elem, c->art, c->idx);
    }
    for (i = 0; i < texts->a.count; i++) {
        TextCmd* c = &texts->a.data[i];
        fn_801473F8(layout, c->elem, "%s", c->text);
    }
}

