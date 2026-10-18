// Overlay layouts (0x80144F40-0x80149588): the .val-driven 2D layout sets used by every HUD/overlay -
// text, art and gart (solid box) elements and gart sets, their tabular save format for the in-game
// overlay editor ("OVEdit"), element lookup and the drawing helpers built on them. Names are guesses
// apart from the keys, messages and allocation tags.
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

// Text element (0x20).
struct TextElem {
    const char* name;     // offset 0x0
    f32 x;                // offset 0x4
    f32 y;                // offset 0x8
    s32 elemJust;         // offset 0xC
    f32 scale;            // offset 0x10
    u32 just;             // offset 0x14
    u32 color;            // offset 0x18
    const char* defText;  // offset 0x1C
};

// Art element (0x28): a textured quad.
struct ArtElem {
    const char* name;  // offset 0x0
    f32 x;             // offset 0x4
    f32 y;             // offset 0x8
    s32 elemJust;      // offset 0xC
    f32 w;             // offset 0x10
    f32 h;             // offset 0x14
    f32 u0;            // offset 0x18
    f32 v0;            // offset 0x1C
    f32 u1;            // offset 0x20
    f32 v1;            // offset 0x24
};

// Gart element (0x1C): a coloured box.
struct GartElem {
    const char* name;  // offset 0x0
    f32 x;             // offset 0x4
    f32 y;             // offset 0x8
    s32 elemJust;      // offset 0xC
    f32 w;             // offset 0x10
    f32 h;             // offset 0x14
    u32 color;         // offset 0x18
};

// Gart set (0xC): a named art resource.
struct GartSet {
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
u8 lbl_8055E208;                 // element positions are centred
f32 lbl_8055E200;                // x offset
f32 lbl_8055E204;                // y offset

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

void fn_80145368(TextElem* e, ValueNode* n) {
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
    fn_80145368((TextElem*)e, n);
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
    fn_80145368((TextElem*)e, n);
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
