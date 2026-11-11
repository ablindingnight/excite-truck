#include "abort_exit.h"
#include "signal.h"
#include "stdlib.h"

void (*__stdio_exit)(void);
#ifndef GAME_REXE01  // abort()/__aborting are not linked into Excite Truck (its unit is data-only)
int __aborting;

void abort(void)
{
    raise(1);
    __aborting = 1;
    exit(1);
}
#endif
