// Art animations (0x802AC0E8-0x802AE2E0): keyframed UI art animations loaded from .val files
// (elements with paths of timed keys), the loaded-animation cache, the per-screen player list
// and a debug viewer. Names are guesses apart from the allocation tags, value keys and messages.
// The element player (fn_802AD4C4) is still a placeholder; the loader defines the element/key layouts.
#include "game/common.h"
#include "game/plainvec.h"
#include <revolution/MTX/vec.h>
#include "macros.h"
#include "game/strcopy.h"
#include "game/mathutil.h"

void* operator new(size_t size, const char* name);
void* operator new[](size_t size, const char* name);

class ArtNode {
public:
    virtual const char* GetName(void);
    virtual int Has(const char* key);
    virtual int GetPair(int index, const char** key, const char** value);
    virtual const char* GetValue(const char* key);
    virtual const char* GetString(const char* key, const char* def);
    virtual s32 GetIntValue(const char* key);
    virtual s32 GetInt(const char* key, s32 def);
    virtual f32 GetFloat(const char* key);
};

// A name -> value mapping table entry, terminated by id 0.
struct ArtMap {
    s32 id;            // offset 0x0
    const char* name;  // offset 0x4
    s32 value;         // offset 0x8
};

// The shared end entry of a name -> value table (id 0); the game modes copy it as their tables' last row.
extern const ArtMap lbl_802FAA40 = {0, 0, 0};

struct ArtJustify {
    const char* name;  // offset 0x0
    u32 flag;          // offset 0x4
    u8 horiz;          // offset 0x8
};

// A parent/attach transform: uniform scale and an offset.
struct ArtXform {
    f32 scale;   // offset 0x0
    f32 pos[3];  // offset 0x4
};

// One animated element ("animelem" allocations): a model (type 2) or a line of text (type 1).
struct AnimElem {
    s32 type;     // offset 0x0
    s32* dst;     // offset 0x4, receives each key's signal
    ArtXform* xform;  // offset 0x8, attach transform
    s32 handle;   // offset 0xC, model or font
    union {
        struct {
            f32 scale;   // offset 0x10
            f32 off[3];  // offset 0x14
            s32 group;   // offset 0x20
            s32* mapped;  // offset 0x24, group override
        } model;
        struct {
            u32 justify;       // offset 0x10
            u32 color;         // offset 0x14
            f32 scale;         // offset 0x18
            const char* text;  // offset 0x1C
        } text;
    };
};

// One timed key of a path.
struct ArtKey {
    ArtKey(void);
    f32 t;         // offset 0x0
    f32 scale[3];  // offset 0x4
    f32 alpha;     // offset 0x10
    f32 pos[3];    // offset 0x14
    f32 q[4];      // offset 0x20, rotation quaternion
    s32 curve;     // offset 0x30: linear, hermite, start, stop
    s32 signal;    // offset 0x34
};

struct ArtPath {
    s32 elem;     // offset 0x0
    s32 nKeys;    // offset 0x4
    ArtKey* keys; // offset 0x8
};

struct ArtAnim {
    char name[0x18];  // offset 0x0
    s32 refs;         // offset 0x18
    s32 nElems;       // offset 0x1C
    AnimElem* elems;   // offset 0x20
    s32 nPaths;       // offset 0x24
    ArtPath* paths;   // offset 0x28
    f32 length;       // offset 0x2C
};

struct ArtAnimEntry {
    char name[100];  // offset 0x0
    ArtAnim* anim;   // offset 0x64
    u8 active;       // offset 0x68
    u8 paused;       // offset 0x69
    f32 t0;          // offset 0x6C
    f32 tp;          // offset 0x70
};

struct ArtListBase {
    ~ArtListBase(void) {
        items = 0;
        count = 0;
        max = 0;
    }
    ArtAnimEntry* items;  // offset 0x0
    s32 count;       // offset 0x4
    s32 max;         // offset 0x8
};

struct ArtList : public ArtListBase {
    ArtList(int n) {
        items = new ("dsa_buf") ArtAnimEntry[n];
        max = n;
        count = 0;
        buf = items;
    }
    ~ArtList(void) { delete[] buf; }
    ArtAnimEntry* buf;  // offset 0xC
};

