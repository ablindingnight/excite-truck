// Career event path screen (0x8027E908-0x8028777C): the career map ("EventPath:..."), unlock
// announcements ("unlocked_career", "CAREER_DIFFICULTY_%d_UNLOCKED", "CONGRATULATIONS"), the
// bracket/contest selection lists and the animation wait list ("add_anim_to_wait_for"). Written
// against raw field offsets; the large screen routines are still stubs. Names are guesses apart from
// the strings.
#include "game/common.h"
#include "game/menu.h"
#include "game/menustate.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

// The screen's image/font/sound set (shared UIObjects implementation, see uiobjs.cpp).
class UIObjects {
public:
    virtual ~UIObjects(void);
    virtual void AddImage(const char* name, const char* file, int frame);
    virtual void AddFont(const char* name, const char* file);
    virtual void AddSound(const char* name, const char* file, u8 loop);
    virtual void s18(void);
    virtual void s1C(void);
    virtual void s20(void);
    virtual void s24(void);
    virtual const char* GetString(u32 id);  // 0x28
    virtual s32 GetInt(u32 id);             // 0x2C
    virtual void s30(void);
    virtual void s34(void);
    virtual void PlaySound(const char* name);  // 0x38
};

class VR {
public:
    virtual ~VR(void);
    virtual void v0C(void);
    virtual u32 v10(void);
    virtual void v14(void);
    virtual int v18(void);
    virtual u8* v1C(s32 x);
    virtual u8* v20(void);
    virtual int v24(void);
    virtual int v28(void);
    virtual void v2C(void);
    virtual void v30(int a);
    virtual void v34(void);
    virtual void v38(const char* s);
    virtual int v3C(void);
    virtual int v40(void);
    virtual void v44(void);
    virtual int v48(void);
    virtual int v4C(void);
};

#define VR(p) ((VR*)(p))

// A list with an inline buffer; the base destructor clears the header.
template <typename T>
class ArrayBase {
public:
    ~ArrayBase(void) {
        data = 0;
        count = 0;
        cap = 0;
    }
    int Count(void) const { return count; }
    template <typename V>
    int Find(V* v) {
        for (int i = 0; i < Count(); i++) {
            if ((V*)data[i] == v) {
                return i;
            }
        }
        return -1;
    }
    template <typename V>
    void Remove(V* v) {
        int i = Find(v);
        if (i == -1) {
            return;
        }
        if (i >= 0 && i < count) {
            int last = count - 1;
            if (i != last) {
                data[i] = data[last];
            }
            count--;
        }
    }
    T* data;    // offset 0x0
    s32 count;  // offset 0x4
    s32 cap;    // offset 0x8
};

template <typename T, int N>
class FixedArray : public ArrayBase<T> {
public:
    FixedArray(void) {
        this->data = buf;
        this->cap = N;
        this->count = 0;
    }
    T buf[N];  // offset 0xC
};

const char* GetEventTypeName(u32 type);

extern "C" {
void fn_80236914(void);
void fn_80216D28(void);
void fn_802E30C8(const char* fmt, ...);
void fn_8025C204(u8* p, s32 a);
f32 fn_8023659C(void);
void fn_80214A9C(int s);
void fn_80214AF0(int s);
void fn_8027E908(void);
UIObjects* fn_80258354(void);
void fn_80286A34(u8* p);
void fn_80287238(u8* p);
u8* fn_80283938(u8* p, int del);
}

void operator delete(void* p);

extern void* lbl_80348678[];

// Copies at most n characters, always terminating.
static inline void CopyName(const char* s, int n, char* d) {
    while ((*d++ = *s++) != 0) {
        if (--n <= 0) {
            d[-1] = 0;
            break;
        }
    }
}

ModuleTag lbl_8055F670;
s32 lbl_8055F678;
u8 lbl_8055F67C;
UIObjects* lbl_8055F680;
bool lbl_8055F684;
u8 lbl_8055F685;
s32 lbl_8055F688;
s32 lbl_8055F68C;
s32 lbl_8055F690;
s32 lbl_8055F694;
u32 lbl_8055F698;
s32 lbl_8055F69C;

// The "cup unlocked" flash shown in the unlock message box (menu item type 5).
class EventUnlockFx : public MenuState {
public:
    EventUnlockFx(void) { start = 0.0f; }
    virtual void Enter(void);
    virtual void Update(void);
    virtual int Key(int key);
    f32 start;  // offset 0x18
    u8 done;    // offset 0x1C
};

