#include <revolution/GX.h>
#include <revolution/OS.h>
#include <revolution/VI.h>

#define ROM_FONT_SJIS_START ((void*)0x001AFF00)
#define ROM_FONT_SJIS_SIZE 0x0004D000
#define ROM_FONT_ANSI_START ((void*)0x001FCF00)
#define ROM_FONT_ANSI_SIZE 0x00003000

typedef const u8* (*ParseStringFunc)(u16 encode, const u8* str,
                                     OSFontHeader** fontOut, u32* codeOut);

static u16 FontEncode = 0xFFFF;

static OSFontHeader* FontDataAnsi;
static OSFontHeader* FontDataSjis;
static BOOL FixedPitch;
static ParseStringFunc ParseString;

static const u8* ParseStringS(u16 encode, const u8* str, OSFontHeader** fontOut,
                              u32* codeOut);
static const u8* ParseStringW(u16 encode, const u8* str, OSFontHeader** fontOut,
                              u32* codeOut);

static BOOL IsSjisLeadByte(u8 ch) {
    return (0x81 <= ch && ch <= 0x9F) || (0xE0 <= ch && ch <= 0xFC);
}

static BOOL IsSjisTrailByte(u8 ch) {
    return (0x40 <= ch && ch <= 0xFC) && (ch != 0x7F);
}

// DUMP_DATA HankakuToCode u16
static u16 HankakuToCode[] = {
#include "dumpdata/HankakuToCode.inc"
};

// DUMP_DATA Zenkaku2Code u16
static u16 Zenkaku2Code[] = {
#include "dumpdata/Zenkaku2Code.inc"
};

static u32 GetFontCode(u16 encode, u16 code) {
    u32 tmp;
    s32 trail;

    if (encode == OS_FONT_ENCODE_SJIS) {
        if (code >= 0x20 && code <= 0xDF) {
            return HankakuToCode[code - 0x20];
        } else if (code > 0x889E && code <= 0x9872) {
            tmp = ((code >> 8) - 0x88) * 0xBC;
            trail = code & 0xFF;

            if (!IsSjisTrailByte(trail)) {
                return 0;
            }

            trail -= 0x40;
            if (trail >= 0x40) {
                trail--;
            }

            return tmp + trail + 0x2BE;
        } else if (code >= 0x8140 && code < 0x879E) {
            tmp = ((code >> 8) - 0x81) * 0xBC;
            trail = code & 0xFF;

            if (!IsSjisTrailByte(trail)) {
                return 0;
            }

            trail -= 0x40;
            if (trail >= 0x40) {
                trail--;
            }

            return Zenkaku2Code[tmp + trail];
        }
    } else if (code > 0x20 && code <= 0xFF) {
        return code - 0x20;
    }

    return 0;
}

// 'Yay0' decompression (See YAGCD sections 16.1.1, 16.1.2)
static void Decode(u8* src, u8* dst) {
    int j;
    s32 linkOfs;
    s32 chunkPos;
    int i;
    s32 chunksOfs;
    u32 maskTblPos;
    s32 expandSize;
    s32 linkTblOfs;
    s32 count;
    u32 maskBits;
    u32 mask;

    expandSize = *(s32*)(src + 0x4);
    linkTblOfs = *(s32*)(src + 0x8);
    chunksOfs = *(s32*)(src + 0xC);

    i = 0;
    maskBits = 0;
    maskTblPos = 16;

    do {
        // Get next mask
        if (maskBits == 0) {
            mask = *(u32*)(src + maskTblPos);
            maskTblPos += sizeof(u32);
            maskBits = sizeof(u32) * 8;
        }

        // Non-linked chunk
        if (mask & 0x80000000) {
            dst[i++] = src[chunksOfs++];
        }
        // Linked chunk
        else {
            // Read offset from link table
            linkOfs = src[linkTblOfs] << 8 | src[linkTblOfs + 1];
            linkTblOfs += sizeof(u16);

            // Apply offset
            chunkPos = i - (linkOfs & 0x0FFF);
            count = linkOfs >> 12;
            if (count == 0) {
                count = src[chunksOfs++] + 0x12;
            } else {
                count += 2;
            }

            // Copy chunk
            for (j = 0; j < count; j++, i++, chunkPos++) {
                dst[i] = dst[chunkPos - 1];
            }
        }

        // Prepare next mask bit
        mask <<= 1;
        maskBits--;
    } while (i < expandSize);
}

