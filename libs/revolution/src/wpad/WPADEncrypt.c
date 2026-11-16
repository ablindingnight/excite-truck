#include "__wpad.h"

#include "cstring"

static u8 SEED;
static u8 NUM_A;
static u8 NUM_B;
static u8 NUM_C;

// DUMP_DATA base u8
static u8 base[0x2A] = {
#include "dumpdata/base.inc"
};
// DUMP_DATA tb u8
static u8 tb[0x900] = {
#include "dumpdata/tb.inc"
};

static u8 random() {
    SEED = (u8)(((u32)SEED * NUM_A + NUM_B) % NUM_C);
    return SEED;
}

static u8 ror(u8 src, u8 rot) {
    u8 ret;
    u8 rt;

    rt = (u8)(rot % 8);
    ret = (u8)(src >> rt | src << (8 - rt));

    return ret;
}

static u8 rol(u8 src, u8 rol) {
    u8 ret;
    u8 rl;

    rl = (u8)(rol % 8);
    ret = (u8)(src << rl | src >> (8 - rl));

    return ret;
}

void WPADiCreateKey(s32 chan) {
    BOOL enable;
    WPADControlBlock* p_wpd = _wpdcb[chan];

    u8 idx;
    u8 baseIdx;
    u8 tblRnd[10];
    u8 ft[8];
    u8 sb[8];
    u8 key[6];
    u8 i;

    SEED = (u8)((OSGetTick() >> 8) & 0xff);
    NUM_A = (u8)((OSGetTick() >> 16) & 0x3f);
    NUM_B = (u8)((OSGetTick() >> 24) & 0x4c);
    NUM_C = 0xff;

    idx = (u8)(random() % 7);
    baseIdx = (u8)(idx * 6);

    for (i = 0; i < 10; i++) {
        tblRnd[i] = tb[random()];
    }

    enable = OSDisableInterrupts();
    key[0] = (u8)((ror((u8)(base[baseIdx] ^ tb[tblRnd[5]]), tb[tblRnd[2]]) - tb[tblRnd[9]]) ^ tb[tblRnd[4]]);
    key[1] = (u8)((ror((u8)(base[baseIdx + 1] ^ tb[tblRnd[1]]), tb[tblRnd[0]]) - tb[tblRnd[5]]) ^ tb[tblRnd[7]]);
    key[2] = (u8)((ror((u8)(base[baseIdx + 2] ^ tb[tblRnd[6]]), tb[tblRnd[8]]) - tb[tblRnd[2]]) ^ tb[tblRnd[0]]);
    key[3] = (u8)((ror((u8)(base[baseIdx + 3] ^ tb[tblRnd[4]]), tb[tblRnd[7]]) - tb[tblRnd[3]]) ^ tb[tblRnd[2]]);
    key[4] = (u8)((ror((u8)(base[baseIdx + 4] ^ tb[tblRnd[1]]), tb[tblRnd[6]]) - tb[tblRnd[3]]) ^ tb[tblRnd[4]]);
    key[5] = (u8)((ror((u8)(base[baseIdx + 5] ^ tb[tblRnd[7]]), tb[tblRnd[8]]) - tb[tblRnd[5]]) ^ tb[tblRnd[9]]);

    ft[0] = (u8)(tb[(idx + 1) * 256 + key[4]] ^ tb[(idx + 2) * 256 + tblRnd[3]]);
    ft[1] = (u8)(tb[(idx + 1) * 256 + key[2]] ^ tb[(idx + 2) * 256 + tblRnd[5]]);
    ft[2] = (u8)(tb[(idx + 1) * 256 + key[5]] ^ tb[(idx + 2) * 256 + tblRnd[7]]);
    ft[3] = (u8)(tb[(idx + 1) * 256 + key[0]] ^ tb[(idx + 2) * 256 + tblRnd[2]]);
    ft[4] = (u8)(tb[(idx + 1) * 256 + key[1]] ^ tb[(idx + 2) * 256 + tblRnd[4]]);
    ft[5] = (u8)(tb[(idx + 1) * 256 + key[3]] ^ tb[(idx + 2) * 256 + tblRnd[9]]);
    ft[6] = (u8)(tb[(idx + 1) * 256 + tblRnd[0]] ^ tb[(idx + 2) * 256 + tblRnd[6]]);
    ft[7] = (u8)(tb[(idx + 1) * 256 + tblRnd[1]] ^ tb[(idx + 2) * 256 + tblRnd[8]]);
    sb[0] = (u8)(tb[(idx + 1) * 256 + key[0]] ^ tb[(idx + 2) * 256 + tblRnd[1]]);
    sb[1] = (u8)(tb[(idx + 1) * 256 + key[5]] ^ tb[(idx + 2) * 256 + tblRnd[4]]);
    sb[2] = (u8)(tb[(idx + 1) * 256 + key[3]] ^ tb[(idx + 2) * 256 + tblRnd[0]]);
    sb[3] = (u8)(tb[(idx + 1) * 256 + key[2]] ^ tb[(idx + 2) * 256 + tblRnd[9]]);
    sb[4] = (u8)(tb[(idx + 1) * 256 + key[4]] ^ tb[(idx + 2) * 256 + tblRnd[7]]);
    sb[5] = (u8)(tb[(idx + 1) * 256 + key[1]] ^ tb[(idx + 2) * 256 + tblRnd[8]]);
    sb[6] = (u8)(tb[(idx + 1) * 256 + tblRnd[3]] ^ tb[(idx + 2) * 256 + tblRnd[5]]);
    sb[7] = (u8)(tb[(idx + 1) * 256 + tblRnd[2]] ^ tb[(idx + 2) * 256 + tblRnd[6]]);

    for (i = 0; i < 10; i++) {
        p_wpd->key[i] = tblRnd[9 - i];
    }

    for (i = 0; i < 6; i++) {
        p_wpd->key[i + 10] = key[5 - i];
    }

    memcpy(p_wpd->ft, ft, 8);
    memcpy(p_wpd->sb, sb, 8);
    OSRestoreInterrupts(enable);
}

void WPADiDecode(s32 chan, void* p_buf, u16 len, u16 offset) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    u8 idx;
    u8* ptr;
    u16 i;

    ptr = (u8*)p_buf;

    for (i = 0; i < len; i++) {
        idx = (u8)((offset + i) % 8);
        ptr[i] = (u8)((ptr[i] ^ p_wpd->sb[idx]) + p_wpd->ft[idx]);
    }
}
