// Anonymous widget-class fragment; no recovered TU or data ownership claim.
typedef unsigned int u32;
struct WidgetDispatch {virtual ~WidgetDispatch();virtual void Slot0c();virtual void Slot10();virtual void Slot14();virtual void Slot18();virtual void Slot1c(int,int);virtual void Slot20();virtual void Slot24();virtual void Slot28();virtual void Slot2c();virtual void Slot30();virtual void Slot34();virtual void Slot38();virtual void Slot3c();virtual void Slot40();virtual void Slot44();virtual void Slot48();virtual void Slot4c();virtual void Slot50();virtual void Slot54();virtual void Slot58();virtual void Slot5c();virtual void Slot60();virtual void Slot64();virtual void Slot68();virtual void Slot6c();virtual void Slot70();virtual void Slot74(int,void*);virtual int Slot78();virtual void Slot7c();virtual void Slot80();virtual int Slot84();virtual const char* Slot88();};
struct OwnedWidget {virtual ~OwnedWidget();virtual void Slot0c();virtual void Slot10();virtual void Slot14();virtual void Slot18();virtual int Slot1c();};
struct CallbackPrefix {char prefix[0x254];unsigned char active,unknown255,flag256,flag257;int(*callback)(void*);void* context;};
struct OwnedWidgetPrefix {char prefix[0x244];OwnedWidget* child;int x,y,kind;};
struct ImagePrefix {void* vtable;void* image;};
struct ResourcePrefix {void* vtable;int resource;const char* text;};
struct WidgetProperty {void* value;int key,bytes;void* buffer;};
struct WidgetDesc {u32 unknown[2];int x,y,kind,dx,dy;char unknown1c[0x80];int field9c,fielda0,fielda4;};
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
WidgetFragment* fn_80219568(WidgetFragment* self,int x,int y,int width,int height,int dx,int dy){
 fn_80218F34(self,x,y,x+dx,y+dy,width);
 self->vtable=lbl_8033FA28;
 self->x=0;self->y=0;self->width=width;self->height=height;
 return self;
}
WidgetFragment* fn_80219900(WidgetFragment* self,int destroy){
 if(self){fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}
 return self;
}
WidgetFragment* fn_8021B2B0(WidgetFragment* self,int x,int y,const char* label,int height,int* value,int width){
 fn_80219568(self,x,y,width,height,0,0);
 self->vtable=lbl_8033F20C;self->label=label;
 strcpy((char*)self+0x268,label);self->value=value;fn_80219718(self);return self;
}
int fn_8021B3A0(WidgetFragment* self){int* value=self->value;if(value)return *value;return 0;}
WidgetFragment* fn_8021B404(WidgetFragment* self,int destroy){
 if(self){if(self)fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}
 return self;
}

WidgetFragment* fn_80218F34(WidgetFragment* self,int left,int top,int right,int bottom,int kind){
 self->vtable=lbl_8033FB38;fn_80218FEC(self,left,top,right,bottom,kind);
 self->style=fn_8021525C();self->stylePtr=0;return self;
}
WidgetFragment* fn_80218F80(WidgetFragment* self,const WidgetDesc* desc){
 self->vtable=lbl_8033FB38;fn_80218FEC(self,desc->x,desc->y,desc->x+desc->dx,desc->y+desc->dy,desc->kind);
 self->style=fn_8021525C();self->stylePtr=0;return self;
}
void fn_80218FEC(WidgetFragment* self,int left,int top,int right,int bottom,int kind){
 self->flag1a=1;self->enabled=1;self->fadeIn=lbl_805634A0;self->fadeOut=lbl_805634A0;
 self->speedIn=lbl_805634A4;self->speedOut=lbl_805634A4;self->dirty=1;self->group=0;self->flag18=0;self->propertyCount=0;
 self->left=left;self->right=right;self->top=top;self->bottom=bottom;self->kind=kind;self->flag19=0;self->window=0;
}
WidgetFragment* fn_8021915C(WidgetFragment* self,int destroy){
 if(self){self->vtable=lbl_8033FB38;for(int i=0;i<self->propertyCount;i++)__dla__FPv(self->properties[i].buffer);if(destroy>0)__dl__FPv(self);}return self;
}

void fn_80219044(WidgetFragment*);
int memcmp(const void*,const void*,unsigned long);
void* memcpy(void*,const void*,unsigned long);
void* __nwa__FUlPCc(unsigned long,const char*);
extern char lbl_8033D878[];
void fn_802179A0(void*);void fn_80218198(void*,void*);void fn_80217AC0(void*);
void fn_802191F0(WidgetFragment* self,int x,int y){
 int right=x+((WidgetDispatch*)self)->Slot78();
 int bottom=y+(self->bottom-self->top);self->left=x;self->right=right;self->top=y;self->bottom=bottom;
}
void fn_80219264(WidgetFragment* self,int x,int y){((WidgetDispatch*)self)->Slot1c(self->left+x,self->top+y);}
void fn_80219284(WidgetFragment* self,int width,int height){self->right=self->left+width;self->bottom=self->top+height;}
void fn_802192A0(WidgetFragment*,void*){}
int fn_802192A4(WidgetFragment*){return 0;}
void fn_802192AC(WidgetFragment* self){
 int bytes;void* value;void* previous;for(int i=0;i<self->propertyCount;i++){
  bytes=self->properties[i].bytes;previous=self->properties[i].buffer;value=self->properties[i].value;
  if(memcmp(value,previous,bytes)!=0){memcpy(previous,value,bytes);((WidgetDispatch*)self)->Slot74(self->properties[i].key,self->properties[i].value);}
 }fn_80219044(self);
}
void fn_80219354(WidgetFragment* self){fn_80219044(self);}
int fn_80219440(WidgetFragment* self,int key,void* value,int bytes){
 void* previous=__nwa__FUlPCc(bytes,lbl_8033D878);
 self->properties[self->propertyCount].key=key;self->properties[self->propertyCount].value=value;
 self->properties[self->propertyCount].bytes=bytes;self->properties[self->propertyCount].buffer=previous;
 memcpy(previous,value,bytes);return self->propertyCount++;
}
void fn_802194F4(WidgetFragment* self){fn_802179A0(self->window);}
void fn_802194FC(WidgetFragment* self){fn_80218198(self->window,0);}
void fn_80219508(WidgetFragment* self){fn_80217AC0(self->window);}

inline void WidgetMarkDirty(WidgetFragment* self){if(self->flag1a)self->dirty=1;}
inline void WidgetShow(WidgetFragment* self){if(!self->flag1a){self->flag1a=1;WidgetMarkDirty(self);}}
inline void WidgetHide(WidgetFragment* self){if(self->flag1a){WidgetMarkDirty(self);self->flag1a=0;}}
void fn_80219044(WidgetFragment* self){
 if(self->flag1a){if(self->fadeIn>=lbl_805634A8){self->fadeIn-=self->speedIn*fn_80214D48();if(self->fadeIn<=lbl_805634AC){WidgetHide(self);self->fadeIn=lbl_805634A0;}}}
 else self->fadeIn=lbl_805634A0;
 if(!self->flag1a){if(self->fadeOut>=lbl_805634A8){self->fadeOut-=self->speedOut*fn_80214D48();if(self->fadeOut<=lbl_805634AC){WidgetShow(self);self->fadeOut=lbl_805634A0;}}}
 else self->fadeOut=lbl_805634A0;
}
void fn_80219358(WidgetFragment* self,float duration){
 if(duration<lbl_805634B0){WidgetShow(self);return;}
 if(!self->flag1a&&self->fadeOut<lbl_805634B4){self->fadeOut=lbl_805634A4;duration=duration>lbl_805634B0?duration:lbl_805634B0;self->speedOut=lbl_805634A4/duration;}
 self->fadeIn=lbl_805634A0;
}
void fn_802193C8(WidgetFragment* self,float duration){
 if(duration<lbl_805634B0){WidgetHide(self);return;}
 if(self->flag1a&&self->fadeIn<lbl_805634B4){self->fadeIn=lbl_805634A4;duration=duration>lbl_805634B0?duration:lbl_805634B0;self->speedIn=lbl_805634A4/duration;}
 self->fadeOut=lbl_805634A0;
}

