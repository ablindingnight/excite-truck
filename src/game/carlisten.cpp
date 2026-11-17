// Two listener registries (0x801D0590-0x801D0BE4): objects add themselves and get broadcast
// callbacks; car events are only forwarded for car-type objects. Names are guesses.
#include "game/common.h"
#include "game/carbody.h"
#include "game/arrayref.h"

static inline int IsCar(const CarBody* o) {
    u32 t = o->type;
    int r = 0;
    if (t == 'CAR ' || t == 'PCAR' || t == 'ACAR' || t == 'NCAR') {
        r = 1;
    }
    return r;
}

class CarListener {
public:
    virtual void OnA(int a);                  // 0x8
    virtual void OnB(int a);                  // 0xC
    virtual void OnCar(int a, s32 id, int b);  // 0x10
};

class RaceListener {
public:
    virtual void OnA(int a);         // 0x8
    virtual void OnB(int a);         // 0xC
    virtual void OnC(int a, int b);  // 0x10
    virtual void OnD(int a, int b);  // 0x14
};

// The lists' own lookup helpers (unit-local: their bodies differ from other units').
struct CarListenerList : ArrayRef<CarListener*> {
    CarListenerList(void) : ArrayRef<CarListener*>(storage, 16) {}
    int Find(CarListener* v) const {
        int i;
        for (i = 0; i < count; i++) {
            if (data[i] == v) {
                return i;
            }
        }
        return -1;
    }
    void RemoveAt(int i) {
        if (i >= 0 && i < count) {
            int last = count - 1;
            if (i != last) {
                data[i] = data[last];
            }
            count--;
        }
    }
    void Remove(CarListener* v) {
        int i = Find(v);
        if (i != -1) {
            RemoveAt(i);
        }
    }
    CarListener* storage[16];
};

struct RaceListenerList : ArrayRef<RaceListener*> {
    RaceListenerList(void) : ArrayRef<RaceListener*>(storage, 16) {}
    int Find(RaceListener* v) const {
        int i;
        for (i = 0; i < count; i++) {
            if (data[i] == v) {
                return i;
            }
        }
        return -1;
    }
    void RemoveAt(int i) {
        if (i >= 0 && i < count) {
            int last = count - 1;
            if (i != last) {
                data[i] = data[last];
            }
            count--;
        }
    }
    void Remove(RaceListener* v) {
        int i = Find(v);
        if (i != -1) {
            RemoveAt(i);
        }
    }
    RaceListener* storage[16];
};

ModuleTag lbl_8055E8C8;
CarListenerList lbl_804386B4;
RaceListenerList lbl_8043870C;

extern "C" {

void fn_801D0590(CarListener* l) { *lbl_804386B4.Next() = l; }

void fn_801D05B4(CarListener* l) { lbl_804386B4.Remove(l); }

void fn_801D0650(int a) {
    int i;
    for (i = 0; i < lbl_804386B4.count; i++) {
        lbl_804386B4.data[i]->OnA(a);
    }
}

void fn_801D06C4(int a) {
    int i;
    for (i = 0; i < lbl_804386B4.count; i++) {
        lbl_804386B4.data[i]->OnB(a);
    }
}

void fn_801D0738(int a, int unused, CarBody* car, int b) {
    if (IsCar(car)) {
        s32 id = car->index;
        int i;
        for (i = 0; i < lbl_804386B4.count; i++) {
            lbl_804386B4.data[i]->OnCar(a, id, b);
        }
    }
}

void fn_801D0800(RaceListener* l) { *lbl_8043870C.Next() = l; }

void fn_801D0824(RaceListener* l) { lbl_8043870C.Remove(l); }

void fn_801D08C0(int a) {
    int i;
    for (i = 0; i < lbl_8043870C.count; i++) {
        lbl_8043870C.data[i]->OnA(a);
    }
}

void fn_801D0934(int a) {
    int i;
    for (i = 0; i < lbl_8043870C.count; i++) {
        lbl_8043870C.data[i]->OnB(a);
    }
}

void fn_801D09A8(int a, int b) {
    int i;
    for (i = 0; i < lbl_8043870C.count; i++) {
        lbl_8043870C.data[i]->OnC(a, b);
    }
}

void fn_801D0A24(int a, int b) {
    int i;
    for (i = 0; i < lbl_8043870C.count; i++) {
        lbl_8043870C.data[i]->OnD(a, b);
    }
}

}  // extern "C"
