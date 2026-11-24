// Terrain editor dashboard (0x802B5B18-0x802B8E50): the debug overlay ("tedash.val") for painting the
// track's material and road-opacity maps with .tga brushes, browsing object layers and their densities
// and scales, dumping the world model, and the stats columns other units register. Names are guesses
// apart from the strings.
#include "game/common.h"
#include "game/buildquery.h"
#include "game/ilayout.h"
#include "game/textelem.h"
#include "game/v2.h"
#include "game/gart.h"
#include "game/plainvec.h"
#include <string.h>
#include <stddef.h>
#include <stdarg.h>
#include "game/strcopy.h"

// The 2D point class (retail weak __ct__4Vec2Fff): a plain V2 with an (x, y) constructor, arguments read right to left.
class Vec2 : public V2 {
public:
    Vec2(f32 x_, f32 y_) {
        x = x_;
        y = y_;
    }
};

template <class T>
void SafeDelete(T*& p) {
    if (p) {
        delete p;
        p = 0;
    }
}

#include "game/vecbase.h"
#include "game/round.h"

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

// A rate limiter on the wall clock: waits period seconds after each Reset (not timer.h's one-shot Timer).
// Name unproven.
struct Cooldown {
    Cooldown(f32 p) {
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

struct DashCmd {
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
void ExtractTweakersFromLayer(InlineVec<ObjItem, 16>* list, u8** clump, void* objs, s32 layer);
s32 fn_802B3F0C(void* objs);
void fn_802B32E0(void* objs);
void fn_802B4E44(void* objs);
void fn_802B4E3C(s32 layer);
s32 fn_802B05DC(void);
int fn_802B05E4(void);
void fn_802B0288(void*);  // GroundSet*
s32 GetLayerCount(void* layers);
const char* fn_802B1C1C(void* layers, s32 i);
void fn_80128684(void);
void fn_80128678(void);
u8 fn_80128690(void);
ObjSrc* fn_80156A40(void);
void fn_802A7BDC(int fd, void* obj);
int CreateFileFromName(const char* name);
void fn_802E3AD0(int fd, const char* fmt, ...);
void fn_802E3734(int* fd);
int FindFirstFile(const char* pattern, char* name, int n);
int FileFindNext(int h, char* name, int n);
void FileFindClose(int h);
f32 fn_801DCF48(V3* nrm, f32 x, f32 z);
void LoadMaterialMaps(void);
void fn_802C21C0(void);
void fn_802C21DC(s32 x, s32 z, s32 v);
f32 fn_800108C8(const char* text, f32 scale);
f32 fn_8001090C(f32 scale);
void fn_80147230(TextElem* e, const char* fmt, ...);
void fn_80147FF0(GartElem* b, u32 color);
void fn_801476C4(TextElem* e);
void fn_801473F8(ILayout* l, const char* name, const char* fmt, ...);
ILayout* fn_80145E70(const char* name);
void fn_80148F4C(void);
Image* LoadTextureImage(const char* path);
void fn_802B52B4(Image* img);
void fn_80204048(V2* a, V2* b);
void fn_8020404C(void);
void fn_80204044(s32 v, f32 x, f32 z);
int fn_80204054(V3* out, int a, int b, void* cam, s32 x, s32 y);
void* fn_802C86DC(void);
void fn_802BE8B4(s16 x, s16 z, s16 size);
int UpdateLayerValueWithLerp(s32 x, s32 z, s32 layer, s32 a, f32 t);
void DrawLineSegment(V3* a, V3* b, u32 color, f32 w);
s32 fn_8026E214(void* fn);
void mrModelUnload(s32 h);
void fn_802C7040(s32 h);
void ResetTransformMatrix(void* m);
void fn_802C8638(void* m);
void SetupCameraProjection(f32 a, f32 b, f32 c);
void fn_8001123C(int a);
void fn_80011A2C(void);
int fn_80127A50(CursorState* c);
void __register_global_object(void* obj, void* dtor, void* chain);
}

extern s32 lbl_8055CFF0;  // screen width
extern s32 lbl_8055CFF4;  // screen height

inline f32 Cooldown::Now(void) { return (f32)fn_802E2C20() / 1000.0f; }


ModuleTag lbl_8055F9F0;
UnkStaticFFFF lbl_8055F9F8;
UnkStatic123s lbl_804BE7B8;
UnkStatic7F lbl_8055F9FC;
s32 lbl_8055FA00;
extern "C" const f32 lbl_80564C88;
Cooldown lbl_8055FA08(lbl_80564C88);  // layer list
Cooldown lbl_8055FA10(lbl_80564C88);  // brush list
Cooldown lbl_8055FA18(5.0f);  // brush rescan
Cooldown lbl_8055FA20(0.05f);  // paint rate
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

extern "C" {

const char* GetResourceTypeName(int i) {
    const char* names[4] = {"Objects", "Advanced", "Material-Map", "Road-Opacity"};
    if (i >= 0 && (u32)i < 4) {
        return names[i];
    }
    return "<unk>";
}

World* fn_802B5B6C(void) { return fn_802B59A4(lbl_8055FA3C); }

int fn_802B5B74(ObjItem* out) {
    InlineVec<ObjItem, 16> list;
    ExtractTweakersFromLayer(&list, 0, fn_802B5B6C()->objs, lbl_8055FA40);
    int ok = lbl_8055FA44 >= 0 && lbl_8055FA44 < list.count;
    if (ok) {
        *out = list.data[lbl_8055FA44];
    }
    return ok;
}

void fn_802B5C24(int d) {
    do {
        lbl_8055FA40 = (lbl_8055FA40 + d + 7) % 7;
    } while (lbl_8055FA40 >= fn_802B3F0C(fn_802B5B6C()->objs) && lbl_8055FA40 != 6);
}

void fn_802B5CAC(const char* name) { fn_802B5C24(1); }

void fn_802B5CB4(int d) { lbl_8055FA3C += d; }

void fn_802B5CC4(const char* name) { fn_802B5CB4(1); }

void ToggleClumpFlag(const char* name) {
    bool* clump;
    ExtractTweakersFromLayer(0, (u8**)&clump, fn_802B5B6C()->objs, lbl_8055FA40);
    *clump = !*clump;
    fn_802B32E0(fn_802B5B6C()->objs);
    fn_802B4E44(fn_802B5B6C()->objs);
}

int ParsePrefixedInteger(const char* prefix, const char* s) {
    char fmt[0x40];
    int v;
    fn_801345D8(fmt, 0x40, "%s:%%d", prefix);
    if (sscanf(s, fmt, &v) != 1) {
        v = -1;
    }
    return v;
}

int fn_802B5D94(const char* s) { return ParsePrefixedInteger("object", s); }

int fn_802B5DA0(const char* s) { return fn_802B5D94(s) != -1; }

int CheckLayerChange(const char* s) {
    int i = ParsePrefixedInteger("layer", s);
    int cur = fn_802B05DC();
    int r = 0;
    if (i != -1 && i != cur) {
        r = 1;
    }
    return r;
}

/* Helper unproven: retail creates the pooled 2.0f BEFORE the -100.0f of the functions below, so an unused static
   function with a 2.0f literal was compiled here (dead static functions keep their constants). */
static f32 CellToWorld(f32 c) { return c * 2.0f; }

void LoadLayerData(const char* s) {
    lbl_8055FA28 = ParsePrefixedInteger("layer", s);
    lbl_8055FA08.last = -100.0f;
}

void LoadBrushTexture(const char* s) {
    lbl_8055FA2C = ParsePrefixedInteger("brush", s);
    lbl_8055FA10.last = -100.0f;
}

int fn_802B5E8C(const char* s) { return ParsePrefixedInteger("brush", s) != -1; }

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

static inline f32 GridToWorld(int v, f32 cell) { return (f32)v * cell; }

void SaveWorldModelFile(const char* name) {
    int fd = CreateFileFromName("worldmodel.mml");
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
            f32 fx = GridToWorld(x, 2.0f);
            f32 fz = GridToWorld(z, 2.0f);
            f32 h = fn_801DCF48(&nrm, fx, fz);
            f32 u = (f32)(x + 0x200) / 1024.0f;
            f32 v = (f32)(z + 0x200) / 1024.0f;
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
        LoadMaterialMaps();
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

static DashCmd lbl_8034A670[4] = {
    {"layer_dropdown", 0, fn_802B61E0},
    {0, CheckLayerChange, fn_802B61E0},
    {"brush_dropdown", 0, fn_802B6228},
    {0, fn_802B5E8C, fn_802B6228},
};

void fn_802B62A4(TextElem* e) {
    const DashCmd* c = lbl_8034A670;
    for (u32 i = 0; i < 4; i++, c++) {
        if ((c->name && strcmp(c->name, e->name) == 0) || (c->pred && c->pred(e->name))) {
            lbl_8034A670[i].fn(e->name);
            break;
        }
    }
}

static DashCmd lbl_8034A70C[11] = {
    {"cur_material", 0, fn_802B5CC4},
    {"objects", 0, fn_802B5CAC},
    {"object_clump", 0, ToggleClumpFlag},
    {"mode_button", 0, fn_802B5EE8},
    {"generate_wmap", 0, fn_802B618C},
    {"save_matmap", 0, fn_802B61B8},
    {"toggle_objects", 0, fn_802B6284},
    {"dump_world_model", 0, SaveWorldModelFile},
    {0, fn_802B5DA0, fn_802B5EC4},
    {0, CheckLayerChange, LoadLayerData},
    {0, fn_802B5E8C, LoadBrushTexture},
};

void ProcessCommandElement(TextElem* e) {
    const DashCmd* c = lbl_8034A70C;
    for (u32 i = 0; i < 11; i++, c++) {
        if ((c->name && strcmp(c->name, e->name) == 0) || (c->pred && c->pred(e->name))) {
            lbl_8034A70C[i].fn(e->name);
            break;
        }
    }
}

int CheckTextElementHover(const TextElem* e, const char* text) {
    f32 w = fn_800108C8(text, e->scale);
    f32 h = fn_8001090C(e->scale);
    u32 just = e->just;
    f32 off;
    f32 x = e->pos.x;
    f32 right = x + w;
    if (just & 1) {
        off = 0.0f;
    } else if (just & 2) {
        off = -w;
    } else {
        off = -w / 2.0f;
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

void UpdateTextElementColor(TextElem* e, const char* fmt, ...) {
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
    } else if (CheckTextElementHover(e, buf)) {
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
            ProcessCommandElement(e);
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
    UpdateTextElementColor(lbl_8055FA48->Find("cur_material"), "Material: %s", w->name);
}

void fn_802B67C8(const char* name) {
    GartElem* b = lbl_8055FA48->FindBox(name);
    fn_80147FF0(b, b->color);
}

}
template <class T>
/* Helper name unproven: the address of element i; the accessor boundary is needed (in place: UpdateStatsDisplay 100 -> 99.56) */
static inline T* ElemAt(T* p, int i) {
    return &p[i];
}
extern "C" {

void UpdateStatsDisplay(void) {
    Stat* s;
    fn_802B67C8("stats_box");
    TextElem* num = lbl_8055FA48->Find("stats_colnumber");
    TextElem* name = lbl_8055FA48->Find("stats_colname");
    f32 row = (lbl_8055FA48->FindBox("stats_col"))->h;
    for (int i = 0; i < lbl_804BFA70.count; i++) {
        s = ElemAt(lbl_804BFA70.data, i);
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

void GenerateWorldMap(void) {
    UpdateTextElementColor(lbl_8055FA48->Find("generate_wmap"), "Generate World Map");
    UpdateTextElementColor(lbl_8055FA48->Find("dump_world_model"), "Generate World Model");
}

const char* GetBrushPath(int mode) {
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

/* Helper name unproven: appends src after dst's text, never writing past size bytes. */
static inline void StrAppend(char* dst, int size, const char* src) {
    char* p = dst;
    int n = size;
    while (*p) {
        n--;
        p++;
    }
    if (n > 0) {
        SafeStrCopy(p, src, n);
    }
}

// PROVISIONAL: channel-maximum helper boundary and value-input ownership are inferred.
static inline u32 IncludeBrushChannel(const u32& maximum, u32 next) { if (maximum > next) next = maximum; return next; }

// PROVISIONAL: image-pixel getter boundary, inputs, row local, and returned-value ownership are inferred.
static inline u32 ReadBrushImagePixel(const Image* image, u32 x, u32 y) { u32 row = image->h - 1 - y; return image->pixels[row * image->w + x]; }

// PROVISIONAL: column-worker inputs/boundary and column-owned borrowed row-counter lifetime are inferred.
static inline void CopyBrushColumn(Brush* b, Image* img, u32 x, u32& y) {
    for (y = 0; y < b->h; y++) {
        u32 px = ReadBrushImagePixel(img, x, y);
        u32 m = (px >> 8) & 0xFF;
        m = IncludeBrushChannel(px & 0xFF, m);
        m = IncludeBrushChannel((px >> 16) & 0xFF, m);
        m = IncludeBrushChannel(px >> 24, m);
        b->data[y * b->w + x] = m;
    }
}

// PROVISIONAL: directory-loop helper boundary, returned-state reference lifetime, and local ownership/order are inferred.
static inline void LoadBrushFiles(char* name, char* path, const char* dir, int h) {
    do {
        Brush* b = &lbl_804BE7D8.data[lbl_804BE7D8.count++];
        SafeStrCopy(b->name, name, 0x20);
        if (strchr(b->name, '.')) {
            *strchr(b->name, '.') = 0;
        }
        b->h = 0;
        b->w = 0;
        const u8& on = fn_80128690();
        if (!on) {
            fn_80128684();
        }
        fn_801345D8(path, 0x40, "%s%s", dir, name);
        Image* img = LoadTextureImage(path);
        if (img) {
            b->w = img->w;
            b->h = img->h;
            if (b->w * b->h > 0x100) {
                b->w = b->w > 16 ? 16 : b->w;
                b->h = b->h > 16 ? 16 : b->h;
            }
            for (u32 x = 0; x < b->w; x++) {
                u32 y;
                CopyBrushColumn(b, img, x, y);
            }
            fn_802B52B4(img);
        }
        if (!on) {
            fn_80128678();
        }
        if (b->w == 0) {
            StrAppend(b->name, 0x20, "-BAD");
        }
        if (lbl_804BE7D8.count + 1 > lbl_804BE7D8.cap) {
            break;
        }
    } while (FileFindNext(h, name, 0x20));
    FileFindClose(h);
}

void fn_802B6AC0(void) {
    char name[0x20];
    char pat[0x40];
    char path[0x40];
    if (lbl_8055FA18.Waiting()) {
        return;
    }
    lbl_8055FA18.Reset();
    lbl_804BE7D8.count = 0;
    const char* dir = GetBrushPath(lbl_8055FA38);
    fn_801345D8(pat, 0x40, "%s*.tga", dir);
    u32 h = FindFirstFile(pat, name, 0x20);
    if (h == -1) {
        return;
    }
    LoadBrushFiles(name, path, dir, h);
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


extern const f32 lbl_80564C88;
s32 lbl_8055CEA0 = 1;  // brush scale (.sdata: defined after the brush-list strings)

// PROVISIONAL: shared-query identity/cut locations and setup-value ownership/order are inferred.
// PROVISIONAL: coordinate/corner record, scalarization and quotient/remainder ownership/order are inferred; source origin is unproven.
struct BrushCellValues { u32 bi; u32 bj; u32 ri; u32 rj; u8 a; };

// PROVISIONAL: equivalent retail-enabled representation of the existing second policy query.
// Helper/type names and original predicates are unproven. Both existing policy sites remain always-retail.
struct BrushRetailPolicy {
    bool enabled;
    BrushRetailPolicy(bool value) : enabled(value) {}
    bool Enabled() const { return enabled; }
};
static inline BrushRetailPolicy BrushRetailEnabled() { return BrushRetailPolicy(true); }

void fn_802B6F34(V3* pos, Brush* b, void (*cb)(s16, s16, int)) {
    if (b->w == 0) {
        return;
    }
    u32 height;
    s16 signedZ;
    int maskZ;
    // PROVISIONAL: spacing and coordinate temporary ownership is inferred; pooled divisor identity is retail-verified.
    f32 spacing;
    f32 coordinateX = pos->x;
    spacing = 2.0f;
    s16 cx = (s16)Round(coordinateX / spacing) & ~(fn_802B6E30() - 1);
    f32 coordinateZ = pos->z;
    spacing = 2.0f;
    int roundedZ = Round(coordinateZ / spacing);
    maskZ = fn_802B6E30() - 1;
    signedZ = (s16)roundedZ;
    s16 cz;
    if (BuildMode() == kBuildRetail) {
        const s32 scale = lbl_8055CEA0;
        u32 width = b->w;
        height = b->h;
        u32 nw = scale * width;
        if (BrushRetailEnabled().Enabled()) {
            int mode = lbl_8055FA38;
            // PROVISIONAL: scaled-height input/result ownership is inferred.
            u32 nh = scale * height;
            int alignedZ = signedZ & ~maskZ;
            cz = (s16)alignedZ;
            if (mode == 5) {
                Vec2 a(2.0f * (f32)fn_802B6E7C(cx, 0, nw), 2.0f * (f32)fn_802B6ED8(cz, 0, nh));
                Vec2 c(2.0f * (f32)fn_802B6E7C(cx, nw - 1, nw), 2.0f * (f32)fn_802B6ED8(cz, nh - 1, nh));
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
                    BrushCellValues cell;
                    u32 quotient = i / k;
                    cell.ri = i - quotient * k;
                    cell.bj = j / k;
                    cell.rj = j - cell.bj * k;
                    cell.bi = i / k;
                    int v;
                    if (!(cell.ri == 0 && cell.rj == 0)) {
                        u32 i1 = cell.bi + 1 < b->w - 1 ? cell.bi + 1 : b->w - 1;
                        u32 j1 = cell.bj + 1 < b->h - 1 ? cell.bj + 1 : b->h - 1;
                        u8 d;
                        f32 tx = (f32)cell.ri / (f32)k;
                        cell.a = b->data[cell.bj * b->w + cell.bi];
                        u8 c = b->data[cell.bj * b->w + i1];
                        d = b->data[j1 * b->w + cell.bi];
                        f32 top = (f32)cell.a + tx * ((f32)c - (f32)cell.a);
                        u8 e = b->data[j1 * b->w + i1];
                        f32 bot = (f32)d + tx * ((f32)e - (f32)d);
                        f32 ty = (f32)cell.rj / (f32)k;
                        v = (int)(top + ty * (bot - top));
                    } else {
                        v = b->data[cell.bj * b->w + cell.bi];
                    }
                    cb(gx, gz, (u8)v);
                }
            }
            if (lbl_8055FA38 == 5) {
                fn_8020404C();
            }
        }
    }
}

// Centre of cell c in world units.
static inline f32 CellCentre(int c, f32 scale) { return lbl_80564C88 + (f32)c * scale; }

void fn_802B7300(int x, int z, int v, u32 color, f32 h) {
    V3 p;
    p.x = CellCentre(x, 2.0f);
    p.y = 0.0f;
    p.z = CellCentre(z, 2.0f);
    p.y = 0.5f + fn_801DCF48(0, p.x, p.z);
    V3 q = p;
    p.y += h;
    f32 w = lbl_80564C88 * (f32)(s16)fn_802B6E30();
    if (v == 0) {
        color = 0xFF000000;
        v = 0x20;
    }
    DrawLineSegment(&p, &q, (color & 0xFFFFFF) | (v << 24), w);
}

const f32 lbl_80564C88 = 2.0f;

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
            if (UpdateLayerValueWithLerp(x, z, lbl_8055FA28, 0xFF, (f32)(u32)v / 255.0f)) {
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
    case 5: {
        f32 fx = GridToWorld(x, 2.0f);
        f32 fz = GridToWorld(z, 2.0f);
        fn_80204044(v, fx, fz);
        break;
    }
    }
}

int fn_802B7694(const V3* hit) {
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
    if (lbl_804BFF80.hover[0] == 0 && lbl_8055FA2C >= 0 && lbl_8055FA2C < lbl_804BE7D8.count) {
        V3 hit;
        if (fn_80204054(&hit, 0, 0, fn_802C86DC(), lbl_804BFF80.s.x, lbl_804BFF80.s.y)) {
            if ((lbl_804BFF80.s.flags & 1) && (lbl_804BFF80.s.op == 0 || was)) {
                lbl_8055FA58 = 1;
                if (fn_802B7694(&hit)) {
                    lbl_8055FA50[0] = 0x400;
                    lbl_8055FA50[2] = 0x400;
                    lbl_8055FA50[3] = -0x400;
                    lbl_8055FA50[1] = -0x400;
                    fn_802B6F34(&hit, &lbl_804BE7D8.data[lbl_8055FA2C], fn_802B7518);
                    fn_802B7434();
                }
            } else if (!(lbl_804BFF80.s.flags & 2)) {
                fn_802B6F34(&hit, &lbl_804BE7D8.data[lbl_8055FA2C], fn_802B7420);
            }
        }
    }
}

void UpdateObjectVisibility(const char* label) {
    UpdateTextElementColor(lbl_8055FA48->Find("save_matmap"), label);
    const char* t = fn_802B6270() ? "Objects On" : "Objects Off";
    UpdateTextElementColor(lbl_8055FA48->Find("toggle_objects"), t);
}

// PROVISIONAL: copy-helper ownership and spelling. Retail copies these two
// position words as bits. The portable path preserves the same representation.
static inline void CopyBrushPosition(V2* dst, const V2* src) {
#if defined(__MWERKS__)
    u32* d = (u32*)dst;
    const u32* s = (const u32*)src;
    u32 x = *s++;
    u32 y = *s;
    *d++ = x;
    *d = y;
#else
    memcpy(dst, src, sizeof(*dst));
#endif
}

void UpdateBrushSelector(void) {
    f32 row = (lbl_8055FA48->FindBox("matmap_col"))->h;
    const TextElem* src = lbl_8055FA48->Find("brush_col");
    TextElem t;
    t.name = src->name;
    CopyBrushPosition(&t.pos, &src->pos);
    t.elemJust = src->elemJust;
    t.scale = src->scale;
    t.just = src->just;
    t.color = src->color;
    t.defText = src->defText;
    char nm[0x20];
    UpdateTextElementColor(lbl_8055FA48->Find("brush_dropdown"), "Brushes");
    if (lbl_8055FA10.Waiting()) {
        fn_802B6AC0();
        if (lbl_804BE7D8.count > 0) {
            for (int i = 0; i < lbl_804BE7D8.count; i++) {
                fn_801345D8(nm, 0x20, "brush:%d", i);
                t.name = nm;
                UpdateTextElementColor(&t, "%s", &lbl_804BE7D8.data[i]);
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

void PopulateLayerDropdown(void) {
    f32 row = (lbl_8055FA48->FindBox("matmap_col"))->h;
    UpdateTextElementColor(lbl_8055FA48->Find("layer_dropdown"), "Layers");
    TextElem t = *lbl_8055FA48->Find("layer_col");
    char nm[0x20];
    if (lbl_8055FA08.Waiting()) {
        void* layers = fn_802B5B6C()->layers;
        if (GetLayerCount(layers) > 0) {
            for (int i = 0; i < GetLayerCount(layers); i++) {
                fn_801345D8(nm, 0x20, "layer:%d", i);
                t.name = nm;
                UpdateTextElementColor(&t, "%s", fn_802B1C1C(layers, i));
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
    UpdateObjectVisibility("Save Road-Opacity Map");
    UpdateBrushSelector();
}

void SaveMaterialMaps(void) {
    UpdateObjectVisibility("Save Material Maps");
    UpdateBrushSelector();
    PopulateLayerDropdown();
}

#pragma push
#pragma opt_loop_invariants off
void UpdateObjectStatistics(void) {
    if (lbl_8055FA40 == 6) {
        UpdateTextElementColor(lbl_8055FA48->Find("objects"), "All Objects");
        return;
    }
    World* w = fn_802B5B6C();
    InlineVec<ObjItem, 16> list;
    u8* clump = 0;
    ExtractTweakersFromLayer(&list, &clump, w->objs, lbl_8055FA40);
    const char* cl = "Dispersed";
    if (*clump) {
        cl = "Clumpy";
    }
    UpdateTextElementColor(lbl_8055FA48->Find("object_clump"), "%s", cl);
    TextElem num = *lbl_8055FA48->Find("objcolnumber");
    TextElem name = *lbl_8055FA48->Find("objcolname");
    f32 row = (lbl_8055FA48->FindBox("stats_col"))->h;
    char nm[0x20];
    for (int i = 0; i < list.count; i++) {
        ObjItem* it = &list.data[i];
        fn_801345D8(nm, 0x20, "object:%d", i);
        TextElem t = num;
        t.name = nm;
        t.pos.y += row * (f32)i;
        UpdateTextElementColor(&t, "%d", *it->density);
        TextElem t2 = name;
        t2.name = nm;
        t2.pos.y += row * (f32)i;
        UpdateTextElementColor(&t2, "%s", it->name);
    }
    ObjItem it;
    if (fn_802B5B74(&it)) {
        fn_802B67C8("object_box");
        fn_801476C4(lbl_8055FA48->Find("scale_title"));
        fn_801473F8(lbl_8055FA48, "obj_name", "%s", it.name);
        UpdateTextElementColor(lbl_8055FA48->Find("obj_scale_min"), "%.2f", it.scale->min);
        UpdateTextElementColor(lbl_8055FA48->Find("obj_scale_max"), "%.2f", it.scale->max);
    }
    UpdateTextElementColor(lbl_8055FA48->Find("objects"), "Object layer: %d", lbl_8055FA40 + 1);
}
#pragma pop

int AdjustItemDensity(int d, const char* s) {
    int changed = 0;
    int idx = fn_802B5D94(s);
    World* w = fn_802B5B6C();
    InlineVec<ObjItem, 16> list;
    ExtractTweakersFromLayer(&list, 0, w->objs, lbl_8055FA40);
    int v;
    s32* dp = list.data[idx].density;
    v = *dp + d / 2;
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
    ObjScale* sc;
    int changed;
    f32 v;
    f32 n;
    changed = 0;
    ObjItem it;
    if (fn_802B5B74(&it)) {
        sc = it.scale;
        v = sc->max;
        n = sc->min + fn_802B822C(d);
        v = v < n ? v : n;
        v = v > 0.05f ? v : 0.05f;
        if (v != sc->min) {
            sc->min = v;
            it.scale->dirty = 1;
            changed = 1;
        }
    }
    return changed;
}

int fn_802B8310(int d, const char* s) {
    ObjScale* sc;
    f32 lo;
    int changed = 0;
    f32 v = 10.0f;
    ObjItem it;
    if (fn_802B5B74(&it)) {
        sc = it.scale;
        lo = sc->min;
        f32 n = sc->max + fn_802B822C(d);
        v = v < n ? v : n;
        v = v > lo ? v : lo;
        if (v != sc->max) {
            sc->max = v;
            it.scale->dirty = 1;
            changed = 1;
        }
    }
    return changed;
}

static Cmd2 lbl_8034A95C[3] = {
    {0, fn_802B5DA0, AdjustItemDensity},
    {"obj_scale_min", 0, fn_802B825C},
    {"obj_scale_max", 0, fn_802B8310},
};

void fn_802B83CC(void) {
    if (!lbl_804BFF80.clicked[0]) {
        return;
    }
    int d = lbl_804BFF80.s.x - lbl_804BFF80.prev.x;
    if (!d) {
        return;
    }
    const Cmd2* m = lbl_8034A95C;
    for (u32 i = 0; i < 3; i++, m++) {
        if ((m->name && strcmp(m->name, lbl_804BFF80.clicked) == 0) || (m->pred && m->pred(lbl_804BFF80.clicked))) {
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

#pragma push
#pragma opt_strength_reduction off  /* Pragma unproven: helps but not exact (PRAGMA_AUDIT_2026-11-15) */
// PROVISIONAL: worker/input shape, next-column return, destination byte view, first-use immutable pointer, and local ownership/order are inferred.
struct BrushNextColumn { u32 value; };
static inline BrushNextColumn NextBrushColumn(u32 x) { BrushNextColumn v = { x + 1 }; return v; }

static inline void RotateBrushSized(Brush* b, const Brush& tmp, int op, u32 h, u32 w) {
    u8* destination;
    u32 x;
    u32 y;
    int r;
    int l;
    const u8* pixels = tmp.data;
    for (x = 0; x < b->w; x++) {
        BrushNextColumn next = NextBrushColumn(x);
        u32 rowLast = h - x - 1;
        l = rowLast * w;
        destination = (u8*)b + x;
        r = w * next.value - 1;
        for (y = 0; y < b->h; y++) {
            if (op == 6) {
                destination[offsetof(Brush, data) + y * b->w] = pixels[(int)(r - y)];
            } else {
                const u8* const sourceBytes = tmp.data;
                destination[offsetof(Brush, data) + y * b->w] = sourceBytes[(int)(y + l)];
            }
        }
    }
}

void fn_802B8510(int op) {
    if ((u32)(op - 6) > 1) {
        return;
    }
    if (lbl_8055FA2C >= 0 && lbl_8055FA2C < lbl_804BE7D8.count) {
        Brush* b = &lbl_804BE7D8.data[lbl_8055FA2C];
        Brush tmp = *b;
        u32 w;
        u32 h;
        h = tmp.h;
        b->w = h;
        w = tmp.w;
        b->h = w;
        RotateBrushSized(b, tmp, op, h, w);
    }
}
#pragma pop

void fn_802B8624(CursorState* in) {
    lbl_804BFF80.s.op = 8;
    if (in) {
        lbl_804BFF80.s = *in;
        fn_802B84E4(&lbl_804BFF80.s);
        fn_802B83CC();
        fn_802B8510(lbl_804BFF80.s.op);
    }
}

void HandleCursorInput(void) {
    if (lbl_8055FA48->IsEditing() == 0) {
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

void LoadTedashValues(void) {
    lbl_8055FA48 = fn_80145E70("tedash.val");
    lbl_8055FA40 = 6;
    lbl_804BE7D8.count = 0;
    lbl_8055FA00 = fn_8026E214((void*)fn_802B8728);
    lbl_8055FA34 = -1;
}

void UnloadRaceAssets(void) {
    fn_8026E214((void*)lbl_8055FA00);
    SafeDelete(lbl_8055FA48);
    if (lbl_8055FA34 != -1) {
        mrModelUnload(lbl_8055FA34);
        lbl_8055FA34 = -1;
    }
    lbl_804BFA70.count = 0;
}

void fn_802B87F4(void) {
    if (lbl_8055FA48) {
        HandleCursorInput();
        fn_802B0288(fn_802B59A4(lbl_8055FA3C));
        if (lbl_8055FA38 != 1 && lbl_8055FA40 != 6) {
            fn_802B4E3C(lbl_8055FA40);
        }
    }
}


void UpdateGameModeDisplay(void) {
    u8 m[0x30];
    if (!lbl_8055FA48) {
        return;
    }
    if ((u32)(lbl_8055FA38 - 2) <= 1) {
        fn_802B773C();
    }
    if (lbl_8055FA32 && lbl_8055FA34 != -1) {
        fn_802C7040(lbl_8055FA34);
    }
    ResetTransformMatrix(m);
    fn_802C8638(m);
    SetupCameraProjection(1.0f, 1000.0f, 1.4396f);
    fn_802B86F8();
    fn_802B676C();
    if (lbl_8055FA38 != 2 && lbl_8055FA38 != 3) {
        UpdateStatsDisplay();
    }
    switch (lbl_8055FA38) {
    case 0:
        UpdateObjectStatistics();
        break;
    case 1:
        GenerateWorldMap();
        break;
    case 2:
        SaveMaterialMaps();
        break;
    case 3:
        fn_802B7CF0();
        break;
    }
    const char* mode = GetResourceTypeName(lbl_8055FA38 % 4);
    UpdateTextElementColor(lbl_8055FA48->Find("mode_button"), mode);
    lbl_8055FA48->Edit();
    fn_8001123C(0);
    fn_80011A2C();
}

// Debug-dash key handler (keys arrive as unsigned codes).
int HandleDebugDashInput(u32 key) {
    ILayout* l = lbl_8055FA48;
    if (l) {
        switch (key) {
        case 0x125:
        case 0x127:
            if (lbl_8055FA38 == 0) {
                fn_802B5C24(key == 0x125 ? -1 : 1);
            }
            lbl_804BFF80.s.op = 8;
            lbl_804BFF80.hover[0] = 0;
            lbl_8055FA44 = -1;
            lbl_804BFF80.clicked[0] = 0;
            break;
        case 5:
            l->ToggleEdit();
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
    return 0;
}

void RecordStatValue(const char* name, u32* value) {
    if (fn_802B05E4()) {
        return;
    }
    Stat s;
    SafeStrCopy(s.name, name, 0x20);
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
