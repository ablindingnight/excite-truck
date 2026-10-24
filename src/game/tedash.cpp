// Terrain editor dashboard (0x802B5B18-0x802B8E50): the debug overlay ("tedash.val") for painting the
// track's material and road-opacity maps with .tga brushes, browsing object layers and their densities
// and scales, dumping the world model, and the stats columns other units register. Names are guesses
// apart from the strings.
#include "game/common.h"
#include <string.h>
#include <stdarg.h>

struct V2 {
    f32 x;
    f32 y;
};

struct V3 {
    f32 x, y, z;
};

struct TextElem {
    TextElem(void) {}
    const char* name;     // offset 0x0
    V2 pos;               // offset 0x4
    s32 elemJust;         // offset 0xC
    f32 scale;            // offset 0x10
    u32 just;             // offset 0x14
    u32 color;            // offset 0x18
    const char* defText;  // offset 0x1C
};

struct BoxElem {
    u8 pad0[0x14];
    f32 h;      // offset 0x14
    u32 color;  // offset 0x18
};

class ILayout {
public:
    virtual ~ILayout(void);
    virtual TextElem* Find(const char* name) = 0;
    virtual void* FindArt(const char* name) = 0;
    virtual BoxElem* FindBox(const char* name) = 0;
    virtual int FindSet(const char* name) = 0;
    virtual void Edit(void) = 0;
    virtual void Toggle(void) = 0;
    virtual int Busy(void) = 0;
    virtual void Save(void) = 0;
};

template <class T>
void SafeDelete(T*& p) {
    if (p) {
        delete p;
        p = 0;
    }
}

template <class T>
struct VecBase {
    VecBase(T* d, int n) {
        data = d;
        cap = n;
        count = 0;
    }
    ~VecBase(void) {
        data = 0;
        count = 0;
        cap = 0;
    }
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
};

template <class T, int N>
struct InlineVec : VecBase<T> {
    InlineVec(void) : VecBase<T>(buf, N) {}
    ~InlineVec(void) {}
    T buf[N];  // offset 0xC
};

struct ObjScale {
    f32 min;   // offset 0x0
    f32 max;   // offset 0x4
    u8 dirty;  // offset 0x8
};

struct ObjItem {
    const char* name;  // offset 0x0
    s32* density;      // offset 0x4
    ObjScale* scale;   // offset 0x8
};

struct World {
    const char* name;  // offset 0x0
    void* layers;      // offset 0x4
    void* objs;        // offset 0x8
};

struct Brush {
    char name[0x20];  // offset 0x0
    u32 w;            // offset 0x20
    u32 h;            // offset 0x24
    u8 data[0x100];   // offset 0x28
};

struct Stat {
    char name[0x20];  // offset 0x0
    u32* value;       // offset 0x20
    u32 peak;         // offset 0x24
};

struct Image {
    u32 w;        // offset 0x0
    u32 h;        // offset 0x4
    u32* pixels;  // offset 0x8
};

struct CursorState {
    s32 op;  // offset 0x0
    s32 x;   // offset 0x4
    s32 y;   // offset 0x8
    u32 flags;  // offset 0xC
    s32 w10;    // offset 0x10
};

struct Cursor {
    CursorState s;        // offset 0x0
    char clicked[0x20];   // offset 0x14
    char hover[0x20];     // offset 0x34
    CursorState prev;     // offset 0x54
};

struct Timer {
    Timer(f32 p) {
        last = -100.0f;
        period = p;
    }
    static f32 Now(void);
    int Waiting(void) { return Now() - period < last; }
    void Reset(void) { last = Now(); }
    f32 last;    // offset 0x0
    f32 period;  // offset 0x4
};

class ObjSrc {
public:
    virtual ~ObjSrc(void);
    virtual void* Get(int i);
};

struct Cmd {
    const char* name;
    int (*pred)(const char* name);
    void (*fn)(const char* name);
};

struct Cmd2 {
    const char* name;
    int (*pred)(const char* name);
    int (*fn)(int d, const char* name);
};

