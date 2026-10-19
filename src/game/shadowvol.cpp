// Shadow volumes (0x802978B8-0x80297D5C): the static shadow list, faded out with distance from the
// camera, and the registered shadow casters drawn each frame. Names are guesses apart from the
// messages.
#include "game/common.h"

extern "C" {
double sqrt(double x);
void fn_802E30C8(const char* fmt, ...);  // debug print
f32 PSVECSquareDistance(const ValVec3* a, const ValVec3* b);
int fn_802C9C8C(u32 color);
void fn_802C9CE0(void);
void fn_802C7ADC(int flag);
void fn_802C7AE0(s32 inst, const ValMatrix* m, u32 color, f32 alpha);
s32 fn_802C6FC0(void* model, int a, int b);
void fn_802C6CF8(s32 inst);
f32 fn_801E88F0(int car);
extern f32 lbl_80558DC4[];  // FLT_EPSILON
extern char lbl_8055CA60[3];  // "%s"
}

void* operator new[](size_t size, const char* tag);

struct ShadowCam {
    u8 pad0[0x24];
    ValVec3 pos;  // offset 0x24
};

struct DebugFlags {
    u8 pad0[0x10];
    u32 flags;  // offset 0x10
};

extern "C" {
ShadowCam* fn_802C86DC(void);
DebugFlags* fn_801E87A8(void);
}

class ShadowCaster {
public:
    virtual void Unk08(void);
    virtual void Unk0C(void);
    virtual void Unk10(void);
    virtual void Unk14(void);
    virtual void Unk18(void);
    virtual void Draw(void);  // 0x1C
};

class StaticShadow {
public:
    StaticShadow(void) {}
    s32 inst;        // offset 0x0
    u32 color;       // offset 0x4
    ValMatrix mtx;   // offset 0x8
    f32 alpha;       // offset 0x38
    f32 radius;      // offset 0x3C
};

static inline f32 MakeF32(f32 v) { return v; }
static inline f32 Min(f32 a, f32 b) { return (b < a) ? b : a; }
static inline f32 Max(f32 a, f32 b) { return (a > b) ? a : b; }

ModuleTag lbl_8055F798;
f32 lbl_8055F7A0 = MakeF32(0.78539819f);
int lbl_8055F7A4;            // caster count
StaticShadow* lbl_8055F7A8;  // static shadows
int lbl_8055F7AC;            // static shadow count
u32 lbl_8055F7B0;            // shadow colour
u8 lbl_8055F7B4;             // failure reported
ShadowCaster* lbl_804B4F10[64];

extern "C" {

void fn_80297998(void);
void fn_80297A30(void);
void fn_80297A84(void);
void fn_80297AFC(void);
void fn_802979A4(void);
void fn_80297A08(ShadowCaster* c);
void fn_80297C3C(void* model, const ValMatrix* mtx, u32 color, f32 alpha, f32 radius);

void fn_802978B8(void) {
    fn_80297998();
    fn_80297A30();
    lbl_8055F7B0 = 0x50000000;
}

void fn_802978E4(void) {
    fn_80297A84();
    lbl_8055F7A4 = 0;
}

void fn_8029790C(void) {
    if (fn_802C9C8C(lbl_8055F7B0)) {
        fn_802C7ADC((fn_801E87A8()->flags >> 1) & 1);
        fn_80297AFC();
        fn_802979A4();
        fn_802C9CE0();
    } else if (!lbl_8055F7B4) {
        lbl_8055F7B4 = 1;
        fn_802E30C8(lbl_8055CA60, "shadow: shadow volumes failed!");
    }
}

void fn_80297980(void) {}

void fn_80297984(void) {}

void fn_80297988(ShadowCaster* c) { fn_80297A08(c); }

void fn_8029798C(void* model, const ValMatrix* mtx, u32 color, f32 alpha, f32 radius) {
    fn_80297C3C(model, mtx, color, alpha, radius);
}

void fn_80297990(u32 color) { lbl_8055F7B0 = color; }

void fn_80297998(void) { lbl_8055F7A4 = 0; }

void fn_802979A4(void) {
    for (int i = 0; i < lbl_8055F7A4; i++) {
        lbl_804B4F10[i]->Draw();
    }
}

void fn_80297A08(ShadowCaster* c) {
    if (lbl_8055F7A4 < 64) {
        lbl_804B4F10[lbl_8055F7A4] = c;
        lbl_8055F7A4++;
    }
}

void fn_80297A30(void) {
    lbl_8055F7A8 = new ("static shadows") StaticShadow[128];
    lbl_8055F7AC = 0;
}

void fn_80297A84(void) {
    for (int i = 0; i < lbl_8055F7AC; i++) {
        fn_802C6CF8(lbl_8055F7A8[i].inst);
    }
    delete[] lbl_8055F7A8;
    lbl_8055F7A8 = NULL;
    lbl_8055F7AC = 0;
}

void fn_80297AFC(void) {
    f32 range;
    f32 zero;
    f32 one;
    ValVec3 camPos = fn_802C86DC()->pos;
    zero = 0.0f;
    one = 1.0f;
    range = 5.0f;
    for (int i = 0; i < lbl_8055F7AC; i++) {
        StaticShadow* s = &lbl_8055F7A8[i];
        f32 scale = fn_801E88F0(-1);
        scale *= scale;
        f32 d2 = PSVECSquareDistance(&camPos, &s->mtx.origin);
        d2 *= scale;
        f32 alpha;
        if (d2 > s->radius * s->radius) {
            f32 dist = sqrt(d2);
            alpha = s->alpha;
            f32 t = (dist - s->radius - zero) / range;
            t = Max(Min(t, one), zero);
            alpha = alpha + t * (zero - alpha);
        } else {
            alpha = s->alpha;
        }
        if (!((f32)__fabs(alpha) <= lbl_80558DC4[0])) {
            fn_802C7AE0(s->inst, &s->mtx, s->color, alpha);
        }
    }
}

void fn_80297C3C(void* model, const ValMatrix* mtx, u32 color, f32 alpha, f32 radius) {
    if (lbl_8055F7AC < 128) {
        StaticShadow* s = &lbl_8055F7A8[lbl_8055F7AC];
        s->inst = fn_802C6FC0(model, 0, 0);
        s->color = color;
        s->mtx = *mtx;
        s->alpha = alpha;
        s->radius = radius;
        lbl_8055F7AC++;
    }
}

}  // extern "C"

// NonMatching: the sinit is one of the epilogue-scheduled kind, and .sdata2 holds 1.0 before 0.0
// in the target; every assignment order that puts 1.0 first changes fn_80297AFC's loads.
