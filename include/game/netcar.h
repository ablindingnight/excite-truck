#ifndef GAME_NETCAR_H
#define GAME_NETCAR_H
// A network-driven car (netcar.cpp; phob creates them): it extrapolates and blends toward the packets it
// receives. Names unproven.
#include "game/common.h"
#include "game/carbody.h"

struct Packet;
struct Blend;
class Link;
struct BodyDesc;

struct ZVec {
    ZVec(void);
    f32 x, y, z;
};

class NetCar : public CarBody {
public:
    NetCar(void);
    virtual void Init(BodyDesc* desc);
    virtual ~NetCar(void);
    virtual void Reset(void);
    virtual void Update(void);
    void Clear(void);
    int Lost(void) const;
    void Start(void);
    void Extrapolate(Blend* b);
    void Snap(void);
    void Rotate(Blend* b);
    void Predict(void);
    void GetBlend(Blend* b);

    Packet* ptab;     // offset 0x2700
    s32 state;        // offset 0x2704
    ZVec v[3];        // offset 0x2708
    ZVec acc;         // offset 0x272C
    f32 t;            // offset 0x2738
    f32 dt;           // offset 0x273C
    u8 waiting;       // offset 0x2740
    u8 first;         // offset 0x2741
    RMtx home;        // offset 0x2744
    Link* linkObj;    // offset 0x2774
    s32 lastLink;     // offset 0x2778
    s32 time;         // offset 0x277C
};

#endif
