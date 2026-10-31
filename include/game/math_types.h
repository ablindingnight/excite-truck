#ifndef GAME_MATH_TYPES_H
#define GAME_MATH_TYPES_H
// Shapes used by the matched Mat3FromEuler/Mat3Mul producer in racing.cpp.
struct Vec3 {float x,y,z;};
struct Mat3 {float m[3][3];};
typedef char Vec3Size12[sizeof(Vec3)==12?1:-1];
typedef char Mat3Size36[sizeof(Mat3)==36?1:-1];
#endif
