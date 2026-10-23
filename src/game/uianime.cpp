// UI animation sets (0x80236728-0x8023875C, uianime.cpp): a stack of animation states for menu screens.
// Each pushed state loads "a_<screen>.val": keyframed "present"/"unpresent" tracks per UI object
// (offset, alpha, scale over time) and named "special" one-shot animations, and applies them to the UI
// objects by anim_index as the screen transitions in or out. Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
int strcasecmp(const char* a, const char* b);
int strncasecmp(const char* a, const char* b, u32 n);
char* strrchr(const char* s, int c);
}

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

struct UiPose {
    UiPose(ValPoint p) {
        x = p.x;
        y = p.y;
        z = 0.0f;
        m[0] = 1.0f;
        m[1] = 0.0f;
        m[2] = 0.0f;
        m[3] = 0.0f;
        m[4] = 1.0f;
        m[5] = 0.0f;
        m[6] = 0.0f;
        m[7] = 0.0f;
        m[8] = 1.0f;
    }
    f32 m[9];       // offset 0x0
    f32 x, y, z;    // offset 0x24
};

extern "C" {
ValueTable* fn_80131810(const char* name);
void fn_80131818(ValueTable* t);
int fn_801345D8(char* buf, int n, const char* fmt, ...);
void fn_80214C54(s32 id, void* obj);
const char* fn_80215264(void);
void* fn_8022EDE4(const UiPose* p, f32 alpha);
void fn_8022EE98(void* obj);
void* fn_8022EE9C(void);
f32 fn_8023659C(void);
void fn_802E30C8(const char* fmt, ...);
void fn_802E317C(const char* fmt, ...);
}

char lbl_80498664[100];  // anim file for the next push

struct UiAnimKey {
    UiAnimKey(void);
    f32 t;        // offset 0x0
    f32 alpha;    // offset 0x4
    f32 scale;    // offset 0x8
    ValPoint p;   // offset 0xC
};

struct UiAnimTrack {
    UiAnimTrack(void) {
        id = -1;
        n = 0;
        keys = 0;
    }
    ~UiAnimTrack(void) {
        if (keys) {
            delete[] keys;
        }
        id = -1;
        n = 0;
        keys = 0;
    }
    s32 id;     // offset 0x0
    UiAnimKey* keys;  // offset 0x4
    s32 n;      // offset 0x8
};

struct UiSpecial {
    char name[32];  // offset 0x0
    UiAnimTrack track;    // offset 0x20
    u8 autoEnd;     // offset 0x2C
    u8 active;      // offset 0x2D
    f32 start;      // offset 0x30
};

class UiAnimBase {
public:
    virtual ~UiAnimBase(void) {}
    virtual f32 Length(void) = 0;
    virtual void Apply(f32 t) = 0;
};

class UiAnimList : public UiAnimBase {
public:
    UiAnimList(ValueTable* t, const char* key);
    virtual ~UiAnimList(void);
    virtual f32 Length(void);
    virtual void Apply(f32 t);

    UiAnimTrack* tracks;  // offset 0x4
    s32 n;          // offset 0x8
    f32 len;        // offset 0xC
};

class UiSpecialList : public UiAnimBase {
public:
    UiSpecialList(ValueTable* t);
    virtual ~UiSpecialList(void);
    virtual f32 Length(void);
    virtual void Apply(f32 t);

    UiSpecial* items;  // offset 0x4
    s32 n;           // offset 0x8
};

struct AnimState {
    u8 flags[33];         // offset 0x0 (0x1F: skip present, 0x20: skip unpresent)
    u8 reverse;           // offset 0x21
    AnimState* next;      // offset 0x24
    UiAnimList* present;    // offset 0x28
    UiAnimList* unpresent;  // offset 0x2C
    UiSpecialList* special; // offset 0x30
};

static inline void StrCopy(char* d, const char* s, int n) {
    char* p = d;
    while ((*p++ = *s++) != 0) {
        if (--n <= 0) {
            p[-1] = 0;
            break;
        }
    }
}

