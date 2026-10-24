// Track B1 partial draft. Offsets are observed fields, not asserted class names.
#include "game/common.h"
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


struct CreditMethodTableFields {const void* slots[24];};
typedef char CreditTableSize[sizeof(CreditMethodTableFields)==96?1:-1];
extern "C" {
struct CreditItemFields {
CreditItemFields(void);
    CreditItemFields(const void* n,int k,void* c,int m,int e) {kind=k;token=n;callback=c;mode=m;extra=e;}
    CreditItemFields(const CreditItemFields& other) {token=other.token;kind=other.kind;callback=other.callback;mode=other.mode;extra=other.extra;}
    const void* token; int kind; void* callback; int mode; int extra;
};
struct CreditFadeFields {
    const void* vtable;
    u8 pad4[0x10];
    int state;                    // +0x14
    f32 value, saved, result;      // +0x18,+0x1C,+0x20; result overlaps timer done
};
struct CreditTimerFields {
    const void* vtable;
    u8 pad4[0x10];
    int state;
    f32 remaining, duration;
    u8 done;
};
extern const u32 lbl_8055C458;
extern f32 lbl_8055C454;

// Unsized address view reproduces original absolute access to the known 4-byte object.
extern const f32 lbl_80558DC4[];
extern const CreditMethodTableFields lbl_803466D8, lbl_803465B8, lbl_80346558;
extern const char lbl_8055C4A8[8], lbl_803464EC[];
extern CreditFadeFields* lbl_8055F444;
extern void* lbl_8055F448;
extern CreditTimerFields* lbl_8055F44C;
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
void fn_8026451C(CreditFadeFields* self);
int fn_802645C0(void);
f32 fn_80264588(void);
void fn_802645EC(void);
void fn_802645FC(CreditFadeFields* self);
CreditFadeFields* fn_80265558(CreditFadeFields* self);
void fn_8026557C(CreditFadeFields* self, f32 value);
void fn_80265584(CreditFadeFields* self);
void fn_80265598(CreditFadeFields* self);
CreditTimerFields* fn_802655A4(CreditTimerFields* self);
void fn_80265610(CreditTimerFields* self, f32 duration);
void fn_80265618(CreditTimerFields* self);
void fn_8026562C(CreditTimerFields* self);
int fn_80265688(void);
void fn_802658FC(void);
void fn_80265904(void);
void fn_80265A28(void);
void fn_80265A34(void);
int fn_80265B5C(void);

// Physical views, not claims about original class or field names.
struct CreditStateFields {const void* methods; u8 reserved4[0x10]; int state;};
struct CreditListHeader {
    ~CreditListHeader(void) {items=0;count=0;capacity=0;}
    CreditStateFields** items;int count,capacity;
};
struct CreditStateListFields : public CreditListHeader {
void Append(CreditStateFields* value){CreditStateFields** data=items;int index=count++;data[index]=value;}

    CreditStateListFields(void) {items=storage;capacity=50;count=0;}
    ~CreditStateListFields(void) {}
    CreditStateFields* storage[50];
};
extern CreditStateListFields lbl_80498F78;
extern const char lbl_803464A8[];
extern const CreditMethodTableFields lbl_803464F8,lbl_80346618;
void* __nw__FUlPCc(unsigned long,const char*);
void __dl__FPv(void*);
void Finish__9MenuStateFi(void*,int);
unsigned long strlen(const char*);
struct CreditScrollFields {
    const void* methods; s32 x,y,width,height; int state;
    void* font; char text[150]; u8 reservedB2[2]; int length; f32 offset;
};
typedef char CreditScrollLayout[sizeof(CreditScrollFields)==0xBC?1:-1];
void fn_80264484(CreditItemFields* self);
CreditFadeFields* fn_80264538(CreditFadeFields* self,int deleteFlag);
CreditTimerFields* fn_802655C8(CreditTimerFields* self,int deleteFlag);
void* fn_80265B9C(void* self,int deleteFlag);
void* fn_80265BDC(void* self,int deleteFlag);
void* fn_80265C1C(void* self,int deleteFlag);
}
class CreditUpdateInterface {
public: virtual void Slot08(void); virtual void Slot0C(void);
virtual void Slot10(void); virtual void Slot14(void); virtual void Slot18(void);
virtual void Update(void);
};
extern "C" {
void fn_80264FD4(CreditStateFields* self);
CreditScrollFields* fn_8026523C(CreditScrollFields* self);
void fn_80265268(CreditScrollFields* self,void* font,const char* text,f32 offset);
void fn_80264618(CreditFadeFields* self);

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
UnkStatic123s creditsStatic123;
static UnkStatic7F creditsStatic7F;
CreditStateListFields lbl_80498F78;
CreditFadeFields* lbl_8055F444;
void* lbl_8055F448;
CreditTimerFields* lbl_8055F44C;
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
// Dispatch-only interface: the observed deleting destructor is slot +0x08.
class CreditDeleteInterface {public:virtual ~CreditDeleteInterface(void);};
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
struct CreditGlyphBatchFields {void* items;int count,capacity;void* allocation;f32 inverseX,inverseY,inverseZ,elapsed;};
struct CreditFontGroupFields {CreditFontGroupFields(const CreditTextDescFields&);void Spawn(const ValPoint&);CreditTextDescFields desc;int resource;CreditGlyphBatchFields glyphs;};
struct CreditBackgroundHeader {CreditBackgroundHeader(const void* t){state=0;methods=t;}const void* methods;u8 reserved4[0x10];int state;};
struct CreditBackgroundFields:CreditBackgroundHeader {CreditBackgroundFields();CreditFontGroupFields fire,smoke;};
typedef char CreditDescriptorSize[sizeof(CreditTextDescFields)==0x3C?1:-1];
typedef char CreditGroupSize[sizeof(CreditFontGroupFields)==0x60?1:-1];
typedef char CreditBackgroundSize[sizeof(CreditBackgroundFields)==0xD8?1:-1];
extern const CreditTextDescFields lbl_802FA700;
extern CreditTextDescFields lbl_80346364;
extern const CreditMethodTableFields lbl_80346678;
extern const char lbl_8055C490[8];

int fn_8023512C(const void*);
}
static inline f32 CreditObservedMax(f32 a,f32 b){return a>b?a:b;}
static inline void CreditGroupInit(CreditFontGroupFields* self,int capacity) {
    void* buffer=__nwa__FUlPCc(capacity*32,lbl_8055C490);
    self->glyphs.items=buffer;self->glyphs.capacity=capacity;self->glyphs.count=0;self->glyphs.allocation=buffer;
    self->glyphs.inverseX=(1.0f)/CreditObservedMax((0.0010000000474974513f),self->desc.lifetime);
    self->glyphs.inverseY=(255.0f)/CreditObservedMax((0.0010000000474974513f),self->desc.lifetime);
    self->glyphs.inverseZ=(1.0f)/CreditObservedMax((0.0010000000474974513f),self->desc.fadeTime);
    self->glyphs.count=0;self->glyphs.elapsed=(0.0f);
}

