// Layer weights (0x802BB87C-0x802BC328): the six terrain material weight maps (matmap_b/g/r and
// matmap2_b/g/r), editor painting into them, and saving them back as two .tga images. Names are
// guesses apart from the file names and messages.
#include "game/common.h"

extern "C" {
char* strchr(const char* s, int c);
void* memset(void* dst, int c, size_t n);
int fn_801345D8(char* buf, int size, const char* fmt, ...);  // snprintf
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
int fn_8012C33C(const char* name);       // resource exists
s32 fn_802CAC28(const char* name, int flags);
void fn_802CAC54(s32 handle);
void* fn_802CA294(s32 handle);
u32 fn_802D972C(void* tex, int x, int y);
void fn_802D9628(void* tex, int x, int y, u32 color);
u8 fn_80128690(void);
void fn_80128684(void);
void fn_80128678(void);
u8 fn_802B05E4(void);
int fn_802B05DC(void);
}

struct WeightImage {
    int w;     // offset 0x0
    int h;     // offset 0x4
    u32* data; // offset 0x8
};

extern "C" {
WeightImage* fn_802B51E4(int w, int h);
void fn_802B52B4(WeightImage* img);
void fn_802B52C0(const char* name, WeightImage* img);
}

ModuleTag lbl_8055FA88;
u8 lbl_8055FA90;        // weights fixed (one layer everywhere)
s32 lbl_804C0098[6];    // weight texture handles
void* lbl_804C00B0[6];  // weight textures

static inline f32 Lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

