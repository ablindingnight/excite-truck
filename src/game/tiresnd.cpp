// Tire sounds (0x80209D4C-0x80209FFC): the squeal voice, loud with the wheels' slip past a
// threshold, plus the car's road sounds. Names are guesses apart from the file names.
#include "game/common.h"

class TireVoice {
public:
    virtual ~TireVoice(void);
    void SetPitch(f32 p) {
        if (p != pitch) {
            pitchChanged = 1;
            pitch = p;
        }
    }
    void SetVolume(f32 v) {
        if (v != volume) {
            volumeChanged = 1;
            volume = v;
        }
    }
    void Play(void) {
        unk4A = 0;
        unk4B = 1;
        unk49 = 1;
    }
    void Stop(void) {
        unk4A = 1;
        unk4C = 0;
    }

    f32 pitch;   // offset 0x4
    f32 volume;  // offset 0x8
    u8 padC[0x4];
    s32 unk10;   // offset 0x10
    u8 pad14[0x35];
    u8 unk49;          // offset 0x49
    u8 unk4A;          // offset 0x4A
    u8 unk4B;          // offset 0x4B
    u8 unk4C;          // offset 0x4C
    u8 volumeChanged;  // offset 0x4D
    u8 pitchChanged;   // offset 0x4E
};

class RoadSound {
public:
    virtual ~RoadSound(void);
    virtual void Update(void);
};

void* operator new(size_t size, const char* tag);

extern "C" {
TireVoice* fn_802077A4(const char* name, int flags, void* a, void* b);
RoadSound* fn_8020CA3C(char* car);
}

struct Wheel {
    u8 pad0[0x68];
    f32 slip;  // offset 0x68
    s8 air;    // offset 0x6C
    u8 pad6D[0x7];
};

struct Car {
    u8 pad0[0x23AC];
    Wheel wheels[4];  // offset 0x23AC
};

class TireSound {
public:
    TireSound(char* car) {
        this->car = car;
        squeal = fn_802077A4("squeal.sfx", 4, car + 0x118, car + 0x60);
        if (squeal) {
            squeal->unk10 = *(s32*)(this->car + 0x1EC);
        }
        road = fn_8020CA3C(car);
    }
    virtual ~TireSound(void);
    virtual void Update(void);

    char* car;          // offset 0x4
    TireVoice* squeal;  // offset 0x8
    RoadSound* road;    // offset 0xC
};

ModuleTag lbl_8055EDC0;

extern "C" TireSound* fn_80209D4C(char* car) { return new ("tiresnd") TireSound(car); }

TireSound::~TireSound(void) {
    if (squeal) {
        delete squeal;
        squeal = NULL;
    }
    if (road) {
        delete road;
    }
}

void TireSound::Update(void) {
    Car* c = (Car*)car;
    f32 sum = 0.0f;
    Wheel* w = c->wheels;
    for (int i = 0; i < 4; i++, w++) {
        f32 s = w->slip - 0.8f;
        if (w->air == 0 && s > 0.0f) {
            if (0.25f < s) {
                s = 0.25f;
            }
            sum += s;
        }
    }
    squeal->SetPitch(1.0f);
    squeal->SetVolume(sum);
    if (sum > 0.001f) {
        squeal->Play();
    } else {
        squeal->Stop();
    }
    if (road) {
        road->Update();
    }
}
