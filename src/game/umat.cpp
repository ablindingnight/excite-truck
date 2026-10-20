// Unified material sets (0x802B539C-0x802B5B18): umat.val (with includes) defines per-track material
// sets with road textures and a sky model, plus a global_parameters node. Names are guesses apart
// from the keys and messages.
#include "game/common.h"

extern "C" {
int strcasecmp(const char* a, const char* b);
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
int fn_801345D8(char* buf, u32 size, const char* fmt, ...);  // snprintf
int fn_8012C33C(const char* name);
s32 fn_802CAC28(const char* name, int flags);  // load texture
void fn_802CAC54(s32 tex);
ValueTable* fn_80131810(const char* name);  // load value file, quiet
void fn_80131818(ValueTable* t);
void fn_8012E96C(const char* name);
ValueNode* fn_8013184C(void);  // empty value node
void fn_802B0B20(ValueNode* n, ValueNode* ground);
void fn_802B224C(ValueNode* n);
s32 fn_802B1138(const char* name);
s32 fn_802B2C98(const char* name);
void* fn_802C6B44(const char* name);
s32 fn_802C6DC4(const char* name, void* model, int a, int b);
void fn_802C6CF8(s32 inst);
}

typedef s32 (*TexLoader)(const char* name);
typedef void* (*ModelLoader)(const char* name);
typedef ValueTable* (*TableLoader)(const char* name);

extern "C" {
void fn_802B5404(s32* tex);
s32 fn_802B110C(TexLoader f, const char* name);
void* fn_802B2C34(ModelLoader f, const char* name);
}

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
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
};

struct UMatTex {
    const char* name;  // offset 0x0
    s32 tex;           // offset 0x4
};

struct UMat {
    void Free(void) {
        int k;
        for (k = 0; k < 4; k++) {
            fn_802B5404(&road[k].tex);
        }
        if (sky != -1) {
            fn_802C6CF8(sky);
        }
    }
    const char* name;  // offset 0x0
    s32 x4;            // offset 0x4
    s32 x8;            // offset 0x8
    UMatTex road[4];   // offset 0xC
    s32 sky;           // offset 0x2C
};

struct UMatTableList : ArrayRef<ValueTable*> {
    UMatTableList(void) : ArrayRef<ValueTable*>(storage, 16) {}
    ValueTable* storage[16];
};

struct UMatList : ArrayRef<UMat> {
    UMatList(void) : ArrayRef<UMat>(storage, 16) {}
    UMat storage[16];
};

ModuleTag lbl_8055F9E0;
ValueNode* lbl_8055F9E8;  // global_parameters
UMatTableList lbl_804BE454;
UMatList lbl_804BE4AC;

extern "C" {

s32 fn_802B539C(const char* name) {
    int ok = 0;
    if (name && fn_8012C33C(name)) {
        ok = 1;
    }
    if (ok) {
        return fn_802CAC28(name, 0);
    }
    return -2;
}

void fn_802B5404(s32* tex) {
    if (*tex != -2) {
        fn_802CAC54(*tex);
        *tex = -2;
    }
}

int fn_802B5448(ValueNode* n) { return !strcasecmp("include", n->GetName()); }

int fn_802B5488(ValueNode* n) { return !strcasecmp("global_parameters", n->GetName()); }

int fn_802B54CC(ValueNode* n) { return !strcasecmp("unified_material_set", n->GetName()); }

ValueTable* fn_802B561C(TableLoader f, const char* name);

void fn_802B5510(const char* name) {
    ValueTable* t = fn_802B561C(fn_80131810, name);
    if (t) {
        ValueNode* n;
        int i;
        *lbl_804BE454.Next() = t;
        for (i = 0; (n = t->GetChild(i)) != 0; i++) {
            if (fn_802B5448(n)) {
                fn_802B5510(n->GetValue("name"));
            } else if (fn_802B5488(n) && !lbl_8055F9E8) {
                lbl_8055F9E8 = n;
            }
        }
    } else {
        fn_802E317C("UMat: Can't load %s", name);
    }
}

ValueTable* fn_802B561C(TableLoader f, const char* name) { return f(name); }

void fn_802B562C(UMat* m, ValueNode* n) {
    const char* def;
    const char* sky;
    void* model;
    int k;
    m->name = n->GetValue("name");
    fn_802B0B20(n, n->FindChild("ground_tex_params"));
    fn_802B224C(n);
    m->x4 = fn_802B1138(m->name);
    m->x8 = fn_802B2C98(m->name);
    def = n->GetString("road_tex", "");
    for (k = 0; k < 4; k++) {
        char key[32];
        fn_801345D8(key, sizeof(key), "road_tex%d", k + 1);
        m->road[k].name = n->GetString(key, def);
        m->road[k].tex = fn_802B110C(fn_802B539C, m->road[k].name);
    }
    {
        const char* skyName = n->GetString("sky_model", 0);
        void* mdl = skyName ? fn_802B2C34(fn_802C6B44, skyName) : 0;
        if (mdl) {
            m->sky = fn_802C6DC4(skyName, mdl, 0, 0);
        } else {
            m->sky = -1;
        }
    }
}

void fn_802B57B8(void) {
    const char* name = "umat.val";
    fn_8012E96C(name);
    fn_802B5510(name);
    if (!lbl_8055F9E8) {
        lbl_8055F9E8 = fn_8013184C();
    }
}

// NonMatching: the inlined Free() loop swaps two callee-saved registers (6 instructions).
void fn_802B5808(void) {
    {
        int i;
        UMat* m;
        for (i = 0; i < lbl_804BE4AC.count; i++) {
            m = &lbl_804BE4AC.data[i];
            m->Free();
        }
    }
    {
        int i;
        for (i = 0; i < lbl_804BE454.count; i++) {
            fn_80131818(lbl_804BE454.data[i]);
        }
    }
    lbl_804BE454.count = 0;
    lbl_804BE4AC.count = 0;
    lbl_8055F9E8 = 0;
}

void fn_802B58E0(void) {
    int i;
    for (i = 0; i < lbl_804BE454.count; i++) {
        ValueTable* t = lbl_804BE454.data[i];
        ValueNode* n;
        int j;
        for (j = 0; (n = t->GetChild(j)) != 0; j++) {
            if (fn_802B54CC(n)) {
                fn_802B562C(lbl_804BE4AC.Next(), n);
            }
        }
    }
}

UMat* fn_802B59A4(u32 i) { return &lbl_804BE4AC.data[i % lbl_804BE4AC.count]; }

ValueNode* fn_802B59CC(void) { return lbl_8055F9E8; }

}  // extern "C"
