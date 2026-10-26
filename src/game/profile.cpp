// Isolated profile research: 26 owning functions; initialized-data ownership
// and full-source placement remain incomplete. Native component proofs are separate.
#include "game/common.h"

class MenuState {
public:
    MenuState(void) { unk14 = 0; }
    virtual ~MenuState(void) {}
    virtual void Enter(void);   // 0x0C
    virtual void Exit(void);    // 0x10
    virtual void Unk14(void);
    virtual void Unk18(void);
    virtual void Update(void);  // 0x1C
    virtual void Unk20(void);
    virtual void Unk24(void);
    virtual void Draw(void);    // 0x28
    virtual void Unk2C(void);
    virtual void Unk30(void);
    virtual void Unk34(void);
    virtual void Unk38(void);
    virtual void Unk3C(void);
    virtual void Unk40(void);
    virtual void Unk44(void);
    virtual void Unk48(void);
    virtual void Unk4C(void);
    virtual void Unk50(void);
    virtual void Unk54(void);
    virtual int Key(int key);   // 0x58
    virtual void Unk5C(void);

    s32 x;      // offset 0x4
    s32 y;      // offset 0x8
    s32 w;      // offset 0xC
    s32 h;      // offset 0x10
    int unk14;  // offset 0x14
};

extern "C" void fn_802328D4(int);
// Physical font payload and release sequence are observed at +0x34/+0x78.
// No held base-method implementation is reconstructed by these declarations.
class ProfileFontState {
public:
    ~ProfileFontState(void) { fn_802328D4(font); font=0; }
    int font; u32 color; int alignment; f32 scale,widthScale;inline void DrawCoordinatesNew(const void* text,f32 x,f32 y)const;
};
class ProfileTextState : public MenuState {
public:
    virtual ~ProfileTextState(void) {}
    virtual void Enter(void);
    virtual void Draw(void);
    virtual int Key(int key);
    u8 displayed[20]; // Physical span; original field semantics remain unknown.
    char* text; int cursor;
    ProfileFontState font;
    bool active;
};
class ProfileKeyboardState : public MenuState {
public:
    virtual ~ProfileKeyboardState(void);
    virtual void Enter(void);
    virtual void Draw(void);
    virtual int Key(int key);
    ProfileTextState textItem;
    int column,row;
    bool upper; char shiftLabel[10];
    ProfileFontState font;
};
typedef char CheckProfileFontSize[sizeof(ProfileFontState)==0x14?1:-1];
typedef char CheckProfileTextSize[sizeof(ProfileTextState)==0x4C?1:-1];
typedef char CheckProfileKeyboardSize[sizeof(ProfileKeyboardState)==0x8C?1:-1];


int IdentIsSlotEmpty(int slot);
int IdentGetCurrentSlot(void);
const char* IdentGetSlotName(int);
int IdentIsSlotUsed(int);
void IdentSelectSlot(int slot);
void IdentClearSlot(void);
const char* IdentGetDefaultName(int slot);
void IdentSetSlotName(const char* name);

u32 AchievementsGetForSlot(int, int);

