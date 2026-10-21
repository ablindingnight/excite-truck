// gArt (0x80234D44-0x80236550): reference-counted 2D art sets. An art set is a texture plus a ".tab"
// table of pictures (texture rectangles and sizes), drawn as screen quads; open sets are tracked so
// they can be reported and freed at shutdown. Also a small set of named textures (dynotex). Names are
// guesses apart from the strings.
#include "game/common.h"
#include <math.h>
#include <string.h>

struct StringTable;

extern "C" {
double atof(const char* s);
int strcasecmp(const char* a, const char* b);
char* strrchr(const char* s, int c);
void* __nw__FUlPCc(u32 size, const char* tag);
void* __nwa__FUlPCc(u32 size, const char* tag);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal
int fn_8012C33C(const char* name);       // file exists
StringTable* fn_8012F150(const char* name);
int fn_8012F2E4(StringTable* t);
int fn_8012F2E8(StringTable* t);
const char* fn_8012F2F0(StringTable* t, int row, int col);
s32 fn_802CAC28(const char* name, s32 key);  // load texture
void fn_802CAC54(s32 tex);
void fn_802CC5B8(s32 tex);
extern f32 lbl_80558DC4[];
}

static inline int NearZero(f32 x) {
    if ((f32)__fabs(x) <= lbl_80558DC4[0]) {
        return 1;
    }
    return 0;
}

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

static inline void StrNCat(char* d, const char* s, int n) {
    while (*d) {
        n--;
        d++;
    }
    if (n > 0) {
        StrNCopy(d, s, n);
    }
}

// One picture: its texture rectangle and its size on screen.
struct Picture {
    f32 u0;         // offset 0x0
    f32 v0;         // offset 0x4
    f32 u1;         // offset 0x8
    f32 v1;         // offset 0xC
    f32 height;     // offset 0x10
    f32 width;      // offset 0x14
    Picture* next;  // offset 0x18
};

struct gArtSet {
    char name[0x80];     // offset 0x0
    int refs;            // offset 0x80
    s32 tex;             // offset 0x84
    Picture* pictures;   // offset 0x88
};

struct TrackNode {
    gArtSet* set;     // offset 0x0
    TrackNode* next;  // offset 0x4
};

struct CamMtx {
    u8 pad0[0x38];
    f32 scale;  // offset 0x38
};

struct QuadXY {
    f32 x;
    f32 y;
} __attribute__((aligned(8)));

struct QuadVert {
    QuadXY xy;
    u32 a8;
    u32 aC;
    f32 u;
    f32 v;
} __attribute__((aligned(8)));

struct QuadMtx {
    f32 rot[3][3];
    f32 pos[3];
};

extern "C" {
const CamMtx* fn_8022EDC4(void);
void fn_802C8B98(s32 tex, const void* m);
void fn_802C8C04(s32 tex);
u32 fn_80230B9C(int color);
QuadVert fn_80230A68(u32 color, f32 x, f32 y, f32 z, f32 u, f32 v);
void fn_80231380(QuadVert* v, u32 color, f32 x, f32 y, f32 z, f32 u, f32 w);
void fn_802C8DB8(QuadVert* v, int nv, const u16* idx, int ni);
void fn_802C8D74(void);
f32 fn_802C9A4C(f32 a);
}

static inline void DrawRect(u32 c, f32 x, f32 y, f32 z, f32 w, f32 h, f32 u0, f32 v0, f32 u1, f32 v1) {
    QuadVert v[4] __attribute__((aligned(16)));
    v[0] = fn_80230A68(c, x, y, z, u0, v0);
    v[1] = fn_80230A68(c, x + w, y, z, u1, v0);
    v[2] = fn_80230A68(c, x, y - h, z, u0, v1);
    v[3] = fn_80230A68(c, x + w, y - h, z, u1, v1);
    u16 idx[6] = {0, 1, 3, 0, 3, 2};
    fn_802C8DB8(v, 4, idx, 6);
    fn_802C8D74();
}

ModuleTag lbl_8055F078;
UnkStatic7F lbl_8055F080;
TrackNode* lbl_8055F084;  // the open art sets

