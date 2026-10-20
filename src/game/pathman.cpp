// Path manager (0x801DA6EC-0x801DB348): objects register with a path file ("pathman" clients); paths
// that contain "loop" restart forever, others are started by name - from action sequences through
// path groups (all clients of an object, or "*", that have the path) or from the console ("pathman"
// command: "<object> <path>" or "<path>"). Every frame the active clients step along their path and
// their pose, velocity and spin are pushed to the object. Names are guesses apart from the strings.
#include "game/common.h"

struct V3 {
    f32 x;
    f32 y;
    f32 z;
};

struct PathMtx {
    f32 m[3][3];  // offset 0x0
    V3 pos;       // offset 0x24
};

struct PathFile;
struct Path {
    char name[0x10];  // offset 0x0
};

class PathFollower {
public:
    PathFollower(void);
    ~PathFollower(void);
    PathFile* file;  // offset 0x0
    Path* path;      // offset 0x4
    s32 idx;         // offset 0x8
    f32 t;           // offset 0xC
};

extern "C" {
int strcmp(const char* a, const char* b);
char* strstr(const char* s, const char* sub);
int sscanf(const char* s, const char* fmt, ...);
void fn_802E30C8(const char* fmt, ...);  // debug print
void fn_8013B490(const char* fmt, ...);  // console print
PathFile* fn_801D9CC4(const char* name);
void fn_801D9F24(PathFile* f);
void fn_801D9F84(PathFollower* p, PathFile* f);
int fn_801D9FC4(PathFollower* p, const char* name);
Path* fn_801DA03C(PathFollower* p, int i);
int fn_801DA064(PathFollower* p, const char* name);
void fn_801DA0EC(PathFollower* p, f32 dt);
void fn_801DA198(PathFollower* p, f32 t);
int fn_801DA294(PathFollower* p, PathMtx* m, V3* vel, V3* angvel);
int fn_801DA648(PathFollower* p);
int fn_801DA688(PathFollower* p);
f32 fn_801DA6B8(PathFollower* p);
f32 fn_801DD88C(f32 x, f32 z);  // ground height
f32 fn_801EA0E8(f32 x, f32 z);  // ground height
void fn_8017CFFC(void* obj, PathMtx* m, V3* vel, V3* angvel);
void fn_8029F458(const char* name, void* obj);
void fn_801D0DDC(class Updatable* u);  // register
void fn_801D0DFC(class Updatable* u);  // unregister
}

void* operator new(size_t size, const char* tag);
void* operator new[](size_t size, const char* tag);

class Updatable {
public:
    Updatable(void) { fn_801D0DDC(this); }
    virtual ~Updatable(void) { fn_801D0DFC(this); }
    virtual void vf0C(void) = 0;
    virtual void vf10(void) = 0;
    virtual void vf14(void) = 0;
    virtual void vf18(void) = 0;
};

class ConsoleCmd {
public:
    virtual int Run(const char* args) = 0;
};

// A path follower made by the path manager.
class PathMover {
public:
    virtual ~PathMover(void) {}
    virtual f32 Length(void) = 0;
    virtual f32 Time(void) = 0;
    virtual void Start(f32 speed) = 0;
    virtual int Done(void) = 0;
    virtual void Reset(void) = 0;
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

namespace {

struct PathClient {
    char name[0x10];         // offset 0x0
    u8 grounded;             // offset 0x10
    f32 groundOffset;        // offset 0x14
    u8 active;               // offset 0x18
    u8 loop;                 // offset 0x19
    f32 speed;               // offset 0x1C
    void* obj;               // offset 0x20
    PathFile* file;          // offset 0x24
    PathFollower* follower;  // offset 0x28
};

class PathGroup : public PathMover {
public:
    PathGroup(const char* path, int n) {
        cap = n;
        StrCopy(name, path, 0x10);
        count = 0;
        data = new ("pathclient") PathClient*[cap];
    }
    virtual ~PathGroup(void);
    virtual f32 Length(void);
    virtual f32 Time(void);
    virtual void Start(f32 speed);
    virtual int Done(void);
    virtual void Reset(void);
    void Add(PathClient* c) { data[count++] = c; }

