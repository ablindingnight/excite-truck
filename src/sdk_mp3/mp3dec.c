/* Nintendo-supplied SDK MP3 (Layer III) decoder, 0x800BC538-0x800C6204 in REXE01 (the same tables
 * ship in the Wii Photo Channel). Library/module name unknown. Work in progress: functions are being
 * decompiled out of address order and will be reordered when the unit is registered.
 * Build: cflags_rvl_tp, GC/3.0a3, plus -use_lmw_stmw on -inline noauto (every function saves with
 * stmw; WENCGetEncodeData right after it does not, so it is a separate object).
 * Matching so far: MP3_Create, MP3_GetInfo, MP3_Fill, MP3_ArenaInit/Alloc/Set. Close (register or
 * scheduling only): FindSync, GetBits, GetBitsRing, ParseHeader, HuffLookup, PeekBitsRing. */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

extern const u8 lbl_802F3408[];        /* bit masks: (1 << n) - 1 */
extern const s16 lbl_802F3418[3][16];  /* MPEG-1 frame size / 8 by [sample rate][bitrate] */
extern const s16 lbl_802F3478[3][16];  /* MPEG-2 */
extern u32 lbl_8055DD90;
extern u32 lbl_8055DD94;

typedef struct MP3Info {
    s32 mpeg1;        /* 0x0 */
    s32 sample_rate;  /* 0x4 */
    s32 channels;     /* 0x8 */
    s32 frame_bytes;  /* 0xC */
    s32 out_bytes;    /* 0x10 */
} MP3Info;

typedef struct MP3Decoder {
    s8 channels;              /* 0x0 */
    u8 pad1[3];
    s32 lsf;                  /* 0x4: MPEG-2 low sampling frequency */
    s32 unk8;                 /* 0x8 */
    s32 unkC;                 /* 0xC */
    u8 pad10[4];
    s32 unk14;                /* 0x14 */
    s32 unk18;                /* 0x18 */
    s16 sr_index;             /* 0x1C */
    u8 pad1E[0x278 - 0x1E];
    u32 unk278;               /* 0x278 */
    u16 unk27C;               /* 0x27C */
    u8 pad27E[0x9CC - 0x27E];
    s32 unk9CC[8];            /* 0x9CC */
    u8 pad9EC[0xED70 - 0x9EC];
    s32 frame_bytes;          /* 0xED70 */
    s32 out_bytes;            /* 0xED74 */
} MP3Decoder;






u32 MP3_GetBits(u32 unused, const u8* buf, u32 n, s32* pos);

s32 MP3_FindSync(u32 ctx, const u8* buf, s32* pos) {
    s32 word = MP3_GetBits(ctx, buf, 12, pos);

    while ((u16)word != 0xFFF) {
        word = (u16)(word << 1) | MP3_GetBits(ctx, buf, 1, pos);
        if (*pos > 0x3FFF) {
            return 0;
        }
    }
    return 1;
}

u32 MP3_GetBits(u32 unused, const u8* buf, u32 n, s32* pos) {
    u16 result = 0;
    s32 left = n;
    u32 avail;
    u32 take;

    buf += *pos >> 3;
    avail = 8 - (*pos & 7);
    while (left > 0) {
        if (avail == 0) {
            avail = 8;
            buf++;
        }
        take = left < avail ? left : avail;
        result |= ((lbl_802F3408[avail] & *buf) >> (avail - take)) << (left - take);
        avail -= take;
        left -= take;
    }
    *pos += n;
    return result;
}

u16 MP3_GetBitsRing(const u8* buf, s32 n, u32* pos) {
    u16 result = 0;
    u32 p;
    u32 idx;
    u32 avail;
    u32 take;

    if (n == 0) {
        return 0;
    }
    p = *pos;
    *pos = p + n;
    idx = (p >> 3) & 0x1FFF;
    avail = 8 - (p & 7);
    while (n > 0) {
        if (avail == 0) {
            idx = (idx + 1) & 0x1FFF;
            avail = 8;
        }
        take = n < avail ? n : avail;
        result |= ((lbl_802F3408[avail] & buf[idx]) >> (avail - take)) << (n - take);
        avail -= take;
        n -= take;
    }
    return result;
}

void MP3_GetInfo(MP3Decoder* d, MP3Info* info) {
    s32 rate = 44100;

    info->mpeg1 = d->lsf == 0;
    if (d->sr_index == 0) {
        rate = 44100;
    } else if (d->sr_index == 1) {
        rate = 48000;
    } else if (d->sr_index == 2) {
        rate = 32000;
    }
    if (info->mpeg1 == 0) {
        rate /= 2;
    }
    info->sample_rate = rate;
    info->channels = d->channels;
    info->out_bytes = d->out_bytes;
    info->frame_bytes = d->frame_bytes;
}

