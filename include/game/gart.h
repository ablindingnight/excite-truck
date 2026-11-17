#ifndef GAME_GART_H
#define GAME_GART_H

// Opaque reference-counted 2D art set; also backs the null-safe image API. Not a font record.
// Built-in types only (some units define their own u32). The gArt API (gart.cpp), one declaration for every caller. The handle is a gArtSet* everywhere: units that kept it
// in an int/u32 field truncate it on a 64-bit PC (port hazard C2, 2026-11-16). Every function is null-safe on the set.
struct gArtSet;
#ifdef __cplusplus
extern "C" {
#endif
void fn_8023509C(void);                            // begin a 2D art batch
void fn_802350BC(void);                            // end it
gArtSet* fn_8023512C(const char*);                 // load (or share) a set by file name
void fn_80235380(gArtSet*);                        // release
void fn_802353F4(gArtSet*, int, float, float, float);  // draw image idx at x, y, z (its own size)
void fn_802354A0(gArtSet* s, int idx, float x, float y, float z, float w, float h, int color);  // draw scaled, tinted
void fn_80235AFC(gArtSet* s, int idx, unsigned long color, float x, float y, float z, float w, float h, float angle);  // rotated
void fn_80235758(gArtSet* s, int idx, float x, float y, float w, float h, float left, float top, float right,
                 float bottom, int color);  // draw a sub-rectangle (u/v bounds)
void fn_80235CBC(gArtSet* s, int idx, float x, float y, float z, float w, float h, int color);
float fn_80235EF0(gArtSet*, int);                  // image width
float fn_80235F64(gArtSet*, int);                  // image height
int fn_80235FD8(gArtSet*);                         // image count
#ifdef __cplusplus
}
#endif
#endif
