// Skid marks (0x8028CED8-0x8028DF58): per-terrain skid mark types read from the terrain
// tables, a strip of quad points per wheel and type, appended while a wheel slides and
// compacted as the marks fade. Names are guesses apart from the value keys and tags.
#include "game/common.h"
#include "macros.h"
#include <revolution/MTX.h>

struct SkidWheel {
    Vec pos;     // offset 0x0
    Vec v0C;     // offset 0xC
    Vec v18;     // offset 0x18
    f32 f24;     // offset 0x24
    f32 slip;    // offset 0x28
    u32 flags;   // offset 0x2C
    u8 ground;   // offset 0x30
    f32 f34;     // offset 0x34
    s32 terrain; // offset 0x38
};

struct SkidCar {
    u8 pad0[0x34];
    Vec vel;    // offset 0x34
    Vec dir;    // offset 0x40
    u8 pad4C[0x64 - 0x4C];
    u32 surface;  // offset 0x64
    u8 pad68[0x70 - 0x68];
    SkidWheel wheels[4];  // offset 0x70
};

struct SkidEntry {
    SkidCar* car;  // offset 0x0
    s32 index;     // offset 0x4
    f32 dist;      // offset 0x8
    u8 active;     // offset 0xC
};

struct SkidPt {
    SkidPt(void);
    Vec pos;    // offset 0x0
    Vec d1;     // offset 0xC
    Vec d2;     // offset 0x18
    u16 alpha;  // offset 0x24
    u8 on;      // offset 0x26
};

struct SkidStrip {
    SkidPt* pts;  // offset 0x0
    s32 n;        // offset 0x4
    s32 len;      // offset 0x8
};

struct SkidType {
    s32 tex;            // offset 0x0
    f32 bias;           // offset 0x4
    f32 slope;          // offset 0x8
    f32 thresh;         // offset 0xC
    s32 size;           // offset 0x10
    SkidStrip* strips;  // offset 0x14
    s32 total;          // offset 0x18
};

struct SkidSlot {
    s32 type;  // offset 0x0
    f32 time;  // offset 0x4
};

class SkidNode {
public:
    virtual const char* GetName(void);
    virtual int Has(const char* key);
    virtual int GetPair(int index, const char** key, const char** value);
    virtual const char* GetValue(const char* key);
    virtual const char* GetString(const char* key, const char* def);
    virtual s32 GetIntValue(const char* key);
    virtual s32 GetInt(const char* key, s32 def);
    virtual f32 GetFloat(const char* key);
    virtual f32 GetFloat(const char* key, f32 def);
    virtual s32 GetIntValue2(const char* key);
    virtual int GetBool(const char* key, int def);
    virtual ValVec3 GetVec3(const char* key);
    virtual ValVec3 GetVec3Or(const char* key, ValVec3 def);
    virtual void Unk3C(void);
    virtual void Unk40(void);
    virtual void Unk44(void);
    virtual void Unk48(void);
    virtual void Unk4C(void);
    virtual void Unk50(void);
    virtual void Unk54(void);
    virtual void Unk58(void);
    virtual SkidNode* Find(const char* name);
};

extern "C" {
double fn_8002F208(double x);
int fn_801345D8(char* buf, int size, const char* fmt, ...);  // snprintf
int fn_801E5370(int n, int k, f32 w);
void fn_801E53A8(const Vec* p, const Vec* a, const Vec* b, f32 alpha, int on);
void fn_801E58FC(void);
f32 fn_801E7E18(void);
f32 fn_801E7E24(void);
f32 fn_801E88F0(int view);
s32 fn_80287B14(void);
void fn_80287B38(void (*cb)(SkidEntry* e, int n));
SkidNode* fn_802AE414(const char* name);
void fn_802C8BD0(s32 tex);
void fn_802C8D74(void);
void fn_802C9648(f32 a);
f32 fn_802C9A4C(f32 lod);
void fn_802C967C(void);
s32 fn_802CAC28(const char* name, int flags);
void fn_802CAC54(s32 tex);
void fn_802E317C(const char* fmt, ...);
}

void* operator new[](size_t size, const char* name);

extern f32 lbl_80558DC4[];  // FLT_EPSILON

ModuleTag lbl_8055F708;
f32 lbl_8055F710 = fn_8002F208(2.2222222222222223);
u8 lbl_8055F714;
SkidPt* lbl_8055F718;     // point pool
SkidStrip* lbl_8055F71C;  // strip pool
SkidSlot* lbl_8055F720;   // per-wheel slots
s32 lbl_8055F724;         // wheel count
s32 lbl_8055F728;         // type count
SkidType lbl_804B3400[4];
s32 lbl_804B3470[8];      // skid type of each terrain
u32 lbl_804B3490[8];      // last surface of each car

