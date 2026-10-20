// Flip books (0x802960A0-0x802971B0): animated billboards read from a .val resource - a "default" block and
// a list of "frame" blocks (duration t, uv rectangle, colour as "0x..." plus alpha/red/green/blue bytes,
// texture, size, offset and which parts blend into the next frame). Loaded books are shared by name and
// reference counted. Drawing picks the frame at time t, eases towards the next one and draws a camera-facing
// quad, cross-fading the two textures when "tex" blends. Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
void* memset(void* p, int c, u32 n);
char* strncpy(char* d, const char* s, u32 n);
char* strstr(const char* s, const char* sub);
int strcasecmp(const char* a, const char* b);
int sscanf(const char* s, const char* fmt, ...);
int fn_8012C33C(const char* name);  // resource exists
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* table);
s32 fn_802CAC28(const char* name, int a);  // texture handle
void fn_802CAC54(s32 tex);
void fn_802C8BD0(s32 tex);
void fn_802C8DB8(void* verts, int n, const u16* idx, int ni);
void fn_802C8D74(void);
}

struct CtypeTables {
    u8 pad0[0x10];
    const u8* lower;  // offset 0x10
};

struct Locale {
    u8 pad0[0x38];
    CtypeTables* ctype;  // offset 0x38
};

extern "C" Locale _current_locale;

static inline int ToLower(int c) { return (c < 0 || c >= 256) ? c : (int)_current_locale.ctype->lower[c]; }

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);
void operator delete(void* p);
void operator delete[](void* p);

struct V3 {
    f32 x;
    f32 y;
    f32 z;
};

struct CamMtx {
    V3 right;  // offset 0x0
    V3 up;     // offset 0xC
};

extern "C" const CamMtx* fn_802C86DC(void);

struct FlipVert {
    V3 pos;     // offset 0x0
    u32 color;  // offset 0xC
    f32 u;      // offset 0x10
    f32 v;      // offset 0x14
};

struct FVec : public ValVec3 {
    FVec(void) {}
    FVec(f32 a, f32 b, f32 c) {
        x = a;
        y = b;
        z = c;
    }
};

// One frame (0x30).
struct FlipFrame {
    f32 t;         // offset 0x0
    f32 u1;        // offset 0x4
    f32 u2;        // offset 0x8
    f32 v1;        // offset 0xC
    f32 v2;        // offset 0x10
    u32 color;     // offset 0x14
    s32 tex;       // offset 0x18
    f32 size;      // offset 0x1C
    FVec offset;   // offset 0x20
    u32 blend;     // offset 0x2C
};

struct FlipBook {
    char name[0x20];    // offset 0x0
    FlipBook* next;     // offset 0x20
    FlipFrame* frames;  // offset 0x24
    s32 refs;           // offset 0x28
    s32 count;          // offset 0x2C
};

struct GVec {
    s32 v[3];
};

struct BlendFlag {
    u32 flag;
    const char* name;
};

static inline ValVec3 Vec3Or(ValueNode* n, const char* k, ValVec3 d) { return n->GetVec3Or(k, d); }
static inline ValVec3 Vec3Or2(ValueNode* n, const char* k, const ValVec3& def) {
    ValVec3 d = def;
    return n->GetVec3Or(k, d);
}

static inline f32 Neg1(void) { return -1.0f; }

ModuleTag lbl_8055F780;
FlipBook* lbl_8055F788;

