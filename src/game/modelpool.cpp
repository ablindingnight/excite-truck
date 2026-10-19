// Model pool (0x802CE3A4-0x802CE860): a fixed set of model slots streamed in through a LoadPool,
// evicting the least recently used loaded model when every slot is busy. Names are guesses.
#include "game/common.h"

extern "C" {
void fn_802E30C8(const char* fmt, ...);  // debug print
char* strcpy(char* d, const char* s);
void fn_802C6FB4(s32 model);
void fn_802C703C(s32 inst);
void fn_802C61BC(u8* data, const char* name);
void fn_802C6FA8(s32 model);
s32 fn_802C6DC4(const char* name, s32 model, int a, int b);
}

class LoadPool {
public:
    ~LoadPool(void);
};

typedef void (*LoadCallback)(u8* data, s32 size, void* arg);

extern "C" {
LoadPool* fn_8013420C(LoadPool* pool, s32 count, s32 slotSize);
void fn_8013436C(LoadPool* pool);
int fn_80134424(LoadPool* pool, const char* name, u32 type, u32* info, LoadCallback cb, void* arg, s32 offset,
                s32 size);
int fn_801345A4(LoadPool* pool, int i);
}

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

struct ModelPool;

struct ModelSlot {
    ModelSlot(void) {
        pool = 0;
        used = 0;
        state = 0;
        inst = -1;
        model = 0;
        handle = -1;
        stamp = -1;
    }
    void Unload(void) {
        if (state == 2) {
            fn_802C6FB4(model);
            fn_802C703C(inst);
            inst = -1;
            model = 0;
            state = 3;
        }
    }

    u8 used;          // offset 0x0
    ModelPool* pool;  // offset 0x4
    char name[0x20];  // offset 0x8
    s32 arg;          // offset 0x28
    s32 handle;       // offset 0x2C
    s32 model;        // offset 0x30
    s32 inst;         // offset 0x34
    s32 state;        // offset 0x38 (0 free, 1 loading, 2 loaded, 3 unloading)
    s32 stamp;        // offset 0x3C
};

struct ModelPool {
    ModelPool(s32 n, s32 m, s32 slotSize) {
        count = n;
        max = m;
        void* lp = operator new(0x10, "loadpool");
        if (lp) {
            lp = fn_8013420C((LoadPool*)lp, max, slotSize);
        }
        loads = (LoadPool*)lp;
        slots = new ("modelpool") ModelSlot[count];
        time = 0;
    }
    ~ModelPool(void) {
        delete[] slots;
        delete loads;
    }

    ModelSlot* slots;  // offset 0x0
    LoadPool* loads;   // offset 0x4
    s32 count;         // offset 0x8
    s32 max;           // offset 0xC
    s32 time;          // offset 0x10
};

ModuleTag lbl_8055FCF0;

extern "C" {

// NonMatching: Create and Update differ only in callee-saved register numbering (22 instructions).
ModelPool* fn_802CE3A4(s32 count, s32 max, s32 slotSize) { return new ("modpool") ModelPool(count, max, slotSize); }

void fn_802CE49C(ModelPool* p) { delete p; }

void fn_802CE4F4(ModelPool* p) {
    s32 oldestAge;
    int busy;
    ModelSlot* oldest;
    int i;
    ModelSlot* s;
    s32 age;
    busy = 0;
    oldest = 0;
    oldestAge = -1;
    for (i = 0; i < p->count; i++) {
        s = &p->slots[i];
        if (s->state == 3 && fn_801345A4(p->loads, s->handle)) {
            s->state = 0;
            s->handle = -1;
        }
        if (s->state != 0) {
            busy++;
        }
        if (s->state == 2) {
            age = p->time - s->stamp;
            if (age > oldestAge || !oldest) {
                oldestAge = age;
                oldest = s;
            }
        }
    }
    if (p->max - busy < 1) {
        if (oldest) {
            oldest->Unload();
        } else {
            fn_802E30C8("modpool: oldest_model is NULL (%d)", p->time);
        }
    }
    fn_8013436C(p->loads);
}

ModelSlot* fn_802CE630(ModelPool* p, const char* name, s32 arg) {
    int i;
    ModelSlot* s;
    for (i = 0; i < p->count; i++) {
        s = &p->slots[i];
        if (!s->used && s->state == 0) {
            s->used = 1;
            s->pool = p;
            strcpy(s->name, name);
            s->arg = arg;
            goto found;
        }
    }
    s = 0;
found:
    return s;
}

void fn_802CE6C0(ModelSlot* s) {
    if (s->state == 2) {
        s->Unload();
    }
    s->used = 0;
}

s32 fn_802CE728(ModelSlot* s) {
    if (s->state == 2) {
        return s->inst;
    }
    return -1;
}

void fn_802CE7D4(u8* data, s32 size, void* arg);

void fn_802CE744(ModelSlot* s) {
    ModelPool* p = s->pool;
    s->stamp = p->time++;
    if (s->state == 0) {
        u32 info = 0;
        int h = fn_80134424(p->loads, s->name, 'MODL', &info, fn_802CE7D4, s, 0, -1);
        if (h >= 0) {
            s->handle = h;
            s->state = 1;
        }
    }
}

void fn_802CE7D4(u8* data, s32 size, void* arg) {
    ModelSlot* s = (ModelSlot*)arg;
    if (s->used) {
        s->model = (s32)data;
        fn_802C61BC(data, s->name);
        fn_802C6FA8(s->model);
        s->inst = fn_802C6DC4(s->name, s->model, 0, 0);
        s->state = 2;
    } else {
        s->state = 0;
        s->handle = -1;
    }
}

s32 fn_802CE850(ModelSlot* s) { return s->state; }

}  // extern "C"
