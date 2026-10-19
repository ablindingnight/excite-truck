// Sky (0x802C0D70-0x802C110C): the sky model and the sun direction, taken from the track's "sun"
// object and clamped to a minimum elevation. Names are guesses apart from the messages and keys.
#include "game/common.h"

extern "C" {
double sqrt(double x);
double atan(double x);
double sin(double x);
double cos(double x);
void fn_802E30C8(const char* fmt, ...);  // debug print
void PSVECNormalize(const void* src, void* dst);
int fn_8012C33C(const char* name);
void* fn_802C6B44(const char* name);
s32 fn_802C6DC4(const char* name, void* model, int a, int b);
void fn_802C6CF8(s32 inst);
void fn_802C744C(s32 inst, const ValMatrix* m);
void fn_802A4E08(void);
}

struct SkyObj {
    u8 pad0[0x24];
    ValVec3 pos;  // offset 0x24
};

struct SkyCam {
    u8 pad0[0x24];
    ValVec3 pos;  // offset 0x24
};

typedef void* (*ModelLoader)(const char* name);

extern "C" {
int fn_801E2CCC(s32 inst, const char* name, SkyObj** out);
void fn_802C9A1C(ValVec3* dir);
void fn_8029231C(ValVec3* dir);
void* fn_802B2C34(ModelLoader f, const char* name);
SkyCam* fn_802C86DC(void);
}

static inline f32 DegToRad(f32 d) { return d * 0.017453292f; }

static inline void Scale(ValVec3* out, const ValVec3* v, f32 s) {
    out->x = v->x * s;
    out->y = v->y * s;
    out->z = v->z * s;
}

struct SkyMtx {
    SkyMtx(void) {
        basis[0].x = 1.0f;
        basis[0].y = 0.0f;
        basis[0].z = 0.0f;
        basis[1].x = 0.0f;
        basis[1].y = 1.0f;
        basis[1].z = 0.0f;
        basis[2].x = 0.0f;
        basis[2].y = 0.0f;
        basis[2].z = 1.0f;
    }
    ValVec3 basis[3];
    ValVec3 origin;
};

ModuleTag lbl_8055FB08;
static f32 lbl_8055FB10 = DegToRad(45.0f);  // minimum sun elevation
s32 lbl_8055FB14;                    // current sky instance
s32 lbl_8055FB18;                    // loaded sky instance
ValVec3 lbl_804C0800;                // sun position

extern "C" {

void fn_802C0D70(ValVec3* d) {
    f32 h = sqrt(d->z * d->z + d->x * d->x);
    f32 up = -d->y;
    if ((f32)atan(up / h) < lbl_8055FB10) {
        fn_802E30C8("sky: bad sun direction (%.0f < %.0f degrees); adjusting", 180.0f * (f32)atan(up / h) / 3.1415927f,
                    180.0f * lbl_8055FB10 / 3.1415927f);
        d->x = d->x * ((f32)cos(lbl_8055FB10) / h);
        d->y = -(f32)sin(lbl_8055FB10);
        d->z = d->z * ((f32)cos(lbl_8055FB10) / h);
    }
}

void fn_802C0E68(s32 inst) {
    if (lbl_8055FB14 != inst) {
        SkyObj* sun;
        ValVec3 dir;
        lbl_8055FB14 = inst;
        sun = 0;
        if (inst != -1 && fn_801E2CCC(inst, "sun", &sun)) {
            lbl_804C0800.x = sun->pos.x;
            lbl_804C0800.y = sun->pos.y;
            lbl_804C0800.z = sun->pos.z;
        } else {
            lbl_804C0800.x = 0.0f;
            lbl_804C0800.y = 250.0f;
            lbl_804C0800.z = -150.0f;
        }
        Scale(&dir, &lbl_804C0800, -1.0f);
        PSVECNormalize(&dir, &dir);
        fn_802C0D70(&dir);
        fn_802C9A1C(&dir);
    }
}

void fn_802C0F4C(void) {
    lbl_8055FB18 = -1;
    if (fn_8012C33C("sky.mod")) {
        const char* name = "sky.mod";
        void* model = fn_802B2C34(fn_802C6B44, name);
        lbl_8055FB18 = model ? fn_802C6DC4(name, model, 0, 0) : -1;
    }
    fn_802C0E68(lbl_8055FB18);
}

void fn_802C0FD4(s32* inst) {
    if (*inst != -1) {
        fn_802C6CF8(*inst);
        *inst = -1;
    }
}

void fn_802C1018(void) { fn_802C0FD4(&lbl_8055FB18); }

// NonMatching: the target keeps &m in r31 across the camera call (3 extra instructions); the sinit
// is one of the epilogue-scheduled kind.
void fn_802C1020(void) {
    if (lbl_8055FB14 != -1) {
        SkyMtx m;
        SkyCam* cam = fn_802C86DC();
        m.origin.x = cam->pos.x;
        m.origin.y = cam->pos.y;
        m.origin.z = cam->pos.z;
        fn_802C744C(lbl_8055FB14, (ValMatrix*)&m);
    }
    fn_802A4E08();
    fn_8029231C(&lbl_804C0800);
}

void fn_802C10B8(void) {
    lbl_8055FB14 = -1;
    fn_802C0F4C();
}

void fn_802C10C4(void) { fn_802C1018(); }

void fn_802C10C8(void) { fn_802C1020(); }

void fn_802C10CC(s32 inst) {
    if (inst == -1) {
        fn_802C0E68(lbl_8055FB18);
    } else {
        fn_802C0E68(inst);
    }
}

}  // extern "C"
