/* threadless.io
 * Copyright (c) 2016 Justin R. Cutler
 * Licensed under the MIT License. See LICENSE file in the project root for
 * full license information.
 */
/** @file
 * @c mmap(3) allocator interface definition
 * @author Justin R. Cutler <justin.r.cutler@gmail.com>
 */
#ifndef THREADLESS_MMAP_ALLOCATOR_H
#define THREADLESS_MMAP_ALLOCATOR_H

/* allocator_t */
#include <threadless/allocation.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Get @c mmap(3) allocator instance
 * @returns default allocator
 */
allocator_t *allocator_get_mmap(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* THREADLESS_MMAP_ALLOCATOR_H */