extern "C" {
void fn_802E712C(void);
int fn_801345D8(char*, unsigned long, const char*, ...);
int fn_8023FAD4(char*, int);
int fn_8027B8E4(int, int);
int fn_8027B984(int, int);
void fn_80214A9C(int);
unsigned long strlen(const char*);
extern char lbl_80498A00[4][18];
extern int lbl_80498A48[4];
extern char* lbl_8055F2C4;
void fn_802E6CA8(void);
void fn_802E6FBC(void);
void fn_802E7128(void);
int fn_802E8D84(void);
const char* fn_80131234(const char* key);
int fn_80215214(const char* title, const char* message, int flags);
void fn_80214B44(int item);
void fn_80214CB4(int item);
int fn_80232B70(int font);
void fn_80230C30(f32*, f32*, int, int);
void fn_80230924(u32, f32, f32, f32, f32, f32);
void fn_80232BA4(const void*, int, f32, f32, u32, int, f32, f32, f32);
int sprintf(char*, const char*, ...);
f32 fn_8023293C(int font, const char* text, f32 widthScale);

// Forward declarations for bodies defined below.
void fn_8024C464(void);
int fn_8024C768(void);
int fn_8024CC54(int* slot, int (*confirm)(int), int create);

extern u8 lbl_8055F2D0[8];
// Physical view of original 8055F2C8..8055F2D0. Accessed bytes are at
// +0,+1,+2,+4; reserved gaps are observed address gaps, not inferred flags.
struct ProfileFlagSpan { u8 flagC8, flagC9, flagCA, reservedCB, flagCC, reservedCD[3]; };
extern ProfileFlagSpan profileFlags;
#define lbl_8055F2C8 profileFlags.flagC8
#define lbl_8055F2C9 profileFlags.flagC9
#define lbl_8055F2CA profileFlags.flagCA
#define lbl_8055F2CC profileFlags.flagCC

void fn_8024C370(void) {}

int fn_8024C524(void) {
    fn_802E712C();
    fn_802E6CA8();
    fn_8024C464();
    return 0;
}

int fn_8024C550(void) {
    fn_802E6FBC();
    fn_8024C464();
    return 0;
}

int fn_8024C6F4(int slot) {
    const char* message;
    int allowed = 0;
    const char* title;
    if (IdentIsSlotEmpty(slot)) {
        message=fn_80131234("Profile:CONFIRM_CREATE");
        title=fn_80131234("Profile:TITLE");
        if (!fn_80215214(title, message, 0)) goto finished;
    }
    allowed=1;
finished:
    return allowed;
}

void fn_8024CBF4(void) {
    u8* available = lbl_8055F2D0;
    for (int i = 0; i < 4; i++, available++) {
        if (!*available) {
            fn_80214B44(i + 1);
        }
    }
    fn_80214CB4(IdentGetCurrentSlot());
}

int fn_8024CEF0(int slot){
if(IdentIsSlotEmpty(slot))return 1;
const char* message="Do you really want overwrite this profile?";
const char* title="Create Profile";
return fn_80215214(title,message,0);
}

int fn_8024CF38(int* slot) { return fn_8024CC54(slot, fn_8024CEF0, 1); }
int fn_8024CF48(int* slot) { return fn_8024CC54(slot, 0, 0); }

int fn_8024CF54(void) {
    lbl_8055F2C8 = 1;
    return fn_8024C768();
}

void fn_8024CF60(void) {
    int slot;
    if (fn_8024CF38(&slot)) {
        IdentSelectSlot(slot);
        IdentClearSlot();
        IdentSetSlotName(IdentGetDefaultName(slot));
        if (fn_802E8D84()) {
            fn_802E7128();
        }
    }
}

void fn_8024CFB4(void) {
    int slot;
    if (fn_8024CF48(&slot)) {
        IdentSelectSlot(slot);
    }
}

struct ProfileTextFields {
    u8 pad0[0x10];
    s32 height;        // +0x10, direct lwz at 8024D004
    u8 pad14[0x20];
    s32 font;          // +0x34, direct lwz at 8024CFFC
    u8 pad38[8];
    f32 scale;         // +0x40, direct stfs at 8024D03C
};

}

void ProfileTextState::Enter(void) {
    ProfileTextFields* self=(ProfileTextFields*)this;
    int fontHeight = fn_80232B70(self->font);
    self->scale = (f32)self->height / (f32)fontHeight;
}

extern "C" {

// Slot label and selection-icon refresh, 8024C464-8024C524.
void fn_8024C464(void) {
    char* name = &lbl_80498A00[0][0];
    int* icon = lbl_80498A48;
    for (int slot = 0; slot < 4; slot++, name += 18, icon++) {
        fn_801345D8(name, 18, "%s", IdentGetSlotName(slot));
        if (!IdentIsSlotEmpty(slot) && slot == IdentGetCurrentSlot()) {
            if (IdentIsSlotUsed(slot)) *icon = 2;
            else *icon = 1;
        } else *icon = 0;
    }
}

int fn_8024C578(int slot) {
    if (IdentIsSlotEmpty(slot)) {
        char name[18] = {0};
        if (fn_8023FAD4(name, 18)) {
            IdentSelectSlot(slot);
            IdentClearSlot();
            IdentSetSlotName(name);
            fn_802E6FBC();
            fn_8024C464();
            return 1;
        }
        return 0;
    }
    lbl_8055F2C4 = lbl_80498A00[slot];
    IdentSelectSlot(slot);
    fn_8024C464();
    return 1;
}

void fn_8024C628(void) {
    int current = IdentGetCurrentSlot();
    
    for (int slot = 0; slot < 4; slot++) {
        unsigned int valid=1;
        for (int j = 0; j < 3; j++) if (!fn_8027B8E4(j, slot)) valid = 0;
        for (int j = 0; j < 2; j++) if (!fn_8027B984(j, slot)) valid = 0;
        if (valid) fn_80214A9C(slot + 1);
        else fn_80214A9C(slot + 11);
        if (slot == current) fn_80214CB4((slot << 1) + valid);
    }
}

// Only observed fields are named; this does not assert a recovered class hierarchy.
struct ProfileCursorFields {
    u8 pad0[0x2C];
    char* text;
    int cursor;
    u8 pad34[0x14];
    u8 dirty;
};
}