static u32 GetFontSize(const u8* font) {
    if (font[0] == 'Y' && font[1] == 'a' && font[2] == 'y') {
        return *(u32*)(font + 0x4);
    }

    return 0;
}

u16 OSGetFontEncode(void) {
    if (FontEncode != 0xFFFF) {
        return FontEncode;
    }

    switch (*(u32*)OSPhysicalToCached(OS_PHYS_TV_FORMAT)) {
    case VI_TVFORMAT_NTSC:
        FontEncode = ((VI_HW_REGS[VI_VISEL] & 2) != 0) ? OS_FONT_ENCODE_SJIS
                                                       : OS_FONT_ENCODE_ANSI;
        break;
    case VI_TVFORMAT_PAL:
    case VI_TVFORMAT_MPAL:
    case VI_TVFORMAT_DEBUG:
    case VI_TVFORMAT_DEBUG_PAL:
    case VI_TVFORMAT_EURGB60:
    default:
        FontEncode = OS_FONT_ENCODE_ANSI;
    }

    ParseString = ParseStringS;

    return FontEncode;
}

u16 OSSetFontEncode(u16 encode) {
    u16 old = OSGetFontEncode();

    if (encode <= OS_FONT_ENCODE_UTF32) {
        FontEncode = encode;

        if (encode >= OS_FONT_ENCODE_UTF8 && encode <= OS_FONT_ENCODE_UTF32) {
            ParseString = ParseStringW;
        }
    }

    return old;
}

static void ReadROM(void* dst, s32 size, const void* src) {
    s32 blockSize;

    while (size > 0) {
        blockSize = (size <= 256) ? size : 256;
        size -= blockSize;

        while (!__OSReadROM(dst, blockSize, src)) {
            ;
        }

        src = (u8*)src + blockSize;
        dst = (u8*)dst + blockSize;
    }
}

static u32 ReadFont(void* dst, u16 encode, OSFontHeader* font) {
    u8* tex;
    int i;
    u32 code;
    u32 size;
    s32 sheet;
    s32 numRestTex;
    s32 row;
    s32 col;
    u8* tmp;

    if (encode == OS_FONT_ENCODE_SJIS) {
        ReadROM(dst, ROM_FONT_SJIS_SIZE, ROM_FONT_SJIS_START);
    } else {
        ReadROM(dst, ROM_FONT_ANSI_SIZE, ROM_FONT_ANSI_START);
    }

    size = GetFontSize(dst);
    if (size == 0) {
        return 0;
    }

    Decode(dst, (u8*)font);

    if (encode == OS_FONT_ENCODE_SJIS) {
        u16 sp28[] = {0x2ABE, 0x003D, 0x003D, 0x003D};

        /**
         * Find 'T' texture (See OSGetFontTexture)
         */
        code = GetFontCode(encode, 'T');
        // Font sheet on which the texture resides
        sheet = (s32)code / (font->sheetNumCol * font->sheetNumRow);
        // Number of succeeding textures on the sheet
        numRestTex = code - (sheet * (font->sheetNumCol * font->sheetNumRow));
        // Texture position on sheet
        row = numRestTex / font->sheetNumCol;
        col = numRestTex - row * font->sheetNumCol;
        // Texture position
        row *= font->cellHeight;
        col *= font->cellWidth;
        // Font code texture
        tex = (u8*)font + font->sheetImageOfs;
        tex += sheet * font->sheetSize / 2;

        // Editing the texture at runtime?
        for (i = 4; i < 8; i++) {
            tmp = tex + (((font->sheetWidth / 8) * 32) / 2) * ((row + i) / 8);
            tmp += (col / 8) * 16;
            tmp += ((row + i) % 8) * 2;
            tmp += (col % 8) / 4;
            *(u16*)tmp = sp28[i - 4];
        }
    }

    return size;
}

