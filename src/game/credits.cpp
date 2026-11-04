// Track B1 partial draft. Offsets are observed fields, not asserted class names.
#include "game/common.h"
#include "game/menustate.h"
void* operator new(unsigned long size, const char* tag);
void* operator new[](unsigned long,const char*);
extern "C" {
extern const char lbl_80346338[12];
extern const char lbl_80346344[14];
extern const char lbl_80346354[15];
extern const char lbl_803463A0[12];
extern const char lbl_803463AC[54];
extern const char lbl_803463E4[13];
extern const char lbl_803463F8[48];
extern const char lbl_80346428[9];
extern const char lbl_80346434[9];
extern const char lbl_80346440[12];
extern const char lbl_8034644C[11];
extern const char lbl_80346458[11];
extern const char lbl_80346464[11];
extern const char lbl_80346470[54];
extern const char lbl_803464A8[11];
extern const char lbl_803464B4[19];
extern const char lbl_803464C8[14];
extern const char lbl_803464D8[17];
extern const char lbl_803464EC[9];
}



class CreditsFade;
class CreditsKeyEnabler;





extern "C" {
// Reconstructed typed input view; original type and helper boundaries are unknown.
struct CreditTokenInputObservedNew {
    const void* value;
    inline CreditTokenInputObservedNew(const void* v) { value=v; }
};
typedef char CreditTokenInputSize4[sizeof(CreditTokenInputObservedNew)==4?1:-1];
struct CreditItemFields {
    inline const void* InitializeTokenInputHeaderObservedNew(CreditTokenInputObservedNew name,int k) {
        kind=k;
        return name.value;
    }
CreditItemFields(void);
    CreditItemFields(const void* n,int k,void* c,int m,int e) {kind=k;token=n;callback=c;mode=m;extra=e;}
    CreditItemFields(const CreditItemFields& other) {token=other.token;kind=other.kind;callback=other.callback;mode=other.mode;extra=other.extra;}
    const void* token; int kind; void* callback; int mode; int extra;
};
extern const u32 lbl_8055C458;
extern f32 lbl_8055C454;

// Unsized address view reproduces original absolute access to the known 4-byte object.
extern f32 lbl_80558DC4[];
extern const char lbl_8055C4A8[8], lbl_803464EC[];
extern CreditsFade* lbl_8055F444;
extern void* lbl_8055F448;
extern CreditsKeyEnabler* lbl_8055F44C;
extern u8 lbl_8055F450;

void fn_80236914(void);
void fn_80236954(const char*);
int fn_8023695C(const char*);
f32 fn_80214D48(void);
void fn_802168BC(int);
void fn_80216D28(void);
void fn_80216CA8(void);
int fn_80265B5C(void);

void fn_80264098(CreditItemFields* self, int kind);
void fn_802640A0(CreditItemFields* self);
void* fn_80264580(void);
void* fn_80264A54(void);
void fn_802643B8(CreditItemFields* self);
void fn_80264404(CreditItemFields* self);
void fn_80264444(CreditItemFields* self);
int fn_802645C0(void);
f32 fn_80264588(void);
void fn_802645EC(void);
void fn_802658FC(void);
void fn_80265904(void);
void fn_80265A28(void);
void fn_80265A34(void);
int fn_80265B5C(void);

// Physical views, not claims about original class or field names.
struct CreditListHeader {
    ~CreditListHeader(void) {items=0;count=0;capacity=0;}
    MenuState** items;int count,capacity;
};
struct CreditStateListFields : public CreditListHeader {
void Append(MenuState* value){MenuState** data=items;int index=count++;data[index]=value;}

    CreditStateListFields(void) {items=storage;capacity=50;count=0;}
    ~CreditStateListFields(void) {}
    MenuState* storage[50];
};
extern CreditStateListFields lbl_80498F78;
extern const char lbl_803464A8[];
void* __nw__FUlPCc(unsigned long,const char*);
void __dl__FPv(void*);
unsigned long strlen(const char*);
void fn_80264484(CreditItemFields* self);
}
extern "C" {

struct CreditMenuDescFields {const char* layout;CreditItemFields* items;void* poll;int argC,arg10;};
extern const CreditMenuDescFields lbl_802FA73C,lbl_802FA750;
extern UnkStatic123s creditsStatic123;
#define lbl_80498F58 (*(const CreditItemFields*)&creditsStatic123)
extern const char lbl_8055BAC0[1],lbl_8055BAB8[1];
int fn_802146BC(CreditMenuDescFields*,int,int,int);
void fn_80265940(void);
void fn_80265A74(void);

// The actual startup stores identify a 20-byte sentinel, rather than 24.
static ModuleTag creditsModuleTag;
static UnkStatic7F creditsStatic7F;
UnkStatic123s creditsStatic123;
CreditStateListFields lbl_80498F78;
CreditsFade* lbl_8055F444;
void* lbl_8055F448;
CreditsKeyEnabler* lbl_8055F44C;
u8 lbl_8055F450;

void* __nwa__FUlPCc(unsigned long,const char*);
void* __construct_new_array(void*,void*,void*,unsigned long,unsigned long);
void __dla__FPv(void*);
extern const char lbl_803464B4[],lbl_803464C8[];
extern const char lbl_8055C498[8],lbl_8055C4A0[8];
int strcasecmp(const char*,const char*);
void fn_802640C4(CreditItemFields*,ValueNode*);
void fn_80236910(const char*);
}
extern "C" {
int fn_802656C8(ValueNode* node);

extern const char lbl_80346338[];
extern const char lbl_8055C45C[5],lbl_8055C464[7],lbl_8055C46C[5];
extern const char lbl_8055C474[5],lbl_8055C47C[6],lbl_8055C484[6];
void fn_802E317C(const char*,...);
void fn_802640C4(CreditItemFields* self,ValueNode* node);

struct CreditTextDescFields {
    const void* resource;f32 angleMin,angleMax,speedMin,speedMax;
    f32 accelerationX,accelerationY,spinMin,spinMax;int capacity;f32 emissionRate;
    f32 lifetime,fadeTime,size,sizeDelta;
};
struct CreditGlyphStorageObservedNew {CreditGlyphStorageObservedNew(int);~CreditGlyphStorageObservedNew(){items=0;count=0;capacity=0;}void* items;int count,capacity;};
struct CreditGlyphBatchFields:CreditGlyphStorageObservedNew {CreditGlyphBatchFields(int);~CreditGlyphBatchFields(){__dla__FPv(allocation);}void* allocation;f32 inverseX,inverseY,inverseZ,elapsed;};
struct CreditFontGroupFields {CreditFontGroupFields(const CreditTextDescFields&);~CreditFontGroupFields();void Spawn(const ValPoint&);CreditTextDescFields desc;int resource;CreditGlyphBatchFields glyphs;};
// The credits' menu states: menu items of type 5 point at one. MWCC emits their vtables in REVERSE declaration
// order, so they are declared here in the reverse of their .data order (lbl_803464F8 QuitHotkey, KeyEnabler,
// ExtraDelay, FireText, background, lbl_803466D8 fade), followed by the zero-filled space of this unit's
// weak IMenuState vtable copy.
// The fade that times the whole credits run; one lives on fn_80263F30's stack (lbl_8055F444 points at it).
class CreditsFade : public MenuState {
public:
    CreditsFade(void);
    virtual ~CreditsFade(void);
    virtual void Enter(void);
    virtual void Update(void);

    f32 value;   // offset 0x18
    f32 saved;   // offset 0x1C
    f32 result;  // offset 0x20: seconds since the fade finished
};

// The credits' background: fire and smoke glyph sprays (lbl_8055F448 points at it).
class CreditBackgroundFields : public MenuState {
public:
    CreditBackgroundFields(void);
    virtual ~CreditBackgroundFields(void);
    virtual void Update(void);
    virtual void Unk20(void);
    virtual void Draw(void);

    CreditFontGroupFields fire;   // offset 0x18
    CreditFontGroupFields smoke;  // offset 0x78
};

// "firetext" item ("TextWoot"): types its text out, with sparks at the caret.
class CreditsFireText : public MenuState {
public:
    CreditsFireText(void);
    virtual ~CreditsFireText(void);
    virtual void Draw(void);
    void Set(void* f, const char* t, f32 delay);

    void* font;      // offset 0x18
    char text[150];  // offset 0x1C
    int length;      // offset 0xB4
    f32 offset;      // offset 0xB8: seconds before typing starts
};
typedef char CreditsFireTextSize[sizeof(CreditsFireText) == 0xBC ? 1 : -1];

// "ExtraDelay" item: swaps the text-hold time lbl_8055C454 while its page is shown.
class CreditsExtraDelay : public MenuState {
public:
    CreditsExtraDelay(void);
    virtual ~CreditsExtraDelay(void);
    virtual void Enter(void);
    virtual void Exit(void);
    void Set(f32 v);

    f32 value;  // offset 0x18
    f32 saved;  // offset 0x1C
};

// "KeyEnabler" item: keys are allowed once its timer runs out (lbl_8055F44C points at it).
class CreditsKeyEnabler : public MenuState {
public:
    CreditsKeyEnabler(void);
    virtual ~CreditsKeyEnabler(void);
    virtual void Enter(void);
    virtual void Update(void);
    void SetDuration(f32 d);

    f32 remaining;  // offset 0x18
    f32 duration;   // offset 0x1C
    u8 done;        // offset 0x20
};

// "QuitHotkey" item: any key ends the credits once keys are enabled.
class CreditsQuitHotkey : public MenuState {
public:
    CreditsQuitHotkey(void) {}
    virtual ~CreditsQuitHotkey(void);
    virtual int Key(int key);
};
typedef char CreditDescriptorSize[sizeof(CreditTextDescFields)==0x3C?1:-1];
typedef char CreditGroupSize[sizeof(CreditFontGroupFields)==0x60?1:-1];
typedef char CreditBackgroundSize[sizeof(CreditBackgroundFields)==0xD8?1:-1];
extern const CreditTextDescFields lbl_802FA700;
extern CreditTextDescFields lbl_80346364;
extern const char lbl_8055C490[8];

int fn_8023512C(const void*);
}
static inline f32 CreditObservedMax(f32 a,f32 b){return a>b?a:b;}
inline CreditGlyphStorageObservedNew::CreditGlyphStorageObservedNew(int n){void* buffer=__nwa__FUlPCc(n*32,lbl_8055C490);items=buffer;capacity=n;count=0;}
inline CreditGlyphBatchFields::CreditGlyphBatchFields(int n):CreditGlyphStorageObservedNew(n),allocation(items){}
static inline void CreditGroupInit(CreditFontGroupFields* self,int capacity) {

    self->glyphs.inverseX=(1.0f)/CreditObservedMax((0.0010000000474974513f),self->desc.lifetime);
    self->glyphs.inverseY=(255.0f)/CreditObservedMax((0.0010000000474974513f),self->desc.lifetime);
    self->glyphs.inverseZ=(1.0f)/CreditObservedMax((0.0010000000474974513f),self->desc.fadeTime);
    self->glyphs.count=0;self->glyphs.elapsed=(0.0f);
}

