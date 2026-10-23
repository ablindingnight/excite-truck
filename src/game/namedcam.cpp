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
    u32 unk0;       // offset 0x0
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

int fn_80276068(int mode) {
    switch (mode) {
    case 4:
    case 5:
    case 6:
    case 9:
    case 23:
        return 1;
    }
    return 0;
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

void fn_80277828(u8* p) {}  // stub (78)

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

void fn_80277F84(NamedCam* c, CamReader* r, int n) {}  // stub (121)

void fn_80278168(u8* p) {}  // stub (41)
}  // extern "C"
