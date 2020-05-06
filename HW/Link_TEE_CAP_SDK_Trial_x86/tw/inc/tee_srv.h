/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _TEE_SRV_H_
#define _TEE_SRV_H_

#include "tee_types.h"
#include "tee_internal_api.h"

#define TEE_MAX_DAEMON_NUM  2 


typedef enum _tee_srv_type_t
{
    SRV_TYPE_INVALID = 0,
    SRV_TYPE_1_I_1_S_NKA = 1,
    SRV_TYPE_1_I_M_S_NKA = 2,
} tee_srv_type_t;

typedef struct _tee_srv_head_t {
    TEE_UUID *     uuid;
    tee_srv_type_t type;
    uint32_t       stack_size;

    TEE_Result  (* TA_CreateEntryPoint)(void);
    void        (* TA_DestroyEntryPoint)(void);
    TEE_Result  (* TA_OpenSessionEntryPoint)(
                    uint32_t, TEE_Param [4], void **);
    void        (* TA_CloseSessionEntryPoint)(void *);
    TEE_Result  (* TA_InvokeCommandEntryPoint)(
                    void *, uint32_t, uint32_t, TEE_Param [4]);
} tee_srv_head_t;

extern tee_srv_head_t * tee_srv_table[];
extern tee_srv_head_t * tee_daemon_table[];

extern int32_t tee_boot_entry(void *arg);

#endif /* _TEE_SRV_H_ */

