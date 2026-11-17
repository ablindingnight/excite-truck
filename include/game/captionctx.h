#ifndef GAME_CAPTIONCTX_H
#define GAME_CAPTIONCTX_H
// A menu caption's context value (MenuItem's caption constructor; widgets' menu entries). Builtin types only, so
// the raw-typedef widgets.cpp can include it. Name unproven.

struct CaptionContext {
    int value;
    CaptionContext(int v = 0) : value(v) {}
};

#endif
