// JumpCam (0x8026B118-0x8026B844): jump points read from the event's jump file; when the local
// player launches off one, the camera switches to a jump view with heart/whoosh sounds. Names are
// guesses apart from the messages and keys.
#include "game/common.h"

extern "C" {
int strcasecmp(const char* a, const char* b);
char* strncpy(char* d, const char* s, size_t n);
char* strchr(const char* s, int c);
char* strcat(char* d, const char* s);
void* memset(void* p, int c, size_t n);
void fn_802E30C8(const char* fmt, ...);  // debug print
int fn_8012C33C(const char* name);
ValueTable* fn_80131808(const char* name);  // load value file
void fn_80131818(ValueTable* t);
void fn_801331C4(int section, const char* name, f32* v);  // register a tunable
int* fn_801769CC(void);
s32 fn_80178CA4(void);
void fn_8020C900(int on);
void fn_80206444(void);
void fn_80206454(void);
void fn_802C2C68(int a, int b);
void fn_80176A00(int mode);
int fn_801B2F24(void);
void fn_8026A1F0(int cam, int player, int unk);
f32 fn_801E9FDC(const s16* hint, f32 x, f32 z);  // ground height
}

class RSound {
public:
    virtual ~RSound(void);
    u8 pad4[0x4];
    f32 vol;  // offset 0x8
    u8 padC[0x49 - 0xC];
    u8 playing;  // offset 0x49
    u8 stop;     // offset 0x4A
    u8 loop;     // offset 0x4B
    u8 f4C;      // offset 0x4C
    u8 dirty;    // offset 0x4D
    void SetVol(f32 v) {
        if (v != vol) {
            dirty = 1;
            vol = v;
        }
    }
    void Play(void) {
        playing = 1;
        loop = 0;
        stop = 0;
    }
    void PlayLoop(void) {
        stop = 0;
        loop = 1;
        playing = 1;
    }
    void Stop(void) {
        stop = 1;
        f4C = 0;
    }
};

extern "C" RSound* fn_80207848(const char* name, int a);

class EventInfo {
public:
    u8 pad0[0x20];
    ValueNode* table;  // offset 0x20
};

EventInfo* GetEvent(int id);

struct JumpCar {
    u8 pad0[0x60];
    f32 x;     // offset 0x60
    f32 y;     // offset 0x64
    f32 z;     // offset 0x68
    s16 hint;  // offset 0x6C
    u8 pad6E[0x1F0 - 0x6E];
    s32 type;  // offset 0x1F0
    u8 pad1F4[0x2620 - 0x1F4];
    s32 airborne;  // offset 0x2620
};

extern "C" JumpCar* fn_8017960C(int id);

struct JumpPt {
    JumpPt(void) {}
    f32 x;    // offset 0x0
    f32 z;    // offset 0x4
    f32 rad;  // offset 0x8
};

struct JumpState {
    u8 active;   // offset 0x0
    f32 time;    // offset 0x4
    f32 blend;   // offset 0x8
    s32 jump;    // offset 0xC
};

struct V2 {
    f32 x, z;
};

extern f32 lbl_8055E3C8;  // race clock

f32 lbl_8055C4F8 = 1.0f;  // effects volume

ModuleTag lbl_8055F4B8;
UnkStaticFFFF lbl_8055F4C0;
static UnkStatic123 lbl_8049A010;
UnkStatic7F lbl_8055F4C4;
s32 lbl_8055F4C8;       // jump count
RSound* lbl_8055F4CC;   // whoosh
RSound* lbl_8055F4D0;   // heartbeat
JumpPt lbl_8049A028[16];
JumpState lbl_8049A0E8[8];