u32 OSLoadFont(OSFontHeader* font, void* dst) {
    u32 size;

    switch (OSGetFontEncode()) {
    case OS_FONT_ENCODE_ANSI:
        FontDataAnsi = font;
        size = ReadFont(dst, OS_FONT_ENCODE_ANSI, FontDataAnsi);
        break;
    case OS_FONT_ENCODE_SJIS:
        FontDataSjis = font;
        size = ReadFont(dst, OS_FONT_ENCODE_SJIS, FontDataSjis);
        break;
    case OS_FONT_ENCODE_UTF8:
    case OS_FONT_ENCODE_UTF16:
    case OS_FONT_ENCODE_UTF32:
        FontDataAnsi = font;
        size = ReadFont(dst, OS_FONT_ENCODE_ANSI, FontDataAnsi);
        if (size == 0) {
            break;
        }

        FontDataSjis = (OSFontHeader*)((u8*)FontDataAnsi + size);
        size += ReadFont(dst, OS_FONT_ENCODE_SJIS, FontDataSjis);
        break;
    case OS_FONT_ENCODE_2:
    default:
        size = 0;
        break;
    }

    return size;
}

static const u8* ParseStringS(u16 encode, const u8* str, OSFontHeader** fontOut,
                              u32* codeOut) {
    OSFontHeader* font;
    u16 code = 0;

    switch (encode) {
    case OS_FONT_ENCODE_ANSI:
        font = FontDataAnsi;
        code = *str;
        if (code != 0) {
            str++;
        }
        break;
    case OS_FONT_ENCODE_SJIS:
        font = FontDataSjis;
        code = *str;
        if (code == 0) {
            break;
        }
        str++;

        if (IsSjisLeadByte(code) && IsSjisTrailByte(*str)) {
            code = (code << 8 | *str++);
        }
        break;
    }

    *fontOut = font;
    *codeOut = GetFontCode(encode, code);

    return str;
}

static const u8* ParseStringW(u16 encode, const u8* str, OSFontHeader** fontOut,
                              u32* codeOut) {
    OSFontHeader* font;
    u16 code = 0;
    u32 utf32 = 0;

    switch (encode) {
    case OS_FONT_ENCODE_ANSI:
        font = FontDataAnsi;
        code = *str;
        if (code != 0) {
            str++;
        }
        break;
    case OS_FONT_ENCODE_SJIS:
        font = FontDataSjis;
        code = *str;
        if (code == 0) {
            break;
        }
        str++;

        if (IsSjisLeadByte(code) && IsSjisTrailByte(*str)) {
            code = (code << 8 | *str++);
        }
        break;
    case OS_FONT_ENCODE_UTF8:
        str = OSUTF8to32(str, &utf32);
        break;
    case OS_FONT_ENCODE_UTF16:
        str = (const u8*)OSUTF16to32((const u16*)str, &utf32);
        break;
    case OS_FONT_ENCODE_UTF32:
        utf32 = *(u32*)str;
        if (utf32 != 0) {
            str += sizeof(u32);
        }
        break;
    }

    if (utf32 != 0) {
        encode = OS_FONT_ENCODE_ANSI;
        font = FontDataAnsi;
        code = OSUTF32toANSI(utf32);

        if (code == 0 || (FixedPitch && utf32 <= 0x7F)) {
            code = OSUTF32toSJIS(utf32);
            if (code != 0) {
                encode = OS_FONT_ENCODE_SJIS;
                font = FontDataSjis;
            }
        }
    }

    *fontOut = font;
    *codeOut = GetFontCode(encode, code);

    return str;
}

