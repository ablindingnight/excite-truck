#include "extras.h"
#include "file_io.h"
#include "locale.h"

inline int tolower_inline(int c)
{
    return ((c < 0) || (c >= 0x100)) ? c : (int)(_current_locale.ctype_cmpt_ptr->lower_map_ptr[c]);
}

#if defined(GAME_REXE01)
// Excite Truck's older MSL has strlwr ahead of stricmp
char* strlwr(char* str)
{
    char* p = str;

    while (*p)
    {
        *p = tolower_inline(*p);
        p++;
    }

    return str;
}
#endif

int stricmp(const char* s1, const char* s2)
{
    char c1, c2;

    while (1)
    {
        c1 = tolower_inline(*s1++);
        c2 = tolower_inline(*s2++);

        if (c1 < c2)
        {
            return -1;
        }

        if (c1 > c2)
        {
            return 1;
        }

        if (c1 == 0)
        {
            return 0;
        }
    }
}

#if !defined(GAME_REXE01)
int strncasecmp(const char* s1, const char* s2, size_t n)
{
    return __msl_strnicmp(s1, s2, n);
}
#endif

int strcmpi(const char* str1, const char* str2)
{
    return stricmp(str1, str2);
}

int strcasecmp(const char* s1, const char* s2)
{
    return stricmp(s1, s2);
}

#if defined(GAME_REXE01)
// Excite Truck's older MSL defines strncasecmp after strcasecmp
int strncasecmp(const char* s1, const char* s2, size_t n)
{
    return __msl_strnicmp(s1, s2, n);
}
#endif

char* itoa(int value, char* str, int base)
{
    return __msl_itoa(value, str, base);
}
