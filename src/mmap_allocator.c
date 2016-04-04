/* threadless.io
 * Copyright (c) 2016 Justin R. Cutler
 * Licensed under the MIT License. See LICENSE file in the project root for
 * full license information.
 */
/** @file
 * @c mmap(3) allocator implementation
 * @author Justin R. Cutler <justin.r.cutler@gmail.com>
 */

#define _GNU_SOURCE

/* HAVE_* */
#include "config.h"

/* errno, ENOSYS */
#include <errno.h>
/* size_t, NULL */
#include <stddef.h>
#ifndef HAVE_MREMAP
/* memcpy */
# include <string.h>
#endif

/* mmap, munmap, PROT_*, MAP_*, mremap(optional), MREMAP_*(optional) */
#include <sys/mman.h>
/* sysconf, _SC_PAGESIZE */
#include <unistd.h>

/* ... */
#include <threadless/mmap_allocator.h>


#ifndef HAVE_MMAP
# error Need mmap()
#endif

#if defined(HAVE_MAP_ANONYMOUS) || defined(HAVE_MAP_ANON)
# ifndef HAVE_MAP_ANONYMOUS
#  define MAP_ANONYMOUS MAP_ANON
# endif
#else
# error Need MAP_ANONYMOUS or MAP_ANON
#endif


static void *do_mmap(size_t size)
{
    return mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS,
        -1, 0);
}


static void *do_mremap(void *memory, size_t old_size, size_t new_size)
{
    void *new_memory;
#ifdef HAVE_MREMAP
    new_memory = mremap(memory, old_size, new_size, MREMAP_MAYMOVE);
#else
    /* slow remap: mmap + copy + munmap */
    size_t copy_size = (new_size < old_size) ? new_size : old_size;
    new_memory = do_mmap(new_size);
    if (MAP_FAILED != new_memory) {
        memcpy(new_memory, memory, copy_size);
        (void) munmap(memory, old_size);
    }
#endif
    return new_memory;
}


static int mmap_allocate(allocation_t *allocation, size_t size)
{
    static size_t page_size = 0;
    static size_t page_mask = 0;
    void *new_memory = MAP_FAILED;

    if (!page_size) {
        /* get page size */
        long result = sysconf(_SC_PAGESIZE);
        if (result > 0 && !(result & (result - 1))) {
            page_size = (size_t) result;
            page_mask = page_size - 1;
        } else {
            /* not a power of 2 */
            errno = ENOSYS;
            return -1;
        }
    }

    /* round size up to multiple of page_size */
    size = (size + page_size - 1) & ~page_mask;

    /* no action required */
    if (size == allocation->size) {
        return 0;
    }

    if (0 == size) {
        if (NULL != allocation->memory && 0 != allocation->size) {
            (void)munmap(allocation->memory, allocation->size);
        }
        new_memory = NULL;
    } else if (0 == allocation->size) {
        new_memory = do_mmap(size);
    } else {
        new_memory = do_mremap(allocation->memory, allocation->size, size);
    }

    if (MAP_FAILED == new_memory) {
        /* operation failed */
        return -1;
    }

    /* update allocation */
    allocation->memory = new_memory;
    allocation->size = size;

    return 0;
}


static void mmap_destroy(allocator_t *allocator)
{
    /* ignore allocator */
    (void) allocator;
}


static allocator_t mmap_allocator = {
    .allocate = mmap_allocate,
    .destroy = mmap_destroy,
};


allocator_t *mmap_allocator_get(void)
{
    return &mmap_allocator;
}
