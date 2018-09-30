/**
 * @summary Implement the functions exported by the pathlib.h module for parsing
 * and manipulating filesystem path strings.
 */
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <errno.h>
#include "strlib.h"
#include "pathlib.h"

/* @summary Figure out the starting and ending points of the directory, filename and extension information in a Linux path string.
 * @oaram o_parts The PATH_PARTS_LINUX to update. The Root, RootEnd and PathFlags fields must be initialized by the caller.
 * @param strinfo Information about the input path string.
 */
static int
LinuxPathExtractPathParts
(
    struct PATH_PARTS_LINUX *o_parts,
    struct STRING_INFO      *strinfo
)
{
    char8_t     *iter = strinfo->BufferEnd;
    char8_t *path_end = strinfo->BufferEnd;
    char8_t *extn_beg = strinfo->BufferEnd;
    char8_t *extn_end = strinfo->BufferEnd;
    char8_t *name_beg = o_parts->RootEnd;
    char8_t *name_end = o_parts->RootEnd;
    char8_t *dirs_beg = o_parts->RootEnd;
    char8_t *dirs_end = o_parts->RootEnd;
    uint32_t    flags = o_parts->PathFlags;

    while (name_end < path_end) {
        if (name_end[0] == '\\') { /* normalize to system standard */
            name_end[0] = (char8_t) '/';
        }
        if (name_end[0] != '/') {
            name_end = Utf8StringNextCodepoint(NULL, NULL, name_end);
        } else {
            /* encountered a path separator.
             * update the end of the directory path string.
             * reset the filename string to be zero length.
             */
            dirs_end = name_end;
            name_beg = name_end + 1;
            name_end = name_end + 1;
            flags   |= PATH_FLAG_DIRECTORY;
        }
    }
    if (dirs_beg[0] == '/') {
        /* skip the leading path separator */
        if (dirs_beg == dirs_end) {
            /* there is no actual path component - something like "/" */
            flags &= ~PATH_FLAG_DIRECTORY;
            dirs_end++;
        }
        dirs_beg++;
    }
    if (name_beg != name_end) {
        /* is this a filename or part of the directory path?
         * consider 'a.b' and '.a.b' and 'a.' to be filenames, but not '.a'.
         */
        while (iter >= name_beg) {
            if (*iter == '.' && iter != name_beg) {
                name_end = iter;
                extn_beg = iter + 1;
                flags    = PATH_FLAG_FILENAME | PATH_FLAG_EXTENSION | flags;
            }
            iter = Utf8StringPrevCodepoint(NULL, NULL, iter);
        }
        if ((flags & PATH_FLAG_FILENAME) == 0) {
            dirs_end = name_end;
            flags   |= PATH_FLAG_DIRECTORY;
            name_beg = path_end;
            name_end = path_end;
        }
    } else {
        /* no filename is present */
        name_beg  = path_end;
        name_end  = path_end;
    }
    o_parts->Path         = dirs_beg;
    o_parts->PathEnd      = dirs_end;
    o_parts->Filename     = name_beg;
    o_parts->FilenameEnd  = name_end;
    o_parts->Extension    = extn_beg;
    o_parts->ExtensionEnd = extn_end;
    o_parts->PathFlags    = flags;
    return 0;
}

/* @summary Figure out the starting and ending points of the directory, filename and extension information in a Win32 path string.
 * @oaram o_parts The PATH_PARTS_WIN32 to update. The Root, RootEnd and PathFlags fields must be initialized by the caller.
 * @param strinfo Information about the input path string.
 */
