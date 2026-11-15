#include <stddef.h>
#include "game/menucustomview.h"
#include "game/uiwidget.h"
#include "game/stringlist.h"
#include "game/uitable.h"
#include "game/gart.h"
#include "game/gxcanvas.h"
#include "game/image.h"
#include "game/uicontext.h"
#include "game/gtext.h"
#include "game/fonttext.h"
#include "game/uidraw.h"
typedef Vec3 RotationVec;typedef Mat3 RotationMatrix;
void Mat3FromEuler(Mat3* out, const Vec3* euler);  // weak, Y then X then Z (racing.cpp)
void Mat3Mul(Mat3* out, const Mat3* a, const Mat3* b);  // racing.cpp
void* fn_80211660(long i);  // uiapi.cpp
// Saves the viewport and sets a new one; the destructor restores it (mrender.cpp).
struct Viewport {Viewport(int x,int y,int w,int h);~Viewport(void);int x,y,w,h;};
static inline void SetVec3(Vec3* out,float x,float y,float z){out->x=x;out->y=y;out->z=z;}
static inline void SetMat3Row(Mat3* m,int row,float x,float y,float z){m->m[row][0]=x;m->m[row][1]=y;m->m[row][2]=z;}
static inline void SetUIVec3(UIVec3* v,float x,float y,float z){v->v[0]=x;v->v[1]=y;v->v[2]=z;}

struct RotatingChoiceDescriptorNew;
static inline float MapMinBranchNew(float a,float b){if(a<b)return a;return b;}
static inline float MapMaxBranchNew(float a,float b){if(a>b)return a;return b;}
extern "C" {extern const float lbl_805634A4,lbl_805634A8;}
static inline float PaletteNaturalLerp(float first,float last,float amount){return first+amount*(last-first);}
static inline float PaletteNaturalRemap(float value,float lower,float upper){float fraction=(value-lower)/(upper-lower);fraction=MapMinBranchNew(1.0f,fraction);return MapMaxBranchNew(fraction,0.0f);}
static inline float MapSpanRatioNew(float value,float origin,float extent){return(value-origin)/extent;}
static inline float MapScopeNew(float screen,float value,float span){float fraction=MapSpanRatioNew(value,0.0f,span);fraction=MapMinBranchNew(1.0f,fraction);fraction=MapMaxBranchNew(fraction,0.0f);return PaletteNaturalLerp(0.0f,screen,fraction);}
static inline int MapIntegerMinNew(int a,int b){if(a<b)return a;return b;}
// Anonymous widget-class fragment; no recovered TU or data ownership claim.
typedef unsigned int u32;
struct Binding {int type;int pad;union {int code;unsigned char key;};unsigned char latched;};
typedef char BindingSize16[sizeof(Binding)==16?1:-1];
struct WidgetDispatch {virtual ~WidgetDispatch();virtual void Slot0c();virtual void Slot10();virtual void Slot14();virtual void Slot18();virtual void Slot1c(int,int);virtual void Slot20();virtual void Slot24(void*);virtual void Slot28();virtual void Slot2c();virtual void Slot30();virtual void Slot34();virtual void Slot38();virtual void Slot3c(int,int);virtual void Slot40(int,int);virtual void Slot44();virtual void Slot48();virtual void Slot4c();virtual void Slot50();virtual void Slot54();virtual void Slot58();virtual void Slot5c();virtual void Slot60();virtual int Slot64(int);virtual void Slot68();virtual void Slot6c();virtual int Slot70(int,int);virtual void Slot74(int,void*);virtual int Slot78();virtual void Slot7c();virtual void Slot80();virtual int Slot84();virtual const char* Slot88();};
struct OwnedWidget {virtual ~OwnedWidget();virtual void Slot0c();virtual void Slot10();virtual void Slot14();virtual void Slot18();virtual int Slot1c();};
struct CallbackPrefix {char prefix[0x254];unsigned char active,unknown255,flag256,flag257;int(*callback)(void*);void* context;};
struct OwnedWidgetPrefix {char prefix[0x244];OwnedWidget* child;int x,y,kind;};
struct ModelResourcePrefix {void* vtable;int resource;};
struct WidgetProperty {void* value;int key,bytes;void* buffer;};
// The factory identifies type/style and two separate integers at +0x88/+0x8c.
// name marks the start of string storage; its capacity is not established here.
struct WidgetDesc {
 u32 type;int flags;int x,y,kind,dx,dy;float scale;
 char style[0x20];char name[1];char unknown41[0x47];
 int field88,field8c;
 int style90,field94,field98,field9c,fielda0,fielda4;
};
// Observed canvas member only; this prefix does not establish the complete widget size.
struct gxCanvas;
struct CanvasWidgetPrefix {char prefix[0x244];gxCanvas* canvas;};
typedef char CanvasWidgetMemberOffset[((unsigned long)&((CanvasWidgetPrefix*)0)->canvas)==0x244?1:-1];
struct IntWatchPrefix {char prefix[0x254];int* value;};
struct FlagTextPrefix {char prefix[0x254];unsigned char* flag;};
struct WidgetFragment {inline int ProjectX(float scale,int index){return left+(int)(scale*(float)index);}inline int Left() const{return left;}inline int Top() const{return top;}
 void* vtable;
 int left,top,right,bottom,kind;
 unsigned char flag18,flag19,flag1a,enabled;
 float fadeIn,fadeOut,speedIn,speedOut;
 void* window;
 WidgetProperty properties[32];
 int propertyCount;unsigned char dirty,pad235[3];u32 group;int style;void* stylePtr;
 int x,y,width,height;
 const char* label;
 u32 unknown258,unknown25c,unknown260;
 int* value;
};
// MATCHING-AID (transitional): the widgets still written as C call UIWidget's members by their linker names;
// each alias goes away when its last caller becomes a real UIWidget subclass (FRAMEWORK_PLAN_2026-11-15.md).
extern "C" {
WidgetFragment* __ct__7UILabelFiiiiii(WidgetFragment*,int,int,int,int,int,int);
void Slot24__7UILabelFPv(WidgetFragment*,void*);
void Slot28__7UILabelFv(WidgetFragment*);
WidgetFragment* __ct__8UIWidgetFiiiii(WidgetFragment*,int,int,int,int,int);
WidgetFragment* __ct__8UIWidgetFP10WidgetDesc(WidgetFragment*,WidgetDesc*);
WidgetFragment* __dt__8UIWidgetFv(WidgetFragment*,int);
void Slot1C__8UIWidgetFii(WidgetFragment*,int,int);
void Slot20__8UIWidgetFii(WidgetFragment*,int,int);
void Slot24__8UIWidgetFPv(WidgetFragment*,void*);
int Slot64__8UIWidgetFi(WidgetFragment*,int);
void Slot7C__8UIWidgetFv(WidgetFragment*);
void Slot80__8UIWidgetFv(WidgetFragment*);
}
// Prefix only. Factory allocation is 0x368 for StaticText and 0x374 for CustomText.
// String storage starts at +0x268; its declared capacity and the unknown fields remain unproved.
typedef char WidgetPrefixSizeCheck[(sizeof(WidgetFragment)==0x268)?1:-1];
extern "C" {
void fn_80227880(float,float,float,float);
void fn_802235B4(WidgetFragment*);void fn_80223644(WidgetFragment*);

extern const float lbl_805634A0,lbl_805634A4,lbl_805634A8,lbl_805634AC,lbl_805634B0,lbl_805634B4;
float fn_80214D48();
int fn_8021525C();void __dla__FPv(void*);
void fn_80218FEC(WidgetFragment*,int,int,int,int,int);
void fn_80219718(WidgetFragment*);
void __dl__FPv(void*);
char* strcpy(char*,const char*);

void fn_80218FEC(WidgetFragment* self,int left,int top,int right,int bottom,int kind);

void fn_80219044(WidgetFragment*);
int memcmp(const void*,const void*,unsigned long);
void* memcpy(void*,const void*,unsigned long);
void* __nwa__FUlPCc(unsigned long,const char*);

void fn_802179A0(void*);void fn_80218198(void*,void*);void fn_80217AC0(void*);
void fn_80219284(WidgetFragment* self,int width,int height);
int fn_80219440(WidgetFragment* self,int key,void* value,int bytes);
void fn_802194F4(WidgetFragment* self);
void fn_802194FC(WidgetFragment* self);
void fn_80219508(WidgetFragment* self);

inline void WidgetMarkDirty(WidgetFragment* self){if(self->flag1a)self->dirty=1;}
inline void WidgetShow(WidgetFragment* self){if(!self->flag1a){self->flag1a=1;WidgetMarkDirty(self);}}
inline void WidgetHide(WidgetFragment* self){if(self->flag1a){WidgetMarkDirty(self);self->flag1a=0;}}
void fn_80219044(WidgetFragment* self);
void fn_80219358(WidgetFragment* self,float duration);
void fn_802193C8(WidgetFragment* self,float duration);


void fn_8022B91C(int,int,int,const char*,u32);
void fn_8022BAD0(int,int,int,int,int,int,const char*,u32,unsigned char,float);
void fn_8022AC1C(int,const char*,int,int,int*,int*,int*,int*);
inline int WidgetMinInt(int value,int limit){if(value<limit)return value;return limit;}
inline int WidgetMaxInt(int value,int limit){if(value>limit)return value;return limit;}
void fn_80219718(WidgetFragment* self);

extern u32 lbl_8033F4B8[];
WidgetFragment* fn_80222340(WidgetFragment*,int,int,int,int,int,OwnedWidget*);
WidgetFragment* fn_802223C4(WidgetFragment*,int);
void fn_80219AB4(WidgetFragment*,int,const char*,int,int,int,bool);
WidgetFragment* fn_8021995C(WidgetFragment* self,int x,int y,int width,OwnedWidget* value,void* watched);
WidgetFragment* fn_802199D0(WidgetFragment* self,int destroy);

int fn_802298DC(WidgetFragment* self);

extern u32 lbl_8033F90C[];
WidgetFragment* fn_80222340(WidgetFragment* self,int x,int y,int kind,int dx,int dy,OwnedWidget* child);
WidgetFragment* fn_802223C4(WidgetFragment* self,int destroy);
void fn_80222450(WidgetFragment* self);
void fn_80222468(WidgetFragment* self,int x,int y);

extern u32 lbl_8033F880[];
// Window completion takes a signed result; callback contexts remain opaque.
void fn_802168BC(int result);
WidgetFragment* fn_802224B4(WidgetFragment* self,void* context,int x,int y,int dx,int dy,OwnedWidget* child,int(*callback)(void*),unsigned char a,unsigned char b);
void fn_80229628(WidgetFragment* self,int,int);
void fn_80229640(WidgetFragment* self,int,int);
int fn_802296C0(WidgetFragment* self);

void fn_80229750(WidgetFragment* self);
void fn_80229790(WidgetFragment* self);
void fn_802297D0(WidgetFragment* self);




extern const char lbl_8055B8A4[3];




}


class AnimationBoundsDispatch {
public:
 virtual void Slot00();virtual void Slot04();virtual void Slot08();
 virtual void Bounds(int,int,int*,int*,int*,int*);
};
extern "C" {





}

extern "C" {
// Opaque method identities only; held bodies are not reconstructed.

}


struct Background;
struct BackgroundWidgetPrefix {
 char base[0x244];Background* active;const char* textureName;float amount;
 unsigned char flag250,flag251,pad252[2];int option;
};
typedef char BackgroundWidgetPrefixSizeCheck[(sizeof(BackgroundWidgetPrefix)==0x258)?1:-1];
extern "C" {

void fn_8022FFE8(Background*);void fn_8022F8CC(Background*);void fn_8022F824(Background*);
}


struct WatchedChildPrefix {char base[0x254];void* watched;int kind;};
extern "C" {
extern u32 lbl_8033F324[],lbl_8033F298[];
unsigned char* fn_80216708();
WidgetFragment* fn_8021A77C(WidgetFragment* self,int x,int y,int kind,void* watched,OwnedWidget* child);
int fn_8021A804(WidgetFragment* self,int key);
WidgetFragment* fn_8021A808(WidgetFragment* self,int x,int y,int kind,void* watched,OwnedWidget* child);
}


struct WindowOffsetPrefix {char prefix[0x454];int x,y;};
struct WidgetWindowOffsetPrefix {char prefix[0x244];int x,y;unsigned char enabled;};
extern "C" {
void fn_8021818C(void*);
void fn_8021A8A8(WidgetFragment* self);
int fn_8021A9A4(WidgetFragment* self);
static inline float InlineWidgetMinimum(float value,float bound){if(value<bound)return value;return bound;}
static inline float InlineWidgetMaximum(float value,float bound){if(value>bound)return value;return bound;}
static inline void InlineOffsetSet(WidgetWindowOffsetPrefix* f,int x,int y){f->x=x;f->enabled=1;f->y=y;}
}


extern "C" {
extern const float lbl_805634E4,lbl_805634E8;
Background* fn_8022F638(const char*,unsigned char,unsigned char,int,float);
void fn_80218968(void*,int,float);void fn_8021889C(void*,int,float);
void fn_8021A910(WidgetFragment* self,int mode,void*);
}


struct TimedAnimationPrefix {
 char base[0x244];unsigned char active,pad245[3];float elapsed,period;void* handle;
};
struct WatchedAnimationPrefix {
 TimedAnimationPrefix base;void* watched;float low,high;int option,mode;
};
typedef char TimedAnimationPrefixSize[(sizeof(TimedAnimationPrefix)==0x254)?1:-1];
extern "C" {
extern u32 lbl_8033F6E0[],lbl_8033F658[];extern const float lbl_805634EC;
WidgetFragment* fn_8021AC24(WidgetFragment* self,int x,int y,void* handle);
int fn_8021ACC0(WidgetFragment* self,void* context);
void fn_8021AE18(WidgetFragment* self);
WidgetFragment* fn_8021AF0C(WidgetFragment* self,int x,int y,void* watched,int option,int mode,void* handle,float low,float high);
}


extern "C" {
extern int lbl_8055FB9C,lbl_8055FBA0;

void fn_8021AD10(WidgetFragment* self);
}


struct IntegerAnimationPrefix {TimedAnimationPrefix base;int* watched;int low,high,step;};
struct ObservedScrollState {int size,visible,offset;};
struct ScrollAnimationPrefix {TimedAnimationPrefix base;int direction;ObservedScrollState* state;};
extern "C" {
extern u32 lbl_8033F5D0[],lbl_8033F548[];extern const float lbl_805634C4;
WidgetFragment* fn_8021AF9C(WidgetFragment* self,int destroy);
void fn_8021AFF4(WidgetFragment* self);
WidgetFragment* fn_8021B0CC(WidgetFragment* self,int x,int y,int* watched,int low,int high,int step,void* handle);
void fn_8021B134(WidgetFragment* self);
WidgetFragment* fn_8021B1C4(WidgetFragment* self,int x,int y,ObservedScrollState* state,int direction,void* handle);
void fn_8021B224(WidgetFragment* self);
}


class SwfAnim {char observedAllocation[0x18];public:SwfAnim();~SwfAnim();void Load(const char*);void Unload();void Draw();void Update();};
class DynoTex {public:void Draw(int,int,int,int,int);};
void* operator new(unsigned long,const char*);
struct SwfWidgetPrefix {char base[0x244];SwfAnim* animation;unsigned char owned;};
struct DynoWidgetPrefix {char base[0x244];DynoTex* texture;int* frame;};
extern "C" {


}


struct ObservedClipRect {int left,top,right,bottom;};
struct ObservedClipPrefix {char prefix[0x14];ObservedClipRect bounds;};
extern "C" {
void fn_80218E98(ObservedClipPrefix* self,ObservedClipRect* out);
void fn_80218EBC(ObservedClipPrefix* self,const ObservedClipRect* in);
void fn_80218EE0(ObservedClipPrefix* self,int left,int top,int right,int bottom);
}


struct ObservedValueProvider {virtual ~ObservedValueProvider();virtual void Read(void**,int*);virtual void Slot10();virtual void Slot14();virtual void Slot18();virtual void Slot1c();virtual void Slot20();virtual float Value();virtual void SetValue(float);};
struct ProviderWidgetPrefix {
 char base[0x244];unsigned char active,pad245[3];ObservedValueProvider* provider;
 void* first;void* second;int width;float elapsed;
};
extern "C" {

}


extern "C" {
}


struct NumericCaptionPrefix {char base[0x254];float scale,bias;int* value;const char* format;char text[1];};
typedef char NumericCaptionPrefixSize[(sizeof(NumericCaptionPrefix)==0x268)?1:-1];
extern "C" {

int sprintf(char*,const char*,...);void fn_80130568(char*);
void fn_8021CF9C(WidgetFragment*);
void fn_8021CF9C(WidgetFragment* self);
}


struct ChoiceWidgetPrefix {char base[0x254];int* value;int count;int first,second,style;char unknown268[0x50];const char** choices;int* focus;int token;};
typedef char ChoiceWidgetPrefixSize[(sizeof(ChoiceWidgetPrefix)==0x2c4)?1:-1];
extern "C" {

int fn_80129BC4(WidgetFragment*);int* fn_80217BD8(void*,int,WidgetFragment*);
}


struct ChoiceWindowPrefix {char unknown[0x494];int depth;};
extern "C" {
void fn_80217D84(void*);void fn_80217C4C(void*);
}


extern "C" {

unsigned int fn_8022D1B4(const char*);
unsigned int strlen(const char*);int fn_80228F40(int,int);void fn_8021D61C(WidgetFragment*);
}


extern "C" {
extern const char lbl_8055B87C[4];
void fn_8021D61C(WidgetFragment* self);
}


extern "C" {
void* fn_8021165C(int,int);

extern const u32 lbl_8055FBC0;
void fn_802C3424(int,int,int,int,int);
}


struct ChoiceProviderData {u32 words[3];};
struct ChoiceProviderDispatch :ChoiceProviderData {
 virtual int Modal();virtual int Selected();virtual int AcceptsKeys();virtual void Slot14();virtual void Slot18();virtual const char* Text();
 virtual void Previous();virtual void Next();virtual int HasPrevious();virtual int HasNext();
};
// Console layout witness: pointer fields are typed; prefix ownership and a 64-bit layout remain open.
struct ProviderChoicePrefix {
 char base[0x254]; // 0x000..0x253: observed prefix, not a recovered base class
 ChoiceProviderDispatch* provider; // 0x254
 int* focus; // 0x258
 int token; // 0x25C
 float previousTime,nextTime; // 0x260, 0x264
 int fontResource; // 0x268: passed to font measurement/text APIs, not a pixel height
};
extern "C" {
float fn_8023659C();void fn_80216D28();void fn_80230E90(int);
}


#include "game/uistyle.h"
struct ChoiceResourcePrefix {char base[0x2c4];int resource;};
extern "C" {
int fn_8022A86C(const UIStyleInputFields*);void fn_8022A8E4(int);
}



// Fixed-record menu prefix only; distinct from StringList. Complete size is unproved.
struct MenuEntryCountPrefix {void* entries;int unknown4,count;};
typedef char MenuEntryCountOffset8[((unsigned long)&((MenuEntryCountPrefix*)0)->count)==8?1:-1];
struct ObservedListViewport {int total,page,start;inline int Total() const{return total;}};
struct RowListPrefix {char base[0x244];StringList* list;ObservedListViewport* view;int unknown24c,columns,rows,rowHeight;int* selected;int direction;float elapsed;unsigned char active;};
static inline void SetListViewport(ObservedListViewport* view,int position){
 view->start=position;int limit=view->total-view->page;if(position>limit)view->start=limit;
 if(view->start<0)view->start=0;
}
static inline void SetListViewportValue(int position,ObservedListViewport* view){SetListViewport(view,position);}
extern "C" {
extern u32 lbl_8033EA90[];extern const char lbl_8055B880[2];
extern const float lbl_80563504,lbl_80563510;
int fn_8022B620(int,const char*);void fn_8021E7D4(WidgetFragment*);
WidgetFragment* fn_8021E0C0(WidgetFragment *self, int x, int y, int width, int height, StringList *list, int *selected, ObservedListViewport *view);
typedef struct RowInputListNew {int unknown0,unknown4,count,version;} RowInputListNew;
typedef struct RowInputViewNew {int total,page,start;} RowInputViewNew;
typedef struct RowInputReceiverNew {void* vtable;int left,top,right,bottom,kind;unsigned char flag18,flag19,shown,enabled;char gap1c[0x218];unsigned char dirty;char gap235[15];RowInputListNew* list;RowInputViewNew* view;int unknown24c,columns,rows,rowHeight;int* selected;int direction;float elapsed;unsigned char active;} RowInputReceiverNew;
typedef char RowInputAddressWordSizeCheckNew[(sizeof(void*)==sizeof(int))?1:-1];
typedef char RowInputReceiverPrefixSizeCheckNew[(sizeof(RowInputReceiverNew)==0x26c)?1:-1];
void fn_8021E1C8(WidgetFragment *source, int unusedX, int y);

void fn_8021E2A0(WidgetFragment* self);
void fn_8021E7D4(WidgetFragment* self);
WidgetFragment* fn_8021E884(WidgetFragment* self,int destroy);
}


extern "C" {
extern const u32 lbl_8055FBDC;

void fn_8021E3BC(WidgetFragment* self,void* context);
void fn_8021E51C(WidgetFragment* self);
}

struct RowInputDispatch {virtual ~RowInputDispatch();virtual void Slot0c();virtual void Slot10();virtual void Slot14();virtual void Slot18();virtual void Slot1c(int,int);virtual void Slot20();virtual void Slot24();virtual void Slot28();virtual void Slot2c();virtual void Slot30();virtual void Slot34();virtual void Slot38();virtual void Slot3c();virtual void Slot40();virtual void Slot44();virtual void Slot48();virtual void Slot4c(int,int);virtual void Slot50();virtual void Slot54();virtual void Slot58();virtual void Slot5c();virtual void Slot60();virtual void Slot64();virtual void Slot68();virtual void Slot6c();virtual void Slot70();virtual void Slot74(int,void*);virtual int Slot78();virtual void Slot7c();virtual void Slot80();virtual int Slot84();virtual const char* Slot88();};

extern "C" {
void fn_80229DC8(WidgetFragment* self,int x,int y);
void fn_80229DE8(WidgetFragment* self,int x,int y);
void fn_80229E1C(WidgetFragment* self,int x,int y);
void fn_80229E34(WidgetFragment* self);
void fn_80229E74(WidgetFragment* self);
void fn_80229EB4(WidgetFragment* self,int,int);
int fn_80229EC4(WidgetFragment* self);
int fn_80229ECC(WidgetFragment* self);
int fn_80228F40(int x,int y);
}

extern "C" {
}
static inline void ObservedNullable_fn_8021915C(WidgetFragment* self){if(self)__dt__8UIWidgetFv(self,0);}
static inline void ObservedNullable_fn_802223C4(WidgetFragment* self){if(self)fn_802223C4(self,0);}
extern "C" {
WidgetFragment* fn_80228FA4(WidgetFragment* self,int destroy);
WidgetFragment* fn_80228FFC(WidgetFragment* self,int destroy);
WidgetFragment* fn_80229054(WidgetFragment* self,int destroy);
WidgetFragment* fn_802290B0(WidgetFragment* self,int destroy);
WidgetFragment* fn_80229168(WidgetFragment* self,int destroy);
WidgetFragment* fn_802292D8(WidgetFragment* self,int destroy);
WidgetFragment* fn_80229330(WidgetFragment* self,int destroy);
WidgetFragment* fn_80229388(WidgetFragment* self,int destroy);
WidgetFragment* fn_802293E0(WidgetFragment* self,int destroy);
WidgetFragment* fn_8022943C(WidgetFragment* self,int destroy);
WidgetFragment* fn_80229498(WidgetFragment* self,int destroy);
WidgetFragment* fn_80229550(WidgetFragment* self,int destroy);

// Forward declarations of functions defined further down (all decompiled; they were empty placeholders
// before 2026-11).
void fn_80219AB4(WidgetFragment*,int,const char*,int,int,int,bool);
}

typedef UIXform ChoicePoseNew;
struct RotatingChoiceFieldsNew {char base[0x368];int resource;unsigned char mode;UIContext* pose;};
struct RotatingChoiceDescriptorNew {u32 first;int flags,x,y,width,height,kind;float scale;char name[0x64];u32 option84,gap88,gap8c,option90,option94,gap98,gap9c,gapa0;float rx,ry,rz;};
extern "C" {extern const float lbl_805634F0,lbl_805634F4,lbl_805634F8;}
static inline float ChoiceDotContractNew(float x,float y,float z,float a,float b,float c){return z*c+(x*a+y*b);}
static inline void ChoiceTransformVectorNew(const RotationMatrix* matrix,const RotationVec* input,RotationVec* out){const float (*m)[3]=matrix->m;if(out==input){float first,second,third;first=ChoiceDotContractNew(input->x,input->y,input->z,m[0][0],m[1][0],m[2][0]);second=ChoiceDotContractNew(input->x,input->y,input->z,m[0][1],m[1][1],m[2][1]);third=ChoiceDotContractNew(input->x,input->y,input->z,m[0][2],m[1][2],m[2][2]);out->x=first;out->y=second;out->z=third;}else{out->x=ChoiceDotContractNew(input->x,input->y,input->z,m[0][0],m[1][0],m[2][0]);out->y=ChoiceDotContractNew(input->x,input->y,input->z,m[0][1],m[1][1],m[2][1]);out->z=ChoiceDotContractNew(input->x,input->y,input->z,m[0][2],m[1][2],m[2][2]);}}

extern "C" {
static inline void ChoiceApplyOffsetNew(ChoicePoseNew* pose,const RotationVec* input){RotationVec translated;ChoiceTransformVectorNew(&pose->basis,input,&translated);pose->translation.v[0]+=translated.x;pose->translation.v[1]+=translated.y;pose->translation.v[2]+=translated.z;}
}

extern "C" {

int fn_8021E620(WidgetFragment* self,int key);
void fn_8021F4F0(WidgetFragment *self);
}
struct SteppedFloatPrefix {char base[0x244];float* value;float minimum,maximum,step;void* image;int* focus;int token;float previousTime,nextTime;};
typedef char SteppedFloatPrefixObservedSize268[sizeof(SteppedFloatPrefix)==0x268?1:-1];
extern "C" {
extern float __float_epsilon[];extern const float lbl_80563508,lbl_8056350C,lbl_805634C8;
void fn_802354A0(void*,int,float,float,float,float,float,u32);
}
static inline float ObservedProviderClamp(float value){float one=(1.0f);float limited=one<value?one:value;float zero=(0.0f);return limited>zero?limited:zero;}
static inline float ObservedArrowOffset(const float* stamp){float now=fn_8023659C();float fraction=PaletteNaturalRemap(0.3f+*stamp-now,0.0f,0.3f);return PaletteNaturalLerp(0.0f,10.0f,fraction);}
static inline u32 ObservedArrowColor(const float* stamp){float now=fn_8023659C();float fraction=PaletteNaturalRemap(now-*stamp,0.0f,0.15f);int alpha=(int)PaletteNaturalLerp(255.0f,0.0f,fraction);return((u32)alpha<<24)|0x00ffffffu;}
extern "C" {

struct DragViewportPrefix {char base[0x244];ObservedListViewport* state;void* first;void* second;bool dragging;char gap251[3];int direction,grabX,grabY;};
extern const u32 lbl_8055FBE0,lbl_8055FBFC;void fn_80228E80(ObservedClipPrefix*,void*,int,int,int,int);void fn_80228DC4(ObservedClipPrefix*,void*,int,int,int,int);
void fn_802E30C8(const char*,...);

void fn_80221F9C(WidgetFragment*,void*);
extern "C" {}
typedef UIContext CircularPoseTypedNew;
typedef struct CircularListTypedNew {int version;void* entries;int count;} CircularListTypedNew;
typedef struct CircularPaintNew {void* vtable;int left,top,right,bottom,kind;char gap18[0x22c];StringList* list;GTextFont* font;unsigned int color;float radius,rate;} CircularPaintNew;


extern "C" {
extern const float lbl_805634D8,lbl_80563528;extern const char lbl_8055B8A8[3];
void fn_80222CFC(WidgetFragment *source);

}

extern "C" {u32 fn_802236C8(WidgetFragment*,int);extern int lbl_8055FB9C,lbl_8055FBA0;}
struct SelectionDrawFieldsNew {char base[0x244];const char* labels[33];int count;int* selected;unsigned char available[33];char pad2f1[3];float widths[33];int icons[33];GTextFont* resource;float spacing,scale,scrollX,first,second,pulse;};
extern "C" {extern const float lbl_805634EC,lbl_805634F4,lbl_80563534;}
static inline float SelectionDrawTopNew(const WidgetFragment* self){return (240.0f)-(float)self->top;}
extern "C" {
// Selection half widths use ordinary load-before-product locals.
}

void fn_8022375C(WidgetFragment*);

struct LargeImage355 {char base[0x244];float x,y,width,height;int frame;const char* name;bool flag;char pad25d[3];void* image;int resource;};
extern u32 lbl_8033E350[];extern int lbl_8055CFF0,lbl_8055CFF4;extern const float lbl_80563538;
int fn_802C9A58();
inline int Round355(float value){return (int)(value<(0.0f)?value-(0.5f):(0.5f)+value);}
WidgetFragment* fn_80223CF0(WidgetFragment* self,WidgetDesc* desc,const char* name);
struct RollingRow;
#include "game/uilists.h"
typedef CycleList PaletteListTypedNew;typedef char CycleListSize28_391[sizeof(CycleList)==28?1:-1];

typedef struct PalettePaintNew {void* vtable;int left,top,right,bottom,kind;unsigned char flag18,flag19,shown,enabled;char gap1c[0x228];PaletteListTypedNew* list;GTextFont* resource;int count;float stamp,x,y,lineHeight,period,upper,lower,elapsed,nextStamp;int direction;RollingRow* rows;int* focus;int token;} PalettePaintNew;
// Observed initialized foreign address view only; no storage or original container ownership.



extern "C" {
extern const float lbl_805634A4,lbl_805634A8,lbl_805634C4,lbl_805634EC,lbl_80563504,lbl_80563508,lbl_80563514,lbl_80563534,lbl_8056354C,lbl_80563550,lbl_80563554,lbl_80563558,lbl_8056355C;
const char* fn_8022DC68(PaletteListTypedNew*,int);
static inline unsigned int PaletteGrayColorObservedNew(int r,int g,int b,int a){return((unsigned int)(unsigned char)a<<24)|((unsigned int)(unsigned char)r<<16)|((unsigned int)(unsigned char)g<<8)|(unsigned char)b;}

}

extern "C" {void fn_8022591C(WidgetFragment*);
static inline double PaletteAbs391(double value){return __fabs(value);}
static inline float PaletteFractionObservedNew(float current,float low,float high){float extent=high-low;float offset=current-low;return(float)PaletteAbs391(offset)/(float)PaletteAbs391(extent);}
}
static inline void PaletteFillCenters391(float* out,float origin,float height){out[0]=origin+(4.0f)*height;out[1]=origin+(3.0f)*height;out[2]=origin+(2.0f)*height;out[3]=origin+height;out[4]=origin;out[5]=origin-height;out[6]=origin-(2.0f)*height;out[7]=origin-(3.0f)*height;out[8]=origin-(4.0f)*height;}



typedef struct TimedImageFields {char base[0x244];float x,y,width,height;int textureHandle;float blendDuration,lastUpdateTime;UIContext* pose;unsigned char blendIncreasing;char unknown265[3];float blend,rotationAngle;unsigned char rotateBackward;} TimedImageFields;
static inline void TransformTimedImageVector(const UIContext* pose,const RotationVec* transformInput,RotationVec* out){ChoiceTransformVectorNew(&pose->xf.basis,transformInput,out);}
extern const float lbl_805634FC,lbl_80563560;
struct TimedImageInputVector:RotationVec{inline TimedImageInputVector(int screenWidth,int screenHeight){z=(0.0f);x=(float)-screenWidth/2.0f;y=(float)-screenHeight/2.0f;} inline void SetZYX(float zValue,float yValue,float xValue){z=zValue;x=xValue;y=yValue;}};typedef char TimedImageInputVectorSize12[sizeof(TimedImageInputVector)==12?1:-1];

void fn_802278F0(gArtSet*,float,float,float,float);
void fn_80228AEC(WidgetFragment*,float);
}
extern "C" {double sin(double);}
struct ModelFitMatrixNew {RotationMatrix basis;RotationVec translation;};
struct ModelFitAssetNew {void* dispatch;int resource;ModelFitMatrixNew matrix;float angle,speed;};
struct ModelAxisRangeNew {
 float minimum, maximum;
 inline float Span() const { return maximum - minimum; }
};
struct ModelFitBoundsNew {
 ModelAxisRangeNew x, y, z;
 inline float RadiusNew() const;
};
extern "C" {ModelFitMatrixNew* fn_802C86DC();void fn_802C77F0(int,ModelFitBoundsNew*);void fn_802C98DC(float*,float*,float*,float*);double sqrt(double);double atan(double);extern const float lbl_80563584;
}
// Rotation angles as a vector built from its three components.
struct RotationVec3New : RotationVec {RotationVec3New(float ax,float ay,float az){x=ax;y=ay;z=az;}};
// Linear interpolation between two bounds (bounds by const reference).
static inline float ModelFitCenterLerpNew(const float& minimum,const float& maximum,float fraction)
{
    float span=maximum-minimum;
    return minimum+span*fraction;
}
static inline float ModelDot3(float ax,float bx,float ay,float by,float az,float bz){return ax*bx+ay*by+az*bz;}
static inline void ModelFitTransformNew(RotationVec* out,const ModelFitMatrixNew* mm,const RotationVec* v){
 const float (*mrow)[3]=mm->basis.m;
 if(out==v){
  float x=ModelDot3(v->x,mrow[0][0],v->y,mrow[1][0],v->z,mrow[2][0]);
  float y=ModelDot3(v->x,mrow[0][1],v->y,mrow[1][1],v->z,mrow[2][1]);
  float z=ModelDot3(v->x,mrow[0][2],v->y,mrow[1][2],v->z,mrow[2][2]);
  out->x=x;out->y=y;out->z=z;
 }else{
  out->x=ModelDot3(v->x,mrow[0][0],v->y,mrow[1][0],v->z,mrow[2][0]);
  out->y=ModelDot3(v->x,mrow[0][1],v->y,mrow[1][1],v->z,mrow[2][1]);
  out->z=ModelDot3(v->x,mrow[0][2],v->y,mrow[1][2],v->z,mrow[2][2]);
 }
}
inline float ModelFitBoundsNew::RadiusNew() const {
 float dy = y.Span();
 float dx = x.Span();
 float dz = z.Span();
 float xx = dx * dx;
 float yy = dy * dy;
 float zz = dz * dz;
 return 0.5f * (float)sqrt(zz + (xx + yy));
}