inline CreditFontGroupFields::CreditFontGroupFields(const CreditTextDescFields& d):desc(d),resource(fn_8023512C(desc.resource)),glyphs(d.capacity){CreditGroupInit(this,d.capacity);}
extern "C" {
CreditBackgroundFields* fn_802646D0(CreditBackgroundFields* self);

// Imported calls only; no reconstruction in the held AX/SPU range.
int fn_8020A69C(void);
void fn_8020A648(int);
ValueTable* fn_80131810(const char*);
void fn_80131818(ValueTable*);
int fn_802ECF00(int);
int fn_802ECF04(void);
extern int lbl_8055C450;
void fn_80263F30(int before,u8 allowSkip,int after);

void fn_80235380(int);
f32 fn_80235F64(int,u32);
f32 fn_80235EF0(int,u32);
void fn_80235AFC(int,u32,u32,f32,f32,f32,f32,f32,f32);
}
struct CreditVelocityArrayObservedNew {f32 values[2];};typedef char CreditVelocityArraySizeNew[sizeof(CreditVelocityArrayObservedNew)==8?1:-1];
struct CreditGlyphFields {
    ValPoint position;CreditVelocityArrayObservedNew velocity;f32 angle,angularVelocity,remaining;u32 glyph;
};
typedef char CreditGlyphSize[sizeof(CreditGlyphFields)==32?1:-1];
typedef char CreditGlyphStorageSize[sizeof(CreditGlyphStorageObservedNew)==12?1:-1];
typedef char CreditGlyphBatchSize[sizeof(CreditGlyphBatchFields)==32?1:-1];
static inline void CreditRemoveGlyph(CreditGlyphBatchFields* batch,int index) {
    if(index<0 || index>=batch->count)return;
    if(index!=batch->count-1){CreditGlyphFields* glyphs=(CreditGlyphFields*)batch->items;glyphs[index]=glyphs[batch->count-1];}
    batch->count--;
}
static inline void CreditUpdateGroup(CreditFontGroupFields* self,f32 delta) {
    int index=0;
    while(index<self->glyphs.count) {
        CreditGlyphFields* glyph=&((CreditGlyphFields*)self->glyphs.items)[index];
        glyph->remaining-=delta;
        if(glyph->remaining<lbl_80558DC4[0])CreditRemoveGlyph(&self->glyphs,index);
        else {f32& vx=glyph->velocity.values[0];f32& vy=glyph->velocity.values[1];
            f32 accelerationX=self->desc.accelerationX;vx+=accelerationX*delta;
            vy+=self->desc.accelerationY*delta;
            glyph->position.x+=vx*delta;
            glyph->position.y+=vy*delta;
            glyph->angle+=glyph->angularVelocity*delta;
            index++;
        }
    }
}
static inline f32 CreditDrawScaleObservedNew(const CreditFontGroupFields* self,const CreditGlyphFields* glyph){return self->desc.size+self->desc.sizeDelta*((1.0f)-glyph->remaining*self->glyphs.inverseX);}
static inline void CreditDrawGroup(CreditFontGroupFields* self,int index=0) {f32 lowerFadeBound;

  
  while (index < self->glyphs.count)
  {
    CreditGlyphFields *glyph = &((CreditGlyphFields *) self->glyphs.items)[index];

    f32 fade = self->glyphs.inverseZ * (self->desc.lifetime - glyph->remaining);
    f32 upperFadeBound = 1.0f;
    if (upperFadeBound < fade)
    {
      fade = 1.0f;
    }
    lowerFadeBound=0.0f;fade = (fade > lowerFadeBound) ? fade : 0.0f;
    int opacity = (int) (fade * (glyph->remaining * self->glyphs.inverseY));
    opacity = (opacity > 0) ? (opacity) : (0);
    opacity = (opacity < 255) ? (opacity) : (255);
    f32 scale = CreditDrawScaleObservedNew(self, glyph);
    u32 code = glyph->glyph;

    f32 yScale = scale * fn_80235F64(self->resource, glyph->glyph);
    f32 xScale = scale * fn_80235EF0(self->resource, glyph->glyph);
    fn_80235AFC(self->resource, code, (((u32) opacity) << 24) | 0xFFFFFF, glyph->position.x, glyph->position.y, 0.0f, xScale, yScale, glyph->angle);
    ++index;
  }

}
static inline void CreditDrawFireGroupObservedNew(CreditFontGroupFields* self) {f32 yScale;

  int index = 0;
  while (index < self->glyphs.count)
  {
    CreditGlyphFields *glyph = &((CreditGlyphFields *) self->glyphs.items)[index];

    f32 fade = self->glyphs.inverseZ * (self->desc.lifetime - glyph->remaining);
    f32 upperFadeBound = 1.0f;
    if (upperFadeBound < fade)
    {
      fade = 1.0f;
    }
    fade = (fade > 0.0f) ? (fade) : (0.0f);
    int opacity = (int) (fade * (glyph->remaining * self->glyphs.inverseY));
    opacity = (opacity > 0) ? (opacity) : (0);
    opacity = (opacity < 255) ? (opacity) : (255);
    f32 scale = CreditDrawScaleObservedNew(self, glyph);
    u32 code = glyph->glyph;

    yScale = scale * fn_80235F64(self->resource, glyph->glyph);
    f32 xScale = scale * fn_80235EF0(self->resource, glyph->glyph);
    fn_80235AFC(self->resource, code, (((u32) opacity) << 24) | 0xFFFFFF, glyph->position.x, glyph->position.y, 0.0f, xScale, yScale, glyph->angle);
    ++index;
  }

}
extern "C" {

extern const f32 lbl_80563C18,lbl_80563C1C;
void fn_8022ACFC(void*,const char*,f32,f32,f32*,f32*,f32*,f32*);
u32 fn_8022AA84(void*);
void fn_8022BD40(void*,const char*,int,int,f32,f32,f32,f32,f32,f32);
void fn_80264A5C(f32,f32);

void fn_80230C30(f32*,f32*,int,int);
f32 fn_80129DC4(int,f32,f32);
int fn_80129D18(int,int);
int fn_80235FD8(int);
f64 cos(f64);f64 sin(f64);

}
inline void CreditFontGroupFields::Spawn(const ValPoint& position) {CreditFontGroupFields* self=this;
    self->glyphs.elapsed+=self->desc.emissionRate;
    if(self->glyphs.elapsed>(1.0f) && self->glyphs.count+1<=self->glyphs.capacity) {
        self->glyphs.elapsed-=(1.0f);
        CreditGlyphFields* glyph=&((CreditGlyphFields*)self->glyphs.items)[self->glyphs.count++];
        glyph->position.x=position.x;glyph->position.y=position.y;
        f32 angle=fn_80129DC4(6,self->desc.angleMin,self->desc.angleMax);
        f32 speed=fn_80129DC4(6,self->desc.speedMin,self->desc.speedMax);
        glyph->velocity.values[0]=speed*(f32)cos((f64)angle);
        glyph->velocity.values[1]=speed*(f32)sin((f64)angle);
        glyph->angle=fn_80129DC4(6,(0.0f),(6.2831854820251465f));
        glyph->angularVelocity=fn_80129DC4(6,self->desc.spinMin,self->desc.spinMax);
        glyph->remaining=self->desc.lifetime;
        glyph->glyph=fn_80129D18(6,fn_80235FD8(self->resource));
    }
}
extern "C" {
void fn_80264A5C(f32 x,f32 y);


extern const char lbl_80346338[12];
extern const char lbl_80346344[14];
extern const char lbl_80346354[15];
extern const char lbl_803463A0[12];
extern const char lbl_803463AC[54];
extern const char lbl_803463E4[13];
extern const char lbl_803463F8[48];
extern const char lbl_80346428[9];
extern const char lbl_80346434[9];
extern const char lbl_80346440[12];
extern const char lbl_8034644C[11];
extern const char lbl_80346458[11];
extern const char lbl_80346464[11];
extern const char lbl_80346470[54];
extern const char lbl_803464A8[11];
extern const char lbl_803464B4[19];
extern const char lbl_803464C8[14];
extern const char lbl_803464D8[17];
extern const char lbl_803464EC[9];
__declspec(section ".data") const char lbl_80346338[12]="credits.bik";
__declspec(section ".data") const char lbl_80346344[14]="cred_fire.art";
__declspec(section ".data") const char lbl_80346354[15]="cred_smoke.art";
__declspec(section ".data") CreditTextDescFields lbl_80346364={lbl_80346354,1.346396803855896f,1.7951958179473877f,30.0f,40.0f,-50.0f,0.0f,-0.5235987901687622f,0.5235987901687622f,1024,0.4000000059604645f,1.5f,0.30000001192092896f,0.5f,1.0f};
__declspec(section ".data") const char lbl_803463A0[12]="credits.val";
__declspec(section ".data") const char lbl_803463AC[54]="credits: failed to push my movie (%s) onto the stack!";
__declspec(section ".data") const char lbl_803463E4[13]="credits_page";
__declspec(section ".data") const char lbl_803463F8[48]="credits: failed to pop my movie from the stack!";
__declspec(section ".data") const char lbl_80346428[9]="firetext";
__declspec(section ".data") const char lbl_80346434[9]="TextWoot";
__declspec(section ".data") const char lbl_80346440[12]="extra_delay";
__declspec(section ".data") const char lbl_8034644C[11]="ExtraDelay";
__declspec(section ".data") const char lbl_80346458[11]="allow_keys";
__declspec(section ".data") const char lbl_80346464[11]="KeyEnabler";
__declspec(section ".data") const char lbl_80346470[54]="ValTable_UIDialogMapping: can't handle item type '%s'";
__declspec(section ".data") const char lbl_803464A8[11]="QuitHotkey";
__declspec(section ".data") const char lbl_803464B4[19]="credits_page_items";
__declspec(section ".data") const char lbl_803464C8[14]="a_credits.val";
__declspec(section ".data") const char lbl_803464D8[17]="credits_fade.lyt";
__declspec(section ".data") const char lbl_803464EC[9]="fade_out";


__declspec(section ".rodata") const CreditTextDescFields lbl_802FA700={lbl_80346344,1.0471975803375244f,2.094395160675049f,100.0f,200.0f,0.0f,200.0f,0.0f,0.0f,128,0.30000001192092896f,0.5f,0.009999999776482582f,1.5f,0.800000011920929f};
__declspec(section ".rodata") const CreditMenuDescFields lbl_802FA73C={lbl_803464D8,0,0,0,0};
__declspec(section ".rodata") const CreditMenuDescFields lbl_802FA750={lbl_803464D8,0,0,0,0};
__declspec(section ".sdata") int lbl_8055C450=(int)lbl_80346338;
__declspec(section ".sdata") f32 lbl_8055C454=1.5f;
__declspec(section ".sdata") const u32 lbl_8055C458=0;
__declspec(section ".sdata") const char lbl_8055C45C[5]="type";
__declspec(section ".sdata") const char lbl_8055C464[7]="string";
__declspec(section ".sdata") const char lbl_8055C46C[5]="name";
__declspec(section ".sdata") const char lbl_8055C474[5]="text";
__declspec(section ".sdata") const char lbl_8055C47C[6]="style";
__declspec(section ".sdata") const char lbl_8055C484[6]="timer";
__declspec(section ".sdata") const char lbl_8055C490[8]="dsa_buf";
__declspec(section ".sdata") const char lbl_8055C498[8]="item";
__declspec(section ".sdata") const char lbl_8055C4A0[8]="layout";
__declspec(section ".sdata") const char lbl_8055C4A8[8]="fade_in";



}

