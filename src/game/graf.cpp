// World display graph (0x801EAFD4-0x801EC1D4): the "GRAF" resource (version 0x15) of group, model and
// special nodes with two display lists, fixed up and endian-swapped on load, the per-type model
// handlers ("Simple Model", "Sky", ...) created from a factory table, the parallax sky model and the
// lightning flash overlay. Names are guesses apart from the strings.
#include "game/common.h"
#include "game/byteswap.h"
#include "game/mathutil.h"

struct GModel {
    s32 kind;    // offset 0x0 (1: typed model)
    s32 handle;  // offset 0x4
    void* res;   // offset 0x8
    s32 unkC;    // offset 0xC
    s32 type;    // offset 0x10
    s32 unk14;   // offset 0x14
    s32 unk18;   // offset 0x18
    s32 unk1C;   // offset 0x1C
    s32 unk20;   // offset 0x20
    s32 unk24;   // offset 0x24
    s32 unk28;   // offset 0x28
};

struct GKid {
    s32 key;            // offset 0x0
    struct GNode* node; // offset 0x4
};

struct GNode {
    s32 type;     // offset 0x0
    GNode* next;  // offset 0x4
    union {
        struct {
            s32 a;      // offset 0x8
            s32 b;      // offset 0xC
            s32 c;      // offset 0x10
            s32 count;  // offset 0x14
            GKid kids[1]; // offset 0x18
        } group;
        struct {
            GNode* child; // offset 0x8
            s32 a;        // offset 0xC
            s32 b;        // offset 0x10
            s32 c;        // offset 0x14
            s32 d;        // offset 0x18
            s32 e;        // offset 0x1C
        } sub;
        GModel* model;    // offset 0x8
    };
};

struct GrafRes {
    GNode* root;  // offset 0x0
    void* list0;  // offset 0x4
    void* list1;  // offset 0x8
};

class GModelHandler {
public:
    virtual ~GModelHandler(void);                                            // 0x8
    virtual void Fixup(GModel* m);                                           // 0xC
    virtual void Init(GModel* m);                                            // 0x10
    virtual void Release(GModel* m);                                         // 0x14
    virtual void Start(void);                                                // 0x18
    virtual void Update(void);                                               // 0x1C
    virtual void Draw(int a);                                                // 0x20
    virtual void vf24(void);
    virtual void vf28(void* a, void* b);                                     // 0x28
    virtual void Attach(GModel* m, void* l0, s32 a, void* l1, s32 b);        // 0x2C
};

class GrafVisitor {
public:
    virtual void Visit(GModel* m, void* res, s32 handle) = 0;  // 0x8
};

struct GrafFactory {
    const char* name;
    GModelHandler* (*create)(void);
};

// GFrame: common.h's PlainValMatrix.
struct GObj {
    u8 pad[0xD8];
    f32 flash;  // offset 0xD8
    u8 padDC[4];
};

// The track-model handler base (trackmod.cpp's TrackMod, the same class as GModelHandler above): graf defines its
// empty default slots in place. Declared here as far as those definitions need, without virtuals: retail graf emits
// no TrackMod table (its copies are the linker-blanked weak tables in drone/fabric). Port: same member names.
struct TObj;
class TrackMod {
public:
    void Swap(TObj* o);
    void Init(TObj* o);
    void Unk14(void);
    void Apply(void);
    void Unk1C(void);
    void Add(void* scene, const TObj* o, s32 id);
    void Link(int a, void* b, int c, void* d, int e);
};

