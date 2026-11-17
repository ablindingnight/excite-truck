#ifndef GAME_BUILDQUERY_H
#define GAME_BUILDQUERY_H

// Helper name unproven; PROVISIONAL: reproduces retail's pass-one block cut (FACTS 2026-11-17 13:40).
//
// At a few places retail's code is scheduled as if a conditional branch stood between two statements, although no
// instruction of such a test is left. The compiler does that for one kind of test only: an equality compare of a
// constant that reached it through memory, which its back end folds after the first scheduling pass (FACTS
// 2026-11-17 02:20). An ordinary source form with that property is a query that returns a small object by value and
// is compared with its zero value, guarding the code that follows:
//
//     if (BuildMode() == kBuildRetail) { ... }      // always true, leaves no instruction
//
// A statement that merely vanishes in release (an assert, a debug print) does not do it: the front end deletes it, or
// its dead call stays behind as unreachable code, which retail does not have. What the original asked at these places
// is unknown; this is the one shared spelling for all of them (the per-site structs it replaces were code-identical).
// The compared value must be 0: in achview's tally run the test shares the loop counter's zero register.
enum BuildKind {
    kBuildRetail,  // 0
    kBuildDebug
};

struct BuildQuery {
    BuildKind kind;
    BuildQuery(BuildKind k) : kind(k) {}
    bool operator==(BuildKind k) const { return kind == k; }
};

inline BuildQuery BuildMode(void) { return BuildQuery(kBuildRetail); }

#endif
