/* threadless.io
 * Copyright (c) 2016 Justin R. Cutler
 * Licensed under the MIT License. See LICENSE file in the project root for
 * full license information.
 */
/** @file
 * allocator interface definition
 * @author Justin R. Cutler <justin.r.cutler@gmail.com>
 */
#ifndef THREADLESS_DEFAULT_ALLOCATOR_H
#define THREADLESS_DEFAULT_ALLOCATOR_H

/* allocator_t */
#include <threadless/allocation.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Get default allocator instance
 * @returns default allocator
 */
allocator_t *allocator_get_default(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* THREADLESS_DEFAULT_ALLOCATOR_H */
