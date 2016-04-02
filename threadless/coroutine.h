/* threadless.io
 * Copyright (c) 2016 Justin R. Cutler
 * Licensed under the MIT License. See LICENSE file in the project root for
 * full license information.
 */
#ifndef THREADLESS_COROUTINE_H
#define THREADLESS_COROUTINE_H

/* bool, true, false */
#include <stdbool.h>
/* NULL, size_t */
#include <stddef.h>

/** Opaque coroutine type */
typedef struct coroutine coroutine_t;

/** Coroutine function type
 * @param[in,out] coro coroutine
 * @param[in,out] data data passed via first call to coroutine_resume()
 * @returns user-defined pointer (which should correspond to value(s) passed to
 *          coroutine_yield())
 */
typedef void *(coroutine_function_t)(coroutine_t *coro, void *data);

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Create a coroutine
 * @param function    function to run in coroutine
 * @param stack_pages number of pages to map for coroutine stack
 * @retval non-NULL new coroutine
 * @retval NULL     error (check @c errno for reason)
 * @post upon success, return value may be passed to coroutine_resume()
 * @post upon success, return value must be passed to coroutine_destroy()
 */
coroutine_t *coroutine_create(coroutine_function_t *function,
    size_t stack_pages);

/** Destroy a coroutine
 * @param[in,out] coro coroutine to destroy
 * @pre @p coro must have been returned by coroutine_create()
 * @post @p coro may no longer be used
 */
void coroutine_destroy(coroutine_t *coro);

/** Test if a coroutine has ended
 * @param[in] coro coroutine to test
 * @retval true  coroutine has ended (or is @c NULL)
 * @retval false coroutine has not ended
 * @pre @p coro must have been returned by coroutine_create()
 */
bool coroutine_ended(const coroutine_t *coro);

/** Resume a coroutine, passing a value to coroutine_yield()
 * @param[in,out] coro  coroutine to resume
 * @param[in,out] value value to pass (to initial call or as return from
 *                      coroutine_yield())
 * @returns value passed by @p coro to coroutine_yield()
 * @pre @p coro must have been returned by coroutine_create()
 */
void *coroutine_resume(coroutine_t *coro, void *value);

/** Yield from a coroutine, passing a value to coroutine_resume()
 * @param[in,out] coro  coroutine to yield from
 * @param[in,out] value value to return from coroutine_resume()
 * @returns value passed by @p coro to coroutine_resume()
 * @pre @p coro must have been returned by coroutine_create()
 */
void *coroutine_yield(coroutine_t *coro, void *value);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* THREADLESS_COROUTINE_H */
