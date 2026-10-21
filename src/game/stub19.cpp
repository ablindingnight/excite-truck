// A stripped unit (0x8023995C-0x802399AC): no code left, only its statics. Names are guesses.
#include "game/common.h"

#pragma force_active on
ModuleTag lbl_8055F0F0;
static UnkStatic123 lbl_804986E0;
UnkStatic7F lbl_8055F0F8;
#pragma force_active reset

// Nothing references this object, so the linker would strip it (its sinit is local, so the
// ldscript FORCEACTIVE entry is ignored); force_active on its statics keeps it.
