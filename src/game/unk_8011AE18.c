// Bink, audio decoder and RAD callback handlers: code 0x8011AE18-0x8011C3A8. Split from game/unk_80116BD4.c on
// 2026-11-16: its constants 0x805611D8-0x80561208 start with a repeated int-to-float bias constant, so they are
// another object's pool. Unit name is provisional. CLEANUP: automated-decompiler source; rewrite with real types
// before the PC port. Tricks are tagged MATCHING-AID.
#include "game/bink_internal.h"
#include "game/ptrint.h"

/* .data tables owned by this unit (0x80329B50-0x80329DA8), in address order. */
// DUMP_DATA lbl_80329B50 u8
u8 lbl_80329B50[16] = {
#include "dumpdata/lbl_80329B50.inc"
};
// DUMP_DATA lbl_80329B60 u32
u32 lbl_80329B60[26] = {
#include "dumpdata/lbl_80329B60.inc"
};
// DUMP_DATA lbl_80329BC8 f32
f32 lbl_80329BC8[96] = {
#include "dumpdata/lbl_80329BC8.inc"
};
// DUMP_DATA lbl_80329D48 f32
f32 lbl_80329D48[24] = {
#include "dumpdata/lbl_80329D48.inc"
};

f32 fxptof(s32 arg0) {
    u32 bits = (u32)arg0;
    s32 magnitude = (bits & ~0x10000000U) >> 5;
    f32 temp = (f32)magnitude * lbl_80329D48[bits & 31U];
    if (bits & 0x10000000U) {
        temp = -temp;
    }
    return temp;
}

// Scales float samples by `gain` into clamped 16-bit PCM.
// The 64-bit borrow of v - bound: d gets the low word of the difference and v becomes the borrow mask (0 or -1).
// RAD wrote this as inline assembly on register variables (proof: retail's scheduler never moves anything across
// the subfe, which only assembly statements cause; the mask lands in v's own register and the difference in d's).
// The C form is the same arithmetic for other compilers.
#pragma push
#pragma global_optimizer off  /* same code as lifetimes/propagation/unroll_loops off; see ExpandPlane */
#pragma ppc_unroll_speculative off
void quanttos16s(s16 *arg0, f32 *arg1, u32 arg2, f32 farg0) {
    register u32 lo = 0x7FFF8000;
    register s16 *out = arg0 - 1;
    register u32 hi = 0x80007FFF;
    u32 i;
    register u32 v;
    register u32 d;
    for (i = 0; i < arg2; i++) {
        v = (s32)(*arg1 * farg0);
        v ^= 0x80000000;
        BINK_BORROW(d, lo, v);
        d &= ~v;
        v = d - 32768;
        v ^= 0x80000000;
        BINK_BORROW(d, hi, v);
        d &= v;
        v = d + 32767;
        arg1++;
        *++out = v;
    }
}
#pragma pop

// Interleaves two float channels (left at arg1, right after it) into clamped 16-bit stereo PCM.
#pragma push
#pragma global_optimizer off  /* same code as lifetimes/propagation/unroll_loops off; see ExpandPlane */
#pragma ppc_unroll_speculative off
void quanttos16chans2(s16 *arg0, f32 *arg1, u32 arg2, f32 farg0) {
    register u32 l;
    register f32 *right;
    register u32 d;
    register s16 *out = arg0 - 1;
    register u32 lo = 0x7FFF8000;
    register u32 hi = 0x80007FFF;
    register u32 r;
    u32 i;
    right = (f32 *)((arg2 << 2) + (u32)arg1);
    for (i = 0; i < arg2; i++) {
        l = (s32)(*arg1 * farg0);
        r = (s32)(farg0 * *right);
        l ^= 0x80000000;
        BINK_BORROW(d, lo, l);
        d &= ~l;
        l = d - 32768;
        l ^= 0x80000000;
        BINK_BORROW(d, hi, l);
        d &= l;
        l = d + 32767;
        r ^= 0x80000000;
        BINK_BORROW(d, lo, r);
        d &= ~r;
        r = d - 32768;
        r ^= 0x80000000;
        BINK_BORROW(d, hi, r);
        d &= r;
        r = d + 32767;
        out[1] = l;
        right++;
        arg1++;
        out += 2;
        *out = r;
    }
}
#pragma pop

