#ifndef GAME_IMAGE_H
#define GAME_IMAGE_H
// Null-safe image wrappers. Handles originate in gxGetImage and refer to gArtSet objects.
// The public API keeps its existing opaque pointer type; ownership is unchanged.
extern "C" {
void* gxGetImage(const char* name);
void fn_802C519C(void* image);
int fn_802C51AC(void* image);
int fn_802C51C0(void* image);
int fn_802C5220(void* image);
int IsPixelInBounds(void* image, int x, int y, int unused);
void DrawImageFrame(void* image, int frame, float x, float y, float z);
void DrawImageWithParams(void* image, int frame, float x, float y, float z, float width, float height);
void fn_802C5334(void* image, int frame, float x, float y, float unusedZ,
                 float width, float height, float left, float top, float right, float bottom);
}
#endif
