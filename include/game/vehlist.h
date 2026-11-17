#ifndef GAME_VEHLIST_H
#define GAME_VEHLIST_H
// The vehicle list an event fills (pathcareer.h's users, evpath): add a vehicle, give a reason it is locked, count.
// Names unproven.

class VehicleList {
public:
    virtual ~VehicleList(void);                                    // 0x8
    virtual void Add(const char* name, int unlocked, int special);  // 0xC
    virtual void SetReason(int i, const char* reason);             // 0x10
    virtual int Count(void);                                       // 0x14
};

#endif