void read_rle_samples(f32 *arg0, s32 arg1, void *arg2, f32 *arg3, s32 arg4) {
    u32 var_r29;
    s32 var_r28;
    f32 *var_r27;
    u32 *var_r26;
    u32 var_r25;
    u32 var_r24;
    u32 *var_r7;
    f32 var_f1;
    f32 var_f29;
    f32 var_f28;
    u32 *var_r5;
    u32 *var_r5_2;
    f32 *var_r8;
    s32 temp_r0;
    int temp_r18;
    s32 temp_r4;
    s32 temp_r4_2;
    s32 temp_r5_3;
    s32 temp_r6;
    struct { f32 *b; f32 *a; } rd;  /* Struct unproven: the two float read cursors as fields of a small local */
    s32 temp_r6_2;
    s32 var_ctr;
    s32 var_r0_2;
    s32 var_r5_3;
    s32 var_r0_3;
    u32 temp_r9;
    u32 temp_r10;
    u32 temp_r5;
    u32 temp_r5_2;
    u32 var_r0;
    u32 var_r24_2;
    u32 var_r3;
    u32 var_r4_3;

    var_f28 = var_f29 = 0.0f;
    var_r26 = ((BitReader8011 *)arg2)->next;
    var_r25 = ((BitReader8011 *)arg2)->value;
    var_r24 = ((BitReader8011 *)arg2)->count;
    rd.a = arg3;
    var_r5 = (u32 *)arg4;
    var_r28 = 0;
    goto init_test;
init_body:
    var_f29 = *rd.a;
    rd.a++;
    var_r5++;
    var_r28++;
    var_f28 = -var_f29;
init_test:
    var_r0 = (u32)*var_r5 << 1;
    if (var_r0 < 2U) goto init_body;

    var_r27 = (f32 *)((u8 *)arg0 + 8);
    var_r3 = 2U;
    goto outer_test;

decode:
    if (var_r24 == 0) {
        var_r0 = *var_r26;
        var_r24 = 0x1F;
        var_r26++;
        var_r25 = var_r0 >> 1U;
    } else {
        var_r0 = var_r25;
        var_r25 = var_r25 >> 1U;
        var_r24--;
    }
    if (var_r0 & 1) {
        if (var_r24 < 4U) {
            temp_r5 = *var_r26;
            temp_r0 = 4 - var_r24;
            var_r26++;
            temp_r4 = temp_r5 << var_r24;
            var_r24 += 0x1C;
            temp_r4_2 = var_r25 | temp_r4;
            var_r25 = temp_r5 >> temp_r0;
            var_r0_2 = temp_r4_2 & 0xF;
        } else {
            var_r0_2 = var_r25 & 0xF;
            var_r25 = var_r25 >> 4U;
            var_r24 -= 4;
        }
        var_r29 = var_r3 + (lbl_80329B50[var_r0_2] * 8U);
    } else {
        var_r29 = var_r3 + 8U;
    }
    if (var_r29 > (u32)arg1) var_r29 = arg1;

    if (var_r24 < 4U) {
        temp_r5_2 = *var_r26;
        temp_r0 = 4 - var_r24;
        var_r26++;
        temp_r4 = temp_r5_2 << var_r24;
        var_r24 += 0x1C;
        temp_r4_2 = var_r25 | temp_r4;
        var_r25 = temp_r5_2 >> temp_r0;
        var_r0_3 = temp_r4_2 & 0xF;
    } else {
        var_r0_3 = var_r25 & 0xF;
        var_r25 = var_r25 >> 4U;
        var_r24 -= 4;
    }

    if (var_r0_3 == 0) {
        temp_r18 = var_r29 - var_r3;
        memset(var_r27, 0, temp_r18 * 4);
        rd.b = arg3 + var_r28;
        var_r3 = var_r29;
        var_r27 += temp_r18;
        var_r5_2 = (u32 *)arg4 + var_r28;
        while (var_r29 > (u32)(*var_r5_2 * 2)) {
            var_f29 = *rd.b;
            rd.b++;
            var_r5_2++;
            var_r28++;
            var_f28 = -var_f29;
        }
    } else {
        temp_r6 = var_r28 * 4;
        var_r7 = (u32 *)arg4 + var_r28;
        var_r8 = arg3 + var_r28;
        temp_r9 = -1U >> (0x20 - var_r0_3);
        temp_r18 = var_r29 - var_r3;
        while (var_r3 < var_r29) {
            if (var_r3 == (u32)(*var_r7 * 2)) {
                var_f29 = *var_r8;
                var_r7++;
                var_r8++;
                var_r28++;
                var_f28 = -var_f29;
            }
            if ((u32)var_r24 < var_r0_3) {
                temp_r10 = *var_r26;
                var_r26++;
                temp_r6_2 = var_r25 | (temp_r10 << var_r24);
                var_r25 = temp_r10 >> (var_r0_3 - var_r24);
                var_r5_3 = temp_r9 & temp_r6_2;
                var_r24 = var_r24 + 0x20 - var_r0_3;
            } else {
                var_r5_3 = var_r25 & temp_r9;
                var_r25 = var_r25 >> var_r0_3;
                var_r24 -= var_r0_3;
            }
            if (var_r5_3 != 0) {
                if (var_r24 == 0) {
                    var_r4_3 = *var_r26;
                    var_r24 = 0x1F;
                    var_r26++;
                    var_r25 = var_r4_3 >> 1U;
                } else {
                    var_r4_3 = var_r25;
                    var_r25 = var_r25 >> 1U;
                    var_r24--;
                }
                if (var_r4_3 & 1) var_f1 = var_f28;
                else var_f1 = var_f29;
                *var_r27 = (f32)var_r5_3 * var_f1;
                var_r27++;
            } else {
                *var_r27 = 0.0f;
                var_r27++;
            }
            var_r3++;
        }
    }
    goto outer_test;

outer_test:
    if (var_r3 < (u32)arg1) goto decode;
    ((BitReader8011 *)arg2)->next = var_r26;
    ((BitReader8011 *)arg2)->value = var_r25;
    ((BitReader8011 *)arg2)->count = var_r24;
}