int ProfileTextState::Key(int key) {
    ProfileCursorFields* self=(ProfileCursorFields*)this;
    switch (key) {
    case 0x6F: {
        int length = strlen(self->text);
        int next = self->cursor + 1;
        if(length < next) next=length; self->cursor=next;
        self->dirty = 1;
        break;
    }
    case 0x20: {
        self->dirty = 1;
        int next = self->cursor - 1;
        self->cursor = next < 0 ? 0 : next;
        break;
    }
    default: return 0;
    }
    return 1;
}

extern "C" {


struct ProfileKeyboardFitFields {
    u8 pad0[0xC];
    int width;                    // +0x0C
    int height;                   // +0x10
    u8 pad14[0x58];
    bool upper;                     // +0x6C
    char shiftLabel[10];          // +0x6D
    u8 pad77;
    int font;                     // +0x78
    u8 pad7C[8];
    f32 scale;                    // +0x84
    f32 widthScale;               // +0x88
};
}

extern "C" {extern const f32 lbl_80563944,lbl_80563948,lbl_8056394C;}
extern "C" const f32 lbl_80563944;
extern "C" const f32 lbl_80563948;
extern "C" const f32 lbl_8056394C;
void ProfileKeyboardState::Enter(void) {
    ProfileKeyboardFitFields* self=(ProfileKeyboardFitFields*)this;
    f32 cellWidth;f32 width=(f32)self->width;f32 cellHeight=(f32)self->height*lbl_80563944;cellWidth=width/lbl_80563948;
    f32 textWidth = fn_8023293C(self->font, "BS", self->widthScale);
    textWidth=self->scale*textWidth;f32 fitWidth=(cellWidth-lbl_8056394C)/textWidth;
    int fontHeight=fn_80232B70(self->font);f32 heightGap=cellHeight-lbl_8056394C;f32 fitHeight=heightGap/(f32)fontHeight;
    self->scale = fitHeight < fitWidth ? fitHeight : fitWidth;
    self->upper=0;self->upper=!self->upper;
    const char* key = self->upper ? "UIKeyboard:SHIFT_UPPER" : "UIKeyboard:SHIFT_LOWER";
const char* src;int n;char* dst;src=fn_80131234(key);dst=self->shiftLabel;n=10;
    while ((*dst++ = *src++) != 0) {
        if (--n <= 0) { dst[-1] = 0; break; }
    }
}

extern "C" {


struct ProfileTextDrawFields {
    u8 pad0[4];
    int x, y, width, height;       // +0x04..+0x10
    u8 pad14[4];
    u8 stringObject[0x14];         // +0x18, opaque; draw takes object address
    char* text;                   // +0x2C
    int cursor;                   // +0x30
    int font;                     // +0x34
    u32 color;                    // +0x38
    int alignment;                // +0x3C
    f32 scale, widthScale;        // +0x40,+0x44
    u8 active;                    // +0x48
};
}

