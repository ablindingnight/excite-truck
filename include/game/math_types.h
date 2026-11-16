#ifndef GAME_MATH_TYPES_H
#define GAME_MATH_TYPES_H

// The game's plain 3-vector and 3x3 matrix (rows m[0..2]) and the matrix helpers racing.cpp defines (Mat3FromEuler:
// Y, then X, then Z). One declaration for every unit (port ODR work, 2026-11-16). Names unproven.
struct Vec3 {float x,y,z;};
extern "C" {
double cos(double x);
double sin(double x);
}

// Rows m[0..2]. The Set* helpers are betarace's (names unproven).
struct Mat3 {
    void SetRotX(float a) {
        float c = cos(a);
        float s = sin(a);
        m[0][0] = 1.0f;
        m[0][1] = 0.0f;
        m[0][2] = 0.0f;
        m[1][0] = 0.0f;
        m[1][1] = c;
        m[1][2] = s;
        m[2][0] = 0.0f;
        m[2][1] = -s;
        m[2][2] = c;
    }
    void SetScale(float s) {
        m[0][0] = s;
        m[0][1] = 0.0f;
        m[0][2] = 0.0f;
        m[1][0] = 0.0f;
        m[1][1] = s;
        m[1][2] = 0.0f;
        m[2][0] = 0.0f;
        m[2][1] = 0.0f;
        m[2][2] = s;
    }
    void SetRotY(float a) {
        float c = cos(a);
        float s = sin(a);
        m[0][0] = c;
        m[0][1] = 0.0f;
        m[0][2] = -s;
        m[1][0] = 0.0f;
        m[1][1] = 1.0f;
        m[1][2] = 0.0f;
        m[2][0] = s;
        m[2][1] = 0.0f;
        m[2][2] = c;
    }
    float m[3][3];
};
typedef char Vec3Size12[sizeof(Vec3)==12?1:-1];
typedef char Mat3Size36[sizeof(Mat3)==36?1:-1];

void Mat3FromEuler(Mat3* m, const Vec3* r);
void Mat3RotY(Mat3* m, float a);
void Mat3RotX(Mat3* m, float a);
void Mat3RotZ(Mat3* m, float a);
void Mat3Mul(Mat3* out, const Mat3* a, const Mat3* b);

#endif
