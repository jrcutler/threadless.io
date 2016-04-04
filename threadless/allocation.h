/* threadless.io
 * Copyright (c) 2016 Justin R. Cutler
 * Licensed under the MIT License. See LICENSE file in the project root for
 * full license information.
 */
/** @file
 * allocation interface definition
 * @author Justin R. Cutler <justin.r.cutler@gmail.com>
 */
#ifndef THREADLESS_ALLOCATION_H
#define THREADLESS_ALLOCATION_H

/* size_t, NULL */
#include <stddef.h>

/** Memory allocator instance */
typedef struct allocator allocator_t;

/** Memory allocation handle */
typedef struct {
    /** allocator used to manage this memory */
    allocator_t *allocator;
    /** pointer to allocated memory */
    void *memory;
    /** current size of allocated memory */
    size_t size;
} allocation_t;

/** Memory allocator function
 * @param[in,out] allocation memory allocation handle
 * @param         size       minimum new size of memory (or 0 to free)
 * @retval 0  success
 * @retval -1 error
 * @pre @p allocation must be initialized
 * @pre If @p allocation->memory is @c NULL and @p allocation->size is
 *      non-zero, function shall allocate new memory
 * @pre If @p size is 0, function shall free allocated memory
 * @post Upon success, @p allocation has been updated to reflect changes
 * @post Upon failure, @p allocation has not been changed
 */
typedef int (allocator_function_t)(allocation_t *allocation, size_t size);

/** Memory allocator destructor function
 * @param[in,out] allocator allocator instance
 * @post @p allocator may no longer be used
 */
typedef void (allocator_destroy_function_t)(allocator_t *allocator);

/** Memory allocator instance structure */
struct allocator {
    /** Memory (re)allocator function */
    allocator_function_t *const allocate;
    /** Memory allocator instance destructor */
    allocator_destroy_function_t *const destroy;
};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Initialize an allocation handle
 * @param[out] allocation allocation
 * @param      allocator  allocator instance
 * @pre @p allocation must be initialized
 * @post @p allocation->allocator has been set to @p allocator,
 *       @p allocation->memory has been set to @c NULL, and @p allocation->size
 *       has been set to 0
 */
static inline void allocation_init(allocation_t *allocation,
    allocator_t *allocator)
{
    allocation->allocator = allocator;
    allocation->memory = NULL;
    allocation->size = 0;
}

/** @c realloc_array() helper function
 * @param[in,out] allocation memory allocation handle
 * @param         nmemb      number of elements to allocate (or 0 to free)
 * @param         size       size of each element to allocate (or 0 to free)
 * @retval 0  success
 * @retval -1 error
 * @pre @p allocation must point to an initialized, valid allocation
 * @pre If @p allocation->memory is @c NULL, function shall allocate new
 *      memory
 * @pre If @p nmemb or @p size is 0, function shall free allocated memory
 * @post Upon success, @p allocation->memory and @p allocation->size have been
 *       updated to reflect changes
 * @post Upon failure, @p allocation has not been changed
 * @note Upon detection of integer overflow, this function shall return
 *       @c NULL (like @c calloc(3) and FreeBSD's @c realloc_array(3))
 */
int allocation_realloc_array(allocation_t *allocation, size_t nmemb,
    size_t size);

/** @c free() helper function
 * @param[in,out] allocation memory allocation handle
 * @pre @p allocation must point to an initialized, valid allocation
 * @post @p allocation->memory is set to @c NULL and @p allocation->size is set
 *       to 0
 */
static inline void allocation_free(allocation_t *allocation)
{
    (void) allocation_realloc_array(allocation, 0, 0);
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