extern "C" {

void fn_802960A0(const V3* p, u32 color, s32 tex, f32 size, f32 u1, f32 u2, f32 v1, f32 v2) {
    const CamMtx* m = fn_802C86DC();
    V3 d;
    f32 uv[4][2];
    FlipVert v[4];
    d.x = m->right.x * -size;
    d.y = m->right.y * -size;
    d.z = m->right.z * -size;
    f32 sx = d.x + m->up.x * size;
    f32 sy = d.y + m->up.y * size;
    f32 sz = d.z + m->up.z * size;
    v[0].pos.x = p->x + sx;
    v[0].pos.y = p->y + sy;
    v[0].pos.z = p->z + sz;
    v[2].pos.x = p->x + sx * -1.0f;
    v[2].pos.y = p->y + sy * -1.0f;
    v[2].pos.z = p->z + sz * -1.0f;
    f32 w = 2.0f * size;
    d.x = sx + m->right.x * w;
    d.y = sy + m->right.y * w;
    d.z = sz + m->right.z * w;
    v[1].pos.x = p->x + d.x;
    v[1].pos.y = p->y + d.y;
    v[1].pos.z = p->z + d.z;
    v[3].pos.x = p->x + d.x * -1.0f;
    v[3].pos.y = p->y + d.y * -1.0f;
    v[3].pos.z = p->z + d.z * -1.0f;
    uv[0][0] = u1;
    uv[0][1] = v1;
    uv[1][0] = u2;
    uv[1][1] = v1;
    uv[2][0] = u2;
    uv[2][1] = v2;
    uv[3][0] = u1;
    uv[3][1] = v2;
    for (int i = 0; i < 4; i++) {
        v[i].color = color;
        v[i].u = uv[i][0];
        v[i].v = uv[i][1];
    }
    fn_802C8BD0(tex);
    u16 idx[6] = {0, 1, 2, 2, 0, 3};
    fn_802C8DB8(v, 4, idx, 6);
    fn_802C8D74();
}

int fn_802962CC(ValueNode* n) { return strcasecmp(n->GetName(), "frame") == 0; }

int fn_80296308(ValueNode* n) { return strcasecmp(n->GetName(), "default") == 0; }

u32 fn_80296344(ValueNode* n, const char* key, u32 def) {
    u32 v;
    const char* s = n->GetString(key, "");
    if (s[0] == '0' && ToLower(s[1]) == 'x') {
        sscanf(s + 2, "%x", &v);
    } else {
        v = def;
    }
    return v;
}

u32 fn_802963F0(ValueNode* n, u32 def) {
    u32 r = def;
    if (n->Has("blend")) {
        BlendFlag flags[4] = {{1, "uv"}, {2, "color"}, {4, "tex"}, {8, "pos"}};
        const char* s = n->GetValue("blend");
        r = 0;
        for (u32 i = 0; i < 4; i++) {
            if (strstr(s, flags[i].name)) {
                r |= flags[i].flag;
            }
        }
    }
    return r;
}

int fn_802964EC(FlipFrame* f, ValueNode* n, const FlipFrame* def) {
    f->t = n->GetFloat("t", def->t);
    f->u1 = n->GetFloat("u1", def->u1);
    f->u2 = n->GetFloat("u2", def->u2);
    f->v1 = n->GetFloat("v1", def->v1);
    f->v2 = n->GetFloat("v2", def->v2);
    f->color = fn_80296344(n, "color", def->color);
    if (n->Has("alpha")) {
        f->color &= 0x00FFFFFF;
        f->color |= n->GetIntValue("alpha") << 24;
    }
    if (n->Has("red")) {
        f->color &= 0xFF00FFFF;
        f->color |= n->GetIntValue("red") << 16;
    }
    if (n->Has("green")) {
        f->color &= 0xFFFF00FF;
        f->color |= n->GetIntValue("green") << 8;
    }
    if (n->Has("blue")) {
        f->color &= 0xFFFFFF00;
        f->color |= n->GetIntValue("blue");
    }
    if (n->Has("tex")) {
        f->tex = fn_802CAC28(n->GetValue("tex"), 0);
    } else {
        f->tex = def->tex;
    }
    if (n->Has("size")) {
        f->size = n->GetFloat("size") / 2.0f / 1.414f;
    } else {
        f->size = def->size;
    }
    ValVec3 d = def->offset;
    (ValVec3&)f->offset = n->GetVec3Or("offset", d);
    f->blend = fn_802963F0(n, def->blend);
    return 1;
}

void fn_80296838(FlipBook* b) {
    if (b) {
        if (b->frames) {
            for (int i = 0; i < b->count; i++) {
                FlipFrame* f = &b->frames[i];
                if (f->tex != -1 && f->tex != -2) {
                    fn_802CAC54(f->tex);
                    f->tex = -2;
                }
            }
            delete[] b->frames;
            b->frames = 0;
        }
        delete b;
    }
}

void fn_802968E8(FlipBook* b) {
    if (b->refs++ == 0) {
        b->next = lbl_8055F788;
        lbl_8055F788 = b;
    }
}

void fn_8029690C(FlipBook* b) {
    if (--b->refs == 0) {
        for (FlipBook** p = &lbl_8055F788; *p; p = &(*p)->next) {
            if (*p == b) {
                *p = b->next;
                break;
            }
        }
        fn_80296838(b);
    }
}

FlipBook* fn_80296950(const char* name) {
    for (FlipBook* b = lbl_8055F788; b; b = b->next) {
        if (strcasecmp(name, b->name) == 0) {
            return b;
        }
    }
    return 0;
}

int fn_802969B4(u8 a, u8 b, f32 t) { return (f32)a + t * ((f32)b - (f32)a); }

u32 fn_80296A10(u32 c0, u32 c1, f32 t) {
    int r = fn_802969B4(c0, c1, t);
    int g = fn_802969B4(c0 >> 8, c1 >> 8, t);
    int b = fn_802969B4(c0 >> 16, c1 >> 16, t);
    int a = fn_802969B4(c0 >> 24, c1 >> 24, t);
    return ((r & 0xFF) | ((g & 0xFF) << 8)) | (((b & 0xFF) << 16) | (a << 24));
}

void fn_80296AAC(FlipFrame* f) {
    memset(f, 0, sizeof(FlipFrame));
    f->v2 = 1.0f;
    f->u2 = 1.0f;
    f->t = 0.1f;
    f->size = 0.7072136f;
    f->tex = -1;
    f->blend = 10;
    f->color = 0xFFFFFFFF;
}

FlipBook* fn_80296B10(const char* res, ValueNode* cfg) {
    int frames;
    int other;
    FlipBook* b = fn_80296950(res);
    if (!b) {
        int ok = 0;
        if (fn_8012C33C(res)) {
            ValueTable* t = fn_80131808(res);
            FlipFrame def;
            frames = 0;
            fn_80296AAC(&def);
            other = 0;
            for (;;) {
                ValueNode* n = t->GetChild(frames + other);
                if (!n) {
                    break;
                }
                if (fn_802962CC(n)) {
                    frames++;
                    continue;
                }
                if (fn_80296308(n)) {
                    fn_802964EC(&def, n, &def);
                }
                other++;
            }
            if (cfg) {
                fn_802964EC(&def, cfg, &def);
            }
            if (frames) {
                ok = 1;
                b = new ("FlipBook") FlipBook;
                b->frames = new ("FlipBookFrame") FlipFrame[frames];
                b->count = frames;
                b->refs = 0;
                if (cfg) {
                    b->name[0] = 0;
                } else {
                    strncpy(b->name, res, 0x20);
                }
                other = 0;
                for (int i = 0; ok; i++) {
                    ValueNode* n = t->GetChild(i);
                    if (!n) {
                        break;
                    }
                    if (fn_802962CC(n)) {
                        if (!fn_802964EC(&b->frames[i] - other, n, &def)) {
                            ok = 0;
                        }
                    } else {
                        other++;
                    }
                }
            }
            fn_80131818(t);
        }
        if (!ok) {
            fn_80296838(b);
            b = 0;
        }
    }
    if (b) {
        fn_802968E8(b);
    }
    return b;
}

void fn_80296D28(FlipBook* b) { fn_8029690C(b); }

static inline f32 Ease(f32 s) {
    f32 k;
    if (s < -1.0f) {
        k = -1.0f;
    } else if (s > 1.0f) {
        k = 1.0f;
    } else if (s > 0.0f) {
        k = s * (-2.0f * s * s) + 3.0f * s * s;
    } else {
        k = s * (-2.0f * s * s) - 3.0f * s * s;
    }
    return k;
}

static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

int fn_80296D2C(FlipBook* b, f32 t, const ValVec3* pos) {
    FlipFrame* fr[2] = {0, 0};
    f32 start = 0.0f;
    for (int i = 0; i < b->count; i++) {
        FlipFrame* f = &b->frames[i];
        if (t < start + f->t) {
            fr[0] = f;
            break;
        }
        start += f->t;
    }
    if (fr[0]) {
    fr[1] = (fr[0] == b->frames + b->count - 1) ? fr[0] : fr[0] + 1;
    FlipFrame* a = fr[0];
    FlipFrame* n = fr[1];
    f32 s = (start - t) / a->t;
    u32 flags = n->blend;
    f32 k = Ease(s);
    f32 u1 = (flags & 1) ? Lerp(a->u1, n->u1, k) : a->u1;
    f32 u2 = (flags & 1) ? Lerp(a->u2, n->u2, k) : a->u2;
    f32 v1 = (flags & 1) ? Lerp(a->v1, n->v1, k) : a->v1;
    f32 v2 = (flags & 1) ? Lerp(a->v2, n->v2, k) : a->v2;
    u32 color = (flags & 2) ? fn_80296A10(a->color, n->color, k) : a->color;
    f32 size = (flags & 8) ? Lerp(fr[0]->size, fr[1]->size, k) : fr[0]->size;
    ValVec3 d;
    V3 p;
    const ValVec3* o;
    if (flags & 8) {
        FVec off(Lerp(fr[0]->offset.x, fr[1]->offset.x, k), Lerp(fr[0]->offset.y, fr[1]->offset.y, k),
                 Lerp(fr[0]->offset.z, fr[1]->offset.z, k));
        o = &off;
    } else {
        o = &fr[0]->offset;
    }
    *(GVec*)&d = *(const GVec*)o;
    p.x = d.x + pos->x;
    p.y = d.y + pos->y;
    p.z = d.z + pos->z;
    if (flags & 4) {
        u32 rgb;
        u8 a0;
        u32 alpha = color >> 24;
        a0 = fn_802969B4(alpha, 0, k);
        rgb = color & 0xFFFFFF;
        fn_802960A0(&p, (a0 << 24) | rgb, fr[0]->tex, size, u1, u2, v1, v2);
        fn_802960A0(&p, ((alpha - a0) << 24) | rgb, fr[1]->tex, size, u1, u2, v1, v2);
    } else {
        fn_802960A0(&p, color, fr[0]->tex, size, u1, u2, v1, v2);
    }
    return 1;
    }
    return 0;
}

f32 fn_802970C8(FlipBook* b) {
    f32 total = 0.0f;
    for (int i = 0; i < b->count; i++) {
        total += b->frames[i].t;
    }
    return total;
}

}  // extern "C"
