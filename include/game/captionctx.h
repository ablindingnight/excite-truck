#ifndef GAME_CAPTIONCTX_H
#define GAME_CAPTIONCTX_H
// A menu caption's context value (MenuItem's caption constructor; widgets' menu entries). Builtin types only, so
// the raw-typedef widgets.cpp can include it. Name unproven.

struct CaptionContext {
    int value;
    CaptionContext(int v = 0) : value(v) {}
};

// The default caption context of a text item: MenuItem(name, text, (int)NoCaption()). The value is written to one
// member and read back through the other, so it goes through a temporary; a text item then costs retail's 12
// instructions at instruction selection (the plain default costs 14, a literal 0 costs 10).
/* Helper name and form unproven: a stand-in until the real default is found (handoff FACTS 2026-11-17 04:32). */
struct NoCaption {
    union {
        int value;
        unsigned int raw;
    };
    NoCaption(void) { value = 0; }
    operator int() const { return (int)raw; }
};

#endif