void fn_802C3034(void*);
void fn_8022B91C(int,int,int,const char*,u32);
void fn_8022BAD0(int,int,int,int,int,int,const char*,u32,unsigned char,float);
void fn_8022AC1C(int,const char*,int,int,int*,int*,int*,int*);
inline int WidgetMinInt(int value,int limit){if(value<limit)return value;return limit;}
inline int WidgetMaxInt(int value,int limit){if(value>limit)return value;return limit;}
void fn_802195DC(WidgetFragment* self,void* context){
 fn_802C3034(context);int frame=((WidgetDispatch*)self)->Slot84();const char* text=((WidgetDispatch*)self)->Slot88();
 fn_8022B91C(self->height,self->x,self->y,text,frame);
}
int fn_80219658(WidgetFragment*){return 0;}
void fn_80219660(WidgetFragment* self){
 const char* text;int frame,height,x,y;
 y=self->top;x=self->left;height=self->bottom-y;frame=((WidgetDispatch*)self)->Slot84();text=((WidgetDispatch*)self)->Slot88();int width=((WidgetDispatch*)self)->Slot78();
 fn_8022BAD0(self->height,self->x+x,self->y+y,self->width,width,height,text,frame,0,lbl_805634A4);
}
void fn_80219718(WidgetFragment* self){
 int left,top,right,bottom;const char* text=((WidgetDispatch*)self)->Slot88();
 fn_8022AC1C(self->height,text,self->x+self->left,self->y+self->top,&left,&top,&right,&bottom);
 left=WidgetMinInt(left,self->left);int oldLeft=self->left;right=WidgetMaxInt(right,oldLeft+((WidgetDispatch*)self)->Slot78());
 top=WidgetMinInt(top,self->top);bottom=WidgetMaxInt(bottom,self->bottom);
 self->x+=self->left-left;self->y+=self->top-top;
 ((WidgetDispatch*)self)->Slot1c(left,top);fn_80219284(self,right-left,bottom-top);
}

extern u32 lbl_8033F768[],lbl_8033F4B8[],lbl_8033F434[];
WidgetFragment* fn_80222340(WidgetFragment*,int,int,int,int,int,OwnedWidget*);
WidgetFragment* fn_802223C4(WidgetFragment*,int);
void fn_80219AB4(WidgetFragment*,int,const char*,int,int,int,bool);
WidgetFragment* fn_80219870(WidgetFragment* self,int x,int y,const char* text,unsigned char* flag,int height){
 fn_80219568(self,x,y,0,height,0,0);self->vtable=lbl_8033F768;
 strcpy((char*)self+0x258,text);((FlagTextPrefix*)self)->flag=flag;
 fn_80219440(self,0,flag,1);fn_80219718(self);return self;
}
int fn_80219958(WidgetFragment* self){return fn_802192A4(self);}
WidgetFragment* fn_8021995C(WidgetFragment* self,int x,int y,int width,OwnedWidget* value,void* watched){
 fn_80222340(self,x,y,width,0,0,value);self->vtable=lbl_8033F4B8;
 self->label=(const char*)watched;if(watched)fn_80219440(self,0,watched,4);return self;
}
WidgetFragment* fn_802199D0(WidgetFragment* self,int destroy){if(self){fn_802223C4(self,0);if(destroy>0)__dl__FPv(self);}return self;}
WidgetFragment* fn_80219A28(WidgetFragment* self,const WidgetDesc* desc,const char* text,int value){
 fn_80218F80(self,desc);self->vtable=lbl_8033F434;
 fn_80219AB4(self,desc->unknown[1],text,desc->field9c,value,desc->fielda0,desc->fielda4!=0);return self;
}

void fn_8022984C(WidgetFragment* self){bool* flag=(bool*)((FlagTextPrefix*)self)->flag;*flag=!*flag;}
void fn_80229864(WidgetFragment* self){WidgetMarkDirty(self);self->flag18=1;}
void fn_80229884(WidgetFragment* self){WidgetMarkDirty(self);self->flag18=0;}
char* fn_802298A4(WidgetFragment* self){return (char*)self+0x258;}
int fn_802298AC(WidgetFragment* self){int mode=0;if(self->flag18)mode=2;return (((FlagTextPrefix*)self)->flag[0]!=0)|mode;}
int fn_802298DC(WidgetFragment* self){int* value=((IntWatchPrefix*)self)->value;if(value)return *value;return 0;}

extern u32 lbl_8033F90C[];
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

