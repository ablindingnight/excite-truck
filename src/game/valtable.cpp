// Value tables (0x80131588-0x80132E7C): 'VALT' resources of named nodes holding key/value string pairs,
// with parent inheritance, typed getters (int/float/bool/enum/scale/vector/point/matrix/real), '@'
// child links and checksums. Byte-swapped and pointer-relocated in place on load. Names are guesses.
#include "game/common.h"

extern "C" {
int strcasecmp(const char* a, const char* b);
size_t strlen(const char* s);
int sprintf(char* s, const char* fmt, ...);
int sscanf(const char* s, const char* fmt, ...);
int atoi(const char* s);
double atof(const char* s);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
void* fn_8012C370(const char* name, u32 type, u32* info, s32* size, bool* first, bool* got);
int fn_8012C4D8(void* p);
const char* fn_80131234(const char* key);  // Localize
int fn_8012F9E8(void* out, s32 size, const char* in);  // base64 decode
void __register_global_object(void* obj, void* dtor, void* chain);
}

struct ValuePair {
    const char* key;
    const char* value;
};

static inline void Swap4(u8* p) {
    u8 t = p[0];
    p[0] = p[3];
    p[3] = t;
    t = p[1];
    p[1] = p[2];
    p[2] = t;
}

class ValueFile;

class ValueNodeImpl : public ValueNode {
public:
    ValueNodeImpl(void) { parent = 0; }
    virtual const char* GetName(void) {
        const char* n = name;
        if (*n == '$') {
            n += 6;
        }
        return n;
    }
    virtual ~ValueNodeImpl(void) {}

    ValuePair* FindPair(const char* key) {
        struct {
            const char* key;
            s32 unk;
        } k = {key, 0};
        ValuePair* r = 0;
        int i;
        for (i = 0; i < count; i++) {
            if (!strcasecmp(key, pairs[i].key)) {
                r = &pairs[i];
                break;
            }
        }
        return r;
    }
    virtual int Has(const char* key) {
        ValuePair* p = FindPair(key);
        if (!p && parent) {
            p = parent->FindPairInherited(key);
        }
        return p != 0;
    }
    ValuePair* FindPairInherited(const char* key) {
        ValuePair* p = FindPair(key);
        if (!p && parent) {
            p = parent->FindPairInherited(key);
        }
        return p;
    }

