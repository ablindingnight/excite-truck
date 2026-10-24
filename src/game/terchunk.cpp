// Terrain chunks (0x80202218-0x80202BE4): the track's terrain split into chunks from
// terchunk.val, streamed in a window around the nearest chunk to the camera, plus the far
// models. Names are guesses apart from the file names, keys and messages.
#include "game/common.h"

// Dot product taking the y term first (x*mx + y*my + z*mz).
static inline f32 Dot3YX(f32 ay, f32 by, f32 ax, f32 bx, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

extern "C" {
int sprintf(char* buf, const char* fmt, ...);
double fmod(double x, double y);
void fn_802E30C8(const char* fmt, ...);  // debug print
int fn_8012C33C(const char* name);       // resource exists
ValueTable* fn_80131808(const char* name);
void fn_80131818(ValueTable* table);
s32 fn_802C6FC0(const char* name, int a, int b);
void fn_802C6CF8(s32 inst);
void fn_802C7040(s32 inst);
void fn_802C7A44(void);
void fn_802C7A50(void);
void fn_802C7458(s32 inst, int a);
void fn_802C70AC(s32 inst, const ValMatrix* m, s32 color, int flags);
s32 fn_802C7A3C(f32 a, f32 b, f32 c);
int fn_802C9C58(int mode);
f32 PSVECSquareDistance(const ValVec3* a, const ValVec3* b);
int fn_801E85A4(void);
int fn_801E85B4(void);
void fn_801E86F8(int a);
f32 fn_801E7E18(void);
void fn_801DCEF0(int a);
}

struct ModelPool;
struct ModelSlot;

extern "C" {
ModelPool* fn_802CE3A4(s32 count, s32 max, s32 slotSize);
void fn_802CE49C(ModelPool* p);
void fn_802CE4F4(ModelPool* p);
ModelSlot* fn_802CE630(ModelPool* p, const char* name, s32 arg);
void fn_802CE6C0(ModelSlot* s);
s32 fn_802CE728(ModelSlot* s);
void fn_802CE744(ModelSlot* s, int a);
int fn_802CE850(ModelSlot* s);
}

void* operator new[](size_t size, const char* tag);

struct DebugFlags {
    u8 pad0[0x10];
    u32 flags;  // offset 0x10
};

struct ViewCam {
    f32 m[3][3];  // offset 0x0
    ValVec3 pos;  // offset 0x24
};

struct TrackState {
    u8 pad0[0x30];
    s16 unk30;  // offset 0x30
};

struct TrackInfo {
    int Unk30(void) const { return state->unk30; }
    u8 pad0[0x8];
    TrackState* state;  // offset 0x8
};

extern "C" {
DebugFlags* fn_801E87A8(void);
ViewCam* fn_802C86DC(void);
TrackInfo* fn_801EA9A0(void);
}

struct ChunkPos : ValVec3 {
    ChunkPos(void) {}
};

struct TerrainChunk {
    ChunkPos pos;     // offset 0x0
    ModelSlot* slot;  // offset 0xC
};

struct VisibleList {
    s32 insts[0x20];  // offset 0x0
    u32 count;        // offset 0x80
};

ModuleTag lbl_8055ECB0;
static UnkStatic123 lbl_804888B8;
UnkStaticFFFF lbl_8055ECB8;
UnkStatic7F lbl_8055ECBC;
int lbl_8055B410 = 7;              // chunk window
int lbl_8055B414 = -1;             // forced chunk
int lbl_8055ECC0;                  // chunk count
TerrainChunk* lbl_8055ECC4;        // chunks
ModelPool* lbl_8055ECC8;           // chunk models
s32 lbl_8055ECCC;                  // far model
s32 lbl_8055ECD0;                  // two-sided far model
int lbl_8055ECD4;                  // chunk window in replays
u8 lbl_8055ECD8;                   // terrain disabled
int lbl_8055ECDC;                  // prefetch chunk
u8 lbl_8055ECE0;                   // prefetch requested
int lbl_804888D0[4];               // nearest chunk per player
VisibleList lbl_804888E0;          // drawn chunk models
ValVec3 lbl_80488964;              // prefetch position

static inline void Identity(ValMatrix* m) {
    m->basis[0].x = 1.0f;
    m->basis[0].y = 0.0f;
    m->basis[0].z = 0.0f;
    m->basis[1].x = 0.0f;
    m->basis[1].y = 1.0f;
    m->basis[1].z = 0.0f;
    m->basis[2].x = 0.0f;
    m->basis[2].y = 0.0f;
    m->basis[2].z = 1.0f;
    m->origin.x = 0.0f;
    m->origin.y = 0.0f;
    m->origin.z = 0.0f;
}

static inline void Rotate(ValVec3* out, const ViewCam* m, const ValVec3* d) {
    out->x = Dot3YX(d->y, m->m[0][1], d->x, m->m[0][0], d->z, m->m[0][2]);
    out->y = Dot3YX(d->y, m->m[1][1], d->x, m->m[1][0], d->z, m->m[1][2]);
    out->z = Dot3YX(d->y, m->m[2][1], d->x, m->m[2][0], d->z, m->m[2][2]);
}

extern "C" {

int fn_80202218(void) { return fn_801E85A4() == 1 ? lbl_8055ECD4 : lbl_8055B410; }

s32 fn_80202248(const char* name) {
    s32 inst = -1;
    if (fn_8012C33C(name)) {
        inst = fn_802C6FC0(name, 0, 0);
    }
    return inst;
}

void fn_802022A0(s32* inst) {
    if (*inst != -1) {
        fn_802C6CF8(*inst);
        *inst = -1;
    }
}

int fn_802022E4(const ValVec3* pos) {
    f32 best = 1e8f;
    int nearest = 0;
    for (int i = 0; i < lbl_8055ECC0; i++) {
        f32 d = PSVECSquareDistance(pos, &lbl_8055ECC4[i].pos);
        if (d < best) {
            best = d;
            nearest = i;
        }
    }
    return nearest;
}

// The k-th chunk of the window of +-range chunks around center, walking in dir; -1 past the end.
int fn_80202378(int center, int range, int dir, int k) {
    if (k >= lbl_8055ECC0) {
        return -1;
    }
    if (k >= range * 2 + 1) {
        return -1;
    }
    return (dir * (k - range) + (lbl_8055ECC0 * 100 + center)) % lbl_8055ECC0;
}

ValVec3* fn_802023C8(void) { return &lbl_80488964; }

void fn_802023D4(void) {
    lbl_8055ECD4 = 7;
    ValueTable* table = fn_80131808("terchunk.val");
    ValueNode* list = table->FindChild("chunklist");
    lbl_8055ECC0 = list->GetIntValue("count");
    if (lbl_8055ECC0 > 0) {
        lbl_8055ECC4 = new ("terchunk") TerrainChunk[lbl_8055ECC0];
        lbl_8055ECC8 = fn_802CE3A4(lbl_8055ECC0, 0x20, 0x20000);
        for (int i = 0; i < lbl_8055ECC0; i++) {
            TerrainChunk* c = &lbl_8055ECC4[i];
            char key[0x20];
            sprintf(key, "chunk%d", i);
            const ValVec3& v = list->GetVec3(key);
            c->pos.x = v.x;
            c->pos.y = v.y;
            c->pos.z = v.z;
            char name[0x20];
            sprintf(name, "trk%d.mo0", i);
            c->slot = fn_802CE630(lbl_8055ECC8, name, 0);
        }
    } else {
        lbl_8055ECC4 = NULL;
        lbl_8055ECC8 = NULL;
    }
    fn_80131818(table);
    lbl_8055ECCC = fn_80202248("farmodel.mod");
    if (lbl_8055ECCC != -1) {
        fn_802E30C8("terchunk: has farmodel");
    }
    lbl_8055ECD0 = fn_80202248("farmodel_2.mod");
    if (lbl_8055ECD0 != -1) {
        fn_802E30C8("terchunk: has two-sided farmodel");
    }
    lbl_8055B414 = -1;
    lbl_80488964.x = 0.0f;
    lbl_80488964.y = 0.0f;
    lbl_80488964.z = 0.0f;
    lbl_8055ECE0 = 0;
    lbl_8055ECDC = -1;
    lbl_804888D0[0] = -1;
    lbl_804888D0[1] = -1;
    lbl_804888D0[2] = -1;
    lbl_804888D0[3] = -1;
    lbl_804888E0.count = 0;
}

void fn_802025D8(void) {
    fn_802022A0(&lbl_8055ECD0);
    fn_802022A0(&lbl_8055ECCC);
    for (int i = 0; i < lbl_8055ECC0; i++) {
        fn_802CE6C0(lbl_8055ECC4[i].slot);
    }
    if (lbl_8055ECC0 > 0) {
        fn_802CE49C(lbl_8055ECC8);
        lbl_8055ECC8 = NULL;
        delete[] lbl_8055ECC4;
        lbl_8055ECC4 = NULL;
    }
    lbl_8055ECC0 = 0;
    lbl_804888E0.count = 0;
}

void fn_8020268C(void) {
    DebugFlags* f = fn_801E87A8();
    int mode = 2;
    if ((f->flags >> 1) & 1) {
        mode = 1;
    }
    mode = fn_802C9C58(mode);
    if (lbl_8055ECCC != -1) {
        fn_802C7A44();
        fn_802C7040(lbl_8055ECCC);
        fn_802C7A50();
    }
    fn_802C9C58(0);
    if (lbl_8055ECD0 != -1) {
        fn_802C7A44();
        fn_802C7040(lbl_8055ECD0);
        fn_802C7A50();
    }
    fn_802C9C58(mode);
}

void fn_80202718(void) {
    lbl_804888E0.count = 0;
    if (lbl_8055ECD8) {
        fn_801E86F8(-1);
        int lap = fn_801EA9A0()->state->unk30;
        if (lap >= 0) {
            fn_801DCEF0(lap);
        }
        return;
    }
    if (lbl_8055ECC0 == 0) {
        fn_8020268C();
        return;
    }
    ViewCam* cam = fn_802C86DC();
    int center = fn_802022E4(&cam->pos);
    lbl_804888D0[fn_801E85B4()] = center;
    TerrainChunk* next = &lbl_8055ECC4[(center + 1) % lbl_8055ECC0];
    f32 dx = next->pos.x - cam->pos.x;
    f32 dy = next->pos.y - cam->pos.y;
    f32 dz = next->pos.z - cam->pos.z;
    ValVec3 v;
    v.x = Dot3YX(dy, cam->m[0][1], dx, cam->m[0][0], dz, cam->m[0][2]);
    v.z = Dot3YX(dy, cam->m[2][1], dx, cam->m[2][0], dz, cam->m[2][2]);
    v.y = Dot3YX(dy, cam->m[1][1], dx, cam->m[1][0], dz, cam->m[1][2]);
    int dir = 1;
    if (v.z < 0.0f) {
        dir = -1;
    }
    int range = fn_80202218();
    if (lbl_8055B414 >= 0) {
        range = 0;
        center = lbl_8055B414;
    }
    int idx;
    for (int k = 0; (idx = fn_80202378(center, range, dir, k)) >= 0; k++) {
        TerrainChunk* c = &lbl_8055ECC4[idx];
        fn_802CE744(c->slot, 0);
        s32 inst = fn_802CE728(c->slot);
        if (inst != -1) {
            fn_802C7458(inst, -13);
            if (lbl_804888E0.count < 0x20) {
                lbl_804888E0.insts[lbl_804888E0.count++] = inst;
            }
        }
    }
    fn_8020268C();
}

void fn_80202924(void) {
    if (lbl_8055ECC0 > 0) {
        lbl_8055ECDC = -1;
        if (lbl_8055ECE0) {
            ValVec3 p = *fn_802023C8();
            int center = fn_802022E4(&p);
            lbl_8055ECDC = center;
            int range = fn_80202218();
            int idx;
            for (int k = 0; (idx = fn_80202378(center, range, 1, k)) >= 0; k++) {
                fn_802CE744(lbl_8055ECC4[idx].slot, 0);
            }
        }
        lbl_8055ECE0 = 0;
        fn_802CE4F4(lbl_8055ECC8);
    }
}

void fn_802029FC(void) {
    s32 color = fn_802C7A3C(1.0f - (f32)fmod(fn_801E7E18(), 1.0), 0.0f, 50.0f);
    ValMatrix m;
    Identity(&m);
    for (int i = 0; i < (int)lbl_804888E0.count; i++) {
        fn_802C70AC(lbl_804888E0.insts[i], &m, color, 8);
    }
}

void fn_80202AD4(const ValVec3* pos) {
    lbl_80488964.x = pos->x;
    lbl_80488964.y = pos->y;
    lbl_80488964.z = pos->z;
    lbl_8055ECE0 = 1;
}

int fn_80202B00(void) {
    if (lbl_8055ECC0 > 0) {
        int n = 0;
        for (int i = 0; i < lbl_8055ECC0; i++) {
            if (fn_802CE850(lbl_8055ECC4[i].slot) == 1) {
                n++;
            }
        }
        return n;
    }
    return 0;
}

}  // extern "C"

// NonMatching: fn_80202718 schedules the camera-space transform differently, and the sinit
// allocates its registers differently (42 diffs in all).