s32 Unquant(s32 size, f32 gain, u32 channels, s32 flags, s32 arg3, s32 arg4, s32 output, u32 *input, u32 bands, s32 arg8) {
    f32 buffer[4096];
    f32 factors[26];
    BitReader8011 bits;
    s32 mode;
    u32 i, j, index;
    f32 *p;
    f32 *q;
    mode = flags & 1;
    bits.start = input;
    bits.next = input;
    bits.count = 0;
    bits.value = 0;
    if (mode) {
        if (bits.count < 2) {
            bits.value = *bits.next >> (2 - bits.count);
            bits.count += 30;
            bits.next++;
        } else {
            bits.value >>= 2;
            bits.count -= 2;
        }
    }
    p = buffer;
    for (i = 0; i < channels; i++) {
        {
            u32 value;
            if (bits.count < 29) {
                u32 word = *bits.next;
                value = bits.value | (word << bits.count);
                bits.value = word >> (29 - bits.count);
                index = value & 0x1fffffff;
                bits.count += 3;
                bits.next++;
            } else {
                bits.count -= 29;
                value = bits.value;
                index = value & 0x1fffffff;
                bits.value >>= 29;
            }
        }
        p[0] = fxptof(index);
        {
            u32 value;
            if (bits.count < 29) {
                u32 word = *bits.next;
                value = bits.value | (word << bits.count);
                bits.value = word >> (29 - bits.count);
                index = value & 0x1fffffff;
                bits.count += 3;
                bits.next++;
            } else {
                bits.count -= 29;
                value = bits.value;
                index = value & 0x1fffffff;
                bits.value >>= 29;
            }
        }
        p[1] = fxptof(index);
        q = factors;
        for (j = 0; j < bands; j++) {
            {
                u32 value;
                u32 have = bits.count;
                if (have < 8) {
                    u32 word = *bits.next;
                    value = bits.value | (word << have);
                    bits.value = word >> (8 - have);
                    index = value & 0xff;
                    bits.count += 24;
                    bits.next++;
                } else {
                    bits.count -= 8;
                    value = bits.value;
                    index = value & 0xff;
                    bits.value >>= 8;
                }
            }
            if (index > 95) index = 95;
            *q++ = lbl_80329BC8[index];
        }
        read_rle_samples(p, size, &bits, factors, arg8);
        if (mode) ddct(size, 1, p, (int *)arg3, (f32 *)arg4);
        else rdft(size, -1, p, (int *)arg3, (f32 *)arg4);
        p += size;
    }
    if (channels == 1) quanttos16s((s16 *)output, buffer, size, gain);
    else quanttos16chans2((s16 *)output, buffer, size, gain);
    bits.count = 0;
    return ((u32)bits.next - (u32)bits.start) & 0x1fffffff;
}