// Definitions ordered by validated original identity, not pair position.
extern "C" void fn_80263F30(int before,u8 allowSkip,int after) {
    lbl_8055F450=allowSkip;
    int previous=fn_8020A69C();fn_8020A648(1);
    if(before)fn_80265A74();
    ValueTable* table=fn_80131810(lbl_803463A0);
    if(table) {
        if(!fn_802ECF00(lbl_8055C450))fn_802E317C(lbl_803463AC,lbl_8055C450);
        fn_80265940();
        CreditBackgroundFields background;
        CreditsFade fade;
        ValueNode* node;int keepGoing;int index;keepGoing=1;index=0;
        while(keepGoing && (node=table->GetChild(index))!=0) {
            if(!strcasecmp(node->GetName(),lbl_803463E4))keepGoing=fn_802656C8(node);
            index++;
        }
        fn_8020A648(-1);fn_80265A74();
        if(!fn_802ECF04())fn_802E317C(lbl_803463F8);
        fn_80131818(table);
    }
    if(after)fn_80265940();
    fn_8020A648(previous);
}

extern "C" void fn_80264098(CreditItemFields* self, int kind) { self->kind = kind; }

CreditItemFields::CreditItemFields(void) {
    token=InitializeTokenInputHeaderObservedNew(CreditTokenInputObservedNew(&lbl_8055C458),123);
    callback=0;
    mode=0;
    extra=0;
}

