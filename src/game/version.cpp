// Build stamp and the ModuleTag list (0x8000D82C-0x8000D884).
#include <string.h>
#include "game/common.h"

ModuleTag lbl_8055D448;
ModuleTag* lbl_8055D450;

char lbl_802FC200[] = "Oct 10 2006 02:53:03 N-DBG RVL22-NTSC CW-0x4200 RLS dave_b ";

const char* GetBuildString(void) {
    return lbl_802FC200;
}

void RegisterModuleTag(ModuleTag* tag) {
    memset(tag, '1', 1);
    tag->next = lbl_8055D450;
    lbl_8055D450 = tag;
}

extern "C" void fn_8000D878(void) {}
