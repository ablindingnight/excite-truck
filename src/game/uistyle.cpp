// Unified research draft. Four bodies remain provisional; no Matching promotion.
// Partial UIStyle research source. No unit registration or promotion claim.
#include "game/common.h"
struct UIStyleRecordFields {
    u8 active; // Three bytes of natural alignment, not semantic fields.
    u32 resource04,resource08; u32 colors0C[5]; u32 colors20[5]; // +8 is the font resource used by these helpers.
    f32 height;      // +0x34
    int field38;
    u32 field3C_0,field3C_1,field3C_2,field3C_3,field3C_4,field3C_5,field3C_6,field3C_7;
    f32 field5C,field60;
    u32 field64_0,field64_1,field64_2,field64_3;
};
typedef char CheckUIStyleRecord[sizeof(UIStyleRecordFields)==0x74?1:-1];
struct UIStyleInputFields {
    const char* fontName; f32 height; u32 field08;
    u32 words0C[11];
    u32 field38,field3C,field40;
    const char* resource; u32 field48; f32 field4C,field50; u32 field54;
};
typedef char CheckUIStyleInput[sizeof(UIStyleInputFields)==0x58?1:-1];
typedef char CheckStylePointerWidth[(sizeof(void*)==4 && sizeof(u32)==4)?1:-1];
struct UIStyleNameFields {int value; const char* name;};
extern "C" {
extern UIStyleRecordFields lbl_80490968[256];
extern const char lbl_8055B8B0[5],lbl_8055B8B8[5];
extern const char lbl_8033FBE8[];
extern const UIStyleNameFields lbl_8033FF98[27];
char* strstr(const char*,const char*);
char* strncpy(char*,const char*,unsigned long);
int strcasecmp(const char*,const char*);
void fn_802E317C(const char*,...);
int fn_80232B70(int);
f32 fn_8023293C(int,const char*,f32);
void fn_8022C9D0(void);
void fn_8022CAA8(UIStyleRecordFields*,const UIStyleInputFields*);
void fn_8022A8E4(int);
UIStyleRecordFields* fn_8022CA2C(int);
f32 fn_8022CA40(UIStyleRecordFields*);


extern UIStyleRecordFields lbl_80497D68;
extern const char lbl_8055B918[5],lbl_8055B920[5],lbl_8033FC40[];
void* memset(void*,int,unsigned long);
void fn_8022C520(void);
int fn_80232578(const char*);
void fn_801E4A2C(int);
void fn_802328D4(int);
void fn_802C4C80(int);
void fn_802C519C(int);
void fn_80235380(int);
int fn_802C5130(const char*);
int fn_8023512C(const char*);
int fn_802C49E8(const char*);





















extern const char lbl_8055B928[5],lbl_80340070[];
int sscanf(const char*,const char*,...);
int abs(int);
void fn_8022B390(int,const char*,int,int*,int*);



void fn_8022ACFC(int,const char*,f32,f32,f32*,f32*,f32*,f32*);
void fn_8022BD40(int,f32,f32,f32,f32,f32,f32,const char*,u32,u8);
void fn_80230C30(f32*,f32*,int,int);
int fn_801E4A6C(int,const char*);
int fn_801E4BA8(int);
extern ModuleTag lbl_8055EFC0;
extern UnkStatic123s lbl_80490950;
extern UnkStatic7F lbl_8055EFC8;










int fn_802C4EE4(u32);
int fn_802C51C0(u32);
u32 fn_802C4F14(u32);
void fn_802C4C90(u32,int,int,u32,int);
u32 fn_802C51AC(u32);
void fn_801E4DBC(u32,int,int,int,int,const char*,int);
char* strchr(const char*,int);
unsigned long strlen(const char*);
extern const char lbl_8033FC08[];




extern const char lbl_8055B930[7],lbl_8055B938[6],lbl_8055B940[7],lbl_8055B948[7],lbl_8055B950[8];
int fn_802C4EFC(u32);
int fn_802C5220(u32);
int fn_801E4B48(int);
int fn_801E4B00(int);
int fn_80232B78(int);

static inline int StyleAddPair(int a,int b) { return a+b; }
static inline f32 StyleMultiply(f32 a,f32 b) { return a*b; }




extern const UIStyleInputFields lbl_802F9F58;
extern const char lbl_8055B8C4[6],lbl_8055B8CC[5],lbl_8055B8D4[6],lbl_8055B8DC[7];
extern const char lbl_8055B8E4[4],lbl_8055B8E8[7],lbl_8055B8F0[7];
extern const char lbl_8055B8F8[4],lbl_8055B8FC[3],lbl_8055B900[6],lbl_8055B908[8],lbl_8055B910[7];
ValueTable* fn_80131810(const char*);
void fn_80131818(ValueTable*);
void fn_802E30C8(const char*,...);



extern const char lbl_8055B8C0[2];
void fn_802C4F2C(u32,int*,int*);
int fn_801E4AB0(int,const char*,int);
template<class T> static inline T StyleMinimum(const T& a,const T& b) {return a<b?a:b;}
template<class T> static inline T StyleMaximum(const T& a,const T& b) {return a>b?a:b;}



struct StyleViewport { u8 pad[0x70];f32 horizontalBias; };
struct StyleViewportStorage { StyleViewport* active;u32 unknownTail; };
typedef char CheckStyleViewportStorage[sizeof(StyleViewportStorage)==8?1:-1];
extern StyleViewportStorage lbl_8055E050;
void fn_80230F78(u8*,f32*,u32*,int);
f32 fn_8023659C(void);
double fmod(double,double);
double sin(double);
void fn_802C5310(u32,u32,f32,f32,f32);
u32 fn_80235FD8(u32);
f32 fn_80235F64(u32,int);
f32 fn_80235EF0(u32,int);
void fn_802354A0(u32,int,u32,f32,f32,f32,f32,f32);
void fn_80230924(u32,f32,f32,f32,f32,f32);
void fn_80232BA4(const char*, u32, f32, f32, u32, int, f32, f32, f32);
void fn_80232928(int);
void fn_80232930(void);



void fn_8022A7D0(void);
int fn_8022A7D4(const char* s);
int fn_8022A804(const char* s);
u32 fn_8022A834(u32 color);
void fn_8022A858(int i,const UIStyleInputFields* input);
int fn_8022A86C(const UIStyleInputFields* input);
int fn_8022AA84(int i);
f32 fn_8022AAA8(int i,const char* text);
int fn_8022AB18(int i,int axis,int mode);
int fn_8022AB94(const signed char* text,signed char value);
f32 fn_8022ABC8(int font,const char* text,int n);
void fn_8022C9D0(void);
UIStyleRecordFields* fn_8022CA2C(int i);
f32 fn_8022CA40(UIStyleRecordFields* s);
int fn_8022CC8C(const char* name);
void fn_8022A5E4(void);
void fn_8022A8E4(int i);
void fn_8022CAA8(UIStyleRecordFields* s,const UIStyleInputFields* input);
u32 fn_8022CD0C(const char* text);
int fn_8022B2D4(int style,const char* text,int targetX,int targetY);
void fn_8022AC1C(int style,const char* text,int x,int y,int* a,int* b,int* c,int* d);
void fn_8022B390(int style,const char* text,int n,int* outX,int* outY);
void fn_8022BAD0(int style,int x,int y,int w,int h,int z,const char* p,u32 q,u8 flag,f32 scale);
void fn_8022BB60(f32* px,f32* py,f32 x,f32 y);
u32 fn_8022BC30(u32 a,u32 b,f32 t);
void fn_8022D088(void);
int fn_8022B498(int style,const char* text);
void fn_8022B91C(int style,int x,int y,const char* text,u32 frame);
int fn_8022B620(int style,const char* text);
void fn_8022CDE4(u32* out,ValueNode* node);
void fn_8022C520(void);
void fn_8022ACFC(int style,const char* text,f32 x,f32 y,f32* outLeft,f32* outTop,f32* outRight,f32* outBottom);
void fn_8022BD40(int style,f32 x,f32 y,f32 depth,f32 width,f32 height,f32 unused,const char* text,u32 frame,u8 initialShadow);
void fn_8022A5E4(void) {
    memset(&lbl_80497D68,0,sizeof(lbl_80497D68));
    lbl_80497D68.field38=9;
    for(int i=0;i<256;i++) lbl_80490968[i]=lbl_80497D68;
    fn_8022C520();
}

void fn_8022A7D0(void) {fn_8022C9D0();}

int fn_8022A7D4(const char* s) {return strstr(s,lbl_8055B8B0)!=0;}

int fn_8022A804(const char* s) {return strstr(s,lbl_8055B8B8)!=0;}

u32 fn_8022A834(u32 color) {
    u32 rgb=color&0xFFFFFF;
    u8 alpha=color>>24;
    if(alpha<32 && alpha!=0) alpha=255;
    color&=0xFFFFFF;
    color|=alpha<<24;
    return color;
}

void fn_8022A858(int i,const UIStyleInputFields* input) {fn_8022CAA8(&lbl_80490968[i],input);}

int fn_8022A86C(const UIStyleInputFields* input) {
    UIStyleRecordFields* p=lbl_80490968;
    for(int i=0;i<256;p++,i++) {
        if(!p->active) {fn_8022A858(i,input);return i;}
    }
    fn_802E317C(lbl_8033FBE8);
    return -1;
}

void fn_8022A8E4(int i) {
    UIStyleRecordFields* s=&lbl_80490968[i];
    if(s->resource04) fn_801E4A2C(s->resource04);
    if(s->resource08) fn_802328D4(s->resource08);
    if(s->field64_0) fn_802C4C80(s->field64_0);
    if(s->field64_1) fn_802C519C(s->field64_1);
    if(s->field64_2) fn_80235380(s->field64_2);
    if(s->field64_3) fn_80235380(s->field64_3);
    lbl_80490968[i]=lbl_80497D68;
    s->active=0;
}

int fn_8022AA84(int i) {return fn_8022CA2C(i)->field38;}

f32 fn_8022AAA8(int i,const char* text) {
    UIStyleRecordFields* s=fn_8022CA2C(i);
    if(s->resource08) {
        f32 scale=fn_8022CA40(s);
        f32 width=fn_8023293C((int)s->resource08,text,1.0f);
        width*=scale;
        return width;
    }
    return 0.0f;
}

int fn_8022AB18(int i,int axis,int mode) {
    UIStyleRecordFields* s=fn_8022CA2C(i);
    if(axis==0) {
        if(mode==2) return (int)s->field3C_2;
        return (int)s->field3C_4;
    }
    if(axis==1) {
        if(mode==2) return (int)s->field3C_3;
        return (int)s->field3C_5;
    }
    return 0;
}

int fn_8022AB94(const signed char* text,signed char value) {
    int n=0;
    while(*text) {
        if(*text==value) n++;
        text++;
    }
    return n;
}

f32 fn_8022ABC8(int font,const char* text,int n) {
    char buffer[2048];
    strncpy(buffer,text,n);
    buffer[n]=0;
    return fn_8023293C(font,buffer,1.0f);
}

void fn_8022AC1C(int style,const char* text,int x,int y,int* a,int* b,int* c,int* d) {
    f32 fa,fb,fc,fd;
    fn_8022ACFC(style,text,(f32)x,(f32)y,&fa,&fb,&fc,&fd);
    *a=(int)fa; *b=(int)fb; *c=(int)fc; *d=(int)fd;
}

void fn_8022ACFC(int style,const char* text,f32 x,f32 y,f32* outLeft,f32* outTop,f32* outRight,f32* outBottom) {
        f32 innerBase,outerBase;
    f32 width;
    UIStyleRecordFields* s=fn_8022CA2C(style);
    f32 left=x,top=y,right=x,bottom=y;
    f32 height,dx,dy;
    if(s->field64_0) {
        int ox,oy;
        fn_802C4F2C(s->field64_0,&ox,&oy);
        int width=fn_802C4EE4(s->field64_0);
        int height=fn_802C4EFC(s->field64_0);
        left=StyleMinimum(x,x-(f32)ox);
        right=StyleMaximum(right,(f32)width+(x-(f32)ox));
        top=StyleMinimum(top,y-(f32)oy);
        bottom=StyleMaximum(bottom,(f32)height+(y-(f32)oy));
    } else if(s->field64_1) {
        right=StyleMaximum(x,x+(f32)fn_802C51C0(s->field64_1));
        bottom=StyleMaximum(bottom,y+(f32)fn_802C5220(s->field64_1));
    }
    if(text) {
        height=0.0f;
        int lines=fn_8022AB94((const signed char*)text,10)+1;
        if(s->resource04) height=(f32)lines*(f32)fn_801E4BA8((int)s->resource04);
        u32 font=s->resource08;
        if(font) {
            if(s->height<1.0f) height=(f32)lines*(f32)fn_80232B70((int)font);
            else {
                outerBase=(f32)fn_80232B70((int)font);
                height=(s->height*(f32)(lines*fn_80232B70((int)font)))/outerBase;
            }
        }
        width=0.0f;
        for(int i=0;i<lines;i++) {
            const char* end=strchr(text,10);
            int n=end?(int)(end-text):(int)strlen(text);
            if(s->resource04) width=StyleMaximum(width,(f32)fn_801E4AB0((int)s->resource04,text,n));
            if(s->resource08) {
                if(s->height<1.0f) width=StyleMaximum(width,fn_8022ABC8((int)s->resource08,text,n));
                else {
                    innerBase=(f32)fn_80232B70((int)s->resource08);
                    width=StyleMaximum(width,(s->height*fn_8022ABC8((int)s->resource08,text,n))/innerBase);
                }
            }
            end=strchr(text,10);text=end;if(text)text++;
        }
        dx=0.0f;dy=0.0f;
        if(s->field38&2) dx=-width;
        else if(s->field38&4) dx=(-width)*0.5f;
        if(s->field38&64) dy=-height;
        else if(s->field38&16) dy=(-height)*0.5f;
        else if(s->field38&32) {
            dy=0.0f;
            if(s->resource04)dy=-(f32)fn_801E4B00((int)s->resource04);
            if(s->resource08)dy=-(f32)fn_80232B78((int)s->resource08);
        }
        width+=0.5f*(f32)(fn_801E4A6C((int)s->resource04,lbl_8055B8C0)+1);
        int a=(int)s->field3C_2,b=(int)s->field3C_4;
        left=StyleMinimum(left,(x+dx)+(f32)(b<a?b:a));
        right=StyleMaximum(right,(width+(x+dx))+(f32)(b>a?b:a));
        a=(int)s->field3C_3;b=(int)s->field3C_5;
        top=StyleMinimum(top,(y+dy)+(f32)(b<a?b:a));
        bottom=StyleMaximum(bottom,(height+(y+dy))+(f32)(b>a?b:a));
    }
    *outLeft=left;*outRight=right;*outTop=top;*outBottom=bottom;
}

int fn_8022B2D4(int style,const char* text,int targetX,int targetY) {
    int bestIndex=0;
    int bestDistance=-1;
    for(int i=0;i==0||text[i-1];i++) {
        int x,y;
        fn_8022B390(style,text,i,&x,&y);
        int dy=abs(y-targetY);
        int dx=abs(x-targetX);
        int distance=dx+dy;
        if(bestDistance<0||distance<bestDistance) {
            bestIndex=i;
            bestDistance=distance;
        }
    }
    return bestIndex;
}

void fn_8022B390(int style,const char* text,int n,int* outX,int* outY) {
    UIStyleRecordFields* s=fn_8022CA2C(style);
    char ch[2]={0,0};
    int x=(int)s->field3C_2;
    int y=(int)s->field3C_3;
    for(int i=0;*text && i<n;i++,text++) {
        if((signed char)*text!=10) {
            ch[0]=*text;
            if(s->resource08) x+=(int)fn_8023293C((int)s->resource08,ch,1.0f);
            else if(s->resource04) x+=fn_801E4A6C((int)s->resource04,ch);
        } else {
            if(s->resource08) y+=fn_80232B70((int)s->resource08);
            else if(s->resource04) y+=fn_801E4BA8((int)s->resource04);
            x=(int)s->field3C_2;
        }
    }
    *outX=x; *outY=y;
}

int fn_8022B498(int style,const char* text) {
    int flags;
    int left;
    int width;
    int edge;
    int imageWidth;
    int right;
    UIStyleRecordFields* s=fn_8022CA2C(style);
    imageWidth=0;
    if(s->field64_0) imageWidth=fn_802C4EE4(s->field64_0);
    else if(s->field64_1) imageWidth=fn_802C51C0(s->field64_1);
    if(text) {
        left=0;right=0;
        width=0;
        if(s->resource04) width=fn_801E4A6C((int)s->resource04,text);
        if(s->resource08) {
            f32 scale;
            if(s->height<1.0f) scale=1.0f;
            else scale=s->height/(f32)fn_80232B70((int)s->resource08);
            width=(int)(scale*fn_8023293C((int)s->resource08,text,1.0f));
        }
        flags=s->field38;
        if(flags&2) left=-width;
        else if(flags&4) left=-width/2;
        if(flags&1) right=width;
        else if(flags&4) right=width/2;
        edge=(int)s->field3C_4;
        if((int)s->field3C_2>edge) edge=(int)s->field3C_2;
        int extent=right+edge-left;
        return imageWidth>extent?imageWidth:extent;
    }
    return imageWidth;
}

int fn_8022B620(int style,const char* text) {
    UIStyleRecordFields* s=fn_8022CA2C(style);
    int imageHeight=0;
    if(s->field64_0) imageHeight=fn_802C4EFC(s->field64_0);
    else if(s->field64_1) imageHeight=fn_802C5220(s->field64_1);
    if(text) {
        int top=0;
        if(s->resource04) {
            if(s->field38&8) top=fn_801E4BA8((int)s->resource04);
            else if(s->field38&16) top=fn_801E4BA8((int)s->resource04)/2;
            else if(s->field38&32) top=fn_801E4B48((int)s->resource04);
            else top=0;
        }
        if(s->resource08) {
            if(s->field38&8) {
                if(s->height<1.0f) top=fn_80232B70((int)s->resource08);
                else top=(int)s->height;
            } else if(s->field38&16) {
                if(s->height<1.0f) top=fn_80232B70((int)s->resource08)/2;
                else top=(int)(0.5f*s->height);
            } else top=0;
        }
        int bottom=0;
        if(s->resource04) {
            if(s->field38&16) bottom=fn_801E4BA8((int)s->resource04)/2;
            else if(s->field38&32) bottom=fn_801E4B00((int)s->resource04);
            else if(s->field38&64) bottom=fn_801E4BA8((int)s->resource04);
            else bottom=0;
        }
        if(s->resource08) {
            f32 scale;
            if(s->height<1.0f) scale=1.0f;
            else scale=s->height/(f32)fn_80232B70((int)s->resource08);
            if(s->field38&16) bottom=(int)(StyleMultiply(scale*(f32)fn_80232B70((int)s->resource08),0.5f));
            else if(s->field38&32) bottom=(int)(scale*(f32)fn_80232B78((int)s->resource08));
            else if(s->field38&64) bottom=(int)(scale*(f32)fn_80232B70((int)s->resource08));
            else bottom=0;
        }
        int edge=(int)s->field3C_5;
        if((int)s->field3C_3>edge) edge=(int)s->field3C_3;
        int extent=top+(int)((u32)bottom+(u32)edge);
        return imageHeight>extent?imageHeight:extent;
    }
    return imageHeight;
}

void fn_8022B91C(int style,int x,int y,const char* text,u32 frame) {
    UIStyleRecordFields* s=fn_8022CA2C(style);
    u32 alternate=frame&1;
    if(s->field64_0) {
        u32 remainder=frame%fn_802C4F14(s->field64_0);
        fn_802C4C90(s->field64_0,x,y,remainder,0);
    } else if(s->field64_1) fn_802C51AC(s->field64_1);
    if(text && !s->resource08) {
        fn_802E317C(lbl_8033FC08);
        int height=fn_801E4BA8((int)s->resource04);
        int lines=fn_8022AB94((const signed char*)text,10)+1;
        if(s->field38&16) y-=((lines-1)*height)/2;
        else if(s->field38&64) y-=(lines-1)*height;
        int i=0;
        if(lines>0) for(;i<lines;i++) {
            const char* end=strchr(text,10);
            int n=end?(int)(end-text):(int)strlen(text);
            int dy=alternate?(int)s->field3C_5:(int)s->field3C_3;
            int dx=alternate?(int)s->field3C_4:(int)s->field3C_2;
            fn_801E4DBC(s->resource04,0,s->field38,x+dx,y+dy,text,n);
            y+=height;
            end=strchr(text,10);
            text=end;
            if(text)text++;
        }
    }
}

void fn_8022BAD0(int style,int x,int y,int w,int h,int z,const char* p,u32 q,u8 flag,f32 scale) {
    fn_8022BD40(style,(f32)x,(f32)y,(f32)w,(f32)h,(f32)z,scale,p,q,flag);
}

void fn_8022BB60(f32* px,f32* py,f32 x,f32 y) {
    fn_80230C30(px,py,(int)x,(int)y);
    *px+=x-(f32)(int)x;
    *py-=y-(f32)(int)y;
}

static inline f32 BlendStyleByte(u32 a,u32 b,f32 t){ f32 bf=(f32)b; f32 af=(f32)a; return af+t*(bf-af); }
u32 fn_8022BC30(u32 a,u32 b,f32 t) {
int c0=(int)BlendStyleByte((a>>0)&255,(b>>0)&255,t);
int c1=(int)BlendStyleByte((a>>8)&255,(b>>8)&255,t);
int c2=(int)BlendStyleByte((a>>16)&255,(b>>16)&255,t);
int c3=(int)BlendStyleByte(a>>24,b>>24,t);
return (c0&255)|((c1&255)<<8)|((c2&255)<<16)|(c3<<24);
}

void fn_8022BD40(int style,f32 x,f32 y,f32 depth,f32 width,f32 height,f32 unused,const char* text,u32 frame,u8 initialShadow) {
    UIStyleRecordFields* s=fn_8022CA2C(style);
    u8 alpha=255;
    f32 glow=0.0f;
    u32 glowColor=s->colors0C[frame];
    u32 alternate=frame&1;
    if((int)(alternate?s->field3C_1:s->field3C_0)==1) fn_80230F78(&alpha,&glow,&glowColor,0);
    int effect=alternate?(int)s->field3C_1:(int)s->field3C_0;
    u32 cycleColor=0xFFFFFFFF;
    if((u32)(effect-5)<=1) {
        f32 phase=(f32)fmod((double)(6.2831854820251465f*fn_8023659C()),9.42477798461914);
        f32 t=1.0f;
        if(phase<3.1415927410125732f) t=1.0f-(f32)sin((double)phase);
        if(alternate)cycleColor=fn_8022BC30(s->colors0C[3],s->colors0C[1],t);
        else cycleColor=fn_8022BC30(s->colors0C[2],s->colors0C[0],t);
    }
    if(s->field64_1) {
        f32 px,py;
        fn_8022BB60(&px,&py,x,y);
        u32 count=fn_802C51AC(s->field64_1);
        u32 animationFrame=frame%count;
        fn_802C5310(s->field64_1,animationFrame,px,py,0.0f);
    } else if(s->field64_2) {
        f32 px,py;
        fn_8022BB60(&px,&py,x,y);
        u32 current=frame%fn_80235FD8(s->field64_2);
        if(s->field38&16)py+=StyleMultiply(fn_80235F64(s->field64_2,current),0.5f);
        else if(s->field38&64)py+=fn_80235F64(s->field64_2,current);
        if(s->field38&4)px-=StyleMultiply(lbl_8055E050.active->horizontalBias+fn_80235EF0(s->field64_2,current),0.5f);
        else if((s->field38&2)&&text&&s->resource08) {
            f32 scale=fn_8022CA40(s);
            px-=fn_8023293C((int)s->resource08,text,1.0f)*scale;
        }
        f32 imageWidth=lbl_8055E050.active->horizontalBias+fn_80235EF0(s->field64_2,current);
        f32 imageHeight=fn_80235F64(s->field64_2,current);
        fn_802354A0(s->field64_2,current,0xFFFFFFFF,px,py,0.0f,imageWidth,imageHeight);
        if(effect==1) {
            f32 halfGlow=0.5f*glow;
            f32 factor=1.0f+glow;
            fn_802354A0(s->field64_2,current,((u32)alpha<<24)|0xFFFFFF,
                px-halfGlow*imageWidth,py+halfGlow*imageHeight,0.0f,factor*imageWidth,factor*imageHeight);
        }
        if(frame==1) {
            int suffix=(int)s->field3C_6;
            if(suffix>=0) {
                f32 h=fn_80235F64(s->field64_2,current);
                f32 w=fn_80235EF0(s->field64_2,suffix);
                f32 dx=fn_80235EF0(s->field64_2,current);
                fn_802354A0(s->field64_2,suffix,0xFFFFFFFF,px+dx,py,0.0f,w,h);
            }
        }
        if(s->field64_3&&frame==1) {
            int overlay=(int)s->field3C_7;
            f32 h=fn_80235F64(s->field64_3,overlay);
            f32 w=fn_80235EF0(s->field64_3,overlay);
            fn_802354A0(s->field64_3,overlay,0xFFFFFFFF,px+s->field5C,py+s->field60,0.0f,w,h);
        }
    } else if(alternate) {
        f32 px,py;
        fn_8022BB60(&px,&py,x,y);
        if(s->field38&16)py+=height*0.5f;
        else if(s->field38&64)py+=height;
        if(s->field38&4)px-=(lbl_8055E050.active->horizontalBias+width)*0.5f;
        else if(s->field38&2)px-=lbl_8055E050.active->horizontalBias+width;
        fn_80230924(s->colors20[frame],px,py,px+width,py-height,depth);
    }
    if(text&&s->resource08) {
        int lineHeight=fn_80232B70((int)s->resource08);
        int lines=fn_8022AB94((const signed char*)text,10)+1;
        if(s->field38&16)y-=(f32)(((lines-1)*lineHeight)/2);
        else if(s->field38&64)y-=(f32)((lines-1)*lineHeight);
        char line[1024];
        int shadow=initialShadow;
        f32 textZ=0.0f;
        f32 shadowOffset=2.0f;
        for(int i=0;i<lines;i++) {
            const char* end=strchr(text,10);
            int n=end?(int)(end-text):(int)strlen(text);
            strncpy(line,text,n);line[n]=0;
            f32 px,py;
            int dx=alternate?(int)s->field3C_4:(int)s->field3C_2;
            int dy=alternate?(int)s->field3C_5:(int)s->field3C_3;
            fn_8022BB60(&px,&py,x+(f32)dx,y+(f32)dy);
            int flags=0;
            u32 color=s->colors0C[frame];
            if(s->field38&1)flags|=1;
            if(s->field38&4)flags|=4;
            if(s->field38&2)flags|=2;
            if(s->field38&8)flags|=8;
            if(s->field38&16)flags|=16;
            if(s->field38&32)flags|=32;
            if(s->field38&64)flags|=64;
            switch(effect) {
            case 2:shadow=1;
            default: {
                f32 scale=fn_8022CA40(s);
                if(shadow)fn_80232BA4(line, s->resource08, px+shadowOffset, py-shadowOffset, 0xFF000000, flags, scale, textZ, 1.0f);
                fn_80232BA4(line, s->resource08, px, py, color, flags, scale, textZ, 1.0f);
                break;
            }
            case 1:case 4: {
                f32 scale=fn_8022CA40(s);
                fn_80232BA4(line, s->resource08, px, py, glowColor, flags, scale, textZ, 1.0f);
                break;
            }
            case 5:case 6: {
                if(effect==5)fn_80232928(-1);
                f32 scale=fn_8022CA40(s);
                fn_80232BA4(line, s->resource08, px, py, cycleColor, flags, scale, textZ, 1.0f);
                if(effect==5)fn_80232930();
                break;
            }
            }
            y+=(f32)lineHeight;
            end=strchr(text,10);text=end;if(text)text++;
        }
    }
}

void fn_8022C520(void) {
    const u32 pool=(u32)lbl_8033FBE8;
    UIStyleInputFields defaults=lbl_802F9F58;
    defaults.words0C[0]=0xFFD6003C; defaults.words0C[1]=0xFFD6003C;
    defaults.words0C[2]=0xFF2DC82D; defaults.words0C[3]=0xFF2DC82D;
    defaults.words0C[7]=0xFF2D2DC8; defaults.words0C[8]=0xFF2D2DC8;
    defaults.words0C[9]=0xFFD6003C; defaults.words0C[10]=0xFFD6003C;
    ValueTable* table=fn_80131810(((const char*)(pool+0x64)));
    if(table) {
        ValueNode* node;
        for(int i=0;(node=table->GetChild(i))!=0;i++) {
            if(strcasecmp(node->GetName(),lbl_8055B8C4)!=0)continue;
            int id;
            if(node->Has(((const char*)(pool+0x70)))) id=node->GetIntValue(((const char*)(pool+0x70)));
            else {
                if(!node->Has(((const char*)(pool+0x7C)))) {fn_802E30C8(((const char*)(pool+0x88)));continue;}
                id=fn_8022CC8C(node->GetValue(((const char*)(pool+0x7C))));
            }
            if(id<0||id>=256) {fn_802E30C8(((const char*)(pool+0xB4)),node->GetString(((const char*)(pool+0x7C)),((const char*)(pool+0xE4))));continue;}
            UIStyleInputFields input=defaults;
            input.fontName=node->GetString(((const char*)(pool+0xF0)),input.fontName);
            input.height=node->GetFloat(((const char*)(pool+0xFC)),input.height);
            int horizontal=1;
            if(node->Has(((const char*)(pool+0x108)))) {
                const char* value=node->GetValue(((const char*)(pool+0x108)));
                if(strcasecmp(value,lbl_8055B8CC)==0)horizontal=1;
                else if(strcasecmp(value,lbl_8055B8D4)==0)horizontal=2;
                else if(strcasecmp(value,lbl_8055B8DC)==0)horizontal=4;
                else fn_802E30C8(((const char*)(pool+0x120)),value);
            }
            int vertical=8;
            if(node->Has(((const char*)(pool+0x154)))) {
                const char* value=node->GetValue(((const char*)(pool+0x154)));
                if(strcasecmp(value,lbl_8055B8E4)==0)vertical=8;
                else if(strcasecmp(value,lbl_8055B8E8)==0)vertical=16;
                else if(strcasecmp(value,((const char*)(pool+0x168)))==0)vertical=32;
                else if(strcasecmp(value,lbl_8055B8F0)==0)vertical=64;
                else fn_802E30C8(((const char*)(pool+0x174)),value);
            }
            input.field08=horizontal|vertical;
            fn_8022CDE4(&input.words0C[0],node->FindChild(lbl_8055B8F8));
            fn_8022CDE4(&input.words0C[7],node->FindChild(lbl_8055B8FC));
            if(node->Has(lbl_8055B900))input.resource=node->GetValue(lbl_8055B900);
            input.field54=node->GetInt(lbl_8055B908,input.field54);
            input.field48=node->GetInt(lbl_8055B910,input.field48);
            input.field4C=node->GetFloat(((const char*)(pool+0x1A8)),input.field4C);
            input.field50=node->GetFloat(((const char*)(pool+0x1B4)),input.field50);
            fn_8022A858(id,&input);
        }
        fn_80131818(table);
    }
}

void fn_8022C9D0(void) {
    UIStyleRecordFields* s=lbl_80490968;
    for(int i=0;i<256;i++,s++) {
        if(s->active) fn_8022A8E4(i);
    }
}

UIStyleRecordFields* fn_8022CA2C(int i) {return &lbl_80490968[i];}

f32 fn_8022CA40(UIStyleRecordFields* s) {
    if(s->height<1.0f) return 1.0f;
    return s->height/(f32)fn_80232B70((int)s->resource08);
}

void fn_8022CAA8(UIStyleRecordFields* s,const UIStyleInputFields* input) {
    s->active=1;
    s->resource04=0;
    s->resource08=0;
    if(strstr(input->fontName,lbl_8055B918)) s->resource08=fn_80232578(lbl_8033FC40);
    const char* font=input->fontName;
    if(strstr(font,lbl_8055B920)) s->resource08=fn_80232578(font);
    s->height=input->height;
    s->colors0C[0]=fn_8022A834(input->words0C[0]);
    s->colors0C[1]=fn_8022A834(input->words0C[7]);
    s->colors0C[2]=fn_8022A834(input->words0C[1]);
    s->colors0C[3]=fn_8022A834(input->words0C[8]);
    s->colors0C[4]=fn_8022A834(input->words0C[4]);
    s->colors20[0]=fn_8022A834(input->words0C[2]);
    s->colors20[1]=fn_8022A834(input->words0C[9]);
    s->colors20[2]=fn_8022A834(input->words0C[3]);
    s->colors20[3]=fn_8022A834(input->words0C[10]);
    s->colors20[4]=fn_8022A834(input->words0C[2]);
    const char* resource=input->resource;
    s->field38=input->field08;
    s->field3C_0=input->words0C[4];
    s->field3C_1=input->field38;
    s->field3C_2=input->words0C[5];
    s->field3C_3=input->words0C[6];
    s->field3C_4=input->field3C;
    s->field3C_5=input->field40;
    s->field3C_6=input->field54;
    s->field3C_7=input->field48;
    s->field5C=input->field4C;
    s->field60=input->field50;
    s->field64_0=0;s->field64_1=0;s->field64_2=0;s->field64_3=0;
    if(resource) {
        if(fn_8022A7D4(resource)) s->field64_1=fn_802C5130(resource);
        else if(fn_8022A804(resource)) s->field64_2=fn_8023512C(resource);
        else s->field64_0=fn_802C49E8(resource);
    }
    s->field3C_7=(u32)-1;
}

int fn_8022CC8C(const char* name) {
    const UIStyleNameFields* p=lbl_8033FF98;
    for(u32 i=0;i<27;i++,p++) {
        if(strcasecmp(name,p->name)==0) return lbl_8033FF98[i].value;
    }
    return 26;
}

u32 fn_8022CD0C(const char* text) {
    u32 result;
    int c0,c1,c2,c3;
    if(sscanf(text,lbl_8055B928,&result)==1) return result;
    int n=sscanf(text,lbl_80340070,&c0,&c1,&c2,&c3);
    if(n==3) { u32 h=(0xFF000000|((c0&255)<<16)); u32 l=((c2&255)|((c1&255)<<8)); result=l|h; }
    else if(n==4) result=((c0&255)<<24)|((c1&255)<<16)|((c2&255)<<8)|(c3&255);
    else result=0xFFFF00FF;
    return result;
}

void fn_8022CDE4(u32* out,ValueNode* node) {
    const u32 pool=(u32)lbl_8033FBE8;
    if(node) {
        if(node->Has(((const char*)(pool+0x498)))) out[0]=fn_8022CD0C(node->GetValue(((const char*)(pool+0x498))));
        if(node->Has(((const char*)(pool+0x4A4)))) out[1]=fn_8022CD0C(node->GetValue(((const char*)(pool+0x4A4))));
        if(node->Has(((const char*)(pool+0x4BC)))) out[2]=fn_8022CD0C(node->GetValue(((const char*)(pool+0x4BC))));
        if(node->Has(((const char*)(pool+0x4C8)))) out[3]=fn_8022CD0C(node->GetValue(((const char*)(pool+0x4C8))));
        if(node->Has(((const char*)(pool+0x4E0)))) {
            const char* value=node->GetValue(((const char*)(pool+0x4E0)));
            if(strcasecmp(value,lbl_8055B930)==0) out[4]=0;
            else if(strcasecmp(value,lbl_8055B938)==0) out[4]=4;
            else if(strcasecmp(value,lbl_8055B940)==0) out[4]=2;
            else if(strcasecmp(value,((const char*)(pool+0x4EC)))==0) out[4]=1;
            else if(strcasecmp(value,((const char*)(pool+0x4F8)))==0) out[4]=5;
            else if(strcasecmp(value,((const char*)(pool+0x504)))==0) out[4]=6;
            else out[4]=fn_8022CD0C(value);
        }
        if(node->Has(lbl_8055B948)) sscanf(node->GetValue(lbl_8055B948),lbl_8055B950,&out[5],&out[6]);
    }
}


}