extern "C" void fn_802640C4(CreditItemFields* self,ValueNode* node) {
    const char* type=node->GetValue(lbl_8055C45C);
    if(!strcasecmp(type,lbl_8055C464)) {
        self->token=node->GetValue(lbl_8055C46C);
        fn_80264098(self,3);
        self->callback=(void*)node->GetValue(lbl_8055C474);self->mode=0;
    } else if(!strcasecmp(type,lbl_80346428)) {
        CreditsFireText* state=new(lbl_80346434) CreditsFireText;
        state->Set((void*)node->GetIntValue(lbl_8055C47C),node->GetValue(lbl_8055C474),node->GetFloat(lbl_8055C484,(0.0f)));
        lbl_80498F78.Append(state);
        self->token=node->GetValue(lbl_8055C46C);fn_80264098(self,5);self->callback=state;
    } else if(!strcasecmp(type,lbl_80346440)) {
        CreditsExtraDelay* state=new(lbl_8034644C) CreditsExtraDelay;
        state->Set(node->GetFloat(lbl_8055C484,(0.0f)));
        lbl_80498F78.Append(state);
        self->token=0;fn_80264098(self,5);self->callback=state;
    } else if(!strcasecmp(type,lbl_80346458)) {
        CreditsKeyEnabler* state=new(lbl_80346464) CreditsKeyEnabler;
        state->SetDuration(node->GetFloat(lbl_8055C484,(0.0f)));
        lbl_80498F78.Append(state);
        self->token=0;fn_80264098(self,5);self->callback=state;
    } else fn_802E317C(lbl_80346470,type);
}