#include "game/artplayer.h"
#include "game/ease.h"
#include "game/round.h"

extern f32 __float_epsilon[];  // FLT_EPSILON
extern "C" ValueTable* fn_80131808(const char* name);
extern "C" void fn_80131818(ValueTable* t);

extern "C" {
void* memset(void* p, int c, size_t n);
double sqrt(double x);
double sin(double x);
double cos(double x);
s32 fn_802C2C00(s32 mode);
void fn_8012F57C(f32* out, const f32* a, const f32* b, f32 t);  // slerp
void fn_802C9900(f32 alpha);
void fn_802C9904(void);
void fn_802C744C(s32 model, f32 m[4][3]);
void fn_802C70AC(s32 model, f32 m[4][3], s32 a, u32 mask);
void fn_8023389C(s32 font, u32 color, u32 justify, f32 m[4][3], const char* fmt, f32 x, f32 y, f32 z, f32 scale,
                 f32 xs, ...);
s32 fn_802C6FC0(const char* name, int a, int b);
s32 fn_80232578(const char* name);
void fn_802E317C(const char* fmt, ...);
int sscanf(const char* s, const char* fmt, ...);
char* strstr(const char* s, const char* sub);
int strcmp(const char* a, const char* b);
double atof(const char* s);
void fn_802328D4(s32 h);
void fn_8023895C(void);
void fn_802389E4(void);
void fn_80238A80(int x, int y, u32 color, const char* fmt, ...);
void fn_80238B44(int x, int y, int w, int h, u32 color);
void mrModelUnload(s32 model);
u32 fn_802E2C20(void);
void fn_802E30C8(const char* fmt, ...);
}


extern f32 lbl_8055E3C8;  // race clock

// Copies at most n - 1 characters and always terminates.

ModuleTag lbl_8055F930;
UnkStatic7F lbl_8055F938;
s32 lbl_8055F93C;  // cached animation count
s32 lbl_8055F940;  // debug selection
u32 lbl_8055F944;  // debug start time
u32 lbl_8055F948;  // debug pause offset
bool lbl_8055F94C;  // debug clock running
ArtAnim* lbl_804BD7C0[64];

