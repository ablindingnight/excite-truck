// Cloud/environment map (0x80202BE4-0x80203FE0): a 256x256 render target redrawn every other frame with
// the "imap%d.tex" sky band chosen (and blended) by the camera's elevation, a rotated fan of the
// "cenvmap.tex"/"shwenv.tex" texture, and, in mode 2, the surrounding terrain silhouette and objects
// projected onto the same hemisphere map. Names are guesses apart from the strings.
#include "game/common.h"

struct EVec3 {
    f32 x, y, z;
};

struct EVec2 {
    f32 x, y;
};

struct EFrame {
    EVec3 x;    // offset 0x0
    EVec3 y;    // offset 0xC
    EVec3 z;    // offset 0x18
    EVec3 pos;  // offset 0x24
};

struct EVtx {
    f32 x, y, z;  // offset 0x0
    u32 color;    // offset 0xC
    f32 u, v;     // offset 0x10
};

// A sky band texture and the elevation (degrees) it is drawn at.
struct EnvTex {
    s32 tex;  // offset 0x0
    s32 idx;  // offset 0x4
};

template <class T>
struct TArray {
    TArray(T* d, s32 c) {
        data = d;
        cap = c;
        n = 0;
    }
    ~TArray(void) {
        data = 0;
        n = 0;
        cap = 0;
    }
    void Push(const T& v) { data[n++] = v; }
    T* data;  // offset 0x0
    s32 n;    // offset 0x4
    s32 cap;  // offset 0x8
};

template <class T, int N>
struct TFixed : TArray<T> {
    TFixed(void) : TArray<T>(items, N) {}
    T items[N];  // offset 0xC
};

extern "C" {
void PSVECCrossProduct(const EVec3* a, const EVec3* b, EVec3* out);
f32 PSVECMag(const EVec3* v);
double asin(double x);
double atan2(double y, double x);
double cos(double x);
double sin(double x);
double sqrt(double x);
int fn_8012C33C(const char* name);
int fn_801345D8(char* buf, int size, const char* fmt, ...);  // snprintf
f32 fn_801DCF48(int a, f32 x, f32 z);                          // terrain height
void fn_802BE07C(const EVec3* pos, void (*cb)(s32, EVec3*, int, int, f32, f32));
void fn_802C8638(const EFrame* cam);
void fn_802C86E8(u32 color);
void fn_802C87B4(int x, int y, int w, int h);
void fn_802C88BC(int a);
void fn_802C8B98(s32 tex, const EFrame* m);
void fn_802C8BD0(s32 tex);
void fn_802C8C04(s32 tex);
void fn_802C8D74(void);
void fn_802C8DB8(EVtx* v, int nv, const u16* idx, int ni);
int fn_802C8DBC(int n);
void fn_802C8E00(u32 color, int a, f32 x, f32 y, f32 z, f32 u, f32 v);
void fn_802C8E48(void);
void fn_802C9294(s32 rt, int a);
void fn_802C974C(f32 a, f32 b, f32 c);
EVec3* fn_802C9A40(void);
void* fn_802CA294(s32 tex);
s32 fn_802CAC28(const char* name, int a);
void fn_802CAC54(s32 tex);
void fn_802CAE98(s32 tex, const char* name);
void fn_802CAF1C(s32 tex, int a, int b);
s32 fn_802CB128(int w, int h, int a, int b);
void fn_802CB228(s32 rt);
void fn_802CB274(s32 rt);
void fn_802CB288(s32 rt);
void fn_802D050C(void* tex);
void fn_802D3050(int a);
void fn_802D306C(int a);
void fn_802D30DC(void* tex);
void fn_802D3ECC(u16 a);
void fn_802D9230(void* tex);
void fn_802D94E4(void* dst, void* a, void* b, f32 t);
}

static inline u16 EnvShort(int v) { return v; }
static inline u32 EnvColor(u8 a, u8 r, u8 g, u8 b) { return (a << 24) | (r << 16) | (g << 8) | b; }

