// Bink, AX sound output and DVD file reader: code 0x80119214-0x8011AE18. Split from game/unk_80116BD4.c on
// 2026-11-16: retail's .sdata2 repeats the int-to-float bias constant at 0x805611A8 and 0x805611D8, so the constants
// 0x80561198-0x805611D8 (Volume, Pan, Open) are another object's pool than BinkOpen's and the audio decoder's.
// Unit name is provisional. CLEANUP: automated-decompiler source; rewrite with real types before the PC port.
#include "game/bink_internal.h"

void fn_80119214(void *arg0, void *arg1) {
    fn_8011FFB8(arg0, arg1);
}

static void SetStreamVolumePan(void *arg0, s32 arg1, s32 arg2, s32 arg3) {
    void *temp_r31 = (u8*)arg0 + 0x14C;
    memset(temp_r31, 0, 0x30);
    *(s16*)temp_r31 = arg2;
    *(s16*)((u8*)temp_r31 + 4) = arg3;
    *(s16*)((u8*)arg0 + 0x1AC) = arg1;
    *(s16*)((u8*)arg0 + 0x1AE) = 0;
    *(s32*)((u8*)arg0 + 0x144) = 3;
    *(s32*)((u8*)arg0 + 0x1C) |= 0x212;
}

static void set_rate_volume_and_pan(void *arg0) {
    if (*(void **)((u8 *)arg0 + 0x7C) != 0) {
        SetStreamVolumePan(*(void **)((u8 *)arg0 + 0x7C), *(s32 *)((u8 *)arg0 + 0xA4), *(s32 *)((u8 *)arg0 + 0xA8),
                    *(s32 *)((u8 *)arg0 + 0x78) == 1 ? *(s32 *)((u8 *)arg0 + 0xAC) : 0);
    }
    if (*(void **)((u8 *)arg0 + 0x80) != 0) {
        SetStreamVolumePan(*(void **)((u8 *)arg0 + 0x80), *(s32 *)((u8 *)arg0 + 0xA4), 0, *(s32 *)((u8 *)arg0 + 0xAC));
    }
}

static void clear(void *arg0, u32 arg1, u32 arg2) {
    u32 len;
    u32 n;
    len = arg2;
    if (arg2 + arg1 > *(u32 *)((u8 *)arg0 + 0x18)) {
        len = *(u32 *)((u8 *)arg0 + 0x18) - arg1;
    }
    if (*(s32 *)((u8 *)arg0 + 4) != 0) {
        arg1 >>= 1;
        n = len >> 1;
        memset(*(u8 **)((u8 *)arg0 + 8) + arg1, 0, n);
        DCFlushRange(*(u8 **)((u8 *)arg0 + 8) + arg1, n);
        memset(*(u8 **)((u8 *)arg0 + 0xC) + arg1, 0, n);
        DCFlushRange(*(u8 **)((u8 *)arg0 + 0xC) + arg1, n);
        if (len < arg2) {
            n = (arg2 - len) >> 1;
            memset(*(u8 **)((u8 *)arg0 + 8), 0, n);
            DCFlushRange(*(u8 **)((u8 *)arg0 + 8), n);
            memset(*(u8 **)((u8 *)arg0 + 0xC), 0, n);
            DCFlushRange(*(u8 **)((u8 *)arg0 + 0xC), n);
        }
    } else {
        memset(*(u8 **)((u8 *)arg0 + 8) + arg1, 0, len);
        DCFlushRange(*(u8 **)((u8 *)arg0 + 8) + arg1, len);
        if (len < arg2) {
            n = arg2 - len;
            memset(*(u8 **)((u8 *)arg0 + 8), 0, n);
            DCFlushRange(*(u8 **)((u8 *)arg0 + 8), n);
        }
    }
}

static void set_both_state(void *arg0, s32 arg1) {
    void *v;
    if ((v = *(void **)arg0) != NULL) {
        AXSetVoiceState(v, arg1);
    }
    if ((v = *(void **)((u8 *)arg0 + 4)) != NULL) {
        AXSetVoiceState(v, arg1);
    }
}

static void fn_80119498(void *arg0) {
    set_both_state(arg0, 1);
    *(s32 *)((u8 *)arg0 + 0x38) = 0;
}

static s32 get_pos(void* arg0) {
 return *(u16*)((u8*)arg0+0x1be) + (*(u16*)((u8*)arg0+0x1bc)<<16);
}

