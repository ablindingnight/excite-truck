// Code 0x8020F3B4-0x802114F4, imported verbatim from the upstream project (github.com/orangedude27/excite-truck,
// CC0; file src/auto/text_8020F3B4_.c), where its functions match exactly. Unit name is provisional.
// CLEANUP: automated-decompiler source with raw offsets and placeholder types; rewrite with real types
// before the PC port. Tricks are tagged MATCHING-AID.
  
#pragma cplusplus on
// Fixed list with its storage inline: two instances, each its own class (their weak dtors are distinct).
struct Unk_804900AC {
    Unk_804900AC(void) {
        m_entries = m_storage;
        m_capacity = 0x80;
        m_count = 0;
    }
    ~Unk_804900AC(void) {
        if (this != 0) {
            m_entries = 0;
            m_count = 0;
            m_capacity = 0;
        }
    }
    long *m_entries;
    long m_count;
    long m_capacity;
    long m_storage[0x80];
};
struct Unk_804902C4 {
    Unk_804902C4(void) {
        m_entries = m_storage;
        m_capacity = 0x80;
        m_count = 0;
    }
    ~Unk_804902C4(void) {
        if (this != 0) {
            m_entries = 0;
            m_count = 0;
            m_capacity = 0;
        }
    }
    long *m_entries;
    long m_count;
    long m_capacity;
    long m_storage[0x80];
};
#pragma cplusplus off
extern struct Unk_804900AC lbl_804900AC;
extern void fn_801345D8(void *, ...);
extern long lbl_8055B664;
extern unsigned char lbl_8055EEF4;
extern unsigned char lbl_8055EEF5;
extern unsigned char lbl_8055EEF6;
extern char lbl_80489CA0[0x6400];
extern long fn_8020F608(long, long);
extern struct Unk_804902C4 lbl_804902C4;
extern long lbl_8055EEC4;
extern long lbl_8055EED0;
extern long lbl_8055EED4;
extern long lbl_8055B658;
extern long __AXFXGetLfoSinTable_801769CC();
extern long lbl_8055B674;
extern long lbl_8033C970[4];
extern void fn_802EC1C0(float vol);
extern long fn_802EC0FC(void);
extern void fn_802EC1E4();
extern long fn_802EC12C();
long fn_8020F3B4(long slot) { return -1; }
 
long fn_8020F3BC(long arg0, long arg1, long arg2) {
    long result = arg1;
    fn_801345D8((void *)arg1, arg2, &lbl_8055B664, arg0);
    return result;
}
 
extern char lbl_8055B66C;
long fn_8020F400(long arg0, long arg1, long arg2) {
    long result = arg1;
    fn_801345D8((void *)arg1, arg2, &lbl_8055B66C, arg0);
    return result;
}

extern char lbl_804904D0[0x40];
extern long lbl_8055B650;
extern long lbl_8055B654;
extern char lbl_8033C95C[];
extern int fn_8012E938(const char *name);
extern void fn_8012E81C(const char *name);
extern void fn_802EC294(int slot, const char *name);
extern void fn_802E30C8(const char *, ...);

void fn_8020F444(void) {
    char buf[0x40];
    if (!lbl_8055EEF4 && fn_8012E938((const char *)fn_8020F400((long)lbl_804904D0, (long)buf, sizeof(buf)))) {
        fn_8012E81C((const char *)fn_8020F400((long)lbl_804904D0, (long)buf, sizeof(buf)));
        fn_802EC294(0, (const char *)fn_8020F3BC((long)lbl_804904D0, (long)buf, sizeof(buf)));
        fn_802E30C8(lbl_8033C95C, buf);
        lbl_8055EEF4 = 1;
    }
    if (!lbl_8055EEF5 && fn_8012E938((const char *)fn_8020F400(lbl_8055B650, (long)buf, sizeof(buf)))) {
        fn_8012E81C((const char *)fn_8020F400(lbl_8055B650, (long)buf, sizeof(buf)));
        fn_802EC294(1, (const char *)fn_8020F3BC(lbl_8055B650, (long)buf, sizeof(buf)));
        fn_802E30C8(lbl_8033C95C, buf);
        lbl_8055EEF5 = 1;
    }
    if (!lbl_8055EEF6 && fn_8012E938((const char *)fn_8020F400(lbl_8055B654, (long)buf, sizeof(buf)))) {
        fn_8012E81C((const char *)fn_8020F400(lbl_8055B654, (long)buf, sizeof(buf)));
        fn_802EC294(2, (const char *)fn_8020F3BC(lbl_8055B654, (long)buf, sizeof(buf)));
        fn_802E30C8(lbl_8033C95C, buf);
        lbl_8055EEF6 = 1;
    }
}

