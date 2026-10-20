// Morph groups (0x801D9120-0x801D9CC4): terrain morph groups from mmorph.val (each moving a set of
// terrain regions over its time window) and the water/lava overlay model lists that blend between
// the groups' states. Names are guesses apart from the file names, keys and messages.
#include "game/common.h"

extern "C" {
int strcmp(const char* a, const char* b);
int fn_801345D8(char* buf, int size, const char* fmt, ...);  // snprintf
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
ValueTable* fn_80131810(const char* name);  // load value file, quiet
void fn_80131818(ValueTable* table);
s32 fn_802C6FC0(const char* name, int a, int b);
void fn_802C6CF8(s32 inst);
void fn_802C7040(s32 inst);
void fn_802C9900(f32 alpha);
void fn_802C9904(void);
void fn_80178DBC(int region, f32 t);
}

void* operator new(size_t size, const char* tag);

template <typename T>
struct ArrayRef {
    ArrayRef(T* d, int n) {
        data = d;
        cap = n;
        count = 0;
    }
    ~ArrayRef(void) {
        data = 0;
        count = 0;
        cap = 0;
    }
    T* Next(void) { return &data[count++]; }
    int Find(T v) const {
        int i;
        for (i = 0; i < count; i++) {
            if (data[i] == v) {
                return i;
            }
        }
        return -1;
    }
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
};

void fn_802E317C(const char* fmt, ...);

// One terrain region moved by a morph group.
struct MorphRegion {
    MorphRegion(ValueNode* node, ValueTable* regions) {
        start = node->GetFloat("start_t");
        stop = node->GetFloat("stop_t");
        const char* name = node->GetValue("terr_region");
        if (!regions->HasChild(name)) {
            fn_802E317C("mgroup: mmorph.txt refers to nonexistent region \"%s\"", name);
        }
        index = regions->FindChild(name)->GetIntValue("index");
        next = NULL;
    }
    f32 start;          // offset 0x0
    f32 stop;           // offset 0x4
    int index;          // offset 0x8
    MorphRegion* next;  // offset 0xC
};

struct OmlList;

struct OmlListRefs : ArrayRef<OmlList*> {
    OmlListRefs(void) : ArrayRef<OmlList*>(storage, 8) {}
    OmlList* storage[8];
};

struct MorphGroup;

struct GroupRefs : ArrayRef<MorphGroup*> {
    GroupRefs(void) : ArrayRef<MorphGroup*>(storage, 2) {}
    MorphGroup* storage[2];
};

struct ModelRefs : ArrayRef<s32> {
    ModelRefs(void) : ArrayRef<s32>(storage, 4) {}
    s32 storage[4];
};

extern "C" void fn_801D94DC(MorphRegion** head, ValueNode* node, const char* key, ValueTable* regions);
extern "C" void fn_801D98F0(MorphRegion** head);

struct MorphGroup {
    MorphGroup(ValueNode* node, ValueTable* regionTable) {
        const char* s = node->GetValue("name");
        char* d = name;
        int n = sizeof(name);
        while ((*d++ = *s++) != '\0') {
            if (--n <= 0) {
                d[-1] = '\0';
                break;
            }
        }
        regions = NULL;
        fn_801D94DC(&regions, node, "morph_region", regionTable);
        next = NULL;
        t = 0.0f;
    }
    ~MorphGroup(void) { fn_801D98F0(&regions); }

    char name[0x20];       // offset 0x0
    f32 t;                 // offset 0x20
    MorphRegion* regions;  // offset 0x24
    OmlListRefs lists;     // offset 0x28
    MorphGroup* next;      // offset 0x54
};

extern "C" MorphGroup* fn_801D9260(int i);

static inline int Min(int a, int b) { return (a < b) ? a : b; }

// Models blended by the states of up to two morph groups.
struct OmlList {
    OmlList(ValueNode* node) {
        char groupKey[0x20];
        char modelKey[0x20];
        int k;
        for (k = 0; k < groups.cap; k++) {
            fn_801345D8(groupKey, sizeof(groupKey), "gidx%d", k);
            int g = node->GetInt(groupKey, -1);
            if (g < 0) {
                break;
            }
            MorphGroup* gp = fn_801D9260(g);
            *groups.Next() = gp;
        }
        int n = Min(1 << groups.count, models.cap);
        for (k = 0; k < n; k++) {
            fn_801345D8(modelKey, sizeof(modelKey), "model%d", k);
            s32 m = fn_802C6FC0(node->GetValue(modelKey), 0, 0);
            *models.Next() = m;
        }
        dirty = 0;
    }
    ~OmlList(void) {
        for (int i = 0; i < models.count; i++) {
            fn_802C6CF8(models.data[i]);
        }
    }
    GroupRefs groups;  // offset 0x0
    ModelRefs models;  // offset 0x14
    u8 dirty;          // offset 0x30
    OmlList* next;     // offset 0x34
};

ModuleTag lbl_8055E958;
s32 lbl_8055E960;            // wlmap model
OmlList* lbl_8055E964;       // overlay model lists
u8 lbl_8055E968;             // groups locked
MorphGroup* lbl_8055E96C;    // morph groups