extern u32 lbl_8033F880[];
void fn_802168BC(void*);
WidgetFragment* fn_802224B4(WidgetFragment* self,void* context,int x,int y,int dx,int dy,OwnedWidget* child,int(*callback)(void*),unsigned char a,unsigned char b){
 fn_80222340(self,x,y,0,dx,dy,child);self->vtable=lbl_8033F880;
 CallbackPrefix* fields=(CallbackPrefix*)self;fields->context=context;fields->callback=callback;fields->flag256=a;fields->flag257=b;fields->unknown255=0;fields->active=0;return self;
}
void fn_80229628(WidgetFragment* self){((CallbackPrefix*)self)->active=1;((WidgetDispatch*)self)->Slot84();}
void fn_80229640(WidgetFragment* self){
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
extern u32 lbl_8033F9E0[];
int fn_80232578(const char*);void fn_802328D4(int);
void fn_80230C30(float*,float*,int,int);
void fn_802336AC(int,int,float,float,float,int,const char*,...);
float fn_8023293C(int,const char*,float);int fn_80232B70(int);
extern const char lbl_8055B8A4[3];
ResourcePrefix* fn_80222530(ResourcePrefix* self,const char* path,const char* text){
 self->vtable=lbl_8033F9E0;self->resource=fn_80232578(path);self->text=text;return self;
}
ResourcePrefix* fn_80222584(ResourcePrefix* self,int destroy){
 if(self){self->vtable=lbl_8033F9E0;fn_802328D4(self->resource);if(destroy>0)__dl__FPv(self);}return self;
}
void fn_802225E8(ResourcePrefix* self,int x,int y){
 float a,b;fn_80230C30(&a,&b,x,y);fn_802336AC(self->resource,-1,a,b,lbl_805634A8,9,lbl_8055B8A4,self->text);
}
void fn_8022264C(ResourcePrefix* self,int first,int second,int* left,int* top,int* right,int* bottom){
 *left=first;*top=second;*right=first+(int)fn_8023293C(self->resource,self->text,lbl_805634A4);*bottom=first+fn_80232B70(self->resource);
}

extern u32 lbl_8033F9BC[];
void* fn_8023512C(const char*);void fn_80235380(void*);
void fn_802353F4(void*,int,float,float,float);
float fn_80235EF0(void*,int);float fn_80235F64(void*,int);
ImagePrefix* fn_802226D4(ImagePrefix* self,const char* path){self->vtable=lbl_8033F9BC;self->image=fn_8023512C(path);return self;}
ImagePrefix* fn_80222718(ImagePrefix* self,int destroy){
 if(self){void* image=self->image;self->vtable=lbl_8033F9BC;if(image)fn_80235380(image);if(destroy>0)__dl__FPv(self);}return self;
}
void fn_80222788(ImagePrefix* self,int x,int y,int unused,int frame){
 if(self->image){float zero=lbl_805634A8;float a,b;fn_80230C30(&a,&b,x,y);fn_802353F4(self->image,frame,a,b,zero);}
}
void fn_80222800(ImagePrefix* self,int first,int second,int* left,int* top,int* right,int* bottom){
 *left=first;*top=second;
 if(!self->image){*right=first+10;*bottom=second+10;}
 else{*right=first+(int)fn_80235EF0(self->image,0);*bottom=first+(int)fn_80235F64(self->image,0);}
}
void fn_80229610(ResourcePrefix*){}
int fn_80229614(ResourcePrefix*){return 1;}
void fn_8022961C(ImagePrefix*){}
int fn_80229620(ImagePrefix*){return 1;}
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
AnimationPrefix* fn_802228B0(AnimationPrefix* self,const char* path,int width,int height){
 self->vtable=lbl_8033F998;self->handle=fn_802C5130(path);
 if(width>0&&height>0){self->width=(float)width;self->height=(float)height;}
 else{self->width=(float)fn_802C51C0(self->handle);self->height=(float)fn_802C5220(self->handle);}
 return self;
}

void fn_802C519C(void*);int fn_802C51AC(void*);
void fn_802C5320(void*,int,float,float,float,float,float);
AnimationPrefix* fn_80222998(AnimationPrefix* self,int destroy){
 if(self){self->vtable=lbl_8033F998;fn_802C519C(self->handle);if(destroy>0)__dl__FPv(self);}return self;
}
int fn_802229FC(AnimationPrefix* self){int result=fn_802C51AC(self->handle);if(result)result=fn_802C51AC(self->handle);return result;}
void fn_80222A3C(AnimationPrefix* self,int x,int y,int unused,u32 frame){
 if(fn_802C51AC(self->handle)){
  float zero=lbl_805634A8;float a=zero,b=zero;fn_80230C30(&a,&b,x,y);
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
void fn_8021A744(WidgetFragment* self){PlaybackPrefix* fields=(PlaybackPrefix*)self;fn_8022F824(fields->active);fields->active=0;}
}


struct WatchedChildPrefix {char base[0x254];void* watched;int kind;};
extern "C" {
extern u32 lbl_8033F324[],lbl_8033F298[];
void* fn_80216708();
WidgetFragment* fn_8021A77C(WidgetFragment* self,int x,int y,int kind,void* watched,OwnedWidget* child){
 fn_80222340(self,x,y,0,0,0,child);self->vtable=lbl_8033F324;
 WatchedChildPrefix* fields=(WatchedChildPrefix*)self;fields->watched=watched;fields->kind=kind;
 if(watched)fn_80219440(self,0,watched,4);return self;
}
int fn_8021A804(WidgetFragment* self){return fn_802192A4(self);}
WidgetFragment* fn_8021A808(WidgetFragment* self,int x,int y,int kind,void* watched,OwnedWidget* child){
 fn_80222340(self,x,y,0,0,0,child);self->vtable=lbl_8033F298;
 WatchedChildPrefix* fields=(WatchedChildPrefix*)self;fields->watched=watched;fields->kind=kind;
 if(watched)fn_80219440(self,0,watched,4);fn_80219440(self,1,fn_80216708(),1);return self;
}
}


struct WindowOffsetPrefix {char prefix[0x454];int x,y;};
struct WidgetWindowOffsetPrefix {char prefix[0x244];int x,y;unsigned char enabled;};
extern "C" {
void fn_8021818C(void*);
void fn_8021A8A8(WidgetFragment* self){
 ((WidgetDispatch*)self)->Slot74(2,0);fn_8021818C(self->window);
 WatchedChildPrefix* fields=(WatchedChildPrefix*)self;
 if(*(int*)fields->watched==fields->kind)fn_80218198(self->window,self);
}
int fn_8021A9A4(WidgetFragment* self){
 unsigned char observed=*(unsigned char*)fn_80216708();int mode=0;if(observed)mode=2;
 WatchedChildPrefix* fields=(WatchedChildPrefix*)self;int selected=0;
 if(fields->watched&&*(int*)fields->watched==fields->kind)selected=1;
 return (selected!=0)|mode;
}
void fn_80219510(WidgetFragment* self,int x,int y){
 WidgetWindowOffsetPrefix* fields=(WidgetWindowOffsetPrefix*)self;
 x=-x;y=-y;fields->x=x;fields->enabled=1;fields->y=y;
}
void fn_8021952C(WidgetFragment* self,int x,int y){
 WidgetWindowOffsetPrefix* fields=(WidgetWindowOffsetPrefix*)self;
 WindowOffsetPrefix* window=(WindowOffsetPrefix*)self->window;
 if(fields->enabled){int dx=fields->x+x;int dy=fields->y+y;int wx=window->x;int wy=window->y;window->x=wx+dx;window->y=wy+dy;}
}
}


extern "C" {
extern const float lbl_805634E4,lbl_805634E8;
void* fn_8022F638(int,int,int,int,float);
void fn_80218968(void*,int,float);void fn_8021889C(void*,int,float);
void fn_8021A6EC(WidgetFragment* self){
 PlaybackPrefix* fields=(PlaybackPrefix*)self;
 fields->flag250=1;fields->flag251=1;fields->amount=lbl_805634E4;fields->option=1;
 fields->active=fn_8022F638(fields->resource,1,1,1,lbl_805634E4);
}
void fn_8021A910(WidgetFragment* self,int mode){
 WatchedChildPrefix* fields=(WatchedChildPrefix*)self;
 if(*(int*)fields->watched==fields->kind)fn_80218968(self->window,fields->kind,mode==2?lbl_805634A8:lbl_805634E8);
 else fn_8021889C(self->window,fields->kind,mode==2?lbl_805634A8:lbl_805634E8);
 ((WidgetDispatch*)self)->Slot84();
}
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
WidgetFragment* fn_8021AC24(WidgetFragment* self,int x,int y,void* handle){
 int height=fn_802C5220(handle);int width=fn_802C51C0(handle);
 fn_80218F34(self,x,y,x+width,y+height,0);self->vtable=lbl_8033F6E0;
 TimedAnimationPrefix* fields=(TimedAnimationPrefix*)self;fields->handle=handle;fields->active=0;fields->elapsed=lbl_805634A8;fields->period=lbl_805634E8;return self;
}
int fn_8021ACC0(WidgetFragment* self,void* context){
 fn_802C3034(context);TimedAnimationPrefix* fields=(TimedAnimationPrefix*)self;
 fn_802C51AC(fields->handle);int result=fn_802C51AC(fields->handle);
 if(result>0)result=fn_802C51AC(fields->handle);return result;
}
void fn_8021AE18(WidgetFragment* self){
 TimedAnimationPrefix* fields=(TimedAnimationPrefix*)self;
 if(fields->active&&self->flag18){
  float increment,one,zero;fields->elapsed-=fn_80214D48();zero=lbl_805634A8;
  if(fields->elapsed<zero){
   one=lbl_805634A4;increment=lbl_805634EC;int limit=10;
   while(fields->elapsed<zero){
    fields->elapsed+=fields->period;((WidgetDispatch*)self)->Slot84();
    fields->period=one/(increment+one/fields->period);
    if(--limit<0)break;
   }
   fields->elapsed=fields->period;
  }
 }else{fields->elapsed=lbl_805634A8;fields->period=lbl_805634E8;}
}
WidgetFragment* fn_8021AF0C(WidgetFragment* self,int x,int y,void* watched,int option,int mode,void* handle,float low,float high){
 fn_8021AC24(self,x,y,handle);self->vtable=lbl_8033F658;
 WatchedAnimationPrefix* fields=(WatchedAnimationPrefix*)self;fields->watched=watched;fields->low=low;fields->high=high;fields->option=option;fields->mode=mode;return self;
}
}


extern "C" {
extern int lbl_8055FB9C,lbl_8055FBA0;
void fn_802C5310(void*,int,float,float,float);
void fn_8021AD10(WidgetFragment* self){
 TimedAnimationPrefix* fields=(TimedAnimationPrefix*)self;int mode;
 if(self->flag18)mode=2+(fields->active!=0);else mode=0;
 if(fn_802C51AC(fields->handle)==3&&mode==3)mode=1;
 if(fn_802C51AC(fields->handle)>0)mode%=fn_802C51AC(fields->handle);
 int x=self->left-lbl_8055FB9C/2;int y=lbl_8055FBA0/2-self->top;
 fn_802C5310(fields->handle,mode,(float)x,(float)y,lbl_805634A8);
}
}


struct IntegerAnimationPrefix {TimedAnimationPrefix base;int* watched;int low,high,step;};
struct ObservedScrollState {int size,visible,offset;};
struct ScrollAnimationPrefix {TimedAnimationPrefix base;int direction;ObservedScrollState* state;};
extern "C" {
extern u32 lbl_8033F5D0[],lbl_8033F548[];extern const float lbl_805634C4;
WidgetFragment* fn_8021AF9C(WidgetFragment* self,int destroy){if(self){fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);}return self;}
void fn_8021AFF4(WidgetFragment* self){
 WatchedAnimationPrefix* fields=(WatchedAnimationPrefix*)self;
 float step=(fields->high-fields->low)/(float)(fields->option-1);
 *(float*)fields->watched+=(float)fields->mode*step;
 *(float*)fields->watched=step*(float)(int)(lbl_805634C4+*(float*)fields->watched/step);
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
WidgetFragment* fn_80228418(WidgetFragment* self,const WidgetDesc* desc,SwfAnim* animation){
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
WidgetFragment* fn_8022855C(WidgetFragment* self,const WidgetDesc* desc,DynoTex* texture,int* frame){
 fn_80218F80(self,desc);self->vtable=lbl_8033DDA4;DynoWidgetPrefix* fields=(DynoWidgetPrefix*)self;
 fields->texture=texture;fields->frame=frame;return self;
}
void fn_802285B8(WidgetFragment* self){
 float x,y;fn_80230C30(&x,&y,self->left,self->top);
 DynoWidgetPrefix* fields=(DynoWidgetPrefix*)self;int frame;if(fields->frame)frame=*fields->frame;else frame=0;
 if(fields->texture){int width=((WidgetDispatch*)self)->Slot78();fields->texture->Draw(frame,(int)x,(int)y,width,self->bottom-self->top);}
 else{int width=((WidgetDispatch*)self)->Slot78();fn_80230924(0x8FAFBFCFu,x,y,x+(float)width,y-(float)(self->bottom-self->top),lbl_805634A8);}
}
}


struct ObservedClipRect {int left,top,right,bottom;};
struct ObservedClipPrefix {char prefix[0x14];ObservedClipRect bounds;};
extern "C" {
void fn_80218E98(ObservedClipPrefix* self,ObservedClipRect* out){
 int left=self->bounds.left,top=self->bounds.top,right=self->bounds.right,bottom=self->bounds.bottom;
 out->left=left;out->top=top;out->right=right;out->bottom=bottom;
}
void fn_80218EBC(ObservedClipPrefix* self,const ObservedClipRect* in){self->bounds=*in;
}
void fn_80218EE0(ObservedClipPrefix* self,int left,int top,int right,int bottom){
 if(self->bounds.left>left)left=self->bounds.left;self->bounds.left=left;
 if(self->bounds.top>top)top=self->bounds.top;self->bounds.top=top;
 if(self->bounds.right<right)right=self->bounds.right;self->bounds.right=right;
 if(self->bounds.bottom<bottom)bottom=self->bounds.bottom;self->bounds.bottom=bottom;
}
}


struct ObservedValueProvider {virtual ~ObservedValueProvider();virtual void Read(void**,int*);virtual void Slot10();virtual void Slot14();virtual void Slot18();virtual void Slot1c();virtual void Slot20();virtual float Value();virtual void SetValue(float);};
struct ProviderWidgetPrefix {
 char base[0x244];unsigned char active,pad245[3];ObservedValueProvider* provider;
 void* first;void* second;int width;float elapsed;
};
extern "C" {
extern u32 lbl_8033ED44[];extern const char lbl_8033D924[],lbl_8033D934[];
WidgetFragment* fn_8021CA0C(WidgetFragment* self,int x,int y,int dx,int dy,ObservedValueProvider* provider){
 fn_80218F34(self,x,y,x+dx,y+dy,0);self->vtable=lbl_8033ED44;
 ProviderWidgetPrefix* fields=(ProviderWidgetPrefix*)self;fields->provider=provider;fields->active=0;
 void* value;int bytes;provider->Read(&value,&bytes);if(bytes>0)fn_80219440(self,0,value,bytes);
 fields->first=fn_802C5130(lbl_8033D924);fields->second=fn_802C5130(lbl_8033D934);
 fields->width=fn_802C51C0(fields->second);fields->elapsed=lbl_805634A0;return self;
}
WidgetFragment* fn_8021CAD4(WidgetFragment* self,int destroy){
 if(self){self->vtable=lbl_8033ED44;ProviderWidgetPrefix* fields=(ProviderWidgetPrefix*)self;
  fn_802C519C(fields->first);fn_802C519C(fields->second);fn_8021915C(self,0);if(destroy>0)__dl__FPv(self);
 }return self;
}
}


extern "C" {
void fn_8021CD04(WidgetFragment* self,int x,int y){
 ProviderWidgetPrefix* fields=(ProviderWidgetPrefix*)self;
 if(!fields->active){fields->active=1;fields->elapsed=fields->provider->Value();}
 int left=self->left;
 if(x<left-32||y<self->top-32||x>left+((WidgetDispatch*)self)->Slot78()+32||y>self->bottom+32){fields->provider->SetValue(fields->elapsed);}
 else{
  int left=self->left,knob=fields->width;int width=((WidgetDispatch*)self)->Slot78();
  float denominator=(float)(width-knob);float ratio=(float)(x-left-knob/2)/denominator;
  float one=lbl_805634A4;float limited=one<ratio?one:ratio;
  float zero=lbl_805634A8;limited=limited>zero?limited:zero;
  fields->provider->SetValue(limited);
 }
}
void fn_8021CE8C(WidgetFragment* self){if(((ProviderWidgetPrefix*)self)->active)((WidgetDispatch*)self)->Slot3c();}
void fn_8021CEAC(WidgetFragment* self){((WidgetDispatch*)self)->Slot4c();((ProviderWidgetPrefix*)self)->active=0;}
}


struct NumericCaptionPrefix {char base[0x254];float scale,bias;int* value;const char* format;};
typedef char NumericCaptionPrefixSize[(sizeof(NumericCaptionPrefix)==0x264)?1:-1];
extern "C" {
extern u32 lbl_8033ECB8[];
int sprintf(char*,const char*,...);void fn_80130568(char*);
void fn_8021CF9C(WidgetFragment*);
WidgetFragment* fn_8021CEEC(WidgetFragment* self,int x,int y,int* value,const char* format,int height,float scale,float bias){
 fn_80219568(self,x,y,0,height,0,0);self->vtable=lbl_8033ECB8;
 NumericCaptionPrefix* fields=(NumericCaptionPrefix*)self;fields->value=value;fields->scale=scale;fields->bias=bias;fields->format=format;
 *((char*)self+0x264)=0;fn_80219440(self,0,value,4);fn_8021CF9C(self);return self;
}
void fn_8021CF9C(WidgetFragment* self){
 NumericCaptionPrefix* fields=(NumericCaptionPrefix*)self;
 float number=fields->bias+(float)*fields->value*fields->scale;
 sprintf((char*)self+0x264,fields->format,(int)(lbl_805634C4+number));
 fn_80130568((char*)self+0x264);fn_80219718(self);WidgetMarkDirty(self);
}
void fn_8021D040(WidgetFragment* self){fn_8021CF9C(self);}
}


struct ChoiceWidgetPrefix {char base[0x254];int* value;int count;int first,second,style;char unknown268[0x50];const char** choices;int* focus;int token;};
typedef char ChoiceWidgetPrefixSize[(sizeof(ChoiceWidgetPrefix)==0x2c4)?1:-1];
extern "C" {
extern u32 lbl_8033EBA0[];
int fn_80129BC4(WidgetFragment*);int* fn_80217BD8(void*,int,WidgetFragment*);
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
WidgetFragment* fn_8021D300(WidgetFragment* self,int destroy){
 if(self){self->vtable=lbl_8033EBA0;__dla__FPv(((ChoiceWidgetPrefix*)self)->choices);
  if(self)fn_8021915C(self,0);
  if(destroy>0)__dl__FPv(self);
 }return self;
}
}


struct ChoiceWindowPrefix {char unknown[0x494];int depth;};
extern "C" {
void fn_80217D84(void*);void fn_80217C4C(void*);
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
}


extern "C" {
extern const char lbl_8033D940[];
unsigned int fn_8022D1B4(const char*);
unsigned int strlen(const char*);int fn_80228F40(int,int);void fn_8021D61C(WidgetFragment*);
WidgetFragment* fn_8021D044(WidgetFragment* self,int x,int y,int* value,const char* list,int height,int first,int second,int style){
 fn_80219568(self,x,y,0,height,0,0);self->vtable=lbl_8033EBA0;
 ChoiceWidgetPrefix* fields=(ChoiceWidgetPrefix*)self;
 fields->first=first;fields->second=second;fields->style=style;fields->value=value;fields->count=0;
 fields->choices=(const char**)__nwa__FUlPCc(fn_8022D1B4(list)*4,lbl_8033D940);
 while(*list){fields->choices[fields->count++]=list;list+=strlen(list)+1;}
 fn_80219440(self,0,value,4);fn_8021D61C(self);fields->focus=0;fields->token=fn_80228F40(x,y);return self;
}
}


extern "C" {
extern const char lbl_8055B87C[4];
void fn_8021D614(WidgetFragment* self){}
void fn_8021D618(WidgetFragment* self){fn_8021D61C(self);}
void fn_8021D61C(WidgetFragment* self){
 ChoiceWidgetPrefix* fields=(ChoiceWidgetPrefix*)self;int value=*fields->value;
 if(value>=0&&value<fields->count)strcpy((char*)self+0x268,fields->choices[value]);
 else strcpy((char*)self+0x268,lbl_8055B87C);
 fn_80219718(self);WidgetMarkDirty(self);
}
}


struct ChoiceDrawStatePrefix {char unknown[0x34];float scale;};
extern "C" {
void* fn_8021165C(int,int);const ChoiceDrawStatePrefix* fn_8022EDC4();
void fn_80219660(WidgetFragment*);
void fn_8021D378(WidgetFragment* self){
 fn_80219660(self);ChoiceWidgetPrefix* fields=(ChoiceWidgetPrefix*)self;
 if(fields->first!=fields->second){
  int selected=0;if(fields->focus&&fields->token==*fields->focus)selected=1;
  void* first=fn_8021165C(0,2);void* second=fn_8021165C(1,2);
  float scale=fn_8022EDC4()->scale;
  float zero=lbl_805634A8;float x=zero,y=zero;
  fn_80230C30(&x,&y,fields->first,fields->style);
  float width,height;height=scale*(float)fn_802C5220(first);width=(float)fn_802C51C0(first);
  fn_802C5320(first,selected,x,y-((float)fn_802C5220(first)-height),zero,width,height);
  fn_80230C30(&x,&y,fields->second,fields->style);
  height=scale*(float)fn_802C5220(second);width=(float)fn_802C51C0(second);
  fn_802C5320(second,selected,x,y-((float)fn_802C5220(second)-height),zero,width,height);
 }
}
extern int lbl_8055FBC0;
void fn_802C3424(int,int,int,int,int);
void fn_8021D568(WidgetFragment* self,void* context){
 fn_802C3034(context);ChoiceWidgetPrefix* fields=(ChoiceWidgetPrefix*)self;
 if(fields->focus&&fields->token==*fields->focus){
  int x=self->left,y=self->top;int width=((WidgetDispatch*)self)->Slot78();
  fn_802C3424(x,y,x+width,self->bottom,lbl_8055FBC0);
 }
 fn_802195DC(self,context);
}
}


struct ChoiceProviderData {u32 words[3];};
struct ChoiceProviderDispatch :ChoiceProviderData {
 virtual int Modal();virtual int Selected();virtual int AcceptsKeys();virtual void Slot14();virtual void Slot18();virtual const char* Text();
 virtual void Previous();virtual void Next();virtual int HasPrevious();virtual int HasNext();
};
struct ProviderChoicePrefix {char base[0x254];ChoiceProviderDispatch* provider;int* focus;int token;float previousTime,nextTime;int height;};
extern "C" {
extern u32 lbl_8033EC2C[];float fn_8023659C();void fn_80216D28();void fn_80230E90(int);
WidgetFragment* fn_8021D7C8(WidgetFragment* self,int x,int y,int dx,int dy,int height,ChoiceProviderDispatch* provider){
 fn_80219568(self,x,y,0,height,dx,dy);self->vtable=lbl_8033EC2C;
 ProviderChoicePrefix* fields=(ProviderChoicePrefix*)self;float time=fn_8023659C()-lbl_805634E8;
 fields->provider=provider;fields->height=height;fields->nextTime=time;fields->previousTime=time;fields->focus=0;
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
}


struct ChoiceResourceSpec {u32 words[22];};
struct ChoiceResourcePrefix {char base[0x2c4];int resource;};
extern "C" {
extern u32 lbl_8033EB14[];int fn_8022A86C(const ChoiceResourceSpec*);void fn_8022A8E4(int);
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
WidgetFragment* fn_8021E0C0(WidgetFragment* self,int x,int y,int width,int height,ObservedStringList* list,int* selected,ObservedListViewport* view){
 fn_80218F34(self,x,y,x+width,y+height,0);self->vtable=lbl_8033EA90;
 RowListPrefix* fields=(RowListPrefix*)self;fields->active=0;fields->direction=0;fields->elapsed=lbl_805634A8;
 int rowHeight=fn_8022B620(7,lbl_8055B880)+2;
 fields->list=list;fields->rowHeight=rowHeight;fields->selected=selected;fields->view=view;fields->rows=height/rowHeight;fields->columns=width/8;
 if(view){view->total=list->count;view->page=fields->rows;fn_80219440(self,0,view,12);}
 fn_8021E7D4(self);fn_80219440(self,0,selected,4);fn_80219440(self,0,&list->version,4);return self;
}
void fn_8021E1C8(WidgetFragment* self,int x,int y){
 RowListPrefix* fields=(RowListPrefix*)self;fields->direction=0;if(!fields->active)return;
 WidgetMarkDirty(self);int start=0;if(fields->view)start=fields->view->start;
 int row=start+(y-self->top-2)/fields->rowHeight;
 if(row<0)row=0;if(row>=fields->list->count)row=fields->list->count-1;
 if(fields->view){
  if(row<fields->view->start){fields->direction=-1;row=fields->view->start;}
  int end=fields->view->start+fields->view->page;
  if(row>=end){fields->direction=1;row=end-1;}
 }
 if(row>=0&&row<fields->list->count)*fields->selected=row;
}
void fn_8021E2A0(WidgetFragment* self){
 fn_802192AC(self);RowListPrefix* fields=(RowListPrefix*)self;
 if(fields->view&&fields->direction){
  fields->elapsed+=fn_80214D48();float elapsed=fields->elapsed;
  if(elapsed<lbl_805634A8||elapsed>=lbl_80563504){
   float remaining=fields->elapsed-lbl_80563504;float cap=lbl_80563510;
   fields->elapsed=cap>remaining?cap:remaining;
   SetListViewport(fields->view,fields->view->start+fields->direction);
   if(fields->direction<0)*fields->selected=fields->view->start;
   else *fields->selected=fields->view->start+fields->view->page-1;
  }
 }else fields->elapsed=lbl_805634A8;
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
}


extern "C" {
extern int lbl_8055FBDC;
const char* fn_8022D3F8(ObservedStringList*,int,int*);
void fn_8021E3BC(WidgetFragment* self,void* context){
 RowListPrefix* fields=(RowListPrefix*)self;int start=0;if(fields->view)start=fields->view->start;
 fn_802192A0(self,context);fn_802C3034(context);int flags=0;int color=lbl_8055FBDC;
 int index;for(int row=0;row<fields->rows&&(index=row+start,fn_8022D3F8(fields->list,index,&flags)!=0);++row){
  int selected=*fields->selected==index;
  if(selected){int x=self->left,y=self->top;int width=((WidgetDispatch*)self)->Slot78();
   fn_802C3424(x+2,y+row*fields->rowHeight+2,x+width-2,y+(row+1)*fields->rowHeight+2,color);
  }
  int y=self->top,x=self->left;
  fn_8022B91C(7,x+2,y+row*fields->rowHeight+2,fn_8022D3F8(fields->list,index,0),(selected!=0)|((flags!=0)?2:0));
 }
}
void fn_8021E51C(WidgetFragment* self){
 RowListPrefix* fields=(RowListPrefix*)self;int start=0;if(fields->view)start=fields->view->start;
 int flags=0;
 int index;for(int row=0;row<fields->rows&&(index=row+start,fn_8022D3F8(fields->list,index,&flags)!=0);++row){
  int selected=*fields->selected==index;int y=self->top,x=self->left;
  fn_8022BAD0(7,x+2,y+row*fields->rowHeight+2,0,0,0,fn_8022D3F8(fields->list,index,0),(selected!=0)|((flags!=0)?2:0),0,lbl_805634A4);
 }
}
}

struct RowInputDispatch {virtual ~RowInputDispatch();virtual void Slot0c();virtual void Slot10();virtual void Slot14();virtual void Slot18();virtual void Slot1c(int,int);virtual void Slot20();virtual void Slot24();virtual void Slot28();virtual void Slot2c();virtual void Slot30();virtual void Slot34();virtual void Slot38();virtual void Slot3c();virtual void Slot40();virtual void Slot44();virtual void Slot48();virtual void Slot4c(int,int);virtual void Slot50();virtual void Slot54();virtual void Slot58();virtual void Slot5c();virtual void Slot60();virtual void Slot64();virtual void Slot68();virtual void Slot6c();virtual void Slot70();virtual void Slot74(int,void*);virtual int Slot78();virtual void Slot7c();virtual void Slot80();virtual int Slot84();virtual const char* Slot88();};

extern "C" {
void fn_80229DC8(WidgetFragment* self,int x,int y){
 RowListPrefix* fields=(RowListPrefix*)self;fields->direction=0;fields->active=1;((RowInputDispatch*)self)->Slot4c(x,y);
}
void fn_80229DE8(WidgetFragment* self,int x,int y){
 int height=self->bottom-self->top;RowListPrefix* fields=(RowListPrefix*)self;fields->direction=0;fields->active=1;((RowInputDispatch*)self)->Slot4c(x,y-height-3);
}
void fn_80229E1C(WidgetFragment* self,int x,int y){fn_8021E1C8(self,x,y-(self->bottom-self->top)-3);}
void fn_80229E34(WidgetFragment* self){RowListPrefix* fields=(RowListPrefix*)self;SetListViewport(fields->view,fields->view->start+1);}
void fn_80229E74(WidgetFragment* self){RowListPrefix* fields=(RowListPrefix*)self;SetListViewport(fields->view,fields->view->start-1);}
void fn_80229EB4(WidgetFragment* self){RowListPrefix* fields=(RowListPrefix*)self;fields->active=0;fields->direction=0;}
int fn_80229EC4(WidgetFragment* self){return 1;}
int fn_80229ECC(WidgetFragment* self){return 1;}
int fn_80228F40(int x,int y){return x+1000*y;}
}

extern "C" {
WidgetFragment* fn_80223208(WidgetFragment*,int);
}
static inline void ObservedNullable_fn_8021915C(WidgetFragment* self){if(self)fn_8021915C(self,0);}
static inline void ObservedNullable_fn_802223C4(WidgetFragment* self){if(self)fn_802223C4(self,0);}
extern "C" {
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

// Placeholders for the original functions not yet implemented above (generated by Claude for
// the provisional code-only widgets unit). Empty bodies; they are not decompilations.
void fn_80219AB4(WidgetFragment*,int,const char*,int,int,int,bool) {}  // placeholder (912 bytes)
void fn_80219E44(void) {}  // placeholder (208 bytes)
void fn_80219F14(void) {}  // placeholder (204 bytes)
void fn_80219FE0(void) {}  // placeholder (1496 bytes)
void fn_8021AA18(void) {}  // placeholder (368 bytes)
void fn_8021AB88(void) {}  // placeholder (8 bytes)
void fn_8021AB90(void) {}  // placeholder (148 bytes)
void fn_8021B32C(void) {}  // placeholder (116 bytes)
void fn_8021B3BC(void) {}  // placeholder (72 bytes)
void fn_8021B460(void) {}  // placeholder (1100 bytes)
void fn_8021B8AC(void) {}  // placeholder (152 bytes)
void fn_8021B944(void) {}  // placeholder (104 bytes)
void fn_8021B9AC(void) {}  // placeholder (184 bytes)
void fn_8021BA64(void) {}  // placeholder (316 bytes)
void fn_8021BBA0(void) {}  // placeholder (128 bytes)
void fn_8021BC20(void) {}  // placeholder (296 bytes)
void fn_8021BD48(void) {}  // placeholder (80 bytes)
void fn_8021BD98(void) {}  // placeholder (176 bytes)
void fn_8021BE48(void) {}  // placeholder (116 bytes)
void fn_8021BEBC(void) {}  // placeholder (4 bytes)
void fn_8021BEC0(void) {}  // placeholder (208 bytes)
void fn_8021BF90(void) {}  // placeholder (92 bytes)
void fn_8021BFEC(void) {}  // placeholder (316 bytes)
void fn_8021C128(void) {}  // placeholder (4 bytes)
void fn_8021C12C(void) {}  // placeholder (96 bytes)
void fn_8021C18C(void) {}  // placeholder (396 bytes)
void fn_8021C318(void) {}  // placeholder (44 bytes)
void fn_8021C344(void) {}  // placeholder (308 bytes)
void fn_8021C478(void) {}  // placeholder (92 bytes)
void fn_8021C4D4(void) {}  // placeholder (504 bytes)
void fn_8021C6CC(void) {}  // placeholder (4 bytes)
void fn_8021C6D0(void) {}  // placeholder (96 bytes)
void fn_8021C730(void) {}  // placeholder (580 bytes)
void fn_8021C974(void) {}  // placeholder (8 bytes)
void fn_8021C97C(void) {}  // placeholder (92 bytes)
void fn_8021C9D8(void) {}  // placeholder (52 bytes)
void fn_8021CB4C(void) {}  // placeholder (112 bytes)
void fn_8021CBBC(void) {}  // placeholder (328 bytes)
void fn_8021DB58(void) {}  // placeholder (1384 bytes)
void fn_8021E620(void) {}  // placeholder (436 bytes)
void fn_8021E8DC(void) {}  // placeholder (392 bytes)
void fn_8021EA64(void) {}  // placeholder (312 bytes)
void fn_8021EB9C(void) {}  // placeholder (128 bytes)
void fn_8021EC1C(void) {}  // placeholder (392 bytes)
void fn_8021EDA4(void) {}  // placeholder (572 bytes)
void fn_8021EFE0(void) {}  // placeholder (100 bytes)
void fn_8021F044(void) {}  // placeholder (40 bytes)
void fn_8021F06C(void) {}  // placeholder (808 bytes)
void fn_8021F394(void) {}  // placeholder (152 bytes)
void fn_8021F42C(void) {}  // placeholder (76 bytes)
void fn_8021F478(void) {}  // placeholder (120 bytes)
void fn_8021F4F0(void) {}  // placeholder (392 bytes)
void fn_8021F678(void) {}  // placeholder (372 bytes)
void fn_8021F7EC(void) {}  // placeholder (92 bytes)
void fn_8021F848(void) {}  // placeholder (592 bytes)
void fn_8021FA98(void) {}  // placeholder (300 bytes)
void fn_8021FBC4(void) {}  // placeholder (124 bytes)
void fn_8021FC40(void) {}  // placeholder (296 bytes)
void fn_8021FD68(void) {}  // placeholder (460 bytes)
void fn_8021FF34(void) {}  // placeholder (264 bytes)
void fn_8022003C(void) {}  // placeholder (112 bytes)
void fn_802200AC(void) {}  // placeholder (1280 bytes)
void fn_802205AC(void) {}  // placeholder (368 bytes)
void fn_8022071C(void) {}  // placeholder (92 bytes)
void fn_80220778(void) {}  // placeholder (252 bytes)
void fn_80220874(void) {}  // placeholder (120 bytes)
void fn_802208EC(void) {}  // placeholder (92 bytes)
void fn_80220948(void) {}  // placeholder (264 bytes)
void fn_80220A50(void) {}  // placeholder (172 bytes)
void fn_80220AFC(void) {}  // placeholder (64 bytes)
void fn_80220B3C(void) {}  // placeholder (32 bytes)
void fn_80220B5C(void) {}  // placeholder (248 bytes)
void fn_80220C54(void) {}  // placeholder (436 bytes)
void fn_80220E08(void) {}  // placeholder (64 bytes)
void fn_80220E48(void) {}  // placeholder (328 bytes)
void fn_80220F90(void) {}  // placeholder (1044 bytes)
void fn_802213A4(void) {}  // placeholder (404 bytes)
void fn_80221538(void) {}  // placeholder (12 bytes)
void fn_80221544(void) {}  // placeholder (1068 bytes)
void fn_80221970(void) {}  // placeholder (1292 bytes)
void fn_80221E7C(void) {}  // placeholder (36 bytes)
void fn_80221EA0(void) {}  // placeholder (8 bytes)
void fn_80221EA8(void) {}  // placeholder (236 bytes)
void Finish__9MenuStateFi(void) {}  // placeholder (8 bytes; original fn_80221F94, named MenuState::Finish in this tree)
void fn_80221F9C(void) {}  // placeholder (388 bytes)
void fn_80222120(void) {}  // placeholder (88 bytes)
void fn_80222178(void) {}  // placeholder (188 bytes)
void fn_80222234(void) {}  // placeholder (268 bytes)
void fn_80222BDC(void) {}  // placeholder (176 bytes)
void fn_80222C8C(void) {}  // placeholder (112 bytes)
void fn_80222CFC(void) {}  // placeholder (936 bytes)
void fn_802230A4(void) {}  // placeholder (356 bytes)
WidgetFragment* fn_80223208(WidgetFragment*,int) { return 0; }  // placeholder (120 bytes)
void fn_80223280(void) {}  // placeholder (820 bytes)
void fn_802235B4(void) {}  // placeholder (144 bytes)
void fn_80223644(void) {}  // placeholder (132 bytes)
void fn_802236C8(void) {}  // placeholder (96 bytes)
void fn_80223728(void) {}  // placeholder (52 bytes)
void fn_8022375C(void) {}  // placeholder (4 bytes)
void fn_80223760(void) {}  // placeholder (112 bytes)
void fn_802237D0(void) {}  // placeholder (12 bytes)
void fn_802237DC(void) {}  // placeholder (12 bytes)
void fn_802237E8(void) {}  // placeholder (8 bytes)
void fn_802237F0(void) {}  // placeholder (16 bytes)
void fn_80223800(void) {}  // placeholder (128 bytes)
void fn_80223880(void) {}  // placeholder (80 bytes)
void fn_802238D0(void) {}  // placeholder (248 bytes)
void fn_802239C8(void) {}  // placeholder (144 bytes)
void fn_80223A58(void) {}  // placeholder (84 bytes)
void fn_80223AAC(void) {}  // placeholder (8 bytes)
void fn_80223AB4(void) {}  // placeholder (224 bytes)
void fn_80223B94(void) {}  // placeholder (140 bytes)
void fn_80223C20(void) {}  // placeholder (164 bytes)
void fn_80223CC4(void) {}  // placeholder (44 bytes)
void fn_80223CF0(void) {}  // placeholder (700 bytes)
void fn_80223FAC(void) {}  // placeholder (124 bytes)
void fn_80224028(void) {}  // placeholder (312 bytes)
void fn_80224160(void) {}  // placeholder (60 bytes)
void fn_8022419C(void) {}  // placeholder (44 bytes)
void fn_802241C8(void) {}  // placeholder (220 bytes)
void fn_802242A4(void) {}  // placeholder (132 bytes)
void fn_80224328(void) {}  // placeholder (52 bytes)
void fn_8022435C(void) {}  // placeholder (364 bytes)
void fn_802244C8(void) {}  // placeholder (148 bytes)
void fn_8022455C(void) {}  // placeholder (108 bytes)
void fn_802245C8(void) {}  // placeholder (668 bytes)
void fn_80224864(void) {}  // placeholder (152 bytes)
void fn_802248FC(void) {}  // placeholder (316 bytes)
void fn_80224A38(void) {}  // placeholder (212 bytes)
void fn_80224B0C(void) {}  // placeholder (72 bytes)
void fn_80224B54(void) {}  // placeholder (492 bytes)
void fn_80224D40(void) {}  // placeholder (156 bytes)
void fn_80224DDC(void) {}  // placeholder (152 bytes)
void fn_80224E74(void) {}  // placeholder (1736 bytes)
void fn_8022553C(void) {}  // placeholder (748 bytes)
void fn_80225828(void) {}  // placeholder (160 bytes)
void fn_802258C8(void) {}  // placeholder (80 bytes)
void fn_80225918(void) {}  // placeholder (4 bytes)
void fn_8022591C(void) {}  // placeholder (592 bytes)
void fn_80225B6C(void) {}  // placeholder (2144 bytes)
void fn_802263CC(void) {}  // placeholder (348 bytes)
void fn_80226528(void) {}  // placeholder (72 bytes)
void fn_80226570(void) {}  // placeholder (192 bytes)
void fn_80226630(void) {}  // placeholder (128 bytes)
void fn_802266B0(void) {}  // placeholder (88 bytes)
void fn_80226708(void) {}  // placeholder (80 bytes)
void fn_80226758(void) {}  // placeholder (16 bytes)
void fn_80226768(void) {}  // placeholder (92 bytes)
void fn_802267C4(void) {}  // placeholder (220 bytes)
void fn_802268A0(void) {}  // placeholder (72 bytes)
void fn_802268E8(void) {}  // placeholder (192 bytes)
void fn_802269A8(void) {}  // placeholder (296 bytes)
void fn_80226AD0(void) {}  // placeholder (156 bytes)
void fn_80226B6C(void) {}  // placeholder (336 bytes)
void fn_80226CBC(void) {}  // placeholder (580 bytes)
void fn_80226F00(void) {}  // placeholder (148 bytes)
void fn_80226F94(void) {}  // placeholder (176 bytes)
void fn_80227044(void) {}  // placeholder (152 bytes)
void fn_802270DC(void) {}  // placeholder (360 bytes)
void fn_80227244(void) {}  // placeholder (144 bytes)
void fn_802272D4(void) {}  // placeholder (1452 bytes)
void fn_80227880(void) {}  // placeholder (112 bytes)
void fn_802278F0(void) {}  // placeholder (940 bytes)
void fn_80227C9C(void) {}  // placeholder (100 bytes)
void fn_80227D00(void) {}  // placeholder (100 bytes)
void fn_80227D64(void) {}  // placeholder (132 bytes)
void fn_80227DE8(void) {}  // placeholder (156 bytes)
void fn_80227E84(void) {}  // placeholder (216 bytes)
void fn_80227F5C(void) {}  // placeholder (180 bytes)
void fn_80228010(void) {}  // placeholder (1032 bytes)
void fn_802286E4(void) {}  // placeholder (328 bytes)
void fn_8022882C(void) {}  // placeholder (64 bytes)
void fn_8022886C(void) {}  // placeholder (116 bytes)
void fn_802288E0(void) {}  // placeholder (156 bytes)
void fn_8022897C(void) {}  // placeholder (20 bytes)
void fn_80228990(void) {}  // placeholder (4 bytes)
void fn_80228994(void) {}  // placeholder (164 bytes)
void fn_80228A38(void) {}  // placeholder (52 bytes)
void fn_80228A6C(void) {}  // placeholder (20 bytes)
void fn_80228A80(void) {}  // placeholder (4 bytes)
void fn_80228A84(void) {}  // placeholder (104 bytes)
void fn_80228AEC(void) {}  // placeholder (676 bytes)
void fn_80228D90(void) {}  // placeholder (20 bytes)
void fn_80228DA4(void) {}  // placeholder (8 bytes)
void fn_80228DAC(void) {}  // placeholder (20 bytes)
void fn_80228DC0(void) {}  // placeholder (4 bytes)
void fn_80228DC4(void) {}  // placeholder (188 bytes)
void fn_80228E80(void) {}  // placeholder (192 bytes)
void fn_80229604(void) {}  // placeholder (12 bytes)
void fn_802298F8(void) {}  // placeholder (4 bytes)
void fn_802298FC(void) {}  // placeholder (4 bytes)
void fn_80229900(void) {}  // placeholder (24 bytes)
void fn_80229918(void) {}  // placeholder (64 bytes)
void fn_80229958(void) {}  // placeholder (64 bytes)
void fn_80229998(void) {}  // placeholder (24 bytes)
void fn_802299B0(void) {}  // placeholder (76 bytes)
void fn_802299FC(void) {}  // placeholder (152 bytes)
void fn_80229A94(void) {}  // placeholder (48 bytes)
void fn_80229AC4(void) {}  // placeholder (24 bytes)
void fn_80229ADC(void) {}  // placeholder (8 bytes)
void fn_80229AE4(void) {}  // placeholder (40 bytes)
void fn_80229B0C(void) {}  // placeholder (92 bytes)
void fn_80229B68(void) {}  // placeholder (8 bytes)
void fn_80229B70(void) {}  // placeholder (8 bytes)
void fn_80229B78(void) {}  // placeholder (112 bytes)
void fn_80229BE8(void) {}  // placeholder (180 bytes)
void fn_80229C9C(void) {}  // placeholder (8 bytes)
void fn_80229CA4(void) {}  // placeholder (8 bytes)
void fn_80229CAC(void) {}  // placeholder (8 bytes)
void fn_80229CB4(void) {}  // placeholder (36 bytes)
void fn_80229CD8(void) {}  // placeholder (32 bytes)
void fn_80229CF8(void) {}  // placeholder (36 bytes)
void fn_80229D1C(void) {}  // placeholder (32 bytes)
void fn_80229D3C(void) {}  // placeholder (32 bytes)
void fn_80229D5C(void) {}  // placeholder (8 bytes)
void fn_80229D64(void) {}  // placeholder (92 bytes)
void fn_80229DC0(void) {}  // placeholder (8 bytes)
void fn_80229ED4(void) {}  // placeholder (32 bytes)
void fn_80229EF4(void) {}  // placeholder (32 bytes)
void fn_80229F14(void) {}  // placeholder (8 bytes)
void fn_80229F1C(void) {}  // placeholder (28 bytes)
void fn_80229F38(void) {}  // placeholder (32 bytes)
void fn_80229F58(void) {}  // placeholder (8 bytes)
void fn_80229F60(void) {}  // placeholder (24 bytes)
void fn_80229F78(void) {}  // placeholder (120 bytes)
void fn_80229FF0(void) {}  // placeholder (24 bytes)
void fn_8022A008(void) {}  // placeholder (12 bytes)
void fn_8022A014(void) {}  // placeholder (32 bytes)
void fn_8022A034(void) {}  // placeholder (32 bytes)
void fn_8022A054(void) {}  // placeholder (120 bytes)
void fn_8022A0CC(void) {}  // placeholder (88 bytes)
void fn_8022A124(void) {}  // placeholder (120 bytes)
void fn_8022A19C(void) {}  // placeholder (116 bytes)
void fn_8022A210(void) {}  // placeholder (868 bytes)
void fn_8022A574(void) {}  // placeholder (32 bytes)
void fn_8022A594(void) {}  // placeholder (80 bytes)
}