void fn_8020F5C8(void) {
    lbl_8055EEF6 = 1;
    lbl_8055EEF5 = 1;
    lbl_8055EEF4 = 1;
}
#pragma dont_inline reset
 
void fn_8020F5DC(void) {
    lbl_8055EEF6 = 0;
    lbl_8055EEF5 = 0;
    lbl_8055EEF4 = 0;
}
 
void *fn_8020F5F0(long arg0) {
    long offset = (arg0 << 6) * 100;
    return (void *)((char *)&lbl_80489CA0 + offset);
}

extern void *memset(void *, int, unsigned long);
long fn_8020F608(long arg0, long arg1) {
    long i;
    char *ptr;

    ptr = (char *)fn_8020F5F0(arg1);
    memset(ptr, 0, 0x1900);
    for (i = 0; i < 64; i++) {
        *(long *)(ptr + i * 100) = arg0;
    }
}
#pragma dont_inline reset
 
void fn_8020F6E0(long arg0, long arg1) {
    if (arg1 != 0) {
        return;
    }
    fn_8020F608(0, arg0);
}
 
long fn_8020F6F8(void) {
    return fn_802EC0FC();
}
 
long fn_8020F6FC(void) {
    long index = lbl_8055EEC4;
    long *entries = lbl_804902C4.m_entries;
    long count = lbl_804902C4.m_count;
    long value = entries[index];
    lbl_8055EEC4 = (index + 1) % count;
    return value;
}

extern long fn_80129D18(long seed, long range);
void fn_8020F730(void) {
    long count = lbl_804900AC.m_count;
    long i;
    lbl_804902C4.m_count = 0;
    if (count != 0) {
        for (i = 0; i < count; i++) {
            lbl_804902C4.m_entries[lbl_804902C4.m_count++] = i;
        }
        for (i = 0; i < lbl_804902C4.m_count; i++) {
            long index = fn_80129D18(6, lbl_804902C4.m_count);
            long *e = lbl_804902C4.m_entries;
            long temp = e[i];
            e[i] = e[index];
            e[index] = temp;
        }
    }
    lbl_8055EEC4 = 0;
}

extern long fn_802EC104(long index);
void fn_8020F950(void) {
    long cap = lbl_804900AC.m_capacity;
    long n;
    long i;
    lbl_804900AC.m_count = 0;
    n = fn_802EC0FC();
    if (cap < n) {
        n = cap;
    }
    for (i = 0; i < n; i++) {
        lbl_804900AC.m_entries[lbl_804900AC.m_count++] = fn_802EC104(i);
    }
    fn_8020F730();
}

#pragma cplusplus on
long IdentGetCurrentSlot(void);
#pragma cplusplus off
extern int strcasecmp(const char *, const char *);
extern int strncasecmp(const char *, const char *, unsigned long);
long fn_8020F9F4(void) {
    char *e = (char *)fn_8020F5F0(IdentGetCurrentSlot());
    long i;
    for (i = 0; i < 64; e += 100, i++) {
        if (e[4] == 0) {
            return i;
        }
    }
    return 0;
}

void *fn_8020FAF0(const char *name) {
    char *base = (char *)fn_8020F5F0(IdentGetCurrentSlot());
    char *e = base;
    long i;
    for (i = 0; i < 64; e += 100, i++) {
        if (strcasecmp(e + 4, name) == 0) {
            return base + i * 100;
        }
    }
    return base + fn_8020F9F4() * 100;
}

long fn_8020FB80(const char *name) {
    long i;
    for (i = 0; i < lbl_804900AC.m_count; i++) {
        if (strncasecmp(((char **)lbl_804900AC.m_entries)[i], name, 0x3F) == 0) {
            return i;
        }
    }
    return fn_8020F3B4(0);
}

long fn_8020FC04(const char *name) {
    long *p = (long *)fn_8020FAF0(name);
    if (p != 0) {
        if (*p == 0) {
            return 0;
        }
        if (*p == 1) {
            return lbl_804900AC.m_entries[fn_8020F6FC()];
        }
        return (long)(p + 9);
    }
    return 0;
}

long fn_8020FC6C(const char *name) {
    struct Unk_804900AC *state = &lbl_804900AC;
    const char *found;
    if (state->m_count == 0) {
        return fn_8020F3B4(0);
    } else if ((found = (const char *)fn_8020FC04(name)) != 0) {
        return fn_8020FB80(found);
    } else {
        return fn_8020F3B4(0);
    }
}

extern long lbl_8055EEBC;
extern long lbl_8055EEE0;
extern long lbl_8055EEE4;
extern long lbl_8055EEE8;
extern void *fn_8001285C(long);
extern long fn_802EC1D8(void);
extern void fn_802E30C8(const char *, ...);
extern char lbl_8033C980[];
#pragma push
#pragma cplusplus on
// The current event's info object (event + 0x20); slot 0x18 looks up a string by key.
class EventInfo {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual const char *GetString(long key, long fallback);
};

