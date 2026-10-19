// Primitive-graphics event handler (0x801B64B4-0x801B664C): replays queued debug-draw events (lines, boxes,
// text at a projected point, ...) and returns each event's size. Names are guesses apart from the message.
#include "game/common.h"

extern "C" u32 strlen(const char* s);

struct PGfxEvent {
    u8 type;      // offset 0x0
    u8 pad1[3];
    ValVec3 pos;  // offset 0x4
    union {
        struct {
            ValVec3 to;  // offset 0x10
            f32 size;    // offset 0x1C
            s32 color;   // offset 0x20
            s32 extra;   // offset 0x24
        } a;
        struct {
            f32 size;   // offset 0x10
            s32 color;  // offset 0x14
        } b;
        char text[0x24];  // offset 0x10
    };
    f32 f34;  // offset 0x34
};

ModuleTag lbl_8055E718;
UnkStaticFFFF lbl_8055E720;

extern "C" {
void fn_801E3CB4(const ValVec3* p, const ValVec3* q, f32 size, s32 color);
void fn_801E4040(const ValVec3* p, const ValVec3* q, f32 size, s32 color, s32 extra);
void fn_801E3FC8(const ValVec3* p, f32 size, s32 color);
void fn_801E41E0(const ValVec3* p, f32 size, s32 color);
void fn_801E43BC(const ValVec3* p, f32 a, f32 b);
void fn_802C8E70(const ValVec3* p, ValVec3* out);
int fn_802C8FF4(ValVec3* p, f32 margin);
void fn_80010724(int x, int y, const char* text);
void fn_802E317C(const char* fmt, ...);
void fn_801EEFDC(int (*handler)(const PGfxEvent*), int kind);
void fn_801EF024(int (*handler)(const PGfxEvent*));

int fn_801B64B4(const PGfxEvent* ev) {
    switch (ev->type) {
    case 0:
        fn_801E3CB4(&ev->pos, &ev->a.to, ev->a.size, ev->a.color);
        return 0x24;
    case 1:
        fn_801E4040(&ev->pos, &ev->a.to, ev->a.size, ev->a.color, ev->a.extra);
        return 0x28;
    case 2:
        fn_801E3FC8(&ev->pos, ev->b.size, ev->b.color);
        return 0x18;
    case 3: {
        ValVec3 p;
        fn_802C8E70(&ev->pos, &p);
        if (fn_802C8FF4(&p, 0.0f)) {
            fn_80010724(p.x, p.y, ev->text);
        }
        return strlen(ev->text) + 0x11;
    }
    case 4:
        fn_801E41E0(&ev->pos, ev->b.size, ev->b.color);
        return 0x18;
    case 5:
        fn_801E43BC(&ev->pos, ev->f34, 0.1f);
        return 0x38;
    default:
        fn_802E317C("pgfx_handler: Unrecognized event type %d", ev->type);
        return 0;
    }
}

void fn_801B6604(void) { fn_801EEFDC(fn_801B64B4, 3); }

void fn_801B6614(void) { fn_801EF024(fn_801B64B4); }

}  // extern "C"
