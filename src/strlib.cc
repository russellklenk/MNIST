/* @summary strlib.cc: Implement the functions exported by the strlib.h module.
 */
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#include "strlib.h"

/* @summary Count the number of bytes between two pointer values.
 * @param _beg A pointer to the start of the range (inclusive).
 * @param _end A pointer to one-past the end of the range (exclusive).
 * @return The number of bytes between _beg and _end.
 */
#ifndef CountBytesInRange
#define CountBytesInRange(_beg, _end)                                          \
    (((uint8_t const*)(_end))-((uint8_t const*)(_beg)))
#endif

/* @summary Search a UTF-8 string for a nul-terminator.
 * @param start A pointer to the first codepoint to examine.
 * @return A pointer to the terminating nul.
 */
static inline char8_t*
Utf8FindNul
(
    char8_t const *start
)
{
    char8_t *b =(char8_t*) start;
    size_t len = 0;
    if (start != NULL) {
        len = strlen(start);
    }
    return (char8_t*)(b + len);
}

/* @summary Search a UTF-16 string for a nul-terminator.
 * @param start A pointer to the first codepoint to examine.
 * @return A pointer to the terminating nul.
 */
static inline char16_t*
Utf16FindNul
(
    char16_t const *start
)
{
    char16_t *nul = (char16_t*) start;
    if (start != NULL) {
        while (*nul) {
            ++nul;
        }
    } return nul;
}

/* @summary Search a UTF-32 string for a nul-terminator.
 * @param start A pointer to the first codepoint to examine.
 * @return A pointer to the terminating nul.
 */
static inline char32_t*
Utf32FindNul
(
    char32_t const *start
)
{
    char32_t *nul = (char32_t*) start;
    if (start != NULL) {
        while (*nul) {
            ++nul;
        }
    } return nul;
}

/* @summary Brute-force convert a UCS-4 codepoint to lower case.
 * Taken from https://github.com/sheredom/utf8.h/blob/master/utf8.h.
 * @param cp The UCS-4 codepoint to convert.
 * @return The corresponding 'lower case' codepoint.
 */
static uint32_t
Utf32ToLower
(
    uint32_t cp
)
{
    if (((0x0041 <= cp) && (0x005a >= cp)) ||
        ((0x00c0 <= cp) && (0x00d6 >= cp)) ||
        ((0x00d8 <= cp) && (0x00de >= cp)) ||
        ((0x0391 <= cp) && (0x03a1 >= cp)) ||
        ((0x03a3 <= cp) && (0x03ab >= cp))) {
        cp += 32;
    } else if (((0x0100 <= cp) && (0x012f >= cp)) ||
               ((0x0132 <= cp) && (0x0137 >= cp)) ||
               ((0x014a <= cp) && (0x0177 >= cp)) ||
               ((0x0182 <= cp) && (0x0185 >= cp)) ||
               ((0x01a0 <= cp) && (0x01a5 >= cp)) ||
               ((0x01de <= cp) && (0x01ef >= cp)) ||
               ((0x01f8 <= cp) && (0x021f >= cp)) ||
               ((0x0222 <= cp) && (0x0233 >= cp)) ||
               ((0x0246 <= cp) && (0x024f >= cp)) ||
               ((0x03d8 <= cp) && (0x03ef >= cp))) {
        cp |= 0x1;
    } else if (((0x0139 <= cp) && (0x0148 >= cp)) ||
               ((0x0179 <= cp) && (0x017e >= cp)) ||
               ((0x01af <= cp) && (0x01b0 >= cp)) ||
               ((0x01b3 <= cp) && (0x01b6 >= cp)) ||
               ((0x01cd <= cp) && (0x01dc >= cp))) {
        cp += 1;
        cp &=~0x1;
    } else {
        switch (cp) {
            case 0x0178: cp = 0x00ff; break;
            case 0x0243: cp = 0x0180; break;
            case 0x018e: cp = 0x01dd; break;
            case 0x023d: cp = 0x019a; break;
            case 0x0220: cp = 0x019e; break;
            case 0x01b7: cp = 0x0292; break;
            case 0x01c4: cp = 0x01c6; break;
            case 0x01c7: cp = 0x01c9; break;
            case 0x01ca: cp = 0x01cc; break;
            case 0x01f1: cp = 0x01f3; break;
            case 0x01f7: cp = 0x01bf; break;
            case 0x0187: cp = 0x0188; break;
            case 0x018b: cp = 0x018c; break;
            case 0x0191: cp = 0x0192; break;
            case 0x0198: cp = 0x0199; break;
            case 0x01a7: cp = 0x01a8; break;
            case 0x01ac: cp = 0x01ad; break;
            case 0x01af: cp = 0x01b0; break;
            case 0x01b8: cp = 0x01b9; break;
            case 0x01bc: cp = 0x01bd; break;
            case 0x01f4: cp = 0x01f5; break;
            case 0x023b: cp = 0x023c; break;
            case 0x0241: cp = 0x0242; break;
            case 0x03fd: cp = 0x037b; break;
            case 0x03fe: cp = 0x037c; break;
            case 0x03ff: cp = 0x037d; break;
            case 0x037f: cp = 0x03f3; break;
            case 0x0386: cp = 0x03ac; break;
            case 0x0388: cp = 0x03ad; break;
            case 0x0389: cp = 0x03ae; break;
            case 0x038a: cp = 0x03af; break;
            case 0x038c: cp = 0x03cc; break;
            case 0x038e: cp = 0x03cd; break;
            case 0x038f: cp = 0x03ce; break;
            case 0x0370: cp = 0x0371; break;
            case 0x0372: cp = 0x0373; break;
            case 0x0376: cp = 0x0377; break;
            case 0x03f4: cp = 0x03d1; break;
            case 0x03cf: cp = 0x03d7; break;
            case 0x03f9: cp = 0x03f2; break;
            case 0x03f7: cp = 0x03f8; break;
            case 0x03fa: cp = 0x03fb; break;
            default: break;
        }
    }
    return cp;
}

/* @summary Brute-force convert a UCS-4 codepoint to upper case.
 * Taken from https://github.com/sheredom/utf8.h/blob/master/utf8.h.
 * @param cp The UCS-4 codepoint to convert.
 * @return The corresponding 'upper case' codepoint.
 */
