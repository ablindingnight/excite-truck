// Car UI model (0x80253258-0x80257284): the car shown in the menus (body, shadow, wheels and the
// efx bone mesh read from the car's .car folder), its resource loader and the menu state that
// draws it. Names are guesses apart from the file names, value keys and messages.
#include "game/common.h"
#include "macros.h"
#include <revolution/MTX.h>

struct Basis {
    Vec v[3];
};

struct Frame {
    Frame(void) {}
    Basis b;  // offset 0x0
    Vec pos;  // offset 0x24
};

// A vector as three array elements (copied with word moves).
struct Vec3a {
    f32 v[3];
};

struct Mtx33 {
    f32 m[9];
};

struct Mat3;
void Mat3Mul(Mat3* out, const Mat3* a, const Mat3* b);

// GetVec3Or takes its default by value.
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

class ValueTableV {
public:
    virtual ValueNodeV* FindChild(const char* name);  // 0x8
    virtual ValueNodeV* GetChild(int index);          // 0xC
    virtual int HasChild(const char* name);          // 0x10
};

struct WheelSetup {
    f32 rideHeight;  // offset 0x0
    u8 pad4[0x18 - 0x4];
    f32 radius;  // offset 0x18
    u8 pad1C[0x24 - 0x1C];
};

struct CarSetup {
    u8 pad0[0x10];
    f32 ftrack;     // offset 0x10
    f32 rtrack;     // offset 0x14
    f32 wheelbase;  // offset 0x18
    u8 pad1C[0x80 - 0x1C];
    WheelSetup wheels[4];  // offset 0x80
    u8 pad110[0x12C - 0x110];
    ValVec3 modelOffset;  // offset 0x12C
};

extern "C" {
int sprintf(char* buf, const char* fmt, ...);
void* memset(void* dst, int c, size_t n);
void fn_80015A20(Mtx33* out, const ValVec3* angles);
void* fn_801284E8(const char* name, u32 size);  // alloc
void fn_80128658(void* p);                      // free
ValueTableV* fn_80131808(const char* name);
ValueTableV* fn_80131810(const char* name);
void fn_80131818(ValueTableV* t);
int fn_801345D8(char* buf, int size, const char* fmt, ...);  // snprintf
s32 fn_80147C50(s32 a, s32 b, f32 t);
void fn_8018BF44(u32* flags);
void* fn_801D2008(const char* name);
void fn_801D21B4(void* p);
void fn_801F0000(void* w);
u32 fn_801F0008(void);
void* fn_801F0010(const char* cls, void* buf);
void fn_801F1394(void* w, const Frame* f);
void fn_801F1474(void* w, const Frame* f, f32 a);
f32 fn_801F159C(void* w);
f32 fn_8023659C(void);
void fn_8027A714(CarSetup* s, const char* name);
s32 fn_802C6FC0(const char* name, s32 inst, const char* prefix);
void fn_802C6CF8(s32 model);
void fn_802C70AC(s32 model, const Frame* f, s32 mat, u32 mask);
s32 fn_802C7A34(int a, Vec* p, s32 n);
s32 fn_802C7A40(int a, Vec* p, s32 n, int b, s32* c, int d);
void fn_802C7AE0(s32 model, const Frame* f, s32 mat, f32 t);
u32 fn_802C7830(const char* s);
const Frame* fn_802C86DC(void);
void fn_802C9A1C(const Vec* dir);
void fn_802E317C(const char* fmt, ...);
}

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

static inline void MulVec(Vec* out, const Vec* in, const Vec* m) {
    if (out == in) {
        f32 x = Dot3(in->x, m[0].x, in->y, m[1].x, in->z, m[2].x);
        f32 y = Dot3(in->x, m[0].y, in->y, m[1].y, in->z, m[2].y);
        f32 z = Dot3(in->x, m[0].z, in->y, m[1].z, in->z, m[2].z);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, m[0].x, in->y, m[1].x, in->z, m[2].x);
        out->y = Dot3(in->x, m[0].y, in->y, m[1].y, in->z, m[2].y);
        out->z = Dot3(in->x, m[0].z, in->y, m[1].z, in->z, m[2].z);
    }
}