inline CreditFontGroupFields::CreditFontGroupFields(const CreditTextDescFields& d):desc(d),resource(fn_8023512C(desc.resource)){CreditGroupInit(this,d.capacity);}
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
CreditBackgroundFields* fn_80264968(CreditBackgroundFields*,int);
void fn_80263F30(int before,u8 allowSkip,int after);

void fn_80235380(int);
f32 fn_80235F64(int,u32);
f32 fn_80235EF0(int,u32);
void fn_80235AFC(int,u32,u32,f32,f32,f32,f32,f32,f32);
}
// Borrowed cleanup views mirror observed nested member destruction. They do
// not change the raw background view's automatic lifetime in the entry draft.
struct CreditGlyphCleanup : public CreditListHeader {
    ~CreditGlyphCleanup(void) {__dla__FPv(allocation);}
    void* allocation;f32 inverseX,inverseY,inverseZ,elapsed;
};
struct CreditGroupCleanup {
    ~CreditGroupCleanup(void) {if(resource){fn_80235380(resource);resource=0;}}
    CreditTextDescFields desc;int resource;CreditGlyphCleanup glyphs;
};
struct CreditGlyphFields {
    ValPoint position,velocity;f32 angle,angularVelocity,remaining;u32 glyph;
};
typedef char CreditGlyphSize[sizeof(CreditGlyphFields)==32?1:-1];
typedef char CreditCleanupSize[sizeof(CreditGroupCleanup)==0x60?1:-1];
static inline void CreditRemoveGlyph(CreditGlyphBatchFields* batch,int index) {
    if(index<0 || index>=batch->count)return;
    CreditGlyphFields* glyphs=(CreditGlyphFields*)batch->items;
    if(index!=batch->count-1)glyphs[index]=glyphs[batch->count-1];
    batch->count--;
}
static inline void CreditUpdateGroup(CreditFontGroupFields* self,f32 delta) {
    int index=0;
    while(index<self->glyphs.count) {
        CreditGlyphFields* glyph=&((CreditGlyphFields*)self->glyphs.items)[index];
        glyph->remaining-=delta;
        if(glyph->remaining<lbl_80558DC4[0])CreditRemoveGlyph(&self->glyphs,index);
        else {
            glyph->velocity.x+=self->desc.accelerationX*delta;
            glyph->velocity.y+=self->desc.accelerationY*delta;
            glyph->position.x+=glyph->velocity.x*delta;
            glyph->position.y+=glyph->velocity.y*delta;
            glyph->angle+=glyph->angularVelocity*delta;
            index++;
        }
    }
}
static inline void CreditDrawGroup(CreditFontGroupFields* self) {
    for(int index=0;index<self->glyphs.count;index++) {
        CreditGlyphFields* glyph=&((CreditGlyphFields*)self->glyphs.items)[index];
        f32 fade=self->glyphs.inverseZ*(self->desc.lifetime-glyph->remaining);
        if((1.0f)<fade)fade=(1.0f);
        fade=fade>(0.0f)?fade:(0.0f);
        int opacity=(int)(fade*(glyph->remaining*self->glyphs.inverseY));
        opacity=opacity>0?opacity:0;
        opacity=opacity<255?opacity:255;
        f32 scale=self->desc.size+self->desc.sizeDelta*((1.0f)-glyph->remaining*self->glyphs.inverseX);
        u32 code=glyph->glyph;
        f32 yScale=scale*fn_80235F64(self->resource,code);
        f32 xScale=scale*fn_80235EF0(self->resource,glyph->glyph);
        fn_80235AFC(self->resource,code,((u32)opacity<<24)|0xFFFFFF,
          glyph->position.x,glyph->position.y,(0.0f),xScale,yScale,glyph->angle);
    }
}
extern "C" {
CreditBackgroundFields* fn_80264968(CreditBackgroundFields* self,int deleteFlag);
void fn_80264D5C(CreditBackgroundFields* self);
void fn_80264FE4(CreditBackgroundFields* self);

extern const f32 lbl_80563C18,lbl_80563C1C;
void fn_8022ACFC(void*,const char*,f32,f32,f32*,f32*,f32*,f32*);
u32 fn_8022AA84(void*);
void fn_8022BD40(void*,const char*,int,int,f32,f32,f32,f32,f32,f32);
void fn_80264A5C(f32,f32);
void fn_802652E8(CreditScrollFields* self);

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
        glyph->velocity.x=speed*(f32)cos((f64)angle);
        glyph->velocity.y=speed*(f32)sin((f64)angle);
        glyph->angle=fn_80129DC4(6,(0.0f),(6.2831854820251465f));
        glyph->angularVelocity=fn_80129DC4(6,self->desc.spinMin,self->desc.spinMax);
        glyph->remaining=self->desc.lifetime;
        glyph->glyph=fn_80129D18(6,fn_80235FD8(self->resource));
    }
}
extern "C" {
void fn_80264A5C(f32 x,f32 y);

void fn_802153A0(void);
void fn_80215CA0(void);
void fn_80215CA4(void);
void fn_80215CA8(void);
void fn_80215CAC(void);
void fn_80215CB0(void);
void fn_80215CB4(void);
void fn_80215CB8(void);
void fn_80215CBC(void);
void fn_80215CC0(void);
void fn_80215CC4(void);
void fn_80215CC8(void);
void fn_80215CCC(void);
void fn_80215CD0(void);
void fn_80215CD4(void);
void fn_80215CD8(void);
void fn_80215CDC(void);
void fn_80215CE0(void);
void fn_80215CE4(void);
void fn_80215CEC(void);
void fn_802153A4(void);

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

__declspec(section ".data") const CreditMethodTableFields lbl_803464F8 = {{
    0,
    0,
    (const void*)fn_80265B9C,
    (const void*)fn_802153A0,
    (const void*)fn_80215CA0,
    (const void*)fn_80215CA4,
    (const void*)fn_80215CA8,
    (const void*)fn_80215CAC,
    (const void*)fn_80215CB0,
    (const void*)fn_80215CB4,
    (const void*)fn_80215CB8,
    (const void*)fn_80215CBC,
    (const void*)fn_80215CC0,
    (const void*)fn_80215CC4,
    (const void*)fn_80215CC8,
    (const void*)fn_80215CCC,
    (const void*)fn_80215CD0,
    (const void*)fn_80215CD4,
    (const void*)fn_80215CD8,
    (const void*)fn_80215CDC,
    (const void*)fn_80215CE0,
    (const void*)fn_80215CE4,
    (const void*)fn_80265688,
    (const void*)fn_80215CEC
}};
__declspec(section ".data") const CreditMethodTableFields lbl_80346558 = {{
    0,
    0,
    (const void*)fn_802655C8,
    (const void*)fn_80265618,
    (const void*)fn_80215CA0,
    (const void*)fn_80215CA4,
    (const void*)fn_80215CA8,
    (const void*)fn_8026562C,
    (const void*)fn_80215CB0,
    (const void*)fn_80215CB4,
    (const void*)fn_80215CB8,
    (const void*)fn_80215CBC,
    (const void*)fn_80215CC0,
    (const void*)fn_80215CC4,
    (const void*)fn_80215CC8,
    (const void*)fn_80215CCC,
    (const void*)fn_80215CD0,
    (const void*)fn_80215CD4,
    (const void*)fn_80215CD8,
    (const void*)fn_80215CDC,
    (const void*)fn_80215CE0,
    (const void*)fn_80215CE4,
    (const void*)fn_802153A4,
    (const void*)fn_80215CEC
}};
__declspec(section ".data") const CreditMethodTableFields lbl_803465B8 = {{
    0,
    0,
    (const void*)fn_80265BDC,
    (const void*)fn_80265584,
    (const void*)fn_80265598,
    (const void*)fn_80215CA4,
    (const void*)fn_80215CA8,
    (const void*)fn_80215CAC,
    (const void*)fn_80215CB0,
    (const void*)fn_80215CB4,
    (const void*)fn_80215CB8,
    (const void*)fn_80215CBC,
    (const void*)fn_80215CC0,
    (const void*)fn_80215CC4,
    (const void*)fn_80215CC8,
    (const void*)fn_80215CCC,
    (const void*)fn_80215CD0,
    (const void*)fn_80215CD4,
    (const void*)fn_80215CD8,
    (const void*)fn_80215CDC,
    (const void*)fn_80215CE0,
    (const void*)fn_80215CE4,
    (const void*)fn_802153A4,
    (const void*)fn_80215CEC
}};
__declspec(section ".data") const CreditMethodTableFields lbl_80346618 = {{
    0,
    0,
    (const void*)fn_80265C1C,
    (const void*)fn_802153A0,
    (const void*)fn_80215CA0,
    (const void*)fn_80215CA4,
    (const void*)fn_80215CA8,
    (const void*)fn_80215CAC,
    (const void*)fn_80215CB0,
    (const void*)fn_80215CB4,
    (const void*)fn_802652E8,
    (const void*)fn_80215CBC,
    (const void*)fn_80215CC0,
    (const void*)fn_80215CC4,
    (const void*)fn_80215CC8,
    (const void*)fn_80215CCC,
    (const void*)fn_80215CD0,
    (const void*)fn_80215CD4,
    (const void*)fn_80215CD8,
    (const void*)fn_80215CDC,
    (const void*)fn_80215CE0,
    (const void*)fn_80215CE4,
    (const void*)fn_802153A4,
    (const void*)fn_80215CEC
}};
__declspec(section ".data") const CreditMethodTableFields lbl_80346678 = {{
    0,
    0,
    (const void*)fn_80264968,
    (const void*)fn_802153A0,
    (const void*)fn_80215CA0,
    (const void*)fn_80215CA4,
    (const void*)fn_80215CA8,
    (const void*)fn_80264D5C,
    (const void*)fn_80264FD4,
    (const void*)fn_80215CB4,
    (const void*)fn_80264FE4,
    (const void*)fn_80215CBC,
    (const void*)fn_80215CC0,
    (const void*)fn_80215CC4,
    (const void*)fn_80215CC8,
    (const void*)fn_80215CCC,
    (const void*)fn_80215CD0,
    (const void*)fn_80215CD4,
    (const void*)fn_80215CD8,
    (const void*)fn_80215CDC,
    (const void*)fn_80215CE0,
    (const void*)fn_80215CE4,
    (const void*)fn_802153A4,
    (const void*)fn_80215CEC
}};
__declspec(section ".data") const CreditMethodTableFields lbl_803466D8 = {{
    0,
    0,
    (const void*)fn_80264538,
    (const void*)fn_802645FC,
    (const void*)fn_80215CA0,
    (const void*)fn_80215CA4,
    (const void*)fn_80215CA8,
    (const void*)fn_80264618,
    (const void*)fn_80215CB0,
    (const void*)fn_80215CB4,
    (const void*)fn_80215CB8,
    (const void*)fn_80215CBC,
    (const void*)fn_80215CC0,
    (const void*)fn_80215CC4,
    (const void*)fn_80215CC8,
    (const void*)fn_80215CCC,
    (const void*)fn_80215CD0,
    (const void*)fn_80215CD4,
    (const void*)fn_80215CD8,
    (const void*)fn_80215CDC,
    (const void*)fn_80215CE0,
    (const void*)fn_80215CE4,
    (const void*)fn_802153A4,
    (const void*)fn_80215CEC
}};

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
        CreditFadeFields fade;fn_8026451C(&fade);
        ValueNode* node;int keepGoing;int index;keepGoing=1;index=0;
        while(keepGoing && (node=table->GetChild(index))!=0) {
            if(!strcasecmp(node->GetName(),lbl_803463E4))keepGoing=fn_802656C8(node);
            index++;
        }
        fn_8020A648(-1);fn_80265A74();
        if(!fn_802ECF04())fn_802E317C(lbl_803463F8);
        fn_80131818(table);
        fn_80264538(&fade,-1);fn_80264968(&background,-1);
    }
    if(after)fn_80265940();
    fn_8020A648(previous);
}

