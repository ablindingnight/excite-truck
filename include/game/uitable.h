#ifndef GAME_UITABLE_H
#define GAME_UITABLE_H
// Shared actual owner710 declaration. Setter definitions remain out of line.
class UITable {
public:
    UITable(int rows, int cols, const int* widths);
    ~UITable(void);
    void Clear(void);
    char* At(int row, int col);
    char* Get(int row, int col, int* flag);
    void Set(int row, int col, const char* s, char flag);
    void SetVisible(int n);
    void SetPage(int p);
    void SetSel(int s);

    char* buf;     // offset 0x0
    signed long cols;      // offset 0x4
    signed long used;      // offset 0x8
    signed long rows;      // offset 0xC
    signed long visible;   // offset 0x10
    signed long rowSize;   // offset 0x14
    signed long* widths;   // offset 0x18
    signed long* offsets;  // offset 0x1C
    signed long page;      // offset 0x20
    signed long changes;   // offset 0x24
    unsigned long magic;     // offset 0x28
    signed long sel;       // offset 0x2C
};
typedef char UITableObservedSize48[sizeof(UITable)==48?1:-1];
#endif
