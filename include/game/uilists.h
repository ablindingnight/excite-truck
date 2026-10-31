#ifndef GAME_UILISTS_H
#define GAME_UILISTS_H
// Shared list models from the matched UITable producer. This does not establish new runtime bounds or allocation extents.
class CycleList {
public:
    CycleList(const char** names, const signed long* values, int count, int a);
    ~CycleList(void);
    const char* Name(void);
    signed long Value(void);
    void Next(void);
    void Prev(void);
    void Set(int i);
    const char* NameAt(int i);

    const char** names;  // offset 0x0
    const signed long* values;   // offset 0x4
    signed long first;           // offset 0x8
    signed long cur;             // offset 0xC
    signed long count;           // offset 0x10
    signed long unk14;           // offset 0x14
    unsigned char unk18;            // offset 0x18
};

class NameList {
public:
    NameList(const char** src, int n, int a);
    ~NameList(void);
    int Count(void);
    int Unk44(void);
    char* Name(int i);

    char names[4][0x10];  // offset 0x0
    signed long count;            // offset 0x40
    signed long unk44;            // offset 0x44
};
typedef char CycleListSize28[sizeof(CycleList)==28?1:-1];
typedef char NameListObservedPrefix72[sizeof(NameList)==72?1:-1];
#endif
