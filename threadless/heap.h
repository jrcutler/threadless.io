/* threadless.io
 * Copyright (c) 2016 Justin R. Cutler
 * Licensed under the MIT License. See LICENSE file in the project root for
 * full license information.
 */
/** @file
 * heap interface definition
 * @author Justin R. Cutler <justin.r.cutler@gmail.com>
 */
#ifndef THREADLESS_HEAP_H
#define THREADLESS_HEAP_H

/* size_t, NULL */
#include <stddef.h>

/* allocation_t, allocator_t, allocation_init */
#include <threadless/allocation.h>

/** Heap descriptor type */
typedef struct heap heap_t;

/** Heap node type */
typedef struct {
    /** Back-pointer to containing heap */
    heap_t *heap;
    /** Current index within @p heap */
    size_t index;
} heap_node_t;

/** Heap node comparison function type
 * @param[in] a first node
 * @param[in] b second node
 * @retval >0 @p a is "better" than @p b
 * @retval 0  @p a is equivalent to @p b
 * @retval <0 @p a is "worse" than @p b
 */
typedef int (heap_compare_function_t)(const heap_node_t *a,
    const heap_node_t *b);

/** Heap descriptor structure */
struct heap {
    /** Heap node pointer storage */
    allocation_t allocation;
    /** Number of nodes in heap */
    size_t count;
    /** Heap node comparison function */
    heap_compare_function_t *compare;
};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Initialize a heap descriptor
 * @param[out] heap      heap to initialize
 * @param      allocator allocator instance
 * @param      compare   comparison function
 * @post @p heap represents an empty heap with given @p compare function
 */
static inline void heap_init(heap_t *heap, allocator_t *allocator,
    heap_compare_function_t *compare)
{
    allocation_init(&heap->allocation, allocator);
    heap->count = 0;
    heap->compare = compare;
}

/** Peek at the "best" node in @p heap
 * @param[in] heap heap
 * @retval NULL     @p heap is empty
 * @retval non-NULL pointer to "best" node in @p heap
 */
static inline heap_node_t *heap_peek(const heap_t *heap)
{
    heap_node_t **storage = heap->count ? heap->allocation.memory : NULL;
    return storage ? *storage : NULL;
}

/** Push a @p node into @p heap
 * @param[in,out] heap heap
 * @param[in,out] node node
 * @retval 0  success
 * @retval -1 error
 */
int heap_push(heap_t *heap, heap_node_t *node);

/** Replace an existing @p old node in a heap with a new @p node
 * @param[in,out] old  old node
 * @param[in,out] node new node
 * @pre @p old must be in a heap
 * @post @p old is no longer in a heap
 * @post @p node is in the heap that previously contained @p old
 */
void heap_replace(heap_node_t *old, heap_node_t *node);

/** Remove an arbitrary @p node from its heap
 * @param[in,out] node node
 * @pre @p node must be in a heap
 * @post @p node is no longer in a heap
 */
void heap_remove(heap_node_t *node);

/** Remove and return the "best" node in @p heap
 * @param[in,out] heap heap
 * @retval NULL     @p heap was empty
 * @retval non-NULL pointer to removed "best" node from @p heap
 */
heap_node_t *heap_pop(heap_t *heap);

/** Finalize a heap
 * @param[in,out] heap heap
 * @post All nodes have been removed from @p heap and all backing memory has
 *       been released
 */
void heap_fini(heap_t *heap);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* THREADLESS_HEAP_H */