extern "C" {
int fn_801345D8(char* dst, int n, const char* fmt, ...);
int sscanf(const char* s, const char* fmt, ...);
int vsprintf(char* dst, const char* fmt, va_list args);
int strcasecmp(const char* a, const char* b);
int fn_802E2C20(void);  // milliseconds
World* fn_802B59A4(s32 i);
void fn_802B321C(InlineVec<ObjItem, 16>* list, u8** clump, void* objs, s32 layer);
s32 fn_802B3F0C(void* objs);
void fn_802B32E0(void* objs);
void fn_802B4E44(void* objs);
void fn_802B4E3C(s32 layer);
s32 fn_802B05DC(void);
int fn_802B05E4(void);
void fn_802B0288(void);
s32 fn_802B1C14(void* layers);
const char* fn_802B1C1C(void* layers, s32 i);
void fn_80128684(void);
void fn_80128678(void);
u8 fn_80128690(void);
ObjSrc* fn_80156A40(void);
void fn_802A7BDC(int fd, void* obj);
int fn_802E3D68(const char* name);
void fn_802E3AD0(int fd, const char* fmt, ...);
void fn_802E3734(int* fd);
int fn_802E3BB4(const char* pattern, char* name, int n);
int fn_802E3C24(int h, char* name, int n);
void fn_802E3CA4(int h);
f32 fn_801DCF48(V3* nrm, f32 x, f32 z);
void fn_802BBEE4(void);
void fn_802C21C0(void);
void fn_802C21DC(s32 x, s32 z, s32 v);
f32 fn_800108C8(const char* text, f32 scale);
f32 fn_8001090C(f32 scale);
void fn_80147230(TextElem* e, const char* fmt, ...);
void fn_80147FF0(BoxElem* b, u32 color);
void fn_801476C4(TextElem* e);
void fn_801473F8(ILayout* l, const char* name, const char* fmt, ...);
ILayout* fn_80145E70(const char* name);
void fn_80148F4C(void);
Image* fn_802B5240(const char* path);
void fn_802B52B4(Image* img);
void fn_80204048(V2* a, V2* b);
void fn_8020404C(void);
void fn_80204044(s32 v, f32 x, f32 z);
int fn_80204054(V3* out, int a, int b, void* cam, s32 x, s32 y);
void* fn_802C86DC(void);
void fn_802BE8B4(s16 x, s16 z, s16 size);
int fn_802BC09C(s32 x, s32 z, s32 layer, s32 a, f32 t);
void fn_801E41D8(V3* a, V3* b, u32 color, f32 w);
s32 fn_8026E214(void* fn);
void fn_802C6CF8(s32 h);
void fn_802C7040(s32 h);
void fn_80230B5C(void* m);
void fn_802C8638(void* m);
void fn_802C974C(f32 a, f32 b, f32 c);
void fn_8001123C(int a);
void fn_80011A2C(void);
int fn_80127A50(CursorState* c);
void __register_global_object(void* obj, void* dtor, void* chain);
}

extern s32 lbl_8055CFF0;  // screen width
extern s32 lbl_8055CFF4;  // screen height

inline f32 Timer::Now(void) { return (f32)fn_802E2C20() / 1000.0f; }

static inline void StrCopy(char* d, const char* s, int n) {
    while ((*d++ = *s++) != 0) {
        if (--n <= 0) {
            d[-1] = 0;
            break;
        }
    }
}

ModuleTag lbl_8055F9F0;
UnkStatic123s lbl_804BE7B8;
UnkStaticFFFF lbl_8055F9F8;
UnkStatic7F lbl_8055F9FC;
s32 lbl_8055FA00;
Timer lbl_8055FA08(2.0f);  // layer list
Timer lbl_8055FA10(2.0f);  // brush list
Timer lbl_8055FA18(5.0f);  // brush rescan
Timer lbl_8055FA20(0.05f);  // paint rate
s32 lbl_8055FA28;  // current layer
s32 lbl_8055FA2C;  // current brush
u8 lbl_8055FA30;   // show peaks
u8 lbl_8055FA31;
bool lbl_8055FA32;   // show world map
s32 lbl_8055FA34;
s32 lbl_8055FA38;  // mode
s32 lbl_8055FA3C;  // world
s32 lbl_8055FA40;  // object layer
s32 lbl_8055FA44;  // selected object
ILayout* lbl_8055FA48;
s16 lbl_8055FA50[4];  // dirty rect
u8 lbl_8055FA58;
bool lbl_8055FA59;

InlineVec<Brush, 16> lbl_804BE7D8;
InlineVec<Stat, 32> lbl_804BFA70;
Cursor lbl_804BFF80;

s32 lbl_8055CEA0 = 1;

