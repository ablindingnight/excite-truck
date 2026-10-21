// Art animations (0x802AC0E8-0x802AE2E0): keyframed UI art animations loaded from .val files
// (elements with paths of timed keys), the loaded-animation cache, the per-screen player list
// and a debug viewer. Names are guesses apart from the allocation tags, value keys and messages.
// The loader (fn_802AC4A8) and the element player (fn_802AD4C4) are placeholders for now.
#include "game/common.h"
#include "macros.h"

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

struct ArtJustify {
    const char* name;  // offset 0x0
    u32 flag;          // offset 0x4
    u8 horiz;          // offset 0x8
};

struct ArtElem {
    s32 type;     // offset 0x0
    s32* dst;     // offset 0x4
    u8 pad8[0xC - 0x8];
    s32 handle;   // offset 0xC
    u8 pad10[0x28 - 0x10];
};

struct ArtKey {
    ArtKey(void);
    f32 t;        // offset 0x0
    u8 pad4[0x34 - 0x4];
    s32 value;    // offset 0x34
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
    ArtElem* elems;   // offset 0x20
    s32 nPaths;       // offset 0x24
    ArtPath* paths;   // offset 0x28
    f32 length;       // offset 0x2C
};

struct ArtItem {
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
    ArtItem* items;  // offset 0x0
    s32 count;       // offset 0x4
    s32 max;         // offset 0x8
};

struct ArtList : public ArtListBase {
    ArtList(int n) {
        items = new ("dsa_buf") ArtItem[n];
        max = n;
        count = 0;
        buf = items;
    }
    ~ArtList(void) { delete[] buf; }
    ArtItem* buf;  // offset 0xC
};

class ArtPlayer {
public:
    ArtPlayer(int n);
    ~ArtPlayer(void);
    int Add(const char* name, int x);
    void Start(int i);
    void Pause(int i);
    void Resume(int i);
    void StopAll(void);
    void Rewind(void);
    void Update(void);
    void Draw(int flags);

    ArtList* list;  // offset 0x0
    f32 t;          // offset 0x4
};

extern "C" {
int sscanf(const char* s, const char* fmt, ...);
char* strstr(const char* s, const char* sub);
int strcmp(const char* a, const char* b);
double atof(const char* s);
void fn_802328D4(s32 h);
void fn_8023895C(void);
void fn_802389E4(void);
void fn_80238A80(int x, int y, u32 color, const char* fmt, ...);
void fn_80238B44(int x, int y, int w, int h, u32 color);
void fn_802C6CF8(s32 model);
u32 fn_802E2C20(void);
void fn_802E30C8(const char* fmt, ...);
}


extern f32 lbl_8055E3C8;  // race clock

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
    ArtJustify table[8] = {
        {"left", 1, 1},   {"right", 2, 1},       {"center", 4, 1},  {"top", 8, 0},
        {"middle", 0x10, 0}, {"baseline", 0x20, 0}, {"bottom", 0x40, 0}, {0, 0, 0},
    };
    u32 flags = 0;
    int hasH = 0;
    int hasV = 0;
    for (ArtJustify* e = table; e->name; e++) {
        if (strstr(s, e->name)) {
            flags = (e->horiz ? flags & ~7 : flags & 7) | e->flag;
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

// Placeholder: the .val loader (elements, paths and keys), not decompiled yet.
ArtAnim* fn_802AC4A8(const char* name, int x) {
    for (int i = 0; i < lbl_8055F93C; i++) {
        if (!strcmp(lbl_804BD7C0[i]->name, name)) {
            lbl_804BD7C0[i]->refs++;
            return lbl_804BD7C0[i];
        }
    }
    return 0;
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
        ArtElem* e = &a->elems[i];
        if (e->type == 1) {
            fn_802328D4(e->handle);
        } else if (e->type == 2) {
            fn_802C6CF8(e->handle);
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
    for (int i = 0; i < a->nPaths; i++) {
        ArtPath* p = &a->paths[i];
        for (int k = 0; k < p->nKeys - 1; k++) {
            if (t >= p->keys[k].t && t <= 0.001f + p->keys[k + 1].t) {
                s32* dst = a->elems[p->elem].dst;
                if (dst) {
                    *dst = p->keys[k].value;
                }
                break;
            }
        }
    }
}

// Placeholder: draws each element at time t, not decompiled yet.
void fn_802AD4C4(ArtAnim* a, f32 t, int flags) { fn_802AD414(a, t); }

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
    for (int i = list->count; i > 0;) {
        i--;
        fn_802AD30C(list->items[i].anim);
    }
    list->count = 0;
    delete list;
}

int ArtPlayer::Add(const char* name, int x) {
    ArtAnim* anim = fn_802AC4A8(name, x);
    if (anim) {
        ArtItem* item = &list->items[list->count++];
        char* d = item->name;
        int k = 100;
        for (;;) {
            char c = *name++;
            *d++ = c;
            if (!c) {
                break;
            }
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
    ArtItem* item = &list->items[i];
    if (!item->active) {
        item->active = 1;
        item->paused = 0;
        item->t0 = lbl_8055E3C8;
    }
}

void ArtPlayer::Pause(int i) {
    ArtItem* item = &list->items[i];
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
    ArtItem* item = &list->items[i];
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
        ArtItem* item = &list->items[i];
        if (item->paused) {
            item->t0 += t - item->tp;
            item->tp = t;
        }
        if (item->active) {
            fn_802AD414(item->anim, t - item->t0);
        }
    }
}

void ArtPlayer::Draw(int flags) {
    for (int i = 0; i < list->count; i++) {
        ArtItem* item = &list->items[i];
        if (item->active) {
            fn_802AD4C4(item->anim, t - item->t0, flags);
        }
    }
}
