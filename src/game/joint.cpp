// Joints (0x801E2C1C-0x801E3640): named joints from a model's MRJT chunks, pooled joint instances
// with a local and a base transform, and their world matrices. Names are guesses apart from the
// messages.
#include "game/common.h"

extern "C" {
double cos(double x);
double sin(double x);
char* strchr(const char* s, int c);
char* strcpy(char* dst, const char* src);
int strcasecmp(const char* a, const char* b);
int sscanf(const char* s, const char* fmt, ...);
void fn_802E30C8(const char* fmt, ...);  // debug print
int fn_8012C33C(const char* name);       // resource exists
s32 fn_802C6FC0(const char* name, int a, int b);
void fn_802C6CF8(s32 inst);
void fn_802C744C(s32 inst, const ValMatrix* m);
}

void* operator new(size_t size, const char* tag);

// A fixed-size block pool.
class Pool {
public:
    Pool(const char* name, s32 count, s32 size);
    virtual ~Pool(void);
    void* Alloc(void);
    void Free(void* p);

    u8 pad4[0x1C];
};

class JointPool : public Pool {
public:
    JointPool(const char* name, s32 count, s32 size) : Pool(name, count, size) {}
    virtual ~JointPool(void) {}
};

struct Mtx33 {
    ValVec3 basis[3];
};

extern "C" {
void fn_801BFD34(ValVec3* out, const ValVec3* v, const ValMatrix* m);
}

struct Mat3;
void Mat3Mul(Mat3* out, const Mat3* a, const Mat3* b);

// One joint record in a model's MRJT chunk: "name model" and the joint's transform.
struct JointEntry {
    char name[0x20];  // offset 0x0
    ValMatrix mtx;    // offset 0x20
};

struct JointChunk {
    u32 tag;               // offset 0x0, 'MRJT'
    u32 unk4;              // offset 0x4
    int count;             // offset 0x8
    JointEntry entries[1]; // offset 0xC
};

struct ModelData {
    u8 pad0[0x10];
    int numChunks;         // offset 0x10
    JointChunk** chunks;   // offset 0x14
};

extern "C" ModelData* fn_802C6C34(int model);

struct Joint {
    s32 inst;          // offset 0x0
    Joint* parent;     // offset 0x4
    ValMatrix local;   // offset 0x8
    ValMatrix base;    // offset 0x38
};

ModuleTag lbl_8055EA48;
JointPool* lbl_8055EA50;  // joint pool

static inline void Identity(ValMatrix* m) {
    m->basis[0].x = 1.0f;
    m->basis[0].y = 0.0f;
    m->basis[0].z = 0.0f;
    m->basis[1].x = 0.0f;
    m->basis[1].y = 1.0f;
    m->basis[1].z = 0.0f;
    m->basis[2].x = 0.0f;
    m->basis[2].y = 0.0f;
    m->basis[2].z = 1.0f;
    m->origin.x = 0.0f;
    m->origin.y = 0.0f;
    m->origin.z = 0.0f;
}

static inline void RotZ(Mtx33* m, f32 angle) {
    f32 c = cos(angle);
    f32 s = sin(angle);
    m->basis[0].x = c;
    m->basis[0].y = s;
    m->basis[0].z = 0.0f;
    m->basis[1].x = -s;
    m->basis[1].y = c;
    m->basis[1].z = 0.0f;
    m->basis[2].x = 0.0f;
    m->basis[2].y = 0.0f;
    m->basis[2].z = 1.0f;
}

