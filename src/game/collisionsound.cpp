// Unregistered draft: all 27 bodies implemented; code residuals and data extent/ownership limits remain.
typedef unsigned char u8;
typedef unsigned int u32;
class ModuleTag;void RegisterModuleTag(ModuleTag*);
class ModuleTag {public:ModuleTag(){RegisterModuleTag(this);}u8 flag;ModuleTag* next;};

typedef unsigned long size_t;
struct OwnedSound { virtual ~OwnedSound(); };
struct Channel {virtual ~Channel();virtual void Slot0c();virtual void Slot10();virtual int Start(const char*);virtual void Slot18();virtual void Slot1c();virtual void Stop();virtual int Status();};
struct SoundHandle: OwnedSound {u32 unknown4;float gain;Channel* channel;char pad[0x39];u8 playing,paused,stopped,unknown4c,dirty;};
struct Vec3 {float x,y,z;Vec3(float a,float b,float c):x(a),y(b),z(c){}};
struct Body { char pad[0x10]; u32 tag; char pad14[0x6c-0x14]; short material; char pad6e[0x1ec-0x6e]; int id; };
struct Contact { void* vtable; Body* body; char pad[0x18]; int kind; };
struct Message { u32 words[14]; };
struct Entry { void* vtable; float a,b,c,d,e,f; void* sound; u32 key1,key2; float time; };
struct Pool { void* items; int count,index; };
struct Node { u32 key; Pool* pool; Node* left; Node* right; };
struct Manager { Pool pools[6]; Node nodes[43]; Node* root; Entry entries[16]; int next; };
typedef char EntrySize[(sizeof(Entry)==0x2c)?1:-1];
typedef char ManagerSize[(sizeof(Manager)==0x5c0)?1:-1];
struct SoundInfo { const char* name; int pool; float gain; };
struct SoundTuple { const char* name; float gain; };
struct KeyInfo { int pool,first,second; };
// The sound interface (declared like mixer.cpp). Only its shared `return 0` slot 0x14 is defined in
// this unit, at its original place below.
class Sound {
public:
    Sound(void);
    virtual ~Sound(void);
    virtual void vf0C(void);
    virtual int vf10(void) = 0;
    virtual int vf14(void);
    virtual void Play(void) = 0;
    virtual void Start(void) = 0;
    virtual void Stop(void) = 0;
    virtual int State(void) = 0;
    virtual void Pause(void) = 0;
    virtual void Resume(void) = 0;
    virtual void vf30(void) = 0;
    virtual void vf34(void) = 0;
    virtual int vf38(void) = 0;
    virtual void vf3C(void) = 0;
    virtual void vf40(void) = 0;
    virtual void vf44(float v);
    virtual void vf48(float v);
    virtual void vf4C(float v);
    virtual void vf50(u32 mask);
    virtual void vf54(void) = 0;
    virtual void vf58(void) = 0;
    virtual void vf5C(void) = 0;
    virtual float vf60(void);
};