extern "C" {

ArtMap* fn_802AC0E8(ArtMap* tab, const char* name, int id) {
    if (!tab) {
        return 0;
    }
    for (; tab->id; tab++) {
        if (tab->id == id && !strcmp(tab->name, name)) {
            return tab;
        }
    }
    return 0;
}

int fn_802AC174(const char* s) {
    int v = 0;
    if (sscanf(s, "#%x", &v) == 1) {
        return v;
    }
    return -1;
}

u32 fn_802AC1BC(const char* s) {
    u32 flags = 0;
    int hasH = 0;
    int hasV = 0;
    ArtJustify table[8] = {
        {"left", 1, 1},   {"right", 2, 1},       {"center", 4, 1},  {"top", 8, 0},
        {"middle", 0x10, 0}, {"baseline", 0x20, 0}, {"bottom", 0x40, 0}, {0, 0, 0},
    };
    for (ArtJustify* e = table; e->name; e++) {
        if (strstr(s, e->name)) {
            flags = (e->horiz ? flags & ~7 : flags & 7);
            flags |= e->flag;
            hasH = hasH || e->horiz;
            hasV = hasV || !e->horiz;
        }
    }
    if (!hasH) {
        flags |= 4;
    }
    if (!hasV) {
        flags |= 0x10;
    }
    return flags;
}

int fn_802AC2B4(ArtNode* n, const char* key, f32* out) {
    if (n && n->Has(key)) {
        if (out) {
            *out = n->GetFloat(key);
        }
        return 1;
    }
    return 0;
}

f32 fn_802AC344(ArtNode* n, ArtNode* named, const f32* base) {
    f32 b = base ? *base : 0.0f;
    if (n->Has("t")) {
        const char* s = n->GetValue("t");
        f32 t = 0.0f;
        if (s[0] == '+') {
            t = b;
            s++;
        }
        if (named && named->Has(s)) {
            t += named->GetFloat(s);
        } else {
            t += (f32)atof(s);
        }
        return t;
    }
    return b;
}

f32 fn_802AC454(ArtNode* n, ArtNode* named, const f32* base) { return fn_802AC344(n, named, base); }

void fn_802AC458(void) {
    lbl_8055F93C = 0;
    lbl_8055F944 = fn_802E2C20();
    lbl_8055F948 = 0;
    lbl_8055F940 = 0;
    lbl_8055F94C = 1;
}

void fn_802AC49C(void) { lbl_8055F93C = 0; }

// A point built in place by a constructor (as XP in interlude): the default of a GetPointOr call. Name unproven.
struct ArtXY : ValPoint {
    ArtXY(f32 x_, f32 y_) {
        x = x_;
        y = y_;
    }
};

// Loads an art animation (.val): "elem" entries (models and text) and "path" entries of timed "frame" keys, with
// optional "named_times" and a "global_transform"; integer/string placeholders are resolved through map.
// Helper name unproven: arguments read right to left, stores go x, y, z.
static inline void SetV3(void* o_, f32 x, f32 y, f32 z) {
    f32* o = (f32*)o_;
    o[0] = x;
    o[1] = y;
    o[2] = z;
}

ArtAnim* fn_802AC4A8(const char* name, ArtMap* map) {
    ValueNode* elems[64];
    ValueNode* paths[64];
    ValueNode* frames[128];
    ArtAnim* a = new ("artanim") ArtAnim;
    memset(a, 0, sizeof(ArtAnim));
    a->refs = 1;
    SafeStrCopy(a->name, name, 0x18);
    ValueTable* t = fn_80131808(name);
    ValueNode* named = t->HasChild("named_times") ? t->FindChild("named_times") : 0;
    int nPaths = 0;
    int nElems = 0;
    int i;
    ValueNode* n;
    for (i = 0; (n = t->GetChild(i)) != 0; i++) {
        if (!strcmp(n->GetName(), "elem")) {
            elems[nElems++] = n;
        } else if (!strcmp(n->GetName(), "path")) {
            paths[nPaths++] = n;
        }
    }
    a->nElems = nElems;
    a->elems = new ("animelem") AnimElem[nElems];
    a->nPaths = nPaths;
    a->paths = new ("animpath") ArtPath[nPaths];
    for (int i = 0; i < nElems; i++) {
        ValueNode* en = elems[i];
        AnimElem* e = &a->elems[i];
        const char* type = en->GetValue("type");
        memset(e, 0, sizeof(AnimElem));
        if (!strcmp(type, "model")) {
            e->type = 2;
            const char* model = en->GetValue("model");
            ArtMap* m = fn_802AC0E8(map, model, 4);
            if (m) {
                model = (const char*)m->value;
            }
            e->handle = fn_802C6FC0(model, 0, 0);
            e->model.scale = en->GetFloat("scale", 1.0f);
            const ValVec3& off = en->GetVec3Or("offset", ValV3(0.0f, 0.0f, 0.0f));
            e->model.off[0] = off.x;
            e->model.off[1] = off.y;
            e->model.off[2] = off.z;
            // The point objects are temporaries in statement order (retail stack: the default built in place, its
            // by-value copy, the result); each result is then copied into its named local.
            ValPoint xy = CopyValPoint(en->GetPointOr("xy", ValPoint(ArtXY(0.0f, 0.0f))));
            e->model.off[0] = e->model.off[0] + xy.x;
            e->model.off[1] = e->model.off[1] - xy.y;
            e->model.group = en->GetInt("group", -1);
            e->model.mapped = 0;
            const char* gm = en->GetString("group_mapping", 0);
            if (gm) {
                if ((m = fn_802AC0E8(map, gm, 3)) != 0) {
                    e->dst = (s32*)m->value;
                }
                if ((m = fn_802AC0E8(map, gm, 5)) != 0) {
                    e->xform = (ArtXform*)m->value;
                }
                if ((m = fn_802AC0E8(map, gm, 2)) != 0) {
                    e->model.mapped = (s32*)m->value;
                } else {
                    fn_802E30C8("artanim: unmapped integer %s", gm);
                }
            }
        } else if (!strcmp(type, "text")) {
            e->type = 1;
            if (strcmp("overlay.gtx", en->GetValue("font"))) {
                e->handle = fn_80232578(en->GetValue("font"));
                e->text.scale = en->GetFloat("scale", 1.0f);
            } else {
                e->handle = fn_80232578("uifnt_o.gtx");
                e->text.scale = 0.73333335f * en->GetFloat("scale", 1.0f);
            }
            e->text.color = fn_802AC174(en->GetString("color", ""));
            e->text.justify = fn_802AC1BC(en->GetString("justify", "center_middle"));
            e->text.text = en->GetValue("text");
            if (e->text.text[0] == '#') {
                ArtMap* m;
                if ((m = fn_802AC0E8(map, e->text.text + 1, 3)) != 0) {
                    e->dst = (s32*)m->value;
                }
                if ((m = fn_802AC0E8(map, e->text.text + 1, 5)) != 0) {
                    e->xform = (ArtXform*)m->value;
                }
                if ((m = fn_802AC0E8(map, e->text.text + 1, 1)) != 0) {
                    e->text.text = (const char*)m->value;
                } else {
                    fn_802E30C8("artanim: unmapped string %s", e->text.text + 1);
                }
            }
        } else {
            fn_802E317C("artanim: unknown elem type %s", type);
        }
    }
    /* PROVISIONAL: retail counts this loop and the scale/alpha loop below with the local nPaths (kept in r14, CTR loop
       there); reading a->nPaths here frees the register ours otherwise spills. One live value too many elsewhere is the open cause. */
    for (int i = 0; i < a->nPaths; i++) {
        ValueNode* pn = paths[i];
        int k;
        ArtPath* p = &a->paths[i];
        p->elem = pn->GetIntValue("elem_idx");
        const ValPoint pxy = CopyValPoint(pn->GetPointOr("xy", ValPoint(ArtXY(0.0f, 0.0f))));
        f32 t0 = fn_802AC344((ArtNode*)pn, (ArtNode*)named, 0);
        int nFrames = 0;
        ValueNode* c;
        for (int j = 0; (c = pn->GetChildAt(j)) != 0; j++) {
            if (!strcmp(c->GetName(), "frame")) {
                frames[nFrames++] = c;
            }
        }
        p->nKeys = nFrames;
        p->keys = new ("artframe") ArtKey[nFrames];
        k = 0;
        if (nFrames > 0) {
            const char* curveKey = "curve";
            for (; k < nFrames; k++) {
                ValueNode* f;
                ArtKey* key = &p->keys[k];
                ArtKey* prev = k > 0 ? &p->keys[k - 1] : 0;
                f = frames[k];
                key->t = t0 + fn_802AC454((ArtNode*)f, (ArtNode*)named, &prev->t);
                if (f->Has("spin")) {
                    ValVec3 axis;
                    SetV3(&axis, 0.0f, 0.0f, -(0.017453292f * f->GetFloat("spin", 0.0f)));
                    f32 d = PSVECDotProduct((const Vec*)&axis.x, (const Vec*)&axis.x);
                    if (!((f32)__fabs(d) <= __float_epsilon[0])) {
                        f32 len = sqrt(d);
                        f32 s;
                        f32 half = 0.5f * len;
                        s = sin(half);
                        f32 w = cos(half);
                        f32 k2 = s / len;
                        key->q[0] = axis.x * k2;
                        key->q[1] = axis.y * k2;
                        key->q[2] = axis.z * k2;
                        key->q[3] = w;
                    } else {
                        key->q[0] = 0.0f;
                        key->q[1] = 0.0f;
                        key->q[3] = 1.0f;
                        key->q[2] = 0.0f;
                    }
                } else {
                    const char* q = f->GetString("q", "0,0,0,1");
                    if (sscanf(q, "%f,%f,%f,%f", &key->q[0], &key->q[1], &key->q[2], &key->q[3]) != 4) {
                        fn_802E317C("artanim: bad quaternion %s in %s", q, name);
                    }
                }
                if (f->Has("xy")) {
                    ValPoint v = CopyValPoint(f->GetPoint("xy"));
                    v.x = v.x + pxy.x;
                    v.y = pxy.y + v.y;
                    key->pos[0] = v.x - 320.0f;
                    key->pos[1] = 240.0f - v.y;
                    key->pos[2] = 0.0f;
                } else if (f->Has("offset")) {
                    const ValVec3& o = f->GetVec3("offset");
                    key->pos[0] = o.x;
                    key->pos[1] = o.y;
                    key->pos[2] = o.z;
                    key->pos[0] = key->pos[0] + (pxy.x - 320.0f);
                    key->pos[1] = key->pos[1] + (240.0f - pxy.y);
                } else {
                    const f32* src;
                    f32 zero[3];
                    if (prev) {
                        src = prev->pos;
                    } else {
                        zero[0] = 0.0f;
                        zero[1] = 0.0f;
                        zero[2] = 0.0f;
                        src = zero;
                    }
                    key->pos[0] = src[0];
                    key->pos[1] = src[1];
                    key->pos[2] = src[2];
                }
                if (f->Has("scale3")) {
                    const ValVec3& s = f->GetVec3("scale3");
                    key->scale[0] = s.x;
                    key->scale[1] = s.y;
                    key->scale[2] = s.z;
                } else if (f->Has("scale")) {
                    f32 s = f->GetFloat("scale");
                    key->scale[0] = s;
                    key->scale[1] = s;
                    key->scale[2] = s;
                } else if (prev) {
                    key->scale[0] = prev->scale[0];
                    key->scale[1] = prev->scale[1];
                    key->scale[2] = prev->scale[2];
                } else {
                    key->scale[0] = 1.0f;
                    key->scale[1] = 1.0f;
                    key->scale[2] = 1.0f;
                }
                key->alpha = f->GetFloat("alpha", prev ? prev->alpha : 1.0f);
                const char* curveNames = "linear\0hermite\0start\0stop\0";
                s32 curve = prev ? prev->curve : 0;
                key->curve = f->Has(curveKey) ? f->GetEnum(curveKey, curveNames) : curve;
                key->signal = f->GetInt("signal", prev ? prev->signal : 0);
            }
        }
    }
    if (t->HasChild("global_transform")) {
        ValueNode* g = t->FindChild("global_transform");
        f32 off[3];
        if (g->Has("xy")) {
            ValPoint v = CopyValPoint(g->GetPoint("xy"));
            off[0] = v.x;
            off[1] = -v.y;
            off[2] = 0.0f;
        } else if (g->Has("offset")) {
            const ValVec3& o = g->GetVec3("offset");
            off[0] = o.x;
            off[1] = o.y;
            off[2] = o.z;
        } else {
            off[0] = 0.0f;
            off[1] = 0.0f;
            off[2] = 0.0f;
        }
        f32 scale = g->GetFloat("scale", 1.0f);
        f32 alpha = g->GetFloat("alpha", 1.0f);
        for (int i = 0; i < a->nPaths; i++) {
            ArtPath* p = &a->paths[i];
            for (int k = 0; k < p->nKeys; k++) {
                ArtKey* key = &p->keys[k];
                key->pos[0] = key->pos[0] + off[0];
                key->pos[1] = key->pos[1] + off[1];
                key->pos[2] = key->pos[2] + off[2];
                key->scale[0] = key->scale[0] * scale;
                key->scale[1] = key->scale[1] * scale;
                key->scale[2] = key->scale[2] * scale;
                key->alpha = key->alpha * alpha;
            }
        }
    }
    a->length = 0.0f;
    for (int i = 0; i < a->nPaths; i++) {
        ArtPath* p = &a->paths[i];
        f32 last = p->keys[p->nKeys - 1].t;
        f32 lengthv = a->length;
        a->length = lengthv > last ? lengthv : last;
    }
    fn_802AC2B4((ArtNode*)named, "__anim_end__", &a->length);
    fn_80131818(t);
    lbl_804BD7C0[lbl_8055F93C++] = a;
    return a;
}

}  // extern "C"

