#include <internal/fdlibm.h>

// wrapper log(x); Excite Truck links this (with _IEEE_LIBM) after w_fmod
double log(double x) {
    return __ieee754_log(x);
}