extern "C" {

void fn_80235380(gArtSet* s);
void fn_802354A0(gArtSet* s, int idx, f32 x, f32 y, f32 z, f32 w, f32 h, int color);

void fn_80234D44(gArtSet* s) {
    if (s) {
        TrackNode* n = lbl_8055F084;
        if (n == NULL) {
            lbl_8055F084 = (TrackNode*)__nw__FUlPCc(8, "gArt tracker");
            lbl_8055F084->set = s;
            lbl_8055F084->next = NULL;
        } else {
            while (n->next != NULL) {
                n = n->next;
            }
            TrackNode* t = (TrackNode*)__nw__FUlPCc(8, "gArt tracker");
            n->next = t;
            t->set = s;
            t->next = NULL;
        }
    }
}

void fn_80234DE4(gArtSet* s) {
    TrackNode* prev = NULL;
    TrackNode* n = lbl_8055F084;
    while (n->set != s) {
        if (n == NULL) {
            fn_802E317C("Asked to forget art %s that I don't remeber.", s->name);
        }
        prev = n;
        n = n->next;
    }
    if (prev == NULL) {
        lbl_8055F084 = n->next;
    } else {
        prev->next = n->next;
    }
    n->next = NULL;
    n->set = NULL;
    operator delete(n);
}

gArtSet* fn_80234E88(const char* name) {
    TrackNode* n = lbl_8055F084;
    while (n != NULL && strcasecmp(name, n->set->name) != 0) {
        n = n->next;
    }
    if (n == NULL) {
        return NULL;
    }
    return n->set;
}

void fn_80234EF4(gArtSet* s, const char* file) {
    if (fn_8012C33C(file)) {
        StringTable* t = fn_8012F150(file);
        int rows = fn_8012F2E8(t);
        Picture* prev = NULL;
        for (int i = 0; i < rows; i++) {
            Picture* p = (Picture*)__nw__FUlPCc(sizeof(Picture), "gArt");
            p->u0 = atof(fn_8012F2F0(t, i, 0));
            p->v0 = atof(fn_8012F2F0(t, i, 1));
            p->u1 = atof(fn_8012F2F0(t, i, 2));
            p->v1 = atof(fn_8012F2F0(t, i, 3));
            p->width = atof(fn_8012F2F0(t, i, 4));
            p->height = atof(fn_8012F2F0(t, i, 5));
            if (prev == NULL) {
                s->pictures = p;
            } else {
                prev->next = p;
            }
            prev = p;
        }
        fn_8012F2E4(t);
        prev->next = NULL;
    } else {
        fn_802E317C("gArt: Failed to get (%s) that holds art info. Texture may be absent too.", file);
    }
}

Picture* fn_8023505C(gArtSet* s, int idx) {
    if (idx == 0) {
        return s->pictures;
    }
    Picture* p = s->pictures;
    int i = 0;
    while (p != NULL && i <= idx) {
        if (i == idx) {
            return p;
        }
        p = p->next;
        i++;
    }
    return p;
}

void fn_8023509C(void) {
    if (lbl_8055F084) {
        fn_802E317C("gArt started twice. (tracker != NULL)");
    }
}

void fn_802350BC(void) {
    if (lbl_8055F084) {
        fn_802E30C8("gArt: Doing some cleanup that you should've done yourself:");
    }
    while (lbl_8055F084) {
        fn_802E30C8("gArt:\t removing %s (call gArtForget next time)", lbl_8055F084->set->name);
        fn_80235380(lbl_8055F084->set);
    }
}

gArtSet* fn_8023512C(const char* name) {
    char path[128];
    StrNCopy(path, name, 128);
    if (!fn_8012C33C(name)) {
        fn_802E30C8("gArtGet(%s): resource not found", name);
        StrNCopy(path, "ugly_x.art", 128);
    }
    gArtSet* s = fn_80234E88(path);
    if (s) {
        s->refs++;
        return s;
    }
    s = (gArtSet*)__nw__FUlPCc(sizeof(gArtSet), "gArtSet");
    s->tex = fn_802CAC28(path, 0);
    if (s->tex == -1) {
        fn_802E317C("gArt: Null texture from %s", path);
    }
    s->refs = 1;
    s->pictures = NULL;
    StrNCopy(s->name, path, 128);
    char info[128];
    StrNCopy(info, path, 128);
    char* dot = strrchr(info, '.');
    if (dot) {
        *dot = 0;
    } else {
        fn_802E317C("gArt: Couldn't find the extension part of %s", info);
    }
    StrNCat(info, ".tab", 128);
    fn_80234EF4(s, info);
    fn_80234D44(s);
    return s;
}

void fn_80235380(gArtSet* s) {
    if (--s->refs <= 0) {
        Picture* p = s->pictures;
        while (p != NULL) {
            Picture* next = p->next;
            operator delete(p);
            p = next;
        }
        fn_802CAC54(s->tex);
        fn_80234DE4(s);
        operator delete(s);
    }
}

void fn_802353F4(gArtSet* s, int idx, f32 x, f32 y, f32 z) {
    if (idx >= 0) {
        Picture* p = fn_8023505C(s, idx);
        if (p == NULL) {
            fn_802E30C8("gArt: Picture #%d not found in gArtSet %s", idx, s->name);
        } else {
            fn_802354A0(s, idx, x, y, z, p->width, p->height, -1);
        }
    }
}

void fn_802354A0(gArtSet* s, int idx, f32 x, f32 y, f32 z, f32 w, f32 h, int color) {
    if (idx >= 0) {
        Picture* p = fn_8023505C(s, idx);
        if (p == NULL) {
            fn_802E30C8("gArt: Picture #%d not found in gArtSet %s", idx, s->name);
        } else {
            f32 sw, sh, u0, v0, u1, v1;
            sh = h;
            v1 = p->v1;
            sw = w;
            u1 = p->u1;
            v0 = p->v0;
            u0 = p->u0;
            s32 tex = s->tex;
            const CamMtx* cam = fn_8022EDC4();
            fn_802C8B98(tex, cam);
            u32 c = fn_80230B9C(color);
            if (!NearZero(cam->scale - 1.0f)) {
                sw = w * cam->scale;
                sh = h * cam->scale;
                x += 0.5f * (w * (1.0f - cam->scale));
                y -= 0.5f * (h * (1.0f - cam->scale));
            }
            DrawRect(c, x, y, z, sw, sh, u0, v0, u1, v1);
        }
    }
}

static inline f32 Lerp(f32 t, f32 a, f32 b) { return a + t * (b - a); }

static inline f32 Mul(f32 a, f32 b) { return a * b; }

static inline f32 Clamp01(f32 t) {
    t = (1.0f < t) ? 1.0f : t;
    return (t > 0.0f) ? t : 0.0f;
}

void fn_80235758(gArtSet* s, int idx, f32 x, f32 y, f32 w, f32 h, f32 left, f32 top, f32 right, f32 bottom,
                 int color) {
    if (idx >= 0) {
        Picture* p = fn_8023505C(s, idx);
        if (p == NULL) {
            fn_802E30C8("gArt: Picture #%d not found in gArtSet %s", idx, s->name);
        } else {
            f32 sw, sh, vb, ur, vt, ul;
            vb = Lerp(Clamp01(bottom), p->v0, p->v1);
            ur = Lerp(Clamp01(right), p->u0, p->u1);
            vt = Lerp(Clamp01(top), p->v0, p->v1);
            ul = Lerp(Clamp01(left), p->u0, p->u1);
            s32 tex = s->tex;
            sh = h;
            sw = w;
            const CamMtx* cam = fn_8022EDC4();
            fn_802C8B98(tex, cam);
            u32 c = fn_80230B9C(color);
            if (!NearZero(cam->scale - 1.0f)) {
                sw = w * cam->scale;
                sh = h * cam->scale;
                x += 0.5f * (w * (1.0f - cam->scale));
                y -= 0.5f * (h * (1.0f - cam->scale));
            }
            DrawRect(c, x, y, 0.0f, sw, sh, ul, vt, ur, vb);
        }
    }
}

void fn_80235AFC(gArtSet* s, int idx, f32 x, f32 y, f32 z, f32 w, f32 h, f32 angle, u32 color) {
    QuadMtx m __attribute__((aligned(16)));
    QuadMtx* mp = &m;
    mp->pos[0] = x;
    mp->pos[1] = y;
    mp->pos[2] = z;
    f32 c = cos(angle);
    f32 sn = sin(angle);
    f32 zero = 0.0f;
    mp->rot[0][0] = c;
    mp->rot[0][1] = sn;
    mp->rot[0][2] = zero;
    mp->rot[1][0] = -sn;
    mp->rot[1][1] = c;
    mp->rot[1][2] = zero;
    mp->rot[2][0] = zero;
    mp->rot[2][1] = zero;
    mp->rot[2][2] = 1.0f;
    Picture* p = fn_8023505C(s, idx);
    f32 hw = Mul(w, 0.5f);
    f32 hh = Mul(h, 0.5f);
    QuadVert v[4] __attribute__((aligned(16)));
    fn_80231380(&v[0], color, -hw, hh, zero, p->u0, p->v0);
    fn_80231380(&v[1], color, hw, hh, zero, p->u1, p->v0);
    fn_80231380(&v[2], color, -hw, -hh, zero, p->u0, p->v1);
    fn_80231380(&v[3], color, hw, -hh, zero, p->u1, p->v1);
    s32 tex = s->tex;
    u16 idx6[6] = {0, 1, 3, 0, 3, 2};
    fn_802C8B98(tex, mp);
    fn_802C8DB8(v, 4, idx6, 6);
    fn_802C8D74();
}

void fn_80235CBC(gArtSet* s, int idx, f32 x, f32 y, f32 z, f32 w, f32 h, int color) {
    if (idx >= 0) {
        Picture* p = fn_8023505C(s, idx);
        if (p == NULL) {
            fn_802E30C8("gArt: Picture #%d not found in gArtSet %s", idx, s->name);
        } else {
            f32 u0 = p->u0;
            f32 v0 = p->v0;
            f32 u1 = p->u1;
            f32 v1 = p->v1;
            fn_802C8C04(s->tex);
            u32 c = fn_80230B9C(color);
            QuadVert v[4] __attribute__((aligned(16)));
            v[0] = fn_80230A68(c, x, y, z, u0, v0);
            v[1] = fn_80230A68(c, x + w, y, z, u1, v0);
            v[2] = fn_80230A68(c, x, y - h, z, u0, v1);
            v[3] = fn_80230A68(c, x + w, y - h, z, u1, v1);
            u16 idx6[6] = {0, 1, 3, 0, 3, 2};
            fn_802C8DB8(v, 4, idx6, 6);
            fn_802C8D74();
        }
    }
}

f32 fn_80235EF0(gArtSet* s, int idx) {
    if (s == NULL) {
        return 0.0f;
    }
    Picture* p = fn_8023505C(s, idx);
    if (p == NULL) {
        fn_802E30C8("gArt: width requested on art that's not part of this set(%s, %d)", s->name, idx);
        return 0.0f;
    }
    return p->width;
}

f32 fn_80235F64(gArtSet* s, int idx) {
    if (s == NULL) {
        return 0.0f;
    }
    Picture* p = fn_8023505C(s, idx);
    if (p == NULL) {
        fn_802E30C8("gArt: height requested on art that's not part of this set(%s, %d)", s->name, idx);
        return 0.0f;
    }
    return p->height;
}

int fn_80235FD8(gArtSet* s) {
    if (s == NULL) {
        return 0;
    }
    int n = 0;
    for (Picture* p = s->pictures; p != NULL; p = p->next) {
        n++;
    }
    return n;
}

// Stripped from ET; only its string remains.
const char* GArtStrippedUnknownId(void) { return "<unknown-id>"; }

}  // extern "C"