extern "C" {

int fn_8028D17C(void);
void fn_8028D25C(void);
void fn_8028D2B4(void);
void fn_8028D4A0(void);
void fn_8028D4FC(void);
void fn_8028D728(void);
void fn_8028D7C0(int i);
void fn_8028DDF0(SkidEntry* e, int n);

void fn_8028CED8(void) {
    if (fn_8028D17C()) {
        fn_8028D2B4();
        fn_80287B38(fn_8028DDF0);
        lbl_8055F714 = 1;
    }
}

void fn_8028CF18(void) {
    if (lbl_8055F714) {
        fn_8028D4A0();
        fn_8028D25C();
        lbl_8055F714 = 0;
    }
}

void fn_8028CF50(void) {
    if (lbl_8055F714) {
        fn_8028D4FC();
    }
}

void fn_8028CF64(void) {
    if (lbl_8055F714) {
        if (!((f32)__fabs(fn_801E7E24()) <= lbl_80558DC4[0])) {
            fn_8028D728();
        }
    }
}

void fn_8028CFB0(void) {
    if (lbl_8055F714) {
        f32 old = fn_802C9A4C(lbl_8055F710);
        fn_802C9648(1.1f);
        SkidType* t = lbl_804B3400;
        for (int i = 0; i < lbl_8055F728; i++, t++) {
            if (t->total > 1) {
                fn_802C8BD0(t->tex);
                if (fn_801E5370(t->total, 4, 0.075f * fn_801E88F0(-1))) {
                    fn_8028D7C0(i);
                    fn_801E58FC();
                }
                fn_802C8D74();
            }
        }
        fn_802C967C();
        fn_802C9A4C(old);
    }
}

void fn_8028D084(SkidType* t, SkidNode* n) {
    t->tex = fn_802CAC28(n->GetValue("texture"), 0);
    t->bias = n->GetFloat("alpha_bias");
    t->slope = n->GetFloat("alpha_slope");
    t->thresh = n->GetFloat("skid_thresh");
    t->size = n->GetIntValue("buffer_size");
    if (t->size < 8 || t->size > 36) {
        fn_802E317C("skid: invalid buffer size %d", t->size);
    }
}

int fn_8028D17C(void) {
    lbl_8055F728 = 0;
    for (int i = 0; i < 8; i++) {
        char buf[32];
        fn_801345D8(buf, 32, "terrain_%d", i);
        SkidNode* n = fn_802AE414(buf);
        if (n) {
            n = n->Find("skid");
        }
        if (n) {
            fn_8028D084(&lbl_804B3400[lbl_8055F728], n);
            lbl_804B3470[i] = lbl_8055F728++;
        } else {
            lbl_804B3470[i] = -1;
        }
    }
    return lbl_8055F728 > 0;
}

void fn_8028D25C(void) {
    for (int i = 0; i < lbl_8055F728; i++) {
        fn_802CAC54(lbl_804B3400[i].tex);
    }
}

void fn_8028D2B4(void) {
    int total = 0;
    int i;
    for (i = 0; i < lbl_8055F728; i++) {
        total += lbl_804B3400[i].size;
    }
    lbl_8055F724 = fn_80287B14() * 4;
    int nPts = total * lbl_8055F724;
    int nStrips = lbl_8055F728 * lbl_8055F724;
    lbl_8055F718 = new ("skid pt pool") SkidPt[nPts];
    lbl_8055F71C = new ("skid buffer pool") SkidStrip[nStrips];
    lbl_8055F720 = new ("skid wheel tab") SkidSlot[lbl_8055F724];
    SkidPt* p = lbl_8055F718;
    SkidStrip* s = lbl_8055F71C;
    for (i = 0; i < lbl_8055F728; i++) {
        lbl_804B3400[i].strips = s;
        for (int k = 0; k < lbl_8055F724; k++) {
            s->pts = p;
            s->len = lbl_804B3400[i].size;
            s++;
            p += lbl_804B3400[i].size;
        }
    }
}

}  // extern "C"

SkidPt::SkidPt(void) {}