extern "C" {
void* fn_8012C370(const char* name, u32 type, u32* version, u32* size, u8* first, u8* swapped);
void fn_8012C4D8(void* res);
int fn_8012C33C(const char* name);
void mrModelFixup(void* res, const char* name);
s32 fn_802C6DC4(const char* tag, void* res, int a, int b);
void fn_802C6FA8(void* res);
void fn_802C6FB4(void* res);
void fn_802C703C(s32 h);
s32 fn_802C6FC0(const char* name, int a, int b);
void mrModelUnload(s32 model);
int fn_801E2CCC(s32 model, const char* name, f32** out);
void fn_802C744C(s32 model, const PlainValMatrix* m);
void fn_802C8B08(PlainValMatrix* cam);
PlainValMatrix* fn_802C86DC(void);
void HandleFullscreenUnsupported(u32 color);
void fn_802C9900(f32 a);
void fn_802C9904(void);
void fn_802CDDC8(void* l, void* base);
void SortAndInitializeRenderBatches(void* l);
void fn_802CD254(void* l);
void fn_802CD37C(void* l, int a, int b, int c, f32 x, f32 y);
void FlushRenderQueue(int pass);
void fn_802CDAA8(void);
void fn_802C7A44(void);
void fn_802C7A50(void);
void fn_802C7A94(int a);
void fn_802C7AD4(void);
void fn_802C7730(PlainValMatrix* f);
void fn_802C9648(f32 a);
void fn_802C967C(void);
void fn_80202718(void);
void CalculateBillboardFrame(PlainValMatrix* f);
int fn_801EA470(void);
void fn_801FA6B8(void);
void fn_8029790C(int a);
void fn_801F4DCC(void);
void fn_80287A34(int a);
void fn_802879E4(int a);
int fn_801E85A4(void);
int fn_801E85B4(void);
void* fn_801E8718(int i);
f32 fn_801E8738(int i);
void fn_802BF598(void* cam, f32 a);
GObj* fn_801E7E34(void);
void fn_802AE7F4(void);
void fn_802AE9D0(void);
void fn_802AEA44(void);
void fn_802AEAA8(void);
void fn_802B0144(void);
void fn_802B01A8(void);
void fn_802B05D8(void);
double cos(double x);
double sin(double x);
void fn_802E30C8(const char* fmt, ...);
void fn_802E317C(const char* fmt, ...);
}

extern f32 __float_epsilon[];  // FLT_EPSILON

static inline f32 GFloat(f32 x) { return x; }


extern "C" {
GModelHandler* CreateGenericTrackModifier(void);
GModelHandler* CreatePointFacingHandler(void);
GModelHandler* CreateAxisFacingTrackModifier(void);
GModelHandler* CreateSkyTrackModifier(void);
GModelHandler* CreateMultiFacingTrackModifier(void);
GModelHandler* CreateRotatorTrackModifier(void);
GModelHandler* fn_801F1EA8(void);
GModelHandler* CreatePinkTriangleManager(void);
GModelHandler* CreateAmbientColorModifier(void);
GModelHandler* CreateShadowColorTrackModifier(void);
GModelHandler* CreateSunIntensityHandler(void);
GModelHandler* CreateFogColorHandler(void);
GModelHandler* CreateOptTerrain(void);
GModelHandler* fn_801FC914(void);
GModelHandler* fn_801FC94C(void);
GModelHandler* CreateXfaderTrackModifier(void);
GModelHandler* CreateFlareSource(void);
GModelHandler* CreateWaterTrackModifier(void);
GModelHandler* fn_801FC994(void);
GModelHandler* fn_801FC9CC(void);
GModelHandler* fn_801F78F0(void);
GModelHandler* CreateFacingTestTrackModifier(void);
GModelHandler* CreateScrollingTrackModifier(void);
GModelHandler* fn_801EB0E8(void);
}

const GrafFactory lbl_802F9A08[25] = {
    {"Simple Model", CreateGenericTrackModifier},     {"Point-Facing", CreatePointFacingHandler}, {"Axis-Facing", CreateAxisFacingTrackModifier},
    {"Sky", CreateSkyTrackModifier},              {"Multi-Facing", CreateMultiFacingTrackModifier}, {"Rotator", CreateRotatorTrackModifier},
    {"Sun", fn_801F1EA8},              {"Animation", CreatePinkTriangleManager},    {"Unknown", CreatePinkTriangleManager},
    {"Ambient", CreateAmbientColorModifier},          {"Shadow", CreateShadowColorTrackModifier},       {"SunBright", CreateSunIntensityHandler},
    {"FogColor", CreateFogColorHandler},         {"Optional Terrain", CreateOptTerrain}, {"Linear Path", fn_801FC914},
    {"Linear Drone", fn_801FC94C},     {"Cross Fader", CreateXfaderTrackModifier},  {"Flare Source", CreateFlareSource},
    {"Water", CreateWaterTrackModifier},            {"Hermite path", fn_801FC994}, {"Hermite drone", fn_801FC9CC},
    {"Fabric", fn_801F78F0},           {"Facing Test", CreateFacingTestTrackModifier},  {"Scrolling", CreateScrollingTrackModifier},
    {"Sanity", fn_801EB0E8},
};