static int
Win32PathExtractPathParts
(
    struct PATH_PARTS_WIN32 *o_parts,
    struct STRING_INFO      *strinfo
)
{
    char8_t     *iter = strinfo->BufferEnd;
    char8_t *path_end = strinfo->BufferEnd;
    char8_t *extn_beg = strinfo->BufferEnd;
    char8_t *extn_end = strinfo->BufferEnd;
    char8_t *name_beg = o_parts->RootEnd;
    char8_t *name_end = o_parts->RootEnd;
    char8_t *dirs_beg = o_parts->RootEnd;
    char8_t *dirs_end = o_parts->RootEnd;
    uint32_t    flags = o_parts->PathFlags;

    while (name_end < path_end) {
        if (name_end[0] == '/') { /* normalize to system standard */
            name_end[0] =  '\\';
        }
        if (name_end[0] != '\\') {
            name_end = Utf8StringNextCodepoint(NULL, NULL, name_end);
        } else {
            /* encountered a path separator.
             * update the end of the directory path string.
             * reset the filename string to be zero length.
             */
            dirs_end = name_end;
            name_beg = name_end + 1;
            name_end = name_end + 1;
            flags   |= PATH_FLAG_DIRECTORY;
        }
    }
    if (dirs_beg[0] == '\\') {
        /* skip the leading path separator */
        if (dirs_beg == dirs_end) {
            /* there is no actual path component - something like "C:\" */
            flags &= ~PATH_FLAG_DIRECTORY;
            dirs_end++;
        }
        dirs_beg++;
    }
    if (name_beg != name_end) {
        /* is this a filename or part of the directory path?
         * consider 'a.b' and '.a.b' and 'a.' to be filenames, but not '.a'.
         */
        while (iter >= name_beg) {
            if (*iter == '.' && iter != name_beg) {
                name_end = iter;
                extn_beg = iter + 1;
                flags    = PATH_FLAG_FILENAME | PATH_FLAG_EXTENSION | flags;
            }
            iter = Utf8StringPrevCodepoint(NULL, NULL, iter);
        }
        if ((flags & PATH_FLAG_FILENAME) == 0) {
            dirs_end = name_end;
            flags   |= PATH_FLAG_DIRECTORY;
            name_beg = path_end;
            name_end = path_end;
        }
    } else {
        /* no filename is present */
        name_beg  = path_end;
        name_end  = path_end;
    }
    o_parts->Path         = dirs_beg;
    o_parts->PathEnd      = dirs_end;
    o_parts->Filename     = name_beg;
    o_parts->FilenameEnd  = name_end;
    o_parts->Extension    = extn_beg;
    o_parts->ExtensionEnd = extn_end;
    o_parts->PathFlags    = flags;
    return 0;
}

PATHLIB_API(size_t)
LinuxPathStringGetMaxChars
(
    void
)
{
    return LINUX_PATH_STRING_MAX_CHARS;
}

PATHLIB_API(size_t)
Win32PathStringGetMaxChars
(
    void
)
{
    return WIN32_PATH_STRING_MAX_CHARS;
}

PATHLIB_API(char8_t*)
LinuxPathBufferCreate
(
    struct STRING_INFO *o_strinfo,
    struct STRING_INFO *o_bufinfo,
    struct STRING_INFO   *strinfo,
    char8_t const         *strbuf
)
{
    return Utf8StringCreate(o_strinfo, o_bufinfo, strinfo, LINUX_PATH_STRING_MAX_CHARS, strbuf);
}

PATHLIB_API(char8_t*)
Win32PathBufferCreate
(
    struct STRING_INFO *o_strinfo,
    struct STRING_INFO *o_bufinfo,
    struct STRING_INFO   *strinfo,
    char8_t const         *strbuf
)
{
    return Utf8StringCreate(o_strinfo, o_bufinfo, strinfo, WIN32_PATH_STRING_MAX_CHARS, strbuf);
}

PATHLIB_API(void)
PathBufferDelete
(
    void *pathbuf
)
{
    free(pathbuf);
}

PATHLIB_API(int)
LinuxPathStringParse
(
    struct PATH_PARTS_LINUX *o_parts,
    struct STRING_INFO    *o_strinfo,
    struct STRING_INFO      *strinfo,
    char8_t const            *strbuf
)
{
    STRING_INFO sinfo;
    char8_t *path_beg = NULL;
    char8_t *path_end = NULL;
    size_t  inp_chars = 0;

    if (o_parts == NULL) {
        assert(o_parts != NULL);
        if (o_strinfo) {
            memset(o_strinfo, 0, sizeof(STRING_INFO));
        } errno = EINVAL;
        return -1;
    }
    if (strbuf != NULL) {
        if (strinfo != NULL) {
            sinfo = *strinfo;
        } else {
            Utf8StringInfo(&sinfo, strbuf);
        }
    } else {
        assert(strbuf != NULL);
        memset(o_parts, 0, sizeof(PATH_PARTS_LINUX));
        o_parts->PathFlags = PATH_FLAG_INVALID;
        if (o_strinfo) {
            memset(o_strinfo, 0, sizeof(STRING_INFO));
        } errno = EINVAL;
        return -1;
    }

    inp_chars          = sinfo.LengthChars;
    path_beg           = sinfo.Buffer;
    path_end           = sinfo.BufferEnd;
    o_parts->Root      = path_beg; o_parts->RootEnd      = path_end;
    o_parts->Path      = path_end; o_parts->PathEnd      = path_end;
    o_parts->Filename  = path_end; o_parts->FilenameEnd  = path_end;
    o_parts->Extension = path_end; o_parts->ExtensionEnd = path_end;
    o_parts->PathFlags = PATH_FLAGS_NONE;
    if (o_strinfo) {
        memcpy(o_strinfo, &sinfo, sizeof(STRING_INFO));
    }

    if (inp_chars >= 1) {
        if (path_beg[0] == '/') {
            o_parts->Root      = path_beg;
            o_parts->RootEnd   = path_beg + 1;
            o_parts->PathFlags = PATH_FLAG_ABSOLUTE | PATH_FLAG_ROOT;
        } else {
            o_parts->Root      = path_beg;
            o_parts->RootEnd   = path_beg;
            o_parts->PathFlags = PATH_FLAG_RELATIVE;
        }
        return LinuxPathExtractPathParts(o_parts, &sinfo);
    } else {
        o_parts->PathFlags = PATH_FLAG_INVALID;
        errno = EINVAL;
        return -1;
    }
}

