/* threadless.io
 * Copyright (c) 2016 Justin R. Cutler
 * Licensed under the MIT License. See LICENSE file in the project root for
 * full license information.
 */

/* printf, perror */
#include <stdio.h>
/* EXIT_SUCCESS, EXIT_FAILURE */
#include <stdlib.h>

/* ... */
#include <threadless/coroutine.h>


static void *fibonacci_generator(coroutine *coro, void *data)
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


static void *output_coroutine(coroutine *coro, void *data)
{
    size_t *value = data;

    while (NULL != value) {
        printf("%zu\n", *value);
        value = coroutine_yield(coro, value);
    }

    return NULL;
}


static int run(void)
{
    int error = -1;
    coroutine *fibonacci = NULL;
    coroutine *output = NULL;

    fibonacci = coroutine_create(fibonacci_generator, 1);
    if (NULL == fibonacci) {
        perror("coroutine_create");
        goto fail;
    }

    output = coroutine_create(output_coroutine, 1);
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

    (void) argc;
    (void) argv;

    error = run();

    return !error ? EXIT_SUCCESS : EXIT_FAILURE;
}
