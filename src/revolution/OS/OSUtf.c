#include <revolution/OS.h>

// DUMP_DATA UcsAnsiTable wchar
static wchar_t UcsAnsiTable[32] = {
#include "dumpdata/UcsAnsiTable.inc"
};

// DUMP_DATA Ucs00 wchar
static wchar_t Ucs00[256] = {
#include "dumpdata/Ucs00.inc"
};

// DUMP_DATA Ucs03 wchar
static wchar_t Ucs03[256] = {
#include "dumpdata/Ucs03.inc"
};

// DUMP_DATA Ucs04 wchar
static wchar_t Ucs04[256] = {
#include "dumpdata/Ucs04.inc"
};

// DUMP_DATA Ucs20 wchar
static wchar_t Ucs20[256] = {
#include "dumpdata/Ucs20.inc"
};

// DUMP_DATA Ucs21 wchar
static wchar_t Ucs21[256] = {
#include "dumpdata/Ucs21.inc"
};

// DUMP_DATA Ucs22 wchar
static wchar_t Ucs22[256] = {
#include "dumpdata/Ucs22.inc"
};

// DUMP_DATA Ucs23 wchar
static wchar_t Ucs23[256] = {
#include "dumpdata/Ucs23.inc"
};

// DUMP_DATA Ucs25 wchar
static wchar_t Ucs25[256] = {
#include "dumpdata/Ucs25.inc"
};

// DUMP_DATA Ucs26 wchar
static wchar_t Ucs26[256] = {
#include "dumpdata/Ucs26.inc"
};

// DUMP_DATA Ucs30 wchar
static wchar_t Ucs30[256] = {
#include "dumpdata/Ucs30.inc"
};

// DUMP_DATA Ucs4E wchar
static wchar_t Ucs4E[256] = {
#include "dumpdata/Ucs4E.inc"
};

// DUMP_DATA Ucs4F wchar
static wchar_t Ucs4F[256] = {
#include "dumpdata/Ucs4F.inc"
};

// DUMP_DATA Ucs50 wchar
static wchar_t Ucs50[256] = {
#include "dumpdata/Ucs50.inc"
};

// DUMP_DATA Ucs51 wchar
static wchar_t Ucs51[256] = {
#include "dumpdata/Ucs51.inc"
};

// DUMP_DATA Ucs52 wchar
static wchar_t Ucs52[256] = {
#include "dumpdata/Ucs52.inc"
};

// DUMP_DATA Ucs53 wchar
static wchar_t Ucs53[256] = {
#include "dumpdata/Ucs53.inc"
};

// DUMP_DATA Ucs54 wchar
static wchar_t Ucs54[256] = {
#include "dumpdata/Ucs54.inc"
};

// DUMP_DATA Ucs55 wchar
static wchar_t Ucs55[256] = {
#include "dumpdata/Ucs55.inc"
};

// DUMP_DATA Ucs56 wchar
static wchar_t Ucs56[256] = {
#include "dumpdata/Ucs56.inc"
};

// DUMP_DATA Ucs57 wchar
static wchar_t Ucs57[256] = {
#include "dumpdata/Ucs57.inc"
};

// DUMP_DATA Ucs58 wchar
static wchar_t Ucs58[256] = {
#include "dumpdata/Ucs58.inc"
};

// DUMP_DATA Ucs59 wchar
static wchar_t Ucs59[256] = {
#include "dumpdata/Ucs59.inc"
};

// DUMP_DATA Ucs5A wchar
static wchar_t Ucs5A[256] = {
#include "dumpdata/Ucs5A.inc"
};

// DUMP_DATA Ucs5B wchar
static wchar_t Ucs5B[256] = {
#include "dumpdata/Ucs5B.inc"
};

// DUMP_DATA Ucs5C wchar
static wchar_t Ucs5C[256] = {
#include "dumpdata/Ucs5C.inc"
};

// DUMP_DATA Ucs5D wchar
static wchar_t Ucs5D[256] = {
#include "dumpdata/Ucs5D.inc"
};

// DUMP_DATA Ucs5E wchar
static wchar_t Ucs5E[256] = {
#include "dumpdata/Ucs5E.inc"
};

// DUMP_DATA Ucs5F wchar
static wchar_t Ucs5F[256] = {
#include "dumpdata/Ucs5F.inc"
};

// DUMP_DATA Ucs60 wchar
static wchar_t Ucs60[256] = {
#include "dumpdata/Ucs60.inc"
};

// DUMP_DATA Ucs61 wchar
static wchar_t Ucs61[256] = {
#include "dumpdata/Ucs61.inc"
};