ArtKey::ArtKey(void) {}

extern "C" {

void fn_802AD30C(ArtAnim* a) {
    if (--a->refs > 0) {
        return;
    }
    lbl_8055F93C--;
    int i;
    for (i = 0; i < a->nElems; i++) {
        AnimElem* e = &a->elems[i];
        if (e->type == 1) {
            fn_802328D4(e->handle);
        } else if (e->type == 2) {
            mrModelUnload(e->handle);
        }
    }
    for (i = 0; i < a->nPaths; i++) {
        delete[] a->paths[i].keys;
    }
    if (a->elems) {
        delete[] a->elems;
    }
    if (a->paths) {
        delete[] a->paths;
    }
    delete a;
}

void fn_802AD414(ArtAnim* a, f32 t) {
    int i;
    s32* dst;
    ArtPath* p;
    int k;
    for (i = 0; i < a->nPaths; i++) {
        p = &a->paths[i];
        for (k = 0; k < p->nKeys - 1; k++) {
            ArtKey* key = &p->keys[k];
            ArtKey* next = &p->keys[k + 1];
            if (t >= key->t && t <= 0.001f + next->t) {
                dst = a->elems[p->elem].dst;
                if (dst) {
                    *dst = key->signal;
                }
                break;
            }
        }
    }
}



// out = in * m (row vector; m's first three rows), safe when out aliases in.
static inline void MulVec(f32* out, const f32* in, f32 m[4][3]) {
    if (out == in) {
        f32 x = Dot3(in[0], m[0][0], in[1], m[1][0], in[2], m[2][0]);
        f32 y = Dot3(in[0], m[0][1], in[1], m[1][1], in[2], m[2][1]);
        f32 z = Dot3(in[0], m[0][2], in[1], m[1][2], in[2], m[2][2]);
        out[0] = x;
        out[1] = y;
        out[2] = z;
    } else {
        out[0] = Dot3(in[0], m[0][0], in[1], m[1][0], in[2], m[2][0]);
        out[1] = Dot3(in[0], m[0][1], in[1], m[1][1], in[2], m[2][1]);
        out[2] = Dot3(in[0], m[0][2], in[1], m[1][2], in[2], m[2][2]);
    }
}


// Draws every path's element at time t: the two keys around t are blended along the key's curve (position, scale
// and alpha linearly, rotation by slerp) into a matrix, under the optional parent transform.
void fn_802AD4C4(ArtAnim* a, f32 t, ArtXform* parent) {
    f32 m[4][3];
    f32 q[4];
    f32 pos[3];
    f32 scale[3];
    f32 off[3];
    s32 save = fn_802C2C00(0);
    for (int i = 0; i < a->nPaths; i++) {
        ArtPath* p = &a->paths[i];
        for (int k = 0; k < p->nKeys - 1; k++) {
            ArtKey* k0 = &p->keys[k];
            ArtKey* k1 = &p->keys[k + 1];
            if (t >= k0->t && t <= 0.001f + k1->t) {
                f32 u = (t - k0->t) / (k1->t - k0->t);
                u = 1.0f < u ? 1.0f : u;
                u = u > 0.0f ? u : 0.0f;
                u = Lerp(0.0f, 1.0f, u);
                if (k0->curve == 1) {
                    u = Ease(u);
                } else if (k0->curve == 2) {
                    u = u * u;
                } else if (k0->curve == 3) {
                    u = 1.0f - (1.0f - u) * (1.0f - u);
                }
                fn_8012F57C(q, k0->q, k1->q, u);
                pos[2] = k0->pos[2] + u * (k1->pos[2] - k0->pos[2]);
                pos[1] = k0->pos[1] + u * (k1->pos[1] - k0->pos[1]);
                pos[0] = k0->pos[0] + u * (k1->pos[0] - k0->pos[0]);
                f32 ps = parent ? parent->scale : 1.0f;
                if (parent) {
                    pos[0] = pos[0] + parent->pos[0];
                    pos[1] = pos[1] + parent->pos[1];
                    pos[2] = pos[2] + parent->pos[2];
                }
                f32 z2;
                f32 y2;
                f32 x2 = q[0] + q[0];
                f32 xx;
                y2 = q[1] + q[1];
                z2 = q[2] + q[2];
                xx = q[0] * x2;
                f32 yy = q[1] * y2;
                f32 zz = q[2] * z2;
                f32 wy;
                f32 xz = q[0] * z2;
                f32 xy = q[0] * y2;
                f32 yz = q[1] * z2;
                f32 wx = q[3] * x2;
                wy = q[3] * y2;
                f32 wz = q[3] * z2;
                m[0][0] = 1.0f - (yy + zz);
                m[0][1] = xy + wz;
                m[0][2] = xz - wy;
                m[1][0] = xy - wz;
                m[1][1] = 1.0f - (xx + zz);
                m[1][2] = yz + wx;
                m[2][1] = yz - wx;
                m[2][0] = xz + wy;
                m[2][2] = 1.0f - (xx + yy);
                m[3][0] = pos[0] * ps;
                m[3][1] = pos[1] * ps;
                f32 alpha = k0->alpha + u * (k1->alpha - k0->alpha);
                AnimElem* e = &a->elems[p->elem];
                m[3][2] = pos[2] * ps;
                scale[2] = k0->scale[2] + u * (k1->scale[2] - k0->scale[2]);
                scale[1] = k0->scale[1] + u * (k1->scale[1] - k0->scale[1]);
                scale[0] = k0->scale[0] + u * (k1->scale[0] - k0->scale[0]);
                f32 es = 1.0f;
                if (e->xform) {
                    ArtXform* xf = e->xform;
                    m[3][0] = m[3][0] + xf->pos[0] * ps;
                    m[3][1] = m[3][1] + xf->pos[1] * ps;
                    m[3][2] = m[3][2] + xf->pos[2] * ps;
                    es *= e->xform->scale;
                }
                if (e->type == 2) {
                    MulVec(off, e->model.off, m);
                    m[3][0] = m[3][0] + off[0];
                    m[3][1] = m[3][1] + off[1];
                    m[3][2] = m[3][2] + off[2];
                    f32 sx = es * (ps * (scale[0] * e->model.scale));
                    m[0][0] = m[0][0] * sx;
                    m[0][1] = m[0][1] * sx;
                    m[0][2] = m[0][2] * sx;
                    f32 sy = es * (ps * (scale[1] * e->model.scale));
                    m[1][0] = m[1][0] * sy;
                    m[1][1] = m[1][1] * sy;
                    m[1][2] = m[1][2] * sy;
                    f32 sz = es * (ps * (scale[2] * e->model.scale));
                    m[2][0] = m[2][0] * sz;
                    m[2][1] = m[2][1] * sz;
                    m[2][2] = m[2][2] * sz;
                    fn_802C9900(alpha);
                    s32 group = e->model.mapped ? *e->model.mapped : e->model.group;
                    if (group < 0) {
                        fn_802C744C(e->handle, m);
                    } else {
                        fn_802C70AC(e->handle, m, -1, 1 << group);
                    }
                    fn_802C9904();
                } else if (e->type == 1) {
                    u32 c = e->text.color;
                    f32 av = alpha * (f32)(c >> 24);
                    int al = Round(av);
                    al = al > 0 ? al : 0;
                    al = al < 255 ? al : 255;
                    fn_8023389C(e->handle, (al << 24) | (c & 0xFF0000) | (c & 0xFF00) | (c & 0xFF),
                                e->text.justify, m, "%s", 0.0f, 0.0f, 0.0f, es * (ps * (scale[0] * e->text.scale)), 1.0f,
                                e->text.text);
                }
                break;
            }
        }
    }
    fn_802C2C00(save);
}

void fn_802ADBB4(void) {
    u32 now = fn_802E2C20();
    if (lbl_8055F94C) {
        lbl_8055F944 = now - lbl_8055F948;
    }
    f32 t = 0.001f * (s32)(now - lbl_8055F944);
    fn_8023895C();
    fn_80238B44(0, 0, 300, 50, 0xC0000000);
    int valid = 0;
    if (lbl_8055F940 >= 0 && lbl_8055F940 < lbl_8055F93C) {
        valid = 1;
    }
    fn_80238A80(10, 10, 0xC0FFFFFF, "ANIM: %d      %s", lbl_8055F940,
                valid ? lbl_804BD7C0[lbl_8055F940]->name : "");
    fn_80238A80(10, 30, 0xC0FFFFFF, "t: %1.1f", t);
    fn_802389E4();
    if (lbl_8055F940 >= 0 && lbl_8055F940 < lbl_8055F93C) {
        fn_802AD4C4(lbl_804BD7C0[lbl_8055F940], t, 0);
    }
}

int fn_802ADCF8(int key) {
    switch (key) {
    case '[': {
        int i = lbl_8055F940 - 1;
        lbl_8055F940 = i < -1 ? -1 : i;
        return 1;
    }
    case ']': {
        int last = lbl_8055F93C - 1;
        lbl_8055F940 = MIN(last, lbl_8055F940 + 1);
        return 1;
    }
    case '"':
        lbl_8055F944 = fn_802E2C20();
        lbl_8055F948 = 0;
        return 1;
    case '\'':
        lbl_8055F94C = !lbl_8055F94C;
        if (lbl_8055F94C) {
            lbl_8055F948 = fn_802E2C20() - lbl_8055F944;
        }
        return 1;
    }
    return 0;
}

}  // extern "C"

