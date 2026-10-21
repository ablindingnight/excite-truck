// Car effects manager (0x8028777C-0x802888D8): drives the per-car effect subsystems (init,
// reset, update, draw), keeps a per-frame snapshot of every car's state and a distance-sorted
// car list for the effect callbacks, reads each car's efx.val, and owns the effects random
// number generator. Names are guesses apart from the allocation tags and messages.
#include "game/common.h"
#include "macros.h"
#include <revolution/MTX.h>

class ValueNodeV {
public:
    virtual const char* GetName(void);
    virtual int Has(const char* key);
    virtual int GetPair(int index, const char** key, const char** value);
    virtual const char* GetValue(const char* key);
    virtual const char* GetString(const char* key, const char* def);
    virtual s32 GetIntValue(const char* key);
    virtual s32 GetInt(const char* key, s32 def);
    virtual f32 GetFloat(const char* key);
    virtual f32 GetFloat(const char* key, f32 def);
    virtual s32 GetIntValue2(const char* key);
    virtual int GetBool(const char* key, int def);
    virtual ValVec3 GetVec3(const char* key);
    virtual ValVec3 GetVec3Or(const char* key, ValVec3 def);
};

class EfxTable {
public:
    virtual ValueNodeV* Find(const char* name);
    virtual void Unk0C(void);
    virtual int Has(const char* name);
};

// One car's effect description: its efx.val table and exhaust nodes.
struct EfxInfo {
    EfxInfo(void);
    ~EfxInfo(void);
    EfxTable* node;   // offset 0x0
    void* ex[8];      // offset 0x4
    s32 n;            // offset 0x24
};

struct EfxWheel {
    Vec pos;    // offset 0x0
    Vec v0C;    // offset 0xC
    Vec v18;    // offset 0x18
    f32 f24;    // offset 0x24
    f32 f28;    // offset 0x28
    s32 i2C;    // offset 0x2C
    u8 b30;     // offset 0x30
    f32 f34;    // offset 0x34
    s32 i38;    // offset 0x38
};

struct EfxMtx {
    Vec rot[3];  // offset 0x0
    Vec pos;     // offset 0x24
};

// Per-frame snapshot of a car for the effects.
struct EfxCar {
    EfxCar(void);
    EfxMtx m;        // offset 0x0
    s16 hint;        // offset 0x30
    Vec vel;         // offset 0x34
    Vec dir;         // offset 0x40
    f32 height;      // offset 0x4C
    f32 speed;       // offset 0x50
    f32 f54;         // offset 0x54
    f32 boost;       // offset 0x58
    s32 state;       // offset 0x5C
    s32 i60;         // offset 0x60
    s32 i64;         // offset 0x64
    u32 flags;       // offset 0x68
    s32 i6C;         // offset 0x6C
    EfxWheel wheels[4];  // offset 0x70
};

struct EfxEntry {
    EfxCar* car;  // offset 0x0
    s32 index;    // offset 0x4
    f32 dist;     // offset 0x8
    u8 active;    // offset 0xC
};

typedef void (*EfxCallback)(EfxEntry* e, int n);

struct EfxGlobals {
    EfxCar* cars;  // offset 0x0
    s32 count;     // offset 0x4
    u8 flag;       // offset 0x8
};

struct PhysWheel {
    Vec pos;   // offset 0x0
    Vec v0C;   // offset 0xC
    Vec v18;   // offset 0x18
    u8 pad24[0x30 - 0x24];
    f32 f30;   // offset 0x30
    u8 pad34[0x38 - 0x34];
    f32 f38;   // offset 0x38
    u8 pad3C[0x60 - 0x3C];
    s32 i60;   // offset 0x60
    u8 b64;    // offset 0x64
    s32 i68;   // offset 0x68
};