static s32 GoodGetCurrentPosition(void *arg0) {
    s32 first;
    s32 current;
    s32 result;
    do {
        first = get_pos(*(void **)arg0);
        get_pos(*(void **)arg0);
        result = get_pos(*(void **)arg0);
    } while ((u32)(result - first) >= 0x400);
    current = result * 2 - *(s32 *)((u8 *)arg0 + 0x10);
    if (*(s32 *)((u8 *)arg0 + 4) != 0)
        current *= 2;
    return current;
}

static void start_streaming(void *arg0) {
    *(s32 *)((u8 *)arg0 + 0x34) = 1;
    fn_80119498(arg0);
    *(s32 *)((u8 *)arg0 + 0x48) = GoodGetCurrentPosition(arg0);
    *(s32 *)((u8 *)arg0 + 0x50) = RADTimerRead();
}

static void fn_801195A0(void *arg0) {
    set_both_state(arg0, 0);
    *(s32 *)((u8 *)arg0 + 0x38) = 1;
}

// Moves buffered data from a track's ring into its voice buffers, splitting interleaved stereo into the left and
// right voices.
#pragma push
#pragma opt_dead_assignments off
static void try_a_mix(StreamTrack *t) {
    u16 *src;
    u32 n;
    u16 *l;
    u16 *r;
    u16 *l0;
    u16 *r0;
    u32 i;
    u16 *dst;
    s32 level;
    if (t->available > t->minimum) {
        n = t->pending;
        if (n > t->available) {
            n = t->available;
        }
        if (n + t->writePos > t->bufSize) {
            n = t->bufSize - t->writePos;
        }
        src = (u16 *)t->readPtr;
        if ((u32)src + n > (u32)t->ringEnd) {
            n = (u32)t->ringEnd - (u32)src;
        }
        if (t->channels != 1) {
            l = l0 = (u16 *)((u8 *)t->bufL + (t->writePos >> 1));
            r = r0 = (u16 *)((u8 *)t->bufR + (t->writePos >> 1));
            for (i = n >> 2; i != 0; i--) {
                *l++ = *src++;
                *r++ = *src++;
            }
            DCFlushRange(l0, (l - l0) * sizeof(u16));
            DCFlushRange(r0, (r - r0) * sizeof(u16));
        } else {
            dst = (u16 *)((u8 *)t->bufL + t->writePos);
            memcpy(dst, src, n);
            DCFlushRange(dst, n);
            src += n / 2;
        }
        level = OSDisableInterrupts();
        t->available -= n;
        OSRestoreInterrupts(level);
        if ((u32)src >= (u32)t->ringEnd) {
            src = (u16 *)t->ringStart;
        }
        t->readPtr = (u8 *)src;
        t->writePos += n;
        if (t->writePos >= t->bufSize) {
            t->writePos -= t->bufSize;
        }
        t->pending -= n;
        if (t->ending == 0 && t->pending == 0 && t->stopping == 0) {
            start_streaming(t->voices);
        }
    }
}
#pragma pop

// Keeps a track's voices fed: tops them up from the ring, or writes silence while the voices outrun the data.
static s32 Ready(StreamTrack *t) {
    u32 pos;
    u32 now;
    u32 space;
    if (t->stopping != 0 || t->playing == 0) {
        return 0;
    }
    if (t->ending == 0) {
        try_a_mix(t);
        return 0;
    }
    pos = t->lastPos;
    if (RADTimerRead() - t->lastTime <= 12) {
        goto compute;
    }
    for (;;) {
        pos = GoodGetCurrentPosition(t->voices);
        now = RADTimerRead();
        t->lastPos = pos;
        if (now - t->lastTime >= t->interval) {
            t->lastTime = now;
            goto silence;
        }
        t->lastTime = now;
    compute:
        if (pos < t->writePos) {
            space = pos + t->bufSize - t->writePos;
        } else {
            space = pos - t->writePos;
        }
        space = space < 1024 ? 0 : (space - 1024) & ~3;
        if (space < t->chunk && (t->pending == 0 || space == 0)) {
            goto done;
        }
        if (space < t->threshold) {
            break;
        }
    silence:
        t->dirty = 1;
        clear(t->voices, t->writePos, t->chunk * 2);
        t->writePos += t->chunk * 2;
        if (t->writePos >= t->bufSize) {
            t->writePos -= t->bufSize;
        }
    }
    t->pending = space;
    try_a_mix(t);
done:
    return 0;
}

