/* threadless.io
 * Copyright (c) 2016 Justin R. Cutler
 * Licensed under the MIT License. See LICENSE file in the project root for
 * full license information.
 */
/** @file
 * default allocator implementation
 * @author Justin R. Cutler <justin.r.cutler@gmail.com>
 */
/* errno, ENOMEM */
#include <errno.h>
/* realloc */
#include <stdlib.h>

/* ... */
#include <threadless/allocator.h>


#ifndef SIZE_MAX
# define SIZE_MAX ((size_t)-1)
#endif

#define SQRT_SIZE_MAX_PLUS_1 ((size_t)1 << (sizeof(size_t) * 4))


static void *default_allocate(allocator_t *allocator, void *ptr, size_t nmemb,
    size_t size)
{
    /* ignore allocator */
    (void) allocator;
    /* test for multiplication overflow */
    if (((nmemb >= SQRT_SIZE_MAX_PLUS_1) || (size >= SQRT_SIZE_MAX_PLUS_1)) &&
        (nmemb != 0) && ((SIZE_MAX / nmemb) < size)) {
        /* overflow detected */
        errno = ENOMEM;
        return NULL;
    }
    /* perform allocator action */
    return realloc(ptr, size * nmemb);
}


static void default_destory(allocator_t *allocator)
{
    /* ignore allocator */
    (void) allocator;
}


static allocator_t default_allocator = {
    .allocate = default_allocate,
    .destroy = default_destory,
};


allocator_t *allocator_get_default(void)
{
    return &default_allocator;
}
