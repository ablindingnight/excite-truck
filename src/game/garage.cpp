// Garage screen (0x8024DAD8-0x802526E0): the car locker ("#locker", "ChassisList", "c_list"), colour
// labels ("UICar:COLOR%d"), trophy and car reveal cues ("trophy_snd", "a_trophy1.sfx", "reveal_car")
// and the chassis list entries. Written against raw field offsets; the large screen routines are still
// stubs. Names are guesses apart from the strings.
#include "game/common.h"
#include <string.h>

#define F32(p, o) (*(f32*)((u8*)(p) + (o)))
#define S32(p, o) (*(s32*)((u8*)(p) + (o)))
#define U32(p, o) (*(u32*)((u8*)(p) + (o)))
#define U8(p, o) (*(u8*)((u8*)(p) + (o)))
#define PTR(p, o) (*(u8**)((u8*)(p) + (o)))

struct VehicleInfo;
VehicleInfo* FindVehicleNamed(const char* name, int i);
u32* GetVehicleColor(VehicleInfo* v, int i);
const char* GetSpecialChassis(const char* name);

class CarUIScreen {
public:
    void UnloadModel(void);
};

class VDel {
public:
    virtual ~VDel(void);
};

class MenuState {
public:
    void Finish(int r);
};

// Garage-wide flags.
struct GarageFlags {
    u32 unk0;  // offset 0x0
    u8 alt;    // offset 0x4
    u8 pad5[0xB];
};

extern "C" {
void fn_8024739C(u8* p);
u32 fn_802E4A10(void);
int fn_80129800(u32 ch, int feature);
void fn_80216D28(void);
void fn_80214D50(int s);
void __ct__9LocStringFPCc(u8* p, const char* key);
void fn_80235380(u8* p);
void fn_802CAC54(u8* p);
void fn_802354A0(u8* o, int kind, s32 v, f32 x, f32 y, f32 z, f32 w, f32 h);
void fn_80216CB4(void);
void fn_80214A9C(int s);
}

void operator delete(void* p);
void operator delete[](void* p);
void* operator new(unsigned long n, const char* tag);
void* operator new[](unsigned long n, const char* tag);

extern GarageFlags lbl_80498C48;
extern u8* lbl_8055F2EC;

class VList {
public:
    virtual void v08(void);
    virtual void v0C(void);
    virtual void v10(void);
    virtual void v14(void);
    virtual void v18(void);
    virtual int v1C(u32 id);
};
extern void* lbl_8034453C[];
extern void* lbl_803444D8[];

// Shared car icon textures, released with the last list.
struct IconEntry {
    u8 pad0[0x20];  // offset 0x0
    u8* tex;        // offset 0x20
    u8* texE;       // offset 0x24
    u8 pad28[0x10]; // offset 0x28
};

struct IconPool {
    IconEntry* entries;  // offset 0x0
    s32 count;           // offset 0x4
};

extern IconPool* lbl_8055F2E4;

static inline void FreeIconPool(IconPool* pool) {
    if (pool) {
        for (int i = 0; i < pool->count; i++) {
            IconEntry* e = &pool->entries[i];
            fn_802CAC54(e->tex);
            fn_802CAC54(e->texE);
        }
        delete[] pool->entries;
        operator delete(pool);
    }
}
extern s32 lbl_8055F2E8;
extern void* lbl_803443B0[];
extern void* lbl_80344384[];
extern f32 lbl_80558DC4[];  // FLT_EPSILON

extern char lbl_8055C0E8[4];  // ""
extern u32 lbl_8055E044;
extern "C" void fn_80131318(u8* p);

static inline void CopyName(const char* s, int n, char* d) {
    while ((*d++ = *s++) != 0) {
        if (--n <= 0) {
            d[-1] = 0;
            break;
        }
    }
}

static inline void CopyNameTo(const char* s, int n, char* d) {
    char* dd;
    int nn;
    const char* ss;
    dd = d;
    ss = s;
    nn = n;
    while ((*dd++ = *ss++) != 0) {
        if (--nn <= 0) {
            dd[-1] = 0;
            break;
        }
    }
}