static inline void StrCat(char* d, const char* s, int n) {
    while (*d) {
        n--;
        d++;
    }
    if (n > 0) {
        StrCopy(d, s, n);
    }
}

static inline void LoadTrack(UiAnimTrack* tr, ValueNode* sub) {
    if (sub) {
        int j;
        ValueNode* f;
        for (j = 0; (f = sub->GetChildAt(j)) != 0; j++) {
            if (!strcasecmp(f->GetName(), "frame")) {
                tr->n++;
            }
        }
        if (tr->n > 0) {
            tr->keys = new ("ui_anim_frame") UiAnimKey[tr->n];
            UiAnimKey def;
            def.t = 0.0f;
            def.scale = 1.0f;
            def.alpha = 1.0f;
            def.p.x = 0.0f;
            def.p.y = 0.0f;
            tr->n = 0;
            for (j = 0; (f = sub->GetChildAt(j)) != 0; j++) {
                if (!strcasecmp(f->GetName(), "frame")) {
                    UiAnimKey* prev = j > 0 ? &tr->keys[j - 1] : &def;
                    UiAnimKey* k = &tr->keys[tr->n++];
                    k->t = f->GetFloat("t");
                    k->scale = f->GetFloat("scale", prev->scale);
                    k->alpha = f->GetFloat("alpha", prev->alpha);
                    k->p = f->GetPointOr("offset", prev->p);
                }
            }
        } else {
            fn_802E30C8("ValFileAnim: anim table %d contains no frames", tr->id);
        }
    } else {
        fn_802E30C8("ValFileAnim: no anim table for %d!", tr->id);
    }
}

static inline int HasNode(ValueNode* n, const char* key) { return n->FindChild(key) != 0; }

static inline f32 LastTime(const UiAnimTrack* tr) { return tr->n > 0 ? tr->keys[tr->n - 1].t : 0.0f; }

inline UiAnimList::UiAnimList(ValueTable* t, const char* key) {
    tracks = 0;
    n = 0;
    len = 0.0f;
    if (t) {
        int i;
        ValueNode* c;
        for (i = 0; (c = t->GetChild(i)) != 0; i++) {
            if (!strcasecmp(c->GetName(), "anim")) {
                n++;
            }
        }
        if (n > 0) {
            tracks = new ("ui_anim") UiAnimTrack[n];
            n = 0;
            for (i = 0; (c = t->GetChild(i)) != 0; i++) {
                if (!strcasecmp(c->GetName(), "anim") && HasNode(c, key)) {
                    ValueNode* sub = c->FindChild(key);
                    UiAnimTrack* tr = &tracks[n];
                    tr->id = c->GetIntValue("anim_index");
                    tr->n = 0;
                    LoadTrack(tr, sub);
                    f32 last = LastTime(&tracks[n]);
                    len = len > last ? len : last;
                    n++;
                }
            }
        } else {
            fn_802E30C8("ValFileAnimList: file %s contains no anims", lbl_80498664);
        }
    } else {
        fn_802E30C8("ValFileAnimList: couldn't open %s", lbl_80498664);
    }
}

inline UiSpecialList::UiSpecialList(ValueTable* t) {
    items = 0;
    n = 0;
    if (t) {
        int i;
        ValueNode* c;
        for (i = 0; (c = t->GetChild(i)) != 0; i++) {
            if (!strcasecmp(c->GetName(), "anim")) {
                ValueNode* s;
                for (int j = 0; (s = c->GetChildAt(j)) != 0; j++) {
                    if (!strcasecmp(s->GetName(), "special")) {
                        n++;
                    }
                }
            }
        }
        if (n > 0) {
            items = new ("ui_anim") UiSpecial[n];
            n = 0;
            for (i = 0; (c = t->GetChild(i)) != 0; i++) {
                if (!strcasecmp(c->GetName(), "anim")) {
                    ValueNode* s;
                    for (int j = 0; (s = c->GetChildAt(j)) != 0; j++) {
                        if (!strcasecmp(s->GetName(), "special")) {
                            UiSpecial* sp = &items[n];
                            sp->autoEnd = s->GetBool("auto_end", 0);
                            StrCopy(sp->name, s->GetValue("name"), 30);
                            sp->active = 0;
                            sp->track.id = c->GetIntValue("anim_index");
                            sp->track.n = 0;
                            LoadTrack(&sp->track, s);
                            n++;
                        }
                    }
                }
            }
        } else {
            fn_802E30C8("ValFileAnimList: file %s contains no special anims", lbl_80498664);
        }
    } else {
        fn_802E30C8("ValFileAnimList: couldn't open %s", lbl_80498664);
    }
}

