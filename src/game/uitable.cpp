// Menu data helpers (0x8022D228-0x8022E0EC): a fixed string list, the UI table text buffer (rows of
// fixed-width columns, "UITB"), cycling name/value lists, a list of 16-character names and the option
// values the menus edit (a float slider shown with "%1.*f" and an integer value), all with clamped
// stepping and 0..1 fractions for sliders. Names are guesses apart from the allocation tags.
#include "game/common.h"

extern "C" {
void* memset(void* p, int c, u32 n);
char* strcpy(char* d, const char* s);
char* strncpy(char* d, const char* s, u32 n);
int sprintf(char* s, const char* fmt, ...);
extern f32 lbl_80558DC4[];
}

void* operator new[](size_t size, const char* tag);

class StringList {
public:
    char* Get(int i, int* flag);

    s32 count;     // offset 0x0
    s32 len;       // offset 0x4
    s32 used;      // offset 0x8
    s32 added;     // offset 0xC
    char** items;  // offset 0x10
};

class UITable {
public:
    UITable(int rows, int cols, const int* widths);
    ~UITable(void);
    void Clear(void);
    char* At(int row, int col);
    char* Get(int row, int col, int* flag);
    void Set(int row, int col, const char* s, char flag);
    void SetVisible(int n);
    void SetPage(int p) {
        page = p;
        changes++;
    }
    void SetSel(int s) {
        sel = s;
        SetPage(s / visible);
        changes++;
    }

    char* buf;     // offset 0x0
    s32 cols;      // offset 0x4
    s32 used;      // offset 0x8
    s32 rows;      // offset 0xC
    s32 visible;   // offset 0x10
    s32 rowSize;   // offset 0x14
    s32* widths;   // offset 0x18
    s32* offsets;  // offset 0x1C
    s32 page;      // offset 0x20
    s32 changes;   // offset 0x24
    u32 magic;     // offset 0x28
    s32 sel;       // offset 0x2C
};

struct TableEntry {
    u8 pad[0x28];
};

struct EntryList {
    TableEntry* entries;  // offset 0x0
};

class OptionValue {
public:
    virtual ~OptionValue(void) {}
    virtual void GetPtr(void** p, int* size) = 0;
    virtual void Format(char* buf) = 0;
    virtual void Inc(void) = 0;
    virtual void Dec(void) = 0;
    virtual int CanInc(void) = 0;
    virtual int CanDec(void) = 0;
    virtual f32 GetFrac(void) = 0;
    virtual void SetFrac(f32 f) = 0;
};

class FloatValue : public OptionValue {
public:
    FloatValue(f32* v, f32 lo, f32 hi, f32 step, f32 min, f32 max);
    virtual ~FloatValue(void) {}
    virtual void GetPtr(void** p, int* size);
    virtual void Format(char* buf);
    virtual void Inc(void);
    virtual void Dec(void);
    virtual int CanInc(void);
    virtual int CanDec(void);
    virtual f32 GetFrac(void);
    virtual void SetFrac(f32 f);

    f32 lo;        // offset 0x4
    f32 hi;        // offset 0x8
    f32 min;       // offset 0xC
    f32 max;       // offset 0x10
    f32 step;      // offset 0x14
    f32* value;    // offset 0x18
    s32 decimals;  // offset 0x1C
};

class IntValue : public OptionValue {
public:
    virtual ~IntValue(void);
    virtual void GetPtr(void** p, int* size);
    virtual void Format(char* buf);
    virtual void Inc(void);
    virtual void Dec(void);
    virtual int CanInc(void);
    virtual int CanDec(void);
    virtual f32 GetFrac(void);
    virtual void SetFrac(f32 f);

    s32* value;       // offset 0x4
    s32 min;          // offset 0x8
    s32 max;          // offset 0xC
    const char* fmt;  // offset 0x10
};

class CycleList {
public:
    CycleList(const char** names, const s32* values, int count, int a);
    ~CycleList(void);
    const char* Name(void);
    s32 Value(void);
    void Next(void);
    void Prev(void);
    void Set(int i);
    const char* NameAt(int i);

