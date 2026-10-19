// Rings (0x8016A0B0-0x8016B964): the stunt/freestyle rings of a ring challenge. The ring types are the
// stunt_ring_N.mod / freestyle_ring_N.mod models (N = radius), the contest's "ring_file" lists "ring_set"s of
// "ring"s (radius + frame64). Sets are switched on and off by name; a car passes a ring by getting within its
// radius, and rings fade in, flash and fade out. Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
int strcmp(const char* a, const char* b);
int strcasecmp(const char* a, const char* b);
void fn_802E30C8(const char* fmt, ...);  // debug print
int fn_801345D8(char* buf, u32 size, const char* fmt, ...);  // snprintf
ValueTable* fn_80131810(const char* name);
void fn_80131818(ValueTable* t);
const char* fn_80204EA0(const char* name);  // tuning string
int fn_8012C33C(const char* name);          // file exists
s32 fn_802C6FC0(const char* name, int a, int b);
void fn_802C6CF8(s32 model);
void fn_802C70AC(s32 model, const ValMatrix* m, int a, int b);
void fn_802C9900(f32 a);
void fn_802C9904(void);
f32 fn_801E7E18(void);
f32 PSVECSquareDistance(const void* a, const void* b);
f32 PSVECDotProduct(const void* a, const void* b);
}

void* operator new[](size_t size, const char* tag);

struct Event {
    u8 pad0[0x20];
    ValueNode* vals;  // offset 0x20
};

struct RaceCarRec {
    u8 pad0[0x60];
    f32 pos[3];  // offset 0x60
};

struct V2 {
    f32 x;
    f32 z;
};

struct V3 {
    f32 x;
    f32 y;
    f32 z;
};

extern "C" {
Event* fn_8000AC38(void);
RaceCarRec* fn_8017960C(int id);
}

extern f32 lbl_8055E3C8;  // race time

template <class T>
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
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
};

template <class T>
struct MechaSimpleArray : ArrayRef<T> {
    MechaSimpleArray(void) : ArrayRef<T>(0, 0) {
        bufCount = 0;
        buf = 0;
    }
    ~MechaSimpleArray(void) { Free(); }
    void Free(void) {
        if (buf) {
            delete[] buf;
            buf = 0;
            bufCount = 0;
            this->data = 0;
            this->count = 0;
            this->cap = 0;
        }
    }
    void Reserve(int n) {
        Free();
        bufCount = n;
        buf = new ("MechaSimpleArray_tab") T[n];
        this->data = buf;
        this->cap = bufCount;
        this->count = 0;
    }
    T* Add(void) { return &this->data[this->count++]; }
    void Remove(int k) {
        if (k >= 0 && k < this->count) {
            int last = this->count - 1;
            if (k != last) {
                this->data[k] = this->data[last];
            }
            this->count--;
        }
    }
    s32 pad;       // offset 0xC
    T* buf;        // offset 0x10
    s32 bufCount;  // offset 0x14
};

struct RingType {
    RingType(void) { loaded = 0; }
    ~RingType(void) {
        if (loaded) {
            fn_802C6CF8(model);
            loaded = 0;
        }
    }
    u8 loaded;      // offset 0x0
    char name[31];  // offset 0x1
    s32 model;      // offset 0x20
    s32 radius;     // offset 0x24
};

struct Ring {
    Ring(void) { valid = 0; }
    ~Ring(void) { valid = 0; }
    u8 valid;        // offset 0x0
    s32 radius;      // offset 0x4
    RingType* type;  // offset 0x8
    ValMatrix frame;  // offset 0xC
    u8 active;       // offset 0x3C
    u8 passed;       // offset 0x3D
    f32 passTime;    // offset 0x40
    f32 fadeTime;    // offset 0x44
};

struct RingSet {
    RingSet(void) { valid = 0; }
    ~RingSet(void) { valid = 0; }
    u8 valid;       // offset 0x0
    char name[31];  // offset 0x1
    s32 first;      // offset 0x20
    s32 count;      // offset 0x24
    u8 enabled;     // offset 0x28
};

