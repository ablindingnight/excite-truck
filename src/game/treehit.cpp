// Tree hits (0x802C110C-0x802C2074): track trees are registered per tree class (from the class's config:
// "no_wobble", a "drop_model" with its "count" and "drop_hit" height, and up to eight "efx_event_id%d"
// events). When a car hits a tree the tree wobbles (one of eight shake slots tilts its matrix with a damped
// sine for two seconds), the class's dropped models are thrown out with random velocity and spin, and the
// events are posted; knocked-over trees are moved below the world and restored on reset. Names are guesses
// apart from the strings.
#include "game/common.h"

extern "C" {
void* memset(void* p, int c, u32 n);
int strcmp(const char* a, const char* b);
double sin(double x);
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
int fn_801345D8(char* buf, int n, const char* fmt, ...);  // snprintf
f32 fn_80129DC4(int seed, f32 lo, f32 hi);                // random float
s32 fn_801FB1F4(int n);
void fn_801796EC(int type, const void* data, int size);   // post event
}

void* operator new[](size_t size, const char* tag);
void operator delete[](void* p);

struct TVec {
    TVec(void) {}
    TVec(f32 a, f32 b, f32 c) {
        x = a;
        y = b;
        z = c;
    }
    f32 x;
    f32 y;
    f32 z;
};

struct TPos {
    f32 x;
    f32 y;
    f32 z;
};

struct TMtx {
    f32 rot[3][3];  // offset 0x0
    TPos pos;       // offset 0x24
};

// A plain copy of a tree matrix (copied as words).
struct TMtxRaw {
    s32 w[12];
};

struct TRot {
    f32 m[3][3];
};

extern "C" {
void fn_8022FF10(TRot* out, const TPos* axis);  // rotation from axis * angle
void fn_80015B20(TMtx* out, const TMtx* a, const TRot* b);
void fn_802BDFF4(TMtx* tree, int a);
}

struct CarRec {
    u8 pad0[0x6C];
    s16 hint;  // offset 0x6C
    u8 pad6E[0x118 - 0x6E];
    TVec vel;  // offset 0x118
};

extern "C" CarRec* fn_8017960C(int car);

class TreeObj {
public:
    virtual ValueNode* Config(void);  // 0x8
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void Hide(int a);  // 0x18
    virtual void vf1C(void);
    virtual s32 Type(void);    // 0x20
    virtual u8* Handle(void);  // 0x24
    virtual void Throw(const TMtx* m, const TVec* vel, const TVec* spin);  // 0x28
};

extern "C" void fn_801FA7F4(const char* model, const TMtx* m, ValueNode* cfg);

// A tree class (0x60).
struct TreeClass {
    s32 first;           // offset 0x0
    s32 last;            // offset 0x4
    char name[0x20];     // offset 0x8
    bool wobble;         // offset 0x28
    f32 dropHit;         // offset 0x2C
    ValueNode* cfg;      // offset 0x30
    TreeObj** drops;     // offset 0x34
    s32 maxDrops;        // offset 0x38
    s32 nDrops;          // offset 0x3C
    s32 events[8];       // offset 0x40
};

// A wobbling tree (0x48).
struct Shake {
    Shake(void) {}
    s32 index;   // offset 0x0
    TMtx* tree;  // offset 0x4
    TMtx mtx;    // offset 0x8
    TVec axis;   // offset 0x38
    f32 start;   // offset 0x44
};

struct LastHit {
    s32 id;    // offset 0x0
    f32 time;  // offset 0x4
};

struct TreeEvent {
    TPos pos;  // offset 0x0
    TVec vel;  // offset 0xC
    s16 hint;  // offset 0x18
    s32 id;    // offset 0x1C
};

static inline void StrCopy(char* d, const char* s, int n) {
    char* p = d;
    while ((*p++ = *s++) != 0) {
        if (--n <= 0) {
            p[-1] = 0;
            break;
        }
    }
}

