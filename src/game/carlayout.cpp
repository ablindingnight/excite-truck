// Car layouts (0x80278A1C-0x80279370): each car's damage bones, bone links, wheel bones and
// bounds from its .car directory, shared by name and reference counted. Names are guesses apart
// from the keys and messages.
#include "game/common.h"

extern "C" {
int sprintf(char* buf, const char* fmt, ...);
void* memset(void* dst, int c, size_t n);
char* strncpy(char* dst, const char* src, size_t n);
int strcasecmp(const char* a, const char* b);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
int fn_8012C33C(const char* name);       // resource exists
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* table);
}

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

struct LayoutBone {
    int id;           // offset 0x0
    ValVec3 min;      // offset 0x4: travel limits, in metres
    ValVec3 max;      // offset 0x10
    ValVec3 center;   // offset 0x1C
    f32 dmgDist;      // offset 0x28
    f32 weakness;     // offset 0x2C
    u8 welded;        // offset 0x30
};

struct LayoutLink {
    int i;       // offset 0x0
    int j;       // offset 0x4
    f32 linear;  // offset 0x8
    f32 planar;  // offset 0xC
    f32 pop;     // offset 0x10
};

struct LayoutWheel {
    int boneId;  // offset 0x0
    f32 xScale;  // offset 0x4
    f32 yScale;  // offset 0x8
    f32 zScale;  // offset 0xC
};

struct LayoutBounds {
    ValVec3 min;  // offset 0x0
    ValVec3 max;  // offset 0xC
};

struct CarLayout {
    int numBones;               // offset 0x0
    LayoutBone bones[162];      // offset 0x4, indexed from 1
    LayoutLink* links;          // offset 0x20EC
    int numLinks;               // offset 0x20F0
    LayoutWheel wheels[4];      // offset 0x20F4
    ValueTable* physics;        // offset 0x2134
    LayoutBounds bounds[32];    // offset 0x2138
    ValueTable* physicsTable;   // offset 0x2438
    char name[0x20];            // offset 0x243C
    int refs;                   // offset 0x245C
    CarLayout* next;            // offset 0x2460
};

ModuleTag lbl_8055F5E8;
CarLayout* lbl_8055F5F0;  // loaded layouts

static inline void SetInches(ValVec3* dst, const ValVec3& v) {
    dst->x = v.x;
    dst->y = v.y;
    dst->z = v.z;
    dst->x *= 0.0254f;
    dst->y *= 0.0254f;
    dst->z *= 0.0254f;
}