s32 lbl_8055AFB8 = -1;  // psky.mod
s32 lbl_8055AFBC = -1;  // litening.mod

ModuleTag lbl_8055EB88;
u32 lbl_8055EB90 = ARGB(0xFF, 0xFF, 0, 0);
extern const f32 lbl_8055EB94 = GFloat(0.7853982f);
f32 lbl_8055EB98[2];   // display list fog/lod parameters
GrafRes* lbl_8055EBA0;
f32 lbl_8055EBA4;      // parallax scale

GModelHandler* lbl_804885E8[24];

class GrafReleaser : public GrafVisitor {
public:
    void Visit(GModel* m, void* res, s32 handle);
};

extern "C" {

void fixup_display_list(GNode* n, u32 base);
void fn_801EB634(void* p, u32 base);
void fn_801EB66C(void* p, u32 base);
void init_display_list(GNode* n);
void fn_801EB96C(GModelHandler** slot, GModelHandler* (*create)(void));
void fn_801EB9A4(GModelHandler** slot);
void fn_801EBB7C(u32* p, u32 base);

s32 fn_801EAFD4(GNode* n) {
    if (n->type == 0x103) {
        return n->sub.e;
    }
    if (n->type == 0x101) {
        return n->sub.a;
    }
    return 0;
}

void fn_801EB000(GNode* root, GrafVisitor* v) {
    GNode* n = root;
    while (n) {
        if (n->type & 0x100) {
            fn_801EAFD4(n);
            fn_801EB000(n->sub.child, v);
        } else {
            switch (n->type) {
            case 0:
                for (int i = 0; i < n->group.count; i++) {
                    fn_801EB000(n->group.kids[i].node, v);
                }
                break;
            case 4:
                if (n->model) {
                    v->Visit(n->model, n->model->res, n->model->handle);
                }
                break;
            }
        }
        n = n->next;
    }
}

GModelHandler* fn_801EB0E8(void) { return 0; }

int ValidateDisplayType(int type) { return type >= 0 && type < 24; }

GModelHandler* GetModelHandler(GModel* m) { return lbl_804885E8[m->type]; }

void fn_801EB128(void) {
    lbl_8055EB98[0] = 1.0f;
    lbl_8055EB98[1] = 1.0f;
}

#pragma push
#pragma opt_propagation off  /* Proven by the pragma gate (aids audit 2026-11-16): FixupModelResources exact with it, 74.30 without */
void FixupModelResources(GModel* m) {
    SwapWordV(&m->handle);
    SwapWordV(&m->res);
    SwapWordV(&m->unkC);
    m->res = (u8*)m + (u32)m->res;
    mrModelFixup(m->res, "simple_model");
    if (m->kind == 1) {
        SwapWordV(&m->type);
        SwapWordV(&m->unk14);
        SwapWordV(&m->unk18);
        SwapWordV(&m->unk1C);
        for (int i = 0; i < 3; ++i) {
            SwapWordV((u8*)m + 32 + 4 * i);
        }
    }
}
#pragma pop

void fixup_display_list(GNode* root, u32 base) {
    GNode* n = root;
    while (n) {
        SwapWordV(&n->type);
        fn_801EB634(&n->next, base);
        switch (n->type) {
        case 0:
            SwapWordV(&n->group.a);
            SwapWordV(&n->group.b);
            SwapWordV(&n->group.c);
            SwapWordV(&n->group.count);
            for (int i = 0; i < n->group.count; i++) {
                SwapWordV(&n->group.kids[i].key);
                fn_801EB634(&n->group.kids[i].node, base);
                fixup_display_list(n->group.kids[i].node, base);
            }
            break;
        case 0x101:
            SwapWordV(&n->sub.a);
            fn_801EB634(&n->sub.child, base);
            fixup_display_list(n->sub.child, base);
            break;
        case 0x102:
        case 0x103:
            SwapWordV(&n->sub.a);
            SwapWordV(&n->sub.b);
            SwapWordV(&n->sub.c);
            SwapWordV(&n->sub.d);
            fn_801EB634(&n->sub.child, base);
            fixup_display_list(n->sub.child, base);
            if (n->type == 0x103) {
                SwapWordV(&n->sub.e);
            }
            break;
        case 4: {
            fn_801EB66C(&n->model, base);
            SwapWordV(&n->model->kind);
            FixupModelResources(n->model);
            if (*(s32*)n->model->res <= 0) {
                n->model = 0;
            }
            GModel* m = n->model;
            if (m && m->kind == 1) {
                if (ValidateDisplayType(m->type)) {
                    GetModelHandler(m)->Fixup(m);
                } else {
                    fn_802E30C8("graf: unrecognized model type %d", m->type);
                    m->type = 8;
                }
            }
            break;
        }
        default:
            fn_802E317C("fixup_display_list: unrecognized DisplayNodeType %d", n->type);
            break;
        }
        n = n->next;
    }
}

void fn_801EB634(void* p, u32 base) {
    SwapWordV(p);
    if (*(u32*)p) {
        *(u32*)p += base;
    }
}

void fn_801EB66C(void* p, u32 base) {
    SwapWordV(p);
    if (*(u32*)p) {
        *(u32*)p += base;
    }
}

}  // extern "C"