extern "C" void fn_8020FCC4(void) {
    EventInfo *object = *(EventInfo **)((long *)fn_8001285C(*(long *)__AXFXGetLfoSinTable_801769CC()) + 8);
    const char *name = object->GetString((long)&lbl_8055B674, (long)lbl_8033C970);
    long idx = fn_8020FC6C(name);
    if (idx >= 0 && idx < lbl_804900AC.m_count) {
        fn_802E30C8(lbl_8033C980, idx, ((char **)lbl_804900AC.m_entries)[idx], name);
    }
    if (idx == fn_8020F3B4(0) && lbl_8055B658 == fn_8020F3B4(1)) {
        idx = fn_8020F3B4(1);
    }
    if (idx == fn_8020F3B4(0) && lbl_8055B658 == fn_8020F3B4(2)) {
        idx = fn_8020F3B4(2);
    }
    if (lbl_8055B658 != -1 && lbl_8055B658 != idx) {
        fn_802EC1D8();
    }
    lbl_8055B658 = idx;
}
#pragma cplusplus reset
#pragma pop

extern char *strncpy(char *, const char *, unsigned long);
void fn_8020FDE4(long arg0, const char *label, const char *name) {
    int left;
    long *e = (long *)fn_8020FAF0(name);
    char *dest;
    memset(e, 0, 4);
    e[0] = arg0;
    strncpy((char *)e + 0x24, label, 0x3F);
    dest = (char *)e + 4;
    left = 0x20;
    while ((*dest++ = *name++) != 0) {
        if (--left <= 0) {
            dest[-1] = 0;
            break;
        }
    }
}

void fn_8020FE90(void) {
    fn_802EC12C();
    fn_802EC1D8();
    if (lbl_8055EEE0 == -2) {
        lbl_8055B658 = fn_8020F3B4(0);
    } else if (lbl_8055EEE0 == -1) {
        lbl_8055B658 = fn_8020F6FC();
    } else {
        lbl_8055B658 = lbl_8055EEE0 + lbl_8055EEE4 * 10;
    }
}
void fn_8020FEF0(long dir) {
    long sel = lbl_8055EEE0;
    long page;
    long max;
    if (sel == -2 && lbl_8055EEBC > 0) {
        if (dir == 1) {
            lbl_8055EEE0 = -1;
            fn_8020FE90();
        }
        return;
    }
    if (sel == -1) {
        if (dir == -1) {
            lbl_8055EEE0 = -2;
            fn_8020FE90();
        }
        return;
    }
    {
        long old = lbl_8055EEE4;
        max = lbl_8055EEE8 - 1;
        page = old + dir;
        page = page > 0 ? page : 0;
        if (page < max) {
            max = page;
        }
        lbl_8055EEE4 = max;
        if (old == max) {
            return;
        }
    }
    if (lbl_8055EEBC == 0) {
        lbl_8055EEE0 = -2;
    } else if (sel >= 0 && sel + max * 10 >= lbl_8055EEBC) {
        lbl_8055EEE0 = (lbl_8055EEBC - 1) % 10;
    }
    if (lbl_8055EEE0 >= 0) {
        fn_8020FE90();
    }
}

void fn_8020FFE4(long arg0) {
    long old = lbl_8055EEE0;
    long value = lbl_8055EEBC;
    long digit;
    if (value == 0) lbl_8055EEE0 = -2;
    else {
        digit = 9;
        if ((lbl_8055EEE4 + 1) * 10 >= value) digit = (value - 1) % 10;
        if (old == -2) { if (arg0 == 1) lbl_8055EEE0 = 0; }
        else if (old == -1) { if (arg0 == 1) lbl_8055EEE0 = 0; }
        else {
            lbl_8055EEE0 += arg0;
            if (lbl_8055EEE0 < 0) lbl_8055EEE0 = -2;
            if (lbl_8055EEE0 > digit) lbl_8055EEE0 = digit;
        }
    }
    if (old != lbl_8055EEE0) fn_8020FE90();
}