// DUMP_DATA Ucs62 wchar
static wchar_t Ucs62[256] = {
#include "dumpdata/Ucs62.inc"
};

// DUMP_DATA Ucs63 wchar
static wchar_t Ucs63[256] = {
#include "dumpdata/Ucs63.inc"
};

// DUMP_DATA Ucs64 wchar
static wchar_t Ucs64[256] = {
#include "dumpdata/Ucs64.inc"
};

// DUMP_DATA Ucs65 wchar
static wchar_t Ucs65[256] = {
#include "dumpdata/Ucs65.inc"
};

// DUMP_DATA Ucs66 wchar
static wchar_t Ucs66[256] = {
#include "dumpdata/Ucs66.inc"
};

// DUMP_DATA Ucs67 wchar
static wchar_t Ucs67[256] = {
#include "dumpdata/Ucs67.inc"
};

// DUMP_DATA Ucs68 wchar
static wchar_t Ucs68[256] = {
#include "dumpdata/Ucs68.inc"
};

// DUMP_DATA Ucs69 wchar
static wchar_t Ucs69[256] = {
#include "dumpdata/Ucs69.inc"
};

// DUMP_DATA Ucs6A wchar
static wchar_t Ucs6A[256] = {
#include "dumpdata/Ucs6A.inc"
};

// DUMP_DATA Ucs6B wchar
static wchar_t Ucs6B[256] = {
#include "dumpdata/Ucs6B.inc"
};

// DUMP_DATA Ucs6C wchar
static wchar_t Ucs6C[256] = {
#include "dumpdata/Ucs6C.inc"
};

// DUMP_DATA Ucs6D wchar
static wchar_t Ucs6D[256] = {
#include "dumpdata/Ucs6D.inc"
};

// DUMP_DATA Ucs6E wchar
static wchar_t Ucs6E[256] = {
#include "dumpdata/Ucs6E.inc"
};

// DUMP_DATA Ucs6F wchar
static wchar_t Ucs6F[256] = {
#include "dumpdata/Ucs6F.inc"
};

// DUMP_DATA Ucs70 wchar
static wchar_t Ucs70[256] = {
#include "dumpdata/Ucs70.inc"
};

// DUMP_DATA Ucs71 wchar
static wchar_t Ucs71[256] = {
#include "dumpdata/Ucs71.inc"
};

// DUMP_DATA Ucs72 wchar
static wchar_t Ucs72[256] = {
#include "dumpdata/Ucs72.inc"
};

// DUMP_DATA Ucs73 wchar
static wchar_t Ucs73[256] = {
#include "dumpdata/Ucs73.inc"
};

// DUMP_DATA Ucs74 wchar
static wchar_t Ucs74[256] = {
#include "dumpdata/Ucs74.inc"
};

// DUMP_DATA Ucs75 wchar
static wchar_t Ucs75[256] = {
#include "dumpdata/Ucs75.inc"
};

// DUMP_DATA Ucs76 wchar
static wchar_t Ucs76[256] = {
#include "dumpdata/Ucs76.inc"
};

// DUMP_DATA Ucs77 wchar
static wchar_t Ucs77[256] = {
#include "dumpdata/Ucs77.inc"
};

// DUMP_DATA Ucs78 wchar
static wchar_t Ucs78[256] = {
#include "dumpdata/Ucs78.inc"
};

// DUMP_DATA Ucs79 wchar
static wchar_t Ucs79[256] = {
#include "dumpdata/Ucs79.inc"
};

// DUMP_DATA Ucs7A wchar
static wchar_t Ucs7A[256] = {
#include "dumpdata/Ucs7A.inc"
};

// DUMP_DATA Ucs7B wchar
static wchar_t Ucs7B[256] = {
#include "dumpdata/Ucs7B.inc"
};

// DUMP_DATA Ucs7C wchar
static wchar_t Ucs7C[256] = {
#include "dumpdata/Ucs7C.inc"
};

// DUMP_DATA Ucs7D wchar
static wchar_t Ucs7D[256] = {
#include "dumpdata/Ucs7D.inc"
};

// DUMP_DATA Ucs7E wchar
static wchar_t Ucs7E[256] = {
#include "dumpdata/Ucs7E.inc"
};

// DUMP_DATA Ucs7F wchar
static wchar_t Ucs7F[256] = {
#include "dumpdata/Ucs7F.inc"
};

// DUMP_DATA Ucs80 wchar
static wchar_t Ucs80[256] = {
#include "dumpdata/Ucs80.inc"
};

// DUMP_DATA Ucs81 wchar
static wchar_t Ucs81[256] = {
#include "dumpdata/Ucs81.inc"
};

// DUMP_DATA Ucs82 wchar
static wchar_t Ucs82[256] = {
#include "dumpdata/Ucs82.inc"
};