extern "C" void fn_802643B8(CreditItemFields* self) {
    self->token = &lbl_8055C458;
    fn_80264098(self, 8);
    self->callback = (void*)fn_80236914;
    self->mode = 2;
}

extern "C" void fn_80264404(CreditItemFields* self) {
    self->token = 0;
    fn_80264098(self, 5);
    self->callback = fn_80264A54();
}

extern "C" void fn_80264444(CreditItemFields* self) {
    self->token = 0;
    fn_80264098(self, 5);
    self->callback = fn_80264580();
}

extern "C" void fn_80264484(CreditItemFields* self) {
    CreditsQuitHotkey* state=new(lbl_803464A8) CreditsQuitHotkey;
    lbl_80498F78.Append(state);
    self->token=0;fn_80264098(self,5);self->callback=state;
}

CreditsFade::CreditsFade(void) { lbl_8055F444 = this; }

CreditsFade::~CreditsFade(void) { lbl_8055F444 = 0; }

extern "C" void* fn_80264580(void) { return lbl_8055F444; }

extern "C" f32 fn_80264588(void) {
    if (fn_802645C0()) return lbl_8055F444->result;
    return (-1.0f);
}

extern "C" int fn_802645C0(void) { return (f32)__fabs((f64)lbl_8055F444->value) <= lbl_80558DC4[0]; }