extern "C" {

int fn_80278A1C(void) { return 0; }

void fn_80278A24(void) {}

// Fallback for a bone missing from the table.
void fn_80278A28(LayoutBone* b) {
    f32 r = 10.0f;
    b->max.x = r;
    b->min.x = -r;
    b->min.y = -r;
    b->min.z = -r;
    b->max.y = r;
    b->max.z = r;
    b->center.x = 0.0f;
    b->center.y = 0.0f;
    b->center.z = 0.0f;
    b->weakness = 1.0f;
}

void fn_80278A64(CarLayout* l, ValueTable* boneTable, ValueTable* modTable, const char* car) {
    if (boneTable) {
        l->numBones = 0;
        l->bones[0].id = 0;
        for (int i = 1; i < 162; i++) {
            char key[0x20];
            LayoutBone* b;
            ValueNode* node;
            ValueNode* mod;
            b = &l->bones[i];
            b->id = i;
            sprintf(key, "bone_%d", i);
            if (!boneTable->HasChild(key)) {
                break;
            }
            node = (ValueNode*)boneTable->FindChild(key);
            l->numBones = i + 1;
            mod = node;
            if (modTable && modTable->HasChild(key)) {
                mod = (ValueNode*)modTable->FindChild(key);
            }
            if (node) {
                SetInches(&b->min, mod->GetVec3("travel_min"));
                SetInches(&b->max, mod->GetVec3("travel_max"));
                SetInches(&b->center, mod->GetVec3("link_center"));
                b->welded = node->GetBool("is_welded", 0);
                b->weakness = node->GetFloat("weakness");
                b->dmgDist = node->GetFloat("dmg_dist", 0.2032f);
            } else {
                fn_802E30C8("layout: WARNING: Car \"%s\" using bad bone info for bone %d", car, i);
                fn_80278A28(b);
            }
        }
        l->numLinks = 0;
        ValueNode* child;
        int n = 0;
        while ((child = boneTable->GetChild(n++)) != NULL) {
            if (strcasecmp(child->GetName(), "link") == 0) {
                l->numLinks++;
            }
        }
        l->links = new ("layout: boneconn") LayoutLink[l->numLinks];
        n = 0;
        l->numLinks = n;
        while ((child = boneTable->GetChild(n++)) != NULL) {
            if (strcasecmp(child->GetName(), "link") == 0) {
                LayoutLink* lk = &l->links[l->numLinks++];
                lk->i = child->GetIntValue("i");
                lk->j = child->GetIntValue("j");
                lk->linear = child->GetFloat("linear");
                lk->planar = child->GetFloat("planar");
                lk->pop = child->GetFloat("pop", 0.0f);
                if (lk->i < 1 || lk->i >= 162) {
                    fn_802E317C("layout: link: \"i\" out of range (%d)", lk->i);
                }
                if (lk->j < 1 || lk->j >= 162) {
                    fn_802E317C("layout: link: \"i\" out of range (%d)", lk->j);
                }
            }
        }
        for (int w = 0; w < 4; w++) {
            const char* names[4] = {"lf", "rf", "lr", "rr"};
            char key[0x20];
            sprintf(key, "wheel_%s", names[w]);
            ValueNode* node = boneTable->HasChild(key) ? (ValueNode*)boneTable->FindChild(key) : NULL;
            LayoutWheel* wh = &l->wheels[w];
            memset(wh, 0, sizeof(LayoutWheel));
            if (node) {
                wh->boneId = node->GetIntValue("bone_id");
                wh->xScale = node->GetFloat("x_scale", 0.0f);
                wh->yScale = node->GetFloat("y_scale", 0.0f);
                wh->zScale = node->GetFloat("z_scale", 0.0f);
            }
        }
    } else {
        fn_802E30C8("layout: WARNING: No bone info for car \"%s\"", car);
        memset(l->bones, 0, sizeof(l->bones));
        l->numLinks = 0;
        memset(l->wheels, 0, sizeof(l->wheels));
    }
}

CarLayout* fn_80279004(const char* car, int unused) {
    char path[0x80];
    char modPath[0x80];
    char key[0x20];
    CarLayout* l = new ("layout") CarLayout;
    sprintf(path, "%s.car/physics.val", car);
    l->physicsTable = fn_80131808(path);
    sprintf(path, "%s.car/bone.mvl", car);
    if (!fn_8012C33C(path)) {
        sprintf(path, "%s.car/bone.val", car);
    }
    ValueTable* bones = fn_80131808(path);
    sprintf(modPath, "%s.car/bonemod.bvl", car);
    ValueTable* mods = fn_8012C33C(modPath) ? fn_80131808(modPath) : NULL;
    fn_80278A64(l, bones, mods, car);
    memset(l->bounds, 0, sizeof(l->bounds));
    if (bones->HasChild("bounds")) {
        ValueNode* node = (ValueNode*)bones->FindChild("bounds");
        for (int i = 0; i < 32; i++) {
            sprintf(key, "min_%d", i);
            const ValVec3& mn = node->GetVec3(key);
            l->bounds[i].min.x = mn.x;
            l->bounds[i].min.y = mn.y;
            l->bounds[i].min.z = mn.z;
            sprintf(key, "max_%d", i);
            const ValVec3& mx = node->GetVec3(key);
            l->bounds[i].max.x = mx.x;
            l->bounds[i].max.y = mx.y;
            l->bounds[i].max.z = mx.z;
        }
    } else {
        fn_802E30C8("layout: %s has no bounds table", car);
    }
    fn_80131818(bones);
    if (mods) {
        fn_80131818(mods);
    }
    l->refs = 1;
    strncpy(l->name, car, sizeof(l->name));
    l->name[sizeof(l->name) - 1] = '\0';
    l->physics = l->physicsTable;
    l->next = lbl_8055F5F0;
    lbl_8055F5F0 = l;
    return l;
}

CarLayout* fn_80279254(const char* car, int unused) {
    CarLayout* l;
    for (l = lbl_8055F5F0; l; l = l->next) {
        if (strcasecmp(l->name, car) == 0) {
            l->refs++;
            break;
        }
    }
    if (!l) {
        l = fn_80279004(car, unused);
    }
    return l;
}

CarLayout* fn_802792E4(const char* car) { return fn_80279254(car, 0); }

void fn_802792EC(CarLayout* l) {
    if (--l->refs == 0) {
        CarLayout** pp = &lbl_8055F5F0;
        while (*pp) {
            if (*pp == l) {
                *pp = (*pp)->next;
                break;
            }
            pp = &(*pp)->next;
        }
        fn_80131818(l->physicsTable);
        delete[] l->links;
        delete l;
    }
}

}  // extern "C"

// NonMatching: register numbering in fn_80278A64 and fn_80279004 differs (every function has the
// target's instruction count and shape).
