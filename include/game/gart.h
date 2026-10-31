#ifndef GAME_GART_H
#define GAME_GART_H
// Opaque reference-counted 2D art set; unrelated to GX image handles or either font record.
struct gArtSet;
extern "C" {
gArtSet* fn_8023512C(const char*);
void fn_80235380(gArtSet*);
float fn_80235EF0(gArtSet*,int);
float fn_80235F64(gArtSet*,int);
int fn_80235FD8(gArtSet*);
}
#endif