    const char** names;  // offset 0x0
    const s32* values;   // offset 0x4
    s32 first;           // offset 0x8
    s32 cur;             // offset 0xC
    s32 count;           // offset 0x10
    s32 unk14;           // offset 0x14
    u8 unk18;            // offset 0x18
};

class NameList {
public:
    NameList(const char** src, int n, int a);
    ~NameList(void);
    int Count(void);
    int Unk44(void);
    char* Name(int i);

    char names[4][0x10];  // offset 0x0
    s32 count;            // offset 0x40
    s32 unk44;            // offset 0x44
};

static inline f32 Snap(f32 v, f32 s) { return s * (f32)(int)(0.5f + v / s); }
static inline int NearEq(f32 a, f32 b) { return (f32)__fabs(a - b) <= lbl_80558DC4[0]; }
static inline bool NearEq2(f32 a, f32 b) { return (f32)__fabs(a - b) <= lbl_80558DC4[0]; }
static inline int MinI(int a, int b) { return (b < a) ? b : a; }
static inline int MaxI(int a, int b) { return (b > a) ? b : a; }
static inline int MinI2(int a, int b) { return (a < b) ? a : b; }
static inline int MaxI2(int a, int b) { return (a > b) ? a : b; }
static inline int ClampI(int v, int lo, int hi) { return MaxI(lo, MinI(hi, v)); }

ModuleTag lbl_8055EFD8;
UnkStatic123 lbl_80497DF8;

extern "C" StringList* fn_8022D228(StringList* sl, int n, int l) {
    sl->count = n;
    sl->len = l;
    sl->used = 0;
    sl->items = new ("StringList") char*[n];
    for (int i = 0; i < n; i++) {
        sl->items[i] = new ("StringListItem") char[l + 1];
    }
    return sl;
}

void operator delete(void* p);

extern "C" StringList* fn_8022D2BC(StringList* sl, int flags) {
    if (sl) {
        for (int i = 0; i < sl->count; i++) {
            delete[] sl->items[i];
        }
        delete[] sl->items;
        if (flags > 0) {
            operator delete(sl);
        }
    }
    return sl;
}

extern "C" void fn_8022D350(StringList* sl, const char* s, char flag) {
    if (sl->used < sl->count) {
        strncpy(sl->items[sl->used], s, sl->len);
        sl->items[sl->used][sl->len - 1] = 0;
        sl->items[sl->used][sl->len] = flag;
        sl->used++;
        sl->added++;
    }
}

char* StringList::Get(int i, int* flag) {
    if (i >= 0 && i < used) {
        if (flag) {
            *flag = items[i][len];
        }
        return items[i];
    }
    if (flag) {
        *flag = 0;
    }
    return 0;
}

extern "C" void fn_802E30C8(const char* fmt, ...);  // debug print

// Not in the shipped game (stripped by the linker); kept for its string.
extern "C" void StringListCheck(StringList* l, int i) {
    if (i >= l->used) {
        fn_802E30C8("Entry %d does not exist", i);
    }
}

UITable::UITable(int r, int c, const int* w) {
    rows = r;
    used = 0;
    cols = c;
    widths = new ("column_wids") s32[c];
    offsets = new ("column_offs") s32[c];
    visible = r;
    sel = -1;
    changes = 0;
    magic = 'UITB';
    rowSize = 0;
    for (int i = 0; i < c; i++) {
        widths[i] = w[i];
        offsets[i] = rowSize;
        rowSize += widths[i] + 1;
    }
    buf = new ("UITable:buf") char[r * rowSize];
    Clear();
}

void UITable::Clear(void) {
    memset(buf, 0, rows * rowSize);
    used = 0;
    page = 0;
    changes++;
}

UITable::~UITable(void) {
    delete[] widths;
    delete[] offsets;
    delete[] buf;
}

char* UITable::At(int row, int col) { return buf + row * rowSize + offsets[col]; }

char* UITable::Get(int row, int col, int* flag) {
    char* p = At(row, col);
    if (flag) {
        *flag = *p;
    }
    return p + 1;
}

