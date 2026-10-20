// Render state ("mr", 0x802C82C0-0x802CA0A4): the display mode (normal, widescreen, letterbox), the
// viewport and its save/restore object, the camera matrix and its 2D/3D projection setups, the field
// of view with its derived frustum factors, sphere/point visibility tests, fog and clear colours,
// fullscreen and screen-rectangle fills, the "mrDrawList" pooled flare objects and the light
// direction. Names are guesses apart from the messages.
#include "game/common.h"

BOOL OptWidescreen(void);

extern "C" {
double tan(double x);
double atan(double x);
double sin(double x);
double cos(double x);
void fn_802E30C8(const char* fmt, ...);  // debug print
int fn_802E9A0C(void);                   // console widescreen setting
int fn_80133010(void);                   // prefs loaded
void fn_80133348(int owner, const char* key, s32 value);
void fn_801332D0(int owner, const char* key, s32* value);
int fn_8013365C(const char* name, void (*cb)(void));
void fn_801336E0(int handle, void (*cb)(void));
void fn_802C5F48(void);
void fn_802C6080(void);
void fn_802CFFAC(int r, int g, int b);
void fn_802D0160(int r, int g, int b);
void fn_802D0188(int r, int g, int b);
void fn_802C7758(void);
void fn_802C7734(int a);
void fn_802CC60C(void);
void fn_802CFCA0(void);
void fn_802CFDE0(void);
void fn_802CC770(void);
void fn_802D03F4(void* mtx);
void fn_802D01E0(int x, int y, int w, int h);
void fn_802CC518(int w, f32 f);
void fn_802D02F8(u32 color, int a);
void fn_802CAD74(void);
void fn_802CAD68(void);
void fn_802D68C4(void);
int fn_801E4F20(int prim);
void fn_801E4F3C(f32 x, f32 y, f32 z, f32 u, f32 v, u32 color, int a);
void fn_801E4FF4(void);
void fn_802CB29C(int a);
void fn_802D0368(f32 a, f32 b, f32 fov, f32 aspect);
void fn_802D007C(f32 a);
void fn_802D01B0(f32 a, void* v);
void fn_802D93E0(void);
int fn_802D31D8(int a);
void fn_802D362C(int a);
void fn_802D3304(void);
void fn_802D08CC(int a);
f32 PSVECDotProduct(const void* a, const void* b);
void PSVECNormalize(const void* in, void* out);
void PSVECCrossProduct(const void* a, const void* b, void* out);
f32 PSVECDistance(const void* a, const void* b);
f32 PSVECSquareDistance(const void* a, const void* b);
void* fn_801E7840(int a);
void fn_801E7888(void* p);
void fn_801E7890(void* p, int a, int b, f32 x, f32 y, f32 z, f32 s, f32 t);
void fn_801E7894(void* p);
}

void* operator new(size_t size, const char* tag);

extern f32 lbl_80558DC4[];  // FLT_EPSILON
extern s32 lbl_8055CFF0;  // screen width
extern s32 lbl_8055CFF4;  // screen height

struct Vec {
    Vec(void) {}
    Vec(f32 x, f32 y, f32 z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    f32 x;
    f32 y;
    f32 z;
};

struct VecP {
    void Set(f32 x_, f32 y_, f32 z_) {
        x = x_;
        y = y_;
        z = z_;
    }
    f32 x;
    f32 y;
    f32 z;
};

// The axes.
struct Rot {
    VecP right;  // offset 0x0
    VecP up;     // offset 0xC
    VecP fwd;    // offset 0x18
};

// Axes and position.
struct Camera {
    Rot rot;   // offset 0x0
    VecP pos;  // offset 0x24
};

struct Viewport {
    s32 x;  // offset 0x0
    s32 y;  // offset 0x4
    s32 w;  // offset 0x8
    s32 h;  // offset 0xC
    Viewport(int x, int y, int w, int h);
    ~Viewport(void);
};

struct Perspective {
    Perspective(void) {}
    Perspective(f32 near, f32 far, f32 fov);
    ~Perspective(void);
    f32 near;  // offset 0x0
    f32 far;   // offset 0x4
    f32 fov;   // offset 0x8
};

struct Pool {
    Pool(const char* name, int count, int size);
    virtual ~Pool(void);
    u8 pad4[0x1C];
};

extern "C" {
void* fn_8012A2D8(Pool* p);
void fn_8012A364(Pool* p, void* item);
}

// A pooled flare.
struct Flare {
    static void* operator new(size_t size, const char* tag);
    static void operator delete(void* p);
    Flare(int id);
    ~Flare(void);
    void Draw(int a, int b, f32 x, f32 y, f32 z, f32 s, f32 t);
    void Done(void);

