// Stream banks (0x802E7578-0x802E7770): the global stream-bank manager and its forwarders; this build only
// ever creates the "null" manager. Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
void fn_802E30C8(const char* fmt, ...);  // debug print
int OSGetConsoleType(void);
}

void* operator new(size_t size, const char* tag);

class StrmBank {
public:
    virtual ~StrmBank(void) {}
    virtual int LoadBank(s32 a) = 0;
    virtual void UnloadBank(s32 a) = 0;
    virtual void vf14(s32 a, s32 b, s32 c) = 0;
    virtual void vf18(void) = 0;
    virtual void vf1C(void) = 0;
    virtual void vf20(s32 a) = 0;
    virtual int vf24(void) = 0;
    virtual int vf28(void) = 0;
    virtual void vf2C(void) = 0;
    virtual void vf30(void) = 0;
    virtual void vf34(void) = 0;
};

class NullStrmBank : public StrmBank {
public:
    virtual ~NullStrmBank(void);
    virtual int LoadBank(s32 a);
    virtual void UnloadBank(s32 a);
    virtual void vf14(s32 a, s32 b, s32 c);
    virtual void vf18(void);
    virtual void vf1C(void);
    virtual void vf20(s32 a);
    virtual int vf24(void);
    virtual int vf28(void);
    virtual void vf2C(void);
    virtual void vf30(void);
    virtual void vf34(void);
};

ModuleTag lbl_8055FFE8;
StrmBank* lbl_8055FFF0;

extern "C" {

StrmBank* fn_802E769C(void);

void fn_802E7578(void) { lbl_8055FFF0 = fn_802E769C(); }

void fn_802E759C(void) {
    if (lbl_8055FFF0) {
        delete lbl_8055FFF0;
        lbl_8055FFF0 = 0;
    }
}

int fn_802E75E4(s32 a) { return lbl_8055FFF0->LoadBank(a); }

void fn_802E75FC(s32 a) {
    fn_802E30C8("+StrmUnloadBank");
    lbl_8055FFF0->UnloadBank(a);
    fn_802E30C8("-StrmUnloadBank");
}

void fn_802E765C(s32 a, s32 b, s32 c) { lbl_8055FFF0->vf14(a, b, c); }

void fn_802E7684(s32 a) { lbl_8055FFF0->vf20(a); }

StrmBank* fn_802E769C(void) {
    OSGetConsoleType();
    fn_802E30C8("StrmBank: creating 'null' manager");
    return new ("NullStrmBank") NullStrmBank;
}

}  // extern "C"

NullStrmBank::~NullStrmBank(void) {}

int NullStrmBank::LoadBank(s32 a) { return 1; }

void NullStrmBank::UnloadBank(s32 a) {}

void NullStrmBank::vf14(s32 a, s32 b, s32 c) {}

void NullStrmBank::vf18(void) {}

void NullStrmBank::vf1C(void) {}

void NullStrmBank::vf20(s32 a) {}

int NullStrmBank::vf24(void) { return 0; }

int NullStrmBank::vf28(void) { return 0; }

void NullStrmBank::vf2C(void) {}

void NullStrmBank::vf30(void) {}

void NullStrmBank::vf34(void) {}
