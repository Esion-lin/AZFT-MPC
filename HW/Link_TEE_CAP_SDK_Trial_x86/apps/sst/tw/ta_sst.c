/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "tee_internal_api.h"
#include "tee_internal_api_ext.h"
#include "tee_srv.h"

#define CMD_ADD_ITEM       (0x1)
#define CMD_READ_ITEM      (0x2)
#define CMD_DEL_ITEM       (0x3)

#define UUID_SST    { 0x11223344, 0xacbd, 0xcedf,   \
                    { 0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x79 } }

#define TA_DBG(format, a...)        \
    do {                            \
        TEE_Print(format, ##a);     \
    } while(0)

static TEE_UUID sst_uuid = UUID_SST;


static TEE_Result _TA_CreateEntryPoint(void)
{
    return TEE_SUCCESS;
}

static void _TA_DestroyEntryPoint(void)
{
    return;
}

static TEE_Result _TA_OpenSessionEntryPoint(
        uint32_t paramTypes, TEE_Param params[4], void **sessionContext)
{
    return TEE_SUCCESS;
}

static void _TA_CloseSessionEntryPoint(void *sessionContext)
{
    return;
}


static TEE_Result sst_write_data(char* name, uint8_t name_len, 
								 uint8_t* data, uint32_t data_len) {

	TEE_Result ret = TEE_SUCCESS;
	TEE_ObjectHandle ptObject = TEE_HANDLE_NULL;
    TA_DBG("sst_write_data: call TEE_CreatePersistentObject name: %s len: %d\n", name, name_len);
	ret = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE, name, name_len,
									 TEE_DATA_FLAG_ACCESS_READ | TEE_DATA_FLAG_ACCESS_WRITE |
									 TEE_DATA_FLAG_ACCESS_WRITE_META | TEE_DATA_FLAG_OVERWRITE,
									 NULL, data, data_len, &ptObject);
	if (TEE_SUCCESS != ret) {
		TA_DBG("sst_write_data: TEE_CreatePersistentObject err: 0x%x\n", ret);
		goto cleanup1;
	}

	ret = TEE_WriteObjectData(ptObject, data, data_len);
	if (TEE_SUCCESS != ret) {
		TA_DBG("sst_write_data: TEE_WriteObjectData err: 0x%x\n",ret);
	}

	TEE_CloseObject(ptObject);

cleanup1:
	return ret;
}

static TEE_Result sst_read_data(char* name, uint8_t name_len,
								uint8_t* data, uint32_t* data_len) {
	TEE_Result ret = 0;
	TEE_ObjectHandle ptObject = TEE_HANDLE_NULL;
	uint32_t read_sz = 0;

    TA_DBG("sst_read_data: call TEE_OpenPersistentObject name: %s len: %d\n", name, name_len);

	ret = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE, name, name_len,
								   TEE_DATA_FLAG_ACCESS_READ |
								   TEE_DATA_FLAG_ACCESS_WRITE |
								   TEE_DATA_FLAG_ACCESS_WRITE_META,
								   &ptObject);
	if (TEE_SUCCESS != ret) {
		TA_DBG("sst_read_data: TEE_OpenPersistentObject err: 0x%x\n", ret);
		goto cleanup1;
	}

	ret = TEE_ReadObjectData(ptObject, data, *data_len, &read_sz);
	if (TEE_SUCCESS != ret) {
		TA_DBG("sst_read_data: TEE_ReadObjectData err: 0x%x\n", ret);
        goto cleanup;
	}

	*data_len = read_sz;

cleanup:
	TEE_CloseObject(ptObject);

cleanup1:
	return ret;

}

static TEE_Result _TA_InvokeCommandEntryPoint(
        void *sessionContext, uint32_t commandID,
        uint32_t param_types, TEE_Param params[4])
{
    TEE_Result res;

    if (CMD_ADD_ITEM == commandID) {
    	if (TEE_PARAM_TYPES(
    				TEE_PARAM_TYPE_MEMREF_INPUT,
    				TEE_PARAM_TYPE_MEMREF_INPUT,
    				TEE_PARAM_TYPE_NONE,
    				TEE_PARAM_TYPE_NONE) != param_types) {
    		return TEE_ERROR_BAD_PARAMETERS;
    	}

        res = sst_write_data(
        			params[0].memref.buffer, 
        			params[0].memref.size,
        			params[1].memref.buffer,
        			params[1].memref.size);

    } else if (CMD_READ_ITEM == commandID) {
    	if (TEE_PARAM_TYPES(
    				TEE_PARAM_TYPE_MEMREF_INPUT,
    				TEE_PARAM_TYPE_MEMREF_OUTPUT,
    				TEE_PARAM_TYPE_NONE,
    				TEE_PARAM_TYPE_NONE) != param_types) {
    		return TEE_ERROR_BAD_PARAMETERS;
    	}

    	res = sst_read_data(
    				params[0].memref.buffer,
    				params[0].memref.size,
    				params[1].memref.buffer,
    				&(params[1].memref.size));
    } else {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    return res;
}

tee_srv_head_t sst_srv_head = {
    .uuid = &sst_uuid,
    .type = SRV_TYPE_1_I_M_S_NKA,
    .stack_size = 0x4000,
    .TA_CreateEntryPoint = _TA_CreateEntryPoint,
    .TA_DestroyEntryPoint = _TA_DestroyEntryPoint,
    .TA_OpenSessionEntryPoint = _TA_OpenSessionEntryPoint,
    .TA_CloseSessionEntryPoint = _TA_CloseSessionEntryPoint,
    .TA_InvokeCommandEntryPoint = _TA_InvokeCommandEntryPoint,
};