void UITable::Set(int row, int col, const char* s, char flag) {
    char* p = At(row, col);
    if (row + 1 > used) {
        used = row + 1;
    }
    int w = widths[col];
    *p = flag;
    strncpy(p + 1, s, w);
    p[w] = 0;
    changes++;
}

void UITable::SetVisible(int n) {
    visible = n;
    changes++;
    if (sel >= 0) {
        SetSel(sel);
    }
}

extern "C" TableEntry* fn_8022D97C(EntryList* l, int i) { return &l->entries[i]; }

FloatValue::FloatValue(f32* v, f32 l, f32 h, f32 s, f32 mn, f32 mx) {
    value = v;
    lo = l;
    hi = h;
    min = mn;
    max = mx;
    decimals = 0;
    f32 x = s;
    while (x - (f32)(int)x > 0.001f && decimals < 4) {
        x *= 10.0f;
        decimals++;
    }
    step = s / (h - l);
}

void FloatValue::Format(char* buf) { sprintf(buf, "%1.*f", decimals, 0.0004f + (lo + *value * (hi - lo))); }

f32 FloatValue::GetFrac(void) { return (*value - min) / (max - min); }

void FloatValue::SetFrac(f32 f) {
    f32 s = step;
    *value = s * (f32)(int)(0.5f + (min + f * (max - min)) / s);
}

void IntValue::Format(char* buf) { sprintf(buf, fmt, *value); }

CycleList::CycleList(const char** n, const s32* v, int c, int a) {
    names = n;
    values = v;
    count = c;
    first = c;
    while (first < 5) {
        first += count;
    }
    unk14 = a;
    cur = 0;
    unk18 = 0;
}

CycleList::~CycleList(void) {}

const char* CycleList::Name(void) { return names[cur]; }

s32 CycleList::Value(void) { return values[cur]; }

void CycleList::Next(void) {
    if (++cur >= count) {
        cur = 0;
    }
}

void CycleList::Prev(void) {
    if (--cur < 0) {
        cur = count - 1;
    }
}

void CycleList::Set(int i) {
    cur = i;
    while (cur >= count) {
        cur -= count;
    }
}

const char* CycleList::NameAt(int i) {
    int old = cur;
    Set(i);
    const char* r = Name();
    Set(old);
    return r;
}

NameList::NameList(const char** src, int n, int a) {
    for (int i = 0; i < n; i++) {
        strcpy(names[i], src[i]);
    }
    count = n;
    unk44 = a;
}

NameList::~NameList(void) { count = 0; }

int NameList::Count(void) { return count; }

int NameList::Unk44(void) { return unk44; }

char* NameList::Name(int i) { return names[i]; }

void FloatValue::GetPtr(void** p, int* size) {
    *p = value;
    *size = 4;
}

void FloatValue::Inc(void) {
    f32 v = step * (f32)(int)(0.5f + (*value + step) / step);
    *value = (max < v) ? max : v;
}

void FloatValue::Dec(void) {
    f32 v = step * (f32)(int)(0.5f + (*value - step) / step);
    *value = (min > v) ? min : v;
}

int FloatValue::CanInc(void) {
    if (NearEq(max, *value)) {
        return 0;
    }
    return 1;
}

int FloatValue::CanDec(void) {
    if (NearEq(min, *value)) {
        return 0;
    }
    return 1;
}

void IntValue::GetPtr(void** p, int* size) {
    *p = value;
    *size = 4;
}

void IntValue::Inc(void) { *value = ClampI(*value + 1, min, max); }

void IntValue::Dec(void) { *value = ClampI(*value - 1, min, max); }

int IntValue::CanInc(void) { return max != *value; }

int IntValue::CanDec(void) { return min != *value; }

f32 IntValue::GetFrac(void) { return (f32)(*value - min) / (f32)(max - min); }

void IntValue::SetFrac(f32 f) { *value = min + (int)(f * (f32)(max - min)); }

IntValue::~IntValue(void) {}