    PathClient** data;  // offset 0x4
    s32 count;          // offset 0x8
    s32 cap;            // offset 0xC
    char name[0x10];    // offset 0x10
};

class PathMan;
}
extern PathMan* lbl_8055E984;
namespace {

class PathMan : public Updatable, public ConsoleCmd {
public:
    PathMan(void) {
        lbl_8055E984 = this;
        count = 0;
    }
    virtual ~PathMan(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void vf18(void);
    virtual int Run(const char* args);

    PathClient clients[0x200];  // offset 0x8
    s32 count;                  // offset 0x5808
};

}  // namespace

ModuleTag lbl_8055E978;
u8 lbl_8055E980;  // warned about uninitialized memory
PathMan* lbl_8055E984;

extern "C" {

void fn_801DA6EC(void) { new ("pathman") PathMan(); }

void fn_801DA758(void* obj, const char* name, const char* file) {
    int i;
    PathMan* m = lbl_8055E984;
    PathClient* c = &m->clients[m->count++];
    StrCopy(c->name, name, 0x10);
    c->obj = obj;
    c->follower = new ("node") PathFollower();
    c->file = fn_801D9CC4(file);
    fn_801D9F84(c->follower, c->file);
    c->loop = 0;
    i = 0;
    for (;;) {
        Path* p = fn_801DA03C(c->follower, i++);
        if (!p) {
            break;
        }
        if (strstr(p->name, "loop")) {
            c->loop = 1;
            break;
        }
    }
    c->active = c->loop;
    if (c->loop) {
        c->speed = 1.0f;
    }
    c->grounded = 0;
    if (c->grounded) {
        V3 pos = {0.0f, 0.0f, 0.0f};
        c->groundOffset = pos.y - fn_801DD88C(pos.x, pos.z);
    } else {
        c->groundOffset = 0.0f;
    }
    fn_8029F458(name, obj);
}

PathMover* fn_801DA8C8(const char* object, const char* path) {
    PathMan* m = lbl_8055E984;
    PathClient* found[0x200];
    int n = 0;
    int any = strcmp(object, "*") == 0;
    for (int i = 0; i < m->count; i++) {
        PathClient* c = &m->clients[i];
        if ((any || strcmp(c->name, object) == 0) && fn_801D9FC4(c->follower, path)) {
            found[n++] = c;
        }
    }
    PathGroup* g = 0;
    if (n > 0) {
        g = new ("pathmanpath") PathGroup(path, n);
        for (int j = 0; j < n; j++) {
            g->Add(found[j]);
        }
    }
    return g;
}

}  // extern "C"

PathGroup::~PathGroup(void) { delete[] data; }

static inline f32 MaxF(f32 a, f32 b) { return (a > b) ? a : b; }

f32 PathGroup::Length(void) {
    f32 r = 0.0f;
    for (int i = 0; i < count; i++) {
        r = MaxF(r, fn_801DA6B8(data[i]->follower));
    }
    return r;
}

f32 PathGroup::Time(void) {
    f32 r = 0.0f;
    for (int i = 0; i < count; i++) {
        r = MaxF(r, data[i]->follower->t);
    }
    return r;
}

void PathGroup::Start(f32 speed) {
    f32 t = (speed >= 0.0f) ? 0.0f : Length();
    for (int i = 0; i < count; i++) {
        PathClient* c = data[i];
        if (c->active) {
            fn_802E30C8("pathman: client %d already active (%s)", i, name);
        }
        c->active = 1;
        c->speed = speed;
        fn_801DA064(c->follower, name);
        fn_801DA198(c->follower, t);
    }
}

int PathGroup::Done(void) {
    for (int i = 0; i < count; i++) {
        if (data[i]->active) {
            return 0;
        }
    }
    return 1;
}

void PathGroup::Reset(void) {
    for (int i = 0; i < count; i++) {
        fn_801DA198(data[i]->follower, 0.0f);
    }
}

PathMan::~PathMan(void) {
    lbl_8055E984 = 0;
    for (int i = 0; i < count; i++) {
        PathClient* c = &clients[i];
        delete c->follower;
        fn_801D9F24(c->file);
    }
}

void PathMan::vf0C(void) {}

void PathMan::vf18(void) {}

void PathMan::vf10(void) {
    for (int i = 0; i < count; i++) {
        PathClient* c = &clients[i];
        c->active = c->loop;
        fn_801DA198(c->follower, 0.0f);
    }
}

void PathMan::vf14(void) {
    for (int i = 0; i < count; i++) {
        PathClient* c = &clients[i];
        if (c->active || c->grounded) {
            if (c->active) {
                fn_801DA0EC(c->follower, 0.016683352f * c->speed);
            }
            PathMtx mtx;
            V3 vel;
            V3 spin;
            if (fn_801DA294(c->follower, &mtx, &vel, &spin)) {
                vel.x *= 59.94f;
                vel.y *= 59.94f;
                vel.z *= 59.94f;
                spin.x *= 59.94f;
                spin.y *= 59.94f;
                spin.z *= 59.94f;
                if (c->grounded) {
                    mtx.pos.y = fn_801EA0E8(mtx.pos.x, mtx.pos.z) + c->groundOffset;
                }
                fn_8017CFFC(c->obj, &mtx, &vel, &spin);
            } else if (!lbl_8055E980) {
                lbl_8055E980 = 1;
                fn_802E30C8("%s", "pathman: would have used uninitialized memory before");
            }
            if (c->speed >= 0.0f) {
                if (fn_801DA648(c->follower)) {
                    if (c->loop) {
                        fn_801DA198(c->follower, 0.0f);
                    } else {
                        c->active = 0;
                    }
                }
            } else if (fn_801DA688(c->follower) && !c->loop) {
                c->active = 0;
            }
        }
    }
}

static inline int Trigger(PathMan* m, const char* object, const char* path) {
    int found = 0;
    int any = strcmp(object, "*") == 0;
    for (int i = 0; i < m->count; i++) {
        PathClient* c = &m->clients[i];
        if ((any || strcmp(c->name, object) == 0) && fn_801DA064(c->follower, path)) {
            fn_801DA198(c->follower, 0.0f);
            c->active = 1;
            found = 1;
            c->speed = 1.0f;
        }
    }
    return found;
}

int PathMan::Run(const char* args) {
    char object[0x40];
    char path[0x40];
    if (sscanf(args, "%s %s", object, path) == 2) {
        if (Trigger(this, object, path)) {
            fn_8013B490("pathman: triggered %s, %s", object, path);
        } else {
            fn_8013B490("pathman: %s, %s not found", object, path);
        }
    } else if (sscanf(args, "%s", path) == 1) {
        if (Trigger(this, "*", path)) {
            fn_8013B490("pathman: triggered %s", path);
        } else {
            fn_8013B490("pathman: %s not found", path);
        }
    }
    return 1;
}