ModuleTag lbl_8055FB20;
s32 lbl_8055FB28;      // classes
s32 lbl_8055FB2C;      // trees
TMtx** lbl_8055FB30;   // trees by id
s32 lbl_8055FB34;      // next shake slot
f32 lbl_8055FB38;      // time
s32 lbl_8055CFA0 = -10000;  // handle base
TreeClass lbl_804C0810[16];
Shake lbl_804C0E10[8];
LastHit lbl_804C1050[8];

class IHitListener {
public:
    virtual void Added(void) = 0;
    virtual void Removed(void) = 0;
    virtual void Hit(TreeObj* o, int car) = 0;
};

class IModelListener {
public:
    virtual void Added(TreeObj* o) = 0;
    virtual void Gone(void) = 0;
    virtual void Changed(void) = 0;
    virtual void Hit(void) = 0;
};

extern "C" {
void fn_801D0590(IHitListener* l);
void fn_801D05B4(IHitListener* l);
void fn_801D0800(IModelListener* l);
void fn_801D0824(IModelListener* l);
TreeClass* fn_802C110C(int id);
void fn_802C1180(Shake* s);
Shake* fn_802C120C(void);
}

class TreeHit : public IHitListener, public IModelListener {
public:
    virtual void Added(void) {}
    virtual void Removed(void) {}
    virtual void Hit(TreeObj* o, int car) {
        int id = o->Handle() - (u8*)lbl_8055CFA0;
        if (id >= 0 && id < lbl_8055FB2C) {
            TreeClass* c = fn_802C110C(id);
            TMtx** tbl = lbl_8055FB30;
            TMtx* t = tbl[id];
            if (t) {
                if (o->Type() == 1003) {
                    if (t->pos.y < -2000.0f) {
                        o->Hide(0);
                    }
                    return;
                }
                if (!(t->pos.y < -2000.0f)) {
                    LastHit* lh = &lbl_804C1050[car];
                    if (id != lh->id || lbl_8055FB38 - lh->time > 8.0f) {
                        if (c->wobble) {
                            Shake* s = fn_802C120C();
                            s->tree = t;
                            tbl[id] = 0;
                            s->index = id;
                            s->mtx = *t;
                            s->start = lbl_8055FB38;
                            s->axis = TVec(1.0f, 0.0f, 0.0f);
                        }
                        if (c->nDrops > 0) {
                            for (int i = 0; i < c->nDrops; i++) {
                                TreeObj* d = c->drops[i];
                                TMtx m;
                                m.pos = t->pos;
                                m.pos.x += fn_80129DC4(4, -2.0f, 2.0f);
                                m.pos.z += fn_80129DC4(4, -2.0f, 2.0f);
                                m.pos.y = m.pos.y + c->dropHit;
                                m.rot[0][0] = 1.0f;
                                m.rot[0][1] = 0.0f;
                                m.rot[0][2] = 0.0f;
                                m.rot[1][0] = 0.0f;
                                m.rot[1][1] = 1.0f;
                                m.rot[1][2] = 0.0f;
                                m.rot[2][0] = 0.0f;
                                m.rot[2][1] = 0.0f;
                                m.rot[2][2] = 1.0f;
                                TVec vel(fn_80129DC4(4, -2.0f, 2.0f), -3.0f + fn_80129DC4(4, -2.0f, 2.0f),
                                         fn_80129DC4(4, -2.0f, 2.0f));
                                TVec spin(fn_80129DC4(4, -7.0f, 7.0f), fn_80129DC4(4, -7.0f, 7.0f),
                                          fn_80129DC4(4, -7.0f, 7.0f));
                                d->Throw(&m, &vel, &spin);
                            }
                        }
                    }
                    lh->id = id;
                    lbl_804C1050[car].time = lbl_8055FB38;
                }
            }
        }
    }
    virtual void Hit(void) {}
    virtual void Changed(void) {}
    virtual void Gone(void) {}
    virtual void Added(TreeObj* o) {
        ValueNode* n = o->Config();
        if (n) {
            TreeClass* c;
            const char* name = n->GetString("name", "");
            c = lbl_804C0810;
            for (int i = 0; i < lbl_8055FB28; c++, i++) {
                if (strcmp(name, c->name) == 0) {
                    c->drops[c->nDrops++] = o;
                }
            }
        }
    }
};

