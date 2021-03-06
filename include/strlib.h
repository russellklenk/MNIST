/**
 * strlib.h: Defines some helper routines and data structures for working with 
 * nul-terminated string data.
 */
#ifndef __STRLIB_H__
#define __STRLIB_H__

#pragma once

#ifndef STRLIB_NO_INCLUDES
#include <stddef.h>
#include <stdint.h>
#include <wchar.h>
#if !defined(_MSC_VER) || (defined(_MSC_VER) && (_MSC_VER > 1800))
#include <uchar.h>
#endif
#endif

#ifndef STRLIB_API
#ifdef  STRLIB_STATIC
#define STRLIB_API(_return_type)                                               \
    static _return_type
#else
#define STRLIB_API(_return_type)                                               \
    extern _return_type
#endif /* STRLIB_STATIC */
#endif /* STRLIB_API */

/* @summary Typedef the various Unicode character types.
 * By default, the uchar.h header defines the char16_t and char32_t types.
 * However, the Visual C++ compiler included with Visual Studio 2013 does not include this header.
 */
typedef char            char8_t;
#ifndef __STDC_UTF_16__
typedef unsigned short char16_t;
#endif
#ifndef __STDC_UTF_32__
typedef unsigned int   char32_t;
#endif

/* @summary Define various constants used internally within this module.
 * UTF8_NUL_BYTES               : The number of bytes used for a nul-terminator in a UTF-8 encoded string.
 * UTF16_NUL_BYTES              : The number of bytes used for a nul-terminator in a UTF-16 encoded string.
 * UTF8_MAX_BYTES_PER_CODEPOINT : The maximum number of bytes that may be used to encode a valid codepoint in a UTF-8 encoded string.
 * UTF16_MAX_BYTES_PER_CODEPOINT: The maximum number of bytes that may be used to encode a valid codepoint in a UTF-16 encoded string.
 */
#ifndef STRLIB_CONSTANTS
#   define STRLIB_CONSTANTS
#   define UTF8_NUL_BYTES                   1
#   define UTF16_NUL_BYTES                  2
#   define UTF32_NUL_BYTES                  4
#   define UTF8_MAX_BYTES_PER_CODEPOINT     4
#   define UTF16_MAX_BYTES_PER_CODEPOINT    4
#   define UTF32_MAX_BYTES_PER_CODEPOINT    4
#endif

/* @summary Define a structure for storing commonly-needed data about a UTF-8 encoded string.
 */
typedef struct STRING_INFO {
    char8_t                     *Buffer;                                       /* Points to the start of the string buffer. */
    char8_t                     *BufferEnd;                                    /* Points to one byte past the terminating nul byte of the string buffer. */
    size_t                       LengthBytes;                                  /* The length of the string buffer, in bytes, including the terminating nul. */
    size_t                       LengthChars;                                  /* The length of the string buffer, in characters, not including the terminating nul. */
} STRING_INFO;

/* @summary Define constants for the different text encodings that can be determined by inspecting the first four bytes of a file for a byte-order marker.
 */
typedef enum TEXT_ENCODING {
    TEXT_ENCODING_UNSURE         =  0,                                         /* The text encoding is not known or could not be determined. */
    TEXT_ENCODING_UTF8           =  1,                                         /* The text encoding was determined to be UTF-8. */
    TEXT_ENCODING_UTF16_MSB      =  2,                                         /* The text encoding was determined to be UTF-16 with the most-significant bit first (big endian). */
    TEXT_ENCODING_UTF16_LSB      =  3,                                         /* The text encoding was determined to be UTF-16 with the least-significant bit first (little endian). */
    TEXT_ENCODING_UTF32_MSB      =  4,                                         /* The text encoding was determined to be UTF-32 with the most-significant bit first (big endian). */
    TEXT_ENCODING_UTF32_LSB      =  5,                                         /* The text encoding was determined to be UTF-32 with the least-significant bit first (little endian). */
} TEXT_ENCODING;