#if 0
static uint32_t
Utf32ToUpper
(
    uint32_t cp
)
{
    if (((0x0041 <= cp) && (0x005a >= cp)) ||
        ((0x00c0 <= cp) && (0x00d6 >= cp)) ||
        ((0x00d8 <= cp) && (0x00de >= cp)) ||
        ((0x0391 <= cp) && (0x03a1 >= cp)) ||
        ((0x03a3 <= cp) && (0x03ab >= cp))) {
        cp -= 32;
    } else if (((0x0100 <= cp) && (0x012f >= cp)) ||
               ((0x0132 <= cp) && (0x0137 >= cp)) ||
               ((0x014a <= cp) && (0x0177 >= cp)) ||
               ((0x0182 <= cp) && (0x0185 >= cp)) ||
               ((0x01a0 <= cp) && (0x01a5 >= cp)) ||
               ((0x01de <= cp) && (0x01ef >= cp)) ||
               ((0x01f8 <= cp) && (0x021f >= cp)) ||
               ((0x0222 <= cp) && (0x0233 >= cp)) ||
               ((0x0246 <= cp) && (0x024f >= cp)) ||
               ((0x03d8 <= cp) && (0x03ef >= cp))) {
        cp &=~0x1;
    } else if (((0x0139 <= cp) && (0x0148 >= cp)) ||
               ((0x0179 <= cp) && (0x017e >= cp)) ||
               ((0x01af <= cp) && (0x01b0 >= cp)) ||
               ((0x01b3 <= cp) && (0x01b6 >= cp)) ||
               ((0x01cd <= cp) && (0x01dc >= cp))) {
        cp -= 1;
        cp |= 0x1;
    } else {
        switch (cp) {
            case 0x00ff: cp = 0x0178; break;
            case 0x0180: cp = 0x0243; break;
            case 0x01dd: cp = 0x018e; break;
            case 0x019a: cp = 0x023d; break;
            case 0x019e: cp = 0x0220; break;
            case 0x0292: cp = 0x01b7; break;
            case 0x01c6: cp = 0x01c4; break;
            case 0x01c9: cp = 0x01c7; break;
            case 0x01cc: cp = 0x01ca; break;
            case 0x01f3: cp = 0x01f1; break;
            case 0x01bf: cp = 0x01f7; break;
            case 0x0188: cp = 0x0187; break;
            case 0x018c: cp = 0x018b; break;
            case 0x0192: cp = 0x0191; break;
            case 0x0199: cp = 0x0198; break;
            case 0x01a8: cp = 0x01a7; break;
            case 0x01ad: cp = 0x01ac; break;
            case 0x01b0: cp = 0x01af; break;
            case 0x01b9: cp = 0x01b8; break;
            case 0x01bd: cp = 0x01bc; break;
            case 0x01f5: cp = 0x01f4; break;
            case 0x023c: cp = 0x023b; break;
            case 0x0242: cp = 0x0241; break;
            case 0x037b: cp = 0x03fd; break;
            case 0x037c: cp = 0x03fe; break;
            case 0x037d: cp = 0x03ff; break;
            case 0x03f3: cp = 0x037f; break;
            case 0x03ac: cp = 0x0386; break;
            case 0x03ad: cp = 0x0388; break;
            case 0x03ae: cp = 0x0389; break;
            case 0x03af: cp = 0x038a; break;
            case 0x03cc: cp = 0x038c; break;
            case 0x03cd: cp = 0x038e; break;
            case 0x03ce: cp = 0x038f; break;
            case 0x0371: cp = 0x0370; break;
            case 0x0373: cp = 0x0372; break;
            case 0x0377: cp = 0x0376; break;
            case 0x03d1: cp = 0x03f4; break;
            case 0x03d7: cp = 0x03cf; break;
            case 0x03f2: cp = 0x03f9; break;
            case 0x03f8: cp = 0x03f7; break;
            case 0x03fb: cp = 0x03fa; break;
            default: break;
        }
    }
    return cp;
}
#endif

STRLIB_API(size_t)
ByteOrderMarkerForEncoding
(
    uint8_t *o_marker, 
    int text_encoding
)
{
    size_t bom_size = 0;
    switch(text_encoding) {
        case TEXT_ENCODING_UNSURE:
            { assert(0 && "TEXT_ENCODING_UNSURE is not valid");
            } break;
        case TEXT_ENCODING_UTF8:
            { bom_size    = 3;
              o_marker[0] = 0xEF;
              o_marker[1] = 0xBB;
              o_marker[2] = 0xBF;
              o_marker[4] = 0x00;
            } break;
        case TEXT_ENCODING_UTF16_MSB:
            { bom_size    = 2;
              o_marker[0] = 0xFE;
              o_marker[1] = 0xFF;
              o_marker[2] = 0x00;
              o_marker[3] = 0x00;
            } break;
        case TEXT_ENCODING_UTF16_LSB:
            { bom_size    = 2;
              o_marker[0] = 0xFF;
              o_marker[1] = 0xFE;
              o_marker[2] = 0x00;
              o_marker[3] = 0x00;
            } break;
        case TEXT_ENCODING_UTF32_MSB:
            { bom_size    = 4;
              o_marker[0] = 0x00;
              o_marker[1] = 0x00;
              o_marker[2] = 0xFE;
              o_marker[3] = 0xFF;
            } break;
        case TEXT_ENCODING_UTF32_LSB:
            { bom_size    = 4;
              o_marker[0] = 0xFF;
              o_marker[1] = 0xFE;
              o_marker[2] = 0x00;
              o_marker[3] = 0x00;
            } break;
        default:
            { assert(0 && "Unknown TEXT_ENCODING value");
            } break;
    }
    return bom_size;
}

