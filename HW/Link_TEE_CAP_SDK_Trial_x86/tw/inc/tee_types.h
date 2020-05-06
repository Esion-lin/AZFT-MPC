/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _TEE_TYPES_H_
#define _TEE_TYPES_H_

#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>

#define TEE_ROUNDUP(a, b)       (((a) + ((b) - 1)) & ~((b) - 1))
#define TEE_ROUNDDOWN(a, b)     ((a) & ~((b) - 1))
#define TEE_IS_ALIGNED(a, b)    (!(((ulong_t)(a)) & (((ulong_t)(b)) - 1)))

#define TEE_MIN(a, b)           (((a) < (b)) ? (a) : (b))
#define TEE_MAX(a, b)           (((a) > (b)) ? (a) : (b))

#define TEE_PAGE_SIZE           (0x1000)
#define TEE_PAGE_SHIFT          (12)

typedef unsigned long  ulong_t;
typedef long           long_t;
typedef ulong_t        tee_impl;
typedef uint32_t       tee_stat_t;

#endif /* _TEE_TYPES_H_ */