extern "C" void fn_802645EC(void) { lbl_8055F444->saved = lbl_8055C454; }

void CreditsFade::Enter(void) {
    value = (0.5f);
    saved = lbl_8055C454;
    result = (0.0f);
}

void CreditsFade::Update(void) {
    if((f32)__fabs((f64)(*(volatile f32*)&saved))<=(*(const volatile f32*)lbl_80558DC4)) {Finish(0);fn_80216CA8();}
    f32 next=value-fn_80214D48();
    value=(0.0f)>next?(0.0f):next;
    if(fn_802645C0())result+=fn_80214D48();
    next=saved-fn_80214D48();
    saved=(0.0f)>next?(0.0f):next;
}

CreditBackgroundFields::CreditBackgroundFields():fire(lbl_802FA700),smoke(lbl_80346364){lbl_8055F448=this;}

inline CreditFontGroupFields::~CreditFontGroupFields(){if(resource){fn_80235380(resource);resource=0;}}
CreditBackgroundFields::~CreditBackgroundFields(){lbl_8055F448=0;}

extern "C" void* fn_80264A54(void) { return lbl_8055F448; }

extern "C" void fn_80264A5C(f32 x,f32 y) {
    if((f32)__fabs((f64)fn_80214D48())<=lbl_80558DC4[0])return;
    f32 screenX,screenY;
    fn_80230C30(&screenX,&screenY,(int)x,(int)y);
    screenX+=x-(f32)(int)x;screenY-=y-(f32)(int)y;
    CreditBackgroundFields* self=(CreditBackgroundFields*)lbl_8055F448;
    ValPoint firePosition={screenX,screenY};self->fire.Spawn(firePosition);
    self=(CreditBackgroundFields*)lbl_8055F448;
    ValPoint smokePosition={screenX,(5.0f)+screenY};self->smoke.Spawn(smokePosition);
}