extern unsigned char lbl_8055EEEC;
extern unsigned char lbl_8055EEED;
#pragma push
#pragma cplusplus on
extern "C" void fn_802100AC(void) {
    EventInfo *ev;
    fn_802EC12C();
    if (lbl_8055EEE0 == -2) {
        ev = *(EventInfo **)((long *)fn_8001285C(*(long *)__AXFXGetLfoSinTable_801769CC()) + 8);
        fn_8020FDE4(0, "", ev->GetString((long)&lbl_8055B674, (long)lbl_8033C970));
        if (lbl_8055EEEC) {
            fn_8020F608(0, IdentGetCurrentSlot());
        }
    } else if (lbl_8055EEE0 == -1) {
        ev = *(EventInfo **)((long *)fn_8001285C(*(long *)__AXFXGetLfoSinTable_801769CC()) + 8);
        fn_8020FDE4(1, "", ev->GetString((long)&lbl_8055B674, (long)lbl_8033C970));
        if (lbl_8055EEED) {
            fn_8020F608(1, IdentGetCurrentSlot());
        }
    } else {
        long idx = lbl_8055EEE0 + lbl_8055EEE4 * 10;
        ev = *(EventInfo **)((long *)fn_8001285C(*(long *)__AXFXGetLfoSinTable_801769CC()) + 8);
        fn_8020FDE4(2, ((char **)lbl_804900AC.m_entries)[idx], ev->GetString((long)&lbl_8055B674, (long)lbl_8033C970));
        lbl_8055B658 = idx;
    }
}
#pragma cplusplus reset
#pragma pop

extern float fn_80235EF0(long, long);
extern void fn_80147940(void *, void *, long, long);
typedef struct PanelPos {
    float x;
    float y;
    float z;
} PanelPos;

typedef struct PanelBox {
    long color;     /* 0x00 */
    PanelPos pos;   /* 0x04 */
    float w;        /* 0x10 */
    float h;        /* 0x14 */
    long unk18;     /* 0x18 */
} PanelBox;

#pragma push
#pragma cplusplus on
// A UI widget; slot 0x18 returns its font.
// A text box from the layout (0x20 bytes); color at 0x18.
typedef struct TextBox {
    long unk00;
    PanelPos pos;   /* 0x04 */
    float size;     /* 0x10 */
    long unk14;
    long color;     /* 0x18 */
    long unk1C;
} TextBox;

class PanelWidget {
public:
    virtual void vf08(void);
    virtual TextBox *FindText(const char *name);
    virtual void vf10(void);
    virtual PanelBox *FindPane(const char *name);
    virtual long GetFont(void);
};

extern "C" void fn_802101F8(PanelWidget *arg0, long arg1, long arg2, void *arg3) {
    PanelBox *src = (PanelBox *)arg3;
    PanelBox b;
    long font;
    long i;

    if (arg0 == 0 || src == 0) return;
    {
        font = arg0->GetFont();
        i = 0;
        b.color = src->color;
        b.pos = src->pos;
        b.w = src->w;
        b.h = src->h;
        b.unk18 = src->unk18;
        do {
            long idx = arg2 + i;
            if (i == 1) {
                float right = fn_80235EF0(font, arg2 + 2);
                b.w = (src->w - fn_80235EF0(font, arg2)) - right;
            } else {
                b.w = fn_80235EF0(font, idx);
            }
            fn_80147940(arg0, &b, arg1, idx);
            i++;
            b.pos.x += b.w;
        } while (i < 3);
    }
}
#pragma cplusplus reset
#pragma pop

 
void fn_8021031C(void) {
    long type;
    char *buffer;
    type = 0x546D7573;
    buffer = lbl_80489CA0;
    memset(&lbl_80489CA0, 0, 0x6400);
    fn_802E6BD8(type, 0, buffer, 0x6400, 0);
    fn_8000DBC8(&fn_8020F6E0);
}
 
extern long fn_80207848(void *, long);
extern long fn_802EC1D8(void);
extern long fn_802EC12C(void);
extern char lbl_8033C9AC[0xD];
extern long lbl_8055EED0;
void fn_8021037C(void) {
    lbl_8055EED0 = fn_80207848(lbl_8033C9AC, 1);
    fn_802EC1D8();
    fn_802EC12C();
    fn_8020FCC4();
}
 
void fn_802103B8(void) {
    fn_8020F5C8();
}

void fn_802103BC(void) {
    struct VTable { void *pad[2]; void (*call)(void *, long); };
    struct Object { struct VTable *vtable; };
    fn_802EC1C0(0.0f);
    fn_802EC12C();
    fn_802EC1E4();
    if (lbl_8055EED4 != 0) {
        ((struct Object *)lbl_8055EED4)->vtable->call((struct Object *)lbl_8055EED4, 1);
    }
    lbl_8055EED4 = 0;
    lbl_8055B658 = -1;
    fn_8020F5DC();
    if (lbl_8055EED0 != 0) {
        ((struct Object *)lbl_8055EED0)->vtable->call((struct Object *)lbl_8055EED0, 1);
    }
    lbl_8055EED0 = 0;
}
#pragma dont_inline reset
extern long fn_80009C84(void);
extern int fn_801B85CC(void);
extern unsigned char lbl_8055EEC0;
extern unsigned char lbl_8055EEC8, lbl_8055B660;
extern long lbl_8055EECC, lbl_8055B658;
void fn_80210444(void) {
    lbl_8055B660 = 1;
    lbl_8055EEC0 = 0;
    lbl_8055EEC8 = 0;
    lbl_8055EECC = 0;
    if (fn_80009C84() == 0 && fn_801B85CC() == 0 &&
        lbl_8055B658 == fn_8020F3B4(1)) {
        lbl_8055B658 = fn_8020F3B4(0);
        fn_802EC12C();
        fn_802EC1D8();
    }
}

