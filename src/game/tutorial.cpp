// Tutorial challenges (0x80160CE4-0x801659D0): the challenge list for a tutorial event ("challenge" entries:
// stunt challenges with a list of required stunts, freestyle challenges with a score threshold), their HUD
// pop-ups and results screen, and the save-slot progress through the tutorial events. Names are guesses
// apart from the strings. FIRST DRAFT: the challenge classes' methods are not written yet.
#include "game/common.h"

extern "C" {
int strcasecmp(const char* a, const char* b);
void fn_802E30C8(const char* fmt, ...);  // debug print
int fn_801345D8(char* buf, u32 size, const char* fmt, ...);  // snprintf
s32 fn_802CAC28(const char* name, int flags);  // load texture
void fn_802CAC54(s32 tex);
s32 fn_8023512C(const char* name);
void fn_80235380(s32 art);
void fn_80176734(void);
int fn_80158618(const char* name);  // stunt type by name
void fn_80158558(void* w);          // add stunt watcher
void fn_801332D0(void* slot, const char* key, s32* value);
void fn_80133348(void* slot, const char* key, s32 value);
void fn_800121A4(void);
void fn_80137C78(int a);
void* fn_802E4A10(void);
void fn_80137C84(void* a, int b);
void fn_80137C80(int a);
void fn_8023BEC4(int event, void* out);
void fn_802E7128(void);
void fn_80148070(void* elem, u32 color, void* art);
void fn_80148134(f32 x, f32 y, f32* ox, f32* oy, s32 mode);
void fn_800103B8(f32 x, f32 y, f32 w, f32 h, s32 a, s32 b, const char* fmt, ...);
void fn_801473F8(void* layout, const char* elem, const char* fmt, ...);
void fn_801476C4(void* elem);
}

void* operator new(size_t size, const char* tag);
int IdentGetCurrentSlot(void);
const char* IdentGetSlotName(int slot);
void SetPlayerName(const char* name, int player);
void SetPlayer(int player, long driver, long vehicle, unsigned long color);
void AchievementsClearSession(void);
int GetNumEvents(void);

struct EventDesc {
    s32 event;  // offset 0x0
};

struct Event {
    s32 id;  // offset 0x0
    u8 pad4[0x20 - 0x4];
    ValueNode* vals;  // offset 0x20
};

extern "C" EventDesc* fn_801769CC(void);
Event* GetEvent(int id);

struct DriverInfo {
    u8 pad0[0x4];
    s32 id;  // offset 0x4
    u8 pad8[0x50 - 0x8];
    u32 color;  // offset 0x50
};

struct VehicleInfo {
    u8 pad0[0x4];
    s32 id;  // offset 0x4
    u8 pad8[0x64 - 0x8];
    u32 colors[1];  // offset 0x64
};

DriverInfo* GetDriver(int i);
VehicleInfo* GetDriverVehicle(long driver);
VehicleInfo* FindVehicleNamed(const char* name, int n);
VehicleInfo* FindVehicle(long id);

// A loaded overlay layout (tutovr.val / challist.val).
class Layout {
public:
    virtual void vf08(void);
    virtual void* Find(const char* name);   // 0x0C
    virtual void* Art(const char* name);    // 0x10
    virtual void* Elem(const char* name);   // 0x14
    virtual void* Image(const char* name);  // 0x18
};

extern "C" Layout* fn_80145E70(const char* name);

struct TextBox {
    u8 pad0[0x4];
    f32 x;     // offset 0x4
    f32 y;     // offset 0x8
    s32 mode;  // offset 0xC
    f32 w;     // offset 0x10
    s32 b;     // offset 0x14
    s32 a;     // offset 0x18
};