void CreditBackgroundFields::Update(void) {
    CreditUpdateGroup(&fire,fn_80214D48());
    CreditUpdateGroup(&smoke,fn_80214D48());
}

void CreditBackgroundFields::Unk20(void) { Update(); }

void CreditBackgroundFields::Draw(void) {
    CreditDrawGroup(&smoke);CreditDrawFireGroupObservedNew(&fire);
}

CreditsFireText::CreditsFireText(void) {
    font=0;
    text[0]=0;length=0;offset=(0.0f);
}

void CreditsFireText::Set(void* f,const char* t,f32 delay) {
    font=f;int n=150;char* dst=text;
    while((*dst++=*t++)!=0) {if(--n<=0){dst[-1]=0;break;}}
    length=strlen(text);offset=delay;
}

struct CreditTextBoundsNew {f32 left,top,right,bottom;};
void CreditsFireText::Draw(void) {
    f32 elapsed=fn_80264588();
    if(elapsed<offset)return; {
        f32 progress=lbl_80563C18*(elapsed-offset);
        CreditTextBoundsNew full;
        fn_8022ACFC(font,text,(f32)x,(f32)y,&full.left,&full.top,&full.right,&full.bottom);
        int cutoff=(int)progress;
        bool complete=cutoff>length;
        if(complete)cutoff=length;
        else fn_802645EC();
        char savedByte=text[cutoff];text[cutoff]=0;int saved=savedByte;
        CreditTextBoundsNew partial;
        fn_8022ACFC(font,text,(f32)x,(f32)y,&partial.left,&partial.top,&partial.right,&partial.bottom);
        f32 delta=(full.right-full.left)-(partial.right-partial.left);
        f32 drawX=(f32)x;
        u32 flags=fn_8022AA84(font);
        if(flags&2)drawX-=delta;
        else if(flags&4)drawX-=(0.5f)*delta;
        fn_8022BD40(font,text,0,0,drawX,(f32)y,(0.0f),full.right-full.left,full.bottom-full.top,(1.0f));
        text[cutoff]=saved;
        if(!complete) {
            f32 fraction=progress-(f32)cutoff;
            f32 caret=(f32)(int)(lbl_80563C1C*fraction)+(full.left+partial.right-partial.left);
            fn_80264A5C(caret,(full.bottom+full.top)/2.0f);
        }
    }
}

