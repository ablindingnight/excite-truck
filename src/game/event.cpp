// World event dispatch (0x801EEFC8-0x801EF190, "world/event.cpp"): up to four handlers per event type;
// queued events are copied out and handed to every handler, which returns the event's payload size.
// Names are guesses apart from the messages.
#include "game/common.h"

extern "C" {
void* memset(void* dst, int c, u32 n);
void* memcpy(void* dst, const void* src, u32 n);
void fn_802E317C(const char* fmt, ...);
}

typedef int (*EventHandler)(void* ev);

struct EventQueue {
    s32 size;     // offset 0x0
    u8 data[1];   // offset 0x4
};

static EventHandler lbl_80488678[18][4];
ModuleTag lbl_8055EBD0;

extern "C" {

void fn_801EEFC8(void) { memset(lbl_80488678, 0, sizeof(lbl_80488678)); }

void fn_801EEFDC(EventHandler h, u8 type) {
    int i;
    for (i = 0; i < 4; i++) {
        if (lbl_80488678[type][i] == 0) {
            lbl_80488678[type][i] = h;
            return;
        }
    }
}

void fn_801EF024(EventHandler h) {
    int found = 0;
    int t;
    for (t = 0; t < 18; t++) {
        int i;
        for (i = 0; i < 4; i++) {
            if (lbl_80488678[t][i] == h) {
                lbl_80488678[t][i] = 0;
                found = 1;
                goto done;
            }
        }
    }
done:
    if (!found) {
        fn_802E317C("EventUnInstallHandler: Can't find handler!");
    }
}

void fn_801EF0AC(EventQueue* q) {
    EventHandler* h;
    int len;
    u8* p;
    int pos;
    u32 i;
    pos = 0;
    while (pos < q->size) {
        u8* ev = &q->data[pos];
        h = lbl_80488678[(s8)ev[0]];
        p = ev + 1;
        len = -1;
        for (i = 0; i < 4; i++, h++) {
            if (*h) {
                u8 buf[0x400];
                memcpy(buf, p, 0x400);
                len = (*h)(buf);
            }
        }
        if (len != -1) {
            pos += len + 1;
        } else {
            fn_802E317C("world/event.cpp: No handler for evt type %d", *((u8*)q + pos + 4));
            break;
        }
    }
}

void fn_801EF184(void) {}

}  // extern "C"