extern "C" {
extern const SoundInfo lbl_802F9630[13];
extern const KeyInfo lbl_802F96CC[43];
extern char lbl_80335D50[],lbl_80335D74[];
extern float lbl_8055E3C8;extern int lbl_8055E3C4;
extern const float lbl_80562624,lbl_80562630,lbl_8056264C,lbl_80562650;
void* __nwa__FUlPCc(unsigned long,const char*);
void __dla__FPv(void*);
void __dl__FPv(void*);
void __destroy_arr(void*,void*,int,int);
void* fn_802077A4(const char*,int,void*,void*);
void qsort(void*,unsigned long,unsigned long,int(*)(const void*,const void*));
extern char lbl_80335D3C[],lbl_80335D80[];extern int lbl_80435C50[8];
extern u32 lbl_80335DAC[];
extern Manager* lbl_8055E6C0;
void* __nw__FUlPCc(unsigned long,const char*);
void* memset(void*,int,size_t);
void fn_801796CC(int,const Message*,int);
void fn_801B5364(int,void*);
void fn_801B537C(void*);
void fn_802E317C(const char*,...);
void __construct_array(void*,void*,void*,int,int);
extern ModuleTag lbl_8055E6B8;
Manager* fn_801B3B70(Manager*);
void* fn_801B3BD4(Entry*,int);
Entry* fn_801B3C64(Entry*);
Manager* fn_801B3D24(Manager*,int);
void fn_801B3DBC(Manager*);
void fn_801B3FD8(Manager*);
void fn_801B4204(Manager*);
void fn_801B42D0(Manager*);
void fn_801B435C(Manager*);
Node* fn_801B4528(Manager*,Node*,u32);


struct V3 {float x,y,z;};
struct CollisionMessage {V3 velocity,position,vectorA,vectorB;int car,kind;CollisionMessage(){}CollisionMessage(const CollisionMessage& other):velocity(other.velocity),position(other.position),vectorA(other.vectorA),vectorB(other.vectorB),car(other.car),kind(other.kind){}};
struct SurfaceMessage {V3 velocity,position;short material,padding;int car;};
struct EffectMessage {V3 position,velocity,vector;float impact;int car,kind;};
struct SoundMessage {int type;V3 position;float amplitude;SoundMessage(int code,V3 p,float gain):type(code),position(p),amplitude(gain){}};
typedef char CollisionMessageSize[(sizeof(CollisionMessage)==0x38)?1:-1];
typedef char SurfaceMessageSize[(sizeof(SurfaceMessage)==0x20)?1:-1];
typedef char EffectMessageSize[(sizeof(EffectMessage)==0x30)?1:-1];
typedef char SoundMessageSize[(sizeof(SoundMessage)==0x14)?1:-1];
extern const float lbl_80562620,lbl_80562628,lbl_8056262C,lbl_80562634,lbl_80562638,lbl_8056263C,lbl_80562640,lbl_80562644,lbl_80562648;
void VelAt__9RigidBodyFP5RVec3PC5RVec3(Body*,V3*,const V3*);
float PSVECDotProduct(const V3*,const V3*);
float PSVECMag(const V3*);
void fn_801B5360(int,const void*,int);
void fn_801B85E0(Body*,Body*,const V3*,const V3*);
void fn_801B3084(Body*,Body*,const V3*,const V3*);
inline void ScaleVInto(const V3& p,float f,V3& result){result.x=p.x*f;result.y=p.y*f;result.z=p.z*f;}


inline bool CollisionAbove(float value,float threshold){return value>threshold;}
inline float CollisionMin(float limit,float value){if(limit<value)return limit;return value;}
int fn_801B32C8(Contact* self,int kind){int result=0;int kinds[2];kinds[0]=self->kind;int selected=-1;kinds[1]=kind;if(kinds[0]==109)selected=0;else if(kinds[1]==109)selected=1; if(selected!=-1)switch(kinds[!selected]){case 1:case 110:result=1;break;}return result;}
Body* fn_801B3338(Contact* self) {
 Body* p=self->body;
 if(p){bool car=false;u32 tag=p->tag;
 if(tag==0x43415220||tag==0x50434152||tag==0x41434152||tag==0x4e434152)car=true;
 if(car)return p;}
 return 0;
}
int fn_801B3390(){return 0x14;}
int fn_801B3398(const Message* input){Message message=*input;fn_801796CC(1,&message,0x38);return 0x38;}
void fn_801B3448(){Manager* p=(Manager*)__nw__FUlPCc(0x5c0,lbl_80335D3C);if(p)p=fn_801B3B70(p);lbl_8055E6C0=p;fn_801B5364(1,(void*)fn_801B3390);fn_801B5364(15,(void*)fn_801B3398);}
void fn_801B34A4(){fn_801B3DBC(lbl_8055E6C0);memset(lbl_80435C50,0,0x20);}
void fn_801B3B24(){}
void fn_801B3B28(){fn_801B537C((void*)fn_801B3390);fn_801B537C((void*)fn_801B3398);fn_801B3D24(lbl_8055E6C0,1);lbl_8055E6C0=0;}
Manager* fn_801B3B70(Manager* self){__construct_array(self->entries,(void*)fn_801B3C64,(void*)fn_801B3BD4,0x2c,16);fn_801B3FD8(self);fn_801B4204(self);fn_801B42D0(self);return self;}
Entry* fn_801B3C64(Entry* self){self->vtable=lbl_80335DAC;self->a=lbl_80562624;self->b=lbl_80562624;self->c=lbl_80562624;self->d=lbl_80562624;self->e=lbl_80562624;self->f=lbl_80562624;self->sound=0;return self;}
void fn_801B3DBC(Manager* self){fn_801B435C(self);}
}  // extern "C"
int Sound::vf14(void) { return 0; }
extern "C" {
int fn_801B4428(const Node* a,const Node* b){if(a->key<b->key)return -1;if(a->key>b->key)return 1;fn_802E317C(lbl_80335D80);return 0;}
Node* fn_801B447C(Manager* self,Node* nodes,int count){if(count==1)return nodes;int middle=count/2;if(middle>0)nodes[middle].left=fn_801B447C(self,nodes,middle);int remaining=count-middle-1;if(remaining>0)nodes[middle].right=fn_801B447C(self,nodes+middle+1,remaining);return nodes+middle;}
Node* fn_801B4528(Manager* self,Node* node,u32 key){if(key<node->key){if(node->left)return fn_801B4528(self,node->left,key);return 0;}if(key>node->key){if(node->right)return fn_801B4528(self,node->right,key);return 0;}return node;}
void* fn_801B4570(Manager* self,u32 key){Pool* pool=(Pool*)self;Node* node=fn_801B4528(self,self->root,key);if(node)pool=node->pool;SoundTuple* result=(SoundTuple*)pool->items+pool->index;pool->index=(pool->index+1)%pool->count;return result;}


Manager* fn_801B3D24(Manager* self,int destroy){if(self){for(int i=0;i<6;i++)__dla__FPv(self->pools[i].items);__destroy_arr(self->entries,(void*)fn_801B3BD4,0x2c,16);if(destroy>0)__dl__FPv(self);}return self;}
int fn_801B3DC0(Manager* self,u32 first,u32 second){float cutoff=lbl_8055E3C8-lbl_80562630;int result=0;Entry* entry=self->entries;int i=0;while(!result&&i<16){result=0;if(entry->key1==first&&entry->key2==second&&entry->time>cutoff)result=1;entry++;i++;}return result;}
void fn_801B3FD8(Manager* self){int at=0;for(int pool=0;pool<6;pool++){int first=at;while(at<13&&lbl_802F9630[at].pool==pool)at++;int count=at-first;if(count<1)fn_802E317C(lbl_80335D50,pool);self->pools[pool].items=__nwa__FUlPCc(count*8,lbl_80335D74);self->pools[pool].count=count;self->pools[pool].index=0;for(int i=0;i<count;i++){((SoundTuple*)self->pools[pool].items)[i].name=lbl_802F9630[first+i].name;((SoundTuple*)self->pools[pool].items)[i].gain=lbl_802F9630[first+i].gain;}}}
inline u32 PairKey(int first,int second){if(first<0||second<0)return (u32)-1;u32 key=(second<<16)|(first&0xffff);if(first<second)key=(first<<16)|(second&0xffff);return key;}
void fn_801B4204(Manager* self){for(int i=0;i<43;i++){self->nodes[i].key=PairKey(lbl_802F96CC[i].first,lbl_802F96CC[i].second);self->nodes[i].pool=self->pools+lbl_802F96CC[i].pool;self->nodes[i].left=0;self->nodes[i].right=0;}qsort(self->nodes,43,16,(int(*)(const void*,const void*))fn_801B4428);self->root=fn_801B447C(self,self->nodes,43);}
void fn_801B42D0(Manager* self){const char* name=lbl_802F9630[0].name;for(int i=0;i<16;i++){Entry* entry=self->entries+i;entry->sound=fn_802077A4(name,5,&entry->d,&entry->a);entry->key1=0;entry->key2=0;entry->time=lbl_80562650;}self->next=0;}
void fn_801B435C(Manager* self){for(int i=0;i<16;i++){self->entries[i].key1=0;self->entries[i].key2=0;self->entries[i].time=lbl_80562650;}}

inline void EntryCleanup(Entry* self){if(self){self->vtable=lbl_80335DAC;if(self->sound){delete (OwnedSound*)self->sound;self->sound=0;}}}
void* fn_801B3C98(Entry* self,int destroy){if(self){self->vtable=lbl_80335DAC;if(self->sound){delete (OwnedSound*)self->sound;self->sound=0;}if(destroy>0)__dl__FPv(self);}return self;}
void* fn_801B3BD4(Entry* self,int destroy){if(self){EntryCleanup(self);if(destroy>0)__dl__FPv(self);}return self;}

void fn_801B3E24(Manager* self,const Vec3* position,Contact* first,Contact* second,int secondKind,float amplitude){
 u32 key=PairKey(first->kind,secondKind);
 SoundTuple* choice=(SoundTuple*)fn_801B4570(self,key);
 Entry* entry=self->entries+self->next;
 const char* name=choice->name;
 Channel* channel=((SoundHandle*)entry->sound)->channel;
 if(channel->Status()!=3)channel->Stop();
 if(channel->Start(name)){
  entry->key1=(u32)first;entry->key2=(u32)second;entry->time=lbl_8055E3C8;
  float gain=CollisionMin(lbl_8056264C,amplitude*choice->gain);
  entry->d=lbl_80562624;entry->e=lbl_80562624;entry->f=lbl_80562624;
  entry->a=position->x;entry->b=position->y;entry->c=position->z;
  SoundHandle* sound=(SoundHandle*)entry->sound;volatile V3 zero={lbl_80562624,lbl_80562624,lbl_80562624};
  if(sound){if(gain!=sound->gain){sound->dirty=1;sound->gain=gain;}sound=(SoundHandle*)entry->sound;sound->playing=1;sound->stopped=0;sound->paused=0;}
  self->next=(self->next+1)%16;
 }
}

void fn_801B34DC(Contact* first,Contact* second,const V3* position,const V3* inputA,const V3* inputB,int kind){
 V3 vectorA=*inputA;V3 vectorB=*inputB;
 if(!first->body){Contact* temp=first;first=second;second=temp;ScaleVInto(vectorA,lbl_80562620,vectorA);ScaleVInto(vectorB,lbl_80562620,vectorB);if(second)kind=second->kind;}
 V3 firstVelocity,relative,secondVelocity;
 VelAt__9RigidBodyFP5RVec3PC5RVec3(first->body,&firstVelocity,position);
 if(second&&second->body)VelAt__9RigidBodyFP5RVec3PC5RVec3(second->body,&secondVelocity,position);
 else{secondVelocity.x=lbl_80562624;secondVelocity.y=lbl_80562624;secondVelocity.z=lbl_80562624;}
 relative.x=firstVelocity.x-secondVelocity.x;relative.y=firstVelocity.y-secondVelocity.y;relative.z=firstVelocity.z-secondVelocity.z;
 float impact=PSVECDotProduct(&vectorB,&vectorA);
 bool allowed=true;int special=0;u32 offset=(u32)kind-107u;if(offset<=3u){if((1u<<offset)&13u)special=1;}if(!special&&kind>=8)allowed=false;
 Body* firstCar=fn_801B3338(first);
 if(firstCar&&allowed&&impact*impact>lbl_80562628&&PSVECDotProduct(&relative,&relative)>lbl_8056262C){
  Body* secondCar=second?fn_801B3338(second):0;
  CollisionMessage message;
float ax=firstVelocity.x*lbl_80562630;float ay=firstVelocity.y*lbl_80562630;float az=firstVelocity.z*lbl_80562630;float bx=secondVelocity.x*lbl_80562630;float by=secondVelocity.y*lbl_80562630;float bz=secondVelocity.z*lbl_80562630;float sumx=ax+bx;float sumy=ay+by;float sumz=az+bz;message.velocity.x=sumx;message.velocity.y=sumy;message.velocity.z=sumz;
  message.vectorA=vectorA;message.position=*position;message.vectorB=vectorB;message.kind=kind;message.car=firstCar->id;
  fn_801796CC(1,(const Message*)&message,0x38);
  CollisionMessage queued=message;fn_801B5360(15,&queued,0x38);
  if(secondCar){ScaleVInto(message.velocity,lbl_80562620,message.velocity);message.car=secondCar->id;fn_801796CC(1,(const Message*)&message,0x38);CollisionMessage queuedOther=message;fn_801B5360(15,&queuedOther,0x38);}
 }
 if(PSVECDotProduct(&firstVelocity,&firstVelocity)>lbl_80562634&&fn_801B32C8(first,kind)){
  SurfaceMessage surface;surface.position=*position;surface.velocity=firstVelocity;surface.material=first->body->material;surface.car=firstCar?firstCar->id:-1;fn_801796CC(0,(const Message*)&surface,0x20);
 }
 if(firstCar&&kind<8&&lbl_80435C50[firstCar->id]<lbl_8055E3C4){
  EffectMessage effect;effect.position=*position;effect.velocity=firstVelocity;effect.vector=vectorB;effect.impact=impact;effect.car=firstCar->id;effect.kind=kind;fn_801796CC(5,(const Message*)&effect,0x30);lbl_80435C50[firstCar->id]=lbl_8055E3C4;
 }
 if(firstCar&&kind==109){Body* other=fn_801B3338(second);fn_801B85E0(firstCar,other,position,&vectorA);fn_801B3084(firstCar,other,position,&vectorA);}
 V3 magnitudeVector;if(kind==110)ScaleVInto(vectorA,lbl_80562638,magnitudeVector);else magnitudeVector=vectorA;
 if(!fn_801B3DC0(lbl_8055E6C0,(u32)first,(u32)second)&&CollisionAbove(PSVECDotProduct(&magnitudeVector,&magnitudeVector),lbl_8056263C)){
  float magnitude=PSVECMag(&magnitudeVector);float capped=CollisionMin(lbl_80562640,magnitude);
  float amplitude=lbl_80562644+lbl_80562648*(capped/lbl_80562640);
  SoundMessage message(1234,*position,amplitude);fn_801B5360(1,&message,0x14);
  fn_801B3E24(lbl_8055E6C0,(const Vec3*)position,first,second,kind,amplitude);
 }
}
}

