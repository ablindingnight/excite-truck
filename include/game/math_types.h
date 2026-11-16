#ifndef GAME_MATH_TYPES_H
#define GAME_MATH_TYPES_H

// The game's plain 3-vector and 3x3 matrix (rows m[0..2]) and the matrix helpers racing.cpp defines (Mat3FromEuler:
// Y, then X, then Z). One declaration for every unit (port ODR work, 2026-11-16). Names unproven.
struct Vec3 {float x,y,z;};
struct Mat3 {float m[3][3];};
typedef char Vec3Size12[sizeof(Vec3)==12?1:-1];
typedef char Mat3Size36[sizeof(Mat3)==36?1:-1];

void Mat3FromEuler(Mat3* m, const Vec3* r);
void Mat3RotY(Mat3* m, float a);
void Mat3RotX(Mat3* m, float a);
void Mat3RotZ(Mat3* m, float a);
void Mat3Mul(Mat3* out, const Mat3* a, const Mat3* b);

#endif
