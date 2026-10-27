// Car name tags and map gems (0x802A7BE8-0x802A81EC): draws each opponent's place/name above the car,
// nearest last, and a gem for registered rivals. Names are guesses apart from the asset names.
#include "game/common.h"
#include "macros.h"

struct TagObj {
    u8 pad0[0x24];
    ValVec3 pos;  // offset 0x24
    u8 pad30[0x6E - 0x30];
    u16 flags;  // offset 0x6E
};

struct TagCar {
    u8 pad0[0x8];
    TagObj* obj;  // offset 0x8
    u8 padC[0x10 - 0xC];
    s32 place;  // offset 0x10
    u8 pad14[0x30 - 0x14];
    s32 hidden;  // offset 0x30
    u8 pad34[0xAC - 0x34];
    s32 regId;  // offset 0xAC
};

struct TagCam {
    u8 pad0[0x24];
    ValVec3 pos;  // offset 0x24
};

struct TagReg {
    u8 pad0[0xE8];
    s32 regId;  // offset 0xE8
};

struct TagSlot {
    TagSlot(void) {}
    u8 data[0x1C];
};

extern "C" {
s32 fn_80232578(const char* name);  // load font
void fn_802328D4(s32 font);
void* fn_8023512C(const char* name);
void fn_80235380(void* art);
int fn_801C5B60(void);
s32* fn_801E87A8(void);
TagCar* fn_801EA9A0(int car);
int fn_801EA994(void);
void fn_802C8E70(const ValVec3* p, ValVec3* out);
int fn_802C8FF4(ValVec3* p, f32 margin);
void fn_80235CBC(void* art, int part, u32 color, f32 x, f32 y, f32 z, f32 w, f32 h);
int fn_801E85B4(void);
TagCam* fn_802678C0(int view);
f32 PSVECSquareDistance(const void* a, const void* b);
int fn_801CF064(void);
int fn_801E866C(int view);
const char* fn_800122B4(int place, char* buf);
void fn_802339A8(s32 font, u32 color, int flags, const char* fmt, f32 x, f32 y, f32 z, f32 scale, f32 alpha, ...);
int fn_8027AA48(void);
TagReg* fn_8027ABF8(s32 id);
int fn_8027AC50(TagReg* r);
s32 fn_8027AC64(TagReg* r);
int fn_8027AC80(TagReg* r);
void qsort(void* base, u32 n, u32 size, int (*cmp)(const void*, const void*));
}

ModuleTag lbl_8055F8B0;
UnkStatic7F lbl_8055F8B8;
s32 lbl_8055F8BC;    // font
void* lbl_8055F8C0;  // gem art
TagSlot lbl_804BD5D8[8];

extern "C" {

void fn_802A7BE8(void) {
    lbl_8055F8BC = fn_80232578("uifnt_o.gtx");
    lbl_8055F8C0 = fn_8023512C("mapgems.art");
}

void fn_802A7C24(void) {
    fn_802328D4(lbl_8055F8BC);
    lbl_8055F8BC = 0;
    fn_80235380(lbl_8055F8C0);
    lbl_8055F8C0 = 0;
}

void fn_802A7C64(void) {}

void fn_802A7EEC(void);

void fn_802A7C68(void) {
    if (!fn_801C5B60() && fn_801E87A8()[1] != 0x180304) {
        fn_802A7EEC();
    }
}

void fn_802A7CA8(int car, int gem, f32 t) {
    TagObj* o = fn_801EA9A0(car)->obj;
    ValVec3 p = o->pos;
    ValVec3 scr;
    f32 a;
    u32 color;
    f32 size;
    p.y += 2.5f;
    fn_802C8E70(&p, &scr);
    a = (t - 1.0f) / -1.0f;
    a = MIN(1.0f, a);
    a = MAX(a, 0.0f);
    color = (int)(255.0f * (1.0f + a * -1.0f)) << 24 | 0xFFFFFF;
    a = (t - 1.0f) / -1.0f;
    a = MIN(1.0f, a);
    a = MAX(a, 0.0f);
    size = 24.0f + a * -10.0f;
    if (fn_802C8FF4(&scr, 0.0f)) {
        fn_80235CBC(lbl_8055F8C0, gem, color, scr.x - 0.5f * size, scr.y - 0.5f * size, 0.0f, size, size);
    }
}

int fn_802A7E08(const void* pa, const void* pb) {
    TagCam* cam = fn_802678C0(fn_801E85B4());
    ValVec3 p = fn_801EA9A0(*(const int*)pa)->obj->pos;
    f32 da;
    p.y += 2.0f;
    da = PSVECSquareDistance(&p, &cam->pos);
    p = fn_801EA9A0(*(const int*)pb)->obj->pos;
    p.y += 2.0f;
    if (da > PSVECSquareDistance(&p, &cam->pos)) {
        return -1;
    }
    return 1;
}

// NonMatching: register numbering only. The run-time (far - near) and 3 * far come from plain
// (non-const) float locals, which MWCC does not fold.
void fn_802A7EEC(void) {
    int order[8];
    int* q;
    int i;
    if (fn_801CF064()) {
        return;
    }
    for (i = 0, q = order; i < fn_801EA994(); q++, i++) {
        *q = i;
    }
    qsort(order, fn_801EA994(), sizeof(int), fn_802A7E08);
    f32 nearDist = 100.0f;
    f32 farDist = 7056.0f;
    for (i = 0, q = order; i < fn_801EA994(); q++, i++) {
        int car = *q;
        TagCar* c;
        TagObj* o;
        f32 d;
        if (car == fn_801E866C(-1)) {
            continue;
        }
        c = fn_801EA9A0(car);
        o = c->obj;
        if (o->flags & 0x40) {
            continue;
        }
        if (c->hidden) {
            continue;
        }
        {
            ValVec3 p = o->pos;
            ValVec3 scr;
            p.y += 2.0f;
            fn_802C8E70(&p, &scr);
            d = PSVECSquareDistance(&p, &fn_802678C0(fn_801E85B4())->pos);
            if (fn_802C8FF4(&scr, 0.0f)) {
                char buf[8];
                f32 a = (d - nearDist) / (farDist - nearDist);
                u32 color;
                a = MIN(1.0f, a);
                a = MAX(a, 0.0f);
                color = (int)(255.0f * (1.0f + a * -1.0f)) << 24 | 0xFFFFFF;
                fn_802339A8(lbl_8055F8BC, color, 0x44, "%s", scr.x, scr.y, 0.0f, 0.42f, 1.0f,
                            fn_800122B4(c->place, buf));
            }
        }
        if (fn_8027AA48()) {
            f32 b = (d - farDist) / (3.0f * farDist - farDist);
            s32 id;
            s32 me;
            b = MIN(1.0f, b);
            b = MAX(b, 0.0f);
            b = 1.0f + b * -1.0f;
            id = c->regId;
            me = fn_801EA9A0(fn_801E866C(-1))->regId;
            if (fn_8027AC50(fn_8027ABF8(me))) {
                if (id == fn_8027AC64(fn_8027ABF8(me))) {
                    fn_802A7CA8(car, 3, b);
                }
            } else if (fn_8027AC80(fn_8027ABF8(me))) {
                if (id == fn_8027ABF8(me)->regId) {
                    fn_802A7CA8(car, 2, b);
                }
            }
        }
    }
}

}  // extern "C"