// One challenge (and the list screen): the interface the tutorial drives.
class Task {
public:
    virtual ~Task(void) {}                                // 0x08
    virtual void Init(void) = 0;                          // 0x0C
    virtual void Shutdown(void) = 0;                      // 0x10
    virtual void Load(void) = 0;                          // 0x14
    virtual void Unload(void) = 0;                        // 0x18
    virtual void Update(void) = 0;                        // 0x1C
    virtual void Start(void) = 0;                         // 0x20
    virtual int IsDone(void) = 0;                         // 0x24
    virtual int IsPassed(void) = 0;                       // 0x28
    virtual int State(void) = 0;                          // 0x2C
    virtual int CanContinue(void) = 0;                    // 0x30
    virtual f32 Time(void) = 0;                           // 0x34
    virtual int Get(f32* out, int i) = 0;                 // 0x38
    virtual int Query(int a, int b, int c) = 0;           // 0x3C
    virtual void Event(int a, int b, int c) = 0;          // 0x40
    virtual int Draw(int a) = 0;                          // 0x44
    virtual int DrawResults(int a) = 0;                   // 0x48
    virtual void Draw3D(void) = 0;                        // 0x4C
    virtual int AllowPause(void) = 0;                     // 0x50
    virtual int AllowExit(void) = 0;                      // 0x54
    virtual void Camera(void) = 0;                        // 0x58
    virtual void vf5C(void) = 0;                          // 0x5C
    virtual void SetResult(int r) = 0;                    // 0x60
    virtual int TimedOut(void) = 0;                       // 0x64
};

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

struct TaskList : ArrayRef<Task*> {
    TaskList(void) : ArrayRef<Task*>(storage, 16) {}
    Task* storage[16];
};

const char* lbl_8055A0B0 = "tutorial_level";
s32 lbl_8055A0B4 = 1;  // tutorial level reached
s32 lbl_8055A0B8 = 1;  // tutorial level saved
s32 lbl_8055A0BC = -1;
s32 lbl_8055A0C0 = -1;

ModuleTag lbl_8055E2D0;
UnkStatic7F lbl_8055E2D8;
u8 lbl_8055E2DC;     // passed a new level
u8 lbl_8055E2DD;     // skip requested
Layout* lbl_8055E2E0;  // challist.val
Layout* lbl_8055E2E4;  // tutovr.val
s32 lbl_8055E2E8;  // bar1_top.tex
s32 lbl_8055E2EC;  // bar1_bot.tex
s32 lbl_8055E2F0;  // tut_pass.tex
s32 lbl_8055E2F4;  // tut_fail.tex
s32 lbl_8055E2F8;  // tut_timer.tex
s32 lbl_8055E2FC;  // tut_box.art
Task* lbl_8055E300;  // current task
s32 lbl_8055E304;    // current challenge index
TaskList lbl_8042FDEC;