s32 fn_80119994(void) { return 0; }

s32 fn_8011999C(void) { return 0; }

static void set_addrs(void* arg0, s32 arg1) {
    extern void AXSetVoiceAddr(void*, s16*);
    s16 rect[8];
    u32 width;
    u32 limit;
    u32 x0;
    u32 x1;
    rect[0] = 1;
    rect[1] = 10;
    width = *(u32*)((u8*)arg0 + 0x10);
    x0 = width >> 1;
    x1 = ((width + *(u32*)((u8*)arg0 + 0x1C)) >> 1) - 1;
    if (arg1 != 0)
        limit = x0;
    else
        limit = *(u32*)(*(u32*)arg0 + 0x1BC);
    rect[2] = x0 >> 16;
    rect[3] = x0;
    rect[4] = x1 >> 16;
    rect[5] = x1;
    rect[6] = limit >> 16;
    rect[7] = limit;
    AXSetVoiceAddr(*(void**)arg0, rect);
    if (*(void**)((u8*)arg0 + 4) != 0) {
        u32 height = *(u32*)((u8*)arg0 + 0x14);
        u32 y0 = height >> 1;
        u32 y1 = ((height + *(u32*)((u8*)arg0 + 0x1C)) >> 1) - 1;
        if (arg1 != 0)
            limit = y0;
        else
            limit = *(u32*)(*(u32*)((u8*)arg0 + 4) + 0x1BC);
        rect[2] = y0 >> 16;
        rect[3] = y0;
        rect[4] = y1 >> 16;
        rect[5] = y1;
        rect[6] = limit >> 16;
        rect[7] = limit;
        AXSetVoiceAddr(*(void**)((u8*)arg0 + 4), rect);
    }
}

static void Volume(StreamTrack *t, s32 volume) {
    s32 value = volume;
    f32 temp;
    if (value < 0) value = 0;
    if (value > 0x7fff) value = 0x7fff;
    temp = (f32) pow((f32)value, 1.6666666f);
    t->volume = (s32)(0.000976582407f * temp);
    set_rate_volume_and_pan(t);
}

static void Pan(StreamTrack *t, s32 pan) {
    s32 value = pan;
    f32 temp_f31;
    f32 temp_f3;
    f32 temp_f1;
    if (value < 0) value = 0;
    if (value > 0x10000) value = 0x10000;
    temp_f31 = (f32)value / 65536.0f;
    temp_f3 = (f32)pow(1.0f - temp_f31, 0.3f);
    t->panL = (s32)(32768.0f * temp_f3);
    temp_f1 = (f32)pow(temp_f31, 0.3f);
    t->panR = (s32)(32768.0f * temp_f1);
    set_rate_volume_and_pan(t);
}

static s32 SetOnOff(StreamTrack *t, s32 run) {
    if (run != 0 && t->playing == 0) {
        t->ending = 0;
        t->playing = 1;
        t->writePos = 0;
        t->pending = t->prefill;
    } else if (run == 0 && t->playing != 0) {
        set_both_state(t->voices, 0);
        set_addrs(t->voices, 1);
        clear(t->voices, 0, t->bufSize);
        t->playing = 0;
    }
    return t->playing;
}

static s32 Pause(StreamTrack *t, s32 paused) {
    s32 *var_r3;
    if (paused != 0) {
        if (t->ending != 0) {
            fn_801195A0(t->voices);
        }
        var_r3 = &t->stopping;
        t->stopping = 1;
    } else {
        if (t->ending == 0) {
            if (t->pending == 0) {
                start_streaming(t->voices);
            }
        } else {
            fn_80119498(t->voices);
        }
        var_r3 = &t->stopping;
        t->stopping = 0;
    }
    return *var_r3;
}

// Stops and releases the voices of a channel, then frees its sample buffer.
static void Close(void *arg0) {
    StreamChannel *ch = arg0;
    if (ch->voices[0] != 0) {
        ch->stopping = 1;
        set_both_state(ch->voices, 0);
        if (ch->voices[0] != 0) {
            AXFreeVoice(ch->voices[0]);
        }
        if (ch->voices[1] != 0) {
            AXFreeVoice(ch->voices[1]);
        }
        ((void (*)(void *))lbl_8055DEE0[1])(ch->bufL);
        lbl_8055DEE8--;
        ch->voices[0] = 0;
    }
}

// Bink movie header fields used for the sound length.

