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
struct WidgetDispatch {virtual ~WidgetDispatch();virtual void Slot0c();virtual void Slot10();virtual void Slot14();virtual void Slot18();virtual void Slot1c(int,int);virtual void Slot20();virtual void Slot24(void*);virtual void Slot28();virtual void Slot2c();virtual void Slot30();virtual void Slot34();virtual void Slot38();virtual void Slot3c(int,int);virtual void Slot40(int,int);virtual void Slot44();virtual void Slot48();virtual void Slot4c();virtual void Slot50();virtual void Slot54();virtual void Slot58();virtual void Slot5c();virtual void Slot60();virtual int Slot64(int);virtual void Slot68();virtual void Slot6c();virtual void Slot70();virtual void Slot74(int,void*);virtual int Slot78();virtual void Slot7c();virtual void Slot80();virtual int Slot84();virtual const char* Slot88();};
struct OwnedWidget {virtual ~OwnedWidget();virtual void Slot0c();virtual void Slot10();virtual void Slot14();virtual void Slot18();virtual int Slot1c();};
struct CallbackPrefix {char prefix[0x254];unsigned char active,unknown255,flag256,flag257;int(*callback)(void*);void* context;};
struct OwnedWidgetPrefix {char prefix[0x244];OwnedWidget* child;int x,y,kind;};
struct ImagePrefix {void* vtable;void* image;};
struct ResourcePrefix {void* vtable;int resource;const char* text;};
struct WidgetProperty {void* value;int key,bytes;void* buffer;};
struct WidgetDesc {u32 unknown0;int flags;int x,y,kind,dx,dy;float scale;char unknown20[0x20];char name[1];char unknown41[0x4f];int style90,field94,field98,field9c,fielda0,fielda4;};
struct IntWatchPrefix {char prefix[0x254];int* value;};
struct FlagTextPrefix {char prefix[0x254];unsigned char* flag;};
struct WidgetFragment {
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
// Prefix only. The original string storage begins at +0x268; its capacity
// and the complete allocation size have not been recovered.
typedef char WidgetPrefixSizeCheck[(sizeof(WidgetFragment)==0x268)?1:-1];
extern "C" {
void fn_80227880(float,float,float,float);
void fn_802235B4(WidgetFragment*);void fn_80223644(WidgetFragment*);
extern u32 lbl_8033FA28[],lbl_8033F20C[],lbl_8033FB38[];
extern const float lbl_805634A0,lbl_805634A4,lbl_805634A8,lbl_805634AC,lbl_805634B0,lbl_805634B4;
float fn_80214D48();
int fn_8021525C();void __dla__FPv(void*);
void fn_80218FEC(WidgetFragment*,int,int,int,int,int);
WidgetFragment* fn_80218F34(WidgetFragment*,int,int,int,int,int);
WidgetFragment* fn_8021915C(WidgetFragment*,int);
void fn_80219718(WidgetFragment*);
void __dl__FPv(void*);
char* strcpy(char*,const char*);
WidgetFragment* fn_80219568(WidgetFragment* self,int x,int y,int width,int height,int dx,int dy);
WidgetFragment* fn_80219900(WidgetFragment* self,int destroy);
WidgetFragment* fn_8021B2B0(WidgetFragment* self,int x,int y,const char* label,int height,int* value,int width);
int fn_8021B3A0(WidgetFragment* self);
WidgetFragment* fn_8021B404(WidgetFragment* self,int destroy);

WidgetFragment* fn_80218F34(WidgetFragment* self,int left,int top,int right,int bottom,int kind);
WidgetFragment* fn_80218F80(WidgetFragment* self,WidgetDesc* desc);
void fn_80218FEC(WidgetFragment* self,int left,int top,int right,int bottom,int kind);
WidgetFragment* fn_8021915C(WidgetFragment* self,int destroy);

void fn_80219044(WidgetFragment*);
int memcmp(const void*,const void*,unsigned long);
void* memcpy(void*,const void*,unsigned long);
void* __nwa__FUlPCc(unsigned long,const char*);
extern char lbl_8033D878[];
void fn_802179A0(void*);void fn_80218198(void*,void*);void fn_80217AC0(void*);
void fn_802191F0(WidgetFragment* self,int x,int y);
void fn_80219264(WidgetFragment* self,int x,int y);
void fn_80219284(WidgetFragment* self,int width,int height);
void fn_802192A0(WidgetFragment*,void*);
int fn_802192A4(WidgetFragment*,int);
void fn_802192AC(WidgetFragment* self);
void fn_80219354(WidgetFragment* self);
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

void fn_802C3034(void*);
void fn_8022B91C(int,int,int,const char*,u32);
void fn_8022BAD0(int,int,int,int,int,int,const char*,u32,unsigned char,float);
void fn_8022AC1C(int,const char*,int,int,int*,int*,int*,int*);
inline int WidgetMinInt(int value,int limit){if(value<limit)return value;return limit;}
inline int WidgetMaxInt(int value,int limit){if(value>limit)return value;return limit;}
void fn_802195DC(WidgetFragment* self,void* context);
int fn_80219658(WidgetFragment*);
void fn_80219660(WidgetFragment* self);
void fn_80219718(WidgetFragment* self);

extern u32 lbl_8033F768[],lbl_8033F4B8[],lbl_8033F434[];
WidgetFragment* fn_80222340(WidgetFragment*,int,int,int,int,int,OwnedWidget*);
WidgetFragment* fn_802223C4(WidgetFragment*,int);
void fn_80219AB4(WidgetFragment*,int,const char*,int,int,int,bool);
WidgetFragment* fn_80219870(WidgetFragment* self,int x,int y,const char* text,unsigned char* flag,int height);
int fn_80219958(WidgetFragment* self,int key);
WidgetFragment* fn_8021995C(WidgetFragment* self,int x,int y,int width,OwnedWidget* value,void* watched);
WidgetFragment* fn_802199D0(WidgetFragment* self,int destroy);
WidgetFragment* fn_80219A28(WidgetFragment* self,WidgetDesc* desc,const char* text,int value);

void fn_8022984C(WidgetFragment* self,int,int);
void fn_80229864(WidgetFragment* self);
void fn_80229884(WidgetFragment* self);
char* fn_802298A4(WidgetFragment* self);
int fn_802298AC(WidgetFragment* self);
int fn_802298DC(WidgetFragment* self);

extern u32 lbl_8033F90C[];
WidgetFragment* fn_80222340(WidgetFragment* self,int x,int y,int kind,int dx,int dy,OwnedWidget* child);
WidgetFragment* fn_802223C4(WidgetFragment* self,int destroy);
void fn_80222450(WidgetFragment* self);
void fn_80222468(WidgetFragment* self,int x,int y);

extern u32 lbl_8033F880[];
void fn_802168BC(void*);
WidgetFragment* fn_802224B4(WidgetFragment* self,void* context,int x,int y,int dx,int dy,OwnedWidget* child,int(*callback)(void*),unsigned char a,unsigned char b);
void fn_80229628(WidgetFragment* self,int,int);
void fn_80229640(WidgetFragment* self,int,int);
int fn_802296C0(WidgetFragment* self);

void fn_80229750(WidgetFragment* self);
void fn_80229790(WidgetFragment* self);
void fn_802297D0(WidgetFragment* self);
extern u32 lbl_8033F9E0[];
int fn_80232578(const char*);void fn_802328D4(int);
void fn_80230C30(float*,float*,int,int);
void fn_802336AC(int,int,float,float,float,int,const char*,...);
float fn_8023293C(int,const char*,float);int fn_80232B70(int);
extern const char lbl_8055B8A4[3];
ResourcePrefix* fn_80222530(ResourcePrefix* self,const char* path,const char* text);
ResourcePrefix* fn_80222584(ResourcePrefix* self,int destroy);
void fn_802225E8(ResourcePrefix* self,int x,int y);
void fn_8022264C(ResourcePrefix* self,int first,int second,int* left,int* top,int* right,int* bottom);

extern u32 lbl_8033F9BC[];
void* fn_8023512C(const char*);void fn_80235380(void*);
void fn_802353F4(void*,int,float,float,float);
float fn_80235EF0(void*,int);float fn_80235F64(void*,int);
ImagePrefix* fn_802226D4(ImagePrefix* self,const char* path);
ImagePrefix* fn_80222718(ImagePrefix* self,int destroy);
void fn_80222788(ImagePrefix* self,int x,int y,int unused,int frame);
void fn_80222800(ImagePrefix* self,int first,int second,int* left,int* top,int* right,int* bottom);
void fn_80229610(ResourcePrefix*);
int fn_80229614(ResourcePrefix*);
void fn_8022961C(ImagePrefix*);
int fn_80229620(ImagePrefix*);
}


struct AnimationPrefix {void* vtable;void* handle;float width,height;};
class AnimationBoundsDispatch {
public:
 virtual void Slot00();virtual void Slot04();virtual void Slot08();
 virtual void Bounds(int,int,int*,int*,int*,int*);
};
extern "C" {
extern u32 lbl_8033F998[];

void* fn_802C5130(const char*);int fn_802C51C0(void*);int fn_802C5220(void*);
AnimationPrefix* fn_802228B0(AnimationPrefix* self,const char* path,int width,int height);

void fn_802C519C(void*);int fn_802C51AC(void*);
void fn_802C5320(void*,int,float,float,float,float,float);
AnimationPrefix* fn_80222998(AnimationPrefix* self,int destroy);
int fn_802229FC(AnimationPrefix* self);
void fn_80222A3C(AnimationPrefix* self,int x,int y,int unused,u32 frame);
void fn_80222AFC(AnimationPrefix* self,int first,int second,int* left,int* top,int* right,int* bottom);
int fn_80222B48(AnimationPrefix* self,int x,int y,int testX,int testY);
int fn_80222BD4(AnimationPrefix* self);
}

extern "C" {
// Opaque method identities only; held bodies are not reconstructed.
void fn_80215828();void fn_8021583C();
u32 lbl_8033F998[9]={0,0,(u32)fn_80222998,(u32)fn_80215828,(u32)fn_80222A3C,(u32)fn_80222AFC,(u32)fn_80222B48,(u32)fn_80222BD4,(u32)fn_802229FC};
u32 lbl_8033F9BC[9]={0,0,(u32)fn_80222718,(u32)fn_80215828,(u32)fn_80222788,(u32)fn_80222800,(u32)fn_8021583C,(u32)fn_80229620,(u32)fn_8022961C};
u32 lbl_8033F9E0[9]={0,0,(u32)fn_80222584,(u32)fn_80215828,(u32)fn_802225E8,(u32)fn_8022264C,(u32)fn_8021583C,(u32)fn_80229614,(u32)fn_80229610};
}


struct PlaybackPrefix {
 char base[0x244];void* active;int resource;float amount;
 unsigned char flag250,flag251,pad252[2];int option;
};
typedef char PlaybackPrefixSizeCheck[(sizeof(PlaybackPrefix)==0x258)?1:-1];
extern "C" {
extern u32 lbl_8033F3B0[];
void fn_8022FFE8(void*);void fn_8022F8CC(void*);void fn_8022F824(void*);
WidgetFragment* fn_8021A5B8(WidgetFragment* self,int resource,unsigned char a,unsigned char b,int option,float amount);
WidgetFragment* fn_8021A650(WidgetFragment* self,int destroy);
void fn_8021A6A8(WidgetFragment* self);
void fn_8021A744(WidgetFragment* self);
}


struct WatchedChildPrefix {char base[0x254];void* watched;int kind;};
extern "C" {
extern u32 lbl_8033F324[],lbl_8033F298[];
void* fn_80216708();
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
void fn_80219510(WidgetFragment* self,int x,int y);
void fn_8021952C(WidgetFragment* self,int x,int y);
}


extern "C" {
extern const float lbl_805634E4,lbl_805634E8;
void* fn_8022F638(int,int,int,int,float);
void fn_80218968(void*,int,float);void fn_8021889C(void*,int,float);
void fn_8021A6EC(WidgetFragment* self);
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
void fn_802C5310(void*,int,float,float,float);
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


class SwfAnim {char observedAllocation[0x18];public:SwfAnim();void Load(const char*);void Unload();void Draw();void Update();};
class DynoTex {public:void Draw(int,int,int,int,int);};
void* operator new(unsigned long,const char*);
struct SwfWidgetPrefix {char base[0x244];SwfAnim* animation;unsigned char owned;};
struct DynoWidgetPrefix {char base[0x244];DynoTex* texture;int* frame;};
extern "C" {
extern u32 lbl_8033DE28[],lbl_8033DDA4[];extern const char lbl_8033DCDC[];
SwfAnim* __dt__7SwfAnimFv(SwfAnim*,int);
void fn_80230924(u32,float,float,float,float,float);
WidgetFragment* fn_80228418(WidgetFragment* self,WidgetDesc* desc,SwfAnim* animation);
WidgetFragment* fn_802284BC(WidgetFragment* self,int destroy);
void fn_8022854C(WidgetFragment* self);
void fn_80228554(WidgetFragment* self);
WidgetFragment* fn_8022855C(WidgetFragment* self,WidgetDesc* desc,DynoTex* texture,int* frame);
void fn_802285B8(WidgetFragment* self);
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
extern u32 lbl_8033ED44[];extern const char lbl_8033D924[],lbl_8033D934[];
WidgetFragment* fn_8021CA0C(WidgetFragment* self,int x,int y,int dx,int dy,ObservedValueProvider* provider);
WidgetFragment* fn_8021CAD4(WidgetFragment* self,int destroy);
}


extern "C" {
void fn_8021CD04(WidgetFragment* self,int x,int y);
void fn_8021CE8C(WidgetFragment* self,int x,int y);
void fn_8021CEAC(WidgetFragment* self,int x,int y);
}


struct NumericCaptionPrefix {char base[0x254];float scale,bias;int* value;const char* format;char text[1];};
typedef char NumericCaptionPrefixSize[(sizeof(NumericCaptionPrefix)==0x268)?1:-1];
extern "C" {
extern u32 lbl_8033ECB8[];
int sprintf(char*,const char*,...);void fn_80130568(char*);
void fn_8021CF9C(WidgetFragment*);
WidgetFragment* fn_8021CEEC(WidgetFragment* self,int x,int y,int* value,const char* format,int height,float scale,float bias);
void fn_8021CF9C(WidgetFragment* self);
void fn_8021D040(WidgetFragment* self,int,void*);
}


struct ChoiceWidgetPrefix {char base[0x254];int* value;int count;int first,second,style;char unknown268[0x50];const char** choices;int* focus;int token;};
typedef char ChoiceWidgetPrefixSize[(sizeof(ChoiceWidgetPrefix)==0x2c4)?1:-1];
extern "C" {
extern u32 lbl_8033EBA0[];
int fn_80129BC4(WidgetFragment*);int* fn_80217BD8(void*,int,WidgetFragment*);
void fn_8021D154(WidgetFragment* self);
int fn_8021D1B0(WidgetFragment* self);
WidgetFragment* fn_8021D300(WidgetFragment* self,int destroy);
}


struct ChoiceWindowPrefix {char unknown[0x494];int depth;};
extern "C" {
void fn_80217D84(void*);void fn_80217C4C(void*);
int fn_8021D1F0(WidgetFragment* self,int key);
}


extern "C" {
extern const char lbl_8033D940[];
unsigned int fn_8022D1B4(const char*);
unsigned int strlen(const char*);int fn_80228F40(int,int);void fn_8021D61C(WidgetFragment*);
WidgetFragment* fn_8021D044(WidgetFragment* self,int x,int y,int* value,const char* list,int height,int first,int second,int style);
}


extern "C" {
extern const char lbl_8055B87C[4];
void fn_8021D614(WidgetFragment* self,int,int);
void fn_8021D618(WidgetFragment* self,int,void*);
void fn_8021D61C(WidgetFragment* self);
}


struct ChoiceDrawStatePrefix {char unknown[0x34];float scale;};
extern "C" {
void* fn_8021165C(int,int);const ChoiceDrawStatePrefix* fn_8022EDC4();
void fn_80219660(WidgetFragment*);
void fn_8021D378(WidgetFragment *self);

extern int lbl_8055FBC0;
void fn_802C3424(int,int,int,int,int);
void fn_8021D568(WidgetFragment* self,void* context);
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
extern u32 lbl_8033EC2C[];float fn_8023659C();void fn_80216D28();void fn_80230E90(int);
WidgetFragment* fn_8021D7C8(WidgetFragment* self,int x,int y,int dx,int dy,int height,ChoiceProviderDispatch* provider);
const char* fn_8021D864(WidgetFragment* self);
void fn_8021D878(WidgetFragment* self);
int fn_8021D8D4(WidgetFragment* self);
int fn_8021D97C(WidgetFragment* self,int key);
}


struct ChoiceResourceSpec {u32 words[22];};
struct ChoiceResourcePrefix {char base[0x2c4];int resource;};
extern "C" {
extern u32 lbl_8033EB14[];int fn_8022A86C(const ChoiceResourceSpec*);void fn_8022A8E4(int);
WidgetFragment* fn_8021D6A0(WidgetFragment* self,int x,int y,int* value,const char* list,int first,int second,int third,int fourth);
WidgetFragment* fn_8021D758(WidgetFragment* self,int destroy);
}


struct ObservedStringList {u32 unknown[2];int count,version;};
struct ObservedListViewport {int total,page,start;};
struct RowListPrefix {char base[0x244];ObservedStringList* list;ObservedListViewport* view;int unknown24c,columns,rows,rowHeight;int* selected;int direction;float elapsed;unsigned char active;};
static inline void SetListViewport(ObservedListViewport* view,int position){
 view->start=position;int limit=view->total-view->page;if(position>limit)view->start=limit;
 if(view->start<0)view->start=0;
}
static inline void SetListViewportValue(int position,ObservedListViewport* view){SetListViewport(view,position);}
extern "C" {
extern u32 lbl_8033EA90[];extern const char lbl_8055B880[2];
extern const float lbl_80563504,lbl_80563510;
int fn_8022B620(int,const char*);void fn_8021E7D4(WidgetFragment*);
void fn_802192AC(WidgetFragment*);
WidgetFragment* fn_8021E0C0(WidgetFragment *self, int x, int y, int width, int height, ObservedStringList *list, int *selected, ObservedListViewport *view);
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
extern int lbl_8055FBDC;
const char* fn_8022D3F8(ObservedStringList*,int,int*);
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
WidgetFragment* fn_80223208(WidgetFragment*,int);
}
static inline void ObservedNullable_fn_8021915C(WidgetFragment* self){if(self)fn_8021915C(self,0);}
static inline void ObservedNullable_fn_802223C4(WidgetFragment* self){if(self)fn_802223C4(self,0);}
extern "C" {
WidgetFragment* fn_80228F4C(WidgetFragment* self,int destroy);
WidgetFragment* fn_80228FA4(WidgetFragment* self,int destroy);
WidgetFragment* fn_80228FFC(WidgetFragment* self,int destroy);
WidgetFragment* fn_80229054(WidgetFragment* self,int destroy);
WidgetFragment* fn_802290B0(WidgetFragment* self,int destroy);
WidgetFragment* fn_8022910C(WidgetFragment* self,int destroy);
WidgetFragment* fn_80229168(WidgetFragment* self,int destroy);
WidgetFragment* fn_802291C4(WidgetFragment* self,int destroy);
WidgetFragment* fn_80229220(WidgetFragment* self,int destroy);
WidgetFragment* fn_8022927C(WidgetFragment* self,int destroy);
WidgetFragment* fn_802292D8(WidgetFragment* self,int destroy);
WidgetFragment* fn_80229330(WidgetFragment* self,int destroy);
WidgetFragment* fn_80229388(WidgetFragment* self,int destroy);
WidgetFragment* fn_802293E0(WidgetFragment* self,int destroy);
WidgetFragment* fn_8022943C(WidgetFragment* self,int destroy);
WidgetFragment* fn_80229498(WidgetFragment* self,int destroy);
WidgetFragment* fn_802294F4(WidgetFragment* self,int destroy);
WidgetFragment* fn_80229550(WidgetFragment* self,int destroy);
WidgetFragment* fn_802295AC(WidgetFragment* self,int destroy);

// Placeholders for the original functions not yet implemented above (generated by Claude for
// the provisional code-only widgets unit). Empty bodies; they are not decompilations.
void fn_80219AB4(WidgetFragment*,int,const char*,int,int,int,bool);  // placeholder (912 bytes)
void fn_80219FE0(WidgetFragment*);
}
struct RotationVec {float x,y,z;};struct RotationMatrix {float m[9];};
extern "C" {void fn_80015A20(RotationMatrix*,const RotationVec*);void Mat3Mul__FP4Mat3PC4Mat3PC4Mat3(RotationMatrix*,const RotationMatrix*,const RotationMatrix*);}
struct ChoicePoseNew {RotationMatrix basis;RotationVec translation;};
struct RotatingChoiceFieldsNew {char base[0x368];int resource;unsigned char mode;char pad36d[3];void* pose;};
struct RotatingChoiceDescriptorNew {u32 first;int flags,x,y,width,height,kind;float scale;char name[0x64];u32 option84,gap88,gap8c,option90,option94,gap98,gap9c,gapa0;float rx,ry,rz;};
extern "C" {extern u32 lbl_8033F180[];extern const float lbl_805634F0,lbl_805634F4,lbl_805634F8;void* fn_8022EDE4(float,const void*);}
static inline float ChoiceDotContractNew(float x,float y,float z,float a,float b,float c){return z*c+(x*a+y*b);}
static inline void ChoiceTransformVectorNew(const RotationMatrix* matrix,const RotationVec* input,RotationVec* out){const float* m=matrix->m;if(out==input){float first,second,third;first=ChoiceDotContractNew(input->x,input->y,input->z,m[0],m[3],m[6]);second=ChoiceDotContractNew(input->x,input->y,input->z,m[1],m[4],m[7]);third=ChoiceDotContractNew(input->x,input->y,input->z,m[2],m[5],m[8]);out->x=first;out->y=second;out->z=third;}else{out->x=ChoiceDotContractNew(input->x,input->y,input->z,m[0],m[3],m[6]);out->y=ChoiceDotContractNew(input->x,input->y,input->z,m[1],m[4],m[7]);out->z=ChoiceDotContractNew(input->x,input->y,input->z,m[2],m[5],m[8]);}}

extern "C" {
static inline void ChoiceApplyOffsetNew(ChoicePoseNew* pose,const RotationVec* input){RotationVec translated;ChoiceTransformVectorNew(&pose->basis,input,&translated);pose->translation.x+=translated.x;pose->translation.y+=translated.y;pose->translation.z+=translated.z;}
WidgetFragment* fn_8021B460(WidgetFragment* self,RotatingChoiceDescriptorNew* desc,const char* label,int* value);
}

extern "C" {

int fn_8021E620(WidgetFragment* self,int key);  // placeholder (436 bytes)
void fn_8021F4F0(void);  // placeholder (392 bytes)
}
struct SteppedFloatPrefix {char base[0x244];float* value;float minimum,maximum,step;void* image;int* focus;int token;float previousTime,nextTime;};
typedef char SteppedFloatPrefixObservedSize268[sizeof(SteppedFloatPrefix)==0x268?1:-1];
extern "C" {
extern float lbl_80558DC4[];extern const float lbl_80563508,lbl_8056350C,lbl_805634C8;
void* fn_80211660(int);float fn_80235EF0(void*,int);float fn_80235F64(void*,int);void fn_802354A0(void*,int,u32,float,float,float,float,float);void fn_802C5334(void*,int,float,float,float,float,float,float,float,float,float);
}
static inline float ObservedProviderClamp(float value){float one=(1.0f);float limited=one<value?one:value;float zero=(0.0f);return limited>zero?limited:zero;}
static inline float ObservedArrowOffset(const float* stamp){float now=fn_8023659C();float fraction=PaletteNaturalRemap(0.3f+*stamp-now,0.0f,0.3f);return PaletteNaturalLerp(0.0f,10.0f,fraction);}
static inline u32 ObservedArrowColor(const float* stamp){float now=fn_8023659C();float fraction=PaletteNaturalRemap(now-*stamp,0.0f,0.15f);int alpha=(int)PaletteNaturalLerp(255.0f,0.0f,fraction);return((u32)alpha<<24)|0x00ffffffu;}
// Read-only half-coefficient accessor; original helper boundary is unproved.
static inline float GaugeHalfReferenceObservedNew(){return 0.5f;}
extern "C" {
void fn_802200AC(WidgetFragment* self);
void fn_80220F90(WidgetFragment*,int,int);  // placeholder (1044 bytes)

struct DragViewportPrefix {char base[0x244];ObservedListViewport* state;void* first;void* second;bool dragging;char gap251[3];int direction,grabX,grabY;};
struct ScrollLocalWordObservedNew {int value;inline ScrollLocalWordObservedNew(){}inline ScrollLocalWordObservedNew(int v):value(v){}};typedef char ScrollLocalWordObservedSize4[sizeof(ScrollLocalWordObservedNew)==4?1:-1];
struct ScrollEdgeWordObservedNew {int value;inline ScrollEdgeWordObservedNew(int v):value(v){}};typedef char ScrollEdgeWordObservedSize4[sizeof(ScrollEdgeWordObservedNew)==4?1:-1];
extern int lbl_8055FBE0,lbl_8055FBFC;void fn_80228E80(ObservedClipPrefix*,void*,int,int,int);void fn_80228DC4(ObservedClipPrefix*,void*,int,int,int,int);
void fn_80221544(WidgetFragment *self, void *context);
extern const char lbl_8033D9F8[];void fn_802E30C8(const char*,...);
void fn_80221970(WidgetFragment* self);

void fn_80221F9C(void);  // placeholder (388 bytes)
extern "C" {void fn_8022ED00(void*);void fn_8022EDB8();void fn_8022EE98(void*);}
typedef struct CircularVectorTypedNew {float v[3];} CircularVectorTypedNew;
typedef struct CircularPoseTypedNew {RotationMatrix basis;CircularVectorTypedNew translation;float alpha,first,second;} CircularPoseTypedNew;
typedef struct CircularListTypedNew {int version;void* entries;int count;} CircularListTypedNew;
typedef struct CircularPaintNew {void* vtable;int left,top,right,bottom,kind;char gap18[0x22c];ObservedStringList* list;int font;unsigned int color;float radius,rate;} CircularPaintNew;


extern "C" {
extern const float lbl_805634D8,lbl_80563528;extern const char lbl_8055B8A8[3];void* fn_8022EDE4(float,const void*);void fn_80232BA4(const char*,int,float,float,unsigned int,int,float,float,float);
void fn_80222CFC(WidgetFragment *source);

}

WidgetFragment* fn_802230A4(WidgetFragment* self,int x,int y,void* list,int* selected);  // placeholder (356 bytes)
extern "C" {u32 fn_802236C8(WidgetFragment*,int);extern int lbl_8055FB9C,lbl_8055FBA0;}
struct SelectionDrawFieldsNew {char base[0x244];const char* labels[33];int count;int* selected;unsigned char available[33];char pad2f1[3];float widths[33];int icons[33];int resource;float spacing,scale,scrollX,first,second,pulse;};
extern "C" {extern const float lbl_805634EC,lbl_805634F4,lbl_80563534;}
static inline float SelectionDrawTopNew(const WidgetFragment* self){return (240.0f)-(float)self->top;}
extern "C" {
// Selection half widths use ordinary load-before-product locals.
void fn_80223280(WidgetFragment* self);
}

void fn_8022375C(WidgetFragment*);  // placeholder (4 bytes)

struct LargeImage355 {char base[0x244];float x,y,width,height;int frame;const char* name;bool flag;char pad25d[3];void* image;int resource;};
extern u32 lbl_8033E350[];extern int lbl_8055CFF0,lbl_8055CFF4;extern const float lbl_80563538;
int fn_802C9A58();
inline int Round355(float value){return (int)(value<(0.0f)?value-(0.5f):(0.5f)+value);}
WidgetFragment* fn_80223CF0(WidgetFragment* self,WidgetDesc* desc,const char* name);  // placeholder (700 bytes)
void fn_80224E74(void);  // placeholder (1736 bytes)
void fn_80225B6C(void);  // placeholder (2144 bytes)
void fn_80226B6C(WidgetFragment*);  // placeholder (336 bytes)


typedef struct TimedImagePose {RotationMatrix basis;RotationVec translation;float alpha;} TimedImagePose;
typedef struct TimedImageFields {char base[0x244];float x,y,width,height;int textureHandle;float blendDuration,lastUpdateTime;TimedImagePose* pose;unsigned char blendIncreasing;char unknown265[3];float blend,rotationAngle;unsigned char rotateBackward;} TimedImageFields;
static inline void TransformTimedImageVector(const TimedImagePose* pose,const RotationVec* transformInput,RotationVec* out){ChoiceTransformVectorNew(&pose->basis,transformInput,out);}
extern const float lbl_805634FC,lbl_80563560;void fn_80231220(float,float,float,float,int,unsigned int,float,float,float,float);
struct TimedImageInputVector:RotationVec{inline TimedImageInputVector(int screenWidth,int screenHeight){z=(0.0f);x=(float)-screenWidth/2.0f;y=(float)-screenHeight/2.0f;} inline void SetZYX(float zValue,float yValue,float xValue){z=zValue;x=xValue;y=yValue;}};typedef char TimedImageInputVectorSize12[sizeof(TimedImageInputVector)==12?1:-1];
void fn_8022ED00(void*);void fn_8022EDB8();
void fn_802272D4(WidgetFragment* source);
void fn_802278F0(void*,float,float,float,float);
void fn_80228010(void);  // placeholder (1032 bytes)
void fn_80228AEC(WidgetFragment*,float);
}
extern "C" {void fn_80015B20(void*,const RotationMatrix*,void*);double sin(double);}
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
 const float* m=mm->basis.m;
 if(out==v){
  float x=ModelDot3(v->x,m[0],v->y,m[3],v->z,m[6]);
  float y=ModelDot3(v->x,m[1],v->y,m[4],v->z,m[7]);
  float z=ModelDot3(v->x,m[2],v->y,m[5],v->z,m[8]);
  out->x=x;out->y=y;out->z=z;
 }else{
  out->x=ModelDot3(v->x,m[0],v->y,m[3],v->z,m[6]);
  out->y=ModelDot3(v->x,m[1],v->y,m[4],v->z,m[7]);
  out->z=ModelDot3(v->x,m[2],v->y,m[5],v->z,m[8]);
 }
}
inline float ModelFitBoundsNew::RadiusNew() const {
 float dy = y.Span();
 float dx = x.Span();
 float dz = z.Span();
 float xx = dx * dx;
 float yy = dy * dy;
 float zz = dz * dz;
 return GaugeHalfReferenceObservedNew() * (float)sqrt(zz + (xx + yy));
}

extern "C" {
void fn_8022A210(ModelFitAssetNew * const self, int x, int y, int width, int height, float delta);
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
extern ProviderDrawMetrics* lbl_8055E050;extern float lbl_80558DC4[];extern const float lbl_80563508,lbl_8056350C,lbl_805634C8;
void* fn_80211660(int);int fn_8022AB18(int,int,int);void fn_8022AAA8(int,const char*);u32 fn_8022AA84(int);
float fn_80235EF0(void*,int);float fn_80235F64(void*,int);void fn_802354A0(void*,int,u32,float,float,float,float,float);
}
static inline float ObservedAbsDecision(float value){return (float)__fabs(value);}
extern "C" {
void fn_8021DB58(WidgetFragment* self);
}

struct EditorDispatch {virtual ~EditorDispatch();virtual void Slot0c();virtual void Slot10();virtual void Slot14();virtual void Slot18();virtual void Slot1c(int,int);virtual void Slot20();virtual void Slot24();virtual void Slot28();virtual void Slot2c();virtual void Slot30();virtual void Slot34();virtual void Slot38();virtual void Slot3c();virtual void Slot40();virtual void Slot44();virtual void Slot48();virtual void Slot4c();virtual void Slot50();virtual void Slot54();virtual void Slot58();virtual void Slot5c();virtual void Slot60();virtual void Slot64(int);virtual void Slot68();virtual void Slot6c();virtual void Slot70();virtual void Slot74(int,void*);virtual int Slot78();virtual void Slot7c();virtual void Slot80();virtual int Slot84();virtual const char* Slot88();};

struct TextEntryPrefix {char base[0x244];int state,capacity,stride,cursor;char unknown254[0x800];char* buffer;int font;void* first;void* second;};
typedef char TextEntryPrefixSize[(sizeof(TextEntryPrefix)==0xa64)?1:-1];
extern "C" {
extern u32 lbl_8033E988[];extern const char lbl_8033D96C[],lbl_8033D978[];
void fn_802C2FC8(void*,int,int,int,int);void fn_80228DC4(ObservedClipPrefix*,void*,int,int,int,int);
void fn_8022B390(int,const char*,int,int*,int*);void fn_802C47B8(int,int,int,int,int);void fn_802C3014(void*);
int fn_8022B2D4(int,const char*,int,int);extern int lbl_8055FBF8;
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
struct LeafChildDispatch {virtual ~LeafChildDispatch();virtual void Slot0c();virtual void Slot10();virtual void Slot14();virtual void Paint(int,int,int,int);};
struct LeafChildPrefix {char base[0x244];LeafChildDispatch* child;int first,second;};
struct LeafResource250 {char base[0x250];void* resource;};
extern "C" {
void fn_802C5280(void*,int,int,int);void fn_802C744C(int,void*);
void fn_80229604(WidgetFragment* self,int,int);
void fn_802298F8(WidgetFragment*,void*);
void fn_802298FC(WidgetFragment*,void*);
void fn_80229900(WidgetFragment* self);
void fn_80229918(WidgetFragment* self);
void fn_80229958(WidgetFragment* self);
void fn_80229998(WidgetFragment* self,int,int);
void fn_80229A94(WidgetFragment* self);
void fn_80229AC4(WidgetFragment* self,int,int);
int fn_80229ADC(WidgetFragment*);
void fn_80229AE4(WidgetFragment* self,int x,int y);
int fn_80229B68(WidgetFragment*);
void* fn_80229B70(WidgetFragment* self);
void* fn_80229C9C(WidgetFragment* self);
void* fn_80229CA4(WidgetFragment* self);
void* fn_80229CAC(WidgetFragment* self);
void fn_80229CB4(WidgetFragment* self,int,int);
void fn_80229CD8(WidgetFragment* self,int,int);
void fn_80229CF8(WidgetFragment* self);
void fn_80229D1C(WidgetFragment* self);
void fn_80229D3C(WidgetFragment* self,int x,int y);
void* fn_80229D5C(WidgetFragment* self);
void* fn_80229DC0(WidgetFragment* self);
void fn_80229ED4(WidgetFragment* self);
void fn_80229EF4(WidgetFragment* self);
void* fn_80229F14(WidgetFragment* self);
void fn_80229F1C(WidgetFragment* self);
void fn_80229F38(WidgetFragment* self);
void* fn_80229F58(WidgetFragment* self);
int fn_80229F60(WidgetFragment* self);
void fn_80229FF0(WidgetFragment* self,int x,int y);
void fn_8022A008(WidgetFragment* self,int,int);
void fn_8022A014(WidgetFragment* self);
void fn_8022A034(WidgetFragment* self);
void fn_8022A574(ResourcePrefix* self);
}


struct AdapterTarget {virtual ~AdapterTarget();virtual void Name(const char*);virtual int Enter();virtual void Slot14();virtual int Leave();virtual void Draw();virtual int Flags();virtual int State();};
struct AdapterPrefix {char base[0x244];const char* name;char lastName[32];AdapterTarget* target;unsigned char owned;char padding[3];int left,top,right,bottom;};
typedef char AdapterPrefixSize[(sizeof(AdapterPrefix)==0x280)?1:-1];
extern "C" {
extern u32 lbl_8033DD20[];
void* memset(void*,int,unsigned long);int strcasecmp(const char*,const char*);
void fn_80228AEC(WidgetFragment*,float);
void fn_802C86F0(ObservedClipRect*,int,int,int,int);void fn_802C88BC(int);void fn_802C8750(ObservedClipRect*,int);
void* fn_8022882C(AdapterTarget* self,int destroy);
WidgetFragment* fn_8022886C(WidgetFragment* self,WidgetDesc* desc,AdapterTarget* target,const char* name);
WidgetFragment* fn_802288E0(WidgetFragment* self,int destroy);
int fn_8022897C(WidgetFragment* self);
void fn_80228990(WidgetFragment*);
void fn_80228994(WidgetFragment* self);
void fn_80228A38(WidgetFragment* self);
int fn_80228A6C(WidgetFragment* self);
void fn_80228A80(WidgetFragment*);
void fn_80228A84(WidgetFragment* self);
int fn_80228D90(WidgetFragment* self,int);
int fn_80228DA4(WidgetFragment*);
int fn_80228DAC(WidgetFragment* self);
void fn_80228DC0(WidgetFragment*);
}


// Measured ABI prefix views for independent remaining widget methods.
struct Bound2a8 {char base[0x2a8];int* focus;int token;};
struct Bound274 {char base[0x274];int* focus;int token;};
struct Bound258 {char base[0x258];int* focus;int token;};
struct Bound25c {char base[0x25c];int* focus;int token;};
struct Bound27c {char base[0x27c];int* focus;int token;};
struct Bound264 {char base[0x258];int(*callback)(void*);void* context;int token;int* focus;};
struct SelectionPrefix {char base[0x2c8];int count;int* selected;unsigned char available[33];char padding2f1[3];float widths[33];int icons[33];int resource;float period,scale,speed,first,second,pulse;char gap418[4];unsigned char locked;};
typedef char SelectionPulseOffset[(sizeof(SelectionPrefix)==0x420)?1:-1];
struct PrefixFlag248 {char base[0x248];unsigned char value;};
struct PrefixFlag250 {char base[0x250];unsigned char value;};
struct PrefixFlag258 {char base[0x258];unsigned char value;};
struct RenderPrefix {char base[0x244];float x,y,w,h;void* resource;};
struct TweenPrefix {char base[0x24c];int selected;char gap250[12];float blend;char gap260[4];int* state;};
struct TextCommandPrefix {char base[0x244];int value;char gap248[0x100];const char* text;};
extern "C" {
void fn_801E4BEC(const char*,int,int,...);
void fn_801A7E54(void*,int,int);void fn_802C40B8(int,int,int);void fn_802ECFEC(void*);void fn_802ED038(void*,float,float,float,float);
void fn_802168BC(void*);void fn_802237DC(WidgetFragment*);
int fn_8021AB88(WidgetFragment*);
int fn_8021C974(WidgetFragment*);
void fn_8021B3BC(WidgetFragment* self,void* context);
void fn_8021BD48(WidgetFragment* self,void* context);
void fn_8021BF90(WidgetFragment* self);
void fn_8021C478(WidgetFragment* self);
int fn_8021C318(WidgetFragment* self);
void fn_8021F42C(WidgetFragment* self,int,void*);
void fn_8021F7EC(WidgetFragment* self);
void fn_8022071C(WidgetFragment* self);
void fn_802208EC(WidgetFragment* self);
void fn_80220AFC(WidgetFragment* self,int x,int y);
void fn_80220B3C(WidgetFragment* self,int x,int y);
void fn_80220E08(WidgetFragment* self,void* context);
void fn_80221538(WidgetFragment* self,int,int);
void fn_80221E7C(WidgetFragment* self);
void fn_80221EA0(WidgetFragment* self,int key,void* value,int size);
void fn_80222120(WidgetFragment* self,int x,int y);
void fn_80223728(WidgetFragment* self);
void fn_802237D0(WidgetFragment* self);
void fn_802237DC(WidgetFragment* self);
int fn_802237E8(WidgetFragment* self);
int fn_802237F0(WidgetFragment* self,int index);
int fn_80223AAC(WidgetFragment*);
void fn_80223CC4(WidgetFragment* self);
void fn_80224160(WidgetFragment* self);
void fn_8022419C(WidgetFragment* self);
void fn_80224328(WidgetFragment* self);
int fn_80224B0C(WidgetFragment* self,int key);
void fn_802258C8(WidgetFragment* self);
void fn_80225918(WidgetFragment* self);
int fn_80226528(WidgetFragment* self,int key);
WidgetFragment* fn_802266B0(WidgetFragment* self,int destroy);
void fn_80226708(WidgetFragment* self);
void fn_80226758(WidgetFragment* self,int,int);
void fn_80226768(WidgetFragment* self,int,int);
int fn_802268A0(WidgetFragment* self,int key);
int fn_802299B0(WidgetFragment* self);
void fn_80229B0C(WidgetFragment* self,int dx,int dy);
WidgetFragment* fn_80229D64(WidgetFragment* self,int destroy);
WidgetFragment* fn_8022A0CC(WidgetFragment* self,int destroy);
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
void fn_8021BEBC(WidgetFragment* self,int,void*);
void fn_8021C12C(WidgetFragment* self);
void fn_8021C128(WidgetFragment* self,int,void*);
void fn_8021C6D0(WidgetFragment* self);
void fn_8021C6CC(WidgetFragment* self,int,void*);
WidgetFragment* fn_8021C97C(WidgetFragment* self,int x,int y,void* handle,int direction,FormatTarget* target);
int fn_8021C9D8(WidgetFragment* self);
u32 fn_802236C8(WidgetFragment* self,int index);
void fn_80223880(WidgetFragment* self);
}


struct CaptionBufferPrefix {char base[0x254];const char* label;char gap258[16];char text[1];};
struct OptionalResourcePrefix {char base[0x244];int resource;char gap248[16];void* optional;};
struct BindingFieldPrefix {char base[0x254];char buffer[24];int parameter;int* value;};
struct HitAreaPrefix {char base[0x264];int x,y,width,height;};
extern "C" {
extern u32 lbl_8033F0FC[],lbl_8033E7F4[],lbl_8033E8FC[];
int strcmp(const char*,const char*);
int fn_8021AB90(WidgetFragment* self,int key);
void fn_8021B32C(WidgetFragment* self);
WidgetFragment* fn_8021BBA0(WidgetFragment* self,int destroy);
WidgetFragment* fn_8021F394(WidgetFragment* self,int x,int y,int parameter,int* value,int height);
int fn_8021FBC4(WidgetFragment* self,int x,int y);
WidgetFragment* fn_8022003C(WidgetFragment* self,int destroy);
}

extern "C" {
int fn_802E4A1C(int);
int fn_8021F06C(WidgetFragment* self,int key);
}

struct ScopedAssetPrefix {char base[0x368];int resource;char gap36c[4];void* asset;};
extern "C" {
extern u32 lbl_8033F180[];void fn_8022EE98(void*);void fn_8022EEF8(void*,void*,const void*);void fn_8022ED00(void*);void fn_8022EDB8();
WidgetFragment* fn_8021B8AC(WidgetFragment* self,int destroy);
void fn_8021B944(WidgetFragment* self);
void fn_80223A58(WidgetFragment* self);
}

struct TwoCleanupPrefix {char base[0x250];void* image;char gap254[12];void* asset;};
struct ThreeCleanupPrefix {char base[0x280];void* image;void* assetFirst;void* assetSecond;};
struct TimedFocusPrefix {char base[0x24c];float stamp;char gap250[4];int token;int* focus;char gap25c[4];unsigned char active,inactive;char gap262[6];float value,previous;};
extern "C" {
extern u32 lbl_8033E3D4[],lbl_8033E350[],lbl_8033E2CC[],lbl_8033E248[];void fn_802ECFF4(void*);
int fn_80223760(WidgetFragment* self,int key);
int fn_802239C8(WidgetFragment* self,int key);
WidgetFragment* fn_80223B94(WidgetFragment* self,int destroy);
WidgetFragment* fn_80223FAC(WidgetFragment* self,int destroy);
WidgetFragment* fn_802242A4(WidgetFragment* self,int destroy);
WidgetFragment* fn_802244C8(WidgetFragment* self,int destroy);
void fn_8022455C(WidgetFragment* self);
void fn_80224864(WidgetFragment* self,int,void*);
}

struct PairImage248 {char base[0x248];void* first;void* second;};
struct PairImage25c {char base[0x25c];void* first;void* second;};
struct AssetImage244 {char base[0x244];void* image;void* asset;};
struct SceneResourcePrefix {char base[0x244];void* model;float x,y,height,width;int frame;void* asset;unsigned char owned;};
struct ResourceHandlePrefix {char base[0x254];int handle;char gap258[8];void* asset;};
struct TripleResourcePrefix {char base[0x250];int resource;void* first;void* second;};
struct RichCaptionPrefix {char base[0x244];const char* label;char text[256];const char* renderer;};
struct SliderRangePrefix {char base[0x244];int* value;char gap248[8];int minimum,maximum;unsigned char dragging;};
struct MenuRecord {char base[0x20];void* context;int(*callback)(void*);};
struct MenuRowPrefix {void* handle;char rest[32];};
struct MenuSelectionPrefix {char base[0x244];MenuRowPrefix* rows;unsigned char active;char pad249[3];int row,column;};
extern "C" {
extern u32 lbl_8033E668[],lbl_8033E560[],lbl_8033E0BC[],lbl_8033E038[],lbl_8033DFB4[],lbl_8033DEAC[],lbl_8033DF30[],lbl_8033EFF4[],lbl_8033E878[],lbl_8033E770[],lbl_8033DCF8[],lbl_8033EF68[];
void fn_802CAC54(int);void fn_802C6CF8(int);int fn_8012C33C(const char*);int fn_802C6FC0(const char*,int,int);void fn_801E4A2C(const char*);int fn_801E4A6C(const char*,const char*);int fn_801E4BA8(const char*);void fn_80230C30(float*,float*,int,int);MenuRecord* fn_8022D97C(void*,int);
WidgetFragment* fn_80220874(WidgetFragment* self,int destroy);
WidgetFragment* fn_80222C8C(WidgetFragment* self,int destroy);
WidgetFragment* fn_80226AD0(WidgetFragment* self,int destroy);
WidgetFragment* fn_80226F00(WidgetFragment* self,int destroy);
void fn_80226F94(WidgetFragment* self);
WidgetFragment* fn_80227244(WidgetFragment* self,int destroy);
WidgetFragment* fn_80227D64(WidgetFragment* self,int destroy);
WidgetFragment* fn_80227F5C(WidgetFragment* self,int destroy);
WidgetFragment* fn_80229B78(WidgetFragment* self,int destroy);
void fn_80229BE8(WidgetFragment* self);
WidgetFragment* fn_80229F78(WidgetFragment* self,int destroy);
WidgetFragment* fn_8022A054(WidgetFragment* self,int destroy);
ResourcePrefix* fn_8022A124(ResourcePrefix* self,int destroy);
void fn_8022A19C(ResourcePrefix* self,const char* name);
WidgetFragment* fn_8021BD98(WidgetFragment* self,int x,int y,float* value,const char* format,int height,float scale,float bias);
void fn_80222178(WidgetFragment* self,int x,int y);
}

struct KeyboardFilePrefix {char path[24];int handle;};
struct DelegatePaintDispatch {virtual ~DelegatePaintDispatch();virtual void Paint(int,int,int,int,int,const char*);};
struct DelegatePaintPrefix {char base[0x244];DelegatePaintDispatch* child;int first,second,third;};
struct ResourceDescriptorPrefix {int zero,token,x,y,kind,width,height;float point;char text[0x64];int extra84,extra88,extra8c,extra90,extra94,extra98,extra9c,extraa0,extraa4;};
extern "C" {
extern KeyboardFilePrefix lbl_80490934;extern u32 lbl_8033DEAC[];extern const char lbl_8033DC4C[],lbl_8033DC58[];
void fn_801A7844(int);int fn_801A7C8C(int);void fn_802E47E8();void fn_802E30C8(const char*,...);
int fn_8021F478(int* result);
void fn_80227C9C(void* self,float x,float y,float width,float height);
void fn_80227DE8(WidgetFragment* self);
WidgetFragment* fn_80227D00(WidgetFragment* self,WidgetDesc* desc);
void fn_802299FC(WidgetFragment* self);
}

struct SelectNavigationPrefix {char base[0x2c8];int count;int* selected;unsigned char available[1];};
struct SelectWatchedPrefix {char base[0x420];int* watched;};
struct ImageAlphaAsset {char base[0x30];float alpha;};
struct ImageAlphaPrefix {char base[0x244];float x,y;char gap24c[4];void* image;float width,height,blend;ImageAlphaAsset* asset;};
struct RollingRow {float current,previous,target;};
struct RollingRowsCount {char base[8];int count;};
struct RollingRowsPrefix {char base[0x244];RollingRowsCount* list;char gap248[20];float speed,period,upper,lower;char gap26c[12];RollingRow* rows;};
struct SelectionActionDispatch {virtual ~SelectionActionDispatch();virtual void Slot0c();virtual void Slot10();virtual void Slot14();virtual void Slot18();virtual int State();};
struct SelectionActionPrefix {char base[0x244];SelectionActionDispatch* child;char gap248[24];int token;int* focus;};
extern "C" {
extern u32 lbl_8033E458[],lbl_8033E140[];
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
WidgetFragment* fn_80223800(WidgetFragment* self,int x,int y,void* model,int* watched);
void fn_80223C20(WidgetFragment* self);
WidgetFragment* fn_80225828(WidgetFragment* self,int destroy);
void fn_80226570(WidgetFragment* self,int delta);
int fn_802268E8(WidgetFragment* self);
}


struct RotationPrefix {char base[0x299];unsigned char owned;char gap29a[2];RotationMatrix* matrix;};
struct AssetRotationPrefix {char base[0x25c];RotationMatrix* matrix;unsigned char owned;};
struct AssetFormatPrefix {char base[0x244];int first,resource;char gap24c[13];char text[64];unsigned char owned;char gap29a[2];void* asset;};
extern "C" {
extern const float lbl_805634F8;extern const char lbl_8033DA2C[],lbl_8055B88C[4];extern u32 lbl_8033E1C4[],lbl_8033F7F4[];
RotationMatrix* fn_8022EE9C();void fn_80015A20(RotationMatrix*,const RotationVec*);void Mat3Mul__FP4Mat3PC4Mat3PC4Mat3(RotationMatrix*,const RotationMatrix*,const RotationMatrix*);void fn_802E317C(const char*,...);
void fn_80224DDC(WidgetFragment*,float,float,float);
void fn_80227044(WidgetFragment*,float,float,float);
WidgetFragment* fn_80224D40(WidgetFragment* self,int destroy);
WidgetFragment* fn_80226630(WidgetFragment* self,void* context,int x,int y,int dx,int dy,OwnedWidget* child,int(*callback)(void*));
void fn_80227880(float x,float y,float width,float height);

void fn_802238D0(WidgetFragment* self,int mode,void*);
}

struct SourceResourcePrefix {char base[0x244];int kind;void* resource;char gap24c[12];const char* name;char cached[32];};
struct DrawStateWordsNew {u32 words[3];};
struct FullDrawStatePrefix {RotationMatrix matrix;DrawStateWordsNew light;float alpha,first,second;};
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
extern u32 lbl_8033F434[],lbl_8033EEDC[];int strncasecmp(const char*,const char*,unsigned long);void fn_802C5310(void*,int,float,float,float);
WidgetFragment* fn_80219E44(WidgetFragment* self,int destroy);
void fn_80219F14(WidgetFragment* self);

WidgetFragment* fn_8021BEC0(WidgetFragment* self,int x,int y,FullFormatDispatch* target,int height,int left,int right,int lineY);
void fn_8021BFEC(WidgetFragment* self);
}

struct ExtendedLabeledFormatPrefix {char base[0x254];char text[80];FullFormatDispatch* target;int* focus;int token,left,right,lineY,labelLeft,labelRight,font;const char* label;int side;bool showArrows;};
extern "C" {
extern u32 lbl_8033EE50[];int fn_8022B498(int,const char*);
int fn_8021C18C(WidgetFragment* self,int key);
WidgetFragment* fn_8021C344(WidgetFragment* self,int x,int y,FullFormatDispatch* target,int height,int left,int right,int lineY,const char* label,int alignment);
void fn_8021C4D4(WidgetFragment* self);
}

struct GaugeValuePrefix {char base[0x248];FullFormatDispatch* target;void* background;void* marker;int inset;};
extern "C" {
void fn_802180BC(void*);void fn_802180B0(void*);void fn_802180C8(void*);void fn_802180D4(void*);extern const float lbl_80563500;
int fn_8021C730(WidgetFragment* self,int key);
void fn_8021CB4C(WidgetFragment* self,void* context);
void fn_8021CBBC(WidgetFragment*);
}

struct SelectResourcePrefix {char base[0x3fc];int resource;};
struct RenderRequestPrefix {char base[0x244];float x,y,w,h;void* resource;const char* name;bool notify;char pad25d[3];int(*callback)(void*);void* context;};
struct ImageDescriptorPrefix {char base[0x244];float x,y;int value;void* image;float width,height,alpha;void* asset;void* context;};
extern "C" {
extern u32 lbl_8033E4DC[];extern const char lbl_8033DA90[],lbl_8033DAAC[],lbl_8033DABC[];void* fn_802ECF08(const char*,int);int fn_802ED040(void*);int fn_802ECF90(void*,const char*);
WidgetFragment* fn_80223208(WidgetFragment* self,int destroy);
WidgetFragment* fn_80223AB4(WidgetFragment* self,WidgetDesc* desc,void* context);
void fn_80224028(WidgetFragment* self);
WidgetFragment* fn_802241C8(WidgetFragment* self,WidgetDesc* desc);
}

struct ToggleImagePrefix {char base[0x244];int frame,mode;float elapsed;void* context;int token;int* focus;int(*callback)(void*);bool active,pressed;char gap262[2];int speed;float alpha,blend,x,y,width,height;void* image;ImageAlphaAsset* asset;void* scope;};
struct SelectionCallbackPrefix {char base[0x258];int(*callback)(void*);void* context;int token;int* focus;};
extern "C" {
int fn_802C51AC(void*);
void fn_802248FC(WidgetFragment* self);
int fn_80224A38(WidgetFragment* self,int key);
int fn_802267C4(WidgetFragment* self,int key);
void fn_80228E80(ObservedClipPrefix* context,void* image,int x,int y,int height);
}

struct SliderFullPrefix {char base[0x244];int* value;void* background;void* marker;int minimum,maximum;bool dragging;char gap259[3];int* focus;int token;};
extern "C" {
extern const float lbl_805634E0;extern u32 lbl_8033E668[];extern const char lbl_8033D9B0[],lbl_8033D9BC[];
WidgetFragment* fn_80220778(WidgetFragment* self,int x,int y,int width,int height,int* value,int minimum,int maximum);
int fn_80220948(WidgetFragment* self,int key);
void fn_80220B5C(WidgetFragment* self,void* context);
void fn_80220C54(WidgetFragment* self);
void fn_80228DC4(ObservedClipPrefix* context,void* image,int x,int y,int width,int);
}

struct MultiValuePrefix {char base[0x244];void* value;void* first;void* second;bool dragging;char gap251[3];int direction;};
struct ThreeResourcePrefix {char base[0x244];bool flag;char gap245[3];void* first;void* second;int resource;void* image;void* overlay;};
extern "C" {
extern u32 lbl_8033E770[],lbl_8033DF30[];
}

struct PageTextList {char base[8];int count;char gapc[4];int pageSize;char gap14[12];int page;int version;};
struct PageTextPrefix {
 char base[0x244]; // 0x000..0x243: observed widget prefix
 int font; // 0x244: resource handle
 char gap248[8]; // 0x248..0x24F: unknown fields, no fill or ownership claim
 int rowHeight; // 0x250
 PageTextList* list; // 0x254
 void* image; // 0x258: observed zero initialization; complete resource type open
 int column,iconOffset; // 0x25C, 0x260
};
struct DropdownRowPrefix {RowListPrefix list;int width,height;};
struct FloatSliderPrefix {char base[0x244];bool dragging;char gap245[3];float minimum,maximum;float* value;int rate;bool negative;char gap259[3];void* background;void* marker;int oldX,oldY,oldWidth,oldHeight;int* focus;int token;};
extern "C" {
const char* fn_8022D850(PageTextList*,int,int,int*);void fn_8022D924(PageTextList*,int);void fn_802C4F58(int,int,const char*,int);extern const char lbl_8055B878[4],lbl_8055B884[7];extern const float lbl_805634FC;extern u32 lbl_8033E878[];
void fn_8021BA64(WidgetFragment*,int,int,PageTextList*,int,const char*,int,int,int,int,int,int,int);
void fn_8021BC20(WidgetFragment* self);
void fn_8021E8DC(WidgetFragment* self,void* context);
static inline int InlineWidgetAbsolute(int value){return value<0?-value:value;}
WidgetFragment* fn_8021F678(WidgetFragment* self,int x,int y,int width,int height,float minimum,float maximum,float* value,int rate);
}

extern "C" {
extern const float lbl_80563518;extern u32 lbl_8033E7F4[];void fn_802C5334(void*,int,float,float,float,float,float,float,float,float,float);
void fn_8021FA98(WidgetFragment *self, int x, int y);
void fn_8021FC40(WidgetFragment* self,void* context);
void fn_8021FD68(WidgetFragment* self);
}

struct MenuTablePrefix {MenuRowPrefix rows[8];int count;};
struct RollingActionList {char base[0x14];int(*callback)(void*);unsigned char vertical;};
struct RollingActionPrefix {char base[0x244];RollingActionList* list;char gap248[8];float stamp;char gap254[32];int direction;char gap278[4];int* focus;int token;};
struct CenteredImageState {char base[0x24];float x,y;};
struct CenteredImagePrefix {char base[0x244];void* image;CenteredImageState* asset;float x,y,height,width;int value;};
extern "C" {
void fn_8022DBFC(void*);void fn_8022DC20(void*);void* fn_8022DBE8(void*);extern u32 lbl_8033E0BC[];extern const char lbl_8033DAF8[];

int fn_802263CC(WidgetFragment* self,int key);
WidgetFragment* fn_802269A8(WidgetFragment* self,WidgetDesc* desc);

}

extern "C" {
int fn_802205AC(WidgetFragment* self,int key);
}

struct RollingInterpolationPrefix {char base[0x244];RollingRowsCount* list;char gap248[8];float stamp;char gap254[12];float period,upper,lower;char gap26c[8];int direction;RollingRow* rows;};
static inline float InlineWidgetInterpolate(float first,float last,float position,float duration){return first+(position/duration)*(last-first);}
extern "C" {
extern const float lbl_80563548,lbl_8056353C,lbl_80563540;double sin(double);
void fn_8022591C(WidgetFragment* self);
void fn_802245C8(WidgetFragment* self);
}

struct ScrollbarPrefix {char base[0x244];void* view;void* track;void* marker;bool dragging;char gap251[3];int vertical;};
extern "C" {
extern u32 lbl_8033E7F4[],lbl_8033E770[];

WidgetFragment* fn_80220E48(WidgetFragment* self,int x,int y,int width,int height,void* view,int vertical);
}

extern "C" {
int fn_8021F848(WidgetFragment* self,int key);
}

struct ResourceTextPrefix {char base[0x244];int context,resource,style,flags;float scale;unsigned char localized;char text[64];bool owned;char gap29a[2];void* asset;};
struct FormattedChoiceDescriptor {int unknown0,font,x,y,kind,width,height;float lineHeight;char name[0x64];int option84,option88,option8c,option90,option94,option98,option9c,optiona0,optiona4;};


extern "C" {
extern const char lbl_8033DAD8[];int fn_80129D90(int,int,int);int fn_80131234(const char*);extern u32 lbl_8033DF30[],lbl_8033E7F4[];
WidgetFragment* fn_80224B54(WidgetFragment* self,int context,WidgetDesc* desc);
WidgetFragment* fn_8021B9AC(WidgetFragment* self,FormattedChoiceDescriptor* desc,PageTextList* provider,int column);
WidgetFragment* fn_80227E84(WidgetFragment* self,void* first,WidgetDesc* desc,void* second);
WidgetFragment* fn_8021FF34(WidgetFragment* self,WidgetDesc* desc,float* value,int divisions,float minimum,float maximum);
}

struct UnsignedAssetPrefix {char base[0x244];void* context;int resource,mode;float scale,elapsed;};
extern "C" {
extern const float lbl_8056351C;extern const char lbl_8033DA60[];extern u32 lbl_8033E560[];
WidgetFragment* fn_80222BDC(WidgetFragment* self,WidgetDesc* desc,void* context);

}


extern "C" {
WidgetFragment* fn_8022435C(WidgetFragment* self,void* context,WidgetDesc* desc,int(*callback)(void*),int* frame);
}

struct RollingListData {char base[8];int count,index;};
struct RollingListFullPrefix {char base[0x244];RollingListData* list;int resource,count;float stamp,x,y,lineHeight,period,upper,lower,elapsed,nextStamp;int direction;RollingRow* rows;int* focus;int token;};
static inline int InlineIntegerMinimum(int value,int bound){if(value<bound)return value;return bound;}
static inline int InlineIntegerMaximum(int value,int bound){if(value>bound)return value;return bound;}
extern "C" { extern const float lbl_80563544;extern const char lbl_8033DAEC[],lbl_8033DA84[];
WidgetFragment* fn_8022553C(WidgetFragment* self,RollingListData* list,WidgetDesc* desc);
}

struct StreamAsset348 {u32* vtable;int resource;float matrix[12];float elapsed,period;};
struct StreamWidget348 {char base[0x244];void* context;char cache[32];StreamAsset348* asset;bool owned;};
extern "C" {
void* __nw__FUlPCc(unsigned long,const char*);extern const float lbl_805634D8;extern const char lbl_8033DCE8[];extern u32 lbl_8033DD20[],lbl_8033DCF8[];
WidgetFragment* fn_802286E4(WidgetFragment* self,WidgetDesc* desc,void* context);
}

// Observed timed-image prefix: byte 0x264 is the fade direction, and 0x270 reverses angular motion.
struct TimedImage349 {char base[0x244];float x,y,width,height;int resource;float rate,stamp;void* asset;bool increasing;char pad265[3];float first,second;bool reverse;};
extern "C" {
extern u32 lbl_8033DFB4[];extern const char lbl_8033DC20[];extern const float lbl_805634C8;
int fn_802CAC28(const char*,int);

}

struct EffectImage351 {char base[0x244];void* image;float x,y,height,width;int mode;RotationMatrix* matrix;bool owned;};
extern "C" {
extern unsigned char lbl_8055EFBC;extern const char lbl_8055B8A4[3];extern u32 lbl_8033E038[];int fn_80235FD8(void*);
WidgetFragment* fn_80226CBC(WidgetFragment* self,WidgetDesc* desc);
}

extern "C" {
extern const float lbl_80563514;
void fn_8021EDA4(WidgetFragment* self);
}

extern "C" {
extern const float lbl_805634EC;
void fn_802278F0(void* image,float x,float y,float width,float height);
}

struct ResourceInit357 {char base[0x244];int kind,resource,argument,style,flags;const char* name;char cache[32];float scale;bool flag;};
extern "C" {
extern const char lbl_8033D8B8[];
void fn_80219AB4(WidgetFragment* self,int kind,const char* name,int style,int argument,int flags,bool flag);
}

extern "C" {
}

extern "C" {
static inline float AddScroll358(float a,float b){return b+a;}
void fn_80220F90(WidgetFragment* self,int x,int y);
}

struct ObservedTransform {float m[12];};
struct SpinVector359 {float x,y,z;SpinVector359(float a,float b,float c):x(a),y(b),z(c){}};
extern "C" {
void fn_80015B20(void*,const RotationMatrix*,void*);
static inline float SpinAngle359(int rate,float time){return time*(float)rate;}
void fn_80226B6C(WidgetFragment* self);
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
void fn_8021CBBC(WidgetFragment* self);
}

extern "C" {extern const float lbl_805634E8;

}

extern "C" {

}

struct FocusTemporaryNew {int selected;const char* label;};
extern "C" {
void fn_8021AA18(FocusPaintPrefix* self);
}

extern "C" {
void fn_8021BA64(WidgetFragment* self,int width,int height,PageTextList* list,int column,const char* font,int flags,int value,int unused,int first,int second,int rowHeight,int third);
}

extern "C" {
WidgetFragment* fn_802270DC(WidgetFragment* self,WidgetDesc* desc);
}

extern "C" {
void fn_802213A4(WidgetFragment* self,int x,int y);
}

extern "C" {
typedef unsigned int GroupSearchWordNew;
typedef struct GroupSearchContextNew {char base[0x238];GroupSearchWordNew group;} GroupSearchContextNew;
typedef struct GroupSearchProxyNew {char base[0x14];GroupSearchContextNew* context;} GroupSearchProxyNew;
extern const char lbl_8033DA2C[];void fn_802E317C(const char*,...);
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
struct ImageFillPaintNew {ImageFillPaintTableNew* vtable;int left,top,right,bottom,kind;char gap18[0x22c];int mode;void* image;int* watched;int index;unsigned int color;int style;char gap25c[0x20];float scale;unsigned char nativeSize;};
typedef struct ImageFillGlobalNew {char gap0[0x70];float padding;} ImageFillGlobalNew;

extern "C" {
extern const float lbl_805634A8,lbl_805634C0,lbl_805634C4,lbl_805634C8,lbl_805634D8,lbl_805634DC,lbl_805634E0;extern const double lbl_805634D0;
float fn_8023659C(void);void fn_80230C30(float*,float*,int,int);float fn_80235EF0(void*,int);float fn_80235F64(void*,int);int fn_80235FD8(void*);double fmod(double,double);double sin(double);
void fn_80230150(float,unsigned int,int,float,float,float,float,float,float,float);void fn_802304EC(int,void*,float,float,float,float,unsigned int,int,float,float,float);void fn_80230650(int,void*,float,float,float,float,unsigned int,int,float,float,float);void fn_802307AC(int,void*,float,float,float,float,unsigned int,int,float,float,float);void fn_80230924(unsigned int,float,float,float,float,float);void fn_802354A0(void*,int,unsigned int,float,float,float,float,float);
// Graphics declarations preserve the observed independent GPR/FPR argument orders.
// Their interleaving is a matching compiler form, not a claim about original headers.
void fn_80219FE0(WidgetFragment *source);
}

// MATCHING-AID (inherited): full-range denominator enum still retains allocation; natural replacement remains open.
enum SliderDenominatorWordNew { SliderDenominatorMinNew=(-2147483647-1),SliderDenominatorMaxNew=2147483647 };
static inline int ClampToExclusiveMaximum(const int& maximum,int value){
 int limit=maximum-1;
 return value>=limit?limit:value;
}
extern "C" {
void fn_80220A50(WidgetFragment* self,int x,int unused);
}



extern "C" {extern int lbl_8055FBE0,lbl_8055FBFC;

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
WidgetFragment* fn_80218F34(WidgetFragment* self,int left,int top,int right,int bottom,int kind){
 self->vtable=lbl_8033FB38;fn_80218FEC(self,left,top,right,bottom,kind);
 self->style=fn_8021525C();self->stylePtr=0;return self;
}
WidgetFragment* fn_80218F80(WidgetFragment* self,WidgetDesc* desc){
 self->vtable=lbl_8033FB38;fn_80218FEC(self,desc->x,desc->y,desc->x+desc->dx,desc->y+desc->dy,desc->kind);
 self->style=fn_8021525C();self->stylePtr=0;return self;
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
WidgetFragment* fn_8021915C(WidgetFragment* self,int destroy){
 if(self){self->vtable=lbl_8033FB38;for(int i=0;i<self->propertyCount;i++)__dla__FPv(self->properties[i].buffer);if(destroy>0)__dl__FPv(self);}return self;
}
void fn_802191F0(WidgetFragment* self,int x,int y){
 int right=x+((WidgetDispatch*)self)->Slot78();
 int bottom=y+(self->bottom-self->top);self->left=x;self->right=right;self->top=y;self->bottom=bottom;
}
void fn_80219264(WidgetFragment* self,int x,int y){((WidgetDispatch*)self)->Slot1c(self->left+x,self->top+y);}
void fn_80219284(WidgetFragment* self,int width,int height){self->right=self->left+width;self->bottom=self->top+height;}
void fn_802192A0(WidgetFragment*,void*){}
int fn_802192A4(WidgetFragment*,int){return 0;}
void fn_802192AC(WidgetFragment* self){
 int bytes;void* value;void* previous;for(int i=0;i<self->propertyCount;i++){
  bytes=self->properties[i].bytes;previous=self->properties[i].buffer;value=self->properties[i].value;
  if(memcmp(value,previous,bytes)!=0){memcpy(previous,value,bytes);((WidgetDispatch*)self)->Slot74(self->properties[i].key,self->properties[i].value);}
 }fn_80219044(self);
}
void fn_80219354(WidgetFragment* self){fn_80219044(self);}
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
 void* previous=__nwa__FUlPCc(bytes,lbl_8033D878);
 self->properties[self->propertyCount].key=key;self->properties[self->propertyCount].value=value;
 self->properties[self->propertyCount].bytes=bytes;self->properties[self->propertyCount].buffer=previous;
 memcpy(previous,value,bytes);return self->propertyCount++;
}
void fn_802194F4(WidgetFragment* self){fn_802179A0(self->window);}
void fn_802194FC(WidgetFragment* self){fn_80218198(self->window,0);}
void fn_80219508(WidgetFragment* self){fn_80217AC0(self->window);}
void fn_80219510(WidgetFragment* self,int x,int y){
 WidgetWindowOffsetPrefix* fields=(WidgetWindowOffsetPrefix*)self;
 InlineOffsetSet(fields,-x,-y);
}
void fn_8021952C(WidgetFragment* self,int x,int y){
 WidgetWindowOffsetPrefix* fields=(WidgetWindowOffsetPrefix*)self;
 WindowOffsetPrefix* window=(WindowOffsetPrefix*)self->window;
 if(fields->enabled){int dx=(int)((unsigned int)fields->x+(unsigned int)x);int dy=(int)((unsigned int)fields->y+(unsigned int)y);int wx=window->x;int wy=window->y;window->x=(int)((unsigned int)dx+(unsigned int)wx);window->y=(int)((unsigned int)wy+(unsigned int)dy);}
}
WidgetFragment* fn_80219568(WidgetFragment* self,int x,int y,int width,int height,int dx,int dy){
 fn_80218F34(self,x,y,x+dx,y+dy,width);
 self->vtable=lbl_8033FA28;
 self->x=0;self->y=0;self->width=width;self->height=height;
 return self;
}
void fn_802195DC(WidgetFragment* self,void* context){
 fn_802C3034(context);int frame=((WidgetDispatch*)self)->Slot84();const char* text=((WidgetDispatch*)self)->Slot88();
 fn_8022B91C(self->height,self->x,self->y,text,frame);
}
int fn_80219658(WidgetFragment*){return 0;}
void fn_80219660(WidgetFragment* self){
 const char* text;int frame,height,x,y;
 y=self->top;x=self->left;height=self->bottom-y;frame=((WidgetDispatch*)self)->Slot84();text=((WidgetDispatch*)self)->Slot88();int width=((WidgetDispatch*)self)->Slot78();
 fn_8022BAD0(self->height,self->x+x,self->y+y,self->width,width,height,text,frame,0,(1.0f));
}
void fn_80219718(WidgetFragment* self){
 int left,top,right,bottom;const char* text=((WidgetDispatch*)self)->Slot88();
 fn_8022AC1C(self->height,text,self->x+self->left,self->y+self->top,&left,&top,&right,&bottom);
 left=WidgetMinInt(left,self->left);int oldLeft=self->left;right=WidgetMaxInt(right,oldLeft+((WidgetDispatch*)self)->Slot78());
 top=WidgetMinInt(top,self->top);bottom=WidgetMaxInt(bottom,self->bottom);
 self->x+=self->left-left;self->y+=self->top-top;
 ((WidgetDispatch*)self)->Slot1c(left,top);fn_80219284(self,right-left,bottom-top);
}
WidgetFragment* fn_80219870(WidgetFragment* self,int x,int y,const char* text,unsigned char* flag,int height){
 fn_80219568(self,x,y,0,height,0,0);self->vtable=lbl_8033F768;
 strcpy((char*)self+0x258,text);((FlagTextPrefix*)self)->flag=flag;
 fn_80219440(self,0,flag,1);fn_80219718(self);return self;
}
WidgetFragment* fn_80219900(WidgetFragment* self,int destroy){
 if(self){fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}
 return self;
}
int fn_80219958(WidgetFragment* self,int key){return fn_802192A4(self,key);}
WidgetFragment* fn_8021995C(WidgetFragment* self,int x,int y,int width,OwnedWidget* value,void* watched){
 fn_80222340(self,x,y,width,0,0,value);self->vtable=lbl_8033F4B8;
 self->label=(const char*)watched;if(watched)fn_80219440(self,0,watched,4);return self;
}
WidgetFragment* fn_802199D0(WidgetFragment* self,int destroy){if(self){fn_802223C4(self,0);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_80219A28(WidgetFragment* self,WidgetDesc* desc,const char* text,int value){
 fn_80218F80(self,desc);self->vtable=lbl_8033F434;
 fn_80219AB4(self,desc->flags,text,desc->field9c,value,desc->fielda0,desc->fielda4!=0);return self;
}
void fn_80219AB4(WidgetFragment* self,int kind,const char* name,int style,int argument,int flags,bool flag){
 ResourceInit357* f=(ResourceInit357*)self;f->argument=argument;f->name=name;f->flags=flags;f->style=style;f->kind=kind;
 const char* source=name;int remaining=32;char* destination=f->cache;while((*destination++=*source++)!=0){if(--remaining<=0){destination[-1]=0;break;}}
 f->flag=flag;f->resource=0;f->scale=(1.0f);
 const char* font=0;if(kind!=6&&kind!=3)font=(const char*)argument;
 float sharedScale,sharedWidth;switch(kind){
 case 0:case 1:case 7:break;
 case 4:if(font){f->resource=fn_80232578(font);int height=fn_80232B70(f->resource);height=height<1?1:height;sharedWidth=(float)((WidgetDispatch*)self)->Slot78()/(float)height;float width=fn_8023293C(f->resource,name,(1.0f));width=width>(1.0f)?width:(1.0f);float other=(float)(self->bottom-self->top)/width;f->scale=sharedWidth<other?sharedWidth:other;}break;
 case 2:if(font){f->resource=fn_80232578(font);int h=fn_80232B70(f->resource);float height=(float)(self->bottom-self->top);h=h<1?1:h;float scale=height/(float)h;float width=fn_8023293C(f->resource,name,(1.0f));sharedWidth=(1.0f);if(width>sharedWidth)sharedWidth=width;float other=(float)((WidgetDispatch*)self)->Slot78()/sharedWidth;f->scale=scale<other?scale:other;}break;
 case 9:f->resource=(int)fn_8023512C(name);break;
 case 3:case 6:case 8:f->resource=(int)fn_8023512C(name);if(argument)fn_80219440(self,0,(void*)argument,4);break;
 case 5:if(font){f->resource=fn_80232578(font);int h=fn_80232B70(f->resource);float height=(float)(self->bottom-self->top);h=h<1?1:h;float scale=height/(float)h;float width=fn_8023293C(f->resource,name,(1.0f));sharedWidth=(1.0f);if(width>sharedWidth)sharedWidth=width;float other=(float)((WidgetDispatch*)self)->Slot78()/sharedWidth;f->scale=scale<other?scale:other;}break;
 default:fn_802E317C(lbl_8033D8B8,kind);break;
 }
}
WidgetFragment* fn_80219E44(WidgetFragment* self,int destroy){if(self){self->vtable=lbl_8033F434;SourceResourcePrefix* f=(SourceResourcePrefix*)self;switch(f->kind){case 3:case 6:case 8:case 9:if(f->resource)fn_80235380(f->resource);break;case 2:case 4:case 5:if(f->resource)fn_802328D4((int)f->resource);break;}f->resource=0;fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;}
void fn_80219F14(WidgetFragment* self){fn_802192AC(self);SourceResourcePrefix* f=(SourceResourcePrefix*)self;switch(f->kind){case 3:case 6:case 8:case 9:if(strncasecmp(f->cached,f->name,31)!=0){if(f->resource)fn_80235380(f->resource);f->resource=fn_8023512C(f->name);const char* source=f->name;int remaining=32;char* destination=f->cached;while((*destination++=*source++)!=0){if(--remaining<=0){destination[-1]=0;break;}}}break;}}
void fn_80219FE0(WidgetFragment *source) {ImageFillPaintNew* fields=(ImageFillPaintNew*)source;
    float x;
    float y;
    int mode;
    void *shaderImage;
    void *stretchImage;
    void *gradientImage;
    int frameCount;
    int solidFlags;
    int pulseFrame;
    int imageFrame;
    int *pulseWatch;
    unsigned int frameSign;
    int *imageWatch;
    int solidHeight;
    void *borderImage;
    void *pulseImage;
    void *image;
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
            fn_80230150( x,fields->color, solidFlags | 8, y - (float) solidHeight, zero, (float) ((WidgetDispatch*)source)->Slot78(), (float) solidHeight, (float) fields->index, time, (60.0f));
            return;
        case 9:
            borderImage = fields->image;
            if (borderImage != 0) {
                borderWidth = ((ImageFillGlobalNew*)lbl_8055E050)->padding + (float) ((WidgetDispatch*)source)->Slot78();
                borderX = x - (borderWidth / 2.0f);
                x = borderX;
                borderHeight = (float) (fields->bottom - fields->top);
                fn_802354A0(borderImage, 0, -1U, borderX, y, zero, borderWidth, borderHeight);
                frameCount = fn_80235FD8(borderImage);
                if (frameCount >= 2) {
                    leftCapWidth = fn_80235EF0(borderImage, 1);
                    fn_802354A0(borderImage, 1, -1U, x - leftCapWidth, y, zero, leftCapWidth, borderHeight);
                }
                if (frameCount >= 3) {
                    fn_802354A0(borderImage, 2, -1U, x + borderWidth, y, zero, fn_80235EF0(borderImage, 2), borderHeight);
                    return;
                }
            }
            break;
        case 8:
            pulseImage = fields->image;
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

                    fn_802354A0(pulseImage, pulseFrame, -1U, x - (0.5f)*extra, y + (0.5f)*extra, (0.0f), extra + (float) ((WidgetDispatch*)source)->Slot78(), extra + (float) (fields->bottom - fields->top));
                    return;
                }
                fn_802354A0(pulseImage, pulseFrame, -1U, x, y, (0.0f), (float) ((WidgetDispatch*)source)->Slot78(), (float) (fields->bottom - fields->top));
                return;
            }
            break;
        case 3:
        case 6:
            image = fields->image;
            if (image != 0) {
                imageWatch = fields->watched;
                if (imageWatch != 0) {
                    imageFrame = *imageWatch;
                } else {
                    imageFrame = fields->index;
                }
                if ((int) fields->nativeSize != 0) {
                    nativeHeight = fn_80235F64(image, imageFrame);
                    fn_802354A0(image, imageFrame, -1U, x, y, (0.0f), fn_80235EF0(image, imageFrame), nativeHeight);
                    return;
                }
                fn_802354A0(image, imageFrame, -1U, x, y, (0.0f), (float) ((WidgetDispatch*)source)->Slot78(), (float) (fields->bottom - fields->top));
                return;
            }
            break;
        case 2:
            shaderImage = fields->image;
            if (shaderImage != 0) {
                fn_802304EC(fields->style, shaderImage, x, y, zero, fields->scale, fields->color, 9, (float) fields->index, time, (60.0f));
                return;
            }
            break;
        case 4:
            stretchImage = fields->image;
            if (stretchImage != 0) {
                fn_80230650(fields->style, stretchImage, x, y, zero, fields->scale, fields->color, 0x41, (float) fields->index, time, (40.0f));
                return;
            }
            break;
        case 5:
            gradientImage = fields->image;
            if (gradientImage != 0) {
                fn_802307AC(fields->style, gradientImage, x, y, zero, fields->scale, fields->color, 9, (float) fields->index, time, (5.0f));
                return;
            }
            break;
        case 7:
            fn_80230924(fields->color, x, y, x + (float) ((WidgetDispatch*)source)->Slot78(), y - (float) (fields->bottom - fields->top), zero);
            break;
        }
    }
}
WidgetFragment* fn_8021A5B8(WidgetFragment* self,int resource,unsigned char a,unsigned char b,int option,float amount){
 fn_80218F34(self,0,0,0,0,(int)amount);self->vtable=lbl_8033F3B0;
 PlaybackPrefix* fields=(PlaybackPrefix*)self;fields->resource=resource;fields->amount=amount;fields->flag250=a;fields->flag251=b;fields->option=option;fields->active=0;return self;
}
WidgetFragment* fn_8021A650(WidgetFragment* self,int destroy){
 if(self){fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;
}
void fn_8021A6A8(WidgetFragment* self){
 PlaybackPrefix* fields=(PlaybackPrefix*)self;
 if(fields->active){fn_8022FFE8(fields->active);fn_8022F8CC(fields->active);}
}
void fn_8021A6EC(WidgetFragment* self){
 PlaybackPrefix* fields=(PlaybackPrefix*)self;
 fields->flag250=1;fields->flag251=1;fields->amount=(30.0f);fields->option=1;
 fields->active=fn_8022F638(fields->resource,1,1,1,(30.0f));
}
void fn_8021A744(WidgetFragment* self){PlaybackPrefix* fields=(PlaybackPrefix*)self;fn_8022F824(fields->active);fields->active=0;}
WidgetFragment* fn_8021A77C(WidgetFragment* self,int x,int y,int kind,void* watched,OwnedWidget* child){
 fn_80222340(self,x,y,0,0,0,child);self->vtable=lbl_8033F324;
 WatchedChildPrefix* fields=(WatchedChildPrefix*)self;fields->watched=watched;fields->kind=kind;
 if(watched)fn_80219440(self,0,watched,4);return self;
}
int fn_8021A804(WidgetFragment* self,int key){return fn_802192A4(self,key);}
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
 unsigned char observed=*(unsigned char*)fn_80216708();int mode=0;if(observed)mode=2;
 WatchedChildPrefix* fields=(WatchedChildPrefix*)self;int selected=0;
 if(fields->watched&&*(int*)fields->watched==fields->kind)selected=1;
 return (selected!=0)|mode;
}
void fn_8021AA18(FocusPaintPrefix* self){FocusTemporaryNew temporary;FullDrawStatePrefix state;const FullDrawStatePrefix* live=(const FullDrawStatePrefix*)fn_8022EDC4();state.matrix=live->matrix;state.light=live->light;state.alpha=live->alpha;state.first=live->first;state.second=live->second;temporary.selected=0;if(self->focus&&*self->focus==self->token)temporary.selected=1;if(!*(unsigned char*)fn_80216708()&&!temporary.selected)state.alpha*=(0.3f);fn_8022ED00(&state);int height=((WidgetFragment*)self)->bottom-((WidgetFragment*)self)->top;temporary.label=((WidgetDispatch*)self)->Slot88();int width=((WidgetDispatch*)self)->Slot78();self->child->Paint(self->first,self->second,self->third,width,height,temporary.label);fn_8022EDB8();}
int fn_8021AB88(WidgetFragment*){return 0;}
int fn_8021AB90(WidgetFragment* self,int key){if(*(unsigned char*)fn_80216708()){switch(key){case 0x126:fn_80217AC0(self->window);return 1;case 0x128:fn_802179A0(self->window);return 1;}}return fn_802192A4(self,key);}
WidgetFragment* fn_8021AC24(WidgetFragment* self,int x,int y,void* handle){
 int height=fn_802C5220(handle);int width=fn_802C51C0(handle);
 fn_80218F34(self,x,y,x+width,y+height,0);self->vtable=lbl_8033F6E0;
 TimedAnimationPrefix* fields=(TimedAnimationPrefix*)self;fields->handle=handle;fields->active=0;fields->elapsed=(0.0f);fields->period=(0.3f);return self;
}
int fn_8021ACC0(WidgetFragment* self,void* context){
 fn_802C3034(context);TimedAnimationPrefix* fields=(TimedAnimationPrefix*)self;
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
WidgetFragment* fn_8021AF9C(WidgetFragment* self,int destroy){if(self){fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;}
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
WidgetFragment* fn_8021B2B0(WidgetFragment* self,int x,int y,const char* label,int height,int* value,int width){
 fn_80219568(self,x,y,width,height,0,0);
 self->vtable=lbl_8033F20C;self->label=label;
 strcpy((char*)self+0x268,label);self->value=value;fn_80219718(self);return self;
}
void fn_8021B32C(WidgetFragment* self){fn_802192AC(self);CaptionBufferPrefix* f=(CaptionBufferPrefix*)self;const char* label=f->label;if(strcmp(label,f->text)!=0){strcpy(f->text,label);fn_80219718(self);WidgetMarkDirty(self);}}
int fn_8021B3A0(WidgetFragment* self){int* value=self->value;if(value)return *value;return 0;}
void fn_8021B3BC(WidgetFragment* self,void* context){fn_802C3034(context);fn_802C3424(self->x,self->y,self->width,self->height,(int)self->label);}
WidgetFragment* fn_8021B404(WidgetFragment* self,int destroy){
 if(self){if(self)fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}
 return self;
}
WidgetFragment* fn_8021B460(WidgetFragment* self,RotatingChoiceDescriptorNew* desc,const char* label,int* value){
fn_8021B2B0(self,desc->x,desc->y,label,8,value,desc->width);self->vtable=lbl_8033F180;RotatingChoiceFieldsNew* f=(RotatingChoiceFieldsNew*)self;f->mode=(u32)desc->flags>>31;
ChoiceResourceSpec spec={};spec.words[0]=(u32)desc->name;*((float*)&spec.words[1])=desc->scale;spec.words[2]=desc->flags;spec.words[3]=desc->option90;spec.words[4]=desc->option90;spec.words[7]=desc->option84;spec.words[10]=desc->option94;spec.words[11]=desc->option94;spec.words[14]=desc->option84;
int resource=fn_8022A86C(&spec);f->resource=resource;self->height=resource;fn_80219718(self);
ChoicePoseNew pose;pose.basis.m[0]=(1.0f);pose.basis.m[1]=(0.0f);pose.basis.m[2]=(0.0f);pose.basis.m[3]=(0.0f);pose.basis.m[4]=(1.0f);pose.basis.m[5]=(0.0f);pose.basis.m[6]=(0.0f);pose.basis.m[7]=(0.0f);pose.basis.m[8]=(1.0f);pose.translation.x=(0.0f);pose.translation.y=(0.0f);pose.translation.z=(0.0f);
RotationVec input={(float)desc->x-(320.0f),(240.0f)-(float)desc->y,0.0f};ChoiceApplyOffsetNew(&pose,&input);
float x,y,z;memcpy(&x,&desc->rx,4);memcpy(&y,&desc->ry,4);memcpy(&z,&desc->rz,4);
if((0.0f)==x&&(0.0f)==y&&(0.0f)==z)f->pose=0;else{RotationVec rotation={(0.017453292f)*x,(0.017453292f)*y,(0.017453292f)*z};RotationMatrix matrix;fn_80015A20(&matrix,&rotation);Mat3Mul__FP4Mat3PC4Mat3PC4Mat3(&pose.basis,&matrix,&pose.basis);input.x=-input.x;input.y=-input.y;ChoiceApplyOffsetNew(&pose,&input);f->pose=fn_8022EDE4((1.0f),&pose);}return self;
}
WidgetFragment* fn_8021B8AC(WidgetFragment* self,int destroy){if(self){ScopedAssetPrefix* f=(ScopedAssetPrefix*)self;self->vtable=lbl_8033F180;if(f->asset){fn_8022EE98(f->asset);f->asset=0;}fn_8022A8E4(f->resource);if(self)ObservedNullable_fn_8021915C(self);if(destroy>0)__dl__FPv(self);}return self;}
void fn_8021B944(WidgetFragment* self){ScopedAssetPrefix* f=(ScopedAssetPrefix*)self;if(f->asset){char scope[64];const void* context=fn_8022EDC4();fn_8022EEF8(scope,f->asset,context);fn_8022ED00(scope);}fn_80219660(self);if(f->asset)fn_8022EDB8();}
WidgetFragment* fn_8021B9AC(WidgetFragment* self,FormattedChoiceDescriptor* desc,PageTextList* provider,int column){fn_80218F80(self,(WidgetDesc*)desc);self->vtable=lbl_8033F0FC;fn_8021BA64(self,desc->width,desc->height,provider,column,desc->name,desc->font,desc->option90,desc->optiona4,desc->option94,desc->option98,(int)desc->lineHeight,desc->option84);return self;}
void fn_8021BA64(WidgetFragment* self,int width,int height,PageTextList* list,int column,const char* font,int flags,int value,int unused,int first,int second,int rowHeight,int third){PageTextPrefix* f=(PageTextPrefix*)self;int horizontal=0;f->column=column;f->list=list;f->rowHeight=rowHeight;f->image=0;int vertical=0;if(flags&4)horizontal=width/2;else if(flags&2)horizontal=width;if(flags&0x60)vertical=rowHeight;else if(flags&0x10)vertical=rowHeight/2;ChoiceResourceSpec spec={};spec.words[0]=(u32)font;spec.words[2]=flags;spec.words[3]=value;spec.words[4]=second;spec.words[5]=third;spec.words[6]=horizontal;spec.words[7]=vertical;spec.words[10]=first;spec.words[11]=second;spec.words[12]=third;spec.words[13]=horizontal;spec.words[14]=vertical;f->font=fn_8022A86C(&spec);f->iconOffset=(fn_8022B620(f->font,lbl_8055B878)-23)/2;fn_80219440(self,0,&list->version,4);fn_8022D924(list,height/rowHeight);}
WidgetFragment* fn_8021BBA0(WidgetFragment* self,int destroy){if(self){self->vtable=lbl_8033F0FC;OptionalResourcePrefix* f=(OptionalResourcePrefix*)self;fn_8022A8E4(f->resource);if(f->optional)fn_802C519C(f->optional);fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;}
void fn_8021BC20(WidgetFragment* self){PageTextPrefix* f=(PageTextPrefix*)self;int end;int x=self->left;int start=f->list->page*f->list->pageSize;int y=self->top;end=start+f->list->pageSize;if(f->list->count<end)end=f->list->count;int zero=0;for(int i=start;i<end;i++){int flags=0;const char* label=fn_8022D850(f->list,i,f->column,&flags);if(flags>2){flags-=3;float px,py;fn_80230C30(&px,&py,x-28,y+f->iconOffset);if(f->image){int frame=flags%fn_802C51AC(f->image);fn_802C5320(f->image,frame,px,py,(0.0f),(25.0f),(25.0f));}}fn_8022BAD0(f->font,x,y,0,0,0,label,flags,zero,(1.0f));y+=f->rowHeight;}}
void fn_8021BD48(WidgetFragment* self,void* context){fn_802C3034(context);TextCommandPrefix* f=(TextCommandPrefix*)self;fn_801E4BEC(f->text,0,9,self->left,self->top,f->value);}
WidgetFragment* fn_8021BD98(WidgetFragment* self,int x,int y,float* value,const char* format,int height,float scale,float bias){fn_80219568(self,x,y,0,height,0,0);FloatFormatPrefix* f=(FloatFormatPrefix*)self;f->value=value;self->vtable=lbl_8033EF68;f->scale=scale;f->bias=bias;f->format=format;f->text[0]=0;fn_80219440(self,0,value,4);fn_8021BE48(self);return self;}
void fn_8021BE48(WidgetFragment* self){FloatFormatPrefix* f=(FloatFormatPrefix*)self;sprintf(f->text,f->format,*f->value*f->scale+f->bias);fn_80130568(f->text);fn_80219718(self);WidgetMarkDirty(self);}
void fn_8021BEBC(WidgetFragment* self,int,void*){fn_8021BE48(self);}
WidgetFragment* fn_8021BEC0(WidgetFragment* self,int x,int y,FullFormatDispatch* target,int height,int left,int right,int lineY){fn_80219568(self,x,y,0,height,0,0);ExtendedFormatPrefix* f=(ExtendedFormatPrefix*)self;f->target=target;self->vtable=lbl_8033EEDC;f->left=left;f->right=right;f->lineY=lineY;void* buffer;int size;target->Bind(&buffer,&size);if(size>0)fn_80219440(self,0,buffer,size);fn_8021C12C(self);f->focus=0;f->token=fn_80228F40(x,y);return self;}
void fn_8021BF90(WidgetFragment* self){if(fn_80129BC4(self)){Bound2a8* f=(Bound2a8*)self;f->focus=fn_80217BD8(self->window,f->token,self);fn_80219440(self,1,f->focus,4);}}
void fn_8021BFEC(WidgetFragment* self){fn_80219660(self);ExtendedFormatPrefix* f=(ExtendedFormatPrefix*)self;if(f->left!=f->right){int selected=0;if(f->focus&&f->token==*f->focus)selected=1;void* left=fn_8021165C(0,2);void* right=fn_8021165C(1,2);float zero=(0.0f);float x=zero,y=zero;fn_80230C30(&x,&y,f->left,f->lineY);if(f->target->CanPrevious())fn_802C5310(left,selected,x,y,zero);fn_80230C30(&x,&y,f->right,f->lineY);if(f->target->CanNext())fn_802C5310(right,selected,x,y,zero);}}
void fn_8021C128(WidgetFragment* self,int,void*){fn_8021C12C(self);}
void fn_8021C12C(WidgetFragment* self){FormatPrefix* f=(FormatPrefix*)self;f->target->Format(f->text,80);fn_80219718(self);WidgetMarkDirty(self);}
int fn_8021C18C(WidgetFragment* self,int key){ExtendedFormatPrefix* f=(ExtendedFormatPrefix*)self;if(!f->focus)return 0;if(f->token!=*f->focus)return 0;float old=f->target->Value();switch(key){case 0x127:f->target->Increase();if(ObservedAbsDecision(old-f->target->Value())<=lbl_80558DC4[0])fn_80216D28();return 1;case 0x125:f->target->Decrease();if(ObservedAbsDecision(old-f->target->Value())<=lbl_80558DC4[0])fn_80216D28();return 1;case 0x126:if(((ChoiceWindowPrefix*)self->window)->depth>1){fn_80217D84(self->window);return 1;}break;case 0x128:if(((ChoiceWindowPrefix*)self->window)->depth>1){fn_80217C4C(self->window);return 1;}break;}return 0;}
int fn_8021C318(WidgetFragment* self){Bound2a8* f=(Bound2a8*)self;return !f->focus?0:*f->focus==f->token;}
WidgetFragment* fn_8021C344(WidgetFragment* self,int x,int y,FullFormatDispatch* target,int height,int left,int right,int lineY,const char* label,int alignment){fn_80219568(self,x,y,0,height,0,0);ExtendedLabeledFormatPrefix* f=(ExtendedLabeledFormatPrefix*)self;f->target=target;self->vtable=lbl_8033EE50;f->left=left;f->right=right;f->lineY=lineY;f->label=label;if(alignment==1)f->side=0;else f->side=1;f->labelLeft=left;f->font=8;int labelEnd=right+fn_802C51C0(fn_8021165C(1,2));f->labelRight=labelEnd-fn_8022B498(f->font,label);f->showArrows=0;void* buffer;int size;target->Bind(&buffer,&size);if(size>0)fn_80219440(self,0,buffer,size);fn_8021C6D0(self);f->focus=0;f->token=fn_80228F40(x,y);return self;}
void fn_8021C478(WidgetFragment* self){if(fn_80129BC4(self)){Bound2a8* f=(Bound2a8*)self;f->focus=fn_80217BD8(self->window,f->token,self);fn_80219440(self,1,f->focus,4);}}
void fn_8021C4D4(WidgetFragment* self){fn_80219660(self);ExtendedLabeledFormatPrefix* f=(ExtendedLabeledFormatPrefix*)self;int selected=0;if(f->focus&&*f->focus==f->token)selected=1;unsigned int flags=selected!=0;if(f->side==0&&f->label)fn_8022BAD0(f->font,f->labelLeft,f->lineY-3,0,0,0,f->label,flags,1,(0.0f));else if(f->side==1&&f->label)fn_8022BAD0(f->font,f->labelRight,f->lineY-3,0,0,0,f->label,flags,1,(0.0f));if(f->left!=f->right&&f->showArrows){selected=0;if(f->focus&&f->token==*f->focus)selected=1;void* left=fn_8021165C(0,2);void* right=fn_8021165C(1,2);float x=(0.0f),y=(0.0f);fn_80230C30(&x,&y,f->left,f->lineY);if(f->target->CanPrevious())fn_802C5310(left,selected,x,y,(0.0f));fn_80230C30(&x,&y,f->right,f->lineY);if(f->target->CanNext())fn_802C5310(right,selected,x,y,(0.0f));}}
void fn_8021C6CC(WidgetFragment* self,int,void*){fn_8021C6D0(self);}
void fn_8021C6D0(WidgetFragment* self){FormatPrefix* f=(FormatPrefix*)self;f->target->Format(f->text,80);fn_80219718(self);WidgetMarkDirty(self);}
int fn_8021C730(WidgetFragment* self,int key){ExtendedLabeledFormatPrefix* f=(ExtendedLabeledFormatPrefix*)self;if(!f->focus)return 0;if(f->token!=*f->focus)return 0;float old=f->target->Value();switch(key){case 0x127:if(f->showArrows){f->target->Increase();if(ObservedAbsDecision(old-f->target->Value())<=lbl_80558DC4[0])fn_80216D28();return 1;}else if(((ChoiceWindowPrefix*)self->window)->depth>1){fn_802180BC(self->window);return 1;}break;case 0x125:if(f->showArrows){f->target->Decrease();if(ObservedAbsDecision(old-f->target->Value())<=lbl_80558DC4[0])fn_80216D28();return 1;}else if(((ChoiceWindowPrefix*)self->window)->depth>1){fn_802180B0(self->window);return 1;}break;case 0x126:if(!f->showArrows&&((ChoiceWindowPrefix*)self->window)->depth>1){fn_802180C8(self->window);return 1;}break;case 0x128:if(!f->showArrows&&((ChoiceWindowPrefix*)self->window)->depth>1){fn_802180D4(self->window);return 1;}break;case 13:f->showArrows=!f->showArrows;return 1;case 27:if(f->showArrows){f->showArrows=0;return 1;}return 0;}return 0;}
int fn_8021C974(WidgetFragment*){return 0;}
WidgetFragment* fn_8021C97C(WidgetFragment* self,int x,int y,void* handle,int direction,FormatTarget* target){fn_8021AC24(self,x,y,handle);DirectionPrefix* f=(DirectionPrefix*)self;f->direction=direction;self->vtable=lbl_8033EDC8;f->target=target;return self;}
int fn_8021C9D8(WidgetFragment* self){DirectionPrefix* f=(DirectionPrefix*)self;if(f->direction>0)return f->target->Increase();return f->target->Decrease();}
WidgetFragment* fn_8021CA0C(WidgetFragment* self,int x,int y,int dx,int dy,ObservedValueProvider* provider){
 fn_80218F34(self,x,y,x+dx,y+dy,0);self->vtable=lbl_8033ED44;
 ProviderWidgetPrefix* fields=(ProviderWidgetPrefix*)self;fields->provider=provider;fields->active=0;
 void* value;int bytes;provider->Read(&value,&bytes);if(bytes>0)fn_80219440(self,0,value,bytes);
 fields->first=fn_802C5130(lbl_8033D924);fields->second=fn_802C5130(lbl_8033D934);
 fields->width=fn_802C51C0(fields->second);fields->elapsed=(-1.0f);return self;
}
WidgetFragment* fn_8021CAD4(WidgetFragment* self,int destroy){
 if(self){self->vtable=lbl_8033ED44;ProviderWidgetPrefix* fields=(ProviderWidgetPrefix*)self;
  fn_802C519C(fields->first);fn_802C519C(fields->second);fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);
 }return self;
}
void fn_8021CB4C(WidgetFragment* self,void* context){fn_802C3034(context);GaugeValuePrefix* f=(GaugeValuePrefix*)self;f->target->Value();float span=(float)(((WidgetDispatch*)self)->Slot78()-f->inset);}
void fn_8021CBBC(WidgetFragment* self){GaugeValuePrefix* f=(GaugeValuePrefix*)self;float x=(float)(self->left-lbl_8055FB9C/2);float y=CenteredGaugeY349(lbl_8055FBA0,self->top);fn_802C5310(f->background,0,x,y-(8.0f),(0.0f));float value=f->target->Value();int width=((WidgetDispatch*)self)->Slot78();int pixel=(int)(value*(float)(width-f->inset));pixel+=self->left;fn_80230C30(&x,&y,pixel,self->top+3);fn_802C5310(f->marker,0,x,y,(0.0f));}
void fn_8021CD04(WidgetFragment* self,int x,int y){
 ProviderWidgetPrefix* fields=(ProviderWidgetPrefix*)self;
 if(!fields->active){fields->active=1;fields->elapsed=fields->provider->Value();}
 int left=self->left;
 if(x<left-32||y<self->top-32||x>left+((WidgetDispatch*)self)->Slot78()+32||y>self->bottom+32){fields->provider->SetValue(fields->elapsed);}
 else{
  int left=self->left,knob=fields->width;int width=((WidgetDispatch*)self)->Slot78();
  float denominator=(float)(width-knob);float ratio=(float)(x-left-knob/2)/denominator;
  float one=(1.0f);float limited=one<ratio?one:ratio;
  float zero=(0.0f);limited=limited>zero?limited:zero;
  fields->provider->SetValue(limited);
 }
}
void fn_8021CE8C(WidgetFragment* self,int x,int y){if(((ProviderWidgetPrefix*)self)->active)((PointerEventDispatch*)self)->Slot3c(x,y);}
void fn_8021CEAC(WidgetFragment* self,int x,int y){((RowInputDispatch*)self)->Slot4c(x,y);((ProviderWidgetPrefix*)self)->active=0;}
WidgetFragment* fn_8021CEEC(WidgetFragment* self,int x,int y,int* value,const char* format,int height,float scale,float bias){
 fn_80219568(self,x,y,0,height,0,0);self->vtable=lbl_8033ECB8;
 NumericCaptionPrefix* fields=(NumericCaptionPrefix*)self;fields->value=value;fields->scale=scale;fields->bias=bias;fields->format=format;
 *((char*)self+0x264)=0;fn_80219440(self,0,value,4);fn_8021CF9C(self);return self;
}
void fn_8021CF9C(WidgetFragment* self){
 NumericCaptionPrefix* fields=(NumericCaptionPrefix*)self;
 float number=fields->bias+(float)*fields->value*fields->scale;
 sprintf(fields->text,fields->format,(int)((0.5f)+number));
 fn_80130568(fields->text);fn_80219718(self);WidgetMarkDirty(self);
}
void fn_8021D040(WidgetFragment* self,int,void*){fn_8021CF9C(self);}
WidgetFragment* fn_8021D044(WidgetFragment* self,int x,int y,int* value,const char* list,int height,int first,int second,int style){
 fn_80219568(self,x,y,0,height,0,0);self->vtable=lbl_8033EBA0;
 ChoiceWidgetPrefix* fields=(ChoiceWidgetPrefix*)self;
 fields->first=first;fields->second=second;fields->style=style;fields->value=value;fields->count=0;
 fields->choices=(const char**)__nwa__FUlPCc(fn_8022D1B4(list)*4,lbl_8033D940);
 while(*list){fields->choices[fields->count++]=list;list+=strlen(list)+1;}
 fn_80219440(self,0,value,4);fn_8021D61C(self);fields->focus=0;fields->token=fn_80228F40(x,y);return self;
}
void fn_8021D154(WidgetFragment* self){
 if(fn_80129BC4(self)){
  ChoiceWidgetPrefix* fields=(ChoiceWidgetPrefix*)self;
  fields->focus=fn_80217BD8(self->window,fields->token,self);
  fn_80219440(self,1,fields->focus,4);
 }
}
int fn_8021D1B0(WidgetFragment* self){
 ChoiceWidgetPrefix* fields=(ChoiceWidgetPrefix*)self;
 if(!fields->focus)return 0;
 if(self->enabled)return *fields->focus==fields->token;
 return 4;
}
int fn_8021D1F0(WidgetFragment* self,int key){
 ChoiceWidgetPrefix* fields=(ChoiceWidgetPrefix*)self;
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
  if(((ChoiceWindowPrefix*)self->window)->depth>1){fn_80217D84(self->window);return 1;}
  break;
 case 0x128:
  if(((ChoiceWindowPrefix*)self->window)->depth>1){fn_80217C4C(self->window);return 1;}
  break;
 }return 0;
}
WidgetFragment* fn_8021D300(WidgetFragment* self,int destroy){
 if(self){self->vtable=lbl_8033EBA0;__dla__FPv(((ChoiceWidgetPrefix*)self)->choices);
  if(self)fn_8021915C(self,0);
  if(destroy>0)__dl__FPv(self);
 }return self;
}
void fn_8021D378(WidgetFragment *self) {
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

    fn_80219660(self);ChoiceWidgetPrefix* fields=(ChoiceWidgetPrefix*)self;
    if ((int) fields->first != (int) fields->second) {
        focus = fields->focus;
        selected = 0;
        if ((focus != 0) && ((int) fields->token == (int) *focus)) {
            selected = 1;
        }
        firstImage = fn_8021165C(0, 2);
        secondImage = fn_8021165C(1, 2);
        scale = fn_8022EDC4()->scale;
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
void fn_8021D568(WidgetFragment* self,void* context){
 fn_802C3034(context);ChoiceWidgetPrefix* fields=(ChoiceWidgetPrefix*)self;
 if(fields->focus&&fields->token==*fields->focus){
  int x=self->left,y=self->top;int width=((WidgetDispatch*)self)->Slot78();
  fn_802C3424(x,y,x+width,self->bottom,lbl_8055FBC0);
 }
 fn_802195DC(self,context);
}
void fn_8021D614(WidgetFragment* self,int,int){}
void fn_8021D618(WidgetFragment* self,int,void*){fn_8021D61C(self);}
void fn_8021D61C(WidgetFragment* self){
 ChoiceWidgetPrefix* fields=(ChoiceWidgetPrefix*)self;int value=*fields->value;
 if(value>=0&&value<fields->count)strcpy((char*)self+0x268,fields->choices[value]);
 else strcpy((char*)self+0x268,lbl_8055B87C);
 fn_80219718(self);WidgetMarkDirty(self);
}
WidgetFragment* fn_8021D6A0(WidgetFragment* self,int x,int y,int* value,const char* list,int first,int second,int third,int fourth){
 fn_8021D044(self,x,y,value,list,8,0,0,0);self->vtable=lbl_8033EB14;
 ChoiceResourceSpec spec={};spec.words[0]=first;spec.words[2]=second;spec.words[3]=third;spec.words[4]=third;spec.words[5]=fourth;
 spec.words[10]=third;spec.words[11]=third;spec.words[12]=fourth;
 int resource=fn_8022A86C(&spec);((ChoiceResourcePrefix*)self)->resource=resource;self->height=resource;
 fn_80219718(self);return self;
}
WidgetFragment* fn_8021D758(WidgetFragment* self,int destroy){
 if(self){self->vtable=lbl_8033EB14;fn_8022A8E4(((ChoiceResourcePrefix*)self)->resource);fn_8021D300(self,0);if(destroy>0)__dl__FPv(self);}
 return self;
}
WidgetFragment* fn_8021D7C8(WidgetFragment* self,int x,int y,int dx,int dy,int height,ChoiceProviderDispatch* provider){
 fn_80219568(self,x,y,0,height,dx,dy);self->vtable=lbl_8033EC2C;
 ProviderChoicePrefix* fields=(ProviderChoicePrefix*)self;float time=fn_8023659C()-(0.3f);
 fields->provider=provider;fields->fontResource=height;fields->nextTime=time;fields->previousTime=time;fields->focus=0;
 fields->token=fn_80228F40(x,y);return self;
}
const char* fn_8021D864(WidgetFragment* self){return ((ProviderChoicePrefix*)self)->provider->Text();}
void fn_8021D878(WidgetFragment* self){
 if(fn_80129BC4(self)){ProviderChoicePrefix* fields=(ProviderChoicePrefix*)self;
  fields->focus=fn_80217BD8(self->window,fields->token,self);fn_80219440(self,1,fields->focus,4);
 }
}
int fn_8021D8D4(WidgetFragment* self){
 ProviderChoicePrefix* fields=(ProviderChoicePrefix*)self;
 if(!fields->focus)return 0;
 if(self->enabled){int status;if(fields->provider->Modal())status=fields->provider->Selected();else status=(*fields->focus==fields->token);return status!=0;}
 return 4;
}
int fn_8021D97C(WidgetFragment* self,int key){
 ProviderChoicePrefix* fields=(ProviderChoicePrefix*)self;
 if(fields->provider->Modal()){if(!fields->provider->AcceptsKeys())return 0;}
 else{if(!fields->focus)return 0;if(fields->token!=*fields->focus)return 0;}
 switch(key){
 case 0x127:
  if(fields->provider->HasNext()){fields->provider->Next();fields->nextTime=fn_8023659C();}else fn_80216D28();return 1;
 case 0x125:
  if(fields->provider->HasPrevious()){fields->provider->Previous();fields->previousTime=fn_8023659C();}else fn_80216D28();return 1;
 case 0x126:
  if(!fields->provider->Modal()&&((ChoiceWindowPrefix*)self->window)->depth>1){fn_80217D84(self->window);fn_80230E90(0);return 1;}break;
 case 0x128:
  if(!fields->provider->Modal()&&((ChoiceWindowPrefix*)self->window)->depth>1){fn_80217C4C(self->window);fn_80230E90(0);return 1;}break;
 }return 0;
}
void fn_8021DB58(WidgetFragment* self){
 ProviderChoicePrefix* fields=(ProviderChoicePrefix*)self;int selected;
 if(fields->provider->Modal())selected=fields->provider->Selected();
 else{selected=0;if(fields->focus&&fields->token==*fields->focus)selected=1;}
 float x=(0.0f),y=(0.0f);fn_80230C30(&x,&y,self->left,self->top);
 float available=(float)((WidgetDispatch*)self)->Slot78()+lbl_8055E050->margin;
 // MWCC retains conversion work even when the float result is unused; see UNUSED-FLOAT-CONVERSION-COMPILER-PROBE.json.
 float unusedHeight=(float)(self->bottom-self->top);
 fn_80219660(self);
 int previousFrame;if(selected){float now=fn_8023659C();if((0.3f)+fields->previousTime>=now)previousFrame=2;else previousFrame=1;}else previousFrame=0;
 int nextFrame;if(selected){float now=fn_8023659C();if((0.3f)+fields->nextTime>=now)nextFrame=2;else nextFrame=1;}else nextFrame=0;
 void* previousArrow=fn_80211660(0);void* nextArrow=fn_80211660(1);
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
 if((!fields->provider||fields->provider->HasPrevious())||ObservedAbsDecision(previousOffset)>lbl_80558DC4[0]){
  if(previousArrow){float width=fn_80235EF0(previousArrow,previousFrame);float height=fn_80235F64(previousArrow,previousFrame);
   fn_802354A0(previousArrow,previousFrame,previousColor,previousX,y+height/2.0f,(0.0f),width,height);}
 }
 if((!fields->provider||fields->provider->HasNext())||ObservedAbsDecision(nextOffset)>lbl_80558DC4[0]){
  if(nextArrow){float width=fn_80235EF0(nextArrow,nextFrame);float height=fn_80235F64(nextArrow,nextFrame);
   fn_802354A0(nextArrow,nextFrame,nextColor,nextX,y+height/2.0f,(0.0f),width,height);}
 }
}
WidgetFragment* fn_8021E0C0(WidgetFragment *self, int x, int y, int width, int height, ObservedStringList *list, int *selected, ObservedListViewport *view)
{
  fn_80218F34(self, x, y, x + width, y + height, 0);
  self->vtable = lbl_8033EA90;
  RowListPrefix *fields = (RowListPrefix *) self;
  fields->active = 0;
  fields->direction = 0;
  fields->elapsed = (0.0f);
  int rowHeight = fn_8022B620(7, lbl_8055B880) + 2;
  fields->columns = width / 8;
  fields->list = list;
  fields->rowHeight = rowHeight;
  fields->selected = selected;
  fields->view = view;
  fields->rows = height / rowHeight;
  if (view)
  {
    view->total = list->count;
    view->page = fields->rows;
    fn_80219440(self, 0, view, 12);
    fn_8021E7D4(self);
  }
  fn_80219440(self, 0, selected, 4);
  fn_80219440(self, 0, &list->version, 4);
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
 fn_802192AC(self);RowListPrefix* fields=(RowListPrefix*)self;
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
 fn_802192A0(self,context);fn_802C3034(context);int flags=0;color=lbl_8055FBDC;
 for(int row=0;row<fields->rows&&(index=row+start,fn_8022D3F8(fields->list,index,&flags)!=0);++row){
  int x,y;bool selected=*fields->selected==index;
  if(selected){x=self->left;y=self->top;int width=x+((WidgetDispatch*)self)->Slot78();
   {int h=fields->rowHeight;int bottom=(row+1)*h;int top=row*h;fn_802C3424(x+2,y+top+2,width-2,y+bottom+2,color);}
  }
  int disabled=flags!=0?2:0;selected=selected!=0;drawY=self->top;drawX=self->left;const char* label=fn_8022D3F8(fields->list,index,0);fn_8022B91C(7,drawX+2,drawY+row*fields->rowHeight+2,label,selected|disabled);
 }
}
void fn_8021E51C(WidgetFragment* self){
 int y;int x;RowListPrefix* f=(RowListPrefix*)self;int start=0;if(f->view)start=f->view->start;int flags=0;int index;
 for(int row=0;row<f->rows&&(index=row+start,fn_8022D3F8(f->list,index,&flags)!=0);++row){
 int disabled;bool selected;y=self->top;x=self->left;selected=*f->selected==index;disabled=flags!=0?2:0;selected=selected!=0;
 const char* label=fn_8022D3F8(f->list,index,0);
 fn_8022BAD0(7,x+2,y+row*f->rowHeight+2,0,0,0,label,selected|disabled,0,(1.0f));
 }
}
int fn_8021E620(WidgetFragment* self,int key){
 RowListPrefix* f=(RowListPrefix*)self;
 switch(key){
 case 0x126:if(*f->selected>0)--*f->selected;fn_8021E7D4(self);return 1;
 case 0x128:if(*f->selected+1<f->list->count)++*f->selected;fn_8021E7D4(self);return 1;
 case 0x122:{int last=f->list->count-1;int next=*f->selected+f->view->page;*f->selected=last<next?last:next;
 SetListViewport(f->view,f->view->start+f->view->page);
 fn_8021E7D4(self);return 1;}
 case 0x121:{int next=*f->selected-f->view->page;*f->selected=next<0?0:next;
 SetListViewport(f->view,f->view->start-f->view->page);
 fn_8021E7D4(self);return 1;}
 case 0x124:*f->selected=0;fn_8021E7D4(self);return 1;
 case 0x123:*f->selected=f->list->count-1;fn_8021E7D4(self);return 1;
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
 if(self){fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;
}
void fn_8021E8DC(WidgetFragment* self,void* context){DropdownRowPrefix* f=(DropdownRowPrefix*)self;if(f->list.active){((WidgetDispatch*)self)->Slot1c(self->left,self->bottom+3);int height=self->bottom-self->top;int width=((WidgetDispatch*)self)->Slot78();fn_80219284(self,f->width,f->height);fn_8021E3BC(self,context);fn_80219284(self,width,height);((WidgetDispatch*)self)->Slot1c(self->left,self->top*2-3-self->bottom);}fn_802C3034(context);fn_802192A0(self,context);int y,x;x=self->left;y=self->top;int width=((WidgetDispatch*)self)->Slot78();fn_802C3424(x,y,x+width,self->bottom,lbl_8055FBDC);const char* label=fn_8022D3F8(f->list.list,*f->list.selected,0);char buffer[80];sprintf(buffer,lbl_8055B884,f->list.columns,label?label:lbl_8055B88C);fn_802C4F58(self->left+1,self->top+1,buffer,lbl_8055FBC0);}
WidgetFragment* fn_8021EA64(WidgetFragment* self,int x,int y,int width,int height,char* buffer,int capacity,int stride,int font,bool clear){
 fn_80218F34(self,x,y,x+width,y+height+3,0);self->vtable=lbl_8033E988;
 if(clear)buffer[0]=0;TextEntryPrefix* fields=(TextEntryPrefix*)self;
 fields->buffer=buffer;fields->font=font;fields->capacity=capacity<0?-capacity:capacity;fields->stride=stride;
 if(stride==1&&capacity>0)fields->second=fn_802C5130(lbl_8033D96C);else fields->second=0;
 fields->state=0;fields->first=fn_802C5130(lbl_8033D978);fields->cursor=strlen(buffer);
 fn_80219440(self,0,buffer,fields->capacity*fields->stride);
 if(fields->second){int height=fn_802C5220(fields->second);int width=((WidgetDispatch*)self)->Slot78();fn_80219284(self,width,height);}
 return self;
}
WidgetFragment* fn_8021EB9C(WidgetFragment* self,int destroy){
 if(self){self->vtable=lbl_8033E988;TextEntryPrefix* fields=(TextEntryPrefix*)self;
  fn_802C519C(fields->first);if(fields->second)fn_802C519C(fields->second);fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);
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

    fn_802C3034(context);
    initialLeft = fields->left;
    initialTop = fields->top;
    fn_802C2FC8(context, initialLeft, initialTop, initialLeft + ((WidgetDispatch*)self)->Slot78(), fields->bottom);
    fn_802192A0(self, context);
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
 default:if(!(key&0x100)&&(unsigned int)key>31){text=f->buffer;if(strlen(text)<(unsigned int)(capacity-1)){int i=strlen(text)+1;while(i>f->cursor){f->buffer[i]=f->buffer[i-1];i--;}f->buffer[f->cursor++]=(char)key;}WidgetMarkDirty(self);}else return fn_802192A4(self,key);break;
 }
 return 1;
}
WidgetFragment* fn_8021F394(WidgetFragment* self,int x,int y,int parameter,int* value,int height){fn_80219568(self,x,y,0,height,0,0);BindingFieldPrefix* f=(BindingFieldPrefix*)self;f->parameter=parameter;self->vtable=lbl_8033E8FC;f->value=value;fn_801A7E54(f->buffer,24,(int)value);fn_80219440(self,0,value,16);fn_80219718(self);return self;}
void fn_8021F42C(WidgetFragment* self,int,void*){fn_801A7E54((char*)self+0x254,24,*(int*)((char*)self+0x270));WidgetMarkDirty(self);}
int fn_8021F478(int* result){fn_801A7844(1);KeyboardFilePrefix* file=&lbl_80490934;if(fn_801A7C8C(file->handle)){fn_801A7E54(file,24,file->handle);fn_802E47E8();*result=-2;return 1;}return 0;}
void fn_8021F4F0(void) {}
WidgetFragment* fn_8021F678(WidgetFragment* self,int x,int y,int width,int height,float minimum,float maximum,float* value,int rate){fn_80218F34(self,x,y,x+width,y+height,0);FloatSliderPrefix* f=(FloatSliderPrefix*)self;self->vtable=lbl_8033E878;f->value=value;f->minimum=minimum;f->maximum=maximum;f->dragging=0;f->rate=InlineWidgetAbsolute(rate);f->negative=rate<0;f->background=fn_802C5130(lbl_8033D9B0);f->marker=fn_802C5130(lbl_8033D9BC);f->oldX=self->left;f->oldY=self->top;f->oldWidth=((WidgetDispatch*)self)->Slot78();f->oldHeight=self->bottom-self->top;int newHeight=fn_802C5220(f->background);if(self->bottom-self->top>newHeight)newHeight=self->bottom-self->top;fn_80219284(self,((WidgetDispatch*)self)->Slot78()-1,newHeight);fn_80219440(self,0,value,4);f->focus=0;f->token=fn_80228F40(x,y);return self;}
void fn_8021F7EC(WidgetFragment* self){if(fn_80129BC4(self)){Bound274* f=(Bound274*)self;f->focus=fn_80217BD8(self->window,f->token,self);fn_80219440(self,1,f->focus,4);}}
int fn_8021F848(WidgetFragment* self,int key){FloatSliderPrefix* f=(FloatSliderPrefix*)self;if(!f->focus)return 0;if(f->token!=*f->focus)return 0;float old=*f->value;switch(key){case 0x127:{int index=(int)((0.005f)+(float)(f->rate-1)*(old-f->minimum)/(f->maximum-f->minimum));float value=f->minimum+(float)(index+1)*(f->maximum-f->minimum)/(float)(f->rate-1);if(value<f->minimum)value=f->minimum;if(value>f->maximum)value=f->maximum;*f->value=value;if(ObservedAbsDecision(*f->value-old)<=lbl_80558DC4[0])fn_80216D28();return 1;}case 0x125:{int index=(int)((0.005f)+(float)(f->rate-1)*(old-f->minimum)/(f->maximum-f->minimum));float value=f->minimum+(float)(index-1)*(f->maximum-f->minimum)/(float)(f->rate-1);if(value<f->minimum)value=f->minimum;if(value>f->maximum)value=f->maximum;*f->value=value;if(ObservedAbsDecision(*f->value-old)<=lbl_80558DC4[0])fn_80216D28();return 1;}case 0x126:if(((ChoiceWindowPrefix*)self->window)->depth>1){fn_80217D84(self->window);return 1;}break;case 0x128:if(((ChoiceWindowPrefix*)self->window)->depth>1){fn_80217C4C(self->window);return 1;}break;}return 0;}
void fn_8021FA98(WidgetFragment *self, int x, int y)
{
  FloatSliderPrefix *f = (FloatSliderPrefix *) self;
  if (f->dragging && fn_8021FBC4(self, x, y))
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
void fn_8021FC40(WidgetFragment* self,void* context){fn_802C3034(context);fn_802192A0(self,context);FloatSliderPrefix* f=(FloatSliderPrefix*)self;int top,left;int span=((WidgetDispatch*)self)->Slot78()-11;int fill;int selected=0;fill=(int)((float)span*(*f->value-f->minimum)/(f->maximum-f->minimum));if(f->focus&&*f->focus==f->token)selected=1;top=self->top;left=self->left;int width=((WidgetDispatch*)self)->Slot78();fn_80228DC4((ObservedClipPrefix*)context,f->background,left,top,width,selected);fn_80228DC4((ObservedClipPrefix*)context,f->marker,self->left+5,self->top+5,fill,selected);}
void fn_8021FD68(WidgetFragment* self){FloatSliderPrefix* f=(FloatSliderPrefix*)self;int span=((WidgetDispatch*)self)->Slot78()-11;int fill;int selected=0;fill=(int)((float)span*(*f->value-f->minimum)/(f->maximum-f->minimum));if(f->focus&&*f->focus==f->token)selected=1;float width=(float)((WidgetDispatch*)self)->Slot78();float height=(float)(self->bottom-self->top);float x=(float)(self->left-lbl_8055FB9C/2),y=(float)(lbl_8055FBA0/2-self->top);fn_802C5320(f->background,selected,x,y,(0.0f),width,height);fn_802C5334(f->marker,selected,x,y,(0.0f),(float)fill,height,(0.0f),(0.0f),(float)fill/width,(1.0f));}
WidgetFragment* fn_8021FF34(WidgetFragment* self,WidgetDesc* desc,float* value,int divisions,float minimum,float maximum){fn_80218F34(self,desc->x,desc->y,desc->x+desc->dx,desc->y+desc->dy,0);self->vtable=lbl_8033E7F4;SteppedFloatPrefix* f=(SteppedFloatPrefix*)self;f->value=value;f->minimum=minimum;f->maximum=maximum;f->step=(maximum-minimum)/(float)divisions;f->image=fn_802C5130(lbl_8033D9BC);f->focus=0;f->token=fn_80228F40(desc->x,desc->y);f->nextTime=fn_8023659C()-(0.3f);f->previousTime=f->nextTime;return self;}
WidgetFragment* fn_8022003C(WidgetFragment* self,int destroy){if(self){self->vtable=lbl_8033E7F4;fn_802C519C((void*)self->label);fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;}
void fn_802200AC(WidgetFragment* self){int previousFrame;
 int nextFrame;
 void* previousArrow;
 void* nextArrow;
 float nextX;
 float fraction;
 float height;
 float filledWidth;
 SteppedFloatPrefix* f=(SteppedFloatPrefix*)self;
 if(f->value)fraction=(*f->value-f->minimum)/(f->maximum-f->minimum);
 else fraction=(1.0f);
 float coordinates[2];
 coordinates[1]=(0.0f);
 coordinates[0]=(0.0f);
 fn_80230C30(&coordinates[1],&coordinates[0],self->left,self->top);
 float width=(float)((WidgetDispatch*)self)->Slot78();
 height=(float)(self->bottom-self->top);
 filledWidth=fraction*(float)fn_802C51C0(f->image);
 float imageHeight=(float)fn_802C5220(f->image);
 previousFrame=0;
 if(f->focus&&*f->focus==f->token)previousFrame=1;
 ((void(*)(void*,int,float,float,float,float,float,float,float,float,float))fn_802C5334)(f->image,previousFrame,coordinates[1]+(GaugeHalfReferenceObservedNew())*(width-(float)fn_802C51C0(f->image)),coordinates[0]+(imageHeight-height)*GaugeHalfReferenceObservedNew(),(0.0f),filledWidth,imageHeight,(0.0f),(0.0f),fraction,(1.0f));
 
previousArrow=fn_80211660(0);
 nextArrow=fn_80211660(1);
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
 if(fraction<=lbl_80558DC4[0])previousColor=ObservedArrowColor(&f->previousTime);
 if(fraction>=(1.0f)-lbl_80558DC4[0])nextColor=ObservedArrowColor(&f->nextTime);
 
if(previousArrow){float arrowWidth=fn_80235EF0(previousArrow,previousFrame);
 float arrowHeight=fn_80235F64(previousArrow,previousFrame);
 fn_802354A0(previousArrow,previousFrame,previousColor,previousOffset,coordinates[0]+(arrowHeight-height)*(GaugeHalfReferenceObservedNew()),(0.0f),arrowWidth,arrowHeight);
 }if(nextArrow){float arrowWidth=fn_80235EF0(nextArrow,nextFrame);
 float arrowHeight=fn_80235F64(nextArrow,nextFrame);
 fn_802354A0(nextArrow,nextFrame,nextColor,nextX,coordinates[0]+(arrowHeight-height)*(GaugeHalfReferenceObservedNew()),(0.0f),arrowWidth,arrowHeight);
 }}
int fn_802205AC(WidgetFragment* self,int key){SteppedFloatPrefix* f=(SteppedFloatPrefix*)self;if(!f->focus||*f->focus!=f->token)return 0;switch(key){case 0x127:if(f->value&&*f->value!=f->maximum){f->nextTime=fn_8023659C();*f->value=InlineWidgetMinimum(*f->value+f->step,f->maximum);}break;case 0x125:if(f->value&&*f->value!=f->minimum){f->previousTime=fn_8023659C();*f->value=InlineWidgetMaximum(*f->value-f->step,f->minimum);}break;case 0x126:if(((ChoiceWindowPrefix*)self->window)->depth>1){fn_80217D84(self->window);fn_80230E90(0);}else return 0;break;case 0x128:if(((ChoiceWindowPrefix*)self->window)->depth>1){fn_80217C4C(self->window);fn_80230E90(0);}else return 0;break;default:return 0;}return 1;}
void fn_8022071C(WidgetFragment* self){if(fn_80129BC4(self)){Bound258* f=(Bound258*)self;f->focus=fn_80217BD8(self->window,f->token,self);fn_80219440(self,1,f->focus,4);}}
WidgetFragment* fn_80220778(WidgetFragment* self,int x,int y,int width,int height,int* value,int minimum,int maximum){fn_80218F34(self,x,y,x+width,y+height,0);SliderFullPrefix* f=(SliderFullPrefix*)self;f->value=value;self->vtable=lbl_8033E668;f->minimum=minimum;f->maximum=maximum;fn_80219440(self,0,value,4);f->background=fn_802C5130(lbl_8033D9B0);f->marker=fn_802C5130(lbl_8033D9BC);if(f->background){int height=fn_802C5220(f->background);int width=((WidgetDispatch*)self)->Slot78();fn_80219284(self,width,height);}f->dragging=0;f->focus=0;f->token=fn_80228F40(x,y);return self;}
WidgetFragment* fn_80220874(WidgetFragment* self,int destroy){if(self){self->vtable=lbl_8033E668;PairImage248* f=(PairImage248*)self;fn_802C519C(f->first);fn_802C519C(f->second);fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;}
void fn_802208EC(WidgetFragment* self){if(fn_80129BC4(self)){Bound25c* f=(Bound25c*)self;f->focus=fn_80217BD8(self->window,f->token,self);fn_80219440(self,1,f->focus,4);}}
int fn_80220948(WidgetFragment* self,int key){SliderFullPrefix* f=(SliderFullPrefix*)self;if(!f->focus)return 0;if(f->token!=*f->focus)return 0;switch(key){case 0x127:if(*f->value<f->maximum-1)++*f->value;else fn_80216D28();return 1;case 0x125:if(*f->value>f->minimum)--*f->value;else fn_80216D28();return 1;case 0x126:if(((ChoiceWindowPrefix*)self->window)->depth>1){fn_80217D84(self->window);return 1;}break;case 0x128:if(((ChoiceWindowPrefix*)self->window)->depth>1){fn_80217C4C(self->window);return 1;}break;}return 0;}
void fn_80220A50(WidgetFragment* self,int x,int unused){SliderFullPrefix* f=(SliderFullPrefix*)self;f->dragging=1;int left=self->left;int minimum=f->minimum;int width=((WidgetDispatch*)self)->Slot78();int value=(f->maximum-minimum)*(x-left-5);SliderDenominatorWordNew denominator=(SliderDenominatorWordNew)(width-10);width=value/(int)denominator;value=minimum+width;if(value<minimum)value=minimum;value=ClampToExclusiveMaximum(f->maximum,value);*f->value=value;}
void fn_80220AFC(WidgetFragment* self,int x,int y){((RowInputDispatch*)self)->Slot4c(x,y);((PrefixFlag258*)self)->value=0;}
void fn_80220B3C(WidgetFragment* self,int x,int y){if(((PrefixFlag258*)self)->value)((PointerEventDispatch*)self)->Slot3c(x,y);}
void fn_80220B5C(WidgetFragment* self,void* context){int top,left;fn_802C3034(context);SliderFullPrefix* f=(SliderFullPrefix*)self;int span=((WidgetDispatch*)self)->Slot78()-11;int fill=(span*(*f->value-f->minimum))/(f->maximum-f->minimum-1);int selected=0;if(f->focus&&*f->focus==f->token)selected=1;top=self->top;left=self->left;int width=((WidgetDispatch*)self)->Slot78();fn_80228DC4((ObservedClipPrefix*)context,f->background,left,top,width,selected);fn_80228DC4((ObservedClipPrefix*)context,f->marker,self->left+5,self->top+5,fill,selected);}
void fn_80220C54(WidgetFragment* self){float height;SliderFullPrefix* f=(SliderFullPrefix*)self;int span=((WidgetDispatch*)self)->Slot78()-11;int fill=(span*(*f->value-f->minimum))/(f->maximum-f->minimum-1);int selected=0;if(f->focus&&*f->focus==f->token)selected=1;float x=(float)(self->left-lbl_8055FB9C/2);float y=(float)(lbl_8055FBA0/2-self->top);height=(float)fn_802C5220(f->background);fn_802C5320(f->background,selected,x,y,(0.0f),(float)((WidgetDispatch*)self)->Slot78(),height);height=(float)fn_802C5220(f->marker);fn_802C5320(f->marker,selected,(5.0f)+x,y-(5.0f),(0.0f),(float)fill,height);}
void fn_80220E08(WidgetFragment* self,void* context){fn_802C3034(context);fn_802C40B8(self->x,self->left,self->top);}
WidgetFragment* fn_80220E48(WidgetFragment* self,int x,int y,int width,int height,void* view,int vertical){const char* strings=lbl_8033D878;fn_80218F34(self,x,y,x+width,y+height,0);self->vtable=lbl_8033E770;ScrollbarPrefix* f=(ScrollbarPrefix*)self;if(!vertical){f->track=fn_802C5130(strings+0x150);f->marker=fn_802C5130(strings+0x15c);}else{f->track=fn_802C5130(strings+0x168);f->marker=fn_802C5130(strings+0x174);}((WidgetDispatch*)self)->Slot1c(self->left,self->top);if(!vertical){int width=fn_802C51C0(f->track);fn_80219284(self,width,self->bottom-self->top);}else{int height=fn_802C5220(f->track);int width=((WidgetDispatch*)self)->Slot78();fn_80219284(self,width,height);}f->view=view;f->vertical=vertical;fn_80219440(self,0,view,12);f->dragging=0;return self;}
void fn_80220F90(WidgetFragment* self,int x,int y){
 DragViewportPrefix* f=(DragViewportPrefix*)self;f->dragging=1;float ratio;
 if(f->direction==0){
 ObservedListViewport* view=f->state;int top=self->top;ratio=(float)(self->bottom-top)/(float)view->total;
 int start=view->start;int low=top+(int)(ratio*(float)start)+5;int high=top+(int)(ratio*(float)(start+view->page))-5;
 if(y>=low&&y<=high){f->grabY=(int)(ratio*(float)start)-y;}
 else {if(y<low){int position=(int)((float)start-((1.0f)/ratio)*(float)(high-low+5));SetListViewportValue(position,view);}
 else {int position=(int)(AddScroll358((float)start,((1.0f)/ratio)*(float)(high-low+5)));SetListViewportValue(position,view);}f->dragging=0;}
 }else{
 int total=f->state->total;int width=((WidgetDispatch*)self)->Slot78();ObservedListViewport* view=f->state;
 ratio=(float)width/(float)total;int start=view->start;int left=self->left;int low=left+(int)(ratio*(float)start)+5;int high=left+(int)(ratio*(float)(start+view->page))-5;
 if(x>=low&&x<=high){f->grabX=(int)(ratio*(float)start)-x;}
 else {if(x<low){int position=(int)((float)start-((1.0f)/ratio)*(float)(high-low+5));SetListViewportValue(position,view);}
 else {int position=(int)(AddScroll358((float)start,((1.0f)/ratio)*(float)(high-low+5)));SetListViewportValue(position,view);}f->dragging=0;}
 }
}
void fn_802213A4(WidgetFragment* self,int x,int y){
 DragViewportPrefix* f=(DragViewportPrefix*)self;
 if(f->dragging){
 if(f->direction==0){
 ObservedListViewport* state=f->state;
 int value=(int)(((float)state->total/(float)(self->bottom-self->top))*(float)(y+f->grabY));
 state->start=value;if(value>state->total-state->page)state->start=state->total-state->page;if(state->start<0)state->start=0;
 }else{
 int total=f->state->total;float ratio=(float)total/(float)((WidgetDispatch*)self)->Slot78();ObservedListViewport* state=f->state;int value=(int)(ratio*(float)(x+f->grabX));
 state->start=value;if(value>state->total-state->page)state->start=state->total-state->page;if(state->start<0)state->start=0;
 }
 }
}
void fn_80221538(WidgetFragment* self,int,int){((PrefixFlag250*)self)->value=0;}
void fn_80221544(WidgetFragment *self, void *context)
{
  int imageExtent;

  int trackWidth;

  DragViewportPrefix *f = (DragViewportPrefix *) self;

  fn_802C3034(context);

  if (f->direction == 0)
  {
    ((void (*)(ObservedClipPrefix *, void *, int, int, int, int)) fn_80228E80)((ObservedClipPrefix *) context, f->first, self->left, self->top, self->bottom - self->top, 0);

  }
  else
  {
    ScrollLocalWordObservedNew y(self->top);

    ScrollLocalWordObservedNew x(self->left);

    int width = ((WidgetDispatch*)self)->Slot78();

    fn_80228DC4((ObservedClipPrefix *) context, f->first, x.value, y.value, width, 0);

  }
  ScrollLocalWordObservedNew total(f->state->total);

  if (f->state->page < total.value)
  {
  if (f->direction == 0) { int left;
 int right;
 int top;
 int bottom;
 ScrollLocalWordObservedNew x(self->left);
 left = x.value + 1;
 float ratio = ((float) (self->bottom - self->top)) / ((float) total.value);
 trackWidth = ((WidgetDispatch*)self)->Slot78();
 int rightEdge = x.value + trackWidth;
 right = rightEdge - 1;
 const int& start=f->state->start;
 top=(self->top + ((int) (ratio * ((float) start))))+5;
bottom=(self->top + ((int) (ratio * ((float) (start + f->state->page)))))-5;
 if (((bottom - 1) > (top + 1)) && ((right - 1) > (left + 1))) { if (f->second) { void *image = f->second;
 imageExtent = fn_802C51C0(f->first);
 int tw = fn_802C51C0(image);
 ((void (*)(ObservedClipPrefix *, void *, int, int, int, int)) fn_80228E80)((ObservedClipPrefix *) context, f->second, left + ((imageExtent - tw) / 2), top, bottom - top, 0);
 }
else { fn_802C3424(left, top, right, bottom, lbl_8055FBDC);
 int dark = lbl_8055FBE0;
 fn_802C3424(left, bottom - 1, right, bottom, dark);
 fn_802C3424(right - 1, top, right, bottom, dark);
 int light = lbl_8055FBFC;
 fn_802C3424(left, top, right, top + 1, light);
 fn_802C3424(left, top, left + 1, bottom, light);
 } } }
else { int left;
 int right;
 int top;
 int bottom;
 int width = ((WidgetDispatch*)self)->Slot78();
 float ratio = ((float) width) / ((float) total.value);
 int start = f->state->start;
 int edge = self->bottom;
 ScrollLocalWordObservedNew y(self->top);
 top = y.value + 1;
 bottom = edge - 1;
 int low = self->left + ((int) (ratio * ((float) start)));
 ScrollEdgeWordObservedNew high(self->left + ((int) (ratio * ((float) (start + f->state->page)))));
 left = low + 5;
 right = high.value - 5;
 if (((right - 1) > (left + 1)) && ((bottom - 1) > (top + 1))) { if (f->second) { void *image = f->second;
 imageExtent = fn_802C5220(f->first);
 int th = fn_802C5220(image);
 fn_80228DC4((ObservedClipPrefix *) context, f->second, left, top + ((imageExtent - th) / 2), right - left, 0);
 }
else { fn_802C3424(left, top, right, bottom, lbl_8055FBDC);
 int dark = lbl_8055FBE0;
 fn_802C3424(left, bottom - 1, right, bottom, dark);
 fn_802C3424(right - 1, top, right, bottom, dark);
 int light = lbl_8055FBFC;
 fn_802C3424(left, top, right, top + 1, light);
 fn_802C3424(left, top, left + 1, bottom, light);
 } } } 
  }
}
void fn_80221970(WidgetFragment* self){
  int imageExtent;
  int trackWidth;
  DragViewportPrefix* f=(DragViewportPrefix*)self;
  float x=(float)(self->left-(lbl_8055FB9C/2));
  float y=(float)((lbl_8055FBA0/2)-self->top);
  float initialWidth=(float)((WidgetDispatch*)self)->Slot78();
  // Width is measured before these height reads, as in the original call sequence.
  fn_802C5320(f->first,0,x,y,(0.0f),initialWidth,(float)(self->bottom-self->top));
  ScrollLocalWordObservedNew total(f->state->total);
  if (f->state->page < total.value) {
    if (f->direction == 0) {
      int left;
      int right;
      int top;
      int bottom;
      ScrollLocalWordObservedNew originX(self->left);
      left = originX.value + 1;
      float ratio = ((float) (self->bottom - self->top)) / ((float) total.value);
      trackWidth = ((WidgetDispatch*)self)->Slot78();
      int rightEdge = originX.value + trackWidth;
      right = rightEdge - 1;
      const int& start=f->state->start;
      top=(self->top + ((int) (ratio * ((float) start))))+5;
      bottom=(self->top + ((int) (ratio * ((float) (start + f->state->page)))))-5;
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
          fn_802E30C8(lbl_8033D9F8);
          fn_802C3424(left, top, right, bottom, lbl_8055FBDC);
          int dark = lbl_8055FBE0;
          fn_802C3424(left, bottom - 1, right, bottom, dark);
          fn_802C3424(right - 1, top, right, bottom, dark);
          int light = lbl_8055FBFC;
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
      int width = ((WidgetDispatch*)self)->Slot78();
      float ratio = ((float) width) / ((float) total.value);
      int start = f->state->start;
      int edge = self->bottom;
      ScrollLocalWordObservedNew originY(self->top);
      top = originY.value + 1;
      bottom = edge - 1;
      int low = self->left + ((int) (ratio * ((float) start)));
      ScrollEdgeWordObservedNew high(self->left + ((int) (ratio * ((float) (start + f->state->page)))));
      left = low + 5;
      right = high.value - 5;
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
          fn_802E30C8(lbl_8033D9F8);
          fn_802C3424(left, top, right, bottom, lbl_8055FBDC);
          int dark = lbl_8055FBE0;
          fn_802C3424(left, bottom - 1, right, bottom, dark);
          fn_802C3424(right - 1, top, right, bottom, dark);
          int light = lbl_8055FBFC;
          fn_802C3424(left, top, right, top + 1, light);
          fn_802C3424(left, top, left + 1, bottom, light);
        }
      }
    }
  }
}
void fn_80221E7C(WidgetFragment* self){WidgetFragment* parent=(WidgetFragment*)self->kind;if(parent)WidgetMarkDirty(parent);}
void fn_80221EA0(WidgetFragment* self,int key,void* value,int size){fn_80219440((WidgetFragment*)self->kind,key,value,size);}
int fn_80221EA8(GroupSearchProxyNew* self){GroupSearchWordNew one=1;GroupSearchWordNew group=self->context->group;for(int index=0;index<32;index++){if(group==(one<<index))return index;}fn_802E317C(lbl_8033DA2C,group);return 0;}
void Finish__9MenuStateFi(void*,void* context){fn_802168BC(context);}
void fn_80221F9C(void) {}
void fn_80222120(WidgetFragment* self,int x,int y){((PrefixFlag248*)self)->value=1;((RowInputDispatch*)self)->Slot4c(x,y);WidgetMarkDirty(self);}
void fn_80222178(WidgetFragment* self,int x,int y){((RowInputDispatch*)self)->Slot4c(x,y);MenuSelectionPrefix* f=(MenuSelectionPrefix*)self;f->active=0;if(f->row>=0&&f->column>=0){MenuRecord* record=fn_8022D97C(f->rows[f->row].handle,f->column);if(!record->callback||record->callback(record->context))fn_802168BC(record->context);}f->row=-1;f->column=-1;WidgetMarkDirty(self);}
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
 fn_80218F34(self,x,y,x+dx,y+dy,kind);self->vtable=lbl_8033F90C;
 OwnedWidgetPrefix* fields=(OwnedWidgetPrefix*)self;fields->x=x;fields->y=y;fields->kind=kind;fields->child=child;
 ((WidgetDispatch*)self)->Slot84();return self;
}
WidgetFragment* fn_802223C4(WidgetFragment* self,int destroy){
 if(self){OwnedWidget* child=((OwnedWidgetPrefix*)self)->child;self->vtable=lbl_8033F90C;delete child;fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;
}
void fn_80222450(WidgetFragment* self){WidgetMarkDirty(self);}
void fn_80222468(WidgetFragment* self,int x,int y){fn_802191F0(self,x,y);OwnedWidgetPrefix* fields=(OwnedWidgetPrefix*)self;fields->x=x;fields->y=y;}
WidgetFragment* fn_802224B4(WidgetFragment* self,void* context,int x,int y,int dx,int dy,OwnedWidget* child,int(*callback)(void*),unsigned char a,unsigned char b){
 fn_80222340(self,x,y,0,dx,dy,child);self->vtable=lbl_8033F880;
 CallbackPrefix* fields=(CallbackPrefix*)self;fields->context=context;fields->callback=callback;fields->flag256=a;fields->flag257=b;fields->unknown255=0;fields->active=0;return self;
}
ResourcePrefix* fn_80222530(ResourcePrefix* self,const char* path,const char* text){
 self->vtable=lbl_8033F9E0;self->resource=fn_80232578(path);self->text=text;return self;
}
ResourcePrefix* fn_80222584(ResourcePrefix* self,int destroy){
 if(self){self->vtable=lbl_8033F9E0;fn_802328D4(self->resource);if(destroy>0)__dl__FPv(self);}return self;
}
void fn_802225E8(ResourcePrefix* self,int x,int y){
 float a,b;fn_80230C30(&a,&b,x,y);float zero=(0.0f);fn_802336AC(self->resource,-1,a,b,zero,9,lbl_8055B8A4,self->text);
}
void fn_8022264C(ResourcePrefix* self,int first,int second,int* left,int* top,int* right,int* bottom){
 *left=first;*top=second;*right=first+(int)fn_8023293C(self->resource,self->text,(1.0f));*bottom=first+fn_80232B70(self->resource);
}
ImagePrefix* fn_802226D4(ImagePrefix* self,const char* path){self->vtable=lbl_8033F9BC;self->image=fn_8023512C(path);return self;}
ImagePrefix* fn_80222718(ImagePrefix* self,int destroy){
 if(self){void* image=self->image;self->vtable=lbl_8033F9BC;if(image)fn_80235380(image);if(destroy>0)__dl__FPv(self);}return self;
}
void fn_80222788(ImagePrefix* self,int x,int y,int unused,int frame){
 if(self->image){float zero=(0.0f);float a,b;fn_80230C30(&a,&b,x,y);fn_802353F4(self->image,frame,a,b,zero);}
}
void fn_80222800(ImagePrefix* self,int first,int second,int* left,int* top,int* right,int* bottom){
 *left=first;*top=second;
 if(!self->image){*right=first+10;*bottom=second+10;}
 else{*right=first+(int)fn_80235EF0(self->image,0);*bottom=first+(int)fn_80235F64(self->image,0);}
}
AnimationPrefix* fn_802228B0(AnimationPrefix* self,const char* path,int width,int height){
 self->vtable=lbl_8033F998;self->handle=fn_802C5130(path);
 if(width>0&&height>0){self->width=(float)width;self->height=(float)height;}
 else{self->width=(float)fn_802C51C0(self->handle);self->height=(float)fn_802C5220(self->handle);}
 return self;
}
AnimationPrefix* fn_80222998(AnimationPrefix* self,int destroy){
 if(self){self->vtable=lbl_8033F998;fn_802C519C(self->handle);if(destroy>0)__dl__FPv(self);}return self;
}
int fn_802229FC(AnimationPrefix* self){int result=fn_802C51AC(self->handle);if(result)result=fn_802C51AC(self->handle);return result;}
void fn_80222A3C(AnimationPrefix* self,int x,int y,int unused,u32 frame){
 if(fn_802C51AC(self->handle)){
  float zero=(0.0f);float a=zero,b=zero;fn_80230C30(&a,&b,x,y);
  if(frame!=0xFFFFFFFFu){int selected=(int)frame%fn_802C51AC(self->handle);fn_802C5320(self->handle,selected,a,b,zero,self->width,self->height);}
 }
}
void fn_80222AFC(AnimationPrefix* self,int first,int second,int* left,int* top,int* right,int* bottom){
 *left=first;*top=second;*right=*left+(int)self->width;*bottom=*top+(int)self->height;
}
int fn_80222B48(AnimationPrefix* self,int x,int y,int testX,int testY){
 int bottom,right,top,left;
 ((AnimationBoundsDispatch*)self)->Bounds(x,y,&left,&top,&right,&bottom);
 return testX>=left&&testX<right&&testY>=top&&testY<bottom;
}
int fn_80222BD4(AnimationPrefix* self){return fn_802C51AC(self->handle);}
WidgetFragment* fn_80222BDC(WidgetFragment* self,WidgetDesc* desc,void* context){fn_80218F80(self,desc);UnsignedAssetPrefix* f=(UnsignedAssetPrefix*)self;f->context=context;self->vtable=lbl_8033E560;f->resource=fn_80232578(desc->name);if(!f->resource)fn_802E317C(lbl_8033DA60,desc->name);f->mode=desc->fielda0;f->scale=(float)(u32)desc->field9c;f->elapsed=(0.2094f);return self;}
WidgetFragment* fn_80222C8C(WidgetFragment* self,int destroy){if(self){self->vtable=lbl_8033E560;fn_802328D4(self->y);fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;}
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
    count = self->list->count;
    base = -self->rate * fn_8023659C();
    if (count > 0) {
        total = (0.0f);
        for (i1 = 0; i1 < count; i1++) {
            total += fn_8023293C(self->font, fn_8022D3F8(self->list, i1, 0), (1.0f));
        }
        spacing = (((6.2831855f) * self->radius) - total) / (float)count;
        scale = lbl_80563528 / (total + (spacing * (float)count));
        fn_80230C30(&x, &y, self->left, self->top);
        src = (const CircularPoseTypedNew*)fn_8022EDC4();
        dst = (CircularPoseTypedNew*)fn_8022EDE4(src->alpha, src);
        pos = (0.0f);
        for (i2 = 0; i2 < count; i2++) {
            text = fn_8022D3F8(self->list, i2, 0);
            for (j = 0; j < (int)strlen(text); j++) {
                sprintf(one, lbl_8055B8A8, (int)(signed char)text[j]);
                dst->basis = src->basis;
                dst->translation = src->translation;
                RotationVec input = {x, y, 0.0f};
                ChoiceApplyOffsetNew((ChoicePoseNew*)dst, &input);
                float angleProduct = scale * pos; RotationVec rotation = {0.0f, 0.0f, angleProduct + base};
                fn_80015A20(&m, &rotation);
                Mat3Mul__FP4Mat3PC4Mat3PC4Mat3(&dst->basis, &m, &dst->basis);
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
WidgetFragment* fn_802230A4(WidgetFragment* self,int x,int y,void* list,int* selected) { return self; } // Unimplemented constructor; ABI-only placeholder.
WidgetFragment* fn_80223208(WidgetFragment* self,int destroy){if(self){self->vtable=lbl_8033E4DC;SelectResourcePrefix* f=(SelectResourcePrefix*)self;fn_802328D4(f->resource);f->resource=0;fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;}
void fn_80223280(WidgetFragment* self){SelectionDrawFieldsNew* f=(SelectionDrawFieldsNew*)self;float y,x,selectedX,half,shadow;x=f->scrollX;y=(float)self->top;y=(240.0f)-y;selectedX=f->scrollX;shadow=(2.0f);half=(0.5f);
int i;for(i=0;i<f->count;i++){if(i==*f->selected){selectedX=x;{
 float w=f->widths[i];
 x+=f->spacing+w*half;
}if(i+1<f->count){
 float w=f->widths[i+1];
 x+=w*half;
}}else{fn_80232BA4(f->labels[i],f->resource,shadow+x,y-shadow,0xff000000u,12,f->scale,(0.0f),(1.0f));u32 color=fn_802236C8(self,i);fn_80232BA4(f->labels[i],f->resource,x,y,color,12,f->scale,(0.0f),(1.0f));{
 float w=f->widths[i];
 x+=f->spacing+w*half;
}if(i+1<f->count){
 float w=f->widths[i+1];
 x+=w*half;
}}}
i=*f->selected;int selected=i;
// MATCHING-AID: retain the observed screen int-to-float conversions; both local values are unused in this recovered body, and their original enclosing interface remains unproved.
float screenWidth=(float)lbl_8055FB9C;float screenHeight=(float)lbl_8055FBA0;(void)screenWidth;(void)screenHeight;float left=selectedX-f->widths[selected]*GaugeHalfReferenceObservedNew()-f->spacing/2.0f;left=lbl_80563534+left;int textHeight=fn_80232B70(f->resource);float height=(2.0f)+f->scale*(float)textHeight;float width=f->spacing+f->widths[selected];
fn_80230924(0x96646464u,left,y-(2.0f),left+width,(y-height)-lbl_80563534,(0.0f));
fn_80230924(0x96282828u,left,y-(2.0f),(2.0f)+left,(y-height)-lbl_80563534,(0.0f));
fn_80230924(0x96282828u,left,y-(2.0f),left+width,y-lbl_80563534,(0.0f));
fn_80230924(0x96aaaaaau,left,(y-height)-(2.0f),left+width,(y-height)-lbl_80563534,(0.0f));
fn_80230924(0x96aaaaaau,(left+width)-(2.0f),y-(2.0f),left+width,(y-height)-lbl_80563534,(0.0f));
fn_80232BA4(f->labels[selected],f->resource,lbl_80563534+selectedX,y-lbl_80563534,0xff000000u,12,f->scale,(0.0f),(1.0f));u32 color=fn_802236C8(self,selected);fn_80232BA4(f->labels[selected],f->resource,(2.0f)+selectedX,y-(2.0f),color,12,f->scale,(0.0f),(1.0f));}
void fn_802235B4(WidgetFragment* self){SelectBooleanNavigationRelativeNew* f=(SelectBooleanNavigationRelativeNew*)self;int* value=f->selected;int selected=*value;int bounds[2]={selected+1,f->count};int step=1;for(;bounds[0]<bounds[1];bounds[0]++,step++){if(((SelectBooleanNavigationRelativeNew*)((unsigned int)f+((unsigned int)selected+(unsigned int)step)))->available[0]){*value+=step;return fn_8022375C(self);}}for(int step=selected;step>0;step--){if(((SelectBooleanNavigationRelativeNew*)((char*)f+(unsigned int)(selected-step)))->available[0]){*value-=step;return fn_8022375C(self);}}}
void fn_80223644(WidgetFragment* self){NavigationPreviousNew* f=(NavigationPreviousNew*)self;int* value=f->selected;int selected=*value;for(int step=1;selected-step>=0;step++){if(((NavigationPreviousNew*)((char*)f+((unsigned int)selected-(unsigned int)step)))->available[0]){*value-=step;fn_8022375C(self);return;}}for(int step=f->count-selected-1;step>0;step--){if(((NavigationPreviousNew*)(((unsigned int)f+(unsigned int)selected)+(unsigned int)step))->available[0]){*value+=step;fn_8022375C(self);return;}}}
u32 fn_802236C8(WidgetFragment* self,int index){SelectionColorPrefix* f=(SelectionColorPrefix*)self;if(f->locked)return 0xe6969696u;u32 color=0xff9f9f9fu;if(f->flags[index])color=0xffffffffu;if(index==*f->selected&&(1.0f)==f->pulse)color=0xffffd03fu;return color;}
void fn_80223728(WidgetFragment* self){fn_802192AC(self);((SelectionPrefix*)self)->pulse=(1.0f);}
void fn_8022375C(WidgetFragment*) {}
int fn_80223760(WidgetFragment* self,int key){if(((SelectionPrefix*)self)->locked)return 0;switch(key){case 0x125:((void(*)(WidgetFragment*))fn_80223644)(self);return 1;case 0x127:((void(*)(WidgetFragment*))fn_802235B4)(self);return 1;}return 0;}
void fn_802237D0(WidgetFragment* self){((SelectionPrefix*)self)->locked=1;}
void fn_802237DC(WidgetFragment* self){((SelectionPrefix*)self)->locked=0;}
int fn_802237E8(WidgetFragment* self){return ((SelectionPrefix*)self)->count;}
int fn_802237F0(WidgetFragment* self,int index){return ((SelectionPrefix*)self)->icons[index];}
WidgetFragment* fn_80223800(WidgetFragment* self,int x,int y,void* model,int* watched){fn_802230A4(self,x,y,model,watched);((SelectWatchedPrefix*)self)->watched=watched;self->vtable=lbl_8033E458;if(watched)fn_80219440(self,0,watched,4);fn_80219440(self,1,fn_80216708(),1);return self;}
void fn_80223880(WidgetFragment* self){((WidgetDispatch*)self)->Slot74(2,0);fn_802237DC(self);fn_8021818C(self->window);}
void fn_802238D0(WidgetFragment* self,int mode,void*){SelectWatchedPrefix* f=(SelectWatchedPrefix*)self;void* window;for(int i=0;i<fn_802237E8(self);i++){if(i==*f->watched){float duration=mode==2?(0.0f):(0.3f);window=self->window;int item=fn_802237F0(self,i);fn_80218968(window,item,duration);}else{float duration=mode==2?(0.0f):(0.3f);window=self->window;int item=fn_802237F0(self,i);fn_8021889C(window,item,duration);}}WidgetMarkDirty(self);}
int fn_802239C8(WidgetFragment* self,int key){if(*(unsigned char*)fn_80216708()){switch(key){case 0x127:((void(*)(WidgetFragment*))fn_802235B4)(self);return 1;case 0x125:((void(*)(WidgetFragment*))fn_80223644)(self);return 1;}}return fn_802192A4(self,key);}
void fn_80223A58(WidgetFragment* self){if(*(unsigned char*)fn_80216708())fn_802237DC(self);else fn_802237D0(self);((void(*)(WidgetFragment*))fn_80223280)(self);}
int fn_80223AAC(WidgetFragment*){return 1;}
WidgetFragment* fn_80223AB4(WidgetFragment* self,WidgetDesc* desc,void* context){fn_80218F80(self,desc);self->vtable=lbl_8033E3D4;ImageDescriptorPrefix* f=(ImageDescriptorPrefix*)self;fn_80230C30(&f->x,&f->y,desc->x,desc->y);f->value=desc->field9c;const char* name=(const char*)desc+0x40;if(name)f->image=fn_802C5130(name);else f->image=0;f->width=(float)desc->dx;f->height=(float)desc->dx;f->alpha=(1.0f);f->asset=fn_8022EE9C();f->context=context;return self;}
WidgetFragment* fn_80223B94(WidgetFragment* self,int destroy){if(self){self->vtable=lbl_8033E3D4;TwoCleanupPrefix* f=(TwoCleanupPrefix*)self;if(f->image)fn_802C519C(f->image);f->image=0;fn_8022EE98(f->asset);fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;}
void fn_80223C20(WidgetFragment* self){ImageAlphaPrefix* f=(ImageAlphaPrefix*)self;f->asset->alpha=(1.0f)-f->blend;float zero=(0.0f);if(f->asset->alpha>zero&&f->image){char scope[64];const void* context=fn_8022EDC4();fn_8022EEF8(scope,f->asset,context);fn_8022ED00(scope);fn_802C5320(f->image,0,f->x,f->y,zero,f->width,f->height);fn_8022EDB8();}}
void fn_80223CC4(WidgetFragment* self){TweenPrefix* f=(TweenPrefix*)self;if(f->selected==f->state[3])f->blend=(0.0f);else f->blend=(1.0f);fn_802192AC(self);}
WidgetFragment* fn_80223CF0(WidgetFragment* self,WidgetDesc* desc,const char* name){
 fn_80218F80(self,desc);LargeImage355* f=(LargeImage355*)self;self->vtable=lbl_8033E350;f->name=0;f->flag=0;f->resource=-2;f->image=0;
 if(!fn_802C9A58()&&((WidgetDispatch*)self)->Slot78()>600&&self->bottom-self->top>460){
 int left=lbl_8055CFF0/2+Round355(lbl_80563538*(float)(self->left-lbl_8055CFF0/2));
 int top=lbl_8055CFF4/2+Round355((float)(self->top-lbl_8055CFF4/2));
 int width=Round355(lbl_80563538*(float)((WidgetDispatch*)self)->Slot78());
 int height=Round355((float)(self->bottom-self->top));
 self->left=left;self->top=top;self->right=left+width;self->bottom=top+height;
 }
 fn_80230C30(&f->x,&f->y,self->left,self->top);f->width=(float)((WidgetDispatch*)self)->Slot78();f->height=(float)(self->bottom-self->top);
 if(name)f->name=name;else f->name=desc->name;bool flag=desc->fielda4!=0;f->frame=0;f->flag=flag;return self;
}
WidgetFragment* fn_80223FAC(WidgetFragment* self,int destroy){if(self){self->vtable=lbl_8033E350;RenderPrefix* f=(RenderPrefix*)self;if(f->resource)fn_802ECFF4(f->resource);fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;}
void fn_80224028(WidgetFragment* self){fn_802192AC(self);RenderRequestPrefix* f=(RenderRequestPrefix*)self;if(!f->resource){if(f->name){f->resource=fn_802ECF08(f->name,!f->notify);if(!f->resource&&f->notify){if(!f->callback||f->callback(f->context))fn_802168BC(f->context);}}}else{if(f->notify&&fn_802ED040(f->resource)){if(!f->callback||f->callback(f->context))fn_802168BC(f->context);}if(f->name&&!fn_802ECF90(f->resource,f->name)){fn_802ECFF4(f->resource);fn_802E30C8(lbl_8033DA90,f->name);f->resource=fn_802ECF08(f->name,!f->notify);}}if(f->resource)fn_802ECFEC(f->resource);}
void fn_80224160(WidgetFragment* self){fn_80219354(self);RenderPrefix* f=(RenderPrefix*)self;if(f->resource)fn_802ECFEC(f->resource);}
void fn_8022419C(WidgetFragment* self){RenderPrefix* f=(RenderPrefix*)self;if(f->resource)fn_802ED038(f->resource,f->x,f->y,f->w,f->h);}
WidgetFragment* fn_802241C8(WidgetFragment* self,WidgetDesc* desc){fn_80218F80(self,desc);self->vtable=lbl_8033E2CC;RenderPrefix* f=(RenderPrefix*)self;fn_80230C30(&f->x,&f->y,desc->x,desc->y);f->w=(float)desc->dx;f->h=(float)desc->dy;char path[256];sprintf(path,lbl_8033DAAC,desc->name);f->resource=fn_802C5130(path);if(!f->resource)fn_802E30C8(lbl_8033DABC,path);return self;}
WidgetFragment* fn_802242A4(WidgetFragment* self,int destroy){if(self){self->vtable=lbl_8033E2CC;RenderPrefix* f=(RenderPrefix*)self;if(f->resource)fn_802C519C(f->resource);f->resource=0;fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;}
void fn_80224328(WidgetFragment* self){RenderPrefix* f=(RenderPrefix*)self;if(f->resource)fn_802C5320(f->resource,0,f->x,f->y,(0.0f),f->w,f->h);}
WidgetFragment* fn_8022435C(WidgetFragment* self,void* context,WidgetDesc* desc,int(*callback)(void*),int* frame){fn_80218F80(self,desc);self->vtable=lbl_8033E248;ToggleImagePrefix* f=(ToggleImagePrefix*)self;fn_80230C30(&f->x,&f->y,desc->x,desc->y);f->width=(float)desc->dx;f->height=(float)desc->dy;f->active=0;f->pressed=0;f->mode=(u32)desc->field9c>>31;float alpha;if(f->mode==0)alpha=(0.0f);else alpha=(0.1f);f->blend=alpha;f->alpha=alpha;f->token=fn_80228F40(self->left,self->top);f->focus=0;f->callback=callback;f->context=context;f->speed=InlineWidgetAbsolute(desc->field9c);f->asset=(ImageAlphaAsset*)fn_8022EE9C();f->scope=fn_8022EE9C();f->elapsed=(0.0f);f->frame=frame?*frame:0;char path[256];strcpy(path,(const char*)desc+0x40);f->image=fn_802C5130(path);if(!f->image)fn_802E30C8(lbl_8033DABC,path);return self;}
WidgetFragment* fn_802244C8(WidgetFragment* self,int destroy){if(self){self->vtable=lbl_8033E248;ThreeCleanupPrefix* f=(ThreeCleanupPrefix*)self;if(f->image)fn_802C519C(f->image);f->image=0;fn_8022EE98(f->assetSecond);fn_8022EE98(f->assetFirst);fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;}
void fn_8022455C(WidgetFragment* self){TimedFocusPrefix* f=(TimedFocusPrefix*)self;f->focus=fn_80217BD8(self->window,f->token,self);fn_80219440(self,0,f->focus,4);((WidgetDispatch*)self)->Slot74(0,0);}
void fn_802245C8(WidgetFragment* self){ToggleImagePrefix* f=(ToggleImagePrefix*)self;if(fn_8023659C()<f->elapsed)f->elapsed=fn_8023659C();switch(f->mode){case 0:if(f->active){float wave=(0.5f)+(0.5f)*(float)sin((float)(f->speed+1)*(fn_8023659C()-f->elapsed));f->alpha=InlineWidgetMinimum((float)(f->speed+1)*(fn_8023659C()-f->elapsed),wave);}else if(f->pressed){if(f->alpha<(0.005f)){f->pressed=0;f->alpha=(0.0f);}else f->alpha=InlineWidgetMaximum(f->blend-(float)(f->speed+1)*(fn_8023659C()-f->elapsed),(0.0f));}break;case 1:if(f->active){float wave=lbl_8056353C+((0.1f)+lbl_8056353C*(float)sin((float)-f->speed*(fn_8023659C()-f->elapsed)));f->alpha=InlineWidgetMinimum((0.1f)+(float)f->speed*(fn_8023659C()-f->elapsed),wave);}else if(f->pressed){if(f->alpha<lbl_80563540){f->pressed=0;f->alpha=(0.1f);}else f->alpha=InlineWidgetMaximum(f->blend-(float)f->speed*(fn_8023659C()-f->elapsed),(0.1f));}break;}fn_802192AC(self);}
void fn_80224864(WidgetFragment* self,int,void*){TimedFocusPrefix* f=(TimedFocusPrefix*)self;if(!f->active&&f->token==*f->focus){f->inactive=0;f->active=1;}else if(f->active&&f->token!=*f->focus){f->active=0;f->inactive=1;}f->stamp=fn_8023659C();f->previous=f->value;}
void fn_802248FC(WidgetFragment* self){ToggleImagePrefix* f=(ToggleImagePrefix*)self;if(f->image){if(!self->enabled)f->asset->alpha=(0.5f);else if(f->active)f->asset->alpha=f->alpha;else f->asset->alpha=f->mode==1?(1.0f):(0.0f);fn_8022EEF8(f->scope,(void*)fn_8022EDC4(),f->asset);fn_8022ED00(f->scope);int frame,height;height=self->bottom-self->top;frame=f->frame%fn_802C51AC(f->image);fn_802C5320(f->image,frame,f->x,f->y,(0.0f),(float)((WidgetDispatch*)self)->Slot78(),(float)height);fn_8022EDB8();}}
int fn_80224A38(WidgetFragment* self,int key){ToggleImagePrefix* f=(ToggleImagePrefix*)self;if(!f->focus)return 0;if(f->token!=*f->focus)return 0;switch(key){case 13:if(f->callback&&!f->callback(f->context))return 1;fn_802168BC(f->context);return 1;case 0x126:fn_80217D84(self->window);return 1;case 0x128:fn_80217C4C(self->window);return 1;}return 0;}
int fn_80224B0C(WidgetFragment* self,int key){int* focus=(int*)self->unknown258;if(focus&&(int)self->label!=*focus)return 0;return key==13||(unsigned int)key==0x126||(unsigned int)key==0x128;}
WidgetFragment* fn_80224B54(WidgetFragment* self,int context,WidgetDesc* desc){fn_80218F80(self,desc);self->vtable=lbl_8033E1C4;ResourceTextPrefix* f=(ResourceTextPrefix*)self;strcpy(f->text,lbl_8055B88C);f->localized=0;switch(desc->fielda0){case 0:f->localized=1;strcpy(f->text,lbl_8055B88C);break;case 1:{int second=fn_80129D90(1,1,8);int first=fn_80129D90(1,0x42,0x45);sprintf(f->text,lbl_8033DAD8,first,second);break;}}if(context)f->context=context;else f->context=fn_80131234((const char*)desc+0x20);f->resource=fn_80232578((const char*)desc+0x40);f->style=desc->style90;f->flags=desc->flags;f->scale=desc->scale/(float)fn_80232B70(f->resource);f->owned=0;f->asset=0;float x,y,z;memcpy(&x,(const char*)desc+0xa4,4);memcpy(&y,(const char*)desc+0xa8,4);memcpy(&z,(const char*)desc+0xac,4);if(!(ObservedAbsDecision(x)<=lbl_80558DC4[0]&&ObservedAbsDecision(y)<=lbl_80558DC4[0]&&ObservedAbsDecision(z)<=lbl_80558DC4[0]))fn_80224DDC(self,x,y,z);return self;}
WidgetFragment* fn_80224D40(WidgetFragment* self,int destroy){if(self){self->vtable=lbl_8033E1C4;AssetFormatPrefix* f=(AssetFormatPrefix*)self;if(f->owned)fn_8022EE98(f->asset);fn_802328D4(f->resource);f->resource=0;f->first=0;strcpy(f->text,lbl_8055B88C);fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;}
void fn_80224DDC(WidgetFragment* self,float x,float y,float z){ResourceTextPrefix* f=(ResourceTextPrefix*)self;f->owned=1;f->asset=fn_8022EE9C();RotationVec rotation={RotationScale360((0.017453292f),x),RotationScale360((0.017453292f),y),RotationScale360((0.017453292f),z)};RotationMatrix matrix;RotationMatrix* target=(RotationMatrix*)f->asset;fn_80015A20(&matrix,&rotation);Mat3Mul__FP4Mat3PC4Mat3PC4Mat3(target,&matrix,target);}
void fn_80224E74(void) {}
WidgetFragment* fn_8022553C(WidgetFragment* self,RollingListData* list,WidgetDesc* desc){fn_80218F80(self,desc);RollingListFullPrefix* f=(RollingListFullPrefix*)self;f->list=list;self->vtable=lbl_8033E140;f->resource=fn_80232578(lbl_8033DA84);f->count=InlineIntegerMinimum(InlineIntegerMaximum(desc->field9c,1),3);f->x=(float)lbl_8055FB9C/lbl_80563544;f->x+=(float)self->left;f->y=(float)lbl_8055FBA0/2.0f-(float)self->top;f->lineHeight=(5.0f)+(float)fn_80232B70(f->resource);f->period=f->lineHeight*(float)f->list->count;f->upper=f->y+(0.5f)*f->period;f->lower=f->y-(0.5f)*f->period;int row=0;f->rows=(RollingRow*)__nwa__FUlPCc(f->list->count*12,lbl_8033DAEC);for(int i=f->list->index;i<f->list->count;i++){f->rows[i].current=f->y-f->lineHeight*(float)row;if(f->rows[i].current<f->lower)f->rows[i].current+=f->period;float current=f->rows[i].current;f->rows[i].target=current;f->rows[i].previous=current;row++;}for(int i=0;i<f->list->index;i++){f->rows[i].current=f->y-f->lineHeight*(float)row;if(f->rows[i].current<f->lower)f->rows[i].current+=f->period;float current=f->rows[i].current;f->rows[i].target=current;f->rows[i].previous=current;row++;}f->direction=0;f->stamp=fn_8023659C();f->elapsed=(0.0f);f->nextStamp=fn_8023659C();f->focus=0;f->token=fn_80228F40(self->left,self->top);return self;}
WidgetFragment* fn_80225828(WidgetFragment* self,int destroy){if(self){self->vtable=lbl_8033E140;RollingRowsPrefix* f=(RollingRowsPrefix*)self;if(f->rows)__dla__FPv(f->rows);f->rows=0;if(self->y)fn_802328D4(self->y);self->y=0;self->x=0;fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;}
void fn_802258C8(WidgetFragment* self){Bound27c* f=(Bound27c*)self;f->focus=fn_80217BD8(self->window,f->token,self);fn_80219440(self,1,f->focus,4);}
void fn_80225918(WidgetFragment* self){fn_802192AC(self);}
void fn_8022591C(WidgetFragment* self){RollingInterpolationPrefix* f=(RollingInterpolationPrefix*)self;if(f->direction){float now=fn_8023659C();if(f->stamp>now)f->stamp=now-lbl_80563548;float elapsed=now-f->stamp;if(elapsed>lbl_80563548){for(int i=0;i<f->list->count;i++){float target=f->rows[i].target;f->rows[i].current=target;f->rows[i].previous=target;}f->direction=0;}else{for(int i=0;i<f->list->count;i++){switch(f->direction){case 0:break;case 2:if(f->rows[i].target>f->rows[i].previous)f->rows[i].current=InlineWidgetInterpolate(f->rows[i].previous,f->rows[i].target,elapsed,lbl_80563548);else if(f->rows[i].target<f->rows[i].previous){if(f->rows[i].current>f->rows[i].target)f->rows[i].current=InlineWidgetInterpolate(f->rows[i].previous,f->upper+(f->rows[i].target-f->lower),elapsed,lbl_80563548);else if(f->rows[i].current<f->rows[i].target)f->rows[i].current=InlineWidgetInterpolate(f->lower-(f->upper-f->rows[i].previous),f->rows[i].target,elapsed,lbl_80563548);}break;case 1:if(f->rows[i].target<f->rows[i].previous)f->rows[i].current=InlineWidgetInterpolate(f->rows[i].previous,f->rows[i].target,elapsed,lbl_80563548);else if(f->rows[i].target>f->rows[i].previous){if(f->rows[i].current<f->rows[i].target)f->rows[i].current=InlineWidgetInterpolate(f->rows[i].previous,f->lower-(f->upper-f->rows[i].target),elapsed,lbl_80563548);else if(f->rows[i].current>f->rows[i].target)f->rows[i].current=InlineWidgetInterpolate(f->upper+(f->rows[i].previous-f->lower),f->rows[i].target,elapsed,lbl_80563548);}break;}if(f->rows[i].current>f->upper)f->rows[i].current-=f->period;else if(f->rows[i].current<f->lower)f->rows[i].current+=f->period;}}}}
void fn_80225B6C(void) {}
int fn_802263CC(WidgetFragment* self,int key){RollingActionPrefix* f=(RollingActionPrefix*)self;if(f->focus&&f->token!=*f->focus)return 0;switch(key){case 0x127:if(f->list->vertical){fn_80217C4C(self->window);return 1;}break;case 0x125:if(f->list->vertical){fn_80217D84(self->window);return 1;}break;case 0x128:fn_8022DBFC(f->list);fn_80226570(self,-1);f->direction=2;f->stamp=fn_8023659C();return 1;case 0x126:fn_8022DC20(f->list);fn_80226570(self,1);f->direction=1;f->stamp=fn_8023659C();return 1;case 13:{void* context;int(*callback)(void*)=f->list->callback;context=fn_8022DBE8(f->list);if(!callback||callback(context))fn_802168BC(context);return 1;}}return 0;}
int fn_80226528(WidgetFragment* self,int key){Bound27c* f=(Bound27c*)self;if(f->focus&&f->token!=*f->focus)return 0;return key==13||(unsigned int)key==0x126||(unsigned int)key==0x128;}
void fn_80226570(WidgetFragment* self,int delta){RollingRowsPrefix* f=(RollingRowsPrefix*)self;for(int i=0;i<f->list->count;i++){f->rows[i].previous=f->rows[i].current;f->rows[i].target-=f->speed*(float)delta;if(f->rows[i].target>f->upper)f->rows[i].target-=f->period;if(f->rows[i].target<f->lower)f->rows[i].target+=f->period;}}
WidgetFragment* fn_80226630(WidgetFragment* self,void* context,int x,int y,int dx,int dy,OwnedWidget* child,int(*callback)(void*)){fn_802224B4(self,context,x,y,dx,dy,child,callback,1,0);self->vtable=lbl_8033F7F4;Bound264* f=(Bound264*)self;f->token=fn_80228F40(x,y);f->focus=0;return self;}
WidgetFragment* fn_802266B0(WidgetFragment* self,int destroy){if(self){fn_802223C4(self,0);if(destroy>0)__dl__FPv(self);}return self;}
void fn_80226708(WidgetFragment* self){Bound264* f=(Bound264*)self;f->focus=fn_80217BD8(self->window,f->token,self);fn_80219440(self,1,f->focus,4);}
void fn_80226758(WidgetFragment* self,int,int){Bound264* f=(Bound264*)self;*f->focus=f->token;}
void fn_80226768(WidgetFragment* self,int,int){Bound264* f=(Bound264*)self;if(self->flag18){if(!f->callback||f->callback(f->context))fn_802168BC(f->context);}}
int fn_802267C4(WidgetFragment* self,int key){SelectionCallbackPrefix* f=(SelectionCallbackPrefix*)self;if(!f->focus)return 0;if(f->token!=*f->focus)return 0;switch(key){case 13:if(!f->callback||f->callback(f->context))fn_802168BC(f->context);return 1;case 0x126:fn_80217D84(self->window);fn_80230E90(0);return 1;case 0x128:fn_80217C4C(self->window);fn_80230E90(0);return 1;}return 0;}
int fn_802268A0(WidgetFragment* self,int key){Bound264* f=(Bound264*)self;if(f->focus&&f->token!=*f->focus)return 0;return key==13||(unsigned int)key==0x126||(unsigned int)key==0x128;}
int fn_802268E8(WidgetFragment* self){SelectionActionPrefix* f=(SelectionActionPrefix*)self;if(!f->focus)return 0;int selected=f->token==*f->focus;unsigned int state;if(self->flag18)state=(selected!=0)+2;else state=selected!=0;if(f->child->State()==3&&state==3)state=1;if(!self->enabled)state=4;return state;}
WidgetFragment* fn_802269A8(WidgetFragment* self,WidgetDesc* desc){fn_80218F80(self,desc);self->vtable=lbl_8033E0BC;CenteredImagePrefix* f=(CenteredImagePrefix*)self;f->image=fn_802C5130(desc->name);if(!f->image)fn_802E30C8(lbl_8033DAF8,desc->name);f->asset=(CenteredImageState*)fn_8022EE9C();fn_80230C30(&f->x,&f->y,self->left,self->top);f->height=(float)(self->bottom-self->top);f->width=(float)((WidgetDispatch*)self)->Slot78();f->asset->x=f->x+f->width/2.0f;f->asset->y=f->y-f->height/2.0f;f->value=desc->field9c;return self;}
WidgetFragment* fn_80226AD0(WidgetFragment* self,int destroy){if(self){self->vtable=lbl_8033E0BC;AssetImage244* f=(AssetImage244*)self;if(f->asset)fn_8022EE98(f->asset);f->asset=0;if(f->image)fn_802C519C(f->image);f->image=0;fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;}
void fn_80226B6C(WidgetFragment* self){char scope[64];CenteredImagePrefix* f=(CenteredImagePrefix*)self;float time=fn_8023659C();ObservedTransform* m=(ObservedTransform*)f->asset;float zero=(0.0f),one=(1.0f);m->m[0]=one;m->m[1]=zero;m->m[2]=zero;m->m[3]=zero;m->m[4]=one;m->m[5]=zero;m->m[6]=zero;m->m[7]=zero;m->m[8]=one;m->m[9]=zero;m->m[10]=zero;m->m[11]=zero;SpinVector359 rotation(zero,zero,SpinAngle359(f->value,time/(30.0f)));RotationMatrix matrix;void* target=f->asset;fn_80015A20(&matrix,(RotationVec*)&rotation);fn_80015B20(target,&matrix,target);f->asset->x=f->x+f->width/2.0f;f->asset->y=f->y-f->height/2.0f;if(f->image){fn_8022EEF8(scope,f->asset,fn_8022EDC4());fn_8022ED00(scope);fn_802C5320(f->image,0,-f->width/2.0f,f->height/2.0f,(0.0f),f->width,f->height);fn_8022EDB8();}}
WidgetFragment* fn_80226CBC(WidgetFragment* self,WidgetDesc* desc){
 const char* strings=lbl_8033D878;fn_80218F80(self,desc);self->vtable=lbl_8033E038;EffectImage351* f=(EffectImage351*)self;
 f->height=(float)(self->bottom-self->top);f->width=(float)((WidgetDispatch*)self)->Slot78();
 f->mode=desc->field9c;if(f->mode<0)goto invalidMode;if(f->mode<=15)goto validMode;invalidMode:fn_802E30C8(strings+0x2a4,f->mode);validMode:;
 if(f->mode==0)goto oldMode;if(f->mode!=1)goto newMode;oldMode:
 if(!lbl_8055EFBC){lbl_8055EFBC=1;fn_802E30C8(lbl_8055B8A4,strings+0x2d0);}
 if(f->mode==0)f->mode=4;else f->mode=7;
 newMode:const char* name=strings+0x328;f->image=fn_8023512C(name);if(!f->image)fn_802E30C8(strings+0x338,name);
 if(f->image&&fn_80235FD8(f->image)!=15){fn_802E30C8(strings+0x368,name,fn_80235FD8(f->image));fn_80235380(f->image);f->image=0;}
 f->owned=0;f->matrix=0;float x,y,z;memcpy(&x,(const char*)desc+0xa4,4);memcpy(&y,(const char*)desc+0xa8,4);memcpy(&z,(const char*)desc+0xac,4);
 if(!(ObservedAbsDecision(x)<=lbl_80558DC4[0]&&ObservedAbsDecision(y)<=lbl_80558DC4[0]&&ObservedAbsDecision(z)<=lbl_80558DC4[0]))((void(*)(WidgetFragment*,float,float,float))fn_80227044)(self,x,y,z);
 return self;
}
WidgetFragment* fn_80226F00(WidgetFragment* self,int destroy){if(self){self->vtable=lbl_8033E038;SceneResourcePrefix* f=(SceneResourcePrefix*)self;if(f->owned)fn_8022EE98(f->asset);if(f->model)fn_80235380(f->model);f->model=0;fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;}
void fn_80226F94(WidgetFragment* self){SceneResourcePrefix* f=(SceneResourcePrefix*)self;if(f->owned){char scope[64];const void* context=fn_8022EDC4();fn_8022EEF8(scope,f->asset,context);fn_8022ED00(scope);}fn_80230C30(&f->x,&f->y,self->left,self->top);if(f->model&&f->frame>=0&&f->frame<15)fn_802354A0(f->model,f->frame,0xffffffffu,f->x,f->y,(0.0f),f->width,f->height);if(f->owned)fn_8022EDB8();}
void fn_80227044(WidgetFragment* self,float x,float y,float z){AssetRotationPrefix* f=(AssetRotationPrefix*)self;f->owned=1;f->matrix=fn_8022EE9C();RotationVec rotation={RotationScale360((0.017453292f),x),RotationScale360((0.017453292f),y),RotationScale360((0.017453292f),z)};RotationMatrix matrix;RotationMatrix* target=f->matrix;fn_80015A20(&matrix,(RotationVec*)&rotation);Mat3Mul__FP4Mat3PC4Mat3PC4Mat3(target,&matrix,target);}
WidgetFragment* fn_802270DC(WidgetFragment* self,WidgetDesc* desc){
 fn_80218F80(self,desc);self->vtable=lbl_8033DFB4;TimedImage349* f=(TimedImage349*)self;
 fn_80230C30(&f->x,&f->y,self->left,self->top);f->height=(float)(self->bottom-self->top);f->width=(float)((WidgetDispatch*)self)->Slot78();
 const char* name=(const char*)desc+0x40;f->resource=-1;if(name)f->resource=fn_802CAC28(name,0);
 if(f->resource==-2)goto failure; if(f->resource!=-1)goto loaded; failure:fn_802E30C8(lbl_8033DC20);loaded:;
 f->rate=(float)(u32)desc->field9c/(10.0f);f->stamp=fn_8023659C();f->asset=fn_8022EE9C();f->increasing=1;f->first=(0.0f);f->second=(0.0f);
 if(*(int*)((char*)desc+0xb0))f->reverse=1;else f->reverse=0;return self;
}
WidgetFragment* fn_80227244(WidgetFragment* self,int destroy){if(self){self->vtable=lbl_8033DFB4;ResourceHandlePrefix* f=(ResourceHandlePrefix*)self;fn_8022EE98(f->asset);if(f->handle!=-2&&f->handle!=-1)fn_802CAC54(f->handle);f->handle=-1;fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;}
void fn_802272D4(WidgetFragment* source)
{
    TimedImageFields* image = (TimedImageFields*)source;
    RotationMatrix rotationMatrix;
    TimedImagePose *resetPose;
    TimedImagePose *firstPose;
    TimedImagePose *rotationPose;
    TimedImagePose *secondPose;
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
        resetPose->basis.m[0] = (1.0f);
        resetPose->basis.m[1] = (0.0f);
        resetPose->basis.m[2] = (0.0f);
        resetPose->basis.m[3] = (0.0f);
        resetPose->basis.m[4] = (1.0f);
        resetPose->basis.m[5] = (0.0f);
        resetPose->basis.m[6] = (0.0f);
        resetPose->basis.m[7] = (0.0f);
        resetPose->basis.m[8] = (1.0f);
        resetPose->translation.x = (0.0f);
        resetPose->translation.y = (0.0f);
        resetPose->translation.z = (0.0f);
        TimedImageInputVector transformInput(lbl_8055FB9C,lbl_8055FBA0);
        RotationVec translatedBeforeRotation,translatedAfterRotation;
        firstPose = image->pose;
        TransformTimedImageVector(firstPose,&transformInput,&translatedBeforeRotation);
        firstPose->translation.x += translatedBeforeRotation.x;
        firstPose->translation.y += translatedBeforeRotation.y;
        firstPose->translation.z += translatedBeforeRotation.z;
        if ((int) image->rotateBackward != 0) {
            image->rotationAngle -= (now - image->lastUpdateTime) / (30.0f);
        } else {
            image->rotationAngle += (now - image->lastUpdateTime) / (25.0f);
        }
        transformInput.x = (0.0f);
        transformInput.y = (0.0f);
        transformInput.z = image->rotationAngle;
        rotationPose = image->pose;
        fn_80015A20((RotationMatrix*)&rotationMatrix, (RotationVec*)&transformInput);
        Mat3Mul__FP4Mat3PC4Mat3PC4Mat3(&rotationPose->basis, &rotationMatrix, &rotationPose->basis);
        transformInput.SetZYX((0.0f),(float)lbl_8055FBA0/2.0f,(float)lbl_8055FB9C/2.0f);
        secondPose = image->pose;
        TransformTimedImageVector(secondPose,&transformInput,&translatedAfterRotation);
        secondPose->translation.x += translatedAfterRotation.x;
        secondPose->translation.y += translatedAfterRotation.y;
        secondPose->translation.z += translatedAfterRotation.z;
        image->pose->alpha = (0.3f) + (lbl_80563560 * image->blend);
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
void fn_802278F0(void* image,float x,float y,float width,float height){if(image){
 float offset=(0.0f);int frame=8;
 while(offset<width){float tile=fn_80235EF0(image,frame);if(tile-(1.0f)<width-offset)fn_802353F4(image,frame,x+offset,y,(0.0f));else{float h=fn_80235F64(image,frame);fn_802354A0(image,frame,0xffffffffu,x+offset,y,(0.0f),width-offset,h);}offset+=tile-(1.0f);frame++;if(frame>=10)frame=8;}
 offset=(0.0f);frame=10;float h=fn_80235F64(image,10);float bottom=h+(y-height);
 while(offset<width){float tile=fn_80235EF0(image,frame);if(tile-(1.0f)<width-offset)fn_802353F4(image,frame,x+offset,bottom,(0.0f));else fn_802354A0(image,frame,0xffffffffu,x+offset,bottom,(0.0f),width-offset,h);offset+=tile-(1.0f);frame++;if(frame>=12)frame=10;}
 float vertical=(0.0f);frame=4;
 while(vertical<height){float tile=fn_80235F64(image,frame);if(tile-(1.0f)<height-vertical)fn_802353F4(image,frame,x,y-vertical,(0.0f));else{float w=fn_80235EF0(image,frame);fn_802354A0(image,frame,0xffffffffu,x,y-vertical,(0.0f),w,height-vertical);}vertical+=tile-(1.0f);frame++;if(frame>=6)frame=4;}
 float rightOffset=(0.0f);frame=4;float right=(x+width)-fn_80235EF0(image,4);
 while(rightOffset<height){float tile=fn_80235F64(image,frame);if(tile-(1.0f)<height-rightOffset)fn_802353F4(image,frame,right,y-rightOffset,(0.0f));else{float w=fn_80235EF0(image,frame);fn_802354A0(image,frame,0xffffffffu,right,y-rightOffset,(0.0f),w,height-rightOffset);}rightOffset+=tile-(1.0f);frame++;if(frame>=8)frame=6;}
 fn_802353F4(image,0,x-(2.0f),(2.0f)+y,(0.0f));
 float w=fn_80235EF0(image,1);fn_802353F4(image,1,((2.0f)+(x+width))-w,(2.0f)+y,(0.0f));
 float cornerY=(y-height-(2.0f))+fn_80235F64(image,2);fn_802353F4(image,2,x-(2.0f),cornerY,(0.0f));
 y=(y-height-(2.0f))+fn_80235F64(image,3);w=fn_80235EF0(image,3);fn_802353F4(image,3,((2.0f)+(x+width))-w,y,(0.0f));
 }}
void fn_80227C9C(void* self,float x,float y,float width,float height){fn_80227880(x,y,width,height);((void(*)(void*,float,float,float,float))fn_802278F0)(self,x,y,width,height);}
WidgetFragment* fn_80227D00(WidgetFragment* self,WidgetDesc* desc){fn_80218F80(self,desc);self->vtable=lbl_8033DEAC;void* image=fn_8023512C(lbl_8033DC4C);((AssetImage244*)self)->image=image;if(!image)fn_802E30C8(lbl_8033DC58);return self;}
WidgetFragment* fn_80227D64(WidgetFragment* self,int destroy){if(self){self->vtable=lbl_8033DEAC;AssetImage244* f=(AssetImage244*)self;if(f->image)fn_80235380(f->image);f->image=0;fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;}
void fn_80227DE8(WidgetFragment* self){float x,y;fn_80230C30(&x,&y,self->left,self->top);float width=(float)((WidgetDispatch*)self)->Slot78();fn_80227C9C(((AssetImage244*)self)->image,x,y,width,(float)(self->bottom-self->top));}
WidgetFragment* fn_80227E84(WidgetFragment* self,void* first,WidgetDesc* desc,void* second){const char* strings=lbl_8033D878;fn_80218F80(self,desc);ThreeResourcePrefix* f=(ThreeResourcePrefix*)self;f->first=first;self->vtable=lbl_8033DF30;f->second=second;f->flag=*(const int*)((const char*)desc+0xb0)==1;f->resource=fn_80232578(strings+0x20c);if(!f->resource)fn_802E30C8(strings+0x408);f->image=fn_8023512C(strings+0x42c);if(!f->image)fn_802E30C8(strings+0x438);f->overlay=fn_8023512C(strings+0x3d4);if(!f->overlay)fn_802E30C8(strings+0x3e0);return self;}
WidgetFragment* fn_80227F5C(WidgetFragment* self,int destroy){if(self){self->vtable=lbl_8033DF30;TripleResourcePrefix* f=(TripleResourcePrefix*)self;if(f->resource)fn_802328D4(f->resource);f->resource=0;if(f->first)fn_80235380(f->first);f->first=0;if(f->second)fn_80235380(f->second);f->second=0;fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;}
void fn_80228010(void) {}
WidgetFragment* fn_80228418(WidgetFragment* self,WidgetDesc* desc,SwfAnim* animation){
 fn_80218F80(self,desc);SwfWidgetPrefix* fields=(SwfWidgetPrefix*)self;
 fields->animation=0;self->vtable=lbl_8033DE28;fields->owned=0;
 if(animation)fields->animation=animation;
 else{fields->animation=new(lbl_8033DCDC)SwfAnim;fields->owned=1;}
 fields->animation->Load((const char*)desc+0x40);return self;
}
WidgetFragment* fn_802284BC(WidgetFragment* self,int destroy){
 if(self){self->vtable=lbl_8033DE28;SwfWidgetPrefix* fields=(SwfWidgetPrefix*)self;
  fields->animation->Unload();if(fields->owned)__dt__7SwfAnimFv(fields->animation,1);
  fields->animation=0;fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);
 }return self;
}
void fn_8022854C(WidgetFragment* self){((SwfWidgetPrefix*)self)->animation->Draw();}
void fn_80228554(WidgetFragment* self){((SwfWidgetPrefix*)self)->animation->Update();}
WidgetFragment* fn_8022855C(WidgetFragment* self,WidgetDesc* desc,DynoTex* texture,int* frame){
 fn_80218F80(self,desc);self->vtable=lbl_8033DDA4;DynoWidgetPrefix* fields=(DynoWidgetPrefix*)self;
 fields->texture=texture;fields->frame=frame;return self;
}
void fn_802285B8(WidgetFragment* self){
 float x,y;fn_80230C30(&x,&y,self->left,self->top);
 DynoWidgetPrefix* fields=(DynoWidgetPrefix*)self;int frame;if(fields->frame)frame=*fields->frame;else frame=0;
 if(fields->texture){int width=((WidgetDispatch*)self)->Slot78();fields->texture->Draw(frame,(int)x,(int)y,width,self->bottom-self->top);}
 else{int width=((WidgetDispatch*)self)->Slot78();fn_80230924(0x8FAFBFCFu,x,y,x+(float)width,y-(float)(self->bottom-self->top),(0.0f));}
}
WidgetFragment* fn_802286E4(WidgetFragment* self,WidgetDesc* desc,void* context){
 fn_80218F80(self,desc);StreamWidget348* f=(StreamWidget348*)self;
 f->context=context;f->asset=0;self->vtable=lbl_8033DD20;f->owned=0;memset(f->cache,0,32);
 StreamAsset348* asset=(StreamAsset348*)__nw__FUlPCc(64,lbl_8033DCE8);
 if(asset){asset->vtable=lbl_8033DCF8;asset->resource=-1;asset->elapsed=(0.0f);asset->period=(0.0f);
 if(fn_8012C33C(desc->name))asset->resource=fn_802C6FC0(desc->name,0,0);
 if(desc->field9c)asset->period=(6.2831855f)/(float)(u32)desc->field9c;
 asset->matrix[0]=(1.0f);asset->matrix[1]=(0.0f);asset->matrix[2]=(0.0f);asset->matrix[3]=(0.0f);
 asset->matrix[4]=(1.0f);asset->matrix[5]=(0.0f);asset->matrix[6]=(0.0f);asset->matrix[7]=(0.0f);
 asset->matrix[8]=(1.0f);asset->matrix[9]=(0.0f);asset->matrix[10]=(0.0f);asset->matrix[11]=(0.0f);
 }f->asset=asset;f->owned=1;return self;
}
void* fn_8022882C(AdapterTarget* self,int destroy){if(self){if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_8022886C(WidgetFragment* self,WidgetDesc* desc,AdapterTarget* target,const char* name){
 fn_80218F80(self,desc);AdapterPrefix* fields=(AdapterPrefix*)self;fields->name=name;self->vtable=lbl_8033DD20;fields->target=target;fields->owned=0;
 memset(fields->lastName,0,32);return self;
}
WidgetFragment* fn_802288E0(WidgetFragment* self,int destroy){
 if(self){self->vtable=lbl_8033DD20;AdapterPrefix* fields=(AdapterPrefix*)self;
  if(fields->owned){delete fields->target;fields->target=0;}
  fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);
 }return self;
}
int fn_8022897C(WidgetFragment* self){return ((AdapterPrefix*)self)->target->Enter();}
void fn_80228990(WidgetFragment*){}
void fn_80228994(WidgetFragment* self){
 AdapterPrefix* fields=(AdapterPrefix*)self;
 if(fields->name&&strcasecmp(fields->lastName,fields->name)!=0){
  const char* source=fields->name;int remaining=32;char* destination=fields->lastName;
  while((*destination++=*source++)!=0){if(--remaining<=0){destination[-1]=0;break;}}
  fields->target->Name(fields->lastName);
 }
 fn_80228AEC(self,fn_80214D48());
}
void fn_80228A38(WidgetFragment* self){fn_80228AEC(self,fn_80214D48());}
int fn_80228A6C(WidgetFragment* self){return ((AdapterPrefix*)self)->target->Leave();}
void fn_80228A80(WidgetFragment*){}
void fn_80228A84(WidgetFragment* self){
 AdapterPrefix* fields=(AdapterPrefix*)self;ObservedClipRect saved;
 fn_802C86F0(&saved,fields->left,fields->top,fields->right,fields->bottom);
 fn_802C88BC(2);fields->target->Draw();fn_802C8750(&saved,-1);
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
int fn_80228D90(WidgetFragment* self,int){return ((AdapterPrefix*)self)->target->Flags();}
int fn_80228DA4(WidgetFragment*){return 0;}
int fn_80228DAC(WidgetFragment* self){return ((AdapterPrefix*)self)->target->State();}
void fn_80228DC0(WidgetFragment*){}
void fn_80228DC4(ObservedClipPrefix* context,void* image,int x,int y,int width,int){fn_802C3034(context);ObservedClipRect saved;fn_80218E98(context,&saved);fn_802C51C0(image);int height=fn_802C5220(image);int bottom=y+height;int middle=x+width/2;fn_80218EE0(context,x,y,middle,bottom);fn_80218EBC(context,&saved);fn_80218EE0(context,middle,y,x+width,bottom);fn_80218EBC(context,&saved);}
void fn_80228E80(ObservedClipPrefix* context,void* image,int x,int y,int height){fn_802C3034(context);ObservedClipRect saved;fn_80218E98(context,&saved);int width=fn_802C51C0(image);int imageHeight=fn_802C5220(image);int middle=y+height/2;fn_80218EE0(context,x,y,x+imageHeight,middle);fn_80218EBC(context,&saved);fn_80218EE0(context,x,middle,x+width,y+height);fn_80218EBC(context,&saved);}
int fn_80228F40(int x,int y){return x+1000*y;}
WidgetFragment* fn_80228F4C(WidgetFragment* self,int destroy){if(self){fn_80223208(self,0);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_80228FA4(WidgetFragment* self,int destroy){if(self){fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_80228FFC(WidgetFragment* self,int destroy){if(self){fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_80229054(WidgetFragment* self,int destroy){if(self){ObservedNullable_fn_8021915C(self);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_802290B0(WidgetFragment* self,int destroy){if(self){ObservedNullable_fn_8021915C(self);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_8022910C(WidgetFragment* self,int destroy){if(self){ObservedNullable_fn_8021915C(self);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_80229168(WidgetFragment* self,int destroy){if(self){ObservedNullable_fn_8021915C(self);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_802291C4(WidgetFragment* self,int destroy){if(self){ObservedNullable_fn_8021915C(self);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_80229220(WidgetFragment* self,int destroy){if(self){ObservedNullable_fn_8021915C(self);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_8022927C(WidgetFragment* self,int destroy){if(self){ObservedNullable_fn_8021915C(self);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_802292D8(WidgetFragment* self,int destroy){if(self){fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_80229330(WidgetFragment* self,int destroy){if(self){fn_802223C4(self,0);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_80229388(WidgetFragment* self,int destroy){if(self){fn_802223C4(self,0);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_802293E0(WidgetFragment* self,int destroy){if(self){ObservedNullable_fn_8021915C(self);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_8022943C(WidgetFragment* self,int destroy){if(self){ObservedNullable_fn_8021915C(self);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_80229498(WidgetFragment* self,int destroy){if(self){ObservedNullable_fn_8021915C(self);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_802294F4(WidgetFragment* self,int destroy){if(self){ObservedNullable_fn_8021915C(self);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_80229550(WidgetFragment* self,int destroy){if(self){ObservedNullable_fn_802223C4(self);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_802295AC(WidgetFragment* self,int destroy){if(self){fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;}
void fn_80229604(WidgetFragment* self,int,int){((LeafByte24c*)self)->value=0;}
void fn_80229610(ResourcePrefix*){}
int fn_80229614(ResourcePrefix*){return 1;}
void fn_8022961C(ImagePrefix*){}
int fn_80229620(ImagePrefix*){return 1;}
void fn_80229628(WidgetFragment* self,int,int){((CallbackPrefix*)self)->active=1;((WidgetDispatch*)self)->Slot84();}
void fn_80229640(WidgetFragment* self,int,int){
 ((WidgetDispatch*)self)->Slot84();CallbackPrefix* fields=(CallbackPrefix*)self;fields->active=0;
 if(self->flag18&&fields->context!=(void*)-66666){if(!fields->callback||fields->callback(fields->context))fn_802168BC(fields->context);}
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
 if(fields->flag257&&fields->active&&self->flag18){if(!fields->callback||fields->callback(fields->context))fn_802168BC(fields->context);}
 fn_802192AC(self);
}
void fn_8022984C(WidgetFragment* self,int,int){bool* flag=(bool*)((FlagTextPrefix*)self)->flag;*flag=!*flag;}
void fn_80229864(WidgetFragment* self){WidgetMarkDirty(self);self->flag18=1;}
void fn_80229884(WidgetFragment* self){WidgetMarkDirty(self);self->flag18=0;}
char* fn_802298A4(WidgetFragment* self){return (char*)self+0x258;}
int fn_802298AC(WidgetFragment* self){int mode=0;if(self->flag18)mode=2;return (((FlagTextPrefix*)self)->flag[0]!=0)|mode;}
int fn_802298DC(WidgetFragment* self){int* value=((IntWatchPrefix*)self)->value;if(value)return *value;return 0;}
void fn_802298F8(WidgetFragment*,void*){}
void fn_802298FC(WidgetFragment*,void*){}
void fn_80229900(WidgetFragment* self){((WidgetDispatch*)self)->Slot74(0,0);}
void fn_80229918(WidgetFragment* self){((WidgetDispatch*)self)->Slot84();self->flag18=1;}
void fn_80229958(WidgetFragment* self){((WidgetDispatch*)self)->Slot84();self->flag18=0;}
void fn_80229998(WidgetFragment* self,int,int){LeafStoredValue* fields=(LeafStoredValue*)self;if(fields->destination)*fields->destination=fields->value;}
int fn_802299B0(WidgetFragment* self){int result=0;if(self->flag18)result=2;int selected=0;int* value=(int*)self->label;if(value&&*value==(int)self->unknown258)selected=1;return (selected!=0)|result;}
void fn_802299FC(WidgetFragment* self){const char* label;int height=self->bottom-self->top;label=((WidgetDispatch*)self)->Slot88();int width=((WidgetDispatch*)self)->Slot78();DelegatePaintPrefix* f=(DelegatePaintPrefix*)self;f->child->Paint(f->first,f->second,f->third,width,height,label);}
void fn_80229A94(WidgetFragment* self){self->flag19=1;WidgetMarkDirty(self);((PointerEventDispatch*)self)->Slot3c(self->left,self->top);}
void fn_80229AC4(WidgetFragment* self,int,int){LeafStoredValue* fields=(LeafStoredValue*)self;if(fields->destination)*fields->destination=fields->value;}
int fn_80229ADC(WidgetFragment*){return 1;}
void fn_80229AE4(WidgetFragment* self,int x,int y){LeafChildPrefix* fields=(LeafChildPrefix*)self;fields->child->Paint(fields->first,fields->second,x,y);}
void fn_80229B0C(WidgetFragment* self,int dx,int dy){fn_80219264(self,dx,dy);self->y+=dx;self->width+=dy;}
int fn_80229B68(WidgetFragment*){return 0;}
void* fn_80229B70(WidgetFragment* self){return ((IntWatchPrefix*)self)->value;}
WidgetFragment* fn_80229B78(WidgetFragment* self,int destroy){if(self){self->vtable=lbl_8033EFF4;fn_801E4A2C(((RichCaptionPrefix*)self)->renderer);fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;}
void fn_80229BE8(WidgetFragment* self){fn_802192AC(self);RichCaptionPrefix* f=(RichCaptionPrefix*)self;const char* label=f->label;if(strcmp(label,f->text)!=0){strcpy(f->text,label);int width=fn_801E4A6C(f->renderer,f->label);int oldWidth=((WidgetDispatch*)self)->Slot78();if(oldWidth>width)width=oldWidth;int height=fn_801E4BA8(f->renderer);fn_80219284(self,width,height);WidgetMarkDirty(self);}}
void* fn_80229C9C(WidgetFragment* self){return(char*)self+0x264;}
void* fn_80229CA4(WidgetFragment* self){return(char*)self+0x254;}
void* fn_80229CAC(WidgetFragment* self){return(char*)self+0x254;}
void fn_80229CB4(WidgetFragment* self,int,int){LeafActivity* fields=(LeafActivity*)self;fields->active=1;fields->elapsed=(0.0f);WidgetMarkDirty(self);}
void fn_80229CD8(WidgetFragment* self,int,int){((LeafActivity*)self)->active=0;WidgetMarkDirty(self);}
void fn_80229CF8(WidgetFragment* self){self->flag18=1;((LeafActivity*)self)->elapsed=(0.0f);WidgetMarkDirty(self);}
void fn_80229D1C(WidgetFragment* self){self->flag18=0;WidgetMarkDirty(self);}
void fn_80229D3C(WidgetFragment* self,int x,int y){fn_802C5280(((LeafResource250*)self)->resource,x-self->left,y-self->top,0);}
void* fn_80229D5C(WidgetFragment* self){return(char*)self+0x264;}
WidgetFragment* fn_80229D64(WidgetFragment* self,int destroy){if(self){ObservedNullable_fn_8021915C(self);if(destroy>0)__dl__FPv(self);}return self;}
void* fn_80229DC0(WidgetFragment* self){return(char*)self+0x268;}
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
WidgetFragment* fn_80229F78(WidgetFragment* self,int destroy){if(self){self->vtable=lbl_8033E878;PairImage25c* f=(PairImage25c*)self;fn_802C519C(f->first);fn_802C519C(f->second);fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;}
void fn_80229FF0(WidgetFragment* self,int x,int y){((LeafActivity*)self)->active=1;((RowInputDispatch*)self)->Slot4c(x,y);}
void fn_8022A008(WidgetFragment* self,int,int){((LeafActivity*)self)->active=0;}
void fn_8022A014(WidgetFragment* self){WidgetMarkDirty(self);self->flag18=1;}
void fn_8022A034(WidgetFragment* self){WidgetMarkDirty(self);self->flag18=0;}
WidgetFragment* fn_8022A054(WidgetFragment* self,int destroy){if(self){self->vtable=lbl_8033E770;PairImage248* f=(PairImage248*)self;fn_802C519C(f->first);fn_802C519C(f->second);fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_8022A0CC(WidgetFragment* self,int destroy){if(self){fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;}
ResourcePrefix* fn_8022A124(ResourcePrefix* self,int destroy){if(self){self->vtable=lbl_8033DCF8;if(self->resource!=-1){fn_802C6CF8(self->resource);self->resource=-1;}if(destroy>0)__dl__FPv(self);}return self;}
void fn_8022A19C(ResourcePrefix* self,const char* name){if(self->resource!=-1){fn_802C6CF8(self->resource);self->resource=-1;}if(fn_8012C33C(name))self->resource=fn_802C6FC0(name,0,0);}
void fn_8022A210(ModelFitAssetNew * const self, int x, int y, int width, int height, float delta)
{

  if (self->resource != (-1))
  {
    self->angle += self->speed * delta;
    self->matrix = *fn_802C86DC();
    RotationVec center;
    RotationVec3New rotation((0.0f),self->angle,(0.0f));
    RotationMatrix tmp;
    fn_80015A20(&tmp, &rotation);
    fn_80015B20(&self->matrix, &tmp, &self->matrix);
    ModelFitBoundsNew bounds;
    fn_802C77F0(self->resource, &bounds);
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
    self->matrix.translation.z += minimum;
    center.z = ModelFitCenterLerpNew(bounds.z.minimum, bounds.z.maximum, GaugeHalfReferenceObservedNew());
    center.y = ModelFitCenterLerpNew(bounds.y.minimum, bounds.y.maximum, GaugeHalfReferenceObservedNew());
    center.x = ModelFitCenterLerpNew(bounds.x.minimum, bounds.x.maximum, GaugeHalfReferenceObservedNew());
    ModelFitTransformNew(&center, &self->matrix, &center);
    self->matrix.translation.x -= center.x;
    self->matrix.translation.y -= center.y;
    self->matrix.translation.z -= center.z;
  }
}
void fn_8022A574(ResourcePrefix* self){if(self->resource!=-1)fn_802C744C(self->resource,(char*)self+8);}
void fn_8022A594(){RegisterModuleTag__FP9ModuleTag(lbl_8055EFB0);StartupSharedPrefixNew* settings=&lbl_80490920;settings->kind=123;settings->fifth=settings->fourth=settings->third=settings->first=0;lbl_8055EFB8=0x7f000000;}
}