const char* OSGetFontTexel(const char* str, void* dst, s32 xOfs, s32 arg3,
                           u32* widthOut) {
    OSFontHeader* font;
    s32 numRestTex;
    u8* local_24;
    s32 row;
    u8* local_20;
    s32 col;
    s32 local_48;
    u32 code;
    int j;
    int i;
    u32 sheet;
    u8* local_4C;
    u8* font_u8;
    u8* tex;
    s32 local_44;

    str = (const char*)ParseString(OSGetFontEncode(), (const u8*)str, &font,
                                   &code);
    local_4C = &font->c0;

    /**
     * Find font code texture (See OSGetFontTexture)
     */
    // Font sheet on which the texture resides
    sheet = (s32)code / (font->sheetNumCol * font->sheetNumRow);
    // Number of succeeding textures on the sheet
    numRestTex = code - (sheet * (font->sheetNumCol * font->sheetNumRow));
    // Texture position on sheet
    row = numRestTex / font->sheetNumCol;
    col = numRestTex - row * font->sheetNumCol;
    // Texture position
    row *= font->cellHeight;
    col *= font->cellWidth;
    // Font code texture
    tex = (u8*)font + font->sheetImageOfs;
    tex += sheet * font->sheetSize / 2;

    for (i = 0; i < font->cellHeight; i++) {
        for (j = 0; j < font->cellWidth; j++) {
            local_20 =
                tex + (((font->sheetWidth / 8) * 32) / 2) * ((row + i) / 8);
            local_20 += ((col + j) / 8) * 16;
            local_20 += ((row + i) % 8) * 2;
            local_20 += ((col + j) % 8) / 4;

            local_44 = (col + j) % 4;

            local_24 = (u8*)dst + ((i / 8) * (((arg3 * 4) / 8) * 32));
            local_24 += (((xOfs + j) / 8) * 32);
            local_24 += ((i % 8) * 4);
            local_24 += ((xOfs + j) % 8) / 2;

            local_48 = (xOfs + j) % 2;

            *local_24 |=
                (u8)(local_4C[(*local_20 >> (6 - (local_44 * 2))) & 3] &
                     (local_48 != 0 ? 0x0F : 0xF0));
        }
    }

    if (widthOut != NULL) {
        // TODO(kiwi) Permuter fake(?)match
        font_u8 = (u8*)font;
        *widthOut = (font_u8 + font->widthTableOfs)[code];
    }

    return str;
}

static void ExpandFontSheet(const OSFontHeader* font, u8* src, u8* dst) {
    int i;
    const u8* tmp = &font->c0;

    if (font->sheetFormat == GX_TF_I4) {
        for (i = (s32)font->sheetFullSize / 2 - 1; i >= 0; i--) {
            dst[i * 2 + 0] =
                tmp[src[i] >> 6 & 3] & 0xF0 | tmp[src[i] >> 4 & 3] & 0x0F;
            dst[i * 2 + 1] =
                tmp[src[i] >> 2 & 3] & 0xF0 | tmp[src[i] >> 0 & 3] & 0x0F;
        }
    } else if (font->sheetFormat == GX_TF_IA4) {
        for (i = (s32)font->sheetFullSize / 4 - 1; i >= 0; i--) {
            dst[i * 4 + 0] = tmp[src[i] >> 6 & 3];
            dst[i * 4 + 1] = tmp[src[i] >> 4 & 3];
            dst[i * 4 + 2] = tmp[src[i] >> 2 & 3];
            dst[i * 4 + 3] = tmp[src[i] >> 0 & 3];
        }
    }

    DCStoreRange(dst, font->sheetFullSize);
}