STRLIB_API(int)
EncodingForByteOrderMarker
(
    size_t   *o_bytecount, 
    uint8_t const *marker
)
{
    size_t bom_size;
    int    encoding;

    if (marker[0] == 0x00) {
        if (marker[1] == 0x00 && marker[2] == 0xFE && marker[3] == 0xFF) {
            bom_size = 4;
            encoding = TEXT_ENCODING_UTF32_MSB;
        } else {
            bom_size = 0;
            encoding = TEXT_ENCODING_UNSURE;
        }
    } else if (marker[0] == 0xFF) {
        if (marker[1] == 0xFE) {
            if (marker[2] == 0x00 && marker[3] == 0x00) {
                bom_size = 4;
                encoding = TEXT_ENCODING_UTF32_LSB;
            } else {
                bom_size = 2;
                encoding = TEXT_ENCODING_UTF16_LSB;
            }
        } else {
            bom_size = 0;
            encoding = TEXT_ENCODING_UNSURE;
        }
    } else if (marker[0] == 0xFE && marker[1] == 0xFF) {
        bom_size = 2;
        encoding = TEXT_ENCODING_UTF16_MSB;
    } else if (marker[0] == 0xEF && marker[1] == 0xBB && marker[2] == 0xBF) {
        bom_size = 3;
        encoding = TEXT_ENCODING_UTF8;
    } else { /* no BOM, or unrecognized BOM */
        bom_size = 0;
        encoding = TEXT_ENCODING_UNSURE;
    }
    if (o_bytecount) *o_bytecount = bom_size;
    return encoding;
}

STRLIB_API(char8_t*)
Utf8StringCreate
(
    struct STRING_INFO *o_strinfo, 
    struct STRING_INFO *o_bufinfo, 
    struct STRING_INFO   *strinfo, 
    size_t              max_chars, 
    char8_t const         *strbuf
)
{   /* the maximum number of bytes per UTF-8 codepoint is 4 */
    size_t  max_bytes =(max_chars * UTF8_MAX_BYTES_PER_CODEPOINT) + UTF8_NUL_BYTES;
    size_t init_chars = 0;
    size_t init_bytes = 0;
    char8_t       *buf = NULL;

    /* determine the attributes of the initial contents */
    if (strbuf != NULL) {
        if (strinfo != NULL) {
            init_bytes = strinfo->LengthBytes;
            init_chars = strinfo->LengthChars;
        } else {
            init_bytes = strlen(strbuf) + UTF8_NUL_BYTES;
            init_chars = mbstowcs(NULL, strbuf, 0);
        }
    }
    /* allocate at least enough data to store the string copy */
    if (max_bytes < init_bytes) {
        max_bytes = init_bytes;
    }
    if ((buf = (char8_t*) malloc(max_bytes)) != NULL) {
        if (strbuf != NULL) {
            /* copy the input string, including nul */
            memcpy(buf, strbuf, init_bytes);
            buf[max_bytes-1] = 0;
        } else {
            /* nul-terminate the new buffer */
            buf[0] = buf[max_bytes-1] = 0;
        }
        if (o_strinfo) {
            o_strinfo->Buffer      = buf;
            o_strinfo->BufferEnd   = buf + init_bytes;
            o_strinfo->LengthBytes = init_bytes;
            o_strinfo->LengthChars = init_chars;
        }
        if (o_bufinfo) {
            o_bufinfo->Buffer      = buf;
            o_bufinfo->BufferEnd   = buf + max_bytes;
            o_bufinfo->LengthBytes = max_bytes;
            o_bufinfo->LengthChars =(max_bytes - UTF8_NUL_BYTES) / UTF8_MAX_BYTES_PER_CODEPOINT;
        }
        return buf;
    } else { /* memory allocation failed */
        if (o_strinfo) {
            memset(o_strinfo, 0, sizeof(STRING_INFO));
        }
        if (o_bufinfo) {
            memset(o_bufinfo, 0, sizeof(STRING_INFO));
        }
        return NULL;
    }
}

STRLIB_API(char8_t*)
Utf8StringCreateFromAscii
(
    struct STRING_INFO *o_strinfo, 
    struct STRING_INFO *o_bufinfo, 
    size_t              max_chars, 
    char const            *strbuf
)
{
    size_t         max_bytes =(max_chars * UTF8_MAX_BYTES_PER_CODEPOINT) + UTF8_NUL_BYTES;
    size_t        init_chars = 0;
    size_t        init_bytes = 1;
    char8_t             *buf = NULL;
    char8_t             *dit = NULL;
    unsigned char const *sit = NULL;
    unsigned char         ch;

    if (strbuf) { /* determine the number of bytes required to store the converted string */
        for (sit = (unsigned char const*) strbuf; ; ++sit) {
            if (*sit < 0x80) {
                init_bytes++;
                if (sit != 0) {
                    init_chars++;
                } else break;
            } else {
                init_bytes += 2;
                init_chars++;
            }
        }
    }
    if (max_bytes < init_bytes) { /* need at least enough data to store the string copy */
        max_bytes = init_bytes;
    }
    if ((buf = (char8_t*) malloc(max_bytes)) != NULL) { /* allocate the required memory */
        if (strbuf) { /* transcode data in strbuf */
            for (dit = buf, sit = (unsigned char const*) strbuf; ; ++sit) {
                ch = *sit;
                if  (ch < 0x80) { /* 0x00 => 0x7F */
                    *dit++ = ch;
                    if (ch == 0) {
                        break;
                    }
                } else { /* 0x80 => 0xFF */
                    *dit++ = (ch >>   6) | 0xC0;
                    *dit++ = (ch & 0x3F) | 0x80;
                }
            }
        } else {
            buf[0] = 0;
        }
        if (o_strinfo) {
            o_strinfo->Buffer      = buf;
            o_strinfo->BufferEnd   = buf + init_bytes;
            o_strinfo->LengthBytes = init_bytes;
            o_strinfo->LengthChars = init_chars;
        }
        if (o_bufinfo) {
            o_bufinfo->Buffer      = buf;
            o_bufinfo->BufferEnd   = buf + max_bytes;
            o_bufinfo->LengthBytes = max_bytes;
            o_bufinfo->LengthChars =(max_bytes - UTF8_NUL_BYTES) / UTF8_MAX_BYTES_PER_CODEPOINT;
        }
        return buf;
    } else { /* memory allocation failed */
        if (o_strinfo) {
            memset(o_strinfo, 0, sizeof(STRING_INFO));
        } 
        if (o_bufinfo) {
            memset(o_bufinfo, 0, sizeof(STRING_INFO));
        }
        return NULL;
    }
}

