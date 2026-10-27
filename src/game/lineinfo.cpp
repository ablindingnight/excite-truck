// Track line info (0x8013BC90-0x8013C748): per-track AI tuning loaded from lineinfo.val (lookahead
// times/distances and AI exceptions), shared through a small reference-counted cache keyed by the ideal
// line resource name. Names are guesses apart from the keys and messages.
#include "game/common.h"

extern "C" {
char* strchr(const char* s, int c);
char* strrchr(const char* s, int c);
int strcmp(const char* a, const char* b);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_80128658(void* p);               // MemFree
ValueTable* fn_80131810(const char* name);
void fn_80131818(ValueTable* t);
ValueNode* fn_8013184C(void);  // empty value node
}

void* operator new(size_t size, const char* tag);

class AIExceptions {
public:
    AIExceptions(void);
    ~AIExceptions(void);
    void Load(ValueNode* node);

    void* list;
    f32 tune[7];
};

struct LineParams {
    f32 minT;      // offset 0x0
    f32 maxT;      // offset 0x4
    f32 minDist;   // offset 0x8
    f32 curvT;     // offset 0xC
    f32 curvDist;  // offset 0x10
};

struct LineInfo {
    char name[64];        // offset 0x0
    AIExceptions exc;     // offset 0x40
    LineParams params;    // offset 0x60
    void* extra;          // offset 0x74
};

struct Name64 {
    char s[64];
};

struct LineEntry {
    LineInfo* info;  // offset 0x0
    Name64 name;     // offset 0x4
    s32 refs;        // offset 0x44
};

struct LineCache {
    LineCache(void) { count = 0; }
    LineEntry e[8];
    s32 count;  // offset 0x240
};

static inline bool StrEq(const char* a, const char* b) { return strcmp(a, b) == 0; }
static inline void StrCopyN(const char* s, int n, char* d) {
    while ((*d++ = *s++) != 0) {
        if (--n <= 0) {
            d[-1] = 0;
            break;
        }
    }
}

ModuleTag lbl_8055E178;
LineCache lbl_8042F8C8;

extern "C" {

char* fn_8013BC90(char* buf, s32 n, const char* src) {
    char* p = buf;
    StrCopyN(src, n, buf);
    if (strchr(p, '/')) {
        p = strchr(p, '/') + 1;
    }
    {
        char* dot = strrchr(p, '.');
        if (dot) {
            *dot = 0;
        }
    }
    return p;
}

void fn_8013BD30(LineParams* p) {
    p->minT = 0.5f;
    p->maxT = 1.5f;
    p->minDist = 13.0f;
    p->curvDist = 60.0f;
    p->curvT = 0.0f;
}

void fn_8013BD5C(LineParams* p, ValueNode* t) {
    ValueNode* n;
    fn_8013BD30(p);
    n = t->FindChild("lookahead");
    if (n) {
        p->minT = n->GetFloat("min_t", p->minT);
        p->maxT = n->GetFloat("max_t", p->maxT);
        p->minDist = n->GetFloat("min_dist", p->minDist);
        p->curvDist = n->GetFloat("curv_dist", p->curvDist);
        p->curvT = n->GetFloat("curv_t", p->curvT);
    }
}

// Stripped by the linker; keeps its literals in place.
ValueNode* fn_8013BD5C_exit(ValueNode* t) {
    ValueNode* n = t->FindChild("lineinfo");
    return n ? n->FindChild("exit_path") : 0;
}

LineInfo* fn_8013BE64(ValueNode* t, const char* name) {
    LineInfo* li = new ("LineInfo") LineInfo;
    StrCopyN(name, 64, li->name);
    li->exc.Load(t->FindChild("exceptions"));
    fn_8013BD5C(&li->params, t);
    li->extra = 0;
    return li;
}

void fn_8013BF3C(LineParams* p) { fn_8013BD30(p); }

LineInfo* fn_8013BF40(const char* name) {
    LineInfo* li = new ("LineInfo") LineInfo;
    StrCopyN(name, 64, li->name);
    fn_8013BF3C(&li->params);
    li->extra = 0;
    return li;
}

void fn_8013BFE0(LineInfo* li) {}

void fn_8013BFE4(LineInfo* li) {}

}  // extern "C"

static inline LineEntry* CacheAdd(LineInfo* li, Name64 n) {
    LineEntry* e = &lbl_8042F8C8.e[lbl_8042F8C8.count++];
    e->info = li;
    e->name = n;
    e->refs = 0;
    return e;
}

extern "C" {

LineInfo* fn_8013BFE8(const char* full) {
    int n;
    int i;
    Name64 name;
    LineEntry* found;
    ValueTable* t;
    LineInfo* li;
    StrCopyN(full, 64, name.s);
    n = lbl_8042F8C8.count;
    found = 0;
    for (i = 0; i < n; i++) {
        if (StrEq(name.s, lbl_8042F8C8.e[i].name.s)) {
            found = &lbl_8042F8C8.e[i];
            break;
        }
    }
    if (!found) {
        li = 0;
        t = fn_80131810("lineinfo.val");
        if (t) {
            char buf[0x80];
            char* key = fn_8013BC90(buf, 0x80, name.s);
            ValueNode* n = t->HasChild(key) ? t->FindChild(key) : 0;
            if (n) {
                fn_802E30C8("lineinfo: using table %s", key);
            } else {
                n = fn_8013184C();
            }
            li = fn_8013BE64(n, name.s);
            fn_80131818(t);
        }
        if (!li) {
            li = fn_8013BF40(name.s);
        }
        if (li) {
            fn_8013BFE0(li);
            fn_8013BFE4(li);
        }
        if (li) {
            found = CacheAdd(li, name);
        }
    }
    if (found) {
        found->refs++;
        return found->info;
    }
    return 0;
}

void fn_8013C428(LineInfo* li) {
    int i;
    for (i = 0; i < lbl_8042F8C8.count; i++) {
        if (lbl_8042F8C8.e[i].info == li) {
            LineEntry* e = &lbl_8042F8C8.e[i];
            if (--e->refs == 0) {
                if (li->extra) {
                    fn_80128658(li->extra);
                }
                delete li;
                lbl_8042F8C8.e[i] = lbl_8042F8C8.e[--lbl_8042F8C8.count];
            }
            return;
        }
    }
}

}  // extern "C"