// Research data fragment; exact original tables, proposed unit ownership only.
extern "C" {
char lbl_80335C80[]="crash1.sfx";
char lbl_80335C8C[]="crash2.sfx";
char lbl_80335C98[]="crash3.sfx";
char lbl_80335CA4[]="crash4.sfx";
char lbl_80335CB0[]="car_gnd1.sfx";
char lbl_80335CC0[]="car_gnd2.sfx";
char lbl_80335CD0[]="cone.sfx";
char lbl_80335CDC[]="wdcrash1.sfx";
char lbl_80335CEC[]="wdcrash2.sfx";
char lbl_80335CFC[]="wdcrash3.sfx";
char lbl_80335D0C[]="mtlcrsh1.sfx";
char lbl_80335D1C[]="mtlcrsh2.sfx";
char lbl_80335D2C[]="haycrash.sfx";
char lbl_80335D3C[]="CollisionSoundMan";
char lbl_80335D50[]="collide: sound pool %d is empty!";
char lbl_80335D74[]="sound pool";
char lbl_80335D80[]="collide: tree nodes can't have the same key";
extern const SoundInfo lbl_802F9630[13]={
 {lbl_80335C80,0,1.0f},
 {lbl_80335C8C,0,1.0f},
 {lbl_80335C98,0,1.0f},
 {lbl_80335CA4,0,1.0f},
 {lbl_80335CB0,1,1.0f},
 {lbl_80335CC0,1,1.0f},
 {lbl_80335CD0,2,1.25f},
 {lbl_80335CDC,3,1.399999976158142f},
 {lbl_80335CEC,3,1.399999976158142f},
 {lbl_80335CFC,3,1.399999976158142f},
 {lbl_80335D0C,4,2.0999999046325684f},
 {lbl_80335D1C,4,2.0999999046325684f},
 {lbl_80335D2C,5,1.2999999523162842f}
};
extern const KeyInfo lbl_802F96CC[43]={
 {1,109,0},
 {1,109,1},
 {2,107,109},
 {2,107,110},
 {2,107,107},
 {2,107,100},
 {2,107,108},
 {2,107,113},
 {2,106,109},
 {2,106,110},
 {2,106,107},
 {2,106,106},
 {2,106,100},
 {2,106,108},
 {2,106,113},
 {2,107,0},
 {2,107,1},
 {2,106,0},
 {2,106,1},
 {3,108,109},
 {3,108,108},
 {3,108,110},
 {3,108,100},
 {3,108,105},
 {3,108,113},
 {3,108,0},
 {3,108,1},
 {4,105,109},
 {4,105,105},
 {4,105,110},
 {4,105,100},
 {4,105,113},
 {4,105,0},
 {4,105,1},
 {5,111,109},
 {5,111,111},
 {5,111,105},
 {5,111,110},
 {5,111,100},
 {5,111,108},
 {5,111,113},
 {5,111,0},
 {5,111,1}
};
}

