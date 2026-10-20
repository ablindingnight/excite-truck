// Background boats (0x802C2298-0x802C26F0): reads boat models and positions from _boat.val and
// draws them bobbing and rolling. Names are guesses apart from the value keys.
#include "game/common.h"

extern "C" {
ValueTable* fn_80131810(const char* name);  // load value file, quiet
void fn_80131818(ValueTable* t);
int fn_801345D8(char* buf, u32 size, const char* fmt, ...);  // snprintf
s32 fn_802C6FC0(const char* name, int a, int b);
void fn_802C6CF8(s32 model);
f32 fn_80129DC4(int seed, f32 lo, f32 hi);  // random float
void fn_802C744C(s32 model, const ValMatrix* m);
double sin(double x);
}

struct Mtx33 {
    f32 m[9];
};

extern "C" {
void fn_80015A20(Mtx33* out, const ValVec3* angles);
}

struct Mat3;
void Mat3Mul(Mat3* out, const Mat3* a, const Mat3* b);

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
    T* Next(void) { return &data[count++]; }
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
};

struct Boat {
    Boat(void) {}
    s32 model;    // offset 0x0
    ValVec3 pos;  // offset 0x4
    f32 phase;    // offset 0x10
};

struct BoatList : ArrayRef<Boat> {
    BoatList(void) : ArrayRef<Boat>(storage, 32) {}
    Boat storage[32];
};

extern f32 lbl_8055E3C8;  // race clock

ModuleTag lbl_8055FB58;
BoatList lbl_804C109C;

extern "C" {

void fn_802C2298(void) {
    ValueTable* t;
    lbl_804C109C.count = 0;
    t = fn_80131810("_boat.val");
    if (t) {
        ValueNode* boats = t->FindChild("boats");
        char name[32];
        int i = 0;
        ValueNode* node;
        fn_801345D8(name, sizeof(name), "boat%d", i++);
        node = boats->FindChild(name);
        while (node) {
            Boat* b = lbl_804C109C.Next();
            b->model = fn_802C6FC0(node->GetValue("model"), 0, 0);
            b->pos = node->GetVec3("pt");
            b->phase = fn_80129DC4(0, 0.0f, 10.0f);
            fn_801345D8(name, sizeof(name), "boat%d", i++);
            node = boats->FindChild(name);
        }
        fn_80131818(t);
    }
}

void fn_802C2410(void) {
    int i;
    for (i = 0; i < lbl_804C109C.count; i++) {
        fn_802C6CF8(lbl_804C109C.data[i].model);
    }
    lbl_804C109C.count = 0;
}

// NonMatching: the target hoists the float constants with 0.6 in f25 (ours: f31) and puts
// `i = 0` before the zero-trip guard; this explicit guard is the closest form found so far.
void fn_802C2488(void) {
    f32 bobHeight;
    int i;
    i = 0;
    if (i < lbl_804C109C.count) {
    for (; i < lbl_804C109C.count; i++) {
        Boat b = lbl_804C109C.data[i];
        bobHeight = 0.6f;
        ValMatrix m;
        Mtx33 rot;
        ValVec3 ang;
        f32 roll;
        ValVec3 off;
        f32 bob;
        m.basis[0].x = 1.0f;
        m.basis[0].y = 0.0f;
        m.basis[0].z = 0.0f;
        m.basis[1].x = 0.0f;
        m.basis[1].y = 1.0f;
        m.basis[1].z = 0.0f;
        m.basis[2].x = 0.0f;
        m.basis[2].y = 0.0f;
        m.basis[2].z = 1.0f;
        m.origin.x = 0.0f;
        m.origin.y = 0.0f;
        m.origin.z = 0.0f;
        bob = sin(3.1415927f * (b.phase + lbl_8055E3C8));
        roll = 0.017453292f * (6.0f * (f32)sin(0.5f * (3.1415927f * (b.phase + lbl_8055E3C8))));
        ang.x = roll;
        ang.y = 0.0f;
        ang.z = roll;
        fn_80015A20(&rot, &ang);
        Mat3Mul((Mat3*)&m, (const Mat3*)&rot, (const Mat3*)&m);
        off.x = 0.0f;
        off.y = 0.5f + bobHeight * bob;
        off.z = 0.0f;
        m.origin.x += b.pos.x;
        m.origin.y += b.pos.y;
        m.origin.z += b.pos.z;
        m.origin.x += off.x;
        m.origin.y += off.y;
        m.origin.z += off.z;
        fn_802C744C(b.model, &m);
    }
    }
}

}  // extern "C"