STRLIB_API(char8_t*)
Utf8StringCreateFromUtf16
(
    struct STRING_INFO *o_strinfo, 
    struct STRING_INFO *o_bufinfo, 
    size_t              max_chars, 
    char16_t const        *strbuf
)
{
    size_t     max_bytes =(max_chars * UTF8_MAX_BYTES_PER_CODEPOINT) + UTF8_NUL_BYTES;
    size_t    init_chars = 0;
    size_t    init_bytes = 1;
    char8_t         *buf = NULL;
    char8_t         *dit = NULL;
    char16_t const  *sit = NULL;
    uint32_t const bmask = 0xBF;
    uint32_t const bmark = 0x80;
    char32_t      ch, c2;
    uint8_t           nb;
    uint8_t  const  F[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

    if (strbuf) { /* determine the number of bytes required to store the converted string */
        for (sit = strbuf; ; ++sit) {
            ch = *sit;
            if (ch >= 0xD800 && ch <= 0xDBFF) { /* convert surrogate pairs to UTF-32 */
                c2 = *(sit+1);
                if (c2 >= 0xDC00 && c2 <= 0xDFFF) {
                    ch = ((ch - 0xD800) << 10) + (c2 - 0xDC00) + 0x10000;
                }
            }
            if (ch < 0x80) {
                init_bytes++;
                if (ch != 0) {
                    init_chars++;
                } else break;
            } else if (ch < 0x800) {
                init_bytes += 2;
                init_chars++;
            } else if (ch < 0x10000) {
                init_bytes += 3;
                init_chars++;
            } else if (ch < 0x110000) {
                init_bytes += 4;
                init_chars++;
            } else {
                if (o_strinfo) {
                    memset(o_strinfo, 0, sizeof(STRING_INFO));
                }
                if (o_bufinfo) {
                    memset(o_bufinfo, 0, sizeof(STRING_INFO));
                } errno = EILSEQ;
                return NULL;
            }
        }
    }
    if (max_bytes < init_bytes) { /* need at least enough data to store the string copy */
        max_bytes = init_bytes;
    }
    if ((buf = (char8_t*) malloc(max_bytes)) != NULL) { /* allocate the required memory */
        if (strbuf) { /* transcode data in strbuf */
            for (dit = buf, sit = strbuf; ; ++sit) {
                ch = *sit;
                if (ch >= 0xD800 && ch <= 0xDBFF) { /* convert surrogate pairs to UTF-32 */
                    c2 = *(sit+1);
                    if (c2 >= 0xDC00 && c2 <= 0xDFFF) {
                        ch = ((ch - 0xD800) << 10) + (c2 - 0xDC00) + 0x10000;
                        sit++; /* also consume low surrogate */
                    }
                }
                if      (ch < 0x80    ) nb = 1;
                else if (ch < 0x800   ) nb = 2;
                else if (ch < 0x10000 ) nb = 3;
                else if (ch < 0x110000) nb = 4;
                switch (nb) {
                    case 4: dit[3] = (char8_t)((ch | bmark) & bmask); ch >>= 6; /* fallthrough */
                    case 3: dit[2] = (char8_t)((ch | bmark) & bmask); ch >>= 6; /* fallthrough */
                    case 2: dit[1] = (char8_t)((ch | bmark) & bmask); ch >>= 6; /* fallthrough */
                    case 1: dit[0] = (char8_t) (ch | F[nb]); /* fallthrough */
                } dit += nb;
                if (ch == 0) {
                    break;
                }
            }
        } else {
            buf[0] = 0;
        }
        if (o_strinfo) {
            o_strinfo->Buffer      = buf;
            o_strinfo->BufferEnd   = buf + init_bytes;
            o_strinfo->LengthBytes = init_bytes;
            o_strinfo->LengthChars = init_chars;
        }
        if (o_bufinfo) {
            o_bufinfo->Buffer      = buf;
            o_bufinfo->BufferEnd   = buf + max_bytes;
            o_bufinfo->LengthBytes = max_bytes;
            o_bufinfo->LengthChars =(max_bytes - UTF8_NUL_BYTES) / UTF8_MAX_BYTES_PER_CODEPOINT;
        }
        return buf;
    } else { /* memory allocation failed */
        if (o_strinfo) {
            memset(o_strinfo, 0, sizeof(STRING_INFO));
        } 
        if (o_bufinfo) {
            memset(o_bufinfo, 0, sizeof(STRING_INFO));
        }
        return NULL;
    }
}

STRLIB_API(char8_t*)
Utf8StringCreateFromUtf32
(
    struct STRING_INFO *o_strinfo, 
    struct STRING_INFO *o_bufinfo, 
    size_t              max_chars, 
    char32_t const        *strbuf
)
{
    size_t     max_bytes =(max_chars * UTF8_MAX_BYTES_PER_CODEPOINT) + UTF8_NUL_BYTES;
    size_t    init_chars = 0;
    size_t    init_bytes = 1;
    char8_t         *buf = NULL;
    char8_t         *dit = NULL;
    char32_t const  *sit = NULL;
    uint32_t const bmask = 0xBF;
    uint32_t const bmark = 0x80;
    char32_t          ch = 0;
    uint8_t           nb = 0;
    uint8_t  const  F[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

    if (strbuf) { /* determine the number of bytes required to store the converted string */
        for (sit = strbuf; ; ++sit) {
            ch = *sit;
            if (ch < 0x80) {
                init_bytes++;
                if (ch != 0) {
                    init_chars++;
                } else break;
            } else if (ch < 0x800) {
                init_bytes += 2;
                init_chars++;
            } else if (ch < 0x10000) {
                init_bytes += 3;
                init_chars++;
            } else if (ch < 0x10FFFF) {
                init_bytes += 4;
                init_chars++;
            } else {
                if (o_strinfo) {
                    memset(o_strinfo, 0, sizeof(STRING_INFO));
                }
                if (o_bufinfo) {
                    memset(o_bufinfo, 0, sizeof(STRING_INFO));
                } errno = EILSEQ;
                return NULL;
            }
        }
    }
    if (max_bytes < init_bytes) { /* need at least enough data to store the string copy */
        max_bytes = init_bytes;
    }
    if ((buf = (char8_t*) malloc(max_bytes)) != NULL) { /* allocate the required memory */
        if (strbuf) { /* transcode data in strbuf */
            for (dit = buf, sit = strbuf; ; ++sit) {
                ch = *sit;
                if      (ch < 0x80    ) nb = 1;
                else if (ch < 0x800   ) nb = 2;
                else if (ch < 0x10000 ) nb = 3;
                else if (ch < 0x10FFFF) nb = 4;
                switch (nb) {
                    case 4: dit[3] = (char8_t)((ch | bmark) & bmask); ch >>= 6; /* fallthrough */
                    case 3: dit[2] = (char8_t)((ch | bmark) & bmask); ch >>= 6; /* fallthrough */
                    case 2: dit[1] = (char8_t)((ch | bmark) & bmask); ch >>= 6; /* fallthrough */
                    case 1: dit[0] = (char8_t) (ch | F[nb]); /* fallthrough */
                } dit += nb;
                if (ch == 0) {
                    break;
                }
            }
        } else {
            buf[0] = 0;
        }
        if (o_strinfo) {
            o_strinfo->Buffer      = buf;
            o_strinfo->BufferEnd   = buf + init_bytes;
            o_strinfo->LengthBytes = init_bytes;
            o_strinfo->LengthChars = init_chars;
        }
        if (o_bufinfo) {
            o_bufinfo->Buffer      = buf;
            o_bufinfo->BufferEnd   = buf + max_bytes;
            o_bufinfo->LengthBytes = max_bytes;
            o_bufinfo->LengthChars =(max_bytes - UTF8_NUL_BYTES) / UTF8_MAX_BYTES_PER_CODEPOINT;
        }
        return buf;
    } else { /* memory allocation failed */
        if (o_strinfo) {
            memset(o_strinfo, 0, sizeof(STRING_INFO));
        } 
        if (o_bufinfo) {
            memset(o_bufinfo, 0, sizeof(STRING_INFO));
        }
        return NULL;
    }
}

STRLIB_API(char16_t*)
Utf8StringConvertToUtf16
(
    struct STRING_INFO *o_u16info, 
    struct STRING_INFO   *strinfo, 
    char8_t const         *strbuf
)
{
    STRING_INFO  sinfo;
    uint32_t max_bytes = 0;
    char32_t     utf32 = 0;
    uint32_t    nbytes = 0;
    uint32_t    nchars = 0;
    char16_t      *buf = NULL;
    char16_t      *dit = NULL;

    if (strbuf) {
        if (strinfo) {
            memcpy(&sinfo, strinfo, sizeof(STRING_INFO));
        } else {
            Utf8StringInfo(&sinfo , strbuf);
        }
    } else {
        memset(&sinfo, 0, sizeof(STRING_INFO));
    }
    max_bytes = (sinfo.LengthChars * UTF16_MAX_BYTES_PER_CODEPOINT) + UTF16_NUL_BYTES;
    if ((buf  = (char16_t*) malloc(max_bytes)) != NULL) {
        if (strbuf) {
            for (dit = buf; sinfo.Buffer != sinfo.BufferEnd; ) {
                sinfo.Buffer = Utf8StringNextCodepoint(&utf32, &nbytes, sinfo.Buffer);
                if (utf32 <= 0xFFFF) { /* basic multilingual plane */
                    *dit++ =(char16_t) utf32;
                } else if (utf32 <= 0x10FFFF) { /* surrogate pair */
                    utf32 -= 0x10000;
                    *dit++ =(char16_t)((utf32 >>   10) + 0xD800);
                    *dit++ =(char16_t)((utf32 & 0x3FF) + 0xDBFF);
                } else { /* invalid - use replacement character */
                    *dit++ = 0xFFFD;
                }
                if (utf32 != 0) {
                    nchars++;
                }
            }
        } else {
            buf[0] = 0;
        }
        if (o_u16info) {
            o_u16info->Buffer      = (char8_t*) buf;
            o_u16info->BufferEnd   =((char8_t*) buf) + max_bytes;
            o_u16info->LengthBytes = max_bytes;
            o_u16info->LengthChars = sinfo.LengthChars;
        }
        return buf;
    } else { /* memory allocation failed */
        if (o_u16info) {
            memset(o_u16info, 0, sizeof(STRING_INFO));
        }
        return NULL;
    }
}

STRLIB_API(char32_t*)
Utf8StringConvertToUtf32
(
    struct STRING_INFO *o_u32info, 
    struct STRING_INFO   *strinfo, 
    char8_t const         *strbuf
)
{
    STRING_INFO  sinfo;
    uint32_t max_bytes = 0;
    char32_t     utf32 = 0;
    uint32_t    nbytes = 0;
    uint32_t    nchars = 0;
    char32_t      *buf = NULL;
    char32_t      *dit = NULL;

    if (strbuf) {
        if (strinfo) {
            memcpy(&sinfo, strinfo, sizeof(STRING_INFO));
        } else {
            Utf8StringInfo(&sinfo , strbuf);
        }
    } else {
        memset(&sinfo, 0, sizeof(STRING_INFO));
    }
    max_bytes = (sinfo.LengthChars * UTF32_MAX_BYTES_PER_CODEPOINT) + UTF32_NUL_BYTES;
    if ((buf  = (char32_t*) malloc(max_bytes)) != NULL) {
        if (strbuf) {
            for (dit = buf; sinfo.Buffer != sinfo.BufferEnd; ) {
                sinfo.Buffer = Utf8StringNextCodepoint(&utf32, &nbytes, sinfo.Buffer);
                if (utf32 <= 0x10FFFF) {
                    *dit++ = utf32;
                } else { /* use replacement character */
                    *dit++ = 0xFFFD;
                }
                if (utf32 != 0) {
                    nchars++;
                }
            }
        } else {
            buf[0] = 0;
        }
        if (o_u32info) {
            o_u32info->Buffer      = (char8_t*) buf;
            o_u32info->BufferEnd   =((char8_t*) buf) + max_bytes;
            o_u32info->LengthBytes = max_bytes;
            o_u32info->LengthChars = sinfo.LengthChars;
        }
        return buf;
    } else { /* memory allocation failed */
        if (o_u32info) {
            memset(o_u32info, 0, sizeof(STRING_INFO));
        }
        return NULL;
    }
}

STRLIB_API(void)
Utf8StringDelete
(
    char8_t *strbuf
)
{
    free(strbuf);
}

STRLIB_API(void)
Utf16StringDelete
(
    char16_t *strbuf
)
{
    free(strbuf);
}

STRLIB_API(void)
Utf32StringDelete
(
    char32_t *strbuf
)
{
    free(strbuf);
}

STRLIB_API(size_t)
Utf8StringByteCount
(
    char8_t const *beg, 
    char8_t const *end
)
{
    assert(beg <= end);
    return (size_t)(((char8_t const*) end) - ((char8_t const*) beg));
}

STRLIB_API(char8_t*)
Utf8StringFindNul
(
    char8_t const *start
)
{
    return Utf8FindNul(start);
}

STRLIB_API(void)
Utf8StringInfo
(
    struct STRING_INFO*o_strinfo, 
    char8_t const        *strbuf
)
{
    size_t len_bytes = 0;
    size_t len_chars = 0;

    assert(o_strinfo != NULL);

    if (strbuf) {
        len_bytes = strlen(strbuf) + UTF8_NUL_BYTES;
        len_chars = mbstowcs(NULL, strbuf, 0);
    }
    o_strinfo->Buffer      =(char8_t*) strbuf;
    o_strinfo->BufferEnd   =(char8_t*) strbuf + len_bytes;
    o_strinfo->LengthBytes = len_bytes;
    o_strinfo->LengthChars = len_chars;
}

STRLIB_API(int)
Utf8StringCompare
(
    char8_t const *a, 
    char8_t const *b
)
{
    return strcmp(a, b);
}

STRLIB_API(int)
Utf8StringCompareNoCase
(
    char8_t const *a, 
    char8_t const *b
)
{
    char8_t *ita =(char8_t*) a;
    char8_t *itb =(char8_t*) b;
    char32_t cpa;
    char32_t cpb;

    for ( ; ; ) {
        ita = Utf8StringNextCodepoint(&cpa, NULL, ita);
        itb = Utf8StringNextCodepoint(&cpb, NULL, itb);
        cpa = Utf32ToLower(cpa);
        cpb = Utf32ToLower(cpb);
        if (cpa == cpb) {
            if (cpa == 0) {
                return 0;
            }
        } else if (cpa < cpb) {
            return -1;
        } else {
            return +1;
        }
    }
}

STRLIB_API(char8_t*)
Utf8StringNextCodepoint
(
    char32_t   *o_codepoint, 
    uint32_t   *o_bytecount, 
    char8_t const   *bufitr
)
{
    if (bufitr != NULL) {
        if ((bufitr[0] & 0x80) == 0) { /* 0x00000 => 0x0007F */
            if (o_codepoint) *o_codepoint = bufitr[0];
            if (o_bytecount) *o_bytecount = 1;
            return(char8_t*) (bufitr + 1);
        }
        if ((bufitr[0] & 0xFF) >= 0xC2 &&   (bufitr[0] & 0xFF) <= 0xDF && (bufitr[1] & 0xC0) == 0x80) { /* 0x00080 => 0x007ff */
            if (o_codepoint) *o_codepoint =((bufitr[0] & 0x1F) <<  6)  |  (bufitr[1] & 0x3F);
            if (o_bytecount) *o_bytecount = 2;
            return(char8_t*) (bufitr + 2);
        }
        if ((bufitr[0] & 0xF0) == 0xE0 &&   (bufitr[1] & 0xC0) == 0x80 && (bufitr[2] & 0xC0) == 0x80) { /* 0x00800 => 0x0ffff */
            if (o_codepoint) *o_codepoint =((bufitr[0] & 0x0F) << 12)  | ((bufitr[1] & 0x3F) <<  6) | (bufitr[2] & 0x3F);
            if (o_bytecount) *o_bytecount = 3;
            return(char8_t*) (bufitr + 3);
        }
        if ((bufitr[0] & 0xFF) == 0xF0 &&   (bufitr[1] & 0xC0) == 0x80 && (bufitr[2] & 0xC0) == 0x80 && (bufitr[3] & 0xC0) == 0x80) { /* 0x10000 => 0x3ffff */
            if (o_codepoint) *o_codepoint =((bufitr[1] & 0x3F) << 12)  | ((bufitr[2] & 0x3F) <<  6)   | (bufitr[3] & 0x3F);
            if (o_bytecount) *o_bytecount = 4;
            return(char8_t*) (bufitr + 4);
        }
    }
    /* invalid codepoint, or NULL bufitr */
    if (o_codepoint) *o_codepoint = 0;
    if (o_bytecount) *o_bytecount = 0;
    return NULL;
}

STRLIB_API(char8_t*)
Utf8StringPrevCodepoint
(
    char32_t *o_codepoint, 
    uint32_t *o_bytecount, 
    char8_t const *bufitr
)
{
    if (bufitr) {
        char8_t   *p;
        uint32_t   n = 0;
        if ((*bufitr & 0xC0) == 0) { /* bufitr points at the start of a codepoint */
            p = (char8_t*)(bufitr - 1);
        } else { /* bufitr points within a codepoint */
            p = (char8_t*)(bufitr);
        }
        while ((*p & 0xC0) == 0x80 && n < UTF8_MAX_BYTES_PER_CODEPOINT) {
            --p;
            ++n;
        }
        Utf8StringNextCodepoint(o_codepoint, o_bytecount, p);
        return p;
    }
    if (o_codepoint) *o_codepoint = 0;
    if (o_bytecount) *o_bytecount = 0;
    return NULL;
}

STRLIB_API(char8_t*)
Utf8StringCopyCodepoint
(
    uint32_t *o_bytecount, 
    uint32_t *o_wordcount, 
    char8_t          *dst, 
    char8_t const    *src
)
{
    if (src != NULL) {
        if ((src[0] & 0x80) == 0) { /* 0x00000 => 0x0007F */
            if (o_bytecount) *o_bytecount = 1;
            if (o_wordcount) *o_wordcount = 1;
            if (dst) {
               *dst++ = src[0];
            } return (char8_t*)(src + 1);
        }
        if ((src[0] & 0xFF) >= 0xC2 && (src[0] & 0xFF) <= 0xDF && (src[1] & 0xC0) == 0x80) { /* 0x00080 => 0x007ff */
            if (o_bytecount) *o_bytecount = 2;
            if (o_wordcount) *o_wordcount = 2;
            if (dst) {
               *dst++ = src[0];
               *dst++ = src[1];
            } return (char8_t*)(src + 2);
        }
        if ((src[0] & 0xF0) == 0xE0 && (src[1] & 0xC0) == 0x80 && (src[2] & 0xC0) == 0x80) { /* 0x00800 => 0x0ffff */
            if (o_bytecount) *o_bytecount = 3;
            if (o_wordcount) *o_wordcount = 3;
            if (dst) {
               *dst++ = src[0];
               *dst++ = src[1];
               *dst++ = src[2];
            } return (char8_t*)(src + 3);
        }
        if ((src[0] & 0xFF) == 0xF0 && (src[1] & 0xC0) == 0x80 && (src[2] & 0xC0) == 0x80 && (src[3] & 0xC0) == 0x80) { /* 0x10000 => 0x3ffff */
            if (o_bytecount) *o_bytecount = 4;
            if (o_wordcount) *o_wordcount = 4;
            if (dst) {
               *dst++ = src[0];
               *dst++ = src[1];
               *dst++ = src[2];
               *dst++ = src[3];
            } return (char8_t*)(src + 4);
        }
    }
    if (o_bytecount) *o_bytecount = 0;
    if (o_wordcount) *o_wordcount = 0;
    return NULL;
}

STRLIB_API(int)
Utf8StringAppend
(
    struct STRING_INFO     *o_dstinfo, 
    struct STRING_INFO       *dstinfo, 
    struct STRING_INFO       *srcinfo, 
    size_t              max_dst_bytes,
    char8_t       * __restrict dstbuf, 
    char8_t const * __restrict srcbuf
)
{
    STRING_INFO dinfo;
    STRING_INFO sinfo;
    char8_t           *nul;

    memset(&dinfo, 0, sizeof(STRING_INFO));
    memset(&sinfo, 0, sizeof(STRING_INFO));

    if (srcbuf != NULL) {
        if (srcinfo != NULL) {
            memcpy(&sinfo, srcinfo, sizeof(STRING_INFO));
        } else {
            Utf8StringInfo(&sinfo , srcbuf);
        }
    }
    if (dstbuf != NULL) {
        if (dstinfo != NULL) {
            memcpy(&dinfo, dstinfo, sizeof(STRING_INFO));
        } else {
            Utf8StringInfo(&dinfo , dstbuf);
        }
    }
    if (sinfo.LengthChars == 0) {
        if (o_dstinfo) {
            memcpy(o_dstinfo, &dinfo, sizeof(STRING_INFO));
        } return 0;
    }
    if (dinfo.LengthBytes == 0) {
        if (o_dstinfo) {
            memcpy(o_dstinfo, &dinfo, sizeof(STRING_INFO));
        } errno = ENOBUFS;
        return -1;
    }
    if (max_dst_bytes < (sinfo.LengthBytes+dinfo.LengthBytes-UTF8_NUL_BYTES)) {
        if (o_dstinfo) {
            memcpy(o_dstinfo, &dinfo, sizeof(STRING_INFO));
        } errno = ENOBUFS;
        return -1;
    }
    nul = &dinfo.BufferEnd[-1];
    memcpy(nul, srcbuf, sinfo.LengthBytes);
    if (o_dstinfo) {
        o_dstinfo->BufferEnd    =((char8_t*) dinfo.Buffer) + (dinfo.LengthBytes-UTF8_NUL_BYTES) + sinfo.LengthBytes;
        o_dstinfo->LengthBytes += sinfo.LengthBytes - UTF8_NUL_BYTES;
        o_dstinfo->LengthChars += sinfo.LengthChars;
    }
    return 0;
}

STRLIB_API(size_t)
BinarySizeForBase64
(
    size_t b64size
)
{
    return ((3 * b64size) / 4);
}

STRLIB_API(size_t)
BinarySizeForBase64Data
(
    char const *encbuf
)
{
    if (encbuf != NULL) {
        size_t pad_bytes = 0;
        size_t len_bytes = strlen(encbuf);
        char const  *end = encbuf + len_bytes;
        if (len_bytes >= 1 && '=' == *end--) pad_bytes++;
        if (len_bytes >= 2 && '=' == *end--) pad_bytes++;
        return (((3 * len_bytes) / 4) - pad_bytes);
    }
    return 0;
}

STRLIB_API(size_t)
Base64SizeForBinary
(
    size_t *o_padsize, 
    size_t    binsize
)
{   /* three input bytes is transformed into 4 output bytes. 
     * padding bytes ensure the input size is evenly divisible by 3. */
    size_t rem = binsize   % 3;
    size_t pad =(rem != 0) ? 3 - rem : 0;
    if (o_padsize) *o_padsize  = pad;
    return ((binsize + pad) / 3) * 4 + 1; /* +1 for nul */
}

STRLIB_API(int)
Base64Encode
(
    size_t      * __restrict o_numdst,
    void        * __restrict      dst, 
    size_t                    max_dst, 
    void const  * __restrict      src, 
    size_t                    num_src
)
{
    size_t         pad_bytes = 0;
    size_t         req_bytes = 0;
    size_t               ins = num_src;
    uint8_t const       *inp =(uint8_t const*) src;
    char                *out =(char         *) dst;
    uint8_t           buf[4] ={ 0x00, 0x00, 0x00, 0x00 };
    char const        B64[ ] ="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    if (src == NULL || num_src == 0) { /* no input data */
        if (o_numdst) {
           *o_numdst = 0;
        } return 0;
    }
    
    req_bytes = Base64SizeForBinary(&pad_bytes, num_src);
    if (dst == NULL && max_dst == 0) {
        if (o_numdst) {
           *o_numdst = req_bytes;
        } return 0;
    }
    if (req_bytes > max_dst) {
        if (o_numdst) {
           *o_numdst = 0;
        }
        errno = ENOBUFS;
        return -1;
    }
    while (ins > 3) {
        /* process input three bytes at a time.
         * buf[0] = left  6 bits of inp[0].
         * buf[1] = right 2 bits of inp[0], left 4 bits if inp[1].
         * buf[2] = right 4 bits of inp[1], left 2 bits of inp[2].
         * buf[3] = right 6 bits of inp[2]. 
         * produce four output bytes at a time. */
        buf[0] = (uint8_t)  ((inp[0] & 0xFC) >> 2);
        buf[1] = (uint8_t) (((inp[0] & 0x03) << 4) + ((inp[1] & 0xF0) >> 4));
        buf[2] = (uint8_t) (((inp[1] & 0x0F) << 2) + ((inp[2] & 0xC0) >> 6));
        buf[3] = (uint8_t)   (inp[2] & 0x3F);
       *out++  = B64[buf[0]];
       *out++  = B64[buf[1]];
       *out++  = B64[buf[2]];
       *out++  = B64[buf[2]];
        inp   += 3; ins -= 3;
    }
    if (ins > 0) {
        /* pad any remaining input (either 1 or 2 bytes) to three bytes */
        uint8_t  s[3];
        size_t      i;
        for (i = 0; i < ins; ++i) { /* copy remaining data from source buffer */
            s[i] = *inp++;
        }
        for (     ; i != 3; ++i) { /* set pad bytes to nul */
            s[i] = 0;
        }
        buf[0] = (uint8_t)  ((s[0] & 0xFC) >> 2);
        buf[1] = (uint8_t) (((s[0] & 0x03) << 4) + ((s[1] & 0xF0) >> 4));
        buf[2] = (uint8_t) (((s[1] & 0x0F) << 2) + ((s[2] & 0xC0) >> 6));
        buf[3] = (uint8_t)   (s[2] & 0x3F);
       *out++  = B64[buf[0]];
       *out++  = B64[buf[1]];
       *out++  = B64[buf[2]];
       *out++  = B64[buf[2]];
        for (out += 1 + ins; ins++ != 3; ) {
            *out++ = '=';
        }
    }
    /* nul-terminate the destination buffer */
    *out++ = 0;
    if (o_numdst) {
       *o_numdst =(size_t)(out - (char*) dst);
    }
    return 0;
}

STRLIB_API(int)
Base64Decode
(
    size_t     * __restrict o_numdst, 
    void       * __restrict      dst, 
    size_t                   max_dst, 
    void const * __restrict      src, 
    size_t                   num_src
)
{
    char const         *inp = (char const*) src;
    char const         *end = (char const*) src + num_src;
    uint8_t            *out = (uint8_t   *) dst;
    size_t              cur = 0;
    size_t              pad = 0;
    size_t        req_bytes = 0;
    signed char      idx[4];
    signed char         chi;
    char                 ch;

    /* a lookup table to map the 256 possible byte values to a value in [0, 63] 
     * or -1 if the value is not valid in a base64-encoded input stream. 
     */
    signed char const B64[ ] = {
        -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1,

        -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, 62, -1, -1, -1, 63,  /* ... , '+', ... '/' */
        52, 53, 54, 55, 56, 57, 58, 59,  /* '0' - '7'          */
        60, 61, -1, -1, -1, -1, -1, -1,  /* '8', '9', ...      */

        -1, 0,  1,  2,  3,  4,  5,  6,   /* ..., 'A' - 'G'     */
         7, 8,  9,  10, 11, 12, 13, 14,  /* 'H' - 'O'          */
        15, 16, 17, 18, 19, 20, 21, 22,  /* 'P' - 'W'          */
        23, 24, 25, -1, -1, -1, -1, -1,  /* 'X', 'Y', 'Z', ... */

        -1, 26, 27, 28, 29, 30, 31, 32,  /* ..., 'a' - 'g'     */
        33, 34, 35, 36, 37, 38, 39, 40,  /* 'h' - 'o'          */
        41, 42, 43, 44, 45, 46, 47, 48,  /* 'p' - 'w'          */
        49, 50, 51, -1, -1, -1, -1, -1,  /* 'x', 'y', 'z', ... */

        -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1,

        -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1,

        -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1,

        -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1
    };

    if (src == NULL) { /* no input data */
        if (o_numdst) {
           *o_numdst = 0;
        } return 0;
    }
    if (num_src == 0) { /* assume input buffer is nul-terminated */
        num_src = strlen((char const*) src);
    }
    if (num_src == 0) { /* no input data */
        if (o_numdst) {
           *o_numdst = 0;
        } return 0;
    }

    req_bytes = BinarySizeForBase64(num_src);
    if (dst == NULL && max_dst == 0) {
        if (o_numdst) {
           *o_numdst = req_bytes;
        } return 0;
    }
    if (req_bytes > max_dst) {
        if (o_numdst) {
           *o_numdst = 0;
        } errno = ENOBUFS;
        return -1;
    }
    while (inp != end) {
        if ((ch = *inp++) != '=') {
            if ((chi =B64[(unsigned)ch]) != -1) { /* valid base64 input character */
                idx[cur++] = chi;
            } else {
                if (o_numdst) {
                   *o_numdst = (size_t)(inp - (char const*) src);
                } errno = EILSEQ;
                return -1;
            }
        } else { /* padding character */
            idx[cur++] = 0; pad++;
        }

        if (cur == 4) {
            cur  = 0;
           *out++=(uint8_t)((idx[0] << 2) + ((idx[1] & 0x30) >> 4));
            if (pad != 2) {
                *out++=(uint8_t)(((idx[1] & 0xF) << 4) + ((idx[2] & 0x3C) >> 2));
                if (pad != 1) {
                    *out++=(uint8_t)(((idx[2] & 0x3) << 6) + idx[3]);
                }
            } pad = 0;
        }
    }
    if (o_numdst) {
       *o_numdst = (size_t)(out - (uint8_t*) dst);
    }
    return 0;
}

