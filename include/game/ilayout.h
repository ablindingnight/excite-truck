#ifndef GAME_ILAYOUT_H
#define GAME_ILAYOUT_H
// A screen layout's lookup interface (ovlayout.cpp's Layout implements it): the named text, art, box and art-set
// elements, and the in-game layout editor. One declaration for every unit (port ODR work, 2026-11-16).
#include "game/common.h"

struct TextElem;
struct ArtElem;
struct GartElem;
struct gArtSet;

class ILayout {
public:
    ILayout(void);
    virtual ~ILayout(void) {}
    virtual TextElem* Find(const char* name) = 0;
    virtual ArtElem* FindArt(const char* name) = 0;
    virtual GartElem* FindBox(const char* name) = 0;
    virtual gArtSet* FindSet(const char* name) = 0;
    virtual void Edit(void) = 0;
    virtual void ToggleEdit(void) = 0;
    virtual int IsEditing(void) = 0;
    virtual void Save(void) = 0;
};

#endif
