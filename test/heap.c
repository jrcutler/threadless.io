/* threadless.io
 * Copyright (c) 2016 Justin R. Cutler
 * Licensed under the MIT License. See LICENSE file in the project root for
 * full license information.
 */
/** @file
 * heap interface test
 * @author Justin R. Cutler <justin.r.cutler@gmail.com>
 */

/* HAVE_* */
#include "config.h"

/* errno, EINVAL */
#include <errno.h>
/* printf, perror */
#include <stdio.h>
/* EXIT_SUCCESS, EXIT_FAILURE */
#include <stdlib.h>

/* container_of */
#include <threadless/container_of.h>
/* default_allocator_get */
#include <threadless/default_allocator.h>
#ifdef HAVE_MMAP
/* mmap_allocator_get */
# include <threadless/mmap_allocator.h>
#endif
/* ... */
#include <threadless/heap.h>


const int data[] = { 4 /* duplicate */, 1, 9, 2, 8, 4, 0, 5, 3, 6, 7, 99 };
const size_t data_count = sizeof(data) / sizeof(data[0]);


typedef struct {
    heap_node_t node;
    int value;
} value_t;


static int min_compare(const heap_node_t *a, const heap_node_t *b)
{
    int va = container_of(a, const value_t, node)->value;
    int vb = container_of(b, const value_t, node)->value;
    return va - vb;
}


static int run(allocator_t *allocator)
{
    int error = 0;
    allocation_t alloc;
    value_t *values = NULL;
    heap_t heap;
    size_t i;
    heap_node_t *node;

    allocation_init(&alloc, allocator);
    error = allocation_realloc_array(&alloc, data_count, sizeof(*values));
    if (error) {
        return error;
    }
    values = alloc.memory;

    heap_init(&heap, allocator, min_compare);

    /* push values into heap */
    for (i = 0; !error && i < data_count; ++i) {
        values[i].value = data[i];
        error = heap_push(&heap, &(values[i].node));
    }

    /* remove from middle */
    heap_remove(&(values[0].node));
    /* remove from end */
    heap_remove(&(values[data_count - 1].node));

    /* pull values out of heap (minimum first) */
    while (NULL != (node = heap_pop(&heap))) {
        value_t *value = container_of(node, value_t, node);
        printf("%i\n", value->value);
    }

    heap_fini(&heap);

    allocation_free(&alloc);

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