// Bink sound open for AX: sets up a track of `chans` 16-bit voices at `freq` Hz with a 600 ms buffer, prefilled with
// up to the whole movie's sound, and installs the track callbacks. Returns 0 without an allocator or voices.
static s32 Open(StreamTrack *t, u32 freq, s32 bits, s32 chans, u32 flags, BinkInfo8011 *bink) {
    u32 bps;
    u32 total;
    u32 n;
    void **voice;
    u32 *vs;
    u32 i;
    u8 *buf;

    if (lbl_8055DEE0[0] == 0) {
        return 0;
    }
    memset(t, 0, sizeof(StreamTrack));
    t->dirty = 0;
    t->rate = freq;
    t->bits = 16;
    t->channels = chans;
    t->voices[0] = AXAcquireVoice(31, 0, 0);
    if (t->voices[0] == 0) {
        return 0;
    }
    if (t->channels != 1) {
        t->voices[1] = AXAcquireVoice(31, 0, 0);
        if (t->voices[1] == 0) {
            AXFreeVoice(t->voices[0]);
            return 0;
        }
    }
    bps = t->channels * (t->rate * 2);
    t->curRate = freq;
    t->bufSize = (bps * 600 / 1000 + 0x3FF) & ~0x3FF;
    t->chunk = (bps * 50 / 1000 + 0xFF) & ~0xFF;
    t->threshold = t->bufSize * 90 / 100;
    t->interval = 0x21C;
    t->unk68 = 0;
    if (bink->frameRate != 0 && bink->frameRateDiv != 0) {
        u64 len = (u64)bps * bink->frameRateDiv;
        total = len * bink->frames / bink->frameRate;
    } else {
        total = t->chunk;
    }
    if (chans != 1) {
        vs = &t->voiceSize;
        *vs = t->bufSize >> 1;
    } else {
        vs = &t->voiceSize;
        *vs = t->bufSize;
    }
    buf = ((void *(*)(u32))lbl_8055DEE0[0])(t->bufSize);
    t->bufL = (s16 *)buf;
    t->bufR = (s16 *)(buf + *vs);
    t->physL = (u32)buf - 0x80000000;
    t->physR = t->physL + *vs;
    t->minimum = t->chunk;
    t->volume = 0x7FFF;
    t->panL = 0x67F8;
    t->panR = 0x67F8;
    set_rate_volume_and_pan(t);
    set_addrs(t->voices, 1);
    voice = t->voices;
    for (i = 0; i < t->channels; i++) {
        AXPBADPCM adpcm;
        AXPBSRC src;
        memset(&adpcm, 0, sizeof(adpcm));
        adpcm.gain = 0x800;
        src.ratioHi = 1;
        src.ratioLo = 0;
        src.currentAddressFrac = 0;
        src.last_samples[0] = 0;
        src.last_samples[1] = 0;
        src.last_samples[2] = 0;
        src.last_samples[3] = 0;
        AXSetVoiceSrcType(voice[i], 1);
        AXSetVoiceSrc(voice[i], &src);
        AXSetVoiceSrcRatio(voice[i], t->rate / 32028.666666666668);
        AXSetVoiceAdpcm(voice[i], &adpcm);
    }
    clear(t->voices, 0, t->bufSize);
    n = t->bufSize;
    t->writePos = 0;
    t->ending = 0;
    if (total < n) {
        n = total;
    }
    t->prefill = n;
    t->pending = n;
    t->stopping = 0;
    t->playing = 1;
    t->IsActive = Ready;
    t->GetBuffer = (s32 (*)(StreamTrack *, u8 **, u32 *))fn_80119994;
    t->Commit = (void (*)(StreamTrack *, u32))fn_8011999C;
    t->SetVolume = Volume;
    t->SetPan = Pan;
    t->SetPaused = (void (*)(StreamTrack *, s32))Pause;
    t->Run = SetOnOff;
    t->Close = (void (*)(StreamTrack *))Close;
    lbl_8055DEE8++;
    return 1;
}

// Installs the sample buffer allocator (0 clears it); returns the stream decode callback.
void *BinkOpenAX(StreamAllocator *allocator) {
    if (allocator == NULL) {
        lbl_8055DEE0[0] = 0;
        lbl_8055DEE0[1] = 0;
    } else {
        *(StreamAllocator *)lbl_8055DEE0 = *allocator;
    }
    return (void *)Open;
}

