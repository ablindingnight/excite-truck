#ifndef GAME_SWFANIM_H
#define GAME_SWFANIM_H
// A loaded flash-style animation (swfanim.cpp; widgets' animation widget owns one). Builtin types only, so the
// raw-typedef widgets.cpp can include it (u32 = unsigned long, u8 = unsigned char). Names unproven.

struct Sw2;
struct Sw2Item;

class SwfAnim {
public:
    void SwapShapeData(unsigned long i);
    SwfAnim();
    ~SwfAnim();
    void DrawItem(const Sw2Item* it);
    void DrawFrame(unsigned long frame);
    void Reset();
    void LoadFile(const char* name);
    void Update();
    void Draw();
    void Load(const char* name);
    void Unload();

    unsigned long lastTime;  // offset 0x0
    unsigned char playing;   // offset 0x4
    unsigned char loop;      // offset 0x5
    unsigned long frame;     // offset 0x8
    unsigned long first;     // offset 0xC
    unsigned long last;      // offset 0x10
    Sw2* data;               // offset 0x14
};

#endif