extern "C" {

const char* fn_802B5B18(int i) {
    const char* names[4] = {"Objects", "Advanced", "Material-Map", "Road-Opacity"};
    if (i >= 0 && (u32)i < 4) {
        return names[i];
    }
    return "<unk>";
}

World* fn_802B5B6C(void) { return fn_802B59A4(lbl_8055FA3C); }

int fn_802B5B74(ObjItem* out) {
    InlineVec<ObjItem, 16> list;
    fn_802B321C(&list, 0, fn_802B5B6C()->objs, lbl_8055FA40);
    s32 i = lbl_8055FA44;
    int ok = i >= 0 && i < list.count;
    if (ok) {
        *out = list.data[i];
    }
    return ok;
}

void fn_802B5C24(int d) {
    s32 l;
    do {
        l = (lbl_8055FA40 + d + 7) % 7;
        lbl_8055FA40 = l;
    } while (l >= fn_802B3F0C(fn_802B5B6C()->objs) && l != 6);
}

void fn_802B5CAC(const char* name) { fn_802B5C24(1); }

void fn_802B5CB4(int d) { lbl_8055FA3C += d; }

void fn_802B5CC4(const char* name) { fn_802B5CB4(1); }

void fn_802B5CCC(const char* name) {
    bool* clump;
    fn_802B321C(0, (u8**)&clump, fn_802B5B6C()->objs, lbl_8055FA40);
    *clump = !*clump;
    fn_802B32E0(fn_802B5B6C()->objs);
    fn_802B4E44(fn_802B5B6C()->objs);
}

int fn_802B5D2C(const char* prefix, const char* s) {
    char fmt[0x40];
    int v;
    fn_801345D8(fmt, 0x40, "%s:%%d", prefix);
    if (sscanf(s, fmt, &v) != 1) {
        v = -1;
    }
    return v;
}

int fn_802B5D94(const char* s) { return fn_802B5D2C("object", s); }

int fn_802B5DA0(const char* s) { return fn_802B5D94(s) != -1; }

int fn_802B5DD0(const char* s) {
    int i = fn_802B5D2C("layer", s);
    int r = 0;
    if (i != -1 && i != fn_802B05DC()) {
        r = 1;
    }
    return r;
}

void fn_802B5E24(const char* s) {
    lbl_8055FA28 = fn_802B5D2C("layer", s);
    lbl_8055FA08.last = -100.0f;
}

void fn_802B5E58(const char* s) {
    lbl_8055FA2C = fn_802B5D2C("brush", s);
    lbl_8055FA10.last = -100.0f;
}

int fn_802B5E8C(const char* s) { return fn_802B5D2C("brush", s) != -1; }

void fn_802B5EC4(const char* s) { lbl_8055FA44 = fn_802B5D94(s); }

void fn_802B5EE8(const char* s) {
    lbl_8055FA18.last = -100.0f;
    lbl_8055FA38 = (lbl_8055FA38 + 1) % 4;
    lbl_804BE7D8.count = 0;
}

void fn_802B5F24(int fd) {
    fn_80128684();
    for (int i = 0; i < 10; i++) {
        void* obj = fn_80156A40()->Get(0);
        if (!obj) {
            break;
        }
        fn_802A7BDC(fd, obj);
    }
    fn_80128678();
}

void fn_802B5F98(const char* name) {
    int fd = fn_802E3D68("worldmodel.mml");
    if (fd == -1) {
        return;
    }
    fn_802E3AD0(fd, "surf\r\n");
    fn_802E3AD0(fd, "\ttex worldmap.tex\r\n");
    fn_802E3AD0(fd, "\ttype normal\r\n");
    int n = 0;
    for (int x = -0x200; x <= 0x200; x += 8) {
        for (int z = -0x200; z <= 0x200; z += 8) {
            V3 nrm;
            f32 fx = (f32)x * 2.0f;
            f32 fz = (f32)z * 2.0f;
            f32 h = fn_801DCF48(&nrm, fx, fz);
            f32 u = (f32)(x + 0x200) * 0.0009765625f;
            f32 v = (f32)(z + 0x200) * 0.0009765625f;
            fn_802E3AD0(fd, "\tpt %.2f,%.2f,%.2f\r\n", fx, h, fz);
            fn_802E3AD0(fd, "\tuv %.3f, %.3f\r\n", u, v);
            fn_802E3AD0(fd, "\tn %.3f, %.3f, %.3f\r\n", nrm.x, nrm.y, nrm.z);
            if (x > -0x200 && z > -0x200) {
                fn_802E3AD0(fd, "\ttri %d, %d, %d\r\n", n, n - 1, n - 0x81);
                fn_802E3AD0(fd, "\ttri %d, %d, %d\r\n", n - 0x81, n - 1, n - 0x82);
            }
            n++;
        }
    }
    fn_802E3AD0(fd, "end\r\n");
    fn_802B5F24(fd);
    fn_802E3AD0(fd, "end\r\n");
    fn_802E3734(&fd);
}

void fn_802B618C(const char* name) {
    fn_802B59A4(lbl_8055FA3C);
    fn_80128684();
    fn_80128678();
}

void fn_802B61B8(const char* name) {
    switch (lbl_8055FA38) {
    case 2:
        fn_802BBEE4();
        break;
    case 3:
        fn_802C21C0();
        break;
    }
}

void fn_802B61E0(const char* name) { lbl_8055FA08.Reset(); }

void fn_802B6228(const char* name) { lbl_8055FA10.Reset(); }

int fn_802B6270(void) { return lbl_8055FA40 == 7; }

void fn_802B6284(const char* name) { lbl_8055FA40 = lbl_8055FA40 == 7 ? 6 : 7; }

static Cmd lbl_8034A670[4] = {
    {"layer_dropdown", 0, fn_802B61E0},
    {0, fn_802B5DD0, fn_802B61E0},
    {"brush_dropdown", 0, fn_802B6228},
    {0, fn_802B5E8C, fn_802B6228},
};

void fn_802B62A4(TextElem* e) {
    const Cmd* c = lbl_8034A670;
    for (u32 i = 0; i < 4; i++, c++) {
        if ((c->name && strcmp(c->name, e->name) == 0) || (c->pred && c->pred(e->name))) {
            lbl_8034A670[i].fn(e->name);
            break;
        }
    }
}

static Cmd lbl_8034A70C[11] = {
    {"cur_material", 0, fn_802B5CC4},
    {"objects", 0, fn_802B5CAC},
    {"object_clump", 0, fn_802B5CCC},
    {"mode_button", 0, fn_802B5EE8},
    {"generate_wmap", 0, fn_802B618C},
    {"save_matmap", 0, fn_802B61B8},
    {"toggle_objects", 0, fn_802B6284},
    {"dump_world_model", 0, fn_802B5F98},
    {0, fn_802B5DA0, fn_802B5EC4},
    {0, fn_802B5DD0, fn_802B5E24},
    {0, fn_802B5E8C, fn_802B5E58},
};

void fn_802B6358(TextElem* e) {
    const Cmd* c = lbl_8034A70C;
    for (u32 i = 0; i < 11; i++, c++) {
        if ((c->name && strcmp(c->name, e->name) == 0) || (c->pred && c->pred(e->name))) {
            lbl_8034A70C[i].fn(e->name);
            break;
        }
    }
}

int fn_802B640C(const TextElem* e, const char* text) {
    f32 w = fn_800108C8(text, e->scale);
    f32 h = fn_8001090C(e->scale);
    u32 just = e->just;
    f32 x = e->pos.x;
    f32 right = x + w;
    f32 off;
    if (just & 1) {
        off = 0.0f;
    } else if (just & 2) {
        off = -w;
    } else {
        off = -w * 0.5f;
    }
    int r = 0;
    int inY = 0;
    int inX = 0;
    if ((f32)lbl_804BFF80.s.x >= x + off && (f32)lbl_804BFF80.s.x < right + off) {
        inX = 1;
    }
    if (inX) {
        if ((f32)lbl_804BFF80.s.y >= e->pos.y) {
            inY = 1;
        }
    }
    if (inY) {
        if ((f32)lbl_804BFF80.s.y < e->pos.y + h) {
            r = 1;
        }
    }
    return r;
}

void fn_802B6568(TextElem* e, const char* fmt, ...) {
    char buf[0x100];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    e->color = 0xFFD8D8D8;
    Cursor* c = &lbl_804BFF80;
    if (c->clicked[0]) {
        if (strcasecmp(c->clicked, e->name) == 0) {
            e->color = 0xFFFFFF00;
        }
    } else if (c->hover[0]) {
        if (strcasecmp(c->hover, e->name) == 0) {
            e->color = 0xFFFFFFFF;
        }
    } else if (fn_802B640C(e, buf)) {
        {
            const char* src = e->name;
            int left = 0x20;
            char* dst = c->hover;
            while ((*dst++ = *src++) != 0) {
                if (--left <= 0) {
                    dst[-1] = 0;
                    break;
                }
            }
        }
        e->color = 0xFFFFFFFF;
        fn_802B62A4(e);
        if (lbl_804BFF80.s.op == 0) {
            fn_802B6358(e);
            {
                const char* src = e->name;
                int left = 0x20;
                char* dst = lbl_804BFF80.clicked;
                while ((*dst++ = *src++) != 0) {
                    if (--left <= 0) {
                        dst[-1] = 0;
                        break;
                    }
                }
            }
            lbl_804BFF80.prev = lbl_804BFF80.s;
        }
    }
    fn_80147230(e, "%s", buf);
}

void fn_802B676C(void) {
    World* w = fn_802B5B6C();
    fn_802B6568(lbl_8055FA48->Find("cur_material"), "Material: %s", w->name);
}

void fn_802B67C8(const char* name) {
    BoxElem* b = lbl_8055FA48->FindBox(name);
    fn_80147FF0(b, b->color);
}

void fn_802B6804(void) {
    fn_802B67C8("stats_box");
    TextElem* num = lbl_8055FA48->Find("stats_colnumber");
    TextElem* name = lbl_8055FA48->Find("stats_colname");
    f32 row = lbl_8055FA48->FindBox("stats_col")->h;
    for (int i = 0; i < lbl_804BFA70.count; i++) {
        Stat* s = &lbl_804BFA70.data[i];
        if (s->peak < *s->value) {
            s->peak = *s->value;
        }
        TextElem t = *num;
        t.pos.y += row * (f32)i;
        fn_80147230(&t, "%d", lbl_8055FA30 ? s->peak : *s->value);
        TextElem t2 = *name;
        t2.pos.y += row * (f32)i;
        fn_80147230(&t2, "%s", s);
    }
}

void fn_802B69FC(void) {
    fn_802B6568(lbl_8055FA48->Find("generate_wmap"), "Generate World Map");
    fn_802B6568(lbl_8055FA48->Find("dump_world_model"), "Generate World Model");
}

const char* fn_802B6A70(int mode) {
    switch (mode) {
    case 2:
        return "brush\\";
    case 3:
        return "roadbrush\\";
    case 5:
        return "excavate\\";
    }
    return "";
}

void fn_802B6AC0(void) {
    char name[0x20];
    char path[0x40];
    char pat[0x40];
    if (lbl_8055FA18.Waiting()) {
        return;
    }
    lbl_8055FA18.Reset();
    lbl_804BE7D8.count = 0;
    const char* dir = fn_802B6A70(lbl_8055FA38);
    fn_801345D8(pat, 0x40, "%s*.tga", dir);
    int h = fn_802E3BB4(pat, name, 0x20);
    if (h == -1) {
        return;
    }
    do {
        Brush* b = &lbl_804BE7D8.data[lbl_804BE7D8.count++];
        StrCopy(b->name, name, 0x20);
        if (strchr(b->name, '.')) {
            *strchr(b->name, '.') = 0;
        }
        b->h = 0;
        b->w = 0;
        u8 on = fn_80128690();
        if (!on) {
            fn_80128684();
        }
        fn_801345D8(path, 0x40, "%s%s", dir, name);
        Image* img = fn_802B5240(path);
        if (img) {
            b->w = img->w;
            b->h = img->h;
            if (img->w * img->h > 0x100) {
                b->w = b->w > 16 ? 16 : b->w;
                b->h = b->h > 16 ? 16 : b->h;
            }
            for (u32 x = 0; x < b->w; x++) {
                for (u32 y = 0; y < b->h; y++) {
                    u32 px = img->pixels[(img->h - 1 - y) * img->w + x];
                    u32 m = (px >> 8) & 0xFF;
                    if ((px & 0xFF) > m) {
                        m = px & 0xFF;
                    }
                    if (((px >> 16) & 0xFF) > m) {
                        m = (px >> 16) & 0xFF;
                    }
                    if ((px >> 24) > m) {
                        m = px >> 24;
                    }
                    b->data[y * b->w + x] = m;
                }
            }
            fn_802B52B4(img);
        }
        if (!on) {
            fn_80128678();
        }
        if (b->w == 0) {
            char* p = b->name;
            int n = 0x20;
            while (*p) {
                n--;
                p++;
            }
            if (n > 0) {
                StrCopy(p, "-BAD", n);
            }
        }
        if (lbl_804BE7D8.count + 1 > lbl_804BE7D8.cap) {
            break;
        }
    } while (fn_802E3C24(h, name, 0x20) == 0);
    fn_802E3CA4(h);
}

int fn_802B6E00(int x, int z) { return x < -0x200 || x > 0x200 || z < -0x200 || z > 0x200; }

int fn_802B6E30(void) {
    switch (lbl_8055FA38) {
    case 2:
        return 2;
    case 3:
        return 4;
    case 5:
        return 1;
    }
    return 0;
}

s16 fn_802B6E7C(s16 base, int i, u32 n) { return base + (i - n / 2) * (s16)fn_802B6E30(); }

s16 fn_802B6ED8(s16 base, int i, u32 n) { return base + (i - n / 2) * (s16)fn_802B6E30(); }

static inline int Round(f32 x) { return (int)(x < 0.0f ? x - 0.5f : x + 0.5f); }

void fn_802B6F34(V3* pos, Brush* b, void (*cb)(s16, s16, int)) {
    if (b->w == 0) {
        return;
    }
    s16 cx = (s16)Round(pos->x / 2.0f) & ~(fn_802B6E30() - 1);
    s16 cz = (s16)Round(pos->z / 2.0f) & ~(fn_802B6E30() - 1);
    u32 nw = lbl_8055CEA0 * b->w;
    u32 nh = lbl_8055CEA0 * b->h;
    if (lbl_8055FA38 == 5) {
        V2 a;
        V2 c;
        a.y = 2.0f * (f32)fn_802B6ED8(cz, 0, nh);
        a.x = 2.0f * (f32)fn_802B6E7C(cx, 0, nw);
        c.y = 2.0f * (f32)fn_802B6ED8(cz, nh - 1, nh);
        c.x = 2.0f * (f32)fn_802B6E7C(cx, nw - 1, nw);
        fn_80204048(&a, &c);
    }
    for (u32 i = 0; i < nw; i++) {
        for (u32 j = 0; j < nh; j++) {
            s16 gx = fn_802B6E7C(cx, i, nw);
            s16 gz = fn_802B6E7C(cz, j, nh);
            if (fn_802B6E00(gx, gz)) {
                continue;
            }
            u32 k = lbl_8055CEA0;
            u32 bi = i / k;
            u32 bj = j / k;
            u32 ri = i - bi * k;
            u32 rj = j - bj * k;
            int v;
            if (ri == 0 && rj == 0) {
                v = b->data[bj * b->w + bi];
            } else {
                u32 i1 = bi + 1 < b->w - 1 ? bi + 1 : b->w - 1;
                u32 j1 = bj + 1 < b->h - 1 ? bj + 1 : b->h - 1;
                f32 tx = (f32)ri / (f32)k;
                f32 a = b->data[bj * b->w + bi];
                f32 c = b->data[bj * b->w + i1];
                f32 d = b->data[j1 * b->w + bi];
                f32 e = b->data[j1 * b->w + i1];
                f32 top = a + tx * (c - a);
                f32 bot = d + tx * (e - d);
                f32 ty = (f32)rj / (f32)k;
                v = (int)(top + ty * (bot - top));
            }
            cb(gx, gz, (u8)v);
        }
    }
    if (lbl_8055FA38 == 5) {
        fn_8020404C();
    }
}

void fn_802B7300(int x, int z, int v, u32 color, f32 h) {
    V3 p;
    p.x = 2.0f + (f32)x * 2.0f;
    p.y = 0.0f;
    p.z = 2.0f + (f32)z * 2.0f;
    p.y = 0.5f + fn_801DCF48(0, p.x, p.z);
    V3 q = p;
    p.y += h;
    f32 w = 2.0f * (f32)(s16)fn_802B6E30();
    if (v == 0) {
        color = 0xFF000000;
        color = (0x20 << 24) | (color & 0xFFFFFF);
        fn_801E41D8(&p, &q, color, w);
    }
}

void fn_802B7420(s16 x, s16 z, int v) { fn_802B7300(x, z, v, 0xFFFFFFFF, 0.2f); }

void fn_802B7434(void) {
    if (lbl_8055FA38 != 2) {
        return;
    }
    for (s16 s = 4; s <= 0x40; s *= 2) {
        for (s16 x = s - 0x200; x < 0x200; x += s * 2) {
            for (s16 z = s - 0x200; z < 0x200; z += s * 2) {
                if (x - s <= lbl_8055FA50[1] && x + s >= lbl_8055FA50[0] && z - s <= lbl_8055FA50[3] &&
                    z + s >= lbl_8055FA50[2]) {
                    fn_802BE8B4(x, z, s);
                }
            }
        }
    }
}

void fn_802B7518(s16 x, s16 z, int v) {
    switch (lbl_8055FA38) {
    case 2:
        if (v) {
            if (fn_802BC09C(x, z, lbl_8055FA28, 0xFF, (f32)(u32)v / 255.0f)) {
                lbl_8055FA50[0] = lbl_8055FA50[0] < x ? lbl_8055FA50[0] : x;
                lbl_8055FA50[1] = lbl_8055FA50[1] > x ? lbl_8055FA50[1] : x;
                lbl_8055FA50[2] = lbl_8055FA50[2] < z ? lbl_8055FA50[2] : z;
                lbl_8055FA50[3] = lbl_8055FA50[3] > z ? lbl_8055FA50[3] : z;
            } else {
                fn_802B7300(x, z, v, 0xFFFF0000, 3.0f);
            }
        }
        break;
    case 3:
        fn_802C21DC(x, z, v);
        break;
    case 5:
        fn_80204044(v, (f32)x * 2.0f, (f32)z * 2.0f);
        break;
    }
}

int fn_802B7694(void) {
    int r = 0;
    if (!lbl_8055FA20.Waiting()) {
        lbl_8055FA20.Reset();
        r = 1;
    }
    return r;
}

void fn_802B773C(void) {
    u8 was = lbl_8055FA58;
    lbl_8055FA58 = 0;
    Cursor* c = &lbl_804BFF80;
    if (c->hover[0] == 0 && lbl_8055FA2C >= 0 && lbl_8055FA2C < lbl_804BE7D8.count) {
        V3 hit;
        if (fn_80204054(&hit, 0, 0, fn_802C86DC(), c->s.x, c->s.y)) {
            if (c->s.flags & 1) {
                if (lbl_804BFF80.s.op == 0 || was) {
                    lbl_8055FA58 = 1;
                    if (fn_802B7694()) {
                        lbl_8055FA50[0] = 0x400;
                        lbl_8055FA50[2] = 0x400;
                        lbl_8055FA50[3] = -0x400;
                        lbl_8055FA50[1] = -0x400;
                        fn_802B6F34(&hit, &lbl_804BE7D8.data[lbl_8055FA2C], fn_802B7518);
                        fn_802B7434();
                    }
                }
            } else if (!(c->s.flags & 2)) {
                fn_802B6F34(&hit, &lbl_804BE7D8.data[lbl_8055FA2C], fn_802B7420);
            }
        }
    }
}

void fn_802B7884(const char* label) {
    fn_802B6568(lbl_8055FA48->Find("save_matmap"), label);
    const char* t = fn_802B6270() ? "Objects On" : "Objects Off";
    fn_802B6568(lbl_8055FA48->Find("toggle_objects"), t);
}

void fn_802B7918(void) {
    f32 row = lbl_8055FA48->FindBox("matmap_col")->h;
    TextElem t = *lbl_8055FA48->Find("brush_col");
    char nm[0x20];
    fn_802B6568(lbl_8055FA48->Find("brush_dropdown"), "Brushes");
    if (lbl_8055FA10.Waiting()) {
        fn_802B6AC0();
        if (lbl_804BE7D8.count > 0) {
            for (int i = 0; i < lbl_804BE7D8.count; i++) {
                fn_801345D8(nm, 0x20, "brush:%d", i);
                t.name = nm;
                fn_802B6568(&t, "%s", &lbl_804BE7D8.data[i]);
                t.pos.y += row;
            }
        } else {
            fn_80147230(&t, "No Brushes");
        }
    } else {
        const char* s = "<no brush>";
        if (lbl_8055FA2C >= 0 && lbl_8055FA2C < lbl_804BE7D8.count) {
            s = lbl_804BE7D8.data[lbl_8055FA2C].name;
        }
        fn_80147230(&t, "%s", s);
    }
}

void fn_802B7B08(void) {
    f32 row = lbl_8055FA48->FindBox("matmap_col")->h;
    fn_802B6568(lbl_8055FA48->Find("layer_dropdown"), "Layers");
    TextElem t = *lbl_8055FA48->Find("layer_col");
    char nm[0x20];
    if (lbl_8055FA08.Waiting()) {
        void* layers = fn_802B5B6C()->layers;
        if (fn_802B1C14(layers) > 0) {
            for (int i = 0; i < fn_802B1C14(layers); i++) {
                fn_801345D8(nm, 0x20, "layer:%d", i);
                t.name = nm;
                fn_802B6568(&t, "%s", fn_802B1C1C(layers, i));
                t.pos.y += row;
            }
        } else {
            fn_80147230(&t, "No Layers");
        }
    } else {
        fn_80147230(&t, "%s", fn_802B1C1C(fn_802B5B6C()->layers, lbl_8055FA28));
    }
}

void fn_802B7CF0(void) {
    fn_802B7884("Save Road-Opacity Map");
    fn_802B7918();
}

void fn_802B7D1C(void) {
    fn_802B7884("Save Material Maps");
    fn_802B7918();
    fn_802B7B08();
}

void fn_802B7D4C(void) {
    if (lbl_8055FA40 == 6) {
        fn_802B6568(lbl_8055FA48->Find("objects"), "All Objects");
        return;
    }
    World* w = fn_802B5B6C();
    InlineVec<ObjItem, 16> list;
    u8* clump = 0;
    fn_802B321C(&list, &clump, w->objs, lbl_8055FA40);
    const char* cl = "Dispersed";
    if (*clump) {
        cl = "Clumpy";
    }
    fn_802B6568(lbl_8055FA48->Find("object_clump"), "%s", cl);
    TextElem num = *lbl_8055FA48->Find("objcolnumber");
    TextElem name = *lbl_8055FA48->Find("objcolname");
    f32 row = lbl_8055FA48->FindBox("stats_col")->h;
    char nm[0x20];
    for (int i = 0; i < list.count; i++) {
        ObjItem* it = &list.data[i];
        fn_801345D8(nm, 0x20, "object:%d", i);
        TextElem t = num;
        t.name = nm;
        t.pos.y += row * (f32)i;
        fn_802B6568(&t, "%d", *it->density);
        TextElem t2 = name;
        t2.name = nm;
        t2.pos.y += row * (f32)i;
        fn_802B6568(&t2, "%s", it->name);
    }
    ObjItem it;
    if (fn_802B5B74(&it)) {
        fn_802B67C8("object_box");
        fn_801476C4(lbl_8055FA48->Find("scale_title"));
        fn_801473F8(lbl_8055FA48, "obj_name", "%s", it.name);
        fn_802B6568(lbl_8055FA48->Find("obj_scale_min"), "%.2f", it.scale->min);
        fn_802B6568(lbl_8055FA48->Find("obj_scale_max"), "%.2f", it.scale->max);
    }
    fn_802B6568(lbl_8055FA48->Find("objects"), "Object layer: %d", lbl_8055FA40 + 1);
}

int fn_802B8110(int d, const char* s) {
    int changed = 0;
    int idx = fn_802B5D94(s);
    World* w = fn_802B5B6C();
    InlineVec<ObjItem, 16> list;
    fn_802B321C(&list, 0, w->objs, lbl_8055FA40);
    s32* dp = list.data[idx].density;
    int v = *dp + d / 2;
    int c = v > 0 ? v : 0;
    if (*dp != (c < 0x80 ? c : 0x80)) {
        c = v > 0 ? v : 0;
        *dp = c < 0x80 ? c : 0x80;
        fn_802B32E0(w->objs);
        fn_802B4E44(w->objs);
        changed = 1;
    }
    return changed;
}

f32 fn_802B822C(int d) { return 0.05f * (f32)d; }

int fn_802B825C(int d, const char* s) {
    int changed = 0;
    ObjItem it;
    if (fn_802B5B74(&it)) {
        ObjScale* sc = it.scale;
        f32 v = sc->max;
        f32 n = sc->min + fn_802B822C(d);
        v = v >= n ? n : v;
        v = v > 0.05f ? v : 0.05f;
        if (v != sc->min) {
            sc->min = v;
            changed = 1;
            it.scale->dirty = 1;
        }
    }
    return changed;
}

int fn_802B8310(int d, const char* s) {
    int changed = 0;
    f32 v = 10.0f;
    ObjItem it;
    if (fn_802B5B74(&it)) {
        ObjScale* sc = it.scale;
        f32 lo = sc->min;
        f32 n = sc->max + fn_802B822C(d);
        v = v >= n ? n : v;
        v = v > lo ? v : lo;
        if (v != sc->max) {
            sc->max = v;
            changed = 1;
            it.scale->dirty = 1;
        }
    }
    return changed;
}

static Cmd2 lbl_8034A95C[3] = {
    {0, fn_802B5DA0, fn_802B8110},
    {"obj_scale_min", 0, fn_802B825C},
    {"obj_scale_max", 0, fn_802B8310},
};

void fn_802B83CC(void) {
    Cursor* c = &lbl_804BFF80;
    if (!c->clicked[0]) {
        return;
    }
    int d = c->s.x - c->prev.x;
    if (!d) {
        return;
    }
    const Cmd2* m = lbl_8034A95C;
    for (u32 i = 0; i < 3; i++, m++) {
        if ((m->name && strcmp(m->name, c->clicked) == 0) || (m->pred && m->pred(c->clicked))) {
            if (lbl_8034A95C[i].fn(d, lbl_804BFF80.clicked)) {
                lbl_804BFF80.prev = lbl_804BFF80.s;
            }
            break;
        }
    }
}

void fn_802B84E4(CursorState* c) {
    c->x = c->x * 640 / lbl_8055CFF0;
    c->y = c->y * 480 / lbl_8055CFF4;
}

void fn_802B8510(int op) {
    if ((u32)(op - 6) > 1) {
        return;
    }
    if (lbl_8055FA2C < 0 || lbl_8055FA2C >= lbl_804BE7D8.count) {
        return;
    }
    Brush* b = &lbl_804BE7D8.data[lbl_8055FA2C];
    Brush tmp = *b;
    u32 tw = tmp.w;
    u32 th = tmp.h;
    b->w = th;
    b->h = tw;
    for (u32 x = 0; x < b->w; x++) {
        for (u32 y = 0; y < b->h; y++) {
            if (op == 6) {
                b->data[x + y * b->w] = tmp.data[tw * (x + 1) - 1 - y];
            } else {
                b->data[x + y * b->w] = tmp.data[y + (th - x - 1) * tw];
            }
        }
    }
}

void fn_802B8624(CursorState* in) {
    lbl_804BFF80.s.op = 8;
    if (in) {
        lbl_804BFF80.s = *in;
        fn_802B84E4(&lbl_804BFF80.s);
        fn_802B83CC();
        fn_802B8510(lbl_804BFF80.s.op);
    }
}

void fn_802B869C(void) {
    if (lbl_8055FA48->Busy() == 0) {
        CursorState c;
        if (fn_80127A50(&c)) {
            fn_802B8624(&c);
        } else {
            fn_802B8624(0);
        }
    }
}

void fn_802B86F8(void) {
    lbl_804BFF80.hover[0] = 0;
    if (lbl_804BFF80.clicked[0] && !(lbl_804BFF80.s.flags & 1)) {
        lbl_804BFF80.clicked[0] = 0;
    }
}

int fn_802B8728(void) { return lbl_804BFF80.hover[0] == 0; }

void fn_802B8744(void) {
    lbl_8055FA48 = fn_80145E70("tedash.val");
    lbl_8055FA40 = 6;
    lbl_804BE7D8.count = 0;
    lbl_8055FA00 = fn_8026E214((void*)fn_802B8728);
    lbl_8055FA34 = -1;
}

void fn_802B87A0(void) {
    fn_8026E214((void*)lbl_8055FA00);
    SafeDelete(lbl_8055FA48);
    fn_80148F4C();
    if (lbl_8055FA34 != -1) {
        fn_802C6CF8(lbl_8055FA34);
        lbl_8055FA34 = -1;
    }
    lbl_804BFA70.count = 0;
}

void fn_802B87F4(void) {
    if (lbl_8055FA48) {
        fn_802B869C();
        fn_802B59A4(lbl_8055FA3C);
        fn_802B0288();
        if (lbl_8055FA38 != 1 && lbl_8055FA40 != 6) {
            fn_802B4E3C(lbl_8055FA40);
        }
    }
}

void fn_802B8848(void) {
    u8 m[0x30];
    if (!lbl_8055FA48) {
        return;
    }
    if ((u32)(lbl_8055FA38 - 2) <= 1) {
        fn_802B773C();
    }
    if (lbl_8055FA32 && lbl_8055FA34 != -1) {
        fn_802C7040(lbl_8055FA34);
        fn_80230B5C(m);
        fn_802C8638(m);
        fn_802C974C(1.0f, 1000.0f, 1.4396f);
    }
    fn_802B86F8();
    fn_802B676C();
    if (lbl_8055FA38 != 2 && lbl_8055FA38 != 3) {
        fn_802B6804();
    }
    switch (lbl_8055FA38) {
    case 0:
        fn_802B7D4C();
        break;
    case 1:
        fn_802B69FC();
        break;
    case 2:
        fn_802B7D1C();
        break;
    case 3:
        fn_802B7CF0();
        break;
    }
    const char* mode = fn_802B5B18(lbl_8055FA38 % 4);
    fn_802B6568(lbl_8055FA48->Find("mode_button"), mode);
    lbl_8055FA48->Edit();
    fn_8001123C(0);
    fn_80011A2C();
}

int fn_802B8998(int key) {
    ILayout* l = lbl_8055FA48;
    if (!l) {
        return 0;
    }
    switch (key) {
    case 0x125:
    case 0x127:
        if (lbl_8055FA38 == 0) {
            fn_802B5C24(key == 0x125 ? -1 : 1);
        }
        lbl_804BFF80.s.op = 8;
        lbl_8055FA44 = -1;
        lbl_804BFF80.hover[0] = 0;
        lbl_804BFF80.clicked[0] = 0;
        break;
    case 5:
        l->Toggle();
        break;
    case 'e':
        fn_802B8510(6);
        break;
    case 's':
        l->Save();
        break;
    case 'n':
        lbl_8055FA59 = !lbl_8055FA59;
        break;
    case 'w':
        lbl_8055FA32 = !lbl_8055FA32;
        break;
    case 'm':
        break;
    default:
        return 0;
    }
    return 1;
}

void fn_802B8AE0(const char* name, u32* value) {
    if (fn_802B05E4()) {
        return;
    }
    Stat s;
    StrCopy(s.name, name, 0x20);
    s.value = value;
    s.peak = 0;
    lbl_804BFA70.data[lbl_804BFA70.count++] = s;
}

void fn_802B8C88(void) {
    lbl_8055FA31 = 1;
    lbl_8055CEA0 = 1;
}

void fn_802B8C98(void) { lbl_8055FA31 = 0; }

}  // extern "C"