/* Helper name unproven. The byte-reversed load is inline asm with the zero index in a register, as in the GameCube
 * Bink's intelendian (cross-checked against n100f, reference only): the __lwbrx intrinsic blocks the x8 unroll. */
static inline u32 Swap32(register u32 *p) {
    register u32 v;
    register u32 z = 0;
#ifdef __MWERKS__
    asm { lwbrx v, z, p }
#else
    /* a little-endian word load, whatever the host byte order */
    v = ((u8 *)&p[z])[0] | ((u8 *)&p[z])[1] << 8 | ((u8 *)&p[z])[2] << 16 | (u32)((u8 *)&p[z])[3] << 24;
#endif
    return v;
}

// intelendian: byte-swaps `size` bytes (rounded up to words) in place.
void intelendian(u32 *data, u32 size) {
    u32 count = (size + 3) >> 2;
    while (count--) {
        *data = Swap32(data);
        data++;
    }
}

// Reads `len` bytes at `offset` into `dst` through an aligned stack buffer, waiting for each chunk.
// Returns the bytes read, or 0 when the drive reports an error.
u32 radreadngc(void *arg0, u32 arg1, s32 arg2, u32 arg3) {
    u8 buf[0x400 + 31];
    u8 *aligned = (u8 *)(((u32)buf + 31) & ~31);
    u32 cap = ((u32)(buf + sizeof(buf)) - (u32)aligned) & ~31;
    u32 total = 0;
    u32 chunk;
    while (arg3 != 0) {
        chunk = (arg3 + 31) & ~31;
        if (arg3 >= cap) {
            chunk = cap;
        }
        DVDReadAsyncPrio((DVDFileInfo *)arg0, aligned, chunk, arg1, 0, 0);
        while (1) {
            switch (DVDGetDriveStatus()) {
            case 0:
                goto done;
            case -1:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
                return 0;
            }
        }
    done:
        if (chunk > arg3) {
            chunk = arg3;
        }
        memcpy((void *)arg2, aligned, chunk);
        arg2 += chunk;
        total += chunk;
        arg3 -= chunk;
        arg1 += chunk;
    }
    return total;
}

void CancelReadRequests(void *arg0) {
    s32 status;
    *(volatile s32 *)((u8 *)arg0 + 0xC4) = 1;
    DVDCancel((DVDCommandBlock *)((u8 *)arg0 + 0x54));
    do {
        status = DVDGetDriveStatus();
    } while (status == 2 || status == 1);
    *(volatile s32 *)((u8 *)arg0 + 0xC4) = 0;
    *(volatile s32 *)((u8 *)arg0 + 0x24) = 0;
}

u32 BinkFileReadHeader(void* arg0, s32 arg1, s32 arg2, u32 arg3) {
    extern u32 radreadngc(void*, u32, s32, u32);
    extern void CancelReadRequests(void*);
    u32 result, current, remaining;
    if (arg1 != -1 && *(u32*)((u8*)arg0 + 0x90) != (u32)arg1) {
        *(u32*)((u8*)arg0 + 0x90) = (u32)arg1;
    }
    result = radreadngc((u8*)arg0 + 0x54,
                         *(u32*)((u8*)arg0 + 0x90) + *(u32*)((u8*)arg0 + 0xB4),
                         arg2, arg3);
    if (result != arg3) {
        *(s32*)((u8*)arg0 + 0x20) = 1;
        CancelReadRequests(arg0);
    }
    // MATCHING-AID?: the read position (0x90) and cap (0x40) are re-read like volatile fields; probably genuine
    // async stream state (the 0xC0 counter in dosimulate is volatile too). Revisit with a real struct.
    *(volatile u32*)((u8*)arg0 + 0x90) += result;
    current = *(volatile u32*)((u8*)arg0 + 0x90);
    *(u32*)((u8*)arg0 + 0x98) = current;
    remaining = *(u32*)((u8*)arg0 + 0xB8) - current;
    *(u32*)((u8*)arg0 + 0x48) = remaining < *(volatile u32*)((u8*)arg0 + 0x40) ? remaining : *(volatile u32*)((u8*)arg0 + 0x40);
    intelendian((u32*)arg2, result);
    return result;
}