PATHLIB_API(int)
Win32PathStringParse
(
    struct PATH_PARTS_WIN32 *o_parts,
    struct STRING_INFO    *o_strinfo,
    struct STRING_INFO      *strinfo,
    char8_t const            *strbuf
)
{
    STRING_INFO sinfo;
    char8_t *path_beg = NULL;
    char8_t *path_end = NULL;
    char8_t *root_beg = NULL;
    char8_t *root_end = NULL;
    size_t   inp_chars = 0;
    uint32_t     flags = PATH_FLAGS_NONE;

    if (o_parts == NULL) {
        assert(o_parts != NULL);
        if (o_strinfo) {
            memset(o_strinfo, 0, sizeof(STRING_INFO));
        } errno = EINVAL;
        return -1;
    }
    if (strbuf != NULL) {
        if (strinfo != NULL) {
            sinfo = *strinfo;
        } else {
            Utf8StringInfo(&sinfo, strbuf);
        }
    } else {
        assert(strbuf != NULL);
        memset(o_parts, 0, sizeof(PATH_PARTS_WIN32));
        o_parts->PathFlags = PATH_FLAG_INVALID;
        if (o_strinfo) {
            memset(o_strinfo, 0, sizeof(STRING_INFO));
        } errno = EINVAL;
        return -1;
    }

    inp_chars          = sinfo.LengthChars;
    path_beg           = sinfo.Buffer;
    path_end           = sinfo.BufferEnd;
    o_parts->Root      = path_beg; o_parts->RootEnd      = path_end;
    o_parts->Path      = path_end; o_parts->PathEnd      = path_end;
    o_parts->Filename  = path_end; o_parts->FilenameEnd  = path_end;
    o_parts->Extension = path_end; o_parts->ExtensionEnd = path_end;
    o_parts->PathFlags = PATH_FLAGS_NONE;
    if (o_strinfo) {
        memcpy(o_strinfo, &sinfo, sizeof(STRING_INFO));
    }

    if (inp_chars >= 3) {
        if (path_beg[0] == '\\' && path_beg[1] == '\\') {
            /* absolute path; may be device, UNC, long device, long UNC or long DOS */
            if ((inp_chars >= 5) && (path_beg[2] == '?') && (path_beg[3] == '\\')) {
                /* may be long UNC or long DOS */
                if ((inp_chars >= 6) && ((path_beg[4] >= 'A' && path_beg[4] <= 'Z') || (path_beg[4] >= 'a' && path_beg[4] <= 'z')) && (path_beg[5] == ':')) {
                    /* long DOS path */
                    o_parts->Root      = path_beg + 4;
                    o_parts->RootEnd   = path_beg + 6;
                    o_parts->PathFlags = PATH_FLAG_ABSOLUTE | PATH_FLAG_LONG | PATH_FLAG_ROOT;
                    return Win32PathExtractPathParts(o_parts, &sinfo);
                } else if ((inp_chars >= 6) && (path_beg[4] == '.' && path_beg[5] == '\\')) {
                    /* long device path */
                    root_beg = path_beg + 6;
                    root_end = path_beg + 6;
                    flags    = PATH_FLAG_ABSOLUTE | PATH_FLAG_LONG | PATH_FLAG_DEVICE | PATH_FLAG_ROOT;
                    goto scan_for_end_of_root;
                } else {
                    /* long UNC path */
                    root_beg = path_beg + 4;
                    root_end = path_beg + 4;
                    flags    = PATH_FLAG_ABSOLUTE | PATH_FLAG_LONG | PATH_FLAG_NETWORK | PATH_FLAG_ROOT;
                    goto scan_for_end_of_root;
                }
            } else if ((inp_chars >= 5) && (path_beg[2] == '.') && (path_beg[3] == '\\')) {
                /* device path, limit MAX_PATH characters */
                root_beg = path_beg + 4;
                root_end = path_beg + 4;
                flags    = PATH_FLAG_ABSOLUTE | PATH_FLAG_DEVICE | PATH_FLAG_ROOT;
                goto scan_for_end_of_root;
            } else {
                /* UNC path, limit MAX_PATH characters */
                root_beg = path_beg + 2;
                root_end = path_beg + 2;
                flags    = PATH_FLAG_ABSOLUTE | PATH_FLAG_NETWORK | PATH_FLAG_ROOT;
                goto scan_for_end_of_root;
            }
        } else if (path_beg[0] == '\\' || path_beg[0] == '/') {
            /* absolute path with a root of '\' (MSDN says this is valid) */
            if (path_beg[0] == '/') {
                path_beg[0] = '\\';
            }
            o_parts->Root      = path_beg;
            o_parts->RootEnd   = path_beg + 1;
            o_parts->PathFlags = PATH_FLAG_ABSOLUTE | PATH_FLAG_ROOT;
            return Win32PathExtractPathParts(o_parts, &sinfo);
        } else if (((path_beg[0] >= 'A' && path_beg[0] <= 'Z') || (path_beg[0] >= 'a' && path_beg[0] <= 'z')) && (path_beg[1] == ':')) {
            /* absolute DOS path with a drive letter root */
            o_parts->Root      = path_beg;
            o_parts->RootEnd   = path_beg + 2;
            o_parts->PathFlags = PATH_FLAG_ABSOLUTE | PATH_FLAG_ROOT;
            return Win32PathExtractPathParts(o_parts, &sinfo);
        } else {
            /* assume this is a relative path */
            o_parts->Root      = path_beg;
            o_parts->RootEnd   = path_beg;
            o_parts->PathFlags = PATH_FLAG_RELATIVE;
            return Win32PathExtractPathParts(o_parts, &sinfo);
        }
    } else if (inp_chars == 2) {
        /* C:, .., .\, aa, .a, etc. */
        if (((path_beg[0] >= 'A' && path_beg[0] <= 'Z') || (path_beg[0] >= 'a' && path_beg[0] <= 'z')) && (path_beg[1] == ':')) {
            /* absolute DOS path with drive letter root; no path information */
            o_parts->Root      = path_beg;
            o_parts->RootEnd   = path_beg + 2;
            o_parts->PathFlags = PATH_FLAG_ABSOLUTE | PATH_FLAG_ROOT;
            return 0;
        } else {
            /* assume a relative path, directory info only */
            o_parts->Root      = path_beg;
            o_parts->RootEnd   = path_beg;
            o_parts->Path      = path_beg;
            o_parts->PathFlags = PATH_FLAG_RELATIVE | PATH_FLAG_DIRECTORY;
            if (path_beg[0] == '.' && (path_beg[1] == '\\' || path_beg[1] == '/')) {
                /*relative path, directory info only */
                if (path_beg[1] == '/') {
                    path_beg[1] = '\\';
                }
                o_parts->PathEnd = path_beg + 1;
            } else {
                /* assume this is a relative directory path */
                o_parts->PathEnd = path_beg + 2;
            }
            return 0;
        }
    } else {
        /* /, ., a, etc. - just a single character */
        if (path_beg[0] == '/') {
            path_beg[0] = '\\';
        }
        if (path_beg[0] == '\\') {
            /* treat this as an absolute path, the root of the filesystem */
            o_parts->Root      = path_beg;
            o_parts->RootEnd   = path_beg;
            o_parts->Path      = path_beg;
            o_parts->PathEnd   = path_beg + 1;
            o_parts->PathFlags = PATH_FLAG_ABSOLUTE | PATH_FLAG_DIRECTORY;
        } else {
            /* assume this is a relative path, directory info only */
            o_parts->Root      = path_beg;
            o_parts->RootEnd   = path_beg;
            o_parts->Path      = path_beg;
            o_parts->PathEnd   = path_beg + 1;
            o_parts->PathFlags = PATH_FLAG_RELATIVE | PATH_FLAG_DIRECTORY;
        }
        return 0;
    }

scan_for_end_of_root:
    while (root_end < path_end) {
        if (root_end[0] == '\\') {
            break;
        }
        if (root_end[0] == '/') {
            root_end[0]  = '\\';
            break;
        }
        root_end = Utf8StringNextCodepoint(NULL, NULL, root_end);
    }
    if (root_end == path_end) {
        /* no additional components will be found */
        return 0;
    }
    o_parts->Root      = root_beg;
    o_parts->RootEnd   = root_end;
    o_parts->PathFlags = flags;
    return Win32PathExtractPathParts(o_parts, &sinfo);
}

