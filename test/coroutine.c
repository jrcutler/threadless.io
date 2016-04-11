/* threadless.io
 * Copyright (c) 2016 Justin R. Cutler
 * Licensed under the MIT License. See LICENSE file in the project root for
 * full license information.
 */
/** @file
 * coroutine interface test
 * @author Justin R. Cutler <justin.r.cutler@gmail.com>
 */

/* HAVE_* */
#include "config.h"

/* printf, perror */
#include <stdio.h>
/* EXIT_SUCCESS, EXIT_FAILURE */
#include <stdlib.h>

/* allocator_t, allocator_destroy */
#include <threadless/allocation.h>
/* default_allocator_get */
#include <threadless/default_allocator.h>
#ifdef HAVE_MMAP
/* mmap_allocator_get */
# include <threadless/mmap_allocator.h>
#endif
/* ... */
#include <threadless/coroutine.h>


static void *fibonacci_generator(coroutine_t *coro, void *data)
{
    size_t x = 0;
    size_t y = 1;

    /* ignore initial input data */
    (void) data;

    /* generate Fibonacci sequence until overflow */
    while (x <= y) {
        size_t tmp = x;
        /* yield next value, ignore new input data */
        (void) coroutine_yield(coro, &tmp);
        /* x, y = y, x + y */
        tmp = y;
        y += x;
        x = tmp;
    }

    /* no more values */
    return NULL;
}


static void deferred_puts(void *data)
{
    puts(data);
}


static void *output_coroutine(coroutine_t *coro, void *data)
{
    size_t *value = data;

    if (coroutine_defer(coro, deferred_puts, "deferred output 0")) {
        return NULL;
    }
    if (coroutine_defer(coro, deferred_puts, "deferred output 1")) {
        return NULL;
    }

    while (NULL != value) {
        printf("%zu\n", *value);
        value = coroutine_yield(coro, value);
    }

    return NULL;
}


static int run(allocator_t *allocator)
{
    int error = -1;
    coroutine_t *fibonacci = NULL;
    coroutine_t *output = NULL;

    fibonacci = coroutine_create(allocator, fibonacci_generator, 4096);
    if (NULL == fibonacci) {
        perror("coroutine_create");
        goto fail;
    }

    output = coroutine_create(allocator, output_coroutine, 4096);
    if (NULL == output) {
        perror("coroutine_create");
        goto fail;
    }

    while (!(coroutine_ended(fibonacci) || coroutine_ended(output))) {
        void *data;
        data = coroutine_resume(fibonacci, NULL);
        data = coroutine_resume(output, data);
    }

    error = 0;

fail:
    coroutine_destroy(output);
    coroutine_destroy(fibonacci);

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