extern "C" {
u32 lbl_80335DAC[3]={0,0,(u32)fn_801B3C98};
int lbl_80435C50[8];
ModuleTag lbl_8055E6B8;
Manager* lbl_8055E6C0;
}

// Named original literal scalars; trailing metadata bytes are not invented.
extern "C" {
__declspec(section ".sdata2") extern const float lbl_80562620=-1.0f;
__declspec(section ".sdata2") extern const float lbl_80562624=0.0f;
__declspec(section ".sdata2") extern const float lbl_80562628=25000000.0f;
__declspec(section ".sdata2") extern const float lbl_8056262C=64.0f;
__declspec(section ".sdata2") extern const float lbl_80562630=0.5f;
__declspec(section ".sdata2") extern const float lbl_80562634=25.0f;
__declspec(section ".sdata2") extern const float lbl_80562638=0.6499999761581421f;
__declspec(section ".sdata2") extern const float lbl_8056263C=250000.0f;
__declspec(section ".sdata2") extern const float lbl_80562640=35000.0f;
__declspec(section ".sdata2") extern const float lbl_80562644=0.30000001192092896f;
__declspec(section ".sdata2") extern const float lbl_80562648=0.699999988079071f;
__declspec(section ".sdata2") extern const float lbl_8056264C=1.0f;
__declspec(section ".sdata2") extern const float lbl_80562650=-5.0f;
}

