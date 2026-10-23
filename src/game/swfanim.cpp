// SW2 vector animations (0x80238BF4-0x8023995C): a "SWF2" resource of textured quads/triangles
// ("shapes"), colour transforms and frames of placed items (2x3 matrix + shape + colour transform),
// played back at the file's frame rate and drawn in 640x480 screen space. Names are guesses apart
// from the strings.
#include "game/common.h"

extern "C" {
void* fn_8012C370(const char* name, u32 type, u32* version, u32* size, u8* first, u8* swapped);
void fn_8012C4D8(void* res);
u32 fn_802E2C20(void);                   // milliseconds
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal
s32 fn_802CAC28(const char* name, int a);
void fn_802CAC54(s32 tex);
s32 fn_8022EDC4(void);
}

struct SwfVtx {
    f32 data[6];
};

extern "C" {
void fn_80231380(SwfVtx* v, u32 color, f32 x, f32 y, f32 z, f32 u, f32 w);
void fn_802C8B98(s32 tex, s32 a);
void fn_802C8DB8(SwfVtx* v, int nv, const u16* idx, int ni);
void fn_802C8D74(void);
}

struct Sw2Texture {
    s32 handle;     // offset 0x0
    char name[16];  // offset 0x4
};

struct Sw2Shape {
    u16 tex;        // offset 0x0
    u8 numPts;      // offset 0x2
    u8 pad;         // offset 0x3
    u32 color;      // offset 0x4
    f32 pts[4][2];  // offset 0x8
};

struct Sw2Cxform {
    s8 mul[4];  // offset 0x0
    s8 add[4];  // offset 0x4
};

struct Sw2Item {
    u16 shape;  // offset 0x0
    u16 cxform; // offset 0x2
    f32 m[6];   // offset 0x4
};

struct Sw2Frame {
    u16 count;      // offset 0x0
    void* extra;    // offset 0x4
    Sw2Item* items; // offset 0x8
};

struct Sw2 {
    u32 fps;                // offset 0x0
    u32 numFrames;          // offset 0x4
    u32 numTextures;        // offset 0x8
    u32 numShapes;          // offset 0xC
    u32 unk10;              // offset 0x10
    u32 unk14;              // offset 0x14
    Sw2Texture* textures;   // offset 0x18
    Sw2Shape* shapes;       // offset 0x1C
    Sw2Cxform* cxforms;     // offset 0x20
    Sw2Frame* frames;       // offset 0x24
};

class SwfAnim {
public:
    SwfAnim();
    ~SwfAnim();
    void DrawItem(const Sw2Item* it);
    void DrawFrame(u32 frame);
    void Reset();
    void LoadFile(const char* name);
    void Update();
    void Draw();
    void Load(const char* name);
    void Unload();

    u32 lastTime;  // offset 0x0
    u8 playing;    // offset 0x4
    u8 loop;       // offset 0x5
    u32 frame;     // offset 0x8
    u32 first;     // offset 0xC
    u32 last;      // offset 0x10
    Sw2* data;     // offset 0x14
};

struct SwfState {
    u32 a;
    u32 b;
    u32 c;
    u32 d;
    u32 e;
    u32 f;
    SwfState() : a(0), b(123), c(0), d(0), e(0) {}
};

static inline u32 SwfColor(u8 r, u8 g, u8 b, u8 a) { return (a << 24) | (r << 16) | (g << 8) | b; }

ModuleTag lbl_8055F0E0;
SwfState lbl_804986C8;
u32 lbl_8055F0E8 = SwfColor(0, 0, 0, 0x7F);

static inline void SwfSwap16(void* p) {
    u8 t;
    t = ((u8*)p)[0];
    ((u8*)p)[0] = ((u8*)p)[1];
    ((u8*)p)[1] = t;
}

static inline void SwfSwap32(void* p) {
    u8 t;
    t = ((u8*)p)[0];
    ((u8*)p)[0] = ((u8*)p)[3];
    ((u8*)p)[3] = t;
    t = ((u8*)p)[1];
    ((u8*)p)[1] = ((u8*)p)[2];
    ((u8*)p)[2] = t;
}

static inline int SwfChannel(int c, s8 mul, s8 add) {
    int v = mul * c / 255 + add * 2;
    v = v > 0 ? v : 0;
    v = v < 255 ? v : 255;
    return v;
}

struct SwfPt {
    f32 x, y, z;
};

void SwfAnim::DrawItem(const Sw2Item* it) {
    SwfPt pts[4];
    SwfVtx v[4] __attribute__((aligned(16)));
    Sw2Shape* sh = &data->shapes[it->shape];
    const Sw2Cxform* cx = &data->cxforms[it->cxform];
    for (int i = 0; i < sh->numPts; i++) {
        f32 x = sh->pts[i][0];
        f32 y = sh->pts[i][1];
        pts[i].x = x * it->m[0] + y * it->m[2] + it->m[4];
        pts[i].y = x * it->m[1] + y * it->m[3] + it->m[5];
        pts[i].z = 0.0f;
        pts[i].x -= 320.0f;
        pts[i].y = -(pts[i].y - 240.0f);
    }
    u32 c = sh->color;
    u8 ca = (c >> 24) & 0xFF;
    u8 cr = (c >> 16) & 0xFF;
    u8 cg = (c >> 8) & 0xFF;
    u8 cb = c & 0xFF;
    int a = SwfChannel(ca << 1, cx->mul[3], cx->add[3]);
    int r = SwfChannel(cr << 1, cx->mul[0], cx->add[0]);
    int g = SwfChannel(cg << 1, cx->mul[1], cx->add[1]);
    int b = SwfChannel(cb << 1, cx->mul[2], cx->add[2]);
    u32 color = SwfColor(r, g, b, a);
    s32 ctx = fn_8022EDC4();
    fn_80231380(&v[0], color, pts[0].x, pts[0].y, 0.0f, 0.0f, 0.0f);
    fn_80231380(&v[1], color, pts[1].x, pts[1].y, 0.0f, 1.0f, 0.0f);
    fn_80231380(&v[2], color, pts[2].x, pts[2].y, 0.0f, 0.0f, 1.0f);
    if (sh->numPts == 4) {
        fn_80231380(&v[3], color, pts[3].x, pts[3].y, 0.0f, 1.0f, 1.0f);
    }
    u16 quad[6] = {0, 1, 3, 0, 3, 2};
    u16 tri[3] = {0, 1, 2};
    fn_802C8B98(data->textures[sh->tex].handle, ctx);
    if (sh->numPts == 4) {
        fn_802C8DB8(v, 4, quad, 6);
    } else {
        fn_802C8DB8(v, 3, tri, 3);
    }
    fn_802C8D74();
}

