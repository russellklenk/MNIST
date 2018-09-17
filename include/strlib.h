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

/* @summary Define a structure for storing commonly-needed data about a UTF-8 encoded string.
 */
typedef struct STRING_INFO_UTF8 {
    char8_t                     *Buffer;                                       /* Points to the start of the string buffer. */
    char8_t                     *BufferEnd;                                    /* Points to one byte past the terminating nul byte of the string buffer. */
    size_t                       LengthBytes;                                  /* The length of the string buffer, in bytes, including the terminating nul. */
    size_t                       LengthChars;                                  /* The length of the string buffer, in characters, not including the terminating nul. */
} STRING_INFO_UTF8;

/* @summary Define a structure for storing commonly-needed data about a UTF-16 encoded string.
 */
typedef struct STRING_INFO_UTF16 {
    char16_t                    *Buffer;                                       /* Points to the start of the string buffer. */
    char16_t                    *BufferEnd;                                    /* Points to one byte past the terminating nul byte of the string buffer. */
    size_t                       LengthBytes;                                  /* The length of the string buffer, in bytes, including the terminating nul. */
    size_t                       LengthChars;                                  /* The length of the string buffer, in characters, not including the terminating nul. */
} STRING_INFO_UTF16;

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
 * @param o_strinfo Pointer to an optional STRING_INFO_UTF8 that if supplied will be initialized with the attributes of the returned string.
 * @param o_bufinfo Pointer to an optional STRING_INFO_UTF8 that if supplied will be initialized with the attributes of the returned buffer.
 * @param strinfo Pointer to an optional STRING_INFO_UTF8 that if supplied contains information about the string pointed to by strbuf.
 * @param max_chars The maximum number of characters that can be stored in the buffer, not including the terminating nul.
 * This parameter can be used to allocate a buffer larger than an existing buffer that is initialized with the contents of the existing buffer.
 * Specify zero for max_chars to allocate a buffer the same size as the supplied initial string strbuf.
 * @param strbuf Pointer to an optional UTF-8 encoded, nul-terminated string that will be used as the initial contents of the new buffer.
 * @return A pointer to the start of the allocated buffer, or NULL if memory allocation failed.
 */
STRLIB_API(char8_t*)
Utf8StringCreate
(
    struct STRING_INFO_UTF8 *o_strinfo, 
    struct STRING_INFO_UTF8 *o_bufinfo, 
    struct STRING_INFO_UTF8   *strinfo, 
    size_t                   max_chars, 
    char8_t const              *strbuf
);

/* @summary Free a string buffer allocated by Utf8StringCreate.
 * @param strbuf A pointer to a string buffer returned by Utf8StringCreate.
 */
STRLIB_API(void)
Utf8StringDelete
(
    char8_t *strbuf
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
    struct STRING_INFO_UTF8 *o_strinfo, 
    char8_t const              *strbuf
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
    char32_t   *o_codepoint, 
    uint32_t   *o_bytecount, 
    char8_t const   *bufitr
);

/* @summary Allocate a buffer for storing UTF-16 encoded characters and optionally initialize the contents with an existing string.
 * @param o_strinfo Pointer to an optional STRING_INFO_UTF16 that if supplied will be initialized with the attributes of the returned string.
 * @param o_bufinfo Pointer to an optional STRING_INFO_UTF16 that if supplied will be initialized with the attributes of the returned buffer.
 * @param strinfo Pointer to an optional STRING_INFO_UTF816 that if supplied contains information about the string pointed to by strbuf.
 * @param max_chars The maximum number of characters that can be stored in the buffer, not including the terminating nul.
 * This parameter can be used to allocate a buffer larger than an existing buffer that is initialized with the contents of the existing buffer.
 * Specify zero for max_chars to allocate a buffer the same size as the supplied initial string strbuf.
 * @param strbuf Pointer to an optional UTF-16 encoded, nul-terminated string that will be used as the initial contents of the new buffer.
 * @return A pointer to the start of the allocated buffer, or NULL if memory allocation failed.
 */
STRLIB_API(char16_t*)
Utf16StringCreate
(
    struct STRING_INFO_UTF16 *o_strinfo, 
    struct STRING_INFO_UTF16 *o_bufinfo, 
    struct STRING_INFO_UTF16   *strinfo, 
    size_t                    max_chars, 
    char16_t const              *strbuf
);

/* @summary Free a string buffer allocated by Utf16StringCreate.
 * @param strbuf A pointer to a string buffer returned by Utf16StringCreate.
 */
STRLIB_API(void)
Utf16StringDelete
(
    char16_t *strbuf
);

/* @summary Search a UTF-16 encoded string buffer for the next nul character.
 * @param start A pointer to the first codepoint to examine.
 * @return A pointer to the next nul occurring at or subsequent to start.
 */
STRLIB_API(char16_t*)
Utf16StringFindNul
(
    char16_t const *start
);

/* @summary Retrieve information about a UTF-16 encoded string.
 * @param o_strinfo Pointer to the structure to be updated with information about the string stored in strbuf.
 * @param strbuf Pointer to the start of a codepoint of a UTF-16 encoded, nul-terminated string.
 */
STRLIB_API(void)
Utf16StringInfo
(
    struct STRING_INFO_UTF16 *o_strinfo, 
    char16_t const              *strbuf
);

/* @summary Given a pointer to the start of a UTF-16 codepoint, return a pointer to the start of the next codepoint.
 * @param o_codepoint On return, this location is updated with the value stored in the codepoint at bufitr.
 * @param o_bytecount On return, this location is updated with the number of bytes required to represent the codepoint stored at bufitr.
 * @param bufitr A pointer to the start of a UTF-16 codepoint.
 * @return A pointer to the start of the next UTF-16 codepoint, or NULL if bufitr is NULL or points to an invalid codepoint.
 */
STRLIB_API(char16_t*)
Utf16StringNextCodepoint
(
    char32_t   *o_codepoint, 
    uint32_t   *o_bytecount, 
    char16_t const  *bufitr
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

