// Animated scene objects (0x801EAB2C-0x801EAFD4): objects bound by name to a slot in an animation's
// data block, which copies the slot's frame (and flags) into the object every update. A model
// object draws a loaded model at the frame; a point object draws a sprite. Names are guesses.
#include "game/common.h"

extern "C" {
void* memset(void* dst, int c, size_t n);
void fn_80176948(const char* name, s32* offset);  // find an animation slot
void* fn_802C6FC0(const char* name, int a, int b);  // load a model
void fn_802C6CF8(void* model);
void fn_802C744C(void* model, const void* frame);
void fn_802C70AC(s32 sprite, const void* frame, s32 tex, s32 size);
void fn_802DAC54(void);
s32 fn_802C7A40(int a, void* buf, int size, int b, const void* src, int c);
f32 PSVECDotProduct(const void* a, const void* b);
}

extern const s16 lbl_80562E28;  // no flags
extern u8 lbl_80488228[];

struct Vec {
    f32 x;
    f32 y;
    f32 z;
};

struct Camera {
    u8 pad[0x18];
    Vec fwd;  // offset 0x18
    Vec pos;  // offset 0x24
};

extern "C" Camera* fn_802C86DC(void);

// Axes and position.
struct Frame {
    Vec axes[3];  // offset 0x0
    Vec pos;      // offset 0x24
};

// An animation slot.
struct AnimSlot {
    Frame frame;  // offset 0x0
    s16 flags;    // offset 0x30
};

struct PointSlot {
    Frame frame;    // offset 0x0
    s16 flags;      // offset 0x30
    s32 sprite;     // offset 0x34
    s32 size;       // offset 0x38
    u32 color;      // offset 0x3C
};

class AnimObj {
public:
    AnimObj(void);
    virtual ~AnimObj(void) {}
    virtual void vf0C(void) {}
    virtual void Draw(void) = 0;
    virtual void vf14(void) {}
    virtual f32 Depth(void);
    virtual void vf1C(void);
    virtual void Set(const AnimSlot* s);
    void Bind(const char* name);
    void Update(u8* data);
    s16 Flags(void);

    Frame frame;  // offset 0x4
    s32 offset;   // offset 0x34
    s16 flags;    // offset 0x38
};

class AnimModel : public AnimObj {
public:
    AnimModel(const char* name, const char* model);
    virtual ~AnimModel(void);
    virtual void Draw(void);

    void* model;  // offset 0x3C
};

class AnimPoint : public AnimObj {
public:
    AnimPoint(const char* name);
    virtual ~AnimPoint(void);
    virtual void Draw(void);
    virtual void Set(const AnimSlot* s);

    PointSlot point;  // offset 0x3C
};

AnimObj::AnimObj(void) {
    frame.axes[0].x = 1.0f;
    frame.axes[0].x = 1.0f;
    frame.axes[0].y = 0.0f;
    frame.axes[0].z = 0.0f;
    frame.axes[1].x = 0.0f;
    frame.axes[1].y = 1.0f;
    frame.axes[1].z = 0.0f;
    frame.axes[2].x = 0.0f;
    frame.axes[2].y = 0.0f;
    frame.axes[2].z = 1.0f;
    frame.pos.x = 0.0f;
    frame.pos.y = 0.0f;
    frame.pos.z = 0.0f;
    flags = lbl_80562E28;
}

void AnimObj::Bind(const char* name) { fn_80176948(name, &offset); }

void AnimObj::Update(u8* data) { Set((const AnimSlot*)(data + offset)); }

void AnimObj::Set(const AnimSlot* s) {
    frame = s->frame;
    flags = s->flags;
}

s16 AnimObj::Flags(void) { return flags; }

AnimModel::AnimModel(const char* name, const char* model) {
    this->model = fn_802C6FC0(model, 0, 0);
    Bind(name);
}

AnimModel::~AnimModel(void) { fn_802C6CF8(model); }

void AnimModel::Draw(void) { fn_802C744C(model, &frame); }

AnimPoint::AnimPoint(const char* name) {
    Bind(name);
    memset(&point, 0, sizeof(point));
    point.sprite = -1;
}

void AnimPoint::Draw(void) {
    if (point.sprite == -1) {
        return;
    }
    s32 tex;
    if (point.color == 0) {
        tex = 0;
    } else {
        fn_802DAC54();
        u32 src[2];
        src[0] = point.color;
        src[1] = 0xFFFFFF00;
        tex = fn_802C7A40(1, lbl_80488228, 0x50, 1, src, 2);
    }
    fn_802C70AC(point.sprite, &frame, tex, point.size);
}

void AnimPoint::Set(const AnimSlot* s) {
    AnimObj::Set(s);
    point = *(const PointSlot*)s;
}

AnimPoint::~AnimPoint(void) {}

void AnimObj::vf1C(void) {}

f32 AnimObj::Depth(void) {
    Camera* c = fn_802C86DC();
    Vec d;
    d.x = frame.pos.x - c->pos.x;
    d.y = frame.pos.y - c->pos.y;
    d.z = frame.pos.z - c->pos.z;
    return PSVECDotProduct(&d, &c->fwd);
}

ModuleTag lbl_8055EB80;

// NonMatching: the AnimObj ctor stores flags before the identity instead of after (13 diffs); vf0C/vf14
// are emitted here as weak copies the target keeps in an earlier unit.
