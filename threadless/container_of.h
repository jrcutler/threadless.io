/* threadless.io
 * Copyright (c) 2016 Justin R. Cutler
 * Licensed under the MIT License. See LICENSE file in the project root for
 * full license information.
 */
/** @file
 * container_of macro definition
 * @author Justin R. Cutler <justin.r.cutler@gmail.com>
 */
#ifndef THREADLESS_CONTAINER_OF_H
#define THREADLESS_CONTAINER_OF_H

/* offsetof */
#include <stddef.h>

/** Get a pointer to the containing structure of a member
 * @param ptr    address of @p member within a @p type
 * @param type   type of container
 * @param member name of the member in @p type
 * @returns pointer to containing @p type
 */
#define container_of(ptr, type, member) ( \
    (type *)((char *)(1 ? (ptr) : &((type *)0)->member) \
        - offsetof(type, member)) \
)

#endif /* THREADLESS_CONTAINER_OF_H */
