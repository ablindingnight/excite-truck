// Per-vehicle state blocks (0x802795E8-0x80279CE4): a 164-entry colour table that fades back to
// neutral, a timer and a flag block, looked up by vehicle id. Names are guesses apart from the
// allocation tag and message.
#include "game/common.h"
#include "macros.h"

extern "C" {
void* memset(void* p, int c, size_t n);
void fn_802E30C8(const char* fmt, ...);  // debug print
}

void* operator new[](size_t size, const char* tag);

struct CarColors {
    u8 c[164][4];
};

struct CarFlags {
    u8 f[18];
};

struct CarState {
    CarColors col;   // offset 0x0
    f32 time;        // offset 0x290
    f32 duration;    // offset 0x294
    s32 x298;        // offset 0x298
    s32 x29C;        // offset 0x29C
    CarFlags flags;  // offset 0x2A0
};

struct CarStateBlock {
    s32 id;          // offset 0x0
    CarState state;  // offset 0x4
};

static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

#define ROUND(x) ((x) < 0.0f ? (x) - 0.5f : (x) + 0.5f)

ModuleTag lbl_8055F610;
CarStateBlock* lbl_8055F618;
s32 lbl_8055F61C;

extern "C" {

void fn_802795E8(CarState* s) {
    int i;
    memset(s, 0, 0x2B4);
    s->x298 = 0;
    s->time = 0.0f;
    s->duration = 0.0f;
    for (i = 0; i < 164; i++) {
        s->col.c[i][0] = 0x80;
        s->col.c[i][1] = 0x80;
        s->col.c[i][2] = 0x80;
    }
}

void fn_8027974C(void) {
    int i;
    for (i = 0; i < lbl_8055F61C; i++) {
        fn_802795E8(&lbl_8055F618[i].state);
    }
}

void fn_802797A8(const s32* ids, int n) {
    int i;
    lbl_8055F61C = n;
    lbl_8055F618 = new ("cs_block") CarStateBlock[n];
    for (i = 0; i < lbl_8055F61C; i++) {
        CarStateBlock* b = &lbl_8055F618[i];
        memset(b, 0, sizeof(CarStateBlock));
        b->id = ids[i];
    }
    fn_8027974C();
}

void fn_80279848(void) {
    delete[] lbl_8055F618;
    lbl_8055F618 = 0;
}

CarStateBlock* fn_80279874(s32 id) {
    int i;
    for (i = 0; i < lbl_8055F61C; i++) {
        if (id == lbl_8055F618[i].id) {
            return &lbl_8055F618[i];
        }
    }
    fn_802E30C8("carstate: can't find vehicle #%d", id);
    return 0;
}

f32 fn_802798E8(CarStateBlock* b) { return b->state.time; }

u8 fn_802798F0(u32 from, u32 to, f32 t) {
    int v = ROUND(Lerp(from, to, t));
    v = MAX(v, 0);
    return MIN(v, 255);
}

void fn_802799EC(CarStateBlock* b, f32 dt) {
    f32 time;
    f32 left;
    f32 total;
    f32 t;
    u8* c;
    CarState* s;
    int i;
    s = &b->state;
    time = b->state.time;
    left = MAX(b->state.duration, time - dt);
    total = MAX(time, 0.001f);
    t = 1.0f - left / total;
    s->time = left;
    s->x298 = 0;
    for (i = 0, c = s->col.c[0]; i < 164; i++, c += 4) {
        c[0] = fn_802798F0(c[0], 0x80, t);
        c[1] = fn_802798F0(c[1], 0x80, t);
        c[2] = fn_802798F0(c[2], 0x80, t);
        c[3] = fn_802798F0(c[3], 0, t);
    }
    for (i = 0; i < 18; i++) {
        s->flags.f[i] = 0;
    }
}

void fn_80279B28(const CarStateBlock* b, CarState* out) {
    out->col = b->state.col;
    out->time = b->state.time;
    out->duration = b->state.duration;
    out->x298 = b->state.x298;
    out->x29C = b->state.x29C;
    out->flags = b->state.flags;
}

void fn_80279C00(CarStateBlock* b, const CarState* in) {
    b->state.col = in->col;
    b->state.time = in->time;
    b->state.duration = in->duration;
    b->state.x298 = in->x298;
    b->state.x29C = in->x29C;
    b->state.flags = in->flags;
}

void fn_80279CD8(CarState* s) { fn_802795E8(s); }

}  // extern "C"
