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

/* ... */
#include <threadless/coroutine.h>


enum {
    COROUTINE_ENDED = 1,
};


struct coroutine {
    allocator_t *allocator;
    ucontext_t  context;
    ucontext_t  caller;
    void        *data;
    int         status;
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

    coro = allocator_malloc(allocator, alloc_size);
    if (NULL == coro) {
        goto fail;
    }

    memset(coro, 0, alloc_size);
    coro->allocator = allocator;
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


void coroutine_destroy(coroutine_t *coro)
{
    if (NULL != coro && NULL != coro->allocator) {
        allocator_free(coro->allocator, coro);
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