long fn_802104B8(void) {
    return lbl_8055B658 != -1;
}

long fn_802104D0(void) {
    return fn_802EC0FC() > 0;
}

long fn_802104FC(void) {
    return (lbl_8055B658 + 1) == 0;
}

 
void fn_80210510(float volume) {
    fn_802EC1C0(volume);
}

void fn_80210514(long arg0) {
    switch (arg0) {
    case 0:
        fn_8020F608(0, IdentGetCurrentSlot());
        break;
    case 1:
        fn_8020F608(1, IdentGetCurrentSlot());
        break;
    case 2:  /* an empty label: its jump to the end is the target's dead second b (which value is a guess) */
        break;
    }
}

long fn_80210570(void) {
    long mode = 3;
    long *e = (long *)fn_8020F5F0(IdentGetCurrentSlot());
    long i;
    for (i = 0; i < 64; e = (long *)((char *)e + 100), i++) {
        if (mode == 3) {
            mode = e[0];
        } else if (e[0] != mode) {
            return 2;
        }
    }
    switch (mode) {
    case 0:
        return 0;
    case 1:
        return 1;
    case 2:
        return 2;
    }
    return 0;
}
 
extern long fn_80145E70(void *);
extern long fn_802090D4(void *);
extern long fn_80209110(void *);
#pragma cplusplus on
struct Dummy_80210904 {
      virtual void f1(long);
  };
#pragma cplusplus off
extern long fn_8000DA10(void);
extern long lbl_8033C9BC[];
extern long lbl_8033C9C8[];
extern long lbl_8055EED4;
extern long lbl_8055EEF0;
extern long lbl_8055B658;
extern long IsLoadingActive__Fv(void);
extern long fn_802E2C20(void);
extern long fn_802EC124(void);
extern void fn_802EC11C(long);
extern void fn_802EC138(void);
extern void fn_802EC158(void);
extern long lbl_8055EECC;

#pragma push
#pragma cplusplus on
extern "C" void fn_80210688(unsigned char arg0) {
    long count;
    long loading;
    long busy;
    long now;
    long inWindow;
    EventInfo *object;

    if (!lbl_8055EEF4 || !lbl_8055EEF5 || !lbl_8055EEF6) {
        fn_8020F444();
    }
    count = fn_802EC0FC();
    if (count != lbl_8055EEBC) {
        fn_802EC12C();
        fn_8020F950();
        fn_8020FCC4();
        lbl_8055EEC0 = 1;
    } else {
        lbl_8055EEC0 = 0;
    }
    lbl_8055EEBC = count;

    loading = 0;
    if (IsLoadingActive__Fv() && !fn_80009C84()) {
        loading = 1;
    }
    busy = 1;
    if (!fn_801B85CC() && !fn_80009C84()) {
        busy = 0;
    }

    if (lbl_8055B658 == -1) {
        return;
    }
    if (lbl_8055B658 == fn_8020F3B4(0) && loading) {
        lbl_8055B658 = fn_8020F3B4(2);
        fn_802EC12C();
        fn_802EC1D8();
    } else if (lbl_8055B658 == fn_8020F3B4(2) && !loading) {
        lbl_8055B658 = fn_8020F3B4(0);
        fn_802EC12C();
        fn_802EC1D8();
    } else if (lbl_8055B658 == fn_8020F3B4(0) && busy) {
        lbl_8055B658 = fn_8020F3B4(1);
        fn_802EC12C();
        fn_802EC1D8();
    }

    inWindow = 0;
    if (lbl_8055EEC8) {
        now = fn_802E2C20();
        if (now >= lbl_8055EECC && now < lbl_8055EECC + 4000) {
            inWindow = 1;
        }
    }
    if (fn_802EC124() == -1 && !inWindow) {
        fn_802EC11C(lbl_8055B658);
        fn_802EC138();
        object = *(EventInfo **)((long *)fn_8001285C(*(long *)__AXFXGetLfoSinTable_801769CC()) + 8);
        if (*(long *)fn_8020FAF0(object->GetString((long)&lbl_8055B674, (long)lbl_8033C970)) == 1 &&
            lbl_8055EEBC > 0) {
            lbl_8055B658 = fn_8020F6FC();
        }
    }

    if (lbl_8055B660 != arg0) {
        if (arg0) {
            fn_802EC158();
        } else {
            fn_802EC138();
        }
    }
    lbl_8055B660 = arg0;
}
#pragma cplusplus reset
#pragma pop

