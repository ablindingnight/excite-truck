#ifndef UICONTEXT_H
#define UICONTEXT_H
// The UI draw context (uictx.cpp produces it; widgets/gtext/uidraw/widgetwin read it). Layout from the matched
// producer; the scalar at 0x34 is still unnamed and distinct from scale at 0x38.
struct UIXform {float m[3][3];float pos[3];};
class UIContext {
public:
 static void* operator new(unsigned long,const char*);
 static void operator delete(void*);
 UIXform xf;
 float alpha;
 float unk34;
 float scale;
};
typedef char UIXformSize48[sizeof(UIXform)==48?1:-1];
typedef char UIContextSize60[sizeof(UIContext)==60?1:-1];
extern "C" { UIContext* fn_8022EDC4(void); }
#endif
