// Named cameras (0x80276068-0x8027820C): the named camera sets ("namedcam", "rddefault", "raceday",
// "house") and the timed camera transitions ("physcam: Clock looped during camera transition!!").
// Written against raw field offsets; the large routines and the initialiser are still stubs. Names
// are guesses apart from the strings.
#include "game/common.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

// A named camera setup ("namedcam" entries in the camera file).
// A camera pose: rotation then position.
struct CamMat3 {
    f32 m[9];
};

struct CamVec3 {
    f32 v[3];
};

struct CamPose {
    CamMat3 rot;  // offset 0x0
    CamVec3 pos;  // offset 0x24
};

struct NamedCam {
    u8 valid;       // offset 0x0
    u8 pad1[0x3];
    CamPose pose;   // offset 0x4
    f32 fov;        // offset 0x34
    char name[16];  // offset 0x38
};

// Camera definition reader (only the slot used here is named).
class CamReader {
public:
    virtual const char* Tag(void);  // 0x08
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual const char* GetString(const char* key, const char* def);  // 0x18
    virtual void v1C(void);
    virtual void v20(void);
    virtual f32 GetFloat(const char* key);  // 0x24
};

extern "C" {
int strcasecmp(const char* a, const char* b);
char* strncpy(char* d, const char* s, u32 n);
void* memcpy(void* d, const void* s, u32 n);
void fn_8026A4A0(int i, f32* pose, f32* fov);
}

extern u8 lbl_8049FD58[];
extern f32 lbl_8055E3C8;
extern NamedCam* lbl_8055F598;
extern NamedCam lbl_8049FE98;
extern NamedCam lbl_8049FEE0[];
extern NamedCam* lbl_8055F59C;
extern s32 lbl_8055F5A0;