TreeHit lbl_8055FB40;

extern "C" {

TreeClass* fn_802C110C(int id) {
    TreeClass* c = lbl_804C0810;
    for (int i = 0; i < lbl_8055FB28; i++, c++) {
        if (id >= c->first && id <= c->last) {
            return c;
        }
    }
    fn_802E317C("treehit: can't find class for tree %d", id);
    return 0;
}

void fn_802C1180(Shake* s) {
    if (s->tree) {
        *s->tree = s->mtx;
        lbl_8055FB30[s->index] = s->tree;
        s->tree = 0;
    }
}

Shake* fn_802C120C(void) {
    Shake* s = &lbl_804C0E10[lbl_8055FB34];
    lbl_8055FB34 = (lbl_8055FB34 + 1) % 8;
    fn_802C1180(s);
    return s;
}

void fn_802C1928(void);
void fn_802C1A08(void);

void fn_802C126C(void) {
    lbl_8055FB2C = 0;
    lbl_8055FB28 = 0;
    lbl_8055CFA0 = fn_801FB1F4(8000);
    lbl_8055FB38 = 0.0f;
    lbl_8055FB30 = new ("treehittree") TMtx*[8000];
    fn_801D0590(&lbl_8055FB40);
    fn_801D0800(&lbl_8055FB40);
    for (int i = 0; i < 16; i++) {
        memset(&lbl_804C0810[i], 0, sizeof(TreeClass));
        StrCopy(lbl_804C0810[i].name, "???INVALID???", 0x20);
    }
    for (int i = 0; i < 8; i++) {
        memset(&lbl_804C0E10[i], 0, sizeof(Shake));
    }
    lbl_8055FB34 = 0;
    fn_802C1928();
    fn_802C1A08();
}

void fn_802C13A4(void) {
    fn_801D05B4(&lbl_8055FB40);
    fn_801D0824(&lbl_8055FB40);
    TreeClass* c = lbl_804C0810;
    for (int i = 0; i < lbl_8055FB28; c++, i++) {
        if (c->drops) {
            delete[] c->drops;
        }
    }
    delete[] lbl_8055FB30;
    lbl_8055FB30 = 0;
    lbl_8055FB2C = 0;
    lbl_8055FB28 = 0;
}

int fn_802C143C(ValueNode* cfg) {
    int n = lbl_8055FB28;
    TreeClass* c = &lbl_804C0810[n];
    c->first = -1;
    lbl_8055FB28 = n + 1;
    c->wobble = 1;
    c->cfg = cfg;
    for (u32 i = 0; i < 8; i++) {
        if (cfg) {
            char key[0x20];
            fn_801345D8(key, 0x20, "efx_event_id%d", i + 1);
            c->events[i] = cfg->GetInt(key, -1);
        } else {
            c->events[i] = -1;
        }
    }
    return n;
}

u8* fn_802C1500(TMtx* t, int cls) {
    TreeClass* c = &lbl_804C0810[cls];
    int id = lbl_8055FB2C++;
    lbl_8055FB30[id] = t;
    if (c->first < 0) {
        c->first = id;
        c->last = id;
    } else {
        c->last = id;
    }
    return (u8*)lbl_8055CFA0 + id;
}

void fn_802C1550(void) {
    int i;
    TreeClass* c = lbl_804C0810;
    for (i = 0; i < lbl_8055FB28; c++, i++) {
        c->wobble = !c->cfg->GetBool("no_wobble", 0);
        ValueNode* d = c->cfg->FindChild("drop_model");
        if (d) {
            c->dropHit = d->GetFloat("drop_hit");
            const char* model = d->GetValue("model");
            int n = d->GetIntValue("count");
            c->maxDrops = n;
            c->nDrops = 0;
            c->drops = new ("tc_obst") TreeObj*[n];
            memset(c->drops, 0, c->maxDrops * 4);
            const char* name = c->cfg->GetValue("name");
            StrCopy(c->name, name, 0x20);
            if (n > 0) {
                for (int j = 0; j < n; j++) {
                TMtx m;
                m.rot[0][0] = 1.0f;
                m.rot[0][1] = 0.0f;
                m.rot[0][2] = 0.0f;
                m.rot[1][0] = 0.0f;
                m.rot[1][1] = 1.0f;
                m.rot[1][2] = 0.0f;
                m.rot[2][0] = 0.0f;
                m.rot[2][1] = 0.0f;
                m.rot[2][2] = 1.0f;
                m.pos.x = 0.0f;
                m.pos.z = 0.0f;
                m.pos.y = -1000.0f;
                fn_801FA7F4(model, &m, c->cfg);
            }
            }
        }
    }
}

void fn_802C173C(f32 time) {
    lbl_8055FB38 = time;
    Shake* s = lbl_804C0E10;
    for (int i = 0; i < 8; i++, s++) {
        if (s->tree) {
            f32 t = lbl_8055FB38 - s->start;
            if (t >= -0.01f && t < 2.0f) {
                TMtx m = s->mtx;
                TRot r;
                TPos a;
                f32 w = (f32)sin(9.0f * ((t > 0.0f) ? t : 0.0f));
                f32 k = (1.0f - t / 2.0f) * (0.17453292f * w);
                a.x = s->axis.x * k;
                a.y = s->axis.y * k;
                a.z = s->axis.z * k;
                fn_8022FF10(&r, &a);
                fn_80015B20(&m, &m, &r);
                *s->tree = m;
            } else {
                fn_802C1180(s);
            }
        }
    }
}

void fn_802C1928(void) {
    for (int i = 0; i < 8; i++) {
        lbl_804C1050[i].time = -1000.0f;
    }
    for (int i = 0; i < 8; i++) {
        fn_802C1180(&lbl_804C0E10[i]);
    }
    for (int i = 0; i < lbl_8055FB2C; i++) {
        if (lbl_8055FB30[i]->pos.y < -2000.0f) {
            lbl_8055FB30[i]->pos.y -= -9000.0f;
            fn_802BDFF4(lbl_8055FB30[i], 1);
        }
    }
}

void fn_802C1A08(void) {}

void fn_802C1A0C(TreeObj* o, int car) {
    Shake* s = lbl_804C0E10;
    for (int i = 0; i < 8; i++, s++) {
        int id = o->Handle() - (u8*)lbl_8055CFA0;
        if (s->tree && s->index == id) {
            fn_802C1180(s);
        }
    }
    int id = o->Handle() - (u8*)lbl_8055CFA0;
    if (id >= 0 && id < lbl_8055FB2C) {
        TreeClass* c = fn_802C110C(id);
        TMtx** tbl = lbl_8055FB30;
        if (tbl[id]->pos.y > -2000.0f) {
            CarRec* r = fn_8017960C(car);
            TreeEvent ev;
            ev.vel = r->vel;
            ev.hint = r->hint;
            for (u32 j = 0; j < 8; j++) {
                if (c->events[j] >= 0) {
                    ev.pos = tbl[id]->pos;
                    ev.pos.y += 1.0f;
                    ev.id = c->events[j];
                    fn_801796EC(10, &ev, sizeof(ev));
                }
            }
            tbl[id]->pos.y += -9000.0f;
            o->Hide(0);
            fn_802BDFF4(tbl[id], 0);
        }
    }
}

}  // extern "C"