extern "C" {
}
extern "C" {

// Observed five-word initialized prefix only; complete object size and data ownership remain unresolved.
typedef struct StartupSharedPrefixNew {unsigned long first,kind,third,fourth,fifth;} StartupSharedPrefixNew;
typedef char StartupSharedPrefixSizeCheckNew[(sizeof(StartupSharedPrefixNew)==20)?1:-1];
extern StartupSharedPrefixNew lbl_80490920;extern unsigned long lbl_8055EFB0[2],lbl_8055EFB8;void RegisterModuleTag__FP9ModuleTag(void*);
void fn_8022A594();
}


struct ProviderDrawMetrics {char unknown[0x70];float margin;};
extern "C" {
extern ProviderDrawMetrics* lbl_8055E050;extern float __float_epsilon[];extern const float lbl_80563508,lbl_8056350C,lbl_805634C8;
int fn_8022AB18(int,int,int);float fn_8022AAA8(int,const char*);int fn_8022AA84(int);
void fn_802354A0(void*,int,float,float,float,float,float,u32);
}
static inline float ObservedAbsDecision(float value){return (float)__fabs(value);}
extern "C" {
}

struct EditorDispatch {virtual ~EditorDispatch();virtual void Slot0c();virtual void Slot10();virtual void Slot14();virtual void Slot18();virtual void Slot1c(int,int);virtual void Slot20();virtual void Slot24();virtual void Slot28();virtual void Slot2c();virtual void Slot30();virtual void Slot34();virtual void Slot38();virtual void Slot3c();virtual void Slot40();virtual void Slot44();virtual void Slot48();virtual void Slot4c();virtual void Slot50();virtual void Slot54();virtual void Slot58();virtual void Slot5c();virtual void Slot60();virtual void Slot64(int);virtual void Slot68();virtual void Slot6c();virtual void Slot70();virtual void Slot74(int,void*);virtual int Slot78();virtual void Slot7c();virtual void Slot80();virtual int Slot84();virtual const char* Slot88();};

struct TextEntryPrefix {char base[0x244];int state,capacity,stride,cursor;char unknown254[0x800];char* buffer;int font;void* first;void* second;};
typedef char TextEntryPrefixSize[(sizeof(TextEntryPrefix)==0xa64)?1:-1];
extern "C" {
extern u32 lbl_8033E988[];
void fn_802C2FC8(void*,int,int,int,int);void fn_80228DC4(ObservedClipPrefix*,void*,int,int,int,int);
void fn_8022B390(int,const char*,int,int*,int*);void fn_802C47B8(int,int,int,int,int);void fn_802C3014(void*);
int fn_8022B2D4(int,const char*,int,int);extern const u32 lbl_8055FBF8;
WidgetFragment* fn_8021EA64(WidgetFragment* self,int x,int y,int width,int height,char* buffer,int capacity,int stride,int font,bool clear);
WidgetFragment* fn_8021EB9C(WidgetFragment* self,int destroy);
// Observed text-entry prefix only; no complete allocation or buffer-storage ownership claim.
typedef struct TextPaintReceiverNew TextPaintReceiverNew;
struct TextPaintReceiverNew {void* vtable;int left,top,right,bottom,kind;unsigned char flag18,flag19,shown,enabled;char gap1c[0x228];int state,capacity,stride,cursor;char unknown254[0x800];char* buffer;int font;void* first;void* second;};
void fn_8021EC1C(WidgetFragment *self, void *context);
void fn_8021EFE0(WidgetFragment* self,int x,int y);
void fn_8021F044(WidgetFragment* self,int,void*);
}

struct PointerEventDispatch {virtual ~PointerEventDispatch();virtual void Slot0c();virtual void Slot10();virtual void Slot14();virtual void Slot18();virtual void Slot1c(int,int);virtual void Slot20();virtual void Slot24();virtual void Slot28();virtual void Slot2c();virtual void Slot30();virtual void Slot34();virtual void Slot38();virtual void Slot3c(int,int);virtual void Slot40();virtual void Slot44();virtual void Slot48();virtual void Slot4c();virtual void Slot50();virtual void Slot54();virtual void Slot58();virtual void Slot5c();virtual void Slot60();virtual void Slot64();virtual void Slot68();virtual void Slot6c();virtual void Slot70();virtual void Slot74(int,void*);virtual int Slot78();virtual void Slot7c();virtual void Slot80();virtual int Slot84();virtual const char* Slot88();};

struct LeafByte24c {char base[0x24c];unsigned char value;};
struct LeafStoredValue {char base[0x254];int* destination;int value;};
struct LeafActivity {char base[0x244];unsigned char active;char padding[3];float elapsed;};
struct LeafChildDispatch {virtual ~LeafChildDispatch();virtual void Slot0c();virtual void Slot10();virtual void Slot14();virtual int HitTest(int,int,int,int);};
struct LeafChildPrefix {char base[0x244];LeafChildDispatch* child;int first,second;};
struct LeafResource250 {char base[0x250];void* resource;};
extern "C" {
void fn_802C744C(int,void*);
void fn_80229900(WidgetFragment* self);
void fn_80229918(WidgetFragment* self);
void fn_80229958(WidgetFragment* self);
void fn_80229998(WidgetFragment* self,int,int);
void fn_80229A94(WidgetFragment* self);
void fn_80229AC4(WidgetFragment* self,int,int);
int fn_80229ADC(WidgetFragment*);
int fn_80229AE4(WidgetFragment* self,int x,int y);
void fn_80229CB4(WidgetFragment* self,int,int);
void fn_80229CD8(WidgetFragment* self,int,int);
void fn_80229CF8(WidgetFragment* self);
void fn_80229D1C(WidgetFragment* self);
int fn_80229D3C(WidgetFragment* self,int x,int y);
void fn_80229ED4(WidgetFragment* self);
void fn_80229EF4(WidgetFragment* self);
void* fn_80229F14(WidgetFragment* self);
void fn_80229F1C(WidgetFragment* self);
void fn_80229F38(WidgetFragment* self);
void* fn_80229F58(WidgetFragment* self);
int fn_80229F60(WidgetFragment* self);
}


struct AdapterTarget {virtual ~AdapterTarget();virtual void Name(const char*);virtual int Enter();virtual void Slot14();virtual int Leave();virtual void Draw();virtual int Flags();virtual int State();};
struct AdapterPrefix {char base[0x244];const char* name;char lastName[32];AdapterTarget* target;unsigned char owned;char padding[3];int left,top,right,bottom;};
typedef char AdapterPrefixSize[(sizeof(AdapterPrefix)==0x280)?1:-1];
extern "C" {
void* memset(void*,int,unsigned long);int strcasecmp(const char*,const char*);
void fn_80228AEC(WidgetFragment*,float);
void fn_802C88BC(int);
}


// Measured ABI prefix views for independent remaining widget methods.
struct Bound2a8 {char base[0x2a8];int* focus;int token;};
struct Bound274 {char base[0x274];int* focus;int token;};
struct Bound258 {char base[0x258];int* focus;int token;};
struct Bound25c {char base[0x25c];int* focus;int token;};
struct Bound27c {char base[0x27c];int* focus;int token;};
struct Bound264 {char base[0x258];int(*callback)(void*);void* context;int token;int* focus;};
struct SelectionPrefix {char base[0x2c8];int count;int* selected;unsigned char available[33];char padding2f1[3];float widths[33];int icons[33];GTextFont* resource;float period,scale,speed,first,second,pulse;char gap418[4];unsigned char locked;};
typedef char SelectionPulseOffset[(sizeof(SelectionPrefix)==0x420)?1:-1];
struct PrefixFlag248 {char base[0x248];unsigned char value;};
struct PrefixFlag250 {char base[0x250];unsigned char value;};
struct PrefixFlag258 {char base[0x258];unsigned char value;};
struct RenderPrefix {char base[0x244];float x,y,w,h;void* resource;};
struct TweenPrefix {char base[0x24c];int selected;char gap250[12];float blend;char gap260[4];int* state;};

// RichCaptionPrefix supplies the same text/font fields to the legacy bitmap draw callback.
extern "C" {

void fn_801A7E54(char*,int,Binding*);void fn_802C40B8(gxCanvas*,int,int);void fn_802ECFEC(void*);void fn_802ED038(void*,float,float,float,float);
void fn_802237DC(WidgetFragment*);
int fn_8021AB88(WidgetFragment*);
void fn_8021B3BC(WidgetFragment* self,void* context);
void fn_8021BD48(WidgetFragment* self,void* context);
void fn_8021F42C(WidgetFragment* self,int,void*);
void fn_802208EC(WidgetFragment* self);
void fn_80220AFC(WidgetFragment* self,int x,int y);
void fn_80220B3C(WidgetFragment* self,int x,int y);
void fn_80220E08(WidgetFragment* self,void* context);
void fn_80221E7C(WidgetFragment* self);
void fn_80221EA0(WidgetFragment* self,int key,void* value,int size);
void fn_80222120(WidgetFragment* self,int x,int y);
void fn_802237D0(WidgetFragment* self);
void fn_802237DC(WidgetFragment* self);
int fn_802237E8(WidgetFragment* self);
int fn_802237F0(WidgetFragment* self,int index);
void fn_80224160(WidgetFragment* self);
void fn_8022419C(WidgetFragment* self);
WidgetFragment* fn_802266B0(WidgetFragment* self,int destroy);
void fn_80226708(WidgetFragment* self);
void fn_80226758(WidgetFragment* self,int,int);
void fn_80226768(WidgetFragment* self,int,int);
int fn_802268A0(WidgetFragment* self,int key);
int fn_802299B0(WidgetFragment* self);
void fn_80229B0C(WidgetFragment* self,int dx,int dy);
}


struct FormatTarget {virtual ~FormatTarget();virtual void Slot0c();virtual void Format(char*,int);virtual int Increase();virtual int Decrease();};
struct FormatPrefix {char base[0x254];char text[80];FormatTarget* target;int* focus;int token;};
struct DirectionPrefix {char base[0x254];FormatTarget* target;int direction;};
struct FloatFormatPrefix {char base[0x254];float scale,bias;float* value;const char* format;char text[1];};
struct SelectionColorPrefix {char base[0x2cc];int* selected;unsigned char flags[0x44];char gap314[0x100];float pulse;char gap418[4];unsigned char locked;};
extern "C" {
extern u32 lbl_8033EDC8[];
int sprintf(char*,const char*,...);void fn_8021818C(void*);
void fn_8021BE48(WidgetFragment* self);
void fn_8021C12C(WidgetFragment* self);
void fn_8021C6D0(WidgetFragment* self);
WidgetFragment* fn_8021C97C(WidgetFragment* self,int x,int y,void* handle,int direction,FormatTarget* target);
int fn_8021C9D8(WidgetFragment* self);
u32 fn_802236C8(WidgetFragment* self,int index);
}


struct CaptionBufferPrefix {char base[0x254];const char* label;char gap258[16];char text[1];};
struct OptionalResourcePrefix {char base[0x244];int resource;char gap248[16];void* optional;};
struct BindingFieldPrefix {char base[0x254];char buffer[24];int parameter;Binding* value;};
struct HitAreaPrefix {char base[0x264];int x,y,width,height;};
extern "C" {
extern u32 lbl_8033E8FC[];
int strcmp(const char*,const char*);
int fn_8021AB90(WidgetFragment* self,int key);
WidgetFragment* fn_8021F394(WidgetFragment* self,int x,int y,int parameter,Binding* value,int height);
int fn_8021FBC4(WidgetFragment* self,int x,int y);
}

extern "C" {
int fn_802E4A1C(int);
int fn_8021F06C(WidgetFragment* self,int key);
}


extern "C" {

}

struct TwoCleanupPrefix {char base[0x250];void* image;char gap254[12];UIContext* asset;};
struct ThreeCleanupPrefix {char base[0x280];void* image;UIContext* assetFirst;UIContext* assetSecond;};
struct TimedFocusPrefix {char base[0x24c];float stamp;char gap250[4];int token;int* focus;char gap25c[4];unsigned char active,inactive;char gap262[6];float value,previous;};
extern "C" {
extern u32 lbl_8033E350[];void fn_802ECFF4(void*);
WidgetFragment* fn_80223FAC(WidgetFragment* self,int destroy);
}

struct PairImage248 {char base[0x248];void* first;void* second;};
struct PairImage25c {char base[0x25c];void* first;void* second;};
struct ArtImage244Prefix {char base[0x244];gArtSet* image;};
struct GXAssetImage244Prefix {char base[0x244];void* image;UIContext* asset;};
struct SceneResourcePrefix {char base[0x244];void* model;float x,y,height,width;int frame;UIContext* asset;unsigned char owned;};
struct ResourceHandlePrefix {char base[0x254];int handle;char gap258[8];UIContext* asset;};
struct TripleResourcePrefix {char base[0x250];GTextFont* resource;gArtSet* first;gArtSet* second;};
struct RichCaptionPrefix {char base[0x244];const char* label;char text[256];BitmapFont* renderer;};
struct SliderRangePrefix {char base[0x244];int* value;char gap248[8];int minimum,maximum;unsigned char dragging;};
struct MenuRecord {char base[0x20];void* context;int(*callback)(void*);};
struct MenuRowPrefix {void* handle;char rest[32];};
struct MenuSelectionPrefix {char base[0x244];MenuRowPrefix* rows;unsigned char active;char pad249[3];int row,column;};
extern "C" {
extern u32 lbl_8033E668[],lbl_8033E560[],lbl_8033EFF4[];
void gxForgetTexture(int);void mrModelUnload(int);int fn_8012C33C(const char*);int fn_802C6FC0(const char*,int,int);MenuRecord* fn_8022D97C(void*,int);
WidgetFragment* fn_80220874(WidgetFragment* self,int destroy);
WidgetFragment* fn_80222C8C(WidgetFragment* self,int destroy);
WidgetFragment* fn_80229B78(WidgetFragment* self,int destroy);
void fn_80229BE8(WidgetFragment* self);
void fn_80222178(WidgetFragment* self,int x,int y);
}

struct KeyDetect {char label[24];Binding* binding;};
typedef char KeyDetectSize28[sizeof(KeyDetect)==28?1:-1];
struct DelegatePaintDispatch {virtual ~DelegatePaintDispatch();virtual void Paint(int,int,int,int,int,const char*);};
struct DelegatePaintPrefix {char base[0x244];DelegatePaintDispatch* child;int first,second,third;};
struct ResourceDescriptorPrefix {int zero,token,x,y,kind,width,height;float point;char text[0x64];int extra84,extra88,extra8c,extra90,extra94,extra98,extra9c,extraa0,extraa4;};
extern "C" {
extern KeyDetect lbl_80490934;
void fn_801A7844(int);int fn_801A7C8C(Binding*);void fn_802E47E8();void fn_802E30C8(const char*,...);
int fn_8021F478(int* result);
void fn_80227C9C(gArtSet* self,float x,float y,float width,float height);
void fn_802299FC(WidgetFragment* self);
}

struct SelectNavigationPrefix {char base[0x2c8];int count;int* selected;unsigned char available[1];};
struct SelectWatchedPrefix {char base[0x420];int* watched;};

struct ImageAlphaPrefix {char base[0x244];float x,y;char gap24c[4];void* image;float width,height,blend;UIContext* asset;};
struct RollingRow {float current,previous,target;};
struct RollingRowsCount {char base[8];int count;};
struct RollingRowsPrefix {char base[0x244];RollingRowsCount* list;char gap248[20];float speed,period,upper,lower;char gap26c[12];RollingRow* rows;};
struct SelectionActionDispatch {virtual ~SelectionActionDispatch();virtual void Slot0c();virtual void Slot10();virtual void Slot14();virtual void Slot18();virtual int State();};
struct SelectionActionPrefix {char base[0x244];SelectionActionDispatch* child;char gap248[24];int token;int* focus;};
extern "C" {

void Finish__9MenuStateFi(void*,void* context);
struct SelectBooleanNavigationRelativeNew {char base[0x2c8];int count;int* selected;bool available[33];};
// Observed 33-slot navigation prefix. The local bounds array keeps the original position/count lifetimes.
// The forward byte sum preserves PPC32 operand order; pointer-preserving alternative is retained for port work.
typedef char NavigationNextAddressWordCheckNew[(sizeof(void*)==sizeof(unsigned int))?1:-1];
void fn_802235B4(WidgetFragment* self);
struct NavigationPreviousNew {char prefix[0x2c8];int count;int* selected;unsigned char available[33];};
// Observed navigation prefix only; the forward-wrap byte-address sum retains the original 32-bit operand order.
// A pointer-preserving counterpart differs by one commuted add and is saved for the later PC port.
typedef char NavigationPreviousAddressWordCheckNew[(sizeof(void*)==sizeof(u32))?1:-1];
void fn_80223644(WidgetFragment* self);
void fn_80226570(WidgetFragment* self,int delta);
int fn_802268E8(WidgetFragment* self);
}


struct AssetRotationPrefix {char base[0x25c];UIContext* matrix;unsigned char owned;};
struct AssetFormatPrefix {char base[0x244];int first;GTextFont* resource;char gap24c[13];char text[64];unsigned char owned;char gap29a[2];UIContext* asset;};
extern "C" {
extern const float lbl_805634F8;extern const char lbl_8055B88C[4];extern u32 lbl_8033F7F4[];
void fn_802E317C(const char*,...);
void fn_80224DDC(WidgetFragment*,float,float,float);
void fn_80227044(WidgetFragment*,float,float,float);
WidgetFragment* fn_80226630(WidgetFragment* self,void* context,int x,int y,int dx,int dy,OwnedWidget* child,int(*callback)(void*));
void fn_80227880(float x,float y,float width,float height);

}

struct SourceResourcePrefix {char base[0x244];int kind;union {void* resource;GTextFont* font;};char gap24c[12];const char* name;char cached[32];};
typedef UIContext FullDrawStatePrefix;
typedef char DrawContextSize60_Normal392[sizeof(FullDrawStatePrefix)==60?1:-1];
struct FocusPaintPrefix {
 char base[0x244]; // 0x000..0x243: observed widget prefix
 DelegatePaintDispatch* child; // 0x244
 int first,second,third; // 0x248, 0x24C, 0x250: forwarded paint parameters
 int* focus; // 0x254
 int token; // 0x258
};
struct FullFormatDispatch {virtual ~FullFormatDispatch();virtual void Bind(void**,int*);virtual void Format(char*,int);virtual int Increase();virtual int Decrease();virtual int CanNext();virtual int CanPrevious();virtual float Value();};
struct ExtendedFormatPrefix {char base[0x254];char text[80];FullFormatDispatch* target;int* focus;int token,left,right,lineY;};
extern "C" {
int strncasecmp(const char*,const char*,unsigned long);

}

struct ExtendedLabeledFormatPrefix {char base[0x254];char text[80];FullFormatDispatch* target;int* focus;int token,left,right,lineY,labelLeft,labelRight,font;const char* label;int side;bool showArrows;};
extern "C" {
int fn_8022B498(int,const char*);
}

struct GaugeValuePrefix {char base[0x248];FullFormatDispatch* target;void* background;void* marker;int inset;};
extern "C" {
void fn_802180BC(void*);void fn_802180B0(void*);void fn_802180C8(void*);void fn_802180D4(void*);extern const float lbl_80563500;
}

struct SelectResourcePrefix {char base[0x3fc];GTextFont* resource;};
struct RenderRequestPrefix {char base[0x244];float x,y,w,h;void* resource;const char* name;bool notify;char pad25d[3];int(*callback)(void*);void* context;};
struct ImageDescriptorPrefix {char base[0x244];float x,y;int value;void* image;float width,height,alpha;UIContext* asset;void* context;};
extern "C" {
void* fn_802ECF08(const char*,int);int fn_802ED040(void*);int fn_802ECF90(void*,const char*);
void fn_80224028(WidgetFragment* self);
}

struct ToggleImagePrefix {char base[0x244];int frame,mode;float elapsed;void* context;int token;int* focus;int(*callback)(void*);bool active,pressed;char gap262[2];int speed;float alpha,blend,x,y,width,height;void* image;UIContext* asset;UIContext* scope;};
struct SelectionCallbackPrefix {char base[0x258];int(*callback)(void*);void* context;int token;int* focus;};
extern "C" {

int fn_802267C4(WidgetFragment* self,int key);
void fn_80228E80(ObservedClipPrefix* context,void* image,int x,int y,int height,int);
}

struct SliderFullPrefix {char base[0x244];int* value;void* background;void* marker;int minimum,maximum;bool dragging;char gap259[3];int* focus;int token;};
extern "C" {
extern const float lbl_805634E0;extern u32 lbl_8033E668[];
WidgetFragment* fn_80220778(WidgetFragment* self,int x,int y,int width,int height,int* value,int minimum,int maximum);
int fn_80220948(WidgetFragment* self,int key);
void fn_80220B5C(WidgetFragment* self,void* context);
void fn_80220C54(WidgetFragment* self);
void fn_80228DC4(ObservedClipPrefix* context,void* image,int x,int y,int width,int);
}

struct MultiValuePrefix {char base[0x244];void* value;void* first;void* second;bool dragging;char gap251[3];int direction;};
struct ThreeResourcePrefix {char base[0x244];bool flag;char gap245[3];void* first;void* second;GTextFont* resource;gArtSet* image;gArtSet* overlay;};
extern "C" {

}


struct PageTextPrefix {
 char base[0x244]; // 0x000..0x243: observed widget prefix
 int font; // 0x244: resource handle
 char gap248[8]; // 0x248..0x24F: unknown fields, no fill or ownership claim
 int rowHeight; // 0x250
 UITable* list; // 0x254
 void* image; // 0x258: observed zero initialization; complete resource type open
 int column,iconOffset; // 0x25C, 0x260
};
struct DropdownRowPrefix {RowListPrefix list;int width,height;};
struct FloatSliderPrefix {char base[0x244];bool dragging;char gap245[3];float minimum,maximum;float* value;int rate;bool negative;char gap259[3];void* background;void* marker;int oldX,oldY,oldWidth,oldHeight;int* focus;int token;};
extern "C" {
void fn_802C4F58(int,int,const char*,int);extern const char lbl_8055B878[4],lbl_8055B884[7];extern const float lbl_805634FC;
void fn_8021BA64(WidgetFragment*,int,int,UITable*,int,const char*,int,int,int,int,int,int,int);
void fn_8021E8DC(WidgetFragment* self,void* context);
static inline int InlineWidgetAbsolute(int value){return value<0?-value:value;}
}

extern "C" {
}

struct MenuTablePrefix {MenuRowPrefix rows[8];int count;};
struct RollingActionList {char base[0x14];int(*callback)(void*);unsigned char vertical;};
struct RollingActionPrefix {char base[0x244];RollingActionList* list;char gap248[8];float stamp;char gap254[32];int direction;char gap278[4];int* focus;int token;};
struct CenteredImagePrefix {char base[0x244];void* image;UIContext* asset;float x,y,height,width;int value;};
extern "C" {



}

extern "C" {
}

struct RollingInterpolationPrefix {char base[0x244];RollingRowsCount* list;char gap248[8];float stamp;char gap254[12];float period,upper,lower;char gap26c[8];int direction;RollingRow* rows;};
static inline float InlineWidgetInterpolate(float first,float last,float position,float duration){return first+(position/duration)*(last-first);}
extern "C" {
extern const float lbl_80563548,lbl_8056353C,lbl_80563540;double sin(double);
void fn_8022591C(WidgetFragment* self);
}

struct ScrollbarPrefix {char base[0x244];void* view;void* track;void* marker;bool dragging;char gap251[3];int vertical;};
extern "C" {


}

extern "C" {
}

struct ResourceTextPrefix {char base[0x244];int context;GTextFont* resource;int style,flags;float scale;unsigned char localized;char text[64];bool owned;char gap29a[2];UIContext* asset;};
struct FormattedChoiceDescriptor {int unknown0,font,x,y,kind,width,height;float lineHeight;char name[0x64];int option84,option88,option8c,option90,option94,option98,option9c,optiona0,optiona4;};


extern "C" {
int fn_80129D90(int,int,int);const char* fn_80131234(const char*);
}

struct UnsignedAssetPrefix {char base[0x244];void* context;GTextFont* resource;int mode;float scale,elapsed;};
extern "C" {
extern const float lbl_8056351C;extern u32 lbl_8033E560[];
WidgetFragment* fn_80222BDC(WidgetFragment* self,WidgetDesc* desc,void* context);

}


extern "C" {
}

struct RollingListData {char base[8];int count,index;};
struct RollingListFullPrefix {char base[0x244];RollingListData* list;GTextFont* resource;int count;float stamp,x,y,lineHeight,period,upper,lower,elapsed,nextStamp;int direction;RollingRow* rows;int* focus;int token;};
static inline int InlineIntegerMinimum(int value,int bound){if(value<bound)return value;return bound;}
static inline int InlineIntegerMaximum(int value,int bound){if(value>bound)return value;return bound;}
extern "C" { extern const float lbl_80563544;
}

// The default "def_ui_model" view a ModelWidget builds when its style names no model (class name unproven): spins a
// model about Y and fits it to the widget's rectangle. Every member is inline, so the copies follow the TU's code.
class ModelView : public MenuCustomView {  /* Class name unproven (allocation tag "def_ui_model"); vtable lbl_8033DCF8 */
public:
    ModelView(WidgetDesc* desc) {
        resource=-1;angle=(0.0f);speed=(0.0f);
        if(fn_8012C33C(desc->name))resource=fn_802C6FC0(desc->name,0,0);
        if(desc->field9c)speed=(6.2831855f)/(float)(u32)desc->field9c;
        float* m=(float*)&matrix;
        m[0]=(1.0f);m[1]=(0.0f);m[2]=(0.0f);m[3]=(0.0f);
        m[4]=(1.0f);m[5]=(0.0f);m[6]=(0.0f);m[7]=(0.0f);
        m[8]=(1.0f);m[9]=(0.0f);m[10]=(0.0f);m[11]=(0.0f);
    }
    virtual ~ModelView(void) {if(resource!=-1){mrModelUnload(resource);resource=-1;}}
    virtual void Load(const char* name) {if(resource!=-1){mrModelUnload(resource);resource=-1;}if(fn_8012C33C(name))resource=fn_802C6FC0(name,0,0);}
    virtual void Update(int x, int y, int width, int height, float delta) {

  if (resource != (-1))
  {
    angle += speed * delta;
    matrix = *fn_802C86DC();
    RotationVec center;
    RotationVec3New rotation((0.0f),angle,(0.0f));
    RotationMatrix tmp;
    Mat3FromEuler(&tmp, &rotation);
    Mat3Mul(&matrix.basis, &tmp, &matrix.basis);
    ModelFitBoundsNew bounds;
    fn_802C77F0(resource, &bounds);
    float radius = bounds.RadiusNew();
    float nearPlane;
    float unused;
    float horizontal;
    float vertical;
    fn_802C98DC(&horizontal, &nearPlane, &unused, &vertical);
    float minimum = radius / vertical;
    float sine = (float) sin((float) atan((horizontal * ((float) height)) / ((float) width)));
    float candidate = radius + (lbl_80563584 * nearPlane);
    float distance = radius / sine;
    distance = (distance > candidate) ? (distance) : (candidate);
    minimum = (minimum > distance) ? (minimum) : (distance);
    matrix.translation.z += minimum;
    center.z = ModelFitCenterLerpNew(bounds.z.minimum, bounds.z.maximum, 0.5f);
    center.y = ModelFitCenterLerpNew(bounds.y.minimum, bounds.y.maximum, 0.5f);
    center.x = ModelFitCenterLerpNew(bounds.x.minimum, bounds.x.maximum, 0.5f);
    ModelFitTransformNew(&center, &matrix, &center);
    matrix.translation.x -= center.x;
    matrix.translation.y -= center.y;
    matrix.translation.z -= center.z;
  }
}
    virtual void Draw(void) {if(resource!=-1)fn_802C744C(resource,&matrix);}

    int resource;               // 0x4
    ModelFitMatrixNew matrix;   // 0x8
    float angle, speed;         // 0x38
};
extern "C" {
void* __nw__FUlPCc(unsigned long,const char*);extern const float lbl_805634D8;
}

// Observed timed-image prefix: byte 0x264 is the fade direction, and 0x270 reverses angular motion.
struct TimedImage349 {char base[0x244];float x,y,width,height;int resource;float rate,stamp;UIContext* asset;bool increasing;char pad265[3];float first,second;bool reverse;};
extern "C" {
extern const float lbl_805634C8;
int fn_802CAC28(const char*,int);

}

struct EffectImage351 {char base[0x244];gArtSet* image;float x,y,height,width;int mode;UIContext* matrix;bool owned;};
extern "C" {
extern unsigned char lbl_8055EFBC;extern const char lbl_8055B8A4[3];
}

extern "C" {
extern const float lbl_80563514;
void fn_8021EDA4(WidgetFragment* self);
}

extern "C" {
extern const float lbl_805634EC;
void fn_802278F0(gArtSet* image,float x,float y,float width,float height);
}

struct ResourceInit357 {char base[0x244];int kind;union {void* resource;GTextFont* fontResource;};int argument,style,flags;const char* name;char cache[32];float scale;bool flag;};
extern "C" {

void fn_80219AB4(WidgetFragment* self,int kind,const char* name,int style,int argument,int flags,bool flag);
}

extern "C" {
}

extern "C" {
static inline float AddScroll358(float a,float b){return b+a;}
}

extern "C" {
static inline float SpinAngle359(int rate,float time){return time*(float)rate;}
}

extern "C" {
static inline float RotationScale360(float a,float b){return a*b;}
void fn_80227044(WidgetFragment* self,float x,float y,float z);
}

extern "C" {
void fn_80224DDC(WidgetFragment* self,float x,float y,float z);
}

extern "C" {
static inline float CenteredGaugeY349(int height,int top){return (float)(height/2-top);}
}

extern "C" {extern const float lbl_805634E8;

}

extern "C" {

}

static inline int FocusSelected(FocusPaintPrefix* w){int on=0;if(w->focus&&*w->focus==w->token)on=1;return on;}
extern "C" {
void fn_8021AA18(FocusPaintPrefix* self);
}

extern "C" {
void fn_8021BA64(WidgetFragment* self,int width,int height,UITable* list,int column,const char* font,int flags,int value,int unused,int first,int second,int rowHeight,int third);
}

extern "C" {
}

extern "C" {
}

extern "C" {
typedef unsigned int GroupSearchWordNew;
typedef struct GroupSearchContextNew {char base[0x238];GroupSearchWordNew group;} GroupSearchContextNew;
typedef struct GroupSearchProxyNew {char base[0x14];GroupSearchContextNew* context;} GroupSearchProxyNew;
void fn_802E317C(const char*,...);
int fn_80221EA8(GroupSearchProxyNew* self);

}

