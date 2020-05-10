/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
extern "C" {
#include "tee_client_api.h"
}

#include "xor.h"

#define ERR_LOG(_fn, _res)  \
        printf("ERROR - failed to call %s, 0x%08x\n", (_fn), (_res))

#define _DEPRESS_UNUSED_WARNING(_x) do { (_x) = (_x); } while (0)

static const TEEC_UUID _g_uuid_xor = UUID_XOR;
void enc(TEEC_Operation *operation, uint8_t *data, uint32_t data_len, uint8_t *out_data, uint32_t *out_data_len){
    (*operation).paramTypes = TEEC_PARAM_TYPES(
                                TEEC_MEMREF_TEMP_INPUT,
                                TEEC_MEMREF_TEMP_OUTPUT,
                                TEEC_NONE,
                                TEEC_NONE);

    (*operation).params[0].tmpref.buffer = data;
    (*operation).params[0].tmpref.size = data_len;
    (*operation).params[1].tmpref.buffer = out_data;
    (*operation).params[1].tmpref.size = *out_data_len;
}
void get_public_key(TEEC_Operation *operation, uint8_t *out_data, uint32_t *out_data_len, uint32_t type){
    (*operation).paramTypes = TEEC_PARAM_TYPES(
                                TEEC_VALUE_INPUT,
                                TEEC_MEMREF_TEMP_OUTPUT,
                                TEEC_NONE,
                                TEEC_NONE);
    (*operation).params[0].value.a = type;
    (*operation).params[1].tmpref.buffer = out_data;
    (*operation).params[1].tmpref.size = *out_data_len;
}
void set_public_key(TEEC_Operation *operation, uint8_t *data, uint32_t data_len, uint32_t type){
    (*operation).paramTypes = TEEC_PARAM_TYPES(
                                TEEC_VALUE_INPUT,
                                TEEC_MEMREF_TEMP_INPUT,
                                TEEC_NONE,
                                TEEC_NONE);
    (*operation).params[0].value.a = type;
    (*operation).params[1].tmpref.buffer = data;
    (*operation).params[1].tmpref.size = data_len;
}
void get_sign(TEEC_Operation *operation, uint8_t *out_data, uint32_t *out_data_len){
    (*operation).paramTypes = TEEC_PARAM_TYPES(
                                TEEC_MEMREF_TEMP_OUTPUT,
                                TEEC_NONE,
                                TEEC_NONE,
                                TEEC_NONE);
    (*operation).params[0].tmpref.buffer = out_data;
    (*operation).params[0].tmpref.size = *out_data_len;
}
void check_sign(TEEC_Operation *operation, uint8_t *data, uint32_t data_len){
    (*operation).paramTypes = TEEC_PARAM_TYPES(
                                TEEC_MEMREF_TEMP_INPUT,
                                TEEC_VALUE_OUTPUT,
                                TEEC_NONE,
                                TEEC_NONE);
    (*operation).params[0].tmpref.buffer = data;
    (*operation).params[0].tmpref.size = data_len;
}
void init_hw(TEEC_Operation *operation){
    (*operation).paramTypes = TEEC_PARAM_TYPES(
                                TEEC_NONE,
                                TEEC_NONE,
                                TEEC_NONE,
                                TEEC_NONE);
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
    /* generate the whole keys*/
    init_hw(&operation);
    ret = TEEC_InvokeCommand(&session, CMD_INIT, &operation, NULL);
    if(ret != TEEC_SUCCESS){
        printf("generate key error. inv cmd failed(0x%08x)\n", ret);
        goto cleanup3;
    }
    //test public exchange
    
    out_len = ASYM_KEY_SIZE/4;
    out_data_buf = (uint8_t *)malloc(out_len);
    
    get_public_key(&operation, out_data_buf, &out_len, PUBLIC_TYPE_CIPHER);
    ret = TEEC_InvokeCommand(&session, CMD_KEY_GET, &operation, NULL);
    if(ret != TEEC_SUCCESS){
        printf("get_public_key. PUBLIC_TYPE_CIPHER inv cmd failed(0x%08x)\n", ret);
        goto cleanup3;
    }
    set_public_key(&operation, out_data_buf, out_len, PUBLIC_TYPE_CIPHER);
    ret = TEEC_InvokeCommand(&session, CMD_KEY_STORE, &operation, NULL);
    if(ret != TEEC_SUCCESS){
        printf("set_public_key. PUBLIC_TYPE_CIPHER inv cmd failed(0x%08x)\n", ret);
        goto cleanup3;
    }
    free(out_data_buf);
    out_len = ASYM_KEY_FOR_SIGN_SIZE/4;
    out_data_buf = (uint8_t *)malloc(out_len);
    get_public_key(&operation, out_data_buf, &out_len, PUBLIC_TYPE_SIGN);
    ret = TEEC_InvokeCommand(&session, CMD_KEY_GET, &operation, NULL);
    if(ret != TEEC_SUCCESS){
        printf("get_public_key. PUBLIC_TYPE_SIGN inv cmd failed(0x%08x)\n", ret);
        goto cleanup3;
    }
    set_public_key(&operation, out_data_buf, out_len, PUBLIC_TYPE_SIGN);
    ret = TEEC_InvokeCommand(&session, CMD_KEY_STORE, &operation, NULL);
    if(ret != TEEC_SUCCESS){
        printf("set_public_key. PUBLIC_TYPE_SIGN inv cmd failed(0x%08x)\n", ret);
        goto cleanup3;
    }
    free(out_data_buf);
    //test symmetric encryption
    out_len = (uint32_t) (SYM_KEY_SIZE/8 + ASYM_KEY_SIZE/8);
    out_data_buf = (uint8_t *)malloc(out_len);
    
    in_len = (uint32_t)(SYM_KEY_SIZE/8);
    in_data_buf = (uint8_t *)malloc(in_len);
    
    for(int i = 0; i < in_len; i ++){
        in_data_buf[i] = i;
    }
    enc(&operation, in_data_buf, in_len, out_data_buf, &out_len);
    ret = TEEC_InvokeCommand(&session, CMD_ENCRYPT, &operation, NULL);
    if(ret != TEEC_SUCCESS){
        printf("encrypt error. inv cmd failed(0x%08x)\n", ret);
        goto cleanup3;
    }
    printf(" cipher text len: %u\n cipher:\n", out_len);
    for(int i = 0; i < out_len; i ++){
        printf("%u ", out_data_buf[i]);
        in_data_buf[i] = 0;
    }
    printf("\n");
    enc(&operation, out_data_buf, out_len, in_data_buf, &in_len);
    ret = TEEC_InvokeCommand(&session, CMD_DECRYPT, &operation, NULL);
    if(ret != TEEC_SUCCESS){
        printf("decrypt error. inv cmd failed(0x%08x)\n", ret);
        goto cleanup3;
    }
    printf(" plain text len: %u\n plain:\n", in_len);
    for(int i = 0; i < in_len; i ++){
        printf("%u ", in_data_buf[i]);
    }
    printf("\n");
    free(out_data_buf);
    free(in_data_buf);
    
    /*test public key print*/
    /*key for sign*/


    /*key for hybrid*/
    /*get_public_key(&operation, out_data_buf, &out_len, PUBLIC_TYPE_CIPHER);
    ret = TEEC_InvokeCommand(&session, CMD_KEY_GET, &operation, NULL);
    if(ret != TEEC_SUCCESS){
        printf("print public key error. inv cmd failed(0x%08x)\n", ret);
        goto cleanup3;
    }
    for(int i = 0; i < out_len; i ++){
        printf("%u ", out_data_buf[i]);
    }*/
    //test sign key
    out_len = (uint32_t)(ASYM_KEY_FOR_SIGN_SIZE/8);
    out_data_buf = (uint8_t *)malloc(out_len);
    get_sign(&operation, out_data_buf, &out_len);
    ret = TEEC_InvokeCommand(&session, CMD_KEY_SIGN, &operation, NULL);
    if(ret != TEEC_SUCCESS){
        printf("get_sign. inv cmd failed(0x%08x)\n", ret);
        goto cleanup3;
    }
    check_sign(&operation, out_data_buf, out_len);
    ret = TEEC_InvokeCommand(&session, CMD_KEY_VERIFY, &operation, NULL);
    if(ret != TEEC_SUCCESS){
        printf("check_sign. inv cmd failed(0x%08x)\n", ret);
        goto cleanup3;
    }
    printf("check sign ans: %u\n",operation.params[1].value.a);
    free(out_data_buf);
    /*test mac*/
    out_data_buf = (uint8_t *)malloc(128);
    out_len = 128;
    in_data_buf = (uint8_t *)malloc(32);
    in_len = 32;
    for(int i = 0; i < out_len; i ++){
        out_data_buf[i] = i;
    }
    enc(&operation, out_data_buf, out_len, in_data_buf, &in_len);
    ret = TEEC_InvokeCommand(&session, CMD_TEST_MAC, &operation, NULL);
    if(ret != TEEC_SUCCESS){
        printf("MAC error. inv cmd failed(0x%08x)\n", ret);
        goto cleanup3;
    }
    printf("mac:\n");
    for(int i = 0; i < 32; i ++){
        printf("%u ", in_data_buf[i]);
    }
    printf("\n");
    /*result = TEEC_InvokeCommand(&session, CMD_PUBLIC_KEY_GET, &operation, NULL);
    if (result != TEEC_SUCCESS) {
        printf("inv cmd failed(0x%08x)\n", result);
        goto cleanup3;
    }*/
cleanup3:
    TEEC_CloseSession(&session);
cleanup2:
    TEEC_FinalizeContext(&context);
cleanup1:
    return ret;
}