void TrackMod::Swap(TObj* o) {}

extern "C" {

void init_display_list(GNode* root) {
    GNode* n = root;
    while (n) {
        switch (n->type) {
        case 0:
            for (int i = 0; i < n->group.count; i++) {
                init_display_list(n->group.kids[i].node);
            }
            break;
        case 0x101:
        case 0x102:
        case 0x103:
            fn_801EAFD4(n);
            init_display_list(n->sub.child);
            break;
        case 4: {
            GModel* m = n->model;
            if (m) {
                m->handle = fn_802C6DC4("graf", m->res, 0, 0);
                fn_802C6FA8(m->res);
                if (m->kind == 1) {
                    GetModelHandler(m)->Init(m);
                    GrafRes* g = lbl_8055EBA0;
                    GetModelHandler(m)->Attach(m, g->list0, m->unk1C, g->list1, m->unk18);
                }
            }
            break;
        }
        default:
            fn_802E317C("init_display_list: unexpected display node type %d", n->type);
            break;
        }
        n = n->next;
    }
}

}  // extern "C"

void TrackMod::Link(int a, void* b, int c, void* d, int e) { ((GModelHandler*)this)->vf28(b, (void*)a); }

void TrackMod::Add(void* scene, const TObj* o, s32 id) {}

void TrackMod::Init(TObj* o) {}

extern "C" {

void fn_801EB824(GNode* root) {
    GrafReleaser v;
    fn_801EB000(root, &v);
}

void fn_801EB854(void) {
    for (u32 i = 0; i < 24; i++) {
        fn_801EB96C(&lbl_804885E8[i], lbl_802F9A08[i].create);
    }
}

void fn_801EB8BC(void) {
    for (u32 i = 0; i < 24; i++) {
        fn_801EB9A4(&lbl_804885E8[i]);
    }
}

void StartAllSprites(void) {
    for (u32 i = 0; i < 24; i++) {
        lbl_804885E8[i]->Start();
    }
}

}  // extern "C"

void TrackMod::Apply(void) {}

