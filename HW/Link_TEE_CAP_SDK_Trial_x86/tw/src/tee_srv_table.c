/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "tee_srv.h"

#define TEE_MAX_SRV_NUM  16

extern tee_srv_head_t xor_srv_head;

/* TEE registered srv table */
tee_srv_head_t * tee_srv_table[TEE_MAX_SRV_NUM] = {
    &xor_srv_head,
    NULL,
};

int32_t tee_load_entry(void *arg)
{
    return tee_boot_entry(arg);
}
