/* threadless.io
 * Copyright (c) 2016 Justin R. Cutler
 * Licensed under the MIT License. See LICENSE file in the project root for
 * full license information.
 */
/** @file
 * allocation interface implementation
 * @author Justin R. Cutler <justin.r.cutler@gmail.com>
 */

/* errno, ENOMEM */
#include <errno.h>
/* size_t, NULL */
#include <stddef.h>

/* ... */
#include <threadless/allocation.h>


#ifndef SIZE_MAX
# define SIZE_MAX ((size_t)-1)
#endif

#define SQRT_SIZE_MAX_PLUS_1 ((size_t)1 << (sizeof(size_t) * 4))


int allocation_realloc_array(allocation_t *allocation, size_t nmemb,
    size_t size)
{
    /* test for multiplication overflow */
    if (((nmemb >= SQRT_SIZE_MAX_PLUS_1) || (size >= SQRT_SIZE_MAX_PLUS_1)) &&
        (nmemb != 0) && ((SIZE_MAX / nmemb) < size)) {
        /* overflow detected */
        errno = ENOMEM;
        return -1;
    }

    size_t alloc_size = nmemb * size;

    /* perform allocator action */
    return allocation->allocator->allocate(allocation, alloc_size);
}