extern "C" {

int fn_80160CE4(void) { return lbl_8055A0B8 - lbl_8055A0C0; }

int fn_80161C38(void);

int fn_801613E0(int key) {
    if (fn_80161C38()) {
        switch (key) {
        case 3:
            lbl_8055E304 = 0;
            fn_80176734();
            return 1;
        }
    }
    return 0;
}

void fn_80161438(const char* title, const char* msg) {
    if (lbl_8055E2E0) {
        void* box = lbl_8055E2E0->Elem("popup_box");
        TextBox* t;
        f32 x;
        f32 y;
        if (box) {
            fn_80148070(box, 0x96FFFFFF, lbl_8055E2E0->Image("popup_box_art"));
        }
        t = (TextBox*)lbl_8055E2E0->Elem("popup_title");
        if (t) {
            fn_80148070(t, -1, lbl_8055E2E0->Image("popup_box_art"));
        }
        t = (TextBox*)lbl_8055E2E0->Find("popup_msg");
        fn_80148134(t->x, t->y, &y, &x, t->mode);
        fn_800103B8(t->w, y, x, ((TextBox*)box)->w - 34.0f, t->a, t->b, "%s", msg);
        fn_801473F8(lbl_8055E2E0, "popup_title_elem", "%s", title);
        fn_801476C4(lbl_8055E2E0->Find("continue"));
    }
}

Event* fn_80161E40(int n);
int fn_8016208C(int event);

int fn_801615B4(int n) {
    int total = 0;
    int i;
    for (i = 0; i < n; i++) {
        total += fn_8016208C((int)fn_80161E40(i));
    }
    return total;
}

void fn_80160CF4(void);

void fn_80161618(void) {
    int i;
    lbl_8055A0B8 = 1;
    fn_801332D0((u8*)IdentGetCurrentSlot() + 0xC, lbl_8055A0B0, &lbl_8055A0B8);
    lbl_8055E2DC = 0;
    lbl_8055A0B4 = lbl_8055A0B8;
    lbl_8055E2DD = 0;
    fn_80160CF4();
    if (lbl_8042FDEC.count > 1) {
        lbl_8055E304 = 0;
        lbl_8055E300 = lbl_8042FDEC.data[0];
    } else {
        lbl_8055E300 = 0;
        lbl_8055E304 = -1;
    }
    for (i = 0; i < lbl_8042FDEC.count; i++) {
        lbl_8042FDEC.data[i]->Init();
    }
}

void fn_801616FC(void) {
    int i;
    fn_80133348((u8*)IdentGetCurrentSlot() + 0xC, lbl_8055A0B0, lbl_8055A0B8);
    for (i = 0; i < lbl_8042FDEC.count; i++) {
        lbl_8042FDEC.data[i]->Shutdown();
        delete lbl_8042FDEC.data[i];
    }
    lbl_8042FDEC.count = 0;
    lbl_8055E300 = 0;
}

void fn_801617BC(void) {
    int i;
    lbl_8055E2E0 = fn_80145E70("challist.val");
    lbl_8055E2E4 = fn_80145E70("tutovr.val");
    lbl_8055E2E8 = fn_802CAC28("bar1_top.tex", 0);
    lbl_8055E2EC = fn_802CAC28("bar1_bot.tex", 0);
    lbl_8055E2F0 = fn_802CAC28("tut_pass.tex", 0);
    lbl_8055E2F4 = fn_802CAC28("tut_fail.tex", 0);
    lbl_8055E2F8 = fn_802CAC28("tut_timer.tex", 0);
    lbl_8055E2FC = fn_8023512C("tut_box.art");
    for (i = 0; i < lbl_8042FDEC.count; i++) {
        lbl_8042FDEC.data[i]->Load();
    }
}

void fn_801618B4(void) {
    int i;
    fn_802CAC54(lbl_8055E2E8);
    fn_802CAC54(lbl_8055E2EC);
    fn_802CAC54(lbl_8055E2F0);
    fn_802CAC54(lbl_8055E2F4);
    fn_802CAC54(lbl_8055E2F8);
    fn_80235380(lbl_8055E2FC);
    delete lbl_8055E2E0;
    delete lbl_8055E2E4;
    for (i = 0; i < lbl_8042FDEC.count; i++) {
        lbl_8042FDEC.data[i]->Unload();
    }
}

void fn_801619A8(void) {
    if (lbl_8055E304 >= 0 && lbl_8055E304 < lbl_8042FDEC.count) {
        lbl_8055E300 = lbl_8042FDEC.data[lbl_8055E304];
    }
    if (lbl_8055E300) {
        lbl_8055E300->Start();
    }
    lbl_8055E2DD = 0;
}

void fn_80161A18(void) {
    if (lbl_8055E300) {
        lbl_8055E300->Update();
    }
}

void fn_80161A38(int a, int b, int c) {
    if (lbl_8055E300) {
        lbl_8055E300->Event(a, b, c);
    }
}

int fn_80161A70(void) {
    if (lbl_8055E300) {
        if (lbl_8055E300->IsDone()) {
            return lbl_8055E300->DrawResults(0);
        }
        return lbl_8055E300->Draw(0);
    }
    return 0;
}

void fn_80161AEC(void) {
    if (lbl_8055E300) {
        lbl_8055E300->Draw3D();
    }
}

int fn_80161B0C(void) {
    if (lbl_8055E300 && lbl_8055E300->CanContinue()) {
        if (!lbl_8055E2DD) {
            fn_800121A4();
        }
        lbl_8055E2DD = 0;
        return 1;
    }
    return 0;
}

f32 fn_80161B70(void) {
    if (lbl_8055E300) {
        return lbl_8055E300->Time();
    }
    return 0.0f;
}

int fn_80161B94(void) {
    if (lbl_8055E300) {
        return lbl_8055E300->State();
    }
    return 0;
}

int fn_80161BB8(void) {
    if (lbl_8055E300) {
        return lbl_8055E300->IsDone();
    }
    return 0;
}

int fn_80161BDC(int a, int b, int c) {
    if (lbl_8055E300) {
        return lbl_8055E300->Query(a, b, c);
    }
    return 0;
}

void fn_80161C18(void) {
    if (lbl_8055E300) {
        lbl_8055E300->Camera();
    }
}

int fn_80161C38(void) { return lbl_8055E300 != 0; }

int fn_80161C4C(void) {
    if (lbl_8055E300) {
        return lbl_8055E300->AllowPause();
    }
    return 1;
}

int fn_80161C70(void) {
    if (lbl_8055E300) {
        return lbl_8055E300->AllowExit();
    }
    return 1;
}

}  // extern "C"

