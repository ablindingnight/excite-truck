// Physics collision meshes (0x801CF080-0x801D02B0): "PMsh" resources of bounding-sphere cells over
// triangles (plane, per-edge inward planes, vertex/edge flags), with a sphere contact query, a raycast,
// the endian swap/relocation on load and a back-face-culled wireframe debug draw. Names are guesses.
#include "game/common.h"

struct PVec3 {
    f32 x, y, z;
};

struct PFrame {
    PVec3 x;    // offset 0x0
    PVec3 y;    // offset 0xC
    PVec3 z;    // offset 0x18
    PVec3 pos;  // offset 0x24
};

struct PCell {
    PVec3 center;  // offset 0x0
    f32 radius;    // offset 0xC
    s32 first;     // offset 0x10
    s32 end;       // offset 0x14
};

// flags bits 0-2: vertex k is a contact vertex; bits 3-5: edge k is a contact edge.
struct PTri {
    u16 flags;       // offset 0x0
    s16 v[3];        // offset 0x2
    PVec3 n;         // offset 0x8
    f32 d;           // offset 0x14
    PVec3 edge[3];   // offset 0x18
};

struct PMesh {
    PCell* cells;    // offset 0x0
    s32 numCells;    // offset 0x4
    PVec3* verts;    // offset 0x8
    s32 numVerts;    // offset 0xC
    PTri* tris;      // offset 0x10
    s32 numTris;     // offset 0x14
};

extern "C" {
void PSVECCrossProduct(const PVec3* a, const PVec3* b, PVec3* out);
f32 PSVECDotProduct(const PVec3* a, const PVec3* b);
f32 PSVECSquareDistance(const PVec3* a, const PVec3* b);
void* fn_8012C370(const char* name, u32 type, u32* version, u32* size, u8* first, u8* swapped);
void fn_8012C4D8(void* res);
void fn_802C8B08(PFrame* cam);
void fn_801E41D8(const PVec3* a, const PVec3* b, u32 color, f32 width);
}

extern f32 lbl_80558DC4[];  // FLT_EPSILON

static inline f32 Dot3(f32 ax, f32 bx, f32 ay, f32 by, f32 az, f32 bz) { return ax * bx + ay * by + az * bz; }

static inline void MulVec(PVec3* out, const PVec3* in, const PFrame* m) {
    if (out == in) {
        f32 x = Dot3(in->x, m->x.x, in->y, m->y.x, in->z, m->z.x);
        f32 y = Dot3(in->x, m->x.y, in->y, m->y.y, in->z, m->z.y);
        f32 z = Dot3(in->x, m->x.z, in->y, m->y.z, in->z, m->z.z);
        out->x = x;
        out->y = y;
        out->z = z;
    } else {
        out->x = Dot3(in->x, m->x.x, in->y, m->y.x, in->z, m->z.x);
        out->y = Dot3(in->x, m->x.y, in->y, m->y.y, in->z, m->z.y);
        out->z = Dot3(in->x, m->x.z, in->y, m->y.z, in->z, m->z.z);
    }
}

static inline void PSwap16(void* p) {
    u8 t;
    t = ((u8*)p)[0];
    ((u8*)p)[0] = ((u8*)p)[1];
    ((u8*)p)[1] = t;
}

static inline void PSwap32(void* p) {
    u8 t;
    t = ((u8*)p)[0];
    ((u8*)p)[0] = ((u8*)p)[3];
    ((u8*)p)[3] = t;
    t = ((u8*)p)[1];
    ((u8*)p)[1] = ((u8*)p)[2];
    ((u8*)p)[2] = t;
}

static inline u32 PColor(u8 a, u8 r, u8 g, u8 b) { return (a << 24) | (r << 16) | (g << 8) | b; }

ModuleTag lbl_8055E8A8;
u32 lbl_8055E8B0 = PColor(0xFF, 0xFF, 0, 0);

