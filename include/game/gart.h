#ifndef GAME_GART_H
#define GAME_GART_H
// Opaque reference-counted 2D art set; also backs the null-safe image API. Not a font record.
struct gArtSet;
extern "C" {
gArtSet* fn_8023512C(const char*);
void fn_80235380(gArtSet*);
void fn_802353F4(gArtSet*,int,float,float,float);
float fn_80235EF0(gArtSet*,int);
float fn_80235F64(gArtSet*,int);
int fn_80235FD8(gArtSet*);
}
#endif
