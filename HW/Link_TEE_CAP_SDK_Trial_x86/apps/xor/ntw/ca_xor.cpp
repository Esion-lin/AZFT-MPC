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
void encryt_mac(TEEC_Operation *operation, uint8_t *label, uint32_t label_len, uint8_t *data, uint32_t data_len, uint8_t *data_out, uint32_t *data_out_len, uint8_t *mac, uint32_t *mac_len){
    (*operation).paramTypes = TEEC_PARAM_TYPES(
                                TEEC_MEMREF_TEMP_INPUT,
                                TEEC_MEMREF_TEMP_INPUT,
                                TEEC_MEMREF_TEMP_OUTPUT,
                                TEEC_MEMREF_TEMP_OUTPUT);
    (*operation).params[0].tmpref.buffer = label;
    (*operation).params[0].tmpref.size = label_len;
    (*operation).params[1].tmpref.buffer = data;
    (*operation).params[1].tmpref.size = data_len;
    (*operation).params[2].tmpref.buffer = data_out;
    (*operation).params[2].tmpref.size = *data_out_len;
    (*operation).params[3].tmpref.buffer = mac;
    (*operation).params[3].tmpref.size = *mac_len;
}
void decryt_mac(TEEC_Operation *operation, uint8_t *label, uint32_t label_len, uint8_t *data, uint32_t data_len, uint8_t *mac, uint32_t mac_len){
    (*operation).paramTypes = TEEC_PARAM_TYPES(
                                TEEC_MEMREF_TEMP_INPUT,
                                TEEC_MEMREF_TEMP_INPUT,
                                TEEC_MEMREF_TEMP_INPUT,
                                TEEC_NONE);
    (*operation).params[0].tmpref.buffer = label;
    (*operation).params[0].tmpref.size = label_len;
    (*operation).params[1].tmpref.buffer = data;
    (*operation).params[1].tmpref.size = data_len;
    (*operation).params[2].tmpref.buffer = mac;
    (*operation).params[2].tmpref.size = mac_len;
}
int main(int argc, char *argv[])
{
    /* define some varible*/
    uint32_t in_len = 32;
    uint32_t out_len = 32;
    uint8_t *in_data_buf = NULL;
    uint8_t *out_data_buf = NULL;
    uint32_t label_len = 32;
    uint32_t mac_len = 32;
    uint8_t *label_buf = NULL;
    uint8_t *mac_buf = NULL;
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
    printf("generate keys...\n");
    init_hw(&operation);
    ret = TEEC_InvokeCommand(&session, CMD_INIT, &operation, NULL);
    if(ret != TEEC_SUCCESS){
        printf("generate key error. inv cmd failed(0x%08x)\n", ret);
        goto cleanup3;
    }
    //test public exchange
    printf("key exchange...\n");
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
    out_data_buf = NULL;
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
    out_data_buf = NULL;
    //test symmetric encryption
    // printf("symmetric encryption...\n");
    // out_len = (uint32_t) (SYM_KEY_SIZE/8 + ASYM_KEY_SIZE/8);
    // out_data_buf = (uint8_t *)malloc(out_len);
    
    // in_len = (uint32_t)(SYM_KEY_SIZE/8);
    // in_data_buf = (uint8_t *)malloc(in_len);
    
    // for(int i = 0; i < in_len; i ++){
    //     in_data_buf[i] = i;
    // }
    // enc(&operation, in_data_buf, in_len, out_data_buf, &out_len);
    // ret = TEEC_InvokeCommand(&session, CMD_ENCRYPT, &operation, NULL);
    // if(ret != TEEC_SUCCESS){
    //     printf("encrypt error. inv cmd failed(0x%08x)\n", ret);
    //     goto cleanup3;
    // }
    // printf("\n");
    // enc(&operation, out_data_buf, out_len, in_data_buf, &in_len);
    // ret = TEEC_InvokeCommand(&session, CMD_DECRYPT, &operation, NULL);
    // printf("decrypt successful outside\n");
    // if(ret != TEEC_SUCCESS){
    //     printf("decrypt error. inv cmd failed(0x%08x)\n", ret);
    //     goto cleanup3;
    // }
    // printf(" plain text len: %u\n plain:\n", in_len);
    // for(int i = 0; i < in_len; i ++){
    //     printf("%u ", in_data_buf[i]);
    // }
    // printf("\n");
    // free(out_data_buf);
    // free(in_data_buf);
    // out_data_buf = NULL;
    // in_data_buf = NULL;
    
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
    out_data_buf = NULL;


    /*test AE*/
    for(int i = 0; i < 10; i++){
    label_len = 32;
    label_buf = (uint8_t *)malloc(label_len);
    for(int i = 0 ; i < label_len; i++){
        label_buf[i] = i;
    }
    out_len = (uint32_t) (SYM_KEY_SIZE/8 + ASYM_KEY_SIZE/8);
    out_data_buf = (uint8_t *)malloc(out_len);
    
    in_len = (uint32_t)(SYM_KEY_SIZE/8);
    in_data_buf = (uint8_t *)malloc(in_len);
    for(int i = 0; i < in_len; i ++){
        in_data_buf[i] = i;
    }
    mac_len = (uint32_t) (SYM_KEY_SIZE/8 + ASYM_KEY_SIZE/8);
    mac_buf = (uint8_t *)malloc(mac_len);

    encryt_mac(&operation, label_buf, label_len, in_data_buf, in_len, out_data_buf, &out_len, mac_buf, &mac_len);
    ret = TEEC_InvokeCommand(&session, CMD_ENCRYPT_MAC, &operation, NULL);
    if(ret != TEEC_SUCCESS){
        printf("authentication encrypt error. inv cmd failed(0x%08x)\n", ret);
        goto cleanup3;
    }
    decryt_mac(&operation, label_buf, label_len, out_data_buf, out_len, mac_buf, mac_len);
    ret = TEEC_InvokeCommand(&session, CMD_DECRYPT_MAC, &operation, NULL);
    if(ret != TEEC_SUCCESS){
        printf("authentication decrypt error. inv cmd failed(0x%08x)\n", ret);
        goto cleanup3;
    }
    free(label_buf);
    free(mac_buf);
    free(out_data_buf);
    free(in_data_buf);
    label_buf = NULL;
    mac_buf = NULL;
    out_data_buf = NULL;
    in_data_buf = NULL;
}
cleanup3:
    TEEC_CloseSession(&session);
cleanup2:
    TEEC_FinalizeContext(&context);
cleanup1:
    return ret;
}
