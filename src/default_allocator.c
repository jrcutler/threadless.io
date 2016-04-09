/* threadless.io
 * Copyright (c) 2016 Justin R. Cutler
 * Licensed under the MIT License. See LICENSE file in the project root for
 * full license information.
 */
/** @file
 * default allocator implementation
 * @author Justin R. Cutler <justin.r.cutler@gmail.com>
 */

/* realloc */
#include <stdlib.h>

/* ... */
#include <threadless/default_allocator.h>


static int default_allocate(allocation_t *allocation, size_t size)
{
    void *new_memory;

    /* perform allocator action */
    new_memory = realloc(allocation->memory, size);

    if ((NULL == new_memory) && (size != 0)) {
        /* realloc failed */
        return -1;
    }

    /* update allocation */
    allocation->memory = new_memory;
    allocation->size = size;

    return 0;
}


static void default_destroy(allocator_t *allocator)
{
    /* ignore allocator */
    (void) allocator;
}


static allocator_t default_allocator = {
    .allocate = default_allocate,
    .destroy = default_destroy,
};


allocator_t *default_allocator_get(void)
{
    return &default_allocator;
}