    virtual int GetPair(int index, const char** key, const char** value) {
        if (index >= 0 && index < count) {
            ValuePair* p = pairs;
            *key = p[index].key;
            *value = p[index].value;
            return 1;
        }
        return 0;
    }
    virtual const char* GetValue(const char* key) {
        ValuePair* p = FindPair(key);
        if (!p && parent) {
            p = parent->FindPairInherited(key);
        }
        if (!p) {
            fn_802E317C("ValueTable: Can't find entry \"%s\" in \"%s\"", key, GetName());
        }
        return p->value;
    }
    virtual s32 GetIntValue(const char* key) { return atoi(GetValue(key)); }
    virtual f32 GetFloat(const char* key) { return atof(GetValue(key)); }
    virtual s32 GetIntValue2(const char* key) {
        int r;
        const char* s = GetValue(key);
        r = 0;
        if (!strcasecmp(s, "yes") || !strcasecmp(s, "true")) {
            r = 1;
        }
        return r;
    }
    virtual int GetEnum(const char* key, const char* names) {
        const char* s = GetValue(key);
        int i;
        for (i = 0; *names; i++) {
            if (!strcasecmp(names, s)) {
                return i;
            }
            names += strlen(names) + 1;
        }
        fn_802E317C("Can't find enum value \"%s\" for \"%s\" in \"%s\"", s, key, GetName());
        return -1;
    }
    virtual f32 GetScale(const char* key, f32 t) {
        f32 a;
        f32 b;
        const char* s = GetValue(key);
        if (sscanf(s, "%f, %f", &a, &b) != 2) {
            fn_802E317C("Bad scale value \"%s\" for \"%s\" in \"%s\"", s, key, GetName());
        }
        return a + t * (b - a);
    }
    virtual ValVec3 GetVec3(const char* key) {
        ValVec3 v;
        const char* s = GetValue(key);
        if (sscanf(s, "%f, %f, %f", &v.x, &v.y, &v.z) != 3) {
            fn_802E317C("Bad vector value \"%s\" for \"%s\" in \"%s\"", s, key, GetName());
        }
        return v;
    }
    virtual ValPoint GetPoint(const char* key) {
        ValPoint v;
        const char* s = GetValue(key);
        if (sscanf(s, "%f, %f", &v.x, &v.y) != 2) {
            fn_802E317C("Bad vector value \"%s\" for \"%s\" in \"%s\"", s, key, GetName());
        }
        return v;
    }
    virtual ValMatrix GetMatrix(const char* key) {
        ValMatrix m;
        if (fn_8012F9E8(&m, sizeof(m), GetValue(key))) {
            int i;
            u32* w = (u32*)&m;
            for (i = 0; i < 12; i++) {
                Swap4((u8*)&w[i]);
            }
        } else {
            m.basis[0] = GetVec3("basis_x");
            m.basis[1] = GetVec3("basis_y");
            m.basis[2] = GetVec3("basis_z");
            m.origin = GetVec3("origin");
        }
        return m;
    }
    virtual f32 GetReal(const char* key) {
        f32 v;
        const char* s = GetValue(key);
        if (fn_8012F9E8(&v, sizeof(v), s)) {
            Swap4((u8*)&v);
        } else {
            fn_802E317C("Bad real64 value \"%s\" for \"%s\" in \"%s\"", s, key, GetName());
        }
        return v;
    }
    virtual ValueNode* GetChildAt(int index) {
        int i;
        for (i = 0; i < count; i++) {
            if (pairs[i].key[0] == '@' && index-- == 0) {
                return ((ValueTable*)file)->FindChild(pairs[i].value);
            }
        }
        if (parent) {
            return parent->GetChildAt(index);
        }
        return 0;
    }
    virtual ValueNode* FindChild(const char* child) {
        char buf[0x40];
        sprintf(buf, "@%s", child);
        if (Has(buf)) {
            return ((ValueTable*)file)->FindChild(GetValue(buf));
        }
        return 0;
    }
    virtual ValVec3 GetVec3Or(const char* key, const ValVec3& def) { return Has(key) ? GetVec3(key) : def; }
    virtual ValPoint GetPointOr(const char* key, const ValPoint& def) {
        return Has(key) ? GetPoint(key) : def;
    }
    virtual ValMatrix GetMatrixOr(const char* key, const ValMatrix& def) {
        return Has(key) ? GetMatrix(key) : def;
    }
    virtual f32 GetRealOr(const char* key, f32 def) { return Has(key) ? GetReal(key) : def; }
    const char* name;          // offset 0x4
    const char* parentName;    // offset 0x8
    s32 count;                 // offset 0xC
    ValuePair* pairs;          // offset 0x10
    ValueNodeImpl* parent;     // offset 0x14
    ValueFile* file;           // offset 0x18
};

class ValueNodeStatic : public ValueNodeImpl {
public:
    ValueNodeStatic(void) {
        name = "";
        parentName = "";
        count = 0;
        pairs = 0;
        parent = 0;
        file = 0;
    }
    virtual ~ValueNodeStatic(void) {}
};

class ValueFileNode : public ValueNodeImpl {
public:
    ValueFileNode(ValueFile* base);
    virtual ~ValueFileNode(void) {}
    virtual u32 Checksum(void) {
        u32 h = 0xBABBA;
        int i;
        for (i = 0; i < count; i++) {
            const char* s;
            for (s = pairs[i].key; *s; s++) {
                h += *s;
                h += 0xBAF;
            }
            for (s = pairs[i].value; *s; s++) {
                h += *s;
                h += 0xFABACAB;
            }
        }
        return h;
    }
};