class CSubTaskMgr {
public:
    virtual ~CSubTaskMgr(void);
};

class ChalSubTaskMgr : public CSubTaskMgr {
public:
    virtual ~ChalSubTaskMgr(void) {}
};

static inline int HasNode(ValueNode* n, const char* name) { return n->FindChild(name) != 0; }

extern "C" {

CSubTaskMgr* fn_80161C94(void) {
    if (HasNode(GetEvent(fn_801769CC()->event)->vals, "challenge")) {
        return new ("chalsubtaskmgr") ChalSubTaskMgr;
    }
    return 0;
}

s32 fn_80161D14(void) {
    s32 v = 1;
    fn_801332D0((u8*)IdentGetCurrentSlot() + 0xC, lbl_8055A0B0, &v);
    return v;
}

int fn_80161ECC(void);

int fn_80161D50(void) {
    int n = fn_80161ECC();
    int total = 0;
    int i;
    for (i = 0; i < n; i++) {
        Event* e = GetEvent((int)fn_80161E40(i));
        if (e->vals->GetBool("mandatory", 0)) {
            total += fn_8016208C(e->id);
        }
    }
    return total;
}

void fn_80161DE4(void) {
    int n = fn_801615B4(fn_80161ECC() - 1);
    n = fn_8016208C((int)fn_80161E40(fn_80161ECC() - 1)) + n;
    fn_80133348((u8*)IdentGetCurrentSlot() + 0xC, lbl_8055A0B0, n + 1);
}

Event* fn_80161E40(int n) {
    int i;
    for (i = 0; i < GetNumEvents(); i++) {
        if (n == GetEvent(i)->vals->GetInt("tutorial", -999)) {
            return (Event*)i;
        }
    }
    return (Event*)-1;
}

int fn_80161ECC(void) {
    int n = 0;
    while ((int)fn_80161E40(n) >= 0) {
        n++;
    }
    return n;
}

void fn_80161F10(int n) {
    int ev;
    Event* e;
    s32 driver;
    s32 vehicle;
    u32 color;
    u8 buf[0xC8];
    fn_80161D14();
    ev = (int)fn_80161E40(n);
    e = GetEvent(ev);
    fn_80137C78(1);
    fn_80137C84(fn_802E4A10(), 0);
    driver = GetDriver(0)->id;
    vehicle = GetDriverVehicle(driver)->id;
    if (e->vals->Has("player_car")) {
        vehicle = FindVehicleNamed(e->vals->GetValue("player_car"), 0)->id;
    }
    color = GetDriver(driver)->color;
    if (e->vals->Has("player_color")) {
        s32 c = e->vals->GetIntValue("player_color");
        color = FindVehicle(vehicle)->colors[c];
    }
    SetPlayerName(IdentGetSlotName(IdentGetCurrentSlot()), 0);
    SetPlayer(0, driver, vehicle, color);
    lbl_8055A0BC = n;
    lbl_8055A0C0 = fn_801615B4(n);
    fn_8023BEC4(ev, buf);
    lbl_8055A0BC = -1;
    lbl_8055A0C0 = -1;
    fn_80137C80(0);
    AchievementsClearSession();
    fn_80161D14();
    fn_802E7128();
}

int fn_8016208C(int event) {
    Event* e = GetEvent(event);
    int count = 0;
    int i = 0;
    ValueNode* n;
    while ((n = e->vals->GetChildAt(i++)) != 0) {
        if (strcasecmp(n->GetName(), "challenge") == 0) {
            count++;
        }
    }
    return count;
}

void fn_80160CF4(void) {}

}  // extern "C"
