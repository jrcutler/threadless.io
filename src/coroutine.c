/* threadless.io
 * Copyright (c) 2016 Justin R. Cutler
 * Licensed under the MIT License. See LICENSE file in the project root for
 * full license information.
 */
/** @file
 * coroutine interface implementation
 * @author Justin R. Cutler <justin.r.cutler@gmail.com>
 */

/* stack_t (in ucontext.h) */
#define _BSD_SOURCE

/* errno, ENOMEM */
#include <errno.h>
/* memset */
#include <string.h>

/* ucontext_t, getcontext, makecontext, swapcontext */
#include <ucontext.h>

/* allocator_t allocation_t, allocation_init, allocation_realloc_array */
#include <threadless/allocation.h>
/* ... */
#include <threadless/coroutine.h>


enum {
    COROUTINE_ENDED = 1,
};

typedef struct deferred deferred_t;
struct deferred {
    allocation_t allocation;
    coroutine_deferred_function_t *function;
    void *data;
    deferred_t *next;
};

struct coroutine {
    allocation_t allocation;
    ucontext_t   context;
    ucontext_t   caller;
    void         *data;
    int          status;
    deferred_t   *deferred;
};


static void coroutine_entry_point(coroutine_t *, coroutine_function_t *)
    __attribute__ ((noreturn));
static void coroutine_entry_point(coroutine_t *c,
    coroutine_function_t *function)
{
    /* run function until it returns */
    void *retval = function(c, c->data);

    /* mark as ended */
    c->status |= COROUTINE_ENDED;

    /* yield final return value (forever) */
    for (;;) {
        (void) coroutine_yield(c, retval);
    }
}


coroutine_t *coroutine_create(allocator_t *allocator,
    coroutine_function_t *function, size_t stack_size)
{
    coroutine_t *coro = NULL;
    size_t alloc_size = sizeof(*coro) + stack_size;

    if (alloc_size < stack_size) {
        /* integer overflow */
        errno = ENOMEM;
        goto fail;
    }

    allocation_t allocation;
    allocation_init(&allocation, allocator);
    if (allocation_realloc_array(&allocation, 1, alloc_size)) {
        goto fail;
    }

    coro = allocation.memory;
    memset(coro, 0, alloc_size);
    coro->allocation = allocation;
    coro->status = 0;
    coro->deferred = NULL;
    (void) getcontext(&coro->context);
    coro->context.uc_stack.ss_sp = coro + 1;
    coro->context.uc_stack.ss_size = stack_size;

    makecontext(&coro->context, (void (*)(void)) coroutine_entry_point, 2,
        coro, function);

    return coro;

fail:
    coroutine_destroy(coro);

    return NULL;
}


static void coroutine_run_deferred(coroutine_t *coro)
{
    deferred_t *deferred = coro->deferred;
    while (NULL != deferred) {
        allocation_t allocation = deferred->allocation;
        deferred->function(deferred->data);
        deferred = deferred->next;
        allocation_free(&allocation);
    }
}


void coroutine_destroy(coroutine_t *coro)
{
    if (NULL != coro) {
        coroutine_run_deferred(coro);
        allocation_t allocation = coro->allocation;
        allocation_free(&allocation);
    }
}


bool coroutine_ended(const coroutine_t *coro)
{
    return (NULL == coro) || !!(coro->status & COROUTINE_ENDED);
}


void *coroutine_resume(coroutine_t *coro, void *value)
{
    if (NULL == coro) {
        return NULL;
    }
    coro->data = value;
    (void) swapcontext(&coro->caller, &coro->context);
    return coro->data;
}


void *coroutine_yield(coroutine_t *coro, void *value)
{
    if (NULL == coro) {
        return NULL;
    }
    coro->data = value;
    (void) swapcontext(&coro->context, &coro->caller);
    return coro->data;
}


int coroutine_defer(coroutine_t *coro, coroutine_deferred_function_t *function,
    void *data)
{
    int error;
    deferred_t *deferred = NULL;
    allocation_t allocation;

    /* allocate memory */
    allocation_init(&allocation, coro->allocation.allocator);
    error = allocation_realloc_array(&allocation, 1, sizeof(*deferred));

    if (!error) {
       /* initialize deferred work */
        deferred = allocation.memory;
        deferred->allocation = allocation;
        deferred->function = function;
        deferred->data = data;

        /* push deferred work to front of list */
        deferred->next = coro->deferred;
        coro->deferred = deferred;
    }

    return error;
}
