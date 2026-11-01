#ifndef GAME_STRINGLIST_H
#define GAME_STRINGLIST_H
// Actual producer fields. count is capacity; used+8 is live entries.
// added+12 keeps the existing producer name; no new initialization guarantee.
class StringList {
public:
    char* Get(int i, int* flag);

    signed long count;     // offset 0x0
    signed long len;       // offset 0x4
    signed long used;      // offset 0x8
    signed long added;     // offset 0xC
    char** items;  // offset 0x10
};
typedef char StringListObservedSize20[sizeof(StringList)==20?1:-1];
typedef char StringListUsedOffset8[((unsigned long)&((StringList*)0)->used)==8?1:-1];
typedef char StringListAddedOffset12[((unsigned long)&((StringList*)0)->added)==12?1:-1];
#endif