BOOL OSInitFont(OSFontHeader* font) {
    u8* sheets;

    switch (OSGetFontEncode()) {
    case OS_FONT_ENCODE_ANSI:
        FontDataAnsi = font;
        if (ReadFont((u8*)font + 0x1D120, OS_FONT_ENCODE_ANSI, FontDataAnsi) ==
            0) {
            return FALSE;
        }

        sheets = (u8*)FontDataAnsi + FontDataAnsi->sheetImageOfs;
        FontDataAnsi->sheetImageOfs = ROUND_UP(FontDataAnsi->sheetImageOfs, 32);
        ExpandFontSheet(FontDataAnsi, sheets,
                        (u8*)FontDataAnsi + FontDataAnsi->sheetImageOfs);
        break;
    case OS_FONT_ENCODE_SJIS:
        FontDataSjis = font;
        if (ReadFont((u8*)font + 0xD3F00, OS_FONT_ENCODE_SJIS, FontDataSjis) ==
            0) {
            return FALSE;
        }

        sheets = (u8*)FontDataSjis + FontDataSjis->sheetImageOfs;
        FontDataSjis->sheetImageOfs = ROUND_UP(FontDataSjis->sheetImageOfs, 32);
        ExpandFontSheet(FontDataSjis, sheets,
                        (u8*)FontDataSjis + FontDataSjis->sheetImageOfs);
        break;
    case OS_FONT_ENCODE_2:
        break;
    case OS_FONT_ENCODE_UTF8:
    case OS_FONT_ENCODE_UTF16:
    case OS_FONT_ENCODE_UTF32:
        FontDataAnsi = font;
        if (ReadFont((u8*)font + 0xF4020, OS_FONT_ENCODE_ANSI, FontDataAnsi) ==
            0) {
            return FALSE;
        }

        sheets = (u8*)FontDataAnsi + FontDataAnsi->sheetImageOfs;
        FontDataAnsi->sheetImageOfs = ROUND_UP(FontDataAnsi->sheetImageOfs, 32);
        ExpandFontSheet(FontDataAnsi, sheets,
                        (u8*)FontDataAnsi + FontDataAnsi->sheetImageOfs);

        FontDataSjis = (OSFontHeader*)((u8*)FontDataAnsi + 0x20120);
        if (ReadFont((u8*)font + 0xF4020, OS_FONT_ENCODE_SJIS, FontDataSjis) ==
            0) {
            return FALSE;
        }

        sheets = (u8*)FontDataSjis + FontDataSjis->sheetImageOfs;
        FontDataSjis->sheetImageOfs = ROUND_UP(FontDataSjis->sheetImageOfs, 32);
        ExpandFontSheet(FontDataSjis, sheets,
                        (u8*)FontDataSjis + FontDataSjis->sheetImageOfs);
        break;
    }

    return TRUE;
}

const char* OSGetFontTexture(const char* str, void** texOut, u32* xOut,
                             u32* yOut, u32* widthOut) {
    OSFontHeader* font;
    s32 numRestTex;
    u8* font_u8;
    u32 code;
    u32 sheet;
    u32 row;
    u32 col;
    u32 tmp;

    str = (const char*)ParseString(OSGetFontEncode(), (const u8*)str, &font,
                                   &code);

    // Font sheet on which the texture resides
    sheet = (s32)code / (font->sheetNumCol * font->sheetNumRow);
    // Font code texture
    *texOut = (font->sheetSize * sheet) + ((u8*)font + font->sheetImageOfs);

    // Number of succeeding textures on the sheet
    // TODO(kiwi) Permuter fake(?)match
    tmp = font->sheetNumRow;
    numRestTex = code - (sheet * (font->sheetNumCol * tmp));

    // Sheet row on which the texure resides
    row = numRestTex / font->sheetNumCol;
    // Sheet column on which the texture resides
    col = numRestTex - row * font->sheetNumCol;

    // Texture position
    *xOut = col * font->cellWidth;
    *yOut = row * font->cellHeight;

    if (widthOut != NULL) {
        // TODO(kiwi) Permuter fake(?)match
        font_u8 = (u8*)font;
        *widthOut = (font_u8 + font->widthTableOfs)[code];
    }

    return str;
}

const char* OSGetFontWidth(const char* str, u32* widthOut) {
    OSFontHeader* font;
    u8* font_u8;
    u32 code;

    str = (const char*)ParseString(OSGetFontEncode(), (const u8*)str, &font,
                                   &code);

    if (widthOut != NULL) {
        // TODO(kiwi) Permuter fake(?)match
        font_u8 = (u8*)font;
        *widthOut = (font_u8 + font->widthTableOfs)[code];
    }

    return str;
}
