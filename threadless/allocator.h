/* threadless.io
 * Copyright (c) 2016 Justin R. Cutler
 * Licensed under the MIT License. See LICENSE file in the project root for
 * full license information.
 */
/** @file
 * allocator interface definition
 * @author Justin R. Cutler <justin.r.cutler@gmail.com>
 */
#ifndef THREADLESS_ALLOCATOR_H
#define THREADLESS_ALLOCATOR_H

/* size_t, NULL */
#include <stddef.h>

/** Memory allocator instance */
typedef struct allocator allocator_t;

/** Memory allocator function
 * @param[in,out] allocator allocator instance
 * @param[in,out] ptr       memory to reallocate/free (or @c NULL to allocate)
 * @param         nmemb     number of elements to allocate (or 0 to free)
 * @param         size      size of each element to allocate (or 0 to free)
 * @retval non-NULL successful allocation/reallocation
 * @retval NULL     successful free (@p nmemb or @p size was 0) or error
 * @note Upon detection of integer overflow, this function shall return
 *       @c NULL (like @c calloc(3) and FreeBSD's @c realloc_array(3))
 */
typedef void *(allocator_function_t)(allocator_t *allocator, void *ptr,
    size_t nmemb, size_t size);

/** Memory allocator destructor function
 * @param[in,out] allocator allocator instance
 * @post @p allocator may no longer be used
 */
typedef void (allocator_destroy_function_t)(allocator_t *allocator);

/** Memory allocator instance structure */
struct allocator {
    /** Memory allocator function */
    allocator_function_t *const allocate;
    /** Memory allocator instance destructor */
    allocator_destroy_function_t *const destroy;
};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Get default allocator instance
 * @returns default allocator
 */
allocator_t *allocator_get_default(void);

/** @c malloc() helper function
 * @param[in,out] allocator allocator
 * @param         size      size to allocate
 * @retval Non-NULL allocated memory
 * @retval NULL     error
 */
static inline void *allocator_malloc(allocator_t *allocator, size_t size)
{
    return allocator->allocate(allocator, NULL, 1, size);
}

/** @c realloc_array() helper function
 * @copydetails allocator_function_t
 */
static inline void *allocator_realloc_array(allocator_t *allocator, void *ptr,
    size_t nmemb, size_t size)
{
    return allocator->allocate(allocator, ptr, nmemb, size);
}

/** @c free() helper function
 * @param[in,out] allocator allocator
 * @param         ptr       memory to free
 */
static inline void allocator_free(allocator_t *allocator, void *ptr)
{
    (void) allocator->allocate(allocator, ptr, 0, 0);
}

/** Memory allocator destructor helper function
 * @copydetails allocator_destroy_function_t
 */
static inline void allocator_destroy(allocator_t *allocator)
{
    allocator->destroy(allocator);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* THREADLESS_ALLOCATOR_H */