extern "C" {

void fn_8028D4A0(void) {
    delete[] lbl_8055F720;
    lbl_8055F720 = 0;
    delete[] lbl_8055F71C;
    lbl_8055F71C = 0;
    delete[] lbl_8055F718;
    lbl_8055F718 = 0;
}

void fn_8028D4FC(void) {
    int i;
    for (i = 0; i < lbl_8055F728; i++) {
        for (int k = 0; k < lbl_8055F724; k++) {
            lbl_804B3400[i].strips[k].n = 0;
        }
    }
    for (i = 0; i < lbl_8055F724; i++) {
        lbl_8055F720[i].type = -1;
        lbl_8055F720[i].time = 0.0f;
    }
}

void fn_8028D58C(SkidStrip* s) {
    SkidPt* end = s->pts + s->n;
    SkidPt* start = s->pts + MAX(s->n - s->len + 2, 0);
    SkidPt* p;
    for (p = start; p < end; p++) {
        int keep = 0;
        if (p > start && p->on &&
            (p->alpha >= 0x100 || p[-1].alpha >= 0x100 || p + 1 >= end || (p[1].on && p[1].alpha >= 0x100))) {
            keep = 1;
        }
        p->on = keep;
    }
    SkidPt* out = s->pts;
    for (p = start; p < end; p++) {
        if (p->on || (p + 1 < end && p[1].on)) {
            out->pos.x = p->pos.x;
            out->pos.y = p->pos.y;
            out->pos.z = p->pos.z;
            out->d1.x = p->d1.x;
            out->d1.y = p->d1.y;
            out->d1.z = p->d1.z;
            out->d2.x = p->d2.x;
            out->d2.y = p->d2.y;
            out->d2.z = p->d2.z;
            out->alpha = MAX(p->alpha - 0x80, 0);
            out->on = p->on;
            out++;
        }
    }
    s->n = out - s->pts;
}

void fn_8028D728(void) {
    SkidType* t = lbl_804B3400;
    for (int i = 0; i < lbl_8055F728; i++, t++) {
        t->total = 0;
        for (int k = 0; k < lbl_8055F724; k++) {
            SkidStrip* s = &t->strips[k];
            fn_8028D58C(s);
            t->total += s->n;
        }
    }
}

void fn_8028D7C0(int i) {
    SkidType* t = &lbl_804B3400[i];
    for (int k = 0; k < lbl_8055F724; k++) {
        SkidStrip* s = &t->strips[k];
        for (int j = 0; j < s->n; j++) {
            SkidPt* p = &s->pts[j];
            fn_801E53A8(&p->pos, &p->d1, &p->d2, (1.0f / 65280.0f) * p->alpha, p->on != 0);
        }
    }
}

static inline void FillPoint(SkidPt* p, const SkidCar* car, const SkidWheel* w, const SkidType* t) {
    Vec side;
    p->pos.x = w->v0C.x;
    p->pos.y = w->v0C.y;
    p->pos.z = w->v0C.z;
    p->d1.x = car->vel.x * 0.1f;
    p->d1.y = car->vel.y * 0.1f;
    p->d1.z = car->vel.z * 0.1f;
    PSVECCrossProduct(&car->dir, &w->v18, &side);
    if (!(w->flags & 8)) {
        p->d2.x = side.x * 0.225f;
        p->d2.y = side.y * 0.225f;
        p->d2.z = side.z * 0.225f;
    } else {
        p->d2.x = side.x * 0.025f;
        p->d2.y = side.y * 0.025f;
        p->d2.z = side.z * 0.025f;
    }
    int a = (int)(65535.0f * (t->bias + t->slope * (w->slip - MAX(0.0f, t->thresh))));
    a = MAX(a, 0);
    p->alpha = MIN(a, 0xFFFF);
}

void fn_8028D89C(SkidEntry* e, int wi, f32 now) {
    SkidWheel* w;
    SkidStrip* s;
    SkidPt* q;
    SkidPt* p;
    int type;
    SkidSlot* slot;
    SkidType* t;
    SkidCar* car = e->car;
    int idx = wi + e->index * 4;
    w = &car->wheels[wi];
    slot = &lbl_8055F720[idx];
    if (w->terrain < 8 && (type = lbl_804B3470[w->terrain]) >= 0) {
        t = &lbl_804B3400[type];
        int skid = 0;
        if (w->ground && w->slip > t->thresh) {
            skid = 1;
        }
        if (skid) {
            if (slot->type == type) {
                if (now > 0.2f + slot->time) {
                    s = &t->strips[idx];
                    p = &s->pts[s->n];
                    FillPoint(p, car, w, t);
                    p->on = 1;
                    s->n++;
                    t->total++;
                    slot->time = now;
                } else {
                    s = &t->strips[idx];
                    p = &s->pts[s->n - 1];
                    FillPoint(p, car, w, t);
                }
            } else {
                s = &t->strips[idx];
                p = &s->pts[s->n];
                q = &s->pts[s->n + 1];
                FillPoint(p, car, w, t);
                p->on = 0;
                q->pos.x = p->pos.x;
                q->pos.y = p->pos.y;
                q->pos.z = p->pos.z;
                q->d2.x = p->d2.x;
                q->d2.y = p->d2.y;
                q->d2.z = p->d2.z;
                q->d1.x = p->d1.x;
                q->d1.y = p->d1.y;
                q->d1.z = p->d1.z;
                q->alpha = p->alpha;
                q->on = 1;
                s->n += 2;
                t->total += 2;
            }
            slot->type = type;
        } else {
            slot->type = -1;
        }
    } else {
        slot->type = -1;
    }
}

void fn_8028DDF0(SkidEntry* e, int n) {
    if ((f32)__fabs(fn_801E7E24()) <= lbl_80558DC4[0]) {
        return;
    }
    f32 now = fn_801E7E18();
    for (int i = 0; i < n; i++, e++) {
        if (e->dist < 40000.0f && e->car->surface == lbl_804B3490[e->index] && e->active) {
            for (int w = 0; w < 4; w++) {
                fn_8028D89C(e, w, now);
            }
        } else {
            lbl_8055F720[e->index * 4 + 0].type = -1;
            lbl_8055F720[e->index * 4 + 1].type = -1;
            lbl_8055F720[e->index * 4 + 2].type = -1;
            lbl_8055F720[e->index * 4 + 3].type = -1;
        }
        lbl_804B3490[e->index] = e->car->surface;
    }
}

}  // extern "C"
