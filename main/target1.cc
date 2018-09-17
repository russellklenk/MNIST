#include <stddef.h>
#include <stdint.h>
#include <locale.h>
#include <stdio.h>

#include "strlib.h"
#include "pathlib.h"

#ifdef _WIN32
#define END_OF_LINE    "\r\n"
#else
#define END_OF_LINE    "\n"
#endif

static void
PrintStringRange_u8
(
    char    const *title, 
    char8_t const   *beg, 
    char8_t const   *end
)
{
    printf("%s", title);
    while (beg != end) {
        printf("%c", *beg++);
    }
    printf(END_OF_LINE);
}

static void
PrintStringRange_u16
(
    char     const *title, 
    char16_t const   *beg, 
    char16_t const   *end
)
{
    printf("%s", title);
    while (beg != end) {
        printf("%lc", *beg++);
    }
    printf(END_OF_LINE);
}

static void
PrintPathFlags
(
    char const *title, 
    uint32_t    flags
)
{
    printf("%s", title);
    if (flags == PATH_FLAGS_NONE) {
        printf("NONE");
    }
    if (flags & PATH_FLAG_INVALID) {
        printf("INVALID");
    }
    if (flags & PATH_FLAG_ABSOLUTE) {
        if (flags > PATH_FLAG_ABSOLUTE) {
            printf("ABSOLUTE | ");
        } else {
            printf("ABSOLUTE");
        }
    }
    if (flags & PATH_FLAG_RELATIVE) {
        if (flags > PATH_FLAG_RELATIVE) {
            printf("RELATIVE | ");
        } else {
            printf("RELATIVE");
        }
    }
    if (flags & PATH_FLAG_NETWORK) {
        if (flags > PATH_FLAG_NETWORK) {
            printf("NETWORK | ");
        } else {
            printf("NETWORK");
        }
    }
    if (flags & PATH_FLAG_DEVICE) {
        if (flags > PATH_FLAG_DEVICE) {
            printf("DEVICE | ");
        } else {
            printf("DEVICE");
        }
    }
    if (flags & PATH_FLAG_LONG) {
        if (flags > PATH_FLAG_LONG) {
            printf("LONG | ");
        } else {
            printf("LONG");
        }
    }
    if (flags & PATH_FLAG_ROOT) {
        if (flags > PATH_FLAG_ROOT) {
            printf("ROOT | ");
        } else {
            printf("ROOT");
        }
    }
    if (flags & PATH_FLAG_DIRECTORY) {
        if (flags > PATH_FLAG_DIRECTORY) {
            printf("DIRECTORY | ");
        } else {
            printf("DIRECTORY");
        }
    }
    if (flags & PATH_FLAG_FILENAME) {
        if (flags > PATH_FLAG_FILENAME) {
            printf("FILENAME | ");
        } else {
            printf("FILENAME");
        }
    }
    if (flags & PATH_FLAG_EXTENSION) {
        if (flags > PATH_FLAG_EXTENSION) {
            printf("EXTENSION | ");
        } else {
            printf("EXTENSION");
        }
    }
    printf(END_OF_LINE);
}

static void
PrintLinuxPathParts
(
    struct PATH_PARTS_LINUX *parts, 
    char8_t const             *str
)
{
    printf("Input String: %s" END_OF_LINE, str);
    PrintStringRange_u8("Root     :", parts->Root, parts->RootEnd);
    PrintStringRange_u8("Directory:", parts->Path, parts->PathEnd);
    PrintStringRange_u8("Filename :", parts->Filename, parts->FilenameEnd);
    PrintStringRange_u8("Extension:", parts->Extension, parts->ExtensionEnd);
    PrintPathFlags     ("Flags    :", parts->PathFlags);
    printf(END_OF_LINE);
}

static void
PrintWin32PathParts
(
    struct PATH_PARTS_WIN32 *parts, 
    char16_t const            *str
)
{
    (void) sizeof(str);
    PrintStringRange_u16("Root     :", parts->Root, parts->RootEnd);
    PrintStringRange_u16("Directory:", parts->Path, parts->PathEnd);
    PrintStringRange_u16("Filename :", parts->Filename, parts->FilenameEnd);
    PrintStringRange_u16("Extension:", parts->Extension, parts->ExtensionEnd);
    PrintPathFlags      ("Flags    :", parts->PathFlags);
    printf(END_OF_LINE);
}

int main
(
    int    argc, 
    char **argv
)
{
    STRING_INFO_UTF16 w_info;
    STRING_INFO_UTF8  l_info;
    PATH_PARTS_LINUX  l_part;
    PATH_PARTS_WIN32  w_part;
    char8_t  const    *path1 = u8"/abs/path/to/file.ext";
    char8_t  const    *path2 = u8"rel/path/to/file.ext";
    char8_t  const    *path3 = u8".filename";
    char8_t  const    *path4 = u8"file.ext";
    char16_t const    *path5 = u"C:\\dos\\path\\to\\file.ext";
    char16_t const    *path6 = u"\\\\?\\C:\\long\\unc\\path\\to\\file.ext";

    setlocale(LC_ALL, "en_US.utf8"); /* locale -a */

    (void) sizeof(argc);
    (void) sizeof(argv);
    printf("Hello, world!\n");

    (void) LinuxPathStringParse(&l_part, &l_info, NULL, path1);
    PrintLinuxPathParts(&l_part, path1);
    (void) LinuxPathStringParse(&l_part, &l_info, NULL, path2);
    PrintLinuxPathParts(&l_part, path2);
    (void) LinuxPathStringParse(&l_part, &l_info, NULL, path3);
    PrintLinuxPathParts(&l_part, path3);
    (void) LinuxPathStringParse(&l_part, &l_info, NULL, path4);
    PrintLinuxPathParts(&l_part, path4);
    (void) Win32PathStringParse(&w_part, &w_info, NULL, path5);
    PrintWin32PathParts(&w_part, path5);
    (void) Win32PathStringParse(&w_part, &w_info, NULL, path6);
    PrintWin32PathParts(&w_part, path6);
    goto cleanup_and_exit;

cleanup_and_exit:
    return 0;
}

