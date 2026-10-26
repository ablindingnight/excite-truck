// MSL's strlen, linked on its own between w_sqrt.c and __va_arg.c (the map marks it weak; its original file is
// unknown).
#include <stddef.h>

size_t strlen(const char* str) {
    size_t len = (size_t)-1;
    const unsigned char* p = (const unsigned char*)str - 1;

    do {
        len++;
    } while (*++p);

    return len;
}
