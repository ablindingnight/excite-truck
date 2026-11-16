#include <math.h>
#include "revolution/hbm/nw4hbm/math.h"
#include "revolution/hbm/nw4hbm/snd/Util.h"
#include "revolution/hbm/nw4hbm/ut.h"

namespace nw4hbm {
namespace snd {
namespace detail {

f32 Util::CalcPitchRatio(int pitch) {
    int octave = 0;
    f32 octaveFloat = 1.0f;

    while (pitch < 0) {
        octave--;
        pitch += SEMITONE_MAX * MICROTONE_MAX;
    }

    while (pitch >= SEMITONE_MAX * MICROTONE_MAX) {
        octave++;
        pitch -= SEMITONE_MAX * MICROTONE_MAX;
    }

    while (octave > 0) {
        octaveFloat *= 2.0f;
        octave--;
    }

    while (octave < 0) {
        octaveFloat /= 2.0f;
        octave++;
    }

    int note = pitch / MICROTONE_MAX;
    pitch %= MICROTONE_MAX;

    f32 ratio = octaveFloat;

    if (note != 0) {
        ratio *= NoteTable[note];
    }
    if (pitch != 0) {
        ratio *= PitchTable[pitch];
    }

    return ratio;
}

f32 Util::CalcVolumeRatio(f32 db) {
    db = ut::Clamp(db, VOLUME_MIN_DB, VOLUME_MAX_DB);
    return Decibel2RatioTable[-VOLUME_MIN + static_cast<int>(10 * db)];
}

f32 Util::CalcPanRatio(f32 pan) {
    pan = (ut::Clamp(pan, -1.0f, 1.0f) + 1.0f) / 2.0f;
    return Pan2RatioTable[static_cast<int>(pan * PAN_TABLE_MAX + 0.5f)];
}

f32 Util::CalcSurroundPanRatio(f32 surroundPan) {
    surroundPan = ut::Clamp(surroundPan, 0.0f, 2.0f) / 2.0f;
    return Pan2RatioTable[static_cast<int>(surroundPan * PAN_TABLE_MAX + 0.5f)];
}

int Util::CalcLpfFreq(f32 scale) {
    scale = ut::Clamp(scale, 0.0f, 1.0f);
    return static_cast<int>(32000 * pow(2.0, 10.0 * (scale - 1.0)));
}

u16 Util::CalcRandom() {
    static int u = 0x12345678;
    u = u * 0x19660D + 0x3C6EF35F; // randq1 :D
    return u >> 16;
}

const void* Util::GetDataRefAddressImpl(RefType type, u32 value, const void* base) {
    if (type == REFTYPE_OFFSET) {
        return ut::AddOffsetToPtr(base, Util::ReadBigEndian(value));
    }

    if (type == REFTYPE_ADDRESS) {
        return reinterpret_cast<void*>(Util::ReadBigEndian(value));
    }

    NW4HBMPanicMessage_Line(502, "invalid DataRef::RefType");
    return NULL;
}

const f32 Util::NoteTable[SEMITONE_MAX] = {1.0000000f, 1.0594631f, 1.1224620f, 1.1892071f, 1.2599211f, 1.3348398f,
                                           1.4142135f, 1.4983071f, 1.5874010f, 1.6817929f, 1.7817974f, 1.8877486f};

// DUMP_DATA PitchTable__Q46nw4hbm3snd6detail4Util f32
const f32 Util::PitchTable[MICROTONE_MAX] = {
#include "dumpdata/PitchTable__Q46nw4hbm3snd6detail4Util.inc"
};

// DUMP_DATA Decibel2RatioTable__Q46nw4hbm3snd6detail4Util f32
const f32 Util::Decibel2RatioTable[VOLUME_TABLE_SIZE] = {
#include "dumpdata/Decibel2RatioTable__Q46nw4hbm3snd6detail4Util.inc"
};

// DUMP_DATA Pan2RatioTable__Q46nw4hbm3snd6detail4Util f32
const f32 Util::Pan2RatioTable[PAN_TABLE_SIZE] = {
#include "dumpdata/Pan2RatioTable__Q46nw4hbm3snd6detail4Util.inc"
};
} // namespace detail
} // namespace snd
} // namespace nw4hbm
