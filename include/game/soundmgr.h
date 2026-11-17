#ifndef GAME_SOUNDMGR_H
#define GAME_SOUNDMGR_H

#include "game/common.h"
#include "game/bgobject.h"
#include "game/listener.h"

class Mixer;
class GameSound;
class Sound;

// A list of game sounds (the manager's 'all' list and one per sound class).
struct SoundList {
    SoundList(void);
    GameSound** first;  // offset 0x0
    s32 count;          // offset 0x4
};

// The sound manager (soundmgr.cpp): a background object that each frame matches the game sounds (sound.cpp) to the
// mixer's voices by class budget and listener distance. Names are guesses.
class SoundManager : public BgObject {
public:
    SoundManager(Mixer* mixer);
    virtual ~SoundManager(void);
    virtual void Update(void);

    int Throttle(void);
    void Process(void);
    void ClearOneShots(void);
    void ReapOneShots(void);
    void Allocate(void);
    void CheckPlaying(void);
    void SortClass(SoundList* l);
    void SortClasses(void);
    void Check(void);
    void Add(GameSound* s);
    void Add2(GameSound* s);
    void Add3(GameSound* s);
    int IsPositional(int cls);
    void Remove(GameSound* s);
    Sound* Create(void* desc, int cls);
    Sound* Create2(void* desc, int cls);
    Sound* Create3(void* desc, int cls);
    void ReleaseSound(Sound* s);
    void SetListener(const Listener& l, int i);
    void SetListenerCount(s32 n);

    u8 enabled;                 // offset 0x4
    u8 primaryOnly;             // offset 0x5
    s32 lastUpdate;             // offset 0x8
    SoundList all;              // offset 0xC
    SoundList classes[9];       // offset 0x14
    Listener listeners[4];      // offset 0x5C
    s32 numListeners;           // offset 0x8C
    Mixer* mixer;               // offset 0x90
    GameSound* oneShots[0x20];  // offset 0x94
    s32 numOneShots;            // offset 0x114
};

#endif
