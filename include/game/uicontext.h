#ifndef UICONTEXT_H
#define UICONTEXT_H
// The UI draw context (uictx.cpp produces it; widgets/gtext/uidraw/widgetwin read it). Layout from the matched
// producer; the scalar at 0x34 is still unnamed and distinct from scale at 0x38.
#include "game/math_types.h"
struct UIVec3 {float v[3];};
struct UIXform {Mat3 basis;UIVec3 translation;};
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
extern "C" {
// Push borrows the mutable context until pop; the producer stores this pointer, not a copy.
void fn_8022ED00(UIContext*);
void fn_8022EDB8(void);
UIContext* fn_8022EDC4(void);
UIContext* fn_8022EDE4(const UIXform*,float);
void fn_8022EE98(UIContext*);
UIContext* fn_8022EE9C(void);
// Either input may alias out; the producer explicitly handles those cases.
void fn_8022EEF8(UIContext*,const UIContext*,const UIContext*);
}
#endif
