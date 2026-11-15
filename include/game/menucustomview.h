#ifndef GAME_MENUCUSTOMVIEW_H
#define GAME_MENUCUSTOMVIEW_H

// A custom view drawn inside a ModelWidget (uiapi item type 0x13). widgets.cpp defines MenuCustomView's defaults
// and emits its vtable (lbl_8033FBC0; the pure slots are 0). achview.cpp and caruimod.cpp still declare local
// copies (with an all-pure IMenuCustomView base, which widgets' vtable and destructor do not show).
// Class and slot names are unproven.
class MenuCustomView {
public:
    MenuCustomView(void) {}
    virtual ~MenuCustomView(void) {}                                         // out-of-line copy fn_8022882C
    virtual void Load(const char* name) = 0;                                  // 0x0C
    virtual void Reset(void);                                                 // 0x10: fn_80228990
    virtual void Update(int x, int y, int width, int height, float dt) = 0;   // 0x14
    virtual void Unk18(void);
    virtual void Draw(void) = 0;                                              // 0x1C
    virtual int Unk20(void);
    virtual void Unk24(void);                                                 // fn_80228DC0
};

#endif
