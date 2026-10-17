/* Shared declarations for the SDK MP3 (Layer III) decoder units (mp3frame.c, mp3dequant.c, mp3layer3.c). */
#ifndef MP3DEC_H
#define MP3DEC_H

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef long long s64;

typedef struct HuffTable {
    u16** nodes;
} HuffTable;

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
    s16 mode_ext;             /* 0x1E: bit 1 = M/S stereo */
    u8 pad20[0x278 - 0x20];
    u32 unk278;               /* 0x278 */
    s16 unk27C;               /* 0x27C: synthesis window position */
    u8 pad27E[0x9BC - 0x27E];
    s32 long_end;             /* 0x9BC: lines covered by the long-block bands */
    s32 short_bands;          /* 0x9C0: short-block bands per window */
    s32 short_end;            /* 0x9C4: lines covered by the short-block bands */
    s32 band_count;           /* 0x9C8 */
    s32 unk9CC[7];            /* 0x9CC: [0] mixed map built, [1] long map built, [2] short map built, [3] band tables copied */
    s32 is_init;              /* 0x9E8: MPEG-2 intensity table built */
    u8 pad9EC[0xA6E - 0x9EC];
    s16 is_table[33];         /* 0xA6E: MPEG-2 intensity ratios (low halves of the Q20 table) */
    u8 padAB0[0xED70 - 0xAB0];
    s32 frame_bytes;          /* 0xED70 */
    s32 out_bytes;            /* 0xED74 */
} MP3Decoder;

/* Per-channel granule buffer (0x2400 bytes); the frame decoder passes decoder + 0xAB0. */
typedef struct MP3ChannelBuf {
    s32 sxr[3][576];          /* 0x0: dequantized short-block windows */
    s32 xr[576];              /* 0x1B00: dequantized spectrum */
} MP3ChannelBuf;

typedef struct BitStream {
    u8 pad0[0xC];
    u32 pos;   /* 0xC */
    u32 end;   /* 0x10 */
} BitStream;

typedef struct HuffState {
    u8 pad0[0x4];
    s32 lsf;      /* 0x4 */
    u8 pad8[0x4];
    u32 pos;      /* 0xC */
    u32 end;      /* 0x10 */
    u8 pad14[0x8];
    s16 sr_index; /* 0x1C */
    u8 pad1E[0x288 - 0x1E];
    s16 count1_end; /* 0x288 */
} HuffState;

#define NULL 0
#ifndef MP3_STATIC
#define MP3_STATIC static
void MP3_Dequantize(MP3Decoder* d, u8* long_map, u8* short_map, u8* mixed_map, s16* long_edges, s16* short_edges,
                    u32* block_flags, MP3ChannelBuf* out, s16* in, s16* global_gain, s16* subblock_gain,
                    s16* sf_scale, s16* sf_long, s16 (*sf_short)[3], s16* preflag);

void fn_800C57B0(s32 count0, s32 count1, u32 flags, MP3ChannelBuf* g);

#endif

void fn_800BD710(MP3Decoder* d, s32 ch, s32 sf, s32* r, s32* l);
void MP3_Antialias(s32 sblim, s32 start, s8 channels, s32 mixed, MP3ChannelBuf* g);
s32 MP3_FindSync(u32 ctx, u8* buf, s32* pos);
u32 MP3_GetBits(u32 unused, u8* buf, u32 n, s32* pos);
u32 MP3_GetBitsRing(u8* buf, s32 n, u32* pos);
void MP3_GetInfo(MP3Decoder* d, MP3Info* info);
s32 MP3_ParseHeader(MP3Decoder* d, u8* buf);
void MP3_Fill(u32* p, u32 value, u32 size);
MP3Decoder* MP3_Create(u8* header, u32 b, s32 buf, s32 size);
void MP3_Delete(u32 p);
void MP3_ArenaInit(s32 base, s32 size);
void* MP3_ArenaAlloc(u32 size);
void MP3_ArenaSet(u32 p);
u32 MP3_HuffLookup(s32 bits, s32 n, HuffTable* t);
u32 MP3_PeekBitsRing(u8* buf, u32 n, u32* pos);
void MP3_DecodeQuads(BitStream* bs, s32 end, u8* buf, s16* out, s32 i, s32 track, s16* zero_start);
void MP3_DecodeQuadsB(BitStream* bs, s32 end, u8* buf, s16* out, s32 i, s32 track, s16* zero_start);
s32 MP3_DecodeBigValues(HuffState* d, u8* buf, s16* out, s32 big_values, u32 block_flags, s32 region0,
                        s32 region1, s16 count1_table, s32 track, s16* table_select, s16* zero_start);
void MP3_Reorder32(u32* dst, u32* src, s32 mode);
void fn_800C18A0(s32* in, s32* out);
void fn_800C1984(s32* in, s32* out);
void fn_800C1A80(s32* a, s32* b, s32* out);
void fn_800C1C1C(s32* in, s32* out);
void fn_800C1E88(s32* coeff, s32* input, s32* output);
void fn_800C1F6C(s32* dst, s32* a, s32* b);
void fn_800C2090(s32* dst, s32* a, s32* b);
void fn_800C4CF4(MP3Decoder* d, s32* buf, s32* vb, s16* out, s8 channels, s32 mode);
s32 fn_800C4E5C(s32* in, s32* out);
void fn_800C21D8(s32* a, s32* b);
void fn_800C2590(s32* cur, s32* prev, s32* out1, s32* out2);
void fn_800C280C(s32* cur, s32* prev, s32* out1, s32* out2);
void fn_800C2A88(s32* cur, s32* prev, s32* out1, s32* out2);
void fn_800C2C18(s32* cur, s32* prev, s32* out1, s32* out2);
void fn_800C2DBC(s32* in, s32* out1, s32* out2);
void fn_800C2F98(s32* cur, s32* out1, s32* out2);
void fn_800C5298(s32* dst, s32* src, s32* c);
void fn_800C3960(s32* v, const s32* w, s32 pos, u8* out, s32 mask, s32 mode);
void fn_800C3574(s32* v, const s32* w, s32 pos, u8* out, s32 mode);
void fn_800C3088(s32* out, s32* in1, s32* in2, s32* in3);
void fn_800C34B4(s32* dst, u8* src, s32 win, s32 mode);
void fn_800C51BC(s32* dst, s32* src, s32* c);
void fn_800C5430(s32* dst, s32* src, s32* c);
void fn_800C5508(s32* dst, s32* src, s32* c);
void fn_800C55D8(s32* dst, s32* src, s32* c);

void MP3_Dequantize(MP3Decoder* d, u8* long_map, u8* short_map, u8* mixed_map, s16* long_edges, s16* short_edges,
                    u32* block_flags, MP3ChannelBuf* out, s16* in, s16* global_gain, s16* subblock_gain,
                    s16* sf_scale, s16* sf_long, s16 (*sf_short)[3], s16* preflag);

void fn_800C57B0(s32 count0, s32 count1, u32 flags, MP3ChannelBuf* g);

#endif