    void* fx;    // offset 0x0
    VecP pos;    // offset 0x4
    f32 radius;  // offset 0x10
    s32 count;   // offset 0x14
};

static inline f32 fabsf(f32 x) { return __fabs(x); }

static inline BOOL Equal(f32 a, f32 b) { return fabsf(a - b) <= lbl_80558DC4[0]; }

static inline f32 Max(f32 a, f32 b) { return a > b ? a : b; }

ModuleTag lbl_8055FC28;
s32 lbl_8055FC30;  // depth mode
u8 lbl_8055FC34;
u8 lbl_8055FC35;  // camera saved
f32 lbl_8055FC38;  // near
f32 lbl_8055FC3C;  // far
f32 lbl_8055FC40;  // fov
f32 lbl_8055FC44;  // tan(fov / 2)
f32 lbl_8055FC48;  // sin(fov / 2)
f32 lbl_8055FC4C;  // effective fov
f32 lbl_8055FC50;
f32 lbl_8055FC54;
f32 lbl_8055FC58;
s32 lbl_8055FC5C;  // viewport
s32 lbl_8055FC60;
s32 lbl_8055FC64;
s32 lbl_8055FC68;
f32 lbl_8055FC6C;  // viewport aspect
s32 lbl_8055FC70;  // draw state
u8 lbl_8055FC74;   // 2D
bool lbl_8055FC75;
u32 lbl_8055FC78;  // fog colour
u32 lbl_8055FC7C;
u32 lbl_8055FC80;
s32 lbl_8055FC84;
f32 lbl_8055FC88;
u8 lbl_8055FC8C;  // widescreen
u8 lbl_8055FC8D;  // letterbox
u8 lbl_8055FC8E;
u8 lbl_8055FC8F;  // fullscreen fill open
s32 lbl_8055FC90;  // display type
u8 lbl_8055FC94;   // warned
s32 lbl_8055FC98;  // flares alive
Pool* lbl_8055FC9C;
u32 lbl_8055D018 = 0xFF7F7F7F;  // clear colour
f32 lbl_8055D01C = 1.0f;        // light
f32 lbl_8055D020 = 1.0f;        // horizontal squeeze
f32 lbl_8055D024 = 1.0f;        // vertical squeeze
s32 lbl_8055D028 = -1;          // prefs callback
Camera lbl_804C13E0;  // saved camera
Camera lbl_804C1410;  // camera
Vec lbl_804C1440(0.0f, -1.0f, 0.0f);  // light direction
// The saved near/far planes and the current field of view.
struct FovState {
    f32 near;     // offset 0x0
    f32 far;      // offset 0x4
    f32 fov;      // offset 0x8
    f32 pad[10];  // offset 0xC
};

FovState lbl_804C144C;

extern "C" {
void fn_802C87B4(int x, int y, int w, int h);
void fn_802C974C(f32 near, f32 far, f32 fov);
s32 fn_802C9C58(s32 mode);
void fn_802C8BD0(int a);
void fn_802C8C04(int a);
void fn_802C8D74(void);
void fn_802C92D0(void);
void fn_802C92D8(void);
void fn_802C92DC(int x0, int y0, int x1, int y1, u32 color);
void fn_802C99D8(void);
int fn_802C9640(const VecP* p, f32 r);
}

extern "C" {

void fn_802C82C0(void) {}

void fn_802C82C4(int type) {
    if (OptWidescreen() || fn_802E9A0C() == 1) {
        type = 1;
    }
    lbl_8055FC90 = type;
    if (fn_80133010()) {
        fn_80133348(1, "display_type", lbl_8055FC90);
    }
    lbl_8055FC8C = 0;
    lbl_8055D020 = 1.0f;
    lbl_8055D024 = 1.0f;
    lbl_8055FC8D = 0;
    switch (type) {
    case 0:
        fn_802E30C8("mr: normal mode");
        break;
    case 1:
        fn_802E30C8("mr: widescreen mode");
        lbl_8055FC8C = 1;
        lbl_8055D020 = 0.75f;
        break;
    case 2:
        fn_802E30C8("mr: letterbox mode");
        lbl_8055FC8D = 1;
        lbl_8055D020 = 0.75f;
        lbl_8055D024 = 0.75f;
        break;
    }
}

s32 fn_802C83C4(void) { return lbl_8055FC90; }

void fn_802C83CC(void) {
    int n = 10;
    lbl_804C1410.rot.right.x = 1.0f;
    lbl_804C1410.rot.right.y = 0.0f;
    lbl_804C1410.rot.right.z = 0.0f;
    lbl_804C1410.rot.up.x = 0.0f;
    lbl_804C1410.rot.up.y = 1.0f;
    lbl_804C1410.rot.up.z = 0.0f;
    lbl_804C1410.rot.fwd.x = 0.0f;
    lbl_804C1410.rot.fwd.y = 0.0f;
    lbl_804C1410.rot.fwd.z = 1.0f;
    lbl_804C1410.pos.x = 0.0f;
    lbl_804C1410.pos.y = 0.0f;
    lbl_804C1410.pos.z = 0.0f;
    lbl_8055FC5C = 0;
    lbl_8055FC60 = 0;
    lbl_8055FC64 = n;
    lbl_8055FC68 = n;
    lbl_8055FC6C = (f32)n / (f32)n;
    fn_802C5F48();
    lbl_8055FC34 = 0;
    fn_802C974C(1.0f, 100.0f, 1.3089969f);
    lbl_8055FC70 = 0;
    fn_802CFFAC(0xC9, 0xC9, 0xEB);
    fn_802C7758();
    fn_802C82C4(0);
    fn_802C9C58(0);
    if (fn_80133010()) {
        fn_801332D0(1, "display_type", &lbl_8055FC90);
        lbl_8055D028 = fn_8013365C("mr", fn_802C82C0);
    }
}

void fn_802C84EC(void) {
    if (fn_80133010()) {
        fn_801336E0(lbl_8055D028, fn_802C82C0);
        lbl_8055D028 = -1;
    }
    fn_802C6080();
}

void fn_802C8530(void) {
    lbl_8055FC70 = 1;
    fn_802CC60C();
    fn_802CFCA0();
}

void fn_802C855C(void) {
    fn_802CFDE0();
    fn_802CC770();
    lbl_8055FC70 = 0;
}

void fn_802C8588(Camera* cam, const VecP* eye, const VecP* target) {
    VecP up;
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    cam->pos = *eye;
    cam->rot.fwd.x = target->x - eye->x;
    cam->rot.fwd.y = target->y - eye->y;
    cam->rot.fwd.z = target->z - eye->z;
    PSVECNormalize(&cam->rot.fwd, &cam->rot.fwd);
    PSVECCrossProduct(&up, &cam->rot.fwd, &cam->rot.right);
    PSVECNormalize(&cam->rot.right, &cam->rot.right);
    PSVECCrossProduct(&cam->rot.fwd, &cam->rot.right, &cam->rot.up);
}

void fn_802C8638(const Camera* cam) {
    lbl_804C1410 = *cam;
    fn_802D03F4(&lbl_804C1410);
}

Camera* fn_802C86DC(void) { return &lbl_804C1410; }

void fn_802C86E8(u32 color) { lbl_8055D018 = color; }

}  // extern "C"

Viewport::Viewport(int x, int y, int w, int h) {
    this->x = lbl_8055FC5C;
    this->y = lbl_8055FC60;
    this->w = lbl_8055FC64;
    this->h = lbl_8055FC68;
    fn_802C87B4(x, y, w, h);
}

Viewport::~Viewport(void) { fn_802C87B4(x, y, w, h); }

extern "C" {

void fn_802C87B4(int x, int y, int w, int h) {
    if (!lbl_8055FC74 && lbl_8055FC8D) {
        int mid = lbl_8055CFF4 / 2;
        int ny = mid + (int)(0.5f + lbl_8055D024 * (y - mid));
        h = (int)(0.5f + h * lbl_8055D024);
        y = ny;
    }
    lbl_8055FC64 = w;
    lbl_8055FC5C = x;
    lbl_8055FC60 = y;
    lbl_8055FC68 = h;
    lbl_8055FC6C = (f32)w / (f32)h;
    fn_802D01E0(x, y, w, h);
    fn_802CC518(w, lbl_8055FC44);
}

void fn_802C88BC(int a) { fn_802D02F8(lbl_8055D018, a & 1); }

void fn_802C88CC(u32 color) { fn_802D02F8(color, 1); }

f32 fn_802C88D4(const VecP* p) { return PSVECDistance(&lbl_804C1410.pos, p); }

f32 fn_802C88E8(const VecP* p) { return PSVECSquareDistance(&lbl_804C1410.pos, p); }

}  // extern "C"

extern "C" {

void fn_802C88FC(Camera* m) {
    f32 d = 0.5f * lbl_8055FC64 / lbl_8055FC44;
    d *= 1.0f / lbl_8055D020;
    m->rot.right.x = 1.0f;
    m->rot.right.y = 0.0f;
    m->rot.right.z = 0.0f;
    m->rot.up.x = 0.0f;
    m->rot.up.y = -1.0f;
    m->rot.up.z = 0.0f;
    m->rot.fwd.x = 0.0f;
    m->rot.fwd.y = 0.0f;
    m->rot.fwd.z = 1.0f;
    m->pos.Set(lbl_8055FC5C + 0.5f * lbl_8055FC64, lbl_8055FC60 + 0.5f * lbl_8055FC68, -d);
}

void fn_802C89E8(Camera* m) {
    f32 d = 0.5f * lbl_8055FC64 / lbl_8055FC44;
    if (!lbl_8055FC74) {
        d *= 1.0f / lbl_8055D020;
    }
    f32 sq = lbl_8055FC74 ? 1.0f : 1.0f / lbl_8055D020;
    m->rot.right.x = sq;
    m->rot.right.y = 0.0f;
    m->rot.right.z = 0.0f;
    m->rot.up.x = 0.0f;
    m->rot.up.y = -1.0f;
    m->rot.up.z = 0.0f;
    m->rot.fwd.x = 0.0f;
    m->rot.fwd.y = 0.0f;
    m->rot.fwd.z = 1.0f;
    m->pos.Set(lbl_8055FC5C + 0.5f * lbl_8055FC64, lbl_8055FC60 + 0.5f * lbl_8055FC68, -d);
}

void fn_802C8B08(Camera* out) { *out = lbl_804C1410; }

void fn_802C8B70(int a) {
    fn_802CAD74();
    lbl_8055FC70 = 2;
}

void fn_802C8B98(int a, int b) {
    fn_802C7734(b);
    fn_802C8B70(a);
}

void fn_802C8BD0(int a) {
    fn_802C7758();
    fn_802C8B70(a);
}

void fn_802C8C04(int a) {
    Camera m;
    lbl_804C13E0 = lbl_804C1410;
    lbl_8055FC35 = 1;
    fn_802C89E8(&m);
    fn_802C8638(&m);
    fn_802C8BD0(a);
}

void fn_802C8CBC(int a) {
    Camera m;
    lbl_804C13E0 = lbl_804C1410;
    lbl_8055FC35 = 1;
    fn_802C88FC(&m);
    fn_802C8638(&m);
    fn_802C8BD0(a);
}

void fn_802C8D74(void) {
    lbl_8055FC70 = 1;
    if (lbl_8055FC35) {
        fn_802C8638(&lbl_804C13E0);
    }
    lbl_8055FC35 = 0;
}

void fn_802C8DB8(void) { fn_802D68C4(); }

int fn_802C8DBC(int prim) {
    if (fn_801E4F20(prim)) {
        lbl_8055FC8F = 1;
        lbl_8055FC70 = 6;
        return 1;
    }
    return 0;
}

}  // extern "C"

extern "C" {

void fn_802C8E00(void) {
    u8 open = 0;
    if (lbl_8055FC8F && !((int (*)(void))fn_801E4F3C)()) {
        open = 1;
    }
    lbl_8055FC8F = open;
}

void fn_802C8E48(void) {
    fn_801E4FF4();
    lbl_8055FC70 = 2;
}

void fn_802C8E70(const VecP* p, VecP* out) {
    Camera* c = &lbl_804C1410;
    f32 x = p->x - c->pos.x;
    out->x = x;
    f32 y = p->y - c->pos.y;
    out->y = y;
    f32 z = p->z - c->pos.z;
    out->z = z;
    f32 rx = z * c->rot.right.z + (x * c->rot.right.x + y * c->rot.right.y);
    f32 ry = z * c->rot.up.z + (x * c->rot.up.x + y * c->rot.up.y);
    f32 rz = z * c->rot.fwd.z + (x * c->rot.fwd.x + y * c->rot.fwd.y);
    out->x = rx;
    out->y = ry;
    out->z = rz;
    f32 depth = lbl_8055FC38 > out->z ? lbl_8055FC38 : out->z;
    f32 k = 1.0f / (lbl_8055FC44 * depth);
    out->x = lbl_8055FC5C + 0.5f * lbl_8055FC64 * (1.0f + k * out->x);
    out->y = lbl_8055FC60 + 0.5f * lbl_8055FC68 * (1.0f + lbl_8055FC6C * (k * -out->y));
}

f32 fn_802C8FEC(void) { return lbl_8055FC6C; }

int fn_802C8FF4(const VecP* p, f32 r) {
    int in = 0;
    if (p->z > lbl_8055FC38 + lbl_80558DC4[0] && p->z < lbl_8055FC3C - lbl_80558DC4[0] && p->x >= lbl_8055FC5C - r &&
        p->x < r + (lbl_8055FC5C + lbl_8055FC64) && p->y >= lbl_8055FC60 - r &&
        p->y < r + (lbl_8055FC60 + lbl_8055FC68)) {
        in = 1;
    }
    return in;
}

int fn_802C90E0(void) { return lbl_8055FC70 == 0; }

void fn_802C90F0(int a) {
    fn_802C8C04(a);
    lbl_8055FC70 = 3;
}

void fn_802C9118(void) {
    lbl_8055FC70 = 2;
    fn_802C8D74();
}

void fn_802C9124(int x0, int y0, int x1, int y1, u32 color, f32 u0, f32 v0, f32 u1, f32 v1) {
    if (fn_801E4F20(4)) {
        fn_801E4F3C(x1, y0, 0.0f, u1, v0, color, 0);
        fn_801E4F3C(x0, y0, 0.0f, u0, v0, color, 0);
        fn_801E4F3C(x1, y1, 0.0f, u1, v1, color, 1);
        fn_801E4F3C(x0, y1, 0.0f, u0, v1, color, 1);
        fn_801E4FF4();
    }
}

void fn_802C9294(int a, int b) {
    if (a != -1 && a != -2) {
        lbl_8055FC75 = b == 1;
        lbl_8055FC74 = 1;
    } else {
        lbl_8055FC74 = 0;
        lbl_8055FC75 = 0;
    }
    fn_802CB29C(a);
}

void fn_802C92D0(void) { fn_802C8C04(-1); }

void fn_802C92D8(void) { fn_802C8D74(); }

void fn_802C92DC(int x0, int y0, int x1, int y1, u32 color) {
    if (fn_801E4F20(4)) {
        fn_801E4F3C(x1, y0, 0.0f, 0.0f, 0.0f, color, 0);
        fn_801E4F3C(x0, y0, 0.0f, 0.0f, 0.0f, color, 0);
        fn_801E4F3C(x1, y1, 0.0f, 0.0f, 0.0f, color, 1);
        fn_801E4F3C(x0, y1, 0.0f, 0.0f, 0.0f, color, 1);
        fn_801E4FF4();
    }
}

void fn_802C942C(u32 color) {
    fn_802C92D0();
    fn_802C92DC(0, 0, lbl_8055CFF0, lbl_8055CFF4, color);
    fn_802C92D8();
}

void fn_802C9474(void) {
    if (lbl_8055FC94) {
        return;
    }
    lbl_8055FC94 = 1;
    fn_802E30C8("%s", "mrFullscreenAdd NOT SUPPORTED!  ANDREW_H IS LAME!");
}

int fn_802C94A0(const VecP* p, u8* visible, f32* depth, f32 r) {
    if (visible) {
        *visible = 1;
    }
    Camera* c = &lbl_804C1410;
    VecP d;
    d.x = p->x - c->pos.x;
    d.y = p->y - c->pos.y;
    d.z = p->z - c->pos.z;
    f32 z = PSVECDotProduct(&d, &c->rot.fwd);
    if (z < lbl_8055FC38 - r || z > lbl_8055FC3C + r) {
        return 0;
    }
    if (z - r > lbl_8055FC3C - 0.15f * lbl_8055FC3C) {
        return 0;
    }
    f32 x = fabsf(PSVECDotProduct(&d, &c->rot.right));
    f32 lim = z * lbl_8055FC44;
    f32 m = Max(0.0f, lim);
    if (x > m + r * lbl_8055FC54) {
        return 0;
    }
    f32 y = fabsf(PSVECDotProduct(&d, &lbl_804C1410.rot.up));
    lim = z * lbl_8055FC50;
    m = Max(0.0f, lim);
    if (y > m + r * lbl_8055FC58) {
        return 0;
    }
    if (depth) {
        *depth = z - r;
    }
    return 1;
}

int fn_802C9638(const VecP* p, u8* visible, f32 r) { return fn_802C94A0(p, visible, NULL, r); }

int fn_802C9640(const VecP* p, f32 r) { return fn_802C9638(p, NULL, r); }

void fn_802C9648(f32 scale) {
    f32 near = lbl_8055FC38;
    f32 far = lbl_8055FC3C;
    lbl_804C144C.near = near;
    lbl_804C144C.far = far;
    lbl_8055FC38 = near * scale;
    lbl_8055FC3C = far * scale;
    fn_802C974C(near * scale, far * scale, lbl_804C144C.fov);
}

void fn_802C967C(void) { fn_802C974C(lbl_804C144C.near, lbl_804C144C.far, lbl_804C144C.fov); }

}  // extern "C"

Perspective::Perspective(f32 near, f32 far, f32 fov) {
    this->near = lbl_8055FC38;
    this->far = lbl_8055FC3C;
    this->fov = lbl_8055FC40;
    fn_802C974C(near, far, fov);
}

Perspective::~Perspective(void) { fn_802C974C(near, far, fov); }

extern "C" {

f32 fn_802C973C(void) { return lbl_8055FC4C; }

f32 fn_802C9744(void) { return lbl_8055FC40; }

void fn_802C974C(f32 near, f32 far, f32 fov) {
    u8 is2D = lbl_8055FC74;
    lbl_8055FC38 = near;
    lbl_8055FC3C = far;
    lbl_8055FC40 = fov;
    lbl_804C144C.fov = fov;
    if (!is2D || lbl_8055FC75) {
        if (!Equal(lbl_8055D020, 1.0f)) {
            fov = 2.0f * (f32)atan((f32)tan(0.5f * fov) / lbl_8055D020);
        }
    }
    lbl_8055FC4C = fov;
    f32 s;
    f32 half = 0.5f * fov;
    s = sin(half);
    f32 t = s / (f32)cos(half);
    lbl_8055FC48 = s;
    lbl_8055FC44 = t;
    lbl_8055FC88 = t;
    f32 a = sin(1.5707964f - fov / 2);
    lbl_8055FC50 = lbl_8055FC44 / lbl_8055FC6C;
    lbl_8055FC54 = 1.0f / a;
    f32 v = atan(lbl_8055FC50);
    lbl_8055FC58 = 1.0f / (f32)sin(1.5707964f - 2.0f * v * 0.5f);
    f32 aspect = is2D ? 1.0f : lbl_8055D020 / lbl_8055D024;
    fn_802D0368(lbl_8055FC38, lbl_8055FC3C, fov, aspect);
    fn_802CC518(lbl_8055FC64, lbl_8055FC44);
}

void fn_802C98DC(f32* tanHalf, f32* near, f32* far, f32* sinHalf) {
    *tanHalf = lbl_8055FC44;
    *near = lbl_8055FC38;
    *far = lbl_8055FC3C;
    *sinHalf = lbl_8055FC48;
}

void fn_802C9900(f32 a) { fn_802D007C(a); }

void fn_802C9904(void) { fn_802D007C(1.0f); }

u32 fn_802C990C(u32 color) {
    u32 old = lbl_8055FC78;
    lbl_8055FC78 = color;
    fn_802CFFAC((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    return old;
}

u32 fn_802C9950(u32 color) {
    u32 old = lbl_8055FC7C;
    lbl_8055FC7C = color;
    fn_802D0160((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    return old;
}

u32 fn_802C9994(u32 color) {
    u32 old = lbl_8055FC80;
    lbl_8055FC80 = color;
    fn_802D0188((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    return old;
}

void fn_802C99D8(void) { fn_802D01B0(lbl_8055D01C, &lbl_804C1440); }

f32 fn_802C99E8(f32 v) {
    f32 old = lbl_8055D01C;
    lbl_8055D01C = v;
    fn_802C99D8();
    return old;
}

void fn_802C9A1C(const Vec* dir) {
    lbl_804C1440 = *dir;
    fn_802C99D8();
}

Vec* fn_802C9A40(void) { return &lbl_804C1440; }

void fn_802C9A4C(void) { fn_802D93E0(); }

f32 fn_802C9A50(void) { return lbl_8055D020; }

u8 fn_802C9A58(void) { return lbl_8055FC8C; }

u8 fn_802C9A60(void) { return lbl_8055FC8D; }

u8 fn_802C9A68(void) { return lbl_8055FC8E; }

void fn_802C9A70(int bars) {
    lbl_8055FC74 = 1;
    if (lbl_8055FC8D) {
        fn_802C87B4(0, 0, lbl_8055CFF0, lbl_8055CFF4);
        fn_802C88BC(2);
        fn_802C974C(lbl_8055FC38, lbl_8055FC3C, 1.5707964f);
    }
    if ((lbl_8055FC8C || lbl_8055FC8D) && bars) {
        int side = (lbl_8055CFF0 - (int)(0.5f + lbl_8055CFF0 * lbl_8055D020)) / 2;
        if (side > 1) {
            fn_802C92D0();
            fn_802C92DC(0, 0, side, lbl_8055CFF4, 0xFF000000);
            fn_802C92DC(lbl_8055CFF0 - side, 0, lbl_8055CFF0, lbl_8055CFF4, 0xFF000000);
            fn_802C92D8();
        }
    }
    if (lbl_8055FC8D) {
        int bar = (lbl_8055CFF4 - (int)(0.5f + lbl_8055CFF4 * lbl_8055D024)) / 2;
        if (bar > 1) {
            fn_802C92D0();
            fn_802C92DC(0, 0, lbl_8055CFF0, bar, 0xFF000000);
            fn_802C92DC(0, lbl_8055CFF4 - bar, lbl_8055CFF0, lbl_8055CFF4, 0xFF000000);
            fn_802C92D8();
        }
    }
    lbl_8055FC74 = 0;
}

struct ImageData {
    u8 pad[4];
    u8** data;  // offset 0x4
};

struct ImageRef {
    u8 pad[8];
    ImageData* img;  // offset 0x8
};

u8* fn_802C9C24(ImageRef* r) { return *r->img->data + 4; }

s32 fn_802C9C38(void) { return lbl_8055FC68; }

s32 fn_802C9C40(void) { return lbl_8055FC60; }

s32 fn_802C9C48(void) { return lbl_8055FC64; }

s32 fn_802C9C50(void) { return lbl_8055FC5C; }

s32 fn_802C9C58(s32 mode) {
    s32 old = lbl_8055FC30;
    lbl_8055FC30 = mode;
    fn_802D08CC(mode);
    return old;
}

int fn_802C9C8C(int a) {
    fn_802CAD68();
    if (fn_802D31D8(1)) {
        lbl_8055FC84 = a;
        lbl_8055FC70 = 4;
        return 1;
    }
    return 0;
}

void fn_802C9CE0(void) {
    fn_802D362C(lbl_8055FC84);
    fn_802D3304();
    lbl_8055FC70 = 1;
}

}  // extern "C"

void* Flare::operator new(size_t size, const char* tag) {
    if (lbl_8055FC98 == 0) {
        lbl_8055FC9C = new ("DrawListWrapper::pool") Pool("DrawListWrapper::pool", 0x10, 0x18);
    }
    void* p = fn_8012A2D8(lbl_8055FC9C);
    if (p) {
        lbl_8055FC98++;
    }
    return p;
}

void Flare::operator delete(void* p) {
    fn_8012A364(lbl_8055FC9C, p);
    if (--lbl_8055FC98 == 0) {
        delete lbl_8055FC9C;
        lbl_8055FC9C = NULL;
    }
}

Flare::Flare(int id) {
    fx = fn_801E7840(id);
    pos.x = 0.0f;
    pos.y = 0.0f;
    pos.z = 0.0f;
    radius = 0.0f;
    count = 0;
}

Flare::~Flare(void) { fn_801E7888(fx); }

void Flare::Draw(int a, int b, f32 x, f32 y, f32 z, f32 s, f32 t) {
    if (count == 0) {
        pos.x = x;
        pos.y = y;
        pos.z = z;
        radius = 0.0f;
    } else {
        VecP p;
        p.x = x;
        p.y = y;
        p.z = z;
        f32 dist = PSVECDistance(&pos, &p);
        if (dist > radius) {
            VecP d;
            d.x = p.x - pos.x;
            d.y = p.y - pos.y;
            d.z = p.z - pos.z;
            PSVECNormalize(&d, &d);
            f32 grow = 0.5f * (dist - radius);
            pos.x += d.x * grow;
            pos.y += d.y * grow;
            radius = 0.5f * (radius + dist);
            pos.z += d.z * grow;
        }
    }
    fn_801E7890(fx, a, b, x, y, z, s, t);
    count++;
}

void Flare::Done(void) {
    if (fn_802C9640(&pos, radius)) {
        fn_801E7894(fx);
    }
}

extern "C" {

Flare* fn_802CA04C(int id) { return new ("mrDrawList") Flare(id); }

void fn_802CA094(Flare* f) { delete f; }

void fn_802CA09C(Flare* f, int a, int b, f32 x, f32 y, f32 z, f32 s, f32 t) { f->Draw(a, b, x, y, z, s, t); }

void fn_802CA0A0(Flare* f) { f->Done(); }

}  // extern "C"

// NonMatching: fn_802C83CC (stfsu addressing), fn_802C8E70 and Flare::Draw (register numbering) and
// fn_802C974C (the aspect check branches through mfcr in ours) still differ.
