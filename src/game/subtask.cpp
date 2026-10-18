// Sub-task manager stub (0x8013C9F4-0x8013CAD8): factory for the default ("csubtasktmgr") manager,
// whose virtual methods all do nothing. Names are guesses apart from the allocation tag.
#include "game/common.h"

void* operator new(size_t size, const char* tag);

class CSubTaskMgr {
public:
    virtual ~CSubTaskMgr(void);
    virtual int vf0C(void);
    virtual int vf10(void);
    virtual int vf14(void);
    virtual void vf18(void);
    virtual int vf1C(void);
    virtual void vf20(void);
    virtual void vf24(void);
    virtual int vf28(void);
};

ModuleTag lbl_8055E190;
UnkStaticFFFF lbl_8055E198;
static UnkStatic123 lbl_8042FB10;
UnkStatic7F lbl_8055E19C;
UnkStaticFFFF lbl_8055E1A0;

extern "C" {

CSubTaskMgr* fn_8013C9F4(int type) {
    if (type == 0) {
        return new ("csubtasktmgr") CSubTaskMgr;
    }
}

void fn_8013CA3C(void) {}

}  // extern "C"

CSubTaskMgr::~CSubTaskMgr(void) {}
int CSubTaskMgr::vf0C(void) { return 0; }
int CSubTaskMgr::vf14(void) { return 1; }
int CSubTaskMgr::vf1C(void) { return 0; }
int CSubTaskMgr::vf10(void) { return 0; }
void CSubTaskMgr::vf18(void) {}
void CSubTaskMgr::vf20(void) {}
void CSubTaskMgr::vf24(void) {}
int CSubTaskMgr::vf28(void) { return 0; }