void dosimulate(void *arg0, s32 arg1, s32 arg2) {
    s32 temp_r0, temp_r31, temp_r3, temp_r3_2, temp_r4, var_r31;
    s32 *counter = (s32 *)((u8 *)arg0 + 0xC0);
    temp_r31 = mult64anddiv(arg1, 0x3E8, *(s32 *)((u8 *)arg0 + 0xBC));
    temp_r3 = RADTimerRead();
    var_r31 = temp_r3;
    *counter = *counter + (temp_r31 - (temp_r3 - arg2));
    while (*counter > 0) {
        do {
            temp_r3_2 = RADTimerRead();
            temp_r0 = *counter;
            temp_r4 = temp_r3_2 - var_r31;
        } while (temp_r4 < temp_r0);
        var_r31 = temp_r3_2;
        *counter = temp_r0 - temp_r4;
    }
}

// RAD's Bink I/O object as the Wii library lays it out (the GameCube one has no slots at 0x18 and 0x50).
// Layout and reader logic cross-checked against the n100f decomp's GameCube Bink (ngcfile.c), used as a
// reference only; the code here is written from this game's own instructions.
// The status fields are volatile: the stream callbacks update them from interrupt context.

// The DVD file reader's state inside BinkIO8011::iodata: a ring buffer filled ahead by async reads.

// DVDReadCallback: an async read of the ring finished. Checks the length (a full 0x8000 block, or the rest
// of the file plus the alignment skip), drops the skip bytes, advances the ring and the counters, then starts
// the next read unless the reader is suspended.
static __inline u32 BinkLeft(BinkFile8011 *f) { return f->fileSize - f->filePos; }

void DVDReadCallback(s32 result, DVDFileInfo *fi) {
    BinkIO8011 *io = ((BinkFile8011 *)fi)->io;

    if (BINKFILE(io)->cancelling)
        return;

    if (result != 0x8000) {
        u32 left = BinkLeft(BINKFILE(io));
        if (result != left + BINKFILE(io)->skip) {
            io->doingRead = 0;
            io->readError = 1;
            return;
        }
    }

    if (BINKFILE(io)->skip) {
        memmove(BINKFILE(io)->bufTail, BINKFILE(io)->bufTail + BINKFILE(io)->skip, BINKFILE(io)->fileSize - BINKFILE(io)->filePos);
        result -= BINKFILE(io)->skip;
        BINKFILE(io)->skip = 0;
    }

    io->bytesRead += result;
    BINKFILE(io)->filePos += result;
    BINKFILE(io)->bufTail += result;
    if (BINKFILE(io)->bufTail >= BINKFILE(io)->bufEnd)
        BINKFILE(io)->bufTail = BINKFILE(io)->buf;
    BINKFILE(io)->bufEmpty -= result;
    io->curBufUsed += result;
    if (io->curBufUsed > io->bufHighUsed)
        io->bufHighUsed = io->curBufUsed;

    if (BINKFILE(io)->simulate)
        dosimulate(io, result, BINKFILE(io)->readStart);

    BINKFILE(io)->readStart = RADTimerRead() - BINKFILE(io)->readStart;
    io->totalTime += BINKFILE(io)->readStart;
    if (BINKFILE(io)->wasWorking || io->working)
        io->threadTime += BINKFILE(io)->readStart;
    else
        io->idleTime += BINKFILE(io)->readStart;

    io->doingRead = 0;
    if (io->unk50 == 0)
        ReadKickoff(io);
}

// ReadKickoff: flags a drive error, or (when the drive is idle and no cancel is in progress) starts the next
// async read of up to 0x8000 bytes into the ring's tail; the file's last block is padded to 32 bytes by
// starting the read earlier (`skip`). With no room or nothing left, the current size is what is buffered.
void ReadKickoff(BinkIO8011 *io) {
    s32 status = DVDGetDriveStatus();
    u32 left = BINKFILE(io)->fileSize - BINKFILE(io)->filePos;

    switch (status) {
    case -1:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 11:
        io->readError = 1;
        return;
    }

    if (BINKFILE(io)->cancelling || (status != 0 && status != 10))
        return;
    {
        if (BINKFILE(io)->bufEmpty >= 0x8000 && left) {
            BINKFILE(io)->wasWorking = io->working;
            BINKFILE(io)->readStart = RADTimerRead();
            io->doingRead = 1;
            if (left > 0x8000)
                left = 0x8000;
            else if (left & 31) {
                BINKFILE(io)->skip = -left & 31;
                left += BINKFILE(io)->skip;
            }
            DVDReadAsyncPrio(&BINKFILE(io)->fi, BINKFILE(io)->bufTail, left,
                             BINKFILE(io)->filePos - BINKFILE(io)->skip + BINKFILE(io)->startOfs, DVDReadCallback, 0);
        } else {
            io->curBufSize = io->curBufUsed;
        }
    }
}