CreditsExtraDelay::CreditsExtraDelay(void) {
    value = (0.0f);
    saved = (0.0f);
}

void CreditsExtraDelay::Set(f32 v) { value = v; }

void CreditsExtraDelay::Enter(void) {
    saved = lbl_8055C454;
    lbl_8055C454 = value;
}

void CreditsExtraDelay::Exit(void) { lbl_8055C454 = saved; }

CreditsKeyEnabler::CreditsKeyEnabler(void) {
    lbl_8055F44C = this;
    duration = (0.0f);
}

CreditsKeyEnabler::~CreditsKeyEnabler(void) { lbl_8055F44C = 0; }

void CreditsKeyEnabler::SetDuration(f32 d) { duration = d; }

void CreditsKeyEnabler::Enter(void) { done = 0; remaining = duration; }

void CreditsKeyEnabler::Update(void) {
    fn_802645EC();
    if (remaining > lbl_80558DC4[0]) remaining -= fn_80214D48();
    else done = 1;
}

int CreditsQuitHotkey::Key(int key) {
    if (fn_80265B5C()) { fn_802168BC(-1); return 1; }
    fn_80216D28(); return 0;
}

extern "C" int fn_802656C8(ValueNode* node) {
    int count=0,index=0;ValueNode* child;
    while((child=node->GetChildAt(index))!=0) {
        if(!strcasecmp(child->GetName(),lbl_8055C498))count++;
        index++;
    }
int allocated=count+5;CreditItemFields* items=new(lbl_803464B4) CreditItemFields[allocated];
    index=0;
    int secondIndex=0;
    while((child=node->GetChildAt(secondIndex))!=0) {
        if(!strcasecmp(child->GetName(),lbl_8055C498))fn_802640C4(&items[index++],child);
        secondIndex++;
    }
    fn_802643B8(&items[index++]);
    fn_80264404(&items[index++]);
    fn_80264444(&items[index++]);
    fn_80264484(&items[index]);
    CreditMenuDescFields desc={0,0,0,0,0};
    desc.layout=node->GetValue(lbl_8055C4A0);
    desc.items=items;
    fn_80236910(lbl_803464C8);
    int result=fn_802146BC(&desc,-999,-999,1);
    if(items)__dla__FPv((char*)items-16);
    for(int i=0;i<lbl_80498F78.count;i++)delete lbl_80498F78.items[i];
    lbl_80498F78.count=0;
    return result!=-1;
}

extern "C" void fn_802658FC(void) { fn_80236954(lbl_8055C4A8); }

extern "C" void fn_80265904(void) {
    fn_80236914();
    if (fn_8023695C(lbl_8055C4A8) == 2) { fn_80216CA8(); fn_802168BC(0); }
}

extern "C" void fn_80265940(void) {
    CreditItemFields items[]={
      CreditItemFields(lbl_8055BAC0,8,(void*)fn_802658FC,0,0),
      CreditItemFields(lbl_8055BAB8,8,(void*)fn_80265904,2,0),
      lbl_80498F58
    };
    CreditMenuDescFields desc=lbl_802FA73C;desc.items=items;
    fn_802146BC(&desc,-999,-999,1);
}

extern "C" void fn_80265A28(void) { fn_80236954(lbl_803464EC); }

extern "C" void fn_80265A34(void) {
    fn_80236914();
    if (fn_8023695C(lbl_803464EC) == 2) { fn_80216CA8(); fn_802168BC(0); }
}

extern "C" void fn_80265A74(void) {
    CreditItemFields items[]={
      CreditItemFields(lbl_8055BAC0,8,(void*)fn_80265A28,0,0),
      CreditItemFields(lbl_8055BAB8,8,(void*)fn_80265A34,2,0),
      lbl_80498F58
    };
    CreditMenuDescFields desc=lbl_802FA750;desc.items=items;
    fn_802146BC(&desc,-999,-999,1);
}

static inline bool CreditTimerDone(void){return lbl_8055F44C && lbl_8055F44C->done;}
extern "C" int fn_80265B5C(void){return CreditTimerDone() || lbl_8055F450;}

CreditsQuitHotkey::~CreditsQuitHotkey(void) {}

CreditsExtraDelay::~CreditsExtraDelay(void) {}

CreditsFireText::~CreditsFireText(void) {}


extern "C" {
__declspec(section ".sdata2") const f32 lbl_80563C18=30.0f;
__declspec(section ".sdata2") const f32 lbl_80563C1C=10.0f;
}
