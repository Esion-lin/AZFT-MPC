/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tee_client_api.h"
#include "sst.h"

#define _DEPRESS_UNUSED_WARNING(_x) do { (_x) = (_x); } while (0)

#define ERR_LOG(_fn, _res)  \
        printf("ERROR - failed to call %s, 0x%08x\n", (_fn), (_res))

static const TEEC_UUID _g_srv_uuid = UUID_SST;

TEEC_Context     context;
TEEC_Session     session;
TEEC_Operation   operation;
TEEC_Result      result;

static const TEEC_UUID ta_sst_uuid = UUID_SST;

// Store Data
uint32_t add_file(const char *name, const uint8_t *data,
                      uint32_t data_len) {
    

	TEEC_Context     context;
	TEEC_Session     session;
	TEEC_Operation   operation;
	TEEC_Result      result = TEEC_SUCCESS;

    char *name_buf = NULL;
    uint8_t name_len;
    uint8_t *data_buf = NULL;

    // prepare name_buf
    if (NULL == name) {
        printf("ERR invalid name\n");
        result = TEEC_ERROR_BAD_PARAMETERS;
        goto cleanup;
    }

    name_len = strlen(name);
    name_buf = (char *)malloc(name_len + 1);
    if (NULL == name_buf) {
        printf("ERR failed to malloc name_buf\n");
        goto cleanup1;
    }

    memcpy(name_buf, name, name_len);
    name_buf[name_len] = '\0';

    // prepare data_buf
    if (NULL == data || 0 == data_len) {
        printf("ERR invalid data_buf/data_len\n");
        result = TEEC_ERROR_BAD_PARAMETERS;
        goto cleanup1;
    }

    data_buf = (uint8_t *)malloc(data_len);
    if (NULL == data_buf) {
        printf("ERR failed to malloc name_buf\n");
        goto cleanup1;
    }

    memcpy(data_buf, data, data_len);

	result = TEEC_InitializeContext(NULL, &context);
	if (result != TEEC_SUCCESS) {
		printf("ERR failed to initial context\n");
		goto cleanup1;
	}

    result = TEEC_OpenSession(&context, &session, &ta_sst_uuid, TEEC_LOGIN_PUBLIC,
                NULL, NULL, NULL);
    if (result != TEEC_SUCCESS) {
        printf("open ss fail(0x%08x)\n", result);
        goto cleanup2;
    }

    operation.paramTypes = TEEC_PARAM_TYPES(
                                TEEC_MEMREF_TEMP_INPUT,
                                TEEC_MEMREF_TEMP_INPUT,
                                TEEC_NONE,
                                TEEC_NONE);

    operation.params[0].tmpref.buffer = name_buf;
    operation.params[0].tmpref.size = strlen(name);
    operation.params[1].tmpref.buffer = data_buf;
    operation.params[1].tmpref.size = data_len;

    result = TEEC_InvokeCommand(&session, CMD_ADD_ITEM, &operation, NULL);
    if (result != TEEC_SUCCESS) {
        printf("inv cmd failed(0x%08x)\n", result);
        goto cleanup3;
    }

cleanup3:
    TEEC_CloseSession(&session);
cleanup2:
	TEEC_FinalizeContext(&context);
cleanup1:
    // free name_buf
    if (name_buf) {
        free(name_buf);
        name_buf = NULL;
    }
    // free data_buf
    if (data_buf) {
        free(data_buf);
        data_buf = NULL;
    }

cleanup:
	return result;
}

// Read Data
uint32_t get_file(const char *name, uint8_t *data, uint32_t *data_len) {

	TEEC_Context     context;
	TEEC_Session     session;
	TEEC_Operation   operation;
	TEEC_Result      result = TEEC_SUCCESS;

    char *name_buf = NULL;
    uint8_t name_len;
    uint8_t *data_buf = NULL;

    // prepare name_buf
    if (NULL == name) {
        printf("ERR invalid name\n");
        result = TEEC_ERROR_BAD_PARAMETERS;
        goto cleanup;
    }

    name_len = strlen(name);
    name_buf = (char *)malloc(name_len + 1);
    if (NULL == name_buf) {
        printf("ERR failed to malloc\n");
        goto cleanup;
    }

    memcpy(name_buf, name, name_len);
    name_buf[name_len] = '\0';

    // prepare data_buf
    if (NULL == data || 0 == data_len) {
        printf("ERR invalid data_buf/data_len\n");
        result = TEEC_ERROR_BAD_PARAMETERS;
        goto cleanup1;
    }

    data_buf = (uint8_t *)malloc(*data_len);
    if (NULL == data_buf) {
        printf("ERR failed to malloc name_buf\n");
        goto cleanup1;
    }

	result = TEEC_InitializeContext(NULL, &context);
	if (result != TEEC_SUCCESS) {
		printf("ERR failed to initial context\n");
		goto cleanup1;
	}

    result = TEEC_OpenSession(&context, &session, &ta_sst_uuid, TEEC_LOGIN_PUBLIC,
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

    operation.params[0].tmpref.buffer = name_buf;
    operation.params[0].tmpref.size = strlen(name_buf);
    operation.params[1].tmpref.buffer = data_buf;
    operation.params[1].tmpref.size = *data_len;

    result = TEEC_InvokeCommand(&session, CMD_READ_ITEM, &operation, NULL);
    if (result != TEEC_SUCCESS) {
        printf("inv cmd failed(0x%08x)\n", result);
        goto cleanup3;
    }

    // prepare data
    memcpy(data, data_buf, *data_len);

cleanup3:
	TEEC_CloseSession(&session);
cleanup2:
    TEEC_FinalizeContext(&context);
cleanup1:
    // free name_buf
    if (NULL != name_buf) {
        free(name_buf);
        name_buf = NULL;
    }
    // free data_buf
    if (NULL != data_buf) {
        free(data_buf);
        data_buf = NULL;
    }

cleanup:
    return result;
}

uint8_t test_data[16] = {
    0x0, 0x1, 0x2, 0x3, 
    0x4, 0x5, 0x6, 0x7, 
    0x8, 0x9, 0xA, 0xB, 
    0xC, 0xD, 0xE, 0xF,
};

int32_t main(int argc, char *argv[]) {

    uint32_t ret;
    uint32_t out_len;
    uint8_t output[100] = {0};
    uint8_t i;

    _DEPRESS_UNUSED_WARNING(argc);
    _DEPRESS_UNUSED_WARNING(argv);

	// add file to Secure Storage
    ret = add_file("test01", test_data, 16);
    if (ret) {
        printf("sst sst_add_item failed, ret 0x%08x\n", ret);
        return -1;
    }

    out_len = 16;
	// get file from Secure Storage
    ret = get_file("test01", output, &out_len);
    if (ret) {
        printf("sst sst_get_item failed, ret 0x%08x\n", ret);
		return -1;
    }

    printf("Succeed, Dump Output: \n");
    for (i=0; i<16; i++) {
        printf("0x%08x\n", output[i]);
    }

    return 0;
}