s32 MP3_ParseHeader(MP3Decoder* d, const u8* buf) {
    s32 pos = 0;
    s16 id;
    u16 bits;
    s16 bitrate;
    s32 size;

    if ((s16)MP3_FindSync(0x400, buf, &pos) == 0) {
        return 8;
    }
    id = MP3_GetBits(d->unk278, buf, 1, &pos);
    if ((s16)MP3_GetBits(d->unk278, buf, 2, &pos) != 1) {
        return 0xD;
    }
    if (id == 0) {
        d->lsf = 1;
    } else {
        d->lsf = 0;
    }
    MP3_GetBits(d->unk278, buf, 1, &pos);
    bits = MP3_GetBits(d->unk278, buf, 16, &pos);
    bitrate = (bits >> 12) & 0xF;
    if (bitrate == 0) {
        return 0xE;
    }
    if (bitrate == 0xF) {
        return 0xE;
    }
    d->sr_index = (bits >> 10) & 3;
    if (d->sr_index == 3) {
        return 0xF;
    }
    if ((s16)((bits >> 6) & 3) == 3) {
        d->channels = 1;
    } else {
        d->channels = 2;
    }
    if (d->lsf == 0) {
        size = lbl_802F3418[d->sr_index][bitrate] * 8;
    } else {
        size = lbl_802F3478[d->sr_index][bitrate] * 8;
    }
    d->frame_bytes = (size + 8) / 8;
    if ((size + 8) & 7) {
        d->frame_bytes++;
    }
    if (d->lsf == 0) {
        d->out_bytes = d->channels * 0x900;
    } else {
        d->out_bytes = d->channels * 0x480;
    }
    return 0;
}

void MP3_Fill(u32* p, u32 value, u32 size) {
    u32 n16 = size >> 4;
    u32 n4 = (size >> 2) & 3;

    while (n16-- != 0) {
        p[0] = value;
        p[1] = value;
        p[2] = value;
        p[3] = value;
        p += 4;
    }
    while (n4-- != 0) {
        *p++ = value;
    }
}

void MP3_ArenaInit(s32 base, s32 size);
void* MP3_ArenaAlloc(u32 size);

MP3Decoder* MP3_Create(const u8* header, u32 b, s32 buf, s32 size) {
    MP3Decoder* d;

    MP3_ArenaInit(buf, size);
    d = MP3_ArenaAlloc(sizeof(MP3Decoder));
    MP3_Fill((u32*)d, 0, sizeof(MP3Decoder));
    d->lsf = 0;
    d->unk8 = 0;
    d->unkC = 0;
    d->unk14 = 0;
    d->unk18 = 0;
    d->unk9CC[0] = 0;
    d->unk9CC[1] = 0;
    d->unk9CC[2] = 0;
    d->unk9CC[3] = 0;
    d->unk9CC[4] = 0;
    d->unk9CC[5] = 0;
    d->unk9CC[6] = 0;
    d->unk9CC[7] = 0;
    d->unk27C = 0x400;
    d->unk278 = b;
    MP3_ParseHeader(d, header);
    return d;
}

void MP3_ArenaInit(s32 base, s32 size) {
    lbl_8055DD94 = base + (4 - base % 4);
    lbl_8055DD90 = base + size - 4;
}

void* MP3_ArenaAlloc(u32 size) {
    u32 old = lbl_8055DD94;

    lbl_8055DD94 = old + (size & ~3) + ((size & 3) != 0) * 4;
    if (lbl_8055DD94 > lbl_8055DD90) {
        lbl_8055DD94 = old;
        return 0;
    }
    return (void*)old;
}

void MP3_ArenaSet(u32 p) {
    lbl_8055DD94 = p;
}

extern const u32 lbl_802F3F54[];  /* (1 << n) - 1, n = 0..24 */
extern const u32 lbl_8031C2B8[];  /* (1 << n) - 1, n = 0..8 */

typedef struct HuffTable {
    const u16** nodes;
} HuffTable;

u32 MP3_HuffLookup(s32 bits, s32 n, const HuffTable* t) {
    s32 node = 0;
    u32 mask = 0xF;
    s32 shift = 28;

    while (n-- > 0) {
        u16 e = t->nodes[node][(bits >> shift) & mask];
        if (e & 0x8000) {
            return e & 0x7FFF;
        }
        mask = lbl_8031C2B8[e & 0xF];
        node = (e >> 8) & 0xFF;
        shift -= e & 0xF;
    }
    return 0;
}

u32 MP3_PeekBitsRing(const u8* buf, u32 n, u32* pos) {
    u32 idx = (*pos >> 3) & 0x1FFF;
    s32 sh = 24 - (*pos & 7);
    u32 b0;
    u32 b1;
    u32 b2;

    *pos += n;
    if (idx < 0x1FFE) {
        b0 = buf[idx];
        b1 = buf[++idx];
        b2 = buf[++idx];
    } else {
        b0 = buf[idx];
        idx = (idx + 1) & 0x1FFF;
        b1 = buf[idx];
        idx = (idx + 1) & 0x1FFF;
        b2 = buf[idx];
    }
    return (u16)((((b0 << 16) | (b1 << 8) | b2) & lbl_802F3F54[sh]) >> (sh - n));
}