ModuleTag lbl_8055E358;
UnkStaticFFFF lbl_8055E360;
u8 lbl_8055E364;  // radius_score_mode

MechaSimpleArray<RingType> lbl_804302F0;
MechaSimpleArray<Ring> lbl_80430318;
MechaSimpleArray<RingSet> lbl_80430340;

static inline void StrCopyN(char* dst, const char* src, int n) {
    while (--n > 0) {
        if ((*dst++ = *src++) == 0) {
            return;
        }
    }
    *dst = 0;
}

extern "C" {

void fn_8016A8F8(void);
void fn_8016AC90(void);
void fn_8016B598(void);
void fn_8016B634(void);

void fn_8016A0B0(void) {
    fn_8016A8F8();
    fn_8016AC90();
}

void fn_8016A0D4(void) {
    int i;
    for (i = 0; i < lbl_80430340.count; i++) {
        lbl_80430340.data[i].enabled = 0;
    }
    for (i = 0; i < lbl_80430318.count; i++) {
        Ring* r = &lbl_80430318.data[i];
        r->active = 0;
        r->passed = 0;
        r->passTime = -1.0f;
        r->fadeTime = -100.0f;
    }
}

int fn_8016A15C(const char* name) {
    int found = 0;
    int i;
    for (i = 0; i < lbl_80430340.count; i++) {
        if (strcasecmp(lbl_80430340.data[i].name, name) == 0) {
            RingSet* s = &lbl_80430340.data[i];
            int k;
            s->enabled = 1;
            for (k = 0; k < s->count; k++) {
                Ring* r = &lbl_80430318.data[s->first + k];
                r->passed = 0;
                r->passTime = -1.0f;
                r->active = 1;
                r->fadeTime = lbl_8055E3C8;
            }
            found = 1;
        }
    }
    return found;
}

void fn_8016A240(const char* name) {
    int i;
    for (i = 0; i < lbl_80430340.count; i++) {
        if (strcasecmp(lbl_80430340.data[i].name, name) == 0) {
            RingSet* s = &lbl_80430340.data[i];
            int k;
            s->enabled = 0;
            for (k = 0; k < s->count; k++) {
                Ring* r = &lbl_80430318.data[s->first + k];
                if (r->active) {
                    r->active = 0;
                    r->fadeTime = lbl_8055E3C8;
                }
            }
        }
    }
}

int fn_8016A30C(int player) {
    RaceCarRec* rec = fn_8017960C(player);
    if (rec) {
        int n = 0;
        int i;
        for (i = 0; i < lbl_80430318.count; i++) {
            Ring* r = &lbl_80430318.data[i];
            int hit = 0;
            int live = 0;
            if (r->active && !r->passed) {
                live = 1;
            }
            if (live) {
                f32 rad = (f32)r->radius;
                if (lbl_8055E364) {
                    rad += 1.0f;
                }
                if (PSVECSquareDistance(rec->pos, &r->frame.origin) < rad * rad) {
                    r->passed = 1;
                    hit = 1;
                    r->passTime = lbl_8055E3C8;
                }
            }
            if (hit) {
                n++;
            }
        }
        return n;
    }
    return 0;
}

void fn_8016A438(void) {
    int i;
    for (i = 0; i < lbl_80430340.count; i++) {
        RingSet* s = &lbl_80430340.data[i];
        int k = s->count;
        while (k-- > 0) {
            Ring* r = &lbl_80430318.data[s->first + k];
            if (r->valid) {
                f32 now = fn_801E7E18();
                f32 a = (now - r->fadeTime - 0.0f) / 0.25f;
                if (1.0f < a) {
                    a = 1.0f;
                }
                a = a > 0.0f ? a : 0.0f;
                a = 0.0f + a * 1.0f;
                if (!r->active) {
                    a = 1.0f - a;
                }
                if (a > 0.01f) {
                    ValMatrix m;
                    fn_802C9900(a);
                    m = r->frame;
                    m.basis[0].x *= a;
                    m.basis[0].y *= a;
                    m.basis[0].z *= a;
                    m.basis[1].x *= a;
                    m.basis[1].y *= a;
                    m.basis[1].z *= a;
                    m.basis[2].x *= a;
                    m.basis[2].y *= a;
                    m.basis[2].z *= a;
                    if (r->passed) {
                        fn_802C70AC(r->type->model, &m, -1, 4);
                        if (now - r->passTime > 0.5f && r->active) {
                            r->active = 0;
                            r->fadeTime = lbl_8055E3C8;
                        }
                    } else {
                        fn_802C70AC(r->type->model, &m, -1, 2);
                    }
                    fn_802C9904();
                }
            }
        }
    }
}

void fn_8016A68C(void) {
    fn_8016B598();
    fn_8016B634();
}

V2 fn_8016A6B0(const char* name, int radius) {
    int i;
    for (i = 0; i < lbl_80430340.count; i++) {
        if (strcasecmp(lbl_80430340.data[i].name, name) == 0 && radius == lbl_80430318.data[i].radius) {
            V2 p;
            p.z = lbl_80430318.data[i].frame.origin.z;
            p.x = lbl_80430318.data[i].frame.origin.x;
            return p;
        }
    }
    {
        V2 p;
        p.x = 0.0f;
        p.z = 0.0f;
        return p;
    }
}

int fn_8016A780(const char* name, int player) {
    int n = 0;
    RaceCarRec* rec = fn_8017960C(player);
    if (rec) {
        int i;
        for (i = 0; i < lbl_80430340.count; i++) {
            if (strcasecmp(lbl_80430340.data[i].name, name) == 0) {
                Ring* r = &lbl_80430318.data[i];
                V3 d;
                d.x = r->frame.origin.x - rec->pos[0];
                d.y = r->frame.origin.y - rec->pos[1];
                d.z = r->frame.origin.z - rec->pos[2];
                if (PSVECDotProduct(&d, &r->frame.basis[2]) < 0.0f) {
                    n++;
                }
            }
        }
    }
    return n;
}

int fn_8016A878(const char* name) {
    int n = 0;
    int i;
    for (i = 0; i < lbl_80430340.count; i++) {
        if (strcasecmp(lbl_80430340.data[i].name, name) == 0) {
            n++;
        }
    }
    return n;
}

void fn_8016A8F8(void) {
    int i;
    lbl_8055E364 = fn_8000AC38()->vals->GetBool("radius_score_mode", 0);
    lbl_804302F0.Reserve(5);
    for (i = 1; lbl_804302F0.count + 1 <= lbl_804302F0.cap && i < 100; i++) {
        RingType* t = lbl_804302F0.Add();
        if (t->loaded) {
            fn_802C6CF8(t->model);
            t->loaded = 0;
        }
        if (lbl_8055E364) {
            fn_801345D8(t->name, 30, "freestyle_ring_%d.mod", i);
        } else {
            fn_801345D8(t->name, 30, "stunt_ring_%d.mod", i);
        }
        if (fn_8012C33C(t->name)) {
            t->model = fn_802C6FC0(t->name, 0, 0);
            t->radius = i;
            t->loaded = 1;
            fn_802E30C8("RingType: created %d/%s", i, t->name);
        }
        if (!t->loaded) {
            lbl_804302F0.Remove(lbl_804302F0.count - 1);
        }
    }
}

void fn_8016AC90(void) {
    ValueNode* vals;
    const char* key;
    lbl_80430318.Reserve(180);
    lbl_80430340.Reserve(36);
    vals = fn_8000AC38()->vals;
    key = fn_80204EA0("ring_file_key");
    if (vals->Has(key)) {
        ValueTable* t = fn_80131810(vals->GetValue(key));
        if (t) {
            int i;
            ValueNode* n;
            for (i = 0; (n = t->GetChild(i)) != 0; i++) {
                if (strcmp(n->GetName(), "ring_set") == 0) {
                    RingSet* s = lbl_80430340.Add();
                    s->valid = 0;
                    if (n->Has("name")) {
                        int j;
                        ValueNode* c;
                        StrCopyN(s->name, n->GetValue("name"), 30);
                        s->first = lbl_80430318.count;
                        s->count = 0;
                        for (j = 0; (c = n->GetChildAt(j)) != 0; j++) {
                            if (strcmp(c->GetName(), "ring") == 0) {
                                Ring* r = lbl_80430318.Add();
                                r->valid = 0;
                                if (c->Has("radius")) {
                                    if (c->Has("frame64")) {
                                        int k;
                                        int idx;
                                        r->radius = c->GetIntValue("radius");
                                        r->frame = c->GetMatrix("frame64");
                                        idx = -1;
                                        for (k = 0; k < lbl_804302F0.count; k++) {
                                            if (lbl_804302F0.data[k].radius == r->radius) {
                                                idx = k;
                                                break;
                                            }
                                        }
                                        if (idx >= 0) {
                                            r->type = &lbl_804302F0.data[idx];
                                            r->active = 0;
                                            r->fadeTime = -100.0f;
                                            r->valid = 1;
                                        } else {
                                            fn_802E30C8("RingInfo: can't find a RingType with radius %d", r->radius);
                                        }
                                    } else {
                                        fn_802E30C8("RingInfo: value 'frame64' not in table");
                                    }
                                } else {
                                    fn_802E30C8("RingInfo: value 'radius' not in table");
                                }
                                if (r->valid) {
                                    s->count++;
                                } else {
                                    lbl_80430318.Remove(lbl_80430318.count - 1);
                                }
                            }
                        }
                        if (s->count > 2) {
                            V3 d0;
                            int k;
                            Ring* a = &lbl_80430318.data[s->first];
                            Ring* b = &lbl_80430318.data[s->first + 1];
                            d0.x = b->frame.origin.x - a->frame.origin.x;
                            d0.y = b->frame.origin.y - a->frame.origin.y;
                            d0.z = b->frame.origin.z - a->frame.origin.z;
                            for (k = s->first + 2; k < s->first + s->count; k++) {
                                V3 d;
                                Ring* p = &lbl_80430318.data[k - 1];
                                Ring* q = &lbl_80430318.data[k];
                                d.x = q->frame.origin.x - p->frame.origin.x;
                                d.y = q->frame.origin.y - p->frame.origin.y;
                                d.z = q->frame.origin.z - p->frame.origin.z;
                                if (PSVECDotProduct(&d, &d0) < 0.0f) {
                                    fn_802E30C8("----------------------------------------------------");
                                    fn_802E30C8("rings(%s): THESE RINGS ARE OUT OF ORDER", s->name);
                                    fn_802E30C8("----------------------------------------------------");
                                    break;
                                }
                            }
                        }
                        s->enabled = 0;
                        s->valid = 1;
                    } else {
                        fn_802E30C8("RingSet: value 'name' not found in table");
                    }
                    if (!s->valid) {
                        lbl_80430340.Remove(lbl_80430340.count - 1);
                    }
                }
            }
            fn_80131818(t);
        } else {
            fn_802E30C8("RingMaster: can't load ring_file '%s'", vals->GetValue("ring_file"));
        }
    } else {
        fn_802E30C8("RingMaster: contest table has no 'ring_file' entry");
    }
}

void fn_8016B598(void) {
    lbl_80430340.Free();
    lbl_80430318.Free();
}

void fn_8016B634(void) {
    int i;
    for (i = 0; i < lbl_804302F0.count; i++) {
        RingType* t = &lbl_804302F0.data[i];
        if (t->loaded) {
            fn_802C6CF8(t->model);
            t->loaded = 0;
        }
    }
    lbl_804302F0.Free();
}

}  // extern "C"
