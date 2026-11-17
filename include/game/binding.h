#ifndef GAME_BINDING_H
#define GAME_BINDING_H
// An input binding (inputbind.cpp; widgets.cpp edits them): the bound control's type and its button/key code, and a
// latch. One declaration for every unit (port ODR work, 2026-11-16). Names unproven. Builtin types only, so the
// raw-typedef widgets.cpp can include it too.

// One control: kind (1 key, 2 stick axis, 3 button, 4 direction bit, 5 mouse axis, 6 mouse button), the
// pad it reads and the control index.
struct Binding {
    int type;  // offset 0x0
    int pad;   // offset 0x4
    union {
        int code;
        unsigned char key;
    };                     // offset 0x8
    unsigned char latched;  // offset 0xC
};

#endif