struct PhysCar {
    u8 pad0[0x30];
    s16 hint;  // offset 0x30
    u8 pad32[0x38 - 0x32];
    f32 f38;   // offset 0x38
    u8 pad3C[0x64 - 0x3C];
    f32 f64;   // offset 0x64
    u8 pad68[0x6E - 0x68];
    u16 bits;  // offset 0x6E
    Vec vel;   // offset 0x70
    PhysWheel wheels[4];  // offset 0x7C
    u8 pad22C[0x28C - 0x22C];
    f32 f28C;  // offset 0x28C
    u8 pad290[0x298 - 0x290];
    s32 i298;  // offset 0x298
    s32 i29C;  // offset 0x29C
    u8 pad2A0[0x2D4 - 0x2A0];
    f32 f2D4;  // offset 0x2D4
    s32 i2D8;  // offset 0x2D8
};

struct EfxDriver {
    u8 b0;       // offset 0x0
    u8 pad1[7];
    PhysCar* phys;  // offset 0x8
};

struct EfxModel {
    u8 pad0[4];
    EfxMtx m;    // offset 0x4
    u8 pad34[0x3EC - 0x34];
    s32 i3EC;    // offset 0x3EC
};

struct CarDef {
    u8 pad0[0x88];
    char name[0x30];  // offset 0x88
    u32 flags;        // offset 0xB8
};

struct EfxShort {
    EfxShort(s16 v) { x = v; }
    s16 x;
};

struct EfxFloat {
    EfxFloat(f32 v) { x = v; }
    f32 x;
};

class Viewer {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(void);
    virtual void v1C(void);
    virtual void v20(void);
    virtual void v24(void);
    virtual void v28(void);
    virtual void v2C(void);
    virtual void v30(void);
    virtual void v34(void);
    virtual void v38(void);
    virtual void v3C(void);
    virtual void v40(void);
    virtual void v44(void);
    virtual void Draw(void);
};

extern "C" {
Viewer* fn_801B83A0(void);
void fn_801B8820(void);
void fn_801B8834(void);
void fn_80161AEC(void);
int fn_8012C33C(const char* name);
EfxTable* fn_80131808(const char* name);
void fn_80131818(EfxTable* t);
void fn_801346B4(char* buf, int size, const char* s);
void fn_801E12A4(void);
int fn_801E2CCC(s32 model, const char* name, void** out);
int fn_801E7DF0(int a);
f32 fn_801E7E24(void);
int fn_801E85A4(void);
const EfxMtx* fn_801E8718(int view);
int fn_801E866C(int view);
f32 fn_801E88F0(int view);
int fn_801E8954(int view);
int fn_801EA994(void);
CarDef* fn_801EA9A0(int i);
EfxDriver* fn_801EA9CC(int i);
s32 fn_801EE0C8(void* model);
f32 fn_801EE0D0(void* model, int wheel);
void fn_802E317C(const char* fmt, ...);
int sprintf(char* buf, const char* fmt, ...);
void* memset(void* dst, int c, size_t n);
void qsort(void* base, size_t n, size_t size, int (*cmp)(const void*, const void*));
double sqrt(double x);

void fn_8028A664(void);
void fn_8028A784(void);
void fn_8028A89C(void);
void fn_8028A92C(void);
void fn_8028A9BC(int mode);
void fn_8028A9F4(void);
void fn_8028CED8(void);
void fn_8028CF18(void);
void fn_8028CF50(void);
void fn_8028CF64(void);
void fn_8028CFB0(void);
void fn_8028DF58(void);
void fn_8028DFB0(void);
void fn_8028E0F0(void);
void fn_8028E138(void);
void fn_8028E1A4(void);
void fn_8028E1D0(void);
void fn_8028E240(int mode);
void fn_802888D8(void);
void fn_80288988(void);
void fn_80288A1C(int mode);
void fn_80288A20(void);
void fn_80292114(void);
void fn_802921F4(void);
void fn_8029222C(void);
void fn_802924E8(void);
void fn_802924F4(void);
void fn_80292618(void);
void fn_8029261C(void);
void fn_80292628(void);
void fn_80292698(void);
void fn_80292864(void);
void fn_80294970(void);
void fn_80294C68(void);
void fn_80294CC0(void);
void fn_80294CC4(void);
void fn_80294CC8(int mode);
void fn_8029605C(void);
void fn_80296060(void);
void fn_80296064(void);
void fn_802978B8(void* a);
void fn_802978E4(void);
void fn_80297980(void);
void fn_80297ED4(void);
void fn_80297ED8(void);
void fn_80297FA4(void);
void fn_802980A0(void);
void fn_80298124(int mode);
void fn_80298160(void);
void fn_8029C1C4(void);
void fn_8029C22C(void);
void fn_8029C25C(void);
void fn_8029C2D4(void);
void fn_8029EF70(void);
void fn_8029EFBC(void);
void fn_8029F03C(void);
void fn_8029F0D0(void);
void fn_8029F124(void);
void fn_8029F188(void);
void fn_8029F300(void);
void fn_802A09B0(void);
void fn_802A0AFC(void);
void fn_802A0B50(void);
void fn_802A0B54(void);
void fn_802A1290(void);
void fn_802A14F8(void);
void fn_802A1548(void);
void fn_802A1584(void);
void fn_802A1590(void);
void fn_802A1660(void);
void fn_802A172C(void);
void fn_802A1794(void);
void fn_802A17FC(int mode);
void fn_802A4ADC(void);
void fn_802A4B90(void);
void fn_802A4C20(void);
void fn_802A4C60(void);
void fn_802A7BE8(void);
void fn_802A7C24(void);
void fn_802A7C64(void);
void fn_802A7C68(void);
void fn_802A81F0(void);
void fn_802A85C8(void);
void fn_802A86A4(void);
void fn_802A8978(void);
void fn_802A8DE8(void);
void fn_802A9574(void);
void fn_802AB21C(void);
void fn_802AB504(void);
void fn_802AB5B8(void);
void fn_802AB628(f32 dt);
void fn_802AB6D0(void);
void fn_802AB6D4(void);
void fn_802AC458(void);
void fn_802AC49C(void);
void fn_802AE374(void);
void fn_802AE394(void);
}