static inline void CopyNameA(const char* s, int n, char* d) {
    char* dd;
    int nn;
    const char* ss;
    nn = n;
    ss = s;
    dd = d;
    while ((*dd++ = *ss++) != 0) {
        if (--nn <= 0) {
            dd[-1] = 0;
            break;
        }
    }
}

static inline int NearZero(f32 x) { return (f32)__fabs(x) <= lbl_80558DC4[0]; }

extern "C" {

void fn_8024DAD8(u8* p) { fn_8024739C(p); }

void fn_8024DADC(u8* p) { fn_8024739C(p); }

void fn_8024DAE0(u8* p) {}  // stub (769)

void fn_8024E6E4(u8* p) {}  // stub (77)

u8* fn_8024E818(u8* p, int del) {
    if (p) {
        lbl_8055F2EC = 0;
        if (p) {
            *(void**)p = lbl_803444D8;
            delete[] PTR(p, 0xCC);
            if (--lbl_8055F2E8 == 0) {
                FreeIconPool(lbl_8055F2E4);
                lbl_8055F2E4 = 0;
            }
            if (p) {
                *(void**)p = lbl_8034453C;
                delete[] PTR(p, 0x18);
                fn_80235380(PTR(p, 0x24));
                fn_802CAC54(PTR(p, 0x20));
            }
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

u8* fn_8024E920(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_803444D8;
        delete[] PTR(p, 0xCC);
        if (--lbl_8055F2E8 == 0) {
            FreeIconPool(lbl_8055F2E4);
            lbl_8055F2E4 = 0;
        }
        if (p) {
            *(void**)p = lbl_8034453C;
            delete[] PTR(p, 0x18);
            fn_80235380(PTR(p, 0x24));
            fn_802CAC54(PTR(p, 0x20));
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

void fn_8024EA1C(u8* p) {}  // stub (94)

u32 fn_8024EB94(u8* p) { return U32(p, 0x1c); }

u8* fn_8024EB9C(u8* p, int i) { return PTR(p, 0xCC) + i * 0x14; }

void fn_8024EBAC(u8* p, s32 v, int set) {
    S32(p, 0x2C) = v;
    if (set) {
        F32(p, 0x28) = (f32)v;
    }
}

u32 fn_8024EBE4(u8* p) {
    VehicleInfo* v = FindVehicleNamed((const char*)(p + 0x60), 0);
    if (lbl_80498C48.alt) {
        return GetVehicleColor(v, 0)[0];
    }
    return GetVehicleColor(v, 0)[1];
}

int fn_8024EC3C(void) {
    int ok = 0;
    if (fn_80129800(fn_802E4A10(), 5) && fn_80129800(fn_802E4A10(), 6)) {
        ok = 1;
    }
    if (!ok) {
        if (((VList*)PTR(lbl_8055F2EC, 0xD0))->v1C(U32(lbl_8055F2EC, 0x2C))) {
            return 1;
        }
    } else {
        fn_80216D28();
    }
    fn_80214D50(1);
    return 0;
}

void fn_8024ECD4(u8* p) {}  // stub (298)

void fn_8024F17C(u8* p) {}  // stub (634)

u8* fn_8024FB64(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_8034453C;
        delete[] PTR(p, 0x18);
        fn_80235380(PTR(p, 0x24));
        fn_802CAC54(PTR(p, 0x20));
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

void fn_8024FBD8(u8* p) {}  // stub (264)

u8* fn_8024FFF8(u8* p, int del) {
    if (p) {
        *(void**)p = lbl_803443B0;
        VDel* c = (VDel*)PTR(p, 0x188);
        if (c) {
            delete c;
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

u8* fn_8025007C(int n) {
    u8* o = (u8*)operator new(0x20, "ChassisList");
    if (o) {
        *(void**)o = lbl_80344384;
        u8* buf = (u8*)operator new[](n * 0x48, "dsa_buf");
        PTR(o, 0x4) = buf;
        S32(o, 0xC) = n;
        U32(o, 0x8) = 0;
        PTR(o, 0x10) = buf;
        __ct__9LocStringFPCc(o + 0x14, "EventPath:CONGRATULATIONS");
    }
    return o;
}

void fn_80250108(u8* p) {}  // stub (63)

void fn_80250204(u8* p) {}  // stub (925)

void fn_80251078(u8* p) {}  // stub (271)

void fn_802514B4(u8* p, int i, f32 x, f32 y, f32 w, f32 h) {
    s32 v = S32(PTR(p, 0xCC) + i * 0x14, 0x10);
    fn_802354A0(PTR(p, 0x24), 0, -1, x, y, 0.0f, w, h);
    fn_80216CB4();
    fn_802354A0(PTR(p, 0x24), 3, v, x, y, 0.0f, w, h);
    fn_802354A0(PTR(p, 0x24), 6, -1, x, y, 0.0f, w, h);
}

void fn_8025157C(u8* p) {}  // stub (238)

void fn_80251934(u8* p) {}  // stub (116)

void fn_80251B04(u8* p) {}  // stub (130)

void fn_80251D0C(u8* p) {
    ((CarUIScreen*)PTR(p, 0x20))->UnloadModel();
    S32(p, 0x190) = -1;
    U8(p, 0x28) = 0;
    U8(p, 0x48) = 0;
    U8(p, 0x88) = 0;
    U8(p, 0x108) = 0;
    F32(p, 0x194) = 0.0f;
    F32(p, 0x198) = 20.0f;
    fn_80214A9C(0x1F);
    U8(p, 0x19C) = 0;
}

void fn_80251D7C(u8* p) {}  // stub (303)

int fn_80252238(u8* p, int key) {
    if (key == 0xD) {
        if (NearZero(F32(p, 0x194)) || NearZero(F32(p, 0x198))) {
            ((MenuState*)p)->Finish(0);
            return 1;
        }
    }
    return 0;
}

u8* fn_802522A8(u8* p, int del) {
    if (p) {
        if (p + 4) {
            delete[] PTR(p, 0x10);
            if (p + 4) {
                U32(p, 0x4) = 0;
                U32(p, 0x8) = 0;
                U32(p, 0xC) = 0;
            }
        }
        if (del > 0) {
            operator delete(p);
        }
    }
    return p;
}

void fn_80252320(u8* p, const char* name, u8 a, u8 b) {
    u8* e = PTR(p, 4) + S32(p, 8)++ * 0x48;
    CopyNameA(name, 0x20, (char*)e);
    U8(e, 0x20) = a;
    U8(e, 0x41) = b;
    if (b) {
        const char* sp = GetSpecialChassis(name);
        CopyName(sp, 0x20, (char*)e + 0x21);
    } else {
        CopyNameTo(lbl_8055C0E8, 0x20, (char*)e + 0x21);
    }
    if (U32(p, 0x1C) != lbl_8055E044) {
        fn_80131318(p + 0x14);
    }
    S32(e, 0x44) = S32(p, 0x18);
}

void fn_80252454(u8* p, int i, s32 v) { S32(PTR(p, 0x4) + i * 0x48, 0x44) = v; }

u32 fn_80252468(u8* p) { return U32(p, 0x8); }

u8* fn_80252470(u8* p, int i) { return PTR(p, 0x4) + i * 0x48; }

u8 fn_80252480(u8* p, int i) { return U8(PTR(p, 0x4) + i * 0x48, 0x20); }

u8* fn_80252494(u8* p, int i) { return PTR(p, 0x4) + i * 0x48 + 0x21; }

u8 fn_802524A8(u8* p, int i) { return U8(PTR(p, 0x4) + i * 0x48, 0x41); }

s32 fn_802524BC(u8* p, int i) { return S32(PTR(p, 0x4) + i * 0x48, 0x44); }

void fn_802524D0(u8* p) {}  // stub (132)
}  // extern "C"
