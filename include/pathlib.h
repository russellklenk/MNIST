/**
 * @summary Defines types and functions for working with filesystem path strings.
 */
#ifndef __PATHLIB_H__
#define __PATHLIB_H__

#pragma once

#ifndef PATHLIB_NO_INCLUDES
#include <stddef.h>
#include <stdint.h>
#include "strlib.h"
#endif

#ifndef PATHLIB_API
#ifdef  PATHLIB_STATIC
#define PATHLIB_API(_return_type)                                              \
    static _return_type
#else
#define PATHLIB_API(_return_type)                                              \
    extern _return_type
#endif /* PATHLIB_STATIC */
#endif /* PATHLIB_API */

/* @summary Define the data used to access the path components of a Linux-style path string.
 * Linux-style paths are specified using UTF-8 encoding and are nul-terminated.
 */
typedef struct PATH_PARTS_LINUX {
    char8_t                     *Root;                                         /* A pointer to the first character of the path root. */
    char8_t                     *RootEnd;                                      /* A pointer to one-past the last character of the path root component. */
    char8_t                     *Path;                                         /* A pointer to the first character of the directory tree component. */
    char8_t                     *PathEnd;                                      /* A pointer to one-past the last character of the directory tree component. */
    char8_t                     *Filename;                                     /* A pointer to the first character of the filename component. */
    char8_t                     *FilenameEnd;                                  /* A pointer to one-past the last character of the filename component. */
    char8_t                     *Extension;                                    /* A pointer to the first character of the file extension component. */
    char8_t                     *ExtensionEnd;                                 /* A pointer to one-past the last character of the file extension component. */
    uint32_t                     PathFlags;                                    /* One or more bitwise-OR'd values of the PATH_FLAGS enumeration. */
} PATH_PARTS_LINUX;

/* @summary Define the data used to access the path components of a Windows-style path string.
 * The path string may specify a UNC path or a device path in addition to a file path.
 */
typedef struct PATH_PARTS_WIN32 {
    char16_t                    *Root;                                         /* A pointer to the first character of the path root. */
    char16_t                    *RootEnd;                                      /* A pointer to one-past the last character of the path root component. */
    char16_t                    *Path;                                         /* A pointer to the first character of the directory tree component. */
    char16_t                    *PathEnd;                                      /* A pointer to one-past the last character of the directory tree component. */
    char16_t                    *Filename;                                     /* A pointer to the first character of the filename component. */
    char16_t                    *FilenameEnd;                                  /* A pointer to one-past the last character of the filename component. */
    char16_t                    *Extension;                                    /* A pointer to the first character of the file extension component. */
    char16_t                    *ExtensionEnd;                                 /* A pointer to one-past the last character of the file extension component. */
    uint32_t                     PathFlags;                                    /* One or more bitwise-OR'd values of the PATH_FLAGS enumeration. */
} PATH_PARTS_WIN32;

/* @summary Define a set of flags that can be bitwise-OR'd together to specify attributes of a path string.
 */
typedef enum PATH_FLAGS {
    PATH_FLAGS_NONE             = (0UL <<  0),                                 /* The path string does not have any components. */
    PATH_FLAG_INVALID           = (1UL <<  0),                                 /* The path string does not specify a valid path. */
    PATH_FLAG_ABSOLUTE          = (1UL <<  1),                                 /* The path string specifies an absolute path. */
    PATH_FLAG_RELATIVE          = (1UL <<  2),                                 /* The path string specifies a relative path. */
    PATH_FLAG_NETWORK           = (1UL <<  3),                                 /* The path string is a Win32 UNC share path. */
    PATH_FLAG_DEVICE            = (1UL <<  4),                                 /* The path string is a Win32 device path. */
    PATH_FLAG_LONG              = (1UL <<  5),                                 /* The path string is a Win32 long path (starts with "\\?\"). */
    PATH_FLAG_ROOT              = (1UL <<  6),                                 /* The path string has a root or drive letter component. */
    PATH_FLAG_DIRECTORY         = (1UL <<  7),                                 /* The path string has a directory tree component. */
    PATH_FLAG_FILENAME          = (1UL <<  8),                                 /* The path string has a filename component. */
    PATH_FLAG_EXTENSION         = (1UL <<  9),                                 /* The path string has a file extension component. */
} PATH_FLAGS;