// DUMP_DATA Ucs83 wchar
static wchar_t Ucs83[256] = {
#include "dumpdata/Ucs83.inc"
};

// DUMP_DATA Ucs84 wchar
static wchar_t Ucs84[256] = {
#include "dumpdata/Ucs84.inc"
};

// DUMP_DATA Ucs85 wchar
static wchar_t Ucs85[256] = {
#include "dumpdata/Ucs85.inc"
};

// DUMP_DATA Ucs86 wchar
static wchar_t Ucs86[256] = {
#include "dumpdata/Ucs86.inc"
};

// DUMP_DATA Ucs87 wchar
static wchar_t Ucs87[256] = {
#include "dumpdata/Ucs87.inc"
};

// DUMP_DATA Ucs88 wchar
static wchar_t Ucs88[256] = {
#include "dumpdata/Ucs88.inc"
};

// DUMP_DATA Ucs89 wchar
static wchar_t Ucs89[256] = {
#include "dumpdata/Ucs89.inc"
};

// DUMP_DATA Ucs8A wchar
static wchar_t Ucs8A[256] = {
#include "dumpdata/Ucs8A.inc"
};

// DUMP_DATA Ucs8B wchar
static wchar_t Ucs8B[256] = {
#include "dumpdata/Ucs8B.inc"
};

// DUMP_DATA Ucs8C wchar
static wchar_t Ucs8C[256] = {
#include "dumpdata/Ucs8C.inc"
};

// DUMP_DATA Ucs8D wchar
static wchar_t Ucs8D[256] = {
#include "dumpdata/Ucs8D.inc"
};

// DUMP_DATA Ucs8E wchar
static wchar_t Ucs8E[256] = {
#include "dumpdata/Ucs8E.inc"
};

// DUMP_DATA Ucs8F wchar
static wchar_t Ucs8F[256] = {
#include "dumpdata/Ucs8F.inc"
};

// DUMP_DATA Ucs90 wchar
static wchar_t Ucs90[256] = {
#include "dumpdata/Ucs90.inc"
};

// DUMP_DATA Ucs91 wchar
static wchar_t Ucs91[256] = {
#include "dumpdata/Ucs91.inc"
};

// DUMP_DATA Ucs92 wchar
static wchar_t Ucs92[256] = {
#include "dumpdata/Ucs92.inc"
};

// DUMP_DATA Ucs93 wchar
static wchar_t Ucs93[256] = {
#include "dumpdata/Ucs93.inc"
};

// DUMP_DATA Ucs94 wchar
static wchar_t Ucs94[256] = {
#include "dumpdata/Ucs94.inc"
};

// DUMP_DATA Ucs95 wchar
static wchar_t Ucs95[256] = {
#include "dumpdata/Ucs95.inc"
};

// DUMP_DATA Ucs96 wchar
static wchar_t Ucs96[256] = {
#include "dumpdata/Ucs96.inc"
};

// DUMP_DATA Ucs97 wchar
static wchar_t Ucs97[256] = {
#include "dumpdata/Ucs97.inc"
};

// DUMP_DATA Ucs98 wchar
static wchar_t Ucs98[256] = {
#include "dumpdata/Ucs98.inc"
};

// DUMP_DATA Ucs99 wchar
static wchar_t Ucs99[256] = {
#include "dumpdata/Ucs99.inc"
};

// DUMP_DATA Ucs9A wchar
static wchar_t Ucs9A[256] = {
#include "dumpdata/Ucs9A.inc"
};

// DUMP_DATA Ucs9B wchar
static wchar_t Ucs9B[256] = {
#include "dumpdata/Ucs9B.inc"
};

// DUMP_DATA Ucs9C wchar
static wchar_t Ucs9C[256] = {
#include "dumpdata/Ucs9C.inc"
};

// DUMP_DATA Ucs9D wchar
static wchar_t Ucs9D[256] = {
#include "dumpdata/Ucs9D.inc"
};

// DUMP_DATA Ucs9E wchar
static wchar_t Ucs9E[256] = {
#include "dumpdata/Ucs9E.inc"
};

// DUMP_DATA Ucs9F wchar
static wchar_t Ucs9F[256] = {
#include "dumpdata/Ucs9F.inc"
};

// DUMP_DATA UcsFF wchar
static wchar_t UcsFF[256] = {
#include "dumpdata/UcsFF.inc"
};