extern "C" {

void fn_801EB96C(GModelHandler** slot, GModelHandler* (*create)(void)) { *slot = create(); }

void fn_801EB9A4(GModelHandler** slot) {
    if (*slot) {
        delete *slot;
    }
    *slot = 0;
}

int LoadGrafResource(const char* name) {
    u8 swapped;
    u8 first;
    u32 size;
    u32 ver;
    lbl_8055EBA0 = (GrafRes*)fn_8012C370(name, 'GRAF', &ver, &size, &first, &swapped);
    if (lbl_8055EBA0) {
        if (ver == 0x15) {
            fn_801EB128();
            fn_801EB854();
            if (swapped) {
                fn_801EB634(lbl_8055EBA0, (u32)lbl_8055EBA0);
                fixup_display_list(lbl_8055EBA0->root, (u32)lbl_8055EBA0);
                fn_801EBB7C((u32*)&lbl_8055EBA0->list0, (u32)lbl_8055EBA0);
                fn_801EBB7C((u32*)&lbl_8055EBA0->list1, (u32)lbl_8055EBA0);
                fn_802CDDC8(lbl_8055EBA0->list0, lbl_8055EBA0->list0);
                fn_802CDDC8(lbl_8055EBA0->list1, lbl_8055EBA0->list1);
            }
            init_display_list(lbl_8055EBA0->root);
            StartAllSprites();
        } else {
            fn_802E317C("bad graf version %d", ver, 0x15);
        }
    } else {
        fn_802AE7F4();
    }
    lbl_8055EBA4 = 0.5f;
    const char* sky = "psky.mod";
    if (fn_8012C33C(sky)) {
        f32* p;
        s32 m = lbl_8055AFB8 = fn_802C6FC0(sky, 0, 0);
        p = 0;
        if (fn_801E2CCC(m, "parallax_scale", &p)) {
            lbl_8055EBA4 = *p;
        }
    } else {
        lbl_8055AFB8 = -1;
    }
    lbl_8055AFBC = fn_802C6FC0("litening.mod", 0, 0);
    return lbl_8055EBA0 != 0;
}

void fn_801EBB7C(u32* p, u32 base) {
    SwapWordV(p);
    if (*p) {
        *p += base;
    }
}

void fn_801EBBB4(void) {
    mrModelUnload(lbl_8055AFBC);
    lbl_8055AFBC = -1;
    if (lbl_8055AFB8 != -1) {
        mrModelUnload(lbl_8055AFB8);
        lbl_8055AFB8 = -1;
    }
    if (lbl_8055EBA0) {
        fn_801EB824(lbl_8055EBA0->root);
        fn_801EB8BC();
        fn_8012C4D8(lbl_8055EBA0);
        lbl_8055EBA0 = 0;
    } else {
        fn_802AE9D0();
    }
}

void fn_801EBC2C(void) {
    if (lbl_8055EBA0) {
        SortAndInitializeRenderBatches(lbl_8055EBA0->list0);
        SortAndInitializeRenderBatches(lbl_8055EBA0->list1);
    } else {
        fn_802AEA44();
    }
}

void fn_801EBC70(void) {
    if (lbl_8055EBA0) {
        fn_802CD254(lbl_8055EBA0->list0);
        fn_802CD254(lbl_8055EBA0->list1);
    }
}

void fn_801EBCAC(void) {
    if (lbl_8055EBA0) {
        for (u32 i = 0; i < 24; i++) {
            lbl_804885E8[i]->Update();
        }
    }
}

}  // extern "C"

void TrackMod::Unk1C(void) {}

