/**
 * data.h: Defines the default data paths and filenames for the test data, and 
 * provides functions for returning the absolute paths to data files given the 
 * relative path portion.
 */
#ifndef __DATA_H__
#define __DATA_H__

#pragma once

#ifndef MNIST_API
#   ifdef  MNIST_STATIC
#       define MNIST_API(_return_type)                                         \
            static _return_type
#else
#       define MNIST_API(_return_type)                                         \
            extern _return_type
#   endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* @summary Retrieve the maximum number of characters that can appear in a path string on the host.
 * @return The maximum number of characters the host operating system can process in a path string.
 */
MNIST_API(size_t)
GetMaxSystemPathChars
(
    void
);

/* @summary Allocate a buffer for storing path strings.
 * @param max_chars The maximum number of characters that can be stored in the path buffer, not including the terminating nul.
 * @return A pointer to the start of the path buffer.
 */
MNIST_API(char*)
PathBufferCreate
(
    size_t max_chars
);

/* @summary Free a path buffer.
 * @param buf The path buffer to free.
 */
MNIST_API(void)
PathBufferDelete
(
    char *buf
);

/* @summary Retrieve the relative path to the default training set image data.
 * @param len_chars Specify the address of a value to store the length of the returned path string, in characters, not including the terminating nul.
 * @param len_bytes Specify the address of a value to store the length of the returned path string, in bytes, including the trailing nul.
 * @return A nul-terminated string specifying the relative path to the default training set image data.
 * Free the returned path string using the PathBufferDelete function.
 */
MNIST_API(char*)
TrainingSetImages_GetDefaultRelativePath
(
    size_t *len_chars, 
    size_t *len_bytes
);

/* @summary Retrieve the relative path to the default training set label data.
 * @param len_chars Specify the address of a value to store the length of the returned path string, in characters, not including the terminating nul.
 * @param len_bytes Specify the address of a value to store the length of the returned path string, in bytes, including the trailing nul.
 * @return A nul-terminated string specifying the relative path to the default training set label data.
 * Free the returned path string using the PathBufferDelete function.
 */
MNIST_API(char*)
TrainingSetLabels_GetDefaultRelativePath
(
    size_t *len_chars, 
    size_t *len_bytes
);

/* @summary Retrieve the relative path to the default test set image data.
 * @param len_chars Specify the address of a value to store the length of the returned path string, in characters, not including the terminating nul.
 * @param len_bytes Specify the address of a value to store the length of the returned path string, in bytes, including the trailing nul.
 * @return A nul-terminated string specifying the relative path to the default test set image data.
 * Free the returned path string using the PathBufferDelete function.
 */
MNIST_API(char*)
TestSetImages_GetDefaultRelativePath
(
    size_t *len_chars, 
    size_t *len_bytes
);

/* @summary Retrieve the relative path to the default test set label data.
 * @param len_chars Specify the address of a value to store the length of the returned path string, in characters, not including the terminating nul.
 * @param len_bytes Specify the address of a value to store the length of the returned path string, in bytes, including the trailing nul.
 * @return A nul-terminated string specifying the relative path to the default test set label data.
 * Free the returned path string using the PathBufferDelete function.
 */
MNIST_API(char*)
TestSetLabels_GetDefaultRelativePath
(
    size_t *len_chars, 
    size_t *len_bytes
);

MNIST_API(char*)
TrainingSetImages_GetDefaultAbsolutePath
(
    size_t *len_chars, 
    size_t *len_bytes
);

MNIST_API(char const*)
TestSetImages_GetDefaultAbsolutePath
(
    size_t *len_chars, 
    size_t *len_bytes
);

#ifdef __cplusplus
}; /* extern "C" */
#endif

#endif /* __DATA_H__ */