extern "C" void fn_80264098(CreditItemFields* self, int kind) { self->kind = kind; }

CreditItemFields::CreditItemFields(void){callback=0;kind=123;token=&lbl_8055C458;mode=0;extra=0;}

extern "C" void fn_802640C4(CreditItemFields* self,ValueNode* node) {
    const char* type=node->GetValue(lbl_8055C45C);
    if(!strcasecmp(type,lbl_8055C464)) {
        self->token=node->GetValue(lbl_8055C46C);
        fn_80264098(self,3);
        self->callback=(void*)node->GetValue(lbl_8055C474);self->mode=0;
    } else if(!strcasecmp(type,lbl_80346428)) {
        CreditScrollFields* state=(CreditScrollFields*)__nw__FUlPCc(0xBC,lbl_80346434);
        if(state)state=fn_8026523C(state);
        f32 offset=node->GetFloat(lbl_8055C484,(0.0f));
        const char* text=node->GetValue(lbl_8055C474);
        int font=node->GetIntValue(lbl_8055C47C);
        fn_80265268(state,(void*)font,text,offset);
        lbl_80498F78.Append((CreditStateFields*)state);
        self->token=node->GetValue(lbl_8055C46C);fn_80264098(self,5);self->callback=state;
    } else if(!strcasecmp(type,lbl_80346440)) {
        CreditFadeFields* state=(CreditFadeFields*)__nw__FUlPCc(0x20,lbl_8034644C);
        if(state)state=fn_80265558(state);
        fn_8026557C(state,node->GetFloat(lbl_8055C484,(0.0f)));
        lbl_80498F78.Append((CreditStateFields*)state);
        self->token=0;fn_80264098(self,5);self->callback=state;
    } else if(!strcasecmp(type,lbl_80346458)) {
        CreditTimerFields* state=(CreditTimerFields*)__nw__FUlPCc(0x24,lbl_80346464);
        if(state)state=fn_802655A4(state);
        fn_80265610(state,node->GetFloat(lbl_8055C484,(0.0f)));
        lbl_80498F78.Append((CreditStateFields*)state);
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
    CreditStateFields* state=(CreditStateFields*)__nw__FUlPCc(0x18,lbl_803464A8);
    if(state) {state->state=0;state->methods=&lbl_803464F8;}
    lbl_80498F78.Append(state);
    self->token=0;fn_80264098(self,5);self->callback=state;
}

extern "C" void fn_8026451C(CreditFadeFields* self) {
    self->vtable = &lbl_803466D8;
    self->state = 0;
    lbl_8055F444 = self;
}

extern "C" CreditFadeFields* fn_80264538(CreditFadeFields* self,int deleteFlag) {
    if(self) {lbl_8055F444=0;if(deleteFlag>0)__dl__FPv(self);}return self;
}

extern "C" void* fn_80264580(void) { return lbl_8055F444; }

extern "C" f32 fn_80264588(void) {
    if (fn_802645C0()) return lbl_8055F444->result;
    return (-1.0f);
}

extern "C" int fn_802645C0(void) { return (f32)__fabs((f64)lbl_8055F444->value) <= lbl_80558DC4[0]; }

extern "C" void fn_802645EC(void) { lbl_8055F444->saved = lbl_8055C454; }

extern "C" void fn_802645FC(CreditFadeFields* self) {
    self->value = (0.5f);
    self->saved = lbl_8055C454;
    self->result = (0.0f);
}

extern "C" void fn_80264618(CreditFadeFields* self) {
    if((f32)__fabs((f64)(*(volatile f32*)&self->saved))<=(*(const volatile f32*)lbl_80558DC4)) {Finish__9MenuStateFi(self,0);fn_80216CA8();}
    f32 next=self->value-fn_80214D48();
    self->value=(0.0f)>next?(0.0f):next;
    if(fn_802645C0())self->result+=fn_80214D48();
    next=self->saved-fn_80214D48();
    self->saved=(0.0f)>next?(0.0f):next;
}

CreditBackgroundFields::CreditBackgroundFields():CreditBackgroundHeader(&lbl_80346678),fire(lbl_802FA700),smoke(lbl_80346364){lbl_8055F448=this;}

extern "C" CreditBackgroundFields* fn_80264968(CreditBackgroundFields* self,int deleteFlag) {
    if(self) {
        lbl_8055F448=0;
        ((CreditGroupCleanup*)&self->smoke)->~CreditGroupCleanup();
        ((CreditGroupCleanup*)&self->fire)->~CreditGroupCleanup();
        if(deleteFlag>0)__dl__FPv(self);
    }
    return self;
}

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

extern "C" void fn_80264D5C(CreditBackgroundFields* self) {
    CreditUpdateGroup(&self->fire,fn_80214D48());
    CreditUpdateGroup(&self->smoke,fn_80214D48());
}

extern "C" void fn_80264FD4(CreditStateFields* self) {
    ((CreditUpdateInterface*)self)->Update();
}

extern "C" void fn_80264FE4(CreditBackgroundFields* self) {
    CreditDrawGroup(&self->smoke);CreditDrawGroup(&self->fire);
}

extern "C" CreditScrollFields* fn_8026523C(CreditScrollFields* self) {
    self->state=0;self->methods=&lbl_80346618;self->font=0;
    self->text[0]=0;self->length=0;self->offset=(0.0f);
    return self;
}

extern "C" void fn_80265268(CreditScrollFields* self,void* font,const char* text,f32 offset) {
    self->font=font;int n=150;char* dst=self->text;
    while((*dst++=*text++)!=0) {if(--n<=0){dst[-1]=0;break;}}
    self->length=strlen(self->text);self->offset=offset;
}

struct CreditTextBoundsNew {f32 left,top,right,bottom;};
extern "C" void fn_802652E8(CreditScrollFields* self) {
    f32 elapsed=fn_80264588();
    if(elapsed<self->offset)return; {
        f32 progress=lbl_80563C18*(elapsed-self->offset);
        CreditTextBoundsNew full;
        fn_8022ACFC(self->font,self->text,(f32)self->x,(f32)self->y,&full.left,&full.top,&full.right,&full.bottom);
        int cutoff=(int)progress;
        bool complete=cutoff>self->length;
        if(complete)cutoff=self->length;
        else fn_802645EC();
        char savedByte=self->text[cutoff];self->text[cutoff]=0;int saved=savedByte;
        CreditTextBoundsNew partial;
        fn_8022ACFC(self->font,self->text,(f32)self->x,(f32)self->y,&partial.left,&partial.top,&partial.right,&partial.bottom);
        f32 delta=(full.right-full.left)-(partial.right-partial.left);
        f32 x=(f32)self->x;
        u32 flags=fn_8022AA84(self->font);
        if(flags&2)x-=delta;
        else if(flags&4)x-=(0.5f)*delta;
        fn_8022BD40(self->font,self->text,0,0,x,(f32)self->y,(0.0f),full.right-full.left,full.bottom-full.top,(1.0f));
        self->text[cutoff]=saved;
        if(!complete) {
            f32 fraction=progress-(f32)cutoff;
            f32 caret=(f32)(int)(lbl_80563C1C*fraction)+(full.left+partial.right-partial.left);
            fn_80264A5C(caret,(full.bottom+full.top)/2.0f);
        }
    }
}

extern "C" CreditFadeFields* fn_80265558(CreditFadeFields* self) {
    self->vtable = &lbl_803465B8;
    self->state = 0;
    self->value = (0.0f);
    self->saved = (0.0f);
    return self;
}

extern "C" void fn_8026557C(CreditFadeFields* self, f32 value) { self->value = value; }

extern "C" void fn_80265584(CreditFadeFields* self) {
    self->saved = lbl_8055C454;
    lbl_8055C454 = self->value;
}

extern "C" void fn_80265598(CreditFadeFields* self) { lbl_8055C454 = self->saved; }

extern "C" CreditTimerFields* fn_802655A4(CreditTimerFields* self) {
    self->vtable = &lbl_80346558;
    self->state = 0;
    lbl_8055F44C = self;
    self->duration = (0.0f);
    return self;
}

extern "C" CreditTimerFields* fn_802655C8(CreditTimerFields* self,int deleteFlag) {
    if(self) {lbl_8055F44C=0;if(deleteFlag>0)__dl__FPv(self);}return self;
}

extern "C" void fn_80265610(CreditTimerFields* self, f32 duration) { self->duration = duration; }

extern "C" void fn_80265618(CreditTimerFields* self) { self->done = 0; self->remaining = self->duration; }

extern "C" void fn_8026562C(CreditTimerFields* self) {
    fn_802645EC();
    if (self->remaining > lbl_80558DC4[0]) self->remaining -= fn_80214D48();
    else self->done = 1;
}

extern "C" int fn_80265688(void) {
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
    for(int i=0;i<lbl_80498F78.count;i++)delete (CreditDeleteInterface*)lbl_80498F78.items[i];
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

extern "C" void* fn_80265B9C(void* self,int deleteFlag) {
    if(self && deleteFlag>0)__dl__FPv(self);return self;
}

extern "C" void* fn_80265BDC(void* self,int deleteFlag) {
    if(self && deleteFlag>0)__dl__FPv(self);return self;
}

extern "C" void* fn_80265C1C(void* self,int deleteFlag) {
    if(self && deleteFlag>0)__dl__FPv(self);return self;
}


extern "C" {
__declspec(section ".sdata2") const f32 lbl_80563C18=30.0f;
__declspec(section ".sdata2") const f32 lbl_80563C1C=10.0f;
}