// Observed menu prefixes only; no complete object or table-storage ownership claim.
typedef struct MenuHitListNew {int unknown0,unknown4,count,version;} MenuHitListNew;
typedef struct MenuHitEntryNew {MenuHitListNew* list;char label[32];} MenuHitEntryNew;
typedef struct MenuHitTableNew {MenuHitEntryNew entries[8];int count;} MenuHitTableNew;
typedef struct MenuHitWidgetNew {void* vtable;int left,top,right,bottom,kind;unsigned char flag18,flag19,shown,enabled;char gap_1c[0x228];MenuHitTableNew* table;unsigned char active;char gap249[3];int row,column;} MenuHitWidgetNew;

extern "C" {
typedef char MenuHitTablePrefixSizeCheckNew[(sizeof(MenuHitTableNew)==292)?1:-1];
typedef char MenuHitWidgetPrefixSizeCheckNew[(sizeof(MenuHitWidgetNew)==0x254)?1:-1];
static inline int MenuBoundAddNew(int a,int b){return a+b;}
void fn_80222234(MenuHitWidgetNew *self, int x, int y);

}

typedef struct MapViewportReceiverNew MapViewportReceiverNew;typedef struct MapRendererNew MapRendererNew;
typedef struct MapReceiverTableNew {void* prefix[30];int(*Width)(MapViewportReceiverNew*);} MapReceiverTableNew;
typedef struct MapRendererTableNew {void* prefix[5];void(*Draw)(MapRendererNew*,int,int,int,int,float);} MapRendererTableNew;
struct MapRendererNew {MapRendererTableNew* vtable;};
struct MapViewportReceiverNew {MapReceiverTableNew* vtable;int left,top,right,bottom,kind;unsigned char flag18,flag19,shown,enabled;char gap1c[0x24c];MapRendererNew* renderer;unsigned char owned;char gap26d[3];int x,y,width,height;};
struct MapRendererDispatchCppNew {virtual ~MapRendererDispatchCppNew();virtual void Slot0c();virtual void Slot10();virtual void Draw(int,int,int,int,float);};

extern "C" {extern const float lbl_8056357C,lbl_80563580;
void fn_80228AEC(WidgetFragment *source, float opacity);
}

typedef struct ImageFillPaintNew ImageFillPaintNew;typedef struct ImageFillPaintTableNew {void* prefix[30];int(*Width)(ImageFillPaintNew*);} ImageFillPaintTableNew;
struct ImageFillPaintNew {ImageFillPaintTableNew* vtable;int left,top,right,bottom,kind;char gap18[0x22c];int mode;union {void* image;GTextFont* font;};int* watched;int index;unsigned int color;const char* text;char gap25c[0x20];float scale;unsigned char nativeSize;};
typedef struct ImageFillGlobalNew {char gap0[0x70];float padding;} ImageFillGlobalNew;

extern "C" {
extern const float lbl_805634A8,lbl_805634C0,lbl_805634C4,lbl_805634C8,lbl_805634D8,lbl_805634DC,lbl_805634E0;extern const double lbl_805634D0;
float fn_8023659C(void);double fmod(double,double);double sin(double);
void fn_802354A0(void*,int,float,float,float,float,float,unsigned int);
// Shared UIDraw declarations use one consistent source contract.
// Remaining opaque graphics imports are bounded views, not recovered original headers.
}

static inline int ClampToExclusiveMaximum(const int& maximum,int value){
 int limit=maximum-1;
 return value>=limit?limit:value;
}
extern "C" {
void fn_80220A50(WidgetFragment* self,int x,int unused);
}



extern "C" {extern const u32 lbl_8055FBE0,lbl_8055FBFC;

}