extern "C" {extern const f32 lbl_80563930,lbl_80563940;extern const char lbl_8055C08C[6];}
inline void ProfileFontState::DrawCoordinatesNew(const void* text,f32 x,f32 y)const{fn_80232BA4(text, font, x, y, color, alignment, scale, lbl_80563930, widthScale);}
void ProfileTextState::Draw(void) {
    ProfileTextDrawFields* self=(ProfileTextDrawFields*)this;
    f32 x, y;
    fn_80230C30(&x, &y, self->x, self->y);
    if (self->active) self->color = 0xFFFFFFFF;
    else {
        self->color = 0xFF000000;
        int height = self->height;
        f32 width = fn_8023293C(self->font, self->text, self->widthScale);
        width=self->scale*width;fn_80230924(0xFFFFFFFF, x, y, x+width, y - (f32)height, lbl_80563940);
    }
    // Use the observed font payload through a typed inline call.
    // The original method name and inline boundary remain unknown.
    f32 callY=y;f32 callX=x;this->font.DrawCoordinatesNew(self->stringObject,callX,callY);
    // Original prefix starts at sp+0x10; capacity 18 is inferred from slot names,
    // not independently recoverable from the non-bounds-checked sprintf call.
    char prefix[18];
    sprintf(prefix, lbl_8055C08C, self->cursor, self->cursor, self->text);
    f32 width = fn_8023293C(self->font, prefix, self->widthScale) * self->scale;
    f32 cursorX = x + width;
    fn_80230924(0xFFFFD03F, cursorX - lbl_80563930, y, lbl_80563930 + cursorX,
               y - (f32)self->height, lbl_80563940);
}

extern "C" {


// Physical cleanup views only. The original retains null checks for embedded
// member addresses; these are represented explicitly, not claimed idiomatic C++.
void fn_802328D4(int);
void __dl__FPv(void*);
struct ProfileTextCleanupFields { u8 pad0[0x34]; int font; };
struct ProfileKeyboardCleanupFields {
    u8 pad0[0x18];
    ProfileTextCleanupFields text;
    u8 pad50[0x28];
    int font;
};
}

ProfileKeyboardState::~ProfileKeyboardState(void) {}

extern "C" {
}



extern "C" {


extern char lbl_80498A58[4][18];
extern char lbl_80498AA0[4][100];
void* memset(void*, int, unsigned long);
void fn_801308F4(char*, int, u32);
// Achievement result is retained as an opaque register-sized word: both this
// build's producer and consumer are four-byte stubs, so no pointee type is inferred.
static inline void ProfileCopyLabelNew(const char* src,int n,char* dst){while((*dst++=*src++)!=0){if(--n<=0){dst[-1]=0;break;}}}
void fn_8024CAAC(int emptyAvailable) {
    char* name = &lbl_80498A58[0][0];
    char* description = &lbl_80498AA0[0][0];
    u8* available;int slot=0;available=lbl_8055F2D0;
    for (; slot < 4; slot++, name += 18, description += 100, available++) {
        if (IdentIsSlotEmpty(slot)) {
            const char* src = fn_80131234("PlayerName:EMPTY");
            ProfileCopyLabelNew(src,18,name);
            memset(description, 0, 100);
            *available = emptyAvailable;
        } else {
            char key[100];
            fn_801345D8(key, 100, "PlayerName:%d", slot + 1);
            const char* src = fn_80131234(key);
            ProfileCopyLabelNew(src,18,name);
            fn_801308F4(description, 100, AchievementsGetForSlot(0, slot));
            *available = 1;
        }
    }
}


struct ProfileKeyboardDrawFields {
    u8 pad0[4]; int x, y, width, height;
    u8 pad14[0x50];
    int selectedColumn, selectedRow; // +0x64,+0x68
    bool upper; char shiftLabel[10]; u8 pad77;
    int font; u32 color; int alignment; f32 scale, widthScale;
};
extern const char lbl_80344120[40];
extern const char lbl_80344148[40];
// The callee's va_list starts after two GPR and two FP fixed arguments.
// Its first two FP values are drawing coordinates, not format substitutions.
void fn_80234AFC(const void*, f32, f32, const char*, ...);
}

void ProfileKeyboardState::Draw(void) {
    ProfileKeyboardDrawFields* self=(ProfileKeyboardDrawFields*)this;
    f32 x, y;
    fn_80230C30(&x, &y, self->x, self->y);
    f32 cellWidth = (f32)self->width / 10.0f;
    f32 cellHeight = (f32)self->height * 0.25f;
    f32 left = x + cellWidth * (f32)self->selectedColumn;
    f32 top = y - cellHeight * (f32)self->selectedRow;
    fn_80230924(0xFFAF7F00, left, top, left + cellWidth, top - 4.0f, 0.0f);
    fn_80230924(0xFFAF7F00, left, top - cellHeight, left + cellWidth, 4.0f + (top - cellHeight), 0.0f);
    fn_80230924(0xFFAF7F00, left, top, left + 4.0f, top - cellHeight, 0.0f);
    fn_80230924(0xFFAF7F00, left + cellWidth, top, (left + cellWidth) - 4.0f, top - cellHeight, 0.0f);
    y = y - cellHeight * 0.5f;
    f32 startX = x + cellWidth * 0.5f;
    for (int row = 0; row < 4; row++) {
        x = startX;
        for (int column = 0; column < 10; column++) {
            if (row == 3 && column == 9) fn_80234AFC(&self->font, x, y, "%cOK", 1);
            else if (row == 3 && column == 8) fn_80234AFC(&self->font, x, y, "%cBS", 1);
            else {
                const char* keys = self->upper ? "!@#$%^&*()ABCDEFGHIJKLMNOPQRSTUVWXYZ. " : "1234567890abcdefghijklmnopqrstuvwxyz. ";
                fn_80234AFC(&self->font, x, y, "%c%c", 1, keys[row * 10 + column]);
            }
            x = x + cellWidth;
        }
        y = y - cellHeight;
    }
}