extern "C" {

// Splits scale over the 2^n combinations of n group weights.
void fn_801D9120(f32* out, const f32* w, int n, f32 scale) {
    // From debug functions the linker stripped.
    static const char* unused[] = {"mgroups are maxed and locked.", "mgroups are zeroed and unlocked."};
    f32 last = w[n - 1];
    if (n == 1) {
        f32 rest = 1.0f - last;
        out[1] = scale * last;
        out[0] = scale * rest;
    } else {
        fn_801D9120(out, w, n - 1, scale * (1.0f - last));
        fn_801D9120(out + (1 << (n - 1)), w, n - 1, scale * last);
    }
}

void fn_801D91D8(void) {
    for (MorphGroup* g = lbl_8055E96C; g; g = g->next) {
        for (OmlList* l = lbl_8055E964; l; l = l->next) {
            if (l->groups.Find(g) != -1) {
                *g->lists.Next() = l;
            }
        }
    }
}

MorphGroup* fn_801D9260(int i) {
    MorphGroup* g;
    for (g = lbl_8055E96C; g && --i >= 0; g = g->next) {
    }
    return g;
}

void fn_801D9394(MorphGroup** head, ValueNode* node, const char* key, ValueTable* regions);
void fn_801D9654(OmlList** head, ValueNode* node, const char* key, int unused);

void fn_801D9284(void) {
    lbl_8055E96C = NULL;
    ValueTable* regions = fn_80131810("mregion.val");
    if (regions) {
        ValueTable* morph = fn_80131810("mmorph.val");
        if (morph) {
            fn_801D9394(&lbl_8055E96C, morph->FindChild("morph_groups"), "morph_group", regions);
            ValueTable* wl = fn_80131810("wlmodels.val");
            if (wl) {
                fn_801D9654(&lbl_8055E964, wl->FindChild("oml_lists"), "oml", 0);
                fn_801D91D8();
                fn_80131818(wl);
            }
            fn_80131818(morph);
        }
        fn_80131818(regions);
    }
    lbl_8055E960 = fn_802C6FC0("wlmap.mod", 0, 0);
    lbl_8055E968 = 0;
}

void fn_801D9394(MorphGroup** head, ValueNode* node, const char* key, ValueTable* regions) {
    ValueNode* child;
    for (int i = 0; (child = node->GetChildAt(i)) != NULL; i++) {
        if (strcmp(child->GetName(), key) == 0) {
            MorphGroup* g = new (key) MorphGroup(child, regions);
            g->next = *head;
            *head = g;
        }
    }
}

void fn_801D94DC(MorphRegion** head, ValueNode* node, const char* key, ValueTable* regions) {
    ValueNode* child;
    for (int i = 0; (child = node->GetChildAt(i)) != NULL; i++) {
        if (strcmp(child->GetName(), key) == 0) {
            MorphRegion* r = new (key) MorphRegion(child, regions);
            r->next = *head;
            *head = r;
        }
    }
}

void fn_801D9654(OmlList** head, ValueNode* node, const char* key, int unused) {
    ValueNode* child;
    for (int i = 0; (child = node->GetChildAt(i)) != NULL; i++) {
        if (strcmp(child->GetName(), key) == 0) {
            OmlList* l = new (key) OmlList(child);
            l->next = *head;
            *head = l;
        }
    }
}

void fn_801D985C(MorphGroup** head);
void fn_801D9948(OmlList** head);

void fn_801D9828(void) {
    fn_802C6CF8(lbl_8055E960);
    fn_801D985C(&lbl_8055E96C);
    fn_801D9948(&lbl_8055E964);
}

void fn_801D985C(MorphGroup** head) {
    MorphGroup* g = *head;
    while (g) {
        MorphGroup* next = g->next;
        delete g;
        g = next;
    }
    *head = NULL;
}

void fn_801D98F0(MorphRegion** head) {
    MorphRegion* r = *head;
    while (r) {
        MorphRegion* next = r->next;
        delete r;
        r = next;
    }
    *head = NULL;
}

void fn_801D9948(OmlList** head) {
    OmlList* l = *head;
    while (l) {
        OmlList* next = l->next;
        delete l;
        l = next;
    }
    *head = NULL;
}

void fn_801D9AB8(MorphGroup* g, f32 t);

void fn_801D9A08(void) {
    for (MorphGroup* g = lbl_8055E96C; g; g = g->next) {
        fn_801D9AB8(g, 0.0f);
    }
    lbl_8055E968 = 0;
}

MorphGroup* fn_801D9A54(const char* name) {
    for (MorphGroup* g = lbl_8055E96C; g; g = g->next) {
        if (strcmp(name, g->name) == 0) {
            return g;
        }
    }
    return NULL;
}

void fn_801D9AB8(MorphGroup* g, f32 t) {
    if (!lbl_8055E968) {
        g->t = t;
        for (MorphRegion* r = g->regions; r; r = r->next) {
            fn_80178DBC(r->index, r->start + t * (r->stop - r->start));
        }
    }
}

int fn_801D9B28(MorphGroup* g, int region) {
    for (MorphRegion* r = g->regions; r; r = r->next) {
        if (r->index == region) {
            return 1;
        }
    }
    return 0;
}

void fn_801D9B58(void) {
    fn_802C7040(lbl_8055E960);
    for (MorphGroup* g = lbl_8055E96C; g; g = g->next) {
        if (g->t > 0.0f) {
            for (int i = 0; i < g->lists.count; i++) {
                g->lists.data[i]->dirty = 1;
            }
        }
    }
    for (OmlList* l = lbl_8055E964; l; l = l->next) {
        if (l->dirty) {
            f32 w[2];
            f32 out[4];
            int i;
            for (i = 0; i < l->groups.count; i++) {
                w[i] = l->groups.data[i]->t;
            }
            fn_801D9120(out, w, l->groups.count, 1.0f);
            fn_802C9900((0.003921569f > out[0]) ? 0.003921569f : out[0]);
            fn_802C7040(l->models.data[0]);
            for (i = 1; i < l->models.count; i++) {
                fn_802C9900(out[i]);
                fn_802C7040(l->models.data[i]);
            }
            l->dirty = 0;
        }
    }
    fn_802C9904();
}

}  // extern "C"

// NonMatching: fn_801D9654 (the inlined OmlList ctor's count register) and fn_801D9B58 (the overlay
// list loop registers) number registers differently.