void* operator new[](size_t size, const char* name);

extern void* lbl_80488648[];  // car models
extern f32 lbl_80558DC4[];    // FLT_EPSILON

static inline void Scale(Vec* out, const Vec* in, f32 s) {
    out->x = in->x * s;
    out->y = in->y * s;
    out->z = in->z * s;
}

ModuleTag lbl_8055F6A8;
EfxShort lbl_8055F6B0(0x100);
EfxShort lbl_8055F6B2(0x405);
EfxShort lbl_8055F6B4(0x401);
EfxFloat lbl_8055F6B8(0.7853982f);
u8 lbl_8055F6BC;
s32 lbl_8055F6C0;      // callback count
EfxInfo* lbl_8055F6C4;
u32 lbl_8055F6C8;      // random seed
EfxGlobals lbl_804B3150;
EfxEntry lbl_804B3160[8];
EfxCallback lbl_804B31E0[16];

extern "C" {

void fn_8028777C(void* a) {}

void fn_80287780(void* a) {
    lbl_8055F6C0 = 0;
    fn_802AE374();
    fn_8028DF58();
    fn_802924E8();
    fn_802978B8(a);
    fn_80297ED4();
    fn_8029605C();
    fn_802AC458();
}

void fn_802877D0(void) {
    fn_802AC49C();
    fn_802978E4();
    fn_8029261C();
    fn_8028E0F0();
    fn_802AE394();
}

void fn_80288218(void* a);
void fn_802880F4(void);
void fn_80288868(u32 seed);

void fn_80287800(void* a) {
    fn_8028777C(a);
    fn_80288218(a);
    fn_802880F4();
    fn_802A81F0();
    fn_802888D8();
    fn_802A1590();
    fn_8028A664();
    fn_8028CED8();
    fn_80294970();
    fn_80297ED8();
    fn_8029C1C4();
    fn_8029EF70();
    fn_802924F4();
    fn_802A09B0();
    fn_802A1290();
    fn_802A4ADC();
    fn_80292114();
    fn_802A7BE8();
    fn_8029F03C();
    fn_802AB21C();
    fn_8028DFB0();
    lbl_8055F6BC = fn_801E7DF0(0) > 1;
    fn_80288868(0);
}

void fn_802881D4(void);
void fn_802882A0(void);

void fn_802878A8(void) {
    fn_802AB504();
    fn_8029F0D0();
    fn_802A7C24();
    fn_802921F4();
    fn_802A4B90();
    fn_802A14F8();
    fn_802A0AFC();
    fn_80292618();
    fn_8029EFBC();
    fn_8029C22C();
    fn_80296064();
    fn_80297FA4();
    fn_80294C68();
    fn_8028CF18();
    fn_8028A784();
    fn_802A1660();
    fn_80288988();
    fn_802A85C8();
    fn_802881D4();
    fn_802882A0();
}

void fn_80287914(void) {
    fn_802A86A4();
    fn_8028E138();
    fn_802980A0();
    fn_8028A89C();
    fn_8028CF50();
    fn_8029222C();
    fn_80294CC0();
    fn_80296060();
    fn_8029F124();
    fn_80292628();
    fn_802A172C();
    fn_802A4C20();
    fn_802A7C64();
    fn_802AB5B8();
    fn_802A9574();
    fn_80288868(0);
}

void fn_80287974(void) { fn_8028E1A4(); }

void fn_80287978(void) { fn_8028E1D0(); }

void fn_802882EC(void);
void fn_802885F0(void);
void fn_802887EC(void);
void fn_802885C0(void);

void fn_8028797C(void) {
    fn_802882EC();
    fn_802A8978();
    fn_80298160();
    fn_80288A20();
    fn_802A1794();
    fn_8028A92C();
    fn_8028CF64();
    fn_80297980();
    fn_80294CC4();
    fn_8029C25C();
    fn_8029F188();
    fn_80292698();
    fn_802A0B50();
    fn_802A4C60();
    fn_802AB628(fn_801E7E24());
    fn_802885F0();
    fn_802887EC();
    fn_802885C0();
}

void fn_802879E4(int mode) {
    if (mode == 1) {
        if (lbl_8055F6BC) {
            fn_8028CFB0();
        }
    } else {
        fn_8028CFB0();
        fn_802A0B54();
        fn_801B8820();
        fn_80161AEC();
        fn_802A8DE8();
        fn_802AB6D0();
    }
}

void fn_80287A34(void) {
    fn_801E12A4();
    fn_802A1548();
    fn_801B83A0()->Draw();
}

void fn_80287A6C(int mode) {
    if (mode != 1 || lbl_8055F6BC) {
        fn_80294CC8(mode);
        fn_801B8834();
        fn_802A1584();
        fn_8029C2D4();
        fn_8028A9BC(mode);
        fn_8028E240(mode);
        fn_80288A1C(mode);
        fn_802A17FC(mode);
        fn_80298124(mode);
        fn_8029F300();
        fn_802A7C68();
        fn_802AB6D4();
    }
}

void fn_80287AF0(void) {
    fn_80292864();
    fn_8028A9F4();
}

s32 fn_80287B14(void) { return lbl_804B3150.count; }

EfxCar* fn_80287B24(s32 i) { return &lbl_804B3150.cars[i]; }

void fn_80287B38(EfxCallback cb) {
    int n = lbl_8055F6C0;
    lbl_804B31E0[n] = cb;
    lbl_8055F6C0 = n + 1;
}

u32 fn_80288870(void);

u32 fn_80287B58(void) { return fn_80288870(); }

u32 fn_80287B5C(u32 n) { return fn_80287B58() % n; }

int fn_80287B94(int lo, int hi) { return lo + fn_80287B5C(hi - lo); }

f32 fn_80287BC8(f32 lo, f32 hi) {
    u32 r = fn_80288870() & 0x1FFFF;
    f32 d = hi - lo;
    return lo + d * (r / 131071.0f);
}

}  // extern "C"