extern "C" {


// Original BSS is exactly the following physical spans, without dummy data.
static ModuleTag profileModuleTag;
static UnkStatic123 profileStatic123;
static UnkStatic7F profileStatic7F;
char lbl_80498A00[4][18];
int lbl_80498A48[4];
char lbl_80498A58[4][18];
char lbl_80498AA0[4][100];
char* lbl_8055F2C4;
ProfileFlagSpan profileFlags;
u8 lbl_8055F2D0[8];


// Menu records preserve five-word physical layout; callback source types remain
// provisional. The current source is a compiling first draft, not a linked unit.
struct ProfileMenuItem {
    ProfileMenuItem(const char* n, int t, const void* d, int a0, int a1) {
        type=t; name=n; data=d; arg0=a0; arg1=a1;
    }
    ProfileMenuItem(const UnkStatic123& end) {
        name=(const char*)end.unk0; type=end.unk4; data=(const void*)end.unk8;
        arg0=end.unkC; arg1=end.unk10;
    }
    const char* name; int type; const void* data; int arg0,arg1;
};
struct ProfileMenuDesc {
    const char* layout; ProfileMenuItem* items;
    int (*poll)(int*); int unkC,unk10;
};
extern char lbl_8055BAC0[];
extern char lbl_8055B860[];
int fn_802146BC(ProfileMenuDesc*, int, int, int);
void fn_802149C4(const char*);
void fn_802149FC(const char*);

int fn_8024C768(void) {
    ProfileMenuItem items[] = {
        ProfileMenuItem(lbl_8055BAC0,8,(const void*)fn_8024C628,0,0),
        ProfileMenuItem("slot_1",3,lbl_80498A00[0],0,0),
        ProfileMenuItem("slot_1",0,(const void*)fn_8024C6F4,0,0),
        ProfileMenuItem("icon_1",2,&lbl_80498A48[0],0,0),
        ProfileMenuItem("slot_2",3,lbl_80498A00[1],0,0),
        ProfileMenuItem("slot_2",0,(const void*)fn_8024C6F4,1,0),
        ProfileMenuItem("icon_2",2,&lbl_80498A48[1],0,0),
        ProfileMenuItem("slot_3",3,lbl_80498A00[2],0,0),
        ProfileMenuItem("slot_3",0,(const void*)fn_8024C6F4,2,0),
        ProfileMenuItem("icon_3",2,&lbl_80498A48[2],0,0),
        ProfileMenuItem("slot_4",3,lbl_80498A00[3],0,0),
        ProfileMenuItem("slot_4",0,(const void*)fn_8024C6F4,3,0),
        ProfileMenuItem("icon_4",2,&lbl_80498A48[3],0,0),
        ProfileMenuItem("load",0,0,4,0),
        ProfileMenuItem("save",0,0,5,0),
        ProfileMenuItem(lbl_8055B860,6,0,-1,27),
        ProfileMenuItem(profileStatic123)
    };
    ProfileMenuDesc desc={"demoprofile.lyt",0,0,0,0};
    desc.items=items;
    fn_8024C464();
    if (!(signed char)lbl_8055F2CA) {
        lbl_8055F2CC=1;
        lbl_8055F2CA=1;
    }
    int done=0;
    int result=-1;
    while (!done) {
        if (lbl_8055F2CC && lbl_8055F2C9) fn_802149C4(desc.layout);
        result=fn_802146BC(&desc,-999,-999,1);
        if (result==-1) {
            lbl_8055F2CC=1;
            fn_802149FC(desc.layout);
        } else lbl_8055F2CC=0;
        switch(result) {
        case -1: done=1; break;
        case 4: fn_8024C524(); break;
        case 5: fn_8024C550(); break;
        default: done=fn_8024C578(result); break;
        }
    }
    return result!=-1;
}

int fn_8024CC54(int* slot, int (*confirm)(int), int create) {
    ProfileMenuItem items[] = {
        ProfileMenuItem(lbl_8055BAC0,8,(const void*)fn_8024CBF4,0,0),
        ProfileMenuItem("slot_1",3,lbl_80498A58[0],0,0),
        ProfileMenuItem("info_1",3,lbl_80498AA0[0],0,0),
        ProfileMenuItem("slot_1",0,(const void*)confirm,0,0),
        ProfileMenuItem("slot_2",3,lbl_80498A58[1],0,0),
        ProfileMenuItem("info_2",3,lbl_80498AA0[1],0,0),
        ProfileMenuItem("slot_2",0,(const void*)confirm,1,0),
        ProfileMenuItem("slot_3",3,lbl_80498A58[2],0,0),
        ProfileMenuItem("info_3",3,lbl_80498AA0[2],0,0),
        ProfileMenuItem("slot_3",0,(const void*)confirm,2,0),
        ProfileMenuItem("slot_4",3,lbl_80498A58[3],0,0),
        ProfileMenuItem("info_4",3,lbl_80498AA0[3],0,0),
        ProfileMenuItem("slot_4",0,(const void*)confirm,3,0),
        ProfileMenuItem("load",0,0,4,0),
        ProfileMenuItem("save",0,0,5,0),
        ProfileMenuItem(lbl_8055B860,6,0,-1,27),
        ProfileMenuItem(profileStatic123)
    };
    ProfileMenuDesc desc={"demoprofile.lyt",0,0,0,0};
    desc.items=items;
    fn_8024CAAC(create);
    int result=fn_802146BC(&desc,-999,-999,1);
    if (result==-1) return 0;
    if (slot) *slot=result;
    return 1;
}

struct ProfileKeyboardInputFields {
    u8 pad0[0x44]; char* text; int cursor;
    u8 pad4C[0x14]; u8 dirty; u8 pad61[3];
    int selectedColumn,selectedRow;
    bool upper; char shiftLabel[10]; u8 pad77;
};
}

