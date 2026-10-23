// UI contexts (0x8022E9E0-0x8022F248): the stack of UI drawing contexts (a 2D transform plus fade
// values) pushed while menus draw, with pooled allocation for the contexts and stack nodes. Names
// are guesses apart from the messages.
#include "game/common.h"

void* operator new(size_t size, const char* tag);

extern "C" {
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
}

f32 Dot(f32 a0, f32 b0, f32 a1, f32 b1, f32 a2, f32 b2);  // a0*b0 + a1*b1 + a2*b2 (modeinl.cpp)

// A fixed-size block pool.
// Fixed-size block pool (pool.cpp).
class Pool {
public:
    Pool(const char* name, s32 count, s32 size);
    virtual ~Pool(void);
    void* Alloc(void);
    void Free(void* p);
    u8 pad4[0x20 - 0x4];
};

struct UIXform {
    f32 m[3][3];  // offset 0x0
    f32 t[3];     // offset 0x24
};

class UIContext {
public:
    static void* operator new(size_t size, const char* tag);
    static void operator delete(void* p);

    UIXform xf;   // offset 0x0
    f32 alpha;    // offset 0x30
    f32 unk34;    // offset 0x34
    f32 unk38;    // offset 0x38
};

class cnode {
public:
    static void* operator new(size_t size, const char* tag);
    static void operator delete(void* p);

    UIContext* ctx;  // offset 0x0
    cnode* next;     // offset 0x4
};

struct UIContextTracker {
    cnode* head;  // offset 0x0
    cnode* top;   // offset 0x4
};

ModuleTag lbl_8055EFE8;
UnkStatic7F lbl_8055EFF0;
int lbl_8055EFF4;             // cnode pool users
Pool* lbl_8055EFF8;        // cnode pool
int lbl_8055EFFC;             // UIContext pool users
Pool* lbl_8055F000;        // UIContext pool
UIContextTracker* lbl_8055F004;

void* cnode::operator new(size_t size, const char* tag) {
    if (lbl_8055EFF4 == 0) {
        lbl_8055EFF8 = new ("cnode::pool") Pool("cnode::pool", 0x20, 8);
    }
    void* node = lbl_8055EFF8->Alloc();
    if (node) {
        lbl_8055EFF4++;
    }
    return node;
}

void cnode::operator delete(void* p) {
    lbl_8055EFF8->Free(p);
    if (--lbl_8055EFF4 == 0) {
        delete lbl_8055EFF8;
        lbl_8055EFF8 = NULL;
    }
}

void* UIContext::operator new(size_t size, const char* tag) {
    if (lbl_8055EFFC == 0) {
        lbl_8055F000 = new ("UIContext::pool") Pool("UIContext::pool", 0x20, 0x3C);
    }
    void* ctx = lbl_8055F000->Alloc();
    if (ctx) {
        lbl_8055EFFC++;
    }
    return ctx;
}

void UIContext::operator delete(void* p) {
    lbl_8055F000->Free(p);
    if (--lbl_8055EFFC == 0) {
        delete lbl_8055F000;
        lbl_8055F000 = NULL;
    }
}

void UIContextBegin(void) {
    if (lbl_8055F004) {
        fn_802E317C("UIContextBegin called twice! UIContextBegin called twice! see how annoying it is?");
    }
    lbl_8055F004 = (UIContextTracker*)::operator new(sizeof(UIContextTracker), "UIContextTracker");
    lbl_8055F004->head = NULL;
    lbl_8055F004->top = lbl_8055F004->head;
}

extern "C" {
cnode* fn_8022EC7C(void);
void fn_8022ED50(cnode* n);
}

void UIContextEnd(void) {
    if (lbl_8055F004->top) {
        fn_802E30C8("UIcontext: Some contexts not popped.  Freeing memory now.");
        for (cnode* n = fn_8022EC7C(); n; n = fn_8022EC7C()) {
            fn_8022ED50(lbl_8055F004->top);
        }
        delete lbl_8055F004->top;
        lbl_8055F004->top = NULL;
        lbl_8055F004->head = NULL;
    }
    ::operator delete(lbl_8055F004);
    lbl_8055F004 = NULL;
}

