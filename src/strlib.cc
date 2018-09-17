/* @summary strlib.cc: Implement the functions exported by the strlib.h module.
 */
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#include "strlib.h"

/* @summary Search a UTF-8 string for a nul-terminator.
 * @param start A pointer to the first character to examine.
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
 * @param start A pointer to the first character to examine.
 * @return A pointer to the terminating nul.
 */
static inline char16_t*
Utf16FindNul
(
    char16_t const *start
)
{
    char16_t *b=(char16_t*) start;
    size_t len = 0;
    if (start != NULL) {
        while (*start++) {
            len++;
        }
    }
    return (char16_t*)(b + len);
}

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
    struct STRING_INFO_UTF8 *o_strinfo, 
    struct STRING_INFO_UTF8 *o_bufinfo, 
    struct STRING_INFO_UTF8   *strinfo, 
    size_t                   max_chars, 
    char8_t const              *strbuf
)
{   /* the maximum number of bytes per UTF-8 codepoint is 4 */
    size_t  max_bytes =(max_chars * 4) + 1;
    size_t init_chars = 0;
    size_t init_bytes = 0;
    char8_t       *buf = NULL;

    /* determine the attributes of the initial contents */
    if (strbuf != NULL) {
        if (strinfo != NULL) {
            init_bytes = strinfo->LengthBytes;
            init_chars = strinfo->LengthChars;
        } else {
            init_bytes = strlen(strbuf) + 1;
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
            o_bufinfo->LengthChars =(max_bytes - 1)  / 4;
        }
        return buf;
    } else { /* memory allocation failed */
        if (o_strinfo) {
            memset(o_strinfo, 0, sizeof(STRING_INFO_UTF8));
        }
        if (o_bufinfo) {
            memset(o_bufinfo, 0, sizeof(STRING_INFO_UTF8));
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
    struct STRING_INFO_UTF8 *o_strinfo, 
    char8_t const              *strbuf
)
{
    size_t len_bytes = 0;
    size_t len_chars = 0;

    assert(o_strinfo != NULL);

    if (strbuf) {
        len_bytes = strlen(strbuf) + 1;
        len_chars = mbstowcs(NULL, strbuf, 0);
    }
    o_strinfo->Buffer      =(char8_t*) strbuf;
    o_strinfo->BufferEnd   =(char8_t*) strbuf + len_bytes;
    o_strinfo->LengthBytes = len_bytes;
    o_strinfo->LengthChars = len_chars;
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
    if (o_codepoint) *o_codepoint = 0xffffffff;
    if (o_bytecount) *o_bytecount = 0;
    return NULL;
}

STRLIB_API(char16_t*)
Utf16StringCreate
(
    struct STRING_INFO_UTF16 *o_strinfo, 
    struct STRING_INFO_UTF16 *o_bufinfo, 
    struct STRING_INFO_UTF16   *strinfo, 
    size_t                    max_chars, 
    char16_t const              *strbuf
)
{   /* the maximum number of bytes per UTF-16 codepoint is 4 - 
     * but this implementation assumes no surrogate pairs and 
     * so it is technically UCS-2 encoding instead of UTF-16 - 
     * only the basic multilingual plane is supported.
     */
    size_t  max_bytes =(max_chars * 2) + 2;
    size_t init_chars = 0;
    size_t init_bytes = 0;
    char16_t     *buf = NULL;
    char16_t     *nul = NULL;

    /* determine the attributes of the initial contents */
    if (strbuf != NULL) {
        if (strinfo != NULL) {
            init_bytes = strinfo->LengthBytes;
            init_chars = strinfo->LengthChars;
        } else {
            nul = Utf16FindNul(strbuf);
            init_bytes =(size_t)((char8_t *)nul - (char8_t *)strbuf) + 2;
            init_chars =(size_t)((char16_t*)nul - (char16_t*)strbuf);
        }
    }
    /* allocate at least enough data to store the string copy */
    if (max_bytes < init_bytes) {
        max_bytes = init_bytes;
    }
    if ((buf = (char16_t*) malloc(max_bytes)) != NULL) {
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
            o_strinfo->BufferEnd   =(char16_t*)((char8_t*)buf + init_bytes);
            o_strinfo->LengthBytes = init_bytes;
            o_strinfo->LengthChars = init_chars;
        }
        if (o_bufinfo) {
            o_bufinfo->Buffer      = buf;
            o_bufinfo->BufferEnd   =(char16_t*)((char8_t*)buf + max_bytes);
            o_bufinfo->LengthBytes = max_bytes;
            o_bufinfo->LengthChars =(max_bytes - 2) / 2;
        }
        return buf;
    } else { /* memory allocation failed */
        if (o_strinfo) {
            memset(o_strinfo, 0, sizeof(STRING_INFO_UTF16));
        }
        if (o_bufinfo) {
            memset(o_bufinfo, 0, sizeof(STRING_INFO_UTF16));
        }
        return NULL;
    }
}

STRLIB_API(void)
Utf16StringDelete
(
    char16_t *strbuf
)
{
    free(strbuf);
}

STRLIB_API(char16_t*)
Utf16StringFindNul
(
    char16_t const *start
)
{
    return Utf16FindNul(start);
}

STRLIB_API(void)
Utf16StringInfo
(
    struct STRING_INFO_UTF16 *o_strinfo, 
    char16_t const              *strbuf
)
{
    size_t len_bytes = 0;
    size_t len_chars = 0;
    char16_t    *nul = NULL;

    assert(o_strinfo != NULL);

    if (strbuf) {
        nul = Utf16FindNul(strbuf);
        len_bytes =(size_t)((char8_t *)nul - (char8_t *)strbuf) + 2;
        len_chars =(size_t)((char16_t*)nul - (char16_t*)strbuf);
    }
    o_strinfo->Buffer      =(char16_t*) (strbuf  );
    o_strinfo->BufferEnd   =(char16_t*)((char8_t*)strbuf + len_bytes);
    o_strinfo->LengthBytes = len_bytes;
    o_strinfo->LengthChars = len_chars;
}

STRLIB_API(char16_t*)
Utf16StringNextCodepoint
(
    char32_t   *o_codepoint, 
    uint32_t   *o_bytecount, 
    char16_t const  *bufitr
)
{
    if (bufitr != NULL) {
        if (bufitr[0] < 0xD800 || bufitr[0] > 0xDFFF) {
            if (o_codepoint)  *o_codepoint = bufitr[0];
            if (o_bytecount)  *o_bytecount = 2;
            return (char16_t*)(bufitr + 1);
        } else if (bufitr[0] >= 0xD800 && bufitr[0] <= 0xDBFF && bufitr[1] >= 0xDC00 && bufitr[1] <= 0xDFFF) {
            if (o_codepoint)  *o_codepoint = ((uint32_t)(bufitr[0] & 0x03FF) << 10 | ((uint32_t)(bufitr[1] & 0x03FF))) + 0x00010000;
            if (o_bytecount)  *o_bytecount = 4;
            return (char16_t*)(bufitr + 2);
        }
    }
    /* invalid codepoint, or NULL bufitr */
    if (o_codepoint) *o_codepoint = 0xffffffff;
    if (o_bytecount) *o_bytecount = 0;
    return NULL;
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