s32 BinkFileIdle(DVDFileInfo *info) {
    s32 *p = (s32 *)info;
    s32 status;
    if (p[0x20 / 4] != 0) return 0;
    if (p[0x50 / 4] != 0) return 0;
    status = DVDGetDriveStatus();
    switch (status) {
    case 0:  /* END */
        ReadKickoff((struct BinkIO8011 *)info);  // info is the BinkIO8011 itself
        break;
    case -1: /* FATAL_ERROR */
    case 3:  /* COVER_CLOSED */
    case 4:  /* NO_DISK */
    case 5:  /* COVER_OPEN */
    case 6:  /* WRONG_DISK */
    case 7:  /* MOTOR_STOPPED */
    case 8:  /* PAUSING */
    case 9:  /* IGNORED */
    case 11: /* RETRY */
        p[0x20 / 4] = 1;
        break;
    }
    return p[0x24 / 4];
}

// 0x50 is a state flag the DVD callbacks also change: every access is volatile.
s32 BinkFileBGControl(void *arg0, s32 arg1) {
    if (arg1 & 1) {
        if (*(volatile s32 *)((char *)arg0 + 0x50) == 0)
            *(volatile s32 *)((char *)arg0 + 0x50) = 1;
        if (arg1 & 0x80000000) {
            // busy-wait until the flag is cleared elsewhere (volatile: re-read every pass)
            while (*(volatile s32 *)((char *)arg0 + 0x24) != 0) {
            }
        }
    } else if (arg1 & 2) {
        if ((u32)*(volatile s32 *)((char *)arg0 + 0x50) == 1U)
            *(volatile s32 *)((char *)arg0 + 0x50) = 0;
        if (arg1 & 0x80000000)
            BinkFileIdle(arg0);
    }
    return *(volatile s32 *)((char *)arg0 + 0x50);
}

// BinkFileReadFrame: reads `size` bytes of frame data at `offset` (-1 = the current position) into dest.
// A seek inside the buffered window just drops the skipped bytes; any other seek cancels the pending reads
// and restarts the ring there. Unbuffered files read directly (synchronously); buffered ones wait on the ring
// (ReadKickoff = ReadKickoff), copying out in at most two pieces around its end. Returns the bytes read,
// byte-swapped to little-endian words like the rest of the file.
u32 BinkFileReadFrame(BinkIO8011 *io, u32 frame, s32 offset, void *dest, u32 size) {
    u32 start, t, avail, chunk, x;
    BOOL enabled;
    u32 amt = 0;
    void *orig = dest;

    if (io->readError)
        return 0;

    start = RADTimerRead();

    if (offset != -1 && BINKFILE(io)->bufPos != offset) {
        if ((u32)offset > BINKFILE(io)->bufPos && (u32)offset <= BINKFILE(io)->filePos) {
            enabled = OSDisableInterrupts();
            x = offset - BINKFILE(io)->bufPos;
            BINKFILE(io)->bufEmpty += x;
            BINKFILE(io)->bufPos = offset;
            io->curBufUsed -= x;
            BINKFILE(io)->bufHead += x;
            if (BINKFILE(io)->bufHead > BINKFILE(io)->bufEnd)
                BINKFILE(io)->bufHead -= io->bufSize;
            OSRestoreInterrupts(enabled);
        } else {
            CancelReadRequests(io);
            enabled = OSDisableInterrupts();
            BINKFILE(io)->filePos = offset;
            BINKFILE(io)->bufPos = offset;
            BINKFILE(io)->bufEmpty = io->bufSize;
            io->curBufUsed = 0;
            BINKFILE(io)->bufHead = BINKFILE(io)->buf;
            BINKFILE(io)->bufTail = BINKFILE(io)->buf;
            OSRestoreInterrupts(enabled);
        }
    }

    if (BINKFILE(io)->buf == 0) {
        t = RADTimerRead();
        amt = radreadngc(&BINKFILE(io)->fi, BINKFILE(io)->filePos, (s32)dest, size);
        if (amt < size) {
            io->readError = 1;
            CancelReadRequests(io);
        }
        BINKFILE(io)->filePos += amt;
        BINKFILE(io)->bufPos += amt;
        io->bytesRead += amt;
        if (BINKFILE(io)->simulate)
            dosimulate(io, amt, t);
        x = RADTimerRead();
        io->totalTime += x - t;
        io->foregroundTime += x - start;
    } else {
        while (size && !io->readError) {
            ReadKickoff(io);
            avail = io->curBufUsed;
            if (avail == 0)
                continue;
            if (avail > size)
                avail = size;
            BINKFILE(io)->bufPos += avail;
            size -= avail;
            amt += avail;
            chunk = BINKFILE(io)->bufEnd - BINKFILE(io)->bufHead;
            if (chunk <= avail) {
                memcpy(dest, BINKFILE(io)->bufHead, chunk);
                dest = (u8 *)dest + chunk;
                avail -= chunk;
                BINKFILE(io)->bufHead = BINKFILE(io)->buf;
                enabled = OSDisableInterrupts();
                io->curBufUsed -= chunk;
                BINKFILE(io)->bufEmpty += chunk;
                OSRestoreInterrupts(enabled);
            }
            if (avail) {
                memcpy(dest, BINKFILE(io)->bufHead, avail);
                dest = (u8 *)dest + avail;
                BINKFILE(io)->bufHead += avail;
                enabled = OSDisableInterrupts();
                io->curBufUsed -= avail;
                BINKFILE(io)->bufEmpty += avail;
                OSRestoreInterrupts(enabled);
            }
        }
        io->foregroundTime += RADTimerRead() - start;
    }

    x = BINKFILE(io)->fileSize - BINKFILE(io)->bufPos;
    io->curBufSize = (x < io->bufSize) ? x : io->bufSize;
    if (io->curBufUsed + 0x8000 > io->curBufSize)
        io->curBufSize = io->curBufUsed;

    intelendian((u32 *)orig, amt);
    return amt;
}

