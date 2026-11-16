#ifndef GAME_LOCSTRING_H
#define GAME_LOCSTRING_H
// A localised string looked up by key and cached (locale.cpp): Get() re-fetches the text after a language
// change (lbl_8055E044 counts them). One declaration for every unit (port ODR work, 2026-11-16).
#include "game/common.h"

class LocString {
public:
    LocString(const char* key);  // locale.cpp
    const char* Get(void);
    void Refresh(void);
    void Update(void);

    const char* key;   // offset 0x0
    const char* text;  // offset 0x4
    u32 version;       // offset 0x8: the localisation version the text was fetched for
};

extern u32 lbl_8055E044;  // localisation version (locale.cpp)
extern "C" void fn_80131318(LocString* s);  // fetches the text again

inline void LocString::Refresh(void) { fn_80131318(this); }

inline const char* LocString::Get(void) {
    if (version != lbl_8055E044) {
        fn_80131318(this);
    }
    return text;
}

inline void LocString::Update(void) {
    if (version != lbl_8055E044) {
        fn_80131318(this);
    }
}

#endif