ArtPlayer::ArtPlayer(int n) {
    list = new ("anim_list") ArtList(n);
    t = lbl_8055E3C8;
}

ArtPlayer::~ArtPlayer(void) {
    int i = list->count;
    int off = i * sizeof(list->items[0]);  // MATCHING-AID: byte offset walker and the comma test, needed for the loop registers
    while (off -= sizeof(list->items[0]), i-- > 0) {
        fn_802AD30C(((ArtAnimEntry*)((u8*)list->items + off))->anim);
    }
    list->count = 0;
    ArtList* l = list;
    if (l) {
        l->~ArtList();
        operator delete(l);
    }
}

int ArtPlayer::Add(const char* name, int map) {
    ArtAnim* anim = fn_802AC4A8(name, (ArtMap*)map);
    if (anim) {
        int k;
        char* d;
        ArtAnimEntry* item;
        item = &list->items[list->count++];
        d = item->name;
        k = 100;
        while ((*d++ = *name++) != 0) {
            if (--k <= 0) {
                d[-1] = 0;
                break;
            }
        }
        item->anim = anim;
        item->active = 0;
        item->paused = 0;
        return list->count - 1;
    }
    return -1;
}

void ArtPlayer::Start(int i) {
    ArtAnimEntry* item = &list->items[i];
    if (!item->active) {
        item->active = 1;
        item->paused = 0;
        item->t0 = lbl_8055E3C8;
    }
}