extern "C" {
void fn_80218E98(ObservedClipPrefix* self,ObservedClipRect* out){// Read the complete signed rectangle before writing potentially overlapping output.
 int snapshot[4]={0,0,0,0};snapshot[0]=self->bounds.left;snapshot[1]=self->bounds.top;snapshot[2]=self->bounds.right;snapshot[3]=self->bounds.bottom;out->left=snapshot[0];out->top=snapshot[1];out->right=snapshot[2];out->bottom=snapshot[3];}
void fn_80218EBC(ObservedClipPrefix* self,const ObservedClipRect* in){self->bounds=*in;
}
void fn_80218EE0(ObservedClipPrefix* self,int left,int top,int right,int bottom){
 if(self->bounds.left>left)left=self->bounds.left;self->bounds.left=left;
 if(self->bounds.top>top)top=self->bounds.top;self->bounds.top=top;
 if(self->bounds.right<right)right=self->bounds.right;self->bounds.right=right;
 if(self->bounds.bottom<bottom)bottom=self->bounds.bottom;self->bounds.bottom=bottom;
}
UIWidget::UIWidget(int l,int t,int r,int b,int k){
 fn_80218FEC((WidgetFragment*)this,l,t,r,b,k);
 style=fn_8021525C();stylePtr=0;
}
UIWidget::UIWidget(WidgetDesc* desc){
 fn_80218FEC((WidgetFragment*)this,desc->x,desc->y,desc->x+desc->dx,desc->y+desc->dy,desc->kind);
 style=fn_8021525C();stylePtr=0;
}
void fn_80218FEC(WidgetFragment* self,int left,int top,int right,int bottom,int kind){
 self->flag1a=1;self->enabled=1;self->fadeIn=(-1.0f);self->fadeOut=(-1.0f);
 self->speedIn=(1.0f);self->speedOut=(1.0f);self->dirty=1;self->group=0;self->flag18=0;self->propertyCount=0;
 self->left=left;self->right=right;self->top=top;self->bottom=bottom;self->kind=kind;self->flag19=0;self->window=0;
}
void fn_80219044(WidgetFragment* self){
 if(self->flag1a){if(self->fadeIn>=(0.0f)){self->fadeIn-=self->speedIn*fn_80214D48();if(self->fadeIn<=(0.01f)){WidgetHide(self);self->fadeIn=(-1.0f);}}}
 else self->fadeIn=(-1.0f);
 if(!self->flag1a){if(self->fadeOut>=(0.0f)){self->fadeOut-=self->speedOut*fn_80214D48();if(self->fadeOut<=(0.01f)){WidgetShow(self);self->fadeOut=(-1.0f);}}}
 else self->fadeOut=(-1.0f);
}
UIWidget::~UIWidget(void){
 for(int i=0;i<propertyCount;i++)__dla__FPv(properties[i].buffer);
}
void UIWidget::Slot1C(int x,int y){
 int r=x+Slot78();
 int b=y+(bottom-top);left=x;right=r;top=y;bottom=b;
}
void UIWidget::Slot20(int x, int y) {
    WidgetFragment* self = (WidgetFragment*)this;((WidgetDispatch*)self)->Slot1c(self->left+x,self->top+y);}
void fn_80219284(WidgetFragment* self,int width,int height){self->right=self->left+width;self->bottom=self->top+height;}
void UIWidget::Slot24(void*) {}
int UIWidget::Slot64(int) {return 0;}
void UIWidget::Slot7C(void){
 int bytes;void* value;void* previous;for(int i=0;i<propertyCount;i++){
  bytes=properties[i].bytes;previous=properties[i].buffer;value=properties[i].value;
  if(memcmp(value,previous,bytes)!=0){memcpy(previous,value,bytes);Slot74(properties[i].key,properties[i].value);}
 }fn_80219044((WidgetFragment*)this);
}
void UIWidget::Slot80(void) {
    WidgetFragment* self = (WidgetFragment*)this;fn_80219044(self);}
void fn_80219358(WidgetFragment* self,float duration){
 if(duration<(0.1f)){WidgetShow(self);return;}
 if(!self->flag1a&&self->fadeOut<(-0.1f)){self->fadeOut=(1.0f);duration=duration>(0.1f)?duration:(0.1f);self->speedOut=(1.0f)/duration;}
 self->fadeIn=(-1.0f);
}
void fn_802193C8(WidgetFragment* self,float duration){
 if(duration<(0.1f)){WidgetHide(self);return;}
 if(self->flag1a&&self->fadeIn<(-0.1f)){self->fadeIn=(1.0f);duration=duration>(0.1f)?duration:(0.1f);self->speedIn=(1.0f)/duration;}
 self->fadeOut=(-1.0f);
}
int fn_80219440(WidgetFragment* self,int key,void* value,int bytes){
 void* previous=__nwa__FUlPCc(bytes,"WidgetNotify::old_data");
 self->properties[self->propertyCount].key=key;self->properties[self->propertyCount].value=value;
 self->properties[self->propertyCount].bytes=bytes;self->properties[self->propertyCount].buffer=previous;
 memcpy(previous,value,bytes);return self->propertyCount++;
}
// MATCHING-AID: retail keeps this message in .data right after "WidgetNotify::old_data" but no code reads it;
// a never-called static keeps its literal in creation order (the linker strips its code; helper name and body unproven).
static void WidgetNotifyRemoveUnknown(void) { fn_802E317C("Widget: Removing unknown notification"); }

void fn_802194F4(WidgetFragment* self){fn_802179A0(self->window);}
void fn_802194FC(WidgetFragment* self){fn_80218198(self->window,0);}
void fn_80219508(WidgetFragment* self){fn_80217AC0(self->window);}
void UIDragArea::Slot3C(int x, int y) {
 WidgetWindowOffsetPrefix* fields=(WidgetWindowOffsetPrefix*)((WidgetFragment*)this);
 InlineOffsetSet(fields,-x,-y);
}
void UIDragArea::Slot4C(int x, int y) {
 WidgetWindowOffsetPrefix* fields=(WidgetWindowOffsetPrefix*)((WidgetFragment*)this);
 WindowOffsetPrefix* window=(WindowOffsetPrefix*)((WidgetFragment*)this)->window;
 if(fields->enabled){int dx=(int)((unsigned int)fields->x+(unsigned int)x);int dy=(int)((unsigned int)fields->y+(unsigned int)y);int wx=window->x;int wy=window->y;window->x=(int)((unsigned int)dx+(unsigned int)wx);window->y=(int)((unsigned int)wy+(unsigned int)dy);}
}
UILabel::UILabel(int x,int y,int width,int height,int dx,int dy) : UIWidget(x,y,x+dx,y+dy,width) {
 WidgetFragment* self=(WidgetFragment*)this;
 self->x=0;self->y=0;self->width=width;self->height=height;
}
void UILabel::Slot24(void* context) {
 fn_802C3034(reinterpret_cast<gxCanvas*>(context));int frame=((WidgetDispatch*)((WidgetFragment*)this))->Slot84();const char* text=((WidgetDispatch*)((WidgetFragment*)this))->Slot88();
 fn_8022B91C(((WidgetFragment*)this)->height,((WidgetFragment*)this)->x,((WidgetFragment*)this)->y,text,frame);
}
int UILabel::Slot84(void){return 0;}
void UILabel::Slot28(void) {
 const char* text;int frame,height,x,y;
 y=((WidgetFragment*)this)->top;x=((WidgetFragment*)this)->left;height=((WidgetFragment*)this)->bottom-y;frame=((WidgetDispatch*)((WidgetFragment*)this))->Slot84();text=((WidgetDispatch*)((WidgetFragment*)this))->Slot88();int width=((WidgetDispatch*)((WidgetFragment*)this))->Slot78();
 fn_8022BAD0(((WidgetFragment*)this)->height,((WidgetFragment*)this)->x+x,((WidgetFragment*)this)->y+y,((WidgetFragment*)this)->width,width,height,text,frame,0,(1.0f));
}
void fn_80219718(WidgetFragment* self){
 int left,top,right,bottom;const char* text=((WidgetDispatch*)self)->Slot88();
 fn_8022AC1C(self->height,text,self->x+self->left,self->y+self->top,&left,&top,&right,&bottom);
 left=WidgetMinInt(left,self->left);int oldLeft=self->left;right=WidgetMaxInt(right,oldLeft+((WidgetDispatch*)self)->Slot78());
 top=WidgetMinInt(top,self->top);bottom=WidgetMaxInt(bottom,self->bottom);
 self->x+=self->left-left;self->y+=self->top-top;
 ((WidgetDispatch*)self)->Slot1c(left,top);fn_80219284(self,right-left,bottom-top);
}
CheckBox::CheckBox(int x,int y,const char* text,unsigned char* flag,int height) : UILabel(x,y,0,height,0,0) {
 WidgetFragment* self=(WidgetFragment*)this;
 strcpy((char*)self+0x258,text);((FlagTextPrefix*)self)->flag=flag;
 fn_80219440(self,0,flag,1);fn_80219718(self);
}

int CheckBox::Slot64(int key){return UIWidget::Slot64(key);}
WidgetFragment* fn_8021995C(WidgetFragment* self,int x,int y,int width,OwnedWidget* value,void* watched){
 fn_80222340(self,x,y,width,0,0,value);self->vtable=lbl_8033F4B8;
 self->label=(const char*)watched;if(watched)fn_80219440(self,0,watched,4);return self;
}
WidgetFragment* fn_802199D0(WidgetFragment* self,int destroy){if(self){fn_802223C4(self,0);if(destroy>0)__dl__FPv(self);}return self;}
ArtWidget::ArtWidget(WidgetDesc* desc,const char* text,int value) : UIWidget(desc) {WidgetFragment* self=(WidgetFragment*)this;
 fn_80219AB4(self,desc->flags,text,desc->field9c,value,desc->fielda0,desc->fielda4!=0);}
void fn_80219AB4(WidgetFragment* self,int kind,const char* name,int style,int argument,int flags,bool flag){
 ResourceInit357* f=(ResourceInit357*)self;f->argument=argument;f->name=name;f->flags=flags;f->style=style;f->kind=kind;
 const char* source=name;int remaining=32;char* destination=f->cache;while((*destination++=*source++)!=0){if(--remaining<=0){destination[-1]=0;break;}}
 f->flag=flag;f->fontResource=0;f->scale=(1.0f);
 const char* font=0;if(kind!=6&&kind!=3)font=(const char*)argument;
 float sharedScale,sharedWidth;switch(kind){
 case 0:case 1:case 7:break;
 case 4:if(font){f->fontResource=fn_80232578(font);int height=fn_80232B70(f->fontResource);height=height<1?1:height;sharedWidth=(float)((WidgetDispatch*)self)->Slot78()/(float)height;float width=fn_8023293C(f->fontResource,name,(1.0f));width=width>(1.0f)?width:(1.0f);float other=(float)(self->bottom-self->top)/width;f->scale=sharedWidth<other?sharedWidth:other;}break;
 case 2:if(font){f->fontResource=fn_80232578(font);int h=fn_80232B70(f->fontResource);float height=(float)(self->bottom-self->top);h=h<1?1:h;float scale=height/(float)h;float width=fn_8023293C(f->fontResource,name,(1.0f));sharedWidth=(1.0f);if(width>sharedWidth)sharedWidth=width;float other=(float)((WidgetDispatch*)self)->Slot78()/sharedWidth;f->scale=scale<other?scale:other;}break;
 case 9:f->resource=fn_8023512C(name);break;
 case 3:case 6:case 8:f->resource=fn_8023512C(name);if(argument)fn_80219440(self,0,(void*)argument,4);break;
 case 5:if(font){f->fontResource=fn_80232578(font);int h=fn_80232B70(f->fontResource);float height=(float)(self->bottom-self->top);h=h<1?1:h;float scale=height/(float)h;float width=fn_8023293C(f->fontResource,name,(1.0f));sharedWidth=(1.0f);if(width>sharedWidth)sharedWidth=width;float other=(float)((WidgetDispatch*)self)->Slot78()/sharedWidth;f->scale=scale<other?scale:other;}break;
 default:fn_802E317C("ArtWidget: unknown type %d",kind);break;
 }
}
ArtWidget::~ArtWidget(void) {WidgetFragment* self=(WidgetFragment*)this;SourceResourcePrefix* f=(SourceResourcePrefix*)self;switch(f->kind){case 3:case 6:case 8:case 9:if(f->resource)fn_80235380(static_cast<gArtSet*>(f->resource));break;case 2:case 4:case 5:if(f->font)fn_802328D4(f->font);break;}f->resource=0;}
void ArtWidget::Slot7C(void) {Slot7C__8UIWidgetFv(((WidgetFragment*)this));SourceResourcePrefix* f=(SourceResourcePrefix*)((WidgetFragment*)this);switch(f->kind){case 3:case 6:case 8:case 9:if(strncasecmp(f->cached,f->name,31)!=0){if(f->resource)fn_80235380(static_cast<gArtSet*>(f->resource));f->resource=fn_8023512C(f->name);const char* source=f->name;int remaining=32;char* destination=f->cached;while((*destination++=*source++)!=0){if(--remaining<=0){destination[-1]=0;break;}}}break;}}
void ArtWidget::Slot28(void) {ImageFillPaintNew* fields=(ImageFillPaintNew*)((WidgetFragment*)this);
    float x;
    float y;
    int mode;
    GTextFont *shaderImage;
    GTextFont *stretchImage;
    GTextFont *gradientImage;
    int frameCount;
    int solidFlags;
    int pulseFrame;
    int imageFrame;
    int *pulseWatch;
    unsigned int frameSign;
    int *imageWatch;
    int solidHeight;
    gArtSet* borderImage;
    gArtSet* pulseImage;
    gArtSet* image;
    float time;
    float borderWidth;
    float nativeHeight;
    float zero;
    float borderHeight;
    float pulseSpan;
    float extra;
    float borderX;
    float leftCapWidth;

    time = fn_8023659C();
    zero = (0.0f);
    x = zero;
    y = zero;
    fn_80230C30(&x, &y, fields->left, fields->top);
    mode = fields->mode;
    {
        switch (mode) {
        case 0:
        case 1:
            solidFlags = 2;
            if (mode == 0) {
                solidFlags = 1;
            }
            solidHeight = fields->bottom - fields->top;
            fn_80230150( x,fields->color, solidFlags | 8, y - (float) solidHeight, zero, (float) ((WidgetDispatch*)((WidgetFragment*)this))->Slot78(), (float) solidHeight, (float) fields->index, time, (60.0f));
            return;
        case 9:
            borderImage = static_cast<gArtSet*>(fields->image);
            if (borderImage != 0) {
                borderWidth = ((ImageFillGlobalNew*)lbl_8055E050)->padding + (float) ((WidgetDispatch*)((WidgetFragment*)this))->Slot78();
                borderX = x - (borderWidth / 2.0f);
                x = borderX;
                borderHeight = (float) (fields->bottom - fields->top);
                fn_802354A0(borderImage, 0, borderX, y, zero, borderWidth, borderHeight, -1U);
                frameCount = fn_80235FD8(borderImage);
                if (frameCount >= 2) {
                    leftCapWidth = fn_80235EF0(borderImage, 1);
                    fn_802354A0(borderImage, 1, x - leftCapWidth, y, zero, leftCapWidth, borderHeight, -1U);
                }
                if (frameCount >= 3) {
                    fn_802354A0(borderImage, 2, x + borderWidth, y, zero, fn_80235EF0(borderImage, 2), borderHeight, -1U);
                    return;
                }
            }
            break;
        case 8:
            pulseImage = static_cast<gArtSet*>(fields->image);
            if (pulseImage != 0) {
                pulseWatch = fields->watched;
                if (pulseWatch != 0) {
                    pulseFrame = *pulseWatch;
                } else {
                    pulseFrame = fields->index;
                }
                frameSign = (unsigned int) pulseFrame >> 0x1FU;
                if ((((pulseFrame & 1) ^ frameSign) - frameSign) == 0) {
                    pulseSpan = (10.0f);
                    double phase = fmod(fn_8023659C(), 1.0);
                    extra = (pulseSpan * ((0.5f) + ((0.5f) * (float) sin((double) ((6.2831855f) * (float) phase))))) - (pulseSpan / 2.0f);

                    fn_802354A0(pulseImage, pulseFrame, x - (0.5f)*extra, y + (0.5f)*extra, (0.0f), extra + (float) ((WidgetDispatch*)((WidgetFragment*)this))->Slot78(), extra + (float) (fields->bottom - fields->top), -1U);
                    return;
                }
                fn_802354A0(pulseImage, pulseFrame, x, y, (0.0f), (float) ((WidgetDispatch*)((WidgetFragment*)this))->Slot78(), (float) (fields->bottom - fields->top), -1U);
                return;
            }
            break;
        case 3:
        case 6:
            image = static_cast<gArtSet*>(fields->image);
            if (image != 0) {
                imageWatch = fields->watched;
                if (imageWatch != 0) {
                    imageFrame = *imageWatch;
                } else {
                    imageFrame = fields->index;
                }
                if ((int) fields->nativeSize != 0) {
                    nativeHeight = fn_80235F64(image, imageFrame);
                    fn_802354A0(image, imageFrame, x, y, (0.0f), fn_80235EF0(image, imageFrame), nativeHeight, -1U);
                    return;
                }
                fn_802354A0(image, imageFrame, x, y, (0.0f), (float) ((WidgetDispatch*)((WidgetFragment*)this))->Slot78(), (float) (fields->bottom - fields->top), -1U);
                return;
            }
            break;
        case 2:
            shaderImage = fields->font;
            if (shaderImage != 0) {
                fn_802304EC(fields->text, shaderImage, x, y, zero, fields->scale, fields->color, 9, (float) fields->index, time, (60.0f));
                return;
            }
            break;
        case 4:
            stretchImage = fields->font;
            if (stretchImage != 0) {
                fn_80230650(fields->text, stretchImage, x, y, zero, fields->scale, fields->color, 0x41, (float) fields->index, time, (40.0f));
                return;
            }
            break;
        case 5:
            gradientImage = fields->font;
            if (gradientImage != 0) {
                fn_802307AC(fields->text, gradientImage, x, y, zero, fields->scale, fields->color, 9, (float) fields->index, time, (5.0f));
                return;
            }
            break;
        case 7:
            fn_80230924(fields->color, x, y, x + (float) ((WidgetDispatch*)((WidgetFragment*)this))->Slot78(), y - (float) (fields->bottom - fields->top), zero);
            break;
        }
    }
}
BackWidget::BackWidget(const char* textureName,unsigned char a,unsigned char b,int option,float amount) : UIWidget(0,0,0,0,(int)amount) {WidgetFragment* self=(WidgetFragment*)this;
 BackgroundWidgetPrefix* fields=(BackgroundWidgetPrefix*)self;fields->textureName=textureName;fields->amount=amount;fields->flag250=a;fields->flag251=b;fields->option=option;fields->active=0;}
BackWidget::~BackWidget(void) {}
void BackWidget::Slot28(void) {
 BackgroundWidgetPrefix* fields=(BackgroundWidgetPrefix*)((WidgetFragment*)this);
 if(fields->active){fn_8022FFE8(fields->active);fn_8022F8CC(fields->active);}
}
void BackWidget::Slot0C(void) {
 BackgroundWidgetPrefix* fields=(BackgroundWidgetPrefix*)((WidgetFragment*)this);
 fields->flag250=1;fields->flag251=1;fields->amount=(30.0f);fields->option=1;
 fields->active=fn_8022F638(fields->textureName,1,1,1,(30.0f));
}
void BackWidget::Slot10(void) {BackgroundWidgetPrefix* fields=(BackgroundWidgetPrefix*)((WidgetFragment*)this);fn_8022F824(fields->active);fields->active=0;}
WidgetFragment* fn_8021A77C(WidgetFragment* self,int x,int y,int kind,void* watched,OwnedWidget* child){
 fn_80222340(self,x,y,0,0,0,child);self->vtable=lbl_8033F324;
 WatchedChildPrefix* fields=(WatchedChildPrefix*)self;fields->watched=watched;fields->kind=kind;
 if(watched)fn_80219440(self,0,watched,4);return self;
}
int fn_8021A804(WidgetFragment* self,int key){return Slot64__8UIWidgetFi(self,key);}
WidgetFragment* fn_8021A808(WidgetFragment* self,int x,int y,int kind,void* watched,OwnedWidget* child){
 fn_80222340(self,x,y,0,0,0,child);self->vtable=lbl_8033F298;
 WatchedChildPrefix* fields=(WatchedChildPrefix*)self;fields->watched=watched;fields->kind=kind;
 if(watched)fn_80219440(self,0,watched,4);fn_80219440(self,1,fn_80216708(),1);return self;
}
void fn_8021A8A8(WidgetFragment* self){
 ((WidgetDispatch*)self)->Slot74(2,0);fn_8021818C(self->window);
 WatchedChildPrefix* fields=(WatchedChildPrefix*)self;
 if(*(int*)fields->watched==fields->kind)fn_80218198(self->window,self);
}
void fn_8021A910(WidgetFragment* self,int mode,void*){
 WatchedChildPrefix* fields=(WatchedChildPrefix*)self;
 if(*(int*)fields->watched==fields->kind)fn_80218968(self->window,fields->kind,mode==2?(0.0f):(0.3f));
 else fn_8021889C(self->window,fields->kind,mode==2?(0.0f):(0.3f));
 ((WidgetDispatch*)self)->Slot84();
}
int fn_8021A9A4(WidgetFragment* self){
 unsigned char observed=*fn_80216708();int mode=0;if(observed)mode=2;
 WatchedChildPrefix* fields=(WatchedChildPrefix*)self;int selected=0;
 if(fields->watched&&*(int*)fields->watched==fields->kind)selected=1;
 return (selected!=0)|mode;
}
void fn_8021AA18(FocusPaintPrefix* self){FullDrawStatePrefix state;const FullDrawStatePrefix* live=fn_8022EDC4();state.xf=live->xf;state.alpha=live->alpha;state.unk34=live->unk34;state.scale=live->scale;int selected=FocusSelected(self);if(!*fn_80216708()&&!selected)state.alpha*=(0.3f);fn_8022ED00(&state);int height=((WidgetFragment*)self)->bottom-((WidgetFragment*)self)->top;self->child->Paint(self->first,self->second,self->third,((WidgetDispatch*)self)->Slot78(),height,((WidgetDispatch*)self)->Slot88());fn_8022EDB8();}
int fn_8021AB88(WidgetFragment*){return 0;}
int fn_8021AB90(WidgetFragment* self,int key){if(*fn_80216708()){switch(key){case 0x126:fn_80217AC0(self->window);return 1;case 0x128:fn_802179A0(self->window);return 1;}}return Slot64__8UIWidgetFi(self,key);}
WidgetFragment* fn_8021AC24(WidgetFragment* self,int x,int y,void* handle){
 int height=fn_802C5220(handle);int width=fn_802C51C0(handle);
 __ct__8UIWidgetFiiiii(self,x,y,x+width,y+height,0);self->vtable=lbl_8033F6E0;
 TimedAnimationPrefix* fields=(TimedAnimationPrefix*)self;fields->handle=handle;fields->active=0;fields->elapsed=(0.0f);fields->period=(0.3f);return self;
}
int fn_8021ACC0(WidgetFragment* self,void* context){
 fn_802C3034(reinterpret_cast<gxCanvas*>(context));TimedAnimationPrefix* fields=(TimedAnimationPrefix*)self;
 fn_802C51AC(fields->handle);int result=fn_802C51AC(fields->handle);
 if(result>0)result=fn_802C51AC(fields->handle);return result;
}
void fn_8021AD10(WidgetFragment* self){
 TimedAnimationPrefix* fields=(TimedAnimationPrefix*)self;int mode;
 if(self->flag18)mode=2+(fields->active!=0);else mode=0;
 if(fn_802C51AC(fields->handle)==3&&mode==3)mode=1;
 if(fn_802C51AC(fields->handle)>0)mode%=fn_802C51AC(fields->handle);
 int x=self->left-lbl_8055FB9C/2;int y=lbl_8055FBA0/2-self->top;
 fn_802C5310(fields->handle,mode,(float)x,(float)y,(0.0f));
}
void fn_8021AE18(WidgetFragment* self){
 TimedAnimationPrefix* fields=(TimedAnimationPrefix*)self;
 if(fields->active&&self->flag18){
  float increment,one,zero;fields->elapsed-=fn_80214D48();zero=(0.0f);
  if(fields->elapsed<zero){
   one=(1.0f);increment=(2.0f);int limit=10;
   while(fields->elapsed<zero){
    fields->elapsed+=fields->period;((WidgetDispatch*)self)->Slot84();
    fields->period=one/(increment+one/fields->period);
    if(--limit<0)break;
   }
   fields->elapsed=fields->period;
  }
 }else{fields->elapsed=(0.0f);fields->period=(0.3f);}
}
WidgetFragment* fn_8021AF0C(WidgetFragment* self,int x,int y,void* watched,int option,int mode,void* handle,float low,float high){
 fn_8021AC24(self,x,y,handle);self->vtable=lbl_8033F658;
 WatchedAnimationPrefix* fields=(WatchedAnimationPrefix*)self;fields->watched=watched;fields->low=low;fields->high=high;fields->option=option;fields->mode=mode;return self;
}
WidgetFragment* fn_8021AF9C(WidgetFragment* self,int destroy){if(self){__dt__8UIWidgetFv(self,0);if(destroy>0)__dl__FPv(self);}return self;}
void fn_8021AFF4(WidgetFragment* self){
 WatchedAnimationPrefix* fields=(WatchedAnimationPrefix*)self;
 float step=(fields->high-fields->low)/(float)(fields->option-1);
 *(float*)fields->watched+=(float)fields->mode*step;
 *(float*)fields->watched=step*(float)(int)((0.5f)+*(float*)fields->watched/step);
 if(*(float*)fields->watched<fields->low)*(float*)fields->watched=fields->low;
 if(*(float*)fields->watched>fields->high)*(float*)fields->watched=fields->high;
}
WidgetFragment* fn_8021B0CC(WidgetFragment* self,int x,int y,int* watched,int low,int high,int step,void* handle){
 fn_8021AC24(self,x,y,handle);self->vtable=lbl_8033F5D0;
 IntegerAnimationPrefix* fields=(IntegerAnimationPrefix*)self;fields->watched=watched;fields->low=low;fields->high=high;fields->step=step;return self;
}
void fn_8021B134(WidgetFragment* self){
 IntegerAnimationPrefix* fields=(IntegerAnimationPrefix*)self;bool wrap=fields->high-fields->low==2;
 *fields->watched+=fields->step;
 if(*fields->watched>=fields->high){if(wrap)*fields->watched=fields->low;else *fields->watched=fields->high-1;}
 if(*fields->watched<fields->low){if(wrap)*fields->watched=fields->high-1;else *fields->watched=fields->low;}
}
WidgetFragment* fn_8021B1C4(WidgetFragment* self,int x,int y,ObservedScrollState* state,int direction,void* handle){
 fn_8021AC24(self,x,y,handle);self->vtable=lbl_8033F548;
 ScrollAnimationPrefix* fields=(ScrollAnimationPrefix*)self;fields->direction=direction;fields->state=state;return self;
}
void fn_8021B224(WidgetFragment* self){
 ScrollAnimationPrefix* fields=(ScrollAnimationPrefix*)self;
 if(fields->direction==1){
  ObservedScrollState* state=fields->state;state->offset++;
  int maximum=state->size-state->visible;if(state->offset>maximum)state->offset=maximum;
  if(state->offset<0)state->offset=0;
 }else{
  ObservedScrollState* state=fields->state;state->offset--;
  int maximum=state->size-state->visible;if(state->offset>maximum)state->offset=maximum;
  if(state->offset<0)state->offset=0;
 }
}
StaticText::StaticText(int x,int y,const char* label,int height,int* value,int width) : UILabel(x,y,width,height,0,0) {WidgetFragment* self=(WidgetFragment*)this;self->label=label;
 strcpy((char*)self+0x268,label);self->value=value;fn_80219718(self);}
void StaticText::Slot7C(void) {Slot7C__8UIWidgetFv(((WidgetFragment*)this));CaptionBufferPrefix* f=(CaptionBufferPrefix*)((WidgetFragment*)this);const char* label=f->label;if(strcmp(label,f->text)!=0){strcpy(f->text,label);fn_80219718(((WidgetFragment*)this));WidgetMarkDirty(((WidgetFragment*)this));}}
int StaticText::Slot84(void) {int* value=((WidgetFragment*)this)->value;if(value)return *value;return 0;}
void fn_8021B3BC(WidgetFragment* self,void* context){fn_802C3034(reinterpret_cast<gxCanvas*>(context));fn_802C3424(self->x,self->y,self->width,self->height,(int)self->label);}

CustomText::CustomText(RotatingChoiceDescriptorNew* desc,const char* label,int* value) : StaticText(desc->x,desc->y,label,8,value,desc->width) {RotatingChoiceFieldsNew* f=(RotatingChoiceFieldsNew*)((WidgetFragment*)this);f->mode=(u32)desc->flags>>31;
UIStyleInputFields spec={};spec.fontName=desc->name;spec.height=desc->scale;spec.field08=desc->flags;spec.words0C[0]=desc->option90;spec.words0C[1]=desc->option90;spec.words0C[4]=desc->option84;spec.words0C[7]=desc->option94;spec.words0C[8]=desc->option94;spec.field38=desc->option84;
int resource=fn_8022A86C(&spec);f->resource=resource;((WidgetFragment*)this)->height=resource;fn_80219718(((WidgetFragment*)this));
ChoicePoseNew pose;pose.basis.m[0][0]=(1.0f);pose.basis.m[0][1]=(0.0f);pose.basis.m[0][2]=(0.0f);pose.basis.m[1][0]=(0.0f);pose.basis.m[1][1]=(1.0f);pose.basis.m[1][2]=(0.0f);pose.basis.m[2][0]=(0.0f);pose.basis.m[2][1]=(0.0f);pose.basis.m[2][2]=(1.0f);pose.translation.v[0]=(0.0f);pose.translation.v[1]=(0.0f);pose.translation.v[2]=(0.0f);
RotationVec input={(float)desc->x-(320.0f),(240.0f)-(float)desc->y,0.0f};ChoiceApplyOffsetNew(&pose,&input);
float x,y,z;memcpy(&x,&desc->rx,4);memcpy(&y,&desc->ry,4);memcpy(&z,&desc->rz,4);
if((0.0f)==x&&(0.0f)==y&&(0.0f)==z)f->pose=0;else{RotationVec rotation={(0.017453292f)*x,(0.017453292f)*y,(0.017453292f)*z};RotationMatrix matrix;Mat3FromEuler(&matrix,&rotation);Mat3Mul(&pose.basis,&matrix,&pose.basis);input.x=-input.x;input.y=-input.y;ChoiceApplyOffsetNew(&pose,&input);f->pose=fn_8022EDE4(&pose,(1.0f));}}
CustomText::~CustomText(void) {WidgetFragment* self=(WidgetFragment*)this;RotatingChoiceFieldsNew* f=(RotatingChoiceFieldsNew*)self;if(f->pose){fn_8022EE98(f->pose);f->pose=0;}fn_8022A8E4(f->resource);}
void CustomText::Slot28(void) {FullDrawStatePrefix scope;/* The context stack retains this record through drawing and pop. */RotatingChoiceFieldsNew* f=(RotatingChoiceFieldsNew*)((WidgetFragment*)this);if(f->pose){const UIContext* context=fn_8022EDC4();fn_8022EEF8(&scope,f->pose,context);fn_8022ED00(&scope);}Slot28__7UILabelFv(((WidgetFragment*)this));if(f->pose)fn_8022EDB8();}
Column::Column(FormattedChoiceDescriptor* desc, UITable* provider, int column) : UIWidget((WidgetDesc*)desc) { WidgetFragment* self=(WidgetFragment*)this;fn_8021BA64(self,desc->width,desc->height,provider,column,desc->name,desc->font,desc->option90,desc->optiona4,desc->option94,desc->option98,(int)desc->lineHeight,desc->option84);}
void fn_8021BA64(WidgetFragment* self,int width,int height,UITable* list,int column,const char* font,int flags,int value,int unused,int first,int second,int rowHeight,int third){PageTextPrefix* f=(PageTextPrefix*)self;int horizontal=0;f->column=column;f->list=list;f->rowHeight=rowHeight;f->image=0;int vertical=0;if(flags&4)horizontal=width/2;else if(flags&2)horizontal=width;if(flags&0x60)vertical=rowHeight;else if(flags&0x10)vertical=rowHeight/2;UIStyleInputFields spec={};spec.fontName=font;spec.field08=flags;spec.words0C[0]=value;spec.words0C[1]=second;spec.words0C[2]=third;spec.words0C[3]=horizontal;spec.words0C[4]=vertical;spec.words0C[7]=first;spec.words0C[8]=second;spec.words0C[9]=third;spec.words0C[10]=horizontal;spec.field38=vertical;f->font=fn_8022A86C(&spec);f->iconOffset=(fn_8022B620(f->font,"Foo")-23)/2;fn_80219440(self,0,&list->changes,4);list->SetVisible(height/rowHeight);}
Column::~Column(void) {WidgetFragment* self=(WidgetFragment*)this;PageTextPrefix* f=(PageTextPrefix*)self;fn_8022A8E4(f->font);if(f->image)fn_802C519C(f->image);}
void Column::Slot28(void) {PageTextPrefix* f=(PageTextPrefix*)((WidgetFragment*)this);int end;int x=((WidgetFragment*)this)->left;int start=f->list->page*f->list->visible;int y=((WidgetFragment*)this)->top;end=start+f->list->visible;if(f->list->used<end)end=f->list->used;int zero=0;for(int i=start;i<end;i++){int flags=0;const char* label=f->list->Get(i,f->column,&flags);if(flags>2){flags-=3;float px,py;fn_80230C30(&px,&py,x-28,y+f->iconOffset);if(f->image){int frame=flags%fn_802C51AC(f->image);fn_802C5320(f->image,frame,px,py,(0.0f),(25.0f),(25.0f));}}fn_8022BAD0(f->font,x,y,0,0,0,label,flags,zero,(1.0f));y+=f->rowHeight;}}
void fn_8021BD48(WidgetFragment* self,void* context){fn_802C3034(reinterpret_cast<gxCanvas*>(context));RichCaptionPrefix* f=(RichCaptionPrefix*)self;fn_801E4BEC(f->renderer,0,9,self->left,self->top,f->label);}
Numeric::Numeric(int x,int y,float* value,float scale,const char* format,float bias,int height) : UILabel(x,y,0,height,0,0), value(value) {WidgetFragment* self=(WidgetFragment*)this;FloatFormatPrefix* f=(FloatFormatPrefix*)self;f->scale=scale;f->bias=bias;f->format=format;f->text[0]=0;fn_80219440(self,0,value,4);fn_8021BE48(self);}
void fn_8021BE48(WidgetFragment* self){FloatFormatPrefix* f=(FloatFormatPrefix*)self;sprintf(f->text,f->format,*f->value*f->scale+f->bias);fn_80130568(f->text);fn_80219718(self);WidgetMarkDirty(self);}
void Numeric::Slot74(int, void*) {fn_8021BE48(((WidgetFragment*)this));}
SpinCtl::SpinCtl(int x,int y,FullFormatDispatch* target,int height,int left,int right,int lineY) : UILabel(x,y,0,height,0,0), target(target) {WidgetFragment* self=(WidgetFragment*)this;ExtendedFormatPrefix* f=(ExtendedFormatPrefix*)self;f->left=left;f->right=right;f->lineY=lineY;void* buffer;int size;target->Bind(&buffer,&size);if(size>0)fn_80219440(self,0,buffer,size);fn_8021C12C(self);f->focus=0;f->token=fn_80228F40(x,y);}
void SpinCtl::Slot14(void) {if(fn_80129BC4(((WidgetFragment*)this))){Bound2a8* f=(Bound2a8*)((WidgetFragment*)this);f->focus=fn_80217BD8(((WidgetFragment*)this)->window,f->token,((WidgetFragment*)this));fn_80219440(((WidgetFragment*)this),1,f->focus,4);}}
void SpinCtl::Slot28(void) {Slot28__7UILabelFv(((WidgetFragment*)this));ExtendedFormatPrefix* f=(ExtendedFormatPrefix*)((WidgetFragment*)this);if(f->left!=f->right){int selected=0;if(f->focus&&f->token==*f->focus)selected=1;void* left=fn_8021165C(0,2);void* right=fn_8021165C(1,2);float zero=(0.0f);float x=zero,y=zero;fn_80230C30(&x,&y,f->left,f->lineY);if(f->target->CanPrevious())fn_802C5310(left,selected,x,y,zero);fn_80230C30(&x,&y,f->right,f->lineY);if(f->target->CanNext())fn_802C5310(right,selected,x,y,zero);}}
void SpinCtl::Slot74(int, void*) {fn_8021C12C(((WidgetFragment*)this));}
void fn_8021C12C(WidgetFragment* self){FormatPrefix* f=(FormatPrefix*)self;f->target->Format(f->text,80);fn_80219718(self);WidgetMarkDirty(self);}
int SpinCtl::Slot64(int key) {ExtendedFormatPrefix* f=(ExtendedFormatPrefix*)((WidgetFragment*)this);if(!f->focus)return 0;if(f->token!=*f->focus)return 0;float old=f->target->Value();switch(key){case 0x127:f->target->Increase();if(ObservedAbsDecision(old-f->target->Value())<=__float_epsilon[0])fn_80216D28();return 1;case 0x125:f->target->Decrease();if(ObservedAbsDecision(old-f->target->Value())<=__float_epsilon[0])fn_80216D28();return 1;case 0x126:if(((ChoiceWindowPrefix*)((WidgetFragment*)this)->window)->depth>1){fn_80217D84(((WidgetFragment*)this)->window);return 1;}break;case 0x128:if(((ChoiceWindowPrefix*)((WidgetFragment*)this)->window)->depth>1){fn_80217C4C(((WidgetFragment*)this)->window);return 1;}break;}return 0;}
int SpinCtl::Slot84(void) {Bound2a8* f=(Bound2a8*)((WidgetFragment*)this);return !f->focus?0:*f->focus==f->token;}
SpinText::SpinText(int x,int y,FullFormatDispatch* target,int height,int left,int right,int lineY,const char* label,int alignment) : UILabel(x,y,0,height,0,0), target(target) {WidgetFragment* self=(WidgetFragment*)this;ExtendedLabeledFormatPrefix* f=(ExtendedLabeledFormatPrefix*)self;f->left=left;f->right=right;f->lineY=lineY;f->label=label;if(alignment==1)f->side=0;else f->side=1;f->labelLeft=left;f->font=8;int labelEnd=right+fn_802C51C0(fn_8021165C(1,2));f->labelRight=labelEnd-fn_8022B498(f->font,label);f->showArrows=0;void* buffer;int size;target->Bind(&buffer,&size);if(size>0)fn_80219440(self,0,buffer,size);fn_8021C6D0(self);f->focus=0;f->token=fn_80228F40(x,y);}
void SpinText::Slot14(void) {if(fn_80129BC4(((WidgetFragment*)this))){Bound2a8* f=(Bound2a8*)((WidgetFragment*)this);f->focus=fn_80217BD8(((WidgetFragment*)this)->window,f->token,((WidgetFragment*)this));fn_80219440(((WidgetFragment*)this),1,f->focus,4);}}
void SpinText::Slot28(void) {Slot28__7UILabelFv(((WidgetFragment*)this));ExtendedLabeledFormatPrefix* f=(ExtendedLabeledFormatPrefix*)((WidgetFragment*)this);int selected=0;if(f->focus&&*f->focus==f->token)selected=1;unsigned int flags=selected!=0;if(f->side==0&&f->label)fn_8022BAD0(f->font,f->labelLeft,f->lineY-3,0,0,0,f->label,flags,1,(0.0f));else if(f->side==1&&f->label)fn_8022BAD0(f->font,f->labelRight,f->lineY-3,0,0,0,f->label,flags,1,(0.0f));if(f->left!=f->right&&f->showArrows){selected=0;if(f->focus&&f->token==*f->focus)selected=1;void* left=fn_8021165C(0,2);void* right=fn_8021165C(1,2);float x=(0.0f),y=(0.0f);fn_80230C30(&x,&y,f->left,f->lineY);if(f->target->CanPrevious())fn_802C5310(left,selected,x,y,(0.0f));fn_80230C30(&x,&y,f->right,f->lineY);if(f->target->CanNext())fn_802C5310(right,selected,x,y,(0.0f));}}
void SpinText::Slot74(int, void*) {fn_8021C6D0(((WidgetFragment*)this));}
void fn_8021C6D0(WidgetFragment* self){FormatPrefix* f=(FormatPrefix*)self;f->target->Format(f->text,80);fn_80219718(self);WidgetMarkDirty(self);}
int SpinText::Slot64(int key) {ExtendedLabeledFormatPrefix* f=(ExtendedLabeledFormatPrefix*)((WidgetFragment*)this);if(!f->focus)return 0;if(f->token!=*f->focus)return 0;float old=f->target->Value();switch(key){case 0x127:if(f->showArrows){f->target->Increase();if(ObservedAbsDecision(old-f->target->Value())<=__float_epsilon[0])fn_80216D28();return 1;}else if(((ChoiceWindowPrefix*)((WidgetFragment*)this)->window)->depth>1){fn_802180BC(((WidgetFragment*)this)->window);return 1;}break;case 0x125:if(f->showArrows){f->target->Decrease();if(ObservedAbsDecision(old-f->target->Value())<=__float_epsilon[0])fn_80216D28();return 1;}else if(((ChoiceWindowPrefix*)((WidgetFragment*)this)->window)->depth>1){fn_802180B0(((WidgetFragment*)this)->window);return 1;}break;case 0x126:if(!f->showArrows&&((ChoiceWindowPrefix*)((WidgetFragment*)this)->window)->depth>1){fn_802180C8(((WidgetFragment*)this)->window);return 1;}break;case 0x128:if(!f->showArrows&&((ChoiceWindowPrefix*)((WidgetFragment*)this)->window)->depth>1){fn_802180D4(((WidgetFragment*)this)->window);return 1;}break;case 13:f->showArrows=!f->showArrows;return 1;case 27:if(f->showArrows){f->showArrows=0;return 1;}return 0;}return 0;}
int SpinText::Slot84(void) {return 0;}
WidgetFragment* fn_8021C97C(WidgetFragment* self,int x,int y,void* handle,int direction,FormatTarget* target){fn_8021AC24(self,x,y,handle);DirectionPrefix* f=(DirectionPrefix*)self;f->direction=direction;self->vtable=lbl_8033EDC8;f->target=target;return self;}
int fn_8021C9D8(WidgetFragment* self){DirectionPrefix* f=(DirectionPrefix*)self;if(f->direction>0)return f->target->Increase();return f->target->Decrease();}
SpinSldr::SpinSldr(int x, int y, int dx, int dy, ObservedValueProvider* provider) : UIWidget(x,y,x+dx,y+dy,0) {
  WidgetFragment* self=(WidgetFragment*)this;
 ProviderWidgetPrefix* fields=(ProviderWidgetPrefix*)self;fields->provider=provider;fields->active=0;
 void* value;int bytes;provider->Read(&value,&bytes);if(bytes>0)fn_80219440(self,0,value,bytes);
 fields->first=gxGetImage("HGSlider.img");fields->second=gxGetImage("HGTab.img");
 fields->width=fn_802C51C0(fields->second);fields->elapsed=(-1.0f);}
SpinSldr::~SpinSldr(void) {WidgetFragment* self=(WidgetFragment*)this;ProviderWidgetPrefix* fields=(ProviderWidgetPrefix*)self;
  fn_802C519C(fields->first);fn_802C519C(fields->second);}
// Reconstructed unused int-to-float conversion work observed in the original.
// MWCC retains integer conversion setup/stores; the original spelling and purpose are unknown.
void SpinSldr::Slot24(void* context) {
    WidgetFragment* self = (WidgetFragment*)this;fn_802C3034(reinterpret_cast<gxCanvas*>(context));GaugeValuePrefix* f=(GaugeValuePrefix*)self;f->target->Value();float span=(float)(((WidgetDispatch*)self)->Slot78()-f->inset);}
void SpinSldr::Slot28(void) {
    WidgetFragment* self = (WidgetFragment*)this;GaugeValuePrefix* f=(GaugeValuePrefix*)self;float x=(float)(self->left-lbl_8055FB9C/2);float y=CenteredGaugeY349(lbl_8055FBA0,self->top);fn_802C5310(f->background,0,x,y-(8.0f),(0.0f));float value=f->target->Value();int width=((WidgetDispatch*)self)->Slot78();int pixel=(int)(value*(float)(width-f->inset));pixel+=self->left;fn_80230C30(&x,&y,pixel,self->top+3);fn_802C5310(f->marker,0,x,y,(0.0f));}
void SpinSldr::Slot3C(int x, int y) {
 ProviderWidgetPrefix* fields=(ProviderWidgetPrefix*)((WidgetFragment*)this);
 if(!fields->active){fields->active=1;fields->elapsed=fields->provider->Value();}
 int left=((WidgetFragment*)this)->left;
 if(x<left-32||y<((WidgetFragment*)this)->top-32||x>left+((WidgetDispatch*)((WidgetFragment*)this))->Slot78()+32||y>((WidgetFragment*)this)->bottom+32){fields->provider->SetValue(fields->elapsed);}
 else{
  int left=((WidgetFragment*)this)->left,knob=fields->width;int width=((WidgetDispatch*)((WidgetFragment*)this))->Slot78();
  float denominator=(float)(width-knob);float ratio=(float)(x-left-knob/2)/denominator;
  float one=(1.0f);float limited=one<ratio?one:ratio;
  float zero=(0.0f);limited=limited>zero?limited:zero;
  fields->provider->SetValue(limited);
 }
}
void SpinSldr::Slot4C(int x, int y) {
    WidgetFragment* self = (WidgetFragment*)this;if(((ProviderWidgetPrefix*)self)->active)((PointerEventDispatch*)self)->Slot3c(x,y);}
void SpinSldr::Slot40(int x, int y) {
    WidgetFragment* self = (WidgetFragment*)this;((RowInputDispatch*)self)->Slot4c(x,y);((ProviderWidgetPrefix*)self)->active=0;}
IntNumeric::IntNumeric(int x,int y,int* value,float scale,const char* format,float bias,int height) : UILabel(x,y,0,height,0,0) {WidgetFragment* self=(WidgetFragment*)this;
 NumericCaptionPrefix* fields=(NumericCaptionPrefix*)self;fields->value=value;fields->scale=scale;fields->bias=bias;fields->format=format;
 *((char*)self+0x264)=0;fn_80219440(self,0,value,4);fn_8021CF9C(self);}
void fn_8021CF9C(WidgetFragment* self){
 NumericCaptionPrefix* fields=(NumericCaptionPrefix*)self;
 float number=fields->bias+(float)*fields->value*fields->scale;
 sprintf(fields->text,fields->format,(int)((0.5f)+number));
 fn_80130568(fields->text);fn_80219718(self);WidgetMarkDirty(self);
}
void IntNumeric::Slot74(int, void*) {fn_8021CF9C(((WidgetFragment*)this));}
Multi::Multi(int x,int y,int* value,const char* list,int height,int first,int second,int style) : UILabel(x,y,0,height,0,0) {WidgetFragment* self=(WidgetFragment*)this;
 ChoiceWidgetPrefix* fields=(ChoiceWidgetPrefix*)self;
 fields->first=first;fields->second=second;fields->style=style;fields->value=value;fields->count=0;
 fields->choices=(const char**)__nwa__FUlPCc(fn_8022D1B4(list)*4,"multi_vlist");
 while(*list){fields->choices[fields->count++]=list;list+=strlen(list)+1;}
 fn_80219440(self,0,value,4);fn_8021D61C(self);fields->focus=0;fields->token=fn_80228F40(x,y);}
void Multi::Slot14(void) {
 if(fn_80129BC4(((WidgetFragment*)this))){
  ChoiceWidgetPrefix* fields=(ChoiceWidgetPrefix*)((WidgetFragment*)this);
  fields->focus=fn_80217BD8(((WidgetFragment*)this)->window,fields->token,((WidgetFragment*)this));
  fn_80219440(((WidgetFragment*)this),1,fields->focus,4);
 }
}
int Multi::Slot84(void) {
 ChoiceWidgetPrefix* fields=(ChoiceWidgetPrefix*)((WidgetFragment*)this);
 if(!fields->focus)return 0;
 if(((WidgetFragment*)this)->enabled)return *fields->focus==fields->token;
 return 4;
}
int Multi::Slot64(int key) {
 ChoiceWidgetPrefix* fields=(ChoiceWidgetPrefix*)((WidgetFragment*)this);
 if(!fields->focus)return 0;
 if(fields->token!=*fields->focus)return 0;
 switch(key){
 case 0x127:
  if(*fields->value<fields->count-1)++*fields->value;else *fields->value=0;
  return 1;
 case 0x125:
  if(*fields->value>0)--*fields->value;else *fields->value=fields->count-1;
  return 1;
 case 0x126:
  if(((ChoiceWindowPrefix*)((WidgetFragment*)this)->window)->depth>1){fn_80217D84(((WidgetFragment*)this)->window);return 1;}
  break;
 case 0x128:
  if(((ChoiceWindowPrefix*)((WidgetFragment*)this)->window)->depth>1){fn_80217C4C(((WidgetFragment*)this)->window);return 1;}
  break;
 }return 0;
}
Multi::~Multi(void) {WidgetFragment* self=(WidgetFragment*)this;__dla__FPv(((ChoiceWidgetPrefix*)self)->choices);}
void Multi::Slot28(void) {
    float x;
    float y;
    int *focus;
    int selected;
    void *firstImage;
    void *secondImage;
    float zero;
    float firstWidth;
    float secondWidth;
    float scale;
    float firstHeight;
    float secondHeight;

    Slot28__7UILabelFv(((WidgetFragment*)this));ChoiceWidgetPrefix* fields=(ChoiceWidgetPrefix*)((WidgetFragment*)this);
    if ((int) fields->first != (int) fields->second) {
        focus = fields->focus;
        selected = 0;
        if ((focus != 0) && ((int) fields->token == (int) *focus)) {
            selected = 1;
        }
        firstImage = fn_8021165C(0, 2);
        secondImage = fn_8021165C(1, 2);
        scale = fn_8022EDC4()->unk34;
        zero = (0.0f);
        x = zero;
        y = zero;
        fn_80230C30(&x, &y, (int) fields->first, (int) fields->style);
        firstHeight = scale * (float) fn_802C5220(firstImage);
        firstWidth = (float) fn_802C51C0(firstImage);
        fn_802C5320(firstImage, selected, x, y - ((float) fn_802C5220(firstImage) - firstHeight), zero, firstWidth, firstHeight);
        fn_80230C30(&x, &y, (int) fields->second, (int) fields->style);
        secondHeight = scale * (float) fn_802C5220(secondImage);
        secondWidth = (float) fn_802C51C0(secondImage);
        fn_802C5320(secondImage, selected, x, y - ((float) fn_802C5220(secondImage) - secondHeight), zero, secondWidth, secondHeight);
    }
}
void Multi::Slot24(void* context) {
 fn_802C3034(reinterpret_cast<gxCanvas*>(context));ChoiceWidgetPrefix* fields=(ChoiceWidgetPrefix*)((WidgetFragment*)this);
 if(fields->focus&&fields->token==*fields->focus){
  int x=((WidgetFragment*)this)->left,y=((WidgetFragment*)this)->top;int width=((WidgetDispatch*)((WidgetFragment*)this))->Slot78();
  fn_802C3424(x,y,x+width,((WidgetFragment*)this)->bottom,lbl_8055FBC0);
 }
 Slot24__7UILabelFPv(((WidgetFragment*)this),context);
}
void Multi::Slot3C(int, int) {}
void Multi::Slot74(int, void*) {fn_8021D61C(((WidgetFragment*)this));}
void fn_8021D61C(WidgetFragment* self){
 ChoiceWidgetPrefix* fields=(ChoiceWidgetPrefix*)self;int value=*fields->value;
 if(value>=0&&value<fields->count)strcpy((char*)self+0x268,fields->choices[value]);
 else strcpy((char*)self+0x268,"???");
 fn_80219718(self);WidgetMarkDirty(self);
}
CustomMulti::CustomMulti(int x,int y,int* value,const char* list,const char* first,int second,int third,int fourth) : Multi(x,y,value,list,8,0,0,0) {WidgetFragment* self=(WidgetFragment*)this;
 UIStyleInputFields spec={};spec.fontName=first;spec.field08=second;spec.words0C[0]=third;spec.words0C[1]=third;spec.words0C[2]=fourth;
 spec.words0C[7]=third;spec.words0C[8]=third;spec.words0C[9]=fourth;
 int resource=fn_8022A86C(&spec);((ChoiceResourcePrefix*)self)->resource=resource;self->height=resource;
 fn_80219718(self);}
CustomMulti::~CustomMulti(void) {WidgetFragment* self=(WidgetFragment*)this;fn_8022A8E4(((ChoiceResourcePrefix*)self)->resource);}
MutableMulti::MutableMulti(int x,int y,int dx,int dy,int height,ChoiceProviderDispatch* provider) : UILabel(x,y,0,height,dx,dy) {WidgetFragment* self=(WidgetFragment*)this;
 ProviderChoicePrefix* fields=(ProviderChoicePrefix*)self;float time=fn_8023659C()-(0.3f);
 fields->provider=provider;fields->fontResource=height;fields->nextTime=time;fields->previousTime=time;fields->focus=0;
 fields->token=fn_80228F40(x,y);}
const char* MutableMulti::Slot88(void) {return ((ProviderChoicePrefix*)((WidgetFragment*)this))->provider->Text();}
void MutableMulti::Slot14(void) {
 if(fn_80129BC4(((WidgetFragment*)this))){ProviderChoicePrefix* fields=(ProviderChoicePrefix*)((WidgetFragment*)this);
  fields->focus=fn_80217BD8(((WidgetFragment*)this)->window,fields->token,((WidgetFragment*)this));fn_80219440(((WidgetFragment*)this),1,fields->focus,4);
 }
}
int MutableMulti::Slot84(void) {
 ProviderChoicePrefix* fields=(ProviderChoicePrefix*)((WidgetFragment*)this);
 if(!fields->focus)return 0;
 if(((WidgetFragment*)this)->enabled){int status;if(fields->provider->Modal())status=fields->provider->Selected();else status=(*fields->focus==fields->token);return status!=0;}
 return 4;
}
int MutableMulti::Slot64(int key) {
 ProviderChoicePrefix* fields=(ProviderChoicePrefix*)((WidgetFragment*)this);
 if(fields->provider->Modal()){if(!fields->provider->AcceptsKeys())return 0;}
 else{if(!fields->focus)return 0;if(fields->token!=*fields->focus)return 0;}
 switch(key){
 case 0x127:
  if(fields->provider->HasNext()){fields->provider->Next();fields->nextTime=fn_8023659C();}else fn_80216D28();return 1;
 case 0x125:
  if(fields->provider->HasPrevious()){fields->provider->Previous();fields->previousTime=fn_8023659C();}else fn_80216D28();return 1;
 case 0x126:
  if(!fields->provider->Modal()&&((ChoiceWindowPrefix*)((WidgetFragment*)this)->window)->depth>1){fn_80217D84(((WidgetFragment*)this)->window);fn_80230E90(0);return 1;}break;
 case 0x128:
  if(!fields->provider->Modal()&&((ChoiceWindowPrefix*)((WidgetFragment*)this)->window)->depth>1){fn_80217C4C(((WidgetFragment*)this)->window);fn_80230E90(0);return 1;}break;
 }return 0;
}
void MutableMulti::Slot28(void) {
 ProviderChoicePrefix* fields=(ProviderChoicePrefix*)((WidgetFragment*)this);int selected;
 if(fields->provider->Modal())selected=fields->provider->Selected();
 else{selected=0;if(fields->focus&&fields->token==*fields->focus)selected=1;}
 float x=(0.0f),y=(0.0f);fn_80230C30(&x,&y,((WidgetFragment*)this)->left,((WidgetFragment*)this)->top);
 float available=(float)((WidgetDispatch*)((WidgetFragment*)this))->Slot78()+lbl_8055E050->margin;
// Reconstructed unused int-to-float conversion work observed in the original.
// MWCC retains integer conversion setup/stores; the original spelling and purpose are unknown.
 float unusedHeight=(float)(((WidgetFragment*)this)->bottom-((WidgetFragment*)this)->top);
 Slot28__7UILabelFv(((WidgetFragment*)this));
 int previousFrame;if(selected){float now=fn_8023659C();if((0.3f)+fields->previousTime>=now)previousFrame=2;else previousFrame=1;}else previousFrame=0;
 int nextFrame;if(selected){float now=fn_8023659C();if((0.3f)+fields->nextTime>=now)nextFrame=2;else nextFrame=1;}else nextFrame=0;
 gArtSet* previousArrow=static_cast<gArtSet*>(fn_80211660(0));gArtSet* nextArrow=static_cast<gArtSet*>(fn_80211660(1));
 float previousOffset=ObservedArrowOffset(&fields->previousTime),nextOffset=ObservedArrowOffset(&fields->nextTime);
 float baseX=x+(float)fn_8022AB18(fields->fontResource,0,3);
 fn_8022AAA8(fields->fontResource,fields->provider->Text());
 y-=(float)fn_8022AB18(fields->fontResource,1,3);
 float previousX=baseX-fn_80235EF0(previousArrow,previousFrame)-previousOffset;float nextX=baseX;
 nextX+=nextOffset;
 u32 align=fn_8022AA84(fields->fontResource);
 if(align&4){float half=available/2.0f;previousX-=half;nextX+=half;}
 else{if(align&2)previousX-=available;if(!(align&2))nextX+=available;}
 u32 previousColor=0xffffffffu,nextColor=0xffffffffu;
 if(!fields->provider||!fields->provider->HasPrevious())previousColor=ObservedArrowColor(&fields->previousTime);
 if(!fields->provider||!fields->provider->HasNext())nextColor=ObservedArrowColor(&fields->nextTime);
 if((!fields->provider||fields->provider->HasPrevious())||ObservedAbsDecision(previousOffset)>__float_epsilon[0]){
  if(previousArrow){float width=fn_80235EF0(previousArrow,previousFrame);float height=fn_80235F64(previousArrow,previousFrame);
   fn_802354A0(previousArrow,previousFrame,previousX,y+height/2.0f,(0.0f),width,height,previousColor);}
 }
 if((!fields->provider||fields->provider->HasNext())||ObservedAbsDecision(nextOffset)>__float_epsilon[0]){
  if(nextArrow){float width=fn_80235EF0(nextArrow,nextFrame);float height=fn_80235F64(nextArrow,nextFrame);
   fn_802354A0(nextArrow,nextFrame,nextX,y+height/2.0f,(0.0f),width,height,nextColor);}
 }
}
WidgetFragment* fn_8021E0C0(WidgetFragment *self, int x, int y, int width, int height, StringList *list, int *selected, ObservedListViewport *view)
{
  __ct__8UIWidgetFiiiii(self, x, y, x + width, y + height, 0);
  self->vtable = lbl_8033EA90;
  RowListPrefix *fields = (RowListPrefix *) self;
  fields->active = 0;
  fields->direction = 0;
  fields->elapsed = (0.0f);
  int rowHeight = fn_8022B620(7, "A") + 2;
  fields->columns = width / 8;
  fields->list = list;
  fields->rowHeight = rowHeight;
  fields->selected = selected;
  fields->view = view;
  fields->rows = height / rowHeight;
  if (view)
  {
    view->total = list->used;
    view->page = fields->rows;
    fn_80219440(self, 0, view, 12);
    fn_8021E7D4(self);
  }
  fn_80219440(self, 0, selected, 4);
  fn_80219440(self, 0, &list->added, 4);
  return self;
}
void fn_8021E1C8(WidgetFragment *source, int unusedX, int y) {RowInputReceiverNew* fields=(RowInputReceiverNew*)source;
    RowInputViewNew *initialView;
    int page;
    int scratch;
    int row;
    int start;

    fields->direction = 0;
    if ((int) fields->active != 0) {
        if ((int) fields->shown != 0) {
            fields->dirty = 1;
        }
        initialView = fields->view;
        start = 0;
        if (initialView != 0) {
            start = initialView->start;
        }
        scratch = fields->top;
        scratch = y - scratch;
        scratch -= 2;
        row = start + (scratch / (int) fields->rowHeight);
        if (row < 0) {
            row = 0;
        }
        scratch = fields->list->count;
        if (row >= scratch) {
            row = scratch - 1;
        }
        if (initialView != 0) {
            scratch = initialView->start;
            if (row < scratch) {
                fields->direction = -1;
                row = scratch;
            }
            // Preserve the original 32-bit address/integer register lifetime.
            // The typed pointer alternative is retained in the research review for a movable PC build.
            scratch = (int) fields->view;
            page = ((RowInputViewNew*)scratch)->page;
            scratch = ((RowInputViewNew*)scratch)->start;
            scratch += page;
            if (row >= scratch) {
                fields->direction = 1;
                row = scratch - 1;
            }
        }
        if (row >= 0) {
            RowInputListNew* currentList=fields->list;
            if (row < currentList->count) {
                *fields->selected = row;
            }
        }
    }
}
void fn_8021E2A0(WidgetFragment* self){
 Slot7C__8UIWidgetFv(self);RowListPrefix* fields=(RowListPrefix*)self;
 if(fields->view&&fields->direction){
  fields->elapsed+=fn_80214D48();float elapsed=fields->elapsed;
  if(elapsed<(0.0f)||elapsed>=(0.15f)){
   float remaining=fields->elapsed-(0.15f);float cap=(-0.15f);
   fields->elapsed=cap>remaining?cap:remaining;
   SetListViewport(fields->view,fields->view->start+fields->direction);
   if(fields->direction<0)*fields->selected=fields->view->start;
   else *fields->selected=fields->view->start+fields->view->page-1;
  }
 }else fields->elapsed=(0.0f);
}
void fn_8021E3BC(WidgetFragment* self,void* context){
 int drawY;int drawX;int color,index;RowListPrefix* fields=(RowListPrefix*)self;int start=0;if(fields->view)start=fields->view->start;
 Slot24__8UIWidgetFPv(self,context);fn_802C3034(reinterpret_cast<gxCanvas*>(context));int flags=0;color=lbl_8055FBDC;
 for(int row=0;row<fields->rows&&(index=row+start,fields->list->Get(index,&flags)!=0);++row){
  int x,y;bool selected=*fields->selected==index;
  if(selected){x=self->left;y=self->top;int width=x+((WidgetDispatch*)self)->Slot78();
   {int h=fields->rowHeight;int bottom=(row+1)*h;int top=row*h;fn_802C3424(x+2,y+top+2,width-2,y+bottom+2,color);}
  }
  int disabled=flags!=0?2:0;selected=selected!=0;drawY=self->top;drawX=self->left;const char* label=fields->list->Get(index,0);fn_8022B91C(7,drawX+2,drawY+row*fields->rowHeight+2,label,selected|disabled);
 }
}
void fn_8021E51C(WidgetFragment* self){
 int y;int x;RowListPrefix* f=(RowListPrefix*)self;int start=0;if(f->view)start=f->view->start;int flags=0;int index;
 for(int row=0;row<f->rows&&(index=row+start,f->list->Get(index,&flags)!=0);++row){
 int disabled;bool selected;y=self->top;x=self->left;selected=*f->selected==index;disabled=flags!=0?2:0;selected=selected!=0;
 const char* label=f->list->Get(index,0);
 fn_8022BAD0(7,x+2,y+row*f->rowHeight+2,0,0,0,label,selected|disabled,0,(1.0f));
 }
}
int fn_8021E620(WidgetFragment* self,int key){
 RowListPrefix* f=(RowListPrefix*)self;
 switch(key){
 case 0x126:if(*f->selected>0)--*f->selected;fn_8021E7D4(self);return 1;
 case 0x128:if(*f->selected+1<f->list->used)++*f->selected;fn_8021E7D4(self);return 1;
 case 0x122:{int last=f->list->used-1;int next=*f->selected+f->view->page;*f->selected=last<next?last:next;
 SetListViewport(f->view,f->view->start+f->view->page);
 fn_8021E7D4(self);return 1;}
 case 0x121:{int next=*f->selected-f->view->page;*f->selected=next<0?0:next;
 SetListViewport(f->view,f->view->start-f->view->page);
 fn_8021E7D4(self);return 1;}
 case 0x124:*f->selected=0;fn_8021E7D4(self);return 1;
 case 0x123:*f->selected=f->list->used-1;fn_8021E7D4(self);return 1;
 }return 0;
}
void fn_8021E7D4(WidgetFragment* self){
 RowListPrefix* fields=(RowListPrefix*)self;if(!fields->view)return;
 if(*fields->selected>fields->view->start+fields->view->page-1){
  SetListViewport(fields->view,*fields->selected-fields->view->page+1);
 }
 if(*fields->selected<fields->view->start){SetListViewportValue(*fields->selected,fields->view);}
}
WidgetFragment* fn_8021E884(WidgetFragment* self,int destroy){
 if(self){__dt__8UIWidgetFv(self,0);if(destroy>0)__dl__FPv(self);}return self;
}
void fn_8021E8DC(WidgetFragment* self,void* context){DropdownRowPrefix* f=(DropdownRowPrefix*)self;if(f->list.active){((WidgetDispatch*)self)->Slot1c(self->left,self->bottom+3);int height=self->bottom-self->top;int width=((WidgetDispatch*)self)->Slot78();fn_80219284(self,f->width,f->height);fn_8021E3BC(self,context);fn_80219284(self,width,height);((WidgetDispatch*)self)->Slot1c(self->left,self->top*2-3-self->bottom);}fn_802C3034(reinterpret_cast<gxCanvas*>(context));Slot24__8UIWidgetFPv(self,context);int y,x;x=self->left;y=self->top;int width=((WidgetDispatch*)self)->Slot78();fn_802C3424(x,y,x+width,self->bottom,lbl_8055FBDC);const char* label=f->list.list->Get(*f->list.selected,0);char buffer[80];sprintf(buffer,"%-*s `",f->list.columns,label?label:"");fn_802C4F58(self->left+1,self->top+1,buffer,lbl_8055FBC0);}
WidgetFragment* fn_8021EA64(WidgetFragment* self,int x,int y,int width,int height,char* buffer,int capacity,int stride,int font,bool clear){
 __ct__8UIWidgetFiiiii(self,x,y,x+width,y+height+3,0);self->vtable=lbl_8033E988;
 if(clear)buffer[0]=0;TextEntryPrefix* fields=(TextEntryPrefix*)self;
 fields->buffer=buffer;fields->font=font;fields->capacity=capacity<0?-capacity:capacity;fields->stride=stride;
 if(stride==1&&capacity>0)fields->second=gxGetImage("slider.img");else fields->second=0;
 fields->state=0;fields->first=gxGetImage("ibeam.img");fields->cursor=strlen(buffer);
 fn_80219440(self,0,buffer,fields->capacity*fields->stride);
 if(fields->second){int height=fn_802C5220(fields->second);int width=((WidgetDispatch*)self)->Slot78();fn_80219284(self,width,height);}
 return self;
}
// MATCHING-AID: retail keeps this message in .data right after "ibeam.img" but no code reads it;
// a never-called static keeps its literal in creation order (the linker strips its code; helper name and body unproven).
static void WidgetGetLineBadRow(int row) { fn_802E317C("GetLine: bad row %d", row); }

WidgetFragment* fn_8021EB9C(WidgetFragment* self,int destroy){
 if(self){self->vtable=lbl_8033E988;TextEntryPrefix* fields=(TextEntryPrefix*)self;
  fn_802C519C(fields->first);if(fields->second)fn_802C519C(fields->second);__dt__8UIWidgetFv(self,0);if(destroy>0)__dl__FPv(self);
 }return self;
}
void fn_8021EC1C(WidgetFragment *self, void *context) {TextPaintReceiverNew* fields=(TextPaintReceiverNew*)self;
    int cursorX;
    int cursorY;
    int caretY;
    int caretX;
    int initialTop;
    int imageTop;
    int finalTop;
    int initialLeft;
    int imageLeft;
    int finalLeft;

    fn_802C3034(reinterpret_cast<gxCanvas*>(context));
    initialLeft = fields->left;
    initialTop = fields->top;
    fn_802C2FC8(context, initialLeft, initialTop, initialLeft + ((WidgetDispatch*)self)->Slot78(), fields->bottom);
    Slot24__8UIWidgetFPv(self, context);
    if ((void *) fields->second != 0) {
        imageTop = fields->top;
        imageLeft = fields->left;
        fn_80228DC4((ObservedClipPrefix*)context, fields->second, imageLeft, imageTop, ((WidgetDispatch*)self)->Slot78(), 0);
    }
    finalLeft = fields->left;
    finalTop = fields->top;
    fn_802C2FC8(context, finalLeft, finalTop, finalLeft + ((WidgetDispatch*)self)->Slot78(), fields->bottom);
    fn_8022B91C(fields->font, fields->left, fields->top, fields->buffer, fields->flag19 != 0);
    if ((int) fields->flag19 != 0) {
        fn_8022B390(fields->font, fields->buffer, fields->cursor, &cursorX, &cursorY);
        caretY = cursorY + fields->top;
        caretX = cursorX + fields->left;
        fn_802C47B8(caretX, caretY + 2, caretX, caretY + 0xC, lbl_8055FBF8);
    }
    fn_802C3014(context);
}
void fn_8021EDA4(WidgetFragment* self){
 TextEntryPrefix* f=(TextEntryPrefix*)self;float height;
 if(f->second){
  float x=(float)(self->left-lbl_8055FB9C/2);float y=(float)(lbl_8055FBA0/2-self->top);
  height=(float)fn_802C5220(f->second);float width=(float)((WidgetDispatch*)self)->Slot78();
  fn_802C5320(f->second,0,x,y,(0.0f),width,height);
 }
 fn_8022BAD0(f->font,self->left,self->top,0,0,0,f->buffer,self->flag19!=0,0,(1.0f));
 if(self->flag19){
  float x=(float)(self->left-lbl_8055FB9C/2);float y=(float)(lbl_8055FBA0/2-self->top);
  char buffer[256];strcpy(buffer,f->buffer);if(f->cursor>=0)buffer[f->cursor]=0;
  x+=(float)fn_8022B498(f->font,buffer);height=(float)fn_8022B620(f->font,buffer);
  fn_80230924(0xffffff00u,x,y-(3.0f),(1.0f)+x,(3.0f)+(y-(float)height),(0.0f));
 }
}
void fn_8021EFE0(WidgetFragment* self,int x,int y){
 TextEntryPrefix* fields=(TextEntryPrefix*)self;fields->cursor=fn_8022B2D4(fields->font,fields->buffer,x-self->left,y-self->top);WidgetMarkDirty(self);
}
void fn_8021F044(WidgetFragment* self,int,void*){WidgetMarkDirty(self);((EditorDispatch*)self)->Slot64(0);}
int fn_8021F06C(WidgetFragment* self,int key){
 TextEntryPrefix* f=(TextEntryPrefix*)self;char* text=f->buffer;
 if(f->cursor>(int)strlen(text))f->cursor=strlen(text);
 int capacity=f->capacity*f->stride;
 if(!self->flag19)return 0;
 WidgetMarkDirty(self);
 if((unsigned int)key>=0x160&&(unsigned int)key<=0x169)key=(unsigned short)(key+0xfed0);
 switch(key){
 case 0x124:f->cursor=0;WidgetMarkDirty(self);break;
 case 0x123:f->cursor=strlen(f->buffer);WidgetMarkDirty(self);break;
 case 0x125:if(f->cursor>0){f->cursor--;WidgetMarkDirty(self);}break;
 case 0x127:if((unsigned int)f->cursor<strlen(f->buffer)){f->cursor++;WidgetMarkDirty(self);}break;
  case 0x12e:{text=f->buffer;if(strlen(text)!=0){int index=f->cursor;if((unsigned int)index<strlen(text)){while(f->buffer[index]){f->buffer[index]=f->buffer[index+1];index++;}}}WidgetMarkDirty(self);break;}
 case 8:if(strlen(f->buffer)!=0&&f->cursor>0){int i=--f->cursor;while(f->buffer[i]){f->buffer[i]=f->buffer[i+1];i++;}}WidgetMarkDirty(self);break;
 case 9:if(!fn_802E4A1C(0xfd))fn_802194F4(self);else fn_80219508(self);break;
 default:if(!(key&0x100)&&(unsigned int)key>31){text=f->buffer;if(strlen(text)<(unsigned int)(capacity-1)){int i=strlen(text)+1;while(i>f->cursor){f->buffer[i]=f->buffer[i-1];i--;}f->buffer[f->cursor++]=(char)key;}WidgetMarkDirty(self);}else return Slot64__8UIWidgetFi(self,key);break;
 }
 return 1;
}
WidgetFragment* fn_8021F394(WidgetFragment* self,int x,int y,int parameter,Binding* value,int height){__ct__7UILabelFiiiiii(self,x,y,0,height,0,0);BindingFieldPrefix* f=(BindingFieldPrefix*)self;f->parameter=parameter;self->vtable=lbl_8033E8FC;f->value=value;fn_801A7E54(f->buffer,24,value);fn_80219440(self,0,value,16);fn_80219718(self);return self;}
void fn_8021F42C(WidgetFragment* self,int,void*){fn_801A7E54(((BindingFieldPrefix*)self)->buffer,24,((BindingFieldPrefix*)self)->value);WidgetMarkDirty(self);}
int fn_8021F478(int* result){fn_801A7844(1);KeyDetect* file=&lbl_80490934;if(fn_801A7C8C(file->binding)){fn_801A7E54(file->label,24,file->binding);fn_802E47E8();*result=-2;return 1;}return 0;}
struct CaptionContext {int value;inline CaptionContext(int v=0):value(v){}};
struct WidgetMenuEntry {
 const char* name;int type;const void* data;int arg0,arg1;
 inline WidgetMenuEntry(const char* n,const char* t,int context=0){data=t;arg0=context;name=n;type=3;arg1=0;}
 inline WidgetMenuEntry(const char* n,int t,const void* d,int a0,int a1){type=t;name=n;data=d;arg0=a0;arg1=a1;}
 inline WidgetMenuEntry(const StartupSharedPrefixNew& end){name=(const char*)end.first;type=end.kind;data=(const void*)end.third;arg0=end.fourth;arg1=end.fifth;}
};
struct WidgetKeyboardFields {char prefix[0x254];char text[24];const char* value;Binding* binding;};
struct WidgetMenuDesc {const char* layout;WidgetMenuEntry* items;int(*poll)(int*);int flags,extra;};
typedef char WidgetMenuEntrySize20[sizeof(WidgetMenuEntry)==20?1:-1];typedef char WidgetMenuDescSize20[sizeof(WidgetMenuDesc)==20?1:-1];typedef char CaptionContextSize4[sizeof(CaptionContext)==4?1:-1];typedef char WidgetKeyboardFieldsSize274[sizeof(WidgetKeyboardFields)==0x274?1:-1];
extern "C" {extern WidgetMenuDesc lbl_802F9EE0;extern const char lbl_8055B890[6],lbl_8055B898[8],lbl_8055B8A0[3],lbl_8055B860[8];void fn_801A7A5C();int fn_802146BC(WidgetMenuDesc*,int,int,int);}
extern "C" {int fn_8021F478(int*);}
static char sKeyDetectTitle[] = "title";
void fn_8021F4F0(WidgetFragment *self)
{
  WidgetKeyboardFields *f = (WidgetKeyboardFields *) self;
  fn_801A7A5C();

  strcpy(lbl_80490934.label, f->text);
  KeyDetect *file = &lbl_80490934;
  file->binding = f->binding;
  WidgetMenuEntry items[] = {
    WidgetMenuEntry(sKeyDetectTitle,"Set Control",128),
    WidgetMenuEntry("message",3,f->value,128,0),
    WidgetMenuEntry("no", 0, 0, -1, 0),
    WidgetMenuEntry(lbl_8055B860, 6, 0, -1, 27),
    WidgetMenuEntry(lbl_80490920)
  };





  WidgetMenuDesc desc = {"cancel.lyt",0,fn_8021F478,0,0};
  desc.items = items;
  if ((-2) == fn_802146BC(&desc, -999, -999, 1))
  {
    strcpy(f->text, file->label);
    fn_80219718(self);
  }
  fn_802194FC(self);
  WidgetMarkDirty(self);
}
Slider::Slider(int x,int y,int width,int height,float* value,float minimum,int rate,float maximum) : UIWidget(x,y,x+width,y+height,0) {WidgetFragment* self=(WidgetFragment*)this;FloatSliderPrefix* f=(FloatSliderPrefix*)self;f->value=value;f->minimum=minimum;f->maximum=maximum;f->dragging=0;f->rate=InlineWidgetAbsolute(rate);f->negative=rate<0;f->background=gxGetImage("slider1.img");f->marker=gxGetImage("slider2.img");f->oldX=self->left;f->oldY=self->top;f->oldWidth=((WidgetDispatch*)self)->Slot78();f->oldHeight=self->bottom-self->top;int newHeight=fn_802C5220(f->background);if(self->bottom-self->top>newHeight)newHeight=self->bottom-self->top;fn_80219284(self,((WidgetDispatch*)self)->Slot78()-1,newHeight);fn_80219440(self,0,value,4);f->focus=0;f->token=fn_80228F40(x,y);}
void Slider::Slot14(void) {
    WidgetFragment* self = (WidgetFragment*)this;if(fn_80129BC4(self)){Bound274* f=(Bound274*)self;f->focus=fn_80217BD8(self->window,f->token,self);fn_80219440(self,1,f->focus,4);}}
int Slider::Slot64(int key) {
    WidgetFragment* self = (WidgetFragment*)this;FloatSliderPrefix* f=(FloatSliderPrefix*)self;if(!f->focus)return 0;if(f->token!=*f->focus)return 0;float old=*f->value;switch(key){case 0x127:{int index=(int)((0.005f)+(float)(f->rate-1)*(old-f->minimum)/(f->maximum-f->minimum));float value=f->minimum+(float)(index+1)*(f->maximum-f->minimum)/(float)(f->rate-1);if(value<f->minimum)value=f->minimum;if(value>f->maximum)value=f->maximum;*f->value=value;if(ObservedAbsDecision(*f->value-old)<=__float_epsilon[0])fn_80216D28();return 1;}case 0x125:{int index=(int)((0.005f)+(float)(f->rate-1)*(old-f->minimum)/(f->maximum-f->minimum));float value=f->minimum+(float)(index-1)*(f->maximum-f->minimum)/(float)(f->rate-1);if(value<f->minimum)value=f->minimum;if(value>f->maximum)value=f->maximum;*f->value=value;if(ObservedAbsDecision(*f->value-old)<=__float_epsilon[0])fn_80216D28();return 1;}case 0x126:if(((ChoiceWindowPrefix*)self->window)->depth>1){fn_80217D84(self->window);return 1;}break;case 0x128:if(((ChoiceWindowPrefix*)self->window)->depth>1){fn_80217C4C(self->window);return 1;}break;}return 0;}
void Slider::Slot4C(int x, int y) {
  FloatSliderPrefix *f = (FloatSliderPrefix *) ((WidgetFragment*)this);
  if (f->dragging && fn_8021FBC4(((WidgetFragment*)this), x, y))
  {
    float raw = f->minimum + ((((float) (x - f->oldX)) * (f->maximum - f->minimum)) / ((float) (f->oldWidth - (f->oldWidth / f->rate))));
    int steps = f->rate - 1;
float snapped=(float)(int)((0.005f)+(float)steps*(raw-f->minimum)/(f->maximum-f->minimum));float value=f->minimum+snapped*(f->maximum-f->minimum)/(float)steps;
    if (value < f->minimum)
    {
      value = f->minimum;
    }
    if (value > f->maximum)
    {
      value = f->maximum;
    }
    *f->value = value;
  }
}
int fn_8021FBC4(WidgetFragment* self,int x,int y){HitAreaPrefix* f=(HitAreaPrefix*)self;int result=0,insideY=0,insideX=0;if(x>=f->x-20&&x<f->x+f->width+20)insideX=1;if(insideX&&y>=f->y-20)insideY=1;if(insideY&&y<f->y+f->height+20)result=1;return result;}
void Slider::Slot24(void* context) {fn_802C3034(reinterpret_cast<gxCanvas*>(context));Slot24__8UIWidgetFPv(((WidgetFragment*)this),context);FloatSliderPrefix* f=(FloatSliderPrefix*)((WidgetFragment*)this);int top,left;int span=((WidgetDispatch*)((WidgetFragment*)this))->Slot78()-11;int fill;int selected=0;fill=(int)((float)span*(*f->value-f->minimum)/(f->maximum-f->minimum));if(f->focus&&*f->focus==f->token)selected=1;top=((WidgetFragment*)this)->top;left=((WidgetFragment*)this)->left;int width=((WidgetDispatch*)((WidgetFragment*)this))->Slot78();fn_80228DC4((ObservedClipPrefix*)context,f->background,left,top,width,selected);fn_80228DC4((ObservedClipPrefix*)context,f->marker,((WidgetFragment*)this)->left+5,((WidgetFragment*)this)->top+5,fill,selected);}
void Slider::Slot28(void) {FloatSliderPrefix* f=(FloatSliderPrefix*)((WidgetFragment*)this);int span=((WidgetDispatch*)((WidgetFragment*)this))->Slot78()-11;int fill;int selected=0;fill=(int)((float)span*(*f->value-f->minimum)/(f->maximum-f->minimum));if(f->focus&&*f->focus==f->token)selected=1;float width=(float)((WidgetDispatch*)((WidgetFragment*)this))->Slot78();float height=(float)(((WidgetFragment*)this)->bottom-((WidgetFragment*)this)->top);float x=(float)(((WidgetFragment*)this)->left-lbl_8055FB9C/2),y=(float)(lbl_8055FBA0/2-((WidgetFragment*)this)->top);fn_802C5320(f->background,selected,x,y,(0.0f),width,height);fn_802C5334(f->marker,selected,x,y,(0.0f),(float)fill,height,(0.0f),(0.0f),(float)fill/width,(1.0f));}
SimpleSlider::SimpleSlider(WidgetDesc* desc,float* value,int divisions,float minimum,float maximum) : UIWidget(desc->x,desc->y,desc->x+desc->dx,desc->y+desc->dy,0) {WidgetFragment* self=(WidgetFragment*)this;SteppedFloatPrefix* f=(SteppedFloatPrefix*)self;f->value=value;f->minimum=minimum;f->maximum=maximum;f->step=(maximum-minimum)/(float)divisions;f->image=gxGetImage("slider2.img");f->focus=0;f->token=fn_80228F40(desc->x,desc->y);f->nextTime=fn_8023659C()-(0.3f);f->previousTime=f->nextTime;}
SimpleSlider::~SimpleSlider(void) {WidgetFragment* self=(WidgetFragment*)this;fn_802C519C((void*)self->label);}
void SimpleSlider::Slot28(void) {int previousFrame;
 int nextFrame;
 gArtSet* previousArrow;
 gArtSet* nextArrow;
 float nextX;
 float fraction;
 float height;
 float filledWidth;
 SteppedFloatPrefix* f=(SteppedFloatPrefix*)((WidgetFragment*)this);
 if(f->value)fraction=(*f->value-f->minimum)/(f->maximum-f->minimum);
 else fraction=(1.0f);
 float coordinates[2];
 coordinates[1]=(0.0f);
 coordinates[0]=(0.0f);
 fn_80230C30(&coordinates[1],&coordinates[0],((WidgetFragment*)this)->left,((WidgetFragment*)this)->top);
 float width=(float)((WidgetDispatch*)((WidgetFragment*)this))->Slot78();
 height=(float)(((WidgetFragment*)this)->bottom-((WidgetFragment*)this)->top);
 filledWidth=fraction*(float)fn_802C51C0(f->image);
 float imageHeight=(float)fn_802C5220(f->image);
 previousFrame=0;
 if(f->focus&&*f->focus==f->token)previousFrame=1;
 fn_802C5334(f->image,previousFrame,coordinates[1]+0.5f*(width-(float)fn_802C51C0(f->image)),coordinates[0]+(imageHeight-height)/2.0f,(0.0f),filledWidth,imageHeight,(0.0f),(0.0f),fraction,(1.0f));
 
previousArrow=static_cast<gArtSet*>(fn_80211660(0));
 nextArrow=static_cast<gArtSet*>(fn_80211660(1));
 {bool previousSelected;
 {int* focus=f->focus;
 previousSelected=false;
 if(focus){if(*focus==f->token)previousSelected=true;
 }}if(previousSelected){float now=fn_8023659C();
 if((0.3f)+f->previousTime>now)previousFrame=2;
 else previousFrame=1;
 }else previousFrame=0;
 }bool nextSelected=false;
 if(f->focus&&*f->focus==f->token)nextSelected=true;
 if(nextSelected){float now=fn_8023659C();
 if((0.3f)+f->nextTime>now)nextFrame=2;
 else nextFrame=1;
 }else nextFrame=0;
 
float previousOffset=ObservedArrowOffset(&f->previousTime),nextOffset=ObservedArrowOffset(&f->nextTime);
 previousOffset=(coordinates[1]-fn_80235EF0(previousArrow,previousFrame))-previousOffset;
 nextX=coordinates[1]+width;
 nextX=nextOffset+nextX;
 u32 previousColor=0xffffffffu,nextColor=0xffffffffu;
 if(fraction<=__float_epsilon[0])previousColor=ObservedArrowColor(&f->previousTime);
 if(fraction>=(1.0f)-__float_epsilon[0])nextColor=ObservedArrowColor(&f->nextTime);
 
if(previousArrow){float arrowWidth=fn_80235EF0(previousArrow,previousFrame);
 float arrowHeight=fn_80235F64(previousArrow,previousFrame);
 fn_802354A0(previousArrow,previousFrame,previousOffset,coordinates[0]+(arrowHeight-height)/2.0f,(0.0f),arrowWidth,arrowHeight,previousColor);
 }if(nextArrow){float arrowWidth=fn_80235EF0(nextArrow,nextFrame);
 float arrowHeight=fn_80235F64(nextArrow,nextFrame);
 fn_802354A0(nextArrow,nextFrame,nextX,coordinates[0]+(arrowHeight-height)/2.0f,(0.0f),arrowWidth,arrowHeight,nextColor);
 }}
int SimpleSlider::Slot64(int key) {SteppedFloatPrefix* f=(SteppedFloatPrefix*)((WidgetFragment*)this);if(!f->focus||*f->focus!=f->token)return 0;switch(key){case 0x127:if(f->value&&*f->value!=f->maximum){f->nextTime=fn_8023659C();*f->value=InlineWidgetMinimum(*f->value+f->step,f->maximum);}break;case 0x125:if(f->value&&*f->value!=f->minimum){f->previousTime=fn_8023659C();*f->value=InlineWidgetMaximum(*f->value-f->step,f->minimum);}break;case 0x126:if(((ChoiceWindowPrefix*)((WidgetFragment*)this)->window)->depth>1){fn_80217D84(((WidgetFragment*)this)->window);fn_80230E90(0);}else return 0;break;case 0x128:if(((ChoiceWindowPrefix*)((WidgetFragment*)this)->window)->depth>1){fn_80217C4C(((WidgetFragment*)this)->window);fn_80230E90(0);}else return 0;break;default:return 0;}return 1;}
void SimpleSlider::Slot14(void) {if(fn_80129BC4(((WidgetFragment*)this))){Bound258* f=(Bound258*)((WidgetFragment*)this);f->focus=fn_80217BD8(((WidgetFragment*)this)->window,f->token,((WidgetFragment*)this));fn_80219440(((WidgetFragment*)this),1,f->focus,4);}}
WidgetFragment* fn_80220778(WidgetFragment* self,int x,int y,int width,int height,int* value,int minimum,int maximum){__ct__8UIWidgetFiiiii(self,x,y,x+width,y+height,0);SliderFullPrefix* f=(SliderFullPrefix*)self;f->value=value;self->vtable=lbl_8033E668;f->minimum=minimum;f->maximum=maximum;fn_80219440(self,0,value,4);f->background=gxGetImage("slider1.img");f->marker=gxGetImage("slider2.img");if(f->background){int height=fn_802C5220(f->background);int width=((WidgetDispatch*)self)->Slot78();fn_80219284(self,width,height);}f->dragging=0;f->focus=0;f->token=fn_80228F40(x,y);return self;}
WidgetFragment* fn_80220874(WidgetFragment* self,int destroy){if(self){self->vtable=lbl_8033E668;PairImage248* f=(PairImage248*)self;fn_802C519C(f->first);fn_802C519C(f->second);__dt__8UIWidgetFv(self,0);if(destroy>0)__dl__FPv(self);}return self;}
void fn_802208EC(WidgetFragment* self){if(fn_80129BC4(self)){Bound25c* f=(Bound25c*)self;f->focus=fn_80217BD8(self->window,f->token,self);fn_80219440(self,1,f->focus,4);}}
int fn_80220948(WidgetFragment* self,int key){SliderFullPrefix* f=(SliderFullPrefix*)self;if(!f->focus)return 0;if(f->token!=*f->focus)return 0;switch(key){case 0x127:if(*f->value<f->maximum-1)++*f->value;else fn_80216D28();return 1;case 0x125:if(*f->value>f->minimum)--*f->value;else fn_80216D28();return 1;case 0x126:if(((ChoiceWindowPrefix*)self->window)->depth>1){fn_80217D84(self->window);return 1;}break;case 0x128:if(((ChoiceWindowPrefix*)self->window)->depth>1){fn_80217C4C(self->window);return 1;}break;}return 0;}
void fn_80220A50(WidgetFragment* self,int x,int unused){SliderFullPrefix* f=(SliderFullPrefix*)self;f->dragging=1;int left=self->left;int minimum=f->minimum;int track=((WidgetDispatch*)self)->Slot78()-10;int value=minimum+(f->maximum-minimum)*(x-left-5)/track;if(value<minimum)value=minimum;value=ClampToExclusiveMaximum(f->maximum,value);*f->value=value;}
void fn_80220AFC(WidgetFragment* self,int x,int y){((RowInputDispatch*)self)->Slot4c(x,y);((PrefixFlag258*)self)->value=0;}
void fn_80220B3C(WidgetFragment* self,int x,int y){if(((PrefixFlag258*)self)->value)((PointerEventDispatch*)self)->Slot3c(x,y);}
void fn_80220B5C(WidgetFragment* self,void* context){int top,left;fn_802C3034(reinterpret_cast<gxCanvas*>(context));SliderFullPrefix* f=(SliderFullPrefix*)self;int span=((WidgetDispatch*)self)->Slot78()-11;int fill=(span*(*f->value-f->minimum))/(f->maximum-f->minimum-1);int selected=0;if(f->focus&&*f->focus==f->token)selected=1;top=self->top;left=self->left;int width=((WidgetDispatch*)self)->Slot78();fn_80228DC4((ObservedClipPrefix*)context,f->background,left,top,width,selected);fn_80228DC4((ObservedClipPrefix*)context,f->marker,self->left+5,self->top+5,fill,selected);}
void fn_80220C54(WidgetFragment* self){float height;SliderFullPrefix* f=(SliderFullPrefix*)self;int span=((WidgetDispatch*)self)->Slot78()-11;int fill=(span*(*f->value-f->minimum))/(f->maximum-f->minimum-1);int selected=0;if(f->focus&&*f->focus==f->token)selected=1;float x=(float)(self->left-lbl_8055FB9C/2);float y=(float)(lbl_8055FBA0/2-self->top);height=(float)fn_802C5220(f->background);fn_802C5320(f->background,selected,x,y,(0.0f),(float)((WidgetDispatch*)self)->Slot78(),height);height=(float)fn_802C5220(f->marker);fn_802C5320(f->marker,selected,(5.0f)+x,y-(5.0f),(0.0f),(float)fill,height);}
void fn_80220E08(WidgetFragment* self,void* context){fn_802C3034(reinterpret_cast<gxCanvas*>(context));fn_802C40B8(((CanvasWidgetPrefix*)self)->canvas,self->left,self->top);}
ScrollBar::ScrollBar(int x,int y,int width,int height,void* view,int vertical) : UIWidget(x,y,x+width,y+height,0) {WidgetFragment* self=(WidgetFragment*)this;ScrollbarPrefix* f=(ScrollbarPrefix*)self;if(!vertical){f->track=gxGetImage("vscroll.img");f->marker=gxGetImage("vthumb.img");}else{f->track=gxGetImage("hscroll.img");f->marker=gxGetImage("hthumb.img");}((WidgetDispatch*)self)->Slot1c(self->left,self->top);if(!vertical){int width=fn_802C51C0(f->track);fn_80219284(self,width,self->bottom-self->top);}else{int height=fn_802C5220(f->track);int width=((WidgetDispatch*)self)->Slot78();fn_80219284(self,width,height);}f->view=view;f->vertical=vertical;fn_80219440(self,0,view,12);f->dragging=0;}
void ScrollBar::Slot3C(int x, int y) {
 DragViewportPrefix* f=(DragViewportPrefix*)((WidgetFragment*)this);f->dragging=1;float ratio;
 if(f->direction==0){
 ObservedListViewport* view=f->state;int top=((WidgetFragment*)this)->top;ratio=(float)(((WidgetFragment*)this)->bottom-top)/(float)view->total;
 int start=view->start;int low=top+(int)(ratio*(float)start)+5;int high=top+(int)(ratio*(float)(start+view->page))-5;
 if(y>=low&&y<=high){f->grabY=(int)(ratio*(float)start)-y;}
 else {if(y<low){int position=(int)((float)start-((1.0f)/ratio)*(float)(high-low+5));SetListViewportValue(position,view);}
 else {int position=(int)(AddScroll358((float)start,((1.0f)/ratio)*(float)(high-low+5)));SetListViewportValue(position,view);}f->dragging=0;}
 }else{
 int total=f->state->total;int width=((WidgetDispatch*)((WidgetFragment*)this))->Slot78();ObservedListViewport* view=f->state;
 ratio=(float)width/(float)total;int start=view->start;int left=((WidgetFragment*)this)->left;int low=left+(int)(ratio*(float)start)+5;int high=left+(int)(ratio*(float)(start+view->page))-5;
 if(x>=low&&x<=high){f->grabX=(int)(ratio*(float)start)-x;}
 else {if(x<low){int position=(int)((float)start-((1.0f)/ratio)*(float)(high-low+5));SetListViewportValue(position,view);}
 else {int position=(int)(AddScroll358((float)start,((1.0f)/ratio)*(float)(high-low+5)));SetListViewportValue(position,view);}f->dragging=0;}
 }
}
void ScrollBar::Slot4C(int x, int y) {
 DragViewportPrefix* f=(DragViewportPrefix*)((WidgetFragment*)this);
 if(f->dragging){
 if(f->direction==0){
 ObservedListViewport* state=f->state;
 int value=(int)(((float)state->total/(float)(((WidgetFragment*)this)->bottom-((WidgetFragment*)this)->top))*(float)(y+f->grabY));
 state->start=value;if(value>state->total-state->page)state->start=state->total-state->page;if(state->start<0)state->start=0;
 }else{
 int total=f->state->total;float ratio=(float)total/(float)((WidgetDispatch*)((WidgetFragment*)this))->Slot78();ObservedListViewport* state=f->state;int value=(int)(ratio*(float)(x+f->grabX));
 state->start=value;if(value>state->total-state->page)state->start=state->total-state->page;if(state->start<0)state->start=0;
 }
 }
}
void ScrollBar::Slot40(int, int) {
    WidgetFragment* self = (WidgetFragment*)this;((PrefixFlag250*)self)->value=0;}
void ScrollBar::Slot24(void* context) {
  int imageExtent;

  int trackWidth;

  DragViewportPrefix *f = (DragViewportPrefix *) ((WidgetFragment*)this);

  fn_802C3034(reinterpret_cast<gxCanvas*>(context));

  if (f->direction == 0)
  {
    fn_80228E80((ObservedClipPrefix *) context, f->first, ((WidgetFragment*)this)->left, ((WidgetFragment*)this)->top, ((WidgetFragment*)this)->bottom - ((WidgetFragment*)this)->top, 0);

  }
  else
  {
    int y=((WidgetFragment*)this)->Top();

    int x=((WidgetFragment*)this)->Left();

    int width = ((WidgetDispatch*)((WidgetFragment*)this))->Slot78();

    fn_80228DC4((ObservedClipPrefix *) context, f->first, x, y, width, 0);

  }
  int total=f->state->Total();

  if (f->state->page < total)
  {
  if (f->direction == 0) { int left;
 int right;
 int top;
 int bottom;
 int x=((WidgetFragment*)this)->Left();
 left = x + 1;
 float ratio = ((float) (((WidgetFragment*)this)->bottom - ((WidgetFragment*)this)->top)) / ((float) total);
 trackWidth = ((WidgetDispatch*)((WidgetFragment*)this))->Slot78();
 int rightEdge = x + trackWidth;
 right = rightEdge - 1;
 const int& start=f->state->start;
 top=(((WidgetFragment*)this)->top + ((int) (ratio * ((float) start))))+5;
bottom=(((WidgetFragment*)this)->top + ((int) (ratio * ((float) (start + f->state->page)))))-5;
 if (((bottom - 1) > (top + 1)) && ((right - 1) > (left + 1))) { if (f->second) { void *image = f->second;
 imageExtent = fn_802C51C0(f->first);
 int tw = fn_802C51C0(image);
 fn_80228E80((ObservedClipPrefix *) context, f->second, left + ((imageExtent - tw) / 2), top, bottom - top, 0);
 }
else { fn_802C3424(left, top, right, bottom, lbl_8055FBDC);
 const u32 dark = lbl_8055FBE0;
 fn_802C3424(left, bottom - 1, right, bottom, dark);
 fn_802C3424(right - 1, top, right, bottom, dark);
 const u32 light = lbl_8055FBFC;
 fn_802C3424(left, top, right, top + 1, light);
 fn_802C3424(left, top, left + 1, bottom, light);
 } } }
else { int left;
 int right;
 int top;
 int bottom;
 int width = ((WidgetDispatch*)((WidgetFragment*)this))->Slot78();
 float ratio = ((float) width) / ((float) total);
 int start = f->state->start;
 int edge = ((WidgetFragment*)this)->bottom;
 int y=((WidgetFragment*)this)->Top();
 top = y + 1;
 bottom = edge - 1;
 int low = ((WidgetFragment*)this)->left + ((int) (ratio * ((float) start)));
 int high=((WidgetFragment*)this)->ProjectX(ratio,start+f->state->page);
 left = low + 5;
 right = high - 5;
 if (((right - 1) > (left + 1)) && ((bottom - 1) > (top + 1))) { if (f->second) { void *image = f->second;
 imageExtent = fn_802C5220(f->first);
 int th = fn_802C5220(image);
 fn_80228DC4((ObservedClipPrefix *) context, f->second, left, top + ((imageExtent - th) / 2), right - left, 0);
 }
else { fn_802C3424(left, top, right, bottom, lbl_8055FBDC);
 const u32 dark = lbl_8055FBE0;
 fn_802C3424(left, bottom - 1, right, bottom, dark);
 fn_802C3424(right - 1, top, right, bottom, dark);
 const u32 light = lbl_8055FBFC;
 fn_802C3424(left, top, right, top + 1, light);
 fn_802C3424(left, top, left + 1, bottom, light);
 } } } 
  }
}
void ScrollBar::Slot28(void) {
  int imageExtent;
  int trackWidth;
  DragViewportPrefix* f=(DragViewportPrefix*)((WidgetFragment*)this);
  float x=(float)(((WidgetFragment*)this)->left-(lbl_8055FB9C/2));
  float y=(float)((lbl_8055FBA0/2)-((WidgetFragment*)this)->top);
  float initialWidth=(float)((WidgetDispatch*)((WidgetFragment*)this))->Slot78();
  // Width is measured before these height reads, as in the original call sequence.
  fn_802C5320(f->first,0,x,y,(0.0f),initialWidth,(float)(((WidgetFragment*)this)->bottom-((WidgetFragment*)this)->top));
  int total=f->state->Total();
  if (f->state->page < total) {
    if (f->direction == 0) {
      int left;
      int right;
      int top;
      int bottom;
      int originX=((WidgetFragment*)this)->Left();
      left = originX + 1;
      float ratio = ((float) (((WidgetFragment*)this)->bottom - ((WidgetFragment*)this)->top)) / ((float) total);
      trackWidth = ((WidgetDispatch*)((WidgetFragment*)this))->Slot78();
      int rightEdge = originX + trackWidth;
      right = rightEdge - 1;
      const int& start=f->state->start;
      top=(((WidgetFragment*)this)->top + ((int) (ratio * ((float) start))))+5;
      bottom=(((WidgetFragment*)this)->top + ((int) (ratio * ((float) (start + f->state->page)))))-5;
      if (((bottom - 1) > (top + 1)) && ((right - 1) > (left + 1))) {
        if (f->second) {
          void *image = f->second;
          imageExtent = fn_802C51C0(f->first);
          int tw = fn_802C51C0(image);
          fn_80230C30(&x,&y,left+((imageExtent-tw)/2),top);
          float imageMetric=(float)fn_802C51C0(f->second);
          fn_802C5320(f->second,0,x,y,(0.0f),imageMetric,(float)(bottom-top));
          return;
        }
        else {
          fn_802E30C8("_wid: ScrollBar, no slider(thumb), using 2D Draw");
          fn_802C3424(left, top, right, bottom, lbl_8055FBDC);
          const u32 dark = lbl_8055FBE0;
          fn_802C3424(left, bottom - 1, right, bottom, dark);
          fn_802C3424(right - 1, top, right, bottom, dark);
          const u32 light = lbl_8055FBFC;
          fn_802C3424(left, top, right, top + 1, light);
          fn_802C3424(left, top, left + 1, bottom, light);
        }
      }
    }
    else {
      int top;
      int left;
      int bottom;
      int right;
      int width = ((WidgetDispatch*)((WidgetFragment*)this))->Slot78();
      float ratio = ((float) width) / ((float) total);
      int start = f->state->start;
      int edge = ((WidgetFragment*)this)->bottom;
      int originY=((WidgetFragment*)this)->Top();
      top = originY + 1;
      bottom = edge - 1;
      int low = ((WidgetFragment*)this)->left + ((int) (ratio * ((float) start)));
      int high=((WidgetFragment*)this)->ProjectX(ratio,start+f->state->page);
      left = low + 5;
      right = high - 5;
      if (((right - 1) > (left + 1)) && ((bottom - 1) > (top + 1))) {
        if (f->second) {
          void *image = f->second;
          imageExtent = fn_802C5220(f->first);
          int th = fn_802C5220(image);
          fn_80230C30(&x,&y,left,top+((imageExtent-th)/2));
          float imageMetric=(float)fn_802C5220(f->second);
          fn_802C5320(f->second,0,x,y,(0.0f),(float)(right-left),imageMetric);
          return;
        }
        else {
          fn_802E30C8("_wid: ScrollBar, no slider(thumb), using 2D Draw");
          fn_802C3424(left, top, right, bottom, lbl_8055FBDC);
          const u32 dark = lbl_8055FBE0;
          fn_802C3424(left, bottom - 1, right, bottom, dark);
          fn_802C3424(right - 1, top, right, bottom, dark);
          const u32 light = lbl_8055FBFC;
          fn_802C3424(left, top, right, top + 1, light);
          fn_802C3424(left, top, left + 1, bottom, light);
        }
      }
    }
  }
}
void fn_80221E7C(WidgetFragment* self){WidgetFragment* parent=(WidgetFragment*)self->kind;if(parent)WidgetMarkDirty(parent);}
void fn_80221EA0(WidgetFragment* self,int key,void* value,int size){fn_80219440((WidgetFragment*)self->kind,key,value,size);}
int fn_80221EA8(GroupSearchProxyNew* self){GroupSearchWordNew one=1;GroupSearchWordNew group=self->context->group;for(int index=0;index<32;index++){if(group==(one<<index))return index;}fn_802E317C("UICustomControl::GetGroup can't find group for %x!",group);return 0;}
void Finish__9MenuStateFi(void*,void* context){fn_802168BC(reinterpret_cast<int>(context));}
extern const u32 lbl_8055FBE4;
// NonMatching: clean two-argument menu draw remains41 original-address words off.
void fn_80221F9C(WidgetFragment* self, void* context) {
    MenuEntryCountPrefix* list;
    struct {
        int ty;
    } row;
    int top;
    int i;
    int y;
    int color;
    int background;
    int highlight;
    int col;
    int x;
    MenuSelectionPrefix* f;
    int w;
    int t;
    int off;
    int c;
    int l;
    fn_802C3034(reinterpret_cast<gxCanvas*>(context));
    l = self->left;
    t = self->top;
    w = ((WidgetDispatch*)self)->Slot78();
    fn_802C3424(l, t, l + w, t + 10, lbl_8055FBE4);
    f = (MenuSelectionPrefix*)self;
    x = self->left;
    y = self->top;
    color = lbl_8055FBC0;
    background = lbl_8055FBFC;
    highlight = lbl_8055FBF8;
    for (col = 0; col < ((MenuTablePrefix*)f->rows)->count; col++) {
        list = (MenuEntryCountPrefix*)f->rows[col].handle;
        fn_802C4F58(x + 2, y + 2, (const char*)&f->rows[col] + 4, color);
        if (col == f->row) {
            fn_802C3424(x, y + 10, x + 50, y + 10 + list->count * 10, background);
            top = y + 11;
            row.ty = y + 12;
            i = 0;
            off = 0;
            for (; i < list->count; i++) {
                if (i == f->column) {
                    fn_802C3424(x + 1, top, x + 49, off + y + 19, color);
                }
                c = color;
                if (i == f->column) {
                    c = highlight;
                }
                fn_802C4F58(x + 2, row.ty, (const char*)fn_8022D97C(list, i), c);
                off += 10;
                top += 10;
                row.ty += 10;
            }
        }
        x += 50;
    }}
void fn_80222120(WidgetFragment* self,int x,int y){((PrefixFlag248*)self)->value=1;((RowInputDispatch*)self)->Slot4c(x,y);WidgetMarkDirty(self);}
void fn_80222178(WidgetFragment* self,int x,int y){((RowInputDispatch*)self)->Slot4c(x,y);MenuSelectionPrefix* f=(MenuSelectionPrefix*)self;f->active=0;if(f->row>=0&&f->column>=0){MenuRecord* record=fn_8022D97C(f->rows[f->row].handle,f->column);if(!record->callback||record->callback(record->context))fn_802168BC(reinterpret_cast<int>(record->context));}f->row=-1;f->column=-1;WidgetMarkDirty(self);}
void fn_80222234(MenuHitWidgetNew *self, int x, int y) {
    int left;
    int row;
    int top;
    int column;
    int headerRow;

    if ((int) self->active != 0) {
        if ((int) self->row >= (int) self->table->count) {
            self->row = -1;
        }
        top = self->top;
        if (y < (int) (top + 0xA)) {
            headerRow = (x - self->left) / 50;
            if ((headerRow >= 0) && ((int) headerRow < (int) self->table->count)) {
                self->row = (int) headerRow;
                self->column = -1;
            }
        } else {
            row = self->row;
            if (row >= 0) {
                left = self->left;
                if ((x >= MenuBoundAddNew(row*50,left)) && (x <= MenuBoundAddNew((row+1)*50,left))) {
                    MenuHitTableNew* table=self->table;MenuHitListNew* list=table->entries[row].list;column = ((y - top) - 0xA) / 10;
                    if ((column >= 0) && ((int) column < (int) list->count)) {
                        self->column = (int) column;
                        return;
                    }
                    self->column = -1;
                    return;
                }
                self->column = -1;
            }
        }
    }
}
WidgetFragment* fn_80222340(WidgetFragment* self,int x,int y,int kind,int dx,int dy,OwnedWidget* child){
 __ct__8UIWidgetFiiiii(self,x,y,x+dx,y+dy,kind);self->vtable=lbl_8033F90C;
 OwnedWidgetPrefix* fields=(OwnedWidgetPrefix*)self;fields->x=x;fields->y=y;fields->kind=kind;fields->child=child;
 ((WidgetDispatch*)self)->Slot84();return self;
}
WidgetFragment* fn_802223C4(WidgetFragment* self,int destroy){
 if(self){OwnedWidget* child=((OwnedWidgetPrefix*)self)->child;self->vtable=lbl_8033F90C;delete child;__dt__8UIWidgetFv(self,0);if(destroy>0)__dl__FPv(self);}return self;
}
void fn_80222450(WidgetFragment* self){WidgetMarkDirty(self);}
void fn_80222468(WidgetFragment* self,int x,int y){Slot1C__8UIWidgetFii(self,x,y);OwnedWidgetPrefix* fields=(OwnedWidgetPrefix*)self;fields->x=x;fields->y=y;}
WidgetFragment* fn_802224B4(WidgetFragment* self,void* context,int x,int y,int dx,int dy,OwnedWidget* child,int(*callback)(void*),unsigned char a,unsigned char b){
 fn_80222340(self,x,y,0,dx,dy,child);self->vtable=lbl_8033F880;
 CallbackPrefix* fields=(CallbackPrefix*)self;fields->context=context;fields->callback=callback;fields->flag256=a;fields->flag257=b;fields->unknown255=0;fields->active=0;return self;
}
ResourcePrefix::ResourcePrefix(const char* path, const char* t) {
 resource=fn_80232578(path);text=t;
}
ResourcePrefix::~ResourcePrefix(void) {
 fn_802328D4(resource);
}
void ResourcePrefix::Draw(int x, int y, int, unsigned int) {
    ResourcePrefix* self = (ResourcePrefix*)this;
 float a,b;fn_80230C30(&a,&b,x,y);float zero=(0.0f);fn_802336AC(self->resource,-1,9,"%s",a,b,zero,self->text);
}
void ResourcePrefix::Rect(int first, int second, int* left, int* top, int* right, int* bottom) {
    ResourcePrefix* self = (ResourcePrefix*)this;
 *left=first;*top=second;*right=first+(int)fn_8023293C(self->resource,self->text,(1.0f));*bottom=first+fn_80232B70(self->resource);
}
ImagePrefix::ImagePrefix(const char* path){image=fn_8023512C(path);}
ImagePrefix::~ImagePrefix(void) {
 if(image)fn_80235380(image);
}
void ImagePrefix::Draw(int x, int y, int unused, unsigned int p_frame) {
    ImagePrefix* self = (ImagePrefix*)this;
    int frame = (int)p_frame;
 if(self->image){float zero=(0.0f);float a,b;fn_80230C30(&a,&b,x,y);fn_802353F4(self->image,frame,a,b,zero);}
}
void ImagePrefix::Rect(int first, int second, int* left, int* top, int* right, int* bottom) {
    ImagePrefix* self = (ImagePrefix*)this;
 *left=first;*top=second;
 if(!self->image){*right=first+10;*bottom=second+10;}
 else{*right=first+(int)fn_80235EF0(self->image,0);*bottom=first+(int)fn_80235F64(self->image,0);}
}
AnimationPrefix::AnimationPrefix(const char* path,int w,int h){
 handle=gxGetImage(path);
 if(w>0&&h>0){width=(float)w;height=(float)h;}
 else{width=(float)fn_802C51C0(handle);height=(float)fn_802C5220(handle);}
}
AnimationPrefix::~AnimationPrefix(void) {
 fn_802C519C(handle);
}
int AnimationPrefix::Playing(void) {
    AnimationPrefix* self = (AnimationPrefix*)this;int result=fn_802C51AC(self->handle);if(result)result=fn_802C51AC(self->handle);return result;}
void AnimationPrefix::Draw(int x, int y, int unused, unsigned int p_frame) {
    AnimationPrefix* self = (AnimationPrefix*)this;
    u32 frame = (u32)p_frame;
 if(fn_802C51AC(self->handle)){
  float zero=(0.0f);float a=zero,b=zero;fn_80230C30(&a,&b,x,y);
  if(frame!=0xFFFFFFFFu){int selected=(int)frame%fn_802C51AC(self->handle);fn_802C5320(self->handle,selected,a,b,zero,self->width,self->height);}
 }
}
void AnimationPrefix::Rect(int first, int second, int* left, int* top, int* right, int* bottom) {
    AnimationPrefix* self = (AnimationPrefix*)this;
 *left=first;*top=second;*right=*left+(int)self->width;*bottom=*top+(int)self->height;
}
int AnimationPrefix::Hit(int x, int y, int testX, int testY) {
    AnimationPrefix* self = (AnimationPrefix*)this;
 int bottom,right,top,left;
 ((AnimationBoundsDispatch*)self)->Bounds(x,y,&left,&top,&right,&bottom);
 return testX>=left&&testX<right&&testY>=top&&testY<bottom;
}
int AnimationPrefix::Frames(void) {
    AnimationPrefix* self = (AnimationPrefix*)this;return fn_802C51AC(self->handle);}
WidgetFragment* fn_80222BDC(WidgetFragment* self,WidgetDesc* desc,void* context){__ct__8UIWidgetFP10WidgetDesc(self,desc);UnsignedAssetPrefix* f=(UnsignedAssetPrefix*)self;f->context=context;self->vtable=lbl_8033E560;f->resource=fn_80232578(desc->name);if(!f->resource)fn_802E317C("Widget CircleList: Invalid font %s",desc->name);f->mode=desc->fielda0;f->scale=(float)(u32)desc->field9c;f->elapsed=(0.2094f);return self;}
WidgetFragment* fn_80222C8C(WidgetFragment* self,int destroy){if(self){self->vtable=lbl_8033E560;fn_802328D4(((UnsignedAssetPrefix*)self)->resource);__dt__8UIWidgetFv(self,0);if(destroy>0)__dl__FPv(self);}return self;}
void fn_80222CFC(WidgetFragment *source) {
    int count;
    int i2;
    const CircularPoseTypedNew* src;
    CircularPoseTypedNew* dst;
    const char* text;
    int j;
    CircularPaintNew* self;
    int i1;
    float base;
    float total;
    float spacing;
    float scale;
    float pos;
    RotationMatrix m;
    float x;
    float y;
    char one[4];
    self = (CircularPaintNew*)source;
    count = self->list->used;
    base = -self->rate * fn_8023659C();
    if (count > 0) {
        total = (0.0f);
        for (i1 = 0; i1 < count; i1++) {
            total += fn_8023293C(self->font, self->list->Get(i1, 0), (1.0f));
        }
        spacing = (((6.2831855f) * self->radius) - total) / (float)count;
        scale = (-6.2831855f) / (total + (spacing * (float)count));
        fn_80230C30(&x, &y, self->left, self->top);
        src = fn_8022EDC4();
        dst = fn_8022EDE4(&src->xf, src->alpha);
        pos = (0.0f);
        for (i2 = 0; i2 < count; i2++) {
            text = self->list->Get(i2, 0);
            for (j = 0; j < (int)strlen(text); j++) {
                sprintf(one, "%c", (int)(signed char)text[j]);
                dst->xf.basis = src->xf.basis;
                dst->xf.translation = src->xf.translation;
                RotationVec input = {x, y, 0.0f};
                ChoiceApplyOffsetNew(&dst->xf, &input);
                float angleProduct = scale * pos; RotationVec rotation = {0.0f, 0.0f, angleProduct + base};
                Mat3FromEuler(&m, &rotation);
                Mat3Mul(&dst->xf.basis, &m, &dst->xf.basis);
                fn_8022ED00(dst);
                fn_80232BA4(one, self->font, (0.0f), self->radius, self->color, 9, (1.0f), (0.0f), (1.0f));
                fn_8022EDB8();
                pos += fn_8023293C(self->font, one, (1.0f));
            }
            pos += spacing;
        }
        fn_8022EE98(dst);
    }
}
void fn_8022375C(WidgetFragment*);extern const float lbl_8056352C,lbl_80563530;
struct SelectionConstructBaseObservedNew {char storage[0x244];inline SelectionConstructBaseObservedNew(int x,int y){__ct__8UIWidgetFiiiii((WidgetFragment*)this,x,y,0,0,0);}};
struct SelectionFullPrefix:SelectionConstructBaseObservedNew {const char* labels[33];int count;int* selected;unsigned char available[33];char padding2f1[3];float widths[33];int icons[33];GTextFont* resource;float period,scale,speed,first,second,pulse;char gap418[4];bool locked;};
typedef char Selection391FullSize420[sizeof(SelectionFullPrefix)==0x420?1:-1];
BottomBar::BottomBar(int x,int y,const StringList* list,int* selectedInput) : UIWidget(x,y,0,0,0), selected(selectedInput) {WidgetFragment* self=(WidgetFragment*)this;SelectionFullPrefix* instance=(SelectionFullPrefix*)self;
  int count = list->used;
  SelectionFullPrefix *f = (SelectionFullPrefix *) self;
  f->count=count;


  f->resource = fn_80232578("uifnt.gtx");

  f->period = (18.0f);
  f->scale = (1.0f);
  float zero = (0.0f);

  int byteOffset;
  SelectionFullPrefix *entry;
  int i;
  const char* label;
  for (i=0,byteOffset=0; i < 33; i++, byteOffset += 4)
  {
    instance->available[i] = 0;
    if (i < f->count)
    {
      int flags = 0;
      label=list->Get(i,&flags);
      if ((flags & 3) != 1)
      {
        f->available[i] = 1;
      }
      entry = (SelectionFullPrefix *) (((u32) f) + byteOffset);
      entry->icons[0] = (flags >> 2) & 63;
      entry->widths[0] = f->scale * fn_8023293C(f->resource, label, (1.0f));
      entry->labels[0] = label;
      continue;
    }
    entry = (SelectionFullPrefix *) (((u32) f) + byteOffset);
    entry->labels[0] = 0;
    entry->widths[0] = zero;
    entry->icons[0] = -1;
  }

  f->pulse = (0.0f);
  f->second = (0.0f);
  f->first = (0.0f);
  f->speed = (-240.0f);
  f->locked = 0;
  fn_8022375C(self);

}
BottomBar::~BottomBar(void) {WidgetFragment* self=(WidgetFragment*)this;SelectResourcePrefix* f=(SelectResourcePrefix*)self;fn_802328D4(f->resource);f->resource=0;}
void BottomBar::Slot28(void) {SelectionDrawFieldsNew* f=(SelectionDrawFieldsNew*)((WidgetFragment*)this);float y,x,selectedX,half,shadow;x=f->scrollX;y=(float)((WidgetFragment*)this)->top;y=(240.0f)-y;selectedX=f->scrollX;shadow=(2.0f);half=(0.5f);
int i;for(i=0;i<f->count;i++){if(i==*f->selected){selectedX=x;{
 float w=f->widths[i];
 x+=f->spacing+w*half;
}if(i+1<f->count){
 float w=f->widths[i+1];
 x+=w*half;
}}else{fn_80232BA4(f->labels[i],f->resource,shadow+x,y-shadow,0xff000000u,12,f->scale,(0.0f),(1.0f));u32 color=fn_802236C8(((WidgetFragment*)this),i);fn_80232BA4(f->labels[i],f->resource,x,y,color,12,f->scale,(0.0f),(1.0f));{
 float w=f->widths[i];
 x+=f->spacing+w*half;
}if(i+1<f->count){
 float w=f->widths[i+1];
 x+=w*half;
}}}
i=*f->selected;int selected=i;
// Reconstructed unused int-to-float conversion work observed in the original.
// MWCC retains integer conversion setup/stores; the original spelling and purpose are unknown.
float screenWidth=(float)lbl_8055FB9C;float screenHeight=(float)lbl_8055FBA0;float left=selectedX-f->widths[selected]/2.0f-f->spacing/2.0f;left=4.0f+left;int textHeight=fn_80232B70(f->resource);float height=(2.0f)+f->scale*(float)textHeight;float width=f->spacing+f->widths[selected];
fn_80230924(0x96646464u,left,y-(2.0f),left+width,(y-height)-4.0f,(0.0f));
fn_80230924(0x96282828u,left,y-(2.0f),(2.0f)+left,(y-height)-4.0f,(0.0f));
fn_80230924(0x96282828u,left,y-(2.0f),left+width,y-4.0f,(0.0f));
fn_80230924(0x96aaaaaau,left,(y-height)-(2.0f),left+width,(y-height)-4.0f,(0.0f));
fn_80230924(0x96aaaaaau,(left+width)-(2.0f),y-(2.0f),left+width,(y-height)-4.0f,(0.0f));
fn_80232BA4(f->labels[selected],f->resource,4.0f+selectedX,y-4.0f,0xff000000u,12,f->scale,(0.0f),(1.0f));u32 color=fn_802236C8(((WidgetFragment*)this),selected);fn_80232BA4(f->labels[selected],f->resource,(2.0f)+selectedX,y-(2.0f),color,12,f->scale,(0.0f),(1.0f));}
void fn_802235B4(WidgetFragment* self){SelectBooleanNavigationRelativeNew* f=(SelectBooleanNavigationRelativeNew*)self;int* value=f->selected;int selected=*value;int bounds[2]={selected+1,f->count};int step=1;for(;bounds[0]<bounds[1];bounds[0]++,step++){if(((SelectBooleanNavigationRelativeNew*)((unsigned int)f+((unsigned int)selected+(unsigned int)step)))->available[0]){*value+=step;return fn_8022375C(self);}}for(int step=selected;step>0;step--){if(((SelectBooleanNavigationRelativeNew*)((char*)f+(unsigned int)(selected-step)))->available[0]){*value-=step;return fn_8022375C(self);}}}
void fn_80223644(WidgetFragment* self){NavigationPreviousNew* f=(NavigationPreviousNew*)self;int* value=f->selected;int selected=*value;for(int step=1;selected-step>=0;step++){if(((NavigationPreviousNew*)((char*)f+((unsigned int)selected-(unsigned int)step)))->available[0]){*value-=step;fn_8022375C(self);return;}}for(int step=f->count-selected-1;step>0;step--){if(((NavigationPreviousNew*)(((unsigned int)f+(unsigned int)selected)+(unsigned int)step))->available[0]){*value+=step;fn_8022375C(self);return;}}}
u32 fn_802236C8(WidgetFragment* self,int index){SelectionColorPrefix* f=(SelectionColorPrefix*)self;if(f->locked)return 0xe6969696u;u32 color=0xff9f9f9fu;if(f->flags[index])color=0xffffffffu;if(index==*f->selected&&(1.0f)==f->pulse)color=0xffffd03fu;return color;}
void BottomBar::Slot7C(void) {Slot7C__8UIWidgetFv(((WidgetFragment*)this));((SelectionPrefix*)((WidgetFragment*)this))->pulse=(1.0f);}
void fn_8022375C(WidgetFragment*) {}
int BottomBar::Slot64(int key) {if(((SelectionPrefix*)((WidgetFragment*)this))->locked)return 0;switch(key){case 0x125:fn_80223644(((WidgetFragment*)this));return 1;case 0x127:fn_802235B4(((WidgetFragment*)this));return 1;}return 0;}
void fn_802237D0(WidgetFragment* self){((SelectionPrefix*)self)->locked=1;}
void fn_802237DC(WidgetFragment* self){((SelectionPrefix*)self)->locked=0;}
int fn_802237E8(WidgetFragment* self){return ((SelectionPrefix*)self)->count;}
int fn_802237F0(WidgetFragment* self,int index){return ((SelectionPrefix*)self)->icons[index];}
BarTab::BarTab(int x,int y,void* model,int* watched) : BottomBar(x,y,(const StringList*)model,watched), watched(watched) {WidgetFragment* self=(WidgetFragment*)this;if(watched)fn_80219440(self,0,watched,4);fn_80219440(self,1,fn_80216708(),1);}
void BarTab::Slot0C(void) {((WidgetDispatch*)((WidgetFragment*)this))->Slot74(2,0);fn_802237DC(((WidgetFragment*)this));fn_8021818C(((WidgetFragment*)this)->window);}
void BarTab::Slot74(int mode, void*) {SelectWatchedPrefix* f=(SelectWatchedPrefix*)((WidgetFragment*)this);void* window;for(int i=0;i<fn_802237E8(((WidgetFragment*)this));i++){if(i==*f->watched){float duration=mode==2?(0.0f):(0.3f);window=((WidgetFragment*)this)->window;int item=fn_802237F0(((WidgetFragment*)this),i);fn_80218968(window,item,duration);}else{float duration=mode==2?(0.0f):(0.3f);window=((WidgetFragment*)this)->window;int item=fn_802237F0(((WidgetFragment*)this),i);fn_8021889C(window,item,duration);}}WidgetMarkDirty(((WidgetFragment*)this));}
int BarTab::Slot64(int key) {if(*fn_80216708()){switch(key){case 0x127:fn_802235B4(((WidgetFragment*)this));return 1;case 0x125:fn_80223644(((WidgetFragment*)this));return 1;}}return Slot64__8UIWidgetFi(((WidgetFragment*)this),key);}
void BarTab::Slot28(void) {if(*fn_80216708())fn_802237DC(((WidgetFragment*)this));else fn_802237D0(((WidgetFragment*)this));BottomBar::Slot28();}
int BarTab::Slot68(void) {return 1;}
MenuImage::MenuImage(WidgetDesc* desc, void* context) : UIWidget(desc) { WidgetFragment* self=(WidgetFragment*)this;ImageDescriptorPrefix* f=(ImageDescriptorPrefix*)self;fn_80230C30(&f->x,&f->y,desc->x,desc->y);f->value=desc->field9c;const char* name=(const char*)desc+0x40;if(name)f->image=gxGetImage(name);else f->image=0;f->width=(float)desc->dx;f->height=(float)desc->dx;f->alpha=(1.0f);f->asset=fn_8022EE9C();f->context=context;}
MenuImage::~MenuImage(void) {WidgetFragment* self=(WidgetFragment*)this;TwoCleanupPrefix* f=(TwoCleanupPrefix*)self;if(f->image)fn_802C519C(f->image);f->image=0;fn_8022EE98(f->asset);}
void MenuImage::Slot28(void) {
    WidgetFragment* self = (WidgetFragment*)this;ImageAlphaPrefix* f=(ImageAlphaPrefix*)self;f->asset->alpha=(1.0f)-f->blend;float zero=(0.0f);if(f->asset->alpha>zero&&f->image){FullDrawStatePrefix scope;const UIContext* context=fn_8022EDC4();fn_8022EEF8(&scope,f->asset,context);fn_8022ED00(&scope);fn_802C5320(f->image,0,f->x,f->y,zero,f->width,f->height);fn_8022EDB8();}}
void MenuImage::Slot7C(void) {
    WidgetFragment* self = (WidgetFragment*)this;TweenPrefix* f=(TweenPrefix*)self;if(f->selected==f->state[3])f->blend=(0.0f);else f->blend=(1.0f);Slot7C__8UIWidgetFv(self);}
WidgetFragment* fn_80223CF0(WidgetFragment* self,WidgetDesc* desc,const char* name){
 __ct__8UIWidgetFP10WidgetDesc(self,desc);LargeImage355* f=(LargeImage355*)self;self->vtable=lbl_8033E350;f->name=0;f->flag=0;f->resource=-2;f->image=0;
 if(!fn_802C9A58()&&((WidgetDispatch*)self)->Slot78()>600&&self->bottom-self->top>460){
 int left=lbl_8055CFF0/2+Round355(1.3333333f*(float)(self->left-lbl_8055CFF0/2));
 int top=lbl_8055CFF4/2+Round355((float)(self->top-lbl_8055CFF4/2));
 int width=Round355(1.3333333f*(float)((WidgetDispatch*)self)->Slot78());
 int height=Round355((float)(self->bottom-self->top));
 self->left=left;self->top=top;self->right=left+width;self->bottom=top+height;
 }
 fn_80230C30(&f->x,&f->y,self->left,self->top);f->width=(float)((WidgetDispatch*)self)->Slot78();f->height=(float)(self->bottom-self->top);
 if(name)f->name=name;else f->name=desc->name;bool flag=desc->fielda4!=0;f->frame=0;f->flag=flag;return self;
}
WidgetFragment* fn_80223FAC(WidgetFragment* self,int destroy){if(self){self->vtable=lbl_8033E350;RenderPrefix* f=(RenderPrefix*)self;if(f->resource)fn_802ECFF4(f->resource);__dt__8UIWidgetFv(self,0);if(destroy>0)__dl__FPv(self);}return self;}
void fn_80224028(WidgetFragment* self){Slot7C__8UIWidgetFv(self);RenderRequestPrefix* f=(RenderRequestPrefix*)self;if(!f->resource){if(f->name){f->resource=fn_802ECF08(f->name,!f->notify);if(!f->resource&&f->notify){if(!f->callback||f->callback(f->context))fn_802168BC(reinterpret_cast<int>(f->context));}}}else{if(f->notify&&fn_802ED040(f->resource)){if(!f->callback||f->callback(f->context))fn_802168BC(reinterpret_cast<int>(f->context));}if(f->name&&!fn_802ECF90(f->resource,f->name)){fn_802ECFF4(f->resource);fn_802E30C8("MovieWidget: loading '%s'",f->name);f->resource=fn_802ECF08(f->name,!f->notify);}}if(f->resource)fn_802ECFEC(f->resource);}
void fn_80224160(WidgetFragment* self){Slot80__8UIWidgetFv(self);RenderPrefix* f=(RenderPrefix*)self;if(f->resource)fn_802ECFEC(f->resource);}
void fn_8022419C(WidgetFragment* self){RenderPrefix* f=(RenderPrefix*)self;if(f->resource)fn_802ED038(f->resource,f->x,f->y,f->w,f->h);}
DynamicLoadBackground::DynamicLoadBackground(WidgetDesc* desc) : UIWidget(desc) { WidgetFragment* self=(WidgetFragment*)this;RenderPrefix* f=(RenderPrefix*)self;fn_80230C30(&f->x,&f->y,desc->x,desc->y);f->w=(float)desc->dx;f->h=(float)desc->dy;char path[256];sprintf(path,"*uibig.res/%s",desc->name);f->resource=gxGetImage(path);if(!f->resource)fn_802E30C8("Couldn't load the image %s",path);}
DynamicLoadBackground::~DynamicLoadBackground(void) {WidgetFragment* self=(WidgetFragment*)this;RenderPrefix* f=(RenderPrefix*)self;if(f->resource)fn_802C519C(f->resource);f->resource=0;}
void DynamicLoadBackground::Slot28(void) {
    WidgetFragment* self = (WidgetFragment*)this;RenderPrefix* f=(RenderPrefix*)self;if(f->resource)fn_802C5320(f->resource,0,f->x,f->y,(0.0f),f->w,f->h);}
FadeButton::FadeButton(void* context, WidgetDesc* desc, int (*callback)(void*) , int* frame) : UIWidget(desc) { WidgetFragment* self=(WidgetFragment*)this;ToggleImagePrefix* f=(ToggleImagePrefix*)self;fn_80230C30(&f->x,&f->y,desc->x,desc->y);f->width=(float)desc->dx;f->height=(float)desc->dy;f->active=0;f->pressed=0;f->mode=(u32)desc->field9c>>31;float alpha;if(f->mode==0)alpha=(0.0f);else alpha=(0.1f);f->blend=alpha;f->alpha=alpha;f->token=fn_80228F40(self->left,self->top);f->focus=0;f->callback=callback;f->context=context;f->speed=InlineWidgetAbsolute(desc->field9c);f->asset=fn_8022EE9C();f->scope=fn_8022EE9C();f->elapsed=(0.0f);f->frame=frame?*frame:0;char path[256];strcpy(path,(const char*)desc+0x40);f->image=gxGetImage(path);if(!f->image)fn_802E30C8("Couldn't load the image %s",path);}
FadeButton::~FadeButton(void) {WidgetFragment* self=(WidgetFragment*)this;ThreeCleanupPrefix* f=(ThreeCleanupPrefix*)self;if(f->image)fn_802C519C(f->image);f->image=0;fn_8022EE98(f->assetSecond);fn_8022EE98(f->assetFirst);}
void FadeButton::Slot14(void) {
    WidgetFragment* self = (WidgetFragment*)this;TimedFocusPrefix* f=(TimedFocusPrefix*)self;f->focus=fn_80217BD8(self->window,f->token,self);fn_80219440(self,0,f->focus,4);((WidgetDispatch*)self)->Slot74(0,0);}
void FadeButton::Slot7C(void) {
    WidgetFragment* self = (WidgetFragment*)this;ToggleImagePrefix* f=(ToggleImagePrefix*)self;if(fn_8023659C()<f->elapsed)f->elapsed=fn_8023659C();switch(f->mode){case 0:if(f->active){float wave=(0.5f)+(0.5f)*(float)sin((float)(f->speed+1)*(fn_8023659C()-f->elapsed));f->alpha=InlineWidgetMinimum((float)(f->speed+1)*(fn_8023659C()-f->elapsed),wave);}else if(f->pressed){if(f->alpha<(0.005f)){f->pressed=0;f->alpha=(0.0f);}else f->alpha=InlineWidgetMaximum(f->blend-(float)(f->speed+1)*(fn_8023659C()-f->elapsed),(0.0f));}break;case 1:if(f->active){float wave=0.45f+((0.1f)+0.45f*(float)sin((float)-f->speed*(fn_8023659C()-f->elapsed)));f->alpha=InlineWidgetMinimum((0.1f)+(float)f->speed*(fn_8023659C()-f->elapsed),wave);}else if(f->pressed){if(f->alpha<0.105000004f){f->pressed=0;f->alpha=(0.1f);}else f->alpha=InlineWidgetMaximum(f->blend-(float)f->speed*(fn_8023659C()-f->elapsed),(0.1f));}break;}Slot7C__8UIWidgetFv(self);}
void FadeButton::Slot74(int, void*) {
    WidgetFragment* self = (WidgetFragment*)this;TimedFocusPrefix* f=(TimedFocusPrefix*)self;if(!f->active&&f->token==*f->focus){f->inactive=0;f->active=1;}else if(f->active&&f->token!=*f->focus){f->active=0;f->inactive=1;}f->stamp=fn_8023659C();f->previous=f->value;}
void FadeButton::Slot28(void) {
    WidgetFragment* self = (WidgetFragment*)this;ToggleImagePrefix* f=(ToggleImagePrefix*)self;if(f->image){if(!self->enabled)f->asset->alpha=(0.5f);else if(f->active)f->asset->alpha=f->alpha;else f->asset->alpha=f->mode==1?(1.0f):(0.0f);fn_8022EEF8(f->scope,fn_8022EDC4(),f->asset);fn_8022ED00(f->scope);int frame,height;height=self->bottom-self->top;frame=f->frame%fn_802C51AC(f->image);fn_802C5320(f->image,frame,f->x,f->y,(0.0f),(float)((WidgetDispatch*)self)->Slot78(),(float)height);fn_8022EDB8();}}
int FadeButton::Slot64(int key) {
    WidgetFragment* self = (WidgetFragment*)this;ToggleImagePrefix* f=(ToggleImagePrefix*)self;if(!f->focus)return 0;if(f->token!=*f->focus)return 0;switch(key){case 13:if(f->callback&&!f->callback(f->context))return 1;fn_802168BC(reinterpret_cast<int>(f->context));return 1;case 0x126:fn_80217D84(self->window);return 1;case 0x128:fn_80217C4C(self->window);return 1;}return 0;}
int FadeButton::Slot6C(int key) {
    WidgetFragment* self = (WidgetFragment*)this;int* focus=(int*)self->unknown258;if(focus&&(int)self->label!=*focus)return 0;return key==13||(unsigned int)key==0x126||(unsigned int)key==0x128;}
TextBox::TextBox(int context, WidgetDesc* desc) : UIWidget(desc) { WidgetFragment* self=(WidgetFragment*)this;ResourceTextPrefix* f=(ResourceTextPrefix*)self;strcpy(f->text,"");f->localized=0;switch(desc->fielda0){case 0:f->localized=1;strcpy(f->text,"");break;case 1:{int second=fn_80129D90(1,1,8);int first=fn_80129D90(1,0x42,0x45);sprintf(f->text,"Continued on %c%d",first,second);break;}}if(context)f->context=context;else f->context=(int)fn_80131234((const char*)desc+0x20);f->resource=fn_80232578((const char*)desc+0x40);f->style=desc->style90;f->flags=desc->flags;f->scale=desc->scale/(float)fn_80232B70(f->resource);f->owned=0;f->asset=0;float x,y,z;memcpy(&x,(const char*)desc+0xa4,4);memcpy(&y,(const char*)desc+0xa8,4);memcpy(&z,(const char*)desc+0xac,4);if(!(ObservedAbsDecision(x)<=__float_epsilon[0]&&ObservedAbsDecision(y)<=__float_epsilon[0]&&ObservedAbsDecision(z)<=__float_epsilon[0]))fn_80224DDC(self,x,y,z);}
TextBox::~TextBox(void) {WidgetFragment* self=(WidgetFragment*)this;AssetFormatPrefix* f=(AssetFormatPrefix*)self;if(f->owned)fn_8022EE98(f->asset);fn_802328D4(f->resource);f->resource=0;f->first=0;strcpy(f->text,"");}
void fn_80224DDC(WidgetFragment* self,float x,float y,float z){ResourceTextPrefix* f=(ResourceTextPrefix*)self;f->owned=1;f->asset=fn_8022EE9C();RotationVec rotation={RotationScale360((0.017453292f),x),RotationScale360((0.017453292f),y),RotationScale360((0.017453292f),z)};RotationMatrix matrix;RotationMatrix* target=&f->asset->xf.basis;Mat3FromEuler(&matrix,&rotation);Mat3Mul(target,&matrix,target);}
extern "C" void fn_80232C50(const char*,GTextFont*,unsigned int,int,float,float,float,float,float,float);
static inline const char* NextResourceTextGlyphNew(const char* p){const unsigned char* u=(const unsigned char*)p;if((*u&0xf0)==0xe0)return p+3;if((*u&0xe0)==0xc0)return p+2;return p+1;}
// NonMatching: text origin uses F24 instead of the original F25 (four instruction words).
/* Helper name unproven */
struct ResourceOrigin{double y;};
void TextBox::Slot28(void) {


 ResourceTextPrefix* f=(ResourceTextPrefix*)((WidgetFragment*)this);char firstPass[512];char output[512];FullDrawStatePrefix scope;
 if(f->owned){fn_8022EEF8(&scope,f->asset,fn_8022EDC4());fn_8022ED00(&scope);}
 float width=(float)((WidgetDispatch*)((WidgetFragment*)this))->Slot78();int top=((WidgetFragment*)this)->top;float height=(float)(((WidgetFragment*)this)->bottom-top);const char* text=(const char*)f->context;strlen((const char*)text);
 float left=(float)((WidgetFragment*)this)->left;ResourceOrigin origin;origin.y=(float)top;
 if(f->flags&16){int lines=0;while(*text){++lines;const char* begin=text;memset(firstPass,0,512);char* destination=firstPass;
  do{const char* next=NextResourceTextGlyphNew(text);int n=next-text;for(int i=0;i<n;i++)*destination++=*text++;}while(f->scale*fn_8023293C(f->resource,firstPass,(1.0f))<width&&*text&&*text!='\n');
  const char* end=text;if(*text){while(*text!=' '&&*text!='\n'&&text>begin)--text;}if(text<=begin)text=end;if(*text==' '||*text=='\n')++text;
 }
 origin.y=(float)origin.y+(0.5f)*(float)(((WidgetFragment*)this)->bottom-((WidgetFragment*)this)->top);int lineHeight=fn_80232B70(f->resource);float fontHeight=(float)lineHeight;float scaledLineCount=((0.5f)*(float)(lines-1))*f->scale;origin.y=(float)origin.y-scaledLineCount*fontHeight;
 }
 int line=0;text=(const char*)f->context;
 while(*text){const char* begin=text;memset(output,0,512);int size=0;
  while(f->scale*fn_8023293C(f->resource,output,(1.0f))<width&&*text&&*text!='\n'){
   const char* next=NextResourceTextGlyphNew(text);int n=next-text;for(int i=0;i<n;i++)output[size++]=(char)*text++;
  }
  const char* end=text;int originalSize=size;if(*text){while(*text!=' '&&text!=begin&&*text!='\n'){--text;--size;}}if(text==begin){text=end;size=originalSize;}output[size]=0;if(*text==' '||*text=='\n')++text;
  for(int i=0;i<size;i++){if(output[i]=='^')output[i]=' ';}
  int screenHeight=lbl_8055FBA0;float projectedX=(float)lbl_8055FB9C/(-2.0f);float x=projectedX+left;int h=fn_80232B70(f->resource);float metric=f->scale*(float)h;float y=(float)line++*metric;y=(float)origin.y+y;y=(((float)screenHeight/2.0f))-y;
  if(!f->localized&&y<((float)screenHeight/2.0f)-(height+(float)((WidgetFragment*)this)->top)){fn_80232BA4(f->text,f->resource,x+width,y,f->style,10,f->scale-(0.1f),(0.0f),(1.0f));goto finish;}
  int flags=f->flags;if(flags&4)x+=(width/2.0f);
  if(flags&2)fn_80232C50(output,f->resource,f->style,9,x,y,f->scale,width,(0.0f),(1.0f));
  else fn_80232BA4(output,f->resource,x,y,f->style,flags,f->scale,(0.0f),(1.0f));

 }
 finish:if(f->owned)fn_8022EDB8();
}
ButtonList::ButtonList(RollingListData* list,WidgetDesc* desc) : UIWidget(desc), list(list) {WidgetFragment* self=(WidgetFragment*)this;RollingListFullPrefix* f=(RollingListFullPrefix*)self;f->resource=fn_80232578("uifnt.gtx");f->count=InlineIntegerMinimum(InlineIntegerMaximum(desc->field9c,1),3);f->x=(float)lbl_8055FB9C/(-2.0f);f->x+=(float)self->left;f->y=(float)lbl_8055FBA0/2.0f-(float)self->top;f->lineHeight=(5.0f)+(float)fn_80232B70(f->resource);f->period=f->lineHeight*(float)f->list->count;f->upper=f->y+(0.5f)*f->period;f->lower=f->y-(0.5f)*f->period;int row=0;f->rows=(RollingRow*)__nwa__FUlPCc(f->list->count*12,"button_info");for(int i=f->list->index;i<f->list->count;i++){f->rows[i].current=f->y-f->lineHeight*(float)row;if(f->rows[i].current<f->lower)f->rows[i].current+=f->period;float current=f->rows[i].current;f->rows[i].target=current;f->rows[i].previous=current;row++;}for(int i=0;i<f->list->index;i++){f->rows[i].current=f->y-f->lineHeight*(float)row;if(f->rows[i].current<f->lower)f->rows[i].current+=f->period;float current=f->rows[i].current;f->rows[i].target=current;f->rows[i].previous=current;row++;}f->direction=0;f->stamp=fn_8023659C();f->elapsed=(0.0f);f->nextStamp=fn_8023659C();f->focus=0;f->token=fn_80228F40(self->left,self->top);}
ButtonList::~ButtonList(void) {WidgetFragment* self=(WidgetFragment*)this;RollingRowsPrefix* f=(RollingRowsPrefix*)self;if(f->rows)__dla__FPv(f->rows);f->rows=0;if(((RollingListFullPrefix*)self)->resource)fn_802328D4(((RollingListFullPrefix*)self)->resource);((RollingListFullPrefix*)self)->resource=0;self->x=0;}
void ButtonList::Slot14(void) {Bound27c* f=(Bound27c*)((WidgetFragment*)this);f->focus=fn_80217BD8(((WidgetFragment*)this)->window,f->token,((WidgetFragment*)this));fn_80219440(((WidgetFragment*)this),1,f->focus,4);}
void ButtonList::Slot7C(void) {Slot7C__8UIWidgetFv(((WidgetFragment*)this));}
void fn_8022591C(WidgetFragment* self){RollingInterpolationPrefix* f=(RollingInterpolationPrefix*)self;if(f->direction){float now=fn_8023659C();if(f->stamp>now)f->stamp=now-0.2f;float elapsed=now-f->stamp;if(elapsed>0.2f){for(int i=0;i<f->list->count;i++){float target=f->rows[i].target;f->rows[i].current=target;f->rows[i].previous=target;}f->direction=0;}else{for(int i=0;i<f->list->count;i++){switch(f->direction){case 0:break;case 2:if(f->rows[i].target>f->rows[i].previous)f->rows[i].current=InlineWidgetInterpolate(f->rows[i].previous,f->rows[i].target,elapsed,0.2f);else if(f->rows[i].target<f->rows[i].previous){if(f->rows[i].current>f->rows[i].target)f->rows[i].current=InlineWidgetInterpolate(f->rows[i].previous,f->upper+(f->rows[i].target-f->lower),elapsed,0.2f);else if(f->rows[i].current<f->rows[i].target)f->rows[i].current=InlineWidgetInterpolate(f->lower-(f->upper-f->rows[i].previous),f->rows[i].target,elapsed,0.2f);}break;case 1:if(f->rows[i].target<f->rows[i].previous)f->rows[i].current=InlineWidgetInterpolate(f->rows[i].previous,f->rows[i].target,elapsed,0.2f);else if(f->rows[i].target>f->rows[i].previous){if(f->rows[i].current<f->rows[i].target)f->rows[i].current=InlineWidgetInterpolate(f->rows[i].previous,f->lower-(f->upper-f->rows[i].target),elapsed,0.2f);else if(f->rows[i].current>f->rows[i].target)f->rows[i].current=InlineWidgetInterpolate(f->upper+(f->rows[i].previous-f->lower),f->rows[i].target,elapsed,0.2f);}break;}if(f->rows[i].current>f->upper)f->rows[i].current-=f->period;else if(f->rows[i].current<f->lower)f->rows[i].current+=f->period;}}}}
void ButtonList::Slot28(void) {PalettePaintNew* arg0=(PalettePaintNew*)((WidgetFragment*)this);
    int var_r23;
    int *temp_r4;
    int temp_r0;
    int var_r14;
    unsigned int var_ctr;
    float *var_r18;
    RollingRow *temp_r16;
    const float *var_r4;
    unsigned int var_r6;
    unsigned int var_r5;
    int var_r7;
    unsigned char *var_r17;
    int temp_r21;
    int temp_r5;

    float var_f4_8;







    float var_f2;
    float var_f3;

    float var_f30;
    float var_f3_2;
    float temp_f1;
    float var_f29;
    float var_f28;
    float var_f27;
    float temp_f26;
    float temp_f25;
    float temp_f24;
    float temp_f2_2;
    float var_f5;

    float temp_f0;
    float temp_f0_2;










    fn_8022591C(((WidgetFragment*)this));
    if ((int) arg0->enabled == 0) {
        if (((float) __fabs(arg0->elapsed) <= *__float_epsilon) || (fn_8023659C() < arg0->nextStamp)) {
            arg0->nextStamp = fn_8023659C();
        }
        temp_f0 = arg0->elapsed + ((4.0f) * (fn_8023659C() - arg0->nextStamp));
        var_f3 = (1.0f);
        var_f3=MapMinBranchNew(var_f3,temp_f0);
        var_f3=MapMaxBranchNew(var_f3,(0.001f));
        arg0->elapsed = var_f3;
        arg0->nextStamp = fn_8023659C();
    } else {
        if (((float) __fabs(arg0->elapsed - (1.0f)) <= *__float_epsilon) || (fn_8023659C() < arg0->nextStamp)) {
            arg0->nextStamp = fn_8023659C();
        }
        temp_f0_2 = arg0->elapsed - ((4.0f) * (fn_8023659C() - arg0->nextStamp));
        var_f3_2 = (0.999f);
        var_f3_2=MapMinBranchNew(var_f3_2,temp_f0_2);
        var_f3_2=MapMaxBranchNew(var_f3_2,(0.0f));
        arg0->elapsed = var_f3_2;
        arg0->nextStamp = fn_8023659C();
    }


    
    
    
    
    
    

    
    
    
    
    
    
    float positions[9];PaletteFillCenters391(positions,arg0->y,arg0->lineHeight);
    
    
    
    
    float normal[9]={0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f};
    normal[0]=PaletteNaturalLerp(0.0f,0.15f,PaletteNaturalRemap((float)arg0->count,3.0f,4.0f));
    normal[1]=PaletteNaturalLerp(0.0f,0.15f,PaletteNaturalRemap((float)arg0->count,2.0f,3.0f));
    normal[2]=PaletteNaturalLerp(0.0f,0.35f,PaletteNaturalRemap((float)arg0->count,1.0f,3.0f));
    normal[3]=PaletteNaturalLerp(0.0f,0.75f,PaletteNaturalRemap((float)arg0->count,0.0f,3.0f));
    normal[5]=PaletteNaturalLerp(0.0f,0.75f,PaletteNaturalRemap((float)arg0->count,0.0f,3.0f));
    normal[6]=PaletteNaturalLerp(0.0f,0.35f,PaletteNaturalRemap((float)arg0->count,1.0f,3.0f));
    normal[7]=PaletteNaturalLerp(0.0f,0.15f,PaletteNaturalRemap((float)arg0->count,2.0f,3.0f));
    normal[8]=PaletteNaturalLerp(0.0f,0.15f,PaletteNaturalRemap((float)arg0->count,3.0f,4.0f));
    var_r23=0;
    unsigned char colorsA[9]={255,255,255,255,255,255,255,255,255};
    
    
    
    
    
    
    
    
    unsigned char colorsB[9]={255,255,255,255,128,255,255,255,255};
    
    
    
    
    
    
    
    
    float alternate[9]={0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f};
    
    
    
    
    
    
    
    
    temp_r4 = arg0->focus;
    if ((temp_r4 != 0) && ((int) arg0->token != (int) *temp_r4)) {
        var_r23 = 1;
    }
    temp_r0 = var_r23 == 0;
    var_r18 = (temp_r0 != 0 ? &normal[0] : &alternate[0]);
    var_r17 = (temp_r0 != 0 ? &colorsA[0] : &colorsB[0]);
    var_f29 = (1.0f);
    var_r14 = 0;
    var_f30 = (0.0f);

    for(;var_r14<(int)arg0->list->first;var_r14++){
    temp_f24 = (255.0f);
    temp_f25 = (1.5f);
    var_f27 = (3.0f);
    var_f28 = (0.5f);
        temp_r16 = arg0->rows;
        temp_f1 = temp_r16[var_r14].current;
        float radiusHeight391=arg0->lineHeight;float radiusProduct391=var_f27*radiusHeight391;float bound=var_f28+radiusProduct391;
        if (!((float)PaletteAbs391(temp_f1-arg0->y) > bound)) {
            var_f4_8 = (0.0f);
            var_r4=&positions[0];var_f2=var_f4_8;var_r6=0;var_r5=0;var_r7=0;
            for(var_ctr=9;var_ctr!=0;--var_ctr){if((temp_f1<=var_r4[0])&&(temp_f1>var_r4[1])){var_f4_8=var_r18[var_r7];var_r6=var_r17[var_r7];var_f2=var_r18[var_r7+1];var_r5=var_r17[var_r7+1];break;}
var_r4++;var_r7++;}
            temp_f2_2 = (&positions[0])[var_r7];
            float highPosition391=positions[var_r7+1];var_f5 = PaletteFractionObservedNew(temp_r16[var_r14].current,temp_f2_2,highPosition391);
            temp_f26 = var_f4_8 + (var_f5 * (var_f2 - var_f4_8));
            if(var_f29 < var_f5) var_f5=var_f29;
            var_f5=(var_f5>var_f30)?var_f5:var_f30;

            temp_r21 = (int) ((float) var_r6 + (var_f5 * ((float) var_r5 - (float) var_r6)));
            temp_r5 = 0xFF - (unsigned char) temp_r21;
            {unsigned int shadowColorWord=PaletteGrayColorObservedNew(temp_r5,temp_r5,temp_r5,(int)(temp_f24*temp_f26));fn_80233600(arg0->list->NameAt(var_r14), arg0->resource,shadowColorWord, 0xC, temp_f25 + arg0->x, temp_r16[var_r14].current - temp_f25, (0.0f));}
            {unsigned int foregroundColorWord=PaletteGrayColorObservedNew(temp_r21,temp_r21,temp_r21,(int)(temp_f24*temp_f26));fn_80233600(arg0->list->NameAt(var_r14), arg0->resource,foregroundColorWord, 0xC, arg0->x, arg0->rows[var_r14].current, (0.0f));}
        }
    }
}
int ButtonList::Slot64(int key) {RollingActionPrefix* f=(RollingActionPrefix*)((WidgetFragment*)this);if(f->focus&&f->token!=*f->focus)return 0;switch(key){case 0x127:if(f->list->vertical){fn_80217C4C(((WidgetFragment*)this)->window);return 1;}break;case 0x125:if(f->list->vertical){fn_80217D84(((WidgetFragment*)this)->window);return 1;}break;case 0x128:((CycleList*)f->list)->Next();fn_80226570(((WidgetFragment*)this),-1);f->direction=2;f->stamp=fn_8023659C();return 1;case 0x126:((CycleList*)f->list)->Prev();fn_80226570(((WidgetFragment*)this),1);f->direction=1;f->stamp=fn_8023659C();return 1;case 13:{void* context;int(*callback)(void*)=f->list->callback;context=(void*)((CycleList*)f->list)->Value();if(!callback||callback(context))fn_802168BC(reinterpret_cast<int>(context));return 1;}}return 0;}
int ButtonList::Slot6C(int key) {Bound27c* f=(Bound27c*)((WidgetFragment*)this);if(f->focus&&f->token!=*f->focus)return 0;return key==13||(unsigned int)key==0x126||(unsigned int)key==0x128;}
void fn_80226570(WidgetFragment* self,int delta){RollingRowsPrefix* f=(RollingRowsPrefix*)self;for(int i=0;i<f->list->count;i++){f->rows[i].previous=f->rows[i].current;f->rows[i].target-=f->speed*(float)delta;if(f->rows[i].target>f->upper)f->rows[i].target-=f->period;if(f->rows[i].target<f->lower)f->rows[i].target+=f->period;}}
WidgetFragment* fn_80226630(WidgetFragment* self,void* context,int x,int y,int dx,int dy,OwnedWidget* child,int(*callback)(void*)){fn_802224B4(self,context,x,y,dx,dy,child,callback,1,0);self->vtable=lbl_8033F7F4;Bound264* f=(Bound264*)self;f->token=fn_80228F40(x,y);f->focus=0;return self;}
WidgetFragment* fn_802266B0(WidgetFragment* self,int destroy){if(self){fn_802223C4(self,0);if(destroy>0)__dl__FPv(self);}return self;}
void fn_80226708(WidgetFragment* self){Bound264* f=(Bound264*)self;f->focus=fn_80217BD8(self->window,f->token,self);fn_80219440(self,1,f->focus,4);}
void fn_80226758(WidgetFragment* self,int,int){Bound264* f=(Bound264*)self;*f->focus=f->token;}
void fn_80226768(WidgetFragment* self,int,int){Bound264* f=(Bound264*)self;if(self->flag18){if(!f->callback||f->callback(f->context))fn_802168BC(reinterpret_cast<int>(f->context));}}
int fn_802267C4(WidgetFragment* self,int key){SelectionCallbackPrefix* f=(SelectionCallbackPrefix*)self;if(!f->focus)return 0;if(f->token!=*f->focus)return 0;switch(key){case 13:if(!f->callback||f->callback(f->context))fn_802168BC(reinterpret_cast<int>(f->context));return 1;case 0x126:fn_80217D84(self->window);fn_80230E90(0);return 1;case 0x128:fn_80217C4C(self->window);fn_80230E90(0);return 1;}return 0;}
int fn_802268A0(WidgetFragment* self,int key){Bound264* f=(Bound264*)self;if(f->focus&&f->token!=*f->focus)return 0;return key==13||(unsigned int)key==0x126||(unsigned int)key==0x128;}
int fn_802268E8(WidgetFragment* self){SelectionActionPrefix* f=(SelectionActionPrefix*)self;if(!f->focus)return 0;int selected=f->token==*f->focus;unsigned int state;if(self->flag18)state=(selected!=0)+2;else state=selected!=0;if(f->child->State()==3&&state==3)state=1;if(!self->enabled)state=4;return state;}
RotImage::RotImage(WidgetDesc* desc) : UIWidget(desc) { WidgetFragment* self=(WidgetFragment*)this;CenteredImagePrefix* f=(CenteredImagePrefix*)self;f->image=gxGetImage(desc->name);if(!f->image)fn_802E30C8("RotImage: couldn't find image %s!",desc->name);f->asset=fn_8022EE9C();fn_80230C30(&f->x,&f->y,self->left,self->top);f->height=(float)(self->bottom-self->top);f->width=(float)((WidgetDispatch*)self)->Slot78();f->asset->xf.translation.v[0]=f->x+f->width/2.0f;f->asset->xf.translation.v[1]=f->y-f->height/2.0f;f->value=desc->field9c;}
RotImage::~RotImage(void) {WidgetFragment* self=(WidgetFragment*)this;GXAssetImage244Prefix* f=(GXAssetImage244Prefix*)self;if(f->asset)fn_8022EE98(f->asset);f->asset=0;if(f->image)fn_802C519C(f->image);f->image=0;}
void RotImage::Slot28(void) {
    WidgetFragment* self = (WidgetFragment*)this;FullDrawStatePrefix scope;CenteredImagePrefix* f=(CenteredImagePrefix*)self;float time=fn_8023659C();UIContext* m=f->asset;float zero=(0.0f),one=(1.0f);SetMat3Row(&m->xf.basis,0,one,zero,zero);SetMat3Row(&m->xf.basis,1,zero,one,zero);SetMat3Row(&m->xf.basis,2,zero,zero,one);SetUIVec3(&m->xf.translation,zero,zero,zero);RotationVec rotation;SetVec3(&rotation,zero,zero,SpinAngle359(f->value,time/(30.0f)));RotationMatrix matrix;RotationMatrix* target=&f->asset->xf.basis;Mat3FromEuler(&matrix,&rotation);Mat3Mul(target,&matrix,target);f->asset->xf.translation.v[0]=f->x+f->width/2.0f;f->asset->xf.translation.v[1]=f->y-f->height/2.0f;if(f->image){fn_8022EEF8(&scope,f->asset,fn_8022EDC4());fn_8022ED00(&scope);fn_802C5320(f->image,0,-f->width/2.0f,f->height/2.0f,(0.0f),f->width,f->height);fn_8022EDB8();}}
ControlButton::ControlButton(WidgetDesc* desc) : UIWidget(desc) {WidgetFragment* self=(WidgetFragment*)this;EffectImage351* f=(EffectImage351*)self;
 f->height=(float)(self->bottom-self->top);f->width=(float)((WidgetDispatch*)self)->Slot78();
 f->mode=desc->field9c;if(f->mode<0)goto invalidMode;if(f->mode<=15)goto validMode;invalidMode:fn_802E30C8("Invalid type for ControlButtonImg! %d",f->mode);validMode:;
 if(f->mode==0)goto oldMode;if(f->mode!=1)goto newMode;oldMode:
 if(!lbl_8055EFBC){lbl_8055EFBC=1;fn_802E30C8("%s","ControlButtonImg HACK: TURNING 0 AND 1 INTO 4 AND 7; REMOVE THIS WHEN UI IS ALL UPDATED");}
 if(f->mode==0)f->mode=4;else f->mode=7;
 newMode:const char* name="controlwii.art";f->image=fn_8023512C(name);if(!f->image)fn_802E30C8("ControlButtonImg: Can't find image file %s!!!",name);
 if(f->image&&fn_80235FD8(f->image)!=15){fn_802E30C8("ControlButtonImg: Should be 16 images in %s, but I found %d!",name,fn_80235FD8(f->image));fn_80235380(f->image);f->image=0;}
 f->owned=0;f->matrix=0;float x,y,z;memcpy(&x,(const char*)desc+0xa4,4);memcpy(&y,(const char*)desc+0xa8,4);memcpy(&z,(const char*)desc+0xac,4);
 if(!(ObservedAbsDecision(x)<=__float_epsilon[0]&&ObservedAbsDecision(y)<=__float_epsilon[0]&&ObservedAbsDecision(z)<=__float_epsilon[0]))fn_80227044(self,x,y,z);
 }
ControlButton::~ControlButton(void) {WidgetFragment* self=(WidgetFragment*)this;EffectImage351* f=(EffectImage351*)self;if(f->owned)fn_8022EE98(f->matrix);if(f->image)fn_80235380(f->image);f->image=0;}
void ControlButton::Slot28(void) {
    WidgetFragment* self = (WidgetFragment*)this;FullDrawStatePrefix scope;/* The context stack retains this record through drawing and pop. */SceneResourcePrefix* f=(SceneResourcePrefix*)self;if(f->owned){const UIContext* context=fn_8022EDC4();fn_8022EEF8(&scope,f->asset,context);fn_8022ED00(&scope);}fn_80230C30(&f->x,&f->y,self->left,self->top);if(f->model&&f->frame>=0&&f->frame<15)fn_802354A0(f->model,f->frame,f->x,f->y,(0.0f),f->width,f->height,0xffffffffu);if(f->owned)fn_8022EDB8();}
void fn_80227044(WidgetFragment* self,float x,float y,float z){AssetRotationPrefix* f=(AssetRotationPrefix*)self;f->owned=1;f->matrix=fn_8022EE9C();RotationVec rotation={RotationScale360((0.017453292f),x),RotationScale360((0.017453292f),y),RotationScale360((0.017453292f),z)};RotationMatrix matrix;RotationMatrix* target=&f->matrix->xf.basis;Mat3FromEuler(&matrix,&rotation);Mat3Mul(target,&matrix,target);}
ColorBackground::ColorBackground(WidgetDesc* desc) : UIWidget(desc) {
  WidgetFragment* self=(WidgetFragment*)this;TimedImage349* f=(TimedImage349*)self;
 fn_80230C30(&f->x,&f->y,self->left,self->top);f->height=(float)(self->bottom-self->top);f->width=(float)((WidgetDispatch*)self)->Slot78();
 const char* name=(const char*)desc+0x40;f->resource=-1;if(name)f->resource=fn_802CAC28(name,0);
 if(f->resource==-2)goto failure; if(f->resource!=-1)goto loaded; failure:fn_802E30C8("Widget: ColorBackground can't find texture");loaded:;
 f->rate=(float)(u32)desc->field9c/(10.0f);f->stamp=fn_8023659C();f->asset=fn_8022EE9C();f->increasing=1;f->first=(0.0f);f->second=(0.0f);
 if(*(int*)((char*)desc+0xb0))f->reverse=1;else f->reverse=0;}
ColorBackground::~ColorBackground(void) {WidgetFragment* self=(WidgetFragment*)this;ResourceHandlePrefix* f=(ResourceHandlePrefix*)self;fn_8022EE98(f->asset);if(f->handle!=-2&&f->handle!=-1)gxForgetTexture(f->handle);f->handle=-1;}
void ColorBackground::Slot28(void) {
    WidgetFragment* source = (WidgetFragment*)this;
    TimedImageFields* image = (TimedImageFields*)source;
    RotationMatrix rotationMatrix;
    UIContext *resetPose;
    UIContext *firstPose;
    UIContext *rotationPose;
    UIContext *secondPose;
    int textureHandle;

    float blendStep;
    float increasedBlend;
    float decreasedBlend;
    float now;

    now = fn_8023659C();
    textureHandle = image->textureHandle;
    if ((textureHandle != -2) && (textureHandle != -1)) {
        blendStep = (now - image->lastUpdateTime) / image->blendDuration;
        if ((int) image->blendIncreasing != 0) {
            increasedBlend = image->blend + blendStep;
            image->blend = increasedBlend;
            if (increasedBlend > (1.0f)) {
                image->blend = (1.0f);
                image->blendIncreasing = 0;
            }
        } else {
            decreasedBlend = image->blend - blendStep;
            image->blend = decreasedBlend;
            if (decreasedBlend < (0.0f)) {
                image->blend = (0.0f);
                image->blendIncreasing = 1;
            }
        }
        resetPose = image->pose;
        resetPose->xf.basis.m[0][0] = (1.0f);
        resetPose->xf.basis.m[0][1] = (0.0f);
        resetPose->xf.basis.m[0][2] = (0.0f);
        resetPose->xf.basis.m[1][0] = (0.0f);
        resetPose->xf.basis.m[1][1] = (1.0f);
        resetPose->xf.basis.m[1][2] = (0.0f);
        resetPose->xf.basis.m[2][0] = (0.0f);
        resetPose->xf.basis.m[2][1] = (0.0f);
        resetPose->xf.basis.m[2][2] = (1.0f);
        resetPose->xf.translation.v[0] = (0.0f);
        resetPose->xf.translation.v[1] = (0.0f);
        resetPose->xf.translation.v[2] = (0.0f);
        TimedImageInputVector transformInput(lbl_8055FB9C,lbl_8055FBA0);
        RotationVec translatedBeforeRotation,translatedAfterRotation;
        firstPose = image->pose;
        TransformTimedImageVector(firstPose,&transformInput,&translatedBeforeRotation);
        firstPose->xf.translation.v[0] += translatedBeforeRotation.x;
        firstPose->xf.translation.v[1] += translatedBeforeRotation.y;
        firstPose->xf.translation.v[2] += translatedBeforeRotation.z;
        if ((int) image->rotateBackward != 0) {
            image->rotationAngle -= (now - image->lastUpdateTime) / (30.0f);
        } else {
            image->rotationAngle += (now - image->lastUpdateTime) / (25.0f);
        }
        transformInput.x = (0.0f);
        transformInput.y = (0.0f);
        transformInput.z = image->rotationAngle;
        rotationPose = image->pose;
        Mat3FromEuler((RotationMatrix*)&rotationMatrix, (RotationVec*)&transformInput);
        Mat3Mul(&rotationPose->xf.basis, &rotationMatrix, &rotationPose->xf.basis);
        transformInput.SetZYX((0.0f),(float)lbl_8055FBA0/2.0f,(float)lbl_8055FB9C/2.0f);
        secondPose = image->pose;
        TransformTimedImageVector(secondPose,&transformInput,&translatedAfterRotation);
        secondPose->xf.translation.v[0] += translatedAfterRotation.x;
        secondPose->xf.translation.v[1] += translatedAfterRotation.y;
        secondPose->xf.translation.v[2] += translatedAfterRotation.z;
        image->pose->alpha = (0.3f) + ((0.7f) * image->blend);
        fn_8022ED00(image->pose);
        fn_80231220(image->x,image->y,image->width,image->height,image->textureHandle,-1U,(0.0f),(0.0f),(1.0f),(1.0f));
        fn_80231220(image->x - (float) lbl_8055FB9C,image->y,image->width,image->height,image->textureHandle,-1U,(0.0f),(0.0f),(1.0f),(1.0f));
        fn_80231220(image->x,image->y - ((30.0f) + (float) lbl_8055FBA0),image->width,image->height,image->textureHandle,-1U,(0.0f),(0.0f),(1.0f),(1.0f));
        fn_80231220(image->x - (float) lbl_8055FB9C,image->y - ((30.0f) + (float) lbl_8055FBA0),image->width,image->height,image->textureHandle,-1U,(0.0f),(0.0f),(1.0f),(1.0f));
        fn_8022EDB8();
    }
    image->lastUpdateTime = now;
}
void fn_80227880(float x,float y,float width,float height){fn_80230924(0x40ffffffu,x,y,x+width,y-height,(0.0f));fn_80230924(0x99000000u,x,y,x+width,y-height,(0.0f));}
void fn_802278F0(gArtSet* image,float x,float y,float width,float height){if(image){
 float offset=(0.0f);int frame=8;
 while(offset<width){float tile=fn_80235EF0(image,frame);if(tile-(1.0f)<width-offset)fn_802353F4(image,frame,x+offset,y,(0.0f));else{float h=fn_80235F64(image,frame);fn_802354A0(image,frame,x+offset,y,(0.0f),width-offset,h,0xffffffffu);}offset+=tile-(1.0f);frame++;if(frame>=10)frame=8;}
 offset=(0.0f);frame=10;float h=fn_80235F64(image,10);float bottom=h+(y-height);
 while(offset<width){float tile=fn_80235EF0(image,frame);if(tile-(1.0f)<width-offset)fn_802353F4(image,frame,x+offset,bottom,(0.0f));else fn_802354A0(image,frame,x+offset,bottom,(0.0f),width-offset,h,0xffffffffu);offset+=tile-(1.0f);frame++;if(frame>=12)frame=10;}
 float vertical=(0.0f);frame=4;
 while(vertical<height){float tile=fn_80235F64(image,frame);if(tile-(1.0f)<height-vertical)fn_802353F4(image,frame,x,y-vertical,(0.0f));else{float w=fn_80235EF0(image,frame);fn_802354A0(image,frame,x,y-vertical,(0.0f),w,height-vertical,0xffffffffu);}vertical+=tile-(1.0f);frame++;if(frame>=6)frame=4;}
 float rightOffset=(0.0f);frame=4;float right=(x+width)-fn_80235EF0(image,4);
 while(rightOffset<height){float tile=fn_80235F64(image,frame);if(tile-(1.0f)<height-rightOffset)fn_802353F4(image,frame,right,y-rightOffset,(0.0f));else{float w=fn_80235EF0(image,frame);fn_802354A0(image,frame,right,y-rightOffset,(0.0f),w,height-rightOffset,0xffffffffu);}rightOffset+=tile-(1.0f);frame++;if(frame>=8)frame=6;}
 fn_802353F4(image,0,x-(2.0f),(2.0f)+y,(0.0f));
 float w=fn_80235EF0(image,1);fn_802353F4(image,1,((2.0f)+(x+width))-w,(2.0f)+y,(0.0f));
 float cornerY=(y-height-(2.0f))+fn_80235F64(image,2);fn_802353F4(image,2,x-(2.0f),cornerY,(0.0f));
 y=(y-height-(2.0f))+fn_80235F64(image,3);w=fn_80235EF0(image,3);fn_802353F4(image,3,((2.0f)+(x+width))-w,y,(0.0f));
 }}
void fn_80227C9C(gArtSet* self,float x,float y,float width,float height){fn_80227880(x,y,width,height);fn_802278F0(self,x,y,width,height);}
TabBox::TabBox(WidgetDesc* desc) : UIWidget(desc) { WidgetFragment* self=(WidgetFragment*)this;gArtSet* image=fn_8023512C("tab_box.art");((ArtImage244Prefix*)self)->image=image;if(!image)fn_802E30C8("TabItemBox: unable to load box artwork");}
TabBox::~TabBox(void) {WidgetFragment* self=(WidgetFragment*)this;ArtImage244Prefix* f=(ArtImage244Prefix*)self;if(f->image)fn_80235380(f->image);f->image=0;}
void TabBox::Slot28(void) {
    WidgetFragment* self = (WidgetFragment*)this;float x,y;fn_80230C30(&x,&y,self->left,self->top);float width=(float)((WidgetDispatch*)self)->Slot78();fn_80227C9C(((ArtImage244Prefix*)self)->image,x,y,width,(float)(self->bottom-self->top));}
TopTabDisplay::TopTabDisplay(void* first,WidgetDesc* desc,void* second) : UIWidget(desc), first(first) {WidgetFragment* self=(WidgetFragment*)this;ThreeResourcePrefix* f=(ThreeResourcePrefix*)self;f->second=second;f->flag=*(const int*)((const char*)desc+0xb0)==1;f->resource=fn_80232578("uifnt.gtx");if(!f->resource)fn_802E30C8("TopTabDisplay: unable to load font");f->image=fn_8023512C("top_tab.art");if(!f->image)fn_802E30C8("TopTabDisplay: unable to load tab artwork");f->overlay=fn_8023512C("tab_box.art");if(!f->overlay)fn_802E30C8("TabItemBox: unable to load box artwork");}
TopTabDisplay::~TopTabDisplay(void) {WidgetFragment* self=(WidgetFragment*)this;TripleResourcePrefix* f=(TripleResourcePrefix*)self;if(f->resource)fn_802328D4(f->resource);f->resource=0;if(f->first)fn_80235380(f->first);f->first=0;if(f->second)fn_80235380(f->second);f->second=0;}
// Observed tabs drawing view only; not a complete class or allocation-size claim.
struct NameTabsPaintShared {void* vtable;int left,top,right,bottom,kind;unsigned char flag18,flag19,shown,enabled;char gap1c[0x228];unsigned char fixedOrigin;char gap245[3];NameList* list;const char* title;GTextFont* font;gArtSet* frame;gArtSet* panel;};
static inline const char* NameTabsTitleShared(const NameTabsPaintShared* f,int selected){if(f->title)return f->title;return f->list->Name(selected);}
// NonMatching: current clean draw remains26 original-address instruction words off.
void TopTabDisplay::Slot28(void) {
 float bottom;
 float firstWidth;
 float lastWidth;
 float frameHeight;
 float negativeHeight;
 float x;
 float y;
 float nextX;
 float measured;
 float width;
 float rowY;
 float textX;
 float textY;
 float left;
 float available;
 float panelY;
 float leftWidth;
 float rightWidth;
 float capWidth;
 float bodyWidth;
 float bodyHeight;
 float titleX;

 NameTabsPaintShared* const f=(NameTabsPaintShared*)((WidgetFragment*)this);
 if(!f->font || !f->frame || !f->panel)return;
 firstWidth = fn_80235EF0(f->frame,0);
 lastWidth = fn_80235EF0(f->frame,2);
 frameHeight = fn_80235F64(f->frame,1);
 int selected=f->list->Unk44();
 negativeHeight = -frameHeight;
 x = (-259.0f);
 y = (207.0f);
 if(!f->fixedOrigin){fn_80230C30(&x,&y,f->left,f->top);x-=(-32.0f);y-=negativeHeight;}
 int count=f->list->Count();
 nextX = (2.0f)+x;
 bool multiple=count>1;
 if(multiple){
  for(int index=0;index<count;index++){
   bool active=index==selected;
   const char* text=f->list->Name(index);
   measured = fn_8023293C(f->font,text,(1.0f));
   width = (0.0f)>measured?(0.0f):measured;
   rowY = y;
   fn_802353F4(f->frame,0,nextX,rowY,(0.0f));
   nextX+=firstWidth;
   fn_802354A0(f->frame,1,nextX,rowY,(0.0f),width,frameHeight,-1U);
   textX = nextX+(width/2.0f);
   textY = rowY-(frameHeight/2.0f);
   unsigned int color=active?-1U:0xFF707070U;
   fn_80232BA4(text,f->font,textX,textY,color,0x14,(1.0f),(0.0f),(1.0f));
   nextX+=width;
   fn_802353F4(f->frame,2,nextX,rowY,(0.0f));
   nextX+=((9.0f)+lastWidth);
  }
 }
 bottom=y+negativeHeight;
 left = (-32.0f)+x;
 int availableInteger=((WidgetDispatch*)((WidgetFragment*)this))->Slot78();
 available = (float)availableInteger;
 panelY = (y-frameHeight)-(7.0f);
 leftWidth = fn_80235EF0(f->frame,3);
 rightWidth = fn_80235EF0(f->frame,4);
 capWidth = multiple?(leftWidth+rightWidth):(0.0f);
 bodyWidth = available-capWidth;
 bodyHeight = fn_80235F64(f->frame,5);
 fn_80227880(left,y-frameHeight,available,(float)(f->bottom-f->top));
 nextX=left;
 if(multiple){fn_802353F4(f->frame,3,left,panelY,(0.0f));nextX=left+leftWidth;}
 fn_802354A0(f->frame,5,nextX,panelY,(0.0f),bodyWidth,bodyHeight,-1U);
 titleX = nextX+((0.5f)*bodyWidth);
 const char* title=NameTabsTitleShared(f,selected);
 fn_80232BA4(title,f->font,titleX,((5.0f)+panelY)-((0.5f)*bodyHeight),-1U,0x14,(1.2f),(0.0f),(1.0f));
 if(multiple){nextX+=bodyWidth;fn_802353F4(f->frame,4,nextX,panelY,(0.0f));}
 fn_802278F0(f->panel,left,bottom,available,(float)(f->bottom-f->top));
}
UISW2Movie::UISW2Movie(WidgetDesc* desc,SwfAnim* movie) : UIWidget(desc), animation(0) {
 owned=0;
 if(movie)animation=movie;
 else{animation=new("SW2Movie")SwfAnim;owned=1;}
 animation->Load((const char*)desc+0x40);
}
UISW2Movie::~UISW2Movie(void) {
 animation->Unload();if(owned)delete animation;
 animation=0;
}
void UISW2Movie::Slot28(void){animation->Draw();}
void UISW2Movie::Slot7C(void){animation->Update();}
DynoTexWidget::DynoTexWidget(WidgetDesc* desc,DynoTex* texture,int* frame) : UIWidget(desc) {
 this->texture=texture;this->frame=frame;
}
void DynoTexWidget::Slot28(void) {
 float x,y;fn_80230C30(&x,&y,((WidgetFragment*)this)->left,((WidgetFragment*)this)->top);
 DynoWidgetPrefix* fields=(DynoWidgetPrefix*)((WidgetFragment*)this);int frame;if(fields->frame)frame=*fields->frame;else frame=0;
 if(fields->texture){int width=((WidgetDispatch*)((WidgetFragment*)this))->Slot78();fields->texture->Draw(frame,(int)x,(int)y,width,((WidgetFragment*)this)->bottom-((WidgetFragment*)this)->top);}
 else{int width=((WidgetDispatch*)((WidgetFragment*)this))->Slot78();fn_80230924(0x8FAFBFCFu,x,y,x+(float)width,y-(float)(((WidgetFragment*)this)->bottom-((WidgetFragment*)this)->top),(0.0f));}
}
ModelWidget::ModelWidget(WidgetDesc* desc,const char* name) : UIWidget(desc), name(name), target(0) {
 owned=0;memset(lastName,0,32);
 ModelView* view=new ("def_ui_model") ModelView(desc);
 target=view;owned=1;
}

ModelWidget::ModelWidget(WidgetDesc* desc,MenuCustomView* target,const char* name) : UIWidget(desc), name(name) {
 this->target=target;owned=0;
 memset(lastName,0,32);
}
ModelWidget::~ModelWidget(void) {
 if(owned){delete target;target=0;}
}
void ModelWidget::Slot0C(void){((AdapterPrefix*)this)->target->Enter();}
void MenuCustomView::Reset(void){}
void ModelWidget::Slot7C(void){
 AdapterPrefix* fields=(AdapterPrefix*)((WidgetFragment*)this);
 if(fields->name&&strcasecmp(fields->lastName,fields->name)!=0){
  const char* source=fields->name;int remaining=32;char* destination=fields->lastName;
  while((*destination++=*source++)!=0){if(--remaining<=0){destination[-1]=0;break;}}
  fields->target->Name(fields->lastName);
 }
 fn_80228AEC(((WidgetFragment*)this),fn_80214D48());
}
void ModelWidget::Slot80(void){fn_80228AEC((WidgetFragment*)this,fn_80214D48());}
void ModelWidget::Slot2C(void){((AdapterPrefix*)this)->target->Leave();}
void MenuCustomView::Unk18(void){}
void ModelWidget::Slot28(void){
 AdapterPrefix* fields=(AdapterPrefix*)((WidgetFragment*)this);Viewport saved(fields->left,fields->top,fields->right,fields->bottom);
 fn_802C88BC(2);fields->target->Draw();
}
void fn_80228AEC(WidgetFragment *source, float opacity) {MapViewportReceiverNew* viewport=(MapViewportReceiverNew*)source;
    MapRendererNew *renderer;
    float scaledWidth;
    float scaledHeight;
    float roundedWidth;
    float roundedHeight;
    int height;
    int displayWidth;
    int clippedHeight;

    int mapped_x=(int)MapScopeNew((float)lbl_8055CFF0,(float)viewport->left,lbl_8056357C);viewport->x=MapIntegerMinNew(mapped_x,lbl_8055CFF0-2);
    int mapped_y=(int)MapScopeNew((float)lbl_8055CFF4,(float)viewport->top,lbl_80563580);viewport->y=MapIntegerMinNew(mapped_y,lbl_8055CFF4-2);
    displayWidth = lbl_8055CFF0;
    scaledWidth = ((float) displayWidth * (float) ((WidgetDispatch*)source)->Slot78()) / lbl_8056357C;
    if (scaledWidth < (0.0f)) {
        roundedWidth = scaledWidth - (0.5f);
    } else {
        roundedWidth = (0.5f) + scaledWidth;
    }
int boundWidth=displayWidth-viewport->x;int resultWidth=(int)roundedWidth;if(resultWidth<boundWidth)boundWidth=resultWidth;viewport->width=boundWidth;
    scaledHeight = ((float) lbl_8055CFF4 * (float) (viewport->bottom - viewport->top)) / lbl_80563580;
    if (scaledHeight < (0.0f)) {
        roundedHeight = scaledHeight - (0.5f);
    } else {
        roundedHeight = (0.5f) + scaledHeight;
    }
    clippedHeight = lbl_8055CFF4 - viewport->y;
    height = (int) roundedHeight;
    if (height < clippedHeight) {
        clippedHeight = height;
    }
    viewport->height = clippedHeight;
    renderer = viewport->renderer;
    ((MapRendererDispatchCppNew*)renderer)->Draw( viewport->x, viewport->y, viewport->width, clippedHeight, opacity);
}
int ModelWidget::Slot64(int){return ((AdapterPrefix*)this)->target->Flags();}
int MenuCustomView::Unk20(void){return 0;}
void ModelWidget::Slot10(void){((AdapterPrefix*)this)->target->State();}
void MenuCustomView::Unk24(void){}
void fn_80228DC4(ObservedClipPrefix* context,void* image,int x,int y,int width,int){fn_802C3034(reinterpret_cast<gxCanvas*>(context));ObservedClipRect saved;fn_80218E98(context,&saved);fn_802C51C0(image);int height=fn_802C5220(image);int bottom=y+height;int middle=x+width/2;fn_80218EE0(context,x,y,middle,bottom);fn_80218EBC(context,&saved);fn_80218EE0(context,middle,y,x+width,bottom);fn_80218EBC(context,&saved);}
void fn_80228E80(ObservedClipPrefix* context,void* image,int x,int y,int height,int){fn_802C3034(reinterpret_cast<gxCanvas*>(context));ObservedClipRect saved;fn_80218E98(context,&saved);int width=fn_802C51C0(image);int imageHeight=fn_802C5220(image);int middle=y+height/2;fn_80218EE0(context,x,y,x+imageHeight,middle);fn_80218EBC(context,&saved);fn_80218EE0(context,x,middle,x+width,y+height);fn_80218EBC(context,&saved);}
int fn_80228F40(int x,int y){return x+1000*y;}

WidgetFragment* fn_80228FA4(WidgetFragment* self,int destroy){if(self){__dt__8UIWidgetFv(self,0);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_80228FFC(WidgetFragment* self,int destroy){if(self){__dt__8UIWidgetFv(self,0);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_80229054(WidgetFragment* self,int destroy){if(self){ObservedNullable_fn_8021915C(self);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_802290B0(WidgetFragment* self,int destroy){if(self){ObservedNullable_fn_8021915C(self);if(destroy>0)__dl__FPv(self);}return self;}

WidgetFragment* fn_80229168(WidgetFragment* self,int destroy){if(self){ObservedNullable_fn_8021915C(self);if(destroy>0)__dl__FPv(self);}return self;}



WidgetFragment* fn_802292D8(WidgetFragment* self,int destroy){if(self){__dt__8UIWidgetFv(self,0);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_80229330(WidgetFragment* self,int destroy){if(self){fn_802223C4(self,0);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_80229388(WidgetFragment* self,int destroy){if(self){fn_802223C4(self,0);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_802293E0(WidgetFragment* self,int destroy){if(self){ObservedNullable_fn_8021915C(self);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_8022943C(WidgetFragment* self,int destroy){if(self){ObservedNullable_fn_8021915C(self);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_80229498(WidgetFragment* self,int destroy){if(self){ObservedNullable_fn_8021915C(self);if(destroy>0)__dl__FPv(self);}return self;}

WidgetFragment* fn_80229550(WidgetFragment* self,int destroy){if(self){ObservedNullable_fn_802223C4(self);if(destroy>0)__dl__FPv(self);}return self;}






void fn_80229628(WidgetFragment* self,int,int){((CallbackPrefix*)self)->active=1;((WidgetDispatch*)self)->Slot84();}
void fn_80229640(WidgetFragment* self,int,int){
 ((WidgetDispatch*)self)->Slot84();CallbackPrefix* fields=(CallbackPrefix*)self;fields->active=0;
 if(self->flag18&&fields->context!=(void*)-66666){if(!fields->callback||fields->callback(fields->context))fn_802168BC(reinterpret_cast<int>(fields->context));}
}
int fn_802296C0(WidgetFragment* self){
 CallbackPrefix* fields=(CallbackPrefix*)self;u32 result;
 if(fields->context!=(void*)-66666){if(self->flag18)result=2+(fields->active!=0);else result=0;}else result=1;
 if(((OwnedWidgetPrefix*)self)->child->Slot1c()==3&&result==3)result=1;return result;
}
void fn_80229750(WidgetFragment* self){((WidgetDispatch*)self)->Slot84();self->flag18=1;}
void fn_80229790(WidgetFragment* self){((WidgetDispatch*)self)->Slot84();self->flag18=0;}
void fn_802297D0(WidgetFragment* self){
 CallbackPrefix* fields=(CallbackPrefix*)self;
 if(fields->flag257&&fields->active&&self->flag18){if(!fields->callback||fields->callback(fields->context))fn_802168BC(reinterpret_cast<int>(fields->context));}
 Slot7C__8UIWidgetFv(self);
}





int fn_802298DC(WidgetFragment* self){int* value=((IntWatchPrefix*)self)->value;if(value)return *value;return 0;}


void fn_80229900(WidgetFragment* self){((WidgetDispatch*)self)->Slot74(0,0);}
void fn_80229918(WidgetFragment* self){((WidgetDispatch*)self)->Slot84();self->flag18=1;}
void fn_80229958(WidgetFragment* self){((WidgetDispatch*)self)->Slot84();self->flag18=0;}
void fn_80229998(WidgetFragment* self,int,int){LeafStoredValue* fields=(LeafStoredValue*)self;if(fields->destination)*fields->destination=fields->value;}
int fn_802299B0(WidgetFragment* self){int result=0;if(self->flag18)result=2;int selected=0;int* value=(int*)self->label;if(value&&*value==(int)self->unknown258)selected=1;return (selected!=0)|result;}
void fn_802299FC(WidgetFragment* self){const char* label;int height=self->bottom-self->top;label=((WidgetDispatch*)self)->Slot88();int width=((WidgetDispatch*)self)->Slot78();DelegatePaintPrefix* f=(DelegatePaintPrefix*)self;f->child->Paint(f->first,f->second,f->third,width,height,label);}
void fn_80229A94(WidgetFragment* self){self->flag19=1;WidgetMarkDirty(self);((PointerEventDispatch*)self)->Slot3c(self->left,self->top);}
void fn_80229AC4(WidgetFragment* self,int,int){LeafStoredValue* fields=(LeafStoredValue*)self;if(fields->destination)*fields->destination=fields->value;}
int fn_80229ADC(WidgetFragment*){return 1;}
int fn_80229AE4(WidgetFragment* self,int x,int y){LeafChildPrefix* fields=(LeafChildPrefix*)self;return fields->child->HitTest(fields->first,fields->second,x,y);}
void fn_80229B0C(WidgetFragment* self,int dx,int dy){Slot20__8UIWidgetFii(self,dx,dy);self->y+=dx;self->width+=dy;}


WidgetFragment* fn_80229B78(WidgetFragment* self,int destroy){if(self){self->vtable=lbl_8033EFF4;fn_801E4A2C(((RichCaptionPrefix*)self)->renderer);__dt__8UIWidgetFv(self,0);if(destroy>0)__dl__FPv(self);}return self;}
void fn_80229BE8(WidgetFragment* self){Slot7C__8UIWidgetFv(self);RichCaptionPrefix* f=(RichCaptionPrefix*)self;const char* label=f->label;if(strcmp(label,f->text)!=0){strcpy(f->text,label);int width=fn_801E4A6C(f->renderer,f->label);int oldWidth=((WidgetDispatch*)self)->Slot78();if(oldWidth>width)width=oldWidth;int height=fn_801E4BA8(f->renderer);fn_80219284(self,width,height);WidgetMarkDirty(self);}}



void fn_80229CB4(WidgetFragment* self,int,int){LeafActivity* fields=(LeafActivity*)self;fields->active=1;fields->elapsed=(0.0f);WidgetMarkDirty(self);}
void fn_80229CD8(WidgetFragment* self,int,int){((LeafActivity*)self)->active=0;WidgetMarkDirty(self);}
void fn_80229CF8(WidgetFragment* self){self->flag18=1;((LeafActivity*)self)->elapsed=(0.0f);WidgetMarkDirty(self);}
void fn_80229D1C(WidgetFragment* self){self->flag18=0;WidgetMarkDirty(self);}
int fn_80229D3C(WidgetFragment* self,int x,int y){return fn_802C5280(((LeafResource250*)self)->resource,x-self->left,y-self->top,0);}



void fn_80229DC8(WidgetFragment* self,int x,int y){
 RowListPrefix* fields=(RowListPrefix*)self;fields->direction=0;fields->active=1;((RowInputDispatch*)self)->Slot4c(x,y);
}
void fn_80229DE8(WidgetFragment* self,int x,int y){
 int height=self->bottom-self->top;RowListPrefix* fields=(RowListPrefix*)self;fields->direction=0;fields->active=1;((RowInputDispatch*)self)->Slot4c(x,y-height-3);
}
void fn_80229E1C(WidgetFragment* self,int x,int y){fn_8021E1C8(self,x,y-(self->bottom-self->top)-3);}
void fn_80229E34(WidgetFragment* self){RowListPrefix* fields=(RowListPrefix*)self;SetListViewport(fields->view,fields->view->start+1);}
void fn_80229E74(WidgetFragment* self){RowListPrefix* fields=(RowListPrefix*)self;SetListViewport(fields->view,fields->view->start-1);}
void fn_80229EB4(WidgetFragment* self,int,int){RowListPrefix* fields=(RowListPrefix*)self;fields->active=0;fields->direction=0;}
int fn_80229EC4(WidgetFragment* self){return 1;}
int fn_80229ECC(WidgetFragment* self){return 1;}
void fn_80229ED4(WidgetFragment* self){WidgetMarkDirty(self);self->flag18=1;}
void fn_80229EF4(WidgetFragment* self){WidgetMarkDirty(self);self->flag18=0;}
void* fn_80229F14(WidgetFragment* self){return ((TextEntryPrefix*)self)->first;}
void fn_80229F1C(WidgetFragment* self){self->flag18=1;WidgetMarkDirty(self);}
void fn_80229F38(WidgetFragment* self){self->flag18=0;WidgetMarkDirty(self);}
void* fn_80229F58(WidgetFragment* self){return(char*)self+0x254;}
int fn_80229F60(WidgetFragment* self){int result=0;if(self->flag18)result=2;return result;}











void fn_8022A594(){RegisterModuleTag__FP9ModuleTag(lbl_8055EFB0);StartupSharedPrefixNew* settings=&lbl_80490920;settings->kind=123;settings->fifth=settings->fourth=settings->third=settings->first=0;lbl_8055EFB8=0x7f000000;}
}

// Allocation/offset guards for observed storage views, not recovered class declarations.
typedef char CustomTextObservedAllocation[sizeof(RotatingChoiceFieldsNew)==0x374?1:-1];
typedef char CustomTextResourceOffset[((unsigned long)&((RotatingChoiceFieldsNew*)0)->resource)==0x368?1:-1];
typedef char CustomTextModeOffset[((unsigned long)&((RotatingChoiceFieldsNew*)0)->mode)==0x36c?1:-1];
typedef char CustomTextContextOffset[((unsigned long)&((RotatingChoiceFieldsNew*)0)->pose)==0x370?1:-1];
typedef char ColumnObservedAllocation[sizeof(PageTextPrefix)==0x264?1:-1];
typedef char ColumnRowHeightOffset[((unsigned long)&((PageTextPrefix*)0)->rowHeight)==0x250?1:-1];
typedef char ColumnListOffset[((unsigned long)&((PageTextPrefix*)0)->list)==0x254?1:-1];
typedef char ResourceTextObservedAllocation[sizeof(ResourceTextPrefix)==0x2a0?1:-1];

// Unit-owned globals (only declared extern above), in address order.
StartupSharedPrefixNew lbl_80490920;
KeyDetect lbl_80490934;
unsigned long lbl_8055EFB0[2];
unsigned long lbl_8055EFB8;
unsigned char lbl_8055EFBC;