class ValueFile : public ValueTable {
public:
    ValueFile(void) { nodes = (ValueFileNode*)data; }
    virtual ValueNode* GetChild(int index) {
        if (index < unk8) {
            return &nodes[index];
        }
        return 0;
    }
    virtual ValueNode* FindChild(const char* name) {
        int i;
        for (i = 0; i < count; i++) {
            if (!strcasecmp(nodes[i].name, name)) {
                return &nodes[i];
            }
        }
        fn_802E317C("ValueTable \"%s\" not found in %d tables", name, count);
        return 0;
    }
    virtual int HasChild(const char* name) {
        int i;
        for (i = 0; i < count; i++) {
            if (!strcasecmp(nodes[i].name, name)) {
                return 1;
            }
        }
        return 0;
    }
    virtual u32 Checksum(void) {
        u32 h = 0xFEEDFACE;
        int i;
        for (i = 0; i < count; i++) {
            h += nodes[i].Checksum();
            h += 0x123B;
        }
        return h;
    }
    virtual ~ValueFile(void) {}

    void ResolveParents(void) {
        int i;
        for (i = 0; i < count; i++) {
            const char* pn = nodes[i].parentName;
            if (pn && *pn) {
                int found = 0;
                int j;
                for (j = 0; j < count; j++) {
                    if (!strcasecmp(pn, nodes[j].name)) {
                        if (i == j) {
                            fn_802E317C("valtab: table \"%s\" parented to self!", pn);
                        }
                        if (found) {
                            fn_802E317C("valtab: table \"%s\" has ambiguous parent \"%s\"", nodes[i].GetName(), pn);
                        }
                        found = 1;
                        nodes[i].parent = &nodes[j];
                    }
                }
                if (!found) {
                    fn_802E317C("valtab: table \"%s\" has missing parent \"%s\"", nodes[i].GetName(), pn);
                }
            }
        }
    }

    s32 count;              // offset 0x4
    s32 unk8;
    ValueFileNode* nodes;   // offset 0xC
    u8 data[4];             // offset 0x10
};

ModuleTag lbl_8055E058;

extern "C" {

ValueTable* fn_80131588(const char* name, int quiet) {
    u32 version;
    bool swap;
    ValueFile* f = (ValueFile*)fn_8012C370(name, 'VALT', &version, 0, 0, &swap);
    if (f) {
        if (version == 11) {
            if (swap) {
                int i;
                new (f) ValueFile;
                Swap4((u8*)&f->count);
                Swap4((u8*)&f->unk8);
                for (i = 0; i < f->count; i++) {
                    new (&f->nodes[i]) ValueFileNode(f);
                    f->nodes[i].file = f;
                }
                f->ResolveParents();
            }
        } else {
            fn_802E317C("valtable: mismatched version on %s", name);
        }
    } else if (!quiet) {
        fn_802E317C("Can't get value file %s", name);
    }
    return f;
}

ValueTable* fn_80131808(const char* name) { return fn_80131588(name, 0); }

ValueTable* fn_80131810(const char* name) { return fn_80131588(name, 1); }

void fn_80131818(ValueTable* t) { fn_8012C4D8(t); }

ValueTable* fn_8013181C(const char* name) {
    fn_802E30C8("Attempt to call ValueFileLoadHumanReadable() from non-PC platform???");
    return 0;
}

ValueNode* fn_8013184C(void) {
    static ValueNodeStatic s_empty;
    return &s_empty;
}

}  // extern "C"

ValueFileNode::ValueFileNode(ValueFile* base) {
    int i;
    Swap4((u8*)&pairs);
    pairs = (ValuePair*)((u8*)pairs + (u32)base);
    Swap4((u8*)&count);
    Swap4((u8*)&name);
    Swap4((u8*)&parentName);
    if (name) {
        name += (u32)base;
    }
    if (parentName) {
        parentName += (u32)base;
    }
    for (i = 0; i < count; i++) {
        ValuePair* p = &pairs[i];
        Swap4((u8*)&p->key);
        Swap4((u8*)&p->value);
        p->key += (u32)base;
        p->value += (u32)base;
        if (*p->value == '|') {
            p->value = fn_80131234(p->value + 1);
        } else if (*p->value == '~') {
            *(char*)p->value = 1;
        }
    }
}
