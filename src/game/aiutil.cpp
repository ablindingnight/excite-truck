// Small 2D vector helpers and stubs (0x80160B14-0x80160CE4), next to a 20-entry pointer list that nothing
// here uses any more. Names are guesses.
#include "game/common.h"

// The game mode base (amaster.cpp); its empty vf44 lives here.
class GameMode {
public:
    void vf44(void);
};

extern "C" {
double sqrt(double x);
void fn_80128658(void* p);  // MemFree
}

template <typename T>
struct ArrayRef {
    ArrayRef(T* d, int n) {
        data = d;
        cap = n;
        count = 0;
    }
    ~ArrayRef(void) {
        data = 0;
        count = 0;
        cap = 0;
    }
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
};

struct V2 {
    f32 x;
    f32 y;
};

ModuleTag lbl_8055E2C0;
UnkStaticFFFF lbl_8055E2C8;
struct PtrList : ArrayRef<void*> {
    PtrList(void) : ArrayRef<void*>(storage, 20) {}
    void* storage[20];
};

PtrList lbl_8042FD84;

static inline f32 Len(f32 x, f32 y) { return sqrt(x * x + y * y); }

extern "C" {

void fn_80160B14(void* p) { fn_80128658(p); }

void fn_80160B18(V2* v) {
    f32 inv = 1.0f / Len(v->x, v->y);
    v->x *= inv;
    v->y *= inv;
}

void fn_80160B7C(V2* r, const V2* a, const V2* b) {
    f32 x = a->x - b->x;
    f32 y = a->y - b->y;
    r->x = x;
    r->y = y;
}

void fn_80160BA0(V2* r, const V2* a, f32 s) {
    f32 x = a->x * s;
    f32 y = a->y * s;
    r->x = x;
    r->y = y;
}

void fn_80160BBC(V2* r, const V2* a, const V2* b) {
    f32 x = a->x + b->x;
    f32 y = a->y + b->y;
    r->x = x;
    r->y = y;
}

f32 fn_80160BE0(const V2* a, const V2* b) {
    V2 d;
    d.y = a->y - b->y;
    d.x = a->x - b->x;
    return d.x * d.x + d.y * d.y;
}

int fn_80160C18(void) { return 0; }

f32 fn_80160C20(void) { return 5.0f; }

void GameMode::vf44(void) {}

}  // extern "C"
