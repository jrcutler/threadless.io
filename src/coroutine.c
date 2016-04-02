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
/* calloc, free */
#include <stdlib.h>

/* malloc, munmap, PROT_*, MAP_* */
#include <sys/mman.h>
/* ucontext_t, getcontext, makecontext, swapcontext */
#include <ucontext.h>
/* sysconf, _SC_PAGE_SIZE */
#include <unistd.h>

/* ... */
#include <threadless/coroutine.h>


enum {
    COROUTINE_ENDED = 1,
};


struct coroutine {
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


static int stack_allocate(coroutine_t *coro, size_t stack_pages)
{
    int error = -1;
    size_t page_size = sysconf(_SC_PAGE_SIZE);
    size_t length = page_size * stack_pages;
    void *mapping = MAP_FAILED;

    if ((length / page_size) == stack_pages) {
        mapping = mmap(NULL, length, PROT_READ|PROT_WRITE,
            MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    } else {
        /* integer overflow */
        errno = ENOMEM;
    }

    if (MAP_FAILED != mapping) {
        coro->context.uc_stack.ss_sp = mapping;
        coro->context.uc_stack.ss_size = length;
        error = 0;
    }

    return error;
}


static void stack_free(coroutine_t *coro)
{
    if (NULL != coro->context.uc_stack.ss_sp) {
        (void) munmap(coro->context.uc_stack.ss_sp,
            coro->context.uc_stack.ss_size);
    }
}


coroutine_t *coroutine_create(coroutine_function_t *function,
    size_t stack_pages)
{
    coroutine_t *coro;

    coro = calloc(1, sizeof(*coro));
    if (NULL == coro) {
        goto fail;
    }

    (void) getcontext(&coro->context);

    if (stack_allocate(coro, stack_pages)) {
        goto fail;
    }

    makecontext(&coro->context, (void (*)(void)) coroutine_entry_point, 2,
        coro, function);

    return coro;

fail:
    if (NULL != coro) {
        coroutine_destroy(coro);
    }

    return NULL;
}


void coroutine_destroy(coroutine_t *coro)
{
    if (NULL != coro) {
        stack_free(coro);
        free(coro);
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
