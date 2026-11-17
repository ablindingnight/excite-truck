// Road sounds (0x8020CA30-0x8020CF68): per-surface rolling sounds from roadsnd.val, blended by
// speed with the surface under the car's wheels. Names are guesses apart from the keys and messages.
#include "game/common.h"
#include "game/carbody.h"
#include "game/mathutil.h"

extern "C" int strcmp(const char* a, const char* b);


class RoadSoundVoice {
public:
    void SetVolume(f32 x) {
        if (x != volume) {
            volumeChanged = 1;
            volume = x;
        }
    }
    void SetPitch(f32 x) {
        if (x != pitch) {
            pitchChanged = 1;
            pitch = x;
        }
    }
    void Silence(void) {
        unk4A = 1;
        unk4C = 0;
    }
    virtual ~RoadSoundVoice(void);

    f32 pitch;   // offset 0x4
    f32 volume;  // offset 0x8
    u8 padC[0x3D];
    u8 unk49;          // offset 0x49
    u8 unk4A;          // offset 0x4A
    u8 unk4B;          // offset 0x4B
    u8 unk4C;          // offset 0x4C
    u8 volumeChanged;  // offset 0x4D
    u8 pitchChanged;   // offset 0x4E
};

struct SurfaceInfo {
    u8 pad0[0x10];
    s32 type;  // offset 0x10
};

void* operator new(size_t size, const char* tag);

extern "C" {
void fn_802E30C8(const char* fmt, ...);  // debug print
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* table);
RoadSoundVoice* fn_802077A4(const char* name, int flags, void* a, void* b);
SurfaceInfo* fn_801D04A8(int surface);
}

static inline f32 Clamp01(f32 t) { return MaxF(MinFRev(t, 1.0f), 0.0f); }

struct RoadSoundEntry {
    RoadSoundVoice* voice;  // offset 0x0
    s32 surface;            // offset 0x4
    f32 speed1;             // offset 0x8
    f32 speed2;             // offset 0xC
    f32 speed3;             // offset 0x10
    f32 vol2;               // offset 0x14
    f32 vol3;               // offset 0x18
    f32 pitch1;             // offset 0x1C
    f32 pitch3;             // offset 0x20
};

static char RoadSoundTag[] = "roadsnd";
static char RoadSoundType[] = "sound";

class RoadSound {
    // The inline definitions emit the constructor strings in retail order.
    static const char* Path() { return "roadsnd.val"; }  /* Helper name unproven */
    static const char* SurfaceKey() { return "surftype"; }  /* Helper name unproven */
    static const char* MissingVoiceFormat() { return "roadsnd: can't find %s"; }  /* Helper name unproven */

public:
    RoadSound(char* car) {
        RoadSoundEntry* e;
        const char* name;
        char* c;
        ValueTable* table;
        ValueNode* node;
        int i;
        this->car = car;
        count = 0;
        table = fn_80131808(Path());
        for (i = 0; (node = table->GetChild(i)) != NULL; i++) {
            if (strcmp(node->GetName(), RoadSoundType) == 0) {
                c = this->car;
                e = &entries[count++];
                name = node->GetValue("sound");
                e->voice = fn_802077A4(name, 4, c + 0x118, c + 0x60);
                if (e->voice == NULL && ((CarBody*)c)->index == 0) {
                    fn_802E30C8(MissingVoiceFormat(), name);
                }
                e->surface = node->GetIntValue(SurfaceKey());
                e->speed1 = 0.44444445f * node->GetFloat("speed1");
                e->speed2 = 0.44444445f * node->GetFloat("speed2");
                e->speed3 = 0.44444445f * node->GetFloat("speed3");
                e->vol2 = node->GetFloat("vol2");
                e->vol3 = node->GetFloat("vol3");
                e->pitch1 = node->GetFloat("pitch1");
                e->pitch3 = node->GetFloat("pitch3");
            }
        }
        fn_80131818(table);
    }
    virtual ~RoadSound(void);
    virtual void Update(void);

    char* car;                    // offset 0x4
    int count;                    // offset 0x8
    RoadSoundEntry entries[8];    // offset 0xC
};

ModuleTag lbl_8055EE60;

extern "C" {

void fn_8020CA30(void) {}

void fn_8020CA34(void) {}

void fn_8020CA38(void) {}

RoadSound* fn_8020CA3C(char* car) { return new (RoadSoundTag) RoadSound(car); }

}  // extern "C"

RoadSound::~RoadSound(void) {
    for (int i = 0; i < count; i++) {
        if (entries[i].voice) {
            delete entries[i].voice;
        }
    }
}

static inline f32 Ramp_hm(f32 x, f32 lo, f32 hi) { return Clamp01((x - lo) / (hi - lo)); }  /* Helper name unproven */
static inline f32 BlendRoadVolume(f32 speed, f32 lo, f32 hi, f32 firstVol, f32 lastVol, f32 firstSpeed, f32 lastSpeed) {
    return LerpT(MaxF(MinFRev((speed - firstSpeed) / (lastSpeed - firstSpeed), hi), lo), firstVol, lastVol);
}

void RoadSound::Update(void) {
    char* c = car;
    f32 hi;
    f32 speed;
    f32 gain;
    f32 lo;
    int surface = 0;
    for (int w = 0; w < 4; w++) {
        s32 s = ((CarBody*)c)->wheels[w].groundH;
        surface = s > surface ? s : surface;
    }
    gain = 0.25f * ((CarBody*)c)->airborne;
    speed = ((CarBody*)c)->speed;
    hi = 1.0f;
    lo = 0.0f;
    for (int i = 0; i < count; i++) {
        int type = fn_801D04A8(surface)->type;
        if (type == entries[i].surface) {
            RoadSoundVoice* v = entries[i].voice;
            if (v) {
                f32 vol = speed < entries[i].speed2
                              ? BlendRoadVolume(speed, lo, hi, 0.0f, entries[i].vol2,
                                                entries[i].speed1, entries[i].speed2)
                              : BlendRoadVolume(speed, lo, hi, entries[i].vol2, entries[i].vol3,
                                                entries[i].speed2, entries[i].speed3);
                f32 pitch = LerpT(Ramp_hm(speed, entries[i].speed1, entries[i].speed3), entries[i].pitch1, entries[i].pitch3);
                v->unk4A = 0;
                v->unk4B = 1;
                v->unk49 = 1;
                entries[i].voice->SetVolume(gain * vol);
                entries[i].voice->SetPitch(pitch);
            }
        } else if (entries[i].voice) {
            entries[i].voice->Silence();
        }
    }
}