void ArtPlayer::Pause(int i) {
    ArtAnimEntry* item = &list->items[i];
    if (!item->paused) {
        if (!item->active) {
            fn_802E30C8("ArtAnimManager: PauseAnim(%d) called, but %d is not running!", i, i);
            item->active = 1;
            item->t0 = t;
        }
        item->paused = 1;
        item->tp = t;
    }
}

void ArtPlayer::Resume(int i) {
    ArtAnimEntry* item = &list->items[i];
    if (item->paused) {
        item->paused = 0;
    }
}

void ArtPlayer::StopAll(void) {
    for (int i = 0; i < list->count; i++) {
        list->items[i].active = 0;
    }
}

void ArtPlayer::Rewind(void) {
    for (int i = 0; i < list->count; i++) {
        if (list->items[i].active) {
            list->items[i].t0 = t - list->items[i].anim->length;
        }
    }
}

void ArtPlayer::Update(void) {
    t = lbl_8055E3C8;
    for (int i = 0; i < list->count; i++) {
        ArtAnimEntry* item = &list->items[i];
        if (item->paused) {
            item->t0 += t - item->tp;
            item->tp = t;
        }
        if (item->active) {
            fn_802AD414(item->anim, t - item->t0);
        }
    }
}

void ArtPlayer::Draw(ArtXform* xf) {
    for (int i = 0; i < list->count; i++) {
        ArtAnimEntry* item = &list->items[i];
        if (item->active) {
            fn_802AD4C4(item->anim, t - item->t0, xf);
        }
    }
}