extern "C" {

int fn_801E2C1C(int model, int index, JointEntry** entry, ValMatrix** mtx) {
    ModelData* md = fn_802C6C34(model);
    for (int i = 0; i < md->numChunks; i++) {
        JointChunk* c = md->chunks[i];
        if (c->tag == 'MRJT' && index >= 0 && index < c->count) {
            *entry = &c->entries[index];
            *mtx = &c->entries[index].mtx;
            return 1;
        }
    }
    return 0;
}

int fn_801E2CCC(int model, const char* name, ValMatrix** out) {
    bool full;
    ValMatrix* mtx;
    JointEntry* entry;
    mtx = NULL;
    entry = NULL;
    full = strchr(name, ' ') != NULL;
    for (int i = 0; fn_801E2C1C(model, i, &entry, &mtx); i++) {
        char jointName[0x40] = {0};
        if (!full) {
            sscanf(entry->name, "%s", jointName);
        } else {
            strcpy(jointName, entry->name);
        }
        if (strcasecmp(jointName, name) == 0) {
            *out = mtx;
            return 1;
        }
    }
    *out = NULL;
    return 0;
}

JointEntry* fn_801E2DEC(int model, const char* name, char* modelName) {
    JointEntry* e;
    ModelData* md = fn_802C6C34(model);
    for (int i = 0; i < md->numChunks; i++) {
        JointChunk* c = md->chunks[i];
        if (c->tag == 'MRJT') {
            e = c->entries;
            for (int k = 0; k < c->count; e++, k++) {
                char jointName[0x20];
                char other[0x20] = {0};
                if (sscanf(e->name, "%s %s", jointName, other) >= 1 && strcasecmp(jointName, name) == 0) {
                    if (modelName) {
                        strcpy(modelName, other);
                    }
                    return &c->entries[k];
                }
            }
        }
    }
    return NULL;
}

void fn_801E2F00(void) { lbl_8055EA50 = new ("Joint Pool") JointPool("Joint Pool", 0x128, sizeof(Joint)); }

void fn_801E2F68(void) {
    delete lbl_8055EA50;
    lbl_8055EA50 = NULL;
}

Joint* fn_801E3004(int model, const char* name, int flags) {
    Joint* j = (Joint*)lbl_8055EA50->Alloc();
    j->parent = NULL;
    j->inst = -1;
    Identity(&j->local);
    Identity(&j->base);
    if (model == -1) {
        return j;
    }
    char modelName[0x28];
    JointEntry* e = fn_801E2DEC(model, name, modelName);
    if (e) {
        j->base = e->mtx;
        if (modelName[0] && strchr(modelName, '.')) {
            if (fn_8012C33C(modelName)) {
                if (flags >= 0) {
                    j->inst = fn_802C6FC0(modelName, flags, 0);
                } else {
                    j->inst = fn_802C6FC0(modelName, 0, 0);
                }
            } else {
                fn_802E30C8("Joint %s: can't load model %s", name, modelName);
            }
        }
    }
    return j;
}

Joint* fn_801E31D8(Joint* parent, const char* name, int flags) {
    Joint* j = fn_801E3004(parent->inst, name, flags);
    j->parent = parent;
    return j;
}

int fn_801E320C(int model, const char* name) { return fn_801E2DEC(model, name, NULL) != NULL; }

void fn_801E323C(Joint* j) {
    if (j->inst != -1) {
        fn_802C6CF8(j->inst);
    }
    lbl_8055EA50->Free(j);
}

void fn_801E3284(ValMatrix* out, const Joint* j, const ValMatrix* root) {
    ValMatrix m;
    if (j->parent) {
        fn_801E3284(&m, j->parent, root);
    } else {
        m = *root;
    }
    ValMatrix b;
    fn_801BFD34(&b.origin, &j->base.origin, &m);
    Mat3Mul((Mat3*)&b, (const Mat3*)&j->base, (const Mat3*)&m);
    ValMatrix d;
    fn_801BFD34(&d.origin, &j->local.origin, &b);
    Mat3Mul((Mat3*)&d, (const Mat3*)&j->local, (const Mat3*)&b);
    *out = d;
}

void fn_801E33E4(ValMatrix* out, const Joint* j) {
    ValMatrix root;
    Identity(&root);
    fn_801E3284(out, j, &root);
}

void fn_801E3440(ValVec3* out, const Joint* j, const ValMatrix* root) {
    ValMatrix w;
    fn_801E3284(&w, j, root);
    out->x = w.origin.x;
    out->y = w.origin.y;
    out->z = w.origin.z;
}

void fn_801E3488(ValVec3* out, const Joint* j) {
    ValMatrix root;
    Identity(&root);
    fn_801E3440(out, j, &root);
}

void fn_801E34E4(const Joint* j, const ValMatrix* root) {
    if (j->inst != -1) {
        ValMatrix w;
        fn_801E3284(&w, j, root);
        fn_802C744C(j->inst, &w);
    }
}

s32 fn_801E3534(const Joint* j) { return j->inst; }

void fn_801E35D4(Joint* j, const Mtx33* r);

void fn_801E353C(Joint* j, f32 angle) {
    Mtx33 r;
    RotZ(&r, angle);
    fn_801E35D4(j, &r);
}

void fn_801E35D4(Joint* j, const Mtx33* r) {
    j->local.origin.x = 0.0f;
    j->local.origin.y = 0.0f;
    j->local.origin.z = 0.0f;
    *(Mtx33*)j->local.basis = *r;
}

ValVec3* fn_801E3630(Joint* j) { return &j->base.origin; }

}  // extern "C"

// NonMatching: needs the Pool base (pool.cpp) symbols to link; fn_801E353C keeps &r in r31 in the
// target, and fn_801E2CCC/fn_801E2DEC number a few registers differently.