extern "C" {

extern const UIStyleInputFields lbl_802F9F58;
extern const char lbl_8033FBE8[25];
extern const char lbl_8033FC08[56];
extern const char lbl_8033FC40[360];
extern const char lbl_8033FDA8[13];
extern const char lbl_8033FDB8[15];
extern const char lbl_8033FDC8[18];
extern const char lbl_8033FDDC[21];
extern const char lbl_8033FDF4[12];
extern const char lbl_8033FE00[14];
extern const char lbl_8033FE10[14];
extern const char lbl_8033FE20[14];
extern const char lbl_8033FE30[12];
extern const char lbl_8033FE3C[13];
extern const char lbl_8033FE4C[14];
extern const char lbl_8033FE5C[19];
extern const char lbl_8033FE70[12];
extern const char lbl_8033FE7C[18];
extern const char lbl_8033FE90[12];
extern const char lbl_8033FE9C[14];
extern const char lbl_8033FEAC[20];
extern const char lbl_8033FEC0[19];
extern const char lbl_8033FED4[14];
extern const char lbl_8033FEE8[16];
extern const char lbl_8033FEF8[14];
extern const char lbl_8033FF08[18];
extern const char lbl_8033FF1C[17];
extern const char lbl_8033FF30[19];
extern const char lbl_8033FF44[21];
extern const char lbl_8033FF5C[26];
extern const char lbl_8033FF78[31];
extern const UIStyleNameFields lbl_8033FF98[27];
extern const char lbl_80340070[152];
extern const char lbl_8055B8B0[5];
extern const char lbl_8055B8B8[5];
extern const char lbl_8055B8C0[2];
extern const char lbl_8055B8C4[6];
extern const char lbl_8055B8CC[5];
extern const char lbl_8055B8D4[6];
extern const char lbl_8055B8DC[7];
extern const char lbl_8055B8E4[4];
extern const char lbl_8055B8E8[7];
extern const char lbl_8055B8F0[7];
extern const char lbl_8055B8F8[4];
extern const char lbl_8055B8FC[3];
extern const char lbl_8055B900[6];
extern const char lbl_8055B908[8];
extern const char lbl_8055B910[7];
extern const char lbl_8055B918[5];
extern const char lbl_8055B920[5];
extern const char lbl_8055B928[5];
extern const char lbl_8055B930[7];
extern const char lbl_8055B938[6];
extern const char lbl_8055B940[7];
extern const char lbl_8055B948[7];
extern const char lbl_8055B950[8];
__declspec(section ".rodata") const UIStyleInputFields lbl_802F9F58 = {lbl_8033FC40,25.0f,20,{0,0,0,0,0,0,0,0,0,0,0},4,0,0,0,0xFFFFFFFF,0.0f,0.0f,0xFFFFFFFF};
__declspec(section ".data") const char lbl_8033FBE8[25] = "\x55\x49\x53\x74\x79\x6C\x65\x3A\x20\x74\x6F\x6F\x20\x6D\x61\x6E\x79\x20\x73\x74\x79\x6C\x65\x73";
__declspec(section ".data") const char lbl_8033FC08[56] = "\x75\x69\x53\x74\x79\x6C\x65\x3A\x20\x67\x78\x46\x6F\x6E\x74\x73\x20\x6E\x6F\x20\x6C\x6F\x6E\x67\x65\x72\x20\x65\x78\x69\x73\x74\x21\x20\x75\x73\x65\x20\x67\x46\x6F\x6E\x74\x73\x20\x61\x6E\x64\x20\x44\x72\x61\x77\x33\x44";
__declspec(section ".data") const char lbl_8033FC40[360] = "\x75\x69\x66\x6E\x74\x2E\x67\x74\x78\x00\x00\x00\x73\x74\x79\x6C\x65\x73\x2E\x76\x61\x6C\x00\x00\x73\x74\x79\x6C\x65\x5F\x69\x6E\x64\x65\x78\x00\x73\x74\x79\x6C\x65\x5F\x6E\x61\x6D\x65\x00\x00\x55\x49\x3A\x20\x73\x74\x79\x6C\x65\x20\x74\x61\x62\x6C\x65\x20\x68\x61\x73\x20\x6E\x6F\x20\x73\x74\x79\x6C\x65\x5F\x6E\x61\x6D\x65\x2C\x20\x73\x6B\x69\x70\x70\x69\x6E\x67\x00\x55\x49\x3A\x20\x73\x74\x79\x6C\x65\x20\x25\x73\x20\x61\x70\x70\x65\x61\x72\x73\x20\x74\x6F\x20\x62\x65\x20\x69\x6E\x76\x61\x6C\x69\x64\x2C\x20\x73\x6B\x69\x70\x70\x69\x6E\x67\x00\x00\x00\x00\x28\x75\x6E\x6E\x61\x6D\x65\x64\x29\x00\x00\x00\x66\x6F\x6E\x74\x5F\x6E\x61\x6D\x65\x00\x00\x00\x66\x6F\x6E\x74\x5F\x73\x69\x7A\x65\x00\x00\x00\x68\x6F\x72\x69\x7A\x6F\x6E\x74\x61\x6C\x5F\x61\x6C\x69\x67\x6E\x6D\x65\x6E\x74\x00\x00\x00\x00\x55\x49\x3A\x20\x75\x6E\x65\x78\x70\x65\x63\x74\x65\x64\x20\x68\x6F\x72\x69\x7A\x6F\x6E\x74\x61\x6C\x20\x61\x6C\x69\x67\x6E\x6D\x65\x6E\x74\x20\x27\x25\x73\x27\x2C\x20\x69\x67\x6E\x6F\x72\x69\x6E\x67\x00\x00\x76\x65\x72\x74\x69\x63\x61\x6C\x5F\x61\x6C\x69\x67\x6E\x6D\x65\x6E\x74\x00\x00\x62\x61\x73\x65\x6C\x69\x6E\x65\x00\x00\x00\x00\x55\x49\x3A\x20\x75\x6E\x65\x78\x70\x65\x63\x74\x65\x64\x20\x76\x65\x72\x74\x69\x63\x61\x6C\x20\x61\x6C\x69\x67\x6E\x6D\x65\x6E\x74\x20\x27\x25\x73\x27\x2C\x20\x69\x67\x6E\x6F\x72\x69\x6E\x67\x00\x00\x00\x00\x62\x75\x74\x74\x6F\x6E\x5F\x78\x00\x00\x00\x00\x62\x75\x74\x74\x6F\x6E\x5F\x79\x00\x00\x00";
__declspec(section ".data") const char lbl_8033FDA8[13] = "\x53\x54\x59\x4C\x45\x5F\x42\x55\x54\x54\x4F\x4E";
__declspec(section ".data") const char lbl_8033FDB8[15] = "\x53\x54\x59\x4C\x45\x5F\x43\x48\x45\x43\x4B\x42\x4F\x58";
__declspec(section ".data") const char lbl_8033FDC8[18] = "\x53\x54\x59\x4C\x45\x5F\x52\x41\x44\x49\x4F\x42\x55\x54\x54\x4F\x4E";
__declspec(section ".data") const char lbl_8033FDDC[21] = "\x53\x54\x59\x4C\x45\x5F\x54\x52\x41\x43\x4B\x55\x49\x5F\x42\x55\x54\x54\x4F\x4E";
__declspec(section ".data") const char lbl_8033FDF4[12] = "\x53\x54\x59\x4C\x45\x5F\x49\x4E\x50\x55\x54";
__declspec(section ".data") const char lbl_8033FE00[14] = "\x53\x54\x59\x4C\x45\x5F\x4E\x55\x4D\x45\x52\x49\x43";
__declspec(section ".data") const char lbl_8033FE10[14] = "\x53\x54\x59\x4C\x45\x5F\x43\x44\x45\x54\x45\x43\x54";
__declspec(section ".data") const char lbl_8033FE20[14] = "\x53\x54\x59\x4C\x45\x5F\x4C\x49\x53\x54\x42\x4F\x58";
__declspec(section ".data") const char lbl_8033FE30[12] = "\x53\x54\x59\x4C\x45\x5F\x4C\x41\x42\x45\x4C";
__declspec(section ".data") const char lbl_8033FE3C[13] = "\x53\x54\x59\x4C\x45\x5F\x52\x4C\x41\x42\x45\x4C";
__declspec(section ".data") const char lbl_8033FE4C[14] = "\x53\x54\x59\x4C\x45\x5F\x4D\x45\x53\x53\x41\x47\x45";
__declspec(section ".data") const char lbl_8033FE5C[19] = "\x53\x54\x59\x4C\x45\x5F\x4C\x4F\x4E\x47\x5F\x4D\x45\x53\x53\x41\x47\x45";
__declspec(section ".data") const char lbl_8033FE70[12] = "\x53\x54\x59\x4C\x45\x5F\x54\x49\x54\x4C\x45";
__declspec(section ".data") const char lbl_8033FE7C[18] = "\x53\x54\x59\x4C\x45\x5F\x42\x49\x47\x43\x48\x45\x43\x4B\x42\x4F\x58";
__declspec(section ".data") const char lbl_8033FE90[12] = "\x53\x54\x59\x4C\x45\x5F\x4D\x55\x4C\x54\x49";
__declspec(section ".data") const char lbl_8033FE9C[14] = "\x53\x54\x59\x4C\x45\x5F\x53\x50\x49\x4E\x4E\x45\x52";
__declspec(section ".data") const char lbl_8033FEAC[20] = "\x53\x54\x59\x4C\x45\x5F\x43\x41\x52\x45\x54\x5F\x42\x55\x54\x54\x4F\x4E\x32";
__declspec(section ".data") const char lbl_8033FEC0[19] = "\x53\x54\x59\x4C\x45\x5F\x43\x41\x52\x45\x54\x5F\x42\x55\x54\x54\x4F\x4E";
__declspec(section ".data") const char lbl_8033FED4[14] = "\x53\x54\x59\x4C\x45\x5F\x42\x55\x54\x54\x4F\x4E\x32";
__declspec(section ".data") const char lbl_8033FEE8[16] = "\x53\x54\x59\x4C\x45\x5F\x43\x48\x41\x4C\x4C\x45\x4E\x47\x45";
__declspec(section ".data") const char lbl_8033FEF8[14] = "\x53\x54\x59\x4C\x45\x5F\x4F\x50\x54\x49\x4F\x4E\x53";
__declspec(section ".data") const char lbl_8033FF08[18] = "\x53\x54\x59\x4C\x45\x5F\x4E\x41\x4D\x45\x43\x48\x4F\x4F\x53\x45\x52";
__declspec(section ".data") const char lbl_8033FF1C[17] = "\x53\x54\x59\x4C\x45\x5F\x42\x54\x48\x5F\x4C\x4F\x43\x4B\x45\x44";
__declspec(section ".data") const char lbl_8033FF30[19] = "\x53\x54\x59\x4C\x45\x5F\x42\x55\x54\x54\x4F\x4E\x5F\x53\x4D\x41\x4C\x4C";
__declspec(section ".data") const char lbl_8033FF44[21] = "\x53\x54\x59\x4C\x45\x5F\x45\x4E\x44\x5F\x45\x56\x45\x5F\x42\x55\x54\x54\x4F\x4E";
__declspec(section ".data") const char lbl_8033FF5C[26] = "\x53\x54\x59\x4C\x45\x5F\x53\x54\x41\x52\x54\x5F\x53\x43\x52\x45\x45\x4E\x5F\x42\x55\x54\x54\x4F\x4E";
__declspec(section ".data") const char lbl_8033FF78[31] = "\x53\x54\x59\x4C\x45\x5F\x4D\x41\x58\x5F\x49\x53\x5F\x4E\x4F\x54\x5F\x41\x5F\x56\x41\x4C\x49\x44\x5F\x53\x54\x59\x4C\x45";
__declspec(section ".data") const UIStyleNameFields lbl_8033FF98[27] = {
 {0,lbl_8033FDA8+0},
 {1,lbl_8033FDB8+0},
 {2,lbl_8033FDC8+0},
 {3,lbl_8033FDDC+0},
 {4,lbl_8033FDF4+0},
 {5,lbl_8033FE00+0},
 {6,lbl_8033FE10+0},
 {7,lbl_8033FE20+0},
 {8,lbl_8033FE30+0},
 {9,lbl_8033FE3C+0},
 {10,lbl_8033FE4C+0},
 {11,lbl_8033FE5C+0},
 {12,lbl_8033FE70+0},
 {13,lbl_8033FE7C+0},
 {14,lbl_8033FE90+0},
 {15,lbl_8033FE9C+0},
 {16,lbl_8033FEAC+0},
 {17,lbl_8033FEC0+0},
 {18,lbl_8033FED4+0},
 {19,lbl_8033FEE8+0},
 {20,lbl_8033FEF8+0},
 {21,lbl_8033FF08+0},
 {22,lbl_8033FF1C+0},
 {23,lbl_8033FF30+0},
 {24,lbl_8033FF44+0},
 {25,lbl_8033FF5C+0},
 {26,lbl_8033FF78+0},
};
__declspec(section ".data") const char lbl_80340070[152] = "\x25\x64\x2C\x20\x25\x64\x2C\x20\x25\x64\x2C\x20\x25\x64\x00\x00\x66\x6F\x72\x65\x67\x72\x6F\x75\x6E\x64\x00\x00\x66\x6F\x72\x65\x67\x72\x6F\x75\x6E\x64\x5F\x68\x69\x67\x68\x6C\x69\x67\x68\x74\x65\x64\x00\x00\x62\x61\x63\x6B\x67\x72\x6F\x75\x6E\x64\x00\x00\x62\x61\x63\x6B\x67\x72\x6F\x75\x6E\x64\x5F\x68\x69\x67\x68\x6C\x69\x67\x68\x74\x65\x64\x00\x00\x74\x65\x78\x74\x5F\x65\x66\x66\x65\x63\x74\x00\x6D\x65\x6E\x75\x5F\x62\x75\x74\x74\x6F\x6E\x00\x63\x6F\x6C\x6F\x72\x5F\x63\x79\x63\x6C\x65\x00\x63\x6F\x6C\x6F\x72\x5F\x63\x79\x63\x6C\x65\x5F\x62\x6C\x61\x63\x6B\x5F\x6F\x75\x74\x6C\x69\x6E\x65\x00\x00";
ModuleTag lbl_8055EFC0;
UnkStatic123s lbl_80490950;
UIStyleRecordFields lbl_80490968[256];
UIStyleRecordFields lbl_80497D68;
__declspec(section ".sdata") const char lbl_8055B8B0[5] = "\x2E\x69\x6D\x67";
__declspec(section ".sdata") const char lbl_8055B8B8[5] = "\x2E\x61\x72\x74";
__declspec(section ".sdata") const char lbl_8055B8C0[2] = "\x58";
__declspec(section ".sdata") const char lbl_8055B8C4[6] = "\x73\x74\x79\x6C\x65";
__declspec(section ".sdata") const char lbl_8055B8CC[5] = "\x6C\x65\x66\x74";
__declspec(section ".sdata") const char lbl_8055B8D4[6] = "\x72\x69\x67\x68\x74";
__declspec(section ".sdata") const char lbl_8055B8DC[7] = "\x63\x65\x6E\x74\x65\x72";
__declspec(section ".sdata") const char lbl_8055B8E4[4] = "\x74\x6F\x70";
__declspec(section ".sdata") const char lbl_8055B8E8[7] = "\x6D\x69\x64\x64\x6C\x65";
__declspec(section ".sdata") const char lbl_8055B8F0[7] = "\x62\x6F\x74\x74\x6F\x6D";
__declspec(section ".sdata") const char lbl_8055B8F8[4] = "\x6F\x66\x66";
__declspec(section ".sdata") const char lbl_8055B8FC[3] = "\x6F\x6E";
__declspec(section ".sdata") const char lbl_8055B900[6] = "\x73\x74\x61\x6D\x70";
__declspec(section ".sdata") const char lbl_8055B908[8] = "\x65\x78\x74\x5F\x61\x72\x74";
__declspec(section ".sdata") const char lbl_8055B910[7] = "\x62\x75\x74\x74\x6F\x6E";
__declspec(section ".sdata") const char lbl_8055B918[5] = "\x2E\x66\x6E\x74";
__declspec(section ".sdata") const char lbl_8055B920[5] = "\x2E\x67\x74\x78";
__declspec(section ".sdata") const char lbl_8055B928[5] = "\x30\x78\x25\x78";
__declspec(section ".sdata") const char lbl_8055B930[7] = "\x6E\x6F\x72\x6D\x61\x6C";
__declspec(section ".sdata") const char lbl_8055B938[6] = "\x70\x75\x6C\x73\x65";
__declspec(section ".sdata") const char lbl_8055B940[7] = "\x73\x68\x61\x64\x6F\x77";
__declspec(section ".sdata") const char lbl_8055B948[7] = "\x6F\x66\x66\x73\x65\x74";
__declspec(section ".sdata") const char lbl_8055B950[8] = "\x25\x64\x2C\x25\x64";
UnkStatic7F lbl_8055EFC8;










}