extern "C" {

s32 fn_802BB87C(const char* name) {
    int ok = 0;
    if (name && fn_8012C33C(name)) {
        ok = 1;
    }
    if (ok) {
        return fn_802CAC28(name, 0);
    }
    return -2;
}

void fn_802BB8E4(s32* handle) {
    if (*handle != -2) {
        fn_802CAC54(*handle);
        *handle = -2;
    }
}

short fn_802BB928(short x) { return (x + 512) / 2; }

short fn_802BB93C(short z) { return 511 - (z + 512) / 2; }

void fn_802BB958(short x, short z, const u8* weights, const u8* old) {
    int px = fn_802BB928(x);
    int pz = fn_802BB93C(z);
    for (int i = 0; i < 6; i++) {
        if (lbl_804C00B0[i] && weights[i] != old[i]) {
            u32 v = weights[i];
            fn_802D9628(lbl_804C00B0[i], px, pz, (v << 24) | (v << 16) | (v << 8) | v);
        }
    }
}

void fn_802BBA08(const char* name, int base) {
    WeightImage* img = fn_802B51E4(512, 512);
    if (img) {
        for (int x = 0; x < 512; x++) {
            for (int z = 0; z < 512; z++) {
                u32 pixel = 0;
                for (int k = 0; k < 3; k++) {
                    if (lbl_804C00B0[base + k]) {
                        pixel |= ((fn_802D972C(lbl_804C00B0[base + k], x, z) >> 8) & 0xFF) << (k * 8);
                    }
                }
                int y = img->h - 1 - z;
                img->data[y * img->w + x] = pixel;
            }
        }
        char path[0x80];
        {
            int n = sizeof(path);
            char* d = path;
            const char* s = name;
            while ((*d++ = *s++) != '\0') {
                if (--n <= 0) {
                    d[-1] = '\0';
                    break;
                }
            }
        }
        if (strchr(path, '.')) {
            char* dot = strchr(path, '.');
            *dot = '\0';
        }
        {
            const char* s;
            int n = sizeof(path);
            char* d = path;
            s = ".tga";
            while (*d) {
                n--;
                d++;
            }
            if (n > 0) {
                while ((*d++ = *s++) != '\0') {
                    if (--n <= 0) {
                        d[-1] = '\0';
                        break;
                    }
                }
            }
        }
        fn_802B52C0(path, img);
        fn_802B52B4(img);
    }
}

void fn_802BBBC8(void) {
    int i;
    const char* chans;
    const char* names[2] = {"matmap", "matmap2"};
    i = 0;
    chans = "bgr";
    for (; i < 6; i++) {
        char buf[0x20];
        fn_801345D8(buf, sizeof(buf), "%s_%c.tex", names[i / 3], chans[i % 3]);
        lbl_804C0098[i] = fn_802BB87C(buf);
        if (lbl_804C0098[i] == -2) {
            fn_802E317C("lweight: no matmap texture for layer %d?", i);
        }
        lbl_804C00B0[i] = (lbl_804C0098[i] == -2) ? NULL : fn_802CA294(lbl_804C0098[i]);
    }
    lbl_8055FA90 = fn_802B05E4();
}

void fn_802BBCC4(void) {
    for (int i = 0; i < 6; i++) {
        fn_802BB8E4(&lbl_804C0098[i]);
        lbl_804C00B0[i] = NULL;
    }
}

u8 fn_802BBD38(short x, short z, int layer) {
    if (lbl_8055FA90) {
        return layer == 1 ? 0xFF : 0;
    }
    u8 w = 0;
    if (lbl_804C00B0[layer]) {
        short px = fn_802BB928(x);
        short pz = fn_802BB93C(z);
        int out = 0;
        if (px < 0 || px >= 512 || pz < 0 || pz >= 512) {
            out = 1;
        }
        if (!out) {
            w = (fn_802D972C(lbl_804C00B0[layer], fn_802BB928(x), fn_802BB93C(z)) >> 8) & 0xFF;
        } else {
            w = layer == 2 ? 0xFF : 0;
        }
    }
    return w;
}

void fn_802BBE34(void) {
    for (int x = 0; x < 512; x++) {
        for (int z = 0; z < 512; z++) {
            u32 sum = 0;
            for (int i = 0; i < 6; i++) {
                if (lbl_804C00B0[i]) {
                    sum += (fn_802D972C(lbl_804C00B0[i], x, z) >> 8) & 0xFF;
                }
            }
            if (sum != 0xFF) {
                fn_802E30C8("at %d,%d: %d", x, z, sum);
            }
        }
    }
}

void fn_802BBEE4(void) {
    fn_802BBE34();
    u8 was = fn_80128690();
    if (!was) {
        fn_80128684();
    }
    fn_802BBA08("matmap", 0);
    fn_802BBA08("matmap2", 3);
    if (!was) {
        fn_80128678();
    }
}

void fn_802BBF44(const u8* w, int adj) {
    u32 sum = 0;
    for (int i = 0; i < 6; i++) {
        sum += w[i];
    }
    if (sum != 0xFF) {
        fn_802E30C8("I AM THE COOLEST EVAR! (%d %d %d %d %d %d) (adj %d)", w[0], w[1], w[2], w[3], w[4],
                    w[5], adj);
    }
}

u8 fn_802BBF94(u8 v) { return (v & 0xF0) | (v >> 4); }

void fn_802BBFA4(u8* w, int layer, int keep) {
    for (int i = 0; i < 6; i++) {
        w[i] = fn_802BBF94(w[i]);
    }
    int sum = 0;
    for (int i = 0; i < 6; i++) {
        sum += w[i];
    }
    if (sum == 0xFF) {
        return;
    }
    if (w[layer] >= sum - 0xFF) {
        w[layer] -= sum - 0xFF;
        return;
    }
    int d;
    u8* p = w;
    for (int i = 0; sum != 0xFF && i < 6; i++, p++) {
        if (i != keep) {
            d = *p;
            if (sum - 0xFF < d) {
                d = sum - 0xFF;
            }
            sum -= d;
            *p -= d;
        }
    }
}

int fn_802BC09C(short x, short z, int layer, u8 value, f32 strength) {
    int locked;
    int i;
    int done;
    u8 keepVal;
    done = 0;
    locked = fn_802B05DC();
    if (layer != locked) {
        short px = fn_802BB928(x);
        short pz = fn_802BB93C(z);
        int out = 0;
        if (px < 0 || px >= 512 || pz < 0 || pz >= 512) {
            out = 1;
        }
        if (!out) {
            u8 cur[6];
            for (i = 0; i < 6; i++) {
                cur[i] = fn_802BBD38(x, z, i);
            }
            if (locked == -1 || cur[locked] < 0x80) {
                keepVal = (locked == -1) ? 0 : cur[locked];
                u8 next[6];
                memset(next, 0, sizeof(next));
                next[layer] = value;
                for (i = 0; i < 6; i++) {
                    f32 f = Lerp(cur[i], next[i], strength);
                    next[i] = (int)((f < 0.0f) ? f - 0.5f : 0.5f + f);
                }
                if (locked != -1) {
                    u8 old = next[locked];
                    next[locked] = keepVal;
                    if (old > keepVal) {
                        next[layer] -= old - keepVal;
                    } else {
                        next[layer] -= keepVal - old;
                    }
                }
                fn_802BBFA4(next, layer, locked);
                fn_802BBF44(next, layer);
                fn_802BB958(x, z, next, cur);
                done = 1;
            }
        }
    }
    return done;
}

s32 fn_802BC30C(int i) { return lbl_804C0098[i]; }

}  // extern "C"

// NonMatching: fn_802BC09C assigns the cur loop's counter/pointer and the unrolled blend's volatile
// registers differently (25 diffs); fn_802BBBC8's sdata2 template reference differs only by symbol.