EfxInfo::EfxInfo(void) {
    node = 0;
    memset(ex, 0, sizeof(ex));
    n = 0;
}

EfxInfo::~EfxInfo(void) {
    if (node) {
        fn_80131818(node);
        node = 0;
    }
}

extern "C" {

void fn_80287D70(EfxInfo* e, s32 model, u32 flags);

void fn_80287CE8(EfxInfo* e, const char* car, s32 model, u32 flags) {
    char buf[64];
    sprintf(buf, "%s.car/efx.val", car);
    if (fn_8012C33C(buf)) {
        e->node = fn_80131808(buf);
    }
    fn_80287D70(e, model, flags);
}

void fn_80287D70(EfxInfo* e, s32 model, u32 flags) {
    const char* fmt = (flags & 0x100000) ? "exhaust%d_20" : "exhaust%d";
    char buf[32];
    for (e->n = 0; e->n < 8; e->n++) {
        sprintf(buf, fmt, e->n + 1);
        if (!fn_801E2CCC(model, buf, &e->ex[e->n])) {
            break;
        }
    }
}

int fn_80287E20(EfxInfo* e, const char* key) {
    char buf[64];
    fn_801346B4(buf, 64, key);
    if (e->node) {
        return e->node->Has(buf);
    }
    return 0;
}

ValueNodeV* fn_80287E80(EfxInfo* e, const char* key) {
    char buf[64];
    fn_801346B4(buf, 64, key);
    return e->node->Find(buf);
}

ValVec3 fn_80287ED0(EfxInfo* e, const char* node, const char* key) {
    if (!e->node || !e->node->Find(node)) {
        fn_802E317C("efx: this model doesn't support '%s'", node);
    }
    return e->node->Find(node)->GetVec3(key);
}

ValVec3 fn_80287F90(EfxInfo* e, const char* node, const char* key, ValVec3 def) {
    if (!e->node || !e->node->Find(node)) {
        fn_802E317C("efx: this model doesn't support '%s'", node);
    }
    return e->node->Find(node)->GetVec3Or(key, def);
}

void* fn_80288060(EfxInfo* e, int i) { return e->ex[i]; }

int fn_80288070(EfxInfo* e, const char* name, void** out) {
    u32 idx = e - lbl_8055F6C4;
    return fn_801E2CCC(fn_801EE0C8(idx < 8 ? lbl_80488648[idx] : 0), name, out);
}

void fn_802880F4(void) {
    int n = fn_801EA994();
    lbl_8055F6C4 = new ("efx car tab") EfxInfo[n];
    for (int i = 0; i < fn_801EA994(); i++) {
        CarDef* d = fn_801EA9A0(i);
        s32 model = fn_801EE0C8((u32)i < 8 ? lbl_80488648[i] : 0);
        fn_80287CE8(&lbl_8055F6C4[i], d->name, model, d->flags);
    }
}

void fn_802881D4(void) {
    delete[] lbl_8055F6C4;
    lbl_8055F6C4 = 0;
}

EfxInfo* fn_80288208(s32 i) { return &lbl_8055F6C4[i]; }

void fn_80288218(void* a) {
    lbl_804B3150.cars = new ("EfxCarInfo tab") EfxCar[fn_801EA994()];
    lbl_804B3150.count = fn_801EA994();
    lbl_804B3150.flag = 0;
    memset(lbl_804B3150.cars, 0, lbl_804B3150.count * sizeof(EfxCar));
}

}  // extern "C"