int ProfileKeyboardState::Key(int key) {
    ProfileKeyboardInputFields* self=(ProfileKeyboardInputFields*)this;
    switch (key) {
    case 0x113: {
        self->upper=!self->upper;
        const char* src=fn_80131234(self->upper ? "UIKeyboard:SHIFT_UPPER" : "UIKeyboard:SHIFT_LOWER");
        int n=10;
        char* dst=self->shiftLabel;
        while ((*dst++=*src++)!=0) {
            if (--n<=0) {dst[-1]=0;break;}
        }
        break;
    }
    case 0x126: self->selectedRow=(self->selectedRow+3)%4; break;
    case 0x128: self->selectedRow=(self->selectedRow+1)%4; break;
    case 0x125: self->selectedColumn=(self->selectedColumn+9)%10; break;
    case 0x127: self->selectedColumn=(self->selectedColumn+1)%10; break;
    case 13: {
        if (self->selectedColumn==9 && self->selectedRow==3) return 0;
        if (self->selectedColumn==8 && self->selectedRow==3) {
            int i=self->cursor;
            if (i==0) break;
            int length=strlen(self->text);
            if (length==0) break;
            for (;i<=length;i++) self->text[i-1]=self->text[i];
            self->cursor=self->cursor-1;
            self->text[length-1]=0;
            self->dirty=1;
        } else {
            const char* keys=self->upper ? "!@#$%^&*()ABCDEFGHIJKLMNOPQRSTUVWXYZ. " : "1234567890abcdefghijklmnopqrstuvwxyz. ";
            char ch=keys[self->selectedRow*10+self->selectedColumn];
            if (!self->dirty) {
                memset(self->text,0,17);
                self->cursor=0;
            }
            int length=strlen(self->text);
            if (length==16) break;
            for (int i=length;i>self->cursor;i--) self->text[i]=self->text[i-1];
            self->text[self->cursor]=ch;
            self->dirty=1;
            self->cursor=self->cursor+1;
        }
        break;
    }
    default: return 0;
    }
    return 1;
}

extern "C" {

}