static inline void SetVec(Vec* v, f32 x, f32 y, f32 z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

static inline void Scale(Vec* out, const Vec* in, f32 s) {
    out->x = in->x * s;
    out->y = in->y * s;
    out->z = in->z * s;
}

static inline void MAdd(Vec* out, const Vec* a, const Vec* d, f32 s) {
    out->x = a->x + d->x * s;
    out->y = a->y + d->y * s;
    out->z = a->z + d->z * s;
}

static inline void Lift(Vec* v, f32 y) {
    v->x = 0.0f;
    v->y = y;
    v->z = 0.0f;
}

static inline void Identity(Frame* f) {
    f->b.v[0].x = 1.0f;
    f->b.v[0].y = 0.0f;
    f->b.v[0].z = 0.0f;
    f->b.v[1].x = 0.0f;
    f->b.v[1].y = 1.0f;
    f->b.v[1].z = 0.0f;
    f->b.v[2].x = 0.0f;
    f->b.v[2].y = 0.0f;
    f->b.v[2].z = 1.0f;
    f->pos.x = 0.0f;
    f->pos.y = 0.0f;
    f->pos.z = 0.0f;
}

// Copies at most n - 1 characters and always terminates.
static inline void CopyName(char* dst, const char* src, int n) {
    char* d = dst;
    int left = n;
    while ((*d++ = *src++) != 0) {
        if (--left <= 0) {
            d[-1] = 0;
            break;
        }
    }
}

static inline int IsFront(int i) { return i == 0 || i == 1; }
static inline int IsLeft(int i) {
    int left = 1;
    if (i != 0 && i != 2) {
        left = 0;
    }
    return left;
}

static inline f32 Sum(f32 a, f32 b) { return a + b; }
static inline f32 InvLerp(f32 a, f32 b, f32 x) { return (x - a) / (b - a); }
static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

// Odd smoothstep: -2|x|^3 + 3x^2 with the sign of x, for x in [-1, 1].
static inline f32 Ease(f32 x) {
    if (x < -1.0f) {
        return -1.0f;
    } else if (x > 1.0f) {
        return 1.0f;
    } else if (x > 0.0f) {
        return x * (-2.0f * x * x) + 3.0f * x * x;
    } else {
        return x * (-2.0f * x * x) - 3.0f * x * x;
    }
}

class CarUIModel {
public:
    CarUIModel(void);
    ~CarUIModel(void);
    void Reset(void);
    void Load(const char* name);
    void Unload(void);
    void LoadEfx(const char* path);
    void UnloadEfx(void);
    void SetPose(const Frame* f, const f32* wheelY, const f32* steer);
    void Draw(void);
    void DrawShadow(void);
    void DrawEfx(void);

    u8 loaded;         // offset 0x0
    f32 start;         // offset 0x4
    s32 anim;          // offset 0x8
    s32 animSet;       // offset 0xC
    s32 model;         // offset 0x10
    s32 shadow;        // offset 0x14
    void* fwheels;     // offset 0x18
    void* rwheels;     // offset 0x1C
    void* fwheelBuf;   // offset 0x20
    void* rwheelBuf;   // offset 0x24
    s32 efx;           // offset 0x28
    s32 facingPos;     // offset 0x2C
    s32 facingNeg;     // offset 0x30
    s32 numBones;      // offset 0x34
    u32 groupMask;     // offset 0x38
    ValVec3 bones[80]; // offset 0x3C
    s32* boneset;      // offset 0x3FC
    char name[0x20];   // offset 0x400
    Vec offset;        // offset 0x420
    Vec wheelPos[4];   // offset 0x42C
    CarSetup setup;    // offset 0x45C
    f32 rideHeight;    // offset 0x594
    Frame frame;       // offset 0x598
    f32 wheelY[4];     // offset 0x5C8
    Frame wheels[4];   // offset 0x5D8
};

extern "C" {

void fn_80253258(void) {
    Vec d;
    Vec v;
    SetVec(&v, 0.0f, 250.0f, -150.0f);
    Scale(&d, &v, -1.0f);
    PSVECNormalize(&d, &d);
    fn_802C9A1C(&d);
}

}  // extern "C"

void CarUIModel::Reset(void) {
    loaded = 0;
    shadow = -1;
    model = -1;
    rwheels = 0;
    fwheels = 0;
    name[0] = 0;
    memset(&efx, 0, 0x3D8);
    efx = -1;
    Identity(&frame);
    Identity(&wheels[0]);
    Identity(&wheels[1]);
    Identity(&wheels[2]);
    Identity(&wheels[3]);
    offset.x = 0.0f;
    offset.y = 0.0f;
    offset.z = 0.0f;
    memset(&setup, 0, sizeof(CarSetup));
    rideHeight = 0.0f;
}

CarUIModel::CarUIModel(void) {
    Reset();
    start = fn_8023659C();
    animSet = 0;
    anim = 0;
    fwheelBuf = fn_801284E8("fwheelbuf", fn_801F0008());
    rwheelBuf = fn_801284E8("rwheelbuf", fn_801F0008());
}

CarUIModel::~CarUIModel(void) {
    fn_80128658(fwheelBuf);
    fn_80128658(rwheelBuf);
}

void CarUIModel::Load(const char* carName) {
    char path[0x100];
    char file[0x20];
    fn_801345D8(path, 0x100, "%s.car", carName);
    CopyName(name, carName, 0x20);
    fn_801345D8(file, 0x20, "%s/low1.mod", path);
    model = fn_802C6FC0(file, 0, path);
    fn_801345D8(file, 0x20, "%s/shadow1.mod", path);
    shadow = fn_802C6FC0(file, 0, path);
    fn_801345D8(file, 0x20, "%s/physics.val", path);
    ValueTableV* t = fn_80131808(file);
    fwheels = fn_801F0010(t->FindChild("front")->GetValue("wheel_class"), fwheelBuf);
    rwheels = fn_801F0010(t->FindChild("rear")->GetValue("wheel_class"), rwheelBuf);
    fn_8027A714(&setup, carName);
    int i;
    for (i = 0; i < 4; i++) {
        setup.wheels[i].radius = IsFront(i) ? fn_801F159C(fwheels) : fn_801F159C(rwheels);
    }
    offset = *(Vec*)&setup.modelOffset;
    f32 wheelbase = setup.wheelbase;
    f32 ftrack = setup.ftrack;
    f32 rtrack = setup.rtrack;
    rideHeight = setup.wheels[3].rideHeight;
    fn_80131818(t);
    for (i = 0; i < 4; i++) {
        int left = IsLeft(i);
        int front = IsFront(i);
        Vec p;
        p.x = 0.5f * (left ? -1.0f : 1.0f) * (front ? ftrack : rtrack);
        p.y = setup.wheels[i].radius - setup.wheels[i].rideHeight;
        p.z = wheelbase * (0.5f * (front ? 1.0f : -1.0f));
        wheelPos[i] = p;
    }
    LoadEfx(path);
    loaded = 1;
}

void CarUIModel::Unload(void) {
    if (loaded) {
        loaded = 0;
        fn_802C6CF8(model);
        fn_802C6CF8(shadow);
        fn_801F0000(fwheels);
        fn_801F0000(rwheels);
        UnloadEfx();
        Reset();
    }
}

void CarUIModel::LoadEfx(const char* path) {
    int i;
    if (efx == -1) {
        char file[0x100];
        fn_801345D8(file, 0x100, "%s/efxmesh.val", path);
        ValueTableV* t = fn_80131810(file);
        if (!t) {
            efx = -1;
            numBones = 0;
            boneset = 0;
            return;
        }
        char bonesetName[0x100] = {0};
        if (t->HasChild("boneset")) {
            ValueNodeV* n = t->FindChild("boneset");
            sprintf(bonesetName, "%s/%s", path, n->GetValue("mesh_name"));
            char mod[0x100];
            fn_801345D8(mod, 0x100, "%s/%s", path, n->GetValue("model_name"));
            efx = fn_802C6FC0(mod, 0, path);
            facingPos = n->GetIntValue("facing_pos");
            facingNeg = n->GetIntValue("facing_neg");
            numBones = n->GetIntValue("num_bones");
            if (n->Has("group_mask")) {
                groupMask = fn_802C7830(n->GetValue("group_mask"));
            } else {
                groupMask = -1;
            }
        } else {
            fn_802E317C("carmodelview: %s has no boneset table", file);
        }
        if (t->HasChild("at_rest")) {
            ValVec3 zero;
            zero.x = 0.0f;
            zero.y = 0.0f;
            zero.z = 0.0f;
            ValueNodeV* r = t->FindChild("at_rest");
            for (i = 0; i < numBones; i++) {
                char key[0x20];
                fn_801345D8(key, 0x20, "%d", i);
                bones[i] = r->GetVec3Or(key, zero);
            }
        } else {
            memset(bones, sizeof(bones), 0);
        }
        const char* susp[4] = {"lf", "rf", "lr", "rr"};
        if (t->HasChild("susp")) {
            ValueNodeV* s = t->FindChild("susp");
            for (int j = 0; j < 4; j++) {
                s->GetInt(susp[j], -1);
            }
        }
        fn_80131818(t);
        if (bonesetName[0]) {
            boneset = (s32*)fn_801D2008(bonesetName);
        } else {
            boneset = 0;
        }
    }
}

void CarUIModel::UnloadEfx(void) {
    if (efx != -1) {
        fn_802C6CF8(efx);
        efx = -1;
    }
    if (boneset) {
        fn_801D21B4(boneset);
        boneset = 0;
    }
}

void CarUIModel::SetPose(const Frame* f, const f32* wy, const f32* steer) {
    Frame w;
    Mtx33 r;
    Mtx33 r2;
    ValVec3 a;
    ValVec3 b;
    Vec o;
    Vec t;
    frame = *f;
    MulVec(&o, &offset, frame.b.v);
    frame.pos.x = frame.pos.x + o.x;
    frame.pos.y = frame.pos.y + o.y;
    frame.pos.z = frame.pos.z + o.z;
    for (int i = 0; i < 4; i++) {
        wheelY[i] = wy[i];
        int left = IsLeft(i);
        w = *f;
        MulVec(&t, &wheelPos[i], w.b.v);
        w.pos.x = w.pos.x + t.x;
        w.pos.y = w.pos.y + t.y;
        w.pos.z = w.pos.z + t.z;
        f32 h = wy[i];
        w.pos.x = w.pos.x + w.b.v[1].x * h;
        w.pos.y = w.pos.y + w.b.v[1].y * h;
        w.pos.z = w.pos.z + w.b.v[1].z * h;
        if (!left) {
            a.x = 0.0f;
            a.y = 3.1415927f;
            a.z = 0.0f;
            fn_80015A20(&r, &a);
            Mat3Mul((Mat3*)&w, (const Mat3*)&r, (const Mat3*)&w);
        }
        b.x = steer[i] * (left ? 1.0f : -1.0f);
        b.y = 0.0f;
        b.z = 0.0f;
        fn_80015A20(&r2, &b);
        Mat3Mul((Mat3*)&w, (const Mat3*)&r2, (const Mat3*)&w);
        wheels[i] = w;
    }
}

void CarUIModel::Draw(void) {
    Vec tmp[80];
    memset(tmp, 0, sizeof(tmp));
    f32 t = InvLerp(0.0f, 0.25f, fn_8023659C() - start);
    t = MIN(1.0f, t);
    t = MAX(t, 0.0f);
    s32 frameNo = fn_80147C50(animSet, anim, Ease(Lerp(0.0f, 1.0f, t)));
    s32 mat = fn_802C7A40(1, tmp, 0x50, 1, &frameNo, 1);
    u32 flags;
    fn_8018BF44(&flags);
    flags |= 0x10000000;
    fn_802C70AC(model, &frame, mat, flags);
    for (int i = 0; i < 4; i++) {
        if (IsFront(i)) {
            fn_801F1394(fwheels, &wheels[i]);
        } else {
            fn_801F1394(rwheels, &wheels[i]);
        }
    }
    DrawEfx();
}

void CarUIModel::DrawShadow(void) {
    fn_802C7AE0(shadow, &frame, -1, 40.0f);
    for (int i = 0; i < 4; i++) {
        if (IsFront(i)) {
            fn_801F1474(fwheels, &wheels[i], 40.0f);
        } else {
            fn_801F1474(rwheels, &wheels[i], 40.0f);
        }
    }
}

void CarUIModel::DrawEfx(void) {
    const Frame* f = &frame;
    if (efx != -1) {
        const Frame* cam = fn_802C86DC();
        Vec p = cam->b.v[0];
        Vec* a = (Vec*)&bones[facingPos];
        a->x = p.x;
        a->y = p.y;
        a->z = p.z;
        Vec* b = (Vec*)&bones[facingNeg];
        b->x = p.x * -1.0f;
        b->y = p.y * -1.0f;
        b->z = p.z * -1.0f;
        s32* bs = boneset;
        if (bs) {
            Lift((Vec*)&bones[bs[0x33]], wheelY[0] - offset.y);
            Lift((Vec*)&bones[bs[0x34]], wheelY[1] - offset.y);
            Lift((Vec*)&bones[bs[0x35]], wheelY[2] - offset.y);
            Lift((Vec*)&bones[bs[0x36]], wheelY[3] - offset.y);
        }
        fn_802C70AC(efx, f, fn_802C7A34(1, (Vec*)&bones[1], numBones), groupMask);
    }
}

// The pose a car is shown in: its frame, wheel heights and steering.
struct CarPose {
    Frame frame;     // offset 0x0
    u8 valid;        // offset 0x30
    u8 pad31[0xF];
    f32 wheelY[4];   // offset 0x40
    f32 steer[4];    // offset 0x50
    u8 pad60[0x18];
};

class AsyncResourceSet {
public:
    void Finish(void);
    int Status(void);
};

struct VehicleInfo {
    u8 pad0[0xA0];
    u32 flags;  // offset 0xA0
};

VehicleInfo* FindVehicleNamed(const char* chassis, int n);

extern "C" {
int strcasecmp(const char* a, const char* b);
void* memmove(void* dst, const void* src, size_t n);
}  // extern "C"
f32 Dot(float, float, float, float, float, float);
extern "C" {
double atan(double x);
double ceil(double x);
double cos(double x);
double sin(double x);
double sqrt(double x);
AsyncResourceSet* fn_8012CAA0(s32 size, const char* name);
int fn_8012CC20(AsyncResourceSet* s, const char* name);
void fn_8012D8A8(AsyncResourceSet* s);
int fn_8012C33C(const char* name);
void fn_8012E81C(const char* name);
void fn_8012E96C(const char* name);
f32 fn_801DCF48(int a);
void fn_801E2CCC(s32 model, const char* name, const Frame** f);
void fn_801EFF4C(void);
void fn_801EFF70(void);
void fn_801F1C24(void* p);
void fn_80203C50(int a);
void fn_80203DC8(void);
void fn_80203E88(const Frame* f, const Vec* target, int a);
void fn_80252BC8(void);
void fn_80252BF0(void);
void fn_80252C78(void);
void fn_80252F50(f32 dt);
void fn_80253004(s32 index, struct CarPose* p);
void fn_80253104(s32 index, CarSetup* s);
void fn_802AB9E0(f32 t);
void fn_802AB9E4(void);
void fn_802AB9E8(void);
void fn_802ABC2C(void);
void fn_802ABC80(const Frame* f, const Vec* off, f32 a, f32 b, f32 c);
void fn_802ABE50(void);
void fn_802ABE58(f32* p, f32* a, u32* b, f32 dt);
void fn_802ABFF8(u32 a, f32 b);
void fn_802AC070(void);
void fn_802AE9D0(void);
void fn_802AEB3C(const Frame* f);
void fn_802B016C(void);
u8 fn_802C2C00(u8 a);
void fn_802C2B78(f32 a, f32 b, f32 c);
void fn_802C2BB8(f32* a, f32* b, f32* c);
void fn_802C744C(s32 model, const Frame* f);
void fn_802C7730(void* p);
void fn_802C77F0(s32 model, Vec* box);
void fn_802C7808(s32 model);
void fn_802C7A00(void);
void fn_802C8638(const Frame* f);
void fn_802C86E8(int a);
void fn_802C87B4(int a, int b, s32 w, s32 h);
void fn_802C88BC(int a);
void fn_802C8BD0(s32 tex);
void fn_802C8D74(void);
void fn_802C8DB8(void* v, int nv, u16* idx, int ni);
void fn_802C942C(u32 a);
f32 fn_802C9744(void);
void fn_802C974C(f32 a, f32 b, f32 c);
void fn_802C98DC(f32* a, f32* b, f32* c, f32* d);
u32 fn_802C990C(u32 c);
void fn_802C9950(u32 c);
void fn_802C9994(u32 c);
void fn_802C99E8(f32 a);
f32 fn_802C9A50(void);
int fn_802C9C8C(u32 a);
void fn_802C9CE0(void);
s32 fn_802CAC28(const char* name, int flags);
void fn_802CAC54(s32 tex);
int fn_802D3EBC(void);
void fn_802E30C8(const char* fmt, ...);
}

void* operator new(size_t size, const char* name);

extern s32 lbl_8055CFF0;  // screen width
extern s32 lbl_8055CFF4;  // screen height

class MenuState {
public:
    MenuState(void) { unk14 = 0; }
    virtual ~MenuState(void) {}
    virtual void Enter(void);
    virtual void Exit(void);
    virtual void Unk14(void);
    virtual void Unk18(void);
    virtual void Update(void);
    virtual void Unk20(void);
    virtual void Unk24(void);
    virtual void Draw(void);
    virtual void Unk2C(void);
    virtual void Unk30(void);
    virtual void Unk34(void);
    virtual void Unk38(void);
    virtual void Unk3C(void);
    virtual void Unk40(void);
    virtual void Unk44(void);
    virtual void Unk48(void);
    virtual void Unk4C(void);
    virtual void Unk50(void);
    virtual void Unk54(void);
    virtual int Key(int key);
    virtual void Unk5C(void);

    s32 rect[4];  // offset 0x4
    int unk14;    // offset 0x14
};

// The menu state the car view sits in; it only tracks whether it is active.
class CarUIState : public MenuState {
public:
    CarUIState(void) { active = 0; }
    virtual ~CarUIState(void) {}
    virtual void Enter(void);
    virtual void Exit(void);
    virtual void Draw(void);

    u8 active;  // offset 0x18
};

// A named async resource set shared between car views.
struct ResourceLoader {
    ResourceLoader(const char* name) { set = fn_8012CAA0(0x16E360, name); }
    int IsIdle(void) { return set->Status() == 0; }
    char name[0x20];        // offset 0x0
    AsyncResourceSet* set;  // offset 0x20
    s32 refs;               // offset 0x24
};

class CarUIScreen {
public:
    CarUIScreen(int mode);
    virtual ~CarUIScreen(void);
    virtual void SetCar(const char* name);
    virtual void Unk10(void);
    virtual void Update(f32 dt, int x, int y, int w, int h);
    virtual void SetupCamera(void);
    virtual void Draw(void);
    virtual int Unk20(void);
    virtual void Unk24(void);
    void Init(const char* view);
    void LoadView(const char* view);
    void Camera(int x, int y, int w, int h, f32 yaw, f32 pitch);
    void Activate(void);
    void Deactivate(void);
    void UnloadModel(void);
    int ModelLoaded(void);
    void SetAnim(u32 a);
    void SetShine(u8 a);
    void DrawLights(void);
    void DrawFloor(u32 color, f32 a);

    s32 index;            // offset 0x4
    CarUIModel* model;    // offset 0x8
    u32 anim;             // offset 0xC
    char carName[0x20];   // offset 0x10
    u8 pending;           // offset 0x30
    f32 requestTime;      // offset 0x34
    char loadName[0x20];  // offset 0x38
    char loading[0x20];   // offset 0x58
    char loaded[0x20];    // offset 0x78
    f32 time;             // offset 0x98
    f32 pitch;            // offset 0x9C
    f32 yaw;              // offset 0xA0
    f32 unkA4;            // offset 0xA4
    Frame cam;            // offset 0xA8
    s32 unkD8;            // offset 0xD8
    u8 tint;              // offset 0xDC
    u8 nextTint;          // offset 0xDD
    s32 unkE0;            // offset 0xE0
    s32 bg;               // offset 0xE4
    s32 tex;              // offset 0xE8
    f32 dist;             // offset 0xEC
    Vec camOffset;        // offset 0xF0
    CarUIState state;     // offset 0xFC
    u8 shine;             // offset 0x118
    f32 unk11C;           // offset 0x11C
    s32 mode;             // offset 0x120
};

struct WheelHit {
    const char* name;  // offset 0x0
    f32 hit;           // offset 0x4
    f32 angle;         // offset 0x8
};

WheelHit lbl_80344890[4] = {{"lf", 0.0f, 0.0f}, {"rf", 0.0f, 0.0f}, {"lr", 0.0f, 0.0f}, {"rr", 0.0f, 0.0f}};

ModuleTag lbl_8055F318;
u16 lbl_8055F320 = 0x100;
u16 lbl_8055F322 = 0x405;
u16 lbl_8055F324 = 0x401;
UnkStaticFFFF lbl_8055F328;
UnkStatic123 lbl_80498DF0;
UnkStatic7F lbl_8055F32C;
u8 lbl_8055F330;   // track scene mode
f32 lbl_8055F334;  // frame time
f32 lbl_8055F338;  // total time
s32 lbl_8055F33C;  // loader count
s32 lbl_8055F340;  // view count
u8 lbl_8055F344;
u8 lbl_8055F345;
u8 lbl_8055F346;
u8 lbl_8055F347;
ResourceLoader* lbl_80498E08[4];
Vec lbl_80498E18;  // camera target
CarUIScreen* lbl_80498E28[4];

static inline void CreateLoader(void) {
    ResourceLoader* l = new ("ResLoader") ResourceLoader("caruimod");
    lbl_80498E08[lbl_8055F33C] = l;
    lbl_8055F33C++;
}

static inline void DestroyLoader(void) {
    for (int i = 0; i < lbl_8055F33C; i++) {
        if (lbl_80498E08[i]->IsIdle()) {
            ResourceLoader* l = lbl_80498E08[i];
            ResourceLoader** p = &lbl_80498E08[i];
            if (l) {
                fn_8012D8A8(l->set);
                delete l;
            }
            lbl_8055F33C--;
            memmove(p, p + 1, (lbl_8055F33C - i) * 4);
            return;
        }
    }
    fn_802E317C("ResourceLoader::DestroyOne - didn't find an idle ResourceLoader - did you forget to call "
                "ResourceLoader::Unload?");
}

static inline void ReleaseLoader(const char* name) {
    for (int i = 0; i < lbl_8055F33C; i++) {
        if (!strcasecmp(lbl_80498E08[i]->name, name)) {
            ResourceLoader* l = lbl_80498E08[i];
            if (--l->refs == 0) {
                l->set->Finish();
                CopyName(l->name, "", 0x20);
            }
            return;
        }
    }
}

static inline int AcquireLoader(const char* name) {
    int idle = -1;
    for (int i = 0; i < lbl_8055F33C; i++) {
        if (lbl_80498E08[i]->IsIdle()) {
            idle = i;
        } else if (!strcasecmp(lbl_80498E08[i]->name, name)) {
            lbl_80498E08[i]->refs++;
            return 1;
        }
    }
    ResourceLoader* l = lbl_80498E08[idle];
    CopyName(l->name, name, 0x20);
    if (fn_8012CC20(l->set, l->name)) {
        l->refs = 1;
        return 1;
    }
    fn_802E30C8("caruimod: failed to load %s (too big?)", l->name);
    CopyName(l->name, "", 0x20);
    l->refs = 0;
    return 0;
}

static inline int LoaderReady(const char* name) {
    if (!name[0]) {
        fn_802E30C8("ResourceLoader: Checking if unnamed resource is loaded???");
        return 0;
    }
    for (int i = 0; i < lbl_8055F33C; i++) {
        if (!strcasecmp(lbl_80498E08[i]->name, name)) {
            return lbl_80498E08[i]->set->Status() == 2;
        }
    }
    return 0;
}

static inline void MulM(Mtx33* out, const Mtx33* a, const Mtx33* b) {
    if (out == a || out == b) {
        f32 m0 = Dot(b->m[0], a->m[0], b->m[3], a->m[1], b->m[6], a->m[2]);
        f32 m3 = Dot(b->m[0], a->m[3], b->m[3], a->m[4], b->m[6], a->m[5]);
        f32 m6 = Dot(b->m[0], a->m[6], b->m[3], a->m[7], b->m[6], a->m[8]);
        f32 m1 = Dot(b->m[1], a->m[0], b->m[4], a->m[1], b->m[7], a->m[2]);
        f32 m4 = Dot(b->m[1], a->m[3], b->m[4], a->m[4], b->m[7], a->m[5]);
        f32 m7 = Dot(b->m[1], a->m[6], b->m[4], a->m[7], b->m[7], a->m[8]);
        f32 m2 = Dot(b->m[2], a->m[0], b->m[5], a->m[1], b->m[8], a->m[2]);
        f32 m5 = Dot(b->m[2], a->m[3], b->m[5], a->m[4], b->m[8], a->m[5]);
        f32 m8 = Dot(b->m[2], a->m[6], b->m[5], a->m[7], b->m[8], a->m[8]);
        out->m[0] = m0;
        out->m[3] = m3;
        out->m[6] = m6;
        out->m[1] = m1;
        out->m[4] = m4;
        out->m[7] = m7;
        out->m[2] = m2;
        out->m[5] = m5;
        out->m[8] = m8;
    } else {
        out->m[0] = Dot(b->m[0], a->m[0], b->m[3], a->m[1], b->m[6], a->m[2]);
        out->m[3] = Dot(b->m[0], a->m[3], b->m[3], a->m[4], b->m[6], a->m[5]);
        out->m[6] = Dot(b->m[0], a->m[6], b->m[3], a->m[7], b->m[6], a->m[8]);
        out->m[1] = Dot(b->m[1], a->m[0], b->m[4], a->m[1], b->m[7], a->m[2]);
        out->m[4] = Dot(b->m[1], a->m[3], b->m[4], a->m[4], b->m[7], a->m[5]);
        out->m[7] = Dot(b->m[1], a->m[6], b->m[4], a->m[7], b->m[7], a->m[8]);
        out->m[2] = Dot(b->m[2], a->m[0], b->m[5], a->m[1], b->m[8], a->m[2]);
        out->m[5] = Dot(b->m[2], a->m[3], b->m[5], a->m[4], b->m[8], a->m[5]);
        out->m[8] = Dot(b->m[2], a->m[6], b->m[5], a->m[7], b->m[8], a->m[8]);
    }
}

extern "C" {

void fn_80254568(Vec3a* out, s32 model, const char* name) {
    const Frame* f = 0;
    fn_801E2CCC(model, name, &f);
    *out = *(const Vec3a*)&f->pos;
}

void fn_802545C4(CarPose* p, s32 model, CarUIModel* m) {
    memset(p, 0, sizeof(CarPose));
    p->valid = 1;
    if (m->loaded) {
        for (int i = 0; i < 4; i++) {
            p->wheelY[i] = m->setup.modelOffset.y;
            if (model != -1) {
                p->wheelY[i] -= 0.2032f;
            }
        }
    }
    if (model != -1) {
        Vec r;
        Vec l;
        Vec c;
        fn_80254568((Vec3a*)&r, model, "front_r");
        fn_80254568((Vec3a*)&l, model, "front_l");
        fn_80254568((Vec3a*)&c, model, "rear_c");
        Vec mid;
        mid.x = l.x + 0.5f * (r.x - l.x);
        f32 ly = l.y;
        mid.y = ly + 0.5f * (r.y - l.y);
        mid.z = l.z + 0.5f * (r.z - l.z);
        p->frame.b.v[2].x = mid.x - c.x;
        p->frame.b.v[2].y = mid.y - c.y;
        p->frame.b.v[2].z = mid.z - c.z;
        p->frame.b.v[0].x = r.x - l.x;
        p->frame.b.v[0].y = r.y - l.y;
        p->frame.b.v[0].z = r.z - l.z;
        PSVECCrossProduct(&p->frame.b.v[2], &p->frame.b.v[0], &p->frame.b.v[1]);
        PSVECCrossProduct(&p->frame.b.v[1], &p->frame.b.v[2], &p->frame.b.v[0]);
        PSVECNormalize(&p->frame.b.v[0], &p->frame.b.v[0]);
        PSVECNormalize(&p->frame.b.v[1], &p->frame.b.v[1]);
        PSVECNormalize(&p->frame.b.v[2], &p->frame.b.v[2]);
        f32 up = 1.2283f;
        f32 back = -1.5566f;
        lbl_80498E18.x = mid.x + p->frame.b.v[2].x * back;
        lbl_80498E18.y = mid.y + p->frame.b.v[2].y * back;
        lbl_80498E18.z = mid.z + p->frame.b.v[2].z * back;
        lbl_80498E18.x = lbl_80498E18.x + p->frame.b.v[1].x * up;
        lbl_80498E18.y = lbl_80498E18.y + p->frame.b.v[1].y * up;
        lbl_80498E18.z = lbl_80498E18.z + p->frame.b.v[1].z * up;
        if (m->loaded) {
            f32 rad = fn_801F159C(m->fwheels);
            f32 h = -p->wheelY[1];
            f32 back = -(0.1524f + m->wheelPos[1].z);
            p->frame.pos.x = mid.x + p->frame.b.v[2].x * back;
            p->frame.pos.y = mid.y + p->frame.b.v[2].y * back;
            p->frame.pos.z = mid.z + p->frame.b.v[2].z * back;
            f32 lift = Sum(0.0f, h + (-m->wheelPos[1].y + rad));
            f32 vx = p->frame.b.v[1].x;
            p->frame.pos.x = p->frame.pos.x + vx * lift;
            p->frame.pos.y = p->frame.pos.y + p->frame.b.v[1].y * lift;
            p->frame.pos.z = p->frame.pos.z + p->frame.b.v[1].z * lift;
        } else {
            p->frame.pos = lbl_80498E18;
        }
    } else {
        Identity(&p->frame);
    }
}

}  // extern "C"

static inline double AbsD(double v) { return __fabs(v); }

void CarUIScreen::Camera(int x, int y, int w, int h, f32 yaw, f32 pitch) {
    if (lbl_8055F330 || mode == 0) {
        f32 offY = 0.0f;
        f32 offX = offY;
        f32 d = dist;
        int w2 = (f32)ceil(w / fn_802C9A50());
        f32 hh = 0.5f * (f32)h;
        f32 cy = (f32)y + hh;
        f32 hw = 0.5f * (f32)w2;
        x -= (w2 - w) / 2;
        f32 cx = (f32)x + hw;
        f32 proj;
        f32 dummy;
        fn_802C98DC(&proj, &dummy, &dummy, &dummy);
        f32 aspect = (proj * hh) / hw;
        if (aspect > proj) {
            pitch *= 3.0f;
        }
        if (state.active) {
            f32 l = (f32)state.rect[0] - cx;
            f32 t = (f32)state.rect[1] - cy;
            f32 r = l + (f32)state.rect[2];
            f32 b = t + (f32)state.rect[3];
            f32 p = 0.01f > proj ? 0.01f : proj;
            f32 f = hw / p;
            f32 a1 = (f32)AbsD((f32)atan(l / f));
            f32 a2 = (f32)AbsD((f32)atan(r / f));
            f32 ax;
            if (l < 0.0f && r > 0.0f) {
                ax = a1 + a2;
            } else {
                ax = (f32)AbsD(a2 - a1);
            }
            f32 b1 = (f32)AbsD((f32)atan(t / f));
            f32 b2 = (f32)AbsD((f32)atan(b / f));
            f32 ay;
            if (t < 0.0f && b > 0.0f) {
                ay = b1 + b2;
            } else {
                ay = (f32)AbsD(b2 - b1);
            }
            ax = MIN(ax, ay);
            f32 s = sin(0.5f * ax);
            s = 0.01f > s ? 0.01f : s;
            d = 3.1903f / s;
            offX = (0.5f * (l + r)) * (proj * d) / hw;
            offY = (0.5f * (t + b)) * (-aspect * d) / hh;
        }
        CarPose pose;
        if (lbl_8055F330) {
            fn_80253004(index, &pose);
        } else {
            fn_802545C4(&pose, bg, model);
        }
        f32 nd = -d;
        f32 nx = -offX;
        f32 floor = 2.0f * aspect;
        f32 ny = -offY;
        Mtx33* m = (Mtx33*)&cam;
        for (int i = 0; i < 400; i++) {
            f32 c = cos(pitch);
            f32 s = sin(pitch);
            Mtx33 rx;
            rx.m[0] = 1.0f;
            rx.m[1] = 0.0f;
            rx.m[2] = 0.0f;
            rx.m[3] = 0.0f;
            rx.m[4] = c;
            rx.m[5] = s;
            rx.m[6] = 0.0f;
            rx.m[7] = -s;
            rx.m[8] = c;
            f32 c2 = cos(yaw);
            f32 s2 = sin(yaw);
            cam.b.v[0].x = c2;
            cam.b.v[0].y = 0.0f;
            cam.b.v[1].x = 0.0f;
            cam.b.v[0].z = -s2;
            cam.b.v[1].y = 1.0f;
            cam.b.v[1].z = 0.0f;
            cam.b.v[2].x = s2;
            cam.b.v[2].y = 0.0f;
            cam.b.v[2].z = c2;
            MulM(m, &rx, m);
            MAdd(&cam.pos, &lbl_80498E18, &cam.b.v[2], nd);
            f32 vx = cam.b.v[0].x;
            cam.pos.x = cam.pos.x + vx * nx + cam.b.v[1].x * ny;
            cam.pos.y = cam.pos.y + cam.b.v[0].y * nx + cam.b.v[1].y * ny;
            cam.pos.z = cam.pos.z + cam.b.v[0].z * nx + cam.b.v[1].z * ny;
            if (!lbl_8055F330 || floor + fn_801DCF48(0) < cam.pos.y) {
                break;
            }
            pitch += 0.0019635f;
        }
    } else {
        Identity(&cam);
        ValVec3 a;
        a.x = 0.0f;
        a.y = yaw;
        a.z = 0.0f;
        Mtx33 ry;
        fn_80015A20(&ry, &a);
        Mat3Mul((Mat3*)&cam, (const Mat3*)&ry, (const Mat3*)&cam);
        ValVec3 b;
        b.x = pitch;
        b.y = 0.0f;
        b.z = 0.0f;
        Mtx33 rx;
        fn_80015A20(&rx, &b);
        Mat3Mul((Mat3*)&cam, (const Mat3*)&rx, (const Mat3*)&cam);
        cam.pos.y = (cam.pos.y - model->rideHeight) + camOffset.y;
        cam.pos.z = (cam.pos.z - dist) + camOffset.z;
        cam.pos.x = cam.pos.x + camOffset.x;
        MulVec(&cam.pos, &cam.pos, cam.b.v);
    }
}

void CarUIScreen::Init(const char* view) {
    shine = 0;
    unk11C = 0.0f;
    nextTint = 0;
    tint = 0;
    pending = 0;
    time = 1.5f;
    Identity(&cam);
    unkD8 = -1;
    dist = 6.0f;
    camOffset.x = 0.0f;
    camOffset.y = 0.0f;
    camOffset.z = 0.0f;
    carName[0] = 0;
    loadName[0] = 0;
    loading[0] = 0;
    loaded[0] = 0;
    anim = 0xFF000000;
    if (view) {
        LoadView(view);
    }
    tex = fn_802CAC28("showtile.tex", 0);
    CreateLoader();
    CreateLoader();
    model = new ("CarUISM") CarUIModel;
    Activate();
}

void CarUIScreen::LoadView(const char* view) {
    if (fn_8012C33C("carview.val")) {
        ValueTableV* t = fn_80131808("carview.val");
        if (t->HasChild(view)) {
            ValueNodeV* n = t->FindChild(view);
            dist = n->GetFloat("camera_dist", 6.0f);
            ValVec3 zero;
            zero.x = 0.0f;
            zero.y = 0.0f;
            zero.z = 0.0f;
            ValVec3 v = n->GetVec3Or("camera_offset", zero);
            camOffset.x = v.x;
            camOffset.y = v.y;
            camOffset.z = v.z;
            ValueNodeV* w = (ValueNodeV*)((ValueNode*)n)->FindChild("wheels");
            if (w) {
                lbl_80344890[0].hit = w->GetFloat("lf_hit", 0.0f);
                lbl_80344890[0].angle = w->GetFloat("lf_angle", 0.0f);
                lbl_80344890[1].hit = w->GetFloat("rf_hit", 0.0f);
                lbl_80344890[1].angle = w->GetFloat("rf_angle", 0.0f);
                lbl_80344890[2].hit = w->GetFloat("lr_hit", 0.0f);
                lbl_80344890[2].angle = w->GetFloat("lr_angle", 0.0f);
                lbl_80344890[3].hit = w->GetFloat("rr_hit", 0.0f);
                lbl_80344890[3].angle = w->GetFloat("rr_angle", 0.0f);
            }
        } else {
            fn_802E30C8("CarUIModel: Can't find Val table %s in Val file %s!!!", view, "carview.val");
        }
        fn_80131818(t);
    } else {
        fn_802E30C8("CarUIModel: Can't load Valfile %s!!!!", "carview.val");
    }
}

extern "C" {

void fn_80255604(int a) {
    fn_8012E81C("hobbyset.res");
    fn_8012E81C("part.res");
    fn_8012E81C("race.res");
    fn_8012E96C("swlmap.tex");
    fn_80203C50(a != 0);
    fn_801EFF4C();
    lbl_8055F330 = 0;
    fn_802AB9E8();
}

void fn_8025567C(void) {
    if (lbl_8055F330) {
        fn_802AE9D0();
        fn_80252BC8();
    }
    fn_802ABC2C();
    fn_801EFF70();
    fn_80203DC8();
    fn_802C7A00();
}

}  // extern "C"

CarUIScreen::CarUIScreen(int m) {
    mode = m;
    Init(0);
}

CarUIScreen::~CarUIScreen(void) {
    Deactivate();
    delete model;
    fn_802CAC54(tex);
    DestroyLoader();
    DestroyLoader();
}

void CarUIScreen::UnloadModel(void) { model->Unload(); }

int CarUIScreen::ModelLoaded(void) { return model->loaded; }

void CarUIScreen::SetAnim(u32 a) { anim = a; }

void CarUIScreen::SetShine(u8 a) { shine = a; }

void CarUIScreen::SetCar(const char* name) {
    if (loading[0]) {
        ReleaseLoader(loading);
        loading[0] = 0;
    }
    CopyName(carName, name, 0x20);
    pending = 1;
    if (loaded[0]) {
        requestTime = fn_8023659C();
    } else {
        requestTime = -10000.0f;
    }
}

void CarUIScreen::Unk10(void) {}

void CarUIScreen::Unk24(void) {}

void CarUIScreen::Activate(void) {
    if (lbl_8055F340 == 0) {
        fn_80255604(mode);
    }
    if (lbl_8055F330) {
        fn_80252BF0();
        fn_80252F50(0.02f);
        bg = fn_802C6FC0("sky.mod", 0, 0);
    } else {
        bg = -1;
        if (mode == 0) {
            bg = fn_802C6FC0("cc_back.mod", 0, 0);
        }
    }
    index = lbl_8055F340;
    lbl_8055F340++;
    lbl_80498E28[index] = this;
    pitch = 0.197f;
    yaw = 2.452f;
    unkA4 = 0.059f;
}

void CarUIScreen::Deactivate(void) {
    model->Unload();
    if (loaded[0]) {
        ReleaseLoader(loaded);
    }
    if (loading[0]) {
        ReleaseLoader(loading);
    }
    if (lbl_8055F330) {
        fn_80252C78();
    }
    if (bg != -1) {
        fn_802C6CF8(bg);
        bg = -1;
    }
    loaded[0] = 0;
    loading[0] = 0;
    loadName[0] = 0;
    carName[0] = 0;
    if (--lbl_8055F340 == 0) {
        fn_8025567C();
    }
}

void CarUIScreen::Update(f32 dt, int x, int y, int w, int h) {
    u32 a;
    lbl_8055F334 = dt;
    if (fn_802D3EBC()) {
        dt = 0.0f;
        lbl_8055F334 = dt;
    }
    if (index == 0) {
        lbl_8055F338 += dt;
    }
    if (lbl_8055F330 && index == 0) {
        fn_80252F50(dt);
    }
    int ready = 0;
    if (pending && fn_8023659C() - requestTime > 0.45f) {
        ready = 1;
    }
    if (ready && !loading[0]) {
        pending = 0;
        fn_801345D8(loading, 0x20, "%s.car", carName);
        CopyName(loadName, carName, 0x20);
        if (!AcquireLoader(loading)) {
            fn_802E30C8("caruimod: load failed (%s)", loading);
            loading[0] = 0;
        }
    } else if (loading[0] && LoaderReady(loading)) {
        if (loaded[0]) {
            model->Unload();
            ReleaseLoader(loaded);
        }
        model->Load(loadName);
        SetShine((FindVehicleNamed(loadName, 0)->flags >> 2) & 1);
        if (lbl_8055F330) {
            fn_80253104(index, &model->setup);
        }
        CopyName(loaded, loading, 0x20);
        loading[0] = 0;
        CarUIModel* nm = model;
        a = anim;
        loadName[0] = 0;
        nm->anim = a;
        nm->animSet = a;
        tint = nextTint;
    }
    if (!loading[0]) {
        CarUIModel* m = model;
        if (m->loaded && !pending) {
            u32 want = anim;
            if (m->anim != want) {
                if (m->anim != 0) {
                    f32 t = InvLerp(0.0f, 0.25f, fn_8023659C() - m->start);
                    t = MIN(1.0f, t);
                    t = MAX(t, 0.0f);
                    m->animSet = fn_80147C50(m->animSet, m->anim, Ease(Lerp(0.0f, 1.0f, t)));
                } else {
                    m->animSet = want;
                }
                m->anim = want;
                m->start = fn_8023659C();
            }
        }
    }
    time = time + dt;
    if (!model->loaded) {
        if (time > 2.0f) {
            yaw = yaw + 0.418879f * dt;
            pitch = pitch + 0.02f * (0.197f - pitch);
        }
        Camera(x, y, w, h, yaw, pitch);
    } else {
        f32 box[6];
        fn_802C77F0(model->model, (Vec*)box);
        fn_802C7808(model->model);
        f32 dx = box[1] - box[0];
        f32 dy = box[3] - box[2];
        f32 dz = box[5] - box[4];
        sqrt(dx * dx + dy * dy + dz * dz);
        sqrt(2.0);
        if (index == 0) {
            fn_802AB9E0(lbl_8055F338);
            for (int i = 0; i < lbl_8055F340; i++) {
                if (lbl_80498E28[i]->shine) {
                    Vec off;
                    off.x = 0.0f;
                    off.y = 0.0f;
                    off.z = 0.0f;
                    if (mode == 0) {
                        CarPose p;
                        Vec t;
                        if (lbl_8055F330) {
                            fn_80253004(i, &p);
                        } else {
                            fn_802545C4(&p, bg, model);
                            t.x = p.frame.b.v[2].x * -13.333333f;
                            t.y = p.frame.b.v[2].y * -13.333333f;
                            t.z = p.frame.b.v[2].z * -13.333333f;
                        }
                        off = t;
                    }
                    fn_802ABC80(&lbl_80498E28[i]->model->frame, &off, 1.0f, 2.5f, 1.0f);
                }
            }
            fn_802AB9E4();
        }
        f32 a;
        f32 b;
        f32 c;
        f32 d;
        fn_802C98DC(&a, &b, &c, &d);
        if (time > 2.0f) {
            yaw = yaw + 0.418879f * dt;
            pitch = pitch + 0.02f * (0.197f - pitch);
        }
        Camera(x, y, w, h, yaw, pitch);
    }
}

void CarUIScreen::SetupCamera(void) {
    Vec t;
    t.x = 0.0f;
    t.y = 0.0f;
    t.z = 0.0f;
    if (mode == 0) {
        CarPose p;
        if (lbl_8055F330) {
            fn_80253004(index, &p);
        } else {
            fn_802545C4(&p, bg, model);
        }
        t = p.frame.pos;
    }
    fn_80203E88(&cam, &t, 0);
    if (!lbl_8055F344) {
        lbl_8055F344 = 1;
        fn_802E30C8("%s", "GC: CarUIModel is clearing screen");
    }
    if (!lbl_8055F345) {
        lbl_8055F345 = 1;
        fn_802E30C8("%s", "GC: because this screen has no background right now");
    }
    if (!lbl_8055F346) {
        lbl_8055F346 = 1;
        fn_802E30C8("%s", "GC: and the GC envmap stomps the backbuffer");
    }
    if (!lbl_8055F347) {
        lbl_8055F347 = 1;
        fn_802E30C8("%s", "GC: so no one is actually clearing the backbuffer besides me");
    }
    fn_802C86E8(0);
    fn_802C87B4(0, 0, lbl_8055CFF0, lbl_8055CFF4);
    fn_802C88BC(-1);
}

struct LightDirs {
    Vec d[3];
};

static const LightDirs lbl_802FA5B8 = {{{0.7f, -0.7f, 0.0f}, {0.0f, -0.7f, 0.7f}, {-0.7f, -0.7f, 0.0f}}};

void CarUIScreen::DrawLights(void) {
    LightDirs dirs;
    Frame f;
    Vec l;
    Vec up;
    Vec z;
    Vec y;
    Vec x;
    int i = 0;
    dirs = lbl_802FA5B8;
    for (; i < 4u; i++) {
        f = *fn_802C86DC();
        f.b.v[1].y = 1.0f;
        f.b.v[0].y = 0.0f;
        f.b.v[2].y = 0.0f;
        SetVec(&x, f.b.v[0].x, 0.0f, f.b.v[0].z);
        SetVec(&z, f.b.v[2].x, 0.0f, f.b.v[2].z);
        PSVECCrossProduct(&z, &x, &y);
        PSVECNormalize(&x, &x);
        PSVECNormalize(&y, &y);
        PSVECCrossProduct(&x, &y, &z);
        f.b.v[0] = x;
        f.b.v[1] = y;
        f.b.v[2] = z;
        const Vec* d;
        if (i > 0) {
            d = &dirs.d[i - 1];
        } else {
            up.x = 0.0f;
            up.y = 1.0f;
            up.z = 0.0f;
            d = &up;
        }
        MulVec(&l, d, f.b.v);
        fn_802C9A1C(&l);
        if (fn_802C9C8C(0x14000000)) {
            if (model->loaded) {
                model->DrawShadow();
            }
            fn_802C9CE0();
        }
    }
}

struct FloorVtx {
    Vec pos;    // offset 0x0
    u32 color;  // offset 0xC
    f32 s;      // offset 0x10
    f32 t;      // offset 0x14
};

void CarUIScreen::DrawFloor(u32 color, f32 a) {
    f32 z0;
    fn_802C942C(-1);
    Frame saved = *fn_802C86DC();
    Frame f = saved;
    f.pos.y *= -1.0f;
    CarUIModel* m = model;
    if (m->loaded) {
        f32 ride = m->rideHeight;
        f32 rad = fn_801F159C(m->fwheels);
        f.pos.y = f.pos.y + (m->wheels[0].pos.y + (m->frame.pos.y - ride) - rad - 0.03f);
    }
    f.b.v[0].y *= -1.0f;
    f.b.v[1].y *= -1.0f;
    f.b.v[2].y *= -1.0f;
    fn_802C8638(&f);
    if (model->loaded) {
        if (shine) {
            fn_802ABFF8(color, a);
        }
        model->Draw();
        if (shine) {
            fn_802AC070();
            fn_802ABE50();
        }
    }
    fn_802C8638(&saved);
    fn_802C942C(0x40FFFFFF);
    m = model;
    f32 y;
    if (m->loaded) {
        y = m->wheels[0].pos.y - fn_801F159C(m->fwheels);
    } else {
        y = 0.0f;
    }
    FloorVtx v[301];
    u16 idx[150 * 9];
    v[0].pos.y = y;
    v[0].pos.x = 0.0f;
    v[0].pos.z = 0.0f;
    v[0].s = 0.0f;
    v[0].t = 0.0f;
    v[0].color = 0x20FFFFFF;
    f32 step = 0.041887902f;
    int i;
    for (i = 0; i < 150; i++) {
        f32 fi = i;
        f32 c = 1.333f * (f32)cos(fi * step);
        f32 s = 1.333f * (f32)sin(fi * step);
        f32 x0 = 2.5f * c;
        z0 = 2.5f * s;
        f32 x1 = 5.0f * c;
        f32 z1 = 5.0f * s;
        v[1 + i * 2].pos.x = x0;
        v[1 + i * 2].pos.y = y;
        v[1 + i * 2].color = 0x20FFFFFF;
        v[1 + i * 2].pos.z = z0;
        v[1 + i * 2].s = 1.333f * x0;
        v[1 + i * 2].t = 1.333f * z0;
        v[2 + i * 2].pos.x = x1;
        v[2 + i * 2].pos.y = y;
        v[2 + i * 2].pos.z = z1;
        v[2 + i * 2].color = 0x00FFFFFF;
        v[2 + i * 2].s = 1.333f * x1;
        v[2 + i * 2].t = 1.333f * z1;
    }
    int n = 1;
    for (i = 0; i < 150; i++, n += 2) {
        u16* q = &idx[i * 9];
        q[0] = 0;
        q[1] = n;
        q[2] = n + 2;
        q[3] = n;
        q[4] = n + 1;
        q[5] = n + 2;
        q[6] = n + 2;
        q[7] = n + 1;
        q[8] = n + 3;
        for (int k = 0; k < 9; k++) {
            if (q[k] > 300) {
                q[k] -= 300;
            }
        }
    }
    fn_802C8BD0(tex);
    fn_802C8DB8(v, 301, idx, 1350);
    fn_802C8D74();
}

struct ColorState {
    u32 color;  // offset 0x0
    f32 a;      // offset 0x4
    f32 b;      // offset 0x8
    f32 c;      // offset 0xC
    u8 alpha;   // offset 0x10
};

void CarUIScreen::Draw(void) {
    Frame saved = *fn_802C86DC();
    CarPose p;
    if (lbl_8055F330) {
        fn_80253004(index, &p);
        p.wheelY[0] = -0.3048f > p.wheelY[0] ? p.wheelY[0] : -0.3048f;
        if (p.wheelY[1] < -0.3048f) {
            p.wheelY[1] = p.wheelY[1];
        } else {
            p.wheelY[1] = -0.3048f;
        }
        p.wheelY[2] = p.wheelY[2] < -0.3048f ? p.wheelY[2] : -0.3048f;
        p.wheelY[3] = p.wheelY[3] < -0.3048f ? p.wheelY[3] : -0.3048f;
    } else {
        fn_802545C4(&p, bg, model);
    }
    fn_802C974C(1.0f, 2250.0f, fn_802C9744());
    fn_802C8638(&cam);
    fn_80253258();
    fn_802C99E8(1.0f);
    if (mode == 1) {
        fn_802C9950(0xAAAAC8);
    } else {
        fn_802C9950(0x555766);
    }
    fn_802C9994(0xFFF5D9);
    u8 env[0x30];
    fn_801F1C24(env);
    fn_802C7730(env);
    f32 refl;
    u32 reflColor;
    fn_802ABE58(&unk11C, &refl, &reflColor, lbl_8055F334);
    if (model->loaded) {
        model->SetPose(&p.frame, p.wheelY, p.steer);
    }
    if (mode == 1) {
        DrawFloor(reflColor, refl);
    }
    if (bg != -1) {
        Frame id;
        Identity(&id);
        if (lbl_8055F330) {
            id.pos.x = cam.pos.x;
            id.pos.y = cam.pos.y;
            id.pos.z = cam.pos.z;
            fn_802C744C(bg, &id);
        } else {
            fn_802C70AC(bg, &id, 0, 1);
        }
    }
    if (lbl_8055F330) {
        fn_802AEB3C(&cam);
        fn_802B016C();
    }
    if (mode == 0 && fn_802C9C8C(0x50000000)) {
        if (model->loaded) {
            model->DrawShadow();
        }
        fn_802C9CE0();
    }
    if (mode == 1) {
        DrawLights();
    }
    ColorState st;
    if (tint) {
        st.alpha = fn_802C2C00(1);
        st.color = fn_802C990C(0xFF000000);
        fn_802C2BB8(&st.a, &st.b, &st.c);
        fn_802C2B78(1.0f, 1.01f, 1.0f);
    } else {
        memset(&st, 0, 0x14);
    }
    if (shine && !tint) {
        fn_802ABFF8(reflColor, refl);
    }
    if (model->loaded) {
        model->Draw();
    }
    if (shine && !tint) {
        fn_802AC070();
    }
    if (tint) {
        fn_802C990C(st.color);
        fn_802C2B78(st.a, st.b, st.c);
        fn_802C2C00(st.alpha);
    }
    if (bg != -1) {
        Frame id;
        Identity(&id);
        fn_802C70AC(bg, &id, 0, 2);
    }
    if (shine) {
        fn_802ABE50();
    }
    fn_802C8638(&saved);
}

int CarUIScreen::Unk20(void) { return 0; }

void CarUIState::Enter(void) { active = 1; }

void CarUIState::Exit(void) { active = 0; }

void CarUIState::Draw(void) {}