void SwfAnim::DrawFrame(u32 frame) {
    frame %= data->numFrames;
    Sw2Frame* f = &data->frames[frame];
    for (int i = 0; i < f->count; i++) {
        DrawItem(&f->items[i]);
    }
}

void SwfAnim::Reset() {
    data = 0;
    this->frame = 0;
    playing = 1;
    loop = 1;
    lastTime = fn_802E2C20();
    last = 0;
    first = 0;
}

void SwfAnim::LoadFile(const char* name) {
    u8* base;
    u32 size = 0;
    u8 firstLoad = 0;
    u8 swapped = 0;
    u32 ver = 1;
    Sw2* res = (Sw2*)fn_8012C370(name, 'SWF2', &ver, &size, &firstLoad, &swapped);
    data = res;
    if (!res) {
        fn_802E30C8("SWF: ERROR! Could not load SW2 animation %s!", name);
        return;
    }
    if (ver != 1) {
        fn_802E317C("SWF: Wrong sw2 version! Expected %u, got %u", 1, ver);
    }
    if (!swapped) {
        return;
    }
    SwfSwap32(&data->fps);
    base = (u8*)data;
    SwfSwap32(&data->numFrames);
    SwfSwap32(&data->numTextures);
    SwfSwap32(&data->numShapes);
    SwfSwap32(&data->unk10);
    SwfSwap32(&data->unk14);
    SwfSwap32(&data->textures);
    SwfSwap32(&data->shapes);
    SwfSwap32(&data->cxforms);
    SwfSwap32(&data->frames);
    data->textures = (Sw2Texture*)(base + (u32)data->textures);
    data->shapes = (Sw2Shape*)(base + (u32)data->shapes);
    data->cxforms = (Sw2Cxform*)(base + (u32)data->cxforms);
    data->frames = (Sw2Frame*)(base + (u32)data->frames);
    u32 i;
    for (i = 0; i < data->numFrames; i++) {
        data->frames[i].items = (Sw2Item*)(base + (u32)data->frames[i].items);
        if (data->frames[i].extra) {
            data->frames[i].extra = base + (u32)data->frames[i].extra;
        }
    }
    for (i = 0; i < data->numTextures; i++) {
        SwfSwap32(&data->textures[i].handle);
    }
    i = 0;
    if (data->numShapes != 0) {
        for (; i < data->numShapes; i++) {
            SwfSwap16(&data->shapes[i].tex);
            SwfSwap32(&data->shapes[i].color);
            for (int j = 0; j < 4; j++) {
                SwfSwap32(&data->shapes[i].pts[j][0]);
                SwfSwap32(&data->shapes[i].pts[j][1]);
            }
        }
    }
    for (i = 0; i < data->numFrames; i++) {
        Sw2Frame* f = &data->frames[i];
        SwfSwap16(&f->count);
        for (int j = 0; j < f->count; j++) {
            Sw2Item* it = &f->items[j];
            SwfSwap16(&it->shape);
            SwfSwap16(&it->cxform);
            SwfSwap32(&it->m[0]);
            SwfSwap32(&it->m[1]);
            SwfSwap32(&it->m[2]);
            SwfSwap32(&it->m[3]);
            SwfSwap32(&it->m[4]);
            SwfSwap32(&it->m[5]);
        }
    }
}

SwfAnim::SwfAnim() { Reset(); }

SwfAnim::~SwfAnim() {}

void SwfAnim::Update() {
    if (data && playing) {
        u32 now = fn_802E2C20();
        if (now - lastTime >= 1000 / data->fps) {
            frame++;
            if (frame > last) {
                if (loop) {
                    frame = first;
                } else {
                    frame = last;
                }
            } else if (frame < first) {
                frame = first;
            }
            lastTime = now;
        }
    }
}

void SwfAnim::Draw() {
    if (data) {
        DrawFrame(frame);
    }
}

void SwfAnim::Load(const char* name) {
    Reset();
    LoadFile(name);
    if (!data) {
        fn_802E30C8("SWF: Error loading SW2 animation %s!", name);
        return;
    }
    first = 0;
    last = data->numFrames - 1;
    for (u32 i = 0; i < data->numTextures; i++) {
        if (data->textures[i].name[0]) {
            data->textures[i].handle = fn_802CAC28(data->textures[i].name, 0);
        } else {
            data->textures[i].handle = -1;
        }
    }
}

void SwfAnim::Unload() {
    if (data) {
        for (u32 i = 0; i < data->numTextures; i++) {
            if (data->textures[i].handle != -2) {
                fn_802CAC54(data->textures[i].handle);
                data->textures[i].handle = -2;
            }
        }
        fn_8012C4D8(data);
        data = 0;
    }
}
