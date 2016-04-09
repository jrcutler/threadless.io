/* threadless.io
 * Copyright (c) 2016 Justin R. Cutler
 * Licensed under the MIT License. See LICENSE file in the project root for
 * full license information.
 */
/** @file
 * heap interface implementation
 * @author Justin R. Cutler <justin.r.cutler@gmail.com>
 */

/* size_t, NULL */
#include <stddef.h>

/* allocation_realloc_array */
#include <threadless/allocation.h>
/* ... */
#include <threadless/heap.h>


static inline void swap(heap_node_t **storage, size_t a, size_t b)
{
    /* swap elements */
    heap_node_t *tmp = storage[a];
    storage[a] = storage[b];
    storage[b] = tmp;
    /* update back references */
    storage[a]->index = a;
    storage[b]->index = b;
}


static void sift_down(const heap_t *heap, size_t start, size_t pos)
{
    heap_node_t **storage = heap->allocation.memory;
    while (pos > start) {
        /* if node pos is "better" than parent, swap them */
        size_t parent = (pos - 1) >> 1;
        if (heap->compare(storage[pos], storage[parent]) < 0) {
            swap(storage, pos, parent);
            pos = parent;
        } else {
            /* node pos is in the correct location */
            break;
        }
    }
}


static void sift_up(const heap_t *heap, size_t pos, size_t end)
{
    heap_node_t **storage = heap->allocation.memory;
    size_t start = pos;
    size_t child = (pos << 1) + 1;
    /* bubble smaller child up until hitting a leaf */
    while (child < end) {
        size_t right = child + 1;
        if (right < end && heap->compare(storage[right], storage[child]) < 0) {
            child = right;
        }
        /* move smaller child up */
        swap(storage, pos, child);
        pos = child;
        child = (pos << 1) + 1;
    }
    /* bubble node originally at pos into place */
    sift_down(heap, start, pos);
}


int heap_push(heap_t *heap, heap_node_t *node)
{
    int error = allocation_realloc_array(&(heap->allocation), heap->count + 1,
        sizeof(heap_node_t *));
    if (!error) {
        heap_node_t **storage = heap->allocation.memory;
        /* place item at end of heap */
        node->heap = heap;
        node->index = heap->count++;
        storage[node->index] = node;
        /* bubble item into place */
        sift_down(heap, 0, node->index);
    }
    return error;
}


void heap_replace(heap_node_t *old, heap_node_t *node)
{
    heap_t *heap = old->heap;
    size_t pos = old->index;
    heap_node_t **storage = heap->allocation.memory;

    /* disassociate old node from heap */
    old->heap = NULL;
    old->index = 0;

    /* place new node in heap */
    node->heap = heap;
    node->index = pos;
    storage[pos] = node;

    /* restore heap invariant */
    sift_up(heap, pos, heap->count);
    sift_down(heap, 0, pos);
}


void heap_remove(heap_node_t *node)
{
    heap_t *heap = node->heap;
    size_t pos = node->index;

    if (heap->count) {
        /* shrink heap */
        heap->count--;
        if (pos != heap->count) {
            /* exchange previous last element for removed element */
            swap(heap->allocation.memory, pos, heap->count);
            /* shrink storage */
            (void) allocation_realloc_array(&(heap->allocation), heap->count,
                sizeof(heap_node_t *));
            /* restore heap invariant */
            sift_up(heap, pos, heap->count);
        }
    }

    /* disassociate node from heap */
    node->heap = NULL;
    node->index = 0;
}


heap_node_t *heap_pop(heap_t *heap)
{
    heap_node_t *node = heap_peek(heap);

    if (NULL != node) {
        heap_remove(node);
    }

    return node;
}


void heap_fini(heap_t *heap)
{
    /* diassociate all remaining nodes from heap */
    heap_node_t **storage = heap->allocation.memory;
    size_t i;
    for (i = 0; i < heap->count; ++i) {
        storage[i]->heap = NULL;
        storage[i]->index = 0;
    }
    allocation_free(&(heap->allocation));
}