static UnkStatic123s lbl_804B2FF8;            // this unit's end-of-menu tag
static EventUnlockFx lbl_804B3018;            // unlock flash widget
static FixedArray<u32, 5> lbl_804B3048;       // events of the current path
static char lbl_804B3068[100];                // name of the next event to unlock
static FixedArray<u8*, 5> lbl_804B30D8;       // the event browser's events
static char sBrowseTitle[32];                 // lbl_804B30D8 + 0x20
static char sBrowseDesc[32];                  // lbl_804B30D8 + 0x40
static char sBrowseBest[20];                  // lbl_804B30D8 + 0x60

extern "C" {

// Loads the screen's images, fonts and sounds.
void fn_8027E908(void) {
    lbl_8055F680 = fn_80258354();
    lbl_8055F680->AddImage("meter_unlockable", "ep_ticker.art", 4);
    lbl_8055F680->AddImage("meter_minor", "ep_ticker.art", 3);
    lbl_8055F680->AddImage("meter_major", "ep_ticker.art", 1);
    lbl_8055F680->AddImage("meter_tick", "ep_ticker.art", 2);
    lbl_8055F680->AddImage("meter_finger", "ep_ticker.art", 4);
    lbl_8055F680->AddImage(GetEventTypeName(0x180201), "event_icons.art", 1);
    lbl_8055F680->AddImage(GetEventTypeName(0x180302), "event_icons.art", 2);
    lbl_8055F680->AddImage("Time Attack", "event_icons.art", 1);
    lbl_8055F680->AddImage("bronze", "ep_cupicon1.art", 0);
    lbl_8055F680->AddImage("silver", "ep_cupicon1.art", 1);
    lbl_8055F680->AddImage("gold", "ep_cupicon1.art", 2);
    lbl_8055F680->AddImage("platinum", "ep_cupicon1.art", 3);
    lbl_8055F680->AddImage("diamond", "ep_cupicon1.art", 4);
    lbl_8055F680->AddImage("cup_star", "ep_cupicon1.art", 5);
    lbl_8055F680->AddImage("star_small_empty", "star_ab.art", 1);
    lbl_8055F680->AddImage("star_small_full", "star_ab.art", 0);
    lbl_8055F680->AddImage("silver_unlock", "cup_unlk1.art", 0);
    lbl_8055F680->AddImage("gold_unlock", "cup_unlk1.art", 1);
    lbl_8055F680->AddImage("platinum_unlock", "cup_unlk2.art", 0);
    lbl_8055F680->AddImage("diamond_unlock", "cup_unlk2.art", 1);
    lbl_8055F680->AddImage("nocup_unlock", "bar5.art", 6);
    lbl_8055F680->AddFont("normal", "uifnt.gtx");
    lbl_8055F680->AddFont("outline", "uifnt_o.gtx");
    lbl_8055F680->AddSound("scoreup_tally", "a_tally.sfx", 1);
    lbl_8055F680->AddSound("scoreup_gradeup", "a_grade1.sfx", 0);
    lbl_8055F680->AddSound("scoreup_AAA", "a_grade2.sfx", 0);
    lbl_8055F680->AddSound("scoreup_passevent", "a_starfill.sfx", 0);
    lbl_8055F680->AddSound("scoreup_perfectevent", "a_starfill.sfx", 0);
    lbl_8055F680->AddSound("passed_cup", "a_cheerhit.sfx", 0);
    lbl_8055F680->AddSound("passed_career", "a_grade1.sfx", 0);
    lbl_8055F680->AddSound("unlocked_career", "a_levelup.sfx", 0);
}

void fn_8027ED2C(void) {
    delete lbl_8055F680;
    lbl_8055F680 = 0;
}

UIObjects* fn_8027ED70(void) { return lbl_8055F680; }

}  // extern "C"

extern char lbl_8055B860[8];

extern "C" {

// Shows the "event unlocked" message box.
void fn_8027ED78(const char* title, const char* message) {
    MenuItem items[] = {
        MenuItem("#title", title),
        MenuItem("#message", message),
        MenuItem(lbl_8055B860, 6, 0, -2, 13),
        MenuItem(lbl_804B2FF8),
    };
    MenuDesc desc = {"ep_unlock.lyt", 0, 0, 0, 0};
    desc.items = items;
    fn_802146BC(&desc, -999, -999, 1);
}

void fn_8027EE74(int x) {
    lbl_8055F684 = x == 2;
    fn_80236914();
}

int fn_8027EE88(void) {
    if (!lbl_8055F685 && lbl_8055F684) {
        lbl_8055F685 = 1;
        fn_8027ED70()->PlaySound("unlocked_career");
    }
    return 0;
}

}  // extern "C"

