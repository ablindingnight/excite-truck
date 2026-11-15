#ifndef GAME_ROLL_STRINGS_H
#define GAME_ROLL_STRINGS_H

// These inline definitions keep the widgets' messages in their retail data order (after the vtables).
struct RollStrings {  /* Helper name unproven */
    static const char* InvalidState(void) { return "JoinLeaveWidget: invalid state %d found on focus group #%d"; }
    static const char* ColorKey(void) { return "UICar:COLOR%d"; }
    static const char* StatFont(void) { return "uifnt_o.gtx"; }
    static const char* StatKey(void) { return "UICar:STAT_%i_TEXT_%i"; }
};

#endif