ModuleTag lbl_8055ECE8;
u16 lbl_8055ECF0 = EnvShort(0x100);
u16 lbl_8055ECF2 = EnvShort(0x405);
u16 lbl_8055ECF4 = EnvShort(0x401);
u32 lbl_8055ECF8 = EnvColor(0xFF, 0xFF, 0, 0);
s32 lbl_8055ECFC;  // render target
s32 lbl_8055ED00;  // cenvmap.tex / shwenv.tex
s32 lbl_8055ED04;  // ambmap.tex
bool lbl_8055ED08;  // draw this frame
s32 lbl_8055ED0C;  // imap.tex (blend target)
u8 lbl_8055ED10;
s32 lbl_8055ED14;  // mode
u8 lbl_8055ED18;
s32 lbl_8055ED1C;  // current object texture

TFixed<EnvTex, 64> lbl_8048897C;
EFrame lbl_80488B88;

static inline f32 EnvLerp(f32 lo, f32 span, f32 t) { return lo + t * span; }

// Horizontal part of v, scaled.
static inline void Flatten(EVec3* o, const EVec3* v, f32 k) {
    o->x = v->x * k;
    o->z = v->z * k;
    o->y = 0.0f;
}

static inline void Rotate(EVec2* p, f32 a) {
    f32 c = cos(a);
    f32 s = sin(a);
    f32 x = p->x;
    p->x = x * c - p->y * s;
    p->y = x * s + p->y * c;
}