UnkStatic123s lbl_80498650;

ModuleTag lbl_8055F0A8;
UiAnimList* lbl_8055F0B0;     // pending present anims
UiAnimList* lbl_8055F0B4;     // pending unpresent anims
UiSpecialList* lbl_8055F0B8;  // pending special anims
AnimState* lbl_8055F0BC;    // stack bottom
AnimState* lbl_8055F0C0;    // current state
u8 lbl_8055F0C4;            // started
u8 lbl_8055F0C5;
u8 lbl_8055F0C6;            // no parent match
u8 lbl_8055F0C7;


static inline ValPoint Lerp2(const ValPoint& a, const ValPoint& b, f32 f) {
    ValPoint r;
    r.x = a.x + f * (b.x - a.x);
    r.y = a.y + f * (b.y - a.y);
    return r;
}

static inline void* EvalTrack(const UiAnimTrack* tr, f32 time) {
    void* obj;
    if (tr->n > 0) {
        int k = -1;
        for (int j = 0; j < tr->n; j++) {
            if (time >= tr->keys[j].t) {
                k = j;
            }
        }
        if (k == -1) {
            UiAnimKey* a = &tr->keys[0];
            UiPose p(a->p);
            f32 s = a->scale;
            obj = fn_8022EDE4(&p, a->alpha);
            *(f32*)((u8*)obj + 0x38) = s;
        } else if (k == tr->n - 1) {
            UiAnimKey* a = &tr->keys[tr->n - 1];
            UiPose p(a->p);
            f32 s = a->scale;
            obj = fn_8022EDE4(&p, a->alpha);
            *(f32*)((u8*)obj + 0x38) = s;
        } else {
            UiAnimKey* a = &tr->keys[k];
            UiAnimKey* b = &tr->keys[k + 1];
            f32 dt = b->t - a->t;
            dt = dt > 0.0001f ? dt : 0.0001f;
            f32 f = (time - a->t) / dt;
            UiPose p(Lerp2(a->p, b->p, f));
            f32 s = a->scale + f * (b->scale - a->scale);
            obj = fn_8022EDE4(&p, a->alpha + f * (b->alpha - a->alpha));
            *(f32*)((u8*)obj + 0x38) = s;
        }
    } else {
        obj = fn_8022EE9C();
    }
    return obj;
}

UiAnimList::~UiAnimList(void) {
    if (tracks) {
        delete[] tracks;
    }
    tracks = 0;
    n = 0;
    len = 0.0f;
}

f32 UiAnimList::Length(void) { return len; }

void UiAnimList::Apply(f32 t) {
    t = 1.0f < t ? 1.0f : t;
    t = t > 0.0f ? t : 0.0f;
    f32 time = len * t;
    for (int i = 0; i < n; i++) {
        UiAnimTrack* tr = &tracks[i];
        void* obj = EvalTrack(tr, time);
        fn_80214C54(tr->id, obj);
        fn_8022EE98(obj);
    }
}

UiSpecialList::~UiSpecialList(void) {
    delete[] items;
    items = 0;
    n = 0;
}

f32 UiSpecialList::Length(void) { return 0.0f; }

void UiSpecialList::Apply(f32 t) {
    f32 now = fn_8023659C();
    for (int i = 0; i < n; i++) {
        UiSpecial* s = &items[i];
        if (s->active) {
            f32 time = now - s->start;
            void* obj = EvalTrack(&s->track, time);
            fn_80214C54(s->track.id, obj);
            fn_8022EE98(obj);
            s = &items[i];
            if (s->autoEnd && time > LastTime(&s->track)) {
                s->active = 0;
            }
        }
    }
}

