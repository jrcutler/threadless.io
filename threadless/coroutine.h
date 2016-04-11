/* threadless.io
 * Copyright (c) 2016 Justin R. Cutler
 * Licensed under the MIT License. See LICENSE file in the project root for
 * full license information.
 */
/** @file
 * coroutine interface definition
 * @author Justin R. Cutler <justin.r.cutler@gmail.com>
 */
#ifndef THREADLESS_COROUTINE_H
#define THREADLESS_COROUTINE_H

/* bool, true, false */
#include <stdbool.h>
/* NULL, size_t */
#include <stddef.h>

/* allocator_t */
#include <threadless/allocation.h>

/** Opaque coroutine type */
typedef struct coroutine coroutine_t;

/** Coroutine function type
 * @param[in,out] coro coroutine
 * @param[in,out] data data passed via first call to coroutine_resume()
 * @returns user-defined pointer (which should correspond to value(s) passed to
 *          coroutine_yield())
 */
typedef void *(coroutine_function_t)(coroutine_t *coro, void *data);

/** Coroutine deferred function type
 * @param[in,out] data user-defined data
 */
typedef void (coroutine_deferred_function_t)(void *data);

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Create a coroutine
 * @param[in,out] allocator  allocator to use to create/destroy memory
 * @param         function   function to run in coroutine
 * @param         stack_size coroutine stack size
 * @retval non-NULL new coroutine
 * @retval NULL     error (check @c errno for reason)
 * @post upon success, return value may be passed to coroutine_resume()
 * @post upon success, return value must be passed to coroutine_destroy()
 */
coroutine_t *coroutine_create(allocator_t *allocator,
    coroutine_function_t *function, size_t stack_size);

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

/** Defer a function call until coroutine termination
 * @param[in,out] coro     coroutine
 * @param         function function to call when @p coro terminates
 * @param         data     user-defined point to pass to @p function
 * @retval 0  success
 * @retval -1 error
 * @post @p function will be called with @p data when @p coro terminates
 * @note Registered functions will be called in reverse order of registration
 */
int coroutine_defer(coroutine_t *coro, coroutine_deferred_function_t *function,
    void *data);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* THREADLESS_COROUTINE_H */
