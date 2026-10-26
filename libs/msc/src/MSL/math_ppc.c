#include <math.h>

// Excite Truck's math_ppc.c also has log2 (log(x) / ln 2), used by the game's texture LOD code.
double log2(double x) {
    return log(x) * 1.4426950408889634;
}

double nan(const char* arg) {
#pragma unused(arg)
}
