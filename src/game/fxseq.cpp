// Effect sequences (0x802971B0-0x802978B8): a .val list of timed effect elements (only flip
// books so far), each placed at an offset and played over its time window. Names are guesses
// apart from the keys and messages.
#include "game/common.h"

extern "C" int strcasecmp(const char* a, const char* b);

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

extern "C" {
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
int fn_8012C33C(const char* name);       // resource exists
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* table);
void* fn_80296B10(const char* resource, ValueNode* config);
void fn_80296D28(void* book);
void fn_80296D2C(void* book, f32 t, const ValVec3* pos);
f32 fn_802970C8(void* book);
}

// One kind of effect a sequence can place.
class FXElem {
public:
    virtual ~FXElem(void) {}
    virtual int IsValid(void) = 0;
    virtual f32 GetLength(void) = 0;
    virtual void Play(f32 t, const ValVec3* pos) = 0;
};

class FlipBookElem : public FXElem {
public:
    FlipBookElem(ValueNode* node) {
        const char* resource = node->GetValue("resource");
        book = fn_80296B10(resource, node->FindChild("config"));
    }
    virtual ~FlipBookElem(void) {
        if (book) {
            fn_80296D28(book);
        }
        book = NULL;
    }
    virtual int IsValid(void) { return book != NULL; }
    virtual void Play(f32 t, const ValVec3* pos) { fn_80296D2C(book, t, pos); }
    virtual f32 GetLength(void) { return fn_802970C8(book); }

    void* book;  // offset 0x4
};

static inline ValVec3 Vec3(f32 x, f32 y, f32 z) {
    ValVec3 v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

typedef FXElem* (*FXElemFactory)(ValueNode* node);

struct FXElemType {
    FXElemFactory create;
    const char* name;
};

class FXSeqNode {
public:
    FXSeqNode(void);
    ~FXSeqNode(void);
    int Init(ValueNode* node);

    f32 start;       // offset 0x0
    ValVec3 offset;  // offset 0x4
    FXElem* elem;    // offset 0x10
    f32 end;         // offset 0x14
};

class FXSequence {
public:
    static FXSequence* Load(const char* name);
    f32 GetLength(void);
    void Free(void);
    int Play(f32 t, const ValVec3* pos);

    FXSeqNode* nodes;  // offset 0x0
    int count;         // offset 0x4
    f32 length;        // offset 0x8
};

extern "C" FXElem* fn_802971B0(ValueNode* node);

static FXElemType lbl_8055CA38[] = {
    {fn_802971B0, "FlipBookElem"},
};

ModuleTag lbl_8055F790;

extern "C" FXElem* fn_802971B0(ValueNode* node) {
    FXElem* elem = new ("FlipBookElem") FlipBookElem(node);
    if (elem && !elem->IsValid()) {
        delete elem;
        elem = NULL;
    }
    return elem;
}

static FXElem* CreateElem(ValueNode* node) {
    int found = 0;
    FXElem* elem = NULL;
    const char* name = node->GetValue("name");
    if (strcasecmp(lbl_8055CA38[0].name, name) == 0) {
        elem = lbl_8055CA38[0].create(node);
        found = 1;
    }
    if (!found) {
        fn_802E317C("efx-seq: Don't recognize type %s", name);
    }
    return elem;
}

int FXSeqNode::Init(ValueNode* node) {
    FXElem* e = CreateElem(node);
    if (e) {
        start = node->GetFloat("start_t");
        const ValVec3& v = node->GetVec3Or("offset", Vec3(0.0f, 0.0f, 0.0f));
        offset.x = v.x;
        offset.y = v.y;
        offset.z = v.z;
        elem = e;
        end = start + e->GetLength();
    }
    return e != NULL;
}

static int IsSeqNode(ValueNode* node) { return strcasecmp(node->GetName(), "seq") == 0; }

f32 FXSequence::GetLength(void) {
    f32 len = 0.0f;
    for (int i = 0; i < count; i++) {
        len = (len > nodes[i].end) ? len : nodes[i].end;
    }
    return len;
}

FXSequence* FXSequence::Load(const char* name) {
    int n;
    ValueTable* table;
    FXSequence* seq;
    int total;
    int ok;
    int i;
    ValueNode* node;
    int k;
    seq = NULL;
    if (fn_8012C33C(name)) {
        table = fn_80131808(name);
        n = 0;
        total = 0;
        for (;;) {
            node = table->GetChild(total);
            if (node == NULL) {
                break;
            }
            total++;
            if (IsSeqNode(node)) {
                n++;
            }
        }
        if (n) {
            seq = new ("FXSequence") FXSequence;
            seq->count = n;
            seq->nodes = new ("FXSeqNodes") FXSeqNode[n];
            k = 0;
            ok = 1;
            for (i = 0; i < total; i++) {
                node = table->GetChild(i);
                if (IsSeqNode(node)) {
                    if (!seq->nodes[k++].Init(node)) {
                        ok = 0;
                        break;
                    }
                }
            }
            if (ok) {
                seq->length = seq->GetLength();
            } else {
                delete[] seq->nodes;
                delete seq;
                seq = NULL;
            }
        }
        fn_80131818(table);
    }
    return seq;
}

FXSeqNode::~FXSeqNode(void) {
    if (elem) {
        delete elem;
        elem = NULL;
    }
}

FXSeqNode::FXSeqNode(void) { elem = NULL; }

void FXSequence::Free(void) {
    delete[] nodes;
    delete this;
}

int FXSequence::Play(f32 t, const ValVec3* pos) {
    for (int i = 0; i < count; i++) {
        FXSeqNode* n = &nodes[i];
        if (t >= n->start && t <= n->end) {
            ValVec3 p;
            p.x = pos->x + n->offset.x;
            p.y = pos->y + n->offset.y;
            p.z = pos->z + n->offset.z;
            n->elem->Play(t - n->start, &p);
        }
    }
    return t < length;
}

// NonMatching: Load's induction-variable registers, GetLength's loop registers and the argument
// scheduling around GetVec3Or differ.