extern "C" {

int fn_80202BE4(void) { return lbl_8055ED14 != 2; }

void fn_80202BFC(s32 rt) {
    s32 tex[2] = {-1, rt};
    u32 colors[2] = {0, 0xFFFFFFFF};
    for (u32 i = 0; i < 2; i++) {
        fn_802C8C04(tex[i]);
        u16 idx[6] = {0, 1, 2, 1, 2, 3};
        EVtx v[4];
        v[0].x = 50.0f;
        v[0].y = 50.0f;
        v[0].z = 0.0f;
        v[1].x = 306.0f;
        v[1].y = 50.0f;
        v[1].z = 0.0f;
        v[2].x = 50.0f;
        v[2].y = 306.0f;
        v[2].z = 0.0f;
        v[3].x = 306.0f;
        v[3].y = 306.0f;
        v[3].z = 0.0f;
        v[2].u = 0.0f;
        v[0].u = 0.0f;
        v[3].u = 1.0f;
        v[1].u = 1.0f;
        v[1].v = 0.0f;
        v[0].v = 0.0f;
        v[3].v = 1.0f;
        v[2].v = 1.0f;
        v[3].color = colors[i];
        v[2].color = colors[i];
        v[1].color = colors[i];
        v[0].color = colors[i];
        fn_802C8DB8(v, 4, idx, 6);
        fn_802C8D74();
    }
}

// Picks (and blends) the sky band for a view direction.
void fn_80202D50(EVec3 dir) {
    f32 m = PSVECMag(&dir);
    m = m > 0.001f ? m : 0.001f;
    f32 s = 1.0f / m;
    dir.y *= s;
    dir.x *= s;
    dir.z *= s;
    f32 y = -dir.y;
    f32 elev = 180.0f * (f32)asin(0.0f > y ? 0.0f : y) / 3.1415927f;
    void* result = 0;
    EnvTex* e = lbl_8048897C.data + 1;
    for (int i = 1; i < lbl_8048897C.n; i++, e++) {
        if (e->idx > elev) {
            EnvTex* a = &lbl_8048897C.data[i - 1];
            EnvTex* b = &lbl_8048897C.data[i];
            f32 t = (elev - (f32)a->idx) / ((f32)b->idx - (f32)a->idx);
            t = 1.0f < t ? 1.0f : t;
            t = t > 0.0f ? t : 0.0f;
            t = EnvLerp(0.0f, 1.0f, t);
            void* tb = fn_802CA294(b->tex);
            void* ta = fn_802CA294(a->tex);
            fn_802D94E4(fn_802CA294(lbl_8055ED0C), ta, tb, t);
            result = fn_802CA294(lbl_8055ED0C);
            break;
        }
    }
    if (!result) {
        result = fn_802CA294(lbl_8048897C.data[lbl_8048897C.n - 1].tex);
    }
    fn_802D30DC(result);
}

void fn_80202F58(const EVec3* v, const EFrame* f, EFrame* out) {
    if ((f32)__fabs(f->z.y) < 0.999f) {
        EVec3 d;
        EFrame m;
        d.x = v->x - f->pos.x;
        d.y = v->y - f->pos.y;
        d.z = v->z - f->pos.z;
        m.pos.x = 0.0f;
        m.pos.y = 0.0f;
        m.pos.z = 0.0f;
        Flatten(&m.z, &d, -1.0f);
        f32 len = PSVECMag(&m.z);
        len = 0.001f > len ? 0.001f : len;
        f32 s = 1.0f / len;
        m.z.x *= s;
        m.z.y *= s;
        m.z.z *= s;
        m.y.x = 0.0f;
        m.y.y = 1.0f;
        m.y.z = 0.0f;
        fn_80202D50(d);
        PSVECCrossProduct(&m.y, &m.z, &m.x);
        fn_802C8B98(lbl_8055ED00, &m);
        *out = m;
        out->x.x *= -1.0f;
        out->x.y *= -1.0f;
        out->x.z *= -1.0f;
    } else {
        EVec3 up;
        up.x = 0.0f;
        up.y = 0.0f;
        up.z = 1.0f;
        fn_80202D50(up);
        fn_802C8BD0(lbl_8055ED00);
        out->x.y = 0.0f;
        out->x.x = 1.0f;
        out->x.z = 0.0f;
        out->y.x = 0.0f;
        out->y.y = 1.0f;
        out->y.z = 0.0f;
        out->z.x = 0.0f;
        out->z.y = 0.0f;
        out->z.z = 1.0f;
        out->pos.x = 0.0f;
        out->pos.y = 0.0f;
        out->pos.z = 0.0f;
    }
    if (fn_802C8DBC(4)) {
        EVec3* c = fn_802C9A40();
        EVec2 h;
        h.x = c->x;
        h.y = c->z;
        f32 len = sqrt(h.x * h.x + h.y * h.y);
        len = 0.01f > len ? 0.01f : len;
        f32 s = 1.0f / len;
        h.x = h.x * s * -1.0f;
        h.y = h.y * s * -1.0f;
        f32 a = atan2(-h.x, h.y);
        EVec2 p;
        p.x = -181.01926f;
        p.y = 181.01926f;
        f32 na = -a;
        Rotate(&p, na);
        fn_802C8E00(-1, 0, p.x, 0.0f, p.y, -0.20710647f, -0.20710647f);
        Rotate(&p, -1.5707963705062866);
        fn_802C8E00(-1, 0, p.x, 0.0f, p.y, 1.2071065f, -0.20710647f);
        p.x *= -1.0f;
        p.y *= -1.0f;
        fn_802C8E00(-1, 1, p.x, 0.0f, p.y, -0.20710647f, 1.2071065f);
        Rotate(&p, 1.5707963705062866);
        fn_802C8E00(-1, 1, p.x, 0.0f, p.y, 1.2071065f, 1.2071065f);
        fn_802C8E48();
    }
    fn_802C8D74();
}

#define ENV_PROJECT(m, w, d, sx, sy)                                      d.x = (w).x - (m)->pos.x;                                             d.y = (w).y - (m)->pos.y;                                             d.z = (w).z - (m)->pos.z;                                             {                                                                         f32 x = d.x * (m)->x.x + d.y * (m)->x.y + d.z * (m)->x.z;             f32 y = d.x * (m)->y.x + d.y * (m)->y.y + d.z * (m)->y.z;             f32 z = d.x * (m)->z.x + d.y * (m)->z.y + d.z * (m)->z.z;             d.x = x;                                                              d.y = y;                                                              d.z = z;                                                          }                                                                     {                                                                         f32 len = PSVECMag(&d);                                               if (0.01f > len) {                                                        len = 0.01f;                                                      }                                                                     f32 s = 1.0f / len;                                                   d.x *= s;                                                             d.y *= s;                                                             d.z *= s;                                                         }                                                                     sx = 73.98844f * d.x / (1.0f + d.y);                                  sy = 73.98844f * d.z / (1.0f + d.y);

// Terrain silhouette rings around the camera.
void fn_80203380(const EFrame* m) {
    fn_802C8BD0(-1);
    f32 rings[4] = {5.0f, 30.0f, 70.0f, 120.0f};
    fn_802C8DBC(0xA0);
    for (u32 r = 0; r < 4; r++) {
        f32 dist = rings[r];
        for (int i = 0; i < 20; i++) {
            EVec2 p;
            EVec3 w;
            f32 sx, sy;
            p.x = 0.0f;
            p.y = dist;
            f32 a = 3.1415927f * (i * 2) / 20.0f;
            Rotate(&p, a);
            p.x += m->pos.x;
            p.y += m->pos.z;
            w.y = fn_801DCF48(0, p.x, p.y);
            w.x = p.x;
            w.z = p.y;
            {
                EVec3 d;
                ENV_PROJECT(m, w, d, sx, sy);
            }
            fn_802C8E00(0xFF000000, 1, sx, 0.0f, sy, 0.0f, 0.0f);
            w.y = -200.0f;
            {
                EVec3 d;
                ENV_PROJECT(m, w, d, sx, sy);
            }
            fn_802C8E00(0xFF000000, 1, sx, 0.0f, sy, 0.0f, 0.0f);
        }
    }
    fn_802C8E48();
    fn_802C8D74();
}

// Object silhouettes: a camera-facing w x h quad over each of the n points.
void fn_802036E4(s32 tex, EVec3* pts, int stride, int n, f32 w, f32 h) {
    if (tex != lbl_8055ED1C) {
        lbl_8055ED1C = tex;
        fn_802C8D74();
        fn_802C8BD0(lbl_8055ED1C);
        fn_802D3ECC(lbl_8055ECF0);
    }
    if (fn_802C8DBC(n * 8)) {
        EFrame* m = &lbl_80488B88;
        for (int i = 0; i < n; i++) {
            EVec3* p = (EVec3*)((u8*)pts + i * stride);
            EVec3 d;
            d.x = p->x - m->pos.x;
            d.y = p->y - m->pos.y;
            d.z = p->z - m->pos.z;
            EVec2 side;
            side.x = d.z;
            side.y = -d.x;
            f32 len = sqrt(d.z * d.z + side.y * side.y);
            if (len > 0.1f) {
                f32 s = 1.0f / len;
                side.x *= s;
                side.y *= s;
                for (int k = 0; k < 4; k++) {
                    EVec3 q = *p;
                    f32 sx, sy;
                    if (k & 1) {
                        q.x += side.x * w;
                        q.z += side.y * w;
                    } else {
                        q.x -= side.x * w;
                        q.z -= side.y * w;
                    }
                    if (k & 2) {
                        q.y += h;
                    }
                    EVec3 e;
                    ENV_PROJECT(m, q, e, sx, sy);
                    fn_802C8E00(0xFF404040, k >= 2, sx, 0.0f, sy, (k & 2) ? 1.0f : 0.0f, (k & 1) ? 1.0f : 0.0f);
                }
            }
        }
        fn_802C8E48();
    }
}

void fn_802039CC(const EFrame* cam, const EFrame* m) {
    lbl_8055ED1C = -1;
    fn_802C8BD0(-1);
    lbl_80488B88 = *m;
    fn_802BE07C(&cam->pos, fn_802036E4);
    fn_802C8D74();
}

void fn_80203A88(const EVec3* v, const EFrame* cam, EFrame* m) {
    if (fn_80202BE4() == 0) {
        fn_802D3050(1);
        fn_802D306C(90);
        m->pos.x = v->x;
        m->pos.z = v->z;
        m->pos.y = v->y + 0.5f;
        fn_802039CC(cam, m);
        fn_80203380(m);
        fn_802D306C(0);
    }
}

void fn_80203B20(const EFrame* cam, const EVec3* v, int objects) {
    fn_802CB274(lbl_8055ECFC);
    if (lbl_8055ED08) {
        fn_802C9294(lbl_8055ECFC, 0);
        EFrame view;
        view.pos.x = 0.0f;
        view.pos.y = 128.0f;
        view.pos.z = 0.0f;
        view.x.x = 1.0f;
        view.x.y = 0.0f;
        view.x.z = 0.0f;
        view.y.x = 0.0f;
        view.y.y = 0.0f;
        view.y.z = 1.0f;
        view.z.x = 0.0f;
        view.z.y = -1.0f;
        view.z.z = 0.0f;
        fn_802C8638(&view);
        fn_802C87B4(0, 0, 0x100, 0x100);
        fn_802C974C(1.0f, 512.0f, 1.5707964f);
        fn_802C86E8(0xFF00FF00);
        fn_802C88BC(-1);
        EFrame out;
        fn_80202F58(v, cam, &out);
        if (fn_80202BE4() == 0 && objects) {
            fn_80203A88(v, cam, &out);
        }
        fn_802C9294(-1, 0);
    }
    lbl_8055ED08 = lbl_8055ED08 == 0;
}

void fn_80203C50(int mode) {
    char buf[32];
    lbl_8055ED14 = mode;
    lbl_8055ED18 = 1;
    const char* name = "cenvmap.tex";
    const char* tn = name;
    if (mode == 1) {
        tn = "shwenv.tex";
    }
    lbl_8055ED00 = fn_802CAC28(tn, 0);
    lbl_8055ED04 = fn_802CAC28("ambmap.tex", 0);
    lbl_8055ED0C = fn_802CAC28("imap.tex", 0);
    for (int i = 0; i < 90; i++) {
        fn_801345D8(buf, 32, "imap%d.tex", i);
        if (fn_8012C33C(buf)) {
            EnvTex e;
            e.tex = fn_802CAC28(buf, 0);
            e.idx = i;
            lbl_8048897C.Push(e);
        }
    }
    lbl_8055ECFC = fn_802CB128(0x100, 0x100, 0, 1);
    fn_802CAE98(lbl_8055ECFC, name);
    fn_802CAE98(lbl_8055ED00, "bad*name.tex");
    fn_802CAF1C(lbl_8055ECFC, 6, 0x50);
    fn_80202BE4();
    lbl_8055ED08 = 1;
}

void fn_80203D84(s32* tex) {
    if (*tex != -2) {
        fn_802CAC54(*tex);
        *tex = -2;
    }
}

void fn_80203DC8(void) {
    lbl_8055ED18 = 0;
    if (lbl_8055ECFC != -2) {
        fn_802CB228(lbl_8055ECFC);
        lbl_8055ECFC = -2;
    }
    fn_80203D84(&lbl_8055ED00);
    fn_80203D84(&lbl_8055ED04);
    fn_80203D84(&lbl_8055ED0C);
    fn_802D050C(0);
    fn_802D30DC(0);
    for (int i = 0; i < lbl_8048897C.n; i++) {
        fn_80203D84(&lbl_8048897C.data[i].tex);
    }
    lbl_8048897C.n = 0;
}

void fn_80203E88(const EFrame* cam, const EVec3* v, int objects) {
    fn_80203B20(cam, v, objects);
    fn_802D9230(fn_802CA294(lbl_8055ECFC));
    fn_802D050C(fn_802CA294(lbl_8055ED04));
}

void fn_80203EC0(void) {}

void fn_80203EC4(void) {
    if (fn_80202BE4() == 0) {
        fn_802CB288(lbl_8055ECFC);
    }
}

void fn_80203EF4(void) {
    if (lbl_8055ED10) {
        fn_80202BFC(lbl_8055ECFC);
    }
}

void fn_80203F0C(void) {}

}  // extern "C"