extern "C" {

void fn_8026B118(void) {}

void fn_8026B11C(RSound** out, const char* name) {
    RSound* s = fn_80207848(name, 0);
    *out = s;
    if (s) {
        s->SetVol(lbl_8055C4F8);
    }
}

void fn_8026B178(RSound** p) {
    if (*p) {
        delete *p;
        *p = 0;
    }
}

void fn_8026B1D0(ValueNode* n) {
    JumpPt* j = &lbl_8049A028[lbl_8055F4C8];
    ValPoint pt = n->GetPoint("pt");
    j->x = pt.x;
    j->z = pt.y;
    j->rad = n->GetFloat("rad", 1.0f);
}

void fn_8026B260(void) {
    char name[24];
    const char* file;
    lbl_8055F4C8 = 0;
    file = GetEvent(*fn_801769CC())->table->GetString("jump_file", 0);
    if (file) {
        char* dot;
        strncpy(name, file, 11);
        dot = strchr(name, '.');
        if (dot) {
            *dot = 0;
        }
        strcat(name, ".val");
        if (!fn_8012C33C(name)) {
            fn_802E30C8("JumpCam: Could not open jump file %s!", name);
        } else {
            ValueTable* t = fn_80131808(name);
            int i = 0;
            ValueNode* n;
            do {
                n = t->GetChild(i++);
                if (n && !strcasecmp(n->GetName(), "Jump")) {
                    fn_8026B1D0(n);
                    lbl_8055F4C8++;
                }
            } while (n);
            fn_80131818(t);
            fn_802E30C8("JumpCam: Loaded %d jumps from file %s", lbl_8055F4C8, name);
        }
    } else {
        fn_802E30C8("JumpCam: no jumps file");
    }
}

// NonMatching: the jump-radius loop gets different float registers (14 instructions).
void fn_8026B3D4(int player) {
    f32 now = lbl_8055E3C8;
    JumpCar* c = fn_8017960C(player);
    if (fn_80178CA4() == 1 && c->type == 'P') {
        JumpState* s = &lbl_8049A0E8[player];
        if (s->active) {
            if ((c->airborne >= 2 && now - s->time > 1.0f) || now - lbl_8049A0E8[player].time > 3.0f) {
                s->active = 0;
                if (lbl_8055F4D0) {
                    lbl_8055F4D0->Stop();
                }
            } else {
                fn_8020C900(1);
                fn_80206444();
                if (lbl_8055F4D0) {
                    lbl_8055F4D0->PlayLoop();
                }
                fn_802C2C68(0, 1);
                fn_80176A00(2);
                lbl_8049A0E8[player].blend = 0.85f;
            }
        } else {
            V2 p;
            JumpPt* j;
            int found;
            int i;
            found = 0;
            p.x = c->x;
            p.z = c->z;
            for (i = 0, j = lbl_8049A028; i < lbl_8055F4C8 && !found; j++, i++) {
                V2 d;
                d.x = p.x - j->x;
                d.z = p.z - j->z;
                if (d.x * d.x + d.z * d.z < j->rad * j->rad) {
                    s->jump = i;
                    found = 1;
                    s->time = now;
                }
            }
            if (now - lbl_8049A0E8[player].time < 1.0f && c->airborne == 0) {
                if (c->y - fn_801E9FDC(&c->hint, c->x, c->z) > 1.0f) {
                    s->active = 1;
                    if (lbl_8055F4CC) {
                        lbl_8055F4CC->Play();
                    }
                }
            } else {
                fn_802C2C68(-1, 0);
                fn_80206454();
                fn_80176A00(1);
                fn_8020C900(0);
                lbl_8049A0E8[player].blend = 0.0f;
            }
        }
    }
}


}  // extern "C"

// Debug dump; unreferenced, so the linker strips it, but its strings stay in .data.
void JumpCamDump(void) {
    int i;
    fn_802E30C8("JumpCam Dump:");
    for (i = 0; i < lbl_8055F4C8; i++) {
        fn_802E30C8("%d: (%0.3f,%0.3f), %0.2fm", i, lbl_8049A028[i].x, lbl_8049A028[i].z, lbl_8049A028[i].rad);
    }
}

extern "C" {

void fn_8026B644(void) {
    fn_8026B260();
    fn_801331C4(4, "effects_volume", &lbl_8055C4F8);
    fn_8026B11C(&lbl_8055F4D0, "heart.sfx");
    fn_8026B11C(&lbl_8055F4CC, "whoosh.sfx");
}

void fn_8026B69C(void) {
    lbl_8055F4C8 = 0;
    fn_8026B178(&lbl_8055F4CC);
    fn_8026B178(&lbl_8055F4D0);
}

void fn_8026B6D0(void) { memset(lbl_8049A0E8, 0, sizeof(lbl_8049A0E8)); }

void fn_8026B6E4(void) {
    if (lbl_8055F4C8 > 0) {
        int i;
        for (i = 0; i < fn_801B2F24(); i++) {
            fn_8026B3D4(i);
        }
    }
}

int fn_8026B734(int player, f32* out) {
    if (lbl_8049A0E8[player].active) {
        *out = lbl_8049A0E8[player].blend;
        return 1;
    }
    return 0;
}

int fn_8026B768(int player, int cam) {
    if (lbl_8049A0E8[player].active) {
        fn_8026A1F0(11, cam, 0);
        return 1;
    }
    return 0;
}

u8 fn_8026B7B4(int player) { return lbl_8049A0E8[player].active; }

}  // extern "C"