void fn_802108C8(void) {
    lbl_8055EED4 = fn_80145E70(&lbl_8033C9BC[0]);
    lbl_8055EEF0 = fn_802090D4(&lbl_8033C9C8[0]);
}
 
#pragma cplusplus on
extern "C" void fn_80210904(void) {
    fn_80209110(&lbl_8055EEF0);
    struct Dummy_80210904 *obj = (struct Dummy_80210904 *)lbl_8055EED4;
    if (obj != 0) {
        obj->f1(1);
    }
    lbl_8055EED4 = 0;
}
#pragma cplusplus off
extern void *lbl_8055EED8;
extern void *lbl_8055EEDC;
void fn_80210950(void *arg0, void *arg1) {
    lbl_8055EED8 = arg0;
    lbl_8055EEDC = arg1;
}

typedef struct CourseVec2 {
    float x;
    float y;
} CourseVec2;

// The menu layout object, read fresh from its global at every use.
#define COURSE_LAYOUT ((PanelWidget *)lbl_8055EED4)

extern char lbl_8033C940[];
extern char lbl_8055B680[6], lbl_8055B688[5], lbl_8055B690[6], lbl_8055B698[3], lbl_8055B69C[4], lbl_8055B6A0[8];

#pragma push
#pragma cplusplus on
extern "C" void fn_80230B5C(void *);
extern "C" void fn_802C8638(void *);
extern "C" void fn_802C974C(float, float, float);
extern "C" void fn_800109E4(unsigned long, float, float, float, float);
extern "C" void fn_80011A2C(void);
extern "C" void fn_802C9904(void);
extern "C" float fn_80230EF4(int);
extern "C" float fn_800108C8(const char *, float);
extern "C" void fn_80147230(void *, const char *, ...);
extern "C" void fn_801473F8(PanelWidget *, const char *, const char *, ...);
extern "C" void fn_801476C4(void *);
extern "C" void fn_8014784C(PanelWidget *, const char *, const char *, int);
extern "C" void *memset(void *, int, unsigned long);
extern "C" char *strncpy(char *, const char *, unsigned long);
extern "C" unsigned long strlen(const char *);

// Draws one of the option buttons below the list, highlighted when it is the selected one.
static inline void CourseMenu_DrawOption(const char *pool, const char *text, const char *pane, int sel,
                                         long hilite) {
    TextBox *t = COURSE_LAYOUT->FindText(text);
    if (t != 0) {
        long style = 6;
        TextBox box;
        if (sel) {
            style = 3;
        }
        fn_802101F8(COURSE_LAYOUT, (long)(pool + 0x94), style, COURSE_LAYOUT->FindPane(pane));
        box = *t;
        if (sel) {
            box.color = hilite;
        }
        fn_801476C4(&box);
    }
}