extern "C" {

// Contact points of a sphere (pos, radius) with the mesh placed at xf (or untransformed). Writes up to
// *count points to out and returns the number written in *count.
void fn_801CF080(PMesh* mesh, const PVec3* pos, PVec3* out, int* count, const PFrame* xf, f32 radius) {
    PVec3 lp;
    if (xf) {
        lp.x = pos->x - xf->pos.x;
        lp.y = pos->y - xf->pos.y;
        lp.z = pos->z - xf->pos.z;
        f32 x = Dot3(lp.x, xf->x.x, lp.y, xf->x.y, lp.z, xf->x.z);
        f32 y = Dot3(lp.x, xf->y.x, lp.y, xf->y.y, lp.z, xf->y.z);
        f32 z = Dot3(lp.x, xf->z.x, lp.y, xf->z.y, lp.z, xf->z.z);
        lp.x = x;
        lp.y = y;
        lp.z = z;
    } else {
        lp.x = pos->x;
        lp.y = pos->y;
        lp.z = pos->z;
    }
    int max = *count;
    int n = 0;
    for (int i = 0; i < mesh->numCells; i++) {
        PCell* c = &mesh->cells[i];
        if (PSVECSquareDistance(&c->center, &lp) > (c->radius + radius) * (c->radius + radius)) {
            continue;
        }
        for (int j = c->first; j < c->end; j++) {
            PTri* t = &mesh->tris[j];
            f32 dist = t->d - PSVECDotProduct(&lp, &t->n);
            if (-dist > radius || -dist < 0.0f) {
                continue;
            }
            PVec3 proj;
            proj.x = lp.x + t->n.x * dist;
            proj.y = lp.y + t->n.y * dist;
            proj.z = lp.z + t->n.z * dist;
            PVec3 tv[3];
            tv[0] = mesh->verts[t->v[0]];
            tv[1] = mesh->verts[t->v[1]];
            tv[2] = mesh->verts[t->v[2]];
            int outIdx[3];
            f32 ed[3];
            int outside = 0;
            int inside = -1;
            int k;
            for (k = 0; k < 3; k++) {
                PVec3 d;
                d.x = proj.x - tv[k].x;
                d.y = proj.y - tv[k].y;
                d.z = proj.z - tv[k].z;
                f32 e = PSVECDotProduct(&d, &t->edge[k]);
                if (e > radius) {
                    goto next;
                }
                if (e >= 0.0f) {
                    outIdx[outside++] = k;
                } else {
                    inside = k;
                }
                ed[k] = e;
            }
            if (outside == 3) {
                continue;
            }
            {
                u16 flags = t->flags;
                PVec3 cp = proj;
                if (outside == 1) {
                    int k0 = outIdx[0];
                    f32 s = -ed[k0];
                    cp.x = proj.x + t->edge[k0].x * s;
                    cp.y = proj.y + t->edge[k0].y * s;
                    cp.z = proj.z + t->edge[k0].z * s;
                    int k1 = (k0 + 1) % 3;
                    PVec3 a = tv[k0];
                    PVec3 b = tv[k1];
                    f32 dab = PSVECSquareDistance(&a, &b);
                    f32 dca = PSVECSquareDistance(&cp, &a);
                    f32 dcb = PSVECSquareDistance(&cp, &b);
                    if (dca > dab) {
                        inside = k1 - 2;
                        outside = 2;
                    } else if (dcb > dab) {
                        outside = 2;
                        inside = k0 - 2;
                    }
                }
                if (outside == 0) {
                    cp.x = proj.x;
                    cp.y = proj.y;
                    cp.z = proj.z;
                } else if (outside == 2) {
                    int v = (inside + 2) % 3;
                    if (!(flags & (1 << v))) {
                        continue;
                    }
                    cp.x = tv[v].x;
                    cp.y = tv[v].y;
                    cp.z = tv[v].z;
                } else if (outside == 1) {
                    int k0 = outIdx[0];
                    if (!(flags & (1 << (k0 + 3)))) {
                        continue;
                    }
                    f32 s = -ed[k0];
                    cp.x = proj.x + t->edge[k0].x * s;
                    cp.y = proj.y + t->edge[k0].y * s;
                    cp.z = proj.z + t->edge[k0].z * s;
                }
                if (PSVECSquareDistance(&cp, &lp) < radius * radius && n < max) {
                    PVec3 w;
                    if (xf) {
                        MulVec(&w, &cp, xf);
                        w.x += xf->pos.x;
                        w.y += xf->pos.y;
                        w.z += xf->pos.z;
                    } else {
                        w.x = cp.x;
                        w.y = cp.y;
                        w.z = cp.z;
                    }
                    out[n].x = w.x;
                    out[n].y = w.y;
                    out[n].z = w.z;
                    n++;
                }
            }
        next:;
        }
    }
    *count = n;
}

// Squared distance of p from the line through o along dir (unit).
f32 fn_801CF778(const PVec3* p, const PVec3* o, const PVec3* dir) {
    PVec3 d;
    d.x = p->x - o->x;
    d.y = p->y - o->y;
    d.z = p->z - o->z;
    f32 t = -PSVECDotProduct(&d, dir);
    d.x += dir->x * t;
    d.y += dir->y * t;
    d.z += dir->z * t;
    return PSVECDotProduct(&d, &d);
}

// Ray against the mesh; *t holds the current best distance and is lowered on a hit.
int fn_801CF828(PMesh* mesh, const PVec3* org, const PVec3* dir, f32* t, PVec3* nrm) {
    int hit = 0;
    for (int i = 0; i < mesh->numCells; i++) {
        PCell* c = &mesh->cells[i];
        f32 r2 = c->radius * c->radius;
        if (fn_801CF778(&c->center, org, dir) > r2) {
            continue;
        }
        for (int j = c->first; j < c->end; j++) {
            PTri* tr = &mesh->tris[j];
            PVec3* n = &tr->n;
            f32 d = tr->d;
            f32 dn = PSVECDotProduct(dir, n);
            if ((f32)__fabs(dn) < 0.001f) {
                continue;
            }
            f32 s = (d - PSVECDotProduct(org, n)) / dn;
            if (s >= 0.0f && s < *t) {
                PVec3 p;
                p.x = org->x + dir->x * s;
                p.y = org->y + dir->y * s;
                p.z = org->z + dir->z * s;
                int in = 1;
                for (int k = 0; k < 3; k++) {
                    PVec3 v = mesh->verts[tr->v[k]];
                    PVec3 e;
                    e.x = p.x - v.x;
                    e.y = p.y - v.y;
                    e.z = p.z - v.z;
                    if (PSVECDotProduct(&e, &tr->edge[k]) > lbl_80558DC4[0]) {
                        in = 0;
                    }
                }
                if (in) {
                    *t = s;
                    hit = 1;
                    nrm->x = n->x;
                    nrm->y = n->y;
                    nrm->z = n->z;
                }
            }
        }
    }
    return hit;
}

void fn_801CFA48(PMesh* m) {
    PSwap32(&m->cells);
    PSwap32(&m->numCells);
    PSwap32(&m->verts);
    PSwap32(&m->numVerts);
    PSwap32(&m->tris);
    PSwap32(&m->numTris);
    u8* p = (u8*)(m + 1);
    m->cells = (PCell*)p;
    p += m->numCells * sizeof(PCell);
    m->verts = (PVec3*)p;
    p += m->numVerts * sizeof(PVec3);
    m->tris = (PTri*)p;
    int i;
    for (i = 0; i < m->numCells; i++) {
        PCell* c = &m->cells[i];
        PSwap32(&c->center.x);
        PSwap32(&c->center.y);
        PSwap32(&c->center.z);
        PSwap32(&c->radius);
        PSwap32(&c->first);
        PSwap32(&c->end);
    }
    for (i = 0; i < m->numVerts; i++) {
        PVec3* v = &m->verts[i];
        PSwap32(&v->x);
        PSwap32(&v->y);
        PSwap32(&v->z);
    }
    for (i = 0; i < m->numTris; i++) {
        PTri* t = &m->tris[i];
        PSwap16(&t->flags);
        for (int k = 0; k < 3; k++) {
            PSwap16(&t->v[k]);
            PSwap32(&t->edge[k].x);
            PSwap32(&t->edge[k].y);
            PSwap32(&t->edge[k].z);
        }
        PSwap32(&t->n.x);
        PSwap32(&t->n.y);
        PSwap32(&t->n.z);
        PSwap32(&t->d);
    }
}

PMesh* fn_801CFEAC(const char* name) {
    u8 swapped = 0;
    u32 ver = 1;
    PMesh* m = (PMesh*)fn_8012C370(name, 'PMsh', &ver, 0, 0, &swapped);
    if (swapped) {
        fn_801CFA48(m);
    }
    return m;
}

void fn_801CFF14(PMesh* m) { fn_8012C4D8(m); }

// Wireframe of the mesh's front faces.
void fn_801CFF18(PMesh* m, const PFrame* xf) {
    PFrame f;
    if (xf) {
        f = *xf;
    } else {
        f.x.y = 0.0f;
        f.x.x = 1.0f;
        f.x.z = 0.0f;
        f.y.x = 0.0f;
        f.y.y = 1.0f;
        f.y.z = 0.0f;
        f.z.x = 0.0f;
        f.z.y = 0.0f;
        f.z.z = 1.0f;
        f.pos.x = 0.0f;
        f.pos.y = 0.0f;
        f.pos.z = 0.0f;
    }
    PFrame cam;
    fn_802C8B08(&cam);
    for (int i = 0; i < m->numTris; i++) {
        PTri* t = &m->tris[i];
        PVec3 v[3];
        for (int k = 0; k < 3; k++) {
            MulVec(&v[k], &m->verts[t->v[k]], &f);
            v[k].x = v[k].x + f.pos.x;
            v[k].y = v[k].y + f.pos.y;
            v[k].z = v[k].z + f.pos.z;
        }
        PVec3 e1;
        PVec3 e2;
        PVec3 nrm;
        PVec3 d;
        e1.x = v[1].x - v[0].x;
        e1.y = v[1].y - v[0].y;
        e1.z = v[1].z - v[0].z;
        e2.x = v[2].x - v[0].x;
        e2.y = v[2].y - v[0].y;
        e2.z = v[2].z - v[0].z;
        PSVECCrossProduct(&e1, &e2, &nrm);
        d.x = v[0].x - cam.pos.x;
        d.y = v[0].y - cam.pos.y;
        d.z = v[0].z - cam.pos.z;
        if (PSVECDotProduct(&nrm, &d) > 0.0f) {
            continue;
        }
        fn_801E41D8(&v[0], &v[1], 0x80FFFF00, 0.2f);
        fn_801E41D8(&v[1], &v[2], 0x80FFFF00, 0.2f);
        fn_801E41D8(&v[2], &v[0], 0x80FFFF00, 0.2f);
    }
}

}  // extern "C"
