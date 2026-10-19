// Preloaders (0x802C2914-0x802C2A48): objects that link themselves into a list; a pass totals their sizes,
// reserves the memory and has each one load. Names are guesses.
#include "game/common.h"

extern "C" void fn_802BAD9C(int size);

struct ListNode {
    ListNode* next;  // offset 0x0
};

class Preloader : public ListNode {
public:
    Preloader(void);
    virtual int Size(void) = 0;
    virtual void Load(void) = 0;
    virtual ~Preloader(void);
    virtual void vf14(void) = 0;
};

ModuleTag lbl_8055FB78;
ListNode* lbl_8055FB80;
u8 lbl_8055FB84;

Preloader::Preloader(void) {
    next = lbl_8055FB80;
    lbl_8055FB80 = this;
}

Preloader::~Preloader(void) {
    ListNode* p;
    ListNode** pp = &lbl_8055FB80;
    for (; (p = *pp) != 0; pp = &p->next) {
        if (p == this) {
            *pp = p->next;
            break;
        }
    }
}

extern "C" {

void fn_802C29A0(void) {
    int total = 0;
    ListNode* p;
    ListNode* q;
    for (p = lbl_8055FB80; p; p = p->next) {
        total += ((Preloader*)p)->Size();
    }
    fn_802BAD9C(total);
    for (q = lbl_8055FB80; q; q = q->next) {
        ((Preloader*)q)->Load();
    }
    lbl_8055FB84 = 1;
}

void fn_802C2A34(void) { lbl_8055FB84 = 0; }

}  // extern "C"
