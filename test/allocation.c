/* threadless.io
 * Copyright (c) 2016 Justin R. Cutler
 * Licensed under the MIT License. See LICENSE file in the project root for
 * full license information.
 */
/** @file
 * allocation interface test
 * @author Justin R. Cutler <justin.r.cutler@gmail.com>
 */

/* HAVE_* */
#include "config.h"

/* printf, perror */
#include <stdio.h>
/* EXIT_SUCCESS, EXIT_FAILURE */
#include <stdlib.h>

/* default_allocator_get */
#include <threadless/default_allocator.h>
#ifdef HAVE_MMAP
/* mmap_allocator_get */
# include <threadless/mmap_allocator.h>
#endif
/* ... */
#include <threadless/allocation.h>


static int run(allocator_t *allocator)
{
    int error = 0;
    allocation_t allocation;
    size_t size;

    allocation_init(&allocation, allocator);

    for (size = 1; !error && size < (1 << 22); size = size << 1) {
        error = allocation_realloc_array(&allocation, 1, size);
    }

    if (!error) {
        size_t big = 1UL << (sizeof(size_t) * 4);
        error = !allocation_realloc_array(&allocation, big, big);
    }

    for (size = 1 << 22; !error && size; size = size >> 1) {
        error = allocation_realloc_array(&allocation, 1, size);
    }

    if (!error) {
        printf("OK\n");
    } else {
        perror("allocation_realloc_array");
    }

    allocation_free(&allocation);

    return error;
}


int main(int argc, char *argv[])
{
    int error;
    allocator_t *allocator;

    (void) argc;
    (void) argv;

    printf("default allocator:\n");
    allocator = default_allocator_get();
    error = run(allocator);
    allocator_destroy(allocator);

#ifdef HAVE_MMAP
    if (!error) {
        printf("mmap allocator:\n");
        allocator = mmap_allocator_get();
        error = run(allocator);
        allocator_destroy(allocator);
    }
#endif

    return !error ? EXIT_SUCCESS : EXIT_FAILURE;
}