extern "C" {

AnimState* fn_80237C40(void);
void fn_80237D00(AnimState* s);
void fn_80236960(const char* name);
void fn_80237AA0(int which, f32* out, f32 t);
void fn_80236A5C(const char* name, const char* parent);
void fn_80237A9C(void);
void fn_80237DA0(const char* name);
void fn_80237E7C(const char* name);
int fn_80237EFC(const char* name);

void fn_80236728(void) {
    if (lbl_8055F0C4) {
        fn_802E317C("uiAnime: system started twice");
    }
    lbl_8055F0BC = 0;
    lbl_8055F0C0 = 0;
    lbl_8055F0C4 = 1;
}

void fn_80236774(void) { lbl_8055F0C4 = 0; }

void fn_80236780(void) {
    if (!lbl_8055F0BC) {
        lbl_8055F0C0 = lbl_8055F0BC = fn_80237C40();
    } else {
        lbl_8055F0C0->next = fn_80237C40();
        lbl_8055F0C0 = lbl_8055F0C0->next;
    }
    lbl_8055F0C0->flags[0x1F] = lbl_8055F0C7;
    lbl_8055F0C0->flags[0x20] = 0;
    lbl_8055F0C0->reverse = lbl_8055F0C5 || !lbl_8055F0C6;
    lbl_8055F0C0->present = lbl_8055F0B0;
    lbl_8055F0C0->unpresent = lbl_8055F0B4;
    lbl_8055F0C0->special = lbl_8055F0B8;
    lbl_8055F0C5 = 0;
    lbl_8055F0C6 = 0;
    lbl_8055F0C7 = 0;
    lbl_8055F0B0 = 0;
    lbl_8055F0B4 = 0;
    lbl_8055F0B8 = 0;
}

void fn_80236848(void) {
    AnimState* cur = lbl_8055F0C0;
    AnimState* p = lbl_8055F0BC;
    if (p == cur) {
        fn_80237D00(cur);
        lbl_8055F0BC = 0;
        lbl_8055F0C0 = 0;
    } else {
        while (p->next != cur) {
            p = p->next;
        }
        fn_80237D00(cur);
        lbl_8055F0C0 = p;
        p->next = 0;
    }
}

void fn_802368B4(int i) {
    if (!lbl_8055F0C0->flags[i]) {
        lbl_8055F0C0->flags[i] = 1;
    }
}

void fn_802368D0(void) { lbl_8055F0C0->reverse = 1; }

void fn_802368E0(void) { lbl_8055F0C5 = 1; }

int fn_802368EC(void) { return lbl_8055F0C0->reverse == 0; }

void fn_80236900(void) { lbl_8055F0C0->reverse = 0; }

void fn_80236910(const char* name) { fn_80236960(name); }

void fn_80236914(int which, f32* out, f32 t) { fn_80237AA0(which, out, t); }

void fn_80236918(const char* name) { fn_80236A5C(name, fn_80215264()); }

void fn_80236950(void) { fn_80237A9C(); }

void fn_80236954(const char* name) { fn_80237DA0(name); }

void fn_80236958(const char* name) { fn_80237E7C(name); }

int fn_8023695C(const char* name) { return fn_80237EFC(name); }

void fn_80236960(const char* name) {
    if (name) {
        StrCopy(lbl_80498664, name, 100);
    }
}

void fn_802369A8(char* dst, int n, const char* name) {
    fn_801345D8(dst, n, "a_%s", name);
    char* dot = strrchr(dst, '.');
    if (dot) {
        *dot = 0;
    }
    StrCat(dst, ".val", n);
}

void fn_80236A5C(const char* name, const char* parent) {
    if (!lbl_80498664[0]) {
        fn_802369A8(lbl_80498664, 100, name);
    }
    ValueTable* t = fn_80131810(lbl_80498664);
    if (t) {
        int any = 1;
        if (!strcasecmp(parent, "")) {
            any = 0;
        } else if (t->HasChild("info")) {
            ValueNode* info = t->FindChild("info");
            const char* key;
            const char* val;
            for (int i = 0; info->GetPair(i, &key, &val); i++) {
                if (!strncasecmp(key, "parent", 6) && (!strcasecmp(val, "__any") || !strcasecmp(val, parent))) {
                    any = 0;
                    break;
                }
            }
        }
        if (any) {
            lbl_8055F0C6 = 1;
        }
        lbl_8055F0B0 = new ("ui_anim_list") UiAnimList(t, "present");
        lbl_8055F0B4 = new ("ui_anim_list") UiAnimList(t, "unpresent");
        lbl_8055F0B8 = new ("ui_anim_list") UiSpecialList(t);
        fn_80131818(t);
    }
    lbl_80498664[0] = 0;
}

void fn_80237A9C(void) {}

void fn_80237AA0(int which, f32* out, f32 t) {
    if (which == 0) {
        out[0] = lbl_8055F0C0->present ? lbl_8055F0C0->present->Length() : 0.0f;
        out[1] = lbl_8055F0C0->unpresent ? lbl_8055F0C0->unpresent->Length() : 0.0f;
        if (!lbl_8055F0C0->reverse) {
            f32 x = out[1];
            out[1] = out[0];
            out[0] = x;
        }
        if (lbl_8055F0C0->flags[0x1F]) {
            out[0] = 0.0f;
        }
        if (lbl_8055F0C0->flags[0x20]) {
            out[1] = 0.0f;
        }
    } else {
        AnimState* s = lbl_8055F0C0;
        if (which == 3) {
            if (s->reverse) {
                t = 1.0f - t;
            }
        } else if (!s->reverse) {
            t = 1.0f - t;
        }
        if ((which == 3) == s->reverse) {
            if (s->unpresent) {
                s->unpresent->Apply(t);
            }
        } else if (s->present) {
            s->present->Apply(t);
        }
        if (lbl_8055F0C0->special) {
            lbl_8055F0C0->special->Apply(t);
        }
    }
}

AnimState* fn_80237C40(void) {
    AnimState* s = new ("AnimeSetStack") AnimState;
    for (int i = 0; i < 33; i++) {
        s->flags[i] = 0;
    }
    s->present = 0;
    s->unpresent = 0;
    s->next = 0;
    return s;
}

void fn_80237D00(AnimState* s) {
    delete s->present;
    delete s->unpresent;
    delete s->special;
    delete s;
}

void fn_80237DA0(const char* name) {
    UiSpecialList* l = lbl_8055F0C0->special;
    for (int i = 0; i < l->n; i++) {
        if (!strcasecmp(l->items[i].name, name)) {
            l->items[i].active = 1;
            l->items[i].start = fn_8023659C();
            for (int j = 0; j < l->n; j++) {
                if (j != i && l->items[j].track.id == l->items[i].track.id) {
                    l->items[j].active = 0;
                }
            }
        }
    }
}

void fn_80237E7C(const char* name) {
    UiSpecialList* l = lbl_8055F0C0->special;
    for (int i = 0; i < l->n; i++) {
        if (!strcasecmp(l->items[i].name, name)) {
            l->items[i].active = 0;
        }
    }
}

int fn_80237EFC(const char* name) {
    UiSpecialList* l = lbl_8055F0C0->special;
    int found = 0;
    int running = 0;
    f32 now = fn_8023659C();
    for (int i = 0; i < l->n; i++) {
        if (!strcasecmp(l->items[i].name, name)) {
            UiSpecial* s = &l->items[i];
            if (s->active) {
                found = 1;
                if (LastTime(&s->track) > now - s->start) {
                    running = 1;
                }
            }
        }
    }
    if (found) {
        return running ? 1 : 2;
    }
    return 0;
}

}  // extern "C"

inline UiAnimKey::UiAnimKey(void) {}
