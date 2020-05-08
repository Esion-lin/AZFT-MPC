/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include "tee_client_api.h"
#include "xor.h"

#define ERR_LOG(_fn, _res)  \
        printf("ERROR - failed to call %s, 0x%08x\n", (_fn), (_res))

#define _DEPRESS_UNUSED_WARNING(_x) do { (_x) = (_x); } while (0)

static const TEEC_UUID _g_uuid_xor = UUID_XOR;

TEEC_Result enc(uint8_t *data, uint32_t data_len, uint8_t *out_data, uint32_t *out_data_len, bool encrypt){
    TEEC_Context     context;
    TEEC_Session     session;
    TEEC_Operation   operation;
    TEEC_Result      result = TEEC_SUCCESS;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("ERR failed to initial context\n");
        goto cleanup1;
    }
    result = TEEC_OpenSession(&context, &session, &_g_uuid_xor, TEEC_LOGIN_PUBLIC,
                NULL, NULL, NULL);
    if (result != TEEC_SUCCESS) {
        printf("open ss fail(0x%08x)\n", result);
        goto cleanup2;
    }
    

    operation.paramTypes = TEEC_PARAM_TYPES(
                                TEEC_MEMREF_TEMP_INPUT,
                                TEEC_MEMREF_TEMP_OUTPUT,
                                TEEC_NONE,
                                TEEC_NONE);

    operation.params[0].tmpref.buffer = data;
    operation.params[0].tmpref.size = data_len;
    operation.params[1].tmpref.buffer = out_data;
    operation.params[1].tmpref.size = *out_data_len;
    if (encrypt){
        result = TEEC_InvokeCommand(&session, CMD_ENCRYPT, &operation, NULL);
    }else {
        result = TEEC_InvokeCommand(&session, CMD_DECRYPT, &operation, NULL);
    }
    if (result != TEEC_SUCCESS) {
        printf("inv cmd failed(0x%08x)\n", result);
        goto cleanup3;
    }
cleanup3:
    TEEC_CloseSession(&session);
cleanup2:
    TEEC_FinalizeContext(&context);
cleanup1:
    return result;
}
TEEC_Result get_public_key(uint8_t *name, uint32_t name_len, uint8_t *out_data, uint32_t *out_data_len, uint32_t type){
    TEEC_Context     context;
    TEEC_Session     session;
    TEEC_Operation   operation;
    TEEC_Result      result = TEEC_SUCCESS;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("ERR failed to initial context\n");
        goto cleanup1;
    }
    result = TEEC_OpenSession(&context, &session, &_g_uuid_xor, TEEC_LOGIN_PUBLIC,
                NULL, NULL, NULL);
    if (result != TEEC_SUCCESS) {
        printf("open ss fail(0x%08x)\n", result);
        goto cleanup2;
    }
    

    operation.paramTypes = TEEC_PARAM_TYPES(
                                TEEC_VALUE_INPUT,
                                TEEC_MEMREF_TEMP_OUTPUT,
                                TEEC_NONE,
                                TEEC_NONE);
    operation.params[0].value.a = type;
    operation.params[1].tmpref.buffer = out_data;
    operation.params[1].tmpref.size = *out_data_len;
    result = TEEC_InvokeCommand(&session, CMD_PUBLIC_KEY_GET, &operation, NULL);
    
    if (result != TEEC_SUCCESS) {
        printf("inv cmd failed(0x%08x)\n", result);
        goto cleanup3;
    }
cleanup3:
    TEEC_CloseSession(&session);
cleanup2:
    TEEC_FinalizeContext(&context);
cleanup1:
    return result;
}
int main(int argc, char *argv[])
{
    /* define some varible*/
    uint32_t in_len = 32;
    uint32_t out_len = 32;
    uint8_t *in_data_buf = NULL;
    uint8_t *out_data_buf = NULL;
    /*init*/
    TEEC_Context     context;
    TEEC_Session     session;
    TEEC_Operation   operation;
    TEEC_Result      ret = TEEC_SUCCESS;
    ret = TEEC_InitializeContext(NULL, &context);
    if (ret != TEEC_SUCCESS) {
        printf("ERR failed to initial context\n");
        goto cleanup1;
    }
    ret = TEEC_OpenSession(&context, &session, &_g_uuid_xor, TEEC_LOGIN_PUBLIC,
                NULL, NULL, NULL);
    if (ret != TEEC_SUCCESS) {
        printf("open ss fail(0x%08x)\n", ret);
        goto cleanup2;
    }
    /* end init */

    
    out_data_buf = (uint8_t *)malloc(32);
    in_data_buf = (uint8_t *)malloc(32);
    for(int i = 0; i < 32; i ++){
        in_data_buf[i] = i;
    }
    ret = enc(in_data_buf, in_len, out_data_buf, &out_len, true);
    if(ret != TEEC_SUCCESS){
        printf("encrypt error.\n");
    }
    for(int i = 0; i < 32; i ++){
        printf("%u ", out_data_buf[i]);
        in_data_buf[i] = 0;
    }
    printf("\n");
    ret = enc(out_data_buf, out_len, in_data_buf, &in_len, false);
    if(ret != TEEC_SUCCESS){
        printf("encrypt error.\n");
    }
    for(int i = 0; i < 32; i ++){
        printf("%u ", in_data_buf[i]);
    }
    printf("\n");
    free(out_data_buf);
    free(in_data_buf);
    result = TEEC_InvokeCommand(&session, CMD_PUBLIC_KEY_GET, &operation, NULL);
    
    if (result != TEEC_SUCCESS) {
        printf("inv cmd failed(0x%08x)\n", result);
        goto cleanup3;
    }
cleanup3:
    TEEC_CloseSession(&session);
cleanup2:
    TEEC_FinalizeContext(&context);
cleanup1:
    return ret;
}