#if 0
PATHLIB_API(char8_t*)
LinuxPathBufferAppend
(
    struct STRING_INFO_UTF8 *o_dstinfo, 
    struct STRING_INFO_UTF8   *dstinfo, 
    struct STRING_INFO_UTF8   *appinfo, 
    char8_t                    *dstbuf, 
    char8_t const              *appstr
);

PATHLIB_API(char16_t*)
Win32PathBufferAppend
(
    struct STRING_INFO_UTF16 *o_dstinfo, 
    struct STRING_INFO_UTF16   *dstinfo, 
    struct STRING_INFO_UTF16   *appinfo, 
    char16_t                    *dstbuf, 
    char16_t const              *appstr
)
{
    STRING_INFO_UTF16 dsinfo;
    STRING_INFO_UTF16 ssinfo;
    
    memset(&dsinfo, 0, sizeof(STRING_INFO_UTF16));
    memset(&ssinfo, 0, sizeof(STRING_INFO_UTF16));

    if (appstr != NULL) {
        if (appinfo != NULL) {
            memcpy(&ssinfo, appinfo, sizeof(STRING_INFO_UTF16));
        } else {
            Utf16StringInfo(&ssinfo, appstr);
        }
    }
    if (dstbuf != NULL) {
        if (dstinfo != NULL) {
            memcpy(&dsinfo, dstinfo, sizeof(STRING_INFO_UTF16));
        } else {
            Utf16StringInfo(&dsinfo, dstbuf);
        }
    }
    
    if (ssinfo.LengthChars == 0) { /* nothing to append */
        if (o_dstinfo) {
            memcpy(o_dstinfo, &dsinfo, sizeof(STRING_INFO_UTF16));
        } return dstbuf;
    }
    if (dsinfo.LengthChars + ssinfo.LengthChars > WIN32_PATH_STRING_MAX_CHARS) {
        if (o_dstinfo) {
            memcpy(o_dstinfo, &dsinfo, sizeof(STRING_INFO_UTF16));
        } errno = ENAMETOOLONG;
        return NULL;
    }

    if (dstbuf == NULL) {
        if ((dstbuf = Win32PathBufferCreate(&dsinfo, NULL, appinfo, appstr)) != NULL) {
            while (dstbuf && *dstbuf) {
                if (*dstbuf == '/') {
                    *dstbuf = '\\';
                } dstbuf = Utf16StringNextCodepoint(NULL, NULL, dstbuf);
            }
            if (o_dstinfo) {
                memcpy(o_dstinfo, &dsinfo, sizeof(STRING_INFO_UTF16));
            } return dsinfo.Buffer;
        } else {
            if (o_dstinfo) {
                memset(o_dstinfo, 0, sizeof(STRING_INFO_UTF16));
            } return NULL; /* allocation failed */
        }
    }

    if (dsinfo.LengthChars > 0 && dsinfo.BufferEnd[-2] != '\\') {
        if (dsinfo.LengthChars + ssinfo.LengthChars + 1 > WIN32_PATH_STRING_MAX_CHARS) {
            if (o_dstinfo) {
                memcpy(o_dstinfo, &dsinfo, sizeof(STRING_INFO_UTF16));
            } errno = ENAMETOOLONG;
            return NULL;
        }
        dsinfo.BufferEnd[-1] = '\\';
    }
    while (ssinfo.Buffer != ssinfo.BufferEnd) {
        if (ssinfo.Buffer[0] != '/') {
            dsinfo.BufferEnd[-1] = ssinfo.Buffer++;
        } else {
        }
    }
}
#endif