extern "C" {

// Mode set as a bit mask over modes 4..23 (the original tests a mask, not a switch).
#define CAM_MODE_BIT(m) (1 << ((m) - 4))

int fn_80276068(int mode) {
    int r = 0;
    u32 i = mode - 4;
    if (i <= 19 && ((1 << i) & (CAM_MODE_BIT(4) | CAM_MODE_BIT(5) | CAM_MODE_BIT(6) | CAM_MODE_BIT(9) | CAM_MODE_BIT(23)))) {
        r = 1;
    }
    return r;
}

void fn_80276098(f32* dst, f32* src, int a, int b, f32 from, f32 to, f32 t) {}  // stub (309)

void fn_8027656C(CamPose* pose, f32* fov, int a) {}  // stub (310)

NamedCam* fn_80276A44(const char* name) {
    NamedCam* c = lbl_8049FEE0;
    for (int i = 0; i < lbl_8055F5A0; i++) {
        if (!strcasecmp(c->name, name)) {
            return &lbl_8049FEE0[i];
        }
        c++;
    }
    return 0;
}

void fn_80277F84(NamedCam* c, CamReader* r, int n);
extern const char* lbl_8055C638;

// Reads one named camera and remembers the special "house" and default ones.
void fn_80276ACC(CamReader* r, const char* def) {
    const char* name = r->GetString("name", def);
    NamedCam* c = &lbl_8049FEE0[lbl_8055F5A0++];
    fn_80277F84(c, r, lbl_8055F5A0);
    strncpy(c->name, name, 0x10);
    if (!strcasecmp(name, "house")) {
        lbl_8055F59C = c;
    } else if (!strcasecmp(name, lbl_8055C638)) {
        lbl_8055F598 = c;
    }
}

void fn_80276B8C(void) {
    u8* e = lbl_8049FD58;
    for (int i = 0; i < 4; i++) {
        memset(e, 0, 0x50);
        e += 0x50;
    }
}

void fn_80276BE4(CamPose* out, f32* fov) {
    if (lbl_8049FD58[0]) {
        fn_8027656C(&lbl_8049FE98.pose, &lbl_8049FE98.fov, 0);
    }
    out->rot = lbl_8049FE98.pose.rot;
    out->pos = lbl_8049FE98.pose.pos;
    *fov = lbl_8049FE98.fov;
}

void fn_80276CB0(const char* name, int blend) {
    NamedCam* c = fn_80276A44(name);
    if (!c) {
        c = lbl_8055F598;
    }
    if (blend) {
        fn_80276098((f32*)&lbl_8049FE98.pose, (f32*)&c->pose, 0, 0x22, lbl_8049FE98.fov, c->fov, 1.0f);
    } else {
        memcpy(&lbl_8049FE98, c, 0x48);
        lbl_8049FD58[0] = 0;
    }
}

void fn_80276D3C(const char* name, int mode) {
    NamedCam* c = fn_80276A44(name);
    if (mode == 1) {
        f32 pose[12];
        f32 fov;
        fn_8026A4A0(0, pose, &fov);
        fn_80276098(pose, (f32*)&c->pose, 0, 0x22, fov, c->fov, 2.0f);
    } else {
        memcpy(&lbl_8049FE98, c, 0x48);
        lbl_8049FD58[0] = 0;
    }
}

void fn_80276DC8(u8* p) {}  // stub (591)

f32 fn_80277704(int i) {
    u8* e = lbl_8049FD58 + i * 0x50;
    if (e[0]) {
        return (lbl_8055E3C8 - F32(e, 0x48)) / F32(e, 0x44);
    }
    return -1.0f;
}

void fn_80277740(void) {
    lbl_8055F5A0 = 0;
    lbl_8055F598 = 0;
    lbl_8055F59C = 0;
}

// Handles one camera-file block; returns whether the tag was recognised.
int fn_80277754(CamReader* r) {
    if (!strcasecmp(r->Tag(), "raceday")) {
        fn_80276ACC(r, lbl_8055C638);
    } else if (!strcasecmp(r->Tag(), "house")) {
        fn_80276ACC(r, "house");
    } else if (!strcasecmp(r->Tag(), "namedcam")) {
        fn_80276ACC(r, "def_name");
    } else {
        return 0;
    }
    return 1;
}

extern "C" int sprintf(char* buf, const char* fmt, ...);
extern "C" char* strcpy(char* d, const char* s);
extern const f32 lbl_80564048;  // 0.0
extern const f32 lbl_8056404C;  // 1.0
extern const f32 lbl_80564070;  // default fov, pi/2
extern const f32 lbl_80564074;  // house fov, 85 degrees

// Identity pose at the origin with the default fov.
static inline void ResetCam(NamedCam* c) {
    c->pose.rot.m[0] = lbl_8056404C;
    c->pose.rot.m[1] = lbl_80564048;
    c->pose.rot.m[2] = lbl_80564048;
    c->pose.rot.m[3] = lbl_80564048;
    c->pose.rot.m[4] = lbl_8056404C;
    c->pose.rot.m[5] = lbl_80564048;
    c->pose.rot.m[6] = lbl_80564048;
    c->pose.rot.m[7] = lbl_80564048;
    c->pose.rot.m[8] = lbl_8056404C;
    c->pose.pos.v[0] = lbl_80564048;
    c->pose.pos.v[1] = lbl_80564048;
    c->pose.pos.v[2] = lbl_80564048;
    c->fov = lbl_80564070;
    c->name[0] = 0;
}

// Makes sure the default and "house" cameras exist.
void fn_80277828(void) {
    NamedCam* c;
    ResetCam(&lbl_8049FE98);
    lbl_8049FE98.valid = 1;
    if (!lbl_8055F598) {
        int n = lbl_8055F5A0;
        c = &lbl_8049FEE0[n];
        ResetCam(c);
        lbl_8055F5A0 = n + 1;
        lbl_8055F598 = c;
        c->valid = 1;
        sprintf(c->name, lbl_8055C638, c, n);
    }
    c = lbl_8055F59C;
    if (!c) {
        int n = lbl_8055F5A0;
        c = &lbl_8049FEE0[n];
        lbl_8055F59C = c;
        lbl_8055F5A0 = n + 1;
        c->valid = 1;
    }
    c->fov = lbl_80564074;
    strcpy(lbl_8055F59C->name, "house");
}

int fn_80277960(const char* name, CamPose* out, f32* fov) {
    NamedCam* c = fn_80276A44(name);
    if (c) {
        out->rot = c->pose.rot;
        out->pos = c->pose.pos;
        *fov = c->fov;
        return 1;
    }
    return 0;
}

void fn_80277A14(u8* p) {}  // stub (348)

extern char lbl_80347378[];  // this unit's string pool ("rddefault", ...)
#define POOL(o) (lbl_80347378 + (o))

void fn_80277F84(NamedCam* c, CamReader* r, int n) {
    c->pose.pos.v[0] = r->GetFloat(POOL(0x178));   // "location_x"
    c->pose.pos.v[1] = r->GetFloat(POOL(0x184));   // "location_y"
    c->pose.pos.v[2] = r->GetFloat(POOL(0x190));   // "location_z"
    c->pose.rot.m[0] = r->GetFloat(POOL(0x19C));   // "basis_x_x"
    c->pose.rot.m[1] = r->GetFloat(POOL(0x1A8));   // "basis_x_y"
    c->pose.rot.m[2] = r->GetFloat(POOL(0x1B4));   // "basis_x_z"
    c->pose.rot.m[3] = r->GetFloat(POOL(0x1C0));   // "basis_y_x"
    c->pose.rot.m[4] = r->GetFloat(POOL(0x1CC));   // "basis_y_y"
    c->pose.rot.m[5] = r->GetFloat(POOL(0x1D8));   // "basis_y_z"
    c->pose.rot.m[6] = r->GetFloat(POOL(0x1E4));   // "basis_z_x"
    c->pose.rot.m[7] = r->GetFloat(POOL(0x1F0));   // "basis_z_y"
    c->pose.rot.m[8] = r->GetFloat(POOL(0x1FC));   // "basis_z_z"
    c->fov = r->GetFloat("fov");
    strncpy(c->name, r->GetString("name", ""), 0x10);
    c->valid = 1;
}

void fn_80278168(u8* p) {}  // stub (41)
}  // extern "C"
