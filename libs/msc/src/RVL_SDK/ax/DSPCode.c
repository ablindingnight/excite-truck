#include <revolution/ax.h>

// DUMP_DATA axDspSlave u8
u8 axDspSlave[] = {
#include "dumpdata/axDspSlave.inc"
};

u16 axDspInitVector = 0x0010;
u16 axDspResumeVector = 0x0031;
u16 axDspSlaveLength = ARRAY_SIZE(axDspSlave);