extern char lbl_8055BAB8[4];  // ""
extern "C" const char* fn_80131234(const char* key);
extern "C" int fn_801345D8(char* buf, int n, const char* fmt, ...);

extern "C" {

// "Career difficulty N unlocked" box; the layout depends on the new difficulty.
void fn_8027EEE4(int level) {
    char buf[100];
    fn_801345D8(buf, 100, "EventPath:CAREER_DIFFICULTY_%d_UNLOCKED", level);
    MenuItem items[] = {
        MenuItem(0, 5, &lbl_804B3018, 0, 0),
        MenuItem("title", fn_80131234("EventPath:CONGRATULATIONS")),
        MenuItem("message", fn_80131234(buf)),
        MenuItem(lbl_8055B860, 6, 0, -2, 13),
        MenuItem(lbl_8055BAB8, 8, (const void*)fn_8027EE74, 2, 0),
        MenuItem(lbl_804B2FF8),
    };
    const char* layouts[3] = {"newmode1.lyt", "newmode2.lyt", "newmode3.lyt"};
    MenuDesc desc = {layouts[level - 1], 0, (int (*)(int*))fn_8027EE88, 0, 0};
    desc.items = items;
    lbl_8055F684 = 0;
    lbl_8055F685 = 0;
    fn_802146BC(&desc, -999, -999, 1);
}

}  // extern "C"

extern char lbl_80347E50[];  // this unit's string pool

