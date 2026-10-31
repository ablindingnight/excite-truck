#ifndef GAME_GTEXT_H
#define GAME_GTEXT_H
// Shared raw font record and APIs from the native gtext producer. Glyph remains private.
struct Glyph;
struct Font {
    long tex;
    unsigned char height,b5,b6;
    Font* outline;
    unsigned short count;
    Glyph* glyphs;
    unsigned long unk14;
    char* texName;
};
typedef char FontSize28[sizeof(Font)==0x1C?1:-1];
extern "C" {
Font* fn_80232578(const char*);
void fn_802328D4(Font*);
void fn_80232928(unsigned long);
float fn_8023293C(Font*,const char*,float);
int fn_80232B70(Font*);
int fn_80232B78(Font*);
void fn_80232BA4(const char*,Font*,float,float,unsigned long,int,float,float,float);
void fn_80233354(const char*,Font*,unsigned long,int,float,float,float,float);
void fn_80233600(const char*,Font*,unsigned long,int,float,float,float);
void fn_802336AC(Font*,unsigned long,int,const char*,float,float,float,...);
}
#endif
