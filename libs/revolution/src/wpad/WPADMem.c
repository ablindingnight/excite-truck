#include "__wpad.h"

// WPAD memory-access control blocks (one per controller); the WPADMem functions themselves are not linked
// into this game, only the data (2026-11-10).
WPADMEMControlBlock _wmb[WPAD_MAX_CONTROLLERS];