// MSL's inline sqrtf: three Newton steps from the frsqrte estimate.
static inline f32 Sqrtf8011(f32 x) {
    if (x > 0.0f) {
        f64 guess = __frsqrte((f64)x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        return (f32)(guess * x);
    }
    return x;
}

// Opens a Bink audio decoder for `channels` channels at `rate` Hz; returns 0 when out of memory.
BinkAudio8011 *BinkAudioDecompressOpen(u32 rate, u32 channels, u32 flags) {
    u32 *bands;
    s32 *quant;
    void *transform;
    s16 *previous;
    s16 *output;
    u32 numBands;
    u32 frameSize;
    u32 half;
    u32 frameBytes;
    u32 nyquist;
    s32 dct;
    u32 i;
    BinkAudio8011 *a;

    if (rate >= 44100) {
        frameSize = 2048;
    } else if (rate >= 22050) {
        frameSize = 1024;
    } else {
        frameSize = 512;
    }
    frameBytes = frameSize * channels * 2;
    dct = flags & 1;
    if (!dct) {
        rate *= channels;
        frameSize *= channels;
        channels = 1;
    }
    half = frameSize >> 1;
    nyquist = (rate + 1) >> 1;
    for (numBands = 0; numBands < 25; numBands++) {
        if (lbl_80329B60[numBands] >= nyquist) {
            break;
        }
    }
    pushmalloc((void **)&bands, (numBands + 1) * 4);
    pushmalloc((void **)&quant, ((s32)Sqrtf8011(half) + 2) * 4);
    if (dct) {
        pushmalloc(&transform, frameSize * 20 / 4);
    } else {
        pushmalloc(&transform, half * 4 - 1);
    }
    pushmalloc((void **)&previous, frameBytes / 2);
    pushmalloc((void **)&output, frameBytes);
    a = popmalloc(sizeof(BinkAudio8011));
    if (a == 0) {
        return 0;
    }
    memset(a, 0, sizeof(BinkAudio8011));
    a->bands = bands;
    a->quant = quant;
    a->transform = transform;
    a->previous = previous;
    a->output = output;
    a->flags = flags;
    a->channels = channels;
    a->numBands = numBands;
    a->frameSize = frameSize;
    a->frameBytes = frameBytes;
    a->overlapBytes = frameBytes / 16;
    a->scale = 2.0f / Sqrtf8011(frameSize);
    for (i = 0; i < numBands; i++) {
        a->bands[i] = half * lbl_80329B60[i] / nyquist;
        if (a->bands[i] == 0) {
            a->bands[i] = 1;
        }
    }
    a->bands[i] = half;
    a->quant[0] = 0;
    a->first = 1;
    return a;
}

// Decodes one Bink audio frame from `src`: returns the output samples (cross-faded with the previous frame's tail)
// and their size, saves the new tail, and sets *next past the frame's data.
void BinkAudioDecompress(BinkAudio8011 *a, s16 **out, u32 *outLen, u8 *src, u8 **next) {
    s32 used = Unquant(a->frameSize, a->scale, a->channels, a->flags, (s32)a->quant, (s32)a->transform,
                           (s32)a->output, (u32 *)src, a->numBands, (s32)a->bands);
    if (a->first) {
        a->first = 0;
    } else {
        u32 i;
        u32 count = a->overlapBytes >> 1;
        for (i = 0; i < count; i++) {
            a->output[i] = (i * a->output[i] + a->previous[i] * (count - i)) / count;
        }
    }
    memcpy(a->previous, (u8 *)a->output + (a->frameBytes - a->overlapBytes), a->overlapBytes);
    if (outLen) {
        *outLen = a->frameBytes - a->overlapBytes;
    }
    if (out) {
        *out = a->output;
    }
    if (next) {
        *next = src + used;
    }
}

void fn_8011BC48(s32 arg0) {
    radfree((void *)arg0);
}

// RAD's callback handler: a periodic alarm that ticks a list of registered requests (0x40 bytes). Names unproven.
typedef struct RadCbNode RadCbNode;
typedef s32 (*RadCbTick)(RadCbNode *n, s32 tick);
typedef void (*RadCbDone)(RadCbNode *n, s32 tick);
struct RadCbNode {
    RadCbNode *next;      // 0x00 the handler's list
    RadCbNode *doneNext;  // 0x04 the finished list (add_sorted)
    s32 result;           // 0x08 the tick's result; finished requests sort by it, highest first
    s32 state;            // 0x0C 1 = active, 2 = suspended
    s32 suspends;         // 0x10 suspend count (shared with interrupt-time code)
    RadCbTick tick;       // 0x14
    RadCbDone done;       // 0x18
};
typedef struct RadCbHandler {
    s32 state;            // 0x00 0 = new, 1 = running, 2 = suspended
    s32 holds;            // 0x04 suspend count (shared with the alarm handler)
    u8 alarm[0x28];       // 0x08 an OSAlarm of the SDK RAD was built against (0x28 bytes; this SDK's is 0x30)
    RadCbNode *list;      // 0x30
    s32 ticks;            // 0x34
    s32 count;            // 0x38 registered requests
    u32 rate;             // 0x3C ticks per second
} RadCbHandler;

// Unlink node from the handler's list. Returns 1 if it was there.
s32 remove_cb_from_list(RadCbHandler *owner, RadCbNode *node) {
    RadCbNode *p;
    if (owner == NULL) return 0;
    p = owner->list;
    if (p == node) {
        owner->list = p->next;
        return 1;
    }
    while (p->next != NULL) {
        if (p->next == node) {
            p->next = node->next;
            return 1;
        }
        p = p->next;
    }
    return 0;
}

void add_sorted(RadCbNode **head, RadCbNode *node) {
    u32 key;
    RadCbNode *cur = *head;
    if (cur == 0) {
        node->doneNext = 0;
        *head = node;
        return;
    }
    key = node->result;
    if (key >= (u32)cur->result) {
        node->doneNext = cur;
        *head = node;
        return;
    }
    while (cur->doneNext != 0) {
        RadCbNode *next = cur->doneNext;
        if (key >= (u32)next->result) {
            node->doneNext = next;
            cur->doneNext = node;
            return;
        }
        cur = next;
    }
    node->doneNext = 0;
    cur->doneNext = node;
}

// Ticks every active request of the queue; finished ones are collected and then told so.
void call_callbacks(RadCbHandler *h) {
    RadCbNode *done;
    RadCbNode *n;
    h->ticks += 1;
    if (h->state == 1) {
        n = h->list;
        done = 0;
        while (n != 0) {
            if (h->state != 1) {
                return;
            }
            if (n->state == 1) {
                n->result = n->tick(n, h->ticks);
                if (n->result != 0) {
                    add_sorted(&done, n);
                }
            }
            n = n->next;
        }
        for (n = done; n != 0; n = n->doneNext) {
            n->done(n, h->ticks);
        }
    }
}

void RAD_callback_timer(OSAlarm *alarm) {
    RadCbHandler *h;
    s32 old;
    h = (RadCbHandler *)((u8 *)alarm - 8);  // the handler holding this alarm (RadCbHandler::alarm)
    if (h != 0 && h->list != 0) {
        old = OSEnableInterrupts();
        call_callbacks(h);
        OSRestoreInterrupts(old);
    }
}

// Creates a sound request queue for `count` entries (negative counts are taken as positive).
void *RADCB_allocate_handler(s32 count) {
    RadCbHandler *q = radmalloc(0x40);
    if (q != NULL) {
        memset(q, 0, 0x40);
        q->state = 0;
        q->holds = 1;
        q->list = 0;
        q->ticks = 1;
        q->count = 0;
        if (count < 0) {
            count = -count;
        }
        q->rate = count;
    }
    return q;
}

// Queues request `arg1` (callbacks arg2/arg3) on `arg0` with interrupts disabled.
void RADCB_register_callback(void *arg0, void *arg1, s32 arg2, s32 arg3) {
    RadCbHandler *h = (RadCbHandler *)arg0;
    RadCbNode *n = (RadCbNode *)arg1;
    s32 level = OSDisableInterrupts();
    n->state = 1;
    n->tick = (RadCbTick)arg2;
    n->done = (RadCbDone)arg3;
    n->suspends = 0;
    n->next = h->list;
    h->list = n;
    h->count += 1;
    OSRestoreInterrupts(level);
}

// The handler and request come in as integers (PtrInt): retail's register order needs int-typed formals copied to
// pointer locals (pointer formals colour differently, tried 2026-11-16).
s32 RADCB_unregister_callback(PtrInt arg0, PtrInt arg1, s32 flags) {
    s32 result = 0;
    RadCbHandler *owner;
    RadCbNode *node;
    s32 level;
    owner = (RadCbHandler *)arg0;
    node = (RadCbNode *)arg1;
    if (arg0 != 0 && arg1 != 0 && node->done != 0 && node->tick != 0) {
        level = OSDisableInterrupts();
        if (remove_cb_from_list(owner, node) != 0) {
            owner->count -= 1;
            if ((flags & 1) && owner->list == 0) {
                RADCB_free_handler(owner);
                result = 1;
            }
        }
        OSRestoreInterrupts(level);
    }
    return result;
}

s32 RADCB_unregister_2_callbacks(s32 arg0, s32 arg1, void *arg2, s32 arg3, s32 arg4) {
    s32 temp = RADCB_unregister_callback((PtrInt)arg2, arg3, arg4) << 1;
    s32 result = RADCB_unregister_callback(arg0, arg1, arg4);
    return result | temp;
}

u32 RADCB_try_to_suspend_handler(void *arg0) {
    RadCbHandler *h = (RadCbHandler *)arg0;
    s32 oldInterrupts;
    s32 value;
    if (h != NULL) {
        oldInterrupts = OSDisableInterrupts();
        if (h->state == 1)
            h->state = 2;
        value = h->holds;
        h->holds = value + 1;
        OSRestoreInterrupts(oldInterrupts);
        value = h->state;
        return (u32)((1 - value) | (value - 1)) >> 31;
    }
    return 1;
}

// Waits (yielding) until RADCB_try_to_suspend_handler reports the request finished.
void RADCB_suspend_handler(void *arg0) {
    while (RADCB_try_to_suspend_handler(arg0) == 0) {
        OSYieldThread();
    }
}

// Releases one hold on the queue; the last release starts its periodic tick (`rate` per second).
void RADCB_resume_handler(void *arg0) {
    RadCbHandler *h = (RadCbHandler *)arg0;
    s32 level;
    if (h != 0) {
        level = OSDisableInterrupts();
        // hold count, shared with the alarm handler
        *(volatile s32 *)&h->holds -= 1;
        if (*(volatile s32 *)&h->holds == 0) {
            if (h->state == 0) {
                OSSetPeriodicAlarm((OSAlarm *)h->alarm, OSGetTime(),
                                   (1000 / h->rate) * (*(u32 *)0x800000F8 / 4 / 1000),
                                   (OSAlarmHandler)RAD_callback_timer);
            }
            h->state = 1;
        }
        OSRestoreInterrupts(level);
    }
}

u32 RADCB_try_to_suspend_callback(void *arg0, void *arg1) {
    RadCbNode *n = (RadCbNode *)arg1;
    if (arg0 != 0 && n != 0 &&
        n->done != 0 &&
        n->tick != 0) {
        s32 level = OSDisableInterrupts();
        if (n->state == 1)
            n->state = 2;
        n->suspends += 1;
        OSRestoreInterrupts(level);
        return n->state != 1;
    }
    return 1;
}

// Waits (yielding) until RADCB_try_to_suspend_callback reports the request finished.
void RADCB_suspend_callback(void *arg0, void *arg1) {
    RadCbNode *n = (RadCbNode *)arg1;
    if (arg0 != 0 && n != 0 &&
        n->done != 0 &&
        n->tick != 0) {
        while (RADCB_try_to_suspend_callback(arg0, arg1) == 0) {
            OSYieldThread();
        }
    }
}

void RADCB_resume_callback(void *arg0, void *arg1) {
    RadCbNode *n = (RadCbNode *)arg1;
    if (arg0 != 0 && n != 0 &&
        n->done != 0 &&
        n->tick != 0) {
        s32 level = OSDisableInterrupts();
        // pending-request count, shared with interrupt-time code
        *(volatile s32 *)&n->suspends -= 1;
        if (*(volatile s32 *)&n->suspends == 0)
            n->state = 1;
        OSRestoreInterrupts(level);
    }
}

void RADCB_free_handler(void *arg0) {
    RadCbHandler *h = (RadCbHandler *)arg0;
    if (h != 0) {
        RADCB_suspend_handler(h);
        OSCancelAlarm((OSAlarm *)h->alarm);
        radfree(h);
    }
}