extern "C" {

// "Career difficulty N unlocked" message box.
void fn_8027F09C(int difficulty) {
    char buf[100];
    fn_801345D8(buf, 100, lbl_80347E50 + 0x3D8, difficulty);  // "EventPath:CAREER_DIFFICULTY_%d_UNLOCK_INSTRUCTIONS"
    MenuItem items[] = {
        MenuItem(0, 5, &lbl_804B3018, 0, 0),
        MenuItem("title", fn_80131234(lbl_80347E50 + 0x40C)),  // "EventPath:CHALLENGE"
        MenuItem("message", fn_80131234(buf)),
        MenuItem(lbl_8055B860, 6, 0, -2, 13),
        MenuItem(lbl_804B2FF8),
    };
    MenuDesc desc = {"unlock_crr_chal.lyt", 0, 0, 0, 0};
    desc.items = items;
    fn_8027ED70()->PlaySound(lbl_80347E50 + 0x214);  // "passed_career"
    fn_802146BC(&desc, -999, -999, 1);
}

void fn_8027F204(const char* name) {
    u8* l = (u8*)lbl_8055F69C;
    if (!l) {
        return;
    }
    if (S32(l, 0x44) + 1 <= S32(l, 0x48)) {
        CopyName(name, 30, (char*)(PTR(l, 0x40) + S32(l, 0x44)++ * 30));
        return;
    }
    fn_802E30C8("add_anim_to_wait_for: can't add, there are %d already!", S32(l, 0x44));
}

void fn_8027F288(u8* p) {}  // stub (363)

extern void* lbl_80348798[];

u8* fn_8027F834(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_80348798;
        if ((u32)lbl_8055F688 == (u32)p) {
            lbl_8055F688 = 0;
        }
        lbl_804B3048.Remove(p);
        if (p) {
            U8(p, 0x18) = 0;
            U8(p, 0x50) = 0;
            U8(p, 0x5C) = 0;
            S32(p, 0x24) = 1;
            S32(p, 0x20) = 0;
            U8(p, 0x44) = 0;
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

void fn_8027F958(u8* p) {}  // stub (976)

int fn_80280898(void) {
    if (lbl_8055F688 == 0) {
        fn_80216D28();
        return 0;
    }
    return 1;
}

}  // extern "C"

int IsUnlockAll(void);

extern "C" {
void fn_80236954(const char* name);
void fn_80214D50(int s);

// Moves the cursor to the current event's slot; returns whether it is selectable.
int fn_802808D0(void) {
    int i = lbl_804B3048.Find((u8*)lbl_8055F688);
    if (i >= 0) {
        int ok;
        u8* e = (u8*)lbl_804B3048.data[i];
        ok = 0;
        if (PTR(e, 0x60) && (IsUnlockAll() || ((VR*)PTR(e, 0x60))->v24())) {
            ok = 1;
        }
        if (ok) {
            int idx = U8((u8*)lbl_8055F688, 0x64) ? 5 : lbl_804B3048.Find((u8*)lbl_8055F688);
            char buf[0x1E];
            fn_801345D8(buf, 0x1E, "select_%d", idx + 1);
            fn_80236954(buf);
            return 1;
        }
        fn_80216D28();
        fn_80214D50(1);
        return 0;
    }
    fn_80216D28();
    return 0;
}

}  // extern "C"

// An event's unlock condition (object at event + 0x60).
class EvCond {
public:
    virtual void v08(void);
    virtual const char* Title(void);  // 0x0C
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(void);
    virtual const char* Desc(void);   // 0x1C
    virtual s32 EventId(void);        // 0x20
    virtual int Difficulty(void);     // 0x24
    virtual void v28(void);
    virtual int Done(void);          // 0x2C
    virtual const char* Name(void);  // 0x30
};


extern "C" {

// When the current event is done, picks the first not-yet-done event as the next unlock.
void fn_80280A38(void) {
    u8* cur = (u8*)lbl_8055F688;
    if (cur && PTR(cur, 0x60) && ((EvCond*)PTR(cur, 0x60))->Done()) {
        lbl_8055F690 = lbl_8055F688;
        if (!lbl_8055F68C) {
            for (int i = 0; i < lbl_804B3048.count; i++) {
                u8* e = (u8*)lbl_804B3048.data[i];
                if (PTR(e, 0x60) && !((EvCond*)PTR(e, 0x60))->Done()) {
                    lbl_8055F68C = (s32)e;
                    break;
                }
            }
        }
        if (lbl_8055F68C) {
            int i = lbl_804B3048.Find((u8*)lbl_8055F68C);
            lbl_804B3068[0] = 0;
            if (i >= 0 && i < lbl_804B3048.count) {
                u8* e = (u8*)lbl_804B3048.data[i];
                lbl_8055F688 = (s32)e;
                if (e && PTR(e, 0x60)) {
                    CopyName(((EvCond*)PTR(e, 0x60))->Name(), 100, lbl_804B3068);
                }
            }
        }
    }
}

}  // extern "C"

// Makes event i current (none when out of range) and copies its caption.
static inline void SelectEvent(int i) {
    lbl_804B3068[0] = 0;
    if (i >= 0 && i < lbl_804B3048.count) {
        u8* e = (u8*)lbl_804B3048.data[i];
        lbl_8055F688 = (s32)e;
        if (e && PTR(e, 0x60)) {
            CopyName(((EvCond*)PTR(e, 0x60))->Name(), 100, lbl_804B3068);
        }
    } else {
        lbl_8055F688 = 0;
    }
}

extern "C" {
void fn_80216D28(void);

// While the current event is not done, picks the first done event as the one to show.
int fn_80280C00(void) {
    u8* cur = (u8*)lbl_8055F688;
    if (cur && PTR(cur, 0x60) && !((EvCond*)PTR(cur, 0x60))->Done()) {
        lbl_8055F68C = lbl_8055F688;
        if (!lbl_8055F690) {
            for (int i = 0; i < lbl_804B3048.count; i++) {
                u8* e = (u8*)lbl_804B3048.data[i];
                if (PTR(e, 0x60) && ((EvCond*)PTR(e, 0x60))->Done()) {
                    lbl_8055F690 = (s32)e;
                    break;
                }
            }
        }
        if (lbl_8055F690) {
            int i = lbl_804B3048.Find((u8*)lbl_8055F690);
            lbl_804B3068[0] = 0;
            if (i >= 0 && i < lbl_804B3048.count) {
                u8* e = (u8*)lbl_804B3048.data[i];
                lbl_8055F688 = (s32)e;
                if (e && PTR(e, 0x60)) {
                    CopyName(((EvCond*)PTR(e, 0x60))->Name(), 100, lbl_804B3068);
                }
            } else {
                lbl_8055F688 = 0;
            }
        }
    }
    return 0;
}

// Next event in the same state (done/not done) as the current one.
int fn_80280DC8(void) {
    if (lbl_8055F688) {
        int i = lbl_804B3048.Find((u8*)lbl_8055F688);
        u8* cur = (u8*)lbl_804B3048.data[i];
        if (PTR(cur, 0x60)) {
            bool done = ((EvCond*)PTR(cur, 0x60))->Done();
            int k;
            for (k = i + 1; k < lbl_804B3048.count; k++) {
                u8* e = (u8*)lbl_804B3048.data[k];
                if (PTR(e, 0x60) && ((EvCond*)PTR(e, 0x60))->Done() == done) {
                    break;
                }
            }
            if (k < lbl_804B3048.count) {
                SelectEvent(k);
            }
        }
    } else {
        fn_80216D28();
    }
    return 0;
}

// Previous event in the same state as the current one.
int fn_80280F7C(void) {
    if (lbl_8055F688) {
        int i = lbl_804B3048.Find((u8*)lbl_8055F688);
        u8* cur = (u8*)lbl_804B3048.data[i];
        if (PTR(cur, 0x60)) {
            bool done = ((EvCond*)PTR(cur, 0x60))->Done();
            int k;
            for (k = i - 1; k >= 0; k--) {
                u8* e = (u8*)lbl_804B3048.data[k];
                if (PTR(e, 0x60) && ((EvCond*)PTR(e, 0x60))->Done() == done) {
                    break;
                }
            }
            if (k >= 0) {
                SelectEvent(k);
            }
        }
    } else {
        fn_80216D28();
    }
    return 0;
}

int fn_802368EC(void);

// Menu slot of an event: the bonus slot (5) or its place in the registry.
static inline int EventSlot(u8* e) { return U8(e, 0x64) ? 5 : lbl_804B3048.Find(e); }

// Plays the "back to slot" animation, then the unlock animation for the newly opened event.
void fn_80281138(void) {
    if (lbl_804B3048.Find((u8*)lbl_8055F688) >= 0 && fn_802368EC()) {
        char buf[0x1E];
        fn_801345D8(buf, 0x1E, "back_select_%d", EventSlot((u8*)lbl_8055F688) + 1);
        fn_80236954(buf);
    }
    u8* next = (u8*)lbl_8055F694;
    if (next) {
        lbl_8055F688 = 0;
        char buf[0x1E];
        fn_801345D8(buf, 0x1E, "unlock_%d", EventSlot(next) + 1);
        fn_80236954(buf);
    }
}

void fn_802812B4(u8* p) {}  // stub (2376)

u8* fn_802837D4(u8* p, int del) {
    if (p && del > 0) {
        operator delete(p);
    }
    return p;
}

extern void* lbl_80348738[];

u8* fn_80283814(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_80348738;
        if ((u32)lbl_8055F698 == (u32)p) {
            lbl_8055F698 = 0;
        }
        lbl_804B30D8.Remove(p);
        if (p) {
            U8(p, 0x18) = 0;
            U8(p, 0x50) = 0;
            U8(p, 0x5C) = 0;
            S32(p, 0x24) = 1;
            S32(p, 0x20) = 0;
            U8(p, 0x44) = 0;
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

u8* fn_80283938(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_80348678;
        *(void**)(p + 0x4) = (u8*)lbl_80348678 + 0x50;
        lbl_8055F69C = 0;
        if (PTR(p, 0x20)) {
            operator delete(PTR(p, 0x20));
        }
        u8* m = p + 0x40;
        if (m) {
            if (m) {
                U32(p, 0x40) = 0;
                U32(p, 0x44) = 0;
                U32(p, 0x48) = 0;
            }
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

int fn_802839D0(void) {
    if (lbl_8055F684 && lbl_8055F698) {
        s32 a = VR(PTR((u8*)lbl_8055F698, 0x60))->v24();
        fn_8025C204(VR(PTR((u8*)lbl_8055F698, 0x60))->v20(), a);
    } else {
        fn_80216D28();
    }
    return 0;
}

int fn_80283A50(void) {
    if (lbl_804B30D8.Find((u8*)lbl_8055F698) >= 0) {
        return 1;
    }
    fn_80216D28();
    return 0;
}

int fn_80283AC8(void) {
    int i = lbl_804B30D8.Find((u8*)lbl_8055F698);
    if (i >= 0 && PTR((u8*)lbl_804B30D8.data[i], 0x60)) {
        return 1;
    }
    fn_80216D28();
    return 0;
}

}  // extern "C"

class Statistic {
public:
    int HasValue(void);
    u8 pad[0x24];
};

class RealStatistic : public Statistic {
public:
    RealStatistic(int slot, u32 type, long event, int difficulty);
    f32 GetValue(void);
};

int IdentGetCurrentSlot(void);
extern s32 lbl_8055C7E0;

extern "C" {
void fn_80130980(char* buf, s32 size, int fields, f32 secs);  // format a time

// Selects the next event that has an unlock condition and shows its title, description and best time.
int fn_80283B58(void) {
    int cur = lbl_804B30D8.Find((u8*)lbl_8055F698);
    if (cur >= 0) {
        int i = cur;
        do {
            if (++i >= lbl_804B30D8.Count()) {
                i = 0;
            }
        } while (!PTR((u8*)lbl_804B30D8.data[i], 0x60) && i != cur);
        if (i >= 0 && i < lbl_804B30D8.Count() && PTR((u8*)lbl_804B30D8.data[i], 0x60)) {
            lbl_8055F698 = (u32)lbl_804B30D8.data[i];
        } else {
            lbl_8055F698 = 0;
        }
        if (i >= 0 && i < lbl_804B30D8.Count() && PTR((u8*)lbl_804B30D8.data[i], 0x60)) {
            u8* e = (u8*)lbl_804B30D8.data[i];
            if (e) {
                CopyName(((EvCond*)PTR(e, 0x60))->Title(), 32, sBrowseTitle);
                CopyName(((EvCond*)PTR(e, 0x60))->Desc(), 32, sBrowseDesc);
                int difficulty = ((EvCond*)PTR(e, 0x60))->Difficulty();
                s32 event = ((EvCond*)PTR(e, 0x60))->EventId();
                RealStatistic best(IdentGetCurrentSlot(), 0x20507, event, difficulty);
                if (best.HasValue()) {
                    fn_80130980(sBrowseBest, 20, 3, best.GetValue());
                } else {
                    CopyName(fn_80131234("EventPath:NO_BEST_TIME"), 20, sBrowseBest);
                }
                lbl_8055C7E0 = ((EvCond*)PTR(e, 0x60))->EventId();
            } else {
                sBrowseDesc[0] = 0;
                sBrowseTitle[0] = 0;
                lbl_8055C7E0 = -1;
            }
        } else {
            sBrowseDesc[0] = 0;
            sBrowseTitle[0] = 0;
            lbl_8055C7E0 = -1;
        }
    } else {
        fn_80216D28();
    }
    return 0;
}

// Selects the previous event that has an unlock condition and shows its title, description and best time.
int fn_80283E28(void) {
    int cur = lbl_804B30D8.Find((u8*)lbl_8055F698);
    if (cur >= 0) {
        int i = cur;
        do {
            if (--i < 0) {
                i += lbl_804B30D8.Count();
            }
        } while (!PTR((u8*)lbl_804B30D8.data[i], 0x60) && i != cur);
        if (i >= 0 && i < lbl_804B30D8.Count() && PTR((u8*)lbl_804B30D8.data[i], 0x60)) {
            lbl_8055F698 = (u32)lbl_804B30D8.data[i];
        } else {
            lbl_8055F698 = 0;
        }
        if (i >= 0 && i < lbl_804B30D8.Count() && PTR((u8*)lbl_804B30D8.data[i], 0x60)) {
            u8* e = (u8*)lbl_804B30D8.data[i];
            if (e) {
                CopyName(((EvCond*)PTR(e, 0x60))->Title(), 32, sBrowseTitle);
                CopyName(((EvCond*)PTR(e, 0x60))->Desc(), 32, sBrowseDesc);
                int difficulty = ((EvCond*)PTR(e, 0x60))->Difficulty();
                s32 event = ((EvCond*)PTR(e, 0x60))->EventId();
                RealStatistic best(IdentGetCurrentSlot(), 0x20507, event, difficulty);
                if (best.HasValue()) {
                    fn_80130980(sBrowseBest, 20, 3, best.GetValue());
                } else {
                    CopyName(fn_80131234("EventPath:NO_BEST_TIME"), 20, sBrowseBest);
                }
                lbl_8055C7E0 = ((EvCond*)PTR(e, 0x60))->EventId();
            } else {
                sBrowseDesc[0] = 0;
                sBrowseTitle[0] = 0;
                lbl_8055C7E0 = -1;
            }
        } else {
            sBrowseDesc[0] = 0;
            sBrowseTitle[0] = 0;
            lbl_8055C7E0 = -1;
        }
    } else {
        fn_80216D28();
    }
    return 0;
}

void fn_802840F4(u8* p) { fn_8027E908(); }

void fn_802840F8(void) { fn_8027ED2C(); }

void fn_802840FC(u8* p) {}  // stub (289)

}  // extern "C"

struct Unlockable {
    const char* type;  // offset 0x0
    const char* name;  // offset 0x4
    s32 points;        // offset 0x8
};

int GetUnlockableCount(void);
Unlockable* GetUnlockable(int i);

class VehicleList {
public:
    virtual ~VehicleList(void);                                    // 0x8
    virtual void Add(const char* name, int unlocked, int special);  // 0xC
    virtual void SetReason(int i, const char* reason);             // 0x10
    virtual int Count(void);                                       // 0x14
};

// Extra vehicles an event offers on top of the unlockables.
class ExtraVehicles {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual int Count(void);            // 0x14
    virtual const char* Name(int i);    // 0x18
    virtual void v1C(void);
    virtual void v20(void);
    virtual void v24(void);
    virtual const char* Reason(int i);  // 0x28
};

extern "C" {
int strcasecmp(const char* a, const char* b);
VehicleList* fn_8025007C(int n);
void fn_8024FBD8(VehicleList* list, u32 arg);

// Shows the cars unlocked between two point totals (plus the event's extra vehicles).
void fn_80284580(int lo, int hi, ExtraVehicles* extra, u32 arg) {
    VehicleList* list = fn_8025007C(GetUnlockableCount() + (extra ? extra->Count() : 0));
    if (extra) {
        for (int i = 0; i < extra->Count(); i++) {
            list->Add(extra->Name(i), 1, 0);
            list->SetReason(list->Count() - 1, extra->Reason(i));
        }
    }
    for (int i = 0; i < GetUnlockableCount(); i++) {
        Unlockable* u = GetUnlockable(i);
        if (!strcasecmp(u->type, "CAR") && lo < u->points && hi >= u->points) {
            list->Add(u->name, 1, 0);
        }
    }
    if (list->Count() > 0) {
        fn_8024FBD8(list, arg);
    }
    delete list;
}

extern char lbl_80348940[];  // "cup_perfected" (string pool)
s32 fn_80221EA8(void);
int fn_8023695C(const char* name);

// Event screen setup: progress text ("%d/%d" of the cup's parts) and the event's title.
void fn_80284760(u8* p) {
    if (!U8(p, 0x64)) {
        U8(p, 0x18) = 1;
        S32(p, 0x1C) = fn_80221EA8();
    }
    if (PTR(p, 0x60)) {
        S32(p, 0x20) = 0;
        U8(p, 0x50) = 1;
        s32 y;
        s32 x = S32(p, 0x4);
        y = S32(p, 0x8);
        S32(p, 0x48) = x;
        S32(p, 0x4C) = y;
        S32(p, 0x6C) = x;
        F32(p, 0x68) = 0.0f;
        if (PTR(p, 0x60)) {
            if (VR(PTR(p, 0x60))->v28() && !U8(p, 0xCC)) {
                fn_80236954(lbl_80348940);
                fn_8027F204(lbl_80348940);
            }
            int show = 0;
            S32(p, 0x70) = p == (u8*)lbl_8055F688 ? 6 : 7;
            if (VR(PTR(p, 0x60))->v24()) {
                if (!VR(PTR(p, 0x60))->v28() || fn_8023695C(lbl_80348940)) {
                    show = 1;
                }
            }
            U8(p, 0xCD) = show;
            if (show) {
                int i;
                int done;
                int n = VR(PTR(p, 0x60))->v18();
                done = 0;
                for (i = 0; i < n; i++) {
                    if (VR(VR(PTR(p, 0x60))->v1C(i))->v40()) {
                        done++;
                    }
                }
                fn_801345D8((char*)p + 0x74, 10, "%d/%d", done, n);
            } else {
                U8(p, 0x74) = 0;
            }
        }
        const char* title = fn_8027ED70()->GetString(VR(PTR(p, 0x60))->v10());
        if (title) {
            CopyName(title, 50, (char*)p + 0x7E);
        }
        S32(p, 0xB0) = fn_8027ED70()->GetInt(VR(PTR(p, 0x60))->v10());
    } else {
        S32(p, 0x20) = 1;
    }
}

void fn_802849E8(u8* p) {}  // stub (702)

void fn_802854E0(u8* p) {}  // stub (178)

void fn_802857A8(u8* p) {}  // stub (324)

void fn_80285CB8(u8* p) {}  // stub (667)

void fn_80286724(u8* p) { VR(PTR(p, 0x1C))->v0C(); }

void fn_80286738(u8* p) { VR(PTR(p, 0x1C))->v10(); }

void fn_8028674C(u8* p) { VR(PTR(p, 0x1C))->v14(); }

void fn_80286760(u8* p) { VR(PTR(p, 0x1C))->v18(); }

void fn_80286774(u8* p, int a) { VR(PTR(p, 0x1C))->v1C(a); }

void fn_80286788(u8* p) { VR(PTR(p, 0x1C))->v20(); }

void fn_8028679C(u8* p) { VR(PTR(p, 0x1C))->v24(); }

s32 fn_802867B0(const u8* p) {
    u8* o = PTR(p, 0x20);
    if (o) {
        s32 v = (s32)((f32)S32(o, 0x0) + F32(o, 0x10));
        s32 m = S32(o, 0x4);
        return v < m ? v : m;
    }
    return VR(PTR(p, 0x1C))->v28();
}

void fn_80286834(u8* p) { VR(p)->v30(VR(p)->v28()); }

void fn_80286884(u8* p, int a) { VR(PTR(p, 0x1C))->v30(a); }

void fn_80286898(u8* p) { VR(PTR(p, 0x1C))->v34(); }

void fn_802868AC(u8* p, const char* a) { VR(PTR(p, 0x1C))->v38(a); }

void fn_802868C0(u8* p) { VR(PTR(p, 0x1C))->v3C(); }

int fn_802868D4(u8* p) {
    int a = VR(p)->v3C();
    return a <= VR(p)->v28();
}

void fn_8028693C(u8* p) { VR(PTR(p, 0x1C))->v44(); }

int fn_80286950(u8* p) {
    int a = VR(p)->v4C();
    return a <= VR(p)->v28();
}

void fn_802869B8(u8* p) { VR(PTR(p, 0x1C))->v4C(); }

int fn_802869CC(u8* p, int key) {
    switch (key) {
    case 0xD: {
        u8* o = PTR(p, 0x20);
        if (o && !U8(p, 0x29)) {
            F32(o, 0x10) = (f32)(S32(o, 0x4) - S32(o, 0x0));
            return 1;
        }
        break;
    }
    }
    return 0;
}

void fn_80286A34(u8* p) {}  // stub (513)

void fn_80287238(u8* p) {
    u8* o = PTR(p, 0x20);
    if (o) {
        F32(o, 0x10) = 0.0f;
        U8(p, 0x29) = 0;
        U8(p, 0x2A) = 0;
        U8(p, 0x28) = VR(p)->v40();
        fn_8027ED70()->PlaySound("scoreup_tally");
        fn_80214A9C(0x1F);
        F32(p, 0x178) = 1.0f;
    } else {
        U8(p, 0x29) = 1;
        U8(p, 0x2A) = 0;
        F32(p, 0x178) = 0.0f;
    }
}

void fn_802872DC(u8* p) { VR(PTR(p, 0x4))->v0C(); }

void fn_802872F0(u8* p) { VR(PTR(p, 0x4))->v10(); }

void fn_80287304(u8* p) { VR(PTR(p, 0x4))->v14(); }

void fn_80287318(u8* p) { VR(PTR(p, 0x4))->v18(); }

u8* fn_8028732C(u8* p, s32 x) {
    u8* r = PTR(p, 0x8);
    if (r && x == S32(p, 0xC)) {
        return r;
    }
    return VR(PTR(p, 0x4))->v1C(x);
}

void fn_80287360(u8* p) {}

int fn_80287364(u8* p) {
    for (int i = 0; i < VR(p)->v18(); i++) {
        if (!VR(VR(p)->v1C(i))->v40()) {
            return 0;
        }
    }
    return 1;
}

void fn_802873F8(u8* p) { VR(PTR(p, 0x4))->v24(); }

int fn_8028740C(u8* p) {
    for (int i = 0; i < VR(p)->v18(); i++) {
        if (!VR(VR(p)->v1C(i))->v48()) {
            return 0;
        }
    }
    return 1;
}

void fn_802874A0(u8* p) { VR(PTR(p, 0x4))->v2C(); }

void fn_802874B4(u8* p, int a) { VR(PTR(p, 0x4))->v30(a); }


}  // extern "C"

void EventUnlockFx::Enter(void) {
    start = fn_8023659C();
    done = 0;
    fn_80214A9C(0x1D);
}

void EventUnlockFx::Update(void) {
    f32 t = fn_8023659C();
    if (t < start || t - start > 1.5f) {
        done = 1;
    }
    if (done) {
        fn_80214AF0(0x1D);
    }
}

int EventUnlockFx::Key(int key) { return done == 0; }

extern "C" {

int fn_8028775C(u8* p, int k) { return fn_802869CC(p - 4, k); }

void fn_80287764(u8* p) { fn_80286A34(p - 4); }

void fn_8028776C(u8* p) { fn_80287238(p - 4); }

u8* fn_80287774(u8* p, int del) { return fn_80283938(p - 4, del); }

}  // extern "C"