extern "C" {

void fn_801EBD18(int view) {
    if (fn_801E85A4() > 1) {
        fn_802BF598(fn_801E8718(view), fn_801E8738(view));
    }
}

void fn_801EBD70(void) {
    if (!lbl_8055EBA0) {
        fn_802B0144();
        if (fn_801E85A4() == 1) {
            fn_802BF598(fn_801E8718(-1), fn_801E8738(-1));
        }
    }
}

void fn_801EBDCC(void) {
    if (lbl_8055EBA0) {
        PlainValMatrix f;
        fn_802C7A44();
        fn_80202718();
        CalculateBillboardFrame(&f);
        fn_802C7730(&f);
        int alt = fn_801EA470();
        void* list;
        if (alt) {
            list = lbl_8055EBA0->list1;
        } else {
            list = lbl_8055EBA0->list0;
        }
        fn_802CD37C(list, 0, 0, 0, lbl_8055EB98[0], lbl_8055EB98[1]);
        FlushRenderQueue(0);
        fn_802C7A94(1);
        if (!alt) {
            fn_802C9648(1.1f);
            FlushRenderQueue(1);
            fn_802C7A94(2);
            fn_802C7A94(3);
            fn_80287A34(0);
            fn_802C967C();
            FlushRenderQueue(2);
            fn_802879E4(0);
            fn_802C7A94(4);
        }
        fn_801FA6B8();
        fn_802C7730(&f);
        FlushRenderQueue(3);
        fn_802C7A50();
        if (!alt) {
            fn_8029790C(0);
            fn_801F4DCC();
            FlushRenderQueue(4);
        }
        fn_802C7AD4();
        fn_802CDAA8();
    } else {
        fn_802B01A8();
    }
}

void fn_801EBEF4(f32 v) { lbl_8055EB98[0] = v * v; }


void fn_801EBF00(void) {
    if (lbl_8055EBA0) {
        lbl_804885E8[3]->Draw(0);
        if (lbl_8055AFB8 != -1) {
            PlainValMatrix cam;
            fn_802C8B08(&cam);
            f32 s = 1.0f - lbl_8055EBA4;
            cam.origin.x *= s;
            cam.origin.y *= s;
            cam.origin.z *= s;
            cam.basis[0].x = 1.0f;
            cam.basis[0].y = 0.0f;
            cam.basis[0].z = 0.0f;
            cam.basis[1].x = 0.0f;
            cam.basis[1].y = 1.0f;
            cam.basis[1].z = 0.0f;
            cam.basis[2].x = 0.0f;
            cam.basis[2].y = 0.0f;
            cam.basis[2].z = 1.0f;
            fn_802C744C(lbl_8055AFB8, &cam);
        }
        lbl_804885E8[6]->Draw(0);
        GObj* o = &fn_801E7E34()[fn_801E85B4()];
        if (!(FAbs(o->flash) <= __float_epsilon[0])) {
            int a = (int)(255.0f * o->flash);
            a = a > 0 ? a : 0;
            int c = 0xFF;
            if (a < 0xFF) {
                c = a;
            }
            HandleFullscreenUnsupported(ARGB(0xFF, c, c, c));
            PlainValMatrix* cf = fn_802C86DC();
            PlainValMatrix m;
            m.origin.x = cf->origin.x;
            m.origin.y = cf->origin.y;
            m.origin.z = cf->origin.z;
            f32 ang = *(f32*)(o + 1);
            f32 co = cos(ang);
            f32 si = sin(ang);
            m.basis[0].x = co;
            m.basis[0].y = 0.0f;
            m.basis[0].z = -si;
            m.basis[1].x = 0.0f;
            m.basis[1].y = 1.0f;
            m.basis[1].z = 0.0f;
            m.basis[2].x = si;
            m.basis[2].y = 0.0f;
            m.basis[2].z = co;
            f32 k = 2.0f * o->flash;
            fn_802C9900(1.0f < k ? 1.0f : k);
            fn_802C744C(lbl_8055AFBC, &m);
            fn_802C9904();
        }
    } else {
        fn_802B05D8();
    }
}

f32 fn_801EC114(void) { return lbl_8055EBA4; }

void fn_801EC11C(void) {
    if (!lbl_8055EBA0) {
        fn_802AEAA8();
    }
}

}  // extern "C"

void GrafReleaser::Visit(GModel* m, void* res, s32 handle) {
    if (m->handle != -1) {
        fn_802C6FB4(res);
        fn_802C703C(m->handle);
    }
    if (m->kind == 1) {
        GetModelHandler(m)->Release(m);
    }
}

void TrackMod::Unk14(void) {}