static wchar_t* UcsSjisTable[256] = {
    Ucs00, NULL,  NULL,  Ucs03, Ucs04, NULL,  NULL,  NULL,  NULL,  NULL,  NULL,
    NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,
    NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  Ucs20,
    Ucs21, Ucs22, Ucs23, NULL,  Ucs25, Ucs26, NULL,  NULL,  NULL,  NULL,  NULL,
    NULL,  NULL,  NULL,  NULL,  Ucs30, NULL,  NULL,  NULL,  NULL,  NULL,  NULL,
    NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,
    NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,
    NULL,  Ucs4E, Ucs4F, Ucs50, Ucs51, Ucs52, Ucs53, Ucs54, Ucs55, Ucs56, Ucs57,
    Ucs58, Ucs59, Ucs5A, Ucs5B, Ucs5C, Ucs5D, Ucs5E, Ucs5F, Ucs60, Ucs61, Ucs62,
    Ucs63, Ucs64, Ucs65, Ucs66, Ucs67, Ucs68, Ucs69, Ucs6A, Ucs6B, Ucs6C, Ucs6D,
    Ucs6E, Ucs6F, Ucs70, Ucs71, Ucs72, Ucs73, Ucs74, Ucs75, Ucs76, Ucs77, Ucs78,
    Ucs79, Ucs7A, Ucs7B, Ucs7C, Ucs7D, Ucs7E, Ucs7F, Ucs80, Ucs81, Ucs82, Ucs83,
    Ucs84, Ucs85, Ucs86, Ucs87, Ucs88, Ucs89, Ucs8A, Ucs8B, Ucs8C, Ucs8D, Ucs8E,
    Ucs8F, Ucs90, Ucs91, Ucs92, Ucs93, Ucs94, Ucs95, Ucs96, Ucs97, Ucs98, Ucs99,
    Ucs9A, Ucs9B, Ucs9C, Ucs9D, Ucs9E, Ucs9F, NULL,  NULL,  NULL,  NULL,  NULL,
    NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,
    NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,
    NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,
    NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,
    NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,
    NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,
    NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,
    NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,
    NULL,  NULL,  UcsFF};

const u8* OSUTF8to32(const u8* utf8, u32* utf32) {
    u32 code;
    u32 full;
    u32 size;
    int i;

    code = *utf8;
    full = 0;
    if (code != 0) {
        utf8++;
    }

    if (!(code & 0x80)) {
        full = code;
        size = 0;
    } else if ((code & 0xE0) == 0xC0) {
        full = code & 0x1F;
        size = 1;
    } else if ((code & 0xF0) == 0xE0) {
        full = code & 0xF;
        size = 2;
    } else if ((code & 0xF8) == 0xF0) {
        full = code & 0x7;
        size = 3;
    } else {
        return NULL;
    }

    for (i = 0; i < size; i++) {
        code = *utf8++;
        full <<= 6;

        if ((code & 0xC0) != 0x80) {
            return NULL;
        }

        full |= code & 0x3F;
    }

    if (full <= 0x7F) {
        if (size != 0) {
            return NULL;
        }
    } else if (full <= 0x7FF) {
        if (size != 1) {
            return NULL;
        }
    } else if (full <= 0xFFFF) {
        if (size != 2) {
            return NULL;
        }
    }

    if (full >= 0xD800 && full <= 0xDFFF) {
        return NULL;
    }

    *utf32 = full;
    return utf8;
}

const wchar_t* OSUTF16to32(const wchar_t* utf16, u32* utf32) {
    wchar_t hi, lo;
    u32 full;

    hi = *utf16;
    if (hi != 0) {
        utf16++;
    }

    if (hi < 0xD800 || hi > 0xDFFF) {
        full = hi;
    } else if (hi <= 0xDBFF) {
        lo = *utf16++;

        if (lo >= 0xDC00 && lo <= 0xDFFF) {
            full = (hi & 0x3FF) << 10 | lo & 0x3FF;
            full += 0x10000;
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }

    *utf32 = full;
    return utf16;
}

u8 OSUTF32toANSI(u32 utf32) {
    int i;

    if (utf32 > 0xFF) {
        return 0;
    }

    if (utf32 < 0x80 || utf32 > 0x9F) {
        return utf32 & 0xFF;
    }

    if (utf32 >= 0x152 && utf32 <= 0x2122) {
        for (i = 0; i < ARRAY_SIZE(UcsAnsiTable); i++) {
            if (UcsAnsiTable[i] == utf32) {
                return i + 0x80 & 0xFF;
            }
        }
    }

    return 0;
}

wchar_t OSUTF32toSJIS(u32 utf32) {
    if (utf32 >= 0x10000) {
        return 0;
    }

    if (UcsSjisTable[utf32 >> 8 & 0xFF] != NULL) {
        return UcsSjisTable[utf32 >> 8 & 0xFF][utf32 & 0xFF];
    }

    return 0;
}