// Draws the custom-course menu: a page of 10 names with their numbers, the cursor, the option buttons,
// the page counter and the page arrows.
extern "C" void fn_8021095C(void) {
    const char *pool = lbl_8033C940;
    char camera[0x30];

    if (lbl_8055EEDC != 0) {
        ((void (*)(void))lbl_8055EEDC)();
    }
    fn_80230B5C(camera);
    fn_802C8638(camera);
    fn_802C974C(1.0f, 1000.0f, 1.4398966f);
    fn_800109E4(0x7F000000, (-428.0f), 241.0f, 855.0f, 482.0f);
    fn_80011A2C();
    if (lbl_8055EED4 != 0) {
        CourseVec2 col;
        CourseVec2 step;
        CourseVec2 cursor;
        long hilite;
        TextBox *name;
        TextBox *num;
        TextBox *next;
        PanelBox *mark;
        long base;
        long i;

        fn_802101F8(COURSE_LAYOUT, (long)(pool + 0x94), 0, COURSE_LAYOUT->FindPane(pool + 0xA0));
        hilite = ((long)(255.0f * fn_80230EF4(1)) << 24) | 0xFFFF00;
        col.x = 0.0f;
        col.y = 0.0f;
        step.x = 0.0f;
        step.y = 0.0f;
        cursor.x = 0.0f;
        cursor.y = 0.0f;
        name = COURSE_LAYOUT->FindText(lbl_8055B680);
        num = COURSE_LAYOUT->FindText(lbl_8055B688);
        next = COURSE_LAYOUT->FindText(lbl_8055B690);
        if (name != 0 && num != 0 && next != 0) {
            col.x = num->pos.x - name->pos.x;
            step.x = next->pos.x - name->pos.x;
            float y2 = num->pos.y;
            col.y = y2 - name->pos.y;
            step.y = next->pos.y - name->pos.y;
        }
        mark = COURSE_LAYOUT->FindPane(pool + 0xAC);
        if (mark != 0 && name != 0) {
            cursor.x = mark->pos.x - name->pos.x;
            cursor.y = mark->pos.y - name->pos.y;
        }
        base = lbl_8055EEE4 * 10;
        for (i = base; i < lbl_804900AC.m_count && i < base + 10; i++) {
            long row = i - base;
            TextBox nameBox;
            TextBox numBox;
            char buf[0xFF];
            nameBox = *name;
            nameBox.pos.x = nameBox.pos.x + step.x * (float)row;
            float yv = step.y;
            nameBox.pos.y = nameBox.pos.y + yv * (float)row;
            numBox = *num;
            numBox.pos.x = nameBox.pos.x + col.x;
            numBox.pos.y = nameBox.pos.y + col.y;
            if (row == lbl_8055EEE0) {
                PanelBox *cur = COURSE_LAYOUT->FindPane(pool + 0xAC);
                if (cur != 0) {
                    PanelBox b = *cur;
                    float y_ = cursor.y;
                    float xv = cursor.x;
                    b.pos.x = nameBox.pos.x + xv;
                    b.pos.y = nameBox.pos.y + y_;
                    fn_802101F8(COURSE_LAYOUT, (long)(pool + 0x94), 9, &b);
                }
                nameBox.color = hilite;
                numBox.color = hilite;
            }
            memset(buf, 0, sizeof(buf));
            strncpy(buf, ((char **)lbl_804900AC.m_entries)[i], sizeof(buf) - 1);
            while (strlen(buf) > 4 && fn_800108C8(buf, nameBox.size) > 385.0f) {
                buf[strlen(buf) - 1] = 0;
            }
            fn_80147230(&nameBox, lbl_8055B698, buf);
            fn_80147230(&numBox, lbl_8055B69C, i + 1);
        }
        CourseMenu_DrawOption(pool, pool + 0xB8, pool + 0xC8, lbl_8055EEE0 == -2 && lbl_8055EEEC == 0, hilite);
        CourseMenu_DrawOption(pool, pool + 0xD4, pool + 0xE8, lbl_8055EEE0 == -2 && lbl_8055EEEC == 1, hilite);
        if (fn_8020F6F8() > 0) {
            CourseMenu_DrawOption(pool, pool + 0xF8, pool + 0x104, lbl_8055EEE0 == -1 && lbl_8055EEED == 0, hilite);
            CourseMenu_DrawOption(pool, pool + 0x110, pool + 0x120, lbl_8055EEE0 == -1 && lbl_8055EEED == 1, hilite);
        }
        fn_801476C4(COURSE_LAYOUT->FindText(pool + 0x130));
        fn_801473F8(COURSE_LAYOUT, pool + 0x144, lbl_8055B6A0, lbl_8055EEE4 + 1, lbl_8055EEE8);
        fn_801476C4(COURSE_LAYOUT->FindText(pool + 0x150));
        if (lbl_8055EEE4 < lbl_8055EEE8 - 1 && lbl_8055EEE0 >= 0) {
            fn_8014784C(COURSE_LAYOUT, pool + 0x15C, pool + 0x168, 1);
        }
        if (lbl_8055EEE4 > 0 && lbl_8055EEE0 >= 0) {
            fn_8014784C(COURSE_LAYOUT, pool + 0x178, pool + 0x184, 1);
        }
        fn_801476C4(COURSE_LAYOUT->FindText(pool + 0x194));
    }
    fn_80011A2C();
    fn_802C9904();
}
#pragma cplusplus reset
#pragma pop

extern void fn_8020FEF0(long);
extern void fn_802100AC(void);
extern long fn_80012158(void);
long fn_802111E4(long arg0) {
    switch (arg0) {
    case 0x126:
        fn_8020FFE4(-1);
        break;
    case 0x128:
        fn_8020FFE4(1);
        break;
    case 0x127:
        fn_8020FEF0(1);
        break;
    case 0x125:
        fn_8020FEF0(-1);
        break;
    case 0x53: {
        long (*hook)(long) = (long (*)(long))lbl_8055EED8;
        hook((arg0 - 0x58) == 0);
        fn_802100AC();
        break;
    }
    case 0x113:
    case 0x1B:
        break;
    default:
        return 0;
    }
    fn_80012158();
    return 1;
}