// Parse a path into a PATH_PARTS
// Append one path fragment to another
// Change the existing file extension in-place
// Append a file extension

#ifdef __cplusplus
extern "C" {
#endif

/* @summary Retrieve the maximum number of characters in a Linux-style path string.
 * @return The maximum number of characters in a Linux-style path string, not including the terminating nul.
 */
PATHLIB_API(size_t)
LinuxPathStringGetMaxChars
(
    void
);

/* @summary Retrieve the maximum number of characters in a Win32-style path string.
 * @return The maximum number of characters in a Win32-style path string, not including the terminating nul.
 */
PATHLIB_API(size_t)
Win32PathStringGetMaxChars
(
    void
);

/* @summary Allocate a buffer for manipulating a Linux-style path string and optionally initialize the contents with an existing string.
 * The buffer can hold up to the number of characters returned by the LinuxPathStringGetMaxChars function, plus one for the terminating nul.
 * @param o_strinfo Pointer to an optional STRING_INFO_UTF8 that if supplied will be initialized with the attributes of the returned string.
 * @param o_bufinfo Pointer to an optional STRING_INFO_UTF8 that if supplied will be initialized with the attributes of the returned buffer.
 * @param strinfo Pointer to an optional STRING_INFO_UTF8 that if supplied contains information about the string pointed to by strbuf.
 * @param strbuf Pointer to an optional UTF-8 encoded, nul-terminated string that will be used as the initial contents of the new buffer.
 * @return A pointer to the start of the allocated buffer, or NULL if memory allocation failed.
 */
PATHLIB_API(char8_t*)
LinuxPathBufferCreate
(
    struct STRING_INFO_UTF8 *o_strinfo, 
    struct STRING_INFO_UTF8 *o_bufinfo, 
    struct STRING_INFO_UTF8   *strinfo, 
    char8_t const              *strbuf
);

/* @summary Allocate a buffer for manipulating a Win32-style path string and optionally initialize the contents with an existing string.
 * The buffer can hold up to the number of characters returned by the Win32PathStringGetMaxChars function, plus one for the terminating nul.
 * @param o_strinfo Pointer to an optional STRING_INFO_UTF16 that if supplied will be initialized with the attributes of the returned string.
 * @param o_bufinfo Pointer to an optional STRING_INFO_UTF16 that if supplied will be initialized with the attributes of the returned buffer.
 * @param strinfo Pointer to an optional STRING_INFO_UTF16 that if supplied contains information about the string pointed to by strbuf.
 * @param strbuf Pointer to an optional UTF-16 encoded, nul-terminated string that will be used as the initial contents of the new buffer.
 * @return A pointer to the start of the allocated buffer, or NULL if memory allocation failed.
 */
PATHLIB_API(char16_t*)
Win32PathBufferCreate
(
    struct STRING_INFO_UTF16 *o_strinfo, 
    struct STRING_INFO_UTF16 *o_bufinfo, 
    struct STRING_INFO_UTF16   *strinfo, 
    char16_t const              *strbuf
);

/* @summary Free a path buffer returned by the LinuxPathBufferCreate or Win32PathBufferCreate functions.
 * @param pathbuf The path buffer to free.
 */
PATHLIB_API(void)
PathBufferDelete
(
    void *pathbuf
);

/* @summary Parse a Linux-style path string into its constituient parts.
 * @param o_parts The PATH_PARTS_LINUX structure to populate.
 * @oaran o_strinfo Pointer to an optional STRING_INFO_UTF8 that if supplied will be initialized with the attributes of the input string.
 * @param strinfo Optional information about the input string strbuf that, if supplied, is used as an optimization.
 * @param strbuf A pointer to the start of the nul-terminated, UTF-8 encoded path string to parse.
 * @return Zero if the path string is parsed successfully, or non-zero if an error occurred.
 */
PATHLIB_API(int)
LinuxPathStringParse
(
    struct PATH_PARTS_LINUX   *o_parts,
    struct STRING_INFO_UTF8 *o_strinfo, 
    struct STRING_INFO_UTF8   *strinfo, 
    char8_t const              *strbuf
);

/* @summary Parse a Win32-style path string into its constituient parts.
 * @param o_parts The PATH_PARTS_WIN32 structure to populate.
 * @oaran o_strinfo Pointer to an optional STRING_INFO_UTF16 that if supplied will be initialized with the attributes of the input string.
 * @param strinfo Optional information about the input string strbuf that, if supplied, is used as an optimization.
 * @param strbuf A pointer to the start of the nul-terminated, UTF-16 encoded path string to parse.
 * @return Zero if the path string is parsed successfully, or non-zero if an error occurred.
 */
PATHLIB_API(int)
Win32PathStringParse
(
    struct PATH_PARTS_WIN32    *o_parts, 
    struct STRING_INFO_UTF16 *o_strinfo, 
    struct STRING_INFO_UTF16   *strinfo, 
    char16_t const              *strbuf
);

/* @summary Append one path fragment to another.
 * @param o_dstinfo Pointer to an optional STRING_INFO_UTF8 that if supplied will be initialized with the attributes of the destination string buffer after the path fragment is appended.
 * @param dstinfo Optional information about the destination string buffer dstbuf that, if supplied, is used as an optimization.
 * @param appinfo Optional information about the path fragment to append that, if supplied, is used as an optimization.
 * @param dstbuf The buffer, returned by LinuxPathBufferCreate, to which the path fragment will be appended. If this value is NULL, a new path buffer is allocated and initialized with the contents of appstr.
 * @param appstr The path fragment to append to the path fragment stored in dstbuf.
 * @return A pointer to the destination path buffer (or the allocated path buffer, if dstbuf is NULL), or NULL if an error occurred.
 */
PATHLIB_API(char8_t*)
LinuxPathBufferAppend
(
    struct STRING_INFO_UTF8 *o_dstinfo, 
    struct STRING_INFO_UTF8   *dstinfo, 
    struct STRING_INFO_UTF8   *appinfo, 
    char8_t                    *dstbuf, 
    char8_t const              *appstr
);

/* @summary Append one path fragment to another.
 * @param o_dstinfo Pointer to an optional STRING_INFO_UTF16 that if supplied will be initialized with the attributes of the destination string buffer after the path fragment is appended.
 * @param dstinfo Optional information about the destination string buffer dstbuf that, if supplied, is used as an optimization.
 * @param appinfo Optional information about the path fragment to append that, if supplied, is used as an optimization.
 * @param dstbuf The buffer, returned by Win32PathBufferCreate, to which the path fragment will be appended. If this value is NULL, a new path buffer is allocated and initialized with the contents of appstr.
 * @param appstr The path fragment to append to the path fragment stored in dstbuf.
 * @return A pointer to the destination path buffer (or the allocated path buffer, if dstbuf is NULL), or NULL if an error occurred.
 */
PATHLIB_API(char16_t*)
Win32PathBufferAppend
(
    struct STRING_INFO_UTF16 *o_dstinfo, 
    struct STRING_INFO_UTF16   *dstinfo, 
    struct STRING_INFO_UTF16   *appinfo, 
    char16_t                    *dstbuf, 
    char16_t const              *appstr
);

#ifdef __cplusplus
}; /* extern "C" */
#endif

#endif /* __PATHLIB_H__ */