s32 fn_8011AC04(s32 arg0, s32 arg1) {
    return (arg1 + 0x7FFF) & 0xFFFF8000;
}

void BinkFileSetInfo(void *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    s32* p = (s32*)arg0;
    s32 level = OSDisableInterrupts();
    arg2 &= 0xFFFF8000;
    p[0xA0/4] = arg2;
    p[0x40/4] = arg2;
    p[0xA4/4] = arg1;
    p[0x9C/4] = arg1;
    p[0xAC/4] = arg1;
    p[0xA8/4] = arg1 + arg2;
    p[0x4C/4] = 0;
    p[0xB8/4] = arg3;
    p[0xBC/4] = arg4;
    p[0xD0/4] = (s32)arg0;
    OSRestoreInterrupts(level);
}

void BinkFileClose(void *arg0) {
    CancelReadRequests(arg0);
    if (*(s32 *)((u8 *)arg0 + 0xB0) == 0) {
        DVDClose((DVDFileInfo *)((u8 *)arg0 + 0x54));
    }
}

// BinkFileOpen: sets up `io` as a DVD file reader for `name`, or (flag 0x800000) for an already open
// DVDFileInfo passed in place of the name, whose userData is the movie's offset in that file.
s32 BinkFileOpen(BinkIO8011 *io, const char *name, u32 flags) {
    memset(io, 0, sizeof(BinkIO8011));

    if (flags & 0x800000) {
        BINKFILE(io)->fi = *(DVDFileInfo *)name;
        BINKFILE(io)->dontClose = 1;
        BINKFILE(io)->startOfs = (u32)BINKFILE(io)->fi.block.userData;
    } else {
        s32 entry = DVDConvertPathToEntrynum(name);
        if (entry == -1)
            return 0;
        if (!DVDFastOpen(entry, &BINKFILE(io)->fi))
            return 0;
    }

    io->readHeader = (void *)BinkFileReadHeader;
    io->readFrame = (void *)BinkFileReadFrame;
    io->getBufferSize = (void *)fn_8011AC04;
    io->setInfo = (void *)BinkFileSetInfo;
    io->idle = (void *)BinkFileIdle;
    io->close = (void *)BinkFileClose;
    io->unk18 = (void *)BinkFileBGControl;
    return 1;
}

/* .sbss owned by this unit (0x8055DEE0-0x8055DEF0), in REVERSE address order: this C unit emits globals last-defined first. */
s32 lbl_8055DEEC;  /* unreferenced (a gap in the target) */
s32 lbl_8055DEE8;
s32 lbl_8055DEE0[2];