extern long fn_80265D64(void);
extern void __AXDSPInitCallback_801769E0(void);
extern void fn_80206444(void);
extern void fn_80209190(long, float);
extern long __AXFXGetLfoSinTable_801769CC(void);
extern void *fn_8001285C(long);
extern long lbl_8033C970[4];
extern long lbl_8055B65C;
extern long lbl_8055B674;
extern unsigned char lbl_8055EEEC;
extern unsigned char lbl_8055EEED;
#pragma push
#pragma cplusplus on
extern "C" void fn_802112AC(void) {
    long mode;
    long n;
    long quotient;
    long remainder;
    long count;
    EventInfo *object;

    if (lbl_8055EEC0) {
        lbl_8055EEE0 = -2;
        if (lbl_8055EEBC > 0) {
            fn_8020FCC4();
            object = *(EventInfo **)((long *)fn_8001285C(*(long *)__AXFXGetLfoSinTable_801769CC()) + 8);
            mode = *(long *)fn_8020FAF0(object->GetString((long)&lbl_8055B674, (long)lbl_8033C970));
            if (mode == 1) {
                lbl_8055EEE0 = -1;
            } else if (mode == 2) {
                n = lbl_8055B658;
                if (n >= 0) {
                    quotient = n / 10;
                    remainder = n % 10;
                    lbl_8055EEE4 = quotient;
                    lbl_8055EEE0 = remainder;
                }
            }
        }
    }
    count = lbl_8055EEBC / 10;
    lbl_8055EEE8 = count;
    if (lbl_8055EEBC % 10 != 0 || lbl_8055EEBC == 0) {
        count++;
        lbl_8055EEE8 = count;
    }
    if (lbl_8055EEE4 >= count) {
        lbl_8055EEE4 = 0;
    }
}
#pragma cplusplus reset
#pragma pop

#pragma push
#pragma cplusplus on
extern "C" void fn_802113D8(void) {
    long mode;
    long n;
    long q;
    long r;
    long zero = 0;
    long minusTwo = -2;
    EventInfo *object;

    if (fn_80265D64() == 0) {
        __AXDSPInitCallback_801769E0();
    }
    fn_80206444();
    fn_80209190(lbl_8055EEF0, 0.0f);
    lbl_8055EEE0 = minusTwo;
    lbl_8055EEE4 = zero;
    lbl_8055B65C = lbl_8055B658;
    lbl_8055EEEC = zero;
    lbl_8055EEED = zero;
    object = *(EventInfo **)((long *)fn_8001285C(*(long *)__AXFXGetLfoSinTable_801769CC()) + 8);
    mode = *(long *)fn_8020FAF0(object->GetString((long)&lbl_8055B674, (long)lbl_8033C970));
    if (mode == 1) {
        lbl_8055EEE0 = -1;
    } else if (mode == 2) {
        n = lbl_8055B658;
        if (n >= 0) {
            q = n / 10;
            r = n % 10;
            lbl_8055EEE4 = q;
            lbl_8055EEE0 = r;
        }
    }
}
#pragma cplusplus reset
#pragma pop

extern void fn_80206454(void);
extern void fn_801769EC(void);
void fn_802114C4(void) {
    fn_80209190(lbl_8055EEF0, 1.0f);
    fn_80206454();
    fn_801769EC();
}

extern void fn_801345D8(void *, ...);
extern long lbl_8055B664;
extern unsigned char lbl_8055EEF4;
extern unsigned char lbl_8055EEF5;
extern unsigned char lbl_8055EEF6;
// Code of main that had no source file yet: decomp.wtf gave it this one.
 // split_file divides it into the game's real files (file_status shows its range).

    extern long fn_802EC104(long);
extern void fn_8020F730(void);
extern long fn_8020F9F4(void);
extern int stricmp(const char *, const char *);
extern int fn_8002B1D8(const char *, const char *, int);

// The module's statics: tag, list terminator, marker, then the two lists. fn_802114F4 is the __sinit of
// this unit and the two weak list dtors follow it (they sat at the head of uiapi's split).
#pragma cplusplus on
class ModuleTag;
void RegisterModuleTag(ModuleTag* tag);
class ModuleTag {
public:
    ModuleTag(void) { RegisterModuleTag(this); }
    unsigned char flag;
    ModuleTag* next;
};
class UnkStatic123s {
public:
    UnkStatic123s(void) {
        unk4 = 123;
        unk0 = 0;
        unk8 = 0;
        unkC = 0;
        unk10 = 0;
    }
    unsigned long unk0, unk4, unk8, unkC, unk10, unk14;
};
class UnkStatic7F {
public:
    UnkStatic7F(void) { value = 0x7F000000; }
    unsigned long value;
};
ModuleTag lbl_8055EEB0;
UnkStatic123s lbl_80489C88;
UnkStatic7F lbl_8055EEB8;
char lbl_80489CA0[0x6400];
Unk_804900AC lbl_804900AC;
Unk_804902C4 lbl_804902C4;
char lbl_804904D0[0x40];
#pragma cplusplus off