extern "C" {

// Returns the node below the top of the stack.
cnode* fn_8022EC7C(void) {
    cnode* n = lbl_8055F004->head;
    if (!n) {
        return NULL;
    }
    cnode* top = lbl_8055F004->top;
    if (n == top) {
        return NULL;
    }
    for (; n->next != top; n = n->next) {
    }
    return n;
}

cnode* fn_8022ECC0(UIContext* ctx) {
    cnode* n = new ("cnode") cnode;
    n->ctx = ctx;
    n->next = NULL;
    return n;
}

void fn_8022ED00(UIContext* ctx) {
    cnode* n = fn_8022ECC0(ctx);
    if (lbl_8055F004->top == NULL) {
        lbl_8055F004->head = n;
        lbl_8055F004->top = lbl_8055F004->head;
    } else {
        lbl_8055F004->top->next = n;
        lbl_8055F004->top = n;
    }
}

void fn_8022ED50(cnode* n) {
    cnode* prev = fn_8022EC7C();
    if (!prev) {
        lbl_8055F004->top = NULL;
        lbl_8055F004->head = lbl_8055F004->top;
    } else {
        prev->next = NULL;
        lbl_8055F004->top = prev;
    }
    delete n;
}

void fn_8022EDB8(void) { fn_8022ED50(lbl_8055F004->top); }

UIContext* fn_8022EDC4(void) {
    if (lbl_8055F004->top) {
        return lbl_8055F004->top->ctx;
    }
    return NULL;
}

UIContext* fn_8022EDE4(const UIXform* xf, f32 alpha) {
    UIContext* c = new ("UIContext") UIContext;
    c->xf = *xf;
    c->alpha = alpha;
    c->unk34 = 1.0f;
    c->unk38 = 1.0f;
    return c;
}

void fn_8022EE98(UIContext* c) { delete c; }

UIContext* fn_8022EE9C(void) {
    UIXform xf;
    xf.m[0][0] = 1.0f;
    xf.m[0][1] = 0.0f;
    xf.m[0][2] = 0.0f;
    xf.m[1][0] = 0.0f;
    xf.m[1][1] = 1.0f;
    xf.m[1][2] = 0.0f;
    xf.m[2][0] = 0.0f;
    xf.m[2][1] = 0.0f;
    xf.m[2][2] = 1.0f;
    xf.t[0] = 0.0f;
    xf.t[1] = 0.0f;
    xf.t[2] = 0.0f;
    return fn_8022EDE4(&xf, 1.0f);
}

void fn_8022EEF8(UIContext* out, const UIContext* l, const UIContext* r) {
    out->alpha = l->alpha * r->alpha;
    out->unk34 = l->unk34 * r->unk34;
    out->unk38 = l->unk38 * r->unk38;
    if (out == l || out == r) {
        f32 m00 = Dot(r->xf.m[0][0], l->xf.m[0][0], r->xf.m[1][0], l->xf.m[0][1], r->xf.m[2][0], l->xf.m[0][2]);
        f32 m10 = Dot(r->xf.m[0][0], l->xf.m[1][0], r->xf.m[1][0], l->xf.m[1][1], r->xf.m[2][0], l->xf.m[1][2]);
        f32 m20 = Dot(r->xf.m[0][0], l->xf.m[2][0], r->xf.m[1][0], l->xf.m[2][1], r->xf.m[2][0], l->xf.m[2][2]);
        f32 m01 = Dot(r->xf.m[0][1], l->xf.m[0][0], r->xf.m[1][1], l->xf.m[0][1], r->xf.m[2][1], l->xf.m[0][2]);
        f32 m11 = Dot(r->xf.m[0][1], l->xf.m[1][0], r->xf.m[1][1], l->xf.m[1][1], r->xf.m[2][1], l->xf.m[1][2]);
        f32 m21 = Dot(r->xf.m[0][1], l->xf.m[2][0], r->xf.m[1][1], l->xf.m[2][1], r->xf.m[2][1], l->xf.m[2][2]);
        f32 m02 = Dot(r->xf.m[0][2], l->xf.m[0][0], r->xf.m[1][2], l->xf.m[0][1], r->xf.m[2][2], l->xf.m[0][2]);
        f32 m12 = Dot(r->xf.m[0][2], l->xf.m[1][0], r->xf.m[1][2], l->xf.m[1][1], r->xf.m[2][2], l->xf.m[1][2]);
        f32 m22 = Dot(r->xf.m[0][2], l->xf.m[2][0], r->xf.m[1][2], l->xf.m[2][1], r->xf.m[2][2], l->xf.m[2][2]);
        out->xf.m[0][0] = m00;
        out->xf.m[1][0] = m10;
        out->xf.m[2][0] = m20;
        out->xf.m[0][1] = m01;
        out->xf.m[1][1] = m11;
        out->xf.m[2][1] = m21;
        out->xf.m[0][2] = m02;
        out->xf.m[1][2] = m12;
        out->xf.m[2][2] = m22;
    } else {
        out->xf.m[0][0] = Dot(r->xf.m[0][0], l->xf.m[0][0], r->xf.m[1][0], l->xf.m[0][1], r->xf.m[2][0], l->xf.m[0][2]);
        out->xf.m[1][0] = Dot(r->xf.m[0][0], l->xf.m[1][0], r->xf.m[1][0], l->xf.m[1][1], r->xf.m[2][0], l->xf.m[1][2]);
        out->xf.m[2][0] = Dot(r->xf.m[0][0], l->xf.m[2][0], r->xf.m[1][0], l->xf.m[2][1], r->xf.m[2][0], l->xf.m[2][2]);
        out->xf.m[0][1] = Dot(r->xf.m[0][1], l->xf.m[0][0], r->xf.m[1][1], l->xf.m[0][1], r->xf.m[2][1], l->xf.m[0][2]);
        out->xf.m[1][1] = Dot(r->xf.m[0][1], l->xf.m[1][0], r->xf.m[1][1], l->xf.m[1][1], r->xf.m[2][1], l->xf.m[1][2]);
        out->xf.m[2][1] = Dot(r->xf.m[0][1], l->xf.m[2][0], r->xf.m[1][1], l->xf.m[2][1], r->xf.m[2][1], l->xf.m[2][2]);
        out->xf.m[0][2] = Dot(r->xf.m[0][2], l->xf.m[0][0], r->xf.m[1][2], l->xf.m[0][1], r->xf.m[2][2], l->xf.m[0][2]);
        out->xf.m[1][2] = Dot(r->xf.m[0][2], l->xf.m[1][0], r->xf.m[1][2], l->xf.m[1][1], r->xf.m[2][2], l->xf.m[1][2]);
        out->xf.m[2][2] = Dot(r->xf.m[0][2], l->xf.m[2][0], r->xf.m[1][2], l->xf.m[2][1], r->xf.m[2][2], l->xf.m[2][2]);
    }
    out->xf.t[0] = l->xf.t[0] + r->xf.t[0];
    out->xf.t[1] = l->xf.t[1] + r->xf.t[1];
    out->xf.t[2] = l->xf.t[2] + r->xf.t[2];
}

}  // extern "C"