#ifdef __cplusplus
extern "C" {
#endif

/* @summary Retrieve the byte order marker for a given TEXT_ENCODING constant.
 * @param o_marker Pointer to an array of at least four bytes to receive the byte order marker.
 * @param text_encoding One of the values of the TEXT_ENCODING enumeration specifying the encoding for which the byte order marker is being queried.
 * @return The number of bytes in the byte order marker written to the o_marker array.
 */
STRLIB_API(size_t)
ByteOrderMarkerForEncoding
(
    uint8_t *o_marker, 
    int text_encoding
);

/* @summary Given four bytes possibly representing a Unicode byte order marker, attempt to determine the text encoding and size of the byte order marker.
 * @param o_bytecount On return, this location is updated with the number of bytes in the byte order marker.
 * @param marker Pointer to an array of four bytes representing a possible byte order marker. Data less than four bytes should be padded with zeroes.
 * @return One of the values of the TEXT_ENCODING enumeration.
 */
STRLIB_API(int)
EncodingForByteOrderMarker
(
    size_t   *o_bytecount, 
    uint8_t const *marker
);

/* @summary Allocate a buffer for storing UTF-8 encoded characters and optionally initialize the contents with an existing string.
 * @param o_strinfo Pointer to an optional STRING_INFO that if supplied will be initialized with the attributes of the returned string.
 * @param o_bufinfo Pointer to an optional STRING_INFO that if supplied will be initialized with the attributes of the returned buffer.
 * @param strinfo Pointer to an optional STRING_INFO that if supplied contains information about the string pointed to by strbuf.
 * @param max_chars The maximum number of characters that can be stored in the buffer, not including the terminating nul.
 * This parameter can be used to allocate a buffer larger than an existing buffer that is initialized with the contents of the existing buffer.
 * Specify zero for max_chars to allocate a buffer the same size as the supplied initial string strbuf.
 * @param strbuf Pointer to an optional UTF-8 encoded, nul-terminated string that will be used as the initial contents of the new buffer.
 * @return A pointer to the start of the allocated buffer, or NULL if memory allocation failed.
 */
STRLIB_API(char8_t*)
Utf8StringCreate
(
    struct STRING_INFO *o_strinfo, 
    struct STRING_INFO *o_bufinfo, 
    struct STRING_INFO   *strinfo, 
    size_t              max_chars, 
    char8_t const         *strbuf
);

/* @summary Convert an ASCII encoded string to use UTF-8 encoding.
 * @param o_strinfo Pointer to an optional STRING_INFO that if supplied will be initialized with the attributes of the returned string.
 * @param o_bufinfo Pointer to an optional STRING_INFO that if supplied will be initialized with the attributes of the returned buffer.
 * @param max_chars The maximum number of characters that can be stored in the buffer, not including the terminating nul.
 * This parameter can be used to allocate a buffer larger than an existing buffer that is initialized with the contents of the existing buffer.
 * Specify zero for max_chars to allocate a buffer the same size as the supplied initial string strbuf.
 * @param strbuf Pointer to the start of the first codepoint of the nul-terminated, ASCII encoded string to convert.
 * @return A pointer to the buffer containing the nul-terminated, UTF-8 encoded string. Free the returned buffer using the Utf8StringDelete function.
 * The function returns NULL if the conversion fails.
 */
STRLIB_API(char8_t*)
Utf8StringCreateFromAscii
(
    struct STRING_INFO *o_strinfo, 
    struct STRING_INFO *o_bufinfo, 
    size_t              max_chars, 
    char const            *strbuf
);

/* @summary Convert a UTF-16 encoded string to use UTF-8 encoding.
 * @param o_strinfo Pointer to an optional STRING_INFO that if supplied will be initialized with the attributes of the returned string.
 * @param o_bufinfo Pointer to an optional STRING_INFO that if supplied will be initialized with the attributes of the returned buffer.
 * @param max_chars The maximum number of characters that can be stored in the buffer, not including the terminating nul.
 * This parameter can be used to allocate a buffer larger than an existing buffer that is initialized with the contents of the existing buffer.
 * Specify zero for max_chars to allocate a buffer the same size as the supplied initial string strbuf.
 * @param strbuf Pointer to the start of the first codepoint of the nul-terminated, UTF-16 encoded string to convert.
 * @return A pointer to the buffer containing the nul-terminated, UTF-8 encoded string. Free the returned buffer using the Utf8StringDelete function.
 * The function returns NULL if the conversion fails.
 */
STRLIB_API(char8_t*)
Utf8StringCreateFromUtf16
(
    struct STRING_INFO *o_strinfo, 
    struct STRING_INFO *o_bufinfo, 
    size_t              max_chars, 
    char16_t const        *strbuf
);

/* @summary Convert a UTF-32 encoded string to use UTF-8 encoding.
 * @param o_strinfo Pointer to an optional STRING_INFO that if supplied will be initialized with the attributes of the returned string.
 * @param o_bufinfo Pointer to an optional STRING_INFO that if supplied will be initialized with the attributes of the returned buffer.
 * @param max_chars The maximum number of characters that can be stored in the buffer, not including the terminating nul.
 * This parameter can be used to allocate a buffer larger than an existing buffer that is initialized with the contents of the existing buffer.
 * Specify zero for max_chars to allocate a buffer the same size as the supplied initial string strbuf.
 * @param strbuf Pointer to the start of the first codepoint of the nul-terminated, UTF-32 encoded string to convert.
 * @return A pointer to the buffer containing the nul-terminated, UTF-8 encoded string. Free the returned buffer using the Utf8StringDelete function.
 * The function returns NULL if the conversion fails.
 */
STRLIB_API(char8_t*)
Utf8StringCreateFromUtf32
(
    struct STRING_INFO *o_strinfo, 
    struct STRING_INFO *o_bufinfo, 
    size_t              max_chars, 
    char32_t const        *strbuf
);

/* @summary Convert a UTF-8 encoded string to a UTF-16 encoding.
 * @param o_u16info Pointer to an optional STRING_INFO which if supplied, will be updated with information about the UTF-16 string.
 * @param strinfo Pointer to an optional STRING_INFO which if supplied, contains information about the string stored in strbuf.
 * @param strbuf Pointer to the start of the first codepoint to convert. This buffer must be nul-terminated.
 * @return A pointer to the buffer containing the nul-terminated, UTF-16 encoded string. Free the returned buffer using the Utf16StringDelete function.
 * The function returns NULL if the conversion fails.
 */
STRLIB_API(char16_t*)
Utf8StringConvertToUtf16
(
    struct STRING_INFO *o_u16info, 
    struct STRING_INFO   *strinfo, 
    char8_t const         *strbuf
);

/* @summary Convert a UTF-8 encoded string to a UTF-32 encoding.
 * @param o_u32info Pointer to an optional STRING_INFO which if supplied, will be updated with information about the UTF-32 string.
 * @param strinfo Pointer to an optional STRING_INFO which if supplied, contains information about the string stored in strbuf.
 * @param strbuf Pointer to the start of the first codepoint to convert. This buffer must be nul-terminated.
 * @return A pointer to the buffer containing the nul-terminated, UTF-32 encoded string. Free the returned buffer using the Utf32StringDelete function.
 * The function returns NULL if the conversion fails.
 */
STRLIB_API(char32_t*)
Utf8StringConvertToUtf32
(
    struct STRING_INFO *o_u32info, 
    struct STRING_INFO   *strinfo, 
    char8_t const         *strbuf
);

/* @summary Free a string buffer allocated by Utf8StringCreate or any of the Utf8StringCreateFrom* functions.
 * @param strbuf A pointer to a string buffer returned by Utf8StringCreate or any of the Utf8StringCreateFrom* functions.
 */
STRLIB_API(void)
Utf8StringDelete
(
    char8_t *strbuf
);

/* @summary Free a string buffer allocated by Utf8StringConvertToUtf16.
 * @param strbuf A pointer to a string buffer returned by Utf8StringConvertToUtf16.
 */
STRLIB_API(void)
Utf16StringDelete
(
    char16_t *strbuf
);

/* @summary Free a string buffer allocated by Utf8StringConvertToUtf32.
 * @param strbuf A pointer to a string buffer returned by Utf8StringConvertToUtf32.
 */
STRLIB_API(void)
Utf32StringDelete
(
    char32_t *strbuf
);

/* @summary Determine the number of bytes in a UTF-8 encoded string given pointers to the start of the first codepoint and one-past the last codepoint.
 * @param beg A pointer to the start of the first codepoint in the string.
 * @param end A pointer to one-past the end of the final codepoint in the string.
 * @return The number of bytes in the string.
 */
STRLIB_API(size_t)
Utf8StringByteCount
(
    char8_t const *beg, 
    char8_t const *end
);

/* @summary Search a UTF-8 encoded string buffer for the next nul character.
 * @param start A pointer to the first codepoint to examine.
 * @return A pointer to the next nul occurring at or subsequent to start.
 */
STRLIB_API(char8_t*)
Utf8StringFindNul
(
    char8_t const *start
);

/* @summary Retrieve information about a UTF-8 encoded string.
 * @param o_strinfo Pointer to the structure to be updated with information about the string stored in strbuf.
 * @param strbuf Pointer to the start of a codepoint of a UTF-8 encoded, nul-terminated string.
 */
STRLIB_API(void)
Utf8StringInfo
(
    struct STRING_INFO *o_strinfo, 
    char8_t const         *strbuf
);

/* @summary Compare two UTF-8 encoded, nul-terminated strings.
 * @param a A pointer to the start of the first codepoint of a nul-terminated, UTF-8 encoded string.
 * @param b A pointer to the start of the first codepoint of a nul-terminated, UTF-8 encoded string.
 * @return Zero if the strings compare as identical, a negative value if a appears lexographically before b, or a positive value if b appears lexographically before a.
 */
STRLIB_API(int)
Utf8StringCompare
(
    char8_t const *a, 
    char8_t const *b
);

/* @summary Compare two UTF-8 encoded, nul-terminated strings, ignoring case differences.
 * @param a A pointer to the start of the first codepoint of a nul-terminated, UTF-8 encoded string.
 * @param b A pointer to the start of the first codepoint of a nul-terminated, UTF-8 encoded string.
 * @return Zero if the strings compare as identical, a negative value if a appears lexographically before b, or a positive value if b appears lexographically before a.
 */
STRLIB_API(int)
Utf8StringCompareNoCase
(
    char8_t const *a, 
    char8_t const *b
);

/* @summary Given a pointer to the start of a UTF-8 codepoint, return a pointer to the start of the next codepoint.
 * @param o_codepoint On return, this location is updated with the value stored in the codepoint at bufitr.
 * @param o_bytecount On return, this location is updated with the number of bytes required to represent the codepoint stored at bufitr.
 * @param bufitr A pointer to the start of a UTF-8 codepoint.
 * @return A pointer to the start of the next UTF-8 codepoint, or NULL if bufitr is NULL or points to an invalid codepoint.
 */
STRLIB_API(char8_t*)
Utf8StringNextCodepoint
(
    char32_t *o_codepoint, 
    uint32_t *o_bytecount, 
    char8_t const *bufitr
);

/* @summary Given a pointer to the start of a UTF-8 codepoint, return a pointer to the start of the codepoint immediately preceeding it.
 * @param o_codepoint On return, this location is updated with the value stored in the codepoint immediately preceeding bufitr.
 * @param o_bytecount On return, this location is updated with the number of bytes required to represent the codepoint immediately preceeding bufitr.
 * @param bufitr A pointer to the start of a UTF-8 codepoint.
 * @return A pointer to the start of the UTF-8 codepoint immediately preceeding bufitr, or NULL if bufitr is NULL or an invalid codepoint was encountered.
 */
STRLIB_API(char8_t*)
Utf8StringPrevCodepoint
(
    char32_t *o_codepoint, 
    uint32_t *o_bytecount, 
    char8_t const *bufitr
);

/* @summary Copy a single UTF-8 codepoint from one buffer to another.
 * @param o_bytecount On return, this location is updated with the number of bytes in the codepoint pointed to by src.
 * @param o_wordcount On return, this location is updated with the number of char8_t's that were/would be written to the dst buffer.
 * @param dst The location to which the codepoint will be written.
 * @param src A pointer to the start of the UTF-8 codepoint to copy.
 * @return A pointer to the start of the next codepoint in the src buffer, or NULL if src pointed to an invalid codepoint.
 */
STRLIB_API(char8_t*)
Utf8StringCopyCodepoint
(
    uint32_t          *o_bytecount, 
    uint32_t          *o_wordcount, 
    char8_t       * __restrict dst, 
    char8_t const * __restrict src
);

/* @summary Append one UTF-8 encoded string to another.
 * @param o_dstinfo Pointer to an optional STRING_INFO_UTF8 to update with the attributes of the string after the append operation is performed.
 * @param dstinfo Pointer to an optional STRING_INFO_UTF8 containing information about the string stored in dstbuf.
 * @param srcinfo Pointer to an optional STRING_INFO_UTF8 containing information about the string stored in srcbuf.
 * @param max_dst_bytes The size of the buffer starting at address dstbuf, in bytes.
 * @param dstbuf Pointer to the start of the string stored in the destination buffer.
 * @param srcbuf Pointer to the start of the nul-terminated, UTF-8 encoded string to append to the destination buffer.
 * @return Zero if the append operation is successful, or non-zero if an error occurred.
 */
STRLIB_API(int)
Utf8StringAppend
(
    struct STRING_INFO     *o_dstinfo, 
    struct STRING_INFO       *dstinfo, 
    struct STRING_INFO       *srcinfo, 
    size_t              max_dst_bytes,
    char8_t       * __restrict dstbuf, 
    char8_t const * __restrict srcbuf
);

/* @summary Calculate the number of bytes required to store the binary data converted from a base64-encoded string.
 * All base64-encoded data is assumed to appear on a single line.
 * @param b64size The number of bytes of base64-encoded data, including any padding bytes.
 * @return The number of bytes required to store the decoded data.
 */
STRLIB_API(size_t)
BinarySizeForBase64
(
    size_t b64size
);

/* @summary Calculate the number of bytes required to store the binary data converted from a base64-encoded, nul-terminated string.
 * @param encbuf Pointer to a nul-terminated buffer containing the base64-encoded data.
 * @return The number of bytes required to store the decocded data.
 */
STRLIB_API(size_t)
BinarySizeForBase64Data
(
    char const *encbuf
);

/* @summary Calculate the maximum number of bytes required to base64-encode binary data of a given size.
 * All base64-encoded data is assumed to appear on a single line.
 * @param o_padsize On return, this location is updated with the number of padding bytes to be added during encoding.
 * @param binsize The size of the binary data, in bytes.
 * @return The maximum number of bytes required to base64-encode a data block of the given size.
 */
STRLIB_API(size_t)
Base64SizeForBinary
(
    size_t *o_padsize, 
    size_t    binsize
);

/* @summary Base64-encode a block of data.
 * @param o_numdst On return, this location is updated with the number of bytes written to the destination buffer.
 * If dst is NULL and max_dst is zero, the call returns zero and this location is updated with the number of bytes required to base64-encode the input data.
 * @param dst Pointer to the buffer to which the base64-encoded data will be written.
 * @param max_dst The maximum number of bytes that can be written to the destination buffer.
 * @param src Pointer to the buffer containing the source data.
 * @param num_src The number of bytes of data to read from the source buffer.
 * @return Zero if the data is successfully encoded, or non-zero if an error occurred.
 */
STRLIB_API(int)
Base64Encode
(
    size_t     * __restrict o_numdst,
    void       * __restrict      dst, 
    size_t                   max_dst, 
    void const * __restrict      src, 
    size_t                   num_src
);

/* @summary Decode a base64-encoded block of data.
 * @param o_numdst On return, this location is updated with the number of bytes written to the destination buffer.
 * If dst is NULL and max_dst is zero, the call returns zero and this location is updated with the number of bytes required to store the decoded data.
 * @param dst Pointer to the buffer to which the decoded data will be written.
 * @param max_dst The maximum number of bytes that can be written to the destination buffer.
 * @param src Pointer to the buffer containing the base64-encoded data.
 * @param num_src The number of bytes of data to read from the source buffer. Specify zero if the source buffer is nul-terminated and the size should be computed.
 * @return Zero if the data is successfully decoded, or non-zero if an error occurs.
 */
STRLIB_API(int)
Base64Decode
(
    size_t     * __restrict o_numdst, 
    void       * __restrict      dst, 
    size_t                   max_dst, 
    void const * __restrict      src, 
    size_t                   num_src
);

#ifdef __cplusplus
}; /* extern "C" */
#endif

#endif /* __STRLIB_H__ */