// A set of named textures, one of which is drawn at a time.
class DynoTex {
public:
    struct Entry {
        char name[0x24];  // offset 0x0
        s32 tex;          // offset 0x24
        int id;           // offset 0x28
    };

    DynoTex(int max, u8 flip);
    virtual ~DynoTex(void);
    void Add(int id, const char* name);
    void Draw(int id, int x, int y, int w, int h);

    Entry* entries;  // offset 0x4
    int max;         // offset 0x8
    int count;       // offset 0xC
    s32 current;     // offset 0x10
    u8 flip;         // offset 0x14
};

DynoTex::DynoTex(int max, u8 flip) {
    this->max = max;
    entries = (Entry*)__nwa__FUlPCc(max * sizeof(Entry), "dynotex");
    count = 0;
    current = -2;
    this->flip = flip;
}

DynoTex::~DynoTex(void) {
    for (int i = 0; i < count; i++) {
        fn_802CAC54(entries[i].tex);
    }
    operator delete[](entries);
    max = 0;
    count = 0;
    if (current != -2) {
        fn_802CAC54(current);
    }
}

void DynoTex::Add(int id, const char* name) {
    if (fn_8012C33C(name)) {
        s32 tex = fn_802CAC28(name, 0);
        Entry* e = &entries[count++];
        StrNCopy(e->name, name, 33);
        e->id = id;
        e->tex = tex;
    }
}

void DynoTex::Draw(int id, int x, int y, int w, int h) {
    s32 tex = current;
    for (int i = 0; i < count; i++) {
        Entry* e = &entries[i];
        if (e->id == id) {
            fn_802CC5B8(e->tex);
            tex = e->tex;
            break;
        }
    }
    if (tex != -2) {
        f32 fx, fy, fw, fh, ur, ul, old;
        old = fn_802C9A4C(-10.0f);
        ur = flip ? 0.0f : 1.0f;
        ul = flip ? 1.0f : 0.0f;
        fh = h;
        fw = w;
        fy = y;
        fx = x;
        const CamMtx* cam = fn_8022EDC4();
        fn_802C8B98(tex, cam);
        u32 c = fn_80230B9C(-1);
        if (!NearZero(cam->scale - 1.0f)) {
            fx += 0.5f * (fw * (1.0f - cam->scale));
            fy -= 0.5f * (fh * (1.0f - cam->scale));
            fw *= cam->scale;
            fh *= cam->scale;
        }
        DrawRect(c, fx, fy, 0.0f, fw, fh, ul, 0.0f, ur, 1.0f);
        fn_802C9A4C(old);
    }
}