EfxCar::EfxCar(void) {}

extern "C" {

void fn_802882A0(void) {
    delete[] lbl_804B3150.cars;
    lbl_804B3150.cars = 0;
    lbl_804B3150.count = 0;
    lbl_804B3150.flag = 0;
}

void fn_802882EC(void) {
    EfxCar* c = lbl_804B3150.cars;
    lbl_804B3150.flag = 0;
    for (int i = 0; i < lbl_804B3150.count; c++, i++) {
        EfxDriver* d = fn_801EA9CC(i);
        PhysCar* phys = d->phys;
        EfxModel* model = (EfxModel*)((u32)i < 8 ? lbl_80488648[i] : 0);
        c->m = model->m;
        c->hint = phys->hint;
        c->vel.x = phys->vel.x;
        c->vel.y = phys->vel.y;
        c->vel.z = phys->vel.z;
        if (!((f32)__fabs(PSVECDotProduct(&phys->vel, &phys->vel)) <= lbl_80558DC4[0])) {
            Scale(&c->dir, &phys->vel, 1.0f / (f32)sqrt(PSVECDotProduct(&phys->vel, &phys->vel)));
        } else {
            c->dir.x = 0.0f;
            c->dir.y = 0.0f;
            c->dir.z = 0.0f;
        }
        c->height = phys->f2D4;
        c->speed = phys->f64;
        c->f54 = phys->f38;
        c->boost = phys->f28C;
        c->state = phys->i298;
        c->i60 = phys->i29C;
        c->i64 = phys->i2D8;
        c->i6C = model->i3EC;
        u32 flags = d->b0 != 0;
        int all = 1;
        for (int j = 0; j < 4; j++) {
            c->wheels[j].pos.x = phys->wheels[j].pos.x;
            c->wheels[j].pos.y = phys->wheels[j].pos.y;
            c->wheels[j].pos.z = phys->wheels[j].pos.z;
            c->wheels[j].v0C.x = phys->wheels[j].v0C.x;
            c->wheels[j].v0C.y = phys->wheels[j].v0C.y;
            c->wheels[j].v0C.z = phys->wheels[j].v0C.z;
            c->wheels[j].v18.x = phys->wheels[j].v18.x;
            c->wheels[j].v18.y = phys->wheels[j].v18.y;
            c->wheels[j].v18.z = phys->wheels[j].v18.z;
            c->wheels[j].f24 = phys->wheels[j].f30;
            c->wheels[j].f28 = phys->wheels[j].f38;
            c->wheels[j].i2C = phys->wheels[j].i60;
            c->wheels[j].b30 = phys->wheels[j].b64;
            c->wheels[j].i38 = phys->wheels[j].i68;
            c->wheels[j].f34 = fn_801EE0D0(model, j);
            all = all && phys->wheels[j].b64;
        }
        if (all) {
            flags |= 2;
        }
        u16 bits = phys->bits;
        if (bits & 0x40) {
            flags |= 4;
        }
        if (bits & 0x20) {
            flags |= 8;
        }
        c->flags = flags;
    }
}

void fn_802885C0(void) {}

int fn_802885C4(const void* a, const void* b) {
    return 128.0f * (((const EfxEntry*)a)->dist - ((const EfxEntry*)b)->dist);
}

void fn_802885F0(void) {
    int carOf[4];
    EfxMtx cams[4];
    int i;
    int n = fn_801E85A4();
    for (i = 0; i < n; i++) {
        cams[i] = *fn_801E8718(i);
        carOf[i] = fn_801E866C(i);
    }
    for (i = 0; i < lbl_804B3150.count; i++) {
        EfxCar* c = fn_80287B24(i);
        const Vec* pos = &c->m.pos;
        f32 s = fn_801E88F0(0);
        s *= s;
        f32 best = PSVECSquareDistance(pos, &cams[0].pos);
        best *= s;
        int vis = 0;
        if (i != carOf[0] || fn_801E8954(0)) {
            vis = 1;
        }
        for (int v = 1; vis && v < n; v++) {
            s = fn_801E88F0(v);
            s *= s;
            f32 d = PSVECSquareDistance(pos, &cams[v].pos);
            d *= s;
            best = MIN(best, d);
            vis = 0;
            if (i != carOf[v] || fn_801E8954(v)) {
                vis = 1;
            }
        }
        EfxEntry* e = &lbl_804B3160[i];
        e->car = c;
        e->index = i;
        e->dist = best;
        e->active = vis;
    }
    qsort(lbl_804B3160, lbl_804B3150.count, sizeof(EfxEntry), fn_802885C4);
}

void fn_802887EC(void) {
    for (int i = 0; i < lbl_8055F6C0; i++) {
        lbl_804B31E0[i](lbl_804B3160, lbl_804B3150.count);
    }
}

void fn_80288868(u32 seed) { lbl_8055F6C8 = seed; }

u32 fn_80288870(void) {
    lbl_8055F6C8 = (lbl_8055F6C8 * 0x12B9B0A1 + 0x1CCE8AC1) & 0x7FFFFFFF;
    return lbl_8055F6C8;
}

}  // extern "C"
